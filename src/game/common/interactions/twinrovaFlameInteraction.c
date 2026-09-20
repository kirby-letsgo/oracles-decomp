#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodea9), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodea9), (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/twinrovaFlame.s (INTERAC_TWINROVA_FLAME
// under ROM_AGES), bank 0x0b. This is a DIFFERENT, unrelated source file from
// object_code/common/parts/twinrovaFlame.s (a PART code routine, bank 0x11, already ported in
// src/game/twinrovaFlame.c) that happens to share the basename "twinrovaFlame.s" in a different
// disasm directory -- kept as its own file to avoid the same collision class documented for
// sparkle.s/sparkleInteraction.c and twinrova.s/twinrovaInteraction.c earlier this session.

static uint16_t interactionCodea9_jump_table(GB *gb) {
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

static void interactionCodea9_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_TWINROVA_FLAME
// ==================================================================================================
void interactionCodea9_hook(GB *gb) {
  BASE(interactionCodea9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodea9_jump_table(gb);
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; } // state1: jumps straight into interactionAnimate
    if (target == b_+85) goto state2;
    // target == 0x5f58 falls through to state0
  }

  // interactionCodea9@state0
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A); // ld (de),a [state]
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_cp(gb, 0x06);
  if (!(F & FC)) { CALL_C_CC(b_+18, interactionIncState_hook, SYM(interactionIncState), b_+21); } else CYC(b_+18, b_+21); // call nc
  CALL_C(b_+21, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+24); // SWITCHES THREADS
  CALL_C(b_+24, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+33); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+33, b_+35); goto positionAndVisible; } // jr c
  CYC(b_+33, b_+35);
  CALL_C(b_+35, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+38);
  CYC(b_+38, b_+40); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+40, b_+43); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+40, b_+43);
  CYC(b_+43, b_+44); A = mem_rd(gb, DE); // ld a,(de) [subid]

positionAndVisible:
  CYC(b_+44, b_+46); alu_and(gb, 0x03);
  CYC(b_+46, b_+47); alu_add(gb, A); // add a
  CYC(b_+47, b_+48); alu_add(gb, A); // add a
  CYC(b_+48, b_+49); alu_add(gb, A); // add a
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+51, b_+52); alu_add(gb, mem_rd(gb, HL)); // add (hl)
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+54); A = B;
  CYC(b_+54, b_+57); SET_HL(b_+67); // @positions
  CYC(b_+57, b_+58); interactionCodea9_addDoubleIndexToHl_from_rst(gb, b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+59, b_+60); C = mem_rd(gb, HL);
  CYC(b_+60, b_+61); B = A;
  CALL_C(b_+61, interactionSetPosition_hook, SYM(interactionSetPosition), b_+64);
  CYC(b_+64, b_+67); objectSetVisiblec2_hook(gb); return; // jp

  // @positions (0b:5f91): pure ROM data, not ported as code -- three groups of three (yh, zh)
  // byte pairs indexed by (subid & 3) * 2, referenced only via SET_HL(0x5f91) above.

state2:
  CALL_C(b_+85, interactionAnimate_hook, SYM(interactionAnimate), b_+88);
  CYC(b_+88, b_+91); A = W8(wFrameCounter);
  CYC(b_+91, b_+92); alu_rrca(gb);
  if (F & FC) { CYCT(b_+92, b_+95); objectSetVisible_hook(gb); return; } // jp c
  CYC(b_+92, b_+95);
  CYC(b_+95, SYM(interactionCodeaa)); objectSetInvisible_hook(gb); return; // jp
}
