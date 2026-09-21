#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Positions of the 10 target tiles for each gallery (10 bytes each).
#define shootingGallery_targetPositions_lynna_bank08 SYM(shootingGallery_targetPositions_lynna)
#define shootingGallery_targetPositions_goron_bank08 SYM(shootingGallery_targetPositions_goron)
#define shootingGallery_targetPositions_biggoron_bank08 SYM(shootingGallery_targetPositions_biggoron)
// Possible target layouts for each gallery (10 layouts of 10 tiles each).
#define shootingGallery_targetTiles_lynna_bank08 SYM(shootingGallery_targetTiles_lynna)
#define shootingGallery_targetTiles_goron_bank08 SYM(shootingGallery_targetTiles_goron)
#define shootingGallery_targetTiles_biggoron_bank08 SYM(shootingGallery_targetTiles_biggoron)
// shootingGallery_addValueToScore@scores: word per hit index; bit 0 set means subtract.
#define shootingGallery_scores_bank08 SYM(shootingGallery_addValueToScore__scores)
// NPC scripts, 3 per row: waiting / game done / try again. Indexed by scriptIndex*3+subid.
#define shootingGalleryScriptTable_bank08 SYM(shootingGalleryScriptTable)
// Scripts to run for each combination of hit tiles ($14 = hit nothing, $15 = strike).
#define shootingGalleryHitScriptTable_bank08 SYM(shootingGalleryHitScriptTable)

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
  BASE(interactionCode30);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (shootingGallery_jumpTable(gb));
    if (jt_ == SYM(shootingGalleryNpc) && hook_is(gb, SYM(shootingGalleryNpc), shootingGalleryNpc_hook)) { shootingGalleryNpc_hook(gb); return; }
    else if (jt_ == SYM(shootingGalleryGame) && hook_is(gb, SYM(shootingGalleryGame), shootingGalleryGame_hook)) { shootingGalleryGame_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// shootingGalleryNpc@setScript: load NPC script A (0 waiting, 1 game done, 2 try again)
// for this subid. Reached by static `call`s from shootingGalleryNpc itself and by
// fallthrough from @loadRetryScriptAndGotoState1.
static void shootingGalleryNpc_setScript(GB *gb) {
  BASE(shootingGalleryNpc);
  CYC(b_+75, b_+76); B = A;
  CYC(b_+76, b_+77); alu_add(gb, A);
  CYC(b_+77, b_+78); alu_add(gb, B);
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+81); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+81, b_+82); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+82, b_+85); SET_HL(shootingGalleryScriptTable_bank08);
  CYC(b_+85, b_+86); shootingGallery_addDoubleIndex(gb, b_+86);
  CYC(b_+86, b_+87); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+87, b_+88); H = mem_rd(gb, HL);
  CYC(b_+88, b_+89); L = A;
  CYC(b_+89, b_+92); interactionSetScript_hook(gb);
}

// shootingGalleryNpc@loadRetryScriptAndGotoState1: reached only by a conditional `call`
// from shootingGalleryNpc state 3.
static void shootingGalleryNpc_loadRetryScriptAndGotoState1(GB *gb) {
  BASE(shootingGalleryNpc);
  CYC(b_+68, b_+69); H = D;
  CYC(b_+69, b_+71); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x01);
  CYC(b_+73, b_+75); A = 0x02;
  shootingGalleryNpc_setScript(gb);
}

// The shooting gallery NPC (subids 0-2). Interaction $8b (goron elder) also calls this.
void shootingGalleryNpc_hook(GB *gb) {
  BASE(shootingGalleryNpc);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (shootingGallery_jumpTable(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+25) { goto state1; }
    else if (jt_ == b_+37) { goto state2; }
    else if (jt_ == b_+51) { goto state3; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, wTmpcfc0_shootingGallery_disableGoronNpcs, A);
  CYC(b_+22, b_+25); push_effect(gb, b_+25); shootingGalleryNpc_setScript(gb);

state1:
  // Waiting for the player to talk to the NPC and start the game
  CALL_C(b_+25, interactionRunScript_hook, SYM(interactionRunScript), b_+28);
  if (!(F & FC)) {
    CYCT(b_+28, b_+30); goto updateAnimation;
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); alu_xor(gb, A);
  CYC(b_+31, b_+34); mem_wr(gb, wTmpcfc0_shootingGallery_gameStatus, A);
  CALL_C(b_+34, interactionIncState_hook, SYM(interactionIncState), b_+37);

state2:
  // Waiting for the game to finish
  CYC(b_+37, b_+40); A = mem_rd(gb, wTmpcfc0_shootingGallery_gameStatus);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+41, b_+43); goto updateAnimation;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x01;
  CYC(b_+45, b_+48); push_effect(gb, b_+48); shootingGalleryNpc_setScript(gb);
  CALL_C(b_+48, interactionIncState_hook, SYM(interactionIncState), b_+51);

state3:
  // Waiting for the "game wrapup" script to finish, then ask to try again
  CALL_C(b_+51, interactionRunScript_hook, SYM(interactionRunScript), b_+54);
  if (F & FC) {
    CYCT(b_+54, b_+57); push_effect(gb, b_+57);
    shootingGalleryNpc_loadRetryScriptAndGotoState1(gb);
  } else {
    CYC(b_+54, b_+57);
  }

updateAnimation:
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+62); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+62, b_+65); TAIL(interactionAnimateAsNpc);
  }
  CYC(b_+62, b_+65);
  CYC(b_+65, b_+68); TAIL(npcFaceLinkAndAnimate);
}

// Interaction $30, subid $03 runs the shooting gallery game. It cycles through states
// 1-6 a total of 10 times; var3f is the round counter.
void shootingGalleryGame_hook(GB *gb) {
  BASE(shootingGalleryGame);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (shootingGallery_jumpTable(gb));
    if (jt_ == b_+18) { goto state0; }
    else if (jt_ == b_+58) { goto state1; }
    else if (jt_ == b_+84) { goto state2; }
    else if (jt_ == b_+99) { goto state3; }
    else if (jt_ == b_+114) { goto state4; }
    else if (jt_ == b_+127) { goto state5; }
    else if (jt_ == b_+227) { goto state6; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+23); mem_wr(gb, wTmpcfc0_shootingGallery_disableGoronNpcs, A);
  CYC(b_+23, b_+25); B = 0x0a;
  CALL_C(b_+25, shootingGallery_initializeGameRounds_hook, SYM(shootingGallery_initializeGameRounds), b_+28);
  // Initialize score
  CYC(b_+28, b_+29); alu_xor(gb, A);
  CYC(b_+29, b_+32); mem_wr(gb, wTextNumberSubstitution, A);
  CYC(b_+32, b_+35); mem_wr(gb, wTextNumberSubstitution + 1, A);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CALL_C(b_+38, interactionIncState_hook, SYM(interactionIncState), b_+41);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x2a);
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x50);
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x78);
  CYC(b_+53, b_+55); A = 0xcc; // SND_WHISTLE
  CALL_C(b_+55, playSound_b00_hook, SYM(playSound_b00), b_+58);

state1:
  CALL_C(b_+58, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+61);
  if (!(F & FZ)) {
    CYCT(b_+61, b_+62); ret_effect(gb); return;
  }
  CYC(b_+61, b_+62);
  // These variables will be set by the "ball" object later
  CYC(b_+62, b_+63); alu_xor(gb, A);
  CYC(b_+63, b_+66); mem_wr(gb, wShootingGalleryBallStatus, A);
  CYC(b_+66, b_+69); mem_wr(gb, wShootingGalleryccd5, A);
  CYC(b_+69, b_+72); mem_wr(gb, wShootingGalleryHitTargets, A);
  CALL_C(b_+72, interactionIncState_hook, SYM(interactionIncState), b_+75);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x28);
  CYC(b_+79, b_+81); A = 0x99; // SND_BASEBALL
  CALL_C(b_+81, playSound_b00_hook, SYM(playSound_b00), b_+84);

state2:
  CALL_C(b_+84, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+87);
  if (!(F & FZ)) {
    CYCT(b_+87, b_+88); ret_effect(gb); return;
  }
  CYC(b_+87, b_+88);
  CALL_C(b_+88, shootingGallery_createPuffAtEachTargetPosition_hook, SYM(shootingGallery_createPuffAtEachTargetPosition), b_+91);
  CALL_C(b_+91, interactionIncState_hook, SYM(interactionIncState), b_+94);
  CYC(b_+94, b_+96); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x0a);
  CYC(b_+98, b_+99); ret_effect(gb);
  return;

state3:
  CALL_C(b_+99, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+102);
  if (!(F & FZ)) {
    CYCT(b_+102, b_+103); ret_effect(gb); return;
  }
  CYC(b_+102, b_+103);
  CALL_C(b_+103, shootingGallery_setRandomTargetLayout_hook, SYM(shootingGallery_setRandomTargetLayout), b_+106);
  CALL_C(b_+106, interactionIncState_hook, SYM(interactionIncState), b_+109);
  CYC(b_+109, b_+111); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x5a);
  CYC(b_+113, b_+114); ret_effect(gb);
  return;

state4:
  CALL_C(b_+114, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+117);
  if (!(F & FZ)) {
    CYCT(b_+117, b_+118); ret_effect(gb); return;
  }
  CYC(b_+117, b_+118);
  CALL_C(b_+118, interactionIncState_hook, SYM(interactionIncState), b_+121);
  // Increment the "round" of the game
  CYC(b_+121, b_+123); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+123, b_+124); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+124, b_+127); shootingGallery_createBallHere_hook(gb);
  return;

state5:
  CYC(b_+127, b_+130); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(b_+130, b_+132); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+132, b_+133); ret_effect(gb); return;
  }
  CYC(b_+132, b_+133);
  // Ball has gone out-of-bounds
  CYC(b_+133, b_+135); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(b_+135, b_+137); goto hitSomething;
  }
  CYC(b_+135, b_+137);
  CYC(b_+137, b_+140); A = mem_rd(gb, wTmpcfc0_shootingGallery_isStrike);
  CYC(b_+140, b_+141); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+141, b_+143); goto strike;
  }
  CYC(b_+141, b_+143);
  // Hit nothing, but not a strike
  CYC(b_+143, b_+145); A = 0x14;
  CYC(b_+145, b_+147); goto setScript;

strike:
  CYC(b_+147, b_+149); A = 0x14;
  CALL_C(b_+149, shootingGallery_addValueToScore_hook, SYM(shootingGallery_addValueToScore), b_+152);
  CYC(b_+152, b_+154); A = 0x15;
  CYC(b_+154, b_+156); goto setScript;

hitSomething:
  CYC(b_+156, b_+158); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+158, b_+160); goto hit2Things;
  }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+163); A = mem_rd(gb, wShootingGalleryHitTargets);
  CYC(b_+163, b_+165); alu_and(gb, 0x0f);
  CALL_C(b_+165, getHighestSetBit_hook, SYM(getHighestSetBit), b_+168);
  CYC(b_+168, b_+170); goto addValueToScore;

hit2Things:
  CYC(b_+170, b_+173); A = mem_rd(gb, wShootingGalleryHitTargets);
  CYC(b_+173, b_+175); alu_and(gb, 0x0f);
  CALL_C(b_+175, getHighestSetBit_hook, SYM(getHighestSetBit), b_+178);
  CYC(b_+178, b_+179); A = alu_inc8(gb, A);
  CYC(b_+179, b_+180); alu_add(gb, A);
  CYC(b_+180, b_+181); alu_add(gb, A);
  CYC(b_+181, b_+182); B = A;
  CYC(b_+182, b_+185); A = mem_rd(gb, wShootingGalleryHitTargets);
  CYC(b_+185, b_+187); A = alu_swap(gb, A);
  CYC(b_+187, b_+189); alu_and(gb, 0x0f);
  CALL_C(b_+189, getHighestSetBit_hook, SYM(getHighestSetBit), b_+192);
  CYC(b_+192, b_+193); alu_add(gb, B);

addValueToScore:
  CYC(b_+193, b_+195); H8(hFF93) = A;
  CALL_C(b_+195, shootingGallery_addValueToScore_hook, SYM(shootingGallery_addValueToScore), b_+198);
  CYC(b_+198, b_+200); A = H8(hFF93);

setScript:
  CYC(b_+200, b_+203); SET_HL(shootingGalleryHitScriptTable_bank08);
  CYC(b_+203, b_+204); shootingGallery_addDoubleIndex(gb, b_+204);
  CYC(b_+204, b_+205); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+205, b_+206); H = mem_rd(gb, HL);
  CYC(b_+206, b_+207); L = A;
  CALL_C(b_+207, interactionSetScript_hook, SYM(interactionSetScript), b_+210);
  CALL_C(b_+210, interactionIncState_hook, SYM(interactionIncState), b_+213);
  CYC(b_+213, b_+215); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+215, b_+217); mem_wr(gb, HL, 0x28);
  CYC(b_+217, b_+219); A = 0x81;
  CYC(b_+219, b_+222); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+222, b_+224); A = 0x80;
  CYC(b_+224, b_+227); mem_wr(gb, wMenuDisabled, A);

state6:
  CALL_C(b_+227, interactionRunScript_hook, SYM(interactionRunScript), b_+230);
  if (!(F & FC)) {
    CYCT(b_+230, b_+231); ret_effect(gb); return;
  }
  CYC(b_+230, b_+231);
  // End the game on the tenth round
  CYC(b_+231, b_+233); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+233, b_+234); A = mem_rd(gb, DE);
  CYC(b_+234, b_+236); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+236, b_+238); goto gameOver;
  }
  CYC(b_+236, b_+238);
  CYC(b_+238, b_+239); H = D;
  CYC(b_+239, b_+241); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0x01);
  CYC(b_+243, b_+245); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+245, b_+247); mem_wr(gb, HL, 0x14);
  CYC(b_+247, b_+248); ret_effect(gb);
  return;

gameOver:
  CYC(b_+248, b_+250); A = 0x01;
  CYC(b_+250, b_+253); mem_wr(gb, wTmpcfc0_shootingGallery_gameStatus, A);
  CYC(b_+253, b_+254); alu_xor(gb, A);
  CYC(b_+254, b_+257); mem_wr(gb, wTmpcfc0_shootingGallery_disableGoronNpcs, A);
  CYC(b_+257, b_+260); TAIL(interactionDelete);
}

// Fill wShootingGalleryTileLayoutsToShow with 0..B-1 and set remainingRounds = B.
// Also used by the goron dance minigame.
void shootingGallery_initializeGameRounds_hook(GB *gb) {
  BASE(shootingGallery_initializeGameRounds);
  CYC(b_+0, b_+3); SET_HL(wShootingGalleryTileLayoutsToShow);
  CYC(b_+3, b_+4); alu_xor(gb, A);
fill:
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9); goto fill;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); mem_wr(gb, wTmpcfc0_shootingGallery_remainingRounds, A);
  CYC(b_+12, b_+13); ret_effect(gb);
}

// Randomly pick the next layout from the remaining-rounds buffer, then shift the buffer
// down by one so the chosen value is removed.
void shootingGallery_getNextTargetLayout_hook(GB *gb) {
  BASE(shootingGallery_getNextTargetLayout);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_shootingGallery_remainingRounds);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, wTmpcfc0_shootingGallery_remainingRounds, A);
  // Get a random number between 0 and b-1
  CALL_C(b_+8, getRandomNumber_hook, SYM(getRandomNumber), b_+11);
modulo:
  CYC(b_+11, b_+12); alu_sub(gb, B);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14); goto modulo;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_add(gb, B);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+19); SET_HL(wShootingGalleryTileLayoutsToShow);
  CYC(b_+19, b_+20); push_effect(gb, b_+20); shootingGallery_addAToHl(gb);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+24); mem_wr(gb, wTmpcfc0_shootingGallery_targetLayoutIndex, A);
  CYC(b_+24, b_+25); push_effect(gb, DE);
  CYC(b_+25, b_+26); D = C;
  CYC(b_+26, b_+27); E = B;
  CYC(b_+27, b_+28); E = alu_dec8(gb, E);
  CYC(b_+28, b_+29); B = H;
  CYC(b_+29, b_+30); C = L;
shift:
  CYC(b_+30, b_+31); A = D;
  CYC(b_+31, b_+32); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(b_+32, b_+34); goto shiftDone;
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); SET_BC(BC + 1);
  CYC(b_+35, b_+36); A = mem_rd(gb, BC);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+37, b_+38); D = alu_inc8(gb, D);
  CYC(b_+38, b_+40); goto shift;
shiftDone:
  CYC(b_+40, b_+41); SET_DE(pop_effect(gb));
  CYC(b_+41, b_+42); ret_effect(gb);
}

// Replace all of this gallery's target tiles with standard floor. Called from bank $15
// via interBankCall.
void shootingGallery_removeAllTargets_b08_hook(GB *gb) {
  BASE(shootingGallery_removeAllTargets_b08);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wTmpcfc0_shootingGallery_useTileIndexData, A);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_sub(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+10, b_+12); goto subid1;
  }
  CYC(b_+10, b_+12);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14); goto subid2;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); SET_BC(shootingGallery_targetPositions_lynna_bank08);
  CYC(b_+17, b_+19); shootingGallery_setTiles_hook(gb);
  return;
subid1:
  CYC(b_+19, b_+22); SET_BC(shootingGallery_targetPositions_goron_bank08);
  CYC(b_+22, b_+24); shootingGallery_setTiles_hook(gb);
  return;
subid2:
  CYC(b_+24, b_+27); SET_BC(shootingGallery_targetPositions_biggoron_bank08);
  CYC(b_+27, b_+29); TAIL(shootingGallery_setTiles);
}

// Choose one of the 10 target layouts (never the same one twice) and load its tiles.
// The biggoron branch falls through into shootingGallery_setTiles.
void shootingGallery_setRandomTargetLayout_hook(GB *gb) {
  BASE(shootingGallery_setRandomTargetLayout);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0_shootingGallery_useTileIndexData, A);
  CALL_C(b_+4, shootingGallery_getNextTargetLayout_hook, SYM(shootingGallery_getNextTargetLayout), b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wTmpcfc0_shootingGallery_targetLayoutIndex);
  // l = a*5
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+12); alu_add(gb, A);
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+14); alu_add(gb, L);
  CYC(b_+14, b_+15); L = A;
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_sub(gb, 0x01);
  CYC(b_+20, b_+21); A = L;
  if (F & FZ) {
    CYCT(b_+21, b_+23); goto goronGallery;
  }
  CYC(b_+21, b_+23);
  if (!(F & FC)) {
    CYCT(b_+23, b_+25); goto biggoronGallery;
  }
  CYC(b_+23, b_+25);
  // @lynnaGallery
  CYC(b_+25, b_+28); SET_HL(shootingGallery_targetTiles_lynna_bank08);
  CYC(b_+28, b_+29); shootingGallery_addDoubleIndex(gb, b_+29);
  CYC(b_+29, b_+32); SET_BC(shootingGallery_targetPositions_lynna_bank08);
  CYC(b_+32, b_+34); shootingGallery_setTiles_hook(gb);
  return;
goronGallery:
  CYC(b_+34, b_+37); SET_HL(shootingGallery_targetTiles_goron_bank08);
  CYC(b_+37, b_+38); shootingGallery_addDoubleIndex(gb, b_+38);
  CYC(b_+38, b_+41); SET_BC(shootingGallery_targetPositions_goron_bank08);
  CYC(b_+41, b_+43); shootingGallery_setTiles_hook(gb);
  return;
biggoronGallery:
  CYC(b_+43, b_+46); SET_HL(shootingGallery_targetTiles_biggoron_bank08);
  CYC(b_+46, b_+47); shootingGallery_addDoubleIndex(gb, b_+47);
  CYC(b_+47, b_+50); SET_BC(shootingGallery_targetPositions_biggoron_bank08);
  TAIL(shootingGallery_setTiles);
}

// Replace the 10 tiles at positions BC with the tile indices at HL, or with
// TILEINDEX_STANDARD_FLOOR when useTileIndexData is nonzero.
void shootingGallery_setTiles_hook(GB *gb) {
  BASE(shootingGallery_setTiles);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0a;
nextTile:
  CYC(b_+2, b_+4); H8(hFF92) = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, BC);
  CYC(b_+5, b_+6); SET_BC(BC + 1);
  CYC(b_+6, b_+7); push_effect(gb, BC);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); A = mem_rd(gb, wTmpcfc0_shootingGallery_useTileIndexData);
  CYC(b_+11, b_+12); alu_or(gb, A);
  CYC(b_+12, b_+14); A = TILEINDEX_STANDARD_FLOOR;
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16); goto haveTile;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
haveTile:
  CYC(b_+17, b_+18); push_effect(gb, HL);
  CALL_C(b_+18, setTile_hook, SYM(setTile), b_+21);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+23); SET_BC(pop_effect(gb));
  CYC(b_+23, b_+25); A = H8(hFF92);
  CYC(b_+25, b_+26); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+26, b_+28); goto nextTile;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); ret_effect(gb);
}

// Spawn an INTERAC_PUFF at each of this gallery's 10 target positions.
void shootingGallery_createPuffAtEachTargetPosition_hook(GB *gb) {
  BASE(shootingGallery_createPuffAtEachTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+5, b_+7); goto subid1;
  }
  CYC(b_+5, b_+7);
  if (!(F & FC)) {
    CYCT(b_+7, b_+9); goto subid2;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_BC(shootingGallery_targetPositions_lynna_bank08);
  CYC(b_+12, b_+14); goto start;
subid1:
  CYC(b_+14, b_+17); SET_BC(shootingGallery_targetPositions_goron_bank08);
  CYC(b_+17, b_+19); goto start;
subid2:
  CYC(b_+19, b_+22); SET_BC(shootingGallery_targetPositions_biggoron_bank08);
start:
  CYC(b_+22, b_+24); A = 0x0a;
nextTile:
  CYC(b_+24, b_+26); H8(hFF92) = A;
  CALL_C(b_+26, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+29);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+30); ret_effect(gb); return;
  }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, HL, INTERAC_PUFF);
  CYC(b_+32, b_+33); A = mem_rd(gb, BC);
  CYC(b_+33, b_+34); SET_BC(BC + 1);
  CYC(b_+34, b_+35); push_effect(gb, BC);
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+37, setShortPosition_hook, SYM(setShortPosition), b_+40);
  CYC(b_+40, b_+41); SET_BC(pop_effect(gb));
  CYC(b_+41, b_+43); A = H8(hFF92);
  CYC(b_+43, b_+44); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+46); goto nextTile;
  }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); ret_effect(gb);
}

// Spawn a PART_BALL at this object's position.
void shootingGallery_createBallHere_hook(GB *gb) {
  BASE(shootingGallery_createBallHere);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, PART_BALL);
  CYC(b_+6, b_+9); TAIL(objectCopyPosition);
}

// Add (or, when bit 0 of the score entry is set, subtract) the score for hit index A.
void shootingGallery_addValueToScore_hook(GB *gb) {
  BASE(shootingGallery_addValueToScore);
  CYC(b_+0, b_+3); SET_HL(shootingGallery_scores_bank08);
  CYC(b_+3, b_+4); shootingGallery_addDoubleIndex(gb, b_+4);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+10); SET_HL(wTextNumberSubstitution);
  CYC(b_+10, b_+12); alu_bit(gb, 0, C);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14); goto subtract;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); addDecimalToHlRef_hook(gb);
  return;
subtract:
  CYC(b_+17, b_+19); C = (uint8_t)(C & ~(1 << 0));
  CYC(b_+19, b_+22); TAIL(subDecimalFromHlRef);
}
