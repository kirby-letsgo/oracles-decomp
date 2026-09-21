#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void partCode09_hook(GB *gb);
void button_state0_hook(GB *gb);
void button_updateTileBeforeDeletion_hook(GB *gb);

void partCode09_hook(GB *gb) {
  BASE(partCode09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(2)); E = 0xc4; // Part.state
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(4), b_+OE(7)); // call z
    button_state0_hook(gb);
  } else {
    CYC(b_+O(4), b_+OE(7));
  }

  CYC(b_+O(7), b_+OE(10)); A = mem_rd(gb, wccb1); // wccb1
  CYC(b_+O(10), b_+OE(11)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+O(11)); return; } // ret nz
  CYC(b_+O(11), b_+OE(12));
  CYC(b_+O(12), b_+OE(15)); SET_HL(w1Link); // w1Link
  CALL_C(b_+O(15), checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+OE(18));
  if (F & FC) { CYCT(b_+O(18), b_+OE(20)); goto linkTouchedButton; } // jr c
  CYC(b_+O(18), b_+OE(20));
  if (game_seasons) {
    CYC(b_+S(20), b_+S(23)); SET_HL(w1Companion);
    CALL_C(b_+S(23), checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+S(26));
    if (F & FC) { CYCT(b_+S(26), b_+S(28)); goto companionTouchedButton; }
    CYC(b_+S(26), b_+S(28));
  }
  CALL_C(b_+O(20), objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+OE(23));
  CYC(b_+O(23), b_+OE(25)); alu_sub(gb, 0x0c); // TILEINDEX_BUTTON
  CYC(b_+O(25), b_+OE(27)); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+O(27), b_+OE(29)); goto somethingOnButton; } // jr nc
  CYC(b_+O(27), b_+OE(29));
  CALL_C(b_+O(29), partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+OE(32));
  if (!(F & FZ)) { RET_TAKEN(b_+O(32)); return; } // ret nz
  CYC(b_+O(32), b_+OE(33));
  CYC(b_+O(33), b_+OE(35)); L = 0xf0; // Part.var30
  CYC(b_+O(35), b_+OE(37)); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+O(37)); return; } // ret z
  CYC(b_+O(37), b_+OE(38));
  CYC(b_+O(38), b_+OE(40)); E = 0xf0; // Part.var30
  CYC(b_+O(40), b_+OE(41)); A = mem_rd(gb, DE);
  CYC(b_+O(41), b_+OE(42)); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+O(42)); return; } // ret z
  CYC(b_+O(42), b_+OE(43));
  CALL_C(b_+O(43), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(46));
  CYC(b_+O(46), b_+OE(47)); C = A;
  CYC(b_+O(47), b_+OE(49)); A = 0x0c; // TILEINDEX_BUTTON
  CALL_C(b_+O(49), setTile_hook, SYM(setTile), b_+OE(52));
  CYC(b_+O(52), b_+OE(54)); E = 0xc3; // Part.var03
  CYC(b_+O(54), b_+OE(55)); A = mem_rd(gb, DE);
  CYC(b_+O(55), b_+OE(58)); SET_HL(wActiveTriggers); // wActiveTriggers
  CALL_C(b_+O(58), unsetFlag_hook, SYM(unsetFlag), b_+OE(61));
  CYC(b_+O(61), b_+OE(63)); E = 0xf0; // Part.var30
  CYC(b_+O(63), b_+OE(64)); alu_xor(gb, A);
  CYC(b_+O(64), b_+OE(65)); mem_wr(gb, DE, A);
  CYC(b_+O(65), b_+OE(67)); A = 0x87; // SND_SPLASH
  CYC(b_+O(67), b_+OE(70)); TAIL(playSound_b00); // jp

somethingOnButton:
  CYC(b_+O(70), b_+OE(71)); H = D;
  CYC(b_+O(71), b_+OE(73)); L = 0xc2; // Part.subid
  CYC(b_+O(73), b_+OE(75)); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(75), b_+OE(77)); goto delete; } // jr z
  CYC(b_+O(75), b_+OE(77));
  CYC(b_+O(77), b_+OE(79)); L = 0xf0; // Part.var30
  CYC(b_+O(79), b_+OE(81)); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+O(81)); return; } // ret nz
  CYC(b_+O(81), b_+OE(82));
  CYC(b_+O(82), b_+OE(84)); L = 0xc6; // Part.counter1
  CYC(b_+O(84), b_+OE(86)); mem_wr(gb, HL, 0x1c);
  CALL_C(b_+O(86), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(89));
  CYC(b_+O(89), b_+OE(90)); C = A;
  CYC(b_+O(90), b_+OE(92)); B = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+O(92), setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+OE(95));
  CYC(b_+O(95), b_+OE(97)); goto setTriggerAndPlaySound; // jr

delete:
  CYC(b_+O(97), b_+OE(100)); push_effect(gb, b_+OE(100)); button_updateTileBeforeDeletion_hook(gb);
  CYC(b_+O(100), b_+OE(103)); TAIL(partDelete); // jp

linkTouchedButton:
  CYC(b_+O(103), b_+OE(106)); A = mem_rd(gb, w1Link_zh); // w1Link.zh
  CYC(b_+O(106), b_+OE(107)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+O(107)); return; } // ret nz
  CYC(b_+O(107), b_+OE(108));

companionTouchedButton:
  CYC(b_+O(108), b_+OE(110)); E = 0xc2; // Part.subid
  CYC(b_+O(110), b_+OE(111)); A = mem_rd(gb, DE);
  CYC(b_+O(111), b_+OE(112)); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+O(112), b_+OE(114)); goto delete; } // jr nc
  CYC(b_+O(112), b_+OE(114));

  CYC(b_+O(114), b_+OE(116)); E = 0xf0; // Part.var30
  CYC(b_+O(116), b_+OE(117)); A = mem_rd(gb, DE);
  CYC(b_+O(117), b_+OE(118)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+O(118)); return; } // ret nz
  CYC(b_+O(118), b_+OE(119));
  CALL_C(b_+O(119), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(122));
  CYC(b_+O(122), b_+OE(123)); C = A;
  CYC(b_+O(123), b_+OE(125)); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+O(125), setTile_hook, SYM(setTile), b_+OE(128));

setTriggerAndPlaySound:
  CYC(b_+O(128), b_+OE(130)); E = 0xc3; // Part.var03
  CYC(b_+O(130), b_+OE(131)); A = mem_rd(gb, DE);
  CYC(b_+O(131), b_+OE(134)); SET_HL(wActiveTriggers); // wActiveTriggers
  CALL_C(b_+O(134), setFlag_hook, SYM(setFlag), b_+OE(137));
  CYC(b_+O(137), b_+OE(139)); E = 0xf0; // Part.var30
  CYC(b_+O(139), b_+OE(141)); A = 0x01;
  CYC(b_+O(141), b_+OE(142)); mem_wr(gb, DE, A);
  CYC(b_+O(142), b_+OE(144)); A = 0x87; // SND_SPLASH
  CYC(b_+O(144), b_+OE(147)); TAIL(playSound_b00); // jp
}

void button_state0_hook(GB *gb) {
  BASE(partCode09);
  CYC(b_+O(165), b_+OE(166)); H = D;
  CYC(b_+O(166), b_+OE(167)); L = E;
  CYC(b_+O(167), b_+OE(168)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(b_+O(168), b_+OE(170)); L = 0xc2; // Part.subid
  CYC(b_+O(170), b_+OE(171)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(171), b_+OE(173)); alu_and(gb, 0x07);
  CYC(b_+O(173), b_+OE(174)); mem_wr(gb, HL, A); SET_HL(HL - 1); // [var03]
  CYC(b_+O(174), b_+OE(175)); return; // ret
}

void button_updateTileBeforeDeletion_hook(GB *gb) {
  BASE(partCode09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+O(147), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(150));
  CYC(b_+O(150), b_+OE(151)); C = A;
  CYC(b_+O(151), b_+OE(153)); B = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+O(153), setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+OE(156));
  CALL_C(b_+O(156), objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+OE(159));
  CYC(b_+O(159), b_+OE(161)); alu_cp(gb, 0x0c); // TILEINDEX_BUTTON
  if (F & FZ) { CYCT(b_+O(161), b_+OE(163)); goto dupCheckButtonPushed; } // jr z
  CYC(b_+O(161), b_+OE(163));
  CYC(b_+O(163), b_+OE(165)); goto dupSetTriggerAndPlaySound; // jr

dupCheckButtonPushed:
  CYC(b_+O(114), b_+OE(116)); E = 0xf0; // Part.var30
  CYC(b_+O(116), b_+OE(117)); A = mem_rd(gb, DE);
  CYC(b_+O(117), b_+OE(118)); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+O(118)); return; } // ret nz
  CYC(b_+O(118), b_+OE(119));
  CALL_C(b_+O(119), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(122));
  CYC(b_+O(122), b_+OE(123)); C = A;
  CYC(b_+O(123), b_+OE(125)); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+O(125), setTile_hook, SYM(setTile), b_+OE(128));

dupSetTriggerAndPlaySound:
  CYC(b_+O(128), b_+OE(130)); E = 0xc3; // Part.var03
  CYC(b_+O(130), b_+OE(131)); A = mem_rd(gb, DE);
  CYC(b_+O(131), b_+OE(134)); SET_HL(wActiveTriggers); // wActiveTriggers
  CALL_C(b_+O(134), setFlag_hook, SYM(setFlag), b_+OE(137));
  CYC(b_+O(137), b_+OE(139)); E = 0xf0; // Part.var30
  CYC(b_+O(139), b_+OE(141)); A = 0x01;
  CYC(b_+O(141), b_+OE(142)); mem_wr(gb, DE, A);
  CYC(b_+O(142), b_+OE(144)); A = 0x87; // SND_SPLASH
  CYC(b_+O(144), b_+OE(147)); TAIL(playSound_b00); // jp
}
