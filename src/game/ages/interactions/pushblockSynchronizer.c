#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/pushblockSynchronizer.s (interactionCodebd /
// INTERAC_PUSHBLOCK_SYNCHRONIZER), bank 0x0b.

static uint16_t interactionCodebd_jump_table(GB *gb) {
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

// interactionCodebd@pushBlockAt (0b:709e): true local, always called with a genuine return
// expected, never dispatches into another hook -- a plain helper with no push/ret_effect at its
// own boundary is correct and safe (matches src/game/ball.c's func_6b00 precedent). It uses
// CALL_C internally so it takes sp0_.
static void interactionCodebd_pushBlockAt(GB *gb, uint16_t sp0_) {
  CYC(0x709e, 0x709f); push_effect(gb, HL); // push hl
  CYC(0x709f, 0x70a1); A = H8(hFF8B);
  CYC(0x70a1, 0x70a3); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (F & FZ) { CYCT(0x70a3, 0x70a5); goto ret_label; } // jr z
  CYC(0x70a3, 0x70a5);
  CYC(0x70a5, 0x70a6); A = L;
  CYC(0x70a6, 0x70a8); H8(hFF8D) = A;
  CYC(0x70a8, 0x70a9); H = D;
  CYC(0x70a9, 0x70ab); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(0x70ab, setShortPosition_hook, 0x20b8, 0x70ae);
  CYC(0x70ae, 0x70b0); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x70b0, 0x70b3); A = W8(wBlockPushAngle);
  CYC(0x70b3, 0x70b5); alu_and(gb, 0x1f);
  CYC(0x70b5, 0x70b6); mem_wr(gb, HL, A);
  CALL_C(0x70b6, interactionCheckAdjacentTileIsSolid_hook, 0x271d, 0x70b9);
  if (!(F & FZ)) { CYCT(0x70b9, 0x70bb); goto ret_label; } // jr nz
  CYC(0x70b9, 0x70bb);
  CALL_C(0x70bb, getFreeInteractionSlot_hook, 0x3aef, 0x70be);
  if (!(F & FZ)) { CYCT(0x70be, 0x70c0); goto ret_label; } // jr nz
  CYC(0x70be, 0x70c0);
  CYC(0x70c0, 0x70c2); mem_wr(gb, HL, 0x14); // INTERAC_PUSHBLOCK
  CYC(0x70c2, 0x70c4); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x70c4, 0x70c5); E = L;
  CYC(0x70c5, 0x70c6); A = mem_rd(gb, DE);
  CYC(0x70c6, 0x70c7); mem_wr(gb, HL, A);
  CYC(0x70c7, 0x70ca); SET_BC(0xfe00); // -$02, $00
  CALL_C(0x70ca, objectCopyPositionWithOffset_hook, 0x225a, 0x70cd);
  CYC(0x70cd, 0x70cf); L = 0x70; // Interaction.var30
  CYC(0x70cf, 0x70d1); A = H8(hFF8D);
  CYC(0x70d1, 0x70d2); mem_wr(gb, HL, A);

ret_label: // interactionCodebd@return
  CYC(0x70d2, 0x70d3); SET_HL(pop_effect(gb)); // pop hl
  CYC(0x70d3, 0x70d4); L = alu_dec8(gb, L);
  CYC(0x70d4, 0x70d5); return; // ret
}

// ==================================================================================================
// INTERAC_PUSHBLOCK_SYNCHRONIZER
// ==================================================================================================
void interactionCodebd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x706d, 0x706f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x706f, 0x7070); A = mem_rd(gb, DE);
  {
    CYC(0x7070, 0x7071); push_effect(gb, 0x7071);
    uint16_t target = interactionCodebd_jump_table(gb);
    if (target == 0x23e0) { interactionIncState_hook(gb); return; } // state list's 1st entry jumps straight into interactionIncState
    if (target == 0x7098) goto state2;
    // target == 0x7077 falls through to state1
  }

  // interactionCodebd@state1
  CYC(0x7077, 0x707a); A = W8(w1ReservedInteraction1_enabled);
  CYC(0x707a, 0x707b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x707b, 0x707c); ret_effect(gb); return; } // ret z
  CYC(0x707b, 0x707c);
  CYC(0x707c, 0x707f); A = W8(w1ReservedInteraction1_var31); // Tile index of block being pushed
  CYC(0x707f, 0x7081); H8(hFF8B) = A;
  CALL_C(0x7081, findTileInRoom_hook, 0x15cc, 0x7084);
  if (!(F & FZ)) { CYCT(0x7084, 0x7086); goto incState; } // jr nz
  CYC(0x7084, 0x7086);
  CYC(0x7086, 0x7089); interactionCodebd_pushBlockAt(gb, sp0_); // call

l7089:
  CYC(0x7089, 0x708b); A = H8(hFF8B);
  CALL_C(0x708b, backwardsSearch_hook, 0x15d0, 0x708e);
  if (!(F & FZ)) { CYCT(0x708e, 0x7090); goto incState; } // jr nz
  CYC(0x708e, 0x7090);
  CYC(0x7090, 0x7093); interactionCodebd_pushBlockAt(gb, sp0_); // call
  CYC(0x7093, 0x7095); goto l7089; // jr

incState: // interactionCodebd@incState
  CYC(0x7095, 0x7098); interactionIncState_hook(gb); return; // jp

state2: // interactionCodebd@state2
  CYC(0x7098, 0x709a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x709a, 0x709c); A = 0x01;
  CYC(0x709c, 0x709d); mem_wr(gb, DE, A);
  CYC(0x709d, 0x709e); ret_effect(gb); return; // ret
}
