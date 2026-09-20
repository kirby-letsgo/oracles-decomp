#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Script pointer tables (bank $0c mainScripts.*), indexed by subid or game progress.
#define interactionCode3b_scriptTable_bank08 SYM(interactionCode3b__scriptTable)
#define interactionCode3b_subid1And2ScriptTable_bank08 SYM(interactionCode3b__subid1And2ScriptTable)
#define interactionCode3b_subid3And4ScriptTable_bank08 SYM(interactionCode3b__subid3And4ScriptTable)
#define interactionCode3b_subid5ScriptTable_bank08 SYM(interactionCode3b__subid5ScriptTable)

// Cross-bank targets referenced by address only.
#define getGameProgress_1_bank09 SYM(getGameProgress_1)
#define getGameProgress_2_bank09 SYM(getGameProgress_2)
#define interactionOscillateXRandomly_bank08 SYM(interactionOscillateXRandomly)
#define villagerGalSubid07Script_bank0c SYM(villagerGalSubid07Script)
#define genericNpcScript_bank0c SYM(genericNpcScript)
#define linkedGameNpcScript_bank0c SYM(linkedGameNpcScript)

static uint16_t femaleVillager_jumpTable(GB *gb) {
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

static void femaleVillager_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @state1: per-subid update. Also reached by fallthrough from @initSubid06.
static void femaleVillager_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3b);
  CYC(b_+212, b_+214); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+214, b_+215); A = mem_rd(gb, DE);
  CYC(b_+215, b_+216); push_effect(gb, b_+216);
  do { uint16_t jt_ = (femaleVillager_jumpTable(gb));
    if (jt_ == b_+234) { goto runSubid00; }
    else if (jt_ == b_+328) { goto runScriptAndAnimateFacingLink; }
    else if (jt_ == b_+334) { goto runSubid06; }
    else if (jt_ == b_+343) { goto runSubid07; }
    else { HANDOFF(HL); }
  } while (0);

runSubid00:
  // Cutscene where guy is struck by lightning in intro
  CYC(b_+234, b_+236); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+236, b_+237); A = mem_rd(gb, DE);
  CYC(b_+237, b_+238); push_effect(gb, b_+238);
  do { uint16_t jt_ = (femaleVillager_jumpTable(gb));
    if (jt_ == b_+248) { goto substate0; }
    else if (jt_ == b_+266) { goto substate1; }
    else if (jt_ == b_+288) { goto substate2; }
    else if (jt_ == b_+306) { goto substate3; }
    else if (jt_ == b_+318) { goto substate4; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+248, b_+251); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+251, b_+253); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+253, b_+256); interactionAnimate_hook(gb); return;
  }
  CYC(b_+253, b_+256);
  CALL_C(b_+256, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+259);
  CYC(b_+259, b_+261); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+261, b_+262); A = mem_rd(gb, HL);
  CYC(b_+262, b_+264); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+264, b_+265); mem_wr(gb, HL, A);
  CYC(b_+265, b_+266); ret_effect(gb);
  return;

substate1:
  // callab interactionOscillateXRandomly
  CYC(b_+266, b_+269); SET_HL(interactionOscillateXRandomly_bank08);
  CYC(b_+269, b_+271); E = 0x08;
  CALL_C(b_+271, interBankCall_hook, 0x008a, b_+274);
  CYC(b_+274, b_+277); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+277, b_+279); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+279, b_+280); ret_effect(gb); return;
  }
  CYC(b_+279, b_+280);
  CALL_C(b_+280, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+283);
  CYC(b_+283, b_+285); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+285, b_+287); mem_wr(gb, HL, 0x1e);
  CYC(b_+287, b_+288); ret_effect(gb);
  return;

substate2:
  CALL_C(b_+288, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+291);
  if (!(F & FZ)) {
    CYCT(b_+291, b_+292); ret_effect(gb); return;
  }
  CYC(b_+291, b_+292);
  CYC(b_+292, b_+295); SET_BC(0xfe40); // -$1c0
  CALL_C(b_+295, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+298);
  CYC(b_+298, b_+300); A = 0x53; // SND_JUMP
  CALL_C(b_+300, playSound_b00_hook, SYM(playSound_b00), b_+303);
  CYC(b_+303, b_+306); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(b_+306, b_+308); C = 0x20;
  CALL_C(b_+308, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+311);
  if (!(F & FZ)) {
    CYCT(b_+311, b_+312); ret_effect(gb); return;
  }
  CYC(b_+311, b_+312);
  CALL_C(b_+312, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+315);
  CYC(b_+315, b_+318); goto loadScript;

substate4:
  CALL_C(b_+318, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+321);
  CALL_C(b_+321, interactionRunScript_hook, SYM(interactionRunScript), b_+324);
  if (!(F & FC)) {
    CYCT(b_+324, b_+325); ret_effect(gb); return;
  }
  CYC(b_+324, b_+325);
  CYC(b_+325, b_+328); interactionDelete_hook(gb);
  return;

runScriptAndAnimateFacingLink:
  // Generic NPCs
  CALL_C(b_+328, interactionRunScript_hook, SYM(interactionRunScript), b_+331);
  CYC(b_+331, b_+334); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid06:
  // Linked game NPC
  CALL_C(b_+334, interactionRunScript_hook, SYM(interactionRunScript), b_+337);
  if (F & FC) {
    CYCT(b_+337, b_+340); interactionDelete_hook(gb); return;
  }
  CYC(b_+337, b_+340);
  CYC(b_+340, b_+343); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid07:
  // NPC in eyeglasses library (present)
  CALL_C(b_+343, interactionRunScript_hook, SYM(interactionRunScript), b_+346);
  CYC(b_+346, b_+349); interactionAnimateAsNpc_hook(gb);
  return;

loadScript:
  CYC(b_+349, b_+351); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+351, b_+352); A = mem_rd(gb, DE);
  CYC(b_+352, b_+355); SET_HL(interactionCode3b_scriptTable_bank08);
  CYC(b_+355, b_+356); femaleVillager_addDoubleIndex(gb, b_+356);
  CYC(b_+356, b_+357); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+357, b_+358); H = mem_rd(gb, HL);
  CYC(b_+358, b_+359); L = A;
  CYC(b_+359, b_+362); interactionSetScript_hook(gb);
}

// @initSubid: per-subid initialization. Reached only by a static `call` from
// interactionCode3b@state0 (NOT HOOKED).
static void femaleVillager_initSubid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3b);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (femaleVillager_jumpTable(gb));
    if (jt_ == b_+50) { goto initSubid00; }
    else if (jt_ == b_+55) { goto initSubid01; }
    else if (jt_ == b_+86) { goto initSubid03; }
    else if (jt_ == b_+118) { goto initSubid05; }
    else if (jt_ == b_+198) { goto initSubid06; }
    else if (jt_ == b_+155) { goto initSubid07; }
    else if (jt_ == b_+183) { goto initSubid08; }
    else { HANDOFF(HL); }
  } while (0);

initSubid00:
  CYC(b_+50, b_+52); A = 0x01;
  CYC(b_+52, b_+55); interactionSetAnimation_hook(gb);
  return;

initSubid01:
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(b_+55, b_+58); SET_HL(getGameProgress_1_bank09);
  CYC(b_+58, b_+60); E = 0x09;
  CALL_C(b_+60, interBankCall_hook, 0x008a, b_+63);
  CYC(b_+63, b_+65); C = 0x01;
  CYC(b_+65, b_+66); alu_xor(gb, A);
  CALL_C(b_+66, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+69);
  if (!(F & FZ)) {
    CYCT(b_+69, b_+72); interactionDelete_hook(gb); return;
  }
  CYC(b_+69, b_+72);
  CYC(b_+72, b_+73); A = B;
  CYC(b_+73, b_+76); SET_HL(interactionCode3b_subid1And2ScriptTable_bank08);
  CYC(b_+76, b_+77); femaleVillager_addDoubleIndex(gb, b_+77);
  CYC(b_+77, b_+78); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+78, b_+79); H = mem_rd(gb, HL);
  CYC(b_+79, b_+80); L = A;
  CALL_C(b_+80, interactionSetScript_hook, SYM(interactionSetScript), b_+83);
  CYC(b_+83, b_+86); objectSetVisible82_hook(gb);
  return;

initSubid03:
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(b_+86, b_+89); SET_HL(getGameProgress_2_bank09);
  CYC(b_+89, b_+91); E = 0x09;
  CALL_C(b_+91, interBankCall_hook, 0x008a, b_+94);
  CYC(b_+94, b_+96); C = 0x03;
  CYC(b_+96, b_+98); A = 0x01;
  CALL_C(b_+98, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+101);
  if (!(F & FZ)) {
    CYCT(b_+101, b_+104); interactionDelete_hook(gb); return;
  }
  CYC(b_+101, b_+104);
  CYC(b_+104, b_+105); A = B;
  CYC(b_+105, b_+108); SET_HL(interactionCode3b_subid3And4ScriptTable_bank08);
  CYC(b_+108, b_+109); femaleVillager_addDoubleIndex(gb, b_+109);
  CYC(b_+109, b_+110); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+110, b_+111); H = mem_rd(gb, HL);
  CYC(b_+111, b_+112); L = A;
  CALL_C(b_+112, interactionSetScript_hook, SYM(interactionSetScript), b_+115);
  CYC(b_+115, b_+118); objectSetVisible82_hook(gb);
  return;

initSubid05:
  CYC(b_+118, b_+120); A = 0x01;
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(b_+123, b_+126); SET_HL(getGameProgress_2_bank09);
  CYC(b_+126, b_+128); E = 0x09;
  CALL_C(b_+128, interBankCall_hook, 0x008a, b_+131);
  CYC(b_+131, b_+133); C = 0x05;
  CYC(b_+133, b_+135); A = 0x02;
  CALL_C(b_+135, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+138);
  if (!(F & FZ)) {
    CYCT(b_+138, b_+141); interactionDelete_hook(gb); return;
  }
  CYC(b_+138, b_+141);
  CYC(b_+141, b_+142); A = B;
  CYC(b_+142, b_+145); SET_HL(interactionCode3b_subid5ScriptTable_bank08);
  CYC(b_+145, b_+146); femaleVillager_addDoubleIndex(gb, b_+146);
  CYC(b_+146, b_+147); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+147, b_+148); H = mem_rd(gb, HL);
  CYC(b_+148, b_+149); L = A;
  CALL_C(b_+149, interactionSetScript_hook, SYM(interactionSetScript), b_+152);
  CYC(b_+152, b_+155); objectSetVisible82_hook(gb);
  return;

initSubid07:
  CYC(b_+155, b_+157); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(b_+157, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+160);
  CYC(b_+160, b_+162); A = 0x26; // <TX_1526
  if (F & FZ) {
    CYCT(b_+162, b_+164);
  } else {
    CYC(b_+162, b_+164);
    CYC(b_+164, b_+166); A = 0x27; // <TX_1527
  }
  CYC(b_+166, b_+168); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+168, b_+169); mem_wr(gb, DE, A);
  CYC(b_+169, b_+170); E = alu_inc8(gb, E);
  CYC(b_+170, b_+172); A = 0x15; // >TX_1500
  CYC(b_+172, b_+173); mem_wr(gb, DE, A);
  CYC(b_+173, b_+174); alu_xor(gb, A);
  CALL_C(b_+174, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+177);
  CYC(b_+177, b_+180); SET_HL(villagerGalSubid07Script_bank0c);
  CYC(b_+180, b_+183); interactionSetScript_hook(gb);
  return;

initSubid08:
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+185, b_+187); A = 0x03; // <TX_0f03
  CYC(b_+187, b_+188); mem_wr(gb, DE, A);
  CYC(b_+188, b_+189); E = alu_inc8(gb, E);
  CYC(b_+189, b_+191); A = 0x0f; // >TX_0f03
  CYC(b_+191, b_+192); mem_wr(gb, DE, A);
  CYC(b_+192, b_+195); SET_HL(genericNpcScript_bank0c);
  CYC(b_+195, b_+198); interactionSetScript_hook(gb);
  return;

initSubid06:
  CYC(b_+198, b_+200); A = 0x05;
  CYC(b_+200, b_+202); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+202, b_+203); mem_wr(gb, DE, A);
  CYC(b_+203, b_+206); SET_HL(linkedGameNpcScript_bank0c);
  CALL_C(b_+206, interactionSetScript_hook, SYM(interactionSetScript), b_+209);
  CALL_C(b_+209, interactionRunScript_hook, SYM(interactionRunScript), b_+212);
  femaleVillager_state1(gb, sp0_);
}

// INTERAC_FEMALE_VILLAGER: assorted female NPCs, subid-dispatched.
void interactionCode3b_hook(GB *gb) {
  BASE(interactionCode3b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (femaleVillager_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+212) { femaleVillager_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); femaleVillager_initSubid(gb, sp0_);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}
