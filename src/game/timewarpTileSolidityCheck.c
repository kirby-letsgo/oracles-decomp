#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(checkSolidObjectAtWarpDestPos), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(checkSolidObjectAtWarpDestPos), (from), (to), true)

void checkSolidObjectAtWarpDestPos_hook(GB *gb) {
  BASE(checkSolidObjectAtWarpDestPos);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+4, b_+7); A = W8(wWarpDestPos);
  CYC(b_+7, b_+10); SET_HL(w2SolidObjectPositions);
  CALL_C(b_+10, checkFlag_hook, SYM(checkFlag), b_+13);
  CYC(b_+13, b_+15); C = 0x00;
  if (F & FZ) {
    CYCT(b_+15, b_+17);
  } else {
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+18); C = alu_inc8(gb, C);
  }
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+21); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+21, SYM(clearSolidObjectPositions)); ret_effect(gb);
}

void clearSolidObjectPositions_hook(GB *gb) {
  BASE(clearSolidObjectPositions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+4, b_+6); B = 0x10;
  CYC(b_+6, b_+9); SET_HL(w2SolidObjectPositions);
  CALL_C(b_+9, clearMemory_hook, SYM(clearMemory), b_+12);
  CYC(b_+12, b_+14); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+14, SYM(checkLinkCanStandOnTile)); ret_effect(gb);
}

void checkLinkCanStandOnTile_hook(GB *gb) {
  BASE(checkLinkCanStandOnTile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(w1Link_yh);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(w1Link_xh);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); SET_HL((SYM(checkDisplayEraOrSeasonInfo) + 21));
  CYC(b_+11, b_+13); E = 0x05;
  CALL_C(b_+13, interBankCall_hook, 0x008a, b_+16);
  CYC(b_+16, b_+18); B = alu_rl(gb, B);
  if (F & FC) {
    CYCT(b_+18, b_+20);
    goto invalid_tile;
  }
  CYC(b_+18, b_+20);
  CALL_C(b_+20, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+23);
  CYC(b_+23, b_+24); E = mem_rd(gb, HL);
  CYC(b_+24, b_+27); SET_HL(SYM(invalidTimewarpTileList));
  CALL_C(b_+27, lookupKey_hook, SYM(lookupKey), b_+30);
  if (F & FC) {
    CYCT(b_+30, b_+32);
  } else {
    CYC(b_+30, b_+32);
    CYC(b_+32, b_+34); C = 0x00;
    CYC(b_+34, b_+35); ret_effect(gb);
    return;
  }
  CYC(b_+35, b_+36); alu_or(gb, A);
  CYC(b_+36, b_+38); A = 0x4a;
  if (!(F & FZ)) {
    CALL_C_CC(b_+38, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+41);
  } else {
    CYC(b_+38, b_+41);
  }
  if (F & FC) {
    CYCT(b_+41, b_+43);
    CYC(b_+32, b_+34); C = 0x00;
    CYC(b_+34, b_+35); ret_effect(gb);
    return;
  }
  CYC(b_+41, b_+43);

invalid_tile:
  CYC(b_+43, b_+45); C = 0x01;
  CYC(b_+45, SYM(invalidTimewarpTileList)); ret_effect(gb);
}
