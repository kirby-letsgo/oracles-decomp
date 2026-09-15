#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static void commonCode_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCommon_getTileCollisionInFront_hook(GB *gb);
void partCommon_getTileCollisionAtAngle_hook(GB *gb);
void partCommon_getTileCollisionAtAngle_allowHoles_hook(GB *gb);
void partCommon_getTileCollisionInFront_allowHoles_hook(GB *gb);
void partCommon_standardUpdate_hook(GB *gb);
void partCommon_checkTileCollisionOrOutOfBounds_hook(GB *gb);
void partCommon_checkOutOfBounds_hook(GB *gb);
void partCommon_decCounter1IfNonzero_hook(GB *gb);
void partCommon_bounceWhenCollisionsEnabled_hook(GB *gb);
void partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(GB *gb);
void partCommon_setPositionOffsetAndRadiusFromAngle_hook(GB *gb);
void partCommon_incSubstate_hook(GB *gb);
static void commonCode_allowHolesTail(GB *gb);
static void commonCode_checkOutOfBounds_roundAngleToDiagonal(GB *gb);

void partCommon_getTileCollisionInFront_hook(GB *gb) {
  CYC(0x4000, 0x4002); E = 0xc9; // Part.angle
  CYC(0x4002, 0x4003); A = mem_rd(gb, DE);
  partCommon_getTileCollisionAtAngle_hook(gb); // falls through
}

void partCommon_getTileCollisionAtAngle_hook(GB *gb) {
  CYC(0x4003, 0x4005); alu_add(gb, 0x02);
  CYC(0x4005, 0x4007); alu_and(gb, 0x1c);
  CYC(0x4007, 0x4008); alu_rrca(gb);
  CYC(0x4008, 0x400b); SET_HL(0x401a); // partCommon_anglePositionOffsets
  CYC(0x400b, 0x400c); commonCode_addAToHl_from_rst(gb, 0x400c);
  CYC(0x400c, 0x400e); E = 0xcb; // Part.yh
  CYC(0x400e, 0x400f); A = mem_rd(gb, DE);
  CYC(0x400f, 0x4010); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4010, 0x4011); B = A;
  CYC(0x4011, 0x4013); E = 0xcd; // Part.xh
  CYC(0x4013, 0x4014); SET_HL(HL + 1);
  CYC(0x4014, 0x4015); A = mem_rd(gb, DE);
  CYC(0x4015, 0x4016); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4016, 0x4017); C = A;
  CYC(0x4017, 0x401a); getTileCollisionsAtPosition_hook(gb); return; // jp
}

void partCommon_getTileCollisionAtAngle_allowHoles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x402a, partCommon_getTileCollisionAtAngle_hook, 0x4003, 0x402d);
  if (F & FZ) { RET_TAKEN(0x402d); return; } // ret z
  CYC(0x402d, 0x402e);
  CYC(0x402e, 0x4030); commonCode_allowHolesTail(gb); return; // jr
}

void partCommon_getTileCollisionInFront_allowHoles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4030, partCommon_getTileCollisionInFront_hook, 0x4000, 0x4033);
  if (F & FZ) { RET_TAKEN(0x4033); return; } // ret z
  CYC(0x4033, 0x4034);
  commonCode_allowHolesTail(gb); // falls through
}

static void commonCode_allowHolesTail(GB *gb) {
  CYC(0x4034, 0x4036); alu_add(gb, 0x01);
  if (F & FC) { RET_TAKEN(0x4036); return; } // ret c
  CYC(0x4036, 0x4037);
  CYC(0x4037, 0x4038); A = alu_dec8(gb, A);
  CYC(0x4038, 0x403b); checkGivenCollision_allowHoles_hook(gb); return; // jp
}

void partCommon_standardUpdate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x403b, 0x403c); H = D;
  CYC(0x403c, 0x403e); L = 0xc4; // Part.state
  CYC(0x403e, 0x403f); A = mem_rd(gb, HL);
  CYC(0x403f, 0x4040); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4040, 0x4042); goto uninitialized; } // jr z
  CYC(0x4040, 0x4042);
  CYC(0x4042, 0x4044); L = 0xeb; // Part.invincibilityCounter
  CYC(0x4044, 0x4045); A = mem_rd(gb, HL);
  CYC(0x4045, 0x4046); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4046, 0x4048); goto doneUpdatingInvincibility; } // jr z
  CYC(0x4046, 0x4048);
  CYC(0x4048, 0x4049); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x4049, 0x404b); goto decHl; } // jr nc
  CYC(0x4049, 0x404b);
  CYC(0x404b, 0x404c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x404c, 0x404e); goto doneUpdatingInvincibility; // jr

decHl:
  CYC(0x404e, 0x404f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

doneUpdatingInvincibility:
  CYC(0x404f, 0x4050); L = alu_dec8(gb, L);
  CYC(0x4050, 0x4052); alu_bit(gb, 7, mem_rd(gb, HL)); // [Part.var2a]
  if (!(F & FZ)) { CYCT(0x4052, 0x4054); goto collision; } // jr nz
  CYC(0x4052, 0x4054);
  CYC(0x4054, 0x4055); L = alu_dec8(gb, L);
  CYC(0x4055, 0x4056); A = mem_rd(gb, HL); // [Part.health]
  CYC(0x4056, 0x4057); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4057, 0x4059); goto dead; } // jr z
  CYC(0x4057, 0x4059);
  CYC(0x4059, 0x405b); C = 0x00; // PARTSTATUS_NORMAL
  RET(0x405b); return; // ret

uninitialized:
  CYC(0x405c, 0x405f); SET_HL(0x43c9);
  CYC(0x405f, 0x4061); E = 0x3f;
  CALL_C(0x4061, interBankCall_hook, 0x008a, 0x4064);
  CYC(0x4064, 0x4066); E = 0xfe; // Part.var3e
  CYC(0x4066, 0x4068); A = 0x08; // TODO: what's this
  CYC(0x4068, 0x4069); mem_wr(gb, DE, A);
  CYC(0x4069, 0x406b); C = 0x00; // PARTSTATUS_NORMAL
  RET(0x406b); return; // ret

collision:
  CYC(0x406c, 0x406e); C = 0x01; // PARTSTATUS_JUST_HIT
  RET(0x406e); return; // ret

dead:
  CYC(0x406f, 0x4071); C = 0x02; // PARTSTATUS_DEAD
  RET(0x4071); return; // ret
}

void partCommon_checkTileCollisionOrOutOfBounds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4072, objectGetTileCollisions_hook, 0x14ad, 0x4075);
  CYC(0x4075, 0x4077); alu_add(gb, 0x01);
  if (F & FZ) { RET_TAKEN(0x4077); return; } // ret z
  CYC(0x4077, 0x4078);
  CALL_C(0x4078, checkTileCollision_allowHoles_hook, 0x14dc, 0x407b);
  if (F & FC) { RET_TAKEN(0x407b); return; } // ret c
  CYC(0x407b, 0x407c);
  CYC(0x407c, 0x407d); alu_or(gb, D);
  RET(0x407d); return; // ret
}

void partCommon_checkOutOfBounds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x407e, 0x407f); H = D;
  CYC(0x407f, 0x4081); L = 0xcb; // Part.yh
  CYC(0x4081, 0x4082); B = mem_rd(gb, HL);
  CYC(0x4082, 0x4084); L = 0xcd; // Part.xh
  CYC(0x4084, 0x4085); C = mem_rd(gb, HL);
  CYC(0x4085, 0x4088); commonCode_checkOutOfBounds_roundAngleToDiagonal(gb);
  CYC(0x4088, 0x4089); A = E;
  CYC(0x4089, 0x408a); alu_rrca(gb);
  CYC(0x408a, 0x408d); SET_HL(0x401a); // partCommon_anglePositionOffsets
  CYC(0x408d, 0x408e); commonCode_addAToHl_from_rst(gb, 0x408e);
  CYC(0x408e, 0x408f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x408f, 0x4090); alu_add(gb, B);
  CYC(0x4090, 0x4091); B = A;
  CYC(0x4091, 0x4092); A = mem_rd(gb, HL);
  CYC(0x4092, 0x4093); alu_add(gb, C);
  CYC(0x4093, 0x4094); C = A;
  CALL_C(0x4094, getTileCollisionsAtPosition_hook, 0x14b7, 0x4097);
  CYC(0x4097, 0x4098); A = alu_inc8(gb, A);
  RET(0x4098); return; // ret
}

static void commonCode_checkOutOfBounds_roundAngleToDiagonal(GB *gb) {
  CYC(0x4099, 0x409b); L = 0xc9; // Part.angle
  CYC(0x409b, 0x409c); A = mem_rd(gb, HL);
  CYC(0x409c, 0x409d); E = A;
  CYC(0x409d, 0x409f); alu_and(gb, 0x07);
  if (F & FZ) { RET_TAKEN(0x409f); return; } // ret z
  CYC(0x409f, 0x40a0);
  CYC(0x40a0, 0x40a1); A = E;
  CYC(0x40a1, 0x40a3); alu_and(gb, 0x18);
  CYC(0x40a3, 0x40a5); alu_add(gb, 0x04);
  CYC(0x40a5, 0x40a6); E = A;
  RET(0x40a6); return; // ret
}

void partCommon_decCounter1IfNonzero_hook(GB *gb) {
  CYC(0x40a7, 0x40a8); H = D;
  CYC(0x40a8, 0x40aa); L = 0xc6; // Part.counter1
  CYC(0x40aa, 0x40ab); A = mem_rd(gb, HL);
  CYC(0x40ab, 0x40ac); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x40ac); return; } // ret z
  CYC(0x40ac, 0x40ad);
  CYC(0x40ad, 0x40ae); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x40ae); return; // ret
}

void partCommon_bounceWhenCollisionsEnabled_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x40af, 0x40b0); H = D;
  CYC(0x40b0, 0x40b2); L = 0xe4; // Part.collisionType
  CYC(0x40b2, 0x40b4); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x40b4); return; } // ret z
  CYC(0x40b4, 0x40b5);
  CYC(0x40b5, 0x40b7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(0x40b7, partSetAnimation_hook, 0x2988, 0x40ba);
  CYC(0x40ba, 0x40bd); SET_BC(0xff20);
  CALL_C(0x40bd, objectSetSpeedZ_hook, 0x239d, 0x40c0);
  CYC(0x40c0, 0x40c2); L = 0xc6; // Part.counter1
  CYC(0x40c2, 0x40c4); mem_wr(gb, HL, 0x20);
  CYC(0x40c4, 0x40c6); L = 0xd0; // Part.speed
  CYC(0x40c6, 0x40c8); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(0x40c8, 0x40ca); L = 0xc9; // Part.angle
  CYC(0x40ca, 0x40cb); A = mem_rd(gb, HL);
  CYC(0x40cb, 0x40cd); alu_xor(gb, 0x10);
  CYC(0x40cd, 0x40ce); mem_wr(gb, HL, A);
  RET(0x40ce); return; // ret
}

void partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x40cf, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x40d2);
  if (F & FZ) { CYCT(0x40d2, 0x40d5); partDelete_hook(gb); return; } // jp z
  CYC(0x40d2, 0x40d5);
  CYC(0x40d5, 0x40d7); C = 0x0e;
  CALL_C(0x40d7, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x40da);
  CALL_C(0x40da, partAnimate_hook, 0x2978, 0x40dd);
  CYC(0x40dd, 0x40e0); objectApplySpeed_hook(gb); return; // jp
}

void partCommon_setPositionOffsetAndRadiusFromAngle_hook(GB *gb) {
  CYC(0x40e0, 0x40e2); E = 0xc9; // Part.angle
  CYC(0x40e2, 0x40e3); A = mem_rd(gb, DE);
  CYC(0x40e3, 0x40e5); alu_add(gb, 0x04);
  CYC(0x40e5, 0x40e7); alu_and(gb, 0x18);
  CYC(0x40e7, 0x40e8); alu_rrca(gb);
  CYC(0x40e8, 0x40eb); SET_HL(0x40fe); // @data
  CYC(0x40eb, 0x40ec); commonCode_addAToHl_from_rst(gb, 0x40ec);
  CYC(0x40ec, 0x40ee); E = 0xcb; // Part.yh
  CYC(0x40ee, 0x40ef); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40ef, 0x40f0); alu_add(gb, B);
  CYC(0x40f0, 0x40f1); mem_wr(gb, DE, A);
  CYC(0x40f1, 0x40f3); E = 0xcd; // Part.xh
  CYC(0x40f3, 0x40f4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40f4, 0x40f5); alu_add(gb, C);
  CYC(0x40f5, 0x40f6); mem_wr(gb, DE, A);
  CYC(0x40f6, 0x40f8); E = 0xe6; // Part.collisionRadiusY
  CYC(0x40f8, 0x40f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40f9, 0x40fa); mem_wr(gb, DE, A);
  CYC(0x40fa, 0x40fb); E = alu_inc8(gb, E);
  CYC(0x40fb, 0x40fc); A = mem_rd(gb, HL);
  CYC(0x40fc, 0x40fd); mem_wr(gb, DE, A); // Part.collisionRadiusX
  RET(0x40fd); return; // ret
}

void partCommon_incSubstate_hook(GB *gb) {
  CYC(0x410e, 0x410f); H = D;
  CYC(0x410f, 0x4111); L = 0xc5; // Part.substate
  CYC(0x4111, 0x4112); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x4112); return; // ret
}
