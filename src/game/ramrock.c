#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/enemies/ramrock.s (ENEMY_RAMROCK), bank $10.

static uint16_t ramrock_jump_table(GB *gb) {
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

void ramrock_updateHorizontalMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6c3f, ecom_applyVelocityForSideviewEnemy_b10_hook, 0x4153, 0x6c42);
  if (!(F & FZ)) { RET_TAKEN(0x6c42); return; }
  CYC(0x6c42, 0x6c43);
  CYC(0x6c43, 0x6c45); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6c45, 0x6c46); A = mem_rd(gb, DE);
  CYC(0x6c46, 0x6c48); alu_xor(gb, 0x10);
  CYC(0x6c48, 0x6c49); mem_wr(gb, DE, A);
  CYC(0x6c49, 0x6c4a); alu_xor(gb, A);
  RET(0x6c4a); return;
}

void ramrock_glovePhase_reverseDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c4b, 0x6c4c); H = D;
  CYC(0x6c4c, 0x6c4e); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6c4e, 0x6c50); A = 0xc0;
  CYC(0x6c50, 0x6c51); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x6c51, 0x6c53);
    goto applySpeed;
  }
  CYC(0x6c51, 0x6c53);
  CYC(0x6c53, 0x6c55); A = 0x28;
  CYC(0x6c55, 0x6c56); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x6c56, 0x6c58);
    goto writeXh;
  }
  CYC(0x6c56, 0x6c58);
  CYC(0x6c58, 0x6c59); A = alu_inc8(gb, A);
writeXh:
  CYC(0x6c59, 0x6c5a); mem_wr(gb, HL, A);
  CYC(0x6c5a, 0x6c5c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6c5c, 0x6c5d); A = mem_rd(gb, DE);
  CYC(0x6c5d, 0x6c5f); alu_xor(gb, 0x10);
  CYC(0x6c5f, 0x6c60); mem_wr(gb, DE, A);
  CYC(0x6c60, 0x6c61); alu_xor(gb, A);
applySpeed:
  CYC(0x6c61, 0x6c64);
  objectApplySpeed_hook(gb);
}

void ramrock_glovePhase_updateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c64, 0x6c67); SET_HL(0xd00b);
  CYC(0x6c67, 0x6c69); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6c69, 0x6c6a); A = mem_rd(gb, DE);
  CYC(0x6c6a, 0x6c6b); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x6c6b, 0x6c6d);
    goto label_10_262;
  }
  CYC(0x6c6b, 0x6c6d);
  CYC(0x6c6d, 0x6c6e); C = A;
  CYC(0x6c6e, 0x6c6f); A = mem_rd(gb, HL);
  CYC(0x6c6f, 0x6c70); alu_sub(gb, C);
  CYC(0x6c70, 0x6c72); alu_cp(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6c72, 0x6c74);
    goto ret;
  }
  CYC(0x6c72, 0x6c74);
  if (F & FC) {
    CYCT(0x6c74, 0x6c76);
    goto label_10_262;
  }
  CYC(0x6c74, 0x6c76);
  CYC(0x6c76, 0x6c77); A = mem_rd(gb, DE);
  CYC(0x6c77, 0x6c79); alu_cp(gb, 0x50);
  CYC(0x6c79, 0x6c7b); C = 0x10;
  if (!(F & FC)) {
    CYCT(0x6c7b, 0x6c7d);
    goto ret;
  }
  CYC(0x6c7b, 0x6c7d);
  CYC(0x6c7d, 0x6c7f);
  goto moveInDirection;
label_10_262:
  CYC(0x6c7f, 0x6c80); A = mem_rd(gb, DE);
  CYC(0x6c80, 0x6c82); alu_cp(gb, 0x41);
  CYC(0x6c82, 0x6c84); C = 0x00;
  if (F & FC) {
    CYCT(0x6c84, 0x6c86);
    goto ret;
  }
  CYC(0x6c84, 0x6c86);
moveInDirection:
  CYC(0x6c86, 0x6c88); B = 0x14;
  CYC(0x6c88, 0x6c8a); E = ENEMY_BASE + OBJ_ANGLE;
  CALL_C(0x6c8a, objectApplyGivenSpeed_hook, 0x2029, 0x6c8d);
ret:
  RET(0x6c8d); return;
}

void ramrock_glovePhase_gotoSubstate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6bd7, 0x6bda); SET_BC(0xff80);
  CALL_C(0x6bda, objectSetSpeedZ_hook, 0x239d, 0x6bdd);
  CYC(0x6bdd, 0x6bdf); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6bdf, 0x6be1); mem_wr(gb, HL, 0x11);
  CYC(0x6be1, 0x6be3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6be3, 0x6be5); mem_wr(gb, HL, 0x03);
  CYC(0x6be5, 0x6be7); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6be7, 0x6be9); mem_wr(gb, HL, 0x08);
  CYC(0x6be9, 0x6beb); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6beb, 0x6bed); mem_wr(gb, HL, 0x78);
  RET(0x6bed); return;
}

void label_10_248_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6aa7, ecom_decCounter2_b10_hook, 0x43a3, 0x6aaa);
  if (!(F & FZ)) { RET_TAKEN(0x6aaa); return; }
  CYC(0x6aaa, 0x6aab);
  CYC(0x6aab, 0x6aad); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6aad, 0x6aaf); mem_wr(gb, HL, 0x08);
  CYC(0x6aaf, 0x6ab1); A = 0x83;
  CALL_C(0x6ab1, loadPaletteHeader_hook, 0x050b, 0x6ab4);
  CYC(0x6ab4, 0x6ab6); A = 0xab;
  CYC(0x6ab6, 0x6ab9);
  playSound_b00_hook(gb);
}

void label_10_236_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x69a6, ecom_incSubstate_b10_hook, 0x4005, 0x69a9);
  CYC(0x69a9, 0x69ab); A = 0x02;
  CYC(0x69ab, 0x69ae);
  enemySetAnimation_hook(gb);
}

void label_10_237_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x69ae, ecom_incState_b10_hook, 0x4000, 0x69b1);
  CYC(0x69b1, 0x69b2); L = alu_inc8(gb, L);
  CYC(0x69b2, 0x69b3); alu_xor(gb, A);
  CYC(0x69b3, 0x69b4); mem_wr(gb, HL, A);
  CYC(0x69b4, 0x69b6); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x69b6, 0x69b7); mem_wr(gb, HL, A);
  CYC(0x69b7, 0x69b9); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x69b9, 0x69bb); mem_wr(gb, HL, 0x0a);
  CYC(0x69bb, 0x69bd); A = 0x04;
  CYC(0x69bd, 0x69c0);
  enemySetAnimation_hook(gb);
}

void ramrock_seedPhase_resumeNormalMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a99, 0x6a9a); H = D;
  CYC(0x6a9a, 0x6a9c); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6a9c, 0x6a9e); mem_wr(gb, HL, 0x02);
  CYC(0x6a9e, 0x6aa0); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6aa0, 0x6aa2); mem_wr(gb, HL, 0x78);
  CYC(0x6aa2, 0x6aa4); A = 0x00;
  CYC(0x6aa4, 0x6aa7);
  enemySetAnimation_hook(gb);
}

void ramrock_seedPhase_6a94_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a94, 0x6a96); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a96, 0x6a98); A = 0x0c;
  CYC(0x6a98, 0x6a99); mem_wr(gb, DE, A);
  ramrock_seedPhase_resumeNormalMovement_hook(gb);
}

void ramrock_bombPhase_gotoSubstate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x696b, 0x696c); H = D;
  CYC(0x696c, 0x696e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x696e, 0x6970); A = 0x04;
  CYC(0x6970, 0x6971); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6971, 0x6973); mem_wr(gb, HL, 0x32);
  CYC(0x6973, 0x6975); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6975, 0x6977); mem_wr(gb, HL, 0x03);
  RET(0x6977); return;
}

void ramrock_state8_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6817, 0x6819); A = 0x01;
  CYC(0x6819, 0x681c); mem_wr(gb, 0xcc8a, A);
  CYC(0x681c, 0x681f); mem_wr(gb, 0xcc02, A);
  CYC(0x681f, 0x6822); A = mem_rd(gb, 0xcc93);
  CYC(0x6822, 0x6823); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6823); return; }
  CYC(0x6823, 0x6824);
  CYC(0x6824, 0x6826); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x6826, 0x6828); A = 0x3c;
  CYC(0x6828, 0x6829); mem_wr(gb, DE, A);
  CYC(0x6829, 0x682c);
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_state8_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x682c, 0x682e); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x682e, 0x682f); A = mem_rd(gb, DE);
  CYC(0x682f, 0x6830); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6830); return; }
  CYC(0x6830, 0x6831);
  CYC(0x6831, 0x6834); SET_BC(0xff80);
  CALL_C(0x6834, objectSetSpeedZ_hook, 0x239d, 0x6837);
  CYC(0x6837, 0x6839); C = 0x00;
  CALL_C(0x6839, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x683c);
  CYC(0x683c, 0x683e); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x683e, 0x683f); A = mem_rd(gb, DE);
  CYC(0x683f, 0x6841); alu_cp(gb, 0xf9);
  if (!(F & FZ)) { RET_TAKEN(0x6841); return; }
  CYC(0x6841, 0x6842);
  CYC(0x6842, 0x6844); C = 0x01;
spawnArm:
  CYC(0x6844, 0x6846); B = 0x05;
  CALL_C(0x6846, ecom_spawnEnemyWithSubid01_b10_hook, 0x4373, 0x6849);
  CYC(0x6849, 0x684b); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x684b, 0x684c); mem_wr(gb, HL, C);
  CYC(0x684c, 0x684e); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x684e, 0x6850); mem_wr(gb, HL, 0x80);
  CYC(0x6850, 0x6851); L = alu_inc8(gb, L);
  CYC(0x6851, 0x6852); mem_wr(gb, HL, D);
  CYC(0x6852, 0x6853); C = alu_dec8(gb, C);
  if (F & FZ) {
    CYCT(0x6853, 0x6855);
    goto spawnArm;
  }
  CYC(0x6853, 0x6855);
  CYC(0x6855, 0x6858);
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_state8_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6858, 0x685a); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x685a, 0x685b); A = mem_rd(gb, DE);
  CYC(0x685b, 0x685d); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x685d); return; }
  CYC(0x685d, 0x685e);
  CYC(0x685e, 0x6860); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6860, 0x6862); A = 0x02;
  CYC(0x6862, 0x6863); mem_wr(gb, DE, A);
  CALL_C(0x6863, ecom_incSubstate_b10_hook, 0x4005, 0x6866);
  CYC(0x6866, 0x6868); A = 0x84;
  CYC(0x6868, 0x686b);
  loadPaletteHeader_hook(gb);
}

void ramrock_state8_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x686b, ecom_decCounter1_b10_hook, 0x439a, 0x686e);
  if (!(F & FZ)) { RET_TAKEN(0x686e); return; }
  CYC(0x686e, 0x686f);
  CALL_C(0x686f, ecom_incSubstate_b10_hook, 0x4005, 0x6872);
  CYC(0x6872, 0x6874); A = 0xab;
  CALL_C(0x6874, playSound_b00_hook, 0x0c98, 0x6877);
  CYC(0x6877, 0x6879); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6879, 0x687a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x687a, 0x687c); A = 0x83;
  CYC(0x687c, 0x687f);
  loadPaletteHeader_hook(gb);
}

void ramrock_state8_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6893, enemyAnimate_hook, 0x2818, 0x6896);
  CALL_C(0x6896, objectApplySpeed_hook, 0x201d, 0x6899);
  CYC(0x6899, 0x689b); E = ENEMY_BASE + OBJ_YH;
  CYC(0x689b, 0x689c); A = mem_rd(gb, DE);
  CYC(0x689c, 0x689e); alu_cp(gb, 0x41);
  if (!(F & FC)) { RET_TAKEN(0x689e); return; }
  CYC(0x689e, 0x689f);
  CYC(0x689f, 0x68a0); alu_xor(gb, A);
  CYC(0x68a0, 0x68a3); mem_wr(gb, 0xcc8a, A);
  CYC(0x68a3, 0x68a6); mem_wr(gb, 0xcc02, A);
  CALL_C(0x68a6, ecom_incState_b10_hook, 0x4000, 0x68a9);
  CYC(0x68a9, 0x68ab); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x68ab, 0x68ad); mem_wr(gb, HL, 0x08);
  CYC(0x68ad, 0x68af); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x68af, 0x68b1); mem_wr(gb, HL, 0x03);
  CYC(0x68b1, 0x68b3); A = 0x2e;
  CYC(0x68b3, 0x68b6);
  playSound_b00_hook(gb);
}

void ramrock_state8_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x687f, enemyAnimate_hook, 0x2818, 0x6882);
  CYC(0x6882, 0x6884); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6884, 0x6885); A = mem_rd(gb, DE);
  CYC(0x6885, 0x6886); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6886); return; }
  CYC(0x6886, 0x6887);
  CALL_C(0x6887, ecom_incSubstate_b10_hook, 0x4005, 0x688a);
  CYC(0x688a, 0x688c); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x688c, 0x688e); mem_wr(gb, HL, 0x00);
  CYC(0x688e, 0x6890); A = 0x00;
  CALL_C(0x6890, enemySetAnimation_hook, 0x282b, 0x6893);
  ramrock_state8_substate5_hook(gb);
}

void ramrock_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6808, 0x6809); E = alu_inc8(gb, E);
  CYC(0x6809, 0x680a); A = mem_rd(gb, DE);
  CYC(0x680a, 0x680b); push_effect(gb, 0x680b);
  switch (ramrock_jump_table(gb)) {
    case 0x6817: ramrock_state8_substate0_hook(gb); return;
    case 0x682c: ramrock_state8_substate1_hook(gb); return;
    case 0x6858: ramrock_state8_substate2_hook(gb); return;
    case 0x686b: ramrock_state8_substate3_hook(gb); return;
    case 0x687f: ramrock_state8_substate4_hook(gb); return;
    case 0x6893: ramrock_state8_substate5_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrock_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67ec, 0x67ee); A = 0x07;
  CYC(0x67ee, 0x67f0); B = 0x83;
  CALL_C(0x67f0, enemyBoss_initializeRoom_b10_hook, 0x4546, 0x67f3);
  CYC(0x67f3, 0x67f5); A = 0x28;
  CALL_C(0x67f5, ecom_setSpeedAndState8_b10_hook, 0x4364, 0x67f8);
  CYC(0x67f8, 0x67fa); A = 0x04;
  CALL_C(0x67fa, enemySetAnimation_hook, 0x282b, 0x67fd);
  CYC(0x67fd, 0x67ff); B = 0x00;
  CYC(0x67ff, 0x6801); C = 0x0c;
  CALL_C(0x6801, enemyBoss_spawnShadow_b10_hook, 0x4534, 0x6804);
  CYC(0x6804, 0x6807);
  objectSetVisible81_hook(gb);
}

void ramrock_state_stub_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(0x6807); return;
}

void ramrock_swordPhase_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x68b6, enemyAnimate_hook, 0x2818, 0x68b9);
  CYC(0x68b9, 0x68bb); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x68bb, 0x68bc); A = mem_rd(gb, DE);
  CYC(0x68bc, 0x68be); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x68be, 0x68c0);
    ramrock_updateHorizontalMovement_hook(gb);
    return;
  }
  CYC(0x68be, 0x68c0);
  CYC(0x68c0, 0x68c3); alu_xor(gb, A);
  CYC(0x68c3, 0x68c5); mem_wr(gb, DE, A);
  CYC(0x68c5, 0x68c8); SET_BC(0x0000);
  CALL_C(0x68c8, objectSetSpeedZ_hook, 0x239d, 0x68cb);
  CALL_C(0x68cb, ecom_incState_b10_hook, 0x4000, 0x68ce);
  CYC(0x68ce, 0x68cf); L = alu_inc8(gb, L);
  CYC(0x68cf, 0x68d1); mem_wr(gb, HL, 0x00);
  CYC(0x68d1, 0x68d3); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x68d3, 0x68d4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x68d4, 0x68d6); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x68d6, 0x68d8); mem_wr(gb, HL, 0x1e);
  CYC(0x68d8, 0x68da); A = 0x04;
  CYC(0x68da, 0x68dd);
  enemySetAnimation_hook(gb);
}

void ramrock_bombPhase_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x690b, 0x690d); C = 0x10;
  CALL_C(0x690d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6910);
  CYC(0x6910, 0x6912); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6912, 0x6913); A = mem_rd(gb, DE);
  CYC(0x6913, 0x6915); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(0x6915); return; }
  CYC(0x6915, 0x6916);
  CALL_C(0x6916, ecom_decCounter2_b10_hook, 0x43a3, 0x6919);
  if (!(F & FZ)) { RET_TAKEN(0x6919); return; }
  CYC(0x6919, 0x691a);
  CYC(0x691a, 0x691c); B = 0x05;
  CALL_C(0x691c, ecom_spawnEnemyWithSubid01_b10_hook, 0x4373, 0x691f);
  CYC(0x691f, 0x6921); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6921, 0x6923); mem_wr(gb, HL, 0x02);
  CYC(0x6923, 0x6925); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x6925, 0x6927); mem_wr(gb, HL, 0x80);
  CYC(0x6927, 0x6928); L = alu_inc8(gb, L);
  CYC(0x6928, 0x6929); mem_wr(gb, HL, D);
  CYC(0x6929, 0x692b); B = 0x05;
  CALL_C(0x692b, ecom_spawnEnemyWithSubid01_b10_hook, 0x4373, 0x692e);
  CYC(0x692e, 0x6930); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6930, 0x6932); mem_wr(gb, HL, 0x03);
  CYC(0x6932, 0x6934); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x6934, 0x6936); mem_wr(gb, HL, 0x80);
  CYC(0x6936, 0x6937); L = alu_inc8(gb, L);
  CYC(0x6937, 0x6938); mem_wr(gb, HL, D);
  CYC(0x6938, 0x693b);
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_bombPhase_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x693b, 0x693d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x693d, 0x693e); A = mem_rd(gb, DE);
  CYC(0x693e, 0x6940); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x6940); return; }
  CYC(0x6940, 0x6941);
  CALL_C(0x6941, enemyAnimate_hook, 0x2818, 0x6944);
  CYC(0x6944, 0x6946); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6946, 0x6947); A = mem_rd(gb, DE);
  CYC(0x6947, 0x6948); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6948); return; }
  CYC(0x6948, 0x6949);
  CYC(0x6949, 0x694c); SET_BC(0xff80);
  CALL_C(0x694c, objectSetSpeedZ_hook, 0x239d, 0x694f);
  CYC(0x694f, 0x6951); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6951, 0x6953); mem_wr(gb, HL, 0x08);
  CYC(0x6953, 0x6955); A = 0x01;
  CALL_C(0x6955, enemySetAnimation_hook, 0x282b, 0x6958);
  CYC(0x6958, 0x695b);
  ecom_incSubstate_b10_hook(gb);
}

void ramrock_bombPhase_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x695b, 0x695d); C = 0x00;
  CALL_C(0x695d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6960);
  CYC(0x6960, 0x6962); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x6962, 0x6963); A = mem_rd(gb, DE);
  CYC(0x6963, 0x6965); alu_cp(gb, 0xf9);
  if (!(F & FZ)) { RET_TAKEN(0x6965); return; }
  CYC(0x6965, 0x6966);
  CYC(0x6966, 0x6968); A = 0xab;
  CALL_C(0x6968, playSound_b00_hook, 0x0c98, 0x696b);
  ramrock_bombPhase_gotoSubstate3_hook(gb);
}

void ramrock_bombPhase_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6978, 0x697a); E = ENEMY_BASE + OBJ_VAR38;
  CYC(0x697a, 0x697b); A = mem_rd(gb, DE);
  CYC(0x697b, 0x697d); alu_sub(gb, 0x01);
  CYC(0x697d, 0x697e); mem_wr(gb, DE, A);
  if (!(F & FC)) {
    CYCT(0x697e, 0x6980);
    goto checkVar35;
  }
  CYC(0x697e, 0x6980);
  CYC(0x6980, 0x6982); A = 0x1e;
  CYC(0x6982, 0x6983); mem_wr(gb, DE, A);
  CYC(0x6983, 0x6985); A = 0xba;
  CALL_C(0x6985, playSound_b00_hook, 0x0c98, 0x6988);
checkVar35:
  CYC(0x6988, 0x698a); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x698a, 0x698b); A = mem_rd(gb, DE);
  CYC(0x698b, 0x698d); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x698d, 0x698f);
    label_10_237_hook(gb);
    return;
  }
  CYC(0x698d, 0x698f);
  CALL_C(0x698f, enemyAnimate_hook, 0x2818, 0x6992);
  CALL_C(0x6992, ecom_applyVelocityForSideviewEnemy_b10_hook, 0x4153, 0x6995);
  CALL_C(0x6995, ecom_decCounter2_b10_hook, 0x43a3, 0x6998);
  if (F & FZ) {
    CYCT(0x6998, 0x699a);
    label_10_236_hook(gb);
    return;
  }
  CYC(0x6998, 0x699a);
  CALL_C(0x699a, ecom_decCounter1_b10_hook, 0x439a, 0x699d);
  if (!(F & FZ)) { RET_TAKEN(0x699d); return; }
  CYC(0x699d, 0x699e);
  CYC(0x699e, 0x69a0); mem_wr(gb, HL, 0x04);
  CALL_C(0x69a0, objectGetAngleTowardLink_hook, 0x1e9c, 0x69a3);
  CYC(0x69a3, 0x69a6);
  objectNudgeAngleTowards_hook(gb);
}

void ramrock_bombPhase_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x69c0, enemyAnimate_hook, 0x2818, 0x69c3);
  CYC(0x69c3, 0x69c4); H = D;
  CYC(0x69c4, 0x69c6); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x69c6, 0x69c8); mem_wr(gb, HL, 0x08);
  CYC(0x69c8, 0x69ca); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x69ca, 0x69cb); A = mem_rd(gb, DE);
  CYC(0x69cb, 0x69cd); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x69cd, 0x69cf);
    goto rla;
  }
  CYC(0x69cd, 0x69cf);
  CYC(0x69cf, 0x69d1); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x69d1, 0x69d3); mem_wr(gb, HL, 0x09);
  CYC(0x69d3, 0x69d5); A = 0x81;
  CYC(0x69d5, 0x69d8);
  playSound_b00_hook(gb);
  return;
rla:
  CYC(0x69d8, 0x69d9); alu_rla(gb);
  if (!(F & FC)) { RET_TAKEN(0x69d9); return; }
  CYC(0x69d9, 0x69da);
  CYC(0x69da, 0x69dc); A = 0x01;
  CALL_C(0x69dc, enemySetAnimation_hook, 0x282b, 0x69df);
  CYC(0x69df, 0x69e2);
  ramrock_bombPhase_gotoSubstate3_hook(gb);
}

void ramrock_bombPhase__func_68fe_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68fe, 0x68ff); E = alu_inc8(gb, E);
  CYC(0x68ff, 0x6900); A = mem_rd(gb, DE);
  CYC(0x6900, 0x6901); push_effect(gb, 0x6901);
  switch (ramrock_jump_table(gb)) {
    case 0x690b: ramrock_bombPhase_substate0_hook(gb); return;
    case 0x693b: ramrock_bombPhase_substate1_hook(gb); return;
    case 0x695b: ramrock_bombPhase_substate2_hook(gb); return;
    case 0x6978: ramrock_bombPhase_substate3_hook(gb); return;
    case 0x69c0: ramrock_bombPhase_substate4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrock_bombPhase_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68dd, 0x68e0); push_effect(gb, 0x68e0);
  ramrock_bombPhase__func_68fe_hook(gb);
  CYC(0x68e0, 0x68e2); C = 0x03;
  CALL_C(0x68e2, findItemWithID_hook, 0x22b9, 0x68e5);
  if (!(F & FZ)) { RET_TAKEN(0x68e5); return; }
  CYC(0x68e5, 0x68e6);
  CALL_C(0x68e6, checkObjectsCollided_hook, 0x1d5a, 0x68e9);
  if (!(F & FC)) {
    CYCT(0x68e9, 0x68eb);
    goto findSecondBomb;
  }
  CYC(0x68e9, 0x68eb);
  CYC(0x68eb, 0x68ed); L = 0x09;
  CYC(0x68ed, 0x68ef); mem_wr(gb, HL, 0xff);
findSecondBomb:
  CYC(0x68ef, 0x68f1); C = 0x03;
  CALL_C(0x68f1, findItemWithID_startingAfterH_hook, 0x22c0, 0x68f4);
  if (!(F & FZ)) { RET_TAKEN(0x68f4); return; }
  CYC(0x68f4, 0x68f5);
  CALL_C(0x68f5, checkObjectsCollided_hook, 0x1d5a, 0x68f8);
  if (!(F & FC)) { RET_TAKEN(0x68f8); return; }
  CYC(0x68f8, 0x68f9);
  CYC(0x68f9, 0x68fb); L = 0x09;
  CYC(0x68fb, 0x68fd); mem_wr(gb, HL, 0xff);
  RET(0x68fd); return;
}

void ramrock_seedPhase_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a48, 0x6a49); H = D;
  CYC(0x6a49, 0x6a4c); SET_BC(0x4878);
  CYC(0x6a4c, 0x6a4e); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6a4e, 0x6a4f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6a4f, 0x6a50); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6a50, 0x6a52);
    goto updateMovement;
  }
  CYC(0x6a50, 0x6a52);
  CYC(0x6a52, 0x6a53); L = alu_inc8(gb, L);
  CYC(0x6a53, 0x6a54); A = mem_rd(gb, HL);
  CYC(0x6a54, 0x6a55); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x6a55, 0x6a57);
    goto updateMovement;
  }
  CYC(0x6a55, 0x6a57);
  CYC(0x6a57, 0x6a59); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a59, 0x6a5a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6a5a, 0x6a5c); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6a5c, 0x6a5e); mem_wr(gb, HL, 0x02);
  CYC(0x6a5e, 0x6a60); C = 0x04;
spawnArm:
  CYC(0x6a60, 0x6a62); B = 0x05;
  CALL_C(0x6a62, ecom_spawnEnemyWithSubid01_b10_hook, 0x4373, 0x6a65);
  CYC(0x6a65, 0x6a67); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a67, 0x6a68); mem_wr(gb, HL, C);
  CYC(0x6a68, 0x6a6a); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x6a6a, 0x6a6c); mem_wr(gb, HL, 0x80);
  CYC(0x6a6c, 0x6a6d); L = alu_inc8(gb, L);
  CYC(0x6a6d, 0x6a6e); mem_wr(gb, HL, D);
  CYC(0x6a6e, 0x6a6f); C = alu_inc8(gb, C);
  CYC(0x6a6f, 0x6a70); A = C;
  CYC(0x6a70, 0x6a72); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(0x6a72, 0x6a74);
    goto spawnArm;
  }
  CYC(0x6a72, 0x6a74);
  CYC(0x6a74, 0x6a77);
  ecom_incSubstate_b10_hook(gb);
  return;
updateMovement:
  CALL_C(0x6a77, objectGetRelativeAngle_hook, 0x1ea4, 0x6a7a);
  CYC(0x6a7a, 0x6a7c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6a7c, 0x6a7d); mem_wr(gb, DE, A);
  CYC(0x6a7d, 0x6a80);
  objectApplySpeed_hook(gb);
}

void ramrock_seedPhase_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a80, 0x6a82); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a82, 0x6a83); A = mem_rd(gb, DE);
  CYC(0x6a83, 0x6a85); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(0x6a85); return; }
  CYC(0x6a85, 0x6a86);
  CYC(0x6a86, 0x6a88); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6a88, 0x6a89); A = mem_rd(gb, DE);
  CYC(0x6a89, 0x6a8a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6a8a, 0x6a8c);
    label_10_248_hook(gb);
    return;
  }
  CYC(0x6a8a, 0x6a8c);
  CALL_C(0x6a8c, enemyAnimate_hook, 0x2818, 0x6a8f);
  CYC(0x6a8f, 0x6a91); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6a91, 0x6a92); A = mem_rd(gb, DE);
  CYC(0x6a92, 0x6a93); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6a93); return; }
  CYC(0x6a93, 0x6a94);
  ramrock_seedPhase_6a94_hook(gb);
}

void ramrock_seedPhase_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6ab9, enemyAnimate_hook, 0x2818, 0x6abc);
  CALL_C(0x6abc, ramrock_updateHorizontalMovement_hook, 0x6c3f, 0x6abf);
  CALL_C(0x6abf, getRandomNumber_hook, 0x043e, 0x6ac2);
  CYC(0x6ac2, 0x6ac3); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x6ac3); return; }
  CYC(0x6ac3, 0x6ac4);
  CALL_C(0x6ac4, ecom_decCounter2_b10_hook, 0x43a3, 0x6ac7);
  if (!(F & FZ)) { RET_TAKEN(0x6ac7); return; }
  CYC(0x6ac7, 0x6ac8);
  CYC(0x6ac8, 0x6aca); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6aca, 0x6acb); A = mem_rd(gb, DE);
  CYC(0x6acb, 0x6acd); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(0x6acd); return; }
  CYC(0x6acd, 0x6ace);
  CALL_C(0x6ace, getRandomNumber_hook, 0x043e, 0x6ad1);
  CYC(0x6ad1, 0x6ad3); alu_and(gb, 0x03);
  CYC(0x6ad3, 0x6ad4); L = E;
  if (F & FZ) {
    CYCT(0x6ad4, 0x6ad6);
    goto gotoNextSubstate;
  }
  CYC(0x6ad4, 0x6ad6);
  CYC(0x6ad6, 0x6ad8); mem_wr(gb, HL, 0x0f);
  CYC(0x6ad8, 0x6ada); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6ada, 0x6adc); mem_wr(gb, HL, 0x06);
  CYC(0x6adc, 0x6ade); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6ade, 0x6ae0); mem_wr(gb, HL, 0x3c);
  CYC(0x6ae0, 0x6ae2); B = 0x4f;
  CALL_C(0x6ae2, ecom_spawnProjectile_b10_hook, 0x437c, 0x6ae5);
  CYC(0x6ae5, 0x6ae8); SET_BC(0x1000);
  CALL_C(0x6ae8, objectCopyPositionWithOffset_hook, 0x225a, 0x6aeb);
  CYC(0x6aeb, 0x6aed);
  goto setAnimation0;
gotoNextSubstate:
  CYC(0x6aed, 0x6af0); mem_wr(gb, HL, 0x0e);
  CYC(0x6af0, 0x6af1); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6af1, 0x6af3); mem_wr(gb, HL, 0x18);
  CALL_C(0x6af3, ecom_incSubstate_b10_hook, 0x4005, 0x6af6);
setAnimation0:
  CYC(0x6af6, 0x6af8); A = 0x00;
  CYC(0x6af8, 0x6afb);
  enemySetAnimation_hook(gb);
}

void ramrock_seedPhase_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6afb, 0x6afd); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6afd, 0x6afe); A = mem_rd(gb, DE);
  CYC(0x6afe, 0x6b00); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x6b00, 0x6b02);
    ramrock_seedPhase_resumeNormalMovement_hook(gb);
    return;
  }
  CYC(0x6b00, 0x6b02);
  CALL_C(0x6b02, ramrock_updateHorizontalMovement_hook, 0x6c3f, 0x6b05);
  if (!(F & FZ)) { RET_TAKEN(0x6b05); return; }
  CYC(0x6b05, 0x6b06);
  CALL_C(0x6b06, ecom_incSubstate_b10_hook, 0x4005, 0x6b09);
  CYC(0x6b09, 0x6b0b); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6b0b, 0x6b0d); mem_wr(gb, HL, 0xb4);
  CYC(0x6b0d, 0x6b0e); L = alu_inc8(gb, L);
  CYC(0x6b0e, 0x6b10); mem_wr(gb, HL, 0x1e);
  CYC(0x6b10, 0x6b12); B = 0x34;
  CALL_C(0x6b12, ecom_spawnProjectile_b10_hook, 0x437c, 0x6b15);
  CYC(0x6b15, 0x6b17); L = PART_BASE + OBJ_SUBID;
  CYC(0x6b17, 0x6b19); mem_wr(gb, HL, 0x0e);
  CYC(0x6b19, 0x6b1c); SET_BC(0x0400);
  CYC(0x6b1c, 0x6b1f);
  objectCopyPositionWithOffset_hook(gb);
}

void ramrock_seedPhase_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6b46, ecom_decCounter1_b10_hook, 0x439a, 0x6b49);
  if (!(F & FZ)) { RET_TAKEN(0x6b49); return; }
  CYC(0x6b49, 0x6b4a);
  CYC(0x6b4a, 0x6b4d);
  ramrock_seedPhase_resumeNormalMovement_hook(gb);
}

void ramrock_seedPhase_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b1f, 0x6b21); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6b21, 0x6b22); A = mem_rd(gb, DE);
  CYC(0x6b22, 0x6b24); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x6b24, 0x6b27);
    ramrock_seedPhase_resumeNormalMovement_hook(gb);
    return;
  }
  CYC(0x6b24, 0x6b27);
  CALL_C(0x6b27, ecom_decCounter2_b10_hook, 0x43a3, 0x6b2a);
  if (!(F & FZ)) { RET_TAKEN(0x6b2a); return; }
  CYC(0x6b2a, 0x6b2b);
  CALL_C(0x6b2b, ecom_decCounter1_b10_hook, 0x439a, 0x6b2e);
  if (F & FZ) {
    CYCT(0x6b2e, 0x6b30);
    goto gotoNextSubstate;
  }
  CYC(0x6b2e, 0x6b30);
  CYC(0x6b30, 0x6b31); A = mem_rd(gb, HL);
  CYC(0x6b31, 0x6b33); alu_and(gb, 0x07);
  CYC(0x6b33, 0x6b35); A = 0x5d;
  if (F & FZ) {
    CALL_C_CC(0x6b35, playSound_b00_hook, 0x0c98, 0x6b38);
  } else {
    CYC(0x6b35, 0x6b38);
  }
  CYC(0x6b38, 0x6b3b);
  ramrock_updateHorizontalMovement_hook(gb);
  return;
gotoNextSubstate:
  CALL_C(0x6b3b, ecom_incSubstate_b10_hook, 0x4005, 0x6b3e);
  CYC(0x6b3e, 0x6b40); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6b40, 0x6b42); mem_wr(gb, HL, 0x5a);
  CYC(0x6b42, 0x6b44); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6b44, 0x6b46); mem_wr(gb, HL, 0x0c);
  ramrock_seedPhase_substate5_hook(gb);
}

void ramrock_seedPhase_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b4d, 0x6b4f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6b4f, 0x6b50); A = mem_rd(gb, DE);
  CYC(0x6b50, 0x6b52); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x6b52, 0x6b54);
    goto callSeedPhase6a94;
  }
  CYC(0x6b52, 0x6b54);
  CALL_C(0x6b54, ecom_decCounter1_b10_hook, 0x439a, 0x6b57);
  if (!(F & FZ)) { RET_TAKEN(0x6b57); return; }
  CYC(0x6b57, 0x6b58);
callSeedPhase6a94:
  CYC(0x6b58, 0x6b5b);
  ramrock_seedPhase_6a94_hook(gb);
}

void ramrock_seedPhase_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69e2, 0x69e3); H = D;
  CYC(0x69e3, 0x69e5); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x69e5, 0x69e6); A = mem_rd(gb, HL);
  CYC(0x69e6, 0x69e7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x69e7, 0x69e9);
    goto runSubstate;
  }
  CYC(0x69e7, 0x69e9);
  CYC(0x69e9, 0x69ea); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x69ea, 0x69ec);
    goto runSubstate;
  }
  CYC(0x69ea, 0x69ec);
  CYC(0x69ec, 0x69ee); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x69ee, 0x69ef); A = mem_rd(gb, DE);
  CYC(0x69ef, 0x69f1); alu_cp(gb, 0x9a);
  if (F & FC) {
    CYCT(0x69f1, 0x69f3);
    goto noSeedCollision;
  }
  CYC(0x69f1, 0x69f3);
  CYC(0x69f3, 0x69f5); alu_cp(gb, 0x9f);
  if (F & FC) {
    CYCT(0x69f5, 0x69f7);
    goto seedCollision;
  }
  CYC(0x69f5, 0x69f7);
noSeedCollision:
  CYC(0x69f7, 0x69f8); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x69f8, 0x69fa);
    goto noCollision;
  }
  CYC(0x69f8, 0x69fa);
otherCollision:
  CYC(0x69fa, 0x69fc); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x69fc, 0x69fe); mem_wr(gb, HL, 0x0d);
  CYC(0x69fe, 0x6a00); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6a00, 0x6a02); mem_wr(gb, HL, 0x10);
  CYC(0x6a02, 0x6a04);
  goto runSubstate;
seedCollision:
  CYC(0x6a04, 0x6a06); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x6a06, 0x6a07); A = mem_rd(gb, HL);
  CYC(0x6a07, 0x6a08); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6a08, 0x6a0a);
    goto otherCollision;
  }
  CYC(0x6a08, 0x6a0a);
  CYC(0x6a0a, 0x6a0c); mem_wr(gb, HL, 0x3c);
  CYC(0x6a0c, 0x6a0e); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x6a0e, 0x6a0f); A = mem_rd(gb, HL);
  CYC(0x6a0f, 0x6a11); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x6a11, 0x6a13);
    goto seedPhaseEnd;
  }
  CYC(0x6a11, 0x6a13);
  CYC(0x6a13, 0x6a14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6a14, 0x6a16); A = 0x63;
  CALL_C(0x6a16, playSound_b00_hook, 0x0c98, 0x6a19);
  CYC(0x6a19, 0x6a1b);
  goto runSubstate;
noCollision:
  CYC(0x6a1b, 0x6a1d); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6a1d, 0x6a1e); A = mem_rd(gb, HL);
  CYC(0x6a1e, 0x6a1f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6a1f, 0x6a21);
    goto runSubstate;
  }
  CYC(0x6a1f, 0x6a21);
  CYC(0x6a21, 0x6a22); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x6a22, 0x6a24);
    goto runSubstate;
  }
  CYC(0x6a22, 0x6a24);
  CYC(0x6a24, 0x6a26); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a26, 0x6a28); mem_wr(gb, HL, 0x0c);
runSubstate:
  CYC(0x6a28, 0x6a2a); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6a2a, 0x6a2b); A = mem_rd(gb, DE);
  CYC(0x6a2b, 0x6a2c); push_effect(gb, 0x6a2c);
  switch (ramrock_jump_table(gb)) {
    case 0x6a48: ramrock_seedPhase_substate0_hook(gb); return;
    case 0x6a80: ramrock_seedPhase_substate1_hook(gb); return;
    case 0x6ab9: ramrock_seedPhase_substate2_hook(gb); return;
    case 0x6afb: ramrock_seedPhase_substate3_hook(gb); return;
    case 0x6b1f: ramrock_seedPhase_substate4_hook(gb); return;
    case 0x6b46: ramrock_seedPhase_substate5_hook(gb); return;
    case 0x6b4d: ramrock_seedPhase_substate6_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
seedPhaseEnd:
  CYC(0x6a3a, 0x6a3c); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6a3c, 0x6a3e); mem_wr(gb, HL, 0x10);
  CALL_C(0x6a3e, ecom_incState_b10_hook, 0x4000, 0x6a41);
  CYC(0x6a41, 0x6a42); L = alu_inc8(gb, L);
  CYC(0x6a42, 0x6a43); alu_xor(gb, A);
  CYC(0x6a43, 0x6a44); mem_wr(gb, HL, A);
  CYC(0x6a44, 0x6a46); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x6a46, 0x6a47); mem_wr(gb, HL, A);
  RET(0x6a47); return;
}

void ramrock_glovePhase_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b68, 0x6b69); H = D;
  CYC(0x6b69, 0x6b6c); SET_BC(0x4878);
  CYC(0x6b6c, 0x6b6e); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6b6e, 0x6b6f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6b6f, 0x6b70); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x6b70, 0x6b72);
    goto updateMovement;
  }
  CYC(0x6b70, 0x6b72);
  CYC(0x6b72, 0x6b73); L = alu_inc8(gb, L);
  CYC(0x6b73, 0x6b74); A = mem_rd(gb, HL);
  CYC(0x6b74, 0x6b75); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x6b75, 0x6b77);
    goto updateMovement;
  }
  CYC(0x6b75, 0x6b77);
  CALL_C(0x6b77, ecom_incSubstate_b10_hook, 0x4005, 0x6b7a);
  CYC(0x6b7a, 0x6b7d); SET_BC(0xe001);
spawnArm:
  CYC(0x6b7d, 0x6b7e); push_effect(gb, BC);
  CYC(0x6b7e, 0x6b80); B = 0x35;
  CALL_C(0x6b80, ecom_spawnProjectile_b10_hook, 0x437c, 0x6b83);
  CYC(0x6b83, 0x6b85); L = PART_BASE + OBJ_SUBID;
  CYC(0x6b85, 0x6b86); mem_wr(gb, HL, C);
  CYC(0x6b86, 0x6b87); SET_BC(pop_effect(gb));
  CYC(0x6b87, 0x6b88); push_effect(gb, BC);
  CYC(0x6b88, 0x6b89); C = B;
  CYC(0x6b89, 0x6b8b); B = 0x18;
  CALL_C(0x6b8b, objectCopyPositionWithOffset_hook, 0x225a, 0x6b8e);
  CYC(0x6b8e, 0x6b8f); SET_BC(pop_effect(gb));
  CYC(0x6b8f, 0x6b90); C = alu_dec8(gb, C);
  CYC(0x6b90, 0x6b92); A = 0x04;
  if (!(F & FZ)) {
    CYCT(0x6b92, 0x6b95);
    enemySetAnimation_hook(gb);
    return;
  }
  CYC(0x6b92, 0x6b95);
  CYC(0x6b95, 0x6b96); A = B;
  CYC(0x6b96, 0x6b97); alu_cpl(gb);
  CYC(0x6b97, 0x6b98); A = alu_inc8(gb, A);
  CYC(0x6b98, 0x6b99); B = A;
  CYC(0x6b99, 0x6b9b);
  goto spawnArm;
updateMovement:
  CALL_C(0x6b9b, objectGetRelativeAngle_hook, 0x1ea4, 0x6b9e);
  CYC(0x6b9e, 0x6ba0); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6ba0, 0x6ba1); mem_wr(gb, DE, A);
  CYC(0x6ba1, 0x6ba4);
  objectApplySpeed_hook(gb);
}

void ramrock_glovePhase_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ba4, 0x6ba6); C = 0x10;
  CALL_C(0x6ba6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6ba9);
  CYC(0x6ba9, 0x6bab); E = ENEMY_BASE + OBJ_VAR37;
  CYC(0x6bab, 0x6bac); A = mem_rd(gb, DE);
  CYC(0x6bac, 0x6bae); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x6bae); return; }
  CYC(0x6bae, 0x6baf);
  CALL_C(0x6baf, ecom_incSubstate_b10_hook, 0x4005, 0x6bb2);
  CYC(0x6bb2, 0x6bb4); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6bb4, 0x6bb6); mem_wr(gb, HL, 0x02);
  CYC(0x6bb6, 0x6bb8); A = 0x84;
  CYC(0x6bb8, 0x6bbb);
  loadPaletteHeader_hook(gb);
}

void ramrock_glovePhase_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6bbb, ecom_decCounter2_b10_hook, 0x43a3, 0x6bbe);
  if (F & FZ) {
    CYCT(0x6bbe, 0x6bc0);
    goto afterPalette;
  }
  CYC(0x6bbe, 0x6bc0);
  CYC(0x6bc0, 0x6bc2); A = 0xab;
  CALL_C(0x6bc2, playSound_b00_hook, 0x0c98, 0x6bc5);
  CYC(0x6bc5, 0x6bc7); A = 0x83;
  CALL_C(0x6bc7, loadPaletteHeader_hook, 0x050b, 0x6bca);
afterPalette:
  CALL_C(0x6bca, enemyAnimate_hook, 0x2818, 0x6bcd);
  CYC(0x6bcd, 0x6bcf); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6bcf, 0x6bd0); A = mem_rd(gb, DE);
  CYC(0x6bd0, 0x6bd1); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6bd1); return; }
  CYC(0x6bd1, 0x6bd2);
  CYC(0x6bd2, 0x6bd4); A = 0x03;
  CALL_C(0x6bd4, enemySetAnimation_hook, 0x282b, 0x6bd7);
  ramrock_glovePhase_gotoSubstate3_hook(gb);
}

void ramrock_glovePhase_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6bee, enemyAnimate_hook, 0x2818, 0x6bf1);
  CYC(0x6bf1, 0x6bf3); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x6bf3, 0x6bf4); A = mem_rd(gb, DE);
  CYC(0x6bf4, 0x6bf6); alu_cp(gb, 0xf9);
  CYC(0x6bf6, 0x6bf8); C = 0x00;
  if (!(F & FZ)) {
    CALL_C_CC(0x6bf8, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6bfb);
  } else {
    CYC(0x6bf8, 0x6bfb);
  }
  CALL_C(0x6bfb, ramrock_glovePhase_updateMovement_hook, 0x6c64, 0x6bfe);
  CALL_C(0x6bfe, ecom_decCounter2_b10_hook, 0x43a3, 0x6c01);
  if (!(F & FZ)) {
    CYCT(0x6c01, 0x6c03);
    ramrock_glovePhase_reverseDirection_hook(gb);
    return;
  }
  CYC(0x6c01, 0x6c03);
  CYC(0x6c03, 0x6c05); C = 0x50;
  CALL_C(0x6c05, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6c08);
  if (!(F & FC)) {
    CYCT(0x6c08, 0x6c0a);
    ramrock_glovePhase_reverseDirection_hook(gb);
    return;
  }
  CYC(0x6c08, 0x6c0a);
  CYC(0x6c0a, 0x6c0b); H = D;
  CYC(0x6c0b, 0x6c0d); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6c0d, 0x6c0f); A = 0x12;
  CYC(0x6c0f, 0x6c10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(0x6c10, getRandomNumber_hook, 0x043e, 0x6c13);
  CYC(0x6c13, 0x6c15); alu_and(gb, 0x01);
  CYC(0x6c15, 0x6c17); A = alu_swap(gb, A);
  CYC(0x6c17, 0x6c18); mem_wr(gb, HL, A);
  CALL_C(0x6c18, getRandomNumber_hook, 0x043e, 0x6c1b);
  CYC(0x6c1b, 0x6c1d); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x6c1d, 0x6c1f);
    goto setSubstate4;
  }
  CYC(0x6c1d, 0x6c1f);
  CYC(0x6c1f, 0x6c21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
setSubstate4:
  CYC(0x6c21, 0x6c23); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6c23, 0x6c25); mem_wr(gb, HL, 0x04);
  RET(0x6c25); return;
}

void ramrock_glovePhase_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6c26, 0x6c28); E = ENEMY_BASE + OBJ_VAR35;
  CYC(0x6c28, 0x6c29); A = mem_rd(gb, DE);
  CYC(0x6c29, 0x6c2b); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x6c2b, 0x6c2d);
    goto dead;
  }
  CYC(0x6c2b, 0x6c2d);
  CALL_C(0x6c2d, enemyAnimate_hook, 0x2818, 0x6c30);
  CYC(0x6c30, 0x6c32); E = ENEMY_BASE + OBJ_VAR37;
  CYC(0x6c32, 0x6c33); A = mem_rd(gb, DE);
  CYC(0x6c33, 0x6c35); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x6c35); return; }
  CYC(0x6c35, 0x6c36);
  CYC(0x6c36, 0x6c38);
  ramrock_glovePhase_gotoSubstate3_hook(gb);
  return;
dead:
  CYC(0x6c38, 0x6c3a); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6c3a, 0x6c3b); alu_xor(gb, A);
  CYC(0x6c3b, 0x6c3c); mem_wr(gb, DE, A);
  CYC(0x6c3c, 0x6c3f);
  enemyBoss_dead_b10_hook(gb);
}

void ramrock_glovePhase_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6b5b, 0x6b5c); E = alu_inc8(gb, E);
  CYC(0x6b5c, 0x6b5d); A = mem_rd(gb, DE);
  CYC(0x6b5d, 0x6b5e); push_effect(gb, 0x6b5e);
  switch (ramrock_jump_table(gb)) {
    case 0x6b68: ramrock_glovePhase_substate0_hook(gb); return;
    case 0x6ba4: ramrock_glovePhase_substate1_hook(gb); return;
    case 0x6bbb: ramrock_glovePhase_substate2_hook(gb); return;
    case 0x6bee: ramrock_glovePhase_substate3_hook(gb); return;
    case 0x6c26: ramrock_glovePhase_substate4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void enemyCode07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67ce, 0x67d0); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x67d0, 0x67d1); A = mem_rd(gb, DE);
  CYC(0x67d1, 0x67d2); push_effect(gb, 0x67d2);
  switch (ramrock_jump_table(gb)) {
    case 0x67ec: ramrock_state0_hook(gb); return;
    case 0x6807: ramrock_state_stub_hook(gb); return;
    case 0x6808: ramrock_state8_hook(gb); return;
    case 0x68b6: ramrock_swordPhase_hook(gb); return;
    case 0x68dd: ramrock_bombPhase_hook(gb); return;
    case 0x69e2: ramrock_seedPhase_hook(gb); return;
    case 0x6b5b: ramrock_glovePhase_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
