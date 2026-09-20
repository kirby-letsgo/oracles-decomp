#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x514c, 0x514e); goto normalStatus; } // jr z
  CYC(0x514c, 0x514e);
  CYC(0x514e, 0x5150); E = 0xea; // Part.var2a
  CYC(0x5150, 0x5151); A = mem_rd(gb, DE);
  CYC(0x5151, 0x5153); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x5153, 0x5155); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(0x5155, 0x5158); partDelete_hook(gb); return; } // jp c
  CYC(0x5155, 0x5158);

normalStatus:
  CYC(0x5158, 0x515a); E = 0xc4; // Part.state
  CYC(0x515a, 0x515b); A = mem_rd(gb, DE);
  CYC(0x515b, 0x515c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x515c, 0x515e); goto L_5174; } // jr z
  CYC(0x515c, 0x515e);
  CALL_C(0x515e, objectCheckWithinScreenBoundary_hook, 0x2184, 0x5161);
  if (!(F & FC)) { CYCT(0x5161, 0x5164); partDelete_hook(gb); return; } // jp nc
  CYC(0x5161, 0x5164);
  CALL_C(0x5164, objectApplySpeed_hook, 0x201d, 0x5167);
  CYC(0x5167, 0x516a); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x516a, 0x516c); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x516c); return; } // ret nz
  CYC(0x516c, 0x516d);
  CYC(0x516d, 0x516f); E = 0xdc; // Part.oamFlags
  CYC(0x516f, 0x5170); A = mem_rd(gb, DE);
  CYC(0x5170, 0x5172); alu_xor(gb, 0x07);
  CYC(0x5172, 0x5173); mem_wr(gb, DE, A);
  RET(0x5173); return; // ret

L_5174:
  CYC(0x5174, 0x5175); H = D;
  CYC(0x5175, 0x5176); L = E;
  CYC(0x5176, 0x5177); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5177, 0x5179); L = 0xd0; // Part.speed
  CYC(0x5179, 0x517b); mem_wr(gb, HL, 0x78);
  CYC(0x517b, 0x517d); L = 0xcb; // Part.yh
  CYC(0x517d, 0x517e); B = mem_rd(gb, HL);
  CYC(0x517e, 0x5180); L = 0xcd; // Part.xh
  CYC(0x5180, 0x5181); C = mem_rd(gb, HL);
  CALL_C(0x5181, partCommon_setPositionOffsetAndRadiusFromAngle_hook, 0x40e0, 0x5184);
  CYC(0x5184, 0x5186); E = 0xc9; // Part.angle
  CYC(0x5186, 0x5187); A = mem_rd(gb, DE);
  CYC(0x5187, 0x5189); A = alu_swap(gb, A);
  CYC(0x5189, 0x518a); alu_rlca(gb);
  CALL_C(0x518a, partSetAnimation_hook, 0x2988, 0x518d);
  CYC(0x518d, 0x5190); objectSetVisible81_hook(gb); return; // jp
}
