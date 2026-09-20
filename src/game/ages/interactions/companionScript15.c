#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void companionScript_makeExclamationMark_hook(GB *gb);

void companionScript_noticeLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6dad, 0x6daf); E = 0x78;
  CYC(0x6daf, 0x6db0); A = mem_rd(gb, DE);
  CYC(0x6db0, 0x6db1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6db1, 0x6db3); companionScript_makeExclamationMark_hook(gb); return; }
  CYC(0x6db1, 0x6db3);
  CYC(0x6db3, 0x6db6); mem_wr(gb, 0xd13f, A);
  companionScript_makeExclamationMark_hook(gb);
}

void companionScript_makeExclamationMark_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6db6, 0x6db9); SET_BC(0xf000);
  CYC(0x6db9, 0x6dbb); A = 0x1e;
  CYC(0x6dbb, 0x6dbe); objectCreateExclamationMark_hook(gb);
}

void companionScript_writeAngleTowardLinkToCompanionVar3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6dbe, objectGetAngleTowardLink_hook, 0x1e9c, 0x6dc1);
  CYC(0x6dc1, 0x6dc3); E = 0x49;
  CALL_C(0x6dc3, convertAngleToDirection_hook, 0x26f9, 0x6dc6);
  CYC(0x6dc6, 0x6dc8); alu_add(gb, 0x01);
  CYC(0x6dc8, 0x6dcb); mem_wr(gb, 0xd13f, A);
  CYC(0x6dcb, 0x6dcc); ret_effect(gb);
}

void companionScript_restoreMusic_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6dcc, 0x6dcf); A = mem_rd(gb, 0xcc46);
  CYC(0x6dcf, 0x6dd2); mem_wr(gb, 0xcc35, A);
  CYC(0x6dd2, 0x6dd5); playSound_b00_hook(gb);
}

void companionScript_spawnFairyAfterFindingCompanionInForest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6dd5, 0x6dd8); SET_BC(0x4903);
  CALL_C(0x6dd8, objectCreateInteraction_hook, 0x24c5, 0x6ddb);
  CYC(0x6ddb, 0x6ddd); L = 0x43;
  CYC(0x6ddd, 0x6ddf); mem_wr(gb, HL, 0x0f);
  CYC(0x6ddf, 0x6de0); ret_effect(gb);
}

void companionScript_warpOutOfForest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6de0, 0x6de3); SET_HL(0x6de6);
  CYC(0x6de3, 0x6de6); setWarpDestVariables_hook(gb);
}

void companionScript_loseRickyGloves_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6deb, 0x6ded); A = 0x48;
  CYC(0x6ded, 0x6df0); loseTreasure_hook(gb);
}
