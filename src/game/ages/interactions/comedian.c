#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void comedian_add_double_index(GB *gb, uint16_t ra) {
  BASE(interactionCode65);
  CYC(b_+60, b_+61); push_effect(gb, ra);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode65__loadScriptAndInitGraphics_hook(GB *gb) {
  BASE(interactionCode65);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+43, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+46);
  CALL_C(b_+46, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+49);
  CYC(b_+49, b_+51); A = 0x0b;
  CALL_C(b_+51, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+54);
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+60); SET_HL(b_+70);
  comedian_add_double_index(gb, b_+61);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+62, b_+63); H = mem_rd(gb, HL);
  CYC(b_+63, b_+64); L = A;
  CALL_C(b_+64, interactionSetScript_hook, SYM(interactionSetScript), b_+67);
  CYC(b_+67, b_+70); TAIL(interactionIncState);
}

void comedian_run_state0_after_load_hook(GB *gb) {
  BASE(interactionCode65);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
  CYC(b_+14, b_+17); TAIL(interactionAnimateAsNpc);
}

void interactionCode65__afterCall750e_hook(GB *gb) {
  comedian_run_state0_after_load_hook(gb);
}

void interactionCode65_hook(GB *gb) {
  BASE(interactionCode65);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5);
    goto state1;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, interactionCode65__loadScriptAndInitGraphics_hook, b_+43, b_+8);
  comedian_run_state0_after_load_hook(gb);
  return;

state1:
  CALL_C(b_+17, interactionRunScript_hook, SYM(interactionRunScript), b_+20);
  if (F & FC) {
    CYCT(b_+20, b_+23);
    TAIL(interactionDelete);
  }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+26); SET_HL((SYM(ambi_runSubid03__substate0) + 15));
  CYC(b_+26, b_+28); E = 0x15;
  CALL_C(b_+28, interBankCall_hook, 0x008a, b_+31);
  CYC(b_+31, b_+34); TAIL(interactionAnimateAsNpc);
}
