#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodead_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void interactionCodead_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:63ba, called from zelda_state0@commonInit.
void zelda_loadScript_hook(GB *gb) {
  CYC(0x63ba, 0x63bc); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x63bc, 0x63bd); A = mem_rd(gb, DE);
  CYC(0x63bd, 0x63c0); SET_HL(0x63c7); // @scriptTable
  CYC(0x63c0, 0x63c1); interactionCodead_addDoubleIndex(gb, 0x63c1);
  CYC(0x63c1, 0x63c2); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x63c2, 0x63c3); H = mem_rd(gb, HL);
  CYC(0x63c3, 0x63c4); L = A;
  CYC(0x63c4, 0x63c7); interactionSetScript_hook(gb); return; // jp
}

// 0b:6381, falls into from zelda_state0@commonInit's own tail call chain (see below).
void zelda_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6381, 0x6383); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6383, 0x6384); A = mem_rd(gb, DE);
  CYC(0x6384, 0x6385); push_effect(gb, 0x6385);
  switch (interactionCodead_jump_table(gb)) {
    case 0x639b: goto animateAndRunScript;
    case 0x63a1: goto runSubid2;
    case 0x63ab: goto runSubid4;
    case 0x63b4: goto faceLinkAndRunScript;
    default: hook_continue(gb, HL, sp0_); return;
  }

animateAndRunScript:
  CALL_C(0x639b, interactionAnimate_hook, 0x261b, 0x639e);
  CYC(0x639e, 0x63a1); interactionRunScript_hook(gb); return; // jp

runSubid2:
  CYC(0x63a1, 0x63a3); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x63a3, 0x63a4); A = mem_rd(gb, DE);
  CYC(0x63a4, 0x63a5); alu_or(gb, A);
  if (F & FZ) { CALL_C_CC(0x63a5, interactionAnimate_hook, 0x261b, 0x63a8); } else { CYC(0x63a5, 0x63a8); } // call z
  CYC(0x63a8, 0x63ab); interactionRunScript_hook(gb); return; // jp

runSubid4:
  CALL_C(0x63ab, interactionRunScript_hook, 0x2552, 0x63ae);
  if (!(F & FC)) { CYCT(0x63ae, 0x63b1); interactionAnimateBasedOnSpeed_hook(gb); return; } // jp nc
  CYC(0x63ae, 0x63b1);
  CYC(0x63b1, 0x63b4); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; // jp

faceLinkAndRunScript:
  CALL_C(0x63b4, interactionRunScript_hook, 0x2552, 0x63b7);
  CYC(0x63b7, 0x63ba); npcFaceLinkAndAnimate_hook(gb); return; // jp
}

// 0b:62b1, called from interactionCodead@state0 (top-level jump table).
void zelda_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x62b1, 0x62b3); A = 0x01;
  CYC(0x62b3, 0x62b4); mem_wr(gb, DE, A); // [state]
  CALL_C(0x62b4, interactionInitGraphics_hook, 0x15fb, 0x62b7);
  CALL_C(0x62b7, objectSetVisiblec2_hook, 0x1e45, 0x62ba);
  CYC(0x62ba, 0x62bc); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x62bc, 0x62bd); A = mem_rd(gb, DE);
  CYC(0x62bd, 0x62be); push_effect(gb, 0x62be);
  switch (interactionCodead_jump_table(gb)) {
    case 0x6360: goto initSubid00;
    case 0x637e: goto commonInit;
    case 0x62f6: goto initSubid03;
    case 0x62d4: goto initSubid04;
    case 0x637b: goto commonInitWithExtraGraphics;
    case 0x6304: goto initSubid07;
    case 0x632c: goto initSubid08;
    case 0x6346: goto initSubid0a;
    default: hook_continue(gb, HL, sp0_); return;
  }

initSubid04:
  CALL_C(0x62d4, checkIsLinkedGame_hook, 0x1992, 0x62d7);
  if (F & FZ) { CYCT(0x62d7, 0x62da); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(0x62d7, 0x62da);
  CYC(0x62da, 0x62dc); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x62dc, checkTreasureObtained_hook, 0x1748, 0x62df);
  if (!(F & FC)) { CYCT(0x62df, 0x62e2); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nc
  CYC(0x62df, 0x62e2);
  CYC(0x62e2, 0x62e4); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x62e4, checkGlobalFlag_hook, 0x31f3, 0x62e7);
  if (!(F & FZ)) { CYCT(0x62e7, 0x62ea); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nz
  CYC(0x62e7, 0x62ea);
  CYC(0x62ea, 0x62eb); H = D;
  CYC(0x62eb, 0x62ed); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x62ed, 0x62ef); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x62ef, 0x62f1); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x62f1, 0x62f3); mem_wr(gb, HL, 0x08);
  CYC(0x62f3, 0x62f6); goto commonInit; // jp

initSubid03:
  CYC(0x62f6, 0x62f9); SET_BC(0x4820);
  CALL_C(0x62f9, interactionSetPosition_hook, 0x2773, 0x62fc);
  CYC(0x62fc, 0x62fe); A = 0x01;
  CALL_C(0x62fe, interactionSetAnimation_hook, 0x262e, 0x6301);
  CYC(0x6301, 0x6304); goto commonInit; // jp

initSubid07:
  CYC(0x6304, 0x6306); A = 0x38; // GLOBALFLAG_GOT_RING_FROM_ZELDA
  CALL_C(0x6306, checkGlobalFlag_hook, 0x31f3, 0x6309);
  if (F & FZ) { CYCT(0x6309, 0x630c); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(0x6309, 0x630c);
  CYC(0x630c, 0x630e); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x630e, checkTreasureObtained_hook, 0x1748, 0x6311);
  if (F & FC) { CYCT(0x6311, 0x6314); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(0x6311, 0x6314);
  CYC(0x6314, 0x6316); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(0x6316, checkGlobalFlag_hook, 0x31f3, 0x6319);
  CYC(0x6319, 0x631b); A = 0x06; // <TX_0606
  if (!(F & FZ)) { CYCT(0x631b, 0x631d); goto actAsGenericNpc; } // jr nz
  CYC(0x631b, 0x631d);
  CYC(0x631d, 0x631f); A = 0x05; // <TX_0605

actAsGenericNpc:
  CYC(0x631f, 0x6321); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x6321, 0x6322); mem_wr(gb, DE, A);
  CYC(0x6322, 0x6323); E = alu_inc8(gb, E);
  CYC(0x6323, 0x6325); A = 0x06; // >TX_0600
  CYC(0x6325, 0x6326); mem_wr(gb, DE, A);
  CYC(0x6326, 0x6329); SET_HL(0x45f0); // mainScripts.genericNpcScript
  CYC(0x6329, 0x632c); interactionSetScript_hook(gb); return; // jp

initSubid08:
  CALL_C(0x632c, checkIsLinkedGame_hook, 0x1992, 0x632f);
  if (F & FZ) { CYCT(0x632f, 0x6332); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(0x632f, 0x6332);
  CYC(0x6332, 0x6334); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x6334, checkGlobalFlag_hook, 0x31f3, 0x6337);
  if (F & FZ) { CYCT(0x6337, 0x633a); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(0x6337, 0x633a);
  CYC(0x633a, 0x633c); A = 0x3a; // GLOBALFLAG_FLAME_OF_DESPAIR_LIT
  CALL_C(0x633c, checkGlobalFlag_hook, 0x31f3, 0x633f);
  if (!(F & FZ)) { CYCT(0x633f, 0x6342); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nz
  CYC(0x633f, 0x6342);
  CYC(0x6342, 0x6344); A = 0x0b; // <TX_060b
  CYCT(0x6344, 0x6346); goto actAsGenericNpc; // jr

initSubid0a:
  CALL_C(0x6346, checkIsLinkedGame_hook, 0x1992, 0x6349);
  if (F & FZ) { CYCT(0x6349, 0x634c); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(0x6349, 0x634c);
  CYC(0x634c, 0x634e); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(0x634e, checkTreasureObtained_hook, 0x1748, 0x6351);
  if (!(F & FC)) { CYCT(0x6351, 0x6354); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nc
  CYC(0x6351, 0x6354);
  CYC(0x6354, 0x6356); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(0x6356, checkGlobalFlag_hook, 0x31f3, 0x6359);
  if (!(F & FZ)) { CYCT(0x6359, 0x635c); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nz
  CYC(0x6359, 0x635c);
  CYC(0x635c, 0x635e); A = 0x0a; // <TX_060a
  CYCT(0x635e, 0x6360); goto actAsGenericNpc; // jr

initSubid00:
  CALL_C(0x6360, getThisRoomFlags_hook, 0x197d, 0x6363);
  CYC(0x6363, 0x6365); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x6365, 0x6367); goto commonInitWithExtraGraphics; } // jr z
  CYC(0x6365, 0x6367);
  CYC(0x6367, 0x6369); A = 0x01;
  CYC(0x6369, 0x636c); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(0x636c, 0x636f); A = mem_rd(gb, wActiveMusic);
  CYC(0x636f, 0x6370); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6370, 0x6372); goto commonInitWithExtraGraphics; } // jr z
  CYC(0x6370, 0x6372);
  CYC(0x6372, 0x6373); alu_xor(gb, A);
  CYC(0x6373, 0x6376); mem_wr(gb, wActiveMusic, A);
  CYC(0x6376, 0x6378); A = 0x38; // MUS_ZELDA_SAVED
  CALL_C(0x6378, playSound_b00_hook, 0x0c98, 0x637b);

commonInitWithExtraGraphics:
  CALL_C(0x637b, interactionLoadExtraGraphics_hook, 0x2781, 0x637e);

commonInit:
  CALL_C(0x637e, zelda_loadScript_hook, 0x63ba, 0x6381);
  zelda_state1_hook(gb); return;
}

// ==================================================================================================
// INTERAC_ZELDA
// ==================================================================================================
void interactionCodead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x62a9, 0x62ab); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x62ab, 0x62ac); A = mem_rd(gb, DE);
  CYC(0x62ac, 0x62ad); push_effect(gb, 0x62ad);
  switch (interactionCodead_jump_table(gb)) {
    case 0x62b1: zelda_state0_hook(gb); return;
    case 0x6381: zelda_state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
