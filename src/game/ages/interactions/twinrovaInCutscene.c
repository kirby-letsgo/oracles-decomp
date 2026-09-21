#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/twinrovaInCutscene.s (interactionCodeb0 /
// INTERAC_TWINROVA_IN_CUTSCENE), bank 0x0b. Distinct from src/game/twinrova.c (bank 0x10,
// INTERAC_TWINROVA proper) and src/game/twinrovaInteraction.c (bank 0x0a, twinrova's other
// bank-0x0a routines) -- verified via this file's own disassembly path and bank. The root's
// rst_jumpTable dispatches into two separately-named (non-@) symbols, twinrovaInCutscene_state0
// and twinrovaInCutscene_state1, which each get their own _hook function here, matching how
// src/game/zora.c and src/game/ambi.c dispatch into sibling named hooks in the same file.

void twinrovaInCutscene_state0_hook(GB *gb);
void twinrovaInCutscene_state1_hook(GB *gb);
void twinrovaInCutscene_loadScript_hook(GB *gb);

static uint16_t twinrovaInCutscene_jump_table(GB *gb) {
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

static void twinrovaInCutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// twinrovaInCutscene_state0@commonInit1 (0b:670a): shared by @subid0 via a real `call` (which
// pushes 0b:6706 before invoking this) and by @subid1 via plain fallthrough (no call, no push).
// It never executes its own ret -- it tail-jumps into the already-hooked interactionSetAnimation,
// whose ret_effect pops whatever the caller left on the stack -- so calling it as a plain C
// function from either site reproduces both entry paths correctly.
static void twinrovaInCutscene_state0_commonInit1(GB *gb) {
  BASE(twinrovaInCutscene_state0);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+44); interactionSetAnimation_hook(gb); // jp
}

// ==================================================================================================
// INTERAC_TWINROVA_IN_CUTSCENE
// ==================================================================================================
void interactionCodeb0_hook(GB *gb) {
  BASE(interactionCodeb0);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  uint16_t target = twinrovaInCutscene_jump_table(gb);
  if (target == SYM(twinrovaInCutscene_state0)) { twinrovaInCutscene_state0_hook(gb); return; }
  TAIL(twinrovaInCutscene_state1); // target == 0x6724
}

void twinrovaInCutscene_state0_hook(GB *gb) {
  BASE(twinrovaInCutscene_state0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6); // SWITCHES THREADS
  CALL_C(b_+6, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+9);
  CYC(b_+9, b_+11); A = 0x28; // >TX_2800
  CALL_C(b_+11, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  {
    CYC(b_+17, b_+18); push_effect(gb, b_+18);
    uint16_t target = twinrovaInCutscene_jump_table(gb);
    if (target == b_+33) goto subid1;
    if (target == b_+44) goto subid2;
    if (target == b_+48) goto subid3;
    // target == 0x6701 falls through to subid0
  }

  // twinrovaInCutscene_state0@subid0
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+31); push_effect(gb, b_+31); twinrovaInCutscene_state0_commonInit1(gb); // call
  CYC(b_+31, b_+33); TAIL(twinrovaInCutscene_loadScript); // jr

subid1: // twinrovaInCutscene_state0@subid1
  CYC(b_+33, b_+35); A = 0x02;
  twinrovaInCutscene_state0_commonInit1(gb); return; // falls through into @commonInit1, no call

subid2: // twinrovaInCutscene_state0@subid2
  CYC(b_+44, b_+46); A = 0x01;
  CYC(b_+46, b_+48); goto commonInit2; // jr

subid3: // twinrovaInCutscene_state0@subid3
  CYC(b_+48, b_+50); A = 0x01;
  CALL_C(b_+50, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+53);
  CYC(b_+53, b_+55); A = 0x02;

commonInit2: // twinrovaInCutscene_state0@commonInit2
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+61); TAIL(interactionSetAlwaysUpdateBit); // jp
}

void twinrovaInCutscene_state1_hook(GB *gb) {
  BASE(twinrovaInCutscene_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = twinrovaInCutscene_jump_table(gb);
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; } // subid list's 2nd/3rd/4th entries jump straight into interactionAnimate
    // target == 0x6730 falls through to subid0
  }

  // twinrovaInCutscene_state1@subid0
  CALL_C(b_+12, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto substate1; } // jr nz
  CYC(b_+15, b_+17);

  // twinrovaInCutscene_state1@substate0
  CALL_C(b_+17, interactionAnimate_hook, SYM(interactionAnimate), b_+20);
  CALL_C(b_+20, interactionRunScript_hook, SYM(interactionRunScript), b_+23);
  if (!(F & FC)) { CYCT(b_+23, b_+24); ret_effect(gb); return; } // ret nc
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CYC(b_+29, b_+30); alu_xor(gb, A);
  CYC(b_+30, b_+33); W8(wGenericCutscene_cbb3) = A;
  CYC(b_+33, b_+34); A = alu_dec8(gb, A);
  CYC(b_+34, b_+37); W8(wGenericCutscene_cbba) = A;
  CYC(b_+37, b_+40); TAIL(interactionIncSubstate); // jp

substate1: // twinrovaInCutscene_state1@substate1
  CYC(b_+40, b_+43); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+43, b_+45); B = 0x02;
  CALL_C(b_+45, flashScreen_hook, SYM(flashScreen), b_+48);
  if (F & FZ) { CYCT(b_+48, b_+49); ret_effect(gb); return; } // ret z
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); A = 0x02;
  CYC(b_+51, b_+54); W8(wGenericCutscene_cbb8) = A;
  CYC(b_+54, b_+56); A = 0x08; // CUTSCENE_BLACK_TOWER_EXPLANATION
  CYC(b_+56, b_+59); W8(wCutsceneTrigger) = A;
  CYC(b_+59, b_+60); ret_effect(gb); return; // ret
}

void twinrovaInCutscene_loadScript_hook(GB *gb) {
  BASE(twinrovaInCutscene_loadScript);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+13); // @scriptTable
  CYC(b_+6, b_+7); twinrovaInCutscene_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); TAIL(interactionSetScript); // jp

  // twinrovaInCutscene_loadScript@scriptTable (0b:676d): pure ROM data (2-entry pointer table),
  // not ported as code -- referenced only via SET_HL(0x676d) above.
}
