#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode7c), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode7c), (from), (to), true)

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
  BASE(enemyCode7c);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); enemyBoss_dead_b0f_hook(gb); return; // jp

normalStatus:
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  {
    uint16_t target = enemyCode7c_jump_table(gb);
    HANDOFF(target);
  }
}
