#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode2d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode2d), (from), (to), true)

// PART_VERAN_FAIRY_PROJECTILE
void partCode2d_hook(GB *gb) {
  BASE(partCode2d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); goto notNormalStatus; } // jr nz
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); A = 0x29; // Object.relatedObj1
  CALL_C(b_+4, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto noRelatedObj; } // jr z
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = 0xc4; // Part.state
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+15, b_+17); goto state0; } // jr z
  CYC(b_+15, b_+17);
  CALL_C(b_+17, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+22); goto notNormalStatus; } // jr z
  CYC(b_+20, b_+22);
  CALL_C(b_+22, objectApplySpeed_hook, SYM(objectApplySpeed), b_+25);
  CYC(b_+25, b_+28); partAnimate_hook(gb); return; // jp

state0:
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+30); L = E;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+33); L = 0xd0; // Part.speed
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+35, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+38);
  CYC(b_+38, b_+40); E = 0xc9; // Part.angle
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CALL_C(b_+41, objectSetVisible82_hook, SYM(objectSetVisible82), b_+44);
  CYC(b_+44, b_+46); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CYC(b_+46, b_+49); playSound_b00_hook(gb); return; // jp

noRelatedObj:
  CALL_C(b_+49, objectCreatePuff_hook, SYM(objectCreatePuff), b_+52);

notNormalStatus:
  CYC(b_+52, SYM(partCode2e)); partDelete_hook(gb); return; // jp
}
