#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t companionScripts_jump_table(GB *gb) {
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

static void companionScripts_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void companionScripts_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void companionScript_cpYToCompanion_hook(GB *gb) {
  BASE(companionScript_cpYToCompanion);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(w1Companion_yh); // w1Companion.yh
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+7); return; // ret
}

void companionScript_cpXToCompanion_hook(GB *gb) {
  BASE(companionScript_cpXToCompanion);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(w1Companion_xh); // w1Companion.xh
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+7); return; // ret
}

// Shared tail for companionScript_restrictHigherX/LowerX/LowerY/HigherY: physically owned by
// restrictHigherY's own instruction range, reached by all four via `jr`. HL still points at
// w1Companion.xh/yh (left there by cpXToCompanion/cpYToCompanion), so `ld (hl),a` here writes
// the clamped position directly into the companion's own struct.
static void companionScripts_restrictTail(GB *gb) {
  BASE(companionScript_restrictHigherY);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); A = W8(wLinkObjectIndex);
  CYC(b_+11, b_+12); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; } // ret nc
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); A = C;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = OBJ_SPEED; // SpecialObject.speed (HL still points into w1Companion)
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00); // SPEED_0
  CYC(b_+19, b_+21); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+25); SET_HL(SYM(companionScript_companionBarrierText)); // companionScript_companionBarrierText
  CYC(b_+25, b_+26); companionScripts_add_double_index(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+27, b_+28); B = mem_rd(gb, HL);
  CYC(b_+28, b_+29); C = A;
  CYC(b_+29, b_+32); TAIL(showText); // jp
}

void companionScript_restrictHigherX_hook(GB *gb) {
  BASE(companionScript_restrictHigherX);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionScript_cpXToCompanion_hook, SYM(companionScript_cpXToCompanion), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, (SYM(companionScript_restrictHigherY) + 7)); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_restrictLowerX_hook(GB *gb) {
  BASE(companionScript_restrictLowerX);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionScript_cpXToCompanion_hook, SYM(companionScript_cpXToCompanion), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } // ret nc
  CYC(b_+3, b_+4);
  CYC(b_+4, (SYM(companionScript_restrictHigherY) + 7)); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_restrictLowerY_hook(GB *gb) {
  BASE(companionScript_restrictLowerY);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionScript_cpYToCompanion_hook, SYM(companionScript_cpYToCompanion), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } // ret nc
  CYC(b_+3, b_+4);
  CYC(b_+4, (SYM(companionScript_restrictHigherY) + 7)); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_restrictHigherY_hook(GB *gb) {
  BASE(companionScript_restrictHigherY);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, companionScript_cpYToCompanion_hook, SYM(companionScript_cpYToCompanion), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+7); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_deleteSelf_hook(GB *gb) {
  BASE(companionScript_deleteSelf);
  CYC(b_+0, b_+3); TAIL(interactionDelete); // jp
}

void companionScript_delete_hook(GB *gb) {
  BASE(companionScript_delete);
  CYC(b_+0, b_+3); TAIL(interactionDelete); // jp
}

// Delete self if game is completed; otherwise, stay in state 0 until Link mounts the companion.
void companionScript_genericState0_hook(GB *gb) {
  BASE(companionScript_genericState0);
  CYC(b_+0, b_+3); A = W8(wFileIsCompleted);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+10); A = W8(wLinkObjectIndex);
  CYC(b_+10, b_+11); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; } // ret nc
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18); A = W8(w1Companion_id);
  CYC(b_+18, b_+20); alu_sub(gb, 0x0b); // SPECIALOBJECT_RICKY
  CYC(b_+20, b_+22); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); alu_add(gb, 0x46); // <wRickyState
  CYC(b_+25, b_+26); L = A;
  CYC(b_+26, b_+28); H = 0xc6; // >wRickyState
  CYC(b_+28, b_+30); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+30, b_+33); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(b_+30, b_+33);
  RET(b_+33); return; // ret
}

void companionScript_runScript_hook(GB *gb) {
  BASE(companionScript_runScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } // ret nc
  CYC(b_+3, b_+4);
  CALL_C(b_+4, setStatusBarNeedsRefreshBit1_hook, SYM(setStatusBarNeedsRefreshBit1), SYM(companionScript_delete));
  TAIL(companionScript_delete);
}

void companionScript_subid00_state1_hook(GB *gb) {
  BASE(companionScript_subid00_state1);
  // If var3a is nonzero, make Moosh shake in fear
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); companionScript_runScript_hook(gb); return; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); companionScript_runScript_hook(gb); return; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = W8(w1Companion_xh);
  CYC(b_+15, b_+17); alu_xor(gb, 0x02);
  CYC(b_+17, b_+20); W8(w1Companion_xh) = A;
  TAIL(companionScript_runScript);
}

void companionScript_subid01_hook(GB *gb) {
  BASE(companionScript_subid01);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_restrictHigherX)) { companionScript_restrictHigherX_hook(gb); return; }
  TAIL(companionScript_genericState0);
}

void companionScript_subid02_hook(GB *gb) {
  BASE(companionScript_subid02);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_restrictLowerY)) { companionScript_restrictLowerY_hook(gb); return; }
  TAIL(companionScript_genericState0);
}

void companionScript_subid04_hook(GB *gb) {
  BASE(companionScript_subid04);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_restrictHigherY)) { companionScript_restrictHigherY_hook(gb); return; }
  TAIL(companionScript_genericState0);
}

void companionScript_subid05_hook(GB *gb) {
  BASE(companionScript_subid05);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_restrictLowerX)) { companionScript_restrictLowerX_hook(gb); return; }
  TAIL(companionScript_genericState0);
}

void companionScript_subid00_hook(GB *gb) {
  BASE(companionScript_subid00);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_subid00_state1)) { companionScript_subid00_state1_hook(gb); return; }

  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); A = W8(wEssencesObtained);
  CYC(b_+14, b_+16); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+16, b_+19); companionScript_deleteSelf_hook(gb); return; } // jp z
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wGroup1RoomFlags + 0x79); // wPastRoomFlags+$79
  CYC(b_+22, b_+24); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+24, b_+27); companionScript_deleteSelf_hook(gb); return; } // jp z
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+30); A = W8(wMooshState);
  CYC(b_+30, b_+32); alu_and(gb, 0x60);
  if (!(F & FZ)) { CYCT(b_+32, b_+35); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+40); W8(wDisableScreenTransitions) = A;
  CYC(b_+40, b_+43); W8(wDiggingUpEnemiesForbidden) = A;
  CYC(b_+43, b_+46); SET_HL((SYM(fallingRock_subid01__state1) + 11)); // mainScripts.companionScript_subid00Script
  CYC(b_+46, b_+49); TAIL(interactionSetScript); // jp
}

// Ricky script when he loses his gloves
void companionScript_subid03_hook(GB *gb) {
  BASE(companionScript_subid03);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }

  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); SET_HL(wRickyState);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); SET_HL((SYM(fallingRock_subid02__angles) + 3)); // mainScripts.companionScript_subid03Script
  CYC(b_+22, b_+25); TAIL(interactionSetScript); // jp
}

// Dimitri script where he's harrassed by tokays
void companionScript_subid07_hook(GB *gb) {
  BASE(companionScript_subid07);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }

  CYC(b_+8, b_+11); A = W8(wDimitriState);
  CYC(b_+11, b_+13); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+21); SET_HL((SYM(fallingRock_subid02__angles) + 7)); // mainScripts.companionScript_subid07Script
  CYC(b_+21, b_+24); TAIL(interactionSetScript); // jp
}

// Dimitri script where he leaves Link after bringing him to the mainland
void companionScript_subid06_hook(GB *gb) {
  BASE(companionScript_subid06);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }

  // Delete self if dimitri isn't here or the event has happened already
  CYC(b_+8, b_+11); A = W8(wDimitriState);
  CYC(b_+11, b_+13); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_HL(w1Companion_id);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0x0c); // SPECIALOBJECT_DIMITRI
  if (!(F & FZ)) { CYCT(b_+21, b_+23); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+21, b_+23);
  // Return if Dimitri's still in the water
  CYC(b_+23, b_+25); L = 0x38; // SpecialObject.var38
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; } // ret nz
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+34); W8(wDisableScreenTransitions) = A;
  // Manipulate Dimitri's state to force a dismount
  CYC(b_+34, b_+36); L = 0x03; // SpecialObject.var03
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x02);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0a);
  CYC(b_+41, b_+44); SET_HL((SYM(fallingRock_subid02__angles) + 11)); // mainScripts.companionScript_subid06Script
  CYC(b_+44, b_+47); TAIL(interactionSetScript); // jp
}

// A fairy appears to tell you about the animal companion in the forest
void companionScript_subid08_hook(GB *gb) {
  BASE(companionScript_subid08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == b_+63) goto state1;
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }

  // Clear $10 bytes starting at $cfd0
  CYC(b_+10, b_+13); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(b_+13, b_+15); B = 0x10;
  CALL_C(b_+15, clearMemory_hook, SYM(clearMemory), b_+18);
  CYC(b_+18, b_+20); A = 0x1d; // GLOBALFLAG_CAN_BUY_FLUTE
  CALL_C(b_+20, unsetGlobalFlag_hook, SYM(unsetGlobalFlag), b_+23);
  CYC(b_+23, b_+25); L = 0x10; // <wAnimalCompanion
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto l5ac8; } // jr nz
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); A = 0x0d; // SPECIALOBJECT_MOOSH
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);

l5ac8:
  CYC(b_+32, b_+34); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(b_+34, b_+36); alu_add(gb, 0x23); // <TX_1123
  CYC(b_+36, b_+39); W8(wTextSubstitutions) = A;
  CYC(b_+39, b_+42); A = W8(wScreenTransitionDirection);
  CYC(b_+42, b_+44); alu_cp(gb, 0x03); // DIR_LEFT
  if (!(F & FZ)) { CYCT(b_+44, b_+46); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); A = 0x22; // GLOBALFLAG_TALKED_TO_HEAD_CARPENTER
  CALL_C(b_+48, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+51);
  if (F & FZ) { CYCT(b_+51, b_+53); companionScript_deleteSelf_hook(gb); return; } // jr z
  CYC(b_+51, b_+53);
  CALL_C(b_+53, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+56);
  CYC(b_+56, b_+58); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+58, b_+60); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+63); TAIL(interactionIncState); // jp

state1:
  CYC(b_+63, b_+66); A = W8(w1Link_xh);
  CYC(b_+66, b_+68); alu_cp(gb, 0x50);
  if (!(F & FC)) { RET_TAKEN(b_+68); return; } // ret nc
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); A = 0x81;
  CYC(b_+71, b_+74); W8(wMenuDisabled) = A;
  CYC(b_+74, b_+77); W8(wDisabledObjects) = A;
  CALL_C(b_+77, putLinkOnGround_hook, SYM(putLinkOnGround), b_+80);
  CYC(b_+80, b_+83); SET_BC((SYM(interactionCode89__label_0a_036) + 14)); // INTERAC_FOREST_FAIRY, $03
  CALL_C(b_+83, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+86);
  CYC(b_+86, b_+88); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x0f);
  CYC(b_+90, b_+93); SET_HL((SYM(fallingRock_updateSpeedAndDeleteWhenLanded) + 7)); // mainScripts.companionScript_subid08Script
  CALL_C(b_+93, interactionSetScript_hook, SYM(interactionSetScript), b_+96);
  CYC(b_+96, b_+99); TAIL(interactionIncState); // jp
}

// Companion script where they're found in the fairy forest
void companionScript_subid09_hook(GB *gb) {
  BASE(companionScript_subid09);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }

  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  // Check whether the event is applicable right now
  CYC(b_+15, b_+17); A = 0x22; // GLOBALFLAG_TALKED_TO_HEAD_CARPENTER
  CALL_C(b_+17, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+22); companionScript_deleteSelf_hook(gb); return; } // jr z
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(b_+24, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+27);
  if (!(F & FZ)) { CYCT(b_+27, b_+30); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(b_+27, b_+30);
  // Put companion index (0-2) in var39
  CYC(b_+30, b_+33); SET_HL(wAnimalCompanion);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); C = A;
  CYC(b_+40, b_+43); SET_HL(b_+86); // @animationWhenNoticingLink
  CYC(b_+43, b_+44); companionScripts_addAToHl_from_rst(gb, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); A = C;
  CYC(b_+49, b_+50); alu_add(gb, A); // add a
  CYC(b_+50, b_+53); SET_HL(b_+74); // @data1
  CYC(b_+53, b_+54); companionScripts_add_double_index(gb, b_+54);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+55, b_+58); W8(wTextSubstitutions) = A;
  CALL_C(b_+58, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+61);
  if (F & FZ) { CYCT(b_+61, b_+63); goto l5b4b; } // jr z
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)

l5b4b:
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+65, b_+68); mem_wr(gb, wTextSubstitutions + 1, A);
  CYC(b_+68, b_+71); SET_HL(SYM(fallingRock_subid03)); // mainScripts.companionScript_subid09Script
  CYC(b_+71, b_+74); TAIL(interactionSetScript); // jp
}

// Script just outside the forest, where you get the flute
void companionScript_subid0a_hook(GB *gb) {
  BASE(companionScript_subid0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }
  if (target == SYM(companionScript_subid0a_state2)) { companionScript_subid0a_state2_hook(gb); return; }
  if (target == SYM(companionScript_subid0a_state3)) { companionScript_subid0a_state3_hook(gb); return; }

  CYC(b_+12, b_+14); A = 0x24; // GLOBALFLAG_SAVED_COMPANION_FROM_FOREST
  CALL_C(b_+14, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+20); companionScript_delete_hook(gb); return; } // jp z
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+22); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(b_+22, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); companionScript_delete_hook(gb); return; } // jp nz
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); A = 0x01; // [state] = 1
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+34); W8(wMenuDisabled) = A;
  CYC(b_+34, b_+37); W8(wDisabledObjects) = A;
  CYC(b_+37, b_+40); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  CYC(b_+40, b_+42); A = 0x00; // DIR_UP
  CYC(b_+42, b_+45); W8(w1Link_direction) = A;
  // Put companion index (0-2) in var39
  CYC(b_+45, b_+48); SET_HL(wAnimalCompanion);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  // Determine text to show for this companion
  CYC(b_+54, b_+55); alu_add(gb, A); // add a
  CYC(b_+55, b_+58); SET_HL(b_+97); // @textIndices
  CYC(b_+58, b_+59); companionScripts_add_double_index(gb, b_+59);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+60, b_+63); mem_wr(gb, wTextSubstitutions + 1, A);
  CALL_C(b_+63, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+66);
  if (F & FZ) { CYCT(b_+66, b_+68); goto l5ba9; } // jr z
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)

l5ba9:
  CYC(b_+69, b_+70); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+70, b_+73); W8(wTextSubstitutions) = A;
  // Spawn in the fairies
  CYC(b_+73, b_+76); SET_BC(0x1103);

nextFairy:
  CYC(b_+76, b_+77); push_effect(gb, BC);
  CYC(b_+77, b_+80); SET_BC((SYM(interactionCode89__label_0a_036) + 15)); // INTERAC_FOREST_FAIRY, $04
  CALL_C(b_+80, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+83);
  CYC(b_+83, b_+84); SET_BC(pop_effect(gb));
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+86, b_+87); mem_wr(gb, HL, B);
  CYC(b_+87, b_+88); B = alu_inc8(gb, B);
  CYC(b_+88, b_+89); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+89, b_+91); goto nextFairy; } // jr nz
  CYC(b_+89, b_+91);
  CYC(b_+91, b_+94); SET_HL((SYM(fallingRock_subid03) + 4)); // mainScripts.companionScript_subid0aScript
  CYC(b_+94, b_+97); TAIL(interactionSetScript); // jp
}

// This is the part which gives Link the flute.
void companionScript_subid0a_state2_hook(GB *gb) {
  BASE(companionScript_subid0a_state2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0e; // TREASURE_FLUTE
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  CYC(b_+5, b_+7); C = 0x38; // <TX_0038
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto l5c44; } // jr nc
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); C = 0x69; // <TX_0069

l5c44:
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_VAR39; // Companion index
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_add(gb, C);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+18); B = 0x00; // >TX_0038
  CALL_C(b_+18, showText_hook, SYM(showText), b_+21);
  CYC(b_+21, b_+23); A = 0x01;
  CYC(b_+23, b_+26); W8(wMenuDisabled) = A;
  CALL_C(b_+26, interactionIncState_hook, SYM(interactionIncState), b_+29);
  // Set wFluteIcon
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); C = A;
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+38); SET_HL(wFluteIcon);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  // Set bit 7 of wRickyState / wDimitriState / wMooshState
  CYC(b_+39, b_+41); alu_add(gb, 0x45); // <wCompanionStates - 1
  CYC(b_+41, b_+42); L = A;
  CYC(b_+42, b_+44); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  // Give flute
  CYC(b_+44, b_+46); A = 0x0e; // TREASURE_FLUTE
  CALL_C(b_+46, giveTreasure_hook, SYM(giveTreasure), b_+49);
  CYC(b_+49, b_+52); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+52, b_+54); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01); // set 0,(hl)
  // Turn this object into the flute graphic?
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+56, b_+57); alu_xor(gb, A);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CALL_C(b_+58, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+61);
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+63, b_+65); A = 0x0a;
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  // Set this object's palette
  CYC(b_+66, b_+68); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); C = A;
  CYC(b_+70, b_+72); alu_and(gb, 0x01);
  CYC(b_+72, b_+73); alu_add(gb, A); // add a
  CYC(b_+73, b_+74); alu_xor(gb, C);
  CYC(b_+74, b_+76); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  // Set this object's position
  CYC(b_+77, b_+80); SET_HL(w1Link);
  CYC(b_+80, b_+83); SET_BC(0xf200);
  CALL_C(b_+83, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+86);
  // Make Link hold it over his head
  CYC(b_+86, b_+89); SET_HL(wLinkForceState);
  CYC(b_+89, b_+91); A = 0x04; // LINK_STATE_04
  CYC(b_+91, b_+92); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x01); // [wcc50] = $01
  CALL_C(b_+94, objectSetVisible80_hook, SYM(objectSetVisible80), b_+97);
  CYC(b_+97, b_+100); TAIL(interactionRunScript); // jp
}

void companionScript_subid0a_state3_hook(GB *gb) {
  BASE(companionScript_subid0a_state3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  // ??
  CYC(b_+3, b_+6); A = W8(wLinkObjectIndex);
  CYC(b_+6, b_+8); alu_and(gb, 0x0f);
  CYC(b_+8, b_+9); alu_add(gb, A); // add a
  CYC(b_+9, b_+11); alu_swap_a(gb);
  CYC(b_+11, b_+14); W8(wDisabledObjects) = A;
  // Make flute disappear, wait for script to end
  CALL_C(b_+14, objectSetInvisible_hook, SYM(objectSetInvisible), b_+17);
  CALL_C(b_+17, interactionRunScript_hook, SYM(interactionRunScript), b_+20);
  if (!(F & FC)) { RET_TAKEN(b_+20); return; } // ret nc
  CYC(b_+20, b_+21);
  // Clean up, delete self
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+25); W8(wDisabledObjects) = A;
  CYC(b_+25, b_+28); W8(wMenuDisabled) = A;
  CYC(b_+28, b_+31); TAIL(companionScript_delete); // jp
}

// Script in first screen of forest, where fairy leads you to the companion
void companionScript_subid0b_hook(GB *gb) {
  BASE(companionScript_subid0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == SYM(companionScript_runScript)) { companionScript_runScript_hook(gb); return; }

  CYC(b_+8, b_+11); A = W8(wScreenTransitionDirection);
  CYC(b_+11, b_+13); alu_cp(gb, 0x02); // DIR_DOWN
  if (!(F & FZ)) { CYCT(b_+13, b_+15); companionScript_delete_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x42; // GLOBALFLAG_COMPANION_LOST_IN_FOREST
  CALL_C(b_+17, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+22); companionScript_delete_hook(gb); return; } // jr z
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(b_+24, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+27);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); companionScript_delete_hook(gb); return; } // jr nz
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC((SYM(interactionCode89__label_0a_036) + 14)); // INTERAC_FOREST_FAIRY, $03
  CALL_C(b_+32, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+35);
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x14);
  CYC(b_+39, b_+41); A = 0x81;
  CYC(b_+41, b_+44); W8(wMenuDisabled) = A;
  CYC(b_+44, b_+47); W8(wDisabledObjects) = A;
  CYC(b_+47, b_+48); alu_xor(gb, A);
  CYC(b_+48, b_+51); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  CYC(b_+51, b_+54); SET_HL((SYM(fallingRock_subid03__state0) + 3)); // mainScripts.companionScript_subid0bScript
  CALL_C(b_+54, interactionSetScript_hook, SYM(interactionSetScript), b_+57);
  CYC(b_+57, b_+60); TAIL(interactionIncState); // jp
}

// Sets bit 6 of wDimitriState so he disappears from Tokay Island
void companionScript_subid0c_hook(GB *gb) {
  BASE(companionScript_subid0c);
  CYC(b_+0, b_+3); A = W8(wDimitriState);
  CYC(b_+3, b_+5); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+5, b_+7); companionScript_delete_hook(gb); return; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_or(gb, 0x40);
  CYC(b_+9, b_+12); W8(wDimitriState) = A;
  CYC(b_+12, b_+14); TAIL(companionScript_delete); // jr
}

// Companion barrier to Symmetry City, until the tuni nut is restored
void companionScript_subid0d_hook(GB *gb) {
  BASE(companionScript_subid0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x29; // GLOBALFLAG_TUNI_NUT_PLACED
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); A = W8(wScrollMode);
  CYC(b_+11, b_+13); alu_and(gb, 0x0e); // SCROLLMODE_08|SCROLLMODE_04|SCROLLMODE_02
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } // ret nz
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); SET_HL(w1Companion); // w1Companion.enabled
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+19); return; } // ret z
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+21, b_+23); alu_cp(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  if (F & FC) { RET_TAKEN(b_+23); return; } // ret c
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); alu_cp(gb, 0x0e); // SPECIALOBJECT_LAST_COMPANION+1
  if (!(F & FC)) { RET_TAKEN(b_+26); return; } // ret nc
  CYC(b_+26, b_+27);
  // Check if the companion is roughly at this object's position
  CYC(b_+27, b_+29); L = 0x0d; // SpecialObject.xh
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+35); alu_add(gb, 0x05);
  CYC(b_+35, b_+37); alu_cp(gb, 0x0b);
  if (!(F & FC)) { RET_TAKEN(b_+37); return; } // ret nc
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); L = 0x0b; // SpecialObject.yh
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { RET_TAKEN(b_+44); return; } // ret c
  CYC(b_+44, b_+45);
  // If so, prevent companion from moving any further up
  CYC(b_+45, b_+46); A = alu_inc8(gb, A);
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);
  CYC(b_+47, b_+49); L = 0x10; // SpecialObject.speed
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x00);
  CYC(b_+51, b_+53); L = 0x04; // SpecialObject.state
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  // If it's Dimitri being held, make Link drop him
  CYC(b_+54, b_+56); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto l5a30; } // jr nz
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x03);
  CALL_C(b_+60, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+63);

l5a30:
  CYC(b_+63, b_+66); A = W8(wAnimalCompanion);
  CYC(b_+66, b_+68); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(b_+68, b_+71); SET_HL(b_+78); // @textIndices
  CYC(b_+71, b_+72); companionScripts_add_double_index(gb, b_+72);
  CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+73, b_+74); B = mem_rd(gb, HL);
  CYC(b_+74, b_+75); C = A;
  CYC(b_+75, b_+78); TAIL(showText); // jp
}

// INTERAC_COMPANION_SCRIPTS
void interactionCode71_hook(GB *gb) {
  BASE(interactionCode71);
  CYC(b_+0, b_+3); A = W8(wLinkDeathTrigger);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto l5915; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+10); W8(wDisabledObjects) = A;
  CYC(b_+10, b_+13); TAIL(interactionDelete); // jp

l5915:
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = companionScripts_jump_table(gb);
    if (target == SYM(companionScript_subid01)) { companionScript_subid01_hook(gb); return; }
    if (target == SYM(companionScript_subid02)) { companionScript_subid02_hook(gb); return; }
    if (target == SYM(companionScript_subid03)) { companionScript_subid03_hook(gb); return; }
    if (target == SYM(companionScript_subid04)) { companionScript_subid04_hook(gb); return; }
    if (target == SYM(companionScript_subid05)) { companionScript_subid05_hook(gb); return; }
    if (target == SYM(companionScript_subid06)) { companionScript_subid06_hook(gb); return; }
    if (target == SYM(companionScript_subid07)) { companionScript_subid07_hook(gb); return; }
    if (target == SYM(companionScript_subid08)) { companionScript_subid08_hook(gb); return; }
    if (target == SYM(companionScript_subid09)) { companionScript_subid09_hook(gb); return; }
    if (target == SYM(companionScript_subid0a)) { companionScript_subid0a_hook(gb); return; }
    if (target == SYM(companionScript_subid0b)) { companionScript_subid0b_hook(gb); return; }
    if (target == SYM(companionScript_subid0c)) { companionScript_subid0c_hook(gb); return; }
    if (target == SYM(companionScript_subid0d)) { companionScript_subid0d_hook(gb); return; }
    TAIL(companionScript_subid00); // target == 0x5935
  }
}
