#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/97.s.
// INTERAC_97

static uint16_t i97_jump_table(GB *gb) {
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

// INTERAC_97
void s_interactionCode97_hook(GB *gb) {
  BASE(interactionCode97);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i97_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+17) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+17);
  TAIL(objectSetVisible83);
state1:
  CYC(b_+17, b_+20); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+25); TAIL(interactionDelete); }
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); push_effect(gb, b_+29);
  do { uint16_t jt_ = (i97_jump_table(gb));
    if (jt_ == b_+35) goto substate0;
    if (jt_ == b_+71) goto substate1;
    if (jt_ == b_+97) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+35, b_+38); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CALL_C(b_+42, s_interactionAnimate, SYM(interactionAnimate), b_+45);
  CYC(b_+45, b_+48); SET_HL(wTmpcfc0);
  CYC(b_+48, b_+50); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CALL_C(b_+51, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+54);
  CALL_C(b_+54, s_objectSetVisible81, SYM(objectSetVisible81), b_+57);
func_581d:
  CYC(b_+57, b_+58); push_effect(gb, DE);
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); L = 0x57;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+63); D = A;
  CYC(b_+63, b_+66); SET_BC(0x0301);
  CALL_C(b_+66, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+69);
  CYC(b_+69, b_+70); SET_DE(pop_effect(gb));
  RET(b_+70); return;
substate1:
  CYC(b_+71, b_+74); SET_HL(wTmpcfc0);
  CYC(b_+74, b_+76); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+76, b_+78); goto L_5843; }
  CYC(b_+76, b_+78);
  CALL_C(b_+78, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+81);
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x10);
  CYC(b_+85, b_+87); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x14);
  CYC(b_+89, b_+92); SET_BC(0xfe80);
  CALL_C(b_+92, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+95);
L_5843:
  CYC(b_+95, b_+97);
  goto func_581d;
substate2:
  CYC(b_+97, b_+99); C = 0x20;
  CALL_C(b_+99, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+105); TAIL(objectApplySpeed); }
  CYC(b_+102, b_+105);
  CYC(b_+105, b_+108);
  TAIL(interactionDelete);
}

