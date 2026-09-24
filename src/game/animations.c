#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void initializeAnimations__locFunc_hook(GB *gb) {
  BASE(initializeAnimations);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+O(25), updateAnimationData_hook, SYM(updateAnimationData), b_+OE(28));
  for (;;) {
    CALL_C(b_+O(28), updateAnimationQueue_hook, SYM(updateAnimationQueue), b_+OE(31));
    if (!(F & FZ)) { CYCT(b_+O(31), b_+OE(33)); continue; }
    CYC(b_+O(31), b_+OE(33));
    CYC(b_+O(33), b_+OE(34));
    ret_effect(gb);
    return;
  }
}

void initializeAnimations_hook(GB *gb) {
  BASE(initializeAnimations);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(3)); A = mem_rd(gb, wTilesetAnimation);
  CYC(b_+O(3), b_+OE(5)); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+O(5), b_+OE(6)); ret_effect(gb); return; }
  CYC(b_+O(5), b_+OE(6));
  CALL_C(b_+O(6), loadAnimationData_hook, SYM(loadAnimationData), b_+OE(9));
  if (game_seasons) { initializeAnimations__locFunc_hook(gb); return; }      // Seasons runs the loop once, inline
  if (!game_seasons) {
    CALL_L(b_+9, initializeAnimations__locFunc_hook, b_+12);
    CYC(b_+12, b_+15); SET_HL(wAnimationState);
    CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
    CALL_L(b_+17, initializeAnimations__locFunc_hook, b_+20);
    CYC(b_+20, b_+23); SET_HL(wAnimationState);
    CYC(b_+23, b_+25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
    TAIL(initializeAnimations__locFunc);
  }
}

void updateAnimations_hook(GB *gb) {
  BASE(updateAnimations);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wAnimationState);
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x40));
  CYC(b_+5, b_+8); A = mem_rd(gb, wTilesetAnimation);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wScreenVariables);
  CYC(b_+13, b_+15); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+15, b_+16); ret_effect(gb); return; }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, updateAnimationQueue_hook, SYM(updateAnimationQueue), b_+19);
  CYC(b_+19, b_+21);
  TAIL(updateAnimationData);
}

static void add_a_to_hl(GB *gb, uint16_t address) {
  burn_rom(gb, 0x04, address, (uint16_t)(address + 1), false);
  push_effect(gb, (uint16_t)(address + 1));
  burn_rom(gb, 0, 0x0010, 0x0012, false); alu_add(gb, L); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x0012, 0x0015, true); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 0x0012, 0x0013, true);
  }
  pop_effect(gb);
}

void updateAnimationQueue_hook(GB *gb) {
  BASE(updateAnimationQueue);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wAnimationQueueHead);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wAnimationQueueTail);
  CYC(b_+7, b_+8); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+13); alu_and(gb, 0x1f);
  CYC(b_+13, b_+16); mem_wr(gb, wAnimationQueueHead, A);
  CYC(b_+16, b_+19); SET_HL(w2AnimationQueue);
  add_a_to_hl(gb, b_+19);
  CYC(b_+20, b_+22); A = 0x02;
  CYC(b_+22, b_+24); mem_wr(gb, 0xff70, A);
  CYC(b_+24, b_+25); B = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+28); mem_wr(gb, 0xff70, A);
  CYC(b_+28, b_+29); A = B;
  CALL_C(b_+29, loadAnimationGfxIndex_hook, SYM(loadAnimationGfxIndex), b_+32);
  CYC(b_+32, b_+35); SET_HL(wAnimationState);
  CYC(b_+35, b_+37); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(b_+37, b_+38); alu_or(gb, H);
  CYC(b_+38, b_+39);
  ret_effect(gb);
}

void loadAnimationGfxIndex_hook(GB *gb) {
  BASE(loadAnimationGfxIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x06;
  CALL_C(b_+2, multiplyAByC_hook, SYM(multiplyAByC), b_+5);
  CYC(b_+5, b_+8); SET_BC(SYM(animationGfxHeaders));
  CYC(b_+8, b_+9); alu_add_hl(gb, BC);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); D = A;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); E = A;
  CYC(b_+15, b_+16); push_effect(gb, DE);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); D = A;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); E = A;
  CYC(b_+20, b_+21); B = mem_rd(gb, HL);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+25);
  TAIL(queueDmaTransfer);
}

void updateAnimationDataPointer_hook(GB *gb) {
  BASE(updateAnimationDataPointer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wAnimationState);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto load_data; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
load_data:
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CYC(b_+10, b_+11); SET_HL(HL + 1);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); H = mem_rd(gb, HL);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+15); E = mem_rd(gb, HL);
  CYC(b_+15, b_+16); SET_HL(HL + 1);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+19); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto save_data; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); C = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_add_hl(gb, BC);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
save_data:
  CYC(b_+25, b_+26); C = L;
  CYC(b_+26, b_+27); B = H;
  CYC(b_+27, b_+28); SET_HL(pop_effect(gb));
  CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, C);
  CYC(b_+30, b_+31); SET_HL(HL + 1);
  CYC(b_+31, b_+32); mem_wr(gb, HL, B);
  CYC(b_+32, b_+33); B = E;
  CYC(b_+33, b_+36); A = mem_rd(gb, wAnimationQueueTail);
  CYC(b_+36, b_+37); A = alu_inc8(gb, A);
  CYC(b_+37, b_+39); alu_and(gb, 0x1f);
  CYC(b_+39, b_+40); E = A;
  CYC(b_+40, b_+43); A = mem_rd(gb, wAnimationQueueHead);
  CYC(b_+43, b_+44); alu_cp(gb, E);
  if (F & FZ) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+46); A = E;
  CYC(b_+46, b_+49); mem_wr(gb, wAnimationQueueTail, A);
  CYC(b_+49, b_+51); A = 0x02;
  CYC(b_+51, b_+53); mem_wr(gb, 0xff70, A);
  CYC(b_+53, b_+54); A = E;
  CYC(b_+54, b_+57); SET_HL(w2AnimationQueue);
  add_a_to_hl(gb, b_+57);
  CYC(b_+58, b_+59); mem_wr(gb, HL, B);
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+62); mem_wr(gb, 0xff70, A);
  CYC(b_+62, b_+63); alu_or(gb, H);
  CYC(b_+63, b_+64);
  ret_effect(gb);
}

void updateAnimationData_hook(GB *gb) {
  BASE(updateAnimationData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wAnimationCounter1);
  CYC(b_+3, b_+6); A = mem_rd(gb, wAnimationState);
  CYC(b_+6, b_+8); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(b_+8, updateAnimationDataPointer_hook, SYM(updateAnimationDataPointer), b_+11); else CYC(b_+8, b_+11);
  CYC(b_+11, b_+14); SET_HL(wAnimationCounter2);
  CYC(b_+14, b_+17); A = mem_rd(gb, wAnimationState);
  CYC(b_+17, b_+19); alu_bit(gb, 1, A);
  if (!(F & FZ)) CALL_C_CC(b_+19, updateAnimationDataPointer_hook, SYM(updateAnimationDataPointer), b_+22); else CYC(b_+19, b_+22);
  CYC(b_+22, b_+25); SET_HL(wAnimationCounter3);
  CYC(b_+25, b_+28); A = mem_rd(gb, wAnimationState);
  CYC(b_+28, b_+30); alu_bit(gb, 2, A);
  if (!(F & FZ)) CALL_C_CC(b_+30, updateAnimationDataPointer_hook, SYM(updateAnimationDataPointer), b_+33); else CYC(b_+30, b_+33);
  CYC(b_+33, b_+36); SET_HL(wAnimationCounter4);
  CYC(b_+36, b_+39); A = mem_rd(gb, wAnimationState);
  CYC(b_+39, b_+41); alu_bit(gb, 3, A);
  if (!(F & FZ)) CALL_C_CC(b_+41, updateAnimationDataPointer_hook, SYM(updateAnimationDataPointer), b_+44); else CYC(b_+41, b_+44);
  CYC(b_+44, b_+47); A = mem_rd(gb, wAnimationState);
  CYC(b_+47, b_+49); alu_and(gb, 0x7f);
  CYC(b_+49, b_+52); mem_wr(gb, wAnimationState, A);
  CYC(b_+52, b_+53);
  ret_effect(gb);
}
