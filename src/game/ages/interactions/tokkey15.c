#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void tokkey_jump_hook(GB *gb) {
  CYC(0x7654, 0x7657); SET_BC(0xfe60);
  CYC(0x7657, 0x765a); objectSetSpeedZ_hook(gb);
}

void tokkey_centerLinkOnTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x765a, 0x765d); SET_HL(w1Link_y);
  CALL_C(0x765d, centerCoordinatesOnTile_hook, 0x20e1, 0x7660);
  CYC(0x7660, 0x7662); L = 0x08;
  CYC(0x7662, 0x7664); mem_wr(gb, HL, 0x01);
  CYC(0x7664, 0x7665); ret_effect(gb);
}

void tokkey_makeLinkPlayTuneOfCurrents_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7665, getFreeInteractionSlot_hook, 0x3aef, 0x7668);
  if (!(F & FZ)) { RET_TAKEN(0x7668); return; }
  CYC(0x7668, 0x7669);
  CYC(0x7669, 0x766b); mem_wr(gb, HL, 0xc5);
  CYC(0x766b, 0x766c); L = alu_inc8(gb, L);
  CYC(0x766c, 0x766d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x766d, 0x766e); ret_effect(gb);
}
