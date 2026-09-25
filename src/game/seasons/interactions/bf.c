#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/bf.s.
// cloaked twinrova?

static uint16_t bf_jump_table(GB *gb) {
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

// cloaked twinrova?
void s_interactionCodebf_hook(GB *gb) {
  BASE(interactionCodebf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bf_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+21) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, s_interactionIncState, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x3c);
  CYC(b_+18, b_+21);
  TAIL(objectSetVisible80);
state1:
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); push_effect(gb, b_+25);
  do { uint16_t jt_ = (bf_jump_table(gb));
    if (jt_ == b_+31) goto substate0;
    if (jt_ == b_+52) goto substate1;
    if (jt_ == b_+82) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+31, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+34);
  if (F & FZ) { CYCT(b_+34, b_+36); goto L_7935; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, wFrameCounter);
  CYC(b_+39, b_+40); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+40, b_+43); TAIL(objectSetInvisible); }
  CYC(b_+40, b_+43);
  CYC(b_+43, b_+46);
  TAIL(objectSetVisible);
L_7935:
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+52);
  TAIL(objectSetVisible);
substate1:
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+57, b_+59); goto subid1; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+62); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+62, b_+64); alu_bit(gb, 0, A);
  if (F & FZ) { RET_TAKEN(b_+64); return; }
  CYC(b_+64, b_+65);
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+68, b_+70); A = 0x02;
  CYC(b_+70, b_+73);
  TAIL(interactionSetAnimation);
subid1:
  CYC(b_+73, b_+76); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+76, b_+78); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+78, b_+81); TAIL(interactionDelete); }
  CYC(b_+78, b_+81);
  RET(b_+81); return;
substate2:
  CYC(b_+82, b_+85); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+85, b_+87); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+87, b_+90); TAIL(interactionDelete); }
  CYC(b_+87, b_+90);
  RET(b_+90); return;
}

