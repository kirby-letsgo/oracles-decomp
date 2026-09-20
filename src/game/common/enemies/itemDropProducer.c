#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ==================================================================================================
// ENEMY_ITEM_DROP_PRODUCER
//
// Variables:
//   var30: Tile at position (item drop will spawn when this changes)
// ==================================================================================================
void enemyCode59_hook(GB *gb) {
  BASE(enemyCode59);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto state1; } // jr nz
  CYC(b_+4, b_+6);

  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CALL_C(b_+9, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);

state1:
  CALL_C(b_+15, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+18);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+21, b_+22); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+22); return; } // ret z
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CALL_C(b_+26, checkItemDropAvailable_hook, SYM(checkItemDropAvailable), b_+29);
  if (F & FZ) { CYCT(b_+29, b_+32); enemyDelete_hook(gb); return; } // jp z
  CYC(b_+29, b_+32);
  CALL_C(b_+32, getFreePartSlot_hook, SYM(getFreePartSlot), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; } // ret nz
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x01);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CALL_C(b_+43, objectCopyPosition_hook, SYM(objectCopyPosition), b_+46);
  CALL_C(b_+46, markEnemyAsKilledInRoom_b00_hook, SYM(markEnemyAsKilledInRoom_b00), b_+49);
  CYC(b_+49, b_+52); enemyDelete_hook(gb); return; // jp
}
