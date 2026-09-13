#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define adlarScript_bank0c 0x4cef

// @state1
static void adlar_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x5448, interactionRunScript_hook, 0x2552, 0x544b);
  CYC(0x544b, 0x544e); interactionAnimateAsNpc_hook(gb);
}

// @state0: init. Decide on a value to write to var38; this will affect the script.
static void adlar_state0(GB *gb, uint16_t sp0_) {
  CALL_C(0x544e, interactionInitGraphics_hook, 0x15fb, 0x5451);
  CALL_C(0x5451, interactionIncState_hook, 0x23e0, 0x5454);
  CYC(0x5454, 0x5456); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x5456, checkGlobalFlag_hook, 0x31f3, 0x5459);
  CYC(0x5459, 0x545b); A = 0x04;
  if (!(F & FZ)) {
    CYCT(0x545b, 0x545d); goto setVar38;
  }
  CYC(0x545b, 0x545d);
  CYC(0x545d, 0x5460); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x5460, 0x5462); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x5462, 0x5464); A = 0x03;
  if (!(F & FZ)) {
    CYCT(0x5464, 0x5466); goto setVar38;
  }
  CYC(0x5464, 0x5466);
  CYC(0x5466, 0x5468); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(0x5468, checkGlobalFlag_hook, 0x31f3, 0x546b);
  CYC(0x546b, 0x546d); A = 0x02;
  if (!(F & FZ)) {
    CYCT(0x546d, 0x546f); goto setVar38;
  }
  CYC(0x546d, 0x546f);
  CALL_C(0x546f, getThisRoomFlags_hook, 0x197d, 0x5472);
  CYC(0x5472, 0x5474); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(0x5474, 0x5476); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x5476, 0x5478); goto setVar38;
  }
  CYC(0x5476, 0x5478);
  CYC(0x5478, 0x5479); alu_xor(gb, A);

setVar38:
  CYC(0x5479, 0x547b); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x547b, 0x547c); mem_wr(gb, DE, A);
  CALL_C(0x547c, objectSetVisiblec2_hook, 0x1e45, 0x547f);
  CYC(0x547f, 0x5482); SET_HL(adlarScript_bank0c);
  CYC(0x5482, 0x5485); interactionSetScript_hook(gb);
}

// INTERAC_ADLAR
void interactionCode29_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5443, checkInteractionState_hook, 0x23fe, 0x5446);
  if (F & FZ) {
    CYCT(0x5446, 0x5448); adlar_state0(gb, sp0_); return;
  }
  CYC(0x5446, 0x5448);
  adlar_state1(gb, sp0_);
}
