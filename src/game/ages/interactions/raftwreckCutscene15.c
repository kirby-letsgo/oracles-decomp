#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void raftwreckCutscene_spawnHelperSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x764a, 0x764b); B = A;
  CALL_C(0x764b, getFreeInteractionSlot_hook, 0x3aef, 0x764e);
  if (!(F & FZ)) { RET_TAKEN(0x764e); return; }
  CYC(0x764e, 0x764f);
  CYC(0x764f, 0x7651); mem_wr(gb, HL, 0x64);
  CYC(0x7651, 0x7652); L = alu_inc8(gb, L);
  CYC(0x7652, 0x7653); mem_wr(gb, HL, B);
  CYC(0x7653, 0x7654); ret_effect(gb);
}
