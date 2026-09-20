#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MONKEY), bank 0x15.

void monkey_decideTextIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5800, 0x5802); B = 0x00; // <TX_5708-8
  CYC(0x5802, 0x5804); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x5804, checkGlobalFlag_hook, 0x31f3, 0x5807);
  if (F & FZ) { CYCT(0x5807, 0x5809); goto set_late_text; } // jr z
  CYC(0x5807, 0x5809);
  CYC(0x5809, 0x580b); B = 0x05; // <TX_570d-8

set_late_text:
  CALL_C(0x580b, getRandomNumber_hook, 0x043e, 0x580e);
  CYC(0x580e, 0x5810); alu_and(gb, 0x03);
  CYC(0x5810, 0x5811); alu_add(gb, B);
  CYC(0x5811, 0x5813); alu_add(gb, 0x08); // <TX_5708
  CYC(0x5813, 0x5815); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x5815, 0x5816); mem_wr(gb, DE, A);
  RET(0x5816); return;
}

void monkey_turnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5817, 0x5818); H = D;
  CYC(0x5818, 0x581a); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x581a, 0x581d); A = W8(w1Link_yh);
  CYC(0x581d, 0x581e); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x581e, 0x5820); A = 0x06;
  if (!(F & FC)) { CYCT(0x5820, 0x5822); goto set_animation; } // jr nc
  CYC(0x5820, 0x5822);
  CYC(0x5822, 0x5823); A = alu_dec8(gb, A);

set_animation:
  CYC(0x5823, 0x5826); interactionSetAnimation_hook(gb); return; // jp
}

void monkey_setAnimationFromVar3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5826, 0x5828); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x5828, 0x5829); A = mem_rd(gb, DE);
  CYC(0x5829, 0x582c); interactionSetAnimation_hook(gb); return; // jp
}
