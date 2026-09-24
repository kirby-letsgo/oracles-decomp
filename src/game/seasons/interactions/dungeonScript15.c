#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_DUNGEON_SCRIPT), bank 0x15: the
// helpers the dungeon scripts call for puzzles, bridges and spawns.

static void dungeon_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void s_D3spawnPitSpreader_hook(GB *gb) {
  BASE(D3spawnPitSpreader);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0072); // subid 0, yh $72
  CYC(b_+3, b_+6);
  TAIL_S(spawnPitSpreader);
}

// wActiveTriggers = 1 when the six statues at $84.. are three blue then three red.
void s_D3StatuePuzzleCheck_hook(GB *gb) {
  BASE(D3StatuePuzzleCheck);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+4, b_+6); L = 0x84;
  CYC(b_+6, b_+8); H = 0xcf; // wRoomLayout
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0x2c);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+15); alu_cp(gb, 0x2c);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+19); alu_cp(gb, 0x2c);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+23); alu_cp(gb, 0x2d);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+27); alu_cp(gb, 0x2d);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+31); alu_cp(gb, 0x2d);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+37); mem_wr(gb, wActiveTriggers, A);
  RET(b_+37); return;
}

void s_solvedPuzzleSetRoomFlag07_hook(GB *gb) {
  BASE(solvedPuzzleSetRoomFlag07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+5, b_+7); A = 0x4d; // SND_SOLVEPUZZLE
  CYC(b_+7, b_+10);
  TAIL(playSound_b00);
}

// A bridge spawner part: counter2 b, angle c, yh e.
void s_createBridgeSpawner_hook(GB *gb) {
  BASE(createBridgeSpawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0c); // PART_BRIDGE_SPAWNER
  CYC(b_+6, b_+8); L = 0xc7; // Part.counter2
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+11); L = 0xc9; // Part.angle
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+14); L = 0xcb; // Part.yh
  CYC(b_+14, b_+15); mem_wr(gb, HL, E);
  RET(b_+15); return;
}

void s_D4spawnBridgeB2_hook(GB *gb) {
  BASE(D4spawnBridgeB2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_solvedPuzzleSetRoomFlag07_hook, SYM(solvedPuzzleSetRoomFlag07), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x0601);
  CYC(b_+6, b_+8); E = 0x59;
  CYC(b_+8, b_+11);
  TAIL_S(createBridgeSpawner);
}

void s_D7spawnDarknutBridge_hook(GB *gb) {
  BASE(D7spawnDarknutBridge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0801);
  CYC(b_+8, b_+10); E = 0x77;
  CYC(b_+10, b_+13);
  TAIL_S(createBridgeSpawner);
}

void s_D8VerticalBridgeUnlockedByOrb_hook(GB *gb) {
  BASE(D8VerticalBridgeUnlockedByOrb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_solvedPuzzleSetRoomFlag07_hook, SYM(solvedPuzzleSetRoomFlag07), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x0c02);
  CYC(b_+6, b_+8); E = 0x3c;
  CYC(b_+8, b_+11);
  TAIL_S(createBridgeSpawner);
}

void s_D8VerticalBridgeInLava_hook(GB *gb) {
  BASE(D8VerticalBridgeInLava);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_solvedPuzzleSetRoomFlag07_hook, SYM(solvedPuzzleSetRoomFlag07), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x0e00);
  CYC(b_+6, b_+8); E = 0x7b;
  CYC(b_+8, b_+11);
  TAIL_S(createBridgeSpawner);
}

void s_D8HorizontalBridgeByMoldorms_hook(GB *gb) {
  BASE(D8HorizontalBridgeByMoldorms);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_solvedPuzzleSetRoomFlag07_hook, SYM(solvedPuzzleSetRoomFlag07), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x0e03);
  CYC(b_+6, b_+8); E = 0x88;
  CYC(b_+8, b_+11);
  TAIL_S(createBridgeSpawner);
}

// A pit-spreading floor trap part with subid b at yh c.
void s_spawnPitSpreader_hook(GB *gb) {
  BASE(spawnPitSpreader);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0a); // PART_HOLES_FLOORTRAP
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);
  CYC(b_+8, b_+10); L = 0xcb; // Part.yh
  CYC(b_+10, b_+11); mem_wr(gb, HL, C);
  RET(b_+11); return;
}

void s_D3hallToMiniboss_buttonStepped_hook(GB *gb) {
  BASE(D3hallToMiniboss_buttonStepped);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wEyePuzzleCorrectDirection, A);
  RET(b_+5); return;
}

// If the D3 boss room ($53) has its bit 7, spawns the door controller that opens the essence
// door's shutter.
void s_D3openEssenceDoorIfBossBeat_body_hook(GB *gb) {
  BASE(D3openEssenceDoorIfBossBeat_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonFlagsAddressH);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); C = 0x53; // ROOM_SEASONS_453
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+9); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x1e); // INTERAC_DOOR_CONTROLLER
  CYC(b_+15, b_+17); L = 0x49; // Interaction.angle
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+19, b_+21); L = 0x4b; // Interaction.yh
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x07);
  RET(b_+23); return;
}

void s_D6setFlagBit7InRoomWithLowIndexInAngle_hook(GB *gb) {
  BASE(D6setFlagBit7InRoomWithLowIndexInAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x49; // Interaction.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); L = A;
  CYCT(b_+4, b_+6);
  TAIL_S(setFlagBit7InRoomLowIndexInL);
}

void s_D6setFlagBit7InFirst4FRoom_hook(GB *gb) {
  BASE(D6setFlagBit7InFirst4FRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0xd4; // ROOM_SEASONS_4d4
  CYCT(b_+2, b_+4);
  TAIL_S(setFlagBit7InRoomLowIndexInL);
}

void s_D6setFlagBit7InLast4FRoom_hook(GB *gb) {
  BASE(D6setFlagBit7InLast4FRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0xd3; // ROOM_SEASONS_4d3
  TAIL_S(setFlagBit7InRoomLowIndexInL);
}

// Sets bit 7 of the current dungeon's room flags for room l.
void s_setFlagBit7InRoomLowIndexInL_hook(GB *gb) {
  BASE(setFlagBit7InRoomLowIndexInL);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonFlagsAddressH);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+6); return;
}

// wTmpcfc0+1 = 0 with no trigger pressed, otherwise 1 or 2 by the frame counter.
void s_D6getRandomButtonResult_hook(GB *gb) {
  BASE(D6getRandomButtonResult);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+5); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto store; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wFrameCounter);
  CYC(b_+11, b_+13); alu_and(gb, 0x01);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
  CYC(b_+14, b_+15); B = A;
store:
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+19); mem_wr(gb, wTmpcfc0 + 0x01, A);
  RET(b_+19); return;
}

void s_D6spawnFloorDestroyerAndEscapeBridge_hook(GB *gb) {
  BASE(D6spawnFloorDestroyerAndEscapeBridge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0a); // PART_HOLES_FLOORTRAP
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x04);
  CYC(b_+9, b_+12); SET_BC(0x0603);
  CYC(b_+12, b_+14); E = 0x14;
  CYC(b_+14, b_+17);
  TAIL_S(createBridgeSpawner);
}

void s_D6spawnChestAfterCrystalTrapRoom_body_hook(GB *gb) {
  BASE(D6spawnChestAfterCrystalTrapRoom_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CALL_C(b_+4, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+7);
  CYC(b_+7, b_+8); SET_HL(HL + 1);
  CYC(b_+8, b_+10); mem_wr(gb, HL, mem_rd(gb, HL) & 0xdf);
  RET(b_+10); return;
}

void s_warpToD7Entrance_hook(GB *gb) {
  BASE(warpToD7Entrance);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+6); // @warpDestVariables
  CYC(b_+3, b_+6);
  TAIL(setWarpDestVariables);
}

// Puts a decorative armos tile at one of eight random positions (layout and the bank-3 tile
// index buffer) and spawns the armos enemy off screen.
void s_D7randomlyPlaceNonEnemyArmos_body_hook(GB *gb) {
  BASE(D7randomlyPlaceNonEnemyArmos_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+8); SET_HL(b_+43); // @armosPositions
  CYC(b_+8, b_+9); dungeon_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+10); L = mem_rd(gb, HL);
  CYC(b_+10, b_+12); H = 0xcf; // wRoomLayout
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x25);
  CYC(b_+14, b_+16); A = 0x03;
  CYC(b_+16, b_+18); mem_wr(gb, IO_SVBK, A);
  CYC(b_+18, b_+20); H = 0xdf; // w3TileIndexBuffer
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x25);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+25); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+25, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x1d); // ENEMY_ARMOS
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x00);
  CYC(b_+34, b_+36); L = 0x8b; // Enemy.yh
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x27);
  CYC(b_+38, b_+40); L = 0x8d; // Enemy.xh
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0xa0);
  RET(b_+42); return;
}

// D7MagnetBallRoom_addChest from its +20: the puff.
static void dungeon_puff(GB *gb) {
  BASE(D7MagnetBallRoom_addChest);
  CYC(b_+20, b_+23);
  TAIL(objectCreatePuff);
}

void s_D7MagnetBallRoom_removeChest_hook(GB *gb) {
  BASE(D7MagnetBallRoom_removeChest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0xa3); // TILEINDEX_DUNGEON_a3
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); C = L;
  CYC(b_+7, b_+9); A = 0xa3;
  CALL_C(b_+9, setTile_hook, SYM(setTile), b_+12);
  CYCT(b_+12, b_+14); dungeon_puff(gb); return;
}

void s_D7MagnetBallRoom_addChest_hook(GB *gb) {
  BASE(D7MagnetBallRoom_addChest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0xf0); // TILEINDEX_CHEST_OPENED
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); C = L;
  CYC(b_+10, b_+12); A = 0xf1;
  CALL_C(b_+12, setTile_hook, SYM(setTile), b_+15);
  CYC(b_+15, b_+17); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+17, playSound_b00_hook, SYM(playSound_b00), b_+20);
  dungeon_puff(gb); return;
}

// A small key at the object, this room's bit 6 and the room below's ($45) bit 7.
void s_D7dropKeyDownAFloor_hook(GB *gb) {
  BASE(D7dropKeyDownAFloor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x30); // TREASURE_SMALL_KEY
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CALL_C(b_+12, objectCopyPosition_hook, SYM(objectCopyPosition), b_+15);
  CALL_C(b_+15, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+18);
  CYC(b_+18, b_+20); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+20, b_+22); L = 0x45; // ROOM_SEASONS_545
  CYC(b_+22, b_+24); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+24, b_+26); A = 0x4d; // SND_SOLVEPUZZLE
  CYC(b_+26, b_+29);
  TAIL(playSound_b00);
}

void s_checkFirstPoeBeaten_hook(GB *gb) {
  BASE(checkFirstPoeBeaten);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x56; // ROOM_SEASONS_556
  TAIL_S(checkPoeBeaten);
}

// wTmpcfc0+1 = 1 when room a's bit 6 is set.
void s_checkPoeBeaten_hook(GB *gb) {
  BASE(checkPoeBeaten);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getARoomFlags_hook, SYM(getARoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(b_+5, b_+7); A = 0x01;
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
store:
  CYC(b_+10, b_+13); mem_wr(gb, wTmpcfc0 + 0x01, A);
  RET(b_+13); return;
}

void s_checkSecondPoeBeaten_hook(GB *gb) {
  BASE(checkSecondPoeBeaten);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x4e; // ROOM_SEASONS_54e
  CYCT(b_+2, b_+4);
  TAIL_S(checkPoeBeaten);
}

// While Link uses no item, every other frame counts Interaction.direction down; at zero the
// armos enemy spawns off screen and the room flag is set. Sets angle = 1 when done.
void s_D8armosCheckIfWillMove_hook(GB *gb) {
  BASE(D8armosCheckIfWillMove);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkUsingItem1);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto done; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+10); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x48; // Interaction.direction
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+19);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto done; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x1d); // ENEMY_ARMOS
  CYC(b_+23, b_+25); L = 0x8b; // Enemy.yh
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x27);
  CYC(b_+27, b_+29); L = 0x8d; // Enemy.xh
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0xa0);
  CYC(b_+31, b_+33); A = 0x45;
  CYC(b_+33, b_+36); mem_wr(gb, wcca2, A);
  CYC(b_+36, b_+38); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+38, playSound_b00_hook, SYM(playSound_b00), b_+41);
  CALL_C(b_+41, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+44);
  CYC(b_+44, b_+46); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
done:
  CYC(b_+46, b_+48); E = 0x49; // Interaction.angle
  CYC(b_+48, b_+50); A = 0x01;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  RET(b_+51); return;
}

void s_D8setSpawnAtLavaHole_hook(GB *gb) {
  BASE(D8setSpawnAtLavaHole);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xd0; // TILEINDEX_LAVA_HOLE
  CALL_C(b_+2, findTileInRoom_hook, SYM(findTileInRoom), b_+5);
  CYC(b_+5, b_+6); A = L;
  CYC(b_+6, b_+8); L = 0x4b; // Interaction.yh
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+12);
  TAIL(setShortPosition);
}

// With fewer than four fire keese, a 1/4 chance per call of one more at the object.
void s_D8SpawnLimitedFireKeese_hook(GB *gb) {
  BASE(D8SpawnLimitedFireKeese);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); B = 0x39; // ENEMY_FIRE_KEESE
  CALL_C(b_+7, s_countFireKeese_hook, SYM(countFireKeese), b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x04);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CALL_C(b_+13, getRandomNumber_hook, SYM(getRandomNumber), b_+16);
  CYC(b_+16, b_+18); alu_cp(gb, 0x40);
  if (F & FC) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+22);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x39); // ENEMY_FIRE_KEESE
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x01);
  CYC(b_+28, b_+31);
  TAIL(objectCopyPosition);
}

// a = the number of enabled enemies with id b.
void s_countFireKeese_hook(GB *gb) {
  BASE(countFireKeese);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x00;
  CYC(b_+2, b_+5); SET_HL(w1Link + 0x80); // the first enemy slot
  for (;;) {
    CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+6, b_+7); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+7, b_+9); goto next; }
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+10); A = mem_rd(gb, HL);
    CYC(b_+10, b_+11); alu_cp(gb, B);
    if (!(F & FZ)) { CYCT(b_+11, b_+13); goto next; }
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); C = alu_inc8(gb, C);
next:
    CYC(b_+14, b_+15); L = alu_dec8(gb, L);
    CYC(b_+15, b_+16); H = alu_inc8(gb, H);
    CYC(b_+16, b_+17); A = H;
    CYC(b_+17, b_+19); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+19, b_+21); continue; }
    CYC(b_+19, b_+21);
    break;
  }
  CYC(b_+21, b_+22); A = C;
  CYC(b_+22, b_+23); alu_or(gb, A);
  RET(b_+23); return;
}

// wTmpcfc0+1 = 1 when the three ice blocks sit at $4d, $5d and $6d.
void s_D8checkAllIceBlocksInPlace_hook(GB *gb) {
  BASE(D8checkAllIceBlocksInPlace);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0 + 0x01, A);
  CYC(b_+4, b_+6); H = 0xcf; // wRoomLayout
  CYC(b_+6, b_+8); L = 0x4d;
  CYC(b_+8, b_+10); A = 0x2f; // TILEINDEX_PUSHABLE_ICE_BLOCK
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); L = 0x5d;
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); L = 0x6d;
  CYC(b_+18, b_+19); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); mem_wr(gb, wTmpcfc0 + 0x01, A);
  RET(b_+25); return;
}

// @spawnRopeAtRandomPosition: a random free (collision 0) position, y 1..7, x 3..10.
static void dungeon_rope_position(GB *gb) {
  BASE(D6RandomButtonSpawnRopes);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  for (;;) {
    CALL_C(b_+18, getRandomNumber_hook, SYM(getRandomNumber), b_+21);
    CYC(b_+21, b_+23); alu_and(gb, 0x07);
    CYC(b_+23, b_+24); A = alu_inc8(gb, A);
    CYC(b_+24, b_+26); A = alu_swap(gb, A);
    CYC(b_+26, b_+27); B = A;
    CYC(b_+27, b_+29); alu_bit(gb, 7, A);
    if (!(F & FZ)) { CYCT(b_+29, b_+31); continue; }
    CYC(b_+29, b_+31);
    CALL_C(b_+31, getRandomNumber_hook, SYM(getRandomNumber), b_+34);
    CYC(b_+34, b_+36); alu_and(gb, 0x07);
    CYC(b_+36, b_+38); alu_add(gb, 0x03);
    CYC(b_+38, b_+39); alu_or(gb, B);
    CYC(b_+39, b_+41); B = 0xce; // wRoomCollisions
    CYC(b_+41, b_+42); C = A;
    CYC(b_+42, b_+43); A = mem_rd(gb, BC);
    CYC(b_+43, b_+44); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+44, b_+46); continue; }
    CYC(b_+44, b_+46);
    break;
  }
  CYC(b_+46, b_+48); L = 0x8b; // Enemy.yh
  CYC(b_+48, b_+51);
  TAIL(setShortPosition_paramC);
}

// Six ropes at random free positions.
void s_D6RandomButtonSpawnRopes_hook(GB *gb) {
  BASE(D6RandomButtonSpawnRopes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x06;
  for (;;) {
    CALL_C(b_+2, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+5);
    if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
    CYC(b_+5, b_+6);
    CYC(b_+6, b_+8); mem_wr(gb, HL, 0x10); // ENEMY_ROPE
    CYC(b_+8, b_+9); L = alu_inc8(gb, L);
    CYC(b_+9, b_+11); mem_wr(gb, HL, 0x01);
    CALL_L(b_+11, dungeon_rope_position, b_+14);
    CYC(b_+14, b_+15); E = alu_dec8(gb, E);
    if (!(F & FZ)) { CYCT(b_+15, b_+17); continue; }
    CYC(b_+15, b_+17);
    break;
  }
  RET(b_+17); return;
}

// wActiveTriggers = 1 when every toggle block bit in Interaction.angle is set.
void s_toggleBlocksInAngleBitsHit_hook(GB *gb) {
  BASE(toggleBlocksInAngleBitsHit);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x49; // Interaction.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+7, b_+8); alu_and(gb, B);
  CYC(b_+8, b_+9); alu_cp(gb, B);
  CYC(b_+9, b_+11); A = 0x01;
  if (F & FZ) { CYCT(b_+11, b_+13); goto store; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
store:
  CYC(b_+14, b_+17); mem_wr(gb, wActiveTriggers, A);
  RET(b_+17); return;
}

void s_createD7Trampoline_hook(GB *gb) {
  BASE(createD7Trampoline);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x49; // Interaction.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); B = 0x7c; // INTERAC_TRAMPOLINE
  CYC(b_+6, b_+9);
  TAIL(objectCreateInteraction);
}

// Clears bit 6 (room cleared) of D9's rooms $93..$95.
void s_D9forceRoomClearsOnDungeonEntry_hook(GB *gb) {
  BASE(D9forceRoomClearsOnDungeonEntry);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); L = 0x93; // ROOM_SEASONS_593
  CYC(b_+5, b_+7); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  RET(b_+13); return;
}

// A lightable torch part on every unlit torch tile in the room.
void s_D8createFiresGoingOut_hook(GB *gb) {
  BASE(D8createFiresGoingOut);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x08; // TILEINDEX_UNLIT_TORCH
  CALL_C(b_+2, findTileInRoom_hook, SYM(findTileInRoom), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_createLightableTorches_hook, SYM(createLightableTorches), b_+9);
  for (;;) {
    CYC(b_+9, b_+11); A = 0x08; // TILEINDEX_UNLIT_TORCH
    CALL_C(b_+11, backwardsSearch_hook, SYM(backwardsSearch), b_+14);
    if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
    CYC(b_+14, b_+15);
    CALL_C(b_+15, s_createLightableTorches_hook, SYM(createLightableTorches), b_+18);
    CYCT(b_+18, b_+20);
  }
}

// A lightable torch part (subid 1, counter2 $30) at layout position hl; returns hl - 1.
void s_createLightableTorches_hook(GB *gb) {
  BASE(createLightableTorches);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); C = L;
  CALL_C(b_+2, getFreePartSlot_hook, SYM(getFreePartSlot), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto done; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x06); // PART_LIGHTABLE_TORCH
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CYC(b_+12, b_+14); L = 0xc7; // Part.counter2
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x30);
  CYC(b_+16, b_+18); L = 0xcb; // Part.yh
  CALL_C(b_+18, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+21);
done:
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+23); SET_HL(HL - 1);
  RET(b_+23); return;
}
