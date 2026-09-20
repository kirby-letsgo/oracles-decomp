#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x669e, 0x66a0); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x66a0, 0x66a1); A = mem_rd(gb, DE);
  {
    CYC(0x66a1, 0x66a2); push_effect(gb, 0x66a2);
    uint16_t target = dragonfly_jump_table(gb);
    if (target == 0x66ae) { dragonfly_state0_hook(gb); return; }
    if (target == 0x66bf) { dragonfly_state1_hook(gb); return; }
    if (target == 0x6705) { dragonfly_state2_hook(gb); return; }
    if (target == 0x6719) { dragonfly_state3_hook(gb); return; }
    if (target == 0x6740) { dragonfly_state4_hook(gb); return; }
    if (target == 0x675b) { dragonfly_state5_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:66ae, bare global; jump-table target from enemyCode53. Initialization.
void dragonfly_state0_hook(GB *gb) {
  CYC(0x66ae, 0x66af); H = D;
  CYC(0x66af, 0x66b0); L = E;
  CYC(0x66b0, 0x66b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x66b1, 0x66b3); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x66b3, 0x66b4); A = mem_rd(gb, HL);
  CYC(0x66b4, 0x66b6); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x66b6, 0x66b7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x66b7, 0x66b8); mem_wr(gb, HL, A);
  CYC(0x66b8, 0x66ba); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(0x66ba, 0x66bc); mem_wr(gb, HL, 0xf8); // -$08
  CYC(0x66bc, 0x66bf); objectSetVisiblec1_hook(gb); return; // jp
}

// 0e:66bf, bare global; jump-table target from enemyCode53. Choosing new direction to move in.
void dragonfly_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x66bf, 0x66c0); H = D;
  CYC(0x66c0, 0x66c1); L = E;
  CYC(0x66c1, 0x66c2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x66c2, 0x66c4); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x66c4, 0x66c6); mem_wr(gb, HL, 0x03);
  CYC(0x66c6, 0x66c8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x66c8, 0x66ca); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x66ca, getRandomNumber_noPreserveVars_hook, 0x0453, 0x66cd);
  CYC(0x66cd, 0x66cf); alu_and(gb, 0x06);
  CYC(0x66cf, 0x66d0); C = A;
  CYC(0x66d0, 0x66d2); B = 0x00;
  CYC(0x66d2, 0x66d4); E = ENEMY_BASE + OBJ_YH;
  CYC(0x66d4, 0x66d5); A = mem_rd(gb, DE);
  CYC(0x66d5, 0x66d7); alu_cp(gb, 0x40); // (SMALL_ROOM_HEIGHT/2)<<4
  if (F & FC) { CYCT(0x66d7, 0x66d9); goto checkX; } // jr c
  CYC(0x66d7, 0x66d9);
  CYC(0x66d9, 0x66da); B = alu_inc8(gb, B);

checkX:
  CYC(0x66da, 0x66dc); E = ENEMY_BASE + OBJ_XH;
  CYC(0x66dc, 0x66dd); A = mem_rd(gb, DE);
  CYC(0x66dd, 0x66df); alu_cp(gb, 0x50); // (SMALL_ROOM_WIDTH/2)<<4
  if (F & FC) { CYCT(0x66df, 0x66e1); goto haveQuadrant; } // jr c
  CYC(0x66df, 0x66e1);
  CYC(0x66e1, 0x66e3); B = (uint8_t)(B | (1 << 1)); // set 1,b

haveQuadrant:
  CYC(0x66e3, 0x66e4); A = B;
  CYC(0x66e4, 0x66e7); SET_HL(0x6701); // @angleVals
  CYC(0x66e7, 0x66e8); dragonfly_addAToHl_from_rst(gb, 0x66e8);
  CYC(0x66e8, 0x66e9); A = mem_rd(gb, HL);
  CYC(0x66e9, 0x66ea); alu_add(gb, C);
  CYC(0x66ea, 0x66ec); alu_and(gb, 0x1f);
  CYC(0x66ec, 0x66ee); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x66ee, 0x66ef); mem_wr(gb, DE, A);
  CYC(0x66ef, 0x66f1); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x66f1, 0x66f2); A = mem_rd(gb, DE);
  CYC(0x66f2, 0x66f3); B = A;
  CYC(0x66f3, 0x66f5); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x66f5); return; } // ret z
  CYC(0x66f5, 0x66f6);
  CYC(0x66f6, 0x66f7); A = B;
  CYC(0x66f7, 0x66f9); alu_cp(gb, 0x10);
  CYC(0x66f9, 0x66fb); A = 0x01;
  if (F & FC) { CYCT(0x66fb, 0x66fd); goto setAnimation; } // jr c
  CYC(0x66fb, 0x66fd);
  CYC(0x66fd, 0x66fe); A = alu_dec8(gb, A);

setAnimation:
  CYC(0x66fe, 0x6701); enemySetAnimation_hook(gb); return; // jp
}

// 0e:6705, bare global; jump-table target from enemyCode53. Move in given direction for 3
// frames at SPEED_200. Falls through into dragonfly_animate.
void dragonfly_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6705, dragonfly_applySpeed_hook, 0x6765, 0x6708);
  if (!(F & FZ)) { CYCT(0x6708, 0x670a); goto nextState; } // jr nz
  CYC(0x6708, 0x670a);
  CALL_C(0x670a, ecom_decCounter1_b0e_hook, 0x439a, 0x670d);
  if (!(F & FZ)) { CYCT(0x670d, 0x670f); dragonfly_animate_hook(gb); return; } // jr nz
  CYC(0x670d, 0x670f);

nextState:
  CALL_C(0x670f, ecom_incState_b0e_hook, 0x4000, 0x6712);
  CYC(0x6712, 0x6714); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6714, 0x6716); mem_wr(gb, HL, 0x0c);
  dragonfly_animate_hook(gb); return; // fallthrough
}

// 0e:6716, bare global; falls into from dragonfly_state2, also reached by genuine jp/jr from
// dragonfly_state3/dragonfly_state4/dragonfly_state5.
void dragonfly_animate_hook(GB *gb) {
  CYC(0x6716, 0x6719); enemyAnimate_hook(gb); return; // jp
}

// 0e:6719, bare global; jump-table target from enemyCode53. Slowing down over 12 frames,
// eventually reaching SPEED_140.
void dragonfly_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6719, dragonfly_applySpeed_hook, 0x6765, 0x671c);
  if (!(F & FZ)) { CYCT(0x671c, 0x671e); goto nextState; } // jr nz
  CYC(0x671c, 0x671e);
  CALL_C(0x671e, ecom_decCounter1_b0e_hook, 0x439a, 0x6721);
  if (F & FZ) { CYCT(0x6721, 0x6723); goto nextState; } // jr z
  CYC(0x6721, 0x6723);
  CYC(0x6723, 0x6724); A = mem_rd(gb, HL); // [counter1]
  CYC(0x6724, 0x6725); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x6725, 0x6727); dragonfly_animate_hook(gb); return; } // jr nc
  CYC(0x6725, 0x6727);
  CYC(0x6727, 0x6729); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6729, 0x672a); A = mem_rd(gb, HL);
  CYC(0x672a, 0x672c); alu_sub(gb, 0x05); // SPEED_20
  CYC(0x672c, 0x672d); mem_wr(gb, HL, A);
  CYC(0x672d, 0x672f); dragonfly_animate_hook(gb); return; // jr

nextState:
  CYC(0x672f, 0x6731); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6731, 0x6733); A = 0x04;
  CYC(0x6733, 0x6734); mem_wr(gb, DE, A);
  CALL_C(0x6734, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6737);
  CYC(0x6737, 0x6739); alu_and(gb, 0x07);
  CYC(0x6739, 0x673b); alu_add(gb, 0x18);
  CYC(0x673b, 0x673d); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x673d, 0x673e); mem_wr(gb, DE, A);
  CYC(0x673e, 0x6740); dragonfly_animate_hook(gb); return; // jr
}

// 0e:6740, bare global; jump-table target from enemyCode53. Moving at SPEED_140 for between
// 24-31 frames.
void dragonfly_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6740, dragonfly_applySpeed_hook, 0x6765, 0x6743);
  if (!(F & FZ)) { CYCT(0x6743, 0x6745); goto nextState; } // jr nz
  CYC(0x6743, 0x6745);
  CALL_C(0x6745, ecom_decCounter1_b0e_hook, 0x439a, 0x6748);
  if (!(F & FZ)) { CYCT(0x6748, 0x674a); dragonfly_animate_hook(gb); return; } // jr nz
  CYC(0x6748, 0x674a);

nextState:
  CALL_C(0x674a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x674d);
  CYC(0x674d, 0x674f); alu_and(gb, 0x7f);
  CYC(0x674f, 0x6751); alu_add(gb, 0x20);
  CYC(0x6751, 0x6753); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6753, 0x6754); mem_wr(gb, DE, A);
  CYC(0x6754, 0x6756); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6756, 0x6758); A = 0x05;
  CYC(0x6758, 0x6759); mem_wr(gb, DE, A);
  CYC(0x6759, 0x675b); dragonfly_animate_hook(gb); return; // jr
}

// 0e:675b, bare global; jump-table target from enemyCode53. Holding still for [counter1]
// frames.
void dragonfly_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x675b, ecom_decCounter1_b0e_hook, 0x439a, 0x675e);
  if (!(F & FZ)) { CYCT(0x675e, 0x6760); dragonfly_animate_hook(gb); return; } // jr nz
  CYC(0x675e, 0x6760);
  CYC(0x6760, 0x6761); L = E;
  CYC(0x6761, 0x6763); mem_wr(gb, HL, 0x01); // [state]
  CYC(0x6763, 0x6765); dragonfly_animate_hook(gb); return; // jr
}

// 0e:6765, bare global; called from dragonfly_state2/dragonfly_state3/dragonfly_state4.
// @return zflag nz if touched a wall
void dragonfly_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6765, 0x6767); A = 0x02; // Only screen boundaries count as walls
  CALL_C(0x6767, ecom_getSideviewAdjacentWallsBitset_b0e_hook, 0x420b, 0x676a);
  if (!(F & FZ)) { RET_TAKEN(0x676a); return; } // ret nz
  CYC(0x676a, 0x676b);
  CALL_C(0x676b, objectApplySpeed_hook, 0x201d, 0x676e);
  CYC(0x676e, 0x676f); alu_xor(gb, A);
  RET(0x676f); return; // ret
}
