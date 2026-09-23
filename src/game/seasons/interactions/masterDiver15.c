#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MASTER_DIVER), bank 0x15: the
// swimming challenge.

// Clears the timer bytes and puts Link's items away.
void s_seasonsFunc_15_654e_hook(GB *gb) {
  BASE(seasonsFunc_15_654e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wcce1);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+10);
  TAIL_SG(clearAllItemsAndPutLinkOnGround);
}

void s_seasonsFunc_15_6558_hook(GB *gb) {
  BASE(seasonsFunc_15_6558);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0 + 0x11, A);
  RET(b_+4); return;
}

// Link facing up at ($60,$50) on the ground, in the forced state.
void s_masterDiver_forceLinkState_hook(GB *gb) {
  BASE(masterDiver_forceLinkState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link_direction);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+10); L = 0x0b; // w1Link.yh
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x60);
  CYC(b_+12, b_+14); L = 0x0d; // w1Link.xh
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x50);
  CYC(b_+16, b_+18); L = 0x0f; // w1Link.zh
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x00);
  RET(b_+20); return;
}

// The challenge flag set when the timer (BCD minutes:seconds in wcce3/wcce2) is under 31
// seconds, else cleared.
void s_masterDiver_checkIfDoneIn30Seconds_hook(GB *gb) {
  BASE(masterDiver_checkIfDoneIn30Seconds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wcce3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto failed; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0x31);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto failed; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x2e; // GLOBALFLAG_SWIMMING_CHALLENGE_SUCCEEDED
  CYC(b_+14, b_+17);
  TAIL(setGlobalFlag);
failed:
  CYC(b_+17, b_+19); A = 0x2e; // GLOBALFLAG_SWIMMING_CHALLENGE_SUCCEEDED
  CYC(b_+19, b_+22);
  TAIL(unsetGlobalFlag);
}

void s_masterDiver_retryChallenge_hook(GB *gb) {
  BASE(masterDiver_retryChallenge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+11); // @warpDestVariables
  CALL_C(b_+3, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+6);
  CYC(b_+6, b_+8); A = 0x8d; // SND_TELEPORT
  CYC(b_+8, b_+11);
  TAIL(playSound_b00);
}

void s_masterDiver_exitChallenge_hook(GB *gb) {
  BASE(masterDiver_exitChallenge);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+11); // @warpDestVariables
  CALL_C(b_+3, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+6);
  CYC(b_+6, b_+8); A = 0x8d; // SND_TELEPORT
  CYC(b_+8, b_+11);
  TAIL(playSound_b00);
}
