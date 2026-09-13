#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

static uint16_t monkey_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

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

static void monkey0_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x7581, interactionAnimate_hook, 0x261b, 0x7584);
  CALL_C(0x7584, interactionDecCounter2_hook, 0x23d1, 0x7587);
  if (!(F & FZ)) {
    CYCT(0x7587, 0x7588); ret_effect(gb);
    return;
  }
  CYC(0x7587, 0x7588);
  CYC(0x7588, 0x758b);
  interactionIncSubstate_hook(gb);
}

static void monkey0_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x758b, interactionDecCounter1_hook, 0x23cc, 0x758e);
  if (F & FZ) {
    CYC(0x758e, 0x7590);
    CYC(0x7590, 0x7592);
    monkeyBeginDisappearing_hook(gb);
    return;
  }
  CYCT(0x758e, 0x7590);
  CALL_C(0x7592, monkeyUpdateGravityAndJumpIfLanded_hook, 0x754d, 0x7595);
  CYC(0x7595, 0x7598);
  interactionAnimate_hook(gb);
}

void monkey0Disappearance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7575, 0x7577); E = 0x45;
  CYC(0x7577, 0x7578); A = mem_rd(gb, DE);
  CYC(0x7578, 0x7579); push_effect(gb, 0x7579);
  switch (monkey_jump_table(gb)) {
    case 0x7581: monkey0_disappearance_substate0_hook(gb, sp0_); return;
    case 0x758b: monkey0_disappearance_substate1_hook(gb, sp0_); return;
    case 0x75aa: monkeyWaitBeforeFlickering_hook(gb); return;
    case 0x75b3: monkeyFlickerUntilDeletion_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey3_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x75ca, interactionDecCounter1_hook, 0x23cc, 0x75cd);
  if (!(F & FZ)) {
    CYCT(0x75cd, 0x75d0);
    interactionAnimate_hook(gb);
    return;
  }
  CYC(0x75cd, 0x75d0);
  CYC(0x75d0, 0x75d2);
  monkeyBeginDisappearing_hook(gb);
}

void monkey3Disappearance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75c0, 0x75c2); E = 0x45;
  CYC(0x75c2, 0x75c3); A = mem_rd(gb, DE);
  CYC(0x75c3, 0x75c4); push_effect(gb, 0x75c4);
  switch (monkey_jump_table(gb)) {
    case 0x75ca: monkey3_disappearance_substate0_hook(gb, sp0_); return;
    case 0x75aa: monkeyWaitBeforeFlickering_hook(gb); return;
    case 0x75b3: monkeyFlickerUntilDeletion_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey5_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x75e7, interactionDecCounter1_hook, 0x23cc, 0x75ea);
  if (!(F & FZ)) {
    CYCT(0x75ea, 0x75eb); ret_effect(gb);
    return;
  }
  CYC(0x75ea, 0x75eb);
  CYC(0x75eb, 0x75ed); mem_wr(gb, HL, 0xb4);
  CALL_C(0x75ed, interactionIncSubstate_hook, 0x23e5, 0x75f0);
  CYC(0x75f0, 0x75f3); SET_BC(0xf3f8);
  CYC(0x75f3, 0x75f5); A = 0x5a;
  CYC(0x75f5, 0x75f8);
  objectCreateExclamationMark_hook(gb);
}

static void monkey5_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x75e0, interactionIncSubstate_hook, 0x23e5, 0x75e3);
  CYC(0x75e3, 0x75e5); L = 0x5c;
  CYC(0x75e5, 0x75e7); mem_wr(gb, HL, 0x02);
  monkey5_disappearance_substate1_hook(gb, sp0_);
}

static void monkey5_disappearance_substate2_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x75f8, interactionDecCounter1_hook, 0x23cc, 0x75fb);
  if (!(F & FZ)) {
    CYCT(0x75fb, 0x75fc); ret_effect(gb);
    return;
  }
  CYC(0x75fb, 0x75fc);
  CYC(0x75fc, 0x75ff);
  monkeyBeginDisappearing_hook(gb);
}

void monkey5Disappearance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75d2, 0x75d4); E = 0x45;
  CYC(0x75d4, 0x75d5); A = mem_rd(gb, DE);
  CYC(0x75d5, 0x75d6); push_effect(gb, 0x75d6);
  switch (monkey_jump_table(gb)) {
    case 0x75e0: monkey5_disappearance_substate0_hook(gb, sp0_); return;
    case 0x75e7: monkey5_disappearance_substate1_hook(gb, sp0_); return;
    case 0x75f8: monkey5_disappearance_substate2_hook(gb, sp0_); return;
    case 0x75aa: monkeyWaitBeforeFlickering_hook(gb); return;
    case 0x75b3: monkeyFlickerUntilDeletion_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void monkeyCheckChangeAnimation_hook(GB *gb) {
  CYC(0x768d, 0x768e); H = D;
  CYC(0x768e, 0x7690); L = 0x4f;
  CYC(0x7690, 0x7691); A = mem_rd(gb, HL);
  CYC(0x7691, 0x7693); alu_sub(gb, 0x03);
  CYC(0x7693, 0x7695); alu_cp(gb, 0xfa);
  CYC(0x7695, 0x7697); A = 0x00;
  if (!(F & FC)) {
    CYCT(0x7697, 0x7699);
  } else {
    CYC(0x7697, 0x7699);
    CYC(0x7699, 0x769a); A = alu_inc8(gb, A);
  }
  CYC(0x769a, 0x769c); L = 0x7a;
  CYC(0x769c, 0x769d); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x769d, 0x769e); ret_effect(gb);
    return;
  }
  CYC(0x769d, 0x769e);
  CYC(0x769e, 0x769f); mem_wr(gb, HL, A);
  CYC(0x769f, 0x76a1); L = 0x60;
  CYC(0x76a1, 0x76a3); mem_wr(gb, HL, 0x01);
  CYC(0x76a3, 0x76a6);
  interactionAnimate_hook(gb);
}

static void monkey9_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  CYC(0x7635, 0x7636); H = D;
  CYC(0x7636, 0x7638); L = 0x48;
  CYC(0x7638, 0x763a); A = 0x08;
  CYC(0x763a, 0x763b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x763b, 0x763c); mem_wr(gb, HL, A);
  CYC(0x763c, 0x763e); L = 0x50;
  CYC(0x763e, 0x7640); mem_wr(gb, HL, 0x28);
  CALL_C(0x7640, interactionIncSubstate_hook, 0x23e5, 0x7643);
  CYC(0x7643, 0x7646);
  monkeyJumpSpeed100_hook(gb);
}

static void monkey9_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  CYC(0x7646, 0x7648); C = 0x20;
  CALL_C(0x7648, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x764b);
  if (!(F & FZ)) {
    CYCT(0x764b, 0x764e);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x764b, 0x764e);
  CALL_C(0x764e, monkeyJumpSpeed100_hook, 0x7522, 0x7651);
  CYC(0x7651, 0x7653); L = 0x7c;
  CYC(0x7653, 0x7654); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7654, 0x7655); A = mem_rd(gb, HL);
  CYC(0x7655, 0x7657); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x7657, 0x7658); ret_effect(gb);
    return;
  }
  CYC(0x7657, 0x7658);
  CALL_C(0x7658, interactionIncSubstate_hook, 0x23e5, 0x765b);
  CYC(0x765b, 0x765d); L = 0x78;
  CYC(0x765d, 0x765f); mem_wr(gb, HL, 0x10);
  CYC(0x765f, 0x7660); ret_effect(gb);
}

static void monkey9_disappearance_substate2_hook(GB *gb, uint16_t sp0_) {
  CYC(0x7660, 0x7661); H = D;
  CYC(0x7661, 0x7663); L = 0x78;
  CYC(0x7663, 0x7664); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7664, 0x7665); ret_effect(gb);
    return;
  }
  CYC(0x7664, 0x7665);
  CYC(0x7665, 0x7667); mem_wr(gb, HL, 0x10);
  CALL_C(0x7667, interactionIncSubstate_hook, 0x23e5, 0x766a);
  CYC(0x766a, 0x766c); L = 0x48;
  CYC(0x766c, 0x766d); A = mem_rd(gb, HL);
  CYC(0x766d, 0x766f); alu_xor(gb, 0x10);
  CYC(0x766f, 0x7670); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7670, 0x7671); mem_wr(gb, HL, A);
  CYC(0x7671, 0x7673); L = 0x49;
  CYC(0x7673, 0x7674); A = mem_rd(gb, HL);
  CYC(0x7674, 0x7676); alu_and(gb, 0x10);
  CYC(0x7676, 0x7678); A = 0x03;
  if (!(F & FZ)) {
    CYCT(0x7678, 0x767a);
  } else {
    CYC(0x7678, 0x767a);
    CYC(0x767a, 0x767c); A = 0x08;
  }
  CYC(0x767c, 0x767f);
  monkeySetAnimationAndJump_hook(gb);
}

static void monkey9_disappearance_substate3_hook(GB *gb, uint16_t sp0_) {
  CYC(0x767f, 0x7680); H = D;
  CYC(0x7680, 0x7682); L = 0x78;
  CYC(0x7682, 0x7683); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7683, 0x7684); ret_effect(gb);
    return;
  }
  CYC(0x7683, 0x7684);
  CYC(0x7684, 0x7686); L = 0x7c;
  CYC(0x7686, 0x7688); mem_wr(gb, HL, 0x00);
  CYC(0x7688, 0x768a); L = 0x45;
  CYC(0x768a, 0x768b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x768b, 0x768c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x768c, 0x768d); ret_effect(gb);
}

void monkey9Disappearance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x760f, monkeyCheckChangeAnimation_hook, 0x768d, 0x7612);
  CYC(0x7612, 0x7614); E = 0x45;
  CYC(0x7614, 0x7615); A = mem_rd(gb, DE);
  CYC(0x7615, 0x7617); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(0x7617, 0x7619);
  } else {
    CYC(0x7617, 0x7619);
    CALL_C(0x7619, interactionDecCounter1_hook, 0x23cc, 0x761c);
    if (!(F & FZ)) {
      CYCT(0x761c, 0x761e);
    } else {
      CYC(0x761c, 0x761e);
      CALL_C(0x761e, monkeyBeginDisappearing_hook, 0x7598, 0x7621);
      CYC(0x7621, 0x7623); L = 0x45;
      CYC(0x7623, 0x7625); mem_wr(gb, HL, 0x04);
    }
  }
  CYC(0x7625, 0x7627); E = 0x45;
  CYC(0x7627, 0x7628); A = mem_rd(gb, DE);
  CYC(0x7628, 0x7629); push_effect(gb, 0x7629);
  switch (monkey_jump_table(gb)) {
    case 0x7635: monkey9_disappearance_substate0_hook(gb, sp0_); return;
    case 0x7646: monkey9_disappearance_substate1_hook(gb, sp0_); return;
    case 0x7660: monkey9_disappearance_substate2_hook(gb, sp0_); return;
    case 0x767f: monkey9_disappearance_substate3_hook(gb, sp0_); return;
    case 0x75aa: monkeyWaitBeforeFlickering_hook(gb); return;
    case 0x75b3: monkeyFlickerUntilDeletion_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey8_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x76b6, interactionDecCounter1_hook, 0x23cc, 0x76b9);
  if (F & FZ) {
    CYC(0x76b9, 0x76bb);
    CYC(0x76bb, 0x76bd); mem_wr(gb, HL, 0x5a);
    CALL_C(0x76bd, interactionIncSubstate_hook, 0x23e5, 0x76c0);
    CYC(0x76c0, 0x76c3); SET_BC(0xf3f8);
    CYC(0x76c3, 0x76c5); A = 0x3c;
    CYC(0x76c5, 0x76c8);
    objectCreateExclamationMark_hook(gb);
    return;
  }
  CYCT(0x76b9, 0x76bb);
  CYC(0x76c8, 0x76cb); A = W8(wFrameCounter);
  CYC(0x76cb, 0x76cd); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x76cd, 0x76ce); ret_effect(gb);
    return;
  }
  CYC(0x76cd, 0x76ce);
  CYC(0x76ce, 0x76d1);
  interactionAnimate_hook(gb);
}

static void monkey8_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x76d1, interactionDecCounter1_hook, 0x23cc, 0x76d4);
  if (!(F & FZ)) {
    CYCT(0x76d4, 0x76d5); ret_effect(gb);
    return;
  }
  CYC(0x76d4, 0x76d5);
  CYC(0x76d5, 0x76d7); mem_wr(gb, HL, 0xb4);
  CYC(0x76d7, 0x76da);
  interactionIncSubstate_hook(gb);
}

static void monkey8_disappearance_substate2_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x76da, interactionDecCounter1_hook, 0x23cc, 0x76dd);
  if (F & FZ) {
    CYC(0x76dd, 0x76df);
    CYC(0x76df, 0x76e2);
    monkeyBeginDisappearing_hook(gb);
    return;
  }
  CYCT(0x76dd, 0x76df);
  CYC(0x76e2, 0x76e5); A = W8(wFrameCounter);
  CYC(0x76e5, 0x76e7); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x76e7, 0x76e8); ret_effect(gb);
    return;
  }
  CYC(0x76e7, 0x76e8);
  CYC(0x76e8, 0x76ea); L = 0x48;
  CYC(0x76ea, 0x76eb); A = mem_rd(gb, HL);
  CYC(0x76eb, 0x76ed); alu_xor(gb, 0x01);
  CYC(0x76ed, 0x76ee); mem_wr(gb, HL, A);
  CYC(0x76ee, 0x76f1);
  interactionSetAnimation_hook(gb);
}

static void monkey8_disappearance_substate3_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x76f1, interactionDecCounter1_hook, 0x23cc, 0x76f4);
  if (F & FZ) {
    CYC(0x76f4, 0x76f6);
    CYC(0x76f6, 0x76f8); mem_wr(gb, HL, 0x1e);
    CALL_C(0x76f8, objectSetInvisible_hook, 0x1e7b, 0x76fb);
    CYC(0x76fb, 0x76fe);
    interactionIncSubstate_hook(gb);
    return;
  }
  CYCT(0x76f4, 0x76f6);
  CYC(0x76fe, 0x7700); B = 0x01;
  CYC(0x7700, 0x7703);
  objectFlickerVisibility_hook(gb);
}

static void monkey8_disappearance_substate4_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x7703, interactionDecCounter1_hook, 0x23cc, 0x7706);
  if (!(F & FZ)) {
    CYCT(0x7706, 0x7707); ret_effect(gb);
    return;
  }
  CYC(0x7706, 0x7707);
  CYC(0x7707, 0x7709); A = 0xff;
  CYC(0x7709, 0x770c); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(0x770c, 0x770f);
  interactionDelete_hook(gb);
}

void monkey8Disappearance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76a6, 0x76a8); E = 0x45;
  CYC(0x76a8, 0x76a9); A = mem_rd(gb, DE);
  CYC(0x76a9, 0x76aa); push_effect(gb, 0x76aa);
  switch (monkey_jump_table(gb)) {
    case 0x76b6: monkey8_disappearance_substate0_hook(gb, sp0_); return;
    case 0x76d1: monkey8_disappearance_substate1_hook(gb, sp0_); return;
    case 0x76da: monkey8_disappearance_substate2_hook(gb, sp0_); return;
    case 0x75aa: monkeyWaitBeforeFlickering_hook(gb); return;
    case 0x76f1: monkey8_disappearance_substate3_hook(gb, sp0_); return;
    case 0x7703: monkey8_disappearance_substate4_hook(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void monkeySubid2State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x770f, interactionRunScript_hook, 0x2552, 0x7712);
  CYC(0x7712, 0x7715);
  interactionAnimateAsNpc_hook(gb);
}

void monkeySubid1State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x755d, 0x755f); E = 0x43;
  CYC(0x755f, 0x7560); A = mem_rd(gb, DE);
  CYC(0x7560, 0x7561); push_effect(gb, 0x7561);
  switch (monkey_jump_table(gb)) {
    case 0x7575: monkey0Disappearance_hook(gb); return;
    case 0x75c0: monkey3Disappearance_hook(gb); return;
    case 0x75d2: monkey5Disappearance_hook(gb); return;
    case 0x76a6: monkey8Disappearance_hook(gb); return;
    case 0x760f: monkey9Disappearance_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
