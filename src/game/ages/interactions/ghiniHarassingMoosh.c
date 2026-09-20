#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode73_jump_table(GB *gb) {
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

static void interactionCode73_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_GHINI_HARASSING_MOOSH
void interactionCode73_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5db5, 0x5db6); H = D;
  CYC(0x5db6, 0x5db8); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5db8, 0x5db9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5db9, 0x5dba); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5dba, 0x5dbc); goto checkState; } // jr nz
  CYC(0x5dba, 0x5dbc);
  CYC(0x5dbc, 0x5dbd); L = alu_inc8(gb, L);
  CYC(0x5dbd, 0x5dbe); A = mem_rd(gb, HL);
  CYC(0x5dbe, 0x5dbf); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5dbf, 0x5dc1); goto checkState; } // jr z
  CYC(0x5dbf, 0x5dc1);
  CYC(0x5dc1, 0x5dc4); A = W8(wScrollMode);
  CYC(0x5dc4, 0x5dc6); alu_and(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(0x5dc6); return; } // ret nz
  CYC(0x5dc6, 0x5dc7);

checkState:
  CYC(0x5dc7, 0x5dc9); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5dc9, 0x5dca); A = mem_rd(gb, DE);
  {
    CYC(0x5dca, 0x5dcb); push_effect(gb, 0x5dcb);
    uint16_t target = interactionCode73_jump_table(gb);
    if (target == 0x5e01) goto state1;
  }

  CYC(0x5dcf, 0x5dd1); A = 0x01;
  CYC(0x5dd1, 0x5dd2); mem_wr(gb, DE, A);
  // Delete self if they shouldn't be here right now
  CYC(0x5dd2, 0x5dd5); A = W8(wEssencesObtained);
  CYC(0x5dd5, 0x5dd7); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x5dd7, 0x5dd9); goto deleteThis; } // jr z
  CYC(0x5dd7, 0x5dd9);
  CYC(0x5dd9, 0x5ddc); A = mem_rd(gb, wGroup1RoomFlags + 0x79); // wPastRoomFlags+$79
  CYC(0x5ddc, 0x5dde); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(0x5dde, 0x5de0); goto deleteThis; } // jr z
  CYC(0x5dde, 0x5de0);
  CYC(0x5de0, 0x5de3); A = W8(wMooshState);
  CYC(0x5de3, 0x5de5); alu_and(gb, 0x60);
  if (!(F & FZ)) { CYCT(0x5de5, 0x5de7); goto deleteThis; } // jr nz
  CYC(0x5de5, 0x5de7);
  CALL_C(0x5de7, interactionInitGraphics_hook, 0x15fb, 0x5dea);
  CALL_C(0x5dea, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5ded);
  CYC(0x5ded, 0x5def); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x5def, 0x5df1); mem_wr(gb, HL, 0xfe); // -2
  // Load script
  CYC(0x5df1, 0x5df3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5df3, 0x5df4); A = mem_rd(gb, DE);
  CYC(0x5df4, 0x5df7); SET_HL(0x5e23); // @scriptTable
  CYC(0x5df7, 0x5df8); interactionCode73_add_double_index(gb, 0x5df8);
  CYC(0x5df8, 0x5df9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5df9, 0x5dfa); H = mem_rd(gb, HL);
  CYC(0x5dfa, 0x5dfb); L = A;
  CALL_C(0x5dfb, interactionSetScript_hook, 0x2544, 0x5dfe);
  CYC(0x5dfe, 0x5e01); objectSetVisiblec0_hook(gb); return; // jp

state1:
  CALL_C(0x5e01, interactionAnimate_hook, 0x261b, 0x5e04);
  CYC(0x5e04, 0x5e06); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5e06, 0x5e07); A = mem_rd(gb, DE);
  CYC(0x5e07, 0x5e08); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5e08, 0x5e0a); goto l5e1c; } // jr z
  CYC(0x5e08, 0x5e0a);
  // While the ghini is moving, make them "rotate" in position.
  CALL_C(0x5e0a, objectApplySpeed_hook, 0x201d, 0x5e0d);
  CYC(0x5e0d, 0x5e0f); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5e0f, 0x5e10); A = mem_rd(gb, DE);
  CYC(0x5e10, 0x5e11); A = alu_dec8(gb, A);
  CYC(0x5e11, 0x5e13); alu_and(gb, 0x1f);
  CYC(0x5e13, 0x5e14); mem_wr(gb, DE, A);
  CYC(0x5e14, 0x5e16); alu_cp(gb, 0x18);
  if (!(F & FZ)) { CYCT(0x5e16, 0x5e18); goto l5e1c; } // jr nz
  CYC(0x5e16, 0x5e18);
  CYC(0x5e18, 0x5e19); alu_xor(gb, A);
  CYC(0x5e19, 0x5e1b); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5e1b, 0x5e1c); mem_wr(gb, DE, A);

l5e1c:
  CALL_C(0x5e1c, interactionRunScript_hook, 0x2552, 0x5e1f);
  if (!(F & FC)) { RET_TAKEN(0x5e1f); return; } // ret nc
  CYC(0x5e1f, 0x5e20);

deleteThis:
  CYC(0x5e20, 0x5e23); interactionDelete_hook(gb); return; // jp
}
