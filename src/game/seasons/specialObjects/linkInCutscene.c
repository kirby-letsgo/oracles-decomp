#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/specialObjects/linkInCutscene.s, bank $06.

// Animates and applies the z speed with gravity $20; on landing, animation 0 (Z set).
// Otherwise, once the animation's parameter reaches 3 (bit 7 clear), it restarts the animation
// at state 3 and returns NZ.
void s_seasonsFunc_06_7178_hook(GB *gb) {
  BASE(seasonsFunc_06_7178);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); C = 0x20;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto landed; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = 0x15; // SpecialObject.animParameter
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); L = 0x14; // SpecialObject.animCounter
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+26); A = 0x03;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+28); alu_or(gb, A);
  RET(b_+28); return;
landed:
  CYC(b_+29, b_+31); A = 0x00;
  CYC(b_+31, b_+34);
  TAIL(specialObjectSetAnimation);
}

// Sets animation a on the interaction in relatedObj2.
void s_setRelatedObj2Animation_hook(GB *gb) {
  BASE(setRelatedObj2Animation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+4); E = 0x19; // SpecialObject.relatedObj2
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); D = A;
  CYC(b_+6, b_+7); A = B;
  CALL_C(b_+7, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+10);
  CYC(b_+10, b_+11); SET_DE(pop_effect(gb));
  RET(b_+11); return;
}
