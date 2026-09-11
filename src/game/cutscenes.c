#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void cutscene_endgameTail_hook(GB *gb);

void cutscene06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d4e, refreshLoadedTreeGfx_hook, 0x1613, 0x5d51);
  CYC(0x5d51, 0x5d53); C = 0x00;
  CYC(0x5d53, 0x5d56); SET_HL(0x6306);
  CYC(0x5d56, 0x5d58); E = 0x03;
  CYC(0x5d58, 0x5d5b);
  interBankCall_hook(gb);
}

void cutscene07_hook(GB *gb) {
  CYC(0x5d5b, 0x5d5d); C = 0x01;
  func_5d5d(gb);
}

void cutscene08_hook(GB *gb) {
  CYC(0x5d6c, 0x5d6e); C = 0x02;
  CYC(0x5d6e, 0x5d70);
  func_5d5d(gb);
}

void cutscene0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d70, refreshLoadedTreeGfx_hook, 0x1613, 0x5d73);
  CYC(0x5d73, 0x5d75); C = 0x03;
  CYC(0x5d75, 0x5d77);
  func_5d5d(gb);
}

void cutscene09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d77, refreshLoadedTreeGfx_hook, 0x1613, 0x5d7a);
  CYC(0x5d7a, 0x5d7d); A = W8(wCutsceneTrigger);
  CYC(0x5d7d, 0x5d7e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5d7e, 0x5d81);
    setCutsceneIndexIfCutsceneTriggerSet(gb);
    return;
  }
  CYC(0x5d7e, 0x5d81);
  CYC(0x5d81, 0x5d83); E = 0x00;
  cutscene_endgameTail_hook(gb);
}

void cutscene_endgameTail_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d83, endgameCutsceneHandler_hook, 0x306c, 0x5d86);
  CYC(0x5d86, 0x5d89); A = W8(wWarpTransition2);
  CYC(0x5d89, 0x5d8a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5d8a, 0x5d8b);
    ret_effect(gb);
    return;
  }
  CYC(0x5d8a, 0x5d8b);
  CYC(0x5d8b, 0x5d8e);
  applyWarpTransition2(gb);
}

void cutscene0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d8e, refreshLoadedTreeGfx_hook, 0x1613, 0x5d91);
  CYC(0x5d91, 0x5d93); E = 0x02;
  CYC(0x5d93, 0x5d96);
  endgameCutsceneHandler_hook(gb);
}

void cutscene0a_hook(GB *gb) {
  CYC(0x5d96, 0x5d98); E = 0x01;
  CYC(0x5d98, 0x5d9b);
  endgameCutsceneHandler_hook(gb);
}

void cutscene20_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d9b, refreshLoadedTreeGfx_hook, 0x1613, 0x5d9e);
  CYC(0x5d9e, 0x5da0); E = 0x03;
  CYC(0x5da0, 0x5da2);
  cutscene_endgameTail_hook(gb);
}

void cutscene0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5da2, refreshLoadedTreeGfx_hook, 0x1613, 0x5da5);
  CYC(0x5da5, 0x5da7); C = 0x06;
  CYC(0x5da7, 0x5daa); SET_HL(0x6306);
  CYC(0x5daa, 0x5dac); E = 0x03;
  CYC(0x5dac, 0x5daf);
  interBankCall_hook(gb);
}

void cutscene0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5daf, refreshLoadedTreeGfx_hook, 0x1613, 0x5db2);
  CYC(0x5db2, 0x5db5); A = W8(wWarpTransition2);
  CYC(0x5db5, 0x5db6); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5db6, 0x5db8);
    applyWarpTransition2(gb);
    return;
  }
  CYC(0x5db6, 0x5db8);
  CYC(0x5db8, 0x5dba); C = 0x05;
  CYC(0x5dba, 0x5dbd); SET_HL(0x6306);
  CYC(0x5dbd, 0x5dbf); E = 0x03;
  CYC(0x5dbf, 0x5dc2);
  interBankCall_hook(gb);
}
