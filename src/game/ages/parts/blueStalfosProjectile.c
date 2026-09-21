#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCode3d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto normalStatus; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); L = 0xea; // Part.var2a
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+15, b_+18); blueStalfosProjectile_hitLink_hook(gb); return; } // jp z
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); alu_sub(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  CYC(b_+20, b_+22); alu_cp(gb, 0x05); // ITEMCOLLISION_SWORDSPIN-ITEMCOLLISION_L1_SWORD+1
  if (!(F & FC)) { CYCT(b_+22, b_+24); goto normalStatus; } // jr nc
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); L = 0xc4; // Part.state
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto normalStatus; } // jr nc
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x04);
  CYC(b_+33, b_+35); L = 0xd0; // Part.speed
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+37, b_+39); A = 0x68; // SND_UNKNOWN3
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);

normalStatus:
  CYC(b_+42, b_+44); E = 0xc2; // Part.subid
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  {
    CYC(b_+45, b_+46); push_effect(gb, b_+46);
    uint16_t target = blueStalfosProjectile_jump_table(gb);
    if (target == SYM(blueStalfosProjectile_subid0)) { blueStalfosProjectile_subid0_hook(gb); return; }
    TAIL(blueStalfosProjectile_subid1);
  }
}

void blueStalfosProjectile_subid0_hook(GB *gb) {
  BASE(blueStalfosProjectile_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = blueStalfosProjectile_jump_table(gb);
    if (target == b_+18) goto state0;
    if (target == b_+73) goto state1;
    if (target == b_+120) goto state2;
    if (target == b_+133) goto state3;
    if (target == b_+138) goto state4;
    if (target == b_+152) goto state5;
    goto state6;
  }

state0:
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+20); L = E; // Part.state
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+21, b_+23); L = 0xc6; // Part.counter1
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x28);
  CYC(b_+25, b_+27); L = 0xcb; // Part.yh
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_sub(gb, 0x18);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = 0xd0; // Part.speed
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+35, b_+36); push_effect(gb, HL);
  CYC(b_+36, b_+38); A = 0x32; // Object.var32
  CALL_C(b_+38, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+41);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+43); A = alu_inc8(gb, A);
  CYC(b_+43, b_+45); alu_and(gb, 0x07);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+49); SET_HL(b_+72); // @ballPatterns
  CALL_C(b_+49, checkFlag_hook, SYM(checkFlag), b_+52);
  CYC(b_+52, b_+53); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(b_+53, b_+55); goto L_6fe1; } // jr z
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+57, b_+59); L = 0xe5; // Part.enemyCollisionMode
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(b_+61, b_+63); L = 0xc3; // Part.var03
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_6fe1:
  CYC(b_+64, b_+66); A = 0xbf; // SND_BLUE_STALFOS_CHARGE
  CALL_C(b_+66, playSound_b00_hook, SYM(playSound_b00), b_+69);
  CYC(b_+69, b_+73); TAIL(objectSetVisible81); // jp

state1:
  CALL_C(b_+73, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+76);
  if (!(F & FZ)) { CYCT(b_+76, b_+78); goto animate; } // jr nz
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x28); // counter1
  CYC(b_+80, b_+81); L = alu_inc8(gb, L);
  CYC(b_+81, b_+82); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // counter2++
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+85); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+85, b_+88); partSetAnimation_hook(gb); return; } // jp c
  CYC(b_+85, b_+88);
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x14); // counter2
  CYC(b_+90, b_+91); L = alu_dec8(gb, L);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x00); // counter1
  CYC(b_+93, b_+94); L = E; // Part.state
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+95, b_+97); L = 0xe4; // Part.collisionType
  CYC(b_+97, b_+99); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(b_+99, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+102);
  CYC(b_+102, b_+104); E = 0xc9; // Part.angle
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CYC(b_+105, b_+107); E = 0xc3; // Part.var03
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+110); alu_add(gb, 0x02);
  CALL_C(b_+110, partSetAnimation_hook, SYM(partSetAnimation), b_+113);
  CYC(b_+113, b_+115); A = 0xba; // SND_BEAM1
  CALL_C(b_+115, playSound_b00_hook, SYM(playSound_b00), b_+118);
  CYC(b_+118, b_+120); goto animate; // jr

state2:
  CYC(b_+120, b_+121); H = D;
  CYC(b_+121, b_+123); L = 0xc7; // Part.counter2
  CYC(b_+123, b_+124); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+124, b_+126); goto L_7021; } // jr nz
  CYC(b_+124, b_+126);
  CYC(b_+126, b_+127); L = E; // Part.state
  CYC(b_+127, b_+128); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_7021:
  CALL_C(b_+128, blueStalfosProjectile_checkShouldExplode_hook, SYM(blueStalfosProjectile_checkShouldExplode), b_+131);
  CYC(b_+131, b_+133); TAIL(blueStalfosProjectile_applySpeed); // jr

state3:
  CALL_C(b_+133, blueStalfosProjectile_checkShouldExplode_hook, SYM(blueStalfosProjectile_checkShouldExplode), b_+136);
  CYC(b_+136, b_+138); TAIL(blueStalfosProjectile_applySpeedAndDeleteIfOffScreen); // jr

state4:
  CYC(b_+138, b_+139); H = D;
  CYC(b_+139, b_+140); L = E; // Part.state
  CYC(b_+140, b_+141); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+141, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+144);
  CYC(b_+144, b_+146); alu_xor(gb, 0x10);
  CYC(b_+146, b_+148); E = 0xc9; // Part.angle
  CYC(b_+148, b_+149); mem_wr(gb, DE, A);
animate:
  CYC(b_+149, b_+152); TAIL(partAnimate); // jp

state5:
  CALL_C(b_+152, blueStalfosProjectile_checkCollidedWithStalfos_hook, SYM(blueStalfosProjectile_checkCollidedWithStalfos), b_+155);
  if (F & FC) { CYCT(b_+155, b_+158); partDelete_hook(gb); return; } // jp c
  CYC(b_+155, b_+158);
  CYC(b_+158, b_+160); TAIL(blueStalfosProjectile_applySpeedAndDeleteIfOffScreen); // jr

state6:
  CYC(b_+160, b_+162); B = 0x06;
  CALL_C(b_+162, checkBPartSlotsAvailable_hook, SYM(checkBPartSlotsAvailable), b_+165);
  if (!(F & FZ)) { RET_TAKEN(b_+165); return; } // ret nz
  CYC(b_+165, b_+166);
  CALL_C(b_+166, blueStalfosProjectile_explode_hook, SYM(blueStalfosProjectile_explode), b_+169);
  CYC(b_+169, b_+171); A = 0xa4; // SND_BEAM
  CALL_C(b_+171, playSound_b00_hook, SYM(playSound_b00), b_+174);
  CYC(b_+174, b_+177); TAIL(partDelete); // jp
}

void blueStalfosProjectile_subid1_hook(GB *gb) {
  BASE(blueStalfosProjectile_subid1);
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); blueStalfosProjectile_subid1_uninitialized_hook(gb); return; } // jr z
  CYC(b_+4, b_+6);
  blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(gb);
}

void blueStalfosProjectile_applySpeedAndDeleteIfOffScreen_hook(GB *gb) {
  BASE(blueStalfosProjectile_applySpeedAndDeleteIfOffScreen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); partDelete_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  blueStalfosProjectile_applySpeed_hook(gb);
}

void blueStalfosProjectile_applySpeed_hook(GB *gb) {
  BASE(blueStalfosProjectile_applySpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+6); TAIL(partAnimate); // jp
}

void blueStalfosProjectile_subid1_uninitialized_hook(GB *gb) {
  BASE(blueStalfosProjectile_subid1_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E; // Part.state
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xe4; // Part.collisionType
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+7, b_+9); L = 0xe5; // Part.enemyCollisionMode
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(b_+11, b_+13); L = 0xd0; // Part.speed
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x46); // SPEED_1c0
  CYC(b_+15, b_+17); L = 0xe8; // Part.damage
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xfc); // -4
  CYC(b_+19, b_+21); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+21, b_+23); A = 0x02;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+26); alu_add(gb, A);
  CALL_C(b_+26, partSetAnimation_hook, SYM(partSetAnimation), b_+29);
  CYC(b_+29, b_+32); TAIL(objectSetVisible81); // jp
}

void blueStalfosProjectile_checkShouldExplode_hook(GB *gb) {
  BASE(blueStalfosProjectile_checkShouldExplode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CALL_C(b_+6, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); C = 0x28;
  CALL_C(b_+12, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+15);
  if (!(F & FC)) { RET_TAKEN(b_+15); return; } // ret nc
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = 0xc6; // Part.counter1
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); E = 0xc3; // Part.var03
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+24); return; } // ret z
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); SET_BC(pop_effect(gb)); // discard return address
  CYC(b_+26, b_+28); L = 0xe4; // Part.collisionType
  CYC(b_+28, b_+30); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+30, b_+32); L = 0xc4; // Part.state
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x06);
  RET(b_+34); return; // ret
}

void blueStalfosProjectile_checkCollidedWithStalfos_hook(GB *gb) {
  BASE(blueStalfosProjectile_checkCollidedWithStalfos);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00; // Object.enabled
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CALL_C(b_+5, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x1e);
  CYC(b_+13, b_+15); L = 0x84; // Enemy.state
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x14);
  RET(b_+17); return; // ret
}

void blueStalfosProjectile_explode_hook(GB *gb) {
  BASE(blueStalfosProjectile_explode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x06;
next:
  CALL_C(b_+2, getFreePartSlot_hook, SYM(getFreePartSlot), b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x3d); // PART_BLUE_STALFOS_PROJECTILE
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // subid = 1
  CALL_C(b_+9, objectCopyPosition_hook, SYM(objectCopyPosition), b_+12);
  CYC(b_+12, b_+14); L = 0xd6; // Part.relatedObj1
  CYC(b_+14, b_+15); E = L;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); E = L;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+21); B = H;
  CYC(b_+21, b_+22); A = C;
  CYC(b_+22, b_+25); SET_HL(b_+35); // @angleVals
  CYC(b_+25, b_+26); blueStalfosProjectile_addAToHl_from_rst(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); H = B;
  CYC(b_+28, b_+30); L = 0xc9; // Part.angle
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+32); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto next; } // jr nz
  CYC(b_+32, b_+34);
  RET(b_+34); return; // ret
}

void blueStalfosProjectile_hitLink_hook(GB *gb) {
  BASE(blueStalfosProjectile_hitLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04; // Object.state
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10);
  CYC(b_+7, b_+10); TAIL(partDelete); // jp
}
