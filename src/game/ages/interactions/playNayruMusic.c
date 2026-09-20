#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// INTERAC_PLAY_NAYRU_MUSIC: starts Nayru's song at half volume until the intro is done,
// then deletes itself.
void interactionCode2f_hook(GB *gb) {
  BASE(interactionCode2f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0a; // GLOBALFLAG_INTRO_DONE
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8); interactionDelete_hook(gb); return;
  }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); SET_HL(wActiveMusic);
  CYC(b_+11, b_+13); A = 0x08; // MUS_NAYRU
  CYC(b_+13, b_+14); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+14, b_+16); goto setVolume;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CALL_C(b_+17, playSound_b00_hook, SYM(playSound_b00), b_+20);

setVolume:
  CYC(b_+20, b_+22); A = 0x02;
  CALL_C(b_+22, setMusicVolume_hook, SYM(setMusicVolume), b_+25);
  CYC(b_+25, b_+28); interactionDelete_hook(gb);
}
