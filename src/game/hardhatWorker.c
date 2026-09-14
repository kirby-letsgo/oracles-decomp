#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t hardhat_worker_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A); burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L); burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A; burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void hardhat_worker_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void hardhat_worker_load_script_and_init_graphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b79, interactionInitGraphics_hook, 0x15fb, 0x6b7c);
  CALL_C(0x6b7c, objectMarkSolidPosition_hook, 0x24f0, 0x6b7f);
  CYC(0x6b7f, 0x6b81); A = 0x10;
  CALL_C(0x6b81, interactionSetHighTextIndex_hook, 0x253b, 0x6b84);
  CYC(0x6b84, 0x6b86); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6b86, 0x6b87); A = mem_rd(gb, DE);
  CYC(0x6b87, 0x6b8a); SET_HL(0x6b94);
  CYC(0x6b8a, 0x6b8b); hardhat_worker_add_double_index(gb, 0x6b8b);
  CYC(0x6b8b, 0x6b8c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6b8c, 0x6b8d); H = mem_rd(gb, HL);
  CYC(0x6b8d, 0x6b8e); L = A;
  CALL_C(0x6b8e, interactionSetScript_hook, 0x2544, 0x6b91);
  CYC(0x6b91, 0x6b94); interactionIncState_hook(gb);
}

void interactionCode58_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6aec, 0x6aee); E = INTERACTION_BASE + OBJ_SUBID; CYC(0x6aee, 0x6aef); A = mem_rd(gb, DE); CYC(0x6aef, 0x6af0); push_effect(gb, 0x6af0);
  switch (hardhat_worker_jump_table(gb)) {
    case 0x6af8: goto subid00; case 0x6b11: goto subid01; case 0x6b28: goto subid02; case 0x6b52: goto subid03; default: HANDOFF(HL);
  }
subid00:
  CALL_C(0x6af8, checkInteractionState_hook, 0x23fe, 0x6afb);
  if (F & FZ) { CYC(0x6afb, 0x6afd); CYC(0x6afd, 0x6b00); push_effect(gb, 0x6b00); hardhat_worker_load_script_and_init_graphics_hook(gb); CALL_C(0x6b00, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6b03); CYC(0x6b03, 0x6b05); A = 4; CALL_C(0x6b05, interactionSetAnimation_hook, 0x262e, 0x6b08); } else CYCT(0x6afb, 0x6afd);
  CALL_C(0x6b08, interactionRunScript_hook, 0x2552, 0x6b0b);
  if (F & FC) { CYCT(0x6b0b, 0x6b0e); interactionDelete_hook(gb); return; }
  CYC(0x6b0b, 0x6b0e); CYC(0x6b0e, 0x6b11); interactionAnimateAsNpc_hook(gb); return;
subid01:
  CALL_C(0x6b11, checkInteractionState_hook, 0x23fe, 0x6b14);
  if (F & FZ) { CYC(0x6b14, 0x6b16); CYC(0x6b16, 0x6b19); push_effect(gb, 0x6b19); hardhat_worker_load_script_and_init_graphics_hook(gb); CALL_C(0x6b19, interactionRunScript_hook, 0x2552, 0x6b1c); CALL_C(0x6b1c, interactionRunScript_hook, 0x2552, 0x6b1f); } else CYCT(0x6b14, 0x6b16);
  CALL_C(0x6b1f, interactionRunScript_hook, 0x2552, 0x6b22);
  if (F & FC) { CYCT(0x6b22, 0x6b25); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(0x6b22, 0x6b25); CYC(0x6b25, 0x6b28); npcFaceLinkAndAnimate_hook(gb); return;
subid02:
  CALL_C(0x6b28, checkInteractionState_hook, 0x23fe, 0x6b2b);
  if (F & FZ) { CYC(0x6b2b, 0x6b2d); CYC(0x6b2d, 0x6b30); A = W8(wEssencesObtained); CYC(0x6b30, 0x6b32); alu_bit(gb, 3, A); if (!(F & FZ)) { CYCT(0x6b32, 0x6b35); interactionDelete_hook(gb); return; } CYC(0x6b32, 0x6b35); CALL_C(0x6b35, getThisRoomFlags_hook, 0x197d, 0x6b38); CYC(0x6b38, 0x6b3a); alu_bit(gb, 7, A); if (F & FZ) CYCT(0x6b3a, 0x6b3c); else { CYC(0x6b3a, 0x6b3c); CYC(0x6b3c, 0x6b3f); SET_BC(0x3858); CALL_C(0x6b3f, interactionSetPosition_hook, 0x2773, 0x6b42); } CYC(0x6b42, 0x6b45); push_effect(gb, 0x6b45); hardhat_worker_load_script_and_init_graphics_hook(gb); } else CYCT(0x6b2b, 0x6b2d);
  CALL_C(0x6b45, interactionRunScript_hook, 0x2552, 0x6b48); CYC(0x6b48, 0x6b4a); E = INTERACTION_BASE + OBJ_VAR38; CYC(0x6b4a, 0x6b4b); A = mem_rd(gb, DE); CYC(0x6b4b, 0x6b4c); alu_or(gb, A); if (F & FZ) { CYCT(0x6b4c, 0x6b4f); npcFaceLinkAndAnimate_hook(gb); } else { CYC(0x6b4c, 0x6b4f); interactionAnimateAsNpc_hook(gb); } return;
subid03:
  CALL_C(0x6b52, checkInteractionState_hook, 0x23fe, 0x6b55);
  if (F & FZ) { CYC(0x6b55, 0x6b57); CYC(0x6b57, 0x6b5a); push_effect(gb, 0x6b5a); hardhat_worker_load_script_and_init_graphics_hook(gb); CALL_C(0x6b5a, interactionRunScript_hook, 0x2552, 0x6b5d); } else CYCT(0x6b55, 0x6b57);
  CALL_C(0x6b5d, interactionRunScript_hook, 0x2552, 0x6b60);
  if (F & FC) { CYCT(0x6b60, 0x6b63); interactionDelete_hook(gb); return; }
  CYC(0x6b60, 0x6b63); CYC(0x6b63, 0x6b65); E = INTERACTION_BASE + OBJ_VAR3F; CYC(0x6b65, 0x6b66); A = mem_rd(gb, DE); CYC(0x6b66, 0x6b67); alu_or(gb, A); if (F & FZ) { CYCT(0x6b67, 0x6b6a); npcFaceLinkAndAnimate_hook(gb); return; } CYC(0x6b67, 0x6b6a); CALL_C(0x6b6a, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x6b6d); CYC(0x6b6d, 0x6b70); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}
