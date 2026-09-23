#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SOKRA), bank 0x15.

// Animation 4 and an exclamation mark above Sokra.
void s_sokra_alert_hook(GB *gb) {
  BASE(sokra_alert);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+5);
  CYC(b_+5, b_+7); B = 0xf0;
  CYC(b_+7, b_+9); C = 0xfc;
  CYC(b_+9, b_+11); A = 0x40;
  CYC(b_+11, b_+14);
  TAIL(objectCreateExclamationMark);
}

// Moves; var36 = 1 once Link is within $10.
void s_villageSokra_waitUntilLinkInPosition_hook(GB *gb) {
  BASE(villageSokra_waitUntilLinkInPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); C = 0x10;
  CALL_C(b_+5, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = 0x76; // Interaction.var36
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return;
}

// Moves until level with Link's y; then var36 = 1.
void s_seasonsFunc_15_5802_hook(GB *gb) {
  BASE(seasonsFunc_15_5802);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x4b; // Interaction.yh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(w1Link_yh);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+7, b_+10); TAIL(objectApplySpeed); }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); E = 0x76; // Interaction.var36
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}

// Moves until yh reaches a; then var36 = 0.
void s_seasonsFunc_15_5812_hook(GB *gb) {
  BASE(seasonsFunc_15_5812);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+2); push_effect(gb, BC);
  CALL_C(b_+2, objectApplySpeed_hook, SYM(objectApplySpeed), b_+5);
  CYC(b_+5, b_+7); E = 0x4b; // Interaction.yh
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); SET_BC(pop_effect(gb));
  CYC(b_+9, b_+10); alu_sub(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = 0x76; // Interaction.var36
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return;
}

// wTmpcfc0 = 0 before the second essence, 1 after it, 2 once the seasons are locked to spring.
void s_villageSokra_checkStageInGame_hook(GB *gb) {
  BASE(villageSokra_checkStageInGame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+5); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+5, b_+7); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CYC(b_+10, b_+12); A = 0x30; // GLOBALFLAG_SEASON_ALWAYS_SPRING
  CALL_C(b_+12, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+17); goto store; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); B = alu_inc8(gb, B);
store:
  CYC(b_+18, b_+21); SET_HL(wTmpcfc0);
  CYC(b_+21, b_+22); mem_wr(gb, HL, B);
  RET(b_+22); return;
}

void s_suburbsSokra_jumpOffStump_hook(GB *gb) {
  BASE(suburbsSokra_jumpOffStump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); C = 0x10;
  CYC(b_+5, b_+8);
  TAIL(objectUpdateSpeedZ_paramC);
}

// Compares yh with Link's y, then faces down (carry) or up.
void s_seasonsFunc_15_5840_hook(GB *gb) {
  BASE(seasonsFunc_15_5840);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x4b; // Interaction.yh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(w1Link_yh);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  TAIL_S(seasonsFunc_15_5847);
}

void s_seasonsFunc_15_5847_hook(GB *gb) {
  BASE(seasonsFunc_15_5847);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x10; // ANGLE_DOWN
  if (F & FC) { CYCT(b_+2, b_+4); goto store; }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+5); alu_xor(gb, A);
store:
  CYC(b_+5, b_+7); E = 0x49; // Interaction.angle
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}
