#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

// INTERAC_RICKYS_GLOVE_SPAWNER
void interactionCode74_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Delete self if already returned gloves, haven't talked to Ricky, or already got gloves
  CYC(0x5e29, 0x5e2c); A = W8(wRickyState);
  CYC(0x5e2c, 0x5e2e); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(0x5e2e, 0x5e30); goto deleteThis; } // jr nz
  CYC(0x5e2e, 0x5e30);
  CYC(0x5e30, 0x5e32); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x5e32, 0x5e34); goto deleteThis; } // jr z
  CYC(0x5e32, 0x5e34);
  CYC(0x5e34, 0x5e36); A = 0x48; // TREASURE_RICKY_GLOVES
  CALL_C(0x5e36, checkTreasureObtained_hook, 0x1748, 0x5e39);
  if (F & FC) { CYCT(0x5e39, 0x5e3b); goto deleteThis; } // jr c
  CYC(0x5e39, 0x5e3b);
  CYC(0x5e3b, 0x5e3e); SET_BC(0x6048); // INTERAC_TREASURE, TREASURE_RICKY_GLOVES
  CALL_C(0x5e3e, objectCreateInteraction_hook, 0x24c5, 0x5e41);
  if (!(F & FZ)) { RET_TAKEN(0x5e41); return; } // ret nz
  CYC(0x5e41, 0x5e42);

deleteThis:
  CYC(0x5e42, 0x5e45); interactionDelete_hook(gb); return; // jp
}
