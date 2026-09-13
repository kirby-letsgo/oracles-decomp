#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

void specialObjectCode_moosh__runState_hook(GB *gb);
void mooshState0_hook(GB *gb);
void mooshState1_hook(GB *gb);
void mooshCheckHazards_hook(GB *gb);
void mooshState2_hook(GB *gb);
void mooshState3_hook(GB *gb);
void mooshState4_hook(GB *gb);
void mooshTryToBreakTileFromMovingAndCheckHazards_hook(GB *gb);
void mooshSetVar37ForHazard_hook(GB *gb);
void mooshState5_hook(GB *gb);
void mooshLandOnGroundAndGotoState5_hook(GB *gb);
void mooshPressedAButton_hook(GB *gb);
void mooshState8_hook(GB *gb);
void mooshState8Substate0_hook(GB *gb);
void mooshState8Substate1_hook(GB *gb);
void mooshState8Substate2_hook(GB *gb);
void mooshState8Substate3_hook(GB *gb);
void mooshState8Substate4_hook(GB *gb);
void mooshState8Substate5_hook(GB *gb);
void mooshState6_hook(GB *gb);
void mooshState7_hook(GB *gb);
void mooshStateC_hook(GB *gb);
void mooshStateA_hook(GB *gb);
void mooshStateASubstate1_hook(GB *gb);
void mooshStateASubstate3_hook(GB *gb);
void mooshStateASubstate4_hook(GB *gb);
void mooshStateASubstate5_hook(GB *gb);
void mooshStateASubstate6_hook(GB *gb);
void mooshUpdateAsNpc_hook(GB *gb);
void mooshIncVar03_hook(GB *gb);

static uint16_t moosh_jump_table(GB *gb) {
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

void specialObjectCode_moosh_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7865, companionRetIfInactive_hook, 0x4883, 0x7868);
  CALL_C(0x7868, companionFunc_47d8_hook, 0x47d8, 0x786b);
  CALL_C(0x786b, specialObjectCode_moosh__runState_hook, 0x7871, 0x786e);
  CYC(0x786e, 0x7871); companionCheckEnableTerrainEffects_hook(gb);
}

void specialObjectCode_moosh__runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7871, 0x7873); E = 0x04;
  CYC(0x7873, 0x7874); A = mem_rd(gb, DE);
  CYC(0x7874, 0x7875); push_effect(gb, 0x7875);
  switch (moosh_jump_table(gb)) {
    case 0x788f: mooshState0_hook(gb); return;
    case 0x78d4: mooshState1_hook(gb); return;
    case 0x799c: mooshState2_hook(gb); return;
    case 0x78e8: mooshState3_hook(gb); return;
    case 0x78f4: mooshState4_hook(gb); return;
    case 0x794b: mooshState5_hook(gb); return;
    case 0x7aff: mooshState6_hook(gb); return;
    case 0x7b2d: mooshState7_hook(gb); return;
    case 0x799d: mooshState8_hook(gb); return;
    case 0x7b81: mooshStateA_hook(gb); return;
    case 0x7b4a: mooshStateC_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void mooshState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x788f, companionCheckCanSpawn_hook, 0x4822, 0x7892);
  CYC(0x7892, 0x7894); A = 0x06;
  CALL_C(0x7894, objectSetCollideRadius_hook, 0x24a1, 0x7897);
  CYC(0x7897, 0x7899); A = 0x02;
  CYC(0x7899, 0x789b); L = 0x08;
  CYC(0x789b, 0x789c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x789c, 0x789d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x789d, 0x78a0); SET_HL(wMooshState);
  CYC(0x78a0, 0x78a2); A = 0x80;
  CYC(0x78a2, 0x78a3); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x78a3, 0x78a5);
    goto set_animation;
  }
  CYC(0x78a3, 0x78a5);
  CYC(0x78a5, 0x78a8); A = W8(wActiveRoom);
  CYC(0x78a8, 0x78aa); alu_cp(gb, 0x54);
  if (F & FZ) {
    CYCT(0x78aa, 0x78ac);
    goto cutscene_state;
  }
  CYC(0x78aa, 0x78ac);
  CYC(0x78ac, 0x78ae); A = 0x20;
  CYC(0x78ae, 0x78af); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x78af, 0x78b1);
    goto cutscene_state;
  }
  CYC(0x78af, 0x78b1);
  CYC(0x78b1, 0x78b3); A = 0x40;
  CYC(0x78b3, 0x78b4); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x78b4, 0x78b6);
    goto cutscene_state;
  }
  CYC(0x78b4, 0x78b6);
  CYC(0x78b6, 0x78b8); A = 0x52;
  CALL_C(0x78b8, checkTreasureObtained_hook, 0x1748, 0x78bb);
  if (!(F & FC)) {
    CYCT(0x78bb, 0x78bd);
    goto set_animation;
  }
  CYC(0x78bb, 0x78bd);
  CYC(0x78bd, 0x78c0); A = W8(wActiveRoom);
  CYC(0x78c0, 0x78c2); alu_cp(gb, 0x6b);
  if (!(F & FZ)) {
    CYCT(0x78c2, 0x78c4);
    goto set_animation;
  }
  CYC(0x78c2, 0x78c4);

cutscene_state:
  CYC(0x78c4, 0x78c6); E = 0x04;
  CYC(0x78c6, 0x78c8); A = 0x0a;
  CYC(0x78c8, 0x78c9); mem_wr(gb, DE, A);
  CYC(0x78c9, 0x78cc); mooshStateA_hook(gb);
  return;

set_animation:
  CYC(0x78cc, 0x78ce); C = 0x01;
  CALL_C(0x78ce, companionSetAnimation_hook, 0x458e, 0x78d1);
  CYC(0x78d1, 0x78d4); objectSetVisiblec1_hook(gb);
}

void mooshState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78d4, companionSetPriorityRelativeToLink_hook, 0x4936, 0x78d7);
  CALL_C(0x78d7, specialObjectAnimate_hook, 0x2aef, 0x78da);
  CYC(0x78da, 0x78dc); C = 0x09;
  CALL_C(0x78dc, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x78df);
  if (F & FC) {
    CYCT(0x78df, 0x78e2);
    companionTryToMount_hook(gb);
    return;
  }
  CYC(0x78df, 0x78e2);
  mooshCheckHazards_hook(gb);
}

void mooshCheckHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78e2, companionCheckHazards_hook, 0x45ec, 0x78e5);
  if (!(F & FC)) { CYCT(0x78e5, 0x78e6); ret_effect(gb); return; }
  CYC(0x78e5, 0x78e6);
  CYC(0x78e6, 0x78e8); mooshSetVar37ForHazard_hook(gb);
}

void mooshState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78e8, companionCheckMountingComplete, 0x48c1, 0x78eb);
  if (!(F & FZ)) { CYCT(0x78eb, 0x78ec); ret_effect(gb); return; }
  CYC(0x78eb, 0x78ec);
  CALL_C(0x78ec, companionFinalizeMounting_hook, 0x479e, 0x78ef);
  CYC(0x78ef, 0x78f1); C = 0x13;
  CYC(0x78f1, 0x78f4); companionSetAnimation_hook(gb);
}

void mooshState4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78f4, 0x78f5); H = D;
  CYC(0x78f5, 0x78f7); L = 0x24;
  CYC(0x78f7, 0x78f9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x78f9, 0x78fb); L = 0x37;
  CYC(0x78fb, 0x78fc); A = mem_rd(gb, HL);
  CYC(0x78fc, 0x78fe); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(0x78fe, 0x7900);
    goto update_counter;
  }
  CYC(0x78fe, 0x7900);
  CYC(0x7900, 0x7902); A = 0x0e;
  CYC(0x7902, 0x7903); mem_wr(gb, HL, A);
  CALL_C(0x7903, companionDragToCenterOfHole_hook, 0x46a0, 0x7906);
  if (!(F & FZ)) { CYCT(0x7906, 0x7907); ret_effect(gb); return; }
  CYC(0x7906, 0x7907);

update_counter:
  CALL_C(0x7907, companionDecCounter1_hook, 0x49a2, 0x790a);
  if (!(F & FZ)) {
    CYCT(0x790a, 0x790c);
    goto animate;
  }
  CYC(0x790a, 0x790c);
  CYC(0x790c, 0x790d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x790d, 0x790f); L = 0x37;
  CYC(0x790f, 0x7910); A = mem_rd(gb, HL);
  CALL_C(0x7910, specialObjectSetAnimation_hook, 0x2b0a, 0x7913);
  CYC(0x7913, 0x7915); E = 0x37;
  CYC(0x7915, 0x7916); A = mem_rd(gb, DE);
  CYC(0x7916, 0x7918); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(0x7918, 0x791a);
    goto animate;
  }
  CYC(0x7918, 0x791a);
  CYC(0x791a, 0x791c); A = 0x65;
  CYC(0x791c, 0x791f); playSound_b00_hook(gb);
  return;

animate:
  CALL_C(0x791f, companionAnimateDrowningOrFallingThenRespawn_hook, 0x4966, 0x7922);
  if (!(F & FC)) { CYCT(0x7922, 0x7923); ret_effect(gb); return; }
  CYC(0x7922, 0x7923);
  CYC(0x7923, 0x7925); C = 0x13;
  CYC(0x7925, 0x7928); A = W8(wLinkObjectIndex);
  CYC(0x7928, 0x7929); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x7929, 0x792b);
  } else {
    CYC(0x7929, 0x792b);
    CYC(0x792b, 0x792d); C = 0x01;
  }
  CYC(0x792d, 0x7930); companionUpdateDirectionAndSetAnimation_hook(gb);
}

void mooshTryToBreakTileFromMovingAndCheckHazards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7930, companionTryToBreakTileFromMoving_hook, 0x4477, 0x7933);
  CALL_C(0x7933, companionCheckHazards_hook, 0x45ec, 0x7936);
  CYC(0x7936, 0x7938); C = 0x13;
  if (!(F & FC)) {
    CYCT(0x7938, 0x793b);
    companionUpdateDirectionAndAnimate_hook(gb);
    return;
  }
  CYC(0x7938, 0x793b);
  mooshSetVar37ForHazard_hook(gb);
}

void mooshSetVar37ForHazard_hook(GB *gb) {
  CYC(0x793b, 0x793c); A = alu_dec8(gb, A);
  CYC(0x793c, 0x793e); C = 0x0d;
  if (F & FZ) {
    CYCT(0x793e, 0x7940);
  } else {
    CYC(0x793e, 0x7940);
    CYC(0x7940, 0x7942); C = 0x0e;
  }
  CYC(0x7942, 0x7944); E = 0x37;
  CYC(0x7944, 0x7945); A = C;
  CYC(0x7945, 0x7946); mem_wr(gb, DE, A);
  CYC(0x7946, 0x7948); E = 0x06;
  CYC(0x7948, 0x7949); alu_xor(gb, A);
  CYC(0x7949, 0x794a); mem_wr(gb, DE, A);
  CYC(0x794a, 0x794b); ret_effect(gb);
}

void mooshState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x794b, 0x794d); C = 0x10;
  CALL_C(0x794d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7950);
  if (!(F & FZ)) { CYCT(0x7950, 0x7951); ret_effect(gb); return; }
  CYC(0x7950, 0x7951);
  CALL_C(0x7951, companionCheckHazards_hook, 0x45ec, 0x7954);
  if (F & FC) {
    CYCT(0x7954, 0x7956);
    mooshSetVar37ForHazard_hook(gb);
    return;
  }
  CYC(0x7954, 0x7956);
  CYC(0x7956, 0x7959); A = W8(wForceCompanionDismount);
  CYC(0x7959, 0x795a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x795a, 0x795c);
    goto check_dismount;
  }
  CYC(0x795a, 0x795c);
  CYC(0x795c, 0x795f); A = W8(wGameKeysJustPressed);
  CYC(0x795f, 0x7961); alu_bit(gb, 0, A);
  if (!(F & FZ)) {
    CYCT(0x7961, 0x7963);
    mooshPressedAButton_hook(gb);
    return;
  }
  CYC(0x7961, 0x7963);
  CYC(0x7963, 0x7965); alu_bit(gb, 1, A);

check_dismount:
  if (!(F & FZ)) {
    CYCT(0x7965, 0x7968);
    companionGotoDismountState_hook(gb);
    return;
  }
  CYC(0x7965, 0x7968);
  CYC(0x7968, 0x796b); A = W8(wLinkAngle);
  CYC(0x796b, 0x796d); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x796d, 0x796e); ret_effect(gb); return; }
  CYC(0x796d, 0x796e);
  CYC(0x796e, 0x7971); SET_HL(w1Companion_angle);
  CYC(0x7971, 0x7972); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x7972, 0x7973); mem_wr(gb, HL, A);
  CYC(0x7973, 0x7975); C = 0x13;
  if (!(F & FZ)) {
    CYCT(0x7975, 0x7978);
    companionUpdateDirectionAndAnimate_hook(gb);
    return;
  }
  CYC(0x7975, 0x7978);
  CALL_C(0x7978, companionCheckHopDownCliff_hook, 0x473c, 0x797b);
  if (F & FZ) { CYCT(0x797b, 0x797c); ret_effect(gb); return; }
  CYC(0x797b, 0x797c);
  CYC(0x797c, 0x797e); E = 0x10;
  CYC(0x797e, 0x7980); A = 0x28;
  CYC(0x7980, 0x7981); mem_wr(gb, DE, A);
  CALL_C(0x7981, companionUpdateMovement_hook, 0x446b, 0x7984);
  CYC(0x7984, 0x7986); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
}

void mooshLandOnGroundAndGotoState5_hook(GB *gb) {
  CYC(0x7986, 0x7987); alu_xor(gb, A);
  CYC(0x7987, 0x798a); W8(wLinkInAir) = A;
  CYC(0x798a, 0x798c); C = 0x13;
  CYC(0x798c, 0x798f); companionSetAnimationAndGotoState5_hook(gb);
}

void mooshPressedAButton_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x798f, 0x7991); A = 0x08;
  CYC(0x7991, 0x7993); E = 0x04;
  CYC(0x7993, 0x7994); mem_wr(gb, DE, A);
  CYC(0x7994, 0x7995); E = alu_inc8(gb, E);
  CYC(0x7995, 0x7996); alu_xor(gb, A);
  CYC(0x7996, 0x7997); mem_wr(gb, DE, A);
  CYC(0x7997, 0x7999); A = 0x53;
  CALL_C(0x7999, playSound_b00_hook, 0x0c98, 0x799c);
  mooshState2_hook(gb);
}

void mooshState2_hook(GB *gb) {
  CYC(0x799c, 0x799d); ret_effect(gb);
}

void mooshState8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x799d, 0x799f); E = 0x05;
  CYC(0x799f, 0x79a0); A = mem_rd(gb, DE);
  CYC(0x79a0, 0x79a1); push_effect(gb, 0x79a1);
  switch (moosh_jump_table(gb)) {
    case 0x79ad: mooshState8Substate0_hook(gb); return;
    case 0x79c7: mooshState8Substate1_hook(gb); return;
    case 0x7a5b: mooshState8Substate2_hook(gb); return;
    case 0x7a9f: mooshState8Substate3_hook(gb); return;
    case 0x7ad8: mooshState8Substate4_hook(gb); return;
    case 0x7aeb: mooshState8Substate5_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void mooshState8Substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79ad, 0x79af); A = 0x01;
  CYC(0x79af, 0x79b0); mem_wr(gb, DE, A);
  CYC(0x79b0, 0x79b3); SET_BC(0xfec0);
  CALL_C(0x79b3, objectSetSpeedZ_hook, 0x239d, 0x79b6);
  CYC(0x79b6, 0x79b8); L = 0x10;
  CYC(0x79b8, 0x79ba); mem_wr(gb, HL, 0x28);
  CYC(0x79ba, 0x79bc); L = 0x39;
  CYC(0x79bc, 0x79be); A = 0x04;
  CYC(0x79be, 0x79bf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x79bf, 0x79c0); alu_xor(gb, A);
  CYC(0x79c0, 0x79c1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x79c1, 0x79c2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x79c2, 0x79c4); C = 0x09;
  CYC(0x79c4, 0x79c7); companionSetAnimation_hook(gb);
}

void mooshState8Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79c7, objectCheckIsOverHazard_hook, 0x2216, 0x79ca);
  CYC(0x79ca, 0x79cc); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x79cc, 0x79ce);
    goto not_over_water;
  }
  CYC(0x79cc, 0x79ce);
  CYC(0x79ce, 0x79d1); SET_BC(0x0000);
  CALL_C(0x79d1, objectSetSpeedZ_hook, 0x239d, 0x79d4);
  CYC(0x79d4, 0x79d6); L = 0x05;
  CYC(0x79d6, 0x79d8); mem_wr(gb, HL, 0x05);
  CYC(0x79d8, 0x79da); B = 0x9f;
  CALL_C(0x79da, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x79dd);
  CYC(0x79dd, 0x79de); L = alu_dec8(gb, L);
  CYC(0x79de, 0x79df); A = mem_rd(gb, HL);
  CYC(0x79df, 0x79e1); alu_sub(gb, 0x20);
  CYC(0x79e1, 0x79e2); mem_wr(gb, HL, A);
  CYC(0x79e2, 0x79e4); L = 0x46;
  CYC(0x79e4, 0x79e6); E = 0x06;
  CYC(0x79e6, 0x79e8); A = 0x3c;
  CYC(0x79e8, 0x79e9); mem_wr(gb, HL, A);
  CYC(0x79e9, 0x79ea); mem_wr(gb, DE, A);
  CYC(0x79ea, 0x79eb); ret_effect(gb);
  return;

not_over_water:
  CYC(0x79eb, 0x79ee); A = W8(wLinkAngle);
  CYC(0x79ee, 0x79f0); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x79f0, 0x79f2);
    goto check_vertical_speed;
  }
  CYC(0x79f0, 0x79f2);
  CYC(0x79f2, 0x79f5); SET_HL(w1Companion_angle);
  CYC(0x79f5, 0x79f6); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x79f6, 0x79f7); mem_wr(gb, HL, A);
  CALL_C(0x79f7, companionUpdateMovement_hook, 0x446b, 0x79fa);

check_vertical_speed:
  CYC(0x79fa, 0x79fc); E = 0x15;
  CYC(0x79fc, 0x79fd); A = mem_rd(gb, DE);
  CYC(0x79fd, 0x79fe); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x79fe, 0x7a00);
    goto moving_up;
  }
  CYC(0x79fe, 0x7a00);
  CYC(0x7a00, 0x7a02); E = 0x3b;
  CYC(0x7a02, 0x7a05); A = W8(wGameKeysPressed);
  CYC(0x7a05, 0x7a07); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x7a07, 0x7a09);
    goto store_hold_counter;
  }
  CYC(0x7a07, 0x7a09);
  CYC(0x7a09, 0x7a0a); A = mem_rd(gb, DE);
  CYC(0x7a0a, 0x7a0b); A = alu_inc8(gb, A);

store_hold_counter:
  CYC(0x7a0b, 0x7a0c); mem_wr(gb, DE, A);
  CYC(0x7a0c, 0x7a0e); alu_cp(gb, 0x0a);
  if (!(F & FC)) {
    CYCT(0x7a0e, 0x7a10);
    goto goto_substate2;
  }
  CYC(0x7a0e, 0x7a10);
  CYC(0x7a10, 0x7a13); A = W8(wGameKeysJustPressed);
  CYC(0x7a13, 0x7a15); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(0x7a15, 0x7a17);
    goto flutter_done;
  }
  CYC(0x7a15, 0x7a17);
  CYC(0x7a17, 0x7a19); E = 0x3a;
  CYC(0x7a19, 0x7a1a); A = mem_rd(gb, DE);
  CYC(0x7a1a, 0x7a1c); alu_cp(gb, 0x10);
  if (F & FZ) {
    CYCT(0x7a1c, 0x7a1e);
    goto flutter_done;
  }
  CYC(0x7a1c, 0x7a1e);
  CYC(0x7a1e, 0x7a1f); A = alu_inc8(gb, A);
  CYC(0x7a1f, 0x7a20); mem_wr(gb, DE, A);
  CYC(0x7a20, 0x7a21); E = alu_dec8(gb, E);
  CYC(0x7a21, 0x7a22); A = mem_rd(gb, DE);
  CYC(0x7a22, 0x7a24); alu_add(gb, 0x08);
  CYC(0x7a24, 0x7a25); mem_wr(gb, DE, A);
  CYC(0x7a25, 0x7a27); E = 0x20;
  CYC(0x7a27, 0x7a29); A = 0x01;
  CYC(0x7a29, 0x7a2a); mem_wr(gb, DE, A);
  CALL_C(0x7a2a, specialObjectAnimate_hook, 0x2aef, 0x7a2d);
  CYC(0x7a2d, 0x7a2f); A = 0x53;
  CALL_C(0x7a2f, playSound_b00_hook, 0x0c98, 0x7a32);

flutter_done:
  CYC(0x7a32, 0x7a34); E = 0x39;
  CYC(0x7a34, 0x7a35); A = mem_rd(gb, DE);
  CYC(0x7a35, 0x7a36); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7a36, 0x7a38);
    goto update_movement;
  }
  CYC(0x7a36, 0x7a38);
  CYC(0x7a38, 0x7a39); A = alu_dec8(gb, A);
  CYC(0x7a39, 0x7a3a); mem_wr(gb, DE, A);
  CYC(0x7a3a, 0x7a3c); E = 0x20;
  CYC(0x7a3c, 0x7a3e); A = 0x0f;
  CYC(0x7a3e, 0x7a3f); mem_wr(gb, DE, A);
  CYC(0x7a3f, 0x7a41); C = 0x09;
  CYC(0x7a41, 0x7a44); companionUpdateDirectionAndAnimate_hook(gb);
  return;

moving_up:
  CYC(0x7a44, 0x7a46); C = 0x09;
  CALL_C(0x7a46, companionUpdateDirectionAndAnimate_hook, 0x456c, 0x7a49);

update_movement:
  CYC(0x7a49, 0x7a4b); C = 0x10;
  CALL_C(0x7a4b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7a4e);
  if (!(F & FZ)) { CYCT(0x7a4e, 0x7a4f); ret_effect(gb); return; }
  CYC(0x7a4e, 0x7a4f);
  CALL_C(0x7a4f, companionTryToBreakTileFromMoving_hook, 0x4477, 0x7a52);
  CALL_C(0x7a52, mooshLandOnGroundAndGotoState5_hook, 0x7986, 0x7a55);
  CYC(0x7a55, 0x7a58); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
  return;

goto_substate2:
  CYC(0x7a58, 0x7a5b); itemIncSubstate_hook(gb);
}

void mooshState8Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a5b, specialObjectAnimate_hook, 0x2aef, 0x7a5e);
  CYC(0x7a5e, 0x7a61); A = W8(wGameKeysPressed);
  CYC(0x7a61, 0x7a63); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(0x7a63, 0x7a65);
    goto next_substate;
  }
  CYC(0x7a63, 0x7a65);
  CYC(0x7a65, 0x7a67); E = 0x3b;
  CYC(0x7a67, 0x7a68); A = mem_rd(gb, DE);
  CYC(0x7a68, 0x7a6a); alu_cp(gb, 0x28);
  if (F & FC) {
    CYCT(0x7a6a, 0x7a6c);
    goto increment_charge;
  }
  CYC(0x7a6a, 0x7a6c);
  CYC(0x7a6c, 0x7a6e); C = 0x02;
  CALL_C(0x7a6e, companionFlashFromChargingAnimation_hook, 0x48ae, 0x7a71);

increment_charge:
  CYC(0x7a71, 0x7a73); E = 0x3b;
  CYC(0x7a73, 0x7a74); A = mem_rd(gb, DE);
  CYC(0x7a74, 0x7a75); A = alu_inc8(gb, A);
  CYC(0x7a75, 0x7a76); mem_wr(gb, DE, A);
  CYC(0x7a76, 0x7a78); alu_cp(gb, 0x28);
  if (F & FC) { CYCT(0x7a78, 0x7a79); ret_effect(gb); return; }
  CYC(0x7a78, 0x7a79);
  CYC(0x7a79, 0x7a7b); A = 0x4f;
  if (F & FZ) {
    CYCT(0x7a7b, 0x7a7e);
    playSound_b00_hook(gb);
    return;
  }
  CYC(0x7a7b, 0x7a7e);
  CYC(0x7a7e, 0x7a81); SET_HL(w1Link_collisionType);
  CYC(0x7a81, 0x7a83); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CYC(0x7a83, 0x7a84); H = alu_inc8(gb, H);
  CYC(0x7a84, 0x7a86); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x80));
  CYC(0x7a86, 0x7a88); E = 0x3b;
  CYC(0x7a88, 0x7a89); A = mem_rd(gb, DE);
  CYC(0x7a89, 0x7a8b); alu_cp(gb, 0x78);
  if (!(F & FZ)) { CYCT(0x7a8b, 0x7a8c); ret_effect(gb); return; }
  CYC(0x7a8b, 0x7a8c);

next_substate:
  CYC(0x7a8c, 0x7a8f); SET_HL(w1Link_oamFlagsBackup);
  CYC(0x7a8f, 0x7a90); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7a90, 0x7a91); mem_wr(gb, HL, A);
  CALL_C(0x7a91, itemIncSubstate_hook, 0x23ef, 0x7a94);
  CYC(0x7a94, 0x7a96); C = 0x17;
  CYC(0x7a96, 0x7a98); E = 0x3b;
  CYC(0x7a98, 0x7a99); A = mem_rd(gb, DE);
  CYC(0x7a99, 0x7a9b); alu_cp(gb, 0x28);
  if (F & FC) { CYCT(0x7a9b, 0x7a9c); ret_effect(gb); return; }
  CYC(0x7a9b, 0x7a9c);
  CYC(0x7a9c, 0x7a9f); companionSetAnimation_hook(gb);
}

void mooshState8Substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a9f, 0x7aa1); C = 0x80;
  CALL_C(0x7aa1, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7aa4);
  if (!(F & FZ)) { CYCT(0x7aa4, 0x7aa5); ret_effect(gb); return; }
  CYC(0x7aa4, 0x7aa5);
  CYC(0x7aa5, 0x7aa7); E = 0x3b;
  CYC(0x7aa7, 0x7aa8); A = mem_rd(gb, DE);
  CYC(0x7aa8, 0x7aaa); alu_cp(gb, 0x28);
  if (!(F & FC)) {
    CYCT(0x7aaa, 0x7aac);
    goto charged;
  }
  CYC(0x7aaa, 0x7aac);
  CALL_C(0x7aac, mooshLandOnGroundAndGotoState5_hook, 0x7986, 0x7aaf);
  CYC(0x7aaf, 0x7ab2); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
  return;

charged:
  CALL_C(0x7ab2, companionCheckHazards_hook, 0x45ec, 0x7ab5);
  if (F & FC) {
    CYCT(0x7ab5, 0x7ab8);
    mooshSetVar37ForHazard_hook(gb);
    return;
  }
  CYC(0x7ab5, 0x7ab8);
  CALL_C(0x7ab8, itemIncSubstate_hook, 0x23ef, 0x7abb);
  CYC(0x7abb, 0x7abd); A = 0x0f;
  CYC(0x7abd, 0x7ac0); W8(wScreenShakeCounterY) = A;
  CYC(0x7ac0, 0x7ac2); A = 0xf1;
  CALL_C(0x7ac2, playSound_b00_hook, 0x0c98, 0x7ac5);
  CYC(0x7ac5, 0x7ac7); A = 0x85;
  CALL_C(0x7ac7, playSound_b00_hook, 0x0c98, 0x7aca);
  CYC(0x7aca, 0x7acc); A = 0x05;
  CYC(0x7acc, 0x7acf); SET_HL(wCompanionTutorialTextShown);
  CALL_C(0x7acf, setFlag_hook, 0x020e, 0x7ad2);
  CYC(0x7ad2, 0x7ad5); SET_BC(0x2800);
  CYC(0x7ad5, 0x7ad8); companionCreateWeaponItem_hook(gb);
}

void mooshState8Substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7ad8, specialObjectAnimate_hook, 0x2aef, 0x7adb);
  CYC(0x7adb, 0x7add); E = 0x21;
  CYC(0x7add, 0x7ade); A = mem_rd(gb, DE);
  CYC(0x7ade, 0x7adf); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x7adf, 0x7ae0); ret_effect(gb); return; }
  CYC(0x7adf, 0x7ae0);
  CYC(0x7ae0, 0x7ae3); SET_HL(w1Link_collisionType);
  CYC(0x7ae3, 0x7ae5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x7ae5, 0x7ae6); H = alu_inc8(gb, H);
  CYC(0x7ae6, 0x7ae8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x7ae8, 0x7aeb); mooshLandOnGroundAndGotoState5_hook(gb);
}

void mooshState8Substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7aeb, companionDecCounter1IfNonzero_hook, 0x495e, 0x7aee);
  if (F & FZ) {
    CYCT(0x7aee, 0x7af0);
    goto descend;
  }
  CYC(0x7aee, 0x7af0);
  CYC(0x7af0, 0x7af3); specialObjectAnimate_hook(gb);
  return;

descend:
  CYC(0x7af3, 0x7af5); C = 0x10;
  CALL_C(0x7af5, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7af8);
  if (!(F & FZ)) { CYCT(0x7af8, 0x7af9); ret_effect(gb); return; }
  CYC(0x7af8, 0x7af9);
  CALL_C(0x7af9, mooshLandOnGroundAndGotoState5_hook, 0x7986, 0x7afc);
  CYC(0x7afc, 0x7aff); mooshTryToBreakTileFromMovingAndCheckHazards_hook(gb);
}

void mooshState6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7aff, 0x7b01); E = 0x05;
  CYC(0x7b01, 0x7b02); A = mem_rd(gb, DE);
  CYC(0x7b02, 0x7b03); push_effect(gb, 0x7b03);
  switch (moosh_jump_table(gb)) {
    case 0x7b09: goto substate0;
    case 0x7b14: goto substate1;
    case 0x7b1c: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7b09, 0x7b0b); A = 0x01;
  CYC(0x7b0b, 0x7b0c); mem_wr(gb, DE, A);
  CALL_C(0x7b0c, companionDismountAndSavePosition_hook, 0x460c, 0x7b0f);
  CYC(0x7b0f, 0x7b11); C = 0x01;
  CYC(0x7b11, 0x7b14); companionSetAnimation_hook(gb);
  return;

substate1:
  CYC(0x7b14, 0x7b17); A = W8(wLinkInAir);
  CYC(0x7b17, 0x7b18); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7b18, 0x7b19); ret_effect(gb); return; }
  CYC(0x7b18, 0x7b19);
  CYC(0x7b19, 0x7b1c); itemIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x7b1c, 0x7b1e); C = 0x09;
  CALL_C(0x7b1e, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x7b21);
  if (F & FC) {
    CYCT(0x7b21, 0x7b24);
    mooshCheckHazards_hook(gb);
    return;
  }
  CYC(0x7b21, 0x7b24);
  CYC(0x7b24, 0x7b26); E = 0x05;
  CYC(0x7b26, 0x7b27); alu_xor(gb, A);
  CYC(0x7b27, 0x7b28); mem_wr(gb, DE, A);
  CYC(0x7b28, 0x7b29); E = alu_dec8(gb, E);
  CYC(0x7b29, 0x7b2b); A = 0x01;
  CYC(0x7b2b, 0x7b2c); mem_wr(gb, DE, A);
  CYC(0x7b2c, 0x7b2d); ret_effect(gb);
}

void mooshState7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b2d, companionDecCounter1ToJumpDownCliff_hook, 0x493f, 0x7b30);
  if (!(F & FC)) {
    CYCT(0x7b30, 0x7b32);
    goto check_walls;
  }
  CYC(0x7b30, 0x7b32);
  if (!(F & FZ)) { CYCT(0x7b32, 0x7b33); ret_effect(gb); return; }
  CYC(0x7b32, 0x7b33);
  CYC(0x7b33, 0x7b35); C = 0x09;
  CYC(0x7b35, 0x7b38); companionSetAnimation_hook(gb);
  return;

check_walls:
  CALL_C(0x7b38, companionCalculateAdjacentWallsBitset_hook, 0x4486, 0x7b3b);
  CALL_C(0x7b3b, specialObjectCheckMovingAwayFromWall_hook, 0x4500, 0x7b3e);
  CYC(0x7b3e, 0x7b40); E = 0x07;
  if (F & FZ) {
    CYCT(0x7b40, 0x7b42);
    goto stopped;
  }
  CYC(0x7b40, 0x7b42);
  CYC(0x7b42, 0x7b43); mem_wr(gb, DE, A);
  CYC(0x7b43, 0x7b44); ret_effect(gb);
  return;

stopped:
  CYC(0x7b44, 0x7b45); A = mem_rd(gb, DE);
  CYC(0x7b45, 0x7b46); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7b46, 0x7b47); ret_effect(gb); return; }
  CYC(0x7b46, 0x7b47);
  CYC(0x7b47, 0x7b4a); mooshLandOnGroundAndGotoState5_hook(gb);
}

void mooshStateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b4a, 0x7b4c); E = 0x03;
  CYC(0x7b4c, 0x7b4d); A = mem_rd(gb, DE);
  CYC(0x7b4d, 0x7b4e); push_effect(gb, 0x7b4e);
  switch (moosh_jump_table(gb)) {
    case 0x7b52: goto substate0;
    case 0x7b61: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x7b52, companionInitializeOnEnteringScreen_hook, 0x4973, 0x7b55);
  CYC(0x7b55, 0x7b57); mem_wr(gb, HL, 0x3c);
  CYC(0x7b57, 0x7b59); A = 0xc5;
  CALL_C(0x7b59, playSound_b00_hook, 0x0c98, 0x7b5c);
  CYC(0x7b5c, 0x7b5e); C = 0x0f;
  CYC(0x7b5e, 0x7b61); companionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x7b61, specialObjectAnimate_hook, 0x2aef, 0x7b64);
  CYC(0x7b64, 0x7b66); E = 0x10;
  CYC(0x7b66, 0x7b68); A = 0x1e;
  CYC(0x7b68, 0x7b69); mem_wr(gb, DE, A);
  CALL_C(0x7b69, companionUpdateMovement_hook, 0x446b, 0x7b6c);
  CYC(0x7b6c, 0x7b6f); SET_HL(0x7b79);
  CALL_C(0x7b6f, companionRetIfNotFinishedWalkingIn_hook, 0x4982, 0x7b72);
  CYC(0x7b72, 0x7b74); E = 0x03;
  CYC(0x7b74, 0x7b75); alu_xor(gb, A);
  CYC(0x7b75, 0x7b76); mem_wr(gb, DE, A);
  CYC(0x7b76, 0x7b79); mooshState0_hook(gb);
}

void mooshStateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b81, 0x7b83); E = 0x03;
  CYC(0x7b83, 0x7b84); A = mem_rd(gb, DE);
  CYC(0x7b84, 0x7b85); push_effect(gb, 0x7b85);
  switch (moosh_jump_table(gb)) {
    case 0x7b93: goto substate0;
    case 0x7bd2: mooshStateASubstate1_hook(gb); return;
    case 0x7ba3: goto substate2;
    case 0x7bee: mooshStateASubstate3_hook(gb); return;
    case 0x7c0e: mooshStateASubstate4_hook(gb); return;
    case 0x7c17: mooshStateASubstate5_hook(gb); return;
    case 0x7c30: mooshStateASubstate6_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7b93, 0x7b95); A = 0x01;
  CYC(0x7b95, 0x7b96); mem_wr(gb, DE, A);
  CYC(0x7b96, 0x7b99); SET_HL(wMooshState);
  CYC(0x7b99, 0x7b9b); A = 0x20;
  CYC(0x7b9b, 0x7b9c); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7b9c, 0x7b9e);
    goto select_animation;
  }
  CYC(0x7b9c, 0x7b9e);
  CYC(0x7b9e, 0x7ba0); A = 0x40;
  CYC(0x7ba0, 0x7ba1); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7ba1, 0x7ba3);
    goto disable_controls;
  }
  CYC(0x7ba1, 0x7ba3);

substate2:
  CYC(0x7ba3, 0x7ba5); A = 0x01;
  CYC(0x7ba5, 0x7ba6); mem_wr(gb, DE, A);
  CYC(0x7ba6, 0x7ba8); E = 0x3d;
  CALL_C(0x7ba8, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x7bab);

select_animation:
  CYC(0x7bab, 0x7bad); A = 0x24;
  CALL_C(0x7bad, checkGlobalFlag_hook, 0x31f3, 0x7bb0);
  CYC(0x7bb0, 0x7bb2); A = 0x00;
  if (F & FZ) {
    CYCT(0x7bb2, 0x7bb4);
    goto store_animation;
  }
  CYC(0x7bb2, 0x7bb4);
  CYC(0x7bb4, 0x7bb6); A = 0x03;

store_animation:
  CYC(0x7bb6, 0x7bb8); E = 0x3f;
  CYC(0x7bb8, 0x7bb9); mem_wr(gb, DE, A);
  CALL_C(0x7bb9, specialObjectSetAnimation_hook, 0x2b0a, 0x7bbc);
  CYC(0x7bbc, 0x7bbf); objectSetVisiblec3_hook(gb);
  return;

disable_controls:
  CYC(0x7bbf, 0x7bc1); A = 0x01;
  CYC(0x7bc1, 0x7bc4); W8(wMenuDisabled) = A;
  CYC(0x7bc4, 0x7bc7); W8(wDisabledObjects) = A;
  CYC(0x7bc7, 0x7bc9); A = 0x04;
  CYC(0x7bc9, 0x7bca); mem_wr(gb, DE, A);
  CYC(0x7bca, 0x7bcc); A = 0x01;
  CALL_C(0x7bcc, specialObjectSetAnimation_hook, 0x2b0a, 0x7bcf);
  CYC(0x7bcf, 0x7bd2); objectSetVisiblec3_hook(gb);
}

void mooshStateASubstate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7bd2, 0x7bd4); E = 0x3d;
  CYC(0x7bd4, 0x7bd5); A = mem_rd(gb, DE);
  CYC(0x7bd5, 0x7bd6); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7bd6, 0x7bd8);
    goto update;
  }
  CYC(0x7bd6, 0x7bd8);
  CYC(0x7bd8, 0x7bda); A = 0x01;
  CYC(0x7bda, 0x7bdd); W8(wDisabledObjects) = A;
  CYC(0x7bdd, 0x7be0); W8(wMenuDisabled) = A;

update:
  CALL_C(0x7be0, companionSetAnimationToVar3f_hook, 0x48a3, 0x7be3);
  CALL_C(0x7be3, mooshUpdateAsNpc_hook, 0x7c57, 0x7be6);
  CYC(0x7be6, 0x7be9); A = W8(wMooshState);
  CYC(0x7be9, 0x7beb); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x7beb, 0x7bec); ret_effect(gb); return; }
  CYC(0x7beb, 0x7bec);
  CYC(0x7bec, 0x7bee);

  CYC(0x7bff, 0x7c01); E = 0x3d;
  CYC(0x7c01, 0x7c02); alu_xor(gb, A);
  CYC(0x7c02, 0x7c03); mem_wr(gb, DE, A);
  CALL_C(0x7c03, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x7c06);
  CYC(0x7c06, 0x7c08); C = 0x01;
  CALL_C(0x7c08, companionSetAnimation_hook, 0x458e, 0x7c0b);
  CYC(0x7c0b, 0x7c0e); companionForceMount_hook(gb);
}

void mooshStateASubstate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7bee, companionSetAnimationToVar3f_hook, 0x48a3, 0x7bf1);
  CALL_C(0x7bf1, mooshUpdateAsNpc_hook, 0x7c57, 0x7bf4);
  CYC(0x7bf4, 0x7bf7); A = W8(wMooshState);
  CYC(0x7bf7, 0x7bf9); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(0x7bf9, 0x7bfa); ret_effect(gb); return; }
  CYC(0x7bf9, 0x7bfa);
  CYC(0x7bfa, 0x7bfc); A = 0xff;
  CYC(0x7bfc, 0x7bff); W8(wStatusBarNeedsRefresh) = A;
  CYC(0x7bff, 0x7c01); E = 0x3d;
  CYC(0x7c01, 0x7c02); alu_xor(gb, A);
  CYC(0x7c02, 0x7c03); mem_wr(gb, DE, A);
  CALL_C(0x7c03, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x7c06);
  CYC(0x7c06, 0x7c08); C = 0x01;
  CALL_C(0x7c08, companionSetAnimation_hook, 0x458e, 0x7c0b);
  CYC(0x7c0b, 0x7c0e); companionForceMount_hook(gb);
}

void mooshStateASubstate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c0e, mooshIncVar03_hook, 0x7c60, 0x7c11);
  CYC(0x7c11, 0x7c14); SET_BC(0x2208);
  CYC(0x7c14, 0x7c17); showText_hook(gb);
}

void mooshStateASubstate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c17, retIfTextIsActive_hook, 0x1859, 0x7c1a);
  CYC(0x7c1a, 0x7c1d); SET_BC(0xfec0);
  CALL_C(0x7c1d, objectSetSpeedZ_hook, 0x239d, 0x7c20);
  CYC(0x7c20, 0x7c22); L = 0x09;
  CYC(0x7c22, 0x7c24); mem_wr(gb, HL, 0x10);
  CYC(0x7c24, 0x7c26); L = 0x10;
  CYC(0x7c26, 0x7c28); mem_wr(gb, HL, 0x28);
  CYC(0x7c28, 0x7c2a); A = 0x0b;
  CALL_C(0x7c2a, specialObjectSetAnimation_hook, 0x2b0a, 0x7c2d);
  CYC(0x7c2d, 0x7c30); mooshIncVar03_hook(gb);
}

void mooshStateASubstate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c30, specialObjectAnimate_hook, 0x2aef, 0x7c33);
  CYC(0x7c33, 0x7c35); E = 0x15;
  CYC(0x7c35, 0x7c36); A = mem_rd(gb, DE);
  CYC(0x7c36, 0x7c37); alu_or(gb, A);
  CYC(0x7c37, 0x7c39); C = 0x10;
  if (!(F & FZ)) {
    CYCT(0x7c39, 0x7c3c);
    objectUpdateSpeedZ_paramC_hook(gb);
    return;
  }
  CYC(0x7c39, 0x7c3c);
  CALL_C(0x7c3c, objectApplySpeed_hook, 0x201d, 0x7c3f);
  CYC(0x7c3f, 0x7c41); E = 0x0b;
  CYC(0x7c41, 0x7c42); A = mem_rd(gb, DE);
  CYC(0x7c42, 0x7c44); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(0x7c44, 0x7c45); ret_effect(gb); return; }
  CYC(0x7c44, 0x7c45);
  CYC(0x7c45, 0x7c46); alu_xor(gb, A);
  CYC(0x7c46, 0x7c49); W8(wDisabledObjects) = A;
  CYC(0x7c49, 0x7c4c); W8(wMenuDisabled) = A;
  CYC(0x7c4c, 0x7c4f); W8(wRememberedCompanionId) = A;
  CYC(0x7c4f, 0x7c52); SET_HL(wMooshState);
  CYC(0x7c52, 0x7c54); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(0x7c54, 0x7c57); itemDelete_hook(gb);
}

void mooshUpdateAsNpc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c57, companionPreventLinkFromPassing_noExtraChecks_hook, 0x4465, 0x7c5a);
  CALL_C(0x7c5a, specialObjectAnimate_hook, 0x2aef, 0x7c5d);
  CYC(0x7c5d, 0x7c60); companionSetPriorityRelativeToLink_hook(gb);
}

void mooshIncVar03_hook(GB *gb) {
  CYC(0x7c60, 0x7c62); E = 0x03;
  CYC(0x7c62, 0x7c63); A = mem_rd(gb, DE);
  CYC(0x7c63, 0x7c64); A = alu_inc8(gb, A);
  CYC(0x7c64, 0x7c65); mem_wr(gb, DE, A);
  CYC(0x7c65, 0x7c66); ret_effect(gb);
}
