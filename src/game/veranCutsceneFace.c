#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// interactionCode2d@warpDestVariables: m_HardcodedWarpA ROOM_AGES_4d4, $0c, $67, $03
#define veranCutsceneFaceWarpDestVariables_bank08 0x55ca
// mainScripts.veranFaceCutsceneScript (bank $0c script data, referenced by address only).
#define veranFaceCutsceneScript_bank0c 0x4f1c

static uint16_t veranCutsceneFace_jumpTable(GB *gb) {
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

// INTERAC_VERAN_CUTSCENE_FACE: Veran's face during the possession cutscene; runs a script
// and warps out when it finishes.
void interactionCode2d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5597, 0x5599); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5599, 0x559a); A = mem_rd(gb, DE);
  CYC(0x559a, 0x559b); push_effect(gb, 0x559b);
  switch (veranCutsceneFace_jumpTable(gb)) {
    case 0x55a1: goto state0;
    case 0x55b6: goto state1;
    case 0x55b5: goto state2;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x55a1, 0x55a3); A = 0x01;
  CYC(0x55a3, 0x55a4); mem_wr(gb, DE, A);
  CALL_C(0x55a4, interactionInitGraphics_hook, 0x15fb, 0x55a7);
  CALL_C(0x55a7, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x55aa);
  CYC(0x55aa, 0x55ac); A = 0x87; // PALH_87
  CALL_C(0x55ac, loadPaletteHeader_hook, 0x050b, 0x55af);
  CYC(0x55af, 0x55b2); SET_HL(veranFaceCutsceneScript_bank0c);
  CALL_C(0x55b2, interactionSetScript_hook, 0x2544, 0x55b5);
  // falls through into state 2 (just a ret)

state2:
  CYC(0x55b5, 0x55b6); ret_effect(gb);
  return;

state1:
  CALL_C(0x55b6, interactionAnimate_hook, 0x261b, 0x55b9);
  CALL_C(0x55b9, interactionRunScript_hook, 0x2552, 0x55bc);
  if (!(F & FC)) {
    CYCT(0x55bc, 0x55bd); ret_effect(gb); return;
  }
  CYC(0x55bc, 0x55bd);
  CYC(0x55bd, 0x55c0); SET_HL(veranCutsceneFaceWarpDestVariables_bank08);
  CALL_C(0x55c0, setWarpDestVariables_hook, 0x1997, 0x55c3);
  CYC(0x55c3, 0x55c4); alu_xor(gb, A);
  CYC(0x55c4, 0x55c7); mem_wr(gb, wcc50, A);
  CYC(0x55c7, 0x55ca); interactionIncState_hook(gb);
}
