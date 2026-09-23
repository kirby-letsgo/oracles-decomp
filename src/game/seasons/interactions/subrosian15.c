#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SUBROSIAN), bank 0x15.

// subrosianFunc_58b1 from its +3: stores this object's slot ($58xx) at hl.
static void subrosian_store_slot(GB *gb) {
  BASE(subrosianFunc_58b1);
  CYC(b_+3, b_+4); mem_wr(gb, HL, D);
  CYC(b_+4, b_+5); SET_HL(HL + 1);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x58);
  RET(b_+7); return;
}

void s_subrosianFunc_58ac_hook(GB *gb) {
  BASE(subrosianFunc_58ac);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x1e);
  CYCT(b_+3, b_+5); subrosian_store_slot(gb); return;
}

void s_subrosianFunc_58b1_hook(GB *gb) {
  BASE(subrosianFunc_58b1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0 + 0x1c);
  subrosian_store_slot(gb); return;
}

void s_subrosian_knockLinkOut_hook(GB *gb) {
  BASE(subrosian_knockLinkOut);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x10;
  CYC(b_+2, b_+5); mem_wr(gb, wcc50, A);
  CYC(b_+5, b_+8); SET_HL(w1Link_direction);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x03);
  RET(b_+10); return;
}

void s_subrosianFunc_58c4_hook(GB *gb) {
  BASE(subrosianFunc_58c4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+5); E = 0x4b; // Interaction.yh
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6); return;
}

void s_subrosian_setYAboveLink_hook(GB *gb) {
  BASE(subrosian_setYAboveLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+5); alu_sub(gb, 0x08);
  CYC(b_+5, b_+7); E = 0x4b; // Interaction.yh
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

// subrosianFunc_58dc from its +6: counter2 = wTmpcfc0+1 + c - xh.
static void subrosian_counter2(GB *gb) {
  BASE(subrosianFunc_58dc);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTmpcfc0 + 0x01);
  CYC(b_+9, b_+10); alu_add(gb, C);
  CYC(b_+10, b_+11); alu_sub(gb, B);
  CYC(b_+11, b_+13); E = 0x47; // Interaction.counter2
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return;
}

void s_subrosianFunc_58d4_hook(GB *gb) {
  BASE(subrosianFunc_58d4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x4d; // Interaction.xh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); C = 0xf2;
  CYCT(b_+6, b_+8); subrosian_counter2(gb); return;
}

void s_subrosianFunc_58dc_hook(GB *gb) {
  BASE(subrosianFunc_58dc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x4d; // Interaction.xh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); C = 0x0e;
  subrosian_counter2(gb); return;
}

void s_subrosian_giveFoolsOre_hook(GB *gb) {
  BASE(subrosian_giveFoolsOre);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x1e; // TREASURE_FOOLS_ORE
  CYC(b_+2, b_+5);
  TAIL(giveTreasure);
}

// wTmpcfc0 = the signs-destroyed tier (0, 1, 20, 50, 90, 100 signs) and the count as decimal
// text.
void s_subrosian_checkSignsDestroyed_hook(GB *gb) {
  BASE(subrosian_checkSignsDestroyed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTotalSignsDestroyed);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); alu_or(gb, A);
  CYC(b_+5, b_+7); C = 0x00;
  if (F & FZ) { CYCT(b_+7, b_+9); goto tier; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); C = alu_inc8(gb, C);
  CYC(b_+10, b_+12); alu_cp(gb, 0x14);
  if (F & FC) { CYCT(b_+12, b_+14); goto tier; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); C = alu_inc8(gb, C);
  CYC(b_+15, b_+17); alu_cp(gb, 0x32);
  if (F & FC) { CYCT(b_+17, b_+19); goto tier; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); C = alu_inc8(gb, C);
  CYC(b_+20, b_+22); alu_cp(gb, 0x5a);
  if (F & FC) { CYCT(b_+22, b_+24); goto tier; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); C = alu_inc8(gb, C);
  CYC(b_+25, b_+27); alu_cp(gb, 0x64);
  if (F & FC) { CYCT(b_+27, b_+29); goto tier; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); C = alu_inc8(gb, C);
tier:
  CYC(b_+30, b_+31); A = C;
  CYC(b_+31, b_+34); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+34, b_+35); A = B;
  CALL_C(b_+35, hexToDec_hook, SYM(hexToDec), b_+38);
  CYC(b_+38, b_+40); C = alu_swap(gb, C);
  CYC(b_+40, b_+41); alu_or(gb, C);
  CYC(b_+41, b_+44); mem_wr(gb, wTextNumberSubstitution, A);
  CYC(b_+44, b_+45); A = B;
  CYC(b_+45, b_+48); mem_wr(gb, wTextNumberSubstitution + 1, A);
  RET(b_+48); return;
}

void s_subrosian_giveSignRing_hook(GB *gb) {
  BASE(subrosian_giveSignRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x3700); // SIGN_RING
  CYC(b_+3, b_+6);
  TAIL(giveRingToLink);
}

void s_subrosian_fakeReset_hook(GB *gb) {
  BASE(subrosian_fakeReset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x09;
  CYC(b_+2, b_+5);
  TAIL(openMenu);
}

// counter2 = xh / 2 + $10.
void s_subrosianFunc_5968_hook(GB *gb) {
  BASE(subrosianFunc_5968);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x4d; // Interaction.xh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_srl(gb, A);
  CYC(b_+5, b_+7); alu_add(gb, 0x10);
  CYC(b_+7, b_+9); E = 0x47; // Interaction.counter2
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}
