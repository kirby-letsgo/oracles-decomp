#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/omuai.s.
// ENEMY_OMUAI

static uint16_t omuai_jump_table(GB *gb) {
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

static void omuai_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void omuai_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// enemyCode72@func_4b72
static void omuai_func_4b72(GB *gb) {
  BASE(enemyCode72);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+373, b_+375); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+375, b_+377); A = 0x80;
  CYC(b_+377, b_+378); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+378, b_+380); mem_wr(gb, HL, 0xfe);
  RET(b_+380); return;
}

// enemyCode72@func_4aaf
static void omuai_func_4aaf(GB *gb) {
  BASE(enemyCode72);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+178, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+181);
  CYC(b_+181, b_+183); alu_and(gb, 0x1f);
  CYC(b_+183, b_+185); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+185, b_+186); mem_wr(gb, DE, A);
  CYC(b_+186, b_+187); H = D;
  CYC(b_+187, b_+189); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+189, b_+191); mem_wr(gb, HL, 0x1e);
  CYC(b_+191, b_+193); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+193, b_+195); A = 0x00;
  CYC(b_+195, b_+196); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0xfe);
  RET(b_+198); return;
}

// ENEMY_OMUAI
void s_enemyCode72_hook(GB *gb) {
  BASE(enemyCode72);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto justHitOrKnockback; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto dead; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); SET_HL(w1ReservedInteraction0_var3f + 0x02);
L_4a0d:
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_cp(gb, 0x72);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_4a17; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = H;
  CYC(b_+22, b_+23); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); TAIL(enemyDie_withoutItemDrop); }
  CYC(b_+23, b_+26);
L_4a17:
  CYC(b_+26, b_+27); H = alu_inc8(gb, H);
  CYC(b_+27, b_+28); A = H;
  CYC(b_+28, b_+30); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+30, b_+32); goto L_4a0d; }
  CYC(b_+30, b_+32);
dead:
  CYC(b_+32, b_+35);
  TAIL(enemyBoss_dead_b0f);
justHitOrKnockback:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+40); A = A & 0x7f;
  CYC(b_+40, b_+42); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+42, b_+44); goto normalStatus; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+46, b_+48); A = 0x01;
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
normalStatus:
  CYC(b_+49, b_+51); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); push_effect(gb, b_+53);
  do { uint16_t jt_ = (omuai_jump_table(gb));
    if (jt_ == b_+89) goto state0;
    if (jt_ == b_+225) goto stateStub;
    if (jt_ == b_+99) goto state2;
    if (jt_ == b_+199) goto state4;
    if (jt_ == b_+226) goto state8;
    if (jt_ == b_+246) goto state9;
    if (jt_ == b_+288) goto stateA;
    if (jt_ == b_+314) goto stateB;
    if (jt_ == b_+353) goto stateC;
    if (jt_ == b_+381) goto stateD;
    if (jt_ == b_+414) goto stateE;
    if (jt_ == b_+465) goto stateF;
    if (jt_ == b_+532) goto stateG;
    if (jt_ == b_+558) goto stateH;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+89, b_+91); B = 0x00;
  CYC(b_+91, b_+93); A = 0x72;
  CALL_C(b_+93, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+96);
  CYC(b_+96, b_+99);
  TAIL(ecom_setSpeedAndState8_b0d);
state2:
  CYC(b_+99, b_+100); E = alu_inc8(gb, E);
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+102); push_effect(gb, b_+102);
  do { uint16_t jt_ = (omuai_jump_table(gb));
    if (jt_ == b_+110) goto state2_substate0;
    if (jt_ == b_+136) goto state2_substate1;
    if (jt_ == b_+156) goto state2_substate2;
    if (jt_ == b_+162) goto state2_substate3;
    HANDOFF(HL);
  } while (0);
state2_substate0:
  CYC(b_+110, b_+112); A = 0x30;
  CYC(b_+112, b_+115); mem_wr(gb, wLinkGrabState2, A);
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+117); L = E;
  CYC(b_+117, b_+118); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+118, b_+119); L = alu_inc8(gb, L);
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x78);
  CYC(b_+121, b_+123); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+123, b_+125); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+125, s_func_4c7f_hook, SYM(func_4c7f), b_+128);
  CYC(b_+128, b_+130); A = 0x03;
  CALL_C(b_+130, s_enemySetAnimation, SYM(enemySetAnimation), b_+133);
  CYC(b_+133, b_+136);
  TAIL(objectSetVisiblec1);
state2_substate1:
  CALL_C(b_+136, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+139);
  if (F & FZ) { CYCT(b_+139, b_+141); goto L_4a93; }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+142); A = mem_rd(gb, HL);
  CYC(b_+142, b_+144); alu_cp(gb, 0x2d);
  if (F & FC) CALL_C_CC(b_+144, s_enemyAnimate, SYM(enemyAnimate), b_+147);
  else CYC(b_+144, b_+147);
  CYC(b_+147, b_+150);
  TAIL(enemyAnimate);
L_4a93:
  CYC(b_+150, b_+151); L = E;
  CYC(b_+151, b_+153); mem_wr(gb, HL, 0x03);
  CYC(b_+153, b_+156);
  TAIL(dropLinkHeldItem);
state2_substate2:
  CALL_C(b_+156, s_func_4c3c_hook, SYM(func_4c3c), b_+159);
  CYC(b_+159, b_+162);
  TAIL(enemyAnimate);
state2_substate3:
  CYC(b_+162, b_+163); H = D;
  CYC(b_+163, b_+165); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x0c);
  CYC(b_+167, b_+169); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x8c);
  CYC(b_+171, b_+173); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+173, b_+175); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+175, b_+176); L = alu_inc8(gb, L);
  CYC(b_+176, b_+178); mem_wr(gb, HL, 0x42);
  CALL_C(b_+178, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+181);
  CYC(b_+181, b_+183); alu_and(gb, 0x1f);
  CYC(b_+183, b_+185); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+185, b_+186); mem_wr(gb, DE, A);
  CYC(b_+186, b_+187); H = D;
  CYC(b_+187, b_+189); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+189, b_+191); mem_wr(gb, HL, 0x1e);
  CYC(b_+191, b_+193); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+193, b_+195); A = 0x00;
  CYC(b_+195, b_+196); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0xfe);
  RET(b_+198); return;
state4:
  CYC(b_+199, b_+202); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+202, b_+203); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+203, b_+206); TAIL_S(func_4c65); }
  CYC(b_+203, b_+206);
  CYC(b_+206, b_+208); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+208, b_+209); A = mem_rd(gb, DE);
  CYC(b_+209, b_+210); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+210, s_func_4d54_hook, SYM(func_4d54), b_+213);
  else CYC(b_+210, b_+213);
  CYC(b_+213, b_+216); SET_HL(w1Link);
  CALL_C(b_+216, s_preventObjectHFromPassingObjectD, SYM(preventObjectHFromPassingObjectD), b_+219);
  CALL_C(b_+219, s_objectAddToGrabbableObjectBuffer, SYM(objectAddToGrabbableObjectBuffer), b_+222);
  CYC(b_+222, b_+225);
  TAIL(enemyAnimate);
stateStub:
  RET(b_+225); return;
state8:
  CYC(b_+226, b_+229); A = mem_rd(gb, wcc93);
  CYC(b_+229, b_+230); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+230); return; }
  CYC(b_+230, b_+231);
  CYC(b_+231, b_+232); H = D;
  CYC(b_+232, b_+233); L = E;
  CYC(b_+233, b_+234); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+234, b_+236); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+236, b_+238); mem_wr(gb, HL, 0x5a);
  CYC(b_+238, b_+240); A = 0x2d;
  CYC(b_+240, b_+243); mem_wr(gb, wActiveMusic, A);
  CYC(b_+243, b_+246);
  TAIL(playSound_b00);
state9:
  CALL_C(b_+246, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+249);
  if (!(F & FZ)) { RET_TAKEN(b_+249); return; }
  CYC(b_+249, b_+250);
  CYC(b_+250, b_+251); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+251, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+254);
  CYC(b_+254, b_+256); alu_and(gb, 0x7f);
  CYC(b_+256, b_+258); alu_add(gb, 0x10);
  CYC(b_+258, b_+259); C = A;
  CYC(b_+259, b_+261); B = 0xcf;
  CYC(b_+261, b_+262); A = mem_rd(gb, BC);
  CYC(b_+262, b_+264); alu_cp(gb, 0xfa);
  if (!(F & FZ)) { RET_TAKEN(b_+264); return; }
  CYC(b_+264, b_+265);
  CALL_C(b_+265, s_objectSetShortPosition, SYM(objectSetShortPosition), b_+268);
  CYC(b_+268, b_+270); A = 0xfe;
  CYC(b_+270, b_+271); mem_wr(gb, BC, A);
  CYC(b_+271, b_+273); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+273, b_+274); mem_wr(gb, HL, C);
  CYC(b_+274, b_+276); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+276, b_+277); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+277, b_+279); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+279, b_+281); mem_wr(gb, HL, 0x5a);
  CYC(b_+281, b_+282); alu_xor(gb, A);
  CALL_C(b_+282, s_enemySetAnimation, SYM(enemySetAnimation), b_+285);
  CYC(b_+285, b_+288);
  TAIL(objectSetVisible82);
stateA:
  CYC(b_+288, b_+291); SET_HL(w1Link);
  CALL_C(b_+291, s_preventObjectHFromPassingObjectD, SYM(preventObjectHFromPassingObjectD), b_+294);
  CALL_C(b_+294, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+297);
  if (!(F & FZ)) { CYCT(b_+297, b_+299); goto animate; }
  CYC(b_+297, b_+299);
  CYC(b_+299, b_+301); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+301, b_+302); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+302, b_+304); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+304, b_+306); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+306, b_+307); L = alu_inc8(gb, L);
  CYC(b_+307, b_+309); mem_wr(gb, HL, 0x59);
  CYC(b_+309, b_+311); A = 0x01;
  CYC(b_+311, b_+314);
  TAIL(enemySetAnimation);
stateB:
  CYC(b_+314, b_+317); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+317, b_+318); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+318, b_+320); goto L_4b47; }
  CYC(b_+318, b_+320);
  CYC(b_+320, b_+322); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+322, b_+324); A = 0x04;
  CYC(b_+324, b_+325); mem_wr(gb, DE, A);
  CYC(b_+325, b_+327); A = 0x06;
  CYC(b_+327, b_+330);
  TAIL(enemySetAnimation);
L_4b47:
  CYC(b_+330, b_+333); SET_HL(w1Link);
  CALL_C(b_+333, s_preventObjectHFromPassingObjectD, SYM(preventObjectHFromPassingObjectD), b_+336);
  CALL_C(b_+336, s_objectAddToGrabbableObjectBuffer, SYM(objectAddToGrabbableObjectBuffer), b_+339);
  CYC(b_+339, b_+341); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+341, b_+342); A = mem_rd(gb, DE);
  CYC(b_+342, b_+343); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+343, b_+346); TAIL_S(func_4c65); }
  CYC(b_+343, b_+346);
  CYC(b_+346, b_+347); A = alu_dec8(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+347, s_func_4d54_hook, SYM(func_4d54), b_+350);
  else CYC(b_+347, b_+350);
animate:
  CYC(b_+350, b_+353);
  TAIL(enemyAnimate);
stateC:
  CALL_C(b_+353, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+356);
  if (F & FZ) { CYCT(b_+356, b_+358); goto L_4b70; }
  CYC(b_+356, b_+358);
  CYC(b_+358, b_+360); C = 0x12;
  CALL_C(b_+360, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+363);
  if (F & FZ) CALL_L_CC(b_+363, omuai_func_4aaf, b_+366);
  else CYC(b_+363, b_+366);
  CALL_C(b_+366, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+369);
  CYC(b_+369, b_+371);
  goto animate;
L_4b70:
  CYC(b_+371, b_+372); L = E;
  CYC(b_+372, b_+373); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+373, b_+375); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+375, b_+377); A = 0x80;
  CYC(b_+377, b_+378); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+378, b_+380); mem_wr(gb, HL, 0xfe);
  RET(b_+380); return;
stateD:
  CYC(b_+381, b_+383); C = 0x10;
  CALL_C(b_+383, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+386);
  if (F & FZ) { CYCT(b_+386, b_+388); goto L_4b8c; }
  CYC(b_+386, b_+388);
  CYC(b_+388, b_+389); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+389, b_+390); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+390); return; }
  CYC(b_+390, b_+391);
  CYC(b_+391, b_+392); A = mem_rd(gb, HL);
  CYC(b_+392, b_+393); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+393); return; }
  CYC(b_+393, b_+394);
  CYC(b_+394, b_+396); A = 0x04;
  CYC(b_+396, b_+399);
  TAIL(enemySetAnimation);
L_4b8c:
  CALL_C(b_+399, s_func_4cb6_hook, SYM(func_4cb6), b_+402);
  CYC(b_+402, b_+404); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+404, b_+405); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+405, b_+407); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+407, b_+409); mem_wr(gb, HL, 0x28);
  CALL_C(b_+409, s_objectSetVisible82, SYM(objectSetVisible82), b_+412);
  CYC(b_+412, b_+414);
  goto animate;
stateE:
  CALL_C(b_+414, s_enemyAnimate, SYM(enemyAnimate), b_+417);
  CYC(b_+417, b_+418); H = D;
  CYC(b_+418, b_+420); L = ENEMY_BASE + OBJ_VAR32;
  CALL_C(b_+420, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+423);
  CYC(b_+423, b_+424); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+424, b_+426); goto moveTowardPosition; }
  CYC(b_+424, b_+426);
  CYC(b_+426, b_+428); A = mem_rd(gb, hFF8F);
  CYC(b_+428, b_+429); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+429, b_+431); goto moveTowardPosition; }
  CYC(b_+429, b_+431);
  CYC(b_+431, b_+433); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+433, b_+434); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+434, b_+436); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+436, b_+437); A = mem_rd(gb, DE);
  CYC(b_+437, b_+438); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+438, b_+440); goto L_4bb6; }
  CYC(b_+438, b_+440);
  CYC(b_+440, b_+441); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_4bb6:
  CYC(b_+441, b_+443); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+443, b_+445); A = 0x60;
  CYC(b_+445, b_+446); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+446, b_+448); mem_wr(gb, HL, 0xfe);
  CYC(b_+448, b_+450); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+450, b_+452); mem_wr(gb, HL, 0x19);
  CALL_C(b_+452, s_objectSetVisiblec1, SYM(objectSetVisiblec1), b_+455);
  CYC(b_+455, b_+457); A = 0x05;
  CALL_C(b_+457, s_enemySetAnimation, SYM(enemySetAnimation), b_+460);
  CYC(b_+460, b_+462);
  goto func_4bf9;
moveTowardPosition:
  CYC(b_+462, b_+465);
  TAIL(ecom_moveTowardPosition_b0d);
stateF:
  CYC(b_+465, b_+467); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+467, b_+468); A = mem_rd(gb, DE);
  CYC(b_+468, b_+469); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+469, b_+471); goto animate2; }
  CYC(b_+469, b_+471);
  CYC(b_+471, b_+473); C = 0x10;
  CALL_C(b_+473, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+476);
  if (F & FZ) { CYCT(b_+476, b_+478); goto L_4be1; }
  CYC(b_+476, b_+478);
  CALL_C(b_+478, s_func_4d36_hook, SYM(func_4d36), b_+481);
  if (!(F & FC)) { CYCT(b_+481, b_+483); goto moveTowardPosition; }
  CYC(b_+481, b_+483);
  RET(b_+483); return;
L_4be1:
  CALL_L(b_+484, omuai_func_4b72, b_+487);
  CYC(b_+487, b_+489); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+489, b_+491); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+491, b_+493); goto L_4bf1; }
  CYC(b_+491, b_+493);
  CYC(b_+493, b_+495); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+495, b_+496); A = mem_rd(gb, HL);
  CYC(b_+496, b_+498); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+498, b_+500); goto L_4bf3; }
  CYC(b_+498, b_+500);
L_4bf1:
  CYC(b_+500, b_+502); L = ENEMY_BASE + OBJ_STATE;
L_4bf3:
  CYC(b_+502, b_+503); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+503, b_+505); A = 0x05;
  CALL_C(b_+505, s_enemySetAnimation, SYM(enemySetAnimation), b_+508);
func_4bf9:
  CYC(b_+508, b_+510); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+510, b_+511); A = mem_rd(gb, DE);
  CYC(b_+511, b_+512); A = alu_inc8(gb, A);
  CYC(b_+512, b_+514); alu_and(gb, 0x03);
  CYC(b_+514, b_+515); mem_wr(gb, DE, A);
  CYC(b_+515, b_+518); SET_HL(b_+524 /* @table_4c09 */);
  CYC(b_+518, b_+519); omuai_add_double_index(gb, b_+519);
  CYC(b_+519, b_+521); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+521, b_+524);
  TAIL(add16BitRefs);

stateG:
  CYC(b_+532, b_+533); H = D;
  CYC(b_+533, b_+534); L = E;
  CYC(b_+534, b_+535); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+535, b_+537); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+537, b_+539); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+539, b_+541); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+541, b_+543); mem_wr(gb, HL, 0x14);
  CYC(b_+543, b_+545); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+545, b_+546); A = mem_rd(gb, HL);
  CYC(b_+546, b_+547); A = alu_inc8(gb, A);
  CYC(b_+547, b_+549); alu_and(gb, 0x03);
  CYC(b_+549, b_+551); A = alu_swap(gb, A);
  CYC(b_+551, b_+552); alu_rrca(gb);
  CYC(b_+552, b_+554); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+554, b_+555); mem_wr(gb, HL, A);
animate2:
  CYC(b_+555, b_+558);
  TAIL(enemyAnimate);
stateH:
  CYC(b_+558, b_+560); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+560, b_+561); A = mem_rd(gb, DE);
  CYC(b_+561, b_+562); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+562, b_+564); goto animate2; }
  CYC(b_+562, b_+564);
  CYC(b_+564, b_+566); C = 0x18;
  CALL_C(b_+566, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+569);
  if (!(F & FZ)) { CYCT(b_+569, b_+572); TAIL(objectApplySpeed); }
  CYC(b_+569, b_+572);
  CYC(b_+572, b_+575);
  TAIL_S(func_4c65);
}

void s_func_4c3c_hook(GB *gb) {
  BASE(func_4c3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(w1ReservedInteraction0_var3f + 0x02);
L_4c44:
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0x72);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_4c54; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = H;
  CYC(b_+14, b_+15); alu_cp(gb, D);
  if (F & FZ) { CYCT(b_+15, b_+17); goto L_4c54; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); push_effect(gb, HL);
  CALL_C(b_+18, s_func_4c88_hook, SYM(func_4c88), b_+21);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(b_+22, b_+24); goto L_4c64; }
  CYC(b_+22, b_+24);
L_4c54:
  CYC(b_+24, b_+25); H = alu_inc8(gb, H);
  CYC(b_+25, b_+26); A = H;
  CYC(b_+26, b_+28); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+28, b_+30); goto L_4c44; }
  CYC(b_+28, b_+30);
  CALL_C(b_+30, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0xfb);
  if (F & FZ) { CYCT(b_+35, b_+37); goto L_4c64; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); alu_cp(gb, 0xfa);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; }
  CYC(b_+39, b_+40);
L_4c64:
  CYC(b_+40, b_+41); SET_HL(pop_effect(gb));
  s_func_4c65_hook(gb); return; // falls through
}

void s_func_4c65_hook(GB *gb) {
  BASE(func_4c65);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x03;
  CALL_C(b_+2, s_objectCreateInteractionWithSubid00, SYM(objectCreateInteractionWithSubid00), b_+5);
  CYC(b_+5, b_+6); H = D;
  s_func_4c6b_hook(gb); return; // falls through
}

void s_func_4c6b_hook(GB *gb) {
  BASE(func_4c6b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x09);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x78);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  s_func_4c7f_hook(gb); return; // falls through
}

void s_func_4c7f_hook(GB *gb) {
  BASE(func_4c7f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  CYC(b_+3, b_+5); B = 0xcf;
  CYC(b_+5, b_+7); A = 0xfa;
  CYC(b_+7, b_+8); mem_wr(gb, BC, A);
  RET(b_+8); return;
}

void s_func_4c88_hook(GB *gb) {
  BASE(func_4c88);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+2); D = H;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+4, s_getShortPositionFromDE, SYM(getShortPositionFromDE), b_+7);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); SET_DE(pop_effect(gb));
  CALL_C(b_+9, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+12);
  CYC(b_+12, b_+13); alu_cp(gb, C);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); push_effect(gb, HL);
  CALL_C(b_+15, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+18);
  CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
  CYC(b_+19, b_+21); alu_cp(gb, 0xfe);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CALL_C(b_+22, s_func_4c6b_hook, SYM(func_4c6b), b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+27, b_+28); B = mem_rd(gb, HL);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+30, b_+31); C = mem_rd(gb, HL);
  CALL_C(b_+31, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+34);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto L_4cb4; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x03);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+40, b_+41); mem_wr(gb, HL, B);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+43, b_+44); mem_wr(gb, HL, C);
L_4cb4:
  CYC(b_+44, b_+45); alu_xor(gb, A);
  RET(b_+45); return;
}

void s_func_4cb6_hook(GB *gb) {
  BASE(func_4cb6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+4); C = L;
  CYC(b_+4, b_+7); SET_HL(wRoomLayout);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+9, b_+11); B = 0xff;
L_4cc1:
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0xfa);
  if (F & FZ) CALL_C_CC(b_+14, s_func_4d10_hook, SYM(func_4d10), b_+17);
  else CYC(b_+14, b_+17);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); A = L;
  CYC(b_+19, b_+21); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+21, b_+23); goto L_4cc1; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); L = A;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xfb);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CALL_C(b_+30, s_func_4cf8_hook, SYM(func_4cf8), b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, hFF8E, A);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+38); mem_wr(gb, hFF8F, A);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+40, b_+41); B = mem_rd(gb, HL);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+43, b_+44); C = mem_rd(gb, HL);
  CALL_C(b_+44, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+47);
  CYC(b_+47, b_+49); alu_add(gb, 0x04);
  CYC(b_+49, b_+51); alu_and(gb, 0x18);
  CYC(b_+51, b_+53); A = alu_swap(gb, A);
  CYC(b_+53, b_+54); alu_rlca(gb);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+60); SET_HL(SYM(table_4d0c));
  CYC(b_+60, b_+61); omuai_add_a_to_hl(gb, b_+61);
  CYC(b_+61, b_+63); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  s_func_4cf8_hook(gb); return; // falls through
}

void s_func_4cf8_hook(GB *gb) {
  BASE(func_4cf8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+10); alu_add(gb, 0x08);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  CYC(b_+14, b_+16); A = alu_swap(gb, A);
  CYC(b_+16, b_+18); alu_add(gb, 0x08);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL - 1);
  RET(b_+19); return;
}

void s_func_4d10_hook(GB *gb) {
  BASE(func_4d10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+2); A = C;
  CYC(b_+2, b_+4); alu_and(gb, 0xf0);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+7); D = A;
  CYC(b_+7, b_+8); A = L;
  CYC(b_+8, b_+10); alu_and(gb, 0xf0);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+13); alu_sub(gb, D);
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto L_4d21; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); alu_cpl(gb);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
L_4d21:
  CYC(b_+17, b_+18); D = A;
  CYC(b_+18, b_+19); A = C;
  CYC(b_+19, b_+21); alu_and(gb, 0x0f);
  CYC(b_+21, b_+22); E = A;
  CYC(b_+22, b_+23); A = L;
  CYC(b_+23, b_+25); alu_and(gb, 0x0f);
  CYC(b_+25, b_+26); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto L_4d2e; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); alu_cpl(gb);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
L_4d2e:
  CYC(b_+30, b_+31); alu_add(gb, D);
  CYC(b_+31, b_+32); SET_DE(pop_effect(gb));
  CYC(b_+32, b_+33); alu_cp(gb, B);
  if (!(F & FC)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+35); B = A;
  CYC(b_+35, b_+36); A = L;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  RET(b_+37); return;
}

void s_func_4d36_hook(GB *gb) {
  BASE(func_4d36);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); mem_wr(gb, hFF8F, A);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_sub(gb, 0x02);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); mem_wr(gb, hFF8E, A);
  CYC(b_+17, b_+18); C = mem_rd(gb, HL);
  CYC(b_+18, b_+19); alu_sub(gb, C);
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);
  CYC(b_+20, b_+22); alu_cp(gb, 0x02);
  if (!(F & FC)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); A = mem_rd(gb, hFF8F);
  CYC(b_+25, b_+26); alu_sub(gb, B);
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);
  CYC(b_+27, b_+29); alu_cp(gb, 0x02);
  RET(b_+29); return;
}

void s_func_4d54_hook(GB *gb) {
  BASE(func_4d54);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+2); mem_wr(gb, DE, A);
  CYC(b_+2, b_+4); B = 0x31;
  CALL_C(b_+4, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_YH;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_sub(gb, 0x04);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  RET(b_+17); return;
}

