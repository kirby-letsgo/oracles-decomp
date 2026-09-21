#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Table of per-dungeon script tables, indexed by wDungeonIndex then by subid.
#define dungeonScriptTable_bank08 SYM(interactionCode20__scriptTable)

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
  BASE(interactionCode20);
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+20); mem_wr(gb, wTmpcfc0 + 1, A);
  CYC(b_+20, b_+23); mem_wr(gb, wTmpcfc0 + 2, A);
  CYC(b_+23, b_+26); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+26, b_+28); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+28, b_+31); TAIL(interactionDelete);
  }
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+34); SET_HL(dungeonScriptTable_bank08);
  CYC(b_+34, b_+35); dungeonScript_addDoubleIndex(gb, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); H = mem_rd(gb, HL);
  CYC(b_+37, b_+38); L = A;
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); dungeonScript_addDoubleIndex(gb, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); H = mem_rd(gb, HL);
  CYC(b_+44, b_+45); L = A;
  CALL_C(b_+45, interactionSetScript_hook, SYM(interactionSetScript), b_+48);
  CYC(b_+48, b_+51); interactionRunScript_hook(gb);
}

// @state1: run the script; delete self when it finishes.
static void dungeonScript_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode20);
  CALL_C(b_+54, interactionRunScript_hook, SYM(interactionRunScript), b_+57);
  if (!(F & FC)) {
    CYCT(b_+57, b_+58); ret_effect(gb); return;
  }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+61); interactionDelete_hook(gb);
}

// @state2: like state 1, but also blocks Link; falls through into state 1.
static void dungeonScript_state2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode20);
  CALL_C(b_+51, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+54);
  dungeonScript_state1(gb, sp0_);
}

// INTERAC_DUNGEON_SCRIPT: runs one of the per-dungeon puzzle scripts.
void interactionCode20_hook(GB *gb) {
  BASE(interactionCode20);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (dungeonScript_jumpTable(gb));
    if (jt_ == b_+13) { dungeonScript_state0(gb, sp0_); return; }
    else if (jt_ == b_+54) { dungeonScript_state1(gb, sp0_); return; }
    else if (jt_ == b_+51) { dungeonScript_state2(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
