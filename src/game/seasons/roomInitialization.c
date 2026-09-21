#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/roomInitialization.s, the Seasons-only pieces.

// While dating Rosa: leaving Subrosia or entering the pirate portal room ends the date; in the
// dancing room she is not spawned; anywhere else she follows Link (spawned if not present).
void s_updateRosaDateStatus_hook(GB *gb) {
  BASE(updateRosaDateStatus);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b; // GLOBALFLAG_DATING_ROSA
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wActiveGroup);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto still_dating; }
  CYC(b_+10, b_+12);

unset:
  CYC(b_+12, b_+14); A = 0x0b; // GLOBALFLAG_DATING_ROSA
  CYC(b_+14, b_+17);
  TAIL(unsetGlobalFlag);

still_dating:
  CYC(b_+17, b_+19); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto spawn_if_not_here; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, wActiveRoom);
  CYC(b_+24, b_+26); alu_cp(gb, 0x95);
  if (F & FZ) { RET_TAKEN(b_+26); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); alu_cp(gb, 0xab);
  if (F & FZ) { CYCT(b_+29, b_+31); goto unset; }
  CYC(b_+29, b_+31);

spawn_if_not_here:
  CYC(b_+31, b_+34); SET_HL(w1ReservedInteraction1_id);
  for (;;) {
    CYC(b_+34, b_+35); A = mem_rd(gb, HL);
    CYC(b_+35, b_+37); alu_cp(gb, 0x31); // INTERAC_ROSA
    if (F & FZ) { RET_TAKEN(b_+37); return; }
    CYC(b_+37, b_+38);
    CYC(b_+38, b_+39); H = alu_inc8(gb, H);
    CYC(b_+39, b_+40); A = H;
    CYC(b_+40, b_+42); alu_cp(gb, 0xe0); // LAST_INTERACTION_INDEX+1
    if (!(F & FZ)) { CYCT(b_+42, b_+44); continue; }
    CYC(b_+42, b_+44);
    break;
  }
  CALL_C(b_+44, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+47);
  if (!(F & FZ)) { RET_TAKEN(b_+47); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x31); // INTERAC_ROSA
  CYC(b_+50, b_+51); L = alu_inc8(gb, L);
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x01);
  RET(b_+53); return;
}
