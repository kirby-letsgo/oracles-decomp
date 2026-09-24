#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/rollingSpikeTrap.s.
// ENEMY_ROLLING_SPIKE_TRAP

static uint16_t rollingSpikeTrap_jump_table(GB *gb) {
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

static void rollingSpikeTrap_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void rollingSpikeTrap_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_ROLLING_SPIKE_TRAP
void s_enemyCode0f_hook(GB *gb) {
  BASE(enemyCode0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+1); return; }
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+7);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto L_6836; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (rollingSpikeTrap_jump_table(gb));
    if (jt_ == b_+34) goto state00;
    if (jt_ == b_+50) goto state01;
    if (jt_ == b_+90) goto state_stub;
    HANDOFF(HL);
  } while (0);
L_6836:
  CYC(b_+26, b_+27); A = B;
  CYC(b_+27, b_+29); alu_sub(gb, 0x08);
  CYC(b_+29, b_+30); push_effect(gb, b_+30);
  do { uint16_t jt_ = (rollingSpikeTrap_jump_table(gb));
    if (jt_ == b_+91) goto state08;
    if (jt_ == b_+128) goto state09;
    HANDOFF(HL);
  } while (0);
state00:
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_VAR3E;
  CYC(b_+36, b_+38); A = 0x08;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); alu_sub(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+42, b_+45); TAIL_S(seasonsFunc_0c_68d6); }
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
state01:
  CYC(b_+50, b_+51); A = B;
  CYC(b_+51, b_+54); SET_HL(b_+82 /* @seasonsTable_0c_686e */);
  CYC(b_+54, b_+55); rollingSpikeTrap_add_a_to_hl(gb, b_+55);
  CYC(b_+55, b_+56); B = mem_rd(gb, HL);
  CALL_C(b_+56, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+59);
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CALL_C(b_+60, s_copyVar03ToVar30_hook, SYM(copyVar03ToVar30), b_+63);
  CYC(b_+63, b_+65); B = 0x0f;
  CALL_C(b_+65, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x08);
  CALL_C(b_+70, s_seasonsFunc_0c_68c8_hook, SYM(seasonsFunc_0c_68c8), b_+73);
  CALL_C(b_+73, s_seasonsFunc_0c_68fa_hook, SYM(seasonsFunc_0c_68fa), b_+76);
  CALL_C(b_+76, s_seasonsFunc_0c_6992_hook, SYM(seasonsFunc_0c_6992), b_+79);
  CYC(b_+79, b_+82);
  TAIL(enemyDelete);

state_stub:
  RET(b_+90); return;
state08:
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+94); alu_sub(gb, 0x08);
  CYC(b_+94, b_+95); push_effect(gb, b_+95);
  do { uint16_t jt_ = (rollingSpikeTrap_jump_table(gb));
    if (jt_ == b_+99) goto state08_substate0;
    if (jt_ == b_+118) goto state08_substate1;
    HANDOFF(HL);
  } while (0);
state08_substate0:
  CALL_C(b_+99, s_seasonsFunc_0c_699d_hook, SYM(seasonsFunc_0c_699d), b_+102);
  CYC(b_+102, b_+104); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+104, b_+105); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+105, s_seasonsFunc_0c_69b4_hook, SYM(seasonsFunc_0c_69b4), b_+108);
  CYC(b_+108, b_+110); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+113); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
  CYC(b_+114, b_+115); E = alu_dec8(gb, E);
  CYC(b_+115, b_+117); A = 0x80;
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
state08_substate1:
  CALL_C(b_+118, s_seasonsFunc_0c_69c9_hook, SYM(seasonsFunc_0c_69c9), b_+121);
  CALL_C(b_+121, s_seasonsFunc_0c_69d2_hook, SYM(seasonsFunc_0c_69d2), b_+124);
  if (F & FZ) { RET_TAKEN(b_+124); return; }
  CYC(b_+124, b_+125);
  CYC(b_+125, b_+128);
  TAIL_S(seasonsFunc_0c_69fd);
state09:
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+131); alu_sub(gb, 0x08);
  CYC(b_+131, b_+132); push_effect(gb, b_+132);
  do { uint16_t jt_ = (rollingSpikeTrap_jump_table(gb));
    if (jt_ == b_+136) goto state09_substate0;
    if (jt_ == b_+151) goto state09_substate1;
    HANDOFF(HL);
  } while (0);
state09_substate0:
  CYC(b_+136, b_+137); H = D;
  CYC(b_+137, b_+138); L = E;
  CYC(b_+138, b_+139); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+139, b_+141); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x08);
  CYC(b_+143, b_+145); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+145, b_+147); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+147, b_+148); A = mem_rd(gb, HL);
  CYC(b_+148, b_+149); mem_wr(gb, DE, A);
  CYC(b_+149, b_+151);
  goto L_68c5;
state09_substate1:
  CALL_C(b_+151, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+154);
  if (!(F & FZ)) { CYCT(b_+154, b_+156); goto L_68c2; }
  CYC(b_+154, b_+156);
  CYC(b_+156, b_+158); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+160); mem_wr(gb, HL, A);
  CYC(b_+160, b_+162); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_xor(gb, 0x10);
  CYC(b_+165, b_+166); mem_wr(gb, HL, A);
L_68c2:
  CALL_C(b_+166, s_objectApplySpeed, SYM(objectApplySpeed), b_+169);
L_68c5:
  CYC(b_+169, b_+172);
  TAIL(enemyAnimate);
}

void s_seasonsFunc_0c_68c8_hook(GB *gb) {
  BASE(seasonsFunc_0c_68c8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); L = E;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+14);
  TAIL(objectCopyPosition);
}

void s_seasonsFunc_0c_68d6_hook(GB *gb) {
  BASE(seasonsFunc_0c_68d6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_68ea; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+9); SET_HL(SYM(seasonsTable_0c_68ed));
  CYC(b_+9, b_+10); rollingSpikeTrap_add_a_to_hl(gb, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); A = C;
  CALL_C(b_+15, s_enemySetAnimation, SYM(enemySetAnimation), b_+18);
  CYC(b_+18, b_+20); A = 0x1e;
L_68ea:
  CYC(b_+20, b_+23);
  TAIL(ecom_setSpeedAndState8_b0d);
}

void s_copyVar03ToVar30_hook(GB *gb) {
  BASE(copyVar03ToVar30);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  RET(b_+7); return;
}

void s_seasonsFunc_0c_68fa_hook(GB *gb) {
  BASE(seasonsFunc_0c_68fa);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); C = H;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+8); SET_HL(SYM(seasonsTable_0c_694c));
  CYC(b_+8, b_+9); rollingSpikeTrap_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); H = mem_rd(gb, HL);
  CYC(b_+11, b_+12); L = A;
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR30;
L_6908:
  CYC(b_+14, b_+15); push_effect(gb, HL);
  CYC(b_+15, b_+16); E = alu_inc8(gb, E);
  CYC(b_+16, b_+17); push_effect(gb, DE);
  CALL_C(b_+17, s_seasonsFunc_0c_6925_hook, SYM(seasonsFunc_0c_6925), b_+20);
  CYC(b_+20, b_+21); push_effect(gb, BC);
  CYC(b_+21, b_+23); B = 0x0f;
  CALL_C(b_+23, s_ecom_spawnEnemyWithSubid01, SYM(ecom_spawnEnemyWithSubid01_b0d), b_+26);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x09);
  CYC(b_+28, b_+29); SET_BC(pop_effect(gb));
  CYC(b_+29, b_+30); A = E;
  CYC(b_+30, b_+31); SET_DE(pop_effect(gb));
  CALL_C(b_+31, s_seasonsFunc_0c_692f_hook, SYM(seasonsFunc_0c_692f), b_+34);
  CYC(b_+34, b_+35); SET_HL(pop_effect(gb));
  CYC(b_+35, b_+36); SET_HL(HL + 1);
  CYC(b_+36, b_+37); SET_HL(HL + 1);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto L_6908; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); SET_HL(pop_effect(gb));
  RET(b_+42); return;
}

void s_seasonsFunc_0c_6925_hook(GB *gb) {
  BASE(seasonsFunc_0c_6925);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); E = C;
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, b_+8); C = mem_rd(gb, HL);
  CYC(b_+8, b_+9); SET_HL(HL + 1);
  RET(b_+9); return;
}

void s_seasonsFunc_0c_692f_hook(GB *gb) {
  BASE(seasonsFunc_0c_692f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); L = 0x97;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x80);
  CYC(b_+6, b_+7); A = H;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+10, b_+11); E = L;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+13, b_+14); L = alu_dec8(gb, L);
  CYC(b_+14, b_+15); mem_wr(gb, HL, B);
  CYC(b_+15, b_+16); D = H;
  CYC(b_+16, b_+17); E = L;
  CYC(b_+17, b_+18); A = C;
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CALL_C(b_+21, s_enemySetAnimation, SYM(enemySetAnimation), b_+24);
  CALL_C(b_+24, s_objectSetVisible82, SYM(objectSetVisible82), b_+27);
  CYC(b_+27, b_+28); SET_DE(pop_effect(gb));
  RET(b_+28); return;
}

void s_seasonsFunc_0c_6992_hook(GB *gb) {
  BASE(seasonsFunc_0c_6992);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x04;
  CYC(b_+2, b_+4); L = ENEMY_BASE + OBJ_VAR31;
L_6996:
  CYC(b_+4, b_+5); E = L;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_6996; }
  CYC(b_+8, b_+10);
  RET(b_+10); return;
}

void s_seasonsFunc_0c_699d_hook(GB *gb) {
  BASE(seasonsFunc_0c_699d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_VAR31;
L_69a2:
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+9); C = 0x81;
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+12); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto L_69ae; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); C = E;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
L_69ae:
  CYC(b_+17, b_+19); A = 0xb5;
  CYC(b_+19, b_+20); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_69a2; }
  CYC(b_+20, b_+22);
  RET(b_+22); return;
}

void s_seasonsFunc_0c_69b4_hook(GB *gb) {
  BASE(seasonsFunc_0c_69b4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(seasonsTable_0c_69c1));
  CYC(b_+6, b_+7); rollingSpikeTrap_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return;
}

void s_seasonsFunc_0c_69c9_hook(GB *gb) {
  BASE(seasonsFunc_0c_69c9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0d;
  CALL_C(b_+2, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); E = L;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}

void s_seasonsFunc_0c_69d2_hook(GB *gb) {
  BASE(seasonsFunc_0c_69d2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x09;
  CALL_C(b_+2, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); C = 0xf7;
  CYC(b_+7, b_+9); alu_bit(gb, 4, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_69df; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); C = 0x08;
L_69df:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_add(gb, C);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_add(gb, 0x02);
  CYC(b_+27, b_+28); E = A;
L_69ee:
  CALL_C(b_+28, s_getTileCollisionsAtPosition, SYM(getTileCollisionsAtPosition), b_+31);
  CYC(b_+31, b_+32); A = alu_dec8(gb, A);
  CYC(b_+32, b_+34); alu_cp(gb, 0x0f);
  if (F & FC) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+38); alu_add(gb, 0x10);
  CYC(b_+38, b_+39); B = A;
  CYC(b_+39, b_+40); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto L_69ee; }
  CYC(b_+40, b_+42);
  RET(b_+42); return;
}

void s_seasonsFunc_0c_69fd_hook(GB *gb) {
  BASE(seasonsFunc_0c_69fd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR31;
L_6a00:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); C = 0x81;
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+10); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_6a0e; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); C = 0x86;
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
L_6a0e:
  CYC(b_+17, b_+19); A = 0xb5;
  CYC(b_+19, b_+20); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_6a00; }
  CYC(b_+20, b_+22);
  RET(b_+22); return;
}

