#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode6a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode6a_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+19); A = 0x27; // >TX_2700
  CALL_C(b_+19, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  {
    CYC(b_+25, b_+26); push_effect(gb, b_+26);
    interactionCode6a_jump_table(gb);
    goto loadScript;
  }

state1:
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  {
    CYC(b_+31, b_+32); push_effect(gb, b_+32);
    interactionCode6a_jump_table(gb);
    goto runSubid00;
  }

runSubid00:
  CALL_C(b_+34, interactionRunScript_hook, SYM(interactionRunScript), b_+37);
  CYC(b_+37, b_+40); interactionAnimateAsNpc_hook(gb); return; // jp

loadScript:
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+46); SET_HL(b_+53); // interactionCode6a@scriptTable
  CYC(b_+46, b_+47); interactionCode6a_addDoubleIndexToHl_from_rst(gb, b_+47);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+48, b_+49); H = mem_rd(gb, HL);
  CYC(b_+49, b_+50); L = A;
  CYC(b_+50, b_+53); interactionSetScript_hook(gb); return; // jp
}
