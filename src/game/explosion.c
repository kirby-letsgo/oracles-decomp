#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void interactionCode56_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4950, checkInteractionState_hook, 0x23fe, 0x4953);
  if (F & FZ) {
    CYCT(0x4953, 0x4955); goto L_495f;
  }
  CYC(0x4953, 0x4955);
  CYC(0x4955, 0x4957); E = 0x61;
  CYC(0x4957, 0x4958); A = mem_rd(gb, DE);
  CYC(0x4958, 0x4959); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4959, 0x495c); interactionAnimate_hook(gb); return;
  }
  CYC(0x4959, 0x495c);
  CYC(0x495c, 0x495f); interactionDelete_hook(gb); return;
L_495f:
  CYC(0x495f, 0x4960); A = alu_inc8(gb, A);
  CYC(0x4960, 0x4961); mem_wr(gb, DE, A);
  CALL_C(0x4961, interactionInitGraphics_hook, 0x15fb, 0x4964);
  CYC(0x4964, 0x4966); A = 0x6f;
  CALL_C(0x4966, playSound_b00_hook, 0x0c98, 0x4969);
  CYC(0x4969, 0x496b); E = 0x43;
  CYC(0x496b, 0x496c); A = mem_rd(gb, DE);
  CYC(0x496c, 0x496d); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x496d, 0x4970); objectSetVisible81_hook(gb); return;
  }
  CYC(0x496d, 0x4970);
  CYC(0x4970, 0x4973); objectSetVisible82_hook(gb); return;
}
