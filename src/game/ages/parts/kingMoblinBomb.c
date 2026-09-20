#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7153, 0x7155); E = 0xc4; // Part.state
  CYC(0x7155, 0x7156); A = mem_rd(gb, DE);
  CYC(0x7156, 0x7157); push_effect(gb, 0x7157);
  {
    uint16_t target = kingMoblinBomb_jump_table(gb);
    if (target == 0x7169) { kingMoblinBomb_state0_hook(gb); return; }
    if (target == 0x719f) { common_kingMoblinBomb_state1_hook(gb); return; }
    if (target == 0x71df) { kingMoblinBomb_state2_hook(gb); return; }
    if (target == 0x7224) { kingMoblinBomb_state3_hook(gb); return; }
    if (target == 0x7240) { kingMoblinBomb_state4_hook(gb); return; }
    if (target == 0x7247) { kingMoblinBomb_state5_hook(gb); return; }
    if (target == 0x725f) { kingMoblinBomb_state6_hook(gb); return; }
    if (target == 0x7281) { kingMoblinBomb_state7_hook(gb); return; }
    kingMoblinBomb_state8_hook(gb); return;
  }
}

void kingMoblinBomb_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7169, 0x716a); H = D;
  CYC(0x716a, 0x716b); L = E; // Part.state
  CYC(0x716b, 0x716c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x716c, 0x716e); L = 0xd0; // Part.speed
  CYC(0x716e, 0x7170); mem_wr(gb, HL, 0x55); // SPEED_220
  CYC(0x7170, 0x7172); L = 0xcb; // Part.yh
  CYC(0x7172, 0x7173); A = mem_rd(gb, HL);
  CYC(0x7173, 0x7175); alu_add(gb, 0x08);
  CYC(0x7175, 0x7176); mem_wr(gb, HL, A);
  CALL_C(0x7176, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7179);
  CYC(0x7179, 0x717b); alu_and(gb, 0x03);
  CYC(0x717b, 0x717e); SET_HL(0x7195); // @counter1Values
  CYC(0x717e, 0x717f); kingMoblinBomb_addAToHl_from_rst(gb, 0x717f);
  CYC(0x717f, 0x7181); E = 0xc6; // Part.counter1
  CYC(0x7181, 0x7182); A = mem_rd(gb, HL);
  CYC(0x7182, 0x7183); mem_wr(gb, DE, A);
  CYC(0x7183, 0x7185); A = 0x29; // Object.health
  CALL_C(0x7185, objectGetRelatedObject1Var_hook, 0x2160, 0x7188);
  CYC(0x7188, 0x7189); A = mem_rd(gb, HL);
  CYC(0x7189, 0x718a); A = alu_dec8(gb, A);
  CYC(0x718a, 0x718d); SET_HL(0x7199); // @numRedFlashes
  CYC(0x718d, 0x718e); kingMoblinBomb_addAToHl_from_rst(gb, 0x718e);
  CYC(0x718e, 0x7190); E = 0xf1; // Part.var31
  CYC(0x7190, 0x7191); A = mem_rd(gb, HL);
  CYC(0x7191, 0x7192); mem_wr(gb, DE, A);
  CYC(0x7192, 0x7195); objectSetVisiblec2_hook(gb); return; // jp
}

void common_kingMoblinBomb_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x719f, 0x71a1); E = 0xc6; // Part.counter1
  CYC(0x71a1, 0x71a2); A = mem_rd(gb, DE);
  CYC(0x71a2, 0x71a3); alu_or(gb, A);
  if (F & FZ) { CYCT(0x71a3, 0x71a5); goto L_71aa; } // jr z
  CYC(0x71a3, 0x71a5);
  CYC(0x71a5, 0x71a8); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x71a8, 0x71a9); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x71a9); return; } // ret c
  CYC(0x71a9, 0x71aa);
L_71aa:
  CALL_C(0x71aa, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x71ad);
  if (!(F & FZ)) { RET_TAKEN(0x71ad); return; } // ret nz
  CYC(0x71ad, 0x71ae);
  CYC(0x71ae, 0x71b0); L = 0xe1; // Part.animParameter
  CYC(0x71b0, 0x71b2); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x71b2, 0x71b4); goto animate; } // jr z
  CYC(0x71b2, 0x71b4);
  CYC(0x71b4, 0x71b6); mem_wr(gb, HL, 0x00);
  CYC(0x71b6, 0x71b8); L = 0xc7; // Part.counter2
  CYC(0x71b8, 0x71b9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x71b9, 0x71ba); A = mem_rd(gb, HL);
  CYC(0x71ba, 0x71bc); L = 0xf1; // Part.var31
  CYC(0x71bc, 0x71bd); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x71bd, 0x71bf); kingMoblinBomb_explode_hook(gb); return; } // jr nc
  CYC(0x71bd, 0x71bf);
animate:
  CYC(0x71bf, 0x71c2); partAnimate_hook(gb); return; // jp
}

void kingMoblinBomb_explode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x71c4, 0x71c6); L = 0xc4; // Part.state
  CYC(0x71c6, 0x71c8); mem_wr(gb, HL, 0x05);
  CYC(0x71c8, 0x71ca); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x71ca, 0x71cc); A = 0x0a;
  CYC(0x71cc, 0x71cd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x71cd, 0x71ce); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x71ce, 0x71d0); mem_wr(gb, HL, 0x0c);
  CYC(0x71d0, 0x71d2); A = 0x01;
  CALL_C(0x71d2, partSetAnimation_hook, 0x2988, 0x71d5);
  CALL_C(0x71d5, objectSetVisible82_hook, 0x1e69, 0x71d8);
  CYC(0x71d8, 0x71da); A = 0x6f; // SND_EXPLOSION
  CALL_C(0x71da, playSound_b00_hook, 0x0c98, 0x71dd);
  CYC(0x71dd, 0x71de); alu_xor(gb, A);
  RET(0x71de); return; // ret
}

void kingMoblinBomb_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x71df, 0x71e0); E = alu_inc8(gb, E); // Part.substate
  CYC(0x71e0, 0x71e1); A = mem_rd(gb, DE);
  CYC(0x71e1, 0x71e2); push_effect(gb, 0x71e2);
  {
    uint16_t target = kingMoblinBomb_jump_table(gb);
    if (target == 0x71ea) goto justGrabbed;
    if (target == 0x71f4) goto beingHeld;
    if (target == 0x71fb) goto released;
    goto atRest;
  }

justGrabbed:
  CYC(0x71ea, 0x71ec); A = 0x01;
  CYC(0x71ec, 0x71ed); mem_wr(gb, DE, A); // Part.substate
  CYC(0x71ed, 0x71ee); alu_xor(gb, A);
  CYC(0x71ee, 0x71f1); mem_wr(gb, 0xcc5b, A); // wLinkGrabState2
  CALL_C(0x71f1, objectSetVisiblec1_hook, 0x1e3c, 0x71f4);

beingHeld:
  CALL_C(0x71f4, common_kingMoblinBomb_state1_hook, 0x719f, 0x71f7);
  if (!(F & FZ)) { RET_TAKEN(0x71f7); return; } // ret nz
  CYC(0x71f7, 0x71f8);
  CYC(0x71f8, 0x71fb); dropLinkHeldItem_hook(gb); return; // jp

released:
  CYC(0x71fb, 0x71fd); E = 0xcb; // Part.yh
  CYC(0x71fd, 0x71fe); A = mem_rd(gb, DE);
  CYC(0x71fe, 0x7200); alu_cp(gb, 0x30);
  if (!(F & FC)) { CYCT(0x7200, 0x7202); goto beingHeld; } // jr nc
  CYC(0x7200, 0x7202);
  CYC(0x7202, 0x7203); H = D;
  CYC(0x7203, 0x7205); L = 0xcf; // Part.zh
  CYC(0x7205, 0x7207); E = 0xc2; // Part.subid
  CYC(0x7207, 0x7208); A = mem_rd(gb, DE);
  CYC(0x7208, 0x7209); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7209, 0x720b); goto beingHeld; } // jr nz
  CYC(0x7209, 0x720b);
  CYC(0x720b, 0x720e); SET_HL(0xdc15); // w1ReservedItemC.speedZ+1
  CYC(0x720e, 0x7210); mem_wr(gb, HL, alu_sra(gb, mem_rd(gb, HL)));
  CYC(0x7210, 0x7211); L = alu_dec8(gb, L);
  CYC(0x7211, 0x7213); mem_wr(gb, HL, alu_rr(gb, mem_rd(gb, HL)));
  CYC(0x7213, 0x7215); L = 0x10; // Item.speed
  CYC(0x7215, 0x7217); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(0x7217, 0x721a); common_kingMoblinBomb_state1_hook(gb); return; // jp

atRest:
  CYC(0x721a, 0x721c); E = 0xc4; // Part.state
  CYC(0x721c, 0x721e); A = 0x04;
  CYC(0x721e, 0x721f); mem_wr(gb, DE, A);
  CALL_C(0x721f, objectSetVisiblec2_hook, 0x1e45, 0x7222);
  CYC(0x7222, 0x7224); kingMoblinBomb_state4_hook(gb); return; // jr
}

void kingMoblinBomb_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7224, common_kingMoblinBomb_state1_hook, 0x719f, 0x7227);
  if (F & FZ) { RET_TAKEN(0x7227); return; } // ret z
  CYC(0x7227, 0x7228);
  CYC(0x7228, 0x722a); C = 0x20;
  CALL_C(0x722a, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x722d);
  if (F & FC) { CYCT(0x722d, 0x722f); goto doneBouncing; } // jr c
  CYC(0x722d, 0x722f);
  CYC(0x722f, 0x7231); A = 0x52; // SND_BOMB_LAND
  if (F & FZ) {
    CALL_C_CC(0x7231, playSound_b00_hook, 0x0c98, 0x7234);
  } else {
    CYC(0x7231, 0x7234);
  }
  CYC(0x7234, 0x7237); objectApplySpeed_hook(gb); return; // jp

doneBouncing:
  CYC(0x7237, 0x7239); A = 0x52; // SND_BOMB_LAND
  CALL_C(0x7239, playSound_b00_hook, 0x0c98, 0x723c);
  CYC(0x723c, 0x723d); H = D;
  CYC(0x723d, 0x723f); L = 0xc4; // Part.state
  CYC(0x723f, 0x7240); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  kingMoblinBomb_state4_hook(gb);
}

void kingMoblinBomb_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7240, common_kingMoblinBomb_state1_hook, 0x719f, 0x7243);
  if (F & FZ) { RET_TAKEN(0x7243); return; } // ret z
  CYC(0x7243, 0x7244);
  CYC(0x7244, 0x7247); objectAddToGrabbableObjectBuffer_hook(gb); return; // jp
}

void kingMoblinBomb_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7247, 0x7248); H = D;
  CYC(0x7248, 0x724a); L = 0xe1; // Part.animParameter
  CYC(0x724a, 0x724b); A = mem_rd(gb, HL);
  CYC(0x724b, 0x724c); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x724c, 0x724f); partDelete_hook(gb); return; } // jp z
  CYC(0x724c, 0x724f);
  CYC(0x724f, 0x7250); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7250, 0x7252); goto animate; } // jr z
  CYC(0x7250, 0x7252);
  CYC(0x7252, 0x7254); L = 0xe6; // Part.collisionRadiusY
  CYC(0x7254, 0x7255); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7255, 0x7256); mem_wr(gb, HL, A);
  CALL_C(0x7256, kingMoblinBomb_checkCollisionWithLink_hook, 0x7293, 0x7259);
  CALL_C(0x7259, kingMoblinBomb_checkCollisionWithKingMoblin_hook, 0x72bb, 0x725c);
animate:
  CYC(0x725c, 0x725f); partAnimate_hook(gb); return; // jp
}

void kingMoblinBomb_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x725f, 0x7262); SET_BC(0xfdc0); // -0x240
  CALL_C(0x7262, objectSetSpeedZ_hook, 0x239d, 0x7265);
  CYC(0x7265, 0x7266); L = E; // Part.state
  CYC(0x7266, 0x7267); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7267, 0x7269); L = 0xd0; // Part.speed
  CYC(0x7269, 0x726b); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x726b, 0x726d); L = 0xc6; // Part.counter1
  CYC(0x726d, 0x726f); mem_wr(gb, HL, 0x07);
  CYC(0x726f, 0x7271); A = 0x0d; // Object.xh
  CALL_C(0x7271, objectGetRelatedObject1Var_hook, 0x2160, 0x7274);
  CYC(0x7274, 0x7275); A = mem_rd(gb, HL);
  CYC(0x7275, 0x7277); alu_cp(gb, 0x50);
  CYC(0x7277, 0x7279); A = 0x07;
  if (F & FC) { CYCT(0x7279, 0x727b); goto L_727d; } // jr c
  CYC(0x7279, 0x727b);
  CYC(0x727b, 0x727d); A = 0x19;
L_727d:
  CYC(0x727d, 0x727f); E = 0xc9; // Part.angle
  CYC(0x727f, 0x7280); mem_wr(gb, DE, A);
  RET(0x7280); return; // ret
}

void kingMoblinBomb_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7281, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7284);
  if (!(F & FZ)) { RET_TAKEN(0x7284); return; } // ret nz
  CYC(0x7284, 0x7285);
  CYC(0x7285, 0x7286); L = E; // Part.state
  CYC(0x7286, 0x7287); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  kingMoblinBomb_state8_hook(gb);
}

void kingMoblinBomb_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7287, 0x7289); C = 0x20;
  CALL_C(0x7289, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x728c);
  if (!(F & FC)) { CYCT(0x728c, 0x728f); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x728c, 0x728f);
  CYC(0x728f, 0x7290); H = D;
  CYC(0x7290, 0x7293); kingMoblinBomb_explode_hook(gb); return; // jp
}

void kingMoblinBomb_checkCollisionWithLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7293, 0x7295); E = 0xf0; // Part.var30
  CYC(0x7295, 0x7296); A = mem_rd(gb, DE);
  CYC(0x7296, 0x7297); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7297); return; } // ret nz
  CYC(0x7297, 0x7298);
  CALL_C(0x7298, checkLinkVulnerable_hook, 0x1d28, 0x729b);
  if (!(F & FC)) { RET_TAKEN(0x729b); return; } // ret nc
  CYC(0x729b, 0x729c);
  CALL_C(0x729c, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x729f);
  if (!(F & FC)) { RET_TAKEN(0x729f); return; } // ret nc
  CYC(0x729f, 0x72a0);
  CALL_C(0x72a0, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x72a3);
  CYC(0x72a3, 0x72a6); SET_HL(0xd02d); // w1Link.knockbackCounter
  CYC(0x72a6, 0x72a8); mem_wr(gb, HL, 0x10);
  CYC(0x72a8, 0x72a9); L = alu_dec8(gb, L);
  CYC(0x72a9, 0x72aa); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x72aa, 0x72ac); mem_wr(gb, HL, 0x14);
  CYC(0x72ac, 0x72ad); L = alu_dec8(gb, L);
  CYC(0x72ad, 0x72af); mem_wr(gb, HL, 0x01);
  CYC(0x72af, 0x72b1); E = 0xe8; // Part.damage
  CYC(0x72b1, 0x72b3); L = 0x25; // <w1Link.damageToApply
  CYC(0x72b3, 0x72b4); A = mem_rd(gb, DE);
  CYC(0x72b4, 0x72b5); mem_wr(gb, HL, A);
  CYC(0x72b5, 0x72b7); E = 0xf0; // Part.var30
  CYC(0x72b7, 0x72b9); A = 0x01;
  CYC(0x72b9, 0x72ba); mem_wr(gb, DE, A);
  RET(0x72ba); return; // ret
}

void kingMoblinBomb_checkCollisionWithKingMoblin_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x72bb, 0x72bd); E = 0xd7; // Part.relatedObj1+1
  CYC(0x72bd, 0x72be); A = mem_rd(gb, DE);
  CYC(0x72be, 0x72bf); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x72bf); return; } // ret z
  CYC(0x72bf, 0x72c0);
  CYC(0x72c0, 0x72c2); A = 0x24; // Object.collisionType
  CALL_C(0x72c2, objectGetRelatedObject1Var_hook, 0x2160, 0x72c5);
  CYC(0x72c5, 0x72c7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x72c7); return; } // ret z
  CYC(0x72c7, 0x72c8);
  CYC(0x72c8, 0x72ca); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x72ca, 0x72cb); A = mem_rd(gb, HL);
  CYC(0x72cb, 0x72cc); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x72cc); return; } // ret nz
  CYC(0x72cc, 0x72cd);
  CALL_C(0x72cd, checkObjectsCollided_hook, 0x1d5a, 0x72d0);
  if (!(F & FC)) { RET_TAKEN(0x72d0); return; } // ret nc
  CYC(0x72d0, 0x72d1);
  CYC(0x72d1, 0x72d3); L = 0xaa; // Enemy.var2a
  CYC(0x72d3, 0x72d5); mem_wr(gb, HL, 0x98); // 0x80|ITEMCOLLISION_BOMB
  CYC(0x72d5, 0x72d7); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x72d7, 0x72d9); mem_wr(gb, HL, 0x1e);
  CYC(0x72d9, 0x72db); L = 0xa9; // Enemy.health
  CYC(0x72db, 0x72dc); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x72dc); return; // ret
}
