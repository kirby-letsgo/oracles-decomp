#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void underwater_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void checkLinkCanSurface_isUnderwater_hook(GB *gb) {
  BASE(checkLinkCanSurface_isUnderwater);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+6); SET_HL(SYM(underWaterSurfaceTable));
  CYC(b_+6, b_+7); underwater_add_double_index_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); A = W8(wActiveRoom);
  CYC(b_+13, b_+14); B = A;

  for (;;) {
    CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+15, b_+16); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+16, b_+18);
      goto no_room_entry;
    }
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+19); alu_cp(gb, B);
    if (F & FZ) {
      CYCT(b_+19, b_+21);
      break;
    }
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+22); SET_HL(HL + 1);
    CYC(b_+22, b_+23); SET_HL(HL + 1);
    CYC(b_+23, b_+25);
  }

  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); H = mem_rd(gb, HL);
  CYC(b_+27, b_+28); L = A;
  CYC(b_+28, b_+31); A = W8(wTilesetFlags);
  CYC(b_+31, b_+33); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+33, b_+35);
    goto check_jabu;
  }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); B = mem_rd(gb, HL);
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+39); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+39, b_+41);
    goto check_tile;
  }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); push_effect(gb, HL);
  CYC(b_+42, b_+44); A = 0x30;
  CALL_C(b_+44, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+47);
  CYC(b_+47, b_+48); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(b_+48, b_+50);
    goto check_tile;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); alu_bit(gb, 0, B);
  if (!(F & FZ)) {
    CYCT(b_+52, b_+54);
    goto no_room_entry;
  }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); A = 0x08;
  CYC(b_+56, b_+57); underwater_add_double_index_to_hl_from_rst(gb, b_+57);
  CYC(b_+57, b_+59);
  goto check_tile;

check_jabu:
  CYC(b_+59, b_+62); A = W8(wDungeonIndex);
  CYC(b_+62, b_+64); alu_cp(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+64, b_+66);
    goto check_tile;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+69); A = W8(wJabuWaterLevel);
  CYC(b_+69, b_+71); alu_and(gb, 0x03);
  CYC(b_+71, b_+73); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+73, b_+75);
    goto check_tile;
  }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+78); A = W8(wActiveRoom);
  CYC(b_+78, b_+80); alu_cp(gb, 0x4c);
  if (F & FZ) {
    CYCT(b_+80, b_+82);
    goto jabu_adjustment;
  }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+84); alu_cp(gb, 0x4d);
  if (!(F & FZ)) {
    CYCT(b_+84, b_+86);
    goto check_tile;
  }
  CYC(b_+84, b_+86);

jabu_adjustment:
  CYC(b_+86, b_+88); A = 0x0b;
  CYC(b_+88, b_+89); underwater_add_double_index_to_hl_from_rst(gb, b_+89);

check_tile:
  CYC(b_+89, b_+92); A = W8(wActiveTilePos);
  CYC(b_+92, b_+93); B = A;
  CYC(b_+93, b_+95); A = alu_swap(gb, A);
  CYC(b_+95, b_+97); alu_and(gb, 0x0f);
  CYC(b_+97, b_+98); underwater_add_double_index_to_hl_from_rst(gb, b_+98);
  CYC(b_+98, b_+99); A = B;
  CYC(b_+99, b_+101); alu_and(gb, 0x0f);
  CYC(b_+101, b_+103); alu_xor(gb, 0x0f);
  CALL_C(b_+103, checkFlag_hook, SYM(checkFlag), b_+106);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+108);
    goto done;
  }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+109); alu_scf(gb);
  CYC(b_+109, b_+111);
  goto done;

no_room_entry:
  CYC(b_+111, b_+114); A = W8(wTilesetFlags);
  CYC(b_+114, b_+116); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+116, b_+118);
    goto done;
  }
  CYC(b_+116, b_+118);
  CYC(b_+118, b_+119); alu_scf(gb);

done:
  CYC(b_+119, b_+121); C = alu_rl(gb, C);
  CYC(b_+121, b_+122); ret_effect(gb);
}
