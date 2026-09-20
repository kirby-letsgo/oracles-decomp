#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define TILEINDEX_PUSHABLE_BLOCK 0x1d

static uint16_t pushblockTrigger_jumpTable(GB *gb) {
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

// interactionCode13@state0: replace the tile at the object's position with a pushable block,
// remembering the original tile in var19, and count it as an "enemy".
static void pushblockTrigger_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode13);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x01);
  CALL_C(b_+23, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+26);
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_VAR18;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+32); B = wRoomLayout >> 8;
  CYC(b_+32, b_+33); A = mem_rd(gb, BC);
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CYC(b_+35, b_+37); A = TILEINDEX_PUSHABLE_BLOCK;
  CYC(b_+37, b_+38); mem_wr(gb, BC, A);
  CYC(b_+38, b_+41); SET_HL(wNumEnemies);
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+42, b_+43); ret_effect(gb);
}

// interactionCode13@state1: wait for wNumEnemies to reach subid, then restore the original tile.
static void pushblockTrigger_state1(GB *gb) {
  BASE(interactionCode13);
  CYC(b_+43, b_+46); A = mem_rd(gb, wNumEnemies);
  CYC(b_+46, b_+47); B = A;
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_cp(gb, B);
  if (F & FC) {
    CYCT(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+54, b_+56); A = 0x02;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); C = A;
  CYC(b_+61, b_+62); E = alu_inc8(gb, E);
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+65); B = wRoomLayout >> 8;
  CYC(b_+65, b_+66); mem_wr(gb, BC, A);
  CYC(b_+66, b_+67); ret_effect(gb);
}

// interactionCode13@state2: wait for the tile at var18 to differ from var19 (block pushed).
static void pushblockTrigger_state2(GB *gb) {
  BASE(interactionCode13);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); L = A;
  CYC(b_+71, b_+72); E = alu_inc8(gb, E);
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  CYC(b_+73, b_+75); H = wRoomLayout >> 8;
  CYC(b_+75, b_+76); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+76, b_+77); ret_effect(gb); return;
  }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+79, b_+81); A = 0x03;
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+84); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+84, b_+86); A = 0x1e;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+88); ret_effect(gb);
}

// interactionCode13@state3: short delay, then clear wNumEnemies and delete self.
static void pushblockTrigger_state3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode13);
  CALL_C(b_+88, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+91);
  if (!(F & FZ)) {
    CYCT(b_+91, b_+92); ret_effect(gb); return;
  }
  CYC(b_+91, b_+92);
  CYC(b_+92, b_+93); alu_xor(gb, A);
  CYC(b_+93, b_+96); mem_wr(gb, wNumEnemies, A);
  CYC(b_+96, b_+99); interactionDelete_hook(gb);
}

void interactionCode13_hook(GB *gb) {
  BASE(interactionCode13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (pushblockTrigger_jumpTable(gb));
    if (jt_ == b_+18) { pushblockTrigger_state0(gb, sp0_); return; }
    else if (jt_ == b_+43) { pushblockTrigger_state1(gb); return; }
    else if (jt_ == b_+67) { pushblockTrigger_state2(gb); return; }
    else if (jt_ == b_+88) { pushblockTrigger_state3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
