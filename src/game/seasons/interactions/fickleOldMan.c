#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/fickleOldMan.s.
// INTERAC_FICKLE_OLD_MAN

static void fickleOldMan_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_FICKLE_OLD_MAN
void s_interactionCode80_hook(GB *gb) {
  BASE(interactionCode80);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+13); B = 0x07;
  CALL_C(b_+13, s_checkIfHoronVillageNPCShouldBeSeen_hook, SYM(checkIfHoronVillageNPCShouldBeSeen), b_+16);
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = B;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); SET_HL(b_+44 /* @table_7717 */);
  CYC(b_+28, b_+29); fickleOldMan_add_double_index(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); H = mem_rd(gb, HL);
  CYC(b_+31, b_+32); L = A;
  CALL_C(b_+32, s_interactionSetScript, SYM(interactionSetScript), b_+35);
  CYC(b_+35, b_+38);
  TAIL(objectSetVisible82);
state1:
  CALL_C(b_+38, s_interactionRunScript, SYM(interactionRunScript), b_+41);
  CYC(b_+41, b_+44);
  TAIL(interactionAnimateAsNpc);
}

