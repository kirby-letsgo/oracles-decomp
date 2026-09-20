#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67be, 0x67c0); E = ENEMY_BASE + OBJ_YH;
  CYC(0x67c0, 0x67c1); A = mem_rd(gb, DE);
  CYC(0x67c1, 0x67c3); alu_sub(gb, 0x10);
  CYC(0x67c3, 0x67c5); alu_cp(gb, 0x90);
  if (!(F & FC)) { RET_TAKEN(0x67c5); return; }
  CYC(0x67c5, 0x67c6);
  CYC(0x67c6, 0x67c8); E = ENEMY_BASE + OBJ_XH;
  CYC(0x67c8, 0x67c9); A = mem_rd(gb, DE);
  CYC(0x67c9, 0x67cb); alu_sub(gb, 0x10);
  CYC(0x67cb, 0x67cd); alu_cp(gb, 0xd0);
  RET(0x67cd); return;
}

void veranFairy_updateVar35BasedOnHealth_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66d9, 0x66db); B = 0x00;
  CYC(0x66db, 0x66dd); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x66dd, 0x66de); A = mem_rd(gb, DE);
  CYC(0x66de, 0x66e0); alu_cp(gb, 0x14);
  if (!(F & FC)) {
    CYCT(0x66e0, 0x66e2);
    goto writeVar35;
  }
  CYC(0x66e0, 0x66e2);
  CYC(0x66e2, 0x66e3); B = alu_inc8(gb, B);
  CYC(0x66e3, 0x66e5); alu_cp(gb, 0x0a);
  if (!(F & FC)) {
    CYCT(0x66e5, 0x66e7);
    goto writeVar35;
  }
  CYC(0x66e5, 0x66e7);
  CYC(0x66e7, 0x66e8); B = alu_inc8(gb, B);
writeVar35:
  CYC(0x66e8, 0x66ea); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x66ea, 0x66eb); A = B;
  CYC(0x66eb, 0x66ec); mem_wr(gb, DE, A);
  RET(0x66ec); return;
}

void attack0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66fb, 0x66fd); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x66fd, 0x66fe); A = mem_rd(gb, DE);
  CYC(0x66fe, 0x66ff); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x66ff, 0x6701);
    goto decCounter;
  }
  CYC(0x66ff, 0x6701);
  CALL_C(0x6701, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6704);
  CYC(0x6704, 0x6706); alu_and(gb, 0x0f);
  CYC(0x6706, 0x6707); B = A;
  CYC(0x6707, 0x6708); H = D;
  CYC(0x6708, 0x670a); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x670a, 0x670b); A = mem_rd(gb, HL);
  CYC(0x670b, 0x670c); alu_add(gb, A);
  CYC(0x670c, 0x670e); alu_add(gb, 0x08);
  CYC(0x670e, 0x670f); alu_cp(gb, B);
  CYC(0x670f, 0x6711); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6711, 0x6713); mem_wr(gb, HL, 0x3c);
  if (!(F & FC)) { RET_TAKEN(0x6713); return; }
  CYC(0x6713, 0x6714);
  CYC(0x6714, 0x6715); alu_xor(gb, A);
  CYC(0x6715, 0x6716); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6716, 0x6717); A = alu_inc8(gb, A);
  CYC(0x6717, 0x6718); mem_wr(gb, HL, A);
  CYC(0x6718, 0x671a); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x671a, 0x671b); mem_wr(gb, HL, A);
  CYC(0x671b, 0x671d); L = ENEMY_BASE + OBJ_VAR37;
  CYC(0x671d, 0x671f); mem_wr(gb, HL, 0x04);
decCounter:
  CALL_C(0x671f, ecom_decCounter1_b10_hook, 0x439a, 0x6722);
  if (F & FZ) {
    CYCT(0x6722, 0x6724);
    goto checkBoundary;
  }
  CYC(0x6722, 0x6724);
  CYC(0x6724, 0x6725); A = mem_rd(gb, HL);
  CYC(0x6725, 0x6727); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(0x6727); return; }
  CYC(0x6727, 0x6728);
  CYC(0x6728, 0x672a); A = 0x05;
  CYC(0x672a, 0x672d);
  enemySetAnimation_hook(gb);
  return;
checkBoundary:
  CALL_C(0x672d, veranFairy_checkWithinBoundary_hook, 0x67be, 0x6730);
  if (!(F & FC)) { RET_TAKEN(0x6730); return; }
  CYC(0x6730, 0x6731);
  CYC(0x6731, 0x6733); L = ENEMY_BASE + OBJ_VAR37;
  CYC(0x6733, 0x6734); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYCT(0x6734, 0x6736);
    goto resetCounters;
  }
  CYC(0x6734, 0x6736);
  CYC(0x6736, 0x6738); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6738, 0x673a); mem_wr(gb, HL, 0x1e);
  CYC(0x673a, 0x673c); B = 0x2d;
  CALL_C(0x673c, ecom_spawnProjectile_b10_hook, 0x437c, 0x673f);
  CYC(0x673f, 0x6741); A = 0x06;
  CYC(0x6741, 0x6744);
  enemySetAnimation_hook(gb);
  return;
resetCounters:
  CYC(0x6744, 0x6746); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6746, 0x6748); mem_wr(gb, HL, 0x5a);
  CYC(0x6748, 0x674a); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x674a, 0x674c); mem_wr(gb, HL, 0x00);
  RET(0x674c); return;
}

void attack1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x674d, 0x674f); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x674f, 0x6750); A = mem_rd(gb, DE);
  CYC(0x6750, 0x6751); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6751, 0x6753);
    goto afterFire;
  }
  CYC(0x6751, 0x6753);
  CALL_C(0x6753, veranFairy_checkWithinBoundary_hook, 0x67be, 0x6756);
  if (!(F & FC)) { RET_TAKEN(0x6756); return; }
  CYC(0x6756, 0x6757);
  CALL_C(0x6757, getRandomNumber_noPreserveVars_hook, 0x0453, 0x675a);
  CYC(0x675a, 0x675c); alu_and(gb, 0x0f);
  CYC(0x675c, 0x675d); B = A;
  CYC(0x675d, 0x675e); H = D;
  CYC(0x675e, 0x6760); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x6760, 0x6761); A = mem_rd(gb, HL);
  CYC(0x6761, 0x6762); alu_add(gb, A);
  CYC(0x6762, 0x6764); alu_add(gb, 0x06);
  CYC(0x6764, 0x6765); alu_cp(gb, B);
  CYC(0x6765, 0x6767); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6767, 0x6769); mem_wr(gb, HL, 0x5a);
  if (!(F & FC)) { RET_TAKEN(0x6769); return; }
  CYC(0x6769, 0x676a);
  CYC(0x676a, 0x676c); mem_wr(gb, HL, 0x00);
  CYC(0x676c, 0x676d); L = alu_dec8(gb, L);
  CYC(0x676d, 0x676f); mem_wr(gb, HL, 0xb4);
  CYC(0x676f, 0x6771); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6771, 0x6773); mem_wr(gb, HL, 0x01);
  CYC(0x6773, 0x6775); B = 0x37;
  CALL_C(0x6775, ecom_spawnProjectile_b10_hook, 0x437c, 0x6778);
  CYC(0x6778, 0x677a); A = 0x06;
  CALL_C(0x677a, enemySetAnimation_hook, 0x282b, 0x677d);
afterFire:
  CYC(0x677d, 0x677e); SET_HL(pop_effect(gb));
  CALL_C(0x677e, ecom_decCounter1_b10_hook, 0x439a, 0x6781);
  if (!(F & FZ)) {
    CYCT(0x6781, 0x6784);
    enemyAnimate_hook(gb);
    return;
  }
  CYC(0x6781, 0x6784);
  CYC(0x6784, 0x6785); L = alu_inc8(gb, L);
  CYC(0x6785, 0x6787); mem_wr(gb, HL, 0x78);
  CYC(0x6787, 0x6789); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6789, 0x678b); mem_wr(gb, HL, 0x00);
  CYC(0x678b, 0x678d); A = 0x05;
  CYC(0x678d, 0x6790);
  enemySetAnimation_hook(gb);
}

void attack2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6790, 0x6791); H = D;
  CYC(0x6791, 0x6793); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6793, 0x6795); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x6795, 0x6797);
    goto afterFire;
  }
  CYC(0x6795, 0x6797);
  CALL_C(0x6797, veranFairy_checkWithinBoundary_hook, 0x67be, 0x679a);
  if (!(F & FC)) { RET_TAKEN(0x679a); return; }
  CYC(0x679a, 0x679b);
  CYC(0x679b, 0x679d); mem_wr(gb, HL, 0x01);
  CYC(0x679d, 0x679f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x679f, 0x67a1); mem_wr(gb, HL, 0x1e);
  CYC(0x67a1, 0x67a3); B = 0x2f;
  CALL_C(0x67a3, ecom_spawnProjectile_b10_hook, 0x437c, 0x67a6);
  CYC(0x67a6, 0x67a8); A = 0x06;
  CALL_C(0x67a8, enemySetAnimation_hook, 0x282b, 0x67ab);
afterFire:
  CYC(0x67ab, 0x67ac); SET_HL(pop_effect(gb));
  CALL_C(0x67ac, ecom_decCounter1_b10_hook, 0x439a, 0x67af);
  if (!(F & FZ)) {
    CYCT(0x67af, 0x67b2);
    enemyAnimate_hook(gb);
    return;
  }
  CYC(0x67af, 0x67b2);
  CYC(0x67b2, 0x67b3); L = alu_inc8(gb, L);
  CYC(0x67b3, 0x67b5); mem_wr(gb, HL, 0xf0);
  CYC(0x67b5, 0x67b7); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x67b7, 0x67b9); mem_wr(gb, HL, 0x00);
  CYC(0x67b9, 0x67bb); A = 0x05;
  CYC(0x67bb, 0x67be);
  enemySetAnimation_hook(gb);
}

void veranFairy_66ed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66ed, ecom_decCounter2_b10_hook, 0x43a3, 0x66f0);
  if (!(F & FZ)) { RET_TAKEN(0x66f0); return; }
  CYC(0x66f0, 0x66f1);
  CYC(0x66f1, 0x66f3); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x66f3, 0x66f4); A = mem_rd(gb, DE);
  CYC(0x66f4, 0x66f5); push_effect(gb, 0x66f5);
  switch (veranFairy_jump_table(gb)) {
    case 0x66fb: attack0_hook(gb); return;
    case 0x674d: attack1_hook(gb); return;
    case 0x6790: attack2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFairy_checkLoopAroundScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6698, objectGetShortPosition_hook, 0x2096, 0x669b);
  CYC(0x669b, 0x669c); E = A;
  CYC(0x669c, 0x669f); SET_HL(0x66b4);
  CALL_C(0x669f, lookupKey_hook, 0x1e06, 0x66a2);
  if (!(F & FC)) { RET_TAKEN(0x66a2); return; }
  CYC(0x66a2, 0x66a3);
  CYC(0x66a3, 0x66a6); SET_HL(0x66c9);
  CYC(0x66a6, 0x66a7);
  veranFairy_addAToHl_from_rst(gb, 0x66a7);
  CYC(0x66a7, 0x66a9); E = ENEMY_BASE + OBJ_YH;
  CYC(0x66a9, 0x66aa); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x66aa, 0x66ab); mem_wr(gb, DE, A);
  CYC(0x66ab, 0x66ad); mem_wr(gb, 0xff8f, A);
  CYC(0x66ad, 0x66af); E = ENEMY_BASE + OBJ_XH;
  CYC(0x66af, 0x66b0); A = mem_rd(gb, HL);
  CYC(0x66b0, 0x66b1); mem_wr(gb, DE, A);
  CYC(0x66b1, 0x66b3); mem_wr(gb, 0xff8e, A);
  RET(0x66b3); return;
}

void veranFairy_saveMovementPatternPointer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x658f, 0x6591); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x6591, 0x6592); A = L;
  CYC(0x6592, 0x6593); mem_wr(gb, DE, A);
  CYC(0x6593, 0x6594); E = alu_inc8(gb, E);
  CYC(0x6594, 0x6595); A = H;
  CYC(0x6595, 0x6596); mem_wr(gb, DE, A);
  RET(0x6596); return;
}

void veranFairy_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65d9, 0x65dc);
  enemyAnimate_hook(gb);
}

void veranFairy_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6450, 0x6452); A = 0x06;
  CYC(0x6452, 0x6455); mem_wr(gb, 0xcc1d, A);
  CALL_C(0x6455, ecom_incState_b10_hook, 0x4000, 0x6458);
  CYC(0x6458, 0x645a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x645a, 0x645c); mem_wr(gb, HL, 0x3c);
  CYC(0x645c, 0x645e); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x645e, 0x6460); mem_wr(gb, HL, 0x32);
  CYC(0x6460, 0x6462); L = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(0x6462, 0x6463); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6463, 0x6465); A = 0x02;
  CALL_C(0x6465, enemySetAnimation_hook, 0x282b, 0x6468);
  CYC(0x6468, 0x646b);
  objectSetVisible82_hook(gb);
}

void veranFairy_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x646b, 0x646c); E = alu_inc8(gb, E);
  CYC(0x646c, 0x646d); A = mem_rd(gb, DE);
  CYC(0x646d, 0x646e); push_effect(gb, 0x646e);
  switch (veranFairy_jump_table(gb)) {
    case 0x6488: goto substate0;
    case 0x6495: goto substate1;
    case 0x64a1: goto substate2;
    case 0x64ad: goto substate3;
    case 0x64c2: goto substate4;
    case 0x64c6: goto substate5;
    case 0x64ca: goto substate6;
    case 0x64ce: goto substate7;
    case 0x64d2: goto substate8;
    case 0x64e0: goto substate9;
    case 0x6502: goto substateA;
    case 0x653a: goto substateB;
    case 0x654b: goto substateC;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x6488, ecom_decCounter1_b10_hook, 0x439a, 0x648b);
  if (!(F & FZ)) {
    CYCT(0x648b, 0x648e);
    ecom_flickerVisibility_b10_hook(gb);
    return;
  }
  CYC(0x648b, 0x648e);
  CYC(0x648e, 0x6490); mem_wr(gb, HL, 0x08);
  CYC(0x6490, 0x6491); L = E;
  CYC(0x6491, 0x6492); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6492, 0x6495);
  objectSetVisible83_hook(gb);
  return;
substate1:
  CALL_C(0x6495, ecom_decCounter1_b10_hook, 0x439a, 0x6498);
  if (!(F & FZ)) { RET_TAKEN(0x6498); return; }
  CYC(0x6498, 0x6499);
  CYC(0x6499, 0x649a); L = E;
  CYC(0x649a, 0x649b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x649b, 0x649e); SET_BC(0x560f);
  CYC(0x649e, 0x64a1);
  showText_hook(gb);
  return;
substate2:
  CALL_C(0x64a1, ecom_incSubstate_b10_hook, 0x4005, 0x64a4);
  CYC(0x64a4, 0x64a6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x64a6, 0x64a8); mem_wr(gb, HL, 0x1e);
  CYC(0x64a8, 0x64aa); A = 0x04;
  CYC(0x64aa, 0x64ad);
  enemySetAnimation_hook(gb);
  return;
substate3:
  CYC(0x64ad, 0x64af); C = 0x33;
strikeLightningAfterCountdown:
  CALL_C(0x64af, ecom_decCounter1_b10_hook, 0x439a, 0x64b2);
  if (!(F & FZ)) { RET_TAKEN(0x64b2); return; }
  CYC(0x64b2, 0x64b3);
  CYC(0x64b3, 0x64b5); mem_wr(gb, HL, 0x0a);
  CYC(0x64b5, 0x64b6); L = E;
  CYC(0x64b6, 0x64b7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
strikeLightning:
  CALL_C(0x64b7, getFreePartSlot_hook, 0x3e8e, 0x64ba);
  if (!(F & FZ)) { RET_TAKEN(0x64ba); return; }
  CYC(0x64ba, 0x64bb);
  CYC(0x64bb, 0x64bd); mem_wr(gb, HL, 0x27);
  CYC(0x64bd, 0x64bf); L = PART_BASE + OBJ_YH;
  CYC(0x64bf, 0x64c2);
  setShortPosition_paramC_hook(gb);
  return;
substate4:
  CYC(0x64c2, 0x64c4); C = 0x7b;
  CYC(0x64c4, 0x64c6);
  goto strikeLightningAfterCountdown;
substate5:
  CYC(0x64c6, 0x64c8); C = 0x55;
  CYC(0x64c8, 0x64ca);
  goto strikeLightningAfterCountdown;
substate6:
  CYC(0x64ca, 0x64cc); C = 0x3b;
  CYC(0x64cc, 0x64ce);
  goto strikeLightningAfterCountdown;
substate7:
  CYC(0x64ce, 0x64d0); C = 0x73;
  CYC(0x64d0, 0x64d2);
  goto strikeLightningAfterCountdown;
substate8:
  CALL_C(0x64d2, ecom_decCounter1_b10_hook, 0x439a, 0x64d5);
  if (!(F & FZ)) { RET_TAKEN(0x64d5); return; }
  CYC(0x64d5, 0x64d6);
  CYC(0x64d6, 0x64d7); L = E;
  CYC(0x64d7, 0x64d8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x64d8, 0x64da); C = 0x59;
  CYC(0x64da, 0x64dd); push_effect(gb, 0x64dd);
  goto strikeLightning;
substate9:
  CYC(0x64e0, 0x64e2); B = 0x0c;
  CYC(0x64e2, 0x64e5); SET_HL(0x64f6);
pillarLoop:
  CYC(0x64e5, 0x64e6); push_effect(gb, BC);
  CYC(0x64e6, 0x64e7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x64e7, 0x64e8); C = A;
  CYC(0x64e8, 0x64ea); A = 0xa5;
  CYC(0x64ea, 0x64eb); push_effect(gb, HL);
  CALL_C(0x64eb, setTile_hook, 0x3a9c, 0x64ee);
  CYC(0x64ee, 0x64ef); SET_HL(pop_effect(gb));
  CYC(0x64ef, 0x64f0); SET_BC(pop_effect(gb));
  CYC(0x64f0, 0x64f1); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x64f1, 0x64f3);
    goto pillarLoop;
  }
  CYC(0x64f1, 0x64f3);
  CYC(0x64f3, 0x64f6);
  ecom_incSubstate_b10_hook(gb);
  return;
substateA:
  CYC(0x6502, 0x6504); B = 0x04;
  CYC(0x6504, 0x6507); SET_HL(0x6536);
nextMimic:
  CYC(0x6507, 0x6508); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6508, 0x6509); C = A;
  CYC(0x6509, 0x650a); push_effect(gb, HL);
  CALL_C(0x650a, getFreeEnemySlot_hook, 0x2e27, 0x650d);
  if (!(F & FZ)) {
    CYCT(0x650d, 0x650f);
    goto afterMimicSpawn;
  }
  CYC(0x650d, 0x650f);
  CYC(0x650f, 0x6511); mem_wr(gb, HL, 0x64);
  CYC(0x6511, 0x6513); L = ENEMY_BASE + OBJ_YH;
  CALL_C(0x6513, setShortPosition_paramC_hook, 0x20b9, 0x6516);
afterMimicSpawn:
  CYC(0x6516, 0x6517); SET_HL(pop_effect(gb));
  CYC(0x6517, 0x6518); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6518, 0x651a);
    goto nextMimic;
  }
  CYC(0x6518, 0x651a);
  CALL_C(0x651a, ecom_incSubstate_b10_hook, 0x4005, 0x651d);
  CYC(0x651d, 0x651f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x651f, 0x6521); mem_wr(gb, HL, 0x1e);
  CYC(0x6521, 0x6523); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x6523, 0x6524); alu_xor(gb, A);
  CYC(0x6524, 0x6525); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6525, 0x6526); mem_wr(gb, HL, A);
  CYC(0x6526, 0x6528); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6528, 0x6529); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x6529, objectSetVisible83_hook, 0x1e72, 0x652c);
  CYC(0x652c, 0x652e); A = 0x05;
  CALL_C(0x652e, enemySetAnimation_hook, 0x282b, 0x6531);
  CYC(0x6531, 0x6533); A = 0x04;
  CYC(0x6533, 0x6536);
  fadeinFromWhiteWithDelay_hook(gb);
  return;
substateB:
  CYC(0x653a, 0x653d); A = mem_rd(gb, 0xc4ab);
  CYC(0x653d, 0x653e); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x653e); return; }
  CYC(0x653e, 0x653f);
  CALL_C(0x653f, ecom_decCounter1_b10_hook, 0x439a, 0x6542);
  if (!(F & FZ)) { RET_TAKEN(0x6542); return; }
  CYC(0x6542, 0x6543);
  CYC(0x6543, 0x6544); L = E;
  CYC(0x6544, 0x6545); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6545, 0x6548); SET_BC(0x5610);
  CYC(0x6548, 0x654b);
  showText_hook(gb);
  return;
substateC:
  CYC(0x654b, 0x654c); H = D;
  CYC(0x654c, 0x654e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x654e, 0x654f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x654f, 0x6551); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6551, 0x6553); mem_wr(gb, HL, 0x78);
  CYC(0x6553, 0x6556);
  enemyBoss_beginBoss_b10_hook(gb);
}

void veranFairy_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6556, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6559);
  CYC(0x6559, 0x655b); alu_and(gb, 0x07);
  CYC(0x655b, 0x655c); B = A;
  CYC(0x655c, 0x655e); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x655e, 0x655f); A = mem_rd(gb, DE);
  CYC(0x655f, 0x6561); A = alu_swap(gb, A);
  CYC(0x6561, 0x6562); alu_rrca(gb);
  CYC(0x6562, 0x6563); alu_add(gb, B);
  CYC(0x6563, 0x6566); SET_HL(0x667d);
  CYC(0x6566, 0x6567);
  veranFairy_addAToHl_from_rst(gb, 0x6567);
  CYC(0x6567, 0x6569); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x6569, 0x656a); A = mem_rd(gb, HL);
  CYC(0x656a, 0x656b); mem_wr(gb, DE, A);
  CALL_C(0x656b, ecom_incState_b10_hook, 0x4000, 0x656e);
  CYC(0x656e, 0x6570); L = ENEMY_BASE + OBJ_VAR38;
  CYC(0x6570, 0x6572); mem_wr(gb, HL, 0x3c);
  CYC(0x6572, 0x6574); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6574, 0x6576); mem_wr(gb, HL, 0x00);
pickMovementPattern:
  CALL_C(0x6576, getRandomNumber_hook, 0x043e, 0x6579);
  CYC(0x6579, 0x657b); alu_and(gb, 0x03);
  CYC(0x657b, 0x657d); L = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(0x657d, 0x657e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x657e, 0x6580);
    goto pickMovementPattern;
  }
  CYC(0x657e, 0x6580);
  CYC(0x6580, 0x6581); mem_wr(gb, HL, A);
  CYC(0x6581, 0x6584); SET_HL(0x664b);
  CYC(0x6584, 0x6585);
  veranFairy_addDoubleIndexToHl_from_rst(gb, 0x6585);
  CYC(0x6585, 0x6586); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6586, 0x6587); H = mem_rd(gb, HL);
  CYC(0x6587, 0x6588); L = A;
  CYC(0x6588, 0x658a); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x658a, 0x658b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x658b, 0x658c); mem_wr(gb, DE, A);
  CYC(0x658c, 0x658d); E = alu_inc8(gb, E);
  CYC(0x658d, 0x658e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x658e, 0x658f); mem_wr(gb, DE, A);
  veranFairy_saveMovementPatternPointer_hook(gb);
}

void veranFairy_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6597, veranFairy_66ed_hook, 0x66ed, 0x659a);
  CYC(0x659a, 0x659b); H = D;
  CYC(0x659b, 0x659d); L = ENEMY_BASE + OBJ_VAR33;
  CALL_C(0x659d, ecom_readPositionVars_b10_hook, 0x4439, 0x65a0);
  CYC(0x65a0, 0x65a1); alu_sub(gb, C);
  CYC(0x65a1, 0x65a3); alu_add(gb, 0x02);
  CYC(0x65a3, 0x65a5); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(0x65a5, 0x65a7);
    goto updateMovement;
  }
  CYC(0x65a5, 0x65a7);
  CYC(0x65a7, 0x65a9); A = mem_rd(gb, 0xff8f);
  CYC(0x65a9, 0x65aa); alu_sub(gb, B);
  CYC(0x65aa, 0x65ac); alu_add(gb, 0x02);
  CYC(0x65ac, 0x65ae); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(0x65ae, 0x65b0);
    goto updateMovement;
  }
  CYC(0x65ae, 0x65b0);
  CYC(0x65b0, 0x65b2); L = ENEMY_BASE + OBJ_YH;
  CYC(0x65b2, 0x65b3); mem_wr(gb, HL, B);
  CYC(0x65b3, 0x65b5); L = ENEMY_BASE + OBJ_XH;
  CYC(0x65b5, 0x65b6); mem_wr(gb, HL, C);
  CALL_C(0x65b6, veranFairy_checkLoopAroundScreen_hook, 0x6698, 0x65b9);
  CYC(0x65b9, 0x65ba); H = D;
  CYC(0x65ba, 0x65bc); L = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x65bc, 0x65bd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65bd, 0x65be); H = mem_rd(gb, HL);
  CYC(0x65be, 0x65bf); L = A;
  CYC(0x65bf, 0x65c0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65c0, 0x65c1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x65c1, 0x65c3);
    goto nextTarget;
  }
  CYC(0x65c1, 0x65c3);
  CYC(0x65c3, 0x65c5); A = 0x05;
  CALL_C(0x65c5, enemySetAnimation_hook, 0x282b, 0x65c8);
  CYC(0x65c8, 0x65cb);
  ecom_incState_b10_hook(gb);
  return;
nextTarget:
  CYC(0x65cb, 0x65cd); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x65cd, 0x65ce); mem_wr(gb, DE, A);
  CYC(0x65ce, 0x65cf); B = A;
  CYC(0x65cf, 0x65d0); E = alu_inc8(gb, E);
  CYC(0x65d0, 0x65d1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65d1, 0x65d2); mem_wr(gb, DE, A);
  CYC(0x65d2, 0x65d3); C = A;
  CALL_C(0x65d3, veranFairy_saveMovementPatternPointer_hook, 0x658f, 0x65d6);
updateMovement:
  CALL_C(0x65d6, ecom_moveTowardPosition_b10_hook, 0x4430, 0x65d9);
  veranFairy_animate_hook(gb);
}

void veranFairy_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65dc, 0x65dd); H = D;
  CYC(0x65dd, 0x65df); L = ENEMY_BASE + OBJ_VAR38;
  CYC(0x65df, 0x65e0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x65e0, 0x65e2);
    veranFairy_animate_hook(gb);
    return;
  }
  CYC(0x65e0, 0x65e2);
  CYC(0x65e2, 0x65e3); L = E;
  CYC(0x65e3, 0x65e5); mem_wr(gb, HL, 0x02);
  CYC(0x65e5, 0x65e7);
  veranFairy_animate_hook(gb);
}

void veranFairy_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65e7, 0x65e8); E = alu_inc8(gb, E);
  CYC(0x65e8, 0x65e9); A = mem_rd(gb, DE);
  CYC(0x65e9, 0x65ea); push_effect(gb, 0x65ea);
  switch (veranFairy_jump_table(gb)) {
    case 0x65f0: goto substate0;
    case 0x65fb: goto substate1;
    case 0x6608: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x65f0, ecom_decCounter1_b10_hook, 0x439a, 0x65f3);
  if (!(F & FZ)) {
    CYCT(0x65f3, 0x65f6);
    ecom_flickerVisibility_b10_hook(gb);
    return;
  }
  CYC(0x65f3, 0x65f6);
  CYC(0x65f6, 0x65f7); L = E;
  CYC(0x65f7, 0x65f8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x65f8, 0x65fb);
  objectSetVisible82_hook(gb);
  return;
substate1:
  CALL_C(0x65fb, ecom_incSubstate_b10_hook, 0x4005, 0x65fe);
  CYC(0x65fe, 0x6600); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6600, 0x6602); mem_wr(gb, HL, 0x41);
  CYC(0x6602, 0x6605); SET_BC(0x5612);
  CYC(0x6605, 0x6608);
  showText_hook(gb);
  return;
substate2:
  CALL_C(0x6608, ecom_decCounter2_b10_hook, 0x43a3, 0x660b);
  if (F & FZ) {
    CYCT(0x660b, 0x660d);
    goto triggerCutscene;
  }
  CYC(0x660b, 0x660d);
  CYC(0x660d, 0x660e); A = mem_rd(gb, HL);
  CYC(0x660e, 0x6610); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x6610); return; }
  CYC(0x6610, 0x6611);
  CYC(0x6611, 0x6612); A = mem_rd(gb, HL);
  CYC(0x6612, 0x6614); alu_and(gb, 0xf0);
  CYC(0x6614, 0x6616); A = alu_swap(gb, A);
  CYC(0x6616, 0x6617); A = alu_dec8(gb, A);
  CYC(0x6617, 0x6618); push_effect(gb, AF);
  CYC(0x6618, 0x6619); A = alu_dec8(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x6619, fadeoutToWhite_hook, 0x326c, 0x661c);
  } else {
    CYC(0x6619, 0x661c);
  }
  SET_AF(pop_effect(gb));
  CYC(0x661c, 0x661d);
  CYC(0x661d, 0x6620); SET_HL(0x6643);
  CYC(0x6620, 0x6621);
  veranFairy_addDoubleIndexToHl_from_rst(gb, 0x6621);
  CYC(0x6621, 0x6622); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6622, 0x6623); C = mem_rd(gb, HL);
  CYC(0x6623, 0x6624); B = A;
  CALL_C(0x6624, getFreeInteractionSlot_hook, 0x3aef, 0x6627);
  if (!(F & FZ)) { RET_TAKEN(0x6627); return; }
  CYC(0x6627, 0x6628);
  CYC(0x6628, 0x662a); mem_wr(gb, HL, 0x56);
  CYC(0x662a, 0x662c); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x662c, 0x662d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x662d, 0x6630);
  objectCopyPositionWithOffset_hook(gb);
  return;
triggerCutscene:
  CYC(0x6630, 0x6633); A = mem_rd(gb, 0xc4ab);
  CYC(0x6633, 0x6634); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6634); return; }
  CYC(0x6634, 0x6635);
  CALL_C(0x6635, clearAllParentItems_hook, 0x2c10, 0x6638);
  CALL_C(0x6638, dropLinkHeldItem_hook, 0x2c43, 0x663b);
  CYC(0x663b, 0x663d); A = 0x1f;
  CYC(0x663d, 0x6640); mem_wr(gb, 0xcc04, A);
  CYC(0x6640, 0x6643);
  enemyDelete_hook(gb);
}

void enemyCode06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) {
    CYCT(0x640d, 0x640f);
    goto normalStatus;
  }
  CYC(0x640d, 0x640f);
  CYC(0x640f, 0x6411); alu_sub(gb, 0x03);
  if (F & FC) { RET_TAKEN(0x6411); return; }
  CYC(0x6411, 0x6412);
  if (!(F & FZ)) {
    CYCT(0x6412, 0x6414);
    goto justHit;
  }
  CYC(0x6412, 0x6414);
  CYC(0x6414, 0x6416); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x6416, 0x6417); A = mem_rd(gb, DE);
  if (!(F & FZ)) { RET_TAKEN(0x6417); return; }
  CYC(0x6417, 0x6418);
  CALL_C(0x6418, checkLinkCollisionsEnabled_hook, 0x1d32, 0x641b);
  if (!(F & FC)) { RET_TAKEN(0x641b); return; }
  CYC(0x641b, 0x641c);
  CYC(0x641c, 0x641e); A = 0x01;
  CYC(0x641e, 0x6421); mem_wr(gb, 0xcc8a, A);
  CYC(0x6421, 0x6424); mem_wr(gb, 0xcc02, A);
  CYC(0x6424, 0x6425); H = D;
  CYC(0x6425, 0x6427); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6427, 0x6428); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6428, 0x642a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x642a, 0x642c); mem_wr(gb, HL, 0x05);
  CYC(0x642c, 0x642d); L = alu_inc8(gb, L);
  CYC(0x642d, 0x642f); mem_wr(gb, HL, 0x00);
  CYC(0x642f, 0x6431); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6431, 0x6433); mem_wr(gb, HL, 0x3c);
  CYC(0x6433, 0x6435);
  goto normalStatus;
justHit:
  CALL_C(0x6435, veranFairy_updateVar35BasedOnHealth_hook, 0x66d9, 0x6438);
  CYC(0x6438, 0x643b); SET_HL(0x6695);
  CYC(0x643b, 0x643c);
  veranFairy_addAToHl_from_rst(gb, 0x643c);
  CYC(0x643c, 0x643e); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x643e, 0x643f); A = mem_rd(gb, HL);
  CYC(0x643f, 0x6440); mem_wr(gb, DE, A);
normalStatus:
  CYC(0x6440, 0x6442); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6442, 0x6443); A = mem_rd(gb, DE);
  CYC(0x6443, 0x6444); push_effect(gb, 0x6444);
  switch (veranFairy_jump_table(gb)) {
    case 0x6450: veranFairy_state0_hook(gb); return;
    case 0x646b: veranFairy_state1_hook(gb); return;
    case 0x6556: veranFairy_state2_hook(gb); return;
    case 0x6597: veranFairy_state3_hook(gb); return;
    case 0x65dc: veranFairy_state4_hook(gb); return;
    case 0x65e7: veranFairy_state5_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
