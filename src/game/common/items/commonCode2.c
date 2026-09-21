#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void tryBreakTileWithSword_hook(GB *gb);

static void common_code2_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void common_code2_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void tryBreakTileWithExpertsRing_hook(GB *gb) {
  BASE(tryBreakTileWithExpertsRing);
  CYC(b_+0, b_+3); A = W8(w1Link_direction);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, b_+9);
  tryBreakTileWithSword_hook(gb);
}

void tryBreakTileWithSword_calculateLevel_hook(GB *gb) {
  BASE(tryBreakTileWithSword_calculateLevel);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); A = W8(wSwordLevel);
  CYC(b_+4, b_+6); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+6, b_+8);
    TAIL(tryBreakTileWithSword);
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); A = 0x02;
  tryBreakTileWithSword_hook(gb);
}

void tryBreakTileWithSword_hook(GB *gb) {
  BASE(tryBreakTileWithSword);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = A;
  CYC(b_+1, b_+4); A = W8(w1Link_zh);
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  CYC(b_+5, b_+7); alu_cp(gb, 0xf6);
  if (F & FC) {
    CYCT(b_+7, b_+8); ret_effect(gb); return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); A = C;
  CYC(b_+9, b_+12); SET_HL(b_+93);
  CYC(b_+12, b_+13); common_code2_add_double_index_from_rst(gb, b_+13);
  CYC(b_+13, b_+16); A = W8(w1Link_yh);
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+22); A = W8(w1Link_xh);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+25); push_effect(gb, BC);
  CYC(b_+25, b_+26); A = E;
  CALL_C(b_+26, tryToBreakTile_hook, SYM(tryToBreakTile), b_+29);
  CYC(b_+29, b_+31); A = H8(hFF93);
  CYC(b_+31, b_+34); W8(wccb0) = A;
  CYC(b_+34, b_+36); A = H8(hFF92);
  CYC(b_+36, b_+39); W8(wccaf) = A;
  CYC(b_+39, b_+40); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+40, b_+41); ret_effect(gb); return;
  }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+44); SET_HL(SYM(clinkSoundTable));
  CALL_C(b_+44, findByteInCollisionTable_hook, SYM(findByteInCollisionTable), b_+47);
  if (F & FC) {
    CYCT(b_+47, b_+49);
    goto bombable_wall_clink;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+52); A = W8(w1ParentItem2_subid);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+53, b_+54); ret_effect(gb); return;
  }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, findByteAtHl_hook, SYM(findByteAtHl), b_+57);
  if (F & FC) {
    CYCT(b_+57, b_+58); ret_effect(gb); return;
  }
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); A = H8(hFF93);
  CYC(b_+60, b_+61); L = A;
  CYC(b_+61, b_+63); H = wRoomCollisions >> 8;
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);
  CYC(b_+64, b_+66); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+66, b_+67); ret_effect(gb); return;
  }
  CYC(b_+66, b_+67);
  CYC(b_+67, b_+69); E = 0x01;
  CYC(b_+69, b_+71);
  goto create_clink;

bombable_wall_clink:
  CYC(b_+71, b_+73); A = 0x58;
  CALL_C(b_+73, playSound_b00_hook, SYM(playSound_b00), b_+76);
  CYC(b_+76, b_+78); E = 0x80;

create_clink:
  CALL_C(b_+78, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+81);
  if (!(F & FZ)) {
    CYCT(b_+81, b_+82); ret_effect(gb); return;
  }
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+84); mem_wr(gb, HL, 0x07);
  CYC(b_+84, b_+85); L = alu_inc8(gb, L);
  CYC(b_+85, b_+86); mem_wr(gb, HL, E);
  CYC(b_+86, b_+88); L = 0x4b;
  CYC(b_+88, b_+89); mem_wr(gb, HL, B);
  CYC(b_+89, b_+91); L = 0x4d;
  CYC(b_+91, b_+92); mem_wr(gb, HL, C);
  CYC(b_+92, b_+93); ret_effect(gb);
}

void itemCalculateSwordDamage_hook(GB *gb) {
  BASE(itemCalculateSwordDamage);
  CYC(b_+0, b_+2); E = 0x3a;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = W8(w1ParentItem2_var3a);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto apply_damage_modifier;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); SET_HL(b_+76);
  CYC(b_+13, b_+16); A = W8(wActiveRing);
  CYC(b_+16, b_+17); E = A;

next_ring:
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+19, b_+21);
    goto no_ring_modifier;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
    goto found_ring_modifier;
  }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); SET_HL(HL + 1);
  CYC(b_+25, b_+27);
  goto next_ring;

no_ring_modifier:
  CYC(b_+27, b_+28); A = E;
  CYC(b_+28, b_+30); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+30, b_+32);
    goto red_ring;
  }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); alu_cp(gb, 0x09);
  if (F & FZ) {
    CYCT(b_+34, b_+36);
    goto green_ring;
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+38, b_+40);
    goto cursed_ring;
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); A = B;
  CYC(b_+41, b_+43);
  goto set_damage;

red_ring:
  CYC(b_+43, b_+44); A = B;
  CYC(b_+44, b_+46);
  goto apply_damage_modifier;

green_ring:
  CYC(b_+46, b_+47); A = B;
  CYC(b_+47, b_+48); alu_cpl(gb);
  CYC(b_+48, b_+49); A = alu_inc8(gb, A);
  CYC(b_+49, b_+51); A = alu_sra(gb, A);
  CYC(b_+51, b_+52); alu_cpl(gb);
  CYC(b_+52, b_+53); A = alu_inc8(gb, A);
  CYC(b_+53, b_+55);
  goto apply_damage_modifier;

cursed_ring:
  CYC(b_+55, b_+56); A = B;
  CYC(b_+56, b_+57); alu_cpl(gb);
  CYC(b_+57, b_+58); A = alu_inc8(gb, A);
  CYC(b_+58, b_+60); A = alu_sra(gb, A);
  CYC(b_+60, b_+61); alu_cpl(gb);
  CYC(b_+61, b_+62); A = alu_inc8(gb, A);
  CYC(b_+62, b_+64);
  goto set_damage;

found_ring_modifier:
  CYC(b_+64, b_+65); A = mem_rd(gb, HL);

apply_damage_modifier:
  CYC(b_+65, b_+66); alu_add(gb, B);

set_damage:
  CYC(b_+66, b_+68); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+68, b_+70);
  } else {
    CYC(b_+68, b_+70);
    CYC(b_+70, b_+72); A = 0xff;
  }
  CYC(b_+72, b_+74); E = 0x28;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); ret_effect(gb);
}

void itemMimicBgTile_hook(GB *gb) {
  BASE(itemMimicBgTile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getTileMappingData_hook, SYM(getTileMappingData), b_+3);
  CYC(b_+3, b_+4); push_effect(gb, BC);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = 0x1b;
  CYC(b_+7, b_+9); A = 0x0f;
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+15); A = W8(wEnemyPlacement_numEnemies);
  CYC(b_+15, b_+16); alu_sub(gb, C);
  if (F & FZ) {
    CYCT(b_+16, b_+18);
  } else {
    CYC(b_+16, b_+18);
    CYC(b_+18, b_+20); A = 0x01;
  }
  CALL_C(b_+20, itemSetAnimation_hook, SYM(itemSetAnimation), b_+23);
  CYC(b_+23, b_+24); SET_AF(pop_effect(gb));
  CYC(b_+24, b_+26); alu_and(gb, 0x07);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+29); alu_rrca(gb);
  CYC(b_+29, b_+32); SET_HL(w2TilesetBgPalettes);
  CYC(b_+32, b_+33); common_code2_add_a_to_hl_from_rst(gb, b_+33);
  CYC(b_+33, b_+34); push_effect(gb, DE);
  CYC(b_+34, b_+36); A = 0x02;
  CYC(b_+36, b_+38); hram_wr(gb, R_SVBK, A);
  CYC(b_+38, b_+41); SET_DE(w2TilesetSprPalettes + 7 * 8);
  CYC(b_+41, b_+43); B = 0x08;
  CALL_C(b_+43, copyMemory_hook, SYM(copyMemory), b_+46);
  CYC(b_+46, b_+49); SET_HL(hDirtySprPalettes);
  CYC(b_+49, b_+51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CYC(b_+52, b_+54); hram_wr(gb, R_SVBK, A);
  CYC(b_+54, b_+55); SET_DE(pop_effect(gb));
  CYC(b_+55, b_+56); ret_effect(gb);
}
