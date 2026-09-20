#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void dragonfly_state0_hook(GB *gb);
void dragonfly_state1_hook(GB *gb);
void dragonfly_state2_hook(GB *gb);
void dragonfly_animate_hook(GB *gb);
void dragonfly_state3_hook(GB *gb);
void dragonfly_state4_hook(GB *gb);
void dragonfly_state5_hook(GB *gb);
void dragonfly_applySpeed_hook(GB *gb);

static uint16_t dragonfly_jump_table(GB *gb) {
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

static void dragonfly_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_DRAGONFLY
// ==================================================================================================
void enemyCode53_hook(GB *gb) {
  BASE(enemyCode53);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = dragonfly_jump_table(gb);
    if (target == SYM(dragonfly_state0)) { dragonfly_state0_hook(gb); return; }
    if (target == SYM(dragonfly_state1)) { dragonfly_state1_hook(gb); return; }
    if (target == SYM(dragonfly_state2)) { dragonfly_state2_hook(gb); return; }
    if (target == SYM(dragonfly_state3)) { dragonfly_state3_hook(gb); return; }
    if (target == SYM(dragonfly_state4)) { dragonfly_state4_hook(gb); return; }
    if (target == SYM(dragonfly_state5)) { dragonfly_state5_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:66ae, bare global; jump-table target from enemyCode53. Initialization.
void dragonfly_state0_hook(GB *gb) {
  BASE(dragonfly_state0);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0xf8); // -$08
  CYC(b_+14, b_+17); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:66bf, bare global; jump-table target from enemyCode53. Choosing new direction to move in.
void dragonfly_state1_hook(GB *gb) {
  BASE(dragonfly_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x03);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+11, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x06);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+19); B = 0x00;
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_cp(gb, 0x40); // (SMALL_ROOM_HEIGHT/2)<<4
  if (F & FC) { CYCT(b_+24, b_+26); goto checkX; } // jr c
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); B = alu_inc8(gb, B);

checkX:
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+32); alu_cp(gb, 0x50); // (SMALL_ROOM_WIDTH/2)<<4
  if (F & FC) { CYCT(b_+32, b_+34); goto haveQuadrant; } // jr c
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); B = (uint8_t)(B | (1 << 1)); // set 1,b

haveQuadrant:
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+40); SET_HL(b_+66); // @angleVals
  CYC(b_+40, b_+41); dragonfly_addAToHl_from_rst(gb, b_+41);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+43); alu_add(gb, C);
  CYC(b_+43, b_+45); alu_and(gb, 0x1f);
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); B = A;
  CYC(b_+52, b_+54); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+54); return; } // ret z
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); A = B;
  CYC(b_+56, b_+58); alu_cp(gb, 0x10);
  CYC(b_+58, b_+60); A = 0x01;
  if (F & FC) { CYCT(b_+60, b_+62); goto setAnimation; } // jr c
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); A = alu_dec8(gb, A);

setAnimation:
  CYC(b_+63, b_+66); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6705, bare global; jump-table target from enemyCode53. Move in given direction for 3
// frames at SPEED_200. Falls through into dragonfly_animate.
void dragonfly_state2_hook(GB *gb) {
  BASE(dragonfly_state2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dragonfly_applySpeed_hook, SYM(dragonfly_applySpeed), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto nextState; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); dragonfly_animate_hook(gb); return; } // jr nz
  CYC(b_+8, b_+10);

nextState:
  CALL_C(b_+10, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x0c);
  dragonfly_animate_hook(gb); return; // fallthrough
}

// 0e:6716, bare global; falls into from dragonfly_state2, also reached by genuine jp/jr from
// dragonfly_state3/dragonfly_state4/dragonfly_state5.
void dragonfly_animate_hook(GB *gb) {
  BASE(dragonfly_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// 0e:6719, bare global; jump-table target from enemyCode53. Slowing down over 12 frames,
// eventually reaching SPEED_140.
void dragonfly_state3_hook(GB *gb) {
  BASE(dragonfly_state3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dragonfly_applySpeed_hook, SYM(dragonfly_applySpeed), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto nextState; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto nextState; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+11, b_+12); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+12, b_+14); dragonfly_animate_hook(gb); return; } // jr nc
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_sub(gb, 0x05); // SPEED_20
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); dragonfly_animate_hook(gb); return; // jr

nextState:
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+24, b_+26); A = 0x04;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+30);
  CYC(b_+30, b_+32); alu_and(gb, 0x07);
  CYC(b_+32, b_+34); alu_add(gb, 0x18);
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+39); dragonfly_animate_hook(gb); return; // jr
}

// 0e:6740, bare global; jump-table target from enemyCode53. Moving at SPEED_140 for between
// 24-31 frames.
void dragonfly_state4_hook(GB *gb) {
  BASE(dragonfly_state4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dragonfly_applySpeed_hook, SYM(dragonfly_applySpeed), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto nextState; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); dragonfly_animate_hook(gb); return; } // jr nz
  CYC(b_+8, b_+10);

nextState:
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x7f);
  CYC(b_+15, b_+17); alu_add(gb, 0x20);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+22, b_+24); A = 0x05;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+27); dragonfly_animate_hook(gb); return; // jr
}

// 0e:675b, bare global; jump-table target from enemyCode53. Holding still for [counter1]
// frames.
void dragonfly_state5_hook(GB *gb) {
  BASE(dragonfly_state5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); dragonfly_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x01); // [state]
  CYC(b_+8, b_+10); dragonfly_animate_hook(gb); return; // jr
}

// 0e:6765, bare global; called from dragonfly_state2/dragonfly_state3/dragonfly_state4.
// @return zflag nz if touched a wall
void dragonfly_applySpeed_hook(GB *gb) {
  BASE(dragonfly_applySpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x02; // Only screen boundaries count as walls
  CALL_C(b_+2, ecom_getSideviewAdjacentWallsBitset_b0e_hook, SYM(ecom_getSideviewAdjacentWallsBitset_b0e), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, objectApplySpeed_hook, SYM(objectApplySpeed), b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
  RET(b_+10); return; // ret
}
