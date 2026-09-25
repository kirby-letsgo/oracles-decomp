#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/usedRodOfSeasons.s.
// INTERAC_USED_ROD_OF_SEASONS

// INTERAC_USED_ROD_OF_SEASONS
void s_interactionCode15_hook(GB *gb) {
  BASE(interactionCode15);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wMenuDisabled);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+7, b_+8); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_5169; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wActiveGroup);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto L_5169; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); SET_HL(wObtainedSeasons);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_add(gb, A);
  if (F & FZ) { CYCT(b_+21, b_+23); goto L_5169; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wRoomStateModifier);
L_5152:
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);
  CYC(b_+27, b_+29); alu_and(gb, 0x03);
  CYC(b_+29, b_+30); B = A;
  CALL_C(b_+30, s_checkFlag, SYM(checkFlag), b_+33);
  CYC(b_+33, b_+34); A = B;
  if (F & FZ) { CYCT(b_+34, b_+36); goto L_5152; }
  CYC(b_+34, b_+36);
  CALL_C(b_+36, s_setSeason_b00_hook, SYM(setSeason), b_+39);
  CYC(b_+39, b_+41); A = 0x5c;
  CALL_C(b_+41, s_playSound, SYM(playSound_b00), b_+44);
  CYC(b_+44, b_+46); A = 0x02;
  CYC(b_+46, b_+49); mem_wr(gb, wPaletteThread_updateRate, A);
L_5169:
  CYC(b_+49, b_+52);
  TAIL(interactionDelete);
}

