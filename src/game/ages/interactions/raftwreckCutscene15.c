#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(raftwreckCutscene_spawnHelperSubid), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(raftwreckCutscene_spawnHelperSubid), (from), (to), true)

void raftwreckCutscene_spawnHelperSubid_hook(GB *gb) {
  BASE(raftwreckCutscene_spawnHelperSubid);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+4);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x64);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, SYM(tokkey_jump)); ret_effect(gb);
}
