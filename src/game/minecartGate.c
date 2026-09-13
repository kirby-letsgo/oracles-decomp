#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// 3 bytes per [var30] value: two collision bytes for the gate's tile pair, then the offset
// of the tile whose collision is set to $5e (closed) or $00 (open).
#define minecartGateCollisions_bank08 0x4ae7

#define OBJ_VAR30 0x30

#define SND_OPEN_GATE 0x7d

static uint16_t minecartGate_jumpTable(GB *gb) {
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

// interactionCode1b@setAnimationAndUpdateCollisions: sets var30 to the animation to play
// (bit 0 set if the gate is open) and updates the tile collisions to match. Reached by a
// `call` from state 0 and by a `jp` from states 1/2.
static void minecartGate_setAnimationAndUpdateCollisions(GB *gb, uint16_t sp0_) {
  CYC(0x4aab, 0x4aae); A = mem_rd(gb, wSwitchState);
  CYC(0x4aae, 0x4aaf); B = A;
  CYC(0x4aaf, 0x4ab1); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4ab1, 0x4ab2); A = mem_rd(gb, DE);
  CYC(0x4ab2, 0x4ab3); alu_and(gb, B);
  CYC(0x4ab3, 0x4ab5); C = 0x00;
  if (!(F & FZ)) {
    CYCT(0x4ab5, 0x4ab7);
  } else {
    CYC(0x4ab5, 0x4ab7);
    CYC(0x4ab7, 0x4ab9); C = 0x01;
  }
  CYC(0x4ab9, 0x4aba); E = alu_dec8(gb, E);
  CYC(0x4aba, 0x4abb); A = mem_rd(gb, DE); // subid: 0 facing left, 2 facing right
  CYC(0x4abb, 0x4abc); alu_or(gb, C);
  CYC(0x4abc, 0x4abe); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x4abe, 0x4abf); mem_wr(gb, DE, A);
  CALL_C(0x4abf, interactionSetAnimation_hook, 0x262e, 0x4ac2);
  CALL_C(0x4ac2, objectGetTileAtPosition_hook, 0x1444, 0x4ac5);
  CYC(0x4ac5, 0x4ac6); H = alu_dec8(gb, H); // h now points to wRoomCollisions
  CYC(0x4ac6, 0x4ac7); L = alu_dec8(gb, L);
  // a = [var30]*3
  CYC(0x4ac7, 0x4ac9); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x4ac9, 0x4aca); A = mem_rd(gb, DE);
  CYC(0x4aca, 0x4acb); B = A;
  CYC(0x4acb, 0x4acc); alu_add(gb, A);
  CYC(0x4acc, 0x4acd); alu_add(gb, B);
  CYC(0x4acd, 0x4ad0); SET_BC(minecartGateCollisions_bank08);
  CALL_C(0x4ad0, addAToBc_hook, 0x006d, 0x4ad3);
  CYC(0x4ad3, 0x4ad4); A = mem_rd(gb, BC);
  CYC(0x4ad4, 0x4ad5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ad5, 0x4ad6); SET_BC(BC + 1);
  CYC(0x4ad6, 0x4ad7); A = mem_rd(gb, BC);
  CYC(0x4ad7, 0x4ad8); mem_wr(gb, HL, A);
  CYC(0x4ad8, 0x4ad9); SET_BC(BC + 1);
  CYC(0x4ad9, 0x4ada); A = mem_rd(gb, BC);
  CYC(0x4ada, 0x4adb); alu_add(gb, L);
  CYC(0x4adb, 0x4adc); L = A;
  CYC(0x4adc, 0x4add); H = alu_inc8(gb, H);
  CYC(0x4add, 0x4ade); A = mem_rd(gb, DE);
  CYC(0x4ade, 0x4adf); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x4adf, 0x4ae1); goto open;
  }
  CYC(0x4adf, 0x4ae1);
  CYC(0x4ae1, 0x4ae3); mem_wr(gb, HL, 0x5e);
  CYC(0x4ae3, 0x4ae4); ret_effect(gb);
  return;
open:
  CYC(0x4ae4, 0x4ae6); mem_wr(gb, HL, 0x00);
  CYC(0x4ae6, 0x4ae7); ret_effect(gb);
}

// @state0: init. Subid bits 4-7 become the gate direction; bits 0-2 select the switch bit
// (var03) that controls it.
static void minecartGate_state0(GB *gb, uint16_t sp0_) {
  CYC(0x4a7b, 0x4a7d); A = 0x01;
  CYC(0x4a7d, 0x4a7e); mem_wr(gb, DE, A);
  CYC(0x4a7e, 0x4a80); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4a80, 0x4a81); A = mem_rd(gb, DE);
  CYC(0x4a81, 0x4a82); B = A;
  CYC(0x4a82, 0x4a84); A = alu_swap(gb, A);
  CYC(0x4a84, 0x4a86); alu_and(gb, 0x0f);
  CYC(0x4a86, 0x4a87); mem_wr(gb, DE, A);
  CYC(0x4a87, 0x4a88); A = B;
  CYC(0x4a88, 0x4a8a); alu_and(gb, 0x07);
  CYC(0x4a8a, 0x4a8d); SET_HL(bitTable);
  CYC(0x4a8d, 0x4a8e); alu_add(gb, L);
  CYC(0x4a8e, 0x4a8f); L = A;
  CYC(0x4a8f, 0x4a90); E = alu_inc8(gb, E);
  CYC(0x4a90, 0x4a91); A = mem_rd(gb, HL);
  CYC(0x4a91, 0x4a92); mem_wr(gb, DE, A);
  CALL_C(0x4a92, interactionInitGraphics_hook, 0x15fb, 0x4a95);
  CALL_C(0x4a95, objectSetVisible82_hook, 0x1e69, 0x4a98);
  CYC(0x4a98, 0x4a9b); push_effect(gb, 0x4a9b); minecartGate_setAnimationAndUpdateCollisions(gb, sp0_);
  CYC(0x4a9b, 0x4a9d); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x4a9d, 0x4a9e); A = mem_rd(gb, DE);
  CYC(0x4a9e, 0x4a9f); B = A;
  CYC(0x4a9f, 0x4aa1); alu_and(gb, 0x01);
  CYC(0x4aa1, 0x4aa2); A = alu_inc8(gb, A);
  CYC(0x4aa2, 0x4aa4); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4aa4, 0x4aa5); mem_wr(gb, DE, A);
  CYC(0x4aa5, 0x4aa6); A = B;
  CYC(0x4aa6, 0x4aa8); alu_xor(gb, 0x01);
  CYC(0x4aa8, 0x4aab); interactionSetAnimation_hook(gb);
}

// Shared tail of states 1 and 2: if the gate's switch bit matches (A was inverted for
// state 1), go to state 3 and start opening/closing.
static void minecartGate_checkSwitch(GB *gb, uint16_t sp0_) {
  CYC(0x4b02, 0x4b03); B = A;
  CYC(0x4b03, 0x4b05); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4b05, 0x4b06); A = mem_rd(gb, DE);
  CYC(0x4b06, 0x4b07); alu_and(gb, B);
  if (F & FZ) {
    CYCT(0x4b07, 0x4b08); ret_effect(gb); return;
  }
  CYC(0x4b07, 0x4b08);
  CYC(0x4b08, 0x4b0a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4b0a, 0x4b0c); A = 0x03;
  CYC(0x4b0c, 0x4b0d); mem_wr(gb, DE, A);
  CYC(0x4b0d, 0x4b0f); A = SND_OPEN_GATE;
  CALL_C(0x4b0f, playSound_b00_hook, 0x0c98, 0x4b12);
  CYC(0x4b12, 0x4b15);
  minecartGate_setAnimationAndUpdateCollisions(gb, sp0_);
}

// @state1: waiting for the switch to be pressed
static void minecartGate_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x4af3, objectSetPriorityRelativeToLink_hook, 0x22dc, 0x4af6);
  CYC(0x4af6, 0x4af9); A = mem_rd(gb, wSwitchState);
  CYC(0x4af9, 0x4afa); alu_cpl(gb);
  CYC(0x4afa, 0x4afc);
  minecartGate_checkSwitch(gb, sp0_);
}

// @state2: waiting for the switch to be released
static void minecartGate_state2(GB *gb, uint16_t sp0_) {
  CALL_C(0x4afc, objectSetPriorityRelativeToLink_hook, 0x22dc, 0x4aff);
  CYC(0x4aff, 0x4b02); A = mem_rd(gb, wSwitchState);
  minecartGate_checkSwitch(gb, sp0_);
}

// @state3: in the process of opening or closing
static void minecartGate_state3(GB *gb, uint16_t sp0_) {
  CALL_C(0x4b15, interactionAnimate_hook, 0x261b, 0x4b18);
  CALL_C(0x4b18, objectSetPriorityRelativeToLink_hook, 0x22dc, 0x4b1b);
  CYC(0x4b1b, 0x4b1d); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4b1d, 0x4b1e); A = mem_rd(gb, DE);
  CYC(0x4b1e, 0x4b1f); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4b1f, 0x4b20); ret_effect(gb); return;
  }
  CYC(0x4b1f, 0x4b20);
  CYC(0x4b20, 0x4b22); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x4b22, 0x4b23); A = mem_rd(gb, DE);
  CYC(0x4b23, 0x4b25); alu_and(gb, 0x01);
  CYC(0x4b25, 0x4b26); A = alu_inc8(gb, A);
  CYC(0x4b26, 0x4b28); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4b28, 0x4b29); mem_wr(gb, DE, A);
  CYC(0x4b29, 0x4b2a); ret_effect(gb);
}

// INTERAC_MINECART_GATE: a gate across the minecart track, opened/closed by a switch.
void interactionCode1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a6f, 0x4a71); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4a71, 0x4a72); A = mem_rd(gb, DE);
  CYC(0x4a72, 0x4a73); push_effect(gb, 0x4a73);
  switch (minecartGate_jumpTable(gb)) {
    case 0x4a7b: minecartGate_state0(gb, sp0_); return;
    case 0x4af3: minecartGate_state1(gb, sp0_); return;
    case 0x4afc: minecartGate_state2(gb, sp0_); return;
    case 0x4b15: minecartGate_state3(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
