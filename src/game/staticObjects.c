#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

static void add_double_index_to_hl(GB *gb) {
  CYC(0x508e, 0x508f); push_effect(gb, 0x508f);
  burn_rom(gb, 0, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x001f, 0x0020, false); ret_effect(gb);
}

void loadStaticObjects_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5085, clearStaticObjects_hook, 0x319f, 0x5088);
  CYC(0x5088, 0x508b); A = mem_rd(gb, 0xcc39);
  CYC(0x508b, 0x508e); SET_HL(0x50a7);
  add_double_index_to_hl(gb);
  CYC(0x508f, 0x5090); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5090, 0x5091); H = mem_rd(gb, HL);
  CYC(0x5091, 0x5092); L = A;
  CYC(0x5092, 0x5095); SET_DE(0xcd80);
  for (;;) {
    CYC(0x5095, 0x5096); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5096, 0x5098); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(0x5098, 0x5099); ret_effect(gb); return; }
    CYC(0x5098, 0x5099);
    CYC(0x5099, 0x509a); mem_wr(gb, DE, A);
    CYC(0x509a, 0x509c); B = 5;
    for (;;) {
      CYC(0x509c, 0x509d); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x509d, 0x509e); E = alu_inc8(gb, E);
      CYC(0x509e, 0x509f); mem_wr(gb, DE, A);
      CYC(0x509f, 0x50a0); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x50a0, 0x50a2); continue; }
      CYC(0x50a0, 0x50a2);
      break;
    }
    CYC(0x50a2, 0x50a3); E = alu_inc8(gb, E);
    CYC(0x50a3, 0x50a4); E = alu_inc8(gb, E);
    CYC(0x50a4, 0x50a5); E = alu_inc8(gb, E);
    CYC(0x50a5, 0x50a8);
  }
}
