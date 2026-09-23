#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_INGO), bank 0x15.

// Speed $100 with a -$200 jump, and the jump sound.
void s_ingo_animatePlaySound_hook(GB *gb) {
  BASE(ingo_animatePlaySound);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x50; // Interaction.speed
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+5, b_+7); L = 0x54; // Interaction.speedZ
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+10); SET_HL(HL + 1);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xfe);
  CYC(b_+12, b_+14); A = 0x53; // SND_JUMP
  CYC(b_+14, b_+17);
  TAIL(playSound_b00);
}

// Applies the jump with gravity $30; var3d = 1 on landing.
void s_ingo_jump_hook(GB *gb) {
  BASE(ingo_jump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x7d; // Interaction.var3d
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x01);
  RET(b_+11); return;
}
