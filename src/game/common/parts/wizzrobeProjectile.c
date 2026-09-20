#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void func_5369_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5369, 0x536a); H = D;
  CYC(0x536a, 0x536b); L = E;
  CYC(0x536b, 0x536c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x536c, 0x536e); L = 0xd0; // Part.speed
  CYC(0x536e, 0x5370); mem_wr(gb, HL, 0x50);
  CYC(0x5370, 0x5372); E = 0xc9; // Part.angle
  CYC(0x5372, 0x5373); A = mem_rd(gb, DE);
  CYC(0x5373, 0x5375); A = alu_swap(gb, A);
  CYC(0x5375, 0x5376); alu_rlca(gb);
  CALL_C(0x5376, partSetAnimation_hook, 0x2988, 0x5379);
  CYC(0x5379, 0x537c); objectSetVisible81_hook(gb); return; // jp
}

void partCode1f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x5353, 0x5355); goto normalStatus; } // jr nz
  CYC(0x5353, 0x5355);
  CYC(0x5355, 0x5357); E = 0xc4; // Part.state
  CYC(0x5357, 0x5358); A = mem_rd(gb, DE);
  CYC(0x5358, 0x5359); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5359, 0x535b); func_5369_hook(gb); return; } // jr z
  CYC(0x5359, 0x535b);
  CALL_C(0x535b, objectCheckWithinScreenBoundary_hook, 0x2184, 0x535e);
  if (!(F & FC)) { CYCT(0x535e, 0x5360); goto normalStatus; } // jr nc
  CYC(0x535e, 0x5360);
  CALL_C(0x5360, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5363);
  if (!(F & FC)) { CYCT(0x5363, 0x5366); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x5363, 0x5366);

normalStatus:
  CYC(0x5366, 0x5369); partDelete_hook(gb); return; // jp
}
