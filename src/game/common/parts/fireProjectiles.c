#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode19), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode19), (from), (to), true)

static uint16_t fireProjectiles_jump_table(GB *gb) {
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

void partCode19_hook(GB *gb) {
  BASE(partCode19);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = 0xc4; // Part.state
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  {
    CYC(b_+6, b_+7); push_effect(gb, b_+7);
    uint16_t target = fireProjectiles_jump_table(gb);
    if (target == b_+13) goto state0;
    if (target == b_+27) goto state1;
    goto state2;
  }

state0:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+18); L = 0xc6; // Part.counter1
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x08);
  CYC(b_+20, b_+22); L = 0xd0; // Part.speed
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x3c);
  CYC(b_+24, b_+27); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(b_+27, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); L = E;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+33, b_+35); L = 0xc2; // Part.subid
  CYC(b_+35, b_+37); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+37, b_+39); goto L_50a1; } // jr z
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); B = H8(hFFB2);
  CYC(b_+42, b_+45); C = H8(hFFB3);
  CALL_C(b_+45, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+48);
  CYC(b_+48, b_+50); E = 0xc9; // Part.angle
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  RET(b_+51); return; // ret

L_50a1:
  CALL_C(b_+52, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+55);
  CYC(b_+55, b_+57); E = 0xc9; // Part.angle
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  RET(b_+58); return; // ret

state2:
  CYC(b_+59, b_+62); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+62, b_+64); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); goto L_50b5; } // jr nz
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); E = 0xdc; // Part.oamFlags
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+71); alu_xor(gb, 0x07);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);

L_50b5:
  CALL_C(b_+72, objectApplySpeed_hook, SYM(objectApplySpeed), b_+75);
  CALL_C(b_+75, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+78);
  if (!(F & FC)) { CYCT(b_+78, b_+81); partDelete_hook(gb); return; } // jp nc
  CYC(b_+78, b_+81);
  CYC(b_+81, SYM(partCode1a)); partAnimate_hook(gb); return; // jp
}
