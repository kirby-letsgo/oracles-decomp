#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void braceletCheckBreakable_hook(GB *gb);

static void common_bomb_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void common_bomb_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void itemBeginThrow_hook(GB *gb) {
  BASE(itemBeginThrow);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemSetVar3cToFF_hook, SYM(itemSetVar3cToFF), b_+3);
  CYC(b_+3, b_+6); A = W8(w1Link_direction);
  CYC(b_+6, b_+9); SET_HL(b_+81);
  CYC(b_+9, b_+10); common_bomb_add_a_to_hl(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); C = mem_rd(gb, HL);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x0b;
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); alu_add(gb, C);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = 0x00;
  CYC(b_+23, b_+25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));
  CYC(b_+25, b_+27); L = 0x37;
  CYC(b_+27, b_+29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_and(gb, 0xf0);
  CYC(b_+33, b_+35); A = alu_swap(gb, A);
  CYC(b_+35, b_+36); alu_add(gb, A);
  CYC(b_+36, b_+39); SET_HL(SYM(itemWeights));
  CYC(b_+39, b_+40); common_bomb_add_double_index(gb, b_+40);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+43); E = 0x39;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+46); E = 0x09;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+48, b_+50);
    goto clear_speed;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); E = 0x14;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); E = alu_inc8(gb, E);
  CYC(b_+55, b_+57); A = 0xff;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+60); A = 0x12;
  CALL_C(b_+60, cpActiveRing_hook, SYM(cpActiveRing), b_+63);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+65);
  } else {
    CYC(b_+63, b_+65);
    CYC(b_+65, b_+66); SET_HL(HL + 1);
  }
  CYC(b_+66, b_+68); E = 0x10;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  CYC(b_+70, b_+71); ret_effect(gb);
  return;

clear_speed:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = 0x10;
  CYC(b_+74, b_+75); alu_xor(gb, A);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
  CYC(b_+76, b_+78); L = 0x14;
  CYC(b_+78, b_+79); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+80, b_+81); ret_effect(gb);
}

void itemUpdateThrowingLaterally_hook(GB *gb) {
  BASE(itemUpdateThrowingLaterally);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x38;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x40);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7);
    goto normal;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, GV(0x30, 0x20));
  if (!(F & FC)) {
    CYCT(b_+9, b_+11);
    goto weight3;
  }
  CYC(b_+9, b_+11);

normal:
  CYC(b_+11, b_+13); E = 0x09;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+16, b_+18);
    goto unset_z;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x18);
  CYC(b_+20, b_+21); alu_rrca(gb);
  CYC(b_+21, b_+22); alu_rrca(gb);
  CYC(b_+22, b_+25); SET_HL(SYM(bombEdgeOffsets));
  CYC(b_+25, b_+26); common_bomb_add_a_to_hl(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); C = mem_rd(gb, HL);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = 0x0b;
  CYC(b_+31, b_+32); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+34); alu_cp(gb, 0xb0);
  if (!(F & FC)) {
    CYCT(b_+34, b_+36);
    goto no_collision;
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); B = A;
  CYC(b_+37, b_+39); L = 0x0d;
  CYC(b_+39, b_+40); A = C;
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+42); C = A;
  CALL_C(b_+42, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+45);
  if (!(F & FC)) {
    CYCT(b_+45, b_+47);
    goto no_collision;
  }
  CYC(b_+45, b_+47);
  CALL_C(b_+47, itemCheckCanPassSolidTileAt_hook, SYM(itemCheckCanPassSolidTileAt), b_+50);
  if (F & FZ) {
    CYCT(b_+50, b_+52);
    goto no_collision;
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54);
  goto collision;

weight3:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = 0x0b;
  CYC(b_+57, b_+58); B = mem_rd(gb, HL);
  CYC(b_+58, b_+60); L = 0x0d;
  CYC(b_+60, b_+61); C = mem_rd(gb, HL);
  CYC(b_+61, b_+63); E = 0x09;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+66); alu_and(gb, 0x18);
  CYC(b_+66, b_+69); SET_HL(SYM(data_649a));
  CYC(b_+69, b_+70); common_bomb_add_a_to_hl(gb, b_+70);
  CYC(b_+70, b_+72); E = 0x04;

check_corner:
  CYC(b_+72, b_+73); push_effect(gb, BC);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+74, b_+75); alu_add(gb, B);
  CYC(b_+75, b_+76); B = A;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+77, b_+78); alu_add(gb, C);
  CYC(b_+78, b_+79); C = A;
  CYC(b_+79, b_+80); push_effect(gb, HL);
  CALL_C(b_+80, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+83);
  CYC(b_+83, b_+84); SET_HL(pop_effect(gb));
  CYC(b_+84, b_+85); SET_BC(pop_effect(gb));
  if (F & FC) {
    CYCT(b_+85, b_+87);
    goto collision;
  }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); E = alu_dec8(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+88, b_+90);
    goto check_corner;
  }
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+92);
  goto no_collision;

collision:
  CALL_C(b_+92, braceletCheckBreakable_hook, SYM(braceletCheckBreakable), b_+95);
  if (!(F & FZ)) {
    CYCT(b_+95, b_+97);
    goto set_z;
  }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); E = 0x09;
  CYC(b_+99, b_+101); A = 0xff;
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);

no_collision:
  CYC(b_+102, b_+105); A = W8(wTilesetFlags);
  CYC(b_+105, b_+107); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+107, b_+109);
    goto apply_speed;
  }
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+111); E = 0x09;
  CYC(b_+111, b_+112); A = mem_rd(gb, DE);
  CYC(b_+112, b_+114); alu_and(gb, 0x0f);
  if (F & FZ) {
    CYCT(b_+114, b_+116);
    goto unset_z;
  }
  CYC(b_+114, b_+116);

apply_speed:
  CALL_C(b_+116, objectApplySpeed_hook, SYM(objectApplySpeed), b_+119);

unset_z:
  CYC(b_+119, b_+120); alu_or(gb, D);
  CYC(b_+120, b_+121); ret_effect(gb);
  return;

set_z:
  CYC(b_+121, b_+122); alu_xor(gb, A);
  CYC(b_+122, b_+123); ret_effect(gb);
}

void itemBounce_hook(GB *gb) {
  BASE(itemBounce);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x52;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CALL_C(b_+5, objectNegateAndHalveSpeedZ_hook, SYM(objectNegateAndHalveSpeedZ), b_+8);
  if (F & FC) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); E = 0x10;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); E = A;
  CYC(b_+13, b_+16); SET_HL(SYM(bounceSpeedReductionMapping));
  CALL_C(b_+16, lookupKey_hook, SYM(lookupKey), b_+19);
  CYC(b_+19, b_+21); E = 0x10;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); alu_or(gb, A);
  CYC(b_+23, b_+24); ret_effect(gb);
}
