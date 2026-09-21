#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// objectData.objectData_faroreSparkle (bank $12 object data, referenced by address only).
#define objectData_faroreSparkle_bank12 SYM(objectData_faroreSparkle)
// interac11_subid01@initialAngles: 8 bytes indexed by the sparkle's subid high nibble.
#define initialAngles_bank08 SYM(interac11_subid01__initialAngles)

// The parent (subid 0) talks to the sparkles through two raw wTmpcfc0 bytes:
// $cfd8 = sparkle distance from the circle center, $cfd9 = "start flying away" signal.
#define faroreSparkleRadius (wTmpcfc0 + 0x18)
#define faroreSparkleFlyAway (wTmpcfc0 + 0x19)

void interac11_subid00_hook(GB *gb);
void interac11_subid01_hook(GB *gb);

static uint16_t faroreMakeChest_jumpTable(GB *gb) {
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

static void faroreMakeChest_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

void interactionCode11_hook(GB *gb) {
  BASE(interactionCode11);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (faroreMakeChest_jumpTable(gb));
    if (jt_ == SYM(interac11_subid00) && hook_enabled_at(gb, SYM(interac11_subid00))) { interac11_subid00_hook(gb); return; }
    else if (jt_ == SYM(interac11_subid01) && hook_enabled_at(gb, SYM(interac11_subid01))) { interac11_subid01_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void interac11_subid00_hook(GB *gb) {
  BASE(interac11_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (faroreMakeChest_jumpTable(gb));
    if (jt_ == b_+26) { goto state0; }
    else if (jt_ == b_+49) { goto state1; }
    else if (jt_ == b_+64) { goto state2; }
    else if (jt_ == b_+73) { goto state3; }
    else if (jt_ == b_+82) { goto state4; }
    else if (jt_ == b_+101) { goto state5; }
    else if (jt_ == b_+137) { goto state678; }
    else if (jt_ == b_+154) { goto state9; }
    else if (jt_ == b_+165) { goto stateA; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+26, b_+28); A = 0x30;
  CYC(b_+28, b_+31); mem_wr(gb, faroreSparkleRadius, A);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+35); mem_wr(gb, faroreSparkleFlyAway, A);
  CALL_C(b_+35, setCameraFocusedObject_hook, SYM(setCameraFocusedObject), b_+38);
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+42); A = 0x5a;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CALL_C(b_+43, darkenRoomLightly_hook, SYM(darkenRoomLightly), b_+46);
  CYC(b_+46, b_+49); interactionIncState_hook(gb);
  return;

state1:
  CALL_C(b_+49, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+52);
  if (!(F & FZ)) {
    CYCT(b_+52, b_+53); ret_effect(gb); return;
  }
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x30);
  CYC(b_+55, b_+58); SET_HL(objectData_faroreSparkle_bank12);
  CALL_C(b_+58, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+61);
  CYC(b_+61, b_+64); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(b_+64, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+67);
  if (!(F & FZ)) {
    CYCT(b_+67, b_+68); ret_effect(gb); return;
  }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x1e);
  CYC(b_+70, b_+73); interactionIncState_hook(gb);
  return;

state3:
  CALL_C(b_+73, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+76);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+77); ret_effect(gb); return;
  }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x50);
  CYC(b_+79, b_+82); interactionIncState_hook(gb);
  return;

state4:
  CYC(b_+82, b_+85); A = mem_rd(gb, wFrameCounter);
  CYC(b_+85, b_+86); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+86, b_+88);
  } else {
    CYC(b_+86, b_+88);
    CYC(b_+88, b_+91); SET_HL(faroreSparkleRadius);
    CYC(b_+91, b_+92); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  }
  CALL_C(b_+92, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+95);
  if (!(F & FZ)) {
    CYCT(b_+95, b_+96); ret_effect(gb); return;
  }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x28);
  CYC(b_+98, b_+101); interactionIncState_hook(gb);
  return;

state5:
  CALL_C(b_+101, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+104);
  if (!(F & FZ)) {
    CYCT(b_+104, b_+105); ret_effect(gb); return;
  }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x08);
  CYC(b_+107, b_+109); A = 0x01;
  CYC(b_+109, b_+112); mem_wr(gb, faroreSparkleFlyAway, A);
  CYC(b_+112, b_+115); SET_BC(GV(0x840c, 0x8404));
  CALL_C(b_+115, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+118);
  CYC(b_+118, b_+120); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+120, b_+122); mem_wr(gb, HL, INTERACTION_BASE);
  CYC(b_+122, b_+123); L = alu_inc8(gb, L);
  CYC(b_+123, b_+124); mem_wr(gb, HL, D);
  CALL_C(b_+124, objectCreatePuff_hook, SYM(objectCreatePuff), b_+127);
  CYC(b_+127, b_+129); A = 0xf1;
  CYC(b_+129, b_+131); C = 0x75;
  CALL_C(b_+131, setTile_hook, SYM(setTile), b_+134);
  CYC(b_+134, b_+137); interactionIncState_hook(gb);
  return;

state678:
  CALL_C(b_+137, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+140);
  if (!(F & FZ)) {
    CYCT(b_+140, b_+141); ret_effect(gb); return;
  }
  CYC(b_+140, b_+141);
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x10);
  CALL_C(b_+143, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+146);

playFadeoutSound:
  CYC(b_+146, b_+148); A = 0xb4;
  CALL_C(b_+148, playSound_b00_hook, SYM(playSound_b00), b_+151);
  CYC(b_+151, b_+154); interactionIncState_hook(gb);
  return;

state9:
  CALL_C(b_+154, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+157);
  if (!(F & FZ)) {
    CYCT(b_+157, b_+158); ret_effect(gb); return;
  }
  CYC(b_+157, b_+158);
  CYC(b_+158, b_+160); A = 0x04;
  CALL_C(b_+160, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+163);
  CYC(b_+163, b_+165); goto playFadeoutSound;

stateA:
  CYC(b_+165, b_+168); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+168, b_+169); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+169, b_+170); ret_effect(gb); return;
  }
  CYC(b_+169, b_+170);
  CYC(b_+170, b_+172); A = 0x01;
  CYC(b_+172, b_+175); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+175, b_+176); alu_xor(gb, A);
  CYC(b_+176, b_+179); mem_wr(gb, wPaletteThread_parameter, A);
  CALL_C(b_+179, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+182);
  CYC(b_+182, b_+185); TAIL(interactionDelete);
}

// interac11_subid01@interac11_updateSparkle: rotate the sparkle around the circle center
// at the radius given by the parent. Reached only by a static `call` from subid 1 state 2;
// not independently hookable.
static void faroreMakeChest_updateSparkle(GB *gb, uint16_t sp0_) {
  BASE(interac11_subid01);
  CYC(b_+96, b_+99); A = mem_rd(gb, wFrameCounter);
  CYC(b_+99, b_+100); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+100, b_+102);
  } else {
    CYC(b_+100, b_+102);
    CYC(b_+102, b_+103); H = D;
    CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_ANGLE;
    CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+106, b_+107); A = mem_rd(gb, HL);
    CYC(b_+107, b_+109); alu_and(gb, 0x1f);
    CYC(b_+109, b_+110); mem_wr(gb, HL, A);
    CYC(b_+110, b_+112); A = 0xc9;
    if (F & FZ) {
      CALL_C_CC(b_+112, playSound_b00_hook, SYM(playSound_b00), b_+115);
    } else {
      CYC(b_+112, b_+115);
    }
  }
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+117, b_+120); SET_BC(0x7858);
  CYC(b_+120, b_+123); A = mem_rd(gb, faroreSparkleRadius);
  CALL_C(b_+123, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+126);
  CYC(b_+126, b_+129); interactionAnimate_hook(gb);
}

void interac11_subid01_hook(GB *gb) {
  BASE(interac11_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (faroreMakeChest_jumpTable(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+54) { goto state1; }
    else if (jt_ == b_+67) { goto state2; }
    else if (jt_ == b_+83) { goto state3; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+22); SET_HL(initialAngles_bank08);
  CYC(b_+22, b_+23); push_effect(gb, b_+23); faroreMakeChest_addAToHl(gb);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+29, b_+31); A = 0x28;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+36); A = 0x30;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CALL_C(b_+37, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+40);
  CALL_C(b_+40, objectSetVisible80_hook, SYM(objectSetVisible80), b_+43);
  CYC(b_+43, b_+46); interactionIncState_hook(gb);
  return;

state1:
  CALL_C(b_+54, objectApplySpeed_hook, SYM(objectApplySpeed), b_+57);
  CALL_C(b_+57, interactionAnimate_hook, SYM(interactionAnimate), b_+60);
  CALL_C(b_+60, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+63);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+64); ret_effect(gb); return;
  }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+67); interactionIncState_hook(gb);
  return;

state2:
  CYC(b_+67, b_+70); push_effect(gb, b_+70); faroreMakeChest_updateSparkle(gb, sp0_);
  CYC(b_+70, b_+73); A = mem_rd(gb, faroreSparkleFlyAway);
  CYC(b_+73, b_+74); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+74, b_+75); ret_effect(gb); return;
  }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+77); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+77, b_+79); A = 0x50;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+83); interactionIncState_hook(gb);
  return;

state3:
  CALL_C(b_+83, objectApplySpeed_hook, SYM(objectApplySpeed), b_+86);
  CALL_C(b_+86, interactionAnimate_hook, SYM(interactionAnimate), b_+89);
  CALL_C(b_+89, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+92);
  if (F & FC) {
    CYCT(b_+92, b_+93); ret_effect(gb); return;
  }
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+96); TAIL(interactionDelete);
}
