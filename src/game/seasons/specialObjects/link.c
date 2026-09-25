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


static uint16_t link_d_jump_table(GB *gb) {
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

// TRANSITION_DEST_TIMEWARP
// Warp in and create a portal. Doesn't update respawn. Ages only.
void s_warpTransition6_hook(GB *gb) {
  BASE(warpTransition6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_d_jump_table(gb));
    if (jt_ == SYM(warpTransition6_00) && hook_is(gb, SYM(warpTransition6_00), s_warpTransition6_00_hook)) { s_warpTransition6_00_hook(gb); return; }
    if (jt_ == SYM(warpTransition6_01) && hook_is(gb, SYM(warpTransition6_01), s_warpTransition6_01_hook)) { s_warpTransition6_01_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// LINK_STATE_TOSSED_BY_GUARDS
void s_linkState0f_hook(GB *gb) {
  BASE(linkState0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_d_jump_table(gb));
    if (jt_ == b_+16) goto substate0;
    if (jt_ == b_+40) goto substate1;
    if (jt_ == b_+59) goto substate2;
    if (jt_ == b_+86) goto substate3;
    if (jt_ == b_+153) goto substate4;
    if (jt_ == b_+171) goto substate5;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+18); L = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x10);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+25); L = 0x08;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CALL_C(b_+27, s_linkCancelAllItemUsageAndClearAdjacentWallsBitset, SYM(linkCancelAllItemUsageAndClearAdjacentWallsBitset), b_+30);
  CYC(b_+30, b_+32); A = 0x01;
  CYC(b_+32, b_+35); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+35, b_+37); A = 0x10;
  CALL_C(b_+37, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+40);
substate1:
  CALL_C(b_+40, s_itemDecCounter1, SYM(itemDecCounter1), b_+43);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto updateObject; }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x5a);
  CYC(b_+47, b_+48); L = alu_dec8(gb, L);
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+51); L = 0x10;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x14);
updateObject:
  CALL_C(b_+53, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+56);
  CYC(b_+56, b_+59);
  TAIL(specialObjectUpdatePositionWithoutTileEdgeAdjust);
substate2:
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = 0x06;
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+64); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+64, b_+66); goto L_5332; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+67); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+67); return;
L_5332:
  CYC(b_+68, b_+69); H = D;
  CYC(b_+69, b_+71); L = 0x0b;
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); alu_cp(gb, 0x74);
  if (!(F & FC)) { CYCT(b_+74, b_+76); goto updateObject; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); L = 0x05;
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+79, b_+80); L = alu_inc8(gb, L);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x60);
  CYC(b_+82, b_+84); L = 0x10;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x28);
substate3:
  CALL_C(b_+86, s_itemDecCounter1, SYM(itemDecCounter1), b_+89);
  if (F & FZ) { CYCT(b_+89, b_+91); goto L_5375; }
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+92); A = mem_rd(gb, HL);
  CYC(b_+92, b_+94); alu_sub(gb, 0x19);
  if (F & FC) { CYCT(b_+94, b_+96); goto L_535f; }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); alu_cp(gb, 0x32);
  if (!(F & FC)) { RET_TAKEN(b_+98); return; }
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+101); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+101); return; }
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); A = alu_swap(gb, A);
  CYC(b_+105, b_+107); alu_and(gb, 0x01);
  CYC(b_+107, b_+108); alu_add(gb, A);
  CYC(b_+108, b_+109); A = alu_inc8(gb, A);
  CYC(b_+109, b_+111); L = 0x08;
  CYC(b_+111, b_+112); mem_wr(gb, HL, A);
  RET(b_+112); return;
L_535f:
  CYC(b_+113, b_+114); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+114); return; }
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+117); L = 0x08;
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x00);
  CYC(b_+119, b_+120); L = alu_inc8(gb, L);
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x10);
  CYC(b_+122, b_+124); A = 0x18;
  CYC(b_+124, b_+127); SET_BC(0xf4f8);
  CALL_C(b_+127, s_objectCreateExclamationMark, SYM(objectCreateExclamationMark), b_+130);
  CYC(b_+130, b_+132); A = 0x50;
  CYC(b_+132, b_+135);
  TAIL(playSound_b00);
L_5375:
  CYC(b_+135, b_+136); L = E;
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+137, b_+140); SET_BC(0xfe80);
  CALL_C(b_+140, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+143);
  CYC(b_+143, b_+145); A = 0x18;
  CALL_C(b_+145, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+148);
  CYC(b_+148, b_+150); A = 0x53;
  CALL_C(b_+150, s_playSound, SYM(playSound_b00), b_+153);
substate4:
  CYC(b_+153, b_+155); C = 0x18;
  CALL_C(b_+155, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+158);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto updateObject; }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); L = 0x05;
  CYC(b_+162, b_+163); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+163, b_+164); L = alu_inc8(gb, L);
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0xf0);
  CYC(b_+166, b_+168); A = 0x10;
  CALL_C(b_+168, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+171);
substate5:
  CYC(b_+171, b_+174); A = mem_rd(gb, wFrameCounter);
  CYC(b_+174, b_+175); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+175); return; }
  CYC(b_+175, b_+176);
  CALL_C(b_+176, s_itemDecCounter1, SYM(itemDecCounter1), b_+179);
  if (!(F & FZ)) { RET_TAKEN(b_+179); return; }
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+181); alu_xor(gb, A);
  CYC(b_+181, b_+184); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+184, b_+187);
  TAIL(initLinkStateAndAnimateStanding);
}

// linkState09@seasonsFunc_05_5043
static void link_d_seasonsFunc_05_5043(GB *gb) {
  BASE(linkState09);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+127, b_+129); C = 0x0c;
  CALL_C(b_+129, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+132);
  CALL_C(b_+132, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+135);
  CYC(b_+135, b_+136); H = D;
  CYC(b_+136, b_+138); L = 0x06;
  CYC(b_+138, b_+139); A = mem_rd(gb, HL);
  CYC(b_+139, b_+140); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+140, b_+142); goto L_505a; }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+143); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+143, b_+145); goto L_505a; }
  CYC(b_+143, b_+145);
  CYC(b_+145, b_+147); A = 0x04;
  CALL_C(b_+147, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+150);
L_505a:
  CALL_C(b_+150, s_objectGetZAboveScreen, SYM(objectGetZAboveScreen), b_+153);
  CYC(b_+153, b_+154); H = D;
  CYC(b_+154, b_+156); L = 0x0f;
  CYC(b_+156, b_+157); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+157); return;
}

// LINK_STATE_AMBI_POSSESSED_CUTSCENE
// This state is used during the cutscene in the black tower where Ambi gets un-possessed.
void s_linkState09_hook(GB *gb) {
  BASE(linkState09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+5); E = 0x05;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (link_d_jump_table(gb));
    if (jt_ == b_+21) goto substate0;
    if (jt_ == b_+59) goto substate1;
    if (jt_ == b_+101) goto substate2;
    if (jt_ == b_+158) goto substate3;
    if (jt_ == b_+170) goto substate4;
    if (jt_ == b_+195) goto substate5;
    if (jt_ == b_+206) goto substate6;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+21, s_clearAllParentItems, SYM(clearAllParentItems), b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+28); mem_wr(gb, wScrollMode, A);
  CYC(b_+28, b_+31); mem_wr(gb, wUsingShield, A);
  CYC(b_+31, b_+34); SET_BC(0xfc00);
  CALL_C(b_+34, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+37);
  CYC(b_+37, b_+39); L = 0x06;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0a);
  CYC(b_+41, b_+44); A = mem_rd(gb, wcc50);
  CYC(b_+44, b_+45); alu_rrca(gb);
  CYC(b_+45, b_+47); A = 0x01;
  if (!(F & FC)) { CYCT(b_+47, b_+49); goto L_4ff6; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); A = alu_inc8(gb, A);
L_4ff6:
  CYC(b_+50, b_+52); L = 0x05;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+55); A = 0x81;
  CYC(b_+55, b_+58); mem_wr(gb, wLinkInAir, A);
  RET(b_+58); return;
substate1:
  CALL_L(b_+59, link_d_seasonsFunc_05_5043, b_+62);
  if (F & FC) { RET_TAKEN(b_+62); return; }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+66); A = mem_rd(gb, wDungeonFloor);
  CYC(b_+66, b_+67); A = alu_inc8(gb, A);
  CYC(b_+67, b_+70); mem_wr(gb, wDungeonFloor, A);
  CALL_C(b_+70, s_getActiveRoomFromDungeonMapPosition, SYM(getActiveRoomFromDungeonMapPosition), b_+73);
  CYC(b_+73, b_+76); mem_wr(gb, wWarpDestRoom, A);
  CALL_C(b_+76, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+79);
  CYC(b_+79, b_+82); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+82, b_+85); A = mem_rd(gb, wActiveGroup);
  CYC(b_+85, b_+87); alu_or(gb, 0x80);
  CYC(b_+87, b_+90); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+90, b_+92); A = 0x06;
  CYC(b_+92, b_+95); mem_wr(gb, wWarpTransition, A);
  CYC(b_+95, b_+97); A = 0x03;
  CYC(b_+97, b_+100); mem_wr(gb, wWarpTransition2, A);
  RET(b_+100); return;
substate2:
  CALL_L(b_+101, link_d_seasonsFunc_05_5043, b_+104);
  if (F & FC) { RET_TAKEN(b_+104); return; }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+107); A = 0x01;
  CYC(b_+107, b_+110); mem_wr(gb, wScrollMode, A);
  CYC(b_+110, b_+112); L = 0x05;
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+113, b_+115); L = 0x06;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x1e);
  CYC(b_+117, b_+119); A = 0x08;
  CALL_C(b_+119, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+122);
  CYC(b_+122, b_+124); A = 0x02;
  CYC(b_+124, b_+127);
  TAIL(specialObjectSetAnimation);

substate3:
  CALL_C(b_+158, s_itemDecCounter1, SYM(itemDecCounter1), b_+161);
  if (!(F & FZ)) { RET_TAKEN(b_+161); return; }
  CYC(b_+161, b_+162);
  CYC(b_+162, b_+163); L = alu_dec8(gb, L);
  CYC(b_+163, b_+164); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+164, b_+167); SET_BC(IO_P1);
  CYC(b_+167, b_+170);
  TAIL(objectSetSpeedZ);
substate4:
  CYC(b_+170, b_+172); C = 0x20;
  CALL_C(b_+172, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+175);
  if (!(F & FZ)) { RET_TAKEN(b_+175); return; }
  CYC(b_+175, b_+176);
  CALL_C(b_+176, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+179);
  CYC(b_+179, b_+181); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+181, b_+183); TAIL_S(bounceLinkOffTrampolineAfterFalling); }
  CYC(b_+181, b_+183);
  CYC(b_+183, b_+184); H = D;
  CYC(b_+184, b_+186); L = 0x05;
  CYC(b_+186, b_+187); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+187, b_+188); L = alu_inc8(gb, L);
  CYC(b_+188, b_+190); mem_wr(gb, HL, 0x1e);
  CYC(b_+190, b_+192); A = 0x02;
  CALL_C(b_+192, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+195);
substate5:
  CALL_C(b_+195, s_itemDecCounter1, SYM(itemDecCounter1), b_+198);
  if (!(F & FZ)) { RET_TAKEN(b_+198); return; }
  CYC(b_+198, b_+199);
L_508b:
  CYC(b_+199, b_+200); alu_xor(gb, A);
  CYC(b_+200, b_+203); mem_wr(gb, wLinkInAir, A);
  CYC(b_+203, b_+206);
  TAIL(initLinkStateAndAnimateStanding);
substate6:
  CALL_C(b_+206, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+209);
  CALL_C(b_+209, s_specialObjectUpdateAdjacentWallsBitset, SYM(specialObjectUpdateAdjacentWallsBitset), b_+212);
  CYC(b_+212, b_+214); C = 0x20;
  CALL_C(b_+214, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+217);
  if (!(F & FZ)) { CYCT(b_+217, b_+220); TAIL(specialObjectUpdatePosition); }
  CYC(b_+217, b_+220);
  CALL_C(b_+220, s_updateLinkLocalRespawnPosition, SYM(updateLinkLocalRespawnPosition), b_+223);
  CYC(b_+223, b_+225);
  goto L_508b;
}

void s_linkState10_hook(GB *gb) {
  BASE(linkState10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_d_jump_table(gb));
    if (jt_ == b_+10) goto substate0;
    if (jt_ == b_+33) goto substate1;
    if (jt_ == b_+81) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, s_linkCancelAllItemUsage, SYM(linkCancelAllItemUsage), b_+16);
  CALL_C(b_+16, s_resetLinkInvincibility, SYM(resetLinkInvincibility), b_+19);
  CYC(b_+19, b_+21); L = 0x10;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x14);
  CYC(b_+23, b_+25); L = 0x08;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x00);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x00);
  CYC(b_+30, b_+33);
  TAIL(animateLinkStanding);
substate1:
  CALL_C(b_+33, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+36);
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+40); A = mem_rd(gb, wFrameCounter);
  CYC(b_+40, b_+42); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto L_53dd; }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); L = 0x10;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+49); alu_sub(gb, 0x05);
  if (F & FZ) { CYCT(b_+49, b_+51); goto L_53dd; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
L_53dd:
  CYC(b_+52, b_+55); A = mem_rd(gb, wTmpcbb3);
  CYC(b_+55, b_+57); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+57, b_+60); TAIL(specialObjectUpdatePosition); }
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+63); A = mem_rd(gb, wCutsceneState);
  CYC(b_+63, b_+64); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+64, b_+67); TAIL(initLinkStateAndAnimateStanding); }
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+69); L = 0x05;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+70, b_+71); L = alu_inc8(gb, L);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x20);
  CYC(b_+73, b_+75); L = 0x09;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x10);
  CYC(b_+77, b_+79); L = 0x10;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x50);
substate2:
  CALL_C(b_+81, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+84);
  CALL_C(b_+84, s_itemDecCounter1, SYM(itemDecCounter1), b_+87);
  if (!(F & FZ)) { CYCT(b_+87, b_+90); TAIL(specialObjectUpdatePosition); }
  CYC(b_+87, b_+90);
  CYC(b_+90, b_+93); SET_HL(wTmpcbb3);
  CYC(b_+93, b_+94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+94, b_+96); A = 0x02;
  CALL_C(b_+96, s_fadeoutToWhiteWithDelay, SYM(fadeoutToWhiteWithDelay), b_+99);
  CYC(b_+99, b_+102);
  TAIL(initLinkStateAndAnimateStanding);
}

// State 3: the normal state when swimming
void s_overworldSwimmingState3_hook(GB *gb) {
  BASE(overworldSwimmingState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_linkUpdateDiving_hook, SYM(linkUpdateDiving), b_+3);
  CALL_C(b_+3, s_objectSetVisiblec1, SYM(objectSetVisiblec1), b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x24;
  CYC(b_+9, b_+11); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+11, b_+14); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+14, b_+15); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+15, b_+17); goto L_56b1; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+19, s_objectSetVisiblec3, SYM(objectSetVisiblec3), b_+22);
L_56b1:
  CALL_C(b_+22, s_updateLinkDirectionFromAngle, SYM(updateLinkDirectionFromAngle), b_+25);
  CALL_C(b_+25, s_linkUpdateFlippersSpeed, SYM(linkUpdateFlippersSpeed), b_+28);
  CALL_C(b_+28, s_func_5933, SYM(func_5933), b_+31);
  CYC(b_+31, b_+34);
  TAIL(specialObjectUpdatePosition);
}

void s_linkUpdateDiving_hook(GB *gb) {
  BASE(linkUpdateDiving);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+6); SET_HL(wLinkSwimmingState);
  CYC(b_+6, b_+9); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+9, b_+11); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto pressedB; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0x3c;
  CALL_C(b_+15, s_cpActiveRing, SYM(cpActiveRing), b_+18);
  if (F & FZ) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); E = 0x07;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); A = alu_dec8(gb, A);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto surface; }
  CYC(b_+24, b_+26);
  RET(b_+26); return;
pressedB:
  CYC(b_+27, b_+29); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+29, b_+31); goto dive; }
  CYC(b_+29, b_+31);
surface:
  CYC(b_+31, b_+33); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+33, b_+35); A = 0x0b;
  CYC(b_+35, b_+38);
  TAIL(specialObjectSetAnimation);
dive:
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+40, b_+42); E = 0x07;
  CYC(b_+42, b_+44); A = 0x78;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CALL_C(b_+45, s_linkCreateSplash, SYM(linkCreateSplash), b_+48);
  CYC(b_+48, b_+50); A = 0x0c;
  CYC(b_+50, b_+53);
  TAIL(specialObjectSetAnimation);
}

// Updates speed and angle for things like ice, jumping, underwater? (Things where he
// accelerates and decelerates)
void s_linkUpdateVelocity_hook(GB *gb) {
  BASE(linkUpdateVelocity);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkAngle);
  s_func_5933(gb); return; // falls through
}
