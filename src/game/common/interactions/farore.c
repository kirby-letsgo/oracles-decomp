#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// mainScripts.faroreScript (bank $0c script data, referenced by address only).
#define faroreScript_bank0c 0x45f5

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
  uint16_t sp0_ = gb->sp;
  CYC(0x4155, 0x4157); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4157, 0x4158); A = mem_rd(gb, DE);
  CYC(0x4158, 0x4159); push_effect(gb, 0x4159);
  switch (farore_jumpTable(gb)) {
    case 0x415d: goto state0;
    case 0x4180: goto state1;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x415d, 0x415f); A = 0x01;
  CYC(0x415f, 0x4160); mem_wr(gb, DE, A);
  CALL_C(0x4160, interactionInitGraphics_hook, 0x15fb, 0x4163);
  CYC(0x4163, 0x4165); A = 0x55;
  CALL_C(0x4165, interactionSetHighTextIndex_hook, 0x253b, 0x4168);
  CYC(0x4168, 0x416b); SET_HL(faroreScript_bank0c);
  CALL_C(0x416b, interactionSetScript_hook, 0x2544, 0x416e);
  CYC(0x416e, 0x4170); A = 0x2c;
  CALL_C(0x4170, unsetGlobalFlag_hook, 0x31ff, 0x4173);
  CYC(0x4173, 0x4175); A = 0x08;
  CYC(0x4175, 0x4178); mem_wr(gb, wTextboxFlags, A);
  CYC(0x4178, 0x417a); A = 0x02;
  CYC(0x417a, 0x417d); mem_wr(gb, wTextboxPosition, A);
  CYC(0x417d, 0x4180); objectSetVisible82_hook(gb);
  return;

state1:
  CYC(0x4180, 0x4183); SET_BC(0x1406);
  CALL_C(0x4183, objectSetCollideRadii_hook, 0x24a9, 0x4186);
  CALL_C(0x4186, interactionRunScript_hook, 0x2552, 0x4189);
  CYC(0x4189, 0x418c); interactionAnimate_hook(gb);
}
