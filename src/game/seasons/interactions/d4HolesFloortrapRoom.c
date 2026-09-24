#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/d4HolesFloortrapRoom.s.
// INTERAC_D4_HOLES_FLOORTRAP_ROOM

static uint16_t d4HolesFloortrapRoom_jump_table(GB *gb) {
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

// interactionCodec5@func_7aea
static void d4HolesFloortrapRoom_func_7aea(GB *gb) {
  BASE(interactionCodec5);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+54, b_+56); C = 0x2c;
  CALL_C(b_+56, s_setTile, SYM(setTile), b_+59);
  CYC(b_+59, b_+62);
  TAIL(objectCreatePuff);
}

// INTERAC_D4_HOLES_FLOORTRAP_ROOM
void s_interactionCodec5_hook(GB *gb) {
  BASE(interactionCodec5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (d4HolesFloortrapRoom_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+27) goto state1;
    if (jt_ == b_+62) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+10, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, s_interactionIncState, SYM(interactionIncState), b_+21);
  CYC(b_+21, b_+24); SET_BC(SYM(d4floorTrapRoom_tilesToBreak));
  CYC(b_+24, b_+27);
  TAIL_S(d4floorTrapRoom_storeAddressOfFirstHoleTilePosition);
state1:
  CYC(b_+27, b_+30); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); A = 0xf1;
  CALL_L(b_+34, d4HolesFloortrapRoom_func_7aea, b_+37);
  CYC(b_+37, b_+39); A = 0x4d;
  CALL_C(b_+39, s_playSound, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+44, b_+46); A = 0x20;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+48); E = alu_dec8(gb, E);
  CYC(b_+48, b_+50); A = 0x10;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+54);
  TAIL(interactionIncState);

state2:
  CYC(b_+62, b_+65); A = mem_rd(gb, wFrameCounter);
  CYC(b_+65, b_+66); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+66); return; }
  CYC(b_+66, b_+67);
  CALL_C(b_+67, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+70);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+72); L = alu_inc8(gb, L);
  CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+73, b_+74); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+74, b_+75); alu_rrca(gb);
  CYC(b_+75, b_+77); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+77, b_+79); goto L_7b04; }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); mem_wr(gb, HL, A);
L_7b04:
  CALL_C(b_+80, s_d4floorTrapRoom_storeNextHoleTileAddressIntoHL_hook, SYM(d4floorTrapRoom_storeNextHoleTileAddressIntoHL), b_+83);
  CYC(b_+83, b_+84); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+84, b_+85); C = A;
  CALL_C(b_+85, s_d4floorTrapRoom_storeIncrementedAddressOfNextHoleTile_hook, SYM(d4floorTrapRoom_storeIncrementedAddressOfNextHoleTile), b_+88);
  CYC(b_+88, b_+89); A = C;
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+90, b_+93); TAIL(interactionDelete); }
  CYC(b_+90, b_+93);
  CYC(b_+93, b_+95); A = 0xf4;
  CYC(b_+95, b_+98);
  TAIL(breakCrackedFloor);
}

void s_d4floorTrapRoom_storeAddressOfFirstHoleTilePosition_hook(GB *gb) {
  BASE(d4floorTrapRoom_storeAddressOfFirstHoleTilePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+3, b_+4); mem_wr(gb, HL, C);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); mem_wr(gb, HL, B);
  RET(b_+6); return;
}

void s_d4floorTrapRoom_storeNextHoleTileAddressIntoHL_hook(GB *gb) {
  BASE(d4floorTrapRoom_storeNextHoleTileAddressIntoHL);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); H = mem_rd(gb, HL);
  CYC(b_+5, b_+6); L = A;
  RET(b_+6); return;
}

void s_d4floorTrapRoom_storeIncrementedAddressOfNextHoleTile_hook(GB *gb) {
  BASE(d4floorTrapRoom_storeIncrementedAddressOfNextHoleTile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = H;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  RET(b_+7); return;
}

