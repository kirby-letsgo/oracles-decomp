#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_KING_MOBLIN, INTERAC_MOBLIN),
// bank 0x15.

void s_kingMoblin_func_61eb_hook(GB *gb) {
  BASE(kingMoblin_func_61eb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0x4b; // Interaction.yh
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x30);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x78);
  RET(b_+14); return;
}

void s_moblin_spawnMaskedMoblin_hook(GB *gb) {
  BASE(moblin_spawnMaskedMoblin);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x20); // ENEMY_MASKED_MOBLIN
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x01);
  CYC(b_+9, b_+12);
  TAIL(objectCopyPosition);
}

void s_moblin_spawnSwordMaskedMoblin_hook(GB *gb) {
  BASE(moblin_spawnSwordMaskedMoblin);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x4a); // ENEMY_SWORD_MASKED_MOBLIN
  CYC(b_+6, b_+9);
  TAIL(objectCopyPosition);
}
