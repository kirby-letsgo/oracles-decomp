#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// roomInitialization.generateRandomBuffer, called through interBankCall (bank $02).
#define generateRandomBuffer_bank02 0x7823

static uint16_t floorColorChanger_jumpTable(GB *gb) {
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

static void floorColorChanger_addAToHl(GB *gb) {
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

// interactionCode22@convertNextTile: converts the tile at the next random position in
// wBigBuffer (indexed by counter1). Sets zflag when the last tile has been converted.
// Reached only by static `call`s from interactionCode22 itself.
static void floorColorChanger_convertNextTile(GB *gb, uint16_t sp0_) {
  CYC(0x514f, 0x5151); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5151, 0x5152); A = mem_rd(gb, DE);
  CYC(0x5152, 0x5155); SET_HL(wBigBuffer);
  CYC(0x5155, 0x5156); push_effect(gb, 0x5156); floorColorChanger_addAToHl(gb);
  CYC(0x5156, 0x5158); A = H8(hFF8C);
  CYC(0x5158, 0x5159); C = A;
  CYC(0x5159, 0x515a); A = mem_rd(gb, HL);
  // Position must be in-bounds and not this object's position
  CYC(0x515a, 0x515c); alu_cp(gb, 0x9f); // LARGE_ROOM_HEIGHT*16 - 17
  if (!(F & FC)) {
    CYCT(0x515c, 0x515e); goto decCounter1;
  }
  CYC(0x515c, 0x515e);
  CYC(0x515e, 0x515f); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x515f, 0x5161); goto decCounter1;
  }
  CYC(0x515f, 0x5161);
  // Position can't be on the screen edge
  CYC(0x5161, 0x5163); alu_and(gb, 0x0f);
  if (F & FZ) {
    CYCT(0x5163, 0x5165); goto decCounter1;
  }
  CYC(0x5163, 0x5165);
  CYC(0x5165, 0x5166); A = mem_rd(gb, HL);
  CYC(0x5166, 0x5168); alu_and(gb, 0xf0);
  if (F & FZ) {
    CYCT(0x5168, 0x516a); goto decCounter1;
  }
  CYC(0x5168, 0x516a);
  CYC(0x516a, 0x516c); alu_cp(gb, 0xa0); // LARGE_ROOM_HEIGHT*16 - 16
  if (F & FZ) {
    CYCT(0x516c, 0x516e); goto decCounter1;
  }
  CYC(0x516c, 0x516e);
  // Check if this is a tile that should be replaced
  CYC(0x516e, 0x516f); A = mem_rd(gb, HL);
  CYC(0x516f, 0x5170); L = A;
  CYC(0x5170, 0x5172); H = wRoomLayout >> 8;
  CYC(0x5172, 0x5173); A = mem_rd(gb, HL);
  CYC(0x5173, 0x5175); alu_sub(gb, 0x9d); // TILEINDEX_RED_FLOOR
  CYC(0x5175, 0x5177); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x5177, 0x5179); goto notColoredFloor;
  }
  CYC(0x5177, 0x5179);
  // Replace the tile
  CYC(0x5179, 0x517b); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x517b, 0x517c); A = mem_rd(gb, DE);
  CYC(0x517c, 0x517d); C = L;
  CALL_C(0x517d, setTile_hook, 0x3a9c, 0x5180);
decCounter1:
  CYC(0x5180, 0x5183); interactionDecCounter1_hook(gb);
  return;

notColoredFloor:
  // Not a colored floor: still change the tile "underneath" it in w3RoomLayoutBuffer,
  // in case it's pushable or something.
  CYC(0x5183, 0x5185); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5185, 0x5186); A = mem_rd(gb, DE);
  CYC(0x5186, 0x5187); B = A;
  CYC(0x5187, 0x5188); C = L;
  CALL_C(0x5188, setTileInRoomLayoutBuffer_hook, 0x1426, 0x518b);
  CYC(0x518b, 0x518e); interactionDecCounter1_hook(gb);
}

// @subid0: the "controller"; detects when the tile under it has changed color and spawns
// a subid 1 object to convert the rest of the room.
static void floorColorChanger_subid0(GB *gb, uint16_t sp0_) {
  CALL_C(0x50c1, checkInteractionState_hook, 0x23fe, 0x50c4);
  if (!(F & FZ)) {
    CYCT(0x50c4, 0x50c6); goto initialized;
  }
  CYC(0x50c4, 0x50c6);
  CALL_C(0x50c6, objectGetTileAtPosition_hook, 0x1444, 0x50c9);
  CYC(0x50c9, 0x50cb); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x50cb, 0x50cc); mem_wr(gb, DE, A);
  CALL_C(0x50cc, interactionIncState_hook, 0x23e0, 0x50cf);

initialized:
  // Check if the tile changed color
  CALL_C(0x50cf, objectGetTileAtPosition_hook, 0x1444, 0x50d2);
  CYC(0x50d2, 0x50d4); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x50d4, 0x50d5); A = mem_rd(gb, DE);
  CYC(0x50d5, 0x50d6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x50d6, 0x50d7); ret_effect(gb); return;
  }
  CYC(0x50d6, 0x50d7);
  CYC(0x50d7, 0x50d8); A = mem_rd(gb, HL);
  CYC(0x50d8, 0x50da); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x50da, 0x50dc); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x50dc, 0x50dd); ret_effect(gb); return;
  }
  CYC(0x50dc, 0x50dd);
  CYC(0x50dd, 0x50de); A = mem_rd(gb, HL);
  CYC(0x50de, 0x50df); mem_wr(gb, DE, A);
  CYC(0x50df, 0x50e1); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x50e1, 0x50e3); alu_add(gb, 0x9d); // TILEINDEX_RED_FLOOR
  CYC(0x50e3, 0x50e4); B = A;
  CALL_C(0x50e4, getFreeInteractionSlot_hook, 0x3aef, 0x50e7);
  if (!(F & FZ)) {
    CYCT(0x50e7, 0x50e8); ret_effect(gb); return;
  }
  CYC(0x50e7, 0x50e8);
  CYC(0x50e8, 0x50ea); mem_wr(gb, HL, 0x22); // INTERAC_FLOOR_COLOR_CHANGER
  CYC(0x50ea, 0x50eb); L = alu_inc8(gb, L);
  CYC(0x50eb, 0x50ed); mem_wr(gb, HL, 0x01);
  // Set var03 to the tile index to convert tiles to
  CYC(0x50ed, 0x50ef); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x50ef, 0x50f0); mem_wr(gb, HL, B);
  CYC(0x50f0, 0x50f3); objectCopyPosition_hook(gb);
}

// @subid1: performs the updates to all tiles in the room in a random order.
static void floorColorChanger_subid1(GB *gb, uint16_t sp0_) {
  CALL_C(0x50f3, checkInteractionState_hook, 0x23fe, 0x50f6);
  if (!(F & FZ)) {
    CYCT(0x50f6, 0x50f8); goto initialized;
  }
  CYC(0x50f6, 0x50f8);
  CALL_C(0x50f8, objectGetTileAtPosition_hook, 0x1444, 0x50fb);
  CYC(0x50fb, 0x50fd); E = INTERACTION_BASE + 0x30; // var30
  CYC(0x50fd, 0x50fe); mem_wr(gb, DE, A);
  CALL_C(0x50fe, interactionIncState_hook, 0x23e0, 0x5101);
  CYC(0x5101, 0x5103); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5103, 0x5105); mem_wr(gb, HL, 0xff);
  // Generate all values from $00-$ff in a random order, and copy them to wBigBuffer.
  CYC(0x5105, 0x5108); SET_HL(generateRandomBuffer_bank02);
  CYC(0x5108, 0x510a); E = 0x02;
  CALL_C(0x510a, interBankCall_hook, 0x008a, 0x510d);
  CYC(0x510d, 0x510f); A = 0x04; // :w4RandomBuffer
  CYC(0x510f, 0x5111); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(0x5111, 0x5114); SET_HL(w4RandomBuffer);
  CYC(0x5114, 0x5117); SET_DE(wBigBuffer);
  CYC(0x5117, 0x5119); B = 0x00;
  CALL_C(0x5119, copyMemory_hook, 0x0486, 0x511c);
  CYC(0x511c, 0x511e); A = 0x01;
  CYC(0x511e, 0x5120); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(0x5120, 0x5122); A = H8(hActiveObject);
  CYC(0x5122, 0x5123); D = A;

initialized:
  CALL_C(0x5123, objectGetTileAtPosition_hook, 0x1444, 0x5126);
  CYC(0x5126, 0x5128); E = INTERACTION_BASE + 0x30; // var30
  CYC(0x5128, 0x5129); A = mem_rd(gb, DE);
  CYC(0x5129, 0x512a); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x512a, 0x512d); goto convert;
  }
  CYC(0x512a, 0x512d);
  CYC(0x512d, 0x512e); A = mem_rd(gb, HL);
  CYC(0x512e, 0x5130); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (!(F & FZ)) {
    CYCT(0x5130, 0x5133); interactionDelete_hook(gb); return;
  }
  CYC(0x5130, 0x5133);

convert:
  CYC(0x5133, 0x5134); A = L;
  CYC(0x5134, 0x5136); H8(hFF8C) = A;
  CYC(0x5136, 0x5139); push_effect(gb, 0x5139); floorColorChanger_convertNextTile(gb, sp0_);
  if (F & FZ) {
    CYCT(0x5139, 0x513b); goto done;
  }
  CYC(0x5139, 0x513b);
  CYC(0x513b, 0x513e); push_effect(gb, 0x513e); floorColorChanger_convertNextTile(gb, sp0_);
  if (F & FZ) {
    CYCT(0x513e, 0x5140); goto done;
  }
  CYC(0x513e, 0x5140);
  CYC(0x5140, 0x5143); push_effect(gb, 0x5143); floorColorChanger_convertNextTile(gb, sp0_);
  if (F & FZ) {
    CYCT(0x5143, 0x5145); goto done;
  }
  CYC(0x5143, 0x5145);
  CYC(0x5145, 0x5148); push_effect(gb, 0x5148); floorColorChanger_convertNextTile(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x5148, 0x5149); ret_effect(gb); return;
  }
  CYC(0x5148, 0x5149);

done:
  CYC(0x5149, 0x514c); push_effect(gb, 0x514c); floorColorChanger_convertNextTile(gb, sp0_);
  CYC(0x514c, 0x514f); interactionDelete_hook(gb);
}

// INTERAC_FLOOR_COLOR_CHANGER: recolors the toggle floor of a room when one tile changes.
void interactionCode22_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50b9, 0x50bb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x50bb, 0x50bc); A = mem_rd(gb, DE);
  CYC(0x50bc, 0x50bd); push_effect(gb, 0x50bd);
  switch (floorColorChanger_jumpTable(gb)) {
    case 0x50c1: floorColorChanger_subid0(gb, sp0_); return;
    case 0x50f3: floorColorChanger_subid1(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
