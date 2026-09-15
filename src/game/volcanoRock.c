#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t volcanoRock_jump_table(GB *gb) {
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

static void volcanoRock_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode11_hook(GB *gb);
void volcanoRock_subid0_hook(GB *gb);
void volcanoRock_subid1_hook(GB *gb);
void volcanoRock_common_substate2_hook(GB *gb);
void volcanoRock_common_substate3_hook(GB *gb);
void volcanoRock_common_substate4_hook(GB *gb);
void volcanoRock_common_substate5_hook(GB *gb);
void volcanoRock_subid2_hook(GB *gb);
void volcanoRock_setRandomPosition_hook(GB *gb);
void volcanoRock_subid0_setSpeedFromAngle_hook(GB *gb);
void volcanoRock_setCollisionSize_hook(GB *gb);

void partCode11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ae7, 0x4ae9); E = 0xc2; // Part.subid
  CYC(0x4ae9, 0x4aea); A = mem_rd(gb, DE);
  CYC(0x4aea, 0x4aec); E = 0xc4; // Part.state
  {
    CYC(0x4aec, 0x4aed); push_effect(gb, 0x4aed);
    uint16_t target = volcanoRock_jump_table(gb);
    if (target == 0x4af3) { volcanoRock_subid0_hook(gb); return; }
    if (target == 0x4b2e) { volcanoRock_subid1_hook(gb); return; }
    volcanoRock_subid2_hook(gb); return;
  }
}

void volcanoRock_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4af3, 0x4af4); A = mem_rd(gb, DE);
  CYC(0x4af4, 0x4af5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4af5, 0x4af7); goto state0; } // jr z
  CYC(0x4af5, 0x4af7);

  CYC(0x4af7, 0x4af9); C = 0x16;
  CALL_C(0x4af9, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4afc);
  if (F & FC) { CYCT(0x4afc, 0x4aff); partDelete_hook(gb); return; } // jp c
  CYC(0x4afc, 0x4aff);
  if (!(F & FZ)) { CYCT(0x4aff, 0x4b02); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x4aff, 0x4b02);
  CALL_C(0x4b02, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4b05);
  CYC(0x4b05, 0x4b07); alu_and(gb, 0x03);
  CYC(0x4b07, 0x4b08); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x4b08); return; } // ret z
  CYC(0x4b08, 0x4b09);
  CYC(0x4b09, 0x4b0a); B = A;
  CYC(0x4b0a, 0x4b0c); E = 0xc9; // Part.angle
  CYC(0x4b0c, 0x4b0d); A = mem_rd(gb, DE);
  CYC(0x4b0d, 0x4b0e); alu_add(gb, B);
  CYC(0x4b0e, 0x4b10); alu_and(gb, 0x1f);

setAngleAndSpeed:
  CYC(0x4b10, 0x4b11); mem_wr(gb, DE, A);
  CYC(0x4b11, 0x4b14); volcanoRock_subid0_setSpeedFromAngle_hook(gb); return; // jp

state0:
  CYC(0x4b14, 0x4b17); SET_BC(0xfd80); // -0x280
  CALL_C(0x4b17, objectSetSpeedZ_hook, 0x239d, 0x4b1a);
  CYC(0x4b1a, 0x4b1b); L = E; // Part.state
  CYC(0x4b1b, 0x4b1c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b1c, 0x4b1e); L = 0xe4; // Part.collisionType
  CYC(0x4b1e, 0x4b20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(0x4b20, objectSetVisible80_hook, 0x1e57, 0x4b23);
  CALL_C(0x4b23, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4b26);
  CYC(0x4b26, 0x4b28); alu_and(gb, 0x0f);
  CYC(0x4b28, 0x4b2a); alu_add(gb, 0x08);
  CYC(0x4b2a, 0x4b2c); E = 0xc9; // Part.angle
  CYC(0x4b2c, 0x4b2e); goto setAngleAndSpeed; // jr
}

void volcanoRock_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b2e, 0x4b2f); A = mem_rd(gb, DE);
  {
    CYC(0x4b2f, 0x4b30); push_effect(gb, 0x4b30);
    uint16_t target = volcanoRock_jump_table(gb);
    if (target == 0x4b3c) goto substate0;
    if (target == 0x4b66) goto substate1;
    if (target == 0x4b82) { volcanoRock_common_substate2_hook(gb); return; }
    if (target == 0x4b8d) { volcanoRock_common_substate3_hook(gb); return; }
    if (target == 0x4ba8) { volcanoRock_common_substate4_hook(gb); return; }
    volcanoRock_common_substate5_hook(gb); return;
  }

substate0:
  CYC(0x4b3c, 0x4b3d); H = D;
  CYC(0x4b3d, 0x4b3e); L = E; // Part.state
  CYC(0x4b3e, 0x4b3f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b3f, 0x4b41); L = 0xe4; // Part.collisionType
  CYC(0x4b41, 0x4b43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4b43, 0x4b45); L = 0xe6; // Part.collisionRadiusY
  CYC(0x4b45, 0x4b46); A = mem_rd(gb, HL);
  CYC(0x4b46, 0x4b47); alu_add(gb, A);
  CYC(0x4b47, 0x4b48); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b48, 0x4b49); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b49, 0x4b4b); mem_wr(gb, HL, alu_sla(gb, mem_rd(gb, HL)));
  CYC(0x4b4b, 0x4b4d); L = 0xd0; // Part.speed
  CYC(0x4b4d, 0x4b4f); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x4b4f, 0x4b51); L = 0xd4; // Part.speedZ
  CYC(0x4b51, 0x4b53); A = 0x00;
  CYC(0x4b53, 0x4b54); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b54, 0x4b56); mem_wr(gb, HL, 0xfc);
  CALL_C(0x4b56, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4b59);
  CYC(0x4b59, 0x4b5b); alu_and(gb, 0x1f);
  CYC(0x4b5b, 0x4b5d); E = 0xc9; // Part.angle
  CYC(0x4b5d, 0x4b5e); mem_wr(gb, DE, A);
  CYC(0x4b5e, 0x4b60); A = 0x01;
  CALL_C(0x4b60, partSetAnimation_hook, 0x2988, 0x4b63);
  CYC(0x4b63, 0x4b66); objectSetVisible80_hook(gb); return; // jp

substate1:
  CYC(0x4b66, 0x4b67); H = D;
  CYC(0x4b67, 0x4b69); L = 0xcb; // Part.yh
  CYC(0x4b69, 0x4b6b); E = 0xcf; // Part.zh
  CYC(0x4b6b, 0x4b6c); A = mem_rd(gb, DE);
  CYC(0x4b6c, 0x4b6d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4b6d, 0x4b6f); alu_add(gb, 0x08);
  CYC(0x4b6f, 0x4b71); alu_cp(gb, 0xf8);
  CYC(0x4b71, 0x4b73); C = 0x10;
  if (F & FC) { CYCT(0x4b73, 0x4b76); objectUpdateSpeedZ_paramC_hook(gb); return; } // jp c
  CYC(0x4b73, 0x4b76);
  CYC(0x4b76, 0x4b78); L = 0xc4; // Part.state
  CYC(0x4b78, 0x4b79); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b79, 0x4b7b); L = 0xc6; // Part.counter1
  CYC(0x4b7b, 0x4b7d); mem_wr(gb, HL, 0x1e);
  CALL_C(0x4b7d, objectSetInvisible_hook, 0x1e7b, 0x4b80);
  CYC(0x4b80, 0x4b82); volcanoRock_setRandomPosition_hook(gb); return; // jr
}

void volcanoRock_common_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4b82, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x4b85);
  if (!(F & FZ)) { RET_TAKEN(0x4b85); return; } // ret nz
  CYC(0x4b85, 0x4b86);
  CYC(0x4b86, 0x4b88); mem_wr(gb, HL, 0x10); // Part.counter1
  CYC(0x4b88, 0x4b89); L = E; // Part.substate
  CYC(0x4b89, 0x4b8a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b8a, 0x4b8d); objectSetVisiblec0_hook(gb); return; // jp
}

void volcanoRock_common_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4b8d, partAnimate_hook, 0x2978, 0x4b90);
  CYC(0x4b90, 0x4b91); H = D;
  CYC(0x4b91, 0x4b93); L = 0xcf; // Part.zh
  CYC(0x4b93, 0x4b94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4b94, 0x4b95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x4b95); return; } // ret nz
  CYC(0x4b95, 0x4b96);
  CALL_C(0x4b96, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x4b99);
  if (F & FC) { CYCT(0x4b99, 0x4b9c); partDelete_hook(gb); return; } // jp c
  CYC(0x4b99, 0x4b9c);
  CYC(0x4b9c, 0x4b9d); H = D;
  CYC(0x4b9d, 0x4b9f); L = 0xc4; // Part.state
  CYC(0x4b9f, 0x4ba0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4ba0, 0x4ba2); L = 0xd4; // Part.speedZ
  CYC(0x4ba2, 0x4ba3); alu_xor(gb, A);
  CYC(0x4ba3, 0x4ba4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ba4, 0x4ba5); mem_wr(gb, HL, A);
  CYC(0x4ba5, 0x4ba8); objectSetVisible82_hook(gb); return; // jp
}

void volcanoRock_common_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4ba8, partAnimate_hook, 0x2978, 0x4bab);
  CYC(0x4bab, 0x4bad); C = 0x16;
  CALL_C(0x4bad, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4bb0);
  if (!(F & FZ)) { CYCT(0x4bb0, 0x4bb3); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x4bb0, 0x4bb3);
  CYC(0x4bb3, 0x4bb5); L = 0xc4; // Part.state
  CYC(0x4bb5, 0x4bb6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4bb6, 0x4bb8); L = 0xdd; // Part.oamTileIndexBase
  CYC(0x4bb8, 0x4bba); mem_wr(gb, HL, 0x26);
  CYC(0x4bba, 0x4bbc); A = 0x03;
  CALL_C(0x4bbc, partSetAnimation_hook, 0x2988, 0x4bbf);
  CYC(0x4bbf, 0x4bc1); A = 0x81; // SND_STRONG_POUND
  CYC(0x4bc1, 0x4bc4); playSound_b00_hook(gb); return; // jp
}

void volcanoRock_common_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4bc4, 0x4bc6); E = 0xe1; // Part.animParameter
  CYC(0x4bc6, 0x4bc7); A = mem_rd(gb, DE);
  CYC(0x4bc7, 0x4bc8); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x4bc8, 0x4bcb); partDelete_hook(gb); return; } // jp z
  CYC(0x4bc8, 0x4bcb);
  CALL_C(0x4bcb, volcanoRock_setCollisionSize_hook, 0x4c1c, 0x4bce);
  CYC(0x4bce, 0x4bd1); partAnimate_hook(gb); return; // jp
}

void volcanoRock_subid2_hook(GB *gb) {
  CYC(0x4bd1, 0x4bd2); A = mem_rd(gb, DE);
  {
    CYC(0x4bd2, 0x4bd3); push_effect(gb, 0x4bd3);
    uint16_t target = volcanoRock_jump_table(gb);
    if (target == 0x4bdd) goto substate0;
    if (target == 0x4b82) { volcanoRock_common_substate2_hook(gb); return; }
    if (target == 0x4b8d) { volcanoRock_common_substate3_hook(gb); return; }
    if (target == 0x4ba8) { volcanoRock_common_substate4_hook(gb); return; }
    volcanoRock_common_substate5_hook(gb); return;
  }

substate0:
  CYC(0x4bdd, 0x4bdf); A = 0x01;
  CYC(0x4bdf, 0x4be0); mem_wr(gb, DE, A);
  volcanoRock_setRandomPosition_hook(gb); // falls through
}

void volcanoRock_setRandomPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4be0, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4be3);
  CYC(0x4be3, 0x4be4); B = A;
  CYC(0x4be4, 0x4be7); SET_HL(0xffaa); // hCameraY
  CYC(0x4be7, 0x4be9); E = 0xcb; // Part.yh
  CYC(0x4be9, 0x4beb); alu_and(gb, 0x70);
  CYC(0x4beb, 0x4bed); alu_add(gb, 0x08);
  CYC(0x4bed, 0x4bee); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4bee, 0x4bef); mem_wr(gb, DE, A);
  CYC(0x4bef, 0x4bf0); alu_cpl(gb);
  CYC(0x4bf0, 0x4bf1); A = alu_inc8(gb, A);
  CYC(0x4bf1, 0x4bf3); alu_and(gb, 0xfe);
  CYC(0x4bf3, 0x4bf5); E = 0xcf; // Part.zh
  CYC(0x4bf5, 0x4bf6); mem_wr(gb, DE, A);
  CYC(0x4bf6, 0x4bf8); L = 0xac; // <hCameraX
  CYC(0x4bf8, 0x4bfa); E = 0xcd; // Part.xh
  CYC(0x4bfa, 0x4bfb); A = B;
  CYC(0x4bfb, 0x4bfd); alu_and(gb, 0x07);
  CYC(0x4bfd, 0x4bfe); A = alu_inc8(gb, A);
  CYC(0x4bfe, 0x4c00); A = alu_swap(gb, A);
  CYC(0x4c00, 0x4c02); alu_add(gb, 0x08);
  CYC(0x4c02, 0x4c03); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4c03, 0x4c04); mem_wr(gb, DE, A);
  CYC(0x4c04, 0x4c06); A = 0x02;
  CYC(0x4c06, 0x4c09); partSetAnimation_hook(gb); return; // jp
}

void volcanoRock_subid0_setSpeedFromAngle_hook(GB *gb) {
  CYC(0x4c09, 0x4c0b); B = 0x14; // SPEED_80
  CYC(0x4c0b, 0x4c0d); alu_cp(gb, 0x0d);
  if (F & FC) { CYCT(0x4c0d, 0x4c0f); goto setSpeed; } // jr c
  CYC(0x4c0d, 0x4c0f);
  CYC(0x4c0f, 0x4c11); B = 0x0a; // SPEED_40
  CYC(0x4c11, 0x4c13); alu_cp(gb, 0x14);
  if (F & FC) { CYCT(0x4c13, 0x4c15); goto setSpeed; } // jr c
  CYC(0x4c13, 0x4c15);
  CYC(0x4c15, 0x4c17); B = 0x14; // SPEED_80
setSpeed:
  CYC(0x4c17, 0x4c18); A = B;
  CYC(0x4c18, 0x4c1a); E = 0xd0; // Part.speed
  CYC(0x4c1a, 0x4c1b); mem_wr(gb, DE, A);
  RET(0x4c1b); return; // ret
}

void volcanoRock_setCollisionSize_hook(GB *gb) {
  CYC(0x4c1c, 0x4c1d); A = alu_dec8(gb, A);
  CYC(0x4c1d, 0x4c20); SET_HL(0x4c29); // @data
  CYC(0x4c20, 0x4c21); volcanoRock_addAToHl_from_rst(gb, 0x4c21);
  CYC(0x4c21, 0x4c23); E = 0xe6; // Part.collisionRadiusY
  CYC(0x4c23, 0x4c24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4c24, 0x4c25); mem_wr(gb, DE, A);
  CYC(0x4c25, 0x4c26); E = alu_inc8(gb, E);
  CYC(0x4c26, 0x4c27); A = mem_rd(gb, HL);
  CYC(0x4c27, 0x4c28); mem_wr(gb, DE, A);
  RET(0x4c28); return; // ret
}
