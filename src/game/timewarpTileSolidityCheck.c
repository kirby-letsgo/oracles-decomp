#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void checkSolidObjectAtWarpDestPos_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62c4, 0x62c6); A = 0x02;
  CYC(0x62c6, 0x62c8); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x62c8, 0x62cb); A = W8(wWarpDestPos);
  CYC(0x62cb, 0x62ce); SET_HL(w2SolidObjectPositions);
  CALL_C(0x62ce, checkFlag_hook, 0x0205, 0x62d1);
  CYC(0x62d1, 0x62d3); C = 0x00;
  if (F & FZ) {
    CYCT(0x62d3, 0x62d5);
  } else {
    CYC(0x62d3, 0x62d5);
    CYC(0x62d5, 0x62d6); C = alu_inc8(gb, C);
  }
  CYC(0x62d6, 0x62d7); alu_xor(gb, A);
  CYC(0x62d7, 0x62d9); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x62d9, 0x62da); ret_effect(gb);
}

void clearSolidObjectPositions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62da, 0x62dc); A = 0x02;
  CYC(0x62dc, 0x62de); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x62de, 0x62e0); B = 0x10;
  CYC(0x62e0, 0x62e3); SET_HL(w2SolidObjectPositions);
  CALL_C(0x62e3, clearMemory_hook, 0x046f, 0x62e6);
  CYC(0x62e6, 0x62e8); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(0x62e8, 0x62e9); ret_effect(gb);
}

void checkLinkCanStandOnTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62e9, 0x62ec); A = W8(w1Link_yh);
  CYC(0x62ec, 0x62ed); B = A;
  CYC(0x62ed, 0x62f0); A = W8(w1Link_xh);
  CYC(0x62f0, 0x62f1); C = A;
  CYC(0x62f1, 0x62f4); SET_HL(0x5e92);
  CYC(0x62f4, 0x62f6); E = 0x05;
  CALL_C(0x62f6, interBankCall_hook, 0x008a, 0x62f9);
  CYC(0x62f9, 0x62fb); B = alu_rl(gb, B);
  if (F & FC) {
    CYCT(0x62fb, 0x62fd);
    goto invalid_tile;
  }
  CYC(0x62fb, 0x62fd);
  CALL_C(0x62fd, objectGetTileAtPosition_hook, 0x1444, 0x6300);
  CYC(0x6300, 0x6301); E = mem_rd(gb, HL);
  CYC(0x6301, 0x6304); SET_HL(0x6317);
  CALL_C(0x6304, lookupKey_hook, 0x1e06, 0x6307);
  if (F & FC) {
    CYCT(0x6307, 0x6309);
  } else {
    CYC(0x6307, 0x6309);
    CYC(0x6309, 0x630b); C = 0x00;
    CYC(0x630b, 0x630c); ret_effect(gb);
    return;
  }
  CYC(0x630c, 0x630d); alu_or(gb, A);
  CYC(0x630d, 0x630f); A = 0x4a;
  if (!(F & FZ)) {
    CALL_C_CC(0x630f, checkTreasureObtained_hook, 0x1748, 0x6312);
  } else {
    CYC(0x630f, 0x6312);
  }
  if (F & FC) {
    CYCT(0x6312, 0x6314);
    CYC(0x6309, 0x630b); C = 0x00;
    CYC(0x630b, 0x630c); ret_effect(gb);
    return;
  }
  CYC(0x6312, 0x6314);

invalid_tile:
  CYC(0x6314, 0x6316); C = 0x01;
  CYC(0x6316, 0x6317); ret_effect(gb);
}
