#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionCodec7__ret_hook(GB *gb) {
  BASE(interactionCodec7);
  CYC(b_+65, b_+66); SET_BC(pop_effect(gb));
  CYC(b_+66, b_+67); SET_HL(pop_effect(gb));
  CYC(b_+67, b_+68); ret_effect(gb);
}

void interactionCodec7__spawnObjectType_hook(GB *gb) {
  BASE(interactionCodec7);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+69, b_+72); getFreeEnemySlot_hook(gb);
    return;
  }
  CYC(b_+69, b_+72);
  CYC(b_+72, b_+73); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+73, b_+76); getFreePartSlot_hook(gb);
    return;
  }
  CYC(b_+73, b_+76);
  CYC(b_+76, b_+77); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+77, b_+80); getFreeInteractionSlot_hook(gb);
    return;
  }
  CYC(b_+77, b_+80);
  CYC(b_+80, b_+81); ret_effect(gb);
}

void interactionCodec7__createObject_hook(GB *gb) {
  BASE(interactionCodec7);
  CYC(b_+21, b_+22); push_effect(gb, HL);
  CYC(b_+22, b_+23); push_effect(gb, BC);
  CYC(b_+23, b_+24); B = L;
  CYC(b_+24, b_+26); E = 0x4d;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); alu_and(gb, 0xf0);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+34); push_effect(gb, b_+34); interactionCodec7__spawnObjectType_hook(gb);
  if (!(F & FZ)) {
    CYCT(b_+34, b_+36); interactionCodec7__ret_hook(gb);
    return;
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); E = 0x4b;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+42); E = 0x4d;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+45); alu_and(gb, 0x0f);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+47); A = L;
  CYC(b_+47, b_+49); alu_add(gb, 0x09);
  CYC(b_+49, b_+50); L = A;
  CYC(b_+50, b_+51); A = B;
  CYC(b_+51, b_+53); alu_and(gb, 0xf0);
  CYC(b_+53, b_+55); alu_add(gb, 0x08);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+58); A = B;
  CYC(b_+58, b_+60); alu_and(gb, 0x0f);
  CYC(b_+60, b_+62); A = alu_swap(gb, A);
  CYC(b_+62, b_+64); alu_add(gb, 0x08);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  TAIL(interactionCodec7__ret);
}

void interactionCodec7_hook(GB *gb) {
  BASE(interactionCodec7);
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); SET_HL(wRoomLayout);
  CYC(b_+7, b_+9); B = 0xb0;
loop:
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+11, b_+14); push_effect(gb, b_+14); interactionCodec7__createObject_hook(gb);
  } else CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+18); goto loop;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); TAIL(interactionDelete);
}
