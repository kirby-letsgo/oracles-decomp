#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void tuniNut_state0_hook(GB *gb);
void tuniNut_gotoState4_hook(GB *gb);
void tuniNut_state1_hook(GB *gb);
void tuniNut_state2_hook(GB *gb);
void tuniNut_beginMovingIntoPlace_hook(GB *gb);
void tuniNut_state3_hook(GB *gb);

static uint16_t tuni_nut_jump_table(GB *gb) {
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

void tuniNut_gotoState4_hook(GB *gb) {
  BASE(tuniNut_gotoState4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x1878);
  CALL_C(b_+3, interactionSetPosition_hook, SYM(interactionSetPosition), b_+6);
  CYC(b_+6, b_+8); L = 0x44;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x04);
  CYC(b_+10, b_+12); A = 0x06;
  CALL_C(b_+12, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+15);
  CYC(b_+15, b_+18);
  TAIL(objectSetVisible82);
}

static void tuni_nut_state0_after_graphics(GB *gb, uint16_t sp0_) {
  BASE(tuniNut_state0);
  CYC(b_+3, b_+5); A = 0x29;
  CALL_C(b_+5, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+8);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    TAIL(tuniNut_gotoState4);
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); A = 0x4c;
  CALL_C(b_+12, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+15);
  if (!(F & FC)) {
    CYCT(b_+15, b_+17);
    CYC(b_+30, b_+33);
    TAIL(interactionDelete);
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    CYC(b_+30, b_+33);
    TAIL(interactionDelete);
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); SET_BC(0x0810);
  CALL_C(b_+24, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+27);
  CYC(b_+27, b_+30);
  interactionIncState_hook(gb);
}

void tuniNut_state0__afterCall7b71_hook(GB *gb) {
  tuni_nut_state0_after_graphics(gb, gb->sp);
}

void tuniNut_state0_hook(GB *gb) {
  BASE(tuniNut_state0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  tuni_nut_state0_after_graphics(gb, sp0_);
}

void tuniNut_beginMovingIntoPlace_hook(GB *gb) {
  BASE(tuniNut_beginMovingIntoPlace);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, w1Link_direction, A);
  CYC(b_+4, b_+6); E = 0x46;
  CYC(b_+6, b_+8); A = 0x3c;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+12); SET_BC(0x8407);
  CALL_C(b_+12, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+15);
  CYC(b_+15, b_+17); L = 0x56;
  CYC(b_+17, b_+18); A = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+20); A = D;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CALL_C(b_+21, darkenRoomLightly_hook, SYM(darkenRoomLightly), b_+24);
  CYC(b_+24, b_+26); A = 0xf0;
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CALL_C(b_+29, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+32);
  CYC(b_+32, b_+35);
  TAIL(interactionIncState);
}

void tuniNut_state1_hook(GB *gb) {
  BASE(tuniNut_state1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+7);
  if (!(F & FC)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); push_effect(gb, DE);
  CALL_C(b_+9, clearAllItemsAndPutLinkOnGround_hook, SYM(clearAllItemsAndPutLinkOnGround), b_+12);
  CYC(b_+12, b_+13); SET_DE(pop_effect(gb));
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+21); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+21, b_+24); A = mem_rd(gb, w1Link_xh);
  CYC(b_+24, b_+26); alu_sub(gb, 0x78);
  if (F & FZ) {
    CYCT(b_+26, b_+28);
    CALL_C(b_+62, interactionIncState_hook, SYM(interactionIncState), b_+65);
    CYC(b_+65, b_+67);
    TAIL(tuniNut_beginMovingIntoPlace);
  }
  CYC(b_+26, b_+28);
  if (F & FC) {
    CYCT(b_+28, b_+30);
    CYC(b_+34, b_+35); alu_cpl(gb);
    CYC(b_+35, b_+36); A = alu_inc8(gb, A);
    CYC(b_+36, b_+38); B = 0x01;
  } else {
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+32); B = 0x03;
    CYC(b_+32, b_+34);
  }
  CYC(b_+38, b_+41); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+41, b_+43); E = 0x46;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+45); A = B;
  CYC(b_+45, b_+48); mem_wr(gb, w1Link_direction, A);
  CYC(b_+48, b_+50); A = alu_swap(gb, A);
  CYC(b_+50, b_+51); alu_rrca(gb);
  CYC(b_+51, b_+54); mem_wr(gb, w1Link_angle, A);
  CYC(b_+54, b_+56); A = 0x0b;
  CYC(b_+56, b_+59); mem_wr(gb, wLinkForceState, A);
  CYC(b_+59, b_+62);
  TAIL(interactionIncState);
}

void tuniNut_state2_hook(GB *gb) {
  BASE(tuniNut_state2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  TAIL(tuniNut_beginMovingIntoPlace);
}

static void tuni_nut_set_row(GB *gb) {
  BASE(tuniNut_state3);
  CYC(b_+146, b_+148); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+148, b_+149); L = alu_inc8(gb, L);
  CYC(b_+149, b_+151); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+151, b_+152); L = alu_inc8(gb, L);
  CYC(b_+152, b_+154); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+154, b_+155); L = alu_inc8(gb, L);
  CYC(b_+155, b_+156); ret_effect(gb);
}

static void tuni_nut_set_room_flags(GB *gb) {
  BASE(tuniNut_state3);
  CYC(b_+138, b_+141); SET_HL(wGroup0RoomFlags + 0x02);
  CYC(b_+141, b_+144); push_effect(gb, b_+144);
  tuni_nut_set_row(gb);
  CYC(b_+144, b_+146); L = 0x12;
  tuni_nut_set_row(gb);
}

void tuniNut_state3_hook(GB *gb) {
  BASE(tuniNut_state3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (tuni_nut_jump_table(gb));
    if (jt_ == b_+16) {
      CALL_C(b_+16, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+19);
      if (!(F & FZ)) {
        CYCT(b_+19, b_+20); ret_effect(gb);
        return;
      }
      CYC(b_+19, b_+20);
      CYC(b_+20, b_+22); mem_wr(gb, HL, 0x10);
      CYC(b_+22, b_+25); interactionIncSubstate_hook(gb);
      return;
    }
    else if (jt_ == b_+25) {
      CYC(b_+25, b_+28); A = mem_rd(gb, wFrameCounter);
      CYC(b_+28, b_+29); alu_rrca(gb);
      if (F & FC) {
        CYCT(b_+29, b_+30); ret_effect(gb);
        return;
      }
      CYC(b_+29, b_+30);
      CYC(b_+30, b_+31); H = D;
      CYC(b_+31, b_+33); L = 0x4f;
      CYC(b_+33, b_+34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
      CALL_C(b_+34, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+37);
      if (!(F & FZ)) {
        CYCT(b_+37, b_+38); ret_effect(gb);
        return;
      }
      CYC(b_+37, b_+38);
      CALL_C(b_+38, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+41);
      CYC(b_+41, b_+44); interactionIncSubstate_hook(gb);
      return;
    }
    else if (jt_ == b_+44) {
      CYC(b_+44, b_+46); B = 0x0a;
      CYC(b_+46, b_+48); C = 0x00;
      CYC(b_+48, b_+50); E = 0x49;
      CALL_C(b_+50, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+53);
      CYC(b_+53, b_+55); E = 0x4b;
      CYC(b_+55, b_+56); A = mem_rd(gb, DE);
      CYC(b_+56, b_+58); alu_cp(gb, 0x18);
      if (!(F & FC)) {
        CYCT(b_+58, b_+59); ret_effect(gb);
        return;
      }
      CYC(b_+58, b_+59);
      CALL_C(b_+59, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+62);
      CYC(b_+62, b_+65); interactionIncSubstate_hook(gb);
      return;
    }
    else if (jt_ == b_+65) {
      CYC(b_+65, b_+67); C = 0x20;
      CALL_C(b_+67, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+70);
      if (!(F & FZ)) {
        CYCT(b_+70, b_+71); ret_effect(gb);
        return;
      }
      CYC(b_+70, b_+71);
      CYC(b_+71, b_+73); A = 0x77;
      CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
      CYC(b_+76, b_+78); E = 0x46;
      CYC(b_+78, b_+80); A = 0x5a;
      CYC(b_+80, b_+81); mem_wr(gb, DE, A);
      CYC(b_+81, b_+83); A = 0x5b;
      CALL_C(b_+83, playSound_b00_hook, SYM(playSound_b00), b_+86);
      CYC(b_+86, b_+89); interactionIncSubstate_hook(gb);
      return;
    }
    else if (jt_ == b_+89) {
      CALL_C(b_+89, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+92);
      if (!(F & FZ)) {
        CYCT(b_+92, b_+93); ret_effect(gb);
        return;
      }
      CYC(b_+92, b_+93);
      CALL_C(b_+93, brightenRoom_hook, SYM(brightenRoom), b_+96);
      CYC(b_+96, b_+99); interactionIncSubstate_hook(gb);
      return;
    }
    else if (jt_ == b_+99) {
      CYC(b_+99, b_+102); A = mem_rd(gb, wPaletteThread_mode);
      CYC(b_+102, b_+103); alu_or(gb, A);
      if (!(F & FZ)) {
        CYCT(b_+103, b_+104); ret_effect(gb);
        return;
      }
      CYC(b_+103, b_+104);
      CYC(b_+104, b_+106); A = 0x29;
      CALL_C(b_+106, setGlobalFlag_hook, SYM(setGlobalFlag), b_+109);
      CYC(b_+109, b_+111); A = 0x4c;
      CALL_C(b_+111, loseTreasure_hook, SYM(loseTreasure), b_+114);
      CYC(b_+114, b_+117); push_effect(gb, b_+117);
      tuni_nut_set_room_flags(gb);
      CYC(b_+117, b_+118); alu_xor(gb, A);
      CYC(b_+118, b_+121); mem_wr(gb, wDisabledObjects, A);
      CYC(b_+121, b_+124); mem_wr(gb, wMenuDisabled, A);
      CYC(b_+124, b_+127); SET_HL(wTmpcfc0_genericCutscene_state);
      CYC(b_+127, b_+129); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
      CYC(b_+129, b_+132); A = mem_rd(gb, wActiveMusic);
      CALL_C(b_+132, playSound_b00_hook, SYM(playSound_b00), b_+135);
      CYC(b_+135, b_+138);
      TAIL(tuniNut_gotoState4);
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}

void interactionCodeb1_body_hook(GB *gb) {
  BASE(interactionCodeb1_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (tuni_nut_jump_table(gb));
    if (jt_ == SYM(tuniNut_state0) && hook_enabled_at(gb, SYM(tuniNut_state0))) { tuniNut_state0_hook(gb); return; }
    else if (jt_ == SYM(tuniNut_state1) && hook_enabled_at(gb, SYM(tuniNut_state1))) { tuniNut_state1_hook(gb); return; }
    else if (jt_ == SYM(tuniNut_state2) && hook_enabled_at(gb, SYM(tuniNut_state2))) { tuniNut_state2_hook(gb); return; }
    else if (jt_ == SYM(tuniNut_state3) && hook_enabled_at(gb, SYM(tuniNut_state3))) { tuniNut_state3_hook(gb); return; }
    else if (jt_ == SYM(objectPreventLinkFromPassing) && hook_enabled_at(gb, SYM(objectPreventLinkFromPassing))) { objectPreventLinkFromPassing_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
