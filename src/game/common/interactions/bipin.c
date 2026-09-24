#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Script pointers into bank $0c, indexed by subid.
#define bipinScriptTable_bank08 SYM(interactionCode28__scriptTable)

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
  BASE(interactionCode28);
  CALL_C(b_+O(141), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(144));
  CYC(b_+O(144), b_+OE(146)); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(146), b_+OE(147)); A = mem_rd(gb, DE);
  CYC(b_+O(147), b_+OE(149)); alu_sub(gb, 0x28);
  CYC(b_+O(149), b_+OE(151)); alu_cp(gb, 0x30);
  if (F & FC) {
    CYCT(b_+O(151), b_+OE(152)); ret_effect(gb); return;
  }
  CYC(b_+O(151), b_+OE(152));
  // Reverse direction
  CYC(b_+O(152), b_+OE(153)); H = D;
  CYC(b_+O(153), b_+OE(155)); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(155), b_+OE(156)); A = mem_rd(gb, HL);
  CYC(b_+O(156), b_+OE(158)); alu_xor(gb, 0x10);
  CYC(b_+O(158), b_+OE(159)); mem_wr(gb, HL, A);
  CYC(b_+O(159), b_+OE(161)); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+O(161), b_+OE(162)); A = mem_rd(gb, HL);
  CYC(b_+O(162), b_+OE(164)); alu_xor(gb, 0x01);
  CYC(b_+O(164), b_+OE(165)); mem_wr(gb, HL, A);
  CYC(b_+O(165), b_+OE(168)); interactionSetAnimation_hook(gb);
}

// @updateCollisionAndVisibility
static void bipin_updateCollisionAndVisibility(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+O(135), objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+OE(138));
  CYC(b_+O(138), b_+OE(141)); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// @updateAnimation: falls through into @updateCollisionAndVisibility.
static void bipin_updateAnimation(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+O(132), interactionAnimate_hook, SYM(interactionAnimate), b_+OE(135));
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @runScriptAndAnimate
static void bipin_runScriptAndAnimate(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+O(126), interactionRunScript_hook, SYM(interactionRunScript), b_+OE(129));
  CYC(b_+O(129), b_+OE(132));
  bipin_updateAnimation(gb, sp0_);
}

// @bipinSubid0: falls through into @runScriptAndAnimate.
static void bipin_subid0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+O(123), b_+OE(126)); push_effect(gb, b_+OE(126)); bipin_updateSpeed(gb, sp0_);
  bipin_runScriptAndAnimate(gb, sp0_);
}

// @state1
static void bipin_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+O(97), b_+OE(99)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(99), b_+OE(100)); A = mem_rd(gb, DE);
  CYC(b_+O(100), b_+OE(101)); push_effect(gb, b_+OE(101));
  do { uint16_t jt_ = (bipin_jumpTable(gb));
    if (jt_ == b_+O(123)) { bipin_subid0(gb, sp0_); return; }
    else if (jt_ == b_+O(126)) { bipin_runScriptAndAnimate(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// @bipin0: Bipin running around, baby just born
static void bipin_init0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+O(53), b_+OE(54)); H = D;
  CYC(b_+O(54), b_+OE(56)); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+O(56), b_+OE(58)); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+O(58), b_+OE(60)); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+O(60), b_+OE(62)); mem_wr(gb, HL, 0x18);
  CYC(b_+O(62), b_+OE(64)); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+O(64), b_+OE(66)); A = 0x04;
  CYC(b_+O(66), b_+OE(67)); mem_wr(gb, HL, A);
  CALL_C(b_+O(67), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(70));
  CYC(b_+O(70), b_+OE(73));
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin1: Bipin gives you a random tip
static void bipin_init1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+O(73), b_+OE(75)); A = 0x03;
  CALL_C(b_+O(75), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(78));
  CYC(b_+O(78), b_+OE(81));
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin2: Bipin just moved to Labrynna
static void bipin_init2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+O(81), b_+OE(83)); A = 0x02;
  CALL_C(b_+O(83), interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+OE(86));
  CYC(b_+O(86), b_+OE(89));
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin3: "Past" version of Bipin who gives you a gasha seed
static void bipin_init3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  AGES_ONLY();
  CYC(b_+89, b_+91); A = 0x09;
  CALL_C(b_+91, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+94);
  CYC(b_+94, b_+97);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @state0: init graphics, pick the script from the subid, then per-subid setup.
static void bipin_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+O(8), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(11));
  CALL_C(b_+O(11), interactionIncState_hook, SYM(interactionIncState), b_+OE(14));
  CYC(b_+O(14), b_+OE(16)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(16), b_+OE(17)); A = mem_rd(gb, DE);
  CYC(b_+O(17), b_+OE(20)); SET_HL(bipinScriptTable_bank08);
  CYC(b_+O(20), b_+OE(21)); bipin_addDoubleIndex(gb, b_+O(21));
  CYC(b_+O(21), b_+OE(22)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(22), b_+OE(23)); H = mem_rd(gb, HL);
  CYC(b_+O(23), b_+OE(24)); L = A;
  CALL_C(b_+O(24), interactionSetScript_hook, SYM(interactionSetScript), b_+OE(27));
  CYC(b_+O(27), b_+OE(29)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(29), b_+OE(30)); A = mem_rd(gb, DE);
  CYC(b_+O(30), b_+OE(31)); push_effect(gb, b_+OE(31));
  do { uint16_t jt_ = (bipin_jumpTable(gb));
    if (jt_ == b_+O(53)) { bipin_init0(gb, sp0_); return; }
    else if (jt_ == b_+O(73)) { bipin_init1(gb, sp0_); return; }
    else if (jt_ == b_+O(81)) { bipin_init2(gb, sp0_); return; }
    else if (!game_seasons && jt_ == b_+89) { bipin_init3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// INTERAC_BIPIN
void interactionCode28_hook(GB *gb) {
  BASE(interactionCode28);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (bipin_jumpTable(gb));
    if (jt_ == b_+O(8)) { bipin_state0(gb, sp0_); return; }
    else if (jt_ == b_+O(97)) { bipin_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
