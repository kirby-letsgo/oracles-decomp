#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t patch_jump_table(GB *gb) {
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

static void patch_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void patch_subid00_hook(GB *gb);
void patch_subid01_hook(GB *gb);
void patch_subid02_hook(GB *gb);
void patch_subid03_hook(GB *gb);
void patch_subid05_hook(GB *gb);
void patch_subid07_hook(GB *gb);

// INTERAC_PATCH
//
// Variables:
//   var38: 0 if Link has the broken tuni nut; 1 otherwise (upstairs script)
//   var39: Set by another object (subid 3) when all beetles are killed
void interactionCode94_hook(GB *gb) {
  CYC(0x787e, 0x7880); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7880, 0x7881); A = mem_rd(gb, DE);
  CYC(0x7881, 0x7883); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(0x7883, 0x7884); push_effect(gb, 0x7884);
    uint16_t target = patch_jump_table(gb);
    if (target == 0x7894) { patch_subid00_hook(gb); return; }
    if (target == 0x793c) { patch_subid01_hook(gb); return; }
    if (target == 0x7a98) { patch_subid02_hook(gb); return; }
    if (target == 0x7b4c) { patch_subid03_hook(gb); return; }
    if (target == 0x7bf0) { patch_subid05_hook(gb); return; } // patch_subid04 and patch_subid05 alias the same address
    patch_subid07_hook(gb); return; // target == 0x7c40, patch_subid06 and patch_subid07 alias the same address
  }
}

// Patch in the upstairs room
void patch_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7894, 0x7896); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7896, 0x7897); A = mem_rd(gb, DE);
  {
    CYC(0x7897, 0x7898); push_effect(gb, 0x7898);
    uint16_t target = patch_jump_table(gb);
    if (target == 0x7913) goto state1;
    if (target == 0x7928) goto state2;
  }

  // patch_subid00@state0
  CYC(0x789e, 0x78a1); SET_HL(wTuniNutState);
  CYC(0x78a1, 0x78a2); A = mem_rd(gb, HL);
  CYC(0x78a2, 0x78a3); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x78a3, 0x78a5); goto checkTradeItem; } // jr nz
  CYC(0x78a3, 0x78a5);
  CYC(0x78a5, 0x78a6); mem_wr(gb, HL, A);

checkTradeItem:
  CYC(0x78a6, 0x78a9); SET_HL(wTradeItem);
  CYC(0x78a9, 0x78aa); A = mem_rd(gb, HL);
  CYC(0x78aa, 0x78ac); alu_cp(gb, 0x0c); // TRADEITEM_DOING_PATCH_GAME
  if (!(F & FZ)) { CYCT(0x78ac, 0x78ae); goto checkPatchDownstairs; } // jr nz
  CYC(0x78ac, 0x78ae);
  CYC(0x78ae, 0x78b0); mem_wr(gb, HL, 0x0b); // TRADEITEM_BROKEN_SWORD

checkPatchDownstairs:
  CYC(0x78b0, 0x78b3); A = W8(wTmpcfc0_patchMinigame_patchDownstairs);
  CYC(0x78b3, 0x78b4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x78b4, 0x78b7); interactionDelete_hook(gb); return; } // jp z
  CYC(0x78b4, 0x78b7);
  CALL_C(0x78b7, interactionInitGraphics_hook, 0x15fb, 0x78ba);
  CALL_C(0x78ba, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x78bd);
  CALL_C(0x78bd, interactionIncState_hook, 0x23e0, 0x78c0);
  CYC(0x78c0, 0x78c2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x78c2, 0x78c4); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x78c4, objectSetVisiblec2_hook, 0x1e45, 0x78c7);
  CYC(0x78c7, 0x78c9); A = 0x1f; // GLOBALFLAG_PATCH_REPAIRED_EVERYTHING
  CALL_C(0x78c9, checkGlobalFlag_hook, 0x31f3, 0x78cc);
  CYC(0x78cc, 0x78cf); SET_HL(0x7886); // mainScripts.patch_upstairsRepairedEverythingScript
  if (!(F & FZ)) { CYCT(0x78cf, 0x78d1); goto setScript; } // jr nz
  CYC(0x78cf, 0x78d1);

  CYC(0x78d1, 0x78d3); A = 0x13; // <TX_5813
  CYC(0x78d3, 0x78d6); W8(wTmpcfc0_patchMinigame_itemNameText) = A;
  CYC(0x78d6, 0x78d8); A = 0x01;
  CYC(0x78d8, 0x78db); W8(wTmpcfc0_patchMinigame_fixingSword) = A;
  CYC(0x78db, 0x78dd); A = 0x41; // TREASURE_TRADEITEM
  CALL_C(0x78dd, checkTreasureObtained_hook, 0x1748, 0x78e0);
  if (!(F & FC)) { CYCT(0x78e0, 0x78e2); goto notRepairingSword; } // jr nc
  CYC(0x78e0, 0x78e2);
  CYC(0x78e2, 0x78e4); alu_cp(gb, 0x0b); // TRADEITEM_BROKEN_SWORD
  if (!(F & FZ)) { CYCT(0x78e4, 0x78e6); goto notRepairingSword; } // jr nz
  CYC(0x78e4, 0x78e6);
  CYC(0x78e6, 0x78e8); A = 0x05; // TREASURE_SWORD
  CALL_C(0x78e8, checkTreasureObtained_hook, 0x1748, 0x78eb);
  CYC(0x78eb, 0x78ed); alu_and(gb, 0x01);
  CYC(0x78ed, 0x78f0); W8(wTmpcfc0_patchMinigame_swordLevel) = A;
  CYC(0x78f0, 0x78f3); SET_HL(0x7882); // mainScripts.patch_upstairsRepairSwordScript
  CYC(0x78f3, 0x78f5); goto setScript; // jr

notRepairingSword:
  CYC(0x78f5, 0x78f7); A = 0x12; // <TX_5812
  CYC(0x78f7, 0x78fa); W8(wTmpcfc0_patchMinigame_itemNameText) = A;
  CYC(0x78fa, 0x78fb); alu_xor(gb, A);
  CYC(0x78fb, 0x78fe); W8(wTmpcfc0_patchMinigame_fixingSword) = A;
  CYC(0x78fe, 0x7900); A = 0x4c; // TREASURE_TUNI_NUT
  CALL_C(0x7900, checkTreasureObtained_hook, 0x1748, 0x7903);
  CYC(0x7903, 0x7906); SET_HL(0x787e); // mainScripts (bank 0c) patch_upstairsRepairTuniNutScript
  if (!(F & FC)) { CYCT(0x7906, 0x7908); goto setVar38; } // jr nc
  CYC(0x7906, 0x7908);
  CYC(0x7908, 0x7909); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7909, 0x790b); goto setScript; } // jr z
  CYC(0x7909, 0x790b);

setVar38:
  CYC(0x790b, 0x790d); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x790d, 0x790f); A = 0x01;
  CYC(0x790f, 0x7910); mem_wr(gb, DE, A);

setScript:
  CYC(0x7910, 0x7913); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(0x7913, 0x7915); C = 0x20;
  CALL_C(0x7915, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7918);
  if (!(F & FZ)) { RET_TAKEN(0x7918); return; } // ret nz
  CYC(0x7918, 0x7919);
  CALL_C(0x7919, interactionRunScript_hook, 0x2552, 0x791c);
  if (!(F & FC)) { CYCT(0x791c, 0x791f); npcFaceLinkAndAnimate_hook(gb); return; } // jp nc
  CYC(0x791c, 0x791f);
  CALL_C(0x791f, interactionIncState_hook, 0x23e0, 0x7922);
  CYC(0x7922, 0x7925); SET_HL(0x788d); // mainScripts.patch_upstairsMoveToStaircaseScript
  CYC(0x7925, 0x7928); interactionSetScript_hook(gb); return; // jp

state2:
  CALL_C(0x7928, interactionRunScript_hook, 0x2552, 0x792b);
  if (!(F & FC)) { CYCT(0x792b, 0x792e); interactionAnimate_hook(gb); return; } // jp nc
  CYC(0x792b, 0x792e);
  CYC(0x792e, 0x792f); alu_xor(gb, A);
  CYC(0x792f, 0x7932); W8(wDisabledObjects) = A;
  CYC(0x7932, 0x7935); W8(wMenuDisabled) = A;
  CYC(0x7935, 0x7936); A = alu_inc8(gb, A);
  CYC(0x7936, 0x7939); W8(wTmpcfc0_patchMinigame_patchDownstairs) = A;
  CYC(0x7939, 0x793c); interactionDelete_hook(gb); return; // jp
}

// Patch in his minigame room
void patch_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x793c, 0x793d); A = mem_rd(gb, DE);
  {
    CYC(0x793d, 0x793e); push_effect(gb, 0x793e);
    uint16_t target = patch_jump_table(gb);
    if (target == 0x7973) goto state1;
    if (target == 0x79c0) goto state2;
    if (target == 0x7a20) goto state3;
    if (target == 0x7a48) goto state4;
    if (target == 0x7a4e) goto state5;
    if (target == 0x7a8d) goto state6;
  }

  // patch_subid01@state0
  CALL_C(0x794c, interactionInitGraphics_hook, 0x15fb, 0x794f);
  CALL_C(0x794f, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7952);
  CALL_C(0x7952, interactionIncState_hook, 0x23e0, 0x7955);
  CALL_C(0x7955, objectSetVisiblec2_hook, 0x1e45, 0x7958);
  CYC(0x7958, 0x795b); SET_HL(wTmpcfc0_patchMinigame_patchDownstairs);
  CYC(0x795b, 0x795c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x795c, 0x795d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x795d, 0x7960); interactionDelete_hook(gb); return; } // jp z
  CYC(0x795d, 0x7960);
  CYC(0x7960, 0x7961); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+); a = wonMinigame
  CYC(0x7961, 0x7962); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7962, 0x7965); goto alreadyWonMinigame; } // jp nz
  CYC(0x7962, 0x7965);
  CYC(0x7965, 0x7966); alu_xor(gb, A);
  CYC(0x7966, 0x7967); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a: gameStarted=0
  CYC(0x7967, 0x7968); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a: failedGame=0
  CYC(0x7968, 0x7969); mem_wr(gb, HL, A); // screenFadedOut=0
  CYC(0x7969, 0x796a); A = alu_inc8(gb, A);
  CYC(0x796a, 0x796d); W8(wDiggingUpEnemiesForbidden) = A;
  CYC(0x796d, 0x7970); SET_HL(0x7897); // mainScripts.patch_downstairsScript
  CYC(0x7970, 0x7973); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(0x7973, 0x7976); A = W8(wPaletteThread_mode);
  CYC(0x7976, 0x7977); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7977); return; } // ret nz
  CYC(0x7977, 0x7978);
  CALL_C(0x7978, interactionRunScript_hook, 0x2552, 0x797b);
  if (!(F & FC)) { CYCT(0x797b, 0x797e); npcFaceLinkAndAnimate_hook(gb); return; } // jp nc
  CYC(0x797b, 0x797e);
  CYC(0x797e, 0x7980); A = 0x01;
  CYC(0x7980, 0x7983); W8(wTmpcfc0_patchMinigame_gameStarted) = A;
  CYC(0x7983, 0x7985); A = 0xcc; // SND_WHISTLE
  CALL_C(0x7985, playSound_b00_hook, 0x0c98, 0x7988);
  CYC(0x7988, 0x798a); A = 0x2d; // MUS_MINIBOSS
  CYC(0x798a, 0x798d); W8(wActiveMusic) = A;
  CALL_C(0x798d, playSound_b00_hook, 0x0c98, 0x7990);

  // Spawn subid 3, a "manager" for the beetle enemies.
  CYC(0x7990, 0x7993); SET_BC(0x9403); // INTERAC_PATCH, subid 3
  CALL_C(0x7993, objectCreateInteraction_hook, 0x24c5, 0x7996);
  if (!(F & FZ)) { RET_TAKEN(0x7996); return; } // ret nz
  CYC(0x7996, 0x7997);
  CYC(0x7997, 0x7999); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x7999, 0x799b); A = 0x40; // Interaction.start
  CYC(0x799b, 0x799c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x799c, 0x799d); mem_wr(gb, HL, D);

  // Update the tuni nut or trade item state
  CYC(0x799d, 0x79a0); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x79a0, 0x79a1); alu_or(gb, A);
  CYC(0x79a1, 0x79a4); SET_HL(wTuniNutState);
  CYC(0x79a4, 0x79a6); A = 0x01;
  if (F & FZ) { CYCT(0x79a6, 0x79a8); goto writeMinigameStateFlag; } // jr z
  CYC(0x79a6, 0x79a8);
  CYC(0x79a8, 0x79ab); SET_HL(wTradeItem);
  CYC(0x79ab, 0x79ad); A = 0x0c; // TRADEITEM_DOING_PATCH_GAME

writeMinigameStateFlag:
  CYC(0x79ad, 0x79ae); mem_wr(gb, HL, A);
  CYC(0x79ae, 0x79b0); A = 0x06;
  CALL_C(0x79b0, interactionSetAnimation_hook, 0x262e, 0x79b3);
  CALL_C(0x79b3, interactionIncState_hook, 0x23e0, 0x79b6);
  CYC(0x79b6, 0x79b8); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x79b8, 0x79ba); mem_wr(gb, HL, 0x00);
  CYC(0x79ba, 0x79bd); SET_HL(0x789b); // mainScripts.patch_duringMinigameScript
  CALL_C(0x79bd, interactionSetScript_hook, 0x2544, 0x79c0);
  // Falls straight through into state2 -- no ret between them in the ROM.

state2:
  CYC(0x79c0, 0x79c3); A = W8(wTmpcfc0_patchMinigame_failedGame);
  CYC(0x79c3, 0x79c4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x79c4, 0x79c6); goto gameRunning; } // jr z
  CYC(0x79c4, 0x79c6);

  // Failed minigame. This build enables the US bugfixes: check checkLinkCollisionsEnabled
  // and disable Link before switching to state $05.
  CALL_C(0x79c6, checkLinkCollisionsEnabled_hook, 0x1d32, 0x79c9);
  if (!(F & FC)) { RET_TAKEN(0x79c9); return; } // ret nc
  CYC(0x79c9, 0x79ca);
  CYC(0x79ca, 0x79cc); A = 0x01; // DISABLE_LINK
  CYC(0x79cc, 0x79cf); W8(wDisabledObjects) = A;
  CYC(0x79cf, 0x79d1); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x79d1, 0x79d3); A = 0x05;
  CYC(0x79d3, 0x79d4); mem_wr(gb, DE, A);
  CYC(0x79d4, 0x79d5); A = alu_dec8(gb, A);
  CYC(0x79d5, 0x79d8); fadeoutToWhiteWithDelay_hook(gb); return; // jp

gameRunning:
  // Subid 3 sets var39 to nonzero when all beetles are killed; wait for the signal.
  CYC(0x79d8, 0x79da); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x79da, 0x79db); A = mem_rd(gb, DE);
  CYC(0x79db, 0x79dc); alu_or(gb, A);
  if (F & FZ) { CYCT(0x79dc, 0x79de); goto runScriptAndAnimate; } // jr z
  CYC(0x79dc, 0x79de);

  // Link won the game.
  CYC(0x79de, 0x79df); alu_xor(gb, A);
  CYC(0x79df, 0x79e2); W8(wTmpcfc0_patchMinigame_gameStarted) = A;
  CYC(0x79e2, 0x79e5); W8(w1Link_knockbackCounter) = A;
  CALL_C(0x79e5, checkLinkVulnerable_hook, 0x1d28, 0x79e8);
  if (!(F & FC)) { RET_TAKEN(0x79e8); return; } // ret nc
  CYC(0x79e8, 0x79e9);
  CYC(0x79e9, 0x79eb); A = 0x80; // DISABLE_ALL_BUT_INTERACTIONS
  CYC(0x79eb, 0x79ee); W8(wDisabledObjects) = A;
  CYC(0x79ee, 0x79f1); W8(wMenuDisabled) = A;
  CYC(0x79f1, 0x79f4); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x79f4, 0x79f6); alu_add(gb, 0x06);
  CYC(0x79f6, 0x79f7); C = A;
  CYC(0x79f7, 0x79f9); B = 0x94; // INTERAC_PATCH
  CALL_C(0x79f9, objectCreateInteraction_hook, 0x24c5, 0x79fc);
  if (!(F & FZ)) { RET_TAKEN(0x79fc); return; } // ret nz
  CYC(0x79fc, 0x79fd);
  CYC(0x79fd, 0x79ff); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x79ff, 0x7a01); A = 0x40; // Interaction.start
  CYC(0x7a01, 0x7a02); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x7a02, 0x7a03); mem_wr(gb, HL, D);
  CALL_C(0x7a03, interactionIncState_hook, 0x23e0, 0x7a06);
  CYC(0x7a06, 0x7a09); SET_HL(0x78a4); // mainScripts.patch_linkWonMinigameScript
  CALL_C(0x7a09, interactionSetScript_hook, 0x2544, 0x7a0c);
  CYC(0x7a0c, 0x7a0e); A = 0x5b; // SND_SOLVEPUZZLE_2
  CALL_C(0x7a0e, playSound_b00_hook, 0x0c98, 0x7a11);
  CYC(0x7a11, 0x7a14); A = W8(wActiveMusic2);
  CYC(0x7a14, 0x7a17); W8(wActiveMusic) = A;
  CYC(0x7a17, 0x7a1a); playSound_b00_hook(gb); return; // jp

runScriptAndAnimate:
  CALL_C(0x7a1a, interactionRunScript_hook, 0x2552, 0x7a1d);
  CYC(0x7a1d, 0x7a20); interactionAnimateAsNpc_hook(gb); return; // jp

// Just won the game
state3:
  CYC(0x7a20, 0x7a23); A = W8(wPaletteThread_mode);
  CYC(0x7a23, 0x7a24); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7a24, 0x7a26); goto animateNpc; } // jr nz
  CYC(0x7a24, 0x7a26);
  CYC(0x7a26, 0x7a29); A = W8(wTextIsActive);
  CYC(0x7a29, 0x7a2a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7a2a, 0x7a2c); goto state3RunScript; } // jr z
  CYC(0x7a2a, 0x7a2c);

animateNpc:
  CYC(0x7a2c, 0x7a2f); interactionAnimate_hook(gb); return; // jp

state3RunScript:
  CALL_C(0x7a2f, interactionRunScript_hook, 0x2552, 0x7a32);
  if (!(F & FC)) { CYCT(0x7a32, 0x7a34); goto faceLinkAndAnimate; } // jr nc
  CYC(0x7a32, 0x7a34);
  CYC(0x7a34, 0x7a37); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x7a37, 0x7a38); alu_or(gb, A);
  CYC(0x7a38, 0x7a3a); A = 0x1f; // GLOBALFLAG_PATCH_REPAIRED_EVERYTHING
  if (!(F & FZ)) { CALL_C(0x7a3a, setGlobalFlag_hook, 0x31f9, 0x7a3d); } // call nz
  else { CYC(0x7a3a, 0x7a3d); }

alreadyWonMinigame:
  CYC(0x7a3d, 0x7a3f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7a3f, 0x7a41); A = 0x04;
  CYC(0x7a41, 0x7a42); mem_wr(gb, DE, A);
  CYC(0x7a42, 0x7a45); SET_HL(0x7911); // mainScripts.patch_downstairsAfterBeatingMinigameScript
  CYC(0x7a45, 0x7a48); interactionSetScript_hook(gb); return; // jp

// NPC after winning the game
state4:
  CALL_C(0x7a48, interactionRunScript_hook, 0x2552, 0x7a4b);

faceLinkAndAnimate:
  CYC(0x7a4b, 0x7a4e); npcFaceLinkAndAnimate_hook(gb); return; // jp

// Failed the game
state5:
  CYC(0x7a4e, 0x7a51); A = W8(wPaletteThread_mode);
  CYC(0x7a51, 0x7a52); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7a52); return; } // ret nz
  CYC(0x7a52, 0x7a53);
  CYC(0x7a53, 0x7a56); SET_HL(0xd081); // FIRST_ENEMY_INDEX, Enemy.id

nextEnemy:
  CYC(0x7a56, 0x7a57); A = mem_rd(gb, HL);
  CYC(0x7a57, 0x7a59); alu_cp(gb, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  if (!(F & FZ)) { CYCT(0x7a59, 0x7a5b); goto nextEnemyIter; } // jr nz
  CYC(0x7a59, 0x7a5b);
  CYC(0x7a5b, 0x7a5c); push_effect(gb, HL); // push hl
  CYC(0x7a5c, 0x7a5d); D = H;
  CYC(0x7a5d, 0x7a5f); E = 0x80; // Enemy.start
  CALL_C(0x7a5f, objectDelete_de_hook, 0x21e3, 0x7a62);
  CYC(0x7a62, 0x7a63); SET_HL(pop_effect(gb)); // pop hl

nextEnemyIter:
  CYC(0x7a63, 0x7a64); H = alu_inc8(gb, H);
  CYC(0x7a64, 0x7a65); A = H;
  CYC(0x7a65, 0x7a67); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x7a67, 0x7a69); goto nextEnemy; } // jr c
  CYC(0x7a67, 0x7a69);
  CYC(0x7a69, 0x7a6b); A = H8(hActiveObject);
  CYC(0x7a6b, 0x7a6c); D = A;
  CYC(0x7a6c, 0x7a6f); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x7a6f, 0x7a70); alu_or(gb, A);
  CYC(0x7a70, 0x7a73); SET_HL(wTuniNutState);
  if (F & FZ) { CYCT(0x7a73, 0x7a75); goto giveBackItem; } // jr z
  CYC(0x7a73, 0x7a75);
  CYC(0x7a75, 0x7a78); SET_HL(wTradeItem);
  CYC(0x7a78, 0x7a7a); A = 0x0b; // TRADEITEM_BROKEN_SWORD

giveBackItem:
  CYC(0x7a7a, 0x7a7b); mem_wr(gb, HL, A);
  CALL_C(0x7a7b, interactionIncState_hook, 0x23e0, 0x7a7e);
  CYC(0x7a7e, 0x7a81); A = W8(wActiveMusic2);
  CYC(0x7a81, 0x7a84); W8(wActiveMusic) = A;
  CALL_C(0x7a84, playSound_b00_hook, 0x0c98, 0x7a87);
  CYC(0x7a87, 0x7a8a); SET_HL(0x78fa); // mainScripts.patch_linkFailedMinigameScript
  CYC(0x7a8a, 0x7a8d); interactionSetScript_hook(gb); return; // jp

state6:
  CALL_C(0x7a8d, interactionRunScript_hook, 0x2552, 0x7a90);
  if (!(F & FC)) { CYCT(0x7a90, 0x7a92); goto faceLinkAndAnimate; } // jr nc
  CYC(0x7a90, 0x7a92);
  CYC(0x7a92, 0x7a94); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7a94, 0x7a95); alu_xor(gb, A);
  CYC(0x7a95, 0x7a96); mem_wr(gb, DE, A);
  CYC(0x7a96, 0x7a98); goto faceLinkAndAnimate; // jr
}

// The minecart in Patch's minigame
void patch_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a98, 0x7a9b); A = W8(wActiveTriggers);
  CYC(0x7a9b, 0x7a9e); W8(wSwitchState) = A;
  CYC(0x7a9e, 0x7aa0); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7aa0, 0x7aa1); A = mem_rd(gb, DE);
  {
    CYC(0x7aa1, 0x7aa2); push_effect(gb, 0x7aa2);
    uint16_t target = patch_jump_table(gb);
    if (target == 0x7ad1) goto state1;
    if (target == 0x7aec) goto state2;
    if (target == 0x7b44) goto state3;
  }

  // patch_subid02@state0
  // Spawn the object that will toggle the minecart track when the button is down
  CALL_C(0x7aaa, getFreeInteractionSlot_hook, 0x3aef, 0x7aad);
  if (!(F & FZ)) { RET_TAKEN(0x7aad); return; } // ret nz
  CYC(0x7aad, 0x7aae);
  CYC(0x7aae, 0x7ab0); mem_wr(gb, HL, 0x78); // INTERAC_SWITCH_TILE_TOGGLER
  CYC(0x7ab0, 0x7ab1); L = alu_inc8(gb, L);
  CYC(0x7ab1, 0x7ab3); mem_wr(gb, HL, 0x01);
  CYC(0x7ab3, 0x7ab5); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7ab5, 0x7ab7); mem_wr(gb, HL, 0x05);
  CYC(0x7ab7, 0x7ab9); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7ab9, 0x7abb); mem_wr(gb, HL, 0x0b);
  CALL_C(0x7abb, interactionInitGraphics_hook, 0x15fb, 0x7abe);
  CALL_C(0x7abe, interactionIncState_hook, 0x23e0, 0x7ac1);
  CYC(0x7ac1, 0x7ac3); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7ac3, 0x7ac5); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CYC(0x7ac5, 0x7ac7); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7ac7, 0x7ac9); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x7ac9, 0x7acb); A = 0x06;
  CALL_C(0x7acb, objectSetCollideRadius_hook, 0x24a1, 0x7ace);
  CYC(0x7ace, 0x7ad1); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(0x7ad1, 0x7ad4); A = W8(wTmpcfc0_patchMinigame_gameStarted);
  CYC(0x7ad4, 0x7ad5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7ad5); return; } // ret z
  CYC(0x7ad5, 0x7ad6);
  CALL_C(0x7ad6, getFreeInteractionSlot_hook, 0x3aef, 0x7ad9);
  if (!(F & FZ)) { RET_TAKEN(0x7ad9); return; } // ret nz
  CYC(0x7ad9, 0x7ada);
  CYC(0x7ada, 0x7adc); mem_wr(gb, HL, 0x94); // INTERAC_PATCH
  CYC(0x7adc, 0x7add); L = alu_inc8(gb, L);
  CYC(0x7add, 0x7ae0); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x7ae0, 0x7ae2); alu_add(gb, 0x04);
  CYC(0x7ae2, 0x7ae3); mem_wr(gb, HL, A);
  CYC(0x7ae3, 0x7ae5); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x7ae5, 0x7ae7); A = 0x40; // Interaction.start
  CYC(0x7ae7, 0x7ae8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x7ae8, 0x7ae9); mem_wr(gb, HL, D);
  CYC(0x7ae9, 0x7aec); interactionIncState_hook(gb); return; // jp

state2:
  CYC(0x7aec, 0x7aef); SET_HL(wTmpcfc0_patchMinigame_gameStarted);
  CYC(0x7aef, 0x7af0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x7af0, 0x7af1); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7af1, 0x7af3); goto incState; } // jr z
  CYC(0x7af1, 0x7af3);
  CYC(0x7af3, 0x7af4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+); a = failedGame
  CYC(0x7af4, 0x7af5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7af5, 0x7af7); goto gameStillGoing; } // jr z
  CYC(0x7af5, 0x7af7);
  CYC(0x7af7, 0x7af8); A = mem_rd(gb, HL); // a = screenFadedOut
  CYC(0x7af8, 0x7af9); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7af9); return; } // ret z
  CYC(0x7af9, 0x7afa);
  CYC(0x7afa, 0x7afb); H = D;
  CYC(0x7afb, 0x7afd); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7afd, 0x7aff); mem_wr(gb, HL, 0x08);
  CYC(0x7aff, 0x7b01); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7b01, 0x7b03); mem_wr(gb, HL, 0x68);

incState:
  CYC(0x7b03, 0x7b06); interactionIncState_hook(gb); return; // jp

gameStillGoing:
  CALL_C(0x7b06, objectApplySpeed_hook, 0x201d, 0x7b09);
  CALL_C(0x7b09, interactionAnimate_hook, 0x261b, 0x7b0c);
  CYC(0x7b0c, 0x7b0d); H = D;
  CYC(0x7b0d, 0x7b0f); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7b0f, 0x7b10); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x7b10, 0x7b12); alu_and(gb, 0x0f);
  CYC(0x7b12, 0x7b14); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(0x7b14); return; } // ret nz
  CYC(0x7b14, 0x7b15);
  CYC(0x7b15, 0x7b16); L = alu_inc8(gb, L);
  CYC(0x7b16, 0x7b17); A = mem_rd(gb, HL);
  CYC(0x7b17, 0x7b19); alu_and(gb, 0x0f);
  CYC(0x7b19, 0x7b1b); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(0x7b1b); return; } // ret nz
  CALL_C(0x7b1c, objectGetTileAtPosition_hook, 0x1444, 0x7b1f);
  CYC(0x7b1f, 0x7b20); E = A;
  CYC(0x7b20, 0x7b21); A = L;
  CYC(0x7b21, 0x7b23); alu_cp(gb, 0x15);
  CYC(0x7b23, 0x7b25); A = 0x08;
  if (F & FZ) { CYCT(0x7b25, 0x7b27); goto applyAngle; } // jr z
  CYC(0x7b25, 0x7b27);
  CYC(0x7b27, 0x7b2a); SET_HL(0x7b3b); // @trackTable
  CALL_C(0x7b2a, lookupKey_hook, 0x1e06, 0x7b2d);
  if (!(F & FC)) { RET_TAKEN(0x7b2d); return; } // ret nc
  CYC(0x7b2d, 0x7b2e);

applyAngle:
  CYC(0x7b2e, 0x7b30); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7b30, 0x7b31); mem_wr(gb, DE, A);
  CYC(0x7b31, 0x7b33); alu_bit(gb, 3, A);
  CYC(0x7b33, 0x7b35); A = 0x07;
  if (F & FZ) { CYCT(0x7b35, 0x7b37); goto setAnim; } // jr z
  CYC(0x7b35, 0x7b37);
  CYC(0x7b37, 0x7b38); A = alu_inc8(gb, A);

setAnim:
  CYC(0x7b38, 0x7b3b); interactionSetAnimation_hook(gb); return; // jp

  // @trackTable: TILEINDEX_TRACK_TR->DOWN, TILEINDEX_TRACK_BR->LEFT,
  // TILEINDEX_TRACK_BL->UP, TILEINDEX_TRACK_TL->RIGHT, then a $00 terminator.
  // Pure ROM data (7b3b..7b44), consumed only by lookupKey above.

state3:
  CYC(0x7b44, 0x7b47); A = W8(wTmpcfc0_patchMinigame_gameStarted);
  CYC(0x7b47, 0x7b48); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7b48); return; } // ret nz
  CYC(0x7b48, 0x7b49);
  CYC(0x7b49, 0x7b4a); A = alu_inc8(gb, A);
  CYC(0x7b4a, 0x7b4b); mem_wr(gb, DE, A);
  RET(0x7b4b); return; // ret
}

// Subid 3 = Beetle "manager"; spawns them and check when they're killed.
//
// Variables:
//   counter1: Number of beetles to be killed (starts at 4 or 8)
//   var3a: Set to 1 when another beetle should be spawned
//   var3b: Number of extra beetles spawned so far
void patch_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b4c, 0x7b4e); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7b4e, 0x7b4f); A = mem_rd(gb, DE);
  {
    CYC(0x7b4f, 0x7b50); push_effect(gb, 0x7b50);
    uint16_t target = patch_jump_table(gb);
    if (target == 0x7b66) goto state1;
    if (target == 0x7b9e) goto state2;
  }

  // patch_subid03@state0
  CYC(0x7b56, 0x7b59); SET_HL(0x414b); // commonInteractions1.clearFallDownHoleEventBuffer
  CYC(0x7b59, 0x7b5b); E = 0x08; // bank of commonInteractions1
  CALL_C(0x7b5b, interBankCall_hook, 0x008a, 0x7b5e);
  CALL_C(0x7b5e, interactionIncState_hook, 0x23e0, 0x7b61);
  CYC(0x7b61, 0x7b63); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7b63, 0x7b65); mem_wr(gb, HL, 60);
  RET(0x7b65); return; // ret

state1:
  CALL_C(0x7b66, interactionDecCounter1_hook, 0x23cc, 0x7b69);
  if (!(F & FZ)) { RET_TAKEN(0x7b69); return; } // ret nz
  CYC(0x7b69, 0x7b6a);
  CYC(0x7b6a, 0x7b6d); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(0x7b6d, 0x7b6e); alu_add(gb, A); // add a
  CYC(0x7b6e, 0x7b6f); alu_add(gb, A); // add a
  CYC(0x7b6f, 0x7b71); alu_add(gb, 0x04);
  CYC(0x7b71, 0x7b72); mem_wr(gb, HL, A);
  CALL_C(0x7b72, interactionIncState_hook, 0x23e0, 0x7b75);
  CYC(0x7b75, 0x7b77); C = 0x44;
  CYC(0x7b77, 0x7b7a); push_effect(gb, 0x7b7a); goto spawnBeetle;
afterSpawnBeetle1:
  CYC(0x7b7a, 0x7b7c); C = 0x4a;
  CYC(0x7b7c, 0x7b7f); push_effect(gb, 0x7b7f); goto spawnBeetle;
afterSpawnBeetle2:
  CYC(0x7b7f, 0x7b81); C = 0x75;
  CYC(0x7b81, 0x7b84); push_effect(gb, 0x7b84); goto spawnBeetle;
afterSpawnBeetle3:
  CYC(0x7b84, 0x7b86); C = 0x78;
  // Falls straight into spawnBeetle a 4th time; no caller frame pushed for this entry.

  // patch_subid03@spawnBeetle: 4 real callers (3 above, plus one from the
  // "spawn extra beetle" logic below), all at top-level depth sp0_, plus this
  // 4th fallthrough entry with no caller frame at all.
spawnBeetle:
  CALL_C(0x7b86, getFreeInteractionSlot_hook, 0x3aef, 0x7b89);
  if (!(F & FZ)) {
    RET_TAKEN(0x7b89);
    if (gb->pc == 0x7b7a && gb->sp == sp0_) goto afterSpawnBeetle1;
    if (gb->pc == 0x7b7f && gb->sp == sp0_) goto afterSpawnBeetle2;
    if (gb->pc == 0x7b84 && gb->sp == sp0_) goto afterSpawnBeetle3;
    if (gb->pc == 0x7bd2 && gb->sp == sp0_) goto afterSpawnBeetleExtra;
    return;
  } // ret nz
  CYC(0x7b89, 0x7b8a);
  CYC(0x7b8a, 0x7b8c); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x7b8c, 0x7b8e); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(0x7b8e, setShortPosition_paramC_hook, 0x20b9, 0x7b91);
  CALL_C(0x7b91, getFreeEnemySlot_hook, 0x2e27, 0x7b94);
  if (!(F & FZ)) {
    RET_TAKEN(0x7b94);
    if (gb->pc == 0x7b7a && gb->sp == sp0_) goto afterSpawnBeetle1;
    if (gb->pc == 0x7b7f && gb->sp == sp0_) goto afterSpawnBeetle2;
    if (gb->pc == 0x7b84 && gb->sp == sp0_) goto afterSpawnBeetle3;
    if (gb->pc == 0x7bd2 && gb->sp == sp0_) goto afterSpawnBeetleExtra;
    return;
  } // ret nz
  CYC(0x7b94, 0x7b95);
  CYC(0x7b95, 0x7b97); mem_wr(gb, HL, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  CYC(0x7b97, 0x7b99); L = ENEMY_BASE + OBJ_YH;
  CALL_C(0x7b99, setShortPosition_paramC_hook, 0x20b9, 0x7b9c);
  CYC(0x7b9c, 0x7b9d); alu_xor(gb, A);
  RET(0x7b9d);
  if (gb->pc == 0x7b7a && gb->sp == sp0_) goto afterSpawnBeetle1;
  if (gb->pc == 0x7b7f && gb->sp == sp0_) goto afterSpawnBeetle2;
  if (gb->pc == 0x7b84 && gb->sp == sp0_) goto afterSpawnBeetle3;
  if (gb->pc == 0x7bd2 && gb->sp == sp0_) goto afterSpawnBeetleExtra;
  return; // ret

state2:
  CYC(0x7b9e, 0x7ba1); A = W8(wTmpcfc0_patchMinigame_failedGame);
  CYC(0x7ba1, 0x7ba2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7ba2, 0x7ba4); goto deleteInteraction; } // jr nz
  CYC(0x7ba2, 0x7ba4);
  CYC(0x7ba4, 0x7ba7); SET_HL(wTmpcfc0_fallDownHoleEvent_cfd8 + 1);
  CYC(0x7ba7, 0x7ba9); B = 0x04;

nextFallenObjectIter:
  CYC(0x7ba9, 0x7baa); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(0x7baa, 0x7bac); alu_cp(gb, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  if (!(F & FZ)) { CYCT(0x7bac, 0x7bae); goto nextFallenObject; } // jr nz
  CYC(0x7bac, 0x7bae);
  CYC(0x7bae, 0x7baf); push_effect(gb, HL); // push hl
  CALL_C(0x7baf, interactionDecCounter1_hook, 0x23cc, 0x7bb2);
  if (F & FZ) { CYCT(0x7bb2, 0x7bb4); goto allBeetlesKilled; } // jr z
  CYC(0x7bb2, 0x7bb4);
  CYC(0x7bb4, 0x7bb5); A = mem_rd(gb, HL);
  CYC(0x7bb5, 0x7bb7); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(0x7bb7, 0x7bb9); goto popAndContinue; } // jr c
  CYC(0x7bb7, 0x7bb9);
  CYC(0x7bb9, 0x7bbb); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7bbb, 0x7bbc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

popAndContinue:
  CYC(0x7bbc, 0x7bbd); SET_HL(pop_effect(gb)); // pop hl

nextFallenObject:
  CYC(0x7bbd, 0x7bbe); L = alu_inc8(gb, L);
  CYC(0x7bbe, 0x7bbf); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x7bbf, 0x7bc1); goto nextFallenObjectIter; } // jr nz
  CYC(0x7bbf, 0x7bc1);
  CYC(0x7bc1, 0x7bc3); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7bc3, 0x7bc4); A = mem_rd(gb, DE);
  CYC(0x7bc4, 0x7bc5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7bc5, 0x7bc7); goto refillFallDownHoleEventBuffer; } // jr z
  CYC(0x7bc5, 0x7bc7);
  CYC(0x7bc7, 0x7bc9); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x7bc9, 0x7bca); A = mem_rd(gb, DE);
  CYC(0x7bca, 0x7bcd); SET_HL(0x7bec); // @extraBeetlePositions
  CYC(0x7bcd, 0x7bce); patch_addAToHl_from_rst(gb, 0x7bce);
  CYC(0x7bce, 0x7bcf); C = mem_rd(gb, HL);
  CYC(0x7bcf, 0x7bd2); push_effect(gb, 0x7bd2); goto spawnBeetle;
afterSpawnBeetleExtra:
  if (!(F & FZ)) { CYCT(0x7bd2, 0x7bd4); goto refillFallDownHoleEventBuffer; } // jr nz
  CYC(0x7bd2, 0x7bd4);
  CYC(0x7bd4, 0x7bd5); H = D;
  CYC(0x7bd5, 0x7bd7); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7bd7, 0x7bd8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(0x7bd8, 0x7bd9); L = alu_inc8(gb, L);
  CYC(0x7bd9, 0x7bda); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

refillFallDownHoleEventBuffer:
  CYC(0x7bda, 0x7bdd); SET_HL(0x414b); // commonInteractions1.clearFallDownHoleEventBuffer
  CYC(0x7bdd, 0x7bdf); E = 0x08; // bank of commonInteractions1
  CYC(0x7bdf, 0x7be2); interBankCall_hook(gb); return; // jp

allBeetlesKilled:
  CYC(0x7be2, 0x7be3); SET_HL(pop_effect(gb)); // pop hl
  CYC(0x7be3, 0x7be5); A = OBJ_VAR39;
  CALL_C(0x7be5, objectGetRelatedObject1Var_hook, 0x2160, 0x7be8);
  CYC(0x7be8, 0x7be9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

deleteInteraction:
  CYC(0x7be9, 0x7bec); interactionDelete_hook(gb); return; // jp

  // @extraBeetlePositions: pure ROM data (7bec..7bf0), consumed by patch_addAToHl_from_rst above.
}

// Broken tuni nut (4) or sword (5) sprite. patch_subid04 and patch_subid05 both target
// 0a:7bf0 in interactionCode94's jump table -- identical code, so patch_subid04 just
// hands off to patch_subid05.
void patch_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7bf0, 0x7bf2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7bf2, 0x7bf3); A = mem_rd(gb, DE);
  {
    CYC(0x7bf3, 0x7bf4); push_effect(gb, 0x7bf4);
    uint16_t target = patch_jump_table(gb);
    if (target == 0x7c11) goto state1;
    if (target == 0x7c38) goto state2;
  }

  // patch_subid05@state0
  CALL_C(0x7bfa, interactionInitGraphics_hook, 0x15fb, 0x7bfd);
  CALL_C(0x7bfd, interactionIncState_hook, 0x23e0, 0x7c00);
  CYC(0x7c00, 0x7c02); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7c02, 0x7c04); mem_wr(gb, HL, 0x18);
  CYC(0x7c04, 0x7c06); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7c06, 0x7c08); mem_wr(gb, HL, 0x78);
  CYC(0x7c08, 0x7c0b); SET_BC(0x0606);
  CALL_C(0x7c0b, objectSetCollideRadii_hook, 0x24a9, 0x7c0e);
  CYC(0x7c0e, 0x7c11); objectSetVisible83_hook(gb); return; // jp

state1:
  CYC(0x7c11, 0x7c14); A = W8(wPaletteThread_mode);
  CYC(0x7c14, 0x7c15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7c15, 0x7c18); interactionIncState_hook(gb); return; } // jp nz
  CYC(0x7c15, 0x7c18);
  CYC(0x7c18, 0x7c1a); A = 0x00; // Object.start
  CALL_C(0x7c1a, objectGetRelatedObject1Var_hook, 0x2160, 0x7c1d);
  CALL_C(0x7c1d, checkObjectsCollided_hook, 0x1d5a, 0x7c20);
  if (!(F & FC)) { RET_TAKEN(0x7c20); return; } // ret nc
  CYC(0x7c20, 0x7c21);
  CYC(0x7c21, 0x7c23); A = 0x01;
  CYC(0x7c23, 0x7c26); W8(wTmpcfc0_patchMinigame_failedGame) = A;
  CYC(0x7c26, 0x7c28); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(0x7c28, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x7c2b);
  if (!(F & FZ)) { RET_TAKEN(0x7c2b); return; } // ret nz
  CYC(0x7c2b, 0x7c2c);
  CYC(0x7c2c, 0x7c2e); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7c2e, 0x7c2f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x7c2f, 0x7c31); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7c31, 0x7c32); A = mem_rd(gb, HL);
  CYC(0x7c32, 0x7c34); alu_sub(gb, 0x08);
  CYC(0x7c34, 0x7c35); mem_wr(gb, HL, A);
  CYC(0x7c35, 0x7c38); interactionIncState_hook(gb); return; // jp

state2:
  CYC(0x7c38, 0x7c3b); A = W8(wTmpcfc0_patchMinigame_screenFadedOut);
  CYC(0x7c3b, 0x7c3c); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7c3c); return; } // ret z
  CYC(0x7c3c, 0x7c3d);
  CYC(0x7c3d, 0x7c40); interactionDelete_hook(gb); return; // jp
}

void patch_subid04_hook(GB *gb) {
  patch_subid05_hook(gb);
}

// Fixed tuni nut (6) or sword (7) sprite. patch_subid06 and patch_subid07 both target
// 0a:7c40 in interactionCode94's jump table -- identical code, so patch_subid06 just
// hands off to patch_subid07.
void patch_subid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7c40, checkInteractionState_hook, 0x23fe, 0x7c43);
  if (F & FZ) { CYCT(0x7c43, 0x7c45); goto state0; } // jr z
  CYC(0x7c43, 0x7c45);

  // patch_subid07@state1
  CALL_C(0x7c45, interactionDecCounter1_hook, 0x23cc, 0x7c48);
  if (!(F & FZ)) { RET_TAKEN(0x7c48); return; } // ret nz
  CYC(0x7c48, 0x7c49);
  CYC(0x7c49, 0x7c4c); interactionDelete_hook(gb); return; // jp

state0:
  CYC(0x7c4c, 0x7c4f); A = W8(wTmpcfc0_patchMinigame_wonMinigame);
  CYC(0x7c4f, 0x7c50); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7c50); return; } // ret z
  CYC(0x7c50, 0x7c51);
  CALL_C(0x7c51, interactionInitGraphics_hook, 0x15fb, 0x7c54);
  CALL_C(0x7c54, interactionIncState_hook, 0x23e0, 0x7c57);
  CYC(0x7c57, 0x7c59); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c59, 0x7c5b); mem_wr(gb, HL, 60);
  CYC(0x7c5b, 0x7c5d); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7c5d, 0x7c5e); A = mem_rd(gb, HL);
  CYC(0x7c5e, 0x7c60); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x7c60, 0x7c62); goto getPosition; } // jr z
  CYC(0x7c60, 0x7c62);
  CYC(0x7c62, 0x7c65); A = W8(wTmpcfc0_patchMinigame_swordLevel);
  CYC(0x7c65, 0x7c66); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7c66, 0x7c68); goto getPosition; } // jr nz
  CYC(0x7c66, 0x7c68);
  CYC(0x7c68, 0x7c6a); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7c6a, 0x7c6c); A = 0x04;
  CYC(0x7c6c, 0x7c6d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x7c6d, 0x7c6e); mem_wr(gb, HL, A);
  CYC(0x7c6e, 0x7c70); A = 0x0c;
  CALL_C(0x7c70, interactionSetAnimation_hook, 0x262e, 0x7c73);

getPosition:
  CYC(0x7c73, 0x7c75); A = 0x00; // Object.start
  CALL_C(0x7c75, objectGetRelatedObject1Var_hook, 0x2160, 0x7c78);
  CYC(0x7c78, 0x7c7b); SET_BC(0xf2f8);
  CALL_C(0x7c7b, objectTakePositionWithOffset_hook, 0x2277, 0x7c7e);
  CYC(0x7c7e, 0x7c81); objectSetVisible81_hook(gb); return; // jp
}

void patch_subid06_hook(GB *gb) {
  patch_subid07_hook(gb);
}
