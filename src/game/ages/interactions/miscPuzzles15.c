#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(miscPuzzles_drawCrownDungeonOpeningFrame3), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(miscPuzzles_drawCrownDungeonOpeningFrame3), (from), (to), true)

static void miscPuzzles_crownDungeonSpawnPuff(GB *gb, uint16_t sp0_) {
  BASE(miscPuzzles_drawCrownDungeonOpeningFrame3);
  CALL_C(b_+46, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+49);
  if (!(F & FZ)) { RET_TAKEN(b_+49); return; }
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x05);
  CYC(b_+52, b_+53); L = alu_inc8(gb, L);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x81);
  CYC(b_+55, b_+57); L = 0x4b;
  CYC(b_+57, b_+58); mem_wr(gb, HL, B);
  CYC(b_+58, b_+60); L = 0x4d;
  CYC(b_+60, b_+61); mem_wr(gb, HL, C);
  CYC(b_+61, SYM(objectWritePositionTocfd5)); ret_effect(gb);
}

static void miscPuzzles_crownDungeonOpeningBody(GB *gb, uint16_t sp0_) {
  BASE(miscPuzzles_drawCrownDungeonOpeningFrame3);
  CYC(b_+2, b_+3); push_effect(gb, DE);
  CYC(b_+3, b_+6); SET_HL((SYM(interactiondc_removeGraveyardGateTiles1) + 16));
  CYC(b_+6, b_+8); E = 0x02;
  CALL_C(b_+8, interBankCall_hook, 0x008a, b_+11);
  CALL_C(b_+11, reloadTileMap_hook, SYM(reloadTileMap), b_+14);
  CYC(b_+14, b_+15); SET_DE(pop_effect(gb));
  CYC(b_+15, b_+17); A = 0x0f;
  CALL_C(b_+17, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+20);
  CYC(b_+20, b_+22); A = 0x70;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+28); SET_BC((SYM(getPositionOffsetForVelocity) + 31));
  CYC(b_+28, b_+31); push_effect(gb, b_+31); miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
  CYC(b_+31, b_+34); SET_BC((SYM(getPositionOffsetForVelocity) + 47));
  CYC(b_+34, b_+37); push_effect(gb, b_+37); miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
  CYC(b_+37, b_+40); SET_BC((SYM(getPositionOffsetForVelocity) + 63));
  CYC(b_+40, b_+43); push_effect(gb, b_+43); miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
  CYC(b_+43, b_+46); SET_BC((SYM(objectGetPosition) + 6));
  miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
}

void miscPuzzles_drawCrownDungeonOpeningFrame1_hook(GB *gb) {
  BASE(miscPuzzles_drawCrownDungeonOpeningFrame1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x00;
  CYC(b_+2, SYM(miscPuzzles_drawCrownDungeonOpeningFrame2));
  miscPuzzles_crownDungeonOpeningBody(gb, sp0_);
}

void miscPuzzles_drawCrownDungeonOpeningFrame2_hook(GB *gb) {
  BASE(miscPuzzles_drawCrownDungeonOpeningFrame2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x01;
  CYC(b_+2, SYM(miscPuzzles_drawCrownDungeonOpeningFrame3));
  miscPuzzles_crownDungeonOpeningBody(gb, sp0_);
}

void miscPuzzles_drawCrownDungeonOpeningFrame3_hook(GB *gb) {
  BASE(miscPuzzles_drawCrownDungeonOpeningFrame3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x02;
  miscPuzzles_crownDungeonOpeningBody(gb, sp0_);
}
