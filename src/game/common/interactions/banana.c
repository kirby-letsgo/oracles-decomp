#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t banana_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void banana_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0, 0x12, 0x13, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0, 0x12, 0x13, false);
  burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0, 0x14, 0x15, false); ret_effect(gb);
}

void interactionCodec0__afterCall48c6_hook(GB *gb) {
  CYC(0x48c6, 0x48c9); objectSetVisible80_hook(gb);
}

void interactionCodec0__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x48c0, 0x48c2); A = 1;
  CYC(0x48c2, 0x48c3); mem_wr(gb, DE, A);
  CALL_C(0x48c3, interactionInitGraphics_hook, 0x15fb, 0x48c6);
  interactionCodec0__afterCall48c6_hook(gb);
}

void interactionCodec0__updatePosition_hook(GB *gb) {
  CYC(0x48f8, 0x48f9); push_effect(gb, HL);
  CYC(0x48f9, 0x48fc); SET_HL(0x4908);
  CYC(0x48fc, 0x48fd); banana_add_a_to_hl(gb, 0x48fd);
  CYC(0x48fd, 0x48ff); B = 0;
  CYC(0x48ff, 0x4900); C = mem_rd(gb, HL);
  CYC(0x4900, 0x4901); SET_HL(pop_effect(gb));
  CYC(0x4901, 0x4904); objectTakePositionWithOffset_hook(gb);
}

void interactionCodec0__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x48c9, interactionAnimate_hook, 0x261b, 0x48cc);
  CYC(0x48cc, 0x48ce); A = OBJ_ENABLED;
  CALL_C(0x48ce, objectGetRelatedObject1Var_hook, 0x2160, 0x48d1);
  CYC(0x48d1, 0x48d3); L = OBJ_ID;
  CYC(0x48d3, 0x48d4); A = mem_rd(gb, HL);
  CYC(0x48d4, 0x48d6); alu_cp(gb, 0x11);
  if (!(F & FZ)) {
    CYCT(0x48d6, 0x48d9); interactionDelete_hook(gb);
    return;
  }
  CYC(0x48d6, 0x48d9);
  CYC(0x48d9, 0x48db); E = 0x48;
  CYC(0x48db, 0x48dc); A = mem_rd(gb, DE);
  CYC(0x48dc, 0x48de); L = OBJ_DIRECTION;
  CYC(0x48de, 0x48df); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x48df, 0x48e0); A = mem_rd(gb, HL);
  if (F & FZ) {
    CYCT(0x48e0, 0x48e2); interactionCodec0__updatePosition_hook(gb);
    return;
  }
  CYC(0x48e0, 0x48e2);
  CYC(0x48e2, 0x48e3); mem_wr(gb, DE, A);
  CYC(0x48e3, 0x48e4); push_effect(gb, AF);
  CYC(0x48e4, 0x48e7); SET_HL(0x4904);
  CYC(0x48e7, 0x48e8); banana_add_a_to_hl(gb, 0x48e8);
  CYC(0x48e8, 0x48e9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x48e9, 0x48eb); E = 0x5a;
  CYC(0x48eb, 0x48ec); mem_wr(gb, DE, A);
  CYC(0x48ec, 0x48ed); SET_AF(pop_effect(gb));
  CALL_C(0x48ed, interactionSetAnimation_hook, 0x262e, 0x48f0);
  CYC(0x48f0, 0x48f2); A = OBJ_ENABLED;
  CALL_C(0x48f2, objectGetRelatedObject1Var_hook, 0x2160, 0x48f5);
  CYC(0x48f5, 0x48f7); L = OBJ_DIRECTION;
  CYC(0x48f7, 0x48f8); A = mem_rd(gb, HL);
  interactionCodec0__updatePosition_hook(gb);
}

void interactionCodec0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x48b8, 0x48ba); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x48ba, 0x48bb); A = mem_rd(gb, DE);
  CYC(0x48bb, 0x48bc); push_effect(gb, 0x48bc);
  switch (banana_jump_table(gb)) {
    case 0x48c0: interactionCodec0__state0_hook(gb); return;
    case 0x48c9: interactionCodec0__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
