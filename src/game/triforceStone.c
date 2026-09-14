#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

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
  // Check Link's X is close enough
  CYC(0x6481, 0x6483); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x6483, 0x6484); A = mem_rd(gb, DE);
  CYC(0x6484, 0x6487); SET_HL(w1Link_xh);
  CYC(0x6487, 0x6488); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x6488, 0x648a); goto absX;
  }
  CYC(0x6488, 0x648a);
  CYC(0x648a, 0x648b); alu_cpl(gb);
  CYC(0x648b, 0x648c); A = alu_inc8(gb, A);
absX:
  CYC(0x648c, 0x648e); alu_cp(gb, 0x11);
  if (!(F & FC)) {
    CYCT(0x648e, 0x6490); goto notPushing;
  }
  CYC(0x648e, 0x6490);
  // Check Link's Y is close enough
  CYC(0x6490, 0x6492); L = (uint8_t)w1Link_yh;
  CYC(0x6492, 0x6493); A = mem_rd(gb, HL);
  CYC(0x6493, 0x6495); alu_cp(gb, 0x2a);
  if (!(F & FC)) {
    CYCT(0x6495, 0x6497); goto notPushing;
  }
  CYC(0x6495, 0x6497);
  // Check he's facing left or right
  CYC(0x6497, 0x6499); L = (uint8_t)w1Link_direction;
  CYC(0x6499, 0x649a); A = mem_rd(gb, HL);
  CYC(0x649a, 0x649c); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x649c, 0x649e); goto notPushing;
  }
  CYC(0x649c, 0x649e);
  // Check if he's pushing
  CALL_C(0x649e, objectCheckLinkPushingAgainstCenter_hook, 0x2707, 0x64a1);
  if (!(F & FC)) {
    CYCT(0x64a1, 0x64a3); goto notPushing;
  }
  CYC(0x64a1, 0x64a3);
  // Make Link do the push animation
  CYC(0x64a3, 0x64a5); A = 0x01;
  CYC(0x64a5, 0x64a8); mem_wr(gb, wForceLinkPushAnimation, A);
  // Wait for him to push for enough frames
  CALL_C(0x64a8, interactionDecCounter1_hook, 0x23cc, 0x64ab);
  if (!(F & FZ)) {
    CYCT(0x64ab, 0x64ac); ret_effect(gb); return;
  }
  CYC(0x64ab, 0x64ac);
  // Get the direction Link is relative to the stone
  CYC(0x64ac, 0x64ae); C = 0x28;
  CALL_C(0x64ae, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x64b1);
  CYC(0x64b1, 0x64b3); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x64b3, 0x64b5); alu_and(gb, 0x07);
  CYC(0x64b5, 0x64b7); alu_xor(gb, 0x04);
  CYC(0x64b7, 0x64b8); alu_add(gb, A);
  CYC(0x64b8, 0x64b9); alu_add(gb, A);
  CYC(0x64b9, 0x64ba); mem_wr(gb, DE, A);
  CYC(0x64ba, 0x64bb); alu_xor(gb, A);
  CYC(0x64bb, 0x64bc); ret_effect(gb);
  return;

notPushing:
  CYC(0x64bc, 0x64bd); alu_xor(gb, A);
  CYC(0x64bd, 0x64c0); mem_wr(gb, wForceLinkPushAnimation, A);
  CYC(0x64c0, 0x64c2); A = 0x14;
  CYC(0x64c2, 0x64c4); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x64c4, 0x64c5); mem_wr(gb, DE, A);
  CYC(0x64c5, 0x64c6); alu_or(gb, A);
  CYC(0x64c6, 0x64c7); ret_effect(gb);
}

// interactionCode34@setSolidTile: clear the layout tile at this position and mark its
// collisions solid. Reached only by a static `call` from interactionCode34 itself.
static void triforceStone_setSolidTile(GB *gb, uint16_t sp0_) {
  CALL_C(0x6500, objectGetShortPosition_hook, 0x2096, 0x6503);
  CYC(0x6503, 0x6504); C = A;
  CYC(0x6504, 0x6506); B = wRoomLayout >> 8;
  CYC(0x6506, 0x6508); A = 0x00;
  CYC(0x6508, 0x6509); mem_wr(gb, BC, A);
  CYC(0x6509, 0x650b); B = wRoomCollisions >> 8;
  CYC(0x650b, 0x650d); A = 0x0f;
  CYC(0x650d, 0x650e); mem_wr(gb, BC, A);
  CYC(0x650e, 0x650f); ret_effect(gb);
}

// @state1: waiting for Link to push (substate 0), pushing (substate 1), done (substate 2).
static void triforceStone_state1(GB *gb, uint16_t sp0_) {
  CYC(0x6448, 0x644a); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x644a, 0x644b); A = mem_rd(gb, DE);
  CYC(0x644b, 0x644c); push_effect(gb, 0x644c);
  switch (triforceStone_jumpTable(gb)) {
    case 0x6452: goto substate0;
    case 0x64c7: goto substate1;
    case 0x64ff: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x6452, objectPreventLinkFromPassing_hook, 0x2680, 0x6455);
  CYC(0x6455, 0x6458); push_effect(gb, 0x6458); triforceStone_checkPushedStoneLongEnough(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x6458, 0x6459); ret_effect(gb); return;
  }
  CYC(0x6458, 0x6459);
  // Begin stone-pushing cutscene
  CALL_C(0x6459, interactionIncSubstate_hook, 0x23e5, 0x645c);
  CYC(0x645c, 0x645e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x645e, 0x6460); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(0x6460, 0x6462); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6462, 0x6464); mem_wr(gb, HL, 0x40);
  CYC(0x6464, 0x6466); A = 0x08; // SPECIALOBJECT_LINK_CUTSCENE
  CALL_C(0x6466, setLinkIDOverride_hook, 0x2acf, 0x6469);
  CYC(0x6469, 0x646b); L = (uint8_t)w1Link_subid;
  CYC(0x646b, 0x646d); mem_wr(gb, HL, 0x06);
  CYC(0x646d, 0x646f); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x646f, 0x6471); L = (uint8_t)w1Link_angle;
  CYC(0x6471, 0x6472); A = mem_rd(gb, DE);
  CYC(0x6472, 0x6473); mem_wr(gb, HL, A);
  CYC(0x6473, 0x6475); L = (uint8_t)w1Link_speed;
  CYC(0x6475, 0x6477); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x6477, 0x647a); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x647a, 0x647c); mem_wr(gb, HL, 0x06);
  CYC(0x647c, 0x647e); A = 0xb2; // SND_MAKUDISAPPEAR
  CYC(0x647e, 0x6481); playSound_b00_hook(gb);
  return;

substate1:
  // In the process of pushing the stone
  CALL_C(0x64c7, objectPreventLinkFromPassing_hook, 0x2680, 0x64ca);
  CALL_C(0x64ca, interactionDecCounter1_hook, 0x23cc, 0x64cd);
  if (!(F & FZ)) {
    CYCT(0x64cd, 0x64cf); goto applySpeed;
  }
  CYC(0x64cd, 0x64cf);
  // Finished pushing; determine new X-position
  CYC(0x64cf, 0x64d1); B = 0x48;
  CYC(0x64d1, 0x64d3); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x64d3, 0x64d4); A = mem_rd(gb, DE);
  CYC(0x64d4, 0x64d6); alu_and(gb, 0x10);
  if (F & FZ) {
    CYCT(0x64d6, 0x64d8);
  } else {
    CYC(0x64d6, 0x64d8);
    CYC(0x64d8, 0x64da); B = 0x28;
  }
  CYC(0x64da, 0x64dc); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x64dc, 0x64dd); mem_wr(gb, HL, B);
  CALL_C(0x64dd, interactionIncSubstate_hook, 0x23e5, 0x64e0);
  // Determine bit to set on room flags (depends which way it was pushed)
  CALL_C(0x64e0, getThisRoomFlags_hook, 0x197d, 0x64e3);
  CYC(0x64e3, 0x64e4); A = B;
  CYC(0x64e4, 0x64e6); alu_cp(gb, 0x28);
  CYC(0x64e6, 0x64e8); B = 0x40;
  if (F & FZ) {
    CYCT(0x64e8, 0x64ea);
  } else {
    CYC(0x64e8, 0x64ea);
    CYC(0x64ea, 0x64ec); B = 0x80;
  }
  CYC(0x64ec, 0x64ed); A = mem_rd(gb, HL);
  CYC(0x64ed, 0x64ee); alu_or(gb, B);
  CYC(0x64ee, 0x64ef); mem_wr(gb, HL, A);
  CYC(0x64ef, 0x64f2); push_effect(gb, 0x64f2); triforceStone_setSolidTile(gb, sp0_);
  CYC(0x64f2, 0x64f4); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(0x64f4, playSound_b00_hook, 0x0c98, 0x64f7);
  CYC(0x64f7, 0x64f9); A = 0x5b; // SND_SOLVEPUZZLE_2
  CYC(0x64f9, 0x64fc); playSound_b00_hook(gb);
  return;

applySpeed:
  CYC(0x64fc, 0x64ff); objectApplySpeed_hook(gb);
  return;

substate2:
  CYC(0x64ff, 0x6500); ret_effect(gb);
}

// INTERAC_TRIFORCE_STONE: the pushable stone in the Zora Seas' triforce room.
void interactionCode34_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x641f, 0x6421); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6421, 0x6422); A = mem_rd(gb, DE);
  CYC(0x6422, 0x6423); push_effect(gb, 0x6423);
  switch (triforceStone_jumpTable(gb)) {
    case 0x6427: goto state0;
    case 0x6448: triforceStone_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x6427, 0x6429); A = 0x01;
  CYC(0x6429, 0x642a); mem_wr(gb, DE, A);
  // Delete self if the stone was pushed already
  CALL_C(0x642a, getThisRoomFlags_hook, 0x197d, 0x642d);
  CYC(0x642d, 0x642f); alu_and(gb, 0xc0);
  if (!(F & FZ)) {
    CYCT(0x642f, 0x6432); interactionDelete_hook(gb); return;
  }
  CYC(0x642f, 0x6432);
  CYC(0x6432, 0x6433); H = D;
  CYC(0x6433, 0x6435); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6435, 0x6437); mem_wr(gb, HL, 0x03);
  CYC(0x6437, 0x6438); L = alu_inc8(gb, L);
  CYC(0x6438, 0x643a); mem_wr(gb, HL, 0x0a);
  CALL_C(0x643a, objectMarkSolidPosition_hook, 0x24f0, 0x643d);
  CALL_C(0x643d, interactionInitGraphics_hook, 0x15fb, 0x6440);
  CYC(0x6440, 0x6442); A = 0x98; // PALH_98
  CALL_C(0x6442, loadPaletteHeader_hook, 0x050b, 0x6445);
  CYC(0x6445, 0x6448); objectSetVisible83_hook(gb);
}
