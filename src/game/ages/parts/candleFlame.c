#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCode36);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01; // Object.id
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x55); // ENEMY_CANDLE
  if (!(F & FZ)) { CYCT(b_+8, b_+11); partDelete_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); B = H;
  CYC(b_+12, b_+14); E = 0xc4; // Part.state
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  {
    CYC(b_+15, b_+16); push_effect(gb, b_+16);
    uint16_t target = candleFlame_jump_table(gb);
    if (target == b_+28) goto state1;
    if (target == b_+45) goto state2;
    goto state0;
  }

state0:
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+24); L = E;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+25, objectSetVisible81_hook, SYM(objectSetVisible81), b_+28);

state1:
  CYC(b_+28, b_+29); H = B;
  CYC(b_+29, b_+31); L = 0x90; // Enemy.speed
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_cp(gb, 0x28); // SPEED_100
  if (F & FZ) { CYCT(b_+34, b_+36); goto state2; } // jr z
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); A = 0x02;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A); // Part.state
  CYC(b_+39, b_+40); push_effect(gb, BC);
  CYC(b_+40, b_+41); A = alu_dec8(gb, A);
  CALL_C(b_+41, partSetAnimation_hook, SYM(partSetAnimation), b_+44);
  CYC(b_+44, b_+45); SET_BC(pop_effect(gb));

state2:
  CYC(b_+45, b_+46); H = B;
  CYC(b_+46, b_+48); L = 0xa5; // Enemy.enemyCollisionMode
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); alu_cp(gb, 0x04); // ENEMYCOLLISION_PODOBOO
  if (F & FZ) { CYCT(b_+51, b_+54); partDelete_hook(gb); return; } // jp z
  CYC(b_+51, b_+54);
  CALL_C(b_+54, objectTakePosition_hook, SYM(objectTakePosition), b_+57);
  CYC(b_+57, b_+59); E = 0xcf; // Part.zh
  CYC(b_+59, b_+61); A = 0xf3;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+65); TAIL(partAnimate); // jp
}
