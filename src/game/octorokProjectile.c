#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t octorokProjectile_jump_table(GB *gb) {
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

void partCode18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5026, 0x5028); goto normalStatus; } // jr z
  CYC(0x5026, 0x5028);
  CYC(0x5028, 0x502a); E = 0xea; // Part.var2a
  CYC(0x502a, 0x502b); A = mem_rd(gb, DE);
  CYC(0x502b, 0x502d); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x502d, 0x5030); partDelete_hook(gb); return; } // jp z
  CYC(0x502d, 0x5030);
  CYC(0x5030, 0x5031); H = D;
  CYC(0x5031, 0x5033); L = 0xc4; // Part.state
  CYC(0x5033, 0x5034); A = mem_rd(gb, HL);
  CYC(0x5034, 0x5036); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x5036, 0x5038); goto normalStatus; } // jr nc
  CYC(0x5036, 0x5038);
  CYC(0x5038, 0x503a); mem_wr(gb, HL, 0x02);

normalStatus:
  CYC(0x503a, 0x503c); E = 0xc4; // Part.state
  CYC(0x503c, 0x503d); A = mem_rd(gb, DE);
  {
    CYC(0x503d, 0x503e); push_effect(gb, 0x503e);
    uint16_t target = octorokProjectile_jump_table(gb);
    if (target == 0x5046) goto state0;
    if (target == 0x5050) goto state1;
    if (target == 0x5066) goto state2;
    partCommon_updateSpeedAndDeleteWhenCounter1Is0_hook(gb); return;
  }

state0:
  CYC(0x5046, 0x5047); H = D;
  CYC(0x5047, 0x5048); L = E;
  CYC(0x5048, 0x5049); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5049, 0x504b); L = 0xd0; // Part.speed
  CYC(0x504b, 0x504d); mem_wr(gb, HL, 0x50);
  CYC(0x504d, 0x5050); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x5050, objectCheckWithinScreenBoundary_hook, 0x2184, 0x5053);
  if (!(F & FC)) { CYCT(0x5053, 0x5056); partDelete_hook(gb); return; } // jp nc
  CYC(0x5053, 0x5056);
  CALL_C(0x5056, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5059);
  if (!(F & FC)) { CYCT(0x5059, 0x505b); goto L_5063; } // jr nc
  CYC(0x5059, 0x505b);
  if (F & FZ) { CYCT(0x505b, 0x505e); partDelete_hook(gb); return; } // jp z
  CYC(0x505b, 0x505e);
  CYC(0x505e, 0x5060); E = 0xc4; // Part.state
  CYC(0x5060, 0x5062); A = 0x02;
  CYC(0x5062, 0x5063); mem_wr(gb, DE, A);

L_5063:
  CYC(0x5063, 0x5066); objectApplySpeed_hook(gb); return; // jp

state2:
  CYC(0x5066, 0x5068); A = 0x03;
  CYC(0x5068, 0x5069); mem_wr(gb, DE, A);
  CYC(0x5069, 0x506a); alu_xor(gb, A);
  CYC(0x506a, 0x506d); partCommon_bounceWhenCollisionsEnabled_hook(gb); return; // jp
}
