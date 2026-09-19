#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodedb_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// interactionCodedb@resetCounter (0b:7f7e): always ends in a real "ret", so it must pop via
// ret_effect() -- reached both via genuine `call nc,` (through CALL_C_CC, which pushes a matching
// return address first) from @state1's two sites, and via plain fallthrough / `jr nc,` tail-jumps
// from @state1's "not enough slates" path and from @placeSlate (no push there, so ret_effect()
// pops whatever the real stack already holds -- the original caller of the whole hook).
static void interactionCodedb_resetCounter(GB *gb) {
  CYC(0x7f7e, 0x7f80); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7f80, 0x7f82); A = 0x0a;
  CYC(0x7f82, 0x7f83); mem_wr(gb, DE, A);
  RET(0x7f83); return;
}

// ==================================================================================================
// INTERAC_SLATE_SLOT
//
// Variables:
//   var3f: Counter to push against this object until the slate will be placed
// ==================================================================================================
void interactionCodedb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7f3d, 0x7f3f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7f3f, 0x7f40); A = mem_rd(gb, DE);
  CYC(0x7f40, 0x7f41); push_effect(gb, 0x7f41);
  switch (interactionCodedb_jump_table(gb)) {
    case 0x7f47: goto state0;
    case 0x7f60: goto state1;
    case 0x7f9a: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  // Check if slate already placed
  CYC(0x7f47, 0x7f49); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7f49, 0x7f4a); A = mem_rd(gb, DE);
  CYC(0x7f4a, 0x7f4d); SET_BC(0x00f8); // bitTable
  CYC(0x7f4d, 0x7f4e); alu_add(gb, C);
  CYC(0x7f4e, 0x7f4f); C = A;
  CALL_C(0x7f4f, getThisRoomFlags_hook, 0x197d, 0x7f52);
  CYC(0x7f52, 0x7f53); A = mem_rd(gb, BC);
  CYC(0x7f53, 0x7f54); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7f54, 0x7f57); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7f54, 0x7f57);
  CYC(0x7f57, 0x7f5a); SET_HL(0x7f5a); // mainScripts.slateSlotScript
  CALL_C(0x7f5a, interactionSetScript_hook, 0x2544, 0x7f5d);
  CYC(0x7f5d, 0x7f60); interactionIncState_hook(gb); return; // jp

state1:
  CALL_C(0x7f60, objectCheckCollidedWithLink_notDead_hook, 0x1c2e, 0x7f63);
  if (!(F & FC)) CALL_C_CC(0x7f63, interactionCodedb_resetCounter, 0x7f7e, 0x7f66); else CYC(0x7f63, 0x7f66); // call nc
  CALL_C(0x7f66, objectCheckLinkPushingAgainstCenter_hook, 0x2707, 0x7f69);
  if (!(F & FC)) CALL_C_CC(0x7f69, interactionCodedb_resetCounter, 0x7f7e, 0x7f6c); else CYC(0x7f69, 0x7f6c); // call nc
  CYC(0x7f6c, 0x7f6d); H = D;
  CYC(0x7f6d, 0x7f6f); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7f6f, 0x7f70); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x7f70, 0x7f72); goto state2; } // jr nz
  CYC(0x7f70, 0x7f72);

  // Time to place the slate, if available
  CYC(0x7f72, 0x7f75); A = mem_rd(gb, wNumSlates);
  CYC(0x7f75, 0x7f76); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7f76, 0x7f78); goto placeSlate; } // jr nz
  CYC(0x7f76, 0x7f78);

  // Not enough slates
  CYC(0x7f78, 0x7f7b); SET_BC(0x5111); // TX_5111
  CALL_C(0x7f7b, showText_hook, 0x1872, 0x7f7e);
  interactionCodedb_resetCounter(gb); return; // falls through into @resetCounter

placeSlate:
  CALL_C(0x7f84, checkLinkVulnerable_hook, 0x1d28, 0x7f87);
  if (!(F & FC)) { CYCT(0x7f87, 0x7f89); interactionCodedb_resetCounter(gb); return; } // jr nc
  CYC(0x7f87, 0x7f89);
  CYC(0x7f89, 0x7f8b); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x7f8b, 0x7f8e); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7f8e, 0x7f91); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7f91, 0x7f94); SET_HL(0x7f5d); // mainScripts.slateSlotScript_placeSlate
  CALL_C(0x7f94, interactionSetScript_hook, 0x2544, 0x7f97);
  CALL_C(0x7f97, interactionIncState_hook, 0x23e0, 0x7f9a);
  // falls through into @state2

state2:
  CALL_C(0x7f9a, interactionRunScript_hook, 0x2552, 0x7f9d);
  if (!(F & FC)) { CYCT(0x7f9d, 0x7f9e); ret_effect(gb); return; } // ret nc
  CYC(0x7f9d, 0x7f9e);
  CYC(0x7f9e, 0x7fa1); interactionDelete_hook(gb); return; // jp
}
