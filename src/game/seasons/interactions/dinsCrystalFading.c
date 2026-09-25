#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/dinsCrystalFading.s.
// INTERAC_DINS_CRYSTAL_FADING

static uint16_t dinsCrystalFading_jump_table(GB *gb) {
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

// INTERAC_DINS_CRYSTAL_FADING
void s_interactionCodea6_hook(GB *gb) {
  BASE(interactionCodea6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dinsCrystalFading_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+37) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); alu_add(gb, A);
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+21); alu_add(gb, A);
  CYC(b_+21, b_+23); alu_add(gb, 0x04);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x64);
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x08);
  CYC(b_+34, b_+37);
  TAIL(objectSetVisible81);
state1:
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto substate1; }
  CYC(b_+41, b_+43);
  CALL_C(b_+43, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+46);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto applySpeedTwice; }
  CYC(b_+46, b_+48);
  CALL_C(b_+48, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+51);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x14);
applySpeedTwice:
  CALL_C(b_+55, s_objectApplySpeed, SYM(objectApplySpeed), b_+58);
  CYC(b_+58, b_+61);
  TAIL(objectApplySpeed);
substate1:
  CALL_C(b_+61, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+64);
  if (F & FZ) { CYCT(b_+64, b_+67); TAIL(interactionDelete); }
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+70); A = mem_rd(gb, wFrameCounter);
  CYC(b_+70, b_+71); alu_xor(gb, D);
  CYC(b_+71, b_+72); alu_rrca(gb);
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+74, b_+76); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto applySpeedTwice; }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+80, b_+82);
  goto applySpeedTwice;
}

