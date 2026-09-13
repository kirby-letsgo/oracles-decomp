#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t magnet_ball_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void itemCode29_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5b51, 0x5b53); E = 0x04;
  CYC(0x5b53, 0x5b54); A = mem_rd(gb, DE);
  CYC(0x5b54, 0x5b55); push_effect(gb, 0x5b55);
  switch (magnet_ball_jump_table(gb)) {
    case 0x5b59:
      CYC(0x5b59, 0x5b5b); A = 0x1d;
      CALL_C(0x5b5b, loadWeaponGfx_b00_hook, 0x166d, 0x5b5e);
      CALL_C(0x5b5e, loadAttributesAndGraphicsAndIncState_hook, 0x498c, 0x5b61);
      CYC(0x5b61, 0x5b63); E = 0x30;
      CYC(0x5b63, 0x5b65); A = 0xff;
      CYC(0x5b65, 0x5b66); mem_wr(gb, DE, A);
      CYC(0x5b66, 0x5b69); objectSetVisible81_hook(gb); return;
    case 0x5b69: CYC(0x5b69, 0x5b6a); ret_effect(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
