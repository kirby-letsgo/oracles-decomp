#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodebc);
  CYC(b_+324, b_+326); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+326, b_+327); A = mem_rd(gb, DE);
  CYC(b_+327, b_+330); SET_HL(b_+336); // @oamFlags data table
  CYC(b_+330, b_+331); interactionCodebc_addAToHl_from_rst(gb, b_+331);
  CYC(b_+331, b_+332); A = mem_rd(gb, HL);
  CYC(b_+332, b_+334); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+334, b_+335); mem_wr(gb, DE, A);
  CYC(b_+335, b_+336); return; // ret
}

// interactionCodebc@updateFloating (0b:6f3f): true local, same reasoning as @loadOamFlags.
static void interactionCodebc_updateFloating(GB *gb) {
  BASE(interactionCodebc);
  CYC(b_+340, b_+343); A = W8(wFrameCounter);
  CYC(b_+343, b_+345); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+345, b_+346); return; } // ret nz
  CYC(b_+345, b_+346);
  CYC(b_+346, b_+349); A = W8(wFrameCounter);
  CYC(b_+349, b_+351); alu_and(gb, 0x38);
  CYC(b_+351, b_+353); alu_swap_a(gb);
  CYC(b_+353, b_+354); alu_rlca(gb);
  CYC(b_+354, b_+357); SET_HL(b_+364); // @zValues data table
  CYC(b_+357, b_+358); interactionCodebc_addAToHl_from_rst(gb, b_+358);
  CYC(b_+358, b_+360); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+360, b_+361); A = mem_rd(gb, DE);
  CYC(b_+361, b_+362); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+362, b_+363); mem_wr(gb, DE, A);
  CYC(b_+363, b_+364); return; // ret
}

// interactionCodebc@commonInit (0b:6e33): true local (uses CALL_C internally, so it takes sp0_,
// but its own final ret needs no push/ret_effect since it never dispatches into another hook).
static void interactionCodebc_commonInit(GB *gb, uint16_t sp0_) {
  BASE(interactionCodebc);
  CALL_C(b_+72, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+75); // SWITCHES THREADS
  CALL_C(b_+75, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+78);
  CALL_C(b_+78, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+81);
  CYC(b_+81, b_+84); interactionCodebc_loadOamFlags(gb); // call
  CALL_C(b_+84, interactionIncState_hook, SYM(interactionIncState), b_+87);
  CYC(b_+87, b_+89); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+91, b_+93); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0xf8);
  CYC(b_+95, b_+97); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0xff);
  CYC(b_+99, b_+100); return; // ret
}

// interactionCodebc@readPositionTable (0b:6fd7): true local.
static void interactionCodebc_readPositionTable(GB *gb) {
  BASE(interactionCodebc);
  CYC(b_+492, b_+493); alu_add(gb, A);
  CYC(b_+493, b_+496); SET_HL(b_+510); // @table
  CYC(b_+496, b_+497); interactionCodebc_add_double_index(gb, b_+497);
  CYC(b_+497, b_+499); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+499, b_+500); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+500, b_+501); mem_wr(gb, DE, A);
  CYC(b_+501, b_+503); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+503, b_+504); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+504, b_+505); mem_wr(gb, DE, A);
  CYC(b_+505, b_+507); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+507, b_+508); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+508, b_+509); mem_wr(gb, DE, A);
  CYC(b_+509, b_+510); return; // ret

  // interactionCodebc@table (0b:6fe9) and @positions0-5: pure ROM data, not ported as code --
  // referenced only via SET_HL(0x6fe9) above.
}

// interactionCodebc@getCurrentPositionPointer (0b:6fc4): true local (uses CALL_C, takes sp0_).
static void interactionCodebc_getCurrentPositionPointer(GB *gb, uint16_t sp0_) {
  BASE(interactionCodebc);
  CYC(b_+473, b_+474); H = D;
  CYC(b_+474, b_+476); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+476, b_+477); A = mem_rd(gb, HL);
  CYC(b_+477, b_+478); alu_add(gb, A);
  CYC(b_+478, b_+479); push_effect(gb, (uint16_t)((A << 8) | F)); // push af
  CYC(b_+479, b_+481); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+481, b_+482); A = mem_rd(gb, DE);
  CYC(b_+482, b_+483); C = A;
  CYC(b_+483, b_+485); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+485, b_+486); A = mem_rd(gb, DE);
  CYC(b_+486, b_+487); B = A;
  CYC(b_+487, b_+488); { uint16_t af = pop_effect(gb); A = (uint8_t)(af >> 8); F = (uint8_t)(af & 0xff); } // pop af
  CALL_C(b_+488, addAToBc_hook, 0x006d, b_+491);
  CYC(b_+491, b_+492); return; // ret
}

// interactionCodebc@checkReachedTargetPosition (0b:6f7b): true local (passes sp0_ through).
static void interactionCodebc_checkReachedTargetPosition(GB *gb, uint16_t sp0_) {
  BASE(interactionCodebc);
  CYC(b_+400, b_+403); interactionCodebc_getCurrentPositionPointer(gb, sp0_); // call
  CYC(b_+403, b_+405); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+405, b_+406); A = mem_rd(gb, BC);
  CYC(b_+406, b_+407); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+407, b_+409); alu_add(gb, 0x01);
  CYC(b_+409, b_+411); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+411, b_+412); return; } // ret nc
  CYC(b_+411, b_+412);
  CYC(b_+412, b_+413); SET_BC(BC + 1);
  CYC(b_+413, b_+415); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+415, b_+416); A = mem_rd(gb, BC);
  CYC(b_+416, b_+417); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+417, b_+419); alu_add(gb, 0x01);
  CYC(b_+419, b_+421); alu_cp(gb, 0x05);
  CYC(b_+421, b_+422); return; // ret
}

// interactionCodebc@updateAnimationIndex (0b:6f91): tail-jumps into the already-hooked
// interactionSetAnimation on its main path, so (like interactionCode8a_checkEssenceObtained in
// src/game/remoteMakuCutscene.c) it takes an explicit return_address and pushes it at entry; its
// early "ret z" pops that same value via ret_effect for consistency.
static void interactionCodebc_updateAnimationIndex(GB *gb, uint16_t sp0_, uint16_t return_address) {
  BASE(interactionCodebc);
  (void)sp0_;
  push_effect(gb, return_address);
  CYC(b_+422, b_+423); H = D;
  CYC(b_+423, b_+425); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+425, b_+426); A = mem_rd(gb, HL);
  CYC(b_+426, b_+428); alu_swap_a(gb);
  CYC(b_+428, b_+430); alu_and(gb, 0x01);
  CYC(b_+430, b_+432); alu_xor(gb, 0x01);
  CYC(b_+432, b_+434); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+434, b_+435); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+435, b_+436); ret_effect(gb); return; } // ret z
  CYC(b_+435, b_+436);
  CYC(b_+436, b_+437); mem_wr(gb, HL, A);
  CYC(b_+437, b_+440); interactionSetAnimation_hook(gb); // jp
}

// interactionCodebc@nextTargetPosition@setPositionToPointerData (0b:6fb3): true local.
static void interactionCodebc_setPositionToPointerData(GB *gb, uint16_t sp0_) {
  BASE(interactionCodebc);
  CYC(b_+456, b_+459); interactionCodebc_getCurrentPositionPointer(gb, sp0_); // call
  CYC(b_+459, b_+461); L = INTERACTION_BASE + OBJ_Y;
  CYC(b_+461, b_+462); alu_xor(gb, A);
  CYC(b_+462, b_+463); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+463, b_+464); A = mem_rd(gb, BC);
  CYC(b_+464, b_+465); mem_wr(gb, HL, A);
  CYC(b_+465, b_+466); SET_BC(BC + 1);
  CYC(b_+466, b_+468); L = INTERACTION_BASE + OBJ_X;
  CYC(b_+468, b_+469); alu_xor(gb, A);
  CYC(b_+469, b_+470); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+470, b_+471); A = mem_rd(gb, BC);
  CYC(b_+471, b_+472); mem_wr(gb, HL, A);
  CYC(b_+472, b_+473); return; // ret
}

// interactionCodebc@nextTargetPosition (0b:6fa3): true local.
static void interactionCodebc_nextTargetPosition(GB *gb, uint16_t sp0_) {
  BASE(interactionCodebc);
  CYC(b_+440, b_+443); interactionCodebc_setPositionToPointerData(gb, sp0_); // call
  CYC(b_+443, b_+444); H = D;
  CYC(b_+444, b_+446); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+446, b_+447); A = mem_rd(gb, HL);
  CYC(b_+447, b_+449); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+449, b_+450); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+450, b_+451); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+451, b_+452); return; } // ret nc
  CYC(b_+451, b_+452);
  CYC(b_+452, b_+454); mem_wr(gb, HL, 0x00);
  CYC(b_+454, b_+455); alu_scf(gb);
  CYC(b_+455, b_+456); return; // ret
}

// interactionCodebc@moveTowardTargetPosition (0b:6f5f): tail-jumps into the already-hooked
// objectApplySpeed, so it takes an explicit return_address (like @updateAnimationIndex above).
static void interactionCodebc_moveTowardTargetPosition(GB *gb, uint16_t sp0_, uint16_t return_address) {
  BASE(interactionCodebc);
  push_effect(gb, return_address);
  CYC(b_+372, b_+373); H = D;
  CYC(b_+373, b_+375); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+375, b_+376); A = mem_rd(gb, HL);
  CYC(b_+376, b_+377); alu_add(gb, A);
  CYC(b_+377, b_+378); B = A;
  CYC(b_+378, b_+380); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+380, b_+381); A = mem_rd(gb, DE);
  CYC(b_+381, b_+382); L = A;
  CYC(b_+382, b_+384); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+384, b_+385); A = mem_rd(gb, DE);
  CYC(b_+385, b_+386); H = A;
  CYC(b_+386, b_+387); A = B;
  CYC(b_+387, b_+388); interactionCodebc_addAToHl_from_rst(gb, b_+388);
  CYC(b_+388, b_+389); B = mem_rd(gb, HL);
  CYC(b_+389, b_+390); SET_HL(HL + 1);
  CYC(b_+390, b_+391); C = mem_rd(gb, HL);
  CALL_C(b_+391, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+394);
  CYC(b_+394, b_+396); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+396, b_+397); mem_wr(gb, DE, A);
  CYC(b_+397, b_+400); objectApplySpeed_hook(gb); // jp
}

// interactionCodebc@animate (0b:6f2c): shared by both direct tail-jumps ("jp"/"jr" -- handled
// inline below with no push, ending the whole hook) and genuine `call`s from @subid0Substate1,
// @subid0Substate2 and @subid2State1, each with different code following (needs its own
// return_address, same trampoline shape as @updateAnimationIndex above).
static void interactionCodebc_animate(GB *gb, uint16_t return_address) {
  BASE(interactionCodebc);
  push_effect(gb, return_address);
  CYC(b_+321, b_+324); interactionAnimate_hook(gb); // jp
}

// interactionCodebc@func_6eac (0b:6eac): true local, no CALL_C, reached both by plain fallthrough
// from @subid0Substate1 (no call at all) and by genuine `call` from @subid0Substate3@looped
// (twice); either way a plain C function call/return reproduces it correctly.
static void interactionCodebc_func_6eac(GB *gb) {
  BASE(interactionCodebc);
  CYC(b_+193, b_+196); SET_HL(wTmpcfc0_genericCutscene_cfc6);
  CYC(b_+196, b_+197); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+197, b_+198); A = mem_rd(gb, HL);
  CYC(b_+198, b_+200); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+200, b_+201); return; } // ret nz
  CYC(b_+200, b_+201);
  CYC(b_+201, b_+203); mem_wr(gb, HL, 0x00);
  CYC(b_+203, b_+206); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+206, b_+208); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01); // set 0,(hl)
  CYC(b_+208, b_+209); return; // ret
}

// ==================================================================================================
// INTERAC_TWINROVA_3
// ==================================================================================================
void interactionCodebc_hook(GB *gb) {
  BASE(interactionCodebc);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == b_+100) goto state1;
    // target == 0x6df3 falls through to state0
  }

  // interactionCodebc@state0
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  {
    CYC(b_+11, b_+12); push_effect(gb, b_+12);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == b_+36) goto initSubid2;
    if (target == b_+56) goto initSubid3;
    // target == 0x6dff falls through to initSubid0/initSubid1 (aliased)
  }

  // interactionCodebc@initSubid0 / @initSubid1
  CYC(b_+20, b_+23); interactionCodebc_commonInit(gb, sp0_); // call
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xfb);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+33); interactionCodebc_readPositionTable(gb); // call
  CYC(b_+33, b_+36); goto state1; // jp

initSubid2: // interactionCodebc@initSubid2
  CYC(b_+36, b_+39); interactionCodebc_commonInit(gb, sp0_); // call
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0xf0);
  CYC(b_+43, b_+45); A = 0x02;
  CYC(b_+45, b_+48); interactionCodebc_readPositionTable(gb); // call
  CYC(b_+48, b_+50); A = 0x04;
  CALL_C(b_+50, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+53);
  CYC(b_+53, b_+56); goto state1; // jp

initSubid3: // interactionCodebc@initSubid3
  CYC(b_+56, b_+59); interactionCodebc_commonInit(gb, sp0_); // call
  CYC(b_+59, b_+61); A = 0x02;
  CYC(b_+61, b_+64); interactionCodebc_readPositionTable(gb); // call
  CYC(b_+64, b_+66); A = 0x01;
  CALL_C(b_+66, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+69);
  CYC(b_+69, b_+72); goto state1; // jp

state1: // interactionCodebc@state1
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  {
    CYC(b_+103, b_+104); push_effect(gb, b_+104);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == b_+294) goto subid2State1;
    // target == 0x6e5b falls through to subid0State1 (subid0/subid1 aliased, subid2/subid3 aliased)
  }

  // interactionCodebc@subid0State1
  CYC(b_+112, b_+114); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+114, b_+115); A = mem_rd(gb, DE);
  {
    CYC(b_+115, b_+116); push_effect(gb, b_+116);
    uint16_t target = interactionCodebc_jump_table(gb);
    if (target == b_+176) goto subid0Substate1;
    if (target == b_+209) goto subid0Substate2;
    if (target == b_+240) goto subid0Substate3;
    if (target == b_+291) { CYC(b_+291, b_+294); interactionAnimate_hook(gb); return; } // subid0Substate4: jp @animate
    // target == 0x6e69 falls through to subid0Substate0
  }

  // interactionCodebc@subid0Substate0
  CYC(b_+126, b_+129); interactionCodebc_moveTowardTargetPosition(gb, sp0_, b_+129); // call
  CYC(b_+129, b_+132); interactionCodebc_updateAnimationIndex(gb, sp0_, b_+132); // call
  CYC(b_+132, b_+135); interactionCodebc_checkReachedTargetPosition(gb, sp0_); // call
  if (F & FC) { CYCT(b_+135, b_+138); interactionCodebc_nextTargetPosition(gb, sp0_); } else { CYC(b_+135, b_+138); } // call c
  if (!(F & FC)) { CYCT(b_+138, b_+141); interactionAnimate_hook(gb); return; } // jp nc @animate
  CYC(b_+138, b_+141);
  CYC(b_+141, b_+142); H = D;
  CYC(b_+142, b_+144); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+144, b_+146); mem_wr(gb, HL, 0x01);
  CYC(b_+146, b_+148); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+148, b_+150); mem_wr(gb, HL, 40);
  CYC(b_+150, b_+152); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+152, b_+153); A = mem_rd(gb, HL);
  CYC(b_+153, b_+154); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+154, b_+156); goto l6e8b; } // jr nz
  CYC(b_+154, b_+156);
  CYC(b_+156, b_+158); A = 0x00;
  CYC(b_+158, b_+160); goto l6e95; // jr

l6e8b:
  CYC(b_+160, b_+162); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+162, b_+164); goto l6e93; } // jr nz
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+166); A = 0x01;
  CYC(b_+166, b_+168); goto l6e95; // jr

l6e93:
  CYC(b_+168, b_+170); A = 0x02;

l6e95:
  CALL_C(b_+170, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+173);
  CYC(b_+173, b_+176); interactionAnimate_hook(gb); return; // jp @animate

subid0Substate1: // interactionCodebc@subid0Substate1
  CYC(b_+176, b_+179); interactionCodebc_updateFloating(gb); // call
  CYC(b_+179, b_+182); interactionCodebc_animate(gb, b_+182); // call @animate
  CALL_C(b_+182, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+185);
  if (!(F & FZ)) { CYCT(b_+185, b_+186); ret_effect(gb); return; } // ret nz
  CYC(b_+185, b_+186);
  CYC(b_+186, b_+188); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+188, b_+189); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+189, b_+191); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+191, b_+193); mem_wr(gb, HL, 40);
  interactionCodebc_func_6eac(gb); return; // falls through into @func_6eac, no call

subid0Substate2: // interactionCodebc@subid0Substate2
  CYC(b_+209, b_+212); interactionCodebc_updateFloating(gb); // call
  CYC(b_+212, b_+215); interactionCodebc_animate(gb, b_+215); // call @animate
  CYC(b_+215, b_+218); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+218, b_+220); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+220, b_+221); ret_effect(gb); return; } // ret nz
  CYC(b_+220, b_+221);
  CALL_C(b_+221, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+224);
  if (!(F & FZ)) { CYCT(b_+224, b_+225); ret_effect(gb); return; } // ret nz
  CYC(b_+224, b_+225);
  CYC(b_+225, b_+227); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+227, b_+228); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+228, b_+230); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+230, b_+232); mem_wr(gb, HL, 0xff);
  CYC(b_+232, b_+234); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+234, b_+235); A = mem_rd(gb, HL);
  CYC(b_+235, b_+237); alu_add(gb, 0x04);
  CYC(b_+237, b_+240); interactionCodebc_readPositionTable(gb); return; // jp

subid0Substate3: // interactionCodebc@subid0Substate3
  CYC(b_+240, b_+243); interactionCodebc_moveTowardTargetPosition(gb, sp0_, b_+243); // call
  CYC(b_+243, b_+246); interactionCodebc_checkReachedTargetPosition(gb, sp0_); // call
  if (F & FC) { CYCT(b_+246, b_+249); interactionCodebc_nextTargetPosition(gb, sp0_); } else { CYC(b_+246, b_+249); } // call c
  if (F & FC) { CYCT(b_+249, b_+251); goto looped; } // jr c
  CYC(b_+249, b_+251);
  CYC(b_+251, b_+254); interactionCodebc_moveTowardTargetPosition(gb, sp0_, b_+254); // call
  CYC(b_+254, b_+256); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+256, b_+257); A = mem_rd(gb, DE);
  CYC(b_+257, b_+259); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+259, b_+262); interactionCodebc_updateAnimationIndex(gb, sp0_, b_+262); } else { CYC(b_+259, b_+262); } // call nz
  CYC(b_+262, b_+265); interactionCodebc_checkReachedTargetPosition(gb, sp0_); // call
  if (F & FC) { CYCT(b_+265, b_+268); interactionCodebc_nextTargetPosition(gb, sp0_); } else { CYC(b_+265, b_+268); } // call c
  if (!(F & FC)) { CYCT(b_+268, b_+270); interactionAnimate_hook(gb); return; } // jr nc @animate
  CYC(b_+268, b_+270);

looped: // interactionCodebc@subid0Substate3@looped
  CYC(b_+270, b_+272); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+272, b_+273); A = mem_rd(gb, DE);
  CYC(b_+273, b_+275); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+275, b_+277); goto l6f06; } // jr c
  CYC(b_+275, b_+277);
  CYC(b_+277, b_+280); interactionCodebc_func_6eac(gb); // call
  CYC(b_+280, b_+283); interactionDelete_hook(gb); return; // jp

l6f06:
  CYC(b_+283, b_+286); interactionCodebc_func_6eac(gb); // call
  CYC(b_+286, b_+287); H = D;
  CYC(b_+287, b_+289); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+289, b_+290); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+290, b_+291); ret_effect(gb); return; // ret

subid2State1: // interactionCodebc@subid2State1
  CALL_C(b_+294, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+297);
  if (!(F & FZ)) { CYCT(b_+297, b_+299); goto l6f2a; } // jr nz
  CYC(b_+297, b_+299);
  CYC(b_+299, b_+302); interactionCodebc_updateFloating(gb); // call
  CYC(b_+302, b_+305); interactionCodebc_animate(gb, b_+305); // call @animate
  CYC(b_+305, b_+308); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+308, b_+310); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+310, b_+311); ret_effect(gb); return; } // ret z
  CYC(b_+310, b_+311);
  CALL_C(b_+311, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+314);
  CYC(b_+314, b_+316); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+316, b_+318); mem_wr(gb, HL, 0xff);
  CYC(b_+318, b_+319); ret_effect(gb); return; // ret

l6f2a:
  CYC(b_+319, b_+321); goto subid0Substate3; // jr
}
