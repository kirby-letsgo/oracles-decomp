#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56bf, 0x56c1); E = 0x44;
  CYC(0x56c1, 0x56c2); A = mem_rd(gb, DE);
  CYC(0x56c2, 0x56c3); push_effect(gb, 0x56c3);
  if (tokay_jump_table(gb) != 0x56c7) { HANDOFF(HL); }
  CYC(0x56c7, 0x56c9); A = 1;
  CYC(0x56c9, 0x56ca); mem_wr(gb, DE, A);
  CALL_C(0x56ca, interactionInitGraphics_hook, 0x15fb, 0x56cd);
  CALL_C(0x56cd, objectSetVisiblec2_hook, 0x1e45, 0x56d0);
  CYC(0x56d0, 0x56d2); A = 0x0a;
  CALL_C(0x56d2, interactionSetHighTextIndex_hook, 0x253b, 0x56d5);
  CALL_ROM(0x56d5, 0x56e0);
  CYC(0x56d8, 0x56da); E = 0x40;
  CYC(0x56da, 0x56db); A = mem_rd(gb, DE);
  CYC(0x56db, 0x56dc); alu_or(gb, A);
  if (F & FZ) { CYC(0x56dc, 0x56df); RET(0x56df); return; }
  CYCT(0x56dc, 0x56df); objectMarkSolidPosition_hook(gb);
}

void tokayState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58d7, 0x58d9); E = 0x42;
  CYC(0x58d9, 0x58da); A = mem_rd(gb, DE);
  uint8_t subid = A;
  CYC(0x58da, 0x58db); push_effect(gb, 0x58db);
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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x591b, 0x591d); E = 0x45;
  CYC(0x591d, 0x591e); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(0x591e, 0x591f); push_effect(gb, 0x591f);
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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x592d, 0x592f); E = 0x42;
  CYC(0x592f, 0x5930); A = mem_rd(gb, DE);
  CYC(0x5930, 0x5932); alu_cp(gb, 2);
  if (F & FZ) CALL_C_CC(0x5932, tokayThief_countdownToStealNextItem_hook, 0x5975, 0x5935); else CYC(0x5932, 0x5935);
  CYC(0x5935, 0x5937); E = 0x79;
  CYC(0x5937, 0x5938); A = mem_rd(gb, DE);
  CYC(0x5938, 0x5939); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(0x5939, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x593c); else CYC(0x5939, 0x593c);
  CALL_C(0x593c, interactionRunScript_hook, 0x2552, 0x593f);
  if (!(F & FC)) { CYCT(0x593f, 0x5940); return; } CYC(0x593f, 0x5940);
  CYC(0x5940, 0x5942); A = 5;
  CALL_C(0x5942, interactionSetAnimation_hook, 0x262e, 0x5945);
  CALL_C(0x5945, interactionIncSubstate_hook, 0x23e5, 0x5948);
  CYC(0x5948, 0x594a); L = 0x42;
  CYC(0x594a, 0x594b); A = mem_rd(gb, HL);
  CYC(0x594b, 0x594c); B = A;
  CYC(0x594c, 0x594d); alu_or(gb, A);
  if (F & FZ) { CYC(0x594d, 0x594f); CYC(0x594f, 0x5951); A = 0x4c; CALL_C(0x5951, playSound_b00_hook, 0x0c98, 0x5954); CYC(0x5954, 0x5955); H = D; }
  else CYCT(0x594d, 0x594f);
  CYC(0x5955, 0x5957); L = 0x46;
  CYC(0x5957, 0x5959); mem_wr(gb, HL, 0x5a);
  tokayInitHeldItem_hook(gb);
}

void tokayInitHeldItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5959, getFreeInteractionSlot_hook, 0x3aef, 0x595c);
  if (!(F & FZ)) { CYCT(0x595c, 0x595d); return; } CYC(0x595c, 0x595d);
  CYC(0x595d, 0x595e); L = alu_inc8(gb, L);
  CYC(0x595e, 0x595f); A = B;
  CYC(0x595f, 0x5962); SET_BC(0x5970);
  CALL_C(0x5962, addAToBc_hook, 0x006d, 0x5965);
  CYC(0x5965, 0x5966); A = mem_rd(gb, BC);
  CYC(0x5966, 0x5967); mem_wr(gb, HL, A); SET_HL(HL - 1);
  tokayInitAccessory_hook(gb);
}

void tokayInitAccessory_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5967, 0x5969); mem_wr(gb, HL, 0x63);
  CYC(0x5969, 0x596b); L = 0x56;
  CYC(0x596b, 0x596d); mem_wr(gb, HL, 0x40);
  CYC(0x596d, 0x596e); L = alu_inc8(gb, L);
  CYC(0x596e, 0x596f); mem_wr(gb, HL, D);
  CYC(0x596f, 0x5970); ret_effect(gb);
}

void tokayThief_countdownToStealNextItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5975, 0x5976); H = D;
  CYC(0x5976, 0x5978); L = 0x78;
  CYC(0x5978, 0x5979); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x5979, 0x597a); ret_effect(gb); return; } CYC(0x5979, 0x597a);
  CYC(0x597a, 0x597c); mem_wr(gb, HL, 0x0a);
  CYC(0x597c, 0x597e); L = 0x7a;
  CYC(0x597e, 0x597f); A = mem_rd(gb, HL);
  CYC(0x597f, 0x5981); alu_cp(gb, 9);
  if (F & FZ) { CYCT(0x5981, 0x5982); ret_effect(gb); return; } CYC(0x5981, 0x5982);
  CYC(0x5982, 0x5983); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5983, 0x5986); SET_HL(0x3d97);
  CYC(0x5986, 0x5987); tokay_add_a_to_hl(gb, 0x5987);
  CYC(0x5987, 0x5988); A = mem_rd(gb, HL);
  CYC(0x5988, 0x598a); alu_cp(gb, 0x19);
  if (!(F & FZ)) { CYCT(0x598a, 0x598c); goto lose; } CYC(0x598a, 0x598c);
  CALL_C(0x598c, loseTreasure_hook, 0x1733, 0x598f);
  CYC(0x598f, 0x5991); A = 0x20;
  CALL_C(0x5991, loseTreasure_hook, 0x1733, 0x5994);
  CYC(0x5994, 0x5996); A = 0x24;
lose:
  CALL_C(0x5996, loseTreasure_hook, 0x1733, 0x5999);
  CYC(0x5999, 0x599b); A = 0x75;
  CYC(0x599b, 0x599e); playSound_b00_hook(gb);
}

void tokayThiefSubstate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x599e, interactionDecCounter1_hook, 0x23cc, 0x59a1);
  if (!(F & FZ)) { CYCT(0x59a1, 0x59a2); ret_effect(gb); return; } CYC(0x59a1, 0x59a2);
  CYC(0x59a2, 0x59a4); L = 0x42;
  CYC(0x59a4, 0x59a5); A = mem_rd(gb, HL);
  CYC(0x59a5, 0x59a7); A = alu_swap(gb, A);
  CYC(0x59a7, 0x59a9); alu_add(gb, 0x14);
  CYC(0x59a9, 0x59ab); L = 0x46;
  CYC(0x59ab, 0x59ac); mem_wr(gb, HL, A);
  CYC(0x59ac, 0x59af); interactionIncSubstate_hook(gb);
}

void tokayThiefSubstate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x59af, interactionAnimate3Times_hook, 0x274f, 0x59b2);
  CALL_C(0x59b2, interactionDecCounter1_hook, 0x23cc, 0x59b5);
  if (!(F & FZ)) { CYCT(0x59b5, 0x59b6); ret_effect(gb); return; } CYC(0x59b5, 0x59b6);
  CYC(0x59b6, 0x59b8); L = 0x49;
  CYC(0x59b8, 0x59ba); mem_wr(gb, HL, 6);
  CYC(0x59ba, 0x59bc); L = 0x50;
  CYC(0x59bc, 0x59be); mem_wr(gb, HL, 0x64);
  tokayThief_jump_hook(gb);
}

void tokayThief_jump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x59be, interactionIncSubstate_hook, 0x23e5, 0x59c1);
  CYC(0x59c1, 0x59c4); SET_BC(0xfe40);
  CALL_C(0x59c4, objectSetSpeedZ_hook, 0x239d, 0x59c7);
  CYC(0x59c7, 0x59c9); A = 5;
  CALL_C(0x59c9, specialObjectSetAnimation_hook, 0x2b0a, 0x59cc);
  CYC(0x59cc, 0x59ce); E = 0x60;
  CYC(0x59ce, 0x59d0); A = 1;
  CYC(0x59d0, 0x59d1); mem_wr(gb, DE, A);
  CALL_C(0x59d1, specialObjectAnimate_hook, 0x2aef, 0x59d4);
  CYC(0x59d4, 0x59d6); A = 0x53;
  CYC(0x59d6, 0x59d9); playSound_b00_hook(gb);
}

void tokayThiefSubstate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x59d9, 0x59db); C = 0x20;
  CALL_C(0x59db, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x59de);
  if (!(F & FZ)) { CYCT(0x59de, 0x59e1); objectApplySpeed_hook(gb); return; } CYC(0x59de, 0x59e1);
  CALL_C(0x59e1, interactionIncSubstate_hook, 0x23e5, 0x59e4);
  CYC(0x59e4, 0x59e6); L = 0x46;
  CYC(0x59e6, 0x59e8); mem_wr(gb, HL, 6);
  CYC(0x59e8, 0x59ea); A = 5;
  CYC(0x59ea, 0x59ed); interactionSetAnimation_hook(gb);
}

void tokayThiefSubstate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x59ed, interactionDecCounter1_hook, 0x23cc, 0x59f0);
  if (!(F & FZ)) { CYCT(0x59f0, 0x59f1); ret_effect(gb); return; } CYC(0x59f0, 0x59f1);
  CYC(0x59f1, 0x59f3); tokayThief_jump_hook(gb);
}

void tokayThiefSubstate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x59f3, objectApplySpeed_hook, 0x201d, 0x59f6);
  CALL_C(0x59f6, objectCheckWithinScreenBoundary_hook, 0x2184, 0x59f9);
  if (F & FC) { CYCT(0x59f9, 0x59fb); CYC(0x5a0d, 0x5a0f); C = 0x20; CYC(0x5a0f, 0x5a12); objectUpdateSpeedZ_paramC_hook(gb); return; }
  CYC(0x59f9, 0x59fb);
  CYC(0x59fb, 0x59fd); E = 0x42;
  CYC(0x59fd, 0x59fe); A = mem_rd(gb, DE);
  CYC(0x59fe, 0x5a00); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(0x5a00, 0x5a02); CYC(0x5a0a, 0x5a0d); interactionDelete_hook(gb); return; }
  CYC(0x5a00, 0x5a02);
  CALL_C(0x5a02, interactionIncSubstate_hook, 0x23e5, 0x5a05);
  CYC(0x5a05, 0x5a07); L = 0x46;
  CYC(0x5a07, 0x5a09); mem_wr(gb, HL, 0x3c);
  CYC(0x5a09, 0x5a0a); ret_effect(gb);
}

void tokayThiefSubstate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a12, interactionDecCounter1_hook, 0x23cc, 0x5a15);
  if (!(F & FZ)) { CYCT(0x5a15, 0x5a16); ret_effect(gb); return; } CYC(0x5a15, 0x5a16);
  CYC(0x5a16, 0x5a17); alu_xor(gb, A);
  CYC(0x5a17, 0x5a1a); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5a1a, 0x5a1d); mem_wr(gb, wUseSimulatedInput, A);
  CYC(0x5a1d, 0x5a20); mem_wr(gb, wMenuDisabled, A);
  CALL_C(0x5a20, getThisRoomFlags_hook, 0x197d, 0x5a23);
  CYC(0x5a23, 0x5a25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(0x5a25, 0x5a28); A = mem_rd(gb, wActiveMusic2);
  CYC(0x5a28, 0x5a2b); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x5a2b, playSound_b00_hook, 0x0c98, 0x5a2e);
  CALL_C(0x5a2e, setDeathRespawnPoint_hook, 0x1100, 0x5a31);
  CYC(0x5a31, 0x5a34); interactionDelete_hook(gb);
}

void tokayRunSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a34, interactionRunScript_hook, 0x2552, 0x5a37);
  if (F & FC) { CYCT(0x5a37, 0x5a3a); interactionDelete_hook(gb); return; } CYC(0x5a37, 0x5a3a);
  CYC(0x5a3a, 0x5a3c); E = 0x7f;
  CYC(0x5a3c, 0x5a3d); A = mem_rd(gb, DE);
  CYC(0x5a3d, 0x5a3e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5a3e, 0x5a41); npcFaceLinkAndAnimate_hook(gb); return; } CYC(0x5a3e, 0x5a41);
  CALL_C(0x5a41, tokayRunStinkBagCutscene_hook, 0x5b7c, 0x5a44);
  CALL_C(0x5a44, interactionAnimate_hook, 0x261b, 0x5a47);
  CYC(0x5a47, 0x5a4a); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void tokayRunSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a4a, interactionRunScript_hook, 0x2552, 0x5a4d);
  CYC(0x5a4d, 0x5a4f); E = 0x7b;
  CYC(0x5a4f, 0x5a50); A = mem_rd(gb, DE);
  CYC(0x5a50, 0x5a51); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5a51, 0x5a54); interactionAnimateAsNpc_hook(gb); return; }
  CYC(0x5a51, 0x5a54); npcFaceLinkAndAnimate_hook(gb);
}

void tokayRunSubid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a57, interactionRunScript_hook, 0x2552, 0x5a5a);
  if (F & FC) { CYCT(0x5a5a, 0x5a5d); interactionDelete_hook(gb); return; }
  CYC(0x5a5a, 0x5a5d); interactionAnimateBasedOnSpeed_hook(gb);
}

void tokayRunSubid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a60, 0x5a62); E = 0x45;
  CYC(0x5a62, 0x5a63); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(0x5a63, 0x5a64); push_effect(gb, 0x5a64);
  tokay_jump_table(gb);
  if (substate == 0) wildTokayParticipantSubstate0_hook(gb);
  else if (substate == 1) wildTokayParticipantSubstate1_hook(gb);
  else if (substate == 2) wildTokayParticipantSubstate2_hook(gb);
  else HANDOFF(HL);
}

void wildTokayParticipantSubstate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a6a, wildTokayParticipant_checkGrabMeat_hook, 0x5a9f, 0x5a6d);
  wildTokayParticipantSubstate2_hook(gb);
}

void wildTokayParticipantSubstate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a6d, objectApplySpeed_hook, 0x201d, 0x5a70);
  CYC(0x5a70, 0x5a72); E = 0x4b;
  CYC(0x5a72, 0x5a73); A = mem_rd(gb, DE);
  CYC(0x5a73, 0x5a75); alu_add(gb, 8);
  CYC(0x5a75, 0x5a77); alu_cp(gb, 0x90);
  if (F & FC) { CYCT(0x5a77, 0x5a7a); interactionAnimateBasedOnSpeed_hook(gb); return; } CYC(0x5a77, 0x5a7a);
  CYC(0x5a7a, 0x5a7c); E = 0x7c;
  CYC(0x5a7c, 0x5a7d); A = mem_rd(gb, DE);
  CYC(0x5a7d, 0x5a7e); alu_or(gb, A);
  if (F & FZ) { CYC(0x5a7e, 0x5a80); CYC(0x5a80, 0x5a82); A = 0xff; CYC(0x5a82, 0x5a85); mem_wr(gb, 0xcfde, A); CYC(0x5a85, 0x5a87); interactionDelete_hook(gb); return; }
  CYCT(0x5a7e, 0x5a80);
  CYC(0x5a87, 0x5a89); E = 0x59;
  CYC(0x5a89, 0x5a8a); A = mem_rd(gb, DE);
  CYC(0x5a8a, 0x5a8b); push_effect(gb, DE);
  CYC(0x5a8b, 0x5a8c); D = A;
  CALL_C(0x5a8c, objectDelete_de_hook, 0x21e3, 0x5a8f);
  CYC(0x5a8f, 0x5a90); SET_DE(pop_effect(gb));
  CYC(0x5a90, 0x5a92); E = 0x5c;
  CYC(0x5a92, 0x5a93); A = mem_rd(gb, DE);
  CYC(0x5a93, 0x5a95); alu_cp(gb, 2);
  if (F & FZ) { CYC(0x5a95, 0x5a97); CYC(0x5a97, 0x5a99); A = 1; CYC(0x5a99, 0x5a9c); mem_wr(gb, 0xcfde, A); }
  else CYCT(0x5a95, 0x5a97);
  CYC(0x5a9c, 0x5a9f); interactionDelete_hook(gb);
}

void wildTokayParticipant_checkGrabMeat_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a9f, 0x5aa2); A = mem_rd(gb, 0xdc00);
  CYC(0x5aa2, 0x5aa3); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5aa3, 0x5aa4); ret_effect(gb); return; } CYC(0x5aa3, 0x5aa4);
  CYC(0x5aa4, 0x5aa7); A = mem_rd(gb, wLinkGrabState);
  CYC(0x5aa7, 0x5aa8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5aa8, 0x5aa9); ret_effect(gb); return; } CYC(0x5aa8, 0x5aa9);
  CYC(0x5aa9, 0x5aab); A = 0x0a;
  CYC(0x5aab, 0x5aae); SET_HL(0xdc0b);
  CYC(0x5aae, 0x5aaf); B = mem_rd(gb, HL);
  CYC(0x5aaf, 0x5ab1); L = 0x0d;
  CYC(0x5ab1, 0x5ab2); C = mem_rd(gb, HL);
  CYC(0x5ab2, 0x5ab3); H = D;
  CYC(0x5ab3, 0x5ab5); L = 0x4b;
  CALL_C(0x5ab5, checkObjectIsCloseToPosition_b00_hook, 0x3d59, 0x5ab8);
  if (!(F & FC)) { CYCT(0x5ab8, 0x5ab9); ret_effect(gb); return; } CYC(0x5ab8, 0x5ab9);
  CALL_C(0x5ab9, interactionIncSubstate_hook, 0x23e5, 0x5abc);
  CYC(0x5abc, 0x5abe); L = 0x7c;
  CYC(0x5abe, 0x5abf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5abf, 0x5ac1); L = 0x46;
  CYC(0x5ac1, 0x5ac3); mem_wr(gb, HL, 6);
  CYC(0x5ac3, 0x5ac5); A = 7;
  CYC(0x5ac5, 0x5ac7); L = 0x48;
  CYC(0x5ac7, 0x5ac8); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x5ac8, interactionSetAnimation_hook, 0x262e, 0x5acb);
  CYC(0x5acb, 0x5acc); push_effect(gb, DE);
  CYC(0x5acc, 0x5acf); SET_DE(0xdc00);
  CALL_C(0x5acf, objectDelete_de_hook, 0x21e3, 0x5ad2);
  CYC(0x5ad2, 0x5ad5); SET_HL(0xcfda);
  CYC(0x5ad5, 0x5ad6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5ad6, 0x5ad7); E = mem_rd(gb, HL);
  CYC(0x5ad7, 0x5ad8); D = A;
  CALL_C(0x5ad8, objectDelete_de_hook, 0x21e3, 0x5adb);
  CYC(0x5adb, 0x5adc); SET_DE(pop_effect(gb));
  CYC(0x5adc, 0x5ade); A = 0x6c;
  CALL_C(0x5ade, playSound_b00_hook, 0x0c98, 0x5ae1);
  tokayInitMeatAccessory_hook(gb);
}

void tokayInitMeatAccessory_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ae1, getFreeInteractionSlot_hook, 0x3aef, 0x5ae4);
  if (!(F & FZ)) { CYCT(0x5ae4, 0x5ae5); ret_effect(gb); return; } CYC(0x5ae4, 0x5ae5);
  CYC(0x5ae5, 0x5ae7); mem_wr(gb, HL, 0x63);
  CYC(0x5ae7, 0x5ae8); L = alu_inc8(gb, L);
  CYC(0x5ae8, 0x5aea); mem_wr(gb, HL, 0x73);
  CYC(0x5aea, 0x5aeb); L = alu_inc8(gb, L);
  CYC(0x5aeb, 0x5aec); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5aec, 0x5aee); L = 0x56;
  CYC(0x5aee, 0x5af0); mem_wr(gb, HL, 0x40);
  CYC(0x5af0, 0x5af1); L = alu_inc8(gb, L);
  CYC(0x5af1, 0x5af2); mem_wr(gb, HL, D);
  CYC(0x5af2, 0x5af4); E = 0x59;
  CYC(0x5af4, 0x5af5); A = H;
  CYC(0x5af5, 0x5af6); mem_wr(gb, DE, A);
  CYC(0x5af6, 0x5af7); ret_effect(gb);
}

void wildTokayParticipantSubstate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5af7, interactionDecCounter1_hook, 0x23cc, 0x5afa);
  if (!(F & FZ)) { CYCT(0x5afa, 0x5afb); ret_effect(gb); return; } CYC(0x5afa, 0x5afb);
  CYC(0x5afb, 0x5afe); interactionIncSubstate_hook(gb);
}

void tokayRunSubid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5afe, 0x5b00); E = 0x45;
  CYC(0x5b00, 0x5b01); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(0x5b01, 0x5b02); push_effect(gb, 0x5b02);
  tokay_jump_table(gb);
  if (substate == 0) {
    CYC(0x5b06, 0x5b09); A = mem_rd(gb, wPaletteThread_mode);
    CYC(0x5b09, 0x5b0a); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(0x5b0a, 0x5b0b); ret_effect(gb); return; } CYC(0x5b0a, 0x5b0b);
    CALL_C(0x5b0b, interactionRunScript_hook, 0x2552, 0x5b0e);
    if (!(F & FC)) { CYCT(0x5b0e, 0x5b11); interactionAnimateAsNpc_hook(gb); return; } CYC(0x5b0e, 0x5b11);
    CALL_C(0x5b11, getFreeInteractionSlot_hook, 0x3aef, 0x5b14);
    if (!(F & FZ)) { CYCT(0x5b14, 0x5b15); ret_effect(gb); return; } CYC(0x5b14, 0x5b15);
    CYC(0x5b15, 0x5b17); mem_wr(gb, HL, 0x70);
    CALL_C(0x5b17, interactionIncSubstate_hook, 0x23e5, 0x5b1a);
    CYC(0x5b1a, 0x5b1c); A = 0xfb;
    CALL_C(0x5b1c, playSound_b00_hook, 0x0c98, 0x5b1f);
    CYC(0x5b1f, 0x5b22); fadeoutToWhite_hook(gb);
    return;
  }
  if (substate == 1) {
    CYC(0x5b22, 0x5b25); A = mem_rd(gb, wPaletteThread_mode);
    CYC(0x5b25, 0x5b26); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(0x5b26, 0x5b27); ret_effect(gb); return; } CYC(0x5b26, 0x5b27);
    CYC(0x5b27, 0x5b28); push_effect(gb, DE);
    CALL_C(0x5b28, clearAllItemsAndPutLinkOnGround_hook, 0x19ad, 0x5b2b);
    CYC(0x5b2b, 0x5b2c); SET_DE(pop_effect(gb));
    CYC(0x5b2c, 0x5b2e); E = 0x42;
    CYC(0x5b2e, 0x5b2f); A = mem_rd(gb, DE);
    CYC(0x5b2f, 0x5b31); alu_cp(gb, 0x19);
    if (F & FZ) { CYC(0x5b31, 0x5b33); CYC(0x5b33, 0x5b35); A = 1; CYC(0x5b35, 0x5b38); mem_wr(gb, wTmpcfc0_wildTokay_inPresent, A); }
    else CYCT(0x5b31, 0x5b33);
    CYC(0x5b38, 0x5b3b); interactionDelete_hook(gb);
    return;
  }
  HANDOFF(HL);
}

void tokayRunSubid0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b3b, 0x5b3e); A = mem_rd(gb, wScrollMode);
  CYC(0x5b3e, 0x5b40); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x5b40, 0x5b41); ret_effect(gb); return; } CYC(0x5b40, 0x5b41);
  tokayRunSubid10_hook(gb);
}

void tokayRunSubid10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b41, 0x5b44); A = mem_rd(gb, 0xd13e);
  CYC(0x5b44, 0x5b46); alu_and(gb, 4);
  if (F & FZ) { CYC(0x5b46, 0x5b48); tokayRunSubid0e_hook(gb); return; }
  CYCT(0x5b46, 0x5b48);
  CALL_C(0x5b4b, interactionRunScript_hook, 0x2552, 0x5b4e);
  if (!(F & FC)) { CYCT(0x5b4e, 0x5b4f); ret_effect(gb); return; } CYC(0x5b4e, 0x5b4f);
  CYC(0x5b4f, 0x5b52); interactionDelete_hook(gb);
}

void tokayRunSubid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b48, interactionAnimateAsNpc_hook, 0x26db, 0x5b4b);
  CALL_C(0x5b4b, interactionRunScript_hook, 0x2552, 0x5b4e);
  if (!(F & FC)) { CYCT(0x5b4e, 0x5b4f); ret_effect(gb); return; } CYC(0x5b4e, 0x5b4f);
  CYC(0x5b4f, 0x5b52); interactionDelete_hook(gb);
}

void tokayRunSubid1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b52, 0x5b54); C = 0x10;
  CALL_C(0x5b54, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5b57);
  CALL_C(0x5b57, interactionAnimateAsNpc_hook, 0x26db, 0x5b5a);
  CALL_C(0x5b5a, getThisRoomFlags_hook, 0x197d, 0x5b5d);
  CYC(0x5b5d, 0x5b5f); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x5b5f, 0x5b62); interactionRunScript_hook(gb); return; } CYC(0x5b5f, 0x5b62);
  CYC(0x5b62, 0x5b64); C = 0x18;
  CALL_C(0x5b64, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x5b67);
  if (!(F & FC)) { CYCT(0x5b67, 0x5b68); ret_effect(gb); return; } CYC(0x5b67, 0x5b68);
  CYC(0x5b68, 0x5b6a); E = 0x71;
  CYC(0x5b6a, 0x5b6b); mem_wr(gb, DE, A);
  CYC(0x5b6b, 0x5b6e); interactionRunScript_hook(gb);
}

void tokayRunSubid12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b6e, interactionRunScript_hook, 0x2552, 0x5b71);
  CYC(0x5b71, 0x5b74); npcFaceLinkAndAnimate_hook(gb);
}

void tokayRunSubid1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b74, 0x5b77); A = mem_rd(gb, wTmpcfc0_wildTokay_inPresent);
  CYC(0x5b77, 0x5b78); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b78, 0x5b79); ret_effect(gb); return; } CYC(0x5b78, 0x5b79);
  CYC(0x5b79, 0x5b7c); interactionDelete_hook(gb);
}

void tokayRunStinkBagCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b7c, 0x5b7e); E = 0x45;
  CYC(0x5b7e, 0x5b7f); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(0x5b7f, 0x5b80); push_effect(gb, 0x5b80);
  tokay_jump_table(gb);
  if (substate == 0 || substate == 2) {
    if (substate == 0) { CYC(0x5b86, 0x5b87); H = D; CYC(0x5b87, 0x5b89); L = 0x50; CYC(0x5b89, 0x5b8b); mem_wr(gb, HL, 0x78); }
    if (substate == 2) { CYC(0x5c04, 0x5c05); H = D; CYC(0x5c05, 0x5c07); L = 0x7b; CYC(0x5c07, 0x5c08); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(0x5c08, 0x5c09); A = mem_rd(gb, HL); CYC(0x5c09, 0x5c0b); alu_cp(gb, 6); if (!(F & FC)) { CYC(0x5c0b, 0x5c0d); CYC(0x5c0d, 0x5c0f); mem_wr(gb, HL, 0); } else CYCT(0x5c0b, 0x5c0d); CYC(0x5c0f, 0x5c12); }
    CYC(0x5b8b, 0x5b8c); H = D; CYC(0x5b8c, 0x5b8e); L = 0x4b; CYC(0x5b8e, 0x5b8f); A = mem_rd(gb, HL); CYC(0x5b8f, 0x5b91); L = 0x79; CYC(0x5b91, 0x5b92); mem_wr(gb, HL, A);
    CYC(0x5b92, 0x5b94); L = 0x4d; CYC(0x5b94, 0x5b95); A = mem_rd(gb, HL); CYC(0x5b95, 0x5b97); L = 0x7a; CYC(0x5b97, 0x5b98); mem_wr(gb, HL, A);
    CYC(0x5b98, 0x5b99); H = D; CYC(0x5b99, 0x5b9b); L = 0x45; CYC(0x5b9b, 0x5b9d); mem_wr(gb, HL, 1); CYC(0x5b9d, 0x5b9f); L = 0x7e; CYC(0x5b9f, 0x5ba1); mem_wr(gb, HL, 1);
    CALL_ROM(0x5ba1, 0x5ba9); CYC(0x5ba4, 0x5ba6); A = 0x53; CYC(0x5ba6, 0x5ba9); playSound_b00_hook(gb); return;
  }
  if (substate == 1) {
    CYC(0x5bde, 0x5be0); E = 0x7c; CYC(0x5be0, 0x5be1); A = mem_rd(gb, DE); CYC(0x5be1, 0x5be2); C = A;
    CALL_C(0x5be2, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5be5);
    if (!(F & FZ)) { CYCT(0x5be5, 0x5be8); objectApplySpeed_hook(gb); return; } CYC(0x5be5, 0x5be8);
    CALL_C(0x5be8, interactionIncSubstate_hook, 0x23e5, 0x5beb);
    CYC(0x5beb, 0x5bed); L = 0x7b; CYC(0x5bed, 0x5bee); A = mem_rd(gb, HL); CYC(0x5bee, 0x5bf0); alu_cp(gb, 5);
    if (!(F & FZ)) { CYCT(0x5bf0, 0x5bf1); ret_effect(gb); return; } CYC(0x5bf0, 0x5bf1);
    CYC(0x5bf1, 0x5bf3); L = 0x4a; CYC(0x5bf3, 0x5bf5); mem_wr(gb, HL, 0); CYC(0x5bf5, 0x5bf6); L = alu_inc8(gb, L); CYC(0x5bf6, 0x5bf8); mem_wr(gb, HL, 0x28);
    CYC(0x5bf8, 0x5bfa); L = 0x4c; CYC(0x5bfa, 0x5bfc); mem_wr(gb, HL, 0); CYC(0x5bfc, 0x5bfd); L = alu_inc8(gb, L); CYC(0x5bfd, 0x5bff); mem_wr(gb, HL, 0x48); CYC(0x5bff, 0x5c01); L = 0x7e; CYC(0x5c01, 0x5c03); mem_wr(gb, HL, 0); CYC(0x5c03, 0x5c04); ret_effect(gb); return;
  }
  HANDOFF(HL);
}

void tokayLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c12, 0x5c14); E = 0x42;
  CYC(0x5c14, 0x5c15); A = mem_rd(gb, DE);
  CYC(0x5c15, 0x5c18); SET_HL(0x5c1f);
  CYC(0x5c18, 0x5c19); tokay_add_double_index(gb, 0x5c19);
  CYC(0x5c19, 0x5c1a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5c1a, 0x5c1b); H = mem_rd(gb, HL);
  CYC(0x5c1b, 0x5c1c); L = A;
  CYC(0x5c1c, 0x5c1f); interactionSetScript_hook(gb);
}
