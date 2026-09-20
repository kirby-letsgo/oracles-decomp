#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodea7_jump_table(GB *gb) {
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

// ==================================================================================================
// INTERAC_ENDGAME_CUTSCENE_BIPSOM_FAMILY
// ==================================================================================================
void interactionCodea7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5dcb, 0x5dcd); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5dcd, 0x5dce); A = mem_rd(gb, DE);
  CYC(0x5dce, 0x5dcf); push_effect(gb, 0x5dcf);
  switch (interactionCodea7_jump_table(gb)) {
    case 0x5dd3: goto state0;
    case 0x5e10: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x5dd3, 0x5dd5); A = 0x01;
  CYC(0x5dd5, 0x5dd6); mem_wr(gb, DE, A); // [state]
  CALL_C(0x5dd6, interactionInitGraphics_hook, 0x15fb, 0x5dd9);
  CALL_C(0x5dd9, objectSetVisible82_hook, 0x1e69, 0x5ddc);
  CYC(0x5ddc, 0x5dde); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5dde, 0x5ddf); A = mem_rd(gb, DE);
  CYC(0x5ddf, 0x5de1); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x5de1, 0x5de2); ret_effect(gb); return; } // ret nz
  CYC(0x5de1, 0x5de2);
  CYC(0x5de2, 0x5de5); A = mem_rd(gb, wChildStage);
  CYC(0x5de5, 0x5de7); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(0x5de7, 0x5de8); ret_effect(gb); return; } // ret c
  CYC(0x5de7, 0x5de8);
  CYC(0x5de8, 0x5dea); A = 0x04;
  CALL_C(0x5dea, interactionSetAnimation_hook, 0x262e, 0x5ded);
  CALL_C(0x5ded, getFreeInteractionSlot_hook, 0x3aef, 0x5df0);
  if (!(F & FZ)) { CYCT(0x5df0, 0x5df1); ret_effect(gb); return; } // ret nz
  CYC(0x5df0, 0x5df1);
  CYC(0x5df1, 0x5df3); mem_wr(gb, HL, 0x35); // INTERAC_CHILD
  CYC(0x5df3, 0x5df4); L = alu_inc8(gb, L);
  CYC(0x5df4, 0x5df7); A = mem_rd(gb, wChildStage);
  CYC(0x5df7, 0x5df9); B = 0x00;
  CYC(0x5df9, 0x5dfb); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(0x5dfb, 0x5dfd); goto l_5dff; } // jr c
  CYC(0x5dfb, 0x5dfd);
  CYC(0x5dfd, 0x5dff); B = 0x03;

l_5dff:
  CYC(0x5dff, 0x5e02); A = mem_rd(gb, wChildPersonality);
  CYC(0x5e02, 0x5e03); alu_add(gb, B);
  CYC(0x5e03, 0x5e04); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [child.subid]
  CYC(0x5e04, 0x5e06); alu_add(gb, 0x16);
  CYC(0x5e06, 0x5e07); mem_wr(gb, HL, A);
  CYC(0x5e07, 0x5e09); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5e09, 0x5e0b); mem_wr(gb, HL, 0x38);
  CYC(0x5e0b, 0x5e0c); L = alu_inc8(gb, L);
  CYC(0x5e0c, 0x5e0d); L = alu_inc8(gb, L);
  CYC(0x5e0d, 0x5e0f); mem_wr(gb, HL, 0x28);
  RET(0x5e0f); return;

state1:
  CYC(0x5e10, 0x5e12); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5e12, 0x5e13); A = mem_rd(gb, DE);
  CYC(0x5e13, 0x5e14); push_effect(gb, 0x5e14);
  switch (interactionCodea7_jump_table(gb)) {
    case 0x5e1a: goto substate0;
    case 0x5e2c: goto substate1;
    case 0x5e3a: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x5e1a, 0x5e1d); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x5e1d, 0x5e1e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5e1e, 0x5e20); goto l_5e29; } // jr z
  CYC(0x5e1e, 0x5e20);
  CALL_C(0x5e20, interactionIncSubstate_hook, 0x23e5, 0x5e23);
  CYC(0x5e23, 0x5e26); SET_BC(0xff00); // -$100
  CALL_C(0x5e26, objectSetSpeedZ_hook, 0x239d, 0x5e29);

l_5e29:
  CYC(0x5e29, 0x5e2c); interactionAnimate_hook(gb); return; // jp

substate1:
  CYC(0x5e2c, 0x5e2e); C = 0x20;
  CALL_C(0x5e2e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5e31);
  if (!(F & FZ)) { CYCT(0x5e31, 0x5e32); ret_effect(gb); return; } // ret nz
  CYC(0x5e31, 0x5e32);
  CALL_C(0x5e32, interactionIncSubstate_hook, 0x23e5, 0x5e35);
  CYC(0x5e35, 0x5e37); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5e37, 0x5e39); mem_wr(gb, HL, 0x0a);
  RET(0x5e39); return;

substate2:
  CALL_C(0x5e3a, interactionDecCounter1_hook, 0x23cc, 0x5e3d);
  if (!(F & FZ)) { CYCT(0x5e3d, 0x5e3e); ret_effect(gb); return; } // ret nz
  CYC(0x5e3d, 0x5e3e);
  CYC(0x5e3e, 0x5e40); A = 0x03;
  CYC(0x5e40, 0x5e43); interactionSetAnimation_hook(gb); return; // jp
}
