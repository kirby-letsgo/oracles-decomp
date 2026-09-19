#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodeb9_jump_table(GB *gb) {
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

static void interactionCodeb9_addAToHl(GB *gb, uint16_t return_address) {
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

static void interactionCodeb9_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:6cb2, interactionCodeb9@jump. Reached by fallthrough from @subid0Init, by a tail `jp` from
// @subid0/@subid1/@subid2 -- goto, same routine -- and by a genuine `call` from @subid3Init and
// @subid4Init, which both still have work to do afterward -- their call sites push their own
// return address and check the resulting pc/sp exactly like CALL_C would, since this plain C function
// itself ends with a bare tail-call into objectSetSpeedZ_hook with no push of its own.
static void interactionCodeb9_jump(GB *gb) {
  CYC(0x6cb2, 0x6cb4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6cb4, 0x6cb5); A = mem_rd(gb, DE);
  CYC(0x6cb5, 0x6cb8); SET_HL(0x6d01); // @speedZVals
  CYC(0x6cb8, 0x6cb9); interactionCodeb9_addDoubleIndex(gb, 0x6cb9);
  CYC(0x6cb9, 0x6cba); C = mem_rd(gb, HL);
  CYC(0x6cba, 0x6cbb); SET_HL(HL + 1); // inc hl
  CYC(0x6cbb, 0x6cbc); B = mem_rd(gb, HL);
  CYC(0x6cbc, 0x6cbf); objectSetSpeedZ_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_HORON_DOG_CREDITS
//
// Variables:
//   subid: Used as a sort of "state" variable?
//   var36: Target x-position
//   var37: ?
// ==================================================================================================
void interactionCodeb9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c5a, 0x6c5c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6c5c, 0x6c5d); A = mem_rd(gb, DE);
  CYC(0x6c5d, 0x6c5e); push_effect(gb, 0x6c5e);
  switch (interactionCodeb9_jump_table(gb)) {
    case 0x6c62: goto state0;
    case 0x6d17: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x6c62, 0x6c64); A = 0x01;
  CYC(0x6c64, 0x6c65); mem_wr(gb, DE, A); // [state]
  CALL_C(0x6c65, interactionInitGraphics_hook, 0x15fb, 0x6c68);
  CALL_C(0x6c68, objectSetVisiblec2_hook, 0x1e45, 0x6c6b);
  CALL_C(0x6c6b, objectSetInvisible_hook, 0x1e7b, 0x6c6e);
  CYC(0x6c6e, 0x6c70); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6c70, 0x6c71); A = mem_rd(gb, DE);
  CYC(0x6c71, 0x6c72); B = A;
  CYC(0x6c72, 0x6c75); SET_HL(0x6ce9); // @counter1Vals
  CYC(0x6c75, 0x6c76); interactionCodeb9_addAToHl(gb, 0x6c76);
  CYC(0x6c76, 0x6c77); A = mem_rd(gb, HL);
  CYC(0x6c77, 0x6c79); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6c79, 0x6c7a); mem_wr(gb, DE, A);
  CYC(0x6c7a, 0x6c7b); A = B;
  CYC(0x6c7b, 0x6c7e); SET_HL(0x6cf1); // @positions
  CYC(0x6c7e, 0x6c7f); interactionCodeb9_addDoubleIndex(gb, 0x6c7f);
  CYC(0x6c7f, 0x6c80); B = mem_rd(gb, HL);
  CYC(0x6c80, 0x6c81); SET_HL(HL + 1); // inc hl
  CYC(0x6c81, 0x6c82); A = mem_rd(gb, HL);
  CYC(0x6c82, 0x6c83); C = A;
  CYC(0x6c83, 0x6c85); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(0x6c85, 0x6c86); mem_wr(gb, DE, A);
  CALL_C(0x6c86, objectGetRelativeAngle_hook, 0x1ea4, 0x6c89);
  CYC(0x6c89, 0x6c8b); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6c8b, 0x6c8c); mem_wr(gb, DE, A);
  CYC(0x6c8c, 0x6c8e); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6c8e, 0x6c90); A = 0x28; // SPEED_100
  CYC(0x6c90, 0x6c91); mem_wr(gb, DE, A);
  CYC(0x6c91, 0x6c93); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6c93, 0x6c94); A = mem_rd(gb, DE);
  CYC(0x6c94, 0x6c95); push_effect(gb, 0x6c95);
  switch (interactionCodeb9_jump_table(gb)) {
    case 0x6ca5: goto subid0Init;
    case 0x6cb2: interactionCodeb9_jump(gb); return;
    case 0x6cbf: goto subid3Init;
    case 0x6cca: goto subid4Init;
    case 0x6ce3: goto subid7Init;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0Init:
  CYC(0x6ca5, 0x6ca7); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6ca7, 0x6ca9); A = 0x04;
  CYC(0x6ca9, 0x6caa); mem_wr(gb, DE, A);
  CYC(0x6caa, 0x6cab); H = D;
  CYC(0x6cab, 0x6cad); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6cad, 0x6caf); mem_wr(gb, HL, 0xe0);
  CYC(0x6caf, 0x6cb0); SET_HL(HL + 1); // inc hl
  CYC(0x6cb0, 0x6cb2); mem_wr(gb, HL, 0x01); // [counter2]
  interactionCodeb9_jump(gb); return; // falls through into @jump

subid3Init:
  CYC(0x6cbf, 0x6cc2); push_effect(gb, 0x6cc2); interactionCodeb9_jump(gb);
  if (!(gb->pc == 0x6cc2 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(0x6cc2, 0x6cc4); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6cc4, 0x6cc6); A = 0x3c; // SPEED_180
  CYC(0x6cc6, 0x6cc7); mem_wr(gb, DE, A);
  CYC(0x6cc7, 0x6cca); goto setZPosition; // jp

subid4Init: // also @subid5Init, @subid6Init -- aliased jump-table entry
  CYC(0x6cca, 0x6ccd); push_effect(gb, 0x6ccd); interactionCodeb9_jump(gb);
  if (!(gb->pc == 0x6ccd && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(0x6ccd, 0x6ccf); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6ccf, 0x6cd1); A = 0x0a; // SPEED_40
  CYC(0x6cd1, 0x6cd2); mem_wr(gb, DE, A);
  // falls through into @setZPosition

setZPosition:
  CYC(0x6cd2, 0x6cd4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6cd4, 0x6cd5); A = mem_rd(gb, DE);
  CYC(0x6cd5, 0x6cd7); alu_sub(gb, 0x03);
  CYC(0x6cd7, 0x6cda); SET_HL(0x6d0f); // @zPositions
  CYC(0x6cda, 0x6cdb); interactionCodeb9_addDoubleIndex(gb, 0x6cdb);
  CYC(0x6cdb, 0x6cdd); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6cdd, 0x6cde); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6cde, 0x6cdf); mem_wr(gb, DE, A);
  CYC(0x6cdf, 0x6ce0); E = alu_dec8(gb, E);
  CYC(0x6ce0, 0x6ce1); A = mem_rd(gb, HL);
  CYC(0x6ce1, 0x6ce2); mem_wr(gb, DE, A);
  RET(0x6ce2); return;

subid7Init:
  CYC(0x6ce3, 0x6ce6); SET_HL(0x7d7b); // mainScripts.horonDogCreditsScript
  CYC(0x6ce6, 0x6ce9); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(0x6d17, 0x6d19); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6d19, 0x6d1a); A = mem_rd(gb, DE);
  CYC(0x6d1a, 0x6d1b); push_effect(gb, 0x6d1b);
  switch (interactionCodeb9_jump_table(gb)) {
    case 0x6d21: goto substate0;
    case 0x6d2b: goto substate1;
    case 0x6d7c: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x6d21, interactionDecCounter1_hook, 0x23cc, 0x6d24);
  if (!(F & FZ)) { CYCT(0x6d24, 0x6d25); ret_effect(gb); return; } // ret nz
  CYC(0x6d24, 0x6d25);
  CALL_C(0x6d25, objectSetVisible_hook, 0x1e84, 0x6d28);
  CYC(0x6d28, 0x6d2b); interactionIncSubstate_hook(gb); return; // jp

substate1:
  CALL_C(0x6d2b, interactionAnimate_hook, 0x261b, 0x6d2e);
  CALL_C(0x6d2e, objectApplySpeed_hook, 0x201d, 0x6d31);
  CYC(0x6d31, 0x6d32); H = D;
  CYC(0x6d32, 0x6d34); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6d34, 0x6d35); A = mem_rd(gb, HL);
  CYC(0x6d35, 0x6d37); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(0x6d37, 0x6d38); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6d38, 0x6d3a); goto reachedTargetXPosition; } // jr nz
  CYC(0x6d38, 0x6d3a);
  CALL_C(0x6d3a, interactionIncSubstate_hook, 0x23e5, 0x6d3d);
  CYC(0x6d3d, 0x6d3f); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6d3f, 0x6d41); mem_wr(gb, HL, 0x00);
  CYC(0x6d41, 0x6d43); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6d43, 0x6d44); A = mem_rd(gb, HL);
  CYC(0x6d44, 0x6d45); alu_add(gb, A);
  CYC(0x6d45, 0x6d46); A = alu_inc8(gb, A);
  CYC(0x6d46, 0x6d49); interactionSetAnimation_hook(gb); return; // jp

reachedTargetXPosition:
  CYC(0x6d49, 0x6d4b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6d4b, 0x6d4c); A = mem_rd(gb, DE);
  CYC(0x6d4c, 0x6d4d); push_effect(gb, 0x6d4d);
  switch (interactionCodeb9_jump_table(gb)) {
    case 0x6d5d: goto subid0_1_2;
    case 0x6d68: goto subid3;
    case 0x6d78: goto subid4_5_6;
    case 0x6d77: goto subid7;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0_1_2:
  CYC(0x6d5d, 0x6d5f); C = 0x20;
  CALL_C(0x6d5f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6d62);
  if (!(F & FZ)) { CYCT(0x6d62, 0x6d63); ret_effect(gb); return; } // ret nz
  CYC(0x6d62, 0x6d63);
  CYC(0x6d63, 0x6d65); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6d65, 0x6d68); interactionCodeb9_jump(gb); return; // jp

subid3:
  CYC(0x6d68, 0x6d6a); C = 0x10;
  // falls through into @label_0b_293

label_0b_293:
  CYC(0x6d6a, 0x6d6c); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x6d6c, 0x6d6d); A = mem_rd(gb, DE);
  CYC(0x6d6d, 0x6d6e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d6e, 0x6d6f); ret_effect(gb); return; } // ret nz
  CYC(0x6d6e, 0x6d6f);
  CALL_C(0x6d6f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6d72);
  if (!(F & FZ)) { CYCT(0x6d72, 0x6d73); ret_effect(gb); return; } // ret nz
  CYC(0x6d72, 0x6d73);
  CYC(0x6d73, 0x6d74); H = D;
  CYC(0x6d74, 0x6d76); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x6d76, 0x6d77); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  // falls through into @subid7

subid7:
  RET(0x6d77); return;

subid4_5_6:
  CYC(0x6d78, 0x6d7a); C = 0x01;
  CYCT(0x6d7a, 0x6d7c); goto label_0b_293; // jr

substate2:
  CYC(0x6d7c, 0x6d7e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6d7e, 0x6d7f); A = mem_rd(gb, DE);
  CYC(0x6d7f, 0x6d80); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d80, 0x6d82); goto substate2_subidNot0; } // jr nz
  CYC(0x6d80, 0x6d82);

  // @substate2_subid0
  CYC(0x6d82, 0x6d83); B = A;
  CYC(0x6d83, 0x6d84); H = D;
  CYC(0x6d84, 0x6d86); L = INTERACTION_BASE + OBJ_COUNTER1;
  CALL_C(0x6d86, decHlRef16WithCap_hook, 0x0237, 0x6d89);
  if (!(F & FZ)) { CYCT(0x6d89, 0x6d8b); goto animate; } // jr nz
  CYC(0x6d89, 0x6d8b);
  CYC(0x6d8b, 0x6d8e); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x6d8e, 0x6d90); mem_wr(gb, HL, 0x01);
  RET(0x6d90); return;

substate2_subidNot0:
  CYC(0x6d91, 0x6d93); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x6d93, 0x6d95); goto animate; } // jr nz
  CYC(0x6d93, 0x6d95);
  CALL_C(0x6d95, interactionRunScript_hook, 0x2552, 0x6d98);
  CYC(0x6d98, 0x6d9a); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6d9a, 0x6d9b); A = mem_rd(gb, DE);
  CYC(0x6d9b, 0x6d9c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6d9c, 0x6d9d); ret_effect(gb); return; } // ret z
  CYC(0x6d9c, 0x6d9d);
  // falls through into @animate

animate:
  CYC(0x6d9d, 0x6da0); interactionAnimate_hook(gb); return; // jp
}
