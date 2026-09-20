#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t stalfosBone_jump_table(GB *gb) {
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

void partCode1c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5190, 0x5192); goto normalStatus; } // jr z
  CYC(0x5190, 0x5192);
  CYC(0x5192, 0x5194); E = 0xea; // Part.var2a
  CYC(0x5194, 0x5195); A = mem_rd(gb, DE);
  CYC(0x5195, 0x5197); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x5197, 0x5199); goto partDelete; } // jr z
  CYC(0x5197, 0x5199);
  CYC(0x5199, 0x519b); goto func_11_51dd; // jr

normalStatus:
  CYC(0x519b, 0x519d); E = 0xc4; // Part.state
  CYC(0x519d, 0x519e); A = mem_rd(gb, DE);
  {
    CYC(0x519e, 0x519f); push_effect(gb, 0x519f);
    uint16_t target = stalfosBone_jump_table(gb);
    if (target == 0x51a5) goto state0;
    if (target == 0x51b5) goto state1;
    goto state2;
  }

state0:
  CYC(0x51a5, 0x51a6); H = D;
  CYC(0x51a6, 0x51a7); L = E;
  CYC(0x51a7, 0x51a8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x51a8, 0x51aa); L = 0xd0; // Part.speed
  CYC(0x51aa, 0x51ac); mem_wr(gb, HL, 0x3c);
  CALL_C(0x51ac, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x51af);
  CYC(0x51af, 0x51b1); E = 0xc9; // Part.angle
  CYC(0x51b1, 0x51b2); mem_wr(gb, DE, A);
  CYC(0x51b2, 0x51b5); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x51b5, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x51b8);
  if (F & FC) { CYCT(0x51b8, 0x51ba); goto L_51db; } // jr c
  CYC(0x51b8, 0x51ba);
  CALL_C(0x51ba, objectApplySpeed_hook, 0x201d, 0x51bd);
  CALL_C(0x51bd, objectCheckWithinScreenBoundary_hook, 0x2184, 0x51c0);
  if (F & FC) { CYCT(0x51c0, 0x51c3); partAnimate_hook(gb); return; } // jp c
  CYC(0x51c0, 0x51c3);

partDelete:
  CYC(0x51c3, 0x51c6); partDelete_hook(gb); return; // jp

state2:
  CALL_C(0x51c6, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x51c9);
  if (F & FZ) { CYCT(0x51c9, 0x51cb); goto partDelete; } // jr z
  CYC(0x51c9, 0x51cb);
  CYC(0x51cb, 0x51cd); C = 0x0e;
  CALL_C(0x51cd, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x51d0);
  CALL_C(0x51d0, objectApplySpeed_hook, 0x201d, 0x51d3);
  CYC(0x51d3, 0x51d6); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x51d6, 0x51d7); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x51d7); return; } // ret c
  CYC(0x51d7, 0x51d8);
  CYC(0x51d8, 0x51db); partAnimate_hook(gb); return; // jp

L_51db:
  if (F & FZ) { CYCT(0x51db, 0x51dd); goto partDelete; } // jr z
  CYC(0x51db, 0x51dd);

func_11_51dd:
  CYC(0x51dd, 0x51df); E = 0xc4; // Part.state
  CYC(0x51df, 0x51e1); A = 0x02;
  CYC(0x51e1, 0x51e2); mem_wr(gb, DE, A);
  CYC(0x51e2, 0x51e3); alu_xor(gb, A);
  CYC(0x51e3, 0x51e6); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}
