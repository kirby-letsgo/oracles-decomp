#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscellaneous1.s, bank $09.

// State 0 remembers the position of the hidden stairs tile (index 4) under the object; state 1
// waits until that tile is gone (the secret was told) and then opens the entrance.
void s_tickTockSecretEntrance_hook(GB *gb) {
  BASE(tickTockSecretEntrance);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+10); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); A = L;
  CYC(b_+12, b_+15); mem_wr(gb, wccaa, A);
  CYC(b_+15, b_+17); E = 0x44; // Interaction.state
  CYC(b_+17, b_+19); A = 0x01;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;

state1:
  CALL_C(b_+21, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+24);
  CALL_C(b_+24, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+27);
  CYC(b_+27, b_+29); alu_cp(gb, 0x04);
  if (F & FZ) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  TAIL_S(setEnteredWarpSetStairsPlaySolvedSound);
}

void s_staticHeartPiece_hook(GB *gb) {
  BASE(staticHeartPiece);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x2b00); // TREASURE_HEART_PIECE, subid 0
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

static uint16_t misc1_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// The floodgate keeper: scripted, blocks Link, animates.
void s_floodgateKeeper_hook(GB *gb) {
  BASE(floodgateKeeper);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+14); SET_HL(SYM(floodgateKeeperScript));
  CALL_C(b_+14, interactionSetScript_hook, SYM(interactionSetScript), b_+17);
  CALL_C(b_+17, objectSetVisible82_hook, SYM(objectSetVisible82), b_+20);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+24); mem_wr(gb, wTmpcfc0 + 0x01, A);
state1:
  CALL_C(b_+24, interactionAnimate_hook, SYM(interactionAnimate), b_+27);
  CALL_C(b_+27, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+30);
  CYC(b_+30, b_+33);
  TAIL(interactionRunScript);
}

// The floodgate switch: already on (room flag bit 6) it is just the tile; otherwise an
// invisible scripted object until the keeper's script is done.
void s_floodgateKeeperSwitchScript_hook(GB *gb) {
  BASE(floodgateKeeperSwitchScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+13, b_+15); goto init; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_BC(wRoomLayout + 0x68);
  CYC(b_+18, b_+20); A = 0x0b; // TILEINDEX_DUNGEON_SWITCH_ON
  CYC(b_+20, b_+21); mem_wr(gb, BC, A);
  CYC(b_+21, b_+24);
  TAIL(interactionDelete);
init:
  CALL_C(b_+24, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+27);
  CALL_C(b_+27, objectSetVisible83_hook, SYM(objectSetVisible83), b_+30);
  CALL_C(b_+30, objectSetInvisible_hook, SYM(objectSetInvisible), b_+33);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37); mem_wr(gb, wSwitchState, A);
  CYC(b_+37, b_+40); SET_HL(SYM(floodgateSwitchScript));
  CYC(b_+40, b_+43);
  TAIL(interactionSetScript);
state1:
  CALL_C(b_+43, interactionAnimate_hook, SYM(interactionAnimate), b_+46);
  TAIL_S(runScriptDeleteWhenDone);
}

void s_runScriptDeleteWhenDone_hook(GB *gb) {
  BASE(runScriptDeleteWhenDone);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7);
  TAIL(interactionDelete);
}

// resetMusicThenSolvePuzzleSound from its +5: re-enable objects and the menu, play the
// stop-music command and the solved-puzzle sound, delete.
static void misc1_solved_sounds(GB *gb) {
  BASE(resetMusicThenSolvePuzzleSound);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+9, b_+12); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+12, b_+14); A = 0xf1;
  CALL_C(b_+14, playSound_b00_hook, SYM(playSound_b00), b_+17);
  CYC(b_+17, b_+19); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+19, playSound_b00_hook, SYM(playSound_b00), b_+22);
  CYC(b_+22, b_+25);
  TAIL(interactionDelete);
}

// The floodgate keyhole: state 1 runs the key-entered script, state 2 shakes the screen until
// the script sets wTmpcfc0 bit 7, then restores the room's music.
void s_floodgateKeyhole_hook(GB *gb) {
  BASE(floodgateKeyhole);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), interactionRunScript_hook)) { interactionRunScript_hook(gb); return; }
    else if (jt_ == b_+27) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+16);
  CYC(b_+16, b_+18); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); SET_HL(SYM(floodgateKeyholeScript_keyEntered));
  CYC(b_+24, b_+27);
  TAIL(interactionSetScript);

state2:
  CYC(b_+27, b_+29); A = 0x04;
  CALL_C(b_+29, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+32);
  CYC(b_+32, b_+35); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+35, b_+37); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+41, b_+44); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
  CYCT(b_+47, b_+49); misc1_solved_sounds(gb); return;
}

void s_resetMusicThenSolvePuzzleSound_hook(GB *gb) {
  BASE(resetMusicThenSolvePuzzleSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wActiveMusic, A);
  misc1_solved_sounds(gb); return;
}

// d4KeyHole@func_621c, also used by natzuSwitch: counts counter1 down; Z when it is 0.
static void misc1_count_down(GB *gb) {
  BASE(d4KeyHole);
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = 0x46; // Interaction.counter1
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+67); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+67); return;
}

// The D4 keyhole: the screen shakes, the waterfall above D4 empties (simple script), the
// entrance opens (room flag bit 7), the waterfall at D4 empties, then Link warps in.
void s_d4KeyHole_hook(GB *gb) {
  BASE(d4KeyHole);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+39) { goto state1; }
    else if (jt_ == b_+68) { goto state2; }
    else if (jt_ == b_+95) { goto state3; }
    else if (jt_ == b_+115) { goto state4; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+20);
  CYC(b_+20, b_+22); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+22, b_+25); TAIL(interactionDelete); }
  CYC(b_+22, b_+25);
  CALL_C(b_+25, objectSetReservedBit1_hook, SYM(objectSetReservedBit1), b_+28);
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+33); mem_wr(gb, wScreenShakeMagnitude, A);
  CYC(b_+33, b_+36); SET_HL(SYM(d4KeyholeScript_disableThingsAndScreenShake));
  CYC(b_+36, b_+39);
  TAIL(interactionSetScript);

state1:
  CYC(b_+39, b_+42); A = mem_rd(gb, wActiveRoom);
  CYC(b_+42, b_+44); alu_cp(gb, 0x0d); // ROOM_SEASONS_00d
  if (!(F & FZ)) { CYCT(b_+44, b_+47); TAIL(interactionDelete); }
  CYC(b_+44, b_+47);
  CALL_C(b_+47, interactionRunScript_hook, SYM(interactionRunScript), b_+50);
  if (!(F & FC)) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CALL_C(b_+51, interactionIncState_hook, SYM(interactionIncState), b_+54);
  CYC(b_+54, b_+57); SET_HL(SYM(simpleScript_waterfallEmptyingAboveD4));
  CYC(b_+57, b_+60);
  TAIL(interactionSetSimpleScript);

state2:
  CALL_L(b_+68, misc1_count_down, b_+71);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CALL_C(b_+72, interactionRunSimpleScript_hook, SYM(interactionRunSimpleScript), b_+75);
  if (!(F & FC)) { RET_TAKEN(b_+75); return; }
  CYC(b_+75, b_+76);
  CALL_C(b_+76, interactionIncState_hook, SYM(interactionIncState), b_+79);
  CYC(b_+79, b_+81); A = 0x1d;
  CYC(b_+81, b_+83); B = 0x02;
  CALL_C(b_+83, func_1383_hook, SYM(func_1383), b_+86);
  CYC(b_+86, b_+89); SET_HL(SYM(d4KeyHolw_disableAllSorts));
  CYC(b_+89, b_+91); E = 0x15;
  CALL_C(b_+91, interBankCall_hook, SYM(interBankCall), b_+94);
  RET(b_+94); return;

state3:
  CYC(b_+95, b_+98); A = mem_rd(gb, wScrollMode);
  CYC(b_+98, b_+100); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+100); return; }
  CYC(b_+100, b_+101);
  CALL_C(b_+101, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+104);
  CYC(b_+104, b_+106); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+106, interactionIncState_hook, SYM(interactionIncState), b_+109);
  CYC(b_+109, b_+112); SET_HL(SYM(simpleScript_waterfallEmptyingAtD4));
  CYC(b_+112, b_+115);
  TAIL(interactionSetSimpleScript);

state4:
  CYC(b_+115, b_+117); A = 0x3c;
  CALL_C(b_+117, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+120);
  CALL_L(b_+120, misc1_count_down, b_+123);
  if (!(F & FZ)) { RET_TAKEN(b_+123); return; }
  CYC(b_+123, b_+124);
  CALL_C(b_+124, interactionRunSimpleScript_hook, SYM(interactionRunSimpleScript), b_+127);
  if (!(F & FC)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+131); SET_HL(b_+137); // @warpDestVariables
  CALL_C(b_+131, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+134);
  CYC(b_+134, b_+137);
  TAIL_S(resetMusicThenSolvePuzzleSound);
}

// The floodgate key: spawned once the keeper has been talked to (room flags $60 == $40);
// picking it up starts the keeper-notices-key script.
void s_floodgateKey_hook(GB *gb) {
  BASE(floodgateKey);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+27) { goto state1; }
    else if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), interactionRunScript_hook)) { interactionRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x60);
  CYC(b_+15, b_+17); alu_cp(gb, 0x40);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+21); SET_BC(0x4300); // TREASURE_FLOODGATE_KEY
  CALL_C(b_+21, s_misc1_spawnTreasureBC_hook, SYM(misc1_spawnTreasureBC), b_+24);
  CYC(b_+24, b_+27);
  TAIL(interactionIncState);

state1:
  CYC(b_+27, b_+29); A = 0x43; // TREASURE_FLOODGATE_KEY
  CALL_C(b_+29, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+32);
  if (!(F & FC)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CALL_C(b_+33, interactionIncState_hook, SYM(interactionIncState), b_+36);
  CYC(b_+36, b_+39); SET_HL(wDisabledObjects);
  CYC(b_+39, b_+41); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+41, b_+43); A = 0x01;
  CYC(b_+43, b_+46); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+46, b_+49); SET_HL(SYM(floodgateKeyScript_keeperNoticesKey));
  CYC(b_+49, b_+52);
  TAIL(interactionSetScript);
}

void s_dragonKey_hook(GB *gb) {
  BASE(dragonKey);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x4400); // TREASURE_DRAGON_KEY
  CYC(b_+3, b_+6);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

// The Tarm Ruins armos that reveals the stairs (tile 4) once its script has run.
void s_tarmArmosUnlockingStairs_hook(GB *gb) {
  BASE(tarmArmosUnlockingStairs);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+27) { goto state1; }
    else if (jt_ == SYM(runScriptDeleteWhenDone) && hook_is(gb, SYM(runScriptDeleteWhenDone), s_runScriptDeleteWhenDone_hook)) { s_runScriptDeleteWhenDone_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, interactionIncState_hook, SYM(interactionIncState), b_+21);
  CYC(b_+21, b_+24); SET_HL(SYM(tarmArmosUnlockingStairsScript));
  CYC(b_+24, b_+27);
  TAIL(interactionSetScript);

state1:
  CALL_C(b_+27, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+30);
  CYC(b_+30, b_+32); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CALL_C(b_+33, interactionIncState_hook, SYM(interactionIncState), b_+36);
  CYC(b_+36, b_+39);
  TAIL_S(runScriptDeleteWhenDone);
}

// tarmEscapedLostWoods from its +10: the solved-puzzle sound, then delete.
static void misc1_solved_delete(GB *gb) {
  BASE(tarmEscapedLostWoods);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+10, b_+12); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+18);
  TAIL(interactionDelete);
}

// The Tarm Ruins wall by the stump (room $42, winter only): once the stump tile ($9c) at xh is
// gone the wall (two tiles) opens when the palette fade ends and the room flag bit yh is set.
void s_tarmArmosWallByStump_hook(GB *gb) {
  BASE(tarmArmosWallByStump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveRoom);
  CYC(b_+3, b_+5); alu_cp(gb, 0x42);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); E = 0x44; // Interaction.state
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+18) { goto state0; }
    else if (jt_ == b_+42) { goto state1; }
    else if (jt_ == b_+63) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+24); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+24, b_+26); alu_cp(gb, 0x03); // SEASON_WINTER
  if (!(F & FZ)) { CYCT(b_+26, b_+29); TAIL(interactionDelete); }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+32);
  CYC(b_+32, b_+34); E = 0x4b; // Interaction.yh
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+36, b_+39); TAIL(interactionDelete); }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+42);
  TAIL(objectSetReservedBit1);

state1:
  CYC(b_+42, b_+45); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+45, b_+47); alu_cp(gb, 0x03); // SEASON_WINTER
  if (!(F & FZ)) { CYCT(b_+47, b_+50); TAIL(interactionDelete); }
  CYC(b_+47, b_+50);
  CYC(b_+50, b_+52); E = 0x4d; // Interaction.xh
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); L = A;
  CYC(b_+54, b_+56); H = 0xcf; // wRoomLayout
  CYC(b_+56, b_+58); A = 0x9c;
  CYC(b_+58, b_+59); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+63);
  TAIL(interactionIncState);

state2:
  CYC(b_+63, b_+66); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+66, b_+68); alu_cp(gb, 0x03); // SEASON_WINTER
  if (F & FZ) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+72); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+73); return; }
  CYC(b_+73, b_+74);
  CALL_C(b_+74, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+77);
  CYC(b_+77, b_+79); E = 0x4b; // Interaction.yh
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  CYC(b_+80, b_+81); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+81, b_+82); mem_wr(gb, HL, A);
  CYC(b_+82, b_+84); E = 0x4d; // Interaction.xh
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); A = alu_dec8(gb, A);
  CYC(b_+86, b_+87); C = A;
  CYC(b_+87, b_+89); A = 0x09;
  CALL_C(b_+89, setTile_hook, SYM(setTile), b_+92);
  CYC(b_+92, b_+93); C = alu_inc8(gb, C);
  CYC(b_+93, b_+95); A = 0xbc;
  CALL_C(b_+95, setTile_hook, SYM(setTile), b_+98);
  CYCT(b_+98, b_+100); misc1_solved_delete(gb); return;
}

// Leaving the Lost Woods by the right path: the solved sound, unless it was a screen transition.
void s_tarmEscapedLostWoods_hook(GB *gb) {
  BASE(tarmEscapedLostWoods);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); TAIL(interactionDelete); }
  CYC(b_+7, b_+10);
  misc1_solved_delete(gb); return;
}

// An ore chunk dig spot: remembers its short position in wccaa; when the dig routine clears
// it to $ff, drops an ore chunk (item drop $0e) thrown in Link's direction.
void s_oreChunkDigSpot_hook(GB *gb) {
  BASE(oreChunkDigSpot);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); E = 0x43; // Interaction.var03
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto remember; }
  CYC(b_+12, b_+14);
  CALL_C(b_+14, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); TAIL(interactionDelete); }
  CYC(b_+19, b_+22);
remember:
  CALL_C(b_+22, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+25);
  CYC(b_+25, b_+28); mem_wr(gb, wccaa, A);
state1:
  CYC(b_+28, b_+31); A = mem_rd(gb, wccaa);
  CYC(b_+31, b_+32); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CALL_C(b_+33, getFreePartSlot_hook, SYM(getFreePartSlot), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x0e);
  CYC(b_+42, b_+43); L = alu_inc8(gb, L);
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x01);
  CYC(b_+45, b_+48); A = mem_rd(gb, w1Link_direction);
  CYC(b_+48, b_+50); A = alu_swap(gb, A);
  CYC(b_+50, b_+51); alu_rrca(gb);
  CYC(b_+51, b_+53); L = 0xc9; // Part.angle
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  CALL_C(b_+54, objectCopyPosition_hook, SYM(objectCopyPosition), b_+57);
  CYC(b_+57, b_+60);
  TAIL(interactionDelete);
}

void s_misc1_spawnTreasureBCifRoomFlagBit5NotSet_hook(GB *gb) {
  BASE(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto done; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_misc1_spawnTreasureBC_hook, SYM(misc1_spawnTreasureBC), b_+10);
done:
  CYC(b_+10, b_+13);
  TAIL(interactionDelete);
}

// A treasure b (subid c) at the object's position.
void s_misc1_spawnTreasureBC_hook(GB *gb) {
  BASE(misc1_spawnTreasureBC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, C);
  CYC(b_+10, b_+13);
  TAIL(objectCopyPosition);
}

// Rocks and ember trees that stay removed: remembers the tile at yh, and when it changes
// (after scrolling in) sets the room flag bits in xh.
void s_permanentlyRemovableObjects_hook(GB *gb) {
  BASE(permanentlyRemovableObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+8);
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); E = 0x4d; // Interaction.xh
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); B = 0xcf; // wRoomLayout
  CYC(b_+20, b_+22); E = 0x4b; // Interaction.yh
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);
  CYC(b_+25, b_+27); E = 0x43; // Interaction.var03
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = 0x44; // Interaction.state
  CYC(b_+30, b_+32); A = 0x01;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
state1:
  CYC(b_+33, b_+36); A = mem_rd(gb, wScrollMode);
  CYC(b_+36, b_+38); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+38, b_+41); TAIL(interactionDelete); }
  CYC(b_+38, b_+41);
  CYC(b_+41, b_+43); E = 0x43; // Interaction.var03
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); B = A;
  CYC(b_+45, b_+47); E = 0x4b; // Interaction.yh
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); L = A;
  CYC(b_+49, b_+51); H = 0xcf; // wRoomLayout
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+53); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+57);
  CYC(b_+57, b_+59); E = 0x4d; // Interaction.xh
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+61, b_+62); mem_wr(gb, HL, A);
  CYC(b_+62, b_+65);
  TAIL(interactionDelete);
}

// The pirates' bell room: once the room flag is set (the floor fell), runs the dropping-in
// script with objects and the menu disabled.
void s_piratesBellRoomWhenFallingIn_hook(GB *gb) {
  BASE(piratesBellRoomWhenFallingIn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+21) { goto state1; }
    else if (jt_ == SYM(runScriptDeleteWhenDone) && hook_is(gb, SYM(runScriptDeleteWhenDone), s_runScriptDeleteWhenDone_hook)) { s_runScriptDeleteWhenDone_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, interactionIncState_hook, SYM(interactionIncState), b_+21);
state1:
  CALL_C(b_+21, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+30); SET_HL(wDisabledObjects);
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+37); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+37, interactionIncState_hook, SYM(interactionIncState), b_+40);
  CYC(b_+40, b_+43); SET_HL(SYM(piratesBellRoomDroppingInScript));
  CYC(b_+43, b_+46);
  TAIL(interactionSetScript);
}

// The green joy ring appears when a trigger is pressed.
void s_greenJoyRing_hook(GB *gb) {
  BASE(greenJoyRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x2701); // GREEN_JOY_RING, subid 1
  TAIL_S(createRingTreasureAtPosition);
}

void s_createRingTreasureAtPosition_hook(GB *gb) {
  BASE(createRingTreasureAtPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, createRingTreasure_hook, SYM(createRingTreasure), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, objectCopyPosition_hook, SYM(objectCopyPosition), b_+7);
  CYC(b_+7, b_+10);
  TAIL(interactionDelete);
}

// masterDiverPuzzle@checkLinkSwordSpin: Z when Link is spin-attacking (wcc63 low nibble 2).
static void misc1_check_sword_spin(GB *gb) {
  BASE(masterDiverPuzzle);
  CYC(b_+92, b_+95); A = mem_rd(gb, wcc63);
  CYC(b_+95, b_+97); alu_and(gb, 0x0f);
  CYC(b_+97, b_+99); alu_cp(gb, 0x02);
  RET(b_+99); return;
}

// The master diver's puzzle: while spin-attacking, every statue ($2a/$2b) hit at a new position
// (wccb0) is recorded in var30..; the third one solves it (the solved script), a hit after the
// spin ends resets it.
void s_masterDiverPuzzle_hook(GB *gb) {
  BASE(masterDiverPuzzle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+32) { goto state1; }
    else if (jt_ == b_+39) { goto state2; }
    else if (jt_ == b_+85) { goto state3; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+12, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+15);
  CYC(b_+15, b_+17); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+17, b_+20); TAIL(interactionDelete); }
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = 0x44; // Interaction.state
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x01);
  CYC(b_+25, b_+27); L = 0x70; // Interaction.var30
  CYC(b_+27, b_+29); B = 0x06;
  CYC(b_+29, b_+32);
  TAIL(clearMemory);

state1:
  CALL_L(b_+32, misc1_check_sword_spin, b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); A = 0x02;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
state2:
  CALL_L(b_+39, misc1_check_sword_spin, b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto state0; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+47); A = mem_rd(gb, wccaf);
  CYC(b_+47, b_+49); alu_cp(gb, 0x2b);
  if (F & FZ) { CYCT(b_+49, b_+51); goto statue; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); alu_cp(gb, 0x2a); // TILEINDEX_PUSHABLE_STATUE
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
statue:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = 0x70; // Interaction.var30
  CYC(b_+57, b_+60); A = mem_rd(gb, wccb0);
  CYC(b_+60, b_+61); C = A;
  for (;;) {
    CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+62, b_+63); alu_cp(gb, C);
    if (F & FZ) { RET_TAKEN(b_+63); return; }
    CYC(b_+63, b_+64);
    CYC(b_+64, b_+65); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+65, b_+67); continue; }
    CYC(b_+65, b_+67);
    break;
  }
  CYC(b_+67, b_+68); L = alu_dec8(gb, L);
  CYC(b_+68, b_+69); mem_wr(gb, HL, C);
  CYC(b_+69, b_+70); A = L;
  CYC(b_+70, b_+72); alu_cp(gb, 0x73);
  if (!(F & FC)) { CYCT(b_+72, b_+74); goto solved; }
  CYC(b_+72, b_+74);
  RET(b_+74); return;
solved:
  CYC(b_+75, b_+77); L = 0x44; // Interaction.state
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x03);
  CYC(b_+79, b_+82); SET_HL(SYM(masterDiverPuzzleScript_solved));
  CALL_C(b_+82, interactionSetScript_hook, SYM(interactionSetScript), b_+85);
state3:
  CALL_C(b_+85, interactionRunScript_hook, SYM(interactionRunScript), b_+88);
  if (F & FC) { CYCT(b_+88, b_+91); TAIL(interactionDelete); }
  CYC(b_+88, b_+91);
  RET(b_+91); return;
}

void s_piratesBell_hook(GB *gb) {
  BASE(piratesBell);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x4a00); // TREASURE_PIRATES_BELL
  CYC(b_+3, b_+6);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

// The armos blocking the flower path to D6: when its tile ($d6) is gone, the room flag records
// which way it was pushed (bit 7 left, bit 6 otherwise).
void s_armosBlockingFlowerPathToD6_hook(GB *gb) {
  BASE(armosBlockingFlowerPathToD6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  CALL_C(b_+3, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+8, b_+11); TAIL(interactionDelete); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CALL_C(b_+16, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0xd6);
  if (F & FZ) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+25); A = mem_rd(gb, wBlockPushAngle);
  CYC(b_+25, b_+27); alu_and(gb, 0x7f);
  CYC(b_+27, b_+29); alu_cp(gb, 0x18); // ANGLE_LEFT
  CYC(b_+29, b_+31); B = 0x80;
  if (F & FZ) { CYCT(b_+31, b_+33); goto flag; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); B = 0x40;
flag:
  CALL_C(b_+35, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+38);
  CYC(b_+38, b_+39); alu_or(gb, B);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43);
  TAIL(interactionDelete);
}

// The Natzu switch (not with Dimitri): a switch part; pressing it builds the bridge (simple
// script) with everything disabled, and sets room flag bit 6.
void s_natzuSwitch_hook(GB *gb) {
  BASE(natzuSwitch);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44; // Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc1_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+41) { goto state1; }
    else if (jt_ == b_+71) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+16, b_+18); alu_cp(gb, 0x0c); // SPECIALOBJECT_DIMITRI
  if (F & FZ) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CALL_C(b_+21, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); TAIL(interactionDelete); }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, getFreePartSlot_hook, SYM(getFreePartSlot), b_+32);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x05); // PART_SWITCH
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x01);
  CYC(b_+38, b_+41);
  TAIL(objectCopyPosition);

state1:
  CYC(b_+41, b_+44); A = mem_rd(gb, wSwitchState);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); A = 0x81;
  CYC(b_+48, b_+51); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+51, b_+54); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+54, b_+57); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(b_+57, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+60);
  CYC(b_+60, b_+62); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CALL_C(b_+62, interactionIncState_hook, SYM(interactionIncState), b_+65);
  CYC(b_+65, b_+68); SET_HL(SYM(simpleScript_creatingBridgeToNatzu));
  CYC(b_+68, b_+71);
  TAIL(interactionSetSimpleScript);

state2:
  CALL_L(b_+71, misc1_count_down, b_+74);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; }
  CYC(b_+74, b_+75);
  CALL_C(b_+75, interactionRunSimpleScript_hook, SYM(interactionRunSimpleScript), b_+78);
  if (!(F & FC)) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+80); alu_xor(gb, A);
  CYC(b_+80, b_+83); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+83, b_+86); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+86, b_+89); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+89, b_+92);
  TAIL(interactionDelete);
}

// Forces the Onox castle cutscene the first time Link scrolls in (before the witches and the
// kidnapping have been seen).
void s_onoxCastleCutscene_hook(GB *gb) {
  BASE(onoxCastleCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x1d; // GLOBALFLAG_WITCHES_2_SEEN
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); A = 0x1f; // GLOBALFLAG_ZELDA_KIDNAPPED_SEEN
  CALL_C(b_+10, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+21); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+21, b_+24); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+24, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+27);
  CYC(b_+27, b_+29); A = 0x14; // CUTSCENE_S_ONOX_CASTLE_FORCE
  CYC(b_+29, b_+32); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+36); mem_wr(gb, w1Link_direction, A);
  CALL_C(b_+36, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+39);
  CALL_C(b_+39, clearAllParentItems_hook, SYM(clearAllParentItems), b_+42);
  CYC(b_+42, b_+45);
  TAIL(interactionDelete);
}

// While Impa's request to save Zelda is open, keeps enemies from spawning (wcc85 bit 7).
void s_savingZeldaNoEnemiesHandler_hook(GB *gb) {
  BASE(savingZeldaNoEnemiesHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x22; // GLOBALFLAG_IMPA_ASKED_TO_SAVE_ZELDA
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x23; // GLOBALFLAG_ZELDA_SAVED_FROM_VIRE
  CALL_C(b_+8, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x80;
  CYC(b_+14, b_+17); mem_wr(gb, wcc85, A);
  CYC(b_+17, b_+20);
  TAIL(interactionDelete);
}

// After counter1 runs out, the D3 dam unblocking simple script; sets wTmpcfc0 bit 7 when done.
void s_unblockingD3Dam_hook(GB *gb) {
  BASE(unblockingD3Dam);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x46; // Interaction.counter1
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto run; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
run:
  CALL_C(b_+9, checkInteractionState_hook, SYM(checkInteractionState), b_+12);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto state1; }
  CYC(b_+12, b_+14);
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
  CYC(b_+17, b_+20); SET_HL(SYM(simpleScript_unblockingD3Dam));
  CYC(b_+20, b_+23);
  TAIL(interactionSetSimpleScript);
state1:
  CALL_C(b_+23, interactionRunSimpleScript_hook, SYM(interactionRunSimpleScript), b_+26);
  if (!(F & FC)) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+30); SET_HL(wTmpcfc0);
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+32, b_+35);
  TAIL(interactionDelete);
}

void s_replacePirateShipWithQuicksand_hook(GB *gb) {
  BASE(replacePirateShipWithQuicksand);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x17; // GLOBALFLAG_PIRATE_SHIP_DOCKED
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); B = 0x5e; // INTERAC_QUICKSAND
  CALL_C(b_+10, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+13);
  CYC(b_+13, b_+16);
  TAIL(interactionDelete);
}

// Keeps screen transitions disabled until the tile under the object (the stolen feather's
// spot) changes.
void s_stolenFeatherGottenHandler_hook(GB *gb) {
  BASE(stolenFeatherGottenHandler);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = 0x49; // Interaction.angle
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); L = 0x44; // Interaction.state
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state1:
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(b_+20, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+23);
  CYC(b_+23, b_+25); E = 0x49; // Interaction.angle
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  CYC(b_+29, b_+32); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+32, b_+35);
  TAIL(interactionDelete);
}

// @spawnBridge: a bridge spawner part with var07 e at y b, x c.
static void misc1_spawn_bridge(GB *gb) {
  BASE(horonVillagePortalBridgeSpawner);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+54, getFreePartSlot_hook, SYM(getFreePartSlot), b_+57);
  if (!(F & FZ)) { RET_TAKEN(b_+57); return; }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x0c); // PART_BRIDGE_SPAWNER
  CYC(b_+60, b_+62); L = 0xc7; // Part.var07
  CYC(b_+62, b_+63); mem_wr(gb, HL, E);
  CYC(b_+63, b_+65); L = 0xc9; // Part.angle
  CYC(b_+65, b_+66); mem_wr(gb, HL, B);
  CYC(b_+66, b_+68); L = 0xcb; // Part.yh
  CYC(b_+68, b_+69); mem_wr(gb, HL, C);
  RET(b_+69); return;
}

// The Horon Village portal switch: pressing it spawns the two bridge halves and sets room flag
// bit 6.
void s_horonVillagePortalBridgeSpawner_hook(GB *gb) {
  BASE(horonVillagePortalBridgeSpawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wSwitchState, A);
  CALL_C(b_+9, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); TAIL(interactionDelete); }
  CYC(b_+14, b_+17);
  CALL_C(b_+17, interactionIncState_hook, SYM(interactionIncState), b_+20);
state1:
  CYC(b_+20, b_+23); A = mem_rd(gb, wSwitchState);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CALL_C(b_+25, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+30, b_+32); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+32, playSound_b00_hook, SYM(playSound_b00), b_+35);
  CYC(b_+35, b_+38); SET_BC(0x0047);
  CYC(b_+38, b_+40); E = 0x08;
  CALL_L(b_+40, misc1_spawn_bridge, b_+43);
  CYC(b_+43, b_+46); SET_BC(0x0114);
  CYC(b_+46, b_+48); E = 0x06;
  CALL_L(b_+48, misc1_spawn_bridge, b_+51);
  CYC(b_+51, b_+54);
  TAIL(interactionDelete);
}

// A random tier-2 ring dig spot (under Vasu's sign, by the wilds ore).
void s_randomRingDigSpot_hook(GB *gb) {
  BASE(randomRingDigSpot);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); C = 0x02;
  CALL_C(b_+10, getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+13);
  CYC(b_+13, b_+14); B = C;
  CYC(b_+14, b_+16); C = 0x03;
  CYC(b_+16, b_+19);
  TAIL_S(createRingTreasureAtPosition);
}

void s_staticGashaSeed_hook(GB *gb) {
  BASE(staticGashaSeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x3404); // TREASURE_GASHA_SEED, subid 4
  CYC(b_+3, b_+6);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

void s_underwaterGashaSeed_hook(GB *gb) {
  BASE(underwaterGashaSeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x3405); // TREASURE_GASHA_SEED, subid 5
  CYC(b_+3, b_+6);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

// Records the entered position (l), replaces the tile with stairs ($e7), plays the solved sound.
void s_setEnteredWarpSetStairsPlaySolvedSound_hook(GB *gb) {
  BASE(setEnteredWarpSetStairsPlaySolvedSound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = L;
  CYC(b_+1, b_+2); A = C;
  CYC(b_+2, b_+5); mem_wr(gb, wEnteredWarpPosition, A);
  CYC(b_+5, b_+7); A = 0xe7;
  CALL_C(b_+7, setTile_hook, SYM(setTile), b_+10);
  CYC(b_+10, b_+12); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+18);
  TAIL(interactionDelete);
}

// The grave secret entrance opens once the grave tile (1) is gone.
void s_graveSecretEntrance_hook(GB *gb) {
  BASE(graveSecretEntrance);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  CALL_C(b_+3, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); A = L;
  CYC(b_+10, b_+13); mem_wr(gb, wccaa, A);
  CYCT(b_+13, b_+15);
  TAIL_S(setEnteredWarpSetStairsPlaySolvedSound);
}

// The D4 miniboss room: spawns its objects until both torches are lit (room flag bit 7).
void s_d4MinibossRoom_hook(GB *gb) {
  BASE(d4MinibossRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto torches; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+19); SET_HL(SYM(objectData7e96));
  CYC(b_+19, b_+22);
  TAIL(parseGivenObjectData_b00);
torches:
  CYC(b_+22, b_+25); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+25, b_+27); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CALL_C(b_+28, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+33, b_+36);
  TAIL(interactionDelete);
}

// After being sent back by the Onox castle barrier: a minute of disabled objects, then the
// explanation text.
void s_sentBackFromOnoxCastleBarrier_hook(GB *gb) {
  BASE(sentBackFromOnoxCastleBarrier);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x24; // GLOBALFLAG_ONOX_CASTLE_BARRIER_GONE
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); TAIL(interactionDelete); }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x24; // GLOBALFLAG_ONOX_CASTLE_BARRIER_GONE
  CALL_C(b_+15, unsetGlobalFlag_hook, SYM(unsetGlobalFlag), b_+18);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = 0x44; // Interaction.state
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+24); L = 0x46; // Interaction.counter1
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x3c);
state1:
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+31); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+31, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+34);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CYC(b_+36, b_+39); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+39, b_+42); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+42, b_+45); SET_BC(0x501b); // TX_501b
  CALL_C(b_+45, showText_hook, SYM(showText), b_+48);
  CYC(b_+48, b_+51);
  TAIL(interactionDelete);
}

void s_sidescrollingStaticGashaSeed_hook(GB *gb) {
  BASE(sidescrollingStaticGashaSeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x3404); // TREASURE_GASHA_SEED, subid 4
  CYC(b_+3, b_+6);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

void s_sidescrollingStaticSeedSatchel_hook(GB *gb) {
  BASE(sidescrollingStaticSeedSatchel);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x1900); // TREASURE_SEED_SATCHEL
  CYC(b_+3, b_+6);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

// The Mt. Cucco banana tree in spring: the spring banana treasure with a gasha-tree part
// linked to it (var06/07 = 6; the part's var17 holds the treasure's slot).
void s_mtCuccoBananaTree_hook(GB *gb) {
  BASE(mtCuccoBananaTree);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wRoomStateModifier);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); TAIL(interactionDelete); }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); TAIL(interactionDelete); }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+18); SET_BC(0x4700); // TREASURE_SPRING_BANANA
  CALL_C(b_+18, s_misc1_spawnTreasureBC_hook, SYM(misc1_spawnTreasureBC), b_+21);
  CYC(b_+21, b_+22); B = H;
  CYC(b_+22, b_+24); A = 0x06;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CALL_C(b_+26, getFreePartSlot_hook, SYM(getFreePartSlot), b_+29);
  if (!(F & FZ)) { CYCT(b_+29, b_+32); TAIL(interactionDelete); }
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x17); // PART_GASHA_TREE
  CYC(b_+34, b_+36); L = 0xd6; // Part.var16
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x40);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+40); mem_wr(gb, HL, B);
  CYC(b_+40, b_+43);
  TAIL(interactionDelete);
}

void s_hardOre_hook(GB *gb) {
  BASE(hardOre);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); SET_BC(0x5200); // TREASURE_HARD_ORE
  CYC(b_+11, b_+14);
  TAIL_S(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}

// Three triggers: pressing them in the right order (4, then $85) sets bits 7 and 6 of
// wActiveTriggers; with all three held, half a second later a tile appears with a puff (or
// the error sound if the order was wrong).
void s_interactionCode6bSubid23_hook(GB *gb) {
  BASE(interactionCode6bSubid23);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); TAIL(interactionDelete); }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); SET_HL(wActiveTriggers);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto second; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
second:
  CYC(b_+23, b_+25); alu_cp(gb, 0x85);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto all; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
all:
  CYC(b_+29, b_+31); alu_and(gb, 0x07);
  CYC(b_+31, b_+33); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); A = 0x1e;
  CYC(b_+36, b_+38); E = 0x46; // Interaction.counter1
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+42);
  TAIL(interactionIncState);
state1:
  CALL_C(b_+42, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+45);
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+49); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+49, b_+51); alu_bit(gb, 6, A);
  CYC(b_+51, b_+53); B = 0x5a; // SND_ERROR
  if (F & FZ) { CYCT(b_+53, b_+55); goto sound; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); C = 0x5c;
  CYC(b_+57, b_+59); A = 0x05;
  CALL_C(b_+59, setTile_hook, SYM(setTile), b_+62);
  CALL_C(b_+62, objectCreatePuff_hook, SYM(objectCreatePuff), b_+65);
  CALL_C(b_+65, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+70, b_+72); B = 0x4d; // SND_SOLVEPUZZLE
sound:
  CYC(b_+72, b_+73); A = B;
  CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
  CYC(b_+76, b_+79);
  TAIL(interactionDelete);
}

// Four orbs: wActiveTriggers is 1 while the toggle blocks show state $e.
void s_interactionCode6bSubid24_hook(GB *gb) {
  BASE(interactionCode6bSubid24);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x0e);
  CYC(b_+7, b_+9); A = 0x01;
  if (F & FZ) { CYCT(b_+9, b_+11); goto set; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
set:
  CYC(b_+12, b_+15); mem_wr(gb, wActiveTriggers, A);
  RET(b_+15); return;
}

// A staircase that appears a second after every enemy is defeated.
void s_interactionCode6bSubid25_hook(GB *gb) {
  BASE(interactionCode6bSubid25);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wNumEnemies);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, interactionIncState_hook, SYM(interactionIncState), b_+13);
  CYC(b_+13, b_+15); L = 0x46; // Interaction.counter1
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x3c);
state1:
  CALL_C(b_+17, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CALL_C(b_+21, objectCreatePuff_hook, SYM(objectCreatePuff), b_+24);
  CALL_C(b_+24, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+27);
  CYC(b_+27, b_+28); C = A;
  CYC(b_+28, b_+30); A = 0x44; // TILEINDEX_INDOOR_UPSTAIRCASE
  CALL_C(b_+30, setTile_hook, SYM(setTile), b_+33);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+37, b_+40);
  TAIL(interactionDelete);
}

// A Subrosian with the temple-fallen text.
void s_interactionCode6bSubid26_hook(GB *gb) {
  BASE(interactionCode6bSubid26);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); TAIL(interactionRunScript); }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); SET_HL(SYM(subrosianScript_templeFallenText));
  CALL_C(b_+9, interactionSetScript_hook, SYM(interactionSetScript), b_+12);
  CYC(b_+12, b_+15);
  TAIL(interactionIncState);
}

static uint16_t miscellaneous1_d_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// INTERAC_MISCELLANEOUS_1
void s_interactionCode6b_hook(GB *gb) {
  BASE(interactionCode6b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (miscellaneous1_d_jump_table(gb));
    if (jt_ == SYM(floodgateKeeper) && hook_is(gb, SYM(floodgateKeeper), s_floodgateKeeper_hook)) { s_floodgateKeeper_hook(gb); return; }
    if (jt_ == SYM(floodgateKeeperSwitchScript) && hook_is(gb, SYM(floodgateKeeperSwitchScript), s_floodgateKeeperSwitchScript_hook)) { s_floodgateKeeperSwitchScript_hook(gb); return; }
    if (jt_ == SYM(floodgateKeyhole) && hook_is(gb, SYM(floodgateKeyhole), s_floodgateKeyhole_hook)) { s_floodgateKeyhole_hook(gb); return; }
    if (jt_ == SYM(d4KeyHole) && hook_is(gb, SYM(d4KeyHole), s_d4KeyHole_hook)) { s_d4KeyHole_hook(gb); return; }
    if (jt_ == SYM(floodgateKey) && hook_is(gb, SYM(floodgateKey), s_floodgateKey_hook)) { s_floodgateKey_hook(gb); return; }
    if (jt_ == SYM(dragonKey) && hook_is(gb, SYM(dragonKey), s_dragonKey_hook)) { s_dragonKey_hook(gb); return; }
    if (jt_ == SYM(tarmArmosUnlockingStairs) && hook_is(gb, SYM(tarmArmosUnlockingStairs), s_tarmArmosUnlockingStairs_hook)) { s_tarmArmosUnlockingStairs_hook(gb); return; }
    if (jt_ == SYM(tarmArmosWallByStump) && hook_is(gb, SYM(tarmArmosWallByStump), s_tarmArmosWallByStump_hook)) { s_tarmArmosWallByStump_hook(gb); return; }
    if (jt_ == SYM(tarmEscapedLostWoods) && hook_is(gb, SYM(tarmEscapedLostWoods), s_tarmEscapedLostWoods_hook)) { s_tarmEscapedLostWoods_hook(gb); return; }
    if (jt_ == SYM(oreChunkDigSpot) && hook_is(gb, SYM(oreChunkDigSpot), s_oreChunkDigSpot_hook)) { s_oreChunkDigSpot_hook(gb); return; }
    if (jt_ == SYM(staticHeartPiece) && hook_is(gb, SYM(staticHeartPiece), s_staticHeartPiece_hook)) { s_staticHeartPiece_hook(gb); return; }
    if (jt_ == SYM(permanentlyRemovableObjects) && hook_is(gb, SYM(permanentlyRemovableObjects), s_permanentlyRemovableObjects_hook)) { s_permanentlyRemovableObjects_hook(gb); return; }
    if (jt_ == SYM(piratesBellRoomWhenFallingIn) && hook_is(gb, SYM(piratesBellRoomWhenFallingIn), s_piratesBellRoomWhenFallingIn_hook)) { s_piratesBellRoomWhenFallingIn_hook(gb); return; }
    if (jt_ == SYM(greenJoyRing) && hook_is(gb, SYM(greenJoyRing), s_greenJoyRing_hook)) { s_greenJoyRing_hook(gb); return; }
    if (jt_ == SYM(masterDiverPuzzle) && hook_is(gb, SYM(masterDiverPuzzle), s_masterDiverPuzzle_hook)) { s_masterDiverPuzzle_hook(gb); return; }
    if (jt_ == SYM(piratesBell) && hook_is(gb, SYM(piratesBell), s_piratesBell_hook)) { s_piratesBell_hook(gb); return; }
    if (jt_ == SYM(armosBlockingFlowerPathToD6) && hook_is(gb, SYM(armosBlockingFlowerPathToD6), s_armosBlockingFlowerPathToD6_hook)) { s_armosBlockingFlowerPathToD6_hook(gb); return; }
    if (jt_ == SYM(natzuSwitch) && hook_is(gb, SYM(natzuSwitch), s_natzuSwitch_hook)) { s_natzuSwitch_hook(gb); return; }
    if (jt_ == SYM(onoxCastleCutscene) && hook_is(gb, SYM(onoxCastleCutscene), s_onoxCastleCutscene_hook)) { s_onoxCastleCutscene_hook(gb); return; }
    if (jt_ == SYM(savingZeldaNoEnemiesHandler) && hook_is(gb, SYM(savingZeldaNoEnemiesHandler), s_savingZeldaNoEnemiesHandler_hook)) { s_savingZeldaNoEnemiesHandler_hook(gb); return; }
    if (jt_ == SYM(unblockingD3Dam) && hook_is(gb, SYM(unblockingD3Dam), s_unblockingD3Dam_hook)) { s_unblockingD3Dam_hook(gb); return; }
    if (jt_ == SYM(replacePirateShipWithQuicksand) && hook_is(gb, SYM(replacePirateShipWithQuicksand), s_replacePirateShipWithQuicksand_hook)) { s_replacePirateShipWithQuicksand_hook(gb); return; }
    if (jt_ == SYM(stolenFeatherGottenHandler) && hook_is(gb, SYM(stolenFeatherGottenHandler), s_stolenFeatherGottenHandler_hook)) { s_stolenFeatherGottenHandler_hook(gb); return; }
    if (jt_ == SYM(horonVillagePortalBridgeSpawner) && hook_is(gb, SYM(horonVillagePortalBridgeSpawner), s_horonVillagePortalBridgeSpawner_hook)) { s_horonVillagePortalBridgeSpawner_hook(gb); return; }
    if (jt_ == SYM(randomRingDigSpot) && hook_is(gb, SYM(randomRingDigSpot), s_randomRingDigSpot_hook)) { s_randomRingDigSpot_hook(gb); return; }
    if (jt_ == SYM(staticGashaSeed) && hook_is(gb, SYM(staticGashaSeed), s_staticGashaSeed_hook)) { s_staticGashaSeed_hook(gb); return; }
    if (jt_ == SYM(underwaterGashaSeed) && hook_is(gb, SYM(underwaterGashaSeed), s_underwaterGashaSeed_hook)) { s_underwaterGashaSeed_hook(gb); return; }
    if (jt_ == SYM(tickTockSecretEntrance) && hook_is(gb, SYM(tickTockSecretEntrance), s_tickTockSecretEntrance_hook)) { s_tickTockSecretEntrance_hook(gb); return; }
    if (jt_ == SYM(graveSecretEntrance) && hook_is(gb, SYM(graveSecretEntrance), s_graveSecretEntrance_hook)) { s_graveSecretEntrance_hook(gb); return; }
    if (jt_ == SYM(d4MinibossRoom) && hook_is(gb, SYM(d4MinibossRoom), s_d4MinibossRoom_hook)) { s_d4MinibossRoom_hook(gb); return; }
    if (jt_ == SYM(sentBackFromOnoxCastleBarrier) && hook_is(gb, SYM(sentBackFromOnoxCastleBarrier), s_sentBackFromOnoxCastleBarrier_hook)) { s_sentBackFromOnoxCastleBarrier_hook(gb); return; }
    if (jt_ == SYM(sidescrollingStaticGashaSeed) && hook_is(gb, SYM(sidescrollingStaticGashaSeed), s_sidescrollingStaticGashaSeed_hook)) { s_sidescrollingStaticGashaSeed_hook(gb); return; }
    if (jt_ == SYM(sidescrollingStaticSeedSatchel) && hook_is(gb, SYM(sidescrollingStaticSeedSatchel), s_sidescrollingStaticSeedSatchel_hook)) { s_sidescrollingStaticSeedSatchel_hook(gb); return; }
    if (jt_ == SYM(mtCuccoBananaTree) && hook_is(gb, SYM(mtCuccoBananaTree), s_mtCuccoBananaTree_hook)) { s_mtCuccoBananaTree_hook(gb); return; }
    if (jt_ == SYM(hardOre) && hook_is(gb, SYM(hardOre), s_hardOre_hook)) { s_hardOre_hook(gb); return; }
    if (jt_ == SYM(interactionCode6bSubid23) && hook_is(gb, SYM(interactionCode6bSubid23), s_interactionCode6bSubid23_hook)) { s_interactionCode6bSubid23_hook(gb); return; }
    if (jt_ == SYM(interactionCode6bSubid24) && hook_is(gb, SYM(interactionCode6bSubid24), s_interactionCode6bSubid24_hook)) { s_interactionCode6bSubid24_hook(gb); return; }
    if (jt_ == SYM(interactionCode6bSubid25) && hook_is(gb, SYM(interactionCode6bSubid25), s_interactionCode6bSubid25_hook)) { s_interactionCode6bSubid25_hook(gb); return; }
    if (jt_ == SYM(interactionCode6bSubid26) && hook_is(gb, SYM(interactionCode6bSubid26), s_interactionCode6bSubid26_hook)) { s_interactionCode6bSubid26_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

