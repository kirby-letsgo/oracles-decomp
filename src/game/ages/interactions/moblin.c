#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x7d0a, 0x7d0c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d0c, 0x7d0d); A = mem_rd(gb, DE);
  {
    CYC(0x7d0d, 0x7d0e); push_effect(gb, 0x7d0e);
    (void)moblin_jump_table(gb);
    // subid 0 and subid 1 both target 0x7d12
  }

  // interactionCode96@subid0 / @subid1
  CALL_C(0x7d12, checkInteractionState_hook, 0x23fe, 0x7d15);
  if (!(F & FZ)) { CYCT(0x7d15, 0x7d17); goto state1; } // jr nz
  CYC(0x7d15, 0x7d17);

  // interactionCode96@state0
  CYC(0x7d17, 0x7d1a); push_effect(gb, 0x7d1a); goto initGraphicsAndLoadScript;

state1:
  CALL_C(0x7d1a, interactionRunScript_hook, 0x2552, 0x7d1d);
  if (F & FC) { CYCT(0x7d1d, 0x7d20); interactionDelete_hook(gb); return; } // jp c
  CYC(0x7d1d, 0x7d20);
  CYC(0x7d20, 0x7d22); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7d22, 0x7d23); A = mem_rd(gb, DE);
  CYC(0x7d23, 0x7d24); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7d24, 0x7d26); goto skipAnimate; } // jr nz
  CYC(0x7d24, 0x7d26);
  CALL_C(0x7d26, interactionAnimate_hook, 0x261b, 0x7d29);

skipAnimate:
  CYC(0x7d29, 0x7d2c); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; // jp

  // interactionCode96@initGraphicsAndLoadScript; single caller (@state0, above), same
  // sp0_ depth, entered via a genuine `call`. Its final tail-call resumes @state1.
initGraphicsAndLoadScript:
  CALL_C(0x7d32, interactionInitGraphics_hook, 0x15fb, 0x7d35);
  CYC(0x7d35, 0x7d37); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7d37, 0x7d38); A = mem_rd(gb, DE);
  CYC(0x7d38, 0x7d3b); SET_HL(0x7d45); // @scriptTable
  CYC(0x7d3b, 0x7d3c); moblin_addDoubleIndexToHl_from_rst(gb, 0x7d3c);
  CYC(0x7d3c, 0x7d3d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7d3d, 0x7d3e); H = mem_rd(gb, HL);
  CYC(0x7d3e, 0x7d3f); L = A;
  CALL_C(0x7d3f, interactionSetScript_hook, 0x2544, 0x7d42);
  CYC(0x7d42, 0x7d45); interactionIncState_hook(gb);
  if (gb->pc == 0x7d1a && gb->sp == sp0_) goto state1;
  return; // jp
}
