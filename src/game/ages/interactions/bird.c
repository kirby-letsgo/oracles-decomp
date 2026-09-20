#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t bank09_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

void interactionCode4c_hook(GB *gb);
void interactionCode4c__state0_hook(GB *gb);
void interactionCode4c__afterCall5ffe_hook(GB *gb);
void interactionCode4c__initSubid_hook(GB *gb);
void interactionCode4c__initSubid00_hook(GB *gb);
void interactionCode4c__initSubid04_hook(GB *gb);
void interactionCode4c__impaNotMoved_hook(GB *gb);
void interactionCode4c__initSubid01_hook(GB *gb);
void interactionCode4c__setAnimation0AndJump_hook(GB *gb);
void interactionCode4c__state1_hook(GB *gb);
void bird_runSubid0_hook(GB *gb);
void bird_runSubid0__substate0_hook(GB *gb);
void bird_runSubid0__substate1_hook(GB *gb);
void bird_runSubid0__substate2_hook(GB *gb);
void bird_runSubid0__substate3_hook(GB *gb);
void bird_runSubid4_hook(GB *gb);
void bird_updateGravityAndHopWhenHitGround_hook(GB *gb);
void bird_hop_hook(GB *gb);

void interactionCode4c_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+8) { interactionCode4c__state0_hook(gb); return; }
    else if (jt_ == b_+121) { interactionCode4c__state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void interactionCode4c__state0_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5ff8:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
L_5ffe:
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CALL_C(b_+17, interactionCode4c__initSubid_hook, b_+28, b_+20);
  CYC(b_+20, b_+22); E = 0x40;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); if (hook_enabled_at(gb, SYM(objectMarkSolidPosition))) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(SYM(objectMarkSolidPosition)); } CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb); return;
}

void interactionCode4c__afterCall5ffe_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5ffe:
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CALL_C(b_+17, interactionCode4c__initSubid_hook, b_+28, b_+20);
  CYC(b_+20, b_+22); E = 0x40;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); if (hook_enabled_at(gb, SYM(objectMarkSolidPosition))) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(SYM(objectMarkSolidPosition)); } CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb); return;
}

void interactionCode4c__initSubid_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_600c:
  CYC(b_+28, b_+30); E = 0x42;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+42) { goto L_601a; }
    else if (jt_ == b_+51) { goto L_6023; }
    else if (jt_ == b_+110) { goto L_605e; }
    else { HANDOFF(HL); }
  } while (0);
L_601a:
  CALL_C(b_+42, bird_hop_hook, SYM(bird_hop), b_+45);
  CYC(b_+45, b_+48); SET_HL((SYM(subrosian_subid00__state0) + 2));
  CYC(b_+48, b_+51); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_6023:
  CYC(b_+51, b_+54); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+54, b_+56); alu_bit(gb, 2, A);
  if ((F & FZ)) { CYCT(b_+56, b_+59); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+56, b_+59);
  CALL_C(b_+59, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+62);
  if ((F & FZ)) { CYCT(b_+62, b_+65); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+62, b_+65);
  CYC(b_+65, b_+67); A = 0x38;
  CALL_C(b_+67, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+73); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+70, b_+73);
  CYC(b_+73, b_+76); SET_HL((SYM(subrosian_subid00__state0) + 41));
  CALL_C(b_+76, interactionSetScript_hook, SYM(interactionSetScript), b_+79);
  CALL_C(b_+79, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+82);
  CYC(b_+82, b_+84); A = 0x39;
  CALL_C(b_+84, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+87);
  if ((F & FZ)) { CYCT(b_+87, b_+89); goto L_6062; } CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); A = 0x3c;
  CALL_C(b_+91, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+94);
  if ((F & FZ)) { CYCT(b_+94, b_+96); goto L_6057; } CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); E = 0x4b;
  CYC(b_+98, b_+100); A = 0x58;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CYC(b_+101, b_+103); goto L_6062;
L_6057:
  CYC(b_+103, b_+105); E = 0x4d;
  CYC(b_+105, b_+107); A = 0x68;
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+110); goto L_6062;
L_605e:
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+113); E = 0x5c;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
L_6062:
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CALL_C(b_+115, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+118);
  CYC(b_+118, b_+121); bird_hop_hook(gb); return;
}

void interactionCode4c__initSubid00_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_601a:
  CALL_C(b_+42, bird_hop_hook, SYM(bird_hop), b_+45);
  CYC(b_+45, b_+48); SET_HL((SYM(subrosian_subid00__state0) + 2));
  CYC(b_+48, b_+51); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void interactionCode4c__initSubid04_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6023:
  CYC(b_+51, b_+54); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+54, b_+56); alu_bit(gb, 2, A);
  if ((F & FZ)) { CYCT(b_+56, b_+59); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+56, b_+59);
  CALL_C(b_+59, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+62);
  if ((F & FZ)) { CYCT(b_+62, b_+65); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+62, b_+65);
  CYC(b_+65, b_+67); A = 0x38;
  CALL_C(b_+67, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+73); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+70, b_+73);
  CYC(b_+73, b_+76); SET_HL((SYM(subrosian_subid00__state0) + 41));
  CALL_C(b_+76, interactionSetScript_hook, SYM(interactionSetScript), b_+79);
  CALL_C(b_+79, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+82);
  CYC(b_+82, b_+84); A = 0x39;
  CALL_C(b_+84, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+87);
  if ((F & FZ)) { CYCT(b_+87, b_+89); goto L_6062; } CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); A = 0x3c;
  CALL_C(b_+91, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+94);
  if ((F & FZ)) { CYCT(b_+94, b_+96); goto L_6057; } CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); E = 0x4b;
  CYC(b_+98, b_+100); A = 0x58;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CYC(b_+101, b_+103); goto L_6062;
L_6057:
  CYC(b_+103, b_+105); E = 0x4d;
  CYC(b_+105, b_+107); A = 0x68;
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+110); goto L_6062;
L_6062:
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CALL_C(b_+115, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+118);
  CYC(b_+118, b_+121); bird_hop_hook(gb); return;
}

void interactionCode4c__impaNotMoved_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6057:
  CYC(b_+103, b_+105); E = 0x4d;
  CYC(b_+105, b_+107); A = 0x68;
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+110); goto L_6062;
L_6062:
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CALL_C(b_+115, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+118);
  CYC(b_+118, b_+121); bird_hop_hook(gb); return;
}

void interactionCode4c__initSubid01_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_605e:
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+113); E = 0x5c;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
L_6062:
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CALL_C(b_+115, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+118);
  CYC(b_+118, b_+121); bird_hop_hook(gb); return;
}

void interactionCode4c__setAnimation0AndJump_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6062:
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CALL_C(b_+115, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+118);
  CYC(b_+118, b_+121); bird_hop_hook(gb); return;
}

void interactionCode4c__state1_hook(GB *gb) {
  BASE(interactionCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6069:
  CYC(b_+121, b_+123); E = 0x42;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); push_effect(gb, b_+125); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(bird_runSubid0)) { bird_runSubid0_hook(gb); return; }
    else if (jt_ == SYM(bird_runSubid4)) { bird_runSubid4_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void bird_runSubid0_hook(GB *gb) {
  BASE(bird_runSubid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+15) { bird_runSubid0__substate0_hook(gb); return; }
    else if (jt_ == b_+40) { bird_runSubid0__substate1_hook(gb); return; }
    else if (jt_ == b_+61) { bird_runSubid0__substate2_hook(gb); return; }
    else if (jt_ == b_+92) { bird_runSubid0__substate3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void bird_runSubid0__substate0_hook(GB *gb) {
  BASE(bird_runSubid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6086:
  CYC(b_+15, b_+18); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+18, b_+20); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_6095; } CYC(b_+20, b_+22);
  CALL_C(b_+22, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+25);
  CYC(b_+25, b_+27); A = 0x01;
  CYC(b_+27, b_+30); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
L_6095:
  CYC(b_+30, b_+32); E = 0x77;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); alu_or(gb, A);
  if (!(F & FZ)) { CALL_C_CC(b_+34, bird_updateGravityAndHopWhenHitGround_hook, SYM(bird_updateGravityAndHopWhenHitGround), b_+37); } else CYC(b_+34, b_+37);
  CYC(b_+37, b_+40); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
}

void bird_runSubid0__substate1_hook(GB *gb) {
  BASE(bird_runSubid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_609f:
  CYC(b_+40, b_+43); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+43, b_+45); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+45, b_+46); ret_effect(gb); return; } CYC(b_+45, b_+46);
  CALL_C(b_+46, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+49);
  CYC(b_+49, b_+51); L = 0x46;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+53, bird_hop_hook, SYM(bird_hop), b_+56);
  CYC(b_+56, b_+58); A = 0x02;
  CYC(b_+58, b_+61); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
}

void bird_runSubid0__substate2_hook(GB *gb) {
  BASE(bird_runSubid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_60b4:
  CALL_C(b_+61, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); bird_updateGravityAndHopWhenHitGround_hook(gb); return; } CYC(b_+64, b_+66);
  CALL_C(b_+66, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+69);
  CYC(b_+69, b_+71); L = 0x4f;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x00);
  CYC(b_+73, b_+75); L = 0x49;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x01);
  CYC(b_+77, b_+79); L = 0x50;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x28);
  CYC(b_+81, b_+84); SET_BC(0xff00);
  CALL_C(b_+84, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+87);
  CYC(b_+87, b_+89); A = 0x03;
  CYC(b_+89, b_+92); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
}

void bird_runSubid0__substate3_hook(GB *gb) {
  BASE(bird_runSubid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_60d3:
  CALL_C(b_+92, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+95);
  if (!(F & FC)) { CYCT(b_+95, b_+98); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+95, b_+98);
  CYC(b_+98, b_+99); alu_xor(gb, A);
  CALL_C(b_+99, objectUpdateSpeedZ_hook, SYM(objectUpdateSpeedZ), b_+102);
  CYC(b_+102, b_+105); if (hook_enabled_at(gb, SYM(objectApplySpeed))) { objectApplySpeed_hook(gb); return; } HANDOFF(SYM(objectApplySpeed));
}

void bird_runSubid4_hook(GB *gb) {
  BASE(bird_runSubid4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+3);
  CALL_C(b_+3, bird_updateGravityAndHopWhenHitGround_hook, SYM(bird_updateGravityAndHopWhenHitGround), b_+6);
  CALL_C(b_+6, interactionRunScript_hook, SYM(interactionRunScript), b_+9);
  if ((F & FC)) { CYCT(b_+9, b_+12); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); A = 0x39;
  CALL_C(b_+14, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+17);
  if ((F & FZ)) { CYCT(b_+17, b_+18); ret_effect(gb); return; } CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); A = 0x3c;
  CALL_C(b_+20, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+24); ret_effect(gb); return; } CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); E = 0x4d;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); alu_cp(gb, 0x68);
  if ((F & FZ)) { CYCT(b_+29, b_+30); ret_effect(gb); return; } CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); A = alu_inc8(gb, A);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); ret_effect(gb); return;
}

void bird_updateGravityAndHopWhenHitGround_hook(GB *gb) {
  BASE(bird_updateGravityAndHopWhenHitGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) {
    CYC(b_+5, b_+6);
    CYC(b_+6, b_+7); H = D;
    bird_hop_hook(gb);
    return;
  }
  CYCT(b_+5, b_+6); ret_effect(gb);
}

void bird_hop_hook(GB *gb) {
  BASE(bird_hop);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xff40);
  CYC(b_+3, b_+6); if (hook_enabled_at(gb, SYM(objectSetSpeedZ))) { objectSetSpeedZ_hook(gb); return; } HANDOFF(SYM(objectSetSpeedZ));
}
