#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// interactionCode2e@scriptTable: subid 0 = mainScripts.oldManScript_givesRupees,
// subid 1 = mainScripts.oldManScript_takesRupees.
#define oldManWithRupeesScriptTable_bank08 0x55fa

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x55cf, checkInteractionState_hook, 0x23fe, 0x55d2);
  if (!(F & FZ)) {
    CYCT(0x55d2, 0x55d4); goto state1;
  }
  CYC(0x55d2, 0x55d4);

  // @state0
  CYC(0x55d4, 0x55d5); A = alu_inc8(gb, A);
  CYC(0x55d5, 0x55d6); mem_wr(gb, DE, A);
  CALL_C(0x55d6, interactionInitGraphics_hook, 0x15fb, 0x55d9);
  CYC(0x55d9, 0x55db); A = 0x33; // >TX_3300
  CALL_C(0x55db, interactionSetHighTextIndex_hook, 0x253b, 0x55de);
  CYC(0x55de, 0x55e0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x55e0, 0x55e1); A = mem_rd(gb, DE);
  CYC(0x55e1, 0x55e4); SET_HL(oldManWithRupeesScriptTable_bank08);
  CYC(0x55e4, 0x55e5); oldManWithRupees_addDoubleIndex(gb, 0x55e5);
  CYC(0x55e5, 0x55e6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x55e6, 0x55e7); H = mem_rd(gb, HL);
  CYC(0x55e7, 0x55e8); L = A;
  CALL_C(0x55e8, interactionSetScript_hook, 0x2544, 0x55eb);
  CYC(0x55eb, 0x55ec); H = D;
  CYC(0x55ec, 0x55ee); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x55ee, 0x55f0); mem_wr(gb, HL, 0x38);
  CYC(0x55f0, 0x55f2); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x55f2, 0x55f4); mem_wr(gb, HL, 0x28);

state1:
  CALL_C(0x55f4, interactionRunScript_hook, 0x2552, 0x55f7);
  CYC(0x55f7, 0x55fa); npcFaceLinkAndAnimate_hook(gb);
}
