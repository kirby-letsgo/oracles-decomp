#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(tokkey_jump), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(tokkey_jump), (from), (to), true)

void tokkey_jump_hook(GB *gb) {
  BASE(tokkey_jump);
  CYC(b_+0, b_+3); SET_BC(0xfe60);
  CYC(b_+3, SYM(tokkey_centerLinkOnTile)); objectSetSpeedZ_hook(gb);
}

void tokkey_centerLinkOnTile_hook(GB *gb) {
  BASE(tokkey_centerLinkOnTile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Link_y);
  CALL_C(b_+3, centerCoordinatesOnTile_hook, SYM(centerCoordinatesOnTile), b_+6);
  CYC(b_+6, b_+8); L = 0x08;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x01);
  CYC(b_+10, SYM(tokkey_makeLinkPlayTuneOfCurrents)); ret_effect(gb);
}

void tokkey_makeLinkPlayTuneOfCurrents_hook(GB *gb) {
  BASE(tokkey_makeLinkPlayTuneOfCurrents);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xc5);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+8, SYM(tokkayScript_justHeardTune_body)); ret_effect(gb);
}
