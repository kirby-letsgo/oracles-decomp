#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static void comedian_add_double_index(GB *gb, uint16_t ra) {
  CYC(0x7542, 0x7543); push_effect(gb, ra);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode65__loadScriptAndInitGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7531, interactionInitGraphics_hook, 0x15fb, 0x7534);
  CALL_C(0x7534, objectMarkSolidPosition_hook, 0x24f0, 0x7537);
  CYC(0x7537, 0x7539); A = 0x0b;
  CALL_C(0x7539, interactionSetHighTextIndex_hook, 0x253b, 0x753c);
  CYC(0x753c, 0x753e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x753e, 0x753f); A = mem_rd(gb, DE);
  CYC(0x753f, 0x7542); SET_HL(0x754c);
  comedian_add_double_index(gb, 0x7543);
  CYC(0x7543, 0x7544); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7544, 0x7545); H = mem_rd(gb, HL);
  CYC(0x7545, 0x7546); L = A;
  CALL_C(0x7546, interactionSetScript_hook, 0x2544, 0x7549);
  CYC(0x7549, 0x754c); interactionIncState_hook(gb);
}

void comedian_run_state0_after_load_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x750e, interactionRunScript_hook, 0x2552, 0x7511);
  CALL_C(0x7511, interactionRunScript_hook, 0x2552, 0x7514);
  CYC(0x7514, 0x7517); interactionAnimateAsNpc_hook(gb);
}

void interactionCode65__afterCall750e_hook(GB *gb) {
  comedian_run_state0_after_load_hook(gb);
}

void interactionCode65_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7506, checkInteractionState_hook, 0x23fe, 0x7509);
  if (!(F & FZ)) {
    CYCT(0x7509, 0x750b);
    goto state1;
  }
  CYC(0x7509, 0x750b);
  CALL_C(0x750b, interactionCode65__loadScriptAndInitGraphics_hook, 0x7531, 0x750e);
  comedian_run_state0_after_load_hook(gb);
  return;

state1:
  CALL_C(0x7517, interactionRunScript_hook, 0x2552, 0x751a);
  if (F & FC) {
    CYCT(0x751a, 0x751d);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x751a, 0x751d);
  CYC(0x751d, 0x7520); SET_HL(0x6289);
  CYC(0x7520, 0x7522); E = 0x15;
  CALL_C(0x7522, interBankCall_hook, 0x008a, 0x7525);
  CYC(0x7525, 0x7528); interactionAnimateAsNpc_hook(gb);
}
