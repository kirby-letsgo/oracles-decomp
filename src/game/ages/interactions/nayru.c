#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// nayruSubid00@swayHorizontally@xOffsets: $ff $ff $ff $00 $01 $01 $01 $00
#define nayruSwayXOffsets_bank08 SYM(nayruSubid00__swayHorizontally__xOffsets)

// mainScripts.* (bank $0c), objectData.* (bank $12) and scriptHelp.* (bank $15), referenced
// by address only.
#define genericNpcScript_bank0c SYM(genericNpcScript)
#define nayruScript00_part1_bank0c SYM(nayruScript00_part1)
#define nayruScript00_part2_bank0c SYM(nayruScript00_part2)
#define nayruScript01_bank0c SYM(nayruScript01_b0c)
#define nayruScript02_part1_bank0c SYM(nayruScript02_part1)
#define nayruScript02_part2_bank0c SYM(nayruScript02_part2_b0c)
#define nayruScript02_part3_bank0c SYM(nayruScript02_part3)
#define nayruScript03_bank0c SYM(nayruScript03_b0c)
#define nayruScript04_part1_bank0c SYM(nayruScript04_part1)
#define nayruScript04_part2_bank0c SYM(nayruScript04_part2)
#define nayruScript05_bank0c SYM(nayruScript05)
#define nayruScript07_bank0c SYM(nayruScript07_b0c)
#define nayruScript08_bank0c SYM(nayruScript08)
#define nayruScript09_bank0c SYM(nayruScript09)
#define nayruScript0a_bank0c SYM(nayruScript0a)
#define nayruScript10_bank0c SYM(nayruScript10_b0c)
#define nayruScript11_bank0c SYM(nayruScript11_b0c)
#define nayruScript13_bank0c SYM(nayruScript13_b0c)
#define ambisPalaceEntranceGuards_bank12 SYM(ambisPalaceEntranceGuards)
#define turnToFaceSomething_bank15 SYM(turnToFaceSomething)
#define objectWritePositionTocfd5_bank15 SYM(objectWritePositionTocfd5)

void nayruState0_hook(GB *gb);
void nayruState0__init0e_hook(GB *gb);
void nayruState1_hook(GB *gb);
void nayruSubid00_hook(GB *gb);
void nayruSubid01_hook(GB *gb);
void nayruSubid02_hook(GB *gb);
void nayruSubid02Substate0_hook(GB *gb);
void nayruAnimateAndRunScript_hook(GB *gb);
void nayruSubid02Substate1_hook(GB *gb);
void nayruFlipDirectionAtRandomIntervals_hook(GB *gb);
void nayruSetCounter1Randomly_hook(GB *gb);
void nayruSubid02Substate2_hook(GB *gb);
void nayruSubid03_hook(GB *gb);
void nayruSubid04_hook(GB *gb);
void nayruSubid05_hook(GB *gb);
void nayruUpdatePossessionPaletteDurations_hook(GB *gb);
void nayruSubid07_hook(GB *gb);
void nayruAsNpc_hook(GB *gb);
void nayruSubid09_hook(GB *gb);
void nayruSubid10_hook(GB *gb);
void nayruSubid0a_hook(GB *gb);
void nayruSubid13_hook(GB *gb);
void nayruRunScriptWithConditionalAnimation_hook(GB *gb);

static uint16_t nayru_jumpTable(GB *gb) {
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

static void nayru_addAToHl(GB *gb) {
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

// nayruState0@loadEvilPalette: load the possessed version of her palette into palette 6.
// Reached by a static `call` from @init00 and by fallthrough from @init0e.
static void nayru_loadEvilPalette(GB *gb) {
  BASE(nayruState0);
  CYC(b_+102, b_+104); A = 0x97; // PALH_97
  CYC(b_+104, b_+107); loadPaletteHeader_hook(gb);
}

// nayruState0@setSingingAnimation: tail shared by @init00 (fallthrough) and @init13 (`jp`).
static void nayru_setSingingAnimation(GB *gb, uint16_t sp0_) {
  BASE(nayruState0);
  CYC(b_+72, b_+74); A = 0x04;
  CALL_C(b_+74, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+77);
  CYC(b_+77, b_+80); interactionLoadExtraGraphics_hook(gb);
}

// nayruState0@init0e: also called from ambi subids 4 and 5 through interBankCall (to
// initialize possessed palettes), so it must stay hookable.
void nayruState0__init0e_hook(GB *gb) {
  BASE(nayruState0);
  CYC(b_+97, b_+99); A = 0x06;
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  nayru_loadEvilPalette(gb);
}

// nayruState0@init03: reached from the init table, from @init04's `jr nz` and by fallthrough.
static void nayru_init03(GB *gb) {
  BASE(nayruState0);
  CYC(b_+166, b_+167); alu_xor(gb, A);
  CYC(b_+167, b_+170); interactionSetAnimation_hook(gb);
}

// nayruState0@runGenericNpc: A = low byte of the text ID (high byte TX_1d00).
static void nayru_runGenericNpc(GB *gb) {
  BASE(nayruState0);
  CYC(b_+279, b_+281); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+281, b_+282); mem_wr(gb, DE, A);
  CYC(b_+282, b_+283); E = alu_inc8(gb, E);
  CYC(b_+283, b_+285); A = 0x1d;
  CYC(b_+285, b_+286); mem_wr(gb, DE, A);
  CYC(b_+286, b_+289); SET_HL(genericNpcScript_bank0c);
  CYC(b_+289, b_+292); interactionSetScript_hook(gb);
}

// nayruState0@initSubid: per-subid initialization. Reached only by a static `call` from
// nayruState0.
static void nayru_initSubid(GB *gb, uint16_t sp0_) {
  BASE(nayruState0);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == b_+64) { goto init00; }
    else if (jt_ == b_+80) { goto init01; }
    else if (jt_ == b_+107) { goto init02; }
    else if (jt_ == b_+166) { nayru_init03(gb); return; }
    else if (jt_ == b_+139) { goto init04; }
    else if (jt_ == b_+170) { goto init05; }
    else if (jt_ == b_+184) { goto init06; }
    else if (jt_ == b_+189) { goto init07; }
    else if (jt_ == b_+200) { goto init08; }
    else if (jt_ == b_+214) { goto init09; }
    else if (jt_ == b_+220) { goto init0a; }
    else if (jt_ == b_+253) { goto init0b; }
    else if (jt_ == b_+292) { goto init0c; }
    else if (jt_ == b_+320) { goto init0d; }
    else if (jt_ == b_+97) { nayruState0__init0e_hook(gb); return; }
    else if (jt_ == b_+340) { goto init0f; }
    else if (jt_ == b_+368) { goto init10; }
    else if (jt_ == b_+384) { goto init11; }
    else if (jt_ == b_+407) { goto init12; }
    else if (jt_ == b_+416) { goto init13; }
    else { HANDOFF(HL); }
  } while (0);

init00:
  CYC(b_+64, b_+66); A = 0x03;
  CALL_C(b_+66, setMusicVolume_hook, SYM(setMusicVolume), b_+69);
  CYC(b_+69, b_+72); push_effect(gb, b_+72); nayru_loadEvilPalette(gb);
  nayru_setSingingAnimation(gb, sp0_);
  return;

init01:
  CYC(b_+80, b_+82); A = 0x0b; // GLOBALFLAG_0b
  CALL_C(b_+82, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+85);
  if (!(F & FZ)) {
    CYCT(b_+85, b_+88); TAIL(interactionDelete);
  }
  CYC(b_+85, b_+88);
  CALL_C(b_+88, objectSetInvisible_hook, SYM(objectSetInvisible), b_+91);
  CYC(b_+91, b_+94); SET_HL(nayruScript01_bank0c);
  CALL_C(b_+94, interactionSetScript_hook, SYM(interactionSetScript), b_+97);
  TAIL(nayruState0__init0e);

init02:
  CYC(b_+107, b_+110); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+110, b_+112); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+112, b_+114); goto init02_part2;
  }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); A = 0x05;
  CALL_C(b_+116, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+119);
  CYC(b_+119, b_+122); SET_HL(nayruScript02_part1_bank0c);
  CALL_C(b_+122, interactionSetScript_hook, SYM(interactionSetScript), b_+125);
  CYC(b_+125, b_+128); objectSetInvisible_hook(gb);
  return;
init02_part2:
  CYC(b_+128, b_+130); A = 0x02;
  CALL_C(b_+130, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+133);
  CYC(b_+133, b_+136); SET_HL(nayruScript02_part2_bank0c);
  CYC(b_+136, b_+139); interactionSetScript_hook(gb);
  return;

init04:
  CYC(b_+139, b_+142); SET_HL(nayruScript04_part1_bank0c);
  CYC(b_+142, b_+145); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+145, b_+147); alu_cp(gb, 0x0b);
  if (!(F & FZ)) {
    CYCT(b_+147, b_+149); goto init04_setScript;
  }
  CYC(b_+147, b_+149);
  CYC(b_+149, b_+152); SET_BC((SYM(interactionCode1e__scriptSubidTable) + 8));
  CALL_C(b_+152, interactionSetPosition_hook, SYM(interactionSetPosition), b_+155);
  CALL_C(b_+155, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+158);
  if (!(F & FZ)) {
    CYCT(b_+158, b_+160); nayru_init03(gb); return;
  }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+163); SET_HL(nayruScript04_part2_bank0c);
init04_setScript:
  CALL_C(b_+163, interactionSetScript_hook, SYM(interactionSetScript), b_+166);
  nayru_init03(gb);
  return;

init05:
  CYC(b_+170, b_+172); A = 0x05;
  CALL_C(b_+172, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+175);
  CYC(b_+175, b_+178); SET_HL(nayruScript05_bank0c);
  CALL_C(b_+178, interactionSetScript_hook, SYM(interactionSetScript), b_+181);
  CYC(b_+181, b_+184); objectSetInvisible_hook(gb);
  return;

init06:
  CYC(b_+184, b_+186); A = 0x07;
  CYC(b_+186, b_+189); interactionSetAnimation_hook(gb);
  return;

init07:
  CYC(b_+189, b_+191); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+191, b_+193); A = 0x1e;
  CYC(b_+193, b_+194); mem_wr(gb, DE, A);
  CALL_C(b_+194, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+197);
  CYC(b_+197, b_+200); interactionSetAlwaysUpdateBit_hook(gb);
  return;

init08:
  CYC(b_+200, b_+203); SET_HL(nayruScript08_bank0c);
  CALL_C(b_+203, interactionSetScript_hook, SYM(interactionSetScript), b_+206);
  CALL_C(b_+206, objectSetVisible82_hook, SYM(objectSetVisible82), b_+209);
  CYC(b_+209, b_+211); A = 0x03;
  CYC(b_+211, b_+214); interactionSetAnimation_hook(gb);
  return;

init09:
  CYC(b_+214, b_+217); SET_HL(nayruScript09_bank0c);
  CYC(b_+217, b_+220); interactionSetScript_hook(gb);
  return;

init0a:
  CALL_C(b_+220, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+223);
  if (F & FZ) {
    CYCT(b_+223, b_+226); TAIL(interactionDelete);
  }
  CYC(b_+223, b_+226);
  CYC(b_+226, b_+228); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+228, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+231);
  if (!(F & FC)) {
    CYCT(b_+231, b_+234); TAIL(interactionDelete);
  }
  CYC(b_+231, b_+234);
  CYC(b_+234, b_+236); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+236, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+239);
  if (!(F & FZ)) {
    CYCT(b_+239, b_+242); TAIL(interactionDelete);
  }
  CYC(b_+239, b_+242);
  CYC(b_+242, b_+244); A = 0x01;
  CALL_C(b_+244, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+247);
  CYC(b_+247, b_+250); SET_HL(nayruScript0a_bank0c);
  CYC(b_+250, b_+253); interactionSetScript_hook(gb);
  return;

init0b:
  CYC(b_+253, b_+255); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+255, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+258);
  if (!(F & FZ)) {
    CYCT(b_+258, b_+261); TAIL(interactionDelete);
  }
  CYC(b_+258, b_+261);
  CYC(b_+261, b_+263); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(b_+263, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+266);
  if (F & FZ) {
    CYCT(b_+266, b_+269); TAIL(interactionDelete);
  }
  CYC(b_+266, b_+269);
  CYC(b_+269, b_+271); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+271, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+274);
  if (F & FC) {
    CYCT(b_+274, b_+277); TAIL(interactionDelete);
  }
  CYC(b_+274, b_+277);
  CYC(b_+277, b_+279); A = 0x14; // <TX_1d14
  nayru_runGenericNpc(gb);
  return;

init0c:
  CYC(b_+292, b_+294); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+294, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+297);
  if (!(F & FZ)) {
    CYCT(b_+297, b_+300); TAIL(interactionDelete);
  }
  CYC(b_+297, b_+300);
  CYC(b_+300, b_+302); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+302, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+305);
  if (F & FZ) {
    CYCT(b_+305, b_+308); TAIL(interactionDelete);
  }
  CYC(b_+305, b_+308);
  CYC(b_+308, b_+310); A = 0x3a; // GLOBALFLAG_FLAME_OF_DESPAIR_LIT
  CALL_C(b_+310, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+313);
  if (!(F & FZ)) {
    CYCT(b_+313, b_+316); TAIL(interactionDelete);
  }
  CYC(b_+313, b_+316);
  CYC(b_+316, b_+318); A = 0x15; // <TX_1d15
  CYC(b_+318, b_+320); nayru_runGenericNpc(gb);
  return;

init0d:
  CYC(b_+320, b_+322); A = 0x3a; // GLOBALFLAG_FLAME_OF_DESPAIR_LIT
  CALL_C(b_+322, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+325);
  if (F & FZ) {
    CYCT(b_+325, b_+328); TAIL(interactionDelete);
  }
  CYC(b_+325, b_+328);
  CYC(b_+328, b_+330); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+330, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+333);
  if (!(F & FZ)) {
    CYCT(b_+333, b_+336); TAIL(interactionDelete);
  }
  CYC(b_+333, b_+336);
  CYC(b_+336, b_+338); A = 0x17; // <TX_1d17
  CYC(b_+338, b_+340); nayru_runGenericNpc(gb);
  return;

init0f:
  CYC(b_+340, b_+342); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+342, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+345);
  if (!(F & FC)) {
    CYCT(b_+345, b_+348); TAIL(interactionDelete);
  }
  CYC(b_+345, b_+348);
  CYC(b_+348, b_+350); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+350, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+353);
  if (!(F & FZ)) {
    CYCT(b_+353, b_+356); TAIL(interactionDelete);
  }
  CYC(b_+353, b_+356);
  CALL_C(b_+356, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+359);
  CYC(b_+359, b_+361); C = 0x32;
  if (!(F & FZ)) {
    CALL_C_CC(b_+361, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+364);
  } else {
    CYC(b_+361, b_+364);
  }
  CYC(b_+364, b_+366); A = 0x20; // <TX_1d20
  CYC(b_+366, b_+368); nayru_runGenericNpc(gb);
  return;

init10:
  CYC(b_+368, b_+370); A = 0x1d; // >TX_1d00
  CALL_C(b_+370, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+373);
  CYC(b_+373, b_+375); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+375, b_+377); A = 0xff;
  CYC(b_+377, b_+378); mem_wr(gb, DE, A);
  CYC(b_+378, b_+381); SET_HL(nayruScript10_bank0c);
  CYC(b_+381, b_+384); interactionSetScript_hook(gb);
  return;

init11:
  CYC(b_+384, b_+385); alu_xor(gb, A);
  CALL_C(b_+385, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+388);
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(b_+388, b_+391); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(b_+391, b_+393); E = 0x15;
  CALL_C(b_+393, interBankCall_hook, 0x008a, b_+396);
  CYC(b_+396, b_+398); A = 0x1d; // >TX_1d00
  CALL_C(b_+398, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+401);
  CYC(b_+401, b_+404); SET_HL(nayruScript11_bank0c);
  CYC(b_+404, b_+407); interactionSetScript_hook(gb);
  return;

init12:
  CALL_C(b_+407, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+410);
  CYC(b_+410, b_+413); SET_BC((SYM(interactionCode15__subid00) + 1));
  CYC(b_+413, b_+416); interactionSetPosition_hook(gb);
  return;

init13:
  CYC(b_+416, b_+418); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+418, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+421);
  if (F & FZ) {
    CYCT(b_+421, b_+424); TAIL(interactionDelete);
  }
  CYC(b_+421, b_+424);
  CYC(b_+424, b_+427); SET_HL(nayruScript13_bank0c);
  CALL_C(b_+427, interactionSetScript_hook, SYM(interactionSetScript), b_+430);
  CYC(b_+430, b_+432); A = 0x1d; // >TX_1d00
  CALL_C(b_+432, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+435);
  CYC(b_+435, b_+437); A = 0x03; // MUS_OVERWORLD
  CYC(b_+437, b_+440); mem_wr(gb, wActiveMusic2, A);
  CYC(b_+440, b_+442); A = 0xff;
  CYC(b_+442, b_+445); mem_wr(gb, wActiveMusic, A);
  CYC(b_+445, b_+448); nayru_setSingingAnimation(gb, sp0_);
}

// nayruSubid00@createMusicNotes: spawn a floating music note when the animation signals
// one (animParameter). Also called from subids 2, 7 and $13 in this file.
static void nayru_createMusicNotes(GB *gb) {
  BASE(nayruSubid00);
  CYC(b_+50, b_+51); H = D;
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+55, b_+56); ret_effect(gb); return;
  }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x00);
  CYC(b_+58, b_+59); A = alu_dec8(gb, A);
  CYC(b_+59, b_+61); C = 0xfa; // -6
  if (F & FZ) {
    CYCT(b_+61, b_+63);
  } else {
    CYC(b_+61, b_+63);
    CYC(b_+63, b_+65); C = 0x08;
  }
  CYC(b_+65, b_+67); B = 0xfc;
  CYC(b_+67, b_+70); objectCreateFloatingMusicNote_hook(gb);
}

// nayruSubid00@swayHorizontally: 3 pixels left, 3 pixels right, repeat. Reached only by a
// static `call nz` from subid 0 substate 1.
static void nayru_swayHorizontally(GB *gb) {
  BASE(nayruSubid00);
  CYC(b_+133, b_+136); A = mem_rd(gb, wFrameCounter);
  CYC(b_+136, b_+138); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+138, b_+139); ret_effect(gb); return;
  }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+142); A = mem_rd(gb, wFrameCounter);
  CYC(b_+142, b_+144); alu_and(gb, 0x38);
  CYC(b_+144, b_+146); A = alu_swap(gb, A);
  CYC(b_+146, b_+147); alu_rlca(gb);
  CYC(b_+147, b_+150); SET_HL(nayruSwayXOffsets_bank08);
  CYC(b_+150, b_+151); push_effect(gb, b_+151); nayru_addAToHl(gb);
  CYC(b_+151, b_+153); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+155); B = A;
  CYC(b_+155, b_+156); A = mem_rd(gb, DE);
  CYC(b_+156, b_+157); alu_add(gb, B);
  CYC(b_+157, b_+158); mem_wr(gb, DE, A);
  CYC(b_+158, b_+159); ret_effect(gb);
}

// INTERAC_NAYRU
void interactionCode36_hook(GB *gb) {
  BASE(interactionCode36);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == SYM(nayruState0) && hook_is(gb, SYM(nayruState0), nayruState0_hook)) { nayruState0_hook(gb); return; }
    else if (jt_ == SYM(nayruState1) && hook_is(gb, SYM(nayruState1), nayruState1_hook)) { nayruState1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void nayruState0_hook(GB *gb) {
  BASE(nayruState0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CALL_C(b_+6, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+9);
  CYC(b_+9, b_+12); push_effect(gb, b_+12); nayru_initSubid(gb, sp0_);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+19); TAIL(objectMarkSolidPosition);
  }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void nayruState1_hook(GB *gb) {
  BASE(nayruState1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == SYM(nayruSubid00) && hook_is(gb, SYM(nayruSubid00), nayruSubid00_hook)) { nayruSubid00_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid01) && hook_is(gb, SYM(nayruSubid01), nayruSubid01_hook)) { nayruSubid01_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid02) && hook_is(gb, SYM(nayruSubid02), nayruSubid02_hook)) { nayruSubid02_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid03) && hook_is(gb, SYM(nayruSubid03), nayruSubid03_hook)) { nayruSubid03_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid04) && hook_is(gb, SYM(nayruSubid04), nayruSubid04_hook)) { nayruSubid04_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid05) && hook_is(gb, SYM(nayruSubid05), nayruSubid05_hook)) { nayruSubid05_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid07) && hook_is(gb, SYM(nayruSubid07), nayruSubid07_hook)) { nayruSubid07_hook(gb); return; }
    else if (jt_ == SYM(nayruAnimateAndRunScript) && hook_is(gb, SYM(nayruAnimateAndRunScript), nayruAnimateAndRunScript_hook)) { nayruAnimateAndRunScript_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid09) && hook_is(gb, SYM(nayruSubid09), nayruSubid09_hook)) { nayruSubid09_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid0a) && hook_is(gb, SYM(nayruSubid0a), nayruSubid0a_hook)) { nayruSubid0a_hook(gb); return; }
    else if (jt_ == SYM(nayruAsNpc) && hook_is(gb, SYM(nayruAsNpc), nayruAsNpc_hook)) { nayruAsNpc_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), interactionAnimate_hook)) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid10) && hook_is(gb, SYM(nayruSubid10), nayruSubid10_hook)) { nayruSubid10_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid13) && hook_is(gb, SYM(nayruSubid13), nayruSubid13_hook)) { nayruSubid13_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Subid $00: cutscene at the beginning of the game (Nayru talks, gets possessed, goes back
// in time).
// Variables:
//   var38:    "Status" of possession flickering
//   var39:    Counter for number of times to flicker palette while being possessed.
//   var3a/3b: Number of frames to stay in her "unpossessed" (var3a) or "possessed" (var3b)
//             palette.
void nayruSubid00_hook(GB *gb) {
  BASE(nayruSubid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == b_+22) { goto substate0; }
    else if (jt_ == b_+70) { goto substate1; }
    else if (jt_ == b_+167) { goto substate2; }
    else if (jt_ == b_+174) { goto substate3; }
    else if (jt_ == b_+188) { goto substate4; }
    else if (jt_ == b_+214) { goto substate5; }
    else if (jt_ == b_+241) { goto substate6; }
    else if (jt_ == b_+254) { goto substate7; }
    else if (jt_ == b_+279) { goto substate8; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  // Waiting for Link to approach (signal in $cfd0)
  CALL_C(b_+22, interactionAnimate_hook, SYM(interactionAnimate), b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+28, b_+30); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+33); nayru_createMusicNotes(gb); return;
  }
  CYC(b_+30, b_+33);
  CALL_C(b_+33, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+36);
  CYC(b_+36, b_+38); A = 0x0f;
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x01);
  CYC(b_+44, b_+47); SET_HL(nayruScript00_part1_bank0c);
  CYC(b_+47, b_+50); interactionSetScript_hook(gb);
  return;

substate1:
  // Palette is flickering while being possessed
  CALL_C(b_+70, interactionAnimate_hook, SYM(interactionAnimate), b_+73);
  CALL_C(b_+73, interactionRunScript_hook, SYM(interactionRunScript), b_+76);
  CYC(b_+76, b_+79); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+79, b_+81); alu_cp(gb, 0x16);
  if (!(F & FZ)) {
    CYCT(b_+81, b_+82); ret_effect(gb); return;
  }
  CYC(b_+81, b_+82);
  // Sway horizontally while moving
  CYC(b_+82, b_+84); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+86, b_+89); push_effect(gb, b_+89); nayru_swayHorizontally(gb);
  } else {
    CYC(b_+86, b_+89);
  }
  // Flip the OAM flags when var39 reaches 0
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+92, b_+93); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+93, b_+94); ret_effect(gb); return;
  }
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+98); A = alu_dec8(gb, A);
  CYC(b_+98, b_+100); alu_xor(gb, 0x05);
  CYC(b_+100, b_+101); A = alu_inc8(gb, A);
  CYC(b_+101, b_+102); mem_wr(gb, HL, A);
  CALL_C(b_+102, nayruUpdatePossessionPaletteDurations_hook, SYM(nayruUpdatePossessionPaletteDurations), b_+105);
  if (!(F & FZ)) {
    CYCT(b_+105, b_+107); goto substate1_stillFlickering;
  }
  CYC(b_+105, b_+107);
  // Done flickering with possession
  CALL_C(b_+107, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+110);
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x06);
  CYC(b_+114, b_+115); ret_effect(gb);
  return;
substate1_stillFlickering:
  CYC(b_+115, b_+117); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+117, b_+118); B = mem_rd(gb, HL);
  CYC(b_+118, b_+119); L = alu_inc8(gb, L);
  CYC(b_+119, b_+120); C = mem_rd(gb, HL);
  CYC(b_+120, b_+122); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+122, b_+123); A = mem_rd(gb, HL);
  CYC(b_+123, b_+125); alu_cp(gb, 0x06);
  CYC(b_+125, b_+126); A = B;
  if (!(F & FZ)) {
    CYCT(b_+126, b_+128);
  } else {
    CYC(b_+126, b_+128);
    CYC(b_+128, b_+129); A = C;
  }
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+131, b_+132); mem_wr(gb, HL, A);
  CYC(b_+132, b_+133); ret_effect(gb);
  return;

substate2:
  // Waiting for script to end
  CALL_C(b_+167, interactionRunScript_hook, SYM(interactionRunScript), b_+170);
  if (!(F & FC)) {
    CYCT(b_+170, b_+171); ret_effect(gb); return;
  }
  CYC(b_+170, b_+171);
  CYC(b_+171, b_+174); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(b_+174, b_+177); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+177, b_+179); alu_cp(gb, 0x1a);
  if (!(F & FZ)) {
    CYCT(b_+179, b_+180); ret_effect(gb); return;
  }
  CYC(b_+179, b_+180);
  CALL_C(b_+180, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+183);
  CYC(b_+183, b_+185); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+185, b_+187); mem_wr(gb, HL, 60);
  CYC(b_+187, b_+188); ret_effect(gb);
  return;

substate4:
  // Waiting 60 frames before jumping
  CALL_C(b_+188, interactionAnimate_hook, SYM(interactionAnimate), b_+191);
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if (!(F & FZ)) {
    CYCT(b_+194, b_+195); ret_effect(gb); return;
  }
  CYC(b_+194, b_+195);
  CALL_C(b_+195, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+198);
  CYC(b_+198, b_+201); SET_BC(0xfc00); // -$400
  CALL_C(b_+201, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+204);
  CYC(b_+204, b_+206); A = 0x6b; // SND_SWORDSPIN
  CALL_C(b_+206, playSound_b00_hook, SYM(playSound_b00), b_+209);
  CYC(b_+209, b_+211); A = 0x05;
  CYC(b_+211, b_+214); interactionSetAnimation_hook(gb);
  return;

substate5:
  // Jumping until off-screen
  CYC(b_+214, b_+215); alu_xor(gb, A);
  CALL_C(b_+215, objectUpdateSpeedZ_hook, SYM(objectUpdateSpeedZ), b_+218);
  CYC(b_+218, b_+220); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+220, b_+221); A = mem_rd(gb, DE);
  CYC(b_+221, b_+223); alu_cp(gb, 0x80);
  if (!(F & FC)) {
    CYCT(b_+223, b_+224); ret_effect(gb); return;
  }
  CYC(b_+223, b_+224);
  // Set position to land at
  CYC(b_+224, b_+227); SET_BC(SYM(loadTilesetUniqueGfx));
  CALL_C(b_+227, interactionSetPosition_hook, SYM(interactionSetPosition), b_+230);
  CYC(b_+230, b_+232); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+232, b_+234); mem_wr(gb, HL, 0x80);
  CYC(b_+234, b_+236); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+236, b_+238); mem_wr(gb, HL, 0x1e);
  CYC(b_+238, b_+241); interactionIncSubstate_hook(gb);
  return;

substate6:
  // Brief delay before falling back down
  CALL_C(b_+241, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+244);
  if (!(F & FZ)) {
    CYCT(b_+244, b_+245); ret_effect(gb); return;
  }
  CYC(b_+244, b_+245);
  CALL_C(b_+245, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+248);
  CYC(b_+248, b_+251); SET_BC(0x0040);
  CYC(b_+251, b_+254); objectSetSpeedZ_hook(gb);
  return;

substate7:
  // Falling back down
  CYC(b_+254, b_+256); C = 0x20;
  CALL_C(b_+256, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+259);
  if (!(F & FZ)) {
    CYCT(b_+259, b_+260); ret_effect(gb); return;
  }
  CYC(b_+259, b_+260);
  CYC(b_+260, b_+262); A = 0x1b;
  CYC(b_+262, b_+265); mem_wr(gb, wTmpcfc0 + 0x10, A);
  // Start next script
  CYC(b_+265, b_+268); SET_HL(nayruScript00_part2_bank0c);
  CALL_C(b_+268, interactionSetScript_hook, SYM(interactionSetScript), b_+271);
  CYC(b_+271, b_+273); A = 0x6a; // SND_SLASH
  CALL_C(b_+273, playSound_b00_hook, SYM(playSound_b00), b_+276);
  CYC(b_+276, b_+279); interactionIncSubstate_hook(gb);
  return;

substate8:
  // Next script running; make Nayru transparent when signal is given. Delete self when
  // the script finishes.
  CALL_C(b_+279, interactionAnimate_hook, SYM(interactionAnimate), b_+282);
  CALL_C(b_+282, interactionRunScript_hook, SYM(interactionRunScript), b_+285);
  if (!(F & FC)) {
    CYCT(b_+285, b_+287); goto substate8_scriptRunning;
  }
  CYC(b_+285, b_+287);
  CYC(b_+287, b_+290); interactionDelete_hook(gb);
  return;
substate8_scriptRunning:
  CYC(b_+290, b_+292); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+292, b_+293); A = mem_rd(gb, DE);
  CYC(b_+293, b_+294); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+294, b_+295); ret_effect(gb); return;
  }
  CYC(b_+294, b_+295);
  CYC(b_+295, b_+297); B = 0x01;
  CYC(b_+297, b_+300); TAIL(objectFlickerVisibility);
}

// Subid $01: Cutscene in Ambi's palace after getting bombs. When the script finishes, load
// the next room.
void nayruSubid01_hook(GB *gb) {
  BASE(nayruSubid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruAnimateAndRunScript_hook, SYM(nayruAnimateAndRunScript), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, DE);
  CYC(b_+5, b_+8); SET_BC(0x0146);
  CALL_C(b_+8, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+11);
  CALL_C(b_+11, resetCamera_hook, SYM(resetCamera), b_+14);
  // disableLcdAndLoadRoom doesn't load the room's objects, so load the guards here
  CYC(b_+14, b_+17); SET_HL(ambisPalaceEntranceGuards_bank12);
  CALL_C(b_+17, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+20);
  // Re-initialize the link object
  CYC(b_+20, b_+23); SET_HL(w1Link_enabled);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x03);
  CYC(b_+25, b_+27); L = (uint8_t)w1Link_yh;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x38);
  CYC(b_+29, b_+31); L = (uint8_t)w1Link_xh;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x50);
  // Re-enable the LCD
  CYC(b_+33, b_+35); A = 0x02;
  CALL_C(b_+35, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+38);
  CYC(b_+38, b_+39); SET_DE(pop_effect(gb));
  CYC(b_+39, b_+42); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+42, b_+45); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+45, playSound_b00_hook, SYM(playSound_b00), b_+48);
  CYC(b_+48, b_+51); TAIL(clearPaletteFadeVariablesAndRefreshPalettes);
}

// Subid $02: Cutscene on maku tree screen after being saved
void nayruSubid02_hook(GB *gb) {
  BASE(nayruSubid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == SYM(nayruSubid02Substate0) && hook_is(gb, SYM(nayruSubid02Substate0), nayruSubid02Substate0_hook)) { nayruSubid02Substate0_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid02Substate1) && hook_is(gb, SYM(nayruSubid02Substate1), nayruSubid02Substate1_hook)) { nayruSubid02Substate1_hook(gb); return; }
    else if (jt_ == SYM(nayruSubid02Substate2) && hook_is(gb, SYM(nayruSubid02Substate2), nayruSubid02Substate2_hook)) { nayruSubid02Substate2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// Also called by Ralph in the same cutscene. Falls through into nayruAnimateAndRunScript.
void nayruSubid02Substate0_hook(GB *gb) {
  BASE(nayruSubid02Substate0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+5); alu_cp(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7); goto createNotes;
  }
  CYC(b_+5, b_+7);
  // Signal received: choose direction randomly (left/right) and go to substate 1
  CALL_C(b_+7, getRandomNumber_hook, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x02);
  CYC(b_+12, b_+14); alu_or(gb, 0x01);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, nayruSetCounter1Randomly_hook, SYM(nayruSetCounter1Randomly), b_+20);
  CYC(b_+20, b_+23); interactionIncSubstate_hook(gb);
  return;

createNotes:
  CYC(b_+23, b_+26); push_effect(gb, SYM(nayruAnimateAndRunScript)); nayru_createMusicNotes(gb);
  TAIL(nayruAnimateAndRunScript);
}

void nayruAnimateAndRunScript_hook(GB *gb) {
  BASE(nayruAnimateAndRunScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionRunScript);
}

void nayruSubid02Substate1_hook(GB *gb) {
  BASE(nayruSubid02Substate1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+5); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7); TAIL(nayruFlipDirectionAtRandomIntervals);
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+10);
  CYC(b_+10, b_+13); SET_HL(nayruScript02_part3_bank0c);
  CALL_C(b_+13, interactionSetScript_hook, SYM(interactionSetScript), b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+21); TAIL(interactionSetAnimation);
}

// Also called by Ralph in the same cutscene. Falls through into nayruSetCounter1Randomly.
void nayruFlipDirectionAtRandomIntervals_hook(GB *gb) {
  BASE(nayruFlipDirectionAtRandomIntervals);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_xor(gb, 0x02);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CALL_C(b_+10, interactionSetAnimation_hook, SYM(interactionSetAnimation), SYM(nayruSetCounter1Randomly));
  TAIL(nayruSetCounter1Randomly);
}

void nayruSetCounter1Randomly_hook(GB *gb) {
  BASE(nayruSetCounter1Randomly);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); alu_add(gb, A);
  CYC(b_+6, b_+7); alu_add(gb, A);
  CYC(b_+7, b_+9); alu_add(gb, 0x10);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void nayruSubid02Substate2_hook(GB *gb) {
  BASE(nayruSubid02Substate2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruAnimateAndRunScript_hook, SYM(nayruAnimateAndRunScript), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); TAIL(interactionDelete);
}

// Subid $03: Cutscene with Nayru and Ralph when Link exits the black tower
void nayruSubid03_hook(GB *gb) {
  BASE(nayruSubid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == b_+13) { goto substate0; }
    else if (jt_ == b_+25) { goto substate1; }
    else if (jt_ == b_+40) { goto substate2; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+13, b_+16); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+16, b_+18); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+19); ret_effect(gb); return;
  }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, startJump_hook, SYM(startJump), b_+22);
  CYC(b_+22, b_+25); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+25, b_+27); C = 0x24;
  CALL_C(b_+27, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+30);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+31); ret_effect(gb); return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); SET_HL(nayruScript03_bank0c);
  CALL_C(b_+34, interactionSetScript_hook, SYM(interactionSetScript), b_+37);
  CYC(b_+37, b_+40); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+40, b_+43); TAIL(interactionRunScript);
}

// Subid $04: Cutscene at end of game with Ambi and her guards
void nayruSubid04_hook(GB *gb) {
  BASE(nayruSubid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+6); TAIL(nayruAnimateAndRunScript);
  }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+9, b_+11); alu_cp(gb, 0x0b);
  if (F & FC) {
    CYCT(b_+11, b_+13); TAIL(nayruAnimateAndRunScript);
  }
  CYC(b_+11, b_+13);
  CALL_C(b_+13, interactionAnimate_hook, SYM(interactionAnimate), b_+16);
  // jpab scriptHelp.turnToFaceSomething
  CYC(b_+16, b_+19); SET_HL(turnToFaceSomething_bank15);
  CYC(b_+19, b_+21); E = 0x15;
  CYC(b_+21, b_+24); TAIL(interBankCall);
}

// Subid $05: ?
void nayruSubid05_hook(GB *gb) {
  BASE(nayruSubid05);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruAnimateAndRunScript_hook, SYM(nayruAnimateAndRunScript), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03);
  if (F & FC) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  // jpab scriptHelp.turnToFaceSomething
  CYC(b_+12, b_+15); SET_HL(turnToFaceSomething_bank15);
  CYC(b_+15, b_+17); E = 0x15;
  CYC(b_+17, b_+20); TAIL(interBankCall);
}

// For Nayru subid 0 (getting possessed cutscene), this updates var3a, var3b representing
// how long Nayru's palette should be "normal" or "possessed".
// @param[out] zflag Set when Nayru is fully possessed
void nayruUpdatePossessionPaletteDurations_hook(GB *gb) {
  BASE(nayruUpdatePossessionPaletteDurations);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (nayru_jumpTable(gb));
    if (jt_ == b_+20) { goto var38_0; }
    else if (jt_ == b_+33) { goto var38_1; }
    else if (jt_ == b_+49) { goto var38_2; }
    else if (jt_ == b_+56) { goto var38_3; }
    else if (jt_ == b_+68) { goto var38_4; }
    else { HANDOFF(HL); }
  } while (0);

var38_0:
  // Decrement var3a (unpossessed palette duration), increment var3b (possessed palette
  // duration) until the two are equal, then increment var38.
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+27, b_+28); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+28, b_+29); ret_effect(gb); return;
  }
  CYC(b_+28, b_+29);

incVar38:
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+33); ret_effect(gb);
  return;

var38_1:
  // Decrement both var3a and var3b until they're both 2
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+42, b_+43); ret_effect(gb); return;
  }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x10);
  CYC(b_+47, b_+49); goto incVar38;

var38_2:
  // Wait 32 frames
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+53, b_+54); ret_effect(gb); return;
  }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); goto incVar38;

var38_3:
  // Increment both var3a and var3b until they're both 8
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+61); L = alu_inc8(gb, L);
  CYC(b_+61, b_+62); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+65); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+65, b_+66); ret_effect(gb); return;
  }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); goto incVar38;

var38_4:
  // Decrement var3a, increment var3b until it's 16
  CYC(b_+68, b_+69); H = D;
  CYC(b_+69, b_+71); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+71, b_+72); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+72, b_+73); L = alu_inc8(gb, L);
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+77); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(b_+77, b_+78); ret_effect(gb); return;
  }
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+79); ret_effect(gb);
}

// Subid $07: Cutscene with the vision of Nayru teaching you Tune of Echoes
void nayruSubid07_hook(GB *gb) {
  BASE(nayruSubid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto substate1;
  }
  CYC(b_+3, b_+5);

  // @substate0
  CALL_C(b_+5, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+10); goto substate0_counterDone;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_xor(gb, 0x80);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); ret_effect(gb);
  return;
substate0_counterDone:
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcfc0, A);
  CALL_C(b_+21, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+24);
  CALL_C(b_+24, objectSetVisible82_hook, SYM(objectSetVisible82), b_+27);
  CYC(b_+27, b_+29); A = 0x08; // MUS_NAYRU
  CYC(b_+29, b_+32); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+32, playSound_b00_hook, SYM(playSound_b00), b_+35);
  CYC(b_+35, b_+38); SET_HL(nayruScript07_bank0c);
  CYC(b_+38, b_+41); interactionSetScript_hook(gb);
  return;

substate1:
  CALL_C(b_+41, interactionRunScript_hook, SYM(interactionRunScript), b_+44);
  if (F & FC) {
    CYCT(b_+44, b_+46); goto scriptDone;
  }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+49); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+49, b_+50); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+50, b_+51); ret_effect(gb); return;
  }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+56); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+56, b_+59); push_effect(gb, b_+59); nayru_createMusicNotes(gb);
  } else {
    CYC(b_+56, b_+59);
  }
  CYC(b_+59, b_+62); interactionAnimate_hook(gb);
  return;

scriptDone:
  CYC(b_+62, b_+65); A = mem_rd(gb, wTextIsActive);
  CYC(b_+65, b_+66); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+66, b_+67); ret_effect(gb); return;
  }
  CYC(b_+66, b_+67);
  // Re-enable objects, menus
  CYC(b_+67, b_+70); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+70, b_+73); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+73, b_+76); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+76, b_+79); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);
  CYC(b_+82, b_+84); A = 0x04;
  CALL_C(b_+84, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+87);
  CALL_C(b_+87, showStatusBar_hook, SYM(showStatusBar), b_+90);
  CYC(b_+90, b_+92); A = H8(hActiveObject);
  CYC(b_+92, b_+93); D = A;
  CYC(b_+93, b_+96); TAIL(interactionDelete);
}

void nayruAsNpc_hook(GB *gb) {
  BASE(nayruAsNpc);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6); TAIL(npcFaceLinkAndAnimate);
}

// Subid $09: Cutscene where Ralph's heritage is revealed (unlinked?)
void nayruSubid09_hook(GB *gb) {
  BASE(nayruSubid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruAnimateAndRunScript_hook, SYM(nayruAnimateAndRunScript), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+8, b_+11); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+11, b_+13); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+13, setGlobalFlag_hook, SYM(setGlobalFlag), b_+16);
  CYC(b_+16, b_+19); TAIL(interactionDelete);
}

// Subid $10: Cutscene in black tower where Nayru/Ralph meet you to try to escape. Falls
// through into nayruSubid0a.
void nayruSubid10_hook(GB *gb) {
  BASE(nayruSubid10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+3, b_+5); alu_cp(gb, 0x5a);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7); TAIL(nayruSubid0a);
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+8, b_+10); TAIL(nayruSubid0a);
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = W8(w1Link_direction);
  CYC(b_+13, b_+14); A = alu_dec8(gb, A);
  CYC(b_+14, b_+16); alu_and(gb, 0x03);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+19, b_+20); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+20, b_+22); TAIL(nayruSubid0a);
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CALL_C(b_+23, interactionSetAnimation_hook, SYM(interactionSetAnimation), SYM(nayruSubid0a));
  TAIL(nayruSubid0a);
}

// Subid $0a: Cutscene where Ralph's heritage is revealed (linked?)
void nayruSubid0a_hook(GB *gb) {
  BASE(nayruSubid0a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, nayruAnimateAndRunScript_hook, SYM(nayruAnimateAndRunScript), b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); TAIL(interactionDelete);
}

// Subid $13: NPC after completing game (singing to animals). Falls through into
// nayruRunScriptWithConditionalAnimation.
void nayruSubid13_hook(GB *gb) {
  BASE(nayruSubid13);
  CYC(b_+0, b_+3); push_effect(gb, SYM(nayruRunScriptWithConditionalAnimation)); nayru_createMusicNotes(gb);
  TAIL(nayruRunScriptWithConditionalAnimation);
}

// This is called by Ralph as well.
void nayruRunScriptWithConditionalAnimation_hook(GB *gb) {
  BASE(nayruRunScriptWithConditionalAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+7, interactionAnimate_hook, SYM(interactionAnimate), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  CALL_C(b_+10, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+13);
  CYC(b_+13, b_+16); TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}
