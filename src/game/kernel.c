#include "game/asm.h"
#include "game/gen.h"

void setCpuToDoubleSpeed(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  I(0x4071, 3); A = mem_rd(gb, 0xff4d);
  I(0x4073, 1); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x4074); return; } I(0x4074, 2);
  I(0x4075, 1); alu_xor(gb, A);
  I(0x4076, 3); mem_wr(gb, 0xff0f, A);
  I(0x4078, 3); mem_wr(gb, 0xffff, A);
  I(0x407a, 2); A = 0x01;
  I(0x407c, 3); mem_wr(gb, 0xff4d, A);
  I(0x407e, 2); A = 0x30;
  I(0x4080, 3); mem_wr(gb, 0xff00, A);
  if (!gb->speed_armed) HANDOFF(0x4082);
  I(0x4082, 1); I(0x4083, 1); gb_speed_switch(gb);
L_4084:
  I(0x4084, 3); A = mem_rd(gb, 0xff4d);
  I(0x4086, 1); alu_rlca(gb);
  if (!(F & FC)) { I(0x4087, 3); goto L_4084; } I(0x4087, 2);
  I(0x4089, 1); alu_xor(gb, A);
  I(0x408a, 3); mem_wr(gb, 0xff00, A);
  I(0x408c, 3); mem_wr(gb, 0xff0f, A);
  I(0x408e, 3); mem_wr(gb, 0xffff, A);
  RET(0x4090); return;
}

void romEntry_hook(GB *gb) {
  I(0x0100, 1);
  I(0x0101, 4);
  begin_hook(gb);
}
