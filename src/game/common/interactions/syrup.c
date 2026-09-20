#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t syrup_jump_table(GB *gb) {
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

void interactionCode5f__runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ea4, 0x6ea6); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6ea6, 0x6ea7); A = mem_rd(gb, DE);
  CYC(0x6ea7, 0x6ea8); push_effect(gb, 0x6ea8);
  switch (syrup_jump_table(gb)) {
    case 0x6eae: goto state0;
    case 0x6ec8: goto state1;
    case 0x6f3e: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x6eae, 0x6eb0); A = 1;
  CYC(0x6eb0, 0x6eb1); mem_wr(gb, DE, A);
  CALL_C(0x6eb1, interactionInitGraphics_hook, 0x15fb, 0x6eb4);
  CALL_C(0x6eb4, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6eb7);
  CYC(0x6eb7, 0x6eb9); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x6eb9, 0x6ebb); mem_wr(gb, HL, 0x12);
  CYC(0x6ebb, 0x6ebc); L = alu_inc8(gb, L);
  CYC(0x6ebc, 0x6ebe); mem_wr(gb, HL, 7);
  CYC(0x6ebe, 0x6ec0); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x6ec0, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x6ec3);
  CYC(0x6ec3, 0x6ec6); SET_HL(0x675f);
  CYC(0x6ec6, 0x6ec8); goto set_script_and_state2;

state1:
  CYC(0x6ec8, 0x6eca); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x6eca, 0x6ecb); A = mem_rd(gb, DE);
  CYC(0x6ecb, 0x6ecc); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6ecc, 0x6ecd); ret_effect(gb); return; }
  CYC(0x6ecc, 0x6ecd);
  CYC(0x6ecd, 0x6ece); alu_xor(gb, A);
  CYC(0x6ece, 0x6ecf); mem_wr(gb, DE, A);
  CYC(0x6ecf, 0x6ed1); A = 0x81;
  CYC(0x6ed1, 0x6ed4); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6ed4, 0x6ed7); A = mem_rd(gb, wLinkGrabState);
  CYC(0x6ed7, 0x6ed8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6ed8, 0x6eda); goto talk_without_item; }
  CYC(0x6ed8, 0x6eda);
  CYC(0x6eda, 0x6edd); A = mem_rd(gb, w1Link_relatedObj2 + 1);
  CYC(0x6edd, 0x6ede); H = A;
  CYC(0x6ede, 0x6ee0); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x6ee0, 0x6ee1); mem_wr(gb, DE, A);
  CYC(0x6ee1, 0x6ee3); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6ee3, 0x6ee4); A = mem_rd(gb, HL);
  CYC(0x6ee4, 0x6ee5); push_effect(gb, AF);
  CYC(0x6ee5, 0x6ee6); B = A;
  CYC(0x6ee6, 0x6ee8); alu_sub(gb, 7);
  CYC(0x6ee8, 0x6eea); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x6eea, 0x6eeb); mem_wr(gb, DE, A);
  CYC(0x6eeb, 0x6eec); A = B;
  CYC(0x6eec, 0x6eef); SET_HL(0x44ba);
  CYC(0x6eef, 0x6ef0); push_effect(gb, 0x6ef0);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(0x6ef0, 0x6ef1); A = mem_rd(gb, HL);
  CALL_C(0x6ef1, cpRupeeValue_hook, 0x1765, 0x6ef4);
  CYC(0x6ef4, 0x6ef7); mem_wr(gb, wShopHaveEnoughRupees, A);
  CYC(0x6ef7, 0x6efa); mem_wr(gb, wcbad, A);
  CYC(0x6efa, 0x6efb); SET_AF(pop_effect(gb));
  CYC(0x6efb, 0x6efd); alu_cp(gb, 7);
  if (F & FZ) { CYCT(0x6efd, 0x6eff); goto check_potion; }
  CYC(0x6efd, 0x6eff);
  CYC(0x6eff, 0x6f01); alu_cp(gb, 9);
  if (F & FZ) { CYCT(0x6f01, 0x6f03); goto check_potion; }
  CYC(0x6f01, 0x6f03);
  CYC(0x6f03, 0x6f05); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(0x6f05, 0x6f07); goto check_bombchus; }
  CYC(0x6f05, 0x6f07);
  CYC(0x6f07, 0x6f0a); A = mem_rd(gb, wNumGashaSeeds);
  CYC(0x6f0a, 0x6f0c); goto check_quantity;

check_bombchus:
  CYC(0x6f0c, 0x6f0f); A = mem_rd(gb, wNumBombchus);
check_quantity:
  CYC(0x6f0f, 0x6f11); alu_cp(gb, 0x99);
  CYC(0x6f11, 0x6f13); A = 1;
  if (!(F & FC)) { CYCT(0x6f13, 0x6f15); goto set_can_purchase; }
  CYC(0x6f13, 0x6f15);
  CYC(0x6f15, 0x6f17); goto can_purchase;

check_potion:
  CYC(0x6f17, 0x6f19); A = 0x2f;
  CALL_C(0x6f19, checkTreasureObtained_hook, 0x1748, 0x6f1c);
  CYC(0x6f1c, 0x6f1e); A = 1;
  if (F & FC) { CYCT(0x6f1e, 0x6f20); goto set_can_purchase; }
  CYC(0x6f1e, 0x6f20);
can_purchase:
  CYC(0x6f20, 0x6f21); alu_xor(gb, A);
set_can_purchase:
  CYC(0x6f21, 0x6f23); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6f23, 0x6f24); mem_wr(gb, DE, A);
  CYC(0x6f24, 0x6f27); SET_HL(0x6777);
  CYC(0x6f27, 0x6f29); goto set_script_and_state2;

talk_without_item:
  CALL_C(0x6f29, shopkeeperCheckAllItemsBought_hook, 0x42ae, 0x6f2c);
  if (F & FZ) { CYCT(0x6f2c, 0x6f2e); goto show_welcome_text; }
  CYC(0x6f2c, 0x6f2e);
  CYC(0x6f2e, 0x6f31); SET_HL(0x6773);
  CYC(0x6f31, 0x6f33); goto set_script_and_state2;
show_welcome_text:
  CYC(0x6f33, 0x6f36); SET_HL(0x676f);
set_script_and_state2:
  CYC(0x6f36, 0x6f38); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6f38, 0x6f3a); A = 2;
  CYC(0x6f3a, 0x6f3b); mem_wr(gb, DE, A);
  CYC(0x6f3b, 0x6f3e); interactionSetScript_hook(gb);
  return;

state2:
  CALL_C(0x6f3e, interactionRunScript_hook, 0x2552, 0x6f41);
  if (!(F & FC)) { CYCT(0x6f41, 0x6f42); ret_effect(gb); return; }
  CYC(0x6f41, 0x6f42);
  CYC(0x6f42, 0x6f43); alu_xor(gb, A);
  CYC(0x6f43, 0x6f46); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6f46, 0x6f48); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6f48, 0x6f49); A = mem_rd(gb, DE);
  CYC(0x6f49, 0x6f4a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6f4a, 0x6f4c); goto goto_state1; }
  CYC(0x6f4a, 0x6f4c);
  CYC(0x6f4c, 0x6f4d); A = alu_inc8(gb, A);
  CYC(0x6f4d, 0x6f4f); C = 3;
  if (!(F & FZ)) { CYCT(0x6f4f, 0x6f51); goto set_held_item_state; }
  CYC(0x6f4f, 0x6f51);
  CYC(0x6f51, 0x6f53); C = 4;
set_held_item_state:
  CYC(0x6f53, 0x6f54); alu_xor(gb, A);
  CYC(0x6f54, 0x6f55); mem_wr(gb, DE, A);
  CYC(0x6f55, 0x6f57); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x6f57, 0x6f58); A = mem_rd(gb, DE);
  CYC(0x6f58, 0x6f59); H = A;
  CYC(0x6f59, 0x6f5b); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6f5b, 0x6f5c); mem_wr(gb, HL, C);
  CALL_C(0x6f5c, dropLinkHeldItem_hook, 0x2c43, 0x6f5f);
goto_state1:
  CYC(0x6f5f, 0x6f61); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6f61, 0x6f63); A = 1;
  CYC(0x6f63, 0x6f64); mem_wr(gb, DE, A);
  CYC(0x6f64, 0x6f65); ret_effect(gb);
}

void interactionCode5f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e96, 0x6e99); SET_HL(0x4000);
  CYC(0x6e99, 0x6e9b); E = 9;
  CALL_C(0x6e9b, interBankCall_hook, 0x008a, 0x6e9e);
  CALL_C(0x6e9e, interactionCode5f__runState_hook, 0x6ea4, 0x6ea1);
  CYC(0x6ea1, 0x6ea4); interactionAnimateAsNpc_hook(gb);
}

void interactionCode5f__afterCall6ea1_hook(GB *gb) {
  CYC(0x6ea1, 0x6ea4); interactionAnimateAsNpc_hook(gb);
}
