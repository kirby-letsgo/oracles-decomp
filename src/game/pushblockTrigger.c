#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

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
  CYC(0x43df, 0x43e0); H = D;
  CYC(0x43e0, 0x43e2); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x43e2, 0x43e4); mem_wr(gb, HL, 0x01);
  CALL_C(0x43e4, objectGetShortPosition_hook, 0x2096, 0x43e7);
  CYC(0x43e7, 0x43e9); L = INTERACTION_BASE + OBJ_VAR18;
  CYC(0x43e9, 0x43ea); mem_wr(gb, HL, A);
  CYC(0x43ea, 0x43eb); C = A;
  CYC(0x43eb, 0x43ed); B = wRoomLayout >> 8;
  CYC(0x43ed, 0x43ee); A = mem_rd(gb, BC);
  CYC(0x43ee, 0x43ef); L = alu_inc8(gb, L);
  CYC(0x43ef, 0x43f0); mem_wr(gb, HL, A);
  CYC(0x43f0, 0x43f2); A = TILEINDEX_PUSHABLE_BLOCK;
  CYC(0x43f2, 0x43f3); mem_wr(gb, BC, A);
  CYC(0x43f3, 0x43f6); SET_HL(wNumEnemies);
  CYC(0x43f6, 0x43f7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x43f7, 0x43f8); ret_effect(gb);
}

// interactionCode13@state1: wait for wNumEnemies to reach subid, then restore the original tile.
static void pushblockTrigger_state1(GB *gb) {
  CYC(0x43f8, 0x43fb); A = mem_rd(gb, wNumEnemies);
  CYC(0x43fb, 0x43fc); B = A;
  CYC(0x43fc, 0x43fe); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x43fe, 0x43ff); A = mem_rd(gb, DE);
  CYC(0x43ff, 0x4400); alu_cp(gb, B);
  if (F & FC) {
    CYCT(0x4400, 0x4401); ret_effect(gb); return;
  }
  CYC(0x4400, 0x4401);
  CYC(0x4401, 0x4403); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4403, 0x4405); A = 0x02;
  CYC(0x4405, 0x4406); mem_wr(gb, DE, A);
  CYC(0x4406, 0x4408); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(0x4408, 0x4409); A = mem_rd(gb, DE);
  CYC(0x4409, 0x440a); C = A;
  CYC(0x440a, 0x440b); E = alu_inc8(gb, E);
  CYC(0x440b, 0x440c); A = mem_rd(gb, DE);
  CYC(0x440c, 0x440e); B = wRoomLayout >> 8;
  CYC(0x440e, 0x440f); mem_wr(gb, BC, A);
  CYC(0x440f, 0x4410); ret_effect(gb);
}

// interactionCode13@state2: wait for the tile at var18 to differ from var19 (block pushed).
static void pushblockTrigger_state2(GB *gb) {
  CYC(0x4410, 0x4412); E = INTERACTION_BASE + OBJ_VAR18;
  CYC(0x4412, 0x4413); A = mem_rd(gb, DE);
  CYC(0x4413, 0x4414); L = A;
  CYC(0x4414, 0x4415); E = alu_inc8(gb, E);
  CYC(0x4415, 0x4416); A = mem_rd(gb, DE);
  CYC(0x4416, 0x4418); H = wRoomLayout >> 8;
  CYC(0x4418, 0x4419); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4419, 0x441a); ret_effect(gb); return;
  }
  CYC(0x4419, 0x441a);
  CYC(0x441a, 0x441c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x441c, 0x441e); A = 0x03;
  CYC(0x441e, 0x441f); mem_wr(gb, DE, A);
  CYC(0x441f, 0x4421); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4421, 0x4423); A = 0x1e;
  CYC(0x4423, 0x4424); mem_wr(gb, DE, A);
  CYC(0x4424, 0x4425); ret_effect(gb);
}

// interactionCode13@state3: short delay, then clear wNumEnemies and delete self.
static void pushblockTrigger_state3(GB *gb, uint16_t sp0_) {
  CALL_C(0x4425, interactionDecCounter1_hook, 0x23cc, 0x4428);
  if (!(F & FZ)) {
    CYCT(0x4428, 0x4429); ret_effect(gb); return;
  }
  CYC(0x4428, 0x4429);
  CYC(0x4429, 0x442a); alu_xor(gb, A);
  CYC(0x442a, 0x442d); mem_wr(gb, wNumEnemies, A);
  CYC(0x442d, 0x4430); interactionDelete_hook(gb);
}

void interactionCode13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x43cd, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x43d0);
  CALL_C(0x43d0, returnIfScrollMode01Unset_hook, 0x26e4, 0x43d3);
  CYC(0x43d3, 0x43d5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x43d5, 0x43d6); A = mem_rd(gb, DE);
  CYC(0x43d6, 0x43d7); push_effect(gb, 0x43d7);
  switch (pushblockTrigger_jumpTable(gb)) {
    case 0x43df: pushblockTrigger_state0(gb, sp0_); return;
    case 0x43f8: pushblockTrigger_state1(gb); return;
    case 0x4410: pushblockTrigger_state2(gb); return;
    case 0x4425: pushblockTrigger_state3(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
