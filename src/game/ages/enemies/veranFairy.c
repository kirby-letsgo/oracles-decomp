#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/ages/enemies/veranFairy.s (ENEMY_VERAN_FAIRY), bank $10.

static uint16_t veranFairy_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void veranFairy_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void veranFairy_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void veranFairy_checkWithinBoundary_hook(GB *gb) {
  BASE(veranFairy_checkWithinBoundary);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x10);
  CYC(b_+5, b_+7); alu_cp(gb, 0x90);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_sub(gb, 0x10);
  CYC(b_+13, b_+15); alu_cp(gb, 0xd0);
  RET(b_+15); return;
}

void veranFairy_updateVar35BasedOnHealth_hook(GB *gb) {
  BASE(veranFairy_updateVar35BasedOnHealth);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x14);
  if (!(F & FC)) {
    CYCT(b_+7, b_+9);
    goto writeVar35;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CYC(b_+10, b_+12); alu_cp(gb, 0x0a);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
    goto writeVar35;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); B = alu_inc8(gb, B);
writeVar35:
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  RET(b_+19); return;
}

void attack0_hook(GB *gb) {
  BASE(attack0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto decCounter;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x0f);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+19); alu_add(gb, 0x08);
  CYC(b_+19, b_+20); alu_cp(gb, B);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x3c);
  if (!(F & FC)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x04);
decCounter:
  CALL_C(b_+36, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+39);
  if (F & FZ) {
    CYCT(b_+39, b_+41);
    goto checkBoundary;
  }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+44); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); A = 0x05;
  CYC(b_+47, b_+50);
  TAIL(enemySetAnimation);
checkBoundary:
  CALL_C(b_+50, veranFairy_checkWithinBoundary_hook, SYM(veranFairy_checkWithinBoundary), b_+53);
  if (!(F & FC)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(b_+57, b_+59);
    goto resetCounters;
  }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x1e);
  CYC(b_+63, b_+65); B = 0x2d;
  CALL_C(b_+65, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+68);
  CYC(b_+68, b_+70); A = 0x06;
  CYC(b_+70, b_+73);
  TAIL(enemySetAnimation);
resetCounters:
  CYC(b_+73, b_+75); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x5a);
  CYC(b_+77, b_+79); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x00);
  RET(b_+81); return;
}

void attack1_hook(GB *gb) {
  BASE(attack1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto afterFire;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, veranFairy_checkWithinBoundary_hook, SYM(veranFairy_checkWithinBoundary), b_+9);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, A);
  CYC(b_+21, b_+23); alu_add(gb, 0x06);
  CYC(b_+23, b_+24); alu_cp(gb, B);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x5a);
  if (!(F & FC)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x00);
  CYC(b_+31, b_+32); L = alu_dec8(gb, L);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0xb4);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x01);
  CYC(b_+38, b_+40); B = 0x37;
  CALL_C(b_+40, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+43);
  CYC(b_+43, b_+45); A = 0x06;
  CALL_C(b_+45, enemySetAnimation_hook, SYM(enemySetAnimation), b_+48);
afterFire:
  CYC(b_+48, b_+49); SET_HL(pop_effect(gb));
  CALL_C(b_+49, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+52);
  if (!(F & FZ)) {
    CYCT(b_+52, b_+55);
    TAIL(enemyAnimate);
  }
  CYC(b_+52, b_+55);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x78);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x00);
  CYC(b_+62, b_+64); A = 0x05;
  CYC(b_+64, b_+67);
  TAIL(enemySetAnimation);
}

void attack2_hook(GB *gb) {
  BASE(attack2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto afterFire;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, veranFairy_checkWithinBoundary_hook, SYM(veranFairy_checkWithinBoundary), b_+10);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x01);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x1e);
  CYC(b_+17, b_+19); B = 0x2f;
  CALL_C(b_+19, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+22);
  CYC(b_+22, b_+24); A = 0x06;
  CALL_C(b_+24, enemySetAnimation_hook, SYM(enemySetAnimation), b_+27);
afterFire:
  CYC(b_+27, b_+28); SET_HL(pop_effect(gb));
  CALL_C(b_+28, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+31);
  if (!(F & FZ)) {
    CYCT(b_+31, b_+34);
    TAIL(enemyAnimate);
  }
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0xf0);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x00);
  CYC(b_+41, b_+43); A = 0x05;
  CYC(b_+43, b_+46);
  TAIL(enemySetAnimation);
}

void veranFairy_66ed_hook(GB *gb) {
  BASE(veranFairy_66ed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (veranFairy_jump_table(gb));
    if (jt_ == SYM(attack0) && hook_is(gb, SYM(attack0), attack0_hook)) { attack0_hook(gb); return; }
    else if (jt_ == SYM(attack1) && hook_is(gb, SYM(attack1), attack1_hook)) { attack1_hook(gb); return; }
    else if (jt_ == SYM(attack2) && hook_is(gb, SYM(attack2), attack2_hook)) { attack2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFairy_checkLoopAroundScreen_hook(GB *gb) {
  BASE(veranFairy_checkLoopAroundScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+7); SET_HL(b_+28);
  CALL_C(b_+7, lookupKey_hook, SYM(lookupKey), b_+10);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_HL(b_+49);
  CYC(b_+14, b_+15);
  veranFairy_addAToHl_from_rst(gb, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); mem_wr(gb, hFF8F, A);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); mem_wr(gb, hFF8E, A);
  RET(b_+27); return;
}

void veranFairy_saveMovementPatternPointer_hook(GB *gb) {
  BASE(veranFairy_saveMovementPatternPointer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = H;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  RET(b_+7); return;
}

void veranFairy_animate_hook(GB *gb) {
  BASE(veranFairy_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(enemyAnimate);
}

void veranFairy_state0_hook(GB *gb) {
  BASE(veranFairy_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x06;
  CYC(b_+2, b_+5); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CALL_C(b_+5, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x3c);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x32);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); A = 0x02;
  CALL_C(b_+21, enemySetAnimation_hook, SYM(enemySetAnimation), b_+24);
  CYC(b_+24, b_+27);
  TAIL(objectSetVisible82);
}

void veranFairy_state1_hook(GB *gb) {
  BASE(veranFairy_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (veranFairy_jump_table(gb));
    if (jt_ == b_+29) { goto substate0; }
    else if (jt_ == b_+42) { goto substate1; }
    else if (jt_ == b_+54) { goto substate2; }
    else if (jt_ == b_+66) { goto substate3; }
    else if (jt_ == b_+87) { goto substate4; }
    else if (jt_ == b_+91) { goto substate5; }
    else if (jt_ == b_+95) { goto substate6; }
    else if (jt_ == b_+99) { goto substate7; }
    else if (jt_ == b_+103) { goto substate8; }
    else if (jt_ == b_+117) { goto substate9; }
    else if (jt_ == b_+151) { goto substateA; }
    else if (jt_ == b_+207) { goto substateB; }
    else if (jt_ == b_+224) { goto substateC; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+29, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+32);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+35);
    TAIL(ecom_flickerVisibility_b10);
  }
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x08);
  CYC(b_+37, b_+38); L = E;
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+39, b_+42);
  TAIL(objectSetVisible83);
substate1:
  CALL_C(b_+42, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+45);
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+47); L = E;
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+48, b_+51); SET_BC((SYM(ganon_stateC_substate8) + 24));
  CYC(b_+51, b_+54);
  TAIL(showText);
substate2:
  CALL_C(b_+54, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+57);
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x1e);
  CYC(b_+61, b_+63); A = 0x04;
  CYC(b_+63, b_+66);
  TAIL(enemySetAnimation);
substate3:
  CYC(b_+66, b_+68); C = 0x33;
strikeLightningAfterCountdown:
  CALL_C(b_+68, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+71);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x0a);
  CYC(b_+74, b_+75); L = E;
  CYC(b_+75, b_+76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
strikeLightning:
  CALL_C(b_+76, getFreePartSlot_hook, SYM(getFreePartSlot), b_+79);
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x27);
  CYC(b_+82, b_+84); L = PART_BASE + OBJ_YH;
  CYC(b_+84, b_+87);
  TAIL(setShortPosition_paramC);
substate4:
  CYC(b_+87, b_+89); C = 0x7b;
  CYC(b_+89, b_+91);
  goto strikeLightningAfterCountdown;
substate5:
  CYC(b_+91, b_+93); C = 0x55;
  CYC(b_+93, b_+95);
  goto strikeLightningAfterCountdown;
substate6:
  CYC(b_+95, b_+97); C = 0x3b;
  CYC(b_+97, b_+99);
  goto strikeLightningAfterCountdown;
substate7:
  CYC(b_+99, b_+101); C = 0x73;
  CYC(b_+101, b_+103);
  goto strikeLightningAfterCountdown;
substate8:
  CALL_C(b_+103, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+106);
  if (!(F & FZ)) { RET_TAKEN(b_+106); return; }
  CYC(b_+106, b_+107);
  CYC(b_+107, b_+108); L = E;
  CYC(b_+108, b_+109); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+109, b_+111); C = 0x59;
  CYC(b_+111, b_+114); push_effect(gb, b_+114);
  goto strikeLightning;
substate9:
  CYC(b_+117, b_+119); B = 0x0c;
  CYC(b_+119, b_+122); SET_HL(b_+139);
pillarLoop:
  CYC(b_+122, b_+123); push_effect(gb, BC);
  CYC(b_+123, b_+124); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+124, b_+125); C = A;
  CYC(b_+125, b_+127); A = 0xa5;
  CYC(b_+127, b_+128); push_effect(gb, HL);
  CALL_C(b_+128, setTile_hook, SYM(setTile), b_+131);
  CYC(b_+131, b_+132); SET_HL(pop_effect(gb));
  CYC(b_+132, b_+133); SET_BC(pop_effect(gb));
  CYC(b_+133, b_+134); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+134, b_+136);
    goto pillarLoop;
  }
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+139);
  TAIL(ecom_incSubstate_b10);
substateA:
  CYC(b_+151, b_+153); B = 0x04;
  CYC(b_+153, b_+156); SET_HL(b_+203);
nextMimic:
  CYC(b_+156, b_+157); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+157, b_+158); C = A;
  CYC(b_+158, b_+159); push_effect(gb, HL);
  CALL_C(b_+159, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+162);
  if (!(F & FZ)) {
    CYCT(b_+162, b_+164);
    goto afterMimicSpawn;
  }
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x64);
  CYC(b_+166, b_+168); L = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+168, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+171);
afterMimicSpawn:
  CYC(b_+171, b_+172); SET_HL(pop_effect(gb));
  CYC(b_+172, b_+173); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+173, b_+175);
    goto nextMimic;
  }
  CYC(b_+173, b_+175);
  CALL_C(b_+175, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+178);
  CYC(b_+178, b_+180); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0x1e);
  CYC(b_+182, b_+184); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+184, b_+185); alu_xor(gb, A);
  CYC(b_+185, b_+186); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+186, b_+187); mem_wr(gb, HL, A);
  CYC(b_+187, b_+189); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+189, b_+190); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+190, objectSetVisible83_hook, SYM(objectSetVisible83), b_+193);
  CYC(b_+193, b_+195); A = 0x05;
  CALL_C(b_+195, enemySetAnimation_hook, SYM(enemySetAnimation), b_+198);
  CYC(b_+198, b_+200); A = 0x04;
  CYC(b_+200, b_+203);
  TAIL(fadeinFromWhiteWithDelay);
substateB:
  CYC(b_+207, b_+210); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+210, b_+211); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CALL_C(b_+212, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+215);
  if (!(F & FZ)) { RET_TAKEN(b_+215); return; }
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+217); L = E;
  CYC(b_+217, b_+218); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+218, b_+221); SET_BC((SYM(ganon_stateC_substate8) + 25));
  CYC(b_+221, b_+224);
  TAIL(showText);
substateC:
  CYC(b_+224, b_+225); H = D;
  CYC(b_+225, b_+227); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+227, b_+228); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+228, b_+230); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+230, b_+232); mem_wr(gb, HL, 0x78);
  CYC(b_+232, b_+235);
  TAIL(enemyBoss_beginBoss_b10);
}

void veranFairy_state2_hook(GB *gb) {
  BASE(veranFairy_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+16); SET_HL(SYM(veranFairy_attackTable));
  CYC(b_+16, b_+17);
  veranFairy_addAToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CALL_C(b_+21, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x3c);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x00);
pickMovementPattern:
  CALL_C(b_+32, getRandomNumber_hook, SYM(getRandomNumber), b_+35);
  CYC(b_+35, b_+37); alu_and(gb, 0x03);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+39, b_+40); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+40, b_+42);
    goto pickMovementPattern;
  }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CYC(b_+43, b_+46); SET_HL(SYM(veranFairy_movementPatternTable));
  CYC(b_+46, b_+47);
  veranFairy_addDoubleIndexToHl_from_rst(gb, b_+47);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+48, b_+49); H = mem_rd(gb, HL);
  CYC(b_+49, b_+50); L = A;
  CYC(b_+50, b_+52); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); E = alu_inc8(gb, E);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  TAIL(veranFairy_saveMovementPatternPointer);
}

void veranFairy_state3_hook(GB *gb) {
  BASE(veranFairy_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, veranFairy_66ed_hook, SYM(veranFairy_66ed), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR33;
  CALL_C(b_+6, ecom_readPositionVars_b10_hook, SYM(ecom_readPositionVars_b10), b_+9);
  CYC(b_+9, b_+10); alu_sub(gb, C);
  CYC(b_+10, b_+12); alu_add(gb, 0x02);
  CYC(b_+12, b_+14); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(b_+14, b_+16);
    goto updateMovement;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = mem_rd(gb, hFF8F);
  CYC(b_+18, b_+19); alu_sub(gb, B);
  CYC(b_+19, b_+21); alu_add(gb, 0x02);
  CYC(b_+21, b_+23); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(b_+23, b_+25);
    goto updateMovement;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+27, b_+28); mem_wr(gb, HL, B);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+30, b_+31); mem_wr(gb, HL, C);
  CALL_C(b_+31, veranFairy_checkLoopAroundScreen_hook, SYM(veranFairy_checkLoopAroundScreen), b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+39); H = mem_rd(gb, HL);
  CYC(b_+39, b_+40); L = A;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+44);
    goto nextTarget;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); A = 0x05;
  CALL_C(b_+46, enemySetAnimation_hook, SYM(enemySetAnimation), b_+49);
  CYC(b_+49, b_+52);
  TAIL(ecom_incState_b10);
nextTarget:
  CYC(b_+52, b_+54); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+56); B = A;
  CYC(b_+56, b_+57); E = alu_inc8(gb, E);
  CYC(b_+57, b_+58); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+60); C = A;
  CALL_C(b_+60, veranFairy_saveMovementPatternPointer_hook, SYM(veranFairy_saveMovementPatternPointer), b_+63);
updateMovement:
  CALL_C(b_+63, ecom_moveTowardPosition_b10_hook, SYM(ecom_moveTowardPosition_b10), SYM(veranFairy_animate));
  TAIL(veranFairy_animate);
}

void veranFairy_state4_hook(GB *gb) {
  BASE(veranFairy_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    TAIL(veranFairy_animate);
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CYC(b_+9, b_+11);
  TAIL(veranFairy_animate);
}

void veranFairy_state5_hook(GB *gb) {
  BASE(veranFairy_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (veranFairy_jump_table(gb));
    if (jt_ == b_+9) { goto substate0; }
    else if (jt_ == b_+20) { goto substate1; }
    else if (jt_ == b_+33) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+9, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+12);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+15);
    TAIL(ecom_flickerVisibility_b10);
  }
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+20);
  TAIL(objectSetVisible82);
substate1:
  CALL_C(b_+20, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+23);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x41);
  CYC(b_+27, b_+30); SET_BC((SYM(ganon_stateC_substate8) + 27));
  CYC(b_+30, b_+33);
  TAIL(showText);
substate2:
  CALL_C(b_+33, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+36);
  if (F & FZ) {
    CYCT(b_+36, b_+38);
    goto triggerCutscene;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_and(gb, 0xf0);
  CYC(b_+45, b_+47); A = alu_swap(gb, A);
  CYC(b_+47, b_+48); A = alu_dec8(gb, A);
  CYC(b_+48, b_+49); push_effect(gb, AF);
  CYC(b_+49, b_+50); A = alu_dec8(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+50, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+53);
  } else {
    CYC(b_+50, b_+53);
  }
  SET_AF(pop_effect(gb));
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+57); SET_HL(b_+92);
  CYC(b_+57, b_+58);
  veranFairy_addDoubleIndexToHl_from_rst(gb, b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+60); C = mem_rd(gb, HL);
  CYC(b_+60, b_+61); B = A;
  CALL_C(b_+61, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+64);
  if (!(F & FZ)) { RET_TAKEN(b_+64); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x56);
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+70, b_+73);
  TAIL(objectCopyPositionWithOffset);
triggerCutscene:
  CYC(b_+73, b_+76); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; }
  CYC(b_+77, b_+78);
  CALL_C(b_+78, clearAllParentItems_hook, SYM(clearAllParentItems), b_+81);
  CALL_C(b_+81, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+84);
  CYC(b_+84, b_+86); A = 0x1f;
  CYC(b_+86, b_+89); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+89, b_+92);
  TAIL(enemyDelete);
}

void enemyCode06_hook(GB *gb) {
  BASE(enemyCode06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) {
    CYCT(b_+0, b_+2);
    goto normalStatus;
  }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto justHit;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+14);
  if (!(F & FC)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+20, b_+23); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x05);
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x00);
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x3c);
  CYC(b_+38, b_+40);
  goto normalStatus;
justHit:
  CALL_C(b_+40, veranFairy_updateVar35BasedOnHealth_hook, SYM(veranFairy_updateVar35BasedOnHealth), b_+43);
  CYC(b_+43, b_+46); SET_HL(SYM(veranFairy_speedTable));
  CYC(b_+46, b_+47);
  veranFairy_addAToHl_from_rst(gb, b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
normalStatus:
  CYC(b_+51, b_+53); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); push_effect(gb, b_+55);
  do { uint16_t jt_ = (veranFairy_jump_table(gb));
    if (jt_ == SYM(veranFairy_state0) && hook_is(gb, SYM(veranFairy_state0), veranFairy_state0_hook)) { veranFairy_state0_hook(gb); return; }
    else if (jt_ == SYM(veranFairy_state1) && hook_is(gb, SYM(veranFairy_state1), veranFairy_state1_hook)) { veranFairy_state1_hook(gb); return; }
    else if (jt_ == SYM(veranFairy_state2) && hook_is(gb, SYM(veranFairy_state2), veranFairy_state2_hook)) { veranFairy_state2_hook(gb); return; }
    else if (jt_ == SYM(veranFairy_state3) && hook_is(gb, SYM(veranFairy_state3), veranFairy_state3_hook)) { veranFairy_state3_hook(gb); return; }
    else if (jt_ == SYM(veranFairy_state4) && hook_is(gb, SYM(veranFairy_state4), veranFairy_state4_hook)) { veranFairy_state4_hook(gb); return; }
    else if (jt_ == SYM(veranFairy_state5) && hook_is(gb, SYM(veranFairy_state5), veranFairy_state5_hook)) { veranFairy_state5_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
