#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void child_addValueToChildStatus_hook(GB *gb);
void child_checkHasRupees_hook(GB *gb);
void child_setStage8ResponseToSelectedTextOption_hook(GB *gb);
void child_setStage8Response_hook(GB *gb);
void child_playMusic_hook(GB *gb);
void child_giveHeartRefill_hook(GB *gb);
void child_giveOneHeart_hook(GB *gb);
void child_giveRupees_hook(GB *gb);

void child_addValueToChildStatus_hook(GB *gb) {
  BASE(child_addValueToChildStatus);
  CYC(b_+0, b_+3); SET_HL(wChildStatus);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  RET(b_+5);
}

void child_checkHasRupees_hook(GB *gb) {
  BASE(child_checkHasRupees);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRupeeValue_hook, SYM(cpRupeeValue), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6);
}

void child_setStage8ResponseToSelectedTextOption_hook(GB *gb) {
  BASE(child_setStage8ResponseToSelectedTextOption);
  CYC(b_+0, b_+3); SET_HL(wSelectedTextOption);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  child_setStage8Response_hook(gb);
}

void child_setStage8Response_hook(GB *gb) {
  BASE(child_setStage8Response);
  CYC(b_+0, b_+3); mem_wr(gb, wChildStage8Response, A);
  RET(b_+3);
}

void child_playMusic_hook(GB *gb) {
  BASE(child_playMusic);
  CYC(b_+0, b_+3); A = mem_rd(gb, wChildStage8Response);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto playHigh;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x38;
  CYC(b_+8, b_+11); playSound_b00_hook(gb);
  return;

playHigh:
  CYC(b_+11, b_+13); A = 0x4a;
  CYC(b_+13, b_+16); playSound_b00_hook(gb);
}

static void child_giveHealthFromC(GB *gb) {
  BASE(child_giveOneHeart);
  CYC(b_+2, b_+4); A = 0x29;
  CYC(b_+4, b_+7); giveTreasure_hook(gb);
}

void child_giveHeartRefill_hook(GB *gb) {
  BASE(child_giveHeartRefill);
  CYC(b_+0, b_+2); C = 0x40;
  CYC(b_+2, b_+4);
  child_giveHealthFromC(gb);
}

void child_giveOneHeart_hook(GB *gb) {
  BASE(child_giveOneHeart);
  CYC(b_+0, b_+2); C = 0x04;
  child_giveHealthFromC(gb);
}

void child_giveRupees_hook(GB *gb) {
  BASE(child_giveRupees);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+3); A = 0x28;
  CYC(b_+3, b_+6); giveTreasure_hook(gb);
}
