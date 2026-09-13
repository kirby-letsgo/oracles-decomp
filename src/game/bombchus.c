#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void bombchuUpdateSpeed_hook(GB *gb);
void bombchuGetTileCollisions_hook(GB *gb);
void bombchuCheckWallsAndApplySpeed_hook(GB *gb);
void bombchuUpdateAngle_topDown_hook(GB *gb);
void bombchuSetAnimationFromAngle_hook(GB *gb);
void bombchuUpdateAngle_sidescrolling_hook(GB *gb);
void bombchuClearCounter2AndInitializeExplosion_hook(GB *gb);

static uint16_t bombchu_jump_table(GB *gb) {
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

static void bombchu_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void bombchu_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void bombchuUpdateVelocity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5272, 0x5275); A = W8(wFrameCounter);
  CYC(0x5275, 0x5277); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(0x5277, bombchuUpdateAngle_topDown_hook, 0x5361, 0x527a);
  else CYC(0x5277, 0x527a);
  bombchuUpdateSpeed_hook(gb);
}

static void bombchu_update_speed(GB *gb, uint16_t sp0_) {
  CYC(0x5285, 0x5287); E = 0x09;
  CALL_C(0x5287, bombchuGetTileCollisions_hook, 0x52bd, 0x528a);
  CYC(0x528a, 0x528c); alu_cp(gb, 0x10);
  if (F & FZ) {
    CYCT(0x528c, 0x528e);
    goto impassable;
  }
  CYC(0x528c, 0x528e);
  CYC(0x528e, 0x5290); alu_cp(gb, 0x15);
  if (F & FZ) {
    CYCT(0x5290, 0x5292);
    goto impassable;
  }
  CYC(0x5290, 0x5292);
  CYC(0x5292, 0x5293); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x5293, 0x5295);
    goto impassable;
  }
  CYC(0x5293, 0x5295);
  CYC(0x5295, 0x5296); A = alu_dec8(gb, A);
  CYC(0x5296, 0x5298); E = 0x11;
  CYC(0x5298, 0x5299); A = mem_rd(gb, DE);
  if (F & FZ) {
    CYCT(0x5299, 0x529b);
  } else {
    CYC(0x5299, 0x529b);
    CYC(0x529b, 0x529c); E = A;
    CYC(0x529c, 0x529f); SET_HL(0x64d2);
    CALL_C(0x529f, lookupKey_hook, 0x1e06, 0x52a2);
  }
  CYC(0x52a2, 0x52a3); H = D;
  CYC(0x52a3, 0x52a5); L = 0x10;
  CYC(0x52a5, 0x52a6); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x52a6, 0x52a7); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(0x52a7, 0x52a8); ret_effect(gb); return; }
  CYC(0x52a7, 0x52a8);
  CYC(0x52a8, 0x52aa); L = 0x14;
  CYC(0x52aa, 0x52ac); A = 0x80;
  CYC(0x52ac, 0x52ad); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x52ad, 0x52af); mem_wr(gb, HL, 0xff);
  CYC(0x52af, 0x52b0); ret_effect(gb);
  return;

impassable:
  CYC(0x52b0, 0x52b1); H = D;
  CYC(0x52b1, 0x52b3); L = 0x31;
  CYC(0x52b3, 0x52b4); A = mem_rd(gb, HL);
  CYC(0x52b4, 0x52b6); L = 0x09;
  CYC(0x52b6, 0x52b7); alu_add(gb, mem_rd(gb, HL));
  CYC(0x52b7, 0x52b9); alu_and(gb, 0x18);
  CYC(0x52b9, 0x52ba); mem_wr(gb, HL, A);
  CYC(0x52ba, 0x52bd);
  bombchuSetAnimationFromAngle_hook(gb);
}

void bombchuUpdateSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x527a, 0x527d); push_effect(gb, 0x527d);
  bombchu_update_speed(gb, sp0_);
  CYC(0x527d, 0x527f); C = 0x18;
  CALL_C(0x527f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5282);
  CYC(0x5282, 0x5285);
  objectApplySpeed_hook(gb);
}

void bombchuGetTileCollisions_hook(GB *gb) {
  CYC(0x52bd, 0x52be); H = D;
  CYC(0x52be, 0x52c0); L = 0x0b;
  CYC(0x52c0, 0x52c1); B = mem_rd(gb, HL);
  CYC(0x52c1, 0x52c3); L = 0x0d;
  CYC(0x52c3, 0x52c4); C = mem_rd(gb, HL);
  CYC(0x52c4, 0x52c5); A = mem_rd(gb, DE);
  CYC(0x52c5, 0x52c6); alu_rrca(gb);
  CYC(0x52c6, 0x52c7); alu_rrca(gb);
  CYC(0x52c7, 0x52ca); SET_HL(0x52d4);
  CYC(0x52ca, 0x52cb); bombchu_add_a_to_hl(gb, 0x52cb);
  CYC(0x52cb, 0x52cc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x52cc, 0x52cd); alu_add(gb, B);
  CYC(0x52cd, 0x52ce); B = A;
  CYC(0x52ce, 0x52cf); A = mem_rd(gb, HL);
  CYC(0x52cf, 0x52d0); alu_add(gb, C);
  CYC(0x52d0, 0x52d1); C = A;
  CYC(0x52d1, 0x52d4);
  getTileCollisionsAtPosition_hook(gb);
}

void bombchuUpdateVelocityAndClimbing_sidescroll_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52dc, 0x52df); A = W8(wFrameCounter);
  CYC(0x52df, 0x52e1); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(0x52e1, bombchuUpdateAngle_sidescrolling_hook, 0x539e, 0x52e4);
  else CYC(0x52e1, 0x52e4);
  bombchuCheckWallsAndApplySpeed_hook(gb);
}

static void bombchu_update_wall_climbing(GB *gb, uint16_t sp0_) {
  CYC(0x52ea, 0x52ec); E = 0x32;
  CYC(0x52ec, 0x52ed); A = mem_rd(gb, DE);
  CYC(0x52ed, 0x52ee); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x52ee, 0x52f0);
    goto climbing;
  }
  CYC(0x52ee, 0x52f0);
  CYC(0x52f0, 0x52f2); E = 0x09;
  CALL_C(0x52f2, bombchuGetTileCollisions_hook, 0x52bd, 0x52f5);
  if (F & FZ) { CYCT(0x52f5, 0x52f6); ret_effect(gb); return; }
  CYC(0x52f5, 0x52f6);
  CYC(0x52f6, 0x52f7); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x52f7, 0x52f9);
    goto start_climbing;
  }
  CYC(0x52f7, 0x52f9);
  CYC(0x52f9, 0x52fb); E = 0x09;
  CYC(0x52fb, 0x52fc); A = mem_rd(gb, DE);
  CYC(0x52fc, 0x52fe); alu_xor(gb, 0x10);
  CYC(0x52fe, 0x52ff); mem_wr(gb, DE, A);
  CYC(0x52ff, 0x5302);
  bombchuSetAnimationFromAngle_hook(gb);
  return;

start_climbing:
  CYC(0x5302, 0x5303); H = D;
  CYC(0x5303, 0x5305); L = 0x09;
  CYC(0x5305, 0x5306); A = mem_rd(gb, HL);
  CYC(0x5306, 0x5308); mem_wr(gb, HL, 0x00);
  CYC(0x5308, 0x530a); L = 0x33;
  CYC(0x530a, 0x530b); mem_wr(gb, HL, A);
  CYC(0x530b, 0x530d); L = 0x32;
  CYC(0x530d, 0x530f); mem_wr(gb, HL, 0x01);
  CYC(0x530f, 0x5312);
  bombchuSetAnimationFromAngle_hook(gb);
  return;

climbing:
  CYC(0x5312, 0x5314); E = 0x33;
  CALL_C(0x5314, bombchuGetTileCollisions_hook, 0x52bd, 0x5317);
  if (!(F & FZ)) {
    CYCT(0x5317, 0x5319);
    goto touching_wall;
  }
  CYC(0x5317, 0x5319);
  CYC(0x5319, 0x531a); H = D;
  CYC(0x531a, 0x531c); L = 0x09;
  CYC(0x531c, 0x531e); E = 0x33;
  CYC(0x531e, 0x531f); A = mem_rd(gb, DE);
  CYC(0x531f, 0x5320); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5320, 0x5322);
  } else {
    CYC(0x5320, 0x5322);
    CYC(0x5322, 0x5323); A = mem_rd(gb, HL);
    CYC(0x5323, 0x5325); E = 0x33;
    CYC(0x5325, 0x5326); mem_wr(gb, DE, A);
  }
  CYC(0x5326, 0x5327); A = mem_rd(gb, DE);
  CYC(0x5327, 0x5328); mem_wr(gb, HL, A);
  CYC(0x5328, 0x532a); L = 0x32;
  CYC(0x532a, 0x532b); alu_xor(gb, A);
  CYC(0x532b, 0x532c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x532c, 0x532d); L = alu_inc8(gb, L);
  CYC(0x532d, 0x532e); mem_wr(gb, HL, A);
  CYC(0x532e, 0x5330); L = 0x14;
  CYC(0x5330, 0x5331); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5331, 0x5332); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5332, 0x5334); L = 0x08;
  CYC(0x5334, 0x5336); mem_wr(gb, HL, 0xff);
  CYC(0x5336, 0x5339);
  bombchuSetAnimationFromAngle_hook(gb);
  return;

touching_wall:
  CYC(0x5339, 0x533b); E = 0x09;
  CALL_C(0x533b, bombchuGetTileCollisions_hook, 0x52bd, 0x533e);
  if (F & FZ) { CYCT(0x533e, 0x533f); ret_effect(gb); return; }
  CYC(0x533e, 0x533f);
  CYC(0x533f, 0x5340); H = D;
  CYC(0x5340, 0x5342); L = 0x09;
  CYC(0x5342, 0x5343); B = mem_rd(gb, HL);
  CYC(0x5343, 0x5345); E = 0x33;
  CYC(0x5345, 0x5346); A = mem_rd(gb, DE);
  CYC(0x5346, 0x5348); alu_xor(gb, 0x10);
  CYC(0x5348, 0x5349); mem_wr(gb, HL, A);
  CYC(0x5349, 0x534b); alu_bit(gb, 3, A);
  if (F & FZ) {
    CYCT(0x534b, 0x534d);
  } else {
    CYC(0x534b, 0x534d);
    CYC(0x534d, 0x534f); alu_bit(gb, 3, B);
    if (F & FZ) {
      CYCT(0x534f, 0x5351);
    } else {
      CYC(0x534f, 0x5351);
      CYC(0x5351, 0x5353); L = 0x32;
      CYC(0x5353, 0x5355); mem_wr(gb, HL, 0x00);
    }
  }
  CYC(0x5355, 0x5356); A = B;
  CYC(0x5356, 0x5357); mem_wr(gb, DE, A);
  CYC(0x5357, 0x5358); alu_or(gb, A);
  CYC(0x5358, 0x535a); L = 0x34;
  CYC(0x535a, 0x535c); mem_wr(gb, HL, 0x00);
  if (!(F & FZ)) {
    CYCT(0x535c, 0x535e);
    bombchuSetAnimationFromAngle_hook(gb);
    return;
  }
  CYC(0x535c, 0x535e);
  CYC(0x535e, 0x535f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x535f, 0x5361);
  bombchuSetAnimationFromAngle_hook(gb);
}

void bombchuCheckWallsAndApplySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52e4, 0x52e7); push_effect(gb, 0x52e7);
  bombchu_update_wall_climbing(gb, sp0_);
  CYC(0x52e7, 0x52ea);
  objectApplySpeed_hook(gb);
}

void bombchuUpdateAngle_topDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5361, 0x5363); A = 0x0b;
  CALL_C(0x5363, objectGetRelatedObject2Var_hook, 0x2164, 0x5366);
  CYC(0x5366, 0x5367); B = mem_rd(gb, HL);
  CYC(0x5367, 0x5369); L = 0x8d;
  CYC(0x5369, 0x536a); C = mem_rd(gb, HL);
  CALL_C(0x536a, objectGetRelativeAngle_hook, 0x1ea4, 0x536d);
  CYC(0x536d, 0x536e); B = A;
  CYC(0x536e, 0x5370); alu_add(gb, 0x04);
  CYC(0x5370, 0x5372); alu_and(gb, 0x18);
  CYC(0x5372, 0x5374); E = 0x09;
  CYC(0x5374, 0x5375); mem_wr(gb, DE, A);
  CYC(0x5375, 0x5376); alu_sub(gb, B);
  CYC(0x5376, 0x5378); alu_and(gb, 0x1f);
  CYC(0x5378, 0x537a); alu_cp(gb, 0x10);
  CYC(0x537a, 0x537c); A = 0x08;
  if (!(F & FC)) {
    CYCT(0x537c, 0x537e);
  } else {
    CYC(0x537c, 0x537e);
    CYC(0x537e, 0x5380); A = 0xf8;
  }
  CYC(0x5380, 0x5382); E = 0x31;
  CYC(0x5382, 0x5383); mem_wr(gb, DE, A);
  bombchuSetAnimationFromAngle_hook(gb);
}

void bombchuSetAnimationFromAngle_hook(GB *gb) {
  CYC(0x5383, 0x5384); H = D;
  CYC(0x5384, 0x5386); L = 0x08;
  CYC(0x5386, 0x5388); E = 0x09;
  CYC(0x5388, 0x5389); A = mem_rd(gb, DE);
  CYC(0x5389, 0x538a); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x538a, 0x538b); ret_effect(gb); return; }
  CYC(0x538a, 0x538b);
  CYC(0x538b, 0x538c); mem_wr(gb, HL, A);
  CYC(0x538c, 0x538e); A = alu_swap(gb, A);
  CYC(0x538e, 0x538f); alu_rlca(gb);
  CYC(0x538f, 0x5391); L = 0x34;
  CYC(0x5391, 0x5393); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5393, 0x5395);
  } else {
    CYC(0x5393, 0x5395);
    CYC(0x5395, 0x5396); A = alu_dec8(gb, A);
    CYC(0x5396, 0x5398); A = 0x04;
    if (F & FZ) {
      CYCT(0x5398, 0x539a);
    } else {
      CYC(0x5398, 0x539a);
      CYC(0x539a, 0x539b); A = alu_inc8(gb, A);
    }
  }
  CYC(0x539b, 0x539e);
  itemSetAnimation_hook(gb);
}

void bombchuUpdateAngle_sidescrolling_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x539e, 0x53a0); A = 0x0b;
  CALL_C(0x53a0, objectGetRelatedObject2Var_hook, 0x2164, 0x53a3);
  CYC(0x53a3, 0x53a4); B = mem_rd(gb, HL);
  CYC(0x53a4, 0x53a6); L = 0x8d;
  CYC(0x53a6, 0x53a7); C = mem_rd(gb, HL);
  CALL_C(0x53a7, objectGetRelativeAngle_hook, 0x1ea4, 0x53aa);
  CYC(0x53aa, 0x53ab); B = A;
  CYC(0x53ab, 0x53ad); E = 0x09;
  CYC(0x53ad, 0x53ae); A = mem_rd(gb, DE);
  CYC(0x53ae, 0x53b0); alu_bit(gb, 3, A);
  CYC(0x53b0, 0x53b1); A = B;
  if (!(F & FZ)) {
    CYCT(0x53b1, 0x53b3);
    goto horizontal;
  }
  CYC(0x53b1, 0x53b3);
  CYC(0x53b3, 0x53b5); alu_sub(gb, 0x08);
  CYC(0x53b5, 0x53b7); alu_and(gb, 0x1f);
  CYC(0x53b7, 0x53b9); alu_cp(gb, 0x10);
  CYC(0x53b9, 0x53bb); A = 0x00;
  if (F & FC) {
    CYCT(0x53bb, 0x53bd);
    goto set_angle;
  }
  CYC(0x53bb, 0x53bd);
  CYC(0x53bd, 0x53bf); A = 0x10;
  CYC(0x53bf, 0x53c1);
  goto set_angle;

horizontal:
  CYC(0x53c1, 0x53c3); alu_cp(gb, 0x10);
  CYC(0x53c3, 0x53c5); A = 0x08;
  if (F & FC) {
    CYCT(0x53c5, 0x53c7);
  } else {
    CYC(0x53c5, 0x53c7);
    CYC(0x53c7, 0x53c9); A = 0x18;
  }

set_angle:
  CYC(0x53c9, 0x53cb); E = 0x09;
  CYC(0x53cb, 0x53cc); mem_wr(gb, DE, A);
  CYC(0x53cc, 0x53ce);
  bombchuSetAnimationFromAngle_hook(gb);
}

void bombchuSetPositionInFrontOfLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53ce, 0x53d1); SET_HL(w1Link_yh);
  CYC(0x53d1, 0x53d2); B = mem_rd(gb, HL);
  CYC(0x53d2, 0x53d4); L = (uint8_t)w1Link_xh;
  CYC(0x53d4, 0x53d5); C = mem_rd(gb, HL);
  CYC(0x53d5, 0x53d8); A = W8(wActiveGroup);
  CYC(0x53d8, 0x53da); alu_cp(gb, 0x06);
  CYC(0x53da, 0x53dc); L = (uint8_t)w1Link_direction;
  CYC(0x53dc, 0x53dd); A = mem_rd(gb, HL);
  CYC(0x53dd, 0x53e0); SET_HL(0x53ff);
  if (F & FC) {
    CYCT(0x53e0, 0x53e2);
  } else {
    CYC(0x53e0, 0x53e2);
    CYC(0x53e2, 0x53e5); SET_HL(0x5407);
  }
  CYC(0x53e5, 0x53e6); bombchu_add_double_index(gb, 0x53e6);
  CYC(0x53e6, 0x53e8); E = 0x0b;
  CYC(0x53e8, 0x53e9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x53e9, 0x53ea); alu_add(gb, B);
  CYC(0x53ea, 0x53eb); mem_wr(gb, DE, A);
  CYC(0x53eb, 0x53ed); E = 0x0d;
  CYC(0x53ed, 0x53ee); A = mem_rd(gb, HL);
  CYC(0x53ee, 0x53ef); alu_add(gb, C);
  CYC(0x53ef, 0x53f0); mem_wr(gb, DE, A);
  CYC(0x53f0, 0x53f1); push_effect(gb, BC);
  CALL_C(0x53f1, objectGetTileCollisions_hook, 0x14ad, 0x53f4);
  CYC(0x53f4, 0x53f5); SET_BC(pop_effect(gb));
  CYC(0x53f5, 0x53f7); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x53f7, 0x53f8); ret_effect(gb); return; }
  CYC(0x53f7, 0x53f8);
  CYC(0x53f8, 0x53f9); A = C;
  CYC(0x53f9, 0x53fa); mem_wr(gb, DE, A);
  CYC(0x53fa, 0x53fc); E = 0x0b;
  CYC(0x53fc, 0x53fd); A = B;
  CYC(0x53fd, 0x53fe); mem_wr(gb, DE, A);
  CYC(0x53fe, 0x53ff); ret_effect(gb);
}

void bombchuCountdownToExplosion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x540f, itemDecCounter2_hook, 0x23db, 0x5412);
  if (!(F & FZ)) { CYCT(0x5412, 0x5413); ret_effect(gb); return; }
  CYC(0x5412, 0x5413);
  bombchuClearCounter2AndInitializeExplosion_hook(gb);
}

void bombchuClearCounter2AndInitializeExplosion_hook(GB *gb) {
  CYC(0x5413, 0x5415); E = 0x07;
  CYC(0x5415, 0x5416); alu_xor(gb, A);
  CYC(0x5416, 0x5417); mem_wr(gb, DE, A);
  CYC(0x5417, 0x541a);
  itemInitializeBombExplosion_hook(gb);
}

void bombchuCheckCollidedWithTarget_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x541a, 0x541c); A = 0x29;
  CALL_C(0x541c, objectGetRelatedObject2Var_hook, 0x2164, 0x541f);
  CYC(0x541f, 0x5420); A = mem_rd(gb, HL);
  CYC(0x5420, 0x5421); alu_or(gb, A);
  CYC(0x5421, 0x5422); alu_scf(gb);
  if (F & FZ) { CYCT(0x5422, 0x5423); ret_effect(gb); return; }
  CYC(0x5422, 0x5423);
  CYC(0x5423, 0x5426);
  checkObjectsCollided_hook(gb);
}

static void bombchu_increase_vision_radius(GB *gb) {
  CYC(0x5481, 0x5483); E = 0x26;
  CYC(0x5483, 0x5484); A = mem_rd(gb, DE);
  CYC(0x5484, 0x5486); alu_add(gb, 0x10);
  CYC(0x5486, 0x5488); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(0x5488, 0x548a);
  } else {
    CYC(0x5488, 0x548a);
    CYC(0x548a, 0x548c); A = 0x18;
  }
  CYC(0x548c, 0x548d); mem_wr(gb, DE, A);
  CYC(0x548d, 0x548e); E = alu_inc8(gb, E);
  CYC(0x548e, 0x548f); mem_wr(gb, DE, A);
  CYC(0x548f, 0x5490); ret_effect(gb);
}

void bombchuCheckForEnemyTarget_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5426, 0x5428); E = 0x30;
  CYC(0x5428, 0x5429); A = mem_rd(gb, DE);
  CYC(0x5429, 0x542a); H = A;
  CYC(0x542a, 0x542c); L = 0x80;
  CYC(0x542c, 0x542d); A = mem_rd(gb, HL);
  CYC(0x542d, 0x542e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x542e, 0x5430); goto next_target; }
  CYC(0x542e, 0x5430);
  CYC(0x5430, 0x5432); L = 0x9a;
  CYC(0x5432, 0x5434); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5434, 0x5436); goto next_target; }
  CYC(0x5434, 0x5436);
  CYC(0x5436, 0x5438); L = 0x81;
  CYC(0x5438, 0x5439); A = mem_rd(gb, HL);
  CYC(0x5439, 0x543a); push_effect(gb, HL);
  CYC(0x543a, 0x543d); SET_HL(0x5490);
  CALL_C(0x543d, checkFlag_hook, 0x0205, 0x5440);
  CYC(0x5440, 0x5441); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(0x5441, 0x5443); goto next_target; }
  CYC(0x5441, 0x5443);
  CALL_C(0x5443, checkObjectsCollided_hook, 0x1d5a, 0x5446);
  if (!(F & FC)) { CYCT(0x5446, 0x5448); goto next_target; }
  CYC(0x5446, 0x5448);
  CYC(0x5448, 0x5449); A = H;
  CYC(0x5449, 0x544a); H = D;
  CYC(0x544a, 0x544c); L = 0x19;
  CYC(0x544c, 0x544d); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x544d, 0x544f); mem_wr(gb, HL, 0x80);
  CYC(0x544f, 0x5451); L = 0x26;
  CYC(0x5451, 0x5453); A = 0x06;
  CYC(0x5453, 0x5454); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5454, 0x5455); mem_wr(gb, HL, A);
  CYC(0x5455, 0x5457); L = 0x06;
  CYC(0x5457, 0x5459); mem_wr(gb, HL, 0x0c);
  CYC(0x5459, 0x545b); L = 0x11;
  CYC(0x545b, 0x545d); mem_wr(gb, HL, 0x46);
  CYC(0x545d, 0x545f); L = 0x04;
  CYC(0x545f, 0x5460); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5460, 0x5463); A = W8(wTilesetFlags);
  CYC(0x5463, 0x5465); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x5465, 0x5467);
    CALL_C(0x546c, bombchuUpdateAngle_sidescrolling_hook, 0x539e, 0x546f);
    CYC(0x546f, 0x5470); alu_xor(gb, A);
    CYC(0x5470, 0x5471); ret_effect(gb);
    return;
  }
  CYC(0x5465, 0x5467);
  CALL_C(0x5467, bombchuUpdateAngle_topDown_hook, 0x5361, 0x546a);
  CYC(0x546a, 0x546b); alu_xor(gb, A);
  CYC(0x546b, 0x546c); ret_effect(gb);
  return;

next_target:
  CYC(0x5471, 0x5472); H = alu_inc8(gb, H);
  CYC(0x5472, 0x5473); A = H;
  CYC(0x5473, 0x5475); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(0x5475, 0x5477);
  } else {
    CYC(0x5475, 0x5477);
    CYC(0x5477, 0x547a); push_effect(gb, 0x547a);
    bombchu_increase_vision_radius(gb);
    CYC(0x547a, 0x547c); A = 0xd0;
  }
  CYC(0x547c, 0x547e); E = 0x30;
  CYC(0x547e, 0x547f); mem_wr(gb, DE, A);
  CYC(0x547f, 0x5480); alu_or(gb, D);
  CYC(0x5480, 0x5481); ret_effect(gb);
}

static void bombchu_initialize(GB *gb, uint16_t sp0_) {
  CALL_C(0x51dc, itemLoadAttributesAndGraphics_hook, 0x4993, 0x51df);
  CALL_C(0x51df, decNumBombchus_hook, 0x17c3, 0x51e2);
  CYC(0x51e2, 0x51e3); H = D;
  CYC(0x51e3, 0x51e5); L = 0x04;
  CYC(0x51e5, 0x51e6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x51e6, 0x51e8); L = 0x30;
  CYC(0x51e8, 0x51ea); mem_wr(gb, HL, 0xd0);
  CYC(0x51ea, 0x51ec); L = 0x11;
  CYC(0x51ec, 0x51ee); mem_wr(gb, HL, 0x14);
  CYC(0x51ee, 0x51f0); L = 0x06;
  CYC(0x51f0, 0x51f2); mem_wr(gb, HL, 0x10);
  CYC(0x51f2, 0x51f3); L = alu_inc8(gb, L);
  CYC(0x51f3, 0x51f5); mem_wr(gb, HL, 0xb4);
  CYC(0x51f5, 0x51f7); L = 0x26;
  CYC(0x51f7, 0x51f9); A = 0x18;
  CYC(0x51f9, 0x51fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x51fa, 0x51fb); mem_wr(gb, HL, A);
  CYC(0x51fb, 0x51fd); L = 0x31;
  CYC(0x51fd, 0x51ff); mem_wr(gb, HL, 0x08);
  CYC(0x51ff, 0x5201); L = 0x09;
  CYC(0x5201, 0x5204); A = W8(w1Link_direction);
  CYC(0x5204, 0x5206); A = alu_swap(gb, A);
  CYC(0x5206, 0x5207); alu_rrca(gb);
  CYC(0x5207, 0x5208); mem_wr(gb, HL, A);
  CYC(0x5208, 0x520a); L = 0x08;
  CYC(0x520a, 0x520c); mem_wr(gb, HL, 0xff);
  CALL_C(0x520c, bombchuSetAnimationFromAngle_hook, 0x5383, 0x520f);
  CYC(0x520f, 0x5212); bombchuSetPositionInFrontOfLink_hook(gb);
}

void itemCode0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5194, bombchuCountdownToExplosion_hook, 0x540f, 0x5197);
  CYC(0x5197, 0x5199); E = 0x04;
  CYC(0x5199, 0x519a); A = mem_rd(gb, DE);
  CYC(0x519a, 0x519c); alu_cp(gb, 0xff);
  if (!(F & FC)) {
    CYCT(0x519c, 0x519f); itemUpdateExplosion_hook(gb); return;
  }
  CYC(0x519c, 0x519f);
  CALL_C(0x519f, objectCheckWithinRoomBoundary_hook, 0x219f, 0x51a2);
  if (!(F & FC)) {
    CYCT(0x51a2, 0x51a5); itemDelete_hook(gb); return;
  }
  CYC(0x51a2, 0x51a5);
  CALL_C(0x51a5, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x51a8);
  CYC(0x51a8, 0x51ab); A = W8(wTilesetFlags);
  CYC(0x51ab, 0x51ad); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x51ad, 0x51af);
    goto sidescroll;
  }
  CYC(0x51ad, 0x51af);
  CYC(0x51af, 0x51b1); C = 0x20;
  CALL_C(0x51b1, itemUpdateSpeedZAndCheckHazards_hook, 0x4a62, 0x51b4);
  CYC(0x51b4, 0x51b6); E = 0x04;
  CYC(0x51b6, 0x51b7); A = mem_rd(gb, DE);
  CYC(0x51b7, 0x51b8); push_effect(gb, 0x51b8);
  switch (bombchu_jump_table(gb)) {
    case 0x51dc: bombchu_initialize(gb, sp0_); return;
    case 0x5212: goto td_state1;
    case 0x521b: goto td_state2;
    case 0x5228: goto td_state3;
    case 0x5233: goto td_state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

sidescroll:
  CYC(0x51c2, 0x51c4); E = 0x32;
  CYC(0x51c4, 0x51c5); A = mem_rd(gb, DE);
  CYC(0x51c5, 0x51c6); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x51c6, 0x51c8);
  } else {
    CYC(0x51c6, 0x51c8);
    CYC(0x51c8, 0x51ca); C = 0x18;
    CALL_C(0x51ca, itemUpdateThrowingVerticallyAndCheckHazards_hook, 0x4b3c, 0x51cd);
    if (F & FC) {
      CYCT(0x51cd, 0x51d0); itemDelete_hook(gb); return;
    }
    CYC(0x51cd, 0x51d0);
  }
  CYC(0x51d0, 0x51d2); E = 0x04;
  CYC(0x51d2, 0x51d3); A = mem_rd(gb, DE);
  CYC(0x51d3, 0x51d4); push_effect(gb, 0x51d4);
  switch (bombchu_jump_table(gb)) {
    case 0x5241: goto ss_state0;
    case 0x5250: goto ss_state1;
    case 0x525f: goto ss_state2;
    case 0x5267: goto ss_state3;
    default: hook_continue(gb, HL, sp0_); return;
  }

td_state1:
  CYC(0x5212, 0x5213); H = D;
  CYC(0x5213, 0x5215); L = 0x0f;
  CYC(0x5215, 0x5217); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x5217, 0x5219);
    goto update_top_down;
  }
  CYC(0x5217, 0x5219);
  CYC(0x5219, 0x521a); L = E;
  CYC(0x521a, 0x521b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

td_state2:
  CALL_C(0x521b, bombchuCheckForEnemyTarget_hook, 0x5426, 0x521e);
  if (F & FZ) {
    CYCT(0x521e, 0x521f); ret_effect(gb); return;
  }
  CYC(0x521e, 0x521f);

update_top_down:
  CALL_C(0x521f, bombchuUpdateSpeed_hook, 0x527a, 0x5222);
  CALL_C(0x5222, itemUpdateConveyorBelt_hook, 0x4bf7, 0x5225);

animate_top_down:
  CYC(0x5225, 0x5228); itemAnimate_hook(gb);
  return;

td_state3:
  CYC(0x5228, 0x5229); H = D;
  CYC(0x5229, 0x522b); L = 0x06;
  CYC(0x522b, 0x522c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x522c, 0x522f); itemUpdateConveyorBelt_hook(gb); return;
  }
  CYC(0x522c, 0x522f);
  CYC(0x522f, 0x5231); mem_wr(gb, HL, 0x0a);
  CYC(0x5231, 0x5232); L = E;
  CYC(0x5232, 0x5233); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

td_state4:
  CALL_C(0x5233, bombchuCheckCollidedWithTarget_hook, 0x541a, 0x5236);
  if (F & FC) {
    CYCT(0x5236, 0x5239); bombchuClearCounter2AndInitializeExplosion_hook(gb); return;
  }
  CYC(0x5236, 0x5239);
  CALL_C(0x5239, bombchuUpdateVelocity_hook, 0x5272, 0x523c);
  CALL_C(0x523c, itemUpdateConveyorBelt_hook, 0x4bf7, 0x523f);
  CYC(0x523f, 0x5241);
  goto animate_top_down;

ss_state0:
  CYC(0x5241, 0x5244); push_effect(gb, 0x5244); bombchu_initialize(gb, sp0_);
  CYC(0x5244, 0x5246); E = 0x09;
  CYC(0x5246, 0x5247); A = mem_rd(gb, DE);
  CYC(0x5247, 0x5249); alu_bit(gb, 3, A);
  if (!(F & FZ)) {
    CYCT(0x5249, 0x524a); ret_effect(gb); return;
  }
  CYC(0x5249, 0x524a);
  CYC(0x524a, 0x524c); alu_add(gb, 0x08);
  CYC(0x524c, 0x524d); mem_wr(gb, DE, A);
  CYC(0x524d, 0x5250); bombchuSetAnimationFromAngle_hook(gb);
  return;

ss_state1:
  CYC(0x5250, 0x5252); E = 0x10;
  CYC(0x5252, 0x5254); A = 0x14;
  CYC(0x5254, 0x5255); mem_wr(gb, DE, A);
  CALL_C(0x5255, bombchuCheckForEnemyTarget_hook, 0x5426, 0x5258);
  if (F & FZ) {
    CYCT(0x5258, 0x5259); ret_effect(gb); return;
  }
  CYC(0x5258, 0x5259);
  CALL_C(0x5259, bombchuCheckWallsAndApplySpeed_hook, 0x52e4, 0x525c);

animate_sidescroll:
  CYC(0x525c, 0x525f); itemAnimate_hook(gb);
  return;

ss_state2:
  CALL_C(0x525f, itemDecCounter1_hook, 0x23d6, 0x5262);
  if (!(F & FZ)) {
    CYCT(0x5262, 0x5263); ret_effect(gb); return;
  }
  CYC(0x5262, 0x5263);
  CYC(0x5263, 0x5265); mem_wr(gb, HL, 0x0a);
  CYC(0x5265, 0x5266); L = E;
  CYC(0x5266, 0x5267); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

ss_state3:
  CALL_C(0x5267, bombchuCheckCollidedWithTarget_hook, 0x541a, 0x526a);
  if (F & FC) {
    CYCT(0x526a, 0x526d); bombchuClearCounter2AndInitializeExplosion_hook(gb); return;
  }
  CYC(0x526a, 0x526d);
  CALL_C(0x526d, bombchuUpdateVelocityAndClimbing_sidescroll_hook, 0x52dc, 0x5270);
  CYC(0x5270, 0x5272);
  goto animate_sidescroll;
}
