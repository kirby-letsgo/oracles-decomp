#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(monkeyJumpSpeed100);
  CYC(b_+0, b_+3); SET_BC(0xff00);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ);
}

void monkeySetAnimationAndJump_hook(GB *gb) {
  BASE(monkeySetAnimationAndJump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionSetAnimation_hook, SYM(interactionSetAnimation), SYM(monkeyJumpSpeed100));
  TAIL(monkeyJumpSpeed100);
}

void monkeySubid0State1Substate3_hook(GB *gb) {
  BASE(monkeySubid0State1Substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+3);
  if (!(F & FC)) {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+7); A = 0x01;
    CYC(b_+7, b_+10); mem_wr(gb, wLoadedTreeGfxIndex, A);
    CYC(b_+10, b_+13);
    TAIL(interactionDelete);
  }
  CYCT(b_+3, b_+5);
  CYC(b_+13, b_+15); C = 0x20;
  CALL_C(b_+15, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+18);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+21);
    TAIL(objectApplySpeed);
  }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); A = 0x04;
  CYC(b_+23, b_+25);
  TAIL(monkeySetAnimationAndJump);
}

void monkeyJumpSpeed120_hook(GB *gb) {
  BASE(monkeyJumpSpeed120);
  CYC(b_+0, b_+3); SET_BC(0xfee0);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ);
}

void monkeyUpdateGravityAndHop_hook(GB *gb) {
  BASE(monkeyUpdateGravityAndHop);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  TAIL(monkeyJumpSpeed120);
}

void monkeySetJumpSpeed_hook(GB *gb) {
  BASE(monkeySetJumpSpeed);
  CYC(b_+0, b_+2); L = 0x78;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); E = 0x54;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void monkeyUpdateGravityAndJumpIfLanded_hook(GB *gb) {
  BASE(monkeyUpdateGravityAndJumpIfLanded);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  TAIL(monkeySetJumpSpeed);
}

void monkeyBeginDisappearing_hook(GB *gb) {
  BASE(monkeyBeginDisappearing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x3c);
  CYC(b_+2, b_+4); L = 0x5c;
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x06);
  CYC(b_+6, b_+8); L = 0x4f;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);
  CYC(b_+10, b_+12); A = 0x50;
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+18);
  TAIL(interactionIncSubstate);
}

void monkeyWaitBeforeFlickering_hook(GB *gb) {
  BASE(monkeyWaitBeforeFlickering);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x3c);
  CYC(b_+6, b_+9);
  TAIL(interactionIncSubstate);
}

void monkeyFlickerUntilDeletion_hook(GB *gb) {
  BASE(monkeyFlickerUntilDeletion);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    CYC(b_+8, b_+10); B = 0x01;
    CYC(b_+10, b_+13);
    TAIL(objectFlickerVisibility);
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8);
  TAIL(interactionDelete);
}

static void monkey0_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey0Disappearance);
  CALL_C(b_+12, interactionAnimate_hook, SYM(interactionAnimate), b_+15);
  CALL_C(b_+15, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+18);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+22);
  interactionIncSubstate_hook(gb);
}

static void monkey0_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey0Disappearance);
  CALL_C(b_+22, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+25);
  if (F & FZ) {
    CYC(b_+25, b_+27);
    CYC(b_+27, b_+29);
    TAIL(monkeyBeginDisappearing);
  }
  CYCT(b_+25, b_+27);
  CALL_C(b_+29, monkeyUpdateGravityAndJumpIfLanded_hook, SYM(monkeyUpdateGravityAndJumpIfLanded), b_+32);
  CYC(b_+32, b_+35);
  interactionAnimate_hook(gb);
}

void monkey0Disappearance_hook(GB *gb) {
  BASE(monkey0Disappearance);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+12) { monkey0_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+22) { monkey0_disappearance_substate1_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeyWaitBeforeFlickering) && hook_enabled_at(gb, SYM(monkeyWaitBeforeFlickering))) { monkeyWaitBeforeFlickering_hook(gb); return; }
    else if (jt_ == SYM(monkeyFlickerUntilDeletion) && hook_enabled_at(gb, SYM(monkeyFlickerUntilDeletion))) { monkeyFlickerUntilDeletion_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey3_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey3Disappearance);
  CALL_C(b_+10, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+13);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+16);
    TAIL(interactionAnimate);
  }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18);
  monkeyBeginDisappearing_hook(gb);
}

void monkey3Disappearance_hook(GB *gb) {
  BASE(monkey3Disappearance);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+10) { monkey3_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeyWaitBeforeFlickering) && hook_enabled_at(gb, SYM(monkeyWaitBeforeFlickering))) { monkeyWaitBeforeFlickering_hook(gb); return; }
    else if (jt_ == SYM(monkeyFlickerUntilDeletion) && hook_enabled_at(gb, SYM(monkeyFlickerUntilDeletion))) { monkeyFlickerUntilDeletion_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey5_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey5Disappearance);
  CALL_C(b_+21, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+24);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+25); ret_effect(gb);
    return;
  }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xb4);
  CALL_C(b_+27, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+30);
  CYC(b_+30, b_+33); SET_BC(0xf3f8);
  CYC(b_+33, b_+35); A = 0x5a;
  CYC(b_+35, b_+38);
  objectCreateExclamationMark_hook(gb);
}

static void monkey5_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey5Disappearance);
  CALL_C(b_+14, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+17);
  CYC(b_+17, b_+19); L = 0x5c;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x02);
  monkey5_disappearance_substate1_hook(gb, sp0_);
}

static void monkey5_disappearance_substate2_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey5Disappearance);
  CALL_C(b_+38, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+41);
  if (!(F & FZ)) {
    CYCT(b_+41, b_+42); ret_effect(gb);
    return;
  }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+45);
  monkeyBeginDisappearing_hook(gb);
}

void monkey5Disappearance_hook(GB *gb) {
  BASE(monkey5Disappearance);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+14) { monkey5_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+21) { monkey5_disappearance_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+38) { monkey5_disappearance_substate2_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeyWaitBeforeFlickering) && hook_enabled_at(gb, SYM(monkeyWaitBeforeFlickering))) { monkeyWaitBeforeFlickering_hook(gb); return; }
    else if (jt_ == SYM(monkeyFlickerUntilDeletion) && hook_enabled_at(gb, SYM(monkeyFlickerUntilDeletion))) { monkeyFlickerUntilDeletion_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void monkeyCheckChangeAnimation_hook(GB *gb) {
  BASE(monkeyCheckChangeAnimation);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x4f;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x03);
  CYC(b_+6, b_+8); alu_cp(gb, 0xfa);
  CYC(b_+8, b_+10); A = 0x00;
  if (!(F & FC)) {
    CYCT(b_+10, b_+12);
  } else {
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  }
  CYC(b_+13, b_+15); L = 0x7a;
  CYC(b_+15, b_+16); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+16, b_+17); ret_effect(gb);
    return;
  }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = 0x60;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x01);
  CYC(b_+22, b_+25);
  TAIL(interactionAnimate);
}

static void monkey9_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey9Disappearance);
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+41); L = 0x48;
  CYC(b_+41, b_+43); A = 0x08;
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+47); L = 0x50;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x28);
  CALL_C(b_+49, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+52);
  CYC(b_+52, b_+55);
  monkeyJumpSpeed100_hook(gb);
}

static void monkey9_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey9Disappearance);
  CYC(b_+55, b_+57); C = 0x20;
  CALL_C(b_+57, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+60);
  if (!(F & FZ)) {
    CYCT(b_+60, b_+63);
    TAIL(objectApplySpeed);
  }
  CYC(b_+60, b_+63);
  CALL_C(b_+63, monkeyJumpSpeed100_hook, SYM(monkeyJumpSpeed100), b_+66);
  CYC(b_+66, b_+68); L = 0x7c;
  CYC(b_+68, b_+69); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+69, b_+70); A = mem_rd(gb, HL);
  CYC(b_+70, b_+72); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+73); ret_effect(gb);
    return;
  }
  CYC(b_+72, b_+73);
  CALL_C(b_+73, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+76);
  CYC(b_+76, b_+78); L = 0x78;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x10);
  CYC(b_+80, b_+81); ret_effect(gb);
}

static void monkey9_disappearance_substate2_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey9Disappearance);
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = 0x78;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+85, b_+86); ret_effect(gb);
    return;
  }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x10);
  CALL_C(b_+88, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+91);
  CYC(b_+91, b_+93); L = 0x48;
  CYC(b_+93, b_+94); A = mem_rd(gb, HL);
  CYC(b_+94, b_+96); alu_xor(gb, 0x10);
  CYC(b_+96, b_+97); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A);
  CYC(b_+98, b_+100); L = 0x49;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+103); alu_and(gb, 0x10);
  CYC(b_+103, b_+105); A = 0x03;
  if (!(F & FZ)) {
    CYCT(b_+105, b_+107);
  } else {
    CYC(b_+105, b_+107);
    CYC(b_+107, b_+109); A = 0x08;
  }
  CYC(b_+109, b_+112);
  monkeySetAnimationAndJump_hook(gb);
}

static void monkey9_disappearance_substate3_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey9Disappearance);
  CYC(b_+112, b_+113); H = D;
  CYC(b_+113, b_+115); L = 0x78;
  CYC(b_+115, b_+116); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+116, b_+117); ret_effect(gb);
    return;
  }
  CYC(b_+116, b_+117);
  CYC(b_+117, b_+119); L = 0x7c;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x00);
  CYC(b_+121, b_+123); L = 0x45;
  CYC(b_+123, b_+124); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+124, b_+125); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+125, b_+126); ret_effect(gb);
}

void monkey9Disappearance_hook(GB *gb) {
  BASE(monkey9Disappearance);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, monkeyCheckChangeAnimation_hook, SYM(monkeyCheckChangeAnimation), b_+3);
  CYC(b_+3, b_+5); E = 0x45;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CALL_C(b_+10, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+13);
    if (!(F & FZ)) {
      CYCT(b_+13, b_+15);
    } else {
      CYC(b_+13, b_+15);
      CALL_C(b_+15, monkeyBeginDisappearing_hook, SYM(monkeyBeginDisappearing), b_+18);
      CYC(b_+18, b_+20); L = 0x45;
      CYC(b_+20, b_+22); mem_wr(gb, HL, 0x04);
    }
  }
  CYC(b_+22, b_+24); E = 0x45;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+38) { monkey9_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+55) { monkey9_disappearance_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+81) { monkey9_disappearance_substate2_hook(gb, sp0_); return; }
    else if (jt_ == b_+112) { monkey9_disappearance_substate3_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeyWaitBeforeFlickering) && hook_enabled_at(gb, SYM(monkeyWaitBeforeFlickering))) { monkeyWaitBeforeFlickering_hook(gb); return; }
    else if (jt_ == SYM(monkeyFlickerUntilDeletion) && hook_enabled_at(gb, SYM(monkeyFlickerUntilDeletion))) { monkeyFlickerUntilDeletion_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey8_disappearance_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey8Disappearance);
  CALL_C(b_+16, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+19);
  if (F & FZ) {
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+23); mem_wr(gb, HL, 0x5a);
    CALL_C(b_+23, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+26);
    CYC(b_+26, b_+29); SET_BC(0xf3f8);
    CYC(b_+29, b_+31); A = 0x3c;
    CYC(b_+31, b_+34);
    TAIL(objectCreateExclamationMark);
  }
  CYCT(b_+19, b_+21);
  CYC(b_+34, b_+37); A = W8(wFrameCounter);
  CYC(b_+37, b_+39); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+39, b_+40); ret_effect(gb);
    return;
  }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+43);
  interactionAnimate_hook(gb);
}

static void monkey8_disappearance_substate1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey8Disappearance);
  CALL_C(b_+43, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+46);
  if (!(F & FZ)) {
    CYCT(b_+46, b_+47); ret_effect(gb);
    return;
  }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0xb4);
  CYC(b_+49, b_+52);
  interactionIncSubstate_hook(gb);
}

static void monkey8_disappearance_substate2_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey8Disappearance);
  CALL_C(b_+52, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+55);
  if (F & FZ) {
    CYC(b_+55, b_+57);
    CYC(b_+57, b_+60);
    TAIL(monkeyBeginDisappearing);
  }
  CYCT(b_+55, b_+57);
  CYC(b_+60, b_+63); A = W8(wFrameCounter);
  CYC(b_+63, b_+65); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+65, b_+66); ret_effect(gb);
    return;
  }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); L = 0x48;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+71); alu_xor(gb, 0x01);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  CYC(b_+72, b_+75);
  interactionSetAnimation_hook(gb);
}

static void monkey8_disappearance_substate3_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey8Disappearance);
  CALL_C(b_+75, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+78);
  if (F & FZ) {
    CYC(b_+78, b_+80);
    CYC(b_+80, b_+82); mem_wr(gb, HL, 0x1e);
    CALL_C(b_+82, objectSetInvisible_hook, SYM(objectSetInvisible), b_+85);
    CYC(b_+85, b_+88);
    TAIL(interactionIncSubstate);
  }
  CYCT(b_+78, b_+80);
  CYC(b_+88, b_+90); B = 0x01;
  CYC(b_+90, b_+93);
  objectFlickerVisibility_hook(gb);
}

static void monkey8_disappearance_substate4_hook(GB *gb, uint16_t sp0_) {
  BASE(monkey8Disappearance);
  CALL_C(b_+93, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+96);
  if (!(F & FZ)) {
    CYCT(b_+96, b_+97); ret_effect(gb);
    return;
  }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+99); A = 0xff;
  CYC(b_+99, b_+102); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+102, b_+105);
  interactionDelete_hook(gb);
}

void monkey8Disappearance_hook(GB *gb) {
  BASE(monkey8Disappearance);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+16) { monkey8_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+43) { monkey8_disappearance_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+52) { monkey8_disappearance_substate2_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeyWaitBeforeFlickering) && hook_enabled_at(gb, SYM(monkeyWaitBeforeFlickering))) { monkeyWaitBeforeFlickering_hook(gb); return; }
    else if (jt_ == b_+75) { monkey8_disappearance_substate3_hook(gb, sp0_); return; }
    else if (jt_ == b_+93) { monkey8_disappearance_substate4_hook(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void monkeySubid2State1_hook(GB *gb) {
  BASE(monkeySubid2State1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6);
  TAIL(interactionAnimateAsNpc);
}

void monkeySubid1State1_hook(GB *gb) {
  BASE(monkeySubid1State1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == SYM(monkey0Disappearance) && hook_enabled_at(gb, SYM(monkey0Disappearance))) { monkey0Disappearance_hook(gb); return; }
    else if (jt_ == SYM(monkey3Disappearance) && hook_enabled_at(gb, SYM(monkey3Disappearance))) { monkey3Disappearance_hook(gb); return; }
    else if (jt_ == SYM(monkey5Disappearance) && hook_enabled_at(gb, SYM(monkey5Disappearance))) { monkey5Disappearance_hook(gb); return; }
    else if (jt_ == SYM(monkey8Disappearance) && hook_enabled_at(gb, SYM(monkey8Disappearance))) { monkey8Disappearance_hook(gb); return; }
    else if (jt_ == SYM(monkey9Disappearance) && hook_enabled_at(gb, SYM(monkey9Disappearance))) { monkey9Disappearance_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_subid0_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid0State1);
  CYC(b_+25, b_+28); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+28, b_+30); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+33);
    TAIL(interactionRunScript);
  }
  CYC(b_+30, b_+33);
  CALL_C(b_+33, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+36);
  CYC(b_+36, b_+38); A = 0x06;
  CYC(b_+38, b_+41);
  interactionSetAnimation_hook(gb);
}

static void monkey_subid0_substate1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid0State1);
  CYC(b_+41, b_+44); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+44, b_+46); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+46, b_+47); ret_effect(gb);
    return;
  }
  CYC(b_+46, b_+47);
  CALL_C(b_+47, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+50);
  CYC(b_+50, b_+52); L = 0x46;
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x32);
  CYC(b_+54, b_+56); A = 0x03;
  CALL_C(b_+56, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+59);
  CYC(b_+59, b_+61);
  monkeyJumpSpeed120_hook(gb);
}

static void monkey_subid0_substate2_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid0State1);
  CALL_C(b_+61, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+64);
  if (!(F & FZ)) {
    CYCT(b_+64, b_+66);
    TAIL(monkeyUpdateGravityAndHop);
  }
  CYC(b_+64, b_+66);
  CALL_C(b_+66, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+69);
  CYC(b_+69, b_+71); L = 0x49;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x02);
  CYC(b_+73, b_+75); L = 0x4f;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x00);
  CYC(b_+77, b_+79); L = 0x50;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x3c);
  monkeySetAnimationAndJump_hook(gb);
}

void monkeySubid0State1_hook(GB *gb) {
  BASE(monkeySubid0State1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CALL_C(b_+3, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+6);
  CYC(b_+6, b_+8); E = 0x45;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+10, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+13);
  else CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = 0x45;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+25) { monkey_subid0_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+41) { monkey_subid0_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+61) { monkey_subid0_substate2_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeySubid0State1Substate3) && hook_enabled_at(gb, SYM(monkeySubid0State1Substate3))) { monkeySubid0State1Substate3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_subid4_substate0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CALL_C(b_+38, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+41);
  if (!(F & FZ)) {
    CYCT(b_+41, b_+42); ret_effect(gb);
    return;
  }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+45);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_substate1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CALL_C(b_+45, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+48);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+49); ret_effect(gb);
    return;
  }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x3c);
  CYC(b_+51, b_+53); L = 0x43;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+56); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYC(b_+56, b_+58);
    CYC(b_+58, b_+60); A = 0x00;
    CALL_C(b_+60, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+63);
    CYC(b_+63, b_+65); L = 0x5c;
    CYC(b_+65, b_+67); mem_wr(gb, HL, 0x06);
  } else {
    CYCT(b_+56, b_+58);
  }
  CYC(b_+67, b_+69); A = 0x90;
  CALL_C(b_+69, playSound_b00_hook, SYM(playSound_b00), b_+72);
  CYC(b_+72, b_+75);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_substate2_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CALL_C(b_+75, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+78);
  if (!(F & FZ)) {
    CYCT(b_+78, b_+80);
    CYC(b_+88, b_+90); B = 0x01;
    CYC(b_+90, b_+93);
    TAIL(objectFlickerVisibility);
  }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+82, objectSetVisible_hook, SYM(objectSetVisible), b_+85);
  CYC(b_+85, b_+88);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_finish_substate3_hook(GB *gb) {
  BASE(monkeySubid4State1);
  CYC(b_+127, b_+129); L = 0x5c;
  CYC(b_+129, b_+130); mem_wr(gb, HL, B);
  CYC(b_+130, b_+133);
  interactionIncSubstate_hook(gb);
}

static void monkey_subid4_substate3_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CALL_C(b_+93, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+96);
  if (!(F & FZ)) {
    CYCT(b_+96, b_+97); ret_effect(gb);
    return;
  }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+99); B = 0x03;
  CYC(b_+99, b_+101); L = 0x43;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+104); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYC(b_+104, b_+106);
    CYC(b_+106, b_+107); B = alu_dec8(gb, B);
    CYC(b_+107, b_+109);
    monkey_subid4_finish_substate3_hook(gb);
    return;
  }
  CYCT(b_+104, b_+106);
  CYC(b_+109, b_+111); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYC(b_+111, b_+113);
    CYC(b_+113, b_+115); A = 0x00;
    CALL_C(b_+115, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+118);
    CYC(b_+118, b_+120); L = 0x5c;
    CYC(b_+120, b_+122); mem_wr(gb, HL, 0x02);
    CYC(b_+122, b_+123); H = D;
    CYC(b_+123, b_+125); L = 0x46;
    CYC(b_+125, b_+127); mem_wr(gb, HL, 0xb4);
  } else {
    CYCT(b_+111, b_+113);
  }
  monkey_subid4_finish_substate3_hook(gb);
}

static void monkey_subid4_substate4_1_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CALL_C(b_+136, interactionAnimate_hook, SYM(interactionAnimate), b_+139);
  CYC(b_+139, b_+141); E = 0x43;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+144); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+144, b_+145); ret_effect(gb);
    return;
  }
  CYC(b_+144, b_+145);
  CALL_C(b_+145, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+148);
  if (!(F & FZ)) {
    CYCT(b_+148, b_+149); ret_effect(gb);
    return;
  }
  CYC(b_+148, b_+149);
  CYC(b_+149, b_+151); A = 0xff;
  CYC(b_+151, b_+154); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+154, b_+155); ret_effect(gb);
}

static void monkey_subid4_substate4_0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CALL_C(b_+133, monkeyUpdateGravityAndJumpIfLanded_hook, SYM(monkeyUpdateGravityAndJumpIfLanded), b_+136);
  monkey_subid4_substate4_1_hook(gb, sp0_);
}

static void monkey_subid4_monkey0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CYC(b_+24, b_+26); E = 0x45;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+38) { monkey_subid4_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+45) { monkey_subid4_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+75) { monkey_subid4_substate2_hook(gb, sp0_); return; }
    else if (jt_ == b_+93) { monkey_subid4_substate3_hook(gb, sp0_); return; }
    else if (jt_ == b_+133) { monkey_subid4_substate4_0_hook(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_subid4_monkey3_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CYC(b_+155, b_+157); E = 0x45;
  CYC(b_+157, b_+158); A = mem_rd(gb, DE);
  CYC(b_+158, b_+159); push_effect(gb, b_+159);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+38) { monkey_subid4_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+45) { monkey_subid4_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+75) { monkey_subid4_substate2_hook(gb, sp0_); return; }
    else if (jt_ == b_+93) { monkey_subid4_substate3_hook(gb, sp0_); return; }
    else if (jt_ == b_+136) { monkey_subid4_substate4_1_hook(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_subid4_monkey9_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid4State1);
  CYC(b_+169, b_+171); E = 0x45;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+174); alu_cp(gb, 0x04);
  if (!(F & FC)) CALL_C_CC(b_+174, monkeyCheckChangeAnimation_hook, SYM(monkeyCheckChangeAnimation), b_+177);
  else CYC(b_+174, b_+177);
  CYC(b_+177, b_+179); E = 0x45;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+181); push_effect(gb, b_+181);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+38) { monkey_subid4_substate0_hook(gb, sp0_); return; }
    else if (jt_ == b_+45) { monkey_subid4_substate1_hook(gb, sp0_); return; }
    else if (jt_ == b_+75) { monkey_subid4_substate2_hook(gb, sp0_); return; }
    else if (jt_ == b_+93) { monkey_subid4_substate3_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate0)) { monkey9_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate1)) { monkey9_disappearance_substate1_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate2)) { monkey9_disappearance_substate2_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate3)) { monkey9_disappearance_substate3_hook(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void monkeySubid4State1_hook(GB *gb) {
  BASE(monkeySubid4State1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+24) { monkey_subid4_monkey0_hook(gb, sp0_); return; }
    else if (jt_ == b_+155) { monkey_subid4_monkey3_hook(gb, sp0_); return; }
    else if (jt_ == b_+169) { monkey_subid4_monkey9_hook(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void monkeyAnimateAndRunScript_hook(GB *gb) {
  BASE(monkeyAnimateAndRunScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6);
  TAIL(interactionAnimateAsNpc);
}

void monkeySubid5State1_monkey9_hook(GB *gb) {
  BASE(monkeySubid5State1_monkey9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CALL_C(b_+3, monkeyCheckChangeAnimation_hook, SYM(monkeyCheckChangeAnimation), b_+6);
  CALL_C(b_+6, objectPushLinkAwayOnCollision_hook, SYM(objectPushLinkAwayOnCollision), b_+9);
  CALL_C(b_+9, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+12);
  CYC(b_+12, b_+14); E = 0x45;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == SYM(monkey9Disappearance__substate0)) { monkey9_disappearance_substate0_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate1)) { monkey9_disappearance_substate1_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate2)) { monkey9_disappearance_substate2_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkey9Disappearance__substate3)) { monkey9_disappearance_substate3_hook(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_subid5_monkey0_hook(GB *gb, uint16_t sp0_) {
  BASE(monkeySubid5State1);
  CALL_C(b_+24, monkeyUpdateGravityAndJumpIfLanded_hook, SYM(monkeyUpdateGravityAndJumpIfLanded), SYM(monkeyAnimateAndRunScript));
  monkeyAnimateAndRunScript_hook(gb);
}

void monkeySubid5State1_hook(GB *gb) {
  BASE(monkeySubid5State1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+24) { monkey_subid5_monkey0_hook(gb, sp0_); return; }
    else if (jt_ == SYM(monkeyAnimateAndRunScript) && hook_enabled_at(gb, SYM(monkeyAnimateAndRunScript))) { monkeyAnimateAndRunScript_hook(gb); return; }
    else if (jt_ == SYM(monkeySubid5State1_monkey9) && hook_enabled_at(gb, SYM(monkeySubid5State1_monkey9))) { monkeySubid5State1_monkey9_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void monkeyState1_hook(GB *gb) {
  BASE(monkeyState1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == SYM(monkeySubid0State1) && hook_enabled_at(gb, SYM(monkeySubid0State1))) { monkeySubid0State1_hook(gb); return; }
    else if (jt_ == SYM(monkeySubid1State1) && hook_enabled_at(gb, SYM(monkeySubid1State1))) { monkeySubid1State1_hook(gb); return; }
    else if (jt_ == SYM(monkeySubid2State1) && hook_enabled_at(gb, SYM(monkeySubid2State1))) { monkeySubid2State1_hook(gb); return; }
    else if (jt_ == SYM(monkeySubid4State1) && hook_enabled_at(gb, SYM(monkeySubid4State1))) { monkeySubid4State1_hook(gb); return; }
    else if (jt_ == SYM(monkeySubid5State1) && hook_enabled_at(gb, SYM(monkeySubid5State1))) { monkeySubid5State1_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(monkeyAnimateAndRunScript) && hook_enabled_at(gb, SYM(monkeyAnimateAndRunScript))) { monkeyAnimateAndRunScript_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_init_subid23_shared(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+87, b_+89); A = 0x0a;
  CALL_C(b_+89, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+92);
  if (!(F & FZ)) {
    CYCT(b_+92, b_+95);
    TAIL(interactionDelete);
  }
  CYC(b_+92, b_+95);
  CYC(b_+95, b_+97); E = 0x42;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+100); alu_sub(gb, 0x02);
  CYC(b_+100, b_+103); SET_HL(SYM(introMonkeyScriptTable));
  CYC(b_+103, b_+104); monkey_add_double_index_from_rst(gb, b_+104);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+105, b_+106); H = mem_rd(gb, HL);
  CYC(b_+106, b_+107); L = A;
  CYC(b_+107, b_+110);
  interactionSetScript_hook(gb);
}

static void monkey_init_subid0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+59, b_+61); A = 0x02;
  CALL_C(b_+61, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+64);
  CYC(b_+64, b_+67); SET_HL((SYM(objectGfxHeaderTable) + 84));
  CYC(b_+67, b_+70);
  interactionSetScript_hook(gb);
}

static void monkey_init_subid2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+70, b_+72); A = 0x02;
  CYC(b_+72, b_+74); E = 0x5c;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+77); A = 0x06;
  CALL_C(b_+77, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+80);
  CYC(b_+80, b_+82);
  monkey_init_subid23_shared(gb, sp0_);
}

static void monkey_init_subid3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+82, b_+84); A = 0x07;
  CALL_C(b_+84, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+87);
  monkey_init_subid23_shared(gb, sp0_);
}

static void monkey_init_subid1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+110, b_+112); E = 0x43;
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+114); C = A;
  CYC(b_+114, b_+115); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+115, b_+117);
  } else {
    CYC(b_+115, b_+117);
    CYC(b_+117, b_+118); E = alu_dec8(gb, E);
    CYC(b_+118, b_+119); A = mem_rd(gb, DE);
    CYC(b_+119, b_+121); alu_cp(gb, 0x05);
    if (F & FZ) {
      CYCT(b_+121, b_+123);
    } else {
      CYC(b_+121, b_+123);
      CYC(b_+123, b_+124); push_effect(gb, BC);
      CYC(b_+124, b_+126); A = 0xad;
      CALL_C(b_+126, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+129);
      CYC(b_+129, b_+130); SET_BC(pop_effect(gb));
    }
    CYC(b_+130, b_+132); B = 0x09;
    for (;;) {
      CALL_C(b_+132, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+135);
      if (!(F & FZ)) {
        CYCT(b_+135, b_+137);
        break;
      }
      CYC(b_+135, b_+137);
      CYC(b_+137, b_+139); mem_wr(gb, HL, 0x39);
      CYC(b_+139, b_+140); L = alu_inc8(gb, L);
      CYC(b_+140, b_+142); E = 0x42;
      CYC(b_+142, b_+143); A = mem_rd(gb, DE);
      CYC(b_+143, b_+144); mem_wr(gb, HL, A);
      CYC(b_+144, b_+145); L = alu_inc8(gb, L);
      CYC(b_+145, b_+146); mem_wr(gb, HL, B);
      CYC(b_+146, b_+147); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(b_+147, b_+149);
      } else {
        CYC(b_+147, b_+149);
        break;
      }
    }
  }
  CYC(b_+149, b_+150); A = C;
  CYC(b_+150, b_+151); alu_add(gb, A);
  CYC(b_+151, b_+154); SET_HL(b_+215);
  CYC(b_+154, b_+155); monkey_add_double_index_from_rst(gb, b_+155);
  CYC(b_+155, b_+156); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+156, b_+158); E = 0x4b;
  CYC(b_+158, b_+159); mem_wr(gb, DE, A);
  CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+160, b_+162); E = 0x4d;
  CYC(b_+162, b_+163); mem_wr(gb, DE, A);
  CYC(b_+163, b_+164); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+164, b_+166); E = 0x46;
  CYC(b_+166, b_+167); mem_wr(gb, DE, A);
  CYC(b_+167, b_+168); A = mem_rd(gb, HL);
  CALL_C(b_+168, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+171);
  CALL_C(b_+171, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+174);
  CYC(b_+174, b_+176); alu_and(gb, 0x0f);
  CYC(b_+176, b_+177); H = D;
  CYC(b_+177, b_+179); L = 0x47;
  CYC(b_+179, b_+180); mem_wr(gb, HL, A);
  CYC(b_+180, b_+182); alu_sub(gb, 0x07);
  CYC(b_+182, b_+184); L = 0x60;
  CYC(b_+184, b_+185); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+185, b_+186); mem_wr(gb, HL, A);
  CALL_C(b_+186, getRandomNumber_hook, SYM(getRandomNumber), b_+189);
  CYC(b_+189, b_+191); alu_and(gb, 0x03);
  CYC(b_+191, b_+194); SET_BC(b_+207);
  CALL_C(b_+194, addDoubleIndexToBc_hook, 0x007e, b_+197);
  CYC(b_+197, b_+199); L = 0x78;
  CYC(b_+199, b_+200); A = mem_rd(gb, BC);
  CYC(b_+200, b_+201); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+201, b_+202); SET_BC(BC + 1);
  CYC(b_+202, b_+203); A = mem_rd(gb, BC);
  CYC(b_+203, b_+204); mem_wr(gb, HL, A);
  CYC(b_+204, b_+207);
  monkeySetJumpSpeed_hook(gb);
}

static void monkey_init_bowtie(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+293, b_+295); A = 0x07;
  CALL_C(b_+295, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+298);
  CALL_C(b_+298, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+301);
  if (!(F & FZ)) {
    CYCT(b_+301, b_+302); ret_effect(gb);
    return;
  }
  CYC(b_+301, b_+302);
  CYC(b_+302, b_+304); mem_wr(gb, HL, 0x63);
  CYC(b_+304, b_+305); L = alu_inc8(gb, L);
  CYC(b_+305, b_+307); mem_wr(gb, HL, 0x3d);
  CYC(b_+307, b_+308); L = alu_inc8(gb, L);
  CYC(b_+308, b_+310); mem_wr(gb, HL, 0x01);
  CYC(b_+310, b_+312); L = 0x56;
  CYC(b_+312, b_+314); mem_wr(gb, HL, 0x40);
  CYC(b_+314, b_+315); L = alu_inc8(gb, L);
  CYC(b_+315, b_+316); mem_wr(gb, HL, D);
  CYC(b_+316, b_+318); E = 0x59;
  CYC(b_+318, b_+319); A = H;
  CYC(b_+319, b_+320); mem_wr(gb, DE, A);
  CYC(b_+320, b_+321); ret_effect(gb);
}

static void monkey_init_subid4(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CALL_C(b_+255, objectSetInvisible_hook, SYM(objectSetInvisible), b_+258);
  CYC(b_+258, b_+261); push_effect(gb, b_+261);
  monkey_init_subid1(gb, sp0_);
  CYC(b_+261, b_+263); L = 0x5c;
  CYC(b_+263, b_+265); mem_wr(gb, HL, 0x06);
  CYC(b_+265, b_+267); L = 0x47;
  CYC(b_+267, b_+269); mem_wr(gb, HL, 0x3c);
  CYC(b_+269, b_+271); L = 0x43;
  CYC(b_+271, b_+272); A = mem_rd(gb, HL);
  CYC(b_+272, b_+274); alu_cp(gb, 0x09);
  if (F & FZ) {
    CYC(b_+274, b_+276);
    CYC(b_+276, b_+278); L = 0x7c;
    CYC(b_+278, b_+279); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+279, b_+282); SET_BC((SYM(itemData) + 127));
    CYC(b_+282, b_+285);
    TAIL(interactionSetPosition);
  }
  CYCT(b_+274, b_+276);
  CYC(b_+285, b_+287); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+287, b_+288); ret_effect(gb);
    return;
  }
  CYC(b_+287, b_+288);
  CYC(b_+288, b_+290); A = 0xfa;
  CYC(b_+290, b_+292); E = 0x46;
  CYC(b_+292, b_+293); mem_wr(gb, DE, A);
  monkey_init_bowtie(gb, sp0_);
}

static void monkey_init_subid5(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+321, b_+323); A = 0x11;
  CALL_C(b_+323, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+326);
  if (F & FZ) {
    CYCT(b_+326, b_+329);
    TAIL(interactionDelete);
  }
  CYC(b_+326, b_+329);
  CYC(b_+329, b_+332); push_effect(gb, b_+332);
  monkey_init_subid1(gb, sp0_);
  CYC(b_+332, b_+334); L = 0x46;
  CYC(b_+334, b_+335); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+335, b_+336); mem_wr(gb, HL, A);
  CYC(b_+336, b_+339); SET_HL((SYM(objectGfxHeaderTable) + 106));
  CYC(b_+339, b_+341); E = 0x43;
  CYC(b_+341, b_+342); A = mem_rd(gb, DE);
  CYC(b_+342, b_+344); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+344, b_+346);
  } else {
    CYC(b_+344, b_+346);
    CYC(b_+346, b_+347); push_effect(gb, AF);
    CYC(b_+347, b_+350); push_effect(gb, b_+350);
    monkey_init_bowtie(gb, sp0_);
    CYC(b_+350, b_+353); SET_HL((SYM(objectGfxHeaderTable) + 124));
    CYC(b_+353, b_+354); SET_AF(pop_effect(gb));
  }
  CYC(b_+354, b_+356); alu_cp(gb, 0x05);
  CYC(b_+356, b_+358); A = 0x03;
  if (!(F & FZ)) {
    CYCT(b_+358, b_+360);
  } else {
    CYC(b_+358, b_+360);
    CYC(b_+360, b_+362); A = 0x02;
  }
  CYC(b_+362, b_+364); E = 0x5c;
  CYC(b_+364, b_+365); mem_wr(gb, DE, A);
  CYC(b_+365, b_+368);
  interactionSetScript_hook(gb);
}

static void monkey_init_set_animation(GB *gb) {
  BASE(interactionCode39_body);
  CYC(b_+462, b_+464); E = 0x7a;
  CYC(b_+464, b_+465); mem_wr(gb, DE, A);
  CYC(b_+465, b_+468);
  interactionSetAnimation_hook(gb);
}

static void monkey_init_subid7(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+373, b_+375); E = 0x43;
  CYC(b_+375, b_+376); A = mem_rd(gb, DE);
  CYC(b_+376, b_+377); push_effect(gb, b_+377);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+383) {
      CYC(b_+383, b_+385); A = 0x14;
      CALL_C(b_+385, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+388);
      if (!(F & FZ)) {
        CYCT(b_+388, b_+391); TAIL(interactionDelete);
      }
      CYC(b_+388, b_+391);
      CYC(b_+391, b_+393); A = 0x11;
      CALL_C(b_+393, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+396);
      if (F & FZ) {
        CYCT(b_+396, b_+399); TAIL(interactionDelete);
      }
      CYC(b_+396, b_+399);
      CYC(b_+399, b_+402); SET_HL((SYM(objectGfxHeaderTable) + 132));
      CALL_C(b_+402, interactionSetScript_hook, SYM(interactionSetScript), b_+405);
      CYC(b_+405, b_+407); A = 0x06;
      CYC(b_+407, b_+409);
      monkey_init_set_animation(gb);
      return;
    }
    else if (jt_ == b_+409) {
      CYC(b_+409, b_+411); A = 0x14;
      CALL_C(b_+411, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+414);
      if (F & FZ) {
        CYCT(b_+414, b_+417); TAIL(interactionDelete);
      }
      CYC(b_+414, b_+417);
      CYC(b_+417, b_+420); SET_HL((SYM(objectGfxHeaderTable) + 144));
      CALL_C(b_+420, interactionSetScript_hook, SYM(interactionSetScript), b_+423);
      CYC(b_+423, b_+425); A = 0x05;
      CYC(b_+425, b_+427);
      monkey_init_set_animation(gb);
      return;
    }
    else if (jt_ == b_+427) {
      CYC(b_+427, b_+429); A = 0x14;
      CALL_C(b_+429, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+432);
      if (!(F & FZ)) {
        CYCT(b_+432, b_+435); TAIL(interactionDelete);
      }
      CYC(b_+432, b_+435);
      CYC(b_+435, b_+437); A = 0x12;
      CALL_C(b_+437, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+440);
      if (F & FZ) {
        CYCT(b_+440, b_+443); TAIL(interactionDelete);
      }
      CYC(b_+440, b_+443);
      CYC(b_+443, b_+445); A = 0x11;
      CALL_C(b_+445, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+448);
      CYC(b_+448, b_+451); SET_HL((SYM(objectGfxHeaderTable) + 156));
      if (F & FZ) {
        CYCT(b_+451, b_+454);
      } else {
        CYC(b_+451, b_+454);
        CYC(b_+454, b_+457); SET_HL((SYM(objectGfxHeaderTable) + 168));
      }
      CALL_C(b_+457, interactionSetScript_hook, SYM(interactionSetScript), b_+460);
      CYC(b_+460, b_+462); A = 0x05;
      monkey_init_set_animation(gb);
      return;
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}

static void monkey_init_subid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CYC(b_+39, b_+41); E = 0x42;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); push_effect(gb, b_+43);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+59) { monkey_init_subid0(gb, sp0_); return; }
    else if (jt_ == b_+110) { monkey_init_subid1(gb, sp0_); return; }
    else if (jt_ == b_+70) { monkey_init_subid2(gb, sp0_); return; }
    else if (jt_ == b_+82) { monkey_init_subid3(gb, sp0_); return; }
    else if (jt_ == b_+255) { monkey_init_subid4(gb, sp0_); return; }
    else if (jt_ == b_+321) { monkey_init_subid5(gb, sp0_); return; }
    else if (jt_ == b_+368) {
      CYC(b_+368, b_+370); A = 0x05;
      CYC(b_+370, b_+373); interactionSetAnimation_hook(gb);
      return;
    }
    else if (jt_ == b_+373) { monkey_init_subid7(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void monkey_state0_before_init_subid_hook(GB *gb, uint16_t sp0_) {
  BASE(interactionCode39_body);
  CALL_C(b_+19, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+22);
  CYC(b_+22, b_+25); push_effect(gb, b_+25);
  monkey_init_subid(gb, sp0_);
}

static void monkey_state0_after_init_subid_hook(GB *gb) {
  BASE(interactionCode39_body);
  CYC(b_+25, b_+27); E = 0x43;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_cp(gb, 0x09);
  if (F & FZ) {
    CYCT(b_+30, b_+31); ret_effect(gb);
    return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); E = 0x40;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+35, b_+38); TAIL(objectMarkSolidPosition);
  }
  CYC(b_+35, b_+38);
  CYC(b_+38, b_+39); ret_effect(gb);
}

void interactionCode39_body__afterCall72f9_hook(GB *gb) {
  BASE(interactionCode39_body);
  uint16_t sp0_ = gb->sp;
  monkey_state0_before_init_subid_hook(gb, sp0_);
  if (!(gb->pc == b_+25 && gb->sp == sp0_)) return;
  monkey_state0_after_init_subid_hook(gb);
}

void interactionCode39_body_hook(GB *gb) {
  BASE(interactionCode39_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (monkey_jump_table(gb));
    if (jt_ == b_+8) {
      CYC(b_+8, b_+10); A = 0x01;
      CYC(b_+10, b_+11); mem_wr(gb, DE, A);
      CYC(b_+11, b_+13); A = 0x57;
      CALL_C(b_+13, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+16);
      CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
      monkey_state0_before_init_subid_hook(gb, sp0_);
      if (!(gb->pc == b_+25 && gb->sp == sp0_)) return;
      monkey_state0_after_init_subid_hook(gb);
      return;
    }
    else if (jt_ == SYM(monkeyState1)) {
      TAIL(monkeyState1);
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}
