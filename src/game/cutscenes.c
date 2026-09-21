#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void cutscene_endgameTail_hook(GB *gb);
void func_5d5d_hook(GB *gb);

void cutscene06_hook(GB *gb) {
  BASE(cutscene06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+5); C = 0x00;
  CYC(b_+5, b_+8); SET_HL((SYM(checkLinkCanStandOnTile) + 29));
  CYC(b_+8, b_+10); E = 0x03;
  CYC(b_+10, b_+13);
  TAIL(interBankCall);
}

void cutscene07_hook(GB *gb) {
  BASE(cutscene07);
  CYC(b_+0, b_+2); C = 0x01;
  TAIL(func_5d5d);
}

void func_5d5d_hook(GB *gb) {
  BASE(func_5d5d);
  CYC(b_+0, b_+3); A = mem_rd(gb, wWarpTransition2);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+7);
    TAIL(applyWarpTransition2);
  }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+10); SET_HL((SYM(checkLinkCanStandOnTile) + 29));
  CYC(b_+10, b_+12); E = 0x03;
  CYC(b_+12, b_+15);
  TAIL(interBankCall);
}

void cutscene08_hook(GB *gb) {
  BASE(cutscene08);
  CYC(b_+0, b_+2); C = 0x02;
  CYC(b_+2, b_+4);
  TAIL(func_5d5d);
}

void cutscene0c_hook(GB *gb) {
  BASE(cutscene0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+5); C = 0x03;
  CYC(b_+5, b_+7);
  TAIL(func_5d5d);
}

void cutscene09_hook(GB *gb) {
  BASE(cutscene09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+O(0), refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+OE(3));
  if (!game_seasons) {
    CYC(b_+3, b_+6); A = W8(wCutsceneTrigger);
    CYC(b_+6, b_+7); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+7, b_+10);
      TAIL(setCutsceneIndexIfCutsceneTriggerSet);
    }
    CYC(b_+7, b_+10);
  }
  CYC(b_+O(10), b_+OE(12)); E = 0x00;
  cutscene_endgameTail_hook(gb);
}

void cutscene_endgameTail_hook(GB *gb) {
  BASE(cutscene09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+O(12), endgameCutsceneHandler_hook, SYM(endgameCutsceneHandler), b_+OE(15));
  CYC(b_+O(15), b_+OE(18)); A = W8(wWarpTransition2);
  CYC(b_+O(18), b_+OE(19)); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+O(19), b_+OE(20));
    ret_effect(gb);
    return;
  }
  CYC(b_+O(19), b_+OE(20));
  CYC(b_+O(20), b_+OE(23));
  TAIL(applyWarpTransition2);
}

void cutscene0f_hook(GB *gb) {
  BASE(cutscene0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+5); E = 0x02;
  CYC(b_+5, b_+8);
  TAIL(endgameCutsceneHandler);
}

void cutscene0a_hook(GB *gb) {
  BASE(cutscene0a);
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+5);
  TAIL(endgameCutsceneHandler);
}

void cutscene20_hook(GB *gb) {
  BASE(cutscene20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+5); E = 0x03;
  CYC(b_+5, b_+7);
  cutscene_endgameTail_hook(gb);
}

void cutscene0d_hook(GB *gb) {
  BASE(cutscene0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+5); C = 0x06;
  CYC(b_+5, b_+8); SET_HL((SYM(checkLinkCanStandOnTile) + 29));
  CYC(b_+8, b_+10); E = 0x03;
  CYC(b_+10, b_+13);
  TAIL(interBankCall);
}

void cutscene0e_hook(GB *gb) {
  BASE(cutscene0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, refreshLoadedTreeGfx_hook, SYM(refreshLoadedTreeGfx), b_+3);
  CYC(b_+3, b_+6); A = W8(wWarpTransition2);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    TAIL(applyWarpTransition2);
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); C = 0x05;
  CYC(b_+11, b_+14); SET_HL((SYM(checkLinkCanStandOnTile) + 29));
  CYC(b_+14, b_+16); E = 0x03;
  CYC(b_+16, b_+19);
  TAIL(interBankCall);
}

void cutscene21_hook(GB *gb) {
  BASE(cutscene21);
  CYC(b_+0, b_+3); A = W8(wCutsceneTrigger);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+7);
    TAIL(setCutsceneIndexIfCutsceneTriggerSet);
  }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); C = 0x07;
  CYC(b_+9, b_+11);
  TAIL(func_5d5d);
}

void cutscene10_hook(GB *gb) {
  BASE(cutscene10);
  CYC(b_+0, b_+2); C = 0x04;
  CYC(b_+2, b_+4);
  TAIL(func_5d5d);
}

void cutscene11_hook(GB *gb) {
  BASE(cutscene11);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_3ed0_hook, GV(SYM(func_3ed0), 0x335a), b_+3);
  CYC(b_+3, b_+6);
  TAIL(func_5d41);
}

void cutscene12_hook(GB *gb) {
  BASE(cutscene12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wCutsceneTrigger);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+7);
    TAIL(setCutsceneIndexIfCutsceneTriggerSet);
  }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, func_3ee4_hook, SYM(func_3ee4), b_+10);
  CYC(b_+10, b_+13);
  TAIL(func_5d41);
}
