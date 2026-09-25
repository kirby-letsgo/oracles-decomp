#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/unicornsCaveFallingMagnetBall.s.
// INTERAC_D5_FALLING_MAGNET_BALL

static uint16_t unicornsCaveFallingMagnetBall_jump_table(GB *gb) {
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

// INTERAC_D5_FALLING_MAGNET_BALL
void s_interactionCode64_hook(GB *gb) {
  BASE(interactionCode64);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (unicornsCaveFallingMagnetBall_jump_table(gb));
    if (jt_ == b_+16) goto state0;
    if (jt_ == b_+46) goto state1;
    if (jt_ == b_+70) goto state2;
    if (jt_ == b_+77) goto state3;
    if (jt_ == b_+88) goto state4;
    if (jt_ == SYM(interactionDelete) && hook_is(gb, SYM(interactionDelete), s_interactionDelete)) { s_interactionDelete(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+16, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+19);
  CALL_C(b_+19, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+22);
  CYC(b_+22, b_+24); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto createBall; }
  CYC(b_+24, b_+26);
  CALL_C(b_+26, s_objectGetZAboveScreen, SYM(objectGetZAboveScreen), b_+29);
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x01);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  RET(b_+37); return;
createBall:
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+40, b_+42); A = 0x04;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+46);
  TAIL(objectSetVisiblec2);
state1:
  CALL_C(b_+46, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+49);
  CYC(b_+49, b_+51); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+54, b_+56); A = 0x02;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+59, b_+61); A = 0x1e;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+64); A = 0x4d;
  CALL_C(b_+64, s_playSound, SYM(playSound_b00), b_+67);
  CYC(b_+67, b_+70);
  TAIL(objectSetVisiblec1);
state2:
  CALL_C(b_+70, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+73);
  if (!(F & FZ)) { RET_TAKEN(b_+73); return; }
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+76); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+76, b_+77); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state3:
  CYC(b_+77, b_+79); C = 0x10;
  CALL_C(b_+79, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+82);
  if (!(F & FC)) { RET_TAKEN(b_+82); return; }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+85); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+85, b_+87); A = 0x04;
  CYC(b_+87, b_+88); mem_wr(gb, DE, A);
state4:
  CYC(b_+88, b_+91); SET_HL(w1MagnetBall);
  CYC(b_+91, b_+92); A = mem_rd(gb, HL);
  CYC(b_+92, b_+93); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+93); return; }
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x01);
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x29);
  CALL_C(b_+99, s_objectCopyPosition, SYM(objectCopyPosition), b_+102);
  CYC(b_+102, b_+104); E = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+104, b_+106); L = 0x16;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+108, b_+109); E = alu_inc8(gb, E);
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+111); mem_wr(gb, HL, A);
  CYC(b_+111, b_+113); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+113, b_+115); A = 0x05;
  CYC(b_+115, b_+116); mem_wr(gb, DE, A);
  RET(b_+116); return;
}

