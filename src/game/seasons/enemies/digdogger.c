#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/digdogger.s.
// ENEMY_DIGDOGGER

static uint16_t digdogger_jump_table(GB *gb) {
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

static void digdogger_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void digdogger_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// enemyCode7c@subid1@func_742f
static void digdogger_subid1_func_742f(GB *gb) {
  BASE(enemyCode7c);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+259, b_+262); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+263); return; }
  CYC(b_+263, b_+264);
  CYC(b_+264, b_+266); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+266, b_+267); A = mem_rd(gb, DE);
  CYC(b_+267, b_+268); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+268); return; }
  CYC(b_+268, b_+269);
  CALL_C(b_+269, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+272);
  CYC(b_+272, b_+275); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+275, b_+277); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+279); goto L_7449; }
  CYC(b_+277, b_+279);
  CYC(b_+279, b_+281); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+284); alu_xor(gb, 0x10);
  CYC(b_+284, b_+285); mem_wr(gb, DE, A);
L_7449:
  CYC(b_+285, b_+286); H = D;
  CYC(b_+286, b_+288); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+288, b_+290); mem_wr(gb, HL, 0x0b);
  CYC(b_+290, b_+292); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+292, b_+294); mem_wr(gb, HL, 0x50);
  CYC(b_+294, b_+296); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+296, b_+298); mem_wr(gb, HL, 0x00);
  CYC(b_+298, b_+299); alu_scf(gb);
  RET(b_+299); return;
}

// ENEMY_DIGDOGGER
void s_enemyCode7c_hook(GB *gb) {
  BASE(enemyCode7c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto normalStatus; }
  CYC(b_+8, b_+10);
  CALL_C(b_+10, s_enemyAnimate, SYM(enemyAnimate), b_+13);
  CYC(b_+13, b_+16);
  TAIL(ecom_updateKnockback_b0d);
dead:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+20, b_+22); goto L_7353; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_or(gb, A);
  CYC(b_+26, b_+28); A = 0x09;
  if (F & FZ) { CYCT(b_+28, b_+30); goto L_7350; }
  CYC(b_+28, b_+30);
  CALL_C(b_+30, s_enemySetAnimation, SYM(enemySetAnimation), b_+33);
  CALL_C(b_+33, s_ecom_killRelatedObj1, SYM(ecom_killRelatedObj1_b0d), b_+36);
L_7350:
  CYC(b_+36, b_+39);
  TAIL(enemyBoss_dead_b0f);
L_7353:
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+44); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto normalStatus; }
  CYC(b_+44, b_+46);
  CALL_C(b_+46, s_objectCreatePuff, SYM(objectCreatePuff), b_+49);
  CYC(b_+49, b_+52);
  TAIL(enemyDelete);
normalStatus:
  CALL_C(b_+52, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+55);
  if (!(F & FC)) { CYCT(b_+55, b_+57); goto L_7376; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+58); push_effect(gb, b_+58);
  do { uint16_t jt_ = (digdogger_jump_table(gb));
    if (jt_ == b_+81) goto state0;
    if (jt_ == b_+115) goto state1;
    if (jt_ == b_+184) goto stateStub;
    HANDOFF(HL);
  } while (0);
L_7376:
  CYC(b_+74, b_+75); B = alu_dec8(gb, B);
  CYC(b_+75, b_+76); A = B;
  CYC(b_+76, b_+77); push_effect(gb, b_+77);
  do { uint16_t jt_ = (digdogger_jump_table(gb));
    if (jt_ == b_+185) goto subid1;
    if (jt_ == b_+365) goto subid2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+81, b_+82); A = B;
  CYC(b_+82, b_+83); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+83, b_+85); goto L_738c; }
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+86); A = alu_inc8(gb, A);
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+89); A = 0x7c;
  CYC(b_+89, b_+91); B = 0x84;
  CALL_C(b_+91, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+94);
  CYC(b_+94, b_+96);
  goto L_739c;
L_738c:
  CYC(b_+96, b_+97); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto L_7399; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); E = ENEMY_BASE + OBJ_VAR3E;
  CYC(b_+101, b_+103); A = 0x08;
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+106); A = 0x08;
  CALL_C(b_+106, s_enemySetAnimation, SYM(enemySetAnimation), b_+109);
L_7399:
  CALL_C(b_+109, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+112);
L_739c:
  CYC(b_+112, b_+115);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+115, b_+117); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+119, b_+122); TAIL(enemyDelete); }
  CYC(b_+119, b_+122);
  CYC(b_+122, b_+124); B = 0x02;
  CALL_C(b_+124, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+127);
  if (!(F & FZ)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+130); B = 0x7c;
  CALL_C(b_+130, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+133);
  CYC(b_+133, b_+135); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x28);
  CYC(b_+137, b_+139); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0xd8);
  CYC(b_+141, b_+143); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+143, b_+145); mem_wr(gb, HL, 0xe8);
  CYC(b_+145, b_+146); C = H;
  CYC(b_+146, b_+147); push_effect(gb, HL);
  CALL_C(b_+147, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+150);
  CYC(b_+150, b_+151); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+151, b_+153); L = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+153, s_objectCopyPosition, SYM(objectCopyPosition), b_+156);
  CYC(b_+156, b_+158); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+158, b_+159); E = L;
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); mem_wr(gb, HL, A);
  CYC(b_+161, b_+163); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+163, b_+164); mem_wr(gb, HL, E);
  CYC(b_+164, b_+165); L = alu_inc8(gb, L);
  CYC(b_+165, b_+166); mem_wr(gb, HL, C);
  CYC(b_+166, b_+168); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x04);
  CYC(b_+170, b_+171); C = H;
  CYC(b_+171, b_+172); SET_HL(pop_effect(gb));
  CYC(b_+172, b_+174); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+174, b_+176); A = 0x80;
  CYC(b_+176, b_+177); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+177, b_+178); mem_wr(gb, HL, C);
  CYC(b_+178, b_+180); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+180, b_+182); A = 0x01;
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  RET(b_+183); return;
stateStub:
  RET(b_+184); return;
subid1:
  CALL_C(b_+185, s_func_779b_hook, SYM(func_779b), b_+188);
  CYC(b_+188, b_+190); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+190, b_+191); A = mem_rd(gb, DE);
  CYC(b_+191, b_+193); alu_sub(gb, 0x08);
  CYC(b_+193, b_+194); push_effect(gb, b_+194);
  do { uint16_t jt_ = (digdogger_jump_table(gb));
    if (jt_ == b_+204) goto subid1_state8;
    if (jt_ == b_+239) goto subid1_state9;
    if (jt_ == b_+255) goto subid1_stateA;
    if (jt_ == b_+300) goto subid1_stateB;
    if (jt_ == b_+334) goto subid1_stateC;
    HANDOFF(HL);
  } while (0);
subid1_state8:
  CYC(b_+204, b_+206); A = 0x04;
  CALL_C(b_+206, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+209);
  CYC(b_+209, b_+210); A = mem_rd(gb, HL);
  CYC(b_+210, b_+212); alu_cp(gb, 0x09);
  if (F & FC) { RET_TAKEN(b_+212); return; }
  CYC(b_+212, b_+213);
  CYC(b_+213, b_+216); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+216, b_+217); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+217); return; }
  CYC(b_+217, b_+218);
  CYC(b_+218, b_+220); alu_bit(gb, 1, A);
  if (F & FZ) { RET_TAKEN(b_+220); return; }
  CYC(b_+220, b_+221);
  CYC(b_+221, b_+224); SET_BC(0x0008);
  CALL_C(b_+224, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+227);
  if (!(F & FZ)) { RET_TAKEN(b_+227); return; }
  CYC(b_+227, b_+228);
  CYC(b_+228, b_+229); H = D;
  CYC(b_+229, b_+231); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+231, b_+232); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+232, b_+234); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+234, b_+236); mem_wr(gb, HL, 0x28);
  CYC(b_+236, b_+239);
  TAIL(ecom_updateAngleTowardTarget_b0d);
subid1_state9:
  CYC(b_+239, b_+241); C = 0x10;
  CALL_C(b_+241, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+244);
  if (!(F & FZ)) { CYCT(b_+244, b_+247); TAIL(objectApplySpeed); }
  CYC(b_+244, b_+247);
  CYC(b_+247, b_+249); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+249, b_+250); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+250, b_+252); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+252, b_+254); mem_wr(gb, HL, 0xff);
  RET(b_+254); return;
subid1_stateA:
  CALL_C(b_+255, s_func_7757_hook, SYM(func_7757), b_+258);
  if (F & FC) { RET_TAKEN(b_+258); return; }
  CYC(b_+258, b_+259);
  CYC(b_+259, b_+262); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+263); return; }
  CYC(b_+263, b_+264);
  CYC(b_+264, b_+266); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+266, b_+267); A = mem_rd(gb, DE);
  CYC(b_+267, b_+268); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+268); return; }
  CYC(b_+268, b_+269);
  CALL_C(b_+269, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+272);
  CYC(b_+272, b_+275); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+275, b_+277); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+279); goto L_7449; }
  CYC(b_+277, b_+279);
  CYC(b_+279, b_+281); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+284); alu_xor(gb, 0x10);
  CYC(b_+284, b_+285); mem_wr(gb, DE, A);
L_7449:
  CYC(b_+285, b_+286); H = D;
  CYC(b_+286, b_+288); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+288, b_+290); mem_wr(gb, HL, 0x0b);
  CYC(b_+290, b_+292); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+292, b_+294); mem_wr(gb, HL, 0x50);
  CYC(b_+294, b_+296); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+296, b_+298); mem_wr(gb, HL, 0x00);
  CYC(b_+298, b_+299); alu_scf(gb);
  RET(b_+299); return;
subid1_stateB:
  CALL_C(b_+300, s_func_7757_hook, SYM(func_7757), b_+303);
  if (F & FC) { CYCT(b_+303, b_+305); goto L_7469; }
  CYC(b_+303, b_+305);
  CYC(b_+305, b_+307); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+307, b_+308); A = mem_rd(gb, DE);
  CYC(b_+308, b_+309); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+309, b_+311); goto L_7469; }
  CYC(b_+309, b_+311);
  CYC(b_+311, b_+314); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+314, b_+315); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+315, b_+317); goto L_746f; }
  CYC(b_+315, b_+317);
L_7469:
  CYC(b_+317, b_+318); H = D;
  CYC(b_+318, b_+320); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+320, b_+321); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+321, b_+323);
  goto toFunc7763;
L_746f:
  CALL_C(b_+323, s_func_7737_hook, SYM(func_7737), b_+326);
  CALL_C(b_+326, s_func_770e_hook, SYM(func_770e), b_+329);
  CALL_C(b_+329, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+332);
  CYC(b_+332, b_+334);
  goto toFunc7763;
subid1_stateC:
  CYC(b_+334, b_+335); H = D;
  CYC(b_+335, b_+337); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+337, b_+338); A = mem_rd(gb, HL);
  CYC(b_+338, b_+339); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+339, b_+341); goto L_7484; }
  CYC(b_+339, b_+341);
  CYC(b_+341, b_+342); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+342, b_+344); goto L_748c; }
  CYC(b_+342, b_+344);
L_7484:
  CYC(b_+344, b_+345); L = E;
  CYC(b_+345, b_+347); mem_wr(gb, HL, 0x0a);
  CYC(b_+347, b_+349); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+349, b_+351); mem_wr(gb, HL, 0x00);
  RET(b_+351); return;
L_748c:
  CALL_L(b_+352, digdogger_subid1_func_742f, b_+355);
  if (F & FC) { RET_TAKEN(b_+355); return; }
  CYC(b_+355, b_+356);
  CALL_C(b_+356, s_func_7740_hook, SYM(func_7740), b_+359);
  CALL_C(b_+359, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+362);
toFunc7763:
  CYC(b_+362, b_+365);
  TAIL_S(func_7763);
subid2:
  CYC(b_+365, b_+366); A = mem_rd(gb, DE);
  CYC(b_+366, b_+368); alu_sub(gb, 0x08);
  CYC(b_+368, b_+369); push_effect(gb, b_+369);
  do { uint16_t jt_ = (digdogger_jump_table(gb));
    if (jt_ == b_+393) goto subid2_state8;
    if (jt_ == b_+424) goto subid2_state9;
    if (jt_ == b_+455) goto subid2_stateA;
    if (jt_ == b_+508) goto subid2_stateB;
    if (jt_ == b_+552) goto subid2_stateC;
    if (jt_ == b_+593) goto subid2_stateD;
    if (jt_ == b_+663) goto subid2_stateE;
    if (jt_ == b_+710) goto subid2_stateF;
    if (jt_ == b_+740) goto subid2_state10;
    if (jt_ == b_+769) goto subid2_state11;
    if (jt_ == b_+841) goto subid2_state12;
    if (jt_ == b_+866) goto subid2_state13;
    HANDOFF(HL);
  } while (0);
subid2_state8:
  CYC(b_+393, b_+396); A = mem_rd(gb, wcc93);
  CYC(b_+396, b_+397); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+397); return; }
  CYC(b_+397, b_+398);
  CYC(b_+398, b_+401); SET_BC(0x0208);
  CALL_C(b_+401, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+404);
  if (!(F & FZ)) { RET_TAKEN(b_+404); return; }
  CYC(b_+404, b_+405);
  CYC(b_+405, b_+406); H = D;
  CYC(b_+406, b_+408); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+408, b_+409); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+409, b_+411); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+411, b_+412); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+412, b_+414); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+414, b_+416); mem_wr(gb, HL, 0x06);
  CYC(b_+416, b_+418); A = 0x2e;
  CYC(b_+418, b_+421); mem_wr(gb, wActiveMusic, A);
  CYC(b_+421, b_+424);
  TAIL(playSound_b00);
subid2_state9:
  CALL_C(b_+424, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+427);
  if (!(F & FZ)) { CYCT(b_+427, b_+430); TAIL(enemyAnimate); }
  CYC(b_+427, b_+430);
  CYC(b_+430, b_+431); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+431, b_+432); L = alu_inc8(gb, L);
  CYC(b_+432, b_+434); mem_wr(gb, HL, 0x06);
  CYC(b_+434, b_+435); L = E;
  CYC(b_+435, b_+436); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+436, b_+438); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+438, b_+439); A = mem_rd(gb, DE);
  CYC(b_+439, b_+440); A = alu_dec8(gb, A);
  CYC(b_+440, b_+443); SET_HL(b_+449 /* @subid2@table_74ed */);
  CYC(b_+443, b_+444); digdogger_add_a_to_hl(gb, b_+444);
  CYC(b_+444, b_+446); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+446, b_+447); A = mem_rd(gb, HL);
  CYC(b_+447, b_+448); mem_wr(gb, DE, A);
  RET(b_+448); return;

subid2_stateA:
  CALL_C(b_+455, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+458);
  if (!(F & FZ)) { RET_TAKEN(b_+458); return; }
  CYC(b_+458, b_+459);
  CYC(b_+459, b_+460); L = E;
  CYC(b_+460, b_+461); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+461, b_+463); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+463, b_+465); A = 0x80;
  CYC(b_+465, b_+466); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+466, b_+468); mem_wr(gb, HL, 0xfe);
  CALL_C(b_+468, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+471);
  CALL_C(b_+471, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+474);
  CYC(b_+474, b_+476); alu_and(gb, 0x03);
  CYC(b_+476, b_+479); SET_HL(b_+504 /* @subid2@table_7524 */);
  CYC(b_+479, b_+480); digdogger_add_a_to_hl(gb, b_+480);
  CYC(b_+480, b_+482); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+482, b_+483); A = mem_rd(gb, DE);
  CYC(b_+483, b_+485); alu_add(gb, 0x02);
  CYC(b_+485, b_+487); alu_and(gb, 0x1c);
  CYC(b_+487, b_+488); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+488, b_+490); alu_and(gb, 0x1f);
  CYC(b_+490, b_+491); mem_wr(gb, DE, A);
  CYC(b_+491, b_+493); A = 0x98;
  CALL_C(b_+493, s_playSound, SYM(playSound_b00), b_+496);
  CYC(b_+496, b_+498); A = 0x04;
  CALL_C(b_+498, s_enemySetAnimation, SYM(enemySetAnimation), b_+501);
  CYC(b_+501, b_+504);
  TAIL(objectSetVisible81);

subid2_stateB:
  CYC(b_+508, b_+510); C = 0x0c;
  CALL_C(b_+510, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+513);
  if (!(F & FZ)) { CYCT(b_+513, b_+516); TAIL_S(func_76de); }
  CYC(b_+513, b_+516);
  CYC(b_+516, b_+518); A = 0x85;
  CALL_C(b_+518, s_playSound, SYM(playSound_b00), b_+521);
  CALL_C(b_+521, s_objectSetVisible82, SYM(objectSetVisible82), b_+524);
  CYC(b_+524, b_+525); H = D;
  CYC(b_+525, b_+527); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+527, b_+529); mem_wr(gb, HL, 0x10);
  CYC(b_+529, b_+530); L = alu_inc8(gb, L);
  CYC(b_+530, b_+531); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+531, b_+533); goto L_7549; }
  CYC(b_+531, b_+533);
  CYC(b_+533, b_+535); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+535, b_+536); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+536, b_+538); A = 0x01;
  CYC(b_+538, b_+541);
  TAIL(enemySetAnimation);
L_7549:
  CYC(b_+541, b_+542); L = alu_dec8(gb, L);
  CYC(b_+542, b_+544); mem_wr(gb, HL, 0x0c);
  CYC(b_+544, b_+546); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+546, b_+547); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+547, b_+549); A = 0x02;
  CYC(b_+549, b_+552);
  TAIL(enemySetAnimation);
subid2_stateC:
  CALL_C(b_+552, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+555);
  if (F & FZ) { CYCT(b_+555, b_+557); goto L_7561; }
  CYC(b_+555, b_+557);
  CYC(b_+557, b_+558); A = mem_rd(gb, HL);
  CYC(b_+558, b_+560); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+560); return; }
  CYC(b_+560, b_+561);
  CYC(b_+561, b_+562); alu_xor(gb, A);
  CYC(b_+562, b_+565);
  TAIL(enemySetAnimation);
L_7561:
  CYC(b_+565, b_+566); L = E;
  CYC(b_+566, b_+567); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+567, b_+569); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+569, b_+571); mem_wr(gb, HL, 0x32);
  CYC(b_+571, b_+574); SET_BC(0xfc00);
  CALL_C(b_+574, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+577);
  CYC(b_+577, b_+579); A = 0x98;
  CALL_C(b_+579, s_playSound, SYM(playSound_b00), b_+582);
  CALL_C(b_+582, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+585);
  CALL_C(b_+585, s_objectSetVisible81, SYM(objectSetVisible81), b_+588);
  CYC(b_+588, b_+590); A = 0x03;
  CYC(b_+590, b_+593);
  TAIL(enemySetAnimation);
subid2_stateD:
  CYC(b_+593, b_+595); C = 0x10;
  CALL_C(b_+595, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+598);
  CYC(b_+598, b_+600); A = mem_rd(gb, hCameraY);
  CYC(b_+600, b_+601); B = A;
  CYC(b_+601, b_+603); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+603, b_+605); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+605, b_+606); A = mem_rd(gb, DE);
  CYC(b_+606, b_+607); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+607, b_+608); alu_sub(gb, B);
  CYC(b_+608, b_+610); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+610, b_+613); TAIL_S(func_76bc); }
  CYC(b_+610, b_+613);
  CYC(b_+613, b_+615); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+615, b_+616); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+616, b_+618); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+618, b_+620); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+620, b_+622); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+622, b_+624); mem_wr(gb, HL, 0x3c);
  CYC(b_+624, b_+626); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+626, b_+628); mem_wr(gb, HL, 0x00);
  CALL_C(b_+628, s_objectSetInvisible, SYM(objectSetInvisible), b_+631);
  CYC(b_+631, b_+633); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+633, b_+634); A = mem_rd(gb, DE);
  CYC(b_+634, b_+635); A = alu_dec8(gb, A);
  CYC(b_+635, b_+638); SET_HL(b_+651 /* @subid2@table_75b7 */);
  CYC(b_+638, b_+639); digdogger_add_double_index(gb, b_+639);
  CYC(b_+639, b_+641); E = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+641, b_+642); alu_xor(gb, A);
  CYC(b_+642, b_+643); mem_wr(gb, DE, A);
  CYC(b_+643, b_+644); E = alu_inc8(gb, E);
  CYC(b_+644, b_+645); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+645, b_+646); mem_wr(gb, DE, A);
  CYC(b_+646, b_+648); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+648, b_+649); A = mem_rd(gb, HL);
  CYC(b_+649, b_+650); mem_wr(gb, DE, A);
  RET(b_+650); return;

subid2_stateE:
  CALL_C(b_+663, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+666);
  if (F & FZ) { CYCT(b_+666, b_+668); goto L_75e5; }
  CYC(b_+666, b_+668);
  CYC(b_+668, b_+669); A = mem_rd(gb, HL);
  CYC(b_+669, b_+670); L = alu_inc8(gb, L);
  CYC(b_+670, b_+671); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+671, b_+673); C = 0x10;
  if (F & FC) CALL_C_CC(b_+673, s_ecom_setZAboveScreen, SYM(ecom_setZAboveScreen_b0d), b_+676);
  else CYC(b_+673, b_+676);
  if (!(F & FZ)) { RET_TAKEN(b_+676); return; }
  CYC(b_+676, b_+677);
  CYC(b_+677, b_+679); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+679, b_+682); A = mem_rd(gb, w1Link_yh);
  CYC(b_+682, b_+684); alu_and(gb, 0xf0);
  CYC(b_+684, b_+686); alu_add(gb, 0x08);
  CYC(b_+686, b_+687); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+687, b_+688); L = alu_inc8(gb, L);
  CYC(b_+688, b_+691); A = mem_rd(gb, w1Link_xh);
  CYC(b_+691, b_+693); alu_and(gb, 0xf0);
  CYC(b_+693, b_+695); alu_add(gb, 0x08);
  CYC(b_+695, b_+696); mem_wr(gb, HL, A);
  RET(b_+696); return;
L_75e5:
  CYC(b_+697, b_+698); L = E;
  CYC(b_+698, b_+699); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+699, b_+701); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+701, b_+703); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+703, b_+705); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+705, b_+707); mem_wr(gb, HL, 0xf8);
  CYC(b_+707, b_+710);
  TAIL(objectSetVisible81);
subid2_stateF:
  CYC(b_+710, b_+712); C = 0x20;
  CALL_C(b_+712, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+715);
  if (!(F & FZ)) { RET_TAKEN(b_+715); return; }
  CYC(b_+715, b_+716);
  CYC(b_+716, b_+718); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+718, b_+720); mem_wr(gb, HL, 0x09);
  CYC(b_+720, b_+722); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+722, b_+724); mem_wr(gb, HL, 0x77);
  CYC(b_+724, b_+726); L = ENEMY_BASE + OBJ_DAMAGE;
  CYC(b_+726, b_+728); mem_wr(gb, HL, 0xfc);
  CYC(b_+728, b_+730); A = 0x85;
  CALL_C(b_+730, s_playSound, SYM(playSound_b00), b_+733);
  CALL_C(b_+733, s_objectSetVisible82, SYM(objectSetVisible82), b_+736);
  CYC(b_+736, b_+737); alu_xor(gb, A);
  CYC(b_+737, b_+740);
  TAIL(enemySetAnimation);
subid2_state10:
  CYC(b_+740, b_+741); H = D;
  CYC(b_+741, b_+742); L = E;
  CYC(b_+742, b_+743); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+743, b_+745); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+745, b_+747); mem_wr(gb, HL, 0x1e);
  CYC(b_+747, b_+749); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+749, b_+751); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+751, b_+753); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+753, b_+754); alu_xor(gb, A);
  CYC(b_+754, b_+755); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+755, b_+756); mem_wr(gb, HL, A);
  CYC(b_+756, b_+758); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+758, b_+760); mem_wr(gb, HL, 0x2d);
  CYC(b_+760, b_+762); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+762, b_+764); mem_wr(gb, HL, 0x12);
  CYC(b_+764, b_+766); A = 0x05;
  CYC(b_+766, b_+769);
  TAIL(enemySetAnimation);
subid2_state11:
  CALL_C(b_+769, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+772);
  if (!(F & FZ)) { CYCT(b_+772, b_+775); TAIL(enemyAnimate); }
  CYC(b_+772, b_+775);
  CYC(b_+775, b_+777); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+777, b_+778); A = mem_rd(gb, DE);
  CYC(b_+778, b_+779); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+779, b_+781); TAIL_S(func_76ac); }
  CYC(b_+779, b_+781);
  CYC(b_+781, b_+782); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+782, b_+784); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+784, b_+785); B = mem_rd(gb, HL);
  CALL_C(b_+785, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+788);
  if (!(F & FZ)) { RET_TAKEN(b_+788); return; }
  CYC(b_+788, b_+789);
  CYC(b_+789, b_+791); E = 0x97;
  CYC(b_+791, b_+792); A = mem_rd(gb, DE);
  CYC(b_+792, b_+794); mem_wr(gb, hFF8B, A);
  CYC(b_+794, b_+796); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+796, b_+797); A = mem_rd(gb, DE);
  CYC(b_+797, b_+798); C = A;
L_764a:
  CYC(b_+798, b_+800); B = 0x55;
  CALL_C(b_+800, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+803);
  CALL_C(b_+803, s_objectCopyPosition, SYM(objectCopyPosition), b_+806);
  CYC(b_+806, b_+808); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+808, b_+810); A = 0x80;
  CYC(b_+810, b_+811); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+811, b_+812); mem_wr(gb, HL, D);
  CYC(b_+812, b_+814); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+814, b_+816); A = mem_rd(gb, hFF8B);
  CYC(b_+816, b_+817); mem_wr(gb, HL, A);
  CYC(b_+817, b_+818); A = C;
  CYC(b_+818, b_+820); alu_add(gb, 0xb2);
  CYC(b_+820, b_+821); E = A;
  CYC(b_+821, b_+822); A = H;
  CYC(b_+822, b_+823); mem_wr(gb, DE, A);
  CYC(b_+823, b_+824); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+824, b_+826); goto L_764a; }
  CYC(b_+824, b_+826);
  CYC(b_+826, b_+827); H = D;
  CYC(b_+827, b_+829); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+829, b_+830); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+830, b_+832); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+832, b_+834); mem_wr(gb, HL, 0xa0);
  CYC(b_+834, b_+836); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+836, b_+837); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+837, b_+838); mem_wr(gb, HL, A);
  CYC(b_+838, b_+841);
  TAIL(objectSetInvisible);
subid2_state12:
  CYC(b_+841, b_+844); A = mem_rd(gb, wFrameCounter);
  CYC(b_+844, b_+846); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+846); return; }
  CYC(b_+846, b_+847);
  CALL_C(b_+847, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+850);
  if (!(F & FZ)) { RET_TAKEN(b_+850); return; }
  CYC(b_+850, b_+851);
  CYC(b_+851, b_+853); mem_wr(gb, HL, 0x19);
  CYC(b_+853, b_+854); L = E;
  CYC(b_+854, b_+855); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+855, s_func_76e4_hook, SYM(func_76e4), b_+858);
  CYC(b_+858, b_+860); A = 0x06;
  CALL_C(b_+860, s_enemySetAnimation, SYM(enemySetAnimation), b_+863);
  CYC(b_+863, b_+866);
  TAIL(objectSetVisible82);
subid2_state13:
  CYC(b_+866, b_+868); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+868, b_+869); A = mem_rd(gb, DE);
  CYC(b_+869, b_+870); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+870, b_+872); goto L_769d; }
  CYC(b_+870, b_+872);
  CYC(b_+872, b_+873); A = alu_dec8(gb, A);
  CYC(b_+873, b_+874); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(b_+874, b_+876); goto L_769d; }
  CYC(b_+874, b_+876);
  CYC(b_+876, b_+878); A = 0x07;
  CALL_C(b_+878, s_enemySetAnimation, SYM(enemySetAnimation), b_+881);
L_769d:
  CYC(b_+881, b_+883); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+883, b_+884); A = mem_rd(gb, DE);
  CYC(b_+884, b_+885); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+885); return; }
  CYC(b_+885, b_+886);
  CYC(b_+886, b_+888); A = 0xc0;
  CALL_C(b_+888, s_playSound, SYM(playSound_b00), b_+891);
  CYC(b_+891, b_+892); H = D;
  CYC(b_+892, b_+894); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+894, b_+896); mem_wr(gb, HL, 0x04);
  s_func_76ac_hook(gb); return; // falls through
}

void s_func_76ac_hook(GB *gb) {
  BASE(func_76ac);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x09);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x69);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16);
  TAIL(enemySetAnimation);
}

void s_func_76bc_hook(GB *gb) {
  BASE(func_76bc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_76d1; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_76d1; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x02);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x14);
  CYC(b_+16, b_+18); A = 0x04;
  CALL_C(b_+18, s_enemySetAnimation, SYM(enemySetAnimation), b_+21);
L_76d1:
  CYC(b_+21, b_+24); A = mem_rd(gb, wFrameCounter);
  CYC(b_+24, b_+26); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); TAIL_S(func_76de); }
  CYC(b_+26, b_+28);
  CALL_C(b_+28, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+31);
  CALL_C(b_+31, s_objectNudgeAngleTowards, SYM(objectNudgeAngleTowards), b_+34);
  s_func_76de_hook(gb); return; // falls through
}

void s_func_76de_hook(GB *gb) {
  BASE(func_76de);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectApplySpeed);
}

void s_func_76e4_hook(GB *gb) {
  BASE(func_76e4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR32;
L_76e6:
  CYC(b_+2, b_+3); E = alu_inc8(gb, E);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); H = A;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_76e6; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+13, b_+14); B = H;
  CYC(b_+14, b_+16); C = 0x06;
L_76f4:
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); H = A;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto L_7702; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+26, b_+28); A = 0x80;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, B);
L_7702:
  CYC(b_+30, b_+31); E = alu_inc8(gb, E);
  CYC(b_+31, b_+32); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto L_76f4; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); H = B;
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x0c);
  CYC(b_+39, b_+42);
  TAIL(objectTakePosition);
}

void s_func_770e_hook(GB *gb) {
  BASE(func_770e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+9, b_+11); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto L_7721; }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+16);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+19);
  goto L_7727;
L_7721:
  CALL_C(b_+19, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+22);
  CYC(b_+22, b_+24); alu_xor(gb, 0x10);
  CYC(b_+24, b_+25); B = A;
L_7727:
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_sub(gb, B);
  CYC(b_+29, b_+31); alu_add(gb, 0x02);
  CYC(b_+31, b_+33); alu_cp(gb, 0x05);
  CYC(b_+33, b_+34); A = B;
  if (!(F & FC)) { CYCT(b_+34, b_+37); TAIL(objectNudgeAngleTowards); }
  CYC(b_+34, b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  RET(b_+40); return;
}

void s_func_7737_hook(GB *gb) {
  BASE(func_7737);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x28);
  if (!(F & FC)) { CYCT(b_+5, b_+7); TAIL_S(func_7740); }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  s_func_7740_hook(gb); return; // falls through
}

void s_func_7740_hook(GB *gb) {
  BASE(func_7740);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x38);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+8); A = alu_swap(gb, A);
  CYC(b_+8, b_+11); SET_HL(SYM(table_7751));
  CYC(b_+11, b_+12); digdogger_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return;
}

void s_func_7757_hook(GB *gb) {
  BASE(func_7757);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+4, b_+6); alu_bit(gb, 1, A);
  if (F & FZ) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); C = 0x0c;
  CYC(b_+9, b_+12);
  TAIL(objectCheckLinkWithinDistance);
}

void s_func_7763_hook(GB *gb) {
  BASE(func_7763);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x3c);
  if (F & FC) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x2b;
  CALL_C(b_+8, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); A = alu_dec8(gb, A);
  CYC(b_+23, b_+25); alu_cp(gb, 0xf5);
  if (F & FC) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CALL_C(b_+26, s_checkObjectsCollided, SYM(checkObjectsCollided), b_+29);
  if (!(F & FC)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x10);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+39, b_+40); B = mem_rd(gb, HL);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+42, b_+43); C = mem_rd(gb, HL);
  CYC(b_+43, b_+44); push_effect(gb, HL);
  CALL_C(b_+44, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+47);
  CYC(b_+47, b_+48); SET_HL(pop_effect(gb));
  CYC(b_+48, b_+50); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  CYC(b_+51, b_+53); A = 0x63;
  CYC(b_+53, b_+56);
  TAIL(playSound_b00);
}

void s_func_779b_hook(GB *gb) {
  BASE(func_779b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); E = L;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+8, b_+11); TAIL(objectSetVisible83); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL(objectSetVisible82);
}

