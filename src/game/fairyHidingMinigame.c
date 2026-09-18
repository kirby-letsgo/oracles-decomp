#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x52a8, 0x52aa); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x52aa, 0x52ab); A = mem_rd(gb, DE);
  {
    CYC(0x52ab, 0x52ac); push_effect(gb, 0x52ac);
    uint16_t target = fairyHidingMinigame_jump_table(gb);
    if (target == 0x52b2) goto state0;
    if (target == 0x52e5) goto state1;
    goto state2;
  }

state0:
  CYC(0x52b2, 0x52b4); A = 0x40; // TREASURE_ESSENCE
  CALL_C(0x52b4, checkTreasureObtained_hook, 0x1748, 0x52b7);
  if (!(F & FC)) { CYCT(0x52b7, 0x52ba); interactionDelete_hook(gb); return; } // jp nc
  CYC(0x52b7, 0x52ba);
  CYC(0x52ba, 0x52bc); A = 0x0e; // GLOBALFLAG_WON_FAIRY_HIDING_GAME
  CALL_C(0x52bc, checkGlobalFlag_hook, 0x31f3, 0x52bf);
  if (!(F & FZ)) { CYCT(0x52bf, 0x52c2); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x52bf, 0x52c2);
  CYC(0x52c2, 0x52c5); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(0x52c5, 0x52c6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x52c6, 0x52c7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x52c7, 0x52ca); interactionIncState_hook(gb); return; } // jp z
  CYC(0x52c7, 0x52ca);
  CYC(0x52ca, 0x52cb); A = mem_rd(gb, HL);
  CYC(0x52cb, 0x52cd); alu_sub(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x52cd, 0x52cf); goto spawn3Fairies; } // jr nz
  CYC(0x52cd, 0x52cf);
  CYC(0x52cf, 0x52d1); A = 0x13; // CUTSCENE_FAIRIES_HIDE
  CYC(0x52d1, 0x52d4); W8(wCutsceneTrigger) = A;
  CYC(0x52d4, 0x52d6); A = 0x80;
  CYC(0x52d6, 0x52d9); W8(wMenuDisabled) = A;
  CYC(0x52d9, 0x52db); A = 0x21; // DISABLE_COMPANION | DISABLE_LINK
  CYC(0x52db, 0x52de); W8(wDisabledObjects) = A;
  CYC(0x52de, 0x52df); alu_xor(gb, A);
  CYC(0x52df, 0x52e2); W8(w1Link_direction) = A;

spawn3Fairies:
  CYC(0x52e2, 0x52e5); fairyHidingMinigame_spawn3FairiesAndDelete_hook(gb); return; // jp

state1:
  CALL_C(0x52e5, fairyHidingMinigame_checkBeginCutscene_hook, 0x53d9, 0x52e8);
  if (!(F & FC)) { RET_TAKEN(0x52e8); return; } // ret nc
  CYC(0x52e8, 0x52e9);
  CYC(0x52e9, 0x52ec); A = W8(wScreenTransitionDirection);
  CYC(0x52ec, 0x52ef); W8(w1Link_direction) = A;
  CYC(0x52ef, 0x52f1); A = 0x01;
  CYC(0x52f1, 0x52f4); W8(wTmpcfc0_fairyHideAndSeek_active) = A;
  CYC(0x52f4, 0x52f7); SET_HL(0x728d); // mainScripts.fairyHidingMinigame_subid00Script
  CYC(0x52f7, 0x52fa); interactionSetScript_hook(gb); return; // jp

state2:
  CALL_C(0x52fa, interactionRunScript_hook, 0x2552, 0x52fd);
  if (!(F & FC)) { RET_TAKEN(0x52fd); return; } // ret nc
  CYC(0x52fd, 0x52fe);
  CYC(0x52fe, 0x5300); A = 0x13; // CUTSCENE_FAIRIES_HIDE
  CYC(0x5300, 0x5303); W8(wCutsceneTrigger) = A;
  CYC(0x5303, 0x5306); interactionDelete_hook(gb); return; // jp
}

// fairyHidingMinigame_spawn3FairiesAndDelete: spawns the 3 fairies; they should delete
// themselves if they're not found yet.
void fairyHidingMinigame_spawn3FairiesAndDelete_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53c7, 0x53c9); B = 0x03;

spawnFairy:
  CALL_C(0x53c9, getFreeInteractionSlot_hook, 0x3aef, 0x53cc);
  if (!(F & FZ)) { RET_TAKEN(0x53cc); return; } // ret nz
  CYC(0x53cc, 0x53cd);
  CYC(0x53cd, 0x53cf); mem_wr(gb, HL, 0x49); // INTERAC_FOREST_FAIRY
  CYC(0x53cf, 0x53d0); L = alu_inc8(gb, L);
  CYC(0x53d0, 0x53d1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = $01
  CYC(0x53d1, 0x53d2); L = alu_inc8(gb, L);
  CYC(0x53d2, 0x53d3); B = alu_dec8(gb, B);
  CYC(0x53d3, 0x53d4); mem_wr(gb, HL, B); // [var03] = 0,1,2
  if (!(F & FZ)) { CYCT(0x53d4, 0x53d6); goto spawnFairy; } // jr nz
  CYC(0x53d4, 0x53d6);
  CYC(0x53d6, 0x53d9); interactionDelete_hook(gb); return; // jp
}

// fairyHidingMinigame_checkBeginCutscene: sets cflag if Link is vulnerable (ready to begin
// the cutscene). Called from fairyHidingMinigame_subid00 and _subid01.
void fairyHidingMinigame_checkBeginCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x53d9, checkLinkVulnerable_hook, 0x1d28, 0x53dc);
  if (!(F & FC)) { RET_TAKEN(0x53dc); return; } // ret nc
  CYC(0x53dc, 0x53dd);
  CYC(0x53dd, 0x53df); A = 0x80;
  CYC(0x53df, 0x53e2); W8(wMenuDisabled) = A;
  CYC(0x53e2, 0x53e4); A = 0x21; // DISABLE_COMPANION | DISABLE_LINK
  CYC(0x53e4, 0x53e7); W8(wDisabledObjects) = A;
  CALL_C(0x53e7, dropLinkHeldItem_hook, 0x2c43, 0x53ea);
  CALL_C(0x53ea, clearAllParentItems_hook, 0x2c10, 0x53ed);
  CALL_C(0x53ed, interactionIncState_hook, 0x23e0, 0x53f0);
  CYC(0x53f0, 0x53f1); alu_scf(gb);
  RET(0x53f1); return; // ret
}

// fairyHidingMinigame_checkMinigameActive: sets cflag if the minigame is active. Called from
// fairyHidingMinigame_subid01 and _subid02.
void fairyHidingMinigame_checkMinigameActive_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53f2, 0x53f4); A = 0x0e; // GLOBALFLAG_WON_FAIRY_HIDING_GAME
  CALL_C(0x53f4, checkGlobalFlag_hook, 0x31f3, 0x53f7);
  if (!(F & FZ)) { RET_TAKEN(0x53f7); return; } // ret nz
  CYC(0x53f7, 0x53f8);
  CYC(0x53f8, 0x53fb); A = W8(wTmpcfc0_fairyHideAndSeek_active);
  CYC(0x53fb, 0x53fc); alu_rrca(gb);
  RET(0x53fc); return; // ret
}

// fairyHidingMinigame_subid01: hiding spot for a fairy.
void fairyHidingMinigame_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5306, 0x5308); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5308, 0x5309); A = mem_rd(gb, DE);
  {
    CYC(0x5309, 0x530a); push_effect(gb, 0x530a);
    uint16_t target = fairyHidingMinigame_jump_table(gb);
    if (target == 0x5312) goto state0;
    if (target == 0x5346) goto state1;
    if (target == 0x535c) goto state2;
    goto state3;
  }

state0:
  CALL_C(0x5312, fairyHidingMinigame_checkMinigameActive_hook, 0x53f2, 0x5315);
  if (!(F & FC)) { CYCT(0x5315, 0x5318); interactionDelete_hook(gb); return; } // jp nc
  CYC(0x5315, 0x5318);
  CALL_C(0x5318, objectGetTileAtPosition_hook, 0x1444, 0x531b);
  CYC(0x531b, 0x531d); E = INTERACTION_BASE + OBJ_VAR38; // Interaction.var38
  CYC(0x531d, 0x531e); mem_wr(gb, DE, A);
  CYC(0x531e, 0x531f); E = L;
  CYC(0x531f, 0x5322); SET_HL(0x533f); // fairyHidingMinigame_subid01@table
  CALL_C(0x5322, lookupKey_hook, 0x1e06, 0x5325);
  CYC(0x5325, 0x5327); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5327, 0x5328); mem_wr(gb, DE, A);
  CYC(0x5328, 0x532a); alu_sub(gb, 0x03);
  CYC(0x532a, 0x532d); SET_HL(wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CALL_C(0x532d, checkFlag_hook, 0x0205, 0x5330);
  if (!(F & FZ)) { CYCT(0x5330, 0x5333); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5330, 0x5333);
  CYC(0x5333, 0x5334); alu_xor(gb, A);
  CYC(0x5334, 0x5337); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  CYC(0x5337, 0x5339); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5339, 0x533b); A = 0x0c;
  CYC(0x533b, 0x533c); mem_wr(gb, DE, A);
  CYC(0x533c, 0x533f); interactionIncState_hook(gb); return; // jp

state1:
  CALL_C(0x5346, objectGetTileAtPosition_hook, 0x1444, 0x5349);
  CYC(0x5349, 0x534a); B = A;
  CYC(0x534a, 0x534c); E = INTERACTION_BASE + OBJ_VAR38; // Interaction.var38
  CYC(0x534c, 0x534d); A = mem_rd(gb, DE);
  CYC(0x534d, 0x534e); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(0x534e); return; } // ret z
  CYC(0x534e, 0x534f);
  CALL_C(0x534f, interactionDecCounter1_hook, 0x23cc, 0x5352);
  if (!(F & FZ)) { RET_TAKEN(0x5352); return; } // ret nz
  CYC(0x5352, 0x5353);
  CALL_C(0x5353, fairyHidingMinigame_checkBeginCutscene_hook, 0x53d9, 0x5356);
  if (!(F & FC)) { RET_TAKEN(0x5356); return; } // ret nc
  CYC(0x5356, 0x5357);
  CYC(0x5357, 0x5359); A = 0x01;
  CYC(0x5359, 0x535c); W8(wDisableScreenTransitions) = A;

state2:
  CALL_C(0x535c, getFreeInteractionSlot_hook, 0x3aef, 0x535f);
  if (!(F & FZ)) { RET_TAKEN(0x535f); return; } // ret nz
  CYC(0x535f, 0x5360);
  CYC(0x5360, 0x5362); mem_wr(gb, HL, 0x49); // INTERAC_FOREST_FAIRY
  CYC(0x5362, 0x5364); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5364, 0x5365); E = L;
  CYC(0x5365, 0x5366); A = mem_rd(gb, DE);
  CYC(0x5366, 0x5367); mem_wr(gb, HL, A);
  CALL_C(0x5367, objectCreatePuff_hook, 0x24c1, 0x536a);
  CALL_C(0x536a, interactionIncState_hook, 0x23e0, 0x536d);
  CYC(0x536d, 0x5370); SET_HL(0x7291); // mainScripts.fairyHidingMinigame_subid01Script
  CYC(0x5370, 0x5373); interactionSetScript_hook(gb); return; // jp

state3:
  CALL_C(0x5373, interactionRunScript_hook, 0x2552, 0x5376);
  if (!(F & FC)) { RET_TAKEN(0x5376); return; } // ret nc
  CYC(0x5376, 0x5377);
  CYC(0x5377, 0x5379); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5379, 0x537a); A = mem_rd(gb, DE);
  CYC(0x537a, 0x537c); alu_sub(gb, 0x03);
  CYC(0x537c, 0x537f); SET_HL(wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CALL_C(0x537f, setFlag_hook, 0x020e, 0x5382);
  CYC(0x5382, 0x5385); A = W8(wTmpcfc0_fairyHideAndSeek_foundFairiesBitset);
  CYC(0x5385, 0x5387); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x5387, 0x5389); goto warpOut; } // jr z
  CYC(0x5387, 0x5389);
  CYC(0x5389, 0x538a); alu_xor(gb, A);
  CYC(0x538a, 0x538d); W8(wMenuDisabled) = A;
  CYC(0x538d, 0x5390); W8(wDisabledObjects) = A;
  CYC(0x5390, 0x5393); W8(wDisableScreenTransitions) = A;
  CYC(0x5393, 0x5395); goto delete_; // jr

warpOut:
  CYC(0x5395, 0x5398); SET_HL(0x539e); // fairyHidingMinigame_subid01@warpDestination
  CALL_C(0x5398, setWarpDestVariables_hook, 0x1997, 0x539b);

delete_:
  CYC(0x539b, 0x539e); interactionDelete_hook(gb); return; // jp
}

// fairyHidingMinigame_subid02: checks for Link leaving the hide-and-seek area.
void fairyHidingMinigame_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53a3, 0x53a5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x53a5, 0x53a6); A = mem_rd(gb, DE);
  CYC(0x53a6, 0x53a7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x53a7, 0x53a9); goto state0; } // jr z
  CYC(0x53a7, 0x53a9);

  // fairyHidingMinigame_subid02@state1
  CALL_C(0x53a9, interactionRunScript_hook, 0x2552, 0x53ac);
  if (!(F & FC)) { RET_TAKEN(0x53ac); return; } // ret nc
  CYC(0x53ac, 0x53ad);
  CYC(0x53ad, 0x53b0); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(0x53b0, 0x53b2); B = 0x10;
  CALL_C(0x53b2, clearMemory_hook, 0x046f, 0x53b5);
  CYC(0x53b5, 0x53b8); interactionDelete_hook(gb); return; // jp

state0:
  CALL_C(0x53b8, fairyHidingMinigame_checkMinigameActive_hook, 0x53f2, 0x53bb);
  if (!(F & FC)) { CYCT(0x53bb, 0x53be); interactionDelete_hook(gb); return; } // jp nc
  CYC(0x53bb, 0x53be);
  CALL_C(0x53be, interactionIncState_hook, 0x23e0, 0x53c1);
  CYC(0x53c1, 0x53c4); SET_HL(0x7295); // mainScripts.fairyHidingMinigame_subid02Script
  CYC(0x53c4, 0x53c7); interactionSetScript_hook(gb); return; // jp
}

// INTERAC_FAIRY_HIDING_MINIGAME
void interactionCode6c_hook(GB *gb) {
  CYC(0x529e, 0x52a0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x52a0, 0x52a1); A = mem_rd(gb, DE);
  {
    CYC(0x52a1, 0x52a2); push_effect(gb, 0x52a2);
    uint16_t target = fairyHidingMinigame_jump_table(gb);
    if (target == 0x52a8) { fairyHidingMinigame_subid00_hook(gb); return; }
    if (target == 0x5306) { fairyHidingMinigame_subid01_hook(gb); return; }
    fairyHidingMinigame_subid02_hook(gb); return; // target == 0x53a3
  }
}
