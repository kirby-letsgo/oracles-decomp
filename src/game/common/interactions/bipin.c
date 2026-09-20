#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Script pointers into bank $0c, indexed by subid.
#define bipinScriptTable_bank08 0x542d

static uint16_t bipin_jumpTable(GB *gb) {
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

static void bipin_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @updateSpeed: Bipin runs around like a madman when his baby is first born. Reached only
// by a static `call` from interactionCode28 itself.
static void bipin_updateSpeed(GB *gb, uint16_t sp0_) {
  CALL_C(0x5412, objectApplySpeed_hook, 0x201d, 0x5415);
  CYC(0x5415, 0x5417); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5417, 0x5418); A = mem_rd(gb, DE);
  CYC(0x5418, 0x541a); alu_sub(gb, 0x28);
  CYC(0x541a, 0x541c); alu_cp(gb, 0x30);
  if (F & FC) {
    CYCT(0x541c, 0x541d); ret_effect(gb); return;
  }
  CYC(0x541c, 0x541d);
  // Reverse direction
  CYC(0x541d, 0x541e); H = D;
  CYC(0x541e, 0x5420); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5420, 0x5421); A = mem_rd(gb, HL);
  CYC(0x5421, 0x5423); alu_xor(gb, 0x10);
  CYC(0x5423, 0x5424); mem_wr(gb, HL, A);
  CYC(0x5424, 0x5426); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x5426, 0x5427); A = mem_rd(gb, HL);
  CYC(0x5427, 0x5429); alu_xor(gb, 0x01);
  CYC(0x5429, 0x542a); mem_wr(gb, HL, A);
  CYC(0x542a, 0x542d); interactionSetAnimation_hook(gb);
}

// @updateCollisionAndVisibility
static void bipin_updateCollisionAndVisibility(GB *gb, uint16_t sp0_) {
  CALL_C(0x540c, objectPreventLinkFromPassing_hook, 0x2680, 0x540f);
  CYC(0x540f, 0x5412); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// @updateAnimation: falls through into @updateCollisionAndVisibility.
static void bipin_updateAnimation(GB *gb, uint16_t sp0_) {
  CALL_C(0x5409, interactionAnimate_hook, 0x261b, 0x540c);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @runScriptAndAnimate
static void bipin_runScriptAndAnimate(GB *gb, uint16_t sp0_) {
  CALL_C(0x5403, interactionRunScript_hook, 0x2552, 0x5406);
  CYC(0x5406, 0x5409);
  bipin_updateAnimation(gb, sp0_);
}

// @bipinSubid0: falls through into @runScriptAndAnimate.
static void bipin_subid0(GB *gb, uint16_t sp0_) {
  CYC(0x5400, 0x5403); push_effect(gb, 0x5403); bipin_updateSpeed(gb, sp0_);
  bipin_runScriptAndAnimate(gb, sp0_);
}

// @state1
static void bipin_state1(GB *gb, uint16_t sp0_) {
  CYC(0x53e6, 0x53e8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x53e8, 0x53e9); A = mem_rd(gb, DE);
  CYC(0x53e9, 0x53ea); push_effect(gb, 0x53ea);
  switch (bipin_jumpTable(gb)) {
    case 0x5400: bipin_subid0(gb, sp0_); return;
    case 0x5403: bipin_runScriptAndAnimate(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}

// @bipin0: Bipin running around, baby just born
static void bipin_init0(GB *gb, uint16_t sp0_) {
  CYC(0x53ba, 0x53bb); H = D;
  CYC(0x53bb, 0x53bd); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x53bd, 0x53bf); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x53bf, 0x53c1); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x53c1, 0x53c3); mem_wr(gb, HL, 0x18);
  CYC(0x53c3, 0x53c5); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x53c5, 0x53c7); A = 0x04;
  CYC(0x53c7, 0x53c8); mem_wr(gb, HL, A);
  CALL_C(0x53c8, interactionSetAnimation_hook, 0x262e, 0x53cb);
  CYC(0x53cb, 0x53ce);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin1: Bipin gives you a random tip
static void bipin_init1(GB *gb, uint16_t sp0_) {
  CYC(0x53ce, 0x53d0); A = 0x03;
  CALL_C(0x53d0, interactionSetAnimation_hook, 0x262e, 0x53d3);
  CYC(0x53d3, 0x53d6);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin2: Bipin just moved to Labrynna
static void bipin_init2(GB *gb, uint16_t sp0_) {
  CYC(0x53d6, 0x53d8); A = 0x02;
  CALL_C(0x53d8, interactionSetAnimation_hook, 0x262e, 0x53db);
  CYC(0x53db, 0x53de);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin3: "Past" version of Bipin who gives you a gasha seed
static void bipin_init3(GB *gb, uint16_t sp0_) {
  CYC(0x53de, 0x53e0); A = 0x09;
  CALL_C(0x53e0, interactionSetAnimation_hook, 0x262e, 0x53e3);
  CYC(0x53e3, 0x53e6);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @state0: init graphics, pick the script from the subid, then per-subid setup.
static void bipin_state0(GB *gb, uint16_t sp0_) {
  CALL_C(0x538d, interactionInitGraphics_hook, 0x15fb, 0x5390);
  CALL_C(0x5390, interactionIncState_hook, 0x23e0, 0x5393);
  CYC(0x5393, 0x5395); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5395, 0x5396); A = mem_rd(gb, DE);
  CYC(0x5396, 0x5399); SET_HL(bipinScriptTable_bank08);
  CYC(0x5399, 0x539a); bipin_addDoubleIndex(gb, 0x539a);
  CYC(0x539a, 0x539b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x539b, 0x539c); H = mem_rd(gb, HL);
  CYC(0x539c, 0x539d); L = A;
  CALL_C(0x539d, interactionSetScript_hook, 0x2544, 0x53a0);
  CYC(0x53a0, 0x53a2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x53a2, 0x53a3); A = mem_rd(gb, DE);
  CYC(0x53a3, 0x53a4); push_effect(gb, 0x53a4);
  switch (bipin_jumpTable(gb)) {
    case 0x53ba: bipin_init0(gb, sp0_); return;
    case 0x53ce: bipin_init1(gb, sp0_); return;
    case 0x53d6: bipin_init2(gb, sp0_); return;
    case 0x53de: bipin_init3(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}

// INTERAC_BIPIN
void interactionCode28_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5385, 0x5387); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5387, 0x5388); A = mem_rd(gb, DE);
  CYC(0x5388, 0x5389); push_effect(gb, 0x5389);
  switch (bipin_jumpTable(gb)) {
    case 0x538d: bipin_state0(gb, sp0_); return;
    case 0x53e6: bipin_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
