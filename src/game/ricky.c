#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

void rickyState0_hook(GB *gb);
void rickyState1_hook(GB *gb);
void rickyCheckHazards_hook(GB *gb);
void rickyState9_hook(GB *gb);
void rickyState2_hook(GB *gb);
void rickyState3_hook(GB *gb);
void rickyState4_hook(GB *gb);
void rickyState5_hook(GB *gb);
void rickyState5Substate0_hook(GB *gb);

static uint16_t ricky_jump_table(GB *gb) {
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

static void ricky_run_state(GB *gb, uint16_t sp0_) {
  CYC(0x6d2a, 0x6d2c); E = 0x04;
  CYC(0x6d2c, 0x6d2d); A = mem_rd(gb, DE);
  CYC(0x6d2d, 0x6d2e); push_effect(gb, 0x6d2e);
  switch (ricky_jump_table(gb)) {
    case 0x6d48: rickyState0_hook(gb); return;
    case 0x6d81: rickyState1_hook(gb); return;
    case 0x6dac: rickyState2_hook(gb); return;
    case 0x6dd7: rickyState3_hook(gb); return;
    case 0x6ded: rickyState4_hook(gb); return;
    case 0x6e24: rickyState5_hook(gb); return;
    case 0x705a: rickyState6(gb); return;
    case 0x7090: rickyState7(gb); return;
    case 0x6f9c: rickyState8(gb); return;
    case 0x6dab: rickyState9_hook(gb); return;
    case 0x70dd: rickyStateA(gb); return;
    case 0x726d: rickyStateC(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void specialObjectCode_ricky_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d1e, companionRetIfInactive_hook, 0x4883, 0x6d21);
  CALL_C(0x6d21, companionFunc_47d8_hook, 0x47d8, 0x6d24);
  CYC(0x6d24, 0x6d27); push_effect(gb, 0x6d27); ricky_run_state(gb, gb->sp);
  CYC(0x6d27, 0x6d2a); companionCheckEnableTerrainEffects_hook(gb);
}

void rickyState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d48, companionCheckCanSpawn_hook, 0x4822, 0x6d4b);
  CYC(0x6d4b, 0x6d4d); A = 0x06;
  CALL_C(0x6d4d, objectSetCollideRadius_hook, 0x24a1, 0x6d50);
  CYC(0x6d50, 0x6d52); A = 0x02;
  CYC(0x6d52, 0x6d54); L = 0x08;
  CYC(0x6d54, 0x6d55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d55, 0x6d56); mem_wr(gb, HL, A);
  CYC(0x6d56, 0x6d58); L = 0x39;
  CYC(0x6d58, 0x6d5a); mem_wr(gb, HL, 0x10);
  CYC(0x6d5a, 0x6d5d); A = W8(wRickyState);
  CYC(0x6d5d, 0x6d5f); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6d5f, 0x6d61);
    goto setAnimation17;
  }
  CYC(0x6d5f, 0x6d61);
  CYC(0x6d61, 0x6d63); C = 0x17;
  CYC(0x6d63, 0x6d65); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x6d65, 0x6d67);
    goto canTalk;
  }
  CYC(0x6d65, 0x6d67);
  CYC(0x6d67, 0x6d69); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x6d69, 0x6d6b);
    goto setAnimation17;
  }
  CYC(0x6d69, 0x6d6b);
  CYC(0x6d6b, 0x6d6d); C = 0x00;
canTalk:
  CYC(0x6d6d, 0x6d6f); L = 0x04;
  CYC(0x6d6f, 0x6d71); mem_wr(gb, HL, 0x0a);
  CYC(0x6d71, 0x6d73); E = 0x3d;
  CALL_C(0x6d73, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x6d76);
  CYC(0x6d76, 0x6d77); A = C;
  CYC(0x6d77, 0x6d79);
  goto setAnimation;
setAnimation17:
  CYC(0x6d79, 0x6d7b); A = 0x17;
setAnimation:
  CALL_C(0x6d7b, specialObjectSetAnimation_hook, 0x2b0a, 0x6d7e);
  CYC(0x6d7e, 0x6d81); objectSetVisiblec1_hook(gb);
}

void rickyState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d81, specialObjectAnimate_hook, 0x2aef, 0x6d84);
  CALL_C(0x6d84, companionSetPriorityRelativeToLink_hook, 0x4936, 0x6d87);
  CYC(0x6d87, 0x6d89); C = 0x09;
  CALL_C(0x6d89, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6d8c);
  if (!(F & FC)) {
    CYCT(0x6d8c, 0x6d8e);
    goto didntMount;
  }
  CYC(0x6d8c, 0x6d8e);
  CALL_C(0x6d8e, companionTryToMount_hook, 0x4599, 0x6d91);
  if (F & FZ) { CYCT(0x6d91, 0x6d92); ret_effect(gb); return; }
  CYC(0x6d91, 0x6d92);
didntMount:
  CYC(0x6d92, 0x6d94); E = 0x21;
  CYC(0x6d94, 0x6d95); A = mem_rd(gb, DE);
  CYC(0x6d95, 0x6d97); alu_and(gb, 0xc0);
  if (F & FZ) {
    CYCT(0x6d97, 0x6d99);
    rickyCheckHazards_hook(gb); return;
  }
  CYC(0x6d97, 0x6d99);
  CYC(0x6d99, 0x6d9a); alu_rlca(gb);
  CYC(0x6d9a, 0x6d9c); C = 0x40;
  if (!(F & FC)) {
    CYCT(0x6d9c, 0x6d9f);
    objectUpdateSpeedZ_paramC_hook(gb); return;
  }
  CYC(0x6d9c, 0x6d9f);
  CYC(0x6d9f, 0x6da2); SET_BC(0xff00);
  CALL_C(0x6da2, objectSetSpeedZ_hook, 0x239d, 0x6da5);
  rickyCheckHazards_hook(gb);
}

void rickyCheckHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6da5, companionCheckHazards_hook, 0x45ec, 0x6da8);
  if (F & FC) {
    CYCT(0x6da8, 0x6dab);
    rickyFunc_70cc(gb); return;
  }
  CYC(0x6da8, 0x6dab);
  rickyState9_hook(gb);
}

void rickyState9_hook(GB *gb) {
  CYC(0x6dab, 0x6dac); ret_effect(gb);
}

void rickyState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6dac, companionDecCounter1_hook, 0x49a2, 0x6daf);
  if (F & FZ) {
    CYCT(0x6daf, 0x6db1);
    goto update;
  }
  CYC(0x6daf, 0x6db1);
  CYC(0x6db1, 0x6db2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x6db2, 0x6db3); ret_effect(gb); return; }
  CYC(0x6db2, 0x6db3);
  CYC(0x6db3, 0x6db5); A = 0xc3;
  CALL_C(0x6db5, playSound_b00_hook, 0x0c98, 0x6db8);
update:
  CYC(0x6db8, 0x6dba); C = 0x40;
  CALL_C(0x6dba, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6dbd);
  CALL_C(0x6dbd, specialObjectAnimate_hook, 0x2aef, 0x6dc0);
  CALL_C(0x6dc0, objectApplySpeed_hook, 0x201d, 0x6dc3);
  CALL_C(0x6dc3, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x6dc6);
  CYC(0x6dc6, 0x6dc8); E = 0x33;
  CYC(0x6dc8, 0x6dc9); A = mem_rd(gb, DE);
  CYC(0x6dc9, 0x6dcb); alu_and(gb, 0x0f);
  CYC(0x6dcb, 0x6dcd); E = 0x07;
  if (F & FZ) {
    CYCT(0x6dcd, 0x6dcf);
    goto noWalls;
  }
  CYC(0x6dcd, 0x6dcf);
  CYC(0x6dcf, 0x6dd0); mem_wr(gb, DE, A);
  CYC(0x6dd0, 0x6dd1); ret_effect(gb); return;
noWalls:
  CYC(0x6dd1, 0x6dd2); A = mem_rd(gb, DE);
  CYC(0x6dd2, 0x6dd3); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6dd3, 0x6dd4); ret_effect(gb); return; }
  CYC(0x6dd3, 0x6dd4);
  CYC(0x6dd4, 0x6dd7); rickyStopUntilLandedOnGround(gb);
}

void rickyState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dd7, 0x6dd9); C = 0x40;
  CALL_C(0x6dd9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6ddc);
  CALL_ROM(0x6ddc, 0x48c1); // companionCheckMountingComplete
  if (!(F & FZ)) { CYCT(0x6ddf, 0x6de0); ret_effect(gb); return; }
  CYC(0x6ddf, 0x6de0);
  CALL_C(0x6de0, companionFinalizeMounting_hook, 0x479e, 0x6de3);
  CYC(0x6de3, 0x6de5); A = 0xc3;
  CALL_C(0x6de5, playSound_b00_hook, 0x0c98, 0x6de8);
  CYC(0x6de8, 0x6dea); C = 0x20;
  CYC(0x6dea, 0x6ded); companionSetAnimation_hook(gb);
}

void rickyState4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ded, 0x6def); E = 0x37;
  CYC(0x6def, 0x6df0); A = mem_rd(gb, DE);
  CYC(0x6df0, 0x6df2); alu_cp(gb, 0x0e);
  if (F & FZ) {
    CYCT(0x6df2, 0x6df4);
    goto countdown;
  }
  CYC(0x6df2, 0x6df4);
  CYC(0x6df4, 0x6df6); A = 0x0d;
  CYC(0x6df6, 0x6df7); mem_wr(gb, DE, A);
  CALL_C(0x6df7, companionDragToCenterOfHole_hook, 0x46a0, 0x6dfa);
  if (!(F & FZ)) { CYCT(0x6dfa, 0x6dfb); ret_effect(gb); return; }
  CYC(0x6dfa, 0x6dfb);
countdown:
  CALL_C(0x6dfb, companionDecCounter1_hook, 0x49a2, 0x6dfe);
  if (!(F & FZ)) {
    CYCT(0x6dfe, 0x6e00);
    goto animate;
  }
  CYC(0x6dfe, 0x6e00);
  CYC(0x6e00, 0x6e01); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6e01, 0x6e03); E = 0x37;
  CYC(0x6e03, 0x6e04); A = mem_rd(gb, DE);
  CALL_C(0x6e04, specialObjectSetAnimation_hook, 0x2b0a, 0x6e07);
  CYC(0x6e07, 0x6e09); E = 0x37;
  CYC(0x6e09, 0x6e0a); A = mem_rd(gb, DE);
  CYC(0x6e0a, 0x6e0c); alu_cp(gb, 0x0e);
  if (F & FZ) {
    CYCT(0x6e0c, 0x6e0e);
    goto animate;
  }
  CYC(0x6e0c, 0x6e0e);
  CYC(0x6e0e, 0x6e10); A = 0x65;
  CYC(0x6e10, 0x6e13); playSound_b00_hook(gb); return;
animate:
  CALL_C(0x6e13, companionAnimateDrowningOrFallingThenRespawn_hook, 0x4966, 0x6e16);
  if (!(F & FC)) { CYCT(0x6e16, 0x6e17); ret_effect(gb); return; }
  CYC(0x6e16, 0x6e17);
  CYC(0x6e17, 0x6e19); C = 0x01;
  CYC(0x6e19, 0x6e1c); A = W8(wLinkObjectIndex);
  CYC(0x6e1c, 0x6e1d); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x6e1d, 0x6e1f);
  } else {
    CYC(0x6e1d, 0x6e1f);
    CYC(0x6e1f, 0x6e21); C = 0x05;
  }
  CYC(0x6e21, 0x6e24); companionUpdateDirectionAndSetAnimation_hook(gb);
}

void rickyState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e24, 0x6e26); E = 0x05;
  CYC(0x6e26, 0x6e27); A = mem_rd(gb, DE);
  CYC(0x6e27, 0x6e28); push_effect(gb, 0x6e28);
  switch (ricky_jump_table(gb)) {
    case 0x6e30: rickyState5Substate0_hook(gb); return;
    case 0x6f16: rickyState5Substate1(gb); return;
    case 0x6f6b: rickyState5Substate2(gb); return;
    case 0x6f8c: rickyState5Substate3(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void rickyState5Substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e30, 0x6e33); A = W8(wForceCompanionDismount);
  CYC(0x6e33, 0x6e34); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6e34, 0x6e36);
    goto checkDismount;
  }
  CYC(0x6e34, 0x6e36);
  CYC(0x6e36, 0x6e39); A = W8(wGameKeysJustPressed);
  CYC(0x6e39, 0x6e3b); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x6e3b, 0x6e3e);
    rickyStartPunch(gb); return;
  }
  CYC(0x6e3b, 0x6e3e);
  CYC(0x6e3e, 0x6e40); alu_bit(gb, 1, A);
checkDismount:
  if (!(F & FZ)) {
    CYCT(0x6e40, 0x6e43);
    companionGotoDismountState_hook(gb); return;
  }
  CYC(0x6e40, 0x6e43);
  CYC(0x6e43, 0x6e44); H = D;
  CYC(0x6e44, 0x6e47); A = W8(wLinkAngle);
  CYC(0x6e47, 0x6e49); L = 0x09;
  CYC(0x6e49, 0x6e4a); mem_wr(gb, HL, A);
  CYC(0x6e4a, 0x6e4b); alu_rlca(gb);
  CYC(0x6e4b, 0x6e4d); L = 0x39;
  if (!(F & FC)) {
    CYCT(0x6e4d, 0x6e4f);
    goto moving;
  }
  CYC(0x6e4d, 0x6e4f);
  CYC(0x6e4f, 0x6e51); A = 0x10;
  CYC(0x6e51, 0x6e52); mem_wr(gb, HL, A);
  CYC(0x6e52, 0x6e54); C = 0x20;
  CALL_C(0x6e54, companionSetAnimation_hook, 0x458e, 0x6e57);
  CYC(0x6e57, 0x6e5a); rickyCheckHazards_hook(gb); return;
moving:
  CYC(0x6e5a, 0x6e5c); L = 0x39;
  CYC(0x6e5c, 0x6e5d); A = mem_rd(gb, HL);
  CYC(0x6e5d, 0x6e5e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6e5e, 0x6e60);
    goto tryToJump;
  }
  CYC(0x6e5e, 0x6e60);
  CYC(0x6e60, 0x6e61); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6e61, 0x6e63); L = 0x10;
  CYC(0x6e63, 0x6e65); mem_wr(gb, HL, 0x1e);
  CYC(0x6e65, 0x6e67); C = 0x20;
  CALL_C(0x6e67, companionUpdateDirectionAndAnimate_hook, 0x456c, 0x6e6a);
  CALL_ROM(0x6e6a, 0x6edf); // rickyCheckForHoleInFront
  if (F & FZ) {
    CYCT(0x6e6d, 0x6e70);
    rickyBeginJumpOverHole(gb); return;
  }
  CYC(0x6e6d, 0x6e70);
  CALL_C(0x6e70, companionCheckHopDownCliff_hook, 0x473c, 0x6e73);
  if (!(F & FZ)) {
    CYCT(0x6e73, 0x6e75);
    goto checkHopUp;
  }
  CYC(0x6e73, 0x6e75);
  CYC(0x6e75, 0x6e78); rickySetJumpSpeed(gb); return;
checkHopUp:
  CALL_ROM(0x6e78, 0x72b8); // rickyCheckHopUpCliff
  if (!(F & FZ)) {
    CYCT(0x6e7b, 0x6e7d);
    goto updateMovement;
  }
  CYC(0x6e7b, 0x6e7d);
  CYC(0x6e7d, 0x6e80); rickySetJumpSpeed_andcc91(gb); return;
updateMovement:
  CALL_C(0x6e80, companionUpdateMovement_hook, 0x446b, 0x6e83);
  CYC(0x6e83, 0x6e86); rickyCheckHazards_hook(gb); return;
tryToJump:
  CYC(0x6e86, 0x6e87); H = D;
  CYC(0x6e87, 0x6e89); L = 0x09;
  CYC(0x6e89, 0x6e8a); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6e8a, 0x6e8b); alu_add(gb, A);
  CYC(0x6e8b, 0x6e8d); A = alu_swap(gb, A);
  CYC(0x6e8d, 0x6e8f); alu_and(gb, 0x03);
  CYC(0x6e8f, 0x6e90); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_ROM(0x6e90, 0x733f); // rickySetJumpSpeed_andcc91
  CYC(0x6e93, 0x6e95); L = 0x09;
  CYC(0x6e95, 0x6e96); A = mem_rd(gb, HL);
  CYC(0x6e96, 0x6e98); alu_bit(gb, 2, A);
  if (!(F & FZ)) {
    CYCT(0x6e98, 0x6e9a);
    goto jump;
  }
  CYC(0x6e98, 0x6e9a);
  CALL_C(0x6e9a, companionCheckHopDownCliff_hook, 0x473c, 0x6e9d);
  if (!(F & FZ)) {
    CYCT(0x6e9d, 0x6e9f);
    goto checkHopUpForJump;
  }
  CYC(0x6e9d, 0x6e9f);
  CYC(0x6e9f, 0x6ea2); W8(wDisableScreenTransitions) = A;
  CYC(0x6ea2, 0x6ea4); C = 0x0f;
  CYC(0x6ea4, 0x6ea7); companionSetAnimation_hook(gb); return;
checkHopUpForJump:
  CALL_ROM(0x6ea7, 0x72b8); // rickyCheckHopUpCliff
  CYC(0x6eaa, 0x6eac); C = 0x0f;
  if (F & FZ) {
    CYCT(0x6eac, 0x6eaf);
    companionSetAnimation_hook(gb); return;
  }
  CYC(0x6eac, 0x6eaf);
jump:
  CYC(0x6eaf, 0x6eb1); E = 0x05;
  CYC(0x6eb1, 0x6eb3); A = 0x02;
  CYC(0x6eb3, 0x6eb4); mem_wr(gb, DE, A);
  CALL_ROM(0x6eb4, 0x6edf); // rickyCheckForHoleInFront
  if (F & FZ) {
    CYCT(0x6eb7, 0x6eba);
    rickyBeginJumpOverHole(gb); return;
  }
  CYC(0x6eb7, 0x6eba);
  CYC(0x6eba, 0x6ebd); SET_BC(0xfe80);
  CALL_C(0x6ebd, objectSetSpeedZ_hook, 0x239d, 0x6ec0);
  CYC(0x6ec0, 0x6ec2); L = 0x05;
  CYC(0x6ec2, 0x6ec4); mem_wr(gb, HL, 0x01);
  CYC(0x6ec4, 0x6ec6); L = 0x06;
  CYC(0x6ec6, 0x6ec8); mem_wr(gb, HL, 0x08);
  CYC(0x6ec8, 0x6eca); L = 0x10;
  CYC(0x6eca, 0x6ecc); mem_wr(gb, HL, 0x50);
  CYC(0x6ecc, 0x6ece); C = 0x19;
  CALL_C(0x6ece, companionSetAnimation_hook, 0x458e, 0x6ed1);
  CALL_C(0x6ed1, getRandomNumber_hook, 0x043e, 0x6ed4);
  CYC(0x6ed4, 0x6ed6); alu_and(gb, 0x0f);
  CYC(0x6ed6, 0x6ed8); A = 0x53;
  if (!(F & FZ)) {
    CYCT(0x6ed8, 0x6eda);
  } else {
    CYC(0x6ed8, 0x6eda);
    CYC(0x6eda, 0x6edc); A = 0xc3;
  }
  CYC(0x6edc, 0x6edf); playSound_b00_hook(gb);
}
