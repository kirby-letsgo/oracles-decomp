#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void checkUpdateUnderwaterWaves_hook(GB *gb);

void checkInitUnderwaterWaves_hook(GB *gb) {
  BASE(checkInitUnderwaterWaves);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x10;
  CYC(b_+8, b_+11); W8(wGfxRegs2_LYC) = A;
  CYC(b_+11, b_+13); A = 0x02;
  CYC(b_+13, b_+15); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+15, b_+17); A = 0x02;
  CALL_C(b_+17, initWaveScrollValues_hook, SYM(initWaveScrollValues), SYM(checkUpdateUnderwaterWaves));
  checkUpdateUnderwaterWaves_hook(gb);
}

void checkUpdateUnderwaterWaves_hook(GB *gb) {
  BASE(checkUpdateUnderwaterWaves);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+10); hram_wr(gb, 0x70, A);
  CYC(b_+10, b_+13); A = W8(wGfxRegs2_SCX);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+17); A = W8(wFrameCounter);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+21); A = W8(wGfxRegs2_SCY);
  CYC(b_+21, b_+22); alu_add(gb, B);
  CYC(b_+22, b_+24); alu_and(gb, 0x7f);
  CYC(b_+24, b_+27); SET_DE(w2WaveScrollValues);
  CALL_C(b_+27, addAToDe_hook, 0x0068, b_+30);
  CYC(b_+30, b_+33); SET_HL(wBigBuffer + 0x10);
  CYC(b_+33, b_+35); B = 0x80;
  for (;;) {
    CYC(b_+35, b_+36); A = mem_rd(gb, DE);
    CYC(b_+36, b_+37); alu_add(gb, C);
    CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+38, b_+39); A = E;
    CYC(b_+39, b_+40); A = alu_inc8(gb, A);
    CYC(b_+40, b_+42); alu_and(gb, 0x7f);
    CYC(b_+42, b_+43); E = A;
    CYC(b_+43, b_+44); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+44, b_+46);
      continue;
    }
    CYC(b_+44, b_+46);
    break;
  }
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CYC(b_+47, b_+49); hram_wr(gb, 0x70, A);
  CYC(b_+49, b_+50); ret_effect(gb);
}

void checkDisableUnderwaterWaves_hook(GB *gb) {
  BASE(checkDisableUnderwaterWaves);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x03;
  CYC(b_+8, b_+10); mem_wr(gb, hNextLcdInterruptBehaviour, A);
  CYC(b_+10, b_+12); A = 0xc7;
  CYC(b_+12, b_+15); W8(wGfxRegs2_LYC) = A;
  CYC(b_+15, b_+16); ret_effect(gb);
}
