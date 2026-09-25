#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/specialObjects/moosh.s.

void s_mooshStateASubstate0_hook(GB *gb) {
  BASE(mooshStateASubstate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_7a7d; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wMooshState);
  CYC(b_+13, b_+15); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_7a7d; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x02;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); C = 0x01;
  CALL_C(b_+22, s_companionSetAnimation, SYM(companionSetAnimation), b_+25);
  CYC(b_+25, b_+27);
  goto L_7a85;
L_7a7d:
  CYC(b_+27, b_+29); A = 0x00;
  CYC(b_+29, b_+31); E = 0x3f;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CALL_C(b_+32, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+35);
L_7a85:
  CALL_C(b_+35, s_objectSetVisiblec3, SYM(objectSetVisiblec3), b_+38);
  CYC(b_+38, b_+40); E = 0x3d;
  CYC(b_+40, b_+43);
  TAIL(objectAddToAButtonSensitiveObjectList);
}

// updateLinkInvincibilityCounter@func_4244
static void moosh_func_4244(GB *gb) {
  BASE(updateLinkInvincibilityCounter);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+13, b_+16); A = mem_rd(gb, wFrameCounter);
  CYC(b_+16, b_+18); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto normalFlags; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); L = 0x1c;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x0d);
  RET(b_+24); return;

normalFlags:
  CYC(b_+26, b_+28); L = 0x1b;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  RET(b_+30); return;
}

void s_mooshStateASubstate2_hook(GB *gb) {
  BASE(mooshStateASubstate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x2b;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+11);
  moosh_func_4244(gb); return;
}

void s_mooshStateASubstate8_hook(GB *gb) {
  BASE(mooshStateASubstate8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CYC(b_+3, b_+5); E = 0x3e;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); C = 0x10;
  CYC(b_+9, b_+12);
  TAIL(objectUpdateSpeedZ_paramC);
}

void s_mooshFunc_05_7aff_hook(GB *gb) {
  BASE(mooshFunc_05_7aff);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x40;
  CYC(b_+2, b_+4); C = 0x70;
  CALL_C(b_+4, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x1c);
  CYC(b_+9, b_+11); E = 0x09;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return;
}

void s_mooshStateASubstate9_hook(GB *gb) {
  BASE(mooshStateASubstate9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+8);
  CALL_C(b_+8, s_companionUpdateMovement, SYM(companionUpdateMovement), b_+11);
  CYC(b_+11, b_+13); E = 0x0d;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x38);
  if (F & FC) { CYCT(b_+16, b_+18); TAIL_S(mooshFunc_05_7aff); }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+22); E = 0x3e;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+26);
  TAIL(mooshIncVar03);
}

void s_mooshStateASubstateA_hook(GB *gb) {
  BASE(mooshStateASubstateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CYC(b_+3, b_+5); E = 0x3e;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_BC(0x220f);
  CALL_C(b_+12, s_showText, SYM(showText), b_+15);
  CYC(b_+15, b_+18);
  TAIL(mooshIncVar03);
}

void s_mooshStateASubstateB_hook(GB *gb) {
  BASE(mooshStateASubstateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+3);
  CALL_C(b_+3, s_companionDismount, SYM(companionDismount), b_+6);
  CYC(b_+6, b_+8); A = 0x18;
  CYC(b_+8, b_+11); mem_wr(gb, w1Link_angle, A);
  CYC(b_+11, b_+14); mem_wr(gb, wLinkAngle, A);
  CYC(b_+14, b_+16); A = 0x32;
  CYC(b_+16, b_+19); mem_wr(gb, w1Link_speed, A);
  CYC(b_+19, b_+22); SET_BC(0xfec0);
  CALL_C(b_+22, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+25);
  CYC(b_+25, b_+27); L = 0x09;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x18);
  CYC(b_+29, b_+31); L = 0x06;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x1e);
  CYC(b_+33, b_+35); C = 0x0c;
  CALL_C(b_+35, s_companionSetAnimation, SYM(companionSetAnimation), b_+38);
  CYC(b_+38, b_+41);
  TAIL(mooshIncVar03);
}

void s_mooshStateASubstateC_hook(GB *gb) {
  BASE(mooshStateASubstateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0x15;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  CYC(b_+7, b_+9); C = 0x10;
  if (!(F & FZ)) CALL_C_CC(b_+9, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+12);
  else CYC(b_+9, b_+12);
  CYC(b_+12, b_+15); A = mem_rd(gb, wLinkInAir);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CALL_C(b_+17, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+20);
  CYC(b_+20, b_+23); SET_HL(w1Link_xh);
  CYC(b_+23, b_+25); E = 0x0d;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto L_7b84; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+33); A = 0x01;
  if (!(F & FC)) { CYCT(b_+33, b_+35); goto L_7b86; }
  CYC(b_+33, b_+35);
L_7b84:
  CYC(b_+35, b_+37); A = 0x03;
L_7b86:
  CYC(b_+37, b_+39); L = 0x08;
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CALL_C(b_+40, s_companionDecCounter1IfNonzero, SYM(companionDecCounter1IfNonzero), b_+43);
  if (!(F & FZ)) { RET_TAKEN(b_+43); return; }
  CYC(b_+43, b_+44);
  CALL_C(b_+44, s_companionUpdateMovement, SYM(companionUpdateMovement), b_+47);
  CALL_C(b_+47, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+50);
  if (F & FC) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CYC(b_+52, b_+55); mem_wr(gb, wRememberedCompanionId, A);
  CYC(b_+55, b_+58); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+58, b_+61);
  TAIL(itemDelete);
}


static uint16_t moosh_d_jump_table(GB *gb) {
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

// State A: cutscene stuff
void s_mooshStateA_hook(GB *gb) {
  BASE(mooshStateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moosh_d_jump_table(gb));
    if (jt_ == SYM(mooshStateASubstate0) && hook_is(gb, SYM(mooshStateASubstate0), s_mooshStateASubstate0_hook)) { s_mooshStateASubstate0_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate1) && hook_is(gb, SYM(mooshStateASubstate1), s_mooshStateASubstate1_hook)) { s_mooshStateASubstate1_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate2) && hook_is(gb, SYM(mooshStateASubstate2), s_mooshStateASubstate2_hook)) { s_mooshStateASubstate2_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate3) && hook_is(gb, SYM(mooshStateASubstate3), s_mooshStateASubstate3_hook)) { s_mooshStateASubstate3_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate4) && hook_is(gb, SYM(mooshStateASubstate4), s_mooshStateASubstate4_hook)) { s_mooshStateASubstate4_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate5) && hook_is(gb, SYM(mooshStateASubstate5), s_mooshStateASubstate5_hook)) { s_mooshStateASubstate5_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate8) && hook_is(gb, SYM(mooshStateASubstate8), s_mooshStateASubstate8_hook)) { s_mooshStateASubstate8_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstate9) && hook_is(gb, SYM(mooshStateASubstate9), s_mooshStateASubstate9_hook)) { s_mooshStateASubstate9_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstateA) && hook_is(gb, SYM(mooshStateASubstateA), s_mooshStateASubstateA_hook)) { s_mooshStateASubstateA_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstateB) && hook_is(gb, SYM(mooshStateASubstateB), s_mooshStateASubstateB_hook)) { s_mooshStateASubstateB_hook(gb); return; }
    if (jt_ == SYM(mooshStateASubstateC) && hook_is(gb, SYM(mooshStateASubstateC), s_mooshStateASubstateC_hook)) { s_mooshStateASubstateC_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// mooshStateASubstate5+17
static void moosh_d_mooshStateASubstate5_17(GB *gb) {
  BASE(mooshStateASubstate5);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+17, b_+19); E = 0x3d;
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CALL_C(b_+21, s_objectRemoveFromAButtonSensitiveObjectList, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+24);
  CYC(b_+24, b_+26); C = 0x01;
  CALL_C(b_+26, s_companionSetAnimation, SYM(companionSetAnimation), b_+29);
  CYC(b_+29, b_+32);
  TAIL(companionForceMount);
}

void s_mooshStateASubstate1_hook(GB *gb) {
  BASE(mooshStateASubstate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, s_mooshUpdateAsNpc, SYM(mooshUpdateAsNpc), b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wMooshState);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+11, b_+13); goto L_7a9c; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15);
  moosh_d_mooshStateASubstate5_17(gb); return;
L_7a9c:
  CYC(b_+15, b_+17); E = 0x3d;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); A = 0x81;
  CYC(b_+22, b_+25); mem_wr(gb, wDisabledObjects, A);
  RET(b_+25); return;
}

void s_mooshStateASubstate3_hook(GB *gb) {
  BASE(mooshStateASubstate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+6);
  CALL_C(b_+6, s_companionDecCounter1IfNonzero, SYM(companionDecCounter1IfNonzero), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); C = 0x10;
  CYC(b_+12, b_+15);
  TAIL(objectUpdateSpeedZ_paramC);
}

void s_mooshStateASubstate4_hook(GB *gb) {
  BASE(mooshStateASubstate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CYC(b_+3, b_+5); C = 0x10;
  CALL_C(b_+5, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = 0x3e;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_or(gb, 0x40);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18);
  TAIL(specialObjectAnimate);
}

void s_mooshStateASubstate5_hook(GB *gb) {
  BASE(mooshStateASubstate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, s_mooshUpdateAsNpc, SYM(mooshUpdateAsNpc), b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wMooshState);
  CYC(b_+9, b_+11); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0xff;
  CYC(b_+14, b_+17); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+17, b_+19); E = 0x3d;
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CALL_C(b_+21, s_objectRemoveFromAButtonSensitiveObjectList, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+24);
  CYC(b_+24, b_+26); C = 0x01;
  CALL_C(b_+26, s_companionSetAnimation, SYM(companionSetAnimation), b_+29);
  CYC(b_+29, b_+32);
  TAIL(companionForceMount);
}

