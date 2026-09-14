#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Script pointer tables (bank $0c mainScripts.*), indexed by subid or game progress.
#define interactionCode3b_scriptTable_bank08 0x7892
#define interactionCode3b_subid1And2ScriptTable_bank08 0x78a0
#define interactionCode3b_subid3And4ScriptTable_bank08 0x78ac
#define interactionCode3b_subid5ScriptTable_bank08 0x78bc

// Cross-bank targets referenced by address only.
#define getGameProgress_1_bank09 0x552b
#define getGameProgress_2_bank09 0x5559
#define interactionOscillateXRandomly_bank08 0x5d87
#define villagerGalSubid07Script_bank0c 0x5c0d
#define genericNpcScript_bank0c 0x45f0
#define linkedGameNpcScript_bank0c 0x7ed9

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
  CYC(0x77fc, 0x77fe); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x77fe, 0x77ff); A = mem_rd(gb, DE);
  CYC(0x77ff, 0x7800); push_effect(gb, 0x7800);
  switch (femaleVillager_jumpTable(gb)) {
    case 0x7812: goto runSubid00;
    case 0x7870: goto runScriptAndAnimateFacingLink;
    case 0x7876: goto runSubid06;
    case 0x787f: goto runSubid07;
    default: HANDOFF(HL);
  }

runSubid00:
  // Cutscene where guy is struck by lightning in intro
  CYC(0x7812, 0x7814); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7814, 0x7815); A = mem_rd(gb, DE);
  CYC(0x7815, 0x7816); push_effect(gb, 0x7816);
  switch (femaleVillager_jumpTable(gb)) {
    case 0x7820: goto substate0;
    case 0x7832: goto substate1;
    case 0x7848: goto substate2;
    case 0x785a: goto substate3;
    case 0x7866: goto substate4;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x7820, 0x7823); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x7823, 0x7825); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7825, 0x7828); interactionAnimate_hook(gb); return;
  }
  CYC(0x7825, 0x7828);
  CALL_C(0x7828, interactionIncSubstate_hook, 0x23e5, 0x782b);
  CYC(0x782b, 0x782d); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x782d, 0x782e); A = mem_rd(gb, HL);
  CYC(0x782e, 0x7830); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7830, 0x7831); mem_wr(gb, HL, A);
  CYC(0x7831, 0x7832); ret_effect(gb);
  return;

substate1:
  // callab interactionOscillateXRandomly
  CYC(0x7832, 0x7835); SET_HL(interactionOscillateXRandomly_bank08);
  CYC(0x7835, 0x7837); E = 0x08;
  CALL_C(0x7837, interBankCall_hook, 0x008a, 0x783a);
  CYC(0x783a, 0x783d); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x783d, 0x783f); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x783f, 0x7840); ret_effect(gb); return;
  }
  CYC(0x783f, 0x7840);
  CALL_C(0x7840, interactionIncSubstate_hook, 0x23e5, 0x7843);
  CYC(0x7843, 0x7845); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7845, 0x7847); mem_wr(gb, HL, 0x1e);
  CYC(0x7847, 0x7848); ret_effect(gb);
  return;

substate2:
  CALL_C(0x7848, interactionDecCounter1_hook, 0x23cc, 0x784b);
  if (!(F & FZ)) {
    CYCT(0x784b, 0x784c); ret_effect(gb); return;
  }
  CYC(0x784b, 0x784c);
  CYC(0x784c, 0x784f); SET_BC(0xfe40); // -$1c0
  CALL_C(0x784f, objectSetSpeedZ_hook, 0x239d, 0x7852);
  CYC(0x7852, 0x7854); A = 0x53; // SND_JUMP
  CALL_C(0x7854, playSound_b00_hook, 0x0c98, 0x7857);
  CYC(0x7857, 0x785a); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(0x785a, 0x785c); C = 0x20;
  CALL_C(0x785c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x785f);
  if (!(F & FZ)) {
    CYCT(0x785f, 0x7860); ret_effect(gb); return;
  }
  CYC(0x785f, 0x7860);
  CALL_C(0x7860, interactionIncSubstate_hook, 0x23e5, 0x7863);
  CYC(0x7863, 0x7866); goto loadScript;

substate4:
  CALL_C(0x7866, interactionAnimate2Times_hook, 0x2752, 0x7869);
  CALL_C(0x7869, interactionRunScript_hook, 0x2552, 0x786c);
  if (!(F & FC)) {
    CYCT(0x786c, 0x786d); ret_effect(gb); return;
  }
  CYC(0x786c, 0x786d);
  CYC(0x786d, 0x7870); interactionDelete_hook(gb);
  return;

runScriptAndAnimateFacingLink:
  // Generic NPCs
  CALL_C(0x7870, interactionRunScript_hook, 0x2552, 0x7873);
  CYC(0x7873, 0x7876); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid06:
  // Linked game NPC
  CALL_C(0x7876, interactionRunScript_hook, 0x2552, 0x7879);
  if (F & FC) {
    CYCT(0x7879, 0x787c); interactionDelete_hook(gb); return;
  }
  CYC(0x7879, 0x787c);
  CYC(0x787c, 0x787f); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid07:
  // NPC in eyeglasses library (present)
  CALL_C(0x787f, interactionRunScript_hook, 0x2552, 0x7882);
  CYC(0x7882, 0x7885); interactionAnimateAsNpc_hook(gb);
  return;

loadScript:
  CYC(0x7885, 0x7887); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7887, 0x7888); A = mem_rd(gb, DE);
  CYC(0x7888, 0x788b); SET_HL(interactionCode3b_scriptTable_bank08);
  CYC(0x788b, 0x788c); femaleVillager_addDoubleIndex(gb, 0x788c);
  CYC(0x788c, 0x788d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x788d, 0x788e); H = mem_rd(gb, HL);
  CYC(0x788e, 0x788f); L = A;
  CYC(0x788f, 0x7892); interactionSetScript_hook(gb);
}

// @initSubid: per-subid initialization. Reached only by a static `call` from
// interactionCode3b@state0 (NOT HOOKED).
static void femaleVillager_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x7744, 0x7746); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7746, 0x7747); A = mem_rd(gb, DE);
  CYC(0x7747, 0x7748); push_effect(gb, 0x7748);
  switch (femaleVillager_jumpTable(gb)) {
    case 0x775a: goto initSubid00;
    case 0x775f: goto initSubid01;
    case 0x777e: goto initSubid03;
    case 0x779e: goto initSubid05;
    case 0x77ee: goto initSubid06;
    case 0x77c3: goto initSubid07;
    case 0x77df: goto initSubid08;
    default: HANDOFF(HL);
  }

initSubid00:
  CYC(0x775a, 0x775c); A = 0x01;
  CYC(0x775c, 0x775f); interactionSetAnimation_hook(gb);
  return;

initSubid01:
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(0x775f, 0x7762); SET_HL(getGameProgress_1_bank09);
  CYC(0x7762, 0x7764); E = 0x09;
  CALL_C(0x7764, interBankCall_hook, 0x008a, 0x7767);
  CYC(0x7767, 0x7769); C = 0x01;
  CYC(0x7769, 0x776a); alu_xor(gb, A);
  CALL_C(0x776a, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x776d);
  if (!(F & FZ)) {
    CYCT(0x776d, 0x7770); interactionDelete_hook(gb); return;
  }
  CYC(0x776d, 0x7770);
  CYC(0x7770, 0x7771); A = B;
  CYC(0x7771, 0x7774); SET_HL(interactionCode3b_subid1And2ScriptTable_bank08);
  CYC(0x7774, 0x7775); femaleVillager_addDoubleIndex(gb, 0x7775);
  CYC(0x7775, 0x7776); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7776, 0x7777); H = mem_rd(gb, HL);
  CYC(0x7777, 0x7778); L = A;
  CALL_C(0x7778, interactionSetScript_hook, 0x2544, 0x777b);
  CYC(0x777b, 0x777e); objectSetVisible82_hook(gb);
  return;

initSubid03:
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(0x777e, 0x7781); SET_HL(getGameProgress_2_bank09);
  CYC(0x7781, 0x7783); E = 0x09;
  CALL_C(0x7783, interBankCall_hook, 0x008a, 0x7786);
  CYC(0x7786, 0x7788); C = 0x03;
  CYC(0x7788, 0x778a); A = 0x01;
  CALL_C(0x778a, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x778d);
  if (!(F & FZ)) {
    CYCT(0x778d, 0x7790); interactionDelete_hook(gb); return;
  }
  CYC(0x778d, 0x7790);
  CYC(0x7790, 0x7791); A = B;
  CYC(0x7791, 0x7794); SET_HL(interactionCode3b_subid3And4ScriptTable_bank08);
  CYC(0x7794, 0x7795); femaleVillager_addDoubleIndex(gb, 0x7795);
  CYC(0x7795, 0x7796); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7796, 0x7797); H = mem_rd(gb, HL);
  CYC(0x7797, 0x7798); L = A;
  CALL_C(0x7798, interactionSetScript_hook, 0x2544, 0x779b);
  CYC(0x779b, 0x779e); objectSetVisible82_hook(gb);
  return;

initSubid05:
  CYC(0x779e, 0x77a0); A = 0x01;
  CYC(0x77a0, 0x77a2); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x77a2, 0x77a3); mem_wr(gb, DE, A);
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(0x77a3, 0x77a6); SET_HL(getGameProgress_2_bank09);
  CYC(0x77a6, 0x77a8); E = 0x09;
  CALL_C(0x77a8, interBankCall_hook, 0x008a, 0x77ab);
  CYC(0x77ab, 0x77ad); C = 0x05;
  CYC(0x77ad, 0x77af); A = 0x02;
  CALL_C(0x77af, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x77b2);
  if (!(F & FZ)) {
    CYCT(0x77b2, 0x77b5); interactionDelete_hook(gb); return;
  }
  CYC(0x77b2, 0x77b5);
  CYC(0x77b5, 0x77b6); A = B;
  CYC(0x77b6, 0x77b9); SET_HL(interactionCode3b_subid5ScriptTable_bank08);
  CYC(0x77b9, 0x77ba); femaleVillager_addDoubleIndex(gb, 0x77ba);
  CYC(0x77ba, 0x77bb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x77bb, 0x77bc); H = mem_rd(gb, HL);
  CYC(0x77bc, 0x77bd); L = A;
  CALL_C(0x77bd, interactionSetScript_hook, 0x2544, 0x77c0);
  CYC(0x77c0, 0x77c3); objectSetVisible82_hook(gb);
  return;

initSubid07:
  CYC(0x77c3, 0x77c5); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(0x77c5, checkGlobalFlag_hook, 0x31f3, 0x77c8);
  CYC(0x77c8, 0x77ca); A = 0x26; // <TX_1526
  if (F & FZ) {
    CYCT(0x77ca, 0x77cc);
  } else {
    CYC(0x77ca, 0x77cc);
    CYC(0x77cc, 0x77ce); A = 0x27; // <TX_1527
  }
  CYC(0x77ce, 0x77d0); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x77d0, 0x77d1); mem_wr(gb, DE, A);
  CYC(0x77d1, 0x77d2); E = alu_inc8(gb, E);
  CYC(0x77d2, 0x77d4); A = 0x15; // >TX_1500
  CYC(0x77d4, 0x77d5); mem_wr(gb, DE, A);
  CYC(0x77d5, 0x77d6); alu_xor(gb, A);
  CALL_C(0x77d6, interactionSetAnimation_hook, 0x262e, 0x77d9);
  CYC(0x77d9, 0x77dc); SET_HL(villagerGalSubid07Script_bank0c);
  CYC(0x77dc, 0x77df); interactionSetScript_hook(gb);
  return;

initSubid08:
  CYC(0x77df, 0x77e1); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x77e1, 0x77e3); A = 0x03; // <TX_0f03
  CYC(0x77e3, 0x77e4); mem_wr(gb, DE, A);
  CYC(0x77e4, 0x77e5); E = alu_inc8(gb, E);
  CYC(0x77e5, 0x77e7); A = 0x0f; // >TX_0f03
  CYC(0x77e7, 0x77e8); mem_wr(gb, DE, A);
  CYC(0x77e8, 0x77eb); SET_HL(genericNpcScript_bank0c);
  CYC(0x77eb, 0x77ee); interactionSetScript_hook(gb);
  return;

initSubid06:
  CYC(0x77ee, 0x77f0); A = 0x05;
  CYC(0x77f0, 0x77f2); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x77f2, 0x77f3); mem_wr(gb, DE, A);
  CYC(0x77f3, 0x77f6); SET_HL(linkedGameNpcScript_bank0c);
  CALL_C(0x77f6, interactionSetScript_hook, 0x2544, 0x77f9);
  CALL_C(0x77f9, interactionRunScript_hook, 0x2552, 0x77fc);
  femaleVillager_state1(gb, sp0_);
}

// INTERAC_FEMALE_VILLAGER: assorted female NPCs, subid-dispatched.
void interactionCode3b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7728, 0x772a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x772a, 0x772b); A = mem_rd(gb, DE);
  CYC(0x772b, 0x772c); push_effect(gb, 0x772c);
  switch (femaleVillager_jumpTable(gb)) {
    case 0x7730: goto state0;
    case 0x77fc: femaleVillager_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x7730, 0x7732); A = 0x01;
  CYC(0x7732, 0x7733); mem_wr(gb, DE, A);
  CALL_C(0x7733, interactionInitGraphics_hook, 0x15fb, 0x7736);
  CALL_C(0x7736, objectSetVisiblec2_hook, 0x1e45, 0x7739);
  CYC(0x7739, 0x773c); push_effect(gb, 0x773c); femaleVillager_initSubid(gb, sp0_);
  CYC(0x773c, 0x773e); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x773e, 0x773f); A = mem_rd(gb, DE);
  CYC(0x773f, 0x7740); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7740, 0x7743); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x7740, 0x7743);
  CYC(0x7743, 0x7744); ret_effect(gb);
}
