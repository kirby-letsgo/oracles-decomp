#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/ad.s.
// INTERAC_ad

static uint16_t ad_jump_table(GB *gb) {
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

static void ad_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// INTERAC_ad
void s_interactionCodead_hook(GB *gb) {
  BASE(interactionCodead);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ad_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+51) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); SET_HL(SYM(script779e));
  CALL_C(b_+23, s_interactionSetScript, SYM(interactionSetScript), b_+26);
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+32); SET_HL(b_+46 /* @table_6a67 */);
  CYC(b_+32, b_+33); ad_add_a_to_hl(gb, b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+40); SET_BC(IO_LCDC);
  CALL_C(b_+40, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+43);
  CYC(b_+43, b_+46);
  TAIL(objectSetVisible82);

state1:
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+56); alu_and(gb, 0x01);
  if (!(F & FZ)) CALL_C_CC(b_+56, s_func_6ae7_hook, SYM(func_6ae7), b_+59);
  else CYC(b_+56, b_+59);
  CALL_C(b_+59, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+62);
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); push_effect(gb, b_+66);
  do { uint16_t jt_ = (ad_jump_table(gb));
    if (jt_ == b_+76) goto substate0;
    if (jt_ == b_+89) goto substate1;
    if (jt_ == b_+115) goto substate2;
    if (jt_ == b_+148) goto substate3;
    if (jt_ == b_+159) goto substate4;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+76, b_+79); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+79, b_+80); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+89);
  TAIL(interactionIncSubstate);
substate1:
  CYC(b_+89, b_+92); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+92, b_+94); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+94, b_+96); goto L_6aa2; }
  CYC(b_+94, b_+96);
  CALL_C(b_+96, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+99);
  CYC(b_+99, b_+102); SET_BC(0xfe80);
  CYC(b_+102, b_+105);
  TAIL(objectSetSpeedZ);
L_6aa2:
  CYC(b_+105, b_+107); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+109); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+109, s_interactionAnimate, SYM(interactionAnimate), b_+112);
  else CYC(b_+109, b_+112);
  CYC(b_+112, b_+115);
  TAIL(interactionRunScript);
substate2:
  CYC(b_+115, b_+117); C = 0x20;
  CALL_C(b_+117, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+120);
  if (!(F & FZ)) { RET_TAKEN(b_+120); return; }
  CYC(b_+120, b_+121);
  CALL_C(b_+121, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+124);
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+126, b_+128); mem_wr(gb, HL, 0x08);
  CYC(b_+128, b_+130); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+130, b_+131); A = mem_rd(gb, HL);
  CYC(b_+131, b_+133); alu_add(gb, 0x10);
  CYC(b_+133, b_+135); alu_and(gb, 0x1f);
  CYC(b_+135, b_+136); mem_wr(gb, HL, A);
  CYC(b_+136, b_+138); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x50);
  CYC(b_+140, b_+142); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+142, b_+143); A = mem_rd(gb, HL);
  CYC(b_+143, b_+144); alu_add(gb, A);
  CYC(b_+144, b_+145); A = alu_inc8(gb, A);
  CYC(b_+145, b_+148);
  TAIL(interactionSetAnimation);
substate3:
  CALL_C(b_+148, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+151);
  if (!(F & FZ)) { RET_TAKEN(b_+151); return; }
  CYC(b_+151, b_+152);
  CYC(b_+152, b_+154); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x40);
  CYC(b_+156, b_+159);
  TAIL(interactionIncSubstate);
substate4:
  CALL_C(b_+159, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+162);
  if (F & FZ) { CYCT(b_+162, b_+165); TAIL(interactionDelete); }
  CYC(b_+162, b_+165);
  CALL_C(b_+165, s_objectApplySpeed, SYM(objectApplySpeed), b_+168);
  CALL_C(b_+168, s_interactionAnimate, SYM(interactionAnimate), b_+171);
  CYC(b_+171, b_+174);
  TAIL(interactionAnimateAsNpc);
}

void s_func_6ae7_hook(GB *gb) {
  BASE(func_6ae7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_BC(IO_LCDC);
  CYC(b_+9, b_+12);
  TAIL(objectSetSpeedZ);
}

