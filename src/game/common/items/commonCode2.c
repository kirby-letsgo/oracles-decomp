#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

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
  CYC(0x618a, 0x618d); A = W8(w1Link_direction);
  CYC(0x618d, 0x618e); alu_add(gb, A);
  CYC(0x618e, 0x618f); C = A;
  CYC(0x618f, 0x6191); A = 0x03;
  CYC(0x6191, 0x6193);
  tryBreakTileWithSword_hook(gb);
}

void tryBreakTileWithSword_calculateLevel_hook(GB *gb) {
  CYC(0x6193, 0x6194); C = A;
  CYC(0x6194, 0x6197); A = W8(wSwordLevel);
  CYC(0x6197, 0x6199); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x6199, 0x619b);
    tryBreakTileWithSword_hook(gb);
    return;
  }
  CYC(0x6199, 0x619b);
  CYC(0x619b, 0x619d); A = 0x02;
  tryBreakTileWithSword_hook(gb);
}

void tryBreakTileWithSword_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x619d, 0x619e); E = A;
  CYC(0x619e, 0x61a1); A = W8(w1Link_zh);
  CYC(0x61a1, 0x61a2); A = alu_dec8(gb, A);
  CYC(0x61a2, 0x61a4); alu_cp(gb, 0xf6);
  if (F & FC) {
    CYCT(0x61a4, 0x61a5); ret_effect(gb); return;
  }
  CYC(0x61a4, 0x61a5);
  CYC(0x61a5, 0x61a6); A = C;
  CYC(0x61a6, 0x61a9); SET_HL(0x61fa);
  CYC(0x61a9, 0x61aa); common_code2_add_double_index_from_rst(gb, 0x61aa);
  CYC(0x61aa, 0x61ad); A = W8(w1Link_yh);
  CYC(0x61ad, 0x61ae); alu_add(gb, mem_rd(gb, HL));
  CYC(0x61ae, 0x61af); B = A;
  CYC(0x61af, 0x61b0); SET_HL(HL + 1);
  CYC(0x61b0, 0x61b3); A = W8(w1Link_xh);
  CYC(0x61b3, 0x61b4); alu_add(gb, mem_rd(gb, HL));
  CYC(0x61b4, 0x61b5); C = A;
  CYC(0x61b5, 0x61b6); push_effect(gb, BC);
  CYC(0x61b6, 0x61b7); A = E;
  CALL_C(0x61b7, tryToBreakTile_hook, 0x2bf6, 0x61ba);
  CYC(0x61ba, 0x61bc); A = H8(hFF93);
  CYC(0x61bc, 0x61bf); W8(wccb0) = A;
  CYC(0x61bf, 0x61c1); A = H8(hFF92);
  CYC(0x61c1, 0x61c4); W8(wccaf) = A;
  CYC(0x61c4, 0x61c5); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(0x61c5, 0x61c6); ret_effect(gb); return;
  }
  CYC(0x61c5, 0x61c6);
  CYC(0x61c6, 0x61c9); SET_HL(0x620c);
  CALL_C(0x61c9, findByteInCollisionTable_hook, 0x1e29, 0x61cc);
  if (F & FC) {
    CYCT(0x61cc, 0x61ce);
    goto bombable_wall_clink;
  }
  CYC(0x61cc, 0x61ce);
  CYC(0x61ce, 0x61d1); A = W8(w1ParentItem2_subid);
  CYC(0x61d1, 0x61d2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x61d2, 0x61d3); ret_effect(gb); return;
  }
  CYC(0x61d2, 0x61d3);
  CALL_C(0x61d3, findByteAtHl_hook, 0x1e17, 0x61d6);
  if (F & FC) {
    CYCT(0x61d6, 0x61d7); ret_effect(gb); return;
  }
  CYC(0x61d6, 0x61d7);
  CYC(0x61d7, 0x61d9); A = H8(hFF93);
  CYC(0x61d9, 0x61da); L = A;
  CYC(0x61da, 0x61dc); H = wRoomCollisions >> 8;
  CYC(0x61dc, 0x61dd); A = mem_rd(gb, HL);
  CYC(0x61dd, 0x61df); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x61df, 0x61e0); ret_effect(gb); return;
  }
  CYC(0x61df, 0x61e0);
  CYC(0x61e0, 0x61e2); E = 0x01;
  CYC(0x61e2, 0x61e4);
  goto create_clink;

bombable_wall_clink:
  CYC(0x61e4, 0x61e6); A = 0x58;
  CALL_C(0x61e6, playSound_b00_hook, 0x0c98, 0x61e9);
  CYC(0x61e9, 0x61eb); E = 0x80;

create_clink:
  CALL_C(0x61eb, getFreeInteractionSlot_hook, 0x3aef, 0x61ee);
  if (!(F & FZ)) {
    CYCT(0x61ee, 0x61ef); ret_effect(gb); return;
  }
  CYC(0x61ee, 0x61ef);
  CYC(0x61ef, 0x61f1); mem_wr(gb, HL, 0x07);
  CYC(0x61f1, 0x61f2); L = alu_inc8(gb, L);
  CYC(0x61f2, 0x61f3); mem_wr(gb, HL, E);
  CYC(0x61f3, 0x61f5); L = 0x4b;
  CYC(0x61f5, 0x61f6); mem_wr(gb, HL, B);
  CYC(0x61f6, 0x61f8); L = 0x4d;
  CYC(0x61f8, 0x61f9); mem_wr(gb, HL, C);
  CYC(0x61f9, 0x61fa); ret_effect(gb);
}

void itemCalculateSwordDamage_hook(GB *gb) {
  CYC(0x6235, 0x6237); E = 0x3a;
  CYC(0x6237, 0x6238); A = mem_rd(gb, DE);
  CYC(0x6238, 0x6239); B = A;
  CYC(0x6239, 0x623c); A = W8(w1ParentItem2_var3a);
  CYC(0x623c, 0x623d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x623d, 0x623f);
    goto apply_damage_modifier;
  }
  CYC(0x623d, 0x623f);
  CYC(0x623f, 0x6242); SET_HL(0x6281);
  CYC(0x6242, 0x6245); A = W8(wActiveRing);
  CYC(0x6245, 0x6246); E = A;

next_ring:
  CYC(0x6246, 0x6247); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6247, 0x6248); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6248, 0x624a);
    goto no_ring_modifier;
  }
  CYC(0x6248, 0x624a);
  CYC(0x624a, 0x624b); alu_cp(gb, E);
  if (F & FZ) {
    CYCT(0x624b, 0x624d);
    goto found_ring_modifier;
  }
  CYC(0x624b, 0x624d);
  CYC(0x624d, 0x624e); SET_HL(HL + 1);
  CYC(0x624e, 0x6250);
  goto next_ring;

no_ring_modifier:
  CYC(0x6250, 0x6251); A = E;
  CYC(0x6251, 0x6253); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(0x6253, 0x6255);
    goto red_ring;
  }
  CYC(0x6253, 0x6255);
  CYC(0x6255, 0x6257); alu_cp(gb, 0x09);
  if (F & FZ) {
    CYCT(0x6257, 0x6259);
    goto green_ring;
  }
  CYC(0x6257, 0x6259);
  CYC(0x6259, 0x625b); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x625b, 0x625d);
    goto cursed_ring;
  }
  CYC(0x625b, 0x625d);
  CYC(0x625d, 0x625e); A = B;
  CYC(0x625e, 0x6260);
  goto set_damage;

red_ring:
  CYC(0x6260, 0x6261); A = B;
  CYC(0x6261, 0x6263);
  goto apply_damage_modifier;

green_ring:
  CYC(0x6263, 0x6264); A = B;
  CYC(0x6264, 0x6265); alu_cpl(gb);
  CYC(0x6265, 0x6266); A = alu_inc8(gb, A);
  CYC(0x6266, 0x6268); A = alu_sra(gb, A);
  CYC(0x6268, 0x6269); alu_cpl(gb);
  CYC(0x6269, 0x626a); A = alu_inc8(gb, A);
  CYC(0x626a, 0x626c);
  goto apply_damage_modifier;

cursed_ring:
  CYC(0x626c, 0x626d); A = B;
  CYC(0x626d, 0x626e); alu_cpl(gb);
  CYC(0x626e, 0x626f); A = alu_inc8(gb, A);
  CYC(0x626f, 0x6271); A = alu_sra(gb, A);
  CYC(0x6271, 0x6272); alu_cpl(gb);
  CYC(0x6272, 0x6273); A = alu_inc8(gb, A);
  CYC(0x6273, 0x6275);
  goto set_damage;

found_ring_modifier:
  CYC(0x6275, 0x6276); A = mem_rd(gb, HL);

apply_damage_modifier:
  CYC(0x6276, 0x6277); alu_add(gb, B);

set_damage:
  CYC(0x6277, 0x6279); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x6279, 0x627b);
  } else {
    CYC(0x6279, 0x627b);
    CYC(0x627b, 0x627d); A = 0xff;
  }
  CYC(0x627d, 0x627f); E = 0x28;
  CYC(0x627f, 0x6280); mem_wr(gb, DE, A);
  CYC(0x6280, 0x6281); ret_effect(gb);
}

void itemMimicBgTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x628e, getTileMappingData_hook, 0x3a72, 0x6291);
  CYC(0x6291, 0x6292); push_effect(gb, BC);
  CYC(0x6292, 0x6293); H = D;
  CYC(0x6293, 0x6295); L = 0x1b;
  CYC(0x6295, 0x6297); A = 0x0f;
  CYC(0x6297, 0x6298); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6298, 0x6299); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6299, 0x629a); mem_wr(gb, HL, C);
  CYC(0x629a, 0x629d); A = W8(wEnemyPlacement_numEnemies);
  CYC(0x629d, 0x629e); alu_sub(gb, C);
  if (F & FZ) {
    CYCT(0x629e, 0x62a0);
  } else {
    CYC(0x629e, 0x62a0);
    CYC(0x62a0, 0x62a2); A = 0x01;
  }
  CALL_C(0x62a2, itemSetAnimation_hook, 0x49e2, 0x62a5);
  CYC(0x62a5, 0x62a6); SET_AF(pop_effect(gb));
  CYC(0x62a6, 0x62a8); alu_and(gb, 0x07);
  CYC(0x62a8, 0x62aa); A = alu_swap(gb, A);
  CYC(0x62aa, 0x62ab); alu_rrca(gb);
  CYC(0x62ab, 0x62ae); SET_HL(w2TilesetBgPalettes);
  CYC(0x62ae, 0x62af); common_code2_add_a_to_hl_from_rst(gb, 0x62af);
  CYC(0x62af, 0x62b0); push_effect(gb, DE);
  CYC(0x62b0, 0x62b2); A = 0x02;
  CYC(0x62b2, 0x62b4); hram_wr(gb, R_SVBK, A);
  CYC(0x62b4, 0x62b7); SET_DE(w2TilesetSprPalettes + 7 * 8);
  CYC(0x62b7, 0x62b9); B = 0x08;
  CALL_C(0x62b9, copyMemory_hook, 0x0486, 0x62bc);
  CYC(0x62bc, 0x62bf); SET_HL(hDirtySprPalettes);
  CYC(0x62bf, 0x62c1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x62c1, 0x62c2); alu_xor(gb, A);
  CYC(0x62c2, 0x62c4); hram_wr(gb, R_SVBK, A);
  CYC(0x62c4, 0x62c5); SET_DE(pop_effect(gb));
  CYC(0x62c5, 0x62c6); ret_effect(gb);
}
