#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MAKU_CUTSCENES), bank 0x15.

// Fades to black, then marks every background palette and sprite palette 0 for fading.
void s_seasonsFunc_15_571a_hook(GB *gb) {
  BASE(seasonsFunc_15_571a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, fadeoutToBlackWithDelay_hook, SYM(fadeoutToBlackWithDelay), b_+3);
  CYCT(b_+3, b_+5);
  CYC(b_+8, b_+10); A = 0xff;
  CYC(b_+10, b_+13); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+13, b_+16); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+21); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+21, b_+23); A = 0xfe;
  CYC(b_+23, b_+26); mem_wr(gb, wFadeSprPaletteSources, A);
  RET(b_+26); return;
}

// TX_17xx, low byte a (+$1b in a linked game).
void s_seasonsFunc_15_5735_hook(GB *gb) {
  BASE(seasonsFunc_15_5735);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CALL_C(b_+1, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+4);
  if (F & FZ) { CYCT(b_+4, b_+6); goto show; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = C;
  CYC(b_+7, b_+9); alu_add(gb, 0x1b);
  CYC(b_+9, b_+10); C = A;
show:
  CYC(b_+10, b_+12); B = 0x17;
  CYC(b_+12, b_+15);
  TAIL(showText);
}

// seasonsFunc_15_5748 from its +2: var3f = 1 when var3e has the bit in a, else 0.
static void maku_test_var3e(GB *gb) {
  BASE(seasonsFunc_15_5748);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0x7e; // Interaction.var3e
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_and(gb, B);
  CYC(b_+7, b_+9); L = 0x7f; // Interaction.var3f
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x00);
  RET(b_+14); return;
}

void s_seasonsFunc_15_5744_hook(GB *gb) {
  BASE(seasonsFunc_15_5744);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x10;
  CYCT(b_+2, b_+4); maku_test_var3e(gb); return;
}

void s_seasonsFunc_15_5748_hook(GB *gb) {
  BASE(seasonsFunc_15_5748);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x08;
  maku_test_var3e(gb); return;
}

// Subid 4 in a linked game spawns the linked cutscene object (subid 4) at ($28,$58).
void s_seasonsFunc_15_576c_hook(GB *gb) {
  BASE(seasonsFunc_15_576c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42; // Interaction.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+9);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+13);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0xb3); // INTERAC_LINKED_CUTSCENE
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x04);
  CYC(b_+19, b_+21); L = 0x4a; // Interaction.y
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x28);
  CYC(b_+23, b_+25); L = 0x4c; // Interaction.x
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x58);
  RET(b_+27); return;
}
