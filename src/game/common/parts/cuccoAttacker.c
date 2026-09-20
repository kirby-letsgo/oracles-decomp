#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t cuccoAttacker_jump_table(GB *gb) {
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

static void cuccoAttacker_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void partCode22_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5439, 0x543b); E = 0xc4; // Part.state
  CYC(0x543b, 0x543c); A = mem_rd(gb, DE);
  {
    CYC(0x543c, 0x543d); push_effect(gb, 0x543d);
    uint16_t target = cuccoAttacker_jump_table(gb);
    if (target == 0x5443) goto state0;
    if (target == 0x549c) goto state1;
    goto state2;
  }

state0:
  CYC(0x5443, 0x5444); H = D;
  CYC(0x5444, 0x5445); L = E;
  CYC(0x5445, 0x5446); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x5446, 0x5448); L = 0xc6; // Part.counter1
  CYC(0x5448, 0x544a); mem_wr(gb, HL, 0x18);
  CYC(0x544a, 0x544c); L = 0xcf; // Part.zh
  CYC(0x544c, 0x544e); mem_wr(gb, HL, 0xfa);
  CYC(0x544e, 0x5450); A = 0x30; // Object.var30
  CALL_C(0x5450, objectGetRelatedObject1Var_hook, 0x2160, 0x5453);
  CYC(0x5453, 0x5454); A = mem_rd(gb, HL);
  CYC(0x5454, 0x5456); alu_sub(gb, 0x10);
  CYC(0x5456, 0x5458); alu_and(gb, 0x1e);
  CYC(0x5458, 0x5459); alu_rrca(gb);
  CYC(0x5459, 0x545c); SET_HL(0x54d5); // @speedVals
  CYC(0x545c, 0x545d); cuccoAttacker_addAToHl_from_rst(gb, 0x545d);
  CYC(0x545d, 0x545f); E = 0xd0; // Part.speed
  CYC(0x545f, 0x5460); A = mem_rd(gb, HL);
  CYC(0x5460, 0x5461); mem_wr(gb, DE, A);
  CALL_C(0x5461, objectSetVisiblec1_hook, 0x1e3c, 0x5464);
  CALL_C(0x5464, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5467);
  CYC(0x5467, 0x5468); C = A;
  CYC(0x5468, 0x546a); alu_and(gb, 0x30);
  CYC(0x546a, 0x546b); B = A;
  CYC(0x546b, 0x546d); B = alu_swap(gb, B);
  CYC(0x546d, 0x546f); alu_and(gb, 0x10);
  CYC(0x546f, 0x5472); SET_HL(0x54b5); // @xOrYVals
  CYC(0x5472, 0x5473); cuccoAttacker_addAToHl_from_rst(gb, 0x5473);
  CYC(0x5473, 0x5474); A = C;
  CYC(0x5474, 0x5476); alu_and(gb, 0x0f);
  CYC(0x5476, 0x5477); cuccoAttacker_addAToHl_from_rst(gb, 0x5477);
  CYC(0x5477, 0x5479); alu_bit(gb, 0, B);
  CYC(0x5479, 0x547b); E = 0xcb; // Part.yh
  CYC(0x547b, 0x547d); C = 0xcd; // Part.xh
  if (!(F & FZ)) { CYCT(0x547d, 0x547f); goto L_5482; } // jr nz
  CYC(0x547d, 0x547f);
  CYC(0x547f, 0x5480); E = C;
  CYC(0x5480, 0x5482); C = 0xcb; // Part.yh

L_5482:
  CYC(0x5482, 0x5483); A = mem_rd(gb, HL);
  CYC(0x5483, 0x5484); mem_wr(gb, DE, A);
  CYC(0x5484, 0x5485); A = B;
  CYC(0x5485, 0x5488); SET_HL(0x54b1); // @screenEdgePositions
  CYC(0x5488, 0x5489); cuccoAttacker_addAToHl_from_rst(gb, 0x5489);
  CYC(0x5489, 0x548a); E = C;
  CYC(0x548a, 0x548b); A = mem_rd(gb, HL);
  CYC(0x548b, 0x548c); mem_wr(gb, DE, A);
  CALL_C(0x548c, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x548f);
  CYC(0x548f, 0x5491); E = 0xc9; // Part.angle
  CYC(0x5491, 0x5492); mem_wr(gb, DE, A);
  CYC(0x5492, 0x5494); alu_cp(gb, 0x11);
  CYC(0x5494, 0x5496); A = 0x00;
  if (!(F & FC)) { CYCT(0x5496, 0x5498); goto L_5499; } // jr nc
  CYC(0x5496, 0x5498);
  CYC(0x5498, 0x5499); A = alu_inc8(gb, A);

L_5499:
  CYC(0x5499, 0x549c); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(0x549c, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x549f);
  if (!(F & FZ)) { CYCT(0x549f, 0x54a1); goto applySpeedAndAnimate; } // jr nz
  CYC(0x549f, 0x54a1);
  CYC(0x54a1, 0x54a2); L = E;
  CYC(0x54a2, 0x54a3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x54a3, 0x54a5); goto applySpeedAndAnimate; // jr

state2:
  CALL_C(0x54a5, objectCheckWithinScreenBoundary_hook, 0x2184, 0x54a8);
  if (!(F & FC)) { CYCT(0x54a8, 0x54ab); partDelete_hook(gb); return; } // jp nc
  CYC(0x54a8, 0x54ab);

applySpeedAndAnimate:
  CALL_C(0x54ab, objectApplySpeed_hook, 0x201d, 0x54ae);
  CYC(0x54ae, 0x54b1); partAnimate_hook(gb); return; // jp
}
