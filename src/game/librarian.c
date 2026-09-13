#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define librarianScript_bank0c 0x4d1a

// @state1
static void librarian_state1(GB *gb, uint16_t sp0_) {
  CALL_C(0x548a, interactionRunScript_hook, 0x2552, 0x548d);
  CYC(0x548d, 0x5490); interactionAnimateAsNpc_hook(gb);
}

// @state0: init. Text is TX_2715 or TX_2716 depending on whether the water pollution is fixed.
static void librarian_state0(GB *gb, uint16_t sp0_) {
  CALL_C(0x5490, interactionInitGraphics_hook, 0x15fb, 0x5493);
  CALL_C(0x5493, interactionIncState_hook, 0x23e0, 0x5496);
  CYC(0x5496, 0x5498); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(0x5498, 0x549a); mem_wr(gb, HL, 0x27); // >TX_2700
  CYC(0x549a, 0x549c); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x549c, 0x549e); mem_wr(gb, HL, 0x0c);
  CYC(0x549e, 0x549f); L = alu_inc8(gb, L);
  CYC(0x549f, 0x54a1); mem_wr(gb, HL, 0x06);
  CYC(0x54a1, 0x54a3); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(0x54a3, checkGlobalFlag_hook, 0x31f3, 0x54a6);
  CYC(0x54a6, 0x54a8); A = 0x15; // <TX_2715
  if (F & FZ) {
    CYCT(0x54a8, 0x54aa); goto setTextId;
  }
  CYC(0x54a8, 0x54aa);
  CYC(0x54aa, 0x54ac); A = 0x16; // <TX_2716

setTextId:
  CYC(0x54ac, 0x54ae); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x54ae, 0x54af); mem_wr(gb, DE, A);
  CALL_C(0x54af, objectSetVisiblec2_hook, 0x1e45, 0x54b2);
  CYC(0x54b2, 0x54b5); SET_HL(librarianScript_bank0c);
  CYC(0x54b5, 0x54b8); interactionSetScript_hook(gb);
}

// INTERAC_LIBRARIAN
void interactionCode2a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5485, checkInteractionState_hook, 0x23fe, 0x5488);
  if (F & FZ) {
    CYCT(0x5488, 0x548a); librarian_state0(gb, sp0_); return;
  }
  CYC(0x5488, 0x548a);
  librarian_state1(gb, sp0_);
}
