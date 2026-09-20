#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// Defined in src/game/circularSidescrollPlatform.c (the shared sidescroll-platform helper
// cluster from ref/oracles-disasm/object_code/ages/interactions/circularSidescrollPlatform.s).
void sidescrollPlatform_checkLinkOnPlatform_hook(GB *gb);
void sidescrollingPlatformCommon_hook(GB *gb);
void sidescrollPlatform_decCounter1_hook(GB *gb);

static uint16_t interactionCodea3_jump_table(GB *gb) {
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

static void interactionCodea3_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:59bb. Reached both by direct jump-table dispatch (state==1) and by a genuine `call` from
// @state3 below -- a plain function works for both: the jump-table redirect has no stack effect
// (matching a tail dispatch), and @state3's CALL_C models the real `call` correctly.
void interactionCodea3_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59bb, sidescrollPlatform_decCounter1_hook, 0x5bce, 0x59be);
  if (!(F & FZ)) { CYCT(0x59be, 0x59bf); ret_effect(gb); return; } // ret nz
  CYC(0x59be, 0x59bf);
  CYC(0x59bf, 0x59c1); mem_wr(gb, HL, 30);
  CYC(0x59c1, 0x59c2); L = E;
  CYC(0x59c2, 0x59c3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x59c3, 0x59c4); alu_xor(gb, A);
  RET(0x59c4); return;
}

// ==================================================================================================
// INTERAC_DISAPPEARING_SIDESCROLL_PLATFORM
// ==================================================================================================
void interactionCodea3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5978, 0x597a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x597a, 0x597b); A = mem_rd(gb, DE);
  CYC(0x597b, 0x597d); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x597d, 0x597f); goto l_5985; } // jr z
  CYC(0x597d, 0x597f);
  CALL_C(0x597f, sidescrollPlatform_checkLinkOnPlatform_hook, 0x5b7f, 0x5982);
  CALL_C(0x5982, sidescrollingPlatformCommon_hook, 0x5a73, 0x5985);

l_5985:
  CYC(0x5985, 0x5987); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5987, 0x5988); A = mem_rd(gb, DE);
  CYC(0x5988, 0x5989); push_effect(gb, 0x5989);
  switch (interactionCodea3_jump_table(gb)) {
    case 0x5993: goto state0;
    case 0x59bb: interactionCodea3_state1_hook(gb); return;
    case 0x59c5: goto state2;
    case 0x59d8: goto state3;
    case 0x59e1: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x5993, 0x5995); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5995, 0x5996); A = mem_rd(gb, DE);
  CYC(0x5996, 0x5999); SET_HL(0x59b5); // @subidData
  CYC(0x5999, 0x599a); interactionCodea3_add_double_index_from_rst(gb, 0x599a);
  CYC(0x599a, 0x599c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x599c, 0x599d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x599d, 0x599e); mem_wr(gb, DE, A);
  CYC(0x599e, 0x59a0); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x59a0, 0x59a1); A = mem_rd(gb, HL);
  CYC(0x59a1, 0x59a2); mem_wr(gb, DE, A);
  CYC(0x59a2, 0x59a4); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x59a4, 0x59a6); A = 0x08;
  CYC(0x59a6, 0x59a7); mem_wr(gb, DE, A);
  CYC(0x59a7, 0x59a8); E = alu_inc8(gb, E);
  CYC(0x59a8, 0x59a9); mem_wr(gb, DE, A);
  CALL_C(0x59a9, interactionInitGraphics_hook, 0x15fb, 0x59ac);
  CYC(0x59ac, 0x59ae); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x59ae, 0x59af); A = mem_rd(gb, DE);
  CYC(0x59af, 0x59b1); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x59b1, 0x59b4); objectSetVisible83_hook(gb); return; } // jp z
  CYC(0x59b1, 0x59b4);
  RET(0x59b4); return;

  // @subidData (0b:59b5): pure ROM data (three (state,counter1) byte pairs), referenced only via
  // SET_HL(0x59b5) above.

state2:
  CALL_C(0x59c5, sidescrollPlatform_decCounter1_hook, 0x5bce, 0x59c8);
  if (!(F & FZ)) { CYCT(0x59c8, 0x59ca); goto flickerVisibility; } // jr nz
  CYC(0x59c8, 0x59ca);
  CYC(0x59ca, 0x59cc); mem_wr(gb, HL, 150);
  CYC(0x59cc, 0x59cd); L = E;
  CYC(0x59cd, 0x59ce); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x59ce, 0x59d1); objectSetInvisible_hook(gb); return; // jp

flickerVisibility:
  CYC(0x59d1, 0x59d3); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x59d3, 0x59d4); A = mem_rd(gb, DE);
  CYC(0x59d4, 0x59d6); A = (uint8_t)(A ^ 0x80);
  CYC(0x59d6, 0x59d7); mem_wr(gb, DE, A);
  RET(0x59d7); return;

state3:
  CALL_C(0x59d8, interactionCodea3_state1_hook, 0x59bb, 0x59db);
  if (!(F & FZ)) { CYCT(0x59db, 0x59dc); ret_effect(gb); return; } // ret nz
  CYC(0x59db, 0x59dc);
  CYC(0x59dc, 0x59de); A = 0x7b; // SND_MYSTERY_SEED
  CYC(0x59de, 0x59e1); playSound_b00_hook(gb); return; // jp

state4:
  CALL_C(0x59e1, sidescrollPlatform_decCounter1_hook, 0x5bce, 0x59e4);
  if (!(F & FZ)) { CYCT(0x59e4, 0x59e6); goto flickerVisibility; } // jr nz
  CYC(0x59e4, 0x59e6);
  CYC(0x59e6, 0x59e8); mem_wr(gb, HL, 120);
  CYC(0x59e8, 0x59e9); L = E;
  CYC(0x59e9, 0x59eb); mem_wr(gb, HL, 0x01);
  CYC(0x59eb, 0x59ee); objectSetVisible83_hook(gb); return; // jp
}
