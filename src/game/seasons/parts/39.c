#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/39.s.
// PART_39

// PART_39
void s_partCode39_hook(GB *gb) {
  BASE(partCode39);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_6a08; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = PART_BASE + OBJ_YH;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_sub(gb, 0x1a);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x20);
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_SPEED;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+23, s_objectSetVisible80, SYM(objectSetVisible80), b_+26);
  CYC(b_+26, b_+28); A = 0xbf;
  CYC(b_+28, b_+31);
  TAIL(playSound_b00);
L_6a08:
  CYC(b_+31, b_+33); E = 0xd7;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+35, b_+38); TAIL(partDelete); }
  CYC(b_+35, b_+38);
  CALL_C(b_+38, s_func_6a28_hook, SYM(func_6a28), b_+41);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CALL_C(b_+42, s_partCommon_checkOutOfBounds, SYM(partCommon_checkOutOfBounds), b_+45);
  if (F & FZ) { CYCT(b_+45, b_+48); TAIL(partDelete); }
  CYC(b_+45, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wFrameCounter);
  CYC(b_+51, b_+52); alu_rrca(gb);
  if (F & FC) { CYCT(b_+52, b_+54); goto L_6a25; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); E = PART_BASE + OBJ_OAM_FLAGS;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_xor(gb, 0x07);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
L_6a25:
  CYC(b_+60, b_+63);
  TAIL(objectApplySpeed);
}

void s_func_6a28_hook(GB *gb) {
  BASE(func_6a28);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x1a;
  CALL_C(b_+7, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+12); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+12, b_+15); TAIL(partDelete); }
  CYC(b_+12, b_+15);
  CALL_C(b_+15, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+18);
  CYC(b_+18, b_+19); A = alu_dec8(gb, A);
  CYC(b_+19, b_+21); B = 0x01;
  CYC(b_+21, b_+23); alu_cp(gb, 0x17);
  if (F & FZ) { CYCT(b_+23, b_+25); goto L_6a57; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); TAIL(partAnimate); }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+32, b_+34); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+34, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+37);
  CYC(b_+37, b_+39); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); A = 0xbe;
  CALL_C(b_+42, s_playSound, SYM(playSound_b00), b_+45);
  CYC(b_+45, b_+47); B = 0x02;
L_6a57:
  CYC(b_+47, b_+48); A = B;
  CALL_C(b_+48, s_partSetAnimation, SYM(partSetAnimation), b_+51);
  CYC(b_+51, b_+52); alu_or(gb, D);
  RET(b_+52); return;
}

