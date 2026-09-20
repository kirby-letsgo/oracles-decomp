#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t kingMoblinBomb_jump_table(GB *gb) {
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

static void kingMoblinBomb_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode3f_hook(GB *gb);
void kingMoblinBomb_state0_hook(GB *gb);
void common_kingMoblinBomb_state1_hook(GB *gb);
void kingMoblinBomb_explode_hook(GB *gb);
void kingMoblinBomb_state2_hook(GB *gb);
void kingMoblinBomb_state3_hook(GB *gb);
void kingMoblinBomb_state4_hook(GB *gb);
void kingMoblinBomb_state5_hook(GB *gb);
void kingMoblinBomb_state6_hook(GB *gb);
void kingMoblinBomb_state7_hook(GB *gb);
void kingMoblinBomb_state8_hook(GB *gb);
void kingMoblinBomb_checkCollisionWithLink_hook(GB *gb);
void kingMoblinBomb_checkCollisionWithKingMoblin_hook(GB *gb);

void partCode3f_hook(GB *gb) {
  BASE(partCode3f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  {
    uint16_t target = kingMoblinBomb_jump_table(gb);
    if (target == SYM(kingMoblinBomb_state0)) { kingMoblinBomb_state0_hook(gb); return; }
    if (target == SYM(common_kingMoblinBomb_state1)) { common_kingMoblinBomb_state1_hook(gb); return; }
    if (target == SYM(kingMoblinBomb_state2)) { kingMoblinBomb_state2_hook(gb); return; }
    if (target == SYM(kingMoblinBomb_state3)) { kingMoblinBomb_state3_hook(gb); return; }
    if (target == SYM(kingMoblinBomb_state4)) { kingMoblinBomb_state4_hook(gb); return; }
    if (target == SYM(kingMoblinBomb_state5)) { kingMoblinBomb_state5_hook(gb); return; }
    if (target == SYM(kingMoblinBomb_state6)) { kingMoblinBomb_state6_hook(gb); return; }
    if (target == SYM(kingMoblinBomb_state7)) { kingMoblinBomb_state7_hook(gb); return; }
    kingMoblinBomb_state8_hook(gb); return;
  }
}

void kingMoblinBomb_state0_hook(GB *gb) {
  BASE(kingMoblinBomb_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E; // Part.state
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xd0; // Part.speed
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x55); // SPEED_220
  CYC(b_+7, b_+9); L = 0xcb; // Part.yh
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_add(gb, 0x08);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CALL_C(b_+13, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x03);
  CYC(b_+18, b_+21); SET_HL(b_+44); // @counter1Values
  CYC(b_+21, b_+22); kingMoblinBomb_addAToHl_from_rst(gb, b_+22);
  CYC(b_+22, b_+24); E = 0xc6; // Part.counter1
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+28); A = 0x29; // Object.health
  CALL_C(b_+28, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+33); A = alu_dec8(gb, A);
  CYC(b_+33, b_+36); SET_HL(b_+48); // @numRedFlashes
  CYC(b_+36, b_+37); kingMoblinBomb_addAToHl_from_rst(gb, b_+37);
  CYC(b_+37, b_+39); E = 0xf1; // Part.var31
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+44); objectSetVisiblec2_hook(gb); return; // jp
}

void common_kingMoblinBomb_state1_hook(GB *gb) {
  BASE(common_kingMoblinBomb_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc6; // Part.counter1
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_71aa; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+9, b_+10); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+10); return; } // ret c
  CYC(b_+10, b_+11);
L_71aa:
  CALL_C(b_+11, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); L = 0xe1; // Part.animParameter
  CYC(b_+17, b_+19); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+19, b_+21); goto animate; } // jr z
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x00);
  CYC(b_+23, b_+25); L = 0xc7; // Part.counter2
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); L = 0xf1; // Part.var31
  CYC(b_+29, b_+30); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+30, b_+32); kingMoblinBomb_explode_hook(gb); return; } // jr nc
  CYC(b_+30, b_+32);
animate:
  CYC(b_+32, b_+35); partAnimate_hook(gb); return; // jp
}

void kingMoblinBomb_explode_hook(GB *gb) {
  BASE(kingMoblinBomb_explode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0xc4; // Part.state
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x05);
  CYC(b_+4, b_+6); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+6, b_+8); A = 0x0a;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x0c);
  CYC(b_+12, b_+14); A = 0x01;
  CALL_C(b_+14, partSetAnimation_hook, SYM(partSetAnimation), b_+17);
  CALL_C(b_+17, objectSetVisible82_hook, SYM(objectSetVisible82), b_+20);
  CYC(b_+20, b_+22); A = 0x6f; // SND_EXPLOSION
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+26); alu_xor(gb, A);
  RET(b_+26); return; // ret
}

void kingMoblinBomb_state2_hook(GB *gb) {
  BASE(kingMoblinBomb_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E); // Part.substate
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = kingMoblinBomb_jump_table(gb);
    if (target == b_+11) goto justGrabbed;
    if (target == b_+21) goto beingHeld;
    if (target == b_+28) goto released;
    goto atRest;
  }

justGrabbed:
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A); // Part.substate
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wLinkGrabState2, A); // wLinkGrabState2
  CALL_C(b_+18, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+21);

beingHeld:
  CALL_C(b_+21, common_kingMoblinBomb_state1_hook, SYM(common_kingMoblinBomb_state1), b_+24);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+28); dropLinkHeldItem_hook(gb); return; // jp

released:
  CYC(b_+28, b_+30); E = 0xcb; // Part.yh
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+33); alu_cp(gb, 0x30);
  if (!(F & FC)) { CYCT(b_+33, b_+35); goto beingHeld; } // jr nc
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+38); L = 0xcf; // Part.zh
  CYC(b_+38, b_+40); E = 0xc2; // Part.subid
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto beingHeld; } // jr nz
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+47); SET_HL((w1ReservedItemC_speedZ + 1)); // w1ReservedItemC.speedZ+1
  CYC(b_+47, b_+49); mem_wr(gb, HL, alu_sra(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+50); L = alu_dec8(gb, L);
  CYC(b_+50, b_+52); mem_wr(gb, HL, alu_rr(gb, mem_rd(gb, HL)));
  CYC(b_+52, b_+54); L = 0x10; // Item.speed
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(b_+56, b_+59); common_kingMoblinBomb_state1_hook(gb); return; // jp

atRest:
  CYC(b_+59, b_+61); E = 0xc4; // Part.state
  CYC(b_+61, b_+63); A = 0x04;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CALL_C(b_+64, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+67);
  CYC(b_+67, b_+69); kingMoblinBomb_state4_hook(gb); return; // jr
}

void kingMoblinBomb_state3_hook(GB *gb) {
  BASE(kingMoblinBomb_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, common_kingMoblinBomb_state1_hook, SYM(common_kingMoblinBomb_state1), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); C = 0x20;
  CALL_C(b_+6, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+9);
  if (F & FC) { CYCT(b_+9, b_+11); goto doneBouncing; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x52; // SND_BOMB_LAND
  if (F & FZ) {
    CALL_C_CC(b_+13, playSound_b00_hook, SYM(playSound_b00), b_+16);
  } else {
    CYC(b_+13, b_+16);
  }
  CYC(b_+16, b_+19); objectApplySpeed_hook(gb); return; // jp

doneBouncing:
  CYC(b_+19, b_+21); A = 0x52; // SND_BOMB_LAND
  CALL_C(b_+21, playSound_b00_hook, SYM(playSound_b00), b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = 0xc4; // Part.state
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  kingMoblinBomb_state4_hook(gb);
}

void kingMoblinBomb_state4_hook(GB *gb) {
  BASE(kingMoblinBomb_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, common_kingMoblinBomb_state1_hook, SYM(common_kingMoblinBomb_state1), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); objectAddToGrabbableObjectBuffer_hook(gb); return; // jp
}

void kingMoblinBomb_state5_hook(GB *gb) {
  BASE(kingMoblinBomb_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xe1; // Part.animParameter
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+8); partDelete_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto animate; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CALL_C(b_+15, kingMoblinBomb_checkCollisionWithLink_hook, SYM(kingMoblinBomb_checkCollisionWithLink), b_+18);
  CALL_C(b_+18, kingMoblinBomb_checkCollisionWithKingMoblin_hook, SYM(kingMoblinBomb_checkCollisionWithKingMoblin), b_+21);
animate:
  CYC(b_+21, b_+24); partAnimate_hook(gb); return; // jp
}

void kingMoblinBomb_state6_hook(GB *gb) {
  BASE(kingMoblinBomb_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xfdc0); // -0x240
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+7); L = E; // Part.state
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+10); L = 0xd0; // Part.speed
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+12, b_+14); L = 0xc6; // Part.counter1
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x07);
  CYC(b_+16, b_+18); A = 0x0d; // Object.xh
  CALL_C(b_+18, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_cp(gb, 0x50);
  CYC(b_+24, b_+26); A = 0x07;
  if (F & FC) { CYCT(b_+26, b_+28); goto L_727d; } // jr c
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); A = 0x19;
L_727d:
  CYC(b_+30, b_+32); E = 0xc9; // Part.angle
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  RET(b_+33); return; // ret
}

void kingMoblinBomb_state7_hook(GB *gb) {
  BASE(kingMoblinBomb_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E; // Part.state
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  kingMoblinBomb_state8_hook(gb);
}

void kingMoblinBomb_state8_hook(GB *gb) {
  BASE(kingMoblinBomb_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+8); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+12); kingMoblinBomb_explode_hook(gb); return; // jp
}

void kingMoblinBomb_checkCollisionWithLink_hook(GB *gb) {
  BASE(kingMoblinBomb_checkCollisionWithLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xf0; // Part.var30
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CALL_C(b_+5, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CALL_C(b_+9, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+12);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; } // ret nc
  CYC(b_+12, b_+13);
  CALL_C(b_+13, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+16);
  CYC(b_+16, b_+19); SET_HL(w1Link_knockbackCounter); // w1Link.knockbackCounter
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x10);
  CYC(b_+21, b_+22); L = alu_dec8(gb, L);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x14);
  CYC(b_+25, b_+26); L = alu_dec8(gb, L);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x01);
  CYC(b_+28, b_+30); E = 0xe8; // Part.damage
  CYC(b_+30, b_+32); L = 0x25; // <w1Link.damageToApply
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); E = 0xf0; // Part.var30
  CYC(b_+36, b_+38); A = 0x01;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  RET(b_+39); return; // ret
}

void kingMoblinBomb_checkCollisionWithKingMoblin_hook(GB *gb) {
  BASE(kingMoblinBomb_checkCollisionWithKingMoblin);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xd7; // Part.relatedObj1+1
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x24; // Object.collisionType
  CALL_C(b_+7, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+12); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+12); return; } // ret z
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } // ret nz
  CYC(b_+17, b_+18);
  CALL_C(b_+18, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+21);
  if (!(F & FC)) { RET_TAKEN(b_+21); return; } // ret nc
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); L = 0xaa; // Enemy.var2a
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x98); // 0x80|ITEMCOLLISION_BOMB
  CYC(b_+26, b_+28); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x1e);
  CYC(b_+30, b_+32); L = 0xa9; // Enemy.health
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+33); return; // ret
}
