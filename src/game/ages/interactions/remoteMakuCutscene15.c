#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void remoteMakuCutscene_setPaletteFadeFlags(GB *gb) {
  BASE(remoteMakuCutscene_fadeinFromBlackWithDelay);
  CYC(b_+3, b_+5); A = 0xff;
  CYC(b_+5, b_+8); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+8, b_+11); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+16); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+16, b_+18); A = 0xfe;
  CYC(b_+18, b_+21); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void remoteMakuCutscene_fadeoutToBlackWithDelay_hook(GB *gb) {
  BASE(remoteMakuCutscene_fadeoutToBlackWithDelay);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fadeoutToBlackWithDelay_hook, SYM(fadeoutToBlackWithDelay), b_+3);
  CYC(b_+3, b_+5);
  remoteMakuCutscene_setPaletteFadeFlags(gb);
}

void remoteMakuCutscene_fadeinFromBlackWithDelay_hook(GB *gb) {
  BASE(remoteMakuCutscene_fadeinFromBlackWithDelay);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fadeinFromBlackWithDelay_hook, SYM(fadeinFromBlackWithDelay), b_+3);
  remoteMakuCutscene_setPaletteFadeFlags(gb);
}

void remoteMakuCutscene_checkinitUnderwaterWaves_hook(GB *gb) {
  BASE(remoteMakuCutscene_checkinitUnderwaterWaves);
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL((SYM(checkEssenceObtained) + 5));
  CYC(b_+9, b_+11); E = 0x01;
  CYC(b_+11, b_+14); TAIL(interBankCall);
}
