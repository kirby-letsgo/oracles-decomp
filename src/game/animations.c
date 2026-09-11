#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

void initializeAnimations__locFunc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x58fd, updateAnimationData_hook, 0x5942, 0x5900);
  for (;;) {
    CALL_C(0x5900, updateAnimationQueue_hook, 0x591b, 0x5903);
    if (!(F & FZ)) { CYCT(0x5903, 0x5905); continue; }
    CYC(0x5903, 0x5905);
    CYC(0x5905, 0x5906);
    ret_effect(gb);
    return;
  }
}

void initializeAnimations_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58e4, 0x58e7); A = mem_rd(gb, 0xcd25);
  CYC(0x58e7, 0x58e9); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x58e9, 0x58ea); ret_effect(gb); return; }
  CYC(0x58e9, 0x58ea);
  CALL_C(0x58ea, loadAnimationData_hook, 0x3659, 0x58ed);
  CALL_C(0x58ed, initializeAnimations__locFunc_hook, 0x58fd, 0x58f0);
  CYC(0x58f0, 0x58f3); SET_HL(0xcd30);
  CYC(0x58f3, 0x58f5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CALL_C(0x58f5, initializeAnimations__locFunc_hook, 0x58fd, 0x58f8);
  CYC(0x58f8, 0x58fb); SET_HL(0xcd30);
  CYC(0x58fb, 0x58fd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  initializeAnimations__locFunc_hook(gb);
}

void updateAnimations_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5906, 0x5909); SET_HL(0xcd30);
  CYC(0x5909, 0x590b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x40));
  CYC(0x590b, 0x590e); A = mem_rd(gb, 0xcd25);
  CYC(0x590e, 0x590f); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x590f, 0x5910); ret_effect(gb); return; }
  CYC(0x590f, 0x5910);
  CYC(0x5910, 0x5913); A = mem_rd(gb, 0xcd00);
  CYC(0x5913, 0x5915); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x5915, 0x5916); ret_effect(gb); return; }
  CYC(0x5915, 0x5916);
  CALL_C(0x5916, updateAnimationQueue_hook, 0x591b, 0x5919);
  CYC(0x5919, 0x591b);
  updateAnimationData_hook(gb);
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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x591b, 0x591e); A = mem_rd(gb, 0xcce4);
  CYC(0x591e, 0x591f); B = A;
  CYC(0x591f, 0x5922); A = mem_rd(gb, 0xcce5);
  CYC(0x5922, 0x5923); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x5923, 0x5924); ret_effect(gb); return; }
  CYC(0x5923, 0x5924);
  CYC(0x5924, 0x5925); B = alu_inc8(gb, B);
  CYC(0x5925, 0x5926); A = B;
  CYC(0x5926, 0x5928); alu_and(gb, 0x1f);
  CYC(0x5928, 0x592b); mem_wr(gb, 0xcce4, A);
  CYC(0x592b, 0x592e); SET_HL(0xdb90);
  add_a_to_hl(gb, 0x592e);
  CYC(0x592f, 0x5931); A = 0x02;
  CYC(0x5931, 0x5933); mem_wr(gb, 0xff70, A);
  CYC(0x5933, 0x5934); B = mem_rd(gb, HL);
  CYC(0x5934, 0x5935); alu_xor(gb, A);
  CYC(0x5935, 0x5937); mem_wr(gb, 0xff70, A);
  CYC(0x5937, 0x5938); A = B;
  CALL_C(0x5938, loadAnimationGfxIndex_hook, 0x59b7, 0x593b);
  CYC(0x593b, 0x593e); SET_HL(0xcd30);
  CYC(0x593e, 0x5940); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(0x5940, 0x5941); alu_or(gb, H);
  CYC(0x5941, 0x5942);
  ret_effect(gb);
}

void loadAnimationGfxIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x59b7, 0x59b9); C = 0x06;
  CALL_C(0x59b9, multiplyAByC_hook, 0x019d, 0x59bc);
  CYC(0x59bc, 0x59bf); SET_BC(0x5be9);
  CYC(0x59bf, 0x59c0); alu_add_hl(gb, BC);
  CYC(0x59c0, 0x59c1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59c1, 0x59c2); C = A;
  CYC(0x59c2, 0x59c3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59c3, 0x59c4); D = A;
  CYC(0x59c4, 0x59c5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59c5, 0x59c6); E = A;
  CYC(0x59c6, 0x59c7); push_effect(gb, DE);
  CYC(0x59c7, 0x59c8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59c8, 0x59c9); D = A;
  CYC(0x59c9, 0x59ca); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x59ca, 0x59cb); E = A;
  CYC(0x59cb, 0x59cc); B = mem_rd(gb, HL);
  CYC(0x59cc, 0x59cd); SET_HL(pop_effect(gb));
  CYC(0x59cd, 0x59d0);
  queueDmaTransfer_hook(gb);
}

void updateAnimationDataPointer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5977, 0x597a); A = mem_rd(gb, 0xcd30);
  CYC(0x597a, 0x597c); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x597c, 0x597e); goto load_data; }
  CYC(0x597c, 0x597e);
  CYC(0x597e, 0x597f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x597f, 0x5980); ret_effect(gb); return; }
  CYC(0x597f, 0x5980);
load_data:
  CYC(0x5980, 0x5981); push_effect(gb, HL);
  CYC(0x5981, 0x5982); SET_HL(HL + 1);
  CYC(0x5982, 0x5983); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5983, 0x5984); H = mem_rd(gb, HL);
  CYC(0x5984, 0x5985); L = A;
  CYC(0x5985, 0x5986); E = mem_rd(gb, HL);
  CYC(0x5986, 0x5987); SET_HL(HL + 1);
  CYC(0x5987, 0x5988); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5988, 0x598a); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(0x598a, 0x598c); goto save_data; }
  CYC(0x598a, 0x598c);
  CYC(0x598c, 0x598d); B = A;
  CYC(0x598d, 0x598e); C = mem_rd(gb, HL);
  CYC(0x598e, 0x598f); alu_add_hl(gb, BC);
  CYC(0x598f, 0x5990); A = mem_rd(gb, HL); SET_HL(HL + 1);
save_data:
  CYC(0x5990, 0x5991); C = L;
  CYC(0x5991, 0x5992); B = H;
  CYC(0x5992, 0x5993); SET_HL(pop_effect(gb));
  CYC(0x5993, 0x5994); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5994, 0x5995); mem_wr(gb, HL, C);
  CYC(0x5995, 0x5996); SET_HL(HL + 1);
  CYC(0x5996, 0x5997); mem_wr(gb, HL, B);
  CYC(0x5997, 0x5998); B = E;
  CYC(0x5998, 0x599b); A = mem_rd(gb, 0xcce5);
  CYC(0x599b, 0x599c); A = alu_inc8(gb, A);
  CYC(0x599c, 0x599e); alu_and(gb, 0x1f);
  CYC(0x599e, 0x599f); E = A;
  CYC(0x599f, 0x59a2); A = mem_rd(gb, 0xcce4);
  CYC(0x59a2, 0x59a3); alu_cp(gb, E);
  if (F & FZ) { CYCT(0x59a3, 0x59a4); ret_effect(gb); return; }
  CYC(0x59a3, 0x59a4);
  CYC(0x59a4, 0x59a5); A = E;
  CYC(0x59a5, 0x59a8); mem_wr(gb, 0xcce5, A);
  CYC(0x59a8, 0x59aa); A = 0x02;
  CYC(0x59aa, 0x59ac); mem_wr(gb, 0xff70, A);
  CYC(0x59ac, 0x59ad); A = E;
  CYC(0x59ad, 0x59b0); SET_HL(0xdb90);
  add_a_to_hl(gb, 0x59b0);
  CYC(0x59b1, 0x59b2); mem_wr(gb, HL, B);
  CYC(0x59b2, 0x59b3); alu_xor(gb, A);
  CYC(0x59b3, 0x59b5); mem_wr(gb, 0xff70, A);
  CYC(0x59b5, 0x59b6); alu_or(gb, H);
  CYC(0x59b6, 0x59b7);
  ret_effect(gb);
}

void updateAnimationData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5942, 0x5945); SET_HL(0xcd31);
  CYC(0x5945, 0x5948); A = mem_rd(gb, 0xcd30);
  CYC(0x5948, 0x594a); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(0x594a, updateAnimationDataPointer_hook, 0x5977, 0x594d); else CYC(0x594a, 0x594d);
  CYC(0x594d, 0x5950); SET_HL(0xcd34);
  CYC(0x5950, 0x5953); A = mem_rd(gb, 0xcd30);
  CYC(0x5953, 0x5955); alu_bit(gb, 1, A);
  if (!(F & FZ)) CALL_C_CC(0x5955, updateAnimationDataPointer_hook, 0x5977, 0x5958); else CYC(0x5955, 0x5958);
  CYC(0x5958, 0x595b); SET_HL(0xcd37);
  CYC(0x595b, 0x595e); A = mem_rd(gb, 0xcd30);
  CYC(0x595e, 0x5960); alu_bit(gb, 2, A);
  if (!(F & FZ)) CALL_C_CC(0x5960, updateAnimationDataPointer_hook, 0x5977, 0x5963); else CYC(0x5960, 0x5963);
  CYC(0x5963, 0x5966); SET_HL(0xcd3a);
  CYC(0x5966, 0x5969); A = mem_rd(gb, 0xcd30);
  CYC(0x5969, 0x596b); alu_bit(gb, 3, A);
  if (!(F & FZ)) CALL_C_CC(0x596b, updateAnimationDataPointer_hook, 0x5977, 0x596e); else CYC(0x596b, 0x596e);
  CYC(0x596e, 0x5971); A = mem_rd(gb, 0xcd30);
  CYC(0x5971, 0x5973); alu_and(gb, 0x7f);
  CYC(0x5973, 0x5976); mem_wr(gb, 0xcd30, A);
  CYC(0x5976, 0x5977);
  ret_effect(gb);
}
