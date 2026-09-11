#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void cutscene0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d6b, 0x7d6e); SET_HL(0x701d);
  CYC(0x7d6e, 0x7d70); E = 0x03;
  CALL_C(0x7d70, interBankCall_hook, 0x008a, 0x7d73);
  CYC(0x7d73, 0x7d76);
  updateAllObjects_hook(gb);
}

void cutscene1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d76, 0x7d79); SET_HL(0x7168);
  CYC(0x7d79, 0x7d7b); E = 0x03;
  CALL_C(0x7d7b, interBankCall_hook, 0x008a, 0x7d7e);
  CYC(0x7d7e, 0x7d81);
  updateAllObjects_hook(gb);
}

void cutscene1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d81, 0x7d83); A = hram_rd(gb, 0x70);
  CYC(0x7d83, 0x7d84); push_effect(gb, AF);
  CYC(0x7d84, 0x7d87); SET_HL(0x7244);
  CYC(0x7d87, 0x7d89); E = 0x03;
  CALL_C(0x7d89, interBankCall_hook, 0x008a, 0x7d8c);
  CYC(0x7d8c, 0x7d8d); SET_AF(pop_effect(gb));
  CYC(0x7d8d, 0x7d8f); hram_wr(gb, 0x70, A);
  CYC(0x7d8f, 0x7d92);
  updateAllObjects_hook(gb);
}

void warpToMoblinKeepUnderground_hook(GB *gb) {
  CYC(0x7d92, 0x7d95); SET_HL(0x7d98);
  CYC(0x7d95, 0x7d98);
  setWarpDestVariables_hook(gb);
}

void cutscene1c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d9d, 0x7da0); SET_HL(0x7493);
  CYC(0x7da0, 0x7da2); E = 0x03;
  CALL_C(0x7da2, interBankCall_hook, 0x008a, 0x7da5);
  CALL_C(0x7da5, updateAllObjects_hook, 0x345b, 0x7da8);
  CYC(0x7da8, 0x7dab);
  updateStatusBar_hook(gb);
}

void cutscene1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7dab, 0x7dae); SET_HL(0x7565);
  CYC(0x7dae, 0x7db0); E = 0x03;
  CALL_C(0x7db0, interBankCall_hook, 0x008a, 0x7db3);
  CYC(0x7db3, 0x7db6); SET_HL(0x6282);
  CYC(0x7db6, 0x7db8); E = 0x01;
  CALL_C(0x7db8, interBankCall_hook, 0x008a, 0x7dbb);
  CYC(0x7dbb, 0x7dbe);
  updateAllObjects_hook(gb);
}

void cutscene1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7dbe, 0x7dc1); SET_HL(0x7619);
  CYC(0x7dc1, 0x7dc3); E = 0x03;
  CALL_C(0x7dc3, interBankCall_hook, 0x008a, 0x7dc6);
  CALL_C(0x7dc6, updateStatusBar_hook, 0x1a9c, 0x7dc9);
  CYC(0x7dc9, 0x7dcc);
  updateAllObjects_hook(gb);
}
