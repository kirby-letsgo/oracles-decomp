#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/brotherGoriyas.s.
// ENEMY_BROTHER_GORIYAS

static uint16_t brotherGoriyas_jump_table(GB *gb) {
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

// enemyCode70@subid0@func_46a9
static void brotherGoriyas_subid0_func_46a9(GB *gb) {
  BASE(enemyCode70);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+263, s_ecom_setRandomCardinalAngle, SYM(ecom_setRandomCardinalAngle_b0d), b_+266);
  CYC(b_+266, b_+269);
  TAIL(ecom_updateAnimationFromAngle_b0d);
}

// ENEMY_BROTHER_GORIYAS
void s_enemyCode70_hook(GB *gb) {
  BASE(enemyCode70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); goto dead; }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); TAIL(ecom_updateKnockback_b0d); }
  CYC(b_+9, b_+12);
normalStatus:
  CALL_C(b_+12, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+15);
  if (!(F & FC)) { CYCT(b_+15, b_+17); goto L_45c4; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (brotherGoriyas_jump_table(gb));
    if (jt_ == b_+40) goto state0;
    if (jt_ == b_+106) goto stateStub;
    HANDOFF(HL);
  } while (0);
L_45c4:
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+36); push_effect(gb, b_+36);
  do { uint16_t jt_ = (brotherGoriyas_jump_table(gb));
    if (jt_ == b_+107) goto subid0;
    if (jt_ == b_+412) goto subid1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+40, b_+42); E = 0x97;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto func_45f0; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); B = 0x70;
  CALL_C(b_+48, s_ecom_spawnEnemyWithSubid01, SYM(ecom_spawnEnemyWithSubid01_b0d), b_+51);
  if (!(F & FZ)) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+54, b_+55); E = L;
  CYC(b_+55, b_+57); A = 0x80;
  CYC(b_+57, b_+58); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+60); E = alu_inc8(gb, E);
  CYC(b_+60, b_+61); mem_wr(gb, HL, D);
  CYC(b_+61, b_+62); A = H;
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CALL_C(b_+63, s_objectCopyPosition, SYM(objectCopyPosition), b_+66);
  CYC(b_+66, b_+67); A = D;
  CYC(b_+67, b_+68); alu_cp(gb, H);
  if (F & FC) { CYCT(b_+68, b_+70); goto func_45f0; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+72, b_+73); alu_xor(gb, A);
  CYC(b_+73, b_+74); mem_wr(gb, HL, A);
  CYC(b_+74, b_+75); E = L;
  CYC(b_+75, b_+76); A = alu_inc8(gb, A);
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  RET(b_+77); return;
func_45f0:
  CYC(b_+78, b_+80); A = 0x32;
  CALL_C(b_+80, s_ecom_setSpeedAndState8AndVisible, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+83);
  CYC(b_+83, b_+85); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+87, b_+89); goto func_4602; }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+93); alu_cpl(gb);
  CYC(b_+93, b_+94); A = alu_inc8(gb, A);
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  RET(b_+95); return;
func_4602:
  CYC(b_+96, b_+98); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+98, b_+99); A = alu_inc8(gb, A);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+102); B = 0x00;
  CYC(b_+102, b_+103); A = alu_dec8(gb, A);
  CYC(b_+103, b_+106);
  TAIL(enemyBoss_initializeRoom_b0f);
stateStub:
  RET(b_+106); return;
subid0:
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+110); alu_sub(gb, 0x08);
  CYC(b_+110, b_+111); push_effect(gb, b_+111);
  do { uint16_t jt_ = (brotherGoriyas_jump_table(gb));
    if (jt_ == b_+119) goto subid0_state8;
    if (jt_ == b_+233) goto subid0_state9;
    if (jt_ == b_+326) goto subid0_stateA;
    if (jt_ == b_+385) goto subid0_stateB;
    HANDOFF(HL);
  } while (0);
subid0_state8:
  CYC(b_+119, b_+120); E = alu_inc8(gb, E);
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+122); push_effect(gb, b_+122);
  do { uint16_t jt_ = (brotherGoriyas_jump_table(gb));
    if (jt_ == b_+128) goto subid0_state8_substate0;
    if (jt_ == b_+147) goto subid0_state8_substate1;
    if (jt_ == b_+158) goto subid0_state8_substate2;
    HANDOFF(HL);
  } while (0);
subid0_state8_substate0:
  CYC(b_+128, b_+131); A = mem_rd(gb, w1Link_yh);
  CYC(b_+131, b_+133); alu_cp(gb, 0x78);
  if (!(F & FC)) { RET_TAKEN(b_+133); return; }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+135); H = D;
  CYC(b_+135, b_+136); L = E;
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+137, b_+139); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+139, b_+141); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+141, b_+144); SET_BC(0x2f00);
  CYC(b_+144, b_+147);
  TAIL(showText);
subid0_state8_substate1:
  CYC(b_+147, b_+149); A = 0x02;
  CYC(b_+149, b_+150); mem_wr(gb, DE, A);
  CYC(b_+150, b_+152); A = 0x2d;
  CYC(b_+152, b_+155); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+155, s_playSound, SYM(playSound_b00), b_+158);
subid0_state8_substate2:
  CYC(b_+158, b_+161); A = mem_rd(gb, w1Link_xh);
  CYC(b_+161, b_+163); alu_sub(gb, 0x28);
  CYC(b_+163, b_+164); C = A;
subid0_func_4646:
  CYC(b_+164, b_+167); A = mem_rd(gb, w1Link_yh);
  CYC(b_+167, b_+168); B = A;
  CYC(b_+168, b_+170); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+170, b_+171); A = mem_rd(gb, DE);
  CYC(b_+171, b_+173); mem_wr(gb, hFF8F, A);
  CYC(b_+173, b_+175); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+178); mem_wr(gb, hFF8E, A);
  CYC(b_+178, b_+180); alu_sub(gb, 0x18);
  CYC(b_+180, b_+182); alu_cp(gb, 0xc0);
  if (!(F & FC)) { CYCT(b_+182, b_+184); goto L_466c; }
  CYC(b_+182, b_+184);
  CYC(b_+184, b_+186); A = mem_rd(gb, hFF8F);
  CYC(b_+186, b_+187); alu_sub(gb, B);
  CYC(b_+187, b_+189); alu_add(gb, 0x08);
  CYC(b_+189, b_+191); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+191, b_+193); goto subid0_func_4682; }
  CYC(b_+191, b_+193);
  CYC(b_+193, b_+195); A = mem_rd(gb, hFF8E);
  CYC(b_+195, b_+196); alu_sub(gb, C);
  CYC(b_+196, b_+198); alu_add(gb, 0x08);
  CYC(b_+198, b_+200); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+200, b_+202); goto subid0_func_4682; }
  CYC(b_+200, b_+202);
L_466c:
  CALL_C(b_+202, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+205);
  CYC(b_+205, b_+207); alu_and(gb, 0x30);
  CYC(b_+207, b_+209); alu_add(gb, 0x60);
  CYC(b_+209, b_+210); H = D;
  CYC(b_+210, b_+212); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+212, b_+213); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+213, b_+215); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+215, b_+217); mem_wr(gb, HL, 0x08);
  CYC(b_+217, b_+219); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+219, b_+220); mem_wr(gb, HL, A);
  CYC(b_+220, b_+221); L = alu_inc8(gb, L);
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0x3c);
  RET(b_+223); return;
subid0_func_4682:
  CALL_C(b_+224, s_ecom_moveTowardPosition, SYM(ecom_moveTowardPosition_b0d), b_+227);
  CALL_C(b_+227, s_ecom_updateAnimationFromAngle, SYM(ecom_updateAnimationFromAngle_b0d), b_+230);
subid0_animate:
  CYC(b_+230, b_+233);
  TAIL(enemyAnimate);
subid0_state9:
  CALL_C(b_+233, s_func_4809_hook, SYM(func_4809), b_+236);
  if (F & FZ) { CYCT(b_+236, b_+238); goto subid0_animate; }
  CYC(b_+236, b_+238);
  CYC(b_+238, b_+240); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+240, b_+242); A = 0x23;
  CYC(b_+242, b_+243); mem_wr(gb, DE, A);
  CALL_C(b_+243, s_func_4797_hook, SYM(func_4797), b_+246);
  if (F & FC) { CYCT(b_+246, b_+248); goto subid0_func_46af; }
  CYC(b_+246, b_+248);
subid0_func_469a:
  CALL_C(b_+248, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+251);
  if (!(F & FZ)) { CYCT(b_+251, b_+253); goto L_46a4; }
  CYC(b_+251, b_+253);
  CYC(b_+253, b_+255); mem_wr(gb, HL, 0x3c);
  CALL_L(b_+255, brotherGoriyas_subid0_func_46a9, b_+258);
L_46a4:
  CALL_C(b_+258, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+261);
  if (!(F & FZ)) { CYCT(b_+261, b_+263); goto subid0_animate; }
  CYC(b_+261, b_+263);
  CALL_C(b_+263, s_ecom_setRandomCardinalAngle, SYM(ecom_setRandomCardinalAngle_b0d), b_+266);
  CYC(b_+266, b_+269);
  TAIL(ecom_updateAnimationFromAngle_b0d);
subid0_func_46af:
  CYC(b_+269, b_+271); A = 0x09;
  CALL_C(b_+271, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+274);
  CYC(b_+274, b_+275); E = L;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+278); alu_add(gb, 0x10);
  CYC(b_+278, b_+280); alu_and(gb, 0x1f);
  CYC(b_+280, b_+281); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+281, b_+283); alu_and(gb, 0x1f);
  CYC(b_+283, b_+285); C = 0x28;
  if (F & FZ) { CYCT(b_+285, b_+287); goto L_46c9; }
  CYC(b_+285, b_+287);
  CYC(b_+287, b_+289); C = 0x32;
  CYC(b_+289, b_+291); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+291, b_+293); goto L_46c9; }
  CYC(b_+291, b_+293);
  CYC(b_+293, b_+295); C = 0x1e;
L_46c9:
  CYC(b_+295, b_+297); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+297, b_+298); A = C;
  CYC(b_+298, b_+299); mem_wr(gb, DE, A);
  CALL_C(b_+299, s_func_47c0_hook, SYM(func_47c0), b_+302);
  CALL_C(b_+302, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+305);
  CYC(b_+305, b_+306); B = A;
  CYC(b_+306, b_+308); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+308, b_+309); A = mem_rd(gb, DE);
  CYC(b_+309, b_+310); alu_add(gb, B);
  CYC(b_+310, b_+312); alu_and(gb, 0x1f);
  CYC(b_+312, b_+314); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+314, b_+315); mem_wr(gb, DE, A);
  CALL_C(b_+315, s_ecom_updateAnimationFromAngle, SYM(ecom_updateAnimationFromAngle_b0d), b_+318);
  CALL_C(b_+318, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+321);
  CALL_C(b_+321, s_func_47d0_hook, SYM(func_47d0), b_+324);
  CYC(b_+324, b_+326);
  goto subid0_animate2;
subid0_stateA:
  CYC(b_+326, b_+328); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+328, b_+329); A = mem_rd(gb, DE);
  CYC(b_+329, b_+330); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+330, b_+332); goto L_46f5; }
  CYC(b_+330, b_+332);
  CYC(b_+332, b_+334); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+334, b_+336); A = 0x09;
  CYC(b_+336, b_+337); mem_wr(gb, DE, A);
  CYC(b_+337, b_+339);
  goto subid0_animate2;
L_46f5:
  CYC(b_+339, b_+341); B = 0x38;
  CALL_C(b_+341, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+344);
  if (!(F & FZ)) { CYCT(b_+344, b_+346); goto subid0_animate2; }
  CYC(b_+344, b_+346);
  CYC(b_+346, b_+348); L = PART_BASE + OBJ_VAR30;
  CYC(b_+348, b_+350); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+350, b_+351); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+351, b_+353); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+353, b_+354); mem_wr(gb, HL, A);
  CYC(b_+354, b_+356); L = PART_BASE + OBJ_RELATED2;
  CYC(b_+356, b_+358); A = 0x80;
  CYC(b_+358, b_+359); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+359, b_+361); E = 0x97;
  CYC(b_+361, b_+362); A = mem_rd(gb, DE);
  CYC(b_+362, b_+363); mem_wr(gb, HL, A);
  CYC(b_+363, b_+364); H = A;
  CYC(b_+364, b_+366); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+366, b_+368); A = 0x0b;
  CYC(b_+368, b_+369); mem_wr(gb, HL, A);
  CYC(b_+369, b_+370); E = L;
  CYC(b_+370, b_+371); mem_wr(gb, DE, A);
  CYC(b_+371, b_+373); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+373, b_+374); alu_xor(gb, A);
  CYC(b_+374, b_+375); mem_wr(gb, DE, A);
  CYC(b_+375, b_+376); L = E;
  CYC(b_+376, b_+377); A = alu_inc8(gb, A);
  CYC(b_+377, b_+378); mem_wr(gb, HL, A);
  CYC(b_+378, b_+380); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+380, b_+382); mem_wr(gb, HL, 0x01);
subid0_animate2:
  CYC(b_+382, b_+385);
  TAIL(enemyAnimate);
subid0_stateB:
  CYC(b_+385, b_+387); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+387, b_+388); A = mem_rd(gb, DE);
  CYC(b_+388, b_+389); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+389, b_+391); goto L_472f; }
  CYC(b_+389, b_+391);
  CYC(b_+391, b_+392); A = alu_dec8(gb, A);
  if (F & FZ) CALL_C_CC(b_+392, s_func_47a5_hook, SYM(func_47a5), b_+395);
  else CYC(b_+392, b_+395);
  CYC(b_+395, b_+397);
  goto subid0_animate2;
L_472f:
  CYC(b_+397, b_+399); A = 0x19;
  CALL_C(b_+399, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+402);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+403, b_+405); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+405, b_+407); A = 0x09;
  CYC(b_+407, b_+408); mem_wr(gb, HL, A);
  CYC(b_+408, b_+409); E = L;
  CYC(b_+409, b_+410); mem_wr(gb, DE, A);
  CYC(b_+410, b_+412);
  goto subid0_animate2;
subid1:
  CYC(b_+412, b_+413); A = mem_rd(gb, DE);
  CYC(b_+413, b_+415); alu_sub(gb, 0x08);
  CYC(b_+415, b_+416); push_effect(gb, b_+416);
  do { uint16_t jt_ = (brotherGoriyas_jump_table(gb));
    if (jt_ == b_+424) goto subid1_state8;
    if (jt_ == b_+452) goto subid1_state9;
    if (jt_ == b_+326) goto subid0_stateA;
    if (jt_ == b_+385) goto subid0_stateB;
    HANDOFF(HL);
  } while (0);
subid1_state8:
  CYC(b_+424, b_+425); E = alu_inc8(gb, E);
  CYC(b_+425, b_+426); A = mem_rd(gb, DE);
  CYC(b_+426, b_+427); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+427, b_+429); goto L_4758; }
  CYC(b_+427, b_+429);
  CYC(b_+429, b_+432); A = mem_rd(gb, w1Link_xh);
  CYC(b_+432, b_+434); alu_add(gb, 0x28);
  CYC(b_+434, b_+435); C = A;
  CYC(b_+435, b_+438);
  goto subid0_func_4646;
L_4758:
  CYC(b_+438, b_+441); A = mem_rd(gb, w1Link_yh);
  CYC(b_+441, b_+443); alu_cp(gb, 0x78);
  if (!(F & FC)) { RET_TAKEN(b_+443); return; }
  CYC(b_+443, b_+444);
  CYC(b_+444, b_+445); H = D;
  CYC(b_+445, b_+446); L = E;
  CYC(b_+446, b_+447); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+447, b_+449); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+449, b_+451); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+451); return;
subid1_state9:
  CALL_C(b_+452, s_func_4809_hook, SYM(func_4809), b_+455);
  if (F & FZ) { CYCT(b_+455, b_+458); TAIL(enemyAnimate); }
  CYC(b_+455, b_+458);
  CYC(b_+458, b_+460); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+460, b_+462); A = 0x28;
  CYC(b_+462, b_+463); mem_wr(gb, DE, A);
  CALL_C(b_+463, s_func_4797_hook, SYM(func_4797), b_+466);
  if (F & FC) { CYCT(b_+466, b_+469); goto subid0_func_46af; }
  CYC(b_+466, b_+469);
  CYC(b_+469, b_+472);
  goto subid0_func_469a;
dead:
  CYC(b_+472, b_+474); E = 0x97;
  CYC(b_+474, b_+475); A = mem_rd(gb, DE);
  CYC(b_+475, b_+476); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+476, b_+478); goto L_4787; }
  CYC(b_+476, b_+478);
  CALL_C(b_+478, s_ecom_killRelatedObj1, SYM(ecom_killRelatedObj1_b0d), b_+481);
  CYC(b_+481, b_+483); E = 0x97;
  CYC(b_+483, b_+484); alu_xor(gb, A);
  CYC(b_+484, b_+485); mem_wr(gb, DE, A);
L_4787:
  CYC(b_+485, b_+487); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+487, b_+488); A = mem_rd(gb, DE);
  CYC(b_+488, b_+490); alu_sub(gb, 0x02);
  if (F & FC) { CYCT(b_+490, b_+492); goto L_4794; }
  CYC(b_+490, b_+492);
  CYC(b_+492, b_+493); H = A;
  CYC(b_+493, b_+495); L = 0xd7;
  CYC(b_+495, b_+496); alu_xor(gb, A);
  CYC(b_+496, b_+497); mem_wr(gb, HL, A);
  CYC(b_+497, b_+498); mem_wr(gb, DE, A);
L_4794:
  CYC(b_+498, b_+501);
  TAIL(enemyBoss_dead_b0f);
}

void s_func_4797_hook(GB *gb) {
  BASE(func_4797);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+2, b_+4); alu_sub(gb, 0x40);
  CYC(b_+4, b_+6); alu_cp(gb, 0x30);
  if (!(F & FC)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+9, b_+11); alu_sub(gb, 0x40);
  CYC(b_+11, b_+13); alu_cp(gb, 0x70);
  RET(b_+13); return;
}

void s_func_47a5_hook(GB *gb) {
  BASE(func_47a5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x19;
  CALL_C(b_+8, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+14); L = PART_BASE + OBJ_YH;
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  CYC(b_+15, b_+17); L = PART_BASE + OBJ_XH;
  CYC(b_+17, b_+18); C = mem_rd(gb, HL);
  CALL_C(b_+18, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+27);
  TAIL(ecom_updateAnimationFromAngle_b0d);
}

void s_func_47c0_hook(GB *gb) {
  BASE(func_47c0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x58;
  CYC(b_+2, b_+3); C = B;
  CYC(b_+3, b_+5); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+5, b_+7); alu_cp(gb, 0x60);
  if (F & FC) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); C = 0x78;
  CYC(b_+10, b_+12); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); C = 0x98;
  RET(b_+15); return;
}

void s_func_47d0_hook(GB *gb) {
  BASE(func_47d0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+7, b_+9); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+9, b_+10); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto L_47de; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); alu_cpl(gb);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
L_47de:
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+17, b_+19); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+19, b_+20); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto L_47e8; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); alu_cpl(gb);
  CYC(b_+23, b_+24); A = alu_inc8(gb, A);
L_47e8:
  CYC(b_+24, b_+25); alu_add(gb, B);
  CYC(b_+25, b_+27); alu_cp(gb, 0x30);
  if (F & FC) { CYCT(b_+27, b_+29); goto L_47fa; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); alu_cp(gb, 0x60);
  if (F & FC) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x0a);
  CYC(b_+36, b_+37); A = C;
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42);
  goto L_4806;
L_47fa:
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x14);
  CYC(b_+46, b_+47); A = C;
  CYC(b_+47, b_+49); alu_add(gb, 0x10);
  CYC(b_+49, b_+51); alu_and(gb, 0x1f);
  CYC(b_+51, b_+53); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
L_4806:
  CYC(b_+54, b_+57);
  TAIL(ecom_applyVelocityForSideviewEnemyNoHoles_b0d);
}

void s_func_4809_hook(GB *gb) {
  BASE(func_4809);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_4812; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
L_4812:
  CYC(b_+9, b_+12); SET_BC(0x0130);
  CALL_C(b_+12, s_ecom_randomBitwiseAndBCE, SYM(ecom_randomBitwiseAndBCE_b0d), b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); A = 0x20;
  CYC(b_+19, b_+20); alu_add(gb, C);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+29, b_+31); L = 0x97;
  CYC(b_+31, b_+32); H = mem_rd(gb, HL);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+34, b_+35); mem_wr(gb, HL, C);
  CALL_C(b_+35, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+38);
  CALL_C(b_+38, s_ecom_updateAnimationFromAngle, SYM(ecom_updateAnimationFromAngle_b0d), b_+41);
  CYC(b_+41, b_+42); alu_xor(gb, A);
  RET(b_+42); return;
}

