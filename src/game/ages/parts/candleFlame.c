#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t candleFlame_jump_table(GB *gb) {
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

// PART_CANDLE_FLAME
void partCode36_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x69ac, 0x69ae); A = 0x01; // Object.id
  CALL_C(0x69ae, objectGetRelatedObject1Var_hook, 0x2160, 0x69b1);
  CYC(0x69b1, 0x69b2); A = mem_rd(gb, HL);
  CYC(0x69b2, 0x69b4); alu_cp(gb, 0x55); // ENEMY_CANDLE
  if (!(F & FZ)) { CYCT(0x69b4, 0x69b7); partDelete_hook(gb); return; } // jp nz
  CYC(0x69b4, 0x69b7);
  CYC(0x69b7, 0x69b8); B = H;
  CYC(0x69b8, 0x69ba); E = 0xc4; // Part.state
  CYC(0x69ba, 0x69bb); A = mem_rd(gb, DE);
  {
    CYC(0x69bb, 0x69bc); push_effect(gb, 0x69bc);
    uint16_t target = candleFlame_jump_table(gb);
    if (target == 0x69c8) goto state1;
    if (target == 0x69d9) goto state2;
    goto state0;
  }

state0:
  CYC(0x69c2, 0x69c3); H = D;
  CYC(0x69c3, 0x69c4); L = E;
  CYC(0x69c4, 0x69c5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x69c5, objectSetVisible81_hook, 0x1e60, 0x69c8);

state1:
  CYC(0x69c8, 0x69c9); H = B;
  CYC(0x69c9, 0x69cb); L = 0x90; // Enemy.speed
  CYC(0x69cb, 0x69cc); A = mem_rd(gb, HL);
  CYC(0x69cc, 0x69ce); alu_cp(gb, 0x28); // SPEED_100
  if (F & FZ) { CYCT(0x69ce, 0x69d0); goto state2; } // jr z
  CYC(0x69ce, 0x69d0);
  CYC(0x69d0, 0x69d2); A = 0x02;
  CYC(0x69d2, 0x69d3); mem_wr(gb, DE, A); // Part.state
  CYC(0x69d3, 0x69d4); push_effect(gb, BC);
  CYC(0x69d4, 0x69d5); A = alu_dec8(gb, A);
  CALL_C(0x69d5, partSetAnimation_hook, 0x2988, 0x69d8);
  CYC(0x69d8, 0x69d9); SET_BC(pop_effect(gb));

state2:
  CYC(0x69d9, 0x69da); H = B;
  CYC(0x69da, 0x69dc); L = 0xa5; // Enemy.enemyCollisionMode
  CYC(0x69dc, 0x69dd); A = mem_rd(gb, HL);
  CYC(0x69dd, 0x69df); alu_cp(gb, 0x04); // ENEMYCOLLISION_PODOBOO
  if (F & FZ) { CYCT(0x69df, 0x69e2); partDelete_hook(gb); return; } // jp z
  CYC(0x69df, 0x69e2);
  CALL_C(0x69e2, objectTakePosition_hook, 0x2274, 0x69e5);
  CYC(0x69e5, 0x69e7); E = 0xcf; // Part.zh
  CYC(0x69e7, 0x69e9); A = 0xf3;
  CYC(0x69e9, 0x69ea); mem_wr(gb, DE, A);
  CYC(0x69ea, 0x69ed); partAnimate_hook(gb); return; // jp
}
