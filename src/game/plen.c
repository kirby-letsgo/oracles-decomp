#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7804, interactionInitGraphics_hook, 0x15fb, 0x7807);
  CYC(0x7807, 0x7809); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7809, 0x780a); A = mem_rd(gb, DE);
  CYC(0x780a, 0x780d); SET_HL(0x7817); // interactionCodecc@scriptTable
  CYC(0x780d, 0x780e); interactionCodecc_addDoubleIndexToHl_from_rst(gb, 0x780e);
  CYC(0x780e, 0x780f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x780f, 0x7810); H = mem_rd(gb, HL);
  CYC(0x7810, 0x7811); L = A;
  CALL_C(0x7811, interactionSetScript_hook, 0x2544, 0x7814);
  CYC(0x7814, 0x7817); interactionIncState_hook(gb); return; // jp
}

// 0b:77fe, unused/unreachable (no label in the disassembly source, but the raw ROM bytes at
// 0x77fe-0x7804 decode to a real, valid instruction sequence identical to the sibling clusters'
// explicitly-labeled "Unused" routines, e.g. interactionCodeca_func_7781_hook in troy.c and
// interactionCodecb_func_77c7_hook in linkedGameGhini.c. Zero incoming references anywhere.
void interactionCodecc_unusedBytes_77fe_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77fe, interactionInitGraphics_hook, 0x15fb, 0x7801);
  CYC(0x7801, 0x7804); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_PLEN
// ==================================================================================================
void interactionCodecc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x77e4, 0x77e6); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x77e6, 0x77e7); A = mem_rd(gb, DE);
  CYC(0x77e7, 0x77e8); push_effect(gb, 0x77e8);
  switch (interactionCodecc_jump_table(gb)) {
    case 0x77ea: goto subid0;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0:
  CALL_C(0x77ea, checkInteractionState_hook, 0x23fe, 0x77ed);
  if (!(F & FZ)) { CYCT(0x77ed, 0x77ef); goto state1; } // jr nz
  CYC(0x77ed, 0x77ef);

  // interactionCodecc@state0 (0x77ef): reached solely by fallthrough, never a jump target.
  CALL_C(0x77ef, interactionCodecc_initialize_hook, 0x7804, 0x77f2);
  CALL_C(0x77f2, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x77f5);

state1:
  CALL_C(0x77f5, interactionRunScript_hook, 0x2552, 0x77f8);
  if (F & FC) { CYCT(0x77f8, 0x77fb); interactionDelete_hook(gb); return; } // jp c
  CYC(0x77f8, 0x77fb);
  CYC(0x77fb, 0x77fe); interactionAnimateAsNpc_hook(gb); return; // jp
}
