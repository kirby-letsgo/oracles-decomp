#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

static void miscPuzzles_crownDungeonSpawnPuff(GB *gb, uint16_t sp0_) {
  CALL_C(0x740b, getFreeInteractionSlot_hook, 0x3aef, 0x740e);
  if (!(F & FZ)) { RET_TAKEN(0x740e); return; }
  CYC(0x740e, 0x740f);
  CYC(0x740f, 0x7411); mem_wr(gb, HL, 0x05);
  CYC(0x7411, 0x7412); L = alu_inc8(gb, L);
  CYC(0x7412, 0x7414); mem_wr(gb, HL, 0x81);
  CYC(0x7414, 0x7416); L = 0x4b;
  CYC(0x7416, 0x7417); mem_wr(gb, HL, B);
  CYC(0x7417, 0x7419); L = 0x4d;
  CYC(0x7419, 0x741a); mem_wr(gb, HL, C);
  CYC(0x741a, 0x741b); ret_effect(gb);
}

static void miscPuzzles_crownDungeonOpeningBody(GB *gb, uint16_t sp0_) {
  CYC(0x73df, 0x73e0); push_effect(gb, DE);
  CYC(0x73e0, 0x73e3); SET_HL(0x7b83);
  CYC(0x73e3, 0x73e5); E = 0x02;
  CALL_C(0x73e5, interBankCall_hook, 0x008a, 0x73e8);
  CALL_C(0x73e8, reloadTileMap_hook, 0x12fc, 0x73eb);
  CYC(0x73eb, 0x73ec); SET_DE(pop_effect(gb));
  CYC(0x73ec, 0x73ee); A = 0x0f;
  CALL_C(0x73ee, setScreenShakeCounter_hook, 0x24bb, 0x73f1);
  CYC(0x73f1, 0x73f3); A = 0x70;
  CALL_C(0x73f3, playSound_b00_hook, 0x0c98, 0x73f6);
  CYC(0x73f6, 0x73f9); SET_BC(0x2060);
  CYC(0x73f9, 0x73fc); push_effect(gb, 0x73fc); miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
  CYC(0x73fc, 0x73ff); SET_BC(0x2070);
  CYC(0x73ff, 0x7402); push_effect(gb, 0x7402); miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
  CYC(0x7402, 0x7405); SET_BC(0x2080);
  CYC(0x7405, 0x7408); push_effect(gb, 0x7408); miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
  CYC(0x7408, 0x740b); SET_BC(0x2090);
  miscPuzzles_crownDungeonSpawnPuff(gb, sp0_);
}

void miscPuzzles_drawCrownDungeonOpeningFrame1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73d5, 0x73d7); C = 0x00;
  CYC(0x73d7, 0x73d9);
  miscPuzzles_crownDungeonOpeningBody(gb, sp0_);
}

void miscPuzzles_drawCrownDungeonOpeningFrame2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73d9, 0x73db); C = 0x01;
  CYC(0x73db, 0x73dd);
  miscPuzzles_crownDungeonOpeningBody(gb, sp0_);
}

void miscPuzzles_drawCrownDungeonOpeningFrame3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73dd, 0x73df); C = 0x02;
  miscPuzzles_crownDungeonOpeningBody(gb, sp0_);
}
