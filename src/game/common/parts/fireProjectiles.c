#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x506d, 0x5070); partDelete_hook(gb); return; } // jp nz
  CYC(0x506d, 0x5070);
  CYC(0x5070, 0x5072); E = 0xc4; // Part.state
  CYC(0x5072, 0x5073); A = mem_rd(gb, DE);
  {
    CYC(0x5073, 0x5074); push_effect(gb, 0x5074);
    uint16_t target = fireProjectiles_jump_table(gb);
    if (target == 0x507a) goto state0;
    if (target == 0x5088) goto state1;
    goto state2;
  }

state0:
  CYC(0x507a, 0x507b); H = D;
  CYC(0x507b, 0x507c); L = E;
  CYC(0x507c, 0x507d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x507d, 0x507f); L = 0xc6; // Part.counter1
  CYC(0x507f, 0x5081); mem_wr(gb, HL, 0x08);
  CYC(0x5081, 0x5083); L = 0xd0; // Part.speed
  CYC(0x5083, 0x5085); mem_wr(gb, HL, 0x3c);
  CYC(0x5085, 0x5088); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x5088, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x508b);
  if (!(F & FZ)) { RET_TAKEN(0x508b); return; } // ret nz
  CYC(0x508b, 0x508c);
  CYC(0x508c, 0x508d); L = E;
  CYC(0x508d, 0x508e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x508e, 0x5090); L = 0xc2; // Part.subid
  CYC(0x5090, 0x5092); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5092, 0x5094); goto L_50a1; } // jr z
  CYC(0x5092, 0x5094);
  CYC(0x5094, 0x5097); B = H8(hFFB2);
  CYC(0x5097, 0x509a); C = H8(hFFB3);
  CALL_C(0x509a, objectGetRelativeAngle_hook, 0x1ea4, 0x509d);
  CYC(0x509d, 0x509f); E = 0xc9; // Part.angle
  CYC(0x509f, 0x50a0); mem_wr(gb, DE, A);
  RET(0x50a0); return; // ret

L_50a1:
  CALL_C(0x50a1, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x50a4);
  CYC(0x50a4, 0x50a6); E = 0xc9; // Part.angle
  CYC(0x50a6, 0x50a7); mem_wr(gb, DE, A);
  RET(0x50a7); return; // ret

state2:
  CYC(0x50a8, 0x50ab); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x50ab, 0x50ad); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x50ad, 0x50af); goto L_50b5; } // jr nz
  CYC(0x50ad, 0x50af);
  CYC(0x50af, 0x50b1); E = 0xdc; // Part.oamFlags
  CYC(0x50b1, 0x50b2); A = mem_rd(gb, DE);
  CYC(0x50b2, 0x50b4); alu_xor(gb, 0x07);
  CYC(0x50b4, 0x50b5); mem_wr(gb, DE, A);

L_50b5:
  CALL_C(0x50b5, objectApplySpeed_hook, 0x201d, 0x50b8);
  CALL_C(0x50b8, objectCheckWithinScreenBoundary_hook, 0x2184, 0x50bb);
  if (!(F & FC)) { CYCT(0x50bb, 0x50be); partDelete_hook(gb); return; } // jp nc
  CYC(0x50bb, 0x50be);
  CYC(0x50be, 0x50c1); partAnimate_hook(gb); return; // jp
}
