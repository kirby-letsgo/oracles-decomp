#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// mainScripts.* (bank $0c), scriptHelp.* (bank $15) and agesInteractionsBank0a.* (bank $0a),
// referenced by address only.
#define ralphSubid00Script_bank0c 0x57ec
#define ralphSubid01Script_bank0c 0x5893
#define ralphSubid02Script_bank0c 0x588f
#define ralphSubid03Script_bank0c 0x58b6
#define ralphSubid04Script_part1_bank0c 0x58ba
#define ralphSubid04Script_part2_bank0c 0x58c9
#define ralphSubid04Script_part3_bank0c 0x58e9
#define ralphSubid05Script_bank0c 0x5913
#define ralphSubid06Script_part1_bank0c 0x5944
#define ralphSubid06Script_part2_bank0c 0x5969
#define ralphSubid07Script_bank0c 0x59a6
#define ralphSubid08Script_bank0c 0x59d4
#define ralphSubid09Script_bank0c 0x59e9
#define ralphSubid0aScript_unlinked_bank0c 0x5a02
#define ralphSubid0aScript_linked_bank0c 0x5a13
#define ralphSubid0bScript_bank0c 0x5a43
#define ralphSubid10Script_bank0c 0x5a47
#define ralphSubid0cScript_bank0c 0x5a4b
#define ralphSubid0dScript_bank0c 0x5a4f
#define ralphSubid0eScript_bank0c 0x5a7d
#define ralphSubid11Script_bank0c 0x5aae
#define ralphSubid12Script_bank0c 0x5ac7
#define ralph_createLinkedSwordAnimation_bank15 0x563a
#define objectWritePositionTocfd5_bank15 0x741b
#define loadAngleAndCounterPreset_bank0a 0x76b8

void ralphState0_hook(GB *gb);
void ralphRunSubid_hook(GB *gb);
void ralphSubid00_hook(GB *gb);
void ralphSubid01_hook(GB *gb);
void ralphSubid02_hook(GB *gb);
void ralphSubid03_hook(GB *gb);
void ralphSubid04_hook(GB *gb);
void ralphSubid05_hook(GB *gb);
void ralphRunScript_hook(GB *gb);
void ralphSubid06_hook(GB *gb);
void ralphSubid07_hook(GB *gb);
void ralphAnimateBasedOnSpeedAndRunScript_hook(GB *gb);
void ralphSubid07Substate1_hook(GB *gb);
void ralphSubid07Substate2_hook(GB *gb);
void ralphSubid08_hook(GB *gb);
void ralphSubid09_hook(GB *gb);
void ralphSubid0a_hook(GB *gb);
void ralphSubid0a_linked_hook(GB *gb);
void ralphSubid0b_hook(GB *gb);
void ralphRunScriptWithConditionalAnimation_hook(GB *gb);
void ralphSubid0e_hook(GB *gb);
void ralphRunScriptAndDeleteWhenOver_hook(GB *gb);
void ralphSubid12_hook(GB *gb);
void ralphFunc_738b_hook(GB *gb);
void ralphTurnLinkTowardSelf_hook(GB *gb);
void startJump_hook(GB *gb);

static uint16_t ralph_jumpTable(GB *gb) {
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

// ralphState0@initSubid: per-subid initialization. Reached only by a static `call` from
// ralphState0.
static void ralph_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x6d9a, 0x6d9c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6d9c, 0x6d9d); A = mem_rd(gb, DE);
  CYC(0x6d9d, 0x6d9e); push_effect(gb, 0x6d9e);
  switch (ralph_jumpTable(gb)) {
    case 0x6dda: goto initSubid00;
    case 0x6ee0: goto initSubid01;
    case 0x6de1: goto initSubid02;
    case 0x6df2: goto initSubid03;
    case 0x6e1b: goto initSubid04;
    case 0x6dc4: goto initSubid06;
    case 0x6e39: goto initSubid07;
    case 0x6e42: goto initSubid08;
    case 0x6e53: goto initSubid09;
    case 0x6e92: goto initSubid0a;
    case 0x6eec: goto initSubid0b;
    case 0x6f4d: goto initSubid0c;
    case 0x6f8c: goto initSubid0d;
    case 0x6ed1: goto initSubid0e;
    case 0x6edb: goto initSubid0f;
    case 0x6f1b: goto initSubid10;
    case 0x6f38: goto initSubid11;
    case 0x6f75: goto initSubid12;
    default: HANDOFF(HL);
  }

initSubid06:
  CYC(0x6dc4, 0x6dc7); SET_HL(ralphSubid06Script_part1_bank0c);
  CYC(0x6dc7, 0x6dca); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6dca, 0x6dcc); alu_cp(gb, 0x0b);
  if (!(F & FZ)) {
    CYCT(0x6dcc, 0x6dce); goto initSubid06_setScript;
  }
  CYC(0x6dcc, 0x6dce);
  CYC(0x6dce, 0x6dd1); SET_BC(0x4850);
  CALL_C(0x6dd1, interactionSetPosition_hook, 0x2773, 0x6dd4);
  CYC(0x6dd4, 0x6dd7); SET_HL(ralphSubid06Script_part2_bank0c);
initSubid06_setScript:
  CALL_C(0x6dd7, interactionSetScript_hook, 0x2544, 0x6dda);

initSubid00:
  // Also @initSubid05
  CYC(0x6dda, 0x6ddb); alu_xor(gb, A);

setAnimation:
  CALL_C(0x6ddb, interactionSetAnimation_hook, 0x262e, 0x6dde);
  CYC(0x6dde, 0x6de1); objectSetVisiblec2_hook(gb);
  return;

initSubid02:
  CYC(0x6de1, 0x6de3); A = 0x09;
  CALL_C(0x6de3, interactionSetAnimation_hook, 0x262e, 0x6de6);
  CYC(0x6de6, 0x6de9); SET_HL(ralphSubid02Script_bank0c);
  CALL_C(0x6de9, interactionSetScript_hook, 0x2544, 0x6dec);
  CALL_C(0x6dec, interactionLoadExtraGraphics_hook, 0x2781, 0x6def);
  CYC(0x6def, 0x6df2); objectSetVisiblec2_hook(gb);
  return;

initSubid03:
  CYC(0x6df2, 0x6df4); A = 0x15; // GLOBALFLAG_GAVE_ROPE_TO_RAFTON
  CALL_C(0x6df4, checkGlobalFlag_hook, 0x31f3, 0x6df7);
  if (F & FZ) {
    CYCT(0x6df7, 0x6dfa); interactionDelete_hook(gb); return;
  }
  CYC(0x6df7, 0x6dfa);
  CALL_C(0x6dfa, getThisRoomFlags_hook, 0x197d, 0x6dfd);
  CYC(0x6dfd, 0x6dff); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x6dff, 0x6e02); interactionDelete_hook(gb); return;
  }
  CYC(0x6dff, 0x6e02);
  CYC(0x6e02, 0x6e04); A = 0x01;
  CYC(0x6e04, 0x6e07); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6e07, 0x6e0a); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6e0a, 0x6e0c); A = 0x03;
  CALL_C(0x6e0c, interactionSetAnimation_hook, 0x262e, 0x6e0f);
  CYC(0x6e0f, 0x6e10); H = D;
  CYC(0x6e10, 0x6e12); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6e12, 0x6e14); mem_wr(gb, HL, 0x78);
  CYC(0x6e14, 0x6e16); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6e16, 0x6e18); mem_wr(gb, HL, 0x01);
  CYC(0x6e18, 0x6e1b); objectSetVisiblec2_hook(gb);
  return;

initSubid04:
  CYC(0x6e1b, 0x6e1d); A = 0x01;
  CALL_C(0x6e1d, interactionSetAnimation_hook, 0x262e, 0x6e20);
  CYC(0x6e20, 0x6e23); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6e23, 0x6e25); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x6e25, 0x6e27); goto initSubid04_part2;
  }
  CYC(0x6e25, 0x6e27);
  CYC(0x6e27, 0x6e2a); SET_HL(ralphSubid04Script_part1_bank0c);
  CALL_C(0x6e2a, interactionSetScript_hook, 0x2544, 0x6e2d);
  CYC(0x6e2d, 0x6e30); objectSetInvisible_hook(gb);
  return;
initSubid04_part2:
  CYC(0x6e30, 0x6e33); SET_HL(ralphSubid04Script_part2_bank0c);
  CALL_C(0x6e33, interactionSetScript_hook, 0x2544, 0x6e36);
  CYC(0x6e36, 0x6e39); objectSetVisiblec2_hook(gb);
  return;

initSubid07:
  CYC(0x6e39, 0x6e3c); SET_HL(ralphSubid07Script_bank0c);
  CALL_C(0x6e3c, interactionSetScript_hook, 0x2544, 0x6e3f);
  CYC(0x6e3f, 0x6e42); objectSetInvisible_hook(gb);
  return;

initSubid08:
  // callab scriptHelp.ralph_createLinkedSwordAnimation
  CYC(0x6e42, 0x6e45); SET_HL(ralph_createLinkedSwordAnimation_bank15);
  CYC(0x6e45, 0x6e47); E = 0x15;
  CALL_C(0x6e47, interBankCall_hook, 0x008a, 0x6e4a);
  CYC(0x6e4a, 0x6e4d); SET_HL(ralphSubid08Script_bank0c);
  CALL_C(0x6e4d, interactionSetScript_hook, 0x2544, 0x6e50);
  CYC(0x6e50, 0x6e53); objectSetVisiblec2_hook(gb);
  return;

initSubid09:
  CYC(0x6e53, 0x6e55); A = 0x32; // GLOBALFLAG_RALPH_ENTERED_AMBIS_PALACE
  CALL_C(0x6e55, checkGlobalFlag_hook, 0x31f3, 0x6e58);
  if (!(F & FZ)) {
    CYCT(0x6e58, 0x6e5a); goto deleteSelf;
  }
  CYC(0x6e58, 0x6e5a);
  // Check that we have the 5th essence
  CYC(0x6e5a, 0x6e5c); A = 0x40; // TREASURE_ESSENCE
  CALL_C(0x6e5c, checkTreasureObtained_hook, 0x1748, 0x6e5f);
  if (!(F & FC)) {
    CYCT(0x6e5f, 0x6e61); goto deleteSelf;
  }
  CYC(0x6e5f, 0x6e61);
  CYC(0x6e61, 0x6e63); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x6e63, 0x6e65); goto initSubid09_hasEssence;
  }
  CYC(0x6e63, 0x6e65);
deleteSelf:
  CYC(0x6e65, 0x6e68); interactionDelete_hook(gb);
  return;
initSubid09_hasEssence:
  CYC(0x6e68, 0x6e6a); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6e6a, 0x6e6c); A = 0x50; // SPEED_200
  CYC(0x6e6c, 0x6e6d); mem_wr(gb, DE, A);
  CYC(0x6e6d, 0x6e6f); A = 0x35; // MUS_RALPH
  CYC(0x6e6f, 0x6e72); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x6e72, playSound_b00_hook, 0x0c98, 0x6e75);
  CALL_C(0x6e75, setLinkForceStateToState08_hook, 0x2aad, 0x6e78);
  CYC(0x6e78, 0x6e79); A = alu_inc8(gb, A);
  CYC(0x6e79, 0x6e7c); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6e7c, 0x6e7f); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6e7f, 0x6e82); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(0x6e82, 0x6e85); W8(w1Link_direction) = A;
  CYC(0x6e85, 0x6e88); SET_HL(ralphSubid09Script_bank0c);
  CALL_C(0x6e88, interactionSetScript_hook, 0x2544, 0x6e8b);
  CYC(0x6e8b, 0x6e8c); alu_xor(gb, A);
  CALL_C(0x6e8c, interactionSetAnimation_hook, 0x262e, 0x6e8f);
  CYC(0x6e8f, 0x6e92); objectSetVisiblec2_hook(gb);
  return;

initSubid0a:
  CYC(0x6e92, 0x6e94); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x6e94, checkTreasureObtained_hook, 0x1748, 0x6e97);
  if (!(F & FC)) {
    CYCT(0x6e97, 0x6e9a); interactionDelete_hook(gb); return;
  }
  CYC(0x6e97, 0x6e9a);
  CYC(0x6e9a, 0x6e9c); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x6e9c, checkGlobalFlag_hook, 0x31f3, 0x6e9f);
  if (!(F & FZ)) {
    CYCT(0x6e9f, 0x6ea2); interactionDelete_hook(gb); return;
  }
  CYC(0x6e9f, 0x6ea2);
  CYC(0x6ea2, 0x6ea4); A = 0x45; // GLOBALFLAG_RALPH_ENTERED_BLACK_TOWER
  CALL_C(0x6ea4, checkGlobalFlag_hook, 0x31f3, 0x6ea7);
  if (!(F & FZ)) {
    CYCT(0x6ea7, 0x6eaa); interactionDelete_hook(gb); return;
  }
  CYC(0x6ea7, 0x6eaa);
  CALL_C(0x6eaa, checkIsLinkedGame_hook, 0x1992, 0x6ead);
  CYC(0x6ead, 0x6eb0); SET_HL(ralphSubid0aScript_unlinked_bank0c);
  if (F & FZ) {
    CYCT(0x6eb0, 0x6eb2); goto initSubid0a_setScript;
  }
  CYC(0x6eb0, 0x6eb2);
  // Linked game: adjust position, load a different script
  CYC(0x6eb2, 0x6eb3); H = D;
  CYC(0x6eb3, 0x6eb5); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6eb5, 0x6eb7); mem_wr(gb, HL, 0x50);
  CYC(0x6eb7, 0x6eb9); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6eb9, 0x6ebb); mem_wr(gb, HL, 0x1e);
  CYC(0x6ebb, 0x6ebe); SET_HL(ralphSubid0aScript_linked_bank0c);
initSubid0a_setScript:
  CALL_C(0x6ebe, interactionSetScript_hook, 0x2544, 0x6ec1);
  CALL_C(0x6ec1, setLinkForceStateToState08_hook, 0x2aad, 0x6ec4);
  CYC(0x6ec4, 0x6ec7); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(0x6ec7, 0x6ec8); A = alu_inc8(gb, A);
  CYC(0x6ec8, 0x6ecb); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6ecb, 0x6ece); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6ece, 0x6ed1); objectSetVisiblec2_hook(gb);
  return;

initSubid0e:
  CYC(0x6ed1, 0x6ed3); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6ed3, 0x6ed5); A = 0xff;
  CYC(0x6ed5, 0x6ed6); mem_wr(gb, DE, A);
  CYC(0x6ed6, 0x6ed9); SET_HL(ralphSubid0eScript_bank0c);
  CYC(0x6ed9, 0x6edb); goto setScriptAndRunState1;

initSubid0f:
  CYC(0x6edb, 0x6edd); A = 0x01;
  CYC(0x6edd, 0x6ee0); goto setAnimation;

initSubid01:
  CYC(0x6ee0, 0x6ee3); SET_HL(ralphSubid01Script_bank0c);

setScriptAndRunState1:
  CALL_C(0x6ee3, interactionSetScript_hook, 0x2544, 0x6ee6);
  CYC(0x6ee6, 0x6ee9); ralphRunSubid_hook(gb);
  return;

delete_:
  CYC(0x6ee9, 0x6eec); interactionDelete_hook(gb);
  return;

initSubid0b:
  CYC(0x6eec, 0x6eee); A = 0x26; // TREASURE_TUNE_OF_CURRENTS
  CALL_C(0x6eee, checkTreasureObtained_hook, 0x1748, 0x6ef1);
  if (F & FC) {
    CYCT(0x6ef1, 0x6ef3); goto delete_;
  }
  CYC(0x6ef1, 0x6ef3);
  CALL_C(0x6ef3, getThisRoomFlags_hook, 0x197d, 0x6ef6);
  CYC(0x6ef6, 0x6ef8); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x6ef8, 0x6efa); goto delete_;
  }
  CYC(0x6ef8, 0x6efa);
  // Check that Link has timewarped in from a specific spot
  CYC(0x6efa, 0x6efd); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(0x6efd, 0x6efe); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6efe, 0x6f00); goto delete_;
  }
  CYC(0x6efe, 0x6f00);
  CYC(0x6f00, 0x6f03); A = mem_rd(gb, wWarpDestPos);
  CYC(0x6f03, 0x6f05); alu_cp(gb, 0x24);
  if (!(F & FZ)) {
    CYCT(0x6f05, 0x6f07); goto delete_;
  }
  CYC(0x6f05, 0x6f07);
  CYC(0x6f07, 0x6f0a); SET_HL(ralphSubid0bScript_bank0c);

setScriptAndDisableObjects:
  CALL_C(0x6f0a, interactionSetScript_hook, 0x2544, 0x6f0d);
  CYC(0x6f0d, 0x6f0f); A = 0x81;
  CYC(0x6f0f, 0x6f12); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6f12, 0x6f15); mem_wr(gb, wMenuDisabled, A);
  CALL_C(0x6f15, objectSetVisiblec1_hook, 0x1e3c, 0x6f18);
  CYC(0x6f18, 0x6f1b); ralphRunSubid_hook(gb);
  return;

initSubid10:
  CALL_C(0x6f1b, getThisRoomFlags_hook, 0x197d, 0x6f1e);
  CYC(0x6f1e, 0x6f20); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x6f20, 0x6f23); interactionDelete_hook(gb); return;
  }
  CYC(0x6f20, 0x6f23);
  CYC(0x6f23, 0x6f25); A = 0x43; // GLOBALFLAG_TALKED_TO_CHEVAL
  CALL_C(0x6f25, checkGlobalFlag_hook, 0x31f3, 0x6f28);
  if (F & FZ) {
    CYCT(0x6f28, 0x6f2b); interactionDelete_hook(gb); return;
  }
  CYC(0x6f28, 0x6f2b);
  CYC(0x6f2b, 0x6f2e); A = mem_rd(gb, wWarpDestPos);
  CYC(0x6f2e, 0x6f30); alu_cp(gb, 0x17);
  if (!(F & FZ)) {
    CYCT(0x6f30, 0x6f33); interactionDelete_hook(gb); return;
  }
  CYC(0x6f30, 0x6f33);
  CYC(0x6f33, 0x6f36); SET_HL(ralphSubid10Script_bank0c);
  CYC(0x6f36, 0x6f38); goto setScriptAndDisableObjects;

initSubid11:
  CYC(0x6f38, 0x6f3a); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x6f3a, checkGlobalFlag_hook, 0x31f3, 0x6f3d);
  if (F & FZ) {
    CYCT(0x6f3d, 0x6f40); interactionDelete_hook(gb); return;
  }
  CYC(0x6f3d, 0x6f40);
  CYC(0x6f40, 0x6f42); A = 0x03;
  CALL_C(0x6f42, interactionSetAnimation_hook, 0x262e, 0x6f45);
  CYC(0x6f45, 0x6f48); SET_HL(ralphSubid11Script_bank0c);
  CALL_C(0x6f48, interactionSetScript_hook, 0x2544, 0x6f4b);
  CYC(0x6f4b, 0x6f4d); ralphRunSubid_hook(gb);
  return;

initSubid0c:
  CYC(0x6f4d, 0x6f50); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x6f50, 0x6f52); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x6f52, 0x6f55); interactionDelete_hook(gb); return;
  }
  CYC(0x6f52, 0x6f55);
  CALL_C(0x6f55, interactionLoadExtraGraphics_hook, 0x2781, 0x6f58);
  // callab scriptHelp.ralph_createLinkedSwordAnimation
  CYC(0x6f58, 0x6f5b); SET_HL(ralph_createLinkedSwordAnimation_bank15);
  CYC(0x6f5b, 0x6f5d); E = 0x15;
  CALL_C(0x6f5d, interBankCall_hook, 0x008a, 0x6f60);
  CYC(0x6f60, 0x6f63); SET_HL(ralphSubid0cScript_bank0c);
  CALL_C(0x6f63, interactionSetScript_hook, 0x2544, 0x6f66);
  CYC(0x6f66, 0x6f67); alu_xor(gb, A);
  CYC(0x6f67, 0x6f6a); mem_wr(gb, wTmpcfc0 + 0x1e, A);
  CYC(0x6f6a, 0x6f6d); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  CALL_C(0x6f6d, interactionSetAnimation_hook, 0x262e, 0x6f70);
  CALL_C(0x6f70, interactionRunScript_hook, 0x2552, 0x6f73);
  CYC(0x6f73, 0x6f75); ralphRunSubid_hook(gb);
  return;

initSubid12:
  CALL_C(0x6f75, checkIsLinkedGame_hook, 0x1992, 0x6f78);
  if (F & FZ) {
    CYCT(0x6f78, 0x6f7b); interactionDelete_hook(gb); return;
  }
  CYC(0x6f78, 0x6f7b);
  CYC(0x6f7b, 0x6f7e); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x6f7e, 0x6f80); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6f80, 0x6f83); interactionDelete_hook(gb); return;
  }
  CYC(0x6f80, 0x6f83);
  CALL_C(0x6f83, objectSetVisiblec2_hook, 0x1e45, 0x6f86);
  CYC(0x6f86, 0x6f89); SET_HL(ralphSubid12Script_bank0c);
  CYC(0x6f89, 0x6f8c); interactionSetScript_hook(gb);
  return;

initSubid0d:
  CYC(0x6f8c, 0x6f8f); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(0x6f8f, 0x6f91); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x6f91, 0x6f94); interactionDelete_hook(gb); return;
  }
  CYC(0x6f91, 0x6f94);
  CYC(0x6f94, 0x6f97); SET_HL(ralphSubid0dScript_bank0c);
  CALL_C(0x6f97, interactionSetScript_hook, 0x2544, 0x6f9a);
  CALL_C(0x6f9a, objectSetVisiblec0_hook, 0x1e33, 0x6f9d);
  // Falls through into ralphRunSubid
  ralphRunSubid_hook(gb);
}

// ralphSubid08@getNextAngle: B = 2 preset index; zflag set when there are no more presets.
// Fallen into from substate 0 and `call z`ed from substate 1.
static void ralph_getNextAngle(GB *gb, uint16_t sp0_) {
  CYC(0x7204, 0x7206); B = 0x02;
  // callab agesInteractionsBank0a.loadAngleAndCounterPreset
  CYC(0x7206, 0x7209); SET_HL(loadAngleAndCounterPreset_bank0a);
  CYC(0x7209, 0x720b); E = 0x0a;
  CALL_C(0x720b, interBankCall_hook, 0x008a, 0x720e);
  CYC(0x720e, 0x720f); A = B;
  CYC(0x720f, 0x7210); alu_or(gb, A);
  CYC(0x7210, 0x7211); ret_effect(gb);
}

// ralphSubid0a@moveHorizontallyTowardRalph: force Link to walk to x=$50. Fallen into from
// ralphSubid0a substate 1 and `jp`ed to from ralphSubid0a_linked substate 0.
static void ralph_moveHorizontallyTowardRalph(GB *gb) {
  CYC(0x728d, 0x7290); A = W8(w1Link_xh);
  CYC(0x7290, 0x7292); alu_sub(gb, 0x50);
  CYC(0x7292, 0x7293); B = A;
  CYC(0x7293, 0x7295); alu_add(gb, 0x02);
  CYC(0x7295, 0x7297); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(0x7297, 0x7299); goto incSubstate;
  }
  CYC(0x7297, 0x7299);
  CYC(0x7299, 0x729a); A = B;
  CYC(0x729a, 0x729c); alu_bit(gb, 7, A);
  CYC(0x729c, 0x729e); B = 0x18;
  if (F & FZ) {
    CYCT(0x729e, 0x72a0); goto setForceMovement;
  }
  CYC(0x729e, 0x72a0);
  CYC(0x72a0, 0x72a2); B = 0x08;
  CYC(0x72a2, 0x72a3); alu_cpl(gb);
  CYC(0x72a3, 0x72a4); A = alu_inc8(gb, A);
setForceMovement:
  CYC(0x72a4, 0x72a7); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x72a7, 0x72a9); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x72a9, 0x72ac); mem_wr(gb, wLinkForceState, A);
  CYC(0x72ac, 0x72ad); A = B;
  CYC(0x72ad, 0x72b0); SET_HL(w1Link_angle);
  CYC(0x72b0, 0x72b1); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x72b1, 0x72b3); A = alu_swap(gb, A);
  CYC(0x72b3, 0x72b4); alu_rlca(gb);
  CYC(0x72b4, 0x72b5); mem_wr(gb, HL, A); // [w1Link.direction]
incSubstate:
  CYC(0x72b5, 0x72b8); interactionIncSubstate_hook(gb);
}

// ralphSubid0a@moveVerticallyTowardRalph: B = distance. Fallen into from ralphSubid0a
// substate 2 and `jp`ed to from ralphSubid0a_linked substate 1.
static void ralph_moveVerticallyTowardRalph(GB *gb) {
  CYC(0x72ba, 0x72bd); A = W8(w1Link_state);
  CYC(0x72bd, 0x72bf); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(0x72bf, 0x72c0); ret_effect(gb); return;
  }
  CYC(0x72bf, 0x72c0);
  // Make Link move vertically toward Ralph
  CYC(0x72c0, 0x72c3); SET_HL(w1Link_angle);
  CYC(0x72c3, 0x72c5); mem_wr(gb, HL, 0x10);
  CYC(0x72c5, 0x72c6); L = alu_dec8(gb, L);
  CYC(0x72c6, 0x72c8); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(0x72c8, 0x72c9); A = B;
  CYC(0x72c9, 0x72cc); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x72cc, 0x72ce); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x72ce, 0x72d1); mem_wr(gb, wLinkForceState, A);
  CYC(0x72d1, 0x72d4); interactionIncSubstate_hook(gb);
}

// ralphSubid0a@setDirectionAndAnimationWhenLinkFinishedMoving: B = Link's direction,
// C = Ralph's animation. Fallen into from ralphSubid0a substate 3 and `jp`ed to from
// ralphSubid0a_linked substate 2.
static void ralph_setDirectionAndAnimationWhenLinkFinishedMoving(GB *gb, uint16_t sp0_) {
  CYC(0x72d7, 0x72da); A = W8(w1Link_state);
  CYC(0x72da, 0x72dc); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(0x72dc, 0x72dd); ret_effect(gb); return;
  }
  CYC(0x72dc, 0x72dd);
  CALL_C(0x72dd, setLinkForceStateToState08_hook, 0x2aad, 0x72e0);
  CYC(0x72e0, 0x72e1); A = B;
  CYC(0x72e1, 0x72e4); W8(w1Link_direction) = A;
  CYC(0x72e4, 0x72e5); A = C;
  CALL_C(0x72e5, interactionSetAnimation_hook, 0x262e, 0x72e8);
  CYC(0x72e8, 0x72eb); interactionIncSubstate_hook(gb);
}

// INTERAC_RALPH
// Variables:
//   var3f: for some subids, ralph's animations only updates when this is 0.
void interactionCode37_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d81, 0x6d83); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6d83, 0x6d84); A = mem_rd(gb, DE);
  CYC(0x6d84, 0x6d85); push_effect(gb, 0x6d85);
  switch (ralph_jumpTable(gb)) {
    case 0x6d89: ralphState0_hook(gb); return;
    case 0x6f9d: ralphRunSubid_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void ralphState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d89, 0x6d8b); A = 0x01;
  CYC(0x6d8b, 0x6d8c); mem_wr(gb, DE, A);
  CALL_C(0x6d8c, interactionInitGraphics_hook, 0x15fb, 0x6d8f);
  CYC(0x6d8f, 0x6d92); push_effect(gb, 0x6d92); ralph_initSubid(gb, sp0_);
  CYC(0x6d92, 0x6d94); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x6d94, 0x6d95); A = mem_rd(gb, DE);
  CYC(0x6d95, 0x6d96); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6d96, 0x6d99); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x6d96, 0x6d99);
  CYC(0x6d99, 0x6d9a); ret_effect(gb);
}

void ralphRunSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f9d, 0x6f9f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6f9f, 0x6fa0); A = mem_rd(gb, DE);
  CYC(0x6fa0, 0x6fa1); push_effect(gb, 0x6fa1);
  switch (ralph_jumpTable(gb)) {
    case 0x6fc7: ralphSubid00_hook(gb); return;
    case 0x7036: ralphSubid01_hook(gb); return;
    case 0x7004: ralphSubid02_hook(gb); return;
    case 0x7049: ralphSubid03_hook(gb); return;
    case 0x7118: ralphSubid04_hook(gb); return;
    case 0x713d: ralphSubid05_hook(gb); return;
    case 0x7186: ralphSubid06_hook(gb); return;
    case 0x71b7: ralphSubid07_hook(gb); return;
    case 0x71ec: ralphSubid08_hook(gb); return;
    case 0x7237: ralphSubid09_hook(gb); return;
    case 0x7247: ralphSubid0a_hook(gb); return;
    case 0x7324: ralphSubid0b_hook(gb); return; // subids $0b and $10
    case 0x737c: ralphRunScriptAndDeleteWhenOver_hook(gb); return;
    case 0x7353: ralphRunScriptWithConditionalAnimation_hook(gb); return;
    case 0x7361: ralphSubid0e_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x6d71: nayruRunScriptWithConditionalAnimation_hook(gb); return;
    case 0x7385: ralphSubid12_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// Cutscene where Nayru gets possessed
void ralphSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6fc7, 0x6fc9); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6fc9, 0x6fca); A = mem_rd(gb, DE);
  CYC(0x6fca, 0x6fcb); push_effect(gb, 0x6fcb);
  switch (ralph_jumpTable(gb)) {
    case 0x6fcf: goto substate0;
    case 0x6ff0: goto substate1;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x6fcf, interactionAnimate_hook, 0x261b, 0x6fd2);
  CYC(0x6fd2, 0x6fd5); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6fd5, 0x6fd7); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(0x6fd7, 0x6fd8); ret_effect(gb); return;
  }
  CYC(0x6fd7, 0x6fd8);
  CALL_C(0x6fd8, interactionIncSubstate_hook, 0x23e5, 0x6fdb);
  CYC(0x6fdb, 0x6fdd); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6fdd, 0x6fdf); mem_wr(gb, HL, 0x3c);
  CYC(0x6fdf, 0x6fe2); SET_BC(0x3088);
  CALL_C(0x6fe2, interactionSetPosition_hook, 0x2773, 0x6fe5);
  CYC(0x6fe5, 0x6fe7); A = 0x03;
  CALL_C(0x6fe7, interactionSetAnimation_hook, 0x262e, 0x6fea);
  CYC(0x6fea, 0x6fed); SET_HL(ralphSubid00Script_bank0c);
  CYC(0x6fed, 0x6ff0); interactionSetScript_hook(gb);
  return;

substate1:
  CALL_C(0x6ff0, interactionAnimate_hook, 0x261b, 0x6ff3);
  CALL_C(0x6ff3, interactionRunScript_hook, 0x2552, 0x6ff6);
  CYC(0x6ff6, 0x6ff8); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6ff8, 0x6ff9); A = mem_rd(gb, DE);
  CYC(0x6ff9, 0x6ffa); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6ffa, 0x6ffb); ret_effect(gb); return;
  }
  CYC(0x6ffa, 0x6ffb);
  // Animate more quickly if moving fast
  CYC(0x6ffb, 0x6ffd); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6ffd, 0x6ffe); A = mem_rd(gb, DE);
  CYC(0x6ffe, 0x7000); alu_cp(gb, 0x28); // SPEED_100
  if (!(F & FC)) {
    CYCT(0x7000, 0x7003); interactionAnimate_hook(gb); return;
  }
  CYC(0x7000, 0x7003);
  CYC(0x7003, 0x7004); ret_effect(gb);
}

// Cutscene after Nayru is possessed
void ralphSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // They probably meant to call "checkInteractionSubstate" instead? It looks like @state0
  // will never be run...
  CALL_C(0x7004, checkInteractionState_hook, 0x23fe, 0x7007);
  if (!(F & FZ)) {
    CYCT(0x7007, 0x7009); goto state1;
  }
  CYC(0x7007, 0x7009);

  // @state0
  CALL_C(0x7009, interactionRunScript_hook, 0x2552, 0x700c);
  CALL_C(0x700c, interactionAnimate_hook, 0x261b, 0x700f);
  CYC(0x700f, 0x7012); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7012, 0x7014); alu_cp(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(0x7014, 0x7015); ret_effect(gb); return;
  }
  CYC(0x7014, 0x7015);
  CYC(0x7015, 0x7018); interactionIncSubstate_hook(gb);
  return;

state1:
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(0x7018, 0x701b); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(0x701b, 0x701d); E = 0x15;
  CALL_C(0x701d, interBankCall_hook, 0x008a, 0x7020);
  CYC(0x7020, 0x7022); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7022, 0x7023); A = mem_rd(gb, DE);
  CYC(0x7023, 0x7024); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(0x7024, interactionAnimate_hook, 0x261b, 0x7027);
  } else {
    CYC(0x7024, 0x7027);
  }
  CALL_C(0x7027, interactionAnimate_hook, 0x261b, 0x702a);
  CALL_C(0x702a, interactionRunScript_hook, 0x2552, 0x702d);
  if (!(F & FC)) {
    CYCT(0x702d, 0x702e); ret_effect(gb); return;
  }
  CYC(0x702d, 0x702e);
  // Script done
  CYC(0x702e, 0x7030); A = 0xfb; // SNDCTRL_MEDIUM_FADEOUT
  CALL_C(0x7030, playSound_b00_hook, 0x0c98, 0x7033);
  CYC(0x7033, 0x7036); interactionDelete_hook(gb);
}

// Cutscene outside Ambi's palace before getting mystery seeds
void ralphSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7036, interactionRunScript_hook, 0x2552, 0x7039);
  if (F & FC) {
    CYCT(0x7039, 0x703c); interactionDelete_hook(gb); return;
  }
  CYC(0x7039, 0x703c);
  CALL_C(0x703c, ralphTurnLinkTowardSelf_hook, 0x73bb, 0x703f);
  CYC(0x703f, 0x7041); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7041, 0x7042); A = mem_rd(gb, DE);
  CYC(0x7042, 0x7043); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x7043, interactionAnimate2Times_hook, 0x2752, 0x7046);
  } else {
    CYC(0x7043, 0x7046);
  }
  CYC(0x7046, 0x7049); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

// Cutscene after talking to Rafton
void ralphSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7049, 0x704b); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x704b, 0x704c); A = mem_rd(gb, DE);
  CYC(0x704c, 0x704d); push_effect(gb, 0x704d);
  switch (ralph_jumpTable(gb)) {
    case 0x705f: goto substate0;
    case 0x707d: goto substate1;
    case 0x7087: goto substate2;
    case 0x7098: goto substate3;
    case 0x70a7: goto substate4;
    case 0x70bd: goto substate5;
    case 0x70cb: goto substate6;
    case 0x70f4: goto substate7;
    case 0x7106: goto substate8;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x705f, interactionDecCounter1_hook, 0x23cc, 0x7062);
  if (!(F & FZ)) {
    CYCT(0x7062, 0x7064); goto substate0_waiting;
  }
  CYC(0x7062, 0x7064);
  CYC(0x7064, 0x7066); mem_wr(gb, HL, 0x1e);
  CYC(0x7066, 0x7068); A = 0x02;
  CALL_C(0x7068, interactionSetAnimation_hook, 0x262e, 0x706b);
  CYC(0x706b, 0x706e); interactionIncSubstate_hook(gb);
  return;
substate0_waiting:
  CYC(0x706e, 0x7071); A = mem_rd(gb, wFrameCounter);
  CYC(0x7071, 0x7073); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x7073, 0x7074); ret_effect(gb); return;
  }
  CYC(0x7073, 0x7074);
  CYC(0x7074, 0x7076); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7076, 0x7077); A = mem_rd(gb, DE);
  CYC(0x7077, 0x7079); alu_xor(gb, 0x02);
  CYC(0x7079, 0x707a); mem_wr(gb, DE, A);
  CYC(0x707a, 0x707d); interactionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x707d, interactionDecCounter1_hook, 0x23cc, 0x7080);
  if (!(F & FZ)) {
    CYCT(0x7080, 0x7081); ret_effect(gb); return;
  }
  CYC(0x7080, 0x7081);
  CALL_C(0x7081, interactionIncSubstate_hook, 0x23e5, 0x7084);
  CYC(0x7084, 0x7087); startJump_hook(gb);
  return;

substate2:
  CALL_C(0x7087, interactionAnimate_hook, 0x261b, 0x708a);
  CYC(0x708a, 0x708c); C = 0x20;
  CALL_C(0x708c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x708f);
  if (!(F & FZ)) {
    CYCT(0x708f, 0x7090); ret_effect(gb); return;
  }
  CYC(0x708f, 0x7090);
  CALL_C(0x7090, interactionIncSubstate_hook, 0x23e5, 0x7093);
  CYC(0x7093, 0x7095); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7095, 0x7097); mem_wr(gb, HL, 0x0a);
  CYC(0x7097, 0x7098); ret_effect(gb);
  return;

substate3:
  CALL_C(0x7098, interactionDecCounter1_hook, 0x23cc, 0x709b);
  if (!(F & FZ)) {
    CYCT(0x709b, 0x709c); ret_effect(gb); return;
  }
  CYC(0x709b, 0x709c);
  CYC(0x709c, 0x709e); mem_wr(gb, HL, 0x1e);
  CALL_C(0x709e, interactionIncSubstate_hook, 0x23e5, 0x70a1);
  CYC(0x70a1, 0x70a4); SET_BC(0x2a0a); // TX_2a0a
  CYC(0x70a4, 0x70a7); showText_hook(gb);
  return;

substate4:
  CALL_C(0x70a7, interactionDecCounter1IfTextNotActive_hook, 0x273c, 0x70aa);
  if (!(F & FZ)) {
    CYCT(0x70aa, 0x70ab); ret_effect(gb); return;
  }
  CYC(0x70aa, 0x70ab);
  CYC(0x70ab, 0x70ad); mem_wr(gb, HL, 0x30);
  CALL_C(0x70ad, interactionIncSubstate_hook, 0x23e5, 0x70b0);
  CYC(0x70b0, 0x70b2); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x70b2, 0x70b4); mem_wr(gb, HL, 0x10);
  CYC(0x70b4, 0x70b6); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x70b6, 0x70b8); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x70b8, 0x70ba); A = 0x02;
  CYC(0x70ba, 0x70bd); interactionSetAnimation_hook(gb);
  return;

substate5:
  CALL_C(0x70bd, interactionAnimate2Times_hook, 0x2752, 0x70c0);
  CALL_C(0x70c0, interactionDecCounter1_hook, 0x23cc, 0x70c3);
  if (!(F & FZ)) {
    CYCT(0x70c3, 0x70c6); objectApplySpeed_hook(gb); return;
  }
  CYC(0x70c3, 0x70c6);
  CYC(0x70c6, 0x70c8); mem_wr(gb, HL, 0x06);
  CYC(0x70c8, 0x70cb); interactionIncSubstate_hook(gb);
  return;

substate6:
  CALL_C(0x70cb, interactionDecCounter1_hook, 0x23cc, 0x70ce);
  if (!(F & FZ)) {
    CYCT(0x70ce, 0x70cf); ret_effect(gb); return;
  }
  CYC(0x70ce, 0x70cf);
  CYC(0x70cf, 0x70d1); mem_wr(gb, HL, 0x0a);
  // Align with Link's x-position
  CALL_C(0x70d1, interactionIncSubstate_hook, 0x23e5, 0x70d4);
  CYC(0x70d4, 0x70d7); A = W8(w1Link_xh);
  CYC(0x70d7, 0x70d9); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x70d9, 0x70da); alu_sub(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x70da, 0x70dc); goto startScript;
  }
  CYC(0x70da, 0x70dc);
  if (F & FC) {
    CYCT(0x70dc, 0x70de); goto substate6_moveLeft;
  }
  CYC(0x70dc, 0x70de);
  // @@moveRight
  CYC(0x70de, 0x70e0); B = 0x08;
  CYC(0x70e0, 0x70e2); C = 0x01; // DIR_RIGHT
  CYC(0x70e2, 0x70e4); goto substate6_setMovement;
substate6_moveLeft:
  CYC(0x70e4, 0x70e5); alu_cpl(gb);
  CYC(0x70e5, 0x70e6); A = alu_inc8(gb, A);
  CYC(0x70e6, 0x70e8); B = 0x18;
  CYC(0x70e8, 0x70ea); C = 0x03; // DIR_LEFT
substate6_setMovement:
  CYC(0x70ea, 0x70ec); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x70ec, 0x70ed); mem_wr(gb, HL, A);
  CYC(0x70ed, 0x70ef); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x70ef, 0x70f0); mem_wr(gb, HL, B);
  CYC(0x70f0, 0x70f1); A = C;
  CYC(0x70f1, 0x70f4); interactionSetAnimation_hook(gb);
  return;

substate7:
  CALL_C(0x70f4, interactionAnimate2Times_hook, 0x2752, 0x70f7);
  CALL_C(0x70f7, interactionDecCounter1_hook, 0x23cc, 0x70fa);
  if (!(F & FZ)) {
    CYCT(0x70fa, 0x70fd); objectApplySpeed_hook(gb); return;
  }
  CYC(0x70fa, 0x70fd);

startScript:
  CALL_C(0x70fd, interactionIncSubstate_hook, 0x23e5, 0x7100);
  CYC(0x7100, 0x7103); SET_HL(ralphSubid03Script_bank0c);
  CYC(0x7103, 0x7106); interactionSetScript_hook(gb);
  return;

substate8:
  CALL_C(0x7106, ralphAnimateBasedOnSpeedAndRunScript_hook, 0x71cb, 0x7109);
  if (!(F & FC)) {
    CYCT(0x7109, 0x710a); ret_effect(gb); return;
  }
  CYC(0x7109, 0x710a);
  CYC(0x710a, 0x710c); A = 0x04; // MUS_OVERWORLD_PAST
  CYC(0x710c, 0x710f); mem_wr(gb, wActiveMusic2, A);
  CYC(0x710f, 0x7112); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x7112, playSound_b00_hook, 0x0c98, 0x7115);
  CYC(0x7115, 0x7118); interactionDelete_hook(gb);
}

// Cutscene on maku tree screen after saving Nayru
void ralphSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7118, 0x711a); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x711a, 0x711b); A = mem_rd(gb, DE);
  CYC(0x711b, 0x711c); push_effect(gb, 0x711c);
  switch (ralph_jumpTable(gb)) {
    // Borrow some of Nayru's code from the same cutscene
    case 0x6bd8: nayruSubid02Substate0_hook(gb); return;
    case 0x7122: goto substate1;
    case 0x7136: goto substate2;
    default: HANDOFF(HL);
  }

substate1:
  CYC(0x7122, 0x7125); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7125, 0x7127); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x7127, 0x712a); nayruFlipDirectionAtRandomIntervals_hook(gb); return;
  }
  CYC(0x7127, 0x712a);
  CALL_C(0x712a, interactionIncSubstate_hook, 0x23e5, 0x712d);
  CYC(0x712d, 0x7130); SET_HL(ralphSubid04Script_part3_bank0c);
  CALL_C(0x7130, interactionSetScript_hook, 0x2544, 0x7133);
  CYC(0x7133, 0x7136);

substate2:
  CALL_C(0x7136, ralphAnimateBasedOnSpeedAndRunScript_hook, 0x71cb, 0x7139);
  if (!(F & FC)) {
    CYCT(0x7139, 0x713a); ret_effect(gb); return;
  }
  CYC(0x7139, 0x713a);
  CYC(0x713a, 0x713d); interactionDelete_hook(gb);
}

// Cutscene in black tower where Nayru/Ralph meet you to try to escape. Substate 3 falls
// through into ralphRunScript.
void ralphSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x713d, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7140);
  CYC(0x7140, 0x7142); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7142, 0x7143); A = mem_rd(gb, DE);
  CYC(0x7143, 0x7144); push_effect(gb, 0x7144);
  switch (ralph_jumpTable(gb)) {
    case 0x714e: goto substate0;
    case 0x715a: goto substate1;
    case 0x7169: goto substate2;
    case 0x7177: goto substate3;
    case 0x7183: ralphRunScript_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x714e, 0x7151); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7151, 0x7153); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x7153, 0x7154); ret_effect(gb); return;
  }
  CYC(0x7153, 0x7154);
  CALL_C(0x7154, startJump_hook, 0x73db, 0x7157);
  CYC(0x7157, 0x715a); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x715a, 0x715c); C = 0x20;
  CALL_C(0x715c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x715f);
  if (!(F & FZ)) {
    CYCT(0x715f, 0x7160); ret_effect(gb); return;
  }
  CYC(0x715f, 0x7160);
  CYC(0x7160, 0x7163); SET_HL(ralphSubid05Script_bank0c);
  CALL_C(0x7163, interactionSetScript_hook, 0x2544, 0x7166);
  CYC(0x7166, 0x7169); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x7169, 0x716c); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x716c, 0x716e); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x716e, 0x7171); interactionRunScript_hook(gb); return;
  }
  CYC(0x716e, 0x7171);
  CALL_C(0x7171, startJump_hook, 0x73db, 0x7174);
  CYC(0x7174, 0x7177); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(0x7177, 0x7179); C = 0x20;
  CALL_C(0x7179, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x717c);
  if (!(F & FZ)) {
    CYCT(0x717c, 0x717d); ret_effect(gb); return;
  }
  CYC(0x717c, 0x717d);
  CALL_C(0x717d, interactionIncSubstate_hook, 0x23e5, 0x7180);
  CYC(0x7180, 0x7182); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x7182, 0x7183); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  ralphRunScript_hook(gb);
}

void ralphRunScript_hook(GB *gb) {
  CYC(0x7183, 0x7186); interactionRunScript_hook(gb);
}

void ralphSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7186, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7189);
  CYC(0x7189, 0x718b); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x718b, 0x718c); A = mem_rd(gb, DE);
  CYC(0x718c, 0x718d); push_effect(gb, 0x718d);
  switch (ralph_jumpTable(gb)) {
    case 0x7193: goto substate0;
    case 0x71a9: goto substate1;
    case 0x7183: ralphRunScript_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(0x7193, 0x7196); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(0x7196, 0x7198); E = 0x15;
  CALL_C(0x7198, interBankCall_hook, 0x008a, 0x719b);
  CYC(0x719b, 0x719e); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x719e, 0x71a0); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x71a0, 0x71a3); interactionRunScript_hook(gb); return;
  }
  CYC(0x71a0, 0x71a3);
  CALL_C(0x71a3, startJump_hook, 0x73db, 0x71a6);
  CYC(0x71a6, 0x71a9); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x71a9, 0x71ab); C = 0x20;
  CALL_C(0x71ab, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x71ae);
  if (!(F & FZ)) {
    CYCT(0x71ae, 0x71af); ret_effect(gb); return;
  }
  CYC(0x71ae, 0x71af);
  CALL_C(0x71af, interactionIncSubstate_hook, 0x23e5, 0x71b2);
  CYC(0x71b2, 0x71b4); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x71b4, 0x71b5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x71b5, 0x71b7); ralphRunScript_hook(gb);
}

// Cutscene postgame where they warp to the maku tree, Ralph notices the statue
void ralphSubid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(0x71b7, 0x71ba); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(0x71ba, 0x71bc); E = 0x15;
  CALL_C(0x71bc, interBankCall_hook, 0x008a, 0x71bf);
  CYC(0x71bf, 0x71c1); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x71c1, 0x71c2); A = mem_rd(gb, DE);
  CYC(0x71c2, 0x71c3); push_effect(gb, 0x71c3);
  switch (ralph_jumpTable(gb)) {
    case 0x71cb: ralphAnimateBasedOnSpeedAndRunScript_hook(gb); return; // substates 0 and 3
    case 0x71d1: ralphSubid07Substate1_hook(gb); return;
    case 0x71dd: ralphSubid07Substate2_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void ralphAnimateBasedOnSpeedAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71cb, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x71ce);
  CYC(0x71ce, 0x71d1); interactionRunScript_hook(gb);
}

// Falls through into ralphSubid07Substate2.
void ralphSubid07Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71d1, interactionIncSubstate_hook, 0x23e5, 0x71d4);
  CALL_C(0x71d4, objectSetVisiblec2_hook, 0x1e45, 0x71d7);
  CYC(0x71d7, 0x71da); SET_BC(0xfe40); // -$1c0
  CALL_C(0x71da, objectSetSpeedZ_hook, 0x239d, 0x71dd);
  ralphSubid07Substate2_hook(gb);
}

void ralphSubid07Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71dd, 0x71df); C = 0x20;
  CALL_C(0x71df, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x71e2);
  if (!(F & FZ)) {
    CYCT(0x71e2, 0x71e3); ret_effect(gb); return;
  }
  CYC(0x71e2, 0x71e3);
  CALL_C(0x71e3, interactionIncSubstate_hook, 0x23e5, 0x71e6);
  CYC(0x71e6, 0x71e8); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x71e8, 0x71e9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x71e9, 0x71ec); objectSetVisible82_hook(gb);
}

// Cutscene in credits where Ralph is training with his sword
void ralphSubid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71ec, 0x71ee); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x71ee, 0x71ef); A = mem_rd(gb, DE);
  CYC(0x71ef, 0x71f0); push_effect(gb, 0x71f0);
  switch (ralph_jumpTable(gb)) {
    case 0x71f6: goto substate0;
    case 0x7211: goto substate1;
    case 0x722a: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x71f6, interactionAnimate_hook, 0x261b, 0x71f9);
  CALL_C(0x71f9, interactionRunScript_hook, 0x2552, 0x71fc);
  if (!(F & FC)) {
    CYCT(0x71fc, 0x71fd); ret_effect(gb); return;
  }
  CYC(0x71fc, 0x71fd);
  // Script done
  CALL_C(0x71fd, interactionIncSubstate_hook, 0x23e5, 0x7200);
  CYC(0x7200, 0x7202); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7202, 0x7204); mem_wr(gb, HL, 0x1e); // SPEED_c0
  ralph_getNextAngle(gb, sp0_);
  return;

substate1:
  CALL_C(0x7211, interactionAnimate_hook, 0x261b, 0x7214);
  CALL_C(0x7214, objectApplySpeed_hook, 0x201d, 0x7217);
  CALL_C(0x7217, interactionDecCounter1_hook, 0x23cc, 0x721a);
  if (F & FZ) {
    CYCT(0x721a, 0x721d); push_effect(gb, 0x721d); ralph_getNextAngle(gb, sp0_);
  } else {
    CYC(0x721a, 0x721d);
  }
  if (!(F & FZ)) {
    CYCT(0x721d, 0x721e); ret_effect(gb); return;
  }
  CYC(0x721d, 0x721e);
  CALL_C(0x721e, interactionIncSubstate_hook, 0x23e5, 0x7221);
  CYC(0x7221, 0x7223); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7223, 0x7225); mem_wr(gb, HL, 0x5a);
  CYC(0x7225, 0x7227); A = 0x08;
  CYC(0x7227, 0x722a); interactionSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(0x722a, interactionAnimate_hook, 0x261b, 0x722d);
  CALL_C(0x722d, interactionDecCounter1_hook, 0x23cc, 0x7230);
  if (!(F & FZ)) {
    CYCT(0x7230, 0x7231); ret_effect(gb); return;
  }
  CYC(0x7230, 0x7231);
  CYC(0x7231, 0x7233); A = 0xff;
  CYC(0x7233, 0x7236); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  CYC(0x7236, 0x7237); ret_effect(gb);
}

// Cutscene where Ralph charges in to Ambi's palace
void ralphSubid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7237, interactionRunScript_hook, 0x2552, 0x723a);
  if (!(F & FC)) {
    CYCT(0x723a, 0x723d); interactionAnimateBasedOnSpeed_hook(gb); return;
  }
  CYC(0x723a, 0x723d);
  // Script done
  CYC(0x723d, 0x723e); alu_xor(gb, A);
  CYC(0x723e, 0x7241); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7241, 0x7244); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7244, 0x7247); interactionDelete_hook(gb);
}

// Cutscene where Ralph's about to charge into the black tower
void ralphSubid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7247, checkIsLinkedGame_hook, 0x1992, 0x724a);
  if (!(F & FZ)) {
    CYCT(0x724a, 0x724d); ralphSubid0a_linked_hook(gb); return;
  }
  CYC(0x724a, 0x724d);

  // Unlinked game
  CYC(0x724d, 0x724f); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x724f, 0x7250); A = mem_rd(gb, DE);
  CYC(0x7250, 0x7251); push_effect(gb, 0x7251);
  switch (ralph_jumpTable(gb)) {
    case 0x725b: goto substate0;
    case 0x7285: goto substate1;
    case 0x72b8: goto substate2;
    case 0x72d4: goto substate3;
    case 0x72eb: goto substate4;
    default: HANDOFF(HL);
  }

substate0:
  // Create an exclamation mark above Link
  CALL_C(0x725b, getFreeInteractionSlot_hook, 0x3aef, 0x725e);
  if (!(F & FZ)) {
    CYCT(0x725e, 0x725f); ret_effect(gb); return;
  }
  CYC(0x725e, 0x725f);
  CYC(0x725f, 0x7261); mem_wr(gb, HL, 0x9f); // INTERAC_EXCLAMATION_MARK
  CYC(0x7261, 0x7263); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7263, 0x7265); mem_wr(gb, HL, 0x1e);
  CYC(0x7265, 0x7267); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7267, 0x726a); A = W8(w1Link_yh);
  CYC(0x726a, 0x726c); alu_add(gb, 0x0e);
  CYC(0x726c, 0x726d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x726d, 0x726e); L = alu_inc8(gb, L);
  CYC(0x726e, 0x7271); A = W8(w1Link_xh);
  CYC(0x7271, 0x7273); alu_sub(gb, 0x0a);
  CYC(0x7273, 0x7274); mem_wr(gb, HL, A);
  CYC(0x7274, 0x7276); A = 0x50; // SND_CLINK
  CALL_C(0x7276, playSound_b00_hook, 0x0c98, 0x7279);
  CALL_C(0x7279, interactionIncSubstate_hook, 0x23e5, 0x727c);
  CYC(0x727c, 0x727e); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x727e, 0x7280); mem_wr(gb, HL, 0x1e);
  CYC(0x7280, 0x7282); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7282, 0x7284); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x7284, 0x7285); ret_effect(gb);
  return;

substate1:
  CALL_C(0x7285, interactionDecCounter1_hook, 0x23cc, 0x7288);
  if (!(F & FZ)) {
    CYCT(0x7288, 0x7289); ret_effect(gb); return;
  }
  CYC(0x7288, 0x7289);
  CYC(0x7289, 0x728a); alu_xor(gb, A);
  CALL_C(0x728a, interactionSetAnimation_hook, 0x262e, 0x728d);
  ralph_moveHorizontallyTowardRalph(gb);
  return;

substate2:
  CYC(0x72b8, 0x72ba); B = 0x50;
  ralph_moveVerticallyTowardRalph(gb);
  return;

substate3:
  CYC(0x72d4, 0x72d7); SET_BC(0x0103); // ldbc DIR_RIGHT,$03
  ralph_setDirectionAndAnimationWhenLinkFinishedMoving(gb, sp0_);
  return;

substate4:
  CALL_C(0x72eb, interactionRunScript_hook, 0x2552, 0x72ee);
  if (!(F & FC)) {
    CYCT(0x72ee, 0x72f1); interactionAnimateBasedOnSpeed_hook(gb); return;
  }
  CYC(0x72ee, 0x72f1);
  CYC(0x72f1, 0x72f2); alu_xor(gb, A);
  CYC(0x72f2, 0x72f5); mem_wr(gb, wDisabledObjects, A);
  CYC(0x72f5, 0x72f8); mem_wr(gb, wMenuDisabled, A);
  CYC(0x72f8, 0x72fb); interactionDelete_hook(gb);
}

void ralphSubid0a_linked_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x72fb, interactionRunScript_hook, 0x2552, 0x72fe);
  if (F & FC) {
    CYCT(0x72fe, 0x7301); interactionDelete_hook(gb); return;
  }
  CYC(0x72fe, 0x7301);
  CALL_C(0x7301, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7304);
  CYC(0x7304, 0x7306); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7306, 0x7307); A = mem_rd(gb, DE);
  CYC(0x7307, 0x7308); push_effect(gb, 0x7308);
  switch (ralph_jumpTable(gb)) {
    case 0x7310: goto substate0;
    case 0x7318: goto substate1;
    case 0x731d: goto substate2;
    case 0x7323: goto substate3;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x7310, 0x7311); H = D;
  CYC(0x7311, 0x7313); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7313, 0x7314); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7314, 0x7315); ret_effect(gb); return;
  }
  CYC(0x7314, 0x7315);
  CYC(0x7315, 0x7318); ralph_moveHorizontallyTowardRalph(gb);
  return;

substate1:
  CYC(0x7318, 0x731a); B = 0x18;
  CYC(0x731a, 0x731d); ralph_moveVerticallyTowardRalph(gb);
  return;

substate2:
  CYC(0x731d, 0x7320); SET_BC(0x0200); // ldbc DIR_DOWN,$00
  CYC(0x7320, 0x7323); ralph_setDirectionAndAnimationWhenLinkFinishedMoving(gb, sp0_);
  return;

substate3:
  CYC(0x7323, 0x7324); ret_effect(gb);
}

// $0b: Cutscene where Ralph tells you about getting Tune of Currents
// $10: Cutscene after talking to Cheval
void ralphSubid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7324, 0x7327); A = mem_rd(gb, wTmpcfc0);
  CYC(0x7327, 0x7328); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(0x7328, ralphTurnLinkTowardSelf_hook, 0x73bb, 0x732b);
  } else {
    CYC(0x7328, 0x732b);
  }
  CYC(0x732b, 0x732d); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x732d, 0x732e); A = mem_rd(gb, DE);
  CYC(0x732e, 0x732f); push_effect(gb, 0x732f);
  switch (ralph_jumpTable(gb)) {
    case 0x7335: goto substate0;
    case 0x733a: goto substate1;
    case 0x7353: ralphRunScriptWithConditionalAnimation_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x7335, interactionAnimate_hook, 0x261b, 0x7338);
  CYC(0x7338, 0x733a); ralphRunScriptWithConditionalAnimation_hook(gb);
  return;

substate1:
  // Create dust at Ralph's feet every 8 frames
  CYC(0x733a, 0x733d); A = mem_rd(gb, wFrameCounter);
  CYC(0x733d, 0x733f); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x733f, 0x7341); ralphRunScriptWithConditionalAnimation_hook(gb); return;
  }
  CYC(0x733f, 0x7341);
  CALL_C(0x7341, getFreeInteractionSlot_hook, 0x3aef, 0x7344);
  if (!(F & FZ)) {
    CYCT(0x7344, 0x7346); ralphRunScriptWithConditionalAnimation_hook(gb); return;
  }
  CYC(0x7344, 0x7346);
  CYC(0x7346, 0x7348); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x7348, 0x7349); L = alu_inc8(gb, L);
  CYC(0x7349, 0x734b); mem_wr(gb, HL, 0x81);
  CYC(0x734b, 0x734e); SET_BC(0x0804);
  CALL_C(0x734e, objectCopyPositionWithOffset_hook, 0x225a, 0x7351);
  CYC(0x7351, 0x7353); ralphRunScriptWithConditionalAnimation_hook(gb);
}

// Runs script, deletes self when finished, and updates animations only if var3f is 0.
void ralphRunScriptWithConditionalAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7353, interactionRunScript_hook, 0x2552, 0x7356);
  if (F & FC) {
    CYCT(0x7356, 0x7359); interactionDelete_hook(gb); return;
  }
  CYC(0x7356, 0x7359);
  CYC(0x7359, 0x735b); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x735b, 0x735c); A = mem_rd(gb, DE);
  CYC(0x735c, 0x735d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x735d, 0x7360); interactionAnimate_hook(gb); return;
  }
  CYC(0x735d, 0x7360);
  CYC(0x7360, 0x7361); ret_effect(gb);
}

// Cutscene with Nayru and Ralph when Link exits the black tower. Falls through into
// ralphRunScriptAndDeleteWhenOver.
void ralphSubid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7361, 0x7364); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(0x7364, 0x7366); alu_cp(gb, 0x5a);
  if (!(F & FC)) {
    CYCT(0x7366, 0x7368); ralphRunScriptAndDeleteWhenOver_hook(gb); return;
  }
  CYC(0x7366, 0x7368);
  CYC(0x7368, 0x7369); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7369, 0x736b); ralphRunScriptAndDeleteWhenOver_hook(gb); return;
  }
  CYC(0x7369, 0x736b);
  CYC(0x736b, 0x736e); A = W8(w1Link_direction);
  CYC(0x736e, 0x7370); alu_sub(gb, 0x02);
  CYC(0x7370, 0x7372); alu_and(gb, 0x03);
  CYC(0x7372, 0x7373); H = D;
  CYC(0x7373, 0x7375); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7375, 0x7376); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x7376, 0x7378); ralphRunScriptAndDeleteWhenOver_hook(gb); return;
  }
  CYC(0x7376, 0x7378);
  CYC(0x7378, 0x7379); mem_wr(gb, HL, A);
  CALL_C(0x7379, interactionSetAnimation_hook, 0x262e, 0x737c);
  ralphRunScriptAndDeleteWhenOver_hook(gb);
}

void ralphRunScriptAndDeleteWhenOver_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x737c, interactionRunScript_hook, 0x2552, 0x737f);
  if (F & FC) {
    CYCT(0x737f, 0x7382); interactionDelete_hook(gb); return;
  }
  CYC(0x737f, 0x7382);
  CYC(0x7382, 0x7385); interactionAnimateAsNpc_hook(gb);
}

// NPC after beating Veran, before beating Twinrova in a linked game
void ralphSubid12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7385, npcFaceLinkAndAnimate_hook, 0x26a9, 0x7388);
  CYC(0x7388, 0x738b); interactionRunScript_hook(gb);
}

// Unused?
void ralphFunc_738b_hook(GB *gb) {
  CYC(0x738b, 0x738c); H = D;
  CYC(0x738c, 0x738e); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x738e, 0x738f); A = mem_rd(gb, HL);
  CYC(0x738f, 0x7390); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7390, 0x7392); goto angleDecreasing;
  }
  CYC(0x7390, 0x7392);
  CYC(0x7392, 0x7395); SET_BC(0x2068);
  CYC(0x7395, 0x7398); A = mem_rd(gb, wFrameCounter);
  CYC(0x7398, 0x7399); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x7399, 0x739a); ret_effect(gb); return;
  }
  CYC(0x7399, 0x739a);
  CYC(0x739a, 0x739c); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x739c, 0x739d); A = mem_rd(gb, HL);
  CYC(0x739d, 0x739e); A = alu_inc8(gb, A);
  CYC(0x739e, 0x73a0); alu_and(gb, 0x1f);
  CYC(0x73a0, 0x73a1); mem_wr(gb, HL, A);
  CYC(0x73a1, 0x73a3); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x73a3, 0x73a4); ret_effect(gb); return;
  }
  CYC(0x73a3, 0x73a4);
  CYC(0x73a4, 0x73a6); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x73a6, 0x73a7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x73a7, 0x73a9); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x73a9, 0x73ab); mem_wr(gb, HL, 0x1f);

angleDecreasing:
  CYC(0x73ab, 0x73ae); SET_BC(0x6890);
  CYC(0x73ae, 0x73b1); A = mem_rd(gb, wFrameCounter);
  CYC(0x73b1, 0x73b2); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x73b2, 0x73b3); ret_effect(gb); return;
  }
  CYC(0x73b2, 0x73b3);
  CYC(0x73b3, 0x73b5); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x73b5, 0x73b6); A = mem_rd(gb, HL);
  CYC(0x73b6, 0x73b7); A = alu_dec8(gb, A);
  CYC(0x73b7, 0x73b9); alu_and(gb, 0x1f);
  CYC(0x73b9, 0x73ba); mem_wr(gb, HL, A);
  CYC(0x73ba, 0x73bb); ret_effect(gb);
}

void ralphTurnLinkTowardSelf_hook(GB *gb) {
  CYC(0x73bb, 0x73be); A = W8(w1Link_xh);
  CYC(0x73be, 0x73c0); alu_add(gb, 0x10);
  CYC(0x73c0, 0x73c1); B = A;
  CYC(0x73c1, 0x73c3); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x73c3, 0x73c4); A = mem_rd(gb, DE);
  CYC(0x73c4, 0x73c6); alu_add(gb, 0x10);
  CYC(0x73c6, 0x73c7); alu_sub(gb, B);
  CYC(0x73c7, 0x73c9); B = 0x01; // DIR_RIGHT
  if (!(F & FC)) {
    CYCT(0x73c9, 0x73cb); goto checkDistance;
  }
  CYC(0x73c9, 0x73cb);
  CYC(0x73cb, 0x73cd); B = 0x03; // DIR_LEFT
  CYC(0x73cd, 0x73ce); alu_cpl(gb);
checkDistance:
  CYC(0x73ce, 0x73d0); alu_cp(gb, 0x0c);
  if (!(F & FC)) {
    CYCT(0x73d0, 0x73d2); goto setDirection;
  }
  CYC(0x73d0, 0x73d2);
  CYC(0x73d2, 0x73d4); B = 0x02; // DIR_DOWN
setDirection:
  CYC(0x73d4, 0x73d7); SET_HL(w1Link_direction);
  CYC(0x73d7, 0x73d8); mem_wr(gb, HL, B);
  CYC(0x73d8, 0x73db); setLinkForceStateToState08_hook(gb);
}

// Also called from boy (subid 4), interactionCode3d and nayruSubid03.
void startJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73db, 0x73de); SET_BC(0xfe40); // -$1c0
  CALL_C(0x73de, objectSetSpeedZ_hook, 0x239d, 0x73e1);
  CYC(0x73e1, 0x73e3); A = 0x53; // SND_JUMP
  CYC(0x73e3, 0x73e6); playSound_b00_hook(gb);
}
