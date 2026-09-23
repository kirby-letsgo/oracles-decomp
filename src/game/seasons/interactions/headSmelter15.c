#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_HEAD_SMELTER), bank 0x15.

void s_headSmelter_loseBombFlower_hook(GB *gb) {
  BASE(headSmelter_loseBombFlower);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x58; // TREASURE_BOMB_FLOWER_LOWER_HALF
  CALL_C(b_+2, loseTreasure_hook, SYM(loseTreasure), b_+5);
  CYC(b_+5, b_+7); A = 0x49; // TREASURE_BOMB_FLOWER
  CYC(b_+7, b_+10);
  TAIL(loseTreasure);
}

void s_headSmelter_loadHideFromBombScript_hook(GB *gb) {
  BASE(headSmelter_loadHideFromBombScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x1e);
  CYC(b_+3, b_+6); SET_BC(SYM(headSmelterAtTempleScript_hideFromBomb));
  CYCT(b_+6, b_+8);
  TAIL_S(headSmelter_loadScriptIntoWram);
}

// Link walks down to y $68 (forced state $0b), and the two dancers get their movement scripts.
void s_headSmelter_loadDanceMovements_hook(GB *gb) {
  BASE(headSmelter_loadDanceMovements);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkForceState, A);
  CYC(b_+5, b_+8); SET_HL(w1Link_yh);
  CYC(b_+8, b_+10); A = 0x68;
  CYC(b_+10, b_+11); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+14); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+14, b_+16); L = 0x08; // w1Link.direction
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(b_+18, b_+20); L = 0x09; // w1Link.angle
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+22, b_+25); SET_HL(wTmpcfc0 + 0x1e);
  CYC(b_+25, b_+28); SET_BC(SYM(headSmelterScript_danceMovementText1));
  CALL_C(b_+28, s_headSmelter_loadScriptIntoWram_hook, SYM(headSmelter_loadScriptIntoWram), b_+31);
  CYC(b_+31, b_+34); SET_HL(wTmpcfc0 + 0x1c);
  CYC(b_+34, b_+37); SET_BC(SYM(headSmelterScript_danceMovementText2));
  TAIL_S(headSmelter_loadScriptIntoWram);
}

// Stores the script pointer bc in the object whose pointer is at hl.
void s_headSmelter_loadScriptIntoWram_hook(GB *gb) {
  BASE(headSmelter_loadScriptIntoWram);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); L = mem_rd(gb, HL);
  CYC(b_+2, b_+3); H = A;
  CYC(b_+3, b_+4); mem_wr(gb, HL, C);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); mem_wr(gb, HL, B);
  RET(b_+6); return;
}

// headSmelter_throwBlueOreIn from its +2: the misc static object c.
static void smelter_throw_ore(GB *gb) {
  BASE(headSmelter_throwBlueOreIn);
  CYC(b_+2, b_+4); B = 0x4c; // INTERAC_MISC_STATIC_OBJECTS
  CYC(b_+4, b_+7);
  TAIL(objectCreateInteraction);
}

void s_headSmelter_throwRedOreIn_hook(GB *gb) {
  BASE(headSmelter_throwRedOreIn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x04;
  CYCT(b_+2, b_+4); smelter_throw_ore(gb); return;
}

void s_headSmelter_throwBlueOreIn_hook(GB *gb) {
  BASE(headSmelter_throwBlueOreIn);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x05;
  smelter_throw_ore(gb); return;
}

void s_headSmelter_smeltingDone_hook(GB *gb) {
  BASE(headSmelter_smeltingDone);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x04); // PART_BOSS_DEATH_EXPLOSION
  CYC(b_+6, b_+8); L = 0xcb; // Part.yh
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x1c);
  CYC(b_+10, b_+12); L = 0xcd; // Part.xh
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x70);
  RET(b_+14); return;
}

// Takes the red and blue ore, drops the hard ore at ($1c,$70).
void s_headSmelter_giveHardOre_hook(GB *gb) {
  BASE(headSmelter_giveHardOre);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x50; // TREASURE_RED_ORE
  CALL_C(b_+2, loseTreasure_hook, SYM(loseTreasure), b_+5);
  CYC(b_+5, b_+7); A = 0x51; // TREASURE_BLUE_ORE
  CALL_C(b_+7, loseTreasure_hook, SYM(loseTreasure), b_+10);
  CALL_C(b_+10, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x52); // TREASURE_HARD_ORE
  CYC(b_+19, b_+21); L = 0x4b; // Interaction.yh
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x1c);
  CYC(b_+23, b_+25); L = 0x4d; // Interaction.xh
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x70);
  RET(b_+27); return;
}

// The smelter's four tiles lit ($e8..$eb), with the fire sound.
void s_headSmelter_setTiles_hook(GB *gb) {
  BASE(headSmelter_setTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xe8;
  CYC(b_+2, b_+4); C = 0x06;
  CALL_C(b_+4, setTile_hook, SYM(setTile), b_+7);
  CYC(b_+7, b_+9); A = 0xe9;
  CYC(b_+9, b_+11); C = 0x07;
  CALL_C(b_+11, setTile_hook, SYM(setTile), b_+14);
  CYC(b_+14, b_+16); A = 0xea;
  CYC(b_+16, b_+18); C = 0x16;
  CALL_C(b_+18, setTile_hook, SYM(setTile), b_+21);
  CYC(b_+21, b_+23); A = 0xeb;
  CYC(b_+23, b_+25); C = 0x17;
  CALL_C(b_+25, setTile_hook, SYM(setTile), b_+28);
  CYC(b_+28, b_+30); A = 0x70;
  CYC(b_+30, b_+33);
  TAIL(playSound_b00);
}

// The smelter's four tiles unlit ($e4..$e7).
void s_headSmelter_resetTiles_hook(GB *gb) {
  BASE(headSmelter_resetTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0xe4;
  CYC(b_+2, b_+4); C = 0x06;
  CALL_C(b_+4, setTile_hook, SYM(setTile), b_+7);
  CYC(b_+7, b_+9); A = 0xe5;
  CYC(b_+9, b_+11); C = 0x07;
  CALL_C(b_+11, setTile_hook, SYM(setTile), b_+14);
  CYC(b_+14, b_+16); A = 0xe6;
  CYC(b_+16, b_+18); C = 0x16;
  CALL_C(b_+18, setTile_hook, SYM(setTile), b_+21);
  CYC(b_+21, b_+23); A = 0xe7;
  CYC(b_+23, b_+25); C = 0x17;
  CYC(b_+25, b_+28);
  TAIL(setTile);
}

// headSmelter_disableScreenTransitions from its +2: a into both flags.
static void smelter_store_flags(GB *gb) {
  BASE(headSmelter_disableScreenTransitions);
  CYC(b_+2, b_+5); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+5, b_+8); mem_wr(gb, wInShop, A);
  RET(b_+8); return;
}

void s_headSmelter_disableScreenTransitions_hook(GB *gb) {
  BASE(headSmelter_disableScreenTransitions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  smelter_store_flags(gb); return;
}

void s_headSmelter_enableScreenTransitions_hook(GB *gb) {
  BASE(headSmelter_enableScreenTransitions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYCT(b_+1, b_+3); smelter_store_flags(gb); return;
}
