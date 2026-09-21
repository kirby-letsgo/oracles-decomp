#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// wRotatingCubeColor -> OAM palette index for the flame.
#define coloredCubeFlamePalettes_bank08 SYM(interactionCode1a__palettes)

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
  BASE(interactionCode1a);
  CYC(b_+38, b_+41); A = mem_rd(gb, wRotatingCubeColor);
  CYC(b_+41, b_+43); alu_and(gb, 0x7f);
  CYC(b_+43, b_+46); SET_HL(coloredCubeFlamePalettes_bank08);
  CYC(b_+46, b_+47); push_effect(gb, b_+47); coloredCubeFlame_addAToHl(gb);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+52); alu_and(gb, 0xf8);
  CYC(b_+52, b_+53); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); ret_effect(gb);
}

// INTERAC_COLORED_CUBE_FLAME: the flame shown on top of the colored cube's current tile.
void interactionCode1a_hook(GB *gb) {
  BASE(interactionCode1a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto initialized;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); push_effect(gb, b_+13); coloredCubeFlame_updateColor(gb);
  CALL_C(b_+13, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+16);
  CALL_C(b_+16, objectSetVisible82_hook, SYM(objectSetVisible82), b_+19);
  CALL_C(b_+19, interactionIncState_hook, SYM(interactionIncState), b_+22);

initialized:
  CYC(b_+22, b_+25); A = mem_rd(gb, wRotatingCubeColor);
  CYC(b_+25, b_+26); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+26, b_+29); TAIL(objectSetInvisible);
  }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, objectSetVisible_hook, SYM(objectSetVisible), b_+32);
  CYC(b_+32, b_+35); push_effect(gb, b_+35); coloredCubeFlame_updateColor(gb);
  CYC(b_+35, b_+38); interactionAnimate_hook(gb);
}
