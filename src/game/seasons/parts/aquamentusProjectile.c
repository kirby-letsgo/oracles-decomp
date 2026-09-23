#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/aquamentusProjectile.s.
// PART_AQUAMENTUS_PROJECTILE

static uint16_t aquamentusProjectile_jump_table(GB *gb) {
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

// PART_AQUAMENTUS_PROJECTILE
void s_partCode40_hook(GB *gb) {
  BASE(partCode40);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); TAIL(partDelete); }
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (aquamentusProjectile_jump_table(gb));
    if (jt_ == b_+13) goto state0;
    if (jt_ == b_+31) goto state1;
    if (jt_ == b_+51) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+18); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x28);
  CYC(b_+20, b_+22); L = PART_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x50);
  CYC(b_+24, b_+26); E = PART_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); TAIL_S(func_7081); }
  CYC(b_+28, b_+30);
  RET(b_+30); return;
state1:
  CALL_C(b_+31, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+34);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); L = E;
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+39); A = 0x00;
  CALL_C(b_+39, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+42);
  CYC(b_+42, b_+45); SET_BC(0xf0f0);
  CALL_C(b_+45, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+48);
  CYC(b_+48, b_+51);
  TAIL(objectSetVisible80);
state2:
  CALL_C(b_+51, s_objectApplySpeed, SYM(objectApplySpeed), b_+54);
  CALL_C(b_+54, s_partCommon_checkOutOfBounds, SYM(partCommon_checkOutOfBounds), b_+57);
  if (F & FZ) { CYCT(b_+57, b_+60); TAIL(partDelete); }
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+63); A = mem_rd(gb, wFrameCounter);
  CYC(b_+63, b_+64); alu_xor(gb, D);
  CYC(b_+64, b_+65); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); E = PART_BASE + OBJ_OAM_FLAGS;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); A = alu_inc8(gb, A);
  CYC(b_+70, b_+72); alu_and(gb, 0x03);
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  RET(b_+73); return;
}

void s_func_7081_hook(GB *gb) {
  BASE(func_7081);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); C = 0x03;
  CALL_C(b_+8, s_func_708e_hook, SYM(func_708e), b_+11);
  CYC(b_+11, b_+13); C = 0xfd;
  s_func_708e_hook(gb); return; // falls through
}

void s_func_708e_hook(GB *gb) {
  BASE(func_708e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x40);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+8, s_objectCopyPosition, SYM(objectCopyPosition), b_+11);
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+13, b_+14); E = L;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, C);
  CYC(b_+16, b_+18); alu_and(gb, 0x1f);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+21, b_+22); E = L;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); E = L;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  RET(b_+27); return;
}

