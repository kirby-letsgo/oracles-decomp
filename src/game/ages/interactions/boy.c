#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// boyRunSubid09@movementData: 3 runs (subids 8, 9, $0a) of {frames, angle} pairs, each
// terminated by a $00 byte (9 bytes per run).
#define boyMovementData_bank08 0x7cb3
// boyLoadScript@scriptTable: 17 mainScripts.boySubidNNScript pointers (bank $0c), indexed by subid.
#define boyLoadScriptTable_bank08 0x7e03
// boySubid02ScriptTable: 6 mainScripts.boySubid02Script_* pointers (bank $0c), indexed by
// game progress.
#define boySubid02ScriptTable_bank08 0x7e25
// agesInteractionsBank09.getGameProgress_1, reached through interBankCall.
#define getGameProgress_1_bank09 0x552b

void boyState1_hook(GB *gb);
void boyRunSubid00_hook(GB *gb);
void boyRunSubid01_hook(GB *gb);
void boyRunSubid02_hook(GB *gb);
void boyRunSubid03_hook(GB *gb);
void boyRunSubid04_hook(GB *gb);
void boyRunSubid05_hook(GB *gb);
void setCounter1To120AndPlaySoundEffectAndIncSubstate_hook(GB *gb);
void childSubid05Substate1_hook(GB *gb);
void childFlickerBetweenStone_hook(GB *gb);
void childAnimateIfVar39IsZeroAndRunScript_hook(GB *gb);
void boyRunSubid06_hook(GB *gb);
void boyRunSubid07_hook(GB *gb);
void boyRunSubid08_hook(GB *gb);
void boyRunSubid0a_hook(GB *gb);
void boyRunSubid0b_hook(GB *gb);
void boyRunSubid0c_hook(GB *gb);
void boyRunSubid0d_hook(GB *gb);
void boyRunSubid0e_hook(GB *gb);
void boyRunSubid0f_hook(GB *gb);
void loadStoneNpcPalette_hook(GB *gb);
void boyUpdateGravityAndHopWhenLanded_hook(GB *gb);
void boyStartHop_hook(GB *gb);
void boyLoadScript_hook(GB *gb);

static uint16_t boy_jumpTable(GB *gb) {
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

static void boy_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void boy_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode3c@saveXToVar3d: reached by a static `call` from @initSubid03 and by
// fallthrough from @initSubid04.
static void boy_saveXToVar3d(GB *gb) {
  CYC(0x7941, 0x7943); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x7943, 0x7944); A = mem_rd(gb, DE);
  CYC(0x7944, 0x7946); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7946, 0x7947); mem_wr(gb, DE, A);
  CYC(0x7947, 0x7948); ret_effect(gb);
}

// interactionCode3c@initSubid: per-subid initialization. Reached only by a static `call`
// from interactionCode3c's state 0.
static void boy_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x78e8, 0x78ea); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x78ea, 0x78eb); A = mem_rd(gb, DE);
  CYC(0x78eb, 0x78ec); push_effect(gb, 0x78ec);
  switch (boy_jumpTable(gb)) {
    case 0x79b1: goto initSubid00;
    case 0x7923: goto initSubid01;
    case 0x79b8: goto initSubid02;
    case 0x790e: goto initSubid03;
    case 0x7930: goto initSubid04;
    case 0x7948: goto initSubid05;
    case 0x795a: goto setStoneAnimationAndLoadScript;
    case 0x79d9: goto initSubid07;
    case 0x79e2: goto initSubid08;
    case 0x79ea: goto initSubid0b;
    case 0x7a03: goto initSubid0d;
    case 0x796a: goto initSubid0e;
    case 0x7a34: goto initSubid0f;
    case 0x7a2c: goto initSubid10;
    default: HANDOFF(HL);
  }

initSubid03:
  CALL_C(0x790e, getThisRoomFlags_hook, 0x197d, 0x7911);
  CYC(0x7911, 0x7913); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x7913, 0x7916); interactionDelete_hook(gb); return;
  }
  CYC(0x7913, 0x7916);
  CYC(0x7916, 0x7919); push_effect(gb, 0x7919); boy_saveXToVar3d(gb);
  CYC(0x7919, 0x791b); A = 0x01;
  CYC(0x791b, 0x791e); mem_wr(gb, wDisabledObjects, A);
  CYC(0x791e, 0x7921); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7921, 0x7923); goto setRedPaletteAndLoadScript;

initSubid01:
  CYC(0x7923, 0x7925); A = 0x01;
  CALL_C(0x7925, interactionSetAnimation_hook, 0x262e, 0x7928);

setRedPaletteAndLoadScript:
  CYC(0x7928, 0x792a); A = 0x02;
  CYC(0x792a, 0x792c); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x792c, 0x792d); mem_wr(gb, DE, A);
  CYC(0x792d, 0x7930); boyLoadScript_hook(gb);
  return;

initSubid04:
  CALL_C(0x7930, getThisRoomFlags_hook, 0x197d, 0x7933);
  CYC(0x7933, 0x7935); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x7935, 0x7938); interactionDelete_hook(gb); return;
  }
  CYC(0x7935, 0x7938);
  CYC(0x7938, 0x793a); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x793a, 0x793c); A = 0x3c;
  CYC(0x793c, 0x793d); mem_wr(gb, DE, A);
  CYC(0x793d, 0x793e); alu_xor(gb, A);
  CALL_C(0x793e, interactionSetAnimation_hook, 0x262e, 0x7941);
  boy_saveXToVar3d(gb);
  return;

initSubid05:
  CALL_C(0x7948, loadStoneNpcPalette_hook, 0x7de5, 0x794b);
  CYC(0x794b, 0x794d); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x794d, 0x794f); A = 0x06;
  CYC(0x794f, 0x7950); mem_wr(gb, DE, A);
  CYC(0x7950, 0x7952); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7952, 0x7954); A = 0x3c;
  CYC(0x7954, 0x7955); mem_wr(gb, DE, A);
  CYC(0x7955, 0x7957); A = 0x03;
  CYC(0x7957, 0x795a); interactionSetAnimation_hook(gb);
  return;

setStoneAnimationAndLoadScript:
  CYC(0x795a, 0x795c); A = 0x03;
  CALL_C(0x795c, interactionSetAnimation_hook, 0x262e, 0x795f);
  CYC(0x795f, 0x7961); A = 0x02;
  CYC(0x7961, 0x7963); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7963, 0x7964); mem_wr(gb, DE, A);
  CALL_C(0x7964, loadStoneNpcPalette_hook, 0x7de5, 0x7967);
  CYC(0x7967, 0x796a); boyLoadScript_hook(gb);
  return;

initSubid0e:
  // Was Veran defeated?
  CYC(0x796a, 0x796d); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x796d, 0x796f); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x796f, 0x7971); A = 0x1e; // <TX_251e
  if (!(F & FZ)) {
    CYCT(0x7971, 0x7973); goto initSubid0e_notStone;
  }
  CYC(0x7971, 0x7973);
  CYC(0x7973, 0x7976); A = mem_rd(gb, wEssencesObtained);
  CYC(0x7976, 0x7978); alu_bit(gb, 6, A);
  CYC(0x7978, 0x797a); A = 0x1d; // <TX_251d
  if (F & FZ) {
    CYCT(0x797a, 0x797c); goto initSubid0e_notStone;
  }
  CYC(0x797a, 0x797c);
  // Veran not defeated and d7 beaten: stand in front of his stone dad
  CALL_C(0x797c, objectUnmarkSolidPosition_hook, 0x2504, 0x797f);
  CYC(0x797f, 0x7982); SET_BC(0x4848);
  CALL_C(0x7982, interactionSetPosition_hook, 0x2773, 0x7985);
  CALL_C(0x7985, objectMarkSolidPosition_hook, 0x24f0, 0x7988);
  CYC(0x7988, 0x7989); H = D;
  CYC(0x7989, 0x798b); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x798b, 0x798c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x798c, 0x798e); A = 0x06;
  CALL_C(0x798e, objectSetCollideRadius_hook, 0x24a1, 0x7991);
  CYC(0x7991, 0x7993); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x7993, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x7996);
  CYC(0x7996, 0x7998); A = 0x1b; // <TX_251b
  CYC(0x7998, 0x799a); goto setTextIDAndLoadScript;

initSubid0e_notStone:
  CYC(0x799a, 0x799b); push_effect(gb, AF);
  CYC(0x799b, 0x799c); alu_xor(gb, A);
  CYC(0x799c, 0x799f); mem_wr(gb, wTmpcfc0 + 0x13, A);
  CYC(0x799f, 0x79a2); SET_BC(0x9500); // INTERAC_BALL, $00
  CALL_C(0x79a2, objectCreateInteraction_hook, 0x24c5, 0x79a5);
  CYC(0x79a5, 0x79a8); SET_BC(0x4a75);
  CALL_C(0x79a8, interactionHSetPosition_hook, 0x2774, 0x79ab);
  CYC(0x79ab, 0x79ac); SET_AF(pop_effect(gb));

setTextIDAndLoadScript:
  CYC(0x79ac, 0x79ae); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x79ae, 0x79af); mem_wr(gb, DE, A);
  CYC(0x79af, 0x79b1); goto setStoneAnimationAndLoadScript;

initSubid00:
  CYC(0x79b1, 0x79b2); alu_xor(gb, A);
  CALL_C(0x79b2, interactionSetAnimation_hook, 0x262e, 0x79b5);
  CYC(0x79b5, 0x79b8); boyLoadScript_hook(gb);
  return;

initSubid02:
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(0x79b8, 0x79bb); SET_HL(getGameProgress_1_bank09);
  CYC(0x79bb, 0x79bd); E = 0x09;
  CALL_C(0x79bd, interBankCall_hook, 0x008a, 0x79c0);
  CYC(0x79c0, 0x79c1); A = B;
  CYC(0x79c1, 0x79c2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x79c2, 0x79c4); goto initSubid02_lookupScript;
  }
  CYC(0x79c2, 0x79c4);
  // In the early game, the boy only exists once you've gotten the satchel
  CYC(0x79c4, 0x79c6); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(0x79c6, checkTreasureObtained_hook, 0x1748, 0x79c9);
  if (!(F & FC)) {
    CYCT(0x79c9, 0x79cc); interactionDelete_hook(gb); return;
  }
  CYC(0x79c9, 0x79cc);
  CYC(0x79cc, 0x79cd); alu_xor(gb, A);
initSubid02_lookupScript:
  CYC(0x79cd, 0x79d0); SET_HL(boySubid02ScriptTable_bank08);
  CYC(0x79d0, 0x79d1); boy_addDoubleIndex(gb, 0x79d1);
  CYC(0x79d1, 0x79d2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x79d2, 0x79d3); H = mem_rd(gb, HL);
  CYC(0x79d3, 0x79d4); L = A;
  CALL_C(0x79d4, interactionSetScript_hook, 0x2544, 0x79d7);
  CYC(0x79d7, 0x79d9); boyState1_hook(gb);
  return;

initSubid07:
  CYC(0x79d9, 0x79da); H = D;
  CYC(0x79da, 0x79dc); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x79dc, 0x79dd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x79dd, boyLoadScript_hook, 0x7df6, 0x79e0);
  CYC(0x79e0, 0x79e2); boyState1_hook(gb);
  return;

initSubid08:
  // Also @initSubid09
  CYC(0x79e2, 0x79e3); H = D;
  CYC(0x79e3, 0x79e5); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x79e5, 0x79e7); mem_wr(gb, HL, 0x78);
  CYC(0x79e7, 0x79ea); objectSetVisiblec1_hook(gb);
  return;

initSubid0b:
  CYC(0x79ea, 0x79eb); alu_xor(gb, A);
  CALL_C(0x79eb, interactionSetAnimation_hook, 0x262e, 0x79ee);
  CYC(0x79ee, 0x79f0); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(0x79f0, checkGlobalFlag_hook, 0x31f3, 0x79f3);
  CYC(0x79f3, 0x79f5); A = 0x19; // <TX_2519
  if (F & FZ) {
    CYCT(0x79f5, 0x79f7);
  } else {
    CYC(0x79f5, 0x79f7);
    CYC(0x79f7, 0x79f9); A = 0x1a; // <TX_251a
  }
  CYC(0x79f9, 0x79fb); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x79fb, 0x79fc); mem_wr(gb, DE, A);
  CYC(0x79fc, 0x79fd); E = alu_inc8(gb, E);
  CYC(0x79fd, 0x79ff); A = 0x25; // >TX_2500
  CYC(0x79ff, 0x7a00); mem_wr(gb, DE, A);
  CYC(0x7a00, 0x7a03); boyLoadScript_hook(gb);
  return;

initSubid0d:
  CYC(0x7a03, 0x7a05); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(0x7a05, checkGlobalFlag_hook, 0x31f3, 0x7a08);
  if (!(F & FZ)) {
    CYCT(0x7a08, 0x7a0a); goto initSubid0d_notStone;
  }
  CYC(0x7a08, 0x7a0a);
  CALL_C(0x7a0a, loadStoneNpcPalette_hook, 0x7de5, 0x7a0d);
  CYC(0x7a0d, 0x7a0e); H = D;
  CYC(0x7a0e, 0x7a10); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7a10, 0x7a12); mem_wr(gb, HL, 0x06);
  CYC(0x7a12, 0x7a14); A = 0x06;
  CALL_C(0x7a14, objectSetCollideRadius_hook, 0x24a1, 0x7a17);
  CYC(0x7a17, 0x7a19); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7a19, 0x7a1a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7a1a, 0x7a1c); A = 0x0c;
  CYC(0x7a1c, 0x7a1f); interactionSetAnimation_hook(gb);
  return;

initSubid0d_notStone:
  CYC(0x7a1f, 0x7a22); SET_BC(0x4868);
  CALL_C(0x7a22, interactionSetPosition_hook, 0x2773, 0x7a25);
  // Load red palette
  CYC(0x7a25, 0x7a27); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7a27, 0x7a29); mem_wr(gb, HL, 0x02);
  CYC(0x7a29, 0x7a2c); boyLoadScript_hook(gb);
  return;

initSubid10:
  CYC(0x7a2c, 0x7a2e); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x7a2e, checkGlobalFlag_hook, 0x31f3, 0x7a31);
  if (F & FZ) {
    CYCT(0x7a31, 0x7a34); interactionDelete_hook(gb); return;
  }
  CYC(0x7a31, 0x7a34);

initSubid0f:
  CYC(0x7a34, 0x7a37); boyLoadScript_hook(gb);
}

// boyRunSubid09@setAnimationFromAngle: reached by fallthrough from @substate0, a static
// `call` from @substate1 and `jp`s from @substate5 and @updateAngleAndCounter.
static void boy_setAnimationFromAngle(GB *gb, uint16_t sp0_) {
  CYC(0x7bcd, 0x7bcf); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7bcf, 0x7bd0); A = mem_rd(gb, DE);
  CALL_C(0x7bd0, convertAngleDeToDirection_hook, 0x26f8, 0x7bd3);
  CYC(0x7bd3, 0x7bd6); interactionSetAnimation_hook(gb);
}

// boyRunSubid09@updateAnimationTwiceAndApplySpeed: reached only by static `call`s from
// substates 1, 5 (val0), 7 and $0b.
static void boy_updateAnimationTwiceAndApplySpeed(GB *gb, uint16_t sp0_) {
  CALL_C(0x7bef, interactionAnimate2Times_hook, 0x2752, 0x7bf2);
  CYC(0x7bf2, 0x7bf5); objectApplySpeed_hook(gb);
}

// boyRunSubid09@updateAngleAndCounter: load the next {frames, angle} pair from
// @movementData for this subid. Reached by a static `call` from @substate4 and by
// fallthrough from @substate5@val2.
static void boy_updateAngleAndCounter(GB *gb, uint16_t sp0_) {
  CYC(0x7c7b, 0x7c7d); E = INTERACTION_BASE + OBJ_ID;
  CYC(0x7c7d, 0x7c7e); A = mem_rd(gb, DE);
  CYC(0x7c7e, 0x7c80); alu_cp(gb, 0x3c); // INTERAC_BOY
  if (F & FZ) {
    CYCT(0x7c80, 0x7c82); goto boy;
  }
  CYC(0x7c80, 0x7c82);
  // Which interaction is this for?
  CYC(0x7c82, 0x7c84); A = 0x02;
  CYC(0x7c84, 0x7c86); goto times9;
boy:
  CYC(0x7c86, 0x7c88); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7c88, 0x7c89); A = mem_rd(gb, DE);
  CYC(0x7c89, 0x7c8b); alu_sub(gb, 0x08);
times9:
  CYC(0x7c8b, 0x7c8c); B = A;
  CYC(0x7c8c, 0x7c8e); A = alu_swap(gb, A);
  CYC(0x7c8e, 0x7c90); A = alu_sra(gb, A);
  CYC(0x7c90, 0x7c91); alu_add(gb, B);
  CYC(0x7c91, 0x7c94); SET_HL(boyMovementData_bank08);
  CYC(0x7c94, 0x7c95); push_effect(gb, 0x7c95); boy_addAToHl(gb);
  CYC(0x7c95, 0x7c97); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7c97, 0x7c98); A = mem_rd(gb, DE);
  CYC(0x7c98, 0x7c99); boy_addDoubleIndex(gb, 0x7c99);
  CYC(0x7c99, 0x7c9a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7c9a, 0x7c9b); B = mem_rd(gb, HL);
  CYC(0x7c9b, 0x7c9c); L = alu_inc8(gb, L);
  CYC(0x7c9c, 0x7c9e); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c9e, 0x7c9f); mem_wr(gb, DE, A);
  CYC(0x7c9f, 0x7ca1); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7ca1, 0x7ca2); A = B;
  CYC(0x7ca2, 0x7ca3); mem_wr(gb, DE, A);
  CYC(0x7ca3, 0x7ca5); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7ca5, 0x7ca6); A = mem_rd(gb, DE);
  CYC(0x7ca6, 0x7ca7); B = A;
  CYC(0x7ca7, 0x7ca8); B = alu_inc8(gb, B);
  CYC(0x7ca8, 0x7ca9); A = mem_rd(gb, HL);
  CYC(0x7ca9, 0x7caa); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7caa, 0x7cac);
  } else {
    CYC(0x7caa, 0x7cac);
    CYC(0x7cac, 0x7cae); B = 0x00;
  }
  CYC(0x7cae, 0x7caf); A = B;
  CYC(0x7caf, 0x7cb0); mem_wr(gb, DE, A);
  CYC(0x7cb0, 0x7cb3); boy_setAnimationFromAngle(gb, sp0_);
}

// INTERAC_BOY
void interactionCode3c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78cc, 0x78ce); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x78ce, 0x78cf); A = mem_rd(gb, DE);
  CYC(0x78cf, 0x78d0); push_effect(gb, 0x78d0);
  switch (boy_jumpTable(gb)) {
    case 0x78d4: goto state0;
    case 0x7a37: boyState1_hook(gb); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x78d4, 0x78d6); A = 0x01;
  CYC(0x78d6, 0x78d7); mem_wr(gb, DE, A);
  CALL_C(0x78d7, interactionInitGraphics_hook, 0x15fb, 0x78da);
  CALL_C(0x78da, objectSetVisiblec2_hook, 0x1e45, 0x78dd);
  CYC(0x78dd, 0x78e0); push_effect(gb, 0x78e0); boy_initSubid(gb, sp0_);
  CYC(0x78e0, 0x78e2); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x78e2, 0x78e3); A = mem_rd(gb, DE);
  CYC(0x78e3, 0x78e4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x78e4, 0x78e7); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x78e4, 0x78e7);
  CYC(0x78e7, 0x78e8); ret_effect(gb);
}

void boyState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a37, 0x7a39); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7a39, 0x7a3a); A = mem_rd(gb, DE);
  CYC(0x7a3a, 0x7a3b); push_effect(gb, 0x7a3b);
  switch (boy_jumpTable(gb)) {
    case 0x7a5d: boyRunSubid00_hook(gb); return;
    case 0x7abb: boyRunSubid01_hook(gb); return;
    case 0x7b00: boyRunSubid02_hook(gb); return;
    case 0x7b06: boyRunSubid03_hook(gb); return;
    case 0x7b23: boyRunSubid04_hook(gb); return;
    case 0x7b46: boyRunSubid05_hook(gb); return;
    case 0x7b87: boyRunSubid06_hook(gb); return;
    case 0x7b90: boyRunSubid07_hook(gb); return;
    case 0x7ba0: boyRunSubid08_hook(gb); return;
    case 0x7d3f: boyRunSubid0a_hook(gb); return;
    case 0x7d45: boyRunSubid0b_hook(gb); return;
    case 0x7d4b: boyRunSubid0c_hook(gb); return;
    case 0x7dab: boyRunSubid0d_hook(gb); return;
    case 0x7db8: boyRunSubid0e_hook(gb); return;
    case 0x7dd9: boyRunSubid0f_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// Watching Nayru sing in intro
void boyRunSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a5d, interactionAnimate_hook, 0x261b, 0x7a60);
  CALL_C(0x7a60, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x7a63);
  CYC(0x7a63, 0x7a65); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7a65, 0x7a66); A = mem_rd(gb, DE);
  CYC(0x7a66, 0x7a67); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x7a67, objectPreventLinkFromPassing_hook, 0x2680, 0x7a6a);
  } else {
    CYC(0x7a67, 0x7a6a);
  }
  CYC(0x7a6a, 0x7a6c); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7a6c, 0x7a6d); A = mem_rd(gb, DE);
  CYC(0x7a6d, 0x7a6e); push_effect(gb, 0x7a6e);
  switch (boy_jumpTable(gb)) {
    case 0x7a76: goto substate0;
    case 0x7a86: goto substate1;
    case 0x7a9d: goto substate2;
    case 0x7ab2: goto substate3;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x7a76, 0x7a79); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7a79, 0x7a7b); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x7a7b, 0x7a7e); interactionRunScript_hook(gb); return;
  }
  CYC(0x7a7b, 0x7a7e);
  CALL_C(0x7a7e, interactionIncSubstate_hook, 0x23e5, 0x7a81);
  CYC(0x7a81, 0x7a83); A = 0x02;
  CYC(0x7a83, 0x7a86); interactionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x7a86, interactionAnimate_hook, 0x261b, 0x7a89);
  CYC(0x7a89, 0x7a8c); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7a8c, 0x7a8e); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x7a8e, 0x7a8f); ret_effect(gb); return;
  }
  CYC(0x7a8e, 0x7a8f);
  CALL_C(0x7a8f, interactionIncSubstate_hook, 0x23e5, 0x7a92);
  CYC(0x7a92, 0x7a95); SET_BC(0xfe80); // -$180
  CALL_C(0x7a95, objectSetSpeedZ_hook, 0x239d, 0x7a98);
  CYC(0x7a98, 0x7a9a); A = 0x02;
  CYC(0x7a9a, 0x7a9d); interactionSetAnimation_hook(gb);
  return;

substate2:
  CYC(0x7a9d, 0x7a9f); C = 0x20;
  CALL_C(0x7a9f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7aa2);
  if (!(F & FZ)) {
    CYCT(0x7aa2, 0x7aa3); ret_effect(gb); return;
  }
  CYC(0x7aa2, 0x7aa3);
  // Run away
  CALL_C(0x7aa3, interactionIncSubstate_hook, 0x23e5, 0x7aa6);
  CYC(0x7aa6, 0x7aa8); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7aa8, 0x7aaa); mem_wr(gb, HL, 0x02);
  CYC(0x7aaa, 0x7aac); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7aac, 0x7aae); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x7aae, 0x7aaf); alu_xor(gb, A);
  CYC(0x7aaf, 0x7ab2); interactionSetAnimation_hook(gb);
  return;

substate3:
  CALL_C(0x7ab2, objectCheckWithinScreenBoundary_hook, 0x2184, 0x7ab5);
  if (!(F & FC)) {
    CYCT(0x7ab5, 0x7ab8); interactionDelete_hook(gb); return;
  }
  CYC(0x7ab5, 0x7ab8);
  CYC(0x7ab8, 0x7abb); objectApplySpeed_hook(gb);
}

// Kid turning to stone cutscene
void boyRunSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7abb, 0x7abd); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7abd, 0x7abe); A = mem_rd(gb, DE);
  CYC(0x7abe, 0x7abf); push_effect(gb, 0x7abf);
  switch (boy_jumpTable(gb)) {
    case 0x7ac5: goto substate0;
    case 0x7ada: goto substate1;
    case 0x7af9: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x7ac5, interactionRunScript_hook, 0x2552, 0x7ac8);
  CYC(0x7ac8, 0x7acb); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x7acb, 0x7acd); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x7acd, 0x7acf); goto substate0_animate;
  }
  CYC(0x7acd, 0x7acf);
  CYC(0x7acf, 0x7ad2); interactionIncSubstate_hook(gb);
  return;
substate0_animate:
  CYC(0x7ad2, 0x7ad4); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7ad4, 0x7ad5); A = mem_rd(gb, DE);
  CYC(0x7ad5, 0x7ad6); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7ad6, 0x7ad7); ret_effect(gb); return;
  }
  CYC(0x7ad6, 0x7ad7);
  CYC(0x7ad7, 0x7ada); interactionAnimate2Times_hook(gb);
  return;

substate1:
  CALL_C(0x7ada, interactionRunScript_hook, 0x2552, 0x7add);
  CYC(0x7add, 0x7ae0); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x7ae0, 0x7ae2); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7ae2, 0x7ae4); goto substate1_flicker;
  }
  CYC(0x7ae2, 0x7ae4);
  CALL_C(0x7ae4, interactionIncSubstate_hook, 0x23e5, 0x7ae7);
  CYC(0x7ae7, 0x7ae9); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7ae9, 0x7aeb); mem_wr(gb, HL, 0x06);
  CYC(0x7aeb, 0x7aec); ret_effect(gb);
  return;
substate1_flicker:
  // Flicker palette from red to stone every 8 frames
  CYC(0x7aec, 0x7aef); A = mem_rd(gb, wFrameCounter);
  CYC(0x7aef, 0x7af1); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x7af1, 0x7af2); ret_effect(gb); return;
  }
  CYC(0x7af1, 0x7af2);
  CYC(0x7af2, 0x7af4); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7af4, 0x7af5); A = mem_rd(gb, DE);
  CYC(0x7af5, 0x7af7); alu_xor(gb, 0x04);
  CYC(0x7af7, 0x7af8); mem_wr(gb, DE, A);
  CYC(0x7af8, 0x7af9); ret_effect(gb);
  return;

substate2:
  CALL_C(0x7af9, interactionRunScript_hook, 0x2552, 0x7afc);
  if (!(F & FC)) {
    CYCT(0x7afc, 0x7aff); interactionAnimate_hook(gb); return;
  }
  CYC(0x7afc, 0x7aff);
  CYC(0x7aff, 0x7b00); ret_effect(gb);
}

// Kid outside shop
void boyRunSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b00, interactionRunScript_hook, 0x2552, 0x7b03);
  CYC(0x7b03, 0x7b06); npcFaceLinkAndAnimate_hook(gb);
}

// Cutscene where kids talk about how they're scared of a ghost (red kid). Also a substate 2
// jump-table target of boyRunSubid04.
void boyRunSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b06, interactionRunScript_hook, 0x2552, 0x7b09);
  CYC(0x7b09, 0x7b0b); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x7b0b, 0x7b0c); A = mem_rd(gb, DE);
  CYC(0x7b0c, 0x7b0d); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x7b0d, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7b10);
  } else {
    CYC(0x7b0d, 0x7b10);
  }
  CALL_C(0x7b10, objectCheckWithinScreenBoundary_hook, 0x2184, 0x7b13);
  if (F & FC) {
    CYCT(0x7b13, 0x7b14); ret_effect(gb); return;
  }
  CYC(0x7b13, 0x7b14);
  CYC(0x7b14, 0x7b15); alu_xor(gb, A);
  CYC(0x7b15, 0x7b18); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7b18, 0x7b1b); mem_wr(gb, wMenuDisabled, A);
  CALL_C(0x7b1b, getThisRoomFlags_hook, 0x197d, 0x7b1e);
  CYC(0x7b1e, 0x7b20); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(0x7b20, 0x7b23); interactionDelete_hook(gb);
}

// Cutscene where kids talk about how they're scared of a ghost (green kid)
void boyRunSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b23, 0x7b25); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7b25, 0x7b26); A = mem_rd(gb, DE);
  CYC(0x7b26, 0x7b27); push_effect(gb, 0x7b27);
  switch (boy_jumpTable(gb)) {
    case 0x7b2d: goto substate0;
    case 0x7b3a: goto substate1;
    case 0x7b06: boyRunSubid03_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x7b2d, interactionAnimate_hook, 0x261b, 0x7b30);
  CALL_C(0x7b30, interactionDecCounter1_hook, 0x23cc, 0x7b33);
  if (!(F & FZ)) {
    CYCT(0x7b33, 0x7b34); ret_effect(gb); return;
  }
  CYC(0x7b33, 0x7b34);
  CALL_C(0x7b34, interactionIncSubstate_hook, 0x23e5, 0x7b37);
  CYC(0x7b37, 0x7b3a); startJump_hook(gb);
  return;

substate1:
  CYC(0x7b3a, 0x7b3c); C = 0x20;
  CALL_C(0x7b3c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7b3f);
  if (!(F & FZ)) {
    CYCT(0x7b3f, 0x7b40); ret_effect(gb); return;
  }
  CYC(0x7b3f, 0x7b40);
  CALL_C(0x7b40, interactionIncSubstate_hook, 0x23e5, 0x7b43);
  CYC(0x7b43, 0x7b46); boyLoadScript_hook(gb);
}

// Cutscene where kid is restored from stone. Substate 0 falls through into
// setCounter1To120AndPlaySoundEffectAndIncSubstate.
void boyRunSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b46, 0x7b48); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7b48, 0x7b49); A = mem_rd(gb, DE);
  CYC(0x7b49, 0x7b4a); push_effect(gb, 0x7b4a);
  switch (boy_jumpTable(gb)) {
    case 0x7b50: goto substate0;
    case 0x7b61: childSubid05Substate1_hook(gb); return;
    case 0x7b7d: childAnimateIfVar39IsZeroAndRunScript_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x7b50, interactionDecCounter1_hook, 0x23cc, 0x7b53);
  if (!(F & FZ)) {
    CYCT(0x7b53, 0x7b54); ret_effect(gb); return;
  }
  CYC(0x7b53, 0x7b54);
  setCounter1To120AndPlaySoundEffectAndIncSubstate_hook(gb);
}

// Used in cutscenes where people get restored from stone?
void setCounter1To120AndPlaySoundEffectAndIncSubstate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b54, 0x7b56); A = 120;
  CYC(0x7b56, 0x7b58); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7b58, 0x7b59); mem_wr(gb, DE, A);
  CYC(0x7b59, 0x7b5b); A = 0x5c; // SND_ENERGYTHING
  CALL_C(0x7b5b, playSound_b00_hook, 0x0c98, 0x7b5e);
  CYC(0x7b5e, 0x7b61); interactionIncSubstate_hook(gb);
}

void childSubid05Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b61, interactionDecCounter1_hook, 0x23cc, 0x7b64);
  if (!(F & FZ)) {
    CYCT(0x7b64, 0x7b66); childFlickerBetweenStone_hook(gb); return;
  }
  CYC(0x7b64, 0x7b66);
  CALL_C(0x7b66, interactionIncSubstate_hook, 0x23e5, 0x7b69);
  CYC(0x7b69, 0x7b6b); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7b6b, 0x7b6d); mem_wr(gb, HL, 0x02);
  CYC(0x7b6d, 0x7b70); boyLoadScript_hook(gb);
}

// Called from other interactions as well?
void childFlickerBetweenStone_hook(GB *gb) {
  CYC(0x7b70, 0x7b73); A = mem_rd(gb, wFrameCounter);
  CYC(0x7b73, 0x7b75); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x7b75, 0x7b76); ret_effect(gb); return;
  }
  CYC(0x7b75, 0x7b76);
  CYC(0x7b76, 0x7b78); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7b78, 0x7b79); A = mem_rd(gb, DE);
  CYC(0x7b79, 0x7b7b); alu_xor(gb, 0x04);
  CYC(0x7b7b, 0x7b7c); mem_wr(gb, DE, A);
  CYC(0x7b7c, 0x7b7d); ret_effect(gb);
}

void childAnimateIfVar39IsZeroAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b7d, 0x7b7f); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x7b7f, 0x7b80); A = mem_rd(gb, DE);
  CYC(0x7b80, 0x7b81); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x7b81, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7b84);
  } else {
    CYC(0x7b81, 0x7b84);
  }
  CYC(0x7b84, 0x7b87); interactionRunScript_hook(gb);
}

// Cutscene where kid sees his dad turn to stone
void boyRunSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b87, checkInteractionSubstate_hook, 0x2403, 0x7b8a);
  if (!(F & FZ)) {
    CALL_C_CC(0x7b8a, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7b8d);
  } else {
    CYC(0x7b8a, 0x7b8d);
  }
  CYC(0x7b8d, 0x7b90); interactionRunScript_hook(gb);
}

// Depressed kid in trade sequence
void boyRunSubid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b90, interactionRunScript_hook, 0x2552, 0x7b93);
  CYC(0x7b93, 0x7b95); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7b95, 0x7b96); A = mem_rd(gb, DE);
  CYC(0x7b96, 0x7b97); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7b97, 0x7b9a); npcFaceLinkAndAnimate_hook(gb); return;
  }
  CYC(0x7b97, 0x7b9a);
  CALL_C(0x7b9a, interactionAnimate_hook, 0x261b, 0x7b9d);
  CYC(0x7b9d, 0x7ba0); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// Subids $08/$09 (and $0a's movement data): kid who runs around in a pattern.
void boyRunSubid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ba0, 0x7ba2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7ba2, 0x7ba3); A = mem_rd(gb, DE);
  CYC(0x7ba3, 0x7ba4); push_effect(gb, 0x7ba4);
  switch (boy_jumpTable(gb)) {
    case 0x7bbc: goto substate0;
    case 0x7bd6: goto substate1;
    case 0x7bf5: goto substate2;
    case 0x7bff: goto substate3;
    case 0x7c15: goto substate4;
    case 0x7c23: goto substate5;
    case 0x7cce: goto substate6;
    case 0x7ce5: goto substate7;
    case 0x7cfb: goto substate8;
    case 0x7d0b: goto substate9;
    case 0x7d21: goto substateA;
    case 0x7d35: goto substateB;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x7bbc, interactionDecCounter1_hook, 0x23cc, 0x7bbf);
  if (!(F & FZ)) {
    CYCT(0x7bbf, 0x7bc0); ret_effect(gb); return;
  }
  CYC(0x7bbf, 0x7bc0);
  CYC(0x7bc0, 0x7bc2); mem_wr(gb, HL, 0x66);
  CYC(0x7bc2, 0x7bc4); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7bc4, 0x7bc6); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(0x7bc6, 0x7bc8); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7bc8, 0x7bca); mem_wr(gb, HL, 0x18);
  CALL_C(0x7bca, interactionIncSubstate_hook, 0x23e5, 0x7bcd);
  boy_setAnimationFromAngle(gb, sp0_);
  return;

substate1:
  CYC(0x7bd6, 0x7bd9); push_effect(gb, 0x7bd9); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(0x7bd9, interactionDecCounter1_hook, 0x23cc, 0x7bdc);
  if (!(F & FZ)) {
    CYCT(0x7bdc, 0x7bdd); ret_effect(gb); return;
  }
  CYC(0x7bdc, 0x7bdd);
  CALL_C(0x7bdd, getRandomNumber_hook, 0x043e, 0x7be0);
  CYC(0x7be0, 0x7be2); alu_and(gb, 0x0f);
  CYC(0x7be2, 0x7be4); alu_add(gb, 0x1e);
  CYC(0x7be4, 0x7be5); mem_wr(gb, HL, A);
  CYC(0x7be5, 0x7be7); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7be7, 0x7be9); mem_wr(gb, HL, 0x08);
  CYC(0x7be9, 0x7bec); push_effect(gb, 0x7bec); boy_setAnimationFromAngle(gb, sp0_);
  CYC(0x7bec, 0x7bef); interactionIncSubstate_hook(gb);
  return;

substate2:
  CALL_C(0x7bf5, interactionDecCounter1_hook, 0x23cc, 0x7bf8);
  if (!(F & FZ)) {
    CYCT(0x7bf8, 0x7bf9); ret_effect(gb); return;
  }
  CYC(0x7bf8, 0x7bf9);
  CALL_C(0x7bf9, boyStartHop_hook, 0x7df0, 0x7bfc);
  CYC(0x7bfc, 0x7bff); interactionIncSubstate_hook(gb);
  return;

substate3:
  CALL_C(0x7bff, boyUpdateGravityAndHopWhenLanded_hook, 0x7dea, 0x7c02);
  CYC(0x7c02, 0x7c05); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7c05, 0x7c07); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x7c07, 0x7c08); ret_effect(gb); return;
  }
  CYC(0x7c07, 0x7c08);
  CYC(0x7c08, 0x7c0a); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7c0a, 0x7c0b); A = mem_rd(gb, DE);
  CYC(0x7c0b, 0x7c0c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7c0c, 0x7c0d); ret_effect(gb); return;
  }
  CYC(0x7c0c, 0x7c0d);
  CALL_C(0x7c0d, interactionIncSubstate_hook, 0x23e5, 0x7c10);
  CYC(0x7c10, 0x7c12); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c12, 0x7c14); mem_wr(gb, HL, 0x1e);
  CYC(0x7c14, 0x7c15); ret_effect(gb);
  return;

substate4:
  CALL_C(0x7c15, interactionDecCounter1_hook, 0x23cc, 0x7c18);
  if (!(F & FZ)) {
    CYCT(0x7c18, 0x7c19); ret_effect(gb); return;
  }
  CYC(0x7c18, 0x7c19);
  CYC(0x7c19, 0x7c1b); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7c1b, 0x7c1d); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x7c1d, 0x7c20); push_effect(gb, 0x7c20); boy_updateAngleAndCounter(gb, sp0_);
  CYC(0x7c20, 0x7c23); interactionIncSubstate_hook(gb);
  return;

substate5:
  CYC(0x7c23, 0x7c26); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7c26, 0x7c28); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7c28, 0x7c2a); goto substate5_dispatchVar37;
  }
  CYC(0x7c28, 0x7c2a);
  CYC(0x7c2a, 0x7c2c); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7c2c, 0x7c2d); A = mem_rd(gb, DE);
  CYC(0x7c2d, 0x7c2e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7c2e, 0x7c30); goto substate5_dispatchVar37;
  }
  CYC(0x7c2e, 0x7c30);
  CALL_C(0x7c30, interactionIncSubstate_hook, 0x23e5, 0x7c33);
  CYC(0x7c33, 0x7c35); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c35, 0x7c37); mem_wr(gb, HL, 0x0a);
  CYC(0x7c37, 0x7c39); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7c39, 0x7c3b); mem_wr(gb, HL, 0x18);
  CYC(0x7c3b, 0x7c3e); boy_setAnimationFromAngle(gb, sp0_);
  return;
substate5_dispatchVar37:
  CYC(0x7c3e, 0x7c40); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x7c40, 0x7c41); A = mem_rd(gb, DE);
  CYC(0x7c41, 0x7c42); push_effect(gb, 0x7c42);
  switch (boy_jumpTable(gb)) {
    case 0x7c48: goto substate5_val0;
    case 0x7c5f: goto substate5_val1;
    case 0x7c6b: goto substate5_val2;
    default: HANDOFF(HL);
  }

substate5_val0:
  CYC(0x7c48, 0x7c4b); push_effect(gb, 0x7c4b); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(0x7c4b, interactionDecCounter1_hook, 0x23cc, 0x7c4e);
  if (!(F & FZ)) {
    CYCT(0x7c4e, 0x7c4f); ret_effect(gb); return;
  }
  CYC(0x7c4e, 0x7c4f);
  CYC(0x7c4f, 0x7c51); mem_wr(gb, HL, 0x0a);
  CYC(0x7c51, 0x7c53); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x7c53, 0x7c54); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c54, 0x7c56); alu_cp(gb, 0x68);
  CYC(0x7c56, 0x7c58); A = 0x01;
  if (F & FC) {
    CYCT(0x7c58, 0x7c5a);
  } else {
    CYC(0x7c58, 0x7c5a);
    CYC(0x7c5a, 0x7c5c); A = 0x03;
  }
  CYC(0x7c5c, 0x7c5f); interactionSetAnimation_hook(gb);
  return;

substate5_val1:
  CALL_C(0x7c5f, interactionDecCounter1_hook, 0x23cc, 0x7c62);
  if (!(F & FZ)) {
    CYCT(0x7c62, 0x7c63); ret_effect(gb); return;
  }
  CYC(0x7c62, 0x7c63);
  CYC(0x7c63, 0x7c65); mem_wr(gb, HL, 0x1e);
  CYC(0x7c65, 0x7c67); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x7c67, 0x7c68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c68, 0x7c6b); boyStartHop_hook(gb);
  return;

substate5_val2:
  CALL_C(0x7c6b, boyUpdateGravityAndHopWhenLanded_hook, 0x7dea, 0x7c6e);
  CALL_C(0x7c6e, interactionDecCounter1_hook, 0x23cc, 0x7c71);
  if (!(F & FZ)) {
    CYCT(0x7c71, 0x7c72); ret_effect(gb); return;
  }
  CYC(0x7c71, 0x7c72);
  CYC(0x7c72, 0x7c73); alu_xor(gb, A);
  CYC(0x7c73, 0x7c75); L = INTERACTION_BASE + OBJ_Z;
  CYC(0x7c75, 0x7c76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7c76, 0x7c77); mem_wr(gb, HL, A);
  CYC(0x7c77, 0x7c79); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x7c79, 0x7c7b); mem_wr(gb, HL, 0x00);
  boy_updateAngleAndCounter(gb, sp0_);
  return;

substate6:
  CALL_C(0x7cce, interactionDecCounter1_hook, 0x23cc, 0x7cd1);
  if (!(F & FZ)) {
    CYCT(0x7cd1, 0x7cd2); ret_effect(gb); return;
  }
  CYC(0x7cd1, 0x7cd2);
  CYC(0x7cd2, 0x7cd4); E = INTERACTION_BASE + OBJ_ID;
  CYC(0x7cd4, 0x7cd5); A = mem_rd(gb, DE);
  CYC(0x7cd5, 0x7cd7); B = 0x34;
  CYC(0x7cd7, 0x7cd9); alu_cp(gb, 0x3f); // INTERAC_BOY_2
  if (F & FZ) {
    CYCT(0x7cd9, 0x7cdb);
  } else {
    CYC(0x7cd9, 0x7cdb);
    CYC(0x7cdb, 0x7cdd); B = 0x20;
  }
  CYC(0x7cdd, 0x7cde); mem_wr(gb, HL, B);
  CYC(0x7cde, 0x7ce0); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7ce0, 0x7ce2); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x7ce2, 0x7ce5); interactionIncSubstate_hook(gb);
  return;

substate7:
  CYC(0x7ce5, 0x7ce8); push_effect(gb, 0x7ce8); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(0x7ce8, interactionDecCounter1_hook, 0x23cc, 0x7ceb);
  if (!(F & FZ)) {
    CYCT(0x7ceb, 0x7cec); ret_effect(gb); return;
  }
  CYC(0x7ceb, 0x7cec);
  CALL_C(0x7cec, getRandomNumber_hook, 0x043e, 0x7cef);
  CYC(0x7cef, 0x7cf1); alu_and(gb, 0x07);
  CYC(0x7cf1, 0x7cf2); A = alu_inc8(gb, A);
  CYC(0x7cf2, 0x7cf3); mem_wr(gb, HL, A);
  CYC(0x7cf3, 0x7cf5); A = 0x01;
  CALL_C(0x7cf5, interactionSetAnimation_hook, 0x262e, 0x7cf8);
  CYC(0x7cf8, 0x7cfb); interactionIncSubstate_hook(gb);
  return;

substate8:
  // Waiting for signal to start hopping again
  CYC(0x7cfb, 0x7cfe); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7cfe, 0x7d00); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x7d00, 0x7d01); ret_effect(gb); return;
  }
  CYC(0x7d00, 0x7d01);
  CALL_C(0x7d01, interactionDecCounter1_hook, 0x23cc, 0x7d04);
  if (!(F & FZ)) {
    CYCT(0x7d04, 0x7d05); ret_effect(gb); return;
  }
  CYC(0x7d04, 0x7d05);
  CALL_C(0x7d05, interactionIncSubstate_hook, 0x23e5, 0x7d08);
  CYC(0x7d08, 0x7d0b); boyStartHop_hook(gb);
  return;

substate9:
  // Waiting for signal to move off the left side of the screen
  CALL_C(0x7d0b, boyUpdateGravityAndHopWhenLanded_hook, 0x7dea, 0x7d0e);
  CYC(0x7d0e, 0x7d11); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x7d11, 0x7d13); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x7d13, 0x7d14); ret_effect(gb); return;
  }
  CYC(0x7d13, 0x7d14);
  CYC(0x7d14, 0x7d16); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7d16, 0x7d17); A = mem_rd(gb, DE);
  CYC(0x7d17, 0x7d18); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7d18, 0x7d19); ret_effect(gb); return;
  }
  CYC(0x7d18, 0x7d19);
  CALL_C(0x7d19, interactionIncSubstate_hook, 0x23e5, 0x7d1c);
  CYC(0x7d1c, 0x7d1e); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7d1e, 0x7d20); mem_wr(gb, HL, 0x0c);
  CYC(0x7d20, 0x7d21); ret_effect(gb);
  return;

substateA:
  CALL_C(0x7d21, interactionDecCounter1_hook, 0x23cc, 0x7d24);
  if (!(F & FZ)) {
    CYCT(0x7d24, 0x7d25); ret_effect(gb); return;
  }
  CYC(0x7d24, 0x7d25);
  CALL_C(0x7d25, interactionIncSubstate_hook, 0x23e5, 0x7d28);
  CYC(0x7d28, 0x7d2a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7d2a, 0x7d2c); mem_wr(gb, HL, 0x50);
  CYC(0x7d2c, 0x7d2e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7d2e, 0x7d30); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x7d30, 0x7d32); A = 0x03;
  CYC(0x7d32, 0x7d35); interactionSetAnimation_hook(gb);
  return;

substateB:
  CYC(0x7d35, 0x7d38); push_effect(gb, 0x7d38); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(0x7d38, interactionDecCounter1_hook, 0x23cc, 0x7d3b);
  if (F & FZ) {
    CYCT(0x7d3b, 0x7d3e); interactionDelete_hook(gb); return;
  }
  CYC(0x7d3b, 0x7d3e);
  CYC(0x7d3e, 0x7d3f); ret_effect(gb);
}

// Cutscene?
void boyRunSubid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d3f, interactionAnimate_hook, 0x261b, 0x7d42);
  CYC(0x7d42, 0x7d45); childAnimateIfVar39IsZeroAndRunScript_hook(gb);
}

// NPC in eyeglasses library present
void boyRunSubid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7d45, interactionRunScript_hook, 0x2552, 0x7d48);
  CYC(0x7d48, 0x7d4b); interactionAnimateAsNpc_hook(gb);
}

// Cutscene where kid's dad gets restored from stone
void boyRunSubid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d4b, 0x7d4d); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7d4d, 0x7d4e); A = mem_rd(gb, DE);
  CYC(0x7d4e, 0x7d4f); push_effect(gb, 0x7d4f);
  switch (boy_jumpTable(gb)) {
    case 0x7d5b: goto substate0;
    case 0x7d78: goto substate1;
    case 0x7d85: goto substate2;
    case 0x7d98: goto substate3;
    case 0x7da1: goto substate4;
    case 0x7b7d: childAnimateIfVar39IsZeroAndRunScript_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x7d5b, interactionAnimate2Times_hook, 0x2752, 0x7d5e);
  CYC(0x7d5e, 0x7d61); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x7d61, 0x7d63); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x7d63, 0x7d64); ret_effect(gb); return;
  }
  CYC(0x7d63, 0x7d64);
  CALL_C(0x7d64, interactionIncSubstate_hook, 0x23e5, 0x7d67);
  CYC(0x7d67, 0x7d69); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7d69, 0x7d6b); mem_wr(gb, HL, 0x78);
  CYC(0x7d6b, 0x7d6d); A = 0x03;
  CALL_C(0x7d6d, interactionSetAnimation_hook, 0x262e, 0x7d70);
  CYC(0x7d70, 0x7d72); A = 0x3c;
  CYC(0x7d72, 0x7d75); SET_BC(0xf408);
  CYC(0x7d75, 0x7d78); objectCreateExclamationMark_hook(gb);
  return;

substate1:
  CALL_C(0x7d78, interactionDecCounter1_hook, 0x23cc, 0x7d7b);
  if (!(F & FZ)) {
    CYCT(0x7d7b, 0x7d7c); ret_effect(gb); return;
  }
  CYC(0x7d7b, 0x7d7c);
  CALL_C(0x7d7c, interactionIncSubstate_hook, 0x23e5, 0x7d7f);
  CYC(0x7d7f, 0x7d82); SET_BC(0xfe40); // -$1c0
  CYC(0x7d82, 0x7d85); objectSetSpeedZ_hook(gb);
  return;

substate2:
  CYC(0x7d85, 0x7d87); C = 0x20;
  CALL_C(0x7d87, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7d8a);
  if (!(F & FZ)) {
    CYCT(0x7d8a, 0x7d8b); ret_effect(gb); return;
  }
  CYC(0x7d8a, 0x7d8b);
  CYC(0x7d8b, 0x7d8d); A = 0x02;
  CYC(0x7d8d, 0x7d90); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CALL_C(0x7d90, interactionIncSubstate_hook, 0x23e5, 0x7d93);
  CYC(0x7d93, 0x7d95); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7d95, 0x7d97); mem_wr(gb, HL, 120);
  CYC(0x7d97, 0x7d98); ret_effect(gb);
  return;

substate3:
  CALL_C(0x7d98, interactionDecCounter1_hook, 0x23cc, 0x7d9b);
  if (!(F & FZ)) {
    CYCT(0x7d9b, 0x7d9c); ret_effect(gb); return;
  }
  CYC(0x7d9b, 0x7d9c);
  CYC(0x7d9c, 0x7d9e); mem_wr(gb, HL, 0x3c);
  CYC(0x7d9e, 0x7da1); interactionIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(0x7da1, interactionAnimate2Times_hook, 0x2752, 0x7da4);
  CALL_C(0x7da4, interactionDecCounter1_hook, 0x23cc, 0x7da7);
  if (!(F & FZ)) {
    CYCT(0x7da7, 0x7da8); ret_effect(gb); return;
  }
  CYC(0x7da7, 0x7da8);
  CYC(0x7da8, 0x7dab); interactionIncSubstate_hook(gb);
}

// Kid with grandma who's either stone or was restored from stone
void boyRunSubid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7dab, 0x7dad); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7dad, 0x7dae); A = mem_rd(gb, DE);
  CYC(0x7dae, 0x7daf); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7daf, 0x7db2); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return;
  }
  CYC(0x7daf, 0x7db2);
  CALL_C(0x7db2, interactionRunScript_hook, 0x2552, 0x7db5);
  CYC(0x7db5, 0x7db8); npcFaceLinkAndAnimate_hook(gb);
}

// NPC playing catch with dad, or standing next to his stone dad
void boyRunSubid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Check if his dad is stone
  CYC(0x7db8, 0x7dba); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7dba, 0x7dbb); A = mem_rd(gb, DE);
  CYC(0x7dbb, 0x7dbc); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7dbc, 0x7dbe); goto runScript;
  }
  CYC(0x7dbc, 0x7dbe);
  CALL_C(0x7dbe, interactionAnimate2Times_hook, 0x2752, 0x7dc1);
  CYC(0x7dc1, 0x7dc3); goto pushLinkAway;
runScript:
  CALL_C(0x7dc3, interactionRunScript_hook, 0x2552, 0x7dc6);
pushLinkAway:
  CALL_C(0x7dc6, interactionPushLinkAwayAndUpdateDrawPriority_hook, 0x26de, 0x7dc9);
  CYC(0x7dc9, 0x7dca); H = D;
  CYC(0x7dca, 0x7dcc); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x7dcc, 0x7dcd); A = mem_rd(gb, HL);
  CYC(0x7dcd, 0x7dce); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7dce, 0x7dcf); ret_effect(gb); return;
  }
  CYC(0x7dce, 0x7dcf);
  CYC(0x7dcf, 0x7dd1); mem_wr(gb, HL, 0x00);
  CYC(0x7dd1, 0x7dd3); B = 0x25; // >TX_2500
  CYC(0x7dd3, 0x7dd5); L = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x7dd5, 0x7dd6); C = mem_rd(gb, HL);
  CYC(0x7dd6, 0x7dd9); showText_hook(gb);
}

// Subid $0f: cutscene where kid runs away? Subid $10: kid listening to Nayru postgame.
void boyRunSubid0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7dd9, interactionRunScript_hook, 0x2552, 0x7ddc);
  if (F & FC) {
    CYCT(0x7ddc, 0x7ddf); interactionDelete_hook(gb); return;
  }
  CYC(0x7ddc, 0x7ddf);
  CALL_C(0x7ddf, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x7de2);
  CYC(0x7de2, 0x7de5); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

// Load palette used for turning npcs to stone?
void loadStoneNpcPalette_hook(GB *gb) {
  CYC(0x7de5, 0x7de7); A = 0xa2; // PALH_a2
  CYC(0x7de7, 0x7dea); loadPaletteHeader_hook(gb);
}

// Falls through into boyStartHop when landed.
void boyUpdateGravityAndHopWhenLanded_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7dea, 0x7dec); C = 0x20;
  CALL_C(0x7dec, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7def);
  if (!(F & FZ)) {
    CYCT(0x7def, 0x7df0); ret_effect(gb); return;
  }
  CYC(0x7def, 0x7df0);
  boyStartHop_hook(gb);
}

void boyStartHop_hook(GB *gb) {
  CYC(0x7df0, 0x7df3); SET_BC(0xff20); // -$e0
  CYC(0x7df3, 0x7df6); objectSetSpeedZ_hook(gb);
}

// Load a script for INTERAC_BOY.
void boyLoadScript_hook(GB *gb) {
  CYC(0x7df6, 0x7df8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7df8, 0x7df9); A = mem_rd(gb, DE);
  CYC(0x7df9, 0x7dfc); SET_HL(boyLoadScriptTable_bank08);
  CYC(0x7dfc, 0x7dfd); boy_addDoubleIndex(gb, 0x7dfd);
  CYC(0x7dfd, 0x7dfe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7dfe, 0x7dff); H = mem_rd(gb, HL);
  CYC(0x7dff, 0x7e00); L = A;
  CYC(0x7e00, 0x7e03); interactionSetScript_hook(gb);
}
