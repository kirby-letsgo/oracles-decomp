#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/syger.s.
// ENEMY_SYGER

static uint16_t syger_jump_table(GB *gb) {
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

static void syger_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void syger_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_SYGER
void s_enemyCode74_hook(GB *gb) {
  BASE(enemyCode74);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x01);
  if (F & FZ) { CYCT(b_+7, b_+9); goto justHit; }
  CYC(b_+7, b_+9);
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto normalStatus; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto normalStatus; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x04;
  CALL_C(b_+19, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+22);
  CYC(b_+22, b_+24); A = 0x0a;
  CYC(b_+24, b_+25); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+25, b_+27); goto L_5171; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); A = 0xd1;
  CALL_C(b_+30, s_playSound, SYM(playSound_b00), b_+33);
L_5171:
  CYC(b_+33, b_+36);
  TAIL(enemyBoss_dead_b0f);
justHit:
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+40, b_+42); goto normalStatus; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); A = 0x2b;
  CALL_C(b_+44, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+47);
  CYC(b_+47, b_+48); E = L;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto L_518d; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x1c);
L_518d:
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+63, b_+64); E = L;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); mem_wr(gb, HL, A);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); goto normalStatus; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+71, b_+73); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
normalStatus:
  CALL_C(b_+73, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+76);
  if (F & FC) { CYCT(b_+76, b_+78); goto L_51a5; }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+79); B = alu_dec8(gb, B);
  if (F & FZ) { CYCT(b_+79, b_+82); TAIL_S(sygerSubId01); }
  CYC(b_+79, b_+82);
  CYC(b_+82, b_+85);
  TAIL_S(sygerSubId00);
L_51a5:
  CYC(b_+85, b_+86); push_effect(gb, b_+86);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+102) goto state0;
    if (jt_ == b_+117) goto state1;
    if (jt_ == b_+151) goto stateStub;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+102, b_+104); A = 0x01;
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CYC(b_+105, b_+107); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+109); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+109, b_+112); TAIL(ecom_setSpeedAndState8_b0d); }
  CYC(b_+109, b_+112);
  CYC(b_+112, b_+114); A = 0x74;
  CYC(b_+114, b_+117);
  TAIL(enemyBoss_initializeRoom_b0f);
state1:
  CYC(b_+117, b_+119); B = 0x02;
  CALL_C(b_+119, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+122);
  if (!(F & FZ)) { RET_TAKEN(b_+122); return; }
  CYC(b_+122, b_+123);
  CYC(b_+123, b_+125); B = 0x74;
  CALL_C(b_+125, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+128);
  CYC(b_+128, b_+129); C = H;
  CALL_C(b_+129, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+132);
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+133, b_+135); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+135, b_+137); A = 0x80;
  CYC(b_+137, b_+138); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+138, b_+139); mem_wr(gb, HL, C);
  CYC(b_+139, b_+140); B = H;
  CYC(b_+140, b_+141); H = C;
  CYC(b_+141, b_+143); L = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+143, b_+144); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+144, b_+145); mem_wr(gb, HL, B);
  CALL_C(b_+145, s_objectCopyPosition, SYM(objectCopyPosition), b_+148);
  CYC(b_+148, b_+151);
  TAIL(enemyDelete);
stateStub:
  RET(b_+151); return;
}

// sygerSubId01@var35_01@func_5339
static void syger_var35_01_func_5339(GB *gb) {
  BASE(sygerSubId01);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+337, b_+339); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+339, b_+340); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+340, b_+341); mem_wr(gb, DE, A);
  CYC(b_+341, b_+343); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+343, b_+344); A = L;
  CYC(b_+344, b_+345); mem_wr(gb, DE, A);
  CYC(b_+345, b_+346); E = alu_inc8(gb, E);
  CYC(b_+346, b_+347); A = H;
  CYC(b_+347, b_+348); mem_wr(gb, DE, A);
  RET(b_+348); return;
}

void s_sygerSubId01_hook(GB *gb) {
  BASE(sygerSubId01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x08);
  CYC(b_+5, b_+7); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+7, b_+9); goto state8toA; }
  CYC(b_+7, b_+9);
  CALL_C(b_+9, s_func_54b5_hook, SYM(func_54b5), b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+212) goto var35_00;
    if (jt_ == b_+277) goto var35_01;
    if (jt_ == b_+396) goto var35_02;
    HANDOFF(HL);
  } while (0);
state8toA:
  CYC(b_+24, b_+25); push_effect(gb, b_+25);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+31) goto state8;
    if (jt_ == b_+126) goto state9;
    if (jt_ == b_+170) goto stateA;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+31, b_+32); E = alu_inc8(gb, E);
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+40) goto state8_substate0;
    if (jt_ == b_+65) goto state8_substate1;
    if (jt_ == b_+100) goto state8_substate2;
    HANDOFF(HL);
  } while (0);
state8_substate0:
  CYC(b_+40, b_+43); SET_BC(0x0108);
  CALL_C(b_+43, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+46);
  if (!(F & FZ)) { RET_TAKEN(b_+46); return; }
  CYC(b_+46, b_+47);
  CALL_C(b_+47, s_ecom_setZAboveScreen, SYM(ecom_setZAboveScreen_b0d), b_+50);
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x5b);
  CYC(b_+57, b_+59); A = 0x02;
  CALL_C(b_+59, s_func_5512_hook, SYM(func_5512), b_+62);
  CYC(b_+62, b_+65);
  TAIL(objectSetVisible81);
state8_substate1:
  CYC(b_+65, b_+67); C = 0x18;
  CALL_C(b_+67, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+72); goto animate; }
  CYC(b_+70, b_+72);
  if (!(F & FC)) { CYCT(b_+72, b_+74); goto L_5245; }
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+79); L = alu_inc8(gb, L);
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x5a);
  CYC(b_+81, b_+82); alu_xor(gb, A);
  CALL_C(b_+82, s_func_5512_hook, SYM(func_5512), b_+85);
  CYC(b_+85, b_+87); A = 0x2d;
  CYC(b_+87, b_+90); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+90, s_playSound, SYM(playSound_b00), b_+93);
L_5245:
  CYC(b_+93, b_+95); A = 0x8f;
  CALL_C(b_+95, s_playSound, SYM(playSound_b00), b_+98);
  CYC(b_+98, b_+100);
  goto animate;
state8_substate2:
  CALL_C(b_+100, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+103);
  if (F & FZ) { CYCT(b_+103, b_+105); goto L_525b; }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+106); A = mem_rd(gb, HL);
  CYC(b_+106, b_+108); alu_cp(gb, 0x46);
  CYC(b_+108, b_+110); A = 0xd1;
  if (F & FZ) CALL_C_CC(b_+110, s_playSound, SYM(playSound_b00), b_+113);
  else CYC(b_+110, b_+113);
  CYC(b_+113, b_+115);
  goto animate;
L_525b:
  CYC(b_+115, b_+116); L = E;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x00);
  CYC(b_+118, b_+119); L = alu_dec8(gb, L);
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x0b);
  CALL_C(b_+121, s_func_556f_hook, SYM(func_556f), b_+124);
  CYC(b_+124, b_+126);
  goto animate;
state9:
  CALL_C(b_+126, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+129);
  if (!(F & FZ)) { CYCT(b_+129, b_+131); goto L_527f; }
  CYC(b_+129, b_+131);
  CYC(b_+131, b_+132); L = alu_inc8(gb, L);
  CYC(b_+132, b_+133); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+133, b_+134); mem_wr(gb, HL, A);
  CYC(b_+134, b_+135); L = E;
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+137); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+137, b_+139); goto animate; }
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+141); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x78);
  CYC(b_+143, b_+145); A = 0x02;
  CYC(b_+145, b_+147); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+147, b_+148); mem_wr(gb, HL, A);
  CYC(b_+148, b_+151);
  TAIL_S(func_5512);
L_527f:
  CYC(b_+151, b_+153); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+156); alu_cp(gb, 0x96);
  if (!(F & FC)) { CYCT(b_+156, b_+158); goto animate; }
  CYC(b_+156, b_+158);
  CALL_C(b_+158, s_func_554a_hook, SYM(func_554a), b_+161);
  CALL_C(b_+161, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+164);
  CALL_C(b_+164, s_objectApplySpeed, SYM(objectApplySpeed), b_+167);
animate:
  CYC(b_+167, b_+170);
  TAIL(enemyAnimate);
stateA:
  CALL_C(b_+170, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+173);
  if (F & FZ) { CYCT(b_+173, b_+175); goto L_52b6; }
  CYC(b_+173, b_+175);
  CYC(b_+175, b_+177); C = 0x12;
  CALL_C(b_+177, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+180);
  if (!(F & FZ)) { CYCT(b_+180, b_+183); TAIL_S(seasonsFunc_0e_5523); }
  CYC(b_+180, b_+183);
  CYC(b_+183, b_+185); A = 0x8f;
  CALL_C(b_+185, s_playSound, SYM(playSound_b00), b_+188);
  CYC(b_+188, b_+189); H = D;
  CYC(b_+189, b_+191); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+191, b_+192); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+192, b_+194); goto L_52b6; }
  CYC(b_+192, b_+194);
  CYC(b_+194, b_+195); L = alu_dec8(gb, L);
  CYC(b_+195, b_+197); mem_wr(gb, HL, 0xf0);
  CYC(b_+197, b_+199); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+199, b_+201); A = 0x40;
  CYC(b_+201, b_+202); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+202, b_+204); mem_wr(gb, HL, 0xfe);
  CYC(b_+204, b_+206);
  goto animate;
L_52b6:
  CYC(b_+206, b_+208); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+208, b_+209); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+209, b_+212);
  TAIL_S(func_556f);
var35_00:
  CYC(b_+212, b_+213); E = alu_inc8(gb, E);
  CYC(b_+213, b_+214); A = mem_rd(gb, DE);
  CYC(b_+214, b_+215); push_effect(gb, b_+215);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+221) goto var35_00_substate0;
    if (jt_ == b_+238) goto var35_00_substate1;
    if (jt_ == b_+260) goto var35_00_substate2;
    HANDOFF(HL);
  } while (0);
var35_00_substate0:
  CYC(b_+221, b_+222); H = D;
  CYC(b_+222, b_+223); L = E;
  CYC(b_+223, b_+224); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+224, b_+225); L = alu_inc8(gb, L);
  CYC(b_+225, b_+227); mem_wr(gb, HL, 0x1e);
  CYC(b_+227, b_+229); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+229, b_+231); A = 0x09;
  CYC(b_+231, b_+232); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+232, b_+233); mem_wr(gb, HL, A);
  CYC(b_+233, b_+235); A = 0x03;
  CALL_C(b_+235, s_func_5512_hook, SYM(func_5512), b_+238);
var35_00_substate1:
  CALL_C(b_+238, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+241);
  if (!(F & FZ)) { CYCT(b_+241, b_+244); TAIL_S(seasonsFunc_0e_557b); }
  CYC(b_+241, b_+244);
  CYC(b_+244, b_+246); mem_wr(gb, HL, 0x78);
  CYC(b_+246, b_+247); L = alu_inc8(gb, L);
  CYC(b_+247, b_+249); mem_wr(gb, HL, 0x02);
  CYC(b_+249, b_+250); L = E;
  CYC(b_+250, b_+251); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+251, b_+253); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+253, b_+255); mem_wr(gb, HL, 0x46);
  CALL_C(b_+255, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+258);
  CYC(b_+258, b_+260);
  goto animate;
var35_00_substate2:
  CALL_C(b_+260, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+263);
  if (!(F & FZ)) { CYCT(b_+263, b_+265); goto func_5365; }
  CYC(b_+263, b_+265);
  CYC(b_+265, b_+267); mem_wr(gb, HL, 0x2d);
  CYC(b_+267, b_+268); L = alu_inc8(gb, L);
  CYC(b_+268, b_+269); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+269, b_+272); TAIL_S(func_54c6); }
  CYC(b_+269, b_+272);
  CALL_C(b_+272, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+275);
  CYC(b_+275, b_+277);
  goto animate;
var35_01:
  CYC(b_+277, b_+278); E = alu_inc8(gb, E);
  CYC(b_+278, b_+279); A = mem_rd(gb, DE);
  CYC(b_+279, b_+280); push_effect(gb, b_+280);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+286) goto var35_01_substate0;
    if (jt_ == b_+307) goto var35_01_substate1;
    if (jt_ == b_+349) goto var35_01_substate2;
    HANDOFF(HL);
  } while (0);
var35_01_substate0:
  CYC(b_+286, b_+287); H = D;
  CYC(b_+287, b_+288); L = E;
  CYC(b_+288, b_+289); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+289, b_+290); L = alu_inc8(gb, L);
  CYC(b_+290, b_+292); mem_wr(gb, HL, 0x0f);
  CYC(b_+292, b_+294); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+294, b_+296); A = 0x09;
  CYC(b_+296, b_+297); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+297, b_+298); mem_wr(gb, HL, A);
  CYC(b_+298, b_+300); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+300, b_+302); mem_wr(gb, HL, 0x78);
  CYC(b_+302, b_+304); A = 0x03;
  CALL_C(b_+304, s_func_5512_hook, SYM(func_5512), b_+307);
var35_01_substate1:
  CALL_C(b_+307, s_seasonsFunc_0e_557b_hook, SYM(seasonsFunc_0e_557b), b_+310);
  CALL_C(b_+310, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+313);
  if (!(F & FZ)) { RET_TAKEN(b_+313); return; }
  CYC(b_+313, b_+314);
  CYC(b_+314, b_+316); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+316, b_+317); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+317, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+320);
  CYC(b_+320, b_+322); alu_and(gb, 0x01);
  CYC(b_+322, b_+325); SET_HL(SYM(table_55ab));
  CYC(b_+325, b_+326); syger_add_double_index(gb, b_+326);
  CYC(b_+326, b_+327); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+327, b_+328); H = mem_rd(gb, HL);
  CYC(b_+328, b_+329); L = A;
  CYC(b_+329, b_+331); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+331, b_+332); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+332, b_+333); mem_wr(gb, DE, A);
  CYC(b_+333, b_+335); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+335, b_+336); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+336, b_+337); mem_wr(gb, DE, A);
  CYC(b_+337, b_+339); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+339, b_+340); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+340, b_+341); mem_wr(gb, DE, A);
  CYC(b_+341, b_+343); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+343, b_+344); A = L;
  CYC(b_+344, b_+345); mem_wr(gb, DE, A);
  CYC(b_+345, b_+346); E = alu_inc8(gb, E);
  CYC(b_+346, b_+347); A = H;
  CYC(b_+347, b_+348); mem_wr(gb, DE, A);
  RET(b_+348); return;
var35_01_substate2:
  CALL_C(b_+349, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+352);
  if (!(F & FZ)) { CYCT(b_+352, b_+355); goto func_5365; }
  CYC(b_+352, b_+355);
  CYC(b_+355, b_+357); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+357, b_+358); A = mem_rd(gb, DE);
  CYC(b_+358, b_+359); L = A;
  CYC(b_+359, b_+360); E = alu_inc8(gb, E);
  CYC(b_+360, b_+361); A = mem_rd(gb, DE);
  CYC(b_+361, b_+362); H = A;
  CYC(b_+362, b_+363); A = mem_rd(gb, HL);
  CYC(b_+363, b_+364); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+364, b_+367); TAIL_S(func_54c6); }
  CYC(b_+364, b_+367);
  CYC(b_+367, b_+369); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+369, b_+370); A = mem_rd(gb, DE);
  CYC(b_+370, b_+371); B = A;
  CYC(b_+371, b_+373); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+373, b_+374); A = mem_rd(gb, DE);
  CYC(b_+374, b_+375); alu_add(gb, B);
  CYC(b_+375, b_+377); alu_and(gb, 0x1f);
  CYC(b_+377, b_+378); mem_wr(gb, DE, A);
  CALL_L(b_+378, syger_var35_01_func_5339, b_+381);
func_5365:
  CALL_C(b_+381, s_func_5563_hook, SYM(func_5563), b_+384);
  CALL_C(b_+384, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+387);
  CALL_C(b_+387, s_objectApplySpeed, SYM(objectApplySpeed), b_+390);
  CALL_C(b_+390, s_func_556f_hook, SYM(func_556f), b_+393);
  CYC(b_+393, b_+396);
  TAIL_S(seasonsFunc_0e_557b);
var35_02:
  CYC(b_+396, b_+397); E = alu_inc8(gb, E);
  CYC(b_+397, b_+398); A = mem_rd(gb, DE);
  CYC(b_+398, b_+399); push_effect(gb, b_+399);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+411) goto var35_02_substate0;
    if (jt_ == b_+427) goto var35_02_substate1;
    if (jt_ == b_+485) goto var35_02_substate2;
    if (jt_ == b_+517) goto var35_02_substate3;
    if (jt_ == b_+551) goto var35_02_substate4;
    if (jt_ == b_+567) goto var35_02_substate5;
    HANDOFF(HL);
  } while (0);
var35_02_substate0:
  CYC(b_+411, b_+412); H = D;
  CYC(b_+412, b_+413); L = E;
  CYC(b_+413, b_+414); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+414, b_+415); L = alu_inc8(gb, L);
  CYC(b_+415, b_+417); mem_wr(gb, HL, 0x3c);
  CYC(b_+417, b_+418); L = alu_inc8(gb, L);
  CYC(b_+418, b_+420); mem_wr(gb, HL, 0x04);
  CYC(b_+420, b_+422); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+422, b_+424); A = 0x09;
  CYC(b_+424, b_+425); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+425, b_+426); mem_wr(gb, HL, A);
  RET(b_+426); return;
var35_02_substate1:
  CALL_C(b_+427, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+430);
  CYC(b_+430, b_+432); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+432, b_+434); A = 0x80;
  CYC(b_+434, b_+435); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+435, b_+437); mem_wr(gb, HL, 0xfe);
  CYC(b_+437, b_+438); L = E;
  if (!(F & FZ)) { CYCT(b_+438, b_+440); goto L_53a3; }
  CYC(b_+438, b_+440);
  CYC(b_+440, b_+442); mem_wr(gb, HL, 0x05);
  RET(b_+442); return;
L_53a3:
  CYC(b_+443, b_+444); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+444, b_+446); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+446, b_+448); mem_wr(gb, HL, 0x64);
  CYC(b_+448, b_+450); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+450, b_+451); A = mem_rd(gb, HL);
  CYC(b_+451, b_+452); A = alu_dec8(gb, A);
  CYC(b_+452, b_+455); SET_BC(SYM(table_55a8));
  CALL_C(b_+455, s_addAToBc, SYM(addAToBc), b_+458);
  CYC(b_+458, b_+460); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+460, b_+461); A = mem_rd(gb, BC);
  CYC(b_+461, b_+462); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+462, b_+464); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+464, b_+466); B = 0x18;
  CYC(b_+466, b_+467); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+467, b_+469); goto L_53c3; }
  CYC(b_+467, b_+469);
  CYC(b_+469, b_+471); B = 0xd8;
  CYC(b_+471, b_+472); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+472, b_+474); goto L_53c3; }
  CYC(b_+472, b_+474);
  CYC(b_+474, b_+475); B = A;
L_53c3:
  CYC(b_+475, b_+476); mem_wr(gb, HL, B);
  CYC(b_+476, b_+478); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+478, b_+479); A = mem_rd(gb, DE);
  CYC(b_+479, b_+481); alu_and(gb, 0x01);
  CYC(b_+481, b_+482); A = alu_inc8(gb, A);
  CYC(b_+482, b_+485);
  TAIL_S(func_5512);
var35_02_substate2:
  CYC(b_+485, b_+487); C = 0x12;
  CALL_C(b_+487, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+490);
  if (!(F & FZ)) { CYCT(b_+490, b_+493); TAIL_S(seasonsFunc_0e_5518); }
  CYC(b_+490, b_+493);
  CYC(b_+493, b_+495); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+495, b_+496); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+496, b_+497); L = alu_inc8(gb, L);
  CYC(b_+497, b_+499); mem_wr(gb, HL, 0x1e);
  CYC(b_+499, b_+501); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+501, b_+502); A = mem_rd(gb, HL);
  CYC(b_+502, b_+504); alu_and(gb, 0x01);
  CYC(b_+504, b_+506); A = alu_swap(gb, A);
  CYC(b_+506, b_+508); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+508, b_+509); mem_wr(gb, HL, A);
  CYC(b_+509, b_+511); A = 0x8f;
  CALL_C(b_+511, s_playSound, SYM(playSound_b00), b_+514);
var35_02_animate:
  CYC(b_+514, b_+517);
  TAIL(enemyAnimate);
var35_02_substate3:
  CALL_C(b_+517, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+520);
  if (F & FZ) { CYCT(b_+520, b_+522); goto L_5406; }
  CYC(b_+520, b_+522);
  CYC(b_+522, b_+523); A = mem_rd(gb, HL);
  CYC(b_+523, b_+525); alu_cp(gb, 0x14);
  if (!(F & FC)) { CYCT(b_+525, b_+527); goto L_5404; }
  CYC(b_+525, b_+527);
  CALL_C(b_+527, s_enemyAnimate, SYM(enemyAnimate), b_+530);
  CYC(b_+530, b_+533); A = mem_rd(gb, wFrameCounter);
  CYC(b_+533, b_+535); alu_and(gb, 0x07);
  CYC(b_+535, b_+537); A = 0x6b;
  if (F & FZ) CALL_C_CC(b_+537, s_playSound, SYM(playSound_b00), b_+540);
  else CYC(b_+537, b_+540);
L_5404:
  CYC(b_+540, b_+542);
  goto var35_02_animate;
L_5406:
  CYC(b_+542, b_+543); L = E;
  CYC(b_+543, b_+544); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+544, b_+546); A = 0xbb;
  CALL_C(b_+546, s_playSound, SYM(playSound_b00), b_+549);
  CYC(b_+549, b_+551);
  goto var35_02_animate;
var35_02_substate4:
  CALL_C(b_+551, s_enemyAnimate, SYM(enemyAnimate), b_+554);
  CALL_C(b_+554, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+557);
  if (!(F & FZ)) CALL_C_CC(b_+557, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+560);
  else CYC(b_+557, b_+560);
  if (!(F & FZ)) { RET_TAKEN(b_+560); return; }
  CYC(b_+560, b_+561);
  CYC(b_+561, b_+563); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+563, b_+565); A = 0x01;
  CYC(b_+565, b_+566); mem_wr(gb, DE, A);
  RET(b_+566); return;
var35_02_substate5:
  CYC(b_+567, b_+569); C = 0x12;
  CALL_C(b_+569, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+572);
  if (!(F & FZ)) { CYCT(b_+572, b_+574); goto var35_02_animate; }
  CYC(b_+572, b_+574);
  CYC(b_+574, b_+576); A = 0x8f;
  CALL_C(b_+576, s_playSound, SYM(playSound_b00), b_+579);
  CYC(b_+579, b_+582);
  TAIL_S(func_54c9);
}

void s_sygerSubId00_hook(GB *gb) {
  BASE(sygerSubId00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (syger_jump_table(gb));
    if (jt_ == b_+12) goto state8;
    if (jt_ == b_+27) goto state9;
    if (jt_ == b_+104) goto stateA;
    if (jt_ == b_+118) goto stateB;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x03);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x08);
  CYC(b_+22, b_+24); A = 0x04;
  CYC(b_+24, b_+27);
  TAIL(enemySetAnimation);
state9:
  CALL_C(b_+27, s_enemyAnimate, SYM(enemyAnimate), b_+30);
  CYC(b_+30, b_+32); A = 0x30;
  CALL_C(b_+32, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+37); alu_or(gb, A);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_VAR30;
  if (F & FZ) { CYCT(b_+39, b_+41); goto L_5462; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); A = 0x01;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+47, b_+49); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+49, b_+52);
  TAIL(objectSetInvisible);
L_5462:
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto L_5484; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CALL_C(b_+57, s_getRandomNumber, SYM(getRandomNumber), b_+60);
  CYC(b_+60, b_+62); alu_and(gb, 0x01);
  CYC(b_+62, b_+63); A = alu_inc8(gb, A);
  CYC(b_+63, b_+65); alu_xor(gb, 0x01);
  CYC(b_+65, b_+68); SET_BC(SYM(table_559c));
  CALL_C(b_+68, s_addAToBc, SYM(addAToBc), b_+71);
  CYC(b_+71, b_+73); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+73, b_+74); A = mem_rd(gb, BC);
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); E = alu_inc8(gb, E);
  CYC(b_+76, b_+77); SET_BC(BC + 1);
  CYC(b_+77, b_+78); A = mem_rd(gb, BC);
  CYC(b_+78, b_+79); mem_wr(gb, DE, A);
  CYC(b_+79, b_+80); SET_BC(BC + 1);
  CYC(b_+80, b_+81); A = mem_rd(gb, BC);
  CYC(b_+81, b_+82); push_effect(gb, HL);
  CALL_C(b_+82, s_enemySetAnimation, SYM(enemySetAnimation), b_+85);
  CYC(b_+85, b_+86); SET_HL(pop_effect(gb));
L_5484:
  CYC(b_+86, b_+88); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); B = A;
  CYC(b_+90, b_+91); E = alu_inc8(gb, E);
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+93); C = A;
  CALL_C(b_+93, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+96);
  CYC(b_+96, b_+97); H = D;
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+99, b_+101); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+101, b_+104);
  TAIL(objectSetVisible82);
stateA:
  CYC(b_+104, b_+105); H = D;
  CYC(b_+105, b_+107); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+107, b_+109); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+109, b_+112); TAIL(enemyDelete); }
  CYC(b_+109, b_+112);
  CYC(b_+112, b_+114); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+114, b_+116); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+116, b_+117); L = E;
  CYC(b_+117, b_+118); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
stateB:
  CYC(b_+118, b_+120); A = 0x01;
  CALL_C(b_+120, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+123);
  CYC(b_+123, b_+124); A = mem_rd(gb, HL);
  CYC(b_+124, b_+126); alu_cp(gb, 0x74);
  if (!(F & FZ)) { CYCT(b_+126, b_+129); TAIL(enemyDelete); }
  CYC(b_+126, b_+129);
  CYC(b_+129, b_+131); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+131, b_+132); E = L;
  CYC(b_+132, b_+133); A = mem_rd(gb, HL);
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  RET(b_+134); return;
}

void s_func_54b5_hook(GB *gb) {
  BASE(func_54b5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x6d;
  CYC(b_+14, b_+17);
  TAIL(playSound_b00);
}

void s_func_54c6_hook(GB *gb) {
  BASE(func_54c6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_5563_hook, SYM(func_5563), b_+3);
  s_func_54c9_hook(gb); return; // falls through
}

void s_func_54c9_hook(GB *gb) {
  BASE(func_54c9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x1f01);
  CALL_C(b_+3, s_ecom_randomBitwiseAndBCE, SYM(ecom_randomBitwiseAndBCE_b0d), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x14);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_add(gb, A);
  CYC(b_+18, b_+19); alu_add(gb, C);
  CYC(b_+19, b_+22); SET_HL(SYM(table_55a2));
  CYC(b_+22, b_+23); syger_add_a_to_hl(gb, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); goto L_54fd; }
  CYC(b_+28, b_+30);
  CALL_C(b_+30, s_func_556f_hook, SYM(func_556f), b_+33);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x09);
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x00);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0xb4);
  CYC(b_+44, b_+45); L = alu_inc8(gb, L);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x01);
  CYC(b_+47, b_+49);
  goto L_5511;

L_54fd:
  CYC(b_+52, b_+55); SET_BC(0xfe20);
  CALL_C(b_+55, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+58);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x09);
  CYC(b_+62, b_+63); L = alu_inc8(gb, L);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x00);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x78);
  CYC(b_+69, b_+70); L = alu_inc8(gb, L);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0xf0);
L_5511:
  CYC(b_+72, b_+73); alu_xor(gb, A);
  s_func_5512_hook(gb); return; // falls through
}

void s_func_5512_hook(GB *gb) {
  BASE(func_5512);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6);
  TAIL(enemySetAnimation);
}

// seasonsFunc_0e_5523+16
static void syger_seasonsFunc_0e_5523_16(GB *gb) {
  BASE(seasonsFunc_0e_5523);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+16, b_+17); alu_sub(gb, C);
  CYC(b_+17, b_+19); alu_add(gb, 0x02);
  CYC(b_+19, b_+21); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+21, b_+24); TAIL(ecom_moveTowardPosition_b0d); }
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF8F);
  CYC(b_+26, b_+27); alu_sub(gb, B);
  CYC(b_+27, b_+29); alu_add(gb, 0x02);
  CYC(b_+29, b_+31); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+31, b_+34); TAIL(ecom_moveTowardPosition_b0d); }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); mem_wr(gb, HL, C);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+38); mem_wr(gb, HL, B);
  RET(b_+38); return;
}

void s_seasonsFunc_0e_5518_hook(GB *gb) {
  BASE(seasonsFunc_0e_5518);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR36;
  CALL_C(b_+6, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+9);
  CYC(b_+9, b_+11);
  syger_seasonsFunc_0e_5523_16(gb); return;
}

void s_seasonsFunc_0e_5523_hook(GB *gb) {
  BASE(seasonsFunc_0e_5523);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x3878);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+12); mem_wr(gb, hFF8F, A);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); mem_wr(gb, hFF8E, A);
  CYC(b_+16, b_+17); alu_sub(gb, C);
  CYC(b_+17, b_+19); alu_add(gb, 0x02);
  CYC(b_+19, b_+21); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+21, b_+24); TAIL(ecom_moveTowardPosition_b0d); }
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF8F);
  CYC(b_+26, b_+27); alu_sub(gb, B);
  CYC(b_+27, b_+29); alu_add(gb, 0x02);
  CYC(b_+29, b_+31); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+31, b_+34); TAIL(ecom_moveTowardPosition_b0d); }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); mem_wr(gb, HL, C);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+38); mem_wr(gb, HL, B);
  RET(b_+38); return;
}

void s_func_554a_hook(GB *gb) {
  BASE(func_554a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_555f; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x1f);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
L_555f:
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+25);
  TAIL(ecom_updateAngleToScentSeed_b0d);
}

void s_func_5563_hook(GB *gb) {
  BASE(func_5563);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11); return;
}

void s_func_556f_hook(GB *gb) {
  BASE(func_556f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return;
}

void s_seasonsFunc_0e_557b_hook(GB *gb) {
  BASE(seasonsFunc_0e_557b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_enemyAnimate, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, s_func_5563_hook, SYM(func_5563), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(SYM(table_5594));
  CYC(b_+12, b_+13); syger_add_a_to_hl(gb, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return;
}

