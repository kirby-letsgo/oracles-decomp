#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodea3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+67, sidescrollPlatform_decCounter1_hook, SYM(sidescrollPlatform_decCounter1), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+71); ret_effect(gb); return; } // ret nz
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 30);
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+76); alu_xor(gb, A);
  RET(b_+76); return;
}

// ==================================================================================================
// INTERAC_DISAPPEARING_SIDESCROLL_PLATFORM
// ==================================================================================================
void interactionCodea3_hook(GB *gb) {
  BASE(interactionCodea3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+5, b_+7); goto l_5985; } // jr z
  CYC(b_+5, b_+7);
  CALL_C(b_+7, sidescrollPlatform_checkLinkOnPlatform_hook, SYM(sidescrollPlatform_checkLinkOnPlatform), b_+10);
  CALL_C(b_+10, sidescrollingPlatformCommon_hook, SYM(sidescrollingPlatformCommon), b_+13);

l_5985:
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (interactionCodea3_jump_table(gb));
    if (jt_ == b_+27) { goto state0; }
    else if (jt_ == b_+67) { interactionCodea3_state1_hook(gb); return; }
    else if (jt_ == b_+77) { goto state2; }
    else if (jt_ == b_+96) { goto state3; }
    else if (jt_ == b_+105) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+33); SET_HL(b_+61); // @subidData
  CYC(b_+33, b_+34); interactionCodea3_add_double_index_from_rst(gb, b_+34);
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+44, b_+46); A = 0x08;
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+48); E = alu_inc8(gb, E);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CALL_C(b_+49, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+52);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+57); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+57, b_+60); objectSetVisible83_hook(gb); return; } // jp z
  CYC(b_+57, b_+60);
  RET(b_+60); return;

  // @subidData (0b:59b5): pure ROM data (three (state,counter1) byte pairs), referenced only via
  // SET_HL(0x59b5) above.

state2:
  CALL_C(b_+77, sidescrollPlatform_decCounter1_hook, SYM(sidescrollPlatform_decCounter1), b_+80);
  if (!(F & FZ)) { CYCT(b_+80, b_+82); goto flickerVisibility; } // jr nz
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+84); mem_wr(gb, HL, 150);
  CYC(b_+84, b_+85); L = E;
  CYC(b_+85, b_+86); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+86, b_+89); objectSetInvisible_hook(gb); return; // jp

flickerVisibility:
  CYC(b_+89, b_+91); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+94); A = (uint8_t)(A ^ 0x80);
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  RET(b_+95); return;

state3:
  CALL_C(b_+96, interactionCodea3_state1_hook, b_+67, b_+99);
  if (!(F & FZ)) { CYCT(b_+99, b_+100); ret_effect(gb); return; } // ret nz
  CYC(b_+99, b_+100);
  CYC(b_+100, b_+102); A = 0x7b; // SND_MYSTERY_SEED
  CYC(b_+102, b_+105); playSound_b00_hook(gb); return; // jp

state4:
  CALL_C(b_+105, sidescrollPlatform_decCounter1_hook, SYM(sidescrollPlatform_decCounter1), b_+108);
  if (!(F & FZ)) { CYCT(b_+108, b_+110); goto flickerVisibility; } // jr nz
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 120);
  CYC(b_+112, b_+113); L = E;
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x01);
  CYC(b_+115, b_+118); objectSetVisible83_hook(gb); return; // jp
}
