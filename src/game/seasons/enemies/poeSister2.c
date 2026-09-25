#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/poeSister2.s.
// ENEMY_POE_SISTER_2

static uint16_t poeSister2_jump_table(GB *gb) {
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

// enemyCode7e@func_5dd5
static void poeSister2_func_5dd5(GB *gb) {
  BASE(enemyCode7e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
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
}

// ENEMY_POE_SISTER_2
void s_enemyCode76_hook(GB *gb) {
  BASE(enemyCode76);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_5bf8; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x0a07);
  CYC(b_+10, b_+13);
  TAIL_S(poeSister5f7e);
L_5bf8:
  CALL_C(b_+13, s_poeSister5fc2_hook, SYM(poeSister5fc2), b_+16);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
normalStatus:
  CALL_C(b_+17, s_poeSister604b_hook, SYM(poeSister604b), b_+20);
  CALL_C(b_+20, s_poeSister602e_hook, SYM(poeSister602e), b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); push_effect(gb, b_+27);
  do { uint16_t jt_ = (poeSister2_jump_table(gb));
    if (jt_ == b_+63) goto state0;
    if (jt_ == b_+172) goto stateStub;
    if (jt_ == b_+155) goto state5;
    if (jt_ == b_+173) goto state8;
    if (jt_ == b_+190) goto state9;
    if (jt_ == b_+203) goto stateA;
    if (jt_ == b_+234) goto stateB;
    if (jt_ == b_+261) goto stateC;
    if (jt_ == b_+272) goto stateD;
    if (jt_ == b_+289) goto stateE;
    if (jt_ == b_+321) goto state10;
    if (jt_ == b_+365) goto state11;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+63, b_+65); A = 0x76;
  CYC(b_+65, b_+68); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CALL_C(b_+68, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+71);
  CYC(b_+71, b_+73); E = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0xff);
  CYC(b_+79, b_+81); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x46);
  CYC(b_+83, b_+85); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+87, b_+89); goto func5c6d; }
  CYC(b_+87, b_+89);
  CALL_C(b_+89, s_getFreePartSlot, SYM(getFreePartSlot), b_+92);
  if (!(F & FZ)) { RET_TAKEN(b_+92); return; }
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x08);
  CYC(b_+95, b_+97); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+97, b_+99); A = 0x04;
  CYC(b_+99, b_+100); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  CYC(b_+101, b_+104); mem_wr(gb, wNumTorchesLit, A);
  CYC(b_+104, b_+107); SET_HL(w1ReservedInteraction0_var3f + 0x02);
L_5c56:
  CYC(b_+107, b_+108); A = mem_rd(gb, HL);
  CYC(b_+108, b_+110); alu_cp(gb, 0x7e);
  if (F & FZ) { CYCT(b_+110, b_+112); goto L_5c5e; }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+113); H = alu_inc8(gb, H);
  CYC(b_+113, b_+115);
  goto L_5c56;
L_5c5e:
  CYC(b_+115, b_+117); E = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+117, b_+118); L = E;
  CYC(b_+118, b_+120); A = 0x80;
  CYC(b_+120, b_+121); mem_wr(gb, DE, A);
  CYC(b_+121, b_+122); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+122, b_+123); E = alu_inc8(gb, E);
  CYC(b_+123, b_+124); A = H;
  CYC(b_+124, b_+125); mem_wr(gb, DE, A);
  CYC(b_+125, b_+126); mem_wr(gb, HL, D);
  CYC(b_+126, b_+127); H = D;
  CYC(b_+127, b_+130);
  poeSister2_func_5dd5(gb); return;
func5c6d:
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
state5:
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
stateStub:
  RET(b_+172); return;
state8:
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
state9:
  CALL_C(b_+190, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+193);
  if (!(F & FZ)) { CYCT(b_+193, b_+196); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+193, b_+196);
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0x1f);
  CYC(b_+198, b_+199); L = E;
  CYC(b_+199, b_+200); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+200, b_+203);
  TAIL(objectSetVisible82);
stateA:
  CALL_C(b_+203, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+206);
  if (F & FZ) { CYCT(b_+206, b_+208); goto L_5cc9; }
  CYC(b_+206, b_+208);
  CYC(b_+208, b_+209); A = mem_rd(gb, HL);
  CYC(b_+209, b_+210); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+210, b_+212); goto animate; }
  CYC(b_+210, b_+212);
  CYC(b_+212, b_+213); alu_xor(gb, A);
  CYC(b_+213, b_+216); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+216, b_+219); SET_BC(0x0a05);
  CYC(b_+219, b_+222);
  TAIL(showText);
L_5cc9:
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x2d);
  CYC(b_+224, b_+225); L = E;
  CYC(b_+225, b_+226); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+226, b_+228); A = 0x2d;
  CYC(b_+228, b_+231); mem_wr(gb, wActiveMusic, A);
  CYC(b_+231, b_+234);
  TAIL(playSound_b00);
stateB:
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
stateC:
  CALL_C(b_+261, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+264);
  if (!(F & FZ)) { RET_TAKEN(b_+264); return; }
  CYC(b_+264, b_+265);
  CYC(b_+265, b_+267); mem_wr(gb, HL, 0x18);
  CYC(b_+267, b_+268); L = E;
  CYC(b_+268, b_+269); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+269, b_+272);
  TAIL_S(func_5e45);
stateD:
  CALL_C(b_+272, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+275);
  if (!(F & FZ)) { CYCT(b_+275, b_+278); TAIL(ecom_flickerVisibility_b0d); }
  CYC(b_+275, b_+278);
  CYC(b_+278, b_+280); mem_wr(gb, HL, 0x30);
  CYC(b_+280, b_+281); L = E;
  CYC(b_+281, b_+282); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+282, b_+284); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+284, b_+286); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+286, b_+289);
  TAIL(objectSetVisible82);
stateE:
  CALL_C(b_+289, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+292);
  if (F & FZ) { CYCT(b_+292, b_+295); TAIL_S(poeSister5f3b); }
  CYC(b_+292, b_+295);
  CYC(b_+295, b_+296); A = mem_rd(gb, HL);
  CYC(b_+296, b_+298); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+298, b_+300); goto L_5d23; }
  CYC(b_+298, b_+300);
  CYC(b_+300, b_+302); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+302, b_+304); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+304, b_+305); A = mem_rd(gb, DE);
  CYC(b_+305, b_+306); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+306, b_+308); alu_and(gb, 0x1f);
  CYC(b_+308, b_+309); mem_wr(gb, HL, A);
  CALL_C(b_+309, s_ecom_updateAnimationFromAngle, SYM(ecom_updateAnimationFromAngle_b0d), b_+312);
L_5d23:
  CALL_C(b_+312, s_func_5f49_hook, SYM(func_5f49), b_+315);
  CALL_C(b_+315, s_objectApplySpeed, SYM(objectApplySpeed), b_+318);
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
state11:
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

