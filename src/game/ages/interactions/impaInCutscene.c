#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// impaScriptTable: 10 script pointers into bank $0c, indexed by subid.
#define impaScriptTable_bank08 SYM(impaScriptTable)

// mainScripts.* (bank $0c), scriptHelp.* (bank $15) and objectData.* (bank $12), referenced
// by address only.
#define impaScript_moveAwayFromRock_bank0c SYM(impaScript_moveAwayFromRock)
#define impaScript_waitForRockToBeMoved_bank0c SYM(impaScript_waitForRockToBeMoved)
#define impaScript_rockJustMoved_bank0c SYM(impaScript_rockJustMoved_b0c)
#define impaScript_revealPossession_bank0c SYM(impaScript_revealPossession)
#define turnToFaceSomething_bank15 SYM(turnToFaceSomething)
#define objectWritePositionTocfd5_bank15 SYM(objectWritePositionTocfd5)
#define impaOctoroks_bank12 SYM(impaOctoroks)

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
  BASE(interactionCode31);
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  CYC(b_+102, b_+105); SET_HL(impaScriptTable_bank08);
  CYC(b_+105, b_+106); impaInCutscene_addDoubleIndex(gb, b_+106);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); interactionSetScript_hook(gb);
}

// interactionCode31@initSubid: per-subid initialization. Reached only by a static `call`
// from interactionCode31 state 0.
static void impaInCutscene_initSubid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode31);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+54) { goto init0; }
    else if (jt_ == b_+89) { goto init1; }
    else if (jt_ == b_+112) { goto init2; }
    else if (jt_ == b_+212) { goto init3; }
    else if (jt_ == b_+222) { goto init4; }
    else if (jt_ == b_+251) { goto init5; }
    else if (jt_ == b_+99) { impaInCutscene_loadScript(gb); return; }
    else if (jt_ == b_+120) { goto init7; }
    else if (jt_ == b_+275) { goto init8; }
    else if (jt_ == b_+269) { goto init9; }
    else if (jt_ == b_+264) { goto initA; }
    else { HANDOFF(HL); }
  } while (0);

init0:
  CALL_C(b_+54, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+57);
  CYC(b_+57, b_+59); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+59, b_+62); TAIL(interactionDelete);
  }
  CYC(b_+59, b_+62);
  // Load a custom palette and use it for possessed impa
  CYC(b_+62, b_+64); A = 0x97; // PALH_97
  CALL_C(b_+64, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+67);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+69, b_+71); A = 0x07;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+75); SET_HL(impaOctoroks_bank12);
  CALL_C(b_+75, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+78);
  CYC(b_+78, b_+80); A = 0x08; // LINK_STATE_08
  CALL_C(b_+80, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+83);
  CYC(b_+83, b_+85); L = (uint8_t)w1Link_subid;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x01);
  CYC(b_+87, b_+89); impaInCutscene_loadScript(gb);
  return;

init1:
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+95, b_+96); mem_wr(gb, HL, A);
  CALL_C(b_+96, impaLoadCollapsedGraphic_hook, SYM(impaLoadCollapsedGraphic), b_+99);
  impaInCutscene_loadScript(gb);
  return;

init2:
  CYC(b_+112, b_+113); H = D;
  CYC(b_+113, b_+115); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x1e);
  CYC(b_+117, b_+120); objectSetVisible82_hook(gb);
  return;

init7:
  // Delete self if Zelda hasn't been kidnapped by vire yet, or she's been rescued already,
  // or this isn't a linked game
  CYC(b_+120, b_+123); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+123, b_+125); alu_bit(gb, 2, A);
  if (F & FZ) {
    CYCT(b_+125, b_+128); TAIL(interactionDelete);
  }
  CYC(b_+125, b_+128);
  CALL_C(b_+128, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+131);
  if (F & FZ) {
    CYCT(b_+131, b_+134); TAIL(interactionDelete);
  }
  CYC(b_+131, b_+134);
  CYC(b_+134, b_+136); A = 0x38; // GLOBALFLAG_GOT_RING_FROM_ZELDA
  CALL_C(b_+136, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+139);
  if (!(F & FZ)) {
    CYCT(b_+139, b_+142); TAIL(interactionDelete);
  }
  CYC(b_+139, b_+142);
  CYC(b_+142, b_+144); A = 0x39; // GLOBALFLAG_IMPA_MOVED_AFTER_ZELDA_KIDNAPPED
  CALL_C(b_+144, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+147);
  CYC(b_+147, b_+149); A = 0x09;
  if (F & FZ) {
    CYCT(b_+149, b_+151); goto setAnimationAndLoadScript;
  }
  CYC(b_+149, b_+151);
  CYC(b_+151, b_+153); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+153, b_+155); A = 0x38;
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);
  CYC(b_+156, b_+158); A = 0x3c; // GLOBALFLAG_ZELDA_SAVED_FROM_VIRE
  CALL_C(b_+158, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+161);
  CYC(b_+161, b_+163); A = 0x02;
  if (F & FZ) {
    CYCT(b_+163, b_+165); goto setAnimationAndLoadScript;
  }
  CYC(b_+163, b_+165);
  CYC(b_+165, b_+167); A = 0x48;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  CYC(b_+168, b_+170); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+170, b_+172); A = 0x58;
  CYC(b_+172, b_+173); mem_wr(gb, DE, A);
  CYC(b_+173, b_+175); A = 0x81;
  CYC(b_+175, b_+178); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+178, b_+181); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+181, b_+183); A = 0x00;
  CYC(b_+183, b_+186); mem_wr(gb, wScrollMode, A);
  CYC(b_+186, b_+189); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+189, b_+191); B = 0x10;
  CALL_C(b_+191, clearMemory_hook, SYM(clearMemory), b_+194);
  CYC(b_+194, b_+197); SET_BC(0xad06); // INTERAC_ZELDA, $06
  CALL_C(b_+197, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+200);
  CYC(b_+200, b_+202); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+202, b_+204); mem_wr(gb, HL, 0x8c);
  CYC(b_+204, b_+206); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+206, b_+208); mem_wr(gb, HL, 0x50);
  CYC(b_+208, b_+210); A = 0x02;
  CYC(b_+210, b_+212); goto setAnimationAndLoadScript;

init3:
  CYC(b_+212, b_+214); A = 0x03;

setAnimationAndLoadScript:
  CALL_C(b_+214, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+217);
  CALL_C(b_+217, objectSetVisible82_hook, SYM(objectSetVisible82), b_+220);
  CYC(b_+220, b_+222); impaInCutscene_loadScript(gb);
  return;

init4:
  CALL_C(b_+222, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+225);
  if (!(F & FZ)) {
    CYCT(b_+225, b_+228); TAIL(interactionDelete);
  }
  CYC(b_+225, b_+228);
  CYC(b_+228, b_+229); alu_xor(gb, A);
  CYC(b_+229, b_+232); mem_wr(gb, wTmpcfc0, A);

preBlackTowerCutscene:
  CYC(b_+232, b_+234); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+234, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+237);
  if (!(F & FC)) {
    CYCT(b_+237, b_+240); TAIL(interactionDelete);
  }
  CYC(b_+237, b_+240);
  CYC(b_+240, b_+242); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+242, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+245);
  if (!(F & FZ)) {
    CYCT(b_+245, b_+248); TAIL(interactionDelete);
  }
  CYC(b_+245, b_+248);
  CYC(b_+248, b_+251); impaInCutscene_loadScript(gb);
  return;

init5:
  CALL_C(b_+251, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+254);
  if (F & FZ) {
    CYCT(b_+254, b_+257); TAIL(interactionDelete);
  }
  CYC(b_+254, b_+257);
  CYC(b_+257, b_+259); A = 0x03;
  CALL_C(b_+259, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+262);
  CYC(b_+262, b_+264); goto preBlackTowerCutscene;

initA:
  CYC(b_+264, b_+266); A = 0x02;
  CYC(b_+266, b_+269); interactionSetAnimation_hook(gb);
  return;

init9:
  CALL_C(b_+269, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+272);
  if (F & FZ) {
    CYCT(b_+272, b_+275); TAIL(interactionDelete);
  }
  CYC(b_+272, b_+275);

init8:
  CYC(b_+275, b_+277); A = 0x03;
  CALL_C(b_+277, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+280);
  CYC(b_+280, b_+283); push_effect(gb, SYM(impaState1)); impaInCutscene_loadScript(gb);
  // No ret: @init8 falls through into impaState1.
  impaState1_hook(gb);
}

// INTERAC_IMPA_IN_CUTSCENE
//
// Variables:
//   var3b: For subid 1, saves impa's "oamTileIndexBase" so it can be restored after Impa
//          gets up (she references a different sprite sheet for her "collapsed" sprite)
void interactionCode31_hook(GB *gb) {
  BASE(interactionCode31);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == SYM(impaState1) && hook_enabled_at(gb, SYM(impaState1))) { impaState1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); impaInCutscene_initSubid(gb, sp0_);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); TAIL(objectMarkSolidPosition);
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void impaState1_hook(GB *gb) {
  BASE(impaState1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == SYM(impaSubid0) && hook_enabled_at(gb, SYM(impaSubid0))) { impaSubid0_hook(gb); return; }
    else if (jt_ == SYM(impaSubid1) && hook_enabled_at(gb, SYM(impaSubid1))) { impaSubid1_hook(gb); return; }
    else if (jt_ == SYM(impaSubid2) && hook_enabled_at(gb, SYM(impaSubid2))) { impaSubid2_hook(gb); return; }
    else if (jt_ == SYM(impaAnimateAndRunScript) && hook_enabled_at(gb, SYM(impaAnimateAndRunScript))) { impaAnimateAndRunScript_hook(gb); return; } // subids 3 and 6
    else if (jt_ == SYM(impaSubid4) && hook_enabled_at(gb, SYM(impaSubid4))) { impaSubid4_hook(gb); return; }
    else if (jt_ == SYM(impaSubid5) && hook_enabled_at(gb, SYM(impaSubid5))) { impaSubid5_hook(gb); return; }
    else if (jt_ == SYM(impaSubid7) && hook_enabled_at(gb, SYM(impaSubid7))) { impaSubid7_hook(gb); return; }
    else if (jt_ == SYM(impaSubid8) && hook_enabled_at(gb, SYM(impaSubid8))) { impaSubid8_hook(gb); return; }
    else if (jt_ == SYM(impaSubid9) && hook_enabled_at(gb, SYM(impaSubid9))) { impaSubid9_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// impaSubid0@setAngleTowardStone: reached only by static `call`s from impaSubid0 itself.
static void impaInCutscene_setAngleTowardStone(GB *gb, uint16_t sp0_) {
  BASE(impaSubid0);
  CYC(b_+212, b_+215); SET_BC(0x3838);
  CALL_C(b_+215, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+218);
  CYC(b_+218, b_+220); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+220, b_+221); mem_wr(gb, DE, A);
  CYC(b_+221, b_+222); ret_effect(gb);
}

// Possessed Impa.
//
// Variables:
//   var37-var3a: Last frame's Y, X, and Direction values. Used for checking whether to
//                update Impa's animation (update if any one has changed).
void impaSubid0_hook(GB *gb) {
  BASE(impaSubid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x0e);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7); goto dispatch;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wActiveMusic);
  CYC(b_+10, b_+12); A = 0x0f; // MUS_FAIRY_FOUNTAIN
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+13, b_+15); goto dispatch;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, wActiveRoom);
  CYC(b_+18, b_+20); alu_cp(gb, 0x39);
  if (F & FZ) {
    CYCT(b_+20, b_+22); goto dispatch;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); alu_cp(gb, 0x49);
  if (F & FZ) {
    CYCT(b_+24, b_+26); goto dispatch;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0x0f; // MUS_FAIRY_FOUNTAIN
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CALL_C(b_+29, playSound_b00_hook, SYM(playSound_b00), b_+32);
  CYC(b_+32, b_+34); A = 0x03;
  CALL_C(b_+34, setMusicVolume_hook, SYM(setMusicVolume), b_+37);
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_SUBSTATE;

dispatch:
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); push_effect(gb, b_+41);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+75) { goto substate0; }
    else if (jt_ == b_+129) { goto substate1; }
    else if (jt_ == b_+222) { goto substate2; }
    else if (jt_ == b_+240) { goto substate3; }
    else if (jt_ == b_+255) { goto substate4; }
    else if (jt_ == b_+266) { goto substate5; }
    else if (jt_ == b_+308) { goto substate6; }
    else if (jt_ == b_+323) { goto substate7; }
    else if (jt_ == b_+341) { goto substate8; }
    else if (jt_ == b_+356) { goto substate9; }
    else if (jt_ == b_+374) { goto substateA; }
    else if (jt_ == b_+395) { goto substateB; }
    else if (jt_ == b_+419) { goto substateC; }
    else if (jt_ == b_+434) { goto substateD; }
    else if (jt_ == b_+463) { goto substateE; }
    else if (jt_ == b_+492) { goto substateF; }
    else if (jt_ == SYM(impaRet) && hook_enabled_at(gb, SYM(impaRet))) { impaRet_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // Running a script until Impa joins Link
  CALL_C(b_+75, impaAnimateAndRunScript_hook, SYM(impaAnimateAndRunScript), b_+78);
  if (!(F & FC)) {
    CYCT(b_+78, b_+79); ret_effect(gb); return;
  }
  CYC(b_+78, b_+79);
  // When the script has finished, make Impa follow Link and go to substate 1
  CYC(b_+79, b_+80); alu_xor(gb, A);
  CYC(b_+80, b_+83); mem_wr(gb, wUseSimulatedInput, A);
  CALL_C(b_+83, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+86);
  CYC(b_+86, b_+88); L = (uint8_t)w1Link_direction;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x00); // DIR_UP

beginFollowingLink:
  CALL_C(b_+90, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+93);
  CALL_C(b_+93, makeActiveObjectFollowLink_b00_hook, SYM(makeActiveObjectFollowLink_b00), b_+96);
  CALL_C(b_+96, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+99);
  CALL_C(b_+99, objectSetReservedBit1_hook, SYM(objectSetReservedBit1), b_+102);
  CYC(b_+102, b_+104); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+104, b_+106); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+108, b_+110); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+112, b_+114); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+114, b_+117); A = W8(w1Link_direction);
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0x00);
  CALL_C(b_+120, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+123);
  CALL_C(b_+123, objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+126);
  CYC(b_+126, b_+129); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
  return;

substate1:
  // Impa following Link (before stone is pushed)
  CALL_C(b_+129, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+132);
  CALL_C(b_+132, impaCheckApproachedStone_hook, SYM(impaCheckApproachedStone), b_+135);
  if (!(F & FC)) {
    CYCT(b_+135, b_+137); goto updateAnimationWhileFollowingLink;
  }
  CYC(b_+135, b_+137);
  // Link has approached the stone; trigger cutscene.
  CYC(b_+137, b_+139); A = 0x08; // LINK_STATE_08
  CALL_C(b_+139, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+142);
  CYC(b_+142, b_+144); L = (uint8_t)w1Link_subid;
  CYC(b_+144, b_+146); mem_wr(gb, HL, 0x02);
  CALL_C(b_+146, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+149);
  CYC(b_+149, b_+151); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+151, b_+153); mem_wr(gb, HL, 0x1e);
  CYC(b_+153, b_+155); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+155, b_+157); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+157, b_+159); A = 0x50; // SND_CLINK
  CALL_C(b_+159, playSound_b00_hook, SYM(playSound_b00), b_+162);
  CYC(b_+162, b_+165); SET_BC(0xfe40); // -$1c0
  CALL_C(b_+165, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+168);
  CALL_C(b_+168, clearFollowingLinkObject_hook, SYM(clearFollowingLinkObject), b_+171);
  CYC(b_+171, b_+174); push_effect(gb, b_+174); impaInCutscene_setAngleTowardStone(gb, sp0_);
  CALL_C(b_+174, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+177);
  CYC(b_+177, b_+180); interactionSetAnimation_hook(gb);
  return;

updateAnimationWhileFollowingLink:
  // Nothing to do here except check whether to update the animation. (It must update if
  // her position or direction has changed.)
  CALL_C(b_+180, impaUpdateAnimationIfDirectionChanged_hook, SYM(impaUpdateAnimationIfDirectionChanged), b_+183);
  CYC(b_+183, b_+184); H = D;
  CYC(b_+184, b_+186); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+186, b_+187); A = mem_rd(gb, HL);
  CYC(b_+187, b_+188); B = A;
  CYC(b_+188, b_+190); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+190, b_+191); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+191, b_+193); goto positionChanged;
  }
  CYC(b_+191, b_+193);
  CYC(b_+193, b_+195); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+195, b_+196); A = mem_rd(gb, HL);
  CYC(b_+196, b_+197); C = A;
  CYC(b_+197, b_+199); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+199, b_+200); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+200, b_+201); ret_effect(gb); return;
  }
  CYC(b_+200, b_+201);

positionChanged:
  CYC(b_+201, b_+203); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+203, b_+204); mem_wr(gb, HL, B);
  CYC(b_+204, b_+205); L = alu_inc8(gb, L);
  CYC(b_+205, b_+206); mem_wr(gb, HL, C);
  CALL_C(b_+206, interactionAnimate_hook, SYM(interactionAnimate), b_+209);
  CYC(b_+209, b_+212); interactionAnimate_hook(gb);
  return;

substate2:
  // Jumping after spotting stone
  CALL_C(b_+222, impaAnimateAndDecCounter1_hook, SYM(impaAnimateAndDecCounter1), b_+225);
  if (!(F & FZ)) {
    CYCT(b_+225, b_+226); ret_effect(gb); return;
  }
  CYC(b_+225, b_+226);
  // Wait until she lands
  CYC(b_+226, b_+228); C = 0x20;
  CALL_C(b_+228, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+231);
  if (!(F & FZ)) {
    CYCT(b_+231, b_+232); ret_effect(gb); return;
  }
  CYC(b_+231, b_+232);
  CALL_C(b_+232, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+235);
  CYC(b_+235, b_+237); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0a);
  CYC(b_+239, b_+240); ret_effect(gb);
  return;

substate3:
  CALL_C(b_+240, impaAnimateAndDecCounter1_hook, SYM(impaAnimateAndDecCounter1), b_+243);
  if (!(F & FZ)) {
    CYCT(b_+243, b_+244); ret_effect(gb); return;
  }
  CYC(b_+243, b_+244);
  CYC(b_+244, b_+246); mem_wr(gb, HL, 0x14);
  CYC(b_+246, b_+249); SET_BC(0x0104); // TX_0104
  CALL_C(b_+249, showText_hook, SYM(showText), b_+252);
  CYC(b_+252, b_+255); interactionIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(b_+255, interactionDecCounter1IfTextNotActive_hook, SYM(interactionDecCounter1IfTextNotActive), b_+258);
  if (!(F & FZ)) {
    CYCT(b_+258, b_+259); ret_effect(gb); return;
  }
  CYC(b_+258, b_+259);
  CYC(b_+259, b_+261); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+261, b_+263); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(b_+263, b_+266); interactionIncSubstate_hook(gb);
  return;

substate5:
  // Moving toward stone
  CALL_C(b_+266, interactionAnimate3Times_hook, SYM(interactionAnimate3Times), b_+269);
  CALL_C(b_+269, objectApplySpeed_hook, SYM(objectApplySpeed), b_+272);
  CYC(b_+272, b_+275); push_effect(gb, b_+275); impaInCutscene_setAngleTowardStone(gb, sp0_);
  CYC(b_+275, b_+277); A = 0x02;
  CYC(b_+277, b_+279); H8(hFF8B) = A;
  CYC(b_+279, b_+282); SET_BC(0x3838);
  CYC(b_+282, b_+283); H = D;
  CYC(b_+283, b_+285); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+285, checkObjectIsCloseToPosition_b08_hook, SYM(checkObjectIsCloseToPosition_b08), b_+288);
  if (!(F & FC)) {
    CYCT(b_+288, b_+289); ret_effect(gb); return;
  }
  CYC(b_+288, b_+289);
  // Reached the stone
  CYC(b_+289, b_+290); H = D;
  CALL_C(b_+290, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+293);
  CYC(b_+293, b_+295); A = 0x38;
  CYC(b_+295, b_+297); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+297, b_+298); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+298, b_+299); L = alu_inc8(gb, L);
  CYC(b_+299, b_+300); mem_wr(gb, HL, A);
  CYC(b_+300, b_+302); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+302, b_+304); mem_wr(gb, HL, 0x1e);
  CYC(b_+304, b_+305); alu_xor(gb, A);
  CYC(b_+305, b_+308); interactionSetAnimation_hook(gb);
  return;

substate6:
  CALL_C(b_+308, impaAnimateAndDecCounter1_hook, SYM(impaAnimateAndDecCounter1), b_+311);
  if (!(F & FZ)) {
    CYCT(b_+311, b_+312); ret_effect(gb); return;
  }
  CYC(b_+311, b_+312);
  // Start a jump
  CYC(b_+312, b_+314); mem_wr(gb, HL, 0x1e);
  CYC(b_+314, b_+317); SET_BC(0xfe80); // -$180
  CALL_C(b_+317, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+320);
  CYC(b_+320, b_+323); interactionIncSubstate_hook(gb);
  return;

substate7:
  // Jumping in front of stone
  CALL_C(b_+323, impaAnimateAndDecCounter1_hook, SYM(impaAnimateAndDecCounter1), b_+326);
  if (!(F & FZ)) {
    CYCT(b_+326, b_+327); ret_effect(gb); return;
  }
  CYC(b_+326, b_+327);
  CYC(b_+327, b_+329); C = 0x20;
  CALL_C(b_+329, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+332);
  if (!(F & FZ)) {
    CYCT(b_+332, b_+333); ret_effect(gb); return;
  }
  CYC(b_+332, b_+333);
  CALL_C(b_+333, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+336);
  CYC(b_+336, b_+338); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+338, b_+340); mem_wr(gb, HL, 0x0a);
  CYC(b_+340, b_+341); ret_effect(gb);
  return;

substate8:
  CALL_C(b_+341, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+344);
  if (!(F & FZ)) {
    CYCT(b_+344, b_+345); ret_effect(gb); return;
  }
  CYC(b_+344, b_+345);
  CYC(b_+345, b_+347); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+347, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+350);
  CYC(b_+350, b_+353); SET_BC(0x0105); // TX_0105
  CYC(b_+353, b_+356); showText_hook(gb);
  return;

substate9:
  CALL_C(b_+356, interactionDecCounter1IfTextNotActive_hook, SYM(interactionDecCounter1IfTextNotActive), b_+359);
  if (!(F & FZ)) {
    CYCT(b_+359, b_+360); ret_effect(gb); return;
  }
  CYC(b_+359, b_+360);
  CYC(b_+360, b_+363); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+363, b_+365); mem_wr(gb, HL, 0x02);
  CYC(b_+365, b_+368); SET_HL(impaScript_moveAwayFromRock_bank0c);
  CALL_C(b_+368, interactionSetScript_hook, SYM(interactionSetScript), b_+371);
  CYC(b_+371, b_+374); interactionIncSubstate_hook(gb);
  return;

substateA:
  // Moving away from rock (the previously loaded script handles this)
  CALL_C(b_+374, impaAnimateAndRunScript_hook, SYM(impaAnimateAndRunScript), b_+377);
  if (!(F & FC)) {
    CYCT(b_+377, b_+378); ret_effect(gb); return;
  }
  CYC(b_+377, b_+378);
  // Done moving away; return control to Link
  CYC(b_+378, b_+379); alu_xor(gb, A);
  CALL_C(b_+379, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+382);
  CYC(b_+382, b_+384); L = (uint8_t)w1Link_direction;
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(b_+386, b_+389); SET_HL(impaScript_waitForRockToBeMoved_bank0c);
  CALL_C(b_+389, interactionSetScript_hook, SYM(interactionSetScript), b_+392);
  CYC(b_+392, b_+395); interactionIncSubstate_hook(gb);
  return;

substateB:
  // Waiting for Link to start pushing the rock
  CALL_C(b_+395, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+398);
  CALL_C(b_+398, interactionRunScript_hook, SYM(interactionRunScript), b_+401);
  CALL_C(b_+401, impaPreventLinkFromLeavingStoneScreen_hook, SYM(impaPreventLinkFromLeavingStoneScreen), b_+404);
  CYC(b_+404, b_+407); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+407, b_+409); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+409, b_+410); ret_effect(gb); return;
  }
  CYC(b_+409, b_+410);
  // The rock has started moving.
  CYC(b_+410, b_+413); SET_HL(impaScript_rockJustMoved_bank0c);
  CALL_C(b_+413, interactionSetScript_hook, SYM(interactionSetScript), b_+416);
  CYC(b_+416, b_+419); interactionIncSubstate_hook(gb);
  return;

substateC:
  CALL_C(b_+419, impaAnimateAndRunScript_hook, SYM(impaAnimateAndRunScript), b_+422);
  if (!(F & FC)) {
    CYCT(b_+422, b_+423); ret_effect(gb); return;
  }
  CYC(b_+422, b_+423);
  CYC(b_+423, b_+424); alu_xor(gb, A);
  CALL_C(b_+424, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+427);
  CYC(b_+427, b_+429); L = (uint8_t)w1Link_direction;
  CYC(b_+429, b_+431); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(b_+431, b_+434); goto beginFollowingLink;

substateD:
  // Following Link, waiting for signal to begin the part of the cutscene where she reveals
  // she's evil
  CALL_C(b_+434, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+437);
  CYC(b_+437, b_+440); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+440, b_+442); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(b_+442, b_+445); goto updateAnimationWhileFollowingLink;
  }
  CYC(b_+442, b_+445);
  // Start the next part of the cutscene
  CALL_C(b_+445, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+448);
  CALL_C(b_+448, clearFollowingLinkObject_hook, SYM(clearFollowingLinkObject), b_+451);
  CYC(b_+451, b_+454); SET_BC((SYM(childUpdateCuriousMovement__substate1) + 24));
  CALL_C(b_+454, interactionSetPosition_hook, SYM(interactionSetPosition), b_+457);
  CYC(b_+457, b_+460); SET_HL(impaScript_revealPossession_bank0c);
  CYC(b_+460, b_+463); interactionSetScript_hook(gb);
  return;

substateE:
  CALL_C(b_+463, impaAnimateAndRunScript_hook, SYM(impaAnimateAndRunScript), b_+466);
  if (!(F & FC)) {
    CYCT(b_+466, b_+467); ret_effect(gb); return;
  }
  CYC(b_+466, b_+467);
  // Impa has just moved into the corner, Veran will now come out.
  CALL_C(b_+467, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+470);
  CYC(b_+470, b_+472); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+472, b_+474); mem_wr(gb, HL, 0x02);
  CYC(b_+474, b_+476); A = 0x05;
  CALL_C(b_+476, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+479);
  CYC(b_+479, b_+481); B = 0x3e; // INTERAC_GHOST_VERAN
  CALL_C(b_+481, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+484);
  CYC(b_+484, b_+486); A = 0x67; // SND_BOSS_DEAD
  CALL_C(b_+486, playSound_b00_hook, SYM(playSound_b00), b_+489);
  CYC(b_+489, b_+492); objectSetVisiblec2_hook(gb);
  return;

substateF:
  CALL_C(b_+492, interactionAnimate_hook, SYM(interactionAnimate), b_+495);
  CYC(b_+495, b_+496); H = D;
  CYC(b_+496, b_+498); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+498, b_+499); A = mem_rd(gb, HL);
  CYC(b_+499, b_+500); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+500, b_+501); ret_effect(gb); return;
  }
  CYC(b_+500, b_+501);
  CALL_C(b_+501, interactionIncSubstate_hook, SYM(interactionIncSubstate), SYM(impaLoadCollapsedGraphic));
  // Falls through into impaLoadCollapsedGraphic.
  impaLoadCollapsedGraphic_hook(gb);
}

// Changes impa's "oamTileIndexBase" to reference her "collapsed" graphic, which is not in
// her normal sprite sheet. Falls through into impaRet.
void impaLoadCollapsedGraphic_hook(GB *gb) {
  BASE(impaLoadCollapsedGraphic);
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x0a);
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x60);
  impaRet_hook(gb);
}

void impaRet_hook(GB *gb) {
  BASE(impaRet);
  CYC(b_+0, b_+1); ret_effect(gb);
}

// Impa talking to you after Nayru is kidnapped
void impaSubid1_hook(GB *gb) {
  BASE(impaSubid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+32) { goto substate1; }
    else if (jt_ == SYM(impaSubid1Substate2) && hook_enabled_at(gb, SYM(impaSubid1Substate2))) { impaSubid1Substate2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+10, b_+13); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+13, b_+15); alu_cp(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+18); TAIL(interactionAnimate);
  }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+21);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x3c);
  CYC(b_+31, b_+32); ret_effect(gb);
  return;

substate1:
  CALL_C(b_+32, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+35);
  if (!(F & FZ)) {
    CYCT(b_+35, b_+37); TAIL(interactionOscillateXRandomly);
  }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+40); interactionIncSubstate_hook(gb);
}

// Uses var3d as the interaction's "base" position, and randomly shifts this position left
// by one or not at all.
void interactionOscillateXRandomly_hook(GB *gb) {
  BASE(interactionOscillateXRandomly);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  CYC(b_+5, b_+7); alu_sub(gb, 0x01);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+10, b_+11); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void impaSubid1Substate2_hook(GB *gb) {
  BASE(impaSubid1Substate2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+13); TAIL(interactionAnimate2Times);
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); interactionAnimate_hook(gb);
}

// Impa in the credits cutscene
void impaSubid2_hook(GB *gb) {
  BASE(impaSubid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+20) { goto substate0; }
    else if (jt_ == b_+37) { goto substate1; }
    else if (jt_ == b_+53) { goto substate2; }
    else if (jt_ == SYM(impaAnimateAndRunScript) && hook_enabled_at(gb, SYM(impaAnimateAndRunScript))) { impaAnimateAndRunScript_hook(gb); return; }
    else if (jt_ == SYM(impaSubid2Substate4) && hook_enabled_at(gb, SYM(impaSubid2Substate4))) { impaSubid2Substate4_hook(gb); return; }
    else if (jt_ == SYM(impaSubid2Substate5) && hook_enabled_at(gb, SYM(impaSubid2Substate5))) { impaSubid2Substate5_hook(gb); return; }
    else if (jt_ == SYM(impaSubid2Substate6) && hook_enabled_at(gb, SYM(impaSubid2Substate6))) { impaSubid2Substate6_hook(gb); return; }
    else if (jt_ == SYM(impaSubid2Substate7) && hook_enabled_at(gb, SYM(impaSubid2Substate7))) { impaSubid2Substate7_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+20, interactionDecCounter1IfPaletteNotFading_hook, SYM(interactionDecCounter1IfPaletteNotFading), b_+23);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+24); ret_effect(gb); return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+26, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+29);
  CYC(b_+29, b_+31); A = 0x50;
  CYC(b_+31, b_+34); SET_BC((SYM(interactionCode32__init2) + 6));
  CYC(b_+34, b_+37); createEnergySwirlGoingIn_hook(gb);
  return;

substate1:
  CALL_C(b_+37, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+40);
  if (!(F & FZ)) {
    CYCT(b_+40, b_+41); ret_effect(gb); return;
  }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+44); SET_HL(wTmpcbb3);
  CYC(b_+44, b_+45); alu_xor(gb, A);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+47); A = alu_dec8(gb, A);
  CYC(b_+47, b_+50); mem_wr(gb, wTmpcbba, A);
  CYC(b_+50, b_+53); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+53, b_+56); SET_HL(wTmpcbb3);
  CYC(b_+56, b_+58); B = 0x02;
  CALL_C(b_+58, flashScreen_hook, SYM(flashScreen), b_+61);
  if (F & FZ) {
    CYCT(b_+61, b_+62); ret_effect(gb); return;
  }
  CYC(b_+61, b_+62);
  CALL_C(b_+62, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+65);
  CYC(b_+65, b_+68); push_effect(gb, b_+68); impaInCutscene_loadScript(gb);
  CYC(b_+68, b_+70); A = 0x01;
  CYC(b_+70, b_+73); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+73, b_+76); fadeinFromWhite_hook(gb);
}

void impaAnimateAndRunScript_hook(GB *gb) {
  BASE(impaAnimateAndRunScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+6); interactionRunScript_hook(gb);
}

// Falls through into impaSetVisibleAndJump.
void impaSubid2Substate4_hook(GB *gb) {
  BASE(impaSubid2Substate4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x02);
  impaSetVisibleAndJump_hook(gb);
}

void impaSetVisibleAndJump_hook(GB *gb) {
  BASE(impaSetVisibleAndJump);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+3);
  CYC(b_+3, b_+6); SET_BC(0xfe80); // -$180
  CYC(b_+6, b_+9); objectSetSpeedZ_hook(gb);
}

void impaSubid2Substate5_hook(GB *gb) {
  BASE(impaSubid2Substate5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+11); TAIL(impaSetVisibleAndJump);
  }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, objectSetVisible82_hook, SYM(objectSetVisible82), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x10);
  CYC(b_+19, b_+22); interactionIncSubstate_hook(gb);
}

void impaSubid2Substate6_hook(GB *gb) {
  BASE(impaSubid2Substate6);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x10);
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+13, b_+15); goto nextState;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); alu_or(gb, A);
  CYC(b_+16, b_+18); A = 0x03;
  if (F & FZ) {
    CYCT(b_+18, b_+20); goto setAnimation;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_xor(gb, 0x02);

setAnimation:
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+29); interactionSetAnimation_hook(gb);
  return;

nextState:
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x00);
  CYC(b_+31, b_+33); A = 0x02;
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+36, b_+39); interactionIncSubstate_hook(gb);
}

void impaSubid2Substate7_hook(GB *gb) {
  BASE(impaSubid2Substate7);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, impaAnimateAndRunScript_hook, SYM(impaAnimateAndRunScript), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03);
  if (F & FC) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  // jpab scriptHelp.turnToFaceSomething
  CYC(b_+9, b_+12); SET_HL(turnToFaceSomething_bank15);
  CYC(b_+12, b_+14); E = 0x15;
  CYC(b_+14, b_+17); interBankCall_hook(gb);
}

// Impa tells you about Ralph's heritage (unlinked)
void impaSubid4_hook(GB *gb) {
  BASE(impaSubid4);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto substate1;
  }
  CYC(b_+3, b_+5);

  // @substate0: wait for Link to move a certain distance down
  CYC(b_+5, b_+8); SET_HL(w1Link_yh);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); L = (uint8_t)w1Link_zh;
  CYC(b_+14, b_+16); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  CALL_C(b_+17, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+20);
  if (!(F & FC)) {
    CYCT(b_+20, b_+21); ret_effect(gb); return;
  }
  CYC(b_+20, b_+21);
  CALL_C(b_+21, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+24);
  CALL_C(b_+24, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+27);
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
  CYC(b_+28, b_+31); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+31, b_+34); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+34, b_+37); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+37, b_+39); C = 0x20;
  CALL_C(b_+39, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+42);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+43); ret_effect(gb); return;
  }
  CYC(b_+42, b_+43);
  CALL_C(b_+43, interactionRunScript_hook, SYM(interactionRunScript), b_+46);
  if (F & FC) {
    CYCT(b_+46, b_+49); TAIL(interactionDelete);
  }
  CYC(b_+46, b_+49);
  CALL_C(b_+49, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+52);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); push_effect(gb, b_+56);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+66) { goto thing0; }
    else if (jt_ == b_+78) { goto thing1; }
    else if (jt_ == b_+128) { goto thing2; }
    else if (jt_ == b_+156) { goto thing3; }
    else if (jt_ == b_+168) { goto thing4; }
    else { HANDOFF(HL); }
  } while (0);

thing0:
  CYC(b_+66, b_+69); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+69, b_+70); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+70, b_+71); ret_effect(gb); return;
  }
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+73); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+73, b_+75); A = 0x10;
  CYC(b_+75, b_+76); mem_wr(gb, DE, A);
  CYC(b_+76, b_+78); goto incVar38;

thing1:
  // Move Link horizontally toward Impa
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+81); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+81, b_+82); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+82, b_+83); ret_effect(gb); return;
  }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+86); A = W8(w1Link_xh);
  CYC(b_+86, b_+88); alu_sub(gb, 0x50);
  CYC(b_+88, b_+89); B = A;
  CYC(b_+89, b_+91); alu_add(gb, 0x02);
  CYC(b_+91, b_+93); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(b_+93, b_+95); goto incVar38;
  }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); A = B;
  CYC(b_+96, b_+98); alu_bit(gb, 7, A);
  CYC(b_+98, b_+100); B = 0x18;
  if (F & FZ) {
    CYCT(b_+100, b_+102); goto forceMovement;
  }
  CYC(b_+100, b_+102);
  CYC(b_+102, b_+104); B = 0x08;
  CYC(b_+104, b_+105); alu_cpl(gb);
  CYC(b_+105, b_+106); A = alu_inc8(gb, A);

forceMovement:
  CYC(b_+106, b_+109); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+109, b_+111); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+111, b_+114); mem_wr(gb, wLinkForceState, A);
  CYC(b_+114, b_+117); SET_HL(w1Link_angle);
  CYC(b_+117, b_+118); A = B;
  CYC(b_+118, b_+119); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+119, b_+121); A = alu_swap(gb, A);
  CYC(b_+121, b_+122); alu_rlca(gb);
  CYC(b_+122, b_+123); mem_wr(gb, HL, A);

incVar38:
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+126, b_+127); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+127, b_+128); ret_effect(gb);
  return;

thing2:
  // Move Link vertically toward Impa
  CYC(b_+128, b_+131); A = W8(w1Link_state);
  CYC(b_+131, b_+133); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(b_+133, b_+134); ret_effect(gb); return;
  }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+137); A = W8(w1Link_yh);
  CYC(b_+137, b_+139); alu_sub(gb, 0x48);
  CYC(b_+139, b_+142); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+146); SET_HL(w1Link_direction);
  CYC(b_+146, b_+147); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+147, b_+148); mem_wr(gb, HL, A);
  CYC(b_+148, b_+150); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+150, b_+153); mem_wr(gb, wLinkForceState, A);
  CYC(b_+153, b_+156); goto incVar38;

thing3:
  CYC(b_+156, b_+159); A = W8(w1Link_state);
  CYC(b_+159, b_+161); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(b_+161, b_+162); ret_effect(gb); return;
  }
  CYC(b_+161, b_+162);
  CALL_C(b_+162, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+165);
  CYC(b_+165, b_+168); goto incVar38;

thing4:
  CYC(b_+168, b_+169); ret_effect(gb);
}

// Like above (explaining ralph's heritage), but for linked game
void impaSubid5_hook(GB *gb) {
  BASE(impaSubid5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, interactionRunScript_hook, SYM(interactionRunScript), b_+9);
  if (!(F & FC)) {
    CYCT(b_+9, b_+11); goto scriptRunning;
  }
  CYC(b_+9, b_+11);
  // Script over
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+15, b_+18); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+18, b_+20); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+20, setGlobalFlag_hook, SYM(setGlobalFlag), b_+23);
  CYC(b_+23, b_+26); interactionDelete_hook(gb);
  return;

scriptRunning:
  CALL_C(b_+26, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+29);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); push_effect(gb, b_+33);
  do { uint16_t jt_ = (impaInCutscene_jumpTable(gb));
    if (jt_ == b_+39) { goto substate0; }
    else if (jt_ == b_+63) { goto substate1; }
    else if (jt_ == b_+75) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+39, b_+42); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+42, b_+44); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+45); ret_effect(gb); return;
  }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); A = 0x29;
  CYC(b_+47, b_+50); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+50, b_+52); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+52, b_+55); mem_wr(gb, wLinkForceState, A);
  CYC(b_+55, b_+57); A = 0x10;
  CYC(b_+57, b_+60); W8(w1Link_angle) = A;
  CYC(b_+60, b_+63); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+63, b_+66); A = W8(w1Link_state);
  CYC(b_+66, b_+68); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(b_+68, b_+69); ret_effect(gb); return;
  }
  CYC(b_+68, b_+69);
  CALL_C(b_+69, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+72);
  CYC(b_+72, b_+75); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+75, b_+76); ret_effect(gb);
}

// Impa tells you that zelda's been kidnapped by Vire
void impaSubid7_hook(GB *gb) {
  BASE(impaSubid7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  if (F & FC) {
    CYCT(b_+8, b_+11); TAIL(interactionDelete);
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); A = 0x39; // GLOBALFLAG_IMPA_MOVED_AFTER_ZELDA_KIDNAPPED
  CALL_C(b_+13, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+16);
  if (F & FZ) {
    CYCT(b_+16, b_+19); TAIL(interactionAnimateAsNpc);
  }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); A = 0x3c; // GLOBALFLAG_ZELDA_SAVED_FROM_VIRE
  CALL_C(b_+21, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+24);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); TAIL(interactionAnimate);
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+30); npcFaceLinkAndAnimate_hook(gb);
}

void impaSubid8_hook(GB *gb) {
  BASE(impaSubid8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, impaAnimateAndRunScript_hook, SYM(impaAnimateAndRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); ret_effect(gb);
}

// Impa tells you that Zelda's been kidnapped by Twinrova
void impaSubid9_hook(GB *gb) {
  BASE(impaSubid9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6); goto animateAndRunScript;
  }
  CYC(b_+4, b_+6);
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(b_+6, b_+9); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(b_+9, b_+11); E = 0x15;
  CALL_C(b_+11, interBankCall_hook, 0x008a, b_+14);

animateAndRunScript:
  CYC(b_+14, b_+17); impaAnimateAndRunScript_hook(gb);
}

// checkObjectIsCloseToPosition@checkComponent: reached by a static `call` from
// checkObjectIsCloseToPosition and by falling through from it.
// @param b     Position
// @param hl    Object position component
// @param hFF8B
// @param[out] cflag Set if we're within [hFF8B] pixels of 'b'.
static void impaInCutscene_checkComponent(GB *gb) {
  BASE(checkObjectIsCloseToPosition_b08);
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+11); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+14); SET_HL(hFF8B);
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  CYC(b_+15, b_+16); alu_add(gb, B);
  CYC(b_+16, b_+18); H8(hFF8D) = A;
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+22); B = alu_inc8(gb, B);
  CYC(b_+22, b_+24); A = H8(hFF8D);
  CYC(b_+24, b_+25); alu_cp(gb, B);
  CYC(b_+25, b_+26); ret_effect(gb);
}

// Checks that an object is within [hFF8B] pixels of a position on both axes.
// @param bc    Target position
// @param hl    Object's Y position
// @param hFF8B Range we must be within on each axis
// @param[out] cflag c if the object is within [hFF8B] pixels of the position
void checkObjectIsCloseToPosition_b08_hook(GB *gb) {
  BASE(checkObjectIsCloseToPosition_b08);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); push_effect(gb, b_+4); impaInCutscene_checkComponent(gb);
  CYC(b_+4, b_+5); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); B = C;
  impaInCutscene_checkComponent(gb);
}

void impaUpdateAnimationIfDirectionChanged_hook(GB *gb) {
  BASE(impaUpdateAnimationIfDirectionChanged);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+7, b_+8); ret_effect(gb); return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+12); interactionSetAnimation_hook(gb);
}

// @param[out] cflag c if Link has approached the stone to trigger Impa's reaction
void impaCheckApproachedStone_hook(GB *gb) {
  BASE(impaCheckApproachedStone);
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveRoom);
  CYC(b_+3, b_+5); alu_cp(gb, 0x59);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7); goto notClose;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wScrollMode);
  CYC(b_+10, b_+12); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+12, b_+13); ret_effect(gb); return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); SET_HL(w1Link_yh);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+19); alu_cp(gb, 0x58);
  if (!(F & FC)) {
    CYCT(b_+19, b_+21); goto notClose;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); L = alu_inc8(gb, L);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_cp(gb, 0x78);
  CYC(b_+25, b_+26); ret_effect(gb);
  return;

notClose:
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+28); ret_effect(gb);
}

// @param[out] zflag z if counter1 has reached 0.
void impaAnimateAndDecCounter1_hook(GB *gb) {
  BASE(impaAnimateAndDecCounter1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+7, interactionAnimate_hook, SYM(interactionAnimate), b_+10);
  CYC(b_+10, b_+12); alu_or(gb, 0x01);
  CYC(b_+12, b_+13); ret_effect(gb);
}

// Shows text if Link tries to leave the screen with the stone.
void impaPreventLinkFromLeavingStoneScreen_hook(GB *gb) {
  BASE(impaPreventLinkFromLeavingStoneScreen);
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); B = 0x76;
  CYC(b_+6, b_+7); alu_cp(gb, B);
  if (F & FC) {
    CYCT(b_+7, b_+9); goto checkX;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wKeysPressed);
  CYC(b_+12, b_+14); alu_and(gb, 0x80); // BTN_DOWN
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16); goto showText;
  }
  CYC(b_+14, b_+16);

checkX:
  CYC(b_+16, b_+18); L = (uint8_t)w1Link_xh;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); B = 0x96;
  CYC(b_+21, b_+22); alu_cp(gb, B);
  if (F & FC) {
    CYCT(b_+22, b_+23); ret_effect(gb); return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wKeysPressed);
  CYC(b_+26, b_+28); alu_and(gb, 0x10); // BTN_RIGHT
  if (F & FZ) {
    CYCT(b_+28, b_+29); ret_effect(gb); return;
  }
  CYC(b_+28, b_+29);

showText:
  CYC(b_+29, b_+30); mem_wr(gb, HL, B);
  CYC(b_+30, b_+33); SET_BC(0x010a); // TX_010a
  CYC(b_+33, b_+36); showText_hook(gb);
}
