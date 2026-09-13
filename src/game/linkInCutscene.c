#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static void link_cutscene_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x0012, 0x0013, false);
    burn_rom(gb, 0, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

void linkCutsceneRet_hook(GB *gb) {
  CYC(0x7250, 0x7251); ret_effect(gb);
}

void linkCutscene_animateAndDecCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x74a1, specialObjectAnimate_hook, 0x2aef, 0x74a4);
  CYC(0x74a4, 0x74a7); itemDecCounter1_hook(gb);
}

void linkCutscene_oscillateZ_hook(GB *gb) {
  CYC(0x7180, 0x7183); A = W8(wTmpcbb7);
  CYC(0x7183, 0x7185); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x7185, 0x7187);
  } else {
    CYC(0x7185, 0x7187);
    CYC(0x7187, 0x718a); A = W8(wTmpcbb7);
    CYC(0x718a, 0x718c); alu_and(gb, 0x38);
    CYC(0x718c, 0x718e); A = alu_swap(gb, A);
    CYC(0x718e, 0x718f); alu_rlca(gb);
    CYC(0x718f, 0x7190); push_effect(gb, 0x7190); link_cutscene_add_a_to_hl(gb);
    CYC(0x7190, 0x7192); E = 0x0f;
    CYC(0x7192, 0x7193); A = mem_rd(gb, HL);
    CYC(0x7193, 0x7194); B = A;
    CYC(0x7194, 0x7195); A = mem_rd(gb, DE);
    CYC(0x7195, 0x7196); alu_add(gb, B);
    CYC(0x7196, 0x7197); mem_wr(gb, DE, A);
  }
  CYC(0x7197, 0x719a); specialObjectAnimate_hook(gb);
}

void linkCutscene_oscillateZ_1_hook(GB *gb) {
  CYC(0x717d, 0x7180); SET_HL(0x71a2);
  linkCutscene_oscillateZ_hook(gb);
}

void linkCutscene_oscillateZ_2_hook(GB *gb) {
  CYC(0x771a, 0x771d); SET_HL(0x71aa);
  CYC(0x771d, 0x7720); linkCutscene_oscillateZ_hook(gb);
}

void linkCutscene_createGlowingOrb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7167, 0x716a); SET_BC(0x8406);
  CALL_C(0x716a, objectCreateInteraction_hook, 0x24c5, 0x716d);
  if (!(F & FZ)) {
    CYCT(0x716d, 0x716f);
  } else {
    CYC(0x716d, 0x716f);
    CYC(0x716f, 0x7171); L = 0x56;
    CYC(0x7171, 0x7173); A = 0;
    CYC(0x7173, 0x7174); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7174, 0x7175); mem_wr(gb, HL, D);
  }
  CALL_C(0x7175, itemIncSubstate_hook, 0x23ef, 0x7178);
  CYC(0x7178, 0x717a); A = 0x05;
  CYC(0x717a, 0x717d); specialObjectSetAnimation_hook(gb);
}
