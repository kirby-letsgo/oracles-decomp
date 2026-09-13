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
    case 0x7aff: mooshState6(gb); return;
    case 0x7b2d: mooshState7(gb); return;
    case 0x799d: mooshState8(gb); return;
    case 0x7b81: mooshStateA(gb); return;
    case 0x7b4a: mooshStateC(gb); return;
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
  CYC(0x78c9, 0x78cc); mooshStateA(gb);
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
