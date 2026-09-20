#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// roomInitialization.generateRandomBuffer, called through interBankCall (bank $02).
#define generateRandomBuffer_bank02 SYM(generateRandomBuffer_b02)

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
  BASE(interactionCode22);
  CYC(b_+150, b_+152); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+152, b_+153); A = mem_rd(gb, DE);
  CYC(b_+153, b_+156); SET_HL(wBigBuffer);
  CYC(b_+156, b_+157); push_effect(gb, b_+157); floorColorChanger_addAToHl(gb);
  CYC(b_+157, b_+159); A = H8(hFF8C);
  CYC(b_+159, b_+160); C = A;
  CYC(b_+160, b_+161); A = mem_rd(gb, HL);
  // Position must be in-bounds and not this object's position
  CYC(b_+161, b_+163); alu_cp(gb, 0x9f); // LARGE_ROOM_HEIGHT*16 - 17
  if (!(F & FC)) {
    CYCT(b_+163, b_+165); goto decCounter1;
  }
  CYC(b_+163, b_+165);
  CYC(b_+165, b_+166); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+166, b_+168); goto decCounter1;
  }
  CYC(b_+166, b_+168);
  // Position can't be on the screen edge
  CYC(b_+168, b_+170); alu_and(gb, 0x0f);
  if (F & FZ) {
    CYCT(b_+170, b_+172); goto decCounter1;
  }
  CYC(b_+170, b_+172);
  CYC(b_+172, b_+173); A = mem_rd(gb, HL);
  CYC(b_+173, b_+175); alu_and(gb, 0xf0);
  if (F & FZ) {
    CYCT(b_+175, b_+177); goto decCounter1;
  }
  CYC(b_+175, b_+177);
  CYC(b_+177, b_+179); alu_cp(gb, 0xa0); // LARGE_ROOM_HEIGHT*16 - 16
  if (F & FZ) {
    CYCT(b_+179, b_+181); goto decCounter1;
  }
  CYC(b_+179, b_+181);
  // Check if this is a tile that should be replaced
  CYC(b_+181, b_+182); A = mem_rd(gb, HL);
  CYC(b_+182, b_+183); L = A;
  CYC(b_+183, b_+185); H = wRoomLayout >> 8;
  CYC(b_+185, b_+186); A = mem_rd(gb, HL);
  CYC(b_+186, b_+188); alu_sub(gb, 0x9d); // TILEINDEX_RED_FLOOR
  CYC(b_+188, b_+190); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+190, b_+192); goto notColoredFloor;
  }
  CYC(b_+190, b_+192);
  // Replace the tile
  CYC(b_+192, b_+194); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+194, b_+195); A = mem_rd(gb, DE);
  CYC(b_+195, b_+196); C = L;
  CALL_C(b_+196, setTile_hook, SYM(setTile), b_+199);
decCounter1:
  CYC(b_+199, b_+202); interactionDecCounter1_hook(gb);
  return;

notColoredFloor:
  // Not a colored floor: still change the tile "underneath" it in w3RoomLayoutBuffer,
  // in case it's pushable or something.
  CYC(b_+202, b_+204); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+204, b_+205); A = mem_rd(gb, DE);
  CYC(b_+205, b_+206); B = A;
  CYC(b_+206, b_+207); C = L;
  CALL_C(b_+207, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+210);
  CYC(b_+210, b_+213); interactionDecCounter1_hook(gb);
}

// @subid0: the "controller"; detects when the tile under it has changed color and spawns
// a subid 1 object to convert the rest of the room.
static void floorColorChanger_subid0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode22);
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13); goto initialized;
  }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, interactionIncState_hook, SYM(interactionIncState), b_+22);

initialized:
  // Check if the tile changed color
  CALL_C(b_+22, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+29, b_+30); ret_effect(gb); return;
  }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+33, b_+35); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+35, b_+36); ret_effect(gb); return;
  }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+40); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+40, b_+42); alu_add(gb, 0x9d); // TILEINDEX_RED_FLOOR
  CYC(b_+42, b_+43); B = A;
  CALL_C(b_+43, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+46);
  if (!(F & FZ)) {
    CYCT(b_+46, b_+47); ret_effect(gb); return;
  }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x22); // INTERAC_FLOOR_COLOR_CHANGER
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x01);
  // Set var03 to the tile index to convert tiles to
  CYC(b_+52, b_+54); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+54, b_+55); mem_wr(gb, HL, B);
  CYC(b_+55, b_+58); objectCopyPosition_hook(gb);
}

// @subid1: performs the updates to all tiles in the room in a random order.
static void floorColorChanger_subid1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode22);
  CALL_C(b_+58, checkInteractionState_hook, SYM(checkInteractionState), b_+61);
  if (!(F & FZ)) {
    CYCT(b_+61, b_+63); goto initialized;
  }
  CYC(b_+61, b_+63);
  CALL_C(b_+63, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+66);
  CYC(b_+66, b_+68); E = INTERACTION_BASE + 0x30; // var30
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CALL_C(b_+69, interactionIncState_hook, SYM(interactionIncState), b_+72);
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0xff);
  // Generate all values from $00-$ff in a random order, and copy them to wBigBuffer.
  CYC(b_+76, b_+79); SET_HL(generateRandomBuffer_bank02);
  CYC(b_+79, b_+81); E = 0x02;
  CALL_C(b_+81, interBankCall_hook, 0x008a, b_+84);
  CYC(b_+84, b_+86); A = 0x04; // :w4RandomBuffer
  CYC(b_+86, b_+88); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(b_+88, b_+91); SET_HL(w4RandomBuffer);
  CYC(b_+91, b_+94); SET_DE(wBigBuffer);
  CYC(b_+94, b_+96); B = 0x00;
  CALL_C(b_+96, copyMemory_hook, SYM(copyMemory), b_+99);
  CYC(b_+99, b_+101); A = 0x01;
  CYC(b_+101, b_+103); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(b_+103, b_+105); A = H8(hActiveObject);
  CYC(b_+105, b_+106); D = A;

initialized:
  CALL_C(b_+106, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+109);
  CYC(b_+109, b_+111); E = INTERACTION_BASE + 0x30; // var30
  CYC(b_+111, b_+112); A = mem_rd(gb, DE);
  CYC(b_+112, b_+113); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+113, b_+116); goto convert;
  }
  CYC(b_+113, b_+116);
  CYC(b_+116, b_+117); A = mem_rd(gb, HL);
  CYC(b_+117, b_+119); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (!(F & FZ)) {
    CYCT(b_+119, b_+122); interactionDelete_hook(gb); return;
  }
  CYC(b_+119, b_+122);

convert:
  CYC(b_+122, b_+123); A = L;
  CYC(b_+123, b_+125); H8(hFF8C) = A;
  CYC(b_+125, b_+128); push_effect(gb, b_+128); floorColorChanger_convertNextTile(gb, sp0_);
  if (F & FZ) {
    CYCT(b_+128, b_+130); goto done;
  }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+133); push_effect(gb, b_+133); floorColorChanger_convertNextTile(gb, sp0_);
  if (F & FZ) {
    CYCT(b_+133, b_+135); goto done;
  }
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+138); push_effect(gb, b_+138); floorColorChanger_convertNextTile(gb, sp0_);
  if (F & FZ) {
    CYCT(b_+138, b_+140); goto done;
  }
  CYC(b_+138, b_+140);
  CYC(b_+140, b_+143); push_effect(gb, b_+143); floorColorChanger_convertNextTile(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+143, b_+144); ret_effect(gb); return;
  }
  CYC(b_+143, b_+144);

done:
  CYC(b_+144, b_+147); push_effect(gb, b_+147); floorColorChanger_convertNextTile(gb, sp0_);
  CYC(b_+147, b_+150); interactionDelete_hook(gb);
}

// INTERAC_FLOOR_COLOR_CHANGER: recolors the toggle floor of a room when one tile changes.
void interactionCode22_hook(GB *gb) {
  BASE(interactionCode22);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (floorColorChanger_jumpTable(gb));
    if (jt_ == b_+8) { floorColorChanger_subid0(gb, sp0_); return; }
    else if (jt_ == b_+58) { floorColorChanger_subid1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
