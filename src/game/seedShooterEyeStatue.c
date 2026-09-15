#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

// PART_SEED_SHOOTER_EYE_STATUE
void partCode46_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x75b7, 0x75b9); goto normalStatus; } // jr z
  CYC(0x75b7, 0x75b9);
  CYC(0x75b9, 0x75ba); H = D;
  CYC(0x75ba, 0x75bc); L = 0xc6; // Part.counter1
  CYC(0x75bc, 0x75be); mem_wr(gb, HL, 0x2d);
  CYC(0x75be, 0x75c0); L = 0xc2; // Part.subid
  CYC(0x75c0, 0x75c1); A = mem_rd(gb, HL);
  CYC(0x75c1, 0x75c3); alu_and(gb, 0x07);
  CYC(0x75c3, 0x75c6); SET_HL(wActiveTriggers);
  CALL_C(0x75c6, setFlag_hook, 0x020e, 0x75c9);
  CALL_C(0x75c9, objectSetVisible83_hook, 0x1e72, 0x75cc);

normalStatus:
  CYC(0x75cc, 0x75ce); E = 0xc4; // Part.state
  CYC(0x75ce, 0x75cf); A = mem_rd(gb, DE);
  CYC(0x75cf, 0x75d0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x75d0, 0x75d2); goto state0; } // jr z
  CYC(0x75d0, 0x75d2);
  CALL_C(0x75d2, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x75d5);
  if (!(F & FZ)) { RET_TAKEN(0x75d5); return; } // ret nz
  CYC(0x75d5, 0x75d6);
  CYC(0x75d6, 0x75d8); E = 0xc2; // Part.subid
  CYC(0x75d8, 0x75d9); A = mem_rd(gb, DE);
  CYC(0x75d9, 0x75dc); SET_HL(wActiveTriggers);
  CALL_C(0x75dc, unsetFlag_hook, 0x0218, 0x75df);
  CYC(0x75df, 0x75e2); objectSetInvisible_hook(gb); return; // jp

state0:
  CYC(0x75e2, 0x75e3); A = alu_inc8(gb, A);
  CYC(0x75e3, 0x75e4); mem_wr(gb, DE, A);
  RET(0x75e4); return; // ret
}
