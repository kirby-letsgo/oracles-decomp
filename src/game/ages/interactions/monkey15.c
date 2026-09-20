#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MONKEY), bank 0x15.

void monkey_decideTextIndex_hook(GB *gb) {
  BASE(monkey_decideTextIndex);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00; // <TX_5708-8
  CYC(b_+2, b_+4); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+4, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+7);
  if (F & FZ) { CYCT(b_+7, b_+9); goto set_late_text; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x05; // <TX_570d-8

set_late_text:
  CALL_C(b_+11, getRandomNumber_hook, SYM(getRandomNumber), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x03);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+19); alu_add(gb, 0x08); // <TX_5708
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  RET(b_+22); return;
}

void monkey_turnToFaceLink_hook(GB *gb) {
  BASE(monkey_turnToFaceLink);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+3, b_+6); A = W8(w1Link_yh);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); A = 0x06;
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto set_animation; } // jr nc
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);

set_animation:
  CYC(b_+12, b_+15); interactionSetAnimation_hook(gb); return; // jp
}

void monkey_setAnimationFromVar3a_hook(GB *gb) {
  BASE(monkey_setAnimationFromVar3a);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); interactionSetAnimation_hook(gb); return; // jp
}
