#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/symmetryNpc.s (interactionCodebf /
// INTERAC_SYMMETRY_NPC), bank 0x0b.

static uint16_t interactionCodebf_jump_table(GB *gb) {
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

static void interactionCodebf_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_SYMMETRY_NPC
// ==================================================================================================
void interactionCodebf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7143, 0x7145); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7145, 0x7146); A = mem_rd(gb, DE);
  {
    CYC(0x7146, 0x7147); push_effect(gb, 0x7147);
    uint16_t target = interactionCodebf_jump_table(gb);
    if (target == 0x71ba) goto runScriptAndAnimate;
    if (target == 0x71a8) goto state2;
    // target == 0x714d falls through to state0
  }

  // interactionCodebf@state0
  CALL_C(0x714d, interactionInitGraphics_hook, 0x15fb, 0x7150); // SWITCHES THREADS
  CALL_C(0x7150, objectSetVisible82_hook, 0x1e69, 0x7153);
  CALL_C(0x7153, interactionIncState_hook, 0x23e0, 0x7156);
  CYC(0x7156, 0x7158); A = 0x2d; // >TX_2d00
  CALL_C(0x7158, interactionSetHighTextIndex_hook, 0x253b, 0x715b);
  CYC(0x715b, 0x715d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x715d, 0x715e); A = mem_rd(gb, DE);
  {
    CYC(0x715e, 0x715f); push_effect(gb, 0x715f);
    uint16_t target = interactionCodebf_jump_table(gb);
    if (target == 0x7181) goto loadScript; // subids 0x0-0xb, all aliased
    // target == 0x7179 falls through to subid0cInit
  }

  // interactionCodebf@subid0cInit
  CYC(0x7179, 0x717b); A = 0x29; // GLOBALFLAG_TUNI_NUT_PLACED
  CALL_C(0x717b, checkGlobalFlag_hook, 0x31f3, 0x717e);
  if (F & FZ) { CYCT(0x717e, 0x7181); interactionDelete_hook(gb); return; } // jp z
  CYC(0x717e, 0x7181);

loadScript: // interactionCodebf@loadScript
  CYC(0x7181, 0x7183); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7183, 0x7184); A = mem_rd(gb, DE);
  CYC(0x7184, 0x7187); SET_HL(0x718e); // @scriptTable
  CYC(0x7187, 0x7188); interactionCodebf_add_double_index(gb, 0x7188);
  CYC(0x7188, 0x7189); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7189, 0x718a); H = mem_rd(gb, HL);
  CYC(0x718a, 0x718b); L = A;
  CYC(0x718b, 0x718e); interactionSetScript_hook(gb); return; // jp

state2: // interactionCodebf@state2
  CYC(0x71a8, 0x71ab); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x71ab, 0x71ad); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x71ad, 0x71af); goto runScriptAndAnimate; } // jr z
  CYC(0x71ad, 0x71af);
  CYC(0x71af, 0x71b2); SET_HL(0x7daf); // mainScripts.symmetryNpcSubid8And9Script_afterTuniNutRestored
  CALL_C(0x71b2, interactionSetScript_hook, 0x2544, 0x71b5);
  CYC(0x71b5, 0x71b7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x71b7, 0x71b9); A = 0x01;
  CYC(0x71b9, 0x71ba); mem_wr(gb, DE, A);

runScriptAndAnimate: // interactionCodebf@runScriptAndAnimate
  CALL_C(0x71ba, interactionRunScript_hook, 0x2552, 0x71bd);
  CYC(0x71bd, 0x71c0); npcFaceLinkAndAnimate_hook(gb); return; // jp
}
