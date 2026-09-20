#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode8c), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode8c), (from), (to), true)

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
  BASE(interactionCode8c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode8c_jump_table(gb);
    if (target == b_+43) goto state1;
    if (target == b_+92) goto state2;
    if (target == b_+137) goto state3;
  }

  // interactionCode8c@state0
  CALL_C(b_+12, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+15);
  CALL_C(b_+15, interactionIncState_hook, SYM(interactionIncState), b_+18);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+22, b_+24); A = 0x08;
  CALL_C(b_+24, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+27);
  CYC(b_+27, b_+30); SET_BC((SYM(loadUniqueGfxHeaderEntry) + 13));
  CALL_C(b_+30, interactionSetPosition_hook, SYM(interactionSetPosition), b_+33);
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0xc0); // -0x40
  CYC(b_+37, b_+40); SET_BC(0x0000);
  CYC(b_+40, b_+43); objectSetSpeedZ_hook(gb); return; // jp

state1:
  CALL_C(b_+43, objectAddToGrabbableObjectBuffer_hook, SYM(objectAddToGrabbableObjectBuffer), b_+46);
  CYC(b_+46, b_+48); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  {
    CYC(b_+49, b_+50); push_effect(gb, b_+50);
    uint16_t target = interactionCode8c_jump_table(gb);
    if (target == b_+75) goto state1_substate1;
    if (target == b_+89) goto state1_substate2;
  }

  // interactionCode8c@state1@substate0 -- starts falling
  CYC(b_+56, b_+57); H = D;
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+61, b_+64); interactionDecCounter1_hook(gb); return; } // jp nz
  CYC(b_+61, b_+64);
  CALL_C(b_+64, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+67);
  CALL_C(b_+67, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+70);
  CYC(b_+70, b_+72); A = 0x59; // SND_FALLINHOLE
  CYC(b_+72, b_+75); playSound_b00_hook(gb); return; // jp

state1_substate1: // Wait for it to land
  CYC(b_+75, b_+77); C = 0x28;
  CALL_C(b_+77, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; } // ret nz
  CYC(b_+80, b_+81);
  CALL_C(b_+81, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+84);
  CYC(b_+84, b_+86); A = 0x52; // SND_BOMB_LAND
  CYC(b_+86, b_+89); playSound_b00_hook(gb); return; // jp

state1_substate2: // Sitting on the ground
  CYC(b_+89, b_+92); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; // jp

// State 2 = grabbed by power bracelet state
state2:
  CYC(b_+92, b_+93); E = alu_inc8(gb, E); // inc e
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  {
    CYC(b_+94, b_+95); push_effect(gb, b_+95);
    uint16_t target = interactionCode8c_jump_table(gb);
    if (target == b_+118) goto beingHeld;
    if (target == b_+119) goto released;
  }

  // interactionCode8c@justGrabbed
  CYC(b_+101, b_+102); A = D;
  CYC(b_+102, b_+105); W8(wTmpcfc0_wildTokay_activeMeatObject) = A;
  CYC(b_+105, b_+106); A = E;
  CYC(b_+106, b_+109); mem_wr(gb, wTmpcfc0_goronDance_numFailedRounds, A); // wTmpcfc0.wildTokay.activeMeatObject+1
  CALL_C(b_+109, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+112);
  if (!(F & FZ)) { RET_TAKEN(b_+112); return; } // ret nz
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x8c); // INTERAC_TOKAY_MEAT
  CYC(b_+115, b_+118); interactionIncSubstate_hook(gb); return; // jp

beingHeld:
  RET(b_+118); return; // ret

released:
  CYC(b_+119, b_+121); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+121, b_+122); A = mem_rd(gb, DE);
  CYC(b_+122, b_+123); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+123); return; } // ret c
  CYC(b_+123, b_+124);
  CALL_C(b_+124, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+127);
  CALL_C(b_+127, interactionIncState_hook, SYM(interactionIncState), b_+130);
  CYC(b_+130, b_+132); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x14); // 20
  CYC(b_+134, b_+137); objectSetVisible83_hook(gb); return; // jp

state3: // Disappearing after being dropped on the ground
  CALL_C(b_+137, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+140);
  if (!(F & FZ)) { CYCT(b_+140, b_+142); goto state3_stillDisappearing; } // jr nz
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+145); interactionDelete_hook(gb); return; // jp

state3_stillDisappearing:
  CYC(b_+145, b_+148); A = W8(wFrameCounter);
  CYC(b_+148, b_+150); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+150, b_+153); objectSetInvisible_hook(gb); return; } // jp z
  CYC(b_+150, b_+153);
  CYC(b_+153, SYM(interactionCode8d)); objectSetPriorityRelativeToLink_hook(gb); return; // jp
}
