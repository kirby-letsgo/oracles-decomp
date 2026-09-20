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
  CALL_C(b_+141, objectApplySpeed_hook, SYM(objectApplySpeed), b_+144);
  CYC(b_+144, b_+146); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+146, b_+147); A = mem_rd(gb, DE);
  CYC(b_+147, b_+149); alu_sub(gb, 0x28);
  CYC(b_+149, b_+151); alu_cp(gb, 0x30);
  if (F & FC) {
    CYCT(b_+151, b_+152); ret_effect(gb); return;
  }
  CYC(b_+151, b_+152);
  // Reverse direction
  CYC(b_+152, b_+153); H = D;
  CYC(b_+153, b_+155); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+155, b_+156); A = mem_rd(gb, HL);
  CYC(b_+156, b_+158); alu_xor(gb, 0x10);
  CYC(b_+158, b_+159); mem_wr(gb, HL, A);
  CYC(b_+159, b_+161); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+161, b_+162); A = mem_rd(gb, HL);
  CYC(b_+162, b_+164); alu_xor(gb, 0x01);
  CYC(b_+164, b_+165); mem_wr(gb, HL, A);
  CYC(b_+165, b_+168); interactionSetAnimation_hook(gb);
}

// @updateCollisionAndVisibility
static void bipin_updateCollisionAndVisibility(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+135, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+138);
  CYC(b_+138, b_+141); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// @updateAnimation: falls through into @updateCollisionAndVisibility.
static void bipin_updateAnimation(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+132, interactionAnimate_hook, SYM(interactionAnimate), b_+135);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @runScriptAndAnimate
static void bipin_runScriptAndAnimate(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+126, interactionRunScript_hook, SYM(interactionRunScript), b_+129);
  CYC(b_+129, b_+132);
  bipin_updateAnimation(gb, sp0_);
}

// @bipinSubid0: falls through into @runScriptAndAnimate.
static void bipin_subid0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+123, b_+126); push_effect(gb, b_+126); bipin_updateSpeed(gb, sp0_);
  bipin_runScriptAndAnimate(gb, sp0_);
}

// @state1
static void bipin_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+97, b_+99); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+99, b_+100); A = mem_rd(gb, DE);
  CYC(b_+100, b_+101); push_effect(gb, b_+101);
  do { uint16_t jt_ = (bipin_jumpTable(gb));
    if (jt_ == b_+123) { bipin_subid0(gb, sp0_); return; }
    else if (jt_ == b_+126) { bipin_runScriptAndAnimate(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// @bipin0: Bipin running around, baby just born
static void bipin_init0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x18);
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+64, b_+66); A = 0x04;
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  CALL_C(b_+67, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+70);
  CYC(b_+70, b_+73);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin1: Bipin gives you a random tip
static void bipin_init1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+73, b_+75); A = 0x03;
  CALL_C(b_+75, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+78);
  CYC(b_+78, b_+81);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin2: Bipin just moved to Labrynna
static void bipin_init2(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+81, b_+83); A = 0x02;
  CALL_C(b_+83, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+86);
  CYC(b_+86, b_+89);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @bipin3: "Past" version of Bipin who gives you a gasha seed
static void bipin_init3(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CYC(b_+89, b_+91); A = 0x09;
  CALL_C(b_+91, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+94);
  CYC(b_+94, b_+97);
  bipin_updateCollisionAndVisibility(gb, sp0_);
}

// @state0: init graphics, pick the script from the subid, then per-subid setup.
static void bipin_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode28);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, interactionIncState_hook, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+20); SET_HL(bipinScriptTable_bank08);
  CYC(b_+20, b_+21); bipin_addDoubleIndex(gb, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  CYC(b_+23, b_+24); L = A;
  CALL_C(b_+24, interactionSetScript_hook, SYM(interactionSetScript), b_+27);
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); push_effect(gb, b_+31);
  do { uint16_t jt_ = (bipin_jumpTable(gb));
    if (jt_ == b_+53) { bipin_init0(gb, sp0_); return; }
    else if (jt_ == b_+73) { bipin_init1(gb, sp0_); return; }
    else if (jt_ == b_+81) { bipin_init2(gb, sp0_); return; }
    else if (jt_ == b_+89) { bipin_init3(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// INTERAC_BIPIN
void interactionCode28_hook(GB *gb) {
  BASE(interactionCode28);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bipin_jumpTable(gb));
    if (jt_ == b_+8) { bipin_state0(gb, sp0_); return; }
    else if (jt_ == b_+97) { bipin_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
