#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodeca_jump_table(GB *gb) {
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

static void interactionCodeca_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:7781, unused (no callers).
void interactionCodeca_func_7781_hook(GB *gb) {
  BASE(interactionCodeca);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+42, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+45);
  CYC(b_+45, b_+48); interactionIncState_hook(gb); return; // jp
}

// 0b:7787, called (with return) once from interactionCodeca@subid0, and tail-jumped into once
// from interactionCodeca@subid1.
void interactionCodeca_initialize_hook(GB *gb) {
  BASE(interactionCodeca);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+48, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+51);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+57); SET_HL(b_+67); // interactionCodeca@scriptTable
  CYC(b_+57, b_+58); interactionCodeca_addDoubleIndexToHl_from_rst(gb, b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+59, b_+60); H = mem_rd(gb, HL);
  CYC(b_+60, b_+61); L = A;
  CALL_C(b_+61, interactionSetScript_hook, SYM(interactionSetScript), b_+64);
  CYC(b_+64, b_+67); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_TROY
// ==================================================================================================
void interactionCodeca_hook(GB *gb) {
  BASE(interactionCodeca);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodeca_jump_table(gb));
    if (jt_ == b_+8) { goto subid0; }
    else if (jt_ == b_+34) { goto subid1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0:
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto state1; } // jr nz
  CYC(b_+11, b_+13);
  CALL_L(b_+13, interactionCodeca_initialize_hook, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto state1; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); mem_wr(gb, wTmpcfc0_targetCarts_beganGameWithTroy, A);

state1:
  CALL_C(b_+25, interactionRunScript_hook, SYM(interactionRunScript), b_+28);
  if (F & FC) { CYCT(b_+28, b_+31); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+34); interactionAnimateAsNpc_hook(gb); return; // jp

subid1:
  CALL_C(b_+34, checkInteractionState_hook, SYM(checkInteractionState), b_+37);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto state1; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); interactionCodeca_initialize_hook(gb); return; // jp
}
