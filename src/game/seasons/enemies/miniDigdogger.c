#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/miniDigdogger.s.
// ENEMY_MINI_DIGDOGGER

static uint16_t miniDigdogger_jump_table(GB *gb) {
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

static void miniDigdogger_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// enemyCode55@bounceOffWallsAndHoles
static void miniDigdogger_bounceOffWallsAndHoles(GB *gb) {
  BASE(enemyCode55);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+220, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+223);
  CALL_C(b_+223, s_seasonsFunc_0d_74ce_hook, SYM(seasonsFunc_0d_74ce), b_+226);
  CYC(b_+226, b_+229);
  TAIL(objectApplySpeed);
}

// ENEMY_MINI_DIGDOGGER
void s_enemyCode55_hook(GB *gb) {
  BASE(enemyCode55);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_7421; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_7421; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto L_7410; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e);
  CYC(b_+19, b_+21); A = 0x69;
  CALL_C(b_+21, s_playSound, SYM(playSound_b00), b_+24);
  CYC(b_+24, b_+26); A = 0x32;
  CALL_C(b_+26, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+29);
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+30, b_+31); L = alu_dec8(gb, L);
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+32, b_+34); goto L_7417; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = 0x04;
  CALL_C(b_+36, s_enemySetAnimation, SYM(enemySetAnimation), b_+39);
L_7410:
  CALL_C(b_+39, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+42);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+46);
  TAIL(enemyDie_uncounted);
L_7417:
  CYC(b_+46, b_+48); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x00);
  CALL_C(b_+50, s_objectCopyPosition, SYM(objectCopyPosition), b_+53);
  CYC(b_+53, b_+56);
  TAIL(enemyDelete);
L_7421:
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); push_effect(gb, b_+60);
  do { uint16_t jt_ = (miniDigdogger_jump_table(gb));
    if (jt_ == b_+86) goto state0;
    if (jt_ == b_+123) goto state_stub;
    if (jt_ == b_+124) goto state8;
    if (jt_ == b_+142) goto state9;
    if (jt_ == b_+162) goto stateA;
    if (jt_ == b_+177) goto stateB;
    if (jt_ == b_+211) goto stateC;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+86, b_+89); SET_BC(0x011f);
  CALL_C(b_+89, s_ecom_randomBitwiseAndBCE, SYM(ecom_randomBitwiseAndBCE_b0d), b_+92);
  CYC(b_+92, b_+94); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+94, b_+95); A = C;
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  CYC(b_+96, b_+97); A = B;
  CYC(b_+97, b_+100); SET_HL(b_+121 /* @seasonsTable_0d_7462 */);
  CYC(b_+100, b_+101); miniDigdogger_add_a_to_hl(gb, b_+101);
  CYC(b_+101, b_+103); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+103, b_+104); A = mem_rd(gb, HL);
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CALL_C(b_+105, s_seasonsFunc_0d_74ce_hook, SYM(seasonsFunc_0d_74ce), b_+108);
  CYC(b_+108, b_+109); H = D;
  CYC(b_+109, b_+111); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+111, b_+113); A = 0x80;
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0xfe);
  CYC(b_+116, b_+118); A = 0x32;
  CYC(b_+118, b_+121);
  TAIL(ecom_setSpeedAndState8AndVisible_b0d);

state_stub:
  RET(b_+123); return;
state8:
  CYC(b_+124, b_+126); C = 0x0e;
  CALL_C(b_+126, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+129);
  if (!(F & FZ)) { CYCT(b_+129, b_+131); goto bounceOffWallsAndHoles; }
  CYC(b_+129, b_+131);
  CYC(b_+131, b_+133); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+133, b_+134); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+134, b_+136); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+136, b_+138); A = 0x00;
  CYC(b_+138, b_+139); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0xff);
  RET(b_+141); return;
state9:
  CALL_C(b_+142, s_seasonsFunc_0d_7523_hook, SYM(seasonsFunc_0d_7523), b_+145);
  CYC(b_+145, b_+147); C = 0x0f;
  CALL_C(b_+147, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+150);
  if (!(F & FZ)) { CYCT(b_+150, b_+152); goto bounceOffWallsAndHoles; }
  CYC(b_+150, b_+152);
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+154, b_+155); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+155, b_+157); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+157, b_+158); A = mem_rd(gb, HL);
  CYC(b_+158, b_+160); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+160, b_+161); mem_wr(gb, HL, A);
  RET(b_+161); return;
stateA:
  CALL_C(b_+162, s_seasonsFunc_0d_7547_hook, SYM(seasonsFunc_0d_7547), b_+165);
  CALL_C(b_+165, s_seasonsFunc_0d_7523_hook, SYM(seasonsFunc_0d_7523), b_+168);
  CALL_C(b_+168, s_seasonsFunc_0d_74e1_hook, SYM(seasonsFunc_0d_74e1), b_+171);
  CALL_L(b_+171, miniDigdogger_bounceOffWallsAndHoles, b_+174);
  CYC(b_+174, b_+177);
  TAIL(enemyAnimate);
stateB:
  CYC(b_+177, b_+179); C = 0x10;
  CALL_C(b_+179, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+182);
  CYC(b_+182, b_+184); L = 0x95;
  CYC(b_+184, b_+185); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+185, b_+186); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+186, b_+188); goto L_74af; }
  CYC(b_+186, b_+188);
  CYC(b_+188, b_+189); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+189, b_+190); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+190, b_+192); goto L_74af; }
  CYC(b_+190, b_+192);
  CYC(b_+192, b_+194); mem_wr(gb, HL, 0x02);
  CYC(b_+194, b_+196); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0x6e);
L_74af:
  CYC(b_+198, b_+200); L = 0x97;
  CYC(b_+200, b_+201); H = mem_rd(gb, HL);
  CALL_C(b_+201, s_seasonsFunc_0d_74fe_hook, SYM(seasonsFunc_0d_74fe), b_+204);
  if (!(F & FC)) { CYCT(b_+204, b_+206); goto animateAndApplySpeed; }
  CYC(b_+204, b_+206);
  CYC(b_+206, b_+208); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+208, b_+209); A = mem_rd(gb, DE);
  CYC(b_+209, b_+210); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+210); return; }
  CYC(b_+210, b_+211);
stateC:
  CYC(b_+211, b_+213); A = 0x32;
  CALL_C(b_+213, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+216);
  CYC(b_+216, b_+217); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+217, b_+220);
  TAIL(enemyDelete);
bounceOffWallsAndHoles:
  CALL_C(b_+220, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+223);
animateAndApplySpeed:
  CALL_C(b_+223, s_seasonsFunc_0d_74ce_hook, SYM(seasonsFunc_0d_74ce), b_+226);
  CYC(b_+226, b_+229);
  TAIL(objectApplySpeed);
}

void s_seasonsFunc_0d_74ce_hook(GB *gb) {
  BASE(seasonsFunc_0d_74ce);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_add(gb, 0x04);
  CYC(b_+8, b_+10); alu_and(gb, 0x18);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+13); alu_rlca(gb);
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+19);
  TAIL(enemySetAnimation);
}

void s_seasonsFunc_0d_74e1_hook(GB *gb) {
  BASE(seasonsFunc_0d_74e1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+10); alu_cp(gb, H);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_74ee; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_74ee:
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x28);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+19, b_+21); A = 0xc0;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0xfc);
  CYC(b_+24, b_+26); A = 0x59;
  CYC(b_+26, b_+29);
  TAIL(playSound_b00);
}

void s_seasonsFunc_0d_74fe_hook(GB *gb) {
  BASE(seasonsFunc_0d_74fe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); E = L;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); mem_wr(gb, hFF8F, A);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); E = L;
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); mem_wr(gb, hFF8E, A);
  CYC(b_+14, b_+15); alu_sub(gb, C);
  CYC(b_+15, b_+17); alu_add(gb, 0x02);
  CYC(b_+17, b_+19); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto L_751b; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = mem_rd(gb, hFF8F);
  CYC(b_+23, b_+24); alu_sub(gb, B);
  CYC(b_+24, b_+26); alu_add(gb, 0x02);
  CYC(b_+26, b_+28); alu_cp(gb, 0x05);
  if (F & FC) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
L_751b:
  CALL_C(b_+29, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+32);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); alu_or(gb, D);
  RET(b_+36); return;
}

void s_seasonsFunc_0d_7523_hook(GB *gb) {
  BASE(seasonsFunc_0d_7523);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+7); E = L;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+11); alu_add(gb, 0x0a);
  CYC(b_+11, b_+13); alu_cp(gb, 0x15);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+16, b_+17); E = L;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); alu_add(gb, 0x0a);
  CYC(b_+21, b_+23); alu_cp(gb, 0x15);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_cp(gb, 0x14);
  if (F & FC) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); SET_HL(pop_effect(gb));
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  RET(b_+35); return;
}

void s_seasonsFunc_0d_7547_hook(GB *gb) {
  BASE(seasonsFunc_0d_7547);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0xa3;
  CYC(b_+8, b_+11);
  TAIL(playSound_b00);
}

