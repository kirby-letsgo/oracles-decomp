#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode8c_jump_table(GB *gb) {
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

// INTERAC_TOKAY_MEAT
void interactionCode8c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6abc, 0x6abe); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6abe, 0x6abf); A = mem_rd(gb, DE);
  {
    CYC(0x6abf, 0x6ac0); push_effect(gb, 0x6ac0);
    uint16_t target = interactionCode8c_jump_table(gb);
    if (target == 0x6ae7) goto state1;
    if (target == 0x6b18) goto state2;
    if (target == 0x6b45) goto state3;
  }

  // interactionCode8c@state0
  CALL_C(0x6ac8, interactionInitGraphics_hook, 0x15fb, 0x6acb);
  CALL_C(0x6acb, interactionIncState_hook, 0x23e0, 0x6ace);
  CYC(0x6ace, 0x6ad0); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6ad0, 0x6ad2); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x6ad2, 0x6ad4); A = 0x08;
  CALL_C(0x6ad4, objectSetCollideRadius_hook, 0x24a1, 0x6ad7);
  CYC(0x6ad7, 0x6ada); SET_BC(0x3850);
  CALL_C(0x6ada, interactionSetPosition_hook, 0x2773, 0x6add);
  CYC(0x6add, 0x6adf); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6adf, 0x6ae1); mem_wr(gb, HL, 0xc0); // -0x40
  CYC(0x6ae1, 0x6ae4); SET_BC(0x0000);
  CYC(0x6ae4, 0x6ae7); objectSetSpeedZ_hook(gb); return; // jp

state1:
  CALL_C(0x6ae7, objectAddToGrabbableObjectBuffer_hook, 0x2c2e, 0x6aea);
  CYC(0x6aea, 0x6aec); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6aec, 0x6aed); A = mem_rd(gb, DE);
  {
    CYC(0x6aed, 0x6aee); push_effect(gb, 0x6aee);
    uint16_t target = interactionCode8c_jump_table(gb);
    if (target == 0x6b07) goto state1_substate1;
    if (target == 0x6b15) goto state1_substate2;
  }

  // interactionCode8c@state1@substate0 -- starts falling
  CYC(0x6af4, 0x6af5); H = D;
  CYC(0x6af5, 0x6af7); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6af7, 0x6af8); A = mem_rd(gb, HL);
  CYC(0x6af8, 0x6af9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6af9, 0x6afc); interactionDecCounter1_hook(gb); return; } // jp nz
  CYC(0x6af9, 0x6afc);
  CALL_C(0x6afc, interactionIncSubstate_hook, 0x23e5, 0x6aff);
  CALL_C(0x6aff, objectSetVisiblec1_hook, 0x1e3c, 0x6b02);
  CYC(0x6b02, 0x6b04); A = 0x59; // SND_FALLINHOLE
  CYC(0x6b04, 0x6b07); playSound_b00_hook(gb); return; // jp

state1_substate1: // Wait for it to land
  CYC(0x6b07, 0x6b09); C = 0x28;
  CALL_C(0x6b09, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6b0c);
  if (!(F & FZ)) { RET_TAKEN(0x6b0c); return; } // ret nz
  CYC(0x6b0c, 0x6b0d);
  CALL_C(0x6b0d, interactionIncSubstate_hook, 0x23e5, 0x6b10);
  CYC(0x6b10, 0x6b12); A = 0x52; // SND_BOMB_LAND
  CYC(0x6b12, 0x6b15); playSound_b00_hook(gb); return; // jp

state1_substate2: // Sitting on the ground
  CYC(0x6b15, 0x6b18); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; // jp

// State 2 = grabbed by power bracelet state
state2:
  CYC(0x6b18, 0x6b19); E = alu_inc8(gb, E); // inc e
  CYC(0x6b19, 0x6b1a); A = mem_rd(gb, DE);
  {
    CYC(0x6b1a, 0x6b1b); push_effect(gb, 0x6b1b);
    uint16_t target = interactionCode8c_jump_table(gb);
    if (target == 0x6b32) goto beingHeld;
    if (target == 0x6b33) goto released;
  }

  // interactionCode8c@justGrabbed
  CYC(0x6b21, 0x6b22); A = D;
  CYC(0x6b22, 0x6b25); W8(wTmpcfc0_wildTokay_activeMeatObject) = A;
  CYC(0x6b25, 0x6b26); A = E;
  CYC(0x6b26, 0x6b29); mem_wr(gb, 0xcfdb, A); // wTmpcfc0.wildTokay.activeMeatObject+1
  CALL_C(0x6b29, getFreeInteractionSlot_hook, 0x3aef, 0x6b2c);
  if (!(F & FZ)) { RET_TAKEN(0x6b2c); return; } // ret nz
  CYC(0x6b2c, 0x6b2d);
  CYC(0x6b2d, 0x6b2f); mem_wr(gb, HL, 0x8c); // INTERAC_TOKAY_MEAT
  CYC(0x6b2f, 0x6b32); interactionIncSubstate_hook(gb); return; // jp

beingHeld:
  RET(0x6b32); return; // ret

released:
  CYC(0x6b33, 0x6b35); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6b35, 0x6b36); A = mem_rd(gb, DE);
  CYC(0x6b36, 0x6b37); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x6b37); return; } // ret c
  CYC(0x6b37, 0x6b38);
  CALL_C(0x6b38, dropLinkHeldItem_hook, 0x2c43, 0x6b3b);
  CALL_C(0x6b3b, interactionIncState_hook, 0x23e0, 0x6b3e);
  CYC(0x6b3e, 0x6b40); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6b40, 0x6b42); mem_wr(gb, HL, 0x14); // 20
  CYC(0x6b42, 0x6b45); objectSetVisible83_hook(gb); return; // jp

state3: // Disappearing after being dropped on the ground
  CALL_C(0x6b45, interactionDecCounter1_hook, 0x23cc, 0x6b48);
  if (!(F & FZ)) { CYCT(0x6b48, 0x6b4a); goto state3_stillDisappearing; } // jr nz
  CYC(0x6b48, 0x6b4a);
  CYC(0x6b4a, 0x6b4d); interactionDelete_hook(gb); return; // jp

state3_stillDisappearing:
  CYC(0x6b4d, 0x6b50); A = W8(wFrameCounter);
  CYC(0x6b50, 0x6b52); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x6b52, 0x6b55); objectSetInvisible_hook(gb); return; } // jp z
  CYC(0x6b52, 0x6b55);
  CYC(0x6b55, 0x6b58); objectSetPriorityRelativeToLink_hook(gb); return; // jp
}
