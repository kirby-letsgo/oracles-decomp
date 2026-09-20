#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void enemyCode7c_hook(GB *gb);

static uint16_t enemyCode7c_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// ==================================================================================================
// ENEMY_SMOG
//
// Variables:
//   var03: Phase of fight (0-3)
//   counter2: Stops movement temporarily (when sword collision occurs)
//   var30: "Adjacent walls bitset" (bitset of solid walls around smog, similar to the
//          variable used for special objects)
//   var31: Position of the tile it's "hugging"
//   var32: Number of frames to wait for a wall before disappearing and respawning
//   var33: Original value of "direction" (for subid 2 respawning)
//   var34/var35: Original Y/X position (for subid 2 respawning)
//   var36: Counter until "fire projectile" animation will begin
//
// The jump table below dispatches into smog_* state handlers that live outside this
// cluster's assigned range; they are not yet rewritten, so every dispatch falls back to
// the interpreter via HANDOFF.
// ==================================================================================================

void enemyCode7c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x7085, 0x7087); goto normalStatus; } // jr z
  CYC(0x7085, 0x7087);
  CYC(0x7087, 0x7089); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x7089); return; } // ret c
  CYC(0x7089, 0x708a);
  if (!(F & FZ)) { CYCT(0x708a, 0x708c); goto normalStatus; } // jr nz
  CYC(0x708a, 0x708c);
  CYC(0x708c, 0x708f); enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(0x708f, 0x7091); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7091, 0x7092); A = mem_rd(gb, DE);
  CYC(0x7092, 0x7093); push_effect(gb, 0x7093);
  {
    uint16_t target = enemyCode7c_jump_table(gb);
    HANDOFF(target);
  }
}
