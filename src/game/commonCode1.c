#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void itemNextAnimationFrame_hook(GB *gb);

static void item_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void itemSetVar3cToFF_hook(GB *gb) {
  CYC(0x49c2, 0x49c4); E = 0x3c;
  CYC(0x49c4, 0x49c6); A = 0xff;
  CYC(0x49c6, 0x49c7); mem_wr(gb, DE, A);
  CYC(0x49c7, 0x49c8); ret_effect(gb);
}

void itemUpdateDamageToApply_hook(GB *gb) {
  CYC(0x49c8, 0x49c9); H = D;
  CYC(0x49c9, 0x49cb); L = 0x25;
  CYC(0x49cb, 0x49cc); A = mem_rd(gb, HL);
  CYC(0x49cc, 0x49ce); mem_wr(gb, HL, 0x00);
  CYC(0x49ce, 0x49d0); L = 0x29;
  CYC(0x49d0, 0x49d1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x49d1, 0x49d2); mem_wr(gb, HL, A);
  CYC(0x49d2, 0x49d3); alu_rlca(gb);
  CYC(0x49d3, 0x49d5); L = 0x2a;
  CYC(0x49d5, 0x49d6); A = mem_rd(gb, HL);
  CYC(0x49d6, 0x49d7); A = alu_dec8(gb, A);
  CYC(0x49d7, 0x49d8); A = alu_inc8(gb, A);
  CYC(0x49d8, 0x49d9); ret_effect(gb);
}

void itemAnimate_hook(GB *gb) {
  CYC(0x49d9, 0x49da); H = D;
  CYC(0x49da, 0x49dc); L = 0x20;
  CYC(0x49dc, 0x49dd); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x49dd, 0x49de); ret_effect(gb); return;
  }
  CYC(0x49dd, 0x49de);
  CYC(0x49de, 0x49e0); L = 0x22;
  CYC(0x49e0, 0x49e2); itemNextAnimationFrame_hook(gb);
}

void itemSetAnimation_hook(GB *gb) {
  CYC(0x49e2, 0x49e3); alu_add(gb, A);
  CYC(0x49e3, 0x49e4); C = A;
  CYC(0x49e4, 0x49e6); B = 0x00;
  CYC(0x49e6, 0x49e8); E = 0x01;
  CYC(0x49e8, 0x49e9); A = mem_rd(gb, DE);
  CYC(0x49e9, 0x49ec); SET_HL(0x6663);
  CYC(0x49ec, 0x49ed); item_add_double_index_from_rst(gb, 0x49ed);
  CYC(0x49ed, 0x49ee); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49ee, 0x49ef); H = mem_rd(gb, HL);
  CYC(0x49ef, 0x49f0); L = A;
  CYC(0x49f0, 0x49f1); alu_add_hl(gb, BC);
  itemNextAnimationFrame_hook(gb);
}

void itemNextAnimationFrame_hook(GB *gb) {
  CYC(0x49f1, 0x49f2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49f2, 0x49f3); H = mem_rd(gb, HL);
  CYC(0x49f3, 0x49f4); L = A;
  CYC(0x49f4, 0x49f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49f5, 0x49f7); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x49f7, 0x49f9);
  } else {
    CYC(0x49f7, 0x49f9);
    CYC(0x49f9, 0x49fa); B = A;
    CYC(0x49fa, 0x49fb); C = mem_rd(gb, HL);
    CYC(0x49fb, 0x49fc); alu_add_hl(gb, BC);
    CYC(0x49fc, 0x49fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  }
  CYC(0x49fd, 0x49ff); E = 0x20;
  CYC(0x49ff, 0x4a00); mem_wr(gb, DE, A);
  CYC(0x4a00, 0x4a01); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a01, 0x4a02); C = A;
  CYC(0x4a02, 0x4a04); B = 0x00;
  CYC(0x4a04, 0x4a05); E = alu_inc8(gb, E);
  CYC(0x4a05, 0x4a06); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a06, 0x4a07); mem_wr(gb, DE, A);
  CYC(0x4a07, 0x4a08); E = alu_inc8(gb, E);
  CYC(0x4a08, 0x4a09); A = L;
  CYC(0x4a09, 0x4a0a); mem_wr(gb, DE, A);
  CYC(0x4a0a, 0x4a0b); E = alu_inc8(gb, E);
  CYC(0x4a0b, 0x4a0c); A = H;
  CYC(0x4a0c, 0x4a0d); mem_wr(gb, DE, A);
  CYC(0x4a0d, 0x4a0f); E = 0x01;
  CYC(0x4a0f, 0x4a10); A = mem_rd(gb, DE);
  CYC(0x4a10, 0x4a13); SET_HL(0x66c3);
  CYC(0x4a13, 0x4a14); item_add_double_index_from_rst(gb, 0x4a14);
  CYC(0x4a14, 0x4a15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a15, 0x4a16); H = mem_rd(gb, HL);
  CYC(0x4a16, 0x4a17); L = A;
  CYC(0x4a17, 0x4a18); alu_add_hl(gb, BC);
  CYC(0x4a18, 0x4a1a); E = 0x1e;
  CYC(0x4a1a, 0x4a1b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a1b, 0x4a1c); mem_wr(gb, DE, A);
  CYC(0x4a1c, 0x4a1d); E = alu_inc8(gb, E);
  CYC(0x4a1d, 0x4a1e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4a1e, 0x4a20); alu_and(gb, 0x3f);
  CYC(0x4a20, 0x4a21); mem_wr(gb, DE, A);
  CYC(0x4a21, 0x4a22); ret_effect(gb);
}
