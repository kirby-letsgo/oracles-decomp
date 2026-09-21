#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCommon_getTileCollisionInFront);
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  partCommon_getTileCollisionAtAngle_hook(gb); // falls through
}

void partCommon_getTileCollisionAtAngle_hook(GB *gb) {
  BASE(partCommon_getTileCollisionAtAngle);
  CYC(b_+0, b_+2); alu_add(gb, 0x02);
  CYC(b_+2, b_+4); alu_and(gb, 0x1c);
  CYC(b_+4, b_+5); alu_rrca(gb);
  CYC(b_+5, b_+8); SET_HL(SYM(partCommon_anglePositionOffsets)); // partCommon_anglePositionOffsets
  CYC(b_+8, b_+9); commonCode_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = 0xcb; // Part.yh
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); E = 0xcd; // Part.xh
  CYC(b_+16, b_+17); SET_HL(HL + 1);
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+23); TAIL(getTileCollisionsAtPosition); // jp
}

void partCommon_getTileCollisionAtAngle_allowHoles_hook(GB *gb) {
  BASE(partCommon_getTileCollisionAtAngle_allowHoles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_getTileCollisionAtAngle_hook, SYM(partCommon_getTileCollisionAtAngle), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); commonCode_allowHolesTail(gb); return; // jr
}

void partCommon_getTileCollisionInFront_allowHoles_hook(GB *gb) {
  BASE(partCommon_getTileCollisionInFront_allowHoles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_getTileCollisionInFront_hook, SYM(partCommon_getTileCollisionInFront), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  commonCode_allowHolesTail(gb); // falls through
}

static void commonCode_allowHolesTail(GB *gb) {
  BASE(partCommon_getTileCollisionInFront_allowHoles);
  CYC(b_+4, b_+6); alu_add(gb, 0x01);
  if (F & FC) { RET_TAKEN(b_+6); return; } // ret c
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  CYC(b_+8, b_+11); TAIL(checkGivenCollision_allowHoles); // jp
}

void partCommon_standardUpdate_hook(GB *gb) {
  BASE(partCommon_standardUpdate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc4; // Part.state
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto uninitialized; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = 0xeb; // Part.invincibilityCounter
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto doneUpdatingInvincibility; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto decHl; } // jr nc
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+17, b_+19); goto doneUpdatingInvincibility; // jr

decHl:
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

doneUpdatingInvincibility:
  CYC(b_+20, b_+21); L = alu_dec8(gb, L);
  CYC(b_+21, b_+23); alu_bit(gb, 7, mem_rd(gb, HL)); // [Part.var2a]
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto collision; } // jr nz
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); L = alu_dec8(gb, L);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); // [Part.health]
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); goto dead; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); C = 0x00; // PARTSTATUS_NORMAL
  RET(b_+32); return; // ret

uninitialized:
  CYC(b_+33, b_+36); SET_HL(0x43c9);
  CYC(b_+36, b_+38); E = 0x3f;
  CALL_C(b_+38, interBankCall_hook, 0x008a, b_+41);
  CYC(b_+41, b_+43); E = 0xfe; // Part.var3e
  CYC(b_+43, b_+45); A = 0x08; // TODO: what's this
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48); C = 0x00; // PARTSTATUS_NORMAL
  RET(b_+48); return; // ret

collision:
  CYC(b_+49, b_+51); C = 0x01; // PARTSTATUS_JUST_HIT
  RET(b_+51); return; // ret

dead:
  CYC(b_+52, b_+54); C = 0x02; // PARTSTATUS_DEAD
  RET(b_+54); return; // ret
}

void partCommon_checkTileCollisionOrOutOfBounds_hook(GB *gb) {
  BASE(partCommon_checkTileCollisionOrOutOfBounds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CALL_C(b_+6, checkTileCollision_allowHoles_hook, SYM(checkTileCollision_allowHoles), b_+9);
  if (F & FC) { RET_TAKEN(b_+9); return; } // ret c
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); alu_or(gb, D);
  RET(b_+11); return; // ret
}

void partCommon_checkOutOfBounds_hook(GB *gb) {
  BASE(partCommon_checkOutOfBounds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xcb; // Part.yh
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = 0xcd; // Part.xh
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+10); commonCode_checkOutOfBounds_roundAngleToDiagonal(gb);
  CYC(b_+10, b_+11); A = E;
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+15); SET_HL(SYM(partCommon_anglePositionOffsets)); // partCommon_anglePositionOffsets
  CYC(b_+15, b_+16); commonCode_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); alu_add(gb, B);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+22); C = A;
  CALL_C(b_+22, getTileCollisionsAtPosition_hook, SYM(getTileCollisionsAtPosition), b_+25);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
  RET(b_+26); return; // ret
}

static void commonCode_checkOutOfBounds_roundAngleToDiagonal(GB *gb) {
  BASE(partCommon_checkOutOfBounds);
  CYC(b_+27, b_+29); L = 0xc9; // Part.angle
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); E = A;
  CYC(b_+31, b_+33); alu_and(gb, 0x07);
  if (F & FZ) { CYCT(b_+33, b_+34); return; } // ret z
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+35); A = E;
  CYC(b_+35, b_+37); alu_and(gb, 0x18);
  CYC(b_+37, b_+39); alu_add(gb, 0x04);
  CYC(b_+39, b_+40); E = A;
  CYC(b_+40, b_+41); return; // ret
}

void partCommon_decCounter1IfNonzero_hook(GB *gb) {
  BASE(partCommon_decCounter1IfNonzero);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc6; // Part.counter1
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return; // ret
}

void partCommon_bounceWhenCollisionsEnabled_hook(GB *gb) {
  BASE(partCommon_bounceWhenCollisionsEnabled);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xe4; // Part.collisionType
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(b_+8, partSetAnimation_hook, SYM(partSetAnimation), b_+11);
  CYC(b_+11, b_+14); SET_BC(0xff20);
  CALL_C(b_+14, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+17);
  CYC(b_+17, b_+19); L = 0xc6; // Part.counter1
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x20);
  CYC(b_+21, b_+23); L = 0xd0; // Part.speed
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(b_+25, b_+27); L = 0xc9; // Part.angle
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+30); alu_xor(gb, 0x10);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  RET(b_+31); return; // ret
}

void partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(GB *gb) {
  BASE(partCommon_updateSpeedAndDeleteWhenCounter1Is0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); partDelete_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); C = 0x0e;
  CALL_C(b_+8, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+11);
  CALL_C(b_+11, partAnimate_hook, SYM(partAnimate), b_+14);
  CYC(b_+14, b_+17); TAIL(objectApplySpeed); // jp
}

void partCommon_setPositionOffsetAndRadiusFromAngle_hook(GB *gb) {
  BASE(partCommon_setPositionOffsetAndRadiusFromAngle);
  CYC(b_+0, b_+2); E = 0xc9; // Part.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+11); SET_HL(b_+30); // @data
  CYC(b_+11, b_+12); commonCode_addAToHl_from_rst(gb, b_+12);
  CYC(b_+12, b_+14); E = 0xcb; // Part.yh
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); E = 0xcd; // Part.xh
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); alu_add(gb, C);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A); // Part.collisionRadiusX
  RET(b_+29); return; // ret
}

void partCommon_incSubstate_hook(GB *gb) {
  BASE(partCommon_incSubstate);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc5; // Part.substate
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+4); return; // ret
}
