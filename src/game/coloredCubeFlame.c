#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// wRotatingCubeColor -> OAM palette index for the flame.
#define coloredCubeFlamePalettes_bank08 0x4a6c

static void coloredCubeFlame_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

// interactionCode1a@updateColor: set the flame's OAM palette from wRotatingCubeColor.
static void coloredCubeFlame_updateColor(GB *gb) {
  CYC(0x4a5b, 0x4a5e); A = mem_rd(gb, wRotatingCubeColor);
  CYC(0x4a5e, 0x4a60); alu_and(gb, 0x7f);
  CYC(0x4a60, 0x4a63); SET_HL(coloredCubeFlamePalettes_bank08);
  CYC(0x4a63, 0x4a64); push_effect(gb, 0x4a64); coloredCubeFlame_addAToHl(gb);
  CYC(0x4a64, 0x4a66); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x4a66, 0x4a67); A = mem_rd(gb, DE);
  CYC(0x4a67, 0x4a69); alu_and(gb, 0xf8);
  CYC(0x4a69, 0x4a6a); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4a6a, 0x4a6b); mem_wr(gb, DE, A);
  CYC(0x4a6b, 0x4a6c); ret_effect(gb);
}

// INTERAC_COLORED_CUBE_FLAME: the flame shown on top of the colored cube's current tile.
void interactionCode1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a35, checkInteractionState_hook, 0x23fe, 0x4a38);
  if (!(F & FZ)) {
    CYCT(0x4a38, 0x4a3a); goto initialized;
  }
  CYC(0x4a38, 0x4a3a);
  CYC(0x4a3a, 0x4a3d); A = mem_rd(gb, wRotatingCubePos);
  CYC(0x4a3d, 0x4a3e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4a3e, 0x4a3f); ret_effect(gb); return;
  }
  CYC(0x4a3e, 0x4a3f);
  CYC(0x4a3f, 0x4a42); push_effect(gb, 0x4a42); coloredCubeFlame_updateColor(gb);
  CALL_C(0x4a42, interactionInitGraphics_hook, 0x15fb, 0x4a45);
  CALL_C(0x4a45, objectSetVisible82_hook, 0x1e69, 0x4a48);
  CALL_C(0x4a48, interactionIncState_hook, 0x23e0, 0x4a4b);

initialized:
  CYC(0x4a4b, 0x4a4e); A = mem_rd(gb, wRotatingCubeColor);
  CYC(0x4a4e, 0x4a4f); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x4a4f, 0x4a52); objectSetInvisible_hook(gb); return;
  }
  CYC(0x4a4f, 0x4a52);
  CALL_C(0x4a52, objectSetVisible_hook, 0x1e84, 0x4a55);
  CYC(0x4a55, 0x4a58); push_effect(gb, 0x4a58); coloredCubeFlame_updateColor(gb);
  CYC(0x4a58, 0x4a5b); interactionAnimate_hook(gb);
}
