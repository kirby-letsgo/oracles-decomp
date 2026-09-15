#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode09_hook(GB *gb);
void button_state0_hook(GB *gb);
void button_updateTileBeforeDeletion_hook(GB *gb);

void partCode09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4724, 0x4726); E = 0xc4; // Part.state
  CYC(0x4726, 0x4727); A = mem_rd(gb, DE);
  CYC(0x4727, 0x4728); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4728, 0x472b); // call z
    button_state0_hook(gb);
  } else {
    CYC(0x4728, 0x472b);
  }

  CYC(0x472b, 0x472e); A = mem_rd(gb, 0xccb1); // wccb1
  CYC(0x472e, 0x472f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x472f); return; } // ret nz
  CYC(0x472f, 0x4730);
  CYC(0x4730, 0x4733); SET_HL(0xd000); // w1Link
  CALL_C(0x4733, checkObjectsCollided_hook, 0x1d5a, 0x4736);
  if (F & FC) { CYCT(0x4736, 0x4738); goto linkTouchedButton; } // jr c
  CYC(0x4736, 0x4738);
  CALL_C(0x4738, objectGetTileAtPosition_hook, 0x1444, 0x473b);
  CYC(0x473b, 0x473d); alu_sub(gb, 0x0c); // TILEINDEX_BUTTON
  CYC(0x473d, 0x473f); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x473f, 0x4741); goto somethingOnButton; } // jr nc
  CYC(0x473f, 0x4741);
  CALL_C(0x4741, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x4744);
  if (!(F & FZ)) { RET_TAKEN(0x4744); return; } // ret nz
  CYC(0x4744, 0x4745);
  CYC(0x4745, 0x4747); L = 0xf0; // Part.var30
  CYC(0x4747, 0x4749); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x4749); return; } // ret z
  CYC(0x4749, 0x474a);
  CYC(0x474a, 0x474c); E = 0xf0; // Part.var30
  CYC(0x474c, 0x474d); A = mem_rd(gb, DE);
  CYC(0x474d, 0x474e); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x474e); return; } // ret z
  CYC(0x474e, 0x474f);
  CALL_C(0x474f, objectGetShortPosition_hook, 0x2096, 0x4752);
  CYC(0x4752, 0x4753); C = A;
  CYC(0x4753, 0x4755); A = 0x0c; // TILEINDEX_BUTTON
  CALL_C(0x4755, setTile_hook, 0x3a9c, 0x4758);
  CYC(0x4758, 0x475a); E = 0xc3; // Part.var03
  CYC(0x475a, 0x475b); A = mem_rd(gb, DE);
  CYC(0x475b, 0x475e); SET_HL(0xcca0); // wActiveTriggers
  CALL_C(0x475e, unsetFlag_hook, 0x0218, 0x4761);
  CYC(0x4761, 0x4763); E = 0xf0; // Part.var30
  CYC(0x4763, 0x4764); alu_xor(gb, A);
  CYC(0x4764, 0x4765); mem_wr(gb, DE, A);
  CYC(0x4765, 0x4767); A = 0x87; // SND_SPLASH
  CYC(0x4767, 0x476a); playSound_b00_hook(gb); return; // jp

somethingOnButton:
  CYC(0x476a, 0x476b); H = D;
  CYC(0x476b, 0x476d); L = 0xc2; // Part.subid
  CYC(0x476d, 0x476f); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x476f, 0x4771); goto delete; } // jr z
  CYC(0x476f, 0x4771);
  CYC(0x4771, 0x4773); L = 0xf0; // Part.var30
  CYC(0x4773, 0x4775); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x4775); return; } // ret nz
  CYC(0x4775, 0x4776);
  CYC(0x4776, 0x4778); L = 0xc6; // Part.counter1
  CYC(0x4778, 0x477a); mem_wr(gb, HL, 0x1c);
  CALL_C(0x477a, objectGetShortPosition_hook, 0x2096, 0x477d);
  CYC(0x477d, 0x477e); C = A;
  CYC(0x477e, 0x4780); B = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(0x4780, setTileInRoomLayoutBuffer_hook, 0x1426, 0x4783);
  CYC(0x4783, 0x4785); goto setTriggerAndPlaySound; // jr

delete:
  CYC(0x4785, 0x4788); push_effect(gb, 0x4788); button_updateTileBeforeDeletion_hook(gb);
  CYC(0x4788, 0x478b); partDelete_hook(gb); return; // jp

linkTouchedButton:
  CYC(0x478b, 0x478e); A = mem_rd(gb, 0xd00f); // w1Link.zh
  CYC(0x478e, 0x478f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x478f); return; } // ret nz
  CYC(0x478f, 0x4790);

  CYC(0x4790, 0x4792); E = 0xc2; // Part.subid
  CYC(0x4792, 0x4793); A = mem_rd(gb, DE);
  CYC(0x4793, 0x4794); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x4794, 0x4796); goto delete; } // jr nc
  CYC(0x4794, 0x4796);

  CYC(0x4796, 0x4798); E = 0xf0; // Part.var30
  CYC(0x4798, 0x4799); A = mem_rd(gb, DE);
  CYC(0x4799, 0x479a); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x479a); return; } // ret nz
  CYC(0x479a, 0x479b);
  CALL_C(0x479b, objectGetShortPosition_hook, 0x2096, 0x479e);
  CYC(0x479e, 0x479f); C = A;
  CYC(0x479f, 0x47a1); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(0x47a1, setTile_hook, 0x3a9c, 0x47a4);

setTriggerAndPlaySound:
  CYC(0x47a4, 0x47a6); E = 0xc3; // Part.var03
  CYC(0x47a6, 0x47a7); A = mem_rd(gb, DE);
  CYC(0x47a7, 0x47aa); SET_HL(0xcca0); // wActiveTriggers
  CALL_C(0x47aa, setFlag_hook, 0x020e, 0x47ad);
  CYC(0x47ad, 0x47af); E = 0xf0; // Part.var30
  CYC(0x47af, 0x47b1); A = 0x01;
  CYC(0x47b1, 0x47b2); mem_wr(gb, DE, A);
  CYC(0x47b2, 0x47b4); A = 0x87; // SND_SPLASH
  CYC(0x47b4, 0x47b7); playSound_b00_hook(gb); return; // jp
}

void button_state0_hook(GB *gb) {
  CYC(0x47c9, 0x47ca); H = D;
  CYC(0x47ca, 0x47cb); L = E;
  CYC(0x47cb, 0x47cc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(0x47cc, 0x47ce); L = 0xc2; // Part.subid
  CYC(0x47ce, 0x47cf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x47cf, 0x47d1); alu_and(gb, 0x07);
  CYC(0x47d1, 0x47d2); mem_wr(gb, HL, A); SET_HL(HL - 1); // [var03]
  CYC(0x47d2, 0x47d3); return; // ret
}

void button_updateTileBeforeDeletion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x47b7, objectGetShortPosition_hook, 0x2096, 0x47ba);
  CYC(0x47ba, 0x47bb); C = A;
  CYC(0x47bb, 0x47bd); B = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(0x47bd, setTileInRoomLayoutBuffer_hook, 0x1426, 0x47c0);
  CALL_C(0x47c0, objectGetTileAtPosition_hook, 0x1444, 0x47c3);
  CYC(0x47c3, 0x47c5); alu_cp(gb, 0x0c); // TILEINDEX_BUTTON
  if (F & FZ) { CYCT(0x47c5, 0x47c7); goto dupCheckButtonPushed; } // jr z
  CYC(0x47c5, 0x47c7);
  CYC(0x47c7, 0x47c9); goto dupSetTriggerAndPlaySound; // jr

dupCheckButtonPushed:
  CYC(0x4796, 0x4798); E = 0xf0; // Part.var30
  CYC(0x4798, 0x4799); A = mem_rd(gb, DE);
  CYC(0x4799, 0x479a); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x479a); return; } // ret nz
  CYC(0x479a, 0x479b);
  CALL_C(0x479b, objectGetShortPosition_hook, 0x2096, 0x479e);
  CYC(0x479e, 0x479f); C = A;
  CYC(0x479f, 0x47a1); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(0x47a1, setTile_hook, 0x3a9c, 0x47a4);

dupSetTriggerAndPlaySound:
  CYC(0x47a4, 0x47a6); E = 0xc3; // Part.var03
  CYC(0x47a6, 0x47a7); A = mem_rd(gb, DE);
  CYC(0x47a7, 0x47aa); SET_HL(0xcca0); // wActiveTriggers
  CALL_C(0x47aa, setFlag_hook, 0x020e, 0x47ad);
  CYC(0x47ad, 0x47af); E = 0xf0; // Part.var30
  CYC(0x47af, 0x47b1); A = 0x01;
  CYC(0x47b1, 0x47b2); mem_wr(gb, DE, A);
  CYC(0x47b2, 0x47b4); A = 0x87; // SND_SPLASH
  CYC(0x47b4, 0x47b7); playSound_b00_hook(gb); return; // jp
}
