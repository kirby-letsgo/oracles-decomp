#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/enemies/ramrockArms.s (ENEMY_RAMROCK_ARMS), bank $10.

static uint16_t ramrockArm_jump_table(GB *gb) {
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

static void ramrockArm_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void enemyCode05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6047, 0x6049); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6049, 0x604a); A = mem_rd(gb, DE);
  CYC(0x604a, 0x604b); push_effect(gb, 0x604b);
  switch (ramrockArm_jump_table(gb)) {
    case 0x605d: ramrockArm_state0_hook(gb); return;
    case 0x60d0: ramrockArm_state_stub_hook(gb); return;
    case 0x60d1: ramrockArm_state8_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrockArm_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x605d, 0x605f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x605f, 0x6060); A = mem_rd(gb, DE);
  CYC(0x6060, 0x6062); alu_and(gb, 0x7f);
  CYC(0x6062, 0x6063); push_effect(gb, 0x6063);
  switch (ramrockArm_jump_table(gb)) {
    case 0x606f: goto initSubid0;
    case 0x6098: goto initSubid2;
    case 0x60a7: goto initSubid4;
    default: hook_continue(gb, HL, sp0_); return;
  }
initSubid0:
  CYC(0x606f, 0x6070); A = mem_rd(gb, DE);
  CYC(0x6070, 0x6071); B = A;
  CYC(0x6071, 0x6074); SET_HL(0x6407);
  CYC(0x6074, 0x6075);
  ramrockArm_addAToHl_from_rst(gb, 0x6075);
  CYC(0x6075, 0x6076); A = mem_rd(gb, HL);
  CYC(0x6076, 0x6077); H = D;
  CYC(0x6077, 0x6079); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6079, 0x607a); mem_wr(gb, HL, A);
  CYC(0x607a, 0x607c); L = ENEMY_BASE + OBJ_YH;
  CYC(0x607c, 0x607e); mem_wr(gb, HL, 0x10);
  CYC(0x607e, 0x6080); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x6080, 0x6082); mem_wr(gb, HL, 0xf9);
  CYC(0x6082, 0x6084); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6084, 0x6086); mem_wr(gb, HL, 0x10);
  CYC(0x6086, 0x6088); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6088, 0x608a); mem_wr(gb, HL, 0x08);
  CYC(0x608a, 0x608c); A = 0x00;
  CYC(0x608c, 0x608d); alu_add(gb, B);
  CALL_C(0x608d, enemySetAnimation_hook, 0x282b, 0x6090);
  CYC(0x6090, 0x6092); A = 0x3c;
commonInit:
  CALL_C(0x6092, ecom_setSpeedAndState8_b10_hook, 0x4364, 0x6095);
  CYC(0x6095, 0x6098);
  objectSetVisiblec0_hook(gb);
  return;
initSubid2:
  CYC(0x6098, 0x6099); A = mem_rd(gb, DE);
  CYC(0x6099, 0x609b); alu_add(gb, 0x02);
  CALL_C(0x609b, enemySetAnimation_hook, 0x282b, 0x609e);
  CALL_C(0x609e, ramrockArm_setRelativePosition_hook, 0x63b0, 0x60a1);
  CYC(0x60a1, 0x60a3); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x60a3, 0x60a5); mem_wr(gb, HL, 0x81);
  CYC(0x60a5, 0x6098);
  goto commonInit;
initSubid4:
  CYC(0x60a7, 0x60a8); A = mem_rd(gb, DE);
  CYC(0x60a8, 0x60aa); alu_sub(gb, 0x04);
  CYC(0x60aa, 0x60ab); B = A;
  CYC(0x60ab, 0x60ae); SET_HL(0x640b);
  CYC(0x60ae, 0x60af);
  ramrockArm_addAToHl_from_rst(gb, 0x60af);
  CYC(0x60af, 0x60b0); C = mem_rd(gb, HL);
  CYC(0x60b0, 0x60b1); A = B;
  CYC(0x60b1, 0x60b4); SET_HL(0x6409);
  CYC(0x60b4, 0x60b5);
  ramrockArm_addAToHl_from_rst(gb, 0x60b5);
  CYC(0x60b5, 0x60b6); A = mem_rd(gb, HL);
  CYC(0x60b6, 0x60b7); H = D;
  CYC(0x60b7, 0x60b9); L = ENEMY_BASE + OBJ_XH;
  CYC(0x60b9, 0x60ba); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x60ba, 0x60bb); L = alu_dec8(gb, L);
  CYC(0x60bb, 0x60bd); mem_wr(gb, HL, 0x4e);
  CYC(0x60bd, 0x60bf); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x60bf, 0x60c0); mem_wr(gb, HL, C);
  CYC(0x60c0, 0x60c2); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x60c2, 0x60c4); mem_wr(gb, HL, 0x81);
  CYC(0x60c4, 0x60c6); L = ENEMY_BASE + OBJ_VAR32;
  CYC(0x60c6, 0x60c8); mem_wr(gb, HL, 0x04);
  CYC(0x60c8, 0x60c9); A = mem_rd(gb, DE);
  CYC(0x60c9, 0x60cb); alu_add(gb, 0x02);
  CALL_C(0x60cb, enemySetAnimation_hook, 0x282b, 0x60ce);
  CYC(0x60ce, 0x6092);
  goto commonInit;
}

void ramrockArm_state_stub_hook(GB *gb) {
  RET(0x60d0); return;
}

void ramrockArm_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60d1, 0x60d3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x60d3, 0x60d4); A = mem_rd(gb, DE);
  CYC(0x60d4, 0x60d6); alu_and(gb, 0x7f);
  CYC(0x60d6, 0x60d7); push_effect(gb, 0x60d7);
  switch (ramrockArm_jump_table(gb)) {
    case 0x60e3: ramrockArm_subid0_hook(gb); return;
    case 0x6248: ramrockArm_subid2_hook(gb); return;
    case 0x62e6: ramrockArm_subid4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrockArm_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60e3, 0x60e5); A = 0x02;
  CALL_C(0x60e5, objectGetRelatedObject1Var_hook, 0x2160, 0x60e8);
  CYC(0x60e8, 0x60e9); A = mem_rd(gb, HL);
  CYC(0x60e9, 0x60eb); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x60eb, 0x60ed);
    goto runStates;
  }
  CYC(0x60eb, 0x60ed);
  CYC(0x60ed, 0x60ef); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x60ef, 0x60f0); A = mem_rd(gb, DE);
  CYC(0x60f0, 0x60f1); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x60f1, 0x60f3);
    goto runStates;
  }
  CYC(0x60f1, 0x60f3);
  CYC(0x60f3, 0x60f4); A = alu_inc8(gb, A);
  CYC(0x60f4, 0x60f5); mem_wr(gb, DE, A);
  CYC(0x60f5, 0x60f7); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x60f7, 0x60f9); A = 0x06;
  CYC(0x60f9, 0x60fa); mem_wr(gb, DE, A);
  CYC(0x60fa, 0x60fc); A = 0x3c;
  CYC(0x60fc, 0x60fe); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x60fe, 0x60ff); mem_wr(gb, DE, A);
runStates:
  CYC(0x60ff, 0x6101); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6101, 0x6102); A = mem_rd(gb, DE);
  CYC(0x6102, 0x6103); push_effect(gb, 0x6103);
  switch (ramrockArm_jump_table(gb)) {
    case 0x6111: ramrockArm_subid0_substate0_hook(gb); return;
    case 0x6141: ramrockArm_subid0_substate1_hook(gb); return;
    case 0x6152: ramrockArm_subid0_substate2_hook(gb); return;
    case 0x6187: ramrockArm_subid0_substate3_hook(gb); return;
    case 0x61d3: ramrockArm_subid0_substate4_hook(gb); return;
    case 0x61f4: ramrockArm_subid0_substate5_hook(gb); return;
    case 0x622a: ramrockArm_subid0_substate6_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrockArm_subid0_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6111, enemyAnimate_hook, 0x2818, 0x6114);
  CALL_C(0x6114, objectApplySpeed_hook, 0x201d, 0x6117);
  CALL_C(0x6117, ecom_decCounter1_b10_hook, 0x439a, 0x611a);
  if (!(F & FZ)) { RET_TAKEN(0x611a); return; }
  CYC(0x611a, 0x611b);
  CYC(0x611b, 0x611d); mem_wr(gb, HL, 0x08);
  CYC(0x611d, 0x611f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x611f, 0x6120); A = mem_rd(gb, DE);
  CYC(0x6120, 0x6121); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6121, 0x6123);
    goto afterDec;
  }
  CYC(0x6121, 0x6123);
  CYC(0x6123, 0x6124); A = alu_dec8(gb, A);
afterDec:
  CYC(0x6124, 0x6126); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6126, 0x6127); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6127, 0x6129); alu_and(gb, 0x1f);
  CYC(0x6129, 0x612a); mem_wr(gb, HL, A);
  CYC(0x612a, 0x612c); alu_and(gb, 0x0f);
  CYC(0x612c, 0x612e); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(0x612e); return; }
  CYC(0x612e, 0x612f);
  CYC(0x612f, 0x6131); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x6131, 0x6132); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6132, 0x6134); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6134, 0x6135); B = mem_rd(gb, HL);
  CYC(0x6135, 0x6137); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x6137, 0x6138); H = mem_rd(gb, HL);
  CYC(0x6138, 0x613a); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x613a, 0x613b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x613b, 0x613d); A = 0x02;
  CYC(0x613d, 0x613e); alu_add(gb, B);
  CYC(0x613e, 0x6141);
  enemySetAnimation_hook(gb);
}

void ramrockArm_subid0_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6141, enemyAnimate_hook, 0x2818, 0x6144);
  CALL_C(0x6144, ramrockArm_setRelativePosition_hook, 0x63b0, 0x6147);
  CYC(0x6147, 0x6149); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(0x6149, 0x614a); H = mem_rd(gb, HL);
  CYC(0x614a, 0x614c); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x614c, 0x614e); A = 0x03;
  CYC(0x614e, 0x614f); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x614f); return; }
  CYC(0x614f, 0x6150);
  CYC(0x6150, 0x6152);
  ramrockArm_subid0_moveBackToRamrock_hook(gb);
}

void ramrockArm_subid0_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6152, enemyAnimate_hook, 0x2818, 0x6155);
  CALL_C(0x6155, ramrockArm_setRelativePosition_hook, 0x63b0, 0x6158);
  CALL_C(0x6158, ecom_decCounter2_b10_hook, 0x43a3, 0x615b);
  if (!(F & FZ)) { RET_TAKEN(0x615b); return; }
  CYC(0x615b, 0x615c);
  CYC(0x615c, 0x615e); B = 0x04;
  CALL_C(0x615e, objectCheckCenteredWithLink_hook, 0x1fee, 0x6161);
  if (!(F & FC)) { RET_TAKEN(0x6161); return; }
  CYC(0x6161, 0x6162);
  CALL_C(0x6162, objectGetAngleTowardLink_hook, 0x1e9c, 0x6165);
  CYC(0x6165, 0x6167); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(0x6167); return; }
  CYC(0x6167, 0x6168);
  CALL_C(0x6168, ecom_incSubstate_b10_hook, 0x4005, 0x616b);
  CYC(0x616b, 0x616d); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x616d, 0x616e); mem_wr(gb, HL, A);
  CYC(0x616e, 0x6170); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6170, 0x6172); mem_wr(gb, HL, 0x06);
  CYC(0x6172, 0x6174); L = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(0x6174, 0x6176); mem_wr(gb, HL, 0x00);
  CYC(0x6176, 0x6178); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6178, 0x617a); mem_wr(gb, HL, 0x28);
  CYC(0x617a, 0x617c); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x617c, 0x617e); A = 0x00;
  CYC(0x617e, 0x617f); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x617f, enemySetAnimation_hook, 0x282b, 0x6182);
  CYC(0x6182, 0x6184); A = 0xb1;
  CYC(0x6184, 0x6187);
  playSound_b00_hook(gb);
}

void ramrockArm_subid0_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6187, objectApplySpeed_hook, 0x201d, 0x618a);
  CYC(0x618a, 0x618c); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x618c, 0x618d); A = mem_rd(gb, DE);
  CYC(0x618d, 0x618f); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(0x618f, 0x6191);
    ramrockArm_subid0_moveBackToRamrock_hook(gb);
    return;
  }
  CYC(0x618f, 0x6191);
  CYC(0x6191, 0x6193); alu_cp(gb, 0x84);
  if (F & FZ) {
    CYCT(0x6193, 0x6195);
    goto sword;
  }
  CYC(0x6193, 0x6195);
  CYC(0x6195, 0x6197); alu_cp(gb, 0x85);
  if (F & FZ) {
    CYCT(0x6197, 0x6199);
    goto sword;
  }
  CYC(0x6197, 0x6199);
  CYC(0x6199, 0x619b); alu_cp(gb, 0x86);
  if (!(F & FZ)) {
    CYCT(0x619b, 0x619d);
    goto moveTowardLink;
  }
  CYC(0x619b, 0x619d);
sword:
  CYC(0x619d, 0x619f); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x619f, 0x61a1); A = 0x05;
  CYC(0x61a1, 0x61a2); mem_wr(gb, DE, A);
  CYC(0x61a2, 0x61a4); A = 0x50;
  CYC(0x61a4, 0x61a6); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x61a6, 0x61a7); mem_wr(gb, DE, A);
  CYC(0x61a7, 0x61a9); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x61a9, 0x61aa); A = mem_rd(gb, DE);
  CYC(0x61aa, 0x61ac); alu_xor(gb, 0x10);
  CYC(0x61ac, 0x61ad); mem_wr(gb, DE, A);
  RET(0x61ad); return;
moveTowardLink:
  CALL_C(0x61ae, ecom_getSideviewAdjacentWallsBitset_b10_hook, 0x420b, 0x61b1);
  if (!(F & FZ)) {
    CYCT(0x61b1, 0x61b3);
    ramrockArm_subid0_moveBackToRamrock_hook(gb);
    return;
  }
  CYC(0x61b1, 0x61b3);
  CALL_C(0x61b3, ecom_decCounter1_b10_hook, 0x439a, 0x61b6);
  if (!(F & FZ)) { RET_TAKEN(0x61b6); return; }
  CYC(0x61b6, 0x61b7);
  CYC(0x61b7, 0x61b9); mem_wr(gb, HL, 0x06);
  CALL_C(0x61b9, objectGetAngleTowardLink_hook, 0x1e9c, 0x61bc);
  CYC(0x61bc, 0x61bf);
  objectNudgeAngleTowards_hook(gb);
}

void ramrockArm_subid0_moveBackToRamrock_hook(GB *gb) {
  CYC(0x61bf, 0x61c1); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x61c1, 0x61c3); A = 0x04;
  CYC(0x61c3, 0x61c4); mem_wr(gb, DE, A);
  CYC(0x61c4, 0x61c6); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x61c6, 0x61c8); A = 0x3c;
  CYC(0x61c8, 0x61c9); mem_wr(gb, DE, A);
  ramrockArm_subid0_setAngleTowardRamrock_hook(gb);
}

void ramrockArm_subid0_setAngleTowardRamrock_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x61c9, ramrockArm_getRelativePosition_hook, 0x63dd, 0x61cc);
  CALL_C(0x61cc, objectGetRelativeAngle_hook, 0x1ea4, 0x61cf);
  CYC(0x61cf, 0x61d1); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x61d1, 0x61d2); mem_wr(gb, DE, A);
  RET(0x61d2); return;
}

void ramrockArm_subid0_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x61d3, objectApplySpeed_hook, 0x201d, 0x61d6);
  CALL_C(0x61d6, ramrockArm_subid0_setAngleTowardRamrock_hook, 0x61c9, 0x61d9);
  CALL_C(0x61d9, ramrockArm_subid0_checkReachedRamrock_hook, 0x63bb, 0x61dc);
  if (!(F & FZ)) { RET_TAKEN(0x61dc); return; }
  CYC(0x61dc, 0x61dd);
  CYC(0x61dd, 0x61df); A = 0x52;
  CALL_C(0x61df, playSound_b00_hook, 0x0c98, 0x61e2);
  CYC(0x61e2, 0x61e4); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x61e4, 0x61e6); A = 0x02;
  CYC(0x61e6, 0x61e7); mem_wr(gb, DE, A);
  CYC(0x61e7, 0x61e9); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x61e9, 0x61eb); A = 0x3c;
  CYC(0x61eb, 0x61ec); mem_wr(gb, DE, A);
  CYC(0x61ec, 0x61ee); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x61ee, 0x61ef); A = mem_rd(gb, DE);
  CYC(0x61ef, 0x61f1); alu_add(gb, 0x02);
  CYC(0x61f1, 0x61d3);
  enemySetAnimation_hook(gb);
}

void ramrockArm_subid0_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x61f4, enemyAnimate_hook, 0x2818, 0x61f7);
  CYC(0x61f7, 0x61f9); E = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(0x61f9, 0x61fa); A = mem_rd(gb, DE);
  CYC(0x61fa, 0x61fb); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61fb, 0x61fd);
    goto noDamage;
  }
  CYC(0x61fb, 0x61fd);
  CYC(0x61fd, 0x61ff); A = 0x00;
  CALL_C(0x61ff, objectGetRelatedObject1Var_hook, 0x2160, 0x6202);
  CALL_C(0x6202, checkObjectsCollided_hook, 0x1d5a, 0x6205);
  if (!(F & FC)) {
    CYCT(0x6205, 0x6207);
    goto noDamage;
  }
  CYC(0x6205, 0x6207);
  CYC(0x6207, 0x6209); E = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(0x6209, 0x620b); A = 0x01;
  CYC(0x620b, 0x620c); mem_wr(gb, DE, A);
  CYC(0x620c, 0x620e); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x620e, 0x620f); A = mem_rd(gb, HL);
  CYC(0x620f, 0x6210); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6210, 0x6212);
    goto noDamage;
  }
  CYC(0x6210, 0x6212);
  CYC(0x6212, 0x6214); mem_wr(gb, HL, 0x3c);
  CYC(0x6214, 0x6216); L = ENEMY_BASE + OBJ_SCRIPT_RET;
  CYC(0x6216, 0x6217); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6217, 0x6219); A = 0x63;
  CALL_C(0x6219, playSound_b00_hook, 0x0c98, 0x621c);
noDamage:
  CYC(0x621c, 0x621d); alu_xor(gb, A);
  CALL_C(0x621d, ecom_getSideviewAdjacentWallsBitset_b10_hook, 0x420b, 0x6220);
  if (F & FZ) {
    CYCT(0x6220, 0x6223);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x6220, 0x6223);
  CYC(0x6223, 0x6225); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6225, 0x6226); A = mem_rd(gb, DE);
  CYC(0x6226, 0x6227); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6227); return; }
  CYC(0x6227, 0x6228);
  CYC(0x6228, 0x622a);
  ramrockArm_subid0_moveBackToRamrock_hook(gb);
}

void ramrockArm_subid0_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x622a, 0x622c); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x622c, 0x622d); A = mem_rd(gb, DE);
  CYC(0x622d, 0x622f); alu_add(gb, 0x04);
  CYC(0x622f, 0x6230); B = A;
  CYC(0x6230, 0x6232); A = 0x02;
  CALL_C(0x6232, objectGetRelatedObject1Var_hook, 0x2160, 0x6235);
  CYC(0x6235, 0x6236); A = mem_rd(gb, HL);
  CYC(0x6236, 0x6237); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x6237); return; }
  CYC(0x6237, 0x6238);
  CALL_C(0x6238, ecom_decCounter1_b10_hook, 0x439a, 0x623b);
  if (!(F & FZ)) { RET_TAKEN(0x623b); return; }
  CYC(0x623b, 0x623c);
  CALL_C(0x623c, objectCreatePuff_hook, 0x24c1, 0x623f);
  CYC(0x623f, 0x6241); A = 0x02;
  CALL_C(0x6241, objectGetRelatedObject1Var_hook, 0x2160, 0x6244);
  CYC(0x6244, 0x6245); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6245, 0x6248);
  ramrockArm_deleteSelf_hook(gb);
}

void ramrockArm_setRelativePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63b0, ramrockArm_getRelativePosition_hook, 0x63dd, 0x63b3);
  CYC(0x63b3, 0x63b4); H = D;
  CYC(0x63b4, 0x63b6); L = ENEMY_BASE + OBJ_YH;
  CYC(0x63b6, 0x63b7); mem_wr(gb, HL, B);
  CYC(0x63b7, 0x63b9); L = ENEMY_BASE + OBJ_XH;
  CYC(0x63b9, 0x63ba); mem_wr(gb, HL, C);
  RET(0x63ba); return;
}

void ramrockArm_subid0_checkReachedRamrock_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x63bb, ramrockArm_getRelativePosition_hook, 0x63dd, 0x63be);
  CYC(0x63be, 0x63c0); E = 0x02;
  ramrockArm_checkPositionAtRamrock_hook(gb);
}

void ramrockArm_checkPositionAtRamrock_hook(GB *gb) {
  CYC(0x63c0, 0x63c1); H = D;
  CYC(0x63c1, 0x63c3); L = ENEMY_BASE + OBJ_YH;
  CYC(0x63c3, 0x63c4); A = E;
  CYC(0x63c4, 0x63c5); alu_add(gb, B);
  CYC(0x63c5, 0x63c6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x63c6, 0x63c8);
    label_10_212_hook(gb);
    return;
  }
  CYC(0x63c6, 0x63c8);
  CYC(0x63c8, 0x63c9); alu_sub(gb, E);
  label_10_211_hook(gb);
}

void label_10_211_hook(GB *gb) {
  CYC(0x63c9, 0x63ca); alu_sub(gb, E);
  CYC(0x63ca, 0x63cb); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x63cb, 0x63cd);
    label_10_212_hook(gb);
    return;
  }
  CYC(0x63cb, 0x63cd);
  CYC(0x63cd, 0x63cf); L = ENEMY_BASE + OBJ_XH;
  CYC(0x63cf, 0x63d0); A = E;
  CYC(0x63d0, 0x63d1); alu_add(gb, C);
  CYC(0x63d1, 0x63d2); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x63d2, 0x63d4);
    label_10_212_hook(gb);
    return;
  }
  CYC(0x63d2, 0x63d4);
  CYC(0x63d4, 0x63d5); alu_sub(gb, E);
  CYC(0x63d5, 0x63d6); alu_sub(gb, E);
  CYC(0x63d6, 0x63d7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(0x63d7, 0x63d9);
    label_10_212_hook(gb);
    return;
  }
  CYC(0x63d7, 0x63d9);
  CYC(0x63d9, 0x63da); alu_xor(gb, A);
  RET(0x63da); return;
}

void label_10_212_hook(GB *gb) {
  CYC(0x63db, 0x63dc); alu_or(gb, D);
  RET(0x63dc); return;
}

void ramrockArm_subid2_copyRamrockPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63f5, 0x63f7); A = 0x0b;
  CALL_C(0x63f7, objectGetRelatedObject1Var_hook, 0x2160, 0x63fa);
  CYC(0x63fa, 0x63fb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63fb, 0x63fd); alu_add(gb, 0x08);
  CYC(0x63fd, 0x63fe); B = A;
  CYC(0x63fe, 0x63ff); L = alu_inc8(gb, L);
  CYC(0x63ff, 0x6400); A = mem_rd(gb, HL);
  CYC(0x6400, 0x6401); H = D;
  CYC(0x6401, 0x6403); L = ENEMY_BASE + OBJ_XH;
  CYC(0x6403, 0x6404); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6404, 0x6405); L = alu_dec8(gb, L);
  CYC(0x6405, 0x6406); mem_wr(gb, HL, B);
  RET(0x6406); return;
}

void ramrockArm_deleteSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x62e0, decNumEnemies_hook, 0x24b3, 0x62e3);
  CYC(0x62e3, 0x62e6);
  enemyDelete_hook(gb);
}

void ramrockArm_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6248, 0x624a); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x624a, 0x624b); A = mem_rd(gb, DE);
  CYC(0x624b, 0x624c); push_effect(gb, 0x624c);
  switch (ramrockArm_jump_table(gb)) {
    case 0x6252: ramrockArm_subid2_substate0_hook(gb); return;
    case 0x6267: ramrockArm_subid2_substate1_hook(gb); return;
    case 0x627c: ramrockArm_subid2_substate2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrockArm_subid2_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6252, 0x6254); C = 0x10;
  CALL_C(0x6254, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6257);
  if (!(F & FZ)) { RET_TAKEN(0x6257); return; }
  CYC(0x6257, 0x6258);
  CYC(0x6258, 0x625a); A = 0x02;
  CALL_C(0x625a, objectGetRelatedObject1Var_hook, 0x2160, 0x625d);
  CYC(0x625d, 0x625f); mem_wr(gb, HL, 0x07);
  CYC(0x625f, 0x6261); A = 0x85;
  CALL_C(0x6261, playSound_b00_hook, 0x0c98, 0x6264);
  CYC(0x6264, 0x6267);
  ecom_incSubstate_b10_hook(gb);
}

void ramrockArm_subid2_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6267, 0x6269); A = 0x02;
  CALL_C(0x6269, objectGetRelatedObject1Var_hook, 0x2160, 0x626c);
  CYC(0x626c, 0x626d); A = mem_rd(gb, HL);
  CYC(0x626d, 0x626f); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(0x626f); return; }
  CYC(0x626f, 0x6270);
  CYC(0x6270, 0x6271); H = D;
  CYC(0x6271, 0x6272); A = mem_rd(gb, HL);
  CYC(0x6272, 0x6273); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x6273, 0x6275);
    ramrockArm_deleteSelf_hook(gb);
    return;
  }
  CYC(0x6273, 0x6275);
  CYC(0x6275, 0x6277); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x6277, 0x6279); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x6279, 0x627c);
  ecom_incSubstate_b10_hook(gb);
}

void ramrockArm_subid2_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x627c, ramrockArm_subid2_copyRamrockPosition_hook, 0x63f5, 0x627f);
  CYC(0x627f, 0x6281); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6281, 0x6283); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x6283, 0x6285); A = 0x02;
  CALL_C(0x6285, objectGetRelatedObject1Var_hook, 0x2160, 0x6288);
  CYC(0x6288, 0x6289); A = mem_rd(gb, HL);
  CYC(0x6289, 0x628b); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x628b, 0x628d);
    goto relatedSubid0a;
  }
  CYC(0x628b, 0x628d);
  CYC(0x628d, 0x628f); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(0x628f); return; }
  CYC(0x628f, 0x6290);
relatedSubid09:
  CYC(0x6290, 0x6291); H = D;
  CYC(0x6291, 0x6293); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6293, 0x6295); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6295, 0x6297); C = 0x03;
  CALL_C(0x6297, findItemWithID_hook, 0x22b9, 0x629a);
  if (!(F & FZ)) { RET_TAKEN(0x629a); return; }
  CYC(0x629a, 0x629b);
  CYC(0x629b, 0x629d); L = 0x0b;
  CYC(0x629d, 0x629e); B = mem_rd(gb, HL);
  CYC(0x629e, 0x62a0); L = 0x0d;
  CYC(0x62a0, 0x62a1); C = mem_rd(gb, HL);
  PUSH(0x62a1, HL);
  CYC(0x62a2, 0x62a4); E = 0x06;
  CALL_C(0x62a4, ramrockArm_checkPositionAtRamrock_hook, 0x63c0, 0x62a7);
  SET_HL(POP(0x62a7));
  if (!(F & FZ)) { RET_TAKEN(0x62a8); return; }
  CYC(0x62a8, 0x62a9);
  CYC(0x62a9, 0x62ab); L = 0x0f;
  CYC(0x62ab, 0x62ac); A = mem_rd(gb, HL);
  CYC(0x62ac, 0x62ad); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x62ad, 0x62af);
    goto setSparkle;
  }
  CYC(0x62ad, 0x62af);
  CYC(0x62af, 0x62b1); alu_cp(gb, 0xfc);
  if (F & FC) { RET_TAKEN(0x62b1); return; }
  CYC(0x62b1, 0x62b2);
setSparkle:
  CYC(0x62b2, 0x62b4); L = 0x2f;
  CYC(0x62b4, 0x62b6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4)));
  CYC(0x62b6, 0x62b8); A = 0x2b;
  CALL_C(0x62b8, objectGetRelatedObject1Var_hook, 0x2160, 0x62bb);
  CYC(0x62bb, 0x62bc); A = mem_rd(gb, HL);
  CYC(0x62bc, 0x62bd); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x62bd); return; }
  CYC(0x62bd, 0x62be);
  CYC(0x62be, 0x62c0); mem_wr(gb, HL, 0x3c);
  CYC(0x62c0, 0x62c2); L = ENEMY_BASE + OBJ_SCRIPT_RET;
  CYC(0x62c2, 0x62c3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x62c3); return;
relatedSubid0a:
  CYC(0x62c4, 0x62c6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x62c6, 0x62c8); A = 0x01;
  CYC(0x62c8, 0x62c9); mem_wr(gb, DE, A);
nextPuff:
  CALL_C(0x62c9, getFreeInteractionSlot_hook, 0x3aef, 0x62cc);
  CYC(0x62cc, 0x62ce); mem_wr(gb, HL, 0x05);
  PUSH(0x62ce, HL);
  CALL_C(0x62cf, ramrockArm_setRelativePosition_hook, 0x63b0, 0x62d2);
  SET_HL(POP(0x62d2));
  CALL_C(0x62d3, objectCopyPosition_hook, 0x2242, 0x62d6);
  CYC(0x62d6, 0x62d8); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x62d8, 0x62d9); A = mem_rd(gb, DE);
  CYC(0x62d9, 0x62da); A = alu_dec8(gb, A);
  CYC(0x62da, 0x62db); mem_wr(gb, DE, A);
  if (F & FZ) {
    CYCT(0x62db, 0x62dd);
    goto nextPuff;
  }
  CYC(0x62db, 0x62dd);
  CYC(0x62dd, 0x62df); A = 0x02;
  CYC(0x62df, 0x62e0); mem_wr(gb, DE, A);
  ramrockArm_deleteSelf_hook(gb);
}

void ramrockArm_subid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62e6, 0x62e8); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x62e8, 0x62e9); A = mem_rd(gb, DE);
  CYC(0x62e9, 0x62ea); push_effect(gb, 0x62ea);
  switch (ramrockArm_jump_table(gb)) {
    case 0x62f2: ramrockArm_subid4_substate0_hook(gb); return;
    case 0x630e: ramrockArm_subid4_substate1_hook(gb); return;
    case 0x6332: ramrockArm_subid4_substate2_hook(gb); return;
    case 0x6381: ramrockArm_subid4_substate3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ramrockArm_subid4_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62f2, 0x62f4); C = 0x10;
  CALL_C(0x62f4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x62f7);
  if (!(F & FZ)) { RET_TAKEN(0x62f7); return; }
  CYC(0x62f7, 0x62f8);
  CYC(0x62f8, 0x62fa); A = 0x06;
  CALL_C(0x62fa, objectSetCollideRadius_hook, 0x24a1, 0x62fd);
  CYC(0x62fd, 0x6300); SET_BC(0xff80);
  CALL_C(0x6300, objectSetSpeedZ_hook, 0x239d, 0x6303);
  CYC(0x6303, 0x6305); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6305, 0x6307); mem_wr(gb, HL, 0x28);
  CYC(0x6307, 0x6309); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6309, 0x630b); mem_wr(gb, HL, 0x3e);
  CYC(0x630b, 0x630e);
  ecom_incSubstate_b10_hook(gb);
}

void ramrockArm_subid4_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x630e, 0x6310); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x6310, 0x6311); A = mem_rd(gb, DE);
  CYC(0x6311, 0x6313); alu_cp(gb, 0xf9);
  CYC(0x6313, 0x6315); C = 0x00;
  if (!(F & FZ)) {
    CYCT(0x6315, 0x6318);
    objectUpdateSpeedZ_paramC_hook(gb);
    return;
  }
  CYC(0x6315, 0x6318);
  CALL_C(0x6318, ecom_decCounter2_b10_hook, 0x43a3, 0x631b);
  if (!(F & FZ)) {
    CYCT(0x631b, 0x631e);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x631b, 0x631e);
  CALL_C(0x631e, ecom_incSubstate_b10_hook, 0x4005, 0x6321);
  CYC(0x6321, 0x6323); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6323, 0x6324); A = mem_rd(gb, DE);
  CYC(0x6324, 0x6325); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x6325); return; }
  CYC(0x6325, 0x6326);
  CYC(0x6326, 0x6328); A = 0x02;
  CALL_C(0x6328, objectGetRelatedObject1Var_hook, 0x2160, 0x632b);
  CYC(0x632b, 0x632d); mem_wr(gb, HL, 0x0c);
  CYC(0x632d, 0x632f); A = 0x84;
  CYC(0x632f, 0x6332);
  loadPaletteHeader_hook(gb);
}

void ramrockArm_subid4_updateXPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x635f, 0x6361); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6361, 0x6362); A = mem_rd(gb, DE);
  CYC(0x6362, 0x6363); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x6363, 0x6365);
    goto related;
  }
  CYC(0x6363, 0x6365);
  CYC(0x6365, 0x6366); A = B;
  CYC(0x6366, 0x6367); alu_cpl(gb);
  CYC(0x6367, 0x6368); A = alu_inc8(gb, A);
  CYC(0x6368, 0x6369); B = A;
related:
  CYC(0x6369, 0x636b); A = 0x0d;
  CALL_C(0x636b, objectGetRelatedObject1Var_hook, 0x2160, 0x636e);
  CYC(0x636e, 0x636f); A = mem_rd(gb, HL);
  CYC(0x636f, 0x6370); alu_add(gb, B);
  CYC(0x6370, 0x6371); E = L;
  CYC(0x6371, 0x6372); mem_wr(gb, DE, A);
  RET(0x6372); return;
}

void ramrockArm_subid4_collisionOccurred_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6373, 0x6375); A = 0x02;
  CALL_C(0x6375, objectGetRelatedObject1Var_hook, 0x2160, 0x6378);
  CYC(0x6378, 0x637a); mem_wr(gb, HL, 0x0d);
  CYC(0x637a, 0x637c); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x637c, 0x637e); mem_wr(gb, HL, 0x10);
  CYC(0x637e, 0x6381);
  ecom_incSubstate_b10_hook(gb);
}

void ramrockArm_subid4_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6332, 0x6334); A = 0x05;
  CALL_C(0x6334, objectGetRelatedObject1Var_hook, 0x2160, 0x6337);
  CYC(0x6337, 0x6338); A = mem_rd(gb, HL);
  CYC(0x6338, 0x6339); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x6339, 0x633b);
    ramrockArm_subid4_substate3_hook(gb);
    return;
  }
  CYC(0x6339, 0x633b);
  CYC(0x633b, 0x633d); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x633d, 0x633e); A = mem_rd(gb, DE);
  CYC(0x633e, 0x633f); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x633f, 0x6341);
    ramrockArm_subid4_collisionOccurred_hook(gb);
    return;
  }
  CYC(0x633f, 0x6341);
  CYC(0x6341, 0x6343); A = 0x02;
  CALL_C(0x6343, objectGetRelatedObject1Var_hook, 0x2160, 0x6346);
  CYC(0x6346, 0x6347); A = mem_rd(gb, HL);
  CYC(0x6347, 0x6349); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(0x6349, 0x634b);
    ramrockArm_subid4_collisionOccurred_hook(gb);
    return;
  }
  CYC(0x6349, 0x634b);
  CYC(0x634b, 0x634d); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x634d, 0x634f);
    ramrockArm_subid4_substate3_hook(gb);
    return;
  }
  CYC(0x634d, 0x634f);
  CYC(0x634f, 0x6351); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x6351, 0x6352); A = mem_rd(gb, DE);
  CYC(0x6352, 0x6353); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6353, 0x6355);
    ramrockArm_subid4_substate3_hook(gb);
    return;
  }
  CYC(0x6353, 0x6355);
  CYC(0x6355, 0x6356); A = alu_dec8(gb, A);
  CYC(0x6356, 0x6357); mem_wr(gb, DE, A);
  if (F & FZ) {
    CYCT(0x6357, 0x6359);
    ramrockArm_subid4_collisionOccurred_hook(gb);
    return;
  }
  CYC(0x6357, 0x6359);
  CYC(0x6359, 0x635f);
  ramrockArm_subid4_updateXPosition_hook(gb);
}

void ramrockArm_subid4_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6381, 0x6383); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6383, 0x6384); A = mem_rd(gb, DE);
  CYC(0x6384, 0x6385); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x6385, 0x6387);
    goto afterCollision;
  }
  CYC(0x6385, 0x6387);
  CYC(0x6387, 0x6389); A = 0x36;
  CALL_C(0x6389, objectGetRelatedObject1Var_hook, 0x2160, 0x638c);
  CYC(0x638c, 0x638e); mem_wr(gb, HL, 0x10);
afterCollision:
  CYC(0x638e, 0x6390); E = ENEMY_BASE + OBJ_TEXT_ID;
  CYC(0x6390, 0x6391); A = mem_rd(gb, DE);
  CYC(0x6391, 0x6393); alu_sub(gb, 0x02);
  CYC(0x6393, 0x6395); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(0x6395, 0x6397);
    goto atLimit;
  }
  CYC(0x6395, 0x6397);
  CYC(0x6397, 0x6399); B = 0x04;
  CYC(0x6399, 0x639b);
  goto checkSubid;
atLimit:
  CYC(0x639b, 0x639c); mem_wr(gb, DE, A);
  CYC(0x639c, 0x639d); B = A;
  CYC(0x639d, 0x63a0);
  ramrockArm_subid4_updateXPosition_hook(gb);
  return;
checkSubid:
  CYC(0x639f, 0x63a1); A = 0x02;
  CALL_C(0x63a1, objectGetRelatedObject1Var_hook, 0x2160, 0x63a4);
  CYC(0x63a4, 0x63a5); A = mem_rd(gb, HL);
  CYC(0x63a5, 0x63a7); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(0x63a7, 0x63a9);
    ramrockArm_subid4_updateXPosition_hook(gb);
    return;
  }
  CYC(0x63a7, 0x63a9);
  CYC(0x63a9, 0x63ab); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x63ab, 0x63ad); A = 0x02;
  CYC(0x63ad, 0x63ae); mem_wr(gb, DE, A);
  CYC(0x63ae, 0x635f);
  ramrockArm_subid4_updateXPosition_hook(gb);
}

void ramrockArm_getRelativePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63dd, 0x63df); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x63df, 0x63e0); A = mem_rd(gb, DE);
  CYC(0x63e0, 0x63e2); C = 0x0e;
  CYC(0x63e2, 0x63e3); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x63e3, 0x63e5);
    goto related;
  }
  CYC(0x63e3, 0x63e5);
  CYC(0x63e5, 0x63e7); C = 0xf2;
related:
  CYC(0x63e7, 0x63e9); A = 0x0b;
  CALL_C(0x63e9, objectGetRelatedObject1Var_hook, 0x2160, 0x63ec);
  CYC(0x63ec, 0x63ed); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x63ed, 0x63ef); alu_add(gb, 0x08);
  CYC(0x63ef, 0x63f0); B = A;
  CYC(0x63f0, 0x63f1); L = alu_inc8(gb, L);
  CYC(0x63f1, 0x63f2); A = mem_rd(gb, HL);
  CYC(0x63f2, 0x63f3); alu_add(gb, C);
  CYC(0x63f3, 0x63f4); C = A;
  RET(0x63f4); return;
}
