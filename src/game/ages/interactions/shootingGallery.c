#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Positions of the 10 target tiles for each gallery (10 bytes each).
#define shootingGallery_targetPositions_lynna_bank08 0x5829
#define shootingGallery_targetPositions_goron_bank08 0x5833
#define shootingGallery_targetPositions_biggoron_bank08 0x583d
// Possible target layouts for each gallery (10 layouts of 10 tiles each).
#define shootingGallery_targetTiles_lynna_bank08 0x5847
#define shootingGallery_targetTiles_goron_bank08 0x58ab
#define shootingGallery_targetTiles_biggoron_bank08 0x590f
// shootingGallery_addValueToScore@scores: word per hit index; bit 0 set means subtract.
#define shootingGallery_scores_bank08 0x59c1
// NPC scripts, 3 per row: waiting / game done / try again. Indexed by scriptIndex*3+subid.
#define shootingGalleryScriptTable_bank08 0x59eb
// Scripts to run for each combination of hit tiles ($14 = hit nothing, $15 = strike).
#define shootingGalleryHitScriptTable_bank08 0x59fd

#define INTERAC_PUFF 0x05
#define PART_BALL 0x38
#define TILEINDEX_STANDARD_FLOOR 0xa0

void shootingGalleryNpc_hook(GB *gb);
void shootingGalleryGame_hook(GB *gb);
void shootingGallery_initializeGameRounds_hook(GB *gb);
void shootingGallery_getNextTargetLayout_hook(GB *gb);
void shootingGallery_removeAllTargets_b08_hook(GB *gb);
void shootingGallery_setRandomTargetLayout_hook(GB *gb);
void shootingGallery_setTiles_hook(GB *gb);
void shootingGallery_createPuffAtEachTargetPosition_hook(GB *gb);
void shootingGallery_createBallHere_hook(GB *gb);
void shootingGallery_addValueToScore_hook(GB *gb);

static uint16_t shootingGallery_jumpTable(GB *gb) {
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

static void shootingGallery_addAToHl(GB *gb) {
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

static void shootingGallery_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SHOOTING_GALLERY: subids 0-2 are the gallery NPCs, subid 3 runs the game.
void interactionCode30_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x561a, 0x561c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x561c, 0x561d); A = mem_rd(gb, DE);
  CYC(0x561d, 0x561e); push_effect(gb, 0x561e);
  switch (shootingGallery_jumpTable(gb)) {
    case 0x5626: shootingGalleryNpc_hook(gb); return;
    case 0x5682: shootingGalleryGame_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// shootingGalleryNpc@setScript: load NPC script A (0 waiting, 1 game done, 2 try again)
// for this subid. Reached by static `call`s from shootingGalleryNpc itself and by
// fallthrough from @loadRetryScriptAndGotoState1.
static void shootingGalleryNpc_setScript(GB *gb) {
  CYC(0x5671, 0x5672); B = A;
  CYC(0x5672, 0x5673); alu_add(gb, A);
  CYC(0x5673, 0x5674); alu_add(gb, B);
  CYC(0x5674, 0x5675); H = D;
  CYC(0x5675, 0x5677); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5677, 0x5678); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5678, 0x567b); SET_HL(shootingGalleryScriptTable_bank08);
  CYC(0x567b, 0x567c); shootingGallery_addDoubleIndex(gb, 0x567c);
  CYC(0x567c, 0x567d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x567d, 0x567e); H = mem_rd(gb, HL);
  CYC(0x567e, 0x567f); L = A;
  CYC(0x567f, 0x5682); interactionSetScript_hook(gb);
}

// shootingGalleryNpc@loadRetryScriptAndGotoState1: reached only by a conditional `call`
// from shootingGalleryNpc state 3.
static void shootingGalleryNpc_loadRetryScriptAndGotoState1(GB *gb) {
  CYC(0x566a, 0x566b); H = D;
  CYC(0x566b, 0x566d); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x566d, 0x566f); mem_wr(gb, HL, 0x01);
  CYC(0x566f, 0x5671); A = 0x02;
  shootingGalleryNpc_setScript(gb);
}

// The shooting gallery NPC (subids 0-2). Interaction $8b (goron elder) also calls this.
void shootingGalleryNpc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5626, 0x5628); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5628, 0x5629); A = mem_rd(gb, DE);
  CYC(0x5629, 0x562a); push_effect(gb, 0x562a);
  switch (shootingGallery_jumpTable(gb)) {
    case 0x5632: goto state0;
    case 0x563f: goto state1;
    case 0x564b: goto state2;
    case 0x5659: goto state3;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x5632, 0x5634); A = 0x01;
  CYC(0x5634, 0x5635); mem_wr(gb, DE, A);
  CALL_C(0x5635, interactionInitGraphics_hook, 0x15fb, 0x5638);
  CYC(0x5638, 0x5639); alu_xor(gb, A);
  CYC(0x5639, 0x563c); mem_wr(gb, wTmpcfc0_shootingGallery_disableGoronNpcs, A);
  CYC(0x563c, 0x563f); push_effect(gb, 0x563f); shootingGalleryNpc_setScript(gb);

state1:
  // Waiting for the player to talk to the NPC and start the game
  CALL_C(0x563f, interactionRunScript_hook, 0x2552, 0x5642);
  if (!(F & FC)) {
    CYCT(0x5642, 0x5644); goto updateAnimation;
  }
  CYC(0x5642, 0x5644);
  CYC(0x5644, 0x5645); alu_xor(gb, A);
  CYC(0x5645, 0x5648); mem_wr(gb, wTmpcfc0_shootingGallery_gameStatus, A);
  CALL_C(0x5648, interactionIncState_hook, 0x23e0, 0x564b);

state2:
  // Waiting for the game to finish
  CYC(0x564b, 0x564e); A = mem_rd(gb, wTmpcfc0_shootingGallery_gameStatus);
  CYC(0x564e, 0x564f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x564f, 0x5651); goto updateAnimation;
  }
  CYC(0x564f, 0x5651);
  CYC(0x5651, 0x5653); A = 0x01;
  CYC(0x5653, 0x5656); push_effect(gb, 0x5656); shootingGalleryNpc_setScript(gb);
  CALL_C(0x5656, interactionIncState_hook, 0x23e0, 0x5659);

state3:
  // Waiting for the "game wrapup" script to finish, then ask to try again
  CALL_C(0x5659, interactionRunScript_hook, 0x2552, 0x565c);
  if (F & FC) {
    CYCT(0x565c, 0x565f); push_effect(gb, 0x565f);
    shootingGalleryNpc_loadRetryScriptAndGotoState1(gb);
  } else {
    CYC(0x565c, 0x565f);
  }

updateAnimation:
  CYC(0x565f, 0x5661); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5661, 0x5662); A = mem_rd(gb, DE);
  CYC(0x5662, 0x5664); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x5664, 0x5667); interactionAnimateAsNpc_hook(gb); return;
  }
  CYC(0x5664, 0x5667);
  CYC(0x5667, 0x566a); npcFaceLinkAndAnimate_hook(gb);
}

// Interaction $30, subid $03 runs the shooting gallery game. It cycles through states
// 1-6 a total of 10 times; var3f is the round counter.
void shootingGalleryGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5682, 0x5684); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5684, 0x5685); A = mem_rd(gb, DE);
  CYC(0x5685, 0x5686); push_effect(gb, 0x5686);
  switch (shootingGallery_jumpTable(gb)) {
    case 0x5694: goto state0;
    case 0x56bc: goto state1;
    case 0x56d6: goto state2;
    case 0x56e5: goto state3;
    case 0x56f4: goto state4;
    case 0x5701: goto state5;
    case 0x5765: goto state6;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x5694, 0x5696); A = 0x01;
  CYC(0x5696, 0x5699); mem_wr(gb, wTmpcfc0_shootingGallery_disableGoronNpcs, A);
  CYC(0x5699, 0x569b); B = 0x0a;
  CALL_C(0x569b, shootingGallery_initializeGameRounds_hook, 0x5786, 0x569e);
  // Initialize score
  CYC(0x569e, 0x569f); alu_xor(gb, A);
  CYC(0x569f, 0x56a2); mem_wr(gb, wTextNumberSubstitution, A);
  CYC(0x56a2, 0x56a5); mem_wr(gb, wTextNumberSubstitution + 1, A);
  CYC(0x56a5, 0x56a7); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x56a7, 0x56a8); mem_wr(gb, DE, A);
  CALL_C(0x56a8, interactionIncState_hook, 0x23e0, 0x56ab);
  CYC(0x56ab, 0x56ad); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x56ad, 0x56af); mem_wr(gb, HL, 0x2a);
  CYC(0x56af, 0x56b1); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x56b1, 0x56b3); mem_wr(gb, HL, 0x50);
  CYC(0x56b3, 0x56b5); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x56b5, 0x56b7); mem_wr(gb, HL, 0x78);
  CYC(0x56b7, 0x56b9); A = 0xcc; // SND_WHISTLE
  CALL_C(0x56b9, playSound_b00_hook, 0x0c98, 0x56bc);

state1:
  CALL_C(0x56bc, interactionDecCounter1_hook, 0x23cc, 0x56bf);
  if (!(F & FZ)) {
    CYCT(0x56bf, 0x56c0); ret_effect(gb); return;
  }
  CYC(0x56bf, 0x56c0);
  // These variables will be set by the "ball" object later
  CYC(0x56c0, 0x56c1); alu_xor(gb, A);
  CYC(0x56c1, 0x56c4); mem_wr(gb, wShootingGalleryBallStatus, A);
  CYC(0x56c4, 0x56c7); mem_wr(gb, wShootingGalleryccd5, A);
  CYC(0x56c7, 0x56ca); mem_wr(gb, wShootingGalleryHitTargets, A);
  CALL_C(0x56ca, interactionIncState_hook, 0x23e0, 0x56cd);
  CYC(0x56cd, 0x56cf); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x56cf, 0x56d1); mem_wr(gb, HL, 0x28);
  CYC(0x56d1, 0x56d3); A = 0x99; // SND_BASEBALL
  CALL_C(0x56d3, playSound_b00_hook, 0x0c98, 0x56d6);

state2:
  CALL_C(0x56d6, interactionDecCounter1_hook, 0x23cc, 0x56d9);
  if (!(F & FZ)) {
    CYCT(0x56d9, 0x56da); ret_effect(gb); return;
  }
  CYC(0x56d9, 0x56da);
  CALL_C(0x56da, shootingGallery_createPuffAtEachTargetPosition_hook, 0x5973, 0x56dd);
  CALL_C(0x56dd, interactionIncState_hook, 0x23e0, 0x56e0);
  CYC(0x56e0, 0x56e2); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x56e2, 0x56e4); mem_wr(gb, HL, 0x0a);
  CYC(0x56e4, 0x56e5); ret_effect(gb);
  return;

state3:
  CALL_C(0x56e5, interactionDecCounter1_hook, 0x23cc, 0x56e8);
  if (!(F & FZ)) {
    CYCT(0x56e8, 0x56e9); ret_effect(gb); return;
  }
  CYC(0x56e8, 0x56e9);
  CALL_C(0x56e9, shootingGallery_setRandomTargetLayout_hook, 0x57da, 0x56ec);
  CALL_C(0x56ec, interactionIncState_hook, 0x23e0, 0x56ef);
  CYC(0x56ef, 0x56f1); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x56f1, 0x56f3); mem_wr(gb, HL, 0x5a);
  CYC(0x56f3, 0x56f4); ret_effect(gb);
  return;

state4:
  CALL_C(0x56f4, interactionDecCounter1_hook, 0x23cc, 0x56f7);
  if (!(F & FZ)) {
    CYCT(0x56f7, 0x56f8); ret_effect(gb); return;
  }
  CYC(0x56f7, 0x56f8);
  CALL_C(0x56f8, interactionIncState_hook, 0x23e0, 0x56fb);
  // Increment the "round" of the game
  CYC(0x56fb, 0x56fd); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x56fd, 0x56fe); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x56fe, 0x5701); shootingGallery_createBallHere_hook(gb);
  return;

state5:
  CYC(0x5701, 0x5704); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(0x5704, 0x5706); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x5706, 0x5707); ret_effect(gb); return;
  }
  CYC(0x5706, 0x5707);
  // Ball has gone out-of-bounds
  CYC(0x5707, 0x5709); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(0x5709, 0x570b); goto hitSomething;
  }
  CYC(0x5709, 0x570b);
  CYC(0x570b, 0x570e); A = mem_rd(gb, wTmpcfc0_shootingGallery_isStrike);
  CYC(0x570e, 0x570f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x570f, 0x5711); goto strike;
  }
  CYC(0x570f, 0x5711);
  // Hit nothing, but not a strike
  CYC(0x5711, 0x5713); A = 0x14;
  CYC(0x5713, 0x5715); goto setScript;

strike:
  CYC(0x5715, 0x5717); A = 0x14;
  CALL_C(0x5717, shootingGallery_addValueToScore_hook, 0x59ab, 0x571a);
  CYC(0x571a, 0x571c); A = 0x15;
  CYC(0x571c, 0x571e); goto setScript;

hitSomething:
  CYC(0x571e, 0x5720); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x5720, 0x5722); goto hit2Things;
  }
  CYC(0x5720, 0x5722);
  CYC(0x5722, 0x5725); A = mem_rd(gb, wShootingGalleryHitTargets);
  CYC(0x5725, 0x5727); alu_and(gb, 0x0f);
  CALL_C(0x5727, getHighestSetBit_hook, 0x01ea, 0x572a);
  CYC(0x572a, 0x572c); goto addValueToScore;

hit2Things:
  CYC(0x572c, 0x572f); A = mem_rd(gb, wShootingGalleryHitTargets);
  CYC(0x572f, 0x5731); alu_and(gb, 0x0f);
  CALL_C(0x5731, getHighestSetBit_hook, 0x01ea, 0x5734);
  CYC(0x5734, 0x5735); A = alu_inc8(gb, A);
  CYC(0x5735, 0x5736); alu_add(gb, A);
  CYC(0x5736, 0x5737); alu_add(gb, A);
  CYC(0x5737, 0x5738); B = A;
  CYC(0x5738, 0x573b); A = mem_rd(gb, wShootingGalleryHitTargets);
  CYC(0x573b, 0x573d); A = alu_swap(gb, A);
  CYC(0x573d, 0x573f); alu_and(gb, 0x0f);
  CALL_C(0x573f, getHighestSetBit_hook, 0x01ea, 0x5742);
  CYC(0x5742, 0x5743); alu_add(gb, B);

addValueToScore:
  CYC(0x5743, 0x5745); H8(hFF93) = A;
  CALL_C(0x5745, shootingGallery_addValueToScore_hook, 0x59ab, 0x5748);
  CYC(0x5748, 0x574a); A = H8(hFF93);

setScript:
  CYC(0x574a, 0x574d); SET_HL(shootingGalleryHitScriptTable_bank08);
  CYC(0x574d, 0x574e); shootingGallery_addDoubleIndex(gb, 0x574e);
  CYC(0x574e, 0x574f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x574f, 0x5750); H = mem_rd(gb, HL);
  CYC(0x5750, 0x5751); L = A;
  CALL_C(0x5751, interactionSetScript_hook, 0x2544, 0x5754);
  CALL_C(0x5754, interactionIncState_hook, 0x23e0, 0x5757);
  CYC(0x5757, 0x5759); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5759, 0x575b); mem_wr(gb, HL, 0x28);
  CYC(0x575b, 0x575d); A = 0x81;
  CYC(0x575d, 0x5760); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5760, 0x5762); A = 0x80;
  CYC(0x5762, 0x5765); mem_wr(gb, wMenuDisabled, A);

state6:
  CALL_C(0x5765, interactionRunScript_hook, 0x2552, 0x5768);
  if (!(F & FC)) {
    CYCT(0x5768, 0x5769); ret_effect(gb); return;
  }
  CYC(0x5768, 0x5769);
  // End the game on the tenth round
  CYC(0x5769, 0x576b); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x576b, 0x576c); A = mem_rd(gb, DE);
  CYC(0x576c, 0x576e); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x576e, 0x5770); goto gameOver;
  }
  CYC(0x576e, 0x5770);
  CYC(0x5770, 0x5771); H = D;
  CYC(0x5771, 0x5773); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5773, 0x5775); mem_wr(gb, HL, 0x01);
  CYC(0x5775, 0x5777); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5777, 0x5779); mem_wr(gb, HL, 0x14);
  CYC(0x5779, 0x577a); ret_effect(gb);
  return;

gameOver:
  CYC(0x577a, 0x577c); A = 0x01;
  CYC(0x577c, 0x577f); mem_wr(gb, wTmpcfc0_shootingGallery_gameStatus, A);
  CYC(0x577f, 0x5780); alu_xor(gb, A);
  CYC(0x5780, 0x5783); mem_wr(gb, wTmpcfc0_shootingGallery_disableGoronNpcs, A);
  CYC(0x5783, 0x5786); interactionDelete_hook(gb);
}

// Fill wShootingGalleryTileLayoutsToShow with 0..B-1 and set remainingRounds = B.
// Also used by the goron dance minigame.
void shootingGallery_initializeGameRounds_hook(GB *gb) {
  CYC(0x5786, 0x5789); SET_HL(wShootingGalleryTileLayoutsToShow);
  CYC(0x5789, 0x578a); alu_xor(gb, A);
fill:
  CYC(0x578a, 0x578b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x578b, 0x578c); A = alu_inc8(gb, A);
  CYC(0x578c, 0x578d); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x578d, 0x578f); goto fill;
  }
  CYC(0x578d, 0x578f);
  CYC(0x578f, 0x5792); mem_wr(gb, wTmpcfc0_shootingGallery_remainingRounds, A);
  CYC(0x5792, 0x5793); ret_effect(gb);
}

// Randomly pick the next layout from the remaining-rounds buffer, then shift the buffer
// down by one so the chosen value is removed.
void shootingGallery_getNextTargetLayout_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5793, 0x5796); A = mem_rd(gb, wTmpcfc0_shootingGallery_remainingRounds);
  CYC(0x5796, 0x5797); B = A;
  CYC(0x5797, 0x5798); A = alu_dec8(gb, A);
  CYC(0x5798, 0x579b); mem_wr(gb, wTmpcfc0_shootingGallery_remainingRounds, A);
  // Get a random number between 0 and b-1
  CALL_C(0x579b, getRandomNumber_hook, 0x043e, 0x579e);
modulo:
  CYC(0x579e, 0x579f); alu_sub(gb, B);
  if (!(F & FC)) {
    CYCT(0x579f, 0x57a1); goto modulo;
  }
  CYC(0x579f, 0x57a1);
  CYC(0x57a1, 0x57a2); alu_add(gb, B);
  CYC(0x57a2, 0x57a3); C = A;
  CYC(0x57a3, 0x57a6); SET_HL(wShootingGalleryTileLayoutsToShow);
  CYC(0x57a6, 0x57a7); push_effect(gb, 0x57a7); shootingGallery_addAToHl(gb);
  CYC(0x57a7, 0x57a8); A = mem_rd(gb, HL);
  CYC(0x57a8, 0x57ab); mem_wr(gb, wTmpcfc0_shootingGallery_targetLayoutIndex, A);
  CYC(0x57ab, 0x57ac); push_effect(gb, DE);
  CYC(0x57ac, 0x57ad); D = C;
  CYC(0x57ad, 0x57ae); E = B;
  CYC(0x57ae, 0x57af); E = alu_dec8(gb, E);
  CYC(0x57af, 0x57b0); B = H;
  CYC(0x57b0, 0x57b1); C = L;
shift:
  CYC(0x57b1, 0x57b2); A = D;
  CYC(0x57b2, 0x57b3); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(0x57b3, 0x57b5); goto shiftDone;
  }
  CYC(0x57b3, 0x57b5);
  CYC(0x57b5, 0x57b6); SET_BC(BC + 1);
  CYC(0x57b6, 0x57b7); A = mem_rd(gb, BC);
  CYC(0x57b7, 0x57b8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x57b8, 0x57b9); D = alu_inc8(gb, D);
  CYC(0x57b9, 0x57bb); goto shift;
shiftDone:
  CYC(0x57bb, 0x57bc); SET_DE(pop_effect(gb));
  CYC(0x57bc, 0x57bd); ret_effect(gb);
}

// Replace all of this gallery's target tiles with standard floor. Called from bank $15
// via interBankCall.
void shootingGallery_removeAllTargets_b08_hook(GB *gb) {
  CYC(0x57bd, 0x57bf); A = 0x01;
  CYC(0x57bf, 0x57c2); mem_wr(gb, wTmpcfc0_shootingGallery_useTileIndexData, A);
  CYC(0x57c2, 0x57c4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x57c4, 0x57c5); A = mem_rd(gb, DE);
  CYC(0x57c5, 0x57c7); alu_sub(gb, 0x01);
  if (F & FZ) {
    CYCT(0x57c7, 0x57c9); goto subid1;
  }
  CYC(0x57c7, 0x57c9);
  if (!(F & FC)) {
    CYCT(0x57c9, 0x57cb); goto subid2;
  }
  CYC(0x57c9, 0x57cb);
  CYC(0x57cb, 0x57ce); SET_BC(shootingGallery_targetPositions_lynna_bank08);
  CYC(0x57ce, 0x57d0); shootingGallery_setTiles_hook(gb);
  return;
subid1:
  CYC(0x57d0, 0x57d3); SET_BC(shootingGallery_targetPositions_goron_bank08);
  CYC(0x57d3, 0x57d5); shootingGallery_setTiles_hook(gb);
  return;
subid2:
  CYC(0x57d5, 0x57d8); SET_BC(shootingGallery_targetPositions_biggoron_bank08);
  CYC(0x57d8, 0x57da); shootingGallery_setTiles_hook(gb);
}

// Choose one of the 10 target layouts (never the same one twice) and load its tiles.
// The biggoron branch falls through into shootingGallery_setTiles.
void shootingGallery_setRandomTargetLayout_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57da, 0x57db); alu_xor(gb, A);
  CYC(0x57db, 0x57de); mem_wr(gb, wTmpcfc0_shootingGallery_useTileIndexData, A);
  CALL_C(0x57de, shootingGallery_getNextTargetLayout_hook, 0x5793, 0x57e1);
  CYC(0x57e1, 0x57e4); A = mem_rd(gb, wTmpcfc0_shootingGallery_targetLayoutIndex);
  // l = a*5
  CYC(0x57e4, 0x57e5); L = A;
  CYC(0x57e5, 0x57e6); alu_add(gb, A);
  CYC(0x57e6, 0x57e7); alu_add(gb, A);
  CYC(0x57e7, 0x57e8); alu_add(gb, L);
  CYC(0x57e8, 0x57e9); L = A;
  CYC(0x57e9, 0x57eb); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x57eb, 0x57ec); A = mem_rd(gb, DE);
  CYC(0x57ec, 0x57ee); alu_sub(gb, 0x01);
  CYC(0x57ee, 0x57ef); A = L;
  if (F & FZ) {
    CYCT(0x57ef, 0x57f1); goto goronGallery;
  }
  CYC(0x57ef, 0x57f1);
  if (!(F & FC)) {
    CYCT(0x57f1, 0x57f3); goto biggoronGallery;
  }
  CYC(0x57f1, 0x57f3);
  // @lynnaGallery
  CYC(0x57f3, 0x57f6); SET_HL(shootingGallery_targetTiles_lynna_bank08);
  CYC(0x57f6, 0x57f7); shootingGallery_addDoubleIndex(gb, 0x57f7);
  CYC(0x57f7, 0x57fa); SET_BC(shootingGallery_targetPositions_lynna_bank08);
  CYC(0x57fa, 0x57fc); shootingGallery_setTiles_hook(gb);
  return;
goronGallery:
  CYC(0x57fc, 0x57ff); SET_HL(shootingGallery_targetTiles_goron_bank08);
  CYC(0x57ff, 0x5800); shootingGallery_addDoubleIndex(gb, 0x5800);
  CYC(0x5800, 0x5803); SET_BC(shootingGallery_targetPositions_goron_bank08);
  CYC(0x5803, 0x5805); shootingGallery_setTiles_hook(gb);
  return;
biggoronGallery:
  CYC(0x5805, 0x5808); SET_HL(shootingGallery_targetTiles_biggoron_bank08);
  CYC(0x5808, 0x5809); shootingGallery_addDoubleIndex(gb, 0x5809);
  CYC(0x5809, 0x580c); SET_BC(shootingGallery_targetPositions_biggoron_bank08);
  shootingGallery_setTiles_hook(gb);
}

// Replace the 10 tiles at positions BC with the tile indices at HL, or with
// TILEINDEX_STANDARD_FLOOR when useTileIndexData is nonzero.
void shootingGallery_setTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x580c, 0x580e); A = 0x0a;
nextTile:
  CYC(0x580e, 0x5810); H8(hFF92) = A;
  CYC(0x5810, 0x5811); A = mem_rd(gb, BC);
  CYC(0x5811, 0x5812); SET_BC(BC + 1);
  CYC(0x5812, 0x5813); push_effect(gb, BC);
  CYC(0x5813, 0x5814); C = A;
  CYC(0x5814, 0x5817); A = mem_rd(gb, wTmpcfc0_shootingGallery_useTileIndexData);
  CYC(0x5817, 0x5818); alu_or(gb, A);
  CYC(0x5818, 0x581a); A = TILEINDEX_STANDARD_FLOOR;
  if (!(F & FZ)) {
    CYCT(0x581a, 0x581c); goto haveTile;
  }
  CYC(0x581a, 0x581c);
  CYC(0x581c, 0x581d); A = mem_rd(gb, HL); SET_HL(HL + 1);
haveTile:
  CYC(0x581d, 0x581e); push_effect(gb, HL);
  CALL_C(0x581e, setTile_hook, 0x3a9c, 0x5821);
  CYC(0x5821, 0x5822); SET_HL(pop_effect(gb));
  CYC(0x5822, 0x5823); SET_BC(pop_effect(gb));
  CYC(0x5823, 0x5825); A = H8(hFF92);
  CYC(0x5825, 0x5826); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5826, 0x5828); goto nextTile;
  }
  CYC(0x5826, 0x5828);
  CYC(0x5828, 0x5829); ret_effect(gb);
}

// Spawn an INTERAC_PUFF at each of this gallery's 10 target positions.
void shootingGallery_createPuffAtEachTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5973, 0x5975); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5975, 0x5976); A = mem_rd(gb, DE);
  CYC(0x5976, 0x5978); alu_sub(gb, 0x01);
  if (F & FZ) {
    CYCT(0x5978, 0x597a); goto subid1;
  }
  CYC(0x5978, 0x597a);
  if (!(F & FC)) {
    CYCT(0x597a, 0x597c); goto subid2;
  }
  CYC(0x597a, 0x597c);
  CYC(0x597c, 0x597f); SET_BC(shootingGallery_targetPositions_lynna_bank08);
  CYC(0x597f, 0x5981); goto start;
subid1:
  CYC(0x5981, 0x5984); SET_BC(shootingGallery_targetPositions_goron_bank08);
  CYC(0x5984, 0x5986); goto start;
subid2:
  CYC(0x5986, 0x5989); SET_BC(shootingGallery_targetPositions_biggoron_bank08);
start:
  CYC(0x5989, 0x598b); A = 0x0a;
nextTile:
  CYC(0x598b, 0x598d); H8(hFF92) = A;
  CALL_C(0x598d, getFreeInteractionSlot_hook, 0x3aef, 0x5990);
  if (!(F & FZ)) {
    CYCT(0x5990, 0x5991); ret_effect(gb); return;
  }
  CYC(0x5990, 0x5991);
  CYC(0x5991, 0x5993); mem_wr(gb, HL, INTERAC_PUFF);
  CYC(0x5993, 0x5994); A = mem_rd(gb, BC);
  CYC(0x5994, 0x5995); SET_BC(BC + 1);
  CYC(0x5995, 0x5996); push_effect(gb, BC);
  CYC(0x5996, 0x5998); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(0x5998, setShortPosition_hook, 0x20b8, 0x599b);
  CYC(0x599b, 0x599c); SET_BC(pop_effect(gb));
  CYC(0x599c, 0x599e); A = H8(hFF92);
  CYC(0x599e, 0x599f); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x599f, 0x59a1); goto nextTile;
  }
  CYC(0x599f, 0x59a1);
  CYC(0x59a1, 0x59a2); ret_effect(gb);
}

// Spawn a PART_BALL at this object's position.
void shootingGallery_createBallHere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59a2, getFreePartSlot_hook, 0x3e8e, 0x59a5);
  if (!(F & FZ)) {
    CYCT(0x59a5, 0x59a6); ret_effect(gb); return;
  }
  CYC(0x59a5, 0x59a6);
  CYC(0x59a6, 0x59a8); mem_wr(gb, HL, PART_BALL);
  CYC(0x59a8, 0x59ab); objectCopyPosition_hook(gb);
}

// Add (or, when bit 0 of the score entry is set, subtract) the score for hit index A.
void shootingGallery_addValueToScore_hook(GB *gb) {
  CYC(0x59ab, 0x59ae); SET_HL(shootingGallery_scores_bank08);
  CYC(0x59ae, 0x59af); shootingGallery_addDoubleIndex(gb, 0x59af);
  CYC(0x59af, 0x59b0); C = mem_rd(gb, HL);
  CYC(0x59b0, 0x59b1); SET_HL(HL + 1);
  CYC(0x59b1, 0x59b2); B = mem_rd(gb, HL);
  CYC(0x59b2, 0x59b5); SET_HL(wTextNumberSubstitution);
  CYC(0x59b5, 0x59b7); alu_bit(gb, 0, C);
  if (!(F & FZ)) {
    CYCT(0x59b7, 0x59b9); goto subtract;
  }
  CYC(0x59b7, 0x59b9);
  CYC(0x59b9, 0x59bc); addDecimalToHlRef_hook(gb);
  return;
subtract:
  CYC(0x59bc, 0x59be); C = (uint8_t)(C & ~(1 << 0));
  CYC(0x59be, 0x59c1); subDecimalFromHlRef_hook(gb);
}
