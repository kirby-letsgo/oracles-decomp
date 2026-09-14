#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

static uint16_t veranCutsceneWallmaster_jumpTable(GB *gb) {
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

// INTERAC_VERAN_CUTSCENE_WALLMASTER: the wallmaster that grabs Link in the Veran cutscene.
void interactionCode2c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5523, 0x5525); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5525, 0x5526); A = mem_rd(gb, DE);
  CYC(0x5526, 0x5527); push_effect(gb, 0x5527);
  switch (veranCutsceneWallmaster_jumpTable(gb)) {
    case 0x552b: goto state0;
    case 0x5542: goto state1;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x552b, 0x552d); A = 0x01;
  CYC(0x552d, 0x552e); mem_wr(gb, DE, A);
  CALL_C(0x552e, interactionInitGraphics_hook, 0x15fb, 0x5531);
  CYC(0x5531, 0x5534); SET_BC(0x0140);
  CALL_C(0x5534, objectSetSpeedZ_hook, 0x239d, 0x5537);
  CYC(0x5537, 0x5539); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5539, 0x553b); mem_wr(gb, HL, 0x14);
  CYC(0x553b, 0x553d); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x553d, 0x553f); mem_wr(gb, HL, 0xa0);
  CALL_C(0x553f, objectSetVisiblec3_hook, 0x1e4e, 0x5542);
  // falls through into state 1

state1:
  CALL_C(0x5542, interactionAnimate_hook, 0x261b, 0x5545);
  CYC(0x5545, 0x5547); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5547, 0x5548); A = mem_rd(gb, DE);
  CYC(0x5548, 0x5549); push_effect(gb, 0x5549);
  switch (veranCutsceneWallmaster_jumpTable(gb)) {
    case 0x5551: goto substate0;
    case 0x5558: goto substate1;
    case 0x5566: goto substate2;
    case 0x557e: goto substate3;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x5551, interactionDecCounter1_hook, 0x23cc, 0x5554);
  if (!(F & FZ)) {
    CYCT(0x5554, 0x5555); ret_effect(gb); return;
  }
  CYC(0x5554, 0x5555);
  CYC(0x5555, 0x5558); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x5558, 0x555a); C = 0x00;
  CALL_C(0x555a, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x555d);
  if (!(F & FZ)) {
    CYCT(0x555d, 0x555e); ret_effect(gb); return;
  }
  CYC(0x555d, 0x555e);
  CYC(0x555e, 0x5560); A = 0x01;
  CALL_C(0x5560, interactionSetAnimation_hook, 0x262e, 0x5563);
  CYC(0x5563, 0x5566); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x5566, 0x5568); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5568, 0x5569); A = mem_rd(gb, DE);
  CYC(0x5569, 0x556b); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x556b, 0x556e); interactionIncSubstate_hook(gb); return;
  }
  CYC(0x556b, 0x556e);
  CYC(0x556e, 0x556f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x556f, 0x5570); ret_effect(gb); return;
  }
  CYC(0x556f, 0x5570);
  CYC(0x5570, 0x5571); alu_xor(gb, A);
  CYC(0x5571, 0x5574); W8(w1Link_visible) = A;
  CYC(0x5574, 0x5576); A = 0x1e;
  CYC(0x5576, 0x5578); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5578, 0x5579); mem_wr(gb, DE, A);
  CYC(0x5579, 0x557b); A = 0x67; // SND_BOSS_DEAD
  CYC(0x557b, 0x557e); playSound_b00_hook(gb);
  return;

substate3:
  CYC(0x557e, 0x5580); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5580, 0x5581); A = mem_rd(gb, DE);
  CYC(0x5581, 0x5582); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5582, 0x5584); goto counterDone;
  }
  CYC(0x5582, 0x5584);
  CYC(0x5584, 0x5585); A = alu_dec8(gb, A);
  CYC(0x5585, 0x5586); mem_wr(gb, DE, A);
  CYC(0x5586, 0x5587); ret_effect(gb);
  return;

counterDone:
  CYC(0x5587, 0x5589); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5589, 0x558a); A = mem_rd(gb, DE);
  CYC(0x558a, 0x558b); A = alu_dec8(gb, A);
  CYC(0x558b, 0x558c); mem_wr(gb, DE, A);
  CYC(0x558c, 0x558e); alu_cp(gb, 0xb0);
  if (!(F & FZ)) {
    CYCT(0x558e, 0x558f); ret_effect(gb); return;
  }
  CYC(0x558e, 0x558f);
  CYC(0x558f, 0x5591); A = 0x08;
  CYC(0x5591, 0x5594); mem_wr(gb, wTmpcbb5, A);
  CYC(0x5594, 0x5597); interactionDelete_hook(gb);
}
