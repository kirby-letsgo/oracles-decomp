#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

void monkeyJumpSpeed100_hook(GB *gb) {
  CYC(0x7522, 0x7525); SET_BC(0xff00);
  CYC(0x7525, 0x7528);
  objectSetSpeedZ_hook(gb);
}

void monkeySetAnimationAndJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x751f, interactionSetAnimation_hook, 0x262e, 0x7522);
  monkeyJumpSpeed100_hook(gb);
}

void monkeySubid0State1Substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7528, objectCheckWithinScreenBoundary_hook, 0x2184, 0x752b);
  if (!(F & FC)) {
    CYC(0x752b, 0x752d);
    CYC(0x752d, 0x752f); A = 0x01;
    CYC(0x752f, 0x7532); mem_wr(gb, wLoadedTreeGfxIndex, A);
    CYC(0x7532, 0x7535);
    interactionDelete_hook(gb);
    return;
  }
  CYCT(0x752b, 0x752d);
  CYC(0x7535, 0x7537); C = 0x20;
  CALL_C(0x7537, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x753a);
  if (!(F & FZ)) {
    CYCT(0x753a, 0x753d);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x753a, 0x753d);
  CYC(0x753d, 0x753f); A = 0x04;
  CYC(0x753f, 0x7541);
  monkeySetAnimationAndJump_hook(gb);
}

void monkeyJumpSpeed120_hook(GB *gb) {
  CYC(0x7547, 0x754a); SET_BC(0xfee0);
  CYC(0x754a, 0x754d);
  objectSetSpeedZ_hook(gb);
}

void monkeyUpdateGravityAndHop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7541, 0x7543); C = 0x20;
  CALL_C(0x7543, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7546);
  if (!(F & FZ)) {
    CYCT(0x7546, 0x7547); ret_effect(gb);
    return;
  }
  CYC(0x7546, 0x7547);
  monkeyJumpSpeed120_hook(gb);
}

void monkeySetJumpSpeed_hook(GB *gb) {
  CYC(0x7553, 0x7555); L = 0x78;
  CYC(0x7555, 0x7556); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7556, 0x7558); E = 0x54;
  CYC(0x7558, 0x7559); mem_wr(gb, DE, A);
  CYC(0x7559, 0x755a); E = alu_inc8(gb, E);
  CYC(0x755a, 0x755b); A = mem_rd(gb, HL);
  CYC(0x755b, 0x755c); mem_wr(gb, DE, A);
  CYC(0x755c, 0x755d); ret_effect(gb);
}

void monkeyUpdateGravityAndJumpIfLanded_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x754d, 0x754f); C = 0x10;
  CALL_C(0x754f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7552);
  if (!(F & FZ)) {
    CYCT(0x7552, 0x7553); ret_effect(gb);
    return;
  }
  CYC(0x7552, 0x7553);
  monkeySetJumpSpeed_hook(gb);
}

void monkeyBeginDisappearing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7598, 0x759a); mem_wr(gb, HL, 0x3c);
  CYC(0x759a, 0x759c); L = 0x5c;
  CYC(0x759c, 0x759e); mem_wr(gb, HL, 0x06);
  CYC(0x759e, 0x75a0); L = 0x4f;
  CYC(0x75a0, 0x75a2); mem_wr(gb, HL, 0x00);
  CYC(0x75a2, 0x75a4); A = 0x50;
  CALL_C(0x75a4, playSound_b00_hook, 0x0c98, 0x75a7);
  CYC(0x75a7, 0x75aa);
  interactionIncSubstate_hook(gb);
}

void monkeyWaitBeforeFlickering_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75aa, interactionDecCounter1_hook, 0x23cc, 0x75ad);
  if (!(F & FZ)) {
    CYCT(0x75ad, 0x75ae); ret_effect(gb);
    return;
  }
  CYC(0x75ad, 0x75ae);
  CYC(0x75ae, 0x75b0); mem_wr(gb, HL, 0x3c);
  CYC(0x75b0, 0x75b3);
  interactionIncSubstate_hook(gb);
}

void monkeyFlickerUntilDeletion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75b3, interactionDecCounter1_hook, 0x23cc, 0x75b6);
  if (!(F & FZ)) {
    CYCT(0x75b6, 0x75b8);
    CYC(0x75bb, 0x75bd); B = 0x01;
    CYC(0x75bd, 0x75c0);
    objectFlickerVisibility_hook(gb);
    return;
  }
  CYC(0x75b6, 0x75b8);
  CYC(0x75b8, 0x75bb);
  interactionDelete_hook(gb);
}
