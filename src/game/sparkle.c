#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static void sparkle_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void partCode26_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6088, 0x608a); E = 0xc4; // Part.state
  CYC(0x608a, 0x608b); A = mem_rd(gb, DE);
  CYC(0x608b, 0x608c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x608c, 0x608e); goto state0; } // jr z
  CYC(0x608c, 0x608e);
  CALL_C(0x608e, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6091);
  if (!(F & FZ)) { CYCT(0x6091, 0x6093); goto counter1NonZero; } // jr nz
  CYC(0x6091, 0x6093);
  CYC(0x6093, 0x6094); L = alu_inc8(gb, L);
  CYC(0x6094, 0x6095); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6095, 0x6096); mem_wr(gb, HL, A);
  CYC(0x6096, 0x6098); L = 0xf0; // Part.var30
  CYC(0x6098, 0x6099); A = mem_rd(gb, HL);
  CYC(0x6099, 0x609a); alu_cpl(gb);
  CYC(0x609a, 0x609c); alu_add(gb, 0x01);
  CYC(0x609c, 0x609d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x609d, 0x609e); A = mem_rd(gb, HL);
  CYC(0x609e, 0x609f); alu_cpl(gb);
  CYC(0x609f, 0x60a1); alu_adc(gb, 0x00);
  CYC(0x60a1, 0x60a2); mem_wr(gb, HL, A);

counter1NonZero:
  CYC(0x60a2, 0x60a4); E = 0xcd; // Part.xh
  CYC(0x60a4, 0x60a5); A = mem_rd(gb, DE);
  CYC(0x60a5, 0x60a6); B = A;
  CYC(0x60a6, 0x60a7); E = alu_dec8(gb, E);
  CYC(0x60a7, 0x60a8); A = mem_rd(gb, DE);
  CYC(0x60a8, 0x60a9); C = A;
  CYC(0x60a9, 0x60ab); L = 0xd2;
  CYC(0x60ab, 0x60ac); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60ac, 0x60ad); H = mem_rd(gb, HL);
  CYC(0x60ad, 0x60ae); L = A;
  CYC(0x60ae, 0x60af); alu_add_hl(gb, BC);
  CYC(0x60af, 0x60b0); A = L;
  CYC(0x60b0, 0x60b1); mem_wr(gb, DE, A);
  CYC(0x60b1, 0x60b2); E = alu_inc8(gb, E);
  CYC(0x60b2, 0x60b3); A = H;
  CYC(0x60b3, 0x60b4); mem_wr(gb, DE, A);
  CYC(0x60b4, 0x60b6); E = 0xf0; // Part.var30
  CYC(0x60b6, 0x60b7); A = mem_rd(gb, DE);
  CYC(0x60b7, 0x60b8); C = A;
  CYC(0x60b8, 0x60b9); E = alu_inc8(gb, E);
  CYC(0x60b9, 0x60ba); A = mem_rd(gb, DE);
  CYC(0x60ba, 0x60bb); B = A;
  CYC(0x60bb, 0x60bd); E = 0xd3;
  CYC(0x60bd, 0x60be); A = mem_rd(gb, DE);
  CYC(0x60be, 0x60bf); H = A;
  CYC(0x60bf, 0x60c0); E = alu_dec8(gb, E);
  CYC(0x60c0, 0x60c1); A = mem_rd(gb, DE);
  CYC(0x60c1, 0x60c2); L = A;
  CYC(0x60c2, 0x60c3); alu_add_hl(gb, BC);
  CYC(0x60c3, 0x60c4); A = L;
  CYC(0x60c4, 0x60c5); mem_wr(gb, DE, A);
  CYC(0x60c5, 0x60c6); E = alu_inc8(gb, E);
  CYC(0x60c6, 0x60c7); A = H;
  CYC(0x60c7, 0x60c8); mem_wr(gb, DE, A);
  CYC(0x60c8, 0x60c9); H = D;
  CYC(0x60c9, 0x60cb); L = 0xce;
  CYC(0x60cb, 0x60cd); E = 0xd4; // Part.speedZ
  CYC(0x60cd, 0x60ce); A = mem_rd(gb, DE);
  CYC(0x60ce, 0x60cf); alu_add(gb, mem_rd(gb, HL));
  CYC(0x60cf, 0x60d0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x60d0, 0x60d1); A = mem_rd(gb, HL);
  CYC(0x60d1, 0x60d3); alu_adc(gb, 0x00);
  if (F & FZ) { CYCT(0x60d3, 0x60d6); partDelete_hook(gb); return; } // jp z
  CYC(0x60d3, 0x60d6);
  CYC(0x60d6, 0x60d7); mem_wr(gb, HL, A);
  CYC(0x60d7, 0x60d9); alu_cp(gb, 0xe8);
  if (F & FC) { CYCT(0x60d9, 0x60db); goto animate; } // jr c
  CYC(0x60d9, 0x60db);
  CYC(0x60db, 0x60dd); L = 0xda; // Part.visible
  CYC(0x60dd, 0x60de); A = mem_rd(gb, HL);
  CYC(0x60de, 0x60e0); alu_xor(gb, 0x80);
  CYC(0x60e0, 0x60e1); mem_wr(gb, HL, A);

animate:
  CYC(0x60e1, 0x60e4); partAnimate_hook(gb); return; // jp

state0:
  CYC(0x60e4, 0x60e5); H = D;
  CYC(0x60e5, 0x60e6); L = E;
  CYC(0x60e6, 0x60e7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x60e7, objectGetZAboveScreen_hook, 0x2172, 0x60ea);
  CYC(0x60ea, 0x60ec); L = 0xcf; // Part.zh
  CYC(0x60ec, 0x60ed); mem_wr(gb, HL, A);
  CYC(0x60ed, 0x60ef); E = 0xc3; // Part.var03
  CYC(0x60ef, 0x60f0); A = mem_rd(gb, DE);
  CYC(0x60f0, 0x60f1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x60f1, 0x60f3); goto var03_00; } // jr z
  CYC(0x60f1, 0x60f3);
  CYC(0x60f3, 0x60f5); mem_wr(gb, HL, 0xf0);

var03_00:
  CALL_C(0x60f5, getRandomNumber_noPreserveVars_hook, 0x0453, 0x60f8);
  CYC(0x60f8, 0x60fa); alu_and(gb, 0x0c);
  CYC(0x60fa, 0x60fd); SET_HL(0x6114); // table_6114
  CYC(0x60fd, 0x60fe); sparkle_addAToHl_from_rst(gb, 0x60fe);
  CYC(0x60fe, 0x6100); E = 0xf0; // Part.var30
  CYC(0x6100, 0x6101); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6101, 0x6102); mem_wr(gb, DE, A);
  CYC(0x6102, 0x6103); E = alu_inc8(gb, E);
  CYC(0x6103, 0x6104); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6104, 0x6105); mem_wr(gb, DE, A);
  CYC(0x6105, 0x6107); E = 0xd4; // Part.speedZ
  CYC(0x6107, 0x6108); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6108, 0x6109); mem_wr(gb, DE, A);
  CYC(0x6109, 0x610b); E = 0xc6; // Part.counter1
  CYC(0x610b, 0x610c); A = mem_rd(gb, HL);
  CYC(0x610c, 0x610d); mem_wr(gb, DE, A);
  CYC(0x610d, 0x610e); E = alu_inc8(gb, E);
  CYC(0x610e, 0x610f); A = alu_dec8(gb, A);
  CYC(0x610f, 0x6110); alu_add(gb, A);
  CYC(0x6110, 0x6111); mem_wr(gb, DE, A);
  CYC(0x6111, 0x6114); objectSetVisible81_hook(gb); return; // jp
}
