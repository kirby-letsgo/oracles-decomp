#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/pokey.s.
// ENEMY_POKEY

static uint16_t pokey_jump_table(GB *gb) {
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

static void pokey_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_POKEY
void s_enemyCode11_hook(GB *gb) {
  BASE(enemyCode11);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_6a4a; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_6a3a; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); TAIL_S(pokeyFunc_0c_6ba8); }
  CYC(b_+12, b_+15);
  CALL_C(b_+15, s_ecom_killRelatedObj1, SYM(ecom_killRelatedObj1_b0d), b_+18);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x00);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+24, b_+25); push_effect(gb, HL);
  CYC(b_+25, b_+26); H = mem_rd(gb, HL);
  CALL_C(b_+26, s_ecom_killObjectH, SYM(ecom_killObjectH_b0d), b_+29);
  CYC(b_+29, b_+30); SET_HL(pop_effect(gb));
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+32); H = mem_rd(gb, HL);
  CALL_C(b_+32, s_ecom_killObjectH, SYM(ecom_killObjectH_b0d), b_+35);
  CYC(b_+35, b_+38);
  TAIL(enemyDie);
L_6a3a:
  CYC(b_+38, b_+40); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+43); alu_cp(gb, 0x9a);
  if (F & FZ) CALL_C_CC(b_+43, s_pokeyFunc_0c_6bfe_hook, SYM(pokeyFunc_0c_6bfe), b_+46);
  else CYC(b_+43, b_+46);
  CALL_C(b_+46, s_pokeyFunc_0c_6c3e_hook, SYM(pokeyFunc_0c_6c3e), b_+49);
  CYC(b_+49, b_+51); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
L_6a4a:
  CALL_C(b_+54, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+57);
  if (!(F & FC)) { CYCT(b_+57, b_+59); goto L_6a60; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+60); push_effect(gb, b_+60);
  do { uint16_t jt_ = (pokey_jump_table(gb));
    if (jt_ == SYM(pokey_state_0) && hook_is(gb, SYM(pokey_state_0), s_pokey_state_0_hook)) { s_pokey_state_0_hook(gb); return; }
    if (jt_ == SYM(pokey_state_stub) && hook_is(gb, SYM(pokey_state_stub), s_pokey_state_stub_hook)) { s_pokey_state_stub_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
L_6a60:
  CYC(b_+76, b_+78); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+80); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+80, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+83);
  else CYC(b_+80, b_+83);
  CYC(b_+83, b_+85); A = 0x33;
  CALL_C(b_+85, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+88);
  CYC(b_+88, b_+89); A = mem_rd(gb, HL);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+90); return; }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+92); B = alu_dec8(gb, B);
  CYC(b_+92, b_+93); A = B;
  CYC(b_+93, b_+94); push_effect(gb, b_+94);
  do { uint16_t jt_ = (pokey_jump_table(gb));
    if (jt_ == SYM(pokey_6b05) && hook_is(gb, SYM(pokey_6b05), s_pokey_6b05_hook)) { s_pokey_6b05_hook(gb); return; }
    if (jt_ == SYM(pokey_6b2e) && hook_is(gb, SYM(pokey_6b2e), s_pokey_6b2e_hook)) { s_pokey_6b2e_hook(gb); return; }
    if (jt_ == SYM(pokey_6b35) && hook_is(gb, SYM(pokey_6b35), s_pokey_6b35_hook)) { s_pokey_6b35_hook(gb); return; }
    if (jt_ == SYM(pokey_6b3c) && hook_is(gb, SYM(pokey_6b3c), s_pokey_6b3c_hook)) { s_pokey_6b3c_hook(gb); return; }
    if (jt_ == SYM(pokey_state_stub) && hook_is(gb, SYM(pokey_state_stub), s_pokey_state_stub_hook)) { s_pokey_state_stub_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_pokey_state_0_hook(GB *gb) {
  BASE(pokey_state_0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto L_6ac1; }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); B = 0x04;
  CALL_C(b_+6, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); B = 0x11;
  CALL_C(b_+12, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x05);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+19, b_+21); A = 0x80;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, HL, H);
  CALL_C(b_+23, s_objectCopyPosition, SYM(objectCopyPosition), b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+28, b_+29); mem_wr(gb, HL, H);
  CYC(b_+29, b_+30); C = H;
  CYC(b_+30, b_+32); E = 0x03;
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
L_6a9d:
  CYC(b_+33, b_+34); push_effect(gb, HL);
  CALL_C(b_+34, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+37);
  CYC(b_+37, b_+39); A = 0x04;
  CYC(b_+39, b_+40); alu_sub(gb, E);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+45, b_+47); A = 0x80;
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+48, b_+49); mem_wr(gb, HL, C);
  CYC(b_+49, b_+50); push_effect(gb, DE);
  CALL_C(b_+50, s_objectCopyPosition, SYM(objectCopyPosition), b_+53);
  CYC(b_+53, b_+54); SET_DE(pop_effect(gb));
  CYC(b_+54, b_+55); A = H;
  CYC(b_+55, b_+56); SET_HL(pop_effect(gb));
  CYC(b_+56, b_+57); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+57, b_+58); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+58, b_+60); goto L_6a9d; }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+61); H = A;
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+63, b_+64); E = L;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); mem_wr(gb, HL, A);
  CYC(b_+66, b_+69);
  TAIL(enemyDelete);
L_6ac1:
  CYC(b_+69, b_+71); alu_cp(gb, 0x03);
  CYC(b_+71, b_+73); A = 0x01;
  if (!(F & FZ)) CALL_C_CC(b_+73, s_enemySetAnimation, SYM(enemySetAnimation), b_+76);
  else CYC(b_+73, b_+76);
  CYC(b_+76, b_+78); A = 0x0f;
  CALL_C(b_+78, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+81);
  CYC(b_+81, b_+83); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+83, b_+85); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+85, b_+87); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+87, b_+88); A = mem_rd(gb, HL);
  CYC(b_+88, b_+90); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+90, b_+92); goto L_6af9; }
  CYC(b_+90, b_+92);
  CYC(b_+92, b_+93); B = A;
  CYC(b_+93, b_+95); A = 0x30;
  CALL_C(b_+95, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+98);
  CYC(b_+98, b_+99); E = L;
  CYC(b_+99, b_+100); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CYC(b_+101, b_+102); E = alu_inc8(gb, E);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+105); E = alu_inc8(gb, E);
  CYC(b_+105, b_+106); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+106, b_+107); mem_wr(gb, DE, A);
  CYC(b_+107, b_+108); E = alu_inc8(gb, E);
  CYC(b_+108, b_+109); A = mem_rd(gb, HL);
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CYC(b_+110, b_+111); B = alu_dec8(gb, B);
  if (F & FZ) { CYCT(b_+111, b_+113); goto L_6af6; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); B = alu_dec8(gb, B);
  CYC(b_+114, b_+116); A = 0xf3;
  if (F & FZ) { CYCT(b_+116, b_+118); goto L_6af3; }
  CYC(b_+116, b_+118);
  CYC(b_+118, b_+119); alu_add(gb, A);
L_6af3:
  CYC(b_+119, b_+121); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
L_6af6:
  CYC(b_+122, b_+125);
  TAIL(objectSetVisible82);
L_6af9:
  CYC(b_+125, b_+127); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+127, b_+129); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+129, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+132);
  CYC(b_+132, b_+134); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+134, b_+135); mem_wr(gb, DE, A);
  RET(b_+135); return;
}

void s_pokey_state_stub_hook(GB *gb) {
  BASE(pokey_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

void s_pokey_6b05_hook(GB *gb) {
  BASE(pokey_6b05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_6b18; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); C = 0x0e;
  CALL_C(b_+8, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); TAIL(objectSetVisiblec2); }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
L_6b18:
  CYC(b_+19, b_+21); A = 0x10;
  CALL_C(b_+21, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+24);
  CYC(b_+24, b_+25); E = L;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_and(gb, 0x3f);
  if (F & FZ) CALL_C_CC(b_+32, s_ecom_setRandomAngle, SYM(ecom_setRandomAngle_b0d), b_+35);
  else CYC(b_+32, b_+35);
  CALL_C(b_+35, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+38);
  CYC(b_+38, b_+41);
  TAIL(objectSetPriorityRelativeToLink);
}

void s_pokey_6b2e_hook(GB *gb) {
  BASE(pokey_6b2e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xf3;
  CALL_C(b_+2, s_pokeyFunc_0c_6b8e_hook, SYM(pokeyFunc_0c_6b8e), b_+5);
  CYC(b_+5, b_+7);
  HANDOFF(SYM(pokey_6b3c) + 5); /* pokey_6b3c+5, interpreted */
}

void s_pokey_6b35_hook(GB *gb) {
  BASE(pokey_6b35);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xe6;
  CALL_C(b_+2, s_pokeyFunc_0c_6b8e_hook, SYM(pokeyFunc_0c_6b8e), b_+5);
  CYC(b_+5, b_+7);
  HANDOFF(SYM(pokey_6b3c) + 5); /* pokey_6b3c+5, interpreted */
}

void s_pokey_6b3c_hook(GB *gb) {
  BASE(pokey_6b3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xd9;
  CALL_C(b_+2, s_pokeyFunc_0c_6b8e_hook, SYM(pokeyFunc_0c_6b8e), b_+5);
  CYC(b_+5, b_+7); A = 0x06;
  CALL_C(b_+7, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_and(gb, 0x1c);
  CYC(b_+13, b_+14); alu_rrca(gb);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_sub(gb, 0x02);
  CYC(b_+21, b_+23); A = alu_swap(gb, A);
  CYC(b_+23, b_+24); alu_rrca(gb);
  CYC(b_+24, b_+25); alu_add(gb, B);
  CYC(b_+25, b_+28); SET_HL(SYM(pokeyTable_0c_6b6a));
  CYC(b_+28, b_+29); pokey_add_a_to_hl(gb, b_+29);
  CYC(b_+29, b_+30); B = mem_rd(gb, HL);
  CALL_C(b_+30, s_pokeyFunc_0c_6b82_hook, SYM(pokeyFunc_0c_6b82), b_+33);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+35, b_+36); E = L;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+40); E = L;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+42); alu_add(gb, B);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+46);
  TAIL(objectSetPriorityRelativeToLink);
}

void s_pokeyFunc_0c_6b82_hook(GB *gb) {
  BASE(pokeyFunc_0c_6b82);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR2F;
  CYC(b_+2, b_+4); L = ENEMY_BASE + OBJ_SUBID;
L_6b86:
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); H = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_6b86; }
  CYC(b_+9, b_+11);
  RET(b_+11); return;
}

void s_pokeyFunc_0c_6b8e_hook(GB *gb) {
  BASE(pokeyFunc_0c_6b8e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_6ba1; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); C = 0x0e;
  CALL_C(b_+11, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_cp(gb, B);
  if (F & FC) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
L_6ba1:
  CYC(b_+19, b_+20); mem_wr(gb, HL, B);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  RET(b_+25); return;
}

void s_pokeyFunc_0c_6ba8_hook(GB *gb) {
  BASE(pokeyFunc_0c_6ba8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x33;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_6bbc; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); TAIL(enemyDie_uncounted_withoutItemDrop); }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+20);
  TAIL(enemyDelete);
L_6bbc:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_add(gb, 0xb1);
  CYC(b_+25, b_+26); L = A;
  CYC(b_+26, b_+27); H = D;
  CYC(b_+27, b_+29); C = 0x82;
  CYC(b_+29, b_+31); alu_sub(gb, 0xb3);
  if (F & FZ) { CYCT(b_+31, b_+33); goto L_6bd0; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+34, s_pokeyFunc_0c_6bf5_hook, SYM(pokeyFunc_0c_6bf5), b_+37);
  else CYC(b_+34, b_+37);
  CALL_C(b_+37, s_pokeyFunc_0c_6bf5_hook, SYM(pokeyFunc_0c_6bf5), b_+40);
L_6bd0:
  CALL_C(b_+40, s_pokeyFunc_0c_6bf5_hook, SYM(pokeyFunc_0c_6bf5), b_+43);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x05);
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x05);
  CYC(b_+55, b_+57); B = 0x02;
  CALL_C(b_+57, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto L_6bef; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x80);
  CYC(b_+66, b_+68); A = 0x73;
  CALL_C(b_+68, s_playSound, SYM(playSound_b00), b_+71);
L_6bef:
  CALL_C(b_+71, s_objectSetInvisible, SYM(objectSetInvisible), b_+74);
  CYC(b_+74, b_+77);
  TAIL_S(pokeyFunc_0c_6c3e);
}

void s_pokeyFunc_0c_6bf5_hook(GB *gb) {
  BASE(pokeyFunc_0c_6bf5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = mem_rd(gb, HL);
  CYC(b_+1, b_+2); L = alu_inc8(gb, L);
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+5); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, BC, A);
  RET(b_+8); return;
}

void s_pokeyFunc_0c_6bfe_hook(GB *gb) {
  BASE(pokeyFunc_0c_6bfe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+5); C = 0x82;
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
L_6c04:
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+9); L = alu_dec8(gb, L);
  CYC(b_+9, b_+11); A = 0xaf;
  CYC(b_+11, b_+12); alu_cp(gb, L);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); B = mem_rd(gb, HL);
  CYC(b_+14, b_+15); A = mem_rd(gb, BC);
  CYC(b_+15, b_+17); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_6c04; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); H = E;
  CYC(b_+20, b_+21); push_effect(gb, HL);
  CALL_C(b_+21, s_pokeyFunc_0c_6b82_hook, SYM(pokeyFunc_0c_6b82), b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+26, b_+27); C = L;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); mem_wr(gb, BC, A);
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+31); C = L;
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+33); mem_wr(gb, BC, A);
  CYC(b_+33, b_+35); C = 0x8f;
  CYC(b_+35, b_+36); alu_xor(gb, A);
  CYC(b_+36, b_+37); mem_wr(gb, BC, A);
  CYC(b_+37, b_+39); C = 0xa4;
  CYC(b_+39, b_+40); A = mem_rd(gb, BC);
  CYC(b_+40, b_+42); alu_or(gb, 0x80);
  CYC(b_+42, b_+43); mem_wr(gb, BC, A);
  CYC(b_+43, b_+44); SET_HL(pop_effect(gb));
  CYC(b_+44, b_+46); C = 0x82;
  CYC(b_+46, b_+47); A = H;
  CYC(b_+47, b_+48); mem_wr(gb, BC, A);
  CYC(b_+48, b_+49); H = D;
L_6c2f:
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+52); A = 0xb3;
  CYC(b_+52, b_+53); alu_cp(gb, L);
  if (F & FC) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+55); B = mem_rd(gb, HL);
  CYC(b_+55, b_+56); A = mem_rd(gb, BC);
  CYC(b_+56, b_+58); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+58, b_+60); goto L_6c2f; }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+61); A = alu_inc8(gb, A);
  CYC(b_+61, b_+62); mem_wr(gb, BC, A);
  CYC(b_+62, b_+64);
  goto L_6c2f;
}

void s_pokeyFunc_0c_6c3e_hook(GB *gb) {
  BASE(pokeyFunc_0c_6c3e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0404);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR34;
L_6c45:
  CYC(b_+7, b_+8); E = alu_dec8(gb, E);
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); H = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+13, b_+15); goto L_6c4e; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); B = alu_dec8(gb, B);
L_6c4e:
  CYC(b_+16, b_+17); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_6c45; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+23); SET_BC(SYM(pokeyTable_0c_6c5d));
  CALL_C(b_+23, s_addAToBc, SYM(addAToBc), b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+28, b_+29); A = mem_rd(gb, BC);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  RET(b_+30); return;
}
