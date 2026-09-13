#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define impaCutsceneState wTmpcfc0_genericCutscene_cfd0
#define impaCutsceneState_BANK wTmpcfc0_genericCutscene_cfd0_BANK

void impaState1_hook(GB *gb);
void impaLoadCollapsedGraphic_hook(GB *gb);
void impaRet_hook(GB *gb);
void impaSubid1_hook(GB *gb);
void interactionOscillateXRandomly_hook(GB *gb);
void impaSubid1Substate2_hook(GB *gb);
void impaSubid2_hook(GB *gb);
void impaAnimateAndRunScript_hook(GB *gb);
void impaSubid2Substate4_hook(GB *gb);
void impaSetVisibleAndJump_hook(GB *gb);
void impaSubid2Substate5_hook(GB *gb);
void impaSubid2Substate6_hook(GB *gb);
void impaSubid2Substate7_hook(GB *gb);
void impaSubid4_hook(GB *gb);
void impaSubid5_hook(GB *gb);
void impaSubid7_hook(GB *gb);
void impaSubid8_hook(GB *gb);
void impaSubid9_hook(GB *gb);
void checkObjectIsCloseToPosition_b08_hook(GB *gb);
void impaUpdateAnimationIfDirectionChanged_hook(GB *gb);
void impaCheckApproachedStone_hook(GB *gb);
void impaAnimateAndDecCounter1_hook(GB *gb);
void impaPreventLinkFromLeavingStoneScreen_hook(GB *gb);

static uint16_t impa_jump_table(GB *gb) {
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

void impaState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b44, 0x5b46); E = 0x42;
  CYC(0x5b46, 0x5b47); A = mem_rd(gb, DE);
  CYC(0x5b47, 0x5b48); push_effect(gb, 0x5b48);
  switch (impa_jump_table(gb)) {
    case 0x5b5e: impaSubid0(gb); return;
    case 0x5d5f: impaSubid1_hook(gb); return;
    case 0x5da6: impaSubid2_hook(gb); return;
    case 0x5df2: impaAnimateAndRunScript_hook(gb); return;
    case 0x5e5b: impaSubid4_hook(gb); return;
    case 0x5f04: impaSubid5_hook(gb); return;
    case 0x5f50: impaSubid7_hook(gb); return;
    case 0x5f6e: impaSubid8_hook(gb); return;
    case 0x5f75: impaSubid9_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void impaLoadCollapsedGraphic_hook(GB *gb) {
  CYC(0x5d56, 0x5d58); L = 0x5c;
  CYC(0x5d58, 0x5d5a); mem_wr(gb, HL, 0x0a);
  CYC(0x5d5a, 0x5d5c); L = 0x5d;
  CYC(0x5d5c, 0x5d5e); mem_wr(gb, HL, 0x60);
  impaRet_hook(gb);
}

void impaRet_hook(GB *gb) {
  CYC(0x5d5e, 0x5d5f);
  ret_effect(gb);
}

void impaSubid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d5f, 0x5d61); E = 0x45;
  CYC(0x5d61, 0x5d62); A = mem_rd(gb, DE);
  CYC(0x5d62, 0x5d63); push_effect(gb, 0x5d63);
  switch (impa_jump_table(gb)) {
    case 0x5d69: goto substate0;
    case 0x5d7f: goto substate1;
    case 0x5d96: impaSubid1Substate2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x5d69, 0x5d6c); A = W8(impaCutsceneState);
  CYC(0x5d6c, 0x5d6e); alu_cp(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x5d6e, 0x5d71); interactionAnimate_hook(gb); return; }
  CYC(0x5d6e, 0x5d71);
  CALL_C(0x5d71, interactionIncSubstate_hook, 0x23e5, 0x5d74);
  CYC(0x5d74, 0x5d76); E = 0x4d;
  CYC(0x5d76, 0x5d77); A = mem_rd(gb, DE);
  CYC(0x5d77, 0x5d79); L = 0x7d;
  CYC(0x5d79, 0x5d7a); mem_wr(gb, HL, A);
  CYC(0x5d7a, 0x5d7c); L = 0x46;
  CYC(0x5d7c, 0x5d7e); mem_wr(gb, HL, 0x3c);
  CYC(0x5d7e, 0x5d7f); ret_effect(gb); return;
substate1:
  CALL_C(0x5d7f, interactionDecCounter1_hook, 0x23cc, 0x5d82);
  if (!(F & FZ)) { CYCT(0x5d82, 0x5d84); interactionOscillateXRandomly_hook(gb); return; }
  CYC(0x5d82, 0x5d84);
  CYC(0x5d84, 0x5d87); interactionIncSubstate_hook(gb);
}

void interactionOscillateXRandomly_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d87, getRandomNumber_hook, 0x043e, 0x5d8a);
  CYC(0x5d8a, 0x5d8c); alu_and(gb, 0x01);
  CYC(0x5d8c, 0x5d8e); alu_sub(gb, 0x01);
  CYC(0x5d8e, 0x5d8f); H = D;
  CYC(0x5d8f, 0x5d91); L = 0x7d;
  CYC(0x5d91, 0x5d92); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5d92, 0x5d94); L = 0x4d;
  CYC(0x5d94, 0x5d95); mem_wr(gb, HL, A);
  CYC(0x5d95, 0x5d96); ret_effect(gb);
}

void impaSubid1Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d96, interactionRunScript_hook, 0x2552, 0x5d99);
  if (F & FC) { CYCT(0x5d99, 0x5d9c); interactionDelete_hook(gb); return; }
  CYC(0x5d99, 0x5d9c);
  CYC(0x5d9c, 0x5d9e); E = 0x47;
  CYC(0x5d9e, 0x5d9f); A = mem_rd(gb, DE);
  CYC(0x5d9f, 0x5da0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5da0, 0x5da3); interactionAnimate2Times_hook(gb); return; }
  CYC(0x5da0, 0x5da3);
  CYC(0x5da3, 0x5da6); interactionAnimate_hook(gb);
}

void impaSubid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5da6, 0x5da8); E = 0x45;
  CYC(0x5da8, 0x5da9); A = mem_rd(gb, DE);
  CYC(0x5da9, 0x5daa); push_effect(gb, 0x5daa);
  switch (impa_jump_table(gb)) {
    case 0x5dba: goto substate0;
    case 0x5dcb: goto substate1;
    case 0x5ddb: goto substate2;
    case 0x5df2: impaAnimateAndRunScript_hook(gb); return;
    case 0x5df8: impaSubid2Substate4_hook(gb); return;
    case 0x5e0d: impaSubid2Substate5_hook(gb); return;
    case 0x5e23: impaSubid2Substate6_hook(gb); return;
    case 0x5e4a: impaSubid2Substate7_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x5dba, interactionDecCounter1IfPaletteNotFading_hook, 0x2744, 0x5dbd);
  if (!(F & FZ)) { CYCT(0x5dbd, 0x5dbe); ret_effect(gb); return; }
  CYC(0x5dbd, 0x5dbe);
  CYC(0x5dbe, 0x5dc0); mem_wr(gb, HL, 0x3c);
  CALL_C(0x5dc0, interactionIncSubstate_hook, 0x23e5, 0x5dc3);
  CYC(0x5dc3, 0x5dc5); A = 0x50;
  CYC(0x5dc5, 0x5dc8); SET_BC(0x6050);
  CYC(0x5dc8, 0x5dcb); createEnergySwirlGoingIn_hook(gb);
  return;
substate1:
  CALL_C(0x5dcb, interactionDecCounter1_hook, 0x23cc, 0x5dce);
  if (!(F & FZ)) { CYCT(0x5dce, 0x5dcf); ret_effect(gb); return; }
  CYC(0x5dce, 0x5dcf);
  CYC(0x5dcf, 0x5dd2); SET_HL(wTmpcbb3);
  CYC(0x5dd2, 0x5dd3); alu_xor(gb, A);
  CYC(0x5dd3, 0x5dd4); mem_wr(gb, HL, A);
  CYC(0x5dd4, 0x5dd5); A = alu_dec8(gb, A);
  CYC(0x5dd5, 0x5dd8); W8(wTmpcbba) = A;
  CYC(0x5dd8, 0x5ddb); interactionIncSubstate_hook(gb);
  return;
substate2:
  CYC(0x5ddb, 0x5dde); SET_HL(wTmpcbb3);
  CYC(0x5dde, 0x5de0); B = 0x02;
  CALL_C(0x5de0, flashScreen_hook, 0x2d73, 0x5de3);
  if (F & FZ) { CYCT(0x5de3, 0x5de4); ret_effect(gb); return; }
  CYC(0x5de3, 0x5de4);
  CALL_C(0x5de4, interactionIncSubstate_hook, 0x23e5, 0x5de7);
  CALL_C(0x5de7, interactionCode31__loadScript, 0x5a8c, 0x5dea);
  CYC(0x5dea, 0x5dec); A = 0x01;
  CYC(0x5dec, 0x5def); W8(wTmpcfc0) = A;
  CYC(0x5def, 0x5df2); fadeinFromWhite_hook(gb);
}

void impaAnimateAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5df2, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5df5);
  CYC(0x5df5, 0x5df8); interactionRunScript_hook(gb);
}

void impaSubid2Substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5df8, 0x5df9); H = D;
  CYC(0x5df9, 0x5dfb); L = 0x78;
  CYC(0x5dfb, 0x5dfc); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x5dfc, 0x5dfd); ret_effect(gb); return; }
  CYC(0x5dfc, 0x5dfd);
  CALL_C(0x5dfd, interactionIncSubstate_hook, 0x23e5, 0x5e00);
  CYC(0x5e00, 0x5e02); L = 0x46;
  CYC(0x5e02, 0x5e04); mem_wr(gb, HL, 0x02);
  impaSetVisibleAndJump_hook(gb);
}

void impaSetVisibleAndJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e04, objectSetVisiblec2_hook, 0x1e45, 0x5e07);
  CYC(0x5e07, 0x5e0a); SET_BC(0xfe80);
  CYC(0x5e0a, 0x5e0d); objectSetSpeedZ_hook(gb);
}

void impaSubid2Substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e0d, 0x5e0f); C = 0x20;
  CALL_C(0x5e0f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5e12);
  if (!(F & FZ)) { CYCT(0x5e12, 0x5e13); ret_effect(gb); return; }
  CYC(0x5e12, 0x5e13);
  CALL_C(0x5e13, interactionDecCounter1_hook, 0x23cc, 0x5e16);
  if (!(F & FZ)) { CYCT(0x5e16, 0x5e18); impaSetVisibleAndJump_hook(gb); return; }
  CYC(0x5e16, 0x5e18);
  CALL_C(0x5e18, objectSetVisible82_hook, 0x1e69, 0x5e1b);
  CYC(0x5e1b, 0x5e1c); H = D;
  CYC(0x5e1c, 0x5e1e); L = 0x78;
  CYC(0x5e1e, 0x5e20); mem_wr(gb, HL, 0x10);
  CYC(0x5e20, 0x5e23); interactionIncSubstate_hook(gb);
}

void impaSubid2Substate6_hook(GB *gb) {
  CYC(0x5e23, 0x5e24); H = D;
  CYC(0x5e24, 0x5e26); L = 0x78;
  CYC(0x5e26, 0x5e27); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x5e27, 0x5e28); ret_effect(gb); return; }
  CYC(0x5e27, 0x5e28);
  CYC(0x5e28, 0x5e2a); mem_wr(gb, HL, 0x10);
  CYC(0x5e2a, 0x5e2c); L = 0x47;
  CYC(0x5e2c, 0x5e2d); A = mem_rd(gb, HL);
  CYC(0x5e2d, 0x5e2e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5e2e, 0x5e30); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x5e30, 0x5e32); goto next_state; }
  CYC(0x5e30, 0x5e32);
  CYC(0x5e32, 0x5e33); alu_or(gb, A);
  CYC(0x5e33, 0x5e35); A = 0x03;
  if (F & FZ) { CYCT(0x5e35, 0x5e37); goto set_animation; }
  CYC(0x5e35, 0x5e37);
  CYC(0x5e37, 0x5e39); alu_xor(gb, 0x02);
set_animation:
  CYC(0x5e39, 0x5e3b); L = 0x45;
  CYC(0x5e3b, 0x5e3c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e3c, 0x5e3d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e3d, 0x5e40); interactionSetAnimation_hook(gb);
  return;
next_state:
  CYC(0x5e40, 0x5e42); mem_wr(gb, HL, 0x00);
  CYC(0x5e42, 0x5e44); A = 0x02;
  CYC(0x5e44, 0x5e47); W8(wTmpcfc0) = A;
  CYC(0x5e47, 0x5e4a); interactionIncSubstate_hook(gb);
}

void impaSubid2Substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e4a, impaAnimateAndRunScript_hook, 0x5df2, 0x5e4d);
  CYC(0x5e4d, 0x5e50); A = W8(wTmpcfc0);
  CYC(0x5e50, 0x5e52); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x5e52, 0x5e53); ret_effect(gb); return; }
  CYC(0x5e52, 0x5e53);
  CYC(0x5e53, 0x5e56); SET_HL(0x5613);
  CYC(0x5e56, 0x5e58); E = 0x15;
  CYC(0x5e58, 0x5e5b); interBankCall_hook(gb);
}

void impaSubid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e5b, checkInteractionSubstate_hook, 0x2403, 0x5e5e);
  if (!(F & FZ)) { CYCT(0x5e5e, 0x5e60); goto substate1; }
  CYC(0x5e5e, 0x5e60);
  CYC(0x5e60, 0x5e63); SET_HL(w1Link_yh);
  CYC(0x5e63, 0x5e64); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5e64, 0x5e66); alu_cp(gb, 0x60);
  if (F & FC) { CYCT(0x5e66, 0x5e67); ret_effect(gb); return; }
  CYC(0x5e66, 0x5e67);
  CYC(0x5e67, 0x5e69); L = 0x0f;
  CYC(0x5e69, 0x5e6b); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5e6b, 0x5e6c); ret_effect(gb); return; }
  CYC(0x5e6b, 0x5e6c);
  CALL_C(0x5e6c, checkLinkCollisionsEnabled_hook, 0x1d32, 0x5e6f);
  if (!(F & FC)) { CYCT(0x5e6f, 0x5e70); ret_effect(gb); return; }
  CYC(0x5e6f, 0x5e70);
  CALL_C(0x5e70, resetLinkInvincibility_hook, 0x2ba9, 0x5e73);
  CALL_C(0x5e73, setLinkForceStateToState08_hook, 0x2aad, 0x5e76);
  CYC(0x5e76, 0x5e77); A = alu_inc8(gb, A);
  CYC(0x5e77, 0x5e7a); W8(wDisabledObjects) = A;
  CYC(0x5e7a, 0x5e7d); W8(wMenuDisabled) = A;
  CYC(0x5e7d, 0x5e80); interactionIncSubstate_hook(gb);
  return;
substate1:
  CYC(0x5e80, 0x5e82); C = 0x20;
  CALL_C(0x5e82, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5e85);
  if (!(F & FZ)) { CYCT(0x5e85, 0x5e86); ret_effect(gb); return; }
  CYC(0x5e85, 0x5e86);
  CALL_C(0x5e86, interactionRunScript_hook, 0x2552, 0x5e89);
  if (F & FC) { CYCT(0x5e89, 0x5e8c); interactionDelete_hook(gb); return; }
  CYC(0x5e89, 0x5e8c);
  CALL_C(0x5e8c, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5e8f);
  CYC(0x5e8f, 0x5e91); E = 0x78;
  CYC(0x5e91, 0x5e92); A = mem_rd(gb, DE);
  CYC(0x5e92, 0x5e93); push_effect(gb, 0x5e93);
  switch (impa_jump_table(gb)) {
    case 0x5e9d: goto thing0;
    case 0x5ea9: goto thing1;
    case 0x5edb: goto thing2;
    case 0x5ef7: goto thing3;
    case 0x5f03: goto thing4;
    default: hook_continue(gb, HL, sp0_); return;
  }
thing0:
  CYC(0x5e9d, 0x5ea0); A = W8(wTmpcfc0);
  CYC(0x5ea0, 0x5ea1); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x5ea1, 0x5ea2); ret_effect(gb); return; }
  CYC(0x5ea1, 0x5ea2);
  CYC(0x5ea2, 0x5ea4); E = 0x79;
  CYC(0x5ea4, 0x5ea6); A = 0x10;
  CYC(0x5ea6, 0x5ea7); mem_wr(gb, DE, A);
  CYC(0x5ea7, 0x5ea9);
  goto increment_var38;
thing1:
  CYC(0x5ea9, 0x5eaa); H = D;
  CYC(0x5eaa, 0x5eac); L = 0x79;
  CYC(0x5eac, 0x5ead); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x5ead, 0x5eae); ret_effect(gb); return; }
  CYC(0x5ead, 0x5eae);
  CYC(0x5eae, 0x5eb1); A = W8(w1Link_xh);
  CYC(0x5eb1, 0x5eb3); alu_sub(gb, 0x50);
  CYC(0x5eb3, 0x5eb4); B = A;
  CYC(0x5eb4, 0x5eb6); alu_add(gb, 0x02);
  CYC(0x5eb6, 0x5eb8); alu_cp(gb, 0x05);
  if (F & FC) { CYCT(0x5eb8, 0x5eba); goto increment_var38; }
  CYC(0x5eb8, 0x5eba);
  CYC(0x5eba, 0x5ebb); A = B;
  CYC(0x5ebb, 0x5ebd); alu_bit(gb, 7, A);
  CYC(0x5ebd, 0x5ebf); B = 0x18;
  if (F & FZ) { CYCT(0x5ebf, 0x5ec1); goto move_link; }
  CYC(0x5ebf, 0x5ec1);
  CYC(0x5ec1, 0x5ec3); B = 0x08;
  CYC(0x5ec3, 0x5ec4); alu_cpl(gb);
  CYC(0x5ec4, 0x5ec5); A = alu_inc8(gb, A);
move_link:
  CYC(0x5ec5, 0x5ec8); W8(wLinkStateParameter) = A;
  CYC(0x5ec8, 0x5eca); A = 0x0b;
  CYC(0x5eca, 0x5ecd); W8(wLinkForceState) = A;
  CYC(0x5ecd, 0x5ed0); SET_HL(w1Link_angle);
  CYC(0x5ed0, 0x5ed1); A = B;
  CYC(0x5ed1, 0x5ed2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5ed2, 0x5ed4); A = alu_swap(gb, A);
  CYC(0x5ed4, 0x5ed5); alu_rlca(gb);
  CYC(0x5ed5, 0x5ed6); mem_wr(gb, HL, A);
increment_var38:
  CYC(0x5ed6, 0x5ed7); H = D;
  CYC(0x5ed7, 0x5ed9); L = 0x78;
  CYC(0x5ed9, 0x5eda); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5eda, 0x5edb); ret_effect(gb); return;
thing2:
  CYC(0x5edb, 0x5ede); A = W8(w1Link_state);
  CYC(0x5ede, 0x5ee0); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(0x5ee0, 0x5ee1); ret_effect(gb); return; }
  CYC(0x5ee0, 0x5ee1);
  CYC(0x5ee1, 0x5ee4); A = W8(w1Link_yh);
  CYC(0x5ee4, 0x5ee6); alu_sub(gb, 0x48);
  CYC(0x5ee6, 0x5ee9); W8(wLinkStateParameter) = A;
  CYC(0x5ee9, 0x5eea); alu_xor(gb, A);
  CYC(0x5eea, 0x5eed); SET_HL(w1Link_direction);
  CYC(0x5eed, 0x5eee); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5eee, 0x5eef); mem_wr(gb, HL, A);
  CYC(0x5eef, 0x5ef1); A = 0x0b;
  CYC(0x5ef1, 0x5ef4); W8(wLinkForceState) = A;
  CYC(0x5ef4, 0x5ef7);
  goto increment_var38;
thing3:
  CYC(0x5ef7, 0x5efa); A = W8(w1Link_state);
  CYC(0x5efa, 0x5efc); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(0x5efc, 0x5efd); ret_effect(gb); return; }
  CYC(0x5efc, 0x5efd);
  CALL_C(0x5efd, setLinkForceStateToState08_hook, 0x2aad, 0x5f00);
  CYC(0x5f00, 0x5f03);
  goto increment_var38;
thing4:
  CYC(0x5f03, 0x5f04); ret_effect(gb);
}

void impaSubid5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f04, 0x5f06); C = 0x20;
  CALL_C(0x5f06, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5f09);
  if (!(F & FZ)) { CYCT(0x5f09, 0x5f0a); ret_effect(gb); return; }
  CYC(0x5f09, 0x5f0a);
  CALL_C(0x5f0a, interactionRunScript_hook, 0x2552, 0x5f0d);
  if (!(F & FC)) { CYCT(0x5f0d, 0x5f0f); goto continue_script; }
  CYC(0x5f0d, 0x5f0f);
  CYC(0x5f0f, 0x5f10); alu_xor(gb, A);
  CYC(0x5f10, 0x5f13); W8(wDisabledObjects) = A;
  CYC(0x5f13, 0x5f16); W8(wMenuDisabled) = A;
  CYC(0x5f16, 0x5f18); A = 0x33;
  CALL_C(0x5f18, setGlobalFlag_hook, 0x31f9, 0x5f1b);
  CYC(0x5f1b, 0x5f1e); interactionDelete_hook(gb);
  return;
continue_script:
  CALL_C(0x5f1e, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5f21);
  CYC(0x5f21, 0x5f23); E = 0x45;
  CYC(0x5f23, 0x5f24); A = mem_rd(gb, DE);
  CYC(0x5f24, 0x5f25); push_effect(gb, 0x5f25);
  switch (impa_jump_table(gb)) {
    case 0x5f2b: goto substate0;
    case 0x5f43: goto substate1;
    case 0x5f4f: CYC(0x5f4f, 0x5f50); ret_effect(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x5f2b, 0x5f2e); A = W8(impaCutsceneState);
  CYC(0x5f2e, 0x5f30); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x5f30, 0x5f31); ret_effect(gb); return; }
  CYC(0x5f30, 0x5f31);
  CYC(0x5f31, 0x5f33); A = 0x29;
  CYC(0x5f33, 0x5f36); W8(wLinkStateParameter) = A;
  CYC(0x5f36, 0x5f38); A = 0x0b;
  CYC(0x5f38, 0x5f3b); W8(wLinkForceState) = A;
  CYC(0x5f3b, 0x5f3d); A = 0x10;
  CYC(0x5f3d, 0x5f40); W8(w1Link_angle) = A;
  CYC(0x5f40, 0x5f43); interactionIncSubstate_hook(gb);
  return;
substate1:
  CYC(0x5f43, 0x5f46); A = W8(w1Link_state);
  CYC(0x5f46, 0x5f48); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(0x5f48, 0x5f49); ret_effect(gb); return; }
  CYC(0x5f48, 0x5f49);
  CALL_C(0x5f49, setLinkForceStateToState08_hook, 0x2aad, 0x5f4c);
  CYC(0x5f4c, 0x5f4f); interactionIncSubstate_hook(gb);
}

void impaSubid7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f50, 0x5f52); C = 0x20;
  CALL_C(0x5f52, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5f55);
  CALL_C(0x5f55, interactionRunScript_hook, 0x2552, 0x5f58);
  if (F & FC) { CYCT(0x5f58, 0x5f5b); interactionDelete_hook(gb); return; }
  CYC(0x5f58, 0x5f5b);
  CYC(0x5f5b, 0x5f5d); A = 0x39;
  CALL_C(0x5f5d, checkGlobalFlag_hook, 0x31f3, 0x5f60);
  if (F & FZ) { CYCT(0x5f60, 0x5f63); interactionAnimateAsNpc_hook(gb); return; }
  CYC(0x5f60, 0x5f63);
  CYC(0x5f63, 0x5f65); A = 0x3c;
  CALL_C(0x5f65, checkGlobalFlag_hook, 0x31f3, 0x5f68);
  if (!(F & FZ)) { CYCT(0x5f68, 0x5f6b); interactionAnimate_hook(gb); return; }
  CYC(0x5f68, 0x5f6b);
  CYC(0x5f6b, 0x5f6e); npcFaceLinkAndAnimate_hook(gb);
}

void impaSubid8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f6e, impaAnimateAndRunScript_hook, 0x5df2, 0x5f71);
  if (F & FC) { CYCT(0x5f71, 0x5f74); interactionDelete_hook(gb); return; }
  CYC(0x5f71, 0x5f74);
  CYC(0x5f74, 0x5f75); ret_effect(gb);
}

void impaSubid9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f75, 0x5f77); E = 0x78;
  CYC(0x5f77, 0x5f78); A = mem_rd(gb, DE);
  CYC(0x5f78, 0x5f79); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5f79, 0x5f7b); goto animate; }
  CYC(0x5f79, 0x5f7b);
  CYC(0x5f7b, 0x5f7e); SET_HL(0x741b);
  CYC(0x5f7e, 0x5f80); E = 0x15;
  CALL_C(0x5f80, interBankCall_hook, 0x008a, 0x5f83);
animate:
  CYC(0x5f83, 0x5f86); impaAnimateAndRunScript_hook(gb);
}

void checkObjectIsCloseToPosition_b08_hook(GB *gb) {
  CYC(0x5f86, 0x5f87); push_effect(gb, HL);
  CYC(0x5f87, 0x5f8a); push_effect(gb, 0x5f8a);
  CYC(0x5f8f, 0x5f90); A = B;
  CYC(0x5f90, 0x5f91); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5f91, 0x5f94); SET_HL(hFF8B);
  CYC(0x5f94, 0x5f95); B = mem_rd(gb, HL);
  CYC(0x5f95, 0x5f96); alu_add(gb, B);
  CYC(0x5f96, 0x5f98); H8(hFF8D) = A;
  CYC(0x5f98, 0x5f99); A = B;
  CYC(0x5f99, 0x5f9a); alu_add(gb, A);
  CYC(0x5f9a, 0x5f9b); B = A;
  CYC(0x5f9b, 0x5f9c); B = alu_inc8(gb, B);
  CYC(0x5f9c, 0x5f9e); A = H8(hFF8D);
  CYC(0x5f9e, 0x5f9f); alu_cp(gb, B);
  CYC(0x5f9f, 0x5fa0); ret_effect(gb);
  CYC(0x5f8a, 0x5f8b); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(0x5f8b, 0x5f8c); ret_effect(gb); return; }
  CYC(0x5f8b, 0x5f8c);
  CYC(0x5f8c, 0x5f8d); L = alu_inc8(gb, L);
  CYC(0x5f8d, 0x5f8e); L = alu_inc8(gb, L);
  CYC(0x5f8e, 0x5f8f); B = C;
  CYC(0x5f8f, 0x5f90); A = B;
  CYC(0x5f90, 0x5f91); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5f91, 0x5f94); SET_HL(hFF8B);
  CYC(0x5f94, 0x5f95); B = mem_rd(gb, HL);
  CYC(0x5f95, 0x5f96); alu_add(gb, B);
  CYC(0x5f96, 0x5f98); H8(hFF8D) = A;
  CYC(0x5f98, 0x5f99); A = B;
  CYC(0x5f99, 0x5f9a); alu_add(gb, A);
  CYC(0x5f9a, 0x5f9b); B = A;
  CYC(0x5f9b, 0x5f9c); B = alu_inc8(gb, B);
  CYC(0x5f9c, 0x5f9e); A = H8(hFF8D);
  CYC(0x5f9e, 0x5f9f); alu_cp(gb, B);
  CYC(0x5f9f, 0x5fa0); ret_effect(gb);
}

void impaUpdateAnimationIfDirectionChanged_hook(GB *gb) {
  CYC(0x5fa0, 0x5fa1); H = D;
  CYC(0x5fa1, 0x5fa3); L = 0x48;
  CYC(0x5fa3, 0x5fa4); A = mem_rd(gb, HL);
  CYC(0x5fa4, 0x5fa6); L = 0x79;
  CYC(0x5fa6, 0x5fa7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5fa7, 0x5fa8); ret_effect(gb); return; }
  CYC(0x5fa7, 0x5fa8);
  CYC(0x5fa8, 0x5fa9); mem_wr(gb, HL, A);
  CYC(0x5fa9, 0x5fac); interactionSetAnimation_hook(gb);
}

void impaCheckApproachedStone_hook(GB *gb) {
  CYC(0x5fac, 0x5faf); A = W8(wActiveRoom);
  CYC(0x5faf, 0x5fb1); alu_cp(gb, 0x59);
  if (!(F & FZ)) { CYCT(0x5fb1, 0x5fb3); goto not_close; }
  CYC(0x5fb1, 0x5fb3);
  CYC(0x5fb3, 0x5fb6); A = W8(wScrollMode);
  CYC(0x5fb6, 0x5fb8); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x5fb8, 0x5fb9); ret_effect(gb); return; }
  CYC(0x5fb8, 0x5fb9);
  CYC(0x5fb9, 0x5fbc); SET_HL(w1Link_yh);
  CYC(0x5fbc, 0x5fbd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fbd, 0x5fbf); alu_cp(gb, 0x58);
  if (!(F & FC)) { CYCT(0x5fbf, 0x5fc1); goto not_close; }
  CYC(0x5fbf, 0x5fc1);
  CYC(0x5fc1, 0x5fc2); L = alu_inc8(gb, L);
  CYC(0x5fc2, 0x5fc3); A = mem_rd(gb, HL);
  CYC(0x5fc3, 0x5fc5); alu_cp(gb, 0x78);
  CYC(0x5fc5, 0x5fc6); ret_effect(gb); return;
not_close:
  CYC(0x5fc6, 0x5fc7); alu_xor(gb, A);
  CYC(0x5fc7, 0x5fc8); ret_effect(gb);
}

void impaAnimateAndDecCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fc8, 0x5fc9); H = D;
  CYC(0x5fc9, 0x5fcb); L = 0x46;
  CYC(0x5fcb, 0x5fcc); A = mem_rd(gb, HL);
  CYC(0x5fcc, 0x5fcd); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5fcd, 0x5fce); ret_effect(gb); return; }
  CYC(0x5fcd, 0x5fce);
  CYC(0x5fce, 0x5fcf); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x5fcf, interactionAnimate_hook, 0x261b, 0x5fd2);
  CYC(0x5fd2, 0x5fd4); alu_or(gb, 0x01);
  CYC(0x5fd4, 0x5fd5); ret_effect(gb);
}

void impaPreventLinkFromLeavingStoneScreen_hook(GB *gb) {
  CYC(0x5fd5, 0x5fd8); SET_HL(w1Link_yh);
  CYC(0x5fd8, 0x5fd9); A = mem_rd(gb, HL);
  CYC(0x5fd9, 0x5fdb); B = 0x76;
  CYC(0x5fdb, 0x5fdc); alu_cp(gb, B);
  if (F & FC) { CYCT(0x5fdc, 0x5fde); goto check_x; }
  CYC(0x5fdc, 0x5fde);
  CYC(0x5fde, 0x5fe1); A = W8(wKeysPressed);
  CYC(0x5fe1, 0x5fe3); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x5fe3, 0x5fe5); goto show_text; }
  CYC(0x5fe3, 0x5fe5);
check_x:
  CYC(0x5fe5, 0x5fe7); L = 0x0d;
  CYC(0x5fe7, 0x5fe8); A = mem_rd(gb, HL);
  CYC(0x5fe8, 0x5fea); B = 0x96;
  CYC(0x5fea, 0x5feb); alu_cp(gb, B);
  if (F & FC) { CYCT(0x5feb, 0x5fec); ret_effect(gb); return; }
  CYC(0x5feb, 0x5fec);
  CYC(0x5fec, 0x5fef); A = W8(wKeysPressed);
  CYC(0x5fef, 0x5ff1); alu_and(gb, 0x10);
  if (F & FZ) { CYCT(0x5ff1, 0x5ff2); ret_effect(gb); return; }
  CYC(0x5ff1, 0x5ff2);
show_text:
  CYC(0x5ff2, 0x5ff3); mem_wr(gb, HL, B);
  CYC(0x5ff3, 0x5ff6); SET_BC(0x010a);
  CYC(0x5ff6, 0x5ff9); showText_hook(gb);
}
