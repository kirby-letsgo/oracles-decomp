#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCoded0_jump_table(GB *gb) {
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

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded0_addAToHl(GB *gb, uint16_t return_address) {
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

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded0_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCoded0@cpYToCompanion (0b:4bee): true local, called (real `call`) twice from
// @setFlagAndDeleteWhenCompanionIsAbove and @setFlagAndDeleteWhenCompanionIsBelowOrRight.
static void interactionCoded0_cpYToCompanion(GB *gb) {
  CYC(0x4bee, 0x4bf0); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4bf0, 0x4bf1); A = mem_rd(gb, DE);
  CYC(0x4bf1, 0x4bf4); SET_HL(w1Companion_yh);
  CYC(0x4bf4, 0x4bf5); alu_cp(gb, mem_rd(gb, HL));
  RET(0x4bf5); return;
}

// interactionCoded0@checkLinkInXRange (0b:4bf6): true local, called (real `call`) once from
// @setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange.
// @param[out] zflag z if Link is within a certain range of X-positions for certain rooms?
static void interactionCoded0_checkLinkInXRange(GB *gb) {
  CYC(0x4bf6, 0x4bf9); A = mem_rd(gb, wActiveRoom);
  CYC(0x4bf9, 0x4bfc); SET_HL(0x4c18); // @rooms
  CYC(0x4bfc, 0x4bfe); B = 0x00;

roomLoop:
  CYC(0x4bfe, 0x4bff); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4bff, 0x4c01); goto foundRoom; } // jr z
  CYC(0x4bff, 0x4c01);
  CYC(0x4c01, 0x4c02); B = alu_inc8(gb, B);
  CYC(0x4c02, 0x4c03); SET_HL(HL + 1);
  CYC(0x4c03, 0x4c05); goto roomLoop; // jr $4bfe

foundRoom:
  CYC(0x4c05, 0x4c06); A = B;
  CYC(0x4c06, 0x4c09); SET_HL(0x4c1b); // @xRanges
  CYC(0x4c09, 0x4c0a); interactionCoded0_addDoubleIndex(gb, 0x4c0a);
  CYC(0x4c0a, 0x4c0d); A = mem_rd(gb, w1Link_xh);
  CYC(0x4c0d, 0x4c0e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x4c0e, 0x4c10); goto notInRange; } // jr c
  CYC(0x4c0e, 0x4c10);
  CYC(0x4c10, 0x4c11); SET_HL(HL + 1);
  CYC(0x4c11, 0x4c12); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x4c12, 0x4c14); goto notInRange; } // jr nc
  CYC(0x4c12, 0x4c14);
  CYC(0x4c14, 0x4c15); alu_xor(gb, A);
  RET(0x4c15); return;

notInRange:
  CYC(0x4c16, 0x4c17); alu_or(gb, D);
  RET(0x4c17); return;
}

// ==================================================================================================
// INTERAC_COMPANION_TUTORIAL
//
// @state2's inner jump table has 6 entries for the ages build; two of them (index 2 and 5) both
// target @setFlagAndDeleteWhenCompanionIsBelow, which collapses to a single switch case here.
// @setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero is dead in the ages build (no jump-table
// entry reaches it) but its bytes are still present in ROM, falling straight through into
// @setFlagAndDeleteWhenCompanionIsAbove; ported below for completeness even though unreachable.
// ==================================================================================================
void interactionCoded0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b34, 0x4b36); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4b36, 0x4b37); A = mem_rd(gb, DE);
  CYC(0x4b37, 0x4b38); push_effect(gb, 0x4b38);
  switch (interactionCoded0_jump_table(gb)) {
    case 0x4b3e: goto state0;
    case 0x4b42: goto state1;
    case 0x4b97: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4b3e, 0x4b40); A = 0x01;
  CYC(0x4b40, 0x4b41); mem_wr(gb, DE, A);
  RET(0x4b41); return;

state1:
  CYC(0x4b42, 0x4b44); A = 0x02;
  CYC(0x4b44, 0x4b45); mem_wr(gb, DE, A);
  CYC(0x4b45, 0x4b48); A = mem_rd(gb, w1Companion_enabled);
  CYC(0x4b48, 0x4b49); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4b49, 0x4b4b); goto deleteIfSubid2Or5; } // jr z
  CYC(0x4b49, 0x4b4b);

  // Verify that the correct companion is on-screen, otherwise delete self
  CYC(0x4b4b, 0x4b4d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4b4d, 0x4b4e); A = mem_rd(gb, DE);
  CYC(0x4b4e, 0x4b50); A = alu_srl(gb, A);
  CYC(0x4b50, 0x4b52); alu_add(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(0x4b52, 0x4b54); alu_cp(gb, 0x0e); // SPECIALOBJECT_LAST_COMPANION+1
  if (F & FC) { CYCT(0x4b54, 0x4b56); goto afterMoosh; } // jr c
  CYC(0x4b54, 0x4b56);
  CYC(0x4b56, 0x4b58); A = 0x0d; // SPECIALOBJECT_MOOSH

afterMoosh:
  CYC(0x4b58, 0x4b5b); SET_HL(w1Companion_id);
  CYC(0x4b5b, 0x4b5c); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4b5c, 0x4b5e); goto delete; } // jr nz

  // Delete self if tutorial text was already shown
  CYC(0x4b5c, 0x4b5e);
  CYC(0x4b5e, 0x4b5f); A = mem_rd(gb, DE);
  CYC(0x4b5f, 0x4b62); SET_HL(0x4c2d); // @flagNumbers
  CYC(0x4b62, 0x4b63); interactionCoded0_addAToHl(gb, 0x4b63);
  CYC(0x4b63, 0x4b64); A = mem_rd(gb, HL);
  CYC(0x4b64, 0x4b67); SET_HL(wCompanionTutorialTextShown);
  CALL_C(0x4b67, checkFlag_hook, 0x0205, 0x4b6a);
  if (!(F & FZ)) { CYCT(0x4b6a, 0x4b6c); goto delete; } // jr nz
  CYC(0x4b6a, 0x4b6c);

  // Check whether to dismount? (subid 2 only)
  CYC(0x4b6c, 0x4b6d); A = mem_rd(gb, DE);
  CYC(0x4b6d, 0x4b6f); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4b6f, 0x4b71); goto afterDismountCheck; } // jr nz
  CYC(0x4b6f, 0x4b71);
  CYC(0x4b71, 0x4b74); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x4b74, 0x4b75); alu_rra(gb);
  CYC(0x4b75, 0x4b76); A = mem_rd(gb, DE);
  if (!(F & FC)) { CYCT(0x4b76, 0x4b78); goto afterDismountCheck; } // jr nc
  CYC(0x4b76, 0x4b78);
  CYC(0x4b78, 0x4b7b); mem_wr(gb, wForceCompanionDismount, A);

afterDismountCheck:
  CYC(0x4b7b, 0x4b7e); SET_HL(0x4c21); // @tutorialTextToShow
  CYC(0x4b7e, 0x4b7f); interactionCoded0_addDoubleIndex(gb, 0x4b7f);
  CYC(0x4b7f, 0x4b80); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4b80, 0x4b81); C = A;
  CYC(0x4b81, 0x4b82); B = mem_rd(gb, HL);
  CYC(0x4b82, 0x4b85); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x4b85, 0x4b87); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(0x4b87, showText_hook, 0x1872, 0x4b8a); else CYC(0x4b87, 0x4b8a); // call nz

deleteIfSubid2Or5:
  CYC(0x4b8a, 0x4b8c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4b8c, 0x4b8d); A = mem_rd(gb, DE);
  CYC(0x4b8d, 0x4b8f); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4b8f, 0x4b91); goto delete; } // jr z
  CYC(0x4b8f, 0x4b91);
  CYC(0x4b91, 0x4b93); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(0x4b93, 0x4b94); ret_effect(gb); return; } // ret nz
  CYC(0x4b93, 0x4b94);

delete:
  CYC(0x4b94, 0x4b97); interactionDelete_hook(gb); return; // jp

state2:
  CYC(0x4b97, 0x4b9a); A = mem_rd(gb, w1Companion_enabled);
  CYC(0x4b9a, 0x4b9b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4b9b, 0x4b9c); ret_effect(gb); return; } // ret z
  CYC(0x4b9b, 0x4b9c);
  CYC(0x4b9c, 0x4b9e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4b9e, 0x4b9f); A = mem_rd(gb, DE);
  CYC(0x4b9f, 0x4ba0); push_effect(gb, 0x4ba0);
  switch (interactionCoded0_jump_table(gb)) {
    case 0x4bdf: goto setFlagAndDeleteWhenCompanionIsBelowOrRight;
    case 0x4bbb: goto setFlagAndDeleteWhenCompanionIsAbove;
    case 0x4bac: goto setFlagAndDeleteWhenCompanionIsBelow;
    case 0x4bcf: goto setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange;
    case 0x4bd5: goto setFlagAndDeleteWhenCompanionIsLeft;
    default: hook_continue(gb, HL, sp0_); return;
  }

setFlagAndDeleteWhenCompanionIsBelow:
  CYC(0x4bac, 0x4bae); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4bae, 0x4baf); A = mem_rd(gb, DE);
  CYC(0x4baf, 0x4bb2); SET_HL(w1Companion_yh);
  CYC(0x4bb2, 0x4bb3); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x4bb3, 0x4bb4); ret_effect(gb); return; } // ret nc
  CYC(0x4bb3, 0x4bb4);
  CYC(0x4bb4, 0x4bb6); goto setFlagAndDelete; // jr $4bbf

  // setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero (0b:4bb6): dead in ages, falls through.
  CYC(0x4bb6, 0x4bb9); A = mem_rd(gb, w1Companion_var38);
  CYC(0x4bb9, 0x4bba); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4bba, 0x4bbb); ret_effect(gb); return; } // ret z
  CYC(0x4bba, 0x4bbb);

setFlagAndDeleteWhenCompanionIsAbove:
  CALL_C(0x4bbb, interactionCoded0_cpYToCompanion, 0x4bee, 0x4bbe);
  if (F & FC) { CYCT(0x4bbe, 0x4bbf); ret_effect(gb); return; } // ret c
  CYC(0x4bbe, 0x4bbf);

setFlagAndDelete:
  CYC(0x4bbf, 0x4bc1); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4bc1, 0x4bc2); A = mem_rd(gb, DE);
  CYC(0x4bc2, 0x4bc5); SET_HL(0x4c2d); // @flagNumbers
  CYC(0x4bc5, 0x4bc6); interactionCoded0_addAToHl(gb, 0x4bc6);
  CYC(0x4bc6, 0x4bc7); A = mem_rd(gb, HL);
  CYC(0x4bc7, 0x4bca); SET_HL(wCompanionTutorialTextShown);
  CALL_C(0x4bca, setFlag_hook, 0x020e, 0x4bcd);

  // goToDelete (falls through, never a goto target)
  CYC(0x4bcd, 0x4bcf); goto delete; // jr $4b94

setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange:
  CALL_C(0x4bcf, interactionCoded0_checkLinkInXRange, 0x4bf6, 0x4bd2);
  if (!(F & FZ)) { CYCT(0x4bd2, 0x4bd3); ret_effect(gb); return; } // ret nz
  CYC(0x4bd2, 0x4bd3);
  CYC(0x4bd3, 0x4bd5); goto setFlagAndDeleteWhenCompanionIsAbove; // jr $4bbb

setFlagAndDeleteWhenCompanionIsLeft:
  CYC(0x4bd5, 0x4bd7); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4bd7, 0x4bd8); A = mem_rd(gb, DE);
  CYC(0x4bd8, 0x4bdb); SET_HL(w1Companion_xh);
  CYC(0x4bdb, 0x4bdc); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x4bdc, 0x4bdd); ret_effect(gb); return; } // ret nc
  CYC(0x4bdc, 0x4bdd);
  CYC(0x4bdd, 0x4bdf); goto setFlagAndDelete; // jr $4bbf

setFlagAndDeleteWhenCompanionIsBelowOrRight:
  CALL_C(0x4bdf, interactionCoded0_cpYToCompanion, 0x4bee, 0x4be2);
  if (F & FC) { CYCT(0x4be2, 0x4be4); goto setFlagAndDelete; } // jr c
  CYC(0x4be2, 0x4be4);
  CYC(0x4be4, 0x4be6); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4be6, 0x4be7); A = mem_rd(gb, DE);
  CYC(0x4be7, 0x4bea); SET_HL(w1Companion_xh);
  CYC(0x4bea, 0x4beb); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x4beb, 0x4bec); ret_effect(gb); return; } // ret c
  CYC(0x4beb, 0x4bec);
  CYC(0x4bec, 0x4bee); goto setFlagAndDelete; // jr $4bbf
}
