#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/gleeok.s.
// ENEMY_GLEEOK

static uint16_t gleeok_jump_table(GB *gb) {
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

static void gleeok_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// enemyCode06@subid2@stateB@func_6a9f
static void gleeok_subid2_stateB_func_6a9f(GB *gb) {
  BASE(enemyCode06);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+970, b_+971); H = D;
  CYC(b_+971, b_+973); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+973, b_+974); A = mem_rd(gb, HL);
  CYC(b_+974, b_+975); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+975); return; }
  CYC(b_+975, b_+976);
  CYC(b_+976, b_+977); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+977); return;
}

// ENEMY_GLEEOK
void s_enemyCode06_hook(GB *gb) {
  BASE(enemyCode06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+14); TAIL(enemyBoss_dead_b0f); }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+21); TAIL(enemyDie_uncounted_withoutItemDrop); }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_cp(gb, 0x02);
  CYC(b_+26, b_+28); B = 0x02;
  if (F & FZ) { CYCT(b_+28, b_+30); goto L_66f5; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); B = 0x04;
L_66f5:
  CYC(b_+32, b_+34); A = 0x38;
  CALL_C(b_+34, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); alu_or(gb, B);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+45); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto L_671c; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+52); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+52, b_+54); goto L_671c; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_xor(gb, 0x01);
  CYC(b_+59, b_+61); alu_add(gb, 0xae);
  CYC(b_+61, b_+62); L = A;
  CYC(b_+62, b_+63); H = mem_rd(gb, HL);
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL);
  CYC(b_+66, b_+68); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto L_671c; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_671c:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x0e);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+78, b_+80); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+80, b_+81); L = alu_inc8(gb, L);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x04);
  CYC(b_+83, b_+85); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x19);
  CYC(b_+87, b_+89); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL);
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+92, b_+93); mem_wr(gb, HL, A);
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x50);
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x96);
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+105);
  TAIL(enemySetAnimation);
normalStatus:
  CALL_C(b_+105, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+108);
  if (!(F & FC)) { CYCT(b_+108, b_+110); goto L_6754; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+111); push_effect(gb, b_+111);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+148) goto state0;
    if (jt_ == b_+168) goto state1;
    if (jt_ == b_+212) goto stateStub;
    HANDOFF(HL);
  } while (0);
L_6754:
  CYC(b_+127, b_+128); B = alu_dec8(gb, B);
  CYC(b_+128, b_+129); A = B;
  CYC(b_+129, b_+130); push_effect(gb, b_+130);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+213) goto subid1;
    if (jt_ == b_+505) goto subid2;
    if (jt_ == b_+1152) goto subid3;
    if (jt_ == b_+1189) goto subid4;
    if (jt_ == b_+1299) goto subid6;
    if (jt_ == b_+1366) goto subid8;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+148, b_+149); A = B;
  CYC(b_+149, b_+150); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+150, b_+153); goto L_6774; }
  CYC(b_+150, b_+153);
  CALL_C(b_+153, s_ecom_setSpeedAndState8AndVisible, SYM(ecom_setSpeedAndState8AndVisible_b0d), b_+156);
  CYC(b_+156, b_+159);
  TAIL_S(func_6c6b);
L_6774:
  CYC(b_+159, b_+160); A = alu_inc8(gb, A);
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  CYC(b_+161, b_+163); A = 0x06;
  CYC(b_+163, b_+165); B = 0x87;
  CALL_C(b_+165, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+168);
state1:
  CYC(b_+168, b_+170); B = 0x09;
  CALL_C(b_+170, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+173);
  if (!(F & FZ)) { RET_TAKEN(b_+173); return; }
  CYC(b_+173, b_+174);
  CYC(b_+174, b_+176); B = 0x06;
  CALL_C(b_+176, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+179);
  CYC(b_+179, b_+181); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+181, b_+182); E = L;
  CYC(b_+182, b_+183); A = mem_rd(gb, DE);
  CYC(b_+183, b_+184); mem_wr(gb, HL, A);
  CYC(b_+184, b_+186); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+186, b_+187); C = H;
  CYC(b_+187, b_+189); E = 0x08;
L_6792:
  CYC(b_+189, b_+190); push_effect(gb, HL);
  CALL_C(b_+190, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+193);
  CYC(b_+193, b_+195); A = 0x0a;
  CYC(b_+195, b_+196); alu_sub(gb, E);
  CYC(b_+196, b_+197); mem_wr(gb, HL, A);
  CYC(b_+197, b_+199); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+199, b_+201); A = 0x80;
  CYC(b_+201, b_+202); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+202, b_+203); mem_wr(gb, HL, C);
  CYC(b_+203, b_+204); A = H;
  CYC(b_+204, b_+205); SET_HL(pop_effect(gb));
  CYC(b_+205, b_+206); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+206, b_+207); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+207, b_+209); goto L_6792; }
  CYC(b_+207, b_+209);
  CYC(b_+209, b_+212);
  TAIL(enemyDelete);
stateStub:
  RET(b_+212); return;
subid1:
  CYC(b_+213, b_+214); A = mem_rd(gb, DE);
  CYC(b_+214, b_+216); alu_sub(gb, 0x08);
  CYC(b_+216, b_+217); push_effect(gb, b_+217);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+235) goto subid1_state8;
    if (jt_ == b_+251) goto subid1_state9;
    if (jt_ == b_+311) goto subid1_stateA;
    if (jt_ == b_+339) goto subid1_stateB;
    if (jt_ == b_+362) goto subid1_stateC;
    if (jt_ == b_+385) goto subid1_stateD;
    if (jt_ == b_+411) goto subid1_stateE;
    if (jt_ == b_+446) goto subid1_stateF;
    if (jt_ == b_+485) goto subid1_stateG;
    HANDOFF(HL);
  } while (0);
subid1_state8:
  CYC(b_+235, b_+238); A = mem_rd(gb, wcc93);
  CYC(b_+238, b_+239); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+239); return; }
  CYC(b_+239, b_+240);
  CYC(b_+240, b_+241); H = D;
  CYC(b_+241, b_+242); L = E;
  CYC(b_+242, b_+243); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+243, b_+245); A = 0x2e;
  CYC(b_+245, b_+248); mem_wr(gb, wActiveMusic, A);
  CYC(b_+248, b_+251);
  TAIL(playSound_b00);
subid1_state9:
  CYC(b_+251, b_+253); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+253, b_+254); A = mem_rd(gb, DE);
  CYC(b_+254, b_+256); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+256, b_+258); goto subid1_animate; }
  CYC(b_+256, b_+258);
  CYC(b_+258, b_+260); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(b_+260, b_+262); goto subid1_animate; }
  CYC(b_+260, b_+262);
  CYC(b_+262, b_+263); H = D;
  CYC(b_+263, b_+265); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+265, b_+266); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+266, b_+268); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+268, b_+270); mem_wr(gb, HL, 0x3c);
  CYC(b_+270, b_+272); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+272, b_+273); A = mem_rd(gb, DE);
  CYC(b_+273, b_+274); H = A;
  CYC(b_+274, b_+276); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+276, b_+277); alu_xor(gb, A);
  CYC(b_+277, b_+278); mem_wr(gb, HL, A);
  CYC(b_+278, b_+280); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+280, b_+281); mem_wr(gb, HL, A);
  CYC(b_+281, b_+282); E = alu_inc8(gb, E);
  CYC(b_+282, b_+283); A = mem_rd(gb, DE);
  CYC(b_+283, b_+284); H = A;
  CYC(b_+284, b_+285); alu_xor(gb, A);
  CYC(b_+285, b_+286); mem_wr(gb, HL, A);
  CYC(b_+286, b_+288); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+288, b_+289); mem_wr(gb, HL, A);
  CYC(b_+289, b_+292); SET_HL(wRoomCollisions + 0x16);
  CYC(b_+292, b_+293); alu_xor(gb, A);
  CYC(b_+293, b_+294); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+294, b_+295); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+295, b_+296); mem_wr(gb, HL, A);
  CYC(b_+296, b_+298); L = 0x26;
  CYC(b_+298, b_+299); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+299, b_+300); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+300, b_+301); mem_wr(gb, HL, A);
  CYC(b_+301, b_+303); A = 0x67;
  CALL_C(b_+303, s_playSound, SYM(playSound_b00), b_+306);
  CYC(b_+306, b_+308); A = 0xf0;
  CYC(b_+308, b_+311);
  TAIL(playSound_b00);
subid1_stateA:
  CALL_C(b_+311, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+314);
  if (!(F & FZ)) { CYCT(b_+314, b_+317); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+314, b_+317);
  CYC(b_+317, b_+320); SET_BC(0x020c);
  CALL_C(b_+320, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+323);
  if (!(F & FZ)) { CYCT(b_+323, b_+326); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+323, b_+326);
  CYC(b_+326, b_+327); H = D;
  CYC(b_+327, b_+329); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+329, b_+330); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+330, b_+332); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+332, b_+334); mem_wr(gb, HL, 0x1e);
  CYC(b_+334, b_+336); A = 0x04;
  CALL_C(b_+336, s_enemySetAnimation, SYM(enemySetAnimation), b_+339);
subid1_stateB:
  CALL_C(b_+339, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+342);
  if (!(F & FZ)) { CYCT(b_+342, b_+345); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+342, b_+345);
  CYC(b_+345, b_+346); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+346, b_+347); L = E;
  CYC(b_+347, b_+348); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+348, b_+350); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+350, b_+352); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+352, b_+354); A = 0x2e;
  CYC(b_+354, b_+357); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+357, s_playSound, SYM(playSound_b00), b_+360);
  CYC(b_+360, b_+362); E = ENEMY_BASE + OBJ_STATE;
subid1_stateC:
  CALL_C(b_+362, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+365);
  if (!(F & FZ)) { CYCT(b_+365, b_+367); goto L_684f; }
  CYC(b_+365, b_+367);
  CYC(b_+367, b_+368); L = E;
  CYC(b_+368, b_+369); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+369, b_+372); SET_BC(0xfdc0);
  CALL_C(b_+372, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+375);
  CYC(b_+375, b_+378);
  TAIL(objectSetVisible81);
L_684f:
  CYC(b_+378, b_+379); A = mem_rd(gb, HL);
  CYC(b_+379, b_+381); alu_cp(gb, 0x0a);
  if (F & FC) { RET_TAKEN(b_+381); return; }
  CYC(b_+381, b_+382);
subid1_animate:
  CYC(b_+382, b_+385);
  TAIL(enemyAnimate);
subid1_stateD:
  CYC(b_+385, b_+387); C = 0x20;
  CALL_C(b_+387, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+390);
  if (!(F & FZ)) { RET_TAKEN(b_+390); return; }
  CYC(b_+390, b_+391);
  CYC(b_+391, b_+393); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+393, b_+394); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+394, b_+396); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+396, b_+398); mem_wr(gb, HL, 0x96);
  CYC(b_+398, b_+400); A = 0x78;
  CALL_C(b_+400, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+403);
  CALL_C(b_+403, s_objectSetVisible82, SYM(objectSetVisible82), b_+406);
  CYC(b_+406, b_+408); A = 0x81;
  CYC(b_+408, b_+411);
  TAIL(playSound_b00);
subid1_stateE:
  CALL_C(b_+411, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+414);
  if (F & FZ) { CYCT(b_+414, b_+416); goto L_6888; }
  CYC(b_+414, b_+416);
  CYC(b_+416, b_+417); A = mem_rd(gb, HL);
  CYC(b_+417, b_+419); alu_cp(gb, 0x87);
  if (F & FC) { CYCT(b_+419, b_+421); goto subid1_animate; }
  CYC(b_+419, b_+421);
  CYC(b_+421, b_+424); A = mem_rd(gb, w1Link_zh);
  CYC(b_+424, b_+425); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+425); return; }
  CYC(b_+425, b_+426);
  CYC(b_+426, b_+429); SET_HL(wLinkForceState);
  CYC(b_+429, b_+431); A = 0x14;
  CYC(b_+431, b_+432); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+432, b_+434); mem_wr(gb, HL, 0x00);
  RET(b_+434); return;
L_6888:
  CYC(b_+435, b_+436); L = E;
  CYC(b_+436, b_+437); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+437, b_+439); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+439, b_+441); mem_wr(gb, HL, 0x50);
  CALL_C(b_+441, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+444);
  CYC(b_+444, b_+446);
  goto subid1_animate;
subid1_stateF:
  CYC(b_+446, b_+448); A = 0x01;
  CALL_C(b_+448, s_ecom_getSideviewAdjacentWallsBitset, SYM(ecom_getSideviewAdjacentWallsBitset_b0d), b_+451);
  if (!(F & FZ)) { CYCT(b_+451, b_+453); goto L_689f; }
  CYC(b_+451, b_+453);
  CALL_C(b_+453, s_objectApplySpeed, SYM(objectApplySpeed), b_+456);
  CYC(b_+456, b_+458);
  goto subid1_animate;
L_689f:
  CYC(b_+458, b_+460); A = 0x28;
  CALL_C(b_+460, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+463);
  CYC(b_+463, b_+464); H = D;
  CYC(b_+464, b_+466); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+466, b_+467); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+467, b_+469); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+469, b_+471); mem_wr(gb, HL, 0x14);
  CYC(b_+471, b_+473); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+473, b_+474); A = mem_rd(gb, HL);
  CYC(b_+474, b_+476); alu_xor(gb, 0x10);
  CYC(b_+476, b_+477); mem_wr(gb, HL, A);
  CYC(b_+477, b_+480); SET_BC(0xfe80);
  CALL_C(b_+480, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+483);
  CYC(b_+483, b_+485);
  goto subid1_animate;
subid1_stateG:
  CALL_C(b_+485, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+488);
  CYC(b_+488, b_+490); C = 0x20;
  CALL_C(b_+490, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+493);
  if (!(F & FZ)) { CYCT(b_+493, b_+495); goto subid1_animate; }
  CYC(b_+493, b_+495);
  CYC(b_+495, b_+497); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+497, b_+499); mem_wr(gb, HL, 0x0c);
  CYC(b_+499, b_+501); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+501, b_+503); mem_wr(gb, HL, 0x3c);
  CYC(b_+503, b_+505);
  goto subid1_animate;
subid2:
  CYC(b_+505, b_+506); A = mem_rd(gb, DE);
  CYC(b_+506, b_+508); alu_sub(gb, 0x08);
  CYC(b_+508, b_+509); push_effect(gb, b_+509);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+529) goto subid2_state8;
    if (jt_ == b_+549) goto subid2_incStateWhenCounter1Is0;
    if (jt_ == b_+558) goto subid2_stateA;
    if (jt_ == b_+644) goto subid2_stateB;
    if (jt_ == b_+978) goto subid2_stateC;
    if (jt_ == b_+1004) goto subid2_stateD;
    if (jt_ == b_+1013) goto subid2_stateE;
    if (jt_ == b_+1036) goto subid2_stateF;
    if (jt_ == b_+1115) goto subid2_stateH;
    HANDOFF(HL);
  } while (0);
subid2_state8:
  CYC(b_+529, b_+530); H = D;
  CYC(b_+530, b_+532); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+532, b_+534); mem_wr(gb, HL, 0x14);
subid2_incStateEnableCollisionsSetCounterAndSpeed:
  CYC(b_+534, b_+535); L = E;
  CYC(b_+535, b_+536); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+536, b_+538); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+538, b_+540); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+540, b_+542); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+542, b_+544); mem_wr(gb, HL, 0x3c);
  CYC(b_+544, b_+546); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+546, b_+548); mem_wr(gb, HL, 0x14);
  RET(b_+548); return;
subid2_incStateWhenCounter1Is0:
  CALL_C(b_+549, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+552);
  if (!(F & FZ)) { CYCT(b_+552, b_+555); TAIL(objectApplySpeed); }
  CYC(b_+552, b_+555);
  CYC(b_+555, b_+556); L = E;
  CYC(b_+556, b_+557); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+557); return;
subid2_stateA:
  CYC(b_+558, b_+560); B = 0x04;
  CYC(b_+560, b_+562); A = 0x38;
  CALL_C(b_+562, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+565);
  CYC(b_+565, b_+566); A = mem_rd(gb, HL);
  CYC(b_+566, b_+567); alu_and(gb, B);
  CYC(b_+567, b_+569); C = 0x03;
  CYC(b_+569, b_+571); L = ENEMY_BASE + OBJ_VAR38;
  if (!(F & FZ)) { CYCT(b_+571, b_+573); goto L_692f; }
  CYC(b_+571, b_+573);
  CYC(b_+573, b_+575); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+575, b_+577); goto L_6935; }
  CYC(b_+575, b_+577);
  CYC(b_+577, b_+579); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+579, b_+580); A = mem_rd(gb, DE);
  CYC(b_+580, b_+582); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+582, b_+584); goto L_692f; }
  CYC(b_+582, b_+584);
  CYC(b_+584, b_+585); B = H;
  CYC(b_+585, b_+587); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+587, b_+588); H = mem_rd(gb, HL);
  CYC(b_+588, b_+590); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+590, b_+591); A = mem_rd(gb, HL);
  CYC(b_+591, b_+593); alu_cp(gb, 0x10);
  CYC(b_+593, b_+594); H = B;
  if (!(F & FC)) { CYCT(b_+594, b_+596); goto L_692f; }
  CYC(b_+594, b_+596);
  CYC(b_+596, b_+598); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+598, b_+600); alu_cp(gb, 0x78);
  if (!(F & FC)) { CYCT(b_+600, b_+602); goto L_6935; }
  CYC(b_+600, b_+602);
L_692f:
  CYC(b_+602, b_+604); L = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+604, b_+606); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+606, b_+608); C = 0x00;
L_6935:
  CYC(b_+608, b_+610); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+610, b_+612); alu_cp(gb, 0x58);
  CYC(b_+612, b_+614); B = 0x00;
  if (F & FC) { CYCT(b_+614, b_+616); goto L_694c; }
  CYC(b_+614, b_+616);
  CYC(b_+616, b_+618); B = 0x02;
  CYC(b_+618, b_+620); alu_sub(gb, 0x70);
  CYC(b_+620, b_+622); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+622, b_+624); goto L_694c; }
  CYC(b_+622, b_+624);
  CALL_C(b_+624, s_getRandomNumber, SYM(getRandomNumber), b_+627);
  CYC(b_+627, b_+629); alu_and(gb, 0x01);
  CYC(b_+629, b_+630); A = alu_inc8(gb, A);
  CYC(b_+630, b_+631); B = A;
L_694c:
  CYC(b_+631, b_+632); H = D;
  CYC(b_+632, b_+634); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+634, b_+635); A = C;
  CYC(b_+635, b_+636); alu_add(gb, B);
  CYC(b_+636, b_+637); mem_wr(gb, HL, A);
  CYC(b_+637, b_+639); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+639, b_+640); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+640, b_+641); L = alu_inc8(gb, L);
  CYC(b_+641, b_+643); mem_wr(gb, HL, 0x00);
  RET(b_+643); return;
subid2_stateB:
  CYC(b_+644, b_+646); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+646, b_+647); A = mem_rd(gb, DE);
  CYC(b_+647, b_+649); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+649, b_+650); push_effect(gb, b_+650);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+662) goto subid2_stateB_var03_00;
    if (jt_ == b_+781) goto subid2_stateB_var03_01;
    if (jt_ == b_+903) goto subid2_stateB_var03_02;
    if (jt_ == b_+954) goto subid2_stateB_var03_03;
    if (jt_ == b_+961) goto subid2_stateB_var03_04;
    HANDOFF(HL);
  } while (0);
subid2_stateB_var03_00:
  CYC(b_+662, b_+663); A = mem_rd(gb, DE);
  CYC(b_+663, b_+664); push_effect(gb, b_+664);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+670) goto subid2_stateB_var03_00_substate0;
    if (jt_ == b_+706) goto subid2_stateB_var03_00_substate1;
    if (jt_ == b_+717) goto subid2_stateB_var03_00_substate2;
    HANDOFF(HL);
  } while (0);
subid2_stateB_var03_00_substate0:
  CYC(b_+670, b_+673); SET_BC(0x3a60);
  CYC(b_+673, b_+674); H = D;
  CYC(b_+674, b_+676); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+676, b_+677); A = mem_rd(gb, HL);
  CYC(b_+677, b_+679); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+679, b_+681); goto L_6980; }
  CYC(b_+679, b_+681);
  CYC(b_+681, b_+683); C = 0x90;
L_6980:
  CYC(b_+683, b_+685); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+685, b_+686); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+686, b_+688); mem_wr(gb, hFF8F, A);
  CYC(b_+688, b_+689); L = alu_inc8(gb, L);
  CYC(b_+689, b_+690); A = mem_rd(gb, HL);
  CYC(b_+690, b_+692); mem_wr(gb, hFF8E, A);
  CYC(b_+692, b_+693); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+693, b_+695); goto L_6991; }
  CYC(b_+693, b_+695);
  CYC(b_+695, b_+697); A = mem_rd(gb, hFF8F);
  CYC(b_+697, b_+698); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+698, b_+700); goto L_6994; }
  CYC(b_+698, b_+700);
L_6991:
  CYC(b_+700, b_+703);
  TAIL(ecom_moveTowardPosition_b0d);
L_6994:
  CYC(b_+703, b_+704); L = E;
  CYC(b_+704, b_+705); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+705); return;
subid2_stateB_var03_00_substate1:
  CYC(b_+706, b_+707); H = D;
  CYC(b_+707, b_+708); L = E;
  CYC(b_+708, b_+709); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+709, b_+710); L = alu_inc8(gb, L);
  CYC(b_+710, b_+712); mem_wr(gb, HL, 0x1e);
  CYC(b_+712, b_+714); A = 0x01;
  CYC(b_+714, b_+717);
  TAIL(enemySetAnimation);
subid2_stateB_var03_00_substate2:
  CALL_C(b_+717, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+720);
  if (F & FZ) { CYCT(b_+720, b_+722); goto L_69b6; }
  CYC(b_+720, b_+722);
  CYC(b_+722, b_+723); A = mem_rd(gb, HL);
  CYC(b_+723, b_+725); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+725); return; }
  CYC(b_+725, b_+726);
  CYC(b_+726, b_+728); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+728, b_+729); A = mem_rd(gb, HL);
  CYC(b_+729, b_+731); alu_sub(gb, 0x04);
  CYC(b_+731, b_+732); mem_wr(gb, HL, A);
  CYC(b_+732, b_+734); B = 0x43;
  CYC(b_+734, b_+737);
  TAIL(ecom_spawnProjectile_b0d);
L_69b6:
  CYC(b_+737, b_+739); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+739, b_+740); A = mem_rd(gb, HL);
  CYC(b_+740, b_+742); alu_add(gb, 0x04);
  CYC(b_+742, b_+743); mem_wr(gb, HL, A);
subid2_stateB_func_69bc:
  CYC(b_+743, b_+745); A = 0x38;
  CALL_C(b_+745, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+748);
  CYC(b_+748, b_+750); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(b_+750, b_+752); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+752, b_+753); A = mem_rd(gb, DE);
  CYC(b_+753, b_+755); alu_sub(gb, 0x02);
  CYC(b_+755, b_+757); alu_xor(gb, 0x01);
  CYC(b_+757, b_+759); alu_add(gb, 0xb0);
  CYC(b_+759, b_+760); L = A;
  CYC(b_+760, b_+761); H = mem_rd(gb, HL);
  CYC(b_+761, b_+763); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+763, b_+764); A = mem_rd(gb, HL);
  CYC(b_+764, b_+766); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+766, b_+768); goto L_69d6; }
  CYC(b_+766, b_+768);
  CYC(b_+768, b_+769); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_69d6:
  CYC(b_+769, b_+770); H = D;
  CYC(b_+770, b_+771); E = L;
  CYC(b_+771, b_+772); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+772, b_+774); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+774, b_+775); A = mem_rd(gb, HL);
  CYC(b_+775, b_+777); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+777); return; }
  CYC(b_+777, b_+778);
  CYC(b_+778, b_+781);
  goto subid2_stateC;
subid2_stateB_var03_01:
  CYC(b_+781, b_+782); A = mem_rd(gb, DE);
  CYC(b_+782, b_+783); push_effect(gb, b_+783);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+789) goto subid2_stateB_var03_01_substate0;
    if (jt_ == b_+801) goto subid2_stateB_var03_01_substate1;
    if (jt_ == b_+818) goto subid2_stateB_var03_01_substate2;
    HANDOFF(HL);
  } while (0);
subid2_stateB_var03_01_substate0:
  CYC(b_+789, b_+790); H = D;
  CYC(b_+790, b_+791); L = E;
  CYC(b_+791, b_+792); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+792, b_+794); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+794, b_+796); mem_wr(gb, HL, 0x28);
  CYC(b_+796, b_+798); A = 0x01;
  CYC(b_+798, b_+801);
  TAIL(enemySetAnimation);
subid2_stateB_var03_01_substate1:
  CALL_C(b_+801, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+804);
  if (!(F & FZ)) { RET_TAKEN(b_+804); return; }
  CYC(b_+804, b_+805);
  CYC(b_+805, b_+807); mem_wr(gb, HL, 0x41);
  CYC(b_+807, b_+808); L = E;
  CYC(b_+808, b_+809); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+809, b_+811); L = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+811, b_+813); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+813, b_+814); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+814, b_+816); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+816, b_+817); mem_wr(gb, HL, A);
  RET(b_+817); return;
subid2_stateB_var03_01_substate2:
  CALL_C(b_+818, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+821);
  if (F & FZ) { CYCT(b_+821, b_+823); goto subid2_stateB_func_69bc; }
  CYC(b_+821, b_+823);
  CYC(b_+823, b_+824); A = mem_rd(gb, HL);
  CYC(b_+824, b_+826); alu_and(gb, 0x0f);
  if (F & FZ) { CYCT(b_+826, b_+828); goto L_6a1b; }
  CYC(b_+826, b_+828);
  CYC(b_+828, b_+830); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+830); return; }
  CYC(b_+830, b_+831);
  CYC(b_+831, b_+833); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+833, b_+834); A = mem_rd(gb, HL);
  CYC(b_+834, b_+836); alu_add(gb, 0x02);
  CYC(b_+836, b_+837); mem_wr(gb, HL, A);
  RET(b_+837); return;
L_6a1b:
  CYC(b_+838, b_+840); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+840, b_+841); A = mem_rd(gb, HL);
  CYC(b_+841, b_+843); alu_sub(gb, 0x02);
  CYC(b_+843, b_+844); mem_wr(gb, HL, A);
  CALL_C(b_+844, s_getFreePartSlot, SYM(getFreePartSlot), b_+847);
  if (!(F & FZ)) { RET_TAKEN(b_+847); return; }
  CYC(b_+847, b_+848);
  CYC(b_+848, b_+850); mem_wr(gb, HL, 0x43);
  CYC(b_+850, b_+851); L = alu_inc8(gb, L);
  CYC(b_+851, b_+852); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+852, b_+854); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+854, b_+855); A = mem_rd(gb, DE);
  CYC(b_+855, b_+857); alu_and(gb, 0x30);
  CYC(b_+857, b_+859); A = alu_swap(gb, A);
  CYC(b_+859, b_+862); SET_BC(b_+895 /* @subid2@stateB@var03_01@table_6a54 */);
  CALL_C(b_+862, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+865);
  CYC(b_+865, b_+867); E = ENEMY_BASE + OBJ_VAR39;
  CYC(b_+867, b_+868); A = mem_rd(gb, DE);
  CYC(b_+868, b_+869); E = A;
  CYC(b_+869, b_+870); A = mem_rd(gb, BC);
  CYC(b_+870, b_+871); alu_add(gb, E);
  CYC(b_+871, b_+873); L = PART_BASE + OBJ_YH;
  CYC(b_+873, b_+874); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+874, b_+875); L = alu_inc8(gb, L);
  CYC(b_+875, b_+876); SET_BC(BC + 1);
  CYC(b_+876, b_+878); E = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+878, b_+879); A = mem_rd(gb, DE);
  CYC(b_+879, b_+880); E = A;
  CYC(b_+880, b_+881); A = mem_rd(gb, BC);
  CYC(b_+881, b_+882); alu_add(gb, E);
  CYC(b_+882, b_+883); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(b_+883, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+886);
  if (!(F & FZ)) { RET_TAKEN(b_+886); return; }
  CYC(b_+886, b_+887);
  CYC(b_+887, b_+889); mem_wr(gb, HL, 0x05);
  CYC(b_+889, b_+892); SET_BC(0x0800);
  CYC(b_+892, b_+895);
  TAIL(objectCopyPositionWithOffset);

subid2_stateB_var03_02:
  CYC(b_+903, b_+904); A = mem_rd(gb, DE);
  CYC(b_+904, b_+905); push_effect(gb, b_+905);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+911) goto subid2_stateB_var03_02_substate0;
    if (jt_ == b_+925) goto subid2_stateB_var03_02_substate1;
    if (jt_ == b_+932) goto subid2_stateB_var03_02_substate2;
    HANDOFF(HL);
  } while (0);
subid2_stateB_var03_02_substate0:
  CYC(b_+911, b_+912); H = D;
  CYC(b_+912, b_+913); L = E;
  CYC(b_+913, b_+914); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+914, b_+915); L = alu_inc8(gb, L);
  CYC(b_+915, b_+917); mem_wr(gb, HL, 0x08);
  CYC(b_+917, b_+918); L = alu_inc8(gb, L);
  CYC(b_+918, b_+920); mem_wr(gb, HL, 0x02);
  CYC(b_+920, b_+922); A = 0x01;
  CYC(b_+922, b_+925);
  TAIL(enemySetAnimation);
subid2_stateB_var03_02_substate1:
  CALL_C(b_+925, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+928);
  if (!(F & FZ)) { RET_TAKEN(b_+928); return; }
  CYC(b_+928, b_+929);
  CYC(b_+929, b_+930); L = E;
  CYC(b_+930, b_+931); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+931); return;
subid2_stateB_var03_02_substate2:
  CYC(b_+932, b_+934); B = 0x43;
  CALL_C(b_+934, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+937);
  if (!(F & FZ)) { RET_TAKEN(b_+937); return; }
  CYC(b_+937, b_+938);
  CYC(b_+938, b_+940); L = PART_BASE + OBJ_SUBID;
  CYC(b_+940, b_+942); mem_wr(gb, HL, 0x02);
  CALL_C(b_+942, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+945);
  if (F & FZ) { CYCT(b_+945, b_+948); goto subid2_stateB_func_69bc; }
  CYC(b_+945, b_+948);
  CYC(b_+948, b_+949); L = alu_dec8(gb, L);
  CYC(b_+949, b_+951); mem_wr(gb, HL, 0x14);
  CYC(b_+951, b_+952); L = alu_dec8(gb, L);
  CYC(b_+952, b_+953); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+953); return;
subid2_stateB_var03_03:
  CYC(b_+954, b_+955); A = mem_rd(gb, DE);
  CYC(b_+955, b_+956); push_effect(gb, b_+956);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+670) goto subid2_stateB_var03_00_substate0;
    HANDOFF(HL);
  } while (0);
subid2_stateB_var03_04:
  CALL_L(b_+961, gleeok_subid2_stateB_func_6a9f, b_+964);
  if (F & FZ) CALL_C_CC(b_+964, s_func_6cf6_hook, SYM(func_6cf6), b_+967);
  else CYC(b_+964, b_+967);
  CYC(b_+967, b_+970);
  TAIL(objectApplySpeed);

subid2_stateC:
  CYC(b_+978, b_+979); H = D;
  CYC(b_+979, b_+980); L = E;
  CYC(b_+980, b_+981); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+981, b_+983); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+983, b_+985); mem_wr(gb, HL, 0x78);
  CYC(b_+985, b_+987); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+987, b_+988); A = mem_rd(gb, HL);
  CYC(b_+988, b_+989); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+989, b_+991); goto L_6ab8; }
  CYC(b_+989, b_+991);
  CYC(b_+991, b_+993); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+993, b_+995); goto L_6abd; }
  CYC(b_+993, b_+995);
L_6ab8:
  CYC(b_+995, b_+997); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+997, b_+998); alu_xor(gb, A);
  CYC(b_+998, b_+999); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+999, b_+1000); mem_wr(gb, HL, A);
L_6abd:
  CYC(b_+1000, b_+1001); alu_xor(gb, A);
  CYC(b_+1001, b_+1004);
  TAIL(enemySetAnimation);
subid2_stateD:
  CALL_C(b_+1004, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+1007);
  if (!(F & FZ)) { CYCT(b_+1007, b_+1009); goto subid2_stateB_var03_04; }
  CYC(b_+1007, b_+1009);
  CYC(b_+1009, b_+1010); L = E;
  CYC(b_+1010, b_+1012); mem_wr(gb, HL, 0x0a);
  RET(b_+1012); return;
subid2_stateE:
  CYC(b_+1013, b_+1016); A = mem_rd(gb, wFrameCounter);
  CYC(b_+1016, b_+1017); alu_rrca(gb);
  if (F & FC) { CYCT(b_+1017, b_+1019); goto L_6adb; }
  CYC(b_+1017, b_+1019);
  CALL_C(b_+1019, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+1022);
  if (!(F & FZ)) { CYCT(b_+1022, b_+1024); goto L_6adb; }
  CYC(b_+1022, b_+1024);
  CYC(b_+1024, b_+1025); L = E;
  CYC(b_+1025, b_+1026); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+1026, b_+1028); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+1028, b_+1030); mem_wr(gb, HL, 0x28);
L_6adb:
  CALL_C(b_+1030, s_objectApplySpeed, SYM(objectApplySpeed), b_+1033);
  CYC(b_+1033, b_+1036);
  TAIL(ecom_bounceOffScreenBoundary_b0d);
subid2_stateF:
  CYC(b_+1036, b_+1037); H = D;
  CYC(b_+1037, b_+1039); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1039, b_+1040); A = mem_rd(gb, HL);
  CYC(b_+1040, b_+1042); alu_cp(gb, 0x02);
  CYC(b_+1042, b_+1045); SET_BC(0x2476);
  if (F & FZ) { CYCT(b_+1045, b_+1047); goto L_6aee; }
  CYC(b_+1045, b_+1047);
  CYC(b_+1047, b_+1049); C = 0x7a;
L_6aee:
  CYC(b_+1049, b_+1051); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+1051, b_+1052); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1052, b_+1054); mem_wr(gb, hFF8F, A);
  CYC(b_+1054, b_+1055); L = alu_inc8(gb, L);
  CYC(b_+1055, b_+1056); A = mem_rd(gb, HL);
  CYC(b_+1056, b_+1058); mem_wr(gb, hFF8E, A);
  CYC(b_+1058, b_+1059); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+1059, b_+1061); goto L_6aff; }
  CYC(b_+1059, b_+1061);
  CYC(b_+1061, b_+1063); A = mem_rd(gb, hFF8F);
  CYC(b_+1063, b_+1064); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+1064, b_+1066); goto L_6b02; }
  CYC(b_+1064, b_+1066);
L_6aff:
  CYC(b_+1066, b_+1069);
  TAIL(ecom_moveTowardPosition_b0d);
L_6b02:
  CYC(b_+1069, b_+1070); L = E;
  CYC(b_+1070, b_+1071); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+1071, b_+1073); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+1073, b_+1075); mem_wr(gb, HL, 0x0d);
  CYC(b_+1075, b_+1077); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+1077, b_+1079); mem_wr(gb, HL, 0x14);
  CYC(b_+1079, b_+1081); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+1081, b_+1083); mem_wr(gb, HL, 0x3c);
  CYC(b_+1083, b_+1085); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+1085, b_+1086); alu_xor(gb, A);
  CYC(b_+1086, b_+1087); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1087, b_+1088); mem_wr(gb, HL, A);
  CYC(b_+1088, b_+1090); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1090, b_+1091); A = mem_rd(gb, HL);
  CYC(b_+1091, b_+1093); alu_cp(gb, 0x02);
  CYC(b_+1093, b_+1095); A = 0x14;
  CYC(b_+1095, b_+1097); B = 0x02;
  if (F & FZ) { CYCT(b_+1097, b_+1099); goto L_6b24; }
  CYC(b_+1097, b_+1099);
  CYC(b_+1099, b_+1101); A = 0x0c;
  CYC(b_+1101, b_+1103); B = 0x04;
L_6b24:
  CYC(b_+1103, b_+1105); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+1105, b_+1106); mem_wr(gb, HL, A);
  CYC(b_+1106, b_+1108); A = 0x38;
  CALL_C(b_+1108, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+1111);
  CYC(b_+1111, b_+1112); A = mem_rd(gb, HL);
  CYC(b_+1112, b_+1113); alu_xor(gb, B);
  CYC(b_+1113, b_+1114); mem_wr(gb, HL, A);
  RET(b_+1114); return;
subid2_stateH:
  CYC(b_+1115, b_+1117); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1117, b_+1118); A = mem_rd(gb, DE);
  CYC(b_+1118, b_+1120); alu_sub(gb, 0x02);
  CYC(b_+1120, b_+1122); alu_xor(gb, 0x01);
  CYC(b_+1122, b_+1124); alu_add(gb, 0x30);
  CALL_C(b_+1124, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+1127);
  CYC(b_+1127, b_+1128); H = mem_rd(gb, HL);
  CYC(b_+1128, b_+1130); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+1130, b_+1131); A = mem_rd(gb, HL);
  CYC(b_+1131, b_+1133); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(b_+1133, b_+1135); goto L_6b49; }
  CYC(b_+1133, b_+1135);
  CYC(b_+1135, b_+1137); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+1137, b_+1140); goto subid2_stateB_var03_04; }
  CYC(b_+1137, b_+1140);
L_6b49:
  CYC(b_+1140, b_+1141); H = D;
  CYC(b_+1141, b_+1143); mem_wr(gb, HL, 0x0a);
  CYC(b_+1143, b_+1145); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1145, b_+1146); A = mem_rd(gb, HL);
  CYC(b_+1146, b_+1148); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+1148); return; }
  CYC(b_+1148, b_+1149);
  CYC(b_+1149, b_+1152);
  goto subid2_stateA;
subid3:
  CYC(b_+1152, b_+1153); A = mem_rd(gb, DE);
  CYC(b_+1153, b_+1155); alu_sub(gb, 0x08);
  CYC(b_+1155, b_+1156); push_effect(gb, b_+1156);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+1176) goto subid3_state8;
    if (jt_ == b_+549) goto subid2_incStateWhenCounter1Is0;
    HANDOFF(HL);
  } while (0);
subid3_state8:
  CYC(b_+1176, b_+1177); H = D;
  CYC(b_+1177, b_+1179); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+1179, b_+1181); mem_wr(gb, HL, 0x0c);
  CYC(b_+1181, b_+1184);
  goto subid2_incStateEnableCollisionsSetCounterAndSpeed;

subid4:
  CYC(b_+1189, b_+1190); A = mem_rd(gb, DE);
  CYC(b_+1190, b_+1192); alu_sub(gb, 0x08);
  CYC(b_+1192, b_+1193); push_effect(gb, b_+1193);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+1199) goto subid5_state8;
    if (jt_ == b_+1224) goto subid5_state9;
    if (jt_ == b_+1258) goto subid5_stateA;
    HANDOFF(HL);
  } while (0);
subid5_state8:
  CYC(b_+1199, b_+1200); H = D;
  CYC(b_+1200, b_+1201); L = E;
  CYC(b_+1201, b_+1202); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+1202, b_+1204); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+1204, b_+1206); mem_wr(gb, HL, 0x04);
  CYC(b_+1206, b_+1208); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1208, b_+1209); A = mem_rd(gb, DE);
  CYC(b_+1209, b_+1211); alu_sub(gb, 0x04);
  CYC(b_+1211, b_+1213); alu_add(gb, 0x30);
  CALL_C(b_+1213, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+1216);
  CYC(b_+1216, b_+1218); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+1218, b_+1219); A = mem_rd(gb, HL);
  CYC(b_+1219, b_+1220); mem_wr(gb, DE, A);
  CYC(b_+1220, b_+1221); E = alu_dec8(gb, E);
  CYC(b_+1221, b_+1223); A = 0x80;
  CYC(b_+1223, b_+1224); mem_wr(gb, DE, A);
subid5_state9:
  CALL_C(b_+1224, s_func_6cb2_hook, SYM(func_6cb2), b_+1227);
  CALL_C(b_+1227, s_func_6cbf_hook, SYM(func_6cbf), b_+1230);
  if (!(F & FZ)) { RET_TAKEN(b_+1230); return; }
  CYC(b_+1230, b_+1231);
  CYC(b_+1231, b_+1233); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+1233, b_+1234); A = B;
  CYC(b_+1234, b_+1235); alu_add(gb, A);
  CYC(b_+1235, b_+1236); alu_add(gb, B);
  CYC(b_+1236, b_+1238); alu_add(gb, 0x24);
  CYC(b_+1238, b_+1239); mem_wr(gb, DE, A);
  CYC(b_+1239, b_+1241); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1241, b_+1242); A = mem_rd(gb, DE);
  CYC(b_+1242, b_+1244); alu_cp(gb, 0x04);
  CYC(b_+1244, b_+1246); B = 0x76;
  if (F & FZ) { CYCT(b_+1246, b_+1248); goto L_6bb7; }
  CYC(b_+1246, b_+1248);
  CYC(b_+1248, b_+1250); B = 0x7a;
L_6bb7:
  CYC(b_+1250, b_+1251); A = C;
  CYC(b_+1251, b_+1252); alu_add(gb, A);
  CYC(b_+1252, b_+1253); alu_add(gb, C);
  CYC(b_+1253, b_+1254); alu_add(gb, B);
  CYC(b_+1254, b_+1256); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+1256, b_+1257); mem_wr(gb, DE, A);
  RET(b_+1257); return;
subid5_stateA:
  CALL_C(b_+1258, s_func_6cb2_hook, SYM(func_6cb2), b_+1261);
  CYC(b_+1261, b_+1263); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1263, b_+1264); A = mem_rd(gb, DE);
  CYC(b_+1264, b_+1265); alu_rrca(gb);
  CYC(b_+1265, b_+1268); SET_BC(0x0276);
  if (!(F & FC)) { CYCT(b_+1268, b_+1270); goto L_6bce; }
  CYC(b_+1268, b_+1270);
  CYC(b_+1270, b_+1273); SET_BC(0x047a);
L_6bce:
  CYC(b_+1273, b_+1275); A = 0x38;
  CALL_C(b_+1275, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+1278);
  CYC(b_+1278, b_+1279); A = mem_rd(gb, HL);
  CYC(b_+1279, b_+1280); alu_and(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+1280); return; }
  CYC(b_+1280, b_+1281);
  CYC(b_+1281, b_+1282); H = D;
  CYC(b_+1282, b_+1284); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+1284, b_+1285); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+1285, b_+1287); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+1287, b_+1289); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+1289, b_+1291); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+1291, b_+1293); mem_wr(gb, HL, 0x24);
  CYC(b_+1293, b_+1295); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+1295, b_+1296); mem_wr(gb, HL, C);
  CYC(b_+1296, b_+1299);
  TAIL(objectSetVisible82);
subid6:
  CYC(b_+1299, b_+1300); A = mem_rd(gb, DE);
  CYC(b_+1300, b_+1302); alu_sub(gb, 0x08);
  CYC(b_+1302, b_+1303); push_effect(gb, b_+1303);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+1309) goto subid7_state8;
    if (jt_ == b_+1334) goto subid7_state9;
    if (jt_ == b_+1258) goto subid5_stateA;
    HANDOFF(HL);
  } while (0);
subid7_state8:
  CYC(b_+1309, b_+1310); H = D;
  CYC(b_+1310, b_+1311); L = E;
  CYC(b_+1311, b_+1312); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+1312, b_+1314); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+1314, b_+1316); mem_wr(gb, HL, 0x04);
  CYC(b_+1316, b_+1318); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1318, b_+1319); A = mem_rd(gb, DE);
  CYC(b_+1319, b_+1321); alu_sub(gb, 0x06);
  CYC(b_+1321, b_+1323); alu_add(gb, 0x30);
  CALL_C(b_+1323, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+1326);
  CYC(b_+1326, b_+1328); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+1328, b_+1329); A = mem_rd(gb, HL);
  CYC(b_+1329, b_+1330); mem_wr(gb, DE, A);
  CYC(b_+1330, b_+1331); E = alu_dec8(gb, E);
  CYC(b_+1331, b_+1333); A = 0x80;
  CYC(b_+1333, b_+1334); mem_wr(gb, DE, A);
subid7_state9:
  CALL_C(b_+1334, s_func_6cb2_hook, SYM(func_6cb2), b_+1337);
  CALL_C(b_+1337, s_func_6cbf_hook, SYM(func_6cbf), b_+1340);
  if (!(F & FZ)) { RET_TAKEN(b_+1340); return; }
  CYC(b_+1340, b_+1341);
  CYC(b_+1341, b_+1343); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+1343, b_+1344); A = B;
  CYC(b_+1344, b_+1345); alu_add(gb, A);
  CYC(b_+1345, b_+1347); alu_add(gb, 0x24);
  CYC(b_+1347, b_+1348); mem_wr(gb, DE, A);
  CYC(b_+1348, b_+1350); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1350, b_+1351); A = mem_rd(gb, DE);
  CYC(b_+1351, b_+1353); alu_cp(gb, 0x06);
  CYC(b_+1353, b_+1355); B = 0x76;
  if (F & FZ) { CYCT(b_+1355, b_+1357); goto L_6c24; }
  CYC(b_+1355, b_+1357);
  CYC(b_+1357, b_+1359); B = 0x7a;
L_6c24:
  CYC(b_+1359, b_+1360); A = C;
  CYC(b_+1360, b_+1361); alu_add(gb, A);
  CYC(b_+1361, b_+1362); alu_add(gb, B);
  CYC(b_+1362, b_+1364); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+1364, b_+1365); mem_wr(gb, DE, A);
  RET(b_+1365); return;
subid8:
  CYC(b_+1366, b_+1367); A = mem_rd(gb, DE);
  CYC(b_+1367, b_+1369); alu_sub(gb, 0x08);
  CYC(b_+1369, b_+1370); push_effect(gb, b_+1370);
  do { uint16_t jt_ = (gleeok_jump_table(gb));
    if (jt_ == b_+1376) goto subid9_state8;
    if (jt_ == b_+1401) goto subid9_state9;
    if (jt_ == b_+1258) goto subid5_stateA;
    HANDOFF(HL);
  } while (0);
subid9_state8:
  CYC(b_+1376, b_+1377); H = D;
  CYC(b_+1377, b_+1378); L = E;
  CYC(b_+1378, b_+1379); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+1379, b_+1381); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+1381, b_+1383); mem_wr(gb, HL, 0x04);
  CYC(b_+1383, b_+1385); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1385, b_+1386); A = mem_rd(gb, DE);
  CYC(b_+1386, b_+1388); alu_sub(gb, 0x08);
  CYC(b_+1388, b_+1390); alu_add(gb, 0x30);
  CALL_C(b_+1390, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+1393);
  CYC(b_+1393, b_+1395); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+1395, b_+1396); A = mem_rd(gb, HL);
  CYC(b_+1396, b_+1397); mem_wr(gb, DE, A);
  CYC(b_+1397, b_+1398); E = alu_dec8(gb, E);
  CYC(b_+1398, b_+1400); A = 0x80;
  CYC(b_+1400, b_+1401); mem_wr(gb, DE, A);
subid9_state9:
  CALL_C(b_+1401, s_func_6cb2_hook, SYM(func_6cb2), b_+1404);
  CALL_C(b_+1404, s_func_6cbf_hook, SYM(func_6cbf), b_+1407);
  if (!(F & FZ)) { RET_TAKEN(b_+1407); return; }
  CYC(b_+1407, b_+1408);
  CYC(b_+1408, b_+1410); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+1410, b_+1411); A = B;
  CYC(b_+1411, b_+1413); alu_add(gb, 0x24);
  CYC(b_+1413, b_+1414); mem_wr(gb, DE, A);
  CYC(b_+1414, b_+1416); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+1416, b_+1417); A = mem_rd(gb, DE);
  CYC(b_+1417, b_+1419); alu_cp(gb, 0x08);
  CYC(b_+1419, b_+1421); A = 0x76;
  if (F & FZ) { CYCT(b_+1421, b_+1423); goto L_6c66; }
  CYC(b_+1421, b_+1423);
  CYC(b_+1423, b_+1425); A = 0x7a;
L_6c66:
  CYC(b_+1425, b_+1426); alu_add(gb, C);
  CYC(b_+1426, b_+1428); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+1428, b_+1429); mem_wr(gb, DE, A);
  RET(b_+1429); return;
}

void s_func_6c6b_hook(GB *gb) {
  BASE(func_6c6b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = alu_dec8(gb, B);
  if (F & FZ) { CYCT(b_+1, b_+3); TAIL_S(func_6c8a); }
  CYC(b_+1, b_+3);
  CYC(b_+3, b_+5); C = 0x76;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+7, b_+9); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_6c78; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); C = 0x7a;
L_6c78:
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x24);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+19, b_+20); mem_wr(gb, HL, C);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 0x04);
  if (F & FC) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); A = 0x02;
  CYC(b_+28, b_+31);
  TAIL(enemySetAnimation);
}

void s_func_6c8a_hook(GB *gb) {
  BASE(func_6c8a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x0c);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x0e);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x20);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x78);
  CYC(b_+19, b_+22); SET_HL(wRoomCollisions + 0x16);
  CYC(b_+22, b_+24); A = 0x0f;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = 0x26;
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); A = 0x03;
  CALL_C(b_+34, s_enemySetAnimation, SYM(enemySetAnimation), b_+37);
  CYC(b_+37, b_+40);
  TAIL(objectSetVisible83);
}

void s_func_6cb2_hook(GB *gb) {
  BASE(func_6cb2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x06);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); SET_HL(pop_effect(gb));
  CYC(b_+10, b_+13);
  TAIL(enemyDelete);
}

void s_func_6cbf_hook(GB *gb) {
  BASE(func_6cbf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); TAIL_S(func_6cd8); }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_rlca(gb);
  CYC(b_+17, b_+19); B = 0x08;
  if (F & FC) CALL_C_CC(b_+19, s_objectCreateInteractionWithSubid00, SYM(objectCreateInteractionWithSubid00), b_+22);
  else CYC(b_+19, b_+22);
  CYC(b_+22, b_+25);
  TAIL(objectSetInvisible);
}

void s_func_6cd8_hook(GB *gb) {
  BASE(func_6cd8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); alu_sub(gb, 0x24);
  CYC(b_+5, b_+7); A = alu_sra(gb, A);
  CYC(b_+7, b_+9); A = alu_sra(gb, A);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+17); C = 0x76;
  if (!(F & FC)) { CYCT(b_+17, b_+19); goto L_6ced; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); C = 0x7a;
L_6ced:
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); alu_sub(gb, C);
  CYC(b_+23, b_+25); A = alu_sra(gb, A);
  CYC(b_+25, b_+27); A = alu_sra(gb, A);
  CYC(b_+27, b_+28); C = A;
  CYC(b_+28, b_+29); alu_xor(gb, A);
  RET(b_+29); return;
}

void s_func_6cf6_hook(GB *gb) {
  BASE(func_6cf6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_6d03; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_getRandomNumber, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x20);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
L_6d03:
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+17); SET_HL(b_+30);
  CYC(b_+17, b_+18); gleeok_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x06);
  RET(b_+29); return;
}

