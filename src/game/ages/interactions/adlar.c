#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode29), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode29), (from), (to), true)

#define adlarScript_bank0c SYM(adlarScript)

// @state1
static void adlar_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode29);
  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  CYC(b_+8, b_+11); interactionAnimateAsNpc_hook(gb);
}

// @state0: init. Decide on a value to write to var38; this will affect the script.
static void adlar_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode29);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
  CYC(b_+17, b_+19); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+19, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+22);
  CYC(b_+22, b_+24); A = 0x04;
  if (!(F & FZ)) {
    CYCT(b_+24, b_+26); goto setVar38;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+29); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+29, b_+31); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+31, b_+33); A = 0x03;
  if (!(F & FZ)) {
    CYCT(b_+33, b_+35); goto setVar38;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(b_+37, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+40);
  CYC(b_+40, b_+42); A = 0x02;
  if (!(F & FZ)) {
    CYCT(b_+42, b_+44); goto setVar38;
  }
  CYC(b_+42, b_+44);
  CALL_C(b_+44, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+47);
  CYC(b_+47, b_+49); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(b_+49, b_+51); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+51, b_+53); goto setVar38;
  }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+54); alu_xor(gb, A);

setVar38:
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CALL_C(b_+57, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+60);
  CYC(b_+60, b_+63); SET_HL(adlarScript_bank0c);
  CYC(b_+63, SYM(interactionCode2a)); interactionSetScript_hook(gb);
}

// INTERAC_ADLAR
void interactionCode29_hook(GB *gb) {
  BASE(interactionCode29);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); adlar_state0(gb, sp0_); return;
  }
  CYC(b_+3, b_+5);
  adlar_state1(gb, sp0_);
}
