#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

void warpTransition0_hook(GB *gb);
void warpTransitionA_hook(GB *gb);
void warpTransition1_hook(GB *gb);
void warpUpdateRespawnPoint_hook(GB *gb);
void warpTransition_setLinkFacingDir_hook(GB *gb);
void warpTransition5_00_hook(GB *gb);
void warpTransition5_01_hook(GB *gb);
void warpTransition5_02_hook(GB *gb);
void linkIncrementDirectionOnOddFrames_hook(GB *gb);
void linkIncrementDirection_hook(GB *gb);

static uint16_t link_jump_table(GB *gb) {
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

void warpTransition0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a4c, warpTransition_setLinkFacingDir_hook, 0x4a77, 0x4a4f);
  warpTransitionA_hook(gb);
}

void warpTransitionA_hook(GB *gb) {
  CYC(0x4a4f, 0x4a52);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransitionE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a52, objectCenterOnTile_hook, 0x20db, 0x4a55);
  CYC(0x4a55, 0x4a56); A = mem_rd(gb, HL);
  CYC(0x4a56, 0x4a58); alu_and(gb, 0xf0);
  CYC(0x4a58, 0x4a59); mem_wr(gb, HL, A);
  warpTransition1_hook(gb);
}

void warpTransition1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a59, warpTransition_setLinkFacingDir_hook, 0x4a77, 0x4a5c);
  warpUpdateRespawnPoint_hook(gb);
}

void warpUpdateRespawnPoint_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a5c, 0x4a5f); A = W8(wActiveGroup);
  CYC(0x4a5f, 0x4a61); alu_cp(gb, 0x06);
  if (!(F & FC)) {
    CYCT(0x4a61, 0x4a63);
    warpTransition0_hook(gb);
    return;
  }
  CYC(0x4a61, 0x4a63);
  CALL_C(0x4a63, setDeathRespawnPoint_hook, 0x1100, 0x4a66);
  CALL_C(0x4a66, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4a69);
  CYC(0x4a69, 0x4a6c);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransitionC_hook(GB *gb) {
  CYC(0x4a6c, 0x4a6f); A = W8(wcc50);
  CYC(0x4a6f, 0x4a71); alu_and(gb, 0x03);
  CYC(0x4a71, 0x4a73); E = (uint8_t)w1Link_direction;
  CYC(0x4a73, 0x4a74); mem_wr(gb, DE, A);
  CYC(0x4a74, 0x4a77);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransition_setLinkFacingDir_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a77, objectGetTileAtPosition_hook, 0x1444, 0x4a7a);
  CYC(0x4a7a, 0x4a7d); SET_HL(0x4a88);
  CALL_C(0x4a7d, lookupCollisionTable_hook, 0x1e1f, 0x4a80);
  if (F & FC) {
    CYCT(0x4a80, 0x4a82);
  } else {
    CYC(0x4a80, 0x4a82);
    CYC(0x4a82, 0x4a84); A = 0x02;
  }
  CYC(0x4a84, 0x4a86); E = (uint8_t)w1Link_direction;
  CYC(0x4a86, 0x4a87); mem_wr(gb, DE, A);
  CYC(0x4a87, 0x4a88); ret_effect(gb);
}

void warpTransition2_hook(GB *gb) {
  CYC(0x4a9b, 0x4a9d); A = 0x03;
  CYC(0x4a9d, 0x4aa0); W8(wWarpTransition2) = A;
  CYC(0x4aa0, 0x4aa2); A = 0x6e;
  CYC(0x4aa2, 0x4aa5);
  playSound_b00_hook(gb);
}

void warpTransition4_hook(GB *gb) {
  CYC(0x4b68, 0x4b6b); A = W8(wWarpTransition);
  CYC(0x4b6b, 0x4b6c); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x4b6c, 0x4b6f);
    warpTransition0_hook(gb);
    return;
  }
  CYC(0x4b6c, 0x4b6f);
  CYC(0x4b6f, 0x4b71); A = 0x01;
  CYC(0x4b71, 0x4b74); W8(wWarpTransition2) = A;
  CYC(0x4b74, 0x4b76); A = 0x6e;
  CYC(0x4b76, 0x4b79);
  playSound_b00_hook(gb);
}

void warpTransition5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b79, 0x4b7b); E = 0x05;
  CYC(0x4b7b, 0x4b7c); A = mem_rd(gb, DE);
  CYC(0x4b7c, 0x4b7d); push_effect(gb, 0x4b7d);
  switch (link_jump_table(gb)) {
    case 0x4b83: warpTransition5_00_hook(gb); return;
    case 0x4ba1: warpTransition5_01_hook(gb); return;
    case 0x4c19: warpTransition5_02_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void warpTransition5_00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b83, 0x4b85); A = 0x01;
  CYC(0x4b85, 0x4b86); mem_wr(gb, DE, A);
  CYC(0x4b86, 0x4b89); SET_BC(0x0020);
  CALL_C(0x4b89, objectSetSpeedZ_hook, 0x239d, 0x4b8c);
  CALL_C(0x4b8c, objectGetZAboveScreen_hook, 0x2172, 0x4b8f);
  CYC(0x4b8f, 0x4b91); L = (uint8_t)w1Link_zh;
  CYC(0x4b91, 0x4b92); mem_wr(gb, HL, A);
  CYC(0x4b92, 0x4b94); L = (uint8_t)w1Link_yh;
  CYC(0x4b94, 0x4b95); A = mem_rd(gb, HL);
  CYC(0x4b95, 0x4b97); alu_sub(gb, 0x04);
  CYC(0x4b97, 0x4b98); mem_wr(gb, HL, A);
  CYC(0x4b98, 0x4b9a); L = (uint8_t)w1Link_direction;
  CYC(0x4b9a, 0x4b9c); mem_wr(gb, HL, 0x02);
  CYC(0x4b9c, 0x4b9e); A = 0x04;
  CYC(0x4b9e, 0x4ba1);
  specialObjectSetAnimation_hook(gb);
}

static void warp_transition7_link_collapsed(GB *gb, uint16_t sp0_) {
  CALL_C(0x4c05, itemIncSubstate_hook, 0x23ef, 0x4c08);
  CYC(0x4c08, 0x4c0a); L = (uint8_t)w1Link_counter1;
  CYC(0x4c0a, 0x4c0c); mem_wr(gb, HL, 0x1e);
  CYC(0x4c0c, 0x4c0e); A = 0x02;
  CALL_C(0x4c0e, specialObjectSetAnimation_hook, 0x2b0a, 0x4c11);
  CYC(0x4c11, 0x4c13); A = 0x87;
  CYC(0x4c13, 0x4c16);
  playSound_b00_hook(gb);
}

void warpTransition5_01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ba1, specialObjectAnimate_hook, 0x2aef, 0x4ba4);
  CYC(0x4ba4, 0x4ba6); C = 0x20;
  CALL_C(0x4ba6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4ba9);
  if (!(F & FZ)) { CYCT(0x4ba9, 0x4baa); ret_effect(gb); return; }
  CYC(0x4ba9, 0x4baa);
  CYC(0x4baa, 0x4bad); SET_HL(0x2408);
  CALL_C(0x4bad, lookupCollisionTable_hook, 0x1e1f, 0x4bb0);
  if (!(F & FC)) {
    CYCT(0x4bb0, 0x4bb3);
    warp_transition7_link_collapsed(gb, sp0_);
    return;
  }
  CYC(0x4bb0, 0x4bb3);
  CYC(0x4bb3, 0x4bb6);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransition7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4bb6, 0x4bb8); E = 0x05;
  CYC(0x4bb8, 0x4bb9); A = mem_rd(gb, DE);
  CYC(0x4bb9, 0x4bba); push_effect(gb, 0x4bba);
  switch (link_jump_table(gb)) {
    case 0x4bc2: goto state0;
    case 0x4be3: goto state1;
    case 0x4bfc: goto state2;
    case 0x4c16: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4bc2, 0x4bc4); A = 0x01;
  CYC(0x4bc4, 0x4bc5); mem_wr(gb, DE, A);
  CYC(0x4bc5, 0x4bc6); H = D;
  CYC(0x4bc6, 0x4bc8); L = (uint8_t)w1Link_direction;
  CYC(0x4bc8, 0x4bca); mem_wr(gb, HL, 0x02);
  CYC(0x4bca, 0x4bcb); L = alu_inc8(gb, L);
  CYC(0x4bcb, 0x4bcd); mem_wr(gb, HL, 0x10);
  CYC(0x4bcd, 0x4bcf); L = (uint8_t)w1Link_speed;
  CYC(0x4bcf, 0x4bd1); mem_wr(gb, HL, 0x28);
  CYC(0x4bd1, 0x4bd3); L = (uint8_t)w1Link_visible;
  CYC(0x4bd3, 0x4bd5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4bd5, 0x4bd7); L = (uint8_t)w1Link_counter1;
  CYC(0x4bd7, 0x4bd9); mem_wr(gb, HL, 0x78);
  CYC(0x4bd9, 0x4bdb); A = 0x04;
  CALL_C(0x4bdb, specialObjectSetAnimation_hook, 0x2b0a, 0x4bde);
  CYC(0x4bde, 0x4be0); A = 0x65;
  CYC(0x4be0, 0x4be3);
  playSound_b00_hook(gb);
  return;

state1:
  CALL_C(0x4be3, itemDecCounter1_hook, 0x23d6, 0x4be6);
  if (!(F & FZ)) { CYCT(0x4be6, 0x4be7); ret_effect(gb); return; }
  CYC(0x4be6, 0x4be7);
  CYC(0x4be7, 0x4be9); L = 0x05;
  CYC(0x4be9, 0x4bea); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4bea, 0x4bec); L = (uint8_t)w1Link_visible;
  CYC(0x4bec, 0x4bee); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x4bee, 0x4bf0); L = (uint8_t)w1Link_counter1;
  CYC(0x4bf0, 0x4bf2); mem_wr(gb, HL, 0x30);
  CYC(0x4bf2, 0x4bf4); A = 0x10;
  CALL_C(0x4bf4, setScreenShakeCounter_hook, 0x24bb, 0x4bf7);
  CYC(0x4bf7, 0x4bf9); A = 0x85;
  CYC(0x4bf9, 0x4bfc);
  playSound_b00_hook(gb);
  return;

state2:
  CALL_C(0x4bfc, specialObjectAnimate_hook, 0x2aef, 0x4bff);
  CALL_C(0x4bff, itemDecCounter1_hook, 0x23d6, 0x4c02);
  if (!(F & FZ)) {
    CYCT(0x4c02, 0x4c05);
    specialObjectUpdatePosition(gb);
    return;
  }
  CYC(0x4c02, 0x4c05);
  warp_transition7_link_collapsed(gb, sp0_);
  return;

state3:
  CALL_C(0x4c16, setDeathRespawnPoint_hook, 0x1100, 0x4c19);
  warpTransition5_02_hook(gb);
}

void warpTransition5_02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4c19, itemDecCounter1_hook, 0x23d6, 0x4c1c);
  if (!(F & FZ)) { CYCT(0x4c1c, 0x4c1d); ret_effect(gb); return; }
  CYC(0x4c1c, 0x4c1d);
  CYC(0x4c1d, 0x4c20);
  initLinkStateAndAnimateStanding(gb);
}

void linkIncrementDirectionOnOddFrames_hook(GB *gb) {
  CYC(0x4c20, 0x4c23); A = W8(wFrameCounter);
  CYC(0x4c23, 0x4c24); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4c24, 0x4c25); ret_effect(gb); return; }
  CYC(0x4c24, 0x4c25);
  linkIncrementDirection_hook(gb);
}

void linkIncrementDirection_hook(GB *gb) {
  CYC(0x4c25, 0x4c27); E = (uint8_t)w1Link_direction;
  CYC(0x4c27, 0x4c28); A = mem_rd(gb, DE);
  CYC(0x4c28, 0x4c29); A = alu_inc8(gb, A);
  CYC(0x4c29, 0x4c2b); alu_and(gb, 0x03);
  CYC(0x4c2b, 0x4c2c); mem_wr(gb, DE, A);
  CYC(0x4c2c, 0x4c2d); ret_effect(gb);
}

void warpTransition8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c2d, 0x4c2f); E = 0x05;
  CYC(0x4c2f, 0x4c30); A = mem_rd(gb, DE);
  CYC(0x4c30, 0x4c31); push_effect(gb, 0x4c31);
  switch (link_jump_table(gb)) {
    case 0x4c41: goto state0;
    case 0x4c6b: goto state1;
    case 0x4c8a: goto state2;
    case 0x4ca4: goto state3;
    case 0x4cb2: goto state4;
    case 0x4cc3: goto state5;
    case 0x4cd2: goto state6;
    case 0x4cec: goto state7;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4c41, 0x4c43); A = 0x01;
  CYC(0x4c43, 0x4c44); mem_wr(gb, DE, A);
  CYC(0x4c44, 0x4c46); A = 0xff;
  CYC(0x4c46, 0x4c49); W8(wDisabledObjects) = A;
  CYC(0x4c49, 0x4c4b); A = 0x80;
  CYC(0x4c4b, 0x4c4e); W8(wMenuDisabled) = A;
  CYC(0x4c4e, 0x4c50); A = 0x15;
  CYC(0x4c50, 0x4c53); W8(wCutsceneTrigger) = A;
  CYC(0x4c53, 0x4c56); SET_BC(0xff60);
  CALL_C(0x4c56, objectSetSpeedZ_hook, 0x239d, 0x4c59);
  CYC(0x4c59, 0x4c5b); L = 0x06;
  CYC(0x4c5b, 0x4c5d); mem_wr(gb, HL, 0x30);
  CALL_C(0x4c5d, linkCancelAllItemUsage, 0x4f49, 0x4c60);
  CALL_C(0x4c60, restartSound_hook, 0x0cb2, 0x4c63);
  CYC(0x4c63, 0x4c65); A = 0xb4;
  CALL_C(0x4c65, playSound_b00_hook, 0x0c98, 0x4c68);
  CYC(0x4c68, 0x4c6b);
  objectCenterOnTile_hook(gb);
  return;

state1:
  CYC(0x4c6b, 0x4c6d); C = 0x02;
  CALL_C(0x4c6d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4c70);
  CYC(0x4c70, 0x4c73); A = W8(wFrameCounter);
  CYC(0x4c73, 0x4c75); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x4c75, 0x4c77);
  } else {
    CYC(0x4c75, 0x4c77);
    CYC(0x4c77, 0x4c7a); SET_HL(wTmpcbbc);
    CYC(0x4c7a, 0x4c7b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(0x4c7b, 0x4c7e); A = W8(wFrameCounter);
  CYC(0x4c7e, 0x4c80); alu_and(gb, 0x03);
  if (F & FZ) CALL_C_CC(0x4c80, linkIncrementDirection_hook, 0x4c25, 0x4c83);
  else CYC(0x4c80, 0x4c83);
  CALL_C(0x4c83, itemDecCounter1_hook, 0x23d6, 0x4c86);
  if (!(F & FZ)) { CYCT(0x4c86, 0x4c87); ret_effect(gb); return; }
  CYC(0x4c86, 0x4c87);
  CYC(0x4c87, 0x4c8a);
  itemIncSubstate_hook(gb);
  return;

state2:
  CYC(0x4c8a, 0x4c8c); C = 0x02;
  CALL_C(0x4c8c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4c8f);
  CALL_C(0x4c8f, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4c92);
  CYC(0x4c92, 0x4c93); H = D;
  CYC(0x4c93, 0x4c95); L = 0x15;
  CYC(0x4c95, 0x4c97); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4c97, 0x4c98); ret_effect(gb); return; }
  CYC(0x4c97, 0x4c98);
  CYC(0x4c98, 0x4c9a); L = 0x06;
  CYC(0x4c9a, 0x4c9c); mem_wr(gb, HL, 0x28);
  CYC(0x4c9c, 0x4c9e); A = 0x02;
  CALL_C(0x4c9e, fadeoutToWhiteWithDelay_hook, 0x3257, 0x4ca1);
  CYC(0x4ca1, 0x4ca4);
  itemIncSubstate_hook(gb);
  return;

state3:
  CALL_C(0x4ca4, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4ca7);
  CALL_C(0x4ca7, itemDecCounter1_hook, 0x23d6, 0x4caa);
  if (!(F & FZ)) { CYCT(0x4caa, 0x4cab); ret_effect(gb); return; }
  CYC(0x4caa, 0x4cab);
  CYC(0x4cab, 0x4cae); SET_HL(wTmpcbb3);
  CYC(0x4cae, 0x4caf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4caf, 0x4cb2);
  itemIncSubstate_hook(gb);
  return;

state4:
  CALL_C(0x4cb2, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4cb5);
  CYC(0x4cb5, 0x4cb8); A = W8(wCutsceneState);
  CYC(0x4cb8, 0x4cba); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4cba, 0x4cbb); ret_effect(gb); return; }
  CYC(0x4cba, 0x4cbb);
  CALL_C(0x4cbb, itemIncSubstate_hook, 0x23ef, 0x4cbe);
  CYC(0x4cbe, 0x4cc0); L = 0x06;
  CYC(0x4cc0, 0x4cc2); mem_wr(gb, HL, 0x28);
  CYC(0x4cc2, 0x4cc3); ret_effect(gb);
  return;

state5:
  CYC(0x4cc3, 0x4cc5); C = 0x02;
  CALL_C(0x4cc5, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4cc8);
  CALL_C(0x4cc8, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4ccb);
  CALL_C(0x4ccb, itemDecCounter1_hook, 0x23d6, 0x4cce);
  if (!(F & FZ)) { CYCT(0x4cce, 0x4ccf); ret_effect(gb); return; }
  CYC(0x4cce, 0x4ccf);
  CYC(0x4ccf, 0x4cd2);
  itemIncSubstate_hook(gb);
  return;

state6:
  CYC(0x4cd2, 0x4cd4); C = 0x02;
  CALL_C(0x4cd4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4cd7);
  CYC(0x4cd7, 0x4cda); A = W8(wFrameCounter);
  CYC(0x4cda, 0x4cdc); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x4cdc, 0x4cdd); ret_effect(gb); return; }
  CYC(0x4cdc, 0x4cdd);
  CALL_C(0x4cdd, linkIncrementDirection_hook, 0x4c25, 0x4ce0);
  CYC(0x4ce0, 0x4ce3); SET_HL(wTmpcbbc);
  CYC(0x4ce3, 0x4ce4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x4ce4, 0x4ce5); ret_effect(gb); return; }
  CYC(0x4ce4, 0x4ce5);
  CYC(0x4ce5, 0x4ce8); SET_HL(wTmpcbb3);
  CYC(0x4ce8, 0x4ce9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4ce9, 0x4cec);
  itemIncSubstate_hook(gb);
  return;

state7:
  CYC(0x4cec, 0x4cef); A = W8(wDisabledObjects);
  CYC(0x4cef, 0x4cf1); alu_and(gb, 0x81);
  if (F & FZ) {
    CYCT(0x4cf1, 0x4cf3);
    goto finish;
  }
  CYC(0x4cf1, 0x4cf3);
  CYC(0x4cf3, 0x4cf6); A = W8(wFrameCounter);
  CYC(0x4cf6, 0x4cf8); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x4cf8, 0x4cf9); ret_effect(gb); return; }
  CYC(0x4cf8, 0x4cf9);
  CYC(0x4cf9, 0x4cfc);
  linkIncrementDirection_hook(gb);
  return;

finish:
  CYC(0x4cfc, 0x4cfe); E = (uint8_t)w1Link_direction;
  CYC(0x4cfe, 0x4cff); A = mem_rd(gb, DE);
  CYC(0x4cff, 0x4d01); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x4d01, 0x4d04);
    linkIncrementDirection_hook(gb);
    return;
  }
  CYC(0x4d01, 0x4d04);
  CYC(0x4d04, 0x4d07); A = W8(wActiveMusic2);
  CYC(0x4d07, 0x4d0a); W8(wActiveMusic) = A;
  CALL_C(0x4d0a, playSound_b00_hook, 0x0c98, 0x4d0d);
  CALL_C(0x4d0d, setDeathRespawnPoint_hook, 0x1100, 0x4d10);
  CALL_C(0x4d10, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4d13);
  CALL_C(0x4d13, resetLinkInvincibility_hook, 0x2ba9, 0x4d16);
  CYC(0x4d16, 0x4d19);
  initLinkStateAndAnimateStanding(gb);
}
