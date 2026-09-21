#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t bomb_jump_table(GB *gb) {
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

// PART_BOMB
void partCode47_hook(GB *gb) {
  BASE(partCode47);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bomb_jump_table(gb);
    if (target == b_+29) goto state1;
    if (target == b_+37) goto state2;
    if (target == b_+76) goto state3;
    goto state0;
  }

state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = 0xd0; // Part.speed
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+19, b_+21); L = 0xd4; // Part.speedZ
  CYC(b_+21, b_+23); A = 0x80;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xfd);
  CALL_C(b_+26, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+29);

state1:
  CYC(b_+29, b_+31); A = 0x00; // Object.id
  CALL_C(b_+31, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+34);
  CYC(b_+34, b_+37); TAIL(objectTakePosition); // jp

state2:
  CALL_C(b_+37, objectApplySpeed_hook, SYM(objectApplySpeed), b_+40);
  CYC(b_+40, b_+42); C = 0x20;
  CALL_C(b_+42, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+48); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+45, b_+48);
  CYC(b_+48, b_+50); L = 0xc4; // Part.state
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+51, b_+53); L = 0xe4; // Part.collisionType
  CYC(b_+53, b_+55); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+55, b_+57); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+57, b_+59); A = 0x0a;
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+60, b_+61); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x0c); // oamTileIndexBase
  CYC(b_+63, b_+65); A = 0x01;
  CALL_C(b_+65, partSetAnimation_hook, SYM(partSetAnimation), b_+68);
  CYC(b_+68, b_+70); A = 0x6f; // SND_EXPLOSION
  CALL_C(b_+70, playSound_b00_hook, SYM(playSound_b00), b_+73);
  CYC(b_+73, b_+76); TAIL(objectSetVisible83); // jp

state3:
  CALL_C(b_+76, partAnimate_hook, SYM(partAnimate), b_+79);
  CYC(b_+79, b_+81); E = 0xe1; // Part.animParameter
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+83, b_+86); partDelete_hook(gb); return; } // jp z
  CYC(b_+83, b_+86);
  CYC(b_+86, b_+87); A = alu_dec8(gb, A);
  CYC(b_+87, b_+89); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+89, b_+90); mem_wr(gb, DE, A);
  CYC(b_+90, b_+91); E = alu_inc8(gb, E);
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  RET(b_+92); return; // ret
}
