#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void func_11_5e8a_hook(GB *gb);

void label_11_212_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e3a, 0x5e3d); D = 0xd0; A = D;
  for (;;) {
    CYC(0x5e3d, 0x5e3f); H8(hActiveObject) = A;
    CYC(0x5e3f, 0x5e42); E = 0xc0; A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(0x5e42, 0x5e45);
    else {
      CYC(0x5e42, 0x5e45); alu_rlca(gb);
      if (F & FC) CYCT(0x5e45, 0x5e48);
      else {
        CYC(0x5e45, 0x5e48); E = 0xc4;
        CYC(0x5e48, 0x5e4b); A = mem_rd(gb, DE);
        alu_or(gb, A);
        if (!(F & FZ)) { CYCT(0x5e4b, 0x5e4e); goto next; }
        CYC(0x5e4b, 0x5e4e);
      }
      CALL_C(0x5e4e, func_11_5e8a_hook, 0x5e8a, 0x5e51);
    }
next:
    CYC(0x5e51, 0x5e53); D = alu_inc8(gb, D); A = D;
    CYC(0x5e53, 0x5e55); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x5e55, 0x5e57); continue; }
    CYC(0x5e55, 0x5e57);
    CYC(0x5e57, 0x5e58);
    ret_effect(gb);
    return;
  }
}

void updateParts_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e58, 0x5e5c); A = 0xc0; H8(hActiveObjectType) = A;
  CYC(0x5e5c, 0x5e5f); A = W8(wScrollMode);
  CYC(0x5e5f, 0x5e61); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(0x5e61, 0x5e63); label_11_212_hook(gb); return; }
  CYC(0x5e61, 0x5e63);
  CYC(0x5e63, 0x5e66); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5e66, 0x5e69); label_11_212_hook(gb); return; }
  CYC(0x5e66, 0x5e69);
  CYC(0x5e69, 0x5e6c); A = W8(wDisabledObjects);
  CYC(0x5e6c, 0x5e6e); alu_and(gb, 0x88);
  if (!(F & FZ)) { CYCT(0x5e6e, 0x5e70); label_11_212_hook(gb); return; }
  CYC(0x5e6e, 0x5e70);
  CYC(0x5e70, 0x5e73); D = 0xd0; A = D;
  for (;;) {
    CYC(0x5e73, 0x5e75); H8(hActiveObject) = A;
    CYC(0x5e75, 0x5e78); E = 0xc0; A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(0x5e78, 0x5e7b);
    else {
      CYC(0x5e78, 0x5e7b);
      CALL_C(0x5e7b, func_11_5e8a_hook, 0x5e8a, 0x5e7e);
      CYC(0x5e7e, 0x5e81); H = D; L = 0xea;
      CYC(0x5e81, 0x5e83); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
    }
    CYC(0x5e83, 0x5e85); D = alu_inc8(gb, D); A = D;
    CYC(0x5e85, 0x5e87); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x5e87, 0x5e89); continue; }
    CYC(0x5e87, 0x5e89);
    CYC(0x5e89, 0x5e8a);
    ret_effect(gb);
    return;
  }
}

void func_11_5e8a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5e8a, partCommon_standardUpdate, 0x403b, 0x5e8d);
  CYC(0x5e8d, 0x5e90); E = 0xc1; A = mem_rd(gb, DE);
  CYC(0x5e90, 0x5e93); alu_add(gb, A); alu_add(gb, 0x9f);
  CYC(0x5e93, 0x5e96); L = A; A = 0;
  CYC(0x5e96, 0x5e99); alu_adc(gb, 0x5e); H = A;
  CYC(0x5e99, 0x5e9a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5e9a, 0x5e9c); H = mem_rd(gb, HL); L = A;
  CYC(0x5e9c, 0x5e9e); A = C; alu_or(gb, A);
  CYC(0x5e9e, 0x5e9f);
  hook_continue(gb, HL, sp0_);
}
