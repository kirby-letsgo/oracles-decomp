#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/enemies/commonCode.s: ecom_seasonsFunc_4446, the same code at
// the same address in each enemy bank ($0c-$0f), so one body serves the four hooks. Burns read
// the mapped bank. With the magnet gloves active, an enemy that can be pulled is pulled toward
// Link (speed $14); otherwise the usual hazard checks.

static void ecom_magnet_pull_or_check_hazards(GB *gb) {
  BASE(ecom_seasonsFunc_4446);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+4, b_+5); alu_or(gb, A);
  CYC(b_+5, b_+6); A = B;
  if (F & FZ) { CYCT(b_+6, b_+9); TAIL(ecom_checkHazards_b0d); }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+12, b_+14); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+18, b_+19); push_effect(gb, AF);
  CALL_C(b_+19, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+22);
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+25); B = 0x14;
  CALL_C(b_+25, s_ecom_applyGivenVelocity, SYM(ecom_applyGivenVelocity_b0d), b_+28);
  CYC(b_+28, b_+29); SET_AF(pop_effect(gb));
  CYC(b_+29, b_+30); alu_or(gb, A);
  RET(b_+30); return;
}

void s_ecom_seasonsFunc_4446_b0c_hook(GB *gb) { ecom_magnet_pull_or_check_hazards(gb); }
void s_ecom_seasonsFunc_4446_b0d_hook(GB *gb) { ecom_magnet_pull_or_check_hazards(gb); }
void s_ecom_seasonsFunc_4446_b0e_hook(GB *gb) { ecom_magnet_pull_or_check_hazards(gb); }
void s_ecom_seasonsFunc_4446_b0f_hook(GB *gb) { ecom_magnet_pull_or_check_hazards(gb); }
