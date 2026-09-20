#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(companionScript_noticeLink), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(companionScript_noticeLink), (from), (to), true)

void companionScript_makeExclamationMark_hook(GB *gb);

void companionScript_noticeLink_hook(GB *gb) {
  BASE(companionScript_noticeLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x78;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); companionScript_makeExclamationMark_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, SYM(companionScript_makeExclamationMark)); mem_wr(gb, w1Companion_var3f, A);
  companionScript_makeExclamationMark_hook(gb);
}

void companionScript_makeExclamationMark_hook(GB *gb) {
  BASE(companionScript_makeExclamationMark);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf000);
  CYC(b_+3, b_+5); A = 0x1e;
  CYC(b_+5, SYM(companionScript_writeAngleTowardLinkToCompanionVar3f)); objectCreateExclamationMark_hook(gb);
}

void companionScript_writeAngleTowardLinkToCompanionVar3f_hook(GB *gb) {
  BASE(companionScript_writeAngleTowardLinkToCompanionVar3f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+5); E = 0x49;
  CALL_C(b_+5, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+8);
  CYC(b_+8, b_+10); alu_add(gb, 0x01);
  CYC(b_+10, b_+13); mem_wr(gb, w1Companion_var3f, A);
  CYC(b_+13, SYM(companionScript_restoreMusic)); ret_effect(gb);
}

void companionScript_restoreMusic_hook(GB *gb) {
  BASE(companionScript_restoreMusic);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+3, b_+6); mem_wr(gb, wActiveMusic, A);
  CYC(b_+6, SYM(companionScript_spawnFairyAfterFindingCompanionInForest)); playSound_b00_hook(gb);
}

void companionScript_spawnFairyAfterFindingCompanionInForest_hook(GB *gb) {
  BASE(companionScript_spawnFairyAfterFindingCompanionInForest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC((SYM(group2ObjectDataTable) + 456));
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  CYC(b_+6, b_+8); L = 0x43;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x0f);
  CYC(b_+10, SYM(companionScript_warpOutOfForest)); ret_effect(gb);
}

void companionScript_warpOutOfForest_hook(GB *gb) {
  BASE(companionScript_warpOutOfForest);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+6);
  CYC(b_+3, b_+6); setWarpDestVariables_hook(gb);
}

void companionScript_loseRickyGloves_hook(GB *gb) {
  BASE(companionScript_loseRickyGloves);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x48;
  CYC(b_+2, SYM(companionScript_subid0bScript_body)); loseTreasure_hook(gb);
}
