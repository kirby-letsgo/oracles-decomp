#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactiondc_subid00), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactiondc_subid00), (from), (to), true)

// object_code/ages/interactions/miscellaneous2.s (INTERAC_MISCELLANEOUS_2), bank $10.

static uint16_t interactiondc_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void interactiondc_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactiondc_subid00_hook(GB *gb) {
  BASE(interactiondc_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+11, b_+13); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); SET_BC((SYM(ecom_getTopDownAdjacentWallsBitsetGivenAngle_b10) + 1));
  CALL_C(b_+17, createTreasure_hook, SYM(createTreasure), b_+20);
  CALL_C(b_+20, objectCopyPosition_hook, SYM(objectCopyPosition), b_+23);
  CYC(b_+23, SYM(interactiondc_subid01));
  interactionDelete_hook(gb);
}

void interactiondc_subid01_hook(GB *gb) {
  BASE(interactiondc_subid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+14);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); SET_HL((SYM(interactionCodee6__checkLinkWithinRange) + 125));
  CALL_C(b_+17, interactionSetScript_hook, SYM(interactionSetScript), b_+20);
  CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23);
  CYC(b_+23, SYM(interactiondc_subid02));
  interactionIncState_hook(gb);
}

void interactiondc_subid02_hook(GB *gb) {
  BASE(interactiondc_subid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+40) { goto state1; }
    else if (jt_ == b_+79) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CALL_C(b_+10, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+13);
  CALL_C(b_+13, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+21);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+22); A = D;
  CYC(b_+22, b_+25); mem_wr(gb, wDiggingUpEnemiesForbidden, A);
  CALL_C(b_+25, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 0x3a);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); C = L;
  CYC(b_+32, b_+34); A = 0x1c;
  CALL_C(b_+34, setTile_hook, SYM(setTile), b_+37);
  CYC(b_+37, b_+40);
  interactionIncState_hook(gb);
  return;
state1:
  CYC(b_+40, b_+43); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+43, b_+45); alu_cp(gb, 0x83);
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); A = 0x01;
  CYC(b_+48, b_+51); mem_wr(gb, w1Link_direction, A);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+53, b_+55); A = 0x1e;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CALL_C(b_+56, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+59);
  if (!(F & FC)) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+65); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+65, b_+68); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+68, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+71);
  CYC(b_+71, b_+73); A = 0xf0;
  CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
  CYC(b_+76, b_+79);
  interactionIncState_hook(gb);
  return;
state2:
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); push_effect(gb, b_+83);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+87) { goto substate0; }
    else if (jt_ == b_+110) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+87, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+90);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x3c);
  CYC(b_+93, b_+95); A = 0x3c;
  CYC(b_+95, b_+98); SET_BC(0xf800);
  CALL_C(b_+98, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+101);
  CALL_C(b_+101, clearAllParentItems_hook, SYM(clearAllParentItems), b_+104);
  CALL_C(b_+104, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+107);
  CYC(b_+107, b_+110);
  interactionIncSubstate_hook(gb);
  return;
substate1:
  CYC(b_+110, b_+112); A = 0x28;
  CALL_C(b_+112, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+115);
  CALL_C(b_+115, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+118);
  if (!(F & FZ)) { RET_TAKEN(b_+118); return; }
  CYC(b_+118, b_+119);
  CYC(b_+119, b_+121); A = 0x1a;
  CYC(b_+121, b_+124); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+124, SYM(interactiondc_subid03));
  interactionDelete_hook(gb);
}

void interactiondc_subid3And4_state1_hook(GB *gb) {
  BASE(interactiondc_subid3And4_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0x3a);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0xd7;
  CYC(b_+8, b_+9); C = L;
  CALL_C(b_+9, setTile_hook, SYM(setTile), b_+12);
  CALL_C(b_+12, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+15);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); A = 0x4d;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, SYM(interactiondc_subid04));
  interactionDelete_hook(gb);
}

void interactiondc_subid03_hook(GB *gb) {
  BASE(interactiondc_subid03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    interactiondc_subid3And4_state1_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+13);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+15, b_+17); A = 0x02;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, SYM(interactiondc_subid3And4_state1));
  interactionIncState_hook(gb);
}

void interactiondc_subid04_hook(GB *gb) {
  BASE(interactiondc_subid04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    interactiondc_subid3And4_state1_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+13);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+15, b_+17); A = 0x04;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, SYM(interactiondc_subid05));
  interactionIncState_hook(gb);
}

void interactiondc_subid05_setRandomShakeDuration_hook(GB *gb) {
  BASE(interactiondc_subid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+75, getRandomNumber_hook, SYM(getRandomNumber), b_+78);
  CYC(b_+78, b_+80); alu_and(gb, 0x7f);
  CYC(b_+80, b_+82); alu_sub(gb, 0x40);
  CYC(b_+82, b_+84); alu_add(gb, 0x60);
  CYC(b_+84, b_+86); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  RET(b_+87); return;
}

void interactiondc_subid05_shakeScreen_hook(GB *gb) {
  BASE(interactiondc_subid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+88, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+91);
  CYC(b_+91, b_+94); A = mem_rd(gb, wFrameCounter);
  CYC(b_+94, b_+95); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CYC(b_+96, SYM(interactiondc_subid06));
  interactionDecCounter1_hook(gb);
}

void interactiondc_subid05_hook(GB *gb) {
  BASE(interactiondc_subid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+44) { goto state1; }
    else if (jt_ == b_+55) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x29;
  CALL_C(b_+12, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+15);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+18);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+21);
  CYC(b_+21, b_+23); A = 0xf1;
  CALL_C(b_+23, playSound_b00_hook, SYM(playSound_b00), b_+26);
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+31); mem_wr(gb, wScreenShakeMagnitude, A);
  CYC(b_+31, b_+34); push_effect(gb, b_+34);
  interactiondc_subid05_setRandomShakeDuration_hook(gb);
  CYC(b_+34, b_+37); A = mem_rd(gb, wFrameCounter);
  CYC(b_+37, b_+38); alu_rrca(gb);
  if (F & FC) {
    CALL_C_CC(b_+38, interactionIncState_hook, SYM(interactionIncState), b_+41);
  } else {
    CYC(b_+38, b_+41);
  }
  CYC(b_+41, b_+44);
  interactionIncState_hook(gb);
  return;
state1:
  CYC(b_+44, b_+45); alu_xor(gb, A);
  CYC(b_+45, b_+48); push_effect(gb, b_+48);
  interactiondc_subid05_shakeScreen_hook(gb);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49); push_effect(gb, b_+52);
  interactiondc_subid05_setRandomShakeDuration_hook(gb);
  CYC(b_+52, b_+55);
  interactionIncState_hook(gb);
  return;
state2:
  CYC(b_+55, b_+58); A = mem_rd(gb, wFrameCounter);
  CYC(b_+58, b_+60); alu_and(gb, 0x0f);
  CYC(b_+60, b_+62); A = 0xb3;
  if (F & FZ) {
    CALL_C_CC(b_+62, playSound_b00_hook, SYM(playSound_b00), b_+65);
  } else {
    CYC(b_+62, b_+65);
  }
  CYC(b_+65, b_+67); A = 0x08;
  CYC(b_+67, b_+70); push_effect(gb, b_+70);
  interactiondc_subid05_shakeScreen_hook(gb);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+73); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x01);
  interactiondc_subid05_setRandomShakeDuration_hook(gb);
}

void interactiondc_subid06_hook(GB *gb) {
  BASE(interactiondc_subid06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto delete_;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0xb201);
  CALL_C(b_+10, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+13);
delete_:
  CYC(b_+13, SYM(interactiondc_subid09));
  interactionDelete_hook(gb);
}

void interactiondc_subid07_hook(GB *gb) {
  BASE(interactiondc_subid07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); SET_BC((SYM(specialObjectAnimate) + 17));
  CALL_C(b_+11, createTreasure_hook, SYM(createTreasure), b_+14);
  CALL_C(b_+14, objectCopyPosition_hook, SYM(objectCopyPosition), b_+17);
  CYC(b_+17, SYM(interactiondc_subid08));
  interactionDelete_hook(gb);
}

void interactiondc_subid08_hook(GB *gb) {
  BASE(interactiondc_subid08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto state0;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+11); B = 0xcf;
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_cp(gb, L);
  if (F & FZ) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+29);
  interactionDelete_hook(gb);
  return;
state0:
  CALL_C(b_+29, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+32);
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+36, b_+39);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); C = A;
  CYC(b_+43, b_+45); B = 0xcf;
  CYC(b_+45, b_+46); A = mem_rd(gb, BC);
  CYC(b_+46, b_+48); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, SYM(interactiondc_subid00));
  interactionIncState_hook(gb);
}

void interactiondc_subid09_replaceTileList_hook(GB *gb) {
  BASE(interactiondc_subid09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
loop:
  CYC(b_+135, b_+136); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+136, b_+137); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+137); return; }
  CYC(b_+137, b_+138);
  CYC(b_+138, b_+139); C = A;
  CYC(b_+139, b_+140); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+140, b_+141); push_effect(gb, HL);
  CALL_C(b_+141, setTile_hook, SYM(setTile), b_+144);
  CYC(b_+144, b_+145); SET_HL(pop_effect(gb));
  CYC(b_+145, b_+148);
  goto loop;
}

void interactiondc_subid09_returnToState1_hook(GB *gb) {
  BASE(interactiondc_subid09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+77, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+83, b_+85); A = 0x01;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  RET(b_+86); return;
}

void interactiondc_subid09_state2_hook(GB *gb) {
  BASE(interactiondc_subid09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); push_effect(gb, b_+46);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+52) { goto substate0; }
    else if (jt_ == b_+66) { goto substate1; }
    else if (jt_ == b_+77) { interactiondc_subid09_returnToState1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+54, b_+56); A = 0x08;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+60); SET_HL(b_+147);
replaceTileListAndIncSubstate:
  CYC(b_+60, b_+63); push_effect(gb, b_+63);
  interactiondc_subid09_replaceTileList_hook(gb);
  CYC(b_+63, b_+66);
  interactionIncSubstate_hook(gb);
  return;
substate1:
  CALL_C(b_+66, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+69);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x08);
  CYC(b_+72, b_+75); SET_HL(b_+156);
  CYC(b_+75, b_+77);
  goto replaceTileListAndIncSubstate;
}

void interactiondc_subid09_state3_hook(GB *gb) {
  BASE(interactiondc_subid09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+89, b_+90); A = mem_rd(gb, DE);
  CYC(b_+90, b_+91); push_effect(gb, b_+91);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+77) { interactiondc_subid09_returnToState1_hook(gb); return; }
    else if (jt_ == b_+99) { goto substate0; }
    else if (jt_ == b_+113) { goto substate1; }
    else if (jt_ == b_+124) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+101, b_+103); A = 0x0c;
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+107); SET_HL(b_+165);
replaceTileListAndIncSubstate:
  CYC(b_+107, b_+110); push_effect(gb, b_+110);
  interactiondc_subid09_replaceTileList_hook(gb);
  CYC(b_+110, b_+113);
  interactionIncSubstate_hook(gb);
  return;
substate1:
  CALL_C(b_+113, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+116);
  if (!(F & FZ)) { RET_TAKEN(b_+116); return; }
  CYC(b_+116, b_+117);
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x0c);
  CYC(b_+119, b_+122); SET_HL(b_+182);
  CYC(b_+122, b_+124);
  goto replaceTileListAndIncSubstate;
substate2:
  CALL_C(b_+124, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+127);
  if (!(F & FZ)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x0c);
  CYC(b_+130, b_+133); SET_HL(b_+199);
  CYC(b_+133, b_+135);
  goto replaceTileListAndIncSubstate;
}

void interactiondc_subid09_hook(GB *gb) {
  BASE(interactiondc_subid09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == SYM(interactionIncState)) { interactionIncState_hook(gb); return; }
    else if (jt_ == b_+12) { goto state1; }
    else if (jt_ == b_+42) { interactiondc_subid09_state2_hook(gb); return; }
    else if (jt_ == b_+87) { interactiondc_subid09_state3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state1:
  CYC(b_+12, b_+15); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wFrameCounter);
  CYC(b_+20, b_+22); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+35); A = mem_rd(gb, hRng2);
  CYC(b_+35, b_+36); alu_rrca(gb);
  if (F & FC) {
    CALL_C_CC(b_+36, interactionIncState_hook, SYM(interactionIncState), b_+39);
  } else {
    CYC(b_+36, b_+39);
  }
  CYC(b_+39, b_+42);
  interactionIncState_hook(gb);
}

void interactiondc_subid0A_hook(GB *gb) {
  BASE(interactiondc_subid0A);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto state0;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x31;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) {
    CYCT(b_+10, b_+13);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x81;
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+21); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+25); mem_wr(gb, w1Link_direction, A);
  CYC(b_+25, b_+27); A = 0x1d;
  CYC(b_+27, b_+30); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+30, b_+33);
  interactionDelete_hook(gb);
  return;
state0:
  CALL_C(b_+33, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+36);
  CYC(b_+36, b_+38); alu_and(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+41);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+38, b_+41);
  CYC(b_+41, SYM(interactiondc_subid0B));
  interactionIncState_hook(gb);
}

void interactiondc_subid0B_hook(GB *gb) {
  BASE(interactiondc_subid0B);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+26) { goto state1; }
    else if (jt_ == b_+64) { goto state2; }
    else if (jt_ == b_+77) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); A = 0x18;
  CALL_C(b_+17, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+20);
  CYC(b_+20, b_+23); SET_HL(b_+101);
  CYC(b_+23, b_+26);
  interactionSetMiniScript_hook(gb);
  return;
state1:
  CALL_C(b_+26, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+29);
  if (!(F & FC)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CALL_C(b_+30, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+33);
  if (!(F & FC)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); A = 0x01;
  CYC(b_+36, b_+39); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+39, b_+41); A = 0x50;
  CALL_C(b_+41, playSound_b00_hook, SYM(playSound_b00), b_+44);
  CYC(b_+44, b_+47); SET_HL(w1Link);
  CALL_C(b_+47, objectTakePosition_hook, SYM(objectTakePosition), b_+50);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+52, b_+54); A = 0x1e;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+58); SET_BC(0xf808);
  CALL_C(b_+58, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+61);
  CYC(b_+61, b_+64);
  interactionIncState_hook(gb);
  return;
state2:
  CALL_C(b_+64, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+67);
  if (!(F & FZ)) { RET_TAKEN(b_+67); return; }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x1e);
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+74); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+74, b_+77);
  interactionIncState_hook(gb);
  return;
state3:
  CALL_C(b_+77, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x07);
  CALL_C(b_+83, interactionGetMiniScript_hook, SYM(interactionGetMiniScript), b_+86);
  CYC(b_+86, b_+87); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+87, b_+88); C = A;
  CALL_C(b_+88, interactionSetMiniScript_hook, SYM(interactionSetMiniScript), b_+91);
  CYC(b_+91, b_+92); A = C;
  CYC(b_+92, b_+93); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+93, b_+96);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+93, b_+96);
  CYC(b_+96, b_+98); A = 0x48;
  CYC(b_+98, b_+101);
  breakCrackedFloor_hook(gb);
}

void interactiondc_subid0C_hook(GB *gb) {
  BASE(interactiondc_subid0C);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto state0;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_sub(gb, 0x0c);
  CYC(b_+15, b_+18); SET_BC((SYM(loadTilesetHlpr) + 3));
  CYC(b_+18, b_+20); E = 0x56;
  if (F & FZ) {
    CYCT(b_+20, b_+22);
    goto spawnBridge;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); SET_BC((SYM(loadUncompressedGfxHeader) + 41));
  CYC(b_+25, b_+27); E = 0x28;
spawnBridge:
  CALL_C(b_+27, getFreePartSlot_hook, SYM(getFreePartSlot), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x0c);
  CYC(b_+33, b_+35); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+35, b_+36); mem_wr(gb, HL, B);
  CYC(b_+36, b_+38); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+38, b_+39); mem_wr(gb, HL, C);
  CYC(b_+39, b_+41); L = PART_BASE + OBJ_YH;
  CYC(b_+41, b_+42); mem_wr(gb, HL, E);
  CALL_C(b_+42, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+47, b_+49); A = 0x4d;
  CALL_C(b_+49, playSound_b00_hook, SYM(playSound_b00), b_+52);
  CYC(b_+52, b_+55);
  interactionDelete_hook(gb);
  return;
state0:
  CALL_C(b_+55, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+58);
  CYC(b_+58, b_+60); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+60, b_+63);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+60, b_+63);
  CYC(b_+63, SYM(interactiondc_subid0E));
  interactionIncState_hook(gb);
}

void interactiondc_subid0E_spawnPuff_hook(GB *gb) {
  BASE(interactiondc_subid0E);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+90, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+93);
  if (!(F & FZ)) { RET_TAKEN(b_+93); return; }
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x05);
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+98, b_+101);
  setShortPosition_paramC_hook(gb);
}

void interactiondc_subid0E_hook(GB *gb) {
  BASE(interactiondc_subid0E);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+21) { goto state1; }
    else if (jt_ == b_+101) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+18);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+21);
  interactionIncState_hook(gb);
  return;
state1:
  CALL_C(b_+21, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+24);
  CYC(b_+24, b_+26); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CALL_C(b_+27, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+30);
  if (!(F & FC)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); A = 0x81;
  CYC(b_+33, b_+36); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+36, b_+39); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+41, b_+43); A = 0x2d;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CALL_C(b_+44, interactionIncState_hook, SYM(interactionIncState), b_+47);
  CYC(b_+47, b_+49); C = 0x04;
  CYC(b_+49, b_+51); A = 0x30;
  CALL_C(b_+51, setTile_hook, SYM(setTile), b_+54);
  CYC(b_+54, b_+55); C = alu_inc8(gb, C);
  CYC(b_+55, b_+57); A = 0x32;
  CALL_C(b_+57, setTile_hook, SYM(setTile), b_+60);
  CYC(b_+60, b_+62); C = 0x14;
  CYC(b_+62, b_+64); A = 0x3a;
  CALL_C(b_+64, setTile_hook, SYM(setTile), b_+67);
  CYC(b_+67, b_+68); C = alu_inc8(gb, C);
  CYC(b_+68, b_+70); A = 0x3a;
  CALL_C(b_+70, setTile_hook, SYM(setTile), b_+73);
  CYC(b_+73, b_+75); C = 0x04;
  CYC(b_+75, b_+78); push_effect(gb, b_+78);
  interactiondc_subid0E_spawnPuff_hook(gb);
  CYC(b_+78, b_+80); C = 0x05;
  CYC(b_+80, b_+83); push_effect(gb, b_+83);
  interactiondc_subid0E_spawnPuff_hook(gb);
  CYC(b_+83, b_+85); C = 0x14;
  CYC(b_+85, b_+88); push_effect(gb, b_+88);
  interactiondc_subid0E_spawnPuff_hook(gb);
  CYC(b_+88, b_+90); C = 0x15;
  interactiondc_subid0E_spawnPuff_hook(gb);
  return;
state2:
  CALL_C(b_+101, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+104);
  if (!(F & FZ)) { RET_TAKEN(b_+104); return; }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+107); A = 0x4d;
  CALL_C(b_+107, playSound_b00_hook, SYM(playSound_b00), b_+110);
  CALL_C(b_+110, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+113);
  CYC(b_+113, b_+115); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+115, b_+116); alu_xor(gb, A);
  CYC(b_+116, b_+119); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+119, b_+122); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+122, SYM(interactiondc_subid0F));
  interactionDelete_hook(gb);
}

void interactiondc_subid0F_hook(GB *gb) {
  BASE(interactiondc_subid0F);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    goto state0;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_BC((SYM(setRoomFlagsForUnlockedKeyDoor) + 14));
  CYC(b_+12, b_+15); A = mem_rd(gb, wActiveRoom);
  CYC(b_+15, b_+17); alu_cp(gb, 0xd0);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19);
    goto showText;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); SET_BC((SYM(checkFlag) + 4));
showText:
  CALL_C(b_+22, showText_hook, SYM(showText), b_+25);
  CYC(b_+25, b_+28);
  interactionDelete_hook(gb);
  return;
state0:
  CYC(b_+28, b_+31); A = mem_rd(gb, wScreenVariables);
  CYC(b_+31, b_+33); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+33, b_+36);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, w1Link_yh);
  CYC(b_+39, b_+41); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(b_+41, b_+44);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+46); A = 0x08;
  CALL_C(b_+46, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+49);
  CYC(b_+49, SYM(interactiondc_subid10));
  interactionIncState_hook(gb);
}

void interactiondc_subid10_hook(GB *gb) {
  BASE(interactiondc_subid10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+31) { goto state1; }
    else if (jt_ == b_+38) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+10, b_+13); SET_HL((wRoomLayout + 68));
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+19); SET_BC((SYM(gfxRegisterStates) + 266));
  CALL_C(b_+19, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+22);
  CALL_C(b_+22, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+25);
  if (!(F & FC)) {
    CALL_C_CC(b_+25, interactionIncState_hook, SYM(interactionIncState), b_+28);
  } else {
    CYC(b_+25, b_+28);
  }
  CYC(b_+28, b_+31);
  interactionIncState_hook(gb);
  return;
state1:
  CALL_C(b_+31, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+34);
  if (F & FC) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+38);
  interactionIncState_hook(gb);
  return;
state2:
  CALL_C(b_+38, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+41);
  if (!(F & FC)) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CALL_C(b_+42, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+45);
  if (!(F & FC)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CALL_C(b_+46, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x01);
  CYC(b_+51, b_+54); SET_HL(b_+70);
  if (F & FZ) {
    CYCT(b_+54, b_+56);
    goto warpDest;
  }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); SET_HL(b_+75);
warpDest:
  CALL_C(b_+59, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+62);
  CYC(b_+62, b_+64); A = 0x6e;
  CALL_C(b_+64, playSound_b00_hook, SYM(playSound_b00), b_+67);
  CYC(b_+67, b_+70);
  interactionDelete_hook(gb);
}

void interactiondc_subid11_hook(GB *gb) {
  BASE(interactiondc_subid11);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(wDungeonBossKeys);
  CYC(b_+9, b_+11); A = 0x0c;
  CYC(b_+11, SYM(interactiondc_subid12));
  setFlag_hook(gb);
}

void interactiondc_subid12_hook(GB *gb) {
  BASE(interactiondc_subid12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CALL_C(b_+13, getFreePartSlot_hook, SYM(getFreePartSlot), b_+16);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x0c);
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0c);
  CYC(b_+23, b_+25); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x01);
  CYC(b_+27, b_+29); L = PART_BASE + OBJ_YH;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x13);
  CALL_C(b_+31, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+36, b_+38); A = 0x4d;
  CALL_C(b_+38, playSound_b00_hook, SYM(playSound_b00), b_+41);
  CYC(b_+41, SYM(interactiondc_subid13));
  interactionDelete_hook(gb);
}

void interactiondc_subid13_hook(GB *gb) {
  BASE(interactiondc_subid13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  CYC(b_+3, b_+5); A = 0xe4;
  CYC(b_+5, b_+8); SET_HL((wRoomLayout + 20));
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); L = 0x24;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = 0x34;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, SYM(interactiondc_subid14));
  interactionDelete_hook(gb);
}

void interactiondc_subid14_hook(GB *gb) {
  BASE(interactiondc_subid14);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+5); alu_cp(gb, 0xdc);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto delete_;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); B = 0x80;
  CALL_C(b_+9, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+12);
delete_:
  CYC(b_+12, SYM(interactiondc_subid15));
  interactionDelete_hook(gb);
}

void interactiondc_subid15And16_setChestContents_hook(GB *gb) {
  BASE(interactiondc_subid15And16_setChestContents);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+15);
  CYC(b_+3, b_+4);
  interactiondc_addDoubleIndexToHl_from_rst(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+8); mem_wr(gb, wChestContentsOverride, A);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+12); mem_wr(gb, (wChestContentsOverride + 1), A);
  CYC(b_+12, b_+15);
  interactionDelete_hook(gb);
}

void interactiondc_subid15And16_state0_hook(GB *gb) {
  BASE(interactiondc_subid15And16_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8);
    interactionDelete_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, SYM(interactiondc_subid16));
  interactionIncState_hook(gb);
}

void interactiondc_subid15_hook(GB *gb) {
  BASE(interactiondc_subid15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    interactiondc_subid15And16_state0_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+8);
  CYC(b_+8, b_+10); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
    interactiondc_subid15And16_setChestContents_hook(gb);
    return;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, SYM(interactiondc_subid15And16_setChestContents)); A = alu_dec8(gb, A);
  interactiondc_subid15And16_setChestContents_hook(gb);
}

void interactiondc_subid16_hook(GB *gb) {
  BASE(interactiondc_subid16);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
    interactiondc_subid15And16_state0_hook(gb);
    return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+8);
  CYC(b_+8, b_+10); A = 0x00;
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
    interactiondc_subid15And16_setChestContents_hook(gb);
    return;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  CYC(b_+13, SYM(interactiondc_subid17));
  interactiondc_subid15And16_setChestContents_hook(gb);
}

void interactiondc_subid17_hook(GB *gb) {
  BASE(interactiondc_subid17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+6);
    interactionIncState_hook(gb);
    return;
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, w1Link_yh);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+13); A = mem_rd(gb, w1Link_xh);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+17); SET_HL((SYM(veranFinal_beeForm_stateB__chooseQuadrant) + 5));
  CYC(b_+17, b_+19); E = 0x05;
  CALL_C(b_+19, interBankCall_hook, 0x008a, b_+22);
  CYC(b_+22, b_+24); B = alu_rl(gb, B);
  if (!(F & FC)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, w1Link_state);
  CYC(b_+28, b_+30); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); SET_HL(wLinkForceState);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x11;
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+43); A = mem_rd(gb, wBlockPushAngle);
  CYC(b_+43, b_+45); alu_and(gb, 0x08);
  CYC(b_+45, b_+47); alu_xor(gb, 0x08);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  RET(b_+48); return;
}

void interactionCodedc_hook(GB *gb) {
  BASE(interactionCodedc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiondc_jump_table(gb));
    if (jt_ == SYM(interactiondc_subid00)) { interactiondc_subid00_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid01)) { interactiondc_subid01_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid02)) { interactiondc_subid02_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid03)) { interactiondc_subid03_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid04)) { interactiondc_subid04_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid05)) { interactiondc_subid05_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid06)) { interactiondc_subid06_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid07)) { interactiondc_subid07_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid08)) { interactiondc_subid08_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid09)) { interactiondc_subid09_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid0A)) { interactiondc_subid0A_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid0B)) { interactiondc_subid0B_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid0C)) { interactiondc_subid0C_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid0E)) { interactiondc_subid0E_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid0F)) { interactiondc_subid0F_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid10)) { interactiondc_subid10_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid11)) { interactiondc_subid11_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid12)) { interactiondc_subid12_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid13)) { interactiondc_subid13_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid14)) { interactiondc_subid14_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid15)) { interactiondc_subid15_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid16)) { interactiondc_subid16_hook(gb); return; }
    else if (jt_ == SYM(interactiondc_subid17)) { interactiondc_subid17_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
