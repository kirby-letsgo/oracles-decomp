#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void func_5369_hook(GB *gb) {
  BASE(func_5369);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xd0; // Part.speed
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x50);
  CYC(b_+7, b_+9); E = 0xc9; // Part.angle
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+13); alu_rlca(gb);
  CALL_C(b_+13, partSetAnimation_hook, SYM(partSetAnimation), b_+16);
  CYC(b_+16, b_+19); TAIL(objectSetVisible81); // jp
}

void partCode1f_hook(GB *gb) {
  BASE(partCode1f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); goto normalStatus; } // jr nz
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xc4; // Part.state
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); func_5369_hook(gb); return; } // jr z
  CYC(b_+6, b_+8);
  CALL_C(b_+8, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+11);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto normalStatus; } // jr nc
  CYC(b_+11, b_+13);
  CALL_C(b_+13, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+19); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+16, b_+19);

normalStatus:
  CYC(b_+19, b_+22); TAIL(partDelete); // jp
}
