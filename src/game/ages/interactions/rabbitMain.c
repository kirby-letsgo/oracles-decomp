#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(rabbitSubid0), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(rabbitSubid0), (from), (to), true)

static uint16_t rabbit_jump_table(GB *gb) {
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

static void rabbit_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void rabbit_subid0_jump(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid0);
  CYC(b_+64, b_+67); SET_BC(0xfe00);
  CALL_C(b_+67, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+70);
  CYC(b_+70, b_+72); A = 0x04;
  CYC(b_+72, b_+75);
  interactionSetAnimation_hook(gb);
}

static void rabbit_subid0_substate0(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid0);
  CYC(b_+15, b_+18); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+23);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(b_+20, b_+23);
  CALL_C(b_+23, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+26);
  CYC(b_+26, b_+28); A = 0x02;
  CYC(b_+28, b_+31);
  interactionSetAnimation_hook(gb);
}

static void rabbit_subid0_substate1(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid0);
  CYC(b_+31, b_+34); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+34, b_+36); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+36, b_+39);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(b_+36, b_+39);
  CALL_C(b_+39, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+42);
  CYC(b_+42, b_+44); L = 0x46;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x28);
  CYC(b_+46, b_+47); ret_effect(gb);
}

static void rabbit_subid0_substate2(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid0);
  CALL_C(b_+47, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+50);
  if (!(F & FZ)) {
    CYCT(b_+50, b_+53);
    interactionAnimate_hook(gb);
    return;
  }
  CYC(b_+50, b_+53);
  CALL_C(b_+53, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+56);
  CYC(b_+56, b_+58); L = 0x49;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x06);
  CYC(b_+60, b_+62); L = 0x50;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x3c);
  rabbit_subid0_jump(gb, sp0_);
}

static void rabbit_subid0_substate3(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid0);
  CALL_C(b_+75, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+78);
  if (!(F & FC)) {
    CYCT(b_+78, b_+81);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+78, b_+81);
  CYC(b_+81, b_+83); C = 0x20;
  CALL_C(b_+83, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+86);
  if (!(F & FZ)) {
    CYCT(b_+86, b_+89);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(b_+86, b_+89);
  CYC(b_+89, SYM(rabbitSubid1));
  rabbit_subid0_jump(gb, sp0_);
}

void rabbitSubid0_hook(GB *gb) {
  BASE(rabbitSubid0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+3);
  CYC(b_+3, b_+5); E = 0x45;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+15) { rabbit_subid0_substate0(gb, sp0_); return; }
    else if (jt_ == b_+31) { rabbit_subid0_substate1(gb, sp0_); return; }
    else if (jt_ == b_+47) { rabbit_subid0_substate2(gb, sp0_); return; }
    else if (jt_ == b_+75) { rabbit_subid0_substate3(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void rabbit_set_jump_animation(GB *gb, uint16_t sp0_) {
  BASE(interactionCode4b_body);
  CYC(b_+63, b_+65); A = 0x05;
  CALL_C(b_+65, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+68);
  CYC(b_+68, b_+71); SET_BC(0xfe80);
  CYC(b_+71, b_+74);
  objectSetSpeedZ_hook(gb);
}

static void rabbit_subid1_substate0(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CALL_C(b_+42, interactionAnimate_hook, SYM(interactionAnimate), b_+45);
  CYC(b_+45, b_+47); E = 0x61;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+49, b_+50); ret_effect(gb);
    return;
  }
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+52); A = 0x53;
  CALL_C(b_+52, playSound_b00_hook, SYM(playSound_b00), b_+55);
  CYC(b_+55, b_+58);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate1(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CYC(b_+58, b_+60); E = 0x4d;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+63); alu_cp(gb, 0xd0);
  if (!(F & FC)) {
    CYCT(b_+63, b_+66);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+63, b_+66);
  CALL_C(b_+66, objectApplySpeed_hook, SYM(objectApplySpeed), b_+69);
  CYC(b_+69, b_+71); C = 0x20;
  CALL_C(b_+71, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+74);
  if (!(F & FZ)) {
    CYCT(b_+74, b_+75); ret_effect(gb);
    return;
  }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+78); L = 0x45;
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+79, b_+82);
  rabbit_set_jump_animation(gb, sp0_);
}

static void rabbit_subid1_substate2(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CALL_C(b_+82, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+85);
  if (!(F & FZ)) {
    CYCT(b_+85, b_+86); ret_effect(gb);
    return;
  }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x3c);
  CYC(b_+88, b_+90); L = 0x4d;
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+93); L = 0x7d;
  CYC(b_+93, b_+94); mem_wr(gb, HL, A);
  CYC(b_+94, b_+97);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate3(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CYC(b_+97, b_+100); SET_HL((SYM(enemyData) + 60));
  CYC(b_+100, b_+102); E = 0x08;
  CALL_C(b_+102, interBankCall_hook, 0x008a, b_+105);
  CALL_C(b_+105, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+108);
  if (!(F & FZ)) {
    CYCT(b_+108, b_+109); ret_effect(gb);
    return;
  }
  CYC(b_+108, b_+109);
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x14);
  CYC(b_+111, b_+113); L = 0x5c;
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x06);
  CYC(b_+115, b_+118);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate4(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CALL_C(b_+118, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+121);
  if (!(F & FZ)) {
    CYCT(b_+121, b_+122); ret_effect(gb);
    return;
  }
  CYC(b_+121, b_+122);
  CYC(b_+122, b_+125); SET_BC(0x0000);
  CALL_C(b_+125, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+128);
  CYC(b_+128, b_+131);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate5(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CYC(b_+131, b_+133); C = 0x20;
  CALL_C(b_+133, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+136);
  if (!(F & FZ)) {
    CYCT(b_+136, b_+137); ret_effect(gb);
    return;
  }
  CYC(b_+136, b_+137);
  CALL_C(b_+137, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+140);
  CYC(b_+140, b_+142); L = 0x47;
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0xf0);
  CYC(b_+144, b_+146); A = 0x04;
  CYC(b_+146, b_+149);
  setScreenShakeCounter_hook(gb);
}

static void rabbit_subid1_substate6(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid1);
  CALL_C(b_+149, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+152);
  if (!(F & FZ)) {
    CYCT(b_+152, b_+153); ret_effect(gb);
    return;
  }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+155); A = 0xff;
  CYC(b_+155, b_+158); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+158, SYM(rabbitSubid2)); ret_effect(gb);
}

void rabbitSubid1_hook(GB *gb) {
  BASE(rabbitSubid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x46;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) {
      CYCT(b_+8, b_+10);
    } else {
      CYC(b_+8, b_+10);
      CYC(b_+10, b_+11); L = alu_inc8(gb, L);
      CYC(b_+11, b_+13); A = 0x1e;
      CYC(b_+13, b_+14); mem_wr(gb, HL, A);
      CYC(b_+14, b_+16); L = 0x45;
      CYC(b_+16, b_+18); mem_wr(gb, HL, 0x02);
      CYC(b_+18, b_+21); SET_BC(0xf000);
      CALL_C(b_+21, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+24);
    }
  }
  CYC(b_+24, b_+26); E = 0x45;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+42) { rabbit_subid1_substate0(gb, sp0_); return; }
    else if (jt_ == b_+58) { rabbit_subid1_substate1(gb, sp0_); return; }
    else if (jt_ == b_+82) { rabbit_subid1_substate2(gb, sp0_); return; }
    else if (jt_ == b_+97) { rabbit_subid1_substate3(gb, sp0_); return; }
    else if (jt_ == b_+118) { rabbit_subid1_substate4(gb, sp0_); return; }
    else if (jt_ == b_+131) { rabbit_subid1_substate5(gb, sp0_); return; }
    else if (jt_ == b_+149) { rabbit_subid1_substate6(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void rabbit_init_subid1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode4b_body);
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = 0x49;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x18);
  CYC(b_+59, b_+61); L = 0x50;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x3c);
  rabbit_set_jump_animation(gb, sp0_);
}

static void rabbit_subid3_substate0(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid3);
  CALL_C(b_+12, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+15);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x5a);
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+23); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(b_+23, b_+25); A = 0xc1;
  CALL_C(b_+25, playSound_b00_hook, SYM(playSound_b00), b_+28);
  CYC(b_+28, b_+31);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid3_substate1(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid3);
  CALL_C(b_+31, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+34);
  if (!(F & FZ)) {
    CYC(b_+34, b_+36);
    CYC(b_+36, b_+39); SET_HL((SYM(tuniNut_state0) + 2));
    CYC(b_+39, b_+41); E = 0x08;
    CYC(b_+41, b_+44);
    interBankCall_hook(gb);
    return;
  }
  CYCT(b_+34, b_+36);
  CALL_C(b_+44, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+47);
  CYC(b_+47, b_+49); L = 0x5c;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x02);
  CYC(b_+51, b_+53); L = 0x78;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x20);
  CYC(b_+55, SYM(rabbitSubid4));
  rabbit_init_subid1(gb, sp0_);
}

void rabbitSubid3_hook(GB *gb) {
  BASE(rabbitSubid3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+12) { rabbit_subid3_substate0(gb, sp0_); return; }
    else if (jt_ == b_+31) { rabbit_subid3_substate1(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid1__substate0)) { rabbit_subid1_substate0(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid1__substate1)) { rabbit_subid1_substate1(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void rabbitJump_hook(GB *gb) {
  BASE(rabbitJump);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x07;
  CALL_C(b_+2, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+5);
  CYC(b_+5, b_+8); SET_BC(0xff20);
  CYC(b_+8, SYM(rabbitSubid4Substate2));
  objectSetSpeedZ_hook(gb);
}

void rabbitSubid4Substate2_hook(GB *gb) {
  BASE(rabbitSubid4Substate2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CALL_C(b_+8, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+11);
  CYC(b_+11, b_+13); L = 0x49;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x18);
  CYC(b_+15, b_+17); L = 0x50;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x19);
  CYC(b_+19, b_+22); SET_BC(0xfe80);
  CALL_C(b_+22, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+25);
  CYC(b_+25, b_+27); A = 0x09;
  CYC(b_+27, SYM(rabbitSubid5));
  interactionSetAnimation_hook(gb);
}

static void rabbit_subid5_substate3(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid5);
  CALL_C(b_+84, objectApplySpeed_hook, SYM(objectApplySpeed), b_+87);
  CYC(b_+87, b_+89); C = 0x20;
  CALL_C(b_+89, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+92);
  if (!(F & FC)) {
    CYCT(b_+92, b_+93); ret_effect(gb);
    return;
  }
  CYC(b_+92, b_+93);
  CALL_C(b_+93, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+96);
  CYC(b_+96, b_+98); L = 0x46;
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x3c);
  CYC(b_+100, b_+101); ret_effect(gb);
}

static void rabbit_subid4_substate0(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid4);
  CYC(b_+14, b_+17); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(b_+17, b_+19); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+22); H = D;
    CYC(b_+22, b_+24); L = 0x45;
    CYC(b_+24, b_+26); mem_wr(gb, HL, 0x02);
    CYC(b_+26, b_+29); SET_HL((SYM(partData) + 655));
    CYC(b_+29, b_+32);
    interactionSetScript_hook(gb);
    return;
  }
  CYCT(b_+19, b_+21);
  CALL_C(b_+32, interactionAnimate_hook, SYM(interactionAnimate), b_+35);
  CYC(b_+35, b_+37); E = 0x61;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+39, b_+40); ret_effect(gb);
    return;
  }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+43);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid4_substate1(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid4);
  CYC(b_+43, b_+45); C = 0x20;
  CALL_C(b_+45, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+48);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+49); ret_effect(gb);
    return;
  }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = 0x45;
  CYC(b_+52, SYM(rabbitJump)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  rabbitJump_hook(gb);
}

static void rabbit_subid5_ret(GB *gb) {
  BASE(rabbitSubid5);
  CYC(b_+100, b_+101); ret_effect(gb);
}

void rabbitSubid4_hook(GB *gb) {
  BASE(rabbitSubid4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+14) { rabbit_subid4_substate0(gb, sp0_); return; }
    else if (jt_ == b_+43) { rabbit_subid4_substate1(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid4Substate2)) { rabbitSubid4Substate2_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid5__substate3)) { rabbit_subid5_substate3(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid5__ret)) { rabbit_subid5_ret(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void rabbitSubid7_hook(GB *gb) {
  BASE(rabbitSubid7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, SYM(rabbitSubid2SetRandomSpawnDelay));
  npcFaceLinkAndAnimate_hook(gb);
}

void rabbitSubid2SetRandomSpawnDelay_hook(GB *gb) {
  BASE(rabbitSubid2SetRandomSpawnDelay);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_BC(SYM(rabbitSubid2SpawnDelays));
  CALL_C(b_+8, addAToBc_hook, 0x006d, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+14); E = 0x78;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, SYM(spawnNextRabbitThatTurnsToStone)); ret_effect(gb);
}

void spawnNextRabbitThatTurnsToStone_hook(GB *gb) {
  BASE(spawnNextRabbitThatTurnsToStone);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); L = alu_inc8(gb, L);
  CYC(b_+1, b_+2); A = mem_rd(gb, HL);
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); alu_add(gb, B);
  CYC(b_+6, b_+9); SET_HL(b_+25);
  CYC(b_+9, b_+10); rabbit_add_a_to_hl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+13); E = 0x46;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  CYC(b_+15, b_+16); SET_HL(HL + 1);
  CYC(b_+16, b_+17); C = mem_rd(gb, HL);
  CALL_ROM(b_+17, SYM(spawnRabbitWithSubid1));
  CYC(b_+20, b_+22); L = 0x46;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x5f);
  CYC(b_+24, b_+25); ret_effect(gb);
}

void rabbitSubid2_hook(GB *gb) {
  BASE(rabbitSubid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x46;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) {
      CALL_C_CC(b_+8, spawnNextRabbitThatTurnsToStone_hook, SYM(spawnNextRabbitThatTurnsToStone), b_+11);
    } else {
      CYC(b_+8, b_+11);
    }
  }
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x78;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+19);
  CYC(b_+19, b_+21); alu_and(gb, 0x07);
  CYC(b_+21, b_+24); SET_HL(SYM(rabbitSubid2YPositions));
  CYC(b_+24, b_+25); rabbit_add_a_to_hl_from_rst(gb, b_+25);
  CYC(b_+25, b_+26); B = mem_rd(gb, HL);
  CALL_C(b_+26, getRandomNumber_hook, SYM(getRandomNumber), b_+29);
  CYC(b_+29, b_+31); alu_and(gb, 0x0f);
  CYC(b_+31, b_+32); alu_cpl(gb);
  CYC(b_+32, b_+33); A = alu_inc8(gb, A);
  CYC(b_+33, b_+35); alu_add(gb, 0xb0);
  CYC(b_+35, b_+36); C = A;
  CALL_ROM(b_+36, SYM(spawnRabbitWithSubid1));
  CYC(b_+39, SYM(rabbitSubid3));
  rabbitSubid2SetRandomSpawnDelay_hook(gb);
}

static void rabbit_subid5_substate0(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid5);
  CYC(b_+67, b_+68); H = D;
  CYC(b_+68, b_+70); L = 0x46;
  CALL_C(b_+70, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+73);
  if (!(F & FZ)) {
    CYCT(b_+73, b_+74); ret_effect(gb);
    return;
  }
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+76, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+79);
  CYC(b_+79, b_+81); A = 0xc1;
  CYC(b_+81, b_+84);
  playSound_b00_hook(gb);
}

static void rabbit_subid5_substate4(GB *gb, uint16_t sp0_) {
  BASE(rabbitSubid5);
  CALL_C(b_+101, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+104);
  if (!(F & FZ)) {
    CYCT(b_+104, b_+105); ret_effect(gb);
    return;
  }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+107); A = 0xff;
  CYC(b_+107, b_+110); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+110, SYM(rabbitSubid7)); ret_effect(gb);
}

void rabbitSubid5_hook(GB *gb) {
  BASE(rabbitSubid5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x78;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) {
      CYCT(b_+8, b_+10);
    } else {
      CYC(b_+8, b_+10);
      CYC(b_+10, b_+12); L = 0x45;
      CYC(b_+12, b_+14); mem_wr(gb, HL, 0x04);
      CYC(b_+14, b_+16); L = 0x49;
      CYC(b_+16, b_+18); mem_wr(gb, HL, 0x08);
      CYC(b_+18, b_+20); L = 0x50;
      CYC(b_+20, b_+22); mem_wr(gb, HL, 0x19);
      CYC(b_+22, b_+25); SET_BC(0xfe20);
      CALL_C(b_+25, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+28);
      CYC(b_+28, b_+31); SET_BC((SYM(_adjustHLSequential) + 10));
      CALL_C(b_+31, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+34);
      if (!(F & FZ)) {
        CYCT(b_+34, b_+36);
      } else {
        CYC(b_+34, b_+36);
        CYC(b_+36, b_+38); A = 0x4e;
        CALL_C(b_+38, playSound_b00_hook, SYM(playSound_b00), b_+41);
        CYC(b_+41, b_+43); A = 0x02;
        CYC(b_+43, b_+46); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
      }
      CYC(b_+46, b_+48); A = 0x08;
      CALL_C(b_+48, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+51);
    }
  }
  CYC(b_+51, b_+53); E = 0x45;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); push_effect(gb, b_+55);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+67) { rabbit_subid5_substate0(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid3__substate1)) { rabbit_subid3_substate1(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid1__substate0)) { rabbit_subid1_substate0(gb, sp0_); return; }
    else if (jt_ == SYM(rabbitSubid1__substate1)) { rabbit_subid1_substate1(gb, sp0_); return; }
    else if (jt_ == b_+84) { rabbit_subid5_substate3(gb, sp0_); return; }
    else if (jt_ == b_+101) { rabbit_subid5_substate4(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void rabbit_dispatch_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode4b_body);
  CYC(b_+184, b_+186); E = 0x42;
  CYC(b_+186, b_+187); A = mem_rd(gb, DE);
  CYC(b_+187, b_+188); push_effect(gb, b_+188);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == SYM(rabbitSubid0)) { rabbitSubid0_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid1)) { rabbitSubid1_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid2)) { rabbitSubid2_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid3)) { rabbitSubid3_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid4)) { rabbitSubid4_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid5)) { rabbitSubid5_hook(gb); return; }
    else if (jt_ == SYM(interactionPushLinkAwayAndUpdateDrawPriority)) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; }
    else if (jt_ == SYM(rabbitSubid7)) { rabbitSubid7_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

static void rabbit_init_subid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode4b_body);
  CYC(b_+28, b_+30); E = 0x42;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+48) {
      CYC(b_+48, b_+51); SET_HL((SYM(partData) + 638));
      CYC(b_+51, b_+54); interactionSetScript_hook(gb);
      return;
    }
    else if (jt_ == b_+54) {
      rabbit_init_subid1(gb, sp0_);
      return;
    }
    else if (jt_ == b_+74) {
      CYC(b_+74, b_+76); E = 0x46;
      CYC(b_+76, b_+78); A = 0xb4;
      CYC(b_+78, b_+79); mem_wr(gb, DE, A);
      CYC(b_+79, b_+82); SET_HL((SYM(func_7cf8) + 237));
      CYC(b_+82, b_+84); E = 0x08;
      CALL_C(b_+84, interBankCall_hook, 0x008a, b_+87);
      CYC(b_+87, b_+90); rabbitSubid2SetRandomSpawnDelay_hook(gb);
      return;
    }
    else if (jt_ == b_+90) {
      CYC(b_+90, b_+93); SET_HL(wGroup4RoomFlags + 0xfc);
      CYC(b_+93, b_+95); alu_bit(gb, 7, mem_rd(gb, HL));
      if (!(F & FZ)) {
        CYCT(b_+95, b_+98); interactionDelete_hook(gb);
        return;
      }
      CYC(b_+95, b_+98);
      CYC(b_+98, b_+101); A = mem_rd(gb, wEssencesObtained);
      CYC(b_+101, b_+103); alu_bit(gb, 6, A);
      if (F & FZ) {
        CYCT(b_+103, b_+106); interactionDelete_hook(gb);
        return;
      }
      CYC(b_+103, b_+106);
      CYC(b_+106, b_+109); SET_HL((SYM(func_7cf8) + 237));
      CYC(b_+109, b_+111); E = 0x08;
      CALL_C(b_+111, interBankCall_hook, 0x008a, b_+114);
      CYC(b_+114, b_+116); A = 0x06;
      CALL_C(b_+116, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+119);
      CYC(b_+119, b_+121); A = 0x78;
      CYC(b_+121, b_+123); E = 0x46;
      CYC(b_+123, b_+124); mem_wr(gb, DE, A);
      CYC(b_+124, b_+126); A = 0x06;
      CYC(b_+126, b_+128); E = 0x5c;
      CYC(b_+128, b_+129); mem_wr(gb, DE, A);
      CYC(b_+129, b_+132); interactionSetAnimation_hook(gb);
      return;
    }
    else if (jt_ == b_+119) {
      CYC(b_+119, b_+121); A = 0x78;
      CYC(b_+121, b_+123); E = 0x46;
      CYC(b_+123, b_+124); mem_wr(gb, DE, A);
      CYC(b_+124, b_+126); A = 0x06;
      CYC(b_+126, b_+128); E = 0x5c;
      CYC(b_+128, b_+129); mem_wr(gb, DE, A);
      CYC(b_+129, b_+132); interactionSetAnimation_hook(gb);
      return;
    }
    else if (jt_ == b_+132) {
      CALL_C(b_+132, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+135);
      CYC(b_+135, b_+136); H = D;
      CYC(b_+136, b_+138); L = 0x46;
      CYC(b_+138, b_+140); mem_wr(gb, HL, 0x0e);
      CYC(b_+140, b_+141); L = alu_inc8(gb, L);
      CYC(b_+141, b_+143); mem_wr(gb, HL, 0x01);
      CYC(b_+143, b_+145);
      CYC(b_+124, b_+126); A = 0x06;
      CYC(b_+126, b_+128); E = 0x5c;
      CYC(b_+128, b_+129); mem_wr(gb, DE, A);
      CYC(b_+129, b_+132); interactionSetAnimation_hook(gb);
      return;
    }
    else if (jt_ == b_+145) {
      CALL_C(b_+145, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+148);
      CYC(b_+148, b_+151); rabbitJump_hook(gb);
      return;
    }
    else if (jt_ == b_+151) {
      CYC(b_+151, b_+153); A = 0x14;
      CALL_C(b_+153, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+156);
      if (!(F & FZ)) {
        CYCT(b_+156, b_+159); interactionDelete_hook(gb);
        return;
      }
      CYC(b_+156, b_+159);
      CYC(b_+159, b_+161); A = 0x12;
      CALL_C(b_+161, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+164);
      if (F & FZ) {
        CYCT(b_+164, b_+167); interactionDelete_hook(gb);
        return;
      }
      CYC(b_+164, b_+167);
      CYC(b_+167, b_+169); A = 0x11;
      CALL_C(b_+169, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+172);
      CYC(b_+172, b_+175); SET_HL((SYM(partData) + 662));
      if (F & FZ) {
        CYCT(b_+175, b_+178);
      } else {
        CYC(b_+175, b_+178);
        CYC(b_+178, b_+181); SET_HL((SYM(partData) + 665));
      }
      CALL_C(b_+181, interactionSetScript_hook, SYM(interactionSetScript), b_+184);
      rabbit_dispatch_state1(gb, sp0_);
      return;
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}

static void rabbit_state0_before_init_subid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode4b_body);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20);
  rabbit_init_subid(gb, sp0_);
}

static void rabbit_state0_after_init_subid(GB *gb) {
  BASE(interactionCode4b_body);
  CYC(b_+20, b_+22); E = 0x40;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); objectMarkSolidPosition_hook(gb);
    return;
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void interactionCode4b_body__afterCall7825_hook(GB *gb) {
  BASE(interactionCode4b_body);
  uint16_t sp0_ = gb->sp;
  rabbit_state0_before_init_subid(gb, sp0_);
  if (!(gb->pc == b_+20 && gb->sp == sp0_)) return;
  rabbit_state0_after_init_subid(gb);
}

void interactionCode4b_body_hook(GB *gb) {
  BASE(interactionCode4b_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (rabbit_jump_table(gb));
    if (jt_ == b_+8) {
{
      CYC(b_+8, b_+10); A = 0x01;
      CYC(b_+10, b_+11); mem_wr(gb, DE, A);
      CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
      rabbit_state0_before_init_subid(gb, sp0_);
      if (!(gb->pc == b_+20 && gb->sp == sp0_)) return;
      rabbit_state0_after_init_subid(gb);
      return;
    }
    }
    else if (jt_ == b_+184) {
      rabbit_dispatch_state1(gb, sp0_);
      return;
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}
