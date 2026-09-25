#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/specialObjects/ricky.s.

void s_rickyStateASubstate9_hook(GB *gb) {
  BASE(rickyStateASubstate9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x80;
  CYC(b_+2, b_+5); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+9); E = 0x08;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, s_rickyIncVar03, SYM(rickyIncVar03), b_+13);
  CYC(b_+13, b_+15); C = 0x20;
  CALL_C(b_+15, s_companionSetAnimation, SYM(companionSetAnimation), b_+18);
  CYC(b_+18, b_+21); SET_BC(0x4070);
  CALL_C(b_+21, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x1c);
  CYC(b_+26, b_+28); E = 0x09;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return;
}

// rickyStateASubstate9+18
static void ricky_rickyStateASubstate9_18(GB *gb) {
  BASE(rickyStateASubstate9);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+18, b_+21); SET_BC(0x4070);
  CALL_C(b_+21, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+24);
  CYC(b_+24, b_+26); alu_and(gb, 0x1c);
  CYC(b_+26, b_+28); E = 0x09;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return;
}

void s_rickyStateASubstateA_hook(GB *gb) {
  BASE(rickyStateASubstateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+3);
  CALL_C(b_+3, s_companionUpdateMovement, SYM(companionUpdateMovement), b_+6);
  CYC(b_+6, b_+8); E = 0x0d;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0x38);
  if (F & FC) { CYCT(b_+11, b_+13); ricky_rickyStateASubstate9_18(gb); return; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_BC(0x2004);
  CALL_C(b_+16, s_showText, SYM(showText), b_+19);
  s_rickyIncVar03(gb); return; // falls through
}


// Ricky leaving upon meeting Tingle (part 1: print text)
void s_rickyStateASubstate2_hook(GB *gb) {
  BASE(rickyStateASubstate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+5, b_+7); A = 0x00;
  CYC(b_+7, b_+9); E = 0x08;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); A = 0x05;
  CYC(b_+12, b_+14); E = 0x3f;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, s_rickyIncVar03, SYM(rickyIncVar03), b_+18);
  s_rickySetJumpSpeedForCutsceneAndSetAngle(gb); return; // falls through
}

// Ricky leaving upon meeting Tingle (part 5: punching the air)
void s_rickyStateASubstate6_hook(GB *gb) {
  BASE(rickyStateASubstate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, s_rickyWaitUntilJumpDone, SYM(rickyWaitUntilJumpDone), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+10);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto L_70fd; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = 0x0b;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x60);
  if (F & FC) { CYCT(b_+17, b_+19); goto L_70f4; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = 0x3e;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_or(gb, 0x04);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
L_70f4:
  CALL_C(b_+25, s_rickySetJumpSpeedForCutsceneAndSetAngle, SYM(rickySetJumpSpeedForCutsceneAndSetAngle), b_+28);
  CYC(b_+28, b_+30); E = 0x09;
  CYC(b_+30, b_+32); A = 0x10;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  RET(b_+33); return;
L_70fd:
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+39); mem_wr(gb, wLinkForceState, A);
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+43); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+43, s_itemDelete, SYM(itemDelete), b_+46);
  CYC(b_+46, b_+49);
  TAIL(saveLinkLocalRespawnAndCompanionPosition);
}

// Ricky leaving upon meeting Tingle (part 2: start moving toward cliff)
void s_rickyStateASubstate3_hook(GB *gb) {
  BASE(rickyStateASubstate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CYC(b_+3, b_+5); E = 0x3e;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, s_rickyWaitUntilJumpDone, SYM(rickyWaitUntilJumpDone), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); E = 0x0b;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_cp(gb, 0x38);
  if (!(F & FC)) { CYCT(b_+18, b_+20); TAIL(rickySetJumpSpeedForCutsceneAndSetAngle); }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); E = 0x3e;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_or(gb, 0x01);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return;
}

// Ricky leaving upon meeting Tingle (part 4: jumping down cliff)
void s_rickyStateASubstate5_hook(GB *gb) {
  BASE(rickyStateASubstate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_rickySetJumpSpeedForCutsceneAndSetAngle, SYM(rickySetJumpSpeedForCutsceneAndSetAngle), b_+3);
  CYC(b_+3, b_+5); E = 0x09;
  CYC(b_+5, b_+7); A = 0x10;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

// Ricky leaving upon meeting Tingle (part 3: moving toward cliff, or...
// part 6: moving toward screen edge)
void s_rickyStateASubstate4_hook(GB *gb) {
  BASE(rickyStateASubstate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CYC(b_+3, b_+5); E = 0x3e;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_bit(gb, 1, A);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); alu_or(gb, 0x02);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15);
  TAIL(companionDismount);
}

void s_rickyStateASubstate7_hook(GB *gb) {
  BASE(rickyStateASubstate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_companionSetAnimationToVar3f, SYM(companionSetAnimationToVar3f), b_+3);
  CALL_C(b_+3, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+6);
  CYC(b_+6, b_+8); E = 0x21;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  CYC(b_+10, b_+12); A = 0xc3;
  if (F & FZ) { CYCT(b_+12, b_+15); TAIL(playSound_b00); }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, s_rickySetJumpSpeedForCutsceneAndSetAngle, SYM(rickySetJumpSpeedForCutsceneAndSetAngle), b_+21);
  CYC(b_+21, b_+23); E = 0x09;
  CYC(b_+23, b_+25); A = 0x10;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return;
}
