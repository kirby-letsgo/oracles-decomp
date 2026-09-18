#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  uint16_t sp1_;
  CYC(0x47b5, 0x47b8); A = W8(wTextIsActive);
  CYC(0x47b8, 0x47b9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x47b9, 0x47bb); goto afterTextboxSetup; } // jr nz
  CYC(0x47b9, 0x47bb);
  CYC(0x47bb, 0x47bd); A = 0x02;
  CYC(0x47bd, 0x47c0); W8(wTextboxPosition) = A;
  CYC(0x47c0, 0x47c2); A = 0x08; // TEXTBOXFLAG_DONTCHECKPOSITION
  CYC(0x47c2, 0x47c5); W8(wTextboxFlags) = A;

afterTextboxSetup:
  CYC(0x47c5, 0x47c8); push_effect(gb, 0x47c8); goto updateState;
afterUpdateState:
  CYC(0x47c8, 0x47ca); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x47ca, 0x47cb); A = mem_rd(gb, DE);
  CYC(0x47cb, 0x47cc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x47cc, 0x47cf); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; } // jp nz
  CYC(0x47cc, 0x47cf);
  CYC(0x47cf, 0x47d2); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; // jp

updateState:
  sp1_ = gb->sp;
  CYC(0x47d2, 0x47d4); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x47d4, 0x47d5); A = mem_rd(gb, DE);
  {
    CYC(0x47d5, 0x47d6); push_effect(gb, 0x47d6);
    uint16_t target = interactionCode89_jump_table(gb);
    if (target == 0x47e2) goto state0;
    if (target == 0x4814) goto state1;
    if (target == 0x4841) goto state2;
    if (target == 0x487f) goto state3;
    if (target == 0x489d) goto state4;
    goto state5;
  }

state0:
  CYC(0x47e2, 0x47e4); A = 0x01;
  CYC(0x47e4, 0x47e5); mem_wr(gb, DE, A);
  CALL_C(0x47e5, interactionInitGraphics_hook, 0x15fb, 0x47e8);
  CALL_C(0x47e8, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x47eb);
  CYC(0x47eb, 0x47ed); A = 0x30; // >TX_3000
  CALL_C(0x47ed, interactionSetHighTextIndex_hook, 0x253b, 0x47f0);
  CYC(0x47f0, 0x47f2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x47f2, 0x47f3); A = mem_rd(gb, DE);
  CYC(0x47f3, 0x47f4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x47f4, 0x47f6); goto state0_initVasu; } // jr z
  CYC(0x47f4, 0x47f6);

  // interactionCode89@state0@initSnake
  CYC(0x47f6, 0x47f8); A = 0x06;
  CALL_C(0x47f8, objectSetCollideRadius_hook, 0x24a1, 0x47fb);
  CALL_C(0x47fb, objectGetTileCollisions_hook, 0x14ad, 0x47fe);
  CYC(0x47fe, 0x4800); mem_wr(gb, HL, 0x0f);
  CYC(0x4800, 0x4801); A = mem_rd(gb, DE);
  CALL_C(0x4801, interactionSetAnimation_hook, 0x262e, 0x4804);
  CYC(0x4804, 0x4806); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x4806, 0x4809); objectAddToAButtonSensitiveObjectList_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state0_initVasu:
  CYC(0x4809, 0x480b); A = 0x04;
  CYC(0x480b, 0x480d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x480d, 0x480e); mem_wr(gb, DE, A);
  CYC(0x480e, 0x4811); SET_HL(0x49de); // mainScripts.vasuScript
  CYC(0x4811, 0x4814); interactionSetScript_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state1:
  CYC(0x4814, 0x4816); C = 0x18;
  CALL_C(0x4816, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x4819);
  CYC(0x4819, 0x481b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x481b, 0x481c); A = mem_rd(gb, DE);
  if (!(F & FC)) { CYCT(0x481c, 0x481f); interactionSetAnimation_hook(gb); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // jp nc
  CYC(0x481c, 0x481f);
  CALL_C(0x481f, interactionAnimate_hook, 0x261b, 0x4822);
  CYC(0x4822, 0x4823); H = D;
  CYC(0x4823, 0x4825); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x4825, 0x4826); A = mem_rd(gb, HL);
  CYC(0x4826, 0x4827); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x4827); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(0x4827, 0x4828);
  CYC(0x4828, 0x4829); alu_xor(gb, A);
  CYC(0x4829, 0x482a); mem_wr(gb, HL, A);
  CYC(0x482a, 0x482b); A = alu_inc8(gb, A);
  CYC(0x482b, 0x482e); W8(wMenuDisabled) = A;
  CYC(0x482e, 0x4831); W8(wDisabledObjects) = A;
  CYC(0x4831, 0x4832); E = L;
  CALL_C(0x4832, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x4835);
  CYC(0x4835, 0x4836); H = D;
  CYC(0x4836, 0x4838); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4838, 0x483a); A = 0x02;
  CYC(0x483a, 0x483b); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x483b, 0x483c); L = alu_dec8(gb, L);
  CYC(0x483c, 0x483d); A = mem_rd(gb, HL);
  CYC(0x483d, 0x483e); A = alu_inc8(gb, A);
  CYC(0x483e, 0x4841); interactionSetAnimation_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state2:
  CYC(0x4841, 0x4844); push_effect(gb, 0x4844); goto checkRingBoxAndRingsObtained;
afterCheckRing_state2:
  CALL_C(0x4844, interactionAnimate_hook, 0x261b, 0x4847);
  CYC(0x4847, 0x4849); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4849, 0x484a); A = mem_rd(gb, DE);
  CYC(0x484a, 0x484c); alu_and(gb, 0x04);
  CYC(0x484c, 0x484d); B = A;
  CYC(0x484d, 0x484f); C = 0x00;
  CYC(0x484f, 0x4851); E = INTERACTION_BASE + 0x36; // Interaction.var36
  CYC(0x4851, 0x4852); A = mem_rd(gb, DE);
  CYC(0x4852, 0x4853); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4853, 0x4855); goto loadPrelinkedScript; } // jr z
  CYC(0x4853, 0x4855);
  CYC(0x4855, 0x4857); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x4857, checkGlobalFlag_hook, 0x31f3, 0x485a);
  if (!(F & FZ)) { CYCT(0x485a, 0x485c); goto loadLinkedScript; } // jr nz
  CYC(0x485a, 0x485c);
  CYC(0x485c, 0x485f); SET_HL(wFileIsLinkedGame);
  CYC(0x485f, 0x4860); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4860, 0x4861); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4861, 0x4863); goto loadLinkedScript; } // jr nz
  CYC(0x4861, 0x4863);

loadPrelinkedScript:
  CYC(0x4863, 0x4865); C = 0x02;

loadLinkedScript:
  CYC(0x4865, 0x4866); A = B;
  CYC(0x4866, 0x4867); alu_add(gb, C);
  CYC(0x4867, 0x486a); SET_HL(0x4877); // interactionCode89@scriptTable
  CYC(0x486a, 0x486b); interactionCode89_addAToHl_from_rst(gb, 0x486b);
  CYC(0x486b, 0x486c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x486c, 0x486d); H = mem_rd(gb, HL);
  CYC(0x486d, 0x486e); L = A;

setScriptAndGotoState4:
  CALL_C(0x486e, interactionSetScript_hook, 0x2544, 0x4871);
  CYC(0x4871, 0x4873); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4873, 0x4875); A = 0x04;
  CYC(0x4875, 0x4876); mem_wr(gb, DE, A);
  RET(0x4876);
  if (gb->pc == 0x4954 && gb->sp == sp1_) goto afterSetScriptFromBlueSnake;
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // ret

state3:
  CALL_C(0x487f, interactionAnimate_hook, 0x261b, 0x4882);
  CYC(0x4882, 0x4884); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x4884, 0x4885); A = mem_rd(gb, DE);
  CYC(0x4885, 0x4886); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x4886); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(0x4886, 0x4887);
  CYC(0x4887, 0x4888); alu_xor(gb, A);
  CYC(0x4888, 0x488b); W8(wMenuDisabled) = A;
  CYC(0x488b, 0x488d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x488d, 0x488f); A = 0x01;
  CYC(0x488f, 0x4890); mem_wr(gb, DE, A);
  CYC(0x4890, 0x4892); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4892, 0x4893); A = mem_rd(gb, DE);
  CYC(0x4893, 0x4894); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x4894); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(0x4894, 0x4895);
  CALL_C(0x4895, interactionSetAnimation_hook, 0x262e, 0x4898);
  CYC(0x4898, 0x489a); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x489a, 0x489d); objectAddToAButtonSensitiveObjectList_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state4:
  CYC(0x489d, 0x48a0); push_effect(gb, 0x48a0); goto checkRingBoxAndRingsObtained;
afterCheckRing_state4:
  CALL_C(0x48a0, interactionAnimate_hook, 0x261b, 0x48a3);
  CALL_C(0x48a3, interactionRunScript_hook, 0x2552, 0x48a6);
  if (!(F & FC)) { RET_TAKEN(0x48a6); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret nc
  CYC(0x48a6, 0x48a7);
  CYC(0x48a7, 0x48a8); alu_xor(gb, A);
  CYC(0x48a8, 0x48ab); W8(wMenuDisabled) = A;
  CYC(0x48ab, 0x48ae); W8(wDisabledObjects) = A;
  CYC(0x48ae, 0x48b0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x48b0, 0x48b1); A = mem_rd(gb, DE);
  CYC(0x48b1, 0x48b2); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x48b2); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret z
  CYC(0x48b2, 0x48b3);
  CYC(0x48b3, 0x48b5); alu_add(gb, 0x02);
  CALL_C(0x48b5, interactionSetAnimation_hook, 0x262e, 0x48b8);
  CYC(0x48b8, 0x48ba); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x48ba, 0x48bc); A = 0x03;
  CYC(0x48bc, 0x48bd); mem_wr(gb, DE, A);
  RET(0x48bd);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // ret

state5:
  CALL_C(0x48be, interactionAnimate_hook, 0x261b, 0x48c1);
  CYC(0x48c1, 0x48c3); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x48c3, 0x48c4); A = mem_rd(gb, DE);
  {
    CYC(0x48c4, 0x48c5); push_effect(gb, 0x48c5);
    uint16_t target = interactionCode89_jump_table(gb);
    if (target == 0x48cf) goto state5Substate0;
    if (target == 0x48e2) goto state5Substate1;
    if (target == 0x490e) goto state5Substate2;
    if (target == 0x4959) goto state5Substate3;
    goto state5Substate4;
  }

state5Substate0:
  CALL_C(0x48cf, retIfTextIsActive_hook, 0x1859, 0x48d2);
  CALL_C(0x48d2, interactionIncSubstate_hook, 0x23e5, 0x48d5);
  CYC(0x48d5, 0x48d6); alu_xor(gb, A);
  CYC(0x48d6, 0x48d8); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x48d8, 0x48d9); mem_wr(gb, HL, A);
  CYC(0x48d9, 0x48db); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x48db, 0x48dd); mem_wr(gb, HL, 0x02);
  CYC(0x48dd, 0x48df); A = 0x04;
  CYC(0x48df, 0x48e2); interactionSetAnimation_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate1:
  CALL_C(0x48e2, interactionDecCounter1_hook, 0x23cc, 0x48e5);
  if (!(F & FZ)) { CYCT(0x48e5, 0x48e7); goto label_0a_036; } // jr nz
  CYC(0x48e5, 0x48e7);
  CYC(0x48e7, 0x48e8); L = alu_inc8(gb, L);
  CYC(0x48e8, 0x48e9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x48e9, 0x48eb); goto label_0a_036; } // jr nz
  CYC(0x48e9, 0x48eb);
  CYC(0x48eb, 0x48ec); alu_xor(gb, A);
  CYC(0x48ec, 0x48ee); mem_wr(gb, 0xff01, A); // R_SB
  CYC(0x48ee, 0x48f1); SET_HL(0x4ad5); // mainScripts.blueSnakeExitScript_cableNotConnected
  CYC(0x48f1, 0x48f3); B = 0x80;
  CYC(0x48f3, 0x48f5); goto setBlueSnakeExitScript; // jr

label_0a_036:
  CYC(0x48f5, 0x48f7); A = H8(hSerialInterruptBehaviour);
  CYC(0x48f7, 0x48f8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x48f8, 0x48fb); serialFunc_0c73_hook(gb); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // jp z
  CYC(0x48f8, 0x48fb);
  CYC(0x48fb, 0x48fd); alu_and(gb, 0x01);
  CYC(0x48fd, 0x48ff); alu_add(gb, 0x01);
  CYC(0x48ff, 0x4901); H8(hFFBE) = A;
  CALL_C(0x4901, interactionIncSubstate_hook, 0x23e5, 0x4904);
  CYC(0x4904, 0x4906); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4906, 0x4908); mem_wr(gb, HL, 180);
  CYC(0x4908, 0x490b); SET_BC(0x3030); // TX_3030
  CYC(0x490b, 0x490e); showTextNonExitable_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate2:
  CALL_C(0x490e, serialFunc_0c8d_hook, 0x0c8d, 0x4911);
  CYC(0x4911, 0x4913); A = H8(hSerialInterruptBehaviour);
  CYC(0x4913, 0x4914); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4914); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret nz
  CYC(0x4914, 0x4915);
  CYC(0x4915, 0x4917); A = hram_rd(gb, 0x70); // R_SVBK
  CYC(0x4917, 0x4918); push_effect(gb, AF);
  CYC(0x4918, 0x491a); A = 0x04; // :w4RingFortuneStuff
  CYC(0x491a, 0x491c); hram_wr(gb, 0x70, A);
  CYC(0x491c, 0x491e); A = H8(hFFBD);
  CYC(0x491e, 0x491f); B = A;
  CYC(0x491f, 0x4922); A = mem_rd(gb, 0xcbc2);
  CYC(0x4922, 0x4923); E = A;
  CYC(0x4923, 0x4926); A = W8(w4RingFortuneStuff);
  CYC(0x4926, 0x4927); C = A;
  CYC(0x4927, 0x4928); SET_AF(pop_effect(gb));
  CYC(0x4928, 0x492a); hram_wr(gb, 0x70, A);
  CYC(0x492a, 0x492b); A = B;
  CYC(0x492b, 0x492c); alu_or(gb, E);
  if (!(F & FZ)) { CYCT(0x492c, 0x492e); goto blueSnakeErrorCondition; } // jr nz
  CYC(0x492c, 0x492e);
  CYC(0x492e, 0x4930); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x4930, 0x4931); A = C;
  CYC(0x4931, 0x4932); mem_wr(gb, DE, A);
  CALL_C(0x4932, interactionDecCounter1_hook, 0x23cc, 0x4935);
  if (!(F & FZ)) { RET_TAKEN(0x4935); if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState; return; } // ret nz
  CYC(0x4935, 0x4936);
  CYC(0x4936, 0x4939); SET_HL(0x4b06); // mainScripts.blueSnakeScript_successfulFortune
  CYC(0x4939, 0x493b); goto setBlueSnakeExitScript; // jr

blueSnakeErrorCondition:
  CYC(0x493b, 0x493e); SET_HL(0x4ad2); // mainScripts.blueSnakeScript_doNotRemoveCable
  CYC(0x493e, 0x493f); A = E;
  CYC(0x493f, 0x4941); alu_cp(gb, 0x8f);
  if (F & FZ) { CYCT(0x4941, 0x4943); goto setBlueSnakeExitScript; } // jr z
  CYC(0x4941, 0x4943);
  CYC(0x4943, 0x4946); SET_HL(0x4adb); // mainScripts.blueSnakeExitScript_noValidFile
  CYC(0x4946, 0x4948); alu_cp(gb, 0x85);
  if (F & FZ) { CYCT(0x4948, 0x494a); goto setBlueSnakeExitScript; } // jr z
  CYC(0x4948, 0x494a);
  CYC(0x494a, 0x494d); SET_HL(0x4ad8); // mainScripts.blueSnakeExitScript_linkFailed

setBlueSnakeExitScript:
  CYC(0x494d, 0x494e); alu_xor(gb, A);
  CYC(0x494e, 0x4951); W8(wDisabledObjects) = A;
  CYC(0x4951, 0x4954); push_effect(gb, 0x4954); goto setScriptAndGotoState4;
afterSetScriptFromBlueSnake:
  CYC(0x4954, 0x4956); A = 0x02;
  CYC(0x4956, 0x4959); interactionSetAnimation_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate3:
  CALL_C(0x4959, retIfTextIsActive_hook, 0x1859, 0x495c);
  CYC(0x495c, 0x495e); A = 0x08;
  CALL_C(0x495e, openMenu_hook, 0x1ab0, 0x4961);
  CYC(0x4961, 0x4964); interactionIncSubstate_hook(gb);
  if (gb->pc == 0x47c8 && gb->sp == sp0_) goto afterUpdateState;
  return; // jp

state5Substate4:
  CYC(0x4964, 0x4966); A = hram_rd(gb, 0x70); // R_SVBK
  CYC(0x4966, 0x4967); push_effect(gb, AF);
  CYC(0x4967, 0x4969); A = 0x04; // :w4RingFortuneStuff
  CYC(0x4969, 0x496b); hram_wr(gb, 0x70, A);
  CYC(0x496b, 0x496d); A = H8(hFFBD);
  CYC(0x496d, 0x496e); B = A;
  CYC(0x496e, 0x4971); A = mem_rd(gb, 0xcbc2);
  CYC(0x4971, 0x4972); E = A;
  CYC(0x4972, 0x4973); SET_AF(pop_effect(gb));
  CYC(0x4973, 0x4975); hram_wr(gb, 0x70, A);
  CYC(0x4975, 0x4976); A = B;
  CYC(0x4976, 0x4977); alu_or(gb, E);
  if (!(F & FZ)) { CYCT(0x4977, 0x4979); goto blueSnakeErrorCondition; } // jr nz
  CYC(0x4977, 0x4979);
  CYC(0x4979, 0x497c); SET_HL(0x4b10); // mainScripts.blueSnakeScript_successfulRingTransfer
  CYC(0x497c, 0x497e); goto setBlueSnakeExitScript; // jr

checkRingBoxAndRingsObtained:
  CYC(0x497e, 0x4980); A = 0x2c; // TREASURE_RING_BOX
  CALL_C(0x4980, checkTreasureObtained_hook, 0x1748, 0x4983);
  CYC(0x4983, 0x4985); A = 0x00;
  CYC(0x4985, 0x4986); alu_rla(gb);
  CYC(0x4986, 0x4988); E = INTERACTION_BASE + 0x36; // Interaction.var36
  CYC(0x4988, 0x4989); mem_wr(gb, DE, A);
  CYC(0x4989, 0x498c); A = W8(wNumUnappraisedRingsBcd);
  CYC(0x498c, 0x498d); E = alu_inc8(gb, E);
  CYC(0x498d, 0x498e); mem_wr(gb, DE, A);
  CYC(0x498e, 0x4991); SET_HL(wRingsObtained);
  CYC(0x4991, 0x4993); B = 0x08;
  CYC(0x4993, 0x4994); alu_xor(gb, A);

nextRing:
  CYC(0x4994, 0x4995); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4995, 0x4996); L = alu_inc8(gb, L);
  CYC(0x4996, 0x4997); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x4997, 0x4999); goto nextRing; } // jr nz
  CYC(0x4997, 0x4999);
  CYC(0x4999, 0x499a); E = alu_inc8(gb, E);
  CYC(0x499a, 0x499b); mem_wr(gb, DE, A);
  RET(0x499b);
  if (gb->pc == 0x4844 && gb->sp == sp1_) goto afterCheckRing_state2;
  if (gb->pc == 0x48a0 && gb->sp == sp1_) goto afterCheckRing_state4;
  return; // ret
}
