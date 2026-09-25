#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/facadeHoleMaker.s.
// PART_FACADE_HOLE_MAKER
// Facade spawns this to make temporary holes under link.
// animParameter has the following values, which are critical to the function of this part:
// $00 (24 frames)  : animation only.
// $01 (90 frames)  : change the tile to hole, storing current tile in var30.
// $02 (10 frames)  : revert the tile to original.
// $00 (20 frames)  : animation only.
// $ff (128 frames) : despawn.
// Variables:
// var30: Used to store the kind of tile that has been replaced by a hole, so that it can be reset later.

// partCode2e@modifyTile
static void facadeHoleMaker_modifyTile(GB *gb) {
  BASE(partCode2e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+49, b_+50); push_effect(gb, AF);
  CYC(b_+50, b_+51); alu_xor(gb, A);
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CALL_C(b_+52, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+55);
  CYC(b_+55, b_+56); SET_AF(pop_effect(gb));
  CYC(b_+56, b_+58); E = PART_BASE + OBJ_VAR30;
  CYC(b_+58, b_+59); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+59, b_+61); goto modifyTile_makeHole; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  RET(b_+63); return;
modifyTile_makeHole:
  CYC(b_+64, b_+65); A = mem_rd(gb, HL);
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0xf3);
  RET(b_+68); return;
}

// PART_FACADE_HOLE_MAKER
// Facade spawns this to make temporary holes under link.
// animParameter has the following values, which are critical to the function of this part:
// $00 (24 frames)  : animation only.
// $01 (90 frames)  : change the tile to hole, storing current tile in var30.
// $02 (10 frames)  : revert the tile to original.
// $00 (20 frames)  : animation only.
// $ff (128 frames) : despawn.
// Variables:
// var30: Used to store the kind of tile that has been replaced by a hole, so that it can be reset later.
void s_partCode2e_hook(GB *gb) {
  BASE(partCode2e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto initialize; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto onlyAnimate; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); TAIL(partDelete); }
  CYC(b_+14, b_+17);
  CALL_L(b_+17, facadeHoleMaker_modifyTile, b_+20);
onlyAnimate:
  CYC(b_+20, b_+23);
  TAIL(partAnimate);
initialize:
  CYC(b_+23, b_+25); A = 0x01;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CALL_C(b_+26, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+29);
  CYC(b_+29, b_+31); alu_cp(gb, 0xf3);
  if (F & FZ) { CYCT(b_+31, b_+34); TAIL(partDelete); }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+36); H = 0xce;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+41); TAIL(partDelete); }
  CYC(b_+38, b_+41);
  CYC(b_+41, b_+43); A = 0x98;
  CALL_C(b_+43, s_playSound, SYM(playSound_b00), b_+46);
  CYC(b_+46, b_+49);
  TAIL(objectSetVisible83);
}

