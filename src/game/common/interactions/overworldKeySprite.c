#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode18);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (overworldKeySprite_jumpTable(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+28) { goto state1; }
    else if (jt_ == b_+47) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+10, interactionIncState_hook, SYM(interactionIncState), b_+13);
  CYC(b_+13, b_+16); SET_BC(0xfe00);
  CALL_C(b_+16, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+19);
  CALL_C(b_+19, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+22);
  CALL_C(b_+22, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+25);
  CYC(b_+25, b_+28); objectSetVisible80_hook(gb);
  return;

state1:
  // Decrease speedZ, wait for it to stop moving up
  CYC(b_+28, b_+30); C = 0x28;
  CALL_C(b_+30, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+33);
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_SPEED_Z + 1;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+39); ret_effect(gb); return;
  }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+41, b_+43); A = 0x3c;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+47); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(b_+47, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+50);
  if (!(F & FZ)) {
    CYCT(b_+50, b_+51); ret_effect(gb); return;
  }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+54); TAIL(interactionDelete);
}
