#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Data format: b0 = tile to transition into, b1 = tile to transition from.
#define doorControllerShutterTiles_bank08 0x4818
#define doorControllerScriptSubidTable_bank08 0x4838

static uint16_t doorController_jumpTable(GB *gb) {
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

static void doorController_addAToHl(GB *gb) {
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

static void doorController_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode1e@playSoundIfInScreenBoundary: plays sound A if the door is on screen.
// Reached only by static `call`s from interactionCode1e itself.
static void doorController_playSoundIfInScreenBoundary(GB *gb, uint16_t sp0_) {
  CYC(0x480d, 0x480f); H8(hFF8B) = A;
  CALL_C(0x480f, objectCheckWithinScreenBoundary_hook, 0x2184, 0x4812);
  if (!(F & FC)) {
    CYCT(0x4812, 0x4813); ret_effect(gb); return;
  }
  CYC(0x4812, 0x4813);
  CYC(0x4813, 0x4815); A = H8(hFF8B);
  CYC(0x4815, 0x4818); playSound_b00_hook(gb);
}

// interactionCode1e@checkRespawnLink: force Link to respawn if he's on the same tile as
// this object.
static void doorController_checkRespawnLink(GB *gb) {
  CYC(0x47c9, 0x47cc); A = W8(w1Link_yh);
  CYC(0x47cc, 0x47ce); alu_and(gb, 0xf0);
  CYC(0x47ce, 0x47cf); B = A;
  CYC(0x47cf, 0x47d2); A = W8(w1Link_xh);
  CYC(0x47d2, 0x47d4); A = alu_swap(gb, A);
  CYC(0x47d4, 0x47d6); alu_and(gb, 0x0f);
  CYC(0x47d6, 0x47d7); alu_or(gb, B);
  CYC(0x47d7, 0x47d8); B = A;
  CYC(0x47d8, 0x47da); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x47da, 0x47db); A = mem_rd(gb, DE);
  CYC(0x47db, 0x47dc); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x47dc, 0x47dd); ret_effect(gb); return;
  }
  CYC(0x47dc, 0x47dd);
  CYC(0x47dd, 0x47df); A = 0x02;
  CYC(0x47df, 0x47e2); mem_wr(gb, wScreenTransitionDelay, A);
  CYC(0x47e2, 0x47e5); respawnLink_hook(gb);
}

// interactionCode1e@func_47ee: for subids >= 4, count this door as one more closed door.
static void doorController_func_47ee(GB *gb) {
  CYC(0x47ee, 0x47f0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x47f0, 0x47f1); A = mem_rd(gb, DE);
  CYC(0x47f1, 0x47f3); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(0x47f3, 0x47f4); ret_effect(gb); return;
  }
  CYC(0x47f3, 0x47f4);
  CYC(0x47f4, 0x47f7); SET_HL(wcc93);
  CYC(0x47f7, 0x47f8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x47f8, 0x47f9); ret_effect(gb);
}

// interactionCode1e@func_47e5: if the door's tile is not solid, fall through into func_47ee.
static void doorController_func_47e5(GB *gb) {
  CYC(0x47e5, 0x47e7); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x47e7, 0x47e8); A = mem_rd(gb, DE);
  CYC(0x47e8, 0x47e9); C = A;
  CYC(0x47e9, 0x47eb); B = wRoomCollisions >> 8;
  CYC(0x47eb, 0x47ec); A = mem_rd(gb, BC);
  CYC(0x47ec, 0x47ed); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x47ed, 0x47ee); ret_effect(gb); return;
  }
  CYC(0x47ed, 0x47ee);
  doorController_func_47ee(gb);
}

// interactionCode1e@func_47f9: for subids >= 4, count one closed door fewer.
static void doorController_func_47f9(GB *gb) {
  CYC(0x47f9, 0x47fb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x47fb, 0x47fc); A = mem_rd(gb, DE);
  CYC(0x47fc, 0x47fe); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(0x47fe, 0x47ff); ret_effect(gb); return;
  }
  CYC(0x47fe, 0x47ff);
  CYC(0x47ff, 0x4802); SET_HL(wcc93);
  CYC(0x4802, 0x4803); A = mem_rd(gb, HL);
  CYC(0x4803, 0x4804); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4804, 0x4805); ret_effect(gb); return;
  }
  CYC(0x4804, 0x4805);
  CYC(0x4805, 0x4806); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4806, 0x4807); A = mem_rd(gb, HL);
  CYC(0x4807, 0x4809); alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(0x4809, 0x480a); ret_effect(gb); return;
  }
  CYC(0x4809, 0x480a);
  CYC(0x480a, 0x480c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x480c, 0x480d); ret_effect(gb);
}

// @state1: run the script; it decides when to move to state 2 (open) or 3 (close).
static void doorController_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x472c, interactionRunScript_hook, 0x2552, 0x472f);
  if (F & FC) {
    CYCT(0x472f, 0x4732); interactionDelete_hook(gb); return;
  }
  CYC(0x472f, 0x4732);
  CYC(0x4732, 0x4734); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4734, 0x4735); alu_xor(gb, A);
  CYC(0x4735, 0x4736); mem_wr(gb, DE, A);
  CYC(0x4736, 0x4737); ret_effect(gb);
}

// @gotoState1: back to state 1 / substate 0, then run state 1 immediately.
static void doorController_gotoState1(GB *gb, uint16_t sp0_) {
  CYC(0x47be, 0x47c0); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x47c0, 0x47c2); A = 0x01;
  CYC(0x47c2, 0x47c3); mem_wr(gb, DE, A);
  CYC(0x47c3, 0x47c4); E = alu_inc8(gb, E);
  CYC(0x47c4, 0x47c5); alu_xor(gb, A);
  CYC(0x47c5, 0x47c6); mem_wr(gb, DE, A);
  CYC(0x47c6, 0x47c9);
  doorController_state1(gb, sp0_);
}

// @setTileAndPlaySound: HL points at the final tile; write it and play the door sound.
static void doorController_setTileAndPlaySound(GB *gb, uint16_t sp0_) {
  CYC(0x47b1, 0x47b3); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x47b3, 0x47b4); A = mem_rd(gb, DE);
  CYC(0x47b4, 0x47b5); C = A;
  CYC(0x47b5, 0x47b6); A = mem_rd(gb, HL);
  CALL_C(0x47b6, setTile_hook, 0x3a9c, 0x47b9);
  CYC(0x47b9, 0x47bb); A = 0x70; // SND_DOORCLOSE
  CYC(0x47bb, 0x47be); push_effect(gb, 0x47be); doorController_playSoundIfInScreenBoundary(gb, sp0_);
  doorController_gotoState1(gb, sp0_);
}

// @interleaveDoorTile: start the half-open/half-closed door animation tile.
static void doorController_interleaveDoorTile(GB *gb, uint16_t sp0_) {
  CYC(0x4749, 0x474b); A = 0x70; // SND_DOORCLOSE
  CYC(0x474b, 0x474e); push_effect(gb, 0x474e); doorController_playSoundIfInScreenBoundary(gb, sp0_);
  CYC(0x474e, 0x4750); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4750, 0x4751); A = mem_rd(gb, DE);
  CYC(0x4751, 0x4754); SET_HL(doorControllerShutterTiles_bank08);
  CYC(0x4754, 0x4755); push_effect(gb, 0x4755); doorController_addAToHl(gb);
  CYC(0x4755, 0x4757); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x4757, 0x4758); A = mem_rd(gb, DE);
  CYC(0x4758, 0x475a); H8(hFF8C) = A;
  CYC(0x475a, 0x475b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x475b, 0x475d); H8(hFF8F) = A;
  CYC(0x475d, 0x475e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x475e, 0x4760); H8(hFF8E) = A;
  CYC(0x4760, 0x4762); alu_and(gb, 0x03);
  CALL_C(0x4762, setInterleavedTile_hook, 0x3acf, 0x4765);
  CYC(0x4765, 0x4767); A = H8(hActiveObject);
  CYC(0x4767, 0x4768); D = A;
  CYC(0x4768, 0x4769); H = D;
  CYC(0x4769, 0x476b); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x476b, 0x476c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x476c, 0x476e); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x476e, 0x4770); mem_wr(gb, HL, 0x06);
  // Set the new tile in the room layout (without calling setTile, so no visual update yet).
  CYC(0x4770, 0x4772); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x4772, 0x4773); C = mem_rd(gb, HL);
  CYC(0x4773, 0x4775); B = wRoomLayout >> 8;
  CYC(0x4775, 0x4777); A = H8(hFF8F);
  CYC(0x4777, 0x4778); mem_wr(gb, BC, A);
  CYC(0x4778, 0x4779); ret_effect(gb);
}

// @state0: init. "xh" is a parameter (0-7): a bit index for wActiveTriggers.
static void doorController_state0(GB *gb, uint16_t sp0_) {
  CYC(0x46fc, 0x46fe); A = 0x01;
  CYC(0x46fe, 0x46ff); mem_wr(gb, DE, A);
  CYC(0x46ff, 0x4700); H = D;
  CYC(0x4700, 0x4702); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x4702, 0x4704); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x4704, 0x4705); A = mem_rd(gb, HL);
  CYC(0x4705, 0x4706); mem_wr(gb, DE, A);
  CYC(0x4706, 0x4708); alu_and(gb, 0x07);
  CYC(0x4708, 0x470b); SET_BC(bitTable);
  CYC(0x470b, 0x470c); alu_add(gb, C);
  CYC(0x470c, 0x470d); C = A;
  CYC(0x470d, 0x470e); A = mem_rd(gb, BC);
  CYC(0x470e, 0x4710); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x4710, 0x4711); mem_wr(gb, HL, A);
  // Convert short-form position in yh to a full y/x position
  CYC(0x4711, 0x4713); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x4713, 0x4715); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x4715, 0x4716); A = mem_rd(gb, HL);
  CYC(0x4716, 0x4717); mem_wr(gb, DE, A);
  CYC(0x4717, 0x4719); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(0x4719, setShortPosition_hook, 0x20b8, 0x471c);
  // Pick the script from the subid
  CYC(0x471c, 0x471e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x471e, 0x471f); A = mem_rd(gb, DE);
  CYC(0x471f, 0x4722); SET_HL(doorControllerScriptSubidTable_bank08);
  CYC(0x4722, 0x4723); doorController_addDoubleIndex(gb, 0x4723);
  CYC(0x4723, 0x4724); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4724, 0x4725); H = mem_rd(gb, HL);
  CYC(0x4725, 0x4726); L = A;
  CALL_C(0x4726, interactionSetScript_hook, 0x2544, 0x4729);
  CYC(0x4729, 0x472c); push_effect(gb, 0x472c); doorController_func_47e5(gb);
  doorController_state1(gb, sp0_);
}

// @state2: a door is opening
static void doorController_state2(GB *gb, uint16_t sp0_) {
  CYC(0x4737, 0x473a); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x473a, 0x473b); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x473b, 0x473c); ret_effect(gb); return;
  }
  CYC(0x473b, 0x473c);
  CYC(0x473c, 0x473e); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x473e, 0x473f); A = mem_rd(gb, DE);
  CYC(0x473f, 0x4740); push_effect(gb, 0x4740);
  switch (doorController_jumpTable(gb)) {
    case 0x4744: goto substate0;
    case 0x4779: goto substate1;
    default: HANDOFF(HL);
  }

substate0:
  // The tile at this position must be solid
  CALL_C(0x4744, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x4747);
  if (!(F & FC)) {
    CYCT(0x4747, 0x4749); doorController_gotoState1(gb, sp0_); return;
  }
  CYC(0x4747, 0x4749);
  doorController_interleaveDoorTile(gb, sp0_);
  return;

substate1:
  CALL_C(0x4779, interactionDecCounter1_hook, 0x23cc, 0x477c);
  if (!(F & FZ)) {
    CYCT(0x477c, 0x477d); ret_effect(gb); return;
  }
  CYC(0x477c, 0x477d);
  // Door will now open fully
  CYC(0x477d, 0x4780); push_effect(gb, 0x4780); doorController_func_47ee(gb);
  CYC(0x4780, 0x4782); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4782, 0x4783); A = mem_rd(gb, DE);
  CYC(0x4783, 0x4786); SET_HL(doorControllerShutterTiles_bank08);
  CYC(0x4786, 0x4787); push_effect(gb, 0x4787); doorController_addAToHl(gb);
  CYC(0x4787, 0x4789);
  doorController_setTileAndPlaySound(gb, sp0_);
}

// @state3: a door is closing
static void doorController_state3(GB *gb, uint16_t sp0_) {
  CYC(0x4789, 0x478b); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x478b, 0x478c); A = mem_rd(gb, DE);
  CYC(0x478c, 0x478d); push_effect(gb, 0x478d);
  switch (doorController_jumpTable(gb)) {
    case 0x4791: goto substate0;
    case 0x479f: goto substate1;
    default: HANDOFF(HL);
  }

substate0:
  // The tile at this position must not be solid (a somaria block counts as not solid)
  CALL_C(0x4791, objectGetTileAtPosition_hook, 0x1444, 0x4794);
  CYC(0x4794, 0x4796); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (F & FZ) {
    CYCT(0x4796, 0x4798); doorController_interleaveDoorTile(gb, sp0_); return;
  }
  CYC(0x4796, 0x4798);
  CALL_C(0x4798, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x479b);
  if (F & FC) {
    CYCT(0x479b, 0x479d); doorController_gotoState1(gb, sp0_); return;
  }
  CYC(0x479b, 0x479d);
  CYC(0x479d, 0x479f);
  doorController_interleaveDoorTile(gb, sp0_);
  return;

substate1:
  CALL_C(0x479f, interactionDecCounter1_hook, 0x23cc, 0x47a2);
  if (!(F & FZ)) {
    CYCT(0x47a2, 0x47a3); ret_effect(gb); return;
  }
  CYC(0x47a2, 0x47a3);
  // Door will now close fully
  CYC(0x47a3, 0x47a6); push_effect(gb, 0x47a6); doorController_checkRespawnLink(gb);
  CYC(0x47a6, 0x47a9); push_effect(gb, 0x47a9); doorController_func_47f9(gb);
  CYC(0x47a9, 0x47ab); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x47ab, 0x47ac); A = mem_rd(gb, DE);
  CYC(0x47ac, 0x47af); SET_HL(doorControllerShutterTiles_bank08);
  CYC(0x47af, 0x47b0); push_effect(gb, 0x47b0); doorController_addAToHl(gb);
  CYC(0x47b0, 0x47b1); SET_HL(HL + 1);
  doorController_setTileAndPlaySound(gb, sp0_);
}

// INTERAC_DOOR_CONTROLLER: opens/closes shutter, key, boss and minecart doors under
// script control.
void interactionCode1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x46e4, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x46e7);
  CALL_C(0x46e7, returnIfScrollMode01Unset_hook, 0x26e4, 0x46ea);
  CYC(0x46ea, 0x46ed); A = mem_rd(gb, wSwitchHookState);
  CYC(0x46ed, 0x46ef); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x46ef, 0x46f0); ret_effect(gb); return;
  }
  CYC(0x46ef, 0x46f0);
  CYC(0x46f0, 0x46f2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x46f2, 0x46f3); A = mem_rd(gb, DE);
  CYC(0x46f3, 0x46f4); push_effect(gb, 0x46f4);
  switch (doorController_jumpTable(gb)) {
    case 0x46fc: doorController_state0(gb, sp0_); return;
    case 0x472c: doorController_state1(gb, sp0_); return;
    case 0x4737: doorController_state2(gb, sp0_); return;
    case 0x4789: doorController_state3(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
