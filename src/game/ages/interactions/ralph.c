#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// mainScripts.* (bank $0c), scriptHelp.* (bank $15) and agesInteractionsBank0a.* (bank $0a),
// referenced by address only.
#define ralphSubid00Script_bank0c SYM(ralphSubid00Script)
#define ralphSubid01Script_bank0c SYM(ralphSubid01Script)
#define ralphSubid02Script_bank0c SYM(ralphSubid02Script_b0c)
#define ralphSubid03Script_bank0c SYM(ralphSubid03Script_b0c)
#define ralphSubid04Script_part1_bank0c SYM(ralphSubid04Script_part1)
#define ralphSubid04Script_part2_bank0c SYM(ralphSubid04Script_part2)
#define ralphSubid04Script_part3_bank0c SYM(ralphSubid04Script_part3)
#define ralphSubid05Script_bank0c SYM(ralphSubid05Script)
#define ralphSubid06Script_part1_bank0c SYM(ralphSubid06Script_part1)
#define ralphSubid06Script_part2_bank0c SYM(ralphSubid06Script_part2)
#define ralphSubid07Script_bank0c SYM(ralphSubid07Script)
#define ralphSubid08Script_bank0c SYM(ralphSubid08Script)
#define ralphSubid09Script_bank0c SYM(ralphSubid09Script)
#define ralphSubid0aScript_unlinked_bank0c SYM(ralphSubid0aScript_unlinked)
#define ralphSubid0aScript_linked_bank0c SYM(ralphSubid0aScript_linked)
#define ralphSubid0bScript_bank0c SYM(ralphSubid0bScript_b0c)
#define ralphSubid10Script_bank0c SYM(ralphSubid10Script_b0c)
#define ralphSubid0cScript_bank0c SYM(ralphSubid0cScript_b0c)
#define ralphSubid0dScript_bank0c SYM(ralphSubid0dScript)
#define ralphSubid0eScript_bank0c SYM(ralphSubid0eScript)
#define ralphSubid11Script_bank0c SYM(ralphSubid11Script)
#define ralphSubid12Script_bank0c SYM(ralphSubid12Script)
#define ralph_createLinkedSwordAnimation_bank15 SYM(ralph_createLinkedSwordAnimation)
#define objectWritePositionTocfd5_bank15 SYM(objectWritePositionTocfd5)
#define loadAngleAndCounterPreset_bank0a SYM(loadAngleAndCounterPreset)

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
  BASE(ralphState0);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+81) { goto initSubid00; }
    else if (jt_ == b_+343) { goto initSubid01; }
    else if (jt_ == b_+88) { goto initSubid02; }
    else if (jt_ == b_+105) { goto initSubid03; }
    else if (jt_ == b_+146) { goto initSubid04; }
    else if (jt_ == b_+59) { goto initSubid06; }
    else if (jt_ == b_+176) { goto initSubid07; }
    else if (jt_ == b_+185) { goto initSubid08; }
    else if (jt_ == b_+202) { goto initSubid09; }
    else if (jt_ == b_+265) { goto initSubid0a; }
    else if (jt_ == b_+355) { goto initSubid0b; }
    else if (jt_ == b_+452) { goto initSubid0c; }
    else if (jt_ == b_+515) { goto initSubid0d; }
    else if (jt_ == b_+328) { goto initSubid0e; }
    else if (jt_ == b_+338) { goto initSubid0f; }
    else if (jt_ == b_+402) { goto initSubid10; }
    else if (jt_ == b_+431) { goto initSubid11; }
    else if (jt_ == b_+492) { goto initSubid12; }
    else { HANDOFF(HL); }
  } while (0);

initSubid06:
  CYC(b_+59, b_+62); SET_HL(ralphSubid06Script_part1_bank0c);
  CYC(b_+62, b_+65); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+65, b_+67); alu_cp(gb, 0x0b);
  if (!(F & FZ)) {
    CYCT(b_+67, b_+69); goto initSubid06_setScript;
  }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+72); SET_BC((SYM(interactionCode1e__scriptSubidTable) + 24));
  CALL_C(b_+72, interactionSetPosition_hook, SYM(interactionSetPosition), b_+75);
  CYC(b_+75, b_+78); SET_HL(ralphSubid06Script_part2_bank0c);
initSubid06_setScript:
  CALL_C(b_+78, interactionSetScript_hook, SYM(interactionSetScript), b_+81);

initSubid00:
  // Also @initSubid05
  CYC(b_+81, b_+82); alu_xor(gb, A);

setAnimation:
  CALL_C(b_+82, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+85);
  CYC(b_+85, b_+88); objectSetVisiblec2_hook(gb);
  return;

initSubid02:
  CYC(b_+88, b_+90); A = 0x09;
  CALL_C(b_+90, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+93);
  CYC(b_+93, b_+96); SET_HL(ralphSubid02Script_bank0c);
  CALL_C(b_+96, interactionSetScript_hook, SYM(interactionSetScript), b_+99);
  CALL_C(b_+99, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+102);
  CYC(b_+102, b_+105); objectSetVisiblec2_hook(gb);
  return;

initSubid03:
  CYC(b_+105, b_+107); A = 0x15; // GLOBALFLAG_GAVE_ROPE_TO_RAFTON
  CALL_C(b_+107, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+110);
  if (F & FZ) {
    CYCT(b_+110, b_+113); TAIL(interactionDelete);
  }
  CYC(b_+110, b_+113);
  CALL_C(b_+113, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+116);
  CYC(b_+116, b_+118); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+118, b_+121); TAIL(interactionDelete);
  }
  CYC(b_+118, b_+121);
  CYC(b_+121, b_+123); A = 0x01;
  CYC(b_+123, b_+126); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+126, b_+129); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+129, b_+131); A = 0x03;
  CALL_C(b_+131, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+134);
  CYC(b_+134, b_+135); H = D;
  CYC(b_+135, b_+137); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+137, b_+139); mem_wr(gb, HL, 0x78);
  CYC(b_+139, b_+141); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x01);
  CYC(b_+143, b_+146); objectSetVisiblec2_hook(gb);
  return;

initSubid04:
  CYC(b_+146, b_+148); A = 0x01;
  CALL_C(b_+148, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+151);
  CYC(b_+151, b_+154); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+154, b_+156); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+156, b_+158); goto initSubid04_part2;
  }
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+161); SET_HL(ralphSubid04Script_part1_bank0c);
  CALL_C(b_+161, interactionSetScript_hook, SYM(interactionSetScript), b_+164);
  CYC(b_+164, b_+167); objectSetInvisible_hook(gb);
  return;
initSubid04_part2:
  CYC(b_+167, b_+170); SET_HL(ralphSubid04Script_part2_bank0c);
  CALL_C(b_+170, interactionSetScript_hook, SYM(interactionSetScript), b_+173);
  CYC(b_+173, b_+176); objectSetVisiblec2_hook(gb);
  return;

initSubid07:
  CYC(b_+176, b_+179); SET_HL(ralphSubid07Script_bank0c);
  CALL_C(b_+179, interactionSetScript_hook, SYM(interactionSetScript), b_+182);
  CYC(b_+182, b_+185); objectSetInvisible_hook(gb);
  return;

initSubid08:
  // callab scriptHelp.ralph_createLinkedSwordAnimation
  CYC(b_+185, b_+188); SET_HL(ralph_createLinkedSwordAnimation_bank15);
  CYC(b_+188, b_+190); E = 0x15;
  CALL_C(b_+190, interBankCall_hook, 0x008a, b_+193);
  CYC(b_+193, b_+196); SET_HL(ralphSubid08Script_bank0c);
  CALL_C(b_+196, interactionSetScript_hook, SYM(interactionSetScript), b_+199);
  CYC(b_+199, b_+202); objectSetVisiblec2_hook(gb);
  return;

initSubid09:
  CYC(b_+202, b_+204); A = 0x32; // GLOBALFLAG_RALPH_ENTERED_AMBIS_PALACE
  CALL_C(b_+204, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+207);
  if (!(F & FZ)) {
    CYCT(b_+207, b_+209); goto deleteSelf;
  }
  CYC(b_+207, b_+209);
  // Check that we have the 5th essence
  CYC(b_+209, b_+211); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+211, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+214);
  if (!(F & FC)) {
    CYCT(b_+214, b_+216); goto deleteSelf;
  }
  CYC(b_+214, b_+216);
  CYC(b_+216, b_+218); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+218, b_+220); goto initSubid09_hasEssence;
  }
  CYC(b_+218, b_+220);
deleteSelf:
  CYC(b_+220, b_+223); interactionDelete_hook(gb);
  return;
initSubid09_hasEssence:
  CYC(b_+223, b_+225); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+225, b_+227); A = 0x50; // SPEED_200
  CYC(b_+227, b_+228); mem_wr(gb, DE, A);
  CYC(b_+228, b_+230); A = 0x35; // MUS_RALPH
  CYC(b_+230, b_+233); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+233, playSound_b00_hook, SYM(playSound_b00), b_+236);
  CALL_C(b_+236, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+239);
  CYC(b_+239, b_+240); A = alu_inc8(gb, A);
  CYC(b_+240, b_+243); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+243, b_+246); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+246, b_+249); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+249, b_+252); W8(w1Link_direction) = A;
  CYC(b_+252, b_+255); SET_HL(ralphSubid09Script_bank0c);
  CALL_C(b_+255, interactionSetScript_hook, SYM(interactionSetScript), b_+258);
  CYC(b_+258, b_+259); alu_xor(gb, A);
  CALL_C(b_+259, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+262);
  CYC(b_+262, b_+265); objectSetVisiblec2_hook(gb);
  return;

initSubid0a:
  CYC(b_+265, b_+267); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+267, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+270);
  if (!(F & FC)) {
    CYCT(b_+270, b_+273); TAIL(interactionDelete);
  }
  CYC(b_+270, b_+273);
  CYC(b_+273, b_+275); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+275, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+278);
  if (!(F & FZ)) {
    CYCT(b_+278, b_+281); TAIL(interactionDelete);
  }
  CYC(b_+278, b_+281);
  CYC(b_+281, b_+283); A = 0x45; // GLOBALFLAG_RALPH_ENTERED_BLACK_TOWER
  CALL_C(b_+283, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+286);
  if (!(F & FZ)) {
    CYCT(b_+286, b_+289); TAIL(interactionDelete);
  }
  CYC(b_+286, b_+289);
  CALL_C(b_+289, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+292);
  CYC(b_+292, b_+295); SET_HL(ralphSubid0aScript_unlinked_bank0c);
  if (F & FZ) {
    CYCT(b_+295, b_+297); goto initSubid0a_setScript;
  }
  CYC(b_+295, b_+297);
  // Linked game: adjust position, load a different script
  CYC(b_+297, b_+298); H = D;
  CYC(b_+298, b_+300); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+300, b_+302); mem_wr(gb, HL, 0x50);
  CYC(b_+302, b_+304); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+304, b_+306); mem_wr(gb, HL, 0x1e);
  CYC(b_+306, b_+309); SET_HL(ralphSubid0aScript_linked_bank0c);
initSubid0a_setScript:
  CALL_C(b_+309, interactionSetScript_hook, SYM(interactionSetScript), b_+312);
  CALL_C(b_+312, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+315);
  CYC(b_+315, b_+318); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+318, b_+319); A = alu_inc8(gb, A);
  CYC(b_+319, b_+322); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+322, b_+325); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+325, b_+328); objectSetVisiblec2_hook(gb);
  return;

initSubid0e:
  CYC(b_+328, b_+330); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+330, b_+332); A = 0xff;
  CYC(b_+332, b_+333); mem_wr(gb, DE, A);
  CYC(b_+333, b_+336); SET_HL(ralphSubid0eScript_bank0c);
  CYC(b_+336, b_+338); goto setScriptAndRunState1;

initSubid0f:
  CYC(b_+338, b_+340); A = 0x01;
  CYC(b_+340, b_+343); goto setAnimation;

initSubid01:
  CYC(b_+343, b_+346); SET_HL(ralphSubid01Script_bank0c);

setScriptAndRunState1:
  CALL_C(b_+346, interactionSetScript_hook, SYM(interactionSetScript), b_+349);
  CYC(b_+349, b_+352); ralphRunSubid_hook(gb);
  return;

delete_:
  CYC(b_+352, b_+355); interactionDelete_hook(gb);
  return;

initSubid0b:
  CYC(b_+355, b_+357); A = 0x26; // TREASURE_TUNE_OF_CURRENTS
  CALL_C(b_+357, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+360);
  if (F & FC) {
    CYCT(b_+360, b_+362); goto delete_;
  }
  CYC(b_+360, b_+362);
  CALL_C(b_+362, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+365);
  CYC(b_+365, b_+367); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+367, b_+369); goto delete_;
  }
  CYC(b_+367, b_+369);
  // Check that Link has timewarped in from a specific spot
  CYC(b_+369, b_+372); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+372, b_+373); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+373, b_+375); goto delete_;
  }
  CYC(b_+373, b_+375);
  CYC(b_+375, b_+378); A = mem_rd(gb, wWarpDestPos);
  CYC(b_+378, b_+380); alu_cp(gb, 0x24);
  if (!(F & FZ)) {
    CYCT(b_+380, b_+382); goto delete_;
  }
  CYC(b_+380, b_+382);
  CYC(b_+382, b_+385); SET_HL(ralphSubid0bScript_bank0c);

setScriptAndDisableObjects:
  CALL_C(b_+385, interactionSetScript_hook, SYM(interactionSetScript), b_+388);
  CYC(b_+388, b_+390); A = 0x81;
  CYC(b_+390, b_+393); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+393, b_+396); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+396, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+399);
  CYC(b_+399, b_+402); ralphRunSubid_hook(gb);
  return;

initSubid10:
  CALL_C(b_+402, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+405);
  CYC(b_+405, b_+407); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+407, b_+410); TAIL(interactionDelete);
  }
  CYC(b_+407, b_+410);
  CYC(b_+410, b_+412); A = 0x43; // GLOBALFLAG_TALKED_TO_CHEVAL
  CALL_C(b_+412, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+415);
  if (F & FZ) {
    CYCT(b_+415, b_+418); TAIL(interactionDelete);
  }
  CYC(b_+415, b_+418);
  CYC(b_+418, b_+421); A = mem_rd(gb, wWarpDestPos);
  CYC(b_+421, b_+423); alu_cp(gb, 0x17);
  if (!(F & FZ)) {
    CYCT(b_+423, b_+426); TAIL(interactionDelete);
  }
  CYC(b_+423, b_+426);
  CYC(b_+426, b_+429); SET_HL(ralphSubid10Script_bank0c);
  CYC(b_+429, b_+431); goto setScriptAndDisableObjects;

initSubid11:
  CYC(b_+431, b_+433); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+433, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+436);
  if (F & FZ) {
    CYCT(b_+436, b_+439); TAIL(interactionDelete);
  }
  CYC(b_+436, b_+439);
  CYC(b_+439, b_+441); A = 0x03;
  CALL_C(b_+441, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+444);
  CYC(b_+444, b_+447); SET_HL(ralphSubid11Script_bank0c);
  CALL_C(b_+447, interactionSetScript_hook, SYM(interactionSetScript), b_+450);
  CYC(b_+450, b_+452); ralphRunSubid_hook(gb);
  return;

initSubid0c:
  CYC(b_+452, b_+455); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+455, b_+457); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+457, b_+460); TAIL(interactionDelete);
  }
  CYC(b_+457, b_+460);
  CALL_C(b_+460, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+463);
  // callab scriptHelp.ralph_createLinkedSwordAnimation
  CYC(b_+463, b_+466); SET_HL(ralph_createLinkedSwordAnimation_bank15);
  CYC(b_+466, b_+468); E = 0x15;
  CALL_C(b_+468, interBankCall_hook, 0x008a, b_+471);
  CYC(b_+471, b_+474); SET_HL(ralphSubid0cScript_bank0c);
  CALL_C(b_+474, interactionSetScript_hook, SYM(interactionSetScript), b_+477);
  CYC(b_+477, b_+478); alu_xor(gb, A);
  CYC(b_+478, b_+481); mem_wr(gb, wTmpcfc0 + 0x1e, A);
  CYC(b_+481, b_+484); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  CALL_C(b_+484, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+487);
  CALL_C(b_+487, interactionRunScript_hook, SYM(interactionRunScript), b_+490);
  CYC(b_+490, b_+492); ralphRunSubid_hook(gb);
  return;

initSubid12:
  CALL_C(b_+492, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+495);
  if (F & FZ) {
    CYCT(b_+495, b_+498); TAIL(interactionDelete);
  }
  CYC(b_+495, b_+498);
  CYC(b_+498, b_+501); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+501, b_+503); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+503, b_+506); TAIL(interactionDelete);
  }
  CYC(b_+503, b_+506);
  CALL_C(b_+506, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+509);
  CYC(b_+509, b_+512); SET_HL(ralphSubid12Script_bank0c);
  CYC(b_+512, b_+515); interactionSetScript_hook(gb);
  return;

initSubid0d:
  CYC(b_+515, b_+518); A = mem_rd(gb, wScreenTransitionDirection);
  CYC(b_+518, b_+520); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+520, b_+523); TAIL(interactionDelete);
  }
  CYC(b_+520, b_+523);
  CYC(b_+523, b_+526); SET_HL(ralphSubid0dScript_bank0c);
  CALL_C(b_+526, interactionSetScript_hook, SYM(interactionSetScript), b_+529);
  CALL_C(b_+529, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), SYM(ralphRunSubid));
  // Falls through into ralphRunSubid
  ralphRunSubid_hook(gb);
}

// ralphSubid08@getNextAngle: B = 2 preset index; zflag set when there are no more presets.
// Fallen into from substate 0 and `call z`ed from substate 1.
static void ralph_getNextAngle(GB *gb, uint16_t sp0_) {
  BASE(ralphSubid08);
  CYC(b_+24, b_+26); B = 0x02;
  // callab agesInteractionsBank0a.loadAngleAndCounterPreset
  CYC(b_+26, b_+29); SET_HL(loadAngleAndCounterPreset_bank0a);
  CYC(b_+29, b_+31); E = 0x0a;
  CALL_C(b_+31, interBankCall_hook, 0x008a, b_+34);
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+36); alu_or(gb, A);
  CYC(b_+36, b_+37); ret_effect(gb);
}

// ralphSubid0a@moveHorizontallyTowardRalph: force Link to walk to x=$50. Fallen into from
// ralphSubid0a substate 1 and `jp`ed to from ralphSubid0a_linked substate 0.
static void ralph_moveHorizontallyTowardRalph(GB *gb) {
  BASE(ralphSubid0a);
  CYC(b_+70, b_+73); A = W8(w1Link_xh);
  CYC(b_+73, b_+75); alu_sub(gb, 0x50);
  CYC(b_+75, b_+76); B = A;
  CYC(b_+76, b_+78); alu_add(gb, 0x02);
  CYC(b_+78, b_+80); alu_cp(gb, 0x05);
  if (F & FC) {
    CYCT(b_+80, b_+82); goto incSubstate;
  }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+83); A = B;
  CYC(b_+83, b_+85); alu_bit(gb, 7, A);
  CYC(b_+85, b_+87); B = 0x18;
  if (F & FZ) {
    CYCT(b_+87, b_+89); goto setForceMovement;
  }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); B = 0x08;
  CYC(b_+91, b_+92); alu_cpl(gb);
  CYC(b_+92, b_+93); A = alu_inc8(gb, A);
setForceMovement:
  CYC(b_+93, b_+96); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+96, b_+98); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+98, b_+101); mem_wr(gb, wLinkForceState, A);
  CYC(b_+101, b_+102); A = B;
  CYC(b_+102, b_+105); SET_HL(w1Link_angle);
  CYC(b_+105, b_+106); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+106, b_+108); A = alu_swap(gb, A);
  CYC(b_+108, b_+109); alu_rlca(gb);
  CYC(b_+109, b_+110); mem_wr(gb, HL, A); // [w1Link.direction]
incSubstate:
  CYC(b_+110, b_+113); interactionIncSubstate_hook(gb);
}

// ralphSubid0a@moveVerticallyTowardRalph: B = distance. Fallen into from ralphSubid0a
// substate 2 and `jp`ed to from ralphSubid0a_linked substate 1.
static void ralph_moveVerticallyTowardRalph(GB *gb) {
  BASE(ralphSubid0a);
  CYC(b_+115, b_+118); A = W8(w1Link_state);
  CYC(b_+118, b_+120); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(b_+120, b_+121); ret_effect(gb); return;
  }
  CYC(b_+120, b_+121);
  // Make Link move vertically toward Ralph
  CYC(b_+121, b_+124); SET_HL(w1Link_angle);
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x10);
  CYC(b_+126, b_+127); L = alu_dec8(gb, L);
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(b_+129, b_+130); A = B;
  CYC(b_+130, b_+133); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+133, b_+135); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+135, b_+138); mem_wr(gb, wLinkForceState, A);
  CYC(b_+138, b_+141); interactionIncSubstate_hook(gb);
}

// ralphSubid0a@setDirectionAndAnimationWhenLinkFinishedMoving: B = Link's direction,
// C = Ralph's animation. Fallen into from ralphSubid0a substate 3 and `jp`ed to from
// ralphSubid0a_linked substate 2.
static void ralph_setDirectionAndAnimationWhenLinkFinishedMoving(GB *gb, uint16_t sp0_) {
  BASE(ralphSubid0a);
  CYC(b_+144, b_+147); A = W8(w1Link_state);
  CYC(b_+147, b_+149); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) {
    CYCT(b_+149, b_+150); ret_effect(gb); return;
  }
  CYC(b_+149, b_+150);
  CALL_C(b_+150, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+153);
  CYC(b_+153, b_+154); A = B;
  CYC(b_+154, b_+157); W8(w1Link_direction) = A;
  CYC(b_+157, b_+158); A = C;
  CALL_C(b_+158, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+161);
  CYC(b_+161, b_+164); interactionIncSubstate_hook(gb);
}

// INTERAC_RALPH
// Variables:
//   var3f: for some subids, ralph's animations only updates when this is 0.
void interactionCode37_hook(GB *gb) {
  BASE(interactionCode37);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == SYM(ralphState0) && hook_is(gb, SYM(ralphState0), ralphState0_hook)) { ralphState0_hook(gb); return; }
    else if (jt_ == SYM(ralphRunSubid) && hook_is(gb, SYM(ralphRunSubid), ralphRunSubid_hook)) { ralphRunSubid_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ralphState0_hook(GB *gb) {
  BASE(ralphState0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CYC(b_+6, b_+9); push_effect(gb, b_+9); ralph_initSubid(gb, sp0_);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+16); TAIL(objectMarkSolidPosition);
  }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void ralphRunSubid_hook(GB *gb) {
  BASE(ralphRunSubid);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == SYM(ralphSubid00) && hook_is(gb, SYM(ralphSubid00), ralphSubid00_hook)) { ralphSubid00_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid01) && hook_is(gb, SYM(ralphSubid01), ralphSubid01_hook)) { ralphSubid01_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid02) && hook_is(gb, SYM(ralphSubid02), ralphSubid02_hook)) { ralphSubid02_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid03) && hook_is(gb, SYM(ralphSubid03), ralphSubid03_hook)) { ralphSubid03_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid04) && hook_is(gb, SYM(ralphSubid04), ralphSubid04_hook)) { ralphSubid04_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid05) && hook_is(gb, SYM(ralphSubid05), ralphSubid05_hook)) { ralphSubid05_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid06) && hook_is(gb, SYM(ralphSubid06), ralphSubid06_hook)) { ralphSubid06_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid07) && hook_is(gb, SYM(ralphSubid07), ralphSubid07_hook)) { ralphSubid07_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid08) && hook_is(gb, SYM(ralphSubid08), ralphSubid08_hook)) { ralphSubid08_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid09) && hook_is(gb, SYM(ralphSubid09), ralphSubid09_hook)) { ralphSubid09_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid0a) && hook_is(gb, SYM(ralphSubid0a), ralphSubid0a_hook)) { ralphSubid0a_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid0b) && hook_is(gb, SYM(ralphSubid0b), ralphSubid0b_hook)) { ralphSubid0b_hook(gb); return; } // subids $0b and $10
    else if (jt_ == SYM(ralphRunScriptAndDeleteWhenOver) && hook_is(gb, SYM(ralphRunScriptAndDeleteWhenOver), ralphRunScriptAndDeleteWhenOver_hook)) { ralphRunScriptAndDeleteWhenOver_hook(gb); return; }
    else if (jt_ == SYM(ralphRunScriptWithConditionalAnimation) && hook_is(gb, SYM(ralphRunScriptWithConditionalAnimation), ralphRunScriptWithConditionalAnimation_hook)) { ralphRunScriptWithConditionalAnimation_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid0e) && hook_is(gb, SYM(ralphSubid0e), ralphSubid0e_hook)) { ralphSubid0e_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), interactionAnimate_hook)) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(nayruRunScriptWithConditionalAnimation) && hook_is(gb, SYM(nayruRunScriptWithConditionalAnimation), nayruRunScriptWithConditionalAnimation_hook)) { nayruRunScriptWithConditionalAnimation_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid12) && hook_is(gb, SYM(ralphSubid12), ralphSubid12_hook)) { ralphSubid12_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Cutscene where Nayru gets possessed
void ralphSubid00_hook(GB *gb) {
  BASE(ralphSubid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+41) { goto substate1; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+8, interactionAnimate_hook, SYM(interactionAnimate), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+14, b_+16); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  CALL_C(b_+17, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+20);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x3c);
  CYC(b_+24, b_+27); SET_BC(0x3088);
  CALL_C(b_+27, interactionSetPosition_hook, SYM(interactionSetPosition), b_+30);
  CYC(b_+30, b_+32); A = 0x03;
  CALL_C(b_+32, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+35);
  CYC(b_+35, b_+38); SET_HL(ralphSubid00Script_bank0c);
  CYC(b_+38, b_+41); interactionSetScript_hook(gb);
  return;

substate1:
  CALL_C(b_+41, interactionAnimate_hook, SYM(interactionAnimate), b_+44);
  CALL_C(b_+44, interactionRunScript_hook, SYM(interactionRunScript), b_+47);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+51, b_+52);
  // Animate more quickly if moving fast
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+57); alu_cp(gb, 0x28); // SPEED_100
  if (!(F & FC)) {
    CYCT(b_+57, b_+60); TAIL(interactionAnimate);
  }
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+61); ret_effect(gb);
}

// Cutscene after Nayru is possessed
void ralphSubid02_hook(GB *gb) {
  BASE(ralphSubid02);
  uint16_t sp0_ = gb->sp;
  // They probably meant to call "checkInteractionSubstate" instead? It looks like @state0
  // will never be run...
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto state1;
  }
  CYC(b_+3, b_+5);

  // @state0
  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  CALL_C(b_+8, interactionAnimate_hook, SYM(interactionAnimate), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+14, b_+16); alu_cp(gb, 0x1f);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); interactionIncSubstate_hook(gb);
  return;

state1:
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(b_+20, b_+23); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(b_+23, b_+25); E = 0x15;
  CALL_C(b_+25, interBankCall_hook, 0x008a, b_+28);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(b_+32, interactionAnimate_hook, SYM(interactionAnimate), b_+35);
  } else {
    CYC(b_+32, b_+35);
  }
  CALL_C(b_+35, interactionAnimate_hook, SYM(interactionAnimate), b_+38);
  CALL_C(b_+38, interactionRunScript_hook, SYM(interactionRunScript), b_+41);
  if (!(F & FC)) {
    CYCT(b_+41, b_+42); ret_effect(gb); return;
  }
  CYC(b_+41, b_+42);
  // Script done
  CYC(b_+42, b_+44); A = 0xfb; // SNDCTRL_MEDIUM_FADEOUT
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
  CYC(b_+47, b_+50); TAIL(interactionDelete);
}

// Cutscene outside Ambi's palace before getting mystery seeds
void ralphSubid01_hook(GB *gb) {
  BASE(ralphSubid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, ralphTurnLinkTowardSelf_hook, SYM(ralphTurnLinkTowardSelf), b_+9);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+13, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+16);
  } else {
    CYC(b_+13, b_+16);
  }
  CYC(b_+16, b_+19); TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
}

// Cutscene after talking to Rafton
void ralphSubid03_hook(GB *gb) {
  BASE(ralphSubid03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+22) { goto substate0; }
    else if (jt_ == b_+52) { goto substate1; }
    else if (jt_ == b_+62) { goto substate2; }
    else if (jt_ == b_+79) { goto substate3; }
    else if (jt_ == b_+94) { goto substate4; }
    else if (jt_ == b_+116) { goto substate5; }
    else if (jt_ == b_+130) { goto substate6; }
    else if (jt_ == b_+171) { goto substate7; }
    else if (jt_ == b_+189) { goto substate8; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+22, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+25);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+27); goto substate0_waiting;
  }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x1e);
  CYC(b_+29, b_+31); A = 0x02;
  CALL_C(b_+31, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+34);
  CYC(b_+34, b_+37); interactionIncSubstate_hook(gb);
  return;
substate0_waiting:
  CYC(b_+37, b_+40); A = mem_rd(gb, wFrameCounter);
  CYC(b_+40, b_+42); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+43); ret_effect(gb); return;
  }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+48); alu_xor(gb, 0x02);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+52); interactionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(b_+52, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+55);
  if (!(F & FZ)) {
    CYCT(b_+55, b_+56); ret_effect(gb); return;
  }
  CYC(b_+55, b_+56);
  CALL_C(b_+56, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+59);
  CYC(b_+59, b_+62); startJump_hook(gb);
  return;

substate2:
  CALL_C(b_+62, interactionAnimate_hook, SYM(interactionAnimate), b_+65);
  CYC(b_+65, b_+67); C = 0x20;
  CALL_C(b_+67, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+70);
  if (!(F & FZ)) {
    CYCT(b_+70, b_+71); ret_effect(gb); return;
  }
  CYC(b_+70, b_+71);
  CALL_C(b_+71, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+74);
  CYC(b_+74, b_+76); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x0a);
  CYC(b_+78, b_+79); ret_effect(gb);
  return;

substate3:
  CALL_C(b_+79, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+82);
  if (!(F & FZ)) {
    CYCT(b_+82, b_+83); ret_effect(gb); return;
  }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+85, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+88);
  CYC(b_+88, b_+91); SET_BC(0x2a0a); // TX_2a0a
  CYC(b_+91, b_+94); showText_hook(gb);
  return;

substate4:
  CALL_C(b_+94, interactionDecCounter1IfTextNotActive_hook, SYM(interactionDecCounter1IfTextNotActive), b_+97);
  if (!(F & FZ)) {
    CYCT(b_+97, b_+98); ret_effect(gb); return;
  }
  CYC(b_+97, b_+98);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x30);
  CALL_C(b_+100, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+103);
  CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x10);
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+111, b_+113); A = 0x02;
  CYC(b_+113, b_+116); interactionSetAnimation_hook(gb);
  return;

substate5:
  CALL_C(b_+116, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+119);
  CALL_C(b_+119, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+122);
  if (!(F & FZ)) {
    CYCT(b_+122, b_+125); TAIL(objectApplySpeed);
  }
  CYC(b_+122, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x06);
  CYC(b_+127, b_+130); interactionIncSubstate_hook(gb);
  return;

substate6:
  CALL_C(b_+130, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+133);
  if (!(F & FZ)) {
    CYCT(b_+133, b_+134); ret_effect(gb); return;
  }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x0a);
  // Align with Link's x-position
  CALL_C(b_+136, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+139);
  CYC(b_+139, b_+142); A = W8(w1Link_xh);
  CYC(b_+142, b_+144); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+144, b_+145); alu_sub(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+145, b_+147); goto startScript;
  }
  CYC(b_+145, b_+147);
  if (F & FC) {
    CYCT(b_+147, b_+149); goto substate6_moveLeft;
  }
  CYC(b_+147, b_+149);
  // @@moveRight
  CYC(b_+149, b_+151); B = 0x08;
  CYC(b_+151, b_+153); C = 0x01; // DIR_RIGHT
  CYC(b_+153, b_+155); goto substate6_setMovement;
substate6_moveLeft:
  CYC(b_+155, b_+156); alu_cpl(gb);
  CYC(b_+156, b_+157); A = alu_inc8(gb, A);
  CYC(b_+157, b_+159); B = 0x18;
  CYC(b_+159, b_+161); C = 0x03; // DIR_LEFT
substate6_setMovement:
  CYC(b_+161, b_+163); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+163, b_+164); mem_wr(gb, HL, A);
  CYC(b_+164, b_+166); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+166, b_+167); mem_wr(gb, HL, B);
  CYC(b_+167, b_+168); A = C;
  CYC(b_+168, b_+171); interactionSetAnimation_hook(gb);
  return;

substate7:
  CALL_C(b_+171, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+174);
  CALL_C(b_+174, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+177);
  if (!(F & FZ)) {
    CYCT(b_+177, b_+180); TAIL(objectApplySpeed);
  }
  CYC(b_+177, b_+180);

startScript:
  CALL_C(b_+180, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+183);
  CYC(b_+183, b_+186); SET_HL(ralphSubid03Script_bank0c);
  CYC(b_+186, b_+189); interactionSetScript_hook(gb);
  return;

substate8:
  CALL_C(b_+189, ralphAnimateBasedOnSpeedAndRunScript_hook, SYM(ralphAnimateBasedOnSpeedAndRunScript), b_+192);
  if (!(F & FC)) {
    CYCT(b_+192, b_+193); ret_effect(gb); return;
  }
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+195); A = 0x04; // MUS_OVERWORLD_PAST
  CYC(b_+195, b_+198); mem_wr(gb, wActiveMusic2, A);
  CYC(b_+198, b_+201); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+201, playSound_b00_hook, SYM(playSound_b00), b_+204);
  CYC(b_+204, b_+207); TAIL(interactionDelete);
}

// Cutscene on maku tree screen after saving Nayru
void ralphSubid04_hook(GB *gb) {
  BASE(ralphSubid04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    // Borrow some of Nayru's code from the same cutscene
    if (jt_ == SYM(nayruSubid02Substate0) && hook_is(gb, SYM(nayruSubid02Substate0), nayruSubid02Substate0_hook)) { nayruSubid02Substate0_hook(gb); return; }
    else if (jt_ == b_+10) { goto substate1; }
    else if (jt_ == b_+30) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate1:
  CYC(b_+10, b_+13); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+13, b_+15); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+18); TAIL(nayruFlipDirectionAtRandomIntervals);
  }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+21);
  CYC(b_+21, b_+24); SET_HL(ralphSubid04Script_part3_bank0c);
  CALL_C(b_+24, interactionSetScript_hook, SYM(interactionSetScript), b_+27);
  CYC(b_+27, b_+30);

substate2:
  CALL_C(b_+30, ralphAnimateBasedOnSpeedAndRunScript_hook, SYM(ralphAnimateBasedOnSpeedAndRunScript), b_+33);
  if (!(F & FC)) {
    CYCT(b_+33, b_+34); ret_effect(gb); return;
  }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+37); TAIL(interactionDelete);
}

// Cutscene in black tower where Nayru/Ralph meet you to try to escape. Substate 3 falls
// through into ralphRunScript.
void ralphSubid05_hook(GB *gb) {
  BASE(ralphSubid05);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+17) { goto substate0; }
    else if (jt_ == b_+29) { goto substate1; }
    else if (jt_ == b_+44) { goto substate2; }
    else if (jt_ == b_+58) { goto substate3; }
    else if (jt_ == SYM(ralphRunScript) && hook_is(gb, SYM(ralphRunScript), ralphRunScript_hook)) { ralphRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+17, b_+20); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+20, b_+22); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+23); ret_effect(gb); return;
  }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, startJump_hook, SYM(startJump), b_+26);
  CYC(b_+26, b_+29); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+29, b_+31); C = 0x20;
  CALL_C(b_+31, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+34);
  if (!(F & FZ)) {
    CYCT(b_+34, b_+35); ret_effect(gb); return;
  }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+38); SET_HL(ralphSubid05Script_bank0c);
  CALL_C(b_+38, interactionSetScript_hook, SYM(interactionSetScript), b_+41);
  CYC(b_+41, b_+44); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+44, b_+47); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+47, b_+49); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+49, b_+52); TAIL(interactionRunScript);
  }
  CYC(b_+49, b_+52);
  CALL_C(b_+52, startJump_hook, SYM(startJump), b_+55);
  CYC(b_+55, b_+58); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(b_+58, b_+60); C = 0x20;
  CALL_C(b_+60, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+63);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+64); ret_effect(gb); return;
  }
  CYC(b_+63, b_+64);
  CALL_C(b_+64, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+67);
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+69, b_+70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(ralphRunScript);
}

void ralphRunScript_hook(GB *gb) {
  BASE(ralphRunScript);
  CYC(b_+0, b_+3); TAIL(interactionRunScript);
}

void ralphSubid06_hook(GB *gb) {
  BASE(ralphSubid06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+13) { goto substate0; }
    else if (jt_ == b_+35) { goto substate1; }
    else if (jt_ == SYM(ralphRunScript) && hook_is(gb, SYM(ralphRunScript), ralphRunScript_hook)) { ralphRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(b_+13, b_+16); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(b_+16, b_+18); E = 0x15;
  CALL_C(b_+18, interBankCall_hook, 0x008a, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+24, b_+26); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+26, b_+29); TAIL(interactionRunScript);
  }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, startJump_hook, SYM(startJump), b_+32);
  CYC(b_+32, b_+35); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+35, b_+37); C = 0x20;
  CALL_C(b_+37, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+40);
  if (!(F & FZ)) {
    CYCT(b_+40, b_+41); ret_effect(gb); return;
  }
  CYC(b_+40, b_+41);
  CALL_C(b_+41, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+44);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+47, b_+49); TAIL(ralphRunScript);
}

// Cutscene postgame where they warp to the maku tree, Ralph notices the statue
void ralphSubid07_hook(GB *gb) {
  BASE(ralphSubid07);
  uint16_t sp0_ = gb->sp;
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(b_+0, b_+3); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(b_+3, b_+5); E = 0x15;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == SYM(ralphAnimateBasedOnSpeedAndRunScript) && hook_is(gb, SYM(ralphAnimateBasedOnSpeedAndRunScript), ralphAnimateBasedOnSpeedAndRunScript_hook)) { ralphAnimateBasedOnSpeedAndRunScript_hook(gb); return; } // substates 0 and 3
    else if (jt_ == SYM(ralphSubid07Substate1) && hook_is(gb, SYM(ralphSubid07Substate1), ralphSubid07Substate1_hook)) { ralphSubid07Substate1_hook(gb); return; }
    else if (jt_ == SYM(ralphSubid07Substate2) && hook_is(gb, SYM(ralphSubid07Substate2), ralphSubid07Substate2_hook)) { ralphSubid07Substate2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ralphAnimateBasedOnSpeedAndRunScript_hook(GB *gb) {
  BASE(ralphAnimateBasedOnSpeedAndRunScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionRunScript);
}

// Falls through into ralphSubid07Substate2.
void ralphSubid07Substate1_hook(GB *gb) {
  BASE(ralphSubid07Substate1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+3);
  CALL_C(b_+3, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+6);
  CYC(b_+6, b_+9); SET_BC(0xfe40); // -$1c0
  CALL_C(b_+9, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), SYM(ralphSubid07Substate2));
  TAIL(ralphSubid07Substate2);
}

void ralphSubid07Substate2_hook(GB *gb) {
  BASE(ralphSubid07Substate2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+9);
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+12, b_+15); TAIL(objectSetVisible82);
}

// Cutscene in credits where Ralph is training with his sword
void ralphSubid08_hook(GB *gb) {
  BASE(ralphSubid08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+37) { goto substate1; }
    else if (jt_ == b_+62) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+10, interactionAnimate_hook, SYM(interactionAnimate), b_+13);
  CALL_C(b_+13, interactionRunScript_hook, SYM(interactionRunScript), b_+16);
  if (!(F & FC)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  // Script done
  CALL_C(b_+17, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+20);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x1e); // SPEED_c0
  ralph_getNextAngle(gb, sp0_);
  return;

substate1:
  CALL_C(b_+37, interactionAnimate_hook, SYM(interactionAnimate), b_+40);
  CALL_C(b_+40, objectApplySpeed_hook, SYM(objectApplySpeed), b_+43);
  CALL_C(b_+43, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+46);
  if (F & FZ) {
    CYCT(b_+46, b_+49); push_effect(gb, b_+49); ralph_getNextAngle(gb, sp0_);
  } else {
    CYC(b_+46, b_+49);
  }
  if (!(F & FZ)) {
    CYCT(b_+49, b_+50); ret_effect(gb); return;
  }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+53);
  CYC(b_+53, b_+55); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x5a);
  CYC(b_+57, b_+59); A = 0x08;
  CYC(b_+59, b_+62); interactionSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(b_+62, interactionAnimate_hook, SYM(interactionAnimate), b_+65);
  CALL_C(b_+65, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+68);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+69); ret_effect(gb); return;
  }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); A = 0xff;
  CYC(b_+71, b_+74); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  CYC(b_+74, b_+75); ret_effect(gb);
}

// Cutscene where Ralph charges in to Ambi's palace
void ralphSubid09_hook(GB *gb) {
  BASE(ralphSubid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+6); TAIL(interactionAnimateBasedOnSpeed);
  }
  CYC(b_+3, b_+6);
  // Script done
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+10); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+10, b_+13); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+13, b_+16); TAIL(interactionDelete);
}

// Cutscene where Ralph's about to charge into the black tower
void ralphSubid0a_hook(GB *gb) {
  BASE(ralphSubid0a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6); TAIL(ralphSubid0a_linked);
  }
  CYC(b_+3, b_+6);

  // Unlinked game
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+20) { goto substate0; }
    else if (jt_ == b_+62) { goto substate1; }
    else if (jt_ == b_+113) { goto substate2; }
    else if (jt_ == b_+141) { goto substate3; }
    else if (jt_ == b_+164) { goto substate4; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // Create an exclamation mark above Link
  CALL_C(b_+20, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+23);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+24); ret_effect(gb); return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x9f); // INTERAC_EXCLAMATION_MARK
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x1e);
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+32, b_+35); A = W8(w1Link_yh);
  CYC(b_+35, b_+37); alu_add(gb, 0x0e);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+42); A = W8(w1Link_xh);
  CYC(b_+42, b_+44); alu_sub(gb, 0x0a);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+47); A = 0x50; // SND_CLINK
  CALL_C(b_+47, playSound_b00_hook, SYM(playSound_b00), b_+50);
  CALL_C(b_+50, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+53);
  CYC(b_+53, b_+55); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x1e);
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+61, b_+62); ret_effect(gb);
  return;

substate1:
  CALL_C(b_+62, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+65);
  if (!(F & FZ)) {
    CYCT(b_+65, b_+66); ret_effect(gb); return;
  }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+67); alu_xor(gb, A);
  CALL_C(b_+67, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+70);
  ralph_moveHorizontallyTowardRalph(gb);
  return;

substate2:
  CYC(b_+113, b_+115); B = 0x50;
  ralph_moveVerticallyTowardRalph(gb);
  return;

substate3:
  CYC(b_+141, b_+144); SET_BC(0x0103); // ldbc DIR_RIGHT,$03
  ralph_setDirectionAndAnimationWhenLinkFinishedMoving(gb, sp0_);
  return;

substate4:
  CALL_C(b_+164, interactionRunScript_hook, SYM(interactionRunScript), b_+167);
  if (!(F & FC)) {
    CYCT(b_+167, b_+170); TAIL(interactionAnimateBasedOnSpeed);
  }
  CYC(b_+167, b_+170);
  CYC(b_+170, b_+171); alu_xor(gb, A);
  CYC(b_+171, b_+174); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+174, b_+177); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+177, b_+180); TAIL(interactionDelete);
}

void ralphSubid0a_linked_hook(GB *gb) {
  BASE(ralphSubid0a_linked);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+9);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+21) { goto substate0; }
    else if (jt_ == b_+29) { goto substate1; }
    else if (jt_ == b_+34) { goto substate2; }
    else if (jt_ == b_+40) { goto substate3; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+25, b_+26); ret_effect(gb); return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+29); ralph_moveHorizontallyTowardRalph(gb);
  return;

substate1:
  CYC(b_+29, b_+31); B = 0x18;
  CYC(b_+31, b_+34); ralph_moveVerticallyTowardRalph(gb);
  return;

substate2:
  CYC(b_+34, b_+37); SET_BC(0x0200); // ldbc DIR_DOWN,$00
  CYC(b_+37, b_+40); ralph_setDirectionAndAnimationWhenLinkFinishedMoving(gb, sp0_);
  return;

substate3:
  CYC(b_+40, b_+41); ret_effect(gb);
}

// $0b: Cutscene where Ralph tells you about getting Tune of Currents
// $10: Cutscene after talking to Cheval
void ralphSubid0b_hook(GB *gb) {
  BASE(ralphSubid0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(b_+4, ralphTurnLinkTowardSelf_hook, SYM(ralphTurnLinkTowardSelf), b_+7);
  } else {
    CYC(b_+4, b_+7);
  }
  CYC(b_+7, b_+9); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (ralph_jumpTable(gb));
    if (jt_ == b_+17) { goto substate0; }
    else if (jt_ == b_+22) { goto substate1; }
    else if (jt_ == SYM(ralphRunScriptWithConditionalAnimation) && hook_is(gb, SYM(ralphRunScriptWithConditionalAnimation), ralphRunScriptWithConditionalAnimation_hook)) { ralphRunScriptWithConditionalAnimation_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+17, interactionAnimate_hook, SYM(interactionAnimate), b_+20);
  CYC(b_+20, b_+22); ralphRunScriptWithConditionalAnimation_hook(gb);
  return;

substate1:
  // Create dust at Ralph's feet every 8 frames
  CYC(b_+22, b_+25); A = mem_rd(gb, wFrameCounter);
  CYC(b_+25, b_+27); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+27, b_+29); TAIL(ralphRunScriptWithConditionalAnimation);
  }
  CYC(b_+27, b_+29);
  CALL_C(b_+29, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+32);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+34); TAIL(ralphRunScriptWithConditionalAnimation);
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x81);
  CYC(b_+39, b_+42); SET_BC(0x0804);
  CALL_C(b_+42, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+45);
  CYC(b_+45, b_+47); TAIL(ralphRunScriptWithConditionalAnimation);
}

// Runs script, deletes self when finished, and updates animations only if var3f is 0.
void ralphRunScriptWithConditionalAnimation_hook(GB *gb) {
  BASE(ralphRunScriptWithConditionalAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+10, b_+13); TAIL(interactionAnimate);
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+14); ret_effect(gb);
}

// Cutscene with Nayru and Ralph when Link exits the black tower. Falls through into
// ralphRunScriptAndDeleteWhenOver.
void ralphSubid0e_hook(GB *gb) {
  BASE(ralphSubid0e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+3, b_+5); alu_cp(gb, 0x5a);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7); TAIL(ralphRunScriptAndDeleteWhenOver);
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+8, b_+10); TAIL(ralphRunScriptAndDeleteWhenOver);
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = W8(w1Link_direction);
  CYC(b_+13, b_+15); alu_sub(gb, 0x02);
  CYC(b_+15, b_+17); alu_and(gb, 0x03);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+20, b_+21); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+21, b_+23); TAIL(ralphRunScriptAndDeleteWhenOver);
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CALL_C(b_+24, interactionSetAnimation_hook, SYM(interactionSetAnimation), SYM(ralphRunScriptAndDeleteWhenOver));
  TAIL(ralphRunScriptAndDeleteWhenOver);
}

void ralphRunScriptAndDeleteWhenOver_hook(GB *gb) {
  BASE(ralphRunScriptAndDeleteWhenOver);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); TAIL(interactionAnimateAsNpc);
}

// NPC after beating Veran, before beating Twinrova in a linked game
void ralphSubid12_hook(GB *gb) {
  BASE(ralphSubid12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, npcFaceLinkAndAnimate_hook, SYM(npcFaceLinkAndAnimate), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionRunScript);
}

// Unused?
void ralphFunc_738b_hook(GB *gb) {
  BASE(ralphFunc_738b);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7); goto angleDecreasing;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_BC(0x2068);
  CYC(b_+10, b_+13); A = mem_rd(gb, wFrameCounter);
  CYC(b_+13, b_+14); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+14, b_+15); ret_effect(gb); return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+21); alu_and(gb, 0x1f);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+25); ret_effect(gb); return;
  }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x1f);

angleDecreasing:
  CYC(b_+32, b_+35); SET_BC((SYM(nayruState0) + 13));
  CYC(b_+35, b_+38); A = mem_rd(gb, wFrameCounter);
  CYC(b_+38, b_+39); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+39, b_+40); ret_effect(gb); return;
  }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+44); A = alu_dec8(gb, A);
  CYC(b_+44, b_+46); alu_and(gb, 0x1f);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  CYC(b_+47, b_+48); ret_effect(gb);
}

void ralphTurnLinkTowardSelf_hook(GB *gb) {
  BASE(ralphTurnLinkTowardSelf);
  CYC(b_+0, b_+3); A = W8(w1Link_xh);
  CYC(b_+3, b_+5); alu_add(gb, 0x10);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_add(gb, 0x10);
  CYC(b_+11, b_+12); alu_sub(gb, B);
  CYC(b_+12, b_+14); B = 0x01; // DIR_RIGHT
  if (!(F & FC)) {
    CYCT(b_+14, b_+16); goto checkDistance;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); B = 0x03; // DIR_LEFT
  CYC(b_+18, b_+19); alu_cpl(gb);
checkDistance:
  CYC(b_+19, b_+21); alu_cp(gb, 0x0c);
  if (!(F & FC)) {
    CYCT(b_+21, b_+23); goto setDirection;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); B = 0x02; // DIR_DOWN
setDirection:
  CYC(b_+25, b_+28); SET_HL(w1Link_direction);
  CYC(b_+28, b_+29); mem_wr(gb, HL, B);
  CYC(b_+29, b_+32); TAIL(setLinkForceStateToState08);
}

// Also called from boy (subid 4), interactionCode3d and nayruSubid03.
void startJump_hook(GB *gb) {
  BASE(startJump);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xfe40); // -$1c0
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+8); A = 0x53; // SND_JUMP
  CYC(b_+8, b_+11); TAIL(playSound_b00);
}
