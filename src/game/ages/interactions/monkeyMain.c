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

static void monkey_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
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

static void monkey_subid0_substate0_hook(GB *gb, uint16_t sp0_) {
  CYC(0x74e7, 0x74ea); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x74ea, 0x74ec); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x74ec, 0x74ef);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(0x74ec, 0x74ef);
  CALL_C(0x74ef, interactionIncSubstate_hook, 0x23e5, 0x74f2);
  CYC(0x74f2, 0x74f4); A = 0x06;
  CYC(0x74f4, 0x74f7);
  interactionSetAnimation_hook(gb);
}

static void monkey_subid0_substate1_hook(GB *gb, uint16_t sp0_) {
  CYC(0x74f7, 0x74fa); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x74fa, 0x74fc); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x74fc, 0x74fd); ret_effect(gb);
    return;
  }
  CYC(0x74fc, 0x74fd);
  CALL_C(0x74fd, interactionIncSubstate_hook, 0x23e5, 0x7500);
  CYC(0x7500, 0x7502); L = 0x46;
  CYC(0x7502, 0x7504); mem_wr(gb, HL, 0x32);
  CYC(0x7504, 0x7506); A = 0x03;
  CALL_C(0x7506, interactionSetAnimation_hook, 0x262e, 0x7509);
  CYC(0x7509, 0x750b);
  monkeyJumpSpeed120_hook(gb);
}

static void monkey_subid0_substate2_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x750b, interactionDecCounter1_hook, 0x23cc, 0x750e);
  if (!(F & FZ)) {
    CYCT(0x750e, 0x7510);
    monkeyUpdateGravityAndHop_hook(gb);
    return;
  }
  CYC(0x750e, 0x7510);
  CALL_C(0x7510, interactionIncSubstate_hook, 0x23e5, 0x7513);
  CYC(0x7513, 0x7515); L = 0x49;
  CYC(0x7515, 0x7517); mem_wr(gb, HL, 0x02);
  CYC(0x7517, 0x7519); L = 0x4f;
  CYC(0x7519, 0x751b); mem_wr(gb, HL, 0x00);
  CYC(0x751b, 0x751d); L = 0x50;
  CYC(0x751d, 0x751f); mem_wr(gb, HL, 0x3c);
  monkeySetAnimationAndJump_hook(gb);
}

void monkeySubid0State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x74ce, interactionAnimate_hook, 0x261b, 0x74d1);
  CALL_C(0x74d1, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x74d4);
  CYC(0x74d4, 0x74d6); E = 0x45;
  CYC(0x74d6, 0x74d7); A = mem_rd(gb, DE);
  CYC(0x74d7, 0x74d8); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(0x74d8, objectPreventLinkFromPassing_hook, 0x2680, 0x74db);
  else CYC(0x74d8, 0x74db);
  CYC(0x74db, 0x74dd); E = 0x45;
  CYC(0x74dd, 0x74de); A = mem_rd(gb, DE);
  CYC(0x74de, 0x74df); push_effect(gb, 0x74df);
  switch (monkey_jump_table(gb)) {
    case 0x74e7: monkey_subid0_substate0_hook(gb, sp0_); return;
    case 0x74f7: monkey_subid0_substate1_hook(gb, sp0_); return;
    case 0x750b: monkey_subid0_substate2_hook(gb, sp0_); return;
    case 0x7528: monkeySubid0State1Substate3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey_subid4_substate0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x773b, interactionDecCounter2_hook, 0x23d1, 0x773e);
  if (!(F & FZ)) {
    CYCT(0x773e, 0x773f); ret_effect(gb);
    return;
  }
  CYC(0x773e, 0x773f);
  CYC(0x773f, 0x7742);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_substate1_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x7742, interactionDecCounter1_hook, 0x23cc, 0x7745);
  if (!(F & FZ)) {
    CYCT(0x7745, 0x7746); ret_effect(gb);
    return;
  }
  CYC(0x7745, 0x7746);
  CYC(0x7746, 0x7748); mem_wr(gb, HL, 0x3c);
  CYC(0x7748, 0x774a); L = 0x43;
  CYC(0x774a, 0x774b); A = mem_rd(gb, HL);
  CYC(0x774b, 0x774d); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYC(0x774d, 0x774f);
    CYC(0x774f, 0x7751); A = 0x00;
    CALL_C(0x7751, objectGetRelatedObject2Var_hook, 0x2164, 0x7754);
    CYC(0x7754, 0x7756); L = 0x5c;
    CYC(0x7756, 0x7758); mem_wr(gb, HL, 0x06);
  } else {
    CYCT(0x774d, 0x774f);
  }
  CYC(0x7758, 0x775a); A = 0x90;
  CALL_C(0x775a, playSound_b00_hook, 0x0c98, 0x775d);
  CYC(0x775d, 0x7760);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_substate2_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x7760, interactionDecCounter1_hook, 0x23cc, 0x7763);
  if (!(F & FZ)) {
    CYCT(0x7763, 0x7765);
    CYC(0x776d, 0x776f); B = 0x01;
    CYC(0x776f, 0x7772);
    objectFlickerVisibility_hook(gb);
    return;
  }
  CYC(0x7763, 0x7765);
  CYC(0x7765, 0x7767); mem_wr(gb, HL, 0x3c);
  CALL_C(0x7767, objectSetVisible_hook, 0x1e84, 0x776a);
  CYC(0x776a, 0x776d);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_finish_substate3_hook(GB *gb) {
  CYC(0x7794, 0x7796); L = 0x5c;
  CYC(0x7796, 0x7797); mem_wr(gb, HL, B);
  CYC(0x7797, 0x779a);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_substate3_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x7772, interactionDecCounter1_hook, 0x23cc, 0x7775);
  if (!(F & FZ)) {
    CYCT(0x7775, 0x7776); ret_effect(gb);
    return;
  }
  CYC(0x7775, 0x7776);
  CYC(0x7776, 0x7778); B = 0x03;
  CYC(0x7778, 0x777a); L = 0x43;
  CYC(0x777a, 0x777b); A = mem_rd(gb, HL);
  CYC(0x777b, 0x777d); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYC(0x777d, 0x777f);
    CYC(0x777f, 0x7780); B = alu_dec8(gb, B);
    CYC(0x7780, 0x7782);
    monkey_subid4_finish_substate3_hook(gb);
    return;
  }
  CYCT(0x777d, 0x777f);
  CYC(0x7782, 0x7784); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYC(0x7784, 0x7786);
    CYC(0x7786, 0x7788); A = 0x00;
    CALL_C(0x7788, objectGetRelatedObject2Var_hook, 0x2164, 0x778b);
    CYC(0x778b, 0x778d); L = 0x5c;
    CYC(0x778d, 0x778f); mem_wr(gb, HL, 0x02);
    CYC(0x778f, 0x7790); H = D;
    CYC(0x7790, 0x7792); L = 0x46;
    CYC(0x7792, 0x7794); mem_wr(gb, HL, 0xb4);
  } else {
    CYCT(0x7784, 0x7786);
  }
  monkey_subid4_finish_substate3_hook(gb);
}

static void monkey_subid4_substate4_1_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x779d, interactionAnimate_hook, 0x261b, 0x77a0);
  CYC(0x77a0, 0x77a2); E = 0x43;
  CYC(0x77a2, 0x77a3); A = mem_rd(gb, DE);
  CYC(0x77a3, 0x77a5); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x77a5, 0x77a6); ret_effect(gb);
    return;
  }
  CYC(0x77a5, 0x77a6);
  CALL_C(0x77a6, interactionDecCounter1_hook, 0x23cc, 0x77a9);
  if (!(F & FZ)) {
    CYCT(0x77a9, 0x77aa); ret_effect(gb);
    return;
  }
  CYC(0x77a9, 0x77aa);
  CYC(0x77aa, 0x77ac); A = 0xff;
  CYC(0x77ac, 0x77af); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(0x77af, 0x77b0); ret_effect(gb);
}

static void monkey_subid4_substate4_0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x779a, monkeyUpdateGravityAndJumpIfLanded_hook, 0x754d, 0x779d);
  monkey_subid4_substate4_1_hook(gb, sp0_);
}

static void monkey_subid4_monkey0_hook(GB *gb, uint16_t sp0_) {
  CYC(0x772d, 0x772f); E = 0x45;
  CYC(0x772f, 0x7730); A = mem_rd(gb, DE);
  CYC(0x7730, 0x7731); push_effect(gb, 0x7731);
  switch (monkey_jump_table(gb)) {
    case 0x773b: monkey_subid4_substate0_hook(gb, sp0_); return;
    case 0x7742: monkey_subid4_substate1_hook(gb, sp0_); return;
    case 0x7760: monkey_subid4_substate2_hook(gb, sp0_); return;
    case 0x7772: monkey_subid4_substate3_hook(gb, sp0_); return;
    case 0x779a: monkey_subid4_substate4_0_hook(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey_subid4_monkey3_hook(GB *gb, uint16_t sp0_) {
  CYC(0x77b0, 0x77b2); E = 0x45;
  CYC(0x77b2, 0x77b3); A = mem_rd(gb, DE);
  CYC(0x77b3, 0x77b4); push_effect(gb, 0x77b4);
  switch (monkey_jump_table(gb)) {
    case 0x773b: monkey_subid4_substate0_hook(gb, sp0_); return;
    case 0x7742: monkey_subid4_substate1_hook(gb, sp0_); return;
    case 0x7760: monkey_subid4_substate2_hook(gb, sp0_); return;
    case 0x7772: monkey_subid4_substate3_hook(gb, sp0_); return;
    case 0x779d: monkey_subid4_substate4_1_hook(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey_subid4_monkey9_hook(GB *gb, uint16_t sp0_) {
  CYC(0x77be, 0x77c0); E = 0x45;
  CYC(0x77c0, 0x77c1); A = mem_rd(gb, DE);
  CYC(0x77c1, 0x77c3); alu_cp(gb, 0x04);
  if (!(F & FC)) CALL_C_CC(0x77c3, monkeyCheckChangeAnimation_hook, 0x768d, 0x77c6);
  else CYC(0x77c3, 0x77c6);
  CYC(0x77c6, 0x77c8); E = 0x45;
  CYC(0x77c8, 0x77c9); A = mem_rd(gb, DE);
  CYC(0x77c9, 0x77ca); push_effect(gb, 0x77ca);
  switch (monkey_jump_table(gb)) {
    case 0x773b: monkey_subid4_substate0_hook(gb, sp0_); return;
    case 0x7742: monkey_subid4_substate1_hook(gb, sp0_); return;
    case 0x7760: monkey_subid4_substate2_hook(gb, sp0_); return;
    case 0x7772: monkey_subid4_substate3_hook(gb, sp0_); return;
    case 0x7635: monkey9_disappearance_substate0_hook(gb, sp0_); return;
    case 0x7646: monkey9_disappearance_substate1_hook(gb, sp0_); return;
    case 0x7660: monkey9_disappearance_substate2_hook(gb, sp0_); return;
    case 0x767f: monkey9_disappearance_substate3_hook(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void monkeySubid4State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7715, 0x7717); E = 0x43;
  CYC(0x7717, 0x7718); A = mem_rd(gb, DE);
  CYC(0x7718, 0x7719); push_effect(gb, 0x7719);
  switch (monkey_jump_table(gb)) {
    case 0x772d: monkey_subid4_monkey0_hook(gb, sp0_); return;
    case 0x77b0: monkey_subid4_monkey3_hook(gb, sp0_); return;
    case 0x77be: monkey_subid4_monkey9_hook(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void monkeyAnimateAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x77f5, interactionRunScript_hook, 0x2552, 0x77f8);
  CYC(0x77f8, 0x77fb);
  interactionAnimateAsNpc_hook(gb);
}

void monkeySubid5State1_monkey9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77fb, interactionRunScript_hook, 0x2552, 0x77fe);
  CALL_C(0x77fe, monkeyCheckChangeAnimation_hook, 0x768d, 0x7801);
  CALL_C(0x7801, objectPushLinkAwayOnCollision_hook, 0x230e, 0x7804);
  CALL_C(0x7804, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x7807);
  CYC(0x7807, 0x7809); E = 0x45;
  CYC(0x7809, 0x780a); A = mem_rd(gb, DE);
  CYC(0x780a, 0x780b); push_effect(gb, 0x780b);
  switch (monkey_jump_table(gb)) {
    case 0x7635: monkey9_disappearance_substate0_hook(gb, sp0_); return;
    case 0x7646: monkey9_disappearance_substate1_hook(gb, sp0_); return;
    case 0x7660: monkey9_disappearance_substate2_hook(gb, sp0_); return;
    case 0x767f: monkey9_disappearance_substate3_hook(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey_subid5_monkey0_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x77f2, monkeyUpdateGravityAndJumpIfLanded_hook, 0x754d, 0x77f5);
  monkeyAnimateAndRunScript_hook(gb);
}

void monkeySubid5State1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x77da, 0x77dc); E = 0x43;
  CYC(0x77dc, 0x77dd); A = mem_rd(gb, DE);
  CYC(0x77dd, 0x77de); push_effect(gb, 0x77de);
  switch (monkey_jump_table(gb)) {
    case 0x77f2: monkey_subid5_monkey0_hook(gb, sp0_); return;
    case 0x77f5: monkeyAnimateAndRunScript_hook(gb); return;
    case 0x77fb: monkeySubid5State1_monkey9_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void monkeyState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74ba, 0x74bc); E = 0x42;
  CYC(0x74bc, 0x74bd); A = mem_rd(gb, DE);
  CYC(0x74bd, 0x74be); push_effect(gb, 0x74be);
  switch (monkey_jump_table(gb)) {
    case 0x74ce: monkeySubid0State1_hook(gb); return;
    case 0x755d: monkeySubid1State1_hook(gb); return;
    case 0x770f: monkeySubid2State1_hook(gb); return;
    case 0x7715: monkeySubid4State1_hook(gb); return;
    case 0x77da: monkeySubid5State1_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x77f5: monkeyAnimateAndRunScript_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey_init_subid23_shared(GB *gb, uint16_t sp0_) {
  CYC(0x733d, 0x733f); A = 0x0a;
  CALL_C(0x733f, checkGlobalFlag_hook, 0x31f3, 0x7342);
  if (!(F & FZ)) {
    CYCT(0x7342, 0x7345);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7342, 0x7345);
  CYC(0x7345, 0x7347); E = 0x42;
  CYC(0x7347, 0x7348); A = mem_rd(gb, DE);
  CYC(0x7348, 0x734a); alu_sub(gb, 0x02);
  CYC(0x734a, 0x734d); SET_HL(0x7813);
  CYC(0x734d, 0x734e); monkey_add_double_index_from_rst(gb, 0x734e);
  CYC(0x734e, 0x734f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x734f, 0x7350); H = mem_rd(gb, HL);
  CYC(0x7350, 0x7351); L = A;
  CYC(0x7351, 0x7354);
  interactionSetScript_hook(gb);
}

static void monkey_init_subid0(GB *gb, uint16_t sp0_) {
  CYC(0x7321, 0x7323); A = 0x02;
  CALL_C(0x7323, interactionSetAnimation_hook, 0x262e, 0x7326);
  CYC(0x7326, 0x7329); SET_HL(0x5ade);
  CYC(0x7329, 0x732c);
  interactionSetScript_hook(gb);
}

static void monkey_init_subid2(GB *gb, uint16_t sp0_) {
  CYC(0x732c, 0x732e); A = 0x02;
  CYC(0x732e, 0x7330); E = 0x5c;
  CYC(0x7330, 0x7331); mem_wr(gb, DE, A);
  CYC(0x7331, 0x7333); A = 0x06;
  CALL_C(0x7333, interactionSetAnimation_hook, 0x262e, 0x7336);
  CYC(0x7336, 0x7338);
  monkey_init_subid23_shared(gb, sp0_);
}

static void monkey_init_subid3(GB *gb, uint16_t sp0_) {
  CYC(0x7338, 0x733a); A = 0x07;
  CALL_C(0x733a, interactionSetAnimation_hook, 0x262e, 0x733d);
  monkey_init_subid23_shared(gb, sp0_);
}

static void monkey_init_subid1(GB *gb, uint16_t sp0_) {
  CYC(0x7354, 0x7356); E = 0x43;
  CYC(0x7356, 0x7357); A = mem_rd(gb, DE);
  CYC(0x7357, 0x7358); C = A;
  CYC(0x7358, 0x7359); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7359, 0x735b);
  } else {
    CYC(0x7359, 0x735b);
    CYC(0x735b, 0x735c); E = alu_dec8(gb, E);
    CYC(0x735c, 0x735d); A = mem_rd(gb, DE);
    CYC(0x735d, 0x735f); alu_cp(gb, 0x05);
    if (F & FZ) {
      CYCT(0x735f, 0x7361);
    } else {
      CYC(0x735f, 0x7361);
      CYC(0x7361, 0x7362); push_effect(gb, BC);
      CYC(0x7362, 0x7364); A = 0xad;
      CALL_C(0x7364, loadPaletteHeader_hook, 0x050b, 0x7367);
      CYC(0x7367, 0x7368); SET_BC(pop_effect(gb));
    }
    CYC(0x7368, 0x736a); B = 0x09;
    for (;;) {
      CALL_C(0x736a, getFreeInteractionSlot_hook, 0x3aef, 0x736d);
      if (!(F & FZ)) {
        CYCT(0x736d, 0x736f);
        break;
      }
      CYC(0x736d, 0x736f);
      CYC(0x736f, 0x7371); mem_wr(gb, HL, 0x39);
      CYC(0x7371, 0x7372); L = alu_inc8(gb, L);
      CYC(0x7372, 0x7374); E = 0x42;
      CYC(0x7374, 0x7375); A = mem_rd(gb, DE);
      CYC(0x7375, 0x7376); mem_wr(gb, HL, A);
      CYC(0x7376, 0x7377); L = alu_inc8(gb, L);
      CYC(0x7377, 0x7378); mem_wr(gb, HL, B);
      CYC(0x7378, 0x7379); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(0x7379, 0x737b);
      } else {
        CYC(0x7379, 0x737b);
        break;
      }
    }
  }
  CYC(0x737b, 0x737c); A = C;
  CYC(0x737c, 0x737d); alu_add(gb, A);
  CYC(0x737d, 0x7380); SET_HL(0x73bd);
  CYC(0x7380, 0x7381); monkey_add_double_index_from_rst(gb, 0x7381);
  CYC(0x7381, 0x7382); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7382, 0x7384); E = 0x4b;
  CYC(0x7384, 0x7385); mem_wr(gb, DE, A);
  CYC(0x7385, 0x7386); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7386, 0x7388); E = 0x4d;
  CYC(0x7388, 0x7389); mem_wr(gb, DE, A);
  CYC(0x7389, 0x738a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x738a, 0x738c); E = 0x46;
  CYC(0x738c, 0x738d); mem_wr(gb, DE, A);
  CYC(0x738d, 0x738e); A = mem_rd(gb, HL);
  CALL_C(0x738e, interactionSetAnimation_hook, 0x262e, 0x7391);
  CALL_C(0x7391, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7394);
  CYC(0x7394, 0x7396); alu_and(gb, 0x0f);
  CYC(0x7396, 0x7397); H = D;
  CYC(0x7397, 0x7399); L = 0x47;
  CYC(0x7399, 0x739a); mem_wr(gb, HL, A);
  CYC(0x739a, 0x739c); alu_sub(gb, 0x07);
  CYC(0x739c, 0x739e); L = 0x60;
  CYC(0x739e, 0x739f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x739f, 0x73a0); mem_wr(gb, HL, A);
  CALL_C(0x73a0, getRandomNumber_hook, 0x043e, 0x73a3);
  CYC(0x73a3, 0x73a5); alu_and(gb, 0x03);
  CYC(0x73a5, 0x73a8); SET_BC(0x73b5);
  CALL_C(0x73a8, addDoubleIndexToBc_hook, 0x007e, 0x73ab);
  CYC(0x73ab, 0x73ad); L = 0x78;
  CYC(0x73ad, 0x73ae); A = mem_rd(gb, BC);
  CYC(0x73ae, 0x73af); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x73af, 0x73b0); SET_BC(BC + 1);
  CYC(0x73b0, 0x73b1); A = mem_rd(gb, BC);
  CYC(0x73b1, 0x73b2); mem_wr(gb, HL, A);
  CYC(0x73b2, 0x73b5);
  monkeySetJumpSpeed_hook(gb);
}

static void monkey_init_bowtie(GB *gb, uint16_t sp0_) {
  CYC(0x740b, 0x740d); A = 0x07;
  CALL_C(0x740d, interactionSetAnimation_hook, 0x262e, 0x7410);
  CALL_C(0x7410, getFreeInteractionSlot_hook, 0x3aef, 0x7413);
  if (!(F & FZ)) {
    CYCT(0x7413, 0x7414); ret_effect(gb);
    return;
  }
  CYC(0x7413, 0x7414);
  CYC(0x7414, 0x7416); mem_wr(gb, HL, 0x63);
  CYC(0x7416, 0x7417); L = alu_inc8(gb, L);
  CYC(0x7417, 0x7419); mem_wr(gb, HL, 0x3d);
  CYC(0x7419, 0x741a); L = alu_inc8(gb, L);
  CYC(0x741a, 0x741c); mem_wr(gb, HL, 0x01);
  CYC(0x741c, 0x741e); L = 0x56;
  CYC(0x741e, 0x7420); mem_wr(gb, HL, 0x40);
  CYC(0x7420, 0x7421); L = alu_inc8(gb, L);
  CYC(0x7421, 0x7422); mem_wr(gb, HL, D);
  CYC(0x7422, 0x7424); E = 0x59;
  CYC(0x7424, 0x7425); A = H;
  CYC(0x7425, 0x7426); mem_wr(gb, DE, A);
  CYC(0x7426, 0x7427); ret_effect(gb);
}

static void monkey_init_subid4(GB *gb, uint16_t sp0_) {
  CALL_C(0x73e5, objectSetInvisible_hook, 0x1e7b, 0x73e8);
  CYC(0x73e8, 0x73eb); push_effect(gb, 0x73eb);
  monkey_init_subid1(gb, sp0_);
  CYC(0x73eb, 0x73ed); L = 0x5c;
  CYC(0x73ed, 0x73ef); mem_wr(gb, HL, 0x06);
  CYC(0x73ef, 0x73f1); L = 0x47;
  CYC(0x73f1, 0x73f3); mem_wr(gb, HL, 0x3c);
  CYC(0x73f3, 0x73f5); L = 0x43;
  CYC(0x73f5, 0x73f6); A = mem_rd(gb, HL);
  CYC(0x73f6, 0x73f8); alu_cp(gb, 0x09);
  if (F & FZ) {
    CYC(0x73f8, 0x73fa);
    CYC(0x73fa, 0x73fc); L = 0x7c;
    CYC(0x73fc, 0x73fd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x73fd, 0x7400); SET_BC(0x6424);
    CYC(0x7400, 0x7403);
    interactionSetPosition_hook(gb);
    return;
  }
  CYCT(0x73f8, 0x73fa);
  CYC(0x7403, 0x7405); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x7405, 0x7406); ret_effect(gb);
    return;
  }
  CYC(0x7405, 0x7406);
  CYC(0x7406, 0x7408); A = 0xfa;
  CYC(0x7408, 0x740a); E = 0x46;
  CYC(0x740a, 0x740b); mem_wr(gb, DE, A);
  monkey_init_bowtie(gb, sp0_);
}

static void monkey_init_subid5(GB *gb, uint16_t sp0_) {
  CYC(0x7427, 0x7429); A = 0x11;
  CALL_C(0x7429, checkGlobalFlag_hook, 0x31f3, 0x742c);
  if (F & FZ) {
    CYCT(0x742c, 0x742f);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x742c, 0x742f);
  CYC(0x742f, 0x7432); push_effect(gb, 0x7432);
  monkey_init_subid1(gb, sp0_);
  CYC(0x7432, 0x7434); L = 0x46;
  CYC(0x7434, 0x7435); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7435, 0x7436); mem_wr(gb, HL, A);
  CYC(0x7436, 0x7439); SET_HL(0x5af4);
  CYC(0x7439, 0x743b); E = 0x43;
  CYC(0x743b, 0x743c); A = mem_rd(gb, DE);
  CYC(0x743c, 0x743e); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x743e, 0x7440);
  } else {
    CYC(0x743e, 0x7440);
    CYC(0x7440, 0x7441); push_effect(gb, AF);
    CYC(0x7441, 0x7444); push_effect(gb, 0x7444);
    monkey_init_bowtie(gb, sp0_);
    CYC(0x7444, 0x7447); SET_HL(0x5b06);
    CYC(0x7447, 0x7448); SET_AF(pop_effect(gb));
  }
  CYC(0x7448, 0x744a); alu_cp(gb, 0x05);
  CYC(0x744a, 0x744c); A = 0x03;
  if (!(F & FZ)) {
    CYCT(0x744c, 0x744e);
  } else {
    CYC(0x744c, 0x744e);
    CYC(0x744e, 0x7450); A = 0x02;
  }
  CYC(0x7450, 0x7452); E = 0x5c;
  CYC(0x7452, 0x7453); mem_wr(gb, DE, A);
  CYC(0x7453, 0x7456);
  interactionSetScript_hook(gb);
}

static void monkey_init_set_animation(GB *gb) {
  CYC(0x74b4, 0x74b6); E = 0x7a;
  CYC(0x74b6, 0x74b7); mem_wr(gb, DE, A);
  CYC(0x74b7, 0x74ba);
  interactionSetAnimation_hook(gb);
}

static void monkey_init_subid7(GB *gb, uint16_t sp0_) {
  CYC(0x745b, 0x745d); E = 0x43;
  CYC(0x745d, 0x745e); A = mem_rd(gb, DE);
  CYC(0x745e, 0x745f); push_effect(gb, 0x745f);
  switch (monkey_jump_table(gb)) {
    case 0x7465:
      CYC(0x7465, 0x7467); A = 0x14;
      CALL_C(0x7467, checkGlobalFlag_hook, 0x31f3, 0x746a);
      if (!(F & FZ)) {
        CYCT(0x746a, 0x746d); interactionDelete_hook(gb); return;
      }
      CYC(0x746a, 0x746d);
      CYC(0x746d, 0x746f); A = 0x11;
      CALL_C(0x746f, checkGlobalFlag_hook, 0x31f3, 0x7472);
      if (F & FZ) {
        CYCT(0x7472, 0x7475); interactionDelete_hook(gb); return;
      }
      CYC(0x7472, 0x7475);
      CYC(0x7475, 0x7478); SET_HL(0x5b0e);
      CALL_C(0x7478, interactionSetScript_hook, 0x2544, 0x747b);
      CYC(0x747b, 0x747d); A = 0x06;
      CYC(0x747d, 0x747f);
      monkey_init_set_animation(gb);
      return;
    case 0x747f:
      CYC(0x747f, 0x7481); A = 0x14;
      CALL_C(0x7481, checkGlobalFlag_hook, 0x31f3, 0x7484);
      if (F & FZ) {
        CYCT(0x7484, 0x7487); interactionDelete_hook(gb); return;
      }
      CYC(0x7484, 0x7487);
      CYC(0x7487, 0x748a); SET_HL(0x5b1a);
      CALL_C(0x748a, interactionSetScript_hook, 0x2544, 0x748d);
      CYC(0x748d, 0x748f); A = 0x05;
      CYC(0x748f, 0x7491);
      monkey_init_set_animation(gb);
      return;
    case 0x7491:
      CYC(0x7491, 0x7493); A = 0x14;
      CALL_C(0x7493, checkGlobalFlag_hook, 0x31f3, 0x7496);
      if (!(F & FZ)) {
        CYCT(0x7496, 0x7499); interactionDelete_hook(gb); return;
      }
      CYC(0x7496, 0x7499);
      CYC(0x7499, 0x749b); A = 0x12;
      CALL_C(0x749b, checkGlobalFlag_hook, 0x31f3, 0x749e);
      if (F & FZ) {
        CYCT(0x749e, 0x74a1); interactionDelete_hook(gb); return;
      }
      CYC(0x749e, 0x74a1);
      CYC(0x74a1, 0x74a3); A = 0x11;
      CALL_C(0x74a3, checkGlobalFlag_hook, 0x31f3, 0x74a6);
      CYC(0x74a6, 0x74a9); SET_HL(0x5b26);
      if (F & FZ) {
        CYCT(0x74a9, 0x74ac);
      } else {
        CYC(0x74a9, 0x74ac);
        CYC(0x74ac, 0x74af); SET_HL(0x5b32);
      }
      CALL_C(0x74af, interactionSetScript_hook, 0x2544, 0x74b2);
      CYC(0x74b2, 0x74b4); A = 0x05;
      monkey_init_set_animation(gb);
      return;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }
}

static void monkey_init_subid(GB *gb, uint16_t sp0_) {
  CYC(0x730d, 0x730f); E = 0x42;
  CYC(0x730f, 0x7310); A = mem_rd(gb, DE);
  CYC(0x7310, 0x7311); push_effect(gb, 0x7311);
  switch (monkey_jump_table(gb)) {
    case 0x7321: monkey_init_subid0(gb, sp0_); return;
    case 0x7354: monkey_init_subid1(gb, sp0_); return;
    case 0x732c: monkey_init_subid2(gb, sp0_); return;
    case 0x7338: monkey_init_subid3(gb, sp0_); return;
    case 0x73e5: monkey_init_subid4(gb, sp0_); return;
    case 0x7427: monkey_init_subid5(gb, sp0_); return;
    case 0x7456:
      CYC(0x7456, 0x7458); A = 0x05;
      CYC(0x7458, 0x745b); interactionSetAnimation_hook(gb);
      return;
    case 0x745b: monkey_init_subid7(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void monkey_state0_before_init_subid_hook(GB *gb, uint16_t sp0_) {
  CALL_C(0x72f9, objectSetVisiblec2_hook, 0x1e45, 0x72fc);
  CYC(0x72fc, 0x72ff); push_effect(gb, 0x72ff);
  monkey_init_subid(gb, sp0_);
}

static void monkey_state0_after_init_subid_hook(GB *gb) {
  CYC(0x72ff, 0x7301); E = 0x43;
  CYC(0x7301, 0x7302); A = mem_rd(gb, DE);
  CYC(0x7302, 0x7304); alu_cp(gb, 0x09);
  if (F & FZ) {
    CYCT(0x7304, 0x7305); ret_effect(gb);
    return;
  }
  CYC(0x7304, 0x7305);
  CYC(0x7305, 0x7307); E = 0x40;
  CYC(0x7307, 0x7308); A = mem_rd(gb, DE);
  CYC(0x7308, 0x7309); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7309, 0x730c); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x7309, 0x730c);
  CYC(0x730c, 0x730d); ret_effect(gb);
}

void interactionCode39_body__afterCall72f9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  monkey_state0_before_init_subid_hook(gb, sp0_);
  if (!(gb->pc == 0x72ff && gb->sp == sp0_)) return;
  monkey_state0_after_init_subid_hook(gb);
}

void interactionCode39_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72e6, 0x72e8); E = 0x44;
  CYC(0x72e8, 0x72e9); A = mem_rd(gb, DE);
  CYC(0x72e9, 0x72ea); push_effect(gb, 0x72ea);
  switch (monkey_jump_table(gb)) {
    case 0x72ee:
      CYC(0x72ee, 0x72f0); A = 0x01;
      CYC(0x72f0, 0x72f1); mem_wr(gb, DE, A);
      CYC(0x72f1, 0x72f3); A = 0x57;
      CALL_C(0x72f3, interactionSetHighTextIndex_hook, 0x253b, 0x72f6);
      CALL_C(0x72f6, interactionInitGraphics_hook, 0x15fb, 0x72f9);
      monkey_state0_before_init_subid_hook(gb, sp0_);
      if (!(gb->pc == 0x72ff && gb->sp == sp0_)) return;
      monkey_state0_after_init_subid_hook(gb);
      return;
    case 0x74ba:
      monkeyState1_hook(gb);
      return;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }
}
