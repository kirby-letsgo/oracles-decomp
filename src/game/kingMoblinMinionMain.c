#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/enemies/kingMoblinMinionMain.s, bank $10.

static void kingMoblinMinion_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void kingMoblinMinion_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// $6ccf is a private 4-byte-per-subid data table (counter1, direction, yh, xh), not code; it is
// only reached through the RST $18 index below, so it needs no hook entry of its own.
void kingMoblinMinion_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6ca8, 0x6ca9); H = D;
  CYC(0x6ca9, 0x6caa); L = E;
  CYC(0x6caa, 0x6cab); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6cab, 0x6cad); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6cad, 0x6caf); mem_wr(gb, HL, 0x50);
  CYC(0x6caf, 0x6cb1); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6cb1, 0x6cb2); A = mem_rd(gb, DE);
  CYC(0x6cb2, 0x6cb3); alu_add(gb, A);
  CYC(0x6cb3, 0x6cb6); SET_HL(0x6ccf);
  CYC(0x6cb6, 0x6cb7);
  kingMoblinMinion_addDoubleIndexToHl_from_rst(gb, 0x6cb7);
  CYC(0x6cb7, 0x6cb9); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6cb9, 0x6cba); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6cba, 0x6cbb); mem_wr(gb, DE, A);
  CYC(0x6cbb, 0x6cbd); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6cbd, 0x6cbe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6cbe, 0x6cbf); mem_wr(gb, DE, A);
  CYC(0x6cbf, 0x6cc1); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6cc1, 0x6cc2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6cc2, 0x6cc3); mem_wr(gb, DE, A);
  CYC(0x6cc3, 0x6cc5); E = ENEMY_BASE + OBJ_XH;
  CYC(0x6cc5, 0x6cc6); A = mem_rd(gb, HL);
  CYC(0x6cc6, 0x6cc7); mem_wr(gb, DE, A);
  CYC(0x6cc7, 0x6cc9); A = 0x02;
  CALL_C(0x6cc9, enemySetAnimation_hook, 0x282b, 0x6ccc);
  CYC(0x6ccc, 0x6ccf);
  objectSetVisiblec2_hook(gb);
}

void kingMoblinMinion_state2_hook(GB *gb) {
  CYC(0x6cd7, 0x6cd8); H = D;
  CYC(0x6cd8, 0x6cd9); L = E;
  CYC(0x6cd9, 0x6cda); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6cda, 0x6cdc); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6cdc, 0x6cde); mem_wr(gb, HL, 0x0c);
  CYC(0x6cde, 0x6ce0); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6ce0, 0x6ce1); A = mem_rd(gb, DE);
  CYC(0x6ce1, 0x6ce4);
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6ce4, ecom_decCounter2_b10_hook, 0x43a3, 0x6ce7);
  if (!(F & FZ)) {
    CYCT(0x6ce7, 0x6ce9);
    kingMoblinMinion_animate_hook(gb);
    return;
  }
  CYC(0x6ce7, 0x6ce9);
  CYC(0x6ce9, 0x6ceb); B = 0x47;
  CALL_C(0x6ceb, ecom_spawnProjectile_b10_hook, 0x437c, 0x6cee);
  if (!(F & FZ)) { RET_TAKEN(0x6cee); return; }
  CYC(0x6cee, 0x6cef);
  CALL_C(0x6cef, ecom_incState_b10_hook, 0x4000, 0x6cf2);
  CYC(0x6cf2, 0x6cf4); A = 0x02;
  CYC(0x6cf4, 0x6cf7);
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6cf7, ecom_decCounter1_b10_hook, 0x439a, 0x6cfa);
  CYC(0x6cfa, 0x6cfb); L = E;
  if (F & FZ) {
    CYCT(0x6cfb, 0x6cfd);
    goto jump;
  }
  CYC(0x6cfb, 0x6cfd);
  CYC(0x6cfd, 0x6d00); A = W8(wScreenShakeCounterY);
  CYC(0x6d00, 0x6d01); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6d01, 0x6d03);
    kingMoblinMinion_animate_hook(gb);
    return;
  }
  CYC(0x6d01, 0x6d03);
  CYC(0x6d03, 0x6d05); mem_wr(gb, HL, 0x07);
  CYC(0x6d05, 0x6d07);
  kingMoblinMinion_animate_hook(gb);
  return;
jump:
  CYC(0x6d07, 0x6d08); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d08, 0x6d0a); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6d0a, 0x6d0c); A = 0x80;
  CYC(0x6d0c, 0x6d0d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d0d, 0x6d0f); mem_wr(gb, HL, 0xfe);
  kingMoblinMinion_animate_hook(gb);
}

void kingMoblinMinion_animate_hook(GB *gb) {
  CYC(0x6d0f, 0x6d12);
  enemyAnimate_hook(gb);
}

void kingMoblinMinion_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d12, 0x6d14); C = 0x20;
  CALL_C(0x6d14, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6d17);
  if (F & FZ) {
    CYCT(0x6d17, 0x6d19);
    goto landed;
  }
  CYC(0x6d17, 0x6d19);
  CYC(0x6d19, 0x6d1a); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6d1a, 0x6d1b); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x6d1b); return; }
  CYC(0x6d1b, 0x6d1c);
  CALL_C(0x6d1c, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6d1f);
  CYC(0x6d1f, 0x6d20); B = A;
  CYC(0x6d20, 0x6d22); A = 0x04;
  CALL_C(0x6d22, objectGetRelatedObject2Var_hook, 0x2164, 0x6d25);
  CYC(0x6d25, 0x6d26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d26, 0x6d28); L = PART_BASE + OBJ_ANGLE;
  CYC(0x6d28, 0x6d29); mem_wr(gb, HL, B);
  RET(0x6d29); return;
landed:
  CYC(0x6d2a, 0x6d2c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d2c, 0x6d2d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d2d, 0x6d2f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d2f, 0x6d31); mem_wr(gb, HL, 0x10);
  CYC(0x6d31, 0x6d33);
  kingMoblinMinion_animate_hook(gb);
}

void kingMoblinMinion_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d33, ecom_decCounter1_b10_hook, 0x439a, 0x6d36);
  if (!(F & FZ)) {
    CYCT(0x6d36, 0x6d38);
    kingMoblinMinion_animate_hook(gb);
    return;
  }
  CYC(0x6d36, 0x6d38);
  CYC(0x6d38, 0x6d3a); mem_wr(gb, HL, 0xc8);
  CYC(0x6d3a, 0x6d3b); L = E;
  CYC(0x6d3b, 0x6d3d); mem_wr(gb, HL, 0x02);
  CYC(0x6d3d, 0x6d3f);
  kingMoblinMinion_animate_hook(gb);
}

// $6d6e is a private per-subid data table (bomb-throw angle pairs), not code; it is only read
// through the RST $10 index below, so it needs no hook entry of its own.
void kingMoblinMinion_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d3f, 0x6d40); H = D;
  CYC(0x6d40, 0x6d41); L = E;
  CYC(0x6d41, 0x6d42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d42, 0x6d44); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d44, 0x6d46); mem_wr(gb, HL, 0x18);
  CYC(0x6d46, 0x6d48); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6d48, 0x6d49); A = mem_rd(gb, HL);
  CYC(0x6d49, 0x6d4a); alu_add(gb, A);
  CYC(0x6d4a, 0x6d4b); A = alu_inc8(gb, A);
  CYC(0x6d4b, 0x6d4c); C = A;
  CYC(0x6d4c, 0x6d4d); A = mem_rd(gb, HL);
  CYC(0x6d4d, 0x6d50); SET_HL(0x6d6e);
  CYC(0x6d50, 0x6d51);
  kingMoblinMinion_addAToHl_from_rst(gb, 0x6d51);
  CYC(0x6d51, 0x6d52); B = mem_rd(gb, HL);
  CYC(0x6d52, 0x6d54); A = 0x04;
  CALL_C(0x6d54, objectGetRelatedObject2Var_hook, 0x2164, 0x6d57);
  CYC(0x6d57, 0x6d58); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d58, 0x6d5b); L = PART_BASE + OBJ_ANGLE;
  CYC(0x6d5b, 0x6d5d); mem_wr(gb, HL, B);
  CYC(0x6d5d, 0x6d5f); L = PART_BASE + OBJ_SPEED;
  CYC(0x6d5f, 0x6d61); mem_wr(gb, HL, 0x37);
  CYC(0x6d61, 0x6d63); L = PART_BASE + OBJ_SPEED_Z;
  CYC(0x6d63, 0x6d64); A = 0x00;
  CYC(0x6d64, 0x6d66); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d66, 0x6d68); mem_wr(gb, HL, 0xff);
  CYC(0x6d68, 0x6d6a); L = PART_BASE + OBJ_VISIBLE;
  CYC(0x6d6a, 0x6d6b); mem_wr(gb, HL, 0x81);
  CYC(0x6d6b, 0x6d6c); A = C;
  CYC(0x6d6c, 0x6d6f);
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d70, ecom_decCounter1_b10_hook, 0x439a, 0x6d73);
  if (!(F & FZ)) { RET_TAKEN(0x6d73); return; }
  CYC(0x6d73, 0x6d74);
  CYC(0x6d74, 0x6d75); L = E;
  CYC(0x6d75, 0x6d76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d76, 0x6d78); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x6d78, 0x6d7a); A = 0xc0;
  CYC(0x6d7a, 0x6d7b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d7b, 0x6d7d); mem_wr(gb, HL, 0xfe);
  CYC(0x6d7d, 0x6d7f); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6d7f, 0x6d81); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x6d81, 0x6d83); C = 0xf4;
  if (F & FZ) {
    CYCT(0x6d83, 0x6d85);
    goto exclaim;
  }
  CYC(0x6d83, 0x6d85);
  CYC(0x6d85, 0x6d87); C = 0x0c;
exclaim:
  CYC(0x6d87, 0x6d89); B = 0xf8;
  CYC(0x6d89, 0x6d8b); A = 0x1e;
  CALL_C(0x6d8b, objectCreateExclamationMark_hook, 0x27e0, 0x6d8e);
  kingMoblinMinion_state9_hook(gb);
}

void kingMoblinMinion_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d8e, 0x6d90); C = 0x20;
  CALL_C(0x6d90, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6d93);
  if (!(F & FZ)) { RET_TAKEN(0x6d93); return; }
  CYC(0x6d93, 0x6d94);
  CYC(0x6d94, 0x6d96); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6d96, 0x6d97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d97, 0x6d99); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6d99, 0x6d9b); mem_wr(gb, HL, 0x0c);
  CYC(0x6d9b, 0x6d9c); L = alu_inc8(gb, L);
  CYC(0x6d9c, 0x6d9e); mem_wr(gb, HL, 0x08);
  CYC(0x6d9e, 0x6d9f); alu_xor(gb, A);
  CYC(0x6d9f, 0x6da2);
  enemySetAnimation_hook(gb);
}

void kingMoblinMinion_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6da2, ecom_decCounter2_b10_hook, 0x43a3, 0x6da5);
  if (!(F & FZ)) {
    CYCT(0x6da5, 0x6da7);
    goto animate;
  }
  CYC(0x6da5, 0x6da7);
  CALL_C(0x6da7, ecom_decCounter1_b10_hook, 0x439a, 0x6daa);
  if (F & FZ) {
    CYCT(0x6daa, 0x6dac);
    goto delete_;
  }
  CYC(0x6daa, 0x6dac);
  CALL_C(0x6dac, objectApplySpeed_hook, 0x201d, 0x6daf);
animate:
  CYC(0x6daf, 0x6db2);
  enemyAnimate_hook(gb);
  return;
delete_:
  CYC(0x6db2, 0x6db4); A = 0x33;
  CALL_C(0x6db4, objectGetRelatedObject1Var_hook, 0x2160, 0x6db7);
  CYC(0x6db7, 0x6db9); mem_wr(gb, HL, 0x01);
  CYC(0x6db9, 0x6dbc);
  enemyDelete_hook(gb);
}
