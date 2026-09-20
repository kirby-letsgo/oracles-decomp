#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f00, 0x6f02); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6f02, 0x6f03); A = mem_rd(gb, DE);
  CYC(0x6f03, 0x6f04); push_effect(gb, 0x6f04);
  switch (interactionCodee0_jump_table(gb)) {
    case 0x6f0c: goto state0;
    case 0x6f29: goto state1;
    case 0x6f3a: goto state2;
    case 0x6f45: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x6f0c, 0x6f0e); A = 0x01;
  CYC(0x6f0e, 0x6f0f); mem_wr(gb, DE, A);
  CYC(0x6f0f, 0x6f12); A = W8(wTilesetFlags);
  CYC(0x6f12, 0x6f14); alu_and(gb, 0x80);
  CYC(0x6f14, 0x6f15); alu_rlca(gb);
  CYC(0x6f15, 0x6f17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6f17, 0x6f18); mem_wr(gb, DE, A);
  CALL_C(0x6f18, interactionInitGraphics_hook, 0x15fb, 0x6f1b);
  CALL_C(0x6f1b, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6f1e);
  CYC(0x6f1e, 0x6f20); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6f20, 0x6f22); mem_wr(gb, HL, 0x0a);
  CYC(0x6f22, 0x6f24); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6f24, 0x6f26); mem_wr(gb, HL, 0xb0);
  CYC(0x6f26, 0x6f29);
  objectSetVisible80_hook(gb);
  return;
state1:
  CYC(0x6f29, 0x6f2a); H = D;
  CYC(0x6f2a, 0x6f2c); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6f2c, 0x6f2d); A = mem_rd(gb, HL);
  CYC(0x6f2d, 0x6f2f); alu_sub(gb, 0x04);
  CYC(0x6f2f, 0x6f30); mem_wr(gb, HL, A);
  CYC(0x6f30, 0x6f32); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(0x6f32); return; }
  CYC(0x6f32, 0x6f33);
  CYC(0x6f33, 0x6f34); L = E;
  CYC(0x6f34, 0x6f35); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f35, 0x6f37); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6f37, 0x6f39); mem_wr(gb, HL, 0x28);
  RET(0x6f39); return;
state2:
  CALL_C(0x6f3a, interactionDecCounter1_hook, 0x23cc, 0x6f3d);
  if (!(F & FZ)) { RET_TAKEN(0x6f3d); return; }
  CYC(0x6f3d, 0x6f3e);
  CYC(0x6f3e, 0x6f3f); L = E;
  CYC(0x6f3f, 0x6f40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f40, 0x6f42); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6f42, 0x6f44); mem_wr(gb, HL, 0x06);
  RET(0x6f44); return;
state3:
  CYC(0x6f45, 0x6f46); H = D;
  CYC(0x6f46, 0x6f48); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6f48, 0x6f49); A = mem_rd(gb, HL);
  CYC(0x6f49, 0x6f4b); alu_sub(gb, 0x06);
  CYC(0x6f4b, 0x6f4c); mem_wr(gb, HL, A);
  CYC(0x6f4c, 0x6f4e); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6f4e, 0x6f4f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x6f4f); return; }
  CYC(0x6f4f, 0x6f50);
  CYC(0x6f50, 0x6f53);
  interactionDelete_hook(gb);
}
