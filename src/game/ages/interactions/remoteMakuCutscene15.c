#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

static void remoteMakuCutscene_setPaletteFadeFlags(GB *gb) {
  CYC(0x7320, 0x7322); A = 0xff;
  CYC(0x7322, 0x7325); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(0x7325, 0x7328); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(0x7328, 0x732a); A = 0x01;
  CYC(0x732a, 0x732d); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(0x732d, 0x732f); A = 0xfe;
  CYC(0x732f, 0x7332); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(0x7332, 0x7333); ret_effect(gb);
}

void remoteMakuCutscene_fadeoutToBlackWithDelay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7318, fadeoutToBlackWithDelay_hook, 0x32ab, 0x731b);
  CYC(0x731b, 0x731d);
  remoteMakuCutscene_setPaletteFadeFlags(gb);
}

void remoteMakuCutscene_fadeinFromBlackWithDelay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x731d, fadeinFromBlackWithDelay_hook, 0x32d1, 0x7320);
  remoteMakuCutscene_setPaletteFadeFlags(gb);
}

void remoteMakuCutscene_checkinitUnderwaterWaves_hook(GB *gb) {
  CYC(0x7333, 0x7335); E = 0x43;
  CYC(0x7335, 0x7336); A = mem_rd(gb, DE);
  CYC(0x7336, 0x7338); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(0x7338); return; }
  CYC(0x7338, 0x7339);
  CYC(0x7339, 0x733c); SET_HL(0x626e);
  CYC(0x733c, 0x733e); E = 0x01;
  CYC(0x733e, 0x7341); interBankCall_hook(gb);
}
