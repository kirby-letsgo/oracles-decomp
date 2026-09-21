#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(itemCode1e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (biggoron_sword_jump_table(gb));
    if (jt_ == SYM(itemCode1d__ret)) { CYC(SYM(itemCode1d__ret), (SYM(itemCode1d__ret) + 1)); ret_effect(gb); return; }
    else if (jt_ == b_+8) {
      CYC(b_+8, b_+10); A = 0x1b;
      CALL_C(b_+10, loadWeaponGfx_b00_hook, SYM(loadWeaponGfx_b00), b_+13);
      CALL_C(b_+13, loadAttributesAndGraphicsAndIncState_hook, SYM(loadAttributesAndGraphicsAndIncState), b_+16);
      CYC(b_+16, b_+18); A = 0xb1;
      CALL_C(b_+18, playSound_b00_hook, SYM(playSound_b00), b_+21);
      CYC(b_+21, b_+24); TAIL(objectSetVisible82);
    }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
