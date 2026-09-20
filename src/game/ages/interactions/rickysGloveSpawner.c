#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode74), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode74), (from), (to), true)

// INTERAC_RICKYS_GLOVE_SPAWNER
void interactionCode74_hook(GB *gb) {
  BASE(interactionCode74);
  uint16_t sp0_ = gb->sp;
  // Delete self if already returned gloves, haven't talked to Ricky, or already got gloves
  CYC(b_+0, b_+3); A = W8(wRickyState);
  CYC(b_+3, b_+5); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto deleteThis; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+9, b_+11); goto deleteThis; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x48; // TREASURE_RICKY_GLOVES
  CALL_C(b_+13, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+16);
  if (F & FC) { CYCT(b_+16, b_+18); goto deleteThis; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); SET_BC((SYM(interactionCode77__foundMatch) + 11)); // INTERAC_TREASURE, TREASURE_RICKY_GLOVES
  CALL_C(b_+21, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+24);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; } // ret nz
  CYC(b_+24, b_+25);

deleteThis:
  CYC(b_+25, SYM(interactionCode75)); interactionDelete_hook(gb); return; // jp
}
