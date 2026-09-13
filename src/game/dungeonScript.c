#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Table of per-dungeon script tables, indexed by wDungeonIndex then by subid.
#define dungeonScriptTable_bank08 0x4c2c

static uint16_t dungeonScript_jumpTable(GB *gb) {
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

static void dungeonScript_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @state0: pick the script for (wDungeonIndex, subid) and start running it.
static void dungeonScript_state0(GB *gb, uint16_t sp0_) {
  CYC(0x4bfc, 0x4bfe); A = 0x01;
  CYC(0x4bfe, 0x4bff); mem_wr(gb, DE, A);
  CYC(0x4bff, 0x4c00); alu_xor(gb, A);
  CYC(0x4c00, 0x4c03); mem_wr(gb, wTmpcfc0 + 1, A);
  CYC(0x4c03, 0x4c06); mem_wr(gb, wTmpcfc0 + 2, A);
  CYC(0x4c06, 0x4c09); A = mem_rd(gb, wDungeonIndex);
  CYC(0x4c09, 0x4c0b); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4c0b, 0x4c0e); interactionDelete_hook(gb); return;
  }
  CYC(0x4c0b, 0x4c0e);
  CYC(0x4c0e, 0x4c11); SET_HL(dungeonScriptTable_bank08);
  CYC(0x4c11, 0x4c12); dungeonScript_addDoubleIndex(gb, 0x4c12);
  CYC(0x4c12, 0x4c13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4c13, 0x4c14); H = mem_rd(gb, HL);
  CYC(0x4c14, 0x4c15); L = A;
  CYC(0x4c15, 0x4c17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4c17, 0x4c18); A = mem_rd(gb, DE);
  CYC(0x4c18, 0x4c19); dungeonScript_addDoubleIndex(gb, 0x4c19);
  CYC(0x4c19, 0x4c1a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4c1a, 0x4c1b); H = mem_rd(gb, HL);
  CYC(0x4c1b, 0x4c1c); L = A;
  CALL_C(0x4c1c, interactionSetScript_hook, 0x2544, 0x4c1f);
  CYC(0x4c1f, 0x4c22); interactionRunScript_hook(gb);
}

// @state1: run the script; delete self when it finishes.
static void dungeonScript_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x4c25, interactionRunScript_hook, 0x2552, 0x4c28);
  if (!(F & FC)) {
    CYCT(0x4c28, 0x4c29); ret_effect(gb); return;
  }
  CYC(0x4c28, 0x4c29);
  CYC(0x4c29, 0x4c2c); interactionDelete_hook(gb);
}

// @state2: like state 1, but also blocks Link; falls through into state 1.
static void dungeonScript_state2(GB *gb, uint16_t sp0_) {
  CALL_C(0x4c22, objectPreventLinkFromPassing_hook, 0x2680, 0x4c25);
  dungeonScript_state1(gb, sp0_);
}

// INTERAC_DUNGEON_SCRIPT: runs one of the per-dungeon puzzle scripts.
void interactionCode20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4bef, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4bf2);
  CYC(0x4bf2, 0x4bf4); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4bf4, 0x4bf5); A = mem_rd(gb, DE);
  CYC(0x4bf5, 0x4bf6); push_effect(gb, 0x4bf6);
  switch (dungeonScript_jumpTable(gb)) {
    case 0x4bfc: dungeonScript_state0(gb, sp0_); return;
    case 0x4c25: dungeonScript_state1(gb, sp0_); return;
    case 0x4c22: dungeonScript_state2(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
