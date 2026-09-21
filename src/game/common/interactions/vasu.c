#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode89_jump_table(GB *gb) {
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

static void interactionCode89_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// INTERAC_VASU
//
// interactionCode89@updateState is called once from the top level, return address 0x47c8;
// sp1_ is captured right after that push, so it is the POST-push value (one level deeper than
// sp0_) for as long as nothing else has pushed or popped since. Every "return to
// interactionCode89_hook" check inside updateState pops that same 0x47c8 slot, restoring the
// stack pointer to what it was BEFORE that push -- i.e. sp0_, not sp1_. Calls made directly
// from within updateState's own body (checkRingBoxAndRingsObtained from @state2/@state4,
// setScriptAndGotoState4's genuine call from @setBlueSnakeExitScript) push FROM the current
// sp1_ value, so THEIR matching pop correctly restores sp1_, not sp0_ and not sp1_-2.
// interactionCode89@setScriptAndGotoState4 is reached two ways: falling straight through from
// @loadLinkedScript with no extra push (so its ret resolves the outer 0x47c8/sp0_ case), and
// the genuine call just described (so its ret also needs the 0x4954/sp1_ case).
void interactionCode89_hook(GB *gb) {
  BASE(interactionCode89);
  uint16_t sp0_ = gb->sp;
  uint16_t sp1_;
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto afterTextboxSetup; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+11); W8(wTextboxPosition) = A;
  CYC(b_+11, b_+13); A = 0x08; // TEXTBOXFLAG_DONTCHECKPOSITION
  CYC(b_+13, b_+16); W8(wTextboxFlags) = A;

afterTextboxSetup:
  CYC(b_+16, b_+19); push_effect(gb, b_+19); goto updateState;
afterUpdateState:
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; } // jp nz
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+29); TAIL(interactionPushLinkAwayAndUpdateDrawPriority); // jp

updateState:
  sp1_ = gb->sp;
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  {
    CYC(b_+32, b_+33); push_effect(gb, b_+33);
    uint16_t target = interactionCode89_jump_table(gb);
    if (target == b_+45) goto state0;
    if (target == b_+95) goto state1;
    if (target == b_+140) goto state2;
    if (target == b_+202) goto state3;
    if (target == b_+232) goto state4;
    goto state5;
  }

state0:
  CYC(b_+45, b_+47); A = 0x01;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CALL_C(b_+48, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+51);
  CALL_C(b_+51, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+54);
  CYC(b_+54, b_+56); A = 0x30; // >TX_3000
  CALL_C(b_+56, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+59);
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+63, b_+65); goto state0_initVasu; } // jr z
  CYC(b_+63, b_+65);

  // interactionCode89@state0@initSnake
  CYC(b_+65, b_+67); A = 0x06;
  CALL_C(b_+67, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+70);
  CALL_C(b_+70, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+73);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x0f);
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  CALL_C(b_+76, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+79);
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+81, b_+84); objectAddToAButtonSensitiveObjectList_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state0_initVasu:
  CYC(b_+84, b_+86); A = 0x04;
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+92); SET_HL((SYM(interactionCode91__subid00__state0) + 7)); // mainScripts.vasuScript
  CYC(b_+92, b_+95); interactionSetScript_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state1:
  CYC(b_+95, b_+97); C = 0x18;
  CALL_C(b_+97, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+100);
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  if (!(F & FC)) { CYCT(b_+103, b_+106); interactionSetAnimation_hook(gb); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // jp nc
  CYC(b_+103, b_+106);
  CALL_C(b_+106, interactionAnimate_hook, SYM(interactionAnimate), b_+109);
  CYC(b_+109, b_+110); H = D;
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CYC(b_+113, b_+114); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+114); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+116); alu_xor(gb, A);
  CYC(b_+116, b_+117); mem_wr(gb, HL, A);
  CYC(b_+117, b_+118); A = alu_inc8(gb, A);
  CYC(b_+118, b_+121); W8(wMenuDisabled) = A;
  CYC(b_+121, b_+124); W8(wDisabledObjects) = A;
  CYC(b_+124, b_+125); E = L;
  CALL_C(b_+125, objectRemoveFromAButtonSensitiveObjectList_hook, SYM(objectRemoveFromAButtonSensitiveObjectList), b_+128);
  CYC(b_+128, b_+129); H = D;
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+131, b_+133); A = 0x02;
  CYC(b_+133, b_+134); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+134, b_+135); L = alu_dec8(gb, L);
  CYC(b_+135, b_+136); A = mem_rd(gb, HL);
  CYC(b_+136, b_+137); A = alu_inc8(gb, A);
  CYC(b_+137, b_+140); interactionSetAnimation_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state2:
  CYC(b_+140, b_+143); push_effect(gb, b_+143); goto checkRingBoxAndRingsObtained;
afterCheckRing_state2:
  CALL_C(b_+143, interactionAnimate_hook, SYM(interactionAnimate), b_+146);
  CYC(b_+146, b_+148); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+148, b_+149); A = mem_rd(gb, DE);
  CYC(b_+149, b_+151); alu_and(gb, 0x04);
  CYC(b_+151, b_+152); B = A;
  CYC(b_+152, b_+154); C = 0x00;
  CYC(b_+154, b_+156); E = INTERACTION_BASE + 0x36; // Interaction.var36
  CYC(b_+156, b_+157); A = mem_rd(gb, DE);
  CYC(b_+157, b_+158); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+158, b_+160); goto loadPrelinkedScript; } // jr z
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+162, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+165);
  if (!(F & FZ)) { CYCT(b_+165, b_+167); goto loadLinkedScript; } // jr nz
  CYC(b_+165, b_+167);
  CYC(b_+167, b_+170); SET_HL(wFileIsLinkedGame);
  CYC(b_+170, b_+171); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+171, b_+172); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+172, b_+174); goto loadLinkedScript; } // jr nz
  CYC(b_+172, b_+174);

loadPrelinkedScript:
  CYC(b_+174, b_+176); C = 0x02;

loadLinkedScript:
  CYC(b_+176, b_+177); A = B;
  CYC(b_+177, b_+178); alu_add(gb, C);
  CYC(b_+178, b_+181); SET_HL(b_+194); // interactionCode89@scriptTable
  CYC(b_+181, b_+182); interactionCode89_addAToHl_from_rst(gb, b_+182);
  CYC(b_+182, b_+183); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+183, b_+184); H = mem_rd(gb, HL);
  CYC(b_+184, b_+185); L = A;

setScriptAndGotoState4:
  CALL_C(b_+185, interactionSetScript_hook, SYM(interactionSetScript), b_+188);
  CYC(b_+188, b_+190); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+190, b_+192); A = 0x04;
  CYC(b_+192, b_+193); mem_wr(gb, DE, A);
  RET(b_+193);
  if (gb->pc == b_+415 && gb->sp == sp1_) goto afterSetScriptFromBlueSnake;
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // ret

state3:
  CALL_C(b_+202, interactionAnimate_hook, SYM(interactionAnimate), b_+205);
  CYC(b_+205, b_+207); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+207, b_+208); A = mem_rd(gb, DE);
  CYC(b_+208, b_+209); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+209); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(b_+209, b_+210);
  CYC(b_+210, b_+211); alu_xor(gb, A);
  CYC(b_+211, b_+214); W8(wMenuDisabled) = A;
  CYC(b_+214, b_+216); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+216, b_+218); A = 0x01;
  CYC(b_+218, b_+219); mem_wr(gb, DE, A);
  CYC(b_+219, b_+221); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+221, b_+222); A = mem_rd(gb, DE);
  CYC(b_+222, b_+223); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+223); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(b_+223, b_+224);
  CALL_C(b_+224, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+227);
  CYC(b_+227, b_+229); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+229, b_+232); objectAddToAButtonSensitiveObjectList_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state4:
  CYC(b_+232, b_+235); push_effect(gb, b_+235); goto checkRingBoxAndRingsObtained;
afterCheckRing_state4:
  CALL_C(b_+235, interactionAnimate_hook, SYM(interactionAnimate), b_+238);
  CALL_C(b_+238, interactionRunScript_hook, SYM(interactionRunScript), b_+241);
  if (!(F & FC)) { RET_TAKEN(b_+241); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret nc
  CYC(b_+241, b_+242);
  CYC(b_+242, b_+243); alu_xor(gb, A);
  CYC(b_+243, b_+246); W8(wMenuDisabled) = A;
  CYC(b_+246, b_+249); W8(wDisabledObjects) = A;
  CYC(b_+249, b_+251); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+251, b_+252); A = mem_rd(gb, DE);
  CYC(b_+252, b_+253); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+253); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(b_+253, b_+254);
  CYC(b_+254, b_+256); alu_add(gb, 0x02);
  CALL_C(b_+256, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+259);
  CYC(b_+259, b_+261); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+261, b_+263); A = 0x03;
  CYC(b_+263, b_+264); mem_wr(gb, DE, A);
  RET(b_+264);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // ret

state5:
  CALL_C(b_+265, interactionAnimate_hook, SYM(interactionAnimate), b_+268);
  CYC(b_+268, b_+270); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+270, b_+271); A = mem_rd(gb, DE);
  {
    CYC(b_+271, b_+272); push_effect(gb, b_+272);
    uint16_t target = interactionCode89_jump_table(gb);
    if (target == b_+282) goto state5Substate0;
    if (target == b_+301) goto state5Substate1;
    if (target == b_+345) goto state5Substate2;
    if (target == b_+420) goto state5Substate3;
    goto state5Substate4;
  }

state5Substate0:
  CALL_C(b_+282, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+285);
  CALL_C(b_+285, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+288);
  CYC(b_+288, b_+289); alu_xor(gb, A);
  CYC(b_+289, b_+291); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+291, b_+292); mem_wr(gb, HL, A);
  CYC(b_+292, b_+294); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+294, b_+296); mem_wr(gb, HL, 0x02);
  CYC(b_+296, b_+298); A = 0x04;
  CYC(b_+298, b_+301); interactionSetAnimation_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate1:
  CALL_C(b_+301, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+304);
  if (!(F & FZ)) { CYCT(b_+304, b_+306); goto label_0a_036; } // jr nz
  CYC(b_+304, b_+306);
  CYC(b_+306, b_+307); L = alu_inc8(gb, L);
  CYC(b_+307, b_+308); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+308, b_+310); goto label_0a_036; } // jr nz
  CYC(b_+308, b_+310);
  CYC(b_+310, b_+311); alu_xor(gb, A);
  CYC(b_+311, b_+313); mem_wr(gb, 0xff01, A); // R_SB
  CYC(b_+313, b_+316); SET_HL((SYM(interactionCode67__label_0a_047) + 70)); // mainScripts.blueSnakeExitScript_cableNotConnected
  CYC(b_+316, b_+318); B = 0x80;
  CYC(b_+318, b_+320); goto setBlueSnakeExitScript; // jr

label_0a_036:
  CYC(b_+320, b_+322); A = H8(hSerialInterruptBehaviour);
  CYC(b_+322, b_+323); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+323, b_+326); serialFunc_0c73_hook(gb); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // jp z
  CYC(b_+323, b_+326);
  CYC(b_+326, b_+328); alu_and(gb, 0x01);
  CYC(b_+328, b_+330); alu_add(gb, 0x01);
  CYC(b_+330, b_+332); H8(hFFBE) = A;
  CALL_C(b_+332, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+335);
  CYC(b_+335, b_+337); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+337, b_+339); mem_wr(gb, HL, 180);
  CYC(b_+339, b_+342); SET_BC(0x3030); // TX_3030
  CYC(b_+342, b_+345); showTextNonExitable_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate2:
  CALL_C(b_+345, serialFunc_0c8d_hook, SYM(serialFunc_0c8d), b_+348);
  CYC(b_+348, b_+350); A = H8(hSerialInterruptBehaviour);
  CYC(b_+350, b_+351); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+351); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret nz
  CYC(b_+351, b_+352);
  CYC(b_+352, b_+354); A = hram_rd(gb, 0x70); // R_SVBK
  CYC(b_+354, b_+355); push_effect(gb, AF);
  CYC(b_+355, b_+357); A = 0x04; // :w4RingFortuneStuff
  CYC(b_+357, b_+359); hram_wr(gb, 0x70, A);
  CYC(b_+359, b_+361); A = H8(hFFBD);
  CYC(b_+361, b_+362); B = A;
  CYC(b_+362, b_+365); A = mem_rd(gb, wGenericCutscene_cbc2);
  CYC(b_+365, b_+366); E = A;
  CYC(b_+366, b_+369); A = W8(w4RingFortuneStuff);
  CYC(b_+369, b_+370); C = A;
  CYC(b_+370, b_+371); SET_AF(pop_effect(gb));
  CYC(b_+371, b_+373); hram_wr(gb, 0x70, A);
  CYC(b_+373, b_+374); A = B;
  CYC(b_+374, b_+375); alu_or(gb, E);
  if (!(F & FZ)) { CYCT(b_+375, b_+377); goto blueSnakeErrorCondition; } // jr nz
  CYC(b_+375, b_+377);
  CYC(b_+377, b_+379); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+379, b_+380); A = C;
  CYC(b_+380, b_+381); mem_wr(gb, DE, A);
  CALL_C(b_+381, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+384);
  if (!(F & FZ)) { RET_TAKEN(b_+384); if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState; return; } // ret nz
  CYC(b_+384, b_+385);
  CYC(b_+385, b_+388); SET_HL((SYM(interactionCode67__label_0a_047) + 119)); // mainScripts.blueSnakeScript_successfulFortune
  CYC(b_+388, b_+390); goto setBlueSnakeExitScript; // jr

blueSnakeErrorCondition:
  CYC(b_+390, b_+393); SET_HL((SYM(interactionCode67__label_0a_047) + 67)); // mainScripts.blueSnakeScript_doNotRemoveCable
  CYC(b_+393, b_+394); A = E;
  CYC(b_+394, b_+396); alu_cp(gb, 0x8f);
  if (F & FZ) { CYCT(b_+396, b_+398); goto setBlueSnakeExitScript; } // jr z
  CYC(b_+396, b_+398);
  CYC(b_+398, b_+401); SET_HL((SYM(interactionCode67__label_0a_047) + 76)); // mainScripts.blueSnakeExitScript_noValidFile
  CYC(b_+401, b_+403); alu_cp(gb, 0x85);
  if (F & FZ) { CYCT(b_+403, b_+405); goto setBlueSnakeExitScript; } // jr z
  CYC(b_+403, b_+405);
  CYC(b_+405, b_+408); SET_HL((SYM(interactionCode67__label_0a_047) + 73)); // mainScripts.blueSnakeExitScript_linkFailed

setBlueSnakeExitScript:
  CYC(b_+408, b_+409); alu_xor(gb, A);
  CYC(b_+409, b_+412); W8(wDisabledObjects) = A;
  CYC(b_+412, b_+415); push_effect(gb, b_+415); goto setScriptAndGotoState4;
afterSetScriptFromBlueSnake:
  CYC(b_+415, b_+417); A = 0x02;
  CYC(b_+417, b_+420); interactionSetAnimation_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate3:
  CALL_C(b_+420, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+423);
  CYC(b_+423, b_+425); A = 0x08;
  CALL_C(b_+425, openMenu_hook, SYM(openMenu), b_+428);
  CYC(b_+428, b_+431); interactionIncSubstate_hook(gb);
  if (gb->pc == b_+19 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate4:
  CYC(b_+431, b_+433); A = hram_rd(gb, 0x70); // R_SVBK
  CYC(b_+433, b_+434); push_effect(gb, AF);
  CYC(b_+434, b_+436); A = 0x04; // :w4RingFortuneStuff
  CYC(b_+436, b_+438); hram_wr(gb, 0x70, A);
  CYC(b_+438, b_+440); A = H8(hFFBD);
  CYC(b_+440, b_+441); B = A;
  CYC(b_+441, b_+444); A = mem_rd(gb, wGenericCutscene_cbc2);
  CYC(b_+444, b_+445); E = A;
  CYC(b_+445, b_+446); SET_AF(pop_effect(gb));
  CYC(b_+446, b_+448); hram_wr(gb, 0x70, A);
  CYC(b_+448, b_+449); A = B;
  CYC(b_+449, b_+450); alu_or(gb, E);
  if (!(F & FZ)) { CYCT(b_+450, b_+452); goto blueSnakeErrorCondition; } // jr nz
  CYC(b_+450, b_+452);
  CYC(b_+452, b_+455); SET_HL((SYM(interactionCode67__label_0a_047) + 129)); // mainScripts.blueSnakeScript_successfulRingTransfer
  CYC(b_+455, b_+457); goto setBlueSnakeExitScript; // jr

checkRingBoxAndRingsObtained:
  CYC(b_+457, b_+459); A = 0x2c; // TREASURE_RING_BOX
  CALL_C(b_+459, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+462);
  CYC(b_+462, b_+464); A = 0x00;
  CYC(b_+464, b_+465); alu_rla(gb);
  CYC(b_+465, b_+467); E = INTERACTION_BASE + 0x36; // Interaction.var36
  CYC(b_+467, b_+468); mem_wr(gb, DE, A);
  CYC(b_+468, b_+471); A = W8(wNumUnappraisedRingsBcd);
  CYC(b_+471, b_+472); E = alu_inc8(gb, E);
  CYC(b_+472, b_+473); mem_wr(gb, DE, A);
  CYC(b_+473, b_+476); SET_HL(wRingsObtained);
  CYC(b_+476, b_+478); B = 0x08;
  CYC(b_+478, b_+479); alu_xor(gb, A);

nextRing:
  CYC(b_+479, b_+480); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+480, b_+481); L = alu_inc8(gb, L);
  CYC(b_+481, b_+482); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+482, b_+484); goto nextRing; } // jr nz
  CYC(b_+482, b_+484);
  CYC(b_+484, b_+485); E = alu_inc8(gb, E);
  CYC(b_+485, b_+486); mem_wr(gb, DE, A);
  RET(b_+486);
  if (gb->pc == b_+143 && gb->sp == sp1_) goto afterCheckRing_state2;
  if (gb->pc == b_+235 && gb->sp == sp1_) goto afterCheckRing_state4;
  return; // ret
}
