#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/agunima.s.
// ENEMY_AGUNIMA

static uint16_t agunima_jump_table(GB *gb) {
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

static void agunima_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void agunima_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ENEMY_AGUNIMA
void s_enemyCode73_hook(GB *gb) {
  BASE(enemyCode73);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x04);
  if (F & FZ) { CYCT(b_+4, b_+6); goto justHit; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+13, b_+16); TAIL(enemyDelete); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wFrameCounter);
  CYC(b_+19, b_+21); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto L_4d88; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+28); alu_sub(gb, 0x04);
  CYC(b_+28, b_+30); alu_and(gb, 0x0c);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, s_enemySetAnimation, SYM(enemySetAnimation), b_+34);
L_4d88:
  CYC(b_+34, b_+37);
  TAIL(enemyBoss_dead_b0f);
justHit:
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_cp(gb, 0x5a);
  if (F & FZ) { CYCT(b_+42, b_+44); goto normalStatus; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); A = 0x29;
  CALL_C(b_+46, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+49);
  CYC(b_+49, b_+50); E = L;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+52, b_+54); goto normalStatus; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+59, b_+61); goto normalStatus; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); C = A;
L_4da9:
  CYC(b_+67, b_+68); push_effect(gb, HL);
  CYC(b_+68, b_+69); H = mem_rd(gb, HL);
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+71, b_+72); mem_wr(gb, HL, C);
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x0e);
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+78, b_+80); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+80, b_+82); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x01);
  CYC(b_+84, b_+85); SET_HL(pop_effect(gb));
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);
  CYC(b_+86, b_+88); A = 0xb4;
  CYC(b_+88, b_+89); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(b_+89, b_+91); goto L_4da9; }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+93); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+93, b_+94); mem_wr(gb, HL, C);
  CYC(b_+94, b_+96); E = ENEMY_BASE + OBJ_VAR19;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+98, b_+100); goto L_4dcf; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); H = A;
  CYC(b_+101, b_+103); L = 0xd7;
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0xff);
L_4dcf:
  CYC(b_+105, b_+106); A = C;
  CYC(b_+106, b_+107); alu_or(gb, A);
  CYC(b_+107, b_+108); H = D;
  if (F & FZ) { RET_TAKEN(b_+108); return; }
  CYC(b_+108, b_+109);
  CYC(b_+109, b_+111); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+111, b_+113); A = 0x4b;
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+116); mem_wr(gb, HL, A);
  RET(b_+116); return;
normalStatus:
  CALL_C(b_+117, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+120);
  if (F & FC) { CYCT(b_+120, b_+122); goto L_4de7; }
  CYC(b_+120, b_+122);
  CYC(b_+122, b_+123); B = alu_dec8(gb, B);
  if (F & FZ) { CYCT(b_+123, b_+126); TAIL_S(agunimaSubId01); }
  CYC(b_+123, b_+126);
  CYC(b_+126, b_+129);
  TAIL_S(agunimaSubId00);
L_4de7:
  CYC(b_+129, b_+130); push_effect(gb, b_+130);
  do { uint16_t jt_ = (agunima_jump_table(gb));
    if (jt_ == b_+146) goto state0;
    if (jt_ == b_+158) goto state1;
    if (jt_ == b_+194) goto stateStub;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+146, b_+147); A = B;
  CYC(b_+147, b_+148); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+148, b_+151); TAIL(ecom_setSpeedAndState8_b0d); }
  CYC(b_+148, b_+151);
  CYC(b_+151, b_+152); A = alu_inc8(gb, A);
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  CYC(b_+153, b_+155); A = 0x73;
  CYC(b_+155, b_+158);
  TAIL(enemyBoss_initializeRoom_b0f);
state1:
  CYC(b_+158, b_+160); B = 0x04;
  CALL_C(b_+160, s_checkBEnemySlotsAvailable, SYM(checkBEnemySlotsAvailable), b_+163);
  if (!(F & FZ)) { RET_TAKEN(b_+163); return; }
  CYC(b_+163, b_+164);
  CYC(b_+164, b_+166); B = 0x73;
  CALL_C(b_+166, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+169);
  CYC(b_+169, b_+171); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+171, b_+172); C = H;
  CYC(b_+172, b_+174); E = 0x03;
L_4e14:
  CYC(b_+174, b_+175); push_effect(gb, HL);
  CALL_C(b_+175, s_ecom_spawnUncountedEnemyWithSubid01, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+178);
  CYC(b_+178, b_+179); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+179, b_+181); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+181, b_+183); A = 0x80;
  CYC(b_+183, b_+184); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+184, b_+185); mem_wr(gb, HL, C);
  CYC(b_+185, b_+186); A = H;
  CYC(b_+186, b_+187); SET_HL(pop_effect(gb));
  CYC(b_+187, b_+188); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+188, b_+189); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+189, b_+191); goto L_4e14; }
  CYC(b_+189, b_+191);
  CYC(b_+191, b_+194);
  TAIL(enemyDelete);
stateStub:
  RET(b_+194); return;
}

void s_agunimaSubId01_hook(GB *gb) {
  BASE(agunimaSubId01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (agunima_jump_table(gb));
    if (jt_ == b_+10) goto state8;
    if (jt_ == b_+161) goto state9;
    if (jt_ == b_+185) goto stateA;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+10, b_+11); E = alu_inc8(gb, E);
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (agunima_jump_table(gb));
    if (jt_ == b_+25) goto state8_substate0;
    if (jt_ == b_+45) goto state8_substate1;
    if (jt_ == b_+65) goto state8_substate2;
    if (jt_ == b_+80) goto state8_substate3;
    if (jt_ == b_+123) goto state8_substate4;
    if (jt_ == b_+139) goto state8_substate5;
    HANDOFF(HL);
  } while (0);
state8_substate0:
  CYC(b_+25, b_+28); A = mem_rd(gb, wcc93);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+33); SET_BC(0x010c);
  CALL_C(b_+33, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); A = alu_inc8(gb, A);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+42); SET_BC(0x2f02);
  CYC(b_+42, b_+45);
  TAIL(showText);
state8_substate1:
  CYC(b_+45, b_+47); A = 0x2d;
  CYC(b_+47, b_+50); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+50, s_playSound, SYM(playSound_b00), b_+53);
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x3c);
  CYC(b_+58, b_+60); A = 0xb1;
  CYC(b_+60, b_+63); SET_BC(SYM(loadTilesetGraphics));
  CYC(b_+63, b_+65);
  goto state8_func_4e86;
state8_substate2:
  CALL_C(b_+65, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+68);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x00);
  CYC(b_+73, b_+75); A = 0xb2;
  CYC(b_+75, b_+78); SET_BC(0x7437);
  CYC(b_+78, b_+80);
  goto state8_func_4e86;
state8_substate3:
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+83); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+83, b_+85); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x00);
  CYC(b_+88, b_+90); A = 0xb3;
  CYC(b_+90, b_+93); SET_BC(0x7a74);
state8_func_4e86:
  CYC(b_+93, b_+94); L = E;
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+95, b_+96); L = A;
  CYC(b_+96, b_+97); H = mem_rd(gb, HL);
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x01);
  CYC(b_+101, b_+103); L = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+103, s_setShortPosition_paramC, SYM(setShortPosition_paramC), b_+106);
  CYC(b_+106, b_+108); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+108, b_+109); A = B;
  CYC(b_+109, b_+111); alu_and(gb, 0xf0);
  CYC(b_+111, b_+113); alu_add(gb, 0x08);
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+114, b_+115); A = B;
  CYC(b_+115, b_+117); alu_and(gb, 0x0f);
  CYC(b_+117, b_+119); A = alu_swap(gb, A);
  CYC(b_+119, b_+121); alu_add(gb, 0x08);
  CYC(b_+121, b_+122); mem_wr(gb, HL, A);
  RET(b_+122); return;
state8_substate4:
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+126, b_+128); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+128); return; }
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+131); mem_wr(gb, HL, 0x00);
  CYC(b_+131, b_+132); L = E;
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+133, b_+134); L = alu_inc8(gb, L);
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x3c);
  CYC(b_+136, b_+139);
  TAIL_S(func_502a);
state8_substate5:
  CALL_C(b_+139, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+142);
  if (!(F & FZ)) { RET_TAKEN(b_+142); return; }
  CYC(b_+142, b_+143);
  CYC(b_+143, b_+145); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+145, b_+146); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+146, b_+148); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+148, b_+150); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+150, b_+152); B = 0x03;
L_4ec1:
  CYC(b_+152, b_+153); A = mem_rd(gb, DE);
  CYC(b_+153, b_+154); H = A;
  CYC(b_+154, b_+156); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+156, b_+157); E = alu_inc8(gb, E);
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_4ec1; }
  CYC(b_+158, b_+160);
  RET(b_+160); return;
state9:
  CYC(b_+161, b_+163); E = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+163, b_+165); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+165, b_+167); B = 0x03;
L_4ed0:
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); H = A;
  CYC(b_+169, b_+170); A = mem_rd(gb, HL);
  CYC(b_+170, b_+171); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+171); return; }
  CYC(b_+171, b_+172);
  CYC(b_+172, b_+173); E = alu_inc8(gb, E);
  CYC(b_+173, b_+174); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+174, b_+176); goto L_4ed0; }
  CYC(b_+174, b_+176);
  CYC(b_+176, b_+177); H = D;
  CYC(b_+177, b_+179); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+179, b_+180); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+180, b_+182); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0x3c);
  RET(b_+184); return;
stateA:
  CALL_C(b_+185, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+188);
  if (!(F & FZ)) { RET_TAKEN(b_+188); return; }
  CYC(b_+188, b_+189);
  CYC(b_+189, b_+190); L = E;
  CYC(b_+190, b_+191); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+191, b_+194);
  TAIL_S(func_5007);
}

// agunimaSubId00@state9@func_4f55
static void agunima_state9_func_4f55(GB *gb) {
  BASE(agunimaSubId00);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = ENEMY_BASE + OBJ_VAR31;
  CALL_C(b_+109, s_ecom_readPositionVars, SYM(ecom_readPositionVars_b0d), b_+112);
  CYC(b_+112, b_+113); alu_cp(gb, C);
  if (!(F & FZ)) { RET_TAKEN(b_+113); return; }
  CYC(b_+113, b_+114);
  CYC(b_+114, b_+116); A = mem_rd(gb, hFF8F);
  CYC(b_+116, b_+117); alu_cp(gb, B);
  RET(b_+117); return;
}

void s_agunimaSubId00_hook(GB *gb) {
  BASE(agunimaSubId00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_5104_hook, SYM(func_5104), b_+3);
  CALL_C(b_+3, s_func_5122_hook, SYM(func_5122), b_+6);
  CALL_C(b_+6, s_func_512b_hook, SYM(func_512b), b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_sub(gb, 0x08);
  CYC(b_+14, b_+15); push_effect(gb, b_+15);
  do { uint16_t jt_ = (agunima_jump_table(gb));
    if (jt_ == b_+29) goto state8;
    if (jt_ == b_+61) goto state9;
    if (jt_ == b_+167) goto stateA;
    if (jt_ == b_+184) goto stateB;
    if (jt_ == b_+207) goto stateC;
    if (jt_ == b_+231) goto stateD;
    if (jt_ == b_+254) goto stateE;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x5a);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x14);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0xfc);
  CALL_C(b_+44, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+52, b_+54); A = mem_rd(gb, hRng2);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CALL_C(b_+55, s_objectSetVisible81, SYM(objectSetVisible81), b_+58);
  CYC(b_+58, b_+61);
  TAIL(objectSetInvisible);
state9:
  CYC(b_+61, b_+62); E = alu_inc8(gb, E);
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+64); push_effect(gb, b_+64);
  do { uint16_t jt_ = (agunima_jump_table(gb));
    if (jt_ == b_+70) goto state9_substate0;
    if (jt_ == b_+118) goto state9_substate1;
    if (jt_ == b_+137) goto state9_substate2;
    HANDOFF(HL);
  } while (0);
state9_substate0:
  CYC(b_+70, b_+71); H = D;
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+73, b_+75); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+75); return; }
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+77); L = E;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+80, b_+82); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CALL_L(b_+82, agunima_state9_func_4f55, b_+85);
  CYC(b_+85, b_+87); A = 0x08;
  if (F & FZ) { CYCT(b_+87, b_+89); goto L_4f4c; }
  CYC(b_+87, b_+89);
  CALL_C(b_+89, s_objectGetRelativeAngleWithTempVars, SYM(objectGetRelativeAngleWithTempVars), b_+92);
  CYC(b_+92, b_+94); alu_add(gb, 0x04);
  CYC(b_+94, b_+96); alu_and(gb, 0x18);
  CYC(b_+96, b_+97); alu_rrca(gb);
L_4f4c:
  CYC(b_+97, b_+99); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CALL_C(b_+100, s_enemySetAnimation, SYM(enemySetAnimation), b_+103);
  CYC(b_+103, b_+106);
  TAIL_S(func_507e);

state9_substate1:
  CALL_L(b_+118, agunima_state9_func_4f55, b_+121);
  if (!(F & FZ)) { CYCT(b_+121, b_+124); TAIL(ecom_moveTowardPosition_b0d); }
  CYC(b_+121, b_+124);
  CYC(b_+124, b_+125); L = E;
  CYC(b_+125, b_+126); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+126, b_+128); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+128, b_+130); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(b_+130, b_+132); L = 0x97;
  CYC(b_+132, b_+133); H = mem_rd(gb, HL);
  CYC(b_+133, b_+135); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x01);
state9_substate2:
  CALL_C(b_+137, s_func_5131_hook, SYM(func_5131), b_+140);
  CYC(b_+140, b_+141); H = D;
  CYC(b_+141, b_+143); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+143, b_+145); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+145, b_+148); TAIL_S(func_5071); }
  CYC(b_+145, b_+148);
  CYC(b_+148, b_+150); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x0c);
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+154, b_+156); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+156, b_+158); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+158, b_+160); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+160, b_+162); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x14);
  CYC(b_+164, b_+167);
  TAIL(objectSetVisible81);
stateA:
  CYC(b_+167, b_+168); H = D;
  CYC(b_+168, b_+170); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+170, b_+172); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+172); return; }
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+175); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+175, b_+176); L = E;
  CYC(b_+176, b_+177); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+177, b_+179); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x3c);
  CYC(b_+181, b_+184);
  TAIL_S(func_5071);
stateB:
  CALL_C(b_+184, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+187);
  if (!(F & FZ)) { CYCT(b_+187, b_+190); TAIL_S(seasonsFunc_0e_506b); }
  CYC(b_+187, b_+190);
  CYC(b_+190, b_+191); L = E;
  CYC(b_+191, b_+192); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+192, b_+194); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+194, b_+196); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+196, b_+198); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+198, b_+200); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+200, b_+202); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+202, b_+204); mem_wr(gb, HL, 0x08);
  CYC(b_+204, b_+207);
  TAIL(objectSetVisible81);
stateC:
  CALL_C(b_+207, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+210);
  if (!(F & FZ)) { CYCT(b_+210, b_+213); TAIL_S(seasonsFunc_0e_506b); }
  CYC(b_+210, b_+213);
  CYC(b_+213, b_+214); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+214, b_+216); B = 0x39;
  CALL_C(b_+216, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+219);
  if (!(F & FZ)) { CYCT(b_+219, b_+222); TAIL_S(seasonsFunc_0e_506b); }
  CYC(b_+219, b_+222);
  CYC(b_+222, b_+223); H = D;
  CYC(b_+223, b_+225); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+225, b_+227); mem_wr(gb, HL, 0x98);
  CYC(b_+227, b_+229); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+229, b_+230); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+230); return;
stateD:
  CALL_C(b_+231, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+234);
  if (!(F & FZ)) { CYCT(b_+234, b_+237); TAIL_S(seasonsFunc_0e_50cf); }
  CYC(b_+234, b_+237);
  CYC(b_+237, b_+238); H = D;
  CYC(b_+238, b_+240); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+240, b_+241); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+241, b_+243); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+243, b_+245); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+245, b_+247); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+247, b_+249); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+249, b_+251); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+251, b_+253); mem_wr(gb, HL, 0x3c);
  RET(b_+253); return;
stateE:
  CALL_C(b_+254, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+257);
  if (!(F & FZ)) { RET_TAKEN(b_+257); return; }
  CYC(b_+257, b_+258);
  CYC(b_+258, b_+259); L = E;
  CYC(b_+259, b_+261); mem_wr(gb, HL, 0x0a);
  CYC(b_+261, b_+263); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+263, b_+265); mem_wr(gb, HL, 0x00);
  CYC(b_+265, b_+267); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+267, b_+268); A = mem_rd(gb, HL);
  CYC(b_+268, b_+270); alu_cp(gb, 0x43);
  if (!(F & FZ)) { CYCT(b_+270, b_+272); goto L_5004; }
  CYC(b_+270, b_+272);
  CYC(b_+272, b_+274); mem_wr(gb, HL, 0x5a);
  CYC(b_+274, b_+276); L = 0x97;
  CYC(b_+276, b_+277); H = mem_rd(gb, HL);
  CYC(b_+277, b_+279); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+279, b_+281); mem_wr(gb, HL, 0x00);
L_5004:
  CYC(b_+281, b_+284);
  TAIL(objectSetInvisible);
}

void s_func_5007_hook(GB *gb) {
  BASE(func_5007);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_add(gb, A);
  CYC(b_+7, b_+8); alu_add(gb, B);
  CYC(b_+8, b_+11); SET_HL(SYM(table_5053));
  CYC(b_+11, b_+12); agunima_add_double_index(gb, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR31;
L_5015:
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+18); C = 0x8b;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); mem_wr(gb, BC, A);
  CYC(b_+20, b_+22); C = 0x8d;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, BC, A);
  CYC(b_+24, b_+26); C = 0xb0;
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+29); mem_wr(gb, BC, A);
  CYC(b_+29, b_+30); E = alu_inc8(gb, E);
  CYC(b_+30, b_+32); A = 0xb4;
  CYC(b_+32, b_+33); alu_cp(gb, E);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_5015; }
  CYC(b_+33, b_+35);
  s_func_502a_hook(gb); return; // falls through
}

void s_func_502a_hook(GB *gb) {
  BASE(func_502a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_502a:
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+7); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_502a; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); alu_add(gb, 0xb1);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); H = A;
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+16, b_+18); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); A = alu_swap(gb, A);
  CYC(b_+23, b_+24); alu_rlca(gb);
  CYC(b_+24, b_+27); SET_BC(SYM(table_50cb));
  CALL_C(b_+27, s_addAToBc, SYM(addAToBc), b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+32, b_+33); E = L;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); E = L;
  CYC(b_+37, b_+38); A = mem_rd(gb, BC);
  CYC(b_+38, b_+39); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  RET(b_+40); return;
}

void s_seasonsFunc_0e_506b_hook(GB *gb) {
  BASE(seasonsFunc_0e_506b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  s_func_5071_hook(gb); return; // falls through
}

void s_func_5071_hook(GB *gb) {
  BASE(func_5071);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_ecom_updateCardinalAngleTowardTarget, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+3);
  CYC(b_+3, b_+4); alu_rrca(gb);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+7, b_+8); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CALL_C(b_+10, s_enemySetAnimation, SYM(enemySetAnimation), b_+13);
  s_func_507e_hook(gb); return; // falls through
}

void s_func_507e_hook(GB *gb) {
  BASE(func_507e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 3, A);
  CYC(b_+5, b_+7); B = 0x08;
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_5089; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x05;
L_5089:
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+13, b_+14); A = B;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, s_func_50ab_hook, SYM(func_50ab), b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); A = alu_swap(gb, A);
  CYC(b_+29, b_+30); alu_rlca(gb);
  CYC(b_+30, b_+33); SET_HL(SYM(table_50cb));
  CYC(b_+33, b_+34); agunima_add_a_to_hl(gb, b_+34);
  CYC(b_+34, b_+35); C = mem_rd(gb, HL);
  CYC(b_+35, b_+37); B = 0x00;
  CYC(b_+37, b_+39); A = 0x0b;
  CALL_C(b_+39, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+42);
  CYC(b_+42, b_+45);
  TAIL(objectCopyPositionWithOffset);
}

void s_func_50ab_hook(GB *gb) {
  BASE(func_50ab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_add(gb, 0x03);
  CYC(b_+12, b_+14); alu_and(gb, 0xf8);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+20); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+23, b_+25); B = 0x03;
  if (F & FZ) { CYCT(b_+25, b_+27); goto L_50c8; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); B = 0xfd;
L_50c8:
  CYC(b_+29, b_+30); alu_add(gb, B);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return;
}

void s_seasonsFunc_0e_50cf_hook(GB *gb) {
  BASE(seasonsFunc_0e_50cf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_50f2_hook, SYM(func_50f2), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+14, s_ecom_updateCardinalAngleTowardTarget, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+17);
  else CYC(b_+14, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); alu_rrca(gb);
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+24, b_+25); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+27, b_+28); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CALL_C(b_+30, s_enemySetAnimation, SYM(enemySetAnimation), b_+33);
  CYC(b_+33, b_+35);
  TAIL_S(func_507e);
}

void s_func_50f2_hook(GB *gb) {
  BASE(func_50f2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); B = 0x00;
  CYC(b_+5, b_+7); alu_cp(gb, 0x8e);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); B = alu_inc8(gb, B);
  CYC(b_+9, b_+11); alu_cp(gb, 0x84);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); B = alu_inc8(gb, B);
  CYC(b_+13, b_+15); alu_cp(gb, 0x7a);
  if (!(F & FC)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); B = alu_inc8(gb, B);
  RET(b_+17); return;
}

void s_func_5104_hook(GB *gb) {
  BASE(func_5104);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_and(gb, 0x70);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+16); SET_HL(SYM(table_511a));
  CYC(b_+16, b_+17); agunima_add_a_to_hl(gb, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return;
}

void s_func_5122_hook(GB *gb) {
  BASE(func_5122);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 1, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9);
  TAIL(ecom_flickerVisibility_b0d);
}

void s_func_512b_hook(GB *gb) {
  BASE(func_512b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0b);
  if (F & FC) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  s_func_5131_hook(gb); return; // falls through
}

void s_func_5131_hook(GB *gb) {
  BASE(func_5131);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  CYC(b_+11, b_+13); A = 0x5a;
  CYC(b_+13, b_+15); B = 0x00;
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_5146; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x43;
  CYC(b_+19, b_+21); B = 0xfc;
L_5146:
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); A = 0x0f;
  CALL_C(b_+26, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+29);
  CYC(b_+29, b_+30); mem_wr(gb, HL, B);
  RET(b_+30); return;
}
