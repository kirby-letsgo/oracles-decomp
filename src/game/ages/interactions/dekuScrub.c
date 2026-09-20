#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCoded6), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCoded6), (from), (to), true)

// 0b:79b9, called once from interactionCoded6@state0.
void interactionCoded6_initialize_hook(GB *gb) {
  BASE(interactionCoded6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+62, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+65);
  CALL_C(b_+65, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+68);
  CYC(b_+68, SYM(interactionCoded7)); interactionIncState_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_DEKU_SCRUB
//
// Variables:
//   var3e: 0 if the deku scrub is hiding, 1 if not
//   var3f: Secret index (for "linkedGameNpcScript")
// ==================================================================================================
void interactionCoded6_hook(GB *gb) {
  BASE(interactionCoded6);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interactionCoded6@state0 (0x7980): reached solely by fallthrough, never a jump target.
  CALL_C(b_+5, interactionCoded6_initialize_hook, b_+62, b_+8);
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x07); // DEKU_SECRET & 0x0f
  CYC(b_+15, b_+18); SET_HL((SYM(interactionCoded8__subid4Script) + 93)); // mainScripts.linkedGameNpcScript
  CALL_C(b_+18, interactionSetScript_hook, SYM(interactionSetScript), b_+21);
  CALL_C(b_+21, interactionRunScript_hook, SYM(interactionRunScript), b_+24);

state1:
  CALL_C(b_+24, interactionRunScript_hook, SYM(interactionRunScript), b_+27);
  if (F & FC) { CYCT(b_+27, b_+30); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(b_+27, b_+30);
  CALL_C(b_+30, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+33);
  CYC(b_+33, b_+35); C = 0x20;
  CALL_C(b_+35, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+38);
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_VAR3E;
  if (F & FC) { CYCT(b_+41, b_+43); goto linkIsClose; } // jr c
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+45, b_+46); ret_effect(gb); return; } // ret z
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+47); alu_xor(gb, A);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+50); A = 0x03;
  CYC(b_+50, b_+53); interactionSetAnimation_hook(gb); return; // jp

linkIsClose:
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+55, b_+56); ret_effect(gb); return; } // ret nz
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+57, b_+59); A = 0x01;
  CYC(b_+59, b_+62); interactionSetAnimation_hook(gb); return; // jp
}
