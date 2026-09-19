#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

static void interaction6b_isLinkWithinBox_from_call(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x6ba4, 0x6ba5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6ba5, 0x6ba7); alu_sub(gb, 0x22);
  CYC(0x6ba7, 0x6ba9); alu_cp(gb, 0x54);
  if (!(F & FC)) { CYCT(0x6ba9, 0x6baa); ret_effect(gb); return; }
  CYC(0x6ba9, 0x6baa);
  CYC(0x6baa, 0x6bab); L = alu_inc8(gb, L);
  CYC(0x6bab, 0x6bac); A = mem_rd(gb, HL);
  CYC(0x6bac, 0x6bae); alu_sub(gb, 0x14);
  CYC(0x6bae, 0x6bb0); alu_cp(gb, 0x84);
  CYC(0x6bb0, 0x6bb1); ret_effect(gb);
}

void interaction6b_loadMoblinsAttackingMakuSprout_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b84, 0x6b87); SET_HL(0x78a9);
  CYC(0x6b87, 0x6b8a); parseGivenObjectData_b00_hook(gb);
}

void interaction6b_layoutSwapMakuTreeRooms_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b8a, 0x6b8d); SET_HL(0xc738);
  CYC(0x6b8d, 0x6b8f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  CYC(0x6b8f, 0x6b92); SET_HL(0xc848);
  CYC(0x6b92, 0x6b94); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(0x6b94, 0x6b95); ret_effect(gb);
}

void interaction6b_isLinkAtScreenEdge_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b95, 0x6b98); SET_HL(0xd00b);
  CYC(0x6b98, 0x6b9b); interaction6b_isLinkWithinBox_from_call(gb, 0x6b9b);
  CYC(0x6b9b, 0x6b9d); A = 0x01;
  if (!(F & FC)) { CYCT(0x6b9d, 0x6b9f); goto L_6ba0; }
  CYC(0x6b9d, 0x6b9f);
  CYC(0x6b9f, 0x6ba0); alu_xor(gb, A);
L_6ba0:
  CYC(0x6ba0, 0x6ba1); alu_or(gb, A);
  CYC(0x6ba1, 0x6ba4); writeFlagsTocddb_hook(gb);
}

void moveLinkToPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bb1, 0x6bb2); push_effect(gb, AF);
  CYC(0x6bb2, 0x6bb4); A = 0x08;
  CALL_C(0x6bb4, setLinkIDOverride_hook, 0x2acf, 0x6bb7);
  CYC(0x6bb7, 0x6bb9); L = 0x02;
  CYC(0x6bb9, 0x6bbb); mem_wr(gb, HL, 0x05);
  CYC(0x6bbb, 0x6bbd); L = 0x03;
  CYC(0x6bbd, 0x6bbe); SET_AF(pop_effect(gb));
  CYC(0x6bbe, 0x6bbf); mem_wr(gb, HL, A);
  CYC(0x6bbf, 0x6bc0); ret_effect(gb);
}

void interaction6b_checkGotBombsFromAmbi_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bc0, 0x6bc3); A = mem_rd(gb, 0xc783);
  CYC(0x6bc3, 0x6bc5); alu_bit(gb, 7, A);
  CYC(0x6bc5, 0x6bc8); writeFlagsTocddb_hook(gb);
}

void interaction6b_checkLinkCanCollect_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bc8, 0x6bcb); SET_HL(0xd00f);
  CYC(0x6bcb, 0x6bcc); A = mem_rd(gb, HL);
  CYC(0x6bcc, 0x6bcd); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6bcd, 0x6bce); ret_effect(gb); return; }
  CYC(0x6bcd, 0x6bce);
  CYC(0x6bce, 0x6bd1); A = mem_rd(gb, 0xcc5a);
  CYC(0x6bd1, 0x6bd2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6bd2, 0x6bd3); ret_effect(gb); return; }
  CYC(0x6bd2, 0x6bd3);
  CYC(0x6bd3, 0x6bd5); C = 0x0e;
  CALL_C(0x6bd5, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6bd8);
  CYC(0x6bd8, 0x6bda); A = 0x01;
  if (F & FC) { CYCT(0x6bda, 0x6bdc); goto L_6bdd; }
  CYC(0x6bda, 0x6bdc);
  CYC(0x6bdc, 0x6bdd); alu_xor(gb, A);
L_6bdd:
  CYC(0x6bdd, 0x6bdf); E = 0x78;
  CYC(0x6bdf, 0x6be0); mem_wr(gb, DE, A);
  CYC(0x6be0, 0x6be1); ret_effect(gb);
}

void interaction6b_refillBombs_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6be1, 0x6be4); SET_HL(0xc6b1);
  CYC(0x6be4, 0x6be5); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6be5, 0x6be6); mem_wr(gb, HL, A);
  CYC(0x6be6, 0x6be7); ret_effect(gb);
}
