#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

// PART_VERAN_FAIRY_PROJECTILE
void partCode2d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x62a7, 0x62a9); goto notNormalStatus; } // jr nz
  CYC(0x62a7, 0x62a9);
  CYC(0x62a9, 0x62ab); A = 0x29; // Object.relatedObj1
  CALL_C(0x62ab, objectGetRelatedObject1Var_hook, 0x2160, 0x62ae);
  CYC(0x62ae, 0x62af); A = mem_rd(gb, HL);
  CYC(0x62af, 0x62b0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x62b0, 0x62b2); goto noRelatedObj; } // jr z
  CYC(0x62b0, 0x62b2);
  CYC(0x62b2, 0x62b4); E = 0xc4; // Part.state
  CYC(0x62b4, 0x62b5); A = mem_rd(gb, DE);
  CYC(0x62b5, 0x62b6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x62b6, 0x62b8); goto state0; } // jr z
  CYC(0x62b6, 0x62b8);
  CALL_C(0x62b8, partCommon_checkOutOfBounds_hook, 0x407e, 0x62bb);
  if (F & FZ) { CYCT(0x62bb, 0x62bd); goto notNormalStatus; } // jr z
  CYC(0x62bb, 0x62bd);
  CALL_C(0x62bd, objectApplySpeed_hook, 0x201d, 0x62c0);
  CYC(0x62c0, 0x62c3); partAnimate_hook(gb); return; // jp

state0:
  CYC(0x62c3, 0x62c4); H = D;
  CYC(0x62c4, 0x62c5); L = E;
  CYC(0x62c5, 0x62c6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x62c6, 0x62c8); L = 0xd0; // Part.speed
  CYC(0x62c8, 0x62ca); mem_wr(gb, HL, 0x3c);
  CALL_C(0x62ca, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x62cd);
  CYC(0x62cd, 0x62cf); E = 0xc9; // Part.angle
  CYC(0x62cf, 0x62d0); mem_wr(gb, DE, A);
  CALL_C(0x62d0, objectSetVisible82_hook, 0x1e69, 0x62d3);
  CYC(0x62d3, 0x62d5); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CYC(0x62d5, 0x62d8); playSound_b00_hook(gb); return; // jp

noRelatedObj:
  CALL_C(0x62d8, objectCreatePuff_hook, 0x24c1, 0x62db);

notNormalStatus:
  CYC(0x62db, 0x62de); partDelete_hook(gb); return; // jp
}
