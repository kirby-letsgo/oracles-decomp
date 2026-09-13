#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// INTERAC_PLAY_NAYRU_MUSIC: starts Nayru's song at half volume until the intro is done,
// then deletes itself.
void interactionCode2f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55fe, 0x5600); A = 0x0a; // GLOBALFLAG_INTRO_DONE
  CALL_C(0x5600, checkGlobalFlag_hook, 0x31f3, 0x5603);
  if (!(F & FZ)) {
    CYCT(0x5603, 0x5606); interactionDelete_hook(gb); return;
  }
  CYC(0x5603, 0x5606);
  CYC(0x5606, 0x5609); SET_HL(wActiveMusic);
  CYC(0x5609, 0x560b); A = 0x08; // MUS_NAYRU
  CYC(0x560b, 0x560c); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x560c, 0x560e); goto setVolume;
  }
  CYC(0x560c, 0x560e);
  CYC(0x560e, 0x560f); mem_wr(gb, HL, A);
  CALL_C(0x560f, playSound_b00_hook, 0x0c98, 0x5612);

setVolume:
  CYC(0x5612, 0x5614); A = 0x02;
  CALL_C(0x5614, setMusicVolume_hook, 0x0cad, 0x5617);
  CYC(0x5617, 0x561a); interactionDelete_hook(gb);
}
