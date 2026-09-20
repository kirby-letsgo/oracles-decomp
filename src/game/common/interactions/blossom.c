#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Script pointers into bank $0c, indexed by subid.
#define blossomScriptTable_bank08 0x550f

static uint16_t blossom_jumpTable(GB *gb) {
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

static void blossom_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @updateCollisionAndVisibility
static void blossom_updateCollisionAndVisibility(GB *gb, uint16_t sp0_) {
  CALL_C(0x5509, objectPreventLinkFromPassing_hook, 0x2680, 0x550c);
  CYC(0x550c, 0x550f); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// @updateAnimation: falls through into @updateCollisionAndVisibility.
static void blossom_updateAnimation(GB *gb, uint16_t sp0_) {
  CALL_C(0x5506, interactionAnimate_hook, 0x261b, 0x5509);
  blossom_updateCollisionAndVisibility(gb, sp0_);
}

// @state1
static void blossom_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x5500, interactionRunScript_hook, 0x2552, 0x5503);
  CYC(0x5503, 0x5506);
  blossom_updateAnimation(gb, sp0_);
}

// @initAnimation0
static void blossom_initAnimation0(GB *gb, uint16_t sp0_) {
  CYC(0x54f0, 0x54f2); A = 0x00;
  CALL_C(0x54f2, interactionSetAnimation_hook, 0x262e, 0x54f5);
  CYC(0x54f5, 0x54f8);
  blossom_updateCollisionAndVisibility(gb, sp0_);
}

// @initAnimation4
static void blossom_initAnimation4(GB *gb, uint16_t sp0_) {
  CYC(0x54f8, 0x54fa); A = 0x04;
  CALL_C(0x54fa, interactionSetAnimation_hook, 0x262e, 0x54fd);
  CYC(0x54fd, 0x5500);
  blossom_updateCollisionAndVisibility(gb, sp0_);
}

// @state0: init graphics, pick the script and initial animation from the subid.
static void blossom_state0(GB *gb, uint16_t sp0_) {
  CALL_C(0x54c0, interactionInitGraphics_hook, 0x15fb, 0x54c3);
  CYC(0x54c3, 0x54c5); A = 0x44; // >TX_4400
  CALL_C(0x54c5, interactionSetHighTextIndex_hook, 0x253b, 0x54c8);
  CALL_C(0x54c8, interactionIncState_hook, 0x23e0, 0x54cb);
  CYC(0x54cb, 0x54cd); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x54cd, 0x54ce); A = mem_rd(gb, DE);
  CYC(0x54ce, 0x54d1); SET_HL(blossomScriptTable_bank08);
  CYC(0x54d1, 0x54d2); blossom_addDoubleIndex(gb, 0x54d2);
  CYC(0x54d2, 0x54d3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54d3, 0x54d4); H = mem_rd(gb, HL);
  CYC(0x54d4, 0x54d5); L = A;
  CALL_C(0x54d5, interactionSetScript_hook, 0x2544, 0x54d8);
  CYC(0x54d8, 0x54da); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x54da, 0x54db); A = mem_rd(gb, DE);
  CYC(0x54db, 0x54dc); push_effect(gb, 0x54dc);
  switch (blossom_jumpTable(gb)) {
    case 0x54f0: blossom_initAnimation0(gb, sp0_); return;
    case 0x54f8: blossom_initAnimation4(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}

// INTERAC_BLOSSOM
void interactionCode2b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54b8, 0x54ba); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x54ba, 0x54bb); A = mem_rd(gb, DE);
  CYC(0x54bb, 0x54bc); push_effect(gb, 0x54bc);
  switch (blossom_jumpTable(gb)) {
    case 0x54c0: blossom_state0(gb, sp0_); return;
    case 0x5500: blossom_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
