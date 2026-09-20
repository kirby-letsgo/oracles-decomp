#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode94), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode94), (from), (to), true)

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
  BASE(interactionCode94);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = patch_jump_table(gb);
    if (target == SYM(patch_subid00)) { patch_subid00_hook(gb); return; }
    if (target == SYM(patch_subid01)) { patch_subid01_hook(gb); return; }
    if (target == SYM(patch_subid02)) { patch_subid02_hook(gb); return; }
    if (target == SYM(patch_subid03)) { patch_subid03_hook(gb); return; }
    if (target == SYM(patch_subid04)) { patch_subid05_hook(gb); return; } // patch_subid04 and patch_subid05 alias the same address
    patch_subid07_hook(gb); return; // target == 0x7c40, patch_subid06 and patch_subid07 alias the same address
  }
}

// Patch in the upstairs room
void patch_subid00_hook(GB *gb) {
  BASE(patch_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = patch_jump_table(gb);
    if (target == b_+127) goto state1;
    if (target == b_+148) goto state2;
  }

  // patch_subid00@state0
  CYC(b_+10, b_+13); SET_HL(wTuniNutState);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto checkTradeItem; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);

checkTradeItem:
  CYC(b_+18, b_+21); SET_HL(wTradeItem);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c); // TRADEITEM_DOING_PATCH_GAME
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto checkPatchDownstairs; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x0b); // TRADEITEM_BROKEN_SWORD

checkPatchDownstairs:
  CYC(b_+28, b_+31); A = W8(wTmpcfc0_patchMinigame_patchDownstairs);
  CYC(b_+31, b_+32); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+32, b_+35); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+32, b_+35);
  CALL_C(b_+35, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+38);
  CALL_C(b_+38, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+41);
  CALL_C(b_+41, interactionIncState_hook, SYM(interactionIncState), b_+44);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+48, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+51);
  CYC(b_+51, b_+53); A = 0x1f; // GLOBALFLAG_PATCH_REPAIRED_EVERYTHING
  CALL_C(b_+53, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+56);
  CYC(b_+56, b_+59); SET_HL((SYM(interactionCode94) + 8)); // mainScripts.patch_upstairsRepairedEverythingScript
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto setScript; } // jr nz
  CYC(b_+59, b_+61);

  CYC(b_+61, b_+63); A = 0x13; // <TX_5813
  CYC(b_+63, b_+66); W8(wTmpcfc0_patchMinigame_itemNameText) = A;
  CYC(b_+66, b_+68); A = 0x01;
  CYC(b_+68, b_+71); W8(wTmpcfc0_patchMinigame_fixingSword) = A;
  CYC(b_+71, b_+73); A = 0x41; // TREASURE_TRADEITEM
  CALL_C(b_+73, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+76);
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto notRepairingSword; } // jr nc
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); alu_cp(gb, 0x0b); // TRADEITEM_BROKEN_SWORD
  if (!(F & FZ)) { CYCT(b_+80, b_+82); goto notRepairingSword; } // jr nz
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+84); A = 0x05; // TREASURE_SWORD
  CALL_C(b_+84, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+87);
  CYC(b_+87, b_+89); alu_and(gb, 0x01);
  CYC(b_+89, b_+92); W8(wTmpcfc0_patchMinigame_swordLevel) = A;
  CYC(b_+92, b_+95); SET_HL((SYM(interactionCode94) + 4)); // mainScripts.patch_upstairsRepairSwordScript
  CYC(b_+95, b_+97); goto setScript; // jr

notRepairingSword:
  CYC(b_+97, b_+99); A = 0x12; // <TX_5812
  CYC(b_+99, b_+102); W8(wTmpcfc0_patchMinigame_itemNameText) = A;
  CYC(b_+102, b_+103); alu_xor(gb, A);
  CYC(b_+103, b_+106); W8(wTmpcfc0_patchMinigame_fixingSword) = A;
  CYC(b_+106, b_+108); A = 0x4c; // TREASURE_TUNI_NUT
  CALL_C(b_+108, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+111);
  CYC(b_+111, b_+114); SET_HL(SYM(interactionCode94)); // mainScripts (bank 0c) patch_upstairsRepairTuniNutScript
  if (!(F & FC)) { CYCT(b_+114, b_+116); goto setVar38; } // jr nc
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+117); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+117, b_+119); goto setScript; } // jr z
  CYC(b_+117, b_+119);

setVar38:
  CYC(b_+119, b_+121); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+121, b_+123); A = 0x01;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);

setScript:
  CYC(b_+124, b_+127); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(b_+127, b_+129); C = 0x20;
  CALL_C(b_+129, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+132);
  if (!(F & FZ)) { RET_TAKEN(b_+132); return; } // ret nz
  CYC(b_+132, b_+133);
  CALL_C(b_+133, interactionRunScript_hook, SYM(interactionRunScript), b_+136);
  if (!(F & FC)) { CYCT(b_+136, b_+139); npcFaceLinkAndAnimate_hook(gb); return; } // jp nc
  CYC(b_+136, b_+139);
  CALL_C(b_+139, interactionIncState_hook, SYM(interactionIncState), b_+142);
  CYC(b_+142, b_+145); SET_HL((SYM(interactionCode94) + 15)); // mainScripts.patch_upstairsMoveToStaircaseScript
  CYC(b_+145, b_+148); interactionSetScript_hook(gb); return; // jp

state2:
  CALL_C(b_+148, interactionRunScript_hook, SYM(interactionRunScript), b_+151);
  if (!(F & FC)) { CYCT(b_+151, b_+154); interactionAnimate_hook(gb); return; } // jp nc
  CYC(b_+151, b_+154);
  CYC(b_+154, b_+155); alu_xor(gb, A);
  CYC(b_+155, b_+158); W8(wDisabledObjects) = A;
  CYC(b_+158, b_+161); W8(wMenuDisabled) = A;
  CYC(b_+161, b_+162); A = alu_inc8(gb, A);
  CYC(b_+162, b_+165); W8(wTmpcfc0_patchMinigame_patchDownstairs) = A;
  CYC(b_+165, SYM(patch_subid01)); interactionDelete_hook(gb); return; // jp
}

// Patch in his minigame room
void patch_subid01_hook(GB *gb) {
  BASE(patch_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = patch_jump_table(gb);
    if (target == b_+55) goto state1;
    if (target == b_+132) goto state2;
    if (target == b_+228) goto state3;
    if (target == b_+268) goto state4;
    if (target == b_+274) goto state5;
    if (target == b_+337) goto state6;
  }

  // patch_subid01@state0
  CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
  CALL_C(b_+19, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+22);
  CALL_C(b_+22, interactionIncState_hook, SYM(interactionIncState), b_+25);
  CALL_C(b_+25, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+28);
  CYC(b_+28, b_+31); SET_HL(wTmpcfc0_patchMinigame_patchDownstairs);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+33, b_+36); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+); a = wonMinigame
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+41); goto alreadyWonMinigame; } // jp nz
  CYC(b_+38, b_+41);
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a: gameStarted=0
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a: failedGame=0
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); // screenFadedOut=0
  CYC(b_+45, b_+46); A = alu_inc8(gb, A);
  CYC(b_+46, b_+49); W8(wDiggingUpEnemiesForbidden) = A;
  CYC(b_+49, b_+52); SET_HL((SYM(patch_subid00) + 3)); // mainScripts.patch_downstairsScript
  CYC(b_+52, b_+55); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(b_+55, b_+58); A = W8(wPaletteThread_mode);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; } // ret nz
  CYC(b_+59, b_+60);
  CALL_C(b_+60, interactionRunScript_hook, SYM(interactionRunScript), b_+63);
  if (!(F & FC)) { CYCT(b_+63, b_+66); npcFaceLinkAndAnimate_hook(gb); return; } // jp nc
  CYC(b_+63, b_+66);
  CYC(b_+66, b_+68); A = 0x01;
  CYC(b_+68, b_+71); W8(wTmpcfc0_patchMinigame_gameStarted) = A;
  CYC(b_+71, b_+73); A = 0xcc; // SND_WHISTLE
  CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
  CYC(b_+76, b_+78); A = 0x2d; // MUS_MINIBOSS
  CYC(b_+78, b_+81); W8(wActiveMusic) = A;
  CALL_C(b_+81, playSound_b00_hook, SYM(playSound_b00), b_+84);

  // Spawn subid 3, a "manager" for the beetle enemies.
  CYC(b_+84, b_+87); SET_BC(0x9403); // INTERAC_PATCH, subid 3
  CALL_C(b_+87, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+90);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; } // ret nz
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+93); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+93, b_+95); A = 0x40; // Interaction.start
  CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+96, b_+97); mem_wr(gb, HL, D);

  // Update the tuni nut or trade item state
  CYC(b_+97, b_+100); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+100, b_+101); alu_or(gb, A);
  CYC(b_+101, b_+104); SET_HL(wTuniNutState);
  CYC(b_+104, b_+106); A = 0x01;
  if (F & FZ) { CYCT(b_+106, b_+108); goto writeMinigameStateFlag; } // jr z
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+111); SET_HL(wTradeItem);
  CYC(b_+111, b_+113); A = 0x0c; // TRADEITEM_DOING_PATCH_GAME

writeMinigameStateFlag:
  CYC(b_+113, b_+114); mem_wr(gb, HL, A);
  CYC(b_+114, b_+116); A = 0x06;
  CALL_C(b_+116, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+119);
  CALL_C(b_+119, interactionIncState_hook, SYM(interactionIncState), b_+122);
  CYC(b_+122, b_+124); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x00);
  CYC(b_+126, b_+129); SET_HL((SYM(patch_subid00) + 7)); // mainScripts.patch_duringMinigameScript
  CALL_C(b_+129, interactionSetScript_hook, SYM(interactionSetScript), b_+132);
  // Falls straight through into state2 -- no ret between them in the ROM.

state2:
  CYC(b_+132, b_+135); A = W8(wTmpcfc0_patchMinigame_failedGame);
  CYC(b_+135, b_+136); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+136, b_+138); goto gameRunning; } // jr z
  CYC(b_+136, b_+138);

  // Failed minigame. This build enables the US bugfixes: check checkLinkCollisionsEnabled
  // and disable Link before switching to state $05.
  CALL_C(b_+138, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+141);
  if (!(F & FC)) { RET_TAKEN(b_+141); return; } // ret nc
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+144); A = 0x01; // DISABLE_LINK
  CYC(b_+144, b_+147); W8(wDisabledObjects) = A;
  CYC(b_+147, b_+149); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+149, b_+151); A = 0x05;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+153); A = alu_dec8(gb, A);
  CYC(b_+153, b_+156); fadeoutToWhiteWithDelay_hook(gb); return; // jp

gameRunning:
  // Subid 3 sets var39 to nonzero when all beetles are killed; wait for the signal.
  CYC(b_+156, b_+158); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+160); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+160, b_+162); goto runScriptAndAnimate; } // jr z
  CYC(b_+160, b_+162);

  // Link won the game.
  CYC(b_+162, b_+163); alu_xor(gb, A);
  CYC(b_+163, b_+166); W8(wTmpcfc0_patchMinigame_gameStarted) = A;
  CYC(b_+166, b_+169); W8(w1Link_knockbackCounter) = A;
  CALL_C(b_+169, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+172);
  if (!(F & FC)) { RET_TAKEN(b_+172); return; } // ret nc
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+175); A = 0x80; // DISABLE_ALL_BUT_INTERACTIONS
  CYC(b_+175, b_+178); W8(wDisabledObjects) = A;
  CYC(b_+178, b_+181); W8(wMenuDisabled) = A;
  CYC(b_+181, b_+184); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+184, b_+186); alu_add(gb, 0x06);
  CYC(b_+186, b_+187); C = A;
  CYC(b_+187, b_+189); B = 0x94; // INTERAC_PATCH
  CALL_C(b_+189, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+192);
  if (!(F & FZ)) { RET_TAKEN(b_+192); return; } // ret nz
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+195); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+195, b_+197); A = 0x40; // Interaction.start
  CYC(b_+197, b_+198); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+198, b_+199); mem_wr(gb, HL, D);
  CALL_C(b_+199, interactionIncState_hook, SYM(interactionIncState), b_+202);
  CYC(b_+202, b_+205); SET_HL((SYM(patch_subid00__state0) + 6)); // mainScripts.patch_linkWonMinigameScript
  CALL_C(b_+205, interactionSetScript_hook, SYM(interactionSetScript), b_+208);
  CYC(b_+208, b_+210); A = 0x5b; // SND_SOLVEPUZZLE_2
  CALL_C(b_+210, playSound_b00_hook, SYM(playSound_b00), b_+213);
  CYC(b_+213, b_+216); A = W8(wActiveMusic2);
  CYC(b_+216, b_+219); W8(wActiveMusic) = A;
  CYC(b_+219, b_+222); playSound_b00_hook(gb); return; // jp

runScriptAndAnimate:
  CALL_C(b_+222, interactionRunScript_hook, SYM(interactionRunScript), b_+225);
  CYC(b_+225, b_+228); interactionAnimateAsNpc_hook(gb); return; // jp

// Just won the game
state3:
  CYC(b_+228, b_+231); A = W8(wPaletteThread_mode);
  CYC(b_+231, b_+232); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+232, b_+234); goto animateNpc; } // jr nz
  CYC(b_+232, b_+234);
  CYC(b_+234, b_+237); A = W8(wTextIsActive);
  CYC(b_+237, b_+238); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+238, b_+240); goto state3RunScript; } // jr z
  CYC(b_+238, b_+240);

animateNpc:
  CYC(b_+240, b_+243); interactionAnimate_hook(gb); return; // jp

state3RunScript:
  CALL_C(b_+243, interactionRunScript_hook, SYM(interactionRunScript), b_+246);
  if (!(F & FC)) { CYCT(b_+246, b_+248); goto faceLinkAndAnimate; } // jr nc
  CYC(b_+246, b_+248);
  CYC(b_+248, b_+251); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+251, b_+252); alu_or(gb, A);
  CYC(b_+252, b_+254); A = 0x1f; // GLOBALFLAG_PATCH_REPAIRED_EVERYTHING
  if (!(F & FZ)) { CALL_C(b_+254, setGlobalFlag_hook, SYM(setGlobalFlag), b_+257); } // call nz
  else { CYC(b_+254, b_+257); }

alreadyWonMinigame:
  CYC(b_+257, b_+259); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+259, b_+261); A = 0x04;
  CYC(b_+261, b_+262); mem_wr(gb, DE, A);
  CYC(b_+262, b_+265); SET_HL((SYM(patch_subid00__setScript) + 1)); // mainScripts.patch_downstairsAfterBeatingMinigameScript
  CYC(b_+265, b_+268); interactionSetScript_hook(gb); return; // jp

// NPC after winning the game
state4:
  CALL_C(b_+268, interactionRunScript_hook, SYM(interactionRunScript), b_+271);

faceLinkAndAnimate:
  CYC(b_+271, b_+274); npcFaceLinkAndAnimate_hook(gb); return; // jp

// Failed the game
state5:
  CYC(b_+274, b_+277); A = W8(wPaletteThread_mode);
  CYC(b_+277, b_+278); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+278); return; } // ret nz
  CYC(b_+278, b_+279);
  CYC(b_+279, b_+282); SET_HL((w1ReservedInteraction0_var3f + 2)); // FIRST_ENEMY_INDEX, Enemy.id

nextEnemy:
  CYC(b_+282, b_+283); A = mem_rd(gb, HL);
  CYC(b_+283, b_+285); alu_cp(gb, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  if (!(F & FZ)) { CYCT(b_+285, b_+287); goto nextEnemyIter; } // jr nz
  CYC(b_+285, b_+287);
  CYC(b_+287, b_+288); push_effect(gb, HL); // push hl
  CYC(b_+288, b_+289); D = H;
  CYC(b_+289, b_+291); E = 0x80; // Enemy.start
  CALL_C(b_+291, objectDelete_de_hook, SYM(objectDelete_de), b_+294);
  CYC(b_+294, b_+295); SET_HL(pop_effect(gb)); // pop hl

nextEnemyIter:
  CYC(b_+295, b_+296); H = alu_inc8(gb, H);
  CYC(b_+296, b_+297); A = H;
  CYC(b_+297, b_+299); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+299, b_+301); goto nextEnemy; } // jr c
  CYC(b_+299, b_+301);
  CYC(b_+301, b_+303); A = H8(hActiveObject);
  CYC(b_+303, b_+304); D = A;
  CYC(b_+304, b_+307); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+307, b_+308); alu_or(gb, A);
  CYC(b_+308, b_+311); SET_HL(wTuniNutState);
  if (F & FZ) { CYCT(b_+311, b_+313); goto giveBackItem; } // jr z
  CYC(b_+311, b_+313);
  CYC(b_+313, b_+316); SET_HL(wTradeItem);
  CYC(b_+316, b_+318); A = 0x0b; // TRADEITEM_BROKEN_SWORD

giveBackItem:
  CYC(b_+318, b_+319); mem_wr(gb, HL, A);
  CALL_C(b_+319, interactionIncState_hook, SYM(interactionIncState), b_+322);
  CYC(b_+322, b_+325); A = W8(wActiveMusic2);
  CYC(b_+325, b_+328); W8(wActiveMusic) = A;
  CALL_C(b_+328, playSound_b00_hook, SYM(playSound_b00), b_+331);
  CYC(b_+331, b_+334); SET_HL((SYM(patch_subid00__notRepairingSword) + 5)); // mainScripts.patch_linkFailedMinigameScript
  CYC(b_+334, b_+337); interactionSetScript_hook(gb); return; // jp

state6:
  CALL_C(b_+337, interactionRunScript_hook, SYM(interactionRunScript), b_+340);
  if (!(F & FC)) { CYCT(b_+340, b_+342); goto faceLinkAndAnimate; } // jr nc
  CYC(b_+340, b_+342);
  CYC(b_+342, b_+344); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+344, b_+345); alu_xor(gb, A);
  CYC(b_+345, b_+346); mem_wr(gb, DE, A);
  CYC(b_+346, SYM(patch_subid02)); goto faceLinkAndAnimate; // jr
}

// The minecart in Patch's minigame
void patch_subid02_hook(GB *gb) {
  BASE(patch_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wActiveTriggers);
  CYC(b_+3, b_+6); W8(wSwitchState) = A;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  {
    CYC(b_+9, b_+10); push_effect(gb, b_+10);
    uint16_t target = patch_jump_table(gb);
    if (target == b_+57) goto state1;
    if (target == b_+84) goto state2;
    if (target == b_+172) goto state3;
  }

  // patch_subid02@state0
  // Spawn the object that will toggle the minecart track when the button is down
  CALL_C(b_+18, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+21);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x78); // INTERAC_SWITCH_TILE_TOGGLER
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x01);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x05);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x0b);
  CALL_C(b_+35, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+38);
  CALL_C(b_+38, interactionIncState_hook, SYM(interactionIncState), b_+41);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x08); // ANGLE_RIGHT
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+49, b_+51); A = 0x06;
  CALL_C(b_+51, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+54);
  CYC(b_+54, b_+57); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(b_+57, b_+60); A = W8(wTmpcfc0_patchMinigame_gameStarted);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+61); return; } // ret z
  CYC(b_+61, b_+62);
  CALL_C(b_+62, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+65);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; } // ret nz
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x94); // INTERAC_PATCH
  CYC(b_+68, b_+69); L = alu_inc8(gb, L);
  CYC(b_+69, b_+72); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+72, b_+74); alu_add(gb, 0x04);
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+77, b_+79); A = 0x40; // Interaction.start
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+80, b_+81); mem_wr(gb, HL, D);
  CYC(b_+81, b_+84); interactionIncState_hook(gb); return; // jp

state2:
  CYC(b_+84, b_+87); SET_HL(wTmpcfc0_patchMinigame_gameStarted);
  CYC(b_+87, b_+88); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+88, b_+89); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+89, b_+91); goto incState; } // jr z
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+92); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+); a = failedGame
  CYC(b_+92, b_+93); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+93, b_+95); goto gameStillGoing; } // jr z
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL); // a = screenFadedOut
  CYC(b_+96, b_+97); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+97); return; } // ret z
  CYC(b_+97, b_+98);
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+101); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x08);
  CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x68);

incState:
  CYC(b_+107, b_+110); interactionIncState_hook(gb); return; // jp

gameStillGoing:
  CALL_C(b_+110, objectApplySpeed_hook, SYM(objectApplySpeed), b_+113);
  CALL_C(b_+113, interactionAnimate_hook, SYM(interactionAnimate), b_+116);
  CYC(b_+116, b_+117); H = D;
  CYC(b_+117, b_+119); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+119, b_+120); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+120, b_+122); alu_and(gb, 0x0f);
  CYC(b_+122, b_+124); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+124); return; } // ret nz
  CYC(b_+124, b_+125);
  CYC(b_+125, b_+126); L = alu_inc8(gb, L);
  CYC(b_+126, b_+127); A = mem_rd(gb, HL);
  CYC(b_+127, b_+129); alu_and(gb, 0x0f);
  CYC(b_+129, b_+131); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+131); return; } // ret nz
  CALL_C(b_+132, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+135);
  CYC(b_+135, b_+136); E = A;
  CYC(b_+136, b_+137); A = L;
  CYC(b_+137, b_+139); alu_cp(gb, 0x15);
  CYC(b_+139, b_+141); A = 0x08;
  if (F & FZ) { CYCT(b_+141, b_+143); goto applyAngle; } // jr z
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+146); SET_HL(b_+163); // @trackTable
  CALL_C(b_+146, lookupKey_hook, SYM(lookupKey), b_+149);
  if (!(F & FC)) { RET_TAKEN(b_+149); return; } // ret nc
  CYC(b_+149, b_+150);

applyAngle:
  CYC(b_+150, b_+152); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  CYC(b_+153, b_+155); alu_bit(gb, 3, A);
  CYC(b_+155, b_+157); A = 0x07;
  if (F & FZ) { CYCT(b_+157, b_+159); goto setAnim; } // jr z
  CYC(b_+157, b_+159);
  CYC(b_+159, b_+160); A = alu_inc8(gb, A);

setAnim:
  CYC(b_+160, b_+163); interactionSetAnimation_hook(gb); return; // jp

  // @trackTable: TILEINDEX_TRACK_TR->DOWN, TILEINDEX_TRACK_BR->LEFT,
  // TILEINDEX_TRACK_BL->UP, TILEINDEX_TRACK_TL->RIGHT, then a $00 terminator.
  // Pure ROM data (7b3b..7b44), consumed only by lookupKey above.

state3:
  CYC(b_+172, b_+175); A = W8(wTmpcfc0_patchMinigame_gameStarted);
  CYC(b_+175, b_+176); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+176); return; } // ret nz
  CYC(b_+176, b_+177);
  CYC(b_+177, b_+178); A = alu_inc8(gb, A);
  CYC(b_+178, b_+179); mem_wr(gb, DE, A);
  RET(b_+179); return; // ret
}

// Subid 3 = Beetle "manager"; spawns them and check when they're killed.
//
// Variables:
//   counter1: Number of beetles to be killed (starts at 4 or 8)
//   var3a: Set to 1 when another beetle should be spawned
//   var3b: Number of extra beetles spawned so far
void patch_subid03_hook(GB *gb) {
  BASE(patch_subid03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = patch_jump_table(gb);
    if (target == b_+26) goto state1;
    if (target == b_+82) goto state2;
  }

  // patch_subid03@state0
  CYC(b_+10, b_+13); SET_HL((SYM(interactionCode7a__state0) + 41)); // commonInteractions1.clearFallDownHoleEventBuffer
  CYC(b_+13, b_+15); E = 0x08; // bank of commonInteractions1
  CALL_C(b_+15, interBankCall_hook, 0x008a, b_+18);
  CALL_C(b_+18, interactionIncState_hook, SYM(interactionIncState), b_+21);
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 60);
  RET(b_+25); return; // ret

state1:
  CALL_C(b_+26, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+29);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } // ret nz
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+33); A = W8(wTmpcfc0_patchMinigame_fixingSword);
  CYC(b_+33, b_+34); alu_add(gb, A); // add a
  CYC(b_+34, b_+35); alu_add(gb, A); // add a
  CYC(b_+35, b_+37); alu_add(gb, 0x04);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  CALL_C(b_+38, interactionIncState_hook, SYM(interactionIncState), b_+41);
  CYC(b_+41, b_+43); C = 0x44;
  CYC(b_+43, b_+46); push_effect(gb, b_+46); goto spawnBeetle;
afterSpawnBeetle1:
  CYC(b_+46, b_+48); C = 0x4a;
  CYC(b_+48, b_+51); push_effect(gb, b_+51); goto spawnBeetle;
afterSpawnBeetle2:
  CYC(b_+51, b_+53); C = 0x75;
  CYC(b_+53, b_+56); push_effect(gb, b_+56); goto spawnBeetle;
afterSpawnBeetle3:
  CYC(b_+56, b_+58); C = 0x78;
  // Falls straight into spawnBeetle a 4th time; no caller frame pushed for this entry.

  // patch_subid03@spawnBeetle: 4 real callers (3 above, plus one from the
  // "spawn extra beetle" logic below), all at top-level depth sp0_, plus this
  // 4th fallthrough entry with no caller frame at all.
spawnBeetle:
  CALL_C(b_+58, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+61);
  if (!(F & FZ)) {
    RET_TAKEN(b_+61);
    if (gb->pc == b_+46 && gb->sp == sp0_) goto afterSpawnBeetle1;
    if (gb->pc == b_+51 && gb->sp == sp0_) goto afterSpawnBeetle2;
    if (gb->pc == b_+56 && gb->sp == sp0_) goto afterSpawnBeetle3;
    if (gb->pc == b_+134 && gb->sp == sp0_) goto afterSpawnBeetleExtra;
    return;
  } // ret nz
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+64, b_+66); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+66, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+69);
  CALL_C(b_+69, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+72);
  if (!(F & FZ)) {
    RET_TAKEN(b_+72);
    if (gb->pc == b_+46 && gb->sp == sp0_) goto afterSpawnBeetle1;
    if (gb->pc == b_+51 && gb->sp == sp0_) goto afterSpawnBeetle2;
    if (gb->pc == b_+56 && gb->sp == sp0_) goto afterSpawnBeetle3;
    if (gb->pc == b_+134 && gb->sp == sp0_) goto afterSpawnBeetleExtra;
    return;
  } // ret nz
  CYC(b_+72, b_+73);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  CYC(b_+75, b_+77); L = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+77, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+80);
  CYC(b_+80, b_+81); alu_xor(gb, A);
  RET(b_+81);
  if (gb->pc == b_+46 && gb->sp == sp0_) goto afterSpawnBeetle1;
  if (gb->pc == b_+51 && gb->sp == sp0_) goto afterSpawnBeetle2;
  if (gb->pc == b_+56 && gb->sp == sp0_) goto afterSpawnBeetle3;
  if (gb->pc == b_+134 && gb->sp == sp0_) goto afterSpawnBeetleExtra;
  return; // ret

state2:
  CYC(b_+82, b_+85); A = W8(wTmpcfc0_patchMinigame_failedGame);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); goto deleteInteraction; } // jr nz
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+91); SET_HL(wTmpcfc0_fallDownHoleEvent_cfd8 + 1);
  CYC(b_+91, b_+93); B = 0x04;

nextFallenObjectIter:
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL + 1); // ld a,(hl+)
  CYC(b_+94, b_+96); alu_cp(gb, 0x5f); // ENEMY_HARMLESS_HARDHAT_BEETLE
  if (!(F & FZ)) { CYCT(b_+96, b_+98); goto nextFallenObject; } // jr nz
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+99); push_effect(gb, HL); // push hl
  CALL_C(b_+99, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+102);
  if (F & FZ) { CYCT(b_+102, b_+104); goto allBeetlesKilled; } // jr z
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+107); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+107, b_+109); goto popAndContinue; } // jr c
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+111); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+111, b_+112); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

popAndContinue:
  CYC(b_+112, b_+113); SET_HL(pop_effect(gb)); // pop hl

nextFallenObject:
  CYC(b_+113, b_+114); L = alu_inc8(gb, L);
  CYC(b_+114, b_+115); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+115, b_+117); goto nextFallenObjectIter; } // jr nz
  CYC(b_+115, b_+117);
  CYC(b_+117, b_+119); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+119, b_+120); A = mem_rd(gb, DE);
  CYC(b_+120, b_+121); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+121, b_+123); goto refillFallDownHoleEventBuffer; } // jr z
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+125); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+129); SET_HL(b_+160); // @extraBeetlePositions
  CYC(b_+129, b_+130); patch_addAToHl_from_rst(gb, b_+130);
  CYC(b_+130, b_+131); C = mem_rd(gb, HL);
  CYC(b_+131, b_+134); push_effect(gb, b_+134); goto spawnBeetle;
afterSpawnBeetleExtra:
  if (!(F & FZ)) { CYCT(b_+134, b_+136); goto refillFallDownHoleEventBuffer; } // jr nz
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+137); H = D;
  CYC(b_+137, b_+139); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+139, b_+140); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+140, b_+141); L = alu_inc8(gb, L);
  CYC(b_+141, b_+142); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

refillFallDownHoleEventBuffer:
  CYC(b_+142, b_+145); SET_HL((SYM(interactionCode7a__state0) + 41)); // commonInteractions1.clearFallDownHoleEventBuffer
  CYC(b_+145, b_+147); E = 0x08; // bank of commonInteractions1
  CYC(b_+147, b_+150); interBankCall_hook(gb); return; // jp

allBeetlesKilled:
  CYC(b_+150, b_+151); SET_HL(pop_effect(gb)); // pop hl
  CYC(b_+151, b_+153); A = OBJ_VAR39;
  CALL_C(b_+153, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+156);
  CYC(b_+156, b_+157); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)

deleteInteraction:
  CYC(b_+157, b_+160); interactionDelete_hook(gb); return; // jp

  // @extraBeetlePositions: pure ROM data (7bec..7bf0), consumed by patch_addAToHl_from_rst above.
}

// Broken tuni nut (4) or sword (5) sprite. patch_subid04 and patch_subid05 both target
// 0a:7bf0 in interactionCode94's jump table -- identical code, so patch_subid04 just
// hands off to patch_subid05.
void patch_subid05_hook(GB *gb) {
  BASE(patch_subid04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = patch_jump_table(gb);
    if (target == b_+33) goto state1;
    if (target == b_+72) goto state2;
  }

  // patch_subid05@state0
  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13);
  CALL_C(b_+13, interactionIncState_hook, SYM(interactionIncState), b_+16);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x18);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x78);
  CYC(b_+24, b_+27); SET_BC((SYM(loadUncompressedGfxHeader) + 44));
  CALL_C(b_+27, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+30);
  CYC(b_+30, b_+33); objectSetVisible83_hook(gb); return; // jp

state1:
  CYC(b_+33, b_+36); A = W8(wPaletteThread_mode);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+37, b_+40); interactionIncState_hook(gb); return; } // jp nz
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+42); A = 0x00; // Object.start
  CALL_C(b_+42, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+45);
  CALL_C(b_+45, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+48);
  if (!(F & FC)) { RET_TAKEN(b_+48); return; } // ret nc
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); A = 0x01;
  CYC(b_+51, b_+54); W8(wTmpcfc0_patchMinigame_failedGame) = A;
  CYC(b_+54, b_+56); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(b_+56, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+59);
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; } // ret nz
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+63, b_+65); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL);
  CYC(b_+66, b_+68); alu_sub(gb, 0x08);
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);
  CYC(b_+69, b_+72); interactionIncState_hook(gb); return; // jp

state2:
  CYC(b_+72, b_+75); A = W8(wTmpcfc0_patchMinigame_screenFadedOut);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+76); return; } // ret z
  CYC(b_+76, b_+77);
  CYC(b_+77, SYM(patch_subid06)); interactionDelete_hook(gb); return; // jp
}

void patch_subid04_hook(GB *gb) {
  patch_subid05_hook(gb);
}

// Fixed tuni nut (6) or sword (7) sprite. patch_subid06 and patch_subid07 both target
// 0a:7c40 in interactionCode94's jump table -- identical code, so patch_subid06 just
// hands off to patch_subid07.
void patch_subid07_hook(GB *gb) {
  BASE(patch_subid06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; } // jr z
  CYC(b_+3, b_+5);

  // patch_subid07@state1
  CALL_C(b_+5, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); interactionDelete_hook(gb); return; // jp

state0:
  CYC(b_+12, b_+15); A = W8(wTmpcfc0_patchMinigame_wonMinigame);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+16); return; } // ret z
  CYC(b_+16, b_+17);
  CALL_C(b_+17, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+20);
  CALL_C(b_+20, interactionIncState_hook, SYM(interactionIncState), b_+23);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 60);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+32, b_+34); goto getPosition; } // jr z
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+37); A = W8(wTmpcfc0_patchMinigame_swordLevel);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto getPosition; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+42, b_+44); A = 0x04;
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+48); A = 0x0c;
  CALL_C(b_+48, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+51);

getPosition:
  CYC(b_+51, b_+53); A = 0x00; // Object.start
  CALL_C(b_+53, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+56);
  CYC(b_+56, b_+59); SET_BC(0xf2f8);
  CALL_C(b_+59, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+62);
  CYC(b_+62, SYM(interactionCode95)); objectSetVisible81_hook(gb); return; // jp
}

void patch_subid06_hook(GB *gb) {
  patch_subid07_hook(gb);
}
