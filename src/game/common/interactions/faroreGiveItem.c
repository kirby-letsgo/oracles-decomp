#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactiond9_jump_table(GB *gb) {
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

static void interactiond9_addAToHl(GB *gb, uint16_t return_address) {
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

static void interactiond9_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:4fae, interactiond9_state2@createTreasure. Reached by a genuine `call` from two
// substate call sites, each of which keeps doing work after it returns, so the resume is
// verified exactly like CALL_C does -- see src/game/decoration.c's @isSymmetryCityRoom for the
// same manual-check idiom. Push the caller's own return address, run the body -- it ends
// either in a real "ret nz" or a tail `jp objectCopyPosition` whose own ret consumes that same
// pushed value -- then the call site checks the resulting pc/sp before continuing.
static void interactiond9_state2_createTreasure(GB *gb, uint16_t sp0_, uint16_t return_address) {
  push_effect(gb, return_address);
  CALL_C(0x4fae, createTreasure_hook, 0x27d4, 0x4fb1);
  if (!(F & FZ)) { CYCT(0x4fb1, 0x4fb2); ret_effect(gb); return; } // ret nz
  CYC(0x4fb1, 0x4fb2);
  CYC(0x4fb2, 0x4fb5); objectCopyPosition_hook(gb); return; // jp
}

// 0b:4f65, interactiond9_state2@createTreasureAndIncSubstate. Reached by plain goto/fallthrough
// from @ringBoxUpgrade/@satchelUpgrade/@label_0b_135 -- tail position, no push there, matches a
// `jr`/fallthrough -- and by a genuine `call` from @bombUpgrade, which pushes its own return
// address before calling and checks pc/sp after -- same manual CALL_C-style idiom as
// @createTreasure above.
void interactiond9_state2_createTreasureAndIncSubstate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f65, 0x4f68); interactiond9_state2_createTreasure(gb, sp0_, 0x4f68);
  if (!(gb->pc == 0x4f68 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(0x4f68, 0x4f6a); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4f6a, 0x4f6c); A = 0x1e;
  CYC(0x4f6c, 0x4f6d); mem_wr(gb, DE, A);
  CYC(0x4f6d, 0x4f70); interactionIncSubstate_hook(gb); return; // jp
}

// 0b:4de4, called from interactionCoded9@state0.
void interactiond9_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4de4, 0x4de6); A = 0x01;
  CYC(0x4de6, 0x4de9); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(0x4de9, 0x4deb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4deb, 0x4dec); A = mem_rd(gb, DE);
  CYC(0x4dec, 0x4ded); B = A;
  CYC(0x4ded, 0x4def); alu_add(gb, 0x5a); // GLOBALFLAG_FIRST_AGES_DONE_SECRET
  CALL_C(0x4def, checkGlobalFlag_hook, 0x31f3, 0x4df2);
  if (F & FZ) { CYCT(0x4df2, 0x4df4); goto secretNotTold; } // jr z
  CYC(0x4df2, 0x4df4);
  CYC(0x4df4, 0x4df7); SET_BC(0x550c); // TX_550c
  CALL_C(0x4df7, showText_hook, 0x1872, 0x4dfa);
  CYC(0x4dfa, 0x4dfc); A = 0x02;
  CYC(0x4dfc, 0x4dff); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x4dff, 0x4e02); interactionDelete_hook(gb); return; // jp

secretNotTold:
  CYC(0x4e02, 0x4e03); A = B;
  CYC(0x4e03, 0x4e06); SET_HL(0x4e12); // @bits
  CALL_C(0x4e06, checkFlag_hook, 0x0205, 0x4e09);
  CYC(0x4e09, 0x4e0b); A = 0x02;
  if (!(F & FZ)) { CYCT(0x4e0b, 0x4e0d); goto skipDec; } // jr nz
  CYC(0x4e0b, 0x4e0d);
  CYC(0x4e0d, 0x4e0e); A = alu_dec8(gb, A);

skipDec:
  CYC(0x4e0e, 0x4e10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4e10, 0x4e11); mem_wr(gb, DE, A);
  RET(0x4e11); return;
}

// 0b:4e14, called from interactiond9_state1@substate0 and interactiond9_state2@substate6.
// @param[out] bc  The item ID. If this is an upgrade, c is 0-4 indicating the behaviour.
void interactiond9_getItemID_hook(GB *gb) {
  CYC(0x4e14, 0x4e16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4e16, 0x4e17); A = mem_rd(gb, DE);
  CYC(0x4e17, 0x4e1a); SET_HL(0x4e1f); // @chestContents
  CYC(0x4e1a, 0x4e1b); interactiond9_addDoubleIndex(gb, 0x4e1b);
  CYC(0x4e1b, 0x4e1c); B = mem_rd(gb, HL);
  CYC(0x4e1c, 0x4e1d); L = alu_inc8(gb, L);
  CYC(0x4e1d, 0x4e1e); C = mem_rd(gb, HL);
  RET(0x4e1e); return;
}

// 0b:4fb5, called from interactiond9_state1@substate3 and interactiond9_state2@cleanup.
void interactiond9_markSecretAsTold_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fb5, 0x4fb7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4fb7, 0x4fb8); A = mem_rd(gb, DE);
  CYC(0x4fb8, 0x4fba); alu_add(gb, 0x5a); // GLOBALFLAG_FIRST_AGES_DONE_SECRET
  CALL_C(0x4fba, setGlobalFlag_hook, 0x31f9, 0x4fbd);
  CYC(0x4fbd, 0x4fbf); A = 0x2c; // GLOBALFLAG_SECRET_CHEST_WAITING
  CYC(0x4fbf, 0x4fc2); unsetGlobalFlag_hook(gb); return; // jp
}

// 0b:4e33, called from interactionCoded9@state1. Handles a brand-new (non-upgrade) item: it
// spawns a chest interaction and waits for the player to open it.
void interactiond9_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e33, 0x4e35); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4e35, 0x4e36); A = mem_rd(gb, DE);
  CYC(0x4e36, 0x4e37); push_effect(gb, 0x4e37);
  switch (interactiond9_jump_table(gb)) {
    case 0x4e41: goto substate0;
    case 0x4e58: goto substate1;
    case 0x4e65: goto substate2;
    case 0x4e7c: goto substate3;
    case 0x4e8c: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4e41, 0x4e43); A = 0x01;
  CYC(0x4e43, 0x4e44); mem_wr(gb, DE, A); // [substate]
  CYC(0x4e44, 0x4e45); alu_xor(gb, A);
  CYC(0x4e45, 0x4e48); mem_wr(gb, wcca2, A);
  CALL_C(0x4e48, interactiond9_getItemID_hook, 0x4e14, 0x4e4b);
  CYC(0x4e4b, 0x4e4c); A = B;
  CYC(0x4e4c, 0x4e4f); mem_wr(gb, wChestContentsOverride, A);
  CYC(0x4e4f, 0x4e50); A = C;
  CYC(0x4e50, 0x4e53); mem_wr(gb, wChestContentsOverride + 1, A);
  CYC(0x4e53, 0x4e55); B = 0x11; // INTERAC_FARORE_MAKECHEST
  CYC(0x4e55, 0x4e58); objectCreateInteractionWithSubid00_hook(gb); return; // jp

substate1:
  CYC(0x4e58, 0x4e5b); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x4e5b, 0x4e5c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4e5c, 0x4e5d); ret_effect(gb); return; } // ret z
  CYC(0x4e5c, 0x4e5d);
  CYC(0x4e5d, 0x4e5f); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4e5f, 0x4e61); A = 0x3c;
  CYC(0x4e61, 0x4e62); mem_wr(gb, DE, A);
  CYC(0x4e62, 0x4e65); interactionIncSubstate_hook(gb); return; // jp

substate2:
  CALL_C(0x4e65, interactionDecCounter1_hook, 0x23cc, 0x4e68);
  if (!(F & FZ)) { CYCT(0x4e68, 0x4e69); ret_effect(gb); return; } // ret nz
  CYC(0x4e68, 0x4e69);
  CYC(0x4e69, 0x4e6b); A = 0x2c; // GLOBALFLAG_SECRET_CHEST_WAITING
  CALL_C(0x4e6b, setGlobalFlag_hook, 0x31f9, 0x4e6e);
  CYC(0x4e6e, 0x4e70); A = 0x02;
  CYC(0x4e70, 0x4e73); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x4e73, 0x4e76); SET_BC(0x5509); // TX_5509
  CALL_C(0x4e76, showText_hook, 0x1872, 0x4e79);
  CYC(0x4e79, 0x4e7c); interactionIncSubstate_hook(gb); return; // jp

substate3:
  CYC(0x4e7c, 0x4e7f); A = mem_rd(gb, wcca2);
  CYC(0x4e7f, 0x4e80); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4e80, 0x4e81); ret_effect(gb); return; } // ret z
  CYC(0x4e80, 0x4e81);
  CALL_C(0x4e81, interactiond9_markSecretAsTold_hook, 0x4fb5, 0x4e84);
  CYC(0x4e84, 0x4e86); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4e86, 0x4e88); A = 0x1e;
  CYC(0x4e88, 0x4e89); mem_wr(gb, DE, A);
  CYC(0x4e89, 0x4e8c); interactionIncSubstate_hook(gb); return; // jp

substate4:
  CALL_C(0x4e8c, interactionDecCounter1_hook, 0x23cc, 0x4e8f);
  if (!(F & FZ)) { CYCT(0x4e8f, 0x4e90); ret_effect(gb); return; } // ret nz
  CYC(0x4e8f, 0x4e90);
  CALL_C(0x4e90, objectCreatePuff_hook, 0x24c1, 0x4e93);
  CALL_C(0x4e93, objectGetShortPosition_hook, 0x2096, 0x4e96);
  CYC(0x4e96, 0x4e97); C = A;
  CYC(0x4e97, 0x4e99); A = 0xac;
  CALL_C(0x4e99, setTile_hook, 0x3a9c, 0x4e9c);
  CYC(0x4e9c, 0x4e9f); interactionDelete_hook(gb); return; // jp
}

// 0b:4e9f, called from interactionCoded9@state2. Handles an upgrade item (ring box, sword,
// shield, bombs, satchel): no chest, straight to a treasure pickup and fanfare.
void interactiond9_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e9f, 0x4ea1); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4ea1, 0x4ea2); A = mem_rd(gb, DE);
  CYC(0x4ea2, 0x4ea3); push_effect(gb, 0x4ea3);
  switch (interactiond9_jump_table(gb)) {
    case 0x4eb5: goto substate0;
    case 0x4ec2: goto substate1;
    case 0x4ed9: goto substate2;
    case 0x4eef: goto substate3; // also @substate4, aliased jump-table entry
    case 0x4f00: goto substate5;
    case 0x4f0b: goto substate6;
    case 0x4f70: goto substate7;
    case 0x4fa8: goto substate8;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4eb5, interactionIncSubstate_hook, 0x23e5, 0x4eb8);
  CYC(0x4eb8, 0x4eba); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4eba, 0x4ebc); mem_wr(gb, HL, 0x1e);
  CYC(0x4ebc, 0x4ebf); SET_HL(w1Link);
  CYC(0x4ebf, 0x4ec2); objectTakePosition_hook(gb); return; // jp

substate1:
  CALL_C(0x4ec2, interactionDecCounter1_hook, 0x23cc, 0x4ec5);
  if (!(F & FZ)) { CYCT(0x4ec5, 0x4ec6); ret_effect(gb); return; } // ret nz
  CYC(0x4ec5, 0x4ec6);
  CYC(0x4ec6, 0x4ec8); mem_wr(gb, HL, 0x3c);
  CALL_C(0x4ec8, getFreeInteractionSlot_hook, 0x3aef, 0x4ecb);
  if (!(F & FZ)) { CYCT(0x4ecb, 0x4ecc); ret_effect(gb); return; } // ret nz
  CYC(0x4ecb, 0x4ecc);
  CYC(0x4ecc, 0x4ece); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  CYC(0x4ece, 0x4ed0); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x4ed0, 0x4ed2); mem_wr(gb, HL, 0x28);
  CYC(0x4ed2, 0x4ed4); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x4ed4, 0x4ed6); mem_wr(gb, HL, 0x58);
  CYC(0x4ed6, 0x4ed9); interactionIncSubstate_hook(gb); return; // jp

substate2:
  CALL_C(0x4ed9, interactionDecCounter1_hook, 0x23cc, 0x4edc);
  if (!(F & FZ)) { CYCT(0x4edc, 0x4edd); ret_effect(gb); return; } // ret nz
  CYC(0x4edc, 0x4edd);
  CYC(0x4edd, 0x4edf); mem_wr(gb, HL, 0x14);
  CYC(0x4edf, 0x4ee2); A = mem_rd(gb, w1Link_yh);
  CYC(0x4ee2, 0x4ee3); B = A;
  CYC(0x4ee3, 0x4ee6); A = mem_rd(gb, w1Link_xh);
  CYC(0x4ee6, 0x4ee7); C = A;
  CYC(0x4ee7, 0x4ee9); A = 0x78;
  CALL_C(0x4ee9, createEnergySwirlGoingIn_hook, 0x29d8, 0x4eec);
  CYC(0x4eec, 0x4eef); interactionIncSubstate_hook(gb); return; // jp

substate3: // also @substate4
  CALL_C(0x4eef, interactionDecCounter1_hook, 0x23cc, 0x4ef2);
  if (!(F & FZ)) { CYCT(0x4ef2, 0x4ef3); ret_effect(gb); return; } // ret nz
  CYC(0x4ef2, 0x4ef3);
  CYC(0x4ef3, 0x4ef5); mem_wr(gb, HL, 0x14);
  CALL_C(0x4ef5, fadeinFromWhite_hook, 0x3299, 0x4ef8);
  // falls through into @playFadeOutSoundAndIncState

playFadeOutSoundAndIncState:
  CYC(0x4ef8, 0x4efa); A = 0xb4; // SND_FADEOUT
  CALL_C(0x4efa, playSound_b00_hook, 0x0c98, 0x4efd);
  CYC(0x4efd, 0x4f00); interactionIncSubstate_hook(gb); return; // jp

substate5:
  CALL_C(0x4f00, interactionDecCounter1_hook, 0x23cc, 0x4f03);
  if (!(F & FZ)) { CYCT(0x4f03, 0x4f04); ret_effect(gb); return; } // ret nz
  CYC(0x4f03, 0x4f04);
  CYC(0x4f04, 0x4f06); A = 0x02;
  CALL_C(0x4f06, fadeinFromWhiteWithDelay_hook, 0x3284, 0x4f09);
  CYCT(0x4f09, 0x4f0b); goto playFadeOutSoundAndIncState; // jr

substate6:
  CYC(0x4f0b, 0x4f0e); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x4f0e, 0x4f0f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f0f, 0x4f10); ret_effect(gb); return; } // ret nz
  CYC(0x4f0f, 0x4f10);
  CALL_C(0x4f10, interactiond9_getItemID_hook, 0x4e14, 0x4f13);
  CYC(0x4f13, 0x4f14); A = C;
  CYC(0x4f14, 0x4f15); push_effect(gb, 0x4f15);
  switch (interactiond9_jump_table(gb)) {
    case 0x4f39: goto swordUpgrade;
    case 0x4f3e: goto shieldUpgrade;
    case 0x4f49: goto bombUpgrade;
    case 0x4f5a: goto satchelUpgrade;
    case 0x4f1f: goto ringBoxUpgrade;
    default: hook_continue(gb, HL, sp0_); return;
  }

ringBoxUpgrade:
  CYC(0x4f1f, 0x4f22); A = mem_rd(gb, wRingBoxLevel);
  CYC(0x4f22, 0x4f24); alu_and(gb, 0x03);
  CYC(0x4f24, 0x4f27); SET_HL(0x4f2d); // @ringBoxSubids
  CYC(0x4f27, 0x4f28); interactiond9_addAToHl(gb, 0x4f28);
  CYC(0x4f28, 0x4f29); C = mem_rd(gb, HL);
  CYC(0x4f29, 0x4f2b); B = 0x2c; // TREASURE_RING_BOX
  CYCT(0x4f2b, 0x4f2d); interactiond9_state2_createTreasureAndIncSubstate_hook(gb); return; // jr

swordUpgrade:
  CYC(0x4f39, 0x4f3c); A = mem_rd(gb, wSwordLevel);
  CYCT(0x4f3c, 0x4f3e); goto l_4f41; // jr

shieldUpgrade:
  CYC(0x4f3e, 0x4f41); A = mem_rd(gb, wShieldLevel);
  // falls through into l_4f41

l_4f41:
  CYC(0x4f41, 0x4f44); SET_HL(0x4f31); // @swordShieldSubids
  CYC(0x4f44, 0x4f45); interactiond9_addDoubleIndex(gb, 0x4f45);
  CYC(0x4f45, 0x4f46); SET_HL(HL + 1); // inc hl
  CYC(0x4f46, 0x4f47); A = mem_rd(gb, HL);
  CYCT(0x4f47, 0x4f49); goto label_0b_135; // jr

bombUpgrade:
  CYC(0x4f49, 0x4f4c); SET_BC(0x6100); // TREASURE_OBJECT_BOMB_UPGRADE_00
  CYC(0x4f4c, 0x4f4f); push_effect(gb, 0x4f4f); interactiond9_state2_createTreasureAndIncSubstate_hook(gb); // call
  if (!(gb->pc == 0x4f4f && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(0x4f4f, 0x4f52); SET_HL(wMaxBombs);
  CYC(0x4f52, 0x4f53); A = mem_rd(gb, HL);
  CYC(0x4f53, 0x4f55); alu_add(gb, 0x20);
  CYC(0x4f55, 0x4f56); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x4f56, 0x4f57); mem_wr(gb, HL, A);
  CYC(0x4f57, 0x4f5a); setStatusBarNeedsRefreshBit1_hook(gb); return; // jp

satchelUpgrade:
  CYC(0x4f5a, 0x4f5d); A = mem_rd(gb, wSeedSatchelLevel);
  CYC(0x4f5d, 0x4f60); SET_BC(0x1904); // TREASURE_OBJECT_SEED_SATCHEL_UPGRADE
  CYCT(0x4f60, 0x4f62); interactiond9_state2_createTreasureAndIncSubstate_hook(gb); return; // jr

label_0b_135:
  CYC(0x4f62, 0x4f64); alu_and(gb, 0x03);
  CYC(0x4f64, 0x4f65); C = A;
  interactiond9_state2_createTreasureAndIncSubstate_hook(gb); return; // falls through into @createTreasureAndIncSubstate

substate7:
  CALL_C(0x4f70, retIfTextIsActive_hook, 0x1859, 0x4f73);
  CALL_C(0x4f73, interactionDecCounter1_hook, 0x23cc, 0x4f76);
  if (!(F & FZ)) { CYCT(0x4f76, 0x4f77); ret_effect(gb); return; } // ret nz
  CYC(0x4f76, 0x4f77);
  CYC(0x4f77, 0x4f79); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4f79, 0x4f7a); A = mem_rd(gb, DE);
  CYC(0x4f7a, 0x4f7c); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x4f7c, 0x4f7e); goto fillSatchel; } // jr z
  CYC(0x4f7c, 0x4f7e);
  CYC(0x4f7e, 0x4f7f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f7f, 0x4f81); goto cleanup; } // jr nz
  CYC(0x4f7f, 0x4f81);
  CYC(0x4f81, 0x4f84); A = mem_rd(gb, wSwordLevel);
  CYC(0x4f84, 0x4f86); alu_add(gb, 0x02);
  CYC(0x4f86, 0x4f87); C = A;
  CYC(0x4f87, 0x4f89); B = 0x05; // TREASURE_SWORD
  CYC(0x4f89, 0x4f8c); interactiond9_state2_createTreasure(gb, sp0_, 0x4f8c);
  if (!(gb->pc == 0x4f8c && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CALL_C(0x4f8c, interactionIncSubstate_hook, 0x23e5, 0x4f8f);
  CYC(0x4f8f, 0x4f91); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4f91, 0x4f93); mem_wr(gb, HL, 0x5a);
  RET(0x4f93); return;

fillSatchel:
  CALL_C(0x4f94, refillSeedSatchel_hook, 0x180c, 0x4f97);
  // falls through into @cleanup

cleanup:
  CYC(0x4f97, 0x4f99); A = 0x02;
  CYC(0x4f99, 0x4f9c); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(0x4f9c, 0x4f9f); SET_BC(0x5509); // TX_5509
  CALL_C(0x4f9f, showText_hook, 0x1872, 0x4fa2);
  CALL_C(0x4fa2, interactiond9_markSecretAsTold_hook, 0x4fb5, 0x4fa5);
  CYC(0x4fa5, 0x4fa8); interactionDelete_hook(gb); return; // jp

substate8:
  CALL_C(0x4fa8, interactionDecCounter1_hook, 0x23cc, 0x4fab);
  if (!(F & FZ)) { CYCT(0x4fab, 0x4fac); ret_effect(gb); return; } // ret nz
  CYC(0x4fab, 0x4fac);
  CYCT(0x4fac, 0x4fae); goto cleanup; // jr
}

// ==================================================================================================
// INTERAC_FARORE_GIVEITEM
// ==================================================================================================
void interactionCoded9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4dda, 0x4ddc); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4ddc, 0x4ddd); A = mem_rd(gb, DE);
  CYC(0x4ddd, 0x4dde); push_effect(gb, 0x4dde);
  switch (interactiond9_jump_table(gb)) {
    case 0x4de4: interactiond9_state0_hook(gb); return;
    case 0x4e33: interactiond9_state1_hook(gb); return;
    case 0x4e9f: interactiond9_state2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
