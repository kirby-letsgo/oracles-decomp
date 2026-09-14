#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t blueStalfosProjectile_jump_table(GB *gb) {
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

static void blueStalfosProjectile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode3d_hook(GB *gb);
void blueStalfosProjectile_subid0_hook(GB *gb);
void blueStalfosProjectile_subid1_hook(GB *gb);
void blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(GB *gb);
void blueStalfosProjectile_applySpeed_hook(GB *gb);
void blueStalfosProjectile_subid1_uninitialized_hook(GB *gb);
void blueStalfosProjectile_checkShouldExplode_hook(GB *gb);
void blueStalfosProjectile_checkCollidedWithStalfos_hook(GB *gb);
void blueStalfosProjectile_explode_hook(GB *gb);
void blueStalfosProjectile_hitLink_hook(GB *gb);

void partCode3d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x6f6f, 0x6f71); goto normalStatus; } // jr z
  CYC(0x6f6f, 0x6f71);
  CYC(0x6f71, 0x6f72); H = D;
  CYC(0x6f72, 0x6f74); L = 0xc2; // Part.subid
  CYC(0x6f74, 0x6f75); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6f75, 0x6f76); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6f76, 0x6f78); goto normalStatus; } // jr nz
  CYC(0x6f76, 0x6f78);
  CYC(0x6f78, 0x6f7a); L = 0xea; // Part.var2a
  CYC(0x6f7a, 0x6f7b); A = mem_rd(gb, HL);
  CYC(0x6f7b, 0x6f7d); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x6f7d, 0x6f7e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6f7e, 0x6f81); blueStalfosProjectile_hitLink_hook(gb); return; } // jp z
  CYC(0x6f7e, 0x6f81);
  CYC(0x6f81, 0x6f83); alu_sub(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  CYC(0x6f83, 0x6f85); alu_cp(gb, 0x05); // ITEMCOLLISION_SWORDSPIN-ITEMCOLLISION_L1_SWORD+1
  if (!(F & FC)) { CYCT(0x6f85, 0x6f87); goto normalStatus; } // jr nc
  CYC(0x6f85, 0x6f87);
  CYC(0x6f87, 0x6f89); L = 0xc4; // Part.state
  CYC(0x6f89, 0x6f8a); A = mem_rd(gb, HL);
  CYC(0x6f8a, 0x6f8c); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(0x6f8c, 0x6f8e); goto normalStatus; } // jr nc
  CYC(0x6f8c, 0x6f8e);
  CYC(0x6f8e, 0x6f90); mem_wr(gb, HL, 0x04);
  CYC(0x6f90, 0x6f92); L = 0xd0; // Part.speed
  CYC(0x6f92, 0x6f94); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x6f94, 0x6f96); A = 0x68; // SND_UNKNOWN3
  CALL_C(0x6f96, playSound_b00_hook, 0x0c98, 0x6f99);

normalStatus:
  CYC(0x6f99, 0x6f9b); E = 0xc2; // Part.subid
  CYC(0x6f9b, 0x6f9c); A = mem_rd(gb, DE);
  {
    CYC(0x6f9c, 0x6f9d); push_effect(gb, 0x6f9d);
    uint16_t target = blueStalfosProjectile_jump_table(gb);
    if (target == 0x6fa1) { blueStalfosProjectile_subid0_hook(gb); return; }
    blueStalfosProjectile_subid1_hook(gb); return;
  }
}

void blueStalfosProjectile_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6fa1, 0x6fa3); E = 0xc4; // Part.state
  CYC(0x6fa3, 0x6fa4); A = mem_rd(gb, DE);
  {
    CYC(0x6fa4, 0x6fa5); push_effect(gb, 0x6fa5);
    uint16_t target = blueStalfosProjectile_jump_table(gb);
    if (target == 0x6fb3) goto state0;
    if (target == 0x6fea) goto state1;
    if (target == 0x7019) goto state2;
    if (target == 0x7026) goto state3;
    if (target == 0x702b) goto state4;
    if (target == 0x7039) goto state5;
    goto state6;
  }

state0:
  CYC(0x6fb3, 0x6fb4); H = D;
  CYC(0x6fb4, 0x6fb5); L = E; // Part.state
  CYC(0x6fb5, 0x6fb6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6fb6, 0x6fb8); L = 0xc6; // Part.counter1
  CYC(0x6fb8, 0x6fba); mem_wr(gb, HL, 0x28);
  CYC(0x6fba, 0x6fbc); L = 0xcb; // Part.yh
  CYC(0x6fbc, 0x6fbd); A = mem_rd(gb, HL);
  CYC(0x6fbd, 0x6fbf); alu_sub(gb, 0x18);
  CYC(0x6fbf, 0x6fc0); mem_wr(gb, HL, A);
  CYC(0x6fc0, 0x6fc2); L = 0xd0; // Part.speed
  CYC(0x6fc2, 0x6fc4); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x6fc4, 0x6fc5); push_effect(gb, HL);
  CYC(0x6fc5, 0x6fc7); A = 0x32; // Object.var32
  CALL_C(0x6fc7, objectGetRelatedObject1Var_hook, 0x2160, 0x6fca);
  CYC(0x6fca, 0x6fcb); A = mem_rd(gb, HL);
  CYC(0x6fcb, 0x6fcc); A = alu_inc8(gb, A);
  CYC(0x6fcc, 0x6fce); alu_and(gb, 0x07);
  CYC(0x6fce, 0x6fcf); mem_wr(gb, HL, A);
  CYC(0x6fcf, 0x6fd2); SET_HL(0x6fe9); // @ballPatterns
  CALL_C(0x6fd2, checkFlag_hook, 0x0205, 0x6fd5);
  CYC(0x6fd5, 0x6fd6); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(0x6fd6, 0x6fd8); goto L_6fe1; } // jr z
  CYC(0x6fd6, 0x6fd8);
  CYC(0x6fd8, 0x6fda); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x6fda, 0x6fdc); L = 0xe5; // Part.enemyCollisionMode
  CYC(0x6fdc, 0x6fde); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(0x6fde, 0x6fe0); L = 0xc3; // Part.var03
  CYC(0x6fe0, 0x6fe1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_6fe1:
  CYC(0x6fe1, 0x6fe3); A = 0xbf; // SND_BLUE_STALFOS_CHARGE
  CALL_C(0x6fe3, playSound_b00_hook, 0x0c98, 0x6fe6);
  CYC(0x6fe6, 0x6fea); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x6fea, partCommon_decCounter1IfNonzero, 0x40a7, 0x6fed);
  if (!(F & FZ)) { CYCT(0x6fed, 0x6fef); goto animate; } // jr nz
  CYC(0x6fed, 0x6fef);
  CYC(0x6fef, 0x6ff1); mem_wr(gb, HL, 0x28); // counter1
  CYC(0x6ff1, 0x6ff2); L = alu_inc8(gb, L);
  CYC(0x6ff2, 0x6ff3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // counter2++
  CYC(0x6ff3, 0x6ff4); A = mem_rd(gb, HL);
  CYC(0x6ff4, 0x6ff6); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x6ff6, 0x6ff9); partSetAnimation_hook(gb); return; } // jp c
  CYC(0x6ff6, 0x6ff9);
  CYC(0x6ff9, 0x6ffb); mem_wr(gb, HL, 0x14); // counter2
  CYC(0x6ffb, 0x6ffc); L = alu_dec8(gb, L);
  CYC(0x6ffc, 0x6ffe); mem_wr(gb, HL, 0x00); // counter1
  CYC(0x6ffe, 0x6fff); L = E; // Part.state
  CYC(0x6fff, 0x7000); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7000, 0x7002); L = 0xe4; // Part.collisionType
  CYC(0x7002, 0x7004); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(0x7004, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x7007);
  CYC(0x7007, 0x7009); E = 0xc9; // Part.angle
  CYC(0x7009, 0x700a); mem_wr(gb, DE, A);
  CYC(0x700a, 0x700c); E = 0xc3; // Part.var03
  CYC(0x700c, 0x700d); A = mem_rd(gb, DE);
  CYC(0x700d, 0x700f); alu_add(gb, 0x02);
  CALL_C(0x700f, partSetAnimation_hook, 0x2988, 0x7012);
  CYC(0x7012, 0x7014); A = 0xba; // SND_BEAM1
  CALL_C(0x7014, playSound_b00_hook, 0x0c98, 0x7017);
  CYC(0x7017, 0x7019); goto animate; // jr

state2:
  CYC(0x7019, 0x701a); H = D;
  CYC(0x701a, 0x701c); L = 0xc7; // Part.counter2
  CYC(0x701c, 0x701d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x701d, 0x701f); goto L_7021; } // jr nz
  CYC(0x701d, 0x701f);
  CYC(0x701f, 0x7020); L = E; // Part.state
  CYC(0x7020, 0x7021); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_7021:
  CALL_C(0x7021, blueStalfosProjectile_checkShouldExplode_hook, 0x7084, 0x7024);
  CYC(0x7024, 0x7026); blueStalfosProjectile_applySpeed_hook(gb); return; // jr

state3:
  CALL_C(0x7026, blueStalfosProjectile_checkShouldExplode_hook, 0x7084, 0x7029);
  CYC(0x7029, 0x702b); blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(gb); return; // jr

state4:
  CYC(0x702b, 0x702c); H = D;
  CYC(0x702c, 0x702d); L = E; // Part.state
  CYC(0x702d, 0x702e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x702e, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x7031);
  CYC(0x7031, 0x7033); alu_xor(gb, 0x10);
  CYC(0x7033, 0x7035); E = 0xc9; // Part.angle
  CYC(0x7035, 0x7036); mem_wr(gb, DE, A);
animate:
  CYC(0x7036, 0x7039); partAnimate_hook(gb); return; // jp

state5:
  CALL_C(0x7039, blueStalfosProjectile_checkCollidedWithStalfos_hook, 0x70a7, 0x703c);
  if (F & FC) { CYCT(0x703c, 0x703f); partDelete_hook(gb); return; } // jp c
  CYC(0x703c, 0x703f);
  CYC(0x703f, 0x7041); blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(gb); return; // jr

state6:
  CYC(0x7041, 0x7043); B = 0x06;
  CALL_C(0x7043, checkBPartSlotsAvailable_hook, 0x20f2, 0x7046);
  if (!(F & FZ)) { RET_TAKEN(0x7046); return; } // ret nz
  CYC(0x7046, 0x7047);
  CALL_C(0x7047, blueStalfosProjectile_explode_hook, 0x70b9, 0x704a);
  CYC(0x704a, 0x704c); A = 0xa4; // SND_BEAM
  CALL_C(0x704c, playSound_b00_hook, 0x0c98, 0x704f);
  CYC(0x704f, 0x7052); partDelete_hook(gb); return; // jp
}

void blueStalfosProjectile_subid1_hook(GB *gb) {
  CYC(0x7052, 0x7054); E = 0xc4; // Part.state
  CYC(0x7054, 0x7055); A = mem_rd(gb, DE);
  CYC(0x7055, 0x7056); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7056, 0x7058); blueStalfosProjectile_subid1_uninitialized_hook(gb); return; } // jr z
  CYC(0x7056, 0x7058);
  blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(gb);
}

void blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7058, partCommon_checkOutOfBounds, 0x407e, 0x705b);
  if (F & FZ) { CYCT(0x705b, 0x705e); partDelete_hook(gb); return; } // jp z
  CYC(0x705b, 0x705e);
  blueStalfosProjectile_applySpeed_hook(gb);
}

void blueStalfosProjectile_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x705e, objectApplySpeed_hook, 0x201d, 0x7061);
  CYC(0x7061, 0x7064); partAnimate_hook(gb); return; // jp
}

void blueStalfosProjectile_subid1_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7064, 0x7065); H = D;
  CYC(0x7065, 0x7066); L = E; // Part.state
  CYC(0x7066, 0x7067); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7067, 0x7069); L = 0xe4; // Part.collisionType
  CYC(0x7069, 0x706b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x706b, 0x706d); L = 0xe5; // Part.enemyCollisionMode
  CYC(0x706d, 0x706f); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(0x706f, 0x7071); L = 0xd0; // Part.speed
  CYC(0x7071, 0x7073); mem_wr(gb, HL, 0x46); // SPEED_1c0
  CYC(0x7073, 0x7075); L = 0xe8; // Part.damage
  CYC(0x7075, 0x7077); mem_wr(gb, HL, 0xfc); // -4
  CYC(0x7077, 0x7079); L = 0xe6; // Part.collisionRadiusY
  CYC(0x7079, 0x707b); A = 0x02;
  CYC(0x707b, 0x707c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x707c, 0x707d); mem_wr(gb, HL, A);
  CYC(0x707d, 0x707e); alu_add(gb, A);
  CALL_C(0x707e, partSetAnimation_hook, 0x2988, 0x7081);
  CYC(0x7081, 0x7084); objectSetVisible81_hook(gb); return; // jp
}

void blueStalfosProjectile_checkShouldExplode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7084, 0x7087); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x7087, 0x7089); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x7089); return; } // ret nz
  CYC(0x7089, 0x708a);
  CALL_C(0x708a, partCommon_decCounter1IfNonzero, 0x40a7, 0x708d);
  if (!(F & FZ)) { RET_TAKEN(0x708d); return; } // ret nz
  CYC(0x708d, 0x708e);
  CYC(0x708e, 0x7090); C = 0x28;
  CALL_C(0x7090, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x7093);
  if (!(F & FC)) { RET_TAKEN(0x7093); return; } // ret nc
  CYC(0x7093, 0x7094);
  CYC(0x7094, 0x7095); H = D;
  CYC(0x7095, 0x7097); L = 0xc6; // Part.counter1
  CYC(0x7097, 0x7098); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7098, 0x709a); E = 0xc3; // Part.var03
  CYC(0x709a, 0x709b); A = mem_rd(gb, DE);
  CYC(0x709b, 0x709c); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x709c); return; } // ret z
  CYC(0x709c, 0x709d);
  CYC(0x709d, 0x709e); SET_BC(pop_effect(gb)); // discard return address
  CYC(0x709e, 0x70a0); L = 0xe4; // Part.collisionType
  CYC(0x70a0, 0x70a2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x70a2, 0x70a4); L = 0xc4; // Part.state
  CYC(0x70a4, 0x70a6); mem_wr(gb, HL, 0x06);
  RET(0x70a6); return; // ret
}

void blueStalfosProjectile_checkCollidedWithStalfos_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x70a7, 0x70a9); A = 0x00; // Object.enabled
  CALL_C(0x70a9, objectGetRelatedObject1Var_hook, 0x2160, 0x70ac);
  CALL_C(0x70ac, checkObjectsCollided_hook, 0x1d5a, 0x70af);
  if (!(F & FC)) { RET_TAKEN(0x70af); return; } // ret nc
  CYC(0x70af, 0x70b0);
  CYC(0x70b0, 0x70b2); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x70b2, 0x70b4); mem_wr(gb, HL, 0x1e);
  CYC(0x70b4, 0x70b6); L = 0x84; // Enemy.state
  CYC(0x70b6, 0x70b8); mem_wr(gb, HL, 0x14);
  RET(0x70b8); return; // ret
}

void blueStalfosProjectile_explode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x70b9, 0x70bb); C = 0x06;
next:
  CALL_C(0x70bb, getFreePartSlot_hook, 0x3e8e, 0x70be);
  CYC(0x70be, 0x70c0); mem_wr(gb, HL, 0x3d); // PART_BLUE_STALFOS_PROJECTILE
  CYC(0x70c0, 0x70c1); L = alu_inc8(gb, L);
  CYC(0x70c1, 0x70c2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // subid = 1
  CALL_C(0x70c2, objectCopyPosition_hook, 0x2242, 0x70c5);
  CYC(0x70c5, 0x70c7); L = 0xd6; // Part.relatedObj1
  CYC(0x70c7, 0x70c8); E = L;
  CYC(0x70c8, 0x70c9); A = mem_rd(gb, DE);
  CYC(0x70c9, 0x70ca); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x70ca, 0x70cb); E = L;
  CYC(0x70cb, 0x70cc); A = mem_rd(gb, DE);
  CYC(0x70cc, 0x70cd); mem_wr(gb, HL, A);
  CYC(0x70cd, 0x70ce); B = H;
  CYC(0x70ce, 0x70cf); A = C;
  CYC(0x70cf, 0x70d2); SET_HL(0x70dc); // @angleVals
  CYC(0x70d2, 0x70d3); blueStalfosProjectile_addAToHl_from_rst(gb, 0x70d3);
  CYC(0x70d3, 0x70d4); A = mem_rd(gb, HL);
  CYC(0x70d4, 0x70d5); H = B;
  CYC(0x70d5, 0x70d7); L = 0xc9; // Part.angle
  CYC(0x70d7, 0x70d8); mem_wr(gb, HL, A);
  CYC(0x70d8, 0x70d9); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x70d9, 0x70db); goto next; } // jr nz
  CYC(0x70d9, 0x70db);
  RET(0x70db); return; // ret
}

void blueStalfosProjectile_hitLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x70e3, 0x70e5); A = 0x04; // Object.state
  CALL_C(0x70e5, objectGetRelatedObject1Var_hook, 0x2160, 0x70e8);
  CYC(0x70e8, 0x70ea); mem_wr(gb, HL, 0x10);
  CYC(0x70ea, 0x70ed); partDelete_hook(gb); return; // jp
}
