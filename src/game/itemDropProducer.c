#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

// ==================================================================================================
// ENEMY_ITEM_DROP_PRODUCER
//
// Variables:
//   var30: Tile at position (item drop will spawn when this changes)
// ==================================================================================================
void enemyCode59_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x685f, 0x6861); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6861, 0x6862); A = mem_rd(gb, DE);
  CYC(0x6862, 0x6863); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6863, 0x6865); goto state1; } // jr nz
  CYC(0x6863, 0x6865);

  CYC(0x6865, 0x6867); A = 0x01;
  CYC(0x6867, 0x6868); mem_wr(gb, DE, A);
  CALL_C(0x6868, objectGetTileAtPosition_hook, 0x1444, 0x686b);
  CYC(0x686b, 0x686d); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x686d, 0x686e); mem_wr(gb, DE, A);

state1:
  CALL_C(0x686e, objectGetTileAtPosition_hook, 0x1444, 0x6871);
  CYC(0x6871, 0x6872); H = D;
  CYC(0x6872, 0x6874); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6874, 0x6875); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6875); return; } // ret z
  CYC(0x6875, 0x6876);
  CYC(0x6876, 0x6878); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6878, 0x6879); A = mem_rd(gb, DE);
  CALL_C(0x6879, checkItemDropAvailable_hook, 0x1703, 0x687c);
  if (F & FZ) { CYCT(0x687c, 0x687f); enemyDelete_hook(gb); return; } // jp z
  CYC(0x687c, 0x687f);
  CALL_C(0x687f, getFreePartSlot_hook, 0x3e8e, 0x6882);
  if (!(F & FZ)) { RET_TAKEN(0x6882); return; } // ret nz
  CYC(0x6882, 0x6883);
  CYC(0x6883, 0x6885); mem_wr(gb, HL, 0x01);
  CYC(0x6885, 0x6886); L = alu_inc8(gb, L);
  CYC(0x6886, 0x6888); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6888, 0x6889); A = mem_rd(gb, DE);
  CYC(0x6889, 0x688a); mem_wr(gb, HL, A);
  CALL_C(0x688a, objectCopyPosition_hook, 0x2242, 0x688d);
  CALL_C(0x688d, markEnemyAsKilledInRoom_b00_hook, 0x320d, 0x6890);
  CYC(0x6890, 0x6893); enemyDelete_hook(gb); return; // jp
}
