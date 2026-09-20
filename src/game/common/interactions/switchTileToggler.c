#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  CYC(0x4003, 0x4005); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4005, 0x4006); A = mem_rd(gb, DE);
  {
    CYC(0x4006, 0x4007); push_effect(gb, 0x4007);
    uint16_t target = interactionCode78_jump_table(gb);
    if (target == 0x400b) goto state0;
    goto state1;
  }

state0:
  CYC(0x400b, 0x400d); A = 0x01;
  CYC(0x400d, 0x400e); mem_wr(gb, DE, A);
  CYC(0x400e, 0x4011); A = W8(wSwitchState);
  CYC(0x4011, 0x4013); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4013, 0x4014); mem_wr(gb, DE, A);

state1:
  CYC(0x4014, 0x4017); A = W8(wSwitchState);
  CYC(0x4017, 0x4018); B = A;
  CYC(0x4018, 0x401a); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x401a, 0x401b); A = mem_rd(gb, DE);
  CYC(0x401b, 0x401c); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(0x401c); return; } // ret z
  CYC(0x401c, 0x401d);
  CYC(0x401d, 0x401e); A = B;
  CYC(0x401e, 0x401f); mem_wr(gb, DE, A);
  CYC(0x401f, 0x4021); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4021, 0x4022); A = mem_rd(gb, DE);
  CYC(0x4022, 0x4025); SET_HL(0x4035); // interactionCode78__tileReplacement
  CYC(0x4025, 0x4026); interactionCode78_addDoubleIndexToHl_from_rst(gb, 0x4026);
  CYC(0x4026, 0x4028); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4028, 0x4029); A = mem_rd(gb, DE);
  CYC(0x4029, 0x402a); alu_and(gb, B);
  if (F & FZ) { CYCT(0x402a, 0x402c); goto afterTileOffset; } // jr z
  CYC(0x402a, 0x402c);
  CYC(0x402c, 0x402d); SET_HL(HL + 1);

afterTileOffset:
  CYC(0x402d, 0x402f); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x402f, 0x4030); A = mem_rd(gb, DE);
  CYC(0x4030, 0x4031); C = A;
  CYC(0x4031, 0x4032); A = mem_rd(gb, HL);
  CYC(0x4032, 0x4035); setTile_hook(gb); // jp
}
