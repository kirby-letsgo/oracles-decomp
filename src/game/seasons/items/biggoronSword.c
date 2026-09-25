#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/items/biggoronSword.s.

static uint16_t biggoronSword_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// itemCode1d@ret
static void biggoronSword_ret(GB *gb) {
  BASE(itemCode1d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  RET(b_+15); return;
}

// ITEM_BIGGORON_SWORD
void s_itemCode0c_hook(GB *gb) {
  BASE(itemCode0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (biggoronSword_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == SYM(itemCode1d__ret)) { biggoronSword_ret(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x1b;
  CALL_C(b_+10, s_loadWeaponGfx, SYM(loadWeaponGfx_b00), b_+13);
  CALL_C(b_+13, s_loadAttributesAndGraphicsAndIncState, SYM(loadAttributesAndGraphicsAndIncState), b_+16);
  CYC(b_+16, b_+18); A = 0xb1;
  CALL_C(b_+18, s_playSound, SYM(playSound_b00), b_+21);
  CYC(b_+21, b_+24);
  TAIL(objectSetVisible82);
}
