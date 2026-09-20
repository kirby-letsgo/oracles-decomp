#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void bladeTrap_subid00_hook(GB *gb);
void bladeTrap_subid01_hook(GB *gb);
void bladeTrap_subid03_hook(GB *gb);
void bladeTrap_subid05_hook(GB *gb);
void bladeTrap_updateAngle_hook(GB *gb);
void bladeTrap_initCircular_hook(GB *gb);
void bladeTrap_checkObstructionsToTarget_hook(GB *gb);
void bladeTrap_checkLinkAligned_hook(GB *gb);
void bladeTrap_incAngle_hook(GB *gb);
void bladeTrap_decAngle_hook(GB *gb);

static uint16_t bladeTrap_jump_table(GB *gb) {
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

static void bladeTrap_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_BLADE_TRAP
// ENEMY_FLAME_TRAP
// ==================================================================================================
void enemyCode0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b9e, 0x4b9f); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x4b9f); return; } // ret z
  CYC(0x4b9f, 0x4ba0);
  CYC(0x4ba0, 0x4ba1); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x4ba1); return; } // ret z
  CYC(0x4ba1, 0x4ba2);
  CALL_C(0x4ba2, enemyAnimate_hook, 0x2818, 0x4ba5);
  CALL_C(0x4ba5, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x4ba8);
  if (!(F & FC)) { CYCT(0x4ba8, 0x4baa); goto normalState; } // jr nc
  CYC(0x4ba8, 0x4baa);
  {
    CYC(0x4baa, 0x4bab); push_effect(gb, 0x4bab);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == 0x4bc9) goto state_uninitialized;
    if (target == 0x4be6) goto state_stub;
    HANDOFF(target);
  }

normalState:
  CYC(0x4bbb, 0x4bbc); A = B;
  {
    CYC(0x4bbc, 0x4bbd); push_effect(gb, 0x4bbd);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == 0x4be7) { bladeTrap_subid00_hook(gb); return; }
    if (target == 0x4c3e) { bladeTrap_subid01_hook(gb); return; }
    if (target == 0x4cbc) { bladeTrap_subid03_hook(gb); return; }
    if (target == 0x4cd7) { bladeTrap_subid05_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x4bc9, 0x4bca); A = B;
  CYC(0x4bca, 0x4bcc); alu_sub(gb, 0x03);
  CYC(0x4bcc, 0x4bce); alu_cp(gb, 0x02);
  if (F & FC) { CALL_C(0x4bce, bladeTrap_initCircular_hook, 0x4d43, 0x4bd1); } else { CYC(0x4bce, 0x4bd1); } // call c
  CYC(0x4bd1, 0x4bd3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4bd3, 0x4bd4); A = mem_rd(gb, DE);
  CYC(0x4bd4, 0x4bd5); alu_or(gb, A);
  CYC(0x4bd5, 0x4bd7); A = 0x08;
  if (!(F & FZ)) { CYCT(0x4bd7, 0x4bd9); goto setVar3e; } // jr nz
  CYC(0x4bd7, 0x4bd9);
  CYC(0x4bd9, 0x4bdb); A = 0x01;
  CALL_C(0x4bdb, enemySetAnimation_hook, 0x282b, 0x4bde);
  CYC(0x4bde, 0x4be0); A = 0x01;

setVar3e:
  CYC(0x4be0, 0x4be2); E = ENEMY_BASE + 0x3e; // Enemy.var3e
  CYC(0x4be2, 0x4be3); mem_wr(gb, DE, A);
  CYC(0x4be3, 0x4be6); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_stub:
  RET(0x4be6); return; // ret
}

// 0d:4be7, bare global; red, spinning trap.
void bladeTrap_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4be7, 0x4be8); A = mem_rd(gb, DE);
  CYC(0x4be8, 0x4bea); alu_sub(gb, 0x08);
  {
    CYC(0x4bea, 0x4beb); push_effect(gb, 0x4beb);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == 0x4bf3) goto state8;
    if (target == 0x4bff) goto state9;
    if (target == 0x4c1c) goto stateA;
    if (target == 0x4c2f) goto stateB;
    HANDOFF(target);
  }

state8:
  CYC(0x4bf3, 0x4bf4); H = D;
  CYC(0x4bf4, 0x4bf5); L = E;
  CYC(0x4bf5, 0x4bf6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4bf6, 0x4bf8); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4bf8, 0x4bfa); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x4bfa, 0x4bfc); A = 0x01;
  CYC(0x4bfc, 0x4bff); enemySetAnimation_hook(gb); return; // jp

state9:
  CYC(0x4bff, 0x4c01); B = 0x0e;
  CALL_C(0x4c01, bladeTrap_checkLinkAligned_hook, 0x4dd8, 0x4c04);
  if (!(F & FC)) { RET_TAKEN(0x4c04); return; } // ret nc
  CYC(0x4c04, 0x4c05);
  CALL_C(0x4c05, bladeTrap_checkObstructionsToTarget_hook, 0x4d75, 0x4c08);
  if (!(F & FZ)) { RET_TAKEN(0x4c08); return; } // ret nz
  CYC(0x4c08, 0x4c09);
  CYC(0x4c09, 0x4c0a); H = D;
  CYC(0x4c0a, 0x4c0c); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4c0c, 0x4c0e); mem_wr(gb, HL, 0x0a);
  CYC(0x4c0e, 0x4c10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4c10, 0x4c12); mem_wr(gb, HL, 0x18);
  CYC(0x4c12, 0x4c14); A = 0x71; // SND_MOVEBLOCK
  CALL_C(0x4c14, playSound_b00_hook, 0x0c98, 0x4c17);
  CYC(0x4c17, 0x4c19); A = 0x02;
  CYC(0x4c19, 0x4c1c); enemySetAnimation_hook(gb); return; // jp

stateA:
  CYC(0x4c1c, 0x4c1e); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4c1e, 0x4c1f); A = mem_rd(gb, DE);
  CYC(0x4c1f, 0x4c20); alu_rrca(gb);
  if (F & FC) { CALL_C(0x4c20, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x4c23); } else { CYC(0x4c20, 0x4c23); } // call c
  CALL_C(0x4c23, ecom_decCounter1_b0d_hook, 0x439a, 0x4c26);
  if (!(F & FZ)) { CYCT(0x4c26, 0x4c28); goto animate; } // jr nz
  CYC(0x4c26, 0x4c28);
  CYC(0x4c28, 0x4c2a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4c2a, 0x4c2c); mem_wr(gb, HL, 0x0b);

animate:
  CYC(0x4c2c, 0x4c2f); enemyAnimate_hook(gb); return; // jp

stateB:
  CALL_C(0x4c2f, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x4c32);
  if (!(F & FZ)) { CYCT(0x4c32, 0x4c34); goto animate; } // jr nz
  CYC(0x4c32, 0x4c34);
  CYC(0x4c34, 0x4c36); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4c36, 0x4c38); A = 0x09;
  CYC(0x4c38, 0x4c39); mem_wr(gb, DE, A);
  CYC(0x4c39, 0x4c3b); A = 0x01;
  CYC(0x4c3b, 0x4c3e); enemySetAnimation_hook(gb); return; // jp
}

// 0d:4c3e, bare global; bladeTrap_subid02 aliases the same address (blue/gold blade traps).
void bladeTrap_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c3e, 0x4c3f); A = mem_rd(gb, DE);
  CYC(0x4c3f, 0x4c41); alu_sub(gb, 0x08);
  {
    CYC(0x4c41, 0x4c42); push_effect(gb, 0x4c42);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == 0x4c4c) goto state8;
    if (target == 0x4c5c) goto state9;
    if (target == 0x4c7a) goto stateA;
    if (target == 0x4ca7) goto stateB;
    if (target == 0x4cb3) goto stateC;
    HANDOFF(target);
  }

state8:
  CYC(0x4c4c, 0x4c4d); H = D;
  CYC(0x4c4d, 0x4c4e); L = E;
  CYC(0x4c4e, 0x4c4f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x4c4f, 0x4c51); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4c51, 0x4c52); A = mem_rd(gb, HL);
  CYC(0x4c52, 0x4c53); A = alu_dec8(gb, A);
  CYC(0x4c53, 0x4c55); A = 0x3c; // SPEED_180
  if (F & FZ) { CYCT(0x4c55, 0x4c57); goto setVar30; } // jr z
  CYC(0x4c55, 0x4c57);
  CYC(0x4c57, 0x4c59); A = 0x78; // SPEED_300

setVar30:
  CYC(0x4c59, 0x4c5b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4c5b, 0x4c5c); mem_wr(gb, HL, A);

state9:
  CYC(0x4c5c, 0x4c5e); B = 0x0d;
  CALL_C(0x4c5e, bladeTrap_checkLinkAligned_hook, 0x4dd8, 0x4c61);
  if (!(F & FC)) { RET_TAKEN(0x4c61); return; } // ret nc
  CYC(0x4c61, 0x4c62);
  CALL_C(0x4c62, bladeTrap_checkObstructionsToTarget_hook, 0x4d75, 0x4c65);
  if (!(F & FZ)) { RET_TAKEN(0x4c65); return; } // ret nz
  CYC(0x4c65, 0x4c66);
  CYC(0x4c66, 0x4c68); A = 0x01;
  CALL_C(0x4c68, ecom_getTopDownAdjacentWallsBitset_b0d_hook, 0x4204, 0x4c6b);
  if (!(F & FZ)) { RET_TAKEN(0x4c6b); return; } // ret nz
  CYC(0x4c6b, 0x4c6c);
  CALL_C(0x4c6c, ecom_incState_b0d_hook, 0x4000, 0x4c6f);
  CYC(0x4c6f, 0x4c71); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4c71, 0x4c73); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4c73, 0x4c74); A = mem_rd(gb, DE);
  CYC(0x4c74, 0x4c75); mem_wr(gb, HL, A);
  CYC(0x4c75, 0x4c77); A = 0x75; // SND_UNKNOWN5
  CYC(0x4c77, 0x4c7a); playSound_b00_hook(gb); return; // jp

stateA:
  CALL_C(0x4c7a, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x4c7d);
  CYC(0x4c7d, 0x4c7e); H = D;
  if (F & FZ) { CYCT(0x4c7e, 0x4c80); goto beginRetracting; } // jr z
  CYC(0x4c7e, 0x4c80);
  CYC(0x4c80, 0x4c82); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4c82, 0x4c84); alu_bit(gb, 3, mem_rd(gb, HL));
  CYC(0x4c84, 0x4c86); B = 0x58; // (LARGE_ROOM_HEIGHT/2 << 4) + 8
  CYC(0x4c86, 0x4c88); L = ENEMY_BASE + OBJ_YH;
  if (F & FZ) { CYCT(0x4c88, 0x4c8a); goto checkOffset; } // jr z
  CYC(0x4c88, 0x4c8a);
  CYC(0x4c8a, 0x4c8c); B = 0x78; // (LARGE_ROOM_WIDTH/2 << 4) + 8
  CYC(0x4c8c, 0x4c8e); L = ENEMY_BASE + OBJ_XH;

checkOffset:
  CYC(0x4c8e, 0x4c8f); A = mem_rd(gb, HL);
  CYC(0x4c8f, 0x4c90); alu_sub(gb, B);
  CYC(0x4c90, 0x4c92); alu_add(gb, 0x07);
  CYC(0x4c92, 0x4c94); alu_cp(gb, 0x0f);
  if (!(F & FC)) { RET_TAKEN(0x4c94); return; } // ret nc
  CYC(0x4c94, 0x4c95);

beginRetracting:
  CYC(0x4c95, 0x4c97); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4c97, 0x4c98); A = mem_rd(gb, HL);
  CYC(0x4c98, 0x4c9a); alu_xor(gb, 0x10);
  CYC(0x4c9a, 0x4c9b); mem_wr(gb, HL, A);
  CYC(0x4c9b, 0x4c9d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4c9d, 0x4c9f); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x4c9f, 0x4ca1); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4ca1, 0x4ca2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4ca2, 0x4ca4); A = 0x50; // SND_CLINK
  CYC(0x4ca4, 0x4ca7); playSound_b00_hook(gb); return; // jp

stateB:
  CALL_C(0x4ca7, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x4caa);
  if (!(F & FZ)) { RET_TAKEN(0x4caa); return; } // ret nz
  CYC(0x4caa, 0x4cab);
  CALL_C(0x4cab, ecom_incState_b0d_hook, 0x4000, 0x4cae);
  CYC(0x4cae, 0x4cb0); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4cb0, 0x4cb2); mem_wr(gb, HL, 0x10);
  RET(0x4cb2); return; // ret

stateC:
  CALL_C(0x4cb3, ecom_decCounter1_b0d_hook, 0x439a, 0x4cb6);
  if (!(F & FZ)) { RET_TAKEN(0x4cb6); return; } // ret nz
  CYC(0x4cb6, 0x4cb7);
  CYC(0x4cb7, 0x4cb9); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4cb9, 0x4cbb); mem_wr(gb, HL, 0x09);
  RET(0x4cbb); return; // ret
}

// 0d:4cbc, bare global; bladeTrap_subid04 aliases the same address (circular blade traps).
void bladeTrap_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4cbc, 0x4cbd); A = mem_rd(gb, DE);
  CYC(0x4cbd, 0x4cbf); alu_sub(gb, 0x08);
  {
    CYC(0x4cbf, 0x4cc0); push_effect(gb, 0x4cc0);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == 0x4cc2) goto state8;
    HANDOFF(target);
  }

state8:
  CYC(0x4cc2, 0x4cc5); A = mem_rd(gb, wFrameCounter);
  CYC(0x4cc5, 0x4cc7); alu_and(gb, 0x01);
  if (F & FZ) { CALL_C(0x4cc7, bladeTrap_updateAngle_hook, 0x4d36, 0x4cca); } else { CYC(0x4cc7, 0x4cca); } // call z
  CYC(0x4cca, 0x4ccb); H = D;
  CYC(0x4ccb, 0x4ccd); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4ccd, 0x4cce); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4cce, 0x4ccf); B = A;
  CYC(0x4ccf, 0x4cd0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4cd0, 0x4cd1); C = A;
  CYC(0x4cd1, 0x4cd2); A = mem_rd(gb, HL);
  CYC(0x4cd2, 0x4cd4); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4cd4, 0x4cd7); objectSetPositionInCircleArc_hook(gb); return; // jp
}

// 0d:4cd7, bare global; unlimited range green blade.
void bladeTrap_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4cd7, 0x4cd8); A = mem_rd(gb, DE);
  CYC(0x4cd8, 0x4cda); alu_sub(gb, 0x08);
  {
    CYC(0x4cda, 0x4cdb); push_effect(gb, 0x4cdb);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == 0x4ce5) goto state8;
    if (target == 0x4cec) goto state9;
    if (target == 0x4d0b) goto stateA;
    if (target == 0x4d21) goto stateB;
    if (target == 0x4d2d) goto stateC;
    HANDOFF(target);
  }

state8:
  CYC(0x4ce5, 0x4ce6); H = D;
  CYC(0x4ce6, 0x4ce7); L = E;
  CYC(0x4ce7, 0x4ce8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x4ce8, 0x4cea); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4cea, 0x4cec); mem_wr(gb, HL, 0x50); // SPEED_200

state9:
  CYC(0x4cec, 0x4cee); B = 0x0e;
  CALL_C(0x4cee, bladeTrap_checkLinkAligned_hook, 0x4dd8, 0x4cf1);
  if (!(F & FC)) { RET_TAKEN(0x4cf1); return; } // ret nc
  CYC(0x4cf1, 0x4cf2);
  CALL_C(0x4cf2, bladeTrap_checkObstructionsToTarget_hook, 0x4d75, 0x4cf5);
  if (!(F & FZ)) { RET_TAKEN(0x4cf5); return; } // ret nz
  CYC(0x4cf5, 0x4cf6);
  CYC(0x4cf6, 0x4cf8); A = 0x01;
  CALL_C(0x4cf8, ecom_getTopDownAdjacentWallsBitset_b0d_hook, 0x4204, 0x4cfb);
  if (!(F & FZ)) { RET_TAKEN(0x4cfb); return; } // ret nz
  CYC(0x4cfb, 0x4cfc);
  CYC(0x4cfc, 0x4cfd); H = D;
  CYC(0x4cfd, 0x4cff); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4cff, 0x4d01); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4d01, 0x4d02); A = mem_rd(gb, DE);
  CYC(0x4d02, 0x4d03); mem_wr(gb, HL, A);
  CYC(0x4d03, 0x4d05); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4d05, 0x4d06); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d06, 0x4d08); A = 0x75; // SND_UNKNOWN5
  CYC(0x4d08, 0x4d0b); playSound_b00_hook(gb); return; // jp

stateA:
  CALL_C(0x4d0b, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x4d0e);
  if (!(F & FZ)) { RET_TAKEN(0x4d0e); return; } // ret nz
  CYC(0x4d0e, 0x4d0f);
  CALL_C(0x4d0f, ecom_incState_b0d_hook, 0x4000, 0x4d12);
  CYC(0x4d12, 0x4d14); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4d14, 0x4d15); A = mem_rd(gb, HL);
  CYC(0x4d15, 0x4d17); alu_xor(gb, 0x10);
  CYC(0x4d17, 0x4d18); mem_wr(gb, HL, A);
  CYC(0x4d18, 0x4d1a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4d1a, 0x4d1c); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x4d1c, 0x4d1e); A = 0x50; // SND_CLINK
  CYC(0x4d1e, 0x4d21); playSound_b00_hook(gb); return; // jp

stateB:
  CALL_C(0x4d21, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x4d24);
  if (!(F & FZ)) { RET_TAKEN(0x4d24); return; } // ret nz
  CYC(0x4d24, 0x4d25);
  CALL_C(0x4d25, ecom_incState_b0d_hook, 0x4000, 0x4d28);
  CYC(0x4d28, 0x4d2a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4d2a, 0x4d2c); mem_wr(gb, HL, 0x10);
  RET(0x4d2c); return; // ret

stateC:
  CALL_C(0x4d2d, ecom_decCounter1_b0d_hook, 0x439a, 0x4d30);
  if (!(F & FZ)) { RET_TAKEN(0x4d30); return; } // ret nz
  CYC(0x4d30, 0x4d31);
  CYC(0x4d31, 0x4d33); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4d33, 0x4d35); mem_wr(gb, HL, 0x09);
  RET(0x4d35); return; // ret
}

// 0d:4d36, bare global.
void bladeTrap_updateAngle_hook(GB *gb) {
  CYC(0x4d36, 0x4d38); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x4d38, 0x4d39); A = mem_rd(gb, DE);
  CYC(0x4d39, 0x4d3b); alu_cp(gb, 0x03);
  CYC(0x4d3b, 0x4d3d); E = ENEMY_BASE + OBJ_ANGLE;
  if (!(F & FZ)) { CYCT(0x4d3d, 0x4d40); bladeTrap_decAngle_hook(gb); return; } // jp nz
  CYC(0x4d3d, 0x4d40);
  CYC(0x4d40, 0x4d43); bladeTrap_incAngle_hook(gb); return; // jp
}

// 0d:4d43, bare global.
void bladeTrap_initCircular_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d43, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4d46);
  CYC(0x4d46, 0x4d48); alu_and(gb, 0x1f);
  CYC(0x4d48, 0x4d4a); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4d4a, 0x4d4b); mem_wr(gb, DE, A);
  CYC(0x4d4b, 0x4d4d); E = ENEMY_BASE + OBJ_YH;
  CYC(0x4d4d, 0x4d4e); A = mem_rd(gb, DE);
  CYC(0x4d4e, 0x4d4f); C = A;
  CYC(0x4d4f, 0x4d51); alu_and(gb, 0xf0);
  CYC(0x4d51, 0x4d53); alu_add(gb, 0x08);
  CYC(0x4d53, 0x4d55); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x4d55, 0x4d56); mem_wr(gb, DE, A);
  CYC(0x4d56, 0x4d57); B = A;
  CYC(0x4d57, 0x4d58); A = C;
  CYC(0x4d58, 0x4d5a); alu_and(gb, 0x0f);
  CYC(0x4d5a, 0x4d5c); A = alu_swap(gb, A);
  CYC(0x4d5c, 0x4d5e); alu_add(gb, 0x08);
  CYC(0x4d5e, 0x4d60); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4d60, 0x4d61); mem_wr(gb, DE, A);
  CYC(0x4d61, 0x4d62); C = A;
  CYC(0x4d62, 0x4d64); E = ENEMY_BASE + OBJ_XH;
  CYC(0x4d64, 0x4d65); A = mem_rd(gb, DE);
  CYC(0x4d65, 0x4d67); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x4d67, 0x4d68); mem_wr(gb, DE, A);
  CYC(0x4d68, 0x4d6a); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4d6a, 0x4d6d); objectSetPositionInCircleArc_hook(gb); return; // jp
}

// 0d:4dbf, called once from bladeTrap_checkObstructionsToTarget.
static void bladeTrap_checkObstructionsToTarget_getNumTilesToTarget(GB *gb) {
  CYC(0x4dbf, 0x4dc0); E = B;
  CYC(0x4dc0, 0x4dc2); A = H8(hEnemyTargetY);
  CYC(0x4dc2, 0x4dc4); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4dc4, 0x4dc6); goto sub; } // jr z
  CYC(0x4dc4, 0x4dc6);
  CYC(0x4dc6, 0x4dc7); E = C;
  CYC(0x4dc7, 0x4dc9); A = H8(hEnemyTargetX);

sub:
  CYC(0x4dc9, 0x4dca); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(0x4dca, 0x4dcc); goto afterAbs; } // jr nc
  CYC(0x4dca, 0x4dcc);
  CYC(0x4dcc, 0x4dcd); alu_cpl(gb);
  CYC(0x4dcd, 0x4dce); A = alu_inc8(gb, A);

afterAbs:
  CYC(0x4dce, 0x4dd0); A = alu_swap(gb, A);
  CYC(0x4dd0, 0x4dd2); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x4dd2, 0x4dd4); goto storeResult; } // jr nz
  CYC(0x4dd2, 0x4dd4);
  CYC(0x4dd4, 0x4dd5); A = alu_inc8(gb, A);

storeResult:
  CYC(0x4dd5, 0x4dd7); hram_wr(gb, 0x8b, A);
  RET(0x4dd7); return; // ret
}

// 0d:4dad, called once from bladeTrap_checkObstructionsToTarget's tile-check loop.
static void bladeTrap_checkObstructionsToTarget_checkNextTileSolid(GB *gb) {
  CYC(0x4dad, 0x4dae); A = B;
  CYC(0x4dae, 0x4daf); alu_add(gb, H);
  CYC(0x4daf, 0x4db0); B = A;
  CYC(0x4db0, 0x4db2); alu_and(gb, 0xf0);
  CYC(0x4db2, 0x4db3); E = A;
  CYC(0x4db3, 0x4db4); A = C;
  CYC(0x4db4, 0x4db5); alu_add(gb, L);
  CYC(0x4db5, 0x4db6); C = A;
  CYC(0x4db6, 0x4db8); alu_and(gb, 0xf0);
  CYC(0x4db8, 0x4dba); A = alu_swap(gb, A);
  CYC(0x4dba, 0x4dbb); alu_or(gb, E);
  CYC(0x4dbb, 0x4dbc); E = A;
  CYC(0x4dbc, 0x4dbd); A = mem_rd(gb, DE);
  CYC(0x4dbd, 0x4dbe); alu_or(gb, A);
  RET(0x4dbe); return; // ret
}

// 0d:4d75, bare global.
void bladeTrap_checkObstructionsToTarget_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d75, 0x4d76); H = D;
  CYC(0x4d76, 0x4d78); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4d78, 0x4d79); B = mem_rd(gb, HL);
  CYC(0x4d79, 0x4d7b); L = ENEMY_BASE + OBJ_XH;
  CYC(0x4d7b, 0x4d7c); C = mem_rd(gb, HL);
  CYC(0x4d7c, 0x4d7e); A = H8(hEnemyTargetX);
  CYC(0x4d7e, 0x4d7f); alu_sub(gb, C);
  CYC(0x4d7f, 0x4d81); alu_add(gb, 0x04);
  CYC(0x4d81, 0x4d83); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x4d83, 0x4d85); goto getDirectionOffset; } // jr nc
  CYC(0x4d83, 0x4d85);
  CYC(0x4d85, 0x4d87); A = H8(hEnemyTargetY);
  CYC(0x4d87, 0x4d88); alu_sub(gb, B);
  CYC(0x4d88, 0x4d8a); alu_add(gb, 0x04);
  CYC(0x4d8a, 0x4d8c); alu_cp(gb, 0x09);
  if (F & FC) { RET_TAKEN(0x4d8c); return; } // ret c
  CYC(0x4d8c, 0x4d8d);

getDirectionOffset:
  CYC(0x4d8d, 0x4d8f); L = ENEMY_BASE + OBJ_ANGLE;
  CALL_C(0x4d8f, bladeTrap_checkObstructionsToTarget_getNumTilesToTarget, 0x4dbf, 0x4d92);
  CYC(0x4d92, 0x4d93); A = mem_rd(gb, HL);
  CYC(0x4d93, 0x4d94); alu_rrca(gb);
  CYC(0x4d94, 0x4d95); alu_rrca(gb);
  CYC(0x4d95, 0x4d98); SET_HL(0x4d6d); // bladeTrap_directionOffsets (data)
  CYC(0x4d98, 0x4d99); bladeTrap_addAToHl_from_rst(gb, 0x4d99);
  CYC(0x4d99, 0x4d9a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4d9a, 0x4d9b); L = mem_rd(gb, HL);
  CYC(0x4d9b, 0x4d9c); H = A;
  PUSH(0x4d9c, DE);
  CYC(0x4d9d, 0x4d9f); D = 0xce; // >wRoomCollisions

checkNextTile:
  CALL_C(0x4d9f, bladeTrap_checkObstructionsToTarget_checkNextTileSolid, 0x4dad, 0x4da2);
  if (!(F & FZ)) { CYCT(0x4da2, 0x4da4); goto doneChecking; } // jr nz
  CYC(0x4da2, 0x4da4);
  CYC(0x4da4, 0x4da6); A = hram_rd(gb, 0x8b);
  CYC(0x4da6, 0x4da7); A = alu_dec8(gb, A);
  CYC(0x4da7, 0x4da9); hram_wr(gb, 0x8b, A);
  if (!(F & FZ)) { CYCT(0x4da9, 0x4dab); goto checkNextTile; } // jr nz
  CYC(0x4da9, 0x4dab);

doneChecking:
  SET_DE(POP(0x4dab));
  RET(0x4dac); return; // ret
}

// 0d:4dd8, bare global.
void bladeTrap_checkLinkAligned_hook(GB *gb) {
  CYC(0x4dd8, 0x4dd9); C = B;
  CYC(0x4dd9, 0x4ddb); C = alu_sla(gb, C);
  CYC(0x4ddb, 0x4ddc); C = alu_inc8(gb, C);
  CYC(0x4ddc, 0x4dde); E = 0x00;
  CYC(0x4dde, 0x4ddf); H = D;
  CYC(0x4ddf, 0x4de1); L = ENEMY_BASE + OBJ_XH;
  CYC(0x4de1, 0x4de3); A = H8(hEnemyTargetX);
  CYC(0x4de3, 0x4de4); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4de4, 0x4de5); alu_add(gb, B);
  CYC(0x4de5, 0x4de6); alu_cp(gb, C);
  CYC(0x4de6, 0x4de8); L = ENEMY_BASE + OBJ_YH;
  CYC(0x4de8, 0x4dea); A = H8(hEnemyTargetY);
  if (F & FC) { CYCT(0x4dea, 0x4dec); goto inRange; } // jr c
  CYC(0x4dea, 0x4dec);
  CYC(0x4dec, 0x4dee); E = 0x18;
  CYC(0x4dee, 0x4def); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4def, 0x4df0); alu_add(gb, B);
  CYC(0x4df0, 0x4df1); alu_cp(gb, C);
  CYC(0x4df1, 0x4df3); L = ENEMY_BASE + OBJ_XH;
  CYC(0x4df3, 0x4df5); A = H8(hEnemyTargetX);
  if (!(F & FC)) { RET_TAKEN(0x4df5); return; } // ret nc
  CYC(0x4df5, 0x4df6);

inRange:
  CYC(0x4df6, 0x4df7); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x4df7, 0x4df8); A = E;
  if (F & FC) { CYCT(0x4df8, 0x4dfa); goto setAngle; } // jr c
  CYC(0x4df8, 0x4dfa);
  CYC(0x4dfa, 0x4dfc); alu_xor(gb, 0x10);

setAngle:
  CYC(0x4dfc, 0x4dfe); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4dfe, 0x4dff); mem_wr(gb, HL, A);
  CYC(0x4dff, 0x4e00); alu_scf(gb);
  RET(0x4e00); return; // ret
}

// 0d:4e01, bare global.
void bladeTrap_incAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4e01, 0x4e02); A = mem_rd(gb, DE);
  CYC(0x4e02, 0x4e03); A = alu_inc8(gb, A);
  CYC(0x4e03, 0x4e07); goto andAndStore; // jr

andAndStore:
  CYC(0x4e07, 0x4e09); alu_and(gb, 0x1f);
  CYC(0x4e09, 0x4e0a); mem_wr(gb, DE, A);
  RET(0x4e0a); return; // ret
}

// 0d:4e05, bare global.
void bladeTrap_decAngle_hook(GB *gb) {
  CYC(0x4e05, 0x4e06); A = mem_rd(gb, DE);
  CYC(0x4e06, 0x4e07); A = alu_dec8(gb, A);
  CYC(0x4e07, 0x4e09); alu_and(gb, 0x1f);
  CYC(0x4e09, 0x4e0a); mem_wr(gb, DE, A);
  RET(0x4e0a); return; // ret
}
