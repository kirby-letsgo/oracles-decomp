#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode46), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode46), (from), (to), true)

// PART_SEED_SHOOTER_EYE_STATUE
void partCode46_hook(GB *gb) {
  BASE(partCode46);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0xc6; // Part.counter1
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x2d);
  CYC(b_+7, b_+9); L = 0xc2; // Part.subid
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_and(gb, 0x07);
  CYC(b_+12, b_+15); SET_HL(wActiveTriggers);
  CALL_C(b_+15, setFlag_hook, SYM(setFlag), b_+18);
  CALL_C(b_+18, objectSetVisible83_hook, SYM(objectSetVisible83), b_+21);

normalStatus:
  CYC(b_+21, b_+23); E = 0xc4; // Part.state
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+25, b_+27); goto state0; } // jr z
  CYC(b_+25, b_+27);
  CALL_C(b_+27, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); E = 0xc2; // Part.subid
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+37); SET_HL(wActiveTriggers);
  CALL_C(b_+37, unsetFlag_hook, SYM(unsetFlag), b_+40);
  CYC(b_+40, b_+43); objectSetInvisible_hook(gb); return; // jp

state0:
  CYC(b_+43, b_+44); A = alu_inc8(gb, A);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  RET(b_+45); return; // ret
}
