#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void postman_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void postman_load_script_and_init_graphics(GB *gb, uint16_t sp0_) {
  BASE(interactionCode55);
  CALL_C(b_+33, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+36);
  CYC(b_+36, b_+38); A = 0x0b;
  CALL_C(b_+38, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+41);
  CYC(b_+41, b_+43); E = OBJ_SUBID;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+47); SET_HL(b_+57);
  CYC(b_+47, b_+48); postman_add_double_index(gb, b_+48);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+50); H = mem_rd(gb, HL);
  CYC(b_+50, b_+51); L = A;
  CALL_C(b_+51, interactionSetScript_hook, SYM(interactionSetScript), b_+54);
  CYC(b_+54, b_+57); interactionIncState_hook(gb);
}

void interactionCode55_hook(GB *gb) {
  BASE(interactionCode55);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) CYCT(b_+3, b_+5); else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+8); push_effect(gb, b_+8);
    postman_load_script_and_init_graphics(gb, sp0_);
  }
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
  if (F & FC) { CYCT(b_+11, b_+14); interactionDelete_hook(gb); return; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); E = OBJ_VAR3F;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+21); npcFaceLinkAndAnimate_hook(gb); return; }
  CYC(b_+18, b_+21);
  CALL_C(b_+21, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+24);
  CYC(b_+24, b_+27); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}
