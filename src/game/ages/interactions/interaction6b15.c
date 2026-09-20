#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

static void interaction6b_isLinkWithinBox_from_call(GB *gb, uint16_t return_address) {
  BASE(interaction6b_isLinkAtScreenEdge);
  push_effect(gb, return_address);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+18); alu_sub(gb, 0x22);
  CYC(b_+18, b_+20); alu_cp(gb, 0x54);
  if (!(F & FC)) { CYCT(b_+20, b_+21); ret_effect(gb); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); L = alu_inc8(gb, L);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_sub(gb, 0x14);
  CYC(b_+25, b_+27); alu_cp(gb, 0x84);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void interaction6b_loadMoblinsAttackingMakuSprout_hook(GB *gb) {
  BASE(interaction6b_loadMoblinsAttackingMakuSprout);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(symmetryNpcSubid8And9Script__askForSecret_b15) + 7));
  CYC(b_+3, b_+6); parseGivenObjectData_b00_hook(gb);
}

void interaction6b_layoutSwapMakuTreeRooms_hook(GB *gb) {
  BASE(interaction6b_layoutSwapMakuTreeRooms);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((wGroup0RoomFlags + 56));
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  CYC(b_+5, b_+8); SET_HL((wGroup1RoomFlags + 72));
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(b_+10, b_+11); ret_effect(gb);
}

void interaction6b_isLinkAtScreenEdge_hook(GB *gb) {
  BASE(interaction6b_isLinkAtScreenEdge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+6); interaction6b_isLinkWithinBox_from_call(gb, b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  if (!(F & FC)) { CYCT(b_+8, b_+10); goto L_6ba0; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); alu_xor(gb, A);
L_6ba0:
  CYC(b_+11, b_+12); alu_or(gb, A);
  CYC(b_+12, b_+15); writeFlagsTocddb_hook(gb);
}

void moveLinkToPosition_hook(GB *gb) {
  BASE(moveLinkToPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+3); A = 0x08;
  CALL_C(b_+3, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+6);
  CYC(b_+6, b_+8); L = 0x02;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x05);
  CYC(b_+10, b_+12); L = 0x03;
  CYC(b_+12, b_+13); SET_AF(pop_effect(gb));
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void interaction6b_checkGotBombsFromAmbi_hook(GB *gb) {
  BASE(interaction6b_checkGotBombsFromAmbi);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, (wGroup0RoomFlags + 131));
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  CYC(b_+5, b_+8); writeFlagsTocddb_hook(gb);
}

void interaction6b_checkLinkCanCollect_hook(GB *gb) {
  BASE(interaction6b_checkLinkCanCollect);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_zh);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); C = 0x0e;
  CALL_C(b_+13, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  if (F & FC) { CYCT(b_+18, b_+20); goto L_6bdd; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); alu_xor(gb, A);
L_6bdd:
  CYC(b_+21, b_+23); E = 0x78;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); ret_effect(gb);
}

void interaction6b_refillBombs_hook(GB *gb) {
  BASE(interaction6b_refillBombs);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wMaxBombs);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}
