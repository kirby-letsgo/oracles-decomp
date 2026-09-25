#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/interactableTiles.s.

void s_nextToSubrosiaKeydoor_hook(GB *gb) {
  BASE(nextToSubrosiaKeydoor);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_specialObjectCheckPushingAgainstTile, SYM(specialObjectCheckPushingAgainstTile), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); TAIL(resetPushingAgainstTileCounter); }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, s_checkFacingBottomOfTile, SYM(checkFacingBottomOfTile), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_4264; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  RET(b_+12); return;
L_4264:
  CALL_C(b_+13, s_decPushingAgainstTileCounter, SYM(decPushingAgainstTileCounter), b_+16);
  if (F & FZ) { CYCT(b_+16, b_+18); goto L_426b; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
L_426b:
  CYC(b_+20, b_+22); A = 0x0b;
  CALL_C(b_+22, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+27); TAIL(jumpToShowInfoText); }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = 0x6c;
  CALL_C(b_+29, s_playSound, SYM(playSound_b00), b_+32);
  CALL_C(b_+32, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+35);
  CYC(b_+35, b_+37); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+37, b_+39); A = 0xe8;
  CALL_C(b_+39, s_setTile, SYM(setTile), b_+42);
  CALL_C(b_+42, s_createKeySpriteInteraction, SYM(createKeySpriteInteraction), b_+45);
  CYC(b_+45, b_+46); alu_scf(gb);
  RET(b_+46); return;
}

