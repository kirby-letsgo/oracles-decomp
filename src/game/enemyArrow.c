#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t enemyArrow_jump_table(GB *gb) {
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

void partCode1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x50c1, 0x50c3); goto normalStatus; } // jr z
  CYC(0x50c1, 0x50c3);
  CYC(0x50c3, 0x50c5); E = 0xea; // Part.var2a
  CYC(0x50c5, 0x50c6); A = mem_rd(gb, DE);
  CYC(0x50c6, 0x50c8); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x50c8, 0x50ca); goto partDelete; } // jr z
  CYC(0x50c8, 0x50ca);
  CYC(0x50ca, 0x50cc); goto func_11_513a; // jr

normalStatus:
  CYC(0x50cc, 0x50ce); E = 0xc2; // Part.subid
  CYC(0x50ce, 0x50cf); A = mem_rd(gb, DE);
  {
    CYC(0x50cf, 0x50d0); push_effect(gb, 0x50d0);
    uint16_t target = enemyArrow_jump_table(gb);
    if (target == 0x50d4) goto subid0;
    goto subid1;
  }

subid0:
  CYC(0x50d4, 0x50d6); E = 0xc4; // Part.state
  CYC(0x50d6, 0x50d7); A = mem_rd(gb, DE);
  {
    CYC(0x50d7, 0x50d8); push_effect(gb, 0x50d8);
    uint16_t target = enemyArrow_jump_table(gb);
    if (target == 0x50de) goto subid0_state0;
    if (target == 0x50fa) goto subid0_state1;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

subid0_state0:
  CYC(0x50de, 0x50df); H = D;
  CYC(0x50df, 0x50e0); L = E;
  CYC(0x50e0, 0x50e1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x50e1, 0x50e3); L = 0xd0; // Part.speed
  CYC(0x50e3, 0x50e5); mem_wr(gb, HL, 0x50);
  CYC(0x50e5, 0x50e7); L = 0xcb; // Part.yh
  CYC(0x50e7, 0x50e8); B = mem_rd(gb, HL);
  CYC(0x50e8, 0x50ea); L = 0xcd; // Part.xh
  CYC(0x50ea, 0x50eb); C = mem_rd(gb, HL);
  CALL_C(0x50eb, partCommon_setPositionOffsetAndRadiusFromAngle_hook, 0x40e0, 0x50ee);
  CYC(0x50ee, 0x50f0); E = 0xc9; // Part.angle
  CYC(0x50f0, 0x50f1); A = mem_rd(gb, DE);
  CYC(0x50f1, 0x50f3); A = alu_swap(gb, A);
  CYC(0x50f3, 0x50f4); alu_rlca(gb);
  CALL_C(0x50f4, partSetAnimation_hook, 0x2988, 0x50f7);
  CYC(0x50f7, 0x50fa); objectSetVisible81_hook(gb); return; // jp

subid0_state1:
  CALL_C(0x50fa, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x50fd);
  if (!(F & FC)) { CYCT(0x50fd, 0x50ff); goto objectApplySpeed; } // jr nc
  CYC(0x50fd, 0x50ff);
  if (F & FZ) { CYCT(0x50ff, 0x5101); goto partDelete; } // jr z
  CYC(0x50ff, 0x5101);
  CYC(0x5101, 0x5103); goto func_11_513a; // jr

subid1:
  CYC(0x5103, 0x5105); E = 0xc4; // Part.state
  CYC(0x5105, 0x5106); A = mem_rd(gb, DE);
  {
    CYC(0x5106, 0x5107); push_effect(gb, 0x5107);
    uint16_t target = enemyArrow_jump_table(gb);
    if (target == 0x510f) goto subid1_state0;
    if (target == 0x5126) goto subid1_state1;
    if (target == 0x50fa) goto subid0_state1;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

subid1_state0:
  CYC(0x510f, 0x5110); H = D;
  CYC(0x5110, 0x5111); L = E;
  CYC(0x5111, 0x5112); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5112, 0x5114); L = 0xc6; // Part.counter1
  CYC(0x5114, 0x5116); mem_wr(gb, HL, 0x08);
  CYC(0x5116, 0x5118); L = 0xd0; // Part.speed
  CYC(0x5118, 0x511a); mem_wr(gb, HL, 0x50);
  CYC(0x511a, 0x511c); E = 0xc9; // Part.angle
  CYC(0x511c, 0x511d); A = mem_rd(gb, DE);
  CYC(0x511d, 0x511f); A = alu_swap(gb, A);
  CYC(0x511f, 0x5120); alu_rlca(gb);
  CALL_C(0x5120, partSetAnimation_hook, 0x2988, 0x5123);
  CYC(0x5123, 0x5126); objectSetVisible81_hook(gb); return; // jp

subid1_state1:
  CALL_C(0x5126, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5129);
  if (!(F & FZ)) { CYCT(0x5129, 0x512b); goto L_512f; } // jr nz
  CYC(0x5129, 0x512b);
  CYC(0x512b, 0x512c); L = E;
  CYC(0x512c, 0x512d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x512d, 0x512f); goto subid0_state1; // jr

L_512f:
  CALL_C(0x512f, partCommon_checkOutOfBounds_hook, 0x407e, 0x5132);
  if (F & FZ) { CYCT(0x5132, 0x5134); goto partDelete; } // jr z
  CYC(0x5132, 0x5134);

objectApplySpeed:
  CYC(0x5134, 0x5137); objectApplySpeed_hook(gb); return; // jp

partDelete:
  CYC(0x5137, 0x513a); partDelete_hook(gb); return; // jp

func_11_513a:
  CYC(0x513a, 0x513c); E = 0xc2; // Part.subid
  CYC(0x513c, 0x513d); A = mem_rd(gb, DE);
  CYC(0x513d, 0x513e); alu_or(gb, A);
  CYC(0x513e, 0x5140); A = 0x02;
  if (F & FZ) { CYCT(0x5140, 0x5142); goto L_5144; } // jr z
  CYC(0x5140, 0x5142);
  CYC(0x5142, 0x5144); A = 0x03;

L_5144:
  CYC(0x5144, 0x5146); E = 0xc4; // Part.state
  CYC(0x5146, 0x5147); mem_wr(gb, DE, A);
  CYC(0x5147, 0x5149); A = 0x04;
  CYC(0x5149, 0x514c); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}
