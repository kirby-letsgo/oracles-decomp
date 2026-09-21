#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t triforceStone_jumpTable(GB *gb) {
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

// interactionCode34@checkPushedStoneLongEnough: zflag set once Link has pushed against
// the stone long enough. Reached only by a static `call` from interactionCode34 itself.
static void triforceStone_checkPushedStoneLongEnough(GB *gb, uint16_t sp0_) {
  BASE(interactionCode34);
  // Check Link's X is close enough
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+104); SET_HL(w1Link_xh);
  CYC(b_+104, b_+105); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+105, b_+107); goto absX;
  }
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+108); alu_cpl(gb);
  CYC(b_+108, b_+109); A = alu_inc8(gb, A);
absX:
  CYC(b_+109, b_+111); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(b_+111, b_+113); goto notPushing;
  }
  CYC(b_+111, b_+113);
  // Check Link's Y is close enough
  CYC(b_+113, b_+115); L = (uint8_t)w1Link_yh;
  CYC(b_+115, b_+116); A = mem_rd(gb, HL);
  CYC(b_+116, b_+118); alu_cp(gb, 0x2a);
  if (!(F & FC)) {
    CYCT(b_+118, b_+120); goto notPushing;
  }
  CYC(b_+118, b_+120);
  // Check he's facing left or right
  CYC(b_+120, b_+122); L = (uint8_t)w1Link_direction;
  CYC(b_+122, b_+123); A = mem_rd(gb, HL);
  CYC(b_+123, b_+125); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+125, b_+127); goto notPushing;
  }
  CYC(b_+125, b_+127);
  // Check if he's pushing
  CALL_C(b_+127, objectCheckLinkPushingAgainstCenter_hook, SYM(objectCheckLinkPushingAgainstCenter), b_+130);
  if (!(F & FC)) {
    CYCT(b_+130, b_+132); goto notPushing;
  }
  CYC(b_+130, b_+132);
  // Make Link do the push animation
  CYC(b_+132, b_+134); A = 0x01;
  CYC(b_+134, b_+137); mem_wr(gb, wForceLinkPushAnimation, A);
  // Wait for him to push for enough frames
  CALL_C(b_+137, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+140);
  if (!(F & FZ)) {
    CYCT(b_+140, b_+141); ret_effect(gb); return;
  }
  CYC(b_+140, b_+141);
  // Get the direction Link is relative to the stone
  CYC(b_+141, b_+143); C = 0x28;
  CALL_C(b_+143, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+146);
  CYC(b_+146, b_+148); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+148, b_+150); alu_and(gb, 0x07);
  CYC(b_+150, b_+152); alu_xor(gb, 0x04);
  CYC(b_+152, b_+153); alu_add(gb, A);
  CYC(b_+153, b_+154); alu_add(gb, A);
  CYC(b_+154, b_+155); mem_wr(gb, DE, A);
  CYC(b_+155, b_+156); alu_xor(gb, A);
  CYC(b_+156, b_+157); ret_effect(gb);
  return;

notPushing:
  CYC(b_+157, b_+158); alu_xor(gb, A);
  CYC(b_+158, b_+161); mem_wr(gb, wForceLinkPushAnimation, A);
  CYC(b_+161, b_+163); A = 0x14;
  CYC(b_+163, b_+165); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+165, b_+166); mem_wr(gb, DE, A);
  CYC(b_+166, b_+167); alu_or(gb, A);
  CYC(b_+167, b_+168); ret_effect(gb);
}

// interactionCode34@setSolidTile: clear the layout tile at this position and mark its
// collisions solid. Reached only by a static `call` from interactionCode34 itself.
static void triforceStone_setSolidTile(GB *gb, uint16_t sp0_) {
  BASE(interactionCode34);
  CALL_C(b_+225, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+228);
  CYC(b_+228, b_+229); C = A;
  CYC(b_+229, b_+231); B = wRoomLayout >> 8;
  CYC(b_+231, b_+233); A = 0x00;
  CYC(b_+233, b_+234); mem_wr(gb, BC, A);
  CYC(b_+234, b_+236); B = wRoomCollisions >> 8;
  CYC(b_+236, b_+238); A = 0x0f;
  CYC(b_+238, b_+239); mem_wr(gb, BC, A);
  CYC(b_+239, b_+240); ret_effect(gb);
}

// @state1: waiting for Link to push (substate 0), pushing (substate 1), done (substate 2).
static void triforceStone_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode34);
  CYC(b_+41, b_+43); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); push_effect(gb, b_+45);
  do { uint16_t jt_ = (triforceStone_jumpTable(gb));
    if (jt_ == b_+51) { goto substate0; }
    else if (jt_ == b_+168) { goto substate1; }
    else if (jt_ == b_+224) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+51, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+54);
  CYC(b_+54, b_+57); push_effect(gb, b_+57); triforceStone_checkPushedStoneLongEnough(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+57, b_+58); ret_effect(gb); return;
  }
  CYC(b_+57, b_+58);
  // Begin stone-pushing cutscene
  CALL_C(b_+58, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+61);
  CYC(b_+61, b_+63); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x40);
  CYC(b_+69, b_+71); A = 0x08; // SPECIALOBJECT_LINK_CUTSCENE
  CALL_C(b_+71, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+74);
  CYC(b_+74, b_+76); L = (uint8_t)w1Link_subid;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x06);
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+80, b_+82); L = (uint8_t)w1Link_angle;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A);
  CYC(b_+84, b_+86); L = (uint8_t)w1Link_speed;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+88, b_+91); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x06);
  CYC(b_+93, b_+95); A = 0xb2; // SND_MAKUDISAPPEAR
  CYC(b_+95, b_+98); playSound_b00_hook(gb);
  return;

substate1:
  // In the process of pushing the stone
  CALL_C(b_+168, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+171);
  CALL_C(b_+171, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+174);
  if (!(F & FZ)) {
    CYCT(b_+174, b_+176); goto applySpeed;
  }
  CYC(b_+174, b_+176);
  // Finished pushing; determine new X-position
  CYC(b_+176, b_+178); B = 0x48;
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  CYC(b_+181, b_+183); alu_and(gb, 0x10);
  if (F & FZ) {
    CYCT(b_+183, b_+185);
  } else {
    CYC(b_+183, b_+185);
    CYC(b_+185, b_+187); B = 0x28;
  }
  CYC(b_+187, b_+189); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+189, b_+190); mem_wr(gb, HL, B);
  CALL_C(b_+190, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+193);
  // Determine bit to set on room flags (depends which way it was pushed)
  CALL_C(b_+193, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+196);
  CYC(b_+196, b_+197); A = B;
  CYC(b_+197, b_+199); alu_cp(gb, 0x28);
  CYC(b_+199, b_+201); B = 0x40;
  if (F & FZ) {
    CYCT(b_+201, b_+203);
  } else {
    CYC(b_+201, b_+203);
    CYC(b_+203, b_+205); B = 0x80;
  }
  CYC(b_+205, b_+206); A = mem_rd(gb, HL);
  CYC(b_+206, b_+207); alu_or(gb, B);
  CYC(b_+207, b_+208); mem_wr(gb, HL, A);
  CYC(b_+208, b_+211); push_effect(gb, b_+211); triforceStone_setSolidTile(gb, sp0_);
  CYC(b_+211, b_+213); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+213, playSound_b00_hook, SYM(playSound_b00), b_+216);
  CYC(b_+216, b_+218); A = 0x5b; // SND_SOLVEPUZZLE_2
  CYC(b_+218, b_+221); playSound_b00_hook(gb);
  return;

applySpeed:
  CYC(b_+221, b_+224); objectApplySpeed_hook(gb);
  return;

substate2:
  CYC(b_+224, b_+225); ret_effect(gb);
}

// INTERAC_TRIFORCE_STONE: the pushable stone in the Zora Seas' triforce room.
void interactionCode34_hook(GB *gb) {
  BASE(interactionCode34);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (triforceStone_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+41) { triforceStone_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  // Delete self if the stone was pushed already
  CALL_C(b_+11, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0xc0);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+19); TAIL(interactionDelete);
  }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x03);
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+27, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+30);
  CALL_C(b_+30, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+33);
  CYC(b_+33, b_+35); A = 0x98; // PALH_98
  CALL_C(b_+35, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+38);
  CYC(b_+38, b_+41); objectSetVisible83_hook(gb);
}
