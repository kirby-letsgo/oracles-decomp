#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode35), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode35), (from), (to), true)

// INTERAC_CHILD (interactionCode35): the child raised by Link in Ages.
//   var03: index of script and code to run (personality + growth stage)
//   var37: animation base (from subid), var39: solidity mode, var3a: animation offset
//   var3c: current position-list index, var3d: entries in list minus one
//   var3e/3f: pointer to position list

// One pointer per var03 value (bank $0c scripts).
#define childScriptTable_bank08 SYM(childScriptTable)
// childDetermineAnimationBase@animations: one byte per subid.
#define childDetermineAnimationBase_animations_bank08 SYM(childDetermineAnimationBase__animations)
#define childHyperactiveMovementAngles_bank08 SYM(childHyperactiveMovementAngles)
// childLoadPositionListPointer@positionTable: word pointer, byte count-1, byte unused.
#define childLoadPositionListPointer_positionTable_bank08 SYM(childLoadPositionListPointer__positionTable)

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
  BASE(interactionCode35);
  CYC(b_+121, b_+122); H = D;
  CYC(b_+122, b_+124); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+124, b_+125); mem_wr(gb, HL, A);
  CYC(b_+125, b_+127); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+127, b_+128); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+128, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+131);
  CYC(b_+131, b_+134); childUpdateSolidityAndVisibility_hook(gb);
}

// @hyperactiveStage4Or5: also `call`ed from @val16 (synthetic local
// interactionCode35__hyperactiveStage4Or5). Falls through into @setAnimation.
static void child_hyperactiveStage4Or5(GB *gb, uint16_t sp0_) {
  BASE(interactionCode35);
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x01);
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+115, b_+117); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x18);
  CYC(b_+119, b_+121); A = 0x00;
  child_setAnimation(gb, sp0_);
}

// @state0: init graphics, pick the script from var03, then per-var03 setup.
static void child_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode35);
  CALL_C(b_+8, childDetermineAnimationBase_hook, SYM(childDetermineAnimationBase), b_+11);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+23); SET_HL(childScriptTable_bank08);
  CYC(b_+23, b_+24); child_addDoubleIndex(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); H = mem_rd(gb, HL);
  CYC(b_+26, b_+27); L = A;
  CALL_C(b_+27, interactionSetScript_hook, SYM(interactionSetScript), b_+30);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  do { uint16_t jt_ = (child_jumpTable(gb));
    if (jt_ == b_+92) { goto initAnimation; }
    else if (jt_ == b_+101) { goto hyperactiveStage6; }
    else if (jt_ == b_+106) { child_hyperactiveStage4Or5(gb, sp0_); return; }
    else if (jt_ == b_+134) { goto val16; }
    else if (jt_ == b_+143) { goto shyStage4Or5; }
    else if (jt_ == b_+150) { goto shyStage6; }
    else if (jt_ == b_+168) { goto curious; }
    else if (jt_ == b_+177) { goto slacker; }
    else if (jt_ == b_+181) { goto warrior; }
    else if (jt_ == b_+188) { goto script0f; }
    else if (jt_ == b_+206) { goto arborist; }
    else if (jt_ == b_+210) { goto singer; }
    else { HANDOFF(HL); }
  } while (0);

initAnimation:
  CYC(b_+92, b_+94); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CALL_C(b_+95, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+98);
  CYC(b_+98, b_+101); childUpdateSolidityAndVisibility_hook(gb);
  return;

hyperactiveStage6:
  CYC(b_+101, b_+103); A = 0x02;
  CALL_C(b_+103, childLoadPositionListPointer_hook, SYM(childLoadPositionListPointer), b_+106);
  child_hyperactiveStage4Or5(gb, sp0_);
  return;

val16:
  CYC(b_+134, b_+137); push_effect(gb, b_+137); child_hyperactiveStage4Or5(gb, sp0_);
  CYC(b_+137, b_+138); H = D;
  CYC(b_+138, b_+140); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+142, b_+143); ret_effect(gb);
  return;

shyStage4Or5:
  CYC(b_+143, b_+145); A = 0x00;
  CALL_C(b_+145, childLoadPositionListPointer_hook, SYM(childLoadPositionListPointer), b_+148);
  CYC(b_+148, b_+150); goto shyCommon;

shyStage6:
  CYC(b_+150, b_+152); A = 0x01;
  CALL_C(b_+152, childLoadPositionListPointer_hook, SYM(childLoadPositionListPointer), b_+155);

shyCommon:
  CYC(b_+155, b_+156); H = D;
  CYC(b_+156, b_+158); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x01);
  CYC(b_+160, b_+162); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+164, b_+166); A = 0x00;
  CYC(b_+166, b_+168); child_setAnimation(gb, sp0_);
  return;

curious:
  CYC(b_+168, b_+169); H = D;
  CYC(b_+169, b_+171); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+171, b_+173); mem_wr(gb, HL, 0x02);
  CYC(b_+173, b_+175); A = 0x00;
  CYC(b_+175, b_+177); child_setAnimation(gb, sp0_);
  return;

slacker:
  CYC(b_+177, b_+179); A = 0x00;
  CYC(b_+179, b_+181); child_setAnimation(gb, sp0_);
  return;

warrior:
  CYC(b_+181, b_+183); A = 0x03;
  CALL_C(b_+183, childLoadPositionListPointer_hook, SYM(childLoadPositionListPointer), b_+186);
  CYC(b_+186, b_+188); goto warriorCommon;

script0f:
  CYC(b_+188, b_+190); A = 0x04;
  CALL_C(b_+190, childLoadPositionListPointer_hook, SYM(childLoadPositionListPointer), b_+193);

warriorCommon:
  CYC(b_+193, b_+194); H = D;
  CYC(b_+194, b_+196); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0x01);
  CYC(b_+198, b_+200); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+200, b_+202); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+202, b_+204); A = 0x00;
  CYC(b_+204, b_+206); child_setAnimation(gb, sp0_);
  return;

arborist:
  CYC(b_+206, b_+208); A = 0x03;
  CYC(b_+208, b_+210); child_setAnimation(gb, sp0_);
  return;

singer:
  CYC(b_+210, b_+212); A = 0x00;
  CYC(b_+212, SYM(interac65_state1)); child_setAnimation(gb, sp0_);
}

void interactionCode35_hook(GB *gb) {
  BASE(interactionCode35);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (child_jumpTable(gb));
    if (jt_ == b_+8) { child_state0(gb, sp0_); return; }
    else if (jt_ == SYM(interac65_state1)) { interac65_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// State 1: per-var03 movement, then run the script and animate.
void interac65_state1_hook(GB *gb) {
  BASE(interac65_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (child_jumpTable(gb));
    if (jt_ == b_+74) { goto updateAnimationAndSolidity; }
    else if (jt_ == b_+62) { goto hyperactiveMovement; }
    else if (jt_ == b_+83) { goto shyMovement; }
    else if (jt_ == b_+114) { goto curiousMovement; }
    else if (jt_ == b_+94) { goto usePositionList; }
    else if (jt_ == b_+127) { goto slackerMovement; }
    else if (jt_ == b_+71) { goto arboristMovement; }
    else if (jt_ == b_+152) { goto singerMovement; }
    else if (jt_ == b_+77) { goto val16; }
    else if (jt_ == b_+124) { goto val1b; }
    else { HANDOFF(HL); }
  } while (0);

hyperactiveMovement:
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+66, b_+68); goto arboristMovement;
  }
  CYC(b_+66, b_+68);
  CALL_C(b_+68, childUpdateHyperactiveMovement_hook, SYM(childUpdateHyperactiveMovement), b_+71);

arboristMovement:
  CALL_C(b_+71, interactionRunScript_hook, SYM(interactionRunScript), b_+74);

updateAnimationAndSolidity:
  CYC(b_+74, b_+77); childUpdateAnimationAndSolidity_hook(gb);
  return;

val16:
  CALL_C(b_+77, childUpdateUnknownMovement_hook, SYM(childUpdateUnknownMovement), b_+80);
  CYC(b_+80, b_+83); childUpdateAnimationAndSolidity_hook(gb);
  return;

shyMovement:
  CYC(b_+83, b_+85); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+87, b_+89); goto shyDone;
  }
  CYC(b_+87, b_+89);
  CALL_C(b_+89, childUpdateShyMovement_hook, SYM(childUpdateShyMovement), b_+92);
shyDone:
  CYC(b_+92, b_+94); goto runScriptAndUpdateAnimation;

usePositionList:
  CYC(b_+94, b_+96); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+98, b_+100); goto positionListDone;
  }
  CYC(b_+98, b_+100);
  CALL_C(b_+100, childUpdateAngleAndApplySpeed_hook, SYM(childUpdateAngleAndApplySpeed), b_+103);
  CALL_C(b_+103, childCheckAnimationDirectionChanged_hook, SYM(childCheckAnimationDirectionChanged), b_+106);
  CALL_C(b_+106, childCheckReachedDestination_hook, SYM(childCheckReachedDestination), b_+109);
  if (F & FC) {
    CALL_C_CC(b_+109, childIncPositionIndex_hook, SYM(childIncPositionIndex), b_+112);
  } else {
    CYC(b_+109, b_+112);
  }
positionListDone:
  CYC(b_+112, b_+114); goto runScriptAndUpdateAnimation;

curiousMovement:
  CALL_C(b_+114, childUpdateCuriousMovement_hook, SYM(childUpdateCuriousMovement), b_+117);
  CYC(b_+117, b_+119); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  CYC(b_+120, b_+121); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+121, interactionRunScript_hook, SYM(interactionRunScript), b_+124);
  } else {
    CYC(b_+121, b_+124);
  }

val1b:
  CYC(b_+124, b_+127); childUpdateAnimationAndSolidity_hook(gb);
  return;

slackerMovement:
  CYC(b_+127, b_+130); A = mem_rd(gb, wFrameCounter);
  CYC(b_+130, b_+132); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(b_+132, b_+134); goto slackerDone;
  }
  CYC(b_+132, b_+134);
  CYC(b_+134, b_+136); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+139); alu_and(gb, 0x01);
  CYC(b_+139, b_+141); C = 0x08;
  if (!(F & FZ)) {
    CYCT(b_+141, b_+143);
  } else {
    CYC(b_+141, b_+143);
    CYC(b_+143, b_+145); C = 0xfc;
  }
  CYC(b_+145, b_+147); B = 0xf4;
  CALL_C(b_+147, objectCreateFloatingMusicNote_hook, SYM(objectCreateFloatingMusicNote), b_+150);
slackerDone:
  CYC(b_+150, b_+152); goto runScriptAndUpdateAnimation;

singerMovement:
  CYC(b_+152, b_+155); A = mem_rd(gb, wFrameCounter);
  CYC(b_+155, b_+157); alu_and(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(b_+157, b_+159); goto runScriptAndUpdateAnimation;
  }
  CYC(b_+157, b_+159);
  CYC(b_+159, b_+161); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+161, b_+162); A = mem_rd(gb, DE);
  CYC(b_+162, b_+163); alu_or(gb, A);
  CYC(b_+163, b_+165); C = 0xfc;
  if (F & FZ) {
    CYCT(b_+165, b_+167);
  } else {
    CYC(b_+165, b_+167);
    CYC(b_+167, b_+169); C = 0x00;
  }
  CYC(b_+169, b_+171); B = 0xfc;
  CALL_C(b_+171, objectCreateFloatingMusicNote_hook, SYM(objectCreateFloatingMusicNote), b_+174);

runScriptAndUpdateAnimation:
  CALL_C(b_+174, interactionRunScript_hook, SYM(interactionRunScript), b_+177);
  CYC(b_+177, SYM(childUpdateAnimationAndSolidity)); childUpdateAnimationAndSolidity_hook(gb);
}

// Falls through into childUpdateSolidityAndVisibility.
void childUpdateAnimationAndSolidity_hook(GB *gb) {
  BASE(childUpdateAnimationAndSolidity);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), SYM(childUpdateSolidityAndVisibility));
  childUpdateSolidityAndVisibility_hook(gb);
}

// var39: 0 = solid, 1 = "light" solidity (pushes Link away), 2 = no solidity.
void childUpdateSolidityAndVisibility_hook(GB *gb) {
  BASE(childUpdateSolidityAndVisibility);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+5, b_+7); goto lightSolidity;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+9, b_+12); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return;
  }
  CYC(b_+9, b_+12);
  CALL_C(b_+12, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+15);
  CYC(b_+15, b_+18); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
  return;

lightSolidity:
  CALL_C(b_+18, objectPushLinkAwayOnCollision_hook, SYM(objectPushLinkAwayOnCollision), b_+21);
  CYC(b_+21, SYM(childDetermineAnimationBase)); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// Writes the "base" animation index to var37 based on subid (personality type).
void childDetermineAnimationBase_hook(GB *gb) {
  BASE(childDetermineAnimationBase);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(childDetermineAnimationBase_animations_bank08);
  CYC(b_+6, b_+7); push_effect(gb, b_+7); child_addAToHl(gb);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

// Bounces between x=$29 and x=$68, cycling through 4 angles. Falls through into
// childFlipAnimation.
void childUpdateHyperactiveMovement_hook(GB *gb) {
  BASE(childUpdateHyperactiveMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_sub(gb, 0x29);
  CYC(b_+9, b_+11); alu_cp(gb, 0x40);
  if (F & FC) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
  } else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+17, b_+18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  CYC(b_+23, b_+25); alu_and(gb, 0x03);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+29); SET_BC(childHyperactiveMovementAngles_bank08);
  CALL_C(b_+29, addAToBc_hook, 0x006d, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, BC);
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+35, SYM(childFlipAnimation)); mem_wr(gb, HL, A);
  childFlipAnimation_hook(gb);
}

// Toggles bit 0 of var3a and sets animation var37 + var3a. Expects H = D.
void childFlipAnimation_hook(GB *gb) {
  BASE(childFlipAnimation);
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_xor(gb, 0x01);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+9, SYM(childHyperactiveMovementAngles)); interactionSetAnimation_hook(gb);
}

// Bounces between x=$14 and x=$3b, flipping the angle horizontally.
void childUpdateUnknownMovement_hook(GB *gb) {
  BASE(childUpdateUnknownMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_sub(gb, 0x14);
  CYC(b_+8, b_+10); alu_cp(gb, 0x28);
  if (F & FC) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_xor(gb, 0x10);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, SYM(childUpdateShyMovement)); childFlipAnimation_hook(gb);
}

// "Shy" personality: runs to the next position when Link approaches.
void childUpdateShyMovement_hook(GB *gb) {
  BASE(childUpdateShyMovement);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (child_jumpTable(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+17) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+8, b_+10); C = 0x18;
  CALL_C(b_+10, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+13);
  if (!(F & FC)) {
    CYCT(b_+13, b_+14); ret_effect(gb); return;
  }
  CYC(b_+13, b_+14);
  CALL_C(b_+14, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+17);

substate1:
  CALL_C(b_+17, childUpdateAngleAndApplySpeed_hook, SYM(childUpdateAngleAndApplySpeed), b_+20);
  CALL_C(b_+20, childCheckReachedDestination_hook, SYM(childCheckReachedDestination), b_+23);
  if (!(F & FC)) {
    CYCT(b_+23, b_+24); ret_effect(gb); return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x00);
  CYC(b_+29, SYM(childUpdateAngleAndApplySpeed)); childIncPositionIndex_hook(gb);
}

// Sets the angle toward position list entry var3c, then applies speed.
void childUpdateAngleAndApplySpeed_hook(GB *gb) {
  BASE(childUpdateAngleAndApplySpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); H = A;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+16); push_effect(gb, b_+16); child_addAToHl(gb);
  CYC(b_+16, b_+17); B = mem_rd(gb, HL);
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CALL_C(b_+19, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, SYM(childCheckReachedDestination)); objectApplySpeed_hook(gb);
}

// @param[out] cflag Set if within 1 pixel of position list entry var3c on both axes.
void childCheckReachedDestination_hook(GB *gb) {
  BASE(childCheckReachedDestination);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); push_effect(gb, AF);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); SET_AF(pop_effect(gb));
  CALL_C(b_+15, addAToBc_hook, 0x006d, b_+18);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+22); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+24); alu_add(gb, 0x01);
  CYC(b_+24, b_+26); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+26, b_+27); ret_effect(gb); return;
  }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); SET_BC(BC + 1);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+30, b_+31); A = mem_rd(gb, BC);
  CYC(b_+31, b_+32); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+34); alu_add(gb, 0x01);
  CYC(b_+34, b_+36); alu_cp(gb, 0x03);
  CYC(b_+36, SYM(childCheckAnimationDirectionChanged)); ret_effect(gb);
}

// Updates the animation when the horizontal direction derived from the angle changes.
void childCheckAnimationDirectionChanged_hook(GB *gb) {
  BASE(childCheckAnimationDirectionChanged);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  CYC(b_+8, b_+10); alu_xor(gb, 0x01);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+13, b_+14); ret_effect(gb); return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+17, b_+18); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+20, b_+21); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+21, SYM(childIncPositionIndex)); interactionSetAnimation_hook(gb);
}

// var3c++, wrapping to 0 once it passes var3d.
void childIncPositionIndex_hook(GB *gb) {
  BASE(childIncPositionIndex);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00);
  CYC(b_+11, SYM(childLoadPositionListPointer)); ret_effect(gb);
}

// Loads position list pointer A into var3e/var3f and its entry count-1 into var3d.
void childLoadPositionListPointer_hook(GB *gb) {
  BASE(childLoadPositionListPointer);
  CYC(b_+0, b_+1); alu_add(gb, A);
  CYC(b_+1, b_+2); alu_add(gb, A);
  CYC(b_+2, b_+5); SET_HL(childLoadPositionListPointer_positionTable_bank08);
  CYC(b_+5, b_+6); push_effect(gb, b_+6); child_addAToHl(gb);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

// "Curious" personality: hops back and forth, pausing $78 frames between hops.
void childUpdateCuriousMovement_hook(GB *gb) {
  BASE(childUpdateCuriousMovement);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (child_jumpTable(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+40) { goto substate1; }
    else if (jt_ == b_+66) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x18);

gotoSubstate1AndJump:
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x01);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x01);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x00);
  CYC(b_+32, b_+33); SET_HL(HL + 1);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xfb);
  CYC(b_+35, b_+37); A = 0x53; // SND_JUMP
  CYC(b_+37, b_+40); playSound_b00_hook(gb);
  return;

substate1:
  CYC(b_+40, b_+42); C = 0x50;
  CALL_C(b_+42, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+45);
  if (!(F & FZ)) {
    CYCT(b_+45, b_+48); objectApplySpeed_hook(gb); return;
  }
  CYC(b_+45, b_+48);
  CALL_C(b_+48, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+51);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x00);
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x78);
  CYC(b_+59, b_+61); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_xor(gb, 0x10);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  CYC(b_+65, b_+66); ret_effect(gb);
  return;

substate2:
  CYC(b_+66, b_+67); H = D;
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+70, b_+71); ret_effect(gb); return;
  }
  CYC(b_+70, b_+71);
  CYC(b_+71, SYM(childScriptTable)); goto gotoSubstate1AndJump;
}
