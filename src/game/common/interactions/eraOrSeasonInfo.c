#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/interactions/eraOrSeasonInfo.s (INTERAC_ERA_OR_SEASON_INFO), bank $10.

static uint16_t interactionCodee0_jump_table(GB *gb) {
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

void interactionCodee0_hook(GB *gb) {
  BASE(interactionCodee0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodee0_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+41) { goto state1; }
    else if (jt_ == b_+58) { goto state2; }
    else if (jt_ == b_+69) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18); A = W8(wTilesetFlags);
  CYC(b_+18, b_+20); alu_and(gb, 0x80);
  CYC(b_+20, b_+21); alu_rlca(gb);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CALL_C(b_+24, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+27);
  CALL_C(b_+27, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+30);
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x0a);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0xb0);
  CYC(b_+38, b_+41);
  TAIL(objectSetVisible80);
state1:
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+47); alu_sub(gb, 0x04);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+50); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); L = E;
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+53, b_+55); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x28);
  RET(b_+57); return;
state2:
  CALL_C(b_+58, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+61);
  if (!(F & FZ)) { RET_TAKEN(b_+61); return; }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+63); L = E;
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+64, b_+66); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x06);
  RET(b_+68); return;
state3:
  CYC(b_+69, b_+70); H = D;
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+75); alu_sub(gb, 0x06);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
  CYC(b_+76, b_+78); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+83);
  interactionDelete_hook(gb);
}
