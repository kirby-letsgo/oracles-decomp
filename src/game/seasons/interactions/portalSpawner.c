#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/portalSpawner.s.
// INTERAC_PORTAL_SPAWNER

static uint16_t portalSpawner_jump_table(GB *gb) {
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

// INTERAC_PORTAL_SPAWNER
void s_interactionCodee1_hook(GB *gb) {
  BASE(interactionCodee1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (portalSpawner_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+19) goto state1;
    if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), s_interactionAnimate)) { s_interactionAnimate(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+16);
  CALL_C(b_+16, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+19);
state1:
  CYC(b_+19, b_+22); A = mem_rd(gb, wActiveGroup);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_6dfb; }
  CYC(b_+23, b_+25);
  CALL_C(b_+25, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 0xe6);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
L_6dfb:
  CALL_C(b_+31, s_func_6e06_hook, SYM(func_6e06), b_+34);
  CYC(b_+34, b_+36); A = 0x02;
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+42);
  TAIL(objectSetVisible83);
}

void s_func_6e06_hook(GB *gb) {
  BASE(func_6e06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wActiveGroup);
  CYC(b_+11, b_+13); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+13, b_+15); goto L_6e25; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, wActiveRoom);
  CYC(b_+21, b_+23); alu_cp(gb, 0xa8);
  CYC(b_+23, b_+26); SET_HL(wGroup0RoomFlags + 0x04);
  if (F & FZ) { CYCT(b_+26, b_+28); goto L_6e25; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+31); SET_HL(wGroup0RoomFlags + 0xf7);
L_6e25:
  CYC(b_+31, b_+33); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  RET(b_+33); return;
}

