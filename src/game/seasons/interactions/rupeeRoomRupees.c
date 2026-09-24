#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/rupeeRoomRupees.s.
// INTERAC_RUPEE_ROOM_RUPEES

static uint16_t rupeeRoomRupees_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void rupeeRoomRupees_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// INTERAC_RUPEE_ROOM_RUPEES
void s_interactionCode1d_hook(GB *gb) {
  BASE(interactionCode1d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (rupeeRoomRupees_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+39) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+20); SET_HL(b_+33 /* @state0@rupeeRoomTable */);
  CYC(b_+20, b_+21); rupeeRoomRupees_add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); E = alu_inc8(gb, E);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  RET(b_+32); return;

state1:
  CYC(b_+39, b_+42); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+42, b_+44); alu_cp(gb, 0x3c);
  if (F & FZ) { CYCT(b_+44, b_+46); goto L_45d1; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); alu_cp(gb, 0x3d);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49);
L_45d1:
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+56); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+56, b_+57); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+57, b_+58); B = A;
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+61, b_+62); H = mem_rd(gb, HL);
  CYC(b_+62, b_+63); L = A;
  CYC(b_+63, b_+64); A = B;
  CYC(b_+64, b_+66); A = alu_swap(gb, A);
  CYC(b_+66, b_+68); alu_and(gb, 0x0f);
  CYC(b_+68, b_+69); rupeeRoomRupees_add_a_to_hl(gb, b_+69);
  CYC(b_+69, b_+70); A = B;
  CYC(b_+70, b_+72); alu_and(gb, 0x0f);
  CYC(b_+72, b_+75); SET_BC(SYM(bitTable));
  CYC(b_+75, b_+76); alu_add(gb, C);
  CYC(b_+76, b_+77); C = A;
  CYC(b_+77, b_+78); A = mem_rd(gb, BC);
  CYC(b_+78, b_+79); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+79, b_+80); mem_wr(gb, HL, A);
  CALL_C(b_+80, s_getRandomNumber, SYM(getRandomNumber), b_+83);
  CYC(b_+83, b_+85); alu_and(gb, 0x0f);
  CYC(b_+85, b_+88); SET_HL(b_+119 /* @state1@chosenRupeeVal */);
  CYC(b_+88, b_+89); rupeeRoomRupees_add_a_to_hl(gb, b_+89);
  CYC(b_+89, b_+90); C = mem_rd(gb, HL);
  CYC(b_+90, b_+92); A = 0x26;
  CALL_C(b_+92, s_cpActiveRing, SYM(cpActiveRing), b_+95);
  if (F & FZ) { CYCT(b_+95, b_+97); goto state1_doubleRupees; }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); A = 0x24;
  CALL_C(b_+99, s_cpActiveRing, SYM(cpActiveRing), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+104); goto state1_giveRupees; }
  CYC(b_+102, b_+104);
state1_doubleRupees:
  CYC(b_+104, b_+105); C = alu_inc8(gb, C);
state1_giveRupees:
  CYC(b_+105, b_+107); A = 0x28;
  CALL_C(b_+107, s_giveTreasure, SYM(giveTreasure), b_+110);
  CYC(b_+110, b_+113); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+113, b_+114); C = A;
  CYC(b_+114, b_+116); A = 0xa0;
  CYC(b_+116, b_+119);
  TAIL(setTile);
}

