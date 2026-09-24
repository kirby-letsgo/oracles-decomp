#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/bank3Cutscenes.s, the Seasons-only pieces. The shared states are in
// src/game/bank3Cutscenes.c.

// Seasons splits Ages' introCinematic_inTemple_state1 in two: state 1 only waits for the
// palette fade, this state walks Link up to the triforce on simulated input.
void s_introCinematic_inTemple_state1_5_hook(GB *gb) {
  BASE(introCinematic_inTemple_state1_5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wUseSimulatedInput);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+7); TAIL(introCinematic_inTemple_updateCamera); }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(b_+11, introCinematic_inTemple_updateCamera_hook, SYM(introCinematic_inTemple_updateCamera), b_+14);
  CYC(b_+14, b_+17);
  TAIL(intro_incState);
}

// Draw the sprites that complement the image of Link on the horse (the 1st image)
void s_introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1_hook(GB *gb) {
  BASE(introCinematic_ridingHorse_drawLinkOnHorseCloseupSprites_1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wGfxRegs2_SCY);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_cpl(gb);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_cpl(gb);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+14); mem_wr(gb, hOamTail, A);
  CYC(b_+14, b_+17); SET_HL(SYM(linkOnHorseCloseupSprites_1));
  CYC(b_+17, b_+20);
  TAIL(addSpritesToOam_withOffset);
}

// seasonsFunc_03_5367@func
static void bank3Cutscenes_d_func(GB *gb) {
  BASE(seasonsFunc_03_5367);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+25, b_+28); A = mem_rd(gb, wTmpcbb6);
  CYC(b_+28, b_+29); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto L_5394; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wTmpcbba);
  CYC(b_+34, b_+36); alu_xor(gb, 0x01);
  CYC(b_+36, b_+39); mem_wr(gb, wTmpcbba, A);
  CYC(b_+39, b_+41); A = 0x05;
  if (F & FZ) { CYCT(b_+41, b_+43); goto L_5394; }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); A = 0x22;
L_5394:
  CYC(b_+45, b_+48); mem_wr(gb, wTmpcbb6, A);
  CYC(b_+48, b_+51); A = mem_rd(gb, wTmpcbba);
  CYC(b_+51, b_+52); alu_or(gb, A);
  RET(b_+52); return;
}

void s_seasonsFunc_03_5367_hook(GB *gb) {
  BASE(seasonsFunc_03_5367);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_L(b_+0, bank3Cutscenes_d_func, b_+3);
  CYC(b_+3, b_+6); SET_BC(0x0506);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_5372; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_BC(0x0000);
L_5372:
  CYC(b_+11, b_+14); SET_HL(wTmpcbbb);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+19); mem_wr(gb, wGfxRegs2_SCY, A);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+24); mem_wr(gb, wGfxRegs2_SCX, A);
  RET(b_+24); return;
}

