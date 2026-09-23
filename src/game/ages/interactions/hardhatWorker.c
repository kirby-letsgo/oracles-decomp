#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode58);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+141, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+144);
  CALL_C(b_+144, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+147);
  CYC(b_+147, b_+149); A = 0x10;
  CALL_C(b_+149, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+152);
  CYC(b_+152, b_+154); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+158); SET_HL(b_+168);
  CYC(b_+158, b_+159); hardhat_worker_add_double_index(gb, b_+159);
  CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+160, b_+161); H = mem_rd(gb, HL);
  CYC(b_+161, b_+162); L = A;
  CALL_C(b_+162, interactionSetScript_hook, SYM(interactionSetScript), b_+165);
  CYC(b_+165, b_+168); TAIL(interactionIncState);
}

void interactionCode58_hook(GB *gb) {
  BASE(interactionCode58);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID; CYC(b_+2, b_+3); A = mem_rd(gb, DE); CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (hardhat_worker_jump_table(gb));
    if (jt_ == b_+12) { goto subid00; }
    else if (jt_ == b_+37) { goto subid01; }
    else if (jt_ == b_+60) { goto subid02; }
    else if (jt_ == b_+102) { goto subid03; }
    else { HANDOFF(HL); }
  } while (0);
subid00:
  CALL_C(b_+12, checkInteractionState_hook, SYM(checkInteractionState), b_+15);
  if (F & FZ) { CYC(b_+15, b_+17); CYC(b_+17, b_+20); push_effect(gb, b_+20); hardhat_worker_load_script_and_init_graphics_hook(gb); CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23); CYC(b_+23, b_+25); A = 4; CALL_C(b_+25, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+28); } else CYCT(b_+15, b_+17);
  CALL_C(b_+28, interactionRunScript_hook, SYM(interactionRunScript), b_+31);
  if (F & FC) { CYCT(b_+31, b_+34); interactionDelete_hook(gb); return; }
  CYC(b_+31, b_+34); CYC(b_+34, b_+37); interactionAnimateAsNpc_hook(gb); return;
subid01:
  CALL_C(b_+37, checkInteractionState_hook, SYM(checkInteractionState), b_+40);
  if (F & FZ) { CYC(b_+40, b_+42); CYC(b_+42, b_+45); push_effect(gb, b_+45); hardhat_worker_load_script_and_init_graphics_hook(gb); CALL_C(b_+45, interactionRunScript_hook, SYM(interactionRunScript), b_+48); CALL_C(b_+48, interactionRunScript_hook, SYM(interactionRunScript), b_+51); } else CYCT(b_+40, b_+42);
  CALL_C(b_+51, interactionRunScript_hook, SYM(interactionRunScript), b_+54);
  if (F & FC) { CYCT(b_+54, b_+57); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(b_+54, b_+57); CYC(b_+57, b_+60); npcFaceLinkAndAnimate_hook(gb); return;
subid02:
  CALL_C(b_+60, checkInteractionState_hook, SYM(checkInteractionState), b_+63);
  if (F & FZ) { CYC(b_+63, b_+65); CYC(b_+65, b_+68); A = W8(wEssencesObtained); CYC(b_+68, b_+70); alu_bit(gb, 3, A); if (!(F & FZ)) { CYCT(b_+70, b_+73); interactionDelete_hook(gb); return; } CYC(b_+70, b_+73); CALL_C(b_+73, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+76); CYC(b_+76, b_+78); alu_bit(gb, 7, A); if (F & FZ) CYCT(b_+78, b_+80); else { CYC(b_+78, b_+80); CYC(b_+80, b_+83); SET_BC(0x3858); CALL_C(b_+83, interactionSetPosition_hook, SYM(interactionSetPosition), b_+86); } CYC(b_+86, b_+89); push_effect(gb, b_+89); hardhat_worker_load_script_and_init_graphics_hook(gb); } else CYCT(b_+63, b_+65);
  CALL_C(b_+89, interactionRunScript_hook, SYM(interactionRunScript), b_+92); CYC(b_+92, b_+94); E = INTERACTION_BASE + OBJ_VAR38; CYC(b_+94, b_+95); A = mem_rd(gb, DE); CYC(b_+95, b_+96); alu_or(gb, A); if (F & FZ) { CYCT(b_+96, b_+99); TAIL(npcFaceLinkAndAnimate); } CYC(b_+96, b_+99); CYC(b_+99, b_+102); TAIL(interactionAnimateAsNpc);
subid03:
  CALL_C(b_+102, checkInteractionState_hook, SYM(checkInteractionState), b_+105);
  if (F & FZ) { CYC(b_+105, b_+107); CYC(b_+107, b_+110); push_effect(gb, b_+110); hardhat_worker_load_script_and_init_graphics_hook(gb); CALL_C(b_+110, interactionRunScript_hook, SYM(interactionRunScript), b_+113); } else CYCT(b_+105, b_+107);
  CALL_C(b_+113, interactionRunScript_hook, SYM(interactionRunScript), b_+116);
  if (F & FC) { CYCT(b_+116, b_+119); interactionDelete_hook(gb); return; }
  CYC(b_+116, b_+119); CYC(b_+119, b_+121); E = INTERACTION_BASE + OBJ_VAR3F; CYC(b_+121, b_+122); A = mem_rd(gb, DE); CYC(b_+122, b_+123); alu_or(gb, A); if (F & FZ) { CYCT(b_+123, b_+126); npcFaceLinkAndAnimate_hook(gb); return; } CYC(b_+123, b_+126); CALL_C(b_+126, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+129); CYC(b_+129, b_+132); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}
