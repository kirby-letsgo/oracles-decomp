#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/frypolar.s.
// ENEMY_FRYPOLAR

static uint16_t frypolar_jump_table(GB *gb) {
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

static void frypolar_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void frypolar_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_FRYPOLAR
void s_enemyCode77_hook(GB *gb) {
  BASE(enemyCode77);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); TAIL(ecom_updateKnockbackNoSolidity_b0d); }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto normalStatus; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_cp(gb, 0x9a);
  if (F & FZ) { CYCT(b_+23, b_+25); goto L_60ba; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0x9b);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto normalStatus; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+33, b_+35); goto normalStatus; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = 0x63;
  CALL_C(b_+37, s_playSound, SYM(playSound_b00), b_+40);
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x3c);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_60ba; }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+52, b_+54); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
L_60ba:
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+56, b_+58); A = 0x1e;
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+61); A = 0x83;
  CALL_C(b_+61, s_playSound, SYM(playSound_b00), b_+64);
normalStatus:
  CALL_C(b_+64, s_func_6257_hook, SYM(func_6257), b_+67);
  CALL_C(b_+67, s_func_6273_hook, SYM(func_6273), b_+70);
  CALL_C(b_+70, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+73);
  CYC(b_+73, b_+75); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+75, b_+77); goto L_60e6; }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+78); push_effect(gb, b_+78);
  do { uint16_t jt_ = (frypolar_jump_table(gb));
    if (jt_ == b_+107) goto state0;
    if (jt_ == b_+128) goto stateStub;
    if (jt_ == b_+129) goto state8;
    if (jt_ == b_+326) goto state9;
    HANDOFF(HL);
  } while (0);
L_60e6:
  CALL_C(b_+98, s_func_62b1_hook, SYM(func_62b1), b_+101);
  CYC(b_+101, b_+102); A = B;
  CYC(b_+102, b_+103); push_effect(gb, b_+103);
  do { uint16_t jt_ = (frypolar_jump_table(gb));
    if (jt_ == b_+335) goto subid0;
    if (jt_ == b_+411) goto subid1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+107, b_+110); SET_BC(0x010c);
  CALL_C(b_+110, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+113);
  if (!(F & FZ)) { RET_TAKEN(b_+113); return; }
  CYC(b_+113, b_+114);
  CALL_C(b_+114, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+117);
  CYC(b_+117, b_+119); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+119, b_+121); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+121, b_+123); B = 0x00;
  CYC(b_+123, b_+125); A = 0x77;
  CYC(b_+125, b_+128);
  TAIL(enemyBoss_initializeRoom_b0f);
stateStub:
  RET(b_+128); return;
state8:
  CYC(b_+129, b_+130); E = alu_inc8(gb, E);
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); push_effect(gb, b_+132);
  do { uint16_t jt_ = (frypolar_jump_table(gb));
    if (jt_ == b_+140) goto state8_substate0;
    if (jt_ == b_+151) goto state8_substate1;
    if (jt_ == b_+261) goto state8_substate2;
    if (jt_ == b_+283) goto state8_substate3;
    HANDOFF(HL);
  } while (0);
state8_substate0:
  CYC(b_+140, b_+143); A = mem_rd(gb, wcc93);
  CYC(b_+143, b_+144); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+144); return; }
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+146); A = alu_inc8(gb, A);
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);
  CYC(b_+147, b_+150); mem_wr(gb, wDisabledObjects, A);
  RET(b_+150); return;
state8_substate1:
  CALL_C(b_+151, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+154);
  if (!(F & FZ)) { RET_TAKEN(b_+154); return; }
  CYC(b_+154, b_+155);
  CYC(b_+155, b_+157); B = 0x02;
  CALL_C(b_+157, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+160);
  if (!(F & FZ)) { RET_TAKEN(b_+160); return; }
  CYC(b_+160, b_+161);
  CYC(b_+161, b_+163); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+167); SET_HL(b_+229 /* @state8@table_6169 */);
  CYC(b_+167, b_+168); frypolar_add_double_index(gb, b_+168);
  CYC(b_+168, b_+169); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+169, b_+170); C = mem_rd(gb, HL);
  CYC(b_+170, b_+171); B = A;
  CALL_C(b_+171, s_getFreePartSlot, SYM(getFreePartSlot), b_+174);
  CYC(b_+174, b_+176); mem_wr(gb, HL, 0x3d);
  CYC(b_+176, b_+177); L = alu_inc8(gb, L);
  CYC(b_+177, b_+179); mem_wr(gb, HL, 0x03);
  CYC(b_+179, b_+180); L = alu_inc8(gb, L);
  CYC(b_+180, b_+181); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+181, b_+183); L = PART_BASE + OBJ_YH;
  CYC(b_+183, b_+184); mem_wr(gb, HL, B);
  CYC(b_+184, b_+186); L = PART_BASE + OBJ_XH;
  CYC(b_+186, b_+187); mem_wr(gb, HL, C);
  CALL_C(b_+187, s_getFreePartSlot, SYM(getFreePartSlot), b_+190);
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x3e);
  CYC(b_+192, b_+194); L = PART_BASE + OBJ_VAR03;
  CYC(b_+194, b_+195); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+195, b_+197); L = PART_BASE + OBJ_YH;
  CYC(b_+197, b_+199); A = 0x58;
  CYC(b_+199, b_+200); alu_sub(gb, B);
  CYC(b_+200, b_+202); alu_add(gb, 0x58);
  CYC(b_+202, b_+203); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+203, b_+204); L = alu_inc8(gb, L);
  CYC(b_+204, b_+206); A = 0x78;
  CYC(b_+206, b_+207); alu_sub(gb, C);
  CYC(b_+207, b_+209); alu_add(gb, 0x78);
  CYC(b_+209, b_+210); mem_wr(gb, HL, A);
  CYC(b_+210, b_+212); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+212, b_+214); A = 0x80;
  CYC(b_+214, b_+215); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+215, b_+216); mem_wr(gb, HL, D);
  CYC(b_+216, b_+217); H = D;
  CYC(b_+217, b_+219); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+219, b_+221); mem_wr(gb, HL, 0x0f);
  CYC(b_+221, b_+222); L = alu_dec8(gb, L);
  CYC(b_+222, b_+223); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+223, b_+224); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+224, b_+226); alu_cp(gb, 0x10);
  if (F & FC) { RET_TAKEN(b_+226); return; }
  CYC(b_+226, b_+227);
  CYC(b_+227, b_+228); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+228); return;

state8_substate2:
  CALL_C(b_+261, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+264);
  if (!(F & FZ)) { RET_TAKEN(b_+264); return; }
  CYC(b_+264, b_+265);
  CYC(b_+265, b_+268); SET_BC(0x0502);
  CALL_C(b_+268, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+271);
  if (!(F & FZ)) { RET_TAKEN(b_+271); return; }
  CYC(b_+271, b_+272);
  CYC(b_+272, b_+273); A = H;
  CYC(b_+273, b_+274); H = D;
  CYC(b_+274, b_+276); L = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+276, b_+277); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+277, b_+279); mem_wr(gb, HL, 0x40);
  CYC(b_+279, b_+281); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+281, b_+282); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+282); return;
state8_substate3:
  CYC(b_+283, b_+285); A = 0x21;
  CALL_C(b_+285, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+288);
  CYC(b_+288, b_+290); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+290); return; }
  CYC(b_+290, b_+291);
  CYC(b_+291, b_+292); H = D;
  CYC(b_+292, b_+294); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+294, b_+295); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+295, b_+297); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+297, b_+299); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+299, b_+301); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+301, b_+303); mem_wr(gb, HL, 0x3c);
  CYC(b_+303, b_+305); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+305, b_+307); mem_wr(gb, HL, 0x56);
  CYC(b_+307, b_+309); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+309, b_+311); mem_wr(gb, HL, 0xfe);
  CALL_C(b_+311, s_objectSetVisible83, SYM(objectSetVisible83), b_+314);
  CYC(b_+314, b_+315); alu_xor(gb, A);
  CYC(b_+315, b_+318); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+318, b_+320); A = 0x2d;
  CYC(b_+320, b_+323); mem_wr(gb, wActiveMusic, A);
  CYC(b_+323, b_+326);
  TAIL(playSound_b00);
state9:
  CALL_C(b_+326, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+329);
  if (!(F & FZ)) { CYCT(b_+329, b_+332); TAIL(enemyAnimate); }
  CYC(b_+329, b_+332);
  CYC(b_+332, b_+333); L = E;
  CYC(b_+333, b_+334); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+334); return;
subid0:
  CYC(b_+335, b_+336); A = mem_rd(gb, DE);
  CYC(b_+336, b_+338); alu_sub(gb, 0x0a);
  CYC(b_+338, b_+339); push_effect(gb, b_+339);
  do { uint16_t jt_ = (frypolar_jump_table(gb));
    if (jt_ == b_+347) goto subid0_stateA;
    if (jt_ == b_+364) goto subid0_stateB;
    if (jt_ == b_+383) goto subid0_stateC;
    if (jt_ == b_+401) goto subid0_stateD;
    HANDOFF(HL);
  } while (0);
subid0_stateA:
  CYC(b_+347, b_+348); H = D;
  CYC(b_+348, b_+349); L = E;
  CYC(b_+349, b_+350); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+350, b_+352); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+352, b_+354); mem_wr(gb, HL, 0x55);
  CYC(b_+354, b_+356); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+356, b_+358); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+358, b_+359); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+359, b_+361); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+361, b_+362); mem_wr(gb, HL, A);
  CYC(b_+362, b_+364);
  goto subid0_animate;
subid0_stateB:
  CYC(b_+364, b_+367); A = mem_rd(gb, wFrameCounter);
  CYC(b_+367, b_+369); alu_and(gb, 0x0f);
  CYC(b_+369, b_+371); A = 0xae;
  if (F & FZ) CALL_C_CC(b_+371, s_playSound, SYM(playSound_b00), b_+374);
  else CYC(b_+371, b_+374);
  CALL_ROM(b_+374, SYM(func_62cc)); /* no hook: interpreted */
  if (!(F & FC)) CALL_C_CC(b_+377, s_ecom_moveTowardPosition, SYM(ecom_moveTowardPosition_b0d), b_+380);
  else CYC(b_+377, b_+380);
subid0_animate:
  CYC(b_+380, b_+383);
  TAIL(enemyAnimate);
subid0_stateC:
  CALL_C(b_+383, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+386);
  if (F & FZ) { CYCT(b_+386, b_+388); goto L_620d; }
  CYC(b_+386, b_+388);
  CALL_C(b_+388, s_func_62f3_hook, SYM(func_62f3), b_+391);
  CYC(b_+391, b_+393);
  goto subid0_animate;
L_620d:
  CALL_C(b_+393, s_func_62a8_hook, SYM(func_62a8), b_+396);
  CALL_C(b_+396, s_func_6304_hook, SYM(func_6304), b_+399);
  CYC(b_+399, b_+401);
  goto subid0_animate;
subid0_stateD:
  CALL_C(b_+401, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+404);
  if (!(F & FZ)) { CYCT(b_+404, b_+406); goto subid0_animate; }
  CYC(b_+404, b_+406);
  CYC(b_+406, b_+407); L = E;
  CYC(b_+407, b_+409); mem_wr(gb, HL, 0x0a);
  CYC(b_+409, b_+411);
  goto subid0_animate;
subid1:
  CYC(b_+411, b_+412); A = mem_rd(gb, DE);
  CYC(b_+412, b_+414); alu_sub(gb, 0x0a);
  CYC(b_+414, b_+415); push_effect(gb, b_+415);
  do { uint16_t jt_ = (frypolar_jump_table(gb));
    if (jt_ == b_+423) goto subid1_stateA;
    if (jt_ == b_+364) goto subid0_stateB;
    HANDOFF(HL);
  } while (0);
subid1_stateA:
  CYC(b_+423, b_+424); H = D;
  CYC(b_+424, b_+425); L = E;
  CYC(b_+425, b_+426); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+426, b_+428); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+428, b_+430); mem_wr(gb, HL, 0x6e);
  CYC(b_+430, b_+433);
  TAIL_S(func_6326);
}

void s_func_6257_hook(GB *gb) {
  BASE(func_6257);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x04);
  if (F & FC) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_6266; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+15);
  TAIL(enemyAnimate);
L_6266:
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x5a);
  CYC(b_+23, b_+25); A = 0x83;
  CYC(b_+25, b_+28);
  TAIL(playSound_b00);
}

void s_func_6273_hook(GB *gb) {
  BASE(func_6273);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  s_seasonsFunc_0e_627d_hook(gb); return; // falls through
}

void s_seasonsFunc_0e_627d_hook(GB *gb) {
  BASE(seasonsFunc_0e_627d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto L_6281; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); SET_BC(pop_effect(gb));
  RET(b_+3); return;
L_6281:
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_6295; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); SET_BC(pop_effect(gb));
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_6292; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); alu_and(gb, 0x01);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
L_6292:
  CYC(b_+21, b_+24);
  TAIL(enemyAnimate);
L_6295:
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
  CYC(b_+28, b_+30); alu_and(gb, 0x01);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+32); B = A;
  CYC(b_+32, b_+34); A = 0x02;
  CYC(b_+34, b_+35); alu_sub(gb, B);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x0a);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x00);
  RET(b_+51); return;
}

void s_func_62b1_hook(GB *gb) {
  BASE(func_62b1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0x30);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+16); SET_HL(SYM(table_62c8));
  CYC(b_+16, b_+17); frypolar_add_a_to_hl(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  RET(b_+22); return;
}

void s_func_62a8_hook(GB *gb) {
  BASE(func_62a8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x78);
  CYC(b_+2, b_+3); L = alu_inc8(gb, L);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x96);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+10); return;
}

void s_func_62f3_hook(GB *gb) {
  BASE(func_62f3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+3); alu_and(gb, 0x03);
  CYC(b_+3, b_+6); SET_HL(SYM(table_6300));
  CYC(b_+6, b_+7); frypolar_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return;
}

void s_func_6304_hook(GB *gb) {
  BASE(func_6304);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, s_getRandomNumber, SYM(getRandomNumber), b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x55);
  CYC(b_+9, b_+10); A = B;
  if (F & FC) { CYCT(b_+10, b_+12); TAIL_S(func_631a); }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_sub(gb, 0x02);
  CYC(b_+14, b_+16); alu_and(gb, 0x1f);
  CALL_C(b_+16, s_func_631a_hook, SYM(func_631a), b_+19);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+22); alu_add(gb, 0x04);
  s_func_631a_hook(gb); return; // falls through
}

void s_func_631a_hook(GB *gb) {
  BASE(func_631a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, AF);
  CYC(b_+1, b_+3); B = 0x3d;
  CALL_C(b_+3, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+6);
  CYC(b_+6, b_+7); SET_BC(pop_effect(gb));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  RET(b_+11); return;
}

void s_func_6326_hook(GB *gb) {
  BASE(func_6326);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6326:
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x0e);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_6326; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+15); SET_HL(SYM(table_633e));
  CYC(b_+15, b_+16); frypolar_add_a_to_hl(gb, b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); E = alu_inc8(gb, E);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  RET(b_+23); return;
}

