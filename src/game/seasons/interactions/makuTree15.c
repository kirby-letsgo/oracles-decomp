#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MAKU_TREE), bank 0x15.

// Sets wTmpcfc0 when Link's sword (not a companion) hits the gate.
void s_makuTree_checkGateHit_hook(GB *gb) {
  BASE(makuTree_checkGateHit);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1WeaponItem_id);
  CYC(b_+3, b_+5); alu_cp(gb, 0x05); // ITEM_SWORD
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wcc63);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+14);
  if (!(F & FC)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wTmpcfc0, A);
  RET(b_+20); return;
}
