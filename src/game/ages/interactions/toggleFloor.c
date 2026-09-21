#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define OBJ_VAR30 0x30

#define INTERAC_TOGGLE_FLOOR 0x15
#define TILEINDEX_RED_TOGGLE_FLOOR 0xad
#define SND_GETSEED 0x5e

// Returns in a and l the position of the tile Link is standing on.
void getLinkTilePosition_hook(GB *gb) {
  BASE(getLinkTilePosition);
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+4); A = W8(w1Link_yh);
  CYC(b_+4, b_+6); alu_add(gb, 0x05);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+12); A = W8(w1Link_xh);
  CYC(b_+12, b_+14); A = alu_swap(gb, A);
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
  CYC(b_+16, b_+17); alu_or(gb, B);
  CYC(b_+17, b_+18); L = A;
  CYC(b_+18, b_+19); SET_BC(pop_effect(gb));
  CYC(b_+19, b_+20); ret_effect(gb);
}

// @subid00: tracks Link's position and spawns a subid 1 instance when he jumps onto a
// colored floor tile.
static void toggleFloor_subid00(GB *gb, uint16_t sp0_) {
  BASE(interactionCode15);
  CALL_C(b_+7, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+10);
  CALL_C(b_+10, checkInteractionState_hook, SYM(checkInteractionState), b_+13);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15); goto initialized;
  }
  CYC(b_+13, b_+15);
  CALL_C(b_+15, interactionIncState_hook, SYM(interactionIncState), b_+18);

updateTilePos:
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+20, b_+23); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); ret_effect(gb);
  return;

initialized:
  CYC(b_+25, b_+28); A = mem_rd(gb, wLinkInAir);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+29, b_+31); goto updateTilePos;
  }
  CYC(b_+29, b_+31);
  // Link must be within 4 pixels of the tile's center on both axes
  CYC(b_+31, b_+34); A = W8(w1Link_yh);
  CYC(b_+34, b_+36); alu_add(gb, 0x05);
  CYC(b_+36, b_+38); alu_and(gb, 0x0f);
  CYC(b_+38, b_+40); alu_sub(gb, 0x04);
  CYC(b_+40, b_+42); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(b_+42, b_+43); ret_effect(gb); return;
  }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+46); A = W8(w1Link_xh);
  CYC(b_+46, b_+48); alu_and(gb, 0x0f);
  CYC(b_+48, b_+50); alu_sub(gb, 0x04);
  CYC(b_+50, b_+52); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(b_+52, b_+53); ret_effect(gb); return;
  }
  CYC(b_+52, b_+53);
  // Link's tile position must have changed
  CYC(b_+53, b_+55); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); C = A;
  CALL_C(b_+57, getLinkTilePosition_hook, SYM(getLinkTilePosition), b_+60);
  CYC(b_+60, b_+61); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+61, b_+62); ret_effect(gb); return;
  }
  CYC(b_+61, b_+62);
  // The new tile must be one of the colored floor tiles
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+64); C = A;
  CYC(b_+64, b_+66); B = wRoomLayout >> 8;
  CYC(b_+66, b_+67); A = mem_rd(gb, BC);
  CYC(b_+67, b_+69); alu_sub(gb, TILEINDEX_RED_TOGGLE_FLOOR);
  CYC(b_+69, b_+71); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+71, b_+72); ret_effect(gb); return;
  }
  CYC(b_+71, b_+72);
  // Spawn an instance of this object with subid 1
  CALL_C(b_+72, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+75);
  if (!(F & FZ)) {
    CYCT(b_+75, b_+76); ret_effect(gb); return;
  }
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+78); mem_wr(gb, HL, INTERAC_TOGGLE_FLOOR);
  CYC(b_+78, b_+79); L = alu_inc8(gb, L);
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x01);
  CYC(b_+81, b_+82); L = alu_inc8(gb, L);
  CYC(b_+82, b_+83); mem_wr(gb, HL, C);
  CYC(b_+83, b_+85); L = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+85, b_+88); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);
  CYC(b_+89, b_+90); ret_effect(gb);
}

// @subid01: toggles the tile at position [var03] once Link lands.
static void toggleFloor_subid01(GB *gb, uint16_t sp0_) {
  BASE(interactionCode15);
  CYC(b_+90, b_+93); A = mem_rd(gb, wLinkInAir);
  CYC(b_+93, b_+94); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+94, b_+95); ret_effect(gb); return;
  }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+97); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+99); C = A;
  // var30 holds Link's position from before the jump; landing on the same spot does not toggle
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  CYC(b_+102, b_+103); B = A;
  CALL_C(b_+103, getLinkTilePosition_hook, SYM(getLinkTilePosition), b_+106);
  CYC(b_+106, b_+107); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+107, b_+110); TAIL(interactionDelete);
  }
  CYC(b_+107, b_+110);
  CYC(b_+110, b_+112); B = wRoomLayout >> 8;
  CYC(b_+112, b_+113); A = mem_rd(gb, BC);
  CYC(b_+113, b_+114); A = alu_inc8(gb, A);
  CYC(b_+114, b_+116); alu_cp(gb, TILEINDEX_RED_TOGGLE_FLOOR + 3);
  if (F & FC) {
    CYCT(b_+116, b_+118); goto setTile;
  }
  CYC(b_+116, b_+118);
  CYC(b_+118, b_+120); A = TILEINDEX_RED_TOGGLE_FLOOR;
setTile:
  CYC(b_+120, b_+122); H8(hFF92) = A;
  CALL_C(b_+122, setTile_hook, SYM(setTile), b_+125);
  CYC(b_+125, b_+127); A = H8(hFF92);
  CYC(b_+127, b_+128); B = A;
  CALL_C(b_+128, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+131);
  CYC(b_+131, b_+133); A = SND_GETSEED;
  CALL_C(b_+133, playSound_b00_hook, SYM(playSound_b00), b_+136);
  CYC(b_+136, b_+139); interactionDelete_hook(gb);
}

// INTERAC_TOGGLE_FLOOR: red/yellow/blue floor tiles that change color when jumped over.
void interactionCode15_hook(GB *gb) {
  BASE(interactionCode15);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+7); toggleFloor_subid01(gb, sp0_); return;
  }
  CYC(b_+4, b_+7);
  toggleFloor_subid00(gb, sp0_);
}
