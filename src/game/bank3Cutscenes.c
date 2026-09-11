#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

void incCutsceneState_hook(GB *gb) {
  CYC(0x4b10, 0x4b13); SET_HL(wCutsceneState);
  CYC(0x4b13, 0x4b14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b14, 0x4b15); ret_effect(gb);
}

void unused_incTmpcbb3_hook(GB *gb) {
  CYC(0x4b15, 0x4b18); SET_HL(wTmpcbb3);
  CYC(0x4b18, 0x4b19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b19, 0x4b1a); ret_effect(gb);
}

void decTmpcbb4_hook(GB *gb) {
  CYC(0x4b1a, 0x4b1d); SET_HL(wTmpcbb4);
  CYC(0x4b1d, 0x4b1e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4b1e, 0x4b1f); ret_effect(gb);
}

void setScreenShakeCounterTo255_hook(GB *gb) {
  CYC(0x4b1f, 0x4b21); A = 0xff;
  CYC(0x4b21, 0x4b24);
  setScreenShakeCounter_hook(gb);
}

void twinrovaCutscene_createLightningStrike_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4cba, getFreePartSlot_hook, 0x3e8e, 0x4cbd);
  if (!(F & FZ)) {
    CYCT(0x4cbd, 0x4cbe); ret_effect(gb);
    return;
  }
  CYC(0x4cbd, 0x4cbe);
  CYC(0x4cbe, 0x4cc0); mem_wr(gb, HL, 0x27);
  CYC(0x4cc0, 0x4cc1); L = alu_inc8(gb, L);
  CYC(0x4cc1, 0x4cc2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4cc2, 0x4cc4); L = 0xcb;
  CYC(0x4cc4, 0x4cc5); mem_wr(gb, HL, B);
  CYC(0x4cc5, 0x4cc6); L = alu_inc8(gb, L);
  CYC(0x4cc6, 0x4cc7); L = alu_inc8(gb, L);
  CYC(0x4cc7, 0x4cc8); mem_wr(gb, HL, C);
  CYC(0x4cc8, 0x4cc9); ret_effect(gb);
}

void incIntroCinematicState_hook(GB *gb) {
  CYC(0x50ca, 0x50cd); SET_HL(wIntro_cinematicState);
  CYC(0x50cd, 0x50ce); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x50ce, 0x50cf); alu_xor(gb, A);
  CYC(0x50cf, 0x50d2); mem_wr(gb, wThreadStateBuffer + 7, A);
  CYC(0x50d2, 0x50d3); ret_effect(gb);
}

void createInteraction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x540c, getFreeInteractionSlot_hook, 0x3aef, 0x540f);
  if (!(F & FZ)) {
    CYCT(0x540f, 0x5410); ret_effect(gb);
    return;
  }
  CYC(0x540f, 0x5410);
  CYC(0x5410, 0x5411); mem_wr(gb, HL, B);
  CYC(0x5411, 0x5412); L = alu_inc8(gb, L);
  CYC(0x5412, 0x5413); mem_wr(gb, HL, C);
  CYC(0x5413, 0x5414); ret_effect(gb);
}
