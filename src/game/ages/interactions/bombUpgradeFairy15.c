#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void bombUpgradeFairy_linkCollapsed_hook(GB *gb);

void bombUpgradeFairy_spawnBombsAroundLink_hook(GB *gb) {
  BASE(bombUpgradeFairy_spawnBombsAroundLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x04;
next:
  CALL_C(b_+2, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x83);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); B = alu_dec8(gb, B);
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto next; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void bombUpgradeFairy_lightningStrikesLink_hook(GB *gb) {
  BASE(bombUpgradeFairy_lightningStrikesLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = alu_dec8(gb, L);
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x27);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+14); L = 0xcb;
  CYC(b_+14, b_+16); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void bombUpgradeFairy_decreaseLinkHealth_hook(GB *gb) {
  BASE(bombUpgradeFairy_decreaseLinkHealth);
  CYC(b_+0, b_+3); SET_HL(wLinkHealth);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x04);
  if (F & FC) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x04);
  TAIL(bombUpgradeFairy_linkCollapsed);
}

void bombUpgradeFairy_linkCollapsed_hook(GB *gb) {
  BASE(bombUpgradeFairy_linkCollapsed);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); mem_wr(gb, wcc50, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void bombUpgradeFairy_loseAllBombs_hook(GB *gb) {
  BASE(bombUpgradeFairy_loseAllBombs);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wNumBombs, A);
  CALL_C(b_+5, decNumBombs_hook, SYM(decNumBombs), b_+8);
  CYC(b_+8, b_+10); bombUpgradeFairy_linkCollapsed_hook(gb);
}

void bombUpgradeFairy_giveBombUpgrade_hook(GB *gb) {
  BASE(bombUpgradeFairy_giveBombUpgrade);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextNumberSubstitution);
  CYC(b_+3, b_+6); mem_wr(gb, wMaxBombs, A);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+9); A = 0x03;
  CYC(b_+9, b_+12); giveTreasure_hook(gb);
}

void bombUpgradeFairy_fadeinFromWhite_hook(GB *gb) {
  BASE(bombUpgradeFairy_fadeinFromWhite);
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+5, b_+7); A = 0x04;
  CYC(b_+7, b_+10); fadeinFromWhiteWithDelay_hook(gb);
}

void bombUpgradeFairy_setGlobalFlag_hook(GB *gb) {
  BASE(bombUpgradeFairy_setGlobalFlag);
  CYC(b_+0, b_+2); A = 0x1c;
  CYC(b_+2, b_+5); setGlobalFlag_hook(gb);
}
