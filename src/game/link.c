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
void warpTransition9_hook(GB *gb);
void warpTransitionB_hook(GB *gb);
void warpTransitionF_hook(GB *gb);
void warpTransition6_hook(GB *gb);
void warpTransition3_hook(GB *gb);
void linkState01_hook(GB *gb);
void linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook(GB *gb);
void linkCancelAllItemUsage_hook(GB *gb);
void linkState0e_hook(GB *gb);
void linkState0f_hook(GB *gb);
void linkState0b_hook(GB *gb);
void setLinkStateToDead_hook(GB *gb);
void linkState03_hook(GB *gb);
void specialObjectCode_link_hook(GB *gb);
void linkState02_hook(GB *gb);
void linkState0c_hook(GB *gb);
void linkState13_hook(GB *gb);
void linkState14_hook(GB *gb);
void linkState0d_hook(GB *gb);
void linkState05_hook(GB *gb);
void linkState06_hook(GB *gb);
void linkState09_hook(GB *gb);
void linkState11_hook(GB *gb);

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
  CALL_C(0x4c5d, linkCancelAllItemUsage_hook, 0x4f49, 0x4c60);
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

void linkState0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a26, 0x4a29); A = W8(wWarpTransition);
  CYC(0x4a29, 0x4a2b); alu_and(gb, 0x0f);
  CYC(0x4a2b, 0x4a2c); push_effect(gb, 0x4a2c);
  switch (link_jump_table(gb)) {
    case 0x4a4c: warpTransition0_hook(gb); return;
    case 0x4a59: warpTransition1_hook(gb); return;
    case 0x4a9b: warpTransition2_hook(gb); return;
    case 0x4aa5: warpTransition3_hook(gb); return;
    case 0x4b68: warpTransition4_hook(gb); return;
    case 0x4b79: warpTransition5_hook(gb); return;
    case 0x4d89: warpTransition6_hook(gb); return;
    case 0x4a4f: warpTransitionA_hook(gb); return;
    case 0x4c2d: warpTransition8_hook(gb); return;
    case 0x4d19: warpTransition9_hook(gb); return;
    case 0x4d47: warpTransitionB_hook(gb); return;
    case 0x4a6c: warpTransitionC_hook(gb); return;
    case 0x4a52: warpTransitionE_hook(gb); return;
    case 0x4d83: warpTransitionF_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void linkState00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49e4, clearAllParentItems_hook, 0x2c10, 0x49e7);
  CALL_C(0x49e7, specialObjectSetOamVariables_hook, 0x41f7, 0x49ea);
  CYC(0x49ea, 0x49ec); A = 0x10;
  CALL_C(0x49ec, specialObjectSetAnimation_hook, 0x2b0a, 0x49ef);
  CYC(0x49ef, 0x49f0); H = D;
  CYC(0x49f0, 0x49f2); L = 0x24;
  CYC(0x49f2, 0x49f4); A = 0x80;
  CYC(0x49f4, 0x49f5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49f5, 0x49f6); L = alu_inc8(gb, L);
  CYC(0x49f6, 0x49f8); A = 0x06;
  CYC(0x49f8, 0x49f9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49f9, 0x49fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49fa, 0x49fc); L = 0x29;
  CYC(0x49fc, 0x49fe); mem_wr(gb, HL, 0x01);
  CYC(0x49fe, 0x4a01); A = W8(wLinkForceState);
  CYC(0x4a01, 0x4a03); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x4a03, 0x4a05);
  } else {
    CYC(0x4a03, 0x4a05);
    CYC(0x4a05, 0x4a08); A = W8(wDisableRingTransformations);
    CYC(0x4a08, 0x4a09); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(0x4a09, 0x4a0b);
    } else {
      CYC(0x4a09, 0x4a0b);
      CALL_C(0x4a0b, objectGetTileCollisions_hook, 0x14ad, 0x4a0e);
      CYC(0x4a0e, 0x4a10); alu_cp(gb, 0x0f);
      if (!(F & FZ)) {
        CYCT(0x4a10, 0x4a12);
      } else {
        CYC(0x4a10, 0x4a12);
        CYC(0x4a12, 0x4a15); SET_HL(wLastAnimalMountPointY);
        CYC(0x4a15, 0x4a16); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x4a16, 0x4a18); E = (uint8_t)w1Link_yh;
        CYC(0x4a18, 0x4a19); mem_wr(gb, DE, A);
        CYC(0x4a19, 0x4a1a); A = mem_rd(gb, HL);
        CYC(0x4a1a, 0x4a1c); E = (uint8_t)w1Link_xh;
        CYC(0x4a1c, 0x4a1d); mem_wr(gb, DE, A);
      }
    }
  }
  CALL_C(0x4a1d, objectSetVisiblec1_hook, 0x1e3c, 0x4a20);
  CALL_C(0x4a20, checkLinkForceState, 0x54c0, 0x4a23);
  CYC(0x4a23, 0x4a26);
  initLinkStateAndAnimateStanding(gb);
}

void linkState08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f0c, 0x4f0e); E = 0x05;
  CYC(0x4f0e, 0x4f0f); A = mem_rd(gb, DE);
  CYC(0x4f0f, 0x4f10); push_effect(gb, 0x4f10);
  switch (link_jump_table(gb)) {
    case 0x4f14: goto substate0;
    case 0x4f27: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4f14, 0x4f16); A = 0x01;
  CYC(0x4f16, 0x4f17); mem_wr(gb, DE, A);
  CYC(0x4f17, 0x4f1a); SET_HL(wcc50);
  CYC(0x4f1a, 0x4f1b); A = mem_rd(gb, HL);
  CYC(0x4f1b, 0x4f1d); mem_wr(gb, HL, 0x00);
  CYC(0x4f1d, 0x4f1e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f1e, 0x4f1f); ret_effect(gb); return; }
  CYC(0x4f1e, 0x4f1f);
  CALL_C(0x4f1f, linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook, 0x4f45, 0x4f22);
  CYC(0x4f22, 0x4f24); A = 0x10;
  CYC(0x4f24, 0x4f27);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x4f27, checkLinkForceState, 0x54c0, 0x4f2a);
  CYC(0x4f2a, 0x4f2d); SET_HL(wcc50);
  CYC(0x4f2d, 0x4f2e); A = mem_rd(gb, HL);
  CYC(0x4f2e, 0x4f2f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(0x4f2f, 0x4f31);
    CYC(0x4f31, 0x4f33); mem_wr(gb, HL, 0x00);
    CALL_C(0x4f33, specialObjectSetAnimation_hook, 0x2b0a, 0x4f36);
  } else {
    CYCT(0x4f2f, 0x4f31);
  }
  CYC(0x4f36, 0x4f39); A = W8(wcc63);
  CYC(0x4f39, 0x4f3a); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x4f3a, checkUseItems_b00_hook, 0x2c18, 0x4f3d);
  else CYC(0x4f3a, 0x4f3d);
  CYC(0x4f3d, 0x4f40); A = W8(wDisabledObjects);
  CYC(0x4f40, 0x4f41); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f41, 0x4f42); ret_effect(gb); return; }
  CYC(0x4f41, 0x4f42);
  CYC(0x4f42, 0x4f45);
  initLinkStateAndAnimateStanding(gb);
}

void linkState04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ff6, 0x4ff8); E = 0x05;
  CYC(0x4ff8, 0x4ff9); A = mem_rd(gb, DE);
  CYC(0x4ff9, 0x4ffa); push_effect(gb, 0x4ffa);
  switch (link_jump_table(gb)) {
    case 0x4ffe: goto substate0;
    case 0x5014: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4ffe, 0x5000); A = 0x01;
  CYC(0x5000, 0x5001); mem_wr(gb, DE, A);
  CALL_C(0x5001, linkCancelAllItemUsage_hook, 0x4f49, 0x5004);
  CYC(0x5004, 0x5006); E = 0x30;
  CYC(0x5006, 0x5007); A = mem_rd(gb, DE);
  CYC(0x5007, 0x500a); W8(wcc52) = A;
  CYC(0x500a, 0x500d); A = W8(wcc50);
  CYC(0x500d, 0x500f); alu_and(gb, 0x0f);
  CYC(0x500f, 0x5011); alu_add(gb, 0x0e);
  CYC(0x5011, 0x5014);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x5014, retIfTextIsActive_hook, 0x1859, 0x5017);
  CYC(0x5017, 0x501a); A = W8(wcc50);
  CYC(0x501a, 0x501b); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x501b, 0x501d);
  } else {
    CYC(0x501b, 0x501d);
    CYC(0x501d, 0x5020); A = W8(wDisabledObjects);
    CYC(0x5020, 0x5022); alu_and(gb, 0x81);
    if (!(F & FZ)) { CYCT(0x5022, 0x5023); ret_effect(gb); return; }
    CYC(0x5022, 0x5023);
  }
  CYC(0x5023, 0x5025); E = 0x04;
  CYC(0x5025, 0x5027); A = 0x01;
  CYC(0x5027, 0x5028); mem_wr(gb, DE, A);
  CYC(0x5028, 0x502b); A = W8(wcc52);
  CYC(0x502b, 0x502e);
  specialObjectSetAnimation_hook(gb);
}

void warpTransition9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d19, 0x4d1b); E = 0x05;
  CYC(0x4d1b, 0x4d1c); A = mem_rd(gb, DE);
  CYC(0x4d1c, 0x4d1d); push_effect(gb, 0x4d1d);
  switch (link_jump_table(gb)) {
    case 0x4d21: goto substate0;
    case 0x4d3a: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4d21, itemIncSubstate_hook, 0x23ef, 0x4d24);
  CYC(0x4d24, 0x4d26); L = 0x0b;
  CYC(0x4d26, 0x4d28); A = 0x08;
  CYC(0x4d28, 0x4d29); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4d29, 0x4d2a); mem_wr(gb, HL, A);
  CALL_C(0x4d2a, objectCenterOnTile_hook, 0x20db, 0x4d2d);
  CALL_C(0x4d2d, clearAllParentItems_hook, 0x2c10, 0x4d30);
  CYC(0x4d30, 0x4d32); A = 0x0d;
  CALL_C(0x4d32, specialObjectSetAnimation_hook, 0x2b0a, 0x4d35);
  CYC(0x4d35, 0x4d37); A = 0x65;
  CYC(0x4d37, 0x4d3a);
  playSound_b00_hook(gb);
  return;

substate1:
  CYC(0x4d3a, 0x4d3c); E = 0x21;
  CYC(0x4d3c, 0x4d3d); A = mem_rd(gb, DE);
  CYC(0x4d3d, 0x4d3e); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x4d3e, 0x4d41); specialObjectAnimate_hook(gb); return; }
  CYC(0x4d3e, 0x4d41);
  CYC(0x4d41, 0x4d43); A = 0x03;
  CYC(0x4d43, 0x4d46); W8(wWarpTransition2) = A;
  CYC(0x4d46, 0x4d47); ret_effect(gb);
}

void warpTransitionB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d47, 0x4d49); E = 0x05;
  CYC(0x4d49, 0x4d4a); A = mem_rd(gb, DE);
  CYC(0x4d4a, 0x4d4b); push_effect(gb, 0x4d4b);
  switch (link_jump_table(gb)) {
    case 0x4d51: goto state0;
    case 0x4d63: goto state1;
    case 0x4d77: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x4d51, itemIncSubstate_hook, 0x23ef, 0x4d54);
  CALL_C(0x4d54, objectGetZAboveScreen_hook, 0x2172, 0x4d57);
  CYC(0x4d57, 0x4d59); L = 0x0f;
  CYC(0x4d59, 0x4d5a); mem_wr(gb, HL, A);
  CYC(0x4d5a, 0x4d5c); L = 0x08;
  CYC(0x4d5c, 0x4d5e); mem_wr(gb, HL, 0x02);
  CYC(0x4d5e, 0x4d60); A = 0x04;
  CYC(0x4d60, 0x4d63);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CALL_C(0x4d63, specialObjectAnimate_hook, 0x2aef, 0x4d66);
  CYC(0x4d66, 0x4d68); C = 0x0c;
  CALL_C(0x4d68, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4d6b);
  if (!(F & FZ)) { CYCT(0x4d6b, 0x4d6c); ret_effect(gb); return; }
  CYC(0x4d6b, 0x4d6c);
  CALL_C(0x4d6c, itemIncSubstate_hook, 0x23ef, 0x4d6f);
  CALL_C(0x4d6f, animateLinkStanding, 0x5cb5, 0x4d72);
  CYC(0x4d72, 0x4d74); A = 0x87;
  CYC(0x4d74, 0x4d77);
  playSound_b00_hook(gb);
  return;

state2:
  CYC(0x4d77, 0x4d7a); A = W8(wDisabledObjects);
  CYC(0x4d7a, 0x4d7c); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(0x4d7c, 0x4d7d); ret_effect(gb); return; }
  CYC(0x4d7c, 0x4d7d);
  CALL_C(0x4d7d, objectSetVisiblec2_hook, 0x1e45, 0x4d80);
  CYC(0x4d80, 0x4d83);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransitionF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d83, checkLinkForceState, 0x54c0, 0x4d86);
  CYC(0x4d86, 0x4d89);
  objectSetInvisible_hook(gb);
}

void linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook(GB *gb) {
  CYC(0x4f45, 0x4f47); E = 0x33;
  CYC(0x4f47, 0x4f48); alu_xor(gb, A);
  CYC(0x4f48, 0x4f49); mem_wr(gb, DE, A);
  linkCancelAllItemUsage_hook(gb);
}

void linkCancelAllItemUsage_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f49, dropLinkHeldItem_hook, 0x2c43, 0x4f4c);
  CYC(0x4f4c, 0x4f4f);
  clearAllParentItems_hook(gb);
}

void linkState0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f4f, 0x4f51); E = 0x05;
  CYC(0x4f51, 0x4f52); A = mem_rd(gb, DE);
  CYC(0x4f52, 0x4f53); push_effect(gb, 0x4f53);
  switch (link_jump_table(gb)) {
    case 0x4f59: goto substate0;
    case 0x4f62: goto substate1;
    case 0x4f6c: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4f59, itemIncSubstate_hook, 0x23ef, 0x4f5c);
  CYC(0x4f5c, 0x4f5e); E = 0x37;
  CYC(0x4f5e, 0x4f61); A = W8(wActiveRoom);
  CYC(0x4f61, 0x4f62); mem_wr(gb, DE, A);

substate1:
  CALL_C(0x4f62, objectCheckWithinScreenBoundary_hook, 0x2184, 0x4f65);
  if (F & FC) { CYCT(0x4f65, 0x4f66); ret_effect(gb); return; }
  CYC(0x4f65, 0x4f66);
  CALL_C(0x4f66, itemIncSubstate_hook, 0x23ef, 0x4f69);
  CALL_C(0x4f69, objectSetInvisible_hook, 0x1e7b, 0x4f6c);

substate2:
  CYC(0x4f6c, 0x4f6d); H = D;
  CYC(0x4f6d, 0x4f6f); L = 0x37;
  CYC(0x4f6f, 0x4f72); A = W8(wActiveRoom);
  CYC(0x4f72, 0x4f73); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4f73, 0x4f74); ret_effect(gb); return; }
  CYC(0x4f73, 0x4f74);
  CALL_C(0x4f74, objectCheckWithinScreenBoundary_hook, 0x2184, 0x4f77);
  if (!(F & FC)) { CYCT(0x4f77, 0x4f78); ret_effect(gb); return; }
  CYC(0x4f77, 0x4f78);
  CYC(0x4f78, 0x4f7a); E = 0x05;
  CYC(0x4f7a, 0x4f7c); A = 0x01;
  CYC(0x4f7c, 0x4f7d); mem_wr(gb, DE, A);
  CYC(0x4f7d, 0x4f80);
  objectSetVisiblec2_hook(gb);
}

void linkState0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f80, 0x4f83); A = W8(wTextIsActive);
  CYC(0x4f83, 0x4f84); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f84, 0x4f85); ret_effect(gb); return; }
  CYC(0x4f84, 0x4f85);
  CYC(0x4f85, 0x4f87); E = 0x05;
  CYC(0x4f87, 0x4f88); A = mem_rd(gb, DE);
  CYC(0x4f88, 0x4f89); push_effect(gb, 0x4f89);
  switch (link_jump_table(gb)) {
    case 0x4f8f: goto substate0;
    case 0x4fb4: goto substate1;
    case 0x4fc0: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4f8f, itemIncSubstate_hook, 0x23ef, 0x4f92);
  CYC(0x4f92, 0x4f93); L = alu_inc8(gb, L);
  CYC(0x4f93, 0x4f95); mem_wr(gb, HL, 0x14);
  CYC(0x4f95, 0x4f97); L = 0x09;
  CYC(0x4f97, 0x4f99); mem_wr(gb, HL, 0x10);
  CYC(0x4f99, 0x4f9b); L = 0x0b;
  CYC(0x4f9b, 0x4f9d); mem_wr(gb, HL, 0x38);
  CYC(0x4f9d, 0x4f9f); L = 0x0d;
  CYC(0x4f9f, 0x4fa1); mem_wr(gb, HL, 0x50);
  CYC(0x4fa1, 0x4fa3); L = 0x10;
  CYC(0x4fa3, 0x4fa5); mem_wr(gb, HL, 0x28);
  CYC(0x4fa5, 0x4fa7); L = 0x14;
  CYC(0x4fa7, 0x4fa9); A = 0x80;
  CYC(0x4fa9, 0x4faa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4faa, 0x4fac); mem_wr(gb, HL, 0xfe);
  CYC(0x4fac, 0x4fae); A = 0x02;
  CALL_C(0x4fae, specialObjectSetAnimation_hook, 0x2b0a, 0x4fb1);
  CYC(0x4fb1, 0x4fb4);
  objectSetVisiblec2_hook(gb);
  return;

substate1:
  CALL_C(0x4fb4, objectApplySpeed_hook, 0x201d, 0x4fb7);
  CYC(0x4fb7, 0x4fb9); C = 0x20;
  CALL_C(0x4fb9, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4fbc);
  if (!(F & FC)) { CYCT(0x4fbc, 0x4fbd); ret_effect(gb); return; }
  CYC(0x4fbc, 0x4fbd);
  CYC(0x4fbd, 0x4fc0);
  itemIncSubstate_hook(gb);
  return;

substate2:
  CALL_C(0x4fc0, itemDecCounter1_hook, 0x23d6, 0x4fc3);
  if (!(F & FZ)) { CYCT(0x4fc3, 0x4fc4); ret_effect(gb); return; }
  CYC(0x4fc3, 0x4fc4);
  CYC(0x4fc4, 0x4fc7);
  initLinkStateAndAnimateStanding(gb);
}

void linkState0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fc7, 0x4fc9); E = 0x05;
  CYC(0x4fc9, 0x4fca); A = mem_rd(gb, DE);
  CYC(0x4fca, 0x4fcb); push_effect(gb, 0x4fcb);
  switch (link_jump_table(gb)) {
    case 0x4fcf: goto substate0;
    case 0x4fe6: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4fcf, 0x4fd1); A = 0x01;
  CYC(0x4fd1, 0x4fd2); mem_wr(gb, DE, A);
  CYC(0x4fd2, 0x4fd4); E = 0x06;
  CYC(0x4fd4, 0x4fd7); A = W8(wLinkStateParameter);
  CYC(0x4fd7, 0x4fd8); mem_wr(gb, DE, A);
  CALL_C(0x4fd8, clearPegasusSeedCounter_hook, 0x2a85, 0x4fdb);
  CALL_C(0x4fdb, linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook, 0x4f45, 0x4fde);
  CALL_C(0x4fde, updateLinkSpeed_standard, 0x5ce6, 0x4fe1);
  CYC(0x4fe1, 0x4fe3); A = 0x10;
  CALL_C(0x4fe3, specialObjectSetAnimation_hook, 0x2b0a, 0x4fe6);

substate1:
  CALL_C(0x4fe6, specialObjectAnimate_hook, 0x2aef, 0x4fe9);
  CALL_C(0x4fe9, itemDecCounter1_hook, 0x23d6, 0x4fec);
  CYC(0x4fec, 0x4fee); L = 0x33;
  CYC(0x4fee, 0x4ff0); mem_wr(gb, HL, 0x00);
  if (!(F & FZ)) {
    CYCT(0x4ff0, 0x4ff3);
    specialObjectUpdatePosition(gb);
    return;
  }
  CYC(0x4ff0, 0x4ff3);
  CYC(0x4ff3, 0x4ff6);
  initLinkStateAndAnimateStanding(gb);
}

void setLinkStateToDead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x502e, 0x5030); A = 0x03;
  CALL_C(0x5030, linkSetState, 0x54c9, 0x5033);
  linkState03_hook(gb);
}

void linkState03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5033, 0x5034); alu_xor(gb, A);
  CYC(0x5034, 0x5037); W8(wLinkHealth) = A;
  CYC(0x5037, 0x5039); E = 0x05;
  CYC(0x5039, 0x503a); A = mem_rd(gb, DE);
  CYC(0x503a, 0x503b); push_effect(gb, 0x503b);
  switch (link_jump_table(gb)) {
    case 0x503f: goto substate0;
    case 0x505e: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x503f, specialObjectUpdateAdjacentWallsBitset, 0x5e62, 0x5042);
  CYC(0x5042, 0x5044); E = 0x2d;
  CYC(0x5044, 0x5045); A = mem_rd(gb, DE);
  CYC(0x5045, 0x5046); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5046, 0x5049);
    linkUpdateKnockback(gb);
    return;
  }
  CYC(0x5046, 0x5049);
  CYC(0x5049, 0x504a); H = D;
  CYC(0x504a, 0x504c); L = 0x05;
  CYC(0x504c, 0x504d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x504d, 0x504f); L = 0x06;
  CYC(0x504f, 0x5051); mem_wr(gb, HL, 0x04);
  CALL_C(0x5051, linkCancelAllItemUsage_hook, 0x4f49, 0x5054);
  CYC(0x5054, 0x5056); A = 0x01;
  CALL_C(0x5056, specialObjectSetAnimation_hook, 0x2b0a, 0x5059);
  CYC(0x5059, 0x505b); A = 0x64;
  CYC(0x505b, 0x505e);
  playSound_b00_hook(gb);
  return;

substate1:
  CALL_C(0x505e, resetLinkInvincibility_hook, 0x2ba9, 0x5061);
  CALL_C(0x5061, specialObjectAnimate_hook, 0x2aef, 0x5064);
  CYC(0x5064, 0x5065); H = D;
  CYC(0x5065, 0x5067); L = 0x21;
  CYC(0x5067, 0x5068); A = mem_rd(gb, HL);
  CYC(0x5068, 0x5069); alu_add(gb, A);
  if (!(F & FZ)) { CYCT(0x5069, 0x506b); goto trigger_game_over; }
  CYC(0x5069, 0x506b);
  if (!(F & FC)) { CYCT(0x506b, 0x506c); ret_effect(gb); return; }
  CYC(0x506b, 0x506c);
  CYC(0x506c, 0x506e); L = 0x06;
  CYC(0x506e, 0x506f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x506f, 0x5070); ret_effect(gb); return; }
  CYC(0x506f, 0x5070);
  CYC(0x5070, 0x5072); A = 0x02;
  CYC(0x5072, 0x5075);
  specialObjectSetAnimation_hook(gb);
  return;

trigger_game_over:
  CYC(0x5075, 0x5077); A = 0xff;
  CYC(0x5077, 0x507a); W8(wGameOverScreenTrigger) = A;
  CYC(0x507a, 0x507b); ret_effect(gb);
}

static void warp_transition3_enter_from_middle_bottom(GB *gb, uint16_t sp0_) {
  CYC(0x4b21, 0x4b23); A = 0x01;
  CYC(0x4b23, 0x4b26); W8(wMenuDisabled) = A;
  CYC(0x4b26, 0x4b28); L = 0x06;
  CYC(0x4b28, 0x4b2a); mem_wr(gb, HL, 0x1c);
  CYC(0x4b2a, 0x4b2d); A = W8(wWarpTransition);
  CYC(0x4b2d, 0x4b2f); alu_and(gb, 0x40);
  CYC(0x4b2f, 0x4b31); A = alu_swap(gb, A);
  CYC(0x4b31, 0x4b32); B = A;
  CYC(0x4b32, 0x4b35); A = W8(wActiveGroup);
  CYC(0x4b35, 0x4b37); alu_and(gb, 0x04);
  CYC(0x4b37, 0x4b38); alu_rrca(gb);
  CYC(0x4b38, 0x4b39); alu_or(gb, B);
  CYC(0x4b39, 0x4b3c); SET_BC(0x4b60);
  CALL_C(0x4b3c, addAToBc_hook, 0x006d, 0x4b3f);
  CYC(0x4b3f, 0x4b41); L = 0x0b;
  CYC(0x4b41, 0x4b42); A = mem_rd(gb, BC);
  CYC(0x4b42, 0x4b43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b43, 0x4b44); SET_BC(BC + 1);
  CYC(0x4b44, 0x4b45); L = alu_inc8(gb, L);
  CYC(0x4b45, 0x4b46); A = mem_rd(gb, BC);
  CYC(0x4b46, 0x4b47); mem_wr(gb, HL, A);
  CYC(0x4b47, 0x4b48); ret_effect(gb);
}

void warpTransition3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4aa5, 0x4aa7); E = 0x05;
  CYC(0x4aa7, 0x4aa8); A = mem_rd(gb, DE);
  CYC(0x4aa8, 0x4aa9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4aa9, 0x4aab); goto each_frame; }
  CYC(0x4aa9, 0x4aab);
  CYC(0x4aab, 0x4aac); H = D;
  CYC(0x4aac, 0x4aad); L = E;
  CYC(0x4aad, 0x4aae); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4aae, 0x4ab0); L = 0x06;
  CYC(0x4ab0, 0x4ab2); mem_wr(gb, HL, 0x10);
  CYC(0x4ab2, 0x4ab5); A = W8(wWarpTransition);
  CYC(0x4ab5, 0x4ab7); alu_and(gb, 0x40);
  CYC(0x4ab7, 0x4ab9); A = alu_swap(gb, A);
  CYC(0x4ab9, 0x4aba); alu_rrca(gb);
  CYC(0x4aba, 0x4abd); SET_BC(0x4ad8);
  CALL_C(0x4abd, addAToBc_hook, 0x006d, 0x4ac0);
  CYC(0x4ac0, 0x4ac2); L = 0x08;
  CYC(0x4ac2, 0x4ac3); A = mem_rd(gb, BC);
  CYC(0x4ac3, 0x4ac4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ac4, 0x4ac5); SET_BC(BC + 1);
  CYC(0x4ac5, 0x4ac6); A = mem_rd(gb, BC);
  CYC(0x4ac6, 0x4ac7); mem_wr(gb, HL, A);
  CALL_C(0x4ac7, updateLinkSpeed_standard, 0x5ce6, 0x4aca);
  CALL_C(0x4aca, animateLinkStanding, 0x5cb5, 0x4acd);
  CYC(0x4acd, 0x4ad0); A = W8(wWarpTransition);
  CYC(0x4ad0, 0x4ad1); alu_rlca(gb);
  if (F & FC) { CYCT(0x4ad1, 0x4ad3); goto destination_init; }
  CYC(0x4ad1, 0x4ad3);
  CYC(0x4ad3, 0x4ad5); A = 0x6e;
  CYC(0x4ad5, 0x4ad8);
  playSound_b00_hook(gb);
  return;

each_frame:
  CYC(0x4adc, 0x4adf); A = W8(wScrollMode);
  CYC(0x4adf, 0x4ae1); alu_and(gb, 0x0a);
  if (!(F & FZ)) { CYCT(0x4ae1, 0x4ae2); ret_effect(gb); return; }
  CYC(0x4ae1, 0x4ae2);
  CYC(0x4ae2, 0x4ae4); A = 0x00;
  CYC(0x4ae4, 0x4ae7); W8(wScrollMode) = A;
  CALL_C(0x4ae7, specialObjectAnimate_hook, 0x2aef, 0x4aea);
  CALL_C(0x4aea, itemDecCounter1_hook, 0x23d6, 0x4aed);
  if (!(F & FZ)) {
    CYCT(0x4aed, 0x4af0);
    specialObjectUpdatePosition(gb);
    return;
  }
  CYC(0x4aed, 0x4af0);
  CYC(0x4af0, 0x4af2); A = 0x01;
  CYC(0x4af2, 0x4af5); W8(wScrollMode) = A;
  CYC(0x4af5, 0x4af6); alu_xor(gb, A);
  CYC(0x4af6, 0x4af9); W8(wMenuDisabled) = A;
  CYC(0x4af9, 0x4afc); A = W8(wWarpTransition);
  CYC(0x4afc, 0x4afe); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x4afe, 0x4b01);
    warpUpdateRespawnPoint_hook(gb);
    return;
  }
  CYC(0x4afe, 0x4b01);
  CYC(0x4b01, 0x4b03); A = alu_swap(gb, A);
  CYC(0x4b03, 0x4b05); alu_and(gb, 0x03);
  CYC(0x4b05, 0x4b08); W8(wWarpTransition2) = A;
  CYC(0x4b08, 0x4b09); ret_effect(gb);
  return;

destination_init:
  CYC(0x4b09, 0x4b0a); H = D;
  CYC(0x4b0a, 0x4b0d); A = W8(wWarpDestPos);
  CYC(0x4b0d, 0x4b0f); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4b0f, 0x4b11);
    warp_transition3_enter_from_middle_bottom(gb, sp0_);
    return;
  }
  CYC(0x4b0f, 0x4b11);
  CYC(0x4b11, 0x4b13); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(0x4b13, 0x4b15);
    goto enter_from_bottom;
  }
  CYC(0x4b13, 0x4b15);
  CYC(0x4b15, 0x4b17); L = 0x0b;
  CALL_C(0x4b17, setShortPosition_hook, 0x20b8, 0x4b1a);
  CYC(0x4b1a, 0x4b1c); L = 0x06;
  CYC(0x4b1c, 0x4b1e); mem_wr(gb, HL, 0x1c);
  CYC(0x4b1e, 0x4b21);
  initLinkStateAndAnimateStanding(gb);
  return;

enter_from_bottom:
  CYC(0x4b48, 0x4b4b); push_effect(gb, 0x4b4b);
  warp_transition3_enter_from_middle_bottom(gb, sp0_);
  CYC(0x4b4b, 0x4b4e); A = W8(wWarpDestPos);
  CYC(0x4b4e, 0x4b50); A = alu_swap(gb, A);
  CYC(0x4b50, 0x4b52); alu_and(gb, 0xf0);
  CYC(0x4b52, 0x4b53); B = A;
  CYC(0x4b53, 0x4b56); A = W8(wActiveGroup);
  CYC(0x4b56, 0x4b58); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(0x4b58, 0x4b5a);
  } else {
    CYC(0x4b58, 0x4b5a);
    CYC(0x4b5a, 0x4b5b); alu_rlca(gb);
  }
  CYC(0x4b5b, 0x4b5c); alu_or(gb, B);
  CYC(0x4b5c, 0x4b5e); L = 0x0d;
  CYC(0x4b5e, 0x4b5f); mem_wr(gb, HL, A);
  CYC(0x4b5f, 0x4b60); ret_effect(gb);
}

static void warp_transition6_flicker_visibility_and_dec_counter1(GB *gb,
                                                                  uint16_t sp0_) {
  CYC(0x4d9d, 0x4d9f); B = 0x03;
  CALL_C(0x4d9f, objectFlickerVisibility_hook, 0x24e6, 0x4da2);
  CYC(0x4da2, 0x4da5);
  itemDecCounter1_hook(gb);
}

static void warp_transition6_create_destination_timewarp_animation(GB *gb,
                                                                    uint16_t sp0_) {
  CALL_C(0x4da5, getFreeInteractionSlot_hook, 0x3aef, 0x4da8);
  if (!(F & FZ)) { CYCT(0x4da8, 0x4da9); ret_effect(gb); return; }
  CYC(0x4da8, 0x4da9);
  CYC(0x4da9, 0x4dab); mem_wr(gb, HL, 0xdd);
  CYC(0x4dab, 0x4dac); L = alu_inc8(gb, L);
  CYC(0x4dac, 0x4dad); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4dad, 0x4db0); A = W8(wcc50);
  CYC(0x4db0, 0x4db1); L = alu_inc8(gb, L);
  CYC(0x4db1, 0x4db2); mem_wr(gb, HL, A);
  CYC(0x4db2, 0x4db3); ret_effect(gb);
}

static void warp_transition6_center_link_on_doorway(GB *gb, uint16_t sp0_) {
  CALL_C(0x4db3, objectGetTileAtPosition_hook, 0x1444, 0x4db6);
  CYC(0x4db6, 0x4db7); push_effect(gb, HL);
  CYC(0x4db7, 0x4db8); A = E;
  CYC(0x4db8, 0x4dbb); SET_HL(0x4de7);
  CALL_C(0x4dbb, findByteAtHl_hook, 0x1e17, 0x4dbe);
  CYC(0x4dbe, 0x4dbf); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(0x4dbf, 0x4dc0); ret_effect(gb); return; }
  CYC(0x4dbf, 0x4dc0);
  CYC(0x4dc0, 0x4dc1); push_effect(gb, HL);
  CYC(0x4dc1, 0x4dc2); L = alu_dec8(gb, L);
  CYC(0x4dc2, 0x4dc3); E = mem_rd(gb, HL);
  CYC(0x4dc3, 0x4dc6); SET_HL(0x4de7);
  CALL_C(0x4dc6, findByteAtHl_hook, 0x1e17, 0x4dc9);
  CYC(0x4dc9, 0x4dca); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(0x4dca, 0x4dcc); goto check_right_tile; }
  CYC(0x4dca, 0x4dcc);
  CYC(0x4dcc, 0x4dce); E = 0x0d;
  CYC(0x4dce, 0x4dcf); A = mem_rd(gb, DE);
  CYC(0x4dcf, 0x4dd1); alu_and(gb, 0xf0);
  CYC(0x4dd1, 0x4dd2); mem_wr(gb, DE, A);
  CYC(0x4dd2, 0x4dd3); ret_effect(gb);
  return;

check_right_tile:
  CYC(0x4dd3, 0x4dd4); L = alu_inc8(gb, L);
  CYC(0x4dd4, 0x4dd5); E = mem_rd(gb, HL);
  CYC(0x4dd5, 0x4dd8); SET_HL(0x4de7);
  CALL_C(0x4dd8, findByteAtHl_hook, 0x1e17, 0x4ddb);
  if (!(F & FC)) { CYCT(0x4ddb, 0x4ddc); ret_effect(gb); return; }
  CYC(0x4ddb, 0x4ddc);
  CYC(0x4ddc, 0x4dde); E = 0x0d;
  CYC(0x4dde, 0x4ddf); A = mem_rd(gb, DE);
  CYC(0x4ddf, 0x4de1); alu_add(gb, 0x08);
  CYC(0x4de1, 0x4de2); mem_wr(gb, DE, A);
  CYC(0x4de2, 0x4de5); SET_HL(wEnteredWarpPosition);
  CYC(0x4de5, 0x4de6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4de6, 0x4de7); ret_effect(gb);
}

void warpTransition6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d89, 0x4d8b); E = 0x05;
  CYC(0x4d8b, 0x4d8c); A = mem_rd(gb, DE);
  CYC(0x4d8c, 0x4d8d); push_effect(gb, 0x4d8d);
  switch (link_jump_table(gb)) {
    case 0x4def: goto substate0;
    case 0x4e07: goto substate1;
    case 0x4e3c: goto substate2;
    case 0x4e4d: goto substate3;
    case 0x4ea3: goto substate4;
    case 0x4eab: goto substate5;
    case 0x4eb7: goto substate6;
    case 0x4ec3: goto substate7;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4def, itemIncSubstate_hook, 0x23ef, 0x4df2);
  CYC(0x4df2, 0x4df4); L = 0x06;
  CYC(0x4df4, 0x4df6); mem_wr(gb, HL, 0x1e);
  CYC(0x4df6, 0x4df8); L = 0x08;
  CYC(0x4df8, 0x4dfa); mem_wr(gb, HL, 0x02);
  CYC(0x4dfa, 0x4dfb); A = D;
  CYC(0x4dfb, 0x4dfe); W8(wLinkCanPassNpcs) = A;
  CYC(0x4dfe, 0x4e01); W8(wMenuDisabled) = A;
  CYC(0x4e01, 0x4e04); push_effect(gb, 0x4e04);
  warp_transition6_center_link_on_doorway(gb, sp0_);
  CYC(0x4e04, 0x4e07);
  objectSetInvisible_hook(gb);
  return;

substate1:
  CYC(0x4e07, 0x4e0a); A = W8(wPaletteThread_mode);
  CYC(0x4e0a, 0x4e0b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4e0b, 0x4e0c); ret_effect(gb); return; }
  CYC(0x4e0b, 0x4e0c);
  CALL_C(0x4e0c, itemDecCounter1_hook, 0x23d6, 0x4e0f);
  if (!(F & FZ)) { CYCT(0x4e0f, 0x4e10); ret_effect(gb); return; }
  CYC(0x4e0f, 0x4e10);
  CYC(0x4e10, 0x4e12); mem_wr(gb, HL, 0x10);
  CYC(0x4e12, 0x4e15); push_effect(gb, 0x4e15);
  warp_transition6_create_destination_timewarp_animation(gb, sp0_);
  CYC(0x4e15, 0x4e18); A = W8(wSentBackByStrangeForce);
  CYC(0x4e18, 0x4e19); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4e19, 0x4e1b); goto warp_failed; }
  CYC(0x4e19, 0x4e1b);
  CYC(0x4e1b, 0x4e1e); SET_HL(0x62e9);
  CYC(0x4e1e, 0x4e20); E = 0x01;
  CALL_C(0x4e20, interBankCall_hook, 0x008a, 0x4e23);
  CYC(0x4e23, 0x4e25); C = alu_srl(gb, C);
  if (F & FC) { CYCT(0x4e25, 0x4e27); goto warp_failed; }
  CYC(0x4e25, 0x4e27);
  CYC(0x4e27, 0x4e2a); SET_HL(0x62c4);
  CYC(0x4e2a, 0x4e2c); E = 0x01;
  CALL_C(0x4e2c, interBankCall_hook, 0x008a, 0x4e2f);
  CYC(0x4e2f, 0x4e31); C = alu_srl(gb, C);
  if (F & FC) { CYCT(0x4e31, 0x4e33); goto warp_failed; }
  CYC(0x4e31, 0x4e33);
  CYC(0x4e33, 0x4e36);
  itemIncSubstate_hook(gb);
  return;

warp_failed:
  CYC(0x4e36, 0x4e38); E = 0x05;
  CYC(0x4e38, 0x4e3a); A = 0x04;
  CYC(0x4e3a, 0x4e3b); mem_wr(gb, DE, A);
  CYC(0x4e3b, 0x4e3c); ret_effect(gb);
  return;

substate2:
  CALL_C(0x4e3c, itemDecCounter1_hook, 0x23d6, 0x4e3f);
  if (!(F & FZ)) { CYCT(0x4e3f, 0x4e40); ret_effect(gb); return; }
  CYC(0x4e3f, 0x4e40);
  CYC(0x4e40, 0x4e42); mem_wr(gb, HL, 0x1e);

make_link_visible_and_play_sound:
  CYC(0x4e42, 0x4e44); A = 0xd4;
  CALL_C(0x4e44, playSound_b00_hook, 0x0c98, 0x4e47);
  CALL_C(0x4e47, objectSetVisiblec0_hook, 0x1e33, 0x4e4a);
  CYC(0x4e4a, 0x4e4d);
  itemIncSubstate_hook(gb);
  return;

substate3:
  CYC(0x4e4d, 0x4e50); push_effect(gb, 0x4e50);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(0x4e50, 0x4e51); ret_effect(gb); return; }
  CYC(0x4e50, 0x4e51);
  CYC(0x4e51, 0x4e54); A = W8(wLinkTimeWarpTile);
  CYC(0x4e54, 0x4e55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4e55, 0x4e57); goto maybe_show_strange_force_text; }
  CYC(0x4e55, 0x4e57);
  CYC(0x4e57, 0x4e5a); SET_HL(wPortalGroup);
  CYC(0x4e5a, 0x4e5d); A = W8(wActiveGroup);
  CYC(0x4e5d, 0x4e5e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e5e, 0x4e61); A = W8(wActiveRoom);
  CYC(0x4e61, 0x4e62); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e62, 0x4e65); A = W8(wWarpDestPos);
  CYC(0x4e65, 0x4e66); mem_wr(gb, HL, A);
  CYC(0x4e66, 0x4e67); C = A;
  CALL_C(0x4e67, getFreeInteractionSlot_hook, 0x3aef, 0x4e6a);
  if (!(F & FZ)) { CYCT(0x4e6a, 0x4e6c); goto maybe_show_strange_force_text; }
  CYC(0x4e6a, 0x4e6c);
  CYC(0x4e6c, 0x4e6e); mem_wr(gb, HL, 0xde);
  CYC(0x4e6e, 0x4e70); L = 0x4b;
  CALL_C(0x4e70, setShortPosition_paramC_hook, 0x20b9, 0x4e73);

maybe_show_strange_force_text:
  CYC(0x4e73, 0x4e76); A = W8(wSentBackByStrangeForce);
  CYC(0x4e76, 0x4e77); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4e77, 0x4e79);
  } else {
    CYC(0x4e77, 0x4e79);
    CYC(0x4e79, 0x4e7c); SET_BC(0x5112);
    CALL_C(0x4e7c, showText_hook, 0x1872, 0x4e7f);
  }
  CYC(0x4e7f, 0x4e80); alu_xor(gb, A);
  CYC(0x4e80, 0x4e83); W8(wLinkTimeWarpTile) = A;
  CYC(0x4e83, 0x4e86); W8(wWarpTransition) = A;
  CYC(0x4e86, 0x4e89); W8(wLinkCanPassNpcs) = A;
  CYC(0x4e89, 0x4e8c); W8(wMenuDisabled) = A;
  CYC(0x4e8c, 0x4e8f); W8(wSentBackByStrangeForce) = A;
  CYC(0x4e8f, 0x4e92); W8(wcddf) = A;
  CYC(0x4e92, 0x4e95); W8(wcde0) = A;
  CYC(0x4e95, 0x4e97); E = 0x2b;
  CYC(0x4e97, 0x4e99); A = 0x88;
  CYC(0x4e99, 0x4e9a); mem_wr(gb, DE, A);
  CALL_C(0x4e9a, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4e9d);
  CALL_C(0x4e9d, objectSetVisiblec2_hook, 0x1e45, 0x4ea0);
  CYC(0x4ea0, 0x4ea3);
  initLinkStateAndAnimateStanding(gb);
  return;

substate4:
  CALL_C(0x4ea3, itemDecCounter1_hook, 0x23d6, 0x4ea6);
  if (!(F & FZ)) { CYCT(0x4ea6, 0x4ea7); ret_effect(gb); return; }
  CYC(0x4ea6, 0x4ea7);
  CYC(0x4ea7, 0x4ea9); mem_wr(gb, HL, 0x78);
  CYCT(0x4ea9, 0x4eab);
  goto make_link_visible_and_play_sound;

substate5:
  CYC(0x4eab, 0x4eae); push_effect(gb, 0x4eae);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(0x4eae, 0x4eaf); ret_effect(gb); return; }
  CYC(0x4eae, 0x4eaf);
  CYC(0x4eaf, 0x4eb1); mem_wr(gb, HL, 0x10);
  CYC(0x4eb1, 0x4eb4); push_effect(gb, 0x4eb4);
  warp_transition6_create_destination_timewarp_animation(gb, sp0_);
  CYC(0x4eb4, 0x4eb7);
  itemIncSubstate_hook(gb);
  return;

substate6:
  CYC(0x4eb7, 0x4eba); push_effect(gb, 0x4eba);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(0x4eba, 0x4ebb); ret_effect(gb); return; }
  CYC(0x4eba, 0x4ebb);
  CYC(0x4ebb, 0x4ebd); mem_wr(gb, HL, 0x14);
  CALL_C(0x4ebd, objectSetInvisible_hook, 0x1e7b, 0x4ec0);
  CYC(0x4ec0, 0x4ec3);
  itemIncSubstate_hook(gb);
  return;

substate7:
  CALL_C(0x4ec3, itemDecCounter1_hook, 0x23d6, 0x4ec6);
  if (!(F & FZ)) { CYCT(0x4ec6, 0x4ec7); ret_effect(gb); return; }
  CYC(0x4ec6, 0x4ec7);
  CALL_C(0x4ec7, objectGetTileAtPosition_hook, 0x1444, 0x4eca);
  CYC(0x4eca, 0x4ecb); C = L;
  CYC(0x4ecb, 0x4ece); SET_HL(wWarpDestGroup);
  CYC(0x4ece, 0x4ed1); A = W8(wActiveGroup);
  CYC(0x4ed1, 0x4ed3); alu_xor(gb, 0x01);
  CYC(0x4ed3, 0x4ed5); alu_or(gb, 0x80);
  CYC(0x4ed5, 0x4ed6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ed6, 0x4ed9); A = W8(wActiveRoom);
  CYC(0x4ed9, 0x4eda); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4eda, 0x4edc); A = 0x06;
  CYC(0x4edc, 0x4edd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4edd, 0x4ede); A = C;
  CYC(0x4ede, 0x4edf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4edf, 0x4ee0); A = alu_inc8(gb, A);
  CYC(0x4ee0, 0x4ee3); W8(wLinkTimeWarpTile) = A;
  CYC(0x4ee3, 0x4ee6); W8(wcddf) = A;
  CYC(0x4ee6, 0x4ee8); A = 0x03;
  CYC(0x4ee8, 0x4ee9); mem_wr(gb, HL, A);
  CYC(0x4ee9, 0x4eea); alu_xor(gb, A);
  CYC(0x4eea, 0x4eed); W8(wScrollMode) = A;
  CYC(0x4eed, 0x4ef0); SET_HL(wSentBackByStrangeForce);
  CYC(0x4ef0, 0x4ef1); A = mem_rd(gb, HL);
  CYC(0x4ef1, 0x4ef2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4ef2, 0x4ef4);
  } else {
    CYC(0x4ef2, 0x4ef4);
    CYC(0x4ef4, 0x4ef5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(0x4ef5, 0x4ef8); A = W8(wLinkStateParameter);
  CYC(0x4ef8, 0x4efa); alu_bit(gb, 4, A);
  if (F & FZ) {
    CYC(0x4efa, 0x4efc);
    CALL_C(0x4efc, getThisRoomFlags_hook, 0x197d, 0x4eff);
    CYC(0x4eff, 0x4f01); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  } else {
    CYCT(0x4efa, 0x4efc);
  }
  CYC(0x4f01, 0x4f03); A = 0xd4;
  CALL_C(0x4f03, playSound_b00_hook, 0x0c98, 0x4f06);
  CYC(0x4f06, 0x4f09); SET_DE(w1Link);
  CYC(0x4f09, 0x4f0c);
  objectDelete_de_hook(gb);
}

void linkState01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54dd, 0x54df); A = 0x80;
  CYC(0x54df, 0x54e2); W8(wForceLinkPushAnimation) = A;
  CYC(0x54e2, 0x54e5); A = W8(wPaletteThread_mode);
  CYC(0x54e5, 0x54e6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x54e6, 0x54e7); ret_effect(gb); return; }
  CYC(0x54e6, 0x54e7);
  CYC(0x54e7, 0x54ea); A = W8(wScrollMode);
  CYC(0x54ea, 0x54ec); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x54ec, 0x54ed); ret_effect(gb); return; }
  CYC(0x54ec, 0x54ed);
  CALL_C(0x54ed, updateLinkDamageTaken_hook, 0x4268, 0x54f0);
  CYC(0x54f0, 0x54f3); A = W8(wLinkDeathTrigger);
  CYC(0x54f3, 0x54f4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x54f4, 0x54f7);
    setLinkStateToDead_hook(gb);
    return;
  }
  CYC(0x54f4, 0x54f7);
  CALL_C(0x54f7, checkLinkForceState, 0x54c0, 0x54fa);
  CALL_C(0x54fa, retIfTextIsActive_hook, 0x1859, 0x54fd);
  CYC(0x54fd, 0x5500); A = W8(wDisabledObjects);
  CYC(0x5500, 0x5502); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(0x5502, 0x5503); ret_effect(gb); return; }
  CYC(0x5502, 0x5503);
  CALL_C(0x5503, decPegasusSeedCounter_hook, 0x2bbd, 0x5506);
  CYC(0x5506, 0x5509); A = W8(w1Companion_id);
  CYC(0x5509, 0x550b); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(0x550b, 0x550d); goto interactions_done; }
  CYC(0x550b, 0x550d);
  CYC(0x550d, 0x550f); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(0x550f, 0x5511); goto interactions_done; }
  CYC(0x550f, 0x5511);
  CYC(0x5511, 0x5514); A = W8(wLinkObjectIndex);
  CYC(0x5514, 0x5515); alu_rrca(gb);
  if (F & FC) { CYCT(0x5515, 0x5516); ret_effect(gb); return; }
  CYC(0x5515, 0x5516);
  CYC(0x5516, 0x5519); A = W8(wLinkPlayingInstrument);
  CYC(0x5519, 0x551a); B = A;
  CYC(0x551a, 0x551d); A = W8(wLinkInAir);
  CYC(0x551d, 0x551e); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x551e, 0x5520); goto interactions_done; }
  CYC(0x551e, 0x5520);
  CYC(0x5520, 0x5522); E = 0x2d;
  CYC(0x5522, 0x5523); A = mem_rd(gb, DE);
  CYC(0x5523, 0x5524); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5524, 0x5526); goto interactions_done; }
  CYC(0x5524, 0x5526);
  CALL_C(0x5526, linkInteractWithAButtonSensitiveObjects_hook, 0x1b5d, 0x5529);
  if (F & FC) { CYCT(0x5529, 0x552a); ret_effect(gb); return; }
  CYC(0x5529, 0x552a);
  CALL_C(0x552a, interactWithTileBeforeLink_b00_hook, 0x1280, 0x552d);
  if (F & FC) { CYCT(0x552d, 0x552e); ret_effect(gb); return; }
  CYC(0x552d, 0x552e);

interactions_done:
  CYC(0x552e, 0x552f); alu_xor(gb, A);
  CYC(0x552f, 0x5532); W8(wForceLinkPushAnimation) = A;
  CYC(0x5532, 0x5535); W8(wLinkPlayingInstrument) = A;
  CYC(0x5535, 0x5538); A = W8(wTilesetFlags);
  CYC(0x5538, 0x553a); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x553a, 0x553d);
    linkState01_sidescroll(gb);
    return;
  }
  CYC(0x553a, 0x553d);
  CALL_C(0x553d, linkApplyTileTypes_hook, 0x42b7, 0x5540);
  CALL_C(0x5540, checkAndUpdateLinkOnChest_hook, 0x1255, 0x5543);
  CALL_C(0x5543, checkUseItems_b00_hook, 0x2c18, 0x5546);
  CYC(0x5546, 0x5549); A = W8(wLinkPlayingInstrument);
  CYC(0x5549, 0x554a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x554a, 0x554b); ret_effect(gb); return; }
  CYC(0x554a, 0x554b);
  CALL_C(0x554b, specialObjectUpdateAdjacentWallsBitset, 0x5e62, 0x554e);
  CALL_C(0x554e, linkUpdateKnockback, 0x5d5b, 0x5551);
  CYC(0x5551, 0x5554); A = W8(wLinkSwimmingState);
  CYC(0x5554, 0x5556); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x5556, 0x5558); goto update_in_air; }
  CYC(0x5556, 0x5558);
  CYC(0x5558, 0x555b); A = W8(wMagnetGloveState);
  CYC(0x555b, 0x555d); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x555d, 0x555f); goto update_in_air; }
  CYC(0x555d, 0x555f);
  CYC(0x555f, 0x5562); A = W8(wLinkInAir);
  CYC(0x5562, 0x5563); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5563, 0x5565); goto update_in_air; }
  CYC(0x5563, 0x5565);
  CYC(0x5565, 0x5568); A = W8(wLinkGrabState);
  CYC(0x5568, 0x5569); C = A;
  CYC(0x5569, 0x556c); A = W8(wLinkImmobilized);
  CYC(0x556c, 0x556d); alu_or(gb, C);
  if (!(F & FZ)) { CYCT(0x556d, 0x556f); goto update_in_air; }
  CYC(0x556d, 0x556f);
  CALL_C(0x556f, checkLinkPushingAgainstBed, 0x5faf, 0x5572);
  CALL_C(0x5572, checkLinkJumpingOffCliff, 0x6034, 0x5575);

update_in_air:
  CALL_C(0x5575, linkUpdateInAir, 0x5af3, 0x5578);
  CYC(0x5578, 0x557b); A = W8(wLinkInAir);
  CYC(0x557b, 0x557c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x557c, 0x557e); goto not_in_air; }
  CYC(0x557c, 0x557e);
  CYC(0x557e, 0x5580); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x5580, 0x5582);
  } else {
    CYC(0x5580, 0x5582);
    CYC(0x5582, 0x5584); E = 0x15;
    CYC(0x5584, 0x5585); A = mem_rd(gb, DE);
    CYC(0x5585, 0x5587); alu_bit(gb, 7, A);
    if (F & FZ) CALL_C_CC(0x5587, linkUpdateVelocity, 0x58f0, 0x558a);
    else CYC(0x5587, 0x558a);
  }
  CYC(0x558a, 0x558d); SET_HL(wcc95);
  CYC(0x558d, 0x558f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CALL_C(0x558f, specialObjectSetAngleRelativeToVar38, 0x5ff3, 0x5592);
  CALL_C(0x5592, specialObjectUpdatePosition, 0x5d97, 0x5595);
  CYC(0x5595, 0x5598);
  specialObjectAnimate_hook(gb);
  return;

not_in_air:
  CYC(0x5598, 0x559b); A = W8(wMagnetGloveState);
  CYC(0x559b, 0x559d); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x559d, 0x55a0);
    animateLinkStanding(gb);
    return;
  }
  CYC(0x559d, 0x55a0);
  CYC(0x55a0, 0x55a2); E = 0x2d;
  CYC(0x55a2, 0x55a3); A = mem_rd(gb, DE);
  CYC(0x55a3, 0x55a4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55a4, 0x55a7);
    func_5631(gb);
    return;
  }
  CYC(0x55a4, 0x55a7);
  CYC(0x55a7, 0x55a8); H = D;
  CYC(0x55a8, 0x55aa); L = 0x24;
  CYC(0x55aa, 0x55ac); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x55ac, 0x55af); A = W8(wLinkSwimmingState);
  CYC(0x55af, 0x55b0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55b0, 0x55b3);
    linkUpdateSwimming(gb);
    return;
  }
  CYC(0x55b0, 0x55b3);
  CALL_C(0x55b3, objectSetVisiblec1_hook, 0x1e3c, 0x55b6);
  CYC(0x55b6, 0x55b9); A = W8(wLinkObjectIndex);
  CYC(0x55b9, 0x55ba); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x55ba, 0x55bc); goto check_underwater_transition; }
  CYC(0x55ba, 0x55bc);
  CYC(0x55bc, 0x55bf); A = W8(w1Companion_id);
  CYC(0x55bf, 0x55c1); alu_or(gb, 0x13);
  if (F & FZ) { CYCT(0x55c1, 0x55c3); goto update_direction_if_not_using_item; }
  CYC(0x55c1, 0x55c3);
  CYCT(0x55c3, 0x55c5);
  goto update_direction;

check_underwater_transition:
  CALL_C(0x55c5, checkForUnderwaterTransition, 0x516c, 0x55c8);
  CYC(0x55c8, 0x55cb); SET_HL(0x462d);
  CYC(0x55cb, 0x55cd); E = 0x06;
  CALL_C(0x55cd, interBankCall_hook, 0x008a, 0x55d0);
  CYC(0x55d0, 0x55d1); A = B;
  CYC(0x55d1, 0x55d2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55d2, 0x55d5);
    setLinkIDOverride_hook(gb);
    return;
  }
  CYC(0x55d2, 0x55d5);
  CYC(0x55d5, 0x55d6); H = D;
  CYC(0x55d6, 0x55d8); L = 0x2f;
  CYC(0x55d8, 0x55da); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x55da, 0x55dc); goto ordinary_movement; }
  CYC(0x55da, 0x55dc);
  CALL_C(0x55dc, linkUpdateVelocity__mermaidSuit, 0x58f7, 0x55df);
  CYCT(0x55df, 0x55e1);
  goto velocity_updated;

ordinary_movement:
  CYC(0x55e1, 0x55e4); A = W8(wLinkGrabState);
  CYC(0x55e4, 0x55e6); alu_and(gb, 0x0f);
  CYC(0x55e6, 0x55e7); A = alu_dec8(gb, A);
  CYC(0x55e7, 0x55e9); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x55e9, 0x55eb); goto normal_movement; }
  CYC(0x55e9, 0x55eb);
  CYC(0x55eb, 0x55ee); SET_HL(wIsTileSlippery);
  CYC(0x55ee, 0x55f0); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x55f0, 0x55f2); goto normal_movement; }
  CYC(0x55f0, 0x55f2);
  CYC(0x55f2, 0x55f4); C = 0x88;
  CALL_C(0x55f4, updateLinkSpeed_withParam, 0x5ce8, 0x55f7);
  CALL_C(0x55f7, linkUpdateVelocity, 0x58f0, 0x55fa);

velocity_updated:
  CYC(0x55fa, 0x55fd); A = W8(wLinkAngle);
  CYC(0x55fd, 0x55fe); alu_rlca(gb);
  CYC(0x55fe, 0x5600); C = 0x02;
  if (F & FC) { CYCT(0x5600, 0x5602); goto update_movement; }
  CYC(0x5600, 0x5602);
  CYCT(0x5602, 0x5604);
  goto walking;

normal_movement:
  CYC(0x5604, 0x5607); A = W8(wcc95);
  CYC(0x5607, 0x5608); B = A;
  CYC(0x5608, 0x560a); E = 0x09;
  CYC(0x560a, 0x560d); A = W8(wLinkAngle);
  CYC(0x560d, 0x560e); mem_wr(gb, DE, A);
  CYC(0x560e, 0x560f); alu_or(gb, B);
  CYC(0x560f, 0x5610); alu_rlca(gb);
  CYC(0x5610, 0x5612); C = 0x00;
  if (F & FC) { CYCT(0x5612, 0x5614); goto update_movement; }
  CYC(0x5612, 0x5614);
  CYC(0x5614, 0x5616); C = 0x01;
  CYC(0x5616, 0x5619); A = W8(wLinkImmobilized);
  CYC(0x5619, 0x561a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x561a, 0x561c); goto update_movement; }
  CYC(0x561a, 0x561c);
  CALL_C(0x561c, updateLinkSpeed_standard, 0x5ce6, 0x561f);

walking:
  CYC(0x561f, 0x5621); C = 0x07;

update_movement:
  CALL_C(0x5621, linkUpdateMovement, 0x5ad0, 0x5624);

update_direction_if_not_using_item:
  CYC(0x5624, 0x5627); A = W8(wLinkTurningDisabled);
  CYC(0x5627, 0x5628); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5628, 0x5629); ret_effect(gb); return; }
  CYC(0x5628, 0x5629);

update_direction:
  CYC(0x5629, 0x562c);
  updateLinkDirectionFromAngle_hook(gb);
}

void specialObjectCode_link_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49b6, 0x49b8); E = 0x04;
  CYC(0x49b8, 0x49b9); A = mem_rd(gb, DE);
  CYC(0x49b9, 0x49ba); push_effect(gb, 0x49ba);
  switch (link_jump_table(gb)) {
    case 0x49e4: linkState00_hook(gb); return;
    case 0x54dd: linkState01_hook(gb); return;
    case 0x507b: linkState02_hook(gb); return;
    case 0x5033: linkState03_hook(gb); return;
    case 0x4ff6: linkState04_hook(gb); return;
    case 0x52fb: linkState05_hook(gb); return;
    case 0x5377: linkState06_hook(gb); return;
    case 0x2cad: linkState07_hook(gb); return;
    case 0x4f0c: linkState08_hook(gb); return;
    case 0x53ea: linkState09_hook(gb); return;
    case 0x4a00: linkState0a_hook(gb); return;
    case 0x4fc7: linkState0b_hook(gb); return;
    case 0x51ec: linkState0c_hook(gb); return;
    case 0x529e: linkState0d_hook(gb); return;
    case 0x4f4f: linkState0e_hook(gb); return;
    case 0x4f80: linkState0f_hook(gb); return;
    case 0x5467: linkState11_hook(gb); return;
    case 0x60a5: linkState12(gb); return;
    case 0x5227: linkState13_hook(gb); return;
    case 0x527f: linkState14_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void linkState02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x507b, 0x507d); A = 0xff;
  CYC(0x507d, 0x5080); W8(wGameKeysPressed) = A;
  CYC(0x5080, 0x5082); A = 0x80;
  CYC(0x5082, 0x5085); W8(wForceLinkPushAnimation) = A;
  CYC(0x5085, 0x5087); E = 0x05;
  CYC(0x5087, 0x5088); A = mem_rd(gb, DE);
  CYC(0x5088, 0x5089); push_effect(gb, 0x5089);
  switch (link_jump_table(gb)) {
    case 0x5095: goto substate0;
    case 0x50cc: goto substate1;
    case 0x510f: goto substate2;
    case 0x514a: goto substate3;
    case 0x5155: goto substate4;
    case 0x5163: goto substate5;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x5095, linkCancelAllItemUsage_hook, 0x4f49, 0x5098);
  CYC(0x5098, 0x509b); A = W8(wLinkStateParameter);
  CYC(0x509b, 0x509c); push_effect(gb, 0x509c);
  switch (link_jump_table(gb)) {
    case 0x50b0: goto parameter_fall_down_hole;
    case 0x50b3: goto parameter_fall_without_centering;
    case 0x50f9: goto respawn;
    case 0x50b8: goto parameter_3;
    case 0x50a6: goto parameter_drown;
    default: hook_continue(gb, HL, sp0_); return;
  }

parameter_drown:
  CYC(0x50a6, 0x50a8); E = 0x05;
  CYC(0x50a8, 0x50aa); A = 0x05;
  CYC(0x50aa, 0x50ab); mem_wr(gb, DE, A);
  CYC(0x50ab, 0x50ad); A = 0x0a;
  CYC(0x50ad, 0x50b0);
  specialObjectSetAnimation_hook(gb);
  return;

parameter_fall_down_hole:
  CALL_C(0x50b0, objectCenterOnTile_hook, 0x20db, 0x50b3);

parameter_fall_without_centering:
  CALL_C(0x50b3, itemIncSubstate_hook, 0x23ef, 0x50b6);
  CYC(0x50b6, 0x50b8);
  goto start_falling;

parameter_3:
  CYC(0x50b8, 0x50ba); E = 0x05;
  CYC(0x50ba, 0x50bc); A = 0x04;
  CYC(0x50bc, 0x50bd); mem_wr(gb, DE, A);

start_falling:
  CYC(0x50bd, 0x50be); H = D;
  CYC(0x50be, 0x50c0); L = 0x24;
  CYC(0x50c0, 0x50c2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x50c2, 0x50c4); A = 0x0d;
  CALL_C(0x50c4, specialObjectSetAnimation_hook, 0x2b0a, 0x50c7);
  CYC(0x50c7, 0x50c9); A = 0x65;
  CYC(0x50c9, 0x50cc);
  playSound_b00_hook(gb);
  return;

substate1:
  CYC(0x50cc, 0x50cd); H = D;
  CYC(0x50cd, 0x50cf); L = 0x21;
  CYC(0x50cf, 0x50d1); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x50d1, 0x50d4);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x50d1, 0x50d4);
  CYC(0x50d4, 0x50d7); A = W8(wActiveTileType);
  CYC(0x50d7, 0x50d9); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x50d9, 0x50db); goto respawn; }
  CYC(0x50d9, 0x50db);
  CYC(0x50db, 0x50de); A = W8(wActiveGroup);
  CYC(0x50de, 0x50e0); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x50e0, 0x50e2); goto ordinary_hole_warp; }
  CYC(0x50e0, 0x50e2);
  CYC(0x50e2, 0x50e5); A = W8(wActiveRoom);
  CYC(0x50e5, 0x50e7); alu_cp(gb, 0x9f);
  if (!(F & FZ)) { CYCT(0x50e7, 0x50e9); goto ordinary_hole_warp; }
  CYC(0x50e7, 0x50e9);
  CYC(0x50e9, 0x50ec); SET_HL(0x7d92);
  CYC(0x50ec, 0x50ee); E = 0x01;
  CYC(0x50ee, 0x50f1);
  interBankCall_hook(gb);
  return;

ordinary_hole_warp:
  CYC(0x50f1, 0x50f4); SET_HL(0x4ad2);
  CYC(0x50f4, 0x50f6); E = 0x01;
  CYC(0x50f6, 0x50f9);
  interBankCall_hook(gb);
  return;

respawn:
  CALL_C(0x50f9, specialObjectSetCoordinatesToRespawnYX_hook, 0x2b8a, 0x50fc);
  CYC(0x50fc, 0x50fe); L = 0x05;
  CYC(0x50fe, 0x5100); A = 0x02;
  CYC(0x5100, 0x5101); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5101, 0x5102); mem_wr(gb, HL, A);
  CALL_C(0x5102, specialObjectTryToBreakTile_source05_hook, 0x49a8, 0x5105);
  CALL_C(0x5105, objectGetTileAtPosition_hook, 0x1444, 0x5108);
  CYC(0x5108, 0x5109); A = L;
  CYC(0x5109, 0x510c); W8(wEnteredWarpPosition) = A;
  CYC(0x510c, 0x510f);
  objectSetInvisible_hook(gb);
  return;

substate2:
  CYC(0x510f, 0x5110); H = D;
  CYC(0x5110, 0x5112); L = 0x06;
  CYC(0x5112, 0x5115); A = W8(wScrollMode);
  CYC(0x5115, 0x5117); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYC(0x5117, 0x5119);
    CYC(0x5119, 0x511b); mem_wr(gb, HL, 0x04);
    CYC(0x511b, 0x511c); ret_effect(gb);
    return;
  }
  CYCT(0x5117, 0x5119);
  CYC(0x511c, 0x511d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x511d, 0x511e); ret_effect(gb); return; }
  CYC(0x511d, 0x511e);
  CYC(0x511e, 0x511f); alu_xor(gb, A);
  CYC(0x511f, 0x5122); W8(wLinkInAir) = A;
  CYC(0x5122, 0x5125); W8(wLinkSwimmingState) = A;
  CYC(0x5125, 0x5127); A = 0x1c;
  CALL_C(0x5127, cpActiveRing_hook, 0x23b0, 0x512a);
  CYC(0x512a, 0x512c); A = 0xfc;
  if (!(F & FZ)) {
    CYCT(0x512c, 0x512e);
  } else {
    CYC(0x512c, 0x512e);
    CYC(0x512e, 0x5130); A = alu_sra(gb, A);
  }
  CALL_C(0x5130, itemIncSubstate_hook, 0x23ef, 0x5133);
  CYC(0x5133, 0x5135); L = 0x25;
  CYC(0x5135, 0x5136); mem_wr(gb, HL, A);
  CYC(0x5136, 0x5138); L = 0x2b;
  CYC(0x5138, 0x513a); mem_wr(gb, HL, 0x3c);
  CYC(0x513a, 0x513c); L = 0x06;
  CYC(0x513c, 0x513e); mem_wr(gb, HL, 0x10);
  CALL_C(0x513e, linkApplyDamage_b00_hook, 0x2ab7, 0x5141);
  CALL_C(0x5141, objectSetVisiblec1_hook, 0x1e3c, 0x5144);
  CALL_C(0x5144, specialObjectUpdateAdjacentWallsBitset, 0x5e62, 0x5147);
  CYC(0x5147, 0x514a);
  animateLinkStanding(gb);
  return;

substate3:
  CALL_C(0x514a, itemDecCounter1_hook, 0x23d6, 0x514d);
  if (!(F & FZ)) { CYCT(0x514d, 0x514e); ret_effect(gb); return; }
  CYC(0x514d, 0x514e);
  CYC(0x514e, 0x5150); L = 0x24;
  CYC(0x5150, 0x5152); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5152, 0x5155);
  initLinkStateAndAnimateStanding(gb);
  return;

substate4:
  CYC(0x5155, 0x5156); H = D;
  CYC(0x5156, 0x5158); L = 0x21;
  CYC(0x5158, 0x515a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x515a, 0x515d);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x515a, 0x515d);
  CALL_C(0x515d, objectSetInvisible_hook, 0x1e7b, 0x5160);
  CYC(0x5160, 0x5163);
  checkLinkForceState(gb);
  return;

substate5:
  CYC(0x5163, 0x5165); E = 0x21;
  CYC(0x5165, 0x5166); A = mem_rd(gb, DE);
  CYC(0x5166, 0x5167); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x5167, 0x516a);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x5167, 0x516a);
  CYC(0x516a, 0x516c);
  goto respawn;
}

void linkState0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51ec, 0x51ee); E = 0x05;
  CYC(0x51ee, 0x51ef); A = mem_rd(gb, DE);
  CYC(0x51ef, 0x51f0); push_effect(gb, 0x51f0);
  switch (link_jump_table(gb)) {
    case 0x51f6: goto substate0;
    case 0x5226: goto done;
    case 0x520d: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x51f6, 0x51f8); A = 0x01;
  CYC(0x51f8, 0x51f9); mem_wr(gb, DE, A);
  CYC(0x51f9, 0x51fc); W8(wWarpsDisabled) = A;
  CYC(0x51fc, 0x51fd); alu_xor(gb, A);
  CYC(0x51fd, 0x51ff); E = 0x24;
  CYC(0x51ff, 0x5200); mem_wr(gb, DE, A);
  CYC(0x5200, 0x5202); A = 0x00;
  CYC(0x5202, 0x5205); W8(wScrollMode) = A;
  CALL_C(0x5205, linkCancelAllItemUsage_hook, 0x4f49, 0x5208);
  CYC(0x5208, 0x520a); A = 0x67;
  CYC(0x520a, 0x520d);
  playSound_b00_hook(gb);
  return;

substate2:
  CYC(0x520d, 0x520e); alu_xor(gb, A);
  CYC(0x520e, 0x5211); W8(wWarpsDisabled) = A;
  CYC(0x5211, 0x5214); SET_HL(wWarpDestGroup);
  CYC(0x5214, 0x5217); A = W8(wActiveGroup);
  CYC(0x5217, 0x5219); alu_or(gb, 0x80);
  CYC(0x5219, 0x521a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x521a, 0x521d); A = W8(wDungeonWallmasterDestRoom);
  CYC(0x521d, 0x521e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x521e, 0x5220); A = 0x05;
  CYC(0x5220, 0x5221); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5221, 0x5223); A = 0x87;
  CYC(0x5223, 0x5224); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5224, 0x5226); mem_wr(gb, HL, 0x03);

done:
  CYC(0x5226, 0x5227); ret_effect(gb);
}

static void link_state13_substate1(GB *gb, uint16_t sp0_) {
  CYC(0x524a, 0x524c); C = 0x40;
  CALL_C(0x524c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x524f);
  CYC(0x524f, 0x5252); A = W8(wcc50);
  CYC(0x5252, 0x5253); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5253, 0x5255);
    goto update_counter;
  }
  CYC(0x5253, 0x5255);
  CALL_C(0x5255, updateLinkDirectionFromAngle_hook, 0x2b64, 0x5258);
  CYC(0x5258, 0x525a); L = 0x2a;
  CYC(0x525a, 0x525b); A = mem_rd(gb, HL);
  CYC(0x525b, 0x525c); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x525c, 0x525e); goto restore; }
  CYC(0x525c, 0x525e);

update_counter:
  CYC(0x525e, 0x5260); C = 0x01;
  CYC(0x5260, 0x5263); A = W8(wGameKeysJustPressed);
  CYC(0x5263, 0x5264); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5264, 0x5266);
  } else {
    CYC(0x5264, 0x5266);
    CYC(0x5266, 0x5268); C = 0x04;
  }
  CYC(0x5268, 0x526a); L = 0x06;
  CYC(0x526a, 0x526b); A = mem_rd(gb, HL);
  CYC(0x526b, 0x526c); alu_sub(gb, C);
  CYC(0x526c, 0x526d); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(0x526d, 0x526e); ret_effect(gb); return; }
  CYC(0x526d, 0x526e);

restore:
  CYC(0x526e, 0x5270); L = 0x1b;
  CYC(0x5270, 0x5272); A = 0x08;
  CYC(0x5272, 0x5273); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5273, 0x5274); mem_wr(gb, HL, A);
  CYC(0x5274, 0x5276); L = 0x2d;
  CYC(0x5276, 0x5278); mem_wr(gb, HL, 0x00);
  CYC(0x5278, 0x5279); alu_xor(gb, A);
  CYC(0x5279, 0x527c); W8(wLinkForceState) = A;
  CYC(0x527c, 0x527f);
  initLinkStateAndAnimateStanding(gb);
}

void linkState13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5227, 0x5229); A = 0x80;
  CYC(0x5229, 0x522c); W8(wForceLinkPushAnimation) = A;
  CYC(0x522c, 0x522e); E = 0x05;
  CYC(0x522e, 0x522f); A = mem_rd(gb, DE);
  CYC(0x522f, 0x5230); push_effect(gb, 0x5230);
  switch (link_jump_table(gb)) {
    case 0x5234: break;
    case 0x524a: link_state13_substate1(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5234, itemIncSubstate_hook, 0x23ef, 0x5237);
  CYC(0x5237, 0x5238); L = alu_inc8(gb, L);
  CYC(0x5238, 0x523a); mem_wr(gb, HL, 0xb4);
  CYC(0x523a, 0x523c); L = 0x1b;
  CYC(0x523c, 0x523e); A = 0x0f;
  CYC(0x523e, 0x523f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x523f, 0x5240); mem_wr(gb, HL, A);
  CYC(0x5240, 0x5242); A = 0x7f;
  CALL_C(0x5242, loadPaletteHeader_hook, 0x050b, 0x5245);
  CYC(0x5245, 0x5246); alu_xor(gb, A);
  CYC(0x5246, 0x5249); W8(wcc50) = A;
  CYC(0x5249, 0x524a); ret_effect(gb);
}

void linkState14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x527f, 0x5281); E = 0x05;
  CYC(0x5281, 0x5282); A = mem_rd(gb, DE);
  CYC(0x5282, 0x5283); push_effect(gb, 0x5283);
  switch (link_jump_table(gb)) {
    case 0x5287: break;
    case 0x524a: link_state13_substate1(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5287, itemIncSubstate_hook, 0x23ef, 0x528a);
  CYC(0x528a, 0x528c); L = 0x06;
  CYC(0x528c, 0x528e); mem_wr(gb, HL, 0xf0);
  CALL_C(0x528e, linkCancelAllItemUsage_hook, 0x4f49, 0x5291);
  CYC(0x5291, 0x5294); A = W8(wcc50);
  CYC(0x5294, 0x5295); alu_or(gb, A);
  CYC(0x5295, 0x5297); A = 0x02;
  if (F & FZ) {
    CYCT(0x5297, 0x5299);
  } else {
    CYC(0x5297, 0x5299);
    CYC(0x5299, 0x529b); A = 0x10;
  }
  CYC(0x529b, 0x529e);
  specialObjectSetAnimation_hook(gb);
}

void linkState0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x529e, 0x52a0); A = 0x80;
  CYC(0x52a0, 0x52a3); W8(wcc92) = A;
  CYC(0x52a3, 0x52a5); E = 0x05;
  CYC(0x52a5, 0x52a6); A = mem_rd(gb, DE);
  CYC(0x52a6, 0x52a7); push_effect(gb, 0x52a7);
  switch (link_jump_table(gb)) {
    case 0x52b1: goto substate0;
    case 0x4268: updateLinkDamageTaken_hook(gb); return;
    case 0x52be: goto substate2;
    case 0x52dc: goto substate3;
    case 0x52ef: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x52b1, 0x52b3); A = 0x01;
  CYC(0x52b3, 0x52b4); mem_wr(gb, DE, A);
  CYC(0x52b4, 0x52b7); W8(wWarpsDisabled) = A;
  CYC(0x52b7, 0x52b9); E = 0x30;
  CYC(0x52b9, 0x52ba); alu_xor(gb, A);
  CYC(0x52ba, 0x52bb); mem_wr(gb, DE, A);
  CYC(0x52bb, 0x52be);
  linkCancelAllItemUsage_hook(gb);
  return;

substate2:
  CYC(0x52be, 0x52c0); A = 0x03;
  CYC(0x52c0, 0x52c1); mem_wr(gb, DE, A);
  CYC(0x52c1, 0x52c2); H = D;
  CYC(0x52c2, 0x52c4); L = 0x06;
  CYC(0x52c4, 0x52c6); mem_wr(gb, HL, 0x1e);
  CYC(0x52c6, 0x52c8); L = 0x14;
  CYC(0x52c8, 0x52ca); A = 0x20;
  CYC(0x52ca, 0x52cb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x52cb, 0x52cd); mem_wr(gb, HL, 0xfe);
  CYC(0x52cd, 0x52cf); L = 0x08;
  CYC(0x52cf, 0x52d0); alu_xor(gb, A);
  CYC(0x52d0, 0x52d1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x52d1, 0x52d3); mem_wr(gb, HL, 0x10);
  CYC(0x52d3, 0x52d5); L = 0x10;
  CYC(0x52d5, 0x52d7); mem_wr(gb, HL, 0x3c);
  CYC(0x52d7, 0x52d9); A = 0x03;
  CYC(0x52d9, 0x52dc);
  specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CALL_C(0x52dc, itemDecCounter1_hook, 0x23d6, 0x52df);
  if (F & FZ) { CYCT(0x52df, 0x52e1); goto restore_normal; }
  CYC(0x52df, 0x52e1);
  CYC(0x52e1, 0x52e3); C = 0x20;
  CALL_C(0x52e3, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x52e6);
  CALL_C(0x52e6, specialObjectUpdateAdjacentWallsBitset, 0x5e62, 0x52e9);
  CALL_C(0x52e9, specialObjectUpdatePosition, 0x5d97, 0x52ec);
  CYC(0x52ec, 0x52ef);
  specialObjectAnimate_hook(gb);
  return;

substate4:
  CYC(0x52ef, 0x52f0); H = D;
  CYC(0x52f0, 0x52f2); L = 0x2b;
  CYC(0x52f2, 0x52f4); mem_wr(gb, HL, 0x94);

restore_normal:
  CYC(0x52f4, 0x52f5); alu_xor(gb, A);
  CYC(0x52f5, 0x52f8); W8(wWarpsDisabled) = A;
  CYC(0x52f8, 0x52fb);
  initLinkStateAndAnimateStanding(gb);
}

void linkState05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52fb, 0x52fd); E = 0x05;
  CYC(0x52fd, 0x52fe); A = mem_rd(gb, DE);
  CYC(0x52fe, 0x52ff); push_effect(gb, 0x52ff);
  switch (link_jump_table(gb)) {
    case 0x5305: goto substate0;
    case 0x5323: goto substate1;
    case 0x5338: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x5305, itemIncSubstate_hook, 0x23ef, 0x5308);
  CYC(0x5308, 0x530a); L = 0x10;
  CYC(0x530a, 0x530c); mem_wr(gb, HL, 0x14);
  CYC(0x530c, 0x530e); L = 0x18;
  CYC(0x530e, 0x5310); A = 0x02;
  CALL_C(0x5310, specialObjectSetVar37AndVar38, 0x5fed, 0x5313);
  CYC(0x5313, 0x5316); SET_BC(0xfe80);
  CALL_C(0x5316, objectSetSpeedZ_hook, 0x239d, 0x5319);
  CYC(0x5319, 0x531b); A = 0x81;
  CYC(0x531b, 0x531e); W8(wLinkInAir) = A;
  CYC(0x531e, 0x5320); A = 0x2f;
  CYC(0x5320, 0x5323);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x5323, specialObjectAnimate_hook, 0x2aef, 0x5326);
  CALL_C(0x5326, specialObjectSetAngleRelativeToVar38, 0x5ff3, 0x5329);
  CALL_C(0x5329, objectApplySpeed_hook, 0x201d, 0x532c);
  CYC(0x532c, 0x532e); C = 0x20;
  CALL_C(0x532e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5331);
  if (!(F & FZ)) { CYCT(0x5331, 0x5332); ret_effect(gb); return; }
  CYC(0x5331, 0x5332);
  CALL_C(0x5332, itemIncSubstate_hook, 0x23ef, 0x5335);
  CYC(0x5335, 0x5338);
  specialObjectSetPositionToVar38IfSet(gb);
  return;

substate2:
  CALL_C(0x5338, specialObjectAnimate_hook, 0x2aef, 0x533b);
  CYC(0x533b, 0x533c); H = D;
  CYC(0x533c, 0x533e); L = 0x21;
  CYC(0x533e, 0x533f); A = mem_rd(gb, HL);
  CYC(0x533f, 0x5341); mem_wr(gb, HL, 0x00);
  CYC(0x5341, 0x5342); push_effect(gb, 0x5342);
  switch (link_jump_table(gb)) {
    case 0x534c: goto anim_parameter1;
    case 0x5355: goto anim_parameter2;
    case 0x535a: goto anim_parameter0;
    case 0x535b: goto anim_parameter3;
    case 0x535e: goto anim_parameter4;
    default: hook_continue(gb, HL, sp0_); return;
  }

anim_parameter1:
  CALL_C(0x534c, darkenRoomLightly_hook, 0x32f8, 0x534f);
  CYC(0x534f, 0x5351); A = 0x06;
  CYC(0x5351, 0x5354); W8(wPaletteThread_updateRate) = A;
  CYC(0x5354, 0x5355); ret_effect(gb);
  return;

anim_parameter2:
  CYC(0x5355, 0x5358); SET_HL(wLinkMaxHealth);
  CYC(0x5358, 0x5359); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5359, 0x535a); mem_wr(gb, HL, A);

anim_parameter0:
  CYC(0x535a, 0x535b); ret_effect(gb);
  return;

anim_parameter3:
  CYC(0x535b, 0x535e);
  brightenRoom_hook(gb);
  return;

anim_parameter4:
  CYC(0x535e, 0x5361); SET_BC(0xfe80);
  CALL_C(0x5361, objectSetSpeedZ_hook, 0x239d, 0x5364);
  CYC(0x5364, 0x5366); L = 0x08;
  CYC(0x5366, 0x5368); mem_wr(gb, HL, 0x03);
  CYC(0x5368, 0x5369); L = alu_inc8(gb, L);
  CYC(0x5369, 0x536b); mem_wr(gb, HL, 0x18);
  CYC(0x536b, 0x536d); L = 0x10;
  CYC(0x536d, 0x536f); mem_wr(gb, HL, 0x14);
  CYC(0x536f, 0x5371); A = 0x81;
  CYC(0x5371, 0x5374); W8(wLinkInAir) = A;
  CYC(0x5374, 0x5377);
  initLinkStateAndAnimateStanding(gb);
}

void linkState06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5377, 0x5379); E = 0x05;
  CYC(0x5379, 0x537a); A = mem_rd(gb, DE);
  CYC(0x537a, 0x537b); push_effect(gb, 0x537b);
  switch (link_jump_table(gb)) {
    case 0x5383: goto substate0;
    case 0x539d: goto substate1;
    case 0x53b0: goto substate2;
    case 0x53d1: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x5383, 0x5385); A = 0x01;
  CYC(0x5385, 0x5386); mem_wr(gb, DE, A);
  CYC(0x5386, 0x5387); H = D;
  CYC(0x5387, 0x5389); L = 0x06;
  CYC(0x5389, 0x538b); mem_wr(gb, HL, 0x08);
  CYC(0x538b, 0x538d); L = 0x10;
  CYC(0x538d, 0x538f); mem_wr(gb, HL, 0x50);
  CYC(0x538f, 0x5391); L = 0x09;
  CYC(0x5391, 0x5393); mem_wr(gb, HL, 0x00);
  CYC(0x5393, 0x5395); A = 0x81;
  CYC(0x5395, 0x5398); W8(wLinkInAir) = A;
  CYC(0x5398, 0x539a); A = 0x53;
  CALL_C(0x539a, playSound_b00_hook, 0x0c98, 0x539d);

substate1:
  CALL_C(0x539d, specialObjectUpdatePositionWithoutTileEdgeAdjust, 0x5d8e, 0x53a0);
  CALL_C(0x53a0, itemDecCounter1_hook, 0x23d6, 0x53a3);
  if (!(F & FZ)) { CYCT(0x53a3, 0x53a4); ret_effect(gb); return; }
  CYC(0x53a3, 0x53a4);
  CYC(0x53a4, 0x53a6); L = 0x05;
  CYC(0x53a6, 0x53a7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53a7, 0x53a9); L = 0x08;
  CYC(0x53a9, 0x53ab); mem_wr(gb, HL, 0x00);
  CYC(0x53ab, 0x53ad); A = 0x04;
  CALL_C(0x53ad, specialObjectSetAnimation_hook, 0x2b0a, 0x53b0);

substate2:
  CALL_C(0x53b0, specialObjectAnimate_hook, 0x2aef, 0x53b3);
  CYC(0x53b3, 0x53b6); A = W8(wScrollMode);
  CYC(0x53b6, 0x53b8); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x53b8, 0x53b9); ret_effect(gb); return; }
  CYC(0x53b8, 0x53b9);
  CALL_C(0x53b9, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x53bc);
  if (F & FC) {
    CYCT(0x53bc, 0x53bf);
    specialObjectUpdatePositionWithoutTileEdgeAdjust(gb);
    return;
  }
  CYC(0x53bc, 0x53bf);
  CYC(0x53bf, 0x53c2); SET_BC(0xfe00);
  CALL_C(0x53c2, objectSetSpeedZ_hook, 0x239d, 0x53c5);
  CYC(0x53c5, 0x53c7); L = 0x05;
  CYC(0x53c7, 0x53c8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53c8, 0x53ca); L = 0x10;
  CYC(0x53ca, 0x53cc); mem_wr(gb, HL, 0x0a);
  CYC(0x53cc, 0x53ce); A = 0x18;
  CALL_C(0x53ce, specialObjectSetAnimation_hook, 0x2b0a, 0x53d1);

substate3:
  CALL_C(0x53d1, specialObjectAnimate_hook, 0x2aef, 0x53d4);
  CALL_C(0x53d4, specialObjectUpdateAdjacentWallsBitset, 0x5e62, 0x53d7);
  CALL_C(0x53d7, specialObjectUpdatePosition, 0x5d97, 0x53da);
  CYC(0x53da, 0x53dc); C = 0x18;
  CALL_C(0x53dc, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x53df);
  if (!(F & FZ)) { CYCT(0x53df, 0x53e0); ret_effect(gb); return; }
  CYC(0x53df, 0x53e0);
  CYC(0x53e0, 0x53e1); alu_xor(gb, A);
  CYC(0x53e1, 0x53e4); W8(wLinkInAir) = A;
  CYC(0x53e4, 0x53e7); W8(wWarpsDisabled) = A;
  CYC(0x53e7, 0x53ea);
  initLinkStateAndAnimateStanding(gb);
}

void linkState09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53ea, 0x53ec); E = 0x05;
  CYC(0x53ec, 0x53ed); A = mem_rd(gb, DE);
  CYC(0x53ed, 0x53ee); push_effect(gb, 0x53ee);
  switch (link_jump_table(gb)) {
    case 0x53fa: goto substate0;
    case 0x540c: goto substate1;
    case 0x5419: goto substate2;
    case 0x5426: goto substate3;
    case 0x543e: goto substate4;
    case 0x5454: goto substate5;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x53fa, itemIncSubstate_hook, 0x23ef, 0x53fd);
  CYC(0x53fd, 0x53ff); L = 0x10;
  CYC(0x53ff, 0x5401); mem_wr(gb, HL, 0x28);
  CYC(0x5401, 0x5403); L = 0x08;
  CYC(0x5403, 0x5405); mem_wr(gb, HL, 0x03);
  CYC(0x5405, 0x5406); L = alu_inc8(gb, L);
  CYC(0x5406, 0x5408); mem_wr(gb, HL, 0x08);
  CYC(0x5408, 0x540a); L = 0x06;
  CYC(0x540a, 0x540c); mem_wr(gb, HL, 0x0c);

substate1:
  CALL_C(0x540c, itemDecCounter1_hook, 0x23d6, 0x540f);
  if (!(F & FZ)) { CYCT(0x540f, 0x5411); goto animate; }
  CYC(0x540f, 0x5411);
  CYC(0x5411, 0x5413); mem_wr(gb, HL, 0x0c);
  CYC(0x5413, 0x5414); L = E;
  CYC(0x5414, 0x5415); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5415, 0x5417); L = 0x09;
  CYC(0x5417, 0x5419); mem_wr(gb, HL, 0x18);

substate2:
  CALL_C(0x5419, itemDecCounter1_hook, 0x23d6, 0x541c);
  if (!(F & FZ)) { CYCT(0x541c, 0x541e); goto animate; }
  CYC(0x541c, 0x541e);
  CYC(0x541e, 0x5420); mem_wr(gb, HL, 0x32);
  CYC(0x5420, 0x5421); L = E;
  CYC(0x5421, 0x5422); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5422, 0x5424); L = 0x08;
  CYC(0x5424, 0x5426); mem_wr(gb, HL, 0x02);

substate3:
  CALL_C(0x5426, itemDecCounter1_hook, 0x23d6, 0x5429);
  if (!(F & FZ)) { CYCT(0x5429, 0x542a); ret_effect(gb); return; }
  CYC(0x5429, 0x542a);
  CYC(0x542a, 0x542b); L = E;
  CYC(0x542b, 0x542c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x542c, 0x542e); L = 0x08;
  CYC(0x542e, 0x5430); mem_wr(gb, HL, 0x00);
  CYC(0x5430, 0x5431); L = alu_inc8(gb, L);
  CYC(0x5431, 0x5433); mem_wr(gb, HL, 0x10);
  CYC(0x5433, 0x5435); L = 0x06;
  CYC(0x5435, 0x5437); A = 0x1e;
  CYC(0x5437, 0x5438); mem_wr(gb, HL, A);
  CYC(0x5438, 0x543b); SET_BC(0xf4f8);
  CYC(0x543b, 0x543e);
  objectCreateExclamationMark_hook(gb);
  return;

substate4:
  CALL_C(0x543e, itemDecCounter1_hook, 0x23d6, 0x5441);
  if (!(F & FZ)) { CYCT(0x5441, 0x5442); ret_effect(gb); return; }
  CYC(0x5441, 0x5442);
  CYC(0x5442, 0x5443); L = E;
  CYC(0x5443, 0x5444); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5444, 0x5447); SET_BC(0xfe80);
  CALL_C(0x5447, objectSetSpeedZ_hook, 0x239d, 0x544a);
  CYC(0x544a, 0x544c); A = 0x18;
  CALL_C(0x544c, specialObjectSetAnimation_hook, 0x2b0a, 0x544f);
  CYC(0x544f, 0x5451); A = 0x53;
  CYC(0x5451, 0x5454);
  playSound_b00_hook(gb);
  return;

substate5:
  CYC(0x5454, 0x5456); C = 0x18;
  CALL_C(0x5456, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5459);
  if (!(F & FZ)) { CYCT(0x5459, 0x545b); goto animate; }
  CYC(0x5459, 0x545b);
  CYC(0x545b, 0x545d); L = 0x05;
  CYC(0x545d, 0x545e); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x545e, 0x5460); mem_wr(gb, HL, 0x08);
  CYC(0x5460, 0x5461); ret_effect(gb);
  return;

animate:
  CALL_C(0x5461, specialObjectAnimate_hook, 0x2aef, 0x5464);
  CYC(0x5464, 0x5467);
  specialObjectUpdatePositionWithoutTileEdgeAdjust(gb);
}

void linkState11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5467, 0x5469); E = 0x05;
  CYC(0x5469, 0x546a); A = mem_rd(gb, DE);
  CYC(0x546a, 0x546b); push_effect(gb, 0x546b);
  switch (link_jump_table(gb)) {
    case 0x5471: goto substate0;
    case 0x548d: goto substate1;
    case 0x549c: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x5471, 0x5473); A = 0x01;
  CYC(0x5473, 0x5474); mem_wr(gb, DE, A);
  CALL_C(0x5474, linkCancelAllItemUsage_hook, 0x4f49, 0x5477);
  CYC(0x5477, 0x5478); alu_xor(gb, A);
  CYC(0x5478, 0x547a); E = 0x24;
  CYC(0x547a, 0x547b); mem_wr(gb, DE, A);
  CYC(0x547b, 0x547d); A = 0x4e;
  CALL_C(0x547d, playSound_b00_hook, 0x0c98, 0x5480);
  CYC(0x5480, 0x5483); A = W8(wcc50);
  CYC(0x5483, 0x5485); alu_and(gb, 0x7f);
  CYC(0x5485, 0x5487); A = 0x06;
  if (F & FZ) {
    CYCT(0x5487, 0x5489);
  } else {
    CYC(0x5487, 0x5489);
    CYC(0x5489, 0x548a); A = alu_inc8(gb, A);
  }
  CALL_C(0x548a, specialObjectSetAnimation_hook, 0x2b0a, 0x548d);

substate1:
  CALL_C(0x548d, specialObjectAnimate_hook, 0x2aef, 0x5490);
  CYC(0x5490, 0x5492); E = 0x21;
  CYC(0x5492, 0x5493); A = mem_rd(gb, DE);
  CYC(0x5493, 0x5494); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x5494, 0x5495); ret_effect(gb); return; }
  CYC(0x5494, 0x5495);
  CALL_C(0x5495, itemIncSubstate_hook, 0x23ef, 0x5498);
  CYC(0x5498, 0x549a); L = 0x06;
  CYC(0x549a, 0x549c); mem_wr(gb, HL, 0x14);

substate2:
  CALL_C(0x549c, specialObjectAnimate_hook, 0x2aef, 0x549f);
  CYC(0x549f, 0x54a2); A = W8(wFrameCounter);
  CYC(0x54a2, 0x54a3); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x54a3, 0x54a6);
    objectSetInvisible_hook(gb);
    return;
  }
  CYC(0x54a3, 0x54a6);
  CALL_C(0x54a6, objectSetVisible_hook, 0x1e84, 0x54a9);
  CALL_C(0x54a9, itemDecCounter1_hook, 0x23d6, 0x54ac);
  if (!(F & FZ)) { CYCT(0x54ac, 0x54ad); ret_effect(gb); return; }
  CYC(0x54ac, 0x54ad);
  CYC(0x54ad, 0x54b0); A = W8(wcc50);
  CYC(0x54b0, 0x54b2); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x54b2, 0x54b4); goto force_state; }
  CYC(0x54b2, 0x54b4);
  CALL_C(0x54b4, respawnLink_hook, 0x2ae3, 0x54b7);
  CYC(0x54b7, 0x54b9);
  checkLinkForceState(gb);
  return;

force_state:
  CYC(0x54b9, 0x54bb); A = 0x03;
  CYC(0x54bb, 0x54be); W8(wLinkForceState) = A;
  CYC(0x54be, 0x54c0);
  checkLinkForceState(gb);
}
