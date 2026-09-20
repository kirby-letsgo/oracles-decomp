#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// oldLadyScriptTable: .dw mainScripts.oldLadySubid{0,1,2,3}Script (bank $0c pointers).
#define oldLadyScriptTable_bank08 0x7f53

// mainScripts.linkedGameNpcScript (bank $0c) and agesInteractionsBank09.getGameProgress_1
// (bank $09), referenced by address only.
#define linkedGameNpcScript_bank0c 0x7ed9
#define getGameProgress_1_bank09 0x552b

static uint16_t oldLady_jumpTable(GB *gb) {
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

static void oldLady_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode3d@loadScript: pick the script from oldLadyScriptTable by subid. Reached
// from the init table, by `jr`/`jp` from @initSubid0, @initSubid2 and @runSubid3@substate1.
static void oldLady_loadScript(GB *gb) {
  CYC(0x7e74, 0x7e76); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7e76, 0x7e77); A = mem_rd(gb, DE);
  CYC(0x7e77, 0x7e7a); SET_HL(oldLadyScriptTable_bank08);
  CYC(0x7e7a, 0x7e7b); oldLady_addDoubleIndex(gb, 0x7e7b);
  CYC(0x7e7b, 0x7e7c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7e7c, 0x7e7d); H = mem_rd(gb, HL);
  CYC(0x7e7d, 0x7e7e); L = A;
  CYC(0x7e7e, 0x7e81); interactionSetScript_hook(gb);
}

// interactionCode3d@state1: subid dispatch. Reached from the state table and by `jr` from
// @initSubid4/5 (while @initSubid's return address is still on the stack).
static void oldLady_state1(GB *gb, uint16_t sp0_) {
  CYC(0x7eb0, 0x7eb2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7eb2, 0x7eb3); A = mem_rd(gb, DE);
  CYC(0x7eb3, 0x7eb4); push_effect(gb, 0x7eb4);
  switch (oldLady_jumpTable(gb)) {
    case 0x7ec0: goto runSubid0;
    case 0x7ecd: goto runSubid1;
    case 0x7f0f: goto runSubid2;
    case 0x7f15: goto runSubid3;
    case 0x7f4a: goto runSubid4;
    default: HANDOFF(HL);
  }

runSubid0:
  // NPC with a grandson that is stone for part of the game
  CALL_C(0x7ec0, interactionRunScript_hook, 0x2552, 0x7ec3);
  CYC(0x7ec3, 0x7ec5); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7ec5, 0x7ec6); A = mem_rd(gb, DE);
  CYC(0x7ec6, 0x7ec7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7ec7, 0x7eca); interactionAnimateAsNpc_hook(gb); return;
  }
  CYC(0x7ec7, 0x7eca);
  CYC(0x7eca, 0x7ecd); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid1:
  // Cutscene where her grandson gets turned to stone
  CYC(0x7ecd, 0x7ecf); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7ecf, 0x7ed0); A = mem_rd(gb, DE);
  CYC(0x7ed0, 0x7ed1); push_effect(gb, 0x7ed1);
  switch (oldLady_jumpTable(gb)) {
    case 0x7ed9: goto subid1_substate0;
    case 0x7ef1: goto subid1_substate1;
    case 0x7efa: goto subid1_substate2;
    case 0x7f05: goto subid1_substate3;
    default: HANDOFF(HL);
  }

subid1_substate0:
  CALL_C(0x7ed9, interactionAnimate_hook, 0x261b, 0x7edc);
  CALL_C(0x7edc, interactionRunScript_hook, 0x2552, 0x7edf);
  if (!(F & FC)) {
    CYCT(0x7edf, 0x7ee1); goto subid1_substate0_scriptRunning;
  }
  CYC(0x7edf, 0x7ee1);
  // Script ended
  CALL_C(0x7ee1, interactionIncSubstate_hook, 0x23e5, 0x7ee4);
  CYC(0x7ee4, 0x7ee6); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7ee6, 0x7ee8); mem_wr(gb, HL, 60);
  CYC(0x7ee8, 0x7ee9); ret_effect(gb);
  return;
subid1_substate0_scriptRunning:
  CYC(0x7ee9, 0x7eeb); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7eeb, 0x7eec); A = mem_rd(gb, DE);
  CYC(0x7eec, 0x7eed); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7eed, 0x7ef0); interactionAnimate2Times_hook(gb); return;
  }
  CYC(0x7eed, 0x7ef0);
  CYC(0x7ef0, 0x7ef1); ret_effect(gb);
  return;

subid1_substate1:
  CALL_C(0x7ef1, interactionDecCounter1_hook, 0x23cc, 0x7ef4);
  if (!(F & FZ)) {
    CYCT(0x7ef4, 0x7ef5); ret_effect(gb); return;
  }
  CYC(0x7ef4, 0x7ef5);
  CYC(0x7ef5, 0x7ef7); mem_wr(gb, HL, 20);
  CYC(0x7ef7, 0x7efa); interactionIncSubstate_hook(gb);
  return;

subid1_substate2:
  CALL_C(0x7efa, interactionDecCounter1_hook, 0x23cc, 0x7efd);
  if (!(F & FZ)) {
    CYCT(0x7efd, 0x7f00); interactionAnimate3Times_hook(gb); return;
  }
  CYC(0x7efd, 0x7f00);
  CYC(0x7f00, 0x7f02); mem_wr(gb, HL, 60);
  CYC(0x7f02, 0x7f05); interactionIncSubstate_hook(gb);
  return;

subid1_substate3:
  CALL_C(0x7f05, interactionDecCounter1_hook, 0x23cc, 0x7f08);
  if (!(F & FZ)) {
    CYCT(0x7f08, 0x7f09); ret_effect(gb); return;
  }
  CYC(0x7f08, 0x7f09);
  CYC(0x7f09, 0x7f0b); A = 0xff;
  CYC(0x7f0b, 0x7f0e); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  CYC(0x7f0e, 0x7f0f); ret_effect(gb);
  return;

runSubid2:
  // NPC in present, screen left from bipin&blossom's house
  CALL_C(0x7f0f, interactionRunScript_hook, 0x2552, 0x7f12);
  CYC(0x7f12, 0x7f15); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid3:
  // Cutscene where her grandson is restored from stone
  CYC(0x7f15, 0x7f17); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7f17, 0x7f18); A = mem_rd(gb, DE);
  CYC(0x7f18, 0x7f19); push_effect(gb, 0x7f19);
  switch (oldLady_jumpTable(gb)) {
    case 0x7f1f: goto subid3_substate0;
    case 0x7f29: goto subid3_substate1;
    case 0x7f39: goto subid3_substate2;
    default: HANDOFF(HL);
  }

subid3_substate0:
  CALL_C(0x7f1f, interactionDecCounter1_hook, 0x23cc, 0x7f22);
  if (!(F & FZ)) {
    CYCT(0x7f22, 0x7f23); ret_effect(gb); return;
  }
  CYC(0x7f22, 0x7f23);
  CALL_C(0x7f23, startJump_hook, 0x73db, 0x7f26);
  CYC(0x7f26, 0x7f29); interactionIncSubstate_hook(gb);
  return;

subid3_substate1:
  CYC(0x7f29, 0x7f2b); C = 0x20;
  CALL_C(0x7f2b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7f2e);
  if (!(F & FZ)) {
    CYCT(0x7f2e, 0x7f2f); ret_effect(gb); return;
  }
  CYC(0x7f2e, 0x7f2f);
  CALL_C(0x7f2f, interactionIncSubstate_hook, 0x23e5, 0x7f32);
  CYC(0x7f32, 0x7f34); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7f34, 0x7f36); mem_wr(gb, HL, 0xb4);
  CYC(0x7f36, 0x7f39); oldLady_loadScript(gb);
  return;

subid3_substate2:
  CYC(0x7f39, 0x7f3a); H = D;
  CYC(0x7f3a, 0x7f3c); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7f3c, 0x7f3d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7f3d, 0x7f3f);
  } else {
    CYC(0x7f3d, 0x7f3f);
    CYC(0x7f3f, 0x7f41); A = 0xff;
    CYC(0x7f41, 0x7f44); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  }
  CALL_C(0x7f44, interactionRunScript_hook, 0x2552, 0x7f47);
  CYC(0x7f47, 0x7f4a); interactionAnimateBasedOnSpeed_hook(gb);
  return;

runSubid4:
  // Linked game NPC (subids 4 and 5)
  CALL_C(0x7f4a, interactionRunScript_hook, 0x2552, 0x7f4d);
  if (F & FC) {
    CYCT(0x7f4d, 0x7f50); interactionDelete_hook(gb); return;
  }
  CYC(0x7f4d, 0x7f50);
  CYC(0x7f50, 0x7f53); npcFaceLinkAndAnimate_hook(gb);
}

// interactionCode3d@initSubid: per-subid initialization. Reached only by a static `call`
// from @state0.
static void oldLady_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x7e4d, 0x7e4f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7e4f, 0x7e50); A = mem_rd(gb, DE);
  CYC(0x7e50, 0x7e51); push_effect(gb, 0x7e51);
  switch (oldLady_jumpTable(gb)) {
    case 0x7e5d: goto initSubid0;
    case 0x7e74: oldLady_loadScript(gb); return;
    case 0x7e81: goto initSubid2;
    case 0x7e92: goto initSubid3;
    case 0x7e9c: goto initSubid4;
    case 0x7ea0: goto initSubid5;
    default: HANDOFF(HL);
  }

initSubid0:
  CYC(0x7e5d, 0x7e5f); A = 0x03;
  CALL_C(0x7e5f, interactionSetAnimation_hook, 0x262e, 0x7e62);
  // Check whether her grandson is stone
  CYC(0x7e62, 0x7e64); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(0x7e64, checkGlobalFlag_hook, 0x31f3, 0x7e67);
  if (F & FZ) {
    CYCT(0x7e67, 0x7e69); oldLady_loadScript(gb); return;
  }
  CYC(0x7e67, 0x7e69);
  // Set var03 to nonzero if her grandson is stone, also change her position
  CYC(0x7e69, 0x7e6b); A = 0x01;
  CYC(0x7e6b, 0x7e6d); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7e6d, 0x7e6e); mem_wr(gb, DE, A);
  CYC(0x7e6e, 0x7e71); SET_BC(0x4878);
  CALL_C(0x7e71, interactionSetPosition_hook, 0x2773, 0x7e74);
  oldLady_loadScript(gb);
  return;

initSubid2:
  // This NPC only exists between saving Nayru and beating d7?
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(0x7e81, 0x7e84); SET_HL(getGameProgress_1_bank09);
  CYC(0x7e84, 0x7e86); E = 0x09;
  CALL_C(0x7e86, interBankCall_hook, 0x008a, 0x7e89);
  CYC(0x7e89, 0x7e8b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7e8b, 0x7e8c); A = mem_rd(gb, DE);
  CYC(0x7e8c, 0x7e8d); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x7e8d, 0x7e90); interactionDelete_hook(gb); return;
  }
  CYC(0x7e8d, 0x7e90);
  CYC(0x7e90, 0x7e92); oldLady_loadScript(gb);
  return;

initSubid3:
  CYC(0x7e92, 0x7e94); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7e94, 0x7e96); A = 220;
  CYC(0x7e96, 0x7e97); mem_wr(gb, DE, A);
  CYC(0x7e97, 0x7e99); A = 0x03;
  CYC(0x7e99, 0x7e9c); interactionSetAnimation_hook(gb);
  return;

initSubid4:
  CYC(0x7e9c, 0x7e9e); A = 0x00;
  CYC(0x7e9e, 0x7ea0); goto linkedGameNpc;

initSubid5:
  CYC(0x7ea0, 0x7ea2); A = 0x09;

linkedGameNpc:
  CYC(0x7ea2, 0x7ea4); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7ea4, 0x7ea5); mem_wr(gb, DE, A);
  CYC(0x7ea5, 0x7ea8); SET_HL(linkedGameNpcScript_bank0c);
  CALL_C(0x7ea8, interactionSetScript_hook, 0x2544, 0x7eab);
  CALL_C(0x7eab, interactionRunScript_hook, 0x2552, 0x7eae);
  CYC(0x7eae, 0x7eb0); oldLady_state1(gb, sp0_);
}

// INTERAC_OLD_LADY
void interactionCode3d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7e31, 0x7e33); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7e33, 0x7e34); A = mem_rd(gb, DE);
  CYC(0x7e34, 0x7e35); push_effect(gb, 0x7e35);
  switch (oldLady_jumpTable(gb)) {
    case 0x7e39: goto state0;
    case 0x7eb0: oldLady_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x7e39, 0x7e3b); A = 0x01;
  CYC(0x7e3b, 0x7e3c); mem_wr(gb, DE, A);
  CALL_C(0x7e3c, interactionInitGraphics_hook, 0x15fb, 0x7e3f);
  CALL_C(0x7e3f, objectSetVisiblec2_hook, 0x1e45, 0x7e42);
  CYC(0x7e42, 0x7e45); push_effect(gb, 0x7e45); oldLady_initSubid(gb, sp0_);
  CYC(0x7e45, 0x7e47); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x7e47, 0x7e48); A = mem_rd(gb, DE);
  CYC(0x7e48, 0x7e49); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7e49, 0x7e4c); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x7e49, 0x7e4c);
  CYC(0x7e4c, 0x7e4d); ret_effect(gb);
}
