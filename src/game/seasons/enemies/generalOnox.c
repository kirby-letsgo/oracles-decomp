#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/enemies/generalOnox.s.
// ENEMY_GENERAL_ONOX

static uint16_t generalOnox_jump_table(GB *gb) {
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

static void generalOnox_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// ENEMY_GENERAL_ONOX
void s_enemyCode02_hook(GB *gb) {
  BASE(enemyCode02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto justHit; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto dying; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0xf0;
  CALL_C(b_+15, s_playSound, SYM(playSound_b00), b_+18);
dying:
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto dead; }
  CYC(b_+22, b_+24);
  CALL_C(b_+24, s_checkLinkCollisionsEnabled, SYM(checkLinkCollisionsEnabled), b_+27);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto dead; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0xff;
  CYC(b_+31, b_+34); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+34, b_+37); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x00);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x78);
  CYC(b_+49, b_+51); A = 0x67;
  CALL_C(b_+51, s_playSound, SYM(playSound_b00), b_+54);
dead:
  CYC(b_+54, b_+57);
  TAIL(enemyBoss_dead_b0f);
justHit:
  CYC(b_+57, b_+59); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+61, s_generalOnox_func_5c75_hook, SYM(generalOnox_func_5c75), b_+64);
  else CYC(b_+61, b_+64);
normalStatus:
  CALL_C(b_+64, s_ecom_getSubidAndCpStateTo08, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+67);
  if (!(F & FC)) { CYCT(b_+67, b_+69); goto L_588b; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+70); push_effect(gb, b_+70);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+94) goto state0;
    if (jt_ == b_+144) goto stateStub;
    HANDOFF(HL);
  } while (0);
L_588b:
  CYC(b_+86, b_+87); A = B;
  CYC(b_+87, b_+88); push_effect(gb, b_+88);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == SYM(generalOnox_subid0) && hook_is(gb, SYM(generalOnox_subid0), s_generalOnox_subid0_hook)) { s_generalOnox_subid0_hook(gb); return; }
    if (jt_ == SYM(generalOnox_subid1) && hook_is(gb, SYM(generalOnox_subid1), s_generalOnox_subid1_hook)) { s_generalOnox_subid1_hook(gb); return; }
    if (jt_ == SYM(generalOnox_subid2) && hook_is(gb, SYM(generalOnox_subid2), s_generalOnox_subid2_hook)) { s_generalOnox_subid2_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+94, b_+95); A = B;
  CYC(b_+95, b_+97); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+97, b_+99); goto L_58b0; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+102); SET_BC(0x0210);
  CALL_C(b_+102, s_enemyBoss_spawnShadow, SYM(enemyBoss_spawnShadow_b0f), b_+105);
  if (!(F & FZ)) { RET_TAKEN(b_+105); return; }
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); A = 0x02;
  CYC(b_+108, b_+110); B = 0x89;
  CALL_C(b_+110, s_enemyBoss_initializeRoom, SYM(enemyBoss_initializeRoom_b0f), b_+113);
  CYC(b_+113, b_+115); A = 0x01;
  CYC(b_+115, b_+118); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+118, b_+120); A = 0x0a;
  CYC(b_+120, b_+123);
  TAIL(ecom_setSpeedAndState8_b0d);
L_58b0:
  CYC(b_+123, b_+125); A = 0x89;
  CALL_C(b_+125, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+128);
  CYC(b_+128, b_+130); A = 0x01;
  CYC(b_+130, b_+133); mem_wr(gb, wTmpcfc0 + 0x0f, A);
  CYC(b_+133, b_+136); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+136, s_ecom_setSpeedAndState8, SYM(ecom_setSpeedAndState8_b0d), b_+139);
  CYC(b_+139, b_+141); A = 0x53;
  CYC(b_+141, b_+144);
  TAIL(playSound_b00);
stateStub:
  RET(b_+144); return;
}

// generalOnox_subid2@func_5c3b
static void generalOnox_func_5c3b(GB *gb) {
  BASE(generalOnox_subid2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+149, b_+150); H = D;
  CYC(b_+150, b_+152); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+152, b_+154); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+154, b_+155); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+155, b_+157); alu_cp(gb, 0x30);
  if (!(F & FC)) { RET_TAKEN(b_+157); return; }
  CYC(b_+157, b_+158);
  CYC(b_+158, b_+160); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+160, b_+162); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+162, b_+163); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+163, b_+165); alu_add(gb, 0x10);
  CYC(b_+165, b_+167); alu_cp(gb, 0x21);
  RET(b_+167); return;
}

void s_generalOnox_subid0_hook(GB *gb) {
  BASE(generalOnox_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+14) goto state8;
    if (jt_ == b_+39) goto state9;
    if (jt_ == b_+153) goto stateA;
    if (jt_ == b_+166) goto stateB;
    if (jt_ == b_+344) goto stateC;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+14, b_+16); B = 0x47;
  CALL_C(b_+16, s_ecom_spawnProjectile, SYM(ecom_spawnProjectile_b0d), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x10);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x18);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x78);
  CYC(b_+36, b_+39);
  TAIL(objectSetVisible83);
state9:
  CYC(b_+39, b_+40); E = alu_inc8(gb, E);
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); push_effect(gb, b_+42);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+50) goto state9_substate0;
    if (jt_ == b_+67) goto state9_substate1;
    if (jt_ == b_+99) goto state9_substate2;
    if (jt_ == b_+118) goto state9_substate3;
    HANDOFF(HL);
  } while (0);
state9_substate0:
  CYC(b_+50, b_+52); A = 0x05;
  CALL_C(b_+52, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+55);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+58); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+58); return; }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x1e);
state9_substate1:
  CALL_C(b_+67, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+70);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+74); A = mem_rd(gb, wFrameCounter);
  CYC(b_+74, b_+76); alu_and(gb, 0x1f);
  CYC(b_+76, b_+78); A = 0x70;
  if (F & FZ) CALL_C_CC(b_+78, s_playSound, SYM(playSound_b00), b_+81);
  else CYC(b_+78, b_+81);
  CALL_C(b_+81, s_objectApplySpeed, SYM(objectApplySpeed), b_+84);
  CYC(b_+84, b_+86); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CYC(b_+87, b_+89); alu_cp(gb, 0x48);
  if (!(F & FZ)) { CYCT(b_+89, b_+92); TAIL(enemyAnimate); }
  CYC(b_+89, b_+92);
  CYC(b_+92, b_+93); H = D;
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+95, b_+96); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x08);
state9_substate2:
  CALL_C(b_+99, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+102);
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; }
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+104); L = E;
  CYC(b_+104, b_+105); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+105, b_+108); SET_BC(0x501c);
  CALL_C(b_+108, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+111);
  if (F & FZ) { CYCT(b_+111, b_+113); goto L_5939; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+115); C = 0x20;
L_5939:
  CYC(b_+115, b_+118);
  TAIL(showText);
state9_substate3:
  CYC(b_+118, b_+120); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+120, b_+122); A = 0x0f;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+125); A = 0x2e;
  CYC(b_+125, b_+128); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+128, s_playSound, SYM(playSound_b00), b_+131);
  CYC(b_+131, b_+133); A = 0x04;
  CALL_C(b_+133, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+136);
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
func_594f:
  CYC(b_+137, b_+138); H = D;
  CYC(b_+138, b_+140); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0x0a);
  CYC(b_+142, b_+143); L = alu_inc8(gb, L);
  CYC(b_+143, b_+145); mem_wr(gb, HL, 0x00);
  CYC(b_+145, b_+146); L = alu_inc8(gb, L);
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x2d);
  CYC(b_+148, b_+150); A = 0x02;
  CYC(b_+150, b_+153);
  TAIL(enemySetAnimation);
stateA:
  CALL_C(b_+153, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+156);
  if (!(F & FZ)) { RET_TAKEN(b_+156); return; }
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0xb4);
  CYC(b_+159, b_+160); L = alu_inc8(gb, L);
  CYC(b_+160, b_+162); mem_wr(gb, HL, 0x0a);
  CYC(b_+162, b_+163); L = E;
  CYC(b_+163, b_+164); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+164, b_+166);
  goto stateB_func_59c0;
stateB:
  CYC(b_+166, b_+167); E = alu_inc8(gb, E);
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); push_effect(gb, b_+169);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+175) goto stateB_substate0;
    if (jt_ == b_+253) goto stateB_substate1;
    if (jt_ == b_+312) goto stateB_substate2;
    HANDOFF(HL);
  } while (0);
stateB_substate0:
  CALL_C(b_+175, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+178);
  if (!(F & FZ)) { CYCT(b_+178, b_+180); goto stateB_func_598b; }
  CYC(b_+178, b_+180);
  CYC(b_+180, b_+182); A = 0x24;
  CALL_C(b_+182, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+185);
  CYC(b_+185, b_+187); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+187, b_+189); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+189, b_+191); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+191, b_+193); L = PART_BASE + OBJ_STATE;
  CYC(b_+193, b_+195); mem_wr(gb, HL, 0x08);
  CYC(b_+195, b_+197);
  goto func_5a06;
stateB_func_598b:
  CALL_L(b_+197, generalOnox_func_5c3b, b_+200);
  if (!(F & FC)) { CYCT(b_+200, b_+202); goto L_59b1; }
  CYC(b_+200, b_+202);
  CALL_C(b_+202, s_enemyAnimate, SYM(enemyAnimate), b_+205);
  CALL_C(b_+205, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+208);
  if (!(F & FZ)) { CYCT(b_+208, b_+210); goto stateB_func_59c0; }
  CYC(b_+208, b_+210);
  CYC(b_+210, b_+212); A = 0x09;
  CALL_C(b_+212, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+215);
  CYC(b_+215, b_+216); A = mem_rd(gb, HL);
  CYC(b_+216, b_+218); alu_sub(gb, 0x0e);
  CYC(b_+218, b_+220); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+220, b_+222); goto stateB_func_59c0; }
  CYC(b_+220, b_+222);
  CYC(b_+222, b_+224); L = PART_BASE + OBJ_STATE;
  CYC(b_+224, b_+225); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+225, b_+227); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+227, b_+229); A = 0x01;
  CYC(b_+229, b_+230); mem_wr(gb, DE, A);
  CYC(b_+230, b_+232); A = 0x05;
  CYC(b_+232, b_+235);
  TAIL(enemySetAnimation);
L_59b1:
  CYC(b_+235, b_+237); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0a);
  CYC(b_+239, b_+242); A = mem_rd(gb, wFrameCounter);
  CYC(b_+242, b_+244); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+244, s_generalOnox_func_59c0_hook, SYM(generalOnox_func_59c0), b_+247);
  else CYC(b_+244, b_+247);
  CALL_C(b_+247, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+250);
stateB_func_59c0:
  CYC(b_+250, b_+253);
  TAIL(enemyAnimate);
stateB_substate1:
  CYC(b_+253, b_+255); A = 0x09;
  CALL_C(b_+255, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+258);
  CYC(b_+258, b_+259); A = mem_rd(gb, HL);
  CYC(b_+259, b_+261); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+261); return; }
  CYC(b_+261, b_+262);
  CYC(b_+262, b_+264); L = PART_BASE + OBJ_STATE;
  CYC(b_+264, b_+265); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+265, b_+267); L = PART_BASE + OBJ_DAMAGE;
  CYC(b_+267, b_+269); mem_wr(gb, HL, 0xf8);
  CYC(b_+269, b_+271); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+271, b_+273); mem_wr(gb, HL, 0x0e);
  CYC(b_+273, b_+275); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+275, b_+277); mem_wr(gb, HL, 0x00);
  CYC(b_+277, b_+279); L = PART_BASE + OBJ_YH;
  CYC(b_+279, b_+281); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+284); alu_sub(gb, 0x10);
  CYC(b_+284, b_+285); mem_wr(gb, HL, A);
  CYC(b_+285, b_+287); L = PART_BASE + OBJ_VAR30;
  CYC(b_+287, b_+289); alu_add(gb, 0x21);
  CYC(b_+289, b_+290); mem_wr(gb, HL, A);
  CYC(b_+290, b_+292); L = PART_BASE + OBJ_XH;
  CYC(b_+292, b_+294); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+294, b_+295); A = mem_rd(gb, DE);
  CYC(b_+295, b_+297); alu_add(gb, 0x08);
  CYC(b_+297, b_+298); mem_wr(gb, HL, A);
  CYC(b_+298, b_+300); L = PART_BASE + OBJ_VAR31;
  CYC(b_+300, b_+302); alu_add(gb, 0xf9);
  CYC(b_+302, b_+303); mem_wr(gb, HL, A);
  CYC(b_+303, b_+305); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+305, b_+307); A = 0x02;
  CYC(b_+307, b_+308); mem_wr(gb, DE, A);
  CYC(b_+308, b_+309); A = alu_inc8(gb, A);
  CYC(b_+309, b_+312);
  TAIL(enemySetAnimation);
stateB_substate2:
  CYC(b_+312, b_+314); A = 0x24;
  CALL_C(b_+314, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+317);
  CYC(b_+317, b_+319); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+319); return; }
  CYC(b_+319, b_+320);
func_5a06:
  CALL_C(b_+320, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+323);
  CYC(b_+323, b_+325); alu_cp(gb, 0x8c);
  if (F & FC) { CYCT(b_+325, b_+328); goto func_594f; }
  CYC(b_+325, b_+328);
  CYC(b_+328, b_+329); H = D;
  CYC(b_+329, b_+331); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+331, b_+332); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+332, b_+333); L = alu_inc8(gb, L);
  CYC(b_+333, b_+335); mem_wr(gb, HL, 0x00);
  CYC(b_+335, b_+337); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+337, b_+339); mem_wr(gb, HL, 0x10);
  CYC(b_+339, b_+341); A = 0x04;
  CYC(b_+341, b_+344);
  TAIL(enemySetAnimation);
stateC:
  CYC(b_+344, b_+345); E = alu_inc8(gb, E);
  CYC(b_+345, b_+346); A = mem_rd(gb, DE);
  CYC(b_+346, b_+347); push_effect(gb, b_+347);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+353) goto stateC_substate0;
    if (jt_ == b_+374) goto stateC_substate1;
    if (jt_ == b_+400) goto stateC_substate2;
    HANDOFF(HL);
  } while (0);
stateC_substate0:
  CALL_C(b_+353, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+356);
  if (!(F & FZ)) { RET_TAKEN(b_+356); return; }
  CYC(b_+356, b_+357);
  CYC(b_+357, b_+358); L = E;
  CYC(b_+358, b_+359); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+359, b_+361); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+361, b_+363); A = 0xc0;
  CYC(b_+363, b_+364); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+364, b_+366); mem_wr(gb, HL, 0xfd);
  CYC(b_+366, b_+368); A = 0x81;
  CALL_C(b_+368, s_playSound, SYM(playSound_b00), b_+371);
  CYC(b_+371, b_+374);
  TAIL(objectSetVisible81);
stateC_substate1:
  CYC(b_+374, b_+376); C = 0x20;
  CALL_C(b_+376, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+379);
  if (!(F & FZ)) { RET_TAKEN(b_+379); return; }
  CYC(b_+379, b_+380);
  CYC(b_+380, b_+382); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+382, b_+383); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+383, b_+384); L = alu_inc8(gb, L);
  CYC(b_+384, b_+386); A = 0xb4;
  CYC(b_+386, b_+387); mem_wr(gb, HL, A);
  CALL_C(b_+387, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+390);
  CALL_C(b_+390, s_objectSetVisible83, SYM(objectSetVisible83), b_+393);
  CALL_C(b_+393, s_getFreePartSlot, SYM(getFreePartSlot), b_+396);
  if (!(F & FZ)) { RET_TAKEN(b_+396); return; }
  CYC(b_+396, b_+397);
  CYC(b_+397, b_+399); mem_wr(gb, HL, 0x48);
  RET(b_+399); return;
stateC_substate2:
  CALL_C(b_+400, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+403);
  if (!(F & FZ)) { RET_TAKEN(b_+403); return; }
  CYC(b_+403, b_+404);
  CYC(b_+404, b_+407);
  goto func_594f;
}

// generalOnox_subid0@stateC@substate1
static void generalOnox_stateC_substate1(GB *gb) {
  BASE(generalOnox_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+374, b_+376); C = 0x20;
  CALL_C(b_+376, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+379);
  if (!(F & FZ)) { RET_TAKEN(b_+379); return; }
  CYC(b_+379, b_+380);
  CYC(b_+380, b_+382); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+382, b_+383); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+383, b_+384); L = alu_inc8(gb, L);
  CYC(b_+384, b_+386); A = 0xb4;
  CYC(b_+386, b_+387); mem_wr(gb, HL, A);
  CALL_C(b_+387, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+390);
  CALL_C(b_+390, s_objectSetVisible83, SYM(objectSetVisible83), b_+393);
  CALL_C(b_+393, s_getFreePartSlot, SYM(getFreePartSlot), b_+396);
  if (!(F & FZ)) { RET_TAKEN(b_+396); return; }
  CYC(b_+396, b_+397);
  CYC(b_+397, b_+399); mem_wr(gb, HL, 0x48);
  RET(b_+399); return;
}

// generalOnox_subid0@stateC@substate0
static void generalOnox_stateC_substate0(GB *gb) {
  BASE(generalOnox_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+353, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+356);
  if (!(F & FZ)) { RET_TAKEN(b_+356); return; }
  CYC(b_+356, b_+357);
  CYC(b_+357, b_+358); L = E;
  CYC(b_+358, b_+359); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+359, b_+361); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+361, b_+363); A = 0xc0;
  CYC(b_+363, b_+364); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+364, b_+366); mem_wr(gb, HL, 0xfd);
  CYC(b_+366, b_+368); A = 0x81;
  CALL_C(b_+368, s_playSound, SYM(playSound_b00), b_+371);
  CYC(b_+371, b_+374);
  TAIL(objectSetVisible81);
}

// generalOnox_subid0@stateB@substate1
static void generalOnox_stateB_substate1(GB *gb) {
  BASE(generalOnox_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+253, b_+255); A = 0x09;
  CALL_C(b_+255, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+258);
  CYC(b_+258, b_+259); A = mem_rd(gb, HL);
  CYC(b_+259, b_+261); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+261); return; }
  CYC(b_+261, b_+262);
  CYC(b_+262, b_+264); L = PART_BASE + OBJ_STATE;
  CYC(b_+264, b_+265); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+265, b_+267); L = PART_BASE + OBJ_DAMAGE;
  CYC(b_+267, b_+269); mem_wr(gb, HL, 0xf8);
  CYC(b_+269, b_+271); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+271, b_+273); mem_wr(gb, HL, 0x0e);
  CYC(b_+273, b_+275); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+275, b_+277); mem_wr(gb, HL, 0x00);
  CYC(b_+277, b_+279); L = PART_BASE + OBJ_YH;
  CYC(b_+279, b_+281); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+284); alu_sub(gb, 0x10);
  CYC(b_+284, b_+285); mem_wr(gb, HL, A);
  CYC(b_+285, b_+287); L = PART_BASE + OBJ_VAR30;
  CYC(b_+287, b_+289); alu_add(gb, 0x21);
  CYC(b_+289, b_+290); mem_wr(gb, HL, A);
  CYC(b_+290, b_+292); L = PART_BASE + OBJ_XH;
  CYC(b_+292, b_+294); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+294, b_+295); A = mem_rd(gb, DE);
  CYC(b_+295, b_+297); alu_add(gb, 0x08);
  CYC(b_+297, b_+298); mem_wr(gb, HL, A);
  CYC(b_+298, b_+300); L = PART_BASE + OBJ_VAR31;
  CYC(b_+300, b_+302); alu_add(gb, 0xf9);
  CYC(b_+302, b_+303); mem_wr(gb, HL, A);
  CYC(b_+303, b_+305); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+305, b_+307); A = 0x02;
  CYC(b_+307, b_+308); mem_wr(gb, DE, A);
  CYC(b_+308, b_+309); A = alu_inc8(gb, A);
  CYC(b_+309, b_+312);
  TAIL(enemySetAnimation);
}

// generalOnox_subid0@stateB@func_598b
static void generalOnox_stateB_func_598b(GB *gb) {
  BASE(generalOnox_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_L(b_+197, generalOnox_func_5c3b, b_+200);
  if (!(F & FC)) { CYCT(b_+200, b_+202); goto L_59b1; }
  CYC(b_+200, b_+202);
  CALL_C(b_+202, s_enemyAnimate, SYM(enemyAnimate), b_+205);
  CALL_C(b_+205, s_ecom_decCounter2, SYM(ecom_decCounter2_b0d), b_+208);
  if (!(F & FZ)) { CYCT(b_+208, b_+210); goto stateB_func_59c0; }
  CYC(b_+208, b_+210);
  CYC(b_+210, b_+212); A = 0x09;
  CALL_C(b_+212, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+215);
  CYC(b_+215, b_+216); A = mem_rd(gb, HL);
  CYC(b_+216, b_+218); alu_sub(gb, 0x0e);
  CYC(b_+218, b_+220); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+220, b_+222); goto stateB_func_59c0; }
  CYC(b_+220, b_+222);
  CYC(b_+222, b_+224); L = PART_BASE + OBJ_STATE;
  CYC(b_+224, b_+225); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+225, b_+227); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+227, b_+229); A = 0x01;
  CYC(b_+229, b_+230); mem_wr(gb, DE, A);
  CYC(b_+230, b_+232); A = 0x05;
  CYC(b_+232, b_+235);
  TAIL(enemySetAnimation);
L_59b1:
  CYC(b_+235, b_+237); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0a);
  CYC(b_+239, b_+242); A = mem_rd(gb, wFrameCounter);
  CYC(b_+242, b_+244); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+244, s_generalOnox_func_59c0_hook, SYM(generalOnox_func_59c0), b_+247);
  else CYC(b_+244, b_+247);
  CALL_C(b_+247, s_ecom_applyVelocityForSideviewEnemyNoHoles, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+250);
stateB_func_59c0:
  CYC(b_+250, b_+253);
  TAIL(enemyAnimate);
}

// generalOnox_subid0@stateA
static void generalOnox_stateA(GB *gb) {
  BASE(generalOnox_subid0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+153, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+156);
  if (!(F & FZ)) { RET_TAKEN(b_+156); return; }
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0xb4);
  CYC(b_+159, b_+160); L = alu_inc8(gb, L);
  CYC(b_+160, b_+162); mem_wr(gb, HL, 0x0a);
  CYC(b_+162, b_+163); L = E;
  CYC(b_+163, b_+164); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+164, b_+166);
  goto stateB_func_59c0;

stateB_func_59c0:
  CYC(b_+250, b_+253);
  TAIL(enemyAnimate);
}

void s_generalOnox_subid1_hook(GB *gb) {
  BASE(generalOnox_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+16) goto state8;
    if (jt_ == b_+47) goto state9;
    if (jt_ == (b_ - 254)) { generalOnox_stateA(gb); return; }
    if (jt_ == b_+159) goto stateB;
    if (jt_ == b_+254) goto stateC;
    if (jt_ == b_+270) goto stateD;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+16, b_+18); C = 0x20;
  CALL_C(b_+18, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+21);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+31); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+31, b_+34); mem_wr(gb, wScreenShakeCounterX, A);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+36, b_+37); A = alu_dec8(gb, A);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); A = 0x01;
  CALL_C(b_+41, s_enemySetAnimation, SYM(enemySetAnimation), b_+44);
  CYC(b_+44, b_+47);
  TAIL(objectSetVisible83);
state9:
  CYC(b_+47, b_+48); E = alu_inc8(gb, E);
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); push_effect(gb, b_+50);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+56) goto state9_substate0;
    if (jt_ == b_+82) goto state9_substate1;
    if (jt_ == b_+121) goto state9_substate2;
    HANDOFF(HL);
  } while (0);
state9_substate0:
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+60); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+64); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+67); A = alu_inc8(gb, A);
  CYC(b_+67, b_+70); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+70, b_+73); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+73, b_+75); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+75, b_+76); mem_wr(gb, DE, A);
  CYC(b_+76, b_+79); SET_BC(0x501d);
  CYC(b_+79, b_+82);
  TAIL(showText);
state9_substate1:
  CYC(b_+82, b_+83); H = D;
  CYC(b_+83, b_+84); L = E;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x64);
  CYC(b_+88, b_+90); A = 0x00;
  CALL_C(b_+90, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+93);
  CYC(b_+93, b_+96); SET_BC(0x18f8);
  CALL_C(b_+96, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+99);
  CYC(b_+99, b_+101); A = mem_rd(gb, hCameraY);
  CYC(b_+101, b_+102); B = A;
  CYC(b_+102, b_+104); L = PART_BASE + OBJ_YH;
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+106); alu_sub(gb, B);
  CYC(b_+106, b_+107); alu_cpl(gb);
  CYC(b_+107, b_+108); A = alu_inc8(gb, A);
  CYC(b_+108, b_+110); alu_sub(gb, 0x10);
  CYC(b_+110, b_+112); L = PART_BASE + OBJ_ZH;
  CYC(b_+112, b_+113); mem_wr(gb, HL, A);
  CYC(b_+113, b_+115); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x81);
  CYC(b_+117, b_+119); L = PART_BASE + OBJ_STATE;
  CYC(b_+119, b_+120); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+120); return;
state9_substate2:
  CALL_C(b_+121, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+124);
  if (!(F & FZ)) { RET_TAKEN(b_+124); return; }
  CYC(b_+124, b_+125);
  CYC(b_+125, b_+127); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+127, b_+129); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+129, b_+130); alu_xor(gb, A);
  CYC(b_+130, b_+133); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+133, b_+136); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
func_5ae5:
  CYC(b_+136, b_+137); H = D;
  CYC(b_+137, b_+139); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x0a);
  CYC(b_+141, b_+142); L = alu_inc8(gb, L);
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0x00);
  CYC(b_+144, b_+146); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x2d);
  CYC(b_+148, b_+150); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+150, b_+151); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+151, b_+152); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x00);
  CYC(b_+154, b_+156); A = 0x02;
  CYC(b_+156, b_+159);
  TAIL(enemySetAnimation);
stateB:
  CYC(b_+159, b_+160); E = alu_inc8(gb, E);
  CYC(b_+160, b_+161); A = mem_rd(gb, DE);
  CYC(b_+161, b_+162); push_effect(gb, b_+162);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+168) goto stateB_substate0;
    if (jt_ == (b_ - 154)) { generalOnox_stateB_substate1(gb); return; }
    if (jt_ == b_+189) goto stateB_substate2;
    HANDOFF(HL);
  } while (0);
stateB_substate0:
  CALL_C(b_+168, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+171);
  if (!(F & FZ)) { CYCT(b_+171, b_+174); generalOnox_stateB_func_598b(gb); return; }
  CYC(b_+171, b_+174);
  CYC(b_+174, b_+176); A = 0x24;
  CALL_C(b_+176, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+179);
  CYC(b_+179, b_+181); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+181, b_+183); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+183, b_+185); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+185, b_+187); L = PART_BASE + OBJ_STATE;
  CYC(b_+187, b_+189); mem_wr(gb, HL, 0x08);
stateB_substate2:
  CYC(b_+189, b_+191); A = 0x24;
  CALL_C(b_+191, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+194);
  CYC(b_+194, b_+196); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+196); return; }
  CYC(b_+196, b_+197);
func_5b22:
  CYC(b_+197, b_+198); H = D;
  CYC(b_+198, b_+200); L = ENEMY_BASE + OBJ_VAR30;
  CYC(b_+200, b_+201); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+201, b_+202); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+202, b_+204); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+204, b_+206); mem_wr(gb, HL, 0x00);
  if (F & FZ) { CYCT(b_+206, b_+208); goto L_5b49; }
  CYC(b_+206, b_+208);
  CALL_C(b_+208, s_getRandomNumber, SYM(getRandomNumber), b_+211);
  CYC(b_+211, b_+212); alu_rrca(gb);
  if (F & FC) { CYCT(b_+212, b_+214); goto func_5ae5; }
  CYC(b_+212, b_+214);
  CYC(b_+214, b_+215); H = D;
  CYC(b_+215, b_+216); L = alu_dec8(gb, L);
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x0d);
  CYC(b_+218, b_+220); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+220, b_+221); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+221, b_+222); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x02);
  CALL_C(b_+224, s_generalOnox_func_5c63_hook, SYM(generalOnox_func_5c63), b_+227);
  CYC(b_+227, b_+229); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+229, b_+230); A = mem_rd(gb, DE);
  CYC(b_+230, b_+231); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+231); return; }
  CYC(b_+231, b_+232);
  CYC(b_+232, b_+233); alu_xor(gb, A);
  CYC(b_+233, b_+236);
  TAIL(enemySetAnimation);
L_5b49:
  CYC(b_+236, b_+237); L = alu_dec8(gb, L);
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0c);
  CYC(b_+239, b_+241); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0x10);
  CYC(b_+243, b_+245); L = ENEMY_BASE + OBJ_VAR31;
  CYC(b_+245, b_+246); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+246, b_+247); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+247, b_+249); mem_wr(gb, HL, 0x01);
  CYC(b_+249, b_+251); A = 0x04;
  CYC(b_+251, b_+254);
  TAIL(enemySetAnimation);
stateC:
  CYC(b_+254, b_+255); E = alu_inc8(gb, E);
  CYC(b_+255, b_+256); A = mem_rd(gb, DE);
  CYC(b_+256, b_+257); push_effect(gb, b_+257);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == (b_ - 54)) { generalOnox_stateC_substate0(gb); return; }
    if (jt_ == (b_ - 33)) { generalOnox_stateC_substate1(gb); return; }
    if (jt_ == b_+263) goto stateC_substate2;
    HANDOFF(HL);
  } while (0);
stateC_substate2:
  CALL_C(b_+263, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+266);
  if (!(F & FZ)) { RET_TAKEN(b_+266); return; }
  CYC(b_+266, b_+267);
  CYC(b_+267, b_+270);
  goto func_5ae5;
stateD:
  CYC(b_+270, b_+271); E = alu_inc8(gb, E);
  CYC(b_+271, b_+272); A = mem_rd(gb, DE);
  CYC(b_+272, b_+273); push_effect(gb, b_+273);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+279) goto stateD_substate0;
    if (jt_ == b_+304) goto stateD_substate1;
    if (jt_ == b_+197) goto func_5b22;
    HANDOFF(HL);
  } while (0);
stateD_substate0:
  CALL_C(b_+279, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+282);
  if (!(F & FZ)) { CYCT(b_+282, b_+284); goto L_5b84; }
  CYC(b_+282, b_+284);
  CYC(b_+284, b_+285); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+285, b_+286); L = alu_inc8(gb, L);
  CYC(b_+286, b_+288); mem_wr(gb, HL, 0x04);
  CYC(b_+288, b_+289); L = E;
  CYC(b_+289, b_+290); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+290, b_+292); A = 0x03;
  CYC(b_+292, b_+295);
  TAIL(enemySetAnimation);
L_5b84:
  CALL_C(b_+295, s_ecom_updateAngleTowardTarget, SYM(ecom_updateAngleTowardTarget_b0d), b_+298);
  CALL_C(b_+298, s_objectApplySpeed, SYM(objectApplySpeed), b_+301);
  CYC(b_+301, b_+304);
  TAIL(enemyAnimate);
stateD_substate1:
  CALL_C(b_+304, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+307);
  if (!(F & FZ)) { RET_TAKEN(b_+307); return; }
  CYC(b_+307, b_+308);
  CYC(b_+308, b_+310); mem_wr(gb, HL, 0x2d);
  CYC(b_+310, b_+311); L = alu_inc8(gb, L);
  CYC(b_+311, b_+312); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+312, b_+314); goto L_5ba3; }
  CYC(b_+312, b_+314);
  CALL_C(b_+314, s_getFreePartSlot, SYM(getFreePartSlot), b_+317);
  if (!(F & FZ)) { RET_TAKEN(b_+317); return; }
  CYC(b_+317, b_+318);
  CYC(b_+318, b_+320); mem_wr(gb, HL, 0x49);
  CYC(b_+320, b_+323); SET_BC(0x19f9);
  CYC(b_+323, b_+326);
  TAIL(objectCopyPositionWithOffset);
L_5ba3:
  CYC(b_+326, b_+327); L = E;
  CYC(b_+327, b_+328); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+328); return;
}

void s_generalOnox_subid2_hook(GB *gb) {
  BASE(generalOnox_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (generalOnox_jump_table(gb));
    if (jt_ == b_+14) goto state8;
    if (jt_ == b_+58) goto state9;
    if (jt_ == b_+77) goto stateA;
    if (jt_ == b_+129) goto stateB;
    if (jt_ == b_+137) goto stateC;
    HANDOFF(HL);
  } while (0);
state8:
  CYC(b_+14, b_+17); A = mem_rd(gb, wLinkInAir);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+22); mem_wr(gb, wTmpcfc0 + 0x0f, A);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  CYC(b_+23, b_+26); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+26, b_+27); H = D;
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x50);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x50);
  CYC(b_+35, b_+38); SET_BC(0x0b02);
  CALL_C(b_+38, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+41);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+44, b_+46); A = 0x40;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+48); E = alu_inc8(gb, E);
  CYC(b_+48, b_+49); A = H;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+52, b_+54); A = 0x09;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+58);
  TAIL(clearAllParentItems);
state9:
  CYC(b_+58, b_+60); A = 0x21;
  CALL_C(b_+60, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+63);
  CYC(b_+63, b_+65); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+67); H = D;
  CYC(b_+67, b_+69); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+74, s_objectSetVisible82, SYM(objectSetVisible82), b_+77);
stateA:
  CALL_C(b_+77, s_ecom_decCounter1, SYM(ecom_decCounter1_b0d), b_+80);
  if (F & FZ) { CYCT(b_+80, b_+82); goto L_5c06; }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+85); alu_and(gb, 0x1c);
  CYC(b_+85, b_+86); alu_rrca(gb);
  CYC(b_+86, b_+87); alu_rrca(gb);
  CYC(b_+87, b_+90); SET_HL(b_+121 /* @table_5c1f */);
  CYC(b_+90, b_+91); generalOnox_add_a_to_hl(gb, b_+91);
  CYC(b_+91, b_+93); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+93, b_+94); A = mem_rd(gb, HL);
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  RET(b_+95); return;
L_5c06:
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x5a);
  CYC(b_+98, b_+99); L = E;
  CYC(b_+99, b_+100); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+100, b_+102); A = 0x30;
  CYC(b_+102, b_+105); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+105, b_+107); A = 0x08;
  CYC(b_+107, b_+110); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+110, b_+112); A = 0x06;
  CYC(b_+112, b_+115); mem_wr(gb, wTextboxPosition, A);
  CYC(b_+115, b_+118); SET_BC(0x5022);
  CYC(b_+118, b_+121);
  TAIL(showText);

stateB:
  CYC(b_+129, b_+131); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+131, b_+133); A = 0x0c;
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  CYC(b_+134, b_+137);
  TAIL(fadeoutToWhite);
stateC:
  CYC(b_+137, b_+140); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+140, b_+141); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+141); return; }
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+145); SET_HL(wTmpcfc0 + 0x08);
  CYC(b_+145, b_+146); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+146, b_+149);
  TAIL(enemyDelete);
}

void s_generalOnox_func_59c0_hook(GB *gb) {
  BASE(generalOnox_func_59c0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+2, b_+4); alu_sub(gb, 0x18);
  CYC(b_+4, b_+6); alu_cp(gb, 0x98);
  if (F & FC) { CYCT(b_+6, b_+8); goto L_5c58; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = 0x10;
L_5c58:
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+13); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+13, b_+14); C = A;
  CALL_C(b_+14, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void s_generalOnox_func_5c63_hook(GB *gb) {
  BASE(generalOnox_func_5c63);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+8); SET_HL(b_+14 /* @table_5c71 */);
  CYC(b_+8, b_+9); generalOnox_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

void s_generalOnox_func_5c75_hook(GB *gb) {
  BASE(generalOnox_func_5c75);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x28);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x08);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+18, b_+20); A = 0x24;
  CALL_C(b_+20, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+23);
  CYC(b_+23, b_+25); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+25, b_+27); L = PART_BASE + OBJ_VISIBLE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+29, b_+31); L = PART_BASE + OBJ_STATE;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x08);
  CYC(b_+33, b_+35); A = 0x67;
  CYC(b_+35, b_+38);
  TAIL(playSound_b00);
}

