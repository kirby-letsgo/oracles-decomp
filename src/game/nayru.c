#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// nayruSubid00@swayHorizontally@xOffsets: $ff $ff $ff $00 $01 $01 $01 $00
#define nayruSwayXOffsets_bank08 0x6b0e

// mainScripts.* (bank $0c), objectData.* (bank $12) and scriptHelp.* (bank $15), referenced
// by address only.
#define genericNpcScript_bank0c 0x45f0
#define nayruScript00_part1_bank0c 0x565a
#define nayruScript00_part2_bank0c 0x56b5
#define nayruScript01_bank0c 0x56cf
#define nayruScript02_part1_bank0c 0x56d3
#define nayruScript02_part2_bank0c 0x56e8
#define nayruScript02_part3_bank0c 0x56ec
#define nayruScript03_bank0c 0x571a
#define nayruScript04_part1_bank0c 0x571e
#define nayruScript04_part2_bank0c 0x573e
#define nayruScript05_bank0c 0x574e
#define nayruScript07_bank0c 0x5760
#define nayruScript08_bank0c 0x5764
#define nayruScript09_bank0c 0x5787
#define nayruScript0a_bank0c 0x57a3
#define nayruScript10_bank0c 0x57e0
#define nayruScript11_bank0c 0x57e4
#define nayruScript13_bank0c 0x57e8
#define ambisPalaceEntranceGuards_bank12 0x77fa
#define turnToFaceSomething_bank15 0x5613
#define objectWritePositionTocfd5_bank15 0x741b

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
  CYC(0x68e9, 0x68eb); A = 0x97; // PALH_97
  CYC(0x68eb, 0x68ee); loadPaletteHeader_hook(gb);
}

// nayruState0@setSingingAnimation: tail shared by @init00 (fallthrough) and @init13 (`jp`).
static void nayru_setSingingAnimation(GB *gb, uint16_t sp0_) {
  CYC(0x68cb, 0x68cd); A = 0x04;
  CALL_C(0x68cd, interactionSetAnimation_hook, 0x262e, 0x68d0);
  CYC(0x68d0, 0x68d3); interactionLoadExtraGraphics_hook(gb);
}

// nayruState0@init0e: also called from ambi subids 4 and 5 through interBankCall (to
// initialize possessed palettes), so it must stay hookable.
void nayruState0__init0e_hook(GB *gb) {
  CYC(0x68e4, 0x68e6); A = 0x06;
  CYC(0x68e6, 0x68e8); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x68e8, 0x68e9); mem_wr(gb, DE, A);
  nayru_loadEvilPalette(gb);
}

// nayruState0@init03: reached from the init table, from @init04's `jr nz` and by fallthrough.
static void nayru_init03(GB *gb) {
  CYC(0x6929, 0x692a); alu_xor(gb, A);
  CYC(0x692a, 0x692d); interactionSetAnimation_hook(gb);
}

// nayruState0@runGenericNpc: A = low byte of the text ID (high byte TX_1d00).
static void nayru_runGenericNpc(GB *gb) {
  CYC(0x699a, 0x699c); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x699c, 0x699d); mem_wr(gb, DE, A);
  CYC(0x699d, 0x699e); E = alu_inc8(gb, E);
  CYC(0x699e, 0x69a0); A = 0x1d;
  CYC(0x69a0, 0x69a1); mem_wr(gb, DE, A);
  CYC(0x69a1, 0x69a4); SET_HL(genericNpcScript_bank0c);
  CYC(0x69a4, 0x69a7); interactionSetScript_hook(gb);
}

// nayruState0@initSubid: per-subid initialization. Reached only by a static `call` from
// nayruState0.
static void nayru_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x6897, 0x6899); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6899, 0x689a); A = mem_rd(gb, DE);
  CYC(0x689a, 0x689b); push_effect(gb, 0x689b);
  switch (nayru_jumpTable(gb)) {
    case 0x68c3: goto init00;
    case 0x68d3: goto init01;
    case 0x68ee: goto init02;
    case 0x6929: nayru_init03(gb); return;
    case 0x690e: goto init04;
    case 0x692d: goto init05;
    case 0x693b: goto init06;
    case 0x6940: goto init07;
    case 0x694b: goto init08;
    case 0x6959: goto init09;
    case 0x695f: goto init0a;
    case 0x6980: goto init0b;
    case 0x69a7: goto init0c;
    case 0x69c3: goto init0d;
    case 0x68e4: nayruState0__init0e_hook(gb); return;
    case 0x69d7: goto init0f;
    case 0x69f3: goto init10;
    case 0x6a03: goto init11;
    case 0x6a1a: goto init12;
    case 0x6a23: goto init13;
    default: HANDOFF(HL);
  }

init00:
  CYC(0x68c3, 0x68c5); A = 0x03;
  CALL_C(0x68c5, setMusicVolume_hook, 0x0cad, 0x68c8);
  CYC(0x68c8, 0x68cb); push_effect(gb, 0x68cb); nayru_loadEvilPalette(gb);
  nayru_setSingingAnimation(gb, sp0_);
  return;

init01:
  CYC(0x68d3, 0x68d5); A = 0x0b; // GLOBALFLAG_0b
  CALL_C(0x68d5, checkGlobalFlag_hook, 0x31f3, 0x68d8);
  if (!(F & FZ)) {
    CYCT(0x68d8, 0x68db); interactionDelete_hook(gb); return;
  }
  CYC(0x68d8, 0x68db);
  CALL_C(0x68db, objectSetInvisible_hook, 0x1e7b, 0x68de);
  CYC(0x68de, 0x68e1); SET_HL(nayruScript01_bank0c);
  CALL_C(0x68e1, interactionSetScript_hook, 0x2544, 0x68e4);
  nayruState0__init0e_hook(gb);
  return;

init02:
  CYC(0x68ee, 0x68f1); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x68f1, 0x68f3); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x68f3, 0x68f5); goto init02_part2;
  }
  CYC(0x68f3, 0x68f5);
  CYC(0x68f5, 0x68f7); A = 0x05;
  CALL_C(0x68f7, interactionSetAnimation_hook, 0x262e, 0x68fa);
  CYC(0x68fa, 0x68fd); SET_HL(nayruScript02_part1_bank0c);
  CALL_C(0x68fd, interactionSetScript_hook, 0x2544, 0x6900);
  CYC(0x6900, 0x6903); objectSetInvisible_hook(gb);
  return;
init02_part2:
  CYC(0x6903, 0x6905); A = 0x02;
  CALL_C(0x6905, interactionSetAnimation_hook, 0x262e, 0x6908);
  CYC(0x6908, 0x690b); SET_HL(nayruScript02_part2_bank0c);
  CYC(0x690b, 0x690e); interactionSetScript_hook(gb);
  return;

init04:
  CYC(0x690e, 0x6911); SET_HL(nayruScript04_part1_bank0c);
  CYC(0x6911, 0x6914); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6914, 0x6916); alu_cp(gb, 0x0b);
  if (!(F & FZ)) {
    CYCT(0x6916, 0x6918); goto init04_setScript;
  }
  CYC(0x6916, 0x6918);
  CYC(0x6918, 0x691b); SET_BC(0x4840);
  CALL_C(0x691b, interactionSetPosition_hook, 0x2773, 0x691e);
  CALL_C(0x691e, checkIsLinkedGame_hook, 0x1992, 0x6921);
  if (!(F & FZ)) {
    CYCT(0x6921, 0x6923); nayru_init03(gb); return;
  }
  CYC(0x6921, 0x6923);
  CYC(0x6923, 0x6926); SET_HL(nayruScript04_part2_bank0c);
init04_setScript:
  CALL_C(0x6926, interactionSetScript_hook, 0x2544, 0x6929);
  nayru_init03(gb);
  return;

init05:
  CYC(0x692d, 0x692f); A = 0x05;
  CALL_C(0x692f, interactionSetAnimation_hook, 0x262e, 0x6932);
  CYC(0x6932, 0x6935); SET_HL(nayruScript05_bank0c);
  CALL_C(0x6935, interactionSetScript_hook, 0x2544, 0x6938);
  CYC(0x6938, 0x693b); objectSetInvisible_hook(gb);
  return;

init06:
  CYC(0x693b, 0x693d); A = 0x07;
  CYC(0x693d, 0x6940); interactionSetAnimation_hook(gb);
  return;

init07:
  CYC(0x6940, 0x6942); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6942, 0x6944); A = 0x1e;
  CYC(0x6944, 0x6945); mem_wr(gb, DE, A);
  CALL_C(0x6945, interactionLoadExtraGraphics_hook, 0x2781, 0x6948);
  CYC(0x6948, 0x694b); interactionSetAlwaysUpdateBit_hook(gb);
  return;

init08:
  CYC(0x694b, 0x694e); SET_HL(nayruScript08_bank0c);
  CALL_C(0x694e, interactionSetScript_hook, 0x2544, 0x6951);
  CALL_C(0x6951, objectSetVisible82_hook, 0x1e69, 0x6954);
  CYC(0x6954, 0x6956); A = 0x03;
  CYC(0x6956, 0x6959); interactionSetAnimation_hook(gb);
  return;

init09:
  CYC(0x6959, 0x695c); SET_HL(nayruScript09_bank0c);
  CYC(0x695c, 0x695f); interactionSetScript_hook(gb);
  return;

init0a:
  CALL_C(0x695f, checkIsLinkedGame_hook, 0x1992, 0x6962);
  if (F & FZ) {
    CYCT(0x6962, 0x6965); interactionDelete_hook(gb); return;
  }
  CYC(0x6962, 0x6965);
  CYC(0x6965, 0x6967); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x6967, checkTreasureObtained_hook, 0x1748, 0x696a);
  if (!(F & FC)) {
    CYCT(0x696a, 0x696d); interactionDelete_hook(gb); return;
  }
  CYC(0x696a, 0x696d);
  CYC(0x696d, 0x696f); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x696f, checkGlobalFlag_hook, 0x31f3, 0x6972);
  if (!(F & FZ)) {
    CYCT(0x6972, 0x6975); interactionDelete_hook(gb); return;
  }
  CYC(0x6972, 0x6975);
  CYC(0x6975, 0x6977); A = 0x01;
  CALL_C(0x6977, interactionSetAnimation_hook, 0x262e, 0x697a);
  CYC(0x697a, 0x697d); SET_HL(nayruScript0a_bank0c);
  CYC(0x697d, 0x6980); interactionSetScript_hook(gb);
  return;

init0b:
  CYC(0x6980, 0x6982); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x6982, checkGlobalFlag_hook, 0x31f3, 0x6985);
  if (!(F & FZ)) {
    CYCT(0x6985, 0x6988); interactionDelete_hook(gb); return;
  }
  CYC(0x6985, 0x6988);
  CYC(0x6988, 0x698a); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(0x698a, checkGlobalFlag_hook, 0x31f3, 0x698d);
  if (F & FZ) {
    CYCT(0x698d, 0x6990); interactionDelete_hook(gb); return;
  }
  CYC(0x698d, 0x6990);
  CYC(0x6990, 0x6992); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x6992, checkTreasureObtained_hook, 0x1748, 0x6995);
  if (F & FC) {
    CYCT(0x6995, 0x6998); interactionDelete_hook(gb); return;
  }
  CYC(0x6995, 0x6998);
  CYC(0x6998, 0x699a); A = 0x14; // <TX_1d14
  nayru_runGenericNpc(gb);
  return;

init0c:
  CYC(0x69a7, 0x69a9); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x69a9, checkGlobalFlag_hook, 0x31f3, 0x69ac);
  if (!(F & FZ)) {
    CYCT(0x69ac, 0x69af); interactionDelete_hook(gb); return;
  }
  CYC(0x69ac, 0x69af);
  CYC(0x69af, 0x69b1); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x69b1, checkGlobalFlag_hook, 0x31f3, 0x69b4);
  if (F & FZ) {
    CYCT(0x69b4, 0x69b7); interactionDelete_hook(gb); return;
  }
  CYC(0x69b4, 0x69b7);
  CYC(0x69b7, 0x69b9); A = 0x3a; // GLOBALFLAG_FLAME_OF_DESPAIR_LIT
  CALL_C(0x69b9, checkGlobalFlag_hook, 0x31f3, 0x69bc);
  if (!(F & FZ)) {
    CYCT(0x69bc, 0x69bf); interactionDelete_hook(gb); return;
  }
  CYC(0x69bc, 0x69bf);
  CYC(0x69bf, 0x69c1); A = 0x15; // <TX_1d15
  CYC(0x69c1, 0x69c3); nayru_runGenericNpc(gb);
  return;

init0d:
  CYC(0x69c3, 0x69c5); A = 0x3a; // GLOBALFLAG_FLAME_OF_DESPAIR_LIT
  CALL_C(0x69c5, checkGlobalFlag_hook, 0x31f3, 0x69c8);
  if (F & FZ) {
    CYCT(0x69c8, 0x69cb); interactionDelete_hook(gb); return;
  }
  CYC(0x69c8, 0x69cb);
  CYC(0x69cb, 0x69cd); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x69cd, checkGlobalFlag_hook, 0x31f3, 0x69d0);
  if (!(F & FZ)) {
    CYCT(0x69d0, 0x69d3); interactionDelete_hook(gb); return;
  }
  CYC(0x69d0, 0x69d3);
  CYC(0x69d3, 0x69d5); A = 0x17; // <TX_1d17
  CYC(0x69d5, 0x69d7); nayru_runGenericNpc(gb);
  return;

init0f:
  CYC(0x69d7, 0x69d9); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x69d9, checkTreasureObtained_hook, 0x1748, 0x69dc);
  if (!(F & FC)) {
    CYCT(0x69dc, 0x69df); interactionDelete_hook(gb); return;
  }
  CYC(0x69dc, 0x69df);
  CYC(0x69df, 0x69e1); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x69e1, checkGlobalFlag_hook, 0x31f3, 0x69e4);
  if (!(F & FZ)) {
    CYCT(0x69e4, 0x69e7); interactionDelete_hook(gb); return;
  }
  CYC(0x69e4, 0x69e7);
  CALL_C(0x69e7, checkIsLinkedGame_hook, 0x1992, 0x69ea);
  CYC(0x69ea, 0x69ec); C = 0x32;
  if (!(F & FZ)) {
    CALL_C_CC(0x69ec, objectSetShortPosition_hook, 0x20c3, 0x69ef);
  } else {
    CYC(0x69ec, 0x69ef);
  }
  CYC(0x69ef, 0x69f1); A = 0x20; // <TX_1d20
  CYC(0x69f1, 0x69f3); nayru_runGenericNpc(gb);
  return;

init10:
  CYC(0x69f3, 0x69f5); A = 0x1d; // >TX_1d00
  CALL_C(0x69f5, interactionSetHighTextIndex_hook, 0x253b, 0x69f8);
  CYC(0x69f8, 0x69fa); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x69fa, 0x69fc); A = 0xff;
  CYC(0x69fc, 0x69fd); mem_wr(gb, DE, A);
  CYC(0x69fd, 0x6a00); SET_HL(nayruScript10_bank0c);
  CYC(0x6a00, 0x6a03); interactionSetScript_hook(gb);
  return;

init11:
  CYC(0x6a03, 0x6a04); alu_xor(gb, A);
  CALL_C(0x6a04, interactionSetAnimation_hook, 0x262e, 0x6a07);
  // callab scriptHelp.objectWritePositionTocfd5
  CYC(0x6a07, 0x6a0a); SET_HL(objectWritePositionTocfd5_bank15);
  CYC(0x6a0a, 0x6a0c); E = 0x15;
  CALL_C(0x6a0c, interBankCall_hook, 0x008a, 0x6a0f);
  CYC(0x6a0f, 0x6a11); A = 0x1d; // >TX_1d00
  CALL_C(0x6a11, interactionSetHighTextIndex_hook, 0x253b, 0x6a14);
  CYC(0x6a14, 0x6a17); SET_HL(nayruScript11_bank0c);
  CYC(0x6a17, 0x6a1a); interactionSetScript_hook(gb);
  return;

init12:
  CALL_C(0x6a1a, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6a1d);
  CYC(0x6a1d, 0x6a20); SET_BC(0x4870);
  CYC(0x6a20, 0x6a23); interactionSetPosition_hook(gb);
  return;

init13:
  CYC(0x6a23, 0x6a25); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x6a25, checkGlobalFlag_hook, 0x31f3, 0x6a28);
  if (F & FZ) {
    CYCT(0x6a28, 0x6a2b); interactionDelete_hook(gb); return;
  }
  CYC(0x6a28, 0x6a2b);
  CYC(0x6a2b, 0x6a2e); SET_HL(nayruScript13_bank0c);
  CALL_C(0x6a2e, interactionSetScript_hook, 0x2544, 0x6a31);
  CYC(0x6a31, 0x6a33); A = 0x1d; // >TX_1d00
  CALL_C(0x6a33, interactionSetHighTextIndex_hook, 0x253b, 0x6a36);
  CYC(0x6a36, 0x6a38); A = 0x03; // MUS_OVERWORLD
  CYC(0x6a38, 0x6a3b); mem_wr(gb, wActiveMusic2, A);
  CYC(0x6a3b, 0x6a3d); A = 0xff;
  CYC(0x6a3d, 0x6a40); mem_wr(gb, wActiveMusic, A);
  CYC(0x6a40, 0x6a43); nayru_setSingingAnimation(gb, sp0_);
}

// nayruSubid00@createMusicNotes: spawn a floating music note when the animation signals
// one (animParameter). Also called from subids 2, 7 and $13 in this file.
static void nayru_createMusicNotes(GB *gb) {
  CYC(0x6aa1, 0x6aa2); H = D;
  CYC(0x6aa2, 0x6aa4); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x6aa4, 0x6aa5); A = mem_rd(gb, HL);
  CYC(0x6aa5, 0x6aa6); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6aa6, 0x6aa7); ret_effect(gb); return;
  }
  CYC(0x6aa6, 0x6aa7);
  CYC(0x6aa7, 0x6aa9); mem_wr(gb, HL, 0x00);
  CYC(0x6aa9, 0x6aaa); A = alu_dec8(gb, A);
  CYC(0x6aaa, 0x6aac); C = 0xfa; // -6
  if (F & FZ) {
    CYCT(0x6aac, 0x6aae);
  } else {
    CYC(0x6aac, 0x6aae);
    CYC(0x6aae, 0x6ab0); C = 0x08;
  }
  CYC(0x6ab0, 0x6ab2); B = 0xfc;
  CYC(0x6ab2, 0x6ab5); objectCreateFloatingMusicNote_hook(gb);
}

// nayruSubid00@swayHorizontally: 3 pixels left, 3 pixels right, repeat. Reached only by a
// static `call nz` from subid 0 substate 1.
static void nayru_swayHorizontally(GB *gb) {
  CYC(0x6af4, 0x6af7); A = mem_rd(gb, wFrameCounter);
  CYC(0x6af7, 0x6af9); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x6af9, 0x6afa); ret_effect(gb); return;
  }
  CYC(0x6af9, 0x6afa);
  CYC(0x6afa, 0x6afd); A = mem_rd(gb, wFrameCounter);
  CYC(0x6afd, 0x6aff); alu_and(gb, 0x38);
  CYC(0x6aff, 0x6b01); A = alu_swap(gb, A);
  CYC(0x6b01, 0x6b02); alu_rlca(gb);
  CYC(0x6b02, 0x6b05); SET_HL(nayruSwayXOffsets_bank08);
  CYC(0x6b05, 0x6b06); push_effect(gb, 0x6b06); nayru_addAToHl(gb);
  CYC(0x6b06, 0x6b08); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x6b08, 0x6b09); A = mem_rd(gb, HL);
  CYC(0x6b09, 0x6b0a); B = A;
  CYC(0x6b0a, 0x6b0b); A = mem_rd(gb, DE);
  CYC(0x6b0b, 0x6b0c); alu_add(gb, B);
  CYC(0x6b0c, 0x6b0d); mem_wr(gb, DE, A);
  CYC(0x6b0d, 0x6b0e); ret_effect(gb);
}

// INTERAC_NAYRU
void interactionCode36_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x687b, 0x687d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x687d, 0x687e); A = mem_rd(gb, DE);
  CYC(0x687e, 0x687f); push_effect(gb, 0x687f);
  switch (nayru_jumpTable(gb)) {
    case 0x6883: nayruState0_hook(gb); return;
    case 0x6a43: nayruState1_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void nayruState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6883, 0x6885); A = 0x01;
  CYC(0x6885, 0x6886); mem_wr(gb, DE, A);
  CALL_C(0x6886, interactionInitGraphics_hook, 0x15fb, 0x6889);
  CALL_C(0x6889, objectSetVisiblec2_hook, 0x1e45, 0x688c);
  CYC(0x688c, 0x688f); push_effect(gb, 0x688f); nayru_initSubid(gb, sp0_);
  CYC(0x688f, 0x6891); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x6891, 0x6892); A = mem_rd(gb, DE);
  CYC(0x6892, 0x6893); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6893, 0x6896); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x6893, 0x6896);
  CYC(0x6896, 0x6897); ret_effect(gb);
}

void nayruState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a43, 0x6a45); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6a45, 0x6a46); A = mem_rd(gb, DE);
  CYC(0x6a46, 0x6a47); push_effect(gb, 0x6a47);
  switch (nayru_jumpTable(gb)) {
    case 0x6a6f: nayruSubid00_hook(gb); return;
    case 0x6b9b: nayruSubid01_hook(gb); return;
    case 0x6bce: nayruSubid02_hook(gb); return;
    case 0x6c2e: nayruSubid03_hook(gb); return;
    case 0x6c59: nayruSubid04_hook(gb); return;
    case 0x6c71: nayruSubid05_hook(gb); return;
    case 0x6cd4: nayruSubid07_hook(gb); return;
    case 0x6bf2: nayruAnimateAndRunScript_hook(gb); return;
    case 0x6d3a: nayruSubid09_hook(gb); return;
    case 0x6d67: nayruSubid0a_hook(gb); return;
    case 0x6d34: nayruAsNpc_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x6d4d: nayruSubid10_hook(gb); return;
    case 0x6d6e: nayruSubid13_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// Subid $00: cutscene at the beginning of the game (Nayru talks, gets possessed, goes back
// in time).
// Variables:
//   var38:    "Status" of possession flickering
//   var39:    Counter for number of times to flicker palette while being possessed.
//   var3a/3b: Number of frames to stay in her "unpossessed" (var3a) or "possessed" (var3b)
//             palette.
void nayruSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a6f, 0x6a71); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6a71, 0x6a72); A = mem_rd(gb, DE);
  CYC(0x6a72, 0x6a73); push_effect(gb, 0x6a73);
  switch (nayru_jumpTable(gb)) {
    case 0x6a85: goto substate0;
    case 0x6ab5: goto substate1;
    case 0x6b16: goto substate2;
    case 0x6b1d: goto substate3;
    case 0x6b2b: goto substate4;
    case 0x6b45: goto substate5;
    case 0x6b60: goto substate6;
    case 0x6b6d: goto substate7;
    case 0x6b86: goto substate8;
    default: HANDOFF(HL);
  }

substate0:
  // Waiting for Link to approach (signal in $cfd0)
  CALL_C(0x6a85, interactionAnimate_hook, 0x261b, 0x6a88);
  CYC(0x6a88, 0x6a8b); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6a8b, 0x6a8d); alu_cp(gb, 0x09);
  if (!(F & FZ)) {
    CYCT(0x6a8d, 0x6a90); nayru_createMusicNotes(gb); return;
  }
  CYC(0x6a8d, 0x6a90);
  CALL_C(0x6a90, interactionIncSubstate_hook, 0x23e5, 0x6a93);
  CYC(0x6a93, 0x6a95); A = 0x0f;
  CYC(0x6a95, 0x6a97); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x6a97, 0x6a98); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a98, 0x6a99); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6a99, 0x6a9b); mem_wr(gb, HL, 0x01);
  CYC(0x6a9b, 0x6a9e); SET_HL(nayruScript00_part1_bank0c);
  CYC(0x6a9e, 0x6aa1); interactionSetScript_hook(gb);
  return;

substate1:
  // Palette is flickering while being possessed
  CALL_C(0x6ab5, interactionAnimate_hook, 0x261b, 0x6ab8);
  CALL_C(0x6ab8, interactionRunScript_hook, 0x2552, 0x6abb);
  CYC(0x6abb, 0x6abe); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6abe, 0x6ac0); alu_cp(gb, 0x16);
  if (!(F & FZ)) {
    CYCT(0x6ac0, 0x6ac1); ret_effect(gb); return;
  }
  CYC(0x6ac0, 0x6ac1);
  // Sway horizontally while moving
  CYC(0x6ac1, 0x6ac3); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6ac3, 0x6ac4); A = mem_rd(gb, DE);
  CYC(0x6ac4, 0x6ac5); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6ac5, 0x6ac8); push_effect(gb, 0x6ac8); nayru_swayHorizontally(gb);
  } else {
    CYC(0x6ac5, 0x6ac8);
  }
  // Flip the OAM flags when var39 reaches 0
  CYC(0x6ac8, 0x6ac9); H = D;
  CYC(0x6ac9, 0x6acb); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x6acb, 0x6acc); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x6acc, 0x6acd); ret_effect(gb); return;
  }
  CYC(0x6acc, 0x6acd);
  CYC(0x6acd, 0x6acf); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x6acf, 0x6ad0); A = mem_rd(gb, HL);
  CYC(0x6ad0, 0x6ad1); A = alu_dec8(gb, A);
  CYC(0x6ad1, 0x6ad3); alu_xor(gb, 0x05);
  CYC(0x6ad3, 0x6ad4); A = alu_inc8(gb, A);
  CYC(0x6ad4, 0x6ad5); mem_wr(gb, HL, A);
  CALL_C(0x6ad5, nayruUpdatePossessionPaletteDurations_hook, 0x6c85, 0x6ad8);
  if (!(F & FZ)) {
    CYCT(0x6ad8, 0x6ada); goto substate1_stillFlickering;
  }
  CYC(0x6ad8, 0x6ada);
  // Done flickering with possession
  CALL_C(0x6ada, interactionIncSubstate_hook, 0x23e5, 0x6add);
  CYC(0x6add, 0x6adf); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x6adf, 0x6ae1); mem_wr(gb, HL, 0x06);
  CYC(0x6ae1, 0x6ae2); ret_effect(gb);
  return;
substate1_stillFlickering:
  CYC(0x6ae2, 0x6ae4); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6ae4, 0x6ae5); B = mem_rd(gb, HL);
  CYC(0x6ae5, 0x6ae6); L = alu_inc8(gb, L);
  CYC(0x6ae6, 0x6ae7); C = mem_rd(gb, HL);
  CYC(0x6ae7, 0x6ae9); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x6ae9, 0x6aea); A = mem_rd(gb, HL);
  CYC(0x6aea, 0x6aec); alu_cp(gb, 0x06);
  CYC(0x6aec, 0x6aed); A = B;
  if (!(F & FZ)) {
    CYCT(0x6aed, 0x6aef);
  } else {
    CYC(0x6aed, 0x6aef);
    CYC(0x6aef, 0x6af0); A = C;
  }
  CYC(0x6af0, 0x6af2); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x6af2, 0x6af3); mem_wr(gb, HL, A);
  CYC(0x6af3, 0x6af4); ret_effect(gb);
  return;

substate2:
  // Waiting for script to end
  CALL_C(0x6b16, interactionRunScript_hook, 0x2552, 0x6b19);
  if (!(F & FC)) {
    CYCT(0x6b19, 0x6b1a); ret_effect(gb); return;
  }
  CYC(0x6b19, 0x6b1a);
  CYC(0x6b1a, 0x6b1d); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(0x6b1d, 0x6b20); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6b20, 0x6b22); alu_cp(gb, 0x1a);
  if (!(F & FZ)) {
    CYCT(0x6b22, 0x6b23); ret_effect(gb); return;
  }
  CYC(0x6b22, 0x6b23);
  CALL_C(0x6b23, interactionIncSubstate_hook, 0x23e5, 0x6b26);
  CYC(0x6b26, 0x6b28); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6b28, 0x6b2a); mem_wr(gb, HL, 60);
  CYC(0x6b2a, 0x6b2b); ret_effect(gb);
  return;

substate4:
  // Waiting 60 frames before jumping
  CALL_C(0x6b2b, interactionAnimate_hook, 0x261b, 0x6b2e);
  CALL_C(0x6b2e, interactionDecCounter1_hook, 0x23cc, 0x6b31);
  if (!(F & FZ)) {
    CYCT(0x6b31, 0x6b32); ret_effect(gb); return;
  }
  CYC(0x6b31, 0x6b32);
  CALL_C(0x6b32, interactionIncSubstate_hook, 0x23e5, 0x6b35);
  CYC(0x6b35, 0x6b38); SET_BC(0xfc00); // -$400
  CALL_C(0x6b38, objectSetSpeedZ_hook, 0x239d, 0x6b3b);
  CYC(0x6b3b, 0x6b3d); A = 0x6b; // SND_SWORDSPIN
  CALL_C(0x6b3d, playSound_b00_hook, 0x0c98, 0x6b40);
  CYC(0x6b40, 0x6b42); A = 0x05;
  CYC(0x6b42, 0x6b45); interactionSetAnimation_hook(gb);
  return;

substate5:
  // Jumping until off-screen
  CYC(0x6b45, 0x6b46); alu_xor(gb, A);
  CALL_C(0x6b46, objectUpdateSpeedZ_hook, 0x1f45, 0x6b49);
  CYC(0x6b49, 0x6b4b); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6b4b, 0x6b4c); A = mem_rd(gb, DE);
  CYC(0x6b4c, 0x6b4e); alu_cp(gb, 0x80);
  if (!(F & FC)) {
    CYCT(0x6b4e, 0x6b4f); ret_effect(gb); return;
  }
  CYC(0x6b4e, 0x6b4f);
  // Set position to land at
  CYC(0x6b4f, 0x6b52); SET_BC(0x3828);
  CALL_C(0x6b52, interactionSetPosition_hook, 0x2773, 0x6b55);
  CYC(0x6b55, 0x6b57); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6b57, 0x6b59); mem_wr(gb, HL, 0x80);
  CYC(0x6b59, 0x6b5b); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6b5b, 0x6b5d); mem_wr(gb, HL, 0x1e);
  CYC(0x6b5d, 0x6b60); interactionIncSubstate_hook(gb);
  return;

substate6:
  // Brief delay before falling back down
  CALL_C(0x6b60, interactionDecCounter1_hook, 0x23cc, 0x6b63);
  if (!(F & FZ)) {
    CYCT(0x6b63, 0x6b64); ret_effect(gb); return;
  }
  CYC(0x6b63, 0x6b64);
  CALL_C(0x6b64, interactionIncSubstate_hook, 0x23e5, 0x6b67);
  CYC(0x6b67, 0x6b6a); SET_BC(0x0040);
  CYC(0x6b6a, 0x6b6d); objectSetSpeedZ_hook(gb);
  return;

substate7:
  // Falling back down
  CYC(0x6b6d, 0x6b6f); C = 0x20;
  CALL_C(0x6b6f, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6b72);
  if (!(F & FZ)) {
    CYCT(0x6b72, 0x6b73); ret_effect(gb); return;
  }
  CYC(0x6b72, 0x6b73);
  CYC(0x6b73, 0x6b75); A = 0x1b;
  CYC(0x6b75, 0x6b78); mem_wr(gb, wTmpcfc0 + 0x10, A);
  // Start next script
  CYC(0x6b78, 0x6b7b); SET_HL(nayruScript00_part2_bank0c);
  CALL_C(0x6b7b, interactionSetScript_hook, 0x2544, 0x6b7e);
  CYC(0x6b7e, 0x6b80); A = 0x6a; // SND_SLASH
  CALL_C(0x6b80, playSound_b00_hook, 0x0c98, 0x6b83);
  CYC(0x6b83, 0x6b86); interactionIncSubstate_hook(gb);
  return;

substate8:
  // Next script running; make Nayru transparent when signal is given. Delete self when
  // the script finishes.
  CALL_C(0x6b86, interactionAnimate_hook, 0x261b, 0x6b89);
  CALL_C(0x6b89, interactionRunScript_hook, 0x2552, 0x6b8c);
  if (!(F & FC)) {
    CYCT(0x6b8c, 0x6b8e); goto substate8_scriptRunning;
  }
  CYC(0x6b8c, 0x6b8e);
  CYC(0x6b8e, 0x6b91); interactionDelete_hook(gb);
  return;
substate8_scriptRunning:
  CYC(0x6b91, 0x6b93); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x6b93, 0x6b94); A = mem_rd(gb, DE);
  CYC(0x6b94, 0x6b95); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6b95, 0x6b96); ret_effect(gb); return;
  }
  CYC(0x6b95, 0x6b96);
  CYC(0x6b96, 0x6b98); B = 0x01;
  CYC(0x6b98, 0x6b9b); objectFlickerVisibility_hook(gb);
}

// Subid $01: Cutscene in Ambi's palace after getting bombs. When the script finishes, load
// the next room.
void nayruSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b9b, nayruAnimateAndRunScript_hook, 0x6bf2, 0x6b9e);
  if (!(F & FC)) {
    CYCT(0x6b9e, 0x6b9f); ret_effect(gb); return;
  }
  CYC(0x6b9e, 0x6b9f);
  CYC(0x6b9f, 0x6ba0); push_effect(gb, DE);
  CYC(0x6ba0, 0x6ba3); SET_BC(0x0146);
  CALL_C(0x6ba3, disableLcdAndLoadRoom_hook, 0x30b0, 0x6ba6);
  CALL_C(0x6ba6, resetCamera_hook, 0x12ce, 0x6ba9);
  // disableLcdAndLoadRoom doesn't load the room's objects, so load the guards here
  CYC(0x6ba9, 0x6bac); SET_HL(ambisPalaceEntranceGuards_bank12);
  CALL_C(0x6bac, parseGivenObjectData_b00_hook, 0x3171, 0x6baf);
  // Re-initialize the link object
  CYC(0x6baf, 0x6bb2); SET_HL(w1Link_enabled);
  CYC(0x6bb2, 0x6bb4); mem_wr(gb, HL, 0x03);
  CYC(0x6bb4, 0x6bb6); L = (uint8_t)w1Link_yh;
  CYC(0x6bb6, 0x6bb8); mem_wr(gb, HL, 0x38);
  CYC(0x6bb8, 0x6bba); L = (uint8_t)w1Link_xh;
  CYC(0x6bba, 0x6bbc); mem_wr(gb, HL, 0x50);
  // Re-enable the LCD
  CYC(0x6bbc, 0x6bbe); A = 0x02;
  CALL_C(0x6bbe, loadGfxRegisterStateIndex_hook, 0x02ea, 0x6bc1);
  CYC(0x6bc1, 0x6bc2); SET_DE(pop_effect(gb));
  CYC(0x6bc2, 0x6bc5); A = mem_rd(gb, wActiveMusic2);
  CYC(0x6bc5, 0x6bc8); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x6bc8, playSound_b00_hook, 0x0c98, 0x6bcb);
  CYC(0x6bcb, 0x6bce); clearPaletteFadeVariablesAndRefreshPalettes_hook(gb);
}

// Subid $02: Cutscene on maku tree screen after being saved
void nayruSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bce, 0x6bd0); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6bd0, 0x6bd1); A = mem_rd(gb, DE);
  CYC(0x6bd1, 0x6bd2); push_effect(gb, 0x6bd2);
  switch (nayru_jumpTable(gb)) {
    case 0x6bd8: nayruSubid02Substate0_hook(gb); return;
    case 0x6bf8: nayruSubid02Substate1_hook(gb); return;
    case 0x6c27: nayruSubid02Substate2_hook(gb); return;
    default: HANDOFF(HL);
  }
}

// Also called by Ralph in the same cutscene. Falls through into nayruAnimateAndRunScript.
void nayruSubid02Substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bd8, 0x6bdb); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6bdb, 0x6bdd); alu_cp(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x6bdd, 0x6bdf); goto createNotes;
  }
  CYC(0x6bdd, 0x6bdf);
  // Signal received: choose direction randomly (left/right) and go to substate 1
  CALL_C(0x6bdf, getRandomNumber_hook, 0x043e, 0x6be2);
  CYC(0x6be2, 0x6be4); alu_and(gb, 0x02);
  CYC(0x6be4, 0x6be6); alu_or(gb, 0x01);
  CYC(0x6be6, 0x6be8); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6be8, 0x6be9); mem_wr(gb, DE, A);
  CALL_C(0x6be9, nayruSetCounter1Randomly_hook, 0x6c1a, 0x6bec);
  CYC(0x6bec, 0x6bef); interactionIncSubstate_hook(gb);
  return;

createNotes:
  CYC(0x6bef, 0x6bf2); push_effect(gb, 0x6bf2); nayru_createMusicNotes(gb);
  nayruAnimateAndRunScript_hook(gb);
}

void nayruAnimateAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6bf2, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x6bf5);
  CYC(0x6bf5, 0x6bf8); interactionRunScript_hook(gb);
}

void nayruSubid02Substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6bf8, 0x6bfb); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6bfb, 0x6bfd); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x6bfd, 0x6bff); nayruFlipDirectionAtRandomIntervals_hook(gb); return;
  }
  CYC(0x6bfd, 0x6bff);
  CALL_C(0x6bff, interactionIncSubstate_hook, 0x23e5, 0x6c02);
  CYC(0x6c02, 0x6c05); SET_HL(nayruScript02_part3_bank0c);
  CALL_C(0x6c05, interactionSetScript_hook, 0x2544, 0x6c08);
  CYC(0x6c08, 0x6c0a); A = 0x01;
  CYC(0x6c0a, 0x6c0d); interactionSetAnimation_hook(gb);
}

// Also called by Ralph in the same cutscene. Falls through into nayruSetCounter1Randomly.
void nayruFlipDirectionAtRandomIntervals_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c0d, interactionDecCounter1_hook, 0x23cc, 0x6c10);
  if (!(F & FZ)) {
    CYCT(0x6c10, 0x6c11); ret_effect(gb); return;
  }
  CYC(0x6c10, 0x6c11);
  CYC(0x6c11, 0x6c13); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6c13, 0x6c14); A = mem_rd(gb, HL);
  CYC(0x6c14, 0x6c16); alu_xor(gb, 0x02);
  CYC(0x6c16, 0x6c17); mem_wr(gb, HL, A);
  CALL_C(0x6c17, interactionSetAnimation_hook, 0x262e, 0x6c1a);
  nayruSetCounter1Randomly_hook(gb);
}

void nayruSetCounter1Randomly_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c1a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c1d);
  CYC(0x6c1d, 0x6c1f); alu_and(gb, 0x03);
  CYC(0x6c1f, 0x6c20); alu_add(gb, A);
  CYC(0x6c20, 0x6c21); alu_add(gb, A);
  CYC(0x6c21, 0x6c23); alu_add(gb, 0x10);
  CYC(0x6c23, 0x6c25); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6c25, 0x6c26); mem_wr(gb, DE, A);
  CYC(0x6c26, 0x6c27); ret_effect(gb);
}

void nayruSubid02Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c27, nayruAnimateAndRunScript_hook, 0x6bf2, 0x6c2a);
  if (!(F & FC)) {
    CYCT(0x6c2a, 0x6c2b); ret_effect(gb); return;
  }
  CYC(0x6c2a, 0x6c2b);
  CYC(0x6c2b, 0x6c2e); interactionDelete_hook(gb);
}

// Subid $03: Cutscene with Nayru and Ralph when Link exits the black tower
void nayruSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c2e, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x6c31);
  CYC(0x6c31, 0x6c33); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6c33, 0x6c34); A = mem_rd(gb, DE);
  CYC(0x6c34, 0x6c35); push_effect(gb, 0x6c35);
  switch (nayru_jumpTable(gb)) {
    case 0x6c3b: goto substate0;
    case 0x6c47: goto substate1;
    case 0x6c56: goto substate2;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x6c3b, 0x6c3e); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6c3e, 0x6c40); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x6c40, 0x6c41); ret_effect(gb); return;
  }
  CYC(0x6c40, 0x6c41);
  CALL_C(0x6c41, startJump_hook, 0x73db, 0x6c44);
  CYC(0x6c44, 0x6c47); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x6c47, 0x6c49); C = 0x24;
  CALL_C(0x6c49, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6c4c);
  if (!(F & FZ)) {
    CYCT(0x6c4c, 0x6c4d); ret_effect(gb); return;
  }
  CYC(0x6c4c, 0x6c4d);
  CYC(0x6c4d, 0x6c50); SET_HL(nayruScript03_bank0c);
  CALL_C(0x6c50, interactionSetScript_hook, 0x2544, 0x6c53);
  CYC(0x6c53, 0x6c56); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x6c56, 0x6c59); interactionRunScript_hook(gb);
}

// Subid $04: Cutscene at end of game with Ambi and her guards
void nayruSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c59, checkIsLinkedGame_hook, 0x1992, 0x6c5c);
  if (F & FZ) {
    CYCT(0x6c5c, 0x6c5f); nayruAnimateAndRunScript_hook(gb); return;
  }
  CYC(0x6c5c, 0x6c5f);
  CYC(0x6c5f, 0x6c62); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(0x6c62, 0x6c64); alu_cp(gb, 0x0b);
  if (F & FC) {
    CYCT(0x6c64, 0x6c66); nayruAnimateAndRunScript_hook(gb); return;
  }
  CYC(0x6c64, 0x6c66);
  CALL_C(0x6c66, interactionAnimate_hook, 0x261b, 0x6c69);
  // jpab scriptHelp.turnToFaceSomething
  CYC(0x6c69, 0x6c6c); SET_HL(turnToFaceSomething_bank15);
  CYC(0x6c6c, 0x6c6e); E = 0x15;
  CYC(0x6c6e, 0x6c71); interBankCall_hook(gb);
}

// Subid $05: ?
void nayruSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6c71, nayruAnimateAndRunScript_hook, 0x6bf2, 0x6c74);
  CYC(0x6c74, 0x6c77); A = mem_rd(gb, wTmpcfc0);
  CYC(0x6c77, 0x6c79); alu_cp(gb, 0x03);
  if (F & FC) {
    CYCT(0x6c79, 0x6c7a); ret_effect(gb); return;
  }
  CYC(0x6c79, 0x6c7a);
  CYC(0x6c7a, 0x6c7c); alu_cp(gb, 0x05);
  if (!(F & FC)) {
    CYCT(0x6c7c, 0x6c7d); ret_effect(gb); return;
  }
  CYC(0x6c7c, 0x6c7d);
  // jpab scriptHelp.turnToFaceSomething
  CYC(0x6c7d, 0x6c80); SET_HL(turnToFaceSomething_bank15);
  CYC(0x6c80, 0x6c82); E = 0x15;
  CYC(0x6c82, 0x6c85); interBankCall_hook(gb);
}

// For Nayru subid 0 (getting possessed cutscene), this updates var3a, var3b representing
// how long Nayru's palette should be "normal" or "possessed".
// @param[out] zflag Set when Nayru is fully possessed
void nayruUpdatePossessionPaletteDurations_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c85, 0x6c88); A = mem_rd(gb, wFrameCounter);
  CYC(0x6c88, 0x6c8a); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x6c8a, 0x6c8b); ret_effect(gb); return;
  }
  CYC(0x6c8a, 0x6c8b);
  CYC(0x6c8b, 0x6c8d); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6c8d, 0x6c8e); A = mem_rd(gb, DE);
  CYC(0x6c8e, 0x6c8f); push_effect(gb, 0x6c8f);
  switch (nayru_jumpTable(gb)) {
    case 0x6c99: goto var38_0;
    case 0x6ca6: goto var38_1;
    case 0x6cb6: goto var38_2;
    case 0x6cbd: goto var38_3;
    case 0x6cc9: goto var38_4;
    default: HANDOFF(HL);
  }

var38_0:
  // Decrement var3a (unpossessed palette duration), increment var3b (possessed palette
  // duration) until the two are equal, then increment var38.
  CYC(0x6c99, 0x6c9a); H = D;
  CYC(0x6c9a, 0x6c9c); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6c9c, 0x6c9d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6c9d, 0x6c9e); L = alu_inc8(gb, L);
  CYC(0x6c9e, 0x6c9f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6c9f, 0x6ca0); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6ca0, 0x6ca1); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x6ca1, 0x6ca2); ret_effect(gb); return;
  }
  CYC(0x6ca1, 0x6ca2);

incVar38:
  CYC(0x6ca2, 0x6ca4); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6ca4, 0x6ca5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ca5, 0x6ca6); ret_effect(gb);
  return;

var38_1:
  // Decrement both var3a and var3b until they're both 2
  CYC(0x6ca6, 0x6ca7); H = D;
  CYC(0x6ca7, 0x6ca9); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6ca9, 0x6caa); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6caa, 0x6cab); L = alu_inc8(gb, L);
  CYC(0x6cab, 0x6cac); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6cac, 0x6cad); A = mem_rd(gb, HL);
  CYC(0x6cad, 0x6caf); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x6caf, 0x6cb0); ret_effect(gb); return;
  }
  CYC(0x6caf, 0x6cb0);
  CYC(0x6cb0, 0x6cb2); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6cb2, 0x6cb4); mem_wr(gb, HL, 0x10);
  CYC(0x6cb4, 0x6cb6); goto incVar38;

var38_2:
  // Wait 32 frames
  CYC(0x6cb6, 0x6cb7); H = D;
  CYC(0x6cb7, 0x6cb9); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x6cb9, 0x6cba); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x6cba, 0x6cbb); ret_effect(gb); return;
  }
  CYC(0x6cba, 0x6cbb);
  CYC(0x6cbb, 0x6cbd); goto incVar38;

var38_3:
  // Increment both var3a and var3b until they're both 8
  CYC(0x6cbd, 0x6cbe); H = D;
  CYC(0x6cbe, 0x6cc0); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6cc0, 0x6cc1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6cc1, 0x6cc2); L = alu_inc8(gb, L);
  CYC(0x6cc2, 0x6cc3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6cc3, 0x6cc4); A = mem_rd(gb, HL);
  CYC(0x6cc4, 0x6cc6); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x6cc6, 0x6cc7); ret_effect(gb); return;
  }
  CYC(0x6cc6, 0x6cc7);
  CYC(0x6cc7, 0x6cc9); goto incVar38;

var38_4:
  // Decrement var3a, increment var3b until it's 16
  CYC(0x6cc9, 0x6cca); H = D;
  CYC(0x6cca, 0x6ccc); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x6ccc, 0x6ccd); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6ccd, 0x6cce); L = alu_inc8(gb, L);
  CYC(0x6cce, 0x6ccf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ccf, 0x6cd0); A = mem_rd(gb, HL);
  CYC(0x6cd0, 0x6cd2); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x6cd2, 0x6cd3); ret_effect(gb); return;
  }
  CYC(0x6cd2, 0x6cd3);
  CYC(0x6cd3, 0x6cd4); ret_effect(gb);
}

// Subid $07: Cutscene with the vision of Nayru teaching you Tune of Echoes
void nayruSubid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6cd4, checkInteractionSubstate_hook, 0x2403, 0x6cd7);
  if (!(F & FZ)) {
    CYCT(0x6cd7, 0x6cd9); goto substate1;
  }
  CYC(0x6cd7, 0x6cd9);

  // @substate0
  CALL_C(0x6cd9, interactionDecCounter1_hook, 0x23cc, 0x6cdc);
  if (F & FZ) {
    CYCT(0x6cdc, 0x6cde); goto substate0_counterDone;
  }
  CYC(0x6cdc, 0x6cde);
  CYC(0x6cde, 0x6ce0); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x6ce0, 0x6ce1); A = mem_rd(gb, HL);
  CYC(0x6ce1, 0x6ce3); alu_xor(gb, 0x80);
  CYC(0x6ce3, 0x6ce4); mem_wr(gb, HL, A);
  CYC(0x6ce4, 0x6ce5); ret_effect(gb);
  return;
substate0_counterDone:
  CYC(0x6ce5, 0x6ce6); alu_xor(gb, A);
  CYC(0x6ce6, 0x6ce9); mem_wr(gb, wTmpcfc0, A);
  CALL_C(0x6ce9, interactionIncSubstate_hook, 0x23e5, 0x6cec);
  CALL_C(0x6cec, objectSetVisible82_hook, 0x1e69, 0x6cef);
  CYC(0x6cef, 0x6cf1); A = 0x08; // MUS_NAYRU
  CYC(0x6cf1, 0x6cf4); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x6cf4, playSound_b00_hook, 0x0c98, 0x6cf7);
  CYC(0x6cf7, 0x6cfa); SET_HL(nayruScript07_bank0c);
  CYC(0x6cfa, 0x6cfd); interactionSetScript_hook(gb);
  return;

substate1:
  CALL_C(0x6cfd, interactionRunScript_hook, 0x2552, 0x6d00);
  if (F & FC) {
    CYCT(0x6d00, 0x6d02); goto scriptDone;
  }
  CYC(0x6d00, 0x6d02);
  CYC(0x6d02, 0x6d05); A = mem_rd(gb, wTmpcfc0);
  CYC(0x6d05, 0x6d06); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x6d06, 0x6d07); ret_effect(gb); return;
  }
  CYC(0x6d06, 0x6d07);
  CYC(0x6d07, 0x6d09); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6d09, 0x6d0a); A = mem_rd(gb, DE);
  CYC(0x6d0a, 0x6d0c); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(0x6d0c, 0x6d0f); push_effect(gb, 0x6d0f); nayru_createMusicNotes(gb);
  } else {
    CYC(0x6d0c, 0x6d0f);
  }
  CYC(0x6d0f, 0x6d12); interactionAnimate_hook(gb);
  return;

scriptDone:
  CYC(0x6d12, 0x6d15); A = mem_rd(gb, wTextIsActive);
  CYC(0x6d15, 0x6d16); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6d16, 0x6d17); ret_effect(gb); return;
  }
  CYC(0x6d16, 0x6d17);
  // Re-enable objects, menus
  CYC(0x6d17, 0x6d1a); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6d1a, 0x6d1d); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6d1d, 0x6d20); A = mem_rd(gb, wActiveMusic2);
  CYC(0x6d20, 0x6d23); mem_wr(gb, wActiveMusic, A);
  CALL_C(0x6d23, playSound_b00_hook, 0x0c98, 0x6d26);
  CYC(0x6d26, 0x6d28); A = 0x04;
  CALL_C(0x6d28, fadeinFromWhiteWithDelay_hook, 0x3284, 0x6d2b);
  CALL_C(0x6d2b, showStatusBar_hook, 0x1aa4, 0x6d2e);
  CYC(0x6d2e, 0x6d30); A = H8(hActiveObject);
  CYC(0x6d30, 0x6d31); D = A;
  CYC(0x6d31, 0x6d34); interactionDelete_hook(gb);
}

void nayruAsNpc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d34, interactionRunScript_hook, 0x2552, 0x6d37);
  CYC(0x6d37, 0x6d3a); npcFaceLinkAndAnimate_hook(gb);
}

// Subid $09: Cutscene where Ralph's heritage is revealed (unlinked?)
void nayruSubid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d3a, nayruAnimateAndRunScript_hook, 0x6bf2, 0x6d3d);
  if (!(F & FC)) {
    CYCT(0x6d3d, 0x6d3e); ret_effect(gb); return;
  }
  CYC(0x6d3d, 0x6d3e);
  CYC(0x6d3e, 0x6d3f); alu_xor(gb, A);
  CYC(0x6d3f, 0x6d42); mem_wr(gb, wDisabledObjects, A);
  CYC(0x6d42, 0x6d45); mem_wr(gb, wMenuDisabled, A);
  CYC(0x6d45, 0x6d47); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x6d47, setGlobalFlag_hook, 0x31f9, 0x6d4a);
  CYC(0x6d4a, 0x6d4d); interactionDelete_hook(gb);
}

// Subid $10: Cutscene in black tower where Nayru/Ralph meet you to try to escape. Falls
// through into nayruSubid0a.
void nayruSubid10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d4d, 0x6d50); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(0x6d50, 0x6d52); alu_cp(gb, 0x5a);
  if (!(F & FC)) {
    CYCT(0x6d52, 0x6d54); nayruSubid0a_hook(gb); return;
  }
  CYC(0x6d52, 0x6d54);
  CYC(0x6d54, 0x6d55); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6d55, 0x6d57); nayruSubid0a_hook(gb); return;
  }
  CYC(0x6d55, 0x6d57);
  CYC(0x6d57, 0x6d5a); A = W8(w1Link_direction);
  CYC(0x6d5a, 0x6d5b); A = alu_dec8(gb, A);
  CYC(0x6d5b, 0x6d5d); alu_and(gb, 0x03);
  CYC(0x6d5d, 0x6d5e); H = D;
  CYC(0x6d5e, 0x6d60); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6d60, 0x6d61); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x6d61, 0x6d63); nayruSubid0a_hook(gb); return;
  }
  CYC(0x6d61, 0x6d63);
  CYC(0x6d63, 0x6d64); mem_wr(gb, HL, A);
  CALL_C(0x6d64, interactionSetAnimation_hook, 0x262e, 0x6d67);
  nayruSubid0a_hook(gb);
}

// Subid $0a: Cutscene where Ralph's heritage is revealed (linked?)
void nayruSubid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d67, nayruAnimateAndRunScript_hook, 0x6bf2, 0x6d6a);
  if (!(F & FC)) {
    CYCT(0x6d6a, 0x6d6b); ret_effect(gb); return;
  }
  CYC(0x6d6a, 0x6d6b);
  CYC(0x6d6b, 0x6d6e); interactionDelete_hook(gb);
}

// Subid $13: NPC after completing game (singing to animals). Falls through into
// nayruRunScriptWithConditionalAnimation.
void nayruSubid13_hook(GB *gb) {
  CYC(0x6d6e, 0x6d71); push_effect(gb, 0x6d71); nayru_createMusicNotes(gb);
  nayruRunScriptWithConditionalAnimation_hook(gb);
}

// This is called by Ralph as well.
void nayruRunScriptWithConditionalAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6d71, interactionRunScript_hook, 0x2552, 0x6d74);
  CYC(0x6d74, 0x6d76); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x6d76, 0x6d77); A = mem_rd(gb, DE);
  CYC(0x6d77, 0x6d78); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x6d78, interactionAnimate_hook, 0x261b, 0x6d7b);
  } else {
    CYC(0x6d78, 0x6d7b);
  }
  CALL_C(0x6d7b, objectPreventLinkFromPassing_hook, 0x2680, 0x6d7e);
  CYC(0x6d7e, 0x6d81); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}
