#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// boyRunSubid09@movementData: 3 runs (subids 8, 9, $0a) of {frames, angle} pairs, each
// terminated by a $00 byte (9 bytes per run).
#define boyMovementData_bank08 SYM(boyRunSubid09__movementData)
// boyLoadScript@scriptTable: 17 mainScripts.boySubidNNScript pointers (bank $0c), indexed by subid.
#define boyLoadScriptTable_bank08 SYM(boyLoadScript__scriptTable)
// boySubid02ScriptTable: 6 mainScripts.boySubid02Script_* pointers (bank $0c), indexed by
// game progress.
#define boySubid02ScriptTable_bank08 SYM(boySubid02ScriptTable)
// agesInteractionsBank09.getGameProgress_1, reached through interBankCall.
#define getGameProgress_1_bank09 SYM(getGameProgress_1)

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
  BASE(interactionCode3c);
  CYC(b_+117, b_+119); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+124); ret_effect(gb);
}

// interactionCode3c@initSubid: per-subid initialization. Reached only by a static `call`
// from interactionCode3c's state 0.
static void boy_initSubid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3c);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+229) { goto initSubid00; }
    else if (jt_ == b_+87) { goto initSubid01; }
    else if (jt_ == b_+236) { goto initSubid02; }
    else if (jt_ == b_+66) { goto initSubid03; }
    else if (jt_ == b_+100) { goto initSubid04; }
    else if (jt_ == b_+124) { goto initSubid05; }
    else if (jt_ == b_+142) { goto setStoneAnimationAndLoadScript; }
    else if (jt_ == b_+269) { goto initSubid07; }
    else if (jt_ == b_+278) { goto initSubid08; }
    else if (jt_ == b_+286) { goto initSubid0b; }
    else if (jt_ == b_+311) { goto initSubid0d; }
    else if (jt_ == b_+158) { goto initSubid0e; }
    else if (jt_ == b_+360) { goto initSubid0f; }
    else if (jt_ == b_+352) { goto initSubid10; }
    else { HANDOFF(HL); }
  } while (0);

initSubid03:
  CALL_C(b_+66, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+69);
  CYC(b_+69, b_+71); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+71, b_+74); TAIL(interactionDelete);
  }
  CYC(b_+71, b_+74);
  CYC(b_+74, b_+77); push_effect(gb, b_+77); boy_saveXToVar3d(gb);
  CYC(b_+77, b_+79); A = 0x01;
  CYC(b_+79, b_+82); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+82, b_+85); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+85, b_+87); goto setRedPaletteAndLoadScript;

initSubid01:
  CYC(b_+87, b_+89); A = 0x01;
  CALL_C(b_+89, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+92);

setRedPaletteAndLoadScript:
  CYC(b_+92, b_+94); A = 0x02;
  CYC(b_+94, b_+96); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  CYC(b_+97, b_+100); boyLoadScript_hook(gb);
  return;

initSubid04:
  CALL_C(b_+100, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+103);
  CYC(b_+103, b_+105); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+105, b_+108); TAIL(interactionDelete);
  }
  CYC(b_+105, b_+108);
  CYC(b_+108, b_+110); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+110, b_+112); A = 0x3c;
  CYC(b_+112, b_+113); mem_wr(gb, DE, A);
  CYC(b_+113, b_+114); alu_xor(gb, A);
  CALL_C(b_+114, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+117);
  boy_saveXToVar3d(gb);
  return;

initSubid05:
  CALL_C(b_+124, loadStoneNpcPalette_hook, SYM(loadStoneNpcPalette), b_+127);
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+129, b_+131); A = 0x06;
  CYC(b_+131, b_+132); mem_wr(gb, DE, A);
  CYC(b_+132, b_+134); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+134, b_+136); A = 0x3c;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  CYC(b_+137, b_+139); A = 0x03;
  CYC(b_+139, b_+142); interactionSetAnimation_hook(gb);
  return;

setStoneAnimationAndLoadScript:
  CYC(b_+142, b_+144); A = 0x03;
  CALL_C(b_+144, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+147);
  CYC(b_+147, b_+149); A = 0x02;
  CYC(b_+149, b_+151); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CALL_C(b_+152, loadStoneNpcPalette_hook, SYM(loadStoneNpcPalette), b_+155);
  CYC(b_+155, b_+158); boyLoadScript_hook(gb);
  return;

initSubid0e:
  // Was Veran defeated?
  CYC(b_+158, b_+161); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+161, b_+163); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+163, b_+165); A = 0x1e; // <TX_251e
  if (!(F & FZ)) {
    CYCT(b_+165, b_+167); goto initSubid0e_notStone;
  }
  CYC(b_+165, b_+167);
  CYC(b_+167, b_+170); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+170, b_+172); alu_bit(gb, 6, A);
  CYC(b_+172, b_+174); A = 0x1d; // <TX_251d
  if (F & FZ) {
    CYCT(b_+174, b_+176); goto initSubid0e_notStone;
  }
  CYC(b_+174, b_+176);
  // Veran not defeated and d7 beaten: stand in front of his stone dad
  CALL_C(b_+176, objectUnmarkSolidPosition_hook, SYM(objectUnmarkSolidPosition), b_+179);
  CYC(b_+179, b_+182); SET_BC((SYM(interactionCode1e__scriptSubidTable) + 16));
  CALL_C(b_+182, interactionSetPosition_hook, SYM(interactionSetPosition), b_+185);
  CALL_C(b_+185, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+188);
  CYC(b_+188, b_+189); H = D;
  CYC(b_+189, b_+191); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+191, b_+192); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+192, b_+194); A = 0x06;
  CALL_C(b_+194, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+197);
  CYC(b_+197, b_+199); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+199, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+202);
  CYC(b_+202, b_+204); A = 0x1b; // <TX_251b
  CYC(b_+204, b_+206); goto setTextIDAndLoadScript;

initSubid0e_notStone:
  CYC(b_+206, b_+207); push_effect(gb, AF);
  CYC(b_+207, b_+208); alu_xor(gb, A);
  CYC(b_+208, b_+211); mem_wr(gb, wTmpcfc0 + 0x13, A);
  CYC(b_+211, b_+214); SET_BC(0x9500); // INTERAC_BALL, $00
  CALL_C(b_+214, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+217);
  CYC(b_+217, b_+220); SET_BC((SYM(interactionCode1b) + 6));
  CALL_C(b_+220, interactionHSetPosition_hook, SYM(interactionHSetPosition), b_+223);
  CYC(b_+223, b_+224); SET_AF(pop_effect(gb));

setTextIDAndLoadScript:
  CYC(b_+224, b_+226); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+226, b_+227); mem_wr(gb, DE, A);
  CYC(b_+227, b_+229); goto setStoneAnimationAndLoadScript;

initSubid00:
  CYC(b_+229, b_+230); alu_xor(gb, A);
  CALL_C(b_+230, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+233);
  CYC(b_+233, b_+236); boyLoadScript_hook(gb);
  return;

initSubid02:
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(b_+236, b_+239); SET_HL(getGameProgress_1_bank09);
  CYC(b_+239, b_+241); E = 0x09;
  CALL_C(b_+241, interBankCall_hook, 0x008a, b_+244);
  CYC(b_+244, b_+245); A = B;
  CYC(b_+245, b_+246); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+246, b_+248); goto initSubid02_lookupScript;
  }
  CYC(b_+246, b_+248);
  // In the early game, the boy only exists once you've gotten the satchel
  CYC(b_+248, b_+250); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(b_+250, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+253);
  if (!(F & FC)) {
    CYCT(b_+253, b_+256); TAIL(interactionDelete);
  }
  CYC(b_+253, b_+256);
  CYC(b_+256, b_+257); alu_xor(gb, A);
initSubid02_lookupScript:
  CYC(b_+257, b_+260); SET_HL(boySubid02ScriptTable_bank08);
  CYC(b_+260, b_+261); boy_addDoubleIndex(gb, b_+261);
  CYC(b_+261, b_+262); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+262, b_+263); H = mem_rd(gb, HL);
  CYC(b_+263, b_+264); L = A;
  CALL_C(b_+264, interactionSetScript_hook, SYM(interactionSetScript), b_+267);
  CYC(b_+267, b_+269); boyState1_hook(gb);
  return;

initSubid07:
  CYC(b_+269, b_+270); H = D;
  CYC(b_+270, b_+272); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+272, b_+273); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+273, boyLoadScript_hook, SYM(boyLoadScript), b_+276);
  CYC(b_+276, b_+278); boyState1_hook(gb);
  return;

initSubid08:
  // Also @initSubid09
  CYC(b_+278, b_+279); H = D;
  CYC(b_+279, b_+281); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+281, b_+283); mem_wr(gb, HL, 0x78);
  CYC(b_+283, b_+286); objectSetVisiblec1_hook(gb);
  return;

initSubid0b:
  CYC(b_+286, b_+287); alu_xor(gb, A);
  CALL_C(b_+287, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+290);
  CYC(b_+290, b_+292); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(b_+292, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+295);
  CYC(b_+295, b_+297); A = 0x19; // <TX_2519
  if (F & FZ) {
    CYCT(b_+297, b_+299);
  } else {
    CYC(b_+297, b_+299);
    CYC(b_+299, b_+301); A = 0x1a; // <TX_251a
  }
  CYC(b_+301, b_+303); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+303, b_+304); mem_wr(gb, DE, A);
  CYC(b_+304, b_+305); E = alu_inc8(gb, E);
  CYC(b_+305, b_+307); A = 0x25; // >TX_2500
  CYC(b_+307, b_+308); mem_wr(gb, DE, A);
  CYC(b_+308, b_+311); boyLoadScript_hook(gb);
  return;

initSubid0d:
  CYC(b_+311, b_+313); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(b_+313, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+316);
  if (!(F & FZ)) {
    CYCT(b_+316, b_+318); goto initSubid0d_notStone;
  }
  CYC(b_+316, b_+318);
  CALL_C(b_+318, loadStoneNpcPalette_hook, SYM(loadStoneNpcPalette), b_+321);
  CYC(b_+321, b_+322); H = D;
  CYC(b_+322, b_+324); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+324, b_+326); mem_wr(gb, HL, 0x06);
  CYC(b_+326, b_+328); A = 0x06;
  CALL_C(b_+328, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+331);
  CYC(b_+331, b_+333); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+333, b_+334); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+334, b_+336); A = 0x0c;
  CYC(b_+336, b_+339); interactionSetAnimation_hook(gb);
  return;

initSubid0d_notStone:
  CYC(b_+339, b_+342); SET_BC(SYM(interactionCode15));
  CALL_C(b_+342, interactionSetPosition_hook, SYM(interactionSetPosition), b_+345);
  // Load red palette
  CYC(b_+345, b_+347); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+347, b_+349); mem_wr(gb, HL, 0x02);
  CYC(b_+349, b_+352); boyLoadScript_hook(gb);
  return;

initSubid10:
  CYC(b_+352, b_+354); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+354, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+357);
  if (F & FZ) {
    CYCT(b_+357, b_+360); TAIL(interactionDelete);
  }
  CYC(b_+357, b_+360);

initSubid0f:
  CYC(b_+360, b_+363); boyLoadScript_hook(gb);
}

// boyRunSubid09@setAnimationFromAngle: reached by fallthrough from @substate0, a static
// `call` from @substate1 and `jp`s from @substate5 and @updateAngleAndCounter.
static void boy_setAnimationFromAngle(GB *gb, uint16_t sp0_) {
  BASE(boyRunSubid08);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CALL_C(b_+48, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+51);
  CYC(b_+51, b_+54); interactionSetAnimation_hook(gb);
}

// boyRunSubid09@updateAnimationTwiceAndApplySpeed: reached only by static `call`s from
// substates 1, 5 (val0), 7 and $0b.
static void boy_updateAnimationTwiceAndApplySpeed(GB *gb, uint16_t sp0_) {
  BASE(boyRunSubid08);
  CALL_C(b_+79, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+82);
  CYC(b_+82, b_+85); objectApplySpeed_hook(gb);
}

// boyRunSubid09@updateAngleAndCounter: load the next {frames, angle} pair from
// @movementData for this subid. Reached by a static `call` from @substate4 and by
// fallthrough from @substate5@val2.
static void boy_updateAngleAndCounter(GB *gb, uint16_t sp0_) {
  BASE(boyRunSubid08);
  CYC(b_+219, b_+221); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+221, b_+222); A = mem_rd(gb, DE);
  CYC(b_+222, b_+224); alu_cp(gb, 0x3c); // INTERAC_BOY
  if (F & FZ) {
    CYCT(b_+224, b_+226); goto boy;
  }
  CYC(b_+224, b_+226);
  // Which interaction is this for?
  CYC(b_+226, b_+228); A = 0x02;
  CYC(b_+228, b_+230); goto times9;
boy:
  CYC(b_+230, b_+232); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+232, b_+233); A = mem_rd(gb, DE);
  CYC(b_+233, b_+235); alu_sub(gb, 0x08);
times9:
  CYC(b_+235, b_+236); B = A;
  CYC(b_+236, b_+238); A = alu_swap(gb, A);
  CYC(b_+238, b_+240); A = alu_sra(gb, A);
  CYC(b_+240, b_+241); alu_add(gb, B);
  CYC(b_+241, b_+244); SET_HL(boyMovementData_bank08);
  CYC(b_+244, b_+245); push_effect(gb, b_+245); boy_addAToHl(gb);
  CYC(b_+245, b_+247); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+247, b_+248); A = mem_rd(gb, DE);
  CYC(b_+248, b_+249); boy_addDoubleIndex(gb, b_+249);
  CYC(b_+249, b_+250); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+250, b_+251); B = mem_rd(gb, HL);
  CYC(b_+251, b_+252); L = alu_inc8(gb, L);
  CYC(b_+252, b_+254); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+254, b_+255); mem_wr(gb, DE, A);
  CYC(b_+255, b_+257); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+257, b_+258); A = B;
  CYC(b_+258, b_+259); mem_wr(gb, DE, A);
  CYC(b_+259, b_+261); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+261, b_+262); A = mem_rd(gb, DE);
  CYC(b_+262, b_+263); B = A;
  CYC(b_+263, b_+264); B = alu_inc8(gb, B);
  CYC(b_+264, b_+265); A = mem_rd(gb, HL);
  CYC(b_+265, b_+266); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+266, b_+268);
  } else {
    CYC(b_+266, b_+268);
    CYC(b_+268, b_+270); B = 0x00;
  }
  CYC(b_+270, b_+271); A = B;
  CYC(b_+271, b_+272); mem_wr(gb, DE, A);
  CYC(b_+272, b_+275); boy_setAnimationFromAngle(gb, sp0_);
}

// INTERAC_BOY
void interactionCode3c_hook(GB *gb) {
  BASE(interactionCode3c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == SYM(boyState1) && hook_enabled_at(gb, SYM(boyState1))) { boyState1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); boy_initSubid(gb, sp0_);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); TAIL(objectMarkSolidPosition);
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void boyState1_hook(GB *gb) {
  BASE(boyState1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == SYM(boyRunSubid00) && hook_enabled_at(gb, SYM(boyRunSubid00))) { boyRunSubid00_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid01) && hook_enabled_at(gb, SYM(boyRunSubid01))) { boyRunSubid01_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid02) && hook_enabled_at(gb, SYM(boyRunSubid02))) { boyRunSubid02_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid03) && hook_enabled_at(gb, SYM(boyRunSubid03))) { boyRunSubid03_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid04) && hook_enabled_at(gb, SYM(boyRunSubid04))) { boyRunSubid04_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid05) && hook_enabled_at(gb, SYM(boyRunSubid05))) { boyRunSubid05_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid06) && hook_enabled_at(gb, SYM(boyRunSubid06))) { boyRunSubid06_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid07) && hook_enabled_at(gb, SYM(boyRunSubid07))) { boyRunSubid07_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid08) && hook_enabled_at(gb, SYM(boyRunSubid08))) { boyRunSubid08_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid0a) && hook_enabled_at(gb, SYM(boyRunSubid0a))) { boyRunSubid0a_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid0b) && hook_enabled_at(gb, SYM(boyRunSubid0b))) { boyRunSubid0b_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid0c) && hook_enabled_at(gb, SYM(boyRunSubid0c))) { boyRunSubid0c_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid0d) && hook_enabled_at(gb, SYM(boyRunSubid0d))) { boyRunSubid0d_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid0e) && hook_enabled_at(gb, SYM(boyRunSubid0e))) { boyRunSubid0e_hook(gb); return; }
    else if (jt_ == SYM(boyRunSubid0f) && hook_enabled_at(gb, SYM(boyRunSubid0f))) { boyRunSubid0f_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Watching Nayru sing in intro
void boyRunSubid00_hook(GB *gb) {
  BASE(boyRunSubid00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CALL_C(b_+3, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+10, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+13);
  } else {
    CYC(b_+10, b_+13);
  }
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+25) { goto substate0; }
    else if (jt_ == b_+41) { goto substate1; }
    else if (jt_ == b_+64) { goto substate2; }
    else if (jt_ == b_+85) { goto substate3; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+25, b_+28); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+28, b_+30); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+33); TAIL(interactionRunScript);
  }
  CYC(b_+30, b_+33);
  CALL_C(b_+33, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+36);
  CYC(b_+36, b_+38); A = 0x02;
  CYC(b_+38, b_+41); interactionSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(b_+41, interactionAnimate_hook, SYM(interactionAnimate), b_+44);
  CYC(b_+44, b_+47); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+47, b_+49); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+49, b_+50); ret_effect(gb); return;
  }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+53);
  CYC(b_+53, b_+56); SET_BC(0xfe80); // -$180
  CALL_C(b_+56, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+59);
  CYC(b_+59, b_+61); A = 0x02;
  CYC(b_+61, b_+64); interactionSetAnimation_hook(gb);
  return;

substate2:
  CYC(b_+64, b_+66); C = 0x20;
  CALL_C(b_+66, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+69);
  if (!(F & FZ)) {
    CYCT(b_+69, b_+70); ret_effect(gb); return;
  }
  CYC(b_+69, b_+70);
  // Run away
  CALL_C(b_+70, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+73);
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x02);
  CYC(b_+77, b_+79); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+81, b_+82); alu_xor(gb, A);
  CYC(b_+82, b_+85); interactionSetAnimation_hook(gb);
  return;

substate3:
  CALL_C(b_+85, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+88);
  if (!(F & FC)) {
    CYCT(b_+88, b_+91); TAIL(interactionDelete);
  }
  CYC(b_+88, b_+91);
  CYC(b_+91, b_+94); objectApplySpeed_hook(gb);
}

// Kid turning to stone cutscene
void boyRunSubid01_hook(GB *gb) {
  BASE(boyRunSubid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+31) { goto substate1; }
    else if (jt_ == b_+62) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+10, interactionRunScript_hook, SYM(interactionRunScript), b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+16, b_+18); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20); goto substate0_animate;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); interactionIncSubstate_hook(gb);
  return;
substate0_animate:
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+27, b_+28); ret_effect(gb); return;
  }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+31); interactionAnimate2Times_hook(gb);
  return;

substate1:
  CALL_C(b_+31, interactionRunScript_hook, SYM(interactionRunScript), b_+34);
  CYC(b_+34, b_+37); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+37, b_+39); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+39, b_+41); goto substate1_flicker;
  }
  CYC(b_+39, b_+41);
  CALL_C(b_+41, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+44);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x06);
  CYC(b_+48, b_+49); ret_effect(gb);
  return;
substate1_flicker:
  // Flicker palette from red to stone every 8 frames
  CYC(b_+49, b_+52); A = mem_rd(gb, wFrameCounter);
  CYC(b_+52, b_+54); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+54, b_+55); ret_effect(gb); return;
  }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+60); alu_xor(gb, 0x04);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+62); ret_effect(gb);
  return;

substate2:
  CALL_C(b_+62, interactionRunScript_hook, SYM(interactionRunScript), b_+65);
  if (!(F & FC)) {
    CYCT(b_+65, b_+68); TAIL(interactionAnimate);
  }
  CYC(b_+65, b_+68);
  CYC(b_+68, b_+69); ret_effect(gb);
}

// Kid outside shop
void boyRunSubid02_hook(GB *gb) {
  BASE(boyRunSubid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6); npcFaceLinkAndAnimate_hook(gb);
}

// Cutscene where kids talk about how they're scared of a ghost (red kid). Also a substate 2
// jump-table target of boyRunSubid04.
void boyRunSubid03_hook(GB *gb) {
  BASE(boyRunSubid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+7, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  CALL_C(b_+10, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+13);
  if (F & FC) {
    CYCT(b_+13, b_+14); ret_effect(gb); return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+21); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+21, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+24);
  CYC(b_+24, b_+26); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(b_+26, b_+29); interactionDelete_hook(gb);
}

// Cutscene where kids talk about how they're scared of a ghost (green kid)
void boyRunSubid04_hook(GB *gb) {
  BASE(boyRunSubid04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+23) { goto substate1; }
    else if (jt_ == SYM(boyRunSubid03) && hook_enabled_at(gb, SYM(boyRunSubid03))) { boyRunSubid03_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+10, interactionAnimate_hook, SYM(interactionAnimate), b_+13);
  CALL_C(b_+13, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+16);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  CALL_C(b_+17, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+20);
  CYC(b_+20, b_+23); startJump_hook(gb);
  return;

substate1:
  CYC(b_+23, b_+25); C = 0x20;
  CALL_C(b_+25, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+28);
  if (!(F & FZ)) {
    CYCT(b_+28, b_+29); ret_effect(gb); return;
  }
  CYC(b_+28, b_+29);
  CALL_C(b_+29, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+32);
  CYC(b_+32, b_+35); boyLoadScript_hook(gb);
}

// Cutscene where kid is restored from stone. Substate 0 falls through into
// setCounter1To120AndPlaySoundEffectAndIncSubstate.
void boyRunSubid05_hook(GB *gb) {
  BASE(boyRunSubid05);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == SYM(childSubid05Substate1) && hook_enabled_at(gb, SYM(childSubid05Substate1))) { childSubid05Substate1_hook(gb); return; }
    else if (jt_ == SYM(childAnimateIfVar39IsZeroAndRunScript) && hook_enabled_at(gb, SYM(childAnimateIfVar39IsZeroAndRunScript))) { childAnimateIfVar39IsZeroAndRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+10, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+13);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+14); ret_effect(gb); return;
  }
  CYC(b_+13, b_+14);
  setCounter1To120AndPlaySoundEffectAndIncSubstate_hook(gb);
}

// Used in cutscenes where people get restored from stone?
void setCounter1To120AndPlaySoundEffectAndIncSubstate_hook(GB *gb) {
  BASE(setCounter1To120AndPlaySoundEffectAndIncSubstate);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 120;
  CYC(b_+2, b_+4); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x5c; // SND_ENERGYTHING
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  CYC(b_+10, b_+13); interactionIncSubstate_hook(gb);
}

void childSubid05Substate1_hook(GB *gb) {
  BASE(childSubid05Substate1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); TAIL(childFlickerBetweenStone);
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x02);
  CYC(b_+12, b_+15); boyLoadScript_hook(gb);
}

// Called from other interactions as well?
void childFlickerBetweenStone_hook(GB *gb) {
  BASE(childFlickerBetweenStone);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_xor(gb, 0x04);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void childAnimateIfVar39IsZeroAndRunScript_hook(GB *gb) {
  BASE(childAnimateIfVar39IsZeroAndRunScript);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+4, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+7);
  } else {
    CYC(b_+4, b_+7);
  }
  CYC(b_+7, b_+10); interactionRunScript_hook(gb);
}

// Cutscene where kid sees his dad turn to stone
void boyRunSubid06_hook(GB *gb) {
  BASE(boyRunSubid06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+3);
  if (!(F & FZ)) {
    CALL_C_CC(b_+3, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+6);
  } else {
    CYC(b_+3, b_+6);
  }
  CYC(b_+6, b_+9); interactionRunScript_hook(gb);
}

// Depressed kid in trade sequence
void boyRunSubid07_hook(GB *gb) {
  BASE(boyRunSubid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+7, b_+10); TAIL(npcFaceLinkAndAnimate);
  }
  CYC(b_+7, b_+10);
  CALL_C(b_+10, interactionAnimate_hook, SYM(interactionAnimate), b_+13);
  CYC(b_+13, b_+16); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

// Subids $08/$09 (and $0a's movement data): kid who runs around in a pattern.
void boyRunSubid08_hook(GB *gb) {
  BASE(boyRunSubid08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+28) { goto substate0; }
    else if (jt_ == b_+54) { goto substate1; }
    else if (jt_ == b_+85) { goto substate2; }
    else if (jt_ == b_+95) { goto substate3; }
    else if (jt_ == b_+117) { goto substate4; }
    else if (jt_ == b_+131) { goto substate5; }
    else if (jt_ == b_+302) { goto substate6; }
    else if (jt_ == b_+325) { goto substate7; }
    else if (jt_ == b_+347) { goto substate8; }
    else if (jt_ == b_+363) { goto substate9; }
    else if (jt_ == b_+385) { goto substateA; }
    else if (jt_ == b_+405) { goto substateB; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+28, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+31);
  if (!(F & FZ)) {
    CYCT(b_+31, b_+32); ret_effect(gb); return;
  }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x66);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x18);
  CALL_C(b_+42, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+45);
  boy_setAnimationFromAngle(gb, sp0_);
  return;

substate1:
  CYC(b_+54, b_+57); push_effect(gb, b_+57); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(b_+57, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+60);
  if (!(F & FZ)) {
    CYCT(b_+60, b_+61); ret_effect(gb); return;
  }
  CYC(b_+60, b_+61);
  CALL_C(b_+61, getRandomNumber_hook, SYM(getRandomNumber), b_+64);
  CYC(b_+64, b_+66); alu_and(gb, 0x0f);
  CYC(b_+66, b_+68); alu_add(gb, 0x1e);
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);
  CYC(b_+69, b_+71); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x08);
  CYC(b_+73, b_+76); push_effect(gb, b_+76); boy_setAnimationFromAngle(gb, sp0_);
  CYC(b_+76, b_+79); interactionIncSubstate_hook(gb);
  return;

substate2:
  CALL_C(b_+85, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+88);
  if (!(F & FZ)) {
    CYCT(b_+88, b_+89); ret_effect(gb); return;
  }
  CYC(b_+88, b_+89);
  CALL_C(b_+89, boyStartHop_hook, SYM(boyStartHop), b_+92);
  CYC(b_+92, b_+95); interactionIncSubstate_hook(gb);
  return;

substate3:
  CALL_C(b_+95, boyUpdateGravityAndHopWhenLanded_hook, SYM(boyUpdateGravityAndHopWhenLanded), b_+98);
  CYC(b_+98, b_+101); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+101, b_+103); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+103, b_+104); ret_effect(gb); return;
  }
  CYC(b_+103, b_+104);
  CYC(b_+104, b_+106); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+108, b_+109); ret_effect(gb); return;
  }
  CYC(b_+108, b_+109);
  CALL_C(b_+109, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+112);
  CYC(b_+112, b_+114); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x1e);
  CYC(b_+116, b_+117); ret_effect(gb);
  return;

substate4:
  CALL_C(b_+117, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+120);
  if (!(F & FZ)) {
    CYCT(b_+120, b_+121); ret_effect(gb); return;
  }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+125, b_+128); push_effect(gb, b_+128); boy_updateAngleAndCounter(gb, sp0_);
  CYC(b_+128, b_+131); interactionIncSubstate_hook(gb);
  return;

substate5:
  CYC(b_+131, b_+134); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+134, b_+136); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+136, b_+138); goto substate5_dispatchVar37;
  }
  CYC(b_+136, b_+138);
  CYC(b_+138, b_+140); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+142); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+142, b_+144); goto substate5_dispatchVar37;
  }
  CYC(b_+142, b_+144);
  CALL_C(b_+144, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+147);
  CYC(b_+147, b_+149); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+149, b_+151); mem_wr(gb, HL, 0x0a);
  CYC(b_+151, b_+153); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+153, b_+155); mem_wr(gb, HL, 0x18);
  CYC(b_+155, b_+158); boy_setAnimationFromAngle(gb, sp0_);
  return;
substate5_dispatchVar37:
  CYC(b_+158, b_+160); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+160, b_+161); A = mem_rd(gb, DE);
  CYC(b_+161, b_+162); push_effect(gb, b_+162);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+168) { goto substate5_val0; }
    else if (jt_ == b_+191) { goto substate5_val1; }
    else if (jt_ == b_+203) { goto substate5_val2; }
    else { HANDOFF(HL); }
  } while (0);

substate5_val0:
  CYC(b_+168, b_+171); push_effect(gb, b_+171); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(b_+171, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+174);
  if (!(F & FZ)) {
    CYCT(b_+174, b_+175); ret_effect(gb); return;
  }
  CYC(b_+174, b_+175);
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0x0a);
  CYC(b_+177, b_+179); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+179, b_+180); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+180, b_+182); alu_cp(gb, 0x68);
  CYC(b_+182, b_+184); A = 0x01;
  if (F & FC) {
    CYCT(b_+184, b_+186);
  } else {
    CYC(b_+184, b_+186);
    CYC(b_+186, b_+188); A = 0x03;
  }
  CYC(b_+188, b_+191); interactionSetAnimation_hook(gb);
  return;

substate5_val1:
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if (!(F & FZ)) {
    CYCT(b_+194, b_+195); ret_effect(gb); return;
  }
  CYC(b_+194, b_+195);
  CYC(b_+195, b_+197); mem_wr(gb, HL, 0x1e);
  CYC(b_+197, b_+199); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+199, b_+200); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+200, b_+203); boyStartHop_hook(gb);
  return;

substate5_val2:
  CALL_C(b_+203, boyUpdateGravityAndHopWhenLanded_hook, SYM(boyUpdateGravityAndHopWhenLanded), b_+206);
  CALL_C(b_+206, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+209);
  if (!(F & FZ)) {
    CYCT(b_+209, b_+210); ret_effect(gb); return;
  }
  CYC(b_+209, b_+210);
  CYC(b_+210, b_+211); alu_xor(gb, A);
  CYC(b_+211, b_+213); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+213, b_+214); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+214, b_+215); mem_wr(gb, HL, A);
  CYC(b_+215, b_+217); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+217, b_+219); mem_wr(gb, HL, 0x00);
  boy_updateAngleAndCounter(gb, sp0_);
  return;

substate6:
  CALL_C(b_+302, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+305);
  if (!(F & FZ)) {
    CYCT(b_+305, b_+306); ret_effect(gb); return;
  }
  CYC(b_+305, b_+306);
  CYC(b_+306, b_+308); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+308, b_+309); A = mem_rd(gb, DE);
  CYC(b_+309, b_+311); B = 0x34;
  CYC(b_+311, b_+313); alu_cp(gb, 0x3f); // INTERAC_BOY_2
  if (F & FZ) {
    CYCT(b_+313, b_+315);
  } else {
    CYC(b_+313, b_+315);
    CYC(b_+315, b_+317); B = 0x20;
  }
  CYC(b_+317, b_+318); mem_wr(gb, HL, B);
  CYC(b_+318, b_+320); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+320, b_+322); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+322, b_+325); interactionIncSubstate_hook(gb);
  return;

substate7:
  CYC(b_+325, b_+328); push_effect(gb, b_+328); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(b_+328, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+331);
  if (!(F & FZ)) {
    CYCT(b_+331, b_+332); ret_effect(gb); return;
  }
  CYC(b_+331, b_+332);
  CALL_C(b_+332, getRandomNumber_hook, SYM(getRandomNumber), b_+335);
  CYC(b_+335, b_+337); alu_and(gb, 0x07);
  CYC(b_+337, b_+338); A = alu_inc8(gb, A);
  CYC(b_+338, b_+339); mem_wr(gb, HL, A);
  CYC(b_+339, b_+341); A = 0x01;
  CALL_C(b_+341, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+344);
  CYC(b_+344, b_+347); interactionIncSubstate_hook(gb);
  return;

substate8:
  // Waiting for signal to start hopping again
  CYC(b_+347, b_+350); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+350, b_+352); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+352, b_+353); ret_effect(gb); return;
  }
  CYC(b_+352, b_+353);
  CALL_C(b_+353, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+356);
  if (!(F & FZ)) {
    CYCT(b_+356, b_+357); ret_effect(gb); return;
  }
  CYC(b_+356, b_+357);
  CALL_C(b_+357, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+360);
  CYC(b_+360, b_+363); boyStartHop_hook(gb);
  return;

substate9:
  // Waiting for signal to move off the left side of the screen
  CALL_C(b_+363, boyUpdateGravityAndHopWhenLanded_hook, SYM(boyUpdateGravityAndHopWhenLanded), b_+366);
  CYC(b_+366, b_+369); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+369, b_+371); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+371, b_+372); ret_effect(gb); return;
  }
  CYC(b_+371, b_+372);
  CYC(b_+372, b_+374); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+374, b_+375); A = mem_rd(gb, DE);
  CYC(b_+375, b_+376); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+376, b_+377); ret_effect(gb); return;
  }
  CYC(b_+376, b_+377);
  CALL_C(b_+377, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+380);
  CYC(b_+380, b_+382); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+382, b_+384); mem_wr(gb, HL, 0x0c);
  CYC(b_+384, b_+385); ret_effect(gb);
  return;

substateA:
  CALL_C(b_+385, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+388);
  if (!(F & FZ)) {
    CYCT(b_+388, b_+389); ret_effect(gb); return;
  }
  CYC(b_+388, b_+389);
  CALL_C(b_+389, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+392);
  CYC(b_+392, b_+394); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+394, b_+396); mem_wr(gb, HL, 0x50);
  CYC(b_+396, b_+398); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+398, b_+400); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+400, b_+402); A = 0x03;
  CYC(b_+402, b_+405); interactionSetAnimation_hook(gb);
  return;

substateB:
  CYC(b_+405, b_+408); push_effect(gb, b_+408); boy_updateAnimationTwiceAndApplySpeed(gb, sp0_);
  CALL_C(b_+408, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+411);
  if (F & FZ) {
    CYCT(b_+411, b_+414); TAIL(interactionDelete);
  }
  CYC(b_+411, b_+414);
  CYC(b_+414, b_+415); ret_effect(gb);
}

// Cutscene?
void boyRunSubid0a_hook(GB *gb) {
  BASE(boyRunSubid0a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+6); childAnimateIfVar39IsZeroAndRunScript_hook(gb);
}

// NPC in eyeglasses library present
void boyRunSubid0b_hook(GB *gb) {
  BASE(boyRunSubid0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6); interactionAnimateAsNpc_hook(gb);
}

// Cutscene where kid's dad gets restored from stone
void boyRunSubid0c_hook(GB *gb) {
  BASE(boyRunSubid0c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (boy_jumpTable(gb));
    if (jt_ == b_+16) { goto substate0; }
    else if (jt_ == b_+45) { goto substate1; }
    else if (jt_ == b_+58) { goto substate2; }
    else if (jt_ == b_+77) { goto substate3; }
    else if (jt_ == b_+86) { goto substate4; }
    else if (jt_ == SYM(childAnimateIfVar39IsZeroAndRunScript) && hook_enabled_at(gb, SYM(childAnimateIfVar39IsZeroAndRunScript))) { childAnimateIfVar39IsZeroAndRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+16, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+22, b_+24); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+25); ret_effect(gb); return;
  }
  CYC(b_+24, b_+25);
  CALL_C(b_+25, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+28);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x78);
  CYC(b_+32, b_+34); A = 0x03;
  CALL_C(b_+34, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+37);
  CYC(b_+37, b_+39); A = 0x3c;
  CYC(b_+39, b_+42); SET_BC(0xf408);
  CYC(b_+42, b_+45); objectCreateExclamationMark_hook(gb);
  return;

substate1:
  CALL_C(b_+45, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+48);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+49); ret_effect(gb); return;
  }
  CYC(b_+48, b_+49);
  CALL_C(b_+49, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+52);
  CYC(b_+52, b_+55); SET_BC(0xfe40); // -$1c0
  CYC(b_+55, b_+58); objectSetSpeedZ_hook(gb);
  return;

substate2:
  CYC(b_+58, b_+60); C = 0x20;
  CALL_C(b_+60, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+63);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+64); ret_effect(gb); return;
  }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+66); A = 0x02;
  CYC(b_+66, b_+69); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CALL_C(b_+69, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+72);
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 120);
  CYC(b_+76, b_+77); ret_effect(gb);
  return;

substate3:
  CALL_C(b_+77, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) {
    CYCT(b_+80, b_+81); ret_effect(gb); return;
  }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x3c);
  CYC(b_+83, b_+86); interactionIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(b_+86, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+89);
  CALL_C(b_+89, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+92);
  if (!(F & FZ)) {
    CYCT(b_+92, b_+93); ret_effect(gb); return;
  }
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+96); interactionIncSubstate_hook(gb);
}

// Kid with grandma who's either stone or was restored from stone
void boyRunSubid0d_hook(GB *gb) {
  BASE(boyRunSubid0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+7); TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
  }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, interactionRunScript_hook, SYM(interactionRunScript), b_+10);
  CYC(b_+10, b_+13); npcFaceLinkAndAnimate_hook(gb);
}

// NPC playing catch with dad, or standing next to his stone dad
void boyRunSubid0e_hook(GB *gb) {
  BASE(boyRunSubid0e);
  uint16_t sp0_ = gb->sp;
  // Check if his dad is stone
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+6); goto runScript;
  }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+9);
  CYC(b_+9, b_+11); goto pushLinkAway;
runScript:
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
pushLinkAway:
  CALL_C(b_+14, interactionPushLinkAwayAndUpdateDrawPriority_hook, SYM(interactionPushLinkAwayAndUpdateDrawPriority), b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+22, b_+23); ret_effect(gb); return;
  }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x00);
  CYC(b_+25, b_+27); B = 0x25; // >TX_2500
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+29, b_+30); C = mem_rd(gb, HL);
  CYC(b_+30, b_+33); showText_hook(gb);
}

// Subid $0f: cutscene where kid runs away? Subid $10: kid listening to Nayru postgame.
void boyRunSubid0f_hook(GB *gb) {
  BASE(boyRunSubid0f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) {
    CYCT(b_+3, b_+6); TAIL(interactionDelete);
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+9);
  CYC(b_+9, b_+12); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
}

// Load palette used for turning npcs to stone?
void loadStoneNpcPalette_hook(GB *gb) {
  BASE(loadStoneNpcPalette);
  CYC(b_+0, b_+2); A = 0xa2; // PALH_a2
  CYC(b_+2, b_+5); loadPaletteHeader_hook(gb);
}

// Falls through into boyStartHop when landed.
void boyUpdateGravityAndHopWhenLanded_hook(GB *gb) {
  BASE(boyUpdateGravityAndHopWhenLanded);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  boyStartHop_hook(gb);
}

void boyStartHop_hook(GB *gb) {
  BASE(boyStartHop);
  CYC(b_+0, b_+3); SET_BC(0xff20); // -$e0
  CYC(b_+3, b_+6); objectSetSpeedZ_hook(gb);
}

// Load a script for INTERAC_BOY.
void boyLoadScript_hook(GB *gb) {
  BASE(boyLoadScript);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(boyLoadScriptTable_bank08);
  CYC(b_+6, b_+7); boy_addDoubleIndex(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); interactionSetScript_hook(gb);
}
