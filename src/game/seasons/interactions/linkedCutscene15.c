#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_LINKED_CUTSCENE), bank 0x15.

// seasonsFunc_15_6334 from its +3: resets the palette thread's fade parameters.
static void linked_reset_fade(GB *gb) {
  BASE(seasonsFunc_15_6334);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+7); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+7, b_+10); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+10, b_+12); A = 0x7e;
  CYC(b_+12, b_+15); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+15, b_+18); mem_wr(gb, wFadeBgPaletteSources, A);
  RET(b_+18); return;
}

void s_seasonsFunc_15_632f_hook(GB *gb) {
  BASE(seasonsFunc_15_632f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_darkenRoom, SYM(darkenRoom), b_+3);
  CYCT(b_+3, b_+5); linked_reset_fade(gb); return;
}

void s_seasonsFunc_15_6334_hook(GB *gb) {
  BASE(seasonsFunc_15_6334);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_brightenRoom, SYM(brightenRoom), b_+3);
  linked_reset_fade(gb); return;
}

// seasonsFunc_15_634c from its +3: a lightning part (subid 1) at (b,c).
static void linked_lightning(GB *gb) {
  BASE(seasonsFunc_15_634c);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+3, getFreePartSlot_hook, SYM(getFreePartSlot), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = 0xcb; // Part.yh
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);
  CYC(b_+14, b_+16); L = 0xcd; // Part.xh
  CYC(b_+16, b_+17); mem_wr(gb, HL, C);
  RET(b_+17); return;
}

void s_seasonsFunc_15_6347_hook(GB *gb) {
  BASE(seasonsFunc_15_6347);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x5838);
  CYCT(b_+3, b_+5); linked_lightning(gb); return;
}

void s_seasonsFunc_15_634c_hook(GB *gb) {
  BASE(seasonsFunc_15_634c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x1850);
  linked_lightning(gb); return;
}

// seasonsFunc_15_6363 from its +3: an INTERAC_bf with subid and position from the table at bc.
static void linked_spawn_bf(GB *gb) {
  BASE(seasonsFunc_15_6363);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+3, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xbf); // INTERAC_bf
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+12); SET_BC(BC + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYCT(b_+13, b_+15);
  TAIL_S(seasonsFunc_15_6396);
}

void s_seasonsFunc_15_635e_hook(GB *gb) {
  BASE(seasonsFunc_15_635e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(seasonsTable_15_6372));
  CYCT(b_+3, b_+5); linked_spawn_bf(gb); return;
}

void s_seasonsFunc_15_6363_hook(GB *gb) {
  BASE(seasonsFunc_15_6363);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(seasonsTable_15_6375));
  linked_spawn_bf(gb); return;
}

void s_seasonsFunc_15_6378_hook(GB *gb) {
  BASE(seasonsFunc_15_6378);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+5, b_+7); A = 0xb4; // INTERAC_b4
  CYC(b_+7, b_+10); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  RET(b_+10); return;
}

// Both INTERAC_b4 objects, from seasonsTable_15_63a0 and _63a3.
void s_seasonsFunc_15_6383_hook(GB *gb) {
  BASE(seasonsFunc_15_6383);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(seasonsTable_15_63a0));
  CALL_C(b_+3, s_seasonsFunc_15_638c_hook, SYM(seasonsFunc_15_638c), b_+6);
  CYC(b_+6, b_+9); SET_BC(SYM(seasonsTable_15_63a3));
  TAIL_S(seasonsFunc_15_638c);
}

// An INTERAC_b4 with subid and position from the table at bc.
void s_seasonsFunc_15_638c_hook(GB *gb) {
  BASE(seasonsFunc_15_638c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xb4); // INTERAC_b4
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); SET_BC(BC + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  TAIL_S(seasonsFunc_15_6396);
}

// yh and xh of the object at hl from the two bytes at bc.
void s_seasonsFunc_15_6396_hook(GB *gb) {
  BASE(seasonsFunc_15_6396);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x4b; // Interaction.yh
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+4); SET_BC(BC + 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+7); L = 0x4d; // Interaction.xh
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  RET(b_+9); return;
}

// The Maku cutscene object (subid 9) at ($40,$50).
void s_seasonsFunc_15_63a6_hook(GB *gb) {
  BASE(seasonsFunc_15_63a6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x22); // INTERAC_MAKU_CUTSCENES
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x09);
  CYC(b_+9, b_+11); L = 0x4b; // Interaction.yh
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x40);
  CYC(b_+13, b_+15); L = 0x4d; // Interaction.xh
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x50);
  RET(b_+17); return;
}
