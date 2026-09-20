#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define librarianScript_bank0c SYM(librarianScript)

// @state1
static void librarian_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2a);
  CALL_C(b_+5, interactionRunScript_hook, SYM(interactionRunScript), b_+8);
  CYC(b_+8, b_+11); interactionAnimateAsNpc_hook(gb);
}

// @state0: init. Text is TX_2715 or TX_2716 depending on whether the water pollution is fixed.
static void librarian_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode2a);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x27); // >TX_2700
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x0c);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x06);
  CYC(b_+28, b_+30); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(b_+30, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+33);
  CYC(b_+33, b_+35); A = 0x15; // <TX_2715
  if (F & FZ) {
    CYCT(b_+35, b_+37); goto setTextId;
  }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); A = 0x16; // <TX_2716

setTextId:
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CALL_C(b_+42, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+45);
  CYC(b_+45, b_+48); SET_HL(librarianScript_bank0c);
  CYC(b_+48, b_+51); interactionSetScript_hook(gb);
}

// INTERAC_LIBRARIAN
void interactionCode2a_hook(GB *gb) {
  BASE(interactionCode2a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); librarian_state0(gb, sp0_); return;
  }
  CYC(b_+3, b_+5);
  librarian_state1(gb, sp0_);
}
