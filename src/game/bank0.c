#include "game/game.h"

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

void addAToDe_hook(GB *gb) {
  alu_add(gb, E);
  E = A;
  if (F & FC) { CYC(0x0068, 0x006d); D = alu_inc8(gb, D); }
  else CYCT(0x0068, 0x006b);
  ret_effect(gb);
}

void addAToBc_hook(GB *gb) {
  alu_add(gb, C);
  C = A;
  if (F & FC) { CYC(0x006d, 0x0072); B = alu_inc8(gb, B); }
  else CYCT(0x006d, 0x0070);
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

void subDecimalFromHlRef_hook(GB *gb) {
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

void compareHlToBc_hook(GB *gb) {
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

void unsetFlag_hook(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x0218, 0x021d);
  flag_hlpr(gb);
  alu_cpl(gb);
  CYC(0x021d, 0x021f); alu_and(gb, mem_rd(gb, HL));
  CYC(0x021f, 0x0220); mem_wr(gb, HL, A);
  SET_BC(bc); SET_HL(hl);
  CYC(0x0220, 0x0223);
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

void add16BitRefs_hook(GB *gb) {
  CYC(0x23a7, 0x23a8); A = mem_rd(gb, DE);
  CYC(0x23a8, 0x23a9); alu_add(gb, mem_rd(gb, HL));
  CYC(0x23a9, 0x23aa); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  SET_HL(HL + 1);
  CYC(0x23aa, 0x23ad); A = mem_rd(gb, DE);
  CYC(0x23ad, 0x23ae); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x23ae, 0x23af); mem_wr(gb, DE, A);
  CYC(0x23af, 0x23b0);
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

static void obj_dec(GB *gb, uint16_t addr, uint8_t off) {
  CYC(addr, addr + 3); SET_HL((uint16_t)((D << 8) | off));
  CYC(addr + 3, addr + 4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(addr + 4, addr + 5);
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
  ret_effect(gb);
}

void objectSetInvisible_hook(GB *gb) { obj_visible_bit(gb, 0x1e7b, false); }
void objectSetVisible_hook(GB *gb) { obj_visible_bit(gb, 0x1e84, true); }

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

void getThisRoomFlags_hook(GB *gb) {
  CYC(0x197d, 0x1980); uint8_t room = W8(wActiveRoom);
  a_room_flags(gb, room);
  ret_effect(gb);
}

void checkIsLinkedGame_hook(GB *gb) {
  CYC(0x1992, 0x1995); A = W8(wIsLinkedGame);
  alu_or(gb, A);
  CYC(0x1995, 0x1997);
  ret_effect(gb);
}

// table searches

void findByteAtHl_hook(GB *gb) {
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
  ret_effect(gb);
}

void lookupKey_hook(GB *gb) {
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

void objectGetShortPosition_hook(GB *gb) {
  CYC(0x2096, 0x2098); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x2098, 0x209b);
  short_pos_from_de(gb);
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

void objectGetPosition_hook(GB *gb) {
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

void objectCopyPosition_hook(GB *gb) {
  CYC(0x2242, 0x2244); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(0x2244, 0x2247);
  copy_pos_raw(gb);
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
