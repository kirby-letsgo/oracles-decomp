#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/items/magnetGloves.s.
// ITEM_MAGNET_GLOVES

static uint16_t magnetGloves_jump_table(GB *gb) {
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

// ITEM_MAGNET_GLOVES
void s_itemCode08_hook(GB *gb) {
  BASE(itemCode08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (magnetGloves_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+19) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x1e;
  CALL_C(b_+10, s_loadWeaponGfx, SYM(loadWeaponGfx_b00), b_+13);
  CALL_C(b_+13, s_loadAttributesAndGraphicsAndIncState, SYM(loadAttributesAndGraphicsAndIncState), b_+16);
  CALL_C(b_+16, s_objectSetVisible81, SYM(objectSetVisible81), b_+19);
state1:
  CYC(b_+19, b_+22); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+22, b_+24); alu_bit(gb, 1, A);
  CYC(b_+24, b_+26); A = 0x0c;
  if (F & FZ) { CYCT(b_+26, b_+28); goto L_5bde; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
L_5bde:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = 0x1b;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  RET(b_+34); return;
}

