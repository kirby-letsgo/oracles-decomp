#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void checkUpdateUnderwaterWaves_hook(GB *gb);

void checkInitUnderwaterWaves_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x626e, 0x6271); A = W8(wTilesetFlags);
  CYC(0x6271, 0x6273); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6273, 0x6274); ret_effect(gb);
    return;
  }
  CYC(0x6273, 0x6274);
  CYC(0x6274, 0x6276); A = 0x10;
  CYC(0x6276, 0x6279); W8(wGfxRegs2_LYC) = A;
  CYC(0x6279, 0x627b); A = 0x02;
  CYC(0x627b, 0x627d); hram_wr(gb, 0x9d, A);
  CYC(0x627d, 0x627f); A = 0x02;
  CALL_C(0x627f, initWaveScrollValues_hook, 0x1384, 0x6282);
  checkUpdateUnderwaterWaves_hook(gb);
}

void checkUpdateUnderwaterWaves_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6282, 0x6285); A = W8(wTilesetFlags);
  CYC(0x6285, 0x6287); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6287, 0x6288); ret_effect(gb);
    return;
  }
  CYC(0x6287, 0x6288);
  CYC(0x6288, 0x628a); A = 0x02;
  CYC(0x628a, 0x628c); hram_wr(gb, 0x70, A);
  CYC(0x628c, 0x628f); A = W8(wGfxRegs2_SCX);
  CYC(0x628f, 0x6290); C = A;
  CYC(0x6290, 0x6293); A = W8(wFrameCounter);
  CYC(0x6293, 0x6294); B = A;
  CYC(0x6294, 0x6297); A = W8(wGfxRegs2_SCY);
  CYC(0x6297, 0x6298); alu_add(gb, B);
  CYC(0x6298, 0x629a); alu_and(gb, 0x7f);
  CYC(0x629a, 0x629d); SET_DE(w2WaveScrollValues);
  CALL_C(0x629d, addAToDe_hook, 0x0068, 0x62a0);
  CYC(0x62a0, 0x62a3); SET_HL(wBigBuffer + 0x10);
  CYC(0x62a3, 0x62a5); B = 0x80;
  for (;;) {
    CYC(0x62a5, 0x62a6); A = mem_rd(gb, DE);
    CYC(0x62a6, 0x62a7); alu_add(gb, C);
    CYC(0x62a7, 0x62a8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x62a8, 0x62a9); A = E;
    CYC(0x62a9, 0x62aa); A = alu_inc8(gb, A);
    CYC(0x62aa, 0x62ac); alu_and(gb, 0x7f);
    CYC(0x62ac, 0x62ad); E = A;
    CYC(0x62ad, 0x62ae); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x62ae, 0x62b0);
      continue;
    }
    CYC(0x62ae, 0x62b0);
    break;
  }
  CYC(0x62b0, 0x62b1); alu_xor(gb, A);
  CYC(0x62b1, 0x62b3); hram_wr(gb, 0x70, A);
  CYC(0x62b3, 0x62b4); ret_effect(gb);
}

void checkDisableUnderwaterWaves_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62b4, 0x62b7); A = W8(wTilesetFlags);
  CYC(0x62b7, 0x62b9); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x62b9, 0x62ba); ret_effect(gb);
    return;
  }
  CYC(0x62b9, 0x62ba);
  CYC(0x62ba, 0x62bc); A = 0x03;
  CYC(0x62bc, 0x62be); hram_wr(gb, 0x9d, A);
  CYC(0x62be, 0x62c0); A = 0xc7;
  CYC(0x62c0, 0x62c3); W8(wGfxRegs2_LYC) = A;
  CYC(0x62c3, 0x62c4); ret_effect(gb);
}
