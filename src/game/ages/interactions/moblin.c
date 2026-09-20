#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode96), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode96), (from), (to), true)

static uint16_t moblin_jump_table(GB *gb) {
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

static void moblin_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_MOBLIN
void interactionCode96_hook(GB *gb) {
  BASE(interactionCode96);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    (void)moblin_jump_table(gb);
    // subid 0 and subid 1 both target 0x7d12
  }

  // interactionCode96@subid0 / @subid1
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto state1; } // jr nz
  CYC(b_+11, b_+13);

  // interactionCode96@state0
  CYC(b_+13, b_+16); push_effect(gb, b_+16); goto initGraphicsAndLoadScript;

state1:
  CALL_C(b_+16, interactionRunScript_hook, SYM(interactionRunScript), b_+19);
  if (F & FC) { CYCT(b_+19, b_+22); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto skipAnimate; } // jr nz
  CYC(b_+26, b_+28);
  CALL_C(b_+28, interactionAnimate_hook, SYM(interactionAnimate), b_+31);

skipAnimate:
  CYC(b_+31, b_+34); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; // jp

  // interactionCode96@initGraphicsAndLoadScript; single caller (@state0, above), same
  // sp0_ depth, entered via a genuine `call`. Its final tail-call resumes @state1.
initGraphicsAndLoadScript:
  CALL_C(b_+40, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+43);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+49); SET_HL(b_+59); // @scriptTable
  CYC(b_+49, b_+50); moblin_addDoubleIndexToHl_from_rst(gb, b_+50);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+51, b_+52); H = mem_rd(gb, HL);
  CYC(b_+52, b_+53); L = A;
  CALL_C(b_+53, interactionSetScript_hook, SYM(interactionSetScript), b_+56);
  CYC(b_+56, b_+59); interactionIncState_hook(gb);
  if (gb->pc == b_+16 && gb->sp == sp0_) goto state1;
  return; // jp
}
