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

static uint16_t rickyB06_jump_table(GB *gb) {
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

// The bank-6 copy of rickyState1 (identical code; the bank-5 copy is the shared hook).
void s_rickyState1_b06_hook(GB *gb) {
  BASE(incState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+14, b_+16); E = 0x02;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (rickyB06_jump_table(gb));
    if (jt_ == b_+22) goto subid0;
    if (jt_ == b_+94) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+22, b_+24); E = 0x05;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (rickyB06_jump_table(gb));
    if (jt_ == b_+30) goto subid0_substate0;
    if (jt_ == b_+61) goto subid0_substate1;
    HANDOFF(HL);
  } while (0);
subid0_substate0:
  CALL_C(b_+30, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+33);
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = 0x21;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+38, b_+40); goto L_6a37; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); A = 0x01;
  CYC(b_+42, b_+45); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+45, b_+47); L = 0x05;
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_6a37:
  CYC(b_+48, b_+50); C = 0x20;
  CALL_C(b_+50, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+53);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+58); SET_BC(0xff20);
  CYC(b_+58, b_+61);
  TAIL(objectSetSpeedZ);
subid0_substate1:
  CALL_C(b_+61, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+64);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = 0x21;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x00);
  CYC(b_+72, b_+73); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+73, b_+75); goto L_6a5e; }
  CYC(b_+73, b_+75);
clink:
  CALL_C(b_+75, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+78);
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x07);
  CYC(b_+81, b_+84); SET_BC(0xf812);
  CYC(b_+84, b_+87);
  TAIL(objectCopyPositionWithOffset);
L_6a5e:
  CYC(b_+87, b_+89); L = 0x05;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x00);
  CYC(b_+91, b_+94);
  TAIL_S(func_69fe);
subid1:
  CYC(b_+94, b_+96); E = 0x05;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); push_effect(gb, b_+98);
  do { uint16_t jt_ = (rickyB06_jump_table(gb));
    if (jt_ == b_+120) goto subid1_substate0;
    if (jt_ == b_+123) goto subid1_substate1;
    if (jt_ == b_+173) goto subid1_substate2;
    if (jt_ == b_+182) goto subid1_substate3;
    if (jt_ == b_+195) goto subid1_substate4;
    if (jt_ == b_+214) goto subid1_substate5;
    if (jt_ == b_+227) goto subid1_substate6;
    if (jt_ == b_+267) goto subid1_substate7;
    if (jt_ == b_+285) goto subid1_substate8;
    if (jt_ == b_+306) goto subid1_substate9;
    if (jt_ == b_+339) goto subid1_substateA;
    HANDOFF(HL);
  } while (0);
subid1_substate0:
  CYC(b_+120, b_+122); L = 0x05;
  CYC(b_+122, b_+123); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
subid1_substate1:
  CALL_C(b_+123, s_objectApplySpeed, SYM(objectApplySpeed), b_+126);
  CYC(b_+126, b_+128); E = 0x0d;
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+131); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+131, b_+133); goto L_6aa4; }
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+136); SET_HL(w1Link_xh);
  CYC(b_+136, b_+137); B = mem_rd(gb, HL);
  CYC(b_+137, b_+139); alu_add(gb, 0x18);
  CYC(b_+139, b_+140); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+140, b_+142); goto L_6aa4; }
  CYC(b_+140, b_+142);
  CALL_C(b_+142, s_itemIncSubstate, SYM(itemIncSubstate), b_+145);
  CYC(b_+145, b_+146); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+146, b_+147); L = alu_inc8(gb, L);
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x3c);
  CYC(b_+149, b_+151); L = 0x0e;
  CYC(b_+151, b_+152); alu_xor(gb, A);
  CYC(b_+152, b_+153); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+153, b_+154); mem_wr(gb, HL, A);
  CYC(b_+154, b_+157);
  TAIL(specialObjectAnimate);
L_6aa4:
  CYC(b_+157, b_+159); C = 0x40;
  CALL_C(b_+159, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+162);
  if (!(F & FZ)) { RET_TAKEN(b_+162); return; }
  CYC(b_+162, b_+163);
  CALL_C(b_+163, s_itemIncSubstate, SYM(itemIncSubstate), b_+166);
  CYC(b_+166, b_+168); L = 0x06;
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x08);
  CYC(b_+170, b_+173);
  TAIL(specialObjectAnimate);
subid1_substate2:
  CALL_C(b_+173, s_itemDecCounter1, SYM(itemDecCounter1), b_+176);
  if (!(F & FZ)) { RET_TAKEN(b_+176); return; }
  CYC(b_+176, b_+177);
  CYC(b_+177, b_+178); L = alu_dec8(gb, L);
  CYC(b_+178, b_+179); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+179, b_+182);
  TAIL_S(func_69f3);
subid1_substate3:
  CALL_C(b_+182, s_itemDecCounter1, SYM(itemDecCounter1), b_+185);
  if (!(F & FZ)) { RET_TAKEN(b_+185); return; }
  CYC(b_+185, b_+186);
  CYC(b_+186, b_+188); mem_wr(gb, HL, 0x5a);
  CYC(b_+188, b_+189); L = alu_dec8(gb, L);
  CYC(b_+189, b_+190); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+190, b_+192); A = 0x14;
  CYC(b_+192, b_+195);
  TAIL(specialObjectSetAnimation);
subid1_substate4:
  CALL_C(b_+195, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+198);
  CALL_C(b_+198, s_itemDecCounter1, SYM(itemDecCounter1), b_+201);
  if (!(F & FZ)) { RET_TAKEN(b_+201); return; }
  CYC(b_+201, b_+202);
  CYC(b_+202, b_+204); mem_wr(gb, HL, 0x0c);
  CYC(b_+204, b_+205); L = alu_dec8(gb, L);
  CYC(b_+205, b_+206); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+206, b_+208); A = 0x1f;
  CALL_C(b_+208, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+211);
  CYC(b_+211, b_+214);
  goto clink;
subid1_substate5:
  CALL_C(b_+214, s_itemDecCounter1, SYM(itemDecCounter1), b_+217);
  if (!(F & FZ)) { RET_TAKEN(b_+217); return; }
  CYC(b_+217, b_+218);
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x3c);
  CYC(b_+220, b_+221); L = alu_dec8(gb, L);
  CYC(b_+221, b_+222); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+222, b_+224); A = 0x1e;
  CYC(b_+224, b_+227);
  TAIL(specialObjectSetAnimation);
subid1_substate6:
  CALL_C(b_+227, s_itemDecCounter1, SYM(itemDecCounter1), b_+230);
  if (!(F & FZ)) { RET_TAKEN(b_+230); return; }
  CYC(b_+230, b_+231);
  CYC(b_+231, b_+233); mem_wr(gb, HL, 0x1e);
  CYC(b_+233, b_+234); L = alu_dec8(gb, L);
  CYC(b_+234, b_+235); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+235, b_+238); SET_HL(wActiveRing);
  CYC(b_+238, b_+240); mem_wr(gb, HL, 0xff);
  CYC(b_+240, b_+242); A = 0x81;
  CYC(b_+242, b_+245); mem_wr(gb, wLinkInAir, A);
  CYC(b_+245, b_+248); SET_HL(w1Link_speed);
  CYC(b_+248, b_+250); mem_wr(gb, HL, 0x14);
  CYC(b_+250, b_+252); L = 0x14;
  CYC(b_+252, b_+254); mem_wr(gb, HL, 0x00);
  CYC(b_+254, b_+255); L = alu_inc8(gb, L);
  CYC(b_+255, b_+257); mem_wr(gb, HL, 0xfe);
  CYC(b_+257, b_+259); A = 0x18;
  CYC(b_+259, b_+262); mem_wr(gb, w1Link_angle, A);
  CYC(b_+262, b_+264); A = 0x53;
  CYC(b_+264, b_+267);
  TAIL(playSound_b00);
subid1_substate7:
  CALL_C(b_+267, s_itemDecCounter1, SYM(itemDecCounter1), b_+270);
  if (!(F & FZ)) { RET_TAKEN(b_+270); return; }
  CYC(b_+270, b_+271);
  CYC(b_+271, b_+273); mem_wr(gb, HL, 0x14);
  CYC(b_+273, b_+274); L = alu_dec8(gb, L);
  CYC(b_+274, b_+275); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+275, b_+276); alu_xor(gb, A);
  CYC(b_+276, b_+279); SET_HL(w1Link_visible);
  CYC(b_+279, b_+280); mem_wr(gb, HL, A);
  CYC(b_+280, b_+281); A = alu_inc8(gb, A);
  CYC(b_+281, b_+284); mem_wr(gb, wDisabledObjects, A);
  RET(b_+284); return;
subid1_substate8:
  CALL_C(b_+285, s_itemDecCounter1, SYM(itemDecCounter1), b_+288);
  if (!(F & FZ)) { RET_TAKEN(b_+288); return; }
  CYC(b_+288, b_+289);
  CYC(b_+289, b_+290); L = alu_dec8(gb, L);
  CYC(b_+290, b_+291); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+291, b_+293); L = 0x09;
  CYC(b_+293, b_+295); mem_wr(gb, HL, 0x18);
subid1_func_6b2e:
  CYC(b_+295, b_+297); A = 0x1c;
  CALL_C(b_+297, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+300);
  CYC(b_+300, b_+303); SET_BC(0xfe00);
  CYC(b_+303, b_+306);
  TAIL(objectSetSpeedZ);
subid1_substate9:
  CALL_C(b_+306, s_objectApplySpeed, SYM(objectApplySpeed), b_+309);
  CYC(b_+309, b_+311); E = 0x0d;
  CYC(b_+311, b_+312); A = mem_rd(gb, DE);
  CYC(b_+312, b_+314); alu_sub(gb, 0x10);
  CYC(b_+314, b_+315); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+315, b_+317); goto L_6b4a; }
  CYC(b_+315, b_+317);
  CYC(b_+317, b_+320); SET_HL(wTmpcfc0 + 0x1f);
  CYC(b_+320, b_+322); mem_wr(gb, HL, 0x01);
  RET(b_+322); return;
L_6b4a:
  CYC(b_+323, b_+325); C = 0x40;
  CALL_C(b_+325, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+328);
  if (!(F & FZ)) { RET_TAKEN(b_+328); return; }
  CYC(b_+328, b_+329);
  CALL_C(b_+329, s_itemIncSubstate, SYM(itemIncSubstate), b_+332);
  CYC(b_+332, b_+334); L = 0x06;
  CYC(b_+334, b_+336); mem_wr(gb, HL, 0x08);
  CYC(b_+336, b_+339);
  TAIL(specialObjectAnimate);
subid1_substateA:
  CALL_C(b_+339, s_itemDecCounter1, SYM(itemDecCounter1), b_+342);
  if (!(F & FZ)) { RET_TAKEN(b_+342); return; }
  CYC(b_+342, b_+343);
  CYC(b_+343, b_+345); L = 0x05;
  CYC(b_+345, b_+346); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+346, b_+349);
  goto subid1_func_6b2e;
}

