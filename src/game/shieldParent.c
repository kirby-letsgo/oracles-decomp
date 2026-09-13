#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t shield_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void parentItemCode_shield_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_ROM(0x4a57, 0x4a80);
  if (!(F & FC)) { CYCT(0x4a5a, 0x4a5c); goto delete_self; }
  CYC(0x4a5a, 0x4a5c);
  CALL_C(0x4a5c, checkNoOtherParentItemsInUse_hook, 0x5366, 0x4a5f);
  if (!(F & FZ)) { CYCT(0x4a5f, 0x4a60); ret_effect(gb); return; }
  CYC(0x4a5f, 0x4a60);
  CYC(0x4a60, 0x4a62); E = 0x04;
  CYC(0x4a62, 0x4a63); A = mem_rd(gb, DE);
  CYC(0x4a63, 0x4a64); push_effect(gb, 0x4a64);
  switch (shield_parent_jump_table(gb)) {
    case 0x4a68:
      CYC(0x4a68, 0x4a6a); A = 0x01;
      CYC(0x4a6a, 0x4a6b); mem_wr(gb, DE, A);
      CYC(0x4a6b, 0x4a6d); A = 0x76;
      CALL_C(0x4a6d, playSound_b00_hook, 0x0c98, 0x4a70);
    case 0x4a70:
      CYC(0x4a70, 0x4a73); A = W8(wShieldLevel);
      CYC(0x4a73, 0x4a75); alu_add(gb, 0x00);
      CYC(0x4a75, 0x4a78); W8(wUsingShield) = A;
      CYC(0x4a78, 0x4a79); ret_effect(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

delete_self:
  CYC(0x4a79, 0x4a7a); alu_xor(gb, A);
  CYC(0x4a7a, 0x4a7d); W8(wUsingShield) = A;
  CYC(0x4a7d, 0x4a80); clearParentItem_hook(gb);
}
