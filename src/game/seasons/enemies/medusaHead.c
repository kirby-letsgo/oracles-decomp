#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/medusaHead.s.
// ENEMY_MEDUSA_HEAD

static uint16_t medusaHead_jump_table(GB *gb) {
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

static void medusaHead_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_MEDUSA_HEAD
void s_enemyCode7f_hook(GB *gb) {
  BASE(enemyCode7f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { CYCT(b_+4, b_+6); goto L_7b4d; }
  CYC(b_+4, b_+6);
  if (F & FZ) { CYCT(b_+6, b_+9); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto justHit; }
  CYC(b_+10, b_+12);
L_7b4d:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+21, b_+23); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+23, b_+25); goto L_7b61; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x01);
  CALL_C(b_+29, s_enemyAnimate, SYM(enemyAnimate), b_+32);
L_7b61:
  CYC(b_+32, b_+35);
  TAIL(ecom_updateKnockback_b0d);
justHit:
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); A = A & 0x7f;
  CYC(b_+41, b_+43); alu_sub(gb, 0x04);
  if (F & FC) { CYCT(b_+43, b_+45); goto normalStatus; }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); alu_sub(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+47, b_+49); goto L_7b7b; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x01);
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x00);
  RET(b_+57); return;
L_7b7b:
  CYC(b_+58, b_+60); alu_sub(gb, 0x13);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto L_7b93; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x0d);
  CYC(b_+66, b_+67); L = alu_inc8(gb, L);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x2d);
  CALL_C(b_+69, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+72);
  CYC(b_+72, b_+74); alu_xor(gb, 0x10);
  CYC(b_+74, b_+76); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CYC(b_+77, b_+79); A = 0x4e;
  CYC(b_+79, b_+82);
  TAIL(playSound_b00);
L_7b93:
  CYC(b_+82, b_+84); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+86); return; }
  CYC(b_+86, b_+87);
normalStatus:
  CYC(b_+87, b_+89); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+89, b_+90); A = mem_rd(gb, DE);
  CYC(b_+90, b_+91); push_effect(gb, b_+91);
  do { uint16_t jt_ = (medusaHead_jump_table(gb));
    if (jt_ == b_+117) goto state0;
    if (jt_ == b_+139) goto state1;
    if (jt_ == b_+207) goto stateStub;
    if (jt_ == b_+208) goto state8;
    if (jt_ == b_+290) goto state9;
    if (jt_ == b_+313) goto stateA;
    if (jt_ == b_+454) goto stateB;
    if (jt_ == b_+655) goto stateC;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+117, b_+119); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  CYC(b_+120, b_+121); alu_or(gb, A);
  CYC(b_+121, b_+123); A = 0x3c;
  if (!(F & FZ)) { CYCT(b_+123, b_+126); TAIL(ecom_setSpeedAndState8_b0d); }
  CYC(b_+123, b_+126);
  CYC(b_+126, b_+128); A = 0x7f;
  CYC(b_+128, b_+130); B = 0x88;
  CALL_C(b_+130, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+133);
  CYC(b_+133, b_+135); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+135, b_+137); A = 0x01;
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  RET(b_+138); return;
state1:
  CYC(b_+139, b_+142); A = mem_rd(gb, wcc93);
  CYC(b_+142, b_+143); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+143); return; }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+146); B = 0x04;
  CALL_C(b_+146, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+149);
  if (!(F & FZ)) { RET_TAKEN(b_+149); return; }
  CYC(b_+149, b_+150);
  CYC(b_+150, b_+153); SET_BC(0x7f04);
L_7bda:
  CALL_C(b_+153, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+156);
  CYC(b_+156, b_+157); mem_wr(gb, HL, C);
  CYC(b_+157, b_+158); A = C;
  CYC(b_+158, b_+159); A = alu_dec8(gb, A);
  CYC(b_+159, b_+160); E = A;
  CYC(b_+160, b_+161); alu_add(gb, A);
  CYC(b_+161, b_+162); alu_add(gb, E);
  CYC(b_+162, b_+165); SET_DE(b_+195 /* @table_7c04 */);
  CALL_C(b_+165, s_addAToDe, SYM(addAToDe), b_+168);
  CYC(b_+168, b_+170); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+170, b_+171); A = mem_rd(gb, DE);
  CYC(b_+171, b_+172); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+172, b_+173); SET_DE(DE + 1);
  CYC(b_+173, b_+174); L = alu_inc8(gb, L);
  CYC(b_+174, b_+175); A = mem_rd(gb, DE);
  CYC(b_+175, b_+176); mem_wr(gb, HL, A);
  CYC(b_+176, b_+177); SET_DE(DE + 1);
  CYC(b_+177, b_+179); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  CYC(b_+181, b_+182); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+182, b_+184); goto L_7bda; }
  CYC(b_+182, b_+184);
  CYC(b_+184, b_+186); A = mem_rd(gb, hActiveObject);
  CYC(b_+186, b_+187); D = A;
  CYC(b_+187, b_+189); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+189, b_+190); E = L;
  CYC(b_+190, b_+191); A = mem_rd(gb, DE);
  CYC(b_+191, b_+192); mem_wr(gb, HL, A);
  CYC(b_+192, b_+195);
  TAIL(enemyDelete);

stateStub:
  RET(b_+207); return;
state8:
  CYC(b_+208, b_+209); E = alu_inc8(gb, E);
  CYC(b_+209, b_+210); A = mem_rd(gb, DE);
  CYC(b_+210, b_+211); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+211, b_+213); goto L_7c29; }
  CYC(b_+211, b_+213);
  CYC(b_+213, b_+214); H = D;
  CYC(b_+214, b_+215); L = E;
  CYC(b_+215, b_+216); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+216, b_+217); L = alu_inc8(gb, L);
  CYC(b_+217, b_+219); mem_wr(gb, HL, 0x70);
  CYC(b_+219, b_+221); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0xfe);
  CYC(b_+223, b_+225); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+225, b_+226); A = mem_rd(gb, HL);
  CYC(b_+226, b_+227); A = alu_dec8(gb, A);
  CYC(b_+227, b_+229); A = 0x8d;
  if (F & FZ) CALL_C_CC(b_+229, s_playSound, SYM(playSound_b00), b_+232);
  else CYC(b_+229, b_+232);
L_7c29:
  CALL_C(b_+232, s_ecom_flickerVisibility, SYM(ecom_flickerVisibility_b0d), b_+235);
  CYC(b_+235, b_+238); A = mem_rd(gb, wFrameCounter);
  CYC(b_+238, b_+239); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+239); return; }
  CYC(b_+239, b_+240);
  CALL_C(b_+240, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+243);
  if (F & FZ) { CYCT(b_+243, b_+245); goto L_7c47; }
  CYC(b_+243, b_+245);
  CYC(b_+245, b_+246); A = mem_rd(gb, HL);
  CYC(b_+246, b_+248); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+248, b_+251); SET_BC(0x5878);
  CALL_C(b_+251, s_objectSetPositionInCircleArc, SYM(objectSetPositionInCircleArc), b_+254);
  CYC(b_+254, b_+256); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+256, b_+257); A = mem_rd(gb, DE);
  CYC(b_+257, b_+258); A = alu_inc8(gb, A);
  CYC(b_+258, b_+260); alu_and(gb, 0x1f);
  CYC(b_+260, b_+261); mem_wr(gb, DE, A);
  RET(b_+261); return;
L_7c47:
  CYC(b_+262, b_+264); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+264, b_+265); A = mem_rd(gb, DE);
  CYC(b_+265, b_+266); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+266, b_+269); TAIL(enemyDelete); }
  CYC(b_+266, b_+269);
  CYC(b_+269, b_+271); mem_wr(gb, HL, 0x1e);
  CYC(b_+271, b_+273); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+273, b_+274); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+274, s_objectSetVisible83, SYM(objectSetVisible83), b_+277);
  CYC(b_+277, b_+279); A = 0x73;
  CALL_C(b_+279, s_playSound, SYM(playSound_b00), b_+282);
  CYC(b_+282, b_+284); A = 0x2e;
  CYC(b_+284, b_+287); mem_wr(gb, wActiveMusic, A);
  CYC(b_+287, b_+290);
  TAIL(playSound_b00);
state9:
  CALL_C(b_+290, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+293);
  if (!(F & FZ)) { RET_TAKEN(b_+293); return; }
  CYC(b_+293, b_+294);
  CYC(b_+294, b_+295); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+295, b_+298); SET_BC(0x020b);
  CALL_C(b_+298, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+301);
  if (!(F & FZ)) { RET_TAKEN(b_+301); return; }
  CYC(b_+301, b_+302);
  CYC(b_+302, b_+303); H = D;
  CYC(b_+303, b_+304); L = E;
  CYC(b_+304, b_+305); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+305, b_+306); L = alu_inc8(gb, L);
  CYC(b_+306, b_+308); mem_wr(gb, HL, 0x00);
  CYC(b_+308, b_+310); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+310, b_+312); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+312); return;
stateA:
  CYC(b_+313, b_+314); E = alu_inc8(gb, E);
  CYC(b_+314, b_+315); A = mem_rd(gb, DE);
  CYC(b_+315, b_+316); push_effect(gb, b_+316);
  do { uint16_t jt_ = (medusaHead_jump_table(gb));
    if (jt_ == b_+324) goto stateA_substate0;
    if (jt_ == b_+363) goto stateA_substate1;
    if (jt_ == b_+386) goto stateA_substate2;
    if (jt_ == b_+440) goto stateA_substate3;
    HANDOFF(HL);
  } while (0);
stateA_substate0:
  CYC(b_+324, b_+325); H = D;
  CYC(b_+325, b_+326); L = E;
  CYC(b_+326, b_+327); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+327, b_+328); L = alu_inc8(gb, L);
  CYC(b_+328, b_+330); mem_wr(gb, HL, 0xb4);
  CYC(b_+330, b_+331); L = alu_inc8(gb, L);
  CYC(b_+331, b_+333); mem_wr(gb, HL, 0x5a);
  CYC(b_+333, b_+335); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+335, b_+337); mem_wr(gb, HL, 0x46);
  CYC(b_+337, b_+339); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+339, b_+341); mem_wr(gb, HL, 0x08);
  CYC(b_+341, b_+343); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+343, b_+345); mem_wr(gb, HL, 0x00);
  CYC(b_+345, b_+347); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+347, b_+348); A = mem_rd(gb, HL);
  CYC(b_+348, b_+350); mem_wr(gb, HL, 0x00);
  CYC(b_+350, b_+351); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+351, b_+353); goto stateA_animate; }
  CYC(b_+351, b_+353);
  CYC(b_+353, b_+355); B = 0x46;
  CALL_C(b_+355, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+358);
  CYC(b_+358, b_+360); A = 0x01;
  CYC(b_+360, b_+363);
  TAIL(enemySetAnimation);
stateA_substate1:
  CYC(b_+363, b_+365); C = 0x40;
  CALL_C(b_+365, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+368);
  if (F & FC) { CYCT(b_+368, b_+370); goto L_7cbb; }
  CYC(b_+368, b_+370);
  CALL_C(b_+370, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+373);
  CALL_C(b_+373, s_objectApplySpeed, SYM(objectApplySpeed), b_+376);
  CYC(b_+376, b_+378);
  goto stateA_animate;
L_7cbb:
  CYC(b_+378, b_+380); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+380, b_+382); A = 0x02;
  CYC(b_+382, b_+383); mem_wr(gb, DE, A);
stateA_animate:
  CYC(b_+383, b_+386);
  TAIL(enemyAnimate);
stateA_substate2:
  CALL_C(b_+386, s_func_7e84_hook, SYM(func_7e84), b_+389);
  if (!(F & FZ)) { CYCT(b_+389, b_+391); goto L_7ccc; }
  CYC(b_+389, b_+391);
  CYC(b_+391, b_+392); L = E;
  CYC(b_+392, b_+393); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+393, b_+395);
  goto stateA_animate;
L_7ccc:
  CALL_C(b_+395, s_func_7ece_hook, SYM(func_7ece), b_+398);
  if (!(F & FZ)) { CYCT(b_+398, b_+400); goto stateA_animate; }
  CYC(b_+398, b_+400);
  CALL_C(b_+400, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+403);
  if (F & FZ) CALL_C_CC(b_+403, s_func_7eb5_hook, SYM(func_7eb5), b_+406);
  else CYC(b_+403, b_+406);
  CALL_C(b_+406, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+409);
  CYC(b_+409, b_+410); C = A;
  CYC(b_+410, b_+412); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+412, b_+413); A = mem_rd(gb, DE);
  CYC(b_+413, b_+414); alu_add(gb, C);
  CYC(b_+414, b_+416); alu_and(gb, 0x1f);
  CYC(b_+416, b_+418); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+418, b_+419); mem_wr(gb, DE, A);
  CALL_C(b_+419, s_func_7e8d_hook, SYM(func_7e8d), b_+422);
  CALL_C(b_+422, s_ecom_applyVelocityForTopDownEnemyNoHoles, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+425);
  if (!(F & FZ)) { CYCT(b_+425, b_+427); goto stateA_animate; }
  CYC(b_+425, b_+427);
  CYC(b_+427, b_+429); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+429, b_+430); A = mem_rd(gb, DE);
  CYC(b_+430, b_+431); alu_cpl(gb);
  CYC(b_+431, b_+432); A = alu_inc8(gb, A);
  CYC(b_+432, b_+433); mem_wr(gb, DE, A);
  CYC(b_+433, b_+435); A = 0xc9;
  CALL_C(b_+435, s_playSound, SYM(playSound_b00), b_+438);
  CYC(b_+438, b_+440);
  goto stateA_animate;
stateA_substate3:
  CYC(b_+440, b_+441); H = D;
  CYC(b_+441, b_+442); L = E;
  CYC(b_+442, b_+444); mem_wr(gb, HL, 0x00);
  CYC(b_+444, b_+445); L = alu_dec8(gb, L);
  CYC(b_+445, b_+446); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+446, s_getRandomNumber, SYM(getRandomNumber), b_+449);
  CYC(b_+449, b_+451); alu_cp(gb, 0x60);
  if (!(F & FC)) { RET_TAKEN(b_+451); return; }
  CYC(b_+451, b_+452);
  CYC(b_+452, b_+453); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+453); return;
stateB:
  CYC(b_+454, b_+455); E = alu_inc8(gb, E);
  CYC(b_+455, b_+456); A = mem_rd(gb, DE);
  CYC(b_+456, b_+457); push_effect(gb, b_+457);
  do { uint16_t jt_ = (medusaHead_jump_table(gb));
    if (jt_ == b_+473) goto stateB_substate0;
    if (jt_ == b_+483) goto stateB_substate1;
    if (jt_ == b_+524) goto stateB_substate2;
    if (jt_ == b_+537) goto stateB_substate3;
    if (jt_ == b_+554) goto stateB_substate4;
    if (jt_ == b_+573) goto stateB_substate5;
    if (jt_ == b_+593) goto stateB_substate6;
    if (jt_ == b_+641) goto stateB_substate7;
    HANDOFF(HL);
  } while (0);
stateB_substate0:
  CYC(b_+473, b_+474); H = D;
  CYC(b_+474, b_+475); L = E;
  CYC(b_+475, b_+476); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+476, b_+477); L = alu_inc8(gb, L);
  CYC(b_+477, b_+479); mem_wr(gb, HL, 0x1e);
  CYC(b_+479, b_+481); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+481, b_+483); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
stateB_substate1:
  CALL_C(b_+483, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+486);
  if (!(F & FZ)) { CYCT(b_+486, b_+489); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+486, b_+489);
  CYC(b_+489, b_+491); mem_wr(gb, HL, 0x14);
  CYC(b_+491, b_+492); L = E;
  CYC(b_+492, b_+493); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+493, b_+495); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+495, b_+497); mem_wr(gb, HL, 0x00);
  CYC(b_+497, b_+499); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+499, b_+500); A = mem_rd(gb, HL);
  CYC(b_+500, b_+502); alu_cp(gb, 0x78);
  CYC(b_+502, b_+504); C = 0x20;
  CYC(b_+504, b_+506); A = 0xd4;
  if (F & FC) { CYCT(b_+506, b_+508); goto L_7d40; }
  CYC(b_+506, b_+508);
  CYC(b_+508, b_+509); C = A;
  CYC(b_+509, b_+511); A = 0x1c;
L_7d40:
  CYC(b_+511, b_+512); mem_wr(gb, HL, C);
  CYC(b_+512, b_+514); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+514, b_+515); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+515, b_+517); mem_wr(gb, HL, 0x20);
  CYC(b_+517, b_+519); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+519, b_+521); mem_wr(gb, HL, 0x20);
  CYC(b_+521, b_+524);
  TAIL(objectSetInvisible);
stateB_substate2:
  CALL_C(b_+524, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+527);
  if (!(F & FZ)) { RET_TAKEN(b_+527); return; }
  CYC(b_+527, b_+528);
  CYC(b_+528, b_+530); mem_wr(gb, HL, 0x1e);
  CYC(b_+530, b_+531); L = E;
  CYC(b_+531, b_+532); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+532, b_+534); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+534, b_+536); mem_wr(gb, HL, 0xfe);
  RET(b_+536); return;
stateB_substate3:
  CALL_C(b_+537, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+540);
  if (!(F & FZ)) { CYCT(b_+540, b_+543); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+540, b_+543);
  CYC(b_+543, b_+545); mem_wr(gb, HL, 0x0f);
  CYC(b_+545, b_+546); L = E;
  CYC(b_+546, b_+547); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+547, b_+549); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+549, b_+551); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+551, b_+554);
  TAIL(objectSetVisible83);
stateB_substate4:
  CALL_C(b_+554, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+557);
  if (!(F & FZ)) { CYCT(b_+557, b_+559); goto stateB_animate; }
  CYC(b_+557, b_+559);
  CYC(b_+559, b_+561); mem_wr(gb, HL, 0x05);
  CYC(b_+561, b_+562); L = E;
  CYC(b_+562, b_+563); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+563, b_+565); B = 0x45;
  CALL_C(b_+565, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+568);
  CYC(b_+568, b_+570); A = 0x02;
  CYC(b_+570, b_+573);
  TAIL(enemySetAnimation);
stateB_substate5:
  CALL_C(b_+573, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+576);
  if (!(F & FZ)) { CYCT(b_+576, b_+578); goto stateB_playSoundAndAnimate; }
  CYC(b_+576, b_+578);
  CYC(b_+578, b_+579); L = E;
  CYC(b_+579, b_+580); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+580, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+583);
  CYC(b_+583, b_+585); alu_and(gb, 0x03);
  CYC(b_+585, b_+588); SET_HL(SYM(table_7edf));
  CYC(b_+588, b_+589); medusaHead_add_a_to_hl(gb, b_+589);
  CYC(b_+589, b_+591); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+591, b_+592); A = mem_rd(gb, HL);
  CYC(b_+592, b_+593); mem_wr(gb, DE, A);
stateB_substate6:
  CYC(b_+593, b_+594); H = D;
  CYC(b_+594, b_+596); L = ENEMY_BASE + OBJ_VAR31;
  CALL_C(b_+596, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+599);
  CYC(b_+599, b_+600); alu_sub(gb, C);
  CYC(b_+600, b_+602); alu_add(gb, 0x02);
  CYC(b_+602, b_+604); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+604, b_+606); goto L_7db2; }
  CYC(b_+604, b_+606);
  CYC(b_+606, b_+608); A = mem_rd(gb, hFF8F);
  CYC(b_+608, b_+609); alu_sub(gb, B);
  CYC(b_+609, b_+611); alu_add(gb, 0x02);
  CYC(b_+611, b_+613); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+613, b_+615); goto L_7db2; }
  CYC(b_+613, b_+615);
  CYC(b_+615, b_+617); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+617, b_+618); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+618, b_+619); L = alu_inc8(gb, L);
  CYC(b_+619, b_+621); mem_wr(gb, HL, 0x08);
  CYC(b_+621, b_+622); alu_xor(gb, A);
  CYC(b_+622, b_+625);
  TAIL(enemySetAnimation);
L_7db2:
  CALL_C(b_+625, s_ecom_moveTowardPosition, SYM(ecom_moveTowardPosition_b0d), b_+628);
stateB_playSoundAndAnimate:
  CYC(b_+628, b_+631); A = mem_rd(gb, wFrameCounter);
  CYC(b_+631, b_+633); alu_and(gb, 0x07);
  CYC(b_+633, b_+635); A = 0xa8;
  if (F & FZ) CALL_C_CC(b_+635, s_playSound, SYM(playSound_b00), b_+638);
  else CYC(b_+635, b_+638);
stateB_animate:
  CYC(b_+638, b_+641);
  TAIL(enemyAnimate);
stateB_substate7:
  CALL_C(b_+641, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+644);
  if (!(F & FZ)) { CYCT(b_+644, b_+646); goto stateB_playSoundAndAnimate; }
  CYC(b_+644, b_+646);
  CYC(b_+646, b_+647); L = E;
  CYC(b_+647, b_+648); alu_xor(gb, A);
  CYC(b_+648, b_+649); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+649, b_+650); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+650, s_ecom_killRelatedObj2, SYM(ecom_killRelatedObj2_b0d), b_+653);
  CYC(b_+653, b_+655);
  goto stateB_animate;
stateC:
  CYC(b_+655, b_+656); E = alu_inc8(gb, E);
  CYC(b_+656, b_+657); A = mem_rd(gb, DE);
  CYC(b_+657, b_+658); push_effect(gb, b_+658);
  do { uint16_t jt_ = (medusaHead_jump_table(gb));
    if (jt_ == b_+676) goto stateC_substate0;
    if (jt_ == b_+688) goto stateC_substate1;
    if (jt_ == b_+705) goto stateC_substate2;
    if (jt_ == b_+725) goto stateC_substate3;
    if (jt_ == b_+745) goto stateC_substate4;
    if (jt_ == b_+759) goto stateC_substate5;
    if (jt_ == b_+775) goto stateC_substate6;
    if (jt_ == b_+807) goto stateC_substate7;
    if (jt_ == b_+821) goto stateC_substate8;
    HANDOFF(HL);
  } while (0);
stateC_substate0:
  CYC(b_+676, b_+677); H = D;
  CYC(b_+677, b_+678); L = E;
  CYC(b_+678, b_+679); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+679, b_+680); L = alu_inc8(gb, L);
  CYC(b_+680, b_+682); mem_wr(gb, HL, 0x1e);
  CYC(b_+682, b_+684); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+684, b_+686); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+686, b_+688);
  goto stateC_animate;
stateC_substate1:
  CALL_C(b_+688, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+691);
  if (!(F & FZ)) { CYCT(b_+691, b_+694); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+691, b_+694);
  CYC(b_+694, b_+696); mem_wr(gb, HL, 0x14);
  CYC(b_+696, b_+697); L = E;
  CYC(b_+697, b_+698); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+698, b_+700); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+700, b_+702); mem_wr(gb, HL, 0x00);
  CYC(b_+702, b_+705);
  TAIL(objectSetInvisible);
stateC_substate2:
  CALL_C(b_+705, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+708);
  if (!(F & FZ)) { RET_TAKEN(b_+708); return; }
  CYC(b_+708, b_+709);
  CYC(b_+709, b_+711); mem_wr(gb, HL, 0x1e);
  CYC(b_+711, b_+712); L = E;
  CYC(b_+712, b_+713); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+713, b_+715); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+715, b_+717); mem_wr(gb, HL, 0x58);
  CYC(b_+717, b_+719); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+719, b_+721); mem_wr(gb, HL, 0x78);
  CYC(b_+721, b_+723); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+723, b_+725); mem_wr(gb, HL, 0xfe);
stateC_substate3:
  CALL_C(b_+725, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+728);
  if (!(F & FZ)) { CYCT(b_+728, b_+731); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+728, b_+731);
  CYC(b_+731, b_+733); mem_wr(gb, HL, 0x14);
  CYC(b_+733, b_+734); L = E;
  CYC(b_+734, b_+735); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+735, b_+737); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+737, b_+739); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+739, s_objectSetVisible83, SYM(objectSetVisible83), b_+742);
stateC_animate:
  CYC(b_+742, b_+745);
  TAIL(enemyAnimate);
stateC_substate4:
  CALL_C(b_+745, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+748);
  if (!(F & FZ)) { CYCT(b_+748, b_+750); goto stateC_animate; }
  CYC(b_+748, b_+750);
  CYC(b_+750, b_+752); mem_wr(gb, HL, 0x14);
  CYC(b_+752, b_+753); L = E;
  CYC(b_+753, b_+754); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+754, b_+756); A = 0x04;
  CYC(b_+756, b_+759);
  TAIL(enemySetAnimation);
stateC_substate5:
  CALL_C(b_+759, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+762);
  if (!(F & FZ)) { CYCT(b_+762, b_+764); goto stateC_animate; }
  CYC(b_+762, b_+764);
  CYC(b_+764, b_+766); mem_wr(gb, HL, 0x20);
  CYC(b_+766, b_+767); L = E;
  CYC(b_+767, b_+768); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+768, b_+770); A = 0xd2;
  CALL_C(b_+770, s_playSound, SYM(playSound_b00), b_+773);
  CYC(b_+773, b_+775);
  goto stateC_animate;
stateC_substate6:
  CALL_C(b_+775, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+778);
  if (F & FZ) { CYCT(b_+778, b_+780); goto L_7e62; }
  CYC(b_+778, b_+780);
  CYC(b_+780, b_+781); A = mem_rd(gb, HL);
  CYC(b_+781, b_+782); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+782, b_+784); goto stateC_animate; }
  CYC(b_+782, b_+784);
  CYC(b_+784, b_+786); B = 0x44;
  CALL_C(b_+786, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+789);
  if (!(F & FZ)) { CYCT(b_+789, b_+791); goto stateC_animate; }
  CYC(b_+789, b_+791);
  CYC(b_+791, b_+793); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+793, b_+794); A = mem_rd(gb, DE);
  CYC(b_+794, b_+795); A = alu_dec8(gb, A);
  CYC(b_+795, b_+796); alu_rrca(gb);
  CYC(b_+796, b_+798); L = PART_BASE + OBJ_SUBID;
  CYC(b_+798, b_+799); mem_wr(gb, HL, A);
  CYC(b_+799, b_+801);
  goto stateC_animate;
L_7e62:
  CYC(b_+801, b_+803); mem_wr(gb, HL, 0x0c);
  CYC(b_+803, b_+804); L = E;
  CYC(b_+804, b_+805); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+805, b_+807);
  goto stateC_animate;
stateC_substate7:
  CALL_C(b_+807, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+810);
  if (!(F & FZ)) { CYCT(b_+810, b_+812); goto stateC_animate; }
  CYC(b_+810, b_+812);
  CYC(b_+812, b_+814); mem_wr(gb, HL, 0x0f);
  CYC(b_+814, b_+815); L = E;
  CYC(b_+815, b_+816); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+816, b_+818); A = 0x03;
  CYC(b_+818, b_+821);
  TAIL(enemySetAnimation);
stateC_substate8:
  CALL_C(b_+821, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+824);
  if (!(F & FZ)) { CYCT(b_+824, b_+827); goto stateC_animate; }
  CYC(b_+824, b_+827);
  CYC(b_+827, b_+828); L = E;
  CYC(b_+828, b_+829); alu_xor(gb, A);
  CYC(b_+829, b_+830); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+830, b_+832); mem_wr(gb, HL, 0x0a);
  CYC(b_+832, b_+835);
  TAIL(enemySetAnimation);
}

void s_func_7e84_hook(GB *gb) {
  BASE(func_7e84);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9);
  TAIL(ecom_decCounter2_b0d);
}

void s_func_7e8d_hook(GB *gb) {
  BASE(func_7e8d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+5); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+6, b_+8); goto L_7e97; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); alu_cpl(gb);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
L_7e97:
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+15); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+15, b_+16); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto L_7ea1; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_cpl(gb);
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);
L_7ea1:
  CYC(b_+20, b_+21); alu_add(gb, B);
  CYC(b_+21, b_+23); alu_and(gb, 0xf0);
  CYC(b_+23, b_+25); alu_cp(gb, 0x50);
  if (F & FZ) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); alu_cp(gb, 0x40);
  if (F & FZ) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto L_7eb0; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); A = C;
  CYC(b_+32, b_+34); alu_xor(gb, 0x10);
  CYC(b_+34, b_+35); C = A;
L_7eb0:
  CYC(b_+35, b_+37); B = 0x1e;
  CYC(b_+37, b_+40);
  TAIL(ecom_applyGivenVelocity_b0d);
}

void s_func_7eb5_hook(GB *gb) {
  BASE(func_7eb5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x3c);
  CYC(b_+2, b_+3); L = alu_inc8(gb, L);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+6); alu_cp(gb, 0x0f);
  if (F & FC) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, s_getRandomNumber, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x60);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x5a);
  CYC(b_+15, b_+17); B = 0x46;
  CALL_C(b_+17, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+20);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25);
  TAIL(enemySetAnimation);
}

void s_func_7ece_hook(GB *gb) {
  BASE(func_7ece);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+8, b_+10); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+16); return;
}

