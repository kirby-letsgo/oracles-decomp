#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t tokay_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void tokay_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void tokay_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void tokayState1_hook(GB *gb);
void tokayRunSubid00_hook(GB *gb);
void tokayThiefSubstate0_hook(GB *gb);
void tokayInitHeldItem_hook(GB *gb);
void tokayInitAccessory_hook(GB *gb);
void tokayThief_countdownToStealNextItem_hook(GB *gb);
void tokayThiefSubstate1_hook(GB *gb);
void tokayThiefSubstate2_hook(GB *gb);
void tokayThief_jump_hook(GB *gb);
void tokayThiefSubstate3_hook(GB *gb);
void tokayThiefSubstate4_hook(GB *gb);
void tokayThiefSubstate5_hook(GB *gb);
void tokayThiefSubstate6_hook(GB *gb);
void tokayRunSubid05_hook(GB *gb);
void tokayRunSubid06_hook(GB *gb);
void tokayRunSubid0b_hook(GB *gb);
void tokayRunSubid0c_hook(GB *gb);
void wildTokayParticipantSubstate0_hook(GB *gb);
void wildTokayParticipantSubstate1_hook(GB *gb);
void wildTokayParticipantSubstate2_hook(GB *gb);
void wildTokayParticipant_checkGrabMeat_hook(GB *gb);
void tokayInitMeatAccessory_hook(GB *gb);
void tokayRunSubid0d_hook(GB *gb);
void tokayRunSubid0e_hook(GB *gb);
void tokayRunSubid0f_hook(GB *gb);
void tokayRunSubid10_hook(GB *gb);
void tokayRunSubid1e_hook(GB *gb);
void tokayRunSubid12_hook(GB *gb);
void tokayRunSubid1a_hook(GB *gb);
void tokayRunStinkBagCutscene_hook(GB *gb);
void tokayLoadScript_hook(GB *gb);

void interactionCode48_hook(GB *gb) {
  BASE(interactionCode48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  if (tokay_jump_table(gb) != b_+8) { HANDOFF(HL); }
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+19); A = 0x0a;
  CALL_C(b_+19, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+22);
  CALL_ROM(b_+22, b_+33);
  CYC(b_+25, b_+27); E = 0x40;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYC(b_+29, b_+32); RET(b_+32); return; }
  CYCT(b_+29, b_+32); objectMarkSolidPosition_hook(gb);
}

void tokayState1_hook(GB *gb) {
  BASE(tokayState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t subid = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  switch (subid) {
    case 0: case 1: case 2: case 3: case 4: tokayRunSubid00_hook(gb); return;
    case 5: tokayRunSubid05_hook(gb); return;
    case 6: case 7: case 8: case 9: case 10: case 29: tokayRunSubid06_hook(gb); return;
    case 11: tokayRunSubid0b_hook(gb); return;
    case 12: tokayRunSubid0c_hook(gb); return;
    case 13: case 25: tokayRunSubid0d_hook(gb); return;
    case 14: case 17: tokayRunSubid0e_hook(gb); return;
    case 15: tokayRunSubid0f_hook(gb); return;
    case 16: tokayRunSubid10_hook(gb); return;
    case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 31: tokayRunSubid12_hook(gb); return;
    case 26: case 27: case 28: tokayRunSubid1a_hook(gb); return;
    case 30: tokayRunSubid1e_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void tokayRunSubid00_hook(GB *gb) {
  BASE(tokayRunSubid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  switch (substate) {
    case 0: tokayThiefSubstate0_hook(gb); return;
    case 1: tokayThiefSubstate1_hook(gb); return;
    case 2: tokayThiefSubstate2_hook(gb); return;
    case 3: tokayThiefSubstate3_hook(gb); return;
    case 4: tokayThiefSubstate4_hook(gb); return;
    case 5: tokayThiefSubstate5_hook(gb); return;
    case 6: tokayThiefSubstate6_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void tokayThiefSubstate0_hook(GB *gb) {
  BASE(tokayThiefSubstate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 2);
  if (F & FZ) CALL_C_CC(b_+5, tokayThief_countdownToStealNextItem_hook, SYM(tokayThief_countdownToStealNextItem), b_+8); else CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); E = 0x79;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+12, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+15); else CYC(b_+12, b_+15);
  CALL_C(b_+15, interactionRunScript_hook, SYM(interactionRunScript), b_+18);
  if (!(F & FC)) { CYCT(b_+18, b_+19); return; } CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); A = 5;
  CALL_C(b_+21, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+24);
  CALL_C(b_+24, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+27);
  CYC(b_+27, b_+29); L = 0x42;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) { CYC(b_+32, b_+34); CYC(b_+34, b_+36); A = 0x4c; CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39); CYC(b_+39, b_+40); H = D; }
  else CYCT(b_+32, b_+34);
  CYC(b_+40, b_+42); L = 0x46;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x5a);
  tokayInitHeldItem_hook(gb);
}

void tokayInitHeldItem_hook(GB *gb) {
  BASE(tokayInitHeldItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+9); SET_BC(SYM(tokayItemGraphics));
  CALL_C(b_+9, addAToBc_hook, 0x006d, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL - 1);
  tokayInitAccessory_hook(gb);
}

void tokayInitAccessory_hook(GB *gb) {
  BASE(tokayInitAccessory);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x63);
  CYC(b_+2, b_+4); L = 0x56;
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x40);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, D);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void tokayThief_countdownToStealNextItem_hook(GB *gb) {
  BASE(tokayThief_countdownToStealNextItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x78;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0a);
  CYC(b_+7, b_+9); L = 0x7a;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 9);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; } CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+17); SET_HL(SYM(tokayIslandStolenItems));
  CYC(b_+17, b_+18); tokay_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0x19);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto lose; } CYC(b_+21, b_+23);
  CALL_C(b_+23, loseTreasure_hook, SYM(loseTreasure), b_+26);
  CYC(b_+26, b_+28); A = 0x20;
  CALL_C(b_+28, loseTreasure_hook, SYM(loseTreasure), b_+31);
  CYC(b_+31, b_+33); A = 0x24;
lose:
  CALL_C(b_+33, loseTreasure_hook, SYM(loseTreasure), b_+36);
  CYC(b_+36, b_+38); A = 0x75;
  CYC(b_+38, b_+41); playSound_b00_hook(gb);
}

void tokayThiefSubstate1_hook(GB *gb) {
  BASE(tokayThiefSubstate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = 0x42;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+11); alu_add(gb, 0x14);
  CYC(b_+11, b_+13); L = 0x46;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+17); interactionIncSubstate_hook(gb);
}

void tokayThiefSubstate2_hook(GB *gb) {
  BASE(tokayThiefSubstate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimate3Times_hook, SYM(interactionAnimate3Times), b_+3);
  CALL_C(b_+3, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; } CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x49;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 6);
  CYC(b_+11, b_+13); L = 0x50;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x64);
  tokayThief_jump_hook(gb);
}

void tokayThief_jump_hook(GB *gb) {
  BASE(tokayThief_jump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+3);
  CYC(b_+3, b_+6); SET_BC(0xfe40);
  CALL_C(b_+6, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+9);
  CYC(b_+9, b_+11); A = 5;
  CALL_C(b_+11, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+14);
  CYC(b_+14, b_+16); E = 0x60;
  CYC(b_+16, b_+18); A = 1;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+22);
  CYC(b_+22, b_+24); A = 0x53;
  CYC(b_+24, b_+27); playSound_b00_hook(gb);
}

void tokayThiefSubstate3_hook(GB *gb) {
  BASE(tokayThiefSubstate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); objectApplySpeed_hook(gb); return; } CYC(b_+5, b_+8);
  CALL_C(b_+8, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+11);
  CYC(b_+11, b_+13); L = 0x46;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 6);
  CYC(b_+15, b_+17); A = 5;
  CYC(b_+17, b_+20); interactionSetAnimation_hook(gb);
}

void tokayThiefSubstate4_hook(GB *gb) {
  BASE(tokayThiefSubstate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); tokayThief_jump_hook(gb);
}

void tokayThiefSubstate5_hook(GB *gb) {
  BASE(tokayThiefSubstate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+6);
  if (F & FC) { CYCT(b_+6, b_+8); CYC(b_+26, b_+28); C = 0x20; CYC(b_+28, b_+31); objectUpdateSpeedZ_paramC_hook(gb); return; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = 0x42;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); CYC(b_+23, b_+26); interactionDelete_hook(gb); return; }
  CYC(b_+13, b_+15);
  CALL_C(b_+15, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+18);
  CYC(b_+18, b_+20); L = 0x46;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x3c);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void tokayThiefSubstate6_hook(GB *gb) {
  BASE(tokayThiefSubstate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+8, b_+11); mem_wr(gb, wUseSimulatedInput, A);
  CYC(b_+11, b_+14); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+14, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(b_+19, b_+22); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+22, b_+25); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+25, playSound_b00_hook, SYM(playSound_b00), b_+28);
  CALL_C(b_+28, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+31);
  CYC(b_+31, b_+34); interactionDelete_hook(gb);
}

void tokayRunSubid05_hook(GB *gb) {
  BASE(tokayRunSubid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; } CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); E = 0x7f;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+13); npcFaceLinkAndAnimate_hook(gb); return; } CYC(b_+10, b_+13);
  CALL_C(b_+13, tokayRunStinkBagCutscene_hook, SYM(tokayRunStinkBagCutscene), b_+16);
  CALL_C(b_+16, interactionAnimate_hook, SYM(interactionAnimate), b_+19);
  CYC(b_+19, b_+22); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void tokayRunSubid06_hook(GB *gb) {
  BASE(tokayRunSubid06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = 0x7b;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+10); interactionAnimateAsNpc_hook(gb); return; }
  CYC(b_+7, b_+10); npcFaceLinkAndAnimate_hook(gb);
}

void tokayRunSubid0b_hook(GB *gb) {
  BASE(tokayRunSubid0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+6); interactionAnimateBasedOnSpeed_hook(gb);
}

void tokayRunSubid0c_hook(GB *gb) {
  BASE(tokayRunSubid0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  if (substate == 0) wildTokayParticipantSubstate0_hook(gb);
  else if (substate == 1) wildTokayParticipantSubstate1_hook(gb);
  else if (substate == 2) wildTokayParticipantSubstate2_hook(gb);
  else HANDOFF(HL);
}

void wildTokayParticipantSubstate0_hook(GB *gb) {
  BASE(wildTokayParticipantSubstate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, wildTokayParticipant_checkGrabMeat_hook, SYM(wildTokayParticipant_checkGrabMeat), SYM(wildTokayParticipantSubstate2));
  wildTokayParticipantSubstate2_hook(gb);
}

void wildTokayParticipantSubstate2_hook(GB *gb) {
  BASE(wildTokayParticipantSubstate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); E = 0x4b;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_add(gb, 8);
  CYC(b_+8, b_+10); alu_cp(gb, 0x90);
  if (F & FC) { CYCT(b_+10, b_+13); interactionAnimateBasedOnSpeed_hook(gb); return; } CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = 0x7c;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { CYC(b_+17, b_+19); CYC(b_+19, b_+21); A = 0xff; CYC(b_+21, b_+24); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A); CYC(b_+24, b_+26); interactionDelete_hook(gb); return; }
  CYCT(b_+17, b_+19);
  CYC(b_+26, b_+28); E = 0x59;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); push_effect(gb, DE);
  CYC(b_+30, b_+31); D = A;
  CALL_C(b_+31, objectDelete_de_hook, SYM(objectDelete_de), b_+34);
  CYC(b_+34, b_+35); SET_DE(pop_effect(gb));
  CYC(b_+35, b_+37); E = 0x5c;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+40); alu_cp(gb, 2);
  if (F & FZ) { CYC(b_+40, b_+42); CYC(b_+42, b_+44); A = 1; CYC(b_+44, b_+47); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A); }
  else CYCT(b_+40, b_+42);
  CYC(b_+47, b_+50); interactionDelete_hook(gb);
}

void wildTokayParticipant_checkGrabMeat_hook(GB *gb) {
  BASE(wildTokayParticipant_checkGrabMeat);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1ReservedItemC);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; } CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x0a;
  CYC(b_+12, b_+15); SET_HL(w1ReservedItemC_yh);
  CYC(b_+15, b_+16); B = mem_rd(gb, HL);
  CYC(b_+16, b_+18); L = 0x0d;
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = 0x4b;
  CALL_C(b_+22, checkObjectIsCloseToPosition_b00_hook, SYM(checkObjectIsCloseToPosition_b00), b_+25);
  if (!(F & FC)) { CYCT(b_+25, b_+26); ret_effect(gb); return; } CYC(b_+25, b_+26);
  CALL_C(b_+26, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+29);
  CYC(b_+29, b_+31); L = 0x7c;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+34); L = 0x46;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 6);
  CYC(b_+36, b_+38); A = 7;
  CYC(b_+38, b_+40); L = 0x48;
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+41, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+44);
  CYC(b_+44, b_+45); push_effect(gb, DE);
  CYC(b_+45, b_+48); SET_DE(w1ReservedItemC);
  CALL_C(b_+48, objectDelete_de_hook, SYM(objectDelete_de), b_+51);
  CYC(b_+51, b_+54); SET_HL(wTmpcfc0_goronDance_roundIndex);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+55, b_+56); E = mem_rd(gb, HL);
  CYC(b_+56, b_+57); D = A;
  CALL_C(b_+57, objectDelete_de_hook, SYM(objectDelete_de), b_+60);
  CYC(b_+60, b_+61); SET_DE(pop_effect(gb));
  CYC(b_+61, b_+63); A = 0x6c;
  CALL_C(b_+63, playSound_b00_hook, SYM(playSound_b00), SYM(tokayInitMeatAccessory));
  tokayInitMeatAccessory_hook(gb);
}

void tokayInitMeatAccessory_hook(GB *gb) {
  BASE(tokayInitMeatAccessory);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x63);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x73);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = 0x56;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x40);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+17); mem_wr(gb, HL, D);
  CYC(b_+17, b_+19); E = 0x59;
  CYC(b_+19, b_+20); A = H;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void wildTokayParticipantSubstate1_hook(GB *gb) {
  BASE(wildTokayParticipantSubstate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); interactionIncSubstate_hook(gb);
}

void tokayRunSubid0d_hook(GB *gb) {
  BASE(tokayRunSubid0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  if (substate == 0) {
    CYC(b_+8, b_+11); A = mem_rd(gb, wPaletteThread_mode);
    CYC(b_+11, b_+12); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; } CYC(b_+12, b_+13);
    CALL_C(b_+13, interactionRunScript_hook, SYM(interactionRunScript), b_+16);
    if (!(F & FC)) { CYCT(b_+16, b_+19); interactionAnimateAsNpc_hook(gb); return; } CYC(b_+16, b_+19);
    CALL_C(b_+19, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+22);
    if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; } CYC(b_+22, b_+23);
    CYC(b_+23, b_+25); mem_wr(gb, HL, 0x70);
    CALL_C(b_+25, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+28);
    CYC(b_+28, b_+30); A = 0xfb;
    CALL_C(b_+30, playSound_b00_hook, SYM(playSound_b00), b_+33);
    CYC(b_+33, b_+36); fadeoutToWhite_hook(gb);
    return;
  }
  if (substate == 1) {
    CYC(b_+36, b_+39); A = mem_rd(gb, wPaletteThread_mode);
    CYC(b_+39, b_+40); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+40, b_+41); ret_effect(gb); return; } CYC(b_+40, b_+41);
    CYC(b_+41, b_+42); push_effect(gb, DE);
    CALL_C(b_+42, clearAllItemsAndPutLinkOnGround_hook, SYM(clearAllItemsAndPutLinkOnGround), b_+45);
    CYC(b_+45, b_+46); SET_DE(pop_effect(gb));
    CYC(b_+46, b_+48); E = 0x42;
    CYC(b_+48, b_+49); A = mem_rd(gb, DE);
    CYC(b_+49, b_+51); alu_cp(gb, 0x19);
    if (F & FZ) { CYC(b_+51, b_+53); CYC(b_+53, b_+55); A = 1; CYC(b_+55, b_+58); mem_wr(gb, wTmpcfc0_wildTokay_inPresent, A); }
    else CYCT(b_+51, b_+53);
    CYC(b_+58, b_+61); interactionDelete_hook(gb);
    return;
  }
  HANDOFF(HL);
}

void tokayRunSubid0f_hook(GB *gb) {
  BASE(tokayRunSubid0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  tokayRunSubid10_hook(gb);
}

void tokayRunSubid10_hook(GB *gb) {
  BASE(tokayRunSubid10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Companion_var3e);
  CYC(b_+3, b_+5); alu_and(gb, 4);
  if (F & FZ) { CYC(b_+5, b_+7); tokayRunSubid0e_hook(gb); return; }
  CYCT(b_+5, b_+7);
  CALL_C((SYM(tokayRunSubid0e) + 3), interactionRunScript_hook, SYM(interactionRunScript), (SYM(tokayRunSubid0e) + 6));
  if (!(F & FC)) { CYCT((SYM(tokayRunSubid0e) + 6), (SYM(tokayRunSubid0e) + 7)); ret_effect(gb); return; } CYC((SYM(tokayRunSubid0e) + 6), (SYM(tokayRunSubid0e) + 7));
  CYC((SYM(tokayRunSubid0e) + 7), (SYM(tokayRunSubid0e) + 10)); interactionDelete_hook(gb);
}

void tokayRunSubid0e_hook(GB *gb) {
  BASE(tokayRunSubid0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+3);
  CALL_C(b_+3, interactionRunScript_hook, SYM(interactionRunScript), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+7); ret_effect(gb); return; } CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); interactionDelete_hook(gb);
}

void tokayRunSubid1e_hook(GB *gb) {
  BASE(tokayRunSubid1e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+8);
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); interactionRunScript_hook(gb); return; } CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); C = 0x18;
  CALL_C(b_+18, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+22); ret_effect(gb); return; } CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = 0x71;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); interactionRunScript_hook(gb);
}

void tokayRunSubid12_hook(GB *gb) {
  BASE(tokayRunSubid12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6); npcFaceLinkAndAnimate_hook(gb);
}

void tokayRunSubid1a_hook(GB *gb) {
  BASE(tokayRunSubid1a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_wildTokay_inPresent);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); interactionDelete_hook(gb);
}

void tokayRunStinkBagCutscene_hook(GB *gb) {
  BASE(tokayRunStinkBagCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  if (substate == 0 || substate == 2) {
    if (substate == 0) { CYC(b_+10, b_+11); H = D; CYC(b_+11, b_+13); L = 0x50; CYC(b_+13, b_+15); mem_wr(gb, HL, 0x78); }
    if (substate == 2) { CYC(b_+136, b_+137); H = D; CYC(b_+137, b_+139); L = 0x7b; CYC(b_+139, b_+140); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(b_+140, b_+141); A = mem_rd(gb, HL); CYC(b_+141, b_+143); alu_cp(gb, 6); if (!(F & FC)) { CYC(b_+143, b_+145); CYC(b_+145, b_+147); mem_wr(gb, HL, 0); } else CYCT(b_+143, b_+145); CYC(b_+147, b_+150); }
    CYC(b_+15, b_+16); H = D; CYC(b_+16, b_+18); L = 0x4b; CYC(b_+18, b_+19); A = mem_rd(gb, HL); CYC(b_+19, b_+21); L = 0x79; CYC(b_+21, b_+22); mem_wr(gb, HL, A);
    CYC(b_+22, b_+24); L = 0x4d; CYC(b_+24, b_+25); A = mem_rd(gb, HL); CYC(b_+25, b_+27); L = 0x7a; CYC(b_+27, b_+28); mem_wr(gb, HL, A);
    CYC(b_+28, b_+29); H = D; CYC(b_+29, b_+31); L = 0x45; CYC(b_+31, b_+33); mem_wr(gb, HL, 1); CYC(b_+33, b_+35); L = 0x7e; CYC(b_+35, b_+37); mem_wr(gb, HL, 1);
    CALL_ROM(b_+37, b_+45); CYC(b_+40, b_+42); A = 0x53; CYC(b_+42, b_+45); playSound_b00_hook(gb); return;
  }
  if (substate == 1) {
    CYC(b_+98, b_+100); E = 0x7c; CYC(b_+100, b_+101); A = mem_rd(gb, DE); CYC(b_+101, b_+102); C = A;
    CALL_C(b_+102, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+105);
    if (!(F & FZ)) { CYCT(b_+105, b_+108); objectApplySpeed_hook(gb); return; } CYC(b_+105, b_+108);
    CALL_C(b_+108, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+111);
    CYC(b_+111, b_+113); L = 0x7b; CYC(b_+113, b_+114); A = mem_rd(gb, HL); CYC(b_+114, b_+116); alu_cp(gb, 5);
    if (!(F & FZ)) { CYCT(b_+116, b_+117); ret_effect(gb); return; } CYC(b_+116, b_+117);
    CYC(b_+117, b_+119); L = 0x4a; CYC(b_+119, b_+121); mem_wr(gb, HL, 0); CYC(b_+121, b_+122); L = alu_inc8(gb, L); CYC(b_+122, b_+124); mem_wr(gb, HL, 0x28);
    CYC(b_+124, b_+126); L = 0x4c; CYC(b_+126, b_+128); mem_wr(gb, HL, 0); CYC(b_+128, b_+129); L = alu_inc8(gb, L); CYC(b_+129, b_+131); mem_wr(gb, HL, 0x48); CYC(b_+131, b_+133); L = 0x7e; CYC(b_+133, b_+135); mem_wr(gb, HL, 0); CYC(b_+135, b_+136); ret_effect(gb); return;
  }
  HANDOFF(HL);
}

void tokayLoadScript_hook(GB *gb) {
  BASE(tokayLoadScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(tokayScriptTable));
  CYC(b_+6, b_+7); tokay_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); interactionSetScript_hook(gb);
}
