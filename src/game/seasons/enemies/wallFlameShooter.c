#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/wallFlameShooter.s.
// ENEMY_WALL_FLAME_SHOOTER

static uint16_t wallFlameShooter_jump_table(GB *gb) {
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

// ENEMY_WALL_FLAME_SHOOTER
void s_enemyCode5c_hook(GB *gb) {
  BASE(enemyCode5c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+1); return; }
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (wallFlameShooter_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+29) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xfe);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+21, b_+23); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x08);
  RET(b_+28); return;
state1:
  CALL_C(b_+29, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+32);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x10);
  CALL_C(b_+35, s_getFreePartSlot, SYM(getFreePartSlot), b_+38);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x26);
  CYC(b_+41, b_+44); SET_BC(0x0600);
  CYC(b_+44, b_+47);
  TAIL(objectCopyPositionWithOffset);
}

