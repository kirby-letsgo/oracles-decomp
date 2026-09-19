#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void greatFairyOctorok_createMagicPowderAnimation_hook(GB *gb);

void greatFairyOctorok_createMagicPowderAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x543a, 0x543c); A = 0x83;
  CALL_C(0x543c, playSound_b00_hook, 0x0c98, 0x543f);
  CYC(0x543f, 0x5442); SET_BC(0x00f8);

next:
  CALL_C(0x5442, getFreePartSlot_hook, 0x3e8e, 0x5445);
  if (!(F & FZ)) {
    RET_TAKEN(0x5445); return;
  }
  CYC(0x5445, 0x5446);
  CYC(0x5446, 0x5448); mem_wr(gb, HL, 0x26);
  CYC(0x5448, 0x544a); L = 0xc3;
  CYC(0x544a, 0x544b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x544b, objectCopyPositionWithOffset_hook, 0x225a, 0x544e);
  CYC(0x544e, 0x544f); A = C;
  CYC(0x544f, 0x5451); alu_add(gb, 0x08);
  CYC(0x5451, 0x5452); C = A;
  CYC(0x5452, 0x5454); alu_cp(gb, 0x18);
  if (!(F & FZ)) {
    CYCT(0x5454, 0x5456);
    goto next;
  }
  CYC(0x5454, 0x5456);
  RET(0x5456);
}
