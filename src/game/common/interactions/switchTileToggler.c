#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode78), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode78), (from), (to), true)

static uint16_t interactionCode78_jump_table(GB *gb) {
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

static void interactionCode78_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// INTERAC_SWITCH_TILE_TOGGLER: swaps a floor tile between two indices as the
// dungeon's crystal-switch state flips, keyed by Interaction.xh into a table of
// on/off tile pairs (interactionCode78__tileReplacement, pure data, no code).
void interactionCode78_hook(GB *gb) {
  BASE(interactionCode78);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode78_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); A = W8(wSwitchState);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);

state1:
  CYC(b_+17, b_+20); A = W8(wSwitchState);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+25); return; } // ret z
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); A = B;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+34); SET_HL(b_+50); // interactionCode78__tileReplacement
  CYC(b_+34, b_+35); interactionCode78_addDoubleIndexToHl_from_rst(gb, b_+35);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+39, b_+41); goto afterTileOffset; } // jr z
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); SET_HL(HL + 1);

afterTileOffset:
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); C = A;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+50); setTile_hook(gb); // jp
}
