#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/blaino.s.
// ENEMY_BLAINO

static uint16_t blaino_jump_table(GB *gb) {
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

static void blaino_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_BLAINO
void s_enemyCode54_hook(GB *gb) {
  BASE(enemyCode54);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(enemyDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); TAIL(ecom_updateKnockback_b0d); }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); A = A & 0x7f;
  CYC(b_+17, b_+19); alu_sub(gb, 0x0a);
  CYC(b_+19, b_+21); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto normalStatus; }
  CYC(b_+21, b_+23);
  CALL_C(b_+23, s_seasonsFunc_0d_73df_hook, SYM(seasonsFunc_0d_73df), b_+26);
  CYC(b_+26, b_+27); H = D;
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x40);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x0a);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+39, b_+41); A = 0x0f;
  CYC(b_+41, b_+42); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+42, b_+44); goto normalStatus; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x00);
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_add(gb, 0x05);
  CALL_C(b_+54, s_enemySetAnimation, SYM(enemySetAnimation), b_+57);
  CYC(b_+57, b_+59); A = 0x24;
  CALL_C(b_+59, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+62);
  CYC(b_+62, b_+64); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+64, b_+66); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x03);
normalStatus:
  CALL_C(b_+68, s_seasonsFunc_0d_7323_hook, SYM(seasonsFunc_0d_7323), b_+71);
  CYC(b_+71, b_+74); A = mem_rd(gb, wShootingGalleryBallStatus);
  CYC(b_+74, b_+76); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+76, b_+78); goto L_70ee; }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+82, b_+85); TAIL_S(seasonsFunc_0d_7312); }
  CYC(b_+82, b_+85);
L_70ee:
  CYC(b_+85, b_+87); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+87, b_+88); A = mem_rd(gb, DE);
  CYC(b_+88, b_+89); push_effect(gb, b_+89);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+121) goto state0;
    if (jt_ == b_+156) goto state_stub;
    if (jt_ == b_+157) goto state8;
    if (jt_ == b_+184) goto state9;
    if (jt_ == b_+190) goto stateA;
    if (jt_ == b_+202) goto stateB;
    if (jt_ == b_+220) goto stateC;
    if (jt_ == b_+236) goto stateD;
    if (jt_ == b_+262) goto stateE;
    if (jt_ == b_+294) goto stateF;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+121, s_getFreeEnemySlot_uncounted, SYM(getFreeEnemySlot_uncounted), b_+124);
  if (!(F & FZ)) { RET_TAKEN(b_+124); return; }
  CYC(b_+124, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x5f);
  CYC(b_+127, b_+129); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+129, b_+131); A = 0x80;
  CYC(b_+131, b_+132); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+132, b_+133); mem_wr(gb, HL, D);
  CYC(b_+133, b_+135); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+135, b_+136); mem_wr(gb, DE, A);
  CYC(b_+136, b_+137); E = alu_inc8(gb, E);
  CYC(b_+137, b_+138); A = H;
  CYC(b_+138, b_+139); mem_wr(gb, DE, A);
  CALL_C(b_+139, s_objectCopyPosition, SYM(objectCopyPosition), b_+142);
  CALL_C(b_+142, s_seasonsFunc_0d_736d_hook, SYM(seasonsFunc_0d_736d), b_+145);
  CYC(b_+145, b_+147); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+147, b_+149); A = 0x01;
  CYC(b_+149, b_+150); mem_wr(gb, DE, A);
  CALL_C(b_+150, s_enemySetAnimation, SYM(enemySetAnimation), b_+153);
  CYC(b_+153, b_+156);
  TAIL(objectSetVisiblec2);
state_stub:
  RET(b_+156); return;
state8:
  CALL_C(b_+157, s_seasonsFunc_0d_72d4_hook, SYM(seasonsFunc_0d_72d4), b_+160);
  CYC(b_+160, b_+161); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+161, b_+163); goto L_7143; }
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+165); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+165, b_+167); A = 0x0d;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  CYC(b_+168, b_+170);
  goto animate;
L_7143:
  CALL_C(b_+170, s_seasonsFunc_0d_73b1_hook, SYM(seasonsFunc_0d_73b1), b_+173);
  CYC(b_+173, b_+175); A = 0x09;
  if (!(F & FC)) { CYCT(b_+175, b_+177); goto L_714c; }
  CYC(b_+175, b_+177);
  CYC(b_+177, b_+179); A = 0x0b;
L_714c:
  CYC(b_+179, b_+181); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+181, b_+182); mem_wr(gb, DE, A);
  CYC(b_+182, b_+184);
  goto animate;
state9:
  CYC(b_+184, b_+186); A = 0x0a;
  CYC(b_+186, b_+187); mem_wr(gb, DE, A);
  CYC(b_+187, b_+190);
  TAIL_S(seasonsFunc_0d_7350);
stateA:
  CALL_C(b_+190, s_seasonsFunc_0d_7312_hook, SYM(seasonsFunc_0d_7312), b_+193);
  if (F & FZ) CALL_C_CC(b_+193, s_seasonsFunc_0d_736d_hook, SYM(seasonsFunc_0d_736d), b_+196);
  else CYC(b_+193, b_+196);
  CALL_C(b_+196, s_objectApplySpeed, SYM(objectApplySpeed), b_+199);
animate:
  CYC(b_+199, b_+202);
  TAIL(enemyAnimate);
stateB:
  CYC(b_+202, b_+204); A = 0x0c;
  CYC(b_+204, b_+205); mem_wr(gb, DE, A);
  CYC(b_+205, b_+206); E = alu_inc8(gb, E);
  CYC(b_+206, b_+207); alu_xor(gb, A);
  CYC(b_+207, b_+208); mem_wr(gb, DE, A);
  CALL_C(b_+208, s_seasonsFunc_0d_737f_hook, SYM(seasonsFunc_0d_737f), b_+211);
  CYC(b_+211, b_+213); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+213, b_+214); A = B;
  CYC(b_+214, b_+215); mem_wr(gb, DE, A);
  CYC(b_+215, b_+217); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+217, b_+218); A = alu_inc8(gb, A);
  CYC(b_+218, b_+219); mem_wr(gb, DE, A);
  RET(b_+219); return;
stateC:
  CYC(b_+220, b_+222); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+222, b_+223); A = mem_rd(gb, DE);
  CYC(b_+223, b_+225); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+225, b_+226); push_effect(gb, b_+226);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+324) goto stateCvar03_0;
    if (jt_ == b_+333) goto stateCvar03_1;
    if (jt_ == b_+352) goto stateCvar03_2;
    if (jt_ == b_+431) goto stateCvar03_3;
    if (jt_ == b_+483) goto stateCvar03_4;
    HANDOFF(HL);
  } while (0);
stateD:
  CYC(b_+236, b_+237); H = D;
  CYC(b_+237, b_+238); L = E;
  CYC(b_+238, b_+239); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+239, b_+240); L = alu_inc8(gb, L);
  CYC(b_+240, b_+242); mem_wr(gb, HL, 0x00);
  CYC(b_+242, b_+244); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+244, b_+246); mem_wr(gb, HL, 0x2d);
  CALL_C(b_+246, s_seasonsFunc_0d_7395_hook, SYM(seasonsFunc_0d_7395), b_+249);
  if (!(F & FC)) { RET_TAKEN(b_+249); return; }
  CYC(b_+249, b_+250);
  CYC(b_+250, b_+252); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+252, b_+253); A = mem_rd(gb, DE);
  CYC(b_+253, b_+256); SET_HL(SYM(seasonsTable_0d_73d7));
  CYC(b_+256, b_+257); blaino_add_a_to_hl(gb, b_+257);
  CYC(b_+257, b_+259); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+259, b_+260); A = mem_rd(gb, HL);
  CYC(b_+260, b_+261); mem_wr(gb, DE, A);
  RET(b_+261); return;
stateE:
  CYC(b_+262, b_+263); E = alu_inc8(gb, E);
  CYC(b_+263, b_+264); A = mem_rd(gb, DE);
  CYC(b_+264, b_+265); push_effect(gb, b_+265);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+269) goto stateEcont;
    if (jt_ == b_+190) goto stateA;
    HANDOFF(HL);
  } while (0);
stateEcont:
  CALL_C(b_+269, s_seasonsFunc_0d_7312_hook, SYM(seasonsFunc_0d_7312), b_+272);
  if (F & FZ) { CYCT(b_+272, b_+274); goto L_71b0; }
  CYC(b_+272, b_+274);
  CALL_C(b_+274, s_objectApplySpeed, SYM(objectApplySpeed), b_+277);
  CYC(b_+277, b_+279);
  goto animate;
L_71b0:
  CYC(b_+279, b_+281); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+281, b_+283); A = 0x01;
  CYC(b_+283, b_+284); mem_wr(gb, DE, A);
  CYC(b_+284, b_+287); SET_BC(0x4050);
  CALL_C(b_+287, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+290);
  CYC(b_+290, b_+292); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+292, b_+293); mem_wr(gb, DE, A);
  RET(b_+293); return;
stateF:
  CALL_C(b_+294, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+297);
  if (!(F & FZ)) { RET_TAKEN(b_+297); return; }
  CYC(b_+297, b_+298);
  CYC(b_+298, b_+300); A = 0x24;
  CALL_C(b_+300, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+303);
  CYC(b_+303, b_+305); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+305, b_+306); L = alu_inc8(gb, L);
  CYC(b_+306, b_+308); mem_wr(gb, HL, 0x40);
  CYC(b_+308, b_+310); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+310, b_+312); mem_wr(gb, HL, 0x01);
  CYC(b_+312, b_+313); L = alu_inc8(gb, L);
  CYC(b_+313, b_+315); mem_wr(gb, HL, 0x00);
  CYC(b_+315, b_+317); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+317, b_+318); A = mem_rd(gb, DE);
  CALL_C(b_+318, s_enemySetAnimation, SYM(enemySetAnimation), b_+321);
  CYC(b_+321, b_+324);
  TAIL_S(seasonsFunc_0d_736d);
stateCvar03_0:
  CALL_C(b_+324, s_seasonsFunc_0d_7312_hook, SYM(seasonsFunc_0d_7312), b_+327);
  if (F & FZ) { CYCT(b_+327, b_+330); TAIL_S(seasonsFunc_0d_736d); }
  CYC(b_+327, b_+330);
  CYC(b_+330, b_+333);
  TAIL(enemyAnimate);
stateCvar03_1:
  CYC(b_+333, b_+334); A = mem_rd(gb, DE);
  CYC(b_+334, b_+335); push_effect(gb, b_+335);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+339) goto stateCvar03_1_state0;
    if (jt_ == b_+345) goto stateCvar03_1_state1;
    HANDOFF(HL);
  } while (0);
stateCvar03_1_state0:
  CYC(b_+339, b_+340); H = D;
  CYC(b_+340, b_+341); L = E;
  CYC(b_+341, b_+342); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+342, b_+343); L = alu_inc8(gb, L);
  CYC(b_+343, b_+345); mem_wr(gb, HL, 0x3c);
stateCvar03_1_state1:
  CALL_C(b_+345, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+348);
  if (!(F & FZ)) { RET_TAKEN(b_+348); return; }
  CYC(b_+348, b_+349);
  CYC(b_+349, b_+352);
  TAIL_S(seasonsFunc_0d_736d);
stateCvar03_2:
  CYC(b_+352, b_+353); A = mem_rd(gb, DE);
  CYC(b_+353, b_+354); push_effect(gb, b_+354);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+364) goto stateCvar03_2_substate0;
    if (jt_ == b_+380) goto stateCvar03_2_substate1;
    if (jt_ == b_+390) goto stateCvar03_2_substate2;
    if (jt_ == b_+408) goto stateCvar03_2_substate3;
    if (jt_ == b_+424) goto stateCvar03_2_substate4;
    HANDOFF(HL);
  } while (0);
stateCvar03_2_substate0:
  CYC(b_+364, b_+365); H = D;
  CYC(b_+365, b_+366); L = E;
  CYC(b_+366, b_+367); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+367, b_+368); L = alu_inc8(gb, L);
  CYC(b_+368, b_+370); mem_wr(gb, HL, 0x1e);
  CYC(b_+370, b_+372); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+372, b_+374); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+374, s_seasonsFunc_0d_73cd_hook, SYM(seasonsFunc_0d_73cd), b_+377);
  CYC(b_+377, b_+379); alu_xor(gb, 0x10);
  CYC(b_+379, b_+380); mem_wr(gb, DE, A);
stateCvar03_2_substate1:
  CALL_C(b_+380, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+386); TAIL(objectApplySpeed); }
  CYC(b_+383, b_+386);
  CYC(b_+386, b_+388); mem_wr(gb, HL, 0x04);
  CYC(b_+388, b_+389); L = E;
  CYC(b_+389, b_+390); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
stateCvar03_2_substate2:
  CALL_C(b_+390, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+393);
  if (!(F & FZ)) { RET_TAKEN(b_+393); return; }
  CYC(b_+393, b_+394);
  CYC(b_+394, b_+396); mem_wr(gb, HL, 0x1e);
  CYC(b_+396, b_+397); L = E;
  CYC(b_+397, b_+398); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+398, b_+400); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+400, b_+402); mem_wr(gb, HL, 0x50);
  CYC(b_+402, b_+404); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+404, b_+405); A = mem_rd(gb, HL);
  CYC(b_+405, b_+407); alu_xor(gb, 0x10);
  CYC(b_+407, b_+408); mem_wr(gb, HL, A);
stateCvar03_2_substate3:
  CALL_C(b_+408, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+411);
  if (F & FZ) { CYCT(b_+411, b_+413); goto L_723d; }
  CYC(b_+411, b_+413);
  CYC(b_+413, b_+414); A = mem_rd(gb, HL);
  CYC(b_+414, b_+416); alu_cp(gb, 0x1a);
  if (!(F & FC)) { CYCT(b_+416, b_+419); TAIL(objectApplySpeed); }
  CYC(b_+416, b_+419);
  RET(b_+419); return;
L_723d:
  CYC(b_+420, b_+422); mem_wr(gb, HL, 0x1e);
  CYC(b_+422, b_+423); L = E;
  CYC(b_+423, b_+424); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
stateCvar03_2_substate4:
  CALL_C(b_+424, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+427);
  if (!(F & FZ)) { RET_TAKEN(b_+427); return; }
  CYC(b_+427, b_+428);
  CYC(b_+428, b_+431);
  TAIL_S(seasonsFunc_0d_736d);
stateCvar03_3:
  CYC(b_+431, b_+432); A = mem_rd(gb, DE);
  CYC(b_+432, b_+433); push_effect(gb, b_+433);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+441) goto stateCvar03_3_substate0;
    if (jt_ == b_+447) goto stateCvar03_3_substate1;
    if (jt_ == b_+466) goto stateCvar03_3_substate2;
    if (jt_ == b_+476) goto stateCvar03_3_substate3;
    HANDOFF(HL);
  } while (0);
stateCvar03_3_substate0:
  CYC(b_+441, b_+442); H = D;
  CYC(b_+442, b_+443); L = E;
  CYC(b_+443, b_+444); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+444, b_+445); L = alu_inc8(gb, L);
  CYC(b_+445, b_+447); mem_wr(gb, HL, 0x03);
stateCvar03_3_substate1:
  CALL_C(b_+447, s_seasonsFunc_0d_7312_hook, SYM(seasonsFunc_0d_7312), b_+450);
  if (!(F & FZ)) { RET_TAKEN(b_+450); return; }
  CYC(b_+450, b_+451);
  CALL_C(b_+451, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+454);
  if (!(F & FZ)) { RET_TAKEN(b_+454); return; }
  CYC(b_+454, b_+455);
  CYC(b_+455, b_+457); mem_wr(gb, HL, 0x0a);
  CYC(b_+457, b_+458); L = alu_dec8(gb, L);
  CYC(b_+458, b_+459); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+459, b_+461); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+461, b_+463); mem_wr(gb, HL, 0x28);
  CALL_C(b_+463, s_seasonsFunc_0d_73cd_hook, SYM(seasonsFunc_0d_73cd), b_+466);
stateCvar03_3_substate2:
  CALL_C(b_+466, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+469);
  if (!(F & FZ)) { CYCT(b_+469, b_+472); TAIL(objectApplySpeed); }
  CYC(b_+469, b_+472);
  CYC(b_+472, b_+474); mem_wr(gb, HL, 0x28);
  CYC(b_+474, b_+475); L = E;
  CYC(b_+475, b_+476); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
stateCvar03_3_substate3:
  CALL_C(b_+476, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+479);
  if (!(F & FZ)) { RET_TAKEN(b_+479); return; }
  CYC(b_+479, b_+480);
  CYC(b_+480, b_+483);
  TAIL_S(seasonsFunc_0d_736d);
stateCvar03_4:
  CYC(b_+483, b_+484); A = mem_rd(gb, DE);
  CYC(b_+484, b_+485); push_effect(gb, b_+485);
  do { uint16_t jt_ = (blaino_jump_table(gb));
    if (jt_ == b_+491) goto stateCvar03_4_substate0;
    if (jt_ == b_+510) goto stateCvar03_4_substate1;
    if (jt_ == b_+535) goto stateCvar03_4_substate2;
    HANDOFF(HL);
  } while (0);
stateCvar03_4_substate0:
  CYC(b_+491, b_+492); H = D;
  CYC(b_+492, b_+493); L = E;
  CYC(b_+493, b_+494); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+494, b_+496); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+496, b_+498); mem_wr(gb, HL, 0x28);
  CYC(b_+498, b_+500); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+500, b_+502); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+502, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+505);
  CYC(b_+505, b_+507); A = 0x04;
  CYC(b_+507, b_+510);
  TAIL(enemySetAnimation);
stateCvar03_4_substate1:
  CALL_C(b_+510, s_seasonsFunc_0d_7312_hook, SYM(seasonsFunc_0d_7312), b_+513);
  if (F & FZ) { CYCT(b_+513, b_+515); goto L_72a1; }
  CYC(b_+513, b_+515);
  CALL_C(b_+515, s_objectApplySpeed, SYM(objectApplySpeed), b_+518);
  CYC(b_+518, b_+520);
  goto L_72d1;
L_72a1:
  CYC(b_+520, b_+521); H = D;
  CYC(b_+521, b_+523); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+523, b_+524); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+524, b_+526); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+526, b_+528); A = 0xc0;
  CYC(b_+528, b_+529); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+529, b_+531); mem_wr(gb, HL, 0xfc);
  CYC(b_+531, b_+533); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+533, b_+535); mem_wr(gb, HL, 0x0f);
stateCvar03_4_substate2:
  CYC(b_+535, b_+537); C = 0x28;
  CALL_C(b_+537, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+540);
  if (!(F & FZ)) { CYCT(b_+540, b_+542); goto L_72c2; }
  CYC(b_+540, b_+542);
  CYC(b_+542, b_+544); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+544, b_+546); A = 0x02;
  CYC(b_+546, b_+547); mem_wr(gb, DE, A);
  CALL_C(b_+547, s_enemySetAnimation, SYM(enemySetAnimation), b_+550);
  CYC(b_+550, b_+553);
  TAIL_S(seasonsFunc_0d_736d);
L_72c2:
  CYC(b_+553, b_+555); E = 0x95;
  CYC(b_+555, b_+556); A = mem_rd(gb, DE);
  CYC(b_+556, b_+557); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+557, b_+559); goto L_72cd; }
  CYC(b_+557, b_+559);
  CYC(b_+559, b_+560); H = D;
  CYC(b_+560, b_+562); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+562, b_+564); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
L_72cd:
  CYC(b_+564, b_+565); alu_rla(gb);
  if (F & FC) CALL_C_CC(b_+565, s_objectApplySpeed, SYM(objectApplySpeed), b_+568);
  else CYC(b_+565, b_+568);
L_72d1:
  CYC(b_+568, b_+571);
  TAIL(enemyAnimate);
}

void s_seasonsFunc_0d_72d4_hook(GB *gb) {
  BASE(seasonsFunc_0d_72d4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_0d_72dc_hook, SYM(seasonsFunc_0d_72dc), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  RET(b_+7); return;
}

void s_seasonsFunc_0d_72dc_hook(GB *gb) {
  BASE(seasonsFunc_0d_72dc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0xff;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_sub(gb, 0x20);
  CYC(b_+7, b_+9); alu_cp(gb, 0x36);
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto L_7302; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_sub(gb, 0x30);
  CYC(b_+16, b_+18); alu_cp(gb, 0x40);
  if (F & FC) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); B = 0x02;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(b_+24, b_+26); goto L_72f7; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); B = alu_inc8(gb, B);
L_72f7:
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+32); alu_cp(gb, 0x39);
  if (F & FC) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+36); alu_add(gb, 0x02);
  CYC(b_+36, b_+37); B = A;
  RET(b_+37); return;
L_7302:
  CYC(b_+38, b_+39); B = alu_inc8(gb, B);
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_cp(gb, 0x39);
  if (F & FC) { CYCT(b_+42, b_+44); goto L_730a; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); B = 0x06;
L_730a:
  CYC(b_+46, b_+48); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+51); alu_cp(gb, 0x50);
  if (F & FC) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+53); B = alu_inc8(gb, B);
  RET(b_+53); return;
}

void s_seasonsFunc_0d_7312_hook(GB *gb) {
  BASE(seasonsFunc_0d_7312);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x0e;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+9); TAIL(objectUpdateSpeedZ_paramC); }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); SET_BC(hOamFunc);
  CYC(b_+14, b_+17);
  TAIL(objectSetSpeedZ);
}

void s_seasonsFunc_0d_7323_hook(GB *gb) {
  BASE(seasonsFunc_0d_7323);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_0d_7340_hook, SYM(seasonsFunc_0d_7340), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wFrameCounter);
  CYC(b_+7, b_+9); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+13);
  CYC(b_+13, b_+15); alu_add(gb, 0x04);
  CYC(b_+15, b_+17); alu_and(gb, 0x18);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+20); alu_rlca(gb);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+29);
  TAIL(enemySetAnimation);
}

void s_seasonsFunc_0d_7340_hook(GB *gb) {
  BASE(seasonsFunc_0d_7340);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto L_734e; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  RET(b_+13); return;
L_734e:
  CYC(b_+14, b_+15); alu_or(gb, D);
  RET(b_+15); return;
}

void s_seasonsFunc_0d_7350_hook(GB *gb) {
  BASE(seasonsFunc_0d_7350);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+8); TAIL(ecom_updateAngleTowardTarget_b0d); }
  CYC(b_+5, b_+8);
  CALL_C(b_+8, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x01);
  CYC(b_+13, b_+15); B = 0xf8;
  if (F & FZ) { CYCT(b_+15, b_+17); goto L_7363; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); B = 0x08;
L_7363:
  CYC(b_+19, b_+20); push_effect(gb, BC);
  CALL_C(b_+20, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+23);
  CYC(b_+23, b_+24); SET_BC(pop_effect(gb));
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return;
}

void s_seasonsFunc_0d_736d_hook(GB *gb) {
  BASE(seasonsFunc_0d_736d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x08);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x56);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x0f);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x00);
  RET(b_+17); return;
}

void s_seasonsFunc_0d_737f_hook(GB *gb) {
  BASE(seasonsFunc_0d_737f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); B = 0x00;
  CYC(b_+5, b_+7); alu_cp(gb, 0x30);
  if (F & FC) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); B = alu_inc8(gb, B);
  CYC(b_+9, b_+11); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); B = alu_inc8(gb, B);
  CYC(b_+13, b_+15); alu_cp(gb, 0xe0);
  if (F & FC) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); B = alu_inc8(gb, B);
  CYC(b_+17, b_+19); alu_cp(gb, 0xff);
  if (F & FC) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); B = alu_inc8(gb, B);
  RET(b_+21); return;
}

void s_seasonsFunc_0d_7395_hook(GB *gb) {
  BASE(seasonsFunc_0d_7395);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x4050);
  CALL_C(b_+3, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+6);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); push_effect(gb, BC);
  CALL_C(b_+8, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+11);
  CYC(b_+11, b_+12); SET_BC(pop_effect(gb));
  CYC(b_+12, b_+13); alu_sub(gb, B);
  CYC(b_+13, b_+15); alu_add(gb, 0x02);
  CYC(b_+15, b_+17); alu_cp(gb, 0x05);
  if (F & FC) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); A = B;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  RET(b_+27); return;
}

void s_seasonsFunc_0d_73b1_hook(GB *gb) {
  BASE(seasonsFunc_0d_73b1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x09;
  CALL_C(b_+2, s_objectCheckCenteredWithLink, SYM(objectCheckCenteredWithLink), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); C = 0x1c;
  CALL_C(b_+8, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+11);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CALL_C(b_+12, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+15);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_rrca(gb);
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+23); alu_sub(gb, B);
  CYC(b_+23, b_+25); alu_add(gb, 0x04);
  CYC(b_+25, b_+27); alu_cp(gb, 0x09);
  RET(b_+27); return;
}

void s_seasonsFunc_0d_73cd_hook(GB *gb) {
  BASE(seasonsFunc_0d_73cd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rrca(gb);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  RET(b_+9); return;
}

void s_seasonsFunc_0d_73df_hook(GB *gb) {
  BASE(seasonsFunc_0d_73df);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x2b;
  CALL_C(b_+2, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  RET(b_+9); return;
}
