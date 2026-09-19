#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x5f4e, 0x5f50); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5f50, 0x5f51); A = mem_rd(gb, DE);
  {
    CYC(0x5f51, 0x5f52); push_effect(gb, 0x5f52);
    uint16_t target = interactionCodea9_jump_table(gb);
    if (target == 0x261b) { interactionAnimate_hook(gb); return; } // state1: jumps straight into interactionAnimate
    if (target == 0x5fa3) goto state2;
    // target == 0x5f58 falls through to state0
  }

  // interactionCodea9@state0
  CYC(0x5f58, 0x5f5a); A = 0x01;
  CYC(0x5f5a, 0x5f5b); mem_wr(gb, DE, A); // ld (de),a [state]
  CYC(0x5f5b, 0x5f5d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5f5d, 0x5f5e); A = mem_rd(gb, DE);
  CYC(0x5f5e, 0x5f60); alu_cp(gb, 0x06);
  if (!(F & FC)) { CALL_C_CC(0x5f60, interactionIncState_hook, 0x23e0, 0x5f63); } else CYC(0x5f60, 0x5f63); // call nc
  CALL_C(0x5f63, interactionInitGraphics_hook, 0x15fb, 0x5f66); // SWITCHES THREADS
  CALL_C(0x5f66, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5f69);
  CYC(0x5f69, 0x5f6b); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5f6b, 0x5f6c); A = mem_rd(gb, HL);
  CYC(0x5f6c, 0x5f6d); B = A;
  CYC(0x5f6d, 0x5f6f); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x5f6f, 0x5f71); goto positionAndVisible; } // jr c
  CYC(0x5f6f, 0x5f71);
  CALL_C(0x5f71, getThisRoomFlags_hook, 0x197d, 0x5f74);
  CYC(0x5f74, 0x5f76); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x5f76, 0x5f79); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5f76, 0x5f79);
  CYC(0x5f79, 0x5f7a); A = mem_rd(gb, DE); // ld a,(de) [subid]

positionAndVisible:
  CYC(0x5f7a, 0x5f7c); alu_and(gb, 0x03);
  CYC(0x5f7c, 0x5f7d); alu_add(gb, A); // add a
  CYC(0x5f7d, 0x5f7e); alu_add(gb, A); // add a
  CYC(0x5f7e, 0x5f7f); alu_add(gb, A); // add a
  CYC(0x5f7f, 0x5f81); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(0x5f81, 0x5f82); alu_add(gb, mem_rd(gb, HL)); // add (hl)
  CYC(0x5f82, 0x5f83); mem_wr(gb, HL, A);
  CYC(0x5f83, 0x5f84); A = B;
  CYC(0x5f84, 0x5f87); SET_HL(0x5f91); // @positions
  CYC(0x5f87, 0x5f88); interactionCodea9_addDoubleIndexToHl_from_rst(gb, 0x5f88);
  CYC(0x5f88, 0x5f89); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5f89, 0x5f8a); C = mem_rd(gb, HL);
  CYC(0x5f8a, 0x5f8b); B = A;
  CALL_C(0x5f8b, interactionSetPosition_hook, 0x2773, 0x5f8e);
  CYC(0x5f8e, 0x5f91); objectSetVisiblec2_hook(gb); return; // jp

  // @positions (0b:5f91): pure ROM data, not ported as code -- three groups of three (yh, zh)
  // byte pairs indexed by (subid & 3) * 2, referenced only via SET_HL(0x5f91) above.

state2:
  CALL_C(0x5fa3, interactionAnimate_hook, 0x261b, 0x5fa6);
  CYC(0x5fa6, 0x5fa9); A = W8(wFrameCounter);
  CYC(0x5fa9, 0x5faa); alu_rrca(gb);
  if (F & FC) { CYCT(0x5faa, 0x5fad); objectSetVisible_hook(gb); return; } // jp c
  CYC(0x5faa, 0x5fad);
  CYC(0x5fad, 0x5fb0); objectSetInvisible_hook(gb); return; // jp
}
