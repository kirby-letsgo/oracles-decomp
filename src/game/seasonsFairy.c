#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void objectOscillateZ_body_hook(GB *gb) {
  CYC(0x492d, 0x4930); A = mem_rd(gb, 0xcc00);
  CYC(0x4930, 0x4932); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x4932, 0x4933); ret_effect(gb); return;
  }
  CYC(0x4932, 0x4933);
  CYC(0x4933, 0x4936); A = mem_rd(gb, 0xcc00);
  CYC(0x4936, 0x4938); alu_and(gb, 0x38);
  CYC(0x4938, 0x493a); A = alu_swap(gb, A);
  CYC(0x493a, 0x493b); alu_rlca(gb);
  CYC(0x493b, 0x493e); SET_HL(0x4948);
  CYC(0x493e, 0x493f); push_effect(gb, 0x493f);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(0x493f, 0x4941); A = mem_rd(gb, 0xffae);
  CYC(0x4941, 0x4943); alu_add(gb, 0x0f);
  CYC(0x4943, 0x4944); E = A;
  CYC(0x4944, 0x4945); A = mem_rd(gb, DE);
  CYC(0x4945, 0x4946); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4946, 0x4947); mem_wr(gb, DE, A);
  CYC(0x4947, 0x4948); ret_effect(gb);
}
