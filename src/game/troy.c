#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7781, interactionInitGraphics_hook, 0x15fb, 0x7784);
  CYC(0x7784, 0x7787); interactionIncState_hook(gb); return; // jp
}

// 0b:7787, called (with return) once from interactionCodeca@subid0, and tail-jumped into once
// from interactionCodeca@subid1.
void interactionCodeca_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7787, interactionInitGraphics_hook, 0x15fb, 0x778a);
  CYC(0x778a, 0x778c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x778c, 0x778d); A = mem_rd(gb, DE);
  CYC(0x778d, 0x7790); SET_HL(0x779a); // interactionCodeca@scriptTable
  CYC(0x7790, 0x7791); interactionCodeca_addDoubleIndexToHl_from_rst(gb, 0x7791);
  CYC(0x7791, 0x7792); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7792, 0x7793); H = mem_rd(gb, HL);
  CYC(0x7793, 0x7794); L = A;
  CALL_C(0x7794, interactionSetScript_hook, 0x2544, 0x7797);
  CYC(0x7797, 0x779a); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_TROY
// ==================================================================================================
void interactionCodeca_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7757, 0x7759); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7759, 0x775a); A = mem_rd(gb, DE);
  CYC(0x775a, 0x775b); push_effect(gb, 0x775b);
  switch (interactionCodeca_jump_table(gb)) {
    case 0x775f: goto subid0;
    case 0x7779: goto subid1;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0:
  CALL_C(0x775f, checkInteractionState_hook, 0x23fe, 0x7762);
  if (!(F & FZ)) { CYCT(0x7762, 0x7764); goto state1; } // jr nz
  CYC(0x7762, 0x7764);
  CALL_C(0x7764, interactionCodeca_initialize_hook, 0x7787, 0x7767);
  CYC(0x7767, 0x776a); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(0x776a, 0x776b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x776b, 0x776d); goto state1; } // jr nz
  CYC(0x776b, 0x776d);
  CYC(0x776d, 0x7770); mem_wr(gb, wTmpcfc0_targetCarts_beganGameWithTroy, A);

state1:
  CALL_C(0x7770, interactionRunScript_hook, 0x2552, 0x7773);
  if (F & FC) { CYCT(0x7773, 0x7776); interactionDelete_hook(gb); return; } // jp c
  CYC(0x7773, 0x7776);
  CYC(0x7776, 0x7779); interactionAnimateAsNpc_hook(gb); return; // jp

subid1:
  CALL_C(0x7779, checkInteractionState_hook, 0x23fe, 0x777c);
  if (!(F & FZ)) { CYCT(0x777c, 0x777e); goto state1; } // jr nz
  CYC(0x777c, 0x777e);
  CYC(0x777e, 0x7781); interactionCodeca_initialize_hook(gb); return; // jp
}
