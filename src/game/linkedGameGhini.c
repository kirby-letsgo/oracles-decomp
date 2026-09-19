#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static void interactionCodecb_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:77be, called once from interactionCodecb@state0.
void interactionCodecb_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77be, interactionInitGraphics_hook, 0x15fb, 0x77c1);
  CALL_C(0x77c1, objectMarkSolidPosition_hook, 0x24f0, 0x77c4);
  CYC(0x77c4, 0x77c7); interactionIncState_hook(gb); return; // jp
}

// 0b:77c7, unused (no callers).
void interactionCodecb_func_77c7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77c7, interactionInitGraphics_hook, 0x15fb, 0x77ca);
  CALL_C(0x77ca, objectMarkSolidPosition_hook, 0x24f0, 0x77cd);
  CYC(0x77cd, 0x77cf); A = 0x4d; // >TX_4d00
  CALL_C(0x77cf, interactionSetHighTextIndex_hook, 0x253b, 0x77d2);
  CYC(0x77d2, 0x77d4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x77d4, 0x77d5); A = mem_rd(gb, DE);
  CYC(0x77d5, 0x77d8); SET_HL(0x77e2); // interactionCodecb@scriptTable
  CYC(0x77d8, 0x77d9); interactionCodecb_addDoubleIndexToHl_from_rst(gb, 0x77d9);
  CYC(0x77d9, 0x77da); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x77da, 0x77db); H = mem_rd(gb, HL);
  CYC(0x77db, 0x77dc); L = A;
  CALL_C(0x77dc, interactionSetScript_hook, 0x2544, 0x77df);
  CYC(0x77df, 0x77e2); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_LINKED_GAME_GHINI
//
// Variables:
//   var3f: Secret index (for "linkedGameNpcScript")
// ==================================================================================================
void interactionCodecb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x779e, checkInteractionState_hook, 0x23fe, 0x77a1);
  if (!(F & FZ)) { CYCT(0x77a1, 0x77a3); goto state1; } // jr nz
  CYC(0x77a1, 0x77a3);

  // interactionCodecb@state0 (0x77a3): reached solely by fallthrough, never a jump target.
  CALL_C(0x77a3, interactionCodecb_initialize_hook, 0x77be, 0x77a6);
  CYC(0x77a6, 0x77a7); H = D;
  CYC(0x77a7, 0x77a9); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x77a9, 0x77ab); mem_wr(gb, HL, 0x02);
  CYC(0x77ab, 0x77ad); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x77ad, 0x77af); mem_wr(gb, HL, 0x01); // GRAVEYARD_SECRET & 0x0f
  CYC(0x77af, 0x77b2); SET_HL(0x7ed9); // mainScripts.linkedGameNpcScript
  CALL_C(0x77b2, interactionSetScript_hook, 0x2544, 0x77b5);

state1:
  CALL_C(0x77b5, interactionRunScript_hook, 0x2552, 0x77b8);
  if (F & FC) { CYCT(0x77b8, 0x77bb); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(0x77b8, 0x77bb);
  CYC(0x77bb, 0x77be); interactionAnimateAsNpc_hook(gb); return; // jp
}
