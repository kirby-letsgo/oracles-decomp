#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode11), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode11), (from), (to), true)

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
  BASE(partCode11);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = 0xc4; // Part.state
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = volcanoRock_jump_table(gb);
    if (target == SYM(volcanoRock_subid0)) { volcanoRock_subid0_hook(gb); return; }
    if (target == SYM(volcanoRock_subid1)) { volcanoRock_subid1_hook(gb); return; }
    volcanoRock_subid2_hook(gb); return;
  }
}

void volcanoRock_subid0_hook(GB *gb) {
  BASE(volcanoRock_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+2, b_+4); goto state0; } // jr z
  CYC(b_+2, b_+4);

  CYC(b_+4, b_+6); C = 0x16;
  CALL_C(b_+6, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+9);
  if (F & FC) { CYCT(b_+9, b_+12); partDelete_hook(gb); return; } // jp c
  CYC(b_+9, b_+12);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CALL_C(b_+15, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x03);
  CYC(b_+20, b_+21); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+21); return; } // ret z
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+25); E = 0xc9; // Part.angle
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); alu_add(gb, B);
  CYC(b_+27, b_+29); alu_and(gb, 0x1f);

setAngleAndSpeed:
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+33); volcanoRock_subid0_setSpeedFromAngle_hook(gb); return; // jp

state0:
  CYC(b_+33, b_+36); SET_BC(0xfd80); // -0x280
  CALL_C(b_+36, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+39);
  CYC(b_+39, b_+40); L = E; // Part.state
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+43); L = 0xe4; // Part.collisionType
  CYC(b_+43, b_+45); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(b_+45, objectSetVisible80_hook, SYM(objectSetVisible80), b_+48);
  CALL_C(b_+48, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+51);
  CYC(b_+51, b_+53); alu_and(gb, 0x0f);
  CYC(b_+53, b_+55); alu_add(gb, 0x08);
  CYC(b_+55, b_+57); E = 0xc9; // Part.angle
  CYC(b_+57, SYM(volcanoRock_subid1)); goto setAngleAndSpeed; // jr
}

void volcanoRock_subid1_hook(GB *gb) {
  BASE(volcanoRock_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = volcanoRock_jump_table(gb);
    if (target == b_+14) goto substate0;
    if (target == b_+56) goto substate1;
    if (target == SYM(volcanoRock_common_substate2)) { volcanoRock_common_substate2_hook(gb); return; }
    if (target == SYM(volcanoRock_common_substate3)) { volcanoRock_common_substate3_hook(gb); return; }
    if (target == SYM(volcanoRock_common_substate4)) { volcanoRock_common_substate4_hook(gb); return; }
    volcanoRock_common_substate5_hook(gb); return;
  }

substate0:
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+16); L = E; // Part.state
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+19); L = 0xe4; // Part.collisionType
  CYC(b_+19, b_+21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+21, b_+23); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_add(gb, A);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+27, b_+29); mem_wr(gb, HL, alu_sla(gb, mem_rd(gb, HL)));
  CYC(b_+29, b_+31); L = 0xd0; // Part.speed
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+33, b_+35); L = 0xd4; // Part.speedZ
  CYC(b_+35, b_+37); A = 0x00;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0xfc);
  CALL_C(b_+40, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+43);
  CYC(b_+43, b_+45); alu_and(gb, 0x1f);
  CYC(b_+45, b_+47); E = 0xc9; // Part.angle
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); A = 0x01;
  CALL_C(b_+50, partSetAnimation_hook, SYM(partSetAnimation), b_+53);
  CYC(b_+53, b_+56); objectSetVisible80_hook(gb); return; // jp

substate1:
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); L = 0xcb; // Part.yh
  CYC(b_+59, b_+61); E = 0xcf; // Part.zh
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+63, b_+65); alu_add(gb, 0x08);
  CYC(b_+65, b_+67); alu_cp(gb, 0xf8);
  CYC(b_+67, b_+69); C = 0x10;
  if (F & FC) { CYCT(b_+69, b_+72); objectUpdateSpeedZ_paramC_hook(gb); return; } // jp c
  CYC(b_+69, b_+72);
  CYC(b_+72, b_+74); L = 0xc4; // Part.state
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); L = 0xc6; // Part.counter1
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+79, objectSetInvisible_hook, SYM(objectSetInvisible), b_+82);
  CYC(b_+82, SYM(volcanoRock_common_substate2)); volcanoRock_setRandomPosition_hook(gb); return; // jr
}

void volcanoRock_common_substate2_hook(GB *gb) {
  BASE(volcanoRock_common_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x10); // Part.counter1
  CYC(b_+6, b_+7); L = E; // Part.substate
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, SYM(volcanoRock_common_substate3)); objectSetVisiblec0_hook(gb); return; // jp
}

void volcanoRock_common_substate3_hook(GB *gb) {
  BASE(volcanoRock_common_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partAnimate_hook, SYM(partAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = 0xcf; // Part.zh
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CALL_C(b_+9, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+12);
  if (F & FC) { CYCT(b_+12, b_+15); partDelete_hook(gb); return; } // jp c
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = 0xc4; // Part.state
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = 0xd4; // Part.speedZ
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, SYM(volcanoRock_common_substate4)); objectSetVisible82_hook(gb); return; // jp
}

void volcanoRock_common_substate4_hook(GB *gb) {
  BASE(volcanoRock_common_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partAnimate_hook, SYM(partAnimate), b_+3);
  CYC(b_+3, b_+5); C = 0x16;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); L = 0xc4; // Part.state
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); L = 0xdd; // Part.oamTileIndexBase
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x26);
  CYC(b_+18, b_+20); A = 0x03;
  CALL_C(b_+20, partSetAnimation_hook, SYM(partSetAnimation), b_+23);
  CYC(b_+23, b_+25); A = 0x81; // SND_STRONG_POUND
  CYC(b_+25, SYM(volcanoRock_common_substate5)); playSound_b00_hook(gb); return; // jp
}

void volcanoRock_common_substate5_hook(GB *gb) {
  BASE(volcanoRock_common_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xe1; // Part.animParameter
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); partDelete_hook(gb); return; } // jp z
  CYC(b_+4, b_+7);
  CALL_C(b_+7, volcanoRock_setCollisionSize_hook, SYM(volcanoRock_setCollisionSize), b_+10);
  CYC(b_+10, SYM(volcanoRock_subid2)); partAnimate_hook(gb); return; // jp
}

void volcanoRock_subid2_hook(GB *gb) {
  BASE(volcanoRock_subid2);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = volcanoRock_jump_table(gb);
    if (target == b_+12) goto substate0;
    if (target == SYM(volcanoRock_common_substate2)) { volcanoRock_common_substate2_hook(gb); return; }
    if (target == SYM(volcanoRock_common_substate3)) { volcanoRock_common_substate3_hook(gb); return; }
    if (target == SYM(volcanoRock_common_substate4)) { volcanoRock_common_substate4_hook(gb); return; }
    volcanoRock_common_substate5_hook(gb); return;
  }

substate0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, SYM(volcanoRock_setRandomPosition)); mem_wr(gb, DE, A);
  volcanoRock_setRandomPosition_hook(gb); // falls through
}

void volcanoRock_setRandomPosition_hook(GB *gb) {
  BASE(volcanoRock_setRandomPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); SET_HL(hCameraY); // hCameraY
  CYC(b_+7, b_+9); E = 0xcb; // Part.yh
  CYC(b_+9, b_+11); alu_and(gb, 0x70);
  CYC(b_+11, b_+13); alu_add(gb, 0x08);
  CYC(b_+13, b_+14); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); alu_cpl(gb);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CYC(b_+17, b_+19); alu_and(gb, 0xfe);
  CYC(b_+19, b_+21); E = 0xcf; // Part.zh
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); L = 0xac; // <hCameraX
  CYC(b_+24, b_+26); E = 0xcd; // Part.xh
  CYC(b_+26, b_+27); A = B;
  CYC(b_+27, b_+29); alu_and(gb, 0x07);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+32); A = alu_swap(gb, A);
  CYC(b_+32, b_+34); alu_add(gb, 0x08);
  CYC(b_+34, b_+35); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); A = 0x02;
  CYC(b_+38, SYM(volcanoRock_subid0_setSpeedFromAngle)); partSetAnimation_hook(gb); return; // jp
}

void volcanoRock_subid0_setSpeedFromAngle_hook(GB *gb) {
  BASE(volcanoRock_subid0_setSpeedFromAngle);
  CYC(b_+0, b_+2); B = 0x14; // SPEED_80
  CYC(b_+2, b_+4); alu_cp(gb, 0x0d);
  if (F & FC) { CYCT(b_+4, b_+6); goto setSpeed; } // jr c
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); B = 0x0a; // SPEED_40
  CYC(b_+8, b_+10); alu_cp(gb, 0x14);
  if (F & FC) { CYCT(b_+10, b_+12); goto setSpeed; } // jr c
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); B = 0x14; // SPEED_80
setSpeed:
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+17); E = 0xd0; // Part.speed
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return; // ret
}

void volcanoRock_setCollisionSize_hook(GB *gb) {
  BASE(volcanoRock_setCollisionSize);
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  CYC(b_+1, b_+4); SET_HL(b_+13); // @data
  CYC(b_+4, b_+5); volcanoRock_addAToHl_from_rst(gb, b_+5);
  CYC(b_+5, b_+7); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return; // ret
}
