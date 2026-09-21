#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodecb);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+32, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+35);
  CALL_C(b_+35, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+38);
  CYC(b_+38, b_+41); TAIL(interactionIncState); // jp
}

// 0b:77c7, unused (no callers).
void interactionCodecb_func_77c7_hook(GB *gb) {
  BASE(interactionCodecb);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+41, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+44);
  CALL_C(b_+44, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+47);
  CYC(b_+47, b_+49); A = 0x4d; // >TX_4d00
  CALL_C(b_+49, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+52);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+58); SET_HL(b_+68); // interactionCodecb@scriptTable
  CYC(b_+58, b_+59); interactionCodecb_addDoubleIndexToHl_from_rst(gb, b_+59);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+60, b_+61); H = mem_rd(gb, HL);
  CYC(b_+61, b_+62); L = A;
  CALL_C(b_+62, interactionSetScript_hook, SYM(interactionSetScript), b_+65);
  CYC(b_+65, b_+68); TAIL(interactionIncState); // jp
}

// ==================================================================================================
// INTERAC_LINKED_GAME_GHINI
//
// Variables:
//   var3f: Secret index (for "linkedGameNpcScript")
// ==================================================================================================
void interactionCodecb_hook(GB *gb) {
  BASE(interactionCodecb);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interactionCodecb@state0 (0x77a3): reached solely by fallthrough, never a jump target.
  CALL_C(b_+5, interactionCodecb_initialize_hook, b_+32, b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x02);
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x01); // GRAVEYARD_SECRET & 0x0f
  CYC(b_+17, b_+20); SET_HL((SYM(interactionCoded8__subid4Script) + 93)); // mainScripts.linkedGameNpcScript
  CALL_C(b_+20, interactionSetScript_hook, SYM(interactionSetScript), b_+23);

state1:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if (F & FC) { CYCT(b_+26, b_+29); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+32); TAIL(interactionAnimateAsNpc); // jp
}
