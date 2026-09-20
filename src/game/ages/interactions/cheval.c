#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode6a_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void interactionCode6a_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_CHEVAL
//
// state0@subid and state1@subid each dispatch through a single-entry jump table (there is only
// one subid used by this NPC), so those two RST calls are modeled but never need a target check --
// they always continue at their one destination. Every transfer here is a goto, never a call.
void interactionCode6a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4dbd, 0x4dbf); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4dbf, 0x4dc0); A = mem_rd(gb, DE);
  {
    CYC(0x4dc0, 0x4dc1); push_effect(gb, 0x4dc1);
    uint16_t target = interactionCode6a_jump_table(gb);
    if (target == 0x4dc5) goto state0;
    goto state1;
  }

state0:
  CYC(0x4dc5, 0x4dc7); A = 0x01;
  CYC(0x4dc7, 0x4dc8); mem_wr(gb, DE, A);
  CALL_C(0x4dc8, interactionInitGraphics_hook, 0x15fb, 0x4dcb);
  CALL_C(0x4dcb, objectSetVisiblec2_hook, 0x1e45, 0x4dce);
  CYC(0x4dce, 0x4dd0); A = 0x27; // >TX_2700
  CALL_C(0x4dd0, interactionSetHighTextIndex_hook, 0x253b, 0x4dd3);
  CYC(0x4dd3, 0x4dd5); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4dd5, 0x4dd6); A = mem_rd(gb, DE);
  {
    CYC(0x4dd6, 0x4dd7); push_effect(gb, 0x4dd7);
    interactionCode6a_jump_table(gb);
    goto loadScript;
  }

state1:
  CYC(0x4dd9, 0x4ddb); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4ddb, 0x4ddc); A = mem_rd(gb, DE);
  {
    CYC(0x4ddc, 0x4ddd); push_effect(gb, 0x4ddd);
    interactionCode6a_jump_table(gb);
    goto runSubid00;
  }

runSubid00:
  CALL_C(0x4ddf, interactionRunScript_hook, 0x2552, 0x4de2);
  CYC(0x4de2, 0x4de5); interactionAnimateAsNpc_hook(gb); return; // jp

loadScript:
  CYC(0x4de5, 0x4de7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4de7, 0x4de8); A = mem_rd(gb, DE);
  CYC(0x4de8, 0x4deb); SET_HL(0x4df2); // interactionCode6a@scriptTable
  CYC(0x4deb, 0x4dec); interactionCode6a_addDoubleIndexToHl_from_rst(gb, 0x4dec);
  CYC(0x4dec, 0x4ded); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4ded, 0x4dee); H = mem_rd(gb, HL);
  CYC(0x4dee, 0x4def); L = A;
  CYC(0x4def, 0x4df2); interactionSetScript_hook(gb); return; // jp
}
