#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode1b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode1b), (from), (to), true)

// 3 bytes per [var30] value: two collision bytes for the gate's tile pair, then the offset
// of the tile whose collision is set to $5e (closed) or $00 (open).
#define minecartGateCollisions_bank08 SYM(interactionCode1b__collisions)

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
  BASE(interactionCode1b);
  CYC(b_+60, b_+63); A = mem_rd(gb, wSwitchState);
  CYC(b_+63, b_+64); B = A;
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); alu_and(gb, B);
  CYC(b_+68, b_+70); C = 0x00;
  if (!(F & FZ)) {
    CYCT(b_+70, b_+72);
  } else {
    CYC(b_+70, b_+72);
    CYC(b_+72, b_+74); C = 0x01;
  }
  CYC(b_+74, b_+75); E = alu_dec8(gb, E);
  CYC(b_+75, b_+76); A = mem_rd(gb, DE); // subid: 0 facing left, 2 facing right
  CYC(b_+76, b_+77); alu_or(gb, C);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CALL_C(b_+80, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+83);
  CALL_C(b_+83, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+86);
  CYC(b_+86, b_+87); H = alu_dec8(gb, H); // h now points to wRoomCollisions
  CYC(b_+87, b_+88); L = alu_dec8(gb, L);
  // a = [var30]*3
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); B = A;
  CYC(b_+92, b_+93); alu_add(gb, A);
  CYC(b_+93, b_+94); alu_add(gb, B);
  CYC(b_+94, b_+97); SET_BC(minecartGateCollisions_bank08);
  CALL_C(b_+97, addAToBc_hook, 0x006d, b_+100);
  CYC(b_+100, b_+101); A = mem_rd(gb, BC);
  CYC(b_+101, b_+102); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+102, b_+103); SET_BC(BC + 1);
  CYC(b_+103, b_+104); A = mem_rd(gb, BC);
  CYC(b_+104, b_+105); mem_wr(gb, HL, A);
  CYC(b_+105, b_+106); SET_BC(BC + 1);
  CYC(b_+106, b_+107); A = mem_rd(gb, BC);
  CYC(b_+107, b_+108); alu_add(gb, L);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+110); H = alu_inc8(gb, H);
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+112, b_+114); goto open;
  }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x5e);
  CYC(b_+116, b_+117); ret_effect(gb);
  return;
open:
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x00);
  CYC(b_+119, b_+120); ret_effect(gb);
}

// @state0: init. Subid bits 4-7 become the gate direction; bits 0-2 select the switch bit
// (var03) that controls it.
static void minecartGate_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1b);
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, b_+23); alu_and(gb, 0x0f);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+27); alu_and(gb, 0x07);
  CYC(b_+27, b_+30); SET_HL(bitTable);
  CYC(b_+30, b_+31); alu_add(gb, L);
  CYC(b_+31, b_+32); L = A;
  CYC(b_+32, b_+33); E = alu_inc8(gb, E);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CALL_C(b_+35, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+38);
  CALL_C(b_+38, objectSetVisible82_hook, SYM(objectSetVisible82), b_+41);
  CYC(b_+41, b_+44); push_effect(gb, b_+44); minecartGate_setAnimationAndUpdateCollisions(gb, sp0_);
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); B = A;
  CYC(b_+48, b_+50); alu_and(gb, 0x01);
  CYC(b_+50, b_+51); A = alu_inc8(gb, A);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); A = B;
  CYC(b_+55, b_+57); alu_xor(gb, 0x01);
  CYC(b_+57, b_+60); interactionSetAnimation_hook(gb);
}

// Shared tail of states 1 and 2: if the gate's switch bit matches (A was inverted for
// state 1), go to state 3 and start opening/closing.
static void minecartGate_checkSwitch(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1b);
  CYC(b_+147, b_+148); B = A;
  CYC(b_+148, b_+150); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+150, b_+151); A = mem_rd(gb, DE);
  CYC(b_+151, b_+152); alu_and(gb, B);
  if (F & FZ) {
    CYCT(b_+152, b_+153); ret_effect(gb); return;
  }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+155, b_+157); A = 0x03;
  CYC(b_+157, b_+158); mem_wr(gb, DE, A);
  CYC(b_+158, b_+160); A = SND_OPEN_GATE;
  CALL_C(b_+160, playSound_b00_hook, SYM(playSound_b00), b_+163);
  CYC(b_+163, b_+166);
  minecartGate_setAnimationAndUpdateCollisions(gb, sp0_);
}

// @state1: waiting for the switch to be pressed
static void minecartGate_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1b);
  CALL_C(b_+132, objectSetPriorityRelativeToLink_hook, SYM(objectSetPriorityRelativeToLink), b_+135);
  CYC(b_+135, b_+138); A = mem_rd(gb, wSwitchState);
  CYC(b_+138, b_+139); alu_cpl(gb);
  CYC(b_+139, b_+141);
  minecartGate_checkSwitch(gb, sp0_);
}

// @state2: waiting for the switch to be released
static void minecartGate_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1b);
  CALL_C(b_+141, objectSetPriorityRelativeToLink_hook, SYM(objectSetPriorityRelativeToLink), b_+144);
  CYC(b_+144, b_+147); A = mem_rd(gb, wSwitchState);
  minecartGate_checkSwitch(gb, sp0_);
}

// @state3: in the process of opening or closing
static void minecartGate_state3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode1b);
  CALL_C(b_+166, interactionAnimate_hook, SYM(interactionAnimate), b_+169);
  CALL_C(b_+169, objectSetPriorityRelativeToLink_hook, SYM(objectSetPriorityRelativeToLink), b_+172);
  CYC(b_+172, b_+174); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+174, b_+175); A = mem_rd(gb, DE);
  CYC(b_+175, b_+176); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+176, b_+177); ret_effect(gb); return;
  }
  CYC(b_+176, b_+177);
  CYC(b_+177, b_+179); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+182); alu_and(gb, 0x01);
  CYC(b_+182, b_+183); A = alu_inc8(gb, A);
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+185, b_+186); mem_wr(gb, DE, A);
  CYC(b_+186, SYM(interactionCode1f)); ret_effect(gb);
}

// INTERAC_MINECART_GATE: a gate across the minecart track, opened/closed by a switch.
void interactionCode1b_hook(GB *gb) {
  BASE(interactionCode1b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (minecartGate_jumpTable(gb));
    if (jt_ == b_+12) { minecartGate_state0(gb, sp0_); return; }
    else if (jt_ == b_+132) { minecartGate_state1(gb, sp0_); return; }
    else if (jt_ == b_+141) { minecartGate_state2(gb, sp0_); return; }
    else if (jt_ == b_+166) { minecartGate_state3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
