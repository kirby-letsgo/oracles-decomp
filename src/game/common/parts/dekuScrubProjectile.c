#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5313, 0x5315); alu_sub(gb, 0x0c);
  CYC(0x5315, 0x5316); alu_rrca(gb);
  CYC(0x5316, 0x5317); B = A;
  CYC(0x5317, 0x531a); A = mem_rd(gb, 0xd008); // w1Link.direction
  CYC(0x531a, 0x531b); alu_add(gb, B);
  CYC(0x531b, 0x531e); SET_HL(0x5322);
  CYC(0x531e, 0x531f); dekuScrubProjectile_addAToHl_from_rst(gb, 0x531f);
  CYC(0x531f, 0x5320); A = mem_rd(gb, HL);
  CYC(0x5320, 0x5321); mem_wr(gb, DE, A);
  RET(0x5321); return; // ret
}

void func_52f4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x52f4, 0x52f6); E = 0xc4; // Part.state
  CYC(0x52f6, 0x52f8); A = 0x04;
  CYC(0x52f8, 0x52f9); mem_wr(gb, DE, A);
  CYC(0x52f9, 0x52fa); alu_xor(gb, A);
  CYC(0x52fa, 0x52fd); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}

void func_52fd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x52fd, 0x52ff); E = 0xc9; // Part.angle
  CYC(0x52ff, 0x5300); A = mem_rd(gb, DE);
  CYC(0x5300, 0x5302); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x5302, 0x5304); func_5313_hook(gb); return; } // jr nz
  CYC(0x5302, 0x5304);
  CYC(0x5304, 0x5306); alu_sub(gb, 0x08);
  CYC(0x5306, 0x5307); alu_rrca(gb);
  CYC(0x5307, 0x5308); B = A;
  CYC(0x5308, 0x530b); A = mem_rd(gb, 0xd008); // w1Link.direction
  CYC(0x530b, 0x530c); alu_add(gb, B);
  CYC(0x530c, 0x530f); SET_HL(0x532a);
  CYC(0x530f, 0x5310); dekuScrubProjectile_addAToHl_from_rst(gb, 0x5310);
  CYC(0x5310, 0x5311); A = mem_rd(gb, HL);
  CYC(0x5311, 0x5312); mem_wr(gb, DE, A);
  RET(0x5312); return; // ret
}

void func_5336_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5336, 0x5338); A = 0x24;
  CALL_C(0x5338, objectGetRelatedObject1Var_hook, 0x2160, 0x533b);
  CYC(0x533b, 0x533d); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x533d); return; } // ret z
  CYC(0x533d, 0x533e);
  CALL_C(0x533e, checkObjectsCollided_hook, 0x1d5a, 0x5341);
  if (!(F & FC)) { RET_TAKEN(0x5341); return; } // ret nc
  CYC(0x5341, 0x5342);
  CYC(0x5342, 0x5344); L = 0xaa;
  CYC(0x5344, 0x5346); mem_wr(gb, HL, 0x82);
  CYC(0x5346, 0x5348); L = 0xb0;
  CYC(0x5348, 0x5349); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5349, 0x534b); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x534b, 0x534d); mem_wr(gb, HL, 0x0c);
  CYC(0x534d, 0x534f); E = 0xc4; // Part.state
  CYC(0x534f, 0x5351); A = 0x04;
  CYC(0x5351, 0x5352); mem_wr(gb, DE, A);
  RET(0x5352); return; // ret
}

void partCode1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x529c, 0x529e); goto normalStatus; } // jr z
  CYC(0x529c, 0x529e);
  CYC(0x529e, 0x52a0); E = 0xea; // Part.var2a
  CYC(0x52a0, 0x52a1); A = mem_rd(gb, DE);
  CYC(0x52a1, 0x52a3); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x52a3, 0x52a5); goto normalStatus; } // jr z
  CYC(0x52a3, 0x52a5);
  CALL_C(0x52a5, func_52fd_hook, 0x52fd, 0x52a8);
  CYC(0x52a8, 0x52a9); H = D;
  CYC(0x52a9, 0x52ab); L = 0xc4; // Part.state
  CYC(0x52ab, 0x52ad); mem_wr(gb, HL, 0x03);
  CYC(0x52ad, 0x52af); L = 0xe4; // Part.collisionType
  CYC(0x52af, 0x52b1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));

normalStatus:
  CYC(0x52b1, 0x52b3); E = 0xc4; // Part.state
  CYC(0x52b3, 0x52b4); A = mem_rd(gb, DE);
  {
    CYC(0x52b4, 0x52b5); push_effect(gb, 0x52b5);
    uint16_t target = dekuScrubProjectile_jump_table(gb);
    if (target == 0x52c1) goto state0;
    if (target == 0x52d4) goto state1;
    if (target == 0x52db) goto state2;
    if (target == 0x52ec) goto state3;
    if (target == 0x52f1) goto state5;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

state0:
  CYC(0x52c1, 0x52c2); H = D;
  CYC(0x52c2, 0x52c3); L = E;
  CYC(0x52c3, 0x52c4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x52c4, 0x52c6); L = 0xd0; // Part.speed
  CYC(0x52c6, 0x52c8); mem_wr(gb, HL, 0x50);
  CYC(0x52c8, 0x52ca); L = 0xc6; // Part.counter1
  CYC(0x52ca, 0x52cc); mem_wr(gb, HL, 0x08);
  CYC(0x52cc, 0x52ce); A = 0xa6; // SND_STRIKE
  CALL_C(0x52ce, playSound_b00_hook, 0x0c98, 0x52d1);
  CYC(0x52d1, 0x52d4); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x52d4, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x52d7);
  if (!(F & FZ)) { CYCT(0x52d7, 0x52d9); goto L_52e4; } // jr nz
  CYC(0x52d7, 0x52d9);
  CYC(0x52d9, 0x52da); L = E;
  CYC(0x52da, 0x52db); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(0x52db, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x52de);
  if (!(F & FC)) { CYCT(0x52de, 0x52e0); goto L_52e4; } // jr nc
  CYC(0x52de, 0x52e0);
  if (!(F & FZ)) { CYCT(0x52e0, 0x52e2); func_52f4_hook(gb); return; } // jr nz
  CYC(0x52e0, 0x52e2);
  CYC(0x52e2, 0x52e4); goto state5; // jr

L_52e4:
  CALL_C(0x52e4, objectCheckWithinScreenBoundary_hook, 0x2184, 0x52e7);
  if (F & FC) { CYCT(0x52e7, 0x52ea); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x52e7, 0x52ea);
  CYC(0x52ea, 0x52ec); goto state5; // jr

state3:
  CALL_C(0x52ec, func_5336_hook, 0x5336, 0x52ef);
  CYC(0x52ef, 0x52f1); goto state2; // jr

state5:
  CYC(0x52f1, 0x52f4); partDelete_hook(gb); return; // jp
}
