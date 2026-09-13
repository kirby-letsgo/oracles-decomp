#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

static uint16_t overworldKeySprite_jumpTable(GB *gb) {
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

// INTERAC_OVERWORLD_KEY_SPRITE: key sprite that pops up out of the ground on the overworld.
void interactionCode18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x468c, 0x468e); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x468e, 0x468f); A = mem_rd(gb, DE);
  CYC(0x468f, 0x4690); push_effect(gb, 0x4690);
  switch (overworldKeySprite_jumpTable(gb)) {
    case 0x4696: goto state0;
    case 0x46a8: goto state1;
    case 0x46bb: goto state2;
    default: HANDOFF(HL);
  }

state0:
  CALL_C(0x4696, interactionIncState_hook, 0x23e0, 0x4699);
  CYC(0x4699, 0x469c); SET_BC(0xfe00);
  CALL_C(0x469c, objectSetSpeedZ_hook, 0x239d, 0x469f);
  CALL_C(0x469f, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x46a2);
  CALL_C(0x46a2, interactionInitGraphics_hook, 0x15fb, 0x46a5);
  CYC(0x46a5, 0x46a8); objectSetVisible80_hook(gb);
  return;

state1:
  // Decrease speedZ, wait for it to stop moving up
  CYC(0x46a8, 0x46aa); C = 0x28;
  CALL_C(0x46aa, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x46ad);
  CYC(0x46ad, 0x46af); E = INTERACTION_BASE + OBJ_SPEED_Z + 1;
  CYC(0x46af, 0x46b0); A = mem_rd(gb, DE);
  CYC(0x46b0, 0x46b2); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x46b2, 0x46b3); ret_effect(gb); return;
  }
  CYC(0x46b2, 0x46b3);
  CYC(0x46b3, 0x46b5); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x46b5, 0x46b7); A = 0x3c;
  CYC(0x46b7, 0x46b8); mem_wr(gb, DE, A);
  CYC(0x46b8, 0x46bb); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(0x46bb, interactionDecCounter1_hook, 0x23cc, 0x46be);
  if (!(F & FZ)) {
    CYCT(0x46be, 0x46bf); ret_effect(gb); return;
  }
  CYC(0x46be, 0x46bf);
  CYC(0x46bf, 0x46c2); interactionDelete_hook(gb);
}
