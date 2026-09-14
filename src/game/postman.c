#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

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
  CALL_C(0x6911, interactionInitGraphics_hook, 0x15fb, 0x6914);
  CYC(0x6914, 0x6916); A = 0x0b;
  CALL_C(0x6916, interactionSetHighTextIndex_hook, 0x253b, 0x6919);
  CYC(0x6919, 0x691b); E = OBJ_SUBID;
  CYC(0x691b, 0x691c); A = mem_rd(gb, DE);
  CYC(0x691c, 0x691f); SET_HL(0x6929);
  CYC(0x691f, 0x6920); postman_add_double_index(gb, 0x6920);
  CYC(0x6920, 0x6921); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6921, 0x6922); H = mem_rd(gb, HL);
  CYC(0x6922, 0x6923); L = A;
  CALL_C(0x6923, interactionSetScript_hook, 0x2544, 0x6926);
  CYC(0x6926, 0x6929); interactionIncState_hook(gb);
}

void interactionCode55_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x68f0, checkInteractionState_hook, 0x23fe, 0x68f3);
  if (!(F & FZ)) CYCT(0x68f3, 0x68f5); else {
    CYC(0x68f3, 0x68f5);
    CYC(0x68f5, 0x68f8); push_effect(gb, 0x68f8);
    postman_load_script_and_init_graphics(gb, sp0_);
  }
  CALL_C(0x68f8, interactionRunScript_hook, 0x2552, 0x68fb);
  if (F & FC) { CYCT(0x68fb, 0x68fe); interactionDelete_hook(gb); return; }
  CYC(0x68fb, 0x68fe);
  CYC(0x68fe, 0x6900); E = OBJ_VAR3F;
  CYC(0x6900, 0x6901); A = mem_rd(gb, DE);
  CYC(0x6901, 0x6902); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6902, 0x6905); npcFaceLinkAndAnimate_hook(gb); return; }
  CYC(0x6902, 0x6905);
  CALL_C(0x6905, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x6908);
  CYC(0x6908, 0x690b); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}
