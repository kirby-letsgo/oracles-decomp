#include "game/game.h"
#include "game/gen.h"

// Rewrites of code/bank0.s. Cycles are burned from the ROM's own instruction stream (CYC/CYCT),
// which keeps interrupt dispatch on instruction boundaries; every memory access follows the burn
// of the instruction that performs it, as the transliteration does.

// memory helpers

void fillMemory(GB *gb, uint16_t dst, uint8_t value, unsigned n) {
  for (unsigned i = 0; i < n; i++) {
    CYC(0x0470, 0x0471); mem_wr(gb, dst + i, value);
    if (i + 1 < n) CYCT(0x0471, 0x0474); else CYC(0x0471, 0x0474);
  }
  CYC(0x0474, 0x0475);
}

void clearMemory(GB *gb, uint16_t dst, unsigned n) {
  CYC(0x046f, 0x0470);
  fillMemory(gb, dst, 0, n);
}

void fillMemoryBc(GB *gb, uint16_t dst, uint8_t value, unsigned n) {
  CYC(0x0476, 0x0477);
  for (unsigned i = 0; i < n; i++) {
    CYC(0x0477, 0x0479); mem_wr(gb, dst + i, value);
    if (i + 1 < n) CYCT(0x0479, 0x047e); else CYC(0x0479, 0x047e);
  }
  CYC(0x047e, 0x047f);
}

void clearMemoryBc(GB *gb, uint16_t dst, unsigned n) {
  CYC(0x0475, 0x0476);
  fillMemoryBc(gb, dst, 0, n);
}

static void copy_loop(GB *gb, uint16_t base, uint16_t dst, uint16_t src, unsigned n) {
  for (unsigned i = 0; i < n; i++) {
    CYC(base, base + 1); uint8_t v = mem_rd(gb, src + i);
    CYC(base + 1, base + 2); mem_wr(gb, dst + i, v);
    if (i + 1 < n) CYCT(base + 2, base + 6); else CYC(base + 2, base + 6);
  }
  CYC(base + 6, base + 7);
}

static void copy_bc_loop(GB *gb, uint16_t base, uint16_t dst, uint16_t src, unsigned n) {
  for (unsigned i = 0; i < n; i++) {
    CYC(base, base + 1); uint8_t v = mem_rd(gb, src + i);
    CYC(base + 1, base + 2); mem_wr(gb, dst + i, v);
    if (i + 1 < n) CYCT(base + 2, base + 8); else CYC(base + 2, base + 8);
  }
  CYC(base + 8, base + 9);
}

void copyMemory(GB *gb, uint16_t dst, uint16_t src, unsigned n) { copy_loop(gb, 0x0486, dst, src, n); }
void copyMemoryBc(GB *gb, uint16_t dst, uint16_t src, unsigned n) { copy_bc_loop(gb, 0x0496, dst, src, n); }

static void fill_done(GB *gb, unsigned n) {
  SET_HL(HL + n);
  B = 0;
  F = (F & FC) | FZ | FN;
  ret_effect(gb);
}

void fillMemory_hook(GB *gb) {
  unsigned n = B ? B : 256;
  fillMemory(gb, HL, A, n);
  fill_done(gb, n);
}

void clearMemory_hook(GB *gb) {
  unsigned n = B ? B : 256;
  clearMemory(gb, HL, n);
  A = 0;
  F = 0;
  fill_done(gb, n);
}

static void fill_bc_done(GB *gb, unsigned n) {
  SET_HL(HL + n);
  SET_BC(0);
  A = 0;
  F = FZ;
  ret_effect(gb);
}

void fillMemoryBc_hook(GB *gb) {
  unsigned n = BC ? BC : 65536;
  E = A;
  fillMemoryBc(gb, HL, A, n);
  fill_bc_done(gb, n);
}

void clearMemoryBc_hook(GB *gb) {
  unsigned n = BC ? BC : 65536;
  E = 0;
  clearMemoryBc(gb, HL, n);
  fill_bc_done(gb, n);
}

static void copy_done(GB *gb, uint16_t src, unsigned n) {
  A = mem_rd(gb, src + n - 1);
  SET_HL(HL + n);
  SET_DE(DE + n);
  B = 0;
  F = (F & FC) | FZ | FN;
  ret_effect(gb);
}

void copyMemory_hook(GB *gb) {
  unsigned n = B ? B : 256;
  copy_loop(gb, 0x0486, DE, HL, n);
  copy_done(gb, HL, n);
}

void copyMemoryReverse_hook(GB *gb) {
  unsigned n = B ? B : 256;
  copy_loop(gb, 0x047f, HL, DE, n);
  copy_done(gb, DE, n);
}

static void copy_bc_done(GB *gb, unsigned n) {
  SET_HL(HL + n);
  SET_DE(DE + n);
  SET_BC(0);
  A = 0;
  F = FZ;
  ret_effect(gb);
}

void copyMemoryBc_hook(GB *gb) {
  unsigned n = BC ? BC : 65536;
  copy_bc_loop(gb, 0x0496, DE, HL, n);
  copy_bc_done(gb, n);
}

void copyMemoryBcReverse_hook(GB *gb) {
  unsigned n = BC ? BC : 65536;
  copy_bc_loop(gb, 0x048d, HL, DE, n);
  copy_bc_done(gb, n);
}

// pointer and integer math

static void add_a_to_de(GB *gb) {
  alu_add(gb, E);
  E = A;
  if (F & FC) { CYC(0x0068, 0x006d); D = alu_inc8(gb, D); }
  else CYCT(0x0068, 0x006b);
}

void addAToDe_hook(GB *gb) {
  add_a_to_de(gb);
  ret_effect(gb);
}

static void add_a_to_bc(GB *gb) {
  alu_add(gb, C);
  C = A;
  if (F & FC) { CYC(0x006d, 0x0072); B = alu_inc8(gb, B); }
  else CYCT(0x006d, 0x0070);
}

void addAToBc_hook(GB *gb) {
  add_a_to_bc(gb);
  ret_effect(gb);
}

static uint16_t double_index(GB *gb, uint16_t base) {
  uint16_t hl = HL;
  alu_add(gb, A);
  L = A;
  A = 0;
  alu_adc(gb, A);
  H = A;
  alu_add_hl(gb, base);
  uint16_t r = HL;
  SET_HL(hl);
  return r;
}

void addDoubleIndexToDe_hook(GB *gb) { SET_DE(double_index(gb, DE)); CYC(0x0072, 0x007e); ret_effect(gb); }
void addDoubleIndexToBc_hook(GB *gb) { SET_BC(double_index(gb, BC)); CYC(0x007e, 0x008a); ret_effect(gb); }

void getNumSetBits_hook(GB *gb) {
  uint8_t a = A, b = 0;
  CYC(0x0176, 0x0178);
  do {
    bool carry = a & 0x80;
    a = (uint8_t)(a << 1);
    if (carry) { CYC(0x0178, 0x017c); b++; }
    else CYCT(0x0178, 0x017b);
    if (a) CYCT(0x017c, 0x017f); else CYC(0x017c, 0x017f);
  } while (a);
  A = B = b;
  F = FZ;
  CYC(0x017f, 0x0181);
  ret_effect(gb);
}

void addDecimalToHlRef_hook(GB *gb) {
  uint16_t p = HL;
  CYC(0x0181, 0x0182); A = mem_rd(gb, p); alu_add(gb, C); alu_daa(gb);
  CYC(0x0182, 0x0185); mem_wr(gb, p, A);
  CYC(0x0185, 0x0186); A = mem_rd(gb, p + 1); alu_adc(gb, B); alu_daa(gb);
  CYC(0x0186, 0x0189); mem_wr(gb, p + 1, A);
  if (F & FC) {
    CYC(0x0189, 0x018d); A = 0x63; mem_wr(gb, p, A);
    CYC(0x018d, 0x018e); mem_wr(gb, p + 1, A);
    CYC(0x018e, 0x018f);
  } else CYCT(0x0189, 0x018a);
  ret_effect(gb);
}

static void sub_decimal_from_hl_ref(GB *gb) {
  uint16_t p = HL;
  CYC(0x018f, 0x0190); A = mem_rd(gb, p); alu_sub(gb, C); alu_daa(gb);
  CYC(0x0190, 0x0193); mem_wr(gb, p, A);
  CYC(0x0193, 0x0194); A = mem_rd(gb, p + 1); alu_sbc(gb, B); alu_daa(gb);
  CYC(0x0194, 0x0197); mem_wr(gb, p + 1, A);
  if (F & FC) {
    alu_xor(gb, A);
    CYC(0x0197, 0x019a); mem_wr(gb, p, 0);
    CYC(0x019a, 0x019b); mem_wr(gb, p + 1, 0);
    alu_scf(gb);
    CYC(0x019b, 0x019d);
  } else CYCT(0x0197, 0x0198);
}

void subDecimalFromHlRef_hook(GB *gb) {
  sub_decimal_from_hl_ref(gb);
  ret_effect(gb);
}

void multiplyAByC_hook(GB *gb) {
  CYC(0x019d, 0x01a3);
  E = 8; B = 0; SET_HL(0);
  do {
    alu_add_hl(gb, HL);
    alu_add(gb, A);
    if (F & FC) { CYC(0x01a3, 0x01a8); alu_add_hl(gb, BC); }
    else CYCT(0x01a3, 0x01a7);
    E = alu_dec8(gb, E);
    if (E) CYCT(0x01a8, 0x01ab); else CYC(0x01a8, 0x01ab);
  } while (E);
  CYC(0x01ab, 0x01ac);
  ret_effect(gb);
}

void multiplyABy16_hook(GB *gb) {
  uint8_t a = A;
  C = (uint8_t)(a << 4);
  A = B = a >> 4;
  F = (A == 0 ? FZ : 0) | FH;
  CYC(0x01ac, 0x01b7);
  ret_effect(gb);
}

void multiplyABy8_hook(GB *gb) {
  uint8_t a = A;
  C = (uint8_t)(a << 3);
  A = B = a >> 5;
  F = (A == 0 ? FZ : 0) | FH;
  CYC(0x01b7, 0x01c3);
  ret_effect(gb);
}

void multiplyABy4_hook(GB *gb) {
  uint8_t a = A;
  B = a >> 6;
  A = C = (uint8_t)(a << 2);
  F = B == 0 ? FZ : 0;
  CYC(0x01c3, 0x01cd);
  ret_effect(gb);
}

void s8ToS16_hook(GB *gb) {
  B = 0xff;
  alu_bit(gb, 7, A);
  if (A & 0x80) CYCT(0x01cd, 0x01d3);
  else { CYC(0x01cd, 0x01d4); B = alu_inc8(gb, B); }
  C = A;
  CYC(0x01d4, 0x01d6);
  ret_effect(gb);
}

static void compare_hl_to_bc(GB *gb) {
  A = H; alu_cp(gb, B);
  if (F & FC) { CYCT(0x01d6, 0x01da); A = 0xff; CYC(0x01e4, 0x01e7); }
  else if (!(F & FZ)) { CYC(0x01d6, 0x01da); CYCT(0x01da, 0x01dc); A = 0x01; CYC(0x01e7, 0x01ea); }
  else {
    CYC(0x01d6, 0x01de);
    A = L; alu_cp(gb, C);
    if (F & FC) { CYCT(0x01de, 0x01e0); A = 0xff; CYC(0x01e4, 0x01e7); }
    else if (!(F & FZ)) { CYC(0x01de, 0x01e0); CYCT(0x01e0, 0x01e2); A = 0x01; CYC(0x01e7, 0x01ea); }
    else { CYC(0x01de, 0x01e4); alu_xor(gb, A); }
  }
}

void compareHlToBc_hook(GB *gb) {
  compare_hl_to_bc(gb);
  ret_effect(gb);
}

void getHighestSetBit_hook(GB *gb) {
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x01ea, 0x01ec); ret_effect(gb); return; }
  CYC(0x01ea, 0x01ef);
  uint8_t c = 0xff;
  do {
    c++;
    A = alu_srl(gb, A);
    if (A) CYCT(0x01ef, 0x01f4); else CYC(0x01ef, 0x01f4);
  } while (A);
  A = c;
  alu_scf(gb);
  CYC(0x01f4, 0x01f8);
  ret_effect(gb);
}

void getLowestSetBit_hook(GB *gb) {
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x01f8, 0x01fa); ret_effect(gb); return; }
  CYC(0x01f8, 0x01fd);
  uint8_t c = 8;
  do {
    c--;
    alu_add(gb, A);
    if (A) CYCT(0x01fd, 0x0201); else CYC(0x01fd, 0x0201);
  } while (A);
  A = c;
  alu_scf(gb);
  CYC(0x0201, 0x0205);
  ret_effect(gb);
}

// flags: one bit per flag, base address in hl, flag index in a

static void flag_hlpr(GB *gb) {
  B = A;
  alu_and(gb, 0xf8);
  alu_rlca(gb);
  alu_swap_a(gb);
  C = A;
  A = B;
  B = 0;
  alu_add_hl(gb, BC);
  alu_and(gb, 0x07);
  SET_BC(0x00f8);
  alu_add(gb, C);
  C = A;
  A = mem_rd(gb, BC);
  CYC(0x0223, 0x0237);
}

void _flagHlpr_hook(GB *gb) {
  flag_hlpr(gb);
  ret_effect(gb);
}

void checkFlag_hook(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x0205, 0x020a);
  flag_hlpr(gb);
  CYC(0x020a, 0x020b); alu_and(gb, mem_rd(gb, HL));
  SET_BC(bc); SET_HL(hl);
  CYC(0x020b, 0x020e);
  ret_effect(gb);
}

void setFlag_hook(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x020e, 0x0213);
  flag_hlpr(gb);
  CYC(0x0213, 0x0214); alu_or(gb, mem_rd(gb, HL));
  CYC(0x0214, 0x0215); mem_wr(gb, HL, A);
  SET_BC(bc); SET_HL(hl);
  CYC(0x0215, 0x0218);
  ret_effect(gb);
}

static void unset_flag(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x0218, 0x021d);
  flag_hlpr(gb);
  alu_cpl(gb);
  CYC(0x021d, 0x021f); alu_and(gb, mem_rd(gb, HL));
  CYC(0x021f, 0x0220); mem_wr(gb, HL, A);
  SET_BC(bc); SET_HL(hl);
  CYC(0x0220, 0x0223);
}

void unsetFlag_hook(GB *gb) {
  unset_flag(gb);
  ret_effect(gb);
}

// saturating counters

static void dec16_cap(GB *gb, uint16_t p) {
  CYC(0x0237, 0x0239); A = mem_rd(gb, p + 1);
  CYC(0x0239, 0x023a); alu_or(gb, mem_rd(gb, p));
  if (F & FZ) { CYCT(0x023a, 0x023b); return; }
  CYC(0x023a, 0x023c); A = mem_rd(gb, p); alu_sub(gb, 1);
  CYC(0x023c, 0x023f); mem_wr(gb, p, A);
  CYC(0x023f, 0x0240); A = mem_rd(gb, p + 1); alu_sbc(gb, 0);
  CYC(0x0240, 0x0243); mem_wr(gb, p + 1, A);
  CYC(0x0243, 0x0244); alu_or(gb, mem_rd(gb, p));
  CYC(0x0244, 0x0245);
}

void decHlRef16WithCap_hook(GB *gb) {
  dec16_cap(gb, HL);
  ret_effect(gb);
}

void dec16_ff8c_hook(GB *gb) {
  uint16_t hl = HL;
  CYC(0x0878, 0x087f);
  dec16_cap(gb, hFF8C);
  SET_HL(hl);
  CYC(0x087f, 0x0881);
  ret_effect(gb);
}

void incHlRefWithCap_hook(GB *gb) {
  CYC(0x0245, 0x0246); uint8_t v = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) CYCT(0x0246, 0x0247);
  else { CYC(0x0246, 0x0249); mem_wr(gb, HL, 0xff); CYC(0x0249, 0x024a); }
  ret_effect(gb);
}

void incHlRef16WithCap_hook(GB *gb) {
  CYC(0x024a, 0x024b); uint8_t lo = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, lo);
  if (lo) { CYCT(0x024b, 0x024c); ret_effect(gb); return; }
  CYC(0x024b, 0x024e); uint8_t hi = alu_inc8(gb, mem_rd(gb, HL + 1)); mem_wr(gb, HL + 1, hi);
  if (hi) { CYC(0x024e, 0x0252); ret_effect(gb); return; }
  CYCT(0x024e, 0x0250);
  CYC(0x0252, 0x0256); mem_wr(gb, HL + 1, 0xff);
  CYC(0x0256, 0x0257); mem_wr(gb, HL, 0xff);
  CYC(0x0257, 0x0259);
  ret_effect(gb);
}

void hexToDec_hook(GB *gb) {
  CYC(0x0259, 0x025c);
  SET_BC(0);
  for (;;) {
    alu_cp(gb, 0x64);
    if (F & FC) { CYCT(0x025c, 0x0260); break; }
    CYC(0x025c, 0x0265);
    alu_sub(gb, 0x64);
    B = alu_inc8(gb, B);
  }
  for (;;) {
    alu_cp(gb, 0x0a);
    if (F & FC) { CYCT(0x0265, 0x0268); break; }
    CYC(0x0265, 0x026d);
    alu_sub(gb, 0x0a);
    C = alu_inc8(gb, C);
  }
  ret_effect(gb);
}

// random numbers: hRng1:hRng2 is a 16-bit state, advanced by x3 plus the low byte. The VBlank
// handler advances it too, so each access sits at its exact cycle.

uint8_t getRandomNumber(GB *gb) {
  CYC(0x043e, 0x0442); uint8_t lo = H8(hRng1);
  CYC(0x0442, 0x0446); uint8_t hi = H8(hRng2);
  uint16_t v = (uint16_t)(((hi << 8) | lo) * 3);
  CYC(0x0446, 0x044d); H8(hRng2) = v >> 8;
  CYC(0x044d, 0x0450); H8(hRng1) = (uint8_t)((v >> 8) + lo);
  CYC(0x0450, 0x0453);
  return H8(hRng1);
}

uint8_t getRandomNumber_noPreserveVars(GB *gb) {
  CYC(0x0453, 0x0455); uint8_t lo = H8(hRng1);
  CYC(0x0455, 0x0459); uint8_t hi = H8(hRng2);
  uint16_t v = (uint16_t)(((hi << 8) | lo) * 3);
  CYC(0x0459, 0x0460); H8(hRng2) = v >> 8;
  CYC(0x0460, 0x0463); H8(hRng1) = (uint8_t)((v >> 8) + lo);
  CYC(0x0463, 0x0464);
  return H8(hRng1);
}

void getRandomNumber_hook(GB *gb) {
  uint8_t lo = H8(hRng1);
  getRandomNumber(gb);
  A = H8(hRng2);
  alu_add(gb, lo);
  ret_effect(gb);
}

void getRandomNumber_noPreserveVars_hook(GB *gb) {
  uint8_t lo = H8(hRng1), hi = H8(hRng2);
  getRandomNumber_noPreserveVars(gb);
  B = hi;
  C = lo;
  SET_HL((uint16_t)(((hi << 8) | lo) * 3));
  A = H8(hRng2);
  alu_add(gb, lo);
  ret_effect(gb);
}

void getRandomIndexFromProbabilityDistribution_hook(GB *gb) {
  B = 0;
  CYC(0x0464, 0x0469);
  A = getRandomNumber(gb);
  for (;;) {
    CYC(0x0469, 0x046a); alu_sub(gb, mem_rd(gb, HL));
    if (F & FC) { CYCT(0x046a, 0x046b); break; }
    CYC(0x046a, 0x046f);
    SET_HL(HL + 1);
    B++;
  }
  ret_effect(gb);
}

// LCD and VRAM setup

void clearOam(GB *gb) {
  CYC(0x049f, 0x04a2); H8(hOamTail) = 0;
  CYC(0x04a2, 0x04a6);
  for (int i = 0; i < 40; i++) {
    CYC(0x04a6, 0x04a8); mem_wr(gb, wOam + i * 4, 0xe0);
    if (i < 39) CYCT(0x04a8, 0x04ae); else CYC(0x04a8, 0x04ae);
  }
  CYC(0x04ae, 0x04af);
}

void clearOam_hook(GB *gb) {
  clearOam(gb);
  A = 0xa0;
  B = 0xe0;
  SET_HL(wOam + 0x9c);
  F = FZ | FN;
  ret_effect(gb);
}

bool disableLcd(GB *gb) {
  CYC(0x02c1, 0x02c3); uint8_t lcdc = mem_rd(gb, IO_LCDC);
  if (!(lcdc & 0x80)) { CYCT(0x02c3, 0x02c5); return false; }
  CYC(0x02c3, 0x02c8); uint8_t ie = mem_rd(gb, IO_IE);
  CYC(0x02c8, 0x02cd); mem_wr(gb, IO_IE, ie & 0xfe);
  for (;;) {
    CYC(0x02cd, 0x02cf); uint8_t ly = mem_rd(gb, IO_LY);
    if (ly >= 0x91) { CYC(0x02cf, 0x02d3); break; }
    CYCT(0x02cf, 0x02d3);
  }
  CYC(0x02d3, 0x02d7); H8(hNextLcdInterruptBehaviour) = 3;
  CYC(0x02d7, 0x02db); W8(wGfxRegsFinal) = 0;
  CYC(0x02db, 0x02de); W8(wGfxRegs2) = 0;
  CYC(0x02de, 0x02e1); W8(wGfxRegs1) = 0;
  CYC(0x02e1, 0x02e3); mem_wr(gb, IO_LCDC, 0);
  CYC(0x02e3, 0x02e5); mem_wr(gb, IO_IF, 0);
  CYC(0x02e5, 0x02e8); mem_wr(gb, IO_IE, ie);
  CYC(0x02e8, 0x02ea);
  return true;
}

void disableLcd_hook(GB *gb) {
  if (disableLcd(gb)) { A = mem_rd(gb, IO_IE); F = FZ; }
  else { A = mem_rd(gb, IO_LCDC); alu_rlca(gb); }
  ret_effect(gb);
}

void clearVram(GB *gb) {
  CYC(0x04af, 0x04b2); disableLcd(gb);
  CYC(0x04b2, 0x04b5); clearOam(gb);
  CYC(0x04b5, 0x04b9); mem_wr(gb, IO_VBK, 1);
  CYC(0x04b9, 0x04c2); clearMemoryBc(gb, 0x8000, 0x2000);
  CYC(0x04c2, 0x04c5); mem_wr(gb, IO_VBK, 0);
  CYC(0x04c5, 0x04cd); clearMemoryBc(gb, 0x8000, 0x2000);
}

static void clear_bc_done(GB *gb, uint16_t end) {
  SET_HL(end);
  SET_BC(0);
  A = 0;
  E = 0;
  F = FZ;
  ret_effect(gb);
}

void clearVram_hook(GB *gb) {
  clearVram(gb);
  clear_bc_done(gb, 0xa000);
}

void initializeVramMap0(GB *gb) {
  CYC(0x04d0, 0x04d3); disableLcd(gb);
  CYC(0x04d3, 0x04d7); mem_wr(gb, IO_VBK, 1);
  CYC(0x04d7, 0x04e2); fillMemoryBc(gb, 0x9800, 0x80, 0x400);
  CYC(0x04e2, 0x04e5); mem_wr(gb, IO_VBK, 0);
  CYC(0x04e5, 0x04ed); clearMemoryBc(gb, 0x9800, 0x400);
}

void initializeVramMap1(GB *gb) {
  CYC(0x04ed, 0x04f0); disableLcd(gb);
  CYC(0x04f0, 0x04f4); mem_wr(gb, IO_VBK, 1);
  CYC(0x04f4, 0x04ff); fillMemoryBc(gb, 0x9c00, 0x80, 0x400);
  CYC(0x04ff, 0x0502); mem_wr(gb, IO_VBK, 0);
  CYC(0x0502, 0x050b); clearMemoryBc(gb, 0x9c00, 0x400);
}

void initializeVramMaps(GB *gb) {
  CYC(0x04cd, 0x04d0);
  initializeVramMap1(gb);
  initializeVramMap0(gb);
}

void initializeVramMap0_hook(GB *gb) { initializeVramMap0(gb); clear_bc_done(gb, 0x9c00); }
void initializeVramMap1_hook(GB *gb) { initializeVramMap1(gb); clear_bc_done(gb, 0xa000); }
void initializeVramMaps_hook(GB *gb) { initializeVramMaps(gb); clear_bc_done(gb, 0x9c00); }

// sequential ROM reads across bank boundaries

void readByteSequential_hook(GB *gb) {
  CYC(0x0788, 0x0789); uint8_t v = mem_rd(gb, HL);
  SET_HL(HL + 1);
  alu_bit(gb, 7, H);
  if (!(H & 0x80)) { CYCT(0x0789, 0x078c); A = v; ret_effect(gb); return; }
  CYC(0x0789, 0x078f); H = 0x40;
  CYC(0x078f, 0x0791); uint8_t bank = (uint8_t)(H8(hRomBank) + 1);
  CYC(0x0791, 0x0794); H8(hRomBank) = bank;
  CYC(0x0794, 0x0797); mem_wr(gb, MBC_ROM_BANK, bank);
  CYC(0x0797, 0x0799);
  A = v;
  ret_effect(gb);
}

// misc

static void add16_bit_refs(GB *gb) {
  CYC(0x23a7, 0x23a8); A = mem_rd(gb, DE);
  CYC(0x23a8, 0x23a9); alu_add(gb, mem_rd(gb, HL));
  CYC(0x23a9, 0x23aa); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  SET_HL(HL + 1);
  CYC(0x23aa, 0x23ad); A = mem_rd(gb, DE);
  CYC(0x23ad, 0x23ae); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x23ae, 0x23af); mem_wr(gb, DE, A);
  CYC(0x23af, 0x23b0);
}

void add16BitRefs_hook(GB *gb) {
  add16_bit_refs(gb);
  ret_effect(gb);
}

void cpActiveRing_hook(GB *gb) {
  CYC(0x23b0, 0x23b5); alu_cp(gb, W8(wActiveRing));
  CYC(0x23b5, 0x23b7);
  ret_effect(gb);
}

void disableActiveRing_hook(GB *gb) {
  CYC(0x23b7, 0x23bd); W8(wActiveRing) |= 0x40;
  CYC(0x23bd, 0x23bf);
  ret_effect(gb);
}

void enableActiveRing_hook(GB *gb) {
  CYC(0x23bf, 0x23c4); A = W8(wActiveRing);
  alu_cp(gb, 0xff);
  if (F & FZ) CYCT(0x23c4, 0x23c8);
  else { CYC(0x23c4, 0x23ca); W8(wActiveRing) &= (uint8_t)~0x40; }
  CYC(0x23ca, 0x23cc);
  ret_effect(gb);
}

static void obj_dec_body(GB *gb, uint16_t addr, uint8_t off) {
  CYC(addr, addr + 3); SET_HL((uint16_t)((D << 8) | off));
  CYC(addr + 3, addr + 4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(addr + 4, addr + 5);
}

static void obj_dec(GB *gb, uint16_t addr, uint8_t off) {
  obj_dec_body(gb, addr, off);
  ret_effect(gb);
}

static void obj_inc(GB *gb, uint16_t addr, uint8_t off) {
  CYC(addr, addr + 3); SET_HL((uint16_t)((D << 8) | off));
  CYC(addr + 3, addr + 4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(addr + 4, addr + 5);
  ret_effect(gb);
}

void interactionDecCounter1_hook(GB *gb) { obj_dec(gb, 0x23cc, INTERACTION_BASE + OBJ_COUNTER1); }
void interactionDecCounter2_hook(GB *gb) { obj_dec(gb, 0x23d1, INTERACTION_BASE + OBJ_COUNTER2); }
void itemDecCounter1_hook(GB *gb) { obj_dec(gb, 0x23d6, OBJ_COUNTER1); }
void itemDecCounter2_hook(GB *gb) { obj_dec(gb, 0x23db, OBJ_COUNTER2); }
void interactionIncState_hook(GB *gb) { obj_inc(gb, 0x23e0, INTERACTION_BASE + OBJ_STATE); }
void interactionIncSubstate_hook(GB *gb) { obj_inc(gb, 0x23e5, INTERACTION_BASE + OBJ_SUBSTATE); }
void itemIncState_hook(GB *gb) { obj_inc(gb, 0x23ea, OBJ_STATE); }
void itemIncSubstate_hook(GB *gb) { obj_inc(gb, 0x23ef, OBJ_SUBSTATE); }

// object visibility (Object.visible at +0x1a of the active object)

static void obj_set_visible(GB *gb, uint16_t addr, uint8_t v) {
  CYC(addr, addr + 2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_VISIBLE);
  E = A;
  A = v;
  CYC(addr + 2, addr + 8); mem_wr(gb, DE, A);
  CYC(addr + 8, addr + 9);
  ret_effect(gb);
}

void objectSetVisiblec0_hook(GB *gb) { obj_set_visible(gb, 0x1e33, 0xc0); }
void objectSetVisiblec1_hook(GB *gb) { obj_set_visible(gb, 0x1e3c, 0xc1); }
void objectSetVisiblec2_hook(GB *gb) { obj_set_visible(gb, 0x1e45, 0xc2); }
void objectSetVisiblec3_hook(GB *gb) { obj_set_visible(gb, 0x1e4e, 0xc3); }
void objectSetVisible80_hook(GB *gb) { obj_set_visible(gb, 0x1e57, 0x80); }
void objectSetVisible81_hook(GB *gb) { obj_set_visible(gb, 0x1e60, 0x81); }
void objectSetVisible82_hook(GB *gb) { obj_set_visible(gb, 0x1e69, 0x82); }
void objectSetVisible83_hook(GB *gb) { obj_set_visible(gb, 0x1e72, 0x83); }

static void obj_visible_bit(GB *gb, uint16_t addr, bool set) {
  CYC(addr, addr + 2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_VISIBLE);
  L = A;
  H = D;
  CYC(addr + 2, addr + 8); uint8_t v = mem_rd(gb, HL);
  mem_wr(gb, HL, set ? (uint8_t)(v | 0x80) : (uint8_t)(v & 0x7f));
  CYC(addr + 8, addr + 9);
}

void objectSetInvisible_hook(GB *gb) { obj_visible_bit(gb, 0x1e7b, false); ret_effect(gb); }
void objectSetVisible_hook(GB *gb) { obj_visible_bit(gb, 0x1e84, true); ret_effect(gb); }

void objectSetReservedBit1_hook(GB *gb) {
  CYC(0x1e8d, 0x1e8f); A = H8(hActiveObjectType);
  L = A;
  H = D;
  CYC(0x1e8f, 0x1e93); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(0x1e93, 0x1e94);
  ret_effect(gb);
}

void checkInteractionState_hook(GB *gb) {
  E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x23fe, 0x2401); A = mem_rd(gb, DE);
  alu_or(gb, A);
  CYC(0x2401, 0x2403);
  ret_effect(gb);
}

// room flags

static void add_a_to_hl(GB *gb) {
  alu_add(gb, L);
  L = A;
  if (F & FC) { CYC(0x0010, 0x0013); H = alu_inc8(gb, H); CYC(0x0013, 0x0015); }
  else CYCT(0x0010, 0x0013);
}

static void room_flags(GB *gb, uint8_t group, uint8_t room) {
  CYC(0x198a, 0x198e); SET_HL(flagLocationGroupTable);
  A = group;
  add_a_to_hl(gb);
  CYC(0x198e, 0x198f); H = mem_rd(gb, HL);
  L = room;
  CYC(0x198f, 0x1991); A = mem_rd(gb, HL);
  CYC(0x1991, 0x1992);
}

void getRoomFlags_hook(GB *gb) {
  room_flags(gb, A, B);
  ret_effect(gb);
}

static void a_room_flags(GB *gb, uint8_t room) {
  uint16_t bc = BC;
  CYC(0x1980, 0x1985); uint8_t group = W8(wActiveGroup);
  CYC(0x1985, 0x1988);
  room_flags(gb, group, room);
  SET_BC(bc);
  CYC(0x1988, 0x198a);
}

void getARoomFlags_hook(GB *gb) {
  a_room_flags(gb, A);
  ret_effect(gb);
}

static void this_room_flags(GB *gb) {
  CYC(0x197d, 0x1980); uint8_t room = W8(wActiveRoom);
  a_room_flags(gb, room);
}

void getThisRoomFlags_hook(GB *gb) {
  this_room_flags(gb);
  ret_effect(gb);
}

void checkIsLinkedGame_hook(GB *gb) {
  CYC(0x1992, 0x1995); A = W8(wIsLinkedGame);
  alu_or(gb, A);
  CYC(0x1995, 0x1997);
  ret_effect(gb);
}

// table searches

static void find_byte_at_hl(GB *gb) {
  for (;;) {
    CYC(0x1e17, 0x1e18); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_or(gb, A);
    if (F & FZ) { CYCT(0x1e18, 0x1e1a); break; }
    alu_cp(gb, E);
    if (!(F & FZ)) { CYCT(0x1e18, 0x1e1d); continue; }
    alu_scf(gb);
    CYC(0x1e18, 0x1e1f);
    break;
  }
}

void findByteAtHl_hook(GB *gb) {
  find_byte_at_hl(gb);
  ret_effect(gb);
}

static void lookup_key(GB *gb) {
  for (;;) {
    CYC(0x1e06, 0x1e07); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_or(gb, A);
    if (F & FZ) { CYCT(0x1e07, 0x1e09); break; }
    alu_cp(gb, E);
    bool found = F & FZ;
    CYC(0x1e07, 0x1e0b); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!found) { CYCT(0x1e0b, 0x1e0d); continue; }
    alu_scf(gb);
    CYC(0x1e0b, 0x1e0f);
    break;
  }
}

void lookupKey_hook(GB *gb) {
  lookup_key(gb);
  ret_effect(gb);
}

// consumable counters (BCD)

void setStatusBarNeedsRefreshBit1(GB *gb) {
  CYC(0x17d8, 0x17de); W8(wStatusBarNeedsRefresh) |= 0x02;
  CYC(0x17de, 0x17e0);
}

void setStatusBarNeedsRefreshBit1_hook(GB *gb) {
  setStatusBarNeedsRefreshBit1(gb);
  ret_effect(gb);
}

static void dec_bcd_count(GB *gb) {
  CYC(0x17cb, 0x17cc); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x17cc, 0x17ce); return; }
  CYC(0x17cc, 0x17d1);
  setStatusBarNeedsRefreshBit1(gb);
  CYC(0x17d1, 0x17d2); A = mem_rd(gb, HL);
  alu_sub(gb, 1);
  alu_daa(gb);
  CYC(0x17d2, 0x17d6); mem_wr(gb, HL, A);
  alu_or(gb, H);
  CYC(0x17d6, 0x17d8);
}

void decNumActiveSeeds_hook(GB *gb) {
  alu_and(gb, 0x07);
  SET_HL(wNumEmberSeeds);
  CYC(0x17bb, 0x17c1);
  add_a_to_hl(gb);
  CYC(0x17c1, 0x17c3);
  dec_bcd_count(gb);
  ret_effect(gb);
}

void decNumBombchus_hook(GB *gb) {
  SET_HL(wNumBombchus);
  CYC(0x17c3, 0x17c8);
  dec_bcd_count(gb);
  ret_effect(gb);
}

void decNumBombs_hook(GB *gb) {
  SET_HL(wNumBombs);
  CYC(0x17c8, 0x17cb);
  dec_bcd_count(gb);
  ret_effect(gb);
}

// object positions (Object.y at +0x0a, yh +0x0b, xh +0x0d, zh +0x0f)

static void short_pos_from_de(GB *gb) {
  CYC(0x209b, 0x209c); A = mem_rd(gb, DE);
  alu_and(gb, 0xf0);
  B = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(0x209c, 0x20a2); A = mem_rd(gb, DE);
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, B);
  CYC(0x20a2, 0x20a8);
}

void getShortPositionFromDE_hook(GB *gb) {
  short_pos_from_de(gb);
  ret_effect(gb);
}

static void object_get_short_position(GB *gb) {
  CYC(0x2096, 0x2098); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x2098, 0x209b);
  short_pos_from_de(gb);
}

void objectGetShortPosition_hook(GB *gb) {
  object_get_short_position(gb);
  ret_effect(gb);
}

void objectGetShortPosition_withYOffset_hook(GB *gb) {
  B = A;
  CYC(0x20a8, 0x20ab); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x20ab, 0x20af); A = mem_rd(gb, DE);
  alu_add(gb, B);
  CYC(0x20af, 0x20b2);
  alu_and(gb, 0xf0);
  B = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(0x209c, 0x20a2); A = mem_rd(gb, DE);
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, B);
  CYC(0x20a2, 0x20a8);
  ret_effect(gb);
}

static void object_get_position(GB *gb) {
  CYC(0x208a, 0x208c); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x208c, 0x2090); A = mem_rd(gb, DE);
  B = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(0x2090, 0x2094); A = mem_rd(gb, DE);
  C = A;
  CYC(0x2094, 0x2096);
}

void objectGetPosition_hook(GB *gb) {
  object_get_position(gb);
  ret_effect(gb);
}

static void short_to_long_pos(GB *gb) {
  A = C;
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  B = A;
  A = C;
  alu_swap_a(gb);
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  C = A;
  CYC(0x20cc, 0x20db);
}

void convertShortToLongPosition_paramC_hook(GB *gb) {
  short_to_long_pos(gb);
  ret_effect(gb);
}

void convertShortToLongPosition_hook(GB *gb) {
  C = A;
  CYC(0x20cb, 0x20cc);
  short_to_long_pos(gb);
  ret_effect(gb);
}

static void set_short_pos_param_c(GB *gb) {
  uint16_t bc = BC;
  CYC(0x20b9, 0x20bd);
  short_to_long_pos(gb);
  CYC(0x20bd, 0x20be); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(0x20be, 0x20c1); mem_wr(gb, HL, C);
  SET_BC(bc);
  CYC(0x20c1, 0x20c3);
}

void setShortPosition_paramC_hook(GB *gb) {
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

void setShortPosition_hook(GB *gb) {
  C = A;
  CYC(0x20b8, 0x20b9);
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

void objectSetShortPosition_hook(GB *gb) {
  H = D;
  CYC(0x20c3, 0x20c6); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x20c6, 0x20cb);
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

static void center_on_tile(GB *gb) {
  alu_xor(gb, A);
  CYC(0x20e1, 0x20e3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x20e3, 0x20e4); A = mem_rd(gb, HL);
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  CYC(0x20e4, 0x20e9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(0x20e9, 0x20eb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x20eb, 0x20ec); A = mem_rd(gb, HL);
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  CYC(0x20ec, 0x20f1); mem_wr(gb, HL, A);
  CYC(0x20f1, 0x20f2);
}

void centerCoordinatesOnTile_hook(GB *gb) {
  center_on_tile(gb);
  ret_effect(gb);
}

void objectCenterOnTile_hook(GB *gb) {
  CYC(0x20db, 0x20dd); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_Y);
  L = A;
  H = D;
  CYC(0x20dd, 0x20e1);
  center_on_tile(gb);
  ret_effect(gb);
}

static void copy_pos_raw(GB *gb) {
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x2247, 0x224e); A = mem_rd(gb, DE);
  CYC(0x224e, 0x224f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x224f, 0x2253); A = mem_rd(gb, DE);
  CYC(0x2253, 0x2254); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x2254, 0x2258); A = mem_rd(gb, DE);
  CYC(0x2258, 0x2259); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2259, 0x225a);
}

void objectCopyPosition_rawAddress_hook(GB *gb) {
  copy_pos_raw(gb);
  ret_effect(gb);
}

static void object_copy_position(GB *gb) {
  CYC(0x2242, 0x2244); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x2244, 0x2247);
  copy_pos_raw(gb);
}

void objectCopyPosition_hook(GB *gb) {
  object_copy_position(gb);
  ret_effect(gb);
}

void objectCopyPositionWithOffset_hook(GB *gb) {
  CYC(0x225a, 0x225c); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x225c, 0x2266); A = mem_rd(gb, DE);
  alu_add(gb, B);
  CYC(0x2266, 0x2268); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x2268, 0x226c); A = mem_rd(gb, DE);
  alu_add(gb, C);
  CYC(0x226c, 0x226e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x226e, 0x2272); A = mem_rd(gb, DE);
  CYC(0x2272, 0x2273); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2273, 0x2274);
  ret_effect(gb);
}

static void take_pos(GB *gb) {
  CYC(0x2277, 0x2279); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x2279, 0x2283); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, B);
  CYC(0x2283, 0x2285); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x2285, 0x2289); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, C);
  CYC(0x2289, 0x228b); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x228b, 0x228f); A = mem_rd(gb, HL);
  CYC(0x228f, 0x2290); mem_wr(gb, DE, A);
  CYC(0x2290, 0x2291);
}

void objectTakePositionWithOffset_hook(GB *gb) {
  take_pos(gb);
  ret_effect(gb);
}

void objectTakePosition_hook(GB *gb) {
  SET_BC(0);
  CYC(0x2274, 0x2277);
  take_pos(gb);
  ret_effect(gb);
}

// collision tables (indexed by wActiveCollisions through rst $18)

static void add_double_index_to_hl(GB *gb) {
  uint16_t bc = BC;
  C = A;
  B = 0;
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  SET_BC(bc);
  CYC(0x0018, 0x0020);
}

static void collision_table(GB *gb, uint16_t base) {
  CYC(base, base + 3); A = W8(wActiveCollisions);
  CYC(base + 3, base + 4);
  add_double_index_to_hl(gb);
  CYC(base + 4, base + 5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(base + 5, base + 6); H = mem_rd(gb, HL);
  L = A;
  CYC(base + 6, base + 9);
}

void lookupCollisionTable_paramE_hook(GB *gb) {
  collision_table(gb, 0x1e20);
  lookup_key(gb);
  ret_effect(gb);
}

static void lookup_collision_table(GB *gb) {
  E = A;
  CYC(0x1e1f, 0x1e20);
  collision_table(gb, 0x1e20);
  lookup_key(gb);
}

void lookupCollisionTable_hook(GB *gb) {
  lookup_collision_table(gb);
  ret_effect(gb);
}

void findByteInCollisionTable_paramE_hook(GB *gb) {
  collision_table(gb, 0x1e2a);
  find_byte_at_hl(gb);
  ret_effect(gb);
}

void findByteInCollisionTable_hook(GB *gb) {
  E = A;
  CYC(0x1e29, 0x1e2a);
  collision_table(gb, 0x1e2a);
  find_byte_at_hl(gb);
  ret_effect(gb);
}

// object slot availability: walk slots from hl (one per 0x40 bytes, via inc h) until b free ones

static void slots_available(GB *gb) {
  for (;;) {
    CYC(0x20fa, 0x20fd);
    CYC(0x2104, 0x2105); A = mem_rd(gb, HL);
    H++;
    alu_or(gb, A);
    if (F & FZ) {
      CYC(0x2105, 0x2107); CYCT(0x2107, 0x2108);
      CYC(0x20fd, 0x2101);
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x2101, 0x2103); continue; }
      CYC(0x2101, 0x2104);
      return;
    }
    CYC(0x2105, 0x2108);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x2108, 0x210c); CYCT(0x20fd, 0x20ff); continue; }
    CYC(0x2108, 0x210c);
    alu_or(gb, H);
    CYC(0x210c, 0x210e);
    CYC(0x20fd, 0x20ff); CYCT(0x20ff, 0x2100);
    return;
  }
}

void checkBPartSlotsAvailable_hook(GB *gb) {
  SET_HL(PART_SLOTS);
  CYC(0x20f2, 0x20f7);
  slots_available(gb);
  ret_effect(gb);
}

void checkBEnemySlotsAvailable_hook(GB *gb) {
  SET_HL(ENEMY_SLOTS);
  CYC(0x20f7, 0x20fa);
  slots_available(gb);
  ret_effect(gb);
}

// related objects and screen checks

static void related_object_var(GB *gb) {
  H = A;
  CYC(0x2166, 0x2169); A = H8(hActiveObjectType);
  alu_add(gb, L);
  E = A;
  CYC(0x2169, 0x216c); A = mem_rd(gb, DE);
  alu_add(gb, H);
  L = A;
  E = alu_inc8(gb, E);
  CYC(0x216c, 0x2170); A = mem_rd(gb, DE);
  H = A;
  CYC(0x2170, 0x2172);
}

void objectGetRelatedObject1Var_hook(GB *gb) {
  L = OBJ_RELATED1;
  CYC(0x2160, 0x2164);
  related_object_var(gb);
  ret_effect(gb);
}

void objectGetRelatedObject2Var_hook(GB *gb) {
  L = OBJ_RELATED2;
  CYC(0x2164, 0x2166);
  related_object_var(gb);
  ret_effect(gb);
}

void objectGetZAboveScreen_hook(GB *gb) {
  CYC(0x2172, 0x2174); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x2174, 0x2178); A = mem_rd(gb, DE);
  B = A;
  CYC(0x2178, 0x217b); A = H8(hCameraY);
  alu_sub(gb, B);
  alu_sub(gb, 0x08);
  alu_cp(gb, 0x80);
  if (!(F & FC)) CYCT(0x217b, 0x2181);
  else { CYC(0x217b, 0x2181); A = 0x80; CYC(0x2181, 0x2184); }
  ret_effect(gb);
}

void objectCheckWithinScreenBoundary_hook(GB *gb) {
  CYC(0x2184, 0x2186); B = H8(hCameraY);
  CYC(0x2186, 0x2189); C = H8(hCameraX);
  CYC(0x2189, 0x218c); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x218c, 0x2190); A = mem_rd(gb, DE);
  alu_sub(gb, B);
  alu_add(gb, 0x07);
  alu_cp(gb, 0x8f);
  if (!(F & FC)) CYCT(0x2190, 0x2196);
  else {
    CYC(0x2190, 0x2196);
    E = alu_inc8(gb, E);
    E = alu_inc8(gb, E);
    CYC(0x2196, 0x2199); A = mem_rd(gb, DE);
    alu_sub(gb, C);
    alu_add(gb, 0x07);
    alu_cp(gb, 0xaf);
    CYC(0x2199, 0x219f);
  }
  ret_effect(gb);
}

void objectCheckWithinRoomBoundary_hook(GB *gb) {
  CYC(0x219f, 0x21a1); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  SET_HL(wRoomEdgeY);
  CYC(0x21a1, 0x21a8); A = mem_rd(gb, DE);
  CYC(0x21a8, 0x21a9); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(0x21a9, 0x21aa);
  else {
    E = alu_inc8(gb, E);
    E = alu_inc8(gb, E);
    L = alu_inc8(gb, L);
    CYC(0x21a9, 0x21ae); A = mem_rd(gb, DE);
    CYC(0x21ae, 0x21af); alu_cp(gb, mem_rd(gb, HL));
    CYC(0x21af, 0x21b0);
  }
  ret_effect(gb);
}

// tiles

static void tile_at_position(GB *gb) {
  A = C;
  alu_and(gb, 0xf0);
  alu_swap_a(gb);
  L = A;
  A = B;
  alu_and(gb, 0xf0);
  alu_or(gb, L);
  L = A;
  H = wRoomLayout >> 8;
  CYC(0x1447, 0x1455); A = mem_rd(gb, HL);
  CYC(0x1455, 0x1456);
}

void getTileAtPosition_hook(GB *gb) {
  tile_at_position(gb);
  ret_effect(gb);
}

static void object_get_tile_at_position(GB *gb) {
  CYC(0x1444, 0x1447);
  object_get_position(gb);
  tile_at_position(gb);
}

void objectGetTileAtPosition_hook(GB *gb) {
  object_get_tile_at_position(gb);
  ret_effect(gb);
}

static void object_get_relative_tile(GB *gb) {
  CYC(0x1435, 0x1437); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  L = A;
  H = D;
  CYC(0x1437, 0x143c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, B);
  B = A;
  L = alu_inc8(gb, L);
  CYC(0x143c, 0x1440); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, C);
  C = A;
  CYC(0x1440, 0x1444);
  tile_at_position(gb);
}

void objectGetRelativeTile_hook(GB *gb) {
  object_get_relative_tile(gb);
  ret_effect(gb);
}

static bool tile_at_offset_matches(GB *gb, uint16_t call_addr) {
  CYC(call_addr, call_addr + 3);
  alu_add(gb, E);
  L = A;
  CYC(0x1481, 0x1485); A = H8(hFF8B);
  CYC(0x1485, 0x1486); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x1486, 0x1487);
  return F & FZ;
}

void objectGetRelativePositionOfTile_hook(GB *gb) {
  CYC(0x1456, 0x1458); H8(hFF8B) = A;
  CYC(0x1458, 0x145b);
  object_get_short_position(gb);
  E = A;
  H = wRoomLayout >> 8;
  CYC(0x145b, 0x1460);
  A = 0xf0;
  if (tile_at_offset_matches(gb, 0x1460)) { A = 0x00; CYCT(0x1463, 0x1466); ret_effect(gb); return; }
  CYC(0x1463, 0x1468);
  A = 0x01;
  if (tile_at_offset_matches(gb, 0x1468)) { A = 0x01; CYCT(0x146b, 0x146e); ret_effect(gb); return; }
  CYC(0x146b, 0x1470);
  A = 0x10;
  if (tile_at_offset_matches(gb, 0x1470)) { A = 0x02; CYCT(0x1473, 0x1476); ret_effect(gb); return; }
  CYC(0x1473, 0x1478);
  A = 0xff;
  if (tile_at_offset_matches(gb, 0x1478)) { A = 0x03; CYCT(0x147b, 0x147e); ret_effect(gb); return; }
  CYC(0x147b, 0x1481);
  A = 0xff;
  ret_effect(gb);
}

static void tile_index_from_layout_buffer(GB *gb) {
  CYC(0x15d8, 0x15da); uint8_t svbk = mem_rd(gb, IO_SVBK);
  CYC(0x15da, 0x15df); mem_wr(gb, IO_SVBK, 3);
  B = w3RoomLayoutBuffer >> 8;
  CYC(0x15df, 0x15e2); A = mem_rd(gb, BC);
  E = A;
  CYC(0x15e2, 0x15e7); mem_wr(gb, IO_SVBK, 3);
  L = E;
  H = w3TileCollisions >> 8;
  CYC(0x15e7, 0x15eb); B = mem_rd(gb, HL);
  CYC(0x15eb, 0x15ee); mem_wr(gb, IO_SVBK, svbk);
  A = B;
  alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x15ee, 0x15f3); A = E; CYC(0x15f9, 0x15fb); return; }
  CYC(0x15ee, 0x15f4);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x15f4, 0x15f6); A = E; CYC(0x15f9, 0x15fb); return; }
  CYC(0x15f4, 0x15f6);
  alu_scf(gb);
  A = E;
  CYC(0x15f6, 0x15f9);
}

void getTileIndexFromRoomLayoutBuffer_paramC_hook(GB *gb) {
  tile_index_from_layout_buffer(gb);
  ret_effect(gb);
}

void getTileIndexFromRoomLayoutBuffer_hook(GB *gb) {
  C = A;
  CYC(0x15d7, 0x15d8);
  tile_index_from_layout_buffer(gb);
  ret_effect(gb);
}

// angles. getRelativeAngle stays transliterated (func_11_7f64 jumps into it), so the entries
// that fall into it call the generated function and let its ret return to the caller.

static void object_get_relative_angle(GB *gb) {
  CYC(0x1ea4, 0x1ea6); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  E = A;
  CYC(0x1ea6, 0x1ea9);
  getRelativeAngle(gb);
}

void objectGetRelativeAngle_hook(GB *gb) { object_get_relative_angle(gb); }

void objectGetAngleTowardLink_hook(GB *gb) {
  CYC(0x1e9c, 0x1e9f); A = mem_rd(gb, w1Link_yh);
  B = A;
  CYC(0x1e9f, 0x1ea3); A = mem_rd(gb, w1Link_xh);
  C = A;
  CYC(0x1ea3, 0x1ea4);
  object_get_relative_angle(gb);
}

static void angle_toward_enemy_target(GB *gb) {
  CYC(0x1e94, 0x1e96); B = H8(hEnemyTargetY);
  CYC(0x1e96, 0x1e99); C = H8(hEnemyTargetX);
  CYC(0x1e99, 0x1e9c);
  object_get_relative_angle(gb);
}

void objectGetAngleTowardEnemyTarget_hook(GB *gb) { angle_toward_enemy_target(gb); }

void objectNudgeAngleTowards_hook(GB *gb) {
  C = A;
  CYC(0x1fd4, 0x1fd7); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ANGLE);
  E = A;
  CYC(0x1fd7, 0x1fdb); A = mem_rd(gb, DE);
  B = A;
  alu_sub(gb, C);
  if (F & FZ) CYCT(0x1fdb, 0x1fdf);
  else {
    CYC(0x1fdb, 0x1fdf);
    alu_and(gb, 0x1f);
    alu_cp(gb, 0x10);
    if (!(F & FC)) { CYCT(0x1fdf, 0x1fe5); CYC(0x1fe8, 0x1fe9); B = alu_inc8(gb, B); }
    else { CYC(0x1fdf, 0x1fe8); B = alu_dec8(gb, B); }
  }
  A = B;
  alu_and(gb, 0x1f);
  CYC(0x1fe9, 0x1fed); mem_wr(gb, DE, A);
  CYC(0x1fed, 0x1fee);
  ret_effect(gb);
}

static void centered_with_link(GB *gb) {
  C = B;
  C = alu_sla(gb, C);
  C = alu_inc8(gb, C);
  H = D;
  CYC(0x1fee, 0x1ff5); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x1ff5, 0x1ffb); A = mem_rd(gb, w1Link_yh);
  CYC(0x1ffb, 0x1ffc); alu_sub(gb, mem_rd(gb, HL));
  alu_add(gb, B);
  alu_cp(gb, C);
  if (F & FC) CYCT(0x1ffc, 0x1fff);
  else {
    L = alu_inc8(gb, L);
    L = alu_inc8(gb, L);
    CYC(0x1ffc, 0x2004); A = mem_rd(gb, w1Link_xh);
    CYC(0x2004, 0x2005); alu_sub(gb, mem_rd(gb, HL));
    alu_add(gb, B);
    alu_cp(gb, C);
    CYC(0x2005, 0x2008);
  }
}

void objectCheckCenteredWithLink_hook(GB *gb) {
  centered_with_link(gb);
  ret_effect(gb);
}

static void link_within_distance(GB *gb) {
  CYC(0x1fa2, 0x1fa4); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  H = D;
  E = 4;
  CYC(0x1fa4, 0x1fad); A = mem_rd(gb, w1Link_yh);
  CYC(0x1fad, 0x1fae); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(0x1fae, 0x1fb0);
  else { CYC(0x1fae, 0x1fb4); alu_cpl(gb); A = alu_inc8(gb, A); E = 0; }
  B = A;
  A = C;
  alu_sub(gb, B);
  alu_ccf(gb);
  if (!(F & FC)) { CYCT(0x1fb4, 0x1fb9); return; }
  CYC(0x1fb4, 0x1fb9);
  C = A;
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  E |= 0x20;
  CYC(0x1fb9, 0x1fc1); A = mem_rd(gb, w1Link_xh);
  CYC(0x1fc1, 0x1fc2); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(0x1fc2, 0x1fc4);
  else { CYC(0x1fc2, 0x1fc8); alu_cpl(gb); A = alu_inc8(gb, A); E |= 0x40; }
  alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x1fc8, 0x1fca); return; }
  CYC(0x1fc8, 0x1fca);
  alu_cp(gb, B);
  if (F & FC) CYCT(0x1fca, 0x1fcd);
  else { CYC(0x1fca, 0x1fcf); E = alu_swap(gb, E); }
  A = E;
  alu_and(gb, 0x06);
  alu_scf(gb);
  CYC(0x1fcf, 0x1fd4);
}

void objectCheckLinkWithinDistance_hook(GB *gb) {
  link_within_distance(gb);
  ret_effect(gb);
}

// speed

static void add_speed_component(GB *gb) {
  CYC(0x2015, 0x2016); A = mem_rd(gb, DE);
  CYC(0x2016, 0x2017); alu_add(gb, mem_rd(gb, HL));
  CYC(0x2017, 0x2018); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E);
  CYC(0x2018, 0x201a); A = mem_rd(gb, DE);
  CYC(0x201a, 0x201b); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x201b, 0x201c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x201c, 0x201d);
}

void objectApplyComponentSpeed_hook(GB *gb) {
  CYC(0x2008, 0x200a); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_Y);
  L = A;
  alu_add(gb, 0x06);
  E = A;
  H = D;
  CYC(0x200a, 0x2014);
  add_speed_component(gb);
  CYC(0x2014, 0x2015);
  E = alu_inc8(gb, E);
  add_speed_component(gb);
  ret_effect(gb);
}

static void velocity_invalid(GB *gb) {
  SET_HL(wTmpcec0 + 3);
  alu_xor(gb, A);
  CYC(0x2081, 0x2086); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x2086, 0x2087); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x2087, 0x2088); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x2088, 0x2089); mem_wr(gb, HL, A);
  CYC(0x2089, 0x208a);
}

static void position_offset_for_velocity(GB *gb) {
  alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(0x2041, 0x2045); velocity_invalid(gb); return; }
  CYC(0x2041, 0x2045);
  B = alu_swap(gb, B);
  if (F & FZ) { CYCT(0x2045, 0x2049); velocity_invalid(gb); return; }
  CYC(0x2045, 0x2049);
  A = B;
  SET_HL(speedTable_bank3);
  C = alu_sla(gb, C);
  B = 0;
  alu_add_hl(gb, BC);
  B = A;
  alu_and(gb, 0xf0);
  C = A;
  A = B;
  alu_and(gb, 0x0f);
  B = A;
  alu_add_hl(gb, BC);
  CYC(0x2049, 0x205d); uint8_t saved = H8(hRomBank);
  CYC(0x205d, 0x2062); H8(hRomBank) = 3;
  CYC(0x2062, 0x2065); mem_wr(gb, MBC_ROM_BANK, 3);
  SET_BC(wTmpcec0);
  CYC(0x2065, 0x2069); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2069, 0x206a); mem_wr(gb, BC, A);
  C = alu_inc8(gb, C);
  CYC(0x206a, 0x206c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x206c, 0x206d); mem_wr(gb, BC, A);
  C = alu_inc8(gb, C);
  A = 0x0e;
  CYC(0x206d, 0x2071);
  add_a_to_hl(gb);
  CYC(0x2071, 0x2072); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2072, 0x2073); mem_wr(gb, BC, A);
  C = alu_inc8(gb, C);
  CYC(0x2073, 0x2075); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2075, 0x2076); mem_wr(gb, BC, A);
  A = saved;
  CYC(0x2076, 0x2079); H8(hRomBank) = saved;
  CYC(0x2079, 0x207c); mem_wr(gb, MBC_ROM_BANK, saved);
  SET_HL(wTmpcec0);
  alu_or(gb, H);
  CYC(0x207c, 0x2081);
}

void getPositionOffsetForVelocity_hook(GB *gb) {
  position_offset_for_velocity(gb);
  ret_effect(gb);
}

static void apply_given_speed(GB *gb) {
  CYC(0x2029, 0x202c);
  position_offset_for_velocity(gb);
  if (F & FZ) { CYCT(0x202c, 0x202d); return; }
  E = alu_inc8(gb, E);
  CYC(0x202c, 0x202f); A = mem_rd(gb, DE);
  CYC(0x202f, 0x2030); alu_add(gb, mem_rd(gb, HL));
  CYC(0x2030, 0x2031); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x2031, 0x2034); A = mem_rd(gb, DE);
  CYC(0x2034, 0x2035); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x2035, 0x2036); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x2036, 0x2039); A = mem_rd(gb, DE);
  CYC(0x2039, 0x203a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x203a, 0x203b); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(0x203b, 0x203e); A = mem_rd(gb, DE);
  CYC(0x203e, 0x203f); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x203f, 0x2040); mem_wr(gb, DE, A);
  CYC(0x2040, 0x2041);
}

void objectApplyGivenSpeed_hook(GB *gb) {
  apply_given_speed(gb);
  ret_effect(gb);
}

static void object_apply_speed(GB *gb) {
  H = D;
  CYC(0x201d, 0x2020); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ANGLE);
  E = A;
  L = A;
  CYC(0x2020, 0x2025); C = mem_rd(gb, HL);
  alu_add(gb, 0x07);
  L = A;
  CYC(0x2025, 0x2029); B = mem_rd(gb, HL);
  apply_given_speed(gb);
}

void objectApplySpeed_hook(GB *gb) {
  object_apply_speed(gb);
  ret_effect(gb);
}

// vertical speed

static void update_speed_z_param_c(GB *gb) {
  CYC(0x1f46, 0x1f48); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_Z);
  E = A;
  alu_add(gb, 0x06);
  L = A;
  H = D;
  CYC(0x1f48, 0x1f52);
  add16_bit_refs(gb);
  alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x1f52, 0x1f56);
    alu_xor(gb, A);
    CYC(0x1f60, 0x1f62); mem_wr(gb, DE, A);
    E = alu_dec8(gb, E);
    CYC(0x1f62, 0x1f64); mem_wr(gb, DE, A);
    alu_xor(gb, A);
    CYC(0x1f64, 0x1f66);
    return;
  }
  CYC(0x1f52, 0x1f56);
  L = alu_dec8(gb, L);
  A = C;
  CYC(0x1f56, 0x1f59); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1f59, 0x1f5a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0;
  CYC(0x1f5a, 0x1f5d); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x1f5d, 0x1f5e); mem_wr(gb, HL, A);
  alu_or(gb, D);
  CYC(0x1f5e, 0x1f60);
}

void objectUpdateSpeedZ_paramC_hook(GB *gb) {
  update_speed_z_param_c(gb);
  ret_effect(gb);
}

void objectUpdateSpeedZ_hook(GB *gb) {
  C = A;
  CYC(0x1f45, 0x1f46);
  update_speed_z_param_c(gb);
  ret_effect(gb);
}

void objectUpdateSpeedZ_sidescroll_hook(GB *gb) {
  B = 0x06;
  CYC(0x1f66, 0x1f68);
  objectUpdateSpeedZ_sidescroll_givenYOffset(gb);
}

void objectSetSpeedZ_hook(GB *gb) {
  CYC(0x239d, 0x239f); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_SPEED_Z);
  L = A;
  H = D;
  CYC(0x239f, 0x23a4); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  CYC(0x23a4, 0x23a6); mem_wr(gb, HL, B);
  CYC(0x23a6, 0x23a7);
  ret_effect(gb);
}

static void negate_and_halve_speed_z(GB *gb) {
  H = D;
  CYC(0x2374, 0x2377); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_SPEED_Z);
  L = A;
  CYC(0x2377, 0x237b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_cpl(gb);
  C = A;
  CYC(0x237b, 0x237e); A = mem_rd(gb, HL);
  alu_cpl(gb);
  B = A;
  SET_BC(BC + 1);
  B = alu_sra(gb, B);
  C = alu_rr(gb, C);
  SET_HL(0x10000 - 0x80);
  CYC(0x237e, 0x238b);
  compare_hl_to_bc(gb);
  A = alu_inc8(gb, A);
  alu_scf(gb);
  if (F & FZ) { CYCT(0x238b, 0x238e); return; }
  CYC(0x238b, 0x2390); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_SPEED_Z);
  E = A;
  A = C;
  CYC(0x2390, 0x2395); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = B;
  CYC(0x2395, 0x2398); mem_wr(gb, DE, A);
  alu_or(gb, C);
  alu_scf(gb);
  if (F & FZ) { CYCT(0x2398, 0x239b); return; }
  CYC(0x2398, 0x239b);
  alu_xor(gb, A);
  CYC(0x239b, 0x239d);
}

void objectNegateAndHalveSpeedZ_hook(GB *gb) {
  negate_and_halve_speed_z(gb);
  ret_effect(gb);
}

void objectUpdateSpeedZAndBounce_hook(GB *gb) {
  CYC(0x2370, 0x2373);
  update_speed_z_param_c(gb);
  if (!(F & FZ)) { CYCT(0x2373, 0x2374); ret_effect(gb); return; }
  CYC(0x2373, 0x2374);
  negate_and_halve_speed_z(gb);
  ret_effect(gb);
}

// collision boxes

static void h_check_contains_point(GB *gb) {
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x1be4, 0x1beb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_sub(gb, B);
  if (!(F & FC)) CYCT(0x1beb, 0x1bee);
  else { CYC(0x1beb, 0x1bf0); alu_cpl(gb); A = alu_inc8(gb, A); }
  B = A;
  L = alu_inc8(gb, L);
  CYC(0x1bf0, 0x1bf3); A = mem_rd(gb, HL);
  alu_sub(gb, C);
  if (!(F & FC)) CYCT(0x1bf3, 0x1bf6);
  else { CYC(0x1bf3, 0x1bf8); alu_cpl(gb); A = alu_inc8(gb, A); }
  C = A;
  A = L;
  alu_add(gb, 0x19);
  L = A;
  A = B;
  CYC(0x1bf8, 0x1bff); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x1bff, 0x1c00); return; }
  L = alu_inc8(gb, L);
  A = C;
  CYC(0x1bff, 0x1c03); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x1c03, 0x1c04);
}

void objectHCheckContainsPoint_hook(GB *gb) {
  h_check_contains_point(gb);
  ret_effect(gb);
}

void objectCheckContainsPoint_hook(GB *gb) {
  H = D;
  CYC(0x1bdb, 0x1bde); L = H8(hActiveObjectType);
  CYC(0x1bde, 0x1be1);
  h_check_contains_point(gb);
  ret_effect(gb);
}

void interactionCheckContainsPoint_hook(GB *gb) {
  H = D;
  L = INTERACTION_BASE;
  CYC(0x1be1, 0x1be4);
  h_check_contains_point(gb);
  ret_effect(gb);
}

static void objects_collided_from_variables(GB *gb) {
  A = B;
  CYC(0x1c04, 0x1c07); H8(hFF8D) = A;
  A = C;
  CYC(0x1c07, 0x1c0a); H8(hFF8C) = A;
  CYC(0x1c0a, 0x1c0b); A = mem_rd(gb, DE);
  CYC(0x1c0b, 0x1c0c); alu_add(gb, mem_rd(gb, HL));
  B = A;
  CYC(0x1c0c, 0x1c0f); A = H8(hFF8F);
  C = A;
  CYC(0x1c0f, 0x1c12); A = H8(hFF8D);
  alu_sub(gb, C);
  alu_add(gb, B);
  B = alu_sla(gb, B);
  alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x1c12, 0x1c18); return; }
  CYC(0x1c12, 0x1c18);
  E = alu_inc8(gb, E);
  SET_HL(HL + 1);
  CYC(0x1c18, 0x1c1b); A = mem_rd(gb, DE);
  CYC(0x1c1b, 0x1c1c); alu_add(gb, mem_rd(gb, HL));
  B = A;
  CYC(0x1c1c, 0x1c1f); A = H8(hFF8E);
  C = A;
  CYC(0x1c1f, 0x1c22); A = H8(hFF8C);
  alu_sub(gb, C);
  alu_add(gb, B);
  B = alu_sla(gb, B);
  alu_cp(gb, B);
  CYC(0x1c22, 0x1c28);
}

void checkObjectsCollidedFromVariables_hook(GB *gb) {
  objects_collided_from_variables(gb);
  ret_effect(gb);
}

static void collided_with_link_from_1c56(GB *gb) {
  CYC(0x1c56, 0x1c57); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x1c57, 0x1c59); H8(hFF8E) = A;
  L = alu_dec8(gb, L);
  CYC(0x1c59, 0x1c5b); A = mem_rd(gb, HL);
  CYC(0x1c5b, 0x1c5d); H8(hFF8F) = A;
  A = L;
  alu_add(gb, 0x1b);
  E = A;
  CYC(0x1c5d, 0x1c64); A = W8(wLinkObjectIndex);
  H = A;
  L = OBJ_YH;
  CYC(0x1c64, 0x1c68); B = mem_rd(gb, HL);
  L = OBJ_XH;
  CYC(0x1c68, 0x1c6b); C = mem_rd(gb, HL);
  L = OBJ_COLLISION_RADIUS_Y;
  CYC(0x1c6b, 0x1c6f);
  objects_collided_from_variables(gb);
}

static void check_collided_with_link(GB *gb) {
  CYC(0x1c47, 0x1c4a); A = W8(wLinkObjectIndex);
  B = A;
  C = OBJ_ZH;
  CYC(0x1c4a, 0x1c4e); A = mem_rd(gb, BC);
  CYC(0x1c4e, 0x1c4f); alu_sub(gb, mem_rd(gb, HL));
  alu_add(gb, 0x07);
  alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(0x1c4f, 0x1c54); return; }
  CYC(0x1c4f, 0x1c54);
  L = alu_dec8(gb, L);
  L = alu_dec8(gb, L);
  CYC(0x1c54, 0x1c56);
  collided_with_link_from_1c56(gb);
}

void _checkCollidedWithLink_hook(GB *gb) {
  check_collided_with_link(gb);
  ret_effect(gb);
}

static void object_check_collided_with_link(GB *gb) {
  CYC(0x1c41, 0x1c43); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  L = A;
  H = D;
  CYC(0x1c43, 0x1c47);
  check_collided_with_link(gb);
}

void objectCheckCollidedWithLink_hook(GB *gb) {
  object_check_collided_with_link(gb);
  ret_effect(gb);
}

void objectCheckCollidedWithLink_ignoreZ_hook(GB *gb) {
  CYC(0x1c6f, 0x1c71); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_XH);
  L = A;
  H = D;
  CYC(0x1c71, 0x1c77);
  collided_with_link_from_1c56(gb);
  ret_effect(gb);
}

static void collided_with_link_not_dead(GB *gb) {
  CYC(0x1c2e, 0x1c31); A = W8(wLinkDeathTrigger);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1c31, 0x1c33); return; }
  CYC(0x1c31, 0x1c35);
  object_check_collided_with_link(gb);
}

void objectCheckCollidedWithLink_notDead_hook(GB *gb) {
  collided_with_link_not_dead(gb);
  ret_effect(gb);
}

void objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook(GB *gb) {
  CYC(0x1c28, 0x1c2b); A = W8(wLinkGrabState);
  alu_and(gb, 0xbe);
  if (!(F & FZ)) { CYCT(0x1c2b, 0x1c2e); ret_effect(gb); return; }
  CYC(0x1c2b, 0x1c2e);
  collided_with_link_not_dead(gb);
  ret_effect(gb);
}

void objectCheckCollidedWithLink_onGround_hook(GB *gb) {
  CYC(0x1c35, 0x1c38); A = W8(wLinkInAir);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1c38, 0x1c3a); ret_effect(gb); return; }
  CYC(0x1c38, 0x1c3d); A = mem_rd(gb, w1Link_zh);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1c3d, 0x1c3f); ret_effect(gb); return; }
  CYC(0x1c3d, 0x1c41);
  collided_with_link_not_dead(gb);
  ret_effect(gb);
}

static void objects_collided(GB *gb) {
  A = L;
  alu_and(gb, 0xc0);
  L = A;
  uint16_t hl = HL;
  H = D;
  CYC(0x1d5a, 0x1d60); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x1d60, 0x1d66); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1d66, 0x1d68); H8(hFF8F) = A;
  L = alu_inc8(gb, L);
  CYC(0x1d68, 0x1d6a); A = mem_rd(gb, HL);
  CYC(0x1d6a, 0x1d6c); H8(hFF8E) = A;
  A = L;
  alu_add(gb, 0x19);
  E = A;
  SET_HL(hl);
  A = L;
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x1d6c, 0x1d76); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(0x1d76, 0x1d79); C = mem_rd(gb, HL);
  alu_add(gb, 0x1b);
  L = A;
  CYC(0x1d79, 0x1d7f);
  objects_collided_from_variables(gb);
}

void checkObjectsCollided_hook(GB *gb) {
  objects_collided(gb);
  ret_effect(gb);
}

static void link_position_plus_direction_offset(GB *gb) {
  CYC(0x1cd3, 0x1cd6); A = mem_rd(gb, w1Link_direction);
  SET_HL(linkDirectionOffsets);
  CYC(0x1cd6, 0x1cda);
  add_double_index_to_hl(gb);
  SET_DE(w1Link_yh);
  CYC(0x1cda, 0x1cde); A = mem_rd(gb, DE);
  CYC(0x1cde, 0x1cdf); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1cdf, 0x1ce1); H8(hFF8F) = A;
  SET_HL(HL + 1);
  E = OBJ_XH;
  CYC(0x1ce1, 0x1ce5); A = mem_rd(gb, DE);
  CYC(0x1ce5, 0x1ce6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1ce6, 0x1ce8); H8(hFF8E) = A;
  E = OBJ_ZH;
  CYC(0x1ce8, 0x1ceb); A = mem_rd(gb, DE);
  alu_sub(gb, 0x03);
  CYC(0x1ceb, 0x1cef); H8(hFF91) = A;
  CYC(0x1cef, 0x1cf0);
}

void _getLinkPositionPlusDirectionOffset_hook(GB *gb) {
  link_position_plus_direction_offset(gb);
  ret_effect(gb);
}

static void check_collision_with_h_and_d(GB *gb) {
  alu_add(gb, OBJ_VAR2A);
  L = A;
  CYC(0x1cf8, 0x1cfb);
  CYC(0x1cfb, 0x1cfd); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x1cfd, 0x1cfe); return; }
  CYC(0x1cfd, 0x1cfe);
  alu_sub(gb, 0x1b);
  L = A;
  CYC(0x1cfe, 0x1d03); A = H8(hFF91);
  CYC(0x1d03, 0x1d04); alu_sub(gb, mem_rd(gb, HL));
  alu_add(gb, 0x07);
  alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(0x1d04, 0x1d09); return; }
  CYC(0x1d04, 0x1d09);
  L = alu_dec8(gb, L);
  L = alu_dec8(gb, L);
  CYC(0x1d09, 0x1d0c); A = mem_rd(gb, HL); SET_HL(HL - 1);
  L = alu_dec8(gb, L);
  CYC(0x1d0c, 0x1d0e); B = mem_rd(gb, HL);
  C = A;
  A = L;
  alu_add(gb, 0x1b);
  L = A;
  E = OBJ_COLLISION_RADIUS_Y;
  CYC(0x1d0e, 0x1d18);
  objects_collided_from_variables(gb);
}

void _checkCollisionWithHAndD_hook(GB *gb) {
  check_collision_with_h_and_d(gb);
  ret_effect(gb);
}

void objectHCheckCollisionWithLink_hook(GB *gb) {
  uint16_t de = DE, hl = HL;
  CYC(0x1c89, 0x1c8e);
  link_position_plus_direction_offset(gb);
  SET_HL(hl);
  A = L;
  alu_and(gb, 0xc0);
  CYC(0x1c8e, 0x1c95);
  check_collision_with_h_and_d(gb);
  SET_DE(de);
  CYC(0x1c95, 0x1c97);
  ret_effect(gb);
}

// Link state checks (all end at checkLinkCollisionsEnabled@noCarry or with scf)

static void link_check_no_carry(GB *gb) {
  alu_xor(gb, A);
  CYC(0x1d58, 0x1d5a);
}

static void link_control_checks_tail(GB *gb) {
  CYC(0x1d44, 0x1d47); A = W8(wLinkDeathTrigger);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1d47, 0x1d4a); link_check_no_carry(gb); return; }
  CYC(0x1d47, 0x1d4d); A = W8(wcc95);
  alu_rlca(gb);
  if (F & FC) { CYCT(0x1d4d, 0x1d50); link_check_no_carry(gb); return; }
  CYC(0x1d4d, 0x1d53); A = W8(wLinkInAir);
  alu_rlca(gb);
  if (F & FC) { CYCT(0x1d53, 0x1d56); link_check_no_carry(gb); return; }
  CYC(0x1d53, 0x1d56);
  alu_scf(gb);
  CYC(0x1d56, 0x1d58);
}

void checkLinkID0AndControlNormal_hook(GB *gb) {
  CYC(0x1d18, 0x1d1b); A = mem_rd(gb, w1Link_id);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(0x1d1b, 0x1d1e); alu_xor(gb, A); CYC(0x1d1e, 0x1d20); ret_effect(gb); return; }
  CYCT(0x1d1b, 0x1d1e);
  link_control_checks_tail(gb);
  ret_effect(gb);
}

static void link_collisions_enabled(GB *gb) {
  CYC(0x1d32, 0x1d35); A = mem_rd(gb, w1Link_collisionType);
  alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x1d35, 0x1d38); link_check_no_carry(gb); return; }
  CYC(0x1d35, 0x1d3b); A = W8(wDisableLinkCollisionsAndMenu);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1d3b, 0x1d3e); link_check_no_carry(gb); return; }
  CYC(0x1d3b, 0x1d41); A = W8(wMenuDisabled);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1d41, 0x1d44); link_check_no_carry(gb); return; }
  CYC(0x1d41, 0x1d44);
  link_control_checks_tail(gb);
}

void checkLinkCollisionsEnabled_hook(GB *gb) {
  link_collisions_enabled(gb);
  ret_effect(gb);
}

static void link_vulnerable(GB *gb) {
  SET_HL(w1Link_var2a);
  CYC(0x1d28, 0x1d2b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1d2b, 0x1d2c); alu_or(gb, mem_rd(gb, HL));
  L = 0x2d;
  CYC(0x1d2c, 0x1d2f); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x1d2f, 0x1d32); link_check_no_carry(gb); return; }
  CYC(0x1d2f, 0x1d32);
  link_collisions_enabled(gb);
}

void checkLinkVulnerable_hook(GB *gb) {
  link_vulnerable(gb);
  ret_effect(gb);
}

void checkLinkVulnerableAndIDZero_hook(GB *gb) {
  CYC(0x1d20, 0x1d23); A = mem_rd(gb, w1Link_id);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(0x1d23, 0x1d26); alu_xor(gb, A); CYC(0x1d26, 0x1d28); ret_effect(gb); return; }
  CYCT(0x1d23, 0x1d26);
  link_vulnerable(gb);
  ret_effect(gb);
}

// simple tile collision (quarter tiles) at the object's position

void objectCheckSimpleCollision_hook(GB *gb) {
  CYC(0x1487, 0x1489); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  L = A;
  H = D;
  CYC(0x1489, 0x148e); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(0x148e, 0x1491); C = mem_rd(gb, HL);
  A = B;
  alu_and(gb, 0xf0);
  L = A;
  A = C;
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, L);
  L = A;
  H = wRoomCollisions >> 8;
  CYC(0x1491, 0x149f); A = mem_rd(gb, HL);
  alu_bit(gb, 3, B);
  if (F & FZ) { CYC(0x149f, 0x14a3); alu_rrca(gb); alu_rrca(gb); CYC(0x14a3, 0x14a5); }
  else CYCT(0x149f, 0x14a3);
  alu_bit(gb, 3, C);
  if (F & FZ) { CYC(0x14a5, 0x14a9); alu_rrca(gb); CYC(0x14a9, 0x14aa); }
  else CYCT(0x14a5, 0x14a9);
  alu_and(gb, 0x01);
  CYC(0x14aa, 0x14ad);
  ret_effect(gb);
}

// tile collisions: wRoomCollisions holds one collision value per tile; values below $10 encode
// quarter tiles as bits, values from $10 up index a table of "special collision" bit masks.

static void position_to_collision_addr(GB *gb, uint16_t from, uint16_t to) {
  A = B;
  alu_and(gb, 0xf0);
  L = A;
  A = C;
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, L);
  L = A;
  CYC(from, to);
}

static void object_position_to_bc(GB *gb, uint16_t from) {
  CYC(from, from + 2); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  L = A;
  H = D;
  CYC(from + 2, from + 7); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(from + 7, from + 10); C = mem_rd(gb, HL);
}

static void tile_collisions_at_position(GB *gb) {
  position_to_collision_addr(gb, 0x14b7, 0x14c2);
  H = wRoomCollisions >> 8;
  CYC(0x14c2, 0x14c4); A = mem_rd(gb, HL);
  alu_or(gb, A);
  CYC(0x14c4, 0x14c7);
}

void getTileCollisionsAtPosition_hook(GB *gb) {
  tile_collisions_at_position(gb);
  ret_effect(gb);
}

static void object_get_tile_collisions(GB *gb) {
  object_position_to_bc(gb, 0x14ad);
  tile_collisions_at_position(gb);
}

void objectGetTileCollisions_hook(GB *gb) {
  object_get_tile_collisions(gb);
  ret_effect(gb);
}

static void simple_collision(GB *gb) {
  alu_bit(gb, 3, B);
  if (F & FZ) { CYC(0x1545, 0x1549); alu_rrca(gb); alu_rrca(gb); CYC(0x1549, 0x154b); }
  else CYCT(0x1545, 0x1549);
  alu_bit(gb, 3, C);
  if (F & FZ) { CYC(0x154b, 0x154f); alu_rrca(gb); CYC(0x154f, 0x1550); }
  else CYCT(0x154b, 0x154f);
  alu_rrca(gb);
  CYC(0x1550, 0x1552);
}

void _simpleCollision_hook(GB *gb) {
  simple_collision(gb);
  ret_effect(gb);
}

static void complex_collision(GB *gb) {
  uint16_t de = DE;
  CYC(0x1552, 0x1553);
  alu_and(gb, 0x0f);
  E = A;
  D = 0;
  alu_add_hl(gb, DE);
  CYC(0x1553, 0x1559); E = mem_rd(gb, HL);
  alu_cp(gb, 0x08);
  A = B;
  if (!(F & FC)) CYCT(0x1559, 0x155f);
  else { CYC(0x1559, 0x1560); A = C; }
  alu_rrca(gb);
  alu_and(gb, 0x07);
  SET_HL(bitTable);
  alu_add(gb, L);
  L = A;
  CYC(0x1560, 0x1568); A = mem_rd(gb, HL);
  alu_and(gb, E);
  SET_DE(de);
  if (F & FZ) { CYCT(0x1568, 0x156c); return; }
  CYC(0x1568, 0x156c);
  alu_scf(gb);
  CYC(0x156c, 0x156e);
}

void _complexCollision_hook(GB *gb) {
  complex_collision(gb);
  ret_effect(gb);
}

static void given_collision(GB *gb, uint16_t from, uint16_t table) {
  alu_cp(gb, 0x10);
  if (F & FC) { CYCT(from, from + 4); simple_collision(gb); return; }
  CYC(from, from + 4);
  SET_HL(table);
  CYC(from + 4, from + 9);
  complex_collision(gb);
}

static void tile_collision(GB *gb, uint16_t from, uint16_t table) {
  H = wRoomCollisions >> 8;
  CYC(from, from + 3); A = mem_rd(gb, HL);
  given_collision(gb, from + 3, table);
}

void checkGivenCollision_allowHoles_hook(GB *gb) { given_collision(gb, 0x14df, specialCollisions_allowHoles); ret_effect(gb); }
void checkTileCollision_allowHoles_hook(GB *gb) { tile_collision(gb, 0x14dc, specialCollisions_allowHoles); ret_effect(gb); }
void checkTileCollisionAt_allowHoles_hook(GB *gb) {
  position_to_collision_addr(gb, 0x14d1, 0x14dc);
  tile_collision(gb, 0x14dc, specialCollisions_allowHoles);
  ret_effect(gb);
}
void objectCheckTileCollision_allowHoles_hook(GB *gb) {
  object_position_to_bc(gb, 0x14c7);
  position_to_collision_addr(gb, 0x14d1, 0x14dc);
  tile_collision(gb, 0x14dc, specialCollisions_allowHoles);
  ret_effect(gb);
}

void checkGivenCollision_disallowHoles_hook(GB *gb) { given_collision(gb, 0x1510, specialCollisions_disallowHoles); ret_effect(gb); }
void checkTileCollision_disallowHoles_hook(GB *gb) { tile_collision(gb, 0x150d, specialCollisions_disallowHoles); ret_effect(gb); }
void checkTileCollisionAt_disallowHoles_hook(GB *gb) {
  position_to_collision_addr(gb, 0x1502, 0x150d);
  tile_collision(gb, 0x150d, specialCollisions_disallowHoles);
  ret_effect(gb);
}
void objectCheckTileCollision_disallowHoles_hook(GB *gb) {
  object_position_to_bc(gb, 0x14f8);
  position_to_collision_addr(gb, 0x1502, 0x150d);
  tile_collision(gb, 0x150d, specialCollisions_disallowHoles);
  ret_effect(gb);
}

void checkCollisionPosition_disallowSmallBridges_hook(GB *gb) {
  tile_collision(gb, 0x1529, specialCollisions_disallowSmallBridges);
  ret_effect(gb);
}

static void retrieve_tile_collision_value(GB *gb) {
  H = w3TileCollisions >> 8;
  L = A;
  A = 0x03;
  CYC(0x156e, 0x1575); mem_wr(gb, IO_SVBK, A);
  CYC(0x1575, 0x1576); L = mem_rd(gb, HL);
  alu_xor(gb, A);
  CYC(0x1576, 0x1579); mem_wr(gb, IO_SVBK, A);
  A = L;
  CYC(0x1579, 0x157b);
}

void retrieveTileCollisionValue_hook(GB *gb) {
  retrieve_tile_collision_value(gb);
  ret_effect(gb);
}

static void backwards_search(GB *gb) {
  for (;;) {
    CYC(0x15d0, 0x15d1); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x15d1, 0x15d2); return; }
    L = alu_dec8(gb, L);
    if (L) { CYC(0x15d1, 0x15d3); CYCT(0x15d3, 0x15d5); continue; }
    CYC(0x15d1, 0x15d6); alu_cp(gb, mem_rd(gb, HL));
    CYC(0x15d6, 0x15d7);
    return;
  }
}

void backwardsSearch_hook(GB *gb) {
  backwards_search(gb);
  ret_effect(gb);
}

void findTileInRoom_hook(GB *gb) {
  H = wRoomLayout >> 8;
  L = 0xbf;
  CYC(0x15cc, 0x15d0);
  backwards_search(gb);
  ret_effect(gb);
}

// hazards (water, holes, lava)

static void object_check_is_over_hazard(GB *gb) {
  SET_BC(0x0500);
  CYC(0x2216, 0x221c);
  object_get_relative_tile(gb);
  CYC(0x221c, 0x221f); W8(wObjectTileIndex) = A;
  SET_HL(hazardCollisionTable);
  CYC(0x221f, 0x2225);
  lookup_collision_table(gb);
}

void objectCheckIsOverHazard_hook(GB *gb) {
  object_check_is_over_hazard(gb);
  ret_effect(gb);
}

static void object_check_is_on_hazard(GB *gb) {
  CYC(0x220d, 0x220f); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  E = A;
  CYC(0x220f, 0x2213); A = mem_rd(gb, DE);
  alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x2213, 0x2216); return; }
  CYC(0x2213, 0x2216);
  object_check_is_over_hazard(gb);
}

void objectCheckIsOnHazard_hook(GB *gb) {
  object_check_is_on_hazard(gb);
  ret_effect(gb);
}

void checkLinkIsOverHazard_hook(GB *gb) {
  CYC(0x21f6, 0x21f9); A = W8(wLinkObjectIndex);
  D = A;
  CYC(0x21f9, 0x21fc); H8(hActiveObject) = A;
  alu_xor(gb, A);
  CYC(0x21fc, 0x21ff); H8(hActiveObjectType) = A;
  E = 0x01;
  CYC(0x21ff, 0x2202); A = mem_rd(gb, DE);
  alu_sub(gb, 0x0c);
  if (F & FZ) { CYCT(0x2202, 0x2205); ret_effect(gb); return; }
  uint16_t bc = BC, hl = HL;
  CYC(0x2202, 0x220a);
  object_check_is_over_hazard(gb);
  SET_HL(hl);
  SET_BC(bc);
  CYC(0x220a, 0x220d);
  ret_effect(gb);
}

void objectCheckTileAtPositionIsWater_hook(GB *gb) {
  CYC(0x22a9, 0x22ac);
  object_get_tile_at_position(gb);
  alu_sub(gb, 0xf9);
  alu_cp(gb, 0x05);
  CYC(0x22ac, 0x22b1);
  ret_effect(gb);
}

void checkTileAtPositionIsWater_hook(GB *gb) {
  CYC(0x22b1, 0x22b4);
  tile_at_position(gb);
  alu_sub(gb, 0xf9);
  alu_cp(gb, 0x05);
  CYC(0x22b4, 0x22b9);
  ret_effect(gb);
}

// object searches by id

static void find_item_with_id(GB *gb, bool check_first) {
  for (;;) {
    if (check_first) {
      L = 0x01;
      CYC(0x22bb, 0x22be); A = mem_rd(gb, HL);
      alu_cp(gb, C);
      if (F & FZ) { CYCT(0x22be, 0x22c0); return; }
      CYC(0x22be, 0x22c0);
    }
    check_first = true;
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x22c0, 0x22c6); continue; }
    CYC(0x22c0, 0x22c6);
    alu_or(gb, H);
    CYC(0x22c6, 0x22c8);
    return;
  }
}

void findItemWithID_hook(GB *gb) {
  H = w1WeaponItem >> 8;
  CYC(0x22b9, 0x22bb);
  find_item_with_id(gb, true);
  ret_effect(gb);
}

void findItemWithID_startingAfterH_hook(GB *gb) {
  find_item_with_id(gb, false);
  ret_effect(gb);
}

static void find_same_type_object(GB *gb, bool check_first) {
  for (;;) {
    if (check_first) {
      CYC(0x22d1, 0x22d2); A = mem_rd(gb, HL);
      alu_cp(gb, C);
      if (F & FZ) { CYCT(0x22d2, 0x22d4); return; }
      CYC(0x22d2, 0x22d4);
    }
    check_first = true;
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x22d4, 0x22da); continue; }
    CYC(0x22d4, 0x22da);
    alu_or(gb, H);
    CYC(0x22da, 0x22dc);
    return;
  }
}

void objectFindSameTypeObjectWithID_hook(GB *gb) {
  CYC(0x22c8, 0x22ca); A = H8(hActiveObject);
  alu_and(gb, 0xf0);
  H = A;
  CYC(0x22ca, 0x22cf); A = H8(hActiveObjectType);
  A = alu_inc8(gb, A);
  L = A;
  CYC(0x22cf, 0x22d1);
  find_same_type_object(gb, true);
  ret_effect(gb);
}

void func_228f_hook(GB *gb) {
  find_same_type_object(gb, false);
  ret_effect(gb);
}

// priority relative to Link

static void get_priority(GB *gb) {
  CYC(0x22ee, 0x22f0); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  E = A;
  CYC(0x22f0, 0x22f4); A = mem_rd(gb, DE);
  A = alu_dec8(gb, A);
  B = 0x03;
  alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x22f4, 0x22fa); return; }
  CYC(0x22f4, 0x22fa);
  B = alu_dec8(gb, B);
  A = E;
  alu_add(gb, 0xfc);
  E = A;
  CYC(0x22fa, 0x2300); A = mem_rd(gb, DE);
  E = A;
  CYC(0x2300, 0x2304); A = W8(wLinkObjectIndex);
  H = A;
  L = OBJ_YH;
  CYC(0x2304, 0x2308); A = mem_rd(gb, HL);
  alu_add(gb, 0x0b);
  alu_cp(gb, E);
  if (!(F & FC)) { CYCT(0x2308, 0x230c); return; }
  CYC(0x2308, 0x230c);
  B = alu_dec8(gb, B);
  CYC(0x230c, 0x230e);
}

static void set_priority_relative_to_link(GB *gb) {
  CYC(0x22e2, 0x22e5);
  get_priority(gb);
  CYC(0x22e5, 0x22e7); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_VISIBLE);
  E = A;
  A = C;
  alu_or(gb, B);
  CYC(0x22e7, 0x22ed); mem_wr(gb, DE, A);
  CYC(0x22ed, 0x22ee);
}

void objectSetPriorityRelativeToLink_hook(GB *gb) {
  C = 0x80;
  CYC(0x22dc, 0x22e0);
  set_priority_relative_to_link(gb);
  ret_effect(gb);
}

void objectSetPriorityRelativeToLink_withTerrainEffects_hook(GB *gb) {
  C = 0xc0;
  CYC(0x22e0, 0x22e2);
  set_priority_relative_to_link(gb);
  ret_effect(gb);
}

// rupees and text

static void get_rupee_value(GB *gb) {
  uint16_t hl = HL;
  alu_cp(gb, 0x14);
  if (F & FC) CYCT(0x1781, 0x1786);
  else { CYC(0x1781, 0x1788); A = 0x14; }
  SET_HL(rupeeValues);
  CYC(0x1788, 0x178c);
  add_double_index_to_hl(gb);
  CYC(0x178c, 0x178d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x178d, 0x178e); B = mem_rd(gb, HL);
  C = A;
  SET_HL(hl);
  CYC(0x178e, 0x1791);
}

void getRupeeValue_hook(GB *gb) {
  get_rupee_value(gb);
  ret_effect(gb);
}

void cpRupeeValue_hook(GB *gb) {
  SET_HL(wNumRupees);
  CYC(0x1765, 0x176b);
  get_rupee_value(gb);
  CYC(0x176b, 0x176c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x176c, 0x176d); H = mem_rd(gb, HL);
  L = A;
  CYC(0x176d, 0x1771);
  compare_hl_to_bc(gb);
  A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x1771, 0x1774); alu_xor(gb, A); CYC(0x1776, 0x1778); }
  else { CYC(0x1771, 0x1774); A = alu_inc8(gb, A); CYC(0x1774, 0x1776); }
  ret_effect(gb);
}

void retIfTextIsActive_hook(GB *gb) {
  CYC(0x1859, 0x185c); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x185c, 0x185e); ret_effect(gb); return; }
  CYC(0x185c, 0x185e); SET_AF(pop_effect(gb));
  CYC(0x185e, 0x1860);
  ret_effect(gb);
}

// scaled velocity

static void scale_component(GB *gb) {
  uint16_t hl = HL;
  CYC(0x2133, 0x2134);
  CYC(0x2134, 0x2135); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(0x2135, 0x2136); B = mem_rd(gb, HL);
  E = 0x08;
  SET_HL(0);
  CYC(0x2136, 0x213e); A = H8(hFF8B);
  do {
    alu_add_hl(gb, HL);
    alu_rlca(gb);
    if (F & FC) { CYC(0x213e, 0x2142); CYC(0x2142, 0x2143); alu_add_hl(gb, BC); }
    else CYCT(0x213e, 0x2142);
    E = alu_dec8(gb, E);
    if (E) CYCT(0x2143, 0x2146); else CYC(0x2143, 0x2146);
  } while (E);
  A = L;
  B = H;
  SET_HL(hl);
  CYC(0x2146, 0x214a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x214a, 0x214b); mem_wr(gb, HL, B);
  CYC(0x214b, 0x214c);
}

static void scaled_position_offset_for_velocity(GB *gb) {
  CYC(0x212a, 0x212c); H8(hFF8B) = A;
  CYC(0x212c, 0x212f);
  position_offset_for_velocity(gb);
  CYC(0x212f, 0x2132);
  scale_component(gb);
  CYC(0x2132, 0x2133);
  L = alu_inc8(gb, L);
  scale_component(gb);
}

void getScaledPositionOffsetForVelocity_hook(GB *gb) {
  scaled_position_offset_for_velocity(gb);
  ret_effect(gb);
}

void objectSetComponentSpeedByScaledVelocity_hook(GB *gb) {
  CYC(0x214c, 0x214f);
  scaled_position_offset_for_velocity(gb);
  CYC(0x214f, 0x2151); A = H8(hActiveObjectType);
  alu_or(gb, 0x13);
  E = A;
  CYC(0x2151, 0x2155); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x2155, 0x2156); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(0x2156, 0x2158); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x2158, 0x2159); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(0x2159, 0x215b); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x215b, 0x215c); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(0x215c, 0x215e); A = mem_rd(gb, HL);
  CYC(0x215e, 0x215f); mem_wr(gb, DE, A);
  CYC(0x215f, 0x2160);
  ret_effect(gb);
}

void objectSetPositionInCircleArc_hook(GB *gb) {
  uint16_t bc = BC;
  H = D;
  L = E;
  CYC(0x210e, 0x2112); C = mem_rd(gb, HL);
  B = 0x28;
  CYC(0x2112, 0x2117);
  scaled_position_offset_for_velocity(gb);
  SET_BC(bc);
  CYC(0x2117, 0x211a); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x211a, 0x2120); A = mem_rd(gb, wTmpcec0 + 1);
  alu_add(gb, B);
  CYC(0x2120, 0x2122); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(0x2122, 0x2127); A = mem_rd(gb, wTmpcec0 + 3);
  alu_add(gb, C);
  CYC(0x2127, 0x2129); mem_wr(gb, DE, A);
  CYC(0x2129, 0x212a);
  ret_effect(gb);
}

// tile setters

void objectMakeTileSolid_hook(GB *gb) {
  CYC(0x20b2, 0x20b5);
  object_get_tile_collisions(gb);
  CYC(0x20b5, 0x20b7); mem_wr(gb, HL, 0x0f);
  CYC(0x20b7, 0x20b8);
  ret_effect(gb);
}

void setTileWithoutGfxReload_hook(GB *gb) {
  B = wRoomLayout >> 8;
  CYC(0x141c, 0x141f); mem_wr(gb, BC, A);
  CYC(0x141f, 0x1422);
  retrieve_tile_collision_value(gb);
  B = wRoomCollisions >> 8;
  CYC(0x1422, 0x1425); mem_wr(gb, BC, A);
  CYC(0x1425, 0x1426);
  ret_effect(gb);
}

void setTileInRoomLayoutBuffer_hook(GB *gb) {
  CYC(0x1426, 0x1428); uint8_t svbk = mem_rd(gb, IO_SVBK);
  CYC(0x1428, 0x142d); mem_wr(gb, IO_SVBK, 0x03);
  A = B;
  B = w3RoomLayoutBuffer >> 8;
  CYC(0x142d, 0x1431); mem_wr(gb, BC, A);
  A = svbk;
  CYC(0x1431, 0x1434); mem_wr(gb, IO_SVBK, A);
  CYC(0x1434, 0x1435);
  ret_effect(gb);
}

// palette colour components (bank $17), 3 x 5-bit components per colour, $30 colours

void extractColorComponents_hook(GB *gb) {
  CYC(0x13e9, 0x13eb); uint8_t saved = H8(hRomBank), f = F;
  CYC(0x13eb, 0x13f0); H8(hRomBank) = 0x17;
  CYC(0x13f0, 0x13f3); mem_wr(gb, MBC_ROM_BANK, 0x17);
  B = 0x30;
  CYC(0x13f3, 0x13f5);
  do {
    CYC(0x13f5, 0x13f6); C = mem_rd(gb, HL);
    SET_HL(HL + 1);
    CYC(0x13f6, 0x13f8); A = mem_rd(gb, HL);
    C = alu_sla(gb, C); alu_rla(gb);
    C = alu_rl(gb, C); alu_rla(gb);
    C = alu_rl(gb, C); alu_rla(gb);
    alu_and(gb, 0x1f);
    CYC(0x13f8, 0x1404); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(0x1404, 0x1406); A = mem_rd(gb, HL); SET_HL(HL - 1);
    alu_rra(gb);
    alu_rra(gb);
    alu_and(gb, 0x1f);
    CYC(0x1406, 0x140b); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(0x140b, 0x140d); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_and(gb, 0x1f);
    CYC(0x140d, 0x1410); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x1410, 0x1415); else CYC(0x1410, 0x1415);
  } while (B);
  A = saved;
  F = f;
  CYC(0x1415, 0x1418); H8(hRomBank) = A;
  CYC(0x1418, 0x141b); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x141b, 0x141c);
  ret_effect(gb);
}

void enableIntroInputs_hook(GB *gb) {
  CYC(0x0881, 0x0883); A = H8(hIntroInputsEnabled);
  alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x0883, 0x0886); ret_effect(gb); return; }
  CYC(0x0883, 0x0886);
  A = 0x01;
  CYC(0x0886, 0x088a); H8(hIntroInputsEnabled) = A;
  CYC(0x088a, 0x088b);
  ret_effect(gb);
}

// chests (bank $16 table by group, then position/room/contents records ending with $ff)

void getChestData_hook(GB *gb) {
  CYC(0x10cc, 0x10ce); uint8_t saved = H8(hRomBank), f = F;
  CYC(0x10ce, 0x10d3); H8(hRomBank) = 0x16;
  CYC(0x10d3, 0x10d6); mem_wr(gb, MBC_ROM_BANK, 0x16);
  CYC(0x10d6, 0x10d9); A = W8(wActiveGroup);
  SET_HL(chestDataGroupTable_bank16);
  CYC(0x10d9, 0x10dd);
  add_double_index_to_hl(gb);
  CYC(0x10dd, 0x10de); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x10de, 0x10df); H = mem_rd(gb, HL);
  L = A;
  CYC(0x10df, 0x10e3); A = W8(wActiveRoom);
  B = A;
  CYC(0x10e3, 0x10e4);
  for (;;) {
    CYC(0x10e4, 0x10e5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    A = alu_inc8(gb, A);
    if (F & FZ) { CYCT(0x10e5, 0x10e9); SET_BC(0x2800); CYC(0x10f6, 0x10f9); break; }
    CYC(0x10e5, 0x10ea); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_cp(gb, B);
    if (F & FZ) {
      CYCT(0x10ea, 0x10ed);
      CYC(0x10f1, 0x10f2); B = mem_rd(gb, HL);
      SET_HL(HL + 1);
      CYC(0x10f2, 0x10f4); C = mem_rd(gb, HL);
      CYC(0x10f4, 0x10f6);
      break;
    }
    CYC(0x10ea, 0x10f1);
    SET_HL(HL + 2);
  }
  A = saved;
  F = f;
  CYC(0x10f9, 0x10fc); H8(hRomBank) = A;
  CYC(0x10fc, 0x10ff); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x10ff, 0x1100);
  ret_effect(gb);
}

// respawn points

void setDeathRespawnPoint_hook(GB *gb) {
  static const struct { uint16_t insn, src; } copies[] = {
    {0x1103, wActiveGroup}, {0x1107, wActiveRoom}, {0x110b, wRoomStateModifier},
    {0x110f, w1Link_direction}, {0x1113, w1Link_yh}, {0x1117, w1Link_xh},
    {0x111b, wRememberedCompanionId}, {0x111f, wRememberedCompanionGroup},
    {0x1123, wRememberedCompanionRoom}, {0x1127, wLinkObjectIndex},
    {0x112c, wRememberedCompanionY}, {0x1130, wRememberedCompanionX},
  };
  SET_HL(wDeathRespawnBuffer);
  CYC(0x1100, 0x1103);
  for (int i = 0; i < 12; i++) {
    if (i == 10) { L = alu_inc8(gb, L); CYC(0x112b, 0x112c); }
    CYC(copies[i].insn, copies[i].insn + 3); A = mem_rd(gb, copies[i].src);
    CYC(copies[i].insn + 3, copies[i].insn + 4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  }
  CYC(0x1134, 0x1135);
  ret_effect(gb);
}

void updateLinkLocalRespawnPosition_hook(GB *gb) {
  CYC(0x113a, 0x113d); A = W8(wLinkObjectIndex);
  H = A;
  L = 0x08;
  CYC(0x113d, 0x1141); A = mem_rd(gb, HL);
  CYC(0x1141, 0x1144); W8(wLinkLocalRespawnDir) = A;
  L = OBJ_YH;
  CYC(0x1144, 0x1147); A = mem_rd(gb, HL);
  CYC(0x1147, 0x114a); W8(wLinkLocalRespawnY) = A;
  L = OBJ_XH;
  CYC(0x114a, 0x114d); A = mem_rd(gb, HL);
  CYC(0x114d, 0x1150); W8(wLinkLocalRespawnX) = A;
  CYC(0x1150, 0x1151);
  ret_effect(gb);
}

// bank switching around a call: ldh a,(hRomBank) / push af / ld a,n / ldh (hRomBank),a /
// ld ($2222),a ... pop af / ldh (hRomBank),a / ld ($2222),a

typedef struct { uint8_t bank, f; } SavedBank;

static SavedBank bank_push(GB *gb, uint16_t a, uint8_t bank) {
  SavedBank sb;
  CYC(a, a + 2); sb.bank = H8(hRomBank); sb.f = F;
  CYC(a + 2, a + 7); H8(hRomBank) = bank;
  CYC(a + 7, a + 10); mem_wr(gb, MBC_ROM_BANK, bank);
  return sb;
}

static void bank_pop(GB *gb, uint16_t a, SavedBank sb) {
  A = sb.bank;
  F = sb.f;
  CYC(a, a + 3); H8(hRomBank) = A;
  CYC(a + 3, a + 6); mem_wr(gb, MBC_ROM_BANK, A);
}

// room flags for broken tiles and unlocked doors

static void set_room_flag(GB *gb) {
  C = A;
  CYC(0x121c, 0x1220); A = W8(wDungeonFlagsAddressH);
  B = A;
  CYC(0x1220, 0x1222); A = mem_rd(gb, DE);
  L = A;
  CYC(0x1222, 0x1224); A = mem_rd(gb, BC);
  alu_or(gb, L);
  CYC(0x1224, 0x1226); mem_wr(gb, BC, A);
  CYC(0x1226, 0x1227);
}

static void set_room_flags_for_unlocked_key_door(GB *gb) {
  alu_and(gb, 0x0f);
  SET_DE(adjacentRoomsData);
  CYC(0x11fc, 0x1204);
  add_a_to_de(gb);
  CYC(0x1204, 0x1207); A = W8(wDungeonIndex);
  alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x1207, 0x120b);
    CYC(0x1227, 0x122a);
    this_room_flags(gb);
    CYC(0x122a, 0x122b); A = mem_rd(gb, DE);
    CYC(0x122b, 0x122c); alu_or(gb, mem_rd(gb, HL));
    CYC(0x122c, 0x122d); mem_wr(gb, HL, A);
    CYC(0x122d, 0x122e);
    return;
  }
  CYC(0x1207, 0x120b);
  CALL_ROM(0x120b, ROM_getActiveRoomFromDungeonMapPosition);
  CYC(0x120e, 0x1211);
  set_room_flag(gb);
  CYC(0x1211, 0x1212); SET_DE(DE + 1);
  CYC(0x1212, 0x1215); A = W8(wDungeonMapPosition);
  L = A;
  CYC(0x1215, 0x1217); A = mem_rd(gb, DE);
  alu_add(gb, L);
  CYC(0x1217, 0x1218);
  CALL_ROM(0x1218, ROM_getRoomInDungeon);
  CYC(0x121b, 0x121c); SET_DE(DE + 1);
  set_room_flag(gb);
}

void setRoomFlagsForUnlockedKeyDoor_hook(GB *gb) {
  set_room_flags_for_unlocked_key_door(gb);
  ret_effect(gb);
}

static void set_room_flags_for_unlocked_key_door_overworld(GB *gb) {
  alu_and(gb, 0x0f);
  SET_HL(adjacentRoomsData);
  CYC(0x123e, 0x1244);
  add_a_to_hl(gb);
  CYC(0x1244, 0x1247); A = W8(wActiveRoom);
  C = A;
  B = 0xc7;
  CYC(0x1247, 0x124b); A = mem_rd(gb, BC);
  CYC(0x124b, 0x124c); alu_or(gb, mem_rd(gb, HL));
  CYC(0x124c, 0x124d); mem_wr(gb, BC, A);
  SET_HL(HL + 1);
  CYC(0x124d, 0x124f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, C);
  C = A;
  CYC(0x124f, 0x1252); A = mem_rd(gb, BC);
  CYC(0x1252, 0x1253); alu_or(gb, mem_rd(gb, HL));
  CYC(0x1253, 0x1254); mem_wr(gb, BC, A);
  CYC(0x1254, 0x1255);
}

void setRoomFlagsForUnlockedKeyDoor_overworldOnly_hook(GB *gb) {
  set_room_flags_for_unlocked_key_door_overworld(gb);
  ret_effect(gb);
}

void updateRoomFlagsForBrokenTile_hook(GB *gb) {
  uint16_t af = AF;
  CYC(0x1151, 0x1152);
  SET_HL(tileIncreaseGashaMaturityOnBreakTable);
  CYC(0x1152, 0x1158);
  lookup_collision_table(gb);
  if (F & FC) { CYCT(0x1158, 0x115b); push_effect(gb, 0x115b); asm_call(gb, ROM_addToGashaMaturity, 0x115b); }
  else CYC(0x1158, 0x115b);
  SET_AF(af);
  CYC(0x115b, 0x115c);
  SET_HL(tileUpdateRoomFlagsOnBreakTable);
  CYC(0x115c, 0x1162);
  lookup_collision_table(gb);
  if (!(F & FC)) { CYCT(0x1162, 0x1163); ret_effect(gb); return; }
  CYC(0x1162, 0x1163);
  alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x1163, 0x1168); set_room_flags_for_unlocked_key_door(gb); ret_effect(gb); return; }
  CYC(0x1163, 0x1168);
  alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x1168, 0x116d); set_room_flags_for_unlocked_key_door_overworld(gb); ret_effect(gb); return; }
  CYC(0x1168, 0x116d);
  alu_and(gb, 0x0f);
  SET_BC(bitTable);
  alu_add(gb, C);
  C = A;
  CYC(0x116d, 0x1177); A = W8(wActiveGroup);
  SET_HL(flagLocationGroupTable);
  CYC(0x1177, 0x117b);
  add_a_to_hl(gb);
  CYC(0x117b, 0x117c); H = mem_rd(gb, HL);
  CYC(0x117c, 0x117f); A = W8(wActiveRoom);
  L = A;
  CYC(0x117f, 0x1181); A = mem_rd(gb, BC);
  CYC(0x1181, 0x1182); alu_or(gb, mem_rd(gb, HL));
  CYC(0x1182, 0x1183); mem_wr(gb, HL, A);
  CYC(0x1183, 0x1184);
  ret_effect(gb);
}

void checkAndUpdateLinkOnChest_hook(GB *gb) {
  CYC(0x1255, 0x1258); A = W8(wLinkOnChest);
  alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x1258, 0x125b);
    C = A;
    CYC(0x126d, 0x1271); A = W8(wActiveTilePos);
    alu_cp(gb, C);
    if (F & FZ) { CYCT(0x1271, 0x1273); ret_effect(gb); return; }
    CYC(0x1271, 0x1273);
    B = wRoomLayout >> 8;
    CYC(0x1273, 0x1276); A = mem_rd(gb, BC);
    CYC(0x1276, 0x1279);
    retrieve_tile_collision_value(gb);
    B = alu_dec8(gb, B);
    CYC(0x1279, 0x127b); mem_wr(gb, BC, A);
    alu_xor(gb, A);
    CYC(0x127b, 0x127f); W8(wLinkOnChest) = A;
    CYC(0x127f, 0x1280);
    ret_effect(gb);
    return;
  }
  CYC(0x1258, 0x125e); A = W8(wActiveTileIndex);
  alu_cp(gb, 0xf1);
  if (!(F & FZ)) { CYCT(0x125e, 0x1261); ret_effect(gb); return; }
  CYC(0x125e, 0x1264); A = W8(wActiveTilePos);
  CYC(0x1264, 0x1267); W8(wLinkOnChest) = A;
  L = A;
  H = wRoomCollisions >> 8;
  CYC(0x1267, 0x126c); mem_wr(gb, HL, 0x00);
  CYC(0x126c, 0x126d);
  ret_effect(gb);
}

void showInfoTextForRoller_hook(GB *gb) {
  SavedBank sb = bank_push(gb, 0x1298, 0x06);
  A = 0x09;
  CYC(0x12a2, 0x12a4);
  CALL_ROM(0x12a4, ROM_b06_showInfoTextForTile);
  bank_pop(gb, 0x12a7, sb);
  CYC(0x12ad, 0x12ae);
  ret_effect(gb);
}

// camera

void updateCamera_hook(GB *gb) {
  CYC(0x12ae, 0x12b1); A = W8(wScreenVariables);
  alu_and(gb, 0x05);
  if (F & FZ) { CYCT(0x12b1, 0x12b4); ret_effect(gb); return; }
  CYC(0x12b1, 0x12b4);
  SavedBank sb = bank_push(gb, 0x12b4, 0x01);
  CALL_ROM(0x12be, ROM_b01_updateCameraPosition);
  CALL_ROM(0x12c1, ROM_b01_updateGfxRegs2Scroll);
  CALL_ROM(0x12c4, ROM_b01_updateScreenShake);
  bank_pop(gb, 0x12c7, sb);
  CYC(0x12cd, 0x12ce);
  ret_effect(gb);
}

void resetCamera_hook(GB *gb) {
  SavedBank sb = bank_push(gb, 0x12ce, 0x01);
  CALL_ROM(0x12d8, ROM_b01_calculateCameraPosition);
  CALL_ROM(0x12db, ROM_b01_updateGfxRegs2Scroll);
  bank_pop(gb, 0x12de, sb);
  CYC(0x12e4, 0x12e5);
  ret_effect(gb);
}

void setCameraFocusedObject_hook(GB *gb) {
  CYC(0x12e5, 0x12e7); A = H8(hActiveObject);
  CYC(0x12e7, 0x12ea); W8(wCameraFocusedObject) = A;
  CYC(0x12ea, 0x12ec); A = H8(hActiveObjectType);
  CYC(0x12ec, 0x12ef); W8(wCameraFocusedObjectType) = A;
  CYC(0x12ef, 0x12f0);
  ret_effect(gb);
}

void setCameraFocusedObjectToLink_hook(GB *gb) {
  CYC(0x12f0, 0x12f3); A = W8(wLinkObjectIndex);
  CYC(0x12f3, 0x12f6); W8(wCameraFocusedObject) = A;
  A = 0x00;
  CYC(0x12f6, 0x12fb); W8(wCameraFocusedObjectType) = A;
  CYC(0x12fb, 0x12fc);
  ret_effect(gb);
}

void reloadTileMap_hook(GB *gb) {
  CYC(0x12fc, 0x12fe); SavedBank sb = {H8(hRomBank), F};
  CYC(0x12fe, 0x12ff);
  alu_xor(gb, A);
  CYC(0x12ff, 0x1303); W8(wScreenOffsetY) = A;
  CYC(0x1303, 0x1306); W8(wScreenOffsetX) = A;
  A = 0x10;
  CYC(0x1306, 0x1308);
  CALL_ROM(0x1308, ROM_loadUncompressedGfxHeader);
  CYC(0x130b, 0x130f); H8(hRomBank) = 0x01;
  CYC(0x130f, 0x1312); mem_wr(gb, MBC_ROM_BANK, 0x01);
  CALL_ROM(0x1312, ROM_b01_setScreenTransitionState02);
  CALL_ROM(0x1315, ROM_b01_updateGfxRegs2Scroll);
  bank_pop(gb, 0x1318, sb);
  CYC(0x131e, 0x131f);
  ret_effect(gb);
}

// wave scroll (bank 1 bodies, WRAM bank preserved around them)

static void wave_scroll_call(GB *gb, uint16_t a, uint16_t target, bool arg_in_b) {
  CYC(a, a + 2); H8(hFF93) = A;
  CYC(a + 2, a + 4); C = mem_rd(gb, IO_SVBK);
  CYC(a + 4, a + 7); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(a + 7, a + 13); H8(hRomBank) = 0x01;
  CYC(a + 13, a + 16); mem_wr(gb, MBC_ROM_BANK, 0x01);
  CYC(a + 16, a + 18); A = H8(hFF93);
  if (arg_in_b) B = A; else C = A;
  CYC(a + 18, a + 19);
  CALL_ROM(a + 19, target);
  SET_BC(bc);
  A = B;
  CYC(a + 22, a + 26); H8(hRomBank) = A;
  CYC(a + 26, a + 29); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(a + 29, a + 32); mem_wr(gb, IO_SVBK, A);
  CYC(a + 32, a + 33);
}

void initWaveScrollValues_hook(GB *gb) {
  wave_scroll_call(gb, 0x1384, ROM_b01_initWaveScrollValues_body, false);
  ret_effect(gb);
}

void loadBigBufferScrollValues_hook(GB *gb) {
  wave_scroll_call(gb, 0x13a5, ROM_b01_loadBigBufferScrollValues_body, true);
  ret_effect(gb);
}

// room collisions from the layout and the bank 3 collision table

static void blank_collisions_horizontally(GB *gb) {
  A = 0xff;
  B = 0x10;
  CYC(0x15b6, 0x15ba);
  do {
    CYC(0x15ba, 0x15bb); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x15bb, 0x15be); else CYC(0x15bb, 0x15be);
  } while (B);
  CYC(0x15be, 0x15bf);
}

static void blank_collisions_vertically(GB *gb) {
  B = 0x0b;
  C = 0xff;
  CYC(0x15bf, 0x15c3);
  do {
    CYC(0x15c3, 0x15c4); mem_wr(gb, HL, C);
    A = L;
    alu_add(gb, 0x10);
    L = A;
    B = alu_dec8(gb, B);
    if (B) CYCT(0x15c4, 0x15cb); else CYC(0x15c4, 0x15cb);
  } while (B);
  CYC(0x15cb, 0x15cc);
}

static void blank_data_around_collisions(GB *gb) {
  SET_HL(wRoomCollisions + 0xf0);
  CYC(0x1596, 0x159c);
  blank_collisions_horizontally(gb);
  SET_HL(wRoomCollisions + 0x0f);
  CYC(0x159c, 0x15a2);
  blank_collisions_vertically(gb);
  CYC(0x15a2, 0x15a5); A = W8(wActiveGroup);
  alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(0x15a5, 0x15a9);
    L = 0x80;
    CYC(0x15ad, 0x15b2);
    blank_collisions_horizontally(gb);
    L = 0x0a;
    CYC(0x15b2, 0x15b6);
    blank_collisions_vertically(gb);
  } else {
    CYC(0x15a5, 0x15a9);
    L = 0xb0;
    CYC(0x15a9, 0x15ad);
    blank_collisions_horizontally(gb);
  }
}

void loadRoomCollisions_hook(GB *gb) {
  A = 0x03;
  CYC(0x157b, 0x157f); mem_wr(gb, IO_SVBK, A);
  D = w3TileCollisions >> 8;
  SET_HL(wRoomLayout);
  B = 0xb0;
  CYC(0x157f, 0x1586);
  do {
    CYC(0x1586, 0x1587); A = mem_rd(gb, HL);
    E = A;
    CYC(0x1587, 0x1589); A = mem_rd(gb, DE);
    H = alu_dec8(gb, H);
    CYC(0x1589, 0x158b); mem_wr(gb, HL, A); SET_HL(HL + 1);
    H = alu_inc8(gb, H);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x158b, 0x158f); else CYC(0x158b, 0x158f);
  } while (B);
  CYC(0x158f, 0x1592);
  blank_data_around_collisions(gb);
  alu_xor(gb, A);
  CYC(0x1592, 0x1595); mem_wr(gb, IO_SVBK, A);
  CYC(0x1595, 0x1596);
  ret_effect(gb);
}

// treasure and item drops (bank $3f bodies)

void decideItemDrop_hook(GB *gb) {
  C = A;
  CYC(0x16eb, 0x16ec);
  SavedBank sb = bank_push(gb, 0x16ec, 0x3f);
  CALL_ROM(0x16f6, ROM_b3f_decideItemDrop_body);
  bank_pop(gb, 0x16f9, sb);
  A = C;
  alu_cp(gb, 0xff);
  CYC(0x16ff, 0x1703);
  ret_effect(gb);
}

void checkItemDropAvailable_hook(GB *gb) {
  C = A;
  CYC(0x1703, 0x1704);
  SavedBank sb = bank_push(gb, 0x1704, 0x3f);
  A = C;
  CYC(0x170e, 0x170f);
  CALL_ROM(0x170f, ROM_b3f_checkItemDropAvailable_body);
  bank_pop(gb, 0x1712, sb);
  A = C;
  alu_cp(gb, 0xff);
  CYC(0x1718, 0x171c);
  ret_effect(gb);
}

static void give_treasure(GB *gb) {
  B = A;
  CYC(0x171c, 0x171d);
  SavedBank sb = bank_push(gb, 0x171d, 0x3f);
  CALL_ROM(0x1727, ROM_b3f_giveTreasure_body);
  bank_pop(gb, 0x172a, sb);
  A = B;
  alu_or(gb, A);
  CYC(0x1730, 0x1733);
}

void giveTreasure_hook(GB *gb) {
  give_treasure(gb);
  ret_effect(gb);
}

void loseTreasure_hook(GB *gb) {
  B = A;
  CYC(0x1733, 0x1734);
  SavedBank sb = bank_push(gb, 0x1734, 0x3f);
  CALL_ROM(0x173e, ROM_b3f_loseTreasure_body);
  bank_pop(gb, 0x1741, sb);
  CYC(0x1747, 0x1748);
  ret_effect(gb);
}

void removeRupeeValue_hook(GB *gb) {
  SET_HL(wNumRupees);
  CYC(0x1778, 0x177e);
  get_rupee_value(gb);
  CYC(0x177e, 0x1781);
  sub_decimal_from_hl_ref(gb);
  ret_effect(gb);
}

void getRandomRingOfGivenTier_hook(GB *gb) {
  SavedBank sb = bank_push(gb, 0x17e0, 0x3f);
  B = 0x01;
  A = C;
  alu_cp(gb, 0x04);
  if (F & FZ) CYCT(0x17ea, 0x17f1);
  else { CYC(0x17ea, 0x17f3); B = 0x07; }
  SET_HL(ringTierTable_bank3f);
  CYC(0x17f3, 0x17f7);
  add_double_index_to_hl(gb);
  CYC(0x17f7, 0x17f8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x17f8, 0x17f9); H = mem_rd(gb, HL);
  L = A;
  CYC(0x17f9, 0x17fd);
  A = getRandomNumber(gb);
  alu_and(gb, B);
  C = A;
  B = 0x00;
  alu_add_hl(gb, BC);
  CYC(0x17fd, 0x1802);
  CYC(0x1802, 0x1803); C = mem_rd(gb, HL);
  bank_pop(gb, 0x1803, sb);
  A = 0x2d;
  CYC(0x1809, 0x180c);
  ret_effect(gb);
}

void refillSeedSatchel_hook(GB *gb) {
  E = 0x20;
  CYC(0x180c, 0x180e);
  for (;;) {
    A = E;
    CYC(0x180e, 0x180f);
    CALL_ROM(0x180f, ROM_checkTreasureObtained);
    if (!(F & FC)) CYCT(0x1812, 0x1814);
    else {
      CYC(0x1812, 0x1814);
      A = E;
      C = 0x99;
      CYC(0x1814, 0x181a);
      give_treasure(gb);
    }
    E = alu_inc8(gb, E);
    A = E;
    alu_cp(gb, 0x25);
    if (F & FC) { CYCT(0x181a, 0x1820); continue; }
    CYC(0x181a, 0x1820);
    break;
  }
  CYC(0x1820, 0x1821);
  ret_effect(gb);
}

void addToGashaMaturity_hook(GB *gb) {
  uint16_t hl = HL;
  SET_HL(wGashaMaturity);
  CYC(0x1821, 0x1826); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1826, 0x1827); mem_wr(gb, HL, A); SET_HL(HL + 1);
  if (!(F & FC)) CYCT(0x1827, 0x1829);
  else {
    CYC(0x1827, 0x182a); uint8_t v = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
    if (v) CYCT(0x182a, 0x182c);
    else {
      CYC(0x182a, 0x182f); A = 0xff; mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(0x182f, 0x1830); mem_wr(gb, HL, A);
    }
  }
  SET_HL(hl);
  CYC(0x1830, 0x1832);
  ret_effect(gb);
}

// room-specific data: table of per-group pointers to (room, value) lists

void findRoomSpecificData_hook(GB *gb) {
  E = A;
  CYC(0x1dfe, 0x1e02); A = W8(wActiveGroup);
  CYC(0x1e02, 0x1e03);
  add_double_index_to_hl(gb);
  CYC(0x1e03, 0x1e04); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1e04, 0x1e05); H = mem_rd(gb, HL);
  L = A;
  CYC(0x1e05, 0x1e06);
  lookup_key(gb);
  ret_effect(gb);
}

// object deletion and replacement

static void remove_from_abutton_list(GB *gb);

static void object_delete_de(GB *gb) {
  A = E;
  alu_and(gb, 0xc0);
  E = A;
  L = A;
  H = D;
  B = 0x10;
  alu_xor(gb, A);
  CYC(0x21e3, 0x21ec);
  do {
    CYC(0x21ec, 0x21ed); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x21ed, 0x21ee); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x21ee, 0x21ef); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x21ef, 0x21f0); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x21f0, 0x21f3); else CYC(0x21f0, 0x21f3);
  } while (B);
  CYC(0x21f3, 0x21f6);
  remove_from_abutton_list(gb);
}

void objectDelete_de_hook(GB *gb) { object_delete_de(gb); ret_effect(gb); }

static void object_delete_use_active_type(GB *gb) {
  CYC(0x21e0, 0x21e2); E = H8(hActiveObjectType);
  CYC(0x21e2, 0x21e3);
  object_delete_de(gb);
}

void objectDelete_useActiveObjectType_hook(GB *gb) { object_delete_use_active_type(gb); ret_effect(gb); }

void objectReplaceWithID_hook(GB *gb) {
  uint16_t bc0 = BC;
  H = D;
  CYC(0x21b0, 0x21b4); A = H8(hActiveObjectType);
  L = A;
  CYC(0x21b4, 0x21b6); B = mem_rd(gb, HL);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(0x21b6, 0x21ba); C = mem_rd(gb, HL);
  uint16_t bc1 = BC;
  L = alu_inc8(gb, L); L = alu_inc8(gb, L);
  CYC(0x21ba, 0x21be); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L); L = alu_inc8(gb, L);
  CYC(0x21be, 0x21c1); C = mem_rd(gb, HL);
  uint16_t bc2 = BC;
  CYC(0x21c1, 0x21c5);
  object_delete_use_active_type(gb);
  SET_BC(bc2);
  H = D;
  A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  L = A;
  CYC(0x21c5, 0x21cd); mem_wr(gb, HL, C);
  L = alu_dec8(gb, L); L = alu_dec8(gb, L);
  CYC(0x21cd, 0x21d0); mem_wr(gb, HL, B);
  SET_BC(bc1);
  L = alu_dec8(gb, L); L = alu_dec8(gb, L);
  CYC(0x21d0, 0x21d4); mem_wr(gb, HL, C);
  A = H8(hActiveObjectType);
  L = A;
  A = B;
  alu_and(gb, 0x03);
  CYC(0x21d4, 0x21db); mem_wr(gb, HL, A); SET_HL(HL + 1);
  SET_BC(bc0);
  CYC(0x21db, 0x21dd); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x21dd, 0x21df); mem_wr(gb, HL, C);
  CYC(0x21df, 0x21e0);
  ret_effect(gb);
}

void checkInteractionSubstate_hook(GB *gb) {
  E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x2403, 0x2406); A = mem_rd(gb, DE);
  alu_or(gb, A);
  CYC(0x2406, 0x2408);
  ret_effect(gb);
}

static void set_collide_radii(GB *gb) {
  CYC(0x24a9, 0x24ab); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_COLLIDE_RADIUS_Y);
  L = A;
  H = D;
  CYC(0x24ab, 0x24b0); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x24b0, 0x24b2); mem_wr(gb, HL, C);
  CYC(0x24b2, 0x24b3);
}

void objectSetCollideRadii_hook(GB *gb) {
  set_collide_radii(gb);
  ret_effect(gb);
}

void objectSetCollideRadius_hook(GB *gb) {
  uint16_t bc = BC;
  B = A;
  C = A;
  CYC(0x24a1, 0x24a7);
  set_collide_radii(gb);
  SET_BC(bc);
  CYC(0x24a7, 0x24a9);
  ret_effect(gb);
}

void decNumEnemies_hook(GB *gb) {
  SET_HL(wNumEnemies);
  CYC(0x24b3, 0x24b7); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x24b7, 0x24b9); ret_effect(gb); return; }
  CYC(0x24b7, 0x24ba); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x24ba, 0x24bb);
  ret_effect(gb);
}

void setScreenShakeCounter_hook(GB *gb) {
  SET_HL(wScreenShakeCounterY);
  CYC(0x24bb, 0x24bf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x24bf, 0x24c0); mem_wr(gb, HL, A);
  CYC(0x24c0, 0x24c1);
  ret_effect(gb);
}

static void object_create_interaction(GB *gb) {
  CALL_ROM(0x24c5, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(0x24c8, 0x24c9); return; }
  CYC(0x24c8, 0x24ca); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x24ca, 0x24cc); mem_wr(gb, HL, C);
  CYC(0x24cc, 0x24cf);
  object_copy_position(gb);
  alu_xor(gb, A);
  CYC(0x24cf, 0x24d1);
}

void objectCreateInteraction_hook(GB *gb) {
  object_create_interaction(gb);
  ret_effect(gb);
}

void objectCreateInteractionWithSubid00_hook(GB *gb) {
  C = 0x00;
  CYC(0x24c3, 0x24c5);
  object_create_interaction(gb);
  ret_effect(gb);
}

void objectCreatePuff_hook(GB *gb) {
  B = 0x05;
  CYC(0x24c1, 0x24c3);
  C = 0x00;
  CYC(0x24c3, 0x24c5);
  object_create_interaction(gb);
  ret_effect(gb);
}

static void object_create_falling_down_hole(GB *gb) {
  CALL_ROM(0x24d1, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(0x24d4, 0x24d5); return; }
  CYC(0x24d4, 0x24d7); mem_wr(gb, HL, 0x0f);
  L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x24d7, 0x24db); A = H8(hActiveObjectType);
  CYC(0x24db, 0x24dc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_add(gb, 0x01);
  E = A;
  CYC(0x24dc, 0x24e0); A = mem_rd(gb, DE);
  CYC(0x24e0, 0x24e1); mem_wr(gb, HL, A);
  CYC(0x24e1, 0x24e4);
  object_copy_position(gb);
  alu_xor(gb, A);
  CYC(0x24e4, 0x24e6);
}

void objectCreateFallingDownHoleInteraction_hook(GB *gb) {
  object_create_falling_down_hole(gb);
  ret_effect(gb);
}

void objectFlickerVisibility_hook(GB *gb) {
  CYC(0x24e6, 0x24e9); A = W8(wFrameCounter);
  alu_and(gb, B);
  if (F & FZ) { CYCT(0x24e9, 0x24ed); obj_visible_bit(gb, 0x1e7b, false); }
  else { CYC(0x24e9, 0x24ed); CYC(0x24ed, 0x24f0); obj_visible_bit(gb, 0x1e84, true); }
  ret_effect(gb);
}

void objectUnmarkSolidPosition_hook(GB *gb) {
  CYC(0x2504, 0x2507);
  object_get_short_position(gb);
  B = A;
  A = 0x02;
  CYC(0x2507, 0x250c); mem_wr(gb, IO_SVBK, A);
  A = B;
  SET_HL(w2SolidObjectPositions);
  CYC(0x250c, 0x2513);
  unset_flag(gb);
  A = 0x00;
  CYC(0x2513, 0x2517); mem_wr(gb, IO_SVBK, A);
  CYC(0x2517, 0x2518);
  ret_effect(gb);
}

// interaction text and scripts

void interactionSetHighTextIndex_hook(GB *gb) {
  E = 0x73;
  CYC(0x253b, 0x253e); mem_wr(gb, DE, A);
  E = 0x70;
  A |= 0x80;
  CYC(0x253e, 0x2543); mem_wr(gb, DE, A);
  CYC(0x2543, 0x2544);
  ret_effect(gb);
}

void interactionSetScript_hook(GB *gb) {
  E = 0x58;
  A = L;
  CYC(0x2544, 0x2548); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(0x2548, 0x254b); mem_wr(gb, DE, A);
  H = D;
  L = INTERACTION_BASE + OBJ_COUNTER1;
  alu_xor(gb, A);
  CYC(0x254b, 0x2550); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2550, 0x2551); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2551, 0x2552);
  ret_effect(gb);
}

// mimic a background tile: oam data from the tile mapping, bg palette copied to spr palette 6

void objectMimicBgTile_hook(GB *gb) {
  CALL_ROM(0x233b, ROM_getTileMappingData);
  H = D;
  CYC(0x233e, 0x2341); A = H8(hActiveObjectType);
  alu_add(gb, 0x1b);
  L = A;
  A = 0x0e;
  CYC(0x2341, 0x2347); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2347, 0x2348); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2348, 0x2349); mem_wr(gb, HL, C);
  A = B;
  alu_and(gb, 0x07);
  alu_swap_a(gb);
  alu_rrca(gb);
  SET_BC(w2TilesetBgPalettes);
  CYC(0x2349, 0x2355);
  add_a_to_bc(gb);
  CYC(0x2355, 0x2357); uint8_t svbk = mem_rd(gb, IO_SVBK), f = F;
  CYC(0x2357, 0x235c); mem_wr(gb, IO_SVBK, 0x02);
  SET_HL(w2TilesetBgPalettes + 0x70);
  E = 0x08;
  CYC(0x235c, 0x2361);
  do {
    CYC(0x2361, 0x2362); A = mem_rd(gb, BC);
    CYC(0x2362, 0x2363); mem_wr(gb, HL, A); SET_HL(HL + 1);
    C = alu_inc8(gb, C);
    E = alu_dec8(gb, E);
    if (E) CYCT(0x2363, 0x2367); else CYC(0x2363, 0x2367);
  } while (E);
  SET_HL(hDirtySprPalettes);
  CYC(0x2367, 0x236c); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  A = svbk;
  F = f;
  CYC(0x236c, 0x236f); mem_wr(gb, IO_SVBK, A);
  CYC(0x236f, 0x2370);
  ret_effect(gb);
}

// bank copies

void copy8BytesFromRingMapToCec0_hook(GB *gb) {
  SavedBank sb = bank_push(gb, 0x1af7, 0x1c);
  SET_DE(wTmpcec0);
  B = 0x08;
  CYC(0x1b01, 0x1b09);
  copyMemory(gb, DE, HL, 8);
  SET_HL(HL + 8);
  SET_DE(DE + 8);
  B = 0;
  bank_pop(gb, 0x1b09, sb);
  CYC(0x1b0f, 0x1b10);
  ret_effect(gb);
}

void copy20BytesFromBank_hook(GB *gb) {
  CYC(0x1a83, 0x1a85); uint8_t saved = H8(hRomBank), f = F;
  A = B;
  CYC(0x1a85, 0x1a89); H8(hRomBank) = A;
  CYC(0x1a89, 0x1a8c); mem_wr(gb, MBC_ROM_BANK, A);
  B = 0x20;
  CYC(0x1a8c, 0x1a91);
  copyMemory(gb, DE, HL, 0x20);
  SET_HL(HL + 0x20);
  SET_DE(DE + 0x20);
  B = 0;
  A = saved;
  F = f;
  CYC(0x1a91, 0x1a94); H8(hRomBank) = A;
  CYC(0x1a94, 0x1a97); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x1a97, 0x1a98);
  ret_effect(gb);
}

// A-button sensitive object list: (object high byte, variable low byte) pairs, $ccb3-$ccd2

void objectAddToAButtonSensitiveObjectList_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x1b2c, 0x1b2e); mem_wr(gb, DE, A);
  SET_HL(wAButtonSensitiveObjectList);
  CYC(0x1b2e, 0x1b31);
  for (;;) {
    CYC(0x1b31, 0x1b32); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x1b32, 0x1b33); alu_or(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x1b33, 0x1b35);
      A = E;
      CYC(0x1b3c, 0x1b3e); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(0x1b3e, 0x1b3f); mem_wr(gb, HL, D);
      alu_scf(gb);
      CYC(0x1b3f, 0x1b41);
      break;
    }
    CYC(0x1b33, 0x1b35);
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, 0xd3);
    if (F & FC) { CYCT(0x1b35, 0x1b3b); continue; }
    CYC(0x1b35, 0x1b3c);
    break;
  }
  ret_effect(gb);
}

static void remove_from_abutton_list(GB *gb) {
  uint16_t de = DE;
  A = E;
  alu_and(gb, 0xc0);
  E = A;
  SET_HL(wAButtonSensitiveObjectList);
  CYC(0x1b41, 0x1b49);
  for (;;) {
    CYC(0x1b49, 0x1b4a); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_cp(gb, D);
    if (!(F & FZ)) CYCT(0x1b4a, 0x1b4d);
    else {
      CYC(0x1b4a, 0x1b4e); A = mem_rd(gb, HL);
      alu_and(gb, 0xc0);
      alu_sub(gb, E);
      if (!(F & FZ)) CYCT(0x1b4e, 0x1b53);
      else {
        CYC(0x1b4e, 0x1b54); mem_wr(gb, HL, A); SET_HL(HL - 1);
        CYC(0x1b54, 0x1b55); mem_wr(gb, HL, A); SET_HL(HL + 1);
      }
    }
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, 0xd3);
    if (F & FC) { CYCT(0x1b55, 0x1b5b); continue; }
    CYC(0x1b55, 0x1b5d);
    break;
  }
  SET_DE(de);
}

void objectRemoveFromAButtonSensitiveObjectList_hook(GB *gb) {
  remove_from_abutton_list(gb);
  ret_effect(gb);
}

void checkEnemyAndPartCollisionsIfTextInactive_hook(GB *gb) {
  CYC(0x1de7, 0x1dea);
  CYC(0x1859, 0x185c); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(0x185c, 0x185e); SET_AF(0x1dea); CYC(0x185e, 0x1860); ret_effect(gb); return; }
  CYCT(0x185c, 0x185e);
  SavedBank sb = bank_push(gb, 0x1dea, 0x07);
  CALL_ROM(0x1df4, ROM_b07_checkEnemyAndPartCollisions);
  bank_pop(gb, 0x1df7, sb);
  CYC(0x1dfd, 0x1dfe);
  ret_effect(gb);
}

// replace an object with a hazard animation and delete it

static void delete_and_scf(GB *gb) {
  CYC(0x223d, 0x2240);
  object_delete_use_active_type(gb);
  alu_scf(gb);
  CYC(0x2240, 0x2242);
}

static void replace_with_interaction_b(GB *gb) {
  CYC(0x223a, 0x223d);
  C = 0x00;
  CYC(0x24c3, 0x24c5);
  object_create_interaction(gb);
  delete_and_scf(gb);
}

static void replace_with_splash(GB *gb) {
  B = 0x03;
  CYC(0x2238, 0x223a);
  replace_with_interaction_b(gb);
}

static void replace_with_falling_down_hole(GB *gb) {
  CYC(0x2233, 0x2236);
  object_create_falling_down_hole(gb);
  CYC(0x2236, 0x2238);
  delete_and_scf(gb);
}

void objectReplaceWithSplash_hook(GB *gb) { replace_with_splash(gb); ret_effect(gb); }
void objectReplaceWithFallingDownHoleInteraction_hook(GB *gb) { replace_with_falling_down_hole(gb); ret_effect(gb); }

void objectReplaceWithAnimationIfOnHazard_hook(GB *gb) {
  CYC(0x2225, 0x2228);
  object_check_is_on_hazard(gb);
  if (!(F & FC)) { CYCT(0x2228, 0x2229); ret_effect(gb); return; }
  CYC(0x2228, 0x2229);
  alu_rrca(gb);
  if (F & FC) { CYCT(0x2229, 0x222c); replace_with_splash(gb); ret_effect(gb); return; }
  CYC(0x2229, 0x222c);
  alu_rrca(gb);
  if (F & FC) { CYCT(0x222c, 0x222f); replace_with_falling_down_hole(gb); ret_effect(gb); return; }
  CYC(0x222c, 0x222f);
  B = 0x04;
  CYC(0x222f, 0x2233);
  replace_with_interaction_b(gb);
  ret_effect(gb);
}

void breakCrackedFloor_hook(GB *gb) {
  uint16_t bc = BC;
  CYC(0x2291, 0x2292);
  CALL_ROM(0x2292, ROM_setTile);
  SET_BC(bc);
  A = 0xb3;
  CYC(0x2295, 0x2298);
  CALL_ROM(0x2298, ROM_playSound);
  CALL_ROM(0x229b, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(0x229e, 0x229f); ret_effect(gb); return; }
  CYC(0x229e, 0x22a1); mem_wr(gb, HL, 0x0f);
  L = alu_inc8(gb, L);
  CYC(0x22a1, 0x22a4); mem_wr(gb, HL, 0x80);
  L = 0x4b;
  CYC(0x22a4, 0x22a9);
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

// Link pushed by moving objects (bank 5 body)

static void update_link_position_given_velocity(GB *gb) {
  SavedBank sb = bank_push(gb, 0x231e, 0x05);
  uint16_t de = DE;
  CYC(0x2328, 0x232c); A = W8(wLinkObjectIndex);
  D = A;
  E = 0x00;
  CYC(0x232c, 0x232f);
  CALL_ROM(0x232f, ROM_b05_specialObjectUpdatePositionGivenVelocity);
  SET_DE(de);
  CYC(0x2332, 0x2333);
  bank_pop(gb, 0x2333, sb);
  alu_scf(gb);
  CYC(0x2339, 0x233b);
}

void updateLinkPositionGivenVelocity_hook(GB *gb) {
  update_link_position_given_velocity(gb);
  ret_effect(gb);
}

void objectPushLinkAwayOnCollision_hook(GB *gb) {
  CYC(0x230e, 0x2311); A = W8(wLinkObjectIndex);
  H = A;
  L = 0x00;
  CYC(0x2311, 0x2317);
  objects_collided(gb);
  if (!(F & FC)) { CYCT(0x2317, 0x2318); ret_effect(gb); return; }
  CYC(0x2317, 0x2318);
  CYC(0x2318, 0x231b);
  push_effect(gb, 0x231b);
  angle_toward_enemy_target(gb);
  C = A;
  B = 0x28;
  CYC(0x231b, 0x231e);
  update_link_position_given_velocity(gb);
  ret_effect(gb);
}

// interaction scripts

void interactionRunScript_hook(GB *gb) {
  CYC(0x2552, 0x2555); A = W8(wLinkDeathTrigger);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x2555, 0x2557); ret_effect(gb); return; }
  CYC(0x2555, 0x255a); A = W8(wTextIsActive);
  alu_add(gb, A);
  if (F & FC) CYCT(0x255a, 0x255d);
  else if (!(F & FZ)) { CYC(0x255a, 0x255d); CYCT(0x255d, 0x255e); ret_effect(gb); return; }
  else CYC(0x255a, 0x255e);
  H = D;
  L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x255e, 0x2562); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) CYCT(0x2562, 0x2565);
  else {
    CYC(0x2562, 0x2566); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
    if (v) { CYCT(0x2566, 0x2567); ret_effect(gb); return; }
    CYC(0x2566, 0x2567);
  }
  L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x2567, 0x256a); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) CYCT(0x256a, 0x256d);
  else {
    CYC(0x256a, 0x256e); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
    if (v) { CYCT(0x256e, 0x2571); object_apply_speed(gb); }
    else CYC(0x256e, 0x2571);
    alu_xor(gb, A);
    CYC(0x2571, 0x2573);
    ret_effect(gb);
    return;
  }
  H = D;
  L = 0x58;
  CYC(0x2573, 0x2577); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2577, 0x2578); H = mem_rd(gb, HL);
  L = A;
  CYC(0x2578, 0x2579);
  CALL_ROM(0x2579, ROM__interactionActuallyRunScript);
  if (F & FC) {
    CYCT(0x257c, 0x257e);
    CALL_ROM(0x2583, ROM__interactionSaveScriptAddress);
    alu_scf(gb);
    CYC(0x2586, 0x2588);
    ret_effect(gb);
    return;
  }
  CYC(0x257c, 0x257e);
  CALL_ROM(0x257e, ROM__interactionSaveScriptAddress);
  alu_xor(gb, A);
  CYC(0x2581, 0x2583);
  ret_effect(gb);
}

// keep object h (Link) from passing through object d

static void set_both_object_variables(GB *gb, uint16_t call_addr) {
  CYC(call_addr, call_addr + 3);
  CYC(0x1dde, 0x1de0); A = H8(hActiveObjectType);
  alu_or(gb, B);
  E = A;
  CYC(0x1de0, 0x1de4); A = H8(hFF8B);
  alu_or(gb, B);
  L = A;
  CYC(0x1de4, 0x1de7);
}

static void check_collision_direction(GB *gb) {
  CYC(0x1d88, 0x1d8b);
  B = OBJ_YH;
  CYC(0x1dab, 0x1dad);
  set_both_object_variables(gb, 0x1dad);
  CYC(0x1db0, 0x1db1); A = mem_rd(gb, DE);
  CYC(0x1db1, 0x1db2); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(0x1db2, 0x1db4);
  else { CYC(0x1db2, 0x1db6); alu_cpl(gb); A = alu_inc8(gb, A); }
  C = A;
  B = OBJ_COLLIDE_RADIUS_Y;
  CYC(0x1db6, 0x1db9);
  set_both_object_variables(gb, 0x1db9);
  CYC(0x1dbc, 0x1dbd); A = mem_rd(gb, DE);
  CYC(0x1dbd, 0x1dbe); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1dbe, 0x1dc0); H8(hFF8D) = A;
  alu_sub(gb, C);
  CYC(0x1dc0, 0x1dc3); H8(hFF8F) = A;
  B = OBJ_XH;
  CYC(0x1dc3, 0x1dc5);
  set_both_object_variables(gb, 0x1dc5);
  CYC(0x1dc8, 0x1dc9); A = mem_rd(gb, DE);
  CYC(0x1dc9, 0x1dca); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(0x1dca, 0x1dcc);
  else { CYC(0x1dca, 0x1dce); alu_cpl(gb); A = alu_inc8(gb, A); }
  C = A;
  B = OBJ_COLLIDE_RADIUS_Y + 1;
  CYC(0x1dce, 0x1dd1);
  set_both_object_variables(gb, 0x1dd1);
  CYC(0x1dd4, 0x1dd5); A = mem_rd(gb, DE);
  CYC(0x1dd5, 0x1dd6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1dd6, 0x1dd8); H8(hFF8C) = A;
  alu_sub(gb, C);
  B = A;
  CYC(0x1dd8, 0x1ddc); A = H8(hFF8F);
  alu_cp(gb, B);
  CYC(0x1ddc, 0x1dde);
}

static void prevent_h_from_passing_d(GB *gb) {
  A = L;
  alu_and(gb, 0xc0);
  CYC(0x1d7f, 0x1d84); H8(hFF8B) = A;
  CYC(0x1d84, 0x1d87);
  objects_collided(gb);
  if (!(F & FC)) { CYCT(0x1d87, 0x1d88); return; }
  CYC(0x1d87, 0x1d88);
  check_collision_direction(gb);
  if (!(F & FC)) {
    CYCT(0x1d8b, 0x1d8d);
    B = OBJ_XH;
    CYC(0x1d94, 0x1d98); A = H8(hFF8C);
    C = A;
    CYC(0x1d98, 0x1d9b);
  } else {
    CYC(0x1d8b, 0x1d8d);
    B = OBJ_YH;
    CYC(0x1d8d, 0x1d91); A = H8(hFF8D);
    C = A;
    CYC(0x1d91, 0x1d94);
  }
  set_both_object_variables(gb, 0x1d9b);
  CYC(0x1d9e, 0x1d9f); A = mem_rd(gb, DE);
  CYC(0x1d9f, 0x1da0); alu_sub(gb, mem_rd(gb, HL));
  A = C;
  if (F & FC) CYCT(0x1da0, 0x1da3);
  else { CYC(0x1da0, 0x1da5); alu_cpl(gb); A = alu_inc8(gb, A); }
  B = A;
  CYC(0x1da5, 0x1da7); A = mem_rd(gb, DE);
  alu_add(gb, B);
  CYC(0x1da7, 0x1da9); mem_wr(gb, HL, A);
  alu_scf(gb);
  CYC(0x1da9, 0x1dab);
}

void preventObjectHFromPassingObjectD_hook(GB *gb) {
  prevent_h_from_passing_d(gb);
  ret_effect(gb);
}

// script helpers

static void script_jump(GB *gb) {
  A = H;
  alu_cp(gb, 0x80);
  if (F & FC) CYCT(0x25ca, 0x25cf);
  else {
    CYC(0x25ca, 0x25d1); C = H8(hScriptAddressL);
    CYC(0x25d1, 0x25d4); B = H8(hScriptAddressH);
    CYC(0x25d4, 0x25d6); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_sub(gb, C);
    E = A;
    CYC(0x25d6, 0x25d9); A = mem_rd(gb, HL); SET_HL(HL - 1);
    alu_sbc(gb, B);
    alu_or(gb, A);
    if (!(F & FZ)) CYCT(0x25d9, 0x25dd);
    else { L = E; H = wBigBuffer >> 8; CYC(0x25d9, 0x25e1); return; }
  }
  CYC(0x25e1, 0x25e2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x25e2, 0x25e3); H = mem_rd(gb, HL);
  L = A;
  CYC(0x25e3, 0x25e6); A = H8(hActiveObject);
  D = A;
  alu_xor(gb, A);
  CYC(0x25e6, 0x25e9);
}

void scriptFunc_jump_hook(GB *gb) {
  script_jump(gb);
  ret_effect(gb);
}

void scriptFunc_jump_scf_hook(GB *gb) {
  CYC(0x25c5, 0x25c8);
  script_jump(gb);
  alu_scf(gb);
  CYC(0x25c8, 0x25ca);
  ret_effect(gb);
}

void scriptFunc_add3ToHl_hook(GB *gb) {
  SET_HL(HL + 3);
  CYC(0x25ea, 0x25ee);
  ret_effect(gb);
}

void scriptFunc_add3ToHl_scf_hook(GB *gb) {
  alu_scf(gb);
  CYC(0x25e9, 0x25ea);
  SET_HL(HL + 3);
  CYC(0x25ea, 0x25ee);
  ret_effect(gb);
}

// interaction animation. _interactionNextAnimationFrame stays transliterated and pops the
// return address itself, so these return whether that happened.

static bool interaction_animate(GB *gb) {
  H = D;
  L = 0x60;
  CYC(0x261b, 0x261f); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(0x261f, 0x2620); return false; }
  CYC(0x261f, 0x2622); A = H8(hRomBank);
  CYC(0x2622, 0x2623); push_effect(gb, AF);
  CYC(0x2623, 0x2627); H8(hRomBank) = 0x16;
  CYC(0x2627, 0x262a); mem_wr(gb, MBC_ROM_BANK, 0x16);
  L = 0x62;
  CYC(0x262a, 0x262e);
  _interactionNextAnimationFrame(gb);
  return true;
}

static void call_interaction_animate(GB *gb, uint16_t a) {
  CYC(a, a + 3);
  push_effect(gb, (uint16_t)(a + 3));
  if (!interaction_animate(gb)) pop_effect(gb);
}

void interactionAnimate_hook(GB *gb) {
  if (!interaction_animate(gb)) ret_effect(gb);
}

static void interaction_set_animation(GB *gb) {
  alu_add(gb, A);
  C = A;
  B = 0x00;
  CYC(0x262e, 0x2634); A = H8(hRomBank);
  CYC(0x2634, 0x2635); push_effect(gb, AF);
  CYC(0x2635, 0x2639); H8(hRomBank) = 0x16;
  CYC(0x2639, 0x263c); mem_wr(gb, MBC_ROM_BANK, 0x16);
  E = INTERACTION_BASE + 0x01;
  CYC(0x263c, 0x263f); A = mem_rd(gb, DE);
  SET_HL(interactionAnimationTable_bank16);
  CYC(0x263f, 0x2643);
  add_double_index_to_hl(gb);
  CYC(0x2643, 0x2644); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2644, 0x2645); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  CYC(0x2645, 0x2647);
  _interactionNextAnimationFrame(gb);
}

void interactionSetAnimation_hook(GB *gb) { interaction_set_animation(gb); }

static bool interaction_animate_2_times(GB *gb) {
  call_interaction_animate(gb, 0x2752);
  CYC(0x2755, 0x2758);
  return interaction_animate(gb);
}

static bool interaction_animate_3_times(GB *gb) {
  call_interaction_animate(gb, 0x274f);
  return interaction_animate_2_times(gb);
}

void interactionAnimate2Times_hook(GB *gb) { if (!interaction_animate_2_times(gb)) ret_effect(gb); }
void interactionAnimate3Times_hook(GB *gb) { if (!interaction_animate_3_times(gb)) ret_effect(gb); }
void interactionAnimate4Times_hook(GB *gb) {
  call_interaction_animate(gb, 0x274c);
  if (!interaction_animate_3_times(gb)) ret_effect(gb);
}

void interactionAnimateBasedOnSpeed_hook(GB *gb) {
  call_interaction_animate(gb, 0x2758);
  E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x275b, 0x275e); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x275e, 0x2760); ret_effect(gb); return; }
  CYC(0x275e, 0x2760);
  E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x2760, 0x2763); A = mem_rd(gb, DE);
  alu_cp(gb, 0x28);
  if (F & FC) { CYCT(0x2763, 0x2766); ret_effect(gb); return; }
  CYC(0x2763, 0x2766);
  alu_cp(gb, 0x50);
  if (F & FC) { CYCT(0x2766, 0x276b); if (!interaction_animate(gb)) ret_effect(gb); return; }
  CYC(0x2766, 0x276b);
  alu_cp(gb, 0x78);
  if (F & FC) { CYCT(0x276b, 0x2770); if (!interaction_animate_2_times(gb)) ret_effect(gb); return; }
  CYC(0x276b, 0x2770);
  CYC(0x2770, 0x2773);
  if (!interaction_animate_3_times(gb)) ret_effect(gb);
}

// npc behaviour

static void prevent_link_from_passing(GB *gb) {
  CYC(0x2680, 0x2683); A = W8(wLinkCanPassNpcs);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x2683, 0x2685); return; }
  CYC(0x2683, 0x2685);
  L = A;
  CYC(0x2685, 0x2689); A = W8(wLinkObjectIndex);
  H = A;
  CYC(0x2689, 0x268d);
  prevent_h_from_passing_d(gb);
  uint16_t af = AF;
  SET_HL(w1Companion_id);
  CYC(0x268d, 0x2692); A = mem_rd(gb, HL);
  alu_cp(gb, 0x0c);
  if (!(F & FZ)) CYCT(0x2692, 0x2696);
  else {
    L = OBJ_STATE;
    CYC(0x2692, 0x2699); A = mem_rd(gb, HL);
    alu_cp(gb, 0x02);
    if (!(F & FZ)) CYCT(0x2699, 0x269d);
    else {
      CYC(0x2699, 0x26a0);
      prevent_h_from_passing_d(gb);
      if (!(F & FC)) CYCT(0x26a0, 0x26a2);
      else {
        CYC(0x26a0, 0x26a2);
        A = 0x01;
        CYC(0x26a2, 0x26a7); W8(wDimitriHitNpc) = A;
      }
    }
  }
  SET_AF(af);
  CYC(0x26a7, 0x26a9);
}

void objectPreventLinkFromPassing_hook(GB *gb) {
  prevent_link_from_passing(gb);
  ret_effect(gb);
}

static void push_link_away_and_update_draw_priority(GB *gb) {
  CYC(0x26de, 0x26e1);
  prevent_link_from_passing(gb);
  CYC(0x26e1, 0x26e4);
  C = 0xc0;
  CYC(0x22e0, 0x22e2);
  set_priority_relative_to_link(gb);
}

void interactionPushLinkAwayAndUpdateDrawPriority_hook(GB *gb) {
  push_link_away_and_update_draw_priority(gb);
  ret_effect(gb);
}

static void interaction_animate_as_npc(GB *gb) {
  call_interaction_animate(gb, 0x26db);
  push_link_away_and_update_draw_priority(gb);
}

void interactionAnimateAsNpc_hook(GB *gb) {
  interaction_animate_as_npc(gb);
  ret_effect(gb);
}

void npcFaceLinkAndAnimate_hook(GB *gb) {
  E = 0x6c;
  A = 0x01;
  CYC(0x26a9, 0x26ae); mem_wr(gb, DE, A);
  E = 0x6b;
  CYC(0x26ae, 0x26b1); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(0x26b1, 0x26b4);
  else {
    CYC(0x26b1, 0x26b4);
    C = 0x28;
    CYC(0x26b4, 0x26b9);
    link_within_distance(gb);
    if (F & FC) CYCT(0x26b9, 0x26bb);
    else {
      CYC(0x26b9, 0x26bd);
      L = 0x6c;
      CYC(0x26bd, 0x26be); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
      A = 0x04;
      CYC(0x26be, 0x26c0);
    }
    B = A;
    alu_add(gb, A);
    alu_add(gb, A);
    H = D;
    L = 0x49;
    CYC(0x26c0, 0x26c7); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x26c7, 0x26c9); interaction_animate_as_npc(gb); ret_effect(gb); return; }
    CYC(0x26c7, 0x26ca); mem_wr(gb, HL, A);
    B = alu_srl(gb, B);
    E = 0x77;
    CYC(0x26ca, 0x26cf); A = mem_rd(gb, DE);
    alu_add(gb, B);
    CYC(0x26cf, 0x26d3);
    push_effect(gb, 0x26d3);
    interaction_set_animation(gb);
    E = 0x6b;
    A = 0x1e;
    CYC(0x26d3, 0x26d7);
  }
  A = alu_dec8(gb, A);
  CYC(0x26d7, 0x26d9); mem_wr(gb, DE, A);
  CYC(0x26d9, 0x26db);
  interaction_animate_as_npc(gb);
  ret_effect(gb);
}

void returnIfScrollMode01Unset_hook(GB *gb) {
  CYC(0x26e4, 0x26e7); A = W8(wScreenVariables);
  alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x26e7, 0x26ea); ret_effect(gb); return; }
  CYC(0x26e7, 0x26ea); SET_HL(pop_effect(gb));
  CYC(0x26ea, 0x26ec);
  ret_effect(gb);
}

void interactionDeleteAndRetIfEnabled02_hook(GB *gb) {
  E = INTERACTION_BASE;
  CYC(0x26ec, 0x26ef); A = mem_rd(gb, DE);
  alu_and(gb, 0x03);
  alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x26ef, 0x26f4); ret_effect(gb); return; }
  CYC(0x26ef, 0x26f4); SET_HL(pop_effect(gb));
  CYC(0x26f4, 0x26f8);
  interactionDelete(gb);
}

static void angle_to_direction(GB *gb) {
  alu_add(gb, 0x04);
  alu_add(gb, A);
  alu_swap_a(gb);
  alu_and(gb, 0x03);
  CYC(0x26f9, 0x2701);
}

void convertAngleToDirection_hook(GB *gb) {
  angle_to_direction(gb);
  ret_effect(gb);
}

static void angle_de_to_direction(GB *gb) {
  CYC(0x26f8, 0x26f9); A = mem_rd(gb, DE);
  angle_to_direction(gb);
}

void convertAngleDeToDirection_hook(GB *gb) {
  angle_de_to_direction(gb);
  ret_effect(gb);
}

void interactionSetAlwaysUpdateBit_hook(GB *gb) {
  H = D;
  L = INTERACTION_BASE;
  CYC(0x2701, 0x2706); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x2706, 0x2707);
  ret_effect(gb);
}

void interactionUnsetAlwaysUpdateBit_hook(GB *gb) {
  H = D;
  L = INTERACTION_BASE;
  CYC(0x277b, 0x2780); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x2780, 0x2781);
  ret_effect(gb);
}

void objectCheckLinkPushingAgainstCenter_hook(GB *gb) {
  CYC(0x2707, 0x270a); A = mem_rd(gb, w1Link_id);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x270a, 0x270c); ret_effect(gb); return; }
  CYC(0x270a, 0x270f); A = W8(wLinkAngle);
  alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x270f, 0x2712); ret_effect(gb); return; }
  CYC(0x270f, 0x2715); A = W8(wGameKeysPressed);
  alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x2715, 0x2718); ret_effect(gb); return; }
  CYC(0x2715, 0x2718);
  B = 0x04;
  CYC(0x2718, 0x271d);
  centered_with_link(gb);
  ret_effect(gb);
}

static void adjacent_tile_is_solid(GB *gb) {
  SET_HL(adjacentTileDirOffsets);
  CYC(0x272a, 0x272e);
  add_a_to_hl(gb);
  CYC(0x272e, 0x2731);
  object_get_short_position(gb);
  CYC(0x2731, 0x2732); alu_add(gb, mem_rd(gb, HL));
  H = wRoomCollisions >> 8;
  L = A;
  CYC(0x2732, 0x2736); A = mem_rd(gb, HL);
  alu_or(gb, A);
  CYC(0x2736, 0x2738);
}

void interactionCheckAdjacentTileIsSolid_hook(GB *gb) {
  E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x271d, 0x2720); A = mem_rd(gb, DE);
  CYC(0x2720, 0x2723);
  angle_de_to_direction(gb);
  CYC(0x2723, 0x2725);
  adjacent_tile_is_solid(gb);
  ret_effect(gb);
}

void interactionCheckAdjacentTileIsSolid_viaDirection_hook(GB *gb) {
  E = INTERACTION_BASE + 0x08;
  CYC(0x2725, 0x2728); A = mem_rd(gb, DE);
  A = alu_sra(gb, A);
  CYC(0x2728, 0x272a);
  adjacent_tile_is_solid(gb);
  ret_effect(gb);
}

void interactionDecCounter1IfTextNotActive_hook(GB *gb) {
  CYC(0x273c, 0x273f); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x273f, 0x2741); ret_effect(gb); return; }
  CYC(0x273f, 0x2744);
  obj_dec_body(gb, 0x23cc, INTERACTION_BASE + OBJ_COUNTER1);
  ret_effect(gb);
}

void interactionDecCounter1IfPaletteNotFading_hook(GB *gb) {
  CYC(0x2744, 0x2747); A = W8(wPaletteThread_mode);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x2747, 0x2749); ret_effect(gb); return; }
  CYC(0x2747, 0x274c);
  obj_dec_body(gb, 0x23cc, INTERACTION_BASE + OBJ_COUNTER1);
  ret_effect(gb);
}

static void interaction_h_set_position(GB *gb) {
  L = INTERACTION_BASE + OBJ_YH;
  CYC(0x2774, 0x2777); mem_wr(gb, HL, B);
  L = INTERACTION_BASE + OBJ_XH;
  CYC(0x2777, 0x277a); mem_wr(gb, HL, C);
  CYC(0x277a, 0x277b);
}

void interactionHSetPosition_hook(GB *gb) {
  interaction_h_set_position(gb);
  ret_effect(gb);
}

void interactionSetPosition_hook(GB *gb) {
  H = D;
  CYC(0x2773, 0x2774);
  interaction_h_set_position(gb);
  ret_effect(gb);
}
