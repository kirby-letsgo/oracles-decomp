#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ramrockArm_jump_table(gb));
    if (jt_ == SYM(ramrockArm_state0) && hook_is(gb, SYM(ramrockArm_state0), ramrockArm_state0_hook)) { ramrockArm_state0_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_state_stub) && hook_is(gb, SYM(ramrockArm_state_stub), ramrockArm_state_stub_hook)) { ramrockArm_state_stub_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_state8) && hook_is(gb, SYM(ramrockArm_state8), ramrockArm_state8_hook)) { ramrockArm_state8_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrockArm_state0_hook(GB *gb) {
  BASE(ramrockArm_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x7f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (ramrockArm_jump_table(gb));
    if (jt_ == b_+18) { goto initSubid0; }
    else if (jt_ == b_+59) { goto initSubid2; }
    else if (jt_ == b_+74) { goto initSubid4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
initSubid0:
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+23); SET_HL(SYM(ramrockArm_subid0And1XPositions));
  CYC(b_+23, b_+24);
  ramrockArm_addAToHl_from_rst(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x10);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0xf9);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x10);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x08);
  CYC(b_+45, b_+47); A = 0x00;
  CYC(b_+47, b_+48); alu_add(gb, B);
  CALL_C(b_+48, enemySetAnimation_hook, SYM(enemySetAnimation), b_+51);
  CYC(b_+51, b_+53); A = 0x3c;
commonInit:
  CALL_C(b_+53, ecom_setSpeedAndState8_b10_hook, SYM(ecom_setSpeedAndState8_b10), b_+56);
  CYC(b_+56, b_+59);
  TAIL(objectSetVisiblec0);
initSubid2:
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+62); alu_add(gb, 0x02);
  CALL_C(b_+62, enemySetAnimation_hook, SYM(enemySetAnimation), b_+65);
  CALL_C(b_+65, ramrockArm_setRelativePosition_hook, SYM(ramrockArm_setRelativePosition), b_+68);
  CYC(b_+68, b_+70); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x81);
  CYC(b_+72, b_+74);
  goto commonInit;
initSubid4:
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+77); alu_sub(gb, 0x04);
  CYC(b_+77, b_+78); B = A;
  CYC(b_+78, b_+81); SET_HL(SYM(ramrockArm_subid4And5Angles));
  CYC(b_+81, b_+82);
  ramrockArm_addAToHl_from_rst(gb, b_+82);
  CYC(b_+82, b_+83); C = mem_rd(gb, HL);
  CYC(b_+83, b_+84); A = B;
  CYC(b_+84, b_+87); SET_HL(SYM(ramrockArm_subid4And5XPositions));
  CYC(b_+87, b_+88);
  ramrockArm_addAToHl_from_rst(gb, b_+88);
  CYC(b_+88, b_+89); A = mem_rd(gb, HL);
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+92, b_+93); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+93, b_+94); L = alu_dec8(gb, L);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x4e);
  CYC(b_+96, b_+98); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+98, b_+99); mem_wr(gb, HL, C);
  CYC(b_+99, b_+101); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x81);
  CYC(b_+103, b_+105); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x04);
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+110); alu_add(gb, 0x02);
  CALL_C(b_+110, enemySetAnimation_hook, SYM(enemySetAnimation), b_+113);
  CYC(b_+113, b_+115);
  goto commonInit;
}

void ramrockArm_state_stub_hook(GB *gb) {
  BASE(ramrockArm_state_stub);
  RET(b_+0); return;
}

void ramrockArm_state8_hook(GB *gb) {
  BASE(ramrockArm_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x7f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (ramrockArm_jump_table(gb));
    if (jt_ == SYM(ramrockArm_subid0) && hook_is(gb, SYM(ramrockArm_subid0), ramrockArm_subid0_hook)) { ramrockArm_subid0_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid2) && hook_is(gb, SYM(ramrockArm_subid2), ramrockArm_subid2_hook)) { ramrockArm_subid2_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid4) && hook_is(gb, SYM(ramrockArm_subid4), ramrockArm_subid4_hook)) { ramrockArm_subid4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrockArm_subid0_hook(GB *gb) {
  BASE(ramrockArm_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto runStates;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
    goto runStates;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+20, b_+22); A = 0x06;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); A = 0x3c;
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
runStates:
  CYC(b_+28, b_+30); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (ramrockArm_jump_table(gb));
    if (jt_ == SYM(ramrockArm_subid0_substate0) && hook_is(gb, SYM(ramrockArm_subid0_substate0), ramrockArm_subid0_substate0_hook)) { ramrockArm_subid0_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid0_substate1) && hook_is(gb, SYM(ramrockArm_subid0_substate1), ramrockArm_subid0_substate1_hook)) { ramrockArm_subid0_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid0_substate2) && hook_is(gb, SYM(ramrockArm_subid0_substate2), ramrockArm_subid0_substate2_hook)) { ramrockArm_subid0_substate2_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid0_substate3) && hook_is(gb, SYM(ramrockArm_subid0_substate3), ramrockArm_subid0_substate3_hook)) { ramrockArm_subid0_substate3_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid0_substate4) && hook_is(gb, SYM(ramrockArm_subid0_substate4), ramrockArm_subid0_substate4_hook)) { ramrockArm_subid0_substate4_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid0_substate5) && hook_is(gb, SYM(ramrockArm_subid0_substate5), ramrockArm_subid0_substate5_hook)) { ramrockArm_subid0_substate5_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid0_substate6) && hook_is(gb, SYM(ramrockArm_subid0_substate6), ramrockArm_subid0_substate6_hook)) { ramrockArm_subid0_substate6_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrockArm_subid0_substate0_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  CALL_C(b_+6, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x08);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18);
    goto afterDec;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); A = alu_dec8(gb, A);
afterDec:
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+24); alu_and(gb, 0x1f);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); alu_and(gb, 0x0f);
  CYC(b_+27, b_+29); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+35, b_+36); B = mem_rd(gb, HL);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+38, b_+39); H = mem_rd(gb, HL);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+42, b_+44); A = 0x02;
  CYC(b_+44, b_+45); alu_add(gb, B);
  CYC(b_+45, b_+48);
  TAIL(enemySetAnimation);
}

void ramrockArm_subid0_substate1_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ramrockArm_setRelativePosition_hook, SYM(ramrockArm_setRelativePosition), b_+6);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+13); A = 0x03;
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17);
  TAIL(ramrockArm_subid0_moveBackToRamrock);
}

void ramrockArm_subid0_substate2_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ramrockArm_setRelativePosition_hook, SYM(ramrockArm_setRelativePosition), b_+6);
  CALL_C(b_+6, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); B = 0x04;
  CALL_C(b_+12, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+15);
  if (!(F & FC)) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CALL_C(b_+22, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+25);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x06);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x28);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+42, b_+44); A = 0x00;
  CYC(b_+44, b_+45); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+45, enemySetAnimation_hook, SYM(enemySetAnimation), b_+48);
  CYC(b_+48, b_+50); A = 0xb1;
  CYC(b_+50, b_+53);
  TAIL(playSound_b00);
}

void ramrockArm_subid0_substate3_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
    TAIL(ramrockArm_subid0_moveBackToRamrock);
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x84);
  if (F & FZ) {
    CYCT(b_+12, b_+14);
    goto sword;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 0x85);
  if (F & FZ) {
    CYCT(b_+16, b_+18);
    goto sword;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); alu_cp(gb, 0x86);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+22);
    goto moveTowardLink;
  }
  CYC(b_+20, b_+22);
sword:
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+24, b_+26); A = 0x05;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); A = 0x50;
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+37); alu_xor(gb, 0x10);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  RET(b_+38); return;
moveTowardLink:
  CALL_C(b_+39, ecom_getSideviewAdjacentWallsBitset_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b10), b_+42);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+44);
    TAIL(ramrockArm_subid0_moveBackToRamrock);
  }
  CYC(b_+42, b_+44);
  CALL_C(b_+44, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+47);
  if (!(F & FZ)) { RET_TAKEN(b_+47); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x06);
  CALL_C(b_+50, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+53);
  CYC(b_+53, b_+56);
  TAIL(objectNudgeAngleTowards);
}

void ramrockArm_subid0_moveBackToRamrock_hook(GB *gb) {
  BASE(ramrockArm_subid0_moveBackToRamrock);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+4); A = 0x04;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+7, b_+9); A = 0x3c;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  TAIL(ramrockArm_subid0_setAngleTowardRamrock);
}

void ramrockArm_subid0_setAngleTowardRamrock_hook(GB *gb) {
  BASE(ramrockArm_subid0_setAngleTowardRamrock);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ramrockArm_getRelativePosition_hook, SYM(ramrockArm_getRelativePosition), b_+3);
  CALL_C(b_+3, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  RET(b_+9); return;
}

void ramrockArm_subid0_substate4_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ramrockArm_subid0_setAngleTowardRamrock_hook, SYM(ramrockArm_subid0_setAngleTowardRamrock), b_+6);
  CALL_C(b_+6, ramrockArm_subid0_checkReachedRamrock_hook, SYM(ramrockArm_subid0_checkReachedRamrock), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x52;
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+17, b_+19); A = 0x02;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+22, b_+24); A = 0x3c;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_add(gb, 0x02);
  CYC(b_+30, b_+33);
  TAIL(enemySetAnimation);
}

void ramrockArm_subid0_substate5_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+9);
    goto noDamage;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x00;
  CALL_C(b_+11, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+14);
  CALL_C(b_+14, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+17);
  if (!(F & FC)) {
    CYCT(b_+17, b_+19);
    goto noDamage;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+21, b_+23); A = 0x01;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+28, b_+30);
    goto noDamage;
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x3c);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_SCRIPT_RET;
  CYC(b_+34, b_+35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+35, b_+37); A = 0x63;
  CALL_C(b_+37, playSound_b00_hook, SYM(playSound_b00), b_+40);
noDamage:
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CALL_C(b_+41, ecom_getSideviewAdjacentWallsBitset_b10_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b10), b_+44);
  if (F & FZ) {
    CYCT(b_+44, b_+47);
    TAIL(objectApplySpeed);
  }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+49); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54);
  TAIL(ramrockArm_subid0_moveBackToRamrock);
}

void ramrockArm_subid0_substate6_hook(GB *gb) {
  BASE(ramrockArm_subid0_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); A = 0x02;
  CALL_C(b_+8, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CALL_C(b_+14, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+17);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, objectCreatePuff_hook, SYM(objectCreatePuff), b_+21);
  CYC(b_+21, b_+23); A = 0x02;
  CALL_C(b_+23, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+30);
  TAIL(ramrockArm_deleteSelf);
}

void ramrockArm_setRelativePosition_hook(GB *gb) {
  BASE(ramrockArm_setRelativePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ramrockArm_getRelativePosition_hook, SYM(ramrockArm_getRelativePosition), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+7); mem_wr(gb, HL, B);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); mem_wr(gb, HL, C);
  RET(b_+10); return;
}

void ramrockArm_subid0_checkReachedRamrock_hook(GB *gb) {
  BASE(ramrockArm_subid0_checkReachedRamrock);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ramrockArm_getRelativePosition_hook, SYM(ramrockArm_getRelativePosition), b_+3);
  CYC(b_+3, b_+5); E = 0x02;
  TAIL(ramrockArm_checkPositionAtRamrock);
}

void ramrockArm_checkPositionAtRamrock_hook(GB *gb) {
  BASE(ramrockArm_checkPositionAtRamrock);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); A = E;
  CYC(b_+4, b_+5); alu_add(gb, B);
  CYC(b_+5, b_+6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+6, b_+8);
    TAIL(label_10_212);
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); alu_sub(gb, E);
  TAIL(label_10_211);
}

void label_10_211_hook(GB *gb) {
  BASE(label_10_211);
  CYC(b_+0, b_+1); alu_sub(gb, E);
  CYC(b_+1, b_+2); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+2, b_+4);
    TAIL(label_10_212);
  }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); A = E;
  CYC(b_+7, b_+8); alu_add(gb, C);
  CYC(b_+8, b_+9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+9, b_+11);
    TAIL(label_10_212);
  }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); alu_sub(gb, E);
  CYC(b_+12, b_+13); alu_sub(gb, E);
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) {
    CYCT(b_+14, b_+16);
    TAIL(label_10_212);
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  RET(b_+17); return;
}

void label_10_212_hook(GB *gb) {
  BASE(label_10_212);
  CYC(b_+0, b_+1); alu_or(gb, D);
  RET(b_+1); return;
}

void ramrockArm_subid2_copyRamrockPosition_hook(GB *gb) {
  BASE(ramrockArm_subid2_copyRamrockPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+8); alu_add(gb, 0x08);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+15, b_+16); L = alu_dec8(gb, L);
  CYC(b_+16, b_+17); mem_wr(gb, HL, B);
  RET(b_+17); return;
}

void ramrockArm_deleteSelf_hook(GB *gb) {
  BASE(ramrockArm_deleteSelf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6);
  TAIL(enemyDelete);
}

void ramrockArm_subid2_hook(GB *gb) {
  BASE(ramrockArm_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ramrockArm_jump_table(gb));
    if (jt_ == SYM(ramrockArm_subid2_substate0) && hook_is(gb, SYM(ramrockArm_subid2_substate0), ramrockArm_subid2_substate0_hook)) { ramrockArm_subid2_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid2_substate1) && hook_is(gb, SYM(ramrockArm_subid2_substate1), ramrockArm_subid2_substate1_hook)) { ramrockArm_subid2_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid2_substate2) && hook_is(gb, SYM(ramrockArm_subid2_substate2), ramrockArm_subid2_substate2_hook)) { ramrockArm_subid2_substate2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrockArm_subid2_substate0_hook(GB *gb) {
  BASE(ramrockArm_subid2_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CALL_C(b_+8, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x07);
  CYC(b_+13, b_+15); A = 0x85;
  CALL_C(b_+15, playSound_b00_hook, SYM(playSound_b00), b_+18);
  CYC(b_+18, b_+21);
  TAIL(ecom_incSubstate_b10);
}

void ramrockArm_subid2_substate1_hook(GB *gb) {
  BASE(ramrockArm_subid2_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+12, b_+14);
    TAIL(ramrockArm_deleteSelf);
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+18, b_+21);
  TAIL(ecom_incSubstate_b10);
}

void ramrockArm_subid2_substate2_hook(GB *gb) {
  BASE(ramrockArm_subid2_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ramrockArm_subid2_copyRamrockPosition_hook, SYM(ramrockArm_subid2_copyRamrockPosition), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+7, b_+9); A = 0x02;
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    goto relatedSubid0a;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
relatedSubid09:
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+25, b_+27); C = 0x03;
  CALL_C(b_+27, findItemWithID_hook, SYM(findItemWithID), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); L = 0x0b;
  CYC(b_+33, b_+34); B = mem_rd(gb, HL);
  CYC(b_+34, b_+36); L = 0x0d;
  CYC(b_+36, b_+37); C = mem_rd(gb, HL);
  PUSH(b_+37, HL);
  CYC(b_+38, b_+40); E = 0x06;
  CALL_C(b_+40, ramrockArm_checkPositionAtRamrock_hook, SYM(ramrockArm_checkPositionAtRamrock), b_+43);
  SET_HL(POP(b_+43));
  if (!(F & FZ)) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); L = 0x0f;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+49); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+49, b_+51);
    goto setSparkle;
  }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); alu_cp(gb, 0xfc);
  if (F & FC) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
setSparkle:
  CYC(b_+54, b_+56); L = 0x2f;
  CYC(b_+56, b_+58); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4)));
  CYC(b_+58, b_+60); A = 0x2b;
  CALL_C(b_+60, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x3c);
  CYC(b_+68, b_+70); L = ENEMY_BASE + OBJ_SCRIPT_RET;
  CYC(b_+70, b_+71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+71); return;
relatedSubid0a:
  CYC(b_+72, b_+74); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+74, b_+76); A = 0x01;
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
nextPuff:
  CALL_C(b_+77, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x05);
  PUSH(b_+82, HL);
  CALL_C(b_+83, ramrockArm_setRelativePosition_hook, SYM(ramrockArm_setRelativePosition), b_+86);
  SET_HL(POP(b_+86));
  CALL_C(b_+87, objectCopyPosition_hook, SYM(objectCopyPosition), b_+90);
  CYC(b_+90, b_+92); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+94); A = alu_dec8(gb, A);
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  if (F & FZ) {
    CYCT(b_+95, b_+97);
    goto nextPuff;
  }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); A = 0x02;
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  TAIL(ramrockArm_deleteSelf);
}

void ramrockArm_subid4_hook(GB *gb) {
  BASE(ramrockArm_subid4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ramrockArm_jump_table(gb));
    if (jt_ == SYM(ramrockArm_subid4_substate0) && hook_is(gb, SYM(ramrockArm_subid4_substate0), ramrockArm_subid4_substate0_hook)) { ramrockArm_subid4_substate0_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid4_substate1) && hook_is(gb, SYM(ramrockArm_subid4_substate1), ramrockArm_subid4_substate1_hook)) { ramrockArm_subid4_substate1_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid4_substate2) && hook_is(gb, SYM(ramrockArm_subid4_substate2), ramrockArm_subid4_substate2_hook)) { ramrockArm_subid4_substate2_hook(gb); return; }
    else if (jt_ == SYM(ramrockArm_subid4_substate3) && hook_is(gb, SYM(ramrockArm_subid4_substate3), ramrockArm_subid4_substate3_hook)) { ramrockArm_subid4_substate3_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void ramrockArm_subid4_substate0_hook(GB *gb) {
  BASE(ramrockArm_subid4_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x06;
  CALL_C(b_+8, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+11);
  CYC(b_+11, b_+14); SET_BC(hOamFunc);
  CALL_C(b_+14, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+17);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x28);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x3e);
  CYC(b_+25, b_+28);
  TAIL(ecom_incSubstate_b10);
}

void ramrockArm_subid4_substate1_hook(GB *gb) {
  BASE(ramrockArm_subid4_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xf9);
  CYC(b_+5, b_+7); C = 0x00;
  if (!(F & FZ)) {
    CYCT(b_+7, b_+10);
    TAIL(objectUpdateSpeedZ_paramC);
  }
  CYC(b_+7, b_+10);
  CALL_C(b_+10, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+13);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+16);
    TAIL(objectApplySpeed);
  }
  CYC(b_+13, b_+16);
  CALL_C(b_+16, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); A = 0x02;
  CALL_C(b_+26, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x0c);
  CYC(b_+31, b_+33); A = 0x84;
  CYC(b_+33, b_+36);
  TAIL(loadPaletteHeader);
}

void ramrockArm_subid4_updateXPosition_hook(GB *gb) {
  BASE(ramrockArm_subid4_updateXPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+4, b_+6);
    goto related;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+8); alu_cpl(gb);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+10); B = A;
related:
  CYC(b_+10, b_+12); A = 0x0d;
  CALL_C(b_+12, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+18); E = L;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  RET(b_+19); return;
}

void ramrockArm_subid4_collisionOccurred_hook(GB *gb) {
  BASE(ramrockArm_subid4_collisionOccurred);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0d);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x10);
  CYC(b_+11, b_+14);
  TAIL(ecom_incSubstate_b10);
}

void ramrockArm_subid4_substate2_hook(GB *gb) {
  BASE(ramrockArm_subid4_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto updateXPosition; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_rlca(gb);
  if (F & FC) { CYCT(b_+13, b_+15); TAIL(ramrockArm_subid4_collisionOccurred); }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = OBJ_SUBID;
  CALL_C(b_+17, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(b_+23, b_+25); TAIL(ramrockArm_subid4_collisionOccurred); }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto updateXPosition; }
  CYC(b_+27, b_+29);
  CALL_C(b_+29, objectCreatePuff_hook, SYM(objectCreatePuff), b_+32);
  CYC(b_+32, b_+34); TAIL(ramrockArm_deleteSelf);

updateXPosition:
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); B = A;
  CYC(b_+38, b_+40); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+40, b_+42); TAIL(ramrockArm_subid4_updateXPosition); }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = alu_inc8(gb, A);
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+45); B = A;
  TAIL(ramrockArm_subid4_updateXPosition);
}

void ramrockArm_subid4_substate3_hook(GB *gb) {
  BASE(ramrockArm_subid4_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+4, b_+6);
    goto afterCollision;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x36;
  CALL_C(b_+8, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10);
afterCollision:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_TEXT_ID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_sub(gb, 0x02);
  CYC(b_+18, b_+20); alu_cp(gb, 0x04);
  if (!(F & FC)) {
    CYCT(b_+20, b_+22);
    goto atLimit;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); B = 0x04;
  CYC(b_+24, b_+26);
  goto checkSubid;
atLimit:
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30);
  TAIL(ramrockArm_subid4_updateXPosition);
checkSubid:
  CYC(b_+30, b_+32); A = 0x02;
  CALL_C(b_+32, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+38); alu_cp(gb, 0x0d);
  if (F & FZ) {
    CYCT(b_+38, b_+40);
    TAIL(ramrockArm_subid4_updateXPosition);
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+42, b_+44); A = 0x02;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+47);
  TAIL(ramrockArm_subid4_updateXPosition);
}

void ramrockArm_getRelativePosition_hook(GB *gb) {
  BASE(ramrockArm_getRelativePosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); C = 0x0e;
  CYC(b_+5, b_+6); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
    goto related;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); C = 0xf2;
related:
  CYC(b_+10, b_+12); A = 0x0b;
  CALL_C(b_+12, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+18); alu_add(gb, 0x08);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); alu_add(gb, C);
  CYC(b_+22, b_+23); C = A;
  RET(b_+23); return;
}
