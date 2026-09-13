#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static void switch_hook_set_position_component(GB *gb) {
  CYC(0x57bf, 0x57c0); A = mem_rd(gb, HL);
  CYC(0x57c0, 0x57c2); alu_cp(gb, 0xf8);
  if (F & FC) {
    CYCT(0x57c2, 0x57c4);
  } else {
    CYC(0x57c2, 0x57c4);
    CYC(0x57c4, 0x57c5); alu_xor(gb, A);
  }
  CYC(0x57c5, 0x57c7); H = 0xd0;
  CYC(0x57c7, 0x57c8); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x57c8, 0x57c9); C = A;
  CYC(0x57c9, 0x57cb); A = 0x00;
  CYC(0x57cb, 0x57cc); alu_sbc(gb, A);
  CYC(0x57cc, 0x57cd); alu_rra(gb);
  CYC(0x57cd, 0x57cf); C = alu_rr(gb, C);
  CYC(0x57cf, 0x57d0); alu_rra(gb);
  CYC(0x57d0, 0x57d2); C = alu_rr(gb, C);
  CYC(0x57d2, 0x57d3); alu_xor(gb, A);
  for (;;) {
    CYC(0x57d3, 0x57d4); alu_add(gb, C);
    CYC(0x57d4, 0x57d5); E = alu_dec8(gb, E);
    if (!(F & FZ)) {
      CYCT(0x57d5, 0x57d7);
    } else {
      CYC(0x57d5, 0x57d7);
      break;
    }
  }
  CYC(0x57d7, 0x57d8); alu_add(gb, mem_rd(gb, HL));
  CYC(0x57d8, 0x57d9); alu_add(gb, B);
  CYC(0x57d9, 0x57da); H = D;
  CYC(0x57da, 0x57db); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x57db, 0x57dc); ret_effect(gb);
}

void itemCode0bPost_hook(GB *gb) {
  CYC(0x5791, 0x5794); A = W8(w1WeaponItem_id);
  CYC(0x5794, 0x5796); alu_cp(gb, 0x0a);
  if (!(F & FZ)) {
    CYCT(0x5796, 0x5799);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x5796, 0x5799);
  CYC(0x5799, 0x579c); A = W8(w1WeaponItem_var2f);
  CYC(0x579c, 0x579e); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(0x579e, 0x57a1);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x579e, 0x57a1);
  CYC(0x57a1, 0x57a2); H = D;
  CYC(0x57a2, 0x57a5); A = W8(w1WeaponItem_zh);
  CYC(0x57a5, 0x57a7); L = 0x0f;
  CYC(0x57a7, 0x57a8); mem_wr(gb, HL, A);
  CYC(0x57a8, 0x57aa); L = 0x06;
  CYC(0x57aa, 0x57ab); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x57ab, 0x57ad);
  } else {
    CYC(0x57ab, 0x57ad);
    CYC(0x57ad, 0x57af); mem_wr(gb, HL, 0x03);
  }
  CYC(0x57af, 0x57b0); E = mem_rd(gb, HL);
  CYC(0x57b0, 0x57b1); push_effect(gb, DE);
  CYC(0x57b1, 0x57b3); B = 0x03;
  CYC(0x57b3, 0x57b6); SET_HL(w1WeaponItem_yh);
  CYC(0x57b6, 0x57b9); push_effect(gb, 0x57b9);
  switch_hook_set_position_component(gb);
  CYC(0x57b9, 0x57ba); SET_DE(pop_effect(gb));
  CYC(0x57ba, 0x57bc); B = 0x00;
  CYC(0x57bc, 0x57bf); SET_HL(w1WeaponItem_xh);
  switch_hook_set_position_component(gb);
}

void itemCode0aPost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57f2, cpRelatedObject1ID_hook, 0x4b86, 0x57f5);
  if (F & FZ) {
    CYCT(0x57f5, 0x57f6); ret_effect(gb); return;
  }
  CYC(0x57f5, 0x57f6);
  CYC(0x57f6, 0x57f9); A = W8(wSwitchHookState);
  CYC(0x57f9, 0x57fa); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x57fa, 0x57fd);
    itemDelete_hook(gb);
    return;
  }
  CYC(0x57fa, 0x57fd);
  CYC(0x57fd, 0x5800);
  func_5902(gb);
}
