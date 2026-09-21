#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(bombchuUpdateVelocity);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+5, bombchuUpdateAngle_topDown_hook, SYM(bombchuUpdateAngle_topDown), SYM(bombchuUpdateSpeed));
  else CYC(b_+5, b_+8);
  TAIL(bombchuUpdateSpeed);
}

static void bombchu_update_speed(GB *gb, uint16_t sp0_) {
  BASE(bombchuUpdateSpeed);
  CYC(b_+11, b_+13); E = 0x09;
  CALL_C(b_+13, bombchuGetTileCollisions_hook, SYM(bombchuGetTileCollisions), b_+16);
  CYC(b_+16, b_+18); alu_cp(gb, 0x10);
  if (F & FZ) {
    CYCT(b_+18, b_+20);
    goto impassable;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_cp(gb, 0x15);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
    goto impassable;
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+25, b_+27);
    goto impassable;
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); A = alu_dec8(gb, A);
  CYC(b_+28, b_+30); E = 0x11;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
  } else {
    CYC(b_+31, b_+33);
    CYC(b_+33, b_+34); E = A;
    CYC(b_+34, b_+37); SET_HL(SYM(bounceSpeedReductionMapping));
    CALL_C(b_+37, lookupKey_hook, SYM(lookupKey), b_+40);
  }
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = 0x10;
  CYC(b_+43, b_+44); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+45, b_+46); ret_effect(gb); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); L = 0x14;
  CYC(b_+48, b_+50); A = 0x80;
  CYC(b_+50, b_+51); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0xff);
  CYC(b_+53, b_+54); ret_effect(gb);
  return;

impassable:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = 0x31;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+60); L = 0x09;
  CYC(b_+60, b_+61); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+61, b_+63); alu_and(gb, 0x18);
  CYC(b_+63, b_+64); mem_wr(gb, HL, A);
  CYC(b_+64, b_+67);
  bombchuSetAnimationFromAngle_hook(gb);
}

void bombchuUpdateSpeed_hook(GB *gb) {
  BASE(bombchuUpdateSpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  bombchu_update_speed(gb, sp0_);
  CYC(b_+3, b_+5); C = 0x18;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  CYC(b_+8, b_+11);
  TAIL(objectApplySpeed);
}

void bombchuGetTileCollisions_hook(GB *gb) {
  BASE(bombchuGetTileCollisions);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x0b;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = 0x0d;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_rrca(gb);
  CYC(b_+9, b_+10); alu_rrca(gb);
  CYC(b_+10, b_+13); SET_HL(b_+23);
  CYC(b_+13, b_+14); bombchu_add_a_to_hl(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); alu_add(gb, C);
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+23);
  TAIL(getTileCollisionsAtPosition);
}

void bombchuUpdateVelocityAndClimbing_sidescroll_hook(GB *gb) {
  BASE(bombchuUpdateVelocityAndClimbing_sidescroll);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (F & FZ) CALL_C_CC(b_+5, bombchuUpdateAngle_sidescrolling_hook, SYM(bombchuUpdateAngle_sidescrolling), SYM(bombchuCheckWallsAndApplySpeed));
  else CYC(b_+5, b_+8);
  TAIL(bombchuCheckWallsAndApplySpeed);
}

static void bombchu_update_wall_climbing(GB *gb, uint16_t sp0_) {
  BASE(bombchuCheckWallsAndApplySpeed);
  CYC(b_+6, b_+8); E = 0x32;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
    goto climbing;
  }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = 0x09;
  CALL_C(b_+14, bombchuGetTileCollisions_hook, SYM(bombchuGetTileCollisions), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto start_climbing;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); E = 0x09;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30);
  TAIL(bombchuSetAnimationFromAngle);

start_climbing:
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = 0x09;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  CYC(b_+36, b_+38); L = 0x33;
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+41); L = 0x32;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x01);
  CYC(b_+43, b_+46);
  TAIL(bombchuSetAnimationFromAngle);

climbing:
  CYC(b_+46, b_+48); E = 0x33;
  CALL_C(b_+48, bombchuGetTileCollisions_hook, SYM(bombchuGetTileCollisions), b_+51);
  if (!(F & FZ)) {
    CYCT(b_+51, b_+53);
    goto touching_wall;
  }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+56); L = 0x09;
  CYC(b_+56, b_+58); E = 0x33;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+60, b_+62);
  } else {
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+63); A = mem_rd(gb, HL);
    CYC(b_+63, b_+65); E = 0x33;
    CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  }
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); mem_wr(gb, HL, A);
  CYC(b_+68, b_+70); L = 0x32;
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+72, b_+73); L = alu_inc8(gb, L);
  CYC(b_+73, b_+74); mem_wr(gb, HL, A);
  CYC(b_+74, b_+76); L = 0x14;
  CYC(b_+76, b_+77); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+78, b_+80); L = 0x08;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0xff);
  CYC(b_+82, b_+85);
  TAIL(bombchuSetAnimationFromAngle);

touching_wall:
  CYC(b_+85, b_+87); E = 0x09;
  CALL_C(b_+87, bombchuGetTileCollisions_hook, SYM(bombchuGetTileCollisions), b_+90);
  if (F & FZ) { CYCT(b_+90, b_+91); ret_effect(gb); return; }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+92); H = D;
  CYC(b_+92, b_+94); L = 0x09;
  CYC(b_+94, b_+95); B = mem_rd(gb, HL);
  CYC(b_+95, b_+97); E = 0x33;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+100); alu_xor(gb, 0x10);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  CYC(b_+101, b_+103); alu_bit(gb, 3, A);
  if (F & FZ) {
    CYCT(b_+103, b_+105);
  } else {
    CYC(b_+103, b_+105);
    CYC(b_+105, b_+107); alu_bit(gb, 3, B);
    if (F & FZ) {
      CYCT(b_+107, b_+109);
    } else {
      CYC(b_+107, b_+109);
      CYC(b_+109, b_+111); L = 0x32;
      CYC(b_+111, b_+113); mem_wr(gb, HL, 0x00);
    }
  }
  CYC(b_+113, b_+114); A = B;
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);
  CYC(b_+115, b_+116); alu_or(gb, A);
  CYC(b_+116, b_+118); L = 0x34;
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0x00);
  if (!(F & FZ)) {
    CYCT(b_+120, b_+122);
    TAIL(bombchuSetAnimationFromAngle);
  }
  CYC(b_+120, b_+122);
  CYC(b_+122, b_+123); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+123, b_+125);
  bombchuSetAnimationFromAngle_hook(gb);
}

void bombchuCheckWallsAndApplySpeed_hook(GB *gb) {
  BASE(bombchuCheckWallsAndApplySpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  bombchu_update_wall_climbing(gb, sp0_);
  CYC(b_+3, b_+6);
  TAIL(objectApplySpeed);
}

void bombchuUpdateAngle_topDown_hook(GB *gb) {
  BASE(bombchuUpdateAngle_topDown);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = 0x8d;
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  CALL_C(b_+9, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+12);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+15); alu_add(gb, 0x04);
  CYC(b_+15, b_+17); alu_and(gb, 0x18);
  CYC(b_+17, b_+19); E = 0x09;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); alu_sub(gb, B);
  CYC(b_+21, b_+23); alu_and(gb, 0x1f);
  CYC(b_+23, b_+25); alu_cp(gb, 0x10);
  CYC(b_+25, b_+27); A = 0x08;
  if (!(F & FC)) {
    CYCT(b_+27, b_+29);
  } else {
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+31); A = 0xf8;
  }
  CYC(b_+31, b_+33); E = 0x31;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  TAIL(bombchuSetAnimationFromAngle);
}

void bombchuSetAnimationFromAngle_hook(GB *gb) {
  BASE(bombchuSetAnimationFromAngle);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x08;
  CYC(b_+3, b_+5); E = 0x09;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rlca(gb);
  CYC(b_+12, b_+14); L = 0x34;
  CYC(b_+14, b_+16); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+16, b_+18);
  } else {
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+19); A = alu_dec8(gb, A);
    CYC(b_+19, b_+21); A = 0x04;
    if (F & FZ) {
      CYCT(b_+21, b_+23);
    } else {
      CYC(b_+21, b_+23);
      CYC(b_+23, b_+24); A = alu_inc8(gb, A);
    }
  }
  CYC(b_+24, b_+27);
  TAIL(itemSetAnimation);
}

void bombchuUpdateAngle_sidescrolling_hook(GB *gb) {
  BASE(bombchuUpdateAngle_sidescrolling);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0b;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = 0x8d;
  CYC(b_+8, b_+9); C = mem_rd(gb, HL);
  CALL_C(b_+9, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+12);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+15); E = 0x09;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_bit(gb, 3, A);
  CYC(b_+18, b_+19); A = B;
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto horizontal;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); alu_sub(gb, 0x08);
  CYC(b_+23, b_+25); alu_and(gb, 0x1f);
  CYC(b_+25, b_+27); alu_cp(gb, 0x10);
  CYC(b_+27, b_+29); A = 0x00;
  if (F & FC) {
    CYCT(b_+29, b_+31);
    goto set_angle;
  }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); A = 0x10;
  CYC(b_+33, b_+35);
  goto set_angle;

horizontal:
  CYC(b_+35, b_+37); alu_cp(gb, 0x10);
  CYC(b_+37, b_+39); A = 0x08;
  if (F & FC) {
    CYCT(b_+39, b_+41);
  } else {
    CYC(b_+39, b_+41);
    CYC(b_+41, b_+43); A = 0x18;
  }

set_angle:
  CYC(b_+43, b_+45); E = 0x09;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48);
  TAIL(bombchuSetAnimationFromAngle);
}

void bombchuSetPositionInFrontOfLink_hook(GB *gb) {
  BASE(bombchuSetPositionInFrontOfLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = (uint8_t)w1Link_xh;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+10); A = W8(wActiveGroup);
  CYC(b_+10, b_+12); alu_cp(gb, 0x06);
  CYC(b_+12, b_+14); L = (uint8_t)w1Link_direction;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+18); SET_HL(b_+49);
  if (F & FC) {
    CYCT(b_+18, b_+20);
  } else {
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+23); SET_HL(b_+57);
  }
  CYC(b_+23, b_+24); bombchu_add_double_index(gb, b_+24);
  CYC(b_+24, b_+26); E = 0x0b;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); alu_add(gb, B);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = 0x0d;
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+33); alu_add(gb, C);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); push_effect(gb, BC);
  CALL_C(b_+35, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+38);
  CYC(b_+38, b_+39); SET_BC(pop_effect(gb));
  CYC(b_+39, b_+41); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+43); A = C;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+46); E = 0x0b;
  CYC(b_+46, b_+47); A = B;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); ret_effect(gb);
}

void bombchuCountdownToExplosion_hook(GB *gb) {
  BASE(bombchuCountdownToExplosion);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemDecCounter2_hook, SYM(itemDecCounter2), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  TAIL(bombchuClearCounter2AndInitializeExplosion);
}

void bombchuClearCounter2AndInitializeExplosion_hook(GB *gb) {
  BASE(bombchuClearCounter2AndInitializeExplosion);
  CYC(b_+0, b_+2); E = 0x07;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+7);
  TAIL(itemInitializeBombExplosion);
}

void bombchuCheckCollidedWithTarget_hook(GB *gb) {
  BASE(bombchuCheckCollidedWithTarget);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x29;
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  CYC(b_+7, b_+8); alu_scf(gb);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12);
  TAIL(checkObjectsCollided);
}

static void bombchu_increase_vision_radius(GB *gb) {
  BASE(bombchuCheckForEnemyTarget);
  CYC(b_+91, b_+93); E = 0x26;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+96); alu_add(gb, 0x10);
  CYC(b_+96, b_+98); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(b_+98, b_+100);
  } else {
    CYC(b_+98, b_+100);
    CYC(b_+100, b_+102); A = 0x18;
  }
  CYC(b_+102, b_+103); mem_wr(gb, DE, A);
  CYC(b_+103, b_+104); E = alu_inc8(gb, E);
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CYC(b_+105, b_+106); ret_effect(gb);
}

void bombchuCheckForEnemyTarget_hook(GB *gb) {
  BASE(bombchuCheckForEnemyTarget);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = 0x80;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto next_target; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = 0x9a;
  CYC(b_+12, b_+14); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+14, b_+16); goto next_target; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = 0x81;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); push_effect(gb, HL);
  CYC(b_+20, b_+23); SET_HL(SYM(bombchuTargets));
  CALL_C(b_+23, checkFlag_hook, SYM(checkFlag), b_+26);
  CYC(b_+26, b_+27); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(b_+27, b_+29); goto next_target; }
  CYC(b_+27, b_+29);
  CALL_C(b_+29, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+32);
  if (!(F & FC)) { CYCT(b_+32, b_+34); goto next_target; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); A = H;
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+38); L = 0x19;
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x80);
  CYC(b_+41, b_+43); L = 0x26;
  CYC(b_+43, b_+45); A = 0x06;
  CYC(b_+45, b_+46); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  CYC(b_+47, b_+49); L = 0x06;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x0c);
  CYC(b_+51, b_+53); L = 0x11;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x46);
  CYC(b_+55, b_+57); L = 0x04;
  CYC(b_+57, b_+58); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+58, b_+61); A = W8(wTilesetFlags);
  CYC(b_+61, b_+63); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+65);
    CALL_C(b_+70, bombchuUpdateAngle_sidescrolling_hook, SYM(bombchuUpdateAngle_sidescrolling), b_+73);
    CYC(b_+73, b_+74); alu_xor(gb, A);
    CYC(b_+74, b_+75); ret_effect(gb);
    return;
  }
  CYC(b_+63, b_+65);
  CALL_C(b_+65, bombchuUpdateAngle_topDown_hook, SYM(bombchuUpdateAngle_topDown), b_+68);
  CYC(b_+68, b_+69); alu_xor(gb, A);
  CYC(b_+69, b_+70); ret_effect(gb);
  return;

next_target:
  CYC(b_+75, b_+76); H = alu_inc8(gb, H);
  CYC(b_+76, b_+77); A = H;
  CYC(b_+77, b_+79); alu_cp(gb, 0xe0);
  if (F & FC) {
    CYCT(b_+79, b_+81);
  } else {
    CYC(b_+79, b_+81);
    CYC(b_+81, b_+84); push_effect(gb, b_+84);
    bombchu_increase_vision_radius(gb);
    CYC(b_+84, b_+86); A = 0xd0;
  }
  CYC(b_+86, b_+88); E = 0x30;
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+90); alu_or(gb, D);
  CYC(b_+90, b_+91); ret_effect(gb);
}

static void bombchu_initialize(GB *gb, uint16_t sp0_) {
  BASE(itemCode0d);
  CALL_C(b_+72, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+75);
  CALL_C(b_+75, decNumBombchus_hook, SYM(decNumBombchus), b_+78);
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+81); L = 0x04;
  CYC(b_+81, b_+82); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+82, b_+84); L = 0x30;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0xd0);
  CYC(b_+86, b_+88); L = 0x11;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x14);
  CYC(b_+90, b_+92); L = 0x06;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x10);
  CYC(b_+94, b_+95); L = alu_inc8(gb, L);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0xb4);
  CYC(b_+97, b_+99); L = 0x26;
  CYC(b_+99, b_+101); A = 0x18;
  CYC(b_+101, b_+102); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+102, b_+103); mem_wr(gb, HL, A);
  CYC(b_+103, b_+105); L = 0x31;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x08);
  CYC(b_+107, b_+109); L = 0x09;
  CYC(b_+109, b_+112); A = W8(w1Link_direction);
  CYC(b_+112, b_+114); A = alu_swap(gb, A);
  CYC(b_+114, b_+115); alu_rrca(gb);
  CYC(b_+115, b_+116); mem_wr(gb, HL, A);
  CYC(b_+116, b_+118); L = 0x08;
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0xff);
  CALL_C(b_+120, bombchuSetAnimationFromAngle_hook, SYM(bombchuSetAnimationFromAngle), b_+123);
  CYC(b_+123, b_+126); bombchuSetPositionInFrontOfLink_hook(gb);
}

void itemCode0d_hook(GB *gb) {
  BASE(itemCode0d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, bombchuCountdownToExplosion_hook, SYM(bombchuCountdownToExplosion), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0xff);
  if (!(F & FC)) {
    CYCT(b_+8, b_+11); TAIL(itemUpdateExplosion);
  }
  CYC(b_+8, b_+11);
  CALL_C(b_+11, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+14);
  if (!(F & FC)) {
    CYCT(b_+14, b_+17); TAIL(itemDelete);
  }
  CYC(b_+14, b_+17);
  CALL_C(b_+17, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+20);
  CYC(b_+20, b_+23); A = W8(wTilesetFlags);
  CYC(b_+23, b_+25); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+27);
    goto sidescroll;
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); C = 0x20;
  CALL_C(b_+29, itemUpdateSpeedZAndCheckHazards_hook, SYM(itemUpdateSpeedZAndCheckHazards), b_+32);
  CYC(b_+32, b_+34); E = 0x04;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); push_effect(gb, b_+36);
  do { uint16_t jt_ = (bombchu_jump_table(gb));
    if (jt_ == b_+72) { bombchu_initialize(gb, sp0_); return; }
    else if (jt_ == b_+126) { goto td_state1; }
    else if (jt_ == b_+135) { goto td_state2; }
    else if (jt_ == b_+148) { goto td_state3; }
    else if (jt_ == b_+159) { goto td_state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

sidescroll:
  CYC(b_+46, b_+48); E = 0x32;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+50, b_+52);
  } else {
    CYC(b_+50, b_+52);
    CYC(b_+52, b_+54); C = 0x18;
    CALL_C(b_+54, itemUpdateThrowingVerticallyAndCheckHazards_hook, SYM(itemUpdateThrowingVerticallyAndCheckHazards), b_+57);
    if (F & FC) {
      CYCT(b_+57, b_+60); TAIL(itemDelete);
    }
    CYC(b_+57, b_+60);
  }
  CYC(b_+60, b_+62); E = 0x04;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+64); push_effect(gb, b_+64);
  do { uint16_t jt_ = (bombchu_jump_table(gb));
    if (jt_ == b_+173) { goto ss_state0; }
    else if (jt_ == b_+188) { goto ss_state1; }
    else if (jt_ == b_+203) { goto ss_state2; }
    else if (jt_ == b_+211) { goto ss_state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

td_state1:
  CYC(b_+126, b_+127); H = D;
  CYC(b_+127, b_+129); L = 0x0f;
  CYC(b_+129, b_+131); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+131, b_+133);
    goto update_top_down;
  }
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+134); L = E;
  CYC(b_+134, b_+135); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

td_state2:
  CALL_C(b_+135, bombchuCheckForEnemyTarget_hook, SYM(bombchuCheckForEnemyTarget), b_+138);
  if (F & FZ) {
    CYCT(b_+138, b_+139); ret_effect(gb); return;
  }
  CYC(b_+138, b_+139);

update_top_down:
  CALL_C(b_+139, bombchuUpdateSpeed_hook, SYM(bombchuUpdateSpeed), b_+142);
  CALL_C(b_+142, itemUpdateConveyorBelt_hook, SYM(itemUpdateConveyorBelt), b_+145);

animate_top_down:
  CYC(b_+145, b_+148); itemAnimate_hook(gb);
  return;

td_state3:
  CYC(b_+148, b_+149); H = D;
  CYC(b_+149, b_+151); L = 0x06;
  CYC(b_+151, b_+152); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+152, b_+155); TAIL(itemUpdateConveyorBelt);
  }
  CYC(b_+152, b_+155);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x0a);
  CYC(b_+157, b_+158); L = E;
  CYC(b_+158, b_+159); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

td_state4:
  CALL_C(b_+159, bombchuCheckCollidedWithTarget_hook, SYM(bombchuCheckCollidedWithTarget), b_+162);
  if (F & FC) {
    CYCT(b_+162, b_+165); TAIL(bombchuClearCounter2AndInitializeExplosion);
  }
  CYC(b_+162, b_+165);
  CALL_C(b_+165, bombchuUpdateVelocity_hook, SYM(bombchuUpdateVelocity), b_+168);
  CALL_C(b_+168, itemUpdateConveyorBelt_hook, SYM(itemUpdateConveyorBelt), b_+171);
  CYC(b_+171, b_+173);
  goto animate_top_down;

ss_state0:
  CYC(b_+173, b_+176); push_effect(gb, b_+176); bombchu_initialize(gb, sp0_);
  CYC(b_+176, b_+178); E = 0x09;
  CYC(b_+178, b_+179); A = mem_rd(gb, DE);
  CYC(b_+179, b_+181); alu_bit(gb, 3, A);
  if (!(F & FZ)) {
    CYCT(b_+181, b_+182); ret_effect(gb); return;
  }
  CYC(b_+181, b_+182);
  CYC(b_+182, b_+184); alu_add(gb, 0x08);
  CYC(b_+184, b_+185); mem_wr(gb, DE, A);
  CYC(b_+185, b_+188); bombchuSetAnimationFromAngle_hook(gb);
  return;

ss_state1:
  CYC(b_+188, b_+190); E = 0x10;
  CYC(b_+190, b_+192); A = 0x14;
  CYC(b_+192, b_+193); mem_wr(gb, DE, A);
  CALL_C(b_+193, bombchuCheckForEnemyTarget_hook, SYM(bombchuCheckForEnemyTarget), b_+196);
  if (F & FZ) {
    CYCT(b_+196, b_+197); ret_effect(gb); return;
  }
  CYC(b_+196, b_+197);
  CALL_C(b_+197, bombchuCheckWallsAndApplySpeed_hook, SYM(bombchuCheckWallsAndApplySpeed), b_+200);

animate_sidescroll:
  CYC(b_+200, b_+203); itemAnimate_hook(gb);
  return;

ss_state2:
  CALL_C(b_+203, itemDecCounter1_hook, SYM(itemDecCounter1), b_+206);
  if (!(F & FZ)) {
    CYCT(b_+206, b_+207); ret_effect(gb); return;
  }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+209); mem_wr(gb, HL, 0x0a);
  CYC(b_+209, b_+210); L = E;
  CYC(b_+210, b_+211); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

ss_state3:
  CALL_C(b_+211, bombchuCheckCollidedWithTarget_hook, SYM(bombchuCheckCollidedWithTarget), b_+214);
  if (F & FC) {
    CYCT(b_+214, b_+217); TAIL(bombchuClearCounter2AndInitializeExplosion);
  }
  CYC(b_+214, b_+217);
  CALL_C(b_+217, bombchuUpdateVelocityAndClimbing_sidescroll_hook, SYM(bombchuUpdateVelocityAndClimbing_sidescroll), b_+220);
  CYC(b_+220, b_+222);
  goto animate_sidescroll;
}
