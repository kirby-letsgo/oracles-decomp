#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define getLinkTilePosition_bank08 0x48f3

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
  CYC(0x5366, 0x5368); H = wRoomLayout >> 8;
  CYC(0x5368, 0x5369); A = mem_rd(gb, HL);
  CYC(0x5369, 0x536b); alu_cp(gb, 0x9f); // TILEINDEX_BLUE_FLOOR
  if (!(F & FZ)) {
    CYCT(0x536b, 0x536c); ret_effect(gb); return;
  }
  CYC(0x536b, 0x536c);
  CYC(0x536c, 0x536d); A = L;
  CYC(0x536d, 0x536f); H8(hFF8B) = A;
  CYC(0x536f, 0x5371); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(0x5371, 0x5372); mem_wr(gb, DE, A);
  CYC(0x5372, 0x5373); C = B;
  CYC(0x5373, 0x5375); A = 0x9d; // TILEINDEX_RED_FLOOR
  CALL_C(0x5375, setTile_hook, 0x3a9c, 0x5378);
  CYC(0x5378, 0x537a); A = H8(hFF8B);
  CYC(0x537a, 0x537b); C = A;
  CYC(0x537b, 0x537d); A = 0x9e; // TILEINDEX_YELLOW_FLOOR
  CALL_C(0x537d, setTile_hook, 0x3a9c, 0x5380);
  CYC(0x5380, 0x5382); A = 0x5e; // SND_GETSEED
  CYC(0x5382, 0x5385); playSound_b00_hook(gb);
}

// @state1: wait for Link to step onto an adjacent tile.
static void tileFiller_state1(GB *gb, uint16_t sp0_) {
  CYC(0x5343, 0x5346); SET_HL(getLinkTilePosition_bank08);
  CYC(0x5346, 0x5348); E = 0x08;
  CALL_C(0x5348, interBankCall_hook, 0x008a, 0x534b);
  CYC(0x534b, 0x534d); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(0x534d, 0x534e); A = mem_rd(gb, DE);
  CYC(0x534e, 0x534f); alu_cp(gb, L);
  if (F & FZ) {
    CYCT(0x534f, 0x5350); ret_effect(gb); return;
  }
  CYC(0x534f, 0x5350);
  // Check that the position changed by exactly one tile horizontally or vertically
  CYC(0x5350, 0x5351); B = A;
  CYC(0x5351, 0x5352); A = L;
  CYC(0x5352, 0x5354); alu_add(gb, 0xf0);
  CYC(0x5354, 0x5355); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x5355, 0x5357); tileFiller_updateFloor(gb, sp0_); return;
  }
  CYC(0x5355, 0x5357);
  CYC(0x5357, 0x5358); A = L;
  CYC(0x5358, 0x5359); A = alu_inc8(gb, A);
  CYC(0x5359, 0x535a); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x535a, 0x535c); tileFiller_updateFloor(gb, sp0_); return;
  }
  CYC(0x535a, 0x535c);
  CYC(0x535c, 0x535d); A = L;
  CYC(0x535d, 0x535f); alu_add(gb, 0x10);
  CYC(0x535f, 0x5360); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x5360, 0x5362); tileFiller_updateFloor(gb, sp0_); return;
  }
  CYC(0x5360, 0x5362);
  CYC(0x5362, 0x5363); A = L;
  CYC(0x5363, 0x5364); A = alu_dec8(gb, A);
  CYC(0x5364, 0x5365); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5365, 0x5366); ret_effect(gb); return;
  }
  CYC(0x5365, 0x5366);
  tileFiller_updateFloor(gb, sp0_);
}

// @state0: init. Turn the tile under the object yellow, spawn a puff, fall into state 1.
static void tileFiller_state0(GB *gb, uint16_t sp0_) {
  CYC(0x5327, 0x5329); A = 0x01;
  CYC(0x5329, 0x532a); mem_wr(gb, DE, A);
  CALL_C(0x532a, objectGetTileAtPosition_hook, 0x1444, 0x532d);
  CYC(0x532d, 0x532e); C = L;
  CYC(0x532e, 0x5330); A = 0x9e; // TILEINDEX_YELLOW_FLOOR
  CALL_C(0x5330, setTile_hook, 0x3a9c, 0x5333);
  CYC(0x5333, 0x5334); A = C;
  CYC(0x5334, 0x5336); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(0x5336, 0x5337); mem_wr(gb, DE, A);
  CALL_C(0x5337, getFreeInteractionSlot_hook, 0x3aef, 0x533a);
  if (!(F & FZ)) {
    CYCT(0x533a, 0x533c); tileFiller_state1(gb, sp0_); return;
  }
  CYC(0x533a, 0x533c);
  CYC(0x533c, 0x533e); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x533e, 0x5340); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(0x5340, setShortPosition_paramC_hook, 0x20b9, 0x5343);
  tileFiller_state1(gb, sp0_);
}

// INTERAC_TILE_FILLER: colours floor tiles as Link walks over them.
void interactionCode25_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x531c, returnIfScrollMode01Unset_hook, 0x26e4, 0x531f);
  CYC(0x531f, 0x5321); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5321, 0x5322); A = mem_rd(gb, DE);
  CYC(0x5322, 0x5323); push_effect(gb, 0x5323);
  switch (tileFiller_jumpTable(gb)) {
    case 0x5327: tileFiller_state0(gb, sp0_); return;
    case 0x5343: tileFiller_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
