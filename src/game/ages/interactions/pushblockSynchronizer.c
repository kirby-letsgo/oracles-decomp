#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodebd), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodebd), (from), (to), true)

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
  BASE(interactionCodebd);
  CYC(b_+49, b_+50); push_effect(gb, HL); // push hl
  CYC(b_+50, b_+52); A = H8(hFF8B);
  CYC(b_+52, b_+54); alu_cp(gb, 0xda); // TILEINDEX_SOMARIA_BLOCK
  if (F & FZ) { CYCT(b_+54, b_+56); goto ret_label; } // jr z
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); A = L;
  CYC(b_+57, b_+59); H8(hFF8D) = A;
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+62, setShortPosition_hook, SYM(setShortPosition), b_+65);
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+67, b_+70); A = W8(wBlockPushAngle);
  CYC(b_+70, b_+72); alu_and(gb, 0x1f);
  CYC(b_+72, b_+73); mem_wr(gb, HL, A);
  CALL_C(b_+73, interactionCheckAdjacentTileIsSolid_hook, SYM(interactionCheckAdjacentTileIsSolid), b_+76);
  if (!(F & FZ)) { CYCT(b_+76, b_+78); goto ret_label; } // jr nz
  CYC(b_+76, b_+78);
  CALL_C(b_+78, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); goto ret_label; } // jr nz
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x14); // INTERAC_PUSHBLOCK
  CYC(b_+85, b_+87); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+87, b_+88); E = L;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); mem_wr(gb, HL, A);
  CYC(b_+90, b_+93); SET_BC(0xfe00); // -$02, $00
  CALL_C(b_+93, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+96);
  CYC(b_+96, b_+98); L = 0x70; // Interaction.var30
  CYC(b_+98, b_+100); A = H8(hFF8D);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);

ret_label: // interactionCodebd@return
  CYC(b_+101, b_+102); SET_HL(pop_effect(gb)); // pop hl
  CYC(b_+102, b_+103); L = alu_dec8(gb, L);
  CYC(b_+103, SYM(interactionCodebe)); return; // ret
}

// ==================================================================================================
// INTERAC_PUSHBLOCK_SYNCHRONIZER
// ==================================================================================================
void interactionCodebd_hook(GB *gb) {
  BASE(interactionCodebd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodebd_jump_table(gb);
    if (target == SYM(interactionIncState)) { interactionIncState_hook(gb); return; } // state list's 1st entry jumps straight into interactionIncState
    if (target == b_+43) goto state2;
    // target == 0x7077 falls through to state1
  }

  // interactionCodebd@state1
  CYC(b_+10, b_+13); A = W8(w1ReservedInteraction1_enabled);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+15); ret_effect(gb); return; } // ret z
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+18); A = W8(w1ReservedInteraction1_var31); // Tile index of block being pushed
  CYC(b_+18, b_+20); H8(hFF8B) = A;
  CALL_C(b_+20, findTileInRoom_hook, SYM(findTileInRoom), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto incState; } // jr nz
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); interactionCodebd_pushBlockAt(gb, sp0_); // call

l7089:
  CYC(b_+28, b_+30); A = H8(hFF8B);
  CALL_C(b_+30, backwardsSearch_hook, SYM(backwardsSearch), b_+33);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto incState; } // jr nz
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); interactionCodebd_pushBlockAt(gb, sp0_); // call
  CYC(b_+38, b_+40); goto l7089; // jr

incState: // interactionCodebd@incState
  CYC(b_+40, b_+43); interactionIncState_hook(gb); return; // jp

state2: // interactionCodebd@state2
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+45, b_+47); A = 0x01;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); ret_effect(gb); return; // ret
}
