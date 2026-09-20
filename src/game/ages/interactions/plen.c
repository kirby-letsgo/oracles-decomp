#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodecc_jump_table(GB *gb) {
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

static void interactionCodecc_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:7804, called once from interactionCodecc@state0.
void interactionCodecc_initialize_hook(GB *gb) {
  BASE(interactionCodecc);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+32, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+35);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+41); SET_HL(b_+51); // interactionCodecc@scriptTable
  CYC(b_+41, b_+42); interactionCodecc_addDoubleIndexToHl_from_rst(gb, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+43, b_+44); H = mem_rd(gb, HL);
  CYC(b_+44, b_+45); L = A;
  CALL_C(b_+45, interactionSetScript_hook, SYM(interactionSetScript), b_+48);
  CYC(b_+48, b_+51); interactionIncState_hook(gb); return; // jp
}

// 0b:77fe, unused/unreachable (no label in the disassembly source, but the raw ROM bytes at
// 0x77fe-0x7804 decode to a real, valid instruction sequence identical to the sibling clusters'
// explicitly-labeled "Unused" routines, e.g. interactionCodeca_func_7781_hook in troy.c and
// interactionCodecb_func_77c7_hook in linkedGameGhini.c. Zero incoming references anywhere.
void interactionCodecc_unusedBytes_77fe_hook(GB *gb) {
  BASE(interactionCodecc);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+26, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+29);
  CYC(b_+29, b_+32); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_PLEN
// ==================================================================================================
void interactionCodecc_hook(GB *gb) {
  BASE(interactionCodecc);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodecc_jump_table(gb));
    if (jt_ == b_+6) { goto subid0; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0:
  CALL_C(b_+6, checkInteractionState_hook, SYM(checkInteractionState), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto state1; } // jr nz
  CYC(b_+9, b_+11);

  // interactionCodecc@state0 (0x77ef): reached solely by fallthrough, never a jump target.
  CALL_L(b_+11, interactionCodecc_initialize_hook, b_+14);
  CALL_C(b_+14, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+17);

state1:
  CALL_C(b_+17, interactionRunScript_hook, SYM(interactionRunScript), b_+20);
  if (F & FC) { CYCT(b_+20, b_+23); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+26); interactionAnimateAsNpc_hook(gb); return; // jp
}
