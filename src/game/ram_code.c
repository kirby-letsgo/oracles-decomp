#include "game/asm.h"
#include "game/gen.h"

void wRamFunction(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (gb->wram[0][0x4b7] != 0xc3) {
    int saved = hook_mode;
    hook_mode = HOOK_MODE_OFF;
    gb->pc = 0xc4b7;
    gb->step(gb);
    hook_mode = saved;
    return;
  }
  uint16_t target = (uint16_t)(gb->wram[0][0x4b8] | (gb->wram[0][0x4b9] << 8));
  I(0xc4b7, 4);
  HANDOFF(target);
}
