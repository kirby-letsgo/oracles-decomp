#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodee2), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodee2), (from), (to), true)

// object_code/common/interactions/statueEyeball.s (INTERAC_STATUE_EYEBALL), bank $10.

static uint16_t interactionCodee2_jump_table(GB *gb) {
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

static void interactionCodee2_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void interactionCodee2__getDirectionToFace_hook(GB *gb) {
  BASE(interactionCodee2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+93, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+96);
  CYC(b_+96, b_+97); B = A;
  CYC(b_+97, b_+99); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+99, b_+101);
    goto returnValue;
  }
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+103); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+103, b_+105);
    goto returnValue;
  }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+107); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+107, b_+109);
    goto returnValue;
  }
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+110); A = B;
  CYC(b_+110, b_+112); alu_and(gb, 0xfc);
  CYC(b_+112, b_+114); alu_or(gb, 0x04);
  CYC(b_+114, b_+115); B = A;
returnValue:
  CYC(b_+115, b_+116); A = B;
  CYC(b_+116, b_+117); alu_rrca(gb);
  CYC(b_+117, b_+118); alu_rrca(gb);
  CYC(b_+118, b_+120); alu_and(gb, 0x07);
  RET(b_+120); return;
}

void interactionCodee2__centerOnTileAndGetDirectionToFace_hook(GB *gb) {
  BASE(interactionCodee2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+90, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+93);
  interactionCodee2__getDirectionToFace_hook(gb);
}

void interactionCodee2__spawnChild_hook(GB *gb) {
  BASE(interactionCodee2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+138, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+141);
  if (!(F & FZ)) { RET_TAKEN(b_+141); return; }
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0xe2);
  CYC(b_+144, b_+145); L = alu_inc8(gb, L);
  CYC(b_+145, b_+146); mem_wr(gb, HL, E);
  CYC(b_+146, b_+147); push_effect(gb, BC);
  CALL_C(b_+147, convertShortToLongPosition_paramC_hook, SYM(convertShortToLongPosition_paramC), b_+150);
  CYC(b_+150, b_+152); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+152, b_+153); B = alu_dec8(gb, B);
  CYC(b_+153, b_+154); B = alu_dec8(gb, B);
  CYC(b_+154, b_+155); mem_wr(gb, HL, B);
  CYC(b_+155, b_+156); L = alu_inc8(gb, L);
  CYC(b_+156, b_+157); L = alu_inc8(gb, L);
  CYC(b_+157, b_+158); mem_wr(gb, HL, C);
  CYC(b_+158, b_+159); SET_BC(pop_effect(gb));
  RET(b_+159); return;
}

void interactionCodee2_hook(GB *gb) {
  BASE(interactionCodee2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodee2_jump_table(gb));
    if (jt_ == b_+14) { goto subid0; }
    else if (jt_ == b_+40) { goto subid2; }
    else if (jt_ == b_+121) { goto subid1; }
    else if (jt_ == b_+160) { goto subid3; }
    else if (jt_ == b_+188) { goto subid4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
subid0:
  CALL_C(b_+14, checkInteractionState_hook, SYM(checkInteractionState), b_+17);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
    goto state0Common;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wScreenVariables);
  CYC(b_+22, b_+24); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); push_effect(gb, b_+28);
  interactionCodee2__getDirectionToFace_hook(gb);
  CYC(b_+28, b_+31);
  interactionSetAnimation_hook(gb);
  return;
state0Common:
  CYC(b_+31, b_+33); A = 0x01;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+37);
  CYC(b_+37, b_+40);
  objectSetVisible83_hook(gb);
  return;
subid2:
  CALL_C(b_+40, checkInteractionState_hook, SYM(checkInteractionState), b_+43);
  if (F & FZ) {
    CYCT(b_+43, b_+45);
    goto state0Common;
  }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+48); A = mem_rd(gb, wScreenVariables);
  CYC(b_+48, b_+50); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+54); push_effect(gb, b_+54);
  interactionCodee2__centerOnTileAndGetDirectionToFace_hook(gb);
offsetPositionTowardLookingDirection:
  CYC(b_+54, b_+57); SET_HL(b_+74);
  CYC(b_+57, b_+58);
  interactionCodee2_addDoubleIndexToHl_from_rst(gb, b_+58);
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+63); alu_and(gb, 0xf0);
  CYC(b_+63, b_+64); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CYC(b_+65, b_+66); SET_HL(HL + 1);
  CYC(b_+66, b_+68); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+71); alu_and(gb, 0xf0);
  CYC(b_+71, b_+72); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  RET(b_+73); return;
subid1:
  CYC(b_+121, b_+123); E = 0x02;
spawnChildren:
  CYC(b_+123, b_+126); SET_BC((wRoomLayout + 174));
spawnChildrenLoop:
  CYC(b_+126, b_+127); A = mem_rd(gb, BC);
  CYC(b_+127, b_+129); alu_cp(gb, 0xee);
  if (F & FZ) {
    CYCT(b_+129, b_+132);
    push_effect(gb, b_+132);
    interactionCodee2__spawnChild_hook(gb);
  } else {
    CYC(b_+129, b_+132);
  }
  CYC(b_+132, b_+133); C = alu_dec8(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+133, b_+135);
    goto spawnChildrenLoop;
  }
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+138);
  interactionDelete_hook(gb);
  return;
subid3:
  CALL_C(b_+160, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+163);
  CYC(b_+163, b_+166); A = mem_rd(gb, wEyePuzzleTransitionCounter);
  CYC(b_+166, b_+168); alu_cp(gb, 0x06);
  CYC(b_+168, b_+170); A = 0x00;
  if (!(F & FC)) {
    CYCT(b_+170, b_+172);
    goto storeCorrectDirection;
  }
  CYC(b_+170, b_+172);
randomDirectionLoop:
  CALL_C(b_+172, getRandomNumber_hook, SYM(getRandomNumber), b_+175);
  CYC(b_+175, b_+177); alu_and(gb, 0x03);
  CYC(b_+177, b_+179); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+179, b_+181);
    goto randomDirectionLoop;
  }
  CYC(b_+179, b_+181);
storeCorrectDirection:
  CYC(b_+181, b_+184); mem_wr(gb, wEyePuzzleCorrectDirection, A);
  CYC(b_+184, b_+186); E = 0x04;
  CYC(b_+186, b_+188);
  goto spawnChildren;
subid4:
  CYC(b_+188, b_+190); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+190, b_+191); A = mem_rd(gb, DE);
  CYC(b_+191, b_+192); push_effect(gb, b_+192);
  do { uint16_t jt_ = (interactionCodee2_jump_table(gb));
    if (jt_ == b_+31) { goto state0Common; }
    else if (jt_ == b_+198) { goto subid4State1; }
    else if (jt_ == SYM(objectSetVisible83)) { objectSetVisible83_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
subid4State1:
  CALL_C(b_+198, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+201);
  if (F & FZ) {
    CYCT(b_+201, b_+203);
    goto substate0;
  }
  CYC(b_+201, b_+203);
  CALL_C(b_+203, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+206);
  if (!(F & FZ)) {
    CYCT(b_+206, b_+208);
    goto eyeSpinning;
  }
  CYC(b_+206, b_+208);
  CALL_C(b_+208, interactionIncState_hook, SYM(interactionIncState), b_+211);
  CYC(b_+211, b_+214); A = mem_rd(gb, wEyePuzzleCorrectDirection);
  CYC(b_+214, b_+215); B = A;
randomFacingLoop:
  CYC(b_+215, b_+218); SET_HL(wFrameCounter);
  CYC(b_+218, b_+219); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+219, b_+220); A = mem_rd(gb, HL);
  CYC(b_+220, b_+222); alu_and(gb, 0x03);
  CYC(b_+222, b_+223); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+223, b_+225);
    goto randomFacingLoop;
  }
  CYC(b_+223, b_+225);
  CYC(b_+225, b_+226); alu_add(gb, A);
  CYC(b_+226, b_+229);
  goto offsetPositionTowardLookingDirection;
eyeSpinning:
  CYC(b_+229, b_+232); A = mem_rd(gb, wFrameCounter);
  CYC(b_+232, b_+234); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+234); return; }
  CYC(b_+234, b_+235);
  CALL_C(b_+235, getRandomNumber_hook, SYM(getRandomNumber), b_+238);
  CYC(b_+238, b_+240); alu_and(gb, 0x07);
  CYC(b_+240, b_+243);
  goto offsetPositionTowardLookingDirection;
substate0:
  CYC(b_+243, b_+245); A = 0x3c;
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CYC(b_+246, b_+248); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+248, b_+249); mem_wr(gb, DE, A);
  RET(b_+249); return;
}
