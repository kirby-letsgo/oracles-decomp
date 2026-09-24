#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/poeSister1.s.
// ENEMY_POE_SISTER_1

static uint16_t poeSister1_jump_table(GB *gb) {
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

static void poeSister1_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// enemyCode76@state11
static void poeSister1_state11(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+365, s_enemyAnimate, SYM(enemyAnimate), b_+368);
  CYC(b_+368, b_+370); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+370, b_+371); A = mem_rd(gb, DE);
  CYC(b_+371, b_+372); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+372, b_+375); TAIL_S(poeSister5f3b); }
  CYC(b_+372, b_+375);
  CYC(b_+375, b_+377); alu_sub(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+377); return; }
  CYC(b_+377, b_+378);
  CALL_C(b_+378, s_func_5f54_hook, SYM(func_5f54), b_+381);
  if (!(F & FZ)) { RET_TAKEN(b_+381); return; }
  CYC(b_+381, b_+382);
  CYC(b_+382, b_+384); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+384, b_+386); A = 0x02;
  CYC(b_+386, b_+387); mem_wr(gb, DE, A);
  RET(b_+387); return;
}

// enemyCode76@state10
static void poeSister1_state10(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto state10;
animate:
  CYC(b_+318, b_+321);
  TAIL(enemyAnimate);
state10:
  CYC(b_+321, b_+322); H = D;
  CYC(b_+322, b_+324); L = ENEMY_BASE + OBJ_VAR34;
  CALL_C(b_+324, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+327);
  CYC(b_+327, b_+328); alu_sub(gb, C);
  CYC(b_+328, b_+330); alu_add(gb, 0x0c);
  CYC(b_+330, b_+332); alu_cp(gb, 0x19);
  if (!(F & FC)) { CYCT(b_+332, b_+334); goto L_5d53; }
  CYC(b_+332, b_+334);
  CYC(b_+334, b_+336); A = mem_rd(gb, hFF8F);
  CYC(b_+336, b_+337); alu_sub(gb, B);
  CYC(b_+337, b_+339); alu_add(gb, 0x07);
  CYC(b_+339, b_+341); alu_cp(gb, 0x0f);
  if (!(F & FC)) { CYCT(b_+341, b_+343); goto L_5d53; }
  CYC(b_+341, b_+343);
  CYC(b_+343, b_+344); L = E;
  CYC(b_+344, b_+345); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+345, b_+347); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+347, b_+348); A = mem_rd(gb, HL);
  CYC(b_+348, b_+350); alu_and(gb, 0x10);
  CYC(b_+350, b_+352); A = alu_swap(gb, A);
  CYC(b_+352, b_+354); alu_add(gb, 0x04);
  CYC(b_+354, b_+356); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+356, b_+357); mem_wr(gb, HL, A);
  CYC(b_+357, b_+360);
  TAIL(enemySetAnimation);
L_5d53:
  CALL_C(b_+360, s_ecom_moveTowardPosition, SYM(ecom_moveTowardPosition_b0d), b_+363);
  CYC(b_+363, b_+365);
  goto animate;
}

// enemyCode76@stateB
static void poeSister1_stateB(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+234, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+237);
  if (!(F & FZ)) { CYCT(b_+237, b_+240); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+237, b_+240);
  CYC(b_+240, b_+242); mem_wr(gb, HL, 0x10);
  CYC(b_+242, b_+243); L = E;
  CYC(b_+243, b_+244); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+244, b_+246); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+246, b_+248); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+248, b_+250); goto L_5ced; }
  CYC(b_+248, b_+250);
  CYC(b_+250, b_+252); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+252, b_+254); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+254, b_+255); A = mem_rd(gb, HL);
  CYC(b_+255, b_+257); alu_add(gb, 0x2c);
  CYC(b_+257, b_+258); mem_wr(gb, HL, A);
L_5ced:
  CYC(b_+258, b_+261);
  TAIL(objectSetInvisible);
}

// enemyCode76@state9
static void poeSister1_state9(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+190, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+193);
  if (!(F & FZ)) { CYCT(b_+193, b_+196); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+193, b_+196);
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0x1f);
  CYC(b_+198, b_+199); L = E;
  CYC(b_+199, b_+200); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+200, b_+203);
  TAIL(objectSetVisible82);
}

// enemyCode76@state8
static void poeSister1_state8(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+173, b_+176); A = mem_rd(gb, wcc93);
  CYC(b_+176, b_+177); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+177); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+179); A = alu_inc8(gb, A);
  CYC(b_+179, b_+182); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+182, b_+183); H = D;
  CYC(b_+183, b_+184); L = E;
  CYC(b_+184, b_+185); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+185, b_+187); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+187, b_+189); mem_wr(gb, HL, 0x2d);
  RET(b_+189); return;
}

// enemyCode76@stateStub
static void poeSister1_stateStub(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  RET(b_+172); return;
}

// enemyCode76@state5
static void poeSister1_state5(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+155, s_ecom_galeSeedEffect, SYM(ecom_galeSeedEffect_b0d), b_+158);
  if (!(F & FC)) { CYCT(b_+158, b_+161); TAIL(enemyDelete); }
  CYC(b_+158, b_+161);
  CYC(b_+161, b_+163); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+165); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+165); return; }
  CYC(b_+165, b_+166);
  CYC(b_+166, b_+169); SET_BC(0x0a08);
  CYC(b_+169, b_+172);
  TAIL(showText);
}

// enemyCode76@func5c6d
static void poeSister1_func5c6d(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+130, b_+132); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x0b);
  CYC(b_+134, b_+136); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x5c);
  CYC(b_+138, b_+140); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0x3c);
  CYC(b_+142, b_+144); A = 0x01;
  CYC(b_+144, b_+147); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CALL_C(b_+147, s_objectSetVisible82, SYM(objectSetVisible82), b_+150);
  CYC(b_+150, b_+152); A = 0x02;
  CYC(b_+152, b_+155);
  TAIL(enemySetAnimation);
}

// enemyCode7e@func_5d86
static void poeSister1_func_5d86(GB *gb) {
  BASE(enemyCode7e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+23, s_poeSister604b_hook, SYM(poeSister604b), b_+26);
  CALL_C(b_+26, s_poeSister602e_hook, SYM(poeSister602e), b_+29);
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); push_effect(gb, b_+33);
  do { uint16_t jt_ = (poeSister1_jump_table(gb));
    if (jt_ == b_+69) goto state0;
    if (jt_ == (b_ - 216)) { poeSister1_stateStub(gb); return; }
    if (jt_ == (b_ - 233)) { poeSister1_state5(gb); return; }
    if (jt_ == (b_ - 215)) { poeSister1_state8(gb); return; }
    if (jt_ == (b_ - 198)) { poeSister1_state9(gb); return; }
    if (jt_ == b_+127) goto stateA;
    if (jt_ == (b_ - 154)) { poeSister1_stateB(gb); return; }
    if (jt_ == b_+137) goto stateC;
    if (jt_ == b_+151) goto stateD;
    if (jt_ == b_+202) goto stateE;
    if (jt_ == (b_ - 67)) { poeSister1_state10(gb); return; }
    if (jt_ == (b_ - 23)) { poeSister1_state11(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+69, b_+71); A = 0x7e;
  CYC(b_+71, b_+74); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CYC(b_+74, b_+76); B = 0x3b;
  CALL_C(b_+76, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+79);
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CALL_C(b_+80, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+83);
  CYC(b_+83, b_+85); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); H = D;
  CYC(b_+87, b_+89); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0xff);
  CYC(b_+91, b_+93); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x3c);
  CYC(b_+95, b_+97); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+99); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+99, b_+102); poeSister1_func5c6d(gb); return; }
  CYC(b_+99, b_+102);
  CYC(b_+102, b_+104); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x08);
  CYC(b_+106, b_+108); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+108, b_+109); A = mem_rd(gb, HL);
  CYC(b_+109, b_+111); alu_add(gb, 0x06);
  CYC(b_+111, b_+112); mem_wr(gb, HL, A);
  CYC(b_+112, b_+114); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+114, b_+115); mem_wr(gb, HL, A);
  CYC(b_+115, b_+117); A = 0x76;
  CYC(b_+117, b_+119); B = 0x00;
  CALL_C(b_+119, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+122);
  CYC(b_+122, b_+124); A = 0x03;
  CYC(b_+124, b_+127);
  TAIL(enemySetAnimation);
stateA:
  CALL_C(b_+127, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+130);
  if (!(F & FZ)) { CYCT(b_+130, b_+132); goto animate; }
  CYC(b_+130, b_+132);
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x2d);
  CYC(b_+134, b_+135); L = E;
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+136); return;
stateC:
  CALL_C(b_+137, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+140);
  if (!(F & FZ)) { RET_TAKEN(b_+140); return; }
  CYC(b_+140, b_+141);
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x30);
  CYC(b_+143, b_+144); L = E;
  CYC(b_+144, b_+145); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+145, s_func_5e7b_hook, SYM(func_5e7b), b_+148);
  CYC(b_+148, b_+151);
  TAIL(objectSetVisible82);
stateD:
  CALL_C(b_+151, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+154);
  if (!(F & FZ)) { CYCT(b_+154, b_+156); goto L_5e1e; }
  CYC(b_+154, b_+156);
  CYC(b_+156, b_+158); mem_wr(gb, HL, 0x30);
  CYC(b_+158, b_+159); L = E;
  CYC(b_+159, b_+160); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+160, b_+162); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+162, b_+164); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+164, b_+166); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+166, b_+168); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+168, b_+169); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  CYC(b_+170, b_+172); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+172, b_+173); A = mem_rd(gb, HL);
  CYC(b_+173, b_+174); mem_wr(gb, DE, A);
  RET(b_+174); return;
L_5e1e:
  CYC(b_+175, b_+176); A = mem_rd(gb, HL);
  CYC(b_+176, b_+178); alu_and(gb, 0x3c);
  CYC(b_+178, b_+179); alu_rrca(gb);
  CYC(b_+179, b_+180); alu_rrca(gb);
  CYC(b_+180, b_+182); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+182, b_+184); goto L_5e29; }
  CYC(b_+182, b_+184);
  CYC(b_+184, b_+185); alu_cpl(gb);
  CYC(b_+185, b_+186); A = alu_inc8(gb, A);
L_5e29:
  CYC(b_+186, b_+188); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+188, b_+190); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+190, b_+192); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+192, b_+194); E = ENEMY_BASE + OBJ_YH;
  if (!(F & FZ)) { CYCT(b_+194, b_+196); goto L_5e36; }
  CYC(b_+194, b_+196);
  CYC(b_+196, b_+197); L = alu_inc8(gb, L);
  CYC(b_+197, b_+199); E = ENEMY_BASE + OBJ_XH;
L_5e36:
  CYC(b_+199, b_+200); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+200, b_+201); mem_wr(gb, DE, A);
  RET(b_+201); return;
stateE:
  CALL_C(b_+202, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+205);
  if (F & FZ) { CYCT(b_+205, b_+208); TAIL_S(poeSister5f3b); }
  CYC(b_+205, b_+208);
  CALL_C(b_+208, s_objectApplySpeed, SYM(objectApplySpeed), b_+211);
animate:
  CYC(b_+211, b_+214);
  TAIL(enemyAnimate);
}

// ENEMY_POE_SISTER_1
void s_enemyCode7e_hook(GB *gb) {
  BASE(enemyCode7e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_5d7c; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x0a06);
  CYC(b_+10, b_+13);
  TAIL_S(poeSister5f7e);
L_5d7c:
  CALL_C(b_+13, s_poeSister5fc2_hook, SYM(poeSister5fc2), b_+16);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
normalStatus:
  CALL_L(b_+17, poeSister1_func_5d86, b_+20);
  CYC(b_+20, b_+23);
  TAIL_S(func_5fec);
}

void s_func_5e45_hook(GB *gb) {
  BASE(func_5e45);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_5e6b));
  CALL_C(b_+3, s_func_5ea3_hook, SYM(func_5ea3), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto L_5e64; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+10, b_+12); alu_cp(gb, 0x58);
  CYC(b_+12, b_+14); A = 0xfe;
  CYC(b_+14, b_+16); C = 0x00;
  if (F & FC) { CYCT(b_+16, b_+18); goto L_5e5a; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); A = 0x02;
  CYC(b_+20, b_+21); C = alu_inc8(gb, C);
L_5e5a:
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+26); alu_add(gb, C);
  CYC(b_+26, b_+29); SET_HL(SYM(table_5e73));
  CYC(b_+29, b_+30); poeSister1_add_a_to_hl(gb, b_+30);
  CYC(b_+30, b_+31); B = mem_rd(gb, HL);
L_5e64:
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+38);
  TAIL(ecom_updateAnimationFromAngle_b0d);
}

void s_func_5e7b_hook(GB *gb) {
  BASE(func_5e7b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_5e9b));
  CALL_C(b_+3, s_func_5ea3_hook, SYM(func_5ea3), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto L_5e89; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+10); alu_add(gb, A);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+13); alu_xor(gb, 0x10);
  CYC(b_+13, b_+14); B = A;
L_5e89:
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+17); A = B;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+32);
  TAIL(ecom_updateAnimationFromAngle_b0d);
}

void s_func_5ea3_hook(GB *gb) {
  BASE(func_5ea3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); TAIL_S(func_5f0b); }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+17); SET_HL(SYM(flags_5f35));
  CALL_C(b_+17, s_checkFlag, SYM(checkFlag), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+22); TAIL_S(func_5f0b); }
  CYC(b_+20, b_+22);
  CALL_C(b_+22, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+25);
  CYC(b_+25, b_+27); alu_and(gb, 0x03);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); C = 0x05;
L_5ec1:
  CYC(b_+30, b_+31); C = alu_dec8(gb, C);
  if (F & FZ) { CYCT(b_+31, b_+33); TAIL_S(func_5f0b); }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+35); A = alu_inc8(gb, A);
  CYC(b_+35, b_+37); alu_and(gb, 0x03);
  CYC(b_+37, b_+38); B = A;
  CYC(b_+38, b_+41); SET_HL(SYM(table_5f37));
  CYC(b_+41, b_+42); poeSister1_add_a_to_hl(gb, b_+42);
  CYC(b_+42, b_+43); L = mem_rd(gb, HL);
  CYC(b_+43, b_+45); H = 0xcf;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_5ec1; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x08);
  CYC(b_+52, b_+53); C = L;
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+55, b_+56); A = L;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); alu_and(gb, 0x06);
  CYC(b_+59, b_+60); SET_HL(pop_effect(gb));
  CYC(b_+60, b_+61); poeSister1_add_a_to_hl(gb, b_+61);
  CYC(b_+61, b_+63); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+63, b_+64); A = C;
  CYC(b_+64, b_+66); alu_and(gb, 0xf0);
  CYC(b_+66, b_+68); alu_add(gb, 0x08);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+71); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+71, b_+72); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+75); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+75, b_+76); SET_HL(HL + 1);
  CYC(b_+76, b_+77); A = C;
  CYC(b_+77, b_+79); alu_and(gb, 0x0f);
  CYC(b_+79, b_+81); A = alu_swap(gb, A);
  CYC(b_+81, b_+83); alu_add(gb, 0x08);
  CYC(b_+83, b_+84); mem_wr(gb, DE, A);
  CYC(b_+84, b_+86); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+86, b_+87); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+87, b_+88); mem_wr(gb, DE, A);
  CYC(b_+88, b_+89); H = D;
  CYC(b_+89, b_+91); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x0f);
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_VAR34;
  CALL_C(b_+95, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+98);
  CALL_C(b_+98, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+101);
  CYC(b_+101, b_+102); B = A;
  CYC(b_+102, b_+103); alu_xor(gb, A);
  RET(b_+103); return;
}

void s_func_5f0b_hook(GB *gb) {
  BASE(func_5f0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5f0b:
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x06);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb));
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CYC(b_+8, b_+9); poeSister1_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto L_5f0b; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); SET_HL(HL + 1);
  CYC(b_+20, b_+22); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+26); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto L_5f0b; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_sub(gb, C);
  if (!(F & FC)) { CYCT(b_+32, b_+34); goto L_5f2f; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); alu_cpl(gb);
  CYC(b_+35, b_+36); A = alu_inc8(gb, A);
L_5f2f:
  CYC(b_+36, b_+37); alu_rlca(gb);
  if (F & FC) { CYCT(b_+37, b_+39); goto L_5f0b; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); SET_HL(pop_effect(gb));
  CYC(b_+40, b_+41); alu_or(gb, D);
  RET(b_+41); return;
}

void s_poeSister5f3b_hook(GB *gb) {
  BASE(poeSister5f3b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0b);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x18);
  RET(b_+13); return;
}

void s_func_5f49_hook(GB *gb) {
  BASE(func_5f49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x3c;
  CYC(b_+8, b_+11);
  TAIL(ecom_spawnProjectile_b0d);
}

void s_func_5f54_hook(GB *gb) {
  BASE(func_5f54);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x06);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_and(gb, 0xf0);
  CYC(b_+11, b_+13); alu_add(gb, 0x08);
  CYC(b_+13, b_+15); L = PART_BASE + OBJ_YH;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+21); A = alu_swap(gb, A);
  CYC(b_+21, b_+23); alu_add(gb, 0x08);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+28); SET_HL(wNumTorchesLit);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_5f75; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
L_5f75:
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); C = A;
  CYC(b_+35, b_+37); A = 0x08;
  CALL_C(b_+37, s_setTile, SYM(setTile), b_+40);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  RET(b_+41); return;
}

void s_poeSister5f7e_hook(GB *gb) {
  BASE(poeSister5f7e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); TAIL_S(poeSister5f82); }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto L_5f95; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = mem_rd(gb, wTextIsActive);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CALL_C(b_+17, s_showText, SYM(showText), b_+20);
  CALL_C(b_+20, s_objectSetInvisible, SYM(objectSetInvisible), b_+23);
L_5f95:
  CYC(b_+23, b_+25); A = 0x00;
  CALL_C(b_+25, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+33); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+36);
  TAIL(enemyDie_withoutItemDrop);
}

void s_poeSister5f82_hook(GB *gb) {
  BASE(poeSister5f82);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_5fb9; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+11); SET_BC(0x0a04);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_ID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x76);
  if (F & FZ) { CYCT(b_+16, b_+18); goto L_5fb6; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); C = 0x02;
L_5fb6:
  CYC(b_+20, b_+23);
  TAIL(showText);
L_5fb9:
  CALL_C(b_+23, s_objectCreatePuff, SYM(objectCreatePuff), b_+26);
  CALL_C(b_+26, s_decNumEnemies, SYM(decNumEnemies), b_+29);
  CYC(b_+29, b_+32);
  TAIL(enemyDelete);
}

void s_poeSister5fc2_hook(GB *gb) {
  BASE(poeSister5fc2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_cp(gb, 0x0f);
  CYC(b_+18, b_+19); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+25, b_+27); goto L_5fe4; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+29, b_+30); L = mem_rd(gb, HL);
  CYC(b_+30, b_+32); H = 0xcf;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x09);
L_5fe4:
  CALL_C(b_+34, s_poeSister5f3b_hook, SYM(poeSister5f3b), b_+37);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_or(gb, A);
  RET(b_+41); return;
}

void s_func_5fec_hook(GB *gb) {
  BASE(func_5fec);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+9, b_+11); goto L_5ffb; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_sub(gb, 0x04);
  CYC(b_+13, b_+14); alu_add(gb, A);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);
L_5ffb:
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+20); SET_HL(SYM(table_601e));
  CYC(b_+20, b_+21); poeSister1_add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+22); A = B;
  CALL_C(b_+22, s_checkFlag, SYM(checkFlag), b_+25);
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_6015; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_and(gb, 0x80);
  CYC(b_+35, b_+37); alu_or(gb, 0x7e);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfb);
  RET(b_+40); return;
L_6015:
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+44); alu_and(gb, 0x80);
  CYC(b_+44, b_+46); alu_or(gb, 0x76);
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+49); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  RET(b_+49); return;
}

void s_poeSister602e_hook(GB *gb) {
  BASE(poeSister602e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_cp(gb, 0x08);
  if (F & FC) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_and(gb, 0x18);
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+16); alu_rlca(gb);
  CYC(b_+16, b_+19); SET_HL(SYM(table_6047));
  CYC(b_+19, b_+20); poeSister1_add_a_to_hl(gb, b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return;
}

void s_poeSister604b_hook(GB *gb) {
  BASE(poeSister604b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+8, b_+11); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+14); goto L_605c; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  RET(b_+16); return;
L_605c:
  CYC(b_+17, b_+19); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_6066; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x18);
L_6066:
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, wWarpTransition2);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+40); return; }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+43); A = 0x8d;
  CALL_C(b_+43, s_playSound, SYM(playSound_b00), b_+46);
  CYC(b_+46, b_+49); SET_HL(b_+52 /* @warpDest */);
  CYC(b_+49, b_+52);
  TAIL(setWarpDestVariables);
}

