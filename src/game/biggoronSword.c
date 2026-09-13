#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t biggoron_sword_jump_table(GB *gb) {
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

void itemCode0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e77, 0x5e79); E = 0x04;
  CYC(0x5e79, 0x5e7a); A = mem_rd(gb, DE);
  CYC(0x5e7a, 0x5e7b); push_effect(gb, 0x5e7b);
  switch (biggoron_sword_jump_table(gb)) {
    case 0x5e69: CYC(0x5e69, 0x5e6a); ret_effect(gb); return;
    case 0x5e7f:
      CYC(0x5e7f, 0x5e81); A = 0x1b;
      CALL_C(0x5e81, loadWeaponGfx_b00_hook, 0x166d, 0x5e84);
      CALL_C(0x5e84, loadAttributesAndGraphicsAndIncState_hook, 0x498c, 0x5e87);
      CYC(0x5e87, 0x5e89); A = 0xb1;
      CALL_C(0x5e89, playSound_b00_hook, 0x0c98, 0x5e8c);
      CYC(0x5e8c, 0x5e8f); objectSetVisible82_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
