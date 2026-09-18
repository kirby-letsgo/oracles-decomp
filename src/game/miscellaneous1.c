#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

// interaction6b_subid00: handles showing Impa's "Help" text when Link's about to screen
// transition. decCounter1IfTextNotActive is called once from @substate1 (bare sp0_) and its own
// two exits (the early "ret nz" and the tail-jump into interactionDecCounter1_hook) both resume
// there, since substate1's own "ret nz" then re-checks the very same flags decCounter1IfTextNot-
// Active left behind.
void interaction6b_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4e26, checkInteractionState_hook, 0x23fe, 0x4e29);
  if (!(F & FZ)) { CYCT(0x4e29, 0x4e2b); goto state1; } // jr nz
  CYC(0x4e29, 0x4e2b);

  // interaction6b_subid00@state0
  CYC(0x4e2b, 0x4e2d); A = 0x01;
  CYC(0x4e2d, 0x4e2e); mem_wr(gb, DE, A);
  CALL_C(0x4e2e, getThisRoomFlags_hook, 0x197d, 0x4e31);
  CYC(0x4e31, 0x4e33); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x4e33, 0x4e36); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x4e33, 0x4e36);

state1:
  CALL_C(0x4e36, checkInteractionSubstate_hook, 0x2403, 0x4e39);
  if (!(F & FZ)) { CYCT(0x4e39, 0x4e3b); goto substate1; } // jr nz
  CYC(0x4e39, 0x4e3b);

  // interaction6b_subid00@substate0
  CALL_C(0x4e3b, interaction6b_checkLinkPressedUpAtScreenEdge_hook, 0x5265, 0x4e3e);
  if (F & FZ) { RET_TAKEN(0x4e3e); return; } // ret z
  CYC(0x4e3e, 0x4e3f);
  CYC(0x4e3f, 0x4e41); A = 0x01;
  CYC(0x4e41, 0x4e44); W8(wMenuDisabled) = A;
  CYC(0x4e44, 0x4e47); W8(wDisabledObjects) = A;
  CYC(0x4e47, 0x4e49); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4e49, 0x4e4b); A = 30;
  CYC(0x4e4b, 0x4e4c); mem_wr(gb, DE, A);
  CYC(0x4e4c, 0x4e4f); SET_BC(0x0100); // TX_0100
  CALL_C(0x4e4f, showText_hook, 0x1872, 0x4e52);
  CYC(0x4e52, 0x4e55); interactionIncSubstate_hook(gb); return; // jp

substate1:
  CYC(0x4e55, 0x4e58); push_effect(gb, 0x4e58); goto decCounter1IfTextNotActive;
afterDecCounter1IfTextNotActive:
  if (!(F & FZ)) { RET_TAKEN(0x4e58); return; } // ret nz
  CYC(0x4e58, 0x4e59);
  CYC(0x4e59, 0x4e5a); alu_xor(gb, A);
  CYC(0x4e5a, 0x4e5d); W8(wDisabledObjects) = A;
  CYC(0x4e5d, 0x4e5e); push_effect(gb, DE);
  CYC(0x4e5e, 0x4e61); SET_HL(0x4e77); // interaction6b_subid00@simulatedInput
  CYC(0x4e61, 0x4e63); A = 0x0a; // bank of interaction6b_subid00@simulatedInput
  CALL_C(0x4e63, setSimulatedInputAddress_hook, 0x2a1d, 0x4e66);
  CYC(0x4e66, 0x4e67); SET_DE(pop_effect(gb));
  CALL_C(0x4e67, getThisRoomFlags_hook, 0x197d, 0x4e6a);
  CYC(0x4e6a, 0x4e6c); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(0x4e6c, 0x4e6f); interactionDelete_hook(gb); return; // jp

decCounter1IfTextNotActive:
  CYC(0x4e6f, 0x4e72); A = W8(wTextIsActive);
  CYC(0x4e72, 0x4e73); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(0x4e73);
    if (gb->pc == 0x4e58 && gb->sp == sp0_) goto afterDecCounter1IfTextNotActive;
    return;
  } // ret nz
  CYC(0x4e73, 0x4e74);
  CYC(0x4e74, 0x4e77); interactionDecCounter1_hook(gb);
  if (gb->pc == 0x4e58 && gb->sp == sp0_) goto afterDecCounter1IfTextNotActive;
  return; // jp
}

// @param[out] zflag  nz if Link pressed up at screen edge
void interaction6b_checkLinkPressedUpAtScreenEdge_hook(GB *gb) {
  CYC(0x5265, 0x5268); A = W8(wScrollMode);
  CYC(0x5268, 0x526a); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x526a, 0x526c); goto l5274; } // jr nz
  CYC(0x526a, 0x526c);
  CYC(0x526c, 0x526f); SET_HL(0xd00b); // w1Link.yh
  CYC(0x526f, 0x5270); A = mem_rd(gb, HL);
  CYC(0x5270, 0x5272); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(0x5272, 0x5274); goto l5276; } // jr c
  CYC(0x5272, 0x5274);

l5274:
  CYC(0x5274, 0x5275); alu_xor(gb, A);
  RET(0x5275); return; // ret

l5276:
  CYC(0x5276, 0x5279); A = W8(wKeysPressed);
  CYC(0x5279, 0x527b); alu_and(gb, 0x40); // BTN_UP
  RET(0x527b); return; // ret
}

static uint16_t interaction6b_subid01_jump_table(GB *gb) {
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

// interaction6b_subid01: spawns Nayru, Ralph, and the animals in the intro cutscene before
// she's possessed.
void interaction6b_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e7c, 0x4e7e); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4e7e, 0x4e7f); A = mem_rd(gb, DE);
  {
    CYC(0x4e7f, 0x4e80); push_effect(gb, 0x4e80);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == 0x4e84) goto state0;
    goto state1;
  }

state0:
  CYC(0x4e84, 0x4e86); A = 0x01;
  CYC(0x4e86, 0x4e87); mem_wr(gb, DE, A);
  CYC(0x4e87, 0x4e89); A = 0x0a; // GLOBALFLAG_INTRO_DONE
  CALL_C(0x4e89, checkGlobalFlag_hook, 0x31f3, 0x4e8c);
  if (!(F & FZ)) { CYCT(0x4e8c, 0x4e8e); goto delete_; } // jr nz
  CYC(0x4e8c, 0x4e8e);
  CYC(0x4e8e, 0x4e91); SET_HL(0x788b); // objectData.nayruAndAnimalsInIntro
  CALL_C(0x4e91, parseGivenObjectData_b00_hook, 0x3171, 0x4e94);
  CYC(0x4e94, 0x4e96); A = 0x36; // INTERAC_NAYRU
  CYC(0x4e96, 0x4e99); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(0x4e99, 0x4e9a); push_effect(gb, DE);
  CYC(0x4e9a, 0x4e9c); A = 0x3a; // UNCMP_GFXH_AGES_IMPA_FAINTED
  CALL_C(0x4e9c, loadUncompressedGfxHeader_hook, 0x05da, 0x4e9f);
  CYC(0x4e9f, 0x4ea0); SET_DE(pop_effect(gb));

delete_:
  CYC(0x4ea0, 0x4ea3); interactionDelete_hook(gb); return; // jp

state1:
  // Never executed (deletes self before running state 1).
  CALL_C(0x4ea3, interactionRunScript_hook, 0x2552, 0x4ea6);
  if (F & FC) { CYCT(0x4ea6, 0x4ea9); interactionDelete_hook(gb); return; } // jp c
  CYC(0x4ea6, 0x4ea9);
  RET(0x4ea9); return; // ret
}

// interaction6b_subid02: script for the cutscene with Ralph outside Ambi's palace, before
// getting the mystery seeds.
void interaction6b_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4eaa, checkInteractionState_hook, 0x23fe, 0x4ead);
  if (!(F & FZ)) { CYCT(0x4ead, 0x4eaf); goto state1; } // jr nz
  CYC(0x4ead, 0x4eaf);

  // interaction6b_subid02@state0
  CYC(0x4eaf, 0x4eb1); A = 0x24; // TREASURE_MYSTERY_SEEDS
  CALL_C(0x4eb1, checkTreasureObtained_hook, 0x1748, 0x4eb4);
  if (F & FC) { CYCT(0x4eb4, 0x4eb7); interactionDelete_hook(gb); return; } // jp c
  CYC(0x4eb4, 0x4eb7);
  CYC(0x4eb7, 0x4eba); interaction6b_loadScript_hook(gb); return; // jp

state1:
  CALL_C(0x4eba, interactionRunScript_hook, 0x2552, 0x4ebd);
  if (F & FC) { CYCT(0x4ebd, 0x4ec0); interactionDelete_hook(gb); return; } // jp c
  CYC(0x4ebd, 0x4ec0);
  RET(0x4ec0); return; // ret
}

static void interaction6b_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interaction6b_loadScript: fallen into from interaction6b_initGraphicsAndLoadScript, and
// tail-jumped into from interaction6b_subid02/subid04/subid05 (each at bare sp0_, so nothing of
// theirs is left outstanding here or after the trailing tail-jump into interactionIncState_hook).
void interaction6b_loadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5255, 0x5257); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5257, 0x5258); A = mem_rd(gb, DE);
  CYC(0x5258, 0x525b); SET_HL(0x527c); // interaction6b_scriptTable
  CYC(0x525b, 0x525c); interaction6b_addDoubleIndexToHl_from_rst(gb, 0x525c);
  CYC(0x525c, 0x525d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x525d, 0x525e); H = mem_rd(gb, HL);
  CYC(0x525e, 0x525f); L = A;
  CALL_C(0x525f, interactionSetScript_hook, 0x2544, 0x5262);
  CYC(0x5262, 0x5265); interactionIncState_hook(gb); return; // jp
}

void interaction6b_initGraphicsAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x524c, interactionInitGraphics_hook, 0x15fb, 0x524f);
  CYC(0x524f, 0x5252); interactionIncState_hook(gb); return; // jp
}

// interaction6b_subid03 (also interaction6b_subid12, an alias for the same code): the Seasons
// troupe member with a guitar or tambourine.
void interaction6b_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ec1, checkInteractionState_hook, 0x23fe, 0x4ec4);
  if (!(F & FZ)) { CYCT(0x4ec4, 0x4ec7); interactionAnimate_hook(gb); return; } // jp nz
  CYC(0x4ec4, 0x4ec7);

  // interaction6b_subid12@state0
  CALL_C(0x4ec7, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x4eca);
  CYC(0x4eca, 0x4ecd); objectSetVisible82_hook(gb); return; // jp
}

// interaction6b_subid04: script for the cutscene where moblins attack the maku sapling. Reaches
// interaction6b_loadScript via a genuine call (unlike subid02's tail-jump), so it falls back
// through to state1 normally -- CALL_C's own return-address/stack check handles that transparently
// regardless of what loadScript's own internal tail-jump chain did.
void interaction6b_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ecd, checkInteractionState_hook, 0x23fe, 0x4ed0);
  if (!(F & FZ)) { CYCT(0x4ed0, 0x4ed2); goto state1; } // jr nz
  CYC(0x4ed0, 0x4ed2);

  // interaction6b_subid04@state0
  CYC(0x4ed2, 0x4ed3); alu_xor(gb, A);
  CYC(0x4ed3, 0x4ed6); W8(wccd4) = A;
  CALL_C(0x4ed6, interaction6b_loadScript_hook, 0x5255, 0x4ed9);

state1:
  CALL_C(0x4ed9, interactionRunScript_hook, 0x2552, 0x4edc);
  if (F & FC) { CYCT(0x4edc, 0x4edf); interactionDelete_hook(gb); return; } // jp c
  CYC(0x4edc, 0x4edf);
  RET(0x4edf); return; // ret
}

// interaction6b_subid05: the intro cutscene where lightning strikes a guy. State 0's jump-table
// entry lands on interaction6b_subid02's own @loadScript address (0x4eb7) -- the exact same
// tail-jump into interaction6b_loadScript_hook that subid02_hook itself already models at that
// address, reached here through a completely different top-level routine's dispatch.
void interaction6b_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ee0, 0x4ee2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4ee2, 0x4ee3); A = mem_rd(gb, DE);
  {
    CYC(0x4ee3, 0x4ee4); push_effect(gb, 0x4ee4);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == 0x4eb7) { CYC(0x4eb7, 0x4eba); interaction6b_loadScript_hook(gb); return; }
    goto state1;
  }

state1:
  CALL_C(0x4ee8, checkInteractionSubstate_hook, 0x2403, 0x4eeb);
  if (!(F & FZ)) { CYCT(0x4eeb, 0x4eed); goto substate1; } // jr nz
  CYC(0x4eeb, 0x4eed);

  // interaction6b_subid05@substate0
  CALL_C(0x4eed, interactionRunScript_hook, 0x2552, 0x4ef0);
  if (!(F & FC)) { RET_TAKEN(0x4ef0); return; } // ret nc
  CYC(0x4ef0, 0x4ef1);
  CALL_C(0x4ef1, interactionIncSubstate_hook, 0x23e5, 0x4ef4);
  CYC(0x4ef4, 0x4ef6); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4ef6, 0x4ef8); mem_wr(gb, HL, 0x01);
  CYC(0x4ef8, 0x4ef9); L = alu_inc8(gb, L);
  CYC(0x4ef9, 0x4efb); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x4efb, 0x4efd); mem_wr(gb, HL, 0x00);

substate1:
  CALL_C(0x4efd, interactionDecCounter1_hook, 0x23cc, 0x4f00);
  if (!(F & FZ)) { RET_TAKEN(0x4f00); return; } // ret nz
  CYC(0x4f00, 0x4f01);
  CYC(0x4f01, 0x4f03); mem_wr(gb, HL, 0x14);
  CYC(0x4f03, 0x4f04); L = alu_inc8(gb, L);
  CYC(0x4f04, 0x4f05); A = mem_rd(gb, HL);
  CYC(0x4f05, 0x4f07); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x4f07, 0x4f0a); goto l4f12; } // jp nz
  CYC(0x4f07, 0x4f0a);
  CYC(0x4f0a, 0x4f0c); A = 0x03;
  CYC(0x4f0c, 0x4f0f); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(0x4f0f, 0x4f12); interactionDelete_hook(gb); return; // jp

l4f12:
  CYC(0x4f12, 0x4f13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f13, 0x4f16); SET_HL(0x4f2b); // interaction6b_subid05@lightningPositions
  CYC(0x4f16, 0x4f17); interaction6b_addDoubleIndexToHl_from_rst(gb, 0x4f17);
  CYC(0x4f17, 0x4f18); B = mem_rd(gb, HL);
  CYC(0x4f18, 0x4f19); SET_HL(HL + 1);
  CYC(0x4f19, 0x4f1a); C = mem_rd(gb, HL);
  CALL_C(0x4f1a, getFreePartSlot_hook, 0x3e8e, 0x4f1d);
  if (!(F & FZ)) { RET_TAKEN(0x4f1d); return; } // ret nz
  CYC(0x4f1d, 0x4f1e);
  CYC(0x4f1e, 0x4f20); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(0x4f20, 0x4f21); L = alu_inc8(gb, L);
  CYC(0x4f21, 0x4f22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f22, 0x4f23); L = alu_inc8(gb, L);
  CYC(0x4f23, 0x4f24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4f24, 0x4f26); L = PART_BASE + OBJ_YH;
  CYC(0x4f26, 0x4f27); mem_wr(gb, HL, B);
  CYC(0x4f27, 0x4f29); L = PART_BASE + OBJ_XH;
  CYC(0x4f29, 0x4f2a); mem_wr(gb, HL, C);
  RET(0x4f2a); return; // ret
}

// interaction6b_subid06: manages the cutscene after beating Dungeon 3.
void interaction6b_subid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f33, checkInteractionState_hook, 0x23fe, 0x4f36);
  if (!(F & FZ)) { CYCT(0x4f36, 0x4f38); goto state1; } // jr nz
  CYC(0x4f36, 0x4f38);

  // interaction6b_subid06@state0
  CYC(0x4f38, 0x4f3b); A = W8(wEssencesObtained);
  CYC(0x4f3b, 0x4f3d); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(0x4f3d, 0x4f3f); goto delete_; } // jr z
  CYC(0x4f3d, 0x4f3f);
  CALL_C(0x4f3f, getThisRoomFlags_hook, 0x197d, 0x4f42);
  CYC(0x4f42, 0x4f44); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x4f44, 0x4f47); goto delete_; } // jp nz
  CYC(0x4f44, 0x4f47);
  CYC(0x4f47, 0x4f49); A = 0x01;
  CYC(0x4f49, 0x4f4c); W8(wDisabledObjects) = A;
  CYC(0x4f4c, 0x4f4f); W8(wMenuDisabled) = A;
  CALL_C(0x4f4f, interactionIncState_hook, 0x23e0, 0x4f52);
  CYC(0x4f52, 0x4f54); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4f54, 0x4f56); mem_wr(gb, HL, 90);
  RET(0x4f56); return; // ret

delete_:
  CYC(0x4f57, 0x4f5a); interactionDelete_hook(gb); return; // jp

state1:
  CYC(0x4f5a, 0x4f5c); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4f5c, 0x4f5d); A = mem_rd(gb, DE);
  {
    CYC(0x4f5d, 0x4f5e); push_effect(gb, 0x4f5e);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == 0x4f64) goto substate0;
    if (target == 0x4f7a) goto substate1;
    goto substate2;
  }

substate0:
  CALL_C(0x4f64, interactionDecCounter1_hook, 0x23cc, 0x4f67);
  if (!(F & FZ)) { RET_TAKEN(0x4f67); return; } // ret nz
  CYC(0x4f67, 0x4f68);
  CYC(0x4f68, 0x4f69); alu_xor(gb, A);
  CYC(0x4f69, 0x4f6c); SET_HL(wGenericCutscene_cbb3);
  CYC(0x4f6c, 0x4f6d); mem_wr(gb, HL, A);
  CYC(0x4f6d, 0x4f6e); A = alu_dec8(gb, A);
  CYC(0x4f6e, 0x4f71); SET_HL(wGenericCutscene_cbba);
  CYC(0x4f71, 0x4f72); mem_wr(gb, HL, A);
  CYC(0x4f72, 0x4f74); A = 0xd2; // SND_LIGHTNING
  CALL_C(0x4f74, playSound_b00_hook, 0x0c98, 0x4f77);
  CYC(0x4f77, 0x4f7a); interactionIncSubstate_hook(gb); return; // jp

substate1:
  CYC(0x4f7a, 0x4f7d); SET_HL(wGenericCutscene_cbb3);
  CYC(0x4f7d, 0x4f7f); B = 0x01;
  CALL_C(0x4f7f, flashScreen_hook, 0x2d73, 0x4f82);
  if (F & FZ) { RET_TAKEN(0x4f82); return; } // ret z
  CYC(0x4f82, 0x4f83);
  CALL_C(0x4f83, interactionIncSubstate_hook, 0x23e5, 0x4f86);
  CYC(0x4f86, 0x4f89); fadeoutToWhite_hook(gb); return; // jp

substate2:
  CYC(0x4f89, 0x4f8c); A = W8(wPaletteThread_mode);
  CYC(0x4f8c, 0x4f8d); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4f8d); return; } // ret nz
  CYC(0x4f8d, 0x4f8e);
  CYC(0x4f8e, 0x4f8f); push_effect(gb, DE);
  CYC(0x4f8f, 0x4f92); SET_BC(0x0116);
  CALL_C(0x4f92, disableLcdAndLoadRoom_hook, 0x30b0, 0x4f95);
  CALL_C(0x4f95, resetCamera_hook, 0x12ce, 0x4f98);
  CYC(0x4f98, 0x4f9b); SET_HL(0x78b3); // objectData.ambiAndNayruInPostD3Cutscene
  CALL_C(0x4f9b, parseGivenObjectData_b00_hook, 0x3171, 0x4f9e);
  CYC(0x4f9e, 0x4fa0); A = 0x02;
  CALL_C(0x4fa0, loadGfxRegisterStateIndex_hook, 0x02ea, 0x4fa3);
  CYC(0x4fa3, 0x4fa4); SET_DE(pop_effect(gb));
  CYC(0x4fa4, 0x4fa6); A = 0x21; // MUS_DISASTER
  CALL_C(0x4fa6, playSound_b00_hook, 0x0c98, 0x4fa9);
  CYC(0x4fa9, 0x4fac); fadeinFromWhite_hook(gb); return; // jp
}

// interaction6b_subid07: a seed satchel that slowly falls toward Link. Possibly unused.
void interaction6b_subid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fac, checkInteractionState_hook, 0x23fe, 0x4faf);
  if (!(F & FZ)) { CYCT(0x4faf, 0x4fb1); goto state1; } // jr nz
  CYC(0x4faf, 0x4fb1);

  // interaction6b_subid07@state0
  CALL_C(0x4fb1, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x4fb4);
  CYC(0x4fb4, 0x4fb7); SET_BC(0x0000);
  CYC(0x4fb7, 0x4fba); SET_HL(0xd00b); // w1Link.yh
  CALL_C(0x4fba, objectTakePositionWithOffset_hook, 0x2277, 0x4fbd);
  CYC(0x4fbd, 0x4fbe); H = D;
  CYC(0x4fbe, 0x4fc0); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x4fc0, 0x4fc2); mem_wr(gb, HL, 0xa8);

state1:
  CYC(0x4fc2, 0x4fc3); H = D;
  CYC(0x4fc3, 0x4fc5); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x4fc5, 0x4fc6); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x4fc6, 0x4fc8); alu_cp(gb, 0xf4);
  if (!(F & FC)) { CYCT(0x4fc8, 0x4fcb); interactionDelete_hook(gb); return; } // jp nc
  CYC(0x4fc8, 0x4fcb);
  CYC(0x4fcb, 0x4fce); SET_BC(0x0080);
  CYC(0x4fce, 0x4fcf); A = C;
  CYC(0x4fcf, 0x4fd0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4fd0, 0x4fd1); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4fd1, 0x4fd2); A = B;
  CYC(0x4fd2, 0x4fd3); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x4fd3, 0x4fd4); mem_wr(gb, HL, A);
  CYC(0x4fd4, 0x4fd7); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; // jp
}

// interaction6b_subid08: part of the cutscene where tokays steal your stuff.
void interaction6b_subid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fd7, checkInteractionState_hook, 0x23fe, 0x4fda);
  if (!(F & FZ)) { CYCT(0x4fda, 0x4fdc); goto state1; } // jr nz
  CYC(0x4fda, 0x4fdc);

  // interaction6b_subid08@state0
  CALL_C(0x4fdc, getThisRoomFlags_hook, 0x197d, 0x4fdf);
  CYC(0x4fdf, 0x4fe1); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x4fe1, 0x4fe4); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x4fe1, 0x4fe4);
  CYC(0x4fe4, 0x4fe7); interactionIncState_hook(gb); return; // jp

state1:
  CYC(0x4fe7, 0x4fea); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(0x4fea, 0x4feb); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4feb, 0x4fee); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x4feb, 0x4fee);
  CYC(0x4fee, 0x4fef); H = D;
  CYC(0x4fef, 0x4ff1); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4ff1, 0x4ff2); A = mem_rd(gb, HL);
  CYC(0x4ff2, 0x4ff3); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4ff3, 0x4ff6); playWaveSoundAtRandomIntervals_hook(gb); return; } // jp z
  CYC(0x4ff3, 0x4ff6);
  CYC(0x4ff6, 0x4ff7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x4ff7); return; // ret
}

// interaction6b_subid09: the shovel that Rosa gives to you in a linked game.
void interaction6b_subid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ff8, checkInteractionState_hook, 0x23fe, 0x4ffb);
  if (!(F & FZ)) { CYCT(0x4ffb, 0x4ffd); goto state1; } // jr nz
  CYC(0x4ffb, 0x4ffd);

  // interaction6b_subid09@state0
  CALL_C(0x4ffd, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x5000);
  CYC(0x5000, 0x5003); SET_BC(0x3848);
  CALL_C(0x5003, interactionSetPosition_hook, 0x2773, 0x5006);
  CYC(0x5006, 0x5009); objectSetVisible80_hook(gb); return; // jp

state1:
  CYC(0x5009, 0x500b); A = 0x00; // Object.enabled
  CALL_C(0x500b, objectGetRelatedObject1Var_hook, 0x2160, 0x500e);
  CYC(0x500e, 0x5010); L = INTERACTION_BASE + OBJ_VAR3E; // Interaction.var3e
  CYC(0x5010, 0x5011); A = mem_rd(gb, HL);
  CYC(0x5011, 0x5012); C = A;
  CYC(0x5012, 0x5013); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5013); return; } // ret z
  CYC(0x5013, 0x5014);
  CYC(0x5014, 0x5015); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x5015, 0x5018); interactionDelete_hook(gb); return; } // jp z
  CYC(0x5015, 0x5018);
  CYC(0x5018, 0x501a); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x501a, 0x501b); A = mem_rd(gb, HL);
  CYC(0x501b, 0x501c); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x501c, objectSetVisible83_hook, 0x1e72, 0x501f); else CYC(0x501c, 0x501f); // call nz
  CYC(0x501f, 0x5021); B = 0x00;
  CYC(0x5021, 0x5024); objectTakePositionWithOffset_hook(gb); return; // jp
}

void interaction6b_initGraphicsAndLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5252, interactionInitGraphics_hook, 0x15fb, 0x5255);
  interaction6b_loadScript_hook(gb); return; // falls through
}

// interaction6b_subid0a (also interaction6b_subid0b and interaction6b_subid0c, aliases for the
// same code): flippers, cheval rope, and bomb treasures.
void interaction6b_subid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5024, checkInteractionState_hook, 0x23fe, 0x5027);
  if (!(F & FZ)) { CYCT(0x5027, 0x5029); goto state1; } // jr nz
  CYC(0x5027, 0x5029);

  // interaction6b_subid0c@state0
  CALL_C(0x5029, getThisRoomFlags_hook, 0x197d, 0x502c);
  CYC(0x502c, 0x502e); alu_bit(gb, 5, A); // ROOMFLAG_BIT_ITEM
  if (!(F & FZ)) { CYCT(0x502e, 0x5031); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x502e, 0x5031);
  CYC(0x5031, 0x5033); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5033, 0x5034); A = mem_rd(gb, DE);
  CYC(0x5034, 0x5036); alu_sub(gb, 0x0a);
  CYC(0x5036, 0x5037); E = alu_inc8(gb, E);
  CYC(0x5037, 0x5038); mem_wr(gb, DE, A);
  CALL_C(0x5038, interaction6b_initGraphicsAndLoadScript_hook, 0x5252, 0x503b);

state1:
  CALL_C(0x503b, interactionRunScript_hook, 0x2552, 0x503e);
  if (!(F & FC)) { CYCT(0x503e, 0x5040); goto l504a; } // jr nc
  CYC(0x503e, 0x5040);
  CYC(0x5040, 0x5041); alu_xor(gb, A);
  CYC(0x5041, 0x5044); W8(wDisabledObjects) = A;
  CYC(0x5044, 0x5047); W8(wMenuDisabled) = A;
  CYC(0x5047, 0x504a); interactionDelete_hook(gb); return; // jp

l504a:
  CALL_C(0x504a, checkInteractionSubstate_hook, 0x2403, 0x504d);
  if (F & FZ) { CYCT(0x504d, 0x5050); interactionAnimateAsNpc_hook(gb); return; } // jp z
  CYC(0x504d, 0x5050);
  RET(0x5050); return; // ret
}

// interaction6b_subid0d: blocks that move over when pulling the lever to get flippers.
// checkLinkSquished is reached via a conditional "call nc", modeled the same way as an
// unconditional local call but gated on the branch condition, with the not-taken side sharing
// the same CYC end address and falling straight into updateXAndDraw.
void interaction6b_subid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5051, checkInteractionState_hook, 0x23fe, 0x5054);
  if (!(F & FZ)) { CYCT(0x5054, 0x5056); goto state1; } // jr nz
  CYC(0x5054, 0x5056);

  // interaction6b_subid0d@state0
  CALL_C(0x5056, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x5059);
  CYC(0x5059, 0x505b); A = 0xa3; // PALH_a3
  CALL_C(0x505b, loadPaletteHeader_hook, 0x050b, 0x505e);
  CYC(0x505e, 0x5060); A = 0x06;
  CALL_C(0x5060, objectSetCollideRadius_hook, 0x24a1, 0x5063);
  CYC(0x5063, 0x5065); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5065, 0x5066); A = mem_rd(gb, HL);
  CYC(0x5066, 0x5068); alu_cp(gb, 0xc0);
  if (!(F & FZ)) { CYCT(0x5068, 0x506a); goto l506e; } // jr nz
  CYC(0x5068, 0x506a);
  CYC(0x506a, 0x506c); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x506c, 0x506e); mem_wr(gb, HL, 0x01);

l506e:
  CYC(0x506e, 0x5070); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x5070, 0x5071); mem_wr(gb, HL, A);

state1:
  CYC(0x5071, 0x5074); A = W8(w1Link_state);
  CYC(0x5074, 0x5076); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x5076); return; } // ret nz
  CYC(0x5076, 0x5077);
  CYC(0x5077, 0x507a); A = W8(wLever1PullDistance);
  CYC(0x507a, 0x507b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x507b, 0x507d); goto updateXAndDraw; } // jr z
  CYC(0x507b, 0x507d);
  CYC(0x507d, 0x507f); alu_and(gb, 0x7c);
  CYC(0x507f, 0x5080); alu_rrca(gb);
  CYC(0x5080, 0x5081); alu_rrca(gb);
  CYC(0x5081, 0x5082); B = A;
  CYC(0x5082, 0x5084); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5084, 0x5085); A = mem_rd(gb, DE);
  CYC(0x5085, 0x5086); alu_or(gb, A);
  CYC(0x5086, 0x5087); A = B;
  if (!(F & FZ)) { CYCT(0x5087, 0x5089); goto updateXAndDraw; } // jr nz
  CYC(0x5087, 0x5089);
  CYC(0x5089, 0x508a); alu_cpl(gb);
  CYC(0x508a, 0x508b); A = alu_inc8(gb, A);
  CYC(0x508b, 0x508d); alu_cp(gb, 0xfe);
  if (!(F & FC)) { CYCT(0x508d, 0x5090); push_effect(gb, 0x5090); goto checkLinkSquished; } // call nc
  CYC(0x508d, 0x5090);

updateXAndDraw:
  CYC(0x5090, 0x5091); H = D;
  CYC(0x5091, 0x5093); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x5093, 0x5094); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5094, 0x5096); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x5096, 0x5097); mem_wr(gb, HL, A);
  CYC(0x5097, 0x509a); interactionAnimateAsNpc_hook(gb); return; // jp

checkLinkSquished:
  CYC(0x509a, 0x509b); push_effect(gb, AF);
  CYC(0x509b, 0x509e); A = W8(wLinkInAir);
  CYC(0x509e, 0x509f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x509f, 0x50a1); goto ret_; } // jr nz
  CYC(0x509f, 0x50a1);
  CYC(0x50a1, 0x50a3); A = 0x08;
  CYC(0x50a3, 0x50a6); SET_BC(0x38b8);
  CYC(0x50a6, 0x50a9); SET_HL(0xd00b); // w1Link.yh
  CALL_C(0x50a9, checkObjectIsCloseToPosition_b00_hook, 0x3d59, 0x50ac);
  if (!(F & FC)) { CYCT(0x50ac, 0x50ae); goto ret_; } // jr nc
  CYC(0x50ac, 0x50ae);
  CYC(0x50ae, 0x50af); alu_xor(gb, A);
  CYC(0x50af, 0x50b2); W8(wcc50) = A;
  CYC(0x50b2, 0x50b4); A = 0x11; // LINK_STATE_SQUISHED
  CYC(0x50b4, 0x50b7); W8(wLinkForceState) = A;

ret_:
  CYC(0x50b7, 0x50b8); SET_AF(pop_effect(gb));
  RET(0x50b8);
  if (gb->pc == 0x5090 && gb->sp == sp0_) goto updateXAndDraw;
  return; // ret
}

// interaction6b_subid0e: the stone statue of Link that appears unconditionally. state0 and
// state1 are also reached directly (cross-routine) by interaction6b_subid15's own dispatch,
// once that routine is ported.
void interaction6b_subid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50b9, checkInteractionState_hook, 0x23fe, 0x50bc);
  if (!(F & FZ)) { CYCT(0x50bc, 0x50be); goto state1; } // jr nz
  CYC(0x50bc, 0x50be);

  // interaction6b_subid0e@state0
  CYC(0x50be, 0x50c1); A = W8(wTilesetFlags);
  CYC(0x50c1, 0x50c3); alu_and(gb, 0x80); // TILESETFLAG_PAST
  CYC(0x50c3, 0x50c5); A = 0xc7; // PALH_c7
  if (!(F & FZ)) { CYCT(0x50c5, 0x50c7); goto l50c8; } // jr nz
  CYC(0x50c5, 0x50c7);
  CYC(0x50c7, 0x50c8); A = alu_dec8(gb, A);

l50c8:
  CALL_C(0x50c8, loadPaletteHeader_hook, 0x050b, 0x50cb);
  CALL_C(0x50cb, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x50ce);
  CYC(0x50ce, 0x50d1); SET_BC(0x080a);
  CALL_C(0x50d1, objectSetCollideRadii_hook, 0x24a9, 0x50d4);
  CALL_C(0x50d4, objectGetShortPosition_hook, 0x2096, 0x50d7);
  CYC(0x50d7, 0x50d8); C = A;
  CYC(0x50d8, 0x50da); B = 0xcf; // >wRoomLayout
  CYC(0x50da, 0x50db); A = mem_rd(gb, BC);
  CYC(0x50db, 0x50dd); alu_cp(gb, 0xf9);
  CYC(0x50dd, 0x50df); A = 0x04;
  if (!(F & FZ)) { CYCT(0x50df, 0x50e1); goto l50e2; } // jr nz
  CYC(0x50df, 0x50e1);
  CYC(0x50e1, 0x50e2); A = alu_inc8(gb, A);

l50e2:
  CALL_C(0x50e2, interactionSetAnimation_hook, 0x262e, 0x50e5);

state1:
  CALL_C(0x50e5, interactionAnimateAsNpc_hook, 0x26db, 0x50e8);
  CYC(0x50e8, 0x50e9); H = D;
  CYC(0x50e9, 0x50eb); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x50eb, 0x50ed); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x40);
  RET(0x50ed); return; // ret
}

// interaction6b_subid0f: the switch that opens the path to Nuun Highlands.
void interaction6b_subid0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50ee, 0x50f0); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x50f0, 0x50f1); A = mem_rd(gb, DE);
  {
    CYC(0x50f1, 0x50f2); push_effect(gb, 0x50f2);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == 0x50f8) goto state0;
    if (target == 0x510f) goto state1;
    goto state2;
  }

state0:
  CYC(0x50f8, 0x50fa); A = 0x01;
  CYC(0x50fa, 0x50fb); mem_wr(gb, DE, A);
  CALL_C(0x50fb, getThisRoomFlags_hook, 0x197d, 0x50fe);
  CYC(0x50fe, 0x5100); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x5100, 0x5103); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5100, 0x5103);
  CALL_C(0x5103, getFreePartSlot_hook, 0x3e8e, 0x5106);
  if (!(F & FZ)) { RET_TAKEN(0x5106); return; } // ret nz
  CYC(0x5106, 0x5107);
  CYC(0x5107, 0x5109); mem_wr(gb, HL, 0x05); // PART_SWITCH
  CYC(0x5109, 0x510a); L = alu_inc8(gb, L);
  CYC(0x510a, 0x510c); mem_wr(gb, HL, 0x01);
  CYC(0x510c, 0x510f); objectCopyPosition_hook(gb); return; // jp

state1:
  CYC(0x510f, 0x5112); A = W8(wSwitchState);
  CYC(0x5112, 0x5113); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5113); return; } // ret z
  CYC(0x5113, 0x5114);
  CYC(0x5114, 0x5116); A = 0x81;
  CYC(0x5116, 0x5119); W8(wMenuDisabled) = A;
  CYC(0x5119, 0x511c); W8(wDisabledObjects) = A;
  CYC(0x511c, 0x511f); W8(wDisableScreenTransitions) = A;
  CALL_C(0x511f, getThisRoomFlags_hook, 0x197d, 0x5122);
  CYC(0x5122, 0x5124); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CALL_C(0x5124, interactionIncState_hook, 0x23e0, 0x5127);
  CYC(0x5127, 0x512a); SET_HL(0x723f); // mainScripts.interaction6b_bridgeToNuunSimpleScript
  CYC(0x512a, 0x512d); interactionSetSimpleScript_hook(gb); return; // jp

state2:
  CYC(0x512d, 0x512f); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x512f, 0x5130); A = mem_rd(gb, DE);
  CYC(0x5130, 0x5131); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5131, 0x5133); goto l5136; } // jr z
  CYC(0x5131, 0x5133);
  CYC(0x5133, 0x5134); A = alu_dec8(gb, A);
  CYC(0x5134, 0x5135); mem_wr(gb, DE, A);
  RET(0x5135); return; // ret

l5136:
  if (!(F & FZ)) { RET_TAKEN(0x5136); return; } // ret nz
  CYC(0x5136, 0x5137);
  CALL_C(0x5137, interactionRunSimpleScript_hook, 0x3da8, 0x513a);
  if (!(F & FC)) { RET_TAKEN(0x513a); return; } // ret nc
  CYC(0x513a, 0x513b);
  CYC(0x513b, 0x513c); alu_xor(gb, A);
  CYC(0x513c, 0x513f); W8(wMenuDisabled) = A;
  CYC(0x513f, 0x5142); W8(wDisabledObjects) = A;
  CYC(0x5142, 0x5145); W8(wDisableScreenTransitions) = A;
  CYC(0x5145, 0x5148); interactionDelete_hook(gb); return; // jp
}

// interaction6b_subid10: the unfinished stone statue of Link in the credits cutscene.
void interaction6b_subid10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5148, checkInteractionState_hook, 0x23fe, 0x514b);
  if (!(F & FZ)) { CYCT(0x514b, 0x514d); goto state1; } // jr nz
  CYC(0x514b, 0x514d);

  // interaction6b_subid10@state0
  CYC(0x514d, 0x514f); A = 0xc8; // PALH_c8
  CALL_C(0x514f, loadPaletteHeader_hook, 0x050b, 0x5152);
  CALL_C(0x5152, interaction6b_initGraphicsAndLoadScript_hook, 0x5252, 0x5155);
  CYC(0x5155, 0x5158); objectSetVisiblec2_hook(gb); return; // jp

state1:
  CYC(0x5158, 0x515a); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x515a, 0x515b); A = mem_rd(gb, DE);
  {
    CYC(0x515b, 0x515c); push_effect(gb, 0x515c);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == 0x516c) goto substate0;
    if (target == 0x517d) goto substate1;
    if (target == 0x5192) goto substate2;
    if (target == 0x5199) goto substate3;
    if (target == 0x51ae) goto substate4;
    if (target == 0x51b7) goto substate5;
    if (target == 0x51cc) goto substate6;
    goto substate7;
  }

substate0:
  CALL_C(0x516c, interactionRunScript_hook, 0x2552, 0x516f);
  CYC(0x516f, 0x5172); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x5172, 0x5174); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x5174); return; } // ret nz
  CYC(0x5174, 0x5175);
  CALL_C(0x5175, interactionIncSubstate_hook, 0x23e5, 0x5178);
  CYC(0x5178, 0x517a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x517a, 0x517c); mem_wr(gb, HL, 32);
  RET(0x517c); return; // ret

substate1:
  CALL_C(0x517d, interactionDecCounter1_hook, 0x23cc, 0x5180);
  if (!(F & FZ)) { CYCT(0x5180, 0x5182); goto l518a; } // jr nz
  CYC(0x5180, 0x5182);
  CYC(0x5182, 0x5184); A = 0x03;
  CYC(0x5184, 0x5187); W8(wTmpcfc0_genericCutscene_state) = A;
  CYC(0x5187, 0x518a); interactionIncSubstate_hook(gb); return; // jp

l518a:
  CYC(0x518a, 0x518b); A = mem_rd(gb, HL);
  CYC(0x518b, 0x518d); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x518d); return; } // ret nz
  CYC(0x518d, 0x518e);
  CYC(0x518e, 0x5190); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5190, 0x5191); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x5191); return; // ret

substate2:
  CALL_C(0x5192, interactionRunScript_hook, 0x2552, 0x5195);
  if (!(F & FC)) { RET_TAKEN(0x5195); return; } // ret nc
  CYC(0x5195, 0x5196);
  CYC(0x5196, 0x5199); interactionIncSubstate_hook(gb); return; // jp

substate3:
  CALL_C(0x5199, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x519c);
  CALL_C(0x519c, objectApplySpeed_hook, 0x201d, 0x519f);
  CYC(0x519f, 0x51a2); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x51a2, 0x51a4); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(0x51a4); return; } // ret nz
  CYC(0x51a4, 0x51a5);
  CALL_C(0x51a5, interactionIncSubstate_hook, 0x23e5, 0x51a8);
  CYC(0x51a8, 0x51ab); SET_BC(0x4084);
  CYC(0x51ab, 0x51ae); interactionSetPosition_hook(gb); return; // jp

substate4:
  CYC(0x51ae, 0x51b1); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x51b1, 0x51b3); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x51b3); return; } // ret nz
  CYC(0x51b3, 0x51b4);
  CYC(0x51b4, 0x51b7); interactionIncSubstate_hook(gb); return; // jp

substate5:
  CYC(0x51b7, 0x51b9); C = 0x01;
  CALL_C(0x51b9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x51bc);
  if (!(F & FZ)) { RET_TAKEN(0x51bc); return; } // ret nz
  CYC(0x51bc, 0x51bd);
  CALL_C(0x51bd, interactionIncSubstate_hook, 0x23e5, 0x51c0);
  CYC(0x51c0, 0x51c2); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x51c2, 0x51c4); mem_wr(gb, HL, 30);
  CALL_C(0x51c4, objectSetVisible82_hook, 0x1e69, 0x51c7);
  CYC(0x51c7, 0x51c9); A = 0x05;
  CYC(0x51c9, 0x51cc); interactionSetAnimation_hook(gb); return; // jp

substate6:
  CALL_C(0x51cc, interactionDecCounter1_hook, 0x23cc, 0x51cf);
  if (!(F & FZ)) { CYCT(0x51cf, 0x51d1); goto l51d8; } // jr nz
  CYC(0x51cf, 0x51d1);
  CYC(0x51d1, 0x51d2); alu_xor(gb, A);
  CYC(0x51d2, 0x51d5); W8(wGfxRegs1_SCY) = A;
  CYC(0x51d5, 0x51d8); interactionIncSubstate_hook(gb); return; // jp

l51d8:
  CYC(0x51d8, 0x51d9); A = mem_rd(gb, HL);
  CYC(0x51d9, 0x51db); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x51db, 0x51dd); goto l51df; } // jr nz
  CYC(0x51db, 0x51dd);
  CYC(0x51dd, 0x51df); A = 0xff;

l51df:
  CYC(0x51df, 0x51e2); W8(wGfxRegs1_SCY) = A;

substate7:
  RET(0x51e2); return; // ret
}

// interaction6b_subid11: triggers the cutscene after beating Jabu-Jabu. Runs unconditionally
// every frame (no checkInteractionState dispatch) until it deletes itself.
void interaction6b_subid11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51e3, 0x51e6); A = W8(wEssencesObtained);
  CYC(0x51e6, 0x51e8); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(0x51e8, 0x51ea); goto delete_; } // jr z
  CYC(0x51e8, 0x51ea);
  CALL_C(0x51ea, getThisRoomFlags_hook, 0x197d, 0x51ed);
  CYC(0x51ed, 0x51ef); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x51ef, 0x51f1); goto delete_; } // jr nz
  CYC(0x51ef, 0x51f1);
  CYC(0x51f1, 0x51f3); A = 0x01;
  CYC(0x51f3, 0x51f6); W8(wDisabledObjects) = A;
  CYC(0x51f6, 0x51f9); W8(wMenuDisabled) = A;
  CYC(0x51f9, 0x51fb); A = 0x21; // CUTSCENE_BLACK_TOWER_COMPLETE
  CYC(0x51fb, 0x51fe); W8(wCutsceneTrigger) = A;

delete_:
  CYC(0x51fe, 0x5201); interactionDelete_hook(gb); return; // jp
}

// interaction6b_subid13 (also interaction6b_subid14, an alias for the same code): the Goron
// bomb statue (left/right).
void interaction6b_subid13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5201, checkInteractionState_hook, 0x23fe, 0x5204);
  if (!(F & FZ)) { CYCT(0x5204, 0x5206); goto state1; } // jr nz
  CYC(0x5204, 0x5206);

  // interaction6b_subid14@state0
  CALL_C(0x5206, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x5209);
  CALL_C(0x5209, objectGetShortPosition_hook, 0x2096, 0x520c);
  CYC(0x520c, 0x520d); C = A;
  CYC(0x520d, 0x520f); B = 0xcf; // >wRoomLayout
  CYC(0x520f, 0x5211); A = 0x00;
  CYC(0x5211, 0x5212); mem_wr(gb, BC, A);
  CYC(0x5212, 0x5214); B = 0xce; // >wRoomCollisions
  CYC(0x5214, 0x5216); A = 0x0f;
  CYC(0x5216, 0x5217); mem_wr(gb, BC, A);

state1:
  CYC(0x5217, 0x521a); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; // jp
}

// interaction6b_subid15: the stone statue of Link as seen in-game. Both of its exits are tail
// jumps into interaction6b_subid0e's own inline state0/state1 code (0x50be/0x50e5) -- addresses
// that live inside a different top-level hook's body, not independently callable -- so that
// entire body is replicated here byte-for-byte (same addresses, same CYC ranges), exactly as
// interaction6b_subid05 already does for a one-instruction case earlier in this file.
void interaction6b_subid15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x521a, checkInteractionState_hook, 0x23fe, 0x521d);
  if (!(F & FZ)) { CYCT(0x521d, 0x5220); goto subid0e_state1; } // jp nz
  CYC(0x521d, 0x5220);

  // interaction6b_subid15@state0
  CYC(0x5220, 0x5222); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x5222, checkGlobalFlag_hook, 0x31f3, 0x5225);
  if (F & FZ) { CYCT(0x5225, 0x5228); interactionDelete_hook(gb); return; } // jp z
  CYC(0x5225, 0x5228);
  CALL_C(0x5228, objectGetShortPosition_hook, 0x2096, 0x522b);
  CYC(0x522b, 0x522d); H = 0xce; // >wRoomCollisions
  CYC(0x522d, 0x522e); L = A;
  CYC(0x522e, 0x5230); mem_wr(gb, HL, 0x0f);
  CYC(0x5230, 0x5233); goto subid0e_state0; // jp

subid0e_state0:
  CYC(0x50be, 0x50c1); A = W8(wTilesetFlags);
  CYC(0x50c1, 0x50c3); alu_and(gb, 0x80); // TILESETFLAG_PAST
  CYC(0x50c3, 0x50c5); A = 0xc7; // PALH_c7
  if (!(F & FZ)) { CYCT(0x50c5, 0x50c7); goto subid0e_l50c8; } // jr nz
  CYC(0x50c5, 0x50c7);
  CYC(0x50c7, 0x50c8); A = alu_dec8(gb, A);

subid0e_l50c8:
  CALL_C(0x50c8, loadPaletteHeader_hook, 0x050b, 0x50cb);
  CALL_C(0x50cb, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x50ce);
  CYC(0x50ce, 0x50d1); SET_BC(0x080a);
  CALL_C(0x50d1, objectSetCollideRadii_hook, 0x24a9, 0x50d4);
  CALL_C(0x50d4, objectGetShortPosition_hook, 0x2096, 0x50d7);
  CYC(0x50d7, 0x50d8); C = A;
  CYC(0x50d8, 0x50da); B = 0xcf; // >wRoomLayout
  CYC(0x50da, 0x50db); A = mem_rd(gb, BC);
  CYC(0x50db, 0x50dd); alu_cp(gb, 0xf9);
  CYC(0x50dd, 0x50df); A = 0x04;
  if (!(F & FZ)) { CYCT(0x50df, 0x50e1); goto subid0e_l50e2; } // jr nz
  CYC(0x50df, 0x50e1);
  CYC(0x50e1, 0x50e2); A = alu_inc8(gb, A);

subid0e_l50e2:
  CALL_C(0x50e2, interactionSetAnimation_hook, 0x262e, 0x50e5);

subid0e_state1:
  CALL_C(0x50e5, interactionAnimateAsNpc_hook, 0x26db, 0x50e8);
  CYC(0x50e8, 0x50e9); H = D;
  CYC(0x50e9, 0x50eb); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x50eb, 0x50ed); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x40);
  RET(0x50ed); return; // ret
}

// interaction6b_subid16: a flame that appears for [counter1] frames.
void interaction6b_subid16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5233, checkInteractionState_hook, 0x23fe, 0x5236);
  if (!(F & FZ)) { CYCT(0x5236, 0x5238); goto state1; } // jr nz
  CYC(0x5236, 0x5238);

  // interaction6b_subid16@state0
  CALL_C(0x5238, interaction6b_initGraphicsAndIncState_hook, 0x524c, 0x523b);
  CALL_C(0x523b, objectSetVisible81_hook, 0x1e60, 0x523e);
  CYC(0x523e, 0x5240); A = 0x72; // SND_LIGHTTORCH
  CYC(0x5240, 0x5243); playSound_b00_hook(gb); return; // jp

state1:
  CALL_C(0x5243, interactionDecCounter1_hook, 0x23cc, 0x5246);
  if (F & FZ) { CYCT(0x5246, 0x5249); interactionDelete_hook(gb); return; } // jp z
  CYC(0x5246, 0x5249);
  CYC(0x5249, 0x524c); interactionAnimate_hook(gb); return; // jp
}

// INTERAC_MISCELLANEOUS_1
void interactionCode6b_hook(GB *gb) {
  CYC(0x4df4, 0x4df6); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4df6, 0x4df7); A = mem_rd(gb, DE);
  {
    CYC(0x4df7, 0x4df8); push_effect(gb, 0x4df8);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == 0x4e26) { interaction6b_subid00_hook(gb); return; }
    if (target == 0x4e7c) { interaction6b_subid01_hook(gb); return; }
    if (target == 0x4eaa) { interaction6b_subid02_hook(gb); return; }
    if (target == 0x4ec1) { interaction6b_subid03_hook(gb); return; } // also subid12
    if (target == 0x4ecd) { interaction6b_subid04_hook(gb); return; }
    if (target == 0x4ee0) { interaction6b_subid05_hook(gb); return; }
    if (target == 0x4f33) { interaction6b_subid06_hook(gb); return; }
    if (target == 0x4fac) { interaction6b_subid07_hook(gb); return; }
    if (target == 0x4fd7) { interaction6b_subid08_hook(gb); return; }
    if (target == 0x4ff8) { interaction6b_subid09_hook(gb); return; }
    if (target == 0x5024) { interaction6b_subid0a_hook(gb); return; } // also subid0b, subid0c
    if (target == 0x5051) { interaction6b_subid0d_hook(gb); return; }
    if (target == 0x50b9) { interaction6b_subid0e_hook(gb); return; }
    if (target == 0x50ee) { interaction6b_subid0f_hook(gb); return; }
    if (target == 0x5148) { interaction6b_subid10_hook(gb); return; }
    if (target == 0x51e3) { interaction6b_subid11_hook(gb); return; }
    if (target == 0x5201) { interaction6b_subid13_hook(gb); return; } // also subid14
    if (target == 0x521a) { interaction6b_subid15_hook(gb); return; }
    interaction6b_subid16_hook(gb); return; // target == 0x5233
  }
}
