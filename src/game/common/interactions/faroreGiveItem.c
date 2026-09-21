#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactiond9_state2);
  push_effect(gb, return_address);
  CALL_C(b_+271, createTreasure_hook, SYM(createTreasure), b_+274);
  if (!(F & FZ)) { CYCT(b_+274, b_+275); ret_effect(gb); return; } // ret nz
  CYC(b_+274, b_+275);
  CYC(b_+275, b_+278); TAIL(objectCopyPosition); // jp
}

// 0b:4f65, interactiond9_state2@createTreasureAndIncSubstate. Reached by plain goto/fallthrough
// from @ringBoxUpgrade/@satchelUpgrade/@label_0b_135 -- tail position, no push there, matches a
// `jr`/fallthrough -- and by a genuine `call` from @bombUpgrade, which pushes its own return
// address before calling and checks pc/sp after -- same manual CALL_C-style idiom as
// @createTreasure above.
void interactiond9_state2_createTreasureAndIncSubstate_hook(GB *gb) {
  BASE(interactiond9_state2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+198, b_+201); interactiond9_state2_createTreasure(gb, sp0_, b_+201);
  if (!(gb->pc == b_+201 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(b_+201, b_+203); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+203, b_+205); A = 0x1e;
  CYC(b_+205, b_+206); mem_wr(gb, DE, A);
  CYC(b_+206, b_+209); TAIL(interactionIncSubstate); // jp
}

// 0b:4de4, called from interactionCoded9@state0.
void interactiond9_state0_hook(GB *gb) {
  BASE(interactiond9_state0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+11); alu_add(gb, GV(0x5a, 0x6e)); // GLOBALFLAG_FIRST_AGES_DONE_SECRET
  CALL_C(b_+11, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+14);
  if (F & FZ) { CYCT(b_+14, b_+16); goto secretNotTold; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); SET_BC(0x550c); // TX_550c
  CALL_C(b_+19, showText_hook, SYM(showText), b_+22);
  CYC(b_+22, b_+24); A = 0x02;
  CYC(b_+24, b_+27); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+27, b_+30); TAIL(interactionDelete); // jp

secretNotTold:
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+34); SET_HL(b_+46); // @bits
  CALL_C(b_+34, checkFlag_hook, SYM(checkFlag), b_+37);
  CYC(b_+37, b_+39); A = 0x02;
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto skipDec; } // jr nz
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); A = alu_dec8(gb, A);

skipDec:
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  RET(b_+45); return;
}

// 0b:4e14, called from interactiond9_state1@substate0 and interactiond9_state2@substate6.
// @param[out] bc  The item ID. If this is an upgrade, c is 0-4 indicating the behaviour.
void interactiond9_getItemID_hook(GB *gb) {
  BASE(interactiond9_getItemID);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+11); // @chestContents
  CYC(b_+6, b_+7); interactiond9_addDoubleIndex(gb, b_+7);
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  RET(b_+10); return;
}

// 0b:4fb5, called from interactiond9_state1@substate3 and interactiond9_state2@cleanup.
void interactiond9_markSecretAsTold_hook(GB *gb) {
  BASE(interactiond9_markSecretAsTold);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, GV(0x5a, 0x6e)); // GLOBALFLAG_FIRST_AGES_DONE_SECRET
  CALL_C(b_+5, setGlobalFlag_hook, SYM(setGlobalFlag), b_+8);
  CYC(b_+8, b_+10); A = 0x2c; // GLOBALFLAG_SECRET_CHEST_WAITING
  CYC(b_+10, b_+13); TAIL(unsetGlobalFlag); // jp
}

// 0b:4e33, called from interactionCoded9@state1. Handles a brand-new (non-upgrade) item: it
// spawns a chest interaction and waits for the player to open it.
void interactiond9_state1_hook(GB *gb) {
  BASE(interactiond9_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiond9_jump_table(gb));
    if (jt_ == b_+14) { goto substate0; }
    else if (jt_ == b_+37) { goto substate1; }
    else if (jt_ == b_+50) { goto substate2; }
    else if (jt_ == b_+73) { goto substate3; }
    else if (jt_ == b_+89) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A); // [substate]
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+21); mem_wr(gb, wcca2, A);
  CALL_C(b_+21, interactiond9_getItemID_hook, SYM(interactiond9_getItemID), b_+24);
  CYC(b_+24, b_+25); A = B;
  CYC(b_+25, b_+28); mem_wr(gb, wChestContentsOverride, A);
  CYC(b_+28, b_+29); A = C;
  CYC(b_+29, b_+32); mem_wr(gb, wChestContentsOverride + 1, A);
  CYC(b_+32, b_+34); B = 0x11; // INTERAC_FARORE_MAKECHEST
  CYC(b_+34, b_+37); TAIL(objectCreateInteractionWithSubid00); // jp

substate1:
  CYC(b_+37, b_+40); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+41, b_+42); ret_effect(gb); return; } // ret z
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+46); A = 0x3c;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+50); TAIL(interactionIncSubstate); // jp

substate2:
  CALL_C(b_+50, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+53);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; } // ret nz
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); A = 0x2c; // GLOBALFLAG_SECRET_CHEST_WAITING
  CALL_C(b_+56, setGlobalFlag_hook, SYM(setGlobalFlag), b_+59);
  CYC(b_+59, b_+61); A = 0x02;
  CYC(b_+61, b_+64); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+64, b_+67); SET_BC(0x5509); // TX_5509
  CALL_C(b_+67, showText_hook, SYM(showText), b_+70);
  CYC(b_+70, b_+73); TAIL(interactionIncSubstate); // jp

substate3:
  CYC(b_+73, b_+76); A = mem_rd(gb, wcca2);
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+77, b_+78); ret_effect(gb); return; } // ret z
  CYC(b_+77, b_+78);
  CALL_C(b_+78, interactiond9_markSecretAsTold_hook, SYM(interactiond9_markSecretAsTold), b_+81);
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+83, b_+85); A = 0x1e;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+89); TAIL(interactionIncSubstate); // jp

substate4:
  CALL_C(b_+89, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+92);
  if (!(F & FZ)) { CYCT(b_+92, b_+93); ret_effect(gb); return; } // ret nz
  CYC(b_+92, b_+93);
  CALL_C(b_+93, objectCreatePuff_hook, SYM(objectCreatePuff), b_+96);
  CALL_C(b_+96, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+99);
  CYC(b_+99, b_+100); C = A;
  CYC(b_+100, b_+102); A = 0xac;
  CALL_C(b_+102, setTile_hook, SYM(setTile), b_+105);
  CYC(b_+105, b_+108); TAIL(interactionDelete); // jp
}

// 0b:4e9f, called from interactionCoded9@state2. Handles an upgrade item (ring box, sword,
// shield, bombs, satchel): no chest, straight to a treasure pickup and fanfare.
void interactiond9_state2_hook(GB *gb) {
  BASE(interactiond9_state2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiond9_jump_table(gb));
    if (jt_ == b_+22) { goto substate0; }
    else if (jt_ == b_+35) { goto substate1; }
    else if (jt_ == b_+58) { goto substate2; }
    else if (jt_ == b_+80) { goto substate3; } // also @substate4, aliased jump-table entry
    else if (jt_ == b_+97) { goto substate5; }
    else if (jt_ == b_+108) { goto substate6; }
    else if (jt_ == b_+209) { goto substate7; }
    else if (jt_ == b_+265) { goto substate8; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+22, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+25);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x1e);
  CYC(b_+29, b_+32); SET_HL(w1Link);
  CYC(b_+32, b_+35); TAIL(objectTakePosition); // jp

substate1:
  CALL_C(b_+35, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+38);
  if (!(F & FZ)) { CYCT(b_+38, b_+39); ret_effect(gb); return; } // ret nz
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+41, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; } // ret nz
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  CYC(b_+47, b_+49); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x28);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x58);
  CYC(b_+55, b_+58); TAIL(interactionIncSubstate); // jp

substate2:
  CALL_C(b_+58, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+61);
  if (!(F & FZ)) { CYCT(b_+61, b_+62); ret_effect(gb); return; } // ret nz
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x14);
  CYC(b_+64, b_+67); A = mem_rd(gb, w1Link_yh);
  CYC(b_+67, b_+68); B = A;
  CYC(b_+68, b_+71); A = mem_rd(gb, w1Link_xh);
  CYC(b_+71, b_+72); C = A;
  CYC(b_+72, b_+74); A = 0x78;
  CALL_C(b_+74, createEnergySwirlGoingIn_hook, SYM(createEnergySwirlGoingIn), b_+77);
  CYC(b_+77, b_+80); TAIL(interactionIncSubstate); // jp

substate3: // also @substate4
  CALL_C(b_+80, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+83);
  if (!(F & FZ)) { CYCT(b_+83, b_+84); ret_effect(gb); return; } // ret nz
  CYC(b_+83, b_+84);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x14);
  CALL_C(b_+86, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+89);
  // falls through into @playFadeOutSoundAndIncState

playFadeOutSoundAndIncState:
  CYC(b_+89, b_+91); A = 0xb4; // SND_FADEOUT
  CALL_C(b_+91, playSound_b00_hook, SYM(playSound_b00), b_+94);
  CYC(b_+94, b_+97); TAIL(interactionIncSubstate); // jp

substate5:
  CALL_C(b_+97, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+100);
  if (!(F & FZ)) { CYCT(b_+100, b_+101); ret_effect(gb); return; } // ret nz
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); A = 0x02;
  CALL_C(b_+103, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+106);
  CYCT(b_+106, b_+108); goto playFadeOutSoundAndIncState; // jr

substate6:
  CYC(b_+108, b_+111); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+111, b_+112); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+112, b_+113); ret_effect(gb); return; } // ret nz
  CYC(b_+112, b_+113);
  CALL_C(b_+113, interactiond9_getItemID_hook, SYM(interactiond9_getItemID), b_+116);
  CYC(b_+116, b_+117); A = C;
  CYC(b_+117, b_+118); push_effect(gb, b_+118);
  do { uint16_t jt_ = (interactiond9_jump_table(gb));
    if (jt_ == b_+154) { goto swordUpgrade; }
    else if (jt_ == b_+159) { goto shieldUpgrade; }
    else if (jt_ == b_+170) { goto bombUpgrade; }
    else if (jt_ == b_+187) { goto satchelUpgrade; }
    else if (jt_ == b_+128) { goto ringBoxUpgrade; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

ringBoxUpgrade:
  CYC(b_+128, b_+131); A = mem_rd(gb, wRingBoxLevel);
  CYC(b_+131, b_+133); alu_and(gb, 0x03);
  CYC(b_+133, b_+136); SET_HL(b_+142); // @ringBoxSubids
  CYC(b_+136, b_+137); interactiond9_addAToHl(gb, b_+137);
  CYC(b_+137, b_+138); C = mem_rd(gb, HL);
  CYC(b_+138, b_+140); B = 0x2c; // TREASURE_RING_BOX
  CYCT(b_+140, b_+142); interactiond9_state2_createTreasureAndIncSubstate_hook(gb); return; // jr

swordUpgrade:
  CYC(b_+154, b_+157); A = mem_rd(gb, wSwordLevel);
  CYCT(b_+157, b_+159); goto l_4f41; // jr

shieldUpgrade:
  CYC(b_+159, b_+162); A = mem_rd(gb, wShieldLevel);
  // falls through into l_4f41

l_4f41:
  CYC(b_+162, b_+165); SET_HL(b_+146); // @swordShieldSubids
  CYC(b_+165, b_+166); interactiond9_addDoubleIndex(gb, b_+166);
  CYC(b_+166, b_+167); SET_HL(HL + 1); // inc hl
  CYC(b_+167, b_+168); A = mem_rd(gb, HL);
  CYCT(b_+168, b_+170); goto label_0b_135; // jr

bombUpgrade:
  CYC(b_+170, b_+173); SET_BC(0x6100); // TREASURE_OBJECT_BOMB_UPGRADE_00
  CYC(b_+173, b_+176); push_effect(gb, b_+176); interactiond9_state2_createTreasureAndIncSubstate_hook(gb); // call
  if (!(gb->pc == b_+176 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(b_+176, b_+179); SET_HL(wMaxBombs);
  CYC(b_+179, b_+180); A = mem_rd(gb, HL);
  CYC(b_+180, b_+182); alu_add(gb, 0x20);
  CYC(b_+182, b_+183); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+183, b_+184); mem_wr(gb, HL, A);
  CYC(b_+184, b_+187); TAIL(setStatusBarNeedsRefreshBit1); // jp

satchelUpgrade:
  CYC(b_+187, b_+190); A = mem_rd(gb, wSeedSatchelLevel);
  CYC(b_+190, b_+193); SET_BC(GV(0x1904, 0x1901)); // TREASURE_OBJECT_SEED_SATCHEL_UPGRADE
  CYCT(b_+193, b_+195); interactiond9_state2_createTreasureAndIncSubstate_hook(gb); return; // jr

label_0b_135:
  CYC(b_+195, b_+197); alu_and(gb, 0x03);
  CYC(b_+197, b_+198); C = A;
  interactiond9_state2_createTreasureAndIncSubstate_hook(gb); return; // falls through into @createTreasureAndIncSubstate

substate7:
  CALL_C(b_+209, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+212);
  CALL_C(b_+212, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+215);
  if (!(F & FZ)) { CYCT(b_+215, b_+216); ret_effect(gb); return; } // ret nz
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+218); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+218, b_+219); A = mem_rd(gb, DE);
  CYC(b_+219, b_+221); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+221, b_+223); goto fillSatchel; } // jr z
  CYC(b_+221, b_+223);
  CYC(b_+223, b_+224); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+224, b_+226); goto cleanup; } // jr nz
  CYC(b_+224, b_+226);
  CYC(b_+226, b_+229); A = mem_rd(gb, wSwordLevel);
  CYC(b_+229, b_+231); alu_add(gb, 0x02);
  CYC(b_+231, b_+232); C = A;
  CYC(b_+232, b_+234); B = 0x05; // TREASURE_SWORD
  CYC(b_+234, b_+237); interactiond9_state2_createTreasure(gb, sp0_, b_+237);
  if (!(gb->pc == b_+237 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CALL_C(b_+237, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+240);
  CYC(b_+240, b_+242); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+242, b_+244); mem_wr(gb, HL, 0x5a);
  RET(b_+244); return;

fillSatchel:
  CALL_C(b_+245, refillSeedSatchel_hook, SYM(refillSeedSatchel), b_+248);
  // falls through into @cleanup

cleanup:
  CYC(b_+248, b_+250); A = 0x02;
  CYC(b_+250, b_+253); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+253, b_+256); SET_BC(0x5509); // TX_5509
  CALL_C(b_+256, showText_hook, SYM(showText), b_+259);
  CALL_C(b_+259, interactiond9_markSecretAsTold_hook, SYM(interactiond9_markSecretAsTold), b_+262);
  CYC(b_+262, b_+265); TAIL(interactionDelete); // jp

substate8:
  CALL_C(b_+265, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+268);
  if (!(F & FZ)) { CYCT(b_+268, b_+269); ret_effect(gb); return; } // ret nz
  CYC(b_+268, b_+269);
  CYCT(b_+269, b_+271); goto cleanup; // jr
}

// ==================================================================================================
// INTERAC_FARORE_GIVEITEM
// ==================================================================================================
void interactionCoded9_hook(GB *gb) {
  BASE(interactionCoded9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactiond9_jump_table(gb));
    if (jt_ == SYM(interactiond9_state0) && hook_enabled_at(gb, SYM(interactiond9_state0))) { interactiond9_state0_hook(gb); return; }
    else if (jt_ == SYM(interactiond9_state1) && hook_enabled_at(gb, SYM(interactiond9_state1))) { interactiond9_state1_hook(gb); return; }
    else if (jt_ == SYM(interactiond9_state2) && hook_enabled_at(gb, SYM(interactiond9_state2))) { interactiond9_state2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
