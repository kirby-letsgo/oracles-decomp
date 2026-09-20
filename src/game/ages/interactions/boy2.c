#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t boy2_jump_table(GB *gb) {
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

static void boy2_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void boy2_initialize_script(GB *gb, uint16_t sp0_) {
  CYC(0x4f49, 0x4f4b); A = 0x29;
  CALL_C(0x4f4b, interactionSetHighTextIndex_hook, 0x253b, 0x4f4e);
  CYC(0x4f4e, 0x4f50); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4f50, 0x4f51); A = mem_rd(gb, DE);
  CYC(0x4f51, 0x4f54); SET_HL(0x4f5e);
  CYC(0x4f54, 0x4f55); boy2_add_double_index(gb, 0x4f55);
  CYC(0x4f55, 0x4f56); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f56, 0x4f57); H = mem_rd(gb, HL);
  CYC(0x4f57, 0x4f58); L = A;
  CALL_C(0x4f58, interactionSetScript_hook, 0x2544, 0x4f5b);
  CYC(0x4f5b, 0x4f5e); interactionIncState_hook(gb);
}

static void boy2_init_graphics_and_inc_state(GB *gb, uint16_t sp0_) {
  CALL_C(0x4f3a, interactionInitGraphics_hook, 0x15fb, 0x4f3d);
  CALL_C(0x4f3d, objectMarkSolidPosition_hook, 0x24f0, 0x4f40);
  CYC(0x4f40, 0x4f43); interactionIncState_hook(gb);
}

static void boy2_initialize_graphics_and_script(GB *gb, uint16_t sp0_) {
  CALL_C(0x4f43, interactionInitGraphics_hook, 0x15fb, 0x4f46);
  CALL_C(0x4f46, objectMarkSolidPosition_hook, 0x24f0, 0x4f49);
  boy2_initialize_script(gb, sp0_);
}

void interactionCode3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e8c, 0x4e8e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4e8e, 0x4e8f); A = mem_rd(gb, DE);
  CYC(0x4e8f, 0x4e90); push_effect(gb, 0x4e90);
  switch (boy2_jump_table(gb)) {
    case 0x4e98: goto subid0;
    case 0x4eb6: goto subid1;
    case 0x4ed2: goto subid2;
    case 0x4f1f: goto subid3;
    default: HANDOFF(HL);
  }

subid0:
  CALL_C(0x4e98, checkInteractionState_hook, 0x23fe, 0x4e9b);
  if (!(F & FZ)) {
    CYCT(0x4e9b, 0x4e9d); goto subid0_state1;
  }
  CYC(0x4e9b, 0x4e9d);
  CYC(0x4e9d, 0x4e9f); A = 0x14;
  CALL_C(0x4e9f, checkGlobalFlag_hook, 0x31f3, 0x4ea2);
  if (!(F & FZ)) {
    CYCT(0x4ea2, 0x4ea5); interactionDelete_hook(gb); return;
  }
  CYC(0x4ea2, 0x4ea5);
  CYC(0x4ea5, 0x4ea7); A = 0x0b;
  CALL_C(0x4ea7, checkGlobalFlag_hook, 0x31f3, 0x4eaa);
  if (!(F & FZ)) {
    CYCT(0x4eaa, 0x4ead); interactionDelete_hook(gb); return;
  }
  CYC(0x4eaa, 0x4ead);
  CYC(0x4ead, 0x4eb0); push_effect(gb, 0x4eb0);
  boy2_initialize_graphics_and_script(gb, sp0_);
subid0_state1:
  CALL_C(0x4eb0, interactionRunScript_hook, 0x2552, 0x4eb3);
  CYC(0x4eb3, 0x4eb6); npcFaceLinkAndAnimate_hook(gb);
  return;

subid1:
  CALL_C(0x4eb6, checkInteractionState_hook, 0x23fe, 0x4eb9);
  if (!(F & FZ)) {
    CYCT(0x4eb9, 0x4ebb); goto subid1_state1;
  }
  CYC(0x4eb9, 0x4ebb);
  CYC(0x4ebb, 0x4ebe); SET_HL(0x552b);
  CYC(0x4ebe, 0x4ec0); E = 0x09;
  CALL_C(0x4ec0, interBankCall_hook, 0x008a, 0x4ec3);
  CYC(0x4ec3, 0x4ec4); A = B;
  CYC(0x4ec4, 0x4ec6); alu_cp(gb, 3);
  if (!(F & FZ)) {
    CYCT(0x4ec6, 0x4ec9); interactionDelete_hook(gb); return;
  }
  CYC(0x4ec6, 0x4ec9);
  CYC(0x4ec9, 0x4ecc); push_effect(gb, 0x4ecc);
  boy2_initialize_graphics_and_script(gb, sp0_);
subid1_state1:
  CALL_C(0x4ecc, interactionRunScript_hook, 0x2552, 0x4ecf);
  CYC(0x4ecf, 0x4ed2); npcFaceLinkAndAnimate_hook(gb);
  return;

subid2:
  CALL_C(0x4ed2, checkInteractionState_hook, 0x23fe, 0x4ed5);
  if (!(F & FZ)) {
    CYCT(0x4ed5, 0x4ed7); goto subid2_state1;
  }
  CYC(0x4ed5, 0x4ed7);
  CALL_C(0x4ed7, getThisRoomFlags_hook, 0x197d, 0x4eda);
  CYC(0x4eda, 0x4edc); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x4edc, 0x4edf); interactionDelete_hook(gb); return;
  }
  CYC(0x4edc, 0x4edf);
  CYC(0x4edf, 0x4ee2); push_effect(gb, 0x4ee2);
  boy2_init_graphics_and_inc_state(gb, sp0_);
  CYC(0x4ee2, 0x4ee4); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x4ee4, 0x4ee6); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4ee6, 0x4ee7); A = mem_rd(gb, DE);
  CYC(0x4ee7, 0x4ee8); mem_wr(gb, HL, A);
  CYC(0x4ee8, 0x4eeb); objectSetVisiblec2_hook(gb);
  return;

subid2_state1:
  CYC(0x4eeb, 0x4eed); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4eed, 0x4eee); A = mem_rd(gb, DE);
  CYC(0x4eee, 0x4eef); push_effect(gb, 0x4eef);
  switch (boy2_jump_table(gb)) {
    case 0x4ef5: goto substate0;
    case 0x4f09: goto substate1;
    case 0x4f17: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x4ef5, interactionAnimate_hook, 0x261b, 0x4ef8);
  CYC(0x4ef8, 0x4efb); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(0x4efb, 0x4efd); alu_cp(gb, 1);
  if (!(F & FZ)) {
    CYCT(0x4efd, 0x4efe); ret_effect(gb); return;
  }
  CYC(0x4efd, 0x4efe);
  CALL_C(0x4efe, interactionIncSubstate_hook, 0x23e5, 0x4f01);
  CYC(0x4f01, 0x4f04); SET_HL(0x73db);
  CYC(0x4f04, 0x4f06); E = 0x08;
  CYC(0x4f06, 0x4f09); interBankCall_hook(gb);
  return;

substate1:
  CYC(0x4f09, 0x4f0b); C = 0x20;
  CALL_C(0x4f0b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4f0e);
  if (!(F & FZ)) {
    CYCT(0x4f0e, 0x4f0f); ret_effect(gb); return;
  }
  CYC(0x4f0e, 0x4f0f);
  CALL_C(0x4f0f, interactionIncSubstate_hook, 0x23e5, 0x4f12);
  CYC(0x4f12, 0x4f15); push_effect(gb, 0x4f15);
  boy2_initialize_script(gb, sp0_);
  CYC(0x4f15, 0x4f16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4f16, 0x4f17); ret_effect(gb);
  return;

substate2:
  CYC(0x4f17, 0x4f1a); SET_HL(0x7b06);
  CYC(0x4f1a, 0x4f1c); E = 0x08;
  CYC(0x4f1c, 0x4f1f); interBankCall_hook(gb);
  return;

subid3:
  CALL_C(0x4f1f, checkInteractionState_hook, 0x23fe, 0x4f22);
  if (F & FZ) {
    CYCT(0x4f22, 0x4f24); goto subid3_state0;
  }
  CYC(0x4f22, 0x4f24);
  CYC(0x4f24, 0x4f27); SET_HL(0x7ba0);
  CYC(0x4f27, 0x4f29); E = 0x08;
  CYC(0x4f29, 0x4f2c); interBankCall_hook(gb);
  return;

subid3_state0:
  CYC(0x4f2c, 0x4f2f); push_effect(gb, 0x4f2f);
  boy2_init_graphics_and_inc_state(gb, sp0_);
  CYC(0x4f2f, 0x4f31); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4f31, 0x4f33); mem_wr(gb, HL, 0x78);
  CYC(0x4f33, 0x4f35); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x4f35, 0x4f37); mem_wr(gb, HL, 0x02);
  CYC(0x4f37, 0x4f3a); objectSetVisiblec1_hook(gb);
}
