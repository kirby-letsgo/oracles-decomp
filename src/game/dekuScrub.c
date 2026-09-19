#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

// 0b:79b9, called once from interactionCoded6@state0.
void interactionCoded6_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79b9, interactionInitGraphics_hook, 0x15fb, 0x79bc);
  CALL_C(0x79bc, objectMarkSolidPosition_hook, 0x24f0, 0x79bf);
  CYC(0x79bf, 0x79c2); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_DEKU_SCRUB
//
// Variables:
//   var3e: 0 if the deku scrub is hiding, 1 if not
//   var3f: Secret index (for "linkedGameNpcScript")
// ==================================================================================================
void interactionCoded6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x797b, checkInteractionState_hook, 0x23fe, 0x797e);
  if (!(F & FZ)) { CYCT(0x797e, 0x7980); goto state1; } // jr nz
  CYC(0x797e, 0x7980);

  // interactionCoded6@state0 (0x7980): reached solely by fallthrough, never a jump target.
  CALL_C(0x7980, interactionCoded6_initialize_hook, 0x79b9, 0x7983);
  CALL_C(0x7983, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7986);
  CYC(0x7986, 0x7988); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7988, 0x798a); mem_wr(gb, HL, 0x07); // DEKU_SECRET & 0x0f
  CYC(0x798a, 0x798d); SET_HL(0x7ed9); // mainScripts.linkedGameNpcScript
  CALL_C(0x798d, interactionSetScript_hook, 0x2544, 0x7990);
  CALL_C(0x7990, interactionRunScript_hook, 0x2552, 0x7993);

state1:
  CALL_C(0x7993, interactionRunScript_hook, 0x2552, 0x7996);
  if (F & FC) { CYCT(0x7996, 0x7999); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(0x7996, 0x7999);
  CALL_C(0x7999, interactionAnimateAsNpc_hook, 0x26db, 0x799c);
  CYC(0x799c, 0x799e); C = 0x20;
  CALL_C(0x799e, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x79a1);
  CYC(0x79a1, 0x79a2); H = D;
  CYC(0x79a2, 0x79a4); L = INTERACTION_BASE + OBJ_VAR3E;
  if (F & FC) { CYCT(0x79a4, 0x79a6); goto linkIsClose; } // jr c
  CYC(0x79a4, 0x79a6);
  CYC(0x79a6, 0x79a7); A = mem_rd(gb, HL);
  CYC(0x79a7, 0x79a8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x79a8, 0x79a9); ret_effect(gb); return; } // ret z
  CYC(0x79a8, 0x79a9);
  CYC(0x79a9, 0x79aa); alu_xor(gb, A);
  CYC(0x79aa, 0x79ab); mem_wr(gb, HL, A);
  CYC(0x79ab, 0x79ad); A = 0x03;
  CYC(0x79ad, 0x79b0); interactionSetAnimation_hook(gb); return; // jp

linkIsClose:
  CYC(0x79b0, 0x79b1); A = mem_rd(gb, HL);
  CYC(0x79b1, 0x79b2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x79b2, 0x79b3); ret_effect(gb); return; } // ret nz
  CYC(0x79b2, 0x79b3);
  CYC(0x79b3, 0x79b4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x79b4, 0x79b6); A = 0x01;
  CYC(0x79b6, 0x79b9); interactionSetAnimation_hook(gb); return; // jp
}
