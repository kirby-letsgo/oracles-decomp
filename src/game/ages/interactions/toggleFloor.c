#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define OBJ_VAR30 0x30

#define INTERAC_TOGGLE_FLOOR 0x15
#define TILEINDEX_RED_TOGGLE_FLOOR 0xad
#define SND_GETSEED 0x5e

// Returns in a and l the position of the tile Link is standing on.
void getLinkTilePosition_hook(GB *gb) {
  CYC(0x48f3, 0x48f4); push_effect(gb, BC);
  CYC(0x48f4, 0x48f7); A = W8(w1Link_yh);
  CYC(0x48f7, 0x48f9); alu_add(gb, 0x05);
  CYC(0x48f9, 0x48fb); alu_and(gb, 0xf0);
  CYC(0x48fb, 0x48fc); B = A;
  CYC(0x48fc, 0x48ff); A = W8(w1Link_xh);
  CYC(0x48ff, 0x4901); A = alu_swap(gb, A);
  CYC(0x4901, 0x4903); alu_and(gb, 0x0f);
  CYC(0x4903, 0x4904); alu_or(gb, B);
  CYC(0x4904, 0x4905); L = A;
  CYC(0x4905, 0x4906); SET_BC(pop_effect(gb));
  CYC(0x4906, 0x4907); ret_effect(gb);
}

// @subid00: tracks Link's position and spawns a subid 1 instance when he jumps onto a
// colored floor tile.
static void toggleFloor_subid00(GB *gb, uint16_t sp0_) {
  CALL_C(0x486f, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4872);
  CALL_C(0x4872, checkInteractionState_hook, 0x23fe, 0x4875);
  if (!(F & FZ)) {
    CYCT(0x4875, 0x4877); goto initialized;
  }
  CYC(0x4875, 0x4877);
  CALL_C(0x4877, interactionIncState_hook, 0x23e0, 0x487a);

updateTilePos:
  CYC(0x487a, 0x487c); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x487c, 0x487f); A = mem_rd(gb, wActiveTilePos);
  CYC(0x487f, 0x4880); mem_wr(gb, DE, A);
  CYC(0x4880, 0x4881); ret_effect(gb);
  return;

initialized:
  CYC(0x4881, 0x4884); A = mem_rd(gb, wLinkInAir);
  CYC(0x4884, 0x4885); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4885, 0x4887); goto updateTilePos;
  }
  CYC(0x4885, 0x4887);
  // Link must be within 4 pixels of the tile's center on both axes
  CYC(0x4887, 0x488a); A = W8(w1Link_yh);
  CYC(0x488a, 0x488c); alu_add(gb, 0x05);
  CYC(0x488c, 0x488e); alu_and(gb, 0x0f);
  CYC(0x488e, 0x4890); alu_sub(gb, 0x04);
  CYC(0x4890, 0x4892); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(0x4892, 0x4893); ret_effect(gb); return;
  }
  CYC(0x4892, 0x4893);
  CYC(0x4893, 0x4896); A = W8(w1Link_xh);
  CYC(0x4896, 0x4898); alu_and(gb, 0x0f);
  CYC(0x4898, 0x489a); alu_sub(gb, 0x04);
  CYC(0x489a, 0x489c); alu_cp(gb, 0x09);
  if (!(F & FC)) {
    CYCT(0x489c, 0x489d); ret_effect(gb); return;
  }
  CYC(0x489c, 0x489d);
  // Link's tile position must have changed
  CYC(0x489d, 0x489f); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x489f, 0x48a0); A = mem_rd(gb, DE);
  CYC(0x48a0, 0x48a1); C = A;
  CALL_C(0x48a1, getLinkTilePosition_hook, 0x48f3, 0x48a4);
  CYC(0x48a4, 0x48a5); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x48a5, 0x48a6); ret_effect(gb); return;
  }
  CYC(0x48a5, 0x48a6);
  // The new tile must be one of the colored floor tiles
  CYC(0x48a6, 0x48a7); mem_wr(gb, DE, A);
  CYC(0x48a7, 0x48a8); C = A;
  CYC(0x48a8, 0x48aa); B = wRoomLayout >> 8;
  CYC(0x48aa, 0x48ab); A = mem_rd(gb, BC);
  CYC(0x48ab, 0x48ad); alu_sub(gb, TILEINDEX_RED_TOGGLE_FLOOR);
  CYC(0x48ad, 0x48af); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x48af, 0x48b0); ret_effect(gb); return;
  }
  CYC(0x48af, 0x48b0);
  // Spawn an instance of this object with subid 1
  CALL_C(0x48b0, getFreeInteractionSlot_hook, 0x3aef, 0x48b3);
  if (!(F & FZ)) {
    CYCT(0x48b3, 0x48b4); ret_effect(gb); return;
  }
  CYC(0x48b3, 0x48b4);
  CYC(0x48b4, 0x48b6); mem_wr(gb, HL, INTERAC_TOGGLE_FLOOR);
  CYC(0x48b6, 0x48b7); L = alu_inc8(gb, L);
  CYC(0x48b7, 0x48b9); mem_wr(gb, HL, 0x01);
  CYC(0x48b9, 0x48ba); L = alu_inc8(gb, L);
  CYC(0x48ba, 0x48bb); mem_wr(gb, HL, C);
  CYC(0x48bb, 0x48bd); L = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x48bd, 0x48c0); A = mem_rd(gb, wActiveTilePos);
  CYC(0x48c0, 0x48c1); mem_wr(gb, HL, A);
  CYC(0x48c1, 0x48c2); ret_effect(gb);
}

// @subid01: toggles the tile at position [var03] once Link lands.
static void toggleFloor_subid01(GB *gb, uint16_t sp0_) {
  CYC(0x48c2, 0x48c5); A = mem_rd(gb, wLinkInAir);
  CYC(0x48c5, 0x48c6); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x48c6, 0x48c7); ret_effect(gb); return;
  }
  CYC(0x48c6, 0x48c7);
  CYC(0x48c7, 0x48c9); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x48c9, 0x48ca); A = mem_rd(gb, DE);
  CYC(0x48ca, 0x48cb); C = A;
  // var30 holds Link's position from before the jump; landing on the same spot does not toggle
  CYC(0x48cb, 0x48cd); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x48cd, 0x48ce); A = mem_rd(gb, DE);
  CYC(0x48ce, 0x48cf); B = A;
  CALL_C(0x48cf, getLinkTilePosition_hook, 0x48f3, 0x48d2);
  CYC(0x48d2, 0x48d3); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x48d3, 0x48d6); interactionDelete_hook(gb); return;
  }
  CYC(0x48d3, 0x48d6);
  CYC(0x48d6, 0x48d8); B = wRoomLayout >> 8;
  CYC(0x48d8, 0x48d9); A = mem_rd(gb, BC);
  CYC(0x48d9, 0x48da); A = alu_inc8(gb, A);
  CYC(0x48da, 0x48dc); alu_cp(gb, TILEINDEX_RED_TOGGLE_FLOOR + 3);
  if (F & FC) {
    CYCT(0x48dc, 0x48de); goto setTile;
  }
  CYC(0x48dc, 0x48de);
  CYC(0x48de, 0x48e0); A = TILEINDEX_RED_TOGGLE_FLOOR;
setTile:
  CYC(0x48e0, 0x48e2); H8(hFF92) = A;
  CALL_C(0x48e2, setTile_hook, 0x3a9c, 0x48e5);
  CYC(0x48e5, 0x48e7); A = H8(hFF92);
  CYC(0x48e7, 0x48e8); B = A;
  CALL_C(0x48e8, setTileInRoomLayoutBuffer_hook, 0x1426, 0x48eb);
  CYC(0x48eb, 0x48ed); A = SND_GETSEED;
  CALL_C(0x48ed, playSound_b00_hook, 0x0c98, 0x48f0);
  CYC(0x48f0, 0x48f3); interactionDelete_hook(gb);
}

// INTERAC_TOGGLE_FLOOR: red/yellow/blue floor tiles that change color when jumped over.
void interactionCode15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4868, 0x486a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x486a, 0x486b); A = mem_rd(gb, DE);
  CYC(0x486b, 0x486c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x486c, 0x486f); toggleFloor_subid01(gb, sp0_); return;
  }
  CYC(0x486c, 0x486f);
  toggleFloor_subid00(gb, sp0_);
}
