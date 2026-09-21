#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// interactionCode2e@scriptTable: subid 0 = mainScripts.oldManScript_givesRupees,
// subid 1 = mainScripts.oldManScript_takesRupees.
#define oldManWithRupeesScriptTable_bank08 SYM(interactionCode2e__scriptTable)

static void oldManWithRupees_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_OLD_MAN_WITH_RUPEES: the old man who gives or takes rupees.
void interactionCode2e_hook(GB *gb) {
  BASE(interactionCode2e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto state1;
  }
  CYC(b_+3, b_+5);

  // @state0
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CALL_C(b_+7, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+10);
  CYC(b_+10, b_+12); A = 0x33; // >TX_3300
  CALL_C(b_+12, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+15);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+21); SET_HL(oldManWithRupeesScriptTable_bank08);
  CYC(b_+21, b_+22); oldManWithRupees_addDoubleIndex(gb, b_+22);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+24); H = mem_rd(gb, HL);
  CYC(b_+24, b_+25); L = A;
  CALL_C(b_+25, interactionSetScript_hook, SYM(interactionSetScript), b_+28);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x38);
  CYC(b_+33, b_+35); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x28);

state1:
  CALL_C(b_+37, interactionRunScript_hook, SYM(interactionRunScript), b_+40);
  CYC(b_+40, b_+43); TAIL(npcFaceLinkAndAnimate);
}
