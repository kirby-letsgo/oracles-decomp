#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// INTERAC_CHILD (interactionCode35): the child raised by Link in Ages.
//   var03: index of script and code to run (personality + growth stage)
//   var37: animation base (from subid), var39: solidity mode, var3a: animation offset
//   var3c: current position-list index, var3d: entries in list minus one
//   var3e/3f: pointer to position list

// One pointer per var03 value (bank $0c scripts).
#define childScriptTable_bank08 0x6841
// childDetermineAnimationBase@animations: one byte per subid.
#define childDetermineAnimationBase_animations_bank08 0x66c0
#define childHyperactiveMovementAngles_bank08 0x66f8
// childLoadPositionListPointer@positionTable: word pointer, byte count-1, byte unused.
#define childLoadPositionListPointer_positionTable_bank08 0x67a8

void interac65_state1_hook(GB *gb);
void childUpdateAnimationAndSolidity_hook(GB *gb);
void childUpdateSolidityAndVisibility_hook(GB *gb);
void childDetermineAnimationBase_hook(GB *gb);
void childUpdateHyperactiveMovement_hook(GB *gb);
void childFlipAnimation_hook(GB *gb);
void childUpdateUnknownMovement_hook(GB *gb);
void childUpdateShyMovement_hook(GB *gb);
void childUpdateAngleAndApplySpeed_hook(GB *gb);
void childCheckReachedDestination_hook(GB *gb);
void childCheckAnimationDirectionChanged_hook(GB *gb);
void childIncPositionIndex_hook(GB *gb);
void childLoadPositionListPointer_hook(GB *gb);
void childUpdateCuriousMovement_hook(GB *gb);

static uint16_t child_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void child_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void child_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @setAnimation: var3a = A; set animation var37 + var3a, then update solidity.
static void child_setAnimation(GB *gb, uint16_t sp0_) {
  CYC(0x6588, 0x6589); H = D;
  CYC(0x6589, 0x658b); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x658b, 0x658c); mem_wr(gb, HL, A);
  CYC(0x658c, 0x658e); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x658e, 0x658f); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x658f, interactionSetAnimation_hook, 0x262e, 0x6592);
  CYC(0x6592, 0x6595); childUpdateSolidityAndVisibility_hook(gb);
}

// @hyperactiveStage4Or5: also `call`ed from @val16 (synthetic local
// interactionCode35__hyperactiveStage4Or5). Falls through into @setAnimation.
static void child_hyperactiveStage4Or5(GB *gb, uint16_t sp0_) {
  CYC(0x6579, 0x657a); H = D;
  CYC(0x657a, 0x657c); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x657c, 0x657e); mem_wr(gb, HL, 0x01);
  CYC(0x657e, 0x6580); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6580, 0x6582); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x6582, 0x6584); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6584, 0x6586); mem_wr(gb, HL, 0x18);
  CYC(0x6586, 0x6588); A = 0x00;
  child_setAnimation(gb, sp0_);
}

// @state0: init graphics, pick the script from var03, then per-var03 setup.
static void child_state0(GB *gb, uint16_t sp0_) {
  CALL_C(0x6517, childDetermineAnimationBase_hook, 0x66b4, 0x651a);
  CALL_C(0x651a, interactionInitGraphics_hook, 0x15fb, 0x651d);
  CALL_C(0x651d, interactionIncState_hook, 0x23e0, 0x6520);
  CYC(0x6520, 0x6522); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6522, 0x6523); A = mem_rd(gb, DE);
  CYC(0x6523, 0x6526); SET_HL(childScriptTable_bank08);
  CYC(0x6526, 0x6527); child_addDoubleIndex(gb, 0x6527);
  CYC(0x6527, 0x6528); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6528, 0x6529); H = mem_rd(gb, HL);
  CYC(0x6529, 0x652a); L = A;
  CALL_C(0x652a, interactionSetScript_hook, 0x2544, 0x652d);
  CYC(0x652d, 0x652f); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x652f, 0x6530); A = mem_rd(gb, DE);
  CYC(0x6530, 0x6531); push_effect(gb, 0x6531);
  switch (child_jumpTable(gb)) {
    case 0x656b: goto initAnimation;
    case 0x6574: goto hyperactiveStage6;
    case 0x6579: child_hyperactiveStage4Or5(gb, sp0_); return;
    case 0x6595: goto val16;
    case 0x659e: goto shyStage4Or5;
    case 0x65a5: goto shyStage6;
    case 0x65b7: goto curious;
    case 0x65c0: goto slacker;
    case 0x65c4: goto warrior;
    case 0x65cb: goto script0f;
    case 0x65dd: goto arborist;
    case 0x65e1: goto singer;
    default: HANDOFF(HL);
  }

initAnimation:
  CYC(0x656b, 0x656d); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x656d, 0x656e); A = mem_rd(gb, DE);
  CALL_C(0x656e, interactionSetAnimation_hook, 0x262e, 0x6571);
  CYC(0x6571, 0x6574); childUpdateSolidityAndVisibility_hook(gb);
  return;

hyperactiveStage6:
  CYC(0x6574, 0x6576); A = 0x02;
  CALL_C(0x6576, childLoadPositionListPointer_hook, 0x6795, 0x6579);
  child_hyperactiveStage4Or5(gb, sp0_);
  return;

val16:
  CYC(0x6595, 0x6598); push_effect(gb, 0x6598); child_hyperactiveStage4Or5(gb, sp0_);
  CYC(0x6598, 0x6599); H = D;
  CYC(0x6599, 0x659b); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x659b, 0x659d); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x659d, 0x659e); ret_effect(gb);
  return;

shyStage4Or5:
  CYC(0x659e, 0x65a0); A = 0x00;
  CALL_C(0x65a0, childLoadPositionListPointer_hook, 0x6795, 0x65a3);
  CYC(0x65a3, 0x65a5); goto shyCommon;

shyStage6:
  CYC(0x65a5, 0x65a7); A = 0x01;
  CALL_C(0x65a7, childLoadPositionListPointer_hook, 0x6795, 0x65aa);

shyCommon:
  CYC(0x65aa, 0x65ab); H = D;
  CYC(0x65ab, 0x65ad); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x65ad, 0x65af); mem_wr(gb, HL, 0x01);
  CYC(0x65af, 0x65b1); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x65b1, 0x65b3); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x65b3, 0x65b5); A = 0x00;
  CYC(0x65b5, 0x65b7); child_setAnimation(gb, sp0_);
  return;

curious:
  CYC(0x65b7, 0x65b8); H = D;
  CYC(0x65b8, 0x65ba); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x65ba, 0x65bc); mem_wr(gb, HL, 0x02);
  CYC(0x65bc, 0x65be); A = 0x00;
  CYC(0x65be, 0x65c0); child_setAnimation(gb, sp0_);
  return;

slacker:
  CYC(0x65c0, 0x65c2); A = 0x00;
  CYC(0x65c2, 0x65c4); child_setAnimation(gb, sp0_);
  return;

warrior:
  CYC(0x65c4, 0x65c6); A = 0x03;
  CALL_C(0x65c6, childLoadPositionListPointer_hook, 0x6795, 0x65c9);
  CYC(0x65c9, 0x65cb); goto warriorCommon;

script0f:
  CYC(0x65cb, 0x65cd); A = 0x04;
  CALL_C(0x65cd, childLoadPositionListPointer_hook, 0x6795, 0x65d0);

warriorCommon:
  CYC(0x65d0, 0x65d1); H = D;
  CYC(0x65d1, 0x65d3); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x65d3, 0x65d5); mem_wr(gb, HL, 0x01);
  CYC(0x65d5, 0x65d7); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x65d7, 0x65d9); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x65d9, 0x65db); A = 0x00;
  CYC(0x65db, 0x65dd); child_setAnimation(gb, sp0_);
  return;

arborist:
  CYC(0x65dd, 0x65df); A = 0x03;
  CYC(0x65df, 0x65e1); child_setAnimation(gb, sp0_);
  return;

singer:
  CYC(0x65e1, 0x65e3); A = 0x00;
  CYC(0x65e3, 0x65e5); child_setAnimation(gb, sp0_);
}

void interactionCode35_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x650f, 0x6511); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6511, 0x6512); A = mem_rd(gb, DE);
  CYC(0x6512, 0x6513); push_effect(gb, 0x6513);
  switch (child_jumpTable(gb)) {
    case 0x6517: child_state0(gb, sp0_); return;
    case 0x65e5: interac65_state1_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// State 1: per-var03 movement, then run the script and animate.
void interac65_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x65e5, 0x65e7); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x65e7, 0x65e8); A = mem_rd(gb, DE);
  CYC(0x65e8, 0x65e9); push_effect(gb, 0x65e9);
  switch (child_jumpTable(gb)) {
    case 0x662f: goto updateAnimationAndSolidity;
    case 0x6623: goto hyperactiveMovement;
    case 0x6638: goto shyMovement;
    case 0x6657: goto curiousMovement;
    case 0x6643: goto usePositionList;
    case 0x6664: goto slackerMovement;
    case 0x662c: goto arboristMovement;
    case 0x667d: goto singerMovement;
    case 0x6632: goto val16;
    case 0x6661: goto val1b;
    default: HANDOFF(HL);
  }

hyperactiveMovement:
  CYC(0x6623, 0x6625); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6625, 0x6626); A = mem_rd(gb, DE);
  CYC(0x6626, 0x6627); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6627, 0x6629); goto arboristMovement;
  }
  CYC(0x6627, 0x6629);
  CALL_C(0x6629, childUpdateHyperactiveMovement_hook, 0x66c8, 0x662c);

arboristMovement:
  CALL_C(0x662c, interactionRunScript_hook, 0x2552, 0x662f);

updateAnimationAndSolidity:
  CYC(0x662f, 0x6632); childUpdateAnimationAndSolidity_hook(gb);
  return;

val16:
  CALL_C(0x6632, childUpdateUnknownMovement_hook, 0x66fc, 0x6635);
  CYC(0x6635, 0x6638); childUpdateAnimationAndSolidity_hook(gb);
  return;

shyMovement:
  CYC(0x6638, 0x663a); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x663a, 0x663b); A = mem_rd(gb, DE);
  CYC(0x663b, 0x663c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x663c, 0x663e); goto shyDone;
  }
  CYC(0x663c, 0x663e);
  CALL_C(0x663e, childUpdateShyMovement_hook, 0x6710, 0x6641);
shyDone:
  CYC(0x6641, 0x6643); goto runScriptAndUpdateAnimation;

usePositionList:
  CYC(0x6643, 0x6645); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6645, 0x6646); A = mem_rd(gb, DE);
  CYC(0x6646, 0x6647); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6647, 0x6649); goto positionListDone;
  }
  CYC(0x6647, 0x6649);
  CALL_C(0x6649, childUpdateAngleAndApplySpeed_hook, 0x6730, 0x664c);
  CALL_C(0x664c, childCheckAnimationDirectionChanged_hook, 0x6771, 0x664f);
  CALL_C(0x664f, childCheckReachedDestination_hook, 0x674c, 0x6652);
  if (F & FC) {
    CALL_C_CC(0x6652, childIncPositionIndex_hook, 0x6789, 0x6655);
  } else {
    CYC(0x6652, 0x6655);
  }
positionListDone:
  CYC(0x6655, 0x6657); goto runScriptAndUpdateAnimation;

curiousMovement:
  CALL_C(0x6657, childUpdateCuriousMovement_hook, 0x67f8, 0x665a);
  CYC(0x665a, 0x665c); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x665c, 0x665d); A = mem_rd(gb, DE);
  CYC(0x665d, 0x665e); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x665e, interactionRunScript_hook, 0x2552, 0x6661);
  } else {
    CYC(0x665e, 0x6661);
  }

val1b:
  CYC(0x6661, 0x6664); childUpdateAnimationAndSolidity_hook(gb);
  return;

slackerMovement:
  CYC(0x6664, 0x6667); A = mem_rd(gb, wFrameCounter);
  CYC(0x6667, 0x6669); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(0x6669, 0x666b); goto slackerDone;
  }
  CYC(0x6669, 0x666b);
  CYC(0x666b, 0x666d); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x666d, 0x666e); A = mem_rd(gb, DE);
  CYC(0x666e, 0x6670); alu_and(gb, 0x01);
  CYC(0x6670, 0x6672); C = 0x08;
  if (!(F & FZ)) {
    CYCT(0x6672, 0x6674);
  } else {
    CYC(0x6672, 0x6674);
    CYC(0x6674, 0x6676); C = 0xfc;
  }
  CYC(0x6676, 0x6678); B = 0xf4;
  CALL_C(0x6678, objectCreateFloatingMusicNote_hook, 0x27fe, 0x667b);
slackerDone:
  CYC(0x667b, 0x667d); goto runScriptAndUpdateAnimation;

singerMovement:
  CYC(0x667d, 0x6680); A = mem_rd(gb, wFrameCounter);
  CYC(0x6680, 0x6682); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(0x6682, 0x6684); goto runScriptAndUpdateAnimation;
  }
  CYC(0x6682, 0x6684);
  CYC(0x6684, 0x6686); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6686, 0x6687); A = mem_rd(gb, DE);
  CYC(0x6687, 0x6688); alu_or(gb, A);
  CYC(0x6688, 0x668a); C = 0xfc;
  if (F & FZ) {
    CYCT(0x668a, 0x668c);
  } else {
    CYC(0x668a, 0x668c);
    CYC(0x668c, 0x668e); C = 0x00;
  }
  CYC(0x668e, 0x6690); B = 0xfc;
  CALL_C(0x6690, objectCreateFloatingMusicNote_hook, 0x27fe, 0x6693);

runScriptAndUpdateAnimation:
  CALL_C(0x6693, interactionRunScript_hook, 0x2552, 0x6696);
  CYC(0x6696, 0x6699); childUpdateAnimationAndSolidity_hook(gb);
}

// Falls through into childUpdateSolidityAndVisibility.
void childUpdateAnimationAndSolidity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6699, interactionAnimate_hook, 0x261b, 0x669c);
  childUpdateSolidityAndVisibility_hook(gb);
}

// var39: 0 = solid, 1 = "light" solidity (pushes Link away), 2 = no solidity.
void childUpdateSolidityAndVisibility_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x669c, 0x669e); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x669e, 0x669f); A = mem_rd(gb, DE);
  CYC(0x669f, 0x66a1); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x66a1, 0x66a3); goto lightSolidity;
  }
  CYC(0x66a1, 0x66a3);
  CYC(0x66a3, 0x66a5); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x66a5, 0x66a8); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return;
  }
  CYC(0x66a5, 0x66a8);
  CALL_C(0x66a8, objectPreventLinkFromPassing_hook, 0x2680, 0x66ab);
  CYC(0x66ab, 0x66ae); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
  return;

lightSolidity:
  CALL_C(0x66ae, objectPushLinkAwayOnCollision_hook, 0x230e, 0x66b1);
  CYC(0x66b1, 0x66b4); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// Writes the "base" animation index to var37 based on subid (personality type).
void childDetermineAnimationBase_hook(GB *gb) {
  CYC(0x66b4, 0x66b6); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x66b6, 0x66b7); A = mem_rd(gb, DE);
  CYC(0x66b7, 0x66ba); SET_HL(childDetermineAnimationBase_animations_bank08);
  CYC(0x66ba, 0x66bb); push_effect(gb, 0x66bb); child_addAToHl(gb);
  CYC(0x66bb, 0x66bc); A = mem_rd(gb, HL);
  CYC(0x66bc, 0x66be); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x66be, 0x66bf); mem_wr(gb, DE, A);
  CYC(0x66bf, 0x66c0); ret_effect(gb);
}

// Bounces between x=$29 and x=$68, cycling through 4 angles. Falls through into
// childFlipAnimation.
void childUpdateHyperactiveMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x66c8, objectApplySpeed_hook, 0x201d, 0x66cb);
  CYC(0x66cb, 0x66cc); H = D;
  CYC(0x66cc, 0x66ce); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x66ce, 0x66cf); A = mem_rd(gb, HL);
  CYC(0x66cf, 0x66d1); alu_sub(gb, 0x29);
  CYC(0x66d1, 0x66d3); alu_cp(gb, 0x40);
  if (F & FC) {
    CYCT(0x66d3, 0x66d4); ret_effect(gb); return;
  }
  CYC(0x66d3, 0x66d4);
  CYC(0x66d4, 0x66d6); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x66d6, 0x66d8);
  } else {
    CYC(0x66d6, 0x66d8);
    CYC(0x66d8, 0x66d9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x66d9, 0x66da); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(0x66da, 0x66db); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x66db, 0x66dd); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x66dd, 0x66de); A = mem_rd(gb, HL);
  CYC(0x66de, 0x66df); A = alu_inc8(gb, A);
  CYC(0x66df, 0x66e1); alu_and(gb, 0x03);
  CYC(0x66e1, 0x66e2); mem_wr(gb, HL, A);
  CYC(0x66e2, 0x66e5); SET_BC(childHyperactiveMovementAngles_bank08);
  CALL_C(0x66e5, addAToBc_hook, 0x006d, 0x66e8);
  CYC(0x66e8, 0x66e9); A = mem_rd(gb, BC);
  CYC(0x66e9, 0x66eb); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x66eb, 0x66ec); mem_wr(gb, HL, A);
  childFlipAnimation_hook(gb);
}

// Toggles bit 0 of var3a and sets animation var37 + var3a. Expects H = D.
void childFlipAnimation_hook(GB *gb) {
  CYC(0x66ec, 0x66ee); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x66ee, 0x66ef); A = mem_rd(gb, HL);
  CYC(0x66ef, 0x66f1); alu_xor(gb, 0x01);
  CYC(0x66f1, 0x66f2); mem_wr(gb, HL, A);
  CYC(0x66f2, 0x66f4); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x66f4, 0x66f5); alu_add(gb, mem_rd(gb, HL));
  CYC(0x66f5, 0x66f8); interactionSetAnimation_hook(gb);
}

// Bounces between x=$14 and x=$3b, flipping the angle horizontally.
void childUpdateUnknownMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x66fc, objectApplySpeed_hook, 0x201d, 0x66ff);
  CYC(0x66ff, 0x6701); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x6701, 0x6702); A = mem_rd(gb, DE);
  CYC(0x6702, 0x6704); alu_sub(gb, 0x14);
  CYC(0x6704, 0x6706); alu_cp(gb, 0x28);
  if (F & FC) {
    CYCT(0x6706, 0x6707); ret_effect(gb); return;
  }
  CYC(0x6706, 0x6707);
  CYC(0x6707, 0x6708); H = D;
  CYC(0x6708, 0x670a); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x670a, 0x670b); A = mem_rd(gb, HL);
  CYC(0x670b, 0x670d); alu_xor(gb, 0x10);
  CYC(0x670d, 0x670e); mem_wr(gb, HL, A);
  CYC(0x670e, 0x6710); childFlipAnimation_hook(gb);
}

// "Shy" personality: runs to the next position when Link approaches.
void childUpdateShyMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6710, 0x6712); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6712, 0x6713); A = mem_rd(gb, DE);
  CYC(0x6713, 0x6714); push_effect(gb, 0x6714);
  switch (child_jumpTable(gb)) {
    case 0x6718: goto substate0;
    case 0x6721: goto substate1;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x6718, 0x671a); C = 0x18;
  CALL_C(0x671a, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x671d);
  if (!(F & FC)) {
    CYCT(0x671d, 0x671e); ret_effect(gb); return;
  }
  CYC(0x671d, 0x671e);
  CALL_C(0x671e, interactionIncSubstate_hook, 0x23e5, 0x6721);

substate1:
  CALL_C(0x6721, childUpdateAngleAndApplySpeed_hook, 0x6730, 0x6724);
  CALL_C(0x6724, childCheckReachedDestination_hook, 0x674c, 0x6727);
  if (!(F & FC)) {
    CYCT(0x6727, 0x6728); ret_effect(gb); return;
  }
  CYC(0x6727, 0x6728);
  CYC(0x6728, 0x6729); H = D;
  CYC(0x6729, 0x672b); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x672b, 0x672d); mem_wr(gb, HL, 0x00);
  CYC(0x672d, 0x6730); childIncPositionIndex_hook(gb);
}

// Sets the angle toward position list entry var3c, then applies speed.
void childUpdateAngleAndApplySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6730, 0x6731); H = D;
  CYC(0x6731, 0x6733); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6733, 0x6734); A = mem_rd(gb, HL);
  CYC(0x6734, 0x6735); alu_add(gb, A);
  CYC(0x6735, 0x6736); B = A;
  CYC(0x6736, 0x6738); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6738, 0x6739); A = mem_rd(gb, DE);
  CYC(0x6739, 0x673a); L = A;
  CYC(0x673a, 0x673c); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x673c, 0x673d); A = mem_rd(gb, DE);
  CYC(0x673d, 0x673e); H = A;
  CYC(0x673e, 0x673f); A = B;
  CYC(0x673f, 0x6740); push_effect(gb, 0x6740); child_addAToHl(gb);
  CYC(0x6740, 0x6741); B = mem_rd(gb, HL);
  CYC(0x6741, 0x6742); SET_HL(HL + 1);
  CYC(0x6742, 0x6743); C = mem_rd(gb, HL);
  CALL_C(0x6743, objectGetRelativeAngle_hook, 0x1ea4, 0x6746);
  CYC(0x6746, 0x6748); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6748, 0x6749); mem_wr(gb, DE, A);
  CYC(0x6749, 0x674c); objectApplySpeed_hook(gb);
}

// @param[out] cflag Set if within 1 pixel of position list entry var3c on both axes.
void childCheckReachedDestination_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x674c, 0x674d); H = D;
  CYC(0x674d, 0x674f); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x674f, 0x6750); A = mem_rd(gb, HL);
  CYC(0x6750, 0x6751); alu_add(gb, A);
  CYC(0x6751, 0x6752); push_effect(gb, AF);
  CYC(0x6752, 0x6754); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6754, 0x6755); A = mem_rd(gb, DE);
  CYC(0x6755, 0x6756); C = A;
  CYC(0x6756, 0x6758); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6758, 0x6759); A = mem_rd(gb, DE);
  CYC(0x6759, 0x675a); B = A;
  CYC(0x675a, 0x675b); SET_AF(pop_effect(gb));
  CALL_C(0x675b, addAToBc_hook, 0x006d, 0x675e);
  CYC(0x675e, 0x6760); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6760, 0x6761); A = mem_rd(gb, BC);
  CYC(0x6761, 0x6762); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6762, 0x6764); alu_add(gb, 0x01);
  CYC(0x6764, 0x6766); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x6766, 0x6767); ret_effect(gb); return;
  }
  CYC(0x6766, 0x6767);
  CYC(0x6767, 0x6768); SET_BC(BC + 1);
  CYC(0x6768, 0x676a); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x676a, 0x676b); A = mem_rd(gb, BC);
  CYC(0x676b, 0x676c); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x676c, 0x676e); alu_add(gb, 0x01);
  CYC(0x676e, 0x6770); alu_cp(gb, 0x03);
  CYC(0x6770, 0x6771); ret_effect(gb);
}

// Updates the animation when the horizontal direction derived from the angle changes.
void childCheckAnimationDirectionChanged_hook(GB *gb) {
  CYC(0x6771, 0x6772); H = D;
  CYC(0x6772, 0x6774); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6774, 0x6775); A = mem_rd(gb, HL);
  CYC(0x6775, 0x6777); A = alu_swap(gb, A);
  CYC(0x6777, 0x6779); alu_and(gb, 0x01);
  CYC(0x6779, 0x677b); alu_xor(gb, 0x01);
  CYC(0x677b, 0x677d); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x677d, 0x677e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x677e, 0x677f); ret_effect(gb); return;
  }
  CYC(0x677e, 0x677f);
  CYC(0x677f, 0x6780); mem_wr(gb, HL, A);
  CYC(0x6780, 0x6782); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6782, 0x6783); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6783, 0x6785); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x6785, 0x6786); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6786, 0x6789); interactionSetAnimation_hook(gb);
}

// var3c++, wrapping to 0 once it passes var3d.
void childIncPositionIndex_hook(GB *gb) {
  CYC(0x6789, 0x678a); H = D;
  CYC(0x678a, 0x678c); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x678c, 0x678d); A = mem_rd(gb, HL);
  CYC(0x678d, 0x678f); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x678f, 0x6790); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6790, 0x6791); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x6791, 0x6792); ret_effect(gb); return;
  }
  CYC(0x6791, 0x6792);
  CYC(0x6792, 0x6794); mem_wr(gb, HL, 0x00);
  CYC(0x6794, 0x6795); ret_effect(gb);
}

// Loads position list pointer A into var3e/var3f and its entry count-1 into var3d.
void childLoadPositionListPointer_hook(GB *gb) {
  CYC(0x6795, 0x6796); alu_add(gb, A);
  CYC(0x6796, 0x6797); alu_add(gb, A);
  CYC(0x6797, 0x679a); SET_HL(childLoadPositionListPointer_positionTable_bank08);
  CYC(0x679a, 0x679b); push_effect(gb, 0x679b); child_addAToHl(gb);
  CYC(0x679b, 0x679d); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x679d, 0x679e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x679e, 0x679f); mem_wr(gb, DE, A);
  CYC(0x679f, 0x67a1); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x67a1, 0x67a2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x67a2, 0x67a3); mem_wr(gb, DE, A);
  CYC(0x67a3, 0x67a5); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x67a5, 0x67a6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x67a6, 0x67a7); mem_wr(gb, DE, A);
  CYC(0x67a7, 0x67a8); ret_effect(gb);
}

// "Curious" personality: hops back and forth, pausing $78 frames between hops.
void childUpdateCuriousMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x67f8, 0x67fa); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x67fa, 0x67fb); A = mem_rd(gb, DE);
  CYC(0x67fb, 0x67fc); push_effect(gb, 0x67fc);
  switch (child_jumpTable(gb)) {
    case 0x6802: goto substate0;
    case 0x6820: goto substate1;
    case 0x683a: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x6802, 0x6803); H = D;
  CYC(0x6803, 0x6805); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6805, 0x6807); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x6807, 0x6809); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6809, 0x680b); mem_wr(gb, HL, 0x18);

gotoSubstate1AndJump:
  CYC(0x680b, 0x680c); H = D;
  CYC(0x680c, 0x680e); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x680e, 0x6810); mem_wr(gb, HL, 0x01);
  CYC(0x6810, 0x6812); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x6812, 0x6814); mem_wr(gb, HL, 0x01);
  CYC(0x6814, 0x6816); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x6816, 0x6818); mem_wr(gb, HL, 0x00);
  CYC(0x6818, 0x6819); SET_HL(HL + 1);
  CYC(0x6819, 0x681b); mem_wr(gb, HL, 0xfb);
  CYC(0x681b, 0x681d); A = 0x53; // SND_JUMP
  CYC(0x681d, 0x6820); playSound_b00_hook(gb);
  return;

substate1:
  CYC(0x6820, 0x6822); C = 0x50;
  CALL_C(0x6822, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6825);
  if (!(F & FZ)) {
    CYCT(0x6825, 0x6828); objectApplySpeed_hook(gb); return;
  }
  CYC(0x6825, 0x6828);
  CALL_C(0x6828, interactionIncSubstate_hook, 0x23e5, 0x682b);
  CYC(0x682b, 0x682d); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x682d, 0x682f); mem_wr(gb, HL, 0x00);
  CYC(0x682f, 0x6831); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6831, 0x6833); mem_wr(gb, HL, 0x78);
  CYC(0x6833, 0x6835); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6835, 0x6836); A = mem_rd(gb, HL);
  CYC(0x6836, 0x6838); alu_xor(gb, 0x10);
  CYC(0x6838, 0x6839); mem_wr(gb, HL, A);
  CYC(0x6839, 0x683a); ret_effect(gb);
  return;

substate2:
  CYC(0x683a, 0x683b); H = D;
  CYC(0x683b, 0x683d); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x683d, 0x683e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x683e, 0x683f); ret_effect(gb); return;
  }
  CYC(0x683e, 0x683f);
  CYC(0x683f, 0x6841); goto gotoSubstate1AndJump;
}
