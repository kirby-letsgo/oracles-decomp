#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodea7), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodea7), (from), (to), true)

static uint16_t interactionCodea7_jump_table(GB *gb) {
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

// ==================================================================================================
// INTERAC_ENDGAME_CUTSCENE_BIPSOM_FAMILY
// ==================================================================================================
void interactionCodea7_hook(GB *gb) {
  BASE(interactionCodea7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodea7_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+69) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisible82_hook, SYM(objectSetVisible82), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; } // ret nz
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wChildStage);
  CYC(b_+26, b_+28); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+28, b_+29); ret_effect(gb); return; } // ret c
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); A = 0x04;
  CALL_C(b_+31, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+34);
  CALL_C(b_+34, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+37);
  if (!(F & FZ)) { CYCT(b_+37, b_+38); ret_effect(gb); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x35); // INTERAC_CHILD
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+44); A = mem_rd(gb, wChildStage);
  CYC(b_+44, b_+46); B = 0x00;
  CYC(b_+46, b_+48); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(b_+48, b_+50); goto l_5dff; } // jr c
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); B = 0x03;

l_5dff:
  CYC(b_+52, b_+55); A = mem_rd(gb, wChildPersonality);
  CYC(b_+55, b_+56); alu_add(gb, B);
  CYC(b_+56, b_+57); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [child.subid]
  CYC(b_+57, b_+59); alu_add(gb, 0x16);
  CYC(b_+59, b_+60); mem_wr(gb, HL, A);
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x38);
  CYC(b_+64, b_+65); L = alu_inc8(gb, L);
  CYC(b_+65, b_+66); L = alu_inc8(gb, L);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x28);
  RET(b_+68); return;

state1:
  CYC(b_+69, b_+71); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+73); push_effect(gb, b_+73);
  do { uint16_t jt_ = (interactionCodea7_jump_table(gb));
    if (jt_ == b_+79) { goto substate0; }
    else if (jt_ == b_+97) { goto substate1; }
    else if (jt_ == b_+111) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+79, b_+82); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+82, b_+83); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+83, b_+85); goto l_5e29; } // jr z
  CYC(b_+83, b_+85);
  CALL_C(b_+85, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+88);
  CYC(b_+88, b_+91); SET_BC(0xff00); // -$100
  CALL_C(b_+91, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+94);

l_5e29:
  CYC(b_+94, b_+97); interactionAnimate_hook(gb); return; // jp

substate1:
  CYC(b_+97, b_+99); C = 0x20;
  CALL_C(b_+99, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; } // ret nz
  CYC(b_+102, b_+103);
  CALL_C(b_+103, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+106);
  CYC(b_+106, b_+108); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x0a);
  RET(b_+110); return;

substate2:
  CALL_C(b_+111, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+114);
  if (!(F & FZ)) { CYCT(b_+114, b_+115); ret_effect(gb); return; } // ret nz
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+117); A = 0x03;
  CYC(b_+117, SYM(interactionCodea8)); interactionSetAnimation_hook(gb); return; // jp
}
