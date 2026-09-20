#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/a8.s (interactionCodea8 / INTERAC_a8), bank
// 0x0b.

static uint16_t interactionCodea8_jump_table(GB *gb) {
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

// interactionCodea8@handleSubidHighNibble (0b:5e7b): only reached via the single `call` at
// 0b:5e75 inside interactionCodea8@subid4. The caller pushes the real return address (0b:5e78)
// before invoking this as a plain C function call; every internal path here ends in a real `ret`
// (modeled with ret_effect, which pops that exact value), so C control simply falls back out to
// the caller's next statement at 0b:5e78 with the stack balanced -- no resume check needed.
static void interactionCodea8_handleSubidHighNibble(GB *gb, uint16_t sp0_) {
  CYC(0x5e7b, 0x5e7d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5e7d, 0x5e7e); A = mem_rd(gb, DE);
  CYC(0x5e7e, 0x5e80); alu_swap_a(gb);
  CYC(0x5e80, 0x5e82); alu_and(gb, 0x0f);
  CYC(0x5e82, 0x5e83); B = A;
  {
    CYC(0x5e83, 0x5e84); push_effect(gb, 0x5e84);
    uint16_t target = interactionCodea8_jump_table(gb);
    if (target == 0x5e9b) goto thing5;
    if (target == 0x5ebd) goto thing6;
    if (target == 0x5ec2) goto thing1;
    if (target == 0x5ec7) goto thing0;
    // target == 0x5e92 falls through to thing2/thing3/thing4 (aliased)
  }

  // interactionCodea8@thing2 / @thing3 / @thing4
  CYC(0x5e92, 0x5e93); A = B;

initLinkInCutscene: // interactionCodea8@initLinkInCutscene
  CYC(0x5e93, 0x5e96); SET_HL(w1Link_id);
  CYC(0x5e96, 0x5e98); mem_wr(gb, HL, 0x08); // SPECIALOBJECT_LINK_CUTSCENE
  CYC(0x5e98, 0x5e99); L = alu_inc8(gb, L);
  CYC(0x5e99, 0x5e9a); mem_wr(gb, HL, A); // [w1Link.subid]
  CYC(0x5e9a, 0x5e9b); ret_effect(gb); return;

thing5: // interactionCodea8@thing5
  CYC(0x5e9b, 0x5e9c); A = D;
  CYC(0x5e9c, 0x5e9f); W8(wLinkObjectIndex) = A;
  CYC(0x5e9f, 0x5ea2); SET_HL(wActiveRing);
  CYC(0x5ea2, 0x5ea4); mem_wr(gb, HL, 0x3d); // FIST_RING
  CYC(0x5ea4, 0x5ea5); alu_xor(gb, A);
  CYC(0x5ea5, 0x5ea7); L = 0x88; // <wInventoryB
  CYC(0x5ea7, 0x5ea8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5ea8, 0x5ea9); mem_wr(gb, HL, A);
  CYC(0x5ea9, 0x5eac); SET_HL(0x5eea); // @simulatedInput_5eea data table
  CYC(0x5eac, 0x5eae); A = 0x0b;

beginSimulatedInput: // interactionCodea8@beginSimulatedInput
  CYC(0x5eae, 0x5eaf); push_effect(gb, DE);
  CALL_C(0x5eaf, setSimulatedInputAddress_hook, 0x2a1d, 0x5eb2);
  CYC(0x5eb2, 0x5eb3); SET_DE(pop_effect(gb));
  CYC(0x5eb3, 0x5eb4); alu_xor(gb, A);
  CYC(0x5eb4, 0x5eb7); W8(wDisabledObjects) = A;
  CYC(0x5eb7, 0x5eba); SET_HL(w1Link_id);
  CYC(0x5eba, 0x5ebc); mem_wr(gb, HL, 0x00); // SPECIALOBJECT_LINK
  CYC(0x5ebc, 0x5ebd); ret_effect(gb); return;

thing6: // interactionCodea8@thing6
  CYC(0x5ebd, 0x5ebf); A = 0x09;
  CYC(0x5ebf, 0x5ec2); goto initLinkInCutscene; // jp

thing1: // interactionCodea8@thing1
  CYC(0x5ec2, 0x5ec4); A = 0x0a;
  CYC(0x5ec4, 0x5ec7); goto initLinkInCutscene; // jp

thing0: // interactionCodea8@thing0
  CYC(0x5ec7, 0x5eca); SET_HL(w1Link_direction);
  CYC(0x5eca, 0x5ecc); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(0x5ecc, 0x5ecd); A = H;
  CYC(0x5ecd, 0x5ed0); W8(wLinkObjectIndex) = A;
  CYC(0x5ed0, 0x5ed3); SET_HL(wInventoryB);
  CYC(0x5ed3, 0x5ed5); mem_wr(gb, HL, 0x05); // ITEM_SWORD
  CYC(0x5ed5, 0x5ed6); L = alu_inc8(gb, L);
  CYC(0x5ed6, 0x5ed8); mem_wr(gb, HL, 0x00); // [wInventoryA]
  CYC(0x5ed8, 0x5edb); SET_HL(0x5f2b); // @linkSwordDemonstrationInput data table
  CYC(0x5edb, 0x5edd); A = 0x0b;
  CYC(0x5edd, 0x5edf); goto beginSimulatedInput; // jr

  // interactionCodea8@unusedInputData (0b:5edf): Seasons-only pure data, dead in Ages -- not
  // ported as code.
  // interactionCodea8@simulatedInput_5eea (0b:5eea) and @linkSwordDemonstrationInput (0b:5f2b):
  // pure ROM data tables, not ported as code -- referenced only via SET_HL(0x5eea)/SET_HL(0x5f2b)
  // above.
}

// ==================================================================================================
// INTERAC_a8
// ==================================================================================================
void interactionCodea8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e43, 0x5e45); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5e45, 0x5e46); A = mem_rd(gb, DE);
  CYC(0x5e46, 0x5e48); alu_and(gb, 0x0f);
  {
    CYC(0x5e48, 0x5e49); push_effect(gb, 0x5e49);
    uint16_t target = interactionCodea8_jump_table(gb);
    if (target == 0x5e6d) goto subid4;
    // target == 0x5e53 falls through to subid0/subid1/subid2/subid3 (aliased)
  }

  // interactionCodea8@subid0 / @subid1 / @subid2 / @subid3
  CYC(0x5e53, 0x5e54); A = mem_rd(gb, DE);
  CYC(0x5e54, 0x5e56); alu_and(gb, 0x0f);
  CYC(0x5e56, 0x5e58); alu_add(gb, 0x0f); // SPECIALOBJECT_RICKY_CUTSCENE
  CYC(0x5e58, 0x5e59); B = A;
  CYC(0x5e59, 0x5e5a); A = mem_rd(gb, DE);
  CYC(0x5e5a, 0x5e5c); alu_swap_a(gb);
  CYC(0x5e5c, 0x5e5e); alu_and(gb, 0x0f);
  CYC(0x5e5e, 0x5e61); SET_HL(w1Companion_enabled);
  CYC(0x5e61, 0x5e63); mem_wr(gb, HL, 0x01);
  CYC(0x5e63, 0x5e64); L = alu_inc8(gb, L);
  CYC(0x5e64, 0x5e65); mem_wr(gb, HL, B); // [w1Companion.id]
  CYC(0x5e65, 0x5e66); L = alu_inc8(gb, L);
  CYC(0x5e66, 0x5e67); mem_wr(gb, HL, A); // [w1Companion.subid]
  CALL_C(0x5e67, objectCopyPosition_hook, 0x2242, 0x5e6a);
  CYC(0x5e6a, 0x5e6d); interactionDelete_hook(gb); return; // jp

subid4: // interactionCodea8@subid4
  CYC(0x5e6d, 0x5e70); SET_HL(w1Link_enabled);
  CYC(0x5e70, 0x5e72); mem_wr(gb, HL, 0x03);
  CALL_C(0x5e72, objectCopyPosition_hook, 0x2242, 0x5e75);
  CYC(0x5e75, 0x5e78); push_effect(gb, 0x5e78); interactionCodea8_handleSubidHighNibble(gb, sp0_); // call
  CYC(0x5e78, 0x5e7b); interactionDelete_hook(gb); return; // jp
}
