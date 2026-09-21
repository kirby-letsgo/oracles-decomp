#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscellaneous1.s, bank $09.

// State 0 remembers the position of the hidden stairs tile (index 4) under the object; state 1
// waits until that tile is gone (the secret was told) and then opens the entrance.
void s_tickTockSecretEntrance_hook(GB *gb) {
  BASE(tickTockSecretEntrance);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+10); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); A = L;
  CYC(b_+12, b_+15); mem_wr(gb, wccaa, A);
  CYC(b_+15, b_+17); E = 0x44; // Interaction.state
  CYC(b_+17, b_+19); A = 0x01;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;

state1:
  CALL_C(b_+21, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+24);
  CALL_C(b_+24, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+27);
  CYC(b_+27, b_+29); alu_cp(gb, 0x04);
  if (F & FZ) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  TAIL_SG(setEnteredWarpSetStairsPlaySolvedSound);
}

void s_staticHeartPiece_hook(GB *gb) {
  BASE(staticHeartPiece);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x2b00); // TREASURE_HEART_PIECE, subid 0
  TAIL_SG(misc1_spawnTreasureBCifRoomFlagBit5NotSet);
}
