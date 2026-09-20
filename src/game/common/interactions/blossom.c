#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode2b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode2b), (from), (to), true)

// Script pointers into bank $0c, indexed by subid.
#define blossomScriptTable_bank08 SYM(interactionCode2b__scriptTable)

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
  BASE(interactionCode2b);
  CALL_C(b_+81, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+84);
  CYC(b_+84, b_+87); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// @updateAnimation: falls through into @updateCollisionAndVisibility.
static void blossom_updateAnimation(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2b);
  CALL_C(b_+78, interactionAnimate_hook, SYM(interactionAnimate), b_+81);
  blossom_updateCollisionAndVisibility(gb, sp0_);
}

// @state1
static void blossom_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2b);
  CALL_C(b_+72, interactionRunScript_hook, SYM(interactionRunScript), b_+75);
  CYC(b_+75, b_+78);
  blossom_updateAnimation(gb, sp0_);
}

// @initAnimation0
static void blossom_initAnimation0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2b);
  CYC(b_+56, b_+58); A = 0x00;
  CALL_C(b_+58, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+61);
  CYC(b_+61, b_+64);
  blossom_updateCollisionAndVisibility(gb, sp0_);
}

// @initAnimation4
static void blossom_initAnimation4(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2b);
  CYC(b_+64, b_+66); A = 0x04;
  CALL_C(b_+66, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+69);
  CYC(b_+69, b_+72);
  blossom_updateCollisionAndVisibility(gb, sp0_);
}

// @state0: init graphics, pick the script and initial animation from the subid.
static void blossom_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2b);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+13); A = 0x44; // >TX_4400
  CALL_C(b_+13, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+25); SET_HL(blossomScriptTable_bank08);
  CYC(b_+25, b_+26); blossom_addDoubleIndex(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); H = mem_rd(gb, HL);
  CYC(b_+28, b_+29); L = A;
  CALL_C(b_+29, interactionSetScript_hook, SYM(interactionSetScript), b_+32);
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); push_effect(gb, b_+36);
  do { uint16_t jt_ = (blossom_jumpTable(gb));
    if (jt_ == b_+56) { blossom_initAnimation0(gb, sp0_); return; }
    else if (jt_ == b_+64) { blossom_initAnimation4(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// INTERAC_BLOSSOM
void interactionCode2b_hook(GB *gb) {
  BASE(interactionCode2b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (blossom_jumpTable(gb));
    if (jt_ == b_+8) { blossom_state0(gb, sp0_); return; }
    else if (jt_ == b_+72) { blossom_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
