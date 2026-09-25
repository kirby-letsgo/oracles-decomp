#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/twinrovaFlame.s.
// INTERAC_TWINROVA_FLAME

static uint16_t twinrovaFlame_jump_table(GB *gb) {
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

static void twinrovaFlame_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void s_interactionCodea9_hook(GB *gb) {
  BASE(interactionCodea9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (twinrovaFlame_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+22) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); A = 0x1e;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+19);
  CYC(b_+19, b_+22);
  TAIL(objectSetVisiblec0);
state1:
  CALL_C(b_+22, s_interactionAnimate, SYM(interactionAnimate), b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (twinrovaFlame_jump_table(gb));
    if (jt_ == b_+47) goto substate0;
    if (jt_ == b_+63) goto substate1;
    if (jt_ == b_+77) goto substate2;
    if (jt_ == b_+90) goto substate3;
    if (jt_ == b_+104) goto substate4;
    if (jt_ == b_+125) goto substate5;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+47, b_+50); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CALL_C(b_+52, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+55);
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+59); alu_add(gb, A);
  CYC(b_+59, b_+60); A = alu_inc8(gb, A);
  CYC(b_+60, b_+63);
  TAIL(interactionSetAnimation);
substate1:
  CYC(b_+63, b_+66); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+66, b_+68); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CALL_C(b_+69, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+72);
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x0a);
  RET(b_+76); return;
substate2:
  CALL_C(b_+77, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CALL_C(b_+81, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+84);
  CYC(b_+84, b_+87); SET_BC(IO_P1);
  CYC(b_+87, b_+90);
  TAIL(objectSetSpeedZ);
substate3:
  CYC(b_+90, b_+92); C = 0x20;
  CALL_C(b_+92, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+95);
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CALL_C(b_+96, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+99);
  CYC(b_+99, b_+101); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x50);
  RET(b_+103); return;
substate4:
  CALL_C(b_+104, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+107);
  if (!(F & FZ)) { RET_TAKEN(b_+107); return; }
  CYC(b_+107, b_+108);
  CALL_C(b_+108, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+111);
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+113, b_+114); A = mem_rd(gb, HL);
  CYC(b_+114, b_+116); alu_cp(gb, 0x01);
  CYC(b_+116, b_+118); A = 0x04;
  if (F & FZ) { CYCT(b_+118, b_+120); goto L_692e; }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+121); alu_xor(gb, A);
L_692e:
  CYC(b_+121, b_+123); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+123, b_+124); mem_wr(gb, HL, A);
  RET(b_+124); return;
substate5:
  CYC(b_+125, b_+127); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+127, b_+128); A = mem_rd(gb, DE);
  CYC(b_+128, b_+130); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+130, b_+132); goto applySpeed; }
  CYC(b_+130, b_+132);
  CYC(b_+132, b_+134); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+134, b_+136); goto applySpeed; }
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+138); alu_cp(gb, 0x05);
  if (!(F & FZ)) { RET_TAKEN(b_+138); return; }
  CYC(b_+138, b_+139);
applySpeed:
  CYC(b_+139, b_+142);
  TAIL(objectApplySpeed);
}

void s_interactionCodeb0_hook(GB *gb) {
  BASE(interactionCodeb0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (twinrovaFlame_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), s_interactionAnimate)) { s_interactionAnimate(gb); return; }
    if (jt_ == b_+108) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_cp(gb, 0x0b);
  if (!(F & FC)) CALL_C_CC(b_+18, s_interactionIncState, SYM(interactionIncState), b_+21);
  else CYC(b_+18, b_+21);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto L_6e13; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); A = 0xb0;
  CYC(b_+26, b_+29); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  CYC(b_+29, b_+32); mem_wr(gb, wLoadedTreeGfxIndex, A);
L_6e13:
  CALL_C(b_+32, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+35);
  CALL_C(b_+35, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+38);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+42); B = A;
  CYC(b_+42, b_+44); alu_cp(gb, 0x08);
  if (F & FC) { CYCT(b_+44, b_+46); goto L_6e2c; }
  CYC(b_+44, b_+46);
  CALL_C(b_+46, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+51, b_+54); TAIL(interactionDelete); }
  CYC(b_+51, b_+54);
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+57); alu_sub(gb, 0x05);
L_6e2c:
  CYC(b_+57, b_+58); alu_add(gb, A);
  CYC(b_+58, b_+59); alu_add(gb, A);
  CYC(b_+59, b_+60); alu_add(gb, A);
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+62, b_+63); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+63, b_+64); mem_wr(gb, HL, A);
  CYC(b_+64, b_+65); A = B;
  CYC(b_+65, b_+68); SET_HL(b_+80 /* @positions */);
  CYC(b_+68, b_+69); twinrovaFlame_add_double_index(gb, b_+69);
  CYC(b_+69, b_+70); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+70, b_+72); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+74); E = alu_inc8(gb, E);
  CYC(b_+74, b_+75); E = alu_inc8(gb, E);
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CYC(b_+77, b_+80);
  TAIL(objectSetVisiblec2);

state2:
  CALL_C(b_+108, s_interactionAnimate, SYM(interactionAnimate), b_+111);
  CYC(b_+111, b_+114); A = mem_rd(gb, wFrameCounter);
  CYC(b_+114, b_+115); alu_rrca(gb);
  if (F & FC) { CYCT(b_+115, b_+118); TAIL(objectSetVisible); }
  CYC(b_+115, b_+118);
  CYC(b_+118, b_+121);
  TAIL(objectSetInvisible);
}

