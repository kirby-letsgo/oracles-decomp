#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

// INTERAC_OCTOGON_SPLASH
void interactionCode8e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c39, 0x6c3b); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6c3b, 0x6c3c); A = mem_rd(gb, DE);
  CYC(0x6c3c, 0x6c3d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6c3d, 0x6c3f); goto state0; } // jr z
  CYC(0x6c3d, 0x6c3f);

// interactionCode8e@state1
  CYC(0x6c3f, 0x6c41); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6c41, 0x6c42); A = mem_rd(gb, DE);
  CYC(0x6c42, 0x6c43); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x6c43, 0x6c46); interactionAnimate_hook(gb); return; } // jp nz
  CYC(0x6c43, 0x6c46);
  CYC(0x6c46, 0x6c49); interactionDelete_hook(gb); return; // jp

state0:
  CALL_C(0x6c49, interactionInitGraphics_hook, 0x15fb, 0x6c4c);
  CALL_C(0x6c4c, interactionIncState_hook, 0x23e0, 0x6c4f);
  CYC(0x6c4f, 0x6c51); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6c51, 0x6c52); A = mem_rd(gb, HL);
  CYC(0x6c52, 0x6c53); alu_rrca(gb);
  CYC(0x6c53, 0x6c54); alu_rrca(gb);
  CALL_C(0x6c54, interactionSetAnimation_hook, 0x262e, 0x6c57);
  CYC(0x6c57, 0x6c5a); objectSetVisible81_hook(gb); return; // jp
}
