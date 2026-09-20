#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  CYC(0x670a, 0x670c); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x670c, 0x670d); mem_wr(gb, DE, A);
  CYC(0x670d, 0x670f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x670f, 0x6710); A = mem_rd(gb, DE);
  CYC(0x6710, 0x6713); interactionSetAnimation_hook(gb); // jp
}

// ==================================================================================================
// INTERAC_TWINROVA_IN_CUTSCENE
// ==================================================================================================
void interactionCodeb0_hook(GB *gb) {
  CYC(0x66df, 0x66e1); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x66e1, 0x66e2); A = mem_rd(gb, DE);
  CYC(0x66e2, 0x66e3); push_effect(gb, 0x66e3);
  uint16_t target = twinrovaInCutscene_jump_table(gb);
  if (target == 0x66e7) { twinrovaInCutscene_state0_hook(gb); return; }
  twinrovaInCutscene_state1_hook(gb); return; // target == 0x6724
}

void twinrovaInCutscene_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x66e7, 0x66e9); A = 0x01;
  CYC(0x66e9, 0x66ea); mem_wr(gb, DE, A); // [state]
  CALL_C(0x66ea, interactionInitGraphics_hook, 0x15fb, 0x66ed); // SWITCHES THREADS
  CALL_C(0x66ed, objectSetVisiblec2_hook, 0x1e45, 0x66f0);
  CYC(0x66f0, 0x66f2); A = 0x28; // >TX_2800
  CALL_C(0x66f2, interactionSetHighTextIndex_hook, 0x253b, 0x66f5);
  CYC(0x66f5, 0x66f7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x66f7, 0x66f8); A = mem_rd(gb, DE);
  {
    CYC(0x66f8, 0x66f9); push_effect(gb, 0x66f9);
    uint16_t target = twinrovaInCutscene_jump_table(gb);
    if (target == 0x6708) goto subid1;
    if (target == 0x6713) goto subid2;
    if (target == 0x6717) goto subid3;
    // target == 0x6701 falls through to subid0
  }

  // twinrovaInCutscene_state0@subid0
  CYC(0x6701, 0x6703); A = 0x01;
  CYC(0x6703, 0x6706); push_effect(gb, 0x6706); twinrovaInCutscene_state0_commonInit1(gb); // call
  CYC(0x6706, 0x6708); twinrovaInCutscene_loadScript_hook(gb); return; // jr

subid1: // twinrovaInCutscene_state0@subid1
  CYC(0x6708, 0x670a); A = 0x02;
  twinrovaInCutscene_state0_commonInit1(gb); return; // falls through into @commonInit1, no call

subid2: // twinrovaInCutscene_state0@subid2
  CYC(0x6713, 0x6715); A = 0x01;
  CYC(0x6715, 0x6717); goto commonInit2; // jr

subid3: // twinrovaInCutscene_state0@subid3
  CYC(0x6717, 0x6719); A = 0x01;
  CALL_C(0x6719, interactionSetAnimation_hook, 0x262e, 0x671c);
  CYC(0x671c, 0x671e); A = 0x02;

commonInit2: // twinrovaInCutscene_state0@commonInit2
  CYC(0x671e, 0x6720); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x6720, 0x6721); mem_wr(gb, DE, A);
  CYC(0x6721, 0x6724); interactionSetAlwaysUpdateBit_hook(gb); return; // jp
}

void twinrovaInCutscene_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6724, 0x6726); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6726, 0x6727); A = mem_rd(gb, DE);
  {
    CYC(0x6727, 0x6728); push_effect(gb, 0x6728);
    uint16_t target = twinrovaInCutscene_jump_table(gb);
    if (target == 0x261b) { interactionAnimate_hook(gb); return; } // subid list's 2nd/3rd/4th entries jump straight into interactionAnimate
    // target == 0x6730 falls through to subid0
  }

  // twinrovaInCutscene_state1@subid0
  CALL_C(0x6730, checkInteractionSubstate_hook, 0x2403, 0x6733);
  if (!(F & FZ)) { CYCT(0x6733, 0x6735); goto substate1; } // jr nz
  CYC(0x6733, 0x6735);

  // twinrovaInCutscene_state1@substate0
  CALL_C(0x6735, interactionAnimate_hook, 0x261b, 0x6738);
  CALL_C(0x6738, interactionRunScript_hook, 0x2552, 0x673b);
  if (!(F & FC)) { CYCT(0x673b, 0x673c); ret_effect(gb); return; } // ret nc
  CYC(0x673b, 0x673c);
  CYC(0x673c, 0x673e); A = 0xd2; // SND_LIGHTNING
  CALL_C(0x673e, playSound_b00_hook, 0x0c98, 0x6741);
  CYC(0x6741, 0x6742); alu_xor(gb, A);
  CYC(0x6742, 0x6745); W8(wGenericCutscene_cbb3) = A;
  CYC(0x6745, 0x6746); A = alu_dec8(gb, A);
  CYC(0x6746, 0x6749); W8(wGenericCutscene_cbba) = A;
  CYC(0x6749, 0x674c); interactionIncSubstate_hook(gb); return; // jp

substate1: // twinrovaInCutscene_state1@substate1
  CYC(0x674c, 0x674f); SET_HL(wGenericCutscene_cbb3);
  CYC(0x674f, 0x6751); B = 0x02;
  CALL_C(0x6751, flashScreen_hook, 0x2d73, 0x6754);
  if (F & FZ) { CYCT(0x6754, 0x6755); ret_effect(gb); return; } // ret z
  CYC(0x6754, 0x6755);
  CYC(0x6755, 0x6757); A = 0x02;
  CYC(0x6757, 0x675a); W8(wGenericCutscene_cbb8) = A;
  CYC(0x675a, 0x675c); A = 0x08; // CUTSCENE_BLACK_TOWER_EXPLANATION
  CYC(0x675c, 0x675f); W8(wCutsceneTrigger) = A;
  CYC(0x675f, 0x6760); ret_effect(gb); return; // ret
}

void twinrovaInCutscene_loadScript_hook(GB *gb) {
  CYC(0x6760, 0x6762); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6762, 0x6763); A = mem_rd(gb, DE);
  CYC(0x6763, 0x6766); SET_HL(0x676d); // @scriptTable
  CYC(0x6766, 0x6767); twinrovaInCutscene_add_double_index(gb, 0x6767);
  CYC(0x6767, 0x6768); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6768, 0x6769); H = mem_rd(gb, HL);
  CYC(0x6769, 0x676a); L = A;
  CYC(0x676a, 0x676d); interactionSetScript_hook(gb); return; // jp

  // twinrovaInCutscene_loadScript@scriptTable (0b:676d): pure ROM data (2-entry pointer table),
  // not ported as code -- referenced only via SET_HL(0x676d) above.
}
