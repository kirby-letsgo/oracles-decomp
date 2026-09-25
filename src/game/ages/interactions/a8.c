#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodea8);
  CYC(b_+56, b_+58); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+61); alu_swap_a(gb);
  CYC(b_+61, b_+63); alu_and(gb, 0x0f);
  CYC(b_+63, b_+64); B = A;
  {
    CYC(b_+64, b_+65); push_effect(gb, b_+65);
    uint16_t target = interactionCodea8_jump_table(gb);
    if (game_seasons) {
      if (target == b_+S(77)) goto s_thing0;
      if (target == b_+S(108)) goto s_thing5;
      goto s_thing1to4;
    }
    if (target == b_+88) goto thing5;
    if (target == b_+122) goto thing6;
    if (target == b_+127) goto thing1;
    if (target == b_+132) goto thing0;
    // target == 0x5e92 falls through to thing2/thing3/thing4 (aliased)
  }

  if (!game_seasons) {
    // interactionCodea8@thing2 / @thing3 / @thing4
    CYC(b_+79, b_+80); A = B;

initLinkInCutscene: // interactionCodea8@initLinkInCutscene
    CYC(b_+80, b_+83); SET_HL(w1Link_id);
    CYC(b_+83, b_+85); mem_wr(gb, HL, 0x08); // SPECIALOBJECT_LINK_CUTSCENE
    CYC(b_+85, b_+86); L = alu_inc8(gb, L);
    CYC(b_+86, b_+87); mem_wr(gb, HL, A); // [w1Link.subid]
    CYC(b_+87, b_+88); ret_effect(gb); return;

thing5: // interactionCodea8@thing5
    CYC(b_+88, b_+89); A = D;
    CYC(b_+89, b_+92); W8(wLinkObjectIndex) = A;
    CYC(b_+92, b_+95); SET_HL(wActiveRing);
    CYC(b_+95, b_+97); mem_wr(gb, HL, 0x3d); // FIST_RING
    CYC(b_+97, b_+98); alu_xor(gb, A);
    CYC(b_+98, b_+100); L = 0x88; // <wInventoryB
    CYC(b_+100, b_+101); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+101, b_+102); mem_wr(gb, HL, A);
    CYC(b_+102, b_+105); SET_HL(b_+167); // @simulatedInput_5eea data table
    CYC(b_+105, b_+107); A = 0x0b;

beginSimulatedInput: // interactionCodea8@beginSimulatedInput
    CYC(b_+107, b_+108); push_effect(gb, DE);
    CALL_C(b_+108, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+111);
    CYC(b_+111, b_+112); SET_DE(pop_effect(gb));
    CYC(b_+112, b_+113); alu_xor(gb, A);
    CYC(b_+113, b_+116); W8(wDisabledObjects) = A;
    CYC(b_+116, b_+119); SET_HL(w1Link_id);
    CYC(b_+119, b_+121); mem_wr(gb, HL, 0x00); // SPECIALOBJECT_LINK
    CYC(b_+121, b_+122); ret_effect(gb); return;

thing6: // interactionCodea8@thing6
    CYC(b_+122, b_+124); A = 0x09;
    CYC(b_+124, b_+127); goto initLinkInCutscene; // jp

thing1: // interactionCodea8@thing1
    CYC(b_+127, b_+129); A = 0x0a;
    CYC(b_+129, b_+132); goto initLinkInCutscene; // jp

thing0: // interactionCodea8@thing0
    CYC(b_+132, b_+135); SET_HL(w1Link_direction);
    CYC(b_+135, b_+137); mem_wr(gb, HL, 0x02); // DIR_DOWN
    CYC(b_+137, b_+138); A = H;
    CYC(b_+138, b_+141); W8(wLinkObjectIndex) = A;
    CYC(b_+141, b_+144); SET_HL(wInventoryB);
    CYC(b_+144, b_+146); mem_wr(gb, HL, 0x05); // ITEM_SWORD
    CYC(b_+146, b_+147); L = alu_inc8(gb, L);
    CYC(b_+147, b_+149); mem_wr(gb, HL, 0x00); // [wInventoryA]
    CYC(b_+149, b_+152); SET_HL(b_+232); // @linkSwordDemonstrationInput data table
    CYC(b_+152, b_+154); A = 0x0b;
    CYC(b_+154, b_+156); goto beginSimulatedInput; // jr
  }

  // Seasons: thing0 plays simulatedInput_6869, thing1-4 load Link's cutscene object with
  // subid (index + 2), thing5 plays simulatedInput_6874 with the fist ring.
s_thing0:
  CYC(b_+S(77), b_+S(80)); SET_HL(b_+S(130)); // simulatedInput_6869
  CYC(b_+S(80), b_+S(82)); A = 0x0a;
s_beginSimulatedInput:
  CYC(b_+S(82), b_+S(83)); push_effect(gb, DE);
  CALL_C(b_+S(83), setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+S(86));
  CYC(b_+S(86), b_+S(87)); SET_DE(pop_effect(gb));
  CYC(b_+S(87), b_+S(88)); alu_xor(gb, A);
  CYC(b_+S(88), b_+S(91)); W8(wDisabledObjects) = A;
  CYC(b_+S(91), b_+S(94)); SET_HL(w1Link_id);
  CYC(b_+S(94), b_+S(96)); mem_wr(gb, HL, 0x00); // SPECIALOBJECT_LINK
  CYC(b_+S(96), b_+S(97)); ret_effect(gb); return;
s_thing1to4:
  CYC(b_+S(97), b_+S(98)); A = B;
  CYC(b_+S(98), b_+S(100)); alu_add(gb, 0x02);
  CYC(b_+S(100), b_+S(103)); SET_HL(w1Link_id);
  CYC(b_+S(103), b_+S(105)); mem_wr(gb, HL, 0x08); // SPECIALOBJECT_LINK_CUTSCENE
  CYC(b_+S(105), b_+S(106)); L = alu_inc8(gb, L);
  CYC(b_+S(106), b_+S(107)); mem_wr(gb, HL, A);
  CYC(b_+S(107), b_+S(108)); ret_effect(gb); return;
s_thing5:
  CYC(b_+S(108), b_+S(109)); A = D;
  CYC(b_+S(109), b_+S(112)); W8(wLinkObjectIndex) = A;
  CYC(b_+S(112), b_+S(115)); SET_HL(wActiveRing);
  CYC(b_+S(115), b_+S(117)); mem_wr(gb, HL, 0x3d); // FIST_RING
  CYC(b_+S(117), b_+S(118)); alu_xor(gb, A);
  CYC(b_+S(118), b_+S(120)); L = 0x80; // <wInventoryB
  CYC(b_+S(120), b_+S(121)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+S(121), b_+S(122)); mem_wr(gb, HL, A);
  CYC(b_+S(122), b_+S(125)); SET_HL(b_+S(141)); // simulatedInput_6874
  CYC(b_+S(125), b_+S(127)); A = 0x0a;
  CYC(b_+S(127), b_+S(130)); goto s_beginSimulatedInput; // jp

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
  BASE(interactionCodea8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = interactionCodea8_jump_table(gb);
    if (target == b_+42) goto subid4;
    // target == 0x5e53 falls through to subid0/subid1/subid2/subid3 (aliased)
  }

  // interactionCodea8@subid0 / @subid1 / @subid2 / @subid3
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+21); alu_add(gb, 0x0f); // SPECIALOBJECT_RICKY_CUTSCENE
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_swap_a(gb);
  CYC(b_+25, b_+27); alu_and(gb, 0x0f);
  CYC(b_+27, b_+30); SET_HL(w1Companion_enabled);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+34); mem_wr(gb, HL, B); // [w1Companion.id]
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); // [w1Companion.subid]
  CALL_C(b_+36, objectCopyPosition_hook, SYM(objectCopyPosition), b_+39);
  CYC(b_+39, b_+42); TAIL(interactionDelete); // jp

subid4: // interactionCodea8@subid4
  CYC(b_+42, b_+45); SET_HL(w1Link_enabled);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x03);
  CALL_C(b_+47, objectCopyPosition_hook, SYM(objectCopyPosition), b_+50);
  CYC(b_+50, b_+53); push_effect(gb, b_+53); interactionCodea8_handleSubidHighNibble(gb, sp0_); // call
  CYC(b_+53, b_+56); TAIL(interactionDelete); // jp
}
