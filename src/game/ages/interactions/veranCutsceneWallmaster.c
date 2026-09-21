#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t veranCutsceneWallmaster_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// INTERAC_VERAN_CUTSCENE_WALLMASTER: the wallmaster that grabs Link in the Veran cutscene.
void interactionCode2c_hook(GB *gb) {
  BASE(interactionCode2c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (veranCutsceneWallmaster_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+31) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+17); SET_BC(0x0140);
  CALL_C(b_+17, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+20);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x14);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xa0);
  CALL_C(b_+28, objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+31);
  // falls through into state 1

state1:
  CALL_C(b_+31, interactionAnimate_hook, SYM(interactionAnimate), b_+34);
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); push_effect(gb, b_+38);
  do { uint16_t jt_ = (veranCutsceneWallmaster_jumpTable(gb));
    if (jt_ == b_+46) { goto substate0; }
    else if (jt_ == b_+53) { goto substate1; }
    else if (jt_ == b_+67) { goto substate2; }
    else if (jt_ == b_+91) { goto substate3; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+46, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+49);
  if (!(F & FZ)) {
    CYCT(b_+49, b_+50); ret_effect(gb); return;
  }
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+53); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+53, b_+55); C = 0x00;
  CALL_C(b_+55, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+58);
  if (!(F & FZ)) {
    CYCT(b_+58, b_+59); ret_effect(gb); return;
  }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+61); A = 0x01;
  CALL_C(b_+61, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+64);
  CYC(b_+64, b_+67); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+72); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+75); TAIL(interactionIncSubstate);
  }
  CYC(b_+72, b_+75);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+76, b_+77); ret_effect(gb); return;
  }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+78); alu_xor(gb, A);
  CYC(b_+78, b_+81); W8(w1Link_visible) = A;
  CYC(b_+81, b_+83); A = 0x1e;
  CYC(b_+83, b_+85); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+88); A = 0x67; // SND_BOSS_DEAD
  CYC(b_+88, b_+91); playSound_b00_hook(gb);
  return;

substate3:
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+95); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+95, b_+97); goto counterDone;
  }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+98); A = alu_dec8(gb, A);
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+100); ret_effect(gb);
  return;

counterDone:
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+104); A = alu_dec8(gb, A);
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CYC(b_+105, b_+107); alu_cp(gb, 0xb0);
  if (!(F & FZ)) {
    CYCT(b_+107, b_+108); ret_effect(gb); return;
  }
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+110); A = 0x08;
  CYC(b_+110, b_+113); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+113, b_+116); TAIL(interactionDelete);
}
