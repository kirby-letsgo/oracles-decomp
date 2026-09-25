#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/linkedFountainLady.s.
// INTERAC_LINKED_FOUNTAIN_LADY

static uint16_t linkedFountainLady_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// INTERAC_LINKED_FOUNTAIN_LADY
void s_interactionCoded8_hook(GB *gb) {
  BASE(interactionCoded8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkedFountainLady_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+38) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+11);
  if (F & FZ) { CYCT(b_+11, b_+14); TAIL(interactionDelete); }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); A = 0x18;
  CALL_C(b_+16, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+19);
  if (F & FZ) { CYCT(b_+19, b_+22); TAIL(interactionDelete); }
  CYC(b_+19, b_+22);
  CALL_C(b_+22, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+25);
  CALL_C(b_+25, s_interactionIncState, SYM(interactionIncState), b_+28);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01);
  CYC(b_+32, b_+35); SET_HL(SYM(linkedGameNpcScript));
  CALL_C(b_+35, s_interactionSetScript, SYM(interactionSetScript), b_+38);
state1:
  CALL_C(b_+38, s_interactionRunScript, SYM(interactionRunScript), b_+41);
  CYC(b_+41, b_+44);
  TAIL(interactionAnimateAsNpc);
}

