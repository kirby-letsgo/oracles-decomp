#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode2d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode2d), (from), (to), true)

// interactionCode2d@warpDestVariables: m_HardcodedWarpA ROOM_AGES_4d4, $0c, $67, $03
#define veranCutsceneFaceWarpDestVariables_bank08 SYM(interactionCode2d__warpDestVariables)
// mainScripts.veranFaceCutsceneScript (bank $0c script data, referenced by address only).
#define veranFaceCutsceneScript_bank0c SYM(veranFaceCutsceneScript_b0c)

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
  BASE(interactionCode2d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (veranCutsceneFace_jumpTable(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+31) { goto state1; }
    else if (jt_ == b_+30) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+16);
  CALL_C(b_+16, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+19);
  CYC(b_+19, b_+21); A = 0x87; // PALH_87
  CALL_C(b_+21, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+24);
  CYC(b_+24, b_+27); SET_HL(veranFaceCutsceneScript_bank0c);
  CALL_C(b_+27, interactionSetScript_hook, SYM(interactionSetScript), b_+30);
  // falls through into state 2 (just a ret)

state2:
  CYC(b_+30, b_+31); ret_effect(gb);
  return;

state1:
  CALL_C(b_+31, interactionAnimate_hook, SYM(interactionAnimate), b_+34);
  CALL_C(b_+34, interactionRunScript_hook, SYM(interactionRunScript), b_+37);
  if (!(F & FC)) {
    CYCT(b_+37, b_+38); ret_effect(gb); return;
  }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); SET_HL(veranCutsceneFaceWarpDestVariables_bank08);
  CALL_C(b_+41, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+44);
  CYC(b_+44, b_+45); alu_xor(gb, A);
  CYC(b_+45, b_+48); mem_wr(gb, wcc50, A);
  CYC(b_+48, b_+51); interactionIncState_hook(gb);
}
