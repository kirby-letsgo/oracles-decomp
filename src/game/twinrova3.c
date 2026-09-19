#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/twinrova3.s (interactionCodebc /
// INTERAC_TWINROVA_3), bank 0x0b. Distinct from src/game/twinrova.c (bank 0x10) and
// src/game/twinrovaInteraction.c (bank 0x0a) -- verified via this file's own disassembly path and
// bank.
//
// Variables:
//   var3c: A target position index for the data in var3e/var3f.
//   var3d: # of values in the position list (var3c must stop here).
//   var3e/var3f: A pointer to a list of target positions.

static uint16_t interactionCodebc_jump_table(GB *gb) {
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

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodebc_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodebc_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCodebc@loadOamFlags (0b:6f2f): true local, always called with a genuine return
// expected, never dispatches into another hook -- a plain helper with no push/ret_effect at its
// own boundary is correct and safe (matches src/game/ball.c's func_6b00 precedent).
static void interactionCodebc_loadOamFlags(GB *gb) {
  CYC(0x6f2f, 0x6f31); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6f31, 0x6f32); A = mem_rd(gb, DE);
  CYC(0x6f32, 0x6f35); SET_HL(0x6f3b); // @oamFlags data table
  CYC(0x6f35, 0x6f36); interactionCodebc_addAToHl_from_rst(gb, 0x6f36);
  CYC(0x6f36, 0x6f37); A = mem_rd(gb, HL);
  CYC(0x6f37, 0x6f39); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x6f39, 0x6f3a); mem_wr(gb, DE, A);
  CYC(0x6f3a, 0x6f3b); return; // ret
}

// interactionCodebc@updateFloating (0b:6f3f): true local, same reasoning as @loadOamFlags.
static void interactionCodebc_updateFloating(GB *gb) {
  CYC(0x6f3f, 0x6f42); A = W8(wFrameCounter);
  CYC(0x6f42, 0x6f44); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6f44, 0x6f45); return; } // ret nz
  CYC(0x6f44, 0x6f45);
  CYC(0x6f45, 0x6f48); A = W8(wFrameCounter);
  CYC(0x6f48, 0x6f4a); alu_and(gb, 0x38);
  CYC(0x6f4a, 0x6f4c); alu_swap_a(gb);
  CYC(0x6f4c, 0x6f4d); alu_rlca(gb);
  CYC(0x6f4d, 0x6f50); SET_HL(0x6f57); // @zValues data table
  CYC(0x6f50, 0x6f51); interactionCodebc_addAToHl_from_rst(gb, 0x6f51);
  CYC(0x6f51, 0x6f53); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6f53, 0x6f54); A = mem_rd(gb, DE);
  CYC(0x6f54, 0x6f55); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6f55, 0x6f56); mem_wr(gb, DE, A);
  CYC(0x6f56, 0x6f57); return; // ret
}

// interactionCodebc@commonInit (0b:6e33): true local (uses CALL_C internally, so it takes sp0_,
// but its own final ret needs no push/ret_effect since it never dispatches into another hook).
static void interactionCodebc_commonInit(GB *gb, uint16_t sp0_) {
  CALL_C(0x6e33, interactionInitGraphics_hook, 0x15fb, 0x6e36); // SWITCHES THREADS
  CALL_C(0x6e36, objectSetVisiblec0_hook, 0x1e33, 0x6e39);
  CALL_C(0x6e39, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6e3c);
  CYC(0x6e3c, 0x6e3f); interactionCodebc_loadOamFlags(gb); // call
  CALL_C(0x6e3f, interactionIncState_hook, 0x23e0, 0x6e42);
  CYC(0x6e42, 0x6e44); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6e44, 0x6e46); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x6e46, 0x6e48); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6e48, 0x6e4a); mem_wr(gb, HL, 0xf8);
  CYC(0x6e4a, 0x6e4c); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6e4c, 0x6e4e); mem_wr(gb, HL, 0xff);
  CYC(0x6e4e, 0x6e4f); return; // ret
}

// interactionCodebc@readPositionTable (0b:6fd7): true local.
static void interactionCodebc_readPositionTable(GB *gb) {
  CYC(0x6fd7, 0x6fd8); alu_add(gb, A);
  CYC(0x6fd8, 0x6fdb); SET_HL(0x6fe9); // @table
  CYC(0x6fdb, 0x6fdc); interactionCodebc_add_double_index(gb, 0x6fdc);
  CYC(0x6fdc, 0x6fde); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6fde, 0x6fdf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6fdf, 0x6fe0); mem_wr(gb, DE, A);
  CYC(0x6fe0, 0x6fe2); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6fe2, 0x6fe3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6fe3, 0x6fe4); mem_wr(gb, DE, A);
  CYC(0x6fe4, 0x6fe6); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x6fe6, 0x6fe7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6fe7, 0x6fe8); mem_wr(gb, DE, A);
  CYC(0x6fe8, 0x6fe9); return; // ret

  // interactionCodebc@table (0b:6fe9) and @positions0-5: pure ROM data, not ported as code --
  // referenced only via SET_HL(0x6fe9) above.
}

// interactionCodebc@getCurrentPositionPointer (0b:6fc4): true local (uses CALL_C, takes sp0_).
static void interactionCodebc_getCurrentPositionPointer(GB *gb, uint16_t sp0_) {
  CYC(0x6fc4, 0x6fc5); H = D;
  CYC(0x6fc5, 0x6fc7); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6fc7, 0x6fc8); A = mem_rd(gb, HL);
  CYC(0x6fc8, 0x6fc9); alu_add(gb, A);
  CYC(0x6fc9, 0x6fca); push_effect(gb, (uint16_t)((A << 8) | F)); // push af
  CYC(0x6fca, 0x6fcc); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6fcc, 0x6fcd); A = mem_rd(gb, DE);
  CYC(0x6fcd, 0x6fce); C = A;
  CYC(0x6fce, 0x6fd0); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6fd0, 0x6fd1); A = mem_rd(gb, DE);
  CYC(0x6fd1, 0x6fd2); B = A;
  CYC(0x6fd2, 0x6fd3); { uint16_t af = pop_effect(gb); A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff); } // pop af
  CALL_C(0x6fd3, addAToBc_hook, 0x006d, 0x6fd6);
  CYC(0x6fd6, 0x6fd7); return; // ret
}

// interactionCodebc@checkReachedTargetPosition (0b:6f7b): true local (passes sp0_ through).
static void interactionCodebc_checkReachedTargetPosition(GB *gb, uint16_t sp0_) {
  CYC(0x6f7b, 0x6f7e); interactionCodebc_getCurrentPositionPointer(gb, sp0_); // call
  CYC(0x6f7e, 0x6f80); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6f80, 0x6f81); A = mem_rd(gb, BC);
  CYC(0x6f81, 0x6f82); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6f82, 0x6f84); alu_add(gb, 0x01);
  CYC(0x6f84, 0x6f86); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x6f86, 0x6f87); return; } // ret nc
  CYC(0x6f86, 0x6f87);
  CYC(0x6f87, 0x6f88); SET_BC(BC + 1);
  CYC(0x6f88, 0x6f8a); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6f8a, 0x6f8b); A = mem_rd(gb, BC);
  CYC(0x6f8b, 0x6f8c); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x6f8c, 0x6f8e); alu_add(gb, 0x01);
  CYC(0x6f8e, 0x6f90); alu_cp(gb, 0x05);
  CYC(0x6f90, 0x6f91); return; // ret
}

// interactionCodebc@updateAnimationIndex (0b:6f91): tail-jumps into the already-hooked
// interactionSetAnimation on its main path, so (like interactionCode8a_checkEssenceObtained in
// src/game/remoteMakuCutscene.c) it takes an explicit return_address and pushes it at entry; its
// early "ret z" pops that same value via ret_effect for consistency.
static void interactionCodebc_updateAnimationIndex(GB *gb, uint16_t sp0_, uint16_t return_address) {
  (void)sp0_;
  push_effect(gb, return_address);
  CYC(0x6f91, 0x6f92); H = D;
  CYC(0x6f92, 0x6f94); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6f94, 0x6f95); A = mem_rd(gb, HL);
  CYC(0x6f95, 0x6f97); alu_swap_a(gb);
  CYC(0x6f97, 0x6f99); alu_and(gb, 0x01);
  CYC(0x6f99, 0x6f9b); alu_xor(gb, 0x01);
  CYC(0x6f9b, 0x6f9d); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6f9d, 0x6f9e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6f9e, 0x6f9f); ret_effect(gb); return; } // ret z
  CYC(0x6f9e, 0x6f9f);
  CYC(0x6f9f, 0x6fa0); mem_wr(gb, HL, A);
  CYC(0x6fa0, 0x6fa3); interactionSetAnimation_hook(gb); // jp
}

// interactionCodebc@nextTargetPosition@setPositionToPointerData (0b:6fb3): true local.
static void interactionCodebc_setPositionToPointerData(GB *gb, uint16_t sp0_) {
  CYC(0x6fb3, 0x6fb6); interactionCodebc_getCurrentPositionPointer(gb, sp0_); // call
  CYC(0x6fb6, 0x6fb8); L = INTERACTION_BASE + OBJ_Y;
  CYC(0x6fb8, 0x6fb9); alu_xor(gb, A);
  CYC(0x6fb9, 0x6fba); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6fba, 0x6fbb); A = mem_rd(gb, BC);
  CYC(0x6fbb, 0x6fbc); mem_wr(gb, HL, A);
  CYC(0x6fbc, 0x6fbd); SET_BC(BC + 1);
  CYC(0x6fbd, 0x6fbf); L = INTERACTION_BASE + OBJ_X;
  CYC(0x6fbf, 0x6fc0); alu_xor(gb, A);
  CYC(0x6fc0, 0x6fc1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6fc1, 0x6fc2); A = mem_rd(gb, BC);
  CYC(0x6fc2, 0x6fc3); mem_wr(gb, HL, A);
  CYC(0x6fc3, 0x6fc4); return; // ret
}

// interactionCodebc@nextTargetPosition (0b:6fa3): true local.
static void interactionCodebc_nextTargetPosition(GB *gb, uint16_t sp0_) {
  CYC(0x6fa3, 0x6fa6); interactionCodebc_setPositionToPointerData(gb, sp0_); // call
  CYC(0x6fa6, 0x6fa7); H = D;
  CYC(0x6fa7, 0x6fa9); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x6fa9, 0x6faa); A = mem_rd(gb, HL);
  CYC(0x6faa, 0x6fac); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6fac, 0x6fad); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6fad, 0x6fae); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x6fae, 0x6faf); return; } // ret nc
  CYC(0x6fae, 0x6faf);
  CYC(0x6faf, 0x6fb1); mem_wr(gb, HL, 0x00);
  CYC(0x6fb1, 0x6fb2); alu_scf(gb);
  CYC(0x6fb2, 0x6fb3); return; // ret
}

// interactionCodebc@moveTowardTargetPosition (0b:6f5f): tail-jumps into the already-hooked
// objectApplySpeed, so it takes an explicit return_address (like @updateAnimationIndex above).
static void interactionCodebc_moveTowardTargetPosition(GB *gb, uint16_t sp0_, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x6f5f, 0x6f60); H = D;
  CYC(0x6f60, 0x6f62); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6f62, 0x6f63); A = mem_rd(gb, HL);
  CYC(0x6f63, 0x6f64); alu_add(gb, A);
  CYC(0x6f64, 0x6f65); B = A;
  CYC(0x6f65, 0x6f67); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6f67, 0x6f68); A = mem_rd(gb, DE);
  CYC(0x6f68, 0x6f69); L = A;
  CYC(0x6f69, 0x6f6b); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6f6b, 0x6f6c); A = mem_rd(gb, DE);
  CYC(0x6f6c, 0x6f6d); H = A;
  CYC(0x6f6d, 0x6f6e); A = B;
  CYC(0x6f6e, 0x6f6f); interactionCodebc_addAToHl_from_rst(gb, 0x6f6f);
  CYC(0x6f6f, 0x6f70); B = mem_rd(gb, HL);
  CYC(0x6f70, 0x6f71); SET_HL(HL + 1);
  CYC(0x6f71, 0x6f72); C = mem_rd(gb, HL);
  CALL_C(0x6f72, objectGetRelativeAngle_hook, 0x1ea4, 0x6f75);
  CYC(0x6f75, 0x6f77); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6f77, 0x6f78); mem_wr(gb, DE, A);
  CYC(0x6f78, 0x6f7b); objectApplySpeed_hook(gb); // jp
}

// interactionCodebc@animate (0b:6f2c): shared by both direct tail-jumps ("jp"/"jr" -- handled
// inline below with no push, ending the whole hook) and genuine `call`s from @subid0Substate1,
// @subid0Substate2 and @subid2State1, each with different code following (needs its own
// return_address, same trampoline shape as @updateAnimationIndex above).
static void interactionCodebc_animate(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x6f2c, 0x6f2f); interactionAnimate_hook(gb); // jp
}

// interactionCodebc@func_6eac (0b:6eac): true local, no CALL_C, reached both by plain fallthrough
// from @subid0Substate1 (no call at all) and by genuine `call` from @subid0Substate3@looped
// (twice); either way a plain C function call/return reproduces it correctly.
static void interactionCodebc_func_6eac(GB *gb) {
  CYC(0x6eac, 0x6eaf); SET_HL(wTmpcfc0_genericCutscene_cfc6);
  CYC(0x6eaf, 0x6eb0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6eb0, 0x6eb1); A = mem_rd(gb, HL);
  CYC(0x6eb1, 0x6eb3); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x6eb3, 0x6eb4); return; } // ret nz
  CYC(0x6eb3, 0x6eb4);
  CYC(0x6eb4, 0x6eb6); mem_wr(gb, HL, 0x00);
  CYC(0x6eb6, 0x6eb9); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x6eb9, 0x6ebb); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01); // set 0,(hl)
  CYC(0x6ebb, 0x6ebc); return; // ret
}

// ==================================================================================================
// INTERAC_TWINROVA_3
// ==================================================================================================
void interactionCodebc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6deb, 0x6ded); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6ded, 0x6dee); A = mem_rd(gb, DE);
  {
    CYC(0x6dee, 0x6def); push_effect(gb, 0x6def);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == 0x6e4f) goto state1;
    // target == 0x6df3 falls through to state0
  }

  // interactionCodebc@state0
  CYC(0x6df3, 0x6df5); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6df5, 0x6df6); A = mem_rd(gb, DE);
  {
    CYC(0x6df6, 0x6df7); push_effect(gb, 0x6df7);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == 0x6e0f) goto initSubid2;
    if (target == 0x6e23) goto initSubid3;
    // target == 0x6dff falls through to initSubid0/initSubid1 (aliased)
  }

  // interactionCodebc@initSubid0 / @initSubid1
  CYC(0x6dff, 0x6e02); interactionCodebc_commonInit(gb, sp0_); // call
  CYC(0x6e02, 0x6e04); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6e04, 0x6e06); mem_wr(gb, HL, 0xfb);
  CYC(0x6e06, 0x6e08); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6e08, 0x6e09); A = mem_rd(gb, HL);
  CYC(0x6e09, 0x6e0c); interactionCodebc_readPositionTable(gb); // call
  CYC(0x6e0c, 0x6e0f); goto state1; // jp

initSubid2: // interactionCodebc@initSubid2
  CYC(0x6e0f, 0x6e12); interactionCodebc_commonInit(gb, sp0_); // call
  CYC(0x6e12, 0x6e14); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6e14, 0x6e16); mem_wr(gb, HL, 0xf0);
  CYC(0x6e16, 0x6e18); A = 0x02;
  CYC(0x6e18, 0x6e1b); interactionCodebc_readPositionTable(gb); // call
  CYC(0x6e1b, 0x6e1d); A = 0x04;
  CALL_C(0x6e1d, interactionSetAnimation_hook, 0x262e, 0x6e20);
  CYC(0x6e20, 0x6e23); goto state1; // jp

initSubid3: // interactionCodebc@initSubid3
  CYC(0x6e23, 0x6e26); interactionCodebc_commonInit(gb, sp0_); // call
  CYC(0x6e26, 0x6e28); A = 0x02;
  CYC(0x6e28, 0x6e2b); interactionCodebc_readPositionTable(gb); // call
  CYC(0x6e2b, 0x6e2d); A = 0x01;
  CALL_C(0x6e2d, interactionSetAnimation_hook, 0x262e, 0x6e30);
  CYC(0x6e30, 0x6e33); goto state1; // jp

state1: // interactionCodebc@state1
  CYC(0x6e4f, 0x6e51); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6e51, 0x6e52); A = mem_rd(gb, DE);
  {
    CYC(0x6e52, 0x6e53); push_effect(gb, 0x6e53);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == 0x6f11) goto subid2State1;
    // target == 0x6e5b falls through to subid0State1 (subid0/subid1 aliased, subid2/subid3 aliased)
  }

  // interactionCodebc@subid0State1
  CYC(0x6e5b, 0x6e5d); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6e5d, 0x6e5e); A = mem_rd(gb, DE);
  {
    CYC(0x6e5e, 0x6e5f); push_effect(gb, 0x6e5f);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == 0x6e9b) goto subid0Substate1;
    if (target == 0x6ebc) goto subid0Substate2;
    if (target == 0x6edb) goto subid0Substate3;
    if (target == 0x6f0e) { CYC(0x6f0e, 0x6f11); interactionAnimate_hook(gb); return; } // subid0Substate4: jp @animate
    // target == 0x6e69 falls through to subid0Substate0
  }

  // interactionCodebc@subid0Substate0
  CYC(0x6e69, 0x6e6c); interactionCodebc_moveTowardTargetPosition(gb, sp0_, 0x6e6c); // call
  CYC(0x6e6c, 0x6e6f); interactionCodebc_updateAnimationIndex(gb, sp0_, 0x6e6f); // call
  CYC(0x6e6f, 0x6e72); interactionCodebc_checkReachedTargetPosition(gb, sp0_); // call
  if (F & FC) { CYCT(0x6e72, 0x6e75); interactionCodebc_nextTargetPosition(gb, sp0_); } else { CYC(0x6e72, 0x6e75); } // call c
  if (!(F & FC)) { CYCT(0x6e75, 0x6e78); interactionAnimate_hook(gb); return; } // jp nc @animate
  CYC(0x6e75, 0x6e78);
  CYC(0x6e78, 0x6e79); H = D;
  CYC(0x6e79, 0x6e7b); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6e7b, 0x6e7d); mem_wr(gb, HL, 0x01);
  CYC(0x6e7d, 0x6e7f); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6e7f, 0x6e81); mem_wr(gb, HL, 40);
  CYC(0x6e81, 0x6e83); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6e83, 0x6e84); A = mem_rd(gb, HL);
  CYC(0x6e84, 0x6e85); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6e85, 0x6e87); goto l6e8b; } // jr nz
  CYC(0x6e85, 0x6e87);
  CYC(0x6e87, 0x6e89); A = 0x00;
  CYC(0x6e89, 0x6e8b); goto l6e95; // jr

l6e8b:
  CYC(0x6e8b, 0x6e8d); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x6e8d, 0x6e8f); goto l6e93; } // jr nz
  CYC(0x6e8d, 0x6e8f);
  CYC(0x6e8f, 0x6e91); A = 0x01;
  CYC(0x6e91, 0x6e93); goto l6e95; // jr

l6e93:
  CYC(0x6e93, 0x6e95); A = 0x02;

l6e95:
  CALL_C(0x6e95, interactionSetAnimation_hook, 0x262e, 0x6e98);
  CYC(0x6e98, 0x6e9b); interactionAnimate_hook(gb); return; // jp @animate

subid0Substate1: // interactionCodebc@subid0Substate1
  CYC(0x6e9b, 0x6e9e); interactionCodebc_updateFloating(gb); // call
  CYC(0x6e9e, 0x6ea1); interactionCodebc_animate(gb, 0x6ea1); // call @animate
  CALL_C(0x6ea1, interactionDecCounter2_hook, 0x23d1, 0x6ea4);
  if (!(F & FZ)) { CYCT(0x6ea4, 0x6ea5); ret_effect(gb); return; } // ret nz
  CYC(0x6ea4, 0x6ea5);
  CYC(0x6ea5, 0x6ea7); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6ea7, 0x6ea8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ea8, 0x6eaa); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6eaa, 0x6eac); mem_wr(gb, HL, 40);
  interactionCodebc_func_6eac(gb); return; // falls through into @func_6eac, no call

subid0Substate2: // interactionCodebc@subid0Substate2
  CYC(0x6ebc, 0x6ebf); interactionCodebc_updateFloating(gb); // call
  CYC(0x6ebf, 0x6ec2); interactionCodebc_animate(gb, 0x6ec2); // call @animate
  CYC(0x6ec2, 0x6ec5); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x6ec5, 0x6ec7); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6ec7, 0x6ec8); ret_effect(gb); return; } // ret nz
  CYC(0x6ec7, 0x6ec8);
  CALL_C(0x6ec8, interactionDecCounter2_hook, 0x23d1, 0x6ecb);
  if (!(F & FZ)) { CYCT(0x6ecb, 0x6ecc); ret_effect(gb); return; } // ret nz
  CYC(0x6ecb, 0x6ecc);
  CYC(0x6ecc, 0x6ece); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6ece, 0x6ecf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ecf, 0x6ed1); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6ed1, 0x6ed3); mem_wr(gb, HL, 0xff);
  CYC(0x6ed3, 0x6ed5); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6ed5, 0x6ed6); A = mem_rd(gb, HL);
  CYC(0x6ed6, 0x6ed8); alu_add(gb, 0x04);
  CYC(0x6ed8, 0x6edb); interactionCodebc_readPositionTable(gb); return; // jp

subid0Substate3: // interactionCodebc@subid0Substate3
  CYC(0x6edb, 0x6ede); interactionCodebc_moveTowardTargetPosition(gb, sp0_, 0x6ede); // call
  CYC(0x6ede, 0x6ee1); interactionCodebc_checkReachedTargetPosition(gb, sp0_); // call
  if (F & FC) { CYCT(0x6ee1, 0x6ee4); interactionCodebc_nextTargetPosition(gb, sp0_); } else { CYC(0x6ee1, 0x6ee4); } // call c
  if (F & FC) { CYCT(0x6ee4, 0x6ee6); goto looped; } // jr c
  CYC(0x6ee4, 0x6ee6);
  CYC(0x6ee6, 0x6ee9); interactionCodebc_moveTowardTargetPosition(gb, sp0_, 0x6ee9); // call
  CYC(0x6ee9, 0x6eeb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6eeb, 0x6eec); A = mem_rd(gb, DE);
  CYC(0x6eec, 0x6eee); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x6eee, 0x6ef1); interactionCodebc_updateAnimationIndex(gb, sp0_, 0x6ef1); } else { CYC(0x6eee, 0x6ef1); } // call nz
  CYC(0x6ef1, 0x6ef4); interactionCodebc_checkReachedTargetPosition(gb, sp0_); // call
  if (F & FC) { CYCT(0x6ef4, 0x6ef7); interactionCodebc_nextTargetPosition(gb, sp0_); } else { CYC(0x6ef4, 0x6ef7); } // call c
  if (!(F & FC)) { CYCT(0x6ef7, 0x6ef9); interactionAnimate_hook(gb); return; } // jr nc @animate
  CYC(0x6ef7, 0x6ef9);

looped: // interactionCodebc@subid0Substate3@looped
  CYC(0x6ef9, 0x6efb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6efb, 0x6efc); A = mem_rd(gb, DE);
  CYC(0x6efc, 0x6efe); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x6efe, 0x6f00); goto l6f06; } // jr c
  CYC(0x6efe, 0x6f00);
  CYC(0x6f00, 0x6f03); interactionCodebc_func_6eac(gb); // call
  CYC(0x6f03, 0x6f06); interactionDelete_hook(gb); return; // jp

l6f06:
  CYC(0x6f06, 0x6f09); interactionCodebc_func_6eac(gb); // call
  CYC(0x6f09, 0x6f0a); H = D;
  CYC(0x6f0a, 0x6f0c); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6f0c, 0x6f0d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f0d, 0x6f0e); ret_effect(gb); return; // ret

subid2State1: // interactionCodebc@subid2State1
  CALL_C(0x6f11, checkInteractionSubstate_hook, 0x2403, 0x6f14);
  if (!(F & FZ)) { CYCT(0x6f14, 0x6f16); goto l6f2a; } // jr nz
  CYC(0x6f14, 0x6f16);
  CYC(0x6f16, 0x6f19); interactionCodebc_updateFloating(gb); // call
  CYC(0x6f19, 0x6f1c); interactionCodebc_animate(gb, 0x6f1c); // call @animate
  CYC(0x6f1c, 0x6f1f); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x6f1f, 0x6f21); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x6f21, 0x6f22); ret_effect(gb); return; } // ret z
  CYC(0x6f21, 0x6f22);
  CALL_C(0x6f22, interactionIncSubstate_hook, 0x23e5, 0x6f25);
  CYC(0x6f25, 0x6f27); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6f27, 0x6f29); mem_wr(gb, HL, 0xff);
  CYC(0x6f29, 0x6f2a); ret_effect(gb); return; // ret

l6f2a:
  CYC(0x6f2a, 0x6f2c); goto subid0Substate3; // jr
}
