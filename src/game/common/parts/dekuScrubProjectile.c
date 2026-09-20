#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(func_5313), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(func_5313), (from), (to), true)

static uint16_t dekuScrubProjectile_jump_table(GB *gb) {
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

static void dekuScrubProjectile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_5313_hook(GB *gb) {
  BASE(func_5313);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_sub(gb, 0x0c);
  CYC(b_+2, b_+3); alu_rrca(gb);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, w1Link_direction); // w1Link.direction
  CYC(b_+7, b_+8); alu_add(gb, B);
  CYC(b_+8, b_+11); SET_HL(SYM(table_5322));
  CYC(b_+11, b_+12); dekuScrubProjectile_addAToHl_from_rst(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

void func_52f4_hook(GB *gb) {
  BASE(func_52f4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+4); A = 0x04;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, SYM(func_52fd)); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}

void func_52fd_hook(GB *gb) {
  BASE(func_52fd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); func_5313_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_sub(gb, 0x08);
  CYC(b_+9, b_+10); alu_rrca(gb);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_direction); // w1Link.direction
  CYC(b_+14, b_+15); alu_add(gb, B);
  CYC(b_+15, b_+18); SET_HL(SYM(table_532a));
  CYC(b_+18, b_+19); dekuScrubProjectile_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret
}

void func_5336_hook(GB *gb) {
  BASE(func_5336);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x24;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CALL_C(b_+8, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+11);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; } // ret nc
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); L = 0xaa;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x82);
  CYC(b_+16, b_+18); L = 0xb0;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0c);
  CYC(b_+23, b_+25); E = 0xc4; // Part.state
  CYC(b_+25, b_+27); A = 0x04;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret
}

void partCode1e_hook(GB *gb) {
  BASE(partCode1e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+7, b_+9); goto normalStatus; } // jr z
  CYC(b_+7, b_+9);
  CALL_C(b_+9, func_52fd_hook, SYM(func_52fd), b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0xc4; // Part.state
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x03);
  CYC(b_+17, b_+19); L = 0xe4; // Part.collisionType
  CYC(b_+19, b_+21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));

normalStatus:
  CYC(b_+21, b_+23); E = 0xc4; // Part.state
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  {
    CYC(b_+24, b_+25); push_effect(gb, b_+25);
    uint16_t target = dekuScrubProjectile_jump_table(gb);
    if (target == b_+37) goto state0;
    if (target == b_+56) goto state1;
    if (target == b_+63) goto state2;
    if (target == b_+80) goto state3;
    if (target == b_+85) goto state5;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

state0:
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+39); L = E;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+42); L = 0xd0; // Part.speed
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x50);
  CYC(b_+44, b_+46); L = 0xc6; // Part.counter1
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x08);
  CYC(b_+48, b_+50); A = 0xa6; // SND_STRIKE
  CALL_C(b_+50, playSound_b00_hook, SYM(playSound_b00), b_+53);
  CYC(b_+53, b_+56); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(b_+56, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+59);
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto L_52e4; } // jr nz
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); L = E;
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(b_+63, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+66);
  if (!(F & FC)) { CYCT(b_+66, b_+68); goto L_52e4; } // jr nc
  CYC(b_+66, b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); func_52f4_hook(gb); return; } // jr nz
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+72); goto state5; // jr

L_52e4:
  CALL_C(b_+72, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+75);
  if (F & FC) { CYCT(b_+75, b_+78); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+75, b_+78);
  CYC(b_+78, b_+80); goto state5; // jr

state3:
  CALL_C(b_+80, func_5336_hook, SYM(func_5336), b_+83);
  CYC(b_+83, b_+85); goto state2; // jr

state5:
  CYC(b_+85, SYM(func_52f4)); partDelete_hook(gb); return; // jp
}
