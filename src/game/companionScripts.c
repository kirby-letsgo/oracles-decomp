#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  CYC(0x59db, 0x59dd); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x59dd, 0x59de); A = mem_rd(gb, DE);
  CYC(0x59de, 0x59e1); SET_HL(0xd10b); // w1Companion.yh
  CYC(0x59e1, 0x59e2); alu_cp(gb, mem_rd(gb, HL));
  RET(0x59e2); return; // ret
}

void companionScript_cpXToCompanion_hook(GB *gb) {
  CYC(0x59e3, 0x59e5); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x59e5, 0x59e6); A = mem_rd(gb, DE);
  CYC(0x59e6, 0x59e9); SET_HL(0xd10d); // w1Companion.xh
  CYC(0x59e9, 0x59ea); alu_cp(gb, mem_rd(gb, HL));
  RET(0x59ea); return; // ret
}

// Shared tail for companionScript_restrictHigherX/LowerX/LowerY/HigherY: physically owned by
// restrictHigherY's own instruction range, reached by all four via `jr`. HL still points at
// w1Companion.xh/yh (left there by cpXToCompanion/cpYToCompanion), so `ld (hl),a` here writes
// the clamped position directly into the companion's own struct.
static void companionScripts_restrictTail(GB *gb) {
  CYC(0x59c2, 0x59c3); C = A;
  CYC(0x59c3, 0x59c6); A = W8(wLinkObjectIndex);
  CYC(0x59c6, 0x59c7); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x59c7); return; } // ret nc
  CYC(0x59c7, 0x59c8);
  CYC(0x59c8, 0x59c9); A = C;
  CYC(0x59c9, 0x59ca); mem_wr(gb, HL, A);
  CYC(0x59ca, 0x59cc); L = OBJ_SPEED; // SpecialObject.speed (HL still points into w1Companion)
  CYC(0x59cc, 0x59ce); mem_wr(gb, HL, 0x00); // SPEED_0
  CYC(0x59ce, 0x59d0); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x59d0, 0x59d1); A = mem_rd(gb, DE);
  CYC(0x59d1, 0x59d4); SET_HL(0x59eb); // companionScript_companionBarrierText
  CYC(0x59d4, 0x59d5); companionScripts_add_double_index(gb, 0x59d5);
  CYC(0x59d5, 0x59d6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x59d6, 0x59d7); B = mem_rd(gb, HL);
  CYC(0x59d7, 0x59d8); C = A;
  CYC(0x59d8, 0x59db); showText_hook(gb); return; // jp
}

void companionScript_restrictHigherX_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59a8, companionScript_cpXToCompanion_hook, 0x59e3, 0x59ab);
  if (F & FC) { RET_TAKEN(0x59ab); return; } // ret c
  CYC(0x59ab, 0x59ac);
  CYC(0x59ac, 0x59ad); A = alu_inc8(gb, A);
  CYC(0x59ad, 0x59c2); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_restrictLowerX_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59af, companionScript_cpXToCompanion_hook, 0x59e3, 0x59b2);
  if (!(F & FC)) { RET_TAKEN(0x59b2); return; } // ret nc
  CYC(0x59b2, 0x59b3);
  CYC(0x59b3, 0x59c2); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_restrictLowerY_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59b5, companionScript_cpYToCompanion_hook, 0x59db, 0x59b8);
  if (!(F & FC)) { RET_TAKEN(0x59b8); return; } // ret nc
  CYC(0x59b8, 0x59b9);
  CYC(0x59b9, 0x59c2); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_restrictHigherY_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59bb, companionScript_cpYToCompanion_hook, 0x59db, 0x59be);
  if (F & FC) { RET_TAKEN(0x59be); return; } // ret c
  CYC(0x59be, 0x59bf);
  CYC(0x59bf, 0x59c0); A = alu_inc8(gb, A);
  CYC(0x59c0, 0x59c2); companionScripts_restrictTail(gb); return; // jr
}

void companionScript_deleteSelf_hook(GB *gb) {
  CYC(0x5aa5, 0x5aa8); interactionDelete_hook(gb); return; // jp
}

void companionScript_delete_hook(GB *gb) {
  CYC(0x5c36, 0x5c39); interactionDelete_hook(gb); return; // jp
}

// Delete self if game is completed; otherwise, stay in state 0 until Link mounts the companion.
void companionScript_genericState0_hook(GB *gb) {
  CYC(0x5986, 0x5989); A = W8(wFileIsCompleted);
  CYC(0x5989, 0x598a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x598a, 0x598d); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(0x598a, 0x598d);
  CYC(0x598d, 0x5990); A = W8(wLinkObjectIndex);
  CYC(0x5990, 0x5991); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x5991); return; } // ret nc
  CYC(0x5991, 0x5992);
  CYC(0x5992, 0x5994); A = 0x01;
  CYC(0x5994, 0x5995); mem_wr(gb, DE, A);
  CYC(0x5995, 0x5998); A = W8(w1Companion_id);
  CYC(0x5998, 0x599a); alu_sub(gb, 0x0b); // SPECIALOBJECT_RICKY
  CYC(0x599a, 0x599c); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x599c, 0x599d); mem_wr(gb, DE, A);
  CYC(0x599d, 0x599f); alu_add(gb, 0x46); // <wRickyState
  CYC(0x599f, 0x59a0); L = A;
  CYC(0x59a0, 0x59a2); H = 0xc6; // >wRickyState
  CYC(0x59a2, 0x59a4); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x59a4, 0x59a7); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(0x59a4, 0x59a7);
  RET(0x59a7); return; // ret
}

void companionScript_runScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c2f, interactionRunScript_hook, 0x2552, 0x5c32);
  if (!(F & FC)) { RET_TAKEN(0x5c32); return; } // ret nc
  CYC(0x5c32, 0x5c33);
  CALL_C(0x5c33, setStatusBarNeedsRefreshBit1_hook, 0x17d8, 0x5c36);
  companionScript_delete_hook(gb); return;
}

void companionScript_subid00_state1_hook(GB *gb) {
  // If var3a is nonzero, make Moosh shake in fear
  CYC(0x5c1b, 0x5c1d); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x5c1d, 0x5c1e); A = mem_rd(gb, DE);
  CYC(0x5c1e, 0x5c1f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5c1f, 0x5c21); companionScript_runScript_hook(gb); return; } // jr z
  CYC(0x5c1f, 0x5c21);
  CYC(0x5c21, 0x5c22); A = alu_dec8(gb, A);
  CYC(0x5c22, 0x5c23); mem_wr(gb, DE, A);
  CYC(0x5c23, 0x5c25); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x5c25, 0x5c27); companionScript_runScript_hook(gb); return; } // jr nz
  CYC(0x5c25, 0x5c27);
  CYC(0x5c27, 0x5c2a); A = W8(w1Companion_xh);
  CYC(0x5c2a, 0x5c2c); alu_xor(gb, 0x02);
  CYC(0x5c2c, 0x5c2f); W8(w1Companion_xh) = A;
  companionScript_runScript_hook(gb); return;
}

void companionScript_subid01_hook(GB *gb) {
  CYC(0x5966, 0x5968); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5968, 0x5969); A = mem_rd(gb, DE);
  CYC(0x5969, 0x596a); push_effect(gb, 0x596a);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x59a8) { companionScript_restrictHigherX_hook(gb); return; }
  companionScript_genericState0_hook(gb); return;
}

void companionScript_subid02_hook(GB *gb) {
  CYC(0x596e, 0x5970); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5970, 0x5971); A = mem_rd(gb, DE);
  CYC(0x5971, 0x5972); push_effect(gb, 0x5972);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x59b5) { companionScript_restrictLowerY_hook(gb); return; }
  companionScript_genericState0_hook(gb); return;
}

void companionScript_subid04_hook(GB *gb) {
  CYC(0x5976, 0x5978); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5978, 0x5979); A = mem_rd(gb, DE);
  CYC(0x5979, 0x597a); push_effect(gb, 0x597a);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x59bb) { companionScript_restrictHigherY_hook(gb); return; }
  companionScript_genericState0_hook(gb); return;
}

void companionScript_subid05_hook(GB *gb) {
  CYC(0x597e, 0x5980); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5980, 0x5981); A = mem_rd(gb, DE);
  CYC(0x5981, 0x5982); push_effect(gb, 0x5982);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x59af) { companionScript_restrictLowerX_hook(gb); return; }
  companionScript_genericState0_hook(gb); return;
}

void companionScript_subid00_hook(GB *gb) {
  CYC(0x5935, 0x5937); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5937, 0x5938); A = mem_rd(gb, DE);
  CYC(0x5938, 0x5939); push_effect(gb, 0x5939);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c1b) { companionScript_subid00_state1_hook(gb); return; }

  CYC(0x593d, 0x593f); A = 0x01;
  CYC(0x593f, 0x5940); mem_wr(gb, DE, A);
  CYC(0x5940, 0x5943); A = W8(wEssencesObtained);
  CYC(0x5943, 0x5945); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x5945, 0x5948); companionScript_deleteSelf_hook(gb); return; } // jp z
  CYC(0x5945, 0x5948);
  CYC(0x5948, 0x594b); A = mem_rd(gb, wGroup1RoomFlags + 0x79); // wPastRoomFlags+$79
  CYC(0x594b, 0x594d); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(0x594d, 0x5950); companionScript_deleteSelf_hook(gb); return; } // jp z
  CYC(0x594d, 0x5950);
  CYC(0x5950, 0x5953); A = W8(wMooshState);
  CYC(0x5953, 0x5955); alu_and(gb, 0x60);
  if (!(F & FZ)) { CYCT(0x5955, 0x5958); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(0x5955, 0x5958);
  CYC(0x5958, 0x595a); A = 0x01;
  CYC(0x595a, 0x595d); W8(wDisableScreenTransitions) = A;
  CYC(0x595d, 0x5960); W8(wDiggingUpEnemiesForbidden) = A;
  CYC(0x5960, 0x5963); SET_HL(0x7465); // mainScripts.companionScript_subid00Script
  CYC(0x5963, 0x5966); interactionSetScript_hook(gb); return; // jp
}

// Ricky script when he loses his gloves
void companionScript_subid03_hook(GB *gb) {
  CYC(0x5a45, 0x5a47); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5a47, 0x5a48); A = mem_rd(gb, DE);
  CYC(0x5a48, 0x5a49); push_effect(gb, 0x5a49);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }

  CYC(0x5a4d, 0x5a4f); A = 0x01;
  CYC(0x5a4f, 0x5a50); mem_wr(gb, DE, A);
  CYC(0x5a50, 0x5a53); SET_HL(wRickyState);
  CYC(0x5a53, 0x5a54); A = mem_rd(gb, HL);
  CYC(0x5a54, 0x5a56); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x5a56, 0x5a58); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x5a56, 0x5a58);
  CYC(0x5a58, 0x5a5b); SET_HL(0x74df); // mainScripts.companionScript_subid03Script
  CYC(0x5a5b, 0x5a5e); interactionSetScript_hook(gb); return; // jp
}

// Dimitri script where he's harrassed by tokays
void companionScript_subid07_hook(GB *gb) {
  CYC(0x5a5e, 0x5a60); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5a60, 0x5a61); A = mem_rd(gb, DE);
  CYC(0x5a61, 0x5a62); push_effect(gb, 0x5a62);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }

  CYC(0x5a66, 0x5a69); A = W8(wDimitriState);
  CYC(0x5a69, 0x5a6b); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x5a6b, 0x5a6d); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x5a6b, 0x5a6d);
  CYC(0x5a6d, 0x5a6f); A = 0x01;
  CYC(0x5a6f, 0x5a70); mem_wr(gb, DE, A);
  CYC(0x5a70, 0x5a73); SET_HL(0x74e3); // mainScripts.companionScript_subid07Script
  CYC(0x5a73, 0x5a76); interactionSetScript_hook(gb); return; // jp
}

// Dimitri script where he leaves Link after bringing him to the mainland
void companionScript_subid06_hook(GB *gb) {
  CYC(0x5a76, 0x5a78); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5a78, 0x5a79); A = mem_rd(gb, DE);
  CYC(0x5a79, 0x5a7a); push_effect(gb, 0x5a7a);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }

  // Delete self if dimitri isn't here or the event has happened already
  CYC(0x5a7e, 0x5a81); A = W8(wDimitriState);
  CYC(0x5a81, 0x5a83); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x5a83, 0x5a85); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x5a83, 0x5a85);
  CYC(0x5a85, 0x5a88); SET_HL(w1Companion_id);
  CYC(0x5a88, 0x5a89); A = mem_rd(gb, HL);
  CYC(0x5a89, 0x5a8b); alu_cp(gb, 0x0c); // SPECIALOBJECT_DIMITRI
  if (!(F & FZ)) { CYCT(0x5a8b, 0x5a8d); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x5a8b, 0x5a8d);
  // Return if Dimitri's still in the water
  CYC(0x5a8d, 0x5a8f); L = 0x38; // SpecialObject.var38
  CYC(0x5a8f, 0x5a90); A = mem_rd(gb, HL);
  CYC(0x5a90, 0x5a91); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5a91); return; } // ret nz
  CYC(0x5a91, 0x5a92);
  CYC(0x5a92, 0x5a94); A = 0x01;
  CYC(0x5a94, 0x5a95); mem_wr(gb, DE, A);
  CYC(0x5a95, 0x5a98); W8(wDisableScreenTransitions) = A;
  // Manipulate Dimitri's state to force a dismount
  CYC(0x5a98, 0x5a9a); L = 0x03; // SpecialObject.var03
  CYC(0x5a9a, 0x5a9c); mem_wr(gb, HL, 0x02);
  CYC(0x5a9c, 0x5a9d); L = alu_inc8(gb, L);
  CYC(0x5a9d, 0x5a9f); mem_wr(gb, HL, 0x0a);
  CYC(0x5a9f, 0x5aa2); SET_HL(0x74e7); // mainScripts.companionScript_subid06Script
  CYC(0x5aa2, 0x5aa5); interactionSetScript_hook(gb); return; // jp
}

// A fairy appears to tell you about the animal companion in the forest
void companionScript_subid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5aa8, 0x5aaa); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5aaa, 0x5aab); A = mem_rd(gb, DE);
  CYC(0x5aab, 0x5aac); push_effect(gb, 0x5aac);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5ae7) goto state1;
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }

  // Clear $10 bytes starting at $cfd0
  CYC(0x5ab2, 0x5ab5); SET_HL(wTmpcfc0_fairyHideAndSeek_active);
  CYC(0x5ab5, 0x5ab7); B = 0x10;
  CALL_C(0x5ab7, clearMemory_hook, 0x046f, 0x5aba);
  CYC(0x5aba, 0x5abc); A = 0x1d; // GLOBALFLAG_CAN_BUY_FLUTE
  CALL_C(0x5abc, unsetGlobalFlag_hook, 0x31ff, 0x5abf);
  CYC(0x5abf, 0x5ac1); L = 0x10; // <wAnimalCompanion
  CYC(0x5ac1, 0x5ac2); A = mem_rd(gb, HL);
  CYC(0x5ac2, 0x5ac3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5ac3, 0x5ac5); goto l5ac8; } // jr nz
  CYC(0x5ac3, 0x5ac5);
  CYC(0x5ac5, 0x5ac7); A = 0x0d; // SPECIALOBJECT_MOOSH
  CYC(0x5ac7, 0x5ac8); mem_wr(gb, HL, A);

l5ac8:
  CYC(0x5ac8, 0x5aca); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(0x5aca, 0x5acc); alu_add(gb, 0x23); // <TX_1123
  CYC(0x5acc, 0x5acf); W8(wTextSubstitutions) = A;
  CYC(0x5acf, 0x5ad2); A = W8(wScreenTransitionDirection);
  CYC(0x5ad2, 0x5ad4); alu_cp(gb, 0x03); // DIR_LEFT
  if (!(F & FZ)) { CYCT(0x5ad4, 0x5ad6); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x5ad4, 0x5ad6);
  CYC(0x5ad6, 0x5ad8); A = 0x22; // GLOBALFLAG_TALKED_TO_HEAD_CARPENTER
  CALL_C(0x5ad8, checkGlobalFlag_hook, 0x31f3, 0x5adb);
  if (F & FZ) { CYCT(0x5adb, 0x5add); companionScript_deleteSelf_hook(gb); return; } // jr z
  CYC(0x5adb, 0x5add);
  CALL_C(0x5add, getThisRoomFlags_hook, 0x197d, 0x5ae0);
  CYC(0x5ae0, 0x5ae2); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x5ae2, 0x5ae4); companionScript_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x5ae2, 0x5ae4);
  CYC(0x5ae4, 0x5ae7); interactionIncState_hook(gb); return; // jp

state1:
  CYC(0x5ae7, 0x5aea); A = W8(w1Link_xh);
  CYC(0x5aea, 0x5aec); alu_cp(gb, 0x50);
  if (!(F & FC)) { RET_TAKEN(0x5aec); return; } // ret nc
  CYC(0x5aec, 0x5aed);
  CYC(0x5aed, 0x5aef); A = 0x81;
  CYC(0x5aef, 0x5af2); W8(wMenuDisabled) = A;
  CYC(0x5af2, 0x5af5); W8(wDisabledObjects) = A;
  CALL_C(0x5af5, putLinkOnGround_hook, 0x2a8c, 0x5af8);
  CYC(0x5af8, 0x5afb); SET_BC(0x4903); // INTERAC_FOREST_FAIRY, $03
  CALL_C(0x5afb, objectCreateInteraction_hook, 0x24c5, 0x5afe);
  CYC(0x5afe, 0x5b00); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5b00, 0x5b02); mem_wr(gb, HL, 0x0f);
  CYC(0x5b02, 0x5b05); SET_HL(0x7502); // mainScripts.companionScript_subid08Script
  CALL_C(0x5b05, interactionSetScript_hook, 0x2544, 0x5b08);
  CYC(0x5b08, 0x5b0b); interactionIncState_hook(gb); return; // jp
}

// Companion script where they're found in the fairy forest
void companionScript_subid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b0b, 0x5b0d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5b0d, 0x5b0e); A = mem_rd(gb, DE);
  CYC(0x5b0e, 0x5b0f); push_effect(gb, 0x5b0f);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }

  CYC(0x5b13, 0x5b15); A = 0x01;
  CYC(0x5b15, 0x5b16); mem_wr(gb, DE, A);
  CYC(0x5b16, 0x5b17); alu_xor(gb, A);
  CYC(0x5b17, 0x5b1a); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  // Check whether the event is applicable right now
  CYC(0x5b1a, 0x5b1c); A = 0x22; // GLOBALFLAG_TALKED_TO_HEAD_CARPENTER
  CALL_C(0x5b1c, checkGlobalFlag_hook, 0x31f3, 0x5b1f);
  if (F & FZ) { CYCT(0x5b1f, 0x5b21); companionScript_deleteSelf_hook(gb); return; } // jr z
  CYC(0x5b1f, 0x5b21);
  CYC(0x5b21, 0x5b23); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(0x5b23, checkGlobalFlag_hook, 0x31f3, 0x5b26);
  if (!(F & FZ)) { CYCT(0x5b26, 0x5b29); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(0x5b26, 0x5b29);
  // Put companion index (0-2) in var39
  CYC(0x5b29, 0x5b2c); SET_HL(wAnimalCompanion);
  CYC(0x5b2c, 0x5b2d); A = mem_rd(gb, HL);
  CYC(0x5b2d, 0x5b2f); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(0x5b2f, 0x5b31); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5b31, 0x5b32); mem_wr(gb, DE, A);
  CYC(0x5b32, 0x5b33); C = A;
  CYC(0x5b33, 0x5b36); SET_HL(0x5b61); // @animationWhenNoticingLink
  CYC(0x5b36, 0x5b37); companionScripts_addAToHl_from_rst(gb, 0x5b37);
  CYC(0x5b37, 0x5b38); A = mem_rd(gb, HL);
  CYC(0x5b38, 0x5b3a); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5b3a, 0x5b3b); mem_wr(gb, DE, A);
  CYC(0x5b3b, 0x5b3c); A = C;
  CYC(0x5b3c, 0x5b3d); alu_add(gb, A); // add a
  CYC(0x5b3d, 0x5b40); SET_HL(0x5b55); // @data1
  CYC(0x5b40, 0x5b41); companionScripts_add_double_index(gb, 0x5b41);
  CYC(0x5b41, 0x5b42); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5b42, 0x5b45); W8(wTextSubstitutions) = A;
  CALL_C(0x5b45, checkIsLinkedGame_hook, 0x1992, 0x5b48);
  if (F & FZ) { CYCT(0x5b48, 0x5b4a); goto l5b4b; } // jr z
  CYC(0x5b48, 0x5b4a);
  CYC(0x5b4a, 0x5b4b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)

l5b4b:
  CYC(0x5b4b, 0x5b4c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5b4c, 0x5b4f); mem_wr(gb, wTextSubstitutions + 1, A);
  CYC(0x5b4f, 0x5b52); SET_HL(0x7506); // mainScripts.companionScript_subid09Script
  CYC(0x5b52, 0x5b55); interactionSetScript_hook(gb); return; // jp
}

// Script just outside the forest, where you get the flute
void companionScript_subid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b64, 0x5b66); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5b66, 0x5b67); A = mem_rd(gb, DE);
  CYC(0x5b67, 0x5b68); push_effect(gb, 0x5b68);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }
  if (target == 0x5c39) { companionScript_subid0a_state2_hook(gb); return; }
  if (target == 0x5c9d) { companionScript_subid0a_state3_hook(gb); return; }

  CYC(0x5b70, 0x5b72); A = 0x24; // GLOBALFLAG_SAVED_COMPANION_FROM_FOREST
  CALL_C(0x5b72, checkGlobalFlag_hook, 0x31f3, 0x5b75);
  if (F & FZ) { CYCT(0x5b75, 0x5b78); companionScript_delete_hook(gb); return; } // jp z
  CYC(0x5b75, 0x5b78);
  CYC(0x5b78, 0x5b7a); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(0x5b7a, checkGlobalFlag_hook, 0x31f3, 0x5b7d);
  if (!(F & FZ)) { CYCT(0x5b7d, 0x5b80); companionScript_delete_hook(gb); return; } // jp nz
  CYC(0x5b7d, 0x5b80);
  CYC(0x5b80, 0x5b82); A = 0x01; // [state] = 1
  CYC(0x5b82, 0x5b83); mem_wr(gb, DE, A);
  CYC(0x5b83, 0x5b86); W8(wMenuDisabled) = A;
  CYC(0x5b86, 0x5b89); W8(wDisabledObjects) = A;
  CYC(0x5b89, 0x5b8c); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  CYC(0x5b8c, 0x5b8e); A = 0x00; // DIR_UP
  CYC(0x5b8e, 0x5b91); W8(w1Link_direction) = A;
  // Put companion index (0-2) in var39
  CYC(0x5b91, 0x5b94); SET_HL(wAnimalCompanion);
  CYC(0x5b94, 0x5b95); A = mem_rd(gb, HL);
  CYC(0x5b95, 0x5b97); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(0x5b97, 0x5b99); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5b99, 0x5b9a); mem_wr(gb, DE, A);
  // Determine text to show for this companion
  CYC(0x5b9a, 0x5b9b); alu_add(gb, A); // add a
  CYC(0x5b9b, 0x5b9e); SET_HL(0x5bc5); // @textIndices
  CYC(0x5b9e, 0x5b9f); companionScripts_add_double_index(gb, 0x5b9f);
  CYC(0x5b9f, 0x5ba0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5ba0, 0x5ba3); mem_wr(gb, wTextSubstitutions + 1, A);
  CALL_C(0x5ba3, checkIsLinkedGame_hook, 0x1992, 0x5ba6);
  if (F & FZ) { CYCT(0x5ba6, 0x5ba8); goto l5ba9; } // jr z
  CYC(0x5ba6, 0x5ba8);
  CYC(0x5ba8, 0x5ba9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)

l5ba9:
  CYC(0x5ba9, 0x5baa); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5baa, 0x5bad); W8(wTextSubstitutions) = A;
  // Spawn in the fairies
  CYC(0x5bad, 0x5bb0); SET_BC(0x1103);

nextFairy:
  CYC(0x5bb0, 0x5bb1); push_effect(gb, BC);
  CYC(0x5bb1, 0x5bb4); SET_BC(0x4904); // INTERAC_FOREST_FAIRY, $04
  CALL_C(0x5bb4, objectCreateInteraction_hook, 0x24c5, 0x5bb7);
  CYC(0x5bb7, 0x5bb8); SET_BC(pop_effect(gb));
  CYC(0x5bb8, 0x5bba); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5bba, 0x5bbb); mem_wr(gb, HL, B);
  CYC(0x5bbb, 0x5bbc); B = alu_inc8(gb, B);
  CYC(0x5bbc, 0x5bbd); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x5bbd, 0x5bbf); goto nextFairy; } // jr nz
  CYC(0x5bbd, 0x5bbf);
  CYC(0x5bbf, 0x5bc2); SET_HL(0x750a); // mainScripts.companionScript_subid0aScript
  CYC(0x5bc2, 0x5bc5); interactionSetScript_hook(gb); return; // jp
}

// This is the part which gives Link the flute.
void companionScript_subid0a_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c39, 0x5c3b); A = 0x0e; // TREASURE_FLUTE
  CALL_C(0x5c3b, checkTreasureObtained_hook, 0x1748, 0x5c3e);
  CYC(0x5c3e, 0x5c40); C = 0x38; // <TX_0038
  if (!(F & FC)) { CYCT(0x5c40, 0x5c42); goto l5c44; } // jr nc
  CYC(0x5c40, 0x5c42);
  CYC(0x5c42, 0x5c44); C = 0x69; // <TX_0069

l5c44:
  CYC(0x5c44, 0x5c46); E = INTERACTION_BASE + OBJ_VAR39; // Companion index
  CYC(0x5c46, 0x5c47); A = mem_rd(gb, DE);
  CYC(0x5c47, 0x5c48); alu_add(gb, C);
  CYC(0x5c48, 0x5c49); C = A;
  CYC(0x5c49, 0x5c4b); B = 0x00; // >TX_0038
  CALL_C(0x5c4b, showText_hook, 0x1872, 0x5c4e);
  CYC(0x5c4e, 0x5c50); A = 0x01;
  CYC(0x5c50, 0x5c53); W8(wMenuDisabled) = A;
  CALL_C(0x5c53, interactionIncState_hook, 0x23e0, 0x5c56);
  // Set wFluteIcon
  CYC(0x5c56, 0x5c58); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5c58, 0x5c59); A = mem_rd(gb, DE);
  CYC(0x5c59, 0x5c5a); C = A;
  CYC(0x5c5a, 0x5c5b); A = alu_inc8(gb, A);
  CYC(0x5c5b, 0x5c5c); mem_wr(gb, DE, A);
  CYC(0x5c5c, 0x5c5f); SET_HL(wFluteIcon);
  CYC(0x5c5f, 0x5c60); mem_wr(gb, HL, A);
  // Set bit 7 of wRickyState / wDimitriState / wMooshState
  CYC(0x5c60, 0x5c62); alu_add(gb, 0x45); // <wCompanionStates - 1
  CYC(0x5c62, 0x5c63); L = A;
  CYC(0x5c63, 0x5c65); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80); // set 7,(hl)
  // Give flute
  CYC(0x5c65, 0x5c67); A = 0x0e; // TREASURE_FLUTE
  CALL_C(0x5c67, giveTreasure_hook, 0x171c, 0x5c6a);
  CYC(0x5c6a, 0x5c6d); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x5c6d, 0x5c6f); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01); // set 0,(hl)
  // Turn this object into the flute graphic?
  CYC(0x5c6f, 0x5c71); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5c71, 0x5c72); alu_xor(gb, A);
  CYC(0x5c72, 0x5c73); mem_wr(gb, DE, A);
  CALL_C(0x5c73, interactionInitGraphics_hook, 0x15fb, 0x5c76);
  CYC(0x5c76, 0x5c78); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5c78, 0x5c7a); A = 0x0a;
  CYC(0x5c7a, 0x5c7b); mem_wr(gb, DE, A);
  // Set this object's palette
  CYC(0x5c7b, 0x5c7d); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5c7d, 0x5c7e); A = mem_rd(gb, DE);
  CYC(0x5c7e, 0x5c7f); C = A;
  CYC(0x5c7f, 0x5c81); alu_and(gb, 0x01);
  CYC(0x5c81, 0x5c82); alu_add(gb, A); // add a
  CYC(0x5c82, 0x5c83); alu_xor(gb, C);
  CYC(0x5c83, 0x5c85); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x5c85, 0x5c86); mem_wr(gb, DE, A);
  // Set this object's position
  CYC(0x5c86, 0x5c89); SET_HL(w1Link);
  CYC(0x5c89, 0x5c8c); SET_BC(0xf200);
  CALL_C(0x5c8c, objectTakePositionWithOffset_hook, 0x2277, 0x5c8f);
  // Make Link hold it over his head
  CYC(0x5c8f, 0x5c92); SET_HL(wLinkForceState);
  CYC(0x5c92, 0x5c94); A = 0x04; // LINK_STATE_04
  CYC(0x5c94, 0x5c95); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5c95, 0x5c97); mem_wr(gb, HL, 0x01); // [wcc50] = $01
  CALL_C(0x5c97, objectSetVisible80_hook, 0x1e57, 0x5c9a);
  CYC(0x5c9a, 0x5c9d); interactionRunScript_hook(gb); return; // jp
}

void companionScript_subid0a_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c9d, retIfTextIsActive_hook, 0x1859, 0x5ca0);
  // ??
  CYC(0x5ca0, 0x5ca3); A = W8(wLinkObjectIndex);
  CYC(0x5ca3, 0x5ca5); alu_and(gb, 0x0f);
  CYC(0x5ca5, 0x5ca6); alu_add(gb, A); // add a
  CYC(0x5ca6, 0x5ca8); alu_swap_a(gb);
  CYC(0x5ca8, 0x5cab); W8(wDisabledObjects) = A;
  // Make flute disappear, wait for script to end
  CALL_C(0x5cab, objectSetInvisible_hook, 0x1e7b, 0x5cae);
  CALL_C(0x5cae, interactionRunScript_hook, 0x2552, 0x5cb1);
  if (!(F & FC)) { RET_TAKEN(0x5cb1); return; } // ret nc
  CYC(0x5cb1, 0x5cb2);
  // Clean up, delete self
  CYC(0x5cb2, 0x5cb3); alu_xor(gb, A);
  CYC(0x5cb3, 0x5cb6); W8(wDisabledObjects) = A;
  CYC(0x5cb6, 0x5cb9); W8(wMenuDisabled) = A;
  CYC(0x5cb9, 0x5cbc); companionScript_delete_hook(gb); return; // jp
}

// Script in first screen of forest, where fairy leads you to the companion
void companionScript_subid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5bd1, 0x5bd3); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5bd3, 0x5bd4); A = mem_rd(gb, DE);
  CYC(0x5bd4, 0x5bd5); push_effect(gb, 0x5bd5);
  uint16_t target = companionScripts_jump_table(gb);
  if (target == 0x5c2f) { companionScript_runScript_hook(gb); return; }

  CYC(0x5bd9, 0x5bdc); A = W8(wScreenTransitionDirection);
  CYC(0x5bdc, 0x5bde); alu_cp(gb, 0x02); // DIR_DOWN
  if (!(F & FZ)) { CYCT(0x5bde, 0x5be0); companionScript_delete_hook(gb); return; } // jr nz
  CYC(0x5bde, 0x5be0);
  CYC(0x5be0, 0x5be2); A = 0x42; // GLOBALFLAG_COMPANION_LOST_IN_FOREST
  CALL_C(0x5be2, checkGlobalFlag_hook, 0x31f3, 0x5be5);
  if (F & FZ) { CYCT(0x5be5, 0x5be7); companionScript_delete_hook(gb); return; } // jr z
  CYC(0x5be5, 0x5be7);
  CYC(0x5be7, 0x5be9); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(0x5be9, checkGlobalFlag_hook, 0x31f3, 0x5bec);
  if (!(F & FZ)) { CYCT(0x5bec, 0x5bee); companionScript_delete_hook(gb); return; } // jr nz
  CYC(0x5bec, 0x5bee);
  CYC(0x5bee, 0x5bf1); SET_BC(0x4903); // INTERAC_FOREST_FAIRY, $03
  CALL_C(0x5bf1, objectCreateInteraction_hook, 0x24c5, 0x5bf4);
  CYC(0x5bf4, 0x5bf6); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5bf6, 0x5bf8); mem_wr(gb, HL, 0x14);
  CYC(0x5bf8, 0x5bfa); A = 0x81;
  CYC(0x5bfa, 0x5bfd); W8(wMenuDisabled) = A;
  CYC(0x5bfd, 0x5c00); W8(wDisabledObjects) = A;
  CYC(0x5c00, 0x5c01); alu_xor(gb, A);
  CYC(0x5c01, 0x5c04); W8(wTmpcfc0_fairyHideAndSeek_cfd2) = A;
  CYC(0x5c04, 0x5c07); SET_HL(0x750e); // mainScripts.companionScript_subid0bScript
  CALL_C(0x5c07, interactionSetScript_hook, 0x2544, 0x5c0a);
  CYC(0x5c0a, 0x5c0d); interactionIncState_hook(gb); return; // jp
}

// Sets bit 6 of wDimitriState so he disappears from Tokay Island
void companionScript_subid0c_hook(GB *gb) {
  CYC(0x5c0d, 0x5c10); A = W8(wDimitriState);
  CYC(0x5c10, 0x5c12); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(0x5c12, 0x5c14); companionScript_delete_hook(gb); return; } // jr z
  CYC(0x5c12, 0x5c14);
  CYC(0x5c14, 0x5c16); alu_or(gb, 0x40);
  CYC(0x5c16, 0x5c19); W8(wDimitriState) = A;
  CYC(0x5c19, 0x5c1b); companionScript_delete_hook(gb); return; // jr
}

// Companion barrier to Symmetry City, until the tuni nut is restored
void companionScript_subid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59f1, 0x59f3); A = 0x29; // GLOBALFLAG_TUNI_NUT_PLACED
  CALL_C(0x59f3, checkGlobalFlag_hook, 0x31f3, 0x59f6);
  if (!(F & FZ)) { CYCT(0x59f6, 0x59f9); companionScript_deleteSelf_hook(gb); return; } // jp nz
  CYC(0x59f6, 0x59f9);
  CYC(0x59f9, 0x59fc); A = W8(wScrollMode);
  CYC(0x59fc, 0x59fe); alu_and(gb, 0x0e); // SCROLLMODE_08|SCROLLMODE_04|SCROLLMODE_02
  if (!(F & FZ)) { RET_TAKEN(0x59fe); return; } // ret nz
  CYC(0x59fe, 0x59ff);
  CYC(0x59ff, 0x5a02); SET_HL(0xd100); // w1Companion.enabled
  CYC(0x5a02, 0x5a03); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5a03, 0x5a04); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5a04); return; } // ret z
  CYC(0x5a04, 0x5a05);
  CYC(0x5a05, 0x5a06); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5a06, 0x5a08); alu_cp(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  if (F & FC) { RET_TAKEN(0x5a08); return; } // ret c
  CYC(0x5a08, 0x5a09);
  CYC(0x5a09, 0x5a0b); alu_cp(gb, 0x0e); // SPECIALOBJECT_LAST_COMPANION+1
  if (!(F & FC)) { RET_TAKEN(0x5a0b); return; } // ret nc
  CYC(0x5a0b, 0x5a0c);
  // Check if the companion is roughly at this object's position
  CYC(0x5a0c, 0x5a0e); L = 0x0d; // SpecialObject.xh
  CYC(0x5a0e, 0x5a10); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5a10, 0x5a11); A = mem_rd(gb, DE);
  CYC(0x5a11, 0x5a12); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5a12, 0x5a14); alu_add(gb, 0x05);
  CYC(0x5a14, 0x5a16); alu_cp(gb, 0x0b);
  if (!(F & FC)) { RET_TAKEN(0x5a16); return; } // ret nc
  CYC(0x5a16, 0x5a17);
  CYC(0x5a17, 0x5a19); L = 0x0b; // SpecialObject.yh
  CYC(0x5a19, 0x5a1b); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5a1b, 0x5a1c); A = mem_rd(gb, DE);
  CYC(0x5a1c, 0x5a1d); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { RET_TAKEN(0x5a1d); return; } // ret c
  CYC(0x5a1d, 0x5a1e);
  // If so, prevent companion from moving any further up
  CYC(0x5a1e, 0x5a1f); A = alu_inc8(gb, A);
  CYC(0x5a1f, 0x5a20); mem_wr(gb, HL, A);
  CYC(0x5a20, 0x5a22); L = 0x10; // SpecialObject.speed
  CYC(0x5a22, 0x5a24); mem_wr(gb, HL, 0x00);
  CYC(0x5a24, 0x5a26); L = 0x04; // SpecialObject.state
  CYC(0x5a26, 0x5a27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  // If it's Dimitri being held, make Link drop him
  CYC(0x5a27, 0x5a29); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x5a29, 0x5a2b); goto l5a30; } // jr nz
  CYC(0x5a29, 0x5a2b);
  CYC(0x5a2b, 0x5a2d); mem_wr(gb, HL, 0x03);
  CALL_C(0x5a2d, dropLinkHeldItem_hook, 0x2c43, 0x5a30);

l5a30:
  CYC(0x5a30, 0x5a33); A = W8(wAnimalCompanion);
  CYC(0x5a33, 0x5a35); alu_sub(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(0x5a35, 0x5a38); SET_HL(0x5a3f); // @textIndices
  CYC(0x5a38, 0x5a39); companionScripts_add_double_index(gb, 0x5a39);
  CYC(0x5a39, 0x5a3a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5a3a, 0x5a3b); B = mem_rd(gb, HL);
  CYC(0x5a3b, 0x5a3c); C = A;
  CYC(0x5a3c, 0x5a3f); showText_hook(gb); return; // jp
}

// INTERAC_COMPANION_SCRIPTS
void interactionCode71_hook(GB *gb) {
  CYC(0x5908, 0x590b); A = W8(wLinkDeathTrigger);
  CYC(0x590b, 0x590c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x590c, 0x590e); goto l5915; } // jr z
  CYC(0x590c, 0x590e);
  CYC(0x590e, 0x590f); alu_xor(gb, A);
  CYC(0x590f, 0x5912); W8(wDisabledObjects) = A;
  CYC(0x5912, 0x5915); interactionDelete_hook(gb); return; // jp

l5915:
  CYC(0x5915, 0x5917); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5917, 0x5918); A = mem_rd(gb, DE);
  {
    CYC(0x5918, 0x5919); push_effect(gb, 0x5919);
    uint16_t target = companionScripts_jump_table(gb);
    if (target == 0x5966) { companionScript_subid01_hook(gb); return; }
    if (target == 0x596e) { companionScript_subid02_hook(gb); return; }
    if (target == 0x5a45) { companionScript_subid03_hook(gb); return; }
    if (target == 0x5976) { companionScript_subid04_hook(gb); return; }
    if (target == 0x597e) { companionScript_subid05_hook(gb); return; }
    if (target == 0x5a76) { companionScript_subid06_hook(gb); return; }
    if (target == 0x5a5e) { companionScript_subid07_hook(gb); return; }
    if (target == 0x5aa8) { companionScript_subid08_hook(gb); return; }
    if (target == 0x5b0b) { companionScript_subid09_hook(gb); return; }
    if (target == 0x5b64) { companionScript_subid0a_hook(gb); return; }
    if (target == 0x5bd1) { companionScript_subid0b_hook(gb); return; }
    if (target == 0x5c0d) { companionScript_subid0c_hook(gb); return; }
    if (target == 0x59f1) { companionScript_subid0d_hook(gb); return; }
    companionScript_subid00_hook(gb); return; // target == 0x5935
  }
}
