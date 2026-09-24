#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/talon.s.
// INTERAC_TALON

static uint16_t talon_jump_table(GB *gb) {
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

// INTERAC_TALON
void s_interactionCode45_hook(GB *gb) {
  BASE(interactionCode45);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (talon_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+81) goto state1;
    if (jt_ == b_+124) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+10, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+13);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto state0_subid1; }
  CYC(b_+17, b_+19);
  CALL_C(b_+19, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+22);
  CYC(b_+22, b_+24); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); TAIL(interactionDelete); }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); H = D;
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x01);
  CYC(b_+36, b_+38); A = 0x0b;
  CALL_C(b_+38, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+41);
  CYC(b_+41, b_+44); SET_HL(SYM(caveTalonScript));
  CALL_C(b_+44, s_interactionSetScript, SYM(interactionSetScript), b_+47);
  CYC(b_+47, b_+49); A = 0x03;
  CALL_C(b_+49, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+52);
  CYC(b_+52, b_+55);
  TAIL(interactionAnimateAsNpc);
state0_subid1:
  CYC(b_+55, b_+56); H = D;
  CYC(b_+56, b_+58); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x02);
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0xff);
  CALL_C(b_+64, s_func_6f3c_hook, SYM(func_6f3c), b_+67);
  CYC(b_+67, b_+69); A = 0x0b;
  CALL_C(b_+69, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+72);
  CYC(b_+72, b_+75); SET_HL(SYM(returnedTalonScript));
  CALL_C(b_+75, s_interactionSetScript, SYM(interactionSetScript), b_+78);
  CYC(b_+78, b_+81);
  TAIL(interactionAnimateAsNpc);
state1:
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+85, b_+87); goto L_6f1e; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+90); A = mem_rd(gb, wFrameCounter);
  CYC(b_+90, b_+92); alu_and(gb, 0x3f);
  if (!(F & FZ)) { CYCT(b_+92, b_+94); goto L_6f1e; }
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+96); A = 0x01;
  CYC(b_+96, b_+98); B = 0xfa;
  CYC(b_+98, b_+100); C = 0x0a;
  CALL_C(b_+100, s_objectCreateFloatingSnore, SYM(objectCreateFloatingSnore), b_+103);
L_6f1e:
  CALL_C(b_+103, s_interactionRunScript, SYM(interactionRunScript), b_+106);
  if (F & FC) { CYCT(b_+106, b_+109); TAIL(interactionDelete); }
  CYC(b_+106, b_+109);
  CALL_C(b_+109, s_interactionAnimate, SYM(interactionAnimate), b_+112);
  CYC(b_+112, b_+114); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+114, b_+115); A = mem_rd(gb, DE);
  CYC(b_+115, b_+116); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+116, b_+118); goto L_6f30; }
  CYC(b_+116, b_+118);
  CALL_C(b_+118, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+121);
L_6f30:
  CYC(b_+121, b_+124);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
state2:
  CALL_C(b_+124, s_interactionRunScript, SYM(interactionRunScript), b_+127);
  CALL_C(b_+127, s_func_6f3c_hook, SYM(func_6f3c), b_+130);
  CYC(b_+130, b_+133);
  TAIL(interactionAnimateAsNpc);
}

void s_func_6f3c_hook(GB *gb) {
  BASE(func_6f3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x28;
  CALL_C(b_+2, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto L_6f4d; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_cp(gb, 0x06);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); A = 0x06;
  CYC(b_+15, b_+17);
  goto L_6f55;
L_6f4d:
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_cp(gb, 0x05);
  if (F & FZ) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); A = 0x05;
L_6f55:
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+29);
  TAIL(interactionSetAnimation);
}

