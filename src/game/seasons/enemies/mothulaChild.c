#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/mothulaChild.s.
// ENEMY_MOTHULA_CHILD

static uint16_t mothulaChild_jump_table(GB *gb) {
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

// ENEMY_MOTHULA_CHILD
void s_enemyCode47_hook(GB *gb) {
  BASE(enemyCode47);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_6fb2; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(enemyDie_uncounted); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); TAIL(ecom_updateKnockbackNoSolidity_b0d); }
  CYC(b_+9, b_+12);
  RET(b_+12); return;
L_6fb2:
  CALL_C(b_+13, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto L_6fc8; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); push_effect(gb, b_+19);
  do { uint16_t jt_ = (mothulaChild_jump_table(gb));
    if (jt_ == b_+45) goto state0;
    if (jt_ == b_+55) goto state1;
    if (jt_ == b_+131) goto state_stub;
    if (jt_ == b_+124) goto state5;
    HANDOFF(HL);
  } while (0);
L_6fc8:
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+37); push_effect(gb, b_+37);
  do { uint16_t jt_ = (mothulaChild_jump_table(gb));
    if (jt_ == b_+132) goto subid0;
    if (jt_ == b_+190) goto subid1;
    if (jt_ == b_+197) goto subid2;
    if (jt_ == b_+204) goto subid3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+45, b_+47); alu_bit(gb, 7, B);
  CYC(b_+47, b_+49); A = 0x46;
  if (F & FZ) { CYCT(b_+49, b_+52); TAIL(ecom_setSpeedAndState8AndVisible_b0d); }
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+54); A = 0x01;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
state1:
  CYC(b_+55, b_+57); alu_bit(gb, 0, B);
  CYC(b_+57, b_+60); SET_BC(0x0400);
  if (F & FZ) { CYCT(b_+60, b_+62); goto L_6fe6; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+65); SET_BC(0x0604);
L_6fe6:
  CYC(b_+65, b_+66); push_effect(gb, BC);
  CALL_C(b_+66, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+69);
  CYC(b_+69, b_+70); SET_BC(pop_effect(gb));
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+72); A = B;
  CYC(b_+72, b_+74); mem_wr(gb, hFF8B, A);
  CYC(b_+74, b_+75); A = C;
  CYC(b_+75, b_+78); SET_BC(b_+114 /* @seasonsTable_0d_7017 */);
  CALL_C(b_+78, s_addAToBc, SYM(addAToBc), b_+81);
L_6ff6:
  CYC(b_+81, b_+82); push_effect(gb, BC);
  CYC(b_+82, b_+84); B = 0x47;
  CALL_C(b_+84, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+87);
  CYC(b_+87, b_+88); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+88, s_objectCopyPosition, SYM(objectCopyPosition), b_+91);
  CYC(b_+91, b_+92); L = alu_dec8(gb, L);
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x00);
  CYC(b_+95, b_+97); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+97, b_+98); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
  CYC(b_+99, b_+100); SET_BC(pop_effect(gb));
  CYC(b_+100, b_+102); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+102, b_+103); A = mem_rd(gb, BC);
  CYC(b_+103, b_+104); mem_wr(gb, HL, A);
  CYC(b_+104, b_+105); SET_BC(BC + 1);
  CYC(b_+105, b_+108); SET_HL(hFF8B);
  CYC(b_+108, b_+109); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+109, b_+111); goto L_6ff6; }
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+114);
  TAIL(enemyDelete);

state5:
  CALL_C(b_+124, s_ecom_galeSeedEffect, SYM(ecom_galeSeedEffect_b0d), b_+127);
  if (F & FC) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+131);
  TAIL(enemyDelete);
state_stub:
  RET(b_+131); return;
subid0:
  CYC(b_+132, b_+133); A = mem_rd(gb, DE);
  CYC(b_+133, b_+135); alu_sub(gb, 0x08);
  CYC(b_+135, b_+136); push_effect(gb, b_+136);
  do { uint16_t jt_ = (mothulaChild_jump_table(gb));
    if (jt_ == b_+140) goto subid0_state8;
    if (jt_ == b_+160) goto subid0_state9;
    HANDOFF(HL);
  } while (0);
subid0_state8:
  CYC(b_+140, b_+141); H = D;
  CYC(b_+141, b_+142); L = E;
  CYC(b_+142, b_+143); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+143, b_+145); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+145, b_+147); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+147, b_+149); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+149, b_+151); mem_wr(gb, HL, 0x04);
  CYC(b_+151, b_+152); L = alu_inc8(gb, L);
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+154, s_seasonsFunc_0d_7089_hook, SYM(seasonsFunc_0d_7089), b_+157);
  CYC(b_+157, b_+160);
  TAIL(objectSetVisible82);
subid0_state9:
  CALL_C(b_+160, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+163);
  if (F & FZ) { CYCT(b_+163, b_+165); goto L_705a; }
  CYC(b_+163, b_+165);
  CALL_C(b_+165, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+168);
  if (!(F & FZ)) { CYCT(b_+168, b_+170); goto L_705a; }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+172); mem_wr(gb, HL, 0x04);
  CALL_C(b_+172, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+175);
  CALL_C(b_+175, s_objectNudgeAngleTowards, SYM(objectNudgeAngleTowards), b_+178);
  CALL_C(b_+178, s_seasonsFunc_0d_7089_hook, SYM(seasonsFunc_0d_7089), b_+181);
L_705a:
  CALL_C(b_+181, s_objectApplySpeed, SYM(objectApplySpeed), b_+184);
  CALL_C(b_+184, s_seasonsFunc_0d_7078_hook, SYM(seasonsFunc_0d_7078), b_+187);
  CYC(b_+187, b_+190);
  TAIL(enemyAnimate);
subid1:
  CYC(b_+190, b_+191); A = mem_rd(gb, DE);
  CYC(b_+191, b_+193); alu_sub(gb, 0x08);
  CYC(b_+193, b_+194); push_effect(gb, b_+194);
  do { uint16_t jt_ = (mothulaChild_jump_table(gb));
    if (jt_ == b_+196) goto ret1;
    HANDOFF(HL);
  } while (0);
ret1:
  RET(b_+196); return;
subid2:
  CYC(b_+197, b_+198); A = mem_rd(gb, DE);
  CYC(b_+198, b_+200); alu_sub(gb, 0x08);
  CYC(b_+200, b_+201); push_effect(gb, b_+201);
  do { uint16_t jt_ = (mothulaChild_jump_table(gb));
    if (jt_ == b_+203) goto ret2;
    HANDOFF(HL);
  } while (0);
ret2:
  RET(b_+203); return;
subid3:
  CYC(b_+204, b_+205); A = mem_rd(gb, DE);
  CYC(b_+205, b_+207); alu_sub(gb, 0x08);
  CYC(b_+207, b_+208); push_effect(gb, b_+208);
  do { uint16_t jt_ = (mothulaChild_jump_table(gb));
    if (jt_ == b_+210) goto ret3;
    HANDOFF(HL);
  } while (0);
ret3:
  RET(b_+210); return;
}

void s_seasonsFunc_0d_7078_hook(GB *gb) {
  BASE(seasonsFunc_0d_7078);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xb8);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto L_7085; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_cp(gb, 0xf8);
  if (F & FC) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
L_7085:
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+17);
  TAIL(enemyDelete);
}

void s_seasonsFunc_0d_7089_hook(GB *gb) {
  BASE(seasonsFunc_0d_7089);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+6); alu_add(gb, 0x02);
  CYC(b_+6, b_+8); alu_and(gb, 0x1c);
  CYC(b_+8, b_+9); alu_rrca(gb);
  CYC(b_+9, b_+10); alu_rrca(gb);
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16);
  TAIL(enemySetAnimation);
}

