#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

// 3f:5091
void clearTextGfxBuffer_hook(GB *gb) {
  CYC(0x5091, 0x5094); SET_HL(0xd200);
  CYC(0x5094, 0x5097); SET_BC(0x0200);
  CYC(0x5097, 0x5099); A = 0xff;
  CYC(0x5099, 0x509c);
  fillMemoryBc_hook(gb);
}

// 3f:509c
void clearLineTextBuffer_hook(GB *gb) {
  CYC(0x509c, 0x509f); SET_HL(0xd400);
  CYC(0x509f, 0x50a0); D = H;
  CYC(0x50a0, 0x50a1); E = L;
  CYC(0x50a1, 0x50a3); B = 0x10;
  CYC(0x50a3, 0x50a6);
  clearMemory_hook(gb);
}

// 3f:50a6
void setLineTextBuffers_hook(GB *gb) {
  CYC(0x50a6, 0x50a7); mem_wr(gb, DE, A);
  CYC(0x50a7, 0x50a8); push_effect(gb, DE);
  CYC(0x50a8, 0x50a9); push_effect(gb, HL);

  CYC(0x50a9, 0x50ac); SET_HL(0xd0c7);
  CYC(0x50ac, 0x50ad); A = E;
  CYC(0x50ad, 0x50af); alu_add(gb, 0x10);
  CYC(0x50af, 0x50b0); E = A;
  CYC(0x50b0, 0x50b1); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x50b1, 0x50b2); mem_wr(gb, DE, A);

  CYC(0x50b2, 0x50b3); A = E;
  CYC(0x50b3, 0x50b5); alu_add(gb, 0x10);
  CYC(0x50b5, 0x50b6); E = A;
  CYC(0x50b6, 0x50b7); L = alu_dec8(gb, L);
  CYC(0x50b7, 0x50b8); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x50b8, 0x50b9); mem_wr(gb, DE, A);

  CYC(0x50b9, 0x50ba); A = E;
  CYC(0x50ba, 0x50bc); alu_add(gb, 0x10);
  CYC(0x50bc, 0x50bd); E = A;
  CYC(0x50bd, 0x50be); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x50be, 0x50bf); mem_wr(gb, DE, A);

  CYC(0x50bf, 0x50c0); A = E;
  CYC(0x50c0, 0x50c2); alu_add(gb, 0x10);
  CYC(0x50c2, 0x50c3); E = A;
  CYC(0x50c3, 0x50c4); A = mem_rd(gb, HL);
  CYC(0x50c4, 0x50c5); mem_wr(gb, DE, A);
  CYC(0x50c5, 0x50c7); mem_wr(gb, HL, 0x00);

  CYC(0x50c7, 0x50c8); SET_HL(pop_effect(gb));
  CYC(0x50c8, 0x50c9); SET_DE(pop_effect(gb));
  CYC(0x50c9, 0x50ca); A = mem_rd(gb, DE);
  CYC(0x50ca, 0x50cb); E = alu_inc8(gb, E);
  CYC(0x50cb, 0x50cc); ret_effect(gb);
}

void dmaTextGfxBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50cc, 0x50ce); alu_add(gb, 0x94);
  CYC(0x50ce, 0x50cf); D = A;
  CYC(0x50cf, 0x50d1); E = 0x00;
  CYC(0x50d1, 0x50d4); SET_HL(w7TextGfxBuffer);
  CYC(0x50d4, 0x50d7); SET_BC(0x1f07);
  CYC(0x50d7, 0x50d8); push_effect(gb, HL);
  CALL_C(0x50d8, queueDmaTransfer_hook, 0x058a, 0x50db);
  CYC(0x50db, 0x50dc); SET_HL(pop_effect(gb));
  CYC(0x50dc, 0x50dd); ret_effect(gb);
}

void initTextboxMapping_hook(GB *gb) {
  CYC(0x514a, 0x514d); A = mem_rd(gb, w7d0cc);
  CYC(0x514d, 0x514e); A = alu_inc8(gb, A);
  CYC(0x514e, 0x5150); alu_and(gb, 0x1f);
  CYC(0x5150, 0x5151); L = A;
  CYC(0x5151, 0x5153); E = 0x05;
  for (;;) {
    CYC(0x5153, 0x5155); B = 0x12;
    CYC(0x5155, 0x5156); A = L;
    CYC(0x5156, 0x5157); D = A;
    CYC(0x5157, 0x5159); alu_and(gb, 0xe0);
    CYC(0x5159, 0x515a); C = A;
    for (;;) {
      CYC(0x515a, 0x515c); H = 0xd0;
      CYC(0x515c, 0x515e); mem_wr(gb, HL, 0x02);
      CYC(0x515e, 0x5160); H = 0xd1;
      CYC(0x5160, 0x5162); mem_wr(gb, HL, 0x80);
      CYC(0x5162, 0x5163); A = L;
      CYC(0x5163, 0x5164); A = alu_inc8(gb, A);
      CYC(0x5164, 0x5166); alu_and(gb, 0x1f);
      CYC(0x5166, 0x5167); alu_or(gb, C);
      CYC(0x5167, 0x5168); L = A;
      CYC(0x5168, 0x5169); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(0x5169, 0x516b);
        continue;
      }
      CYC(0x5169, 0x516b);
      break;
    }
    CYC(0x516b, 0x516c); A = D;
    CYC(0x516c, 0x516e); alu_add(gb, 0x20);
    CYC(0x516e, 0x516f); L = A;
    CYC(0x516f, 0x5170); E = alu_dec8(gb, E);
    if (!(F & FZ)) {
      CYCT(0x5170, 0x5172);
      continue;
    }
    CYC(0x5170, 0x5172);
    break;
  }
  CYC(0x5172, 0x5173); ret_effect(gb);
}
