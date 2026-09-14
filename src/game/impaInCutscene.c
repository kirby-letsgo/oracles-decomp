#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// impaScriptTable: 10 script pointers into bank $0c, indexed by subid.
#define impaScriptTable_bank08 0x5ff9

// mainScripts.* (bank $0c), scriptHelp.* (bank $15) and objectData.* (bank $12), referenced
// by address only.
#define impaScript_moveAwayFromRock_bank0c 0x51cd
#define impaScript_waitForRockToBeMoved_bank0c 0x51f1
#define impaScript_rockJustMoved_bank0c 0x51f4
#define impaScript_revealPossession_bank0c 0x51f8
#define turnToFaceSomething_bank15 0x5613
#define objectWritePositionTocfd5_bank15 0x741b
#define impaOctoroks_bank12 0x77e6

void impaState1_hook(GB *gb);
void impaSubid0_hook(GB *gb);
void impaLoadCollapsedGraphic_hook(GB *gb);
void impaRet_hook(GB *gb);
void impaSubid1_hook(GB *gb);
void interactionOscillateXRandomly_hook(GB *gb);
void impaSubid1Substate2_hook(GB *gb);
void impaSubid2_hook(GB *gb);
void impaAnimateAndRunScript_hook(GB *gb);
void impaSubid2Substate4_hook(GB *gb);
void impaSetVisibleAndJump_hook(GB *gb);
void impaSubid2Substate5_hook(GB *gb);
void impaSubid2Substate6_hook(GB *gb);
void impaSubid2Substate7_hook(GB *gb);
void impaSubid4_hook(GB *gb);
void impaSubid5_hook(GB *gb);
void impaSubid7_hook(GB *gb);
void impaSubid8_hook(GB *gb);
void impaSubid9_hook(GB *gb);
void checkObjectIsCloseToPosition_b08_hook(GB *gb);
void impaUpdateAnimationIfDirectionChanged_hook(GB *gb);
void impaCheckApproachedStone_hook(GB *gb);
void impaAnimateAndDecCounter1_hook(GB *gb);
void impaPreventLinkFromLeavingStoneScreen_hook(GB *gb);

static uint16_t impaInCutscene_jumpTable(GB *gb) {
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

static void impaInCutscene_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode31@loadScript: load the script for this subid from impaScriptTable. Reached
// by jumps from the other init cases and by static `call`s from @init8 and impaSubid2.
static void impaInCutscene_loadScript(GB *gb) {
  CYC(0x5a8c, 0x5a8e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5a8e, 0x5a8f); A = mem_rd(gb, DE);
  CYC(0x5a8f, 0x5a92); SET_HL(impaScriptTable_bank08);
  CYC(0x5a92, 0x5a93); impaInCutscene_addDoubleIndex(gb, 0x5a93);
  CYC(0x5a93, 0x5a94); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5a94, 0x5a95); H = mem_rd(gb, HL);
  CYC(0x5a95, 0x5a96); L = A;
  CYC(0x5a96, 0x5a99); interactionSetScript_hook(gb);
}

// interactionCode31@initSubid: per-subid initialization. Reached only by a static `call`
// from interactionCode31 state 0.
static void impaInCutscene_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x5a45, 0x5a47); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5a47, 0x5a48); A = mem_rd(gb, DE);
  CYC(0x5a48, 0x5a49); push_effect(gb, 0x5a49);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5a5f: goto init0;
    case 0x5a82: goto init1;
    case 0x5a99: goto init2;
    case 0x5afd: goto init3;
    case 0x5b07: goto init4;
    case 0x5b24: goto init5;
    case 0x5a8c: impaInCutscene_loadScript(gb); return;
    case 0x5aa1: goto init7;
    case 0x5b3c: goto init8;
    case 0x5b36: goto init9;
    case 0x5b31: goto initA;
    default: HANDOFF(HL);
  }

init0:
  CALL_C(0x5a5f, getThisRoomFlags_hook, 0x197d, 0x5a62);
  CYC(0x5a62, 0x5a64); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x5a64, 0x5a67); interactionDelete_hook(gb); return;
  }
  CYC(0x5a64, 0x5a67);
  // Load a custom palette and use it for possessed impa
  CYC(0x5a67, 0x5a69); A = 0x97; // PALH_97
  CALL_C(0x5a69, loadPaletteHeader_hook, 0x050b, 0x5a6c);
  CYC(0x5a6c, 0x5a6e); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x5a6e, 0x5a70); A = 0x07;
  CYC(0x5a70, 0x5a71); mem_wr(gb, DE, A);
  CYC(0x5a71, 0x5a74); SET_HL(impaOctoroks_bank12);
  CALL_C(0x5a74, parseGivenObjectData_b00_hook, 0x3171, 0x5a77);
  CYC(0x5a77, 0x5a79); A = 0x08; // LINK_STATE_08
  CALL_C(0x5a79, setLinkIDOverride_hook, 0x2acf, 0x5a7c);
  CYC(0x5a7c, 0x5a7e); L = (uint8_t)w1Link_subid;
  CYC(0x5a7e, 0x5a80); mem_wr(gb, HL, 0x01);
  CYC(0x5a80, 0x5a82); impaInCutscene_loadScript(gb);
  return;

init1:
  CYC(0x5a82, 0x5a83); H = D;
  CYC(0x5a83, 0x5a85); L = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(0x5a85, 0x5a86); A = mem_rd(gb, HL);
  CYC(0x5a86, 0x5a88); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x5a88, 0x5a89); mem_wr(gb, HL, A);
  CALL_C(0x5a89, impaLoadCollapsedGraphic_hook, 0x5d56, 0x5a8c);
  impaInCutscene_loadScript(gb);
  return;

init2:
  CYC(0x5a99, 0x5a9a); H = D;
  CYC(0x5a9a, 0x5a9c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5a9c, 0x5a9e); mem_wr(gb, HL, 0x1e);
  CYC(0x5a9e, 0x5aa1); objectSetVisible82_hook(gb);
  return;

init7:
  // Delete self if Zelda hasn't been kidnapped by vire yet, or she's been rescued already,
  // or this isn't a linked game
  CYC(0x5aa1, 0x5aa4); A = mem_rd(gb, wEssencesObtained);
  CYC(0x5aa4, 0x5aa6); alu_bit(gb, 2, A);
  if (F & FZ) {
    CYCT(0x5aa6, 0x5aa9); interactionDelete_hook(gb); return;
  }
  CYC(0x5aa6, 0x5aa9);
  CALL_C(0x5aa9, checkIsLinkedGame_hook, 0x1992, 0x5aac);
  if (F & FZ) {
    CYCT(0x5aac, 0x5aaf); interactionDelete_hook(gb); return;
  }
  CYC(0x5aac, 0x5aaf);
  CYC(0x5aaf, 0x5ab1); A = 0x38; // GLOBALFLAG_GOT_RING_FROM_ZELDA
  CALL_C(0x5ab1, checkGlobalFlag_hook, 0x31f3, 0x5ab4);
  if (!(F & FZ)) {
    CYCT(0x5ab4, 0x5ab7); interactionDelete_hook(gb); return;
  }
  CYC(0x5ab4, 0x5ab7);
  CYC(0x5ab7, 0x5ab9); A = 0x39; // GLOBALFLAG_IMPA_MOVED_AFTER_ZELDA_KIDNAPPED
  CALL_C(0x5ab9, checkGlobalFlag_hook, 0x31f3, 0x5abc);
  CYC(0x5abc, 0x5abe); A = 0x09;
  if (F & FZ) {
    CYCT(0x5abe, 0x5ac0); goto setAnimationAndLoadScript;
  }
  CYC(0x5abe, 0x5ac0);
  CYC(0x5ac0, 0x5ac2); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5ac2, 0x5ac4); A = 0x38;
  CYC(0x5ac4, 0x5ac5); mem_wr(gb, DE, A);
  CYC(0x5ac5, 0x5ac7); A = 0x3c; // GLOBALFLAG_ZELDA_SAVED_FROM_VIRE
  CALL_C(0x5ac7, checkGlobalFlag_hook, 0x31f3, 0x5aca);
  CYC(0x5aca, 0x5acc); A = 0x02;
  if (F & FZ) {
    CYCT(0x5acc, 0x5ace); goto setAnimationAndLoadScript;
  }
  CYC(0x5acc, 0x5ace);
  CYC(0x5ace, 0x5ad0); A = 0x48;
  CYC(0x5ad0, 0x5ad1); mem_wr(gb, DE, A);
  CYC(0x5ad1, 0x5ad3); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5ad3, 0x5ad5); A = 0x58;
  CYC(0x5ad5, 0x5ad6); mem_wr(gb, DE, A);
  CYC(0x5ad6, 0x5ad8); A = 0x81;
  CYC(0x5ad8, 0x5adb); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5adb, 0x5ade); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5ade, 0x5ae0); A = 0x00;
  CYC(0x5ae0, 0x5ae3); mem_wr(gb, wScrollMode, A);
  CYC(0x5ae3, 0x5ae6); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5ae6, 0x5ae8); B = 0x10;
  CALL_C(0x5ae8, clearMemory_hook, 0x046f, 0x5aeb);
  CYC(0x5aeb, 0x5aee); SET_BC(0xad06); // INTERAC_ZELDA, $06
  CALL_C(0x5aee, objectCreateInteraction_hook, 0x24c5, 0x5af1);
  CYC(0x5af1, 0x5af3); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5af3, 0x5af5); mem_wr(gb, HL, 0x8c);
  CYC(0x5af5, 0x5af7); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5af7, 0x5af9); mem_wr(gb, HL, 0x50);
  CYC(0x5af9, 0x5afb); A = 0x02;
  CYC(0x5afb, 0x5afd); goto setAnimationAndLoadScript;

init3:
  CYC(0x5afd, 0x5aff); A = 0x03;

setAnimationAndLoadScript:
  CALL_C(0x5aff, interactionSetAnimation_hook, 0x262e, 0x5b02);
  CALL_C(0x5b02, objectSetVisible82_hook, 0x1e69, 0x5b05);
  CYC(0x5b05, 0x5b07); impaInCutscene_loadScript(gb);
  return;

init4:
  CALL_C(0x5b07, checkIsLinkedGame_hook, 0x1992, 0x5b0a);
  if (!(F & FZ)) {
    CYCT(0x5b0a, 0x5b0d); interactionDelete_hook(gb); return;
  }
  CYC(0x5b0a, 0x5b0d);
  CYC(0x5b0d, 0x5b0e); alu_xor(gb, A);
  CYC(0x5b0e, 0x5b11); mem_wr(gb, wTmpcfc0, A);

preBlackTowerCutscene:
  CYC(0x5b11, 0x5b13); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x5b13, checkTreasureObtained_hook, 0x1748, 0x5b16);
  if (!(F & FC)) {
    CYCT(0x5b16, 0x5b19); interactionDelete_hook(gb); return;
  }
  CYC(0x5b16, 0x5b19);
  CYC(0x5b19, 0x5b1b); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x5b1b, checkGlobalFlag_hook, 0x31f3, 0x5b1e);
  if (!(F & FZ)) {
    CYCT(0x5b1e, 0x5b21); interactionDelete_hook(gb); return;
  }
  CYC(0x5b1e, 0x5b21);
  CYC(0x5b21, 0x5b24); impaInCutscene_loadScript(gb);
  return;

init5:
  CALL_C(0x5b24, checkIsLinkedGame_hook, 0x1992, 0x5b27);
  if (F & FZ) {
    CYCT(0x5b27, 0x5b2a); interactionDelete_hook(gb); return;
  }
  CYC(0x5b27, 0x5b2a);
  CYC(0x5b2a, 0x5b2c); A = 0x03;
  CALL_C(0x5b2c, interactionSetAnimation_hook, 0x262e, 0x5b2f);
  CYC(0x5b2f, 0x5b31); goto preBlackTowerCutscene;

initA:
  CYC(0x5b31, 0x5b33); A = 0x02;
  CYC(0x5b33, 0x5b36); interactionSetAnimation_hook(gb);
  return;

init9:
  CALL_C(0x5b36, checkIsLinkedGame_hook, 0x1992, 0x5b39);
  if (F & FZ) {
    CYCT(0x5b39, 0x5b3c); interactionDelete_hook(gb); return;
  }
  CYC(0x5b39, 0x5b3c);

init8:
  CYC(0x5b3c, 0x5b3e); A = 0x03;
  CALL_C(0x5b3e, interactionSetAnimation_hook, 0x262e, 0x5b41);
  CYC(0x5b41, 0x5b44); push_effect(gb, 0x5b44); impaInCutscene_loadScript(gb);
  // No ret: @init8 falls through into impaState1.
  impaState1_hook(gb);
}

// INTERAC_IMPA_IN_CUTSCENE
//
// Variables:
//   var3b: For subid 1, saves impa's "oamTileIndexBase" so it can be restored after Impa
//          gets up (she references a different sprite sheet for her "collapsed" sprite)
void interactionCode31_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5a29, 0x5a2b); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5a2b, 0x5a2c); A = mem_rd(gb, DE);
  CYC(0x5a2c, 0x5a2d); push_effect(gb, 0x5a2d);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5a31: goto state0;
    case 0x5b44: impaState1_hook(gb); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x5a31, 0x5a33); A = 0x01;
  CYC(0x5a33, 0x5a34); mem_wr(gb, DE, A);
  CALL_C(0x5a34, interactionInitGraphics_hook, 0x15fb, 0x5a37);
  CALL_C(0x5a37, objectSetVisiblec2_hook, 0x1e45, 0x5a3a);
  CYC(0x5a3a, 0x5a3d); push_effect(gb, 0x5a3d); impaInCutscene_initSubid(gb, sp0_);
  CYC(0x5a3d, 0x5a3f); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x5a3f, 0x5a40); A = mem_rd(gb, DE);
  CYC(0x5a40, 0x5a41); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5a41, 0x5a44); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x5a41, 0x5a44);
  CYC(0x5a44, 0x5a45); ret_effect(gb);
}

void impaState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b44, 0x5b46); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5b46, 0x5b47); A = mem_rd(gb, DE);
  CYC(0x5b47, 0x5b48); push_effect(gb, 0x5b48);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5b5e: impaSubid0_hook(gb); return;
    case 0x5d5f: impaSubid1_hook(gb); return;
    case 0x5da6: impaSubid2_hook(gb); return;
    case 0x5df2: impaAnimateAndRunScript_hook(gb); return; // subids 3 and 6
    case 0x5e5b: impaSubid4_hook(gb); return;
    case 0x5f04: impaSubid5_hook(gb); return;
    case 0x5f50: impaSubid7_hook(gb); return;
    case 0x5f6e: impaSubid8_hook(gb); return;
    case 0x5f75: impaSubid9_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// impaSubid0@setAngleTowardStone: reached only by static `call`s from impaSubid0 itself.
static void impaInCutscene_setAngleTowardStone(GB *gb, uint16_t sp0_) {
  CYC(0x5c32, 0x5c35); SET_BC(0x3838);
  CALL_C(0x5c35, objectGetRelativeAngle_hook, 0x1ea4, 0x5c38);
  CYC(0x5c38, 0x5c3a); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5c3a, 0x5c3b); mem_wr(gb, DE, A);
  CYC(0x5c3b, 0x5c3c); ret_effect(gb);
}

// Possessed Impa.
//
// Variables:
//   var37-var3a: Last frame's Y, X, and Direction values. Used for checking whether to
//                update Impa's animation (update if any one has changed).
void impaSubid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b5e, 0x5b60); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5b60, 0x5b61); A = mem_rd(gb, DE);
  CYC(0x5b61, 0x5b63); alu_cp(gb, 0x0e);
  if (!(F & FC)) {
    CYCT(0x5b63, 0x5b65); goto dispatch;
  }
  CYC(0x5b63, 0x5b65);
  CYC(0x5b65, 0x5b68); SET_HL(wActiveMusic);
  CYC(0x5b68, 0x5b6a); A = 0x0f; // MUS_FAIRY_FOUNTAIN
  CYC(0x5b6a, 0x5b6b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5b6b, 0x5b6d); goto dispatch;
  }
  CYC(0x5b6b, 0x5b6d);
  CYC(0x5b6d, 0x5b70); A = mem_rd(gb, wActiveRoom);
  CYC(0x5b70, 0x5b72); alu_cp(gb, 0x39);
  if (F & FZ) {
    CYCT(0x5b72, 0x5b74); goto dispatch;
  }
  CYC(0x5b72, 0x5b74);
  CYC(0x5b74, 0x5b76); alu_cp(gb, 0x49);
  if (F & FZ) {
    CYCT(0x5b76, 0x5b78); goto dispatch;
  }
  CYC(0x5b76, 0x5b78);
  CYC(0x5b78, 0x5b7a); A = 0x0f; // MUS_FAIRY_FOUNTAIN
  CYC(0x5b7a, 0x5b7b); mem_wr(gb, HL, A);
  CALL_C(0x5b7b, playSound_b00_hook, 0x0c98, 0x5b7e);
  CYC(0x5b7e, 0x5b80); A = 0x03;
  CALL_C(0x5b80, setMusicVolume_hook, 0x0cad, 0x5b83);
  CYC(0x5b83, 0x5b85); E = INTERACTION_BASE + OBJ_SUBSTATE;

dispatch:
  CYC(0x5b85, 0x5b86); A = mem_rd(gb, DE);
  CYC(0x5b86, 0x5b87); push_effect(gb, 0x5b87);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5ba9: goto substate0;
    case 0x5bdf: goto substate1;
    case 0x5c3c: goto substate2;
    case 0x5c4e: goto substate3;
    case 0x5c5d: goto substate4;
    case 0x5c68: goto substate5;
    case 0x5c92: goto substate6;
    case 0x5ca1: goto substate7;
    case 0x5cb3: goto substate8;
    case 0x5cc2: goto substate9;
    case 0x5cd4: goto substateA;
    case 0x5ce9: goto substateB;
    case 0x5d01: goto substateC;
    case 0x5d10: goto substateD;
    case 0x5d2d: goto substateE;
    case 0x5d4a: goto substateF;
    case 0x5d5e: impaRet_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  // Running a script until Impa joins Link
  CALL_C(0x5ba9, impaAnimateAndRunScript_hook, 0x5df2, 0x5bac);
  if (!(F & FC)) {
    CYCT(0x5bac, 0x5bad); ret_effect(gb); return;
  }
  CYC(0x5bac, 0x5bad);
  // When the script has finished, make Impa follow Link and go to substate 1
  CYC(0x5bad, 0x5bae); alu_xor(gb, A);
  CYC(0x5bae, 0x5bb1); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(0x5bb1, setLinkIDOverride_hook, 0x2acf, 0x5bb4);
  CYC(0x5bb4, 0x5bb6); L = (uint8_t)w1Link_direction;
  CYC(0x5bb6, 0x5bb8); mem_wr(gb, HL, 0x00); // DIR_UP

beginFollowingLink:
  CALL_C(0x5bb8, interactionIncSubstate_hook, 0x23e5, 0x5bbb);
  CALL_C(0x5bbb, makeActiveObjectFollowLink_b00_hook, 0x1832, 0x5bbe);
  CALL_C(0x5bbe, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5bc1);
  CALL_C(0x5bc1, objectSetReservedBit1_hook, 0x1e8d, 0x5bc4);
  CYC(0x5bc4, 0x5bc6); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x5bc6, 0x5bc8); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5bc8, 0x5bc9); A = mem_rd(gb, DE);
  CYC(0x5bc9, 0x5bca); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5bca, 0x5bcc); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5bcc, 0x5bcd); A = mem_rd(gb, DE);
  CYC(0x5bcd, 0x5bce); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5bce, 0x5bd0); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x5bd0, 0x5bd3); A = W8(w1Link_direction);
  CYC(0x5bd3, 0x5bd4); mem_wr(gb, DE, A);
  CYC(0x5bd4, 0x5bd6); mem_wr(gb, HL, 0x00);
  CALL_C(0x5bd6, interactionSetAnimation_hook, 0x262e, 0x5bd9);
  CALL_C(0x5bd9, objectSetVisiblec3_hook, 0x1e4e, 0x5bdc);
  CYC(0x5bdc, 0x5bdf); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
  return;

substate1:
  // Impa following Link (before stone is pushed)
  CALL_C(0x5bdf, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x5be2);
  CALL_C(0x5be2, impaCheckApproachedStone_hook, 0x5fac, 0x5be5);
  if (!(F & FC)) {
    CYCT(0x5be5, 0x5be7); goto updateAnimationWhileFollowingLink;
  }
  CYC(0x5be5, 0x5be7);
  // Link has approached the stone; trigger cutscene.
  CYC(0x5be7, 0x5be9); A = 0x08; // LINK_STATE_08
  CALL_C(0x5be9, setLinkIDOverride_hook, 0x2acf, 0x5bec);
  CYC(0x5bec, 0x5bee); L = (uint8_t)w1Link_subid;
  CYC(0x5bee, 0x5bf0); mem_wr(gb, HL, 0x02);
  CALL_C(0x5bf0, interactionIncSubstate_hook, 0x23e5, 0x5bf3);
  CYC(0x5bf3, 0x5bf5); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5bf5, 0x5bf7); mem_wr(gb, HL, 0x1e);
  CYC(0x5bf7, 0x5bf9); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x5bf9, 0x5bfb); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x5bfb, 0x5bfd); A = 0x50; // SND_CLINK
  CALL_C(0x5bfd, playSound_b00_hook, 0x0c98, 0x5c00);
  CYC(0x5c00, 0x5c03); SET_BC(0xfe40); // -$1c0
  CALL_C(0x5c03, objectSetSpeedZ_hook, 0x239d, 0x5c06);
  CALL_C(0x5c06, clearFollowingLinkObject_hook, 0x1846, 0x5c09);
  CYC(0x5c09, 0x5c0c); push_effect(gb, 0x5c0c); impaInCutscene_setAngleTowardStone(gb, sp0_);
  CALL_C(0x5c0c, convertAngleDeToDirection_hook, 0x26f8, 0x5c0f);
  CYC(0x5c0f, 0x5c12); interactionSetAnimation_hook(gb);
  return;

updateAnimationWhileFollowingLink:
  // Nothing to do here except check whether to update the animation. (It must update if
  // her position or direction has changed.)
  CALL_C(0x5c12, impaUpdateAnimationIfDirectionChanged_hook, 0x5fa0, 0x5c15);
  CYC(0x5c15, 0x5c16); H = D;
  CYC(0x5c16, 0x5c18); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5c18, 0x5c19); A = mem_rd(gb, HL);
  CYC(0x5c19, 0x5c1a); B = A;
  CYC(0x5c1a, 0x5c1c); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x5c1c, 0x5c1d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x5c1d, 0x5c1f); goto positionChanged;
  }
  CYC(0x5c1d, 0x5c1f);
  CYC(0x5c1f, 0x5c21); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5c21, 0x5c22); A = mem_rd(gb, HL);
  CYC(0x5c22, 0x5c23); C = A;
  CYC(0x5c23, 0x5c25); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5c25, 0x5c26); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5c26, 0x5c27); ret_effect(gb); return;
  }
  CYC(0x5c26, 0x5c27);

positionChanged:
  CYC(0x5c27, 0x5c29); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x5c29, 0x5c2a); mem_wr(gb, HL, B);
  CYC(0x5c2a, 0x5c2b); L = alu_inc8(gb, L);
  CYC(0x5c2b, 0x5c2c); mem_wr(gb, HL, C);
  CALL_C(0x5c2c, interactionAnimate_hook, 0x261b, 0x5c2f);
  CYC(0x5c2f, 0x5c32); interactionAnimate_hook(gb);
  return;

substate2:
  // Jumping after spotting stone
  CALL_C(0x5c3c, impaAnimateAndDecCounter1_hook, 0x5fc8, 0x5c3f);
  if (!(F & FZ)) {
    CYCT(0x5c3f, 0x5c40); ret_effect(gb); return;
  }
  CYC(0x5c3f, 0x5c40);
  // Wait until she lands
  CYC(0x5c40, 0x5c42); C = 0x20;
  CALL_C(0x5c42, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5c45);
  if (!(F & FZ)) {
    CYCT(0x5c45, 0x5c46); ret_effect(gb); return;
  }
  CYC(0x5c45, 0x5c46);
  CALL_C(0x5c46, interactionIncSubstate_hook, 0x23e5, 0x5c49);
  CYC(0x5c49, 0x5c4b); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5c4b, 0x5c4d); mem_wr(gb, HL, 0x0a);
  CYC(0x5c4d, 0x5c4e); ret_effect(gb);
  return;

substate3:
  CALL_C(0x5c4e, impaAnimateAndDecCounter1_hook, 0x5fc8, 0x5c51);
  if (!(F & FZ)) {
    CYCT(0x5c51, 0x5c52); ret_effect(gb); return;
  }
  CYC(0x5c51, 0x5c52);
  CYC(0x5c52, 0x5c54); mem_wr(gb, HL, 0x14);
  CYC(0x5c54, 0x5c57); SET_BC(0x0104); // TX_0104
  CALL_C(0x5c57, showText_hook, 0x1872, 0x5c5a);
  CYC(0x5c5a, 0x5c5d); interactionIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(0x5c5d, interactionDecCounter1IfTextNotActive_hook, 0x273c, 0x5c60);
  if (!(F & FZ)) {
    CYCT(0x5c60, 0x5c61); ret_effect(gb); return;
  }
  CYC(0x5c60, 0x5c61);
  CYC(0x5c61, 0x5c63); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5c63, 0x5c65); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(0x5c65, 0x5c68); interactionIncSubstate_hook(gb);
  return;

substate5:
  // Moving toward stone
  CALL_C(0x5c68, interactionAnimate3Times_hook, 0x274f, 0x5c6b);
  CALL_C(0x5c6b, objectApplySpeed_hook, 0x201d, 0x5c6e);
  CYC(0x5c6e, 0x5c71); push_effect(gb, 0x5c71); impaInCutscene_setAngleTowardStone(gb, sp0_);
  CYC(0x5c71, 0x5c73); A = 0x02;
  CYC(0x5c73, 0x5c75); H8(hFF8B) = A;
  CYC(0x5c75, 0x5c78); SET_BC(0x3838);
  CYC(0x5c78, 0x5c79); H = D;
  CYC(0x5c79, 0x5c7b); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(0x5c7b, checkObjectIsCloseToPosition_b08_hook, 0x5f86, 0x5c7e);
  if (!(F & FC)) {
    CYCT(0x5c7e, 0x5c7f); ret_effect(gb); return;
  }
  CYC(0x5c7e, 0x5c7f);
  // Reached the stone
  CYC(0x5c7f, 0x5c80); H = D;
  CALL_C(0x5c80, interactionIncSubstate_hook, 0x23e5, 0x5c83);
  CYC(0x5c83, 0x5c85); A = 0x38;
  CYC(0x5c85, 0x5c87); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5c87, 0x5c88); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5c88, 0x5c89); L = alu_inc8(gb, L);
  CYC(0x5c89, 0x5c8a); mem_wr(gb, HL, A);
  CYC(0x5c8a, 0x5c8c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5c8c, 0x5c8e); mem_wr(gb, HL, 0x1e);
  CYC(0x5c8e, 0x5c8f); alu_xor(gb, A);
  CYC(0x5c8f, 0x5c92); interactionSetAnimation_hook(gb);
  return;

substate6:
  CALL_C(0x5c92, impaAnimateAndDecCounter1_hook, 0x5fc8, 0x5c95);
  if (!(F & FZ)) {
    CYCT(0x5c95, 0x5c96); ret_effect(gb); return;
  }
  CYC(0x5c95, 0x5c96);
  // Start a jump
  CYC(0x5c96, 0x5c98); mem_wr(gb, HL, 0x1e);
  CYC(0x5c98, 0x5c9b); SET_BC(0xfe80); // -$180
  CALL_C(0x5c9b, objectSetSpeedZ_hook, 0x239d, 0x5c9e);
  CYC(0x5c9e, 0x5ca1); interactionIncSubstate_hook(gb);
  return;

substate7:
  // Jumping in front of stone
  CALL_C(0x5ca1, impaAnimateAndDecCounter1_hook, 0x5fc8, 0x5ca4);
  if (!(F & FZ)) {
    CYCT(0x5ca4, 0x5ca5); ret_effect(gb); return;
  }
  CYC(0x5ca4, 0x5ca5);
  CYC(0x5ca5, 0x5ca7); C = 0x20;
  CALL_C(0x5ca7, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5caa);
  if (!(F & FZ)) {
    CYCT(0x5caa, 0x5cab); ret_effect(gb); return;
  }
  CYC(0x5caa, 0x5cab);
  CALL_C(0x5cab, interactionIncSubstate_hook, 0x23e5, 0x5cae);
  CYC(0x5cae, 0x5cb0); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5cb0, 0x5cb2); mem_wr(gb, HL, 0x0a);
  CYC(0x5cb2, 0x5cb3); ret_effect(gb);
  return;

substate8:
  CALL_C(0x5cb3, interactionDecCounter1_hook, 0x23cc, 0x5cb6);
  if (!(F & FZ)) {
    CYCT(0x5cb6, 0x5cb7); ret_effect(gb); return;
  }
  CYC(0x5cb6, 0x5cb7);
  CYC(0x5cb7, 0x5cb9); mem_wr(gb, HL, 0x1e);
  CALL_C(0x5cb9, interactionIncSubstate_hook, 0x23e5, 0x5cbc);
  CYC(0x5cbc, 0x5cbf); SET_BC(0x0105); // TX_0105
  CYC(0x5cbf, 0x5cc2); showText_hook(gb);
  return;

substate9:
  CALL_C(0x5cc2, interactionDecCounter1IfTextNotActive_hook, 0x273c, 0x5cc5);
  if (!(F & FZ)) {
    CYCT(0x5cc5, 0x5cc6); ret_effect(gb); return;
  }
  CYC(0x5cc5, 0x5cc6);
  CYC(0x5cc6, 0x5cc9); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5cc9, 0x5ccb); mem_wr(gb, HL, 0x02);
  CYC(0x5ccb, 0x5cce); SET_HL(impaScript_moveAwayFromRock_bank0c);
  CALL_C(0x5cce, interactionSetScript_hook, 0x2544, 0x5cd1);
  CYC(0x5cd1, 0x5cd4); interactionIncSubstate_hook(gb);
  return;

substateA:
  // Moving away from rock (the previously loaded script handles this)
  CALL_C(0x5cd4, impaAnimateAndRunScript_hook, 0x5df2, 0x5cd7);
  if (!(F & FC)) {
    CYCT(0x5cd7, 0x5cd8); ret_effect(gb); return;
  }
  CYC(0x5cd7, 0x5cd8);
  // Done moving away; return control to Link
  CYC(0x5cd8, 0x5cd9); alu_xor(gb, A);
  CALL_C(0x5cd9, setLinkIDOverride_hook, 0x2acf, 0x5cdc);
  CYC(0x5cdc, 0x5cde); L = (uint8_t)w1Link_direction;
  CYC(0x5cde, 0x5ce0); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(0x5ce0, 0x5ce3); SET_HL(impaScript_waitForRockToBeMoved_bank0c);
  CALL_C(0x5ce3, interactionSetScript_hook, 0x2544, 0x5ce6);
  CYC(0x5ce6, 0x5ce9); interactionIncSubstate_hook(gb);
  return;

substateB:
  // Waiting for Link to start pushing the rock
  CALL_C(0x5ce9, interactionAnimateAsNpc_hook, 0x26db, 0x5cec);
  CALL_C(0x5cec, interactionRunScript_hook, 0x2552, 0x5cef);
  CALL_C(0x5cef, impaPreventLinkFromLeavingStoneScreen_hook, 0x5fd5, 0x5cf2);
  CYC(0x5cf2, 0x5cf5); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5cf5, 0x5cf7); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x5cf7, 0x5cf8); ret_effect(gb); return;
  }
  CYC(0x5cf7, 0x5cf8);
  // The rock has started moving.
  CYC(0x5cf8, 0x5cfb); SET_HL(impaScript_rockJustMoved_bank0c);
  CALL_C(0x5cfb, interactionSetScript_hook, 0x2544, 0x5cfe);
  CYC(0x5cfe, 0x5d01); interactionIncSubstate_hook(gb);
  return;

substateC:
  CALL_C(0x5d01, impaAnimateAndRunScript_hook, 0x5df2, 0x5d04);
  if (!(F & FC)) {
    CYCT(0x5d04, 0x5d05); ret_effect(gb); return;
  }
  CYC(0x5d04, 0x5d05);
  CYC(0x5d05, 0x5d06); alu_xor(gb, A);
  CALL_C(0x5d06, setLinkIDOverride_hook, 0x2acf, 0x5d09);
  CYC(0x5d09, 0x5d0b); L = (uint8_t)w1Link_direction;
  CYC(0x5d0b, 0x5d0d); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(0x5d0d, 0x5d10); goto beginFollowingLink;

substateD:
  // Following Link, waiting for signal to begin the part of the cutscene where she reveals
  // she's evil
  CALL_C(0x5d10, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x5d13);
  CYC(0x5d13, 0x5d16); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5d16, 0x5d18); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(0x5d18, 0x5d1b); goto updateAnimationWhileFollowingLink;
  }
  CYC(0x5d18, 0x5d1b);
  // Start the next part of the cutscene
  CALL_C(0x5d1b, interactionIncSubstate_hook, 0x23e5, 0x5d1e);
  CALL_C(0x5d1e, clearFollowingLinkObject_hook, 0x1846, 0x5d21);
  CYC(0x5d21, 0x5d24); SET_BC(0x6838);
  CALL_C(0x5d24, interactionSetPosition_hook, 0x2773, 0x5d27);
  CYC(0x5d27, 0x5d2a); SET_HL(impaScript_revealPossession_bank0c);
  CYC(0x5d2a, 0x5d2d); interactionSetScript_hook(gb);
  return;

substateE:
  CALL_C(0x5d2d, impaAnimateAndRunScript_hook, 0x5df2, 0x5d30);
  if (!(F & FC)) {
    CYCT(0x5d30, 0x5d31); ret_effect(gb); return;
  }
  CYC(0x5d30, 0x5d31);
  // Impa has just moved into the corner, Veran will now come out.
  CALL_C(0x5d31, interactionIncSubstate_hook, 0x23e5, 0x5d34);
  CYC(0x5d34, 0x5d36); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x5d36, 0x5d38); mem_wr(gb, HL, 0x02);
  CYC(0x5d38, 0x5d3a); A = 0x05;
  CALL_C(0x5d3a, interactionSetAnimation_hook, 0x262e, 0x5d3d);
  CYC(0x5d3d, 0x5d3f); B = 0x3e; // INTERAC_GHOST_VERAN
  CALL_C(0x5d3f, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5d42);
  CYC(0x5d42, 0x5d44); A = 0x67; // SND_BOSS_DEAD
  CALL_C(0x5d44, playSound_b00_hook, 0x0c98, 0x5d47);
  CYC(0x5d47, 0x5d4a); objectSetVisiblec2_hook(gb);
  return;

substateF:
  CALL_C(0x5d4a, interactionAnimate_hook, 0x261b, 0x5d4d);
  CYC(0x5d4d, 0x5d4e); H = D;
  CYC(0x5d4e, 0x5d50); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5d50, 0x5d51); A = mem_rd(gb, HL);
  CYC(0x5d51, 0x5d52); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5d52, 0x5d53); ret_effect(gb); return;
  }
  CYC(0x5d52, 0x5d53);
  CALL_C(0x5d53, interactionIncSubstate_hook, 0x23e5, 0x5d56);
  // Falls through into impaLoadCollapsedGraphic.
  impaLoadCollapsedGraphic_hook(gb);
}

// Changes impa's "oamTileIndexBase" to reference her "collapsed" graphic, which is not in
// her normal sprite sheet. Falls through into impaRet.
void impaLoadCollapsedGraphic_hook(GB *gb) {
  CYC(0x5d56, 0x5d58); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x5d58, 0x5d5a); mem_wr(gb, HL, 0x0a);
  CYC(0x5d5a, 0x5d5c); L = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(0x5d5c, 0x5d5e); mem_wr(gb, HL, 0x60);
  impaRet_hook(gb);
}

void impaRet_hook(GB *gb) {
  CYC(0x5d5e, 0x5d5f); ret_effect(gb);
}

// Impa talking to you after Nayru is kidnapped
void impaSubid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d5f, 0x5d61); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5d61, 0x5d62); A = mem_rd(gb, DE);
  CYC(0x5d62, 0x5d63); push_effect(gb, 0x5d63);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5d69: goto substate0;
    case 0x5d7f: goto substate1;
    case 0x5d96: impaSubid1Substate2_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x5d69, 0x5d6c); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5d6c, 0x5d6e); alu_cp(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x5d6e, 0x5d71); interactionAnimate_hook(gb); return;
  }
  CYC(0x5d6e, 0x5d71);
  CALL_C(0x5d71, interactionIncSubstate_hook, 0x23e5, 0x5d74);
  CYC(0x5d74, 0x5d76); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5d76, 0x5d77); A = mem_rd(gb, DE);
  CYC(0x5d77, 0x5d79); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x5d79, 0x5d7a); mem_wr(gb, HL, A);
  CYC(0x5d7a, 0x5d7c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5d7c, 0x5d7e); mem_wr(gb, HL, 0x3c);
  CYC(0x5d7e, 0x5d7f); ret_effect(gb);
  return;

substate1:
  CALL_C(0x5d7f, interactionDecCounter1_hook, 0x23cc, 0x5d82);
  if (!(F & FZ)) {
    CYCT(0x5d82, 0x5d84); interactionOscillateXRandomly_hook(gb); return;
  }
  CYC(0x5d82, 0x5d84);
  CYC(0x5d84, 0x5d87); interactionIncSubstate_hook(gb);
}

// Uses var3d as the interaction's "base" position, and randomly shifts this position left
// by one or not at all.
void interactionOscillateXRandomly_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d87, getRandomNumber_hook, 0x043e, 0x5d8a);
  CYC(0x5d8a, 0x5d8c); alu_and(gb, 0x01);
  CYC(0x5d8c, 0x5d8e); alu_sub(gb, 0x01);
  CYC(0x5d8e, 0x5d8f); H = D;
  CYC(0x5d8f, 0x5d91); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x5d91, 0x5d92); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5d92, 0x5d94); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5d94, 0x5d95); mem_wr(gb, HL, A);
  CYC(0x5d95, 0x5d96); ret_effect(gb);
}

void impaSubid1Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5d96, interactionRunScript_hook, 0x2552, 0x5d99);
  if (F & FC) {
    CYCT(0x5d99, 0x5d9c); interactionDelete_hook(gb); return;
  }
  CYC(0x5d99, 0x5d9c);
  CYC(0x5d9c, 0x5d9e); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x5d9e, 0x5d9f); A = mem_rd(gb, DE);
  CYC(0x5d9f, 0x5da0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5da0, 0x5da3); interactionAnimate2Times_hook(gb); return;
  }
  CYC(0x5da0, 0x5da3);
  CYC(0x5da3, 0x5da6); interactionAnimate_hook(gb);
}

// Impa in the credits cutscene
void impaSubid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5da6, 0x5da8); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5da8, 0x5da9); A = mem_rd(gb, DE);
  CYC(0x5da9, 0x5daa); push_effect(gb, 0x5daa);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5dba: goto substate0;
    case 0x5dcb: goto substate1;
    case 0x5ddb: goto substate2;
    case 0x5df2: impaAnimateAndRunScript_hook(gb); return;
    case 0x5df8: impaSubid2Substate4_hook(gb); return;
    case 0x5e0d: impaSubid2Substate5_hook(gb); return;
    case 0x5e23: impaSubid2Substate6_hook(gb); return;
    case 0x5e4a: impaSubid2Substate7_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x5dba, interactionDecCounter1IfPaletteNotFading_hook, 0x2744, 0x5dbd);
  if (!(F & FZ)) {
    CYCT(0x5dbd, 0x5dbe); ret_effect(gb); return;
  }
  CYC(0x5dbd, 0x5dbe);
  CYC(0x5dbe, 0x5dc0); mem_wr(gb, HL, 0x3c);
  CALL_C(0x5dc0, interactionIncSubstate_hook, 0x23e5, 0x5dc3);
  CYC(0x5dc3, 0x5dc5); A = 0x50;
  CYC(0x5dc5, 0x5dc8); SET_BC(0x6050);
  CYC(0x5dc8, 0x5dcb); createEnergySwirlGoingIn_hook(gb);
  return;

substate1:
  CALL_C(0x5dcb, interactionDecCounter1_hook, 0x23cc, 0x5dce);
  if (!(F & FZ)) {
    CYCT(0x5dce, 0x5dcf); ret_effect(gb); return;
  }
  CYC(0x5dce, 0x5dcf);
  CYC(0x5dcf, 0x5dd2); SET_HL(wTmpcbb3);
  CYC(0x5dd2, 0x5dd3); alu_xor(gb, A);
  CYC(0x5dd3, 0x5dd4); mem_wr(gb, HL, A);
  CYC(0x5dd4, 0x5dd5); A = alu_dec8(gb, A);
  CYC(0x5dd5, 0x5dd8); mem_wr(gb, wTmpcbba, A);
  CYC(0x5dd8, 0x5ddb); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x5ddb, 0x5dde); SET_HL(wTmpcbb3);
  CYC(0x5dde, 0x5de0); B = 0x02;
  CALL_C(0x5de0, flashScreen_hook, 0x2d73, 0x5de3);
  if (F & FZ) {
    CYCT(0x5de3, 0x5de4); ret_effect(gb); return;
  }
  CYC(0x5de3, 0x5de4);
  CALL_C(0x5de4, interactionIncSubstate_hook, 0x23e5, 0x5de7);
  CYC(0x5de7, 0x5dea); push_effect(gb, 0x5dea); impaInCutscene_loadScript(gb);
  CYC(0x5dea, 0x5dec); A = 0x01;
  CYC(0x5dec, 0x5def); mem_wr(gb, wTmpcfc0, A);
  CYC(0x5def, 0x5df2); fadeinFromWhite_hook(gb);
}

void impaAnimateAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5df2, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5df5);
  CYC(0x5df5, 0x5df8); interactionRunScript_hook(gb);
}

// Falls through into impaSetVisibleAndJump.
void impaSubid2Substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5df8, 0x5df9); H = D;
  CYC(0x5df9, 0x5dfb); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5dfb, 0x5dfc); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5dfc, 0x5dfd); ret_effect(gb); return;
  }
  CYC(0x5dfc, 0x5dfd);
  CALL_C(0x5dfd, interactionIncSubstate_hook, 0x23e5, 0x5e00);
  CYC(0x5e00, 0x5e02); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5e02, 0x5e04); mem_wr(gb, HL, 0x02);
  impaSetVisibleAndJump_hook(gb);
}

void impaSetVisibleAndJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e04, objectSetVisiblec2_hook, 0x1e45, 0x5e07);
  CYC(0x5e07, 0x5e0a); SET_BC(0xfe80); // -$180
  CYC(0x5e0a, 0x5e0d); objectSetSpeedZ_hook(gb);
}

void impaSubid2Substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e0d, 0x5e0f); C = 0x20;
  CALL_C(0x5e0f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5e12);
  if (!(F & FZ)) {
    CYCT(0x5e12, 0x5e13); ret_effect(gb); return;
  }
  CYC(0x5e12, 0x5e13);
  CALL_C(0x5e13, interactionDecCounter1_hook, 0x23cc, 0x5e16);
  if (!(F & FZ)) {
    CYCT(0x5e16, 0x5e18); impaSetVisibleAndJump_hook(gb); return;
  }
  CYC(0x5e16, 0x5e18);
  CALL_C(0x5e18, objectSetVisible82_hook, 0x1e69, 0x5e1b);
  CYC(0x5e1b, 0x5e1c); H = D;
  CYC(0x5e1c, 0x5e1e); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5e1e, 0x5e20); mem_wr(gb, HL, 0x10);
  CYC(0x5e20, 0x5e23); interactionIncSubstate_hook(gb);
}

void impaSubid2Substate6_hook(GB *gb) {
  CYC(0x5e23, 0x5e24); H = D;
  CYC(0x5e24, 0x5e26); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5e26, 0x5e27); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5e27, 0x5e28); ret_effect(gb); return;
  }
  CYC(0x5e27, 0x5e28);
  CYC(0x5e28, 0x5e2a); mem_wr(gb, HL, 0x10);
  CYC(0x5e2a, 0x5e2c); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x5e2c, 0x5e2d); A = mem_rd(gb, HL);
  CYC(0x5e2d, 0x5e2e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5e2e, 0x5e30); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x5e30, 0x5e32); goto nextState;
  }
  CYC(0x5e30, 0x5e32);
  CYC(0x5e32, 0x5e33); alu_or(gb, A);
  CYC(0x5e33, 0x5e35); A = 0x03;
  if (F & FZ) {
    CYCT(0x5e35, 0x5e37); goto setAnimation;
  }
  CYC(0x5e35, 0x5e37);
  CYC(0x5e37, 0x5e39); alu_xor(gb, 0x02);

setAnimation:
  CYC(0x5e39, 0x5e3b); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5e3b, 0x5e3c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e3c, 0x5e3d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5e3d, 0x5e40); interactionSetAnimation_hook(gb);
  return;

nextState:
  CYC(0x5e40, 0x5e42); mem_wr(gb, HL, 0x00);
  CYC(0x5e42, 0x5e44); A = 0x02;
  CYC(0x5e44, 0x5e47); mem_wr(gb, wTmpcfc0, A);
  CYC(0x5e47, 0x5e4a); interactionIncSubstate_hook(gb);
}

void impaSubid2Substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e4a, impaAnimateAndRunScript_hook, 0x5df2, 0x5e4d);
  CYC(0x5e4d, 0x5e50); A = mem_rd(gb, wTmpcfc0);
  CYC(0x5e50, 0x5e52); alu_cp(gb, 0x03);
  if (F & FC) {
    CYCT(0x5e52, 0x5e53); ret_effect(gb); return;
  }
  CYC(0x5e52, 0x5e53);
  // jpab scriptHelp.turnToFaceSomething
  CYC(0x5e53, 0x5e56); SET_HL(turnToFaceSomething_bank15);
  CYC(0x5e56, 0x5e58); E = 0x15;
  CYC(0x5e58, 0x5e5b); interBankCall_hook(gb);
}

// Impa tells you about Ralph's heritage (unlinked)
void impaSubid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e5b, checkInteractionSubstate_hook, 0x2403, 0x5e5e);
  if (!(F & FZ)) {
    CYCT(0x5e5e, 0x5e60); goto substate1;
  }
  CYC(0x5e5e, 0x5e60);

  // @substate0: wait for Link to move a certain distance down
  CYC(0x5e60, 0x5e63); SET_HL(w1Link_yh);
  CYC(0x5e63, 0x5e64); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5e64, 0x5e66); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(0x5e66, 0x5e67); ret_effect(gb); return;
  }
  CYC(0x5e66, 0x5e67);
  CYC(0x5e67, 0x5e69); L = (uint8_t)w1Link_zh;
  CYC(0x5e69, 0x5e6b); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x5e6b, 0x5e6c); ret_effect(gb); return;
  }
  CYC(0x5e6b, 0x5e6c);
  CALL_C(0x5e6c, checkLinkCollisionsEnabled_hook, 0x1d32, 0x5e6f);
  if (!(F & FC)) {
    CYCT(0x5e6f, 0x5e70); ret_effect(gb); return;
  }
  CYC(0x5e6f, 0x5e70);
  CALL_C(0x5e70, resetLinkInvincibility_hook, 0x2ba9, 0x5e73);
  CALL_C(0x5e73, setLinkForceStateToState08_hook, 0x2aad, 0x5e76);
  CYC(0x5e76, 0x5e77); A = alu_inc8(gb, A);
  CYC(0x5e77, 0x5e7a); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5e7a, 0x5e7d); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5e7d, 0x5e80); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x5e80, 0x5e82); C = 0x20;
  CALL_C(0x5e82, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5e85);
  if (!(F & FZ)) {
    CYCT(0x5e85, 0x5e86); ret_effect(gb); return;
  }
  CYC(0x5e85, 0x5e86);
  CALL_C(0x5e86, interactionRunScript_hook, 0x2552, 0x5e89);
  if (F & FC) {
    CYCT(0x5e89, 0x5e8c); interactionDelete_hook(gb); return;
  }
  CYC(0x5e89, 0x5e8c);
  CALL_C(0x5e8c, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5e8f);
  CYC(0x5e8f, 0x5e91); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5e91, 0x5e92); A = mem_rd(gb, DE);
  CYC(0x5e92, 0x5e93); push_effect(gb, 0x5e93);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5e9d: goto thing0;
    case 0x5ea9: goto thing1;
    case 0x5edb: goto thing2;
    case 0x5ef7: goto thing3;
    case 0x5f03: goto thing4;
    default: HANDOFF(HL);
  }

thing0:
  CYC(0x5e9d, 0x5ea0); A = mem_rd(gb, wTmpcfc0);
  CYC(0x5ea0, 0x5ea1); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x5ea1, 0x5ea2); ret_effect(gb); return;
  }
  CYC(0x5ea1, 0x5ea2);
  CYC(0x5ea2, 0x5ea4); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5ea4, 0x5ea6); A = 0x10;
  CYC(0x5ea6, 0x5ea7); mem_wr(gb, DE, A);
  CYC(0x5ea7, 0x5ea9); goto incVar38;

thing1:
  // Move Link horizontally toward Impa
  CYC(0x5ea9, 0x5eaa); H = D;
  CYC(0x5eaa, 0x5eac); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5eac, 0x5ead); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x5ead, 0x5eae); ret_effect(gb); return;
  }
  CYC(0x5ead, 0x5eae);
  CYC(0x5eae, 0x5eb1); A = W8(w1Link_xh);
  CYC(0x5eb1, 0x5eb3); alu_sub(gb, 0x50);
  CYC(0x5eb3, 0x5eb4); B = A;
  CYC(0x5eb4, 0x5eb6); alu_add(gb, 0x02);
  CYC(0x5eb6, 0x5eb8); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(0x5eb8, 0x5eba); goto incVar38;
  }
  CYC(0x5eb8, 0x5eba);
  CYC(0x5eba, 0x5ebb); A = B;
  CYC(0x5ebb, 0x5ebd); alu_bit(gb, 7, A);
  CYC(0x5ebd, 0x5ebf); B = 0x18;
  if (F & FZ) {
    CYCT(0x5ebf, 0x5ec1); goto forceMovement;
  }
  CYC(0x5ebf, 0x5ec1);
  CYC(0x5ec1, 0x5ec3); B = 0x08;
  CYC(0x5ec3, 0x5ec4); alu_cpl(gb);
  CYC(0x5ec4, 0x5ec5); A = alu_inc8(gb, A);

forceMovement:
  CYC(0x5ec5, 0x5ec8); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x5ec8, 0x5eca); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x5eca, 0x5ecd); mem_wr(gb, wLinkForceState, A);
  CYC(0x5ecd, 0x5ed0); SET_HL(w1Link_angle);
  CYC(0x5ed0, 0x5ed1); A = B;
  CYC(0x5ed1, 0x5ed2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5ed2, 0x5ed4); A = alu_swap(gb, A);
  CYC(0x5ed4, 0x5ed5); alu_rlca(gb);
  CYC(0x5ed5, 0x5ed6); mem_wr(gb, HL, A);

incVar38:
  CYC(0x5ed6, 0x5ed7); H = D;
  CYC(0x5ed7, 0x5ed9); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5ed9, 0x5eda); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5eda, 0x5edb); ret_effect(gb);
  return;

thing2:
  // Move Link vertically toward Impa
  CYC(0x5edb, 0x5ede); A = W8(w1Link_state);
  CYC(0x5ede, 0x5ee0); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(0x5ee0, 0x5ee1); ret_effect(gb); return;
  }
  CYC(0x5ee0, 0x5ee1);
  CYC(0x5ee1, 0x5ee4); A = W8(w1Link_yh);
  CYC(0x5ee4, 0x5ee6); alu_sub(gb, 0x48);
  CYC(0x5ee6, 0x5ee9); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x5ee9, 0x5eea); alu_xor(gb, A);
  CYC(0x5eea, 0x5eed); SET_HL(w1Link_direction);
  CYC(0x5eed, 0x5eee); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5eee, 0x5eef); mem_wr(gb, HL, A);
  CYC(0x5eef, 0x5ef1); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x5ef1, 0x5ef4); mem_wr(gb, wLinkForceState, A);
  CYC(0x5ef4, 0x5ef7); goto incVar38;

thing3:
  CYC(0x5ef7, 0x5efa); A = W8(w1Link_state);
  CYC(0x5efa, 0x5efc); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(0x5efc, 0x5efd); ret_effect(gb); return;
  }
  CYC(0x5efc, 0x5efd);
  CALL_C(0x5efd, setLinkForceStateToState08_hook, 0x2aad, 0x5f00);
  CYC(0x5f00, 0x5f03); goto incVar38;

thing4:
  CYC(0x5f03, 0x5f04); ret_effect(gb);
}

// Like above (explaining ralph's heritage), but for linked game
void impaSubid5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f04, 0x5f06); C = 0x20;
  CALL_C(0x5f06, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5f09);
  if (!(F & FZ)) {
    CYCT(0x5f09, 0x5f0a); ret_effect(gb); return;
  }
  CYC(0x5f09, 0x5f0a);
  CALL_C(0x5f0a, interactionRunScript_hook, 0x2552, 0x5f0d);
  if (!(F & FC)) {
    CYCT(0x5f0d, 0x5f0f); goto scriptRunning;
  }
  CYC(0x5f0d, 0x5f0f);
  // Script over
  CYC(0x5f0f, 0x5f10); alu_xor(gb, A);
  CYC(0x5f10, 0x5f13); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5f13, 0x5f16); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5f16, 0x5f18); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x5f18, setGlobalFlag_hook, 0x31f9, 0x5f1b);
  CYC(0x5f1b, 0x5f1e); interactionDelete_hook(gb);
  return;

scriptRunning:
  CALL_C(0x5f1e, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x5f21);
  CYC(0x5f21, 0x5f23); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5f23, 0x5f24); A = mem_rd(gb, DE);
  CYC(0x5f24, 0x5f25); push_effect(gb, 0x5f25);
  switch (impaInCutscene_jumpTable(gb)) {
    case 0x5f2b: goto substate0;
    case 0x5f43: goto substate1;
    case 0x5f4f: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x5f2b, 0x5f2e); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x5f2e, 0x5f30); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x5f30, 0x5f31); ret_effect(gb); return;
  }
  CYC(0x5f30, 0x5f31);
  CYC(0x5f31, 0x5f33); A = 0x29;
  CYC(0x5f33, 0x5f36); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x5f36, 0x5f38); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x5f38, 0x5f3b); mem_wr(gb, wLinkForceState, A);
  CYC(0x5f3b, 0x5f3d); A = 0x10;
  CYC(0x5f3d, 0x5f40); W8(w1Link_angle) = A;
  CYC(0x5f40, 0x5f43); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x5f43, 0x5f46); A = W8(w1Link_state);
  CYC(0x5f46, 0x5f48); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(0x5f48, 0x5f49); ret_effect(gb); return;
  }
  CYC(0x5f48, 0x5f49);
  CALL_C(0x5f49, setLinkForceStateToState08_hook, 0x2aad, 0x5f4c);
  CYC(0x5f4c, 0x5f4f); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x5f4f, 0x5f50); ret_effect(gb);
}

// Impa tells you that zelda's been kidnapped by Vire
void impaSubid7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f50, 0x5f52); C = 0x20;
  CALL_C(0x5f52, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5f55);
  CALL_C(0x5f55, interactionRunScript_hook, 0x2552, 0x5f58);
  if (F & FC) {
    CYCT(0x5f58, 0x5f5b); interactionDelete_hook(gb); return;
  }
  CYC(0x5f58, 0x5f5b);
  CYC(0x5f5b, 0x5f5d); A = 0x39; // GLOBALFLAG_IMPA_MOVED_AFTER_ZELDA_KIDNAPPED
  CALL_C(0x5f5d, checkGlobalFlag_hook, 0x31f3, 0x5f60);
  if (F & FZ) {
    CYCT(0x5f60, 0x5f63); interactionAnimateAsNpc_hook(gb); return;
  }
  CYC(0x5f60, 0x5f63);
  CYC(0x5f63, 0x5f65); A = 0x3c; // GLOBALFLAG_ZELDA_SAVED_FROM_VIRE
  CALL_C(0x5f65, checkGlobalFlag_hook, 0x31f3, 0x5f68);
  if (!(F & FZ)) {
    CYCT(0x5f68, 0x5f6b); interactionAnimate_hook(gb); return;
  }
  CYC(0x5f68, 0x5f6b);
  CYC(0x5f6b, 0x5f6e); npcFaceLinkAndAnimate_hook(gb);
}

void impaSubid8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f6e, impaAnimateAndRunScript_hook, 0x5df2, 0x5f71);
  if (F & FC) {
    CYCT(0x5f71, 0x5f74); interactionDelete_hook(gb); return;
  }
  CYC(0x5f71, 0x5f74);
  CYC(0x5f74, 0x5f75); ret_effect(gb);
}

// Impa tells you that Zelda's been kidnapped by Twinrova
void impaSubid9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f75, 0x5f77); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5f77, 0x5f78); A = mem_rd(gb, DE);
  CYC(0x5f78, 0x5f79); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5f79, 0x5f7b); goto animateAndRunScript;
  }
  CYC(0x5f79, 0x5f7b);
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(0x5f7b, 0x5f7e); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(0x5f7e, 0x5f80); E = 0x15;
  CALL_C(0x5f80, interBankCall_hook, 0x008a, 0x5f83);

animateAndRunScript:
  CYC(0x5f83, 0x5f86); impaAnimateAndRunScript_hook(gb);
}

// checkObjectIsCloseToPosition@checkComponent: reached by a static `call` from
// checkObjectIsCloseToPosition and by falling through from it.
// @param b     Position
// @param hl    Object position component
// @param hFF8B
// @param[out] cflag Set if we're within [hFF8B] pixels of 'b'.
static void impaInCutscene_checkComponent(GB *gb) {
  CYC(0x5f8f, 0x5f90); A = B;
  CYC(0x5f90, 0x5f91); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5f91, 0x5f94); SET_HL(hFF8B);
  CYC(0x5f94, 0x5f95); B = mem_rd(gb, HL);
  CYC(0x5f95, 0x5f96); alu_add(gb, B);
  CYC(0x5f96, 0x5f98); H8(hFF8D) = A;
  CYC(0x5f98, 0x5f99); A = B;
  CYC(0x5f99, 0x5f9a); alu_add(gb, A);
  CYC(0x5f9a, 0x5f9b); B = A;
  CYC(0x5f9b, 0x5f9c); B = alu_inc8(gb, B);
  CYC(0x5f9c, 0x5f9e); A = H8(hFF8D);
  CYC(0x5f9e, 0x5f9f); alu_cp(gb, B);
  CYC(0x5f9f, 0x5fa0); ret_effect(gb);
}

// Checks that an object is within [hFF8B] pixels of a position on both axes.
// @param bc    Target position
// @param hl    Object's Y position
// @param hFF8B Range we must be within on each axis
// @param[out] cflag c if the object is within [hFF8B] pixels of the position
void checkObjectIsCloseToPosition_b08_hook(GB *gb) {
  CYC(0x5f86, 0x5f87); push_effect(gb, HL);
  CYC(0x5f87, 0x5f8a); push_effect(gb, 0x5f8a); impaInCutscene_checkComponent(gb);
  CYC(0x5f8a, 0x5f8b); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x5f8b, 0x5f8c); ret_effect(gb); return;
  }
  CYC(0x5f8b, 0x5f8c);
  CYC(0x5f8c, 0x5f8d); L = alu_inc8(gb, L);
  CYC(0x5f8d, 0x5f8e); L = alu_inc8(gb, L);
  CYC(0x5f8e, 0x5f8f); B = C;
  impaInCutscene_checkComponent(gb);
}

void impaUpdateAnimationIfDirectionChanged_hook(GB *gb) {
  CYC(0x5fa0, 0x5fa1); H = D;
  CYC(0x5fa1, 0x5fa3); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x5fa3, 0x5fa4); A = mem_rd(gb, HL);
  CYC(0x5fa4, 0x5fa6); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5fa6, 0x5fa7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5fa7, 0x5fa8); ret_effect(gb); return;
  }
  CYC(0x5fa7, 0x5fa8);
  CYC(0x5fa8, 0x5fa9); mem_wr(gb, HL, A);
  CYC(0x5fa9, 0x5fac); interactionSetAnimation_hook(gb);
}

// @param[out] cflag c if Link has approached the stone to trigger Impa's reaction
void impaCheckApproachedStone_hook(GB *gb) {
  CYC(0x5fac, 0x5faf); A = mem_rd(gb, wActiveRoom);
  CYC(0x5faf, 0x5fb1); alu_cp(gb, 0x59);
  if (!(F & FZ)) {
    CYCT(0x5fb1, 0x5fb3); goto notClose;
  }
  CYC(0x5fb1, 0x5fb3);
  CYC(0x5fb3, 0x5fb6); A = mem_rd(gb, wScrollMode);
  CYC(0x5fb6, 0x5fb8); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x5fb8, 0x5fb9); ret_effect(gb); return;
  }
  CYC(0x5fb8, 0x5fb9);
  CYC(0x5fb9, 0x5fbc); SET_HL(w1Link_yh);
  CYC(0x5fbc, 0x5fbd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fbd, 0x5fbf); alu_cp(gb, 0x58);
  if (!(F & FC)) {
    CYCT(0x5fbf, 0x5fc1); goto notClose;
  }
  CYC(0x5fbf, 0x5fc1);
  CYC(0x5fc1, 0x5fc2); L = alu_inc8(gb, L);
  CYC(0x5fc2, 0x5fc3); A = mem_rd(gb, HL);
  CYC(0x5fc3, 0x5fc5); alu_cp(gb, 0x78);
  CYC(0x5fc5, 0x5fc6); ret_effect(gb);
  return;

notClose:
  CYC(0x5fc6, 0x5fc7); alu_xor(gb, A);
  CYC(0x5fc7, 0x5fc8); ret_effect(gb);
}

// @param[out] zflag z if counter1 has reached 0.
void impaAnimateAndDecCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5fc8, 0x5fc9); H = D;
  CYC(0x5fc9, 0x5fcb); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5fcb, 0x5fcc); A = mem_rd(gb, HL);
  CYC(0x5fcc, 0x5fcd); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5fcd, 0x5fce); ret_effect(gb); return;
  }
  CYC(0x5fcd, 0x5fce);
  CYC(0x5fce, 0x5fcf); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x5fcf, interactionAnimate_hook, 0x261b, 0x5fd2);
  CYC(0x5fd2, 0x5fd4); alu_or(gb, 0x01);
  CYC(0x5fd4, 0x5fd5); ret_effect(gb);
}

// Shows text if Link tries to leave the screen with the stone.
void impaPreventLinkFromLeavingStoneScreen_hook(GB *gb) {
  CYC(0x5fd5, 0x5fd8); SET_HL(w1Link_yh);
  CYC(0x5fd8, 0x5fd9); A = mem_rd(gb, HL);
  CYC(0x5fd9, 0x5fdb); B = 0x76;
  CYC(0x5fdb, 0x5fdc); alu_cp(gb, B);
  if (F & FC) {
    CYCT(0x5fdc, 0x5fde); goto checkX;
  }
  CYC(0x5fdc, 0x5fde);
  CYC(0x5fde, 0x5fe1); A = mem_rd(gb, wKeysPressed);
  CYC(0x5fe1, 0x5fe3); alu_and(gb, 0x80); // BTN_DOWN
  if (!(F & FZ)) {
    CYCT(0x5fe3, 0x5fe5); goto showText;
  }
  CYC(0x5fe3, 0x5fe5);

checkX:
  CYC(0x5fe5, 0x5fe7); L = (uint8_t)w1Link_xh;
  CYC(0x5fe7, 0x5fe8); A = mem_rd(gb, HL);
  CYC(0x5fe8, 0x5fea); B = 0x96;
  CYC(0x5fea, 0x5feb); alu_cp(gb, B);
  if (F & FC) {
    CYCT(0x5feb, 0x5fec); ret_effect(gb); return;
  }
  CYC(0x5feb, 0x5fec);
  CYC(0x5fec, 0x5fef); A = mem_rd(gb, wKeysPressed);
  CYC(0x5fef, 0x5ff1); alu_and(gb, 0x10); // BTN_RIGHT
  if (F & FZ) {
    CYCT(0x5ff1, 0x5ff2); ret_effect(gb); return;
  }
  CYC(0x5ff1, 0x5ff2);

showText:
  CYC(0x5ff2, 0x5ff3); mem_wr(gb, HL, B);
  CYC(0x5ff3, 0x5ff6); SET_BC(0x010a); // TX_010a
  CYC(0x5ff6, 0x5ff9); showText_hook(gb);
}
