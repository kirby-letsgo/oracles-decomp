#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b8f, 0x7b92); SET_BC(0x1878);
  CALL_C(0x7b92, interactionSetPosition_hook, 0x2773, 0x7b95);
  CYC(0x7b95, 0x7b97); L = 0x44;
  CYC(0x7b97, 0x7b99); mem_wr(gb, HL, 0x04);
  CYC(0x7b99, 0x7b9b); A = 0x06;
  CALL_C(0x7b9b, objectSetCollideRadius_hook, 0x24a1, 0x7b9e);
  CYC(0x7b9e, 0x7ba1);
  objectSetVisible82_hook(gb);
}

static void tuni_nut_state0_after_graphics(GB *gb, uint16_t sp0_) {
  CYC(0x7b71, 0x7b73); A = 0x29;
  CALL_C(0x7b73, checkGlobalFlag_hook, 0x31f3, 0x7b76);
  if (!(F & FZ)) {
    CYCT(0x7b76, 0x7b78);
    tuniNut_gotoState4_hook(gb);
    return;
  }
  CYC(0x7b76, 0x7b78);
  CYC(0x7b78, 0x7b7a); A = 0x4c;
  CALL_C(0x7b7a, checkTreasureObtained_hook, 0x1748, 0x7b7d);
  if (!(F & FC)) {
    CYCT(0x7b7d, 0x7b7f);
    CYC(0x7b8c, 0x7b8f);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7b7d, 0x7b7f);
  CYC(0x7b7f, 0x7b81); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7b81, 0x7b83);
    CYC(0x7b8c, 0x7b8f);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7b81, 0x7b83);
  CYC(0x7b83, 0x7b86); SET_BC(0x0810);
  CALL_C(0x7b86, objectSetCollideRadii_hook, 0x24a9, 0x7b89);
  CYC(0x7b89, 0x7b8c);
  interactionIncState_hook(gb);
}

void tuniNut_state0__afterCall7b71_hook(GB *gb) {
  tuni_nut_state0_after_graphics(gb, gb->sp);
}

void tuniNut_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b6e, interactionInitGraphics_hook, 0x15fb, 0x7b71);
  tuni_nut_state0_after_graphics(gb, sp0_);
}

void tuniNut_beginMovingIntoPlace_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7be8, 0x7be9); alu_xor(gb, A);
  CYC(0x7be9, 0x7bec); mem_wr(gb, w1Link_direction, A);
  CYC(0x7bec, 0x7bee); E = 0x46;
  CYC(0x7bee, 0x7bf0); A = 0x3c;
  CYC(0x7bf0, 0x7bf1); mem_wr(gb, DE, A);
  CYC(0x7bf1, 0x7bf4); SET_BC(0x8407);
  CALL_C(0x7bf4, objectCreateInteraction_hook, 0x24c5, 0x7bf7);
  CYC(0x7bf7, 0x7bf9); L = 0x56;
  CYC(0x7bf9, 0x7bfa); A = E;
  CYC(0x7bfa, 0x7bfb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7bfb, 0x7bfc); A = D;
  CYC(0x7bfc, 0x7bfd); mem_wr(gb, HL, A);
  CALL_C(0x7bfd, darkenRoomLightly_hook, 0x32f8, 0x7c00);
  CYC(0x7c00, 0x7c02); A = 0xf0;
  CALL_C(0x7c02, playSound_b00_hook, 0x0c98, 0x7c05);
  CALL_C(0x7c05, objectSetVisiblec0_hook, 0x1e33, 0x7c08);
  CYC(0x7c08, 0x7c0b);
  interactionIncState_hook(gb);
}

void tuniNut_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7ba1, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x7ba4);
  if (!(F & FC)) {
    CYCT(0x7ba4, 0x7ba5); ret_effect(gb);
    return;
  }
  CYC(0x7ba4, 0x7ba5);
  CALL_C(0x7ba5, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7ba8);
  if (!(F & FC)) {
    CYCT(0x7ba8, 0x7ba9); ret_effect(gb);
    return;
  }
  CYC(0x7ba8, 0x7ba9);
  CYC(0x7ba9, 0x7baa); push_effect(gb, DE);
  CALL_C(0x7baa, clearAllItemsAndPutLinkOnGround_hook, 0x19ad, 0x7bad);
  CYC(0x7bad, 0x7bae); SET_DE(pop_effect(gb));
  CYC(0x7bae, 0x7bb0); A = 0x01;
  CYC(0x7bb0, 0x7bb3); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7bb3, 0x7bb6); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7bb6, 0x7bb9); A = mem_rd(gb, w1Link_xh);
  CYC(0x7bb9, 0x7bbb); alu_sub(gb, 0x78);
  if (F & FZ) {
    CYCT(0x7bbb, 0x7bbd);
    CALL_C(0x7bdf, interactionIncState_hook, 0x23e0, 0x7be2);
    CYC(0x7be2, 0x7be4);
    tuniNut_beginMovingIntoPlace_hook(gb);
    return;
  }
  CYC(0x7bbb, 0x7bbd);
  if (F & FC) {
    CYCT(0x7bbd, 0x7bbf);
    CYC(0x7bc3, 0x7bc4); alu_cpl(gb);
    CYC(0x7bc4, 0x7bc5); A = alu_inc8(gb, A);
    CYC(0x7bc5, 0x7bc7); B = 0x01;
  } else {
    CYC(0x7bbd, 0x7bbf);
    CYC(0x7bbf, 0x7bc1); B = 0x03;
    CYC(0x7bc1, 0x7bc3);
  }
  CYC(0x7bc7, 0x7bca); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x7bca, 0x7bcc); E = 0x46;
  CYC(0x7bcc, 0x7bcd); mem_wr(gb, DE, A);
  CYC(0x7bcd, 0x7bce); A = B;
  CYC(0x7bce, 0x7bd1); mem_wr(gb, w1Link_direction, A);
  CYC(0x7bd1, 0x7bd3); A = alu_swap(gb, A);
  CYC(0x7bd3, 0x7bd4); alu_rrca(gb);
  CYC(0x7bd4, 0x7bd7); mem_wr(gb, w1Link_angle, A);
  CYC(0x7bd7, 0x7bd9); A = 0x0b;
  CYC(0x7bd9, 0x7bdc); mem_wr(gb, wLinkForceState, A);
  CYC(0x7bdc, 0x7bdf);
  interactionIncState_hook(gb);
}

void tuniNut_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7be4, interactionDecCounter1_hook, 0x23cc, 0x7be7);
  if (!(F & FZ)) {
    CYCT(0x7be7, 0x7be8); ret_effect(gb);
    return;
  }
  CYC(0x7be7, 0x7be8);
  tuniNut_beginMovingIntoPlace_hook(gb);
}

static void tuni_nut_set_row(GB *gb) {
  CYC(0x7c9d, 0x7c9f); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7c9f, 0x7ca0); L = alu_inc8(gb, L);
  CYC(0x7ca0, 0x7ca2); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7ca2, 0x7ca3); L = alu_inc8(gb, L);
  CYC(0x7ca3, 0x7ca5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x7ca5, 0x7ca6); L = alu_inc8(gb, L);
  CYC(0x7ca6, 0x7ca7); ret_effect(gb);
}

static void tuni_nut_set_room_flags(GB *gb) {
  CYC(0x7c95, 0x7c98); SET_HL(wGroup0RoomFlags + 0x02);
  CYC(0x7c98, 0x7c9b); push_effect(gb, 0x7c9b);
  tuni_nut_set_row(gb);
  CYC(0x7c9b, 0x7c9d); L = 0x12;
  tuni_nut_set_row(gb);
}

void tuniNut_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7c0b, 0x7c0d); E = 0x45;
  CYC(0x7c0d, 0x7c0e); A = mem_rd(gb, DE);
  CYC(0x7c0e, 0x7c0f); push_effect(gb, 0x7c0f);
  switch (tuni_nut_jump_table(gb)) {
    case 0x7c1b:
      CALL_C(0x7c1b, interactionDecCounter1_hook, 0x23cc, 0x7c1e);
      if (!(F & FZ)) {
        CYCT(0x7c1e, 0x7c1f); ret_effect(gb);
        return;
      }
      CYC(0x7c1e, 0x7c1f);
      CYC(0x7c1f, 0x7c21); mem_wr(gb, HL, 0x10);
      CYC(0x7c21, 0x7c24); interactionIncSubstate_hook(gb);
      return;
    case 0x7c24:
      CYC(0x7c24, 0x7c27); A = mem_rd(gb, wFrameCounter);
      CYC(0x7c27, 0x7c28); alu_rrca(gb);
      if (F & FC) {
        CYCT(0x7c28, 0x7c29); ret_effect(gb);
        return;
      }
      CYC(0x7c28, 0x7c29);
      CYC(0x7c29, 0x7c2a); H = D;
      CYC(0x7c2a, 0x7c2c); L = 0x4f;
      CYC(0x7c2c, 0x7c2d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
      CALL_C(0x7c2d, interactionDecCounter1_hook, 0x23cc, 0x7c30);
      if (!(F & FZ)) {
        CYCT(0x7c30, 0x7c31); ret_effect(gb);
        return;
      }
      CYC(0x7c30, 0x7c31);
      CALL_C(0x7c31, objectCenterOnTile_hook, 0x20db, 0x7c34);
      CYC(0x7c34, 0x7c37); interactionIncSubstate_hook(gb);
      return;
    case 0x7c37:
      CYC(0x7c37, 0x7c39); B = 0x0a;
      CYC(0x7c39, 0x7c3b); C = 0x00;
      CYC(0x7c3b, 0x7c3d); E = 0x49;
      CALL_C(0x7c3d, objectApplyGivenSpeed_hook, 0x2029, 0x7c40);
      CYC(0x7c40, 0x7c42); E = 0x4b;
      CYC(0x7c42, 0x7c43); A = mem_rd(gb, DE);
      CYC(0x7c43, 0x7c45); alu_cp(gb, 0x18);
      if (!(F & FC)) {
        CYCT(0x7c45, 0x7c46); ret_effect(gb);
        return;
      }
      CYC(0x7c45, 0x7c46);
      CALL_C(0x7c46, objectCenterOnTile_hook, 0x20db, 0x7c49);
      CYC(0x7c49, 0x7c4c); interactionIncSubstate_hook(gb);
      return;
    case 0x7c4c:
      CYC(0x7c4c, 0x7c4e); C = 0x20;
      CALL_C(0x7c4e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7c51);
      if (!(F & FZ)) {
        CYCT(0x7c51, 0x7c52); ret_effect(gb);
        return;
      }
      CYC(0x7c51, 0x7c52);
      CYC(0x7c52, 0x7c54); A = 0x77;
      CALL_C(0x7c54, playSound_b00_hook, 0x0c98, 0x7c57);
      CYC(0x7c57, 0x7c59); E = 0x46;
      CYC(0x7c59, 0x7c5b); A = 0x5a;
      CYC(0x7c5b, 0x7c5c); mem_wr(gb, DE, A);
      CYC(0x7c5c, 0x7c5e); A = 0x5b;
      CALL_C(0x7c5e, playSound_b00_hook, 0x0c98, 0x7c61);
      CYC(0x7c61, 0x7c64); interactionIncSubstate_hook(gb);
      return;
    case 0x7c64:
      CALL_C(0x7c64, interactionDecCounter1_hook, 0x23cc, 0x7c67);
      if (!(F & FZ)) {
        CYCT(0x7c67, 0x7c68); ret_effect(gb);
        return;
      }
      CYC(0x7c67, 0x7c68);
      CALL_C(0x7c68, brightenRoom_hook, 0x3350, 0x7c6b);
      CYC(0x7c6b, 0x7c6e); interactionIncSubstate_hook(gb);
      return;
    case 0x7c6e:
      CYC(0x7c6e, 0x7c71); A = mem_rd(gb, wPaletteThread_mode);
      CYC(0x7c71, 0x7c72); alu_or(gb, A);
      if (!(F & FZ)) {
        CYCT(0x7c72, 0x7c73); ret_effect(gb);
        return;
      }
      CYC(0x7c72, 0x7c73);
      CYC(0x7c73, 0x7c75); A = 0x29;
      CALL_C(0x7c75, setGlobalFlag_hook, 0x31f9, 0x7c78);
      CYC(0x7c78, 0x7c7a); A = 0x4c;
      CALL_C(0x7c7a, loseTreasure_hook, 0x1733, 0x7c7d);
      CYC(0x7c7d, 0x7c80); push_effect(gb, 0x7c80);
      tuni_nut_set_room_flags(gb);
      CYC(0x7c80, 0x7c81); alu_xor(gb, A);
      CYC(0x7c81, 0x7c84); mem_wr(gb, wDisabledObjects, A);
      CYC(0x7c84, 0x7c87); mem_wr(gb, wMenuDisabled, A);
      CYC(0x7c87, 0x7c8a); SET_HL(wTmpcfc0_genericCutscene_state);
      CYC(0x7c8a, 0x7c8c); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
      CYC(0x7c8c, 0x7c8f); A = mem_rd(gb, wActiveMusic);
      CALL_C(0x7c8f, playSound_b00_hook, 0x0c98, 0x7c92);
      CYC(0x7c92, 0x7c95);
      tuniNut_gotoState4_hook(gb);
      return;
    default:
      hook_continue(gb, HL, sp0_);
      return;
  }
}

void interactionCodeb1_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b60, 0x7b62); E = 0x44;
  CYC(0x7b62, 0x7b63); A = mem_rd(gb, DE);
  CYC(0x7b63, 0x7b64); push_effect(gb, 0x7b64);
  switch (tuni_nut_jump_table(gb)) {
    case 0x7b6e: tuniNut_state0_hook(gb); return;
    case 0x7ba1: tuniNut_state1_hook(gb); return;
    case 0x7be4: tuniNut_state2_hook(gb); return;
    case 0x7c0b: tuniNut_state3_hook(gb); return;
    case 0x2680: objectPreventLinkFromPassing_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
