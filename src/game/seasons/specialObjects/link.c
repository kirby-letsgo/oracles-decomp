#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/specialObjects/link.s, the .ifdef ROM_SEASONS part:
// Link pushing against a tree stump (bank $05).

static void link_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void link_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// Copies speed, speedTmp, speedZ (16-bit) from the 3-byte table entry at hl into the object at de.
static void link_load_stump_speeds(GB *gb, uint16_t a) {
  BANKOF(checkLinkPushingAgainstTreeStump);
  CYC(a, a + 2); E = 0x10; // SpecialObject.speed
  CYC(a + 2, a + 3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(a + 3, a + 4); mem_wr(gb, DE, A);
  CYC(a + 4, a + 5); E = alu_inc8(gb, E);
  CYC(a + 5, a + 6); mem_wr(gb, DE, A);
  CYC(a + 6, a + 8); E = 0x14; // SpecialObject.speedZ
  CYC(a + 8, a + 9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(a + 9, a + 10); mem_wr(gb, DE, A);
  CYC(a + 10, a + 11); E = alu_inc8(gb, E);
  CYC(a + 11, a + 12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(a + 12, a + 13); mem_wr(gb, DE, A);
}

void s_checkLinkPushingAgainstTreeStump_hook(GB *gb) {
  BASE(checkLinkPushingAgainstTreeStump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveTileType);
  CYC(b_+3, b_+5); alu_cp(gb, 0x08); // TILETYPE_STUMP
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL_S(seasonsFunc_05_5ed3); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wActiveGroup);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wLinkAngle);
  CYC(b_+16, b_+18); alu_and(gb, 0xe7);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, checkLinkPushingAgainstWall_hook, SYM(checkLinkPushingAgainstWall), b_+22);
  if (!(F & FC)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); E = 0x08; // SpecialObject.direction
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+29); SET_HL(b_+75); // @relativeTile
  CYC(b_+29, b_+30); link_add_double_index(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+32); B = A;
  CYC(b_+32, b_+33); C = mem_rd(gb, HL);
  CALL_C(b_+33, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+36);
  CYC(b_+36, b_+38); alu_cp(gb, 0x20);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); A = 0x01;
  CALL_C(b_+41, specialObjectSetVar37AndVar38_hook, SYM(specialObjectSetVar37AndVar38), b_+44);
  CYC(b_+44, b_+46); E = 0x08; // SpecialObject.direction
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); L = A;
  CYC(b_+48, b_+49); alu_add(gb, A);
  CYC(b_+49, b_+50); alu_add(gb, L);
  CYC(b_+50, b_+53); SET_HL(b_+83); // @speedValues
  CYC(b_+53, b_+54); link_add_a_to_hl(gb, b_+54);
  link_load_stump_speeds(gb, b_+54);
  CYC(b_+67, b_+69); A = 0x81;
  CYC(b_+69, b_+72); mem_wr(gb, wLinkInAir, A);
  CYC(b_+72, b_+75);
  TAIL(linkCancelAllItemUsage);
}

// Link walked onto a stump tile: jump off it in the direction he faces if the landing tile
// allows it. Returns z (a=0) when the jump started, nz otherwise.
void s_seasonsFunc_05_5ed3_hook(GB *gb) {
  BASE(seasonsFunc_05_5ed3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkAngle);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); alu_and(gb, 0xe7);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto blocked; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+10); alu_add(gb, A);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+15); SET_HL(b_+84); // @relativeTile
  CYC(b_+15, b_+16); link_add_double_index(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); C = mem_rd(gb, HL);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = 0x0b; // SpecialObject.yh
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+25); L = 0x0d; // SpecialObject.xh
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); alu_add(gb, C);
  CYC(b_+27, b_+28); C = A;
  CALL_C(b_+28, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+31);
  if (F & FC) { CYCT(b_+31, b_+33); goto blocked; }
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); A = mem_rd(gb, wLinkAngle);
  CYC(b_+36, b_+38); E = 0x09; // SpecialObject.angle
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); alu_add(gb, A);
  CYC(b_+40, b_+42); A = alu_swap(gb, A);
  CYC(b_+42, b_+43); C = A;
  CYC(b_+43, b_+44); alu_add(gb, A);
  CYC(b_+44, b_+45); alu_add(gb, C);
  CYC(b_+45, b_+48); SET_HL(b_+92); // @speedValues
  CYC(b_+48, b_+49); link_add_a_to_hl(gb, b_+49);
  CYC(b_+49, b_+52); A = mem_rd(gb, wLinkTurningDisabled);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto keep_direction; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); E = 0x08; // SpecialObject.direction
  CYC(b_+57, b_+58); A = C;
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
keep_direction:
  link_load_stump_speeds(gb, b_+59);
  CALL_C(b_+72, clearVar37AndVar38_hook, SYM(clearVar37AndVar38), b_+75);
  CYC(b_+75, b_+77); A = 0x81;
  CYC(b_+77, b_+80); mem_wr(gb, wLinkInAir, A);
  CYC(b_+80, b_+81); alu_xor(gb, A);
  RET(b_+81); return;
blocked:
  CYC(b_+82, b_+83); alu_or(gb, D);
  RET(b_+83); return;
}

static void link_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

void s_warpTransition6_00_hook(GB *gb) {
  BASE(warpTransition6_00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); A = mem_rd(gb, wcc50);
  CYC(b_+6, b_+8); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto L_4ba0; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); alu_rrca(gb);
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+16); mem_wr(gb, wcc50, A);
  CYC(b_+16, b_+18); A = 0x09;
  CYC(b_+18, b_+21);
  TAIL(linkSetState);
L_4ba0:
  CYC(b_+21, b_+24); SET_BC(0xfd00);
  CALL_C(b_+24, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+27);
  CYC(b_+27, b_+29); L = 0x06;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x78);
  CYC(b_+31, b_+33); L = 0x0b;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_sub(gb, 0x04);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+39); A = 0x04;
  CALL_C(b_+39, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+42);
  s_warpTransition6_01_hook(gb); return; // falls through
}

void s_warpTransition6_01_hook(GB *gb) {
  BASE(warpTransition6_01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x18;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto hitGround; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+10);
  CALL_C(b_+10, s_specialObjectUpdateAdjacentWallsBitset, SYM(specialObjectUpdateAdjacentWallsBitset), b_+13);
  CYC(b_+13, b_+15); E = 0x10;
  CYC(b_+15, b_+17); A = 0x14;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+21); A = mem_rd(gb, wLinkAngle);
  CYC(b_+21, b_+23); E = 0x09;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CALL_C(b_+24, s_updateLinkDirectionFromAngle, SYM(updateLinkDirectionFromAngle), b_+27);
  CYC(b_+27, b_+30);
  TAIL(specialObjectUpdatePosition);
hitGround:
  CALL_C(b_+30, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+35, b_+38); TAIL_S(bounceLinkOffTrampolineAfterFalling); }
  CYC(b_+35, b_+38);
  CYC(b_+38, b_+41);
  TAIL(initLinkStateAndAnimateStanding);
}

void s_bounceLinkOffTrampolineAfterFalling_hook(GB *gb) {
  BASE(bounceLinkOffTrampolineAfterFalling);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+3);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); B = 0x02;
L_50ab:
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+10); SET_HL(b_+63 /* @offsets */);
  CYC(b_+10, b_+11); link_d_add_a_to_hl(gb, b_+11);
  CYC(b_+11, b_+12); A = C;
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+15); H = 0xce;
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+20); goto L_50c0; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); A = B;
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);
  CYC(b_+22, b_+24); alu_and(gb, 0x03);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+27);
  goto L_50ab;
L_50c0:
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = 0x08;
  CYC(b_+30, b_+31); mem_wr(gb, HL, B);
  CYC(b_+31, b_+32); A = B;
  CYC(b_+32, b_+34); A = alu_swap(gb, A);
  CYC(b_+34, b_+35); alu_rrca(gb);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+39); L = 0x0f;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0xff);
  CYC(b_+41, b_+44); SET_BC(0xfd00);
  CALL_C(b_+44, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+47);
  CYC(b_+47, b_+49); L = 0x10;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x14);
  CYC(b_+51, b_+53); L = 0x04;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x09);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x06);
  CYC(b_+58, b_+60); A = 0x04;
  CYC(b_+60, b_+63);
  TAIL(specialObjectSetAnimation);
}

