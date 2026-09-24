#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/items/rodOfSeasons.s.
// ITEM_ROD_OF_SEASONS

static uint16_t rodOfSeasons_jump_table(GB *gb) {
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

// ITEM_ROD_OF_SEASONS
void s_itemCode07_hook(GB *gb) {
  BASE(itemCode07);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_itemTransferKnockbackToLink, SYM(itemTransferKnockbackToLink), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (rodOfSeasons_jump_table(gb));
    if (jt_ == b_+11) goto state0;
    if (jt_ == b_+39) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = 0x00;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x03);
  CYC(b_+19, b_+21); L = 0x06;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x10);
  CYC(b_+23, b_+25); A = 0x74;
  CALL_C(b_+25, s_playSound, SYM(playSound_b00), b_+28);
  CYC(b_+28, b_+30); A = 0x1c;
  CALL_C(b_+30, s_loadWeaponGfx, SYM(loadWeaponGfx_b00), b_+33);
  CALL_C(b_+33, s_itemLoadAttributesAndGraphics, SYM(itemLoadAttributesAndGraphics), b_+36);
  CYC(b_+36, b_+39);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+39, b_+40); H = D;
  CYC(b_+40, b_+42); L = 0x06;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+43); return; }
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+47); A = mem_rd(gb, wActiveTileType);
  CYC(b_+47, b_+49); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+49); return; }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+53);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x15);
  CYC(b_+56, b_+58); E = 0x09;
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+62, b_+65);
  TAIL(objectCopyPosition);
}

