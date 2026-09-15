#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

// PART_HEAD_THWOMP_CIRCULAR_PROJECTILE
void partCode3c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(0x6f2c, 0x6f2f); partDelete_hook(gb); return; } // jp nz
  CYC(0x6f2c, 0x6f2f);
  CYC(0x6f2f, 0x6f31); E = 0xc4; // Part.state
  CYC(0x6f31, 0x6f32); A = mem_rd(gb, DE);
  CYC(0x6f32, 0x6f33); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6f33, 0x6f35); goto state0; } // jr z
  CYC(0x6f33, 0x6f35);
  CALL_C(0x6f35, partCommon_checkOutOfBounds_hook, 0x407e, 0x6f38);
  if (F & FZ) { CYCT(0x6f38, 0x6f3b); partDelete_hook(gb); return; } // jp z
  CYC(0x6f38, 0x6f3b);
  CALL_C(0x6f3b, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6f3e);
  if (!(F & FZ)) { CYCT(0x6f3e, 0x6f40); goto counter1NonZero; } // jr nz
  CYC(0x6f3e, 0x6f40);
  CYC(0x6f40, 0x6f41); L = alu_inc8(gb, L);
  CYC(0x6f41, 0x6f43); E = 0xf0; // Part.var30
  CYC(0x6f43, 0x6f44); A = mem_rd(gb, DE);
  CYC(0x6f44, 0x6f45); A = alu_inc8(gb, A);
  CYC(0x6f45, 0x6f47); alu_and(gb, 0x01);
  CYC(0x6f47, 0x6f48); mem_wr(gb, DE, A);
  CYC(0x6f48, 0x6f49); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6f49, 0x6f4a); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x6f4a, 0x6f4b); mem_wr(gb, HL, A);
  CYC(0x6f4b, 0x6f4d); L = 0xc9; // Part.angle
  CYC(0x6f4d, 0x6f4f); E = 0xc2; // Part.subid
  CYC(0x6f4f, 0x6f50); A = mem_rd(gb, DE);
  CYC(0x6f50, 0x6f51); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6f51, 0x6f53); alu_and(gb, 0x1f);
  CYC(0x6f53, 0x6f54); mem_wr(gb, HL, A);

counter1NonZero:
  CALL_C(0x6f54, objectApplySpeed_hook, 0x201d, 0x6f57);
  CYC(0x6f57, 0x6f5a); partAnimate_hook(gb); return; // jp

state0:
  CYC(0x6f5a, 0x6f5b); H = D;
  CYC(0x6f5b, 0x6f5c); L = E;
  CYC(0x6f5c, 0x6f5d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f5d, 0x6f5f); L = 0xc6; // Part.counter1
  CYC(0x6f5f, 0x6f61); A = 0x02;
  CYC(0x6f61, 0x6f62); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6f62, 0x6f63); mem_wr(gb, HL, A);
  CYC(0x6f63, 0x6f65); L = 0xd0; // Part.speed
  CYC(0x6f65, 0x6f67); mem_wr(gb, HL, 0x64); // SPEED_280
  CALL_C(0x6f67, objectSetVisible82_hook, 0x1e69, 0x6f6a);
  CYC(0x6f6a, 0x6f6c); A = 0xa4; // SND_BEAM
  CYC(0x6f6c, 0x6f6f); playSound_b00_hook(gb); return; // jp
}
