#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t c1_jump_table(GB *gb) {
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

void interactionCodec1__afterCall71ce_hook(GB *gb) {
  CYC(0x71ce, 0x71cf); H = D;
  CYC(0x71cf, 0x71d1); L = 0x46;
  CYC(0x71d1, 0x71d3); mem_wr(gb, HL, 0x86);
  CYC(0x71d3, 0x71d4); L = alu_inc8(gb, L);
  CYC(0x71d4, 0x71d6); mem_wr(gb, HL, 1);
  CYC(0x71d6, 0x71d8); L = 0x76;
  CYC(0x71d8, 0x71da); mem_wr(gb, HL, 6);
  CYC(0x71da, 0x71dc); L = 0x49;
  CYC(0x71dc, 0x71de); mem_wr(gb, HL, 0x15);
  CYC(0x71de, 0x71e0); L = 0x50;
  CYC(0x71e0, 0x71e2); mem_wr(gb, HL, 0x78);
  CYC(0x71e2, 0x71e5); objectSetVisible82_hook(gb);
}

void interactionCodec1__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71c8, 0x71ca); A = 1;
  CYC(0x71ca, 0x71cb); mem_wr(gb, DE, A);
  CALL_C(0x71cb, interactionInitGraphics_hook, 0x15fb, 0x71ce);
  interactionCodec1__afterCall71ce_hook(gb);
}

void interactionCodec1__ret_hook(GB *gb) {
  CYC(0x7223, 0x7224); ret_effect(gb);
}

void interactionCodec1__updateSparkles_hook(GB *gb) {
  CYC(0x722f, 0x7230); H = D;
  CYC(0x7230, 0x7232); L = 0x76;
  CYC(0x7232, 0x7233); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x7233, 0x7234); ret_effect(gb); return; }
  CYC(0x7233, 0x7234);
  CYC(0x7234, 0x7236); mem_wr(gb, HL, 6);
  CYC(0x7236, 0x7239); SET_BC(0x8409);
  CYC(0x7239, 0x723c); objectCreateInteraction_hook(gb);
}

void interactionCodec1__updateMovementAndSparkles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x721a, interactionCodec1__updateSparkles_hook, 0x722f, 0x721d);
  CALL_C(0x721d, objectApplySpeed_hook, 0x201d, 0x7220);
  CYC(0x7220, 0x7223); interactionDecCounter1_hook(gb);
}

void interactionCodec1__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71ef, 0x71f0); H = D;
  CYC(0x71f0, 0x71f2); L = 0x46;
  CALL_C(0x71f2, decHlRef16WithCap_hook, 0x0237, 0x71f5);
  if (!(F & FZ)) { CYCT(0x71f5, 0x71f6); ret_effect(gb); return; }
  CYC(0x71f5, 0x71f6);
  CYC(0x71f6, 0x71f8); L = 0x46;
  CYC(0x71f8, 0x71fa); mem_wr(gb, HL, 0x28);
  CYC(0x71fa, 0x71fd); interactionIncSubstate_hook(gb);
}

void interactionCodec1__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71fd, interactionCodec1__updateMovementAndSparkles_hook, 0x721a, 0x7200);
  if (!(F & FZ)) { CYCT(0x7200, 0x7202); interactionCodec1__ret_hook(gb); return; }
  CYC(0x7200, 0x7202);
  CYC(0x7202, 0x7204); L = 0x60;
  CYC(0x7204, 0x7206); mem_wr(gb, HL, 1);
  CYC(0x7206, 0x7209); interactionIncSubstate_hook(gb);
}

void interactionCodec1__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7209, interactionAnimate_hook, 0x261b, 0x720c);
  CALL_C(0x720c, interactionCodec1__updateSparkles_hook, 0x722f, 0x720f);
  CALL_C(0x720f, objectApplySpeed_hook, 0x201d, 0x7212);
  CYC(0x7212, 0x7214); E = 0x61;
  CYC(0x7214, 0x7215); A = mem_rd(gb, DE);
  CYC(0x7215, 0x7216); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x7216, 0x7219); interactionDelete_hook(gb); return; }
  CYC(0x7216, 0x7219);
  CYC(0x7219, 0x721a); ret_effect(gb);
}

void interactionCodec1__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71e5, 0x71e7); E = 0x45;
  CYC(0x71e7, 0x71e8); A = mem_rd(gb, DE);
  CYC(0x71e8, 0x71e9); push_effect(gb, 0x71e9);
  switch (c1_jump_table(gb)) {
    case 0x71ef: interactionCodec1__substate0_hook(gb); return;
    case 0x71fd: interactionCodec1__substate1_hook(gb); return;
    case 0x7209: interactionCodec1__substate2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void interactionCodec1__func_7224_hook(GB *gb) {
  CYC(0x7224, 0x7227); A = mem_rd(gb, wFrameCounter);
  CYC(0x7227, 0x7229); alu_and(gb, 1);
  if (F & FZ) { CYCT(0x7229, 0x722c); objectSetInvisible_hook(gb); return; }
  CYC(0x7229, 0x722c);
  CYC(0x722c, 0x722f); objectSetVisible_hook(gb);
}

void interactionCodec1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71c0, 0x71c2); E = 0x44;
  CYC(0x71c2, 0x71c3); A = mem_rd(gb, DE);
  CYC(0x71c3, 0x71c4); push_effect(gb, 0x71c4);
  switch (c1_jump_table(gb)) {
    case 0x71c8: interactionCodec1__state0_hook(gb); return;
    case 0x71e5: interactionCodec1__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
