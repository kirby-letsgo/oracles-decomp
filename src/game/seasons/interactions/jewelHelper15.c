#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_JEWEL_HELPER), bank 0x15.

// A puff at the a-th position of table_61ca.
void s_jewelHelper_createPuff_hook(GB *gb) {
  BASE(jewelHelper_createPuff);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_61ca));
  CALL_C(b_+3, addDoubleIndexToBc_hook, SYM(addDoubleIndexToBc), b_+6);
  CALL_C(b_+6, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+12, b_+14); L = 0x4b; // Interaction.yh
  CYC(b_+14, b_+15); A = mem_rd(gb, BC);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); SET_BC(BC + 1);
  CYC(b_+17, b_+19); L = 0x4d; // Interaction.xh
  CYC(b_+19, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  RET(b_+21); return;
}

void s_jewelHelper_createMoldorm_hook(GB *gb) {
  BASE(jewelHelper_createMoldorm);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x4f); // ENEMY_MOLDORM
  CYC(b_+6, b_+8); L = 0x8b; // Enemy.yh
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x30);
  CYC(b_+10, b_+12); L = 0x8d; // Enemy.xh
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x30);
  RET(b_+14); return;
}
