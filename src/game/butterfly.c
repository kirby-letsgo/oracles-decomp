#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

static uint16_t butterfly_jump_table(GB *gb) {
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
// ENEMY_BUTTERFLY
// ==================================================================================================
void enemyCode37_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fd9, 0x4fdb); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4fdb, 0x4fdc); A = mem_rd(gb, DE);
  {
    CYC(0x4fdc, 0x4fdd); push_effect(gb, 0x4fdd);
    uint16_t target = butterfly_jump_table(gb);
    if (target == 0x4fe1) goto state0;
    if (target == 0x4fee) goto state1;
    HANDOFF(target);
  }

state0:
  CYC(0x4fe1, 0x4fe2); H = D;
  CYC(0x4fe2, 0x4fe3); L = E;
  CYC(0x4fe3, 0x4fe4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x4fe4, 0x4fe6); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x4fe6, 0x4fe8); mem_wr(gb, HL, 0x0a); // SPEED_40
  CALL_C(0x4fe8, ecom_setRandomAngle_b0e_hook, 0x43cf, 0x4feb);
  CYC(0x4feb, 0x4fee); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(0x4fee, 0x4ff1); SET_BC(0x1f1f);
  CALL_C(0x4ff1, ecom_randomBitwiseAndBCE_b0e_hook, 0x434f, 0x4ff4);
  CYC(0x4ff4, 0x4ff5); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x4ff5, 0x4ff7); goto L_4ffb; } // jr nz
  CYC(0x4ff5, 0x4ff7);
  CYC(0x4ff7, 0x4ff8); H = D;
  CYC(0x4ff8, 0x4ffa); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x4ffa, 0x4ffb); mem_wr(gb, HL, C);

L_4ffb:
  CALL_C(0x4ffb, objectApplySpeed_hook, 0x201d, 0x4ffe);
  CALL_C(0x4ffe, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x5001);
  CYC(0x5001, 0x5004); enemyAnimate_hook(gb); return; // jp
}
