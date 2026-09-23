#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MOBLIN_KEEP_SCENES), bank 0x15.

// Marks the keep destroyed and warps Link out.
void s_moblinKeepScene_warpOutOfMoblinKeep_hook(GB *gb) {
  BASE(moblinKeepScene_warpOutOfMoblinKeep);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x16; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+2, setGlobalFlag_hook, SYM(setGlobalFlag), b_+5);
  CYC(b_+5, b_+7); A = 0x2f; // GLOBALFLAG_DONT_DISPLAY_SEASON_INFO
  CALL_C(b_+7, setGlobalFlag_hook, SYM(setGlobalFlag), b_+10);
  CYC(b_+10, b_+13); SET_HL(b_+21); // @warpDestVariables
  CALL_C(b_+13, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+16);
  CYC(b_+16, b_+18); A = 0xbc;
  CYC(b_+18, b_+21);
  TAIL(playSound_b00);
}

void s_moblinKeepScene_faceLinkUp_hook(GB *gb) {
  BASE(moblinKeepScene_faceLinkUp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00; // DIR_UP
  CYC(b_+2, b_+5); mem_wr(gb, w1Link_direction, A);
  CYC(b_+5, b_+8);
  TAIL(setLinkForceStateToState08);
}

void s_moblinKeepScene_putLinkOnGround_hook(GB *gb) {
  BASE(moblinKeepScene_putLinkOnGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6);
  TAIL(putLinkOnGround);
}
