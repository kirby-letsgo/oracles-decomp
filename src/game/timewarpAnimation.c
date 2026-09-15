#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode2b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6124, 0x6126); E = 0xc4; // Part.state
  CYC(0x6126, 0x6127); A = mem_rd(gb, DE);
  CYC(0x6127, 0x6128); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6128, 0x612a); goto state1; } // jr nz
  CYC(0x6128, 0x612a);
  CYC(0x612a, 0x612b); A = alu_inc8(gb, A);
  CYC(0x612b, 0x612c); mem_wr(gb, DE, A);
  CYC(0x612c, 0x612d); H = D;
  CYC(0x612d, 0x612f); L = 0xc0;
  CYC(0x612f, 0x6131); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

state1:
  CALL_C(0x6131, objectApplySpeed_hook, 0x201d, 0x6134);
  CYC(0x6134, 0x6136); E = 0xcb; // Part.yh
  CYC(0x6136, 0x6137); A = mem_rd(gb, DE);
  CYC(0x6137, 0x6139); alu_add(gb, 0x04);
  CYC(0x6139, 0x613b); alu_cp(gb, 0xf4);
  if (!(F & FC)) { CYCT(0x613b, 0x613e); partDelete_hook(gb); return; } // jp nc
  CYC(0x613b, 0x613e);
  CYC(0x613e, 0x6140); A = 0x04;
  CALL_C(0x6140, objectGetRelatedObject1Var_hook, 0x2160, 0x6143);
  CYC(0x6143, 0x6144); A = mem_rd(gb, HL);
  CYC(0x6144, 0x6146); alu_cp(gb, 0x03);
  CYC(0x6146, 0x6148); E = 0xc2; // Part.subid
  CYC(0x6148, 0x6149); A = mem_rd(gb, DE);
  if (F & FC) { CYCT(0x6149, 0x614b); goto relatedObj1_stateLessThan3; } // jr c
  CYC(0x6149, 0x614b);
  CYC(0x614b, 0x614d); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x614d, 0x6150); partDelete_hook(gb); return; } // jp nz

relatedObj1_stateLessThan3:
  CYC(0x6150, 0x6152); L = 0x61;
  CYC(0x6152, 0x6153); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x6153, 0x6154); alu_rrca(gb);
  if (F & FC) { CYCT(0x6154, 0x6157); objectSetInvisible_hook(gb); return; } // jp c
  CYC(0x6154, 0x6157);
  CYC(0x6157, 0x615a); objectSetVisible83_hook(gb); return; // jp
}
