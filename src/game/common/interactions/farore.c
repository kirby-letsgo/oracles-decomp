#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode10), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode10), (from), (to), true)

// mainScripts.faroreScript (bank $0c script data, referenced by address only).
#define faroreScript_bank0c SYM(faroreScript)

static uint16_t farore_jumpTable(GB *gb) {
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

void interactionCode10_hook(GB *gb) {
  BASE(interactionCode10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (farore_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+43) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); A = 0x55;
  CALL_C(b_+16, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+19);
  CYC(b_+19, b_+22); SET_HL(faroreScript_bank0c);
  CALL_C(b_+22, interactionSetScript_hook, SYM(interactionSetScript), b_+25);
  CYC(b_+25, b_+27); A = 0x2c;
  CALL_C(b_+27, unsetGlobalFlag_hook, SYM(unsetGlobalFlag), b_+30);
  CYC(b_+30, b_+32); A = 0x08;
  CYC(b_+32, b_+35); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+35, b_+37); A = 0x02;
  CYC(b_+37, b_+40); mem_wr(gb, wTextboxPosition, A);
  CYC(b_+40, b_+43); objectSetVisible82_hook(gb);
  return;

state1:
  CYC(b_+43, b_+46); SET_BC((SYM(extractColorComponents) + 29));
  CALL_C(b_+46, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+49);
  CALL_C(b_+49, interactionRunScript_hook, SYM(interactionRunScript), b_+52);
  CYC(b_+52, SYM(interactionCode11)); interactionAnimate_hook(gb);
}
