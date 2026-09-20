#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode25), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode25), (from), (to), true)

#define getLinkTilePosition_bank08 SYM(getLinkTilePosition)

static uint16_t tileFiller_jumpTable(GB *gb) {
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

// @updateFloor: L = Link's new tile position, B = the old one. If the new tile is blue
// floor, turn the old tile red and the new tile yellow.
static void tileFiller_updateFloor(GB *gb, uint16_t sp0_) {
  BASE(interactionCode25);
  CYC(b_+74, b_+76); H = wRoomLayout >> 8;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_cp(gb, 0x9f); // TILEINDEX_BLUE_FLOOR
  if (!(F & FZ)) {
    CYCT(b_+79, b_+80); ret_effect(gb); return;
  }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+81); A = L;
  CYC(b_+81, b_+83); H8(hFF8B) = A;
  CYC(b_+83, b_+85); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); C = B;
  CYC(b_+87, b_+89); A = 0x9d; // TILEINDEX_RED_FLOOR
  CALL_C(b_+89, setTile_hook, SYM(setTile), b_+92);
  CYC(b_+92, b_+94); A = H8(hFF8B);
  CYC(b_+94, b_+95); C = A;
  CYC(b_+95, b_+97); A = 0x9e; // TILEINDEX_YELLOW_FLOOR
  CALL_C(b_+97, setTile_hook, SYM(setTile), b_+100);
  CYC(b_+100, b_+102); A = 0x5e; // SND_GETSEED
  CYC(b_+102, SYM(interactionCode28)); playSound_b00_hook(gb);
}

// @state1: wait for Link to step onto an adjacent tile.
static void tileFiller_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode25);
  CYC(b_+39, b_+42); SET_HL(getLinkTilePosition_bank08);
  CYC(b_+42, b_+44); E = 0x08;
  CALL_C(b_+44, interBankCall_hook, 0x008a, b_+47);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_cp(gb, L);
  if (F & FZ) {
    CYCT(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+51, b_+52);
  // Check that the position changed by exactly one tile horizontally or vertically
  CYC(b_+52, b_+53); B = A;
  CYC(b_+53, b_+54); A = L;
  CYC(b_+54, b_+56); alu_add(gb, 0xf0);
  CYC(b_+56, b_+57); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+57, b_+59); tileFiller_updateFloor(gb, sp0_); return;
  }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+60); A = L;
  CYC(b_+60, b_+61); A = alu_inc8(gb, A);
  CYC(b_+61, b_+62); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+62, b_+64); tileFiller_updateFloor(gb, sp0_); return;
  }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+65); A = L;
  CYC(b_+65, b_+67); alu_add(gb, 0x10);
  CYC(b_+67, b_+68); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+68, b_+70); tileFiller_updateFloor(gb, sp0_); return;
  }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); A = L;
  CYC(b_+71, b_+72); A = alu_dec8(gb, A);
  CYC(b_+72, b_+73); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+73, b_+74); ret_effect(gb); return;
  }
  CYC(b_+73, b_+74);
  tileFiller_updateFloor(gb, sp0_);
}

// @state0: init. Turn the tile under the object yellow, spawn a puff, fall into state 1.
static void tileFiller_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode25);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CALL_C(b_+14, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+17);
  CYC(b_+17, b_+18); C = L;
  CYC(b_+18, b_+20); A = 0x9e; // TILEINDEX_YELLOW_FLOOR
  CALL_C(b_+20, setTile_hook, SYM(setTile), b_+23);
  CYC(b_+23, b_+24); A = C;
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+30);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+32); tileFiller_state1(gb, sp0_); return;
  }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+36, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+39);
  tileFiller_state1(gb, sp0_);
}

// INTERAC_TILE_FILLER: colours floor tiles as Link walks over them.
void interactionCode25_hook(GB *gb) {
  BASE(interactionCode25);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (tileFiller_jumpTable(gb));
    if (jt_ == b_+11) { tileFiller_state0(gb, sp0_); return; }
    else if (jt_ == b_+39) { tileFiller_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
