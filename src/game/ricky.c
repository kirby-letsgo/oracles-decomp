#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

#define rickyCliffOffsets_bank05 0x730c
#define rickyHoleCheckOffsets_bank05 0x737a

void rickyState0_hook(GB *gb);
void rickyState1_hook(GB *gb);
void rickyCheckHazards_hook(GB *gb);
void rickyState9_hook(GB *gb);
void rickyState2_hook(GB *gb);
void rickyState3_hook(GB *gb);
void rickyState4_hook(GB *gb);
void rickyState5_hook(GB *gb);
void rickyState5Substate0_hook(GB *gb);
void rickyCheckForHoleInFront_hook(GB *gb);
void rickyState5Substate1_hook(GB *gb);
void rickyState5Substate2_hook(GB *gb);
void rickyState5Substate3_hook(GB *gb);
void rickyState8_hook(GB *gb);
void rickyStartPunch_hook(GB *gb);
void rickyStopUntilLandedOnGround_hook(GB *gb);
void rickyCheckHazards2_hook(GB *gb);
void rickyBreakTilesOnLanding_hook(GB *gb);
void rickyCheckAtScreenEdge_hook(GB *gb);
void rickyState6_hook(GB *gb);
void rickyState7_hook(GB *gb);
void rickyFunc_70cc_hook(GB *gb);
void rickyStateA_hook(GB *gb);
void rickyStateASubstate0_hook(GB *gb);
void rickyStateASubstate1_hook(GB *gb);
void rickyStateASubstate2_hook(GB *gb);
void rickySetJumpSpeedForCutsceneAndSetAngle_hook(GB *gb);
void rickySetJumpSpeedForCutscene_hook(GB *gb);
void rickyStateASubstate6_hook(GB *gb);
void rickyStateASubstate3_hook(GB *gb);
void rickyStateASubstate5_hook(GB *gb);
void rickyStateASubstate4_hook(GB *gb);
void rickyIncVar03_hook(GB *gb);
void rickyStateASubstateB_hook(GB *gb);
void rickyStateASubstateC_hook(GB *gb);
void rickyWaitUntilJumpDone_hook(GB *gb);
void rickyStateC_hook(GB *gb);
void rickyCheckHopUpCliff_hook(GB *gb);
void rickyBeginJumpOverHole_hook(GB *gb);
void rickySetJumpSpeed_andcc91_hook(GB *gb);
void rickySetJumpSpeed_hook(GB *gb);

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

static void ricky_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
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
    case 0x705a: rickyState6_hook(gb); return;
    case 0x7090: rickyState7_hook(gb); return;
    case 0x6f9c: rickyState8_hook(gb); return;
    case 0x6dab: rickyState9_hook(gb); return;
    case 0x70dd: rickyStateA_hook(gb); return;
    case 0x726d: rickyStateC_hook(gb); return;
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
    rickyFunc_70cc_hook(gb); return;
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
  CYC(0x6dd4, 0x6dd7); rickyStopUntilLandedOnGround_hook(gb);
}

void rickyState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6dd7, 0x6dd9); C = 0x40;
  CALL_C(0x6dd9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6ddc);
  CALL_C(0x6ddc, companionCheckMountingComplete_hook, 0x48c1, 0x6ddf);
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
    case 0x6f16: rickyState5Substate1_hook(gb); return;
    case 0x6f6b: rickyState5Substate2_hook(gb); return;
    case 0x6f8c: rickyState5Substate3_hook(gb); return;
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
    rickyStartPunch_hook(gb); return;
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
  CALL_C(0x6e6a, rickyCheckForHoleInFront_hook, 0x6edf, 0x6e6d);
  if (F & FZ) {
    CYCT(0x6e6d, 0x6e70);
    rickyBeginJumpOverHole_hook(gb); return;
  }
  CYC(0x6e6d, 0x6e70);
  CALL_C(0x6e70, companionCheckHopDownCliff_hook, 0x473c, 0x6e73);
  if (!(F & FZ)) {
    CYCT(0x6e73, 0x6e75);
    goto checkHopUp;
  }
  CYC(0x6e73, 0x6e75);
  CYC(0x6e75, 0x6e78); rickySetJumpSpeed_hook(gb); return;
checkHopUp:
  CALL_C(0x6e78, rickyCheckHopUpCliff_hook, 0x72b8, 0x6e7b);
  if (!(F & FZ)) {
    CYCT(0x6e7b, 0x6e7d);
    goto updateMovement;
  }
  CYC(0x6e7b, 0x6e7d);
  CYC(0x6e7d, 0x6e80); rickySetJumpSpeed_andcc91_hook(gb); return;
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
  CALL_C(0x6e90, rickySetJumpSpeed_andcc91_hook, 0x733f, 0x6e93);
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
  CALL_C(0x6ea7, rickyCheckHopUpCliff_hook, 0x72b8, 0x6eaa);
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
  CALL_C(0x6eb4, rickyCheckForHoleInFront_hook, 0x6edf, 0x6eb7);
  if (F & FZ) {
    CYCT(0x6eb7, 0x6eba);
    rickyBeginJumpOverHole_hook(gb); return;
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

void rickyCheckForHoleInFront_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6edf, 0x6ee2); A = W8(wLinkAngle);
  CYC(0x6ee2, 0x6ee4); alu_and(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x6ee4, 0x6ee5); ret_effect(gb); return; }
  CYC(0x6ee4, 0x6ee5);
  CYC(0x6ee5, 0x6ee7); E = 0x08;
  CYC(0x6ee7, 0x6ee8); A = mem_rd(gb, DE);
  CYC(0x6ee8, 0x6eeb); SET_HL(0x737a);
  CYC(0x6eeb, 0x6eec); ricky_add_double_index(gb, 0x6eec);
  CYC(0x6eec, 0x6eee); E = 0x0b;
  CYC(0x6eee, 0x6eef); A = mem_rd(gb, DE);
  CYC(0x6eef, 0x6ef0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6ef0, 0x6ef2); H8(hFF90) = A;
  CYC(0x6ef2, 0x6ef3); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6ef3, 0x6ef4); B = A;
  CYC(0x6ef4, 0x6ef5); SET_HL(HL + 1);
  CYC(0x6ef5, 0x6ef7); E = 0x0d;
  CYC(0x6ef7, 0x6ef8); A = mem_rd(gb, DE);
  CYC(0x6ef8, 0x6ef9); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6ef9, 0x6efb); H8(hFF91) = A;
  CYC(0x6efb, 0x6efc); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6efc, 0x6efd); C = A;
  CALL_C(0x6efd, getTileAtPosition_hook, 0x1447, 0x6f00);
  CYC(0x6f00, 0x6f01); A = L;
  CYC(0x6f01, 0x6f03); E = 0x36;
  CYC(0x6f03, 0x6f04); mem_wr(gb, DE, A);
  CYC(0x6f04, 0x6f06); A = H8(hFF90);
  CYC(0x6f06, 0x6f07); B = A;
  CYC(0x6f07, 0x6f09); A = H8(hFF91);
  CYC(0x6f09, 0x6f0a); C = A;
  CALL_C(0x6f0a, getTileAtPosition_hook, 0x1447, 0x6f0d);
  CYC(0x6f0d, 0x6f0f); H = 0xcf;
  CYC(0x6f0f, 0x6f10); A = mem_rd(gb, HL);
  CYC(0x6f10, 0x6f12); alu_cp(gb, 0xf3);
  if (F & FZ) { CYCT(0x6f12, 0x6f13); ret_effect(gb); return; }
  CYC(0x6f12, 0x6f13);
  CYC(0x6f13, 0x6f15); alu_cp(gb, 0xfd);
  CYC(0x6f15, 0x6f16); ret_effect(gb);
}

void rickyState5Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f16, 0x6f17); E = alu_dec8(gb, E);
  CYC(0x6f17, 0x6f18); A = mem_rd(gb, DE);
  CYC(0x6f18, 0x6f1a); alu_cp(gb, 0x05);
  if (!(F & FZ)) {
    CYCT(0x6f1a, 0x6f1c);
    goto doneInputParsing;
  }
  CYC(0x6f1a, 0x6f1c);
  CYC(0x6f1c, 0x6f1f); A = W8(wGameKeysJustPressed);
  CYC(0x6f1f, 0x6f21); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x6f21, 0x6f24);
    rickyStartPunch_hook(gb); return;
  }
  CYC(0x6f21, 0x6f24);
  CYC(0x6f24, 0x6f27); A = W8(wLinkAngle);
  CYC(0x6f27, 0x6f29); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6f29, 0x6f2b);
    goto doneInputParsing;
  }
  CYC(0x6f29, 0x6f2b);
  CYC(0x6f2b, 0x6f2e); SET_HL(w1Companion_direction);
  CYC(0x6f2e, 0x6f2f); B = A;
  CYC(0x6f2f, 0x6f30); alu_add(gb, A);
  CYC(0x6f30, 0x6f32); A = alu_swap(gb, A);
  CYC(0x6f32, 0x6f34); alu_and(gb, 0x03);
  CYC(0x6f34, 0x6f35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6f35, 0x6f36); A = B;
  CYC(0x6f36, 0x6f37); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x6f37, 0x6f38); mem_wr(gb, HL, A);
  CYC(0x6f38, 0x6f3a); C = 0x19;
  if (!(F & FZ)) CALL_C_CC(0x6f3a, companionSetAnimation_hook, 0x458e, 0x6f3d);
  else CYC(0x6f3a, 0x6f3d);
doneInputParsing:
  CYC(0x6f3d, 0x6f3f); C = 0x40;
  CALL_C(0x6f3f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6f42);
  if (F & FZ) {
    CYCT(0x6f42, 0x6f44);
    goto landed;
  }
  CYC(0x6f42, 0x6f44);
  CYC(0x6f44, 0x6f47); A = W8(wLinkObjectIndex);
  CYC(0x6f47, 0x6f48); alu_rra(gb);
  if (!(F & FC)) {
    CYCT(0x6f48, 0x6f4a);
    goto checkHole;
  }
  CYC(0x6f48, 0x6f4a);
  CYC(0x6f4a, 0x6f4d); A = W8(wLinkAngle);
  CYC(0x6f4d, 0x6f4f); alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x6f4f, 0x6f51);
    goto updateMovement;
  }
  CYC(0x6f4f, 0x6f51);
checkHole:
  CYC(0x6f51, 0x6f54); SET_HL(0x737a);
  CALL_C(0x6f54, specialObjectGetRelativeTileWithDirectionTable_hook, 0x44f1, 0x6f57);
  CYC(0x6f57, 0x6f58); A = B;
  CYC(0x6f58, 0x6f5a); alu_cp(gb, 0xf3);
  if (F & FZ) { CYCT(0x6f5a, 0x6f5b); ret_effect(gb); return; }
  CYC(0x6f5a, 0x6f5b);
  CYC(0x6f5b, 0x6f5d); alu_cp(gb, 0xfd);
  if (F & FZ) { CYCT(0x6f5d, 0x6f5e); ret_effect(gb); return; }
  CYC(0x6f5d, 0x6f5e);
updateMovement:
  CYC(0x6f5e, 0x6f61); companionUpdateMovement_hook(gb); return;
landed:
  CALL_C(0x6f61, specialObjectAnimate_hook, 0x2aef, 0x6f64);
  CALL_C(0x6f64, companionDecCounter1IfNonzero_hook, 0x495e, 0x6f67);
  if (!(F & FZ)) { CYCT(0x6f67, 0x6f68); ret_effect(gb); return; }
  CYC(0x6f67, 0x6f68);
  CYC(0x6f68, 0x6f6b); rickyStopUntilLandedOnGround_hook(gb);
}

void rickyState5Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6f6b, companionDecCounter1_hook, 0x49a2, 0x6f6e);
  if (F & FZ) {
    CYCT(0x6f6e, 0x6f70);
    goto update;
  }
  CYC(0x6f6e, 0x6f70);
  CYC(0x6f70, 0x6f71); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x6f71, 0x6f72); ret_effect(gb); return; }
  CYC(0x6f71, 0x6f72);
  CYC(0x6f72, 0x6f74); A = 0xc3;
  CALL_C(0x6f74, playSound_b00_hook, 0x0c98, 0x6f77);
update:
  CYC(0x6f77, 0x6f79); C = 0x40;
  CALL_C(0x6f79, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6f7c);
  if (F & FZ) {
    CYCT(0x6f7c, 0x6f7f);
    rickyStopUntilLandedOnGround_hook(gb); return;
  }
  CYC(0x6f7c, 0x6f7f);
  CALL_C(0x6f7f, specialObjectAnimate_hook, 0x2aef, 0x6f82);
  CALL_C(0x6f82, companionUpdateMovement_hook, 0x446b, 0x6f85);
  CALL_C(0x6f85, specialObjectCheckMovingTowardWall_hook, 0x451b, 0x6f88);
  if (!(F & FZ)) {
    CYCT(0x6f88, 0x6f8b);
    rickyStopUntilLandedOnGround_hook(gb); return;
  }
  CYC(0x6f88, 0x6f8b);
  CYC(0x6f8b, 0x6f8c); ret_effect(gb);
}

void rickyState5Substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f8c, 0x6f8e); C = 0x40;
  CALL_C(0x6f8e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6f91);
  if (!(F & FZ)) { CYCT(0x6f91, 0x6f92); ret_effect(gb); return; }
  CYC(0x6f91, 0x6f92);
  CALL_C(0x6f92, rickyBreakTilesOnLanding_hook, 0x7314, 0x6f95);
  CYC(0x6f95, 0x6f96); alu_xor(gb, A);
  CYC(0x6f96, 0x6f98); E = 0x05;
  CYC(0x6f98, 0x6f99); mem_wr(gb, DE, A);
  CYC(0x6f99, 0x6f9c); rickyCheckHazards2_hook(gb);
}

void rickyState8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f9c, 0x6f9e); E = 0x05;
  CYC(0x6f9e, 0x6f9f); A = mem_rd(gb, DE);
  CYC(0x6f9f, 0x6fa0); push_effect(gb, 0x6fa0);
  switch (ricky_jump_table(gb)) {
    case 0x6fa4: goto substate0;
    case 0x6fe3: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x6fa4, 0x6fa6); C = 0x40;
  CALL_C(0x6fa6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6fa9);
  if (F & FZ) {
    CYCT(0x6fa9, 0x6fab);
    goto onGround;
  }
  CYC(0x6fa9, 0x6fab);
  CALL_C(0x6fab, companionUpdateMovement_hook, 0x446b, 0x6fae);
  CYC(0x6fae, 0x6fb0);
  goto animate;
onGround:
  CALL_C(0x6fb0, companionTryToBreakTileFromMoving_hook, 0x4477, 0x6fb3);
  CALL_C(0x6fb3, rickyCheckHazards_hook, 0x6da5, 0x6fb6);
animate:
  CALL_C(0x6fb6, specialObjectAnimate_hook, 0x2aef, 0x6fb9);
  CYC(0x6fb9, 0x6fbb); E = 0x21;
  CYC(0x6fbb, 0x6fbc); A = mem_rd(gb, DE);
  CYC(0x6fbc, 0x6fbe); alu_and(gb, 0xc0);
  if (F & FZ) { CYCT(0x6fbe, 0x6fbf); ret_effect(gb); return; }
  CYC(0x6fbe, 0x6fbf);
  CYC(0x6fbf, 0x6fc0); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x6fc0, 0x6fc2);
    goto startTornadoCharge;
  }
  CYC(0x6fc0, 0x6fc2);
  CYC(0x6fc2, 0x6fc4); A = 0x75;
  CYC(0x6fc4, 0x6fc7); playSound_b00_hook(gb); return;
startTornadoCharge:
  CYC(0x6fc7, 0x6fc9); E = 0x0f;
  CYC(0x6fc9, 0x6fca); A = mem_rd(gb, DE);
  CYC(0x6fca, 0x6fcb); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6fcb, 0x6fcc); ret_effect(gb); return; }
  CYC(0x6fcb, 0x6fcc);
  CYC(0x6fcc, 0x6fcf); A = W8(wGameKeysPressed);
  CYC(0x6fcf, 0x6fd1); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x6fd1, 0x6fd4);
    rickyStopUntilLandedOnGround_hook(gb); return;
  }
  CYC(0x6fd1, 0x6fd4);
  CALL_C(0x6fd4, itemIncSubstate_hook, 0x23ef, 0x6fd7);
  CYC(0x6fd7, 0x6fd9); C = 0x13;
  CALL_C(0x6fd9, companionSetAnimation_hook, 0x458e, 0x6fdc);
  CALL_C(0x6fdc, companionCheckHazards_hook, 0x45ec, 0x6fdf);
  if (!(F & FC)) { CYCT(0x6fdf, 0x6fe0); ret_effect(gb); return; }
  CYC(0x6fdf, 0x6fe0);
  CYC(0x6fe0, 0x6fe3); rickyFunc_70cc_hook(gb); return;
substate1:
  CYC(0x6fe3, 0x6fe6); A = W8(wLinkAngle);
  CYC(0x6fe6, 0x6fe8); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6fe8, 0x6fea);
    goto animateCharge;
  }
  CYC(0x6fe8, 0x6fea);
  CYC(0x6fea, 0x6fed); SET_HL(w1Companion_angle);
  CYC(0x6fed, 0x6fee); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x6fee, 0x6fef); mem_wr(gb, HL, A);
  CYC(0x6fef, 0x6ff1); C = 0x13;
  if (!(F & FZ)) CALL_C_CC(0x6ff1, companionUpdateDirectionAndAnimate_hook, 0x456c, 0x6ff4);
  else CYC(0x6ff1, 0x6ff4);
animateCharge:
  CALL_C(0x6ff4, specialObjectAnimate_hook, 0x2aef, 0x6ff7);
  CYC(0x6ff7, 0x6ffa); A = W8(wGameKeysPressed);
  CYC(0x6ffa, 0x6ffc); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x6ffc, 0x6ffe);
    goto releasedAButton;
  }
  CYC(0x6ffc, 0x6ffe);
  CYC(0x6ffe, 0x7000); E = 0x35;
  CYC(0x7000, 0x7001); A = mem_rd(gb, DE);
  CYC(0x7001, 0x7003); alu_cp(gb, 0x1e);
  if (!(F & FZ)) {
    CYCT(0x7003, 0x7005);
    goto continueCharging;
  }
  CYC(0x7003, 0x7005);
  CALL_C(0x7005, companionTryToBreakTileFromMoving_hook, 0x4477, 0x7008);
  CALL_C(0x7008, rickyCheckHazards_hook, 0x6da5, 0x700b);
  CYC(0x700b, 0x700d); C = 0x04;
  CYC(0x700d, 0x7010); companionFlashFromChargingAnimation_hook(gb); return;
continueCharging:
  CYC(0x7010, 0x7011); A = alu_inc8(gb, A);
  CYC(0x7011, 0x7012); mem_wr(gb, DE, A);
  CYC(0x7012, 0x7014); alu_cp(gb, 0x1e);
  if (!(F & FZ)) { CYCT(0x7014, 0x7015); ret_effect(gb); return; }
  CYC(0x7014, 0x7015);
  CYC(0x7015, 0x7017); A = 0x4f;
  CYC(0x7017, 0x701a); playSound_b00_hook(gb); return;
releasedAButton:
  CYC(0x701a, 0x701d); SET_HL(w1Link_oamFlagsBackup);
  CYC(0x701d, 0x701e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x701e, 0x701f); mem_wr(gb, HL, A);
  CYC(0x701f, 0x7021); E = 0x35;
  CYC(0x7021, 0x7022); A = mem_rd(gb, DE);
  CYC(0x7022, 0x7024); alu_cp(gb, 0x1e);
  if (!(F & FZ)) {
    CYCT(0x7024, 0x7026);
    goto notCharged;
  }
  CYC(0x7024, 0x7026);
  CYC(0x7026, 0x7029); SET_BC(0x2a00);
  CALL_C(0x7029, companionCreateItem_hook, 0x4555, 0x702c);
  CYC(0x702c, 0x702e); A = 0xf1;
  CALL_C(0x702e, playSound_b00_hook, 0x0c98, 0x7031);
  CYC(0x7031, 0x7033); A = 0x6b;
  CALL_C(0x7033, playSound_b00_hook, 0x0c98, 0x7036);
  CYC(0x7036, 0x7038);
  rickyStartPunch_hook(gb); return;
notCharged:
  CYC(0x7038, 0x703a); C = 0x05;
  CYC(0x703a, 0x703d); companionSetAnimationAndGotoState5_hook(gb);
}

void rickyStartPunch_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x703d, 0x7040); SET_BC(0x2800);
  CALL_C(0x7040, companionCreateWeaponItem_hook, 0x455b, 0x7043);
  if (!(F & FZ)) { CYCT(0x7043, 0x7044); ret_effect(gb); return; }
  CYC(0x7043, 0x7044);
  CYC(0x7044, 0x7045); H = D;
  CYC(0x7045, 0x7047); L = 0x04;
  CYC(0x7047, 0x7049); A = 0x08;
  CYC(0x7049, 0x704a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x704a, 0x704b); alu_xor(gb, A);
  CYC(0x704b, 0x704c); mem_wr(gb, HL, A);
  CYC(0x704c, 0x704d); A = alu_inc8(gb, A);
  CYC(0x704d, 0x704f); L = 0x35;
  CYC(0x704f, 0x7050); mem_wr(gb, HL, A);
  CYC(0x7050, 0x7052); C = 0x09;
  CALL_C(0x7052, companionSetAnimation_hook, 0x458e, 0x7055);
  CYC(0x7055, 0x7057); A = 0x74;
  CYC(0x7057, 0x705a); playSound_b00_hook(gb);
}

void rickyState6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x705a, 0x705c); E = 0x05;
  CYC(0x705c, 0x705d); A = mem_rd(gb, DE);
  CYC(0x705d, 0x705e); push_effect(gb, 0x705e);
  switch (ricky_jump_table(gb)) {
    case 0x7064: goto substate0;
    case 0x7075: goto substate1;
    case 0x707d: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x7064, 0x7066); C = 0x40;
  CALL_C(0x7066, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7069);
  if (!(F & FZ)) { CYCT(0x7069, 0x706a); ret_effect(gb); return; }
  CYC(0x7069, 0x706a);
  CALL_C(0x706a, itemIncSubstate_hook, 0x23ef, 0x706d);
  CALL_C(0x706d, companionDismountAndSavePosition_hook, 0x460c, 0x7070);
  CYC(0x7070, 0x7072); A = 0x17;
  CYC(0x7072, 0x7075); specialObjectSetAnimation_hook(gb); return;
substate1:
  CYC(0x7075, 0x7078); A = W8(wLinkInAir);
  CYC(0x7078, 0x7079); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7079, 0x707a); ret_effect(gb); return; }
  CYC(0x7079, 0x707a);
  CYC(0x707a, 0x707d); itemIncSubstate_hook(gb); return;
substate2:
  CALL_C(0x707d, companionSetPriorityRelativeToLink_hook, 0x4936, 0x7080);
  CYC(0x7080, 0x7082); C = 0x09;
  CALL_C(0x7082, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x7085);
  if (F & FC) {
    CYCT(0x7085, 0x7088);
    rickyCheckHazards_hook(gb); return;
  }
  CYC(0x7085, 0x7088);
  CYC(0x7088, 0x708a); E = 0x05;
  CYC(0x708a, 0x708b); alu_xor(gb, A);
  CYC(0x708b, 0x708c); mem_wr(gb, DE, A);
  CYC(0x708c, 0x708d); E = alu_dec8(gb, E);
  CYC(0x708d, 0x708e); A = alu_inc8(gb, A);
  CYC(0x708e, 0x708f); mem_wr(gb, DE, A);
  CYC(0x708f, 0x7090); ret_effect(gb);
}

void rickyState7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7090, companionDecCounter1ToJumpDownCliff_hook, 0x493f, 0x7093);
  if (F & FC) { CYCT(0x7093, 0x7094); ret_effect(gb); return; }
  CYC(0x7093, 0x7094);
  CALL_C(0x7094, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x7097);
  CALL_C(0x7097, specialObjectCheckMovingAwayFromWall_hook, 0x4500, 0x709a);
  CYC(0x709a, 0x709c); E = 0x07;
  if (F & FZ) {
    CYCT(0x709c, 0x709e);
    goto noWalls;
  }
  CYC(0x709c, 0x709e);
  CYC(0x709e, 0x709f); mem_wr(gb, DE, A);
  CYC(0x709f, 0x70a0); ret_effect(gb); return;
noWalls:
  CYC(0x70a0, 0x70a1); A = mem_rd(gb, DE);
  CYC(0x70a1, 0x70a2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x70a2, 0x70a3); ret_effect(gb); return; }
  CYC(0x70a2, 0x70a3);
  rickyStopUntilLandedOnGround_hook(gb);
}

void rickyStopUntilLandedOnGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70a3, 0x70a6); A = W8(wLinkObjectIndex);
  CYC(0x70a6, 0x70a7); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x70a7, 0x70a9);
    goto setState;
  }
  CYC(0x70a7, 0x70a9);
  CYC(0x70a9, 0x70aa); alu_xor(gb, A);
  CYC(0x70aa, 0x70ad); W8(wLinkInAir) = A;
  CYC(0x70ad, 0x70b0); W8(wDisableScreenTransitions) = A;
setState:
  CYC(0x70b0, 0x70b2); A = 0x05;
  CYC(0x70b2, 0x70b4); E = 0x04;
  CYC(0x70b4, 0x70b5); mem_wr(gb, DE, A);
  CYC(0x70b5, 0x70b7); A = 0x03;
  CYC(0x70b7, 0x70b9); E = 0x05;
  CYC(0x70b9, 0x70ba); mem_wr(gb, DE, A);
  CALL_C(0x70ba, rickyCheckAtScreenEdge_hook, 0x7359, 0x70bd);
  if (F & FZ) {
    CYCT(0x70bd, 0x70bf);
    rickyCheckHazards2_hook(gb); return;
  }
  CYC(0x70bd, 0x70bf);
  CYC(0x70bf, 0x70c1); E = 0x39;
  CYC(0x70c1, 0x70c3); A = 0x10;
  CYC(0x70c3, 0x70c4); mem_wr(gb, DE, A);
  rickyCheckHazards2_hook(gb);
}

void rickyCheckHazards2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70c4, companionCheckHazards_hook, 0x45ec, 0x70c7);
  CYC(0x70c7, 0x70c9); C = 0x20;
  if (!(F & FC)) {
    CYCT(0x70c9, 0x70cc);
    companionSetAnimation_hook(gb); return;
  }
  CYC(0x70c9, 0x70cc);
  rickyFunc_70cc_hook(gb);
}

void rickyFunc_70cc_hook(GB *gb) {
  CYC(0x70cc, 0x70ce); C = 0x0e;
  CYC(0x70ce, 0x70d0); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x70d0, 0x70d2);
    goto setHazard;
  }
  CYC(0x70d0, 0x70d2);
  CYC(0x70d2, 0x70d4); C = 0x0d;
setHazard:
  CYC(0x70d4, 0x70d5); H = D;
  CYC(0x70d5, 0x70d7); L = 0x37;
  CYC(0x70d7, 0x70d8); mem_wr(gb, HL, C);
  CYC(0x70d8, 0x70da); L = 0x06;
  CYC(0x70da, 0x70dc); mem_wr(gb, HL, 0x00);
  CYC(0x70dc, 0x70dd); ret_effect(gb);
}

void rickyStateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70dd, 0x70df); E = 0x03;
  CYC(0x70df, 0x70e0); A = mem_rd(gb, DE);
  CYC(0x70e0, 0x70e1); push_effect(gb, 0x70e1);
  switch (ricky_jump_table(gb)) {
    case 0x70f1: rickyStateASubstate0_hook(gb); return;
    case 0x7109: rickyStateASubstate1_hook(gb); return;
    case 0x7111: rickyStateASubstate2_hook(gb); return;
    case 0x7178: rickyStateASubstate3_hook(gb); return;
    case 0x71a0: rickyStateASubstate4_hook(gb); return;
    case 0x718c: rickyStateASubstate5_hook(gb); return;
    case 0x7159: rickyStateASubstate6_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void rickyStateASubstate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70f1, companionPreventLinkFromPassing_noExtraChecks_hook, 0x4465, 0x70f4);
  CALL_C(0x70f4, companionSetPriorityRelativeToLink_hook, 0x4936, 0x70f7);
  CALL_C(0x70f7, specialObjectAnimate_hook, 0x2aef, 0x70fa);
  CYC(0x70fa, 0x70fc); E = 0x21;
  CYC(0x70fc, 0x70fd); A = mem_rd(gb, DE);
  CYC(0x70fd, 0x70fe); alu_rlca(gb);
  CYC(0x70fe, 0x7100); C = 0x40;
  if (!(F & FC)) {
    CYCT(0x7100, 0x7103);
    objectUpdateSpeedZ_paramC_hook(gb); return;
  }
  CYC(0x7100, 0x7103);
  CYC(0x7103, 0x7106); SET_BC(0xff00);
  CYC(0x7106, 0x7109); objectSetSpeedZ_hook(gb);
}

void rickyStateASubstate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7109, 0x710b); E = 0x3d;
  CALL_C(0x710b, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x710e);
  CYC(0x710e, 0x7111); companionForceMount_hook(gb);
}

void rickyStateASubstate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7111, 0x7113); C = 0x40;
  CALL_C(0x7113, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7116);
  if (!(F & FZ)) { CYCT(0x7116, 0x7117); ret_effect(gb); return; }
  CYC(0x7116, 0x7117);
  CYC(0x7117, 0x711a); SET_BC(0x2006);
  CALL_C(0x711a, showText_hook, 0x1872, 0x711d);
  CYC(0x711d, 0x7120); SET_HL(w1Link_yh);
  CYC(0x7120, 0x7122); E = 0x0b;
  CYC(0x7122, 0x7123); A = mem_rd(gb, DE);
  CYC(0x7123, 0x7124); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x7124, 0x7126); A = 0x02;
  if (F & FC) {
    CYCT(0x7126, 0x7128);
    goto setDirection;
  }
  CYC(0x7126, 0x7128);
  CYC(0x7128, 0x712a); A = 0x00;
setDirection:
  CYC(0x712a, 0x712c); E = 0x08;
  CYC(0x712c, 0x712d); mem_wr(gb, DE, A);
  CYC(0x712d, 0x712f); A = 0x03;
  CYC(0x712f, 0x7131); E = 0x3f;
  CYC(0x7131, 0x7132); mem_wr(gb, DE, A);
  CALL_C(0x7132, specialObjectSetAnimation_hook, 0x2b0a, 0x7135);
  CALL_C(0x7135, rickyIncVar03_hook, 0x71f1, 0x7138);
  CYC(0x7138, 0x713a); rickySetJumpSpeedForCutscene_hook(gb);
}

void rickySetJumpSpeedForCutsceneAndSetAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x713a, 0x713c); B = 0x30;
  CYC(0x713c, 0x713e); C = 0x58;
  CALL_C(0x713e, objectGetRelativeAngle_hook, 0x1ea4, 0x7141);
  CYC(0x7141, 0x7143); alu_and(gb, 0x1c);
  CYC(0x7143, 0x7145); E = 0x09;
  CYC(0x7145, 0x7146); mem_wr(gb, DE, A);
  rickySetJumpSpeedForCutscene_hook(gb);
}

void rickySetJumpSpeedForCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7146, 0x7149); SET_BC(0xfe80);
  CALL_C(0x7149, objectSetSpeedZ_hook, 0x239d, 0x714c);
  CYC(0x714c, 0x714e); L = 0x05;
  CYC(0x714e, 0x7150); mem_wr(gb, HL, 0x01);
  CYC(0x7150, 0x7152); L = 0x10;
  CYC(0x7152, 0x7154); mem_wr(gb, HL, 0x50);
  CYC(0x7154, 0x7156); L = 0x06;
  CYC(0x7156, 0x7158); mem_wr(gb, HL, 0x08);
  CYC(0x7158, 0x7159); ret_effect(gb);
}

void rickyStateASubstate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7159, specialObjectAnimate_hook, 0x2aef, 0x715c);
  CYC(0x715c, 0x715e); E = 0x21;
  CYC(0x715e, 0x715f); A = mem_rd(gb, DE);
  CYC(0x715f, 0x7160); alu_or(gb, A);
  CYC(0x7160, 0x7162); A = 0xc3;
  if (F & FZ) {
    CYCT(0x7162, 0x7165);
    playSound_b00_hook(gb); return;
  }
  CYC(0x7162, 0x7165);
  CYC(0x7165, 0x7166); A = mem_rd(gb, DE);
  CYC(0x7166, 0x7167); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x7167, 0x7168); ret_effect(gb); return; }
  CYC(0x7167, 0x7168);
  CALL_C(0x7168, rickySetJumpSpeedForCutsceneAndSetAngle_hook, 0x713a, 0x716b);
  CYC(0x716b, 0x716d); E = 0x09;
  CYC(0x716d, 0x716f); A = 0x10;
  CYC(0x716f, 0x7170); mem_wr(gb, DE, A);
  CYC(0x7170, 0x7172); C = 0x05;
  CALL_C(0x7172, companionSetAnimation_hook, 0x458e, 0x7175);
  CYC(0x7175, 0x7178); rickyIncVar03_hook(gb);
}

void rickyStateASubstate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7178, retIfTextIsActive_hook, 0x1859, 0x717b);
  CYC(0x717b, 0x717d); A = 0x14;
  CYC(0x717d, 0x717f); E = 0x09;
  CYC(0x717f, 0x7180); mem_wr(gb, DE, A);
  CYC(0x7180, 0x7181); E = alu_dec8(gb, E);
  CYC(0x7181, 0x7183); A = 0x02;
  CYC(0x7183, 0x7184); mem_wr(gb, DE, A);
  CYC(0x7184, 0x7186); C = 0x05;
  CALL_C(0x7186, companionSetAnimation_hook, 0x458e, 0x7189);
  CYC(0x7189, 0x718c); rickyIncVar03_hook(gb);
}

void rickyStateASubstate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x718c, specialObjectAnimate_hook, 0x2aef, 0x718f);
  CALL_C(0x718f, objectApplySpeed_hook, 0x201d, 0x7192);
  CYC(0x7192, 0x7194); C = 0x40;
  CALL_C(0x7194, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7197);
  if (!(F & FZ)) { CYCT(0x7197, 0x7198); ret_effect(gb); return; }
  CYC(0x7197, 0x7198);
  CYC(0x7198, 0x719a); A = 0x18;
  CALL_C(0x719a, specialObjectSetAnimation_hook, 0x2b0a, 0x719d);
  CYC(0x719d, 0x71a0); rickyIncVar03_hook(gb);
}

void rickyStateASubstate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71a0, companionSetAnimationToVar3f_hook, 0x48a3, 0x71a3);
  CALL_C(0x71a3, rickyWaitUntilJumpDone_hook, 0x7259, 0x71a6);
  if (!(F & FZ)) { CYCT(0x71a6, 0x71a7); ret_effect(gb); return; }
  CYC(0x71a6, 0x71a7);
  CYC(0x71a7, 0x71a9); A = 0x18;
  CYC(0x71a9, 0x71ab); E = 0x09;
  CYC(0x71ab, 0x71ac); mem_wr(gb, DE, A);
  CALL_C(0x71ac, specialObjectCheckMovingTowardWall_hook, 0x451b, 0x71af);
  if (F & FZ) {
    CYCT(0x71af, 0x71b1);
    goto hop;
  }
  CYC(0x71af, 0x71b1);
  CYC(0x71b1, 0x71b3); A = 0x10;
  CYC(0x71b3, 0x71b5); E = 0x09;
  CYC(0x71b5, 0x71b6); mem_wr(gb, DE, A);
  CALL_C(0x71b6, specialObjectCheckMovingTowardWall_hook, 0x451b, 0x71b9);
  if (F & FZ) {
    CYCT(0x71b9, 0x71bb);
    goto hop;
  }
  CYC(0x71b9, 0x71bb);
  CALL_C(0x71bb, rickySetJumpSpeed_hook, 0x7344, 0x71be);
  CYC(0x71be, 0x71c0); A = 0x53;
  CALL_C(0x71c0, playSound_b00_hook, 0x0c98, 0x71c3);
  CYC(0x71c3, 0x71c6); rickyIncVar03_hook(gb);
  return;

hop:
  CALL_C(0x71c6, objectCheckWithinScreenBoundary_hook, 0x2184, 0x71c9);
  if (!(F & FC)) {
    CYCT(0x71c9, 0x71cb);
    goto left_screen;
  }
  CYC(0x71c9, 0x71cb);
  CYC(0x71cb, 0x71cd); E = 0x03;
  CYC(0x71cd, 0x71ce); A = mem_rd(gb, DE);
  CYC(0x71ce, 0x71d0); alu_cp(gb, 0x07);
  CYC(0x71d0, 0x71d2); A = 0x10;
  if (F & FZ) {
    CYCT(0x71d2, 0x71d4);
    goto set_angle;
  }
  CYC(0x71d2, 0x71d4);
  CYC(0x71d4, 0x71d6); A = 0x14;

set_angle:
  CYC(0x71d6, 0x71d8); E = 0x09;
  CYC(0x71d8, 0x71d9); mem_wr(gb, DE, A);
  CYC(0x71d9, 0x71dc); rickySetJumpSpeedForCutscene_hook(gb);
  return;

left_screen:
  CYC(0x71dc, 0x71dd); alu_xor(gb, A);
  CYC(0x71dd, 0x71e0); W8(wDisabledObjects) = A;
  CYC(0x71e0, 0x71e3); W8(wMenuDisabled) = A;
  CYC(0x71e3, 0x71e6); W8(wDeathRespawnBuffer_rememberedCompanionId) = A;
  CALL_C(0x71e6, itemDelete_hook, 0x2ce2, 0x71e9);
  CYC(0x71e9, 0x71ec); SET_HL(wRickyState);
  CYC(0x71ec, 0x71ee); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(0x71ee, 0x71f1); saveLinkLocalRespawnAndCompanionPosition_hook(gb);
}

void rickyIncVar03_hook(GB *gb) {
  CYC(0x71f1, 0x71f3); E = 0x03;
  CYC(0x71f3, 0x71f4); A = mem_rd(gb, DE);
  CYC(0x71f4, 0x71f5); A = alu_inc8(gb, A);
  CYC(0x71f5, 0x71f6); mem_wr(gb, DE, A);
  CYC(0x71f6, 0x71f7); ret_effect(gb);
}

void rickyStateASubstateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71f7, retIfTextIsActive_hook, 0x1859, 0x71fa);
  CALL_C(0x71fa, companionDismount_hook, 0x4630, 0x71fd);
  CYC(0x71fd, 0x71ff); A = 0x18;
  CYC(0x71ff, 0x7202); W8(w1Link_angle) = A;
  CYC(0x7202, 0x7205); W8(wLinkAngle) = A;
  CYC(0x7205, 0x7207); A = 0x32;
  CYC(0x7207, 0x720a); W8(w1Link_speed) = A;
  CYC(0x720a, 0x720b); H = D;
  CYC(0x720b, 0x720d); L = 0x09;
  CYC(0x720d, 0x720f); A = 0x18;
  CYC(0x720f, 0x7210); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x7210, 0x7212); A = 0x03;
  CYC(0x7212, 0x7213); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x7213, 0x7215); A = 0x1e;
  CYC(0x7215, 0x7216); mem_wr(gb, HL, A);
  CYC(0x7216, 0x7218); A = 0x24;
  CALL_C(0x7218, specialObjectSetAnimation_hook, 0x2b0a, 0x721b);
  CYC(0x721b, 0x721d); rickyIncVar03_hook(gb);
}

void rickyStateASubstateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x721d, 0x7220); A = W8(wLinkInAir);
  CYC(0x7220, 0x7221); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7221, 0x7222); ret_effect(gb); return; }
  CYC(0x7221, 0x7222);
  CALL_C(0x7222, setLinkForceStateToState08_hook, 0x2aad, 0x7225);
  CYC(0x7225, 0x7228); SET_HL(w1Link_xh);
  CYC(0x7228, 0x722a); E = 0x0d;
  CYC(0x722a, 0x722b); A = mem_rd(gb, DE);
  CYC(0x722b, 0x722d); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x722d, 0x722f);
    goto facing_right;
  }
  CYC(0x722d, 0x722f);
  CYC(0x722f, 0x7230); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x7230, 0x7232); A = 0x01;
  if (!(F & FC)) {
    CYCT(0x7232, 0x7234);
    goto set_direction;
  }
  CYC(0x7232, 0x7234);

facing_right:
  CYC(0x7234, 0x7236); A = 0x03;

set_direction:
  CYC(0x7236, 0x7238); L = 0x08;
  CYC(0x7238, 0x7239); mem_wr(gb, HL, A);
  CYC(0x7239, 0x723b); E = 0x07;
  CYC(0x723b, 0x723c); A = mem_rd(gb, DE);
  CYC(0x723c, 0x723d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x723d, 0x723f);
    goto move_companion;
  }
  CYC(0x723d, 0x723f);
  CYC(0x723f, 0x7240); A = alu_dec8(gb, A);
  CYC(0x7240, 0x7241); mem_wr(gb, DE, A);
  CYC(0x7241, 0x7242); ret_effect(gb); return;

move_companion:
  CALL_C(0x7242, specialObjectAnimate_hook, 0x2aef, 0x7245);
  CALL_C(0x7245, companionUpdateMovement_hook, 0x446b, 0x7248);
  CALL_C(0x7248, objectCheckWithinScreenBoundary_hook, 0x2184, 0x724b);
  if (F & FC) { CYCT(0x724b, 0x724c); ret_effect(gb); return; }
  CYC(0x724b, 0x724c);
  CYC(0x724c, 0x724d); alu_xor(gb, A);
  CYC(0x724d, 0x7250); W8(wRememberedCompanionId) = A;
  CYC(0x7250, 0x7253); W8(wDisabledObjects) = A;
  CYC(0x7253, 0x7256); W8(wMenuDisabled) = A;
  CYC(0x7256, 0x7259); itemDelete_hook(gb);
}

void rickyWaitUntilJumpDone_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7259, 0x725b); C = 0x40;
  CALL_C(0x725b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x725e);
  if (F & FZ) {
    CYCT(0x725e, 0x7260);
    goto on_ground;
  }
  CYC(0x725e, 0x7260);
  CALL_C(0x7260, companionUpdateMovement_hook, 0x446b, 0x7263);
  CYC(0x7263, 0x7264); alu_or(gb, D);
  CYC(0x7264, 0x7265); ret_effect(gb); return;

on_ground:
  CYC(0x7265, 0x7267); C = 0x05;
  CALL_C(0x7267, companionSetAnimation_hook, 0x458e, 0x726a);
  CYC(0x726a, 0x726d); companionDecCounter1IfNonzero_hook(gb);
}

void rickyStateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x726d, 0x726f); E = 0x03;
  CYC(0x726f, 0x7270); A = mem_rd(gb, DE);
  CYC(0x7270, 0x7271); push_effect(gb, 0x7271);
  switch (ricky_jump_table(gb)) {
    case 0x7275: goto parameter0;
    case 0x7287: goto parameter1;
    default: hook_continue(gb, HL, sp0_); return;
  }

parameter0:
  CALL_C(0x7275, companionInitializeOnEnteringScreen_hook, 0x4973, 0x7278);
  CYC(0x7278, 0x727a); mem_wr(gb, HL, 0x02);
  CALL_C(0x727a, rickySetJumpSpeedForCutscene_hook, 0x7146, 0x727d);
  CYC(0x727d, 0x727f); A = 0xc3;
  CALL_C(0x727f, playSound_b00_hook, 0x0c98, 0x7282);
  CYC(0x7282, 0x7284); C = 0x01;
  CYC(0x7284, 0x7287); companionSetAnimation_hook(gb);
  return;

parameter1:
  CALL_C(0x7287, rickyState5_hook, 0x6e24, 0x728a);
  CYC(0x728a, 0x728c); E = 0x04;
  CYC(0x728c, 0x728d); A = mem_rd(gb, DE);
  CYC(0x728d, 0x728f); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x728f, 0x7290); ret_effect(gb); return; }
  CYC(0x728f, 0x7290);
  CYC(0x7290, 0x7292); A = 0x0c;
  CYC(0x7292, 0x7293); mem_wr(gb, DE, A);
  CYC(0x7293, 0x7294); E = alu_inc8(gb, E);
  CYC(0x7294, 0x7295); A = mem_rd(gb, DE);
  CYC(0x7295, 0x7297); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x7297, 0x7298); ret_effect(gb); return; }
  CYC(0x7297, 0x7298);
  CALL_C(0x7298, rickyBreakTilesOnLanding_hook, 0x7314, 0x729b);
  CYC(0x729b, 0x729e); SET_HL(rickyHoleCheckOffsets_bank05);
  CALL_C(0x729e, specialObjectGetRelativeTileWithDirectionTable_hook, 0x44f1, 0x72a1);
  CYC(0x72a1, 0x72a2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x72a2, 0x72a4);
    goto initialize_ricky;
  }
  CYC(0x72a2, 0x72a4);
  CALL_C(0x72a4, itemDecCounter2_hook, 0x23db, 0x72a7);
  if (F & FZ) {
    CYCT(0x72a7, 0x72a9);
    goto initialize_ricky;
  }
  CYC(0x72a7, 0x72a9);
  CALL_C(0x72a9, rickySetJumpSpeedForCutscene_hook, 0x7146, 0x72ac);
  CYC(0x72ac, 0x72ae); C = 0x01;
  CYC(0x72ae, 0x72b1); companionSetAnimation_hook(gb);
  return;

initialize_ricky:
  CYC(0x72b1, 0x72b3); E = 0x03;
  CYC(0x72b3, 0x72b4); alu_xor(gb, A);
  CYC(0x72b4, 0x72b5); mem_wr(gb, DE, A);
  CYC(0x72b5, 0x72b8); rickyState0_hook(gb);
}

void rickyCheckHopUpCliff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72b8, 0x72ba); E = 0x33;
  CYC(0x72ba, 0x72bb); A = mem_rd(gb, DE);
  CYC(0x72bb, 0x72bd); alu_and(gb, 0xc0);
  CYC(0x72bd, 0x72bf); alu_cp(gb, 0xc0);
  if (!(F & FZ)) { CYCT(0x72bf, 0x72c0); ret_effect(gb); return; }
  CYC(0x72bf, 0x72c0);
  CYC(0x72c0, 0x72c3); A = W8(wLinkAngle);
  CYC(0x72c3, 0x72c5); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(0x72c5, 0x72c6); ret_effect(gb); return; }
  CYC(0x72c5, 0x72c6);
  CYC(0x72c6, 0x72c9); SET_HL(rickyCliffOffsets_bank05);
  CALL_C(0x72c9, specialObjectGetRelativeTileFromHl_hook, 0x44f5, 0x72cc);
  CYC(0x72cc, 0x72ce); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x72ce, 0x72d0);
    goto check_one_up_left;
  }
  CYC(0x72ce, 0x72d0);
  CYC(0x72d0, 0x72d1); A = B;
  CYC(0x72d1, 0x72d3); alu_cp(gb, 0xd4);
  if (!(F & FZ)) {
    CYCT(0x72d3, 0x72d5);
    goto try_two_tiles_up;
  }
  CYC(0x72d3, 0x72d5);

check_one_up_left:
  CYC(0x72d5, 0x72d8); SET_HL(rickyCliffOffsets_bank05 + 2);
  CALL_C(0x72d8, specialObjectGetRelativeTileFromHl_hook, 0x44f5, 0x72db);
  CYC(0x72db, 0x72dd); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x72dd, 0x72df);
    goto can_jump;
  }
  CYC(0x72dd, 0x72df);
  CYC(0x72df, 0x72e0); A = B;
  CYC(0x72e0, 0x72e2); alu_cp(gb, 0xd4);
  if (F & FZ) {
    CYCT(0x72e2, 0x72e4);
    goto can_jump;
  }
  CYC(0x72e2, 0x72e4);

try_two_tiles_up:
  CYC(0x72e4, 0x72e7); SET_HL(rickyCliffOffsets_bank05 + 4);
  CALL_C(0x72e7, specialObjectGetRelativeTileFromHl_hook, 0x44f5, 0x72ea);
  CYC(0x72ea, 0x72ec); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x72ec, 0x72ee);
    goto check_two_up_left;
  }
  CYC(0x72ec, 0x72ee);
  CYC(0x72ee, 0x72ef); A = B;
  CYC(0x72ef, 0x72f1); alu_cp(gb, 0xd4);
  if (!(F & FZ)) { CYCT(0x72f1, 0x72f2); ret_effect(gb); return; }
  CYC(0x72f1, 0x72f2);

check_two_up_left:
  CYC(0x72f2, 0x72f5); SET_HL(rickyCliffOffsets_bank05 + 6);
  CALL_C(0x72f5, specialObjectGetRelativeTileFromHl_hook, 0x44f5, 0x72f8);
  CYC(0x72f8, 0x72fa); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x72fa, 0x72fc);
    goto can_jump;
  }
  CYC(0x72fa, 0x72fc);
  CYC(0x72fc, 0x72fd); A = B;
  CYC(0x72fd, 0x72ff); alu_cp(gb, 0xd4);
  if (!(F & FZ)) { CYCT(0x72ff, 0x7300); ret_effect(gb); return; }
  CYC(0x72ff, 0x7300);

can_jump:
  CYC(0x7300, 0x7302); E = 0x04;
  CYC(0x7302, 0x7304); A = 0x02;
  CYC(0x7304, 0x7305); mem_wr(gb, DE, A);
  CYC(0x7305, 0x7306); E = alu_inc8(gb, E);
  CYC(0x7306, 0x7307); alu_xor(gb, A);
  CYC(0x7307, 0x7308); mem_wr(gb, DE, A);
  CYC(0x7308, 0x730a); E = 0x07;
  CYC(0x730a, 0x730b); mem_wr(gb, DE, A);
  CYC(0x730b, 0x730c); ret_effect(gb);
}

void rickyBreakTilesOnLanding_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7314, 0x7317); SET_HL(0x7330);
next:
  CYC(0x7317, 0x7318); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7318, 0x7319); B = A;
  CYC(0x7319, 0x731a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x731a, 0x731b); C = A;
  CYC(0x731b, 0x731c); alu_or(gb, B);
  if (F & FZ) { CYCT(0x731c, 0x731d); ret_effect(gb); return; }
  CYC(0x731c, 0x731d);
  CYC(0x731d, 0x731e); push_effect(gb, HL);
  CYC(0x731e, 0x7321); A = W8(w1Companion_yh);
  CYC(0x7321, 0x7322); alu_add(gb, B);
  CYC(0x7322, 0x7323); B = A;
  CYC(0x7323, 0x7326); A = W8(w1Companion_xh);
  CYC(0x7326, 0x7327); alu_add(gb, C);
  CYC(0x7327, 0x7328); C = A;
  CYC(0x7328, 0x732a); A = 0x10;
  CALL_C(0x732a, tryToBreakTile_hook, 0x2bf6, 0x732d);
  CYC(0x732d, 0x732e); SET_HL(pop_effect(gb));
  CYC(0x732e, 0x7330);
  goto next;
}

void rickyBeginJumpOverHole_hook(GB *gb) {
  CYC(0x733a, 0x733c); A = 0x01;
  CYC(0x733c, 0x733f); W8(wLinkInAir) = A;
  rickySetJumpSpeed_andcc91_hook(gb);
}

void rickySetJumpSpeed_andcc91_hook(GB *gb) {
  CYC(0x733f, 0x7341); A = 0x01;
  CYC(0x7341, 0x7344); W8(wDisableScreenTransitions) = A;
  rickySetJumpSpeed_hook(gb);
}

void rickySetJumpSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7344, 0x7347); SET_BC(0xfd00);
  CALL_C(0x7347, objectSetSpeedZ_hook, 0x239d, 0x734a);
  CYC(0x734a, 0x734c); L = 0x06;
  CYC(0x734c, 0x734e); mem_wr(gb, HL, 0x08);
  CYC(0x734e, 0x7350); L = 0x10;
  CYC(0x7350, 0x7352); mem_wr(gb, HL, 0x32);
  CYC(0x7352, 0x7354); C = 0x0f;
  CALL_C(0x7354, companionSetAnimation_hook, 0x458e, 0x7357);
  CYC(0x7357, 0x7358); H = D;
  CYC(0x7358, 0x7359); ret_effect(gb);
}

void rickyCheckAtScreenEdge_hook(GB *gb) {
  CYC(0x7359, 0x735a); H = D;
  CYC(0x735a, 0x735c); L = 0x0b;
  CYC(0x735c, 0x735e); A = 0x06;
  CYC(0x735e, 0x735f); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x735f, 0x7361);
    goto outsideScreen;
  }
  CYC(0x735f, 0x7361);
  CYC(0x7361, 0x7364); A = W8(wScreenTransitionBoundaryY);
  CYC(0x7364, 0x7365); A = alu_dec8(gb, A);
  CYC(0x7365, 0x7366); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x7366, 0x7368);
    goto outsideScreen;
  }
  CYC(0x7366, 0x7368);
  CYC(0x7368, 0x736a); L = 0x0d;
  CYC(0x736a, 0x736c); A = 0x06;
  CYC(0x736c, 0x736d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x736d, 0x736f);
    goto outsideScreen;
  }
  CYC(0x736d, 0x736f);
  CYC(0x736f, 0x7372); A = W8(wScreenTransitionBoundaryX);
  CYC(0x7372, 0x7373); A = alu_dec8(gb, A);
  CYC(0x7373, 0x7374); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x7374, 0x7376);
    goto outsideScreen;
  }
  CYC(0x7374, 0x7376);
  CYC(0x7376, 0x7377); alu_xor(gb, A);
  CYC(0x7377, 0x7378); ret_effect(gb); return;
outsideScreen:
  CYC(0x7378, 0x7379); alu_or(gb, D);
  CYC(0x7379, 0x737a); ret_effect(gb);
}
