#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionCode40_hook(GB *gb);
void soldierSubid00_hook(GB *gb);
void soldierSubid0c_hook(GB *gb);
void soldierSubid02_hook(GB *gb);
void soldierSubid03_hook(GB *gb);
void soldierSubid04_hook(GB *gb);
void soldierSubid04Substate0_hook(GB *gb);
void soldierUpdateAnimationAndRunScript_hook(GB *gb);
void soldierSubid04Substate1_hook(GB *gb);
void soldierSubid04Substate2_hook(GB *gb);
void soldierSubid04Substate3_hook(GB *gb);
void soldierSubid04Substate4_hook(GB *gb);
void soldierSubid05_hook(GB *gb);
void soldierSubid06_hook(GB *gb);
void soldierSubid07_hook(GB *gb);
void soldierSubid08_hook(GB *gb);
void soldierSubid0a_hook(GB *gb);
void soldierSubid0b_hook(GB *gb);
void soldierSubid0d_hook(GB *gb);
void soldierInitGraphics_hook(GB *gb);
void soldierInitGraphicsAndLoadScript_hook(GB *gb);
void soldierCheckBeatD6_hook(GB *gb);
void label_09_090_hook(GB *gb);
void label_09_092_hook(GB *gb);
void label_09_093_hook(GB *gb);
void label_09_094_hook(GB *gb);
void label_09_095_hook(GB *gb);

static uint16_t soldier_jump_table(GB *gb) {
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

static void soldier_addDoubleIndex(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode40_hook(GB *gb) {
  BASE(interactionCode40);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (soldier_jump_table(gb));
    if (jt_ == SYM(soldierSubid00) && hook_enabled_at(gb, SYM(soldierSubid00))) { soldierSubid00_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid02) && hook_enabled_at(gb, SYM(soldierSubid02))) { soldierSubid02_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid03) && hook_enabled_at(gb, SYM(soldierSubid03))) { soldierSubid03_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid04) && hook_enabled_at(gb, SYM(soldierSubid04))) { soldierSubid04_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid05) && hook_enabled_at(gb, SYM(soldierSubid05))) { soldierSubid05_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid06) && hook_enabled_at(gb, SYM(soldierSubid06))) { soldierSubid06_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid07) && hook_enabled_at(gb, SYM(soldierSubid07))) { soldierSubid07_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid08) && hook_enabled_at(gb, SYM(soldierSubid08))) { soldierSubid08_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid0a) && hook_enabled_at(gb, SYM(soldierSubid0a))) { soldierSubid0a_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid0b) && hook_enabled_at(gb, SYM(soldierSubid0b))) { soldierSubid0b_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid0c) && hook_enabled_at(gb, SYM(soldierSubid0c))) { soldierSubid0c_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid0d) && hook_enabled_at(gb, SYM(soldierSubid0d))) { soldierSubid0d_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void soldierSubid00_hook(GB *gb) {
  BASE(soldierSubid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x14;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); interactionDelete_hook(gb); return; }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); A = 0x0b;
  CALL_C(b_+10, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+13);
  CYC(b_+13, b_+15); E = 0x43;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); label_09_090_hook(gb); return; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); interactionDelete_hook(gb); return; }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); soldierSubid0c_hook(gb);
}

void soldierSubid0c_hook(GB *gb) {
  BASE(soldierSubid0c);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); label_09_092_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), SYM(label_09_092));
  TAIL(label_09_092);
}

void soldierSubid02_hook(GB *gb) {
  BASE(soldierSubid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); label_09_093_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; }
  CYC(b_+8, b_+11);
  CALL_C(b_+11, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+14);
  CALL_C(b_+14, objectSetVisible82_hook, SYM(objectSetVisible82), SYM(label_09_093));
  TAIL(label_09_093);
}

void soldierSubid03_hook(GB *gb) {
  BASE(soldierSubid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); label_09_095_hook(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; }
  CYC(b_+8, b_+11);
  CALL_C(b_+11, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+14);
  CYC(b_+14, b_+17); objectSetVisible82_hook(gb);
}

void soldierSubid04_hook(GB *gb) {
  BASE(soldierSubid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); A = 0x0b;
  CALL_C(b_+13, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; }
  CYC(b_+16, b_+19);
  CALL_C(b_+19, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+22);
  CYC(b_+22, b_+24); E = 0x5c;
  CYC(b_+24, b_+26); A = 3;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30); objectSetVisiblec2_hook(gb);
  return;

state1:
  CYC(b_+30, b_+32); E = 0x45;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  do { uint16_t jt_ = (soldier_jump_table(gb));
    if (jt_ == SYM(soldierSubid04Substate0) && hook_enabled_at(gb, SYM(soldierSubid04Substate0))) { soldierSubid04Substate0_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid04Substate1) && hook_enabled_at(gb, SYM(soldierSubid04Substate1))) { soldierSubid04Substate1_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid04Substate2) && hook_enabled_at(gb, SYM(soldierSubid04Substate2))) { soldierSubid04Substate2_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid04Substate3) && hook_enabled_at(gb, SYM(soldierSubid04Substate3))) { soldierSubid04Substate3_hook(gb); return; }
    else if (jt_ == SYM(soldierSubid04Substate4) && hook_enabled_at(gb, SYM(soldierSubid04Substate4))) { soldierSubid04Substate4_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void soldierSubid04Substate0_hook(GB *gb) {
  BASE(soldierSubid04Substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+3, b_+5); alu_cp(gb, 6);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); soldierUpdateAnimationAndRunScript_hook(gb); return; } CYC(b_+5, b_+7);
  CALL_C(b_+7, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+10);
  CYC(b_+10, b_+12); L = 0x46;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x1e);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); interactionSetAnimation_hook(gb);
}

void soldierUpdateAnimationAndRunScript_hook(GB *gb) {
  BASE(soldierUpdateAnimationAndRunScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+6); interactionRunScript_hook(gb);
}

void soldierSubid04Substate1_hook(GB *gb) {
  BASE(soldierSubid04Substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CALL_C(b_+3, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; } CYC(b_+6, b_+7);
  CALL_C(b_+7, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+10);
  CYC(b_+10, b_+13); SET_BC(0xfe40);
  CALL_C(b_+13, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+16);
  CYC(b_+16, b_+18); A = 0x53;
  CYC(b_+18, b_+21); playSound_b00_hook(gb);
}

void soldierSubid04Substate2_hook(GB *gb) {
  BASE(soldierSubid04Substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CALL_C(b_+6, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+9);
  CYC(b_+9, b_+11); L = 0x46;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 8);
  CYC(b_+13, b_+15); A = 2;
  CYC(b_+15, b_+18); interactionSetAnimation_hook(gb);
}

void soldierSubid04Substate3_hook(GB *gb) {
  BASE(soldierSubid04Substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = 0x49;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x10);
  CYC(b_+8, b_+10); L = 0x50;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x50);
  CYC(b_+12, b_+15); interactionIncSubstate_hook(gb);
}

void soldierSubid04Substate4_hook(GB *gb) {
  BASE(soldierSubid04Substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+9); interactionDelete_hook(gb); return; } CYC(b_+6, b_+9);
  CYC(b_+9, b_+12); interactionAnimateBasedOnSpeed_hook(gb);
}

void soldierSubid05_hook(GB *gb) {
  BASE(soldierSubid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; } CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); A = 0x0b;
  CALL_C(b_+13, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; } CYC(b_+16, b_+19);
  CALL_C(b_+19, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+22);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CALL_C(b_+23, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+26);
  CYC(b_+26, b_+29); SET_HL(w1Link_xh);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x50);
  CYC(b_+31, b_+34); TAIL(objectSetVisible82);
state1:
  CALL_C(b_+34, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+37);
  if (!(F & FC)) { CYCT(b_+37, b_+40); interactionDelete_hook(gb); return; } CYC(b_+37, b_+40);
  CALL_C(b_+40, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+43);
  CYC(b_+43, b_+45); alu_cp(gb, 0xd0);
  CYC(b_+45, b_+47); A = 0x28;
  if (!(F & FZ)) { CYCT(b_+47, b_+49); goto L_50b0; } CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); A = 0x19;
L_50b0:
  CYC(b_+51, b_+53); E = 0x50;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CALL_C(b_+54, interactionRunScript_hook, SYM(interactionRunScript), b_+57);
  CYC(b_+57, b_+60); interactionAnimate2Times_hook(gb);
}

void soldierSubid06_hook(GB *gb) {
  BASE(soldierSubid06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; } CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); A = 0x0b;
  CALL_C(b_+13, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; } CYC(b_+16, b_+19);
  CALL_C(b_+19, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+22);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CALL_C(b_+23, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+26);
  CYC(b_+26, b_+29); TAIL(objectSetVisible82);
state1:
  CALL_C(b_+29, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto update; } CYC(b_+32, b_+34);
  CYC(b_+34, b_+37); A = mem_rd(gb, w1Link_yh);
  CYC(b_+37, b_+39); alu_cp(gb, 0x68);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto update; } CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CYC(b_+42, b_+45); mem_wr(gb, wMenuUnionEnd, A);
  CYC(b_+45, b_+46); A = alu_inc8(gb, A);
  CYC(b_+46, b_+49); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+49, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+52);
update:
  CYC(b_+52, b_+55); soldierUpdateAnimationAndRunScript_hook(gb);
}

void soldierSubid07_hook(GB *gb) {
  BASE(soldierSubid07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; } CYC(b_+8, b_+11);
  CALL_C(b_+11, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+14);
  CYC(b_+14, b_+17); TAIL(objectSetVisible82);
state1:
  CALL_C(b_+17, interactionRunScript_hook, SYM(interactionRunScript), b_+20);
  CYC(b_+20, b_+23); interactionAnimateAsNpc_hook(gb);
}

void soldierSubid08_hook(GB *gb) {
  BASE(soldierSubid08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierInitGraphics_hook, SYM(soldierInitGraphics), b_+8);
  CYC(b_+8, b_+10); L = 0x5c;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 3);
  CYC(b_+12, b_+15); TAIL(objectSetVisible82);
state1:
  CYC(b_+15, b_+18); SET_HL((SYM(checkNpcShouldExistAtGameStage_body__data5__subid2) + 3));
  CYC(b_+18, b_+20); E = 0x15;
  CALL_C(b_+20, interBankCall_hook, 0x008a, b_+23);
  CYC(b_+23, b_+26); interactionAnimate_hook(gb);
}

void soldierSubid0a_hook(GB *gb) {
  BASE(soldierSubid0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);
  CALL_C(b_+5, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+8);
  CYC(b_+8, b_+10); L = 0x5c;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 2);
  CYC(b_+12, b_+15); SET_BC(SYM(interactionCode55));
  CYC(b_+15, b_+18); TAIL(interactionSetPosition);
state1:
  CALL_C(b_+18, soldierUpdateAnimationAndRunScript_hook, SYM(soldierUpdateAnimationAndRunScript), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+22); ret_effect(gb); return; } CYC(b_+21, b_+22);
  CYC(b_+22, b_+25); SET_HL(wcc05);
  CYC(b_+25, b_+27); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 2));
  CYC(b_+27, b_+30); SET_HL(b_+33);
  CYC(b_+30, b_+33); setWarpDestVariables_hook(gb);
}

void soldierSubid0b_hook(GB *gb) {
  BASE(soldierSubid0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); interactionAnimate_hook(gb); return; } CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); A = 0x0b;
  CALL_C(b_+8, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; } CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); A = 0x24;
  CALL_C(b_+16, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+19);
  if (!(F & FC)) { CYCT(b_+19, b_+22); interactionDelete_hook(gb); return; } CYC(b_+19, b_+22);
  CALL_C(b_+22, soldierInitGraphics_hook, SYM(soldierInitGraphics), b_+25);
  CYC(b_+25, b_+27); L = 0x5c;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 2);
  CYC(b_+29, b_+31); A = 1;
  CALL_C(b_+31, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+34);
  CYC(b_+34, b_+37); objectSetVisible82_hook(gb);
}

void soldierSubid0d_hook(GB *gb) {
  BASE(soldierSubid0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x14;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; } CYC(b_+10, b_+13);
  CALL_C(b_+13, soldierInitGraphicsAndLoadScript_hook, SYM(soldierInitGraphicsAndLoadScript), b_+16);
  CYC(b_+16, b_+18); E = 0x43;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); L = 0x5c;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 1);
  CYC(b_+23, b_+25); alu_cp(gb, 7);
  if (F & FC) { CYCT(b_+25, b_+27); goto L_5188; } CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_5188:
  CYC(b_+28, b_+31); SET_BC(b_+43);
  CALL_C(b_+31, addAToBc_hook, 0x006d, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, BC);
  CYC(b_+35, b_+37); L = 0x7b;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  CALL_C(b_+38, interactionRunScript_hook, SYM(interactionRunScript), b_+41);
  CYC(b_+41, b_+43); goto state1;
state1:
  CALL_C(b_+59, interactionRunScript_hook, SYM(interactionRunScript), b_+62);
  if (F & FC) { CYCT(b_+62, b_+65); interactionDelete_hook(gb); return; } CYC(b_+62, b_+65);
  CYC(b_+65, b_+67); E = 0x7b;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+69, b_+71); goto L_51bc; } CYC(b_+69, b_+71);
  CALL_C(b_+71, interactionRunScript_hook, SYM(interactionRunScript), b_+74);
  if (F & FC) { CYCT(b_+74, b_+77); interactionDelete_hook(gb); return; } CYC(b_+74, b_+77);
  CYC(b_+77, b_+80); TAIL(npcFaceLinkAndAnimate);
L_51bc:
  CYC(b_+80, b_+82); E = 0x7f;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+84, b_+87); npcFaceLinkAndAnimate_hook(gb); return; } CYC(b_+84, b_+87);
  CALL_C(b_+87, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+90);
  CYC(b_+90, b_+93); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

void soldierInitGraphics_hook(GB *gb) {
  BASE(soldierInitGraphics);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+9); interactionIncState_hook(gb);
}

void soldierInitGraphicsAndLoadScript_hook(GB *gb) {
  BASE(soldierInitGraphicsAndLoadScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+8); E = 0x42;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(SYM(soldierScriptTable));
  CYC(b_+12, b_+13); soldier_addDoubleIndex(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CALL_C(b_+16, interactionSetScript_hook, SYM(interactionSetScript), b_+19);
  CYC(b_+19, b_+22); interactionIncState_hook(gb);
}

void soldierCheckBeatD6_hook(GB *gb) {
  BASE(soldierCheckBeatD6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x40;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto L_5205; } CYC(b_+5, b_+7);
  CALL_C(b_+7, getHighestSetBit_hook, SYM(getHighestSetBit), b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 5);
  CYC(b_+12, b_+13); ret_effect(gb);
  return;
L_5205:
  CYC(b_+13, b_+14); alu_scf(gb);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void label_09_090_hook(GB *gb) {
  BASE(label_09_090);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+1, b_+4); interactionDelete_hook(gb); return; }
  CYC(b_+1, b_+4);
  TAIL(soldierSubid0c);
}

void label_09_092_hook(GB *gb) {
  BASE(label_09_092);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); npcFaceLinkAndAnimate_hook(gb);
}

void label_09_093_hook(GB *gb) {
  BASE(label_09_093);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, soldierUpdateAnimationAndRunScript_hook, SYM(soldierUpdateAnimationAndRunScript), b_+9);
  CALL_C(b_+9, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+12);
  CYC(b_+12, b_+14); A = 0x10;
  CALL_C(b_+14, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+19); label_09_094_hook(gb); return; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 0x0b;
  CALL_C(b_+21, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+24);
  if (F & FZ) { CYCT(b_+24, b_+25); ret_effect(gb); return; }
  CYC(b_+24, b_+25);
  TAIL(label_09_094);
}

void label_09_094_hook(GB *gb) {
  BASE(label_09_094);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); objectPreventLinkFromPassing_hook(gb);
}

void label_09_095_hook(GB *gb) {
  BASE(label_09_095);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6); interactionAnimate_hook(gb);
}
