#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/46.s.
// PART_46

static uint16_t part46_jump_table(GB *gb) {
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

// PART_46
void s_partCode46_hook(GB *gb) {
  BASE(partCode46);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); goto delete; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_SUBID;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto subid0; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = PART_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (part46_jump_table(gb));
    if (jt_ == b_+132) goto subid1;
    if (jt_ == b_+153) goto subid1_state1;
    if (jt_ == b_+160) goto subid1_state2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+18, b_+20); A = 0x29;
  CALL_C(b_+20, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+25, b_+27); goto delete; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto delete; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); E = PART_BASE + OBJ_STATE;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); push_effect(gb, b_+43);
  do { uint16_t jt_ = (part46_jump_table(gb));
    if (jt_ == b_+53) goto subid0_state0;
    if (jt_ == b_+60) goto subid0_state1;
    if (jt_ == b_+66) goto subid0_state2;
    if (jt_ == b_+100) goto subid0_state3;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+55); L = E;
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+56, b_+58); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x1e);
subid0_state1:
  CALL_C(b_+60, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+63);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+65); L = E;
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
subid0_state2:
  CYC(b_+66, b_+68); B = 0x03;
  CALL_C(b_+68, s_func_7517_hook, SYM(func_7517), b_+71);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+73); A = B;
  CYC(b_+73, b_+75); alu_sub(gb, 0x08);
  CYC(b_+75, b_+77); alu_and(gb, 0x1f);
  CYC(b_+77, b_+78); B = A;
  CALL_C(b_+78, s_func_74fd_hook, SYM(func_74fd), b_+81);
  CALL_C(b_+81, s_func_74fd_hook, SYM(func_74fd), b_+84);
  CALL_C(b_+84, s_func_74fd_hook, SYM(func_74fd), b_+87);
  CYC(b_+87, b_+89); A = 0xba;
  CALL_C(b_+89, s_playSound, SYM(playSound_b00), b_+92);
  CYC(b_+92, b_+93); H = D;
  CYC(b_+93, b_+95); L = PART_BASE + OBJ_STATE;
  CYC(b_+95, b_+96); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+96, b_+98); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x1e);
subid0_state3:
  CALL_C(b_+100, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+103);
  if (!(F & FZ)) { RET_TAKEN(b_+103); return; }
  CYC(b_+103, b_+104);
  CYC(b_+104, b_+105); L = E;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+108); B = 0x02;
  CALL_C(b_+108, s_func_7517_hook, SYM(func_7517), b_+111);
  if (!(F & FZ)) { RET_TAKEN(b_+111); return; }
  CYC(b_+111, b_+112);
  CYC(b_+112, b_+113); A = B;
  CYC(b_+113, b_+115); alu_sub(gb, 0x06);
  CYC(b_+115, b_+117); alu_and(gb, 0x1f);
  CYC(b_+117, b_+118); B = A;
  CALL_C(b_+118, s_func_74fd_hook, SYM(func_74fd), b_+121);
  CALL_C(b_+121, s_func_74fd_hook, SYM(func_74fd), b_+124);
  CYC(b_+124, b_+126); A = 0xba;
  CALL_C(b_+126, s_playSound, SYM(playSound_b00), b_+129);
delete:
  CYC(b_+129, b_+132);
  TAIL(partDelete);
subid1:
  CYC(b_+132, b_+133); H = D;
  CYC(b_+133, b_+134); L = E;
  CYC(b_+134, b_+135); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+135, b_+137); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+137, b_+139); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+139, b_+141); L = PART_BASE + OBJ_SPEED;
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x64);
  CYC(b_+143, b_+145); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x08);
  CALL_C(b_+147, s_func_7524_hook, SYM(func_7524), b_+150);
  CALL_C(b_+150, s_objectSetVisible82, SYM(objectSetVisible82), b_+153);
subid1_state1:
  CALL_C(b_+153, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+156);
  if (!(F & FZ)) { CYCT(b_+156, b_+158); goto L_74f7; }
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+159); L = E;
  CYC(b_+159, b_+160); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
subid1_state2:
  CALL_C(b_+160, s_objectCheckSimpleCollision, SYM(objectCheckSimpleCollision), b_+163);
  if (!(F & FZ)) { CYCT(b_+163, b_+165); goto delete; }
  CYC(b_+163, b_+165);
L_74f7:
  CALL_C(b_+165, s_objectApplySpeed, SYM(objectApplySpeed), b_+168);
  CYC(b_+168, b_+171);
  TAIL(partAnimate);
}

void s_func_74fd_hook(GB *gb) {
  BASE(func_74fd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x46);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+13); alu_add(gb, 0x04);
  CYC(b_+13, b_+15); alu_and(gb, 0x1f);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+19); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+19, b_+20); E = L;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); E = L;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  RET(b_+25); return;
}

void s_func_7517_hook(GB *gb) {
  BASE(func_7517);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, s_func_7524_hook, SYM(func_7524), b_+7);
  CALL_C(b_+7, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+10);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); alu_xor(gb, A);
  RET(b_+12); return;
}

void s_func_7524_hook(GB *gb) {
  BASE(func_7524);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0a00);
  CALL_C(b_+8, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

