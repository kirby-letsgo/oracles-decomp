#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6647, 0x6649); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6649, 0x664a); A = mem_rd(gb, DE);
  {
    CYC(0x664a, 0x664b); push_effect(gb, 0x664b);
    uint16_t target = interactionCode86_jump_table(gb);
    if (target == 0x664f) goto subid0;
    if (target == 0x6684) goto subid1;
    HANDOFF(target);
  }

// Present maku tree flower
subid0:
  CALL_C(0x664f, checkInteractionState_hook, 0x23fe, 0x6652);
  if (!(F & FZ)) { CYCT(0x6652, 0x6654); goto subid0State1; } // jr nz
  CYC(0x6652, 0x6654);

  // interactionCode86@subid0State0
  CALL_C(0x6654, interactionInitGraphics_hook, 0x15fb, 0x6657);
  CALL_C(0x6657, objectSetVisible82_hook, 0x1e69, 0x665a);
  CALL_C(0x665a, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x665d);
  CALL_C(0x665d, interactionIncState_hook, 0x23e0, 0x6660);

subid0State1:
  // Watch var3b of relatedObject1 to set the flower's animation
  CYC(0x6660, 0x6662); A = OBJ_VAR3B; // Object.var3b
  CALL_C(0x6662, objectGetRelatedObject2Var_hook, 0x2164, 0x6665);
  CYC(0x6665, 0x6666); A = mem_rd(gb, HL);
  CYC(0x6666, 0x6669); SET_BC(0x667f); // @anims
  CALL_C(0x6669, addAToBc_hook, 0x006d, 0x666c);
  CYC(0x666c, 0x666d); A = mem_rd(gb, BC);
  CYC(0x666d, 0x666f); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x666f, 0x6671); goto setAnimA; } // jr z
  CYC(0x666f, 0x6671);
  CYC(0x6671, 0x6672); B = A;
  CYC(0x6672, 0x6674); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6674, 0x6675); A = mem_rd(gb, HL);
  CYC(0x6675, 0x6677); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x6677, 0x6679); goto setAnimB; } // jr nz
  CYC(0x6677, 0x6679);
  CYC(0x6679, 0x667b); B = 0x03;

setAnimB:
  CYC(0x667b, 0x667c); A = B;

setAnimA:
  CYC(0x667c, 0x667f); interactionSetAnimation_hook(gb); return; // jp

subid1:
  CALL_C(0x6684, checkInteractionState_hook, 0x23fe, 0x6687);
  if (!(F & FZ)) { CYCT(0x6687, 0x6689); goto subid1State1; } // jr nz
  CYC(0x6687, 0x6689);

  // interactionCode86@subid1State0
  CALL_C(0x6689, interactionInitGraphics_hook, 0x15fb, 0x668c);
  CALL_C(0x668c, objectSetVisible82_hook, 0x1e69, 0x668f);
  CALL_C(0x668f, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6692);
  CALL_C(0x6692, interactionIncState_hook, 0x23e0, 0x6695);
  CYC(0x6695, 0x6697); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6697, 0x6699); mem_wr(gb, HL, 0xd4);

subid1State1:
  CYC(0x6699, 0x669a); H = D;
  CYC(0x669a, 0x669c); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x669c, 0x669d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  if (F & FZ) { CYCT(0x669d, 0x66a0); interactionDelete_hook(gb); return; } // jp z
  CYC(0x669d, 0x66a0);
  RET(0x66a0);
  return; // ret
}
