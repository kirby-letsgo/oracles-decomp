#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Subid 0, indexed by var03 (the original xh): b0 = dest room, b1 = dest position.
#define specialWarpSubid0WarpData_bank08 0x4b63
// Subid 1: m_HardcodedWarpA ROOM_AGES_5b8, $00, $93, $03
#define specialWarpSubid1WarpDestVariables_bank08 0x4bb7
// Subid 2: m_HardcodedWarpB ROOM_AGES_037, $0e, $22, $03
#define specialWarpSubid2WarpDestVariables_bank08 0x4be4

#define SPECIALOBJECT_DIMITRI 0x0c

static uint16_t specialWarp_jumpTable(GB *gb) {
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

static void specialWarp_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @subid0: trigger a warp when Link dives while touching this object.
static void specialWarp_subid0(GB *gb, uint16_t sp0_) {
  CALL_C(0x4b34, checkInteractionState_hook, 0x23fe, 0x4b37);
  if (F & FZ) {
    CYCT(0x4b37, 0x4b39); goto initialize;
  }
  CYC(0x4b37, 0x4b39);
  // Link must be diving, and have collided with this object without holding anything
  CYC(0x4b39, 0x4b3c); A = mem_rd(gb, wLinkSwimmingState);
  CYC(0x4b3c, 0x4b3d); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x4b3d, 0x4b3e); ret_effect(gb); return;
  }
  CYC(0x4b3d, 0x4b3e);
  CALL_C(0x4b3e, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x4b41);
  if (!(F & FC)) {
    CYCT(0x4b41, 0x4b42); ret_effect(gb); return;
  }
  CYC(0x4b41, 0x4b42);
  CYC(0x4b42, 0x4b44); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4b44, 0x4b45); A = mem_rd(gb, DE);
  CYC(0x4b45, 0x4b48); SET_HL(specialWarpSubid0WarpData_bank08);
  CYC(0x4b48, 0x4b49); specialWarp_addDoubleIndex(gb, 0x4b49);
  CYC(0x4b49, 0x4b4a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4b4a, 0x4b4d); mem_wr(gb, wWarpDestRoom, A);
  CYC(0x4b4d, 0x4b4e); A = mem_rd(gb, HL);
  CYC(0x4b4e, 0x4b51); mem_wr(gb, wWarpDestPos, A);
  CYC(0x4b51, 0x4b53); A = 0x87;
  CYC(0x4b53, 0x4b56); mem_wr(gb, wWarpDestGroup, A);
  CYC(0x4b56, 0x4b58); A = 0x01;
  CYC(0x4b58, 0x4b5b); mem_wr(gb, wWarpTransition, A);
  CYC(0x4b5b, 0x4b5d); A = 0x03;
  CYC(0x4b5d, 0x4b60); mem_wr(gb, wWarpTransition2, A);
  CYC(0x4b60, 0x4b63); interactionDelete_hook(gb);
  return;

initialize:
  // @subid0@initialize: xh is a parameter (warp index) saved to var03 before the
  // short-form position in yh is expanded.
  CYC(0x4b67, 0x4b69); A = 0x01;
  CYC(0x4b69, 0x4b6a); mem_wr(gb, DE, A);
  CYC(0x4b6a, 0x4b6c); A = 0x02;
  CALL_C(0x4b6c, objectSetCollideRadius_hook, 0x24a1, 0x4b6f);
  CYC(0x4b6f, 0x4b71); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x4b71, 0x4b72); A = mem_rd(gb, HL);
  CYC(0x4b72, 0x4b74); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4b74, 0x4b75); mem_wr(gb, HL, A);
  CYC(0x4b75, 0x4b77); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x4b77, 0x4b78); C = mem_rd(gb, HL);
  CYC(0x4b78, 0x4b7b); setShortPosition_paramC_hook(gb);
}

// @subid1State0: only exists while riding Dimitri.
static void specialWarp_subid1State0(GB *gb, uint16_t sp0_) {
  CYC(0x4b85, 0x4b88); A = mem_rd(gb, wAnimalCompanion);
  CYC(0x4b88, 0x4b8a); alu_cp(gb, SPECIALOBJECT_DIMITRI);
  if (!(F & FZ)) {
    CYCT(0x4b8a, 0x4b8d); interactionDelete_hook(gb); return;
  }
  CYC(0x4b8a, 0x4b8d);
  CYC(0x4b8d, 0x4b90); SET_BC(0x0810);
  CALL_C(0x4b90, objectSetCollideRadii_hook, 0x24a9, 0x4b93);
  CALL_C(0x4b93, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x4b96);
  if (!(F & FC)) {
    CALL_C_CC(0x4b96, interactionIncState_hook, 0x23e0, 0x4b99);
  } else {
    CYC(0x4b96, 0x4b99);
  }
  CYC(0x4b99, 0x4b9c); interactionIncState_hook(gb);
}

// @subid1State1: wait for Link to stop touching the object.
static void specialWarp_subid1State1(GB *gb, uint16_t sp0_) {
  CALL_C(0x4b9c, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x4b9f);
  if (F & FC) {
    CYCT(0x4b9f, 0x4ba0); ret_effect(gb); return;
  }
  CYC(0x4b9f, 0x4ba0);
  CYC(0x4ba0, 0x4ba3); interactionIncState_hook(gb);
}

// @subid1State2: warp when Link (riding the companion) touches the object.
static void specialWarp_subid1State2(GB *gb, uint16_t sp0_) {
  CYC(0x4ba3, 0x4ba4); A = D;
  CYC(0x4ba4, 0x4ba7); mem_wr(gb, wDisableWarpTiles, A);
  CYC(0x4ba7, 0x4baa); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x4baa, 0x4bac); alu_cp(gb, w1Companion >> 8);
  if (!(F & FZ)) {
    CYCT(0x4bac, 0x4bad); ret_effect(gb); return;
  }
  CYC(0x4bac, 0x4bad);
  CALL_C(0x4bad, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x4bb0);
  if (!(F & FC)) {
    CYCT(0x4bb0, 0x4bb1); ret_effect(gb); return;
  }
  CYC(0x4bb0, 0x4bb1);
  CYC(0x4bb1, 0x4bb4); SET_HL(specialWarpSubid1WarpDestVariables_bank08);
  CYC(0x4bb4, 0x4bb7); setWarpDestVariables_hook(gb);
}

// @subid1: a warp at the top of a waterfall.
static void specialWarp_subid1(GB *gb, uint16_t sp0_) {
  CYC(0x4b7b, 0x4b7d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4b7d, 0x4b7e); A = mem_rd(gb, DE);
  CYC(0x4b7e, 0x4b7f); push_effect(gb, 0x4b7f);
  switch (specialWarp_jumpTable(gb)) {
    case 0x4b85: specialWarp_subid1State0(gb, sp0_); return;
    case 0x4b9c: specialWarp_subid1State1(gb, sp0_); return;
    case 0x4ba3: specialWarp_subid1State2(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}

// @subid2: a warp in a cave in a waterfall; triggers once the companion's y reaches $a8.
static void specialWarp_subid2(GB *gb, uint16_t sp0_) {
  CYC(0x4bbc, 0x4bbd); A = D;
  CYC(0x4bbd, 0x4bc0); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(0x4bc0, checkInteractionState_hook, 0x23fe, 0x4bc3);
  if (F & FZ) {
    CYCT(0x4bc3, 0x4bc5); goto initialize;
  }
  CYC(0x4bc3, 0x4bc5);
  CALL_C(0x4bc5, checkLinkCollisionsEnabled_hook, 0x1d32, 0x4bc8);
  if (!(F & FC)) {
    CYCT(0x4bc8, 0x4bc9); ret_effect(gb); return;
  }
  CYC(0x4bc8, 0x4bc9);
  CYC(0x4bc9, 0x4bcc); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x4bcc, 0x4bce); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(0x4bce, 0x4bcf); ret_effect(gb); return;
  }
  CYC(0x4bce, 0x4bcf);
  CYC(0x4bcf, 0x4bd0); H = A;
  CYC(0x4bd0, 0x4bd2); L = (uint8_t)w1Companion_yh;
  CYC(0x4bd2, 0x4bd3); A = mem_rd(gb, HL);
  CYC(0x4bd3, 0x4bd5); alu_cp(gb, 0xa8);
  if (F & FC) {
    CYCT(0x4bd5, 0x4bd6); ret_effect(gb); return;
  }
  CYC(0x4bd5, 0x4bd6);
  CYC(0x4bd6, 0x4bd8); A = 0xff;
  CYC(0x4bd8, 0x4bdb); mem_wr(gb, wDisabledObjects, A);
  CYC(0x4bdb, 0x4bde); SET_HL(specialWarpSubid2WarpDestVariables_bank08);
  CALL_C(0x4bde, setWarpDestVariables_hook, 0x1997, 0x4be1);
  CYC(0x4be1, 0x4be4); interactionDelete_hook(gb);
  return;

initialize:
  // @subid2@initialize
  CALL_C(0x4be9, interactionIncState_hook, 0x23e0, 0x4bec);
  CYC(0x4bec, 0x4bef); interactionSetAlwaysUpdateBit_hook(gb);
}

// INTERAC_SPECIAL_WARP: hardcoded warps that are not part of the room's warp data.
void interactionCode1f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b2a, 0x4b2c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4b2c, 0x4b2d); A = mem_rd(gb, DE);
  CYC(0x4b2d, 0x4b2e); push_effect(gb, 0x4b2e);
  switch (specialWarp_jumpTable(gb)) {
    case 0x4b34: specialWarp_subid0(gb, sp0_); return;
    case 0x4b7b: specialWarp_subid1(gb, sp0_); return;
    case 0x4bbc: specialWarp_subid2(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
