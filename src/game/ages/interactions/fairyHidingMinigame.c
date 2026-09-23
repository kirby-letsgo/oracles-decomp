#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t fairyHidingMinigame_jump_table(GB *gb) {
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

// fairyHidingMinigame_subid00: begins the fairy-hiding minigame.
void fairyHidingMinigame_subid00_hook(GB *gb) {
  BASE(fairyHidingMinigame_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = fairyHidingMinigame_jump_table(gb);
    if (target == b_+10) goto state0;
    if (target == b_+61) goto state1;
    goto state2;
  }

state0:
  CYC(b_+10, b_+12); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+12, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+15);
  if (!(F & FC)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nc
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); A = 0x0e; // GLOBALFLAG_WON_FAIRY_HIDING_GAME
  CALL_C(b_+20, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+29); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+31, b_+34); interactionIncState_hook(gb); return; } // jp z
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); alu_sub(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto spawn3Fairies; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0x13; // CUTSCENE_FAIRIES_HIDE
  CYC(b_+41, b_+44); W8(wCutsceneTrigger) = A;
  CYC(b_+44, b_+46); A = 0x80;
  CYC(b_+46, b_+49); W8(wMenuDisabled) = A;
  CYC(b_+49, b_+51); A = 0x21; // DISABLE_COMPANION | DISABLE_LINK
  CYC(b_+51, b_+54); W8(wDisabledObjects) = A;
  CYC(b_+54, b_+55); alu_xor(gb, A);
  CYC(b_+55, b_+58); W8(w1Link_direction) = A;

spawn3Fairies:
  CYC(b_+58, b_+61); TAIL(fairyHidingMinigame_spawn3FairiesAndDelete); // jp

state1:
  CALL_C(b_+61, fairyHidingMinigame_checkBeginCutscene_hook, SYM(fairyHidingMinigame_checkBeginCutscene), b_+64);
  if (!(F & FC)) { RET_TAKEN(b_+64); return; } // ret nc
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+68); A = W8(wScreenTransitionDirection);
  CYC(b_+68, b_+71); W8(w1Link_direction) = A;
  CYC(b_+71, b_+73); A = 0x01;
  CYC(b_+73, b_+76); W8(wTmpcfc0_fairyHideAndSeek_active) = A;
  CYC(b_+76, b_+79); SET_HL((SYM(miscPuzzles_subid19__state4) + 15)); // mainScripts.fairyHidingMinigame_subid00Script
  CYC(b_+79, b_+82); TAIL(interactionSetScript); // jp

state2:
  CALL_C(b_+82, interactionRunScript_hook, SYM(interactionRunScript), b_+85);
  if (!(F & FC)) { RET_TAKEN(b_+85); return; } // ret nc
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); A = 0x13; // CUTSCENE_FAIRIES_HIDE
  CYC(b_+88, b_+91); W8(wCutsceneTrigger) = A;
  CYC(b_+91, b_+94); TAIL(interactionDelete); // jp
}

// fairyHidingMinigame_spawn3FairiesAndDelete: spawns the 3 fairies; they should delete
// themselves if they're not found yet.
void fairyHidingMinigame_spawn3FairiesAndDelete_hook(GB *gb) {
  BASE(fairyHidingMinigame_spawn3FairiesAndDelete);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x03;

spawnFairy:
  CALL_C(b_+2, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x49); // INTERAC_FOREST_FAIRY
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = $01
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); B = alu_dec8(gb, B);
  CYC(b_+12, b_+13); mem_wr(gb, HL, B); // [var03] = 0,1,2
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto spawnFairy; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); TAIL(interactionDelete); // jp
}

// fairyHidingMinigame_checkBeginCutscene: sets cflag if Link is vulnerable (ready to begin
// the cutscene). Called from fairyHidingMinigame_subid00 and _subid01.
void fairyHidingMinigame_checkBeginCutscene_hook(GB *gb) {
  BASE(fairyHidingMinigame_checkBeginCutscene);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } // ret nc
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x80;
  CYC(b_+6, b_+9); W8(wMenuDisabled) = A;
  CYC(b_+9, b_+11); A = 0x21; // DISABLE_COMPANION | DISABLE_LINK
  CYC(b_+11, b_+14); W8(wDisabledObjects) = A;
  CALL_C(b_+14, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+17);
  CALL_C(b_+17, clearAllParentItems_hook, SYM(clearAllParentItems), b_+20);
  CALL_C(b_+20, interactionIncState_hook, SYM(interactionIncState), b_+23);
  CYC(b_+23, b_+24); alu_scf(gb);
  RET(b_+24); return; // ret
}

// fairyHidingMinigame_checkMinigameActive: sets cflag if the minigame is active. Called from
// fairyHidingMinigame_subid01 and _subid02.
void fairyHidingMinigame_checkMinigameActive_hook(GB *gb) {
  BASE(fairyHidingMinigame_checkMinigameActive);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0e; // GLOBALFLAG_WON_FAIRY_HIDING_GAME
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wTmpcfc0_fairyHideAndSeek_active);
  CYC(b_+9, b_+10); alu_rrca(gb);
  RET(b_+10); return; // ret
}

// fairyHidingMinigame_subid01: hiding spot for a fairy.
void fairyHidingMinigame_subid01_hook(GB *gb) {
  BASE(fairyHidingMinigame_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = fairyHidingMinigame_jump_table(gb);
    if (target == b_+12) goto state0;
    if (target == b_+64) goto state1;
    if (target == b_+86) goto state2;
    goto state3;
  }

state0:
  CALL_C(b_+12, fairyHidingMinigame_checkMinigameActive_hook, SYM(fairyHidingMinigame_checkMinigameActive), b_+15);
  if (!(F & FC)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nc
  CYC(b_+15, b_+18);
  CALL_C(b_+18, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+21);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_VAR38; // Interaction.var38
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); E = L;
  CYC(b_+25, b_+28); SET_HL(b_+57); // fairyHidingMinigame_subid01@table
  CALL_C(b_+28, lookupKey_hook, SYM(lookupKey), b_+31);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+36); alu_sub(gb, 0x03);
  CYC(b_+36, b_+39); SET_HL(wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CALL_C(b_+39, checkFlag_hook, SYM(checkFlag), b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+45); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+46); alu_xor(gb, A);
  CYC(b_+46, b_+49); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+51, b_+53); A = 0x0c;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+57); TAIL(interactionIncState); // jp

state1:
  CALL_C(b_+64, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+67);
  CYC(b_+67, b_+68); B = A;
  CYC(b_+68, b_+70); E = INTERACTION_BASE + OBJ_VAR38; // Interaction.var38
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  CYC(b_+71, b_+72); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+72); return; } // ret z
  CYC(b_+72, b_+73);
  CALL_C(b_+73, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+76);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; } // ret nz
  CYC(b_+76, b_+77);
  CALL_C(b_+77, fairyHidingMinigame_checkBeginCutscene_hook, SYM(fairyHidingMinigame_checkBeginCutscene), b_+80);
  if (!(F & FC)) { RET_TAKEN(b_+80); return; } // ret nc
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); A = 0x01;
  CYC(b_+83, b_+86); W8(wDisableScreenTransitions) = A;

state2:
  CALL_C(b_+86, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+89);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; } // ret nz
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x49); // INTERAC_FOREST_FAIRY
  CYC(b_+92, b_+94); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+94, b_+95); E = L;
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+97); mem_wr(gb, HL, A);
  CALL_C(b_+97, objectCreatePuff_hook, SYM(objectCreatePuff), b_+100);
  CALL_C(b_+100, interactionIncState_hook, SYM(interactionIncState), b_+103);
  CYC(b_+103, b_+106); SET_HL((SYM(miscPuzzles_subid19__state4) + 19)); // mainScripts.fairyHidingMinigame_subid01Script
  CYC(b_+106, b_+109); TAIL(interactionSetScript); // jp

state3:
  CALL_C(b_+109, interactionRunScript_hook, SYM(interactionRunScript), b_+112);
  if (!(F & FC)) { RET_TAKEN(b_+112); return; } // ret nc
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+115); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); alu_sub(gb, 0x03);
  CYC(b_+118, b_+121); SET_HL(wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CALL_C(b_+121, setFlag_hook, SYM(setFlag), b_+124);
  CYC(b_+124, b_+127); A = W8(wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CYC(b_+127, b_+129); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+129, b_+131); goto warpOut; } // jr z
  CYC(b_+129, b_+131);
  CYC(b_+131, b_+132); alu_xor(gb, A);
  CYC(b_+132, b_+135); W8(wMenuDisabled) = A;
  CYC(b_+135, b_+138); W8(wDisabledObjects) = A;
  CYC(b_+138, b_+141); W8(wDisableScreenTransitions) = A;
  CYC(b_+141, b_+143); goto delete_; // jr

warpOut:
  CYC(b_+143, b_+146); SET_HL(b_+152); // fairyHidingMinigame_subid01@warpDestination
  CALL_C(b_+146, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+149);

delete_:
  CYC(b_+149, b_+152); TAIL(interactionDelete); // jp
}

// fairyHidingMinigame_subid02: checks for Link leaving the hide-and-seek area.
void fairyHidingMinigame_subid02_hook(GB *gb) {
  BASE(fairyHidingMinigame_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; } // jr z
  CYC(b_+4, b_+6);

  // fairyHidingMinigame_subid02@state1
  CALL_C(b_+6, interactionRunScript_hook, SYM(interactionRunScript), b_+9);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; } // ret nc
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(b_+13, b_+15); B = 0x10;
  CALL_C(b_+15, clearMemory_hook, SYM(clearMemory), b_+18);
  CYC(b_+18, b_+21); TAIL(interactionDelete); // jp

state0:
  CALL_C(b_+21, fairyHidingMinigame_checkMinigameActive_hook, SYM(fairyHidingMinigame_checkMinigameActive), b_+24);
  if (!(F & FC)) { CYCT(b_+24, b_+27); interactionDelete_hook(gb); return; } // jp nc
  CYC(b_+24, b_+27);
  CALL_C(b_+27, interactionIncState_hook, SYM(interactionIncState), b_+30);
  CYC(b_+30, b_+33); SET_HL((SYM(miscPuzzles_subid19__state4) + 23)); // mainScripts.fairyHidingMinigame_subid02Script
  CYC(b_+33, b_+36); TAIL(interactionSetScript); // jp
}

// INTERAC_FAIRY_HIDING_MINIGAME
void interactionCode6c_hook(GB *gb) {
  BASE(interactionCode6c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
    uint16_t target = fairyHidingMinigame_jump_table(gb);
    if (target == SYM(fairyHidingMinigame_subid00)) { fairyHidingMinigame_subid00_hook(gb); return; }
    if (target == SYM(fairyHidingMinigame_subid01)) { fairyHidingMinigame_subid01_hook(gb); return; }
    if (target == SYM(fairyHidingMinigame_subid02)) TAIL(fairyHidingMinigame_subid02);
    hook_continue(gb, target, sp0_); return;
  }
}
