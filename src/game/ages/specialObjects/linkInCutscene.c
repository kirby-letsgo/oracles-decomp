#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define linkCutscene0OscillationData_bank06 SYM(linkCutscene_zOscillation0)

void linkCutscene_initOam_setVisible_incState_hook(GB *gb);
void linkCutscene_updateAngleOnPath_hook(GB *gb);
void linkCutsceneFunc_73e8_hook(GB *gb);
void linkCutscene0_hook(GB *gb);

static uint16_t link_cutscene_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void link_cutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void link_cutscene_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x0012, 0x0013, false);
    burn_rom(gb, 0, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

void linkCutsceneRet_hook(GB *gb) {
  BASE(linkCutsceneRet);
  CYC(b_+0, b_+1); ret_effect(gb);
}

void linkCutscene_animateAndDecCounter1_hook(GB *gb) {
  BASE(linkCutscene_animateAndDecCounter1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+6); TAIL(itemDecCounter1);
}

void linkCutscene_oscillateZ_hook(GB *gb) {
  BASE(linkCutscene_oscillateZ);
  CYC(b_+0, b_+3); A = W8(wTmpcbb7);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+10); A = W8(wTmpcbb7);
    CYC(b_+10, b_+12); alu_and(gb, 0x38);
    CYC(b_+12, b_+14); A = alu_swap(gb, A);
    CYC(b_+14, b_+15); alu_rlca(gb);
    CYC(b_+15, b_+16); push_effect(gb, b_+16); link_cutscene_add_a_to_hl(gb);
    CYC(b_+16, b_+18); E = 0x0f;
    CYC(b_+18, b_+19); A = mem_rd(gb, HL);
    CYC(b_+19, b_+20); B = A;
    CYC(b_+20, b_+21); A = mem_rd(gb, DE);
    CYC(b_+21, b_+22); alu_add(gb, B);
    CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  }
  CYC(b_+23, b_+26); TAIL(specialObjectAnimate);
}

void linkCutscene_oscillateZ_1_hook(GB *gb) {
  BASE(linkCutscene_oscillateZ_1);
  CYC(b_+0, b_+3); SET_HL(SYM(linkCutscene_zOscillation1));
  TAIL(linkCutscene_oscillateZ);
}

void linkCutscene_oscillateZ_2_hook(GB *gb) {
  BASE(linkCutscene_oscillateZ_2);
  CYC(b_+0, b_+3); SET_HL(SYM(linkCutscene_zOscillation2));
  CYC(b_+3, b_+6); TAIL(linkCutscene_oscillateZ);
}

void linkCutscene_createGlowingOrb_hook(GB *gb) {
  BASE(linkCutscene_createGlowingOrb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x8406);
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+10); L = 0x56;
    CYC(b_+10, b_+12); A = 0;
    CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+13, b_+14); mem_wr(gb, HL, D);
  }
  CALL_C(b_+14, itemIncSubstate_hook, SYM(itemIncSubstate), b_+17);
  CYC(b_+17, b_+19); A = 0x05;
  CYC(b_+19, b_+22); TAIL(specialObjectSetAnimation);
}

void linkCutscene0_substate6_hook(GB *gb) {
  BASE(linkCutscene0_substate6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x21;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+8); A = 0x07;
    CYC(b_+8, b_+11); W8(wTmpcbb9) = A;
    CYC(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CALL_C(b_+12, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+15);
  CYC(b_+15, b_+18); A = W8(wTmpcbb7);
  CYC(b_+18, b_+19); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+19, b_+22);
    TAIL(objectSetInvisible);
  }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+25);
  TAIL(objectSetVisible);
}

void linkCutscene0_hook(GB *gb) {
  BASE(linkCutscene0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+18) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CALL_C(b_+11, objectSetVisible81_hook, SYM(objectSetVisible81), b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CALL_C(b_+15, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+18);

state1:
  CYC(b_+18, b_+20); E = 0x05;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); push_effect(gb, b_+22);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+36) { goto substate0; }
    else if (jt_ == b_+80) { goto substate1; }
    else if (jt_ == b_+103) { goto substate2; }
    else if (jt_ == b_+125) { goto substate3; }
    else if (jt_ == b_+148) { goto substate4; }
    else if (jt_ == b_+162) { goto substate5; }
    else if (jt_ == SYM(linkCutscene0_substate6) && hook_enabled_at(gb, SYM(linkCutscene0_substate6))) { linkCutscene0_substate6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+36, b_+39); A = W8(wLinkAngle);
  CYC(b_+39, b_+40); alu_rlca(gb);
  CYC(b_+40, b_+42); A = 0x00;
  if (F & FC) {
    CYCT(b_+42, b_+45);
    TAIL(specialObjectSetAnimation);
  }
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+46); H = D;
  CYC(b_+46, b_+48); L = 0x0b;
  CYC(b_+48, b_+51); A = W8(wGameKeysPressed);
  CYC(b_+51, b_+53); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYC(b_+53, b_+55);
    CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  } else {
    CYCT(b_+53, b_+55);
  }
  CYC(b_+56, b_+58); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYC(b_+58, b_+60);
    CYC(b_+60, b_+61); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  } else {
    CYCT(b_+58, b_+60);
  }
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_cp(gb, 0x40);
  if (!(F & FC)) {
    CYCT(b_+64, b_+67);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+69); A = 0x01;
  CYC(b_+69, b_+72); W8(wTmpcbb9) = A;
  CYC(b_+72, b_+74); A = 0x77;
  CALL_C(b_+74, playSound_b00_hook, SYM(playSound_b00), b_+77);
  CYC(b_+77, b_+80);
  TAIL(itemIncSubstate);

substate1:
  CYC(b_+80, b_+83); A = W8(wTmpcbb9);
  CYC(b_+83, b_+85); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+85, b_+86); ret_effect(gb); return; }
  CYC(b_+85, b_+86);
  CALL_C(b_+86, itemIncSubstate_hook, SYM(itemIncSubstate), b_+89);
  CYC(b_+89, b_+91); B = 0x04;
  CALL_C(b_+91, func_2d48_hook, SYM(func_2d48), b_+94);
  CYC(b_+94, b_+95); A = B;
  CYC(b_+95, b_+97); E = 0x06;
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+100); A = 0x04;
  CYC(b_+100, b_+103);
  TAIL(specialObjectSetAnimation);

substate2:
  CALL_C(b_+103, itemDecCounter1_hook, SYM(itemDecCounter1), b_+106);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+109);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+106, b_+109);
  CYC(b_+109, b_+111); L = 0x10;
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x05);
  CYC(b_+113, b_+115); B = 0x05;
  CALL_C(b_+115, func_2d48_hook, SYM(func_2d48), b_+118);
  CYC(b_+118, b_+119); A = B;
  CYC(b_+119, b_+121); E = 0x06;
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
  CYC(b_+122, b_+125);
  TAIL(itemIncSubstate);

substate3:
  CALL_C(b_+125, itemDecCounter1_hook, SYM(itemDecCounter1), b_+128);
  if (!(F & FZ)) {
    CYCT(b_+128, b_+131);
    goto oscillate;
  }
  CYC(b_+128, b_+131);
  CALL_C(b_+131, itemIncSubstate_hook, SYM(itemIncSubstate), b_+134);
  CYC(b_+134, b_+136); B = 0x07;
  CALL_C(b_+136, func_2d48_hook, SYM(func_2d48), b_+139);
  CYC(b_+139, b_+140); A = B;
  CYC(b_+140, b_+142); E = 0x06;
  CYC(b_+142, b_+143); mem_wr(gb, DE, A);
oscillate:
  CYC(b_+143, b_+146); SET_HL(linkCutscene0OscillationData_bank06);
  CYC(b_+146, b_+148);
  TAIL(linkCutscene_oscillateZ);

substate4:
  CALL_C(b_+148, itemDecCounter1_hook, SYM(itemDecCounter1), b_+151);
  if (!(F & FZ)) {
    CYCT(b_+151, b_+154);
    TAIL(linkCutscene_oscillateZ_1);
  }
  CYC(b_+151, b_+154);
  CYC(b_+154, b_+156); A = 0x03;
  CYC(b_+156, b_+159); W8(wTmpcbb9) = A;
  CALL_C(b_+159, itemIncSubstate_hook, SYM(itemIncSubstate), b_+162);

substate5:
  CYC(b_+162, b_+165); A = W8(wTmpcbb9);
  CYC(b_+165, b_+167); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+167, b_+169);
    TAIL(linkCutscene_oscillateZ_1);
  }
  CYC(b_+167, b_+169);
  TAIL(linkCutscene_createGlowingOrb);
}

void linkCutscene_cpyTo48_hook(GB *gb) {
  BASE(linkCutscene_cpyTo48);
  CYC(b_+0, b_+2); E = 0x0b;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x48);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void linkCutscene_cpxTo38_hook(GB *gb) {
  BASE(linkCutscene_cpxTo38);
  CYC(b_+0, b_+2); E = 0x0d;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x38);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void linkCutscene_initOam_setVisible_incState_hook(GB *gb) {
  BASE(linkCutscene_initOam_setVisible_incState);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(GV((SYM(nextToKeyDoor) + 49), 0x41b5));
  CYC(b_+3, b_+5); E = 0x05;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_C(b_+8, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+11);
  CYC(b_+11, b_+14);
  TAIL(itemIncState);
}

void linkCutscene_updateAngleOnPath_hook(GB *gb) {
  BASE(linkCutscene_updateAngleOnPath);
  uint16_t sp0_ = gb->sp;
  bool loop_after_update = false;

start:
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+79);
  CYC(b_+6, b_+7); link_cutscene_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); E = 0x3d;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_add(gb, A);
  CYC(b_+14, b_+15); push_effect(gb, b_+15); link_cutscene_add_a_to_hl(gb);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+18); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    CYC(b_+36, b_+38); E = 0x0d;
    CYC(b_+38, b_+39); A = mem_rd(gb, DE);
    CYC(b_+39, b_+40); alu_sub(gb, mem_rd(gb, HL));
    CYC(b_+40, b_+42); B = 0x03;
    if (!(F & FC)) {
      CYCT(b_+42, b_+44);
    } else {
      CYC(b_+42, b_+44);
      CYC(b_+44, b_+46); B = 0x01;
    }
    if (!(F & FZ)) {
      CYCT(b_+46, b_+48);
      goto update_direction;
    }
    CYC(b_+46, b_+48);
  } else {
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+24); E = 0x0b;
    CYC(b_+24, b_+25); A = mem_rd(gb, DE);
    CYC(b_+25, b_+26); alu_sub(gb, mem_rd(gb, HL));
    CYC(b_+26, b_+28); B = 0x00;
    if (!(F & FC)) {
      CYCT(b_+28, b_+30);
    } else {
      CYC(b_+28, b_+30);
      CYC(b_+30, b_+32); B = 0x02;
    }
    if (!(F & FZ)) {
      CYCT(b_+32, b_+34);
      goto update_direction;
    }
    CYC(b_+32, b_+34);
    CYC(b_+34, b_+36);
  }

  CYC(b_+48, b_+49); H = D;
  CYC(b_+49, b_+51); L = 0x3d;
  CYC(b_+51, b_+52); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+52, b_+55); push_effect(gb, b_+55);
  loop_after_update = true;

update_direction:
  CYC(b_+57, b_+59); E = 0x08;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+61, b_+63);
  } else {
    CYC(b_+61, b_+63);
    CYC(b_+63, b_+64); A = B;
    CYC(b_+64, b_+65); mem_wr(gb, DE, A);
    CALL_C(b_+65, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+68);
    CYC(b_+68, b_+70); E = 0x08;
    CYC(b_+70, b_+71); A = mem_rd(gb, DE);
    CYC(b_+71, b_+73); A = alu_swap(gb, A);
    CYC(b_+73, b_+74); alu_rrca(gb);
    CYC(b_+74, b_+76); E = 0x09;
    CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  }
  CYC(b_+77, b_+78); alu_or(gb, D);
  CYC(b_+78, b_+79); ret_effect(gb);
  if (!loop_after_update) return;
  loop_after_update = false;
  CYC(b_+55, b_+57);
  goto start;
}

void linkCutsceneFunc_73e8_hook(GB *gb) {
  BASE(linkCutsceneFunc_73e8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL((SYM(breakableTileCollisionTable__overworld) + 83));
  CYC(b_+9, b_+11); E = 0x0a;
  CALL_C(b_+11, interBankCall_hook, 0x008a, b_+14);
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CALL_C(b_+17, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = 0x08;
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+24, b_+25); ret_effect(gb); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+29);
  TAIL(specialObjectSetAnimation);
}

static void link_cutscene_set_direction_and_animation(GB *gb, uint16_t sp0_) {
  BASE(linkCutscene2);
  CALL_C(b_+123, itemIncSubstate_hook, SYM(itemIncSubstate), b_+126);
  CYC(b_+126, b_+128); L = 0x09;
  CYC(b_+128, b_+129); mem_wr(gb, HL, B);
  CYC(b_+129, b_+130); A = B;
  CYC(b_+130, b_+132); A = alu_swap(gb, A);
  CYC(b_+132, b_+133); alu_rlca(gb);
  CYC(b_+133, b_+136);
  specialObjectSetAnimation_hook(gb);
}

void linkCutscene3_hook(GB *gb) {
  BASE(linkCutscene3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+16) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+16);
  TAIL(specialObjectSetAnimation);

state1:
  CYC(b_+16, b_+18); E = 0x05;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+40) { goto substate0; }
    else if (jt_ == b_+63) { goto substate1; }
    else if (jt_ == b_+77) { goto substate2; }
    else if (jt_ == b_+88) { goto substate3; }
    else if (jt_ == b_+104) { goto substate4; }
    else if (jt_ == b_+135) { goto substate5; }
    else if (jt_ == b_+149) { goto substate6; }
    else if (jt_ == b_+170) { goto substate7; }
    else if (jt_ == b_+184) { goto substate8; }
    else if (jt_ == b_+148) { goto substate9; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+40, b_+43); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+43, b_+45); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(b_+45, b_+46); ret_effect(gb); return; }
  CYC(b_+45, b_+46);
  CALL_C(b_+46, itemIncSubstate_hook, SYM(itemIncSubstate), b_+49);
  CYC(b_+49, b_+51); L = 0x0b;
  CYC(b_+51, b_+53); A = 0x30;
  CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+54, b_+55); L = alu_inc8(gb, L);
  CYC(b_+55, b_+57); A = 0x78;
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); A = 0x01;
  CYC(b_+60, b_+63);
  TAIL(specialObjectSetAnimation);

substate1:
  CYC(b_+63, b_+66); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+66, b_+68); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+68, b_+69); ret_effect(gb); return; }
  CYC(b_+68, b_+69);
  CALL_C(b_+69, itemIncSubstate_hook, SYM(itemIncSubstate), b_+72);
  CYC(b_+72, b_+74); L = 0x06;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x1e);
  CYC(b_+76, b_+77); ret_effect(gb);
  return;

substate2:
  CALL_C(b_+77, itemDecCounter1_hook, SYM(itemDecCounter1), b_+80);
  if (!(F & FZ)) { CYCT(b_+80, b_+81); ret_effect(gb); return; }
  CYC(b_+80, b_+81);
  CALL_C(b_+81, itemIncSubstate_hook, SYM(itemIncSubstate), b_+84);
  CYC(b_+84, b_+85); alu_xor(gb, A);
  CYC(b_+85, b_+88);
  TAIL(specialObjectSetAnimation);

substate3:
  CYC(b_+88, b_+90); B = 0x0e;
  CYC(b_+90, b_+92); C = 0x02;
  CYC(b_+92, b_+95); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+95, b_+96); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+96, b_+97); ret_effect(gb); return; }
  CYC(b_+96, b_+97);
  CALL_C(b_+97, itemIncSubstate_hook, SYM(itemIncSubstate), b_+100);
  CYC(b_+100, b_+101); A = C;
  CYC(b_+101, b_+104);
  TAIL(specialObjectSetAnimation);

substate4:
  CYC(b_+104, b_+107); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+107, b_+109); alu_cp(gb, 0x11);
  if (!(F & FZ)) { CYCT(b_+109, b_+110); ret_effect(gb); return; }
  CYC(b_+109, b_+110);
  CALL_C(b_+110, itemIncSubstate_hook, SYM(itemIncSubstate), b_+113);
  CYC(b_+113, b_+115); L = 0x09;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x18);
  CYC(b_+117, b_+119); L = 0x10;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x3c);
  CYC(b_+121, b_+123); L = 0x06;
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x16);
  CYC(b_+125, b_+127); A = 0x75;
  CALL_C(b_+127, playSound_b00_hook, SYM(playSound_b00), b_+130);
  CYC(b_+130, b_+132); A = 0x03;
  CYC(b_+132, b_+135);
  TAIL(specialObjectSetAnimation);

substate5:
  CALL_C(b_+135, linkCutscene_animateAndDecCounter1_hook, SYM(linkCutscene_animateAndDecCounter1), b_+138);
  if (!(F & FZ)) {
    CYCT(b_+138, b_+141);
    TAIL(objectApplySpeed);
  }
  CYC(b_+138, b_+141);
  CALL_C(b_+141, itemIncSubstate_hook, SYM(itemIncSubstate), b_+144);
  CYC(b_+144, b_+146); L = 0x06;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x06);

substate9:
  CYC(b_+148, b_+149); ret_effect(gb);
  return;

substate6:
  CALL_C(b_+149, itemDecCounter1_hook, SYM(itemDecCounter1), b_+152);
  if (!(F & FZ)) { CYCT(b_+152, b_+153); ret_effect(gb); return; }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+155); mem_wr(gb, HL, 0x08);
  CYC(b_+155, b_+157); L = 0x09;
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0x10);
  CYC(b_+159, b_+161); A = 0x02;
  CYC(b_+161, b_+163); L = 0x08;
  CYC(b_+163, b_+164); mem_wr(gb, HL, A);
  CALL_C(b_+164, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+167);
  CYC(b_+167, b_+170);
  TAIL(itemIncSubstate);

substate7:
  CALL_C(b_+170, linkCutscene_animateAndDecCounter1_hook, SYM(linkCutscene_animateAndDecCounter1), b_+173);
  if (!(F & FZ)) {
    CYCT(b_+173, b_+176);
    TAIL(objectApplySpeed);
  }
  CYC(b_+173, b_+176);
  CYC(b_+176, b_+178); A = 0x75;
  CALL_C(b_+178, playSound_b00_hook, SYM(playSound_b00), b_+181);
  CYC(b_+181, b_+184);
  TAIL(itemIncSubstate);

substate8:
  CYC(b_+184, b_+187); A = W8(wTmpcfc0_genericCutscene_cfd2);
  CYC(b_+187, b_+188); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+188, b_+190);
    TAIL(linkCutsceneFunc_73e8);
  }
  CYC(b_+188, b_+190);
  CYC(b_+190, b_+192); A = 0x03;
  CALL_C(b_+192, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+195);
  CYC(b_+195, b_+198);
  TAIL(itemIncSubstate);
}

void linkCutscene4_hook(GB *gb) {
  BASE(linkCutscene4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+24) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x0b;
  CYC(b_+13, b_+15); A = 0x38;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); A = 0x58;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+24);
  TAIL(specialObjectSetAnimation);

state1:
  CYC(b_+24, b_+26); E = 0x05;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+40) { goto substate0; }
    else if (jt_ == b_+49) { goto substate1; }
    else if (jt_ == b_+65) { goto substate2; }
    else if (jt_ == b_+80) { goto substate3; }
    else if (jt_ == b_+91) { goto substate4; }
    else if (jt_ == b_+102) { goto substate5; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+40, b_+43); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+43, b_+45); alu_cp(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+45, b_+46); ret_effect(gb); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+49);
  TAIL(itemIncSubstate);

substate1:
  CYC(b_+49, b_+52); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+52, b_+54); alu_cp(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+54, b_+57);
    TAIL(linkCutsceneFunc_73e8);
  }
  CYC(b_+54, b_+57);
  CALL_C(b_+57, itemIncSubstate_hook, SYM(itemIncSubstate), b_+60);
  CYC(b_+60, b_+62); L = 0x06;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x50);
  CYC(b_+64, b_+65); ret_effect(gb);
  return;

substate2:
  CALL_C(b_+65, itemDecCounter1_hook, SYM(itemDecCounter1), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+69); ret_effect(gb); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x30);
  CYC(b_+71, b_+73); L = 0x10;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x28);
  CYC(b_+75, b_+77); B = 0x10;
  CYC(b_+77, b_+80);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  return;

substate3:
  CALL_C(b_+80, linkCutscene_animateAndDecCounter1_hook, SYM(linkCutscene_animateAndDecCounter1), b_+83);
  if (!(F & FZ)) {
    CYCT(b_+83, b_+86);
    TAIL(objectApplySpeed);
  }
  CYC(b_+83, b_+86);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x08);
  CYC(b_+88, b_+91);
  TAIL(itemIncSubstate);

substate4:
  CALL_C(b_+91, itemDecCounter1_hook, SYM(itemDecCounter1), b_+94);
  if (!(F & FZ)) { CYCT(b_+94, b_+95); ret_effect(gb); return; }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x10);
  CYC(b_+97, b_+99); B = 0x18;
  CYC(b_+99, b_+102);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  return;

substate5:
  CALL_C(b_+102, linkCutscene_animateAndDecCounter1_hook, SYM(linkCutscene_animateAndDecCounter1), b_+105);
  if (!(F & FZ)) {
    CYCT(b_+105, b_+108);
    TAIL(objectApplySpeed);
  }
  CYC(b_+105, b_+108);
  CYC(b_+108, b_+110); A = 0x21;
  CYC(b_+110, b_+113); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(b_+113, b_+115); A = 0x81;
  CYC(b_+115, b_+118); W8(wMenuDisabled) = A;
  CYC(b_+118, b_+121); W8(wDisabledObjects) = A;
  CYC(b_+121, b_+123); E = 0x08;
  CYC(b_+123, b_+125); A = 0x03;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+127); alu_xor(gb, A);
  CYC(b_+127, b_+130);
  TAIL(setLinkIDOverride);
}

void linkCutscene1_hook(GB *gb) {
  BASE(linkCutscene1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+23) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); E = 0x06;
  CYC(b_+13, b_+15); A = 0x78;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+19); E = 0x08;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+23);

state1:
  CYC(b_+23, b_+25); E = 0x05;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); push_effect(gb, b_+27);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+37) { goto substate0; }
    else if (jt_ == b_+72) { goto substate1; }
    else if (jt_ == b_+93) { goto substate2; }
    else if (jt_ == b_+110) { goto substate3; }
    else if (jt_ == SYM(linkCutsceneRet) && hook_enabled_at(gb, SYM(linkCutsceneRet))) { linkCutsceneRet_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+37, itemDecCounter1_hook, SYM(itemDecCounter1), b_+40);
  if (!(F & FZ)) { CYCT(b_+40, b_+41); ret_effect(gb); return; }
  CYC(b_+40, b_+41);
  CALL_C(b_+41, itemIncSubstate_hook, SYM(itemIncSubstate), b_+44);
  CYC(b_+44, b_+46); L = 0x10;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x28);
  CYC(b_+48, b_+50); L = 0x0d;
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+53); alu_cp(gb, 0x48);
  CYC(b_+53, b_+55); A = 0x00;
  if (F & FZ) {
    CYCT(b_+55, b_+57);
  } else {
    CYC(b_+55, b_+57);
    CYC(b_+57, b_+59); A = 0x18;
    if (!(F & FC)) {
      CYCT(b_+59, b_+61);
    } else {
      CYC(b_+59, b_+61);
      CYC(b_+61, b_+63); A = 0x08;
    }
  }
  CYC(b_+63, b_+65); L = 0x09;
  CYC(b_+65, b_+66); mem_wr(gb, HL, A);
  CYC(b_+66, b_+68); A = alu_swap(gb, A);
  CYC(b_+68, b_+69); alu_rlca(gb);
  CYC(b_+69, b_+72);
  TAIL(specialObjectSetAnimation);

substate1:
  CYC(b_+72, b_+74); E = 0x0d;
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+77); alu_cp(gb, 0x48);
  if (!(F & FZ)) {
    CYCT(b_+77, b_+79);
    CALL_C(b_+87, objectApplySpeed_hook, SYM(objectApplySpeed), b_+90);
    CYC(b_+90, b_+93);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+77, b_+79);
  CALL_C(b_+79, itemIncSubstate_hook, SYM(itemIncSubstate), b_+82);
  CYC(b_+82, b_+84); L = 0x06;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x04);
  CYC(b_+86, b_+87); ret_effect(gb);
  return;

substate2:
  CALL_C(b_+93, itemDecCounter1_hook, SYM(itemDecCounter1), b_+96);
  if (!(F & FZ)) { CYCT(b_+96, b_+97); ret_effect(gb); return; }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x2e);
  CALL_C(b_+99, itemIncSubstate_hook, SYM(itemIncSubstate), b_+102);
  CYC(b_+102, b_+104); L = 0x09;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x00);
  CYC(b_+106, b_+107); alu_xor(gb, A);
  CYC(b_+107, b_+110);
  TAIL(specialObjectSetAnimation);

substate3:
  CALL_C(b_+110, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+113);
  CALL_C(b_+113, objectApplySpeed_hook, SYM(objectApplySpeed), b_+116);
  CALL_C(b_+116, itemDecCounter1_hook, SYM(itemDecCounter1), b_+119);
  if (!(F & FZ)) { CYCT(b_+119, b_+120); ret_effect(gb); return; }
  CYC(b_+119, b_+120);
  CYC(b_+120, b_+123); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x01);
  CYC(b_+125, b_+127); A = 0x50;
  CALL_C(b_+127, playSound_b00_hook, SYM(playSound_b00), b_+130);
  CYC(b_+130, b_+133);
  TAIL(itemIncSubstate);
}

void linkCutscene2_hook(GB *gb) {
  BASE(linkCutscene2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+26) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+14); SET_BC(0x3838);
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = 0x09;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+23);
  CYC(b_+23, b_+26);
  TAIL(specialObjectSetAnimation);

state1:
  CYC(b_+26, b_+28); E = 0x05;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); push_effect(gb, b_+30);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+48) { goto substate0; }
    else if (jt_ == b_+100) { goto substate1; }
    else if (jt_ == b_+117) { goto substate2; }
    else if (jt_ == b_+136) { goto substate3; }
    else if (jt_ == b_+158) { goto substate4; }
    else if (jt_ == b_+175) { goto substate5; }
    else if (jt_ == b_+180) { goto substate6; }
    else if (jt_ == b_+191) { goto substate7; }
    else if (jt_ == b_+208) { goto substate8; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+48, b_+51); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+51, b_+53); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, itemIncSubstate_hook, SYM(itemIncSubstate), b_+57);
  CYC(b_+57, b_+59); L = 0x0b;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+60, b_+62); alu_cp(gb, 0x48);
  CYC(b_+62, b_+64); A = 0x18;
  CYC(b_+64, b_+66); B = 0x04;
  if (F & FZ) {
    CYCT(b_+66, b_+68);
  } else {
    CYC(b_+66, b_+68);
    CYC(b_+68, b_+70); A = 0x10;
    if (F & FC) {
      CYCT(b_+70, b_+72);
    } else {
      CYC(b_+70, b_+72);
      CYC(b_+72, b_+73); L = alu_inc8(gb, L);
      CYC(b_+73, b_+75); B = 0x01;
      CYC(b_+75, b_+76); A = mem_rd(gb, HL);
      CYC(b_+76, b_+78); alu_cp(gb, 0x38);
      CYC(b_+78, b_+80); A = 0x00;
      if (F & FZ) {
        CYCT(b_+80, b_+82);
      } else {
        CYC(b_+80, b_+82);
        CYC(b_+82, b_+84); A = 0x18;
        if (!(F & FC)) {
          CYCT(b_+84, b_+86);
        } else {
          CYC(b_+84, b_+86);
          CYC(b_+86, b_+88); A = 0x08;
        }
      }
    }
  }
  CYC(b_+88, b_+90); L = 0x05;
  CYC(b_+90, b_+91); mem_wr(gb, HL, B);
  CYC(b_+91, b_+93); L = 0x09;
  CYC(b_+93, b_+94); mem_wr(gb, HL, A);
  CYC(b_+94, b_+96); A = alu_swap(gb, A);
  CYC(b_+96, b_+97); alu_rlca(gb);
  CYC(b_+97, b_+100);
  TAIL(specialObjectSetAnimation);

substate1:
  CALL_C(b_+100, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+103);
  CALL_C(b_+103, linkCutscene_cpxTo38_hook, SYM(linkCutscene_cpxTo38), b_+106);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+109);
    TAIL(objectApplySpeed);
  }
  CYC(b_+106, b_+109);
  CALL_C(b_+109, itemIncSubstate_hook, SYM(itemIncSubstate), b_+112);
  CYC(b_+112, b_+114); L = 0x06;
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x08);
  CYC(b_+116, b_+117); ret_effect(gb);
  return;

substate2:
  CYC(b_+117, b_+119); B = 0x00;
  goto label_72c8;

substate3:
  CALL_C(b_+136, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+139);
  CALL_C(b_+139, linkCutscene_cpyTo48_hook, SYM(linkCutscene_cpyTo48), b_+142);
  if (!(F & FZ)) {
    CYCT(b_+142, b_+145);
    TAIL(objectApplySpeed);
  }
  CYC(b_+142, b_+145);
  goto goto_state7;

substate4:
  CALL_C(b_+158, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+161);
  CALL_C(b_+161, linkCutscene_cpyTo48_hook, SYM(linkCutscene_cpyTo48), b_+164);
  if (!(F & FZ)) {
    CYCT(b_+164, b_+167);
    TAIL(objectApplySpeed);
  }
  CYC(b_+164, b_+167);
  CALL_C(b_+167, itemIncSubstate_hook, SYM(itemIncSubstate), b_+170);
  CYC(b_+170, b_+172); L = 0x06;
  CYC(b_+172, b_+174); mem_wr(gb, HL, 0x08);
  CYC(b_+174, b_+175); ret_effect(gb);
  return;

substate5:
  CYC(b_+175, b_+177); B = 0x18;
  CYC(b_+177, b_+180);
  goto label_72c8;

substate6:
  CALL_C(b_+180, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+183);
  CALL_C(b_+183, linkCutscene_cpxTo38_hook, SYM(linkCutscene_cpxTo38), b_+186);
  if (!(F & FZ)) {
    CYCT(b_+186, b_+189);
    TAIL(objectApplySpeed);
  }
  CYC(b_+186, b_+189);
  CYC(b_+189, b_+191);
  goto goto_state7;

substate7:
  CALL_C(b_+191, itemDecCounter1_hook, SYM(itemDecCounter1), b_+194);
  if (!(F & FZ)) { CYCT(b_+194, b_+195); ret_effect(gb); return; }
  CYC(b_+194, b_+195);
  CYC(b_+195, b_+197); mem_wr(gb, HL, 0x10);
  CYC(b_+197, b_+199); B = 0x00;
  CYC(b_+199, b_+202); push_effect(gb, b_+202);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  CYC(b_+202, b_+205); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+205, b_+207); mem_wr(gb, HL, 0x03);
  CYC(b_+207, b_+208); ret_effect(gb);
  return;

substate8:
  CYC(b_+208, b_+209); ret_effect(gb);
  return;

label_72c8:
  CALL_C(b_+119, itemDecCounter1_hook, SYM(itemDecCounter1), b_+122);
  if (!(F & FZ)) { CYCT(b_+122, b_+123); ret_effect(gb); return; }
  CYC(b_+122, b_+123);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  return;

goto_state7:
  CYC(b_+145, b_+146); H = D;
  CYC(b_+146, b_+148); L = 0x05;
  CYC(b_+148, b_+150); mem_wr(gb, HL, 0x07);
  CYC(b_+150, b_+152); L = 0x06;
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x3c);
  CYC(b_+154, b_+155); alu_xor(gb, A);
  CYC(b_+155, b_+158);
  TAIL(specialObjectSetAnimation);
}

void linkCutscene5_hook(GB *gb) {
  BASE(linkCutscene5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+23) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x10;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28);
  CYC(b_+15, b_+17); L = 0x3d;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  CYC(b_+19, b_+21); L = 0x08;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xff);

state1:
  CALL_C(b_+23, linkCutscene_updateAngleOnPath_hook, SYM(linkCutscene_updateAngleOnPath), b_+26);
  if (F & FZ) {
    CYCT(b_+26, b_+28);
  } else {
    CYC(b_+26, b_+28);
    CALL_C(b_+28, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+31);
    CYC(b_+31, b_+34);
    TAIL(objectApplySpeed);
  }
  CYC(b_+34, b_+36); A = 0x00;
  CYC(b_+36, b_+39);
  TAIL(setLinkIDOverride);
}

void linkCutscene6_hook(GB *gb) {
  BASE(linkCutscene6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+30) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x10;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x14);
  CYC(b_+15, b_+17); B = 0x16;
  CYC(b_+17, b_+19); L = 0x09;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
  } else {
    CYC(b_+22, b_+24);
    CYC(b_+24, b_+26); B = 0x15;
  }
  CYC(b_+26, b_+27); A = B;
  CALL_C(b_+27, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+30);

state1:
  CYC(b_+30, b_+32); E = 0x05;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+40) { goto substate0; }
    else if (jt_ == b_+54) { goto substate1; }
    else if (jt_ == b_+68) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+40, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+43);
  CALL_C(b_+43, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+46);
  CYC(b_+46, b_+48); alu_and(gb, 0xc0);
  if (F & FZ) {
    CYCT(b_+48, b_+51);
    TAIL(objectApplySpeed);
  }
  CYC(b_+48, b_+51);
  CYC(b_+51, b_+54);
  TAIL(itemIncSubstate);

substate1:
  CYC(b_+54, b_+57); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+57, b_+59); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+59, b_+60); ret_effect(gb); return; }
  CYC(b_+59, b_+60);
  CALL_C(b_+60, itemIncSubstate_hook, SYM(itemIncSubstate), b_+63);
  CYC(b_+63, b_+65); A = 0x02;
  CYC(b_+65, b_+68);
  TAIL(specialObjectSetAnimation);

substate2:
  CYC(b_+68, b_+69); ret_effect(gb);
}

void linkCutscene7_hook(GB *gb) {
  BASE(linkCutscene7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+20) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x06;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf0);
  CYC(b_+15, b_+17); A = 0x14;
  CYC(b_+17, b_+20);
  TAIL(specialObjectSetAnimation);

state1:
  CALL_C(b_+20, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+23);
  CALL_C(b_+23, itemDecCounter1_hook, SYM(itemDecCounter1), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CALL_C(b_+28, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+31);
  CYC(b_+31, b_+33); L = 0x08;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x02);
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+40); W8(wUseSimulatedInput) = A;
  CYC(b_+40, b_+43); W8(wMenuDisabled) = A;
  CYC(b_+43, b_+44); ret_effect(gb);
}

void linkCutscene8_hook(GB *gb) {
  BASE(linkCutscene8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+27) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x0b;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x68);
  CYC(b_+15, b_+17); L = 0x0d;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x50);
  CYC(b_+19, b_+21); A = 0x00;
  CALL_C(b_+21, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+24);
  CYC(b_+24, b_+27);
  TAIL(objectSetInvisible);

state1:
  CYC(b_+27, b_+29); E = 0x05;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); push_effect(gb, b_+31);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+35) { goto substate0; }
    else if (jt_ == b_+54) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+35, b_+38); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+38, b_+40); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+40, b_+42);
  } else {
    CYC(b_+40, b_+42);
    CYC(b_+42, b_+45); A = W8(wTmpcfc0_genericCutscene_cfd0);
    CYC(b_+45, b_+47); alu_cp(gb, 0x01);
    if (!(F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; }
    CYC(b_+47, b_+48);
  }
  CALL_C(b_+48, itemIncSubstate_hook, SYM(itemIncSubstate), b_+51);
  CYC(b_+51, b_+54);
  TAIL(objectSetVisiblec2);

substate1:
  CYC(b_+54, b_+55); ret_effect(gb);
}

void linkCutscene9_hook(GB *gb) {
  BASE(linkCutscene9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+19) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); A = 0x02;
  CALL_C(b_+13, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+16);
  CYC(b_+16, b_+19);
  TAIL(objectSetInvisible);

state1:
  CYC(b_+19, b_+21); E = 0x05;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); push_effect(gb, b_+23);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+33) { goto substate0; }
    else if (jt_ == b_+45) { goto substate1; }
    else if (jt_ == b_+54) { goto substate2; }
    else if (jt_ == b_+76) { goto substate3; }
    else if (jt_ == b_+94) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+33, b_+36); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+36, b_+38); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+38, b_+39); ret_effect(gb); return; }
  CYC(b_+38, b_+39);
  CALL_C(b_+39, itemIncSubstate_hook, SYM(itemIncSubstate), b_+42);
  CYC(b_+42, b_+45);
  TAIL(objectSetVisible82);

substate1:
  CYC(b_+45, b_+48); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+48, b_+50); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
  CYC(b_+50, b_+51);
  CALL_C(b_+51, itemIncSubstate_hook, SYM(itemIncSubstate), b_+54);

substate2:
  CYC(b_+54, b_+57); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+57, b_+59); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+62);
    TAIL(linkCutsceneFunc_73e8);
  }
  CYC(b_+59, b_+62);
  CALL_C(b_+62, itemIncSubstate_hook, SYM(itemIncSubstate), b_+65);
  CYC(b_+65, b_+68); SET_BC(0xfe40);
  CALL_C(b_+68, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+71);
  CYC(b_+71, b_+73); A = 0x0d;
  CYC(b_+73, b_+76);
  TAIL(specialObjectSetAnimation);

substate3:
  CYC(b_+76, b_+78); C = 0x20;
  CALL_C(b_+78, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+82); ret_effect(gb); return; }
  CYC(b_+81, b_+82);
  CALL_C(b_+82, itemIncSubstate_hook, SYM(itemIncSubstate), b_+85);
  CYC(b_+85, b_+87); L = 0x06;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x78);
  CYC(b_+89, b_+91); L = 0x20;
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x01);
  CYC(b_+93, b_+94); ret_effect(gb);
  return;

substate4:
  CALL_C(b_+94, itemDecCounter1_hook, SYM(itemDecCounter1), b_+97);
  if (!(F & FZ)) {
    CYCT(b_+97, b_+100);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+103); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0xff);
  CYC(b_+105, b_+106); ret_effect(gb);
}

static void link_cutscene_check_shield_equipped(GB *gb) {
  BASE(linkCutsceneA);
  CYC(b_+26, b_+29); SET_HL(wInventoryB);
  CYC(b_+29, b_+31); A = 0x01;
  CYC(b_+31, b_+32); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+32, b_+33); ret_effect(gb); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+35); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); ret_effect(gb);
}

void linkCutsceneA_hook(GB *gb) {
  BASE(linkCutsceneA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+36) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CALL_C(b_+11, objectSetInvisible_hook, SYM(objectSetInvisible), b_+14);
  CYC(b_+14, b_+17); push_effect(gb, b_+17);
  link_cutscene_check_shield_equipped(gb);
  CYC(b_+17, b_+19); A = 0x0b;
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
  } else {
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+23); A = 0x0f;
  }
  CYC(b_+23, b_+26);
  TAIL(specialObjectSetAnimation);

state1:
  CYC(b_+36, b_+38); E = 0x05;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); push_effect(gb, b_+40);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+48) { goto substate0; }
    else if (jt_ == b_+60) { goto substate1; }
    else if (jt_ == b_+80) { goto substate2; }
    else if (jt_ == b_+120) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+48, b_+51); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+51, b_+53); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, itemIncSubstate_hook, SYM(itemIncSubstate), b_+57);
  CYC(b_+57, b_+60);
  TAIL(objectSetVisible82);

substate1:
  CYC(b_+60, b_+63); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+63, b_+65); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+65, b_+66); ret_effect(gb); return; }
  CYC(b_+65, b_+66);
  CALL_C(b_+66, itemIncSubstate_hook, SYM(itemIncSubstate), b_+69);
  CYC(b_+69, b_+72); push_effect(gb, b_+72);
  link_cutscene_check_shield_equipped(gb);
  CYC(b_+72, b_+74); A = 0x10;
  if (!(F & FZ)) {
    CYCT(b_+74, b_+76);
  } else {
    CYC(b_+74, b_+76);
    CYC(b_+76, b_+77); A = alu_inc8(gb, A);
  }
  CYC(b_+77, b_+80);
  TAIL(specialObjectSetAnimation);

substate2:
  CYC(b_+80, b_+83); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+83, b_+85); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+85, b_+86); ret_effect(gb); return; }
  CYC(b_+85, b_+86);
  CALL_C(b_+86, itemIncSubstate_hook, SYM(itemIncSubstate), b_+89);
  CYC(b_+89, b_+91); L = 0x06;
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x68);
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x01);
  CYC(b_+96, b_+98); B = 0x02;

spawn_heart:
  CALL_C(b_+98, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+101);
  if (!(F & FZ)) {
    CYCT(b_+101, b_+103);
    goto set_animation;
  }
  CYC(b_+101, b_+103);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0xb7);
  CYC(b_+105, b_+106); L = alu_inc8(gb, L);
  CYC(b_+106, b_+107); A = B;
  CYC(b_+107, b_+108); A = alu_dec8(gb, A);
  CYC(b_+108, b_+109); mem_wr(gb, HL, A);
  CALL_C(b_+109, objectCopyPosition_hook, SYM(objectCopyPosition), b_+112);
  CYC(b_+112, b_+113); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+113, b_+115);
    goto spawn_heart;
  }
  CYC(b_+113, b_+115);

set_animation:
  CYC(b_+115, b_+117); A = 0x12;
  CYC(b_+117, b_+120);
  TAIL(specialObjectSetAnimation);

substate3:
  CALL_C(b_+120, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+123);
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = 0x06;
  CALL_C(b_+126, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+129);
  if (!(F & FZ)) { CYCT(b_+129, b_+130); ret_effect(gb); return; }
  CYC(b_+129, b_+130);
  CYC(b_+130, b_+133); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x09);
  CYC(b_+135, b_+136); ret_effect(gb);
}

void linkCutsceneB_hook(GB *gb) {
  BASE(linkCutsceneB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+52) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CALL_C(b_+11, objectSetVisible81_hook, SYM(objectSetVisible81), b_+14);
  CYC(b_+14, b_+16); L = 0x06;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x2c);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x01);
  CYC(b_+21, b_+23); L = 0x0b;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0xd0);
  CYC(b_+25, b_+27); L = 0x0d;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x50);
  CYC(b_+29, b_+31); A = 0x08;
  CALL_C(b_+31, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+34);
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+38); W8(wTmpcbb9) = A;
  CYC(b_+38, b_+41); SET_BC(0x840d);
  CALL_C(b_+41, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+44);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+46);
  } else {
    CYC(b_+44, b_+46);
    CYC(b_+46, b_+48); L = 0x56;
    CYC(b_+48, b_+50); A = 0x00;
    CYC(b_+50, b_+51); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+51, b_+52); mem_wr(gb, HL, D);
  }

state1:
  CYC(b_+52, b_+55); A = W8(wFrameCounter);
  CYC(b_+55, b_+58); W8(wTmpcbb7) = A;
  CYC(b_+58, b_+60); E = 0x05;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); push_effect(gb, b_+62);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+70) { goto substate0; }
    else if (jt_ == b_+85) { goto substate1; }
    else if (jt_ == b_+101) { goto substate2; }
    else if (jt_ == b_+125) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+70, linkCutscene_oscillateZ_2_hook, SYM(linkCutscene_oscillateZ_2), b_+73);
  CYC(b_+73, b_+76); SET_HL(w1Link_counter1);
  CALL_C(b_+76, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+79);
  if (!(F & FZ)) { CYCT(b_+79, b_+80); ret_effect(gb); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x3c);
  CYC(b_+82, b_+85);
  TAIL(itemIncSubstate);

substate1:
  CALL_C(b_+85, linkCutscene_oscillateZ_2_hook, SYM(linkCutscene_oscillateZ_2), b_+88);
  CALL_C(b_+88, itemDecCounter1_hook, SYM(itemDecCounter1), b_+91);
  if (!(F & FZ)) { CYCT(b_+91, b_+92); ret_effect(gb); return; }
  CYC(b_+91, b_+92);
  CALL_C(b_+92, itemIncSubstate_hook, SYM(itemIncSubstate), b_+95);
  CYC(b_+95, b_+98); SET_BC(0x1213);
  CYC(b_+98, b_+101);
  TAIL(showText);

substate2:
  CYC(b_+101, b_+104); SET_HL(SYM(linkCutscene_zOscillation1));
  CALL_C(b_+104, linkCutscene_oscillateZ_hook, SYM(linkCutscene_oscillateZ), b_+107);
  CYC(b_+107, b_+110); A = W8(wTextIsActive);
  CYC(b_+110, b_+111); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+111, b_+112); ret_effect(gb); return; }
  CYC(b_+111, b_+112);
  CYC(b_+112, b_+114); A = 0x06;
  CYC(b_+114, b_+117); W8(wTmpcbb9) = A;
  CYC(b_+117, b_+119); A = 0x91;
  CALL_C(b_+119, playSound_b00_hook, SYM(playSound_b00), b_+122);
  CYC(b_+122, b_+125);
  TAIL(linkCutscene_createGlowingOrb);

substate3:
  CYC(b_+125, b_+127); E = 0x21;
  CYC(b_+127, b_+128); A = mem_rd(gb, DE);
  CYC(b_+128, b_+129); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+129, b_+131);
  } else {
    CYC(b_+129, b_+131);
    CYC(b_+131, b_+133); A = 0x07;
    CYC(b_+133, b_+136); W8(wTmpcbb9) = A;
    CYC(b_+136, b_+137); ret_effect(gb);
    return;
  }
  CALL_C(b_+137, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+140);
  CYC(b_+140, b_+143); A = W8(wFrameCounter);
  CYC(b_+143, b_+144); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+144, b_+147);
    TAIL(objectSetInvisible);
  }
  CYC(b_+144, b_+147);
  CYC(b_+147, b_+150);
  TAIL(objectSetVisible);
}

void linkCutsceneC_hook(GB *gb) {
  BASE(linkCutsceneC);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == SYM(linkCutsceneRet) && hook_enabled_at(gb, SYM(linkCutsceneRet))) { linkCutsceneRet_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, linkCutscene_initOam_setVisible_incState_hook, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+14); SET_BC(0xf804);
  CYC(b_+14, b_+16); A = 0xff;
  CALL_C(b_+16, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+19);
  CYC(b_+19, b_+21); L = 0x42;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x01);
  CYC(b_+23, b_+25); A = 0x06;
  CYC(b_+25, b_+28);
  TAIL(specialObjectSetAnimation);
}

void specialObjectCode_linkInCutscene_b06_hook(GB *gb) {
  BASE(specialObjectCode_linkInCutscene_b06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+O(2)); E = 0x02;
  CYC(b_+O(2), b_+O(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+O(4)); push_effect(gb, b_+O(4));
  do { uint16_t jt_ = (link_cutscene_jump_table(gb));
    if (jt_ == SYM(linkCutscene0) && hook_enabled_at(gb, SYM(linkCutscene0))) { linkCutscene0_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene1) && hook_enabled_at(gb, SYM(linkCutscene1))) { linkCutscene1_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene2) && hook_enabled_at(gb, SYM(linkCutscene2))) { linkCutscene2_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene3) && hook_enabled_at(gb, SYM(linkCutscene3))) { linkCutscene3_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene4) && hook_enabled_at(gb, SYM(linkCutscene4))) { linkCutscene4_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene5) && hook_enabled_at(gb, SYM(linkCutscene5))) { linkCutscene5_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene6) && hook_enabled_at(gb, SYM(linkCutscene6))) { linkCutscene6_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene7) && hook_enabled_at(gb, SYM(linkCutscene7))) { linkCutscene7_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene8) && hook_enabled_at(gb, SYM(linkCutscene8))) { linkCutscene8_hook(gb); return; }
    else if (jt_ == SYM(linkCutscene9) && hook_enabled_at(gb, SYM(linkCutscene9))) { linkCutscene9_hook(gb); return; }
    else if (jt_ == SYM(linkCutsceneA) && hook_enabled_at(gb, SYM(linkCutsceneA))) { linkCutsceneA_hook(gb); return; }
    else if (jt_ == SYM(linkCutsceneB) && hook_enabled_at(gb, SYM(linkCutsceneB))) { linkCutsceneB_hook(gb); return; }
    else if (jt_ == SYM(linkCutsceneC) && hook_enabled_at(gb, SYM(linkCutsceneC))) { linkCutsceneC_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
