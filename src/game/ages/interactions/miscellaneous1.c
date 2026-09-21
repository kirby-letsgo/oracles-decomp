#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// interaction6b_subid00: handles showing Impa's "Help" text when Link's about to screen
// transition. decCounter1IfTextNotActive is called once from @substate1 (bare sp0_) and its own
// two exits (the early "ret nz" and the tail-jump into interactionDecCounter1_hook) both resume
// there, since substate1's own "ret nz" then re-checks the very same flags decCounter1IfTextNot-
// Active left behind.
void interaction6b_subid00_hook(GB *gb) {
  BASE(interaction6b_subid00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid00@state0
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+13, b_+16);

state1:
  CALL_C(b_+16, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+19);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto substate1; } // jr nz
  CYC(b_+19, b_+21);

  // interaction6b_subid00@substate0
  CALL_C(b_+21, interaction6b_checkLinkPressedUpAtScreenEdge_hook, SYM(interaction6b_checkLinkPressedUpAtScreenEdge), b_+24);
  if (F & FZ) { RET_TAKEN(b_+24); return; } // ret z
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); A = 0x01;
  CYC(b_+27, b_+30); W8(wMenuDisabled) = A;
  CYC(b_+30, b_+33); W8(wDisabledObjects) = A;
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+35, b_+37); A = 30;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+41); SET_BC(0x0100); // TX_0100
  CALL_C(b_+41, showText_hook, SYM(showText), b_+44);
  CYC(b_+44, b_+47); TAIL(interactionIncSubstate); // jp

substate1:
  CYC(b_+47, b_+50); push_effect(gb, b_+50); goto decCounter1IfTextNotActive;
afterDecCounter1IfTextNotActive:
  if (!(F & FZ)) { RET_TAKEN(b_+50); return; } // ret nz
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CYC(b_+52, b_+55); W8(wDisabledObjects) = A;
  CYC(b_+55, b_+56); push_effect(gb, DE);
  CYC(b_+56, b_+59); SET_HL(b_+81); // interaction6b_subid00@simulatedInput
  CYC(b_+59, b_+61); A = 0x0a; // bank of interaction6b_subid00@simulatedInput
  CALL_C(b_+61, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+64);
  CYC(b_+64, b_+65); SET_DE(pop_effect(gb));
  CALL_C(b_+65, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+70, b_+73); TAIL(interactionDelete); // jp

decCounter1IfTextNotActive:
  CYC(b_+73, b_+76); A = W8(wTextIsActive);
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(b_+77);
    if (gb->pc == b_+50 && gb->sp == sp0_) goto afterDecCounter1IfTextNotActive;
    return;
  } // ret nz
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+81); interactionDecCounter1_hook(gb);
  if (gb->pc == b_+50 && gb->sp == sp0_) goto afterDecCounter1IfTextNotActive;
  return; // jp
}

// @param[out] zflag  nz if Link pressed up at screen edge
void interaction6b_checkLinkPressedUpAtScreenEdge_hook(GB *gb) {
  BASE(interaction6b_checkLinkPressedUpAtScreenEdge);
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  CYC(b_+3, b_+5); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto l5274; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(w1Link_yh); // w1Link.yh
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(b_+13, b_+15); goto l5276; } // jr c
  CYC(b_+13, b_+15);

l5274:
  CYC(b_+15, b_+16); alu_xor(gb, A);
  RET(b_+16); return; // ret

l5276:
  CYC(b_+17, b_+20); A = W8(wKeysPressed);
  CYC(b_+20, b_+22); alu_and(gb, 0x40); // BTN_UP
  RET(b_+22); return; // ret
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
  BASE(interaction6b_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x0a; // GLOBALFLAG_INTRO_DONE
  CALL_C(b_+13, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto delete_; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); SET_HL((SYM(interactionCode94) + 13)); // objectData.nayruAndAnimalsInIntro
  CALL_C(b_+21, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+24);
  CYC(b_+24, b_+26); A = 0x36; // INTERAC_NAYRU
  CYC(b_+26, b_+29); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(b_+29, b_+30); push_effect(gb, DE);
  CYC(b_+30, b_+32); A = 0x3a; // UNCMP_GFXH_AGES_IMPA_FAINTED
  CALL_C(b_+32, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+35);
  CYC(b_+35, b_+36); SET_DE(pop_effect(gb));

delete_:
  CYC(b_+36, b_+39); TAIL(interactionDelete); // jp

state1:
  // Never executed (deletes self before running state 1).
  CALL_C(b_+39, interactionRunScript_hook, SYM(interactionRunScript), b_+42);
  if (F & FC) { CYCT(b_+42, b_+45); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+42, b_+45);
  RET(b_+45); return; // ret
}

// interaction6b_subid02: script for the cutscene with Ralph outside Ambi's palace, before
// getting the mystery seeds.
void interaction6b_subid02_hook(GB *gb) {
  BASE(interaction6b_subid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid02@state0
  CYC(b_+5, b_+7); A = 0x24; // TREASURE_MYSTERY_SEEDS
  CALL_C(b_+7, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+10);
  if (F & FC) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); TAIL(interaction6b_loadScript); // jp

state1:
  CALL_C(b_+16, interactionRunScript_hook, SYM(interactionRunScript), b_+19);
  if (F & FC) { CYCT(b_+19, b_+22); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+19, b_+22);
  RET(b_+22); return; // ret
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
  BASE(interaction6b_loadScript);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(interaction6b_scriptTable)); // interaction6b_scriptTable
  CYC(b_+6, b_+7); interaction6b_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CALL_C(b_+10, interactionSetScript_hook, SYM(interactionSetScript), b_+13);
  CYC(b_+13, b_+16); TAIL(interactionIncState); // jp
}

void interaction6b_initGraphicsAndIncState_hook(GB *gb) {
  BASE(interaction6b_initGraphicsAndIncState);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionIncState); // jp
}

// interaction6b_subid03 (also interaction6b_subid12, an alias for the same code): the Seasons
// troupe member with a guitar or tambourine.
void interaction6b_subid03_hook(GB *gb) {
  BASE(interaction6b_subid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); interactionAnimate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);

  // interaction6b_subid12@state0
  CALL_C(b_+6, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+9);
  CYC(b_+9, b_+12); TAIL(objectSetVisible82); // jp
}

// interaction6b_subid04: script for the cutscene where moblins attack the maku sapling. Reaches
// interaction6b_loadScript via a genuine call (unlike subid02's tail-jump), so it falls back
// through to state1 normally -- CALL_C's own return-address/stack check handles that transparently
// regardless of what loadScript's own internal tail-jump chain did.
void interaction6b_subid04_hook(GB *gb) {
  BASE(interaction6b_subid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid04@state0
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); W8(wccd4) = A;
  CALL_C(b_+9, interaction6b_loadScript_hook, SYM(interaction6b_loadScript), b_+12);

state1:
  CALL_C(b_+12, interactionRunScript_hook, SYM(interactionRunScript), b_+15);
  if (F & FC) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+15, b_+18);
  RET(b_+18); return; // ret
}

// interaction6b_subid05: the intro cutscene where lightning strikes a guy. State 0's jump-table
// entry lands on interaction6b_subid02's own @loadScript address (0x4eb7) -- the exact same
// tail-jump into interaction6b_loadScript_hook that subid02_hook itself already models at that
// address, reached here through a completely different top-level routine's dispatch.
void interaction6b_subid05_hook(GB *gb) {
  BASE(interaction6b_subid05);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == SYM(interaction6b_subid02__loadScript)) { CYC(SYM(interaction6b_subid02__loadScript), (SYM(interaction6b_subid02__loadScript) + 3)); interaction6b_loadScript_hook(gb); return; }
    goto state1;
  }

state1:
  CALL_C(b_+8, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto substate1; } // jr nz
  CYC(b_+11, b_+13);

  // interaction6b_subid05@substate0
  CALL_C(b_+13, interactionRunScript_hook, SYM(interactionRunScript), b_+16);
  if (!(F & FC)) { RET_TAKEN(b_+16); return; } // ret nc
  CYC(b_+16, b_+17);
  CALL_C(b_+17, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+20);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x01);
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x00);

substate1:
  CALL_C(b_+29, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+32);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; } // ret nz
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x14);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+39); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+39, b_+42); goto l4f12; } // jp nz
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); A = 0x03;
  CYC(b_+44, b_+47); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(b_+47, b_+50); TAIL(interactionDelete); // jp

l4f12:
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+51, b_+54); SET_HL(b_+75); // interaction6b_subid05@lightningPositions
  CYC(b_+54, b_+55); interaction6b_addDoubleIndexToHl_from_rst(gb, b_+55);
  CYC(b_+55, b_+56); B = mem_rd(gb, HL);
  CYC(b_+56, b_+57); SET_HL(HL + 1);
  CYC(b_+57, b_+58); C = mem_rd(gb, HL);
  CALL_C(b_+58, getFreePartSlot_hook, SYM(getFreePartSlot), b_+61);
  if (!(F & FZ)) { RET_TAKEN(b_+61); return; } // ret nz
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(b_+64, b_+65); L = alu_inc8(gb, L);
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+66, b_+67); L = alu_inc8(gb, L);
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+68, b_+70); L = PART_BASE + OBJ_YH;
  CYC(b_+70, b_+71); mem_wr(gb, HL, B);
  CYC(b_+71, b_+73); L = PART_BASE + OBJ_XH;
  CYC(b_+73, b_+74); mem_wr(gb, HL, C);
  RET(b_+74); return; // ret
}

// interaction6b_subid06: manages the cutscene after beating Dungeon 3.
void interaction6b_subid06_hook(GB *gb) {
  BASE(interaction6b_subid06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid06@state0
  CYC(b_+5, b_+8); A = W8(wEssencesObtained);
  CYC(b_+8, b_+10); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto delete_; } // jr z
  CYC(b_+10, b_+12);
  CALL_C(b_+12, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); goto delete_; } // jp nz
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+25); W8(wDisabledObjects) = A;
  CYC(b_+25, b_+28); W8(wMenuDisabled) = A;
  CALL_C(b_+28, interactionIncState_hook, SYM(interactionIncState), b_+31);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 90);
  RET(b_+35); return; // ret

delete_:
  CYC(b_+36, b_+39); TAIL(interactionDelete); // jp

state1:
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  {
    CYC(b_+42, b_+43); push_effect(gb, b_+43);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == b_+49) goto substate0;
    if (target == b_+71) goto substate1;
    goto substate2;
  }

substate0:
  CALL_C(b_+49, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+52);
  if (!(F & FZ)) { RET_TAKEN(b_+52); return; } // ret nz
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+54); alu_xor(gb, A);
  CYC(b_+54, b_+57); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+59); A = alu_dec8(gb, A);
  CYC(b_+59, b_+62); SET_HL(wGenericCutscene_cbba);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+65); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+65, playSound_b00_hook, SYM(playSound_b00), b_+68);
  CYC(b_+68, b_+71); TAIL(interactionIncSubstate); // jp

substate1:
  CYC(b_+71, b_+74); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+74, b_+76); B = 0x01;
  CALL_C(b_+76, flashScreen_hook, SYM(flashScreen), b_+79);
  if (F & FZ) { RET_TAKEN(b_+79); return; } // ret z
  CYC(b_+79, b_+80);
  CALL_C(b_+80, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+83);
  CYC(b_+83, b_+86); TAIL(fadeoutToWhite); // jp

substate2:
  CYC(b_+86, b_+89); A = W8(wPaletteThread_mode);
  CYC(b_+89, b_+90); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; } // ret nz
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+92); push_effect(gb, DE);
  CYC(b_+92, b_+95); SET_BC(0x0116);
  CALL_C(b_+95, disableLcdAndLoadRoom_hook, SYM(disableLcdAndLoadRoom), b_+98);
  CALL_C(b_+98, resetCamera_hook, SYM(resetCamera), b_+101);
  CYC(b_+101, b_+104); SET_HL((SYM(patch_subid00__state0) + 21)); // objectData.ambiAndNayruInPostD3Cutscene
  CALL_C(b_+104, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+107);
  CYC(b_+107, b_+109); A = 0x02;
  CALL_C(b_+109, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+112);
  CYC(b_+112, b_+113); SET_DE(pop_effect(gb));
  CYC(b_+113, b_+115); A = 0x21; // MUS_DISASTER
  CALL_C(b_+115, playSound_b00_hook, SYM(playSound_b00), b_+118);
  CYC(b_+118, b_+121); TAIL(fadeinFromWhite); // jp
}

// interaction6b_subid07: a seed satchel that slowly falls toward Link. Possibly unused.
void interaction6b_subid07_hook(GB *gb) {
  BASE(interaction6b_subid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid07@state0
  CALL_C(b_+5, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+8);
  CYC(b_+8, b_+11); SET_BC(0x0000);
  CYC(b_+11, b_+14); SET_HL(w1Link_yh); // w1Link.yh
  CALL_C(b_+14, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xa8);

state1:
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+26, b_+28); alu_cp(gb, 0xf4);
  if (!(F & FC)) { CYCT(b_+28, b_+31); interactionDelete_hook(gb); return; } // jp nc
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+34); SET_BC(0x0080);
  CYC(b_+34, b_+35); A = C;
  CYC(b_+35, b_+36); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+37, b_+38); A = B;
  CYC(b_+38, b_+39); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+43); TAIL(objectSetPriorityRelativeToLink_withTerrainEffects); // jp
}

// interaction6b_subid08: part of the cutscene where tokays steal your stuff.
void interaction6b_subid08_hook(GB *gb) {
  BASE(interaction6b_subid08);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid08@state0
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); TAIL(interactionIncState); // jp

state1:
  CYC(b_+16, b_+19); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+31); playWaveSoundAtRandomIntervals_hook(gb); return; } // jp z
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+32); return; // ret
}

// interaction6b_subid09: the shovel that Rosa gives to you in a linked game.
void interaction6b_subid09_hook(GB *gb) {
  BASE(interaction6b_subid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid09@state0
  CALL_C(b_+5, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+8);
  CYC(b_+8, b_+11); SET_BC(0x3848);
  CALL_C(b_+11, interactionSetPosition_hook, SYM(interactionSetPosition), b_+14);
  CYC(b_+14, b_+17); TAIL(objectSetVisible80); // jp

state1:
  CYC(b_+17, b_+19); A = 0x00; // Object.enabled
  CALL_C(b_+19, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+22);
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_VAR3E; // Interaction.var3e
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); C = A;
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+27); return; } // ret z
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+29, b_+32); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+36, objectSetVisible83_hook, SYM(objectSetVisible83), b_+39); else CYC(b_+36, b_+39); // call nz
  CYC(b_+39, b_+41); B = 0x00;
  CYC(b_+41, b_+44); TAIL(objectTakePositionWithOffset); // jp
}

void interaction6b_initGraphicsAndLoadScript_hook(GB *gb) {
  BASE(interaction6b_initGraphicsAndLoadScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), SYM(interaction6b_loadScript));
  TAIL(interaction6b_loadScript); // falls through
}

// interaction6b_subid0a (also interaction6b_subid0b and interaction6b_subid0c, aliases for the
// same code): flippers, cheval rope, and bomb treasures.
void interaction6b_subid0a_hook(GB *gb) {
  BASE(interaction6b_subid0a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid0c@state0
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 5, A); // ROOMFLAG_BIT_ITEM
  if (!(F & FZ)) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_sub(gb, 0x0a);
  CYC(b_+18, b_+19); E = alu_inc8(gb, E);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, interaction6b_initGraphicsAndLoadScript_hook, SYM(interaction6b_initGraphicsAndLoadScript), b_+23);

state1:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto l504a; } // jr nc
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); alu_xor(gb, A);
  CYC(b_+29, b_+32); W8(wDisabledObjects) = A;
  CYC(b_+32, b_+35); W8(wMenuDisabled) = A;
  CYC(b_+35, b_+38); TAIL(interactionDelete); // jp

l504a:
  CALL_C(b_+38, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+41);
  if (F & FZ) { CYCT(b_+41, b_+44); interactionAnimateAsNpc_hook(gb); return; } // jp z
  CYC(b_+41, b_+44);
  RET(b_+44); return; // ret
}

// interaction6b_subid0d: blocks that move over when pulling the lever to get flippers.
// checkLinkSquished is reached via a conditional "call nc", modeled the same way as an
// unconditional local call but gated on the branch condition, with the not-taken side sharing
// the same CYC end address and falling straight into updateXAndDraw.
void interaction6b_subid0d_hook(GB *gb) {
  BASE(interaction6b_subid0d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid0d@state0
  CALL_C(b_+5, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+8);
  CYC(b_+8, b_+10); A = 0xa3; // PALH_a3
  CALL_C(b_+10, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+13);
  CYC(b_+13, b_+15); A = 0x06;
  CALL_C(b_+15, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+18);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0xc0);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto l506e; } // jr nz
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x01);

l506e:
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);

state1:
  CYC(b_+32, b_+35); A = W8(w1Link_state);
  CYC(b_+35, b_+37); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); A = W8(wLever1PullDistance);
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+42, b_+44); goto updateXAndDraw; } // jr z
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); alu_and(gb, 0x7c);
  CYC(b_+46, b_+47); alu_rrca(gb);
  CYC(b_+47, b_+48); alu_rrca(gb);
  CYC(b_+48, b_+49); B = A;
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); alu_or(gb, A);
  CYC(b_+53, b_+54); A = B;
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto updateXAndDraw; } // jr nz
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); alu_cpl(gb);
  CYC(b_+57, b_+58); A = alu_inc8(gb, A);
  CYC(b_+58, b_+60); alu_cp(gb, 0xfe);
  if (!(F & FC)) { CYCT(b_+60, b_+63); push_effect(gb, b_+63); goto checkLinkSquished; } // call nc
  CYC(b_+60, b_+63);

updateXAndDraw:
  CYC(b_+63, b_+64); H = D;
  CYC(b_+64, b_+66); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+66, b_+67); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+69, b_+70); mem_wr(gb, HL, A);
  CYC(b_+70, b_+73); TAIL(interactionAnimateAsNpc); // jp

checkLinkSquished:
  CYC(b_+73, b_+74); push_effect(gb, AF);
  CYC(b_+74, b_+77); A = W8(wLinkInAir);
  CYC(b_+77, b_+78); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+78, b_+80); goto ret_; } // jr nz
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); A = 0x08;
  CYC(b_+82, b_+85); SET_BC(0x38b8);
  CYC(b_+85, b_+88); SET_HL(w1Link_yh); // w1Link.yh
  CALL_C(b_+88, checkObjectIsCloseToPosition_b00_hook, SYM(checkObjectIsCloseToPosition_b00), b_+91);
  if (!(F & FC)) { CYCT(b_+91, b_+93); goto ret_; } // jr nc
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, b_+97); W8(wcc50) = A;
  CYC(b_+97, b_+99); A = 0x11; // LINK_STATE_SQUISHED
  CYC(b_+99, b_+102); W8(wLinkForceState) = A;

ret_:
  CYC(b_+102, b_+103); SET_AF(pop_effect(gb));
  RET(b_+103);
  if (gb->pc == b_+63 && gb->sp == sp0_) goto updateXAndDraw;
  return; // ret
}

// interaction6b_subid0e: the stone statue of Link that appears unconditionally. state0 and
// state1 are also reached directly (cross-routine) by interaction6b_subid15's own dispatch,
// once that routine is ported.
void interaction6b_subid0e_hook(GB *gb) {
  BASE(interaction6b_subid0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid0e@state0
  CYC(b_+5, b_+8); A = W8(wTilesetFlags);
  CYC(b_+8, b_+10); alu_and(gb, 0x80); // TILESETFLAG_PAST
  CYC(b_+10, b_+12); A = 0xc7; // PALH_c7
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto l50c8; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);

l50c8:
  CALL_C(b_+15, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+18);
  CALL_C(b_+18, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+21);
  CYC(b_+21, b_+24); SET_BC(0x080a);
  CALL_C(b_+24, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+27);
  CALL_C(b_+27, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+30);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+33); B = 0xcf; // >wRoomLayout
  CYC(b_+33, b_+34); A = mem_rd(gb, BC);
  CYC(b_+34, b_+36); alu_cp(gb, 0xf9);
  CYC(b_+36, b_+38); A = 0x04;
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto l50e2; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); A = alu_inc8(gb, A);

l50e2:
  CALL_C(b_+41, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+44);

state1:
  CALL_C(b_+44, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+47);
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+50, b_+52); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x40);
  RET(b_+52); return; // ret
}

// interaction6b_subid0f: the switch that opens the path to Nuun Highlands.
void interaction6b_subid0f_hook(GB *gb) {
  BASE(interaction6b_subid0f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == b_+10) goto state0;
    if (target == b_+33) goto state1;
    goto state2;
  }

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+18, b_+21);
  CALL_C(b_+21, getFreePartSlot_hook, SYM(getFreePartSlot), b_+24);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x05); // PART_SWITCH
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x01);
  CYC(b_+30, b_+33); TAIL(objectCopyPosition); // jp

state1:
  CYC(b_+33, b_+36); A = W8(wSwitchState);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+37); return; } // ret z
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); A = 0x81;
  CYC(b_+40, b_+43); W8(wMenuDisabled) = A;
  CYC(b_+43, b_+46); W8(wDisabledObjects) = A;
  CYC(b_+46, b_+49); W8(wDisableScreenTransitions) = A;
  CALL_C(b_+49, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+52);
  CYC(b_+52, b_+54); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CALL_C(b_+54, interactionIncState_hook, SYM(interactionIncState), b_+57);
  CYC(b_+57, b_+60); SET_HL((SYM(miscPuzzles_subid19__state1) + 2)); // mainScripts.interaction6b_bridgeToNuunSimpleScript
  CYC(b_+60, b_+63); TAIL(interactionSetSimpleScript); // jp

state2:
  CYC(b_+63, b_+65); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+67, b_+69); goto l5136; } // jr z
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+70); A = alu_dec8(gb, A);
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  RET(b_+71); return; // ret

l5136:
  if (!(F & FZ)) { RET_TAKEN(b_+72); return; } // ret nz
  CYC(b_+72, b_+73);
  CALL_C(b_+73, interactionRunSimpleScript_hook, SYM(interactionRunSimpleScript), b_+76);
  if (!(F & FC)) { RET_TAKEN(b_+76); return; } // ret nc
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+78); alu_xor(gb, A);
  CYC(b_+78, b_+81); W8(wMenuDisabled) = A;
  CYC(b_+81, b_+84); W8(wDisabledObjects) = A;
  CYC(b_+84, b_+87); W8(wDisableScreenTransitions) = A;
  CYC(b_+87, b_+90); TAIL(interactionDelete); // jp
}

// interaction6b_subid10: the unfinished stone statue of Link in the credits cutscene.
void interaction6b_subid10_hook(GB *gb) {
  BASE(interaction6b_subid10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid10@state0
  CYC(b_+5, b_+7); A = 0xc8; // PALH_c8
  CALL_C(b_+7, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+10);
  CALL_C(b_+10, interaction6b_initGraphicsAndLoadScript_hook, SYM(interaction6b_initGraphicsAndLoadScript), b_+13);
  CYC(b_+13, b_+16); TAIL(objectSetVisiblec2); // jp

state1:
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == b_+36) goto substate0;
    if (target == b_+53) goto substate1;
    if (target == b_+74) goto substate2;
    if (target == b_+81) goto substate3;
    if (target == b_+102) goto substate4;
    if (target == b_+111) goto substate5;
    if (target == b_+132) goto substate6;
    goto substate7;
  }

substate0:
  CALL_C(b_+36, interactionRunScript_hook, SYM(interactionRunScript), b_+39);
  CYC(b_+39, b_+42); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+42, b_+44); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+44); return; } // ret nz
  CYC(b_+44, b_+45);
  CALL_C(b_+45, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+48);
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 32);
  RET(b_+52); return; // ret

substate1:
  CALL_C(b_+53, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto l518a; } // jr nz
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); A = 0x03;
  CYC(b_+60, b_+63); W8(wTmpcfc0_genericCutscene_state) = A;
  CYC(b_+63, b_+66); TAIL(interactionIncSubstate); // jp

l518a:
  CYC(b_+66, b_+67); A = mem_rd(gb, HL);
  CYC(b_+67, b_+69); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; } // ret nz
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+72, b_+73); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+73); return; // ret

substate2:
  CALL_C(b_+74, interactionRunScript_hook, SYM(interactionRunScript), b_+77);
  if (!(F & FC)) { RET_TAKEN(b_+77); return; } // ret nc
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+81); TAIL(interactionIncSubstate); // jp

substate3:
  CALL_C(b_+81, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+84);
  CALL_C(b_+84, objectApplySpeed_hook, SYM(objectApplySpeed), b_+87);
  CYC(b_+87, b_+90); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+90, b_+92); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(b_+92); return; } // ret nz
  CYC(b_+92, b_+93);
  CALL_C(b_+93, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+96);
  CYC(b_+96, b_+99); SET_BC((SYM(interactionCode79__state0) + 23));
  CYC(b_+99, b_+102); TAIL(interactionSetPosition); // jp

substate4:
  CYC(b_+102, b_+105); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+105, b_+107); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+107); return; } // ret nz
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+111); TAIL(interactionIncSubstate); // jp

substate5:
  CYC(b_+111, b_+113); C = 0x01;
  CALL_C(b_+113, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+116);
  if (!(F & FZ)) { RET_TAKEN(b_+116); return; } // ret nz
  CYC(b_+116, b_+117);
  CALL_C(b_+117, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+120);
  CYC(b_+120, b_+122); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 30);
  CALL_C(b_+124, objectSetVisible82_hook, SYM(objectSetVisible82), b_+127);
  CYC(b_+127, b_+129); A = 0x05;
  CYC(b_+129, b_+132); TAIL(interactionSetAnimation); // jp

substate6:
  CALL_C(b_+132, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+135);
  if (!(F & FZ)) { CYCT(b_+135, b_+137); goto l51d8; } // jr nz
  CYC(b_+135, b_+137);
  CYC(b_+137, b_+138); alu_xor(gb, A);
  CYC(b_+138, b_+141); W8(wGfxRegs1_SCY) = A;
  CYC(b_+141, b_+144); TAIL(interactionIncSubstate); // jp

l51d8:
  CYC(b_+144, b_+145); A = mem_rd(gb, HL);
  CYC(b_+145, b_+147); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+147, b_+149); goto l51df; } // jr nz
  CYC(b_+147, b_+149);
  CYC(b_+149, b_+151); A = 0xff;

l51df:
  CYC(b_+151, b_+154); W8(wGfxRegs1_SCY) = A;

substate7:
  RET(b_+154); return; // ret
}

// interaction6b_subid11: triggers the cutscene after beating Jabu-Jabu. Runs unconditionally
// every frame (no checkInteractionState dispatch) until it deletes itself.
void interaction6b_subid11_hook(GB *gb) {
  BASE(interaction6b_subid11);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wEssencesObtained);
  CYC(b_+3, b_+5); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto delete_; } // jr z
  CYC(b_+5, b_+7);
  CALL_C(b_+7, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto delete_; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+19); W8(wDisabledObjects) = A;
  CYC(b_+19, b_+22); W8(wMenuDisabled) = A;
  CYC(b_+22, b_+24); A = 0x21; // CUTSCENE_BLACK_TOWER_COMPLETE
  CYC(b_+24, b_+27); W8(wCutsceneTrigger) = A;

delete_:
  CYC(b_+27, b_+30); TAIL(interactionDelete); // jp
}

// interaction6b_subid13 (also interaction6b_subid14, an alias for the same code): the Goron
// bomb statue (left/right).
void interaction6b_subid13_hook(GB *gb) {
  BASE(interaction6b_subid13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid14@state0
  CALL_C(b_+5, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+8);
  CALL_C(b_+8, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+11);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); B = 0xcf; // >wRoomLayout
  CYC(b_+14, b_+16); A = 0x00;
  CYC(b_+16, b_+17); mem_wr(gb, BC, A);
  CYC(b_+17, b_+19); B = 0xce; // >wRoomCollisions
  CYC(b_+19, b_+21); A = 0x0f;
  CYC(b_+21, b_+22); mem_wr(gb, BC, A);

state1:
  CYC(b_+22, b_+25); TAIL(interactionPushLinkAwayAndUpdateDrawPriority); // jp
}

// interaction6b_subid15: the stone statue of Link as seen in-game. Both of its exits are tail
// jumps into interaction6b_subid0e's own inline state0/state1 code (0x50be/0x50e5) -- addresses
// that live inside a different top-level hook's body, not independently callable -- so that
// entire body is replicated here byte-for-byte (same addresses, same CYC ranges), exactly as
// interaction6b_subid05 already does for a one-instruction case earlier in this file.
void interaction6b_subid15_hook(GB *gb) {
  BASE(interaction6b_subid0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(SYM(interaction6b_subid15), checkInteractionState_hook, SYM(checkInteractionState), (SYM(interaction6b_subid15) + 3));
  if (!(F & FZ)) { CYCT((SYM(interaction6b_subid15) + 3), (SYM(interaction6b_subid15) + 6)); goto subid0e_state1; } // jp nz
  CYC((SYM(interaction6b_subid15) + 3), (SYM(interaction6b_subid15) + 6));

  // interaction6b_subid15@state0
  CYC(SYM(interaction6b_subid15__state0), (SYM(interaction6b_subid15__state0) + 2)); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C((SYM(interaction6b_subid15__state0) + 2), checkGlobalFlag_hook, SYM(checkGlobalFlag), (SYM(interaction6b_subid15__state0) + 5));
  if (F & FZ) { CYCT((SYM(interaction6b_subid15__state0) + 5), (SYM(interaction6b_subid15__state0) + 8)); interactionDelete_hook(gb); return; } // jp z
  CYC((SYM(interaction6b_subid15__state0) + 5), (SYM(interaction6b_subid15__state0) + 8));
  CALL_C((SYM(interaction6b_subid15__state0) + 8), objectGetShortPosition_hook, SYM(objectGetShortPosition), (SYM(interaction6b_subid15__state0) + 11));
  CYC((SYM(interaction6b_subid15__state0) + 11), (SYM(interaction6b_subid15__state0) + 13)); H = 0xce; // >wRoomCollisions
  CYC((SYM(interaction6b_subid15__state0) + 13), (SYM(interaction6b_subid15__state0) + 14)); L = A;
  CYC((SYM(interaction6b_subid15__state0) + 14), (SYM(interaction6b_subid15__state0) + 16)); mem_wr(gb, HL, 0x0f);
  CYC((SYM(interaction6b_subid15__state0) + 16), (SYM(interaction6b_subid15__state0) + 19)); goto subid0e_state0; // jp

subid0e_state0:
  CYC(b_+5, b_+8); A = W8(wTilesetFlags);
  CYC(b_+8, b_+10); alu_and(gb, 0x80); // TILESETFLAG_PAST
  CYC(b_+10, b_+12); A = 0xc7; // PALH_c7
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto subid0e_l50c8; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);

subid0e_l50c8:
  CALL_C(b_+15, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+18);
  CALL_C(b_+18, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+21);
  CYC(b_+21, b_+24); SET_BC(0x080a);
  CALL_C(b_+24, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+27);
  CALL_C(b_+27, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+30);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+33); B = 0xcf; // >wRoomLayout
  CYC(b_+33, b_+34); A = mem_rd(gb, BC);
  CYC(b_+34, b_+36); alu_cp(gb, 0xf9);
  CYC(b_+36, b_+38); A = 0x04;
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto subid0e_l50e2; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); A = alu_inc8(gb, A);

subid0e_l50e2:
  CALL_C(b_+41, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+44);

subid0e_state1:
  CALL_C(b_+44, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+47);
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+50, b_+52); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x40);
  RET(b_+52); return; // ret
}

// interaction6b_subid16: a flame that appears for [counter1] frames.
void interaction6b_subid16_hook(GB *gb) {
  BASE(interaction6b_subid16);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction6b_subid16@state0
  CALL_C(b_+5, interaction6b_initGraphicsAndIncState_hook, SYM(interaction6b_initGraphicsAndIncState), b_+8);
  CALL_C(b_+8, objectSetVisible81_hook, SYM(objectSetVisible81), b_+11);
  CYC(b_+11, b_+13); A = 0x72; // SND_LIGHTTORCH
  CYC(b_+13, b_+16); TAIL(playSound_b00); // jp

state1:
  CALL_C(b_+16, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+19);
  if (F & FZ) { CYCT(b_+19, b_+22); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+25); TAIL(interactionAnimate); // jp
}

// INTERAC_MISCELLANEOUS_1
void interactionCode6b_hook(GB *gb) {
  BASE(interactionCode6b);
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
    uint16_t target = interaction6b_subid01_jump_table(gb);
    if (target == SYM(interaction6b_subid00)) { interaction6b_subid00_hook(gb); return; }
    if (target == SYM(interaction6b_subid01)) { interaction6b_subid01_hook(gb); return; }
    if (target == SYM(interaction6b_subid02)) { interaction6b_subid02_hook(gb); return; }
    if (target == SYM(interaction6b_subid03)) { interaction6b_subid03_hook(gb); return; } // also subid12
    if (target == SYM(interaction6b_subid04)) { interaction6b_subid04_hook(gb); return; }
    if (target == SYM(interaction6b_subid05)) { interaction6b_subid05_hook(gb); return; }
    if (target == SYM(interaction6b_subid06)) { interaction6b_subid06_hook(gb); return; }
    if (target == SYM(interaction6b_subid07)) { interaction6b_subid07_hook(gb); return; }
    if (target == SYM(interaction6b_subid08)) { interaction6b_subid08_hook(gb); return; }
    if (target == SYM(interaction6b_subid09)) { interaction6b_subid09_hook(gb); return; }
    if (target == SYM(interaction6b_subid0a)) { interaction6b_subid0a_hook(gb); return; } // also subid0b, subid0c
    if (target == SYM(interaction6b_subid0d)) { interaction6b_subid0d_hook(gb); return; }
    if (target == SYM(interaction6b_subid0e)) { interaction6b_subid0e_hook(gb); return; }
    if (target == SYM(interaction6b_subid0f)) { interaction6b_subid0f_hook(gb); return; }
    if (target == SYM(interaction6b_subid10)) { interaction6b_subid10_hook(gb); return; }
    if (target == SYM(interaction6b_subid11)) { interaction6b_subid11_hook(gb); return; }
    if (target == SYM(interaction6b_subid13)) { interaction6b_subid13_hook(gb); return; } // also subid14
    if (target == SYM(interaction6b_subid15)) { interaction6b_subid15_hook(gb); return; }
    TAIL(interaction6b_subid16); // target == 0x5233
  }
}
