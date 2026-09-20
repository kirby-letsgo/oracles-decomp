#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void func_11_5e8a_hook(GB *gb);

void label_11_212_hook(GB *gb) {
  BASE(label_11_212);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); D = 0xd0; A = D;
  for (;;) {
    CYC(b_+3, b_+5); H8(hActiveObject) = A;
    CYC(b_+5, b_+8); E = 0xc0; A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(b_+8, b_+11);
    else {
      CYC(b_+8, b_+11); alu_rlca(gb);
      if (F & FC) CYCT(b_+11, b_+14);
      else {
        CYC(b_+11, b_+14); E = 0xc4;
        CYC(b_+14, b_+17); A = mem_rd(gb, DE);
        alu_or(gb, A);
        if (!(F & FZ)) { CYCT(b_+17, b_+20); goto next; }
        CYC(b_+17, b_+20);
      }
      CALL_C(b_+20, func_11_5e8a_hook, SYM(func_11_5e8a), b_+23);
    }
next:
    CYC(b_+23, b_+25); D = alu_inc8(gb, D); A = D;
    CYC(b_+25, b_+27); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+27, b_+29); continue; }
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+30);
    ret_effect(gb);
    return;
  }
}

void updateParts_hook(GB *gb) {
  BASE(updateParts);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+4); A = 0xc0; H8(hActiveObjectType) = A;
  CYC(b_+4, b_+7); A = W8(wScrollMode);
  CYC(b_+7, b_+9); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+9, b_+11); label_11_212_hook(gb); return; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); label_11_212_hook(gb); return; }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+20); A = W8(wDisabledObjects);
  CYC(b_+20, b_+22); alu_and(gb, 0x88);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); label_11_212_hook(gb); return; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); D = 0xd0; A = D;
  for (;;) {
    CYC(b_+27, b_+29); H8(hActiveObject) = A;
    CYC(b_+29, b_+32); E = 0xc0; A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(b_+32, b_+35);
    else {
      CYC(b_+32, b_+35);
      CALL_C(b_+35, func_11_5e8a_hook, SYM(func_11_5e8a), b_+38);
      CYC(b_+38, b_+41); H = D; L = 0xea;
      CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
    }
    CYC(b_+43, b_+45); D = alu_inc8(gb, D); A = D;
    CYC(b_+45, b_+47); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+47, b_+49); continue; }
    CYC(b_+47, b_+49);
    CYC(b_+49, b_+50);
    ret_effect(gb);
    return;
  }
}

void func_11_5e8a_hook(GB *gb) {
  BASE(func_11_5e8a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, partCommon_standardUpdate_hook, SYM(partCommon_standardUpdate), b_+3);
  CYC(b_+3, b_+6); E = 0xc1; A = mem_rd(gb, DE);
  CYC(b_+6, b_+9); alu_add(gb, A); alu_add(gb, GV(0x9f, 0x23));
  CYC(b_+9, b_+12); L = A; A = 0;
  CYC(b_+12, b_+15); alu_adc(gb, GV(0x5e, 0x62)); H = A;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+18); H = mem_rd(gb, HL); L = A;
  CYC(b_+18, b_+20); A = C; alu_or(gb, A);
  CYC(b_+20, b_+21);
  hook_continue(gb, HL, sp0_);
}
