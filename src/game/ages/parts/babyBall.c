#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t babyBall_jump_table(GB *gb) {
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

// PART_BABY_BALL
// Turns Link into a baby
void partCode2f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x6455, 0x6458); partDelete_hook(gb); return; } // jp nz
  CYC(0x6455, 0x6458);
  CYC(0x6458, 0x645a); A = 0x29; // Object.health
  CALL_C(0x645a, objectGetRelatedObject1Var_hook, 0x2160, 0x645d);
  CYC(0x645d, 0x645e); A = mem_rd(gb, HL);
  CYC(0x645e, 0x645f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x645f, 0x6461); goto veranFairyBeat; } // jr z
  CYC(0x645f, 0x6461);
  CYC(0x6461, 0x6462); B = H;
  CYC(0x6462, 0x6464); E = 0xc4; // Part.state
  CYC(0x6464, 0x6465); A = mem_rd(gb, DE);
  {
    CYC(0x6465, 0x6466); push_effect(gb, 0x6466);
    uint16_t target = babyBall_jump_table(gb);
    if (target == 0x646c) goto state0;
    if (target == 0x647f) goto state1;
    goto state2;
  }

state0:
  CYC(0x646c, 0x646d); H = D;
  CYC(0x646d, 0x646e); L = E;
  CYC(0x646e, 0x646f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x646f, 0x6471); L = 0xd0; // Part.speed
  CYC(0x6471, 0x6473); mem_wr(gb, HL, 0x14);
  CYC(0x6473, 0x6475); L = 0xc6; // Part.counter1
  CYC(0x6475, 0x6477); mem_wr(gb, HL, 0x1e);
  CYC(0x6477, 0x6479); A = 0xbf; // SND_BLUE_STALFOS_CHARGE
  CALL_C(0x6479, playSound_b00_hook, 0x0c98, 0x647c);
  CYC(0x647c, 0x647f); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x647f, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6482);
  if (!(F & FZ)) { CYCT(0x6482, 0x6484); goto animate; } // jr nz
  CYC(0x6482, 0x6484);
  CYC(0x6484, 0x6485); L = E;
  CYC(0x6485, 0x6486); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x6486, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6489);
  CYC(0x6489, 0x648b); E = 0xc9; // Part.angle
  CYC(0x648b, 0x648c); mem_wr(gb, DE, A);
  CYC(0x648c, 0x648e); A = 0xbb; // SND_BEAM2
  CALL_C(0x648e, playSound_b00_hook, 0x0c98, 0x6491);
  CYC(0x6491, 0x6493); goto animate; // jr

state2:
  CYC(0x6493, 0x6495); C = 0x84; // Enemy.state
  CYC(0x6495, 0x6496); A = mem_rd(gb, BC);
  CYC(0x6496, 0x6498); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6498, 0x649a); goto applySpeed; } // jr nz
  CYC(0x6498, 0x649a);
  CYC(0x649a, 0x649c); C = 0x83; // Enemy.var03
  CYC(0x649c, 0x649d); A = mem_rd(gb, BC);
  CYC(0x649d, 0x649f); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x649f, 0x64a1); goto applySpeed; } // jr nz
  CYC(0x649f, 0x64a1);
  CYC(0x64a1, 0x64a4); A = mem_rd(gb, wFrameCounter);
  CYC(0x64a4, 0x64a6); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x64a6, 0x64a8); goto applySpeed; } // jr nz
  CYC(0x64a6, 0x64a8);
  CALL_C(0x64a8, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x64ab);
  CALL_C(0x64ab, objectNudgeAngleTowards_hook, 0x1fd4, 0x64ae);

applySpeed:
  CALL_C(0x64ae, partCommon_checkOutOfBounds_hook, 0x407e, 0x64b1);
  if (F & FZ) { CYCT(0x64b1, 0x64b3); goto delete; } // jr z
  CYC(0x64b1, 0x64b3);
  CALL_C(0x64b3, objectApplySpeed_hook, 0x201d, 0x64b6);

animate:
  CYC(0x64b6, 0x64b9); partAnimate_hook(gb); return; // jp

veranFairyBeat:
  CALL_C(0x64b9, objectCreatePuff_hook, 0x24c1, 0x64bc);

delete:
  CYC(0x64bc, 0x64bf); partDelete_hook(gb); return; // jp
}
