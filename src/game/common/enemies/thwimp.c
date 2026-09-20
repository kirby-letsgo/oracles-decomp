#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode2e_jump_table(GB *gb) {
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

// enemyCode2e@state8: an @-local reached both by the jump table below (tail transfer) and by a
// genuine `call` from @stateB (which pushes its own return address first). Declared as a bare
// _hook (not static) only because it needs CALL_C/sp0_ internally -- never registered on its
// own, since registering enemyCode2e collapses this whole routine's @-locals out of the
// generated fallback (rule 6).
void enemyCode2e_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x66af, ecom_decCounter1_b0d_hook, 0x439a, 0x66b2);
  if (!(F & FZ)) { CYCT(0x66b2, 0x66b3); ret_effect(gb); return; } // ret nz
  CYC(0x66b2, 0x66b3);
  CYC(0x66b3, 0x66b4); L = E;
  CYC(0x66b4, 0x66b5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x66b5, 0x66b6); alu_xor(gb, A);
  RET(0x66b6); return;
}

// ==================================================================================================
// ENEMY_THWIMP
//
// Variables:
//   var30: Original y-position (where it returns to after stomping)
//
// All states below (aside from @state8 above) are @-local to enemyCode2e in the disassembly (no
// separate top-level symbols), so they live inline in this one hook.
// ==================================================================================================
void enemyCode2e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x667a, 0x667c); goto normalStatus; } // jr z
  CYC(0x667a, 0x667c);
  CYC(0x667c, 0x667e); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x667e, 0x667f); ret_effect(gb); return; } // ret c
  CYC(0x667e, 0x667f);

normalStatus:
  CYC(0x667f, 0x6681); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6681, 0x6682); A = mem_rd(gb, DE);
  CYC(0x6682, 0x6683); push_effect(gb, 0x6683);
  {
    uint16_t target = enemyCode2e_jump_table(gb);
    if (target == 0x66af) { enemyCode2e_state8_hook(gb); return; }
    if (target == 0x66b7) goto state9;
    if (target == 0x66cd) goto stateA;
    if (target == 0x66e7) goto stateB;
    if (target == 0x66ee) goto stateC;
    if (target == 0x669d) goto state_uninitialized;
    RET(0x66ae); return; // states 1-7 all target 0x66ae, a bare `ret`
  }

state_uninitialized:
  CYC(0x669d, 0x669f); E = ENEMY_BASE + OBJ_YH;
  CYC(0x669f, 0x66a0); A = mem_rd(gb, DE);
  CYC(0x66a0, 0x66a2); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x66a2, 0x66a3); mem_wr(gb, DE, A);
  CYC(0x66a3, 0x66a4); H = D;
  CYC(0x66a4, 0x66a6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x66a6, 0x66a7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x66a7, 0x66a9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x66a9, 0x66ab); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x66ab, 0x66ae); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

  // Waiting for Link to approach
state9:
  CYC(0x66b7, 0x66b8); H = D;
  CYC(0x66b8, 0x66ba); L = ENEMY_BASE + OBJ_XH;
  CYC(0x66ba, 0x66bc); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x66bc, 0x66bd); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x66bd, 0x66bf); alu_add(gb, 0x0a);
  CYC(0x66bf, 0x66c1); alu_cp(gb, 0x15);
  if (!(F & FC)) { CYCT(0x66c1, 0x66c2); ret_effect(gb); return; } // ret nc
  CYC(0x66c1, 0x66c2);
  CYC(0x66c2, 0x66c3); L = E;
  CYC(0x66c3, 0x66c4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x66c4, 0x66c6); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x66c6, 0x66c7); alu_xor(gb, A);
  CYC(0x66c7, 0x66c8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x66c8, 0x66c9); mem_wr(gb, HL, A);
  CYC(0x66c9, 0x66ca); A = alu_inc8(gb, A);
  CYC(0x66ca, 0x66cd); enemySetAnimation_hook(gb); return; // jp

  // Falling down
stateA:
  CYC(0x66cd, 0x66cf); A = 0x40;
  CALL_C(0x66cf, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x66d2);
  if (F & FC) { CYCT(0x66d2, 0x66d4); goto landed; } // jr c
  CYC(0x66d2, 0x66d4);
  // Cap speedZ to $0200 (ish... doesn't fix the low byte)
  CYC(0x66d4, 0x66d5); A = mem_rd(gb, HL);
  CYC(0x66d5, 0x66d7); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x66d7, 0x66d8); ret_effect(gb); return; } // ret c
  CYC(0x66d7, 0x66d8);
  CYC(0x66d8, 0x66da); mem_wr(gb, HL, 0x02);
  RET(0x66da); return;

landed:
  CALL_C(0x66db, ecom_incState_b0d_hook, 0x4000, 0x66de);
  CYC(0x66de, 0x66e0); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x66e0, 0x66e2); mem_wr(gb, HL, 45);
  CYC(0x66e2, 0x66e4); A = 0x50; // SND_CLINK
  CYC(0x66e4, 0x66e7); playSound_b00_hook(gb); return; // jp

  // Just landed. Wait for [counter1] frames
stateB:
  CYC(0x66e7, 0x66ea); push_effect(gb, 0x66ea); enemyCode2e_state8_hook(gb);
  if (!(F & FZ)) { CYCT(0x66ea, 0x66eb); ret_effect(gb); return; } // ret nz
  CYC(0x66ea, 0x66eb);
  CYC(0x66eb, 0x66ee); enemySetAnimation_hook(gb); return; // jp

  // Moving back up at constant speed
stateC:
  CYC(0x66ee, 0x66ef); H = D;
  CYC(0x66ef, 0x66f1); L = ENEMY_BASE + OBJ_Y;
  CYC(0x66f1, 0x66f2); A = mem_rd(gb, HL);
  CYC(0x66f2, 0x66f4); alu_sub(gb, 0x80);
  CYC(0x66f4, 0x66f5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x66f5, 0x66f6); A = mem_rd(gb, HL);
  CYC(0x66f6, 0x66f8); alu_sbc(gb, 0x00);
  CYC(0x66f8, 0x66f9); mem_wr(gb, HL, A);
  CYC(0x66f9, 0x66fb); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x66fb, 0x66fc); A = mem_rd(gb, DE);
  CYC(0x66fc, 0x66fd); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x66fd, 0x66fe); ret_effect(gb); return; } // ret nz
  CYC(0x66fd, 0x66fe);
  CYC(0x66fe, 0x6700); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6700, 0x6702); mem_wr(gb, HL, 24);
  CYC(0x6702, 0x6704); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6704, 0x6706); mem_wr(gb, HL, 0x08);
  RET(0x6706); return;
}
