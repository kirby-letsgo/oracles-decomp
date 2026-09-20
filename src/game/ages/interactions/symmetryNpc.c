#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodebf), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodebf), (from), (to), true)

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
  BASE(interactionCodebf);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodebf_jump_table(gb);
    if (target == b_+119) goto runScriptAndAnimate;
    if (target == b_+101) goto state2;
    // target == 0x714d falls through to state0
  }

  // interactionCodebf@state0
  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13); // SWITCHES THREADS
  CALL_C(b_+13, objectSetVisible82_hook, SYM(objectSetVisible82), b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+21); A = 0x2d; // >TX_2d00
  CALL_C(b_+21, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+24);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  {
    CYC(b_+27, b_+28); push_effect(gb, b_+28);
    uint16_t target = interactionCodebf_jump_table(gb);
    if (target == b_+62) goto loadScript; // subids 0x0-0xb, all aliased
    // target == 0x7179 falls through to subid0cInit
  }

  // interactionCodebf@subid0cInit
  CYC(b_+54, b_+56); A = 0x29; // GLOBALFLAG_TUNI_NUT_PLACED
  CALL_C(b_+56, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+59);
  if (F & FZ) { CYCT(b_+59, b_+62); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+59, b_+62);

loadScript: // interactionCodebf@loadScript
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+68); SET_HL(b_+75); // @scriptTable
  CYC(b_+68, b_+69); interactionCodebf_add_double_index(gb, b_+69);
  CYC(b_+69, b_+70); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+70, b_+71); H = mem_rd(gb, HL);
  CYC(b_+71, b_+72); L = A;
  CYC(b_+72, b_+75); interactionSetScript_hook(gb); return; // jp

state2: // interactionCodebf@state2
  CYC(b_+101, b_+104); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+104, b_+106); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+106, b_+108); goto runScriptAndAnimate; } // jr z
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+111); SET_HL((SYM(interactionCoded8__subid1Script) + 6)); // mainScripts.symmetryNpcSubid8And9Script_afterTuniNutRestored
  CALL_C(b_+111, interactionSetScript_hook, SYM(interactionSetScript), b_+114);
  CYC(b_+114, b_+116); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+116, b_+118); A = 0x01;
  CYC(b_+118, b_+119); mem_wr(gb, DE, A);

runScriptAndAnimate: // interactionCodebf@runScriptAndAnimate
  CALL_C(b_+119, interactionRunScript_hook, SYM(interactionRunScript), b_+122);
  CYC(b_+122, SYM(interactionCodec1)); npcFaceLinkAndAnimate_hook(gb); return; // jp
}
