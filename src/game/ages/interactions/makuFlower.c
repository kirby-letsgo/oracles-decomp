#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode86_jump_table(GB *gb) {
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

// INTERAC_MAKU_FLOWER
void interactionCode86_hook(GB *gb) {
  BASE(interactionCode86);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode86_jump_table(gb);
    if (target == b_+8) goto subid0;
    if (target == b_+61) goto subid1;
    HANDOFF(target);
  }

// Present maku tree flower
subid0:
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto subid0State1; } // jr nz
  CYC(b_+11, b_+13);

  // interactionCode86@subid0State0
  CALL_C(b_+13, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+16);
  CALL_C(b_+16, objectSetVisible82_hook, SYM(objectSetVisible82), b_+19);
  CALL_C(b_+19, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+22);
  CALL_C(b_+22, interactionIncState_hook, SYM(interactionIncState), b_+25);

subid0State1:
  // Watch var3b of relatedObject1 to set the flower's animation
  CYC(b_+25, b_+27); A = OBJ_VAR3B; // Object.var3b
  CALL_C(b_+27, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+34); SET_BC(b_+56); // @anims
  CALL_C(b_+34, addAToBc_hook, 0x006d, b_+37);
  CYC(b_+37, b_+38); A = mem_rd(gb, BC);
  CYC(b_+38, b_+40); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+40, b_+42); goto setAnimA; } // jr z
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); B = A;
  CYC(b_+43, b_+45); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto setAnimB; } // jr nz
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); B = 0x03;

setAnimB:
  CYC(b_+52, b_+53); A = B;

setAnimA:
  CYC(b_+53, b_+56); TAIL(interactionSetAnimation); // jp

subid1:
  CALL_C(b_+61, checkInteractionState_hook, SYM(checkInteractionState), b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); goto subid1State1; } // jr nz
  CYC(b_+64, b_+66);

  // interactionCode86@subid1State0
  CALL_C(b_+66, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+69);
  CALL_C(b_+69, objectSetVisible82_hook, SYM(objectSetVisible82), b_+72);
  CALL_C(b_+72, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+75);
  CALL_C(b_+75, interactionIncState_hook, SYM(interactionIncState), b_+78);
  CYC(b_+78, b_+80); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0xd4);

subid1State1:
  CYC(b_+82, b_+83); H = D;
  CYC(b_+83, b_+85); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+85, b_+86); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  if (F & FZ) { CYCT(b_+86, b_+89); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+86, b_+89);
  RET(b_+89);
  return; // ret
}
