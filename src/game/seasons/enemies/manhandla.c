#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/manhandla.s.
// ENEMY_MANHANDLA

static uint16_t manhandla_jump_table(GB *gb) {
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

static void manhandla_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// enemyCode7d@subid1@func_78ce
static void manhandla_subid1_func_78ce(GB *gb) {
  BASE(enemyCode7d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto subid1_func_78ce;
subid1_animate:
  CYC(b_+286, b_+289);
  TAIL(enemyAnimate);

subid1_func_78ce:
  CYC(b_+293, b_+294); L = E;
  CYC(b_+294, b_+296); mem_wr(gb, HL, 0x09);
  CALL_C(b_+296, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+299);
  CYC(b_+299, b_+301); alu_and(gb, 0x07);
  CYC(b_+301, b_+304); SET_HL(b_+333 /* @subid1@table_78f6 */);
  CYC(b_+304, b_+305); manhandla_add_a_to_hl(gb, b_+305);
  CYC(b_+305, b_+307); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+307, b_+308); A = mem_rd(gb, HL);
  CYC(b_+308, b_+309); mem_wr(gb, DE, A);
  CYC(b_+309, b_+312); SET_BC(0x5078);
  CALL_C(b_+312, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+315);
  CYC(b_+315, b_+316); push_effect(gb, AF);
  CALL_C(b_+316, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+319);
  CYC(b_+319, b_+321); alu_and(gb, 0x01);
  CYC(b_+321, b_+322); SET_AF(pop_effect(gb));
  if (F & FZ) { CYCT(b_+322, b_+324); goto L_78f1; }
  CYC(b_+322, b_+324);
  CYC(b_+324, b_+326); alu_sub(gb, 0x02);
  CYC(b_+326, b_+328); alu_and(gb, 0x1f);
L_78f1:
  CYC(b_+328, b_+330); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+330, b_+331); mem_wr(gb, DE, A);
  CYC(b_+331, b_+333);
  goto subid1_animate;
}

// ENEMY_MANHANDLA
void s_enemyCode7d_hook(GB *gb) {
  BASE(enemyCode7d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto L_77c2; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto normalStatus; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+15, b_+18); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+19); A = alu_dec8(gb, A);
  if (F & FZ) CALL_C_CC(b_+19, s_ecom_killRelatedObj1, SYM(ecom_killRelatedObj1_b0d), b_+22);
  else CYC(b_+19, b_+22);
  CYC(b_+22, b_+25);
  TAIL(enemyDie_uncounted);
L_77c2:
  CALL_C(b_+25, s_func_7a44_hook, SYM(func_7a44), b_+28);
normalStatus:
  CALL_C(b_+28, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+31);

L_77c7:
  CYC(b_+30, b_+31); B = H;
  if (!(F & FC)) { CYCT(b_+31, b_+33); goto L_77db; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+34); push_effect(gb, b_+34);
  do { uint16_t jt_ = (manhandla_jump_table(gb));
    if (jt_ == b_+65) goto state0;
    if (jt_ == b_+133) goto state1;
    if (jt_ == b_+205) goto stateStub;
    if (jt_ == b_+92) goto L_7805;
    if (jt_ == b_+30) goto L_77c7;
    HANDOFF(HL);
  } while (0);
L_77db:
  CYC(b_+50, b_+51); B = alu_dec8(gb, B);
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+53); push_effect(gb, b_+53);
  do { uint16_t jt_ = (manhandla_jump_table(gb));
    if (jt_ == b_+206) goto subid1;
    if (jt_ == b_+457) goto subid2;
    if (jt_ == b_+564) goto subid3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+65, b_+66); A = B;
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); goto L_77f7; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); A = 0x7d;
  CYC(b_+71, b_+73); B = 0x85;
  CALL_C(b_+73, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+76);
  CYC(b_+76, b_+78);
  goto state1;
L_77f7:
  CYC(b_+78, b_+79); A = alu_dec8(gb, A);
  CYC(b_+79, b_+82); SET_HL(b_+127 /* @table_7828 */);
  CYC(b_+82, b_+83); manhandla_add_a_to_hl(gb, b_+83);
  CYC(b_+83, b_+85); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CALL_C(b_+87, s_enemySetAnimation, SYM(enemySetAnimation), b_+90);
  CALL_C(b_+90, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+93);

L_7805:
  CYC(b_+92, b_+93); B = E;
  CYC(b_+93, b_+95); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+98); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+98, b_+100); goto L_7813; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+101, b_+103); goto L_7825; }
  CYC(b_+101, b_+103);
  CYC(b_+103, b_+106);
  TAIL(objectSetInvisible);
L_7813:
  CALL_C(b_+106, s_func_7a14_hook, SYM(func_7a14), b_+109);
  CYC(b_+109, b_+111); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+111, b_+113); A = 0x03;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
  CYC(b_+114, b_+116); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+116, b_+117); A = mem_rd(gb, DE);
  CYC(b_+117, b_+119); alu_sub(gb, 0x04);
  CYC(b_+119, b_+121); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+121, b_+124); TAIL(objectSetVisible82); }
  CYC(b_+121, b_+124);
L_7825:
  CYC(b_+124, b_+127);
  TAIL(objectSetVisible83);

state1:
  CYC(b_+133, b_+135); B = 0x06;
  CALL_C(b_+135, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+138);
  if (!(F & FZ)) { RET_TAKEN(b_+138); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); B = 0x7d;
  CALL_C(b_+141, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+144);
  CYC(b_+144, b_+146); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+146, b_+147); E = L;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); mem_wr(gb, HL, A);
  CALL_C(b_+149, s_objectCopyPosition, SYM(objectCopyPosition), b_+152);
  CYC(b_+152, b_+153); push_effect(gb, HL);
  CYC(b_+153, b_+154); C = H;
  CALL_C(b_+154, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+157);
  CYC(b_+157, b_+158); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+158, s_objectCopyPosition, SYM(objectCopyPosition), b_+161);
  CALL_C(b_+161, s_func_7a3d_hook, SYM(func_7a3d), b_+164);
  CYC(b_+164, b_+165); A = H;
  CYC(b_+165, b_+168); SET_HL(hFF8A);
  CYC(b_+168, b_+169); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+169, b_+171); A = 0x04;
L_7854:
  CYC(b_+171, b_+173); mem_wr(gb, hFF8F, A);
  CYC(b_+173, b_+174); push_effect(gb, HL);
  CALL_C(b_+174, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+177);
  CALL_C(b_+177, s_func_7a1f_hook, SYM(func_7a1f), b_+180);
  CYC(b_+180, b_+181); A = H;
  CYC(b_+181, b_+182); SET_HL(pop_effect(gb));
  CYC(b_+182, b_+183); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+183, b_+185); A = mem_rd(gb, hFF8F);
  CYC(b_+185, b_+186); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+186, b_+188); goto L_7854; }
  CYC(b_+186, b_+188);
  CYC(b_+188, b_+189); SET_HL(pop_effect(gb));
  CYC(b_+189, b_+192); SET_BC(hFF8A);
  CYC(b_+192, b_+194); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+194, b_+196); E = 0x05;
L_786d:
  CYC(b_+196, b_+197); A = mem_rd(gb, BC);
  CYC(b_+197, b_+198); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+198, b_+199); C = alu_inc8(gb, C);
  CYC(b_+199, b_+200); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+200, b_+202); goto L_786d; }
  CYC(b_+200, b_+202);
  CYC(b_+202, b_+205);
  TAIL(enemyDelete);
stateStub:
  RET(b_+205); return;
subid1:
  CALL_C(b_+206, s_func_7ac8_hook, SYM(func_7ac8), b_+209);
  CYC(b_+209, b_+211); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+211, b_+212); A = mem_rd(gb, DE);
  CYC(b_+212, b_+214); alu_sub(gb, 0x08);
  CYC(b_+214, b_+215); push_effect(gb, b_+215);
  do { uint16_t jt_ = (manhandla_jump_table(gb));
    if (jt_ == b_+229) goto subid1_state8;
    if (jt_ == b_+267) goto subid1_state9;
    if (jt_ == b_+289) goto subid1_stateA;
    if (jt_ == b_+341) goto subid1_stateB;
    if (jt_ == b_+370) goto subid1_stateC;
    if (jt_ == b_+395) goto subid1_stateD;
    if (jt_ == b_+416) goto subid1_stateE;
    HANDOFF(HL);
  } while (0);
subid1_state8:
  CYC(b_+229, b_+232); A = mem_rd(gb, wcc93);
  CYC(b_+232, b_+233); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+233); return; }
  CYC(b_+233, b_+234);
  CYC(b_+234, b_+235); H = D;
  CYC(b_+235, b_+237); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0a);
  CYC(b_+239, b_+241); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0x61);
  CYC(b_+243, b_+245); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+245, b_+247); mem_wr(gb, HL, 0x04);
  CYC(b_+247, b_+248); L = alu_inc8(gb, L);
  CYC(b_+248, b_+250); mem_wr(gb, HL, 0x58);
  CYC(b_+250, b_+251); L = alu_inc8(gb, L);
  CYC(b_+251, b_+253); mem_wr(gb, HL, 0x78);
  CYC(b_+253, b_+254); L = alu_inc8(gb, L);
  CYC(b_+254, b_+256); mem_wr(gb, HL, 0xff);
  CALL_L(b_+256, manhandla_subid1_func_78ce, b_+259);
  CYC(b_+259, b_+261); A = 0x2e;
  CYC(b_+261, b_+264); mem_wr(gb, wActiveMusic, A);
  CYC(b_+264, b_+267);
  TAIL(playSound_b00);
subid1_state9:
  CALL_C(b_+267, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+270);
  if (!(F & FZ)) { CYCT(b_+270, b_+272); goto subid1_bounceAndApplySpeed; }
  CYC(b_+270, b_+272);
  CYC(b_+272, b_+274); mem_wr(gb, HL, 0x78);
  CYC(b_+274, b_+275); L = E;
  CYC(b_+275, b_+276); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+276, b_+277); alu_xor(gb, A);
  CALL_C(b_+277, s_enemySetAnimation, SYM(enemySetAnimation), b_+280);
subid1_bounceAndApplySpeed:
  CALL_C(b_+280, s_ecom_bounceOffWallsAndHoles, SYM(ecom_bounceOffWallsAndHoles_b0d), b_+283);
  CALL_C(b_+283, s_objectApplySpeed, SYM(objectApplySpeed), b_+286);
subid1_animate:
  CYC(b_+286, b_+289);
  TAIL(enemyAnimate);
subid1_stateA:
  CALL_C(b_+289, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+292);
  if (!(F & FZ)) { RET_TAKEN(b_+292); return; }
  CYC(b_+292, b_+293);
  CYC(b_+293, b_+294); L = E;
  CYC(b_+294, b_+296); mem_wr(gb, HL, 0x09);
  CALL_C(b_+296, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+299);
  CYC(b_+299, b_+301); alu_and(gb, 0x07);
  CYC(b_+301, b_+304); SET_HL(b_+333 /* @subid1@table_78f6 */);
  CYC(b_+304, b_+305); manhandla_add_a_to_hl(gb, b_+305);
  CYC(b_+305, b_+307); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+307, b_+308); A = mem_rd(gb, HL);
  CYC(b_+308, b_+309); mem_wr(gb, DE, A);
  CYC(b_+309, b_+312); SET_BC(0x5078);
  CALL_C(b_+312, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+315);
  CYC(b_+315, b_+316); push_effect(gb, AF);
  CALL_C(b_+316, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+319);
  CYC(b_+319, b_+321); alu_and(gb, 0x01);
  CYC(b_+321, b_+322); SET_AF(pop_effect(gb));
  if (F & FZ) { CYCT(b_+322, b_+324); goto L_78f1; }
  CYC(b_+322, b_+324);
  CYC(b_+324, b_+326); alu_sub(gb, 0x02);
  CYC(b_+326, b_+328); alu_and(gb, 0x1f);
L_78f1:
  CYC(b_+328, b_+330); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+330, b_+331); mem_wr(gb, DE, A);
  CYC(b_+331, b_+333);
  goto subid1_animate;

subid1_stateB:
  CALL_C(b_+341, s_func_7ab4_hook, SYM(func_7ab4), b_+344);
  if (!(F & FC)) { CYCT(b_+344, b_+346); goto L_7913; }
  CYC(b_+344, b_+346);
  CYC(b_+346, b_+347); L = E;
  CYC(b_+347, b_+348); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+348, b_+350); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+350, b_+352); mem_wr(gb, HL, 0x00);
  CYC(b_+352, b_+354); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+354, b_+356); mem_wr(gb, HL, 0x04);
  CYC(b_+356, b_+358); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+358, b_+360); mem_wr(gb, HL, 0x55);
  CYC(b_+360, b_+362);
  goto subid1_animate;
L_7913:
  CALL_C(b_+362, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+365);
  CYC(b_+365, b_+367); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+367, b_+368); mem_wr(gb, DE, A);
  CYC(b_+368, b_+370);
  goto subid1_bounceAndApplySpeed;
subid1_stateC:
  CALL_C(b_+370, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+373);
  if (!(F & FZ)) { CYCT(b_+373, b_+375); goto subid1_bounceAndApplySpeed; }
  CYC(b_+373, b_+375);
  CYC(b_+375, b_+377); mem_wr(gb, HL, 0x04);
  CYC(b_+377, b_+379); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+379, b_+381); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+381, b_+382); A = mem_rd(gb, DE);
  CYC(b_+382, b_+383); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+383, b_+385); alu_and(gb, 0x1f);
  CYC(b_+385, b_+386); mem_wr(gb, DE, A);
  CYC(b_+386, b_+387); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+387, b_+389); goto subid1_bounceAndApplySpeed; }
  CYC(b_+387, b_+389);
  CYC(b_+389, b_+390); A = mem_rd(gb, HL);
  CYC(b_+390, b_+391); alu_cpl(gb);
  CYC(b_+391, b_+392); A = alu_inc8(gb, A);
  CYC(b_+392, b_+393); mem_wr(gb, HL, A);
  CYC(b_+393, b_+395);
  goto subid1_bounceAndApplySpeed;
subid1_stateD:
  CALL_C(b_+395, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+398);
  if (!(F & FZ)) { CYCT(b_+398, b_+400); goto subid1_animateAndUpdateMovingPlatform; }
  CYC(b_+398, b_+400);
  CYC(b_+400, b_+402); mem_wr(gb, HL, 0x3c);
  CYC(b_+402, b_+404); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+404, b_+405); A = mem_rd(gb, HL);
  CYC(b_+405, b_+406); A = alu_dec8(gb, A);
  CYC(b_+406, b_+407); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+407, b_+409); goto L_7946; }
  CYC(b_+407, b_+409);
  CYC(b_+409, b_+411); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+411, b_+413); mem_wr(gb, HL, 0x0b);
L_7946:
  CYC(b_+413, b_+416);
  TAIL(enemySetAnimation);
subid1_stateE:
  CALL_C(b_+416, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+419);
  if (!(F & FZ)) { CYCT(b_+419, b_+421); goto L_7960; }
  CYC(b_+419, b_+421);
  CYC(b_+421, b_+422); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+422, b_+424); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+424, b_+425); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+425, b_+427); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+427, b_+429); mem_wr(gb, HL, 0xfd);
  CYC(b_+429, b_+431); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+431, b_+432); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+432, b_+433); A = mem_rd(gb, HL);
  CALL_C(b_+433, s_enemySetAnimation, SYM(enemySetAnimation), b_+436);
  CYC(b_+436, b_+439);
  TAIL(objectSetVisible82);
L_7960:
  CYC(b_+439, b_+440); A = mem_rd(gb, HL);
  CYC(b_+440, b_+442); alu_cp(gb, 0x78);
  if (!(F & FZ)) { CYCT(b_+442, b_+444); goto subid1_animateAndUpdateMovingPlatform; }
  CYC(b_+442, b_+444);
  CYC(b_+444, b_+446); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+446, b_+447); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+447, b_+448); A = mem_rd(gb, HL);
  CYC(b_+448, b_+451);
  TAIL(enemySetAnimation);
subid1_animateAndUpdateMovingPlatform:
  CALL_C(b_+451, s_enemyAnimate, SYM(enemyAnimate), b_+454);
  CYC(b_+454, b_+457);
  TAIL(ecom_updateMovingPlatform_b0d);
subid2:
  CALL_C(b_+457, s_func_7ad6_hook, SYM(func_7ad6), b_+460);
  CYC(b_+460, b_+462); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+462, b_+463); A = mem_rd(gb, DE);
  CYC(b_+463, b_+465); alu_sub(gb, 0x08);
  CYC(b_+465, b_+466); push_effect(gb, b_+466);
  do { uint16_t jt_ = (manhandla_jump_table(gb));
    if (jt_ == b_+472) goto subid2_state8;
    if (jt_ == b_+483) goto subid2_state9;
    if (jt_ == b_+517) goto subid2_stateA;
    HANDOFF(HL);
  } while (0);
subid2_state8:
  CYC(b_+472, b_+473); H = D;
  CYC(b_+473, b_+474); L = E;
  CYC(b_+474, b_+475); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+475, b_+477); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+477, b_+479); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+479, b_+480); L = alu_inc8(gb, L);
  CYC(b_+480, b_+482); mem_wr(gb, HL, 0x63);
  RET(b_+482); return;
subid2_state9:
  CYC(b_+483, b_+485); A = 0x04;
  CALL_C(b_+485, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+488);
  CYC(b_+488, b_+489); A = mem_rd(gb, HL);
  CYC(b_+489, b_+491); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+491); return; }
  CYC(b_+491, b_+492);
  CYC(b_+492, b_+493); H = D;
  CYC(b_+493, b_+495); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+495, b_+496); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+496, b_+498); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+498, b_+500); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+500, b_+502); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+502, b_+504); mem_wr(gb, HL, 0xf9);
  CYC(b_+504, b_+506); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+506, b_+507); alu_xor(gb, A);
  CYC(b_+507, b_+508); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+508, b_+509); mem_wr(gb, HL, A);
  CALL_C(b_+509, s_objectSetVisible81, SYM(objectSetVisible81), b_+512);
  CYC(b_+512, b_+514); A = 0x05;
  CYC(b_+514, b_+517);
  TAIL(enemySetAnimation);
subid2_stateA:
  CYC(b_+517, b_+519); A = 0x04;
  CALL_C(b_+519, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+522);
  CYC(b_+522, b_+523); A = mem_rd(gb, HL);
  CYC(b_+523, b_+525); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+525, b_+527); goto L_79c3; }
  CYC(b_+525, b_+527);
  CYC(b_+527, b_+528); H = D;
  CYC(b_+528, b_+530); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+530, b_+531); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+531, b_+533); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+533, b_+535); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+535, b_+538);
  TAIL(objectSetInvisible);
L_79c3:
  CYC(b_+538, b_+540); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+540, b_+541); A = mem_rd(gb, HL);
  CYC(b_+541, b_+543); alu_cp(gb, 0x78);
  if (!(F & FC)) { RET_TAKEN(b_+543); return; }
  CYC(b_+543, b_+544);
  CYC(b_+544, b_+546); alu_add(gb, 0x03);
  CYC(b_+546, b_+548); alu_and(gb, 0x0c);
  CYC(b_+548, b_+549); alu_rrca(gb);
  CYC(b_+549, b_+550); alu_rrca(gb);
  CYC(b_+550, b_+553); SET_HL(b_+560 /* @subid2@table_79d9 */);
  CYC(b_+553, b_+554); manhandla_add_a_to_hl(gb, b_+554);
  CYC(b_+554, b_+556); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+556, b_+557); A = mem_rd(gb, DE);
  CYC(b_+557, b_+558); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+558, b_+559); mem_wr(gb, DE, A);
  RET(b_+559); return;

subid3:
  CYC(b_+564, b_+565); A = mem_rd(gb, DE);
  CYC(b_+565, b_+567); alu_sub(gb, 0x08);
  CYC(b_+567, b_+568); push_effect(gb, b_+568);
  do { uint16_t jt_ = (manhandla_jump_table(gb));
    if (jt_ == b_+572) goto subid6_state8;
    if (jt_ == b_+595) goto subid6_state9;
    HANDOFF(HL);
  } while (0);
subid6_state8:
  CALL_C(b_+572, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+575);
  if (!(F & FZ)) { CYCT(b_+575, b_+577); goto subid6_toFunc7ad6; }
  CYC(b_+575, b_+577);
  CALL_C(b_+577, s_func_7b1c_hook, SYM(func_7b1c), b_+580);
  if (F & FC) { CYCT(b_+580, b_+582); goto subid6_toFunc7ad6; }
  CYC(b_+580, b_+582);
  CALL_C(b_+582, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+585);
  CYC(b_+585, b_+587); alu_and(gb, 0x50);
  CYC(b_+587, b_+589); alu_add(gb, 0x5a);
  CYC(b_+589, b_+591); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+591, b_+592); mem_wr(gb, DE, A);
subid6_toFunc7ad6:
  CYC(b_+592, b_+595);
  TAIL_S(func_7ad6);
subid6_state9:
  CALL_C(b_+595, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+598);
  if (F & FZ) { CYCT(b_+598, b_+600); goto L_7a0d; }
  CYC(b_+598, b_+600);
  CYC(b_+600, b_+601); A = mem_rd(gb, HL);
  CYC(b_+601, b_+603); alu_cp(gb, 0x5a);
  if (!(F & FZ)) { CYCT(b_+603, b_+605); goto subid6_toFunc7ad6; }
  CYC(b_+603, b_+605);
  CYC(b_+605, b_+607); B = 0x31;
  CALL_C(b_+607, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+610);
  CYC(b_+610, b_+612);
  goto subid6_toFunc7ad6;
L_7a0d:
  CYC(b_+612, b_+614); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+614, b_+615); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+615, b_+616); A = mem_rd(gb, HL);
  CALL_C(b_+616, s_enemySetAnimation, SYM(enemySetAnimation), b_+619);
  s_func_7a14_hook(gb); return; // falls through
}

// enemyCode7d+582
static void manhandla_enemyCode7d_582(GB *gb) {
  BASE(enemyCode7d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+582, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+585);
  CYC(b_+585, b_+587); alu_and(gb, 0x50);
  CYC(b_+587, b_+589); alu_add(gb, 0x5a);
  CYC(b_+589, b_+591); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+591, b_+592); mem_wr(gb, DE, A);
  CYC(b_+592, b_+595);
  TAIL_S(func_7ad6);
}

void s_func_7a14_hook(GB *gb) {
  BASE(func_7a14);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x08);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x0a);
  CYC(b_+9, b_+11);
  manhandla_enemyCode7d_582(gb); return;
}

void s_func_7a1f_hook(GB *gb) {
  BASE(func_7a1f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+4); A = mem_rd(gb, hFF8F);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+7); A = 0x07;
  CYC(b_+7, b_+8); alu_sub(gb, B);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CALL_C(b_+9, s_func_7af2_hook, SYM(func_7af2), b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+23); C = A;
  CYC(b_+23, b_+24); SET_HL(pop_effect(gb));
  CYC(b_+24, b_+25); L = E;
  CYC(b_+25, b_+26); mem_wr(gb, HL, C);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+28, b_+29); mem_wr(gb, HL, B);
  CYC(b_+29, b_+30); SET_BC(pop_effect(gb));
  s_func_7a3d_hook(gb); return; // falls through
}

void s_func_7a3d_hook(GB *gb) {
  BASE(func_7a3d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+2, b_+4); A = 0x80;
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, C);
  RET(b_+6); return;
}

void s_func_7a44_hook(GB *gb) {
  BASE(func_7a44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); TAIL_S(func_7a70); }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); A = 0x36;
  CALL_C(b_+18, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+22, b_+24); TAIL_S(func_7a63); }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_add(gb, 0x14);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  RET(b_+30); return;
}

void s_func_7a63_hook(GB *gb) {
  BASE(func_7a63);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x0b);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x50);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x4c);
  RET(b_+12); return;
}

void s_func_7a70_hook(GB *gb) {
  BASE(func_7a70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_cp(gb, 0xa0);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_7a7b; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x3c);
L_7a7b:
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x40);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 0x96);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+32); alu_cp(gb, 0x03);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+33, b_+35); TAIL_S(func_7aa1); }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x3c);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x0d);
  CALL_C(b_+43, s_enemySetAnimation, SYM(enemySetAnimation), b_+46);
  CYC(b_+46, b_+49);
  TAIL(objectSetVisible81);
}

void s_func_7aa1_hook(GB *gb) {
  BASE(func_7aa1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x03);
  CYC(b_+2, b_+4); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0e);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xb4);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0xa9);
  CYC(b_+14, b_+16); A = 0x03;
  CYC(b_+16, b_+19);
  TAIL(enemySetAnimation);
}

void s_func_7ab4_hook(GB *gb) {
  BASE(func_7ab4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR37;
  CALL_C(b_+3, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x04);
  CYC(b_+9, b_+11); alu_cp(gb, 0x09);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8F);
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  RET(b_+19); return;
}

void s_func_7ac8_hook(GB *gb) {
  BASE(func_7ac8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_BC(pop_effect(gb));
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+13); return;
}

void s_func_7ad6_hook(GB *gb) {
  BASE(func_7ad6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CALL_C(b_+14, s_func_7af2_hook, SYM(func_7af2), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); alu_add(gb, B);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_add(gb, C);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  RET(b_+27); return;
}

void s_func_7af2_hook(GB *gb) {
  BASE(func_7af2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_sub(gb, 0x02);
  CYC(b_+2, b_+3); E = A;
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); alu_add(gb, E);
  CYC(b_+5, b_+6); alu_add(gb, A);
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+10); SET_HL(SYM(table_7afe));
  CYC(b_+10, b_+11); manhandla_add_a_to_hl(gb, b_+11);
  RET(b_+11); return;
}

void s_func_7b1c_hook(GB *gb) {
  BASE(func_7b1c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_sub(gb, 0x03);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+13); alu_sub(gb, B);
  CYC(b_+13, b_+15); alu_cp(gb, 0xf8);
  if (!(F & FC)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x62);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x78);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CALL_C(b_+32, s_enemySetAnimation, SYM(enemySetAnimation), b_+35);
  CYC(b_+35, b_+36); alu_scf(gb);
  RET(b_+36); return;
}

