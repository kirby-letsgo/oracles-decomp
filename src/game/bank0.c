#include "game/game.h"
#include "game/gen.h"

void loadDungeonLayout_b01_hook(GB *gb);
void paletteFadeHandler_hook(GB *gb);
void checkLockBG7Color3ToBlack_hook(GB *gb);
void b2_fileSelectScreen_hook(GB *gb);
void runBank2Function_hook(GB *gb);
void runIntroCinematic_hook(GB *gb);
void intro_cinematic__afterCall2d27_hook(GB *gb);
void updateTextbox_hook(GB *gb);
void reloadObjectGfx_b3f_hook(GB *gb);
void refreshObjectGfx_body_hook(GB *gb);
void loadObjectGfxHeaderToSlot4_body_hook(GB *gb);
void checkEnemyAndPartCollisions_hook(GB *gb);
void specialObjectCode_companionCutscene_b06_hook(GB *gb);
void specialObjectUpdatePositionGivenVelocity_hook(GB *gb);

// Rewrites of code/bank0.s. Cycles are burned from the ROM's own instruction stream (CYC/CYCT),
// which keeps interrupt dispatch on instruction boundaries; every memory access follows the burn
// of the instruction that performs it, as the transliteration does.

// memory helpers

void fillMemory(GB *gb, uint16_t dst, uint8_t value, unsigned n) {
  BASE(fillMemory);
  for (unsigned i = 0; i < n; i++) {
    CYC(b_+0, b_+1); mem_wr(gb, dst + i, value);
    if (i + 1 < n) CYCT(b_+1, b_+4); else CYC(b_+1, b_+4);
  }
  CYC(b_+4, b_+5);
}

void clearMemory(GB *gb, uint16_t dst, unsigned n) {
  BASE(clearMemory);
  CYC(b_+0, b_+1);
  fillMemory(gb, dst, 0, n);
}

void fillMemoryBc(GB *gb, uint16_t dst, uint8_t value, unsigned n) {
  BASE(fillMemoryBc);
  CYC(b_+0, b_+1);
  for (unsigned i = 0; i < n; i++) {
    CYC(b_+1, b_+3); mem_wr(gb, dst + i, value);
    if (i + 1 < n) CYCT(b_+3, b_+8); else CYC(b_+3, b_+8);
  }
  CYC(b_+8, b_+9);
}

void clearMemoryBc(GB *gb, uint16_t dst, unsigned n) {
  BASE(clearMemoryBc);
  CYC(b_+0, b_+1);
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

void copyMemory(GB *gb, uint16_t dst, uint16_t src, unsigned n) { copy_loop(gb, SYM(copyMemory), dst, src, n); }
void copyMemoryBc(GB *gb, uint16_t dst, uint16_t src, unsigned n) { copy_bc_loop(gb, SYM(copyMemoryBc), dst, src, n); }

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
  copy_loop(gb, SYM(copyMemory), DE, HL, n);
  copy_done(gb, HL, n);
}

void copyMemoryReverse_hook(GB *gb) {
  unsigned n = B ? B : 256;
  copy_loop(gb, SYM(copyMemoryReverse), HL, DE, n);
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
  copy_bc_loop(gb, SYM(copyMemoryBc), DE, HL, n);
  copy_bc_done(gb, n);
}

void copyMemoryBcReverse_hook(GB *gb) {
  unsigned n = BC ? BC : 65536;
  copy_bc_loop(gb, SYM(copyMemoryBcReverse), HL, DE, n);
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
  BASE(getNumSetBits);
  uint8_t a = A, b = 0;
  CYC(b_+0, b_+2);
  do {
    bool carry = a & 0x80;
    a = (uint8_t)(a << 1);
    if (carry) { CYC(b_+2, b_+6); b++; }
    else CYCT(b_+2, b_+5);
    if (a) CYCT(b_+6, b_+9); else CYC(b_+6, b_+9);
  } while (a);
  A = B = b;
  F = FZ;
  CYC(b_+9, b_+11);
  ret_effect(gb);
}

void addDecimalToHlRef_hook(GB *gb) {
  BASE(addDecimalToHlRef);
  uint16_t p = HL;
  CYC(b_+0, b_+1); A = mem_rd(gb, p); alu_add(gb, C); alu_daa(gb);
  CYC(b_+1, b_+4); mem_wr(gb, p, A);
  CYC(b_+4, b_+5); A = mem_rd(gb, p + 1); alu_adc(gb, B); alu_daa(gb);
  CYC(b_+5, b_+8); mem_wr(gb, p + 1, A);
  if (F & FC) {
    CYC(b_+8, b_+12); A = 0x63; mem_wr(gb, p, A);
    CYC(b_+12, b_+13); mem_wr(gb, p + 1, A);
    CYC(b_+13, b_+14);
  } else CYCT(b_+8, b_+9);
  ret_effect(gb);
}

static void sub_decimal_from_hl_ref(GB *gb) {
  BASE(subDecimalFromHlRef);
  uint16_t p = HL;
  CYC(b_+0, b_+1); A = mem_rd(gb, p); alu_sub(gb, C); alu_daa(gb);
  CYC(b_+1, b_+4); mem_wr(gb, p, A);
  CYC(b_+4, b_+5); A = mem_rd(gb, p + 1); alu_sbc(gb, B); alu_daa(gb);
  CYC(b_+5, b_+8); mem_wr(gb, p + 1, A);
  if (F & FC) {
    alu_xor(gb, A);
    CYC(b_+8, b_+11); mem_wr(gb, p, 0);
    CYC(b_+11, b_+12); mem_wr(gb, p + 1, 0);
    alu_scf(gb);
    CYC(b_+12, b_+14);
  } else CYCT(b_+8, b_+9);
}

void subDecimalFromHlRef_hook(GB *gb) {
  sub_decimal_from_hl_ref(gb);
  ret_effect(gb);
}

static void multiply_a_by_c(GB *gb) {
  BASE(multiplyAByC);
  CYC(b_+0, b_+6);
  E = 8; B = 0; SET_HL(0);
  do {
    alu_add_hl(gb, HL);
    alu_add(gb, A);
    if (F & FC) { CYC(b_+6, b_+11); alu_add_hl(gb, BC); }
    else CYCT(b_+6, b_+10);
    E = alu_dec8(gb, E);
    if (E) CYCT(b_+11, b_+14); else CYC(b_+11, b_+14);
  } while (E);
  CYC(b_+14, b_+15);
}

void multiplyAByC_hook(GB *gb) {
  multiply_a_by_c(gb);
  ret_effect(gb);
}

static void multiply_a_by_16(GB *gb) {
  BASE(multiplyABy16);
  uint8_t a = A;
  C = (uint8_t)(a << 4);
  A = B = a >> 4;
  F = (A == 0 ? FZ : 0) | FH;
  CYC(b_+0, b_+11);
}

void multiplyABy16_hook(GB *gb) {
  multiply_a_by_16(gb);
  ret_effect(gb);
}

static void multiply_a_by_8(GB *gb) {
  BASE(multiplyABy8);
  uint8_t a = A;
  C = (uint8_t)(a << 3);
  A = B = a >> 5;
  F = (A == 0 ? FZ : 0) | FH;
  CYC(b_+0, b_+12);
}

void multiplyABy8_hook(GB *gb) {
  multiply_a_by_8(gb);
  ret_effect(gb);
}

void multiplyABy4_hook(GB *gb) {
  BASE(multiplyABy4);
  uint8_t a = A;
  B = a >> 6;
  A = C = (uint8_t)(a << 2);
  F = B == 0 ? FZ : 0;
  CYC(b_+0, b_+10);
  ret_effect(gb);
}

void s8ToS16_hook(GB *gb) {
  BASE(s8ToS16);
  B = 0xff;
  alu_bit(gb, 7, A);
  if (A & 0x80) CYCT(b_+0, b_+6);
  else { CYC(b_+0, b_+7); B = alu_inc8(gb, B); }
  C = A;
  CYC(b_+7, b_+9);
  ret_effect(gb);
}

static void compare_hl_to_bc(GB *gb) {
  BASE(compareHlToBc);
  A = H; alu_cp(gb, B);
  if (F & FC) { CYCT(b_+0, b_+4); A = 0xff; CYC(b_+14, b_+17); }
  else if (!(F & FZ)) { CYC(b_+0, b_+4); CYCT(b_+4, b_+6); A = 0x01; CYC(b_+17, b_+20); }
  else {
    CYC(b_+0, b_+8);
    A = L; alu_cp(gb, C);
    if (F & FC) { CYCT(b_+8, b_+10); A = 0xff; CYC(b_+14, b_+17); }
    else if (!(F & FZ)) { CYC(b_+8, b_+10); CYCT(b_+10, b_+12); A = 0x01; CYC(b_+17, b_+20); }
    else { CYC(b_+8, b_+14); alu_xor(gb, A); }
  }
}

void compareHlToBc_hook(GB *gb) {
  compare_hl_to_bc(gb);
  ret_effect(gb);
}

void getHighestSetBit_hook(GB *gb) {
  BASE(getHighestSetBit);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+0, b_+2); ret_effect(gb); return; }
  CYC(b_+0, b_+5);
  uint8_t c = 0xff;
  do {
    c++;
    A = alu_srl(gb, A);
    if (A) CYCT(b_+5, b_+10); else CYC(b_+5, b_+10);
  } while (A);
  A = c;
  alu_scf(gb);
  CYC(b_+10, b_+14);
  ret_effect(gb);
}

void getLowestSetBit_hook(GB *gb) {
  BASE(getLowestSetBit);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+0, b_+2); ret_effect(gb); return; }
  CYC(b_+0, b_+5);
  uint8_t c = 8;
  do {
    c--;
    alu_add(gb, A);
    if (A) CYCT(b_+5, b_+9); else CYC(b_+5, b_+9);
  } while (A);
  A = c;
  alu_scf(gb);
  CYC(b_+9, b_+13);
  ret_effect(gb);
}

// flags: one bit per flag, base address in hl, flag index in a

static void flag_hlpr(GB *gb) {
  BASE(_flagHlpr);
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
  CYC(b_+0, b_+20);
}

void _flagHlpr_hook(GB *gb) {
  flag_hlpr(gb);
  ret_effect(gb);
}

static void check_flag(GB *gb) {
  BASE(checkFlag);
  uint16_t hl = HL, bc = BC;
  CYC(b_+0, b_+5);
  flag_hlpr(gb);
  CYC(b_+5, b_+6); alu_and(gb, mem_rd(gb, HL));
  SET_BC(bc); SET_HL(hl);
  CYC(b_+6, b_+9);
}

void checkFlag_hook(GB *gb) {
  check_flag(gb);
  ret_effect(gb);
}

static void set_flag(GB *gb) {
  BASE(setFlag);
  uint16_t hl = HL, bc = BC;
  CYC(b_+0, b_+5);
  flag_hlpr(gb);
  CYC(b_+5, b_+6); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  SET_BC(bc); SET_HL(hl);
  CYC(b_+7, b_+10);
}

void setFlag_hook(GB *gb) {
  set_flag(gb);
  ret_effect(gb);
}

static void unset_flag(GB *gb) {
  BASE(unsetFlag);
  uint16_t hl = HL, bc = BC;
  CYC(b_+0, b_+5);
  flag_hlpr(gb);
  alu_cpl(gb);
  CYC(b_+5, b_+7); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  SET_BC(bc); SET_HL(hl);
  CYC(b_+8, b_+11);
}

void unsetFlag_hook(GB *gb) {
  unset_flag(gb);
  ret_effect(gb);
}

// saturating counters

static void dec16_cap(GB *gb, uint16_t p) {
  BASE(decHlRef16WithCap);
  CYC(b_+0, b_+2); A = mem_rd(gb, p + 1);
  CYC(b_+2, b_+3); alu_or(gb, mem_rd(gb, p));
  if (F & FZ) { CYCT(b_+3, b_+4); return; }
  CYC(b_+3, b_+5); A = mem_rd(gb, p); alu_sub(gb, 1);
  CYC(b_+5, b_+8); mem_wr(gb, p, A);
  CYC(b_+8, b_+9); A = mem_rd(gb, p + 1); alu_sbc(gb, 0);
  CYC(b_+9, b_+12); mem_wr(gb, p + 1, A);
  CYC(b_+12, b_+13); alu_or(gb, mem_rd(gb, p));
  CYC(b_+13, b_+14);
}

void decHlRef16WithCap_hook(GB *gb) {
  dec16_cap(gb, HL);
  ret_effect(gb);
}

static void dec16_ff8c(GB *gb) {
  BASE(dec16_ff8c);
  uint16_t hl = HL;
  CYC(b_+0, b_+7);
  dec16_cap(gb, hFF8C);
  SET_HL(hl);
  CYC(b_+7, b_+9);
}

void dec16_ff8c_hook(GB *gb) {
  dec16_ff8c(gb);
  ret_effect(gb);
}

static void inc_hl_ref_with_cap(GB *gb) {
  BASE(incHlRefWithCap);
  CYC(b_+0, b_+1); uint8_t v = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) CYCT(b_+1, b_+2);
  else { CYC(b_+1, b_+4); mem_wr(gb, HL, 0xff); CYC(b_+4, b_+5); }
}

void incHlRefWithCap_hook(GB *gb) {
  inc_hl_ref_with_cap(gb);
  ret_effect(gb);
}

static void inc_hl_ref16_with_cap(GB *gb) {
  BASE(incHlRef16WithCap);
  CYC(b_+0, b_+1); uint8_t lo = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, lo);
  if (lo) { CYCT(b_+1, b_+2); return; }
  CYC(b_+1, b_+4); uint8_t hi = alu_inc8(gb, mem_rd(gb, HL + 1)); mem_wr(gb, HL + 1, hi);
  if (hi) { CYC(b_+4, b_+8); return; }
  CYCT(b_+4, b_+6);
  CYC(b_+8, b_+12); mem_wr(gb, HL + 1, 0xff);
  CYC(b_+12, b_+13); mem_wr(gb, HL, 0xff);
  CYC(b_+13, b_+15);
}

void incHlRef16WithCap_hook(GB *gb) {
  inc_hl_ref16_with_cap(gb);
  ret_effect(gb);
}

void hexToDec_hook(GB *gb) {
  BASE(hexToDec);
  CYC(b_+0, b_+3);
  SET_BC(0);
  for (;;) {
    alu_cp(gb, 0x64);
    if (F & FC) { CYCT(b_+3, b_+7); break; }
    CYC(b_+3, b_+12);
    alu_sub(gb, 0x64);
    B = alu_inc8(gb, B);
  }
  for (;;) {
    alu_cp(gb, 0x0a);
    if (F & FC) { CYCT(b_+12, b_+15); break; }
    CYC(b_+12, b_+20);
    alu_sub(gb, 0x0a);
    C = alu_inc8(gb, C);
  }
  ret_effect(gb);
}

// random numbers: hRng1:hRng2 is a 16-bit state, advanced by x3 plus the low byte. The VBlank
// handler advances it too, so each access sits at its exact cycle.

uint8_t getRandomNumber(GB *gb) {
  BASE(getRandomNumber);
  CYC(b_+0, b_+4); uint8_t lo = H8(hRng1);
  CYC(b_+4, b_+8); uint8_t hi = H8(hRng2);
  uint16_t v = (uint16_t)(((hi << 8) | lo) * 3);
  CYC(b_+8, b_+15); H8(hRng2) = v >> 8;
  CYC(b_+15, b_+18); H8(hRng1) = (uint8_t)((v >> 8) + lo);
  CYC(b_+18, b_+21);
  return H8(hRng1);
}

uint8_t getRandomNumber_noPreserveVars(GB *gb) {
  BASE(getRandomNumber_noPreserveVars);
  CYC(b_+0, b_+2); uint8_t lo = H8(hRng1);
  CYC(b_+2, b_+6); uint8_t hi = H8(hRng2);
  uint16_t v = (uint16_t)(((hi << 8) | lo) * 3);
  CYC(b_+6, b_+13); H8(hRng2) = v >> 8;
  CYC(b_+13, b_+16); H8(hRng1) = (uint8_t)((v >> 8) + lo);
  CYC(b_+16, b_+17);
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
  BASE(getRandomIndexFromProbabilityDistribution);
  B = 0;
  CYC(b_+0, b_+5);
  A = getRandomNumber(gb);
  for (;;) {
    CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
    if (F & FC) { CYCT(b_+6, b_+7); break; }
    CYC(b_+6, b_+11);
    SET_HL(HL + 1);
    B++;
  }
  ret_effect(gb);
}

// LCD and VRAM setup

void clearOam(GB *gb) {
  BASE(clearOam);
  CYC(b_+0, b_+3); H8(hOamTail) = 0;
  CYC(b_+3, b_+7);
  for (int i = 0; i < 40; i++) {
    CYC(b_+7, b_+9); mem_wr(gb, wOam + i * 4, 0xe0);
    if (i < 39) CYCT(b_+9, b_+15); else CYC(b_+9, b_+15);
  }
  CYC(b_+15, b_+16);
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
  BASE(disableLcd);
  CYC(b_+0, b_+2); uint8_t lcdc = mem_rd(gb, IO_LCDC);
  if (!(lcdc & 0x80)) { CYCT(b_+2, b_+4); return false; }
  CYC(b_+2, b_+7); uint8_t ie = mem_rd(gb, IO_IE);
  CYC(b_+7, b_+12); mem_wr(gb, IO_IE, ie & 0xfe);
  for (;;) {
    CYC(b_+12, b_+14); uint8_t ly = mem_rd(gb, IO_LY);
    if (ly >= 0x91) { CYC(b_+14, b_+18); break; }
    CYCT(b_+14, b_+18);
  }
  CYC(b_+18, b_+22); H8(hNextLcdInterruptBehaviour) = 3;
  CYC(b_+22, b_+26); W8(wGfxRegsFinal) = 0;
  CYC(b_+26, b_+29); W8(wGfxRegs2) = 0;
  CYC(b_+29, b_+32); W8(wGfxRegs1) = 0;
  CYC(b_+32, b_+34); mem_wr(gb, IO_LCDC, 0);
  CYC(b_+34, b_+36); mem_wr(gb, IO_IF, 0);
  CYC(b_+36, b_+39); mem_wr(gb, IO_IE, ie);
  CYC(b_+39, b_+41);
  return true;
}

void disableLcd_hook(GB *gb) {
  if (disableLcd(gb)) { A = mem_rd(gb, IO_IE); F = FZ; }
  else { A = mem_rd(gb, IO_LCDC); alu_rlca(gb); }
  ret_effect(gb);
}

void clearVram(GB *gb) {
  BASE(clearVram);
  CYC(b_+0, b_+3); disableLcd(gb);
  CYC(b_+3, b_+6); clearOam(gb);
  CYC(b_+6, b_+10); mem_wr(gb, IO_VBK, 1);
  CYC(b_+10, b_+19); clearMemoryBc(gb, 0x8000, 0x2000);
  CYC(b_+19, b_+22); mem_wr(gb, IO_VBK, 0);
  CYC(b_+22, b_+30); clearMemoryBc(gb, 0x8000, 0x2000);
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
  BASE(initializeVramMap0);
  CYC(b_+0, b_+3); disableLcd(gb);
  CYC(b_+3, b_+7); mem_wr(gb, IO_VBK, 1);
  CYC(b_+7, b_+18); fillMemoryBc(gb, 0x9800, 0x80, 0x400);
  CYC(b_+18, b_+21); mem_wr(gb, IO_VBK, 0);
  CYC(b_+21, b_+29); clearMemoryBc(gb, 0x9800, 0x400);
}

void initializeVramMap1(GB *gb) {
  BASE(initializeVramMap1);
  CYC(b_+0, b_+3); disableLcd(gb);
  CYC(b_+3, b_+7); mem_wr(gb, IO_VBK, 1);
  CYC(b_+7, b_+18); fillMemoryBc(gb, 0x9c00, 0x80, 0x400);
  CYC(b_+18, b_+21); mem_wr(gb, IO_VBK, 0);
  CYC(b_+21, b_+30); clearMemoryBc(gb, 0x9c00, 0x400);
}

void initializeVramMaps(GB *gb) {
  BASE(initializeVramMaps);
  CYC(b_+0, b_+3);
  initializeVramMap1(gb);
  initializeVramMap0(gb);
}

void initializeVramMap0_hook(GB *gb) { initializeVramMap0(gb); clear_bc_done(gb, 0x9c00); }
void initializeVramMap1_hook(GB *gb) { initializeVramMap1(gb); clear_bc_done(gb, 0xa000); }
void initializeVramMaps_hook(GB *gb) { initializeVramMaps(gb); clear_bc_done(gb, 0x9c00); }

// sequential ROM reads across bank boundaries

static void read_byte_sequential(GB *gb) {
  BASE(readByteSequential);
  CYC(b_+0, b_+1); uint8_t v = mem_rd(gb, HL);
  SET_HL(HL + 1);
  alu_bit(gb, 7, H);
  if (!(H & 0x80)) { CYCT(b_+1, b_+4); A = v; return; }
  CYC(b_+1, b_+7); H = 0x40;
  CYC(b_+7, b_+9); uint8_t bank = (uint8_t)(H8(hRomBank) + 1);
  CYC(b_+9, b_+12); H8(hRomBank) = bank;
  CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, bank);
  CYC(b_+15, b_+17);
  A = v;
}

void readByteSequential_hook(GB *gb) {
  read_byte_sequential(gb);
  ret_effect(gb);
}

// misc

static void add16_bit_refs(GB *gb) {
  BASE(add16BitRefs);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  SET_HL(HL + 1);
  CYC(b_+3, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9);
}

void add16BitRefs_hook(GB *gb) {
  add16_bit_refs(gb);
  ret_effect(gb);
}

static void cp_active_ring(GB *gb) {
  BASE(cpActiveRing);
  CYC(b_+0, b_+5); alu_cp(gb, W8(wActiveRing));
  CYC(b_+5, b_+7);
}

void cpActiveRing_hook(GB *gb) {
  cp_active_ring(gb);
  ret_effect(gb);
}

void disableActiveRing_hook(GB *gb) {
  BASE(disableActiveRing);
  CYC(b_+0, b_+6); W8(wActiveRing) |= 0x40;
  CYC(b_+6, b_+8);
  ret_effect(gb);
}

void enableActiveRing_hook(GB *gb) {
  BASE(enableActiveRing);
  CYC(b_+0, b_+5); A = W8(wActiveRing);
  alu_cp(gb, 0xff);
  if (F & FZ) CYCT(b_+5, b_+9);
  else { CYC(b_+5, b_+11); W8(wActiveRing) &= (uint8_t)~0x40; }
  CYC(b_+11, b_+13);
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

void interactionDecCounter1_hook(GB *gb) { obj_dec(gb, SYM(interactionDecCounter1), INTERACTION_BASE + OBJ_COUNTER1); }
void interactionDecCounter2_hook(GB *gb) { obj_dec(gb, SYM(interactionDecCounter2), INTERACTION_BASE + OBJ_COUNTER2); }
void itemDecCounter1_hook(GB *gb) { obj_dec(gb, SYM(itemDecCounter1), OBJ_COUNTER1); }
void itemDecCounter2_hook(GB *gb) { obj_dec(gb, SYM(itemDecCounter2), OBJ_COUNTER2); }
void interactionIncState_hook(GB *gb) { obj_inc(gb, SYM(interactionIncState), INTERACTION_BASE + OBJ_STATE); }
void interactionIncSubstate_hook(GB *gb) { obj_inc(gb, SYM(interactionIncSubstate), INTERACTION_BASE + OBJ_SUBSTATE); }
void itemIncState_hook(GB *gb) { obj_inc(gb, SYM(itemIncState), OBJ_STATE); }
void itemIncSubstate_hook(GB *gb) { obj_inc(gb, SYM(itemIncSubstate), OBJ_SUBSTATE); }

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

void objectSetVisiblec0_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisiblec0), 0xc0); }
void objectSetVisiblec1_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisiblec1), 0xc1); }
void objectSetVisiblec2_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisiblec2), 0xc2); }
void objectSetVisiblec3_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisiblec3), 0xc3); }
void objectSetVisible80_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisible80), 0x80); }
void objectSetVisible81_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisible81), 0x81); }
void objectSetVisible82_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisible82), 0x82); }
void objectSetVisible83_hook(GB *gb) { obj_set_visible(gb, SYM(objectSetVisible83), 0x83); }

static void obj_visible_bit(GB *gb, uint16_t addr, bool set) {
  CYC(addr, addr + 2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_VISIBLE);
  L = A;
  H = D;
  CYC(addr + 2, addr + 8); uint8_t v = mem_rd(gb, HL);
  mem_wr(gb, HL, set ? (uint8_t)(v | 0x80) : (uint8_t)(v & 0x7f));
  CYC(addr + 8, addr + 9);
}

void objectSetInvisible_hook(GB *gb) { obj_visible_bit(gb, SYM(objectSetInvisible), false); ret_effect(gb); }
void objectSetVisible_hook(GB *gb) { obj_visible_bit(gb, SYM(objectSetVisible), true); ret_effect(gb); }

void objectSetReservedBit1_hook(GB *gb) {
  BASE(objectSetReservedBit1);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  L = A;
  H = D;
  CYC(b_+2, b_+6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  CYC(b_+6, b_+7);
  ret_effect(gb);
}

void checkInteractionState_hook(GB *gb) {
  BASE(checkInteractionState);
  E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  alu_or(gb, A);
  CYC(b_+3, b_+5);
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
  BASE(getRoomFlags);
  CYC(b_+0, b_+4); SET_HL(flagLocationGroupTable);
  A = group;
  add_a_to_hl(gb);
  CYC(b_+4, b_+5); H = mem_rd(gb, HL);
  L = room;
  CYC(b_+5, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+8);
}

void getRoomFlags_hook(GB *gb) {
  room_flags(gb, A, B);
  ret_effect(gb);
}

static void a_room_flags(GB *gb, uint8_t room) {
  BASE(getARoomFlags);
  uint16_t bc = BC;
  CYC(b_+0, b_+5); uint8_t group = W8(wActiveGroup);
  CYC(b_+5, b_+8);
  room_flags(gb, group, room);
  SET_BC(bc);
  CYC(b_+8, b_+10);
}

void getARoomFlags_hook(GB *gb) {
  a_room_flags(gb, A);
  ret_effect(gb);
}

static void this_room_flags(GB *gb) {
  BASE(getThisRoomFlags);
  CYC(b_+0, b_+3); uint8_t room = W8(wActiveRoom);
  a_room_flags(gb, room);
}

void getThisRoomFlags_hook(GB *gb) {
  this_room_flags(gb);
  ret_effect(gb);
}

void checkIsLinkedGame_hook(GB *gb) {
  BASE(checkIsLinkedGame);
  CYC(b_+0, b_+3); A = W8(wIsLinkedGame);
  alu_or(gb, A);
  CYC(b_+3, b_+5);
  ret_effect(gb);
}

// table searches

static void find_byte_at_hl(GB *gb) {
  BASE(findByteAtHl);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_or(gb, A);
    if (F & FZ) { CYCT(b_+1, b_+3); break; }
    alu_cp(gb, E);
    if (!(F & FZ)) { CYCT(b_+1, b_+6); continue; }
    alu_scf(gb);
    CYC(b_+1, b_+8);
    break;
  }
}

void findByteAtHl_hook(GB *gb) {
  find_byte_at_hl(gb);
  ret_effect(gb);
}

static void lookup_key(GB *gb) {
  BASE(lookupKey);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_or(gb, A);
    if (F & FZ) { CYCT(b_+1, b_+3); break; }
    alu_cp(gb, E);
    bool found = F & FZ;
    CYC(b_+1, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    if (!found) { CYCT(b_+5, b_+7); continue; }
    alu_scf(gb);
    CYC(b_+5, b_+9);
    break;
  }
}

void lookupKey_hook(GB *gb) {
  lookup_key(gb);
  ret_effect(gb);
}

// consumable counters (BCD)

void setStatusBarNeedsRefreshBit1(GB *gb) {
  BASE(setStatusBarNeedsRefreshBit1);
  CYC(b_+0, b_+6); W8(wStatusBarNeedsRefresh) |= 0x02;
  CYC(b_+6, b_+8);
}

void setStatusBarNeedsRefreshBit1_hook(GB *gb) {
  setStatusBarNeedsRefreshBit1(gb);
  ret_effect(gb);
}

static void dec_bcd_count(GB *gb) {
  BASE(decNumBombs);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); return; }
  CYC(b_+4, b_+9);
  setStatusBarNeedsRefreshBit1(gb);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  alu_sub(gb, 1);
  alu_daa(gb);
  CYC(b_+10, b_+14); mem_wr(gb, HL, A);
  alu_or(gb, H);
  CYC(b_+14, b_+16);
}

void decNumActiveSeeds_hook(GB *gb) {
  BASE(decNumActiveSeeds);
  alu_and(gb, 0x07);
  SET_HL(wNumEmberSeeds);
  CYC(b_+0, b_+6);
  add_a_to_hl(gb);
  CYC(b_+6, b_+8);
  dec_bcd_count(gb);
  ret_effect(gb);
}

void decNumBombchus_hook(GB *gb) {
  BASE(decNumBombchus);
  SET_HL(wNumBombchus);
  CYC(b_+0, b_+5);
  dec_bcd_count(gb);
  ret_effect(gb);
}

void decNumBombs_hook(GB *gb) {
  BASE(decNumBombs);
  SET_HL(wNumBombs);
  CYC(b_+0, b_+3);
  dec_bcd_count(gb);
  ret_effect(gb);
}

// object positions (Object.y at +0x0a, yh +0x0b, xh +0x0d, zh +0x0f)

static void short_pos_from_de(GB *gb) {
  BASE(getShortPositionFromDE);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  alu_and(gb, 0xf0);
  B = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(b_+1, b_+7); A = mem_rd(gb, DE);
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, B);
  CYC(b_+7, b_+13);
}

void getShortPositionFromDE_hook(GB *gb) {
  short_pos_from_de(gb);
  ret_effect(gb);
}

static void object_get_short_position(GB *gb) {
  BASE(objectGetShortPosition);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+2, b_+5);
  short_pos_from_de(gb);
}

void objectGetShortPosition_hook(GB *gb) {
  object_get_short_position(gb);
  ret_effect(gb);
}

void objectGetShortPosition_withYOffset_hook(GB *gb) {
  BASE(objectGetShortPosition_withYOffset);
  B = A;
  CYC(b_+0, b_+3); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+3, b_+7); A = mem_rd(gb, DE);
  alu_add(gb, B);
  CYC(b_+7, b_+10);
  alu_and(gb, 0xf0);
  B = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC((SYM(getShortPositionFromDE) + 1), (SYM(getShortPositionFromDE) + 7)); A = mem_rd(gb, DE);
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, B);
  CYC((SYM(getShortPositionFromDE) + 7), (SYM(getShortPositionFromDE) + 13));
  ret_effect(gb);
}

static void object_get_position(GB *gb) {
  BASE(objectGetPosition);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+2, b_+6); A = mem_rd(gb, DE);
  B = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(b_+6, b_+10); A = mem_rd(gb, DE);
  C = A;
  CYC(b_+10, b_+12);
}

void objectGetPosition_hook(GB *gb) {
  object_get_position(gb);
  ret_effect(gb);
}

static void short_to_long_pos(GB *gb) {
  BASE(convertShortToLongPosition_paramC);
  A = C;
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  B = A;
  A = C;
  alu_swap_a(gb);
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  C = A;
  CYC(b_+0, b_+15);
}

void convertShortToLongPosition_paramC_hook(GB *gb) {
  short_to_long_pos(gb);
  ret_effect(gb);
}

void convertShortToLongPosition_hook(GB *gb) {
  BASE(convertShortToLongPosition);
  C = A;
  CYC(b_+0, b_+1);
  short_to_long_pos(gb);
  ret_effect(gb);
}

static void set_short_pos_param_c(GB *gb) {
  BASE(setShortPosition_paramC);
  uint16_t bc = BC;
  CYC(b_+0, b_+4);
  short_to_long_pos(gb);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(b_+5, b_+8); mem_wr(gb, HL, C);
  SET_BC(bc);
  CYC(b_+8, b_+10);
}

void setShortPosition_paramC_hook(GB *gb) {
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

void setShortPosition_hook(GB *gb) {
  BASE(setShortPosition);
  C = A;
  CYC(b_+0, b_+1);
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

void objectSetShortPosition_hook(GB *gb) {
  BASE(objectSetShortPosition);
  H = D;
  CYC(b_+0, b_+3); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+3, b_+8);
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

static void center_on_tile(GB *gb) {
  BASE(centerCoordinatesOnTile);
  alu_xor(gb, A);
  CYC(b_+0, b_+2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  CYC(b_+3, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(b_+8, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  alu_and(gb, 0xf0);
  alu_or(gb, 0x08);
  CYC(b_+11, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17);
}

void centerCoordinatesOnTile_hook(GB *gb) {
  center_on_tile(gb);
  ret_effect(gb);
}

void objectCenterOnTile_hook(GB *gb) {
  BASE(objectCenterOnTile);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_Y);
  L = A;
  H = D;
  CYC(b_+2, b_+6);
  center_on_tile(gb);
  ret_effect(gb);
}

static void copy_pos_raw(GB *gb) {
  BASE(objectCopyPosition_rawAddress);
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+0, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+8, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+13, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19);
}

void objectCopyPosition_rawAddress_hook(GB *gb) {
  copy_pos_raw(gb);
  ret_effect(gb);
}

static void object_copy_position(GB *gb) {
  BASE(objectCopyPosition);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+2, b_+5);
  copy_pos_raw(gb);
}

void objectCopyPosition_hook(GB *gb) {
  object_copy_position(gb);
  ret_effect(gb);
}

static void copy_position_with_offset(GB *gb) {
  BASE(objectCopyPositionWithOffset);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+2, b_+12); A = mem_rd(gb, DE);
  alu_add(gb, B);
  CYC(b_+12, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+14, b_+18); A = mem_rd(gb, DE);
  alu_add(gb, C);
  CYC(b_+18, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+20, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26);
}

void objectCopyPositionWithOffset_hook(GB *gb) {
  copy_position_with_offset(gb);
  ret_effect(gb);
}

static void take_pos(GB *gb) {
  BASE(objectTakePositionWithOffset);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+2, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, B);
  CYC(b_+12, b_+14); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+14, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, C);
  CYC(b_+18, b_+20); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+20, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26);
}

void objectTakePositionWithOffset_hook(GB *gb) {
  take_pos(gb);
  ret_effect(gb);
}

void objectTakePosition_hook(GB *gb) {
  BASE(objectTakePosition);
  SET_BC(0);
  CYC(b_+0, b_+3);
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
  collision_table(gb, SYM(lookupCollisionTable_paramE));
  lookup_key(gb);
  ret_effect(gb);
}

static void lookup_collision_table(GB *gb) {
  BASE(lookupCollisionTable);
  E = A;
  CYC(b_+0, b_+1);
  collision_table(gb, SYM(lookupCollisionTable_paramE));
  lookup_key(gb);
}

void lookupCollisionTable_hook(GB *gb) {
  lookup_collision_table(gb);
  ret_effect(gb);
}

void findByteInCollisionTable_paramE_hook(GB *gb) {
  collision_table(gb, SYM(findByteInCollisionTable_paramE));
  find_byte_at_hl(gb);
  ret_effect(gb);
}

void findByteInCollisionTable_hook(GB *gb) {
  BASE(findByteInCollisionTable);
  E = A;
  CYC(b_+0, b_+1);
  collision_table(gb, SYM(findByteInCollisionTable_paramE));
  find_byte_at_hl(gb);
  ret_effect(gb);
}

// object slot availability: walk slots from hl (one per 0x40 bytes, via inc h) until b free ones

static void slots_available(GB *gb) {
  BASE(checkBEnemySlotsAvailable);
  for (;;) {
    CYC(b_+3, b_+6);
    CYC(b_+13, b_+14); A = mem_rd(gb, HL);
    H++;
    alu_or(gb, A);
    if (F & FZ) {
      CYC(b_+14, b_+16); CYCT(b_+16, b_+17);
      CYC(b_+6, b_+10);
      B = alu_dec8(gb, B);
      if (B) { CYCT(b_+10, b_+12); continue; }
      CYC(b_+10, b_+13);
      return;
    }
    CYC(b_+14, b_+17);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+17, b_+21); CYCT(b_+6, b_+8); continue; }
    CYC(b_+17, b_+21);
    alu_or(gb, H);
    CYC(b_+21, b_+23);
    CYC(b_+6, b_+8); CYCT(b_+8, b_+9);
    return;
  }
}

void checkBPartSlotsAvailable_hook(GB *gb) {
  BASE(checkBPartSlotsAvailable);
  SET_HL(PART_SLOTS);
  CYC(b_+0, b_+5);
  slots_available(gb);
  ret_effect(gb);
}

void checkBEnemySlotsAvailable_hook(GB *gb) {
  BASE(checkBEnemySlotsAvailable);
  SET_HL(ENEMY_SLOTS);
  CYC(b_+0, b_+3);
  slots_available(gb);
  ret_effect(gb);
}

// related objects and screen checks

static void related_object_var(GB *gb) {
  BASE(objectGetRelatedObject2Var);
  H = A;
  CYC(b_+2, b_+5); A = H8(hActiveObjectType);
  alu_add(gb, L);
  E = A;
  CYC(b_+5, b_+8); A = mem_rd(gb, DE);
  alu_add(gb, H);
  L = A;
  E = alu_inc8(gb, E);
  CYC(b_+8, b_+12); A = mem_rd(gb, DE);
  H = A;
  CYC(b_+12, b_+14);
}

void objectGetRelatedObject1Var_hook(GB *gb) {
  BASE(objectGetRelatedObject1Var);
  L = OBJ_RELATED1;
  CYC(b_+0, b_+4);
  related_object_var(gb);
  ret_effect(gb);
}

void objectGetRelatedObject2Var_hook(GB *gb) {
  BASE(objectGetRelatedObject2Var);
  L = OBJ_RELATED2;
  CYC(b_+0, b_+2);
  related_object_var(gb);
  ret_effect(gb);
}

void objectGetZAboveScreen_hook(GB *gb) {
  BASE(objectGetZAboveScreen);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+2, b_+6); A = mem_rd(gb, DE);
  B = A;
  CYC(b_+6, b_+9); A = H8(hCameraY);
  alu_sub(gb, B);
  alu_sub(gb, 0x08);
  alu_cp(gb, 0x80);
  if (!(F & FC)) CYCT(b_+9, b_+15);
  else { CYC(b_+9, b_+15); A = 0x80; CYC(b_+15, b_+18); }
  ret_effect(gb);
}

void objectCheckWithinScreenBoundary_hook(GB *gb) {
  BASE(objectCheckWithinScreenBoundary);
  CYC(b_+0, b_+2); B = H8(hCameraY);
  CYC(b_+2, b_+5); C = H8(hCameraX);
  CYC(b_+5, b_+8); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+8, b_+12); A = mem_rd(gb, DE);
  alu_sub(gb, B);
  alu_add(gb, 0x07);
  alu_cp(gb, 0x8f);
  if (!(F & FC)) CYCT(b_+12, b_+18);
  else {
    CYC(b_+12, b_+18);
    E = alu_inc8(gb, E);
    E = alu_inc8(gb, E);
    CYC(b_+18, b_+21); A = mem_rd(gb, DE);
    alu_sub(gb, C);
    alu_add(gb, 0x07);
    alu_cp(gb, 0xaf);
    CYC(b_+21, b_+27);
  }
  ret_effect(gb);
}

void objectCheckWithinRoomBoundary_hook(GB *gb) {
  BASE(objectCheckWithinRoomBoundary);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  SET_HL(wRoomEdgeY);
  CYC(b_+2, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(b_+10, b_+11);
  else {
    E = alu_inc8(gb, E);
    E = alu_inc8(gb, E);
    L = alu_inc8(gb, L);
    CYC(b_+10, b_+15); A = mem_rd(gb, DE);
    CYC(b_+15, b_+16); alu_cp(gb, mem_rd(gb, HL));
    CYC(b_+16, b_+17);
  }
  ret_effect(gb);
}

// tiles

static void tile_at_position(GB *gb) {
  BASE(getTileAtPosition);
  A = C;
  alu_and(gb, 0xf0);
  alu_swap_a(gb);
  L = A;
  A = B;
  alu_and(gb, 0xf0);
  alu_or(gb, L);
  L = A;
  H = wRoomLayout >> 8;
  CYC(b_+0, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15);
}

void getTileAtPosition_hook(GB *gb) {
  tile_at_position(gb);
  ret_effect(gb);
}

static void object_get_tile_at_position(GB *gb) {
  BASE(objectGetTileAtPosition);
  CYC(b_+0, b_+3);
  object_get_position(gb);
  tile_at_position(gb);
}

void objectGetTileAtPosition_hook(GB *gb) {
  object_get_tile_at_position(gb);
  ret_effect(gb);
}

static void object_get_relative_tile(GB *gb) {
  BASE(objectGetRelativeTile);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  L = A;
  H = D;
  CYC(b_+2, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, B);
  B = A;
  L = alu_inc8(gb, L);
  CYC(b_+7, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, C);
  C = A;
  CYC(b_+11, b_+15);
  tile_at_position(gb);
}

void objectGetRelativeTile_hook(GB *gb) {
  object_get_relative_tile(gb);
  ret_effect(gb);
}

static bool tile_at_offset_matches(GB *gb, uint16_t call_addr) {
  BASE(objectGetRelativePositionOfTile);
  CYC(call_addr, call_addr + 3);
  alu_add(gb, E);
  L = A;
  CYC(b_+43, b_+47); A = H8(hFF8B);
  CYC(b_+47, b_+48); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+49);
  return F & FZ;
}

void objectGetRelativePositionOfTile_hook(GB *gb) {
  BASE(objectGetRelativePositionOfTile);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  CYC(b_+2, b_+5);
  object_get_short_position(gb);
  E = A;
  H = wRoomLayout >> 8;
  CYC(b_+5, b_+10);
  A = 0xf0;
  if (tile_at_offset_matches(gb, b_+10)) { A = 0x00; CYCT(b_+13, b_+16); ret_effect(gb); return; }
  CYC(b_+13, b_+18);
  A = 0x01;
  if (tile_at_offset_matches(gb, b_+18)) { A = 0x01; CYCT(b_+21, b_+24); ret_effect(gb); return; }
  CYC(b_+21, b_+26);
  A = 0x10;
  if (tile_at_offset_matches(gb, b_+26)) { A = 0x02; CYCT(b_+29, b_+32); ret_effect(gb); return; }
  CYC(b_+29, b_+34);
  A = 0xff;
  if (tile_at_offset_matches(gb, b_+34)) { A = 0x03; CYCT(b_+37, b_+40); ret_effect(gb); return; }
  CYC(b_+37, b_+43);
  A = 0xff;
  ret_effect(gb);
}

static void tile_index_from_layout_buffer(GB *gb) {
  BASE(getTileIndexFromRoomLayoutBuffer_paramC);
  CYC(b_+0, b_+2); uint8_t svbk = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+7); mem_wr(gb, IO_SVBK, 3);
  B = w3RoomLayoutBuffer >> 8;
  CYC(b_+7, b_+10); A = mem_rd(gb, BC);
  E = A;
  CYC(b_+10, b_+15); mem_wr(gb, IO_SVBK, 3);
  L = E;
  H = w3TileCollisions >> 8;
  CYC(b_+15, b_+19); B = mem_rd(gb, HL);
  CYC(b_+19, b_+22); mem_wr(gb, IO_SVBK, svbk);
  A = B;
  alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+22, b_+27); A = E; CYC(b_+33, b_+35); return; }
  CYC(b_+22, b_+28);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); A = E; CYC(b_+33, b_+35); return; }
  CYC(b_+28, b_+30);
  alu_scf(gb);
  A = E;
  CYC(b_+30, b_+33);
}

void getTileIndexFromRoomLayoutBuffer_paramC_hook(GB *gb) {
  tile_index_from_layout_buffer(gb);
  ret_effect(gb);
}

void getTileIndexFromRoomLayoutBuffer_hook(GB *gb) {
  BASE(getTileIndexFromRoomLayoutBuffer);
  C = A;
  CYC(b_+0, b_+1);
  tile_index_from_layout_buffer(gb);
  ret_effect(gb);
}

// angles

static void object_get_relative_angle(GB *gb) {
  BASE(objectGetRelativeAngle);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  E = A;
  CYC(b_+2, b_+5);
  getRelativeAngle_hook(gb);
}

void objectGetRelativeAngle_hook(GB *gb) { object_get_relative_angle(gb); }

void objectGetAngleTowardLink_hook(GB *gb) {
  BASE(objectGetAngleTowardLink);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  B = A;
  CYC(b_+3, b_+7); A = mem_rd(gb, w1Link_xh);
  C = A;
  CYC(b_+7, b_+8);
  object_get_relative_angle(gb);
}

static void angle_toward_enemy_target(GB *gb) {
  BASE(objectGetAngleTowardEnemyTarget);
  CYC(b_+0, b_+2); B = H8(hEnemyTargetY);
  CYC(b_+2, b_+5); C = H8(hEnemyTargetX);
  CYC(b_+5, b_+8);
  object_get_relative_angle(gb);
}

void objectGetAngleTowardEnemyTarget_hook(GB *gb) { angle_toward_enemy_target(gb); }

void objectNudgeAngleTowards_hook(GB *gb) {
  BASE(objectNudgeAngleTowards);
  C = A;
  CYC(b_+0, b_+3); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ANGLE);
  E = A;
  CYC(b_+3, b_+7); A = mem_rd(gb, DE);
  B = A;
  alu_sub(gb, C);
  if (F & FZ) CYCT(b_+7, b_+11);
  else {
    CYC(b_+7, b_+11);
    alu_and(gb, 0x1f);
    alu_cp(gb, 0x10);
    if (!(F & FC)) { CYCT(b_+11, b_+17); CYC(b_+20, b_+21); B = alu_inc8(gb, B); }
    else { CYC(b_+11, b_+20); B = alu_dec8(gb, B); }
  }
  A = B;
  alu_and(gb, 0x1f);
  CYC(b_+21, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26);
  ret_effect(gb);
}

static void centered_with_link(GB *gb) {
  BASE(objectCheckCenteredWithLink);
  C = B;
  C = alu_sla(gb, C);
  C = alu_inc8(gb, C);
  H = D;
  CYC(b_+0, b_+7); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+7, b_+13); A = mem_rd(gb, w1Link_yh);
  CYC(b_+13, b_+14); alu_sub(gb, mem_rd(gb, HL));
  alu_add(gb, B);
  alu_cp(gb, C);
  if (F & FC) CYCT(b_+14, b_+17);
  else {
    L = alu_inc8(gb, L);
    L = alu_inc8(gb, L);
    CYC(b_+14, b_+22); A = mem_rd(gb, w1Link_xh);
    CYC(b_+22, b_+23); alu_sub(gb, mem_rd(gb, HL));
    alu_add(gb, B);
    alu_cp(gb, C);
    CYC(b_+23, b_+26);
  }
}

void objectCheckCenteredWithLink_hook(GB *gb) {
  centered_with_link(gb);
  ret_effect(gb);
}

static void link_within_distance(GB *gb) {
  BASE(objectCheckLinkWithinDistance);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  H = D;
  E = 4;
  CYC(b_+2, b_+11); A = mem_rd(gb, w1Link_yh);
  CYC(b_+11, b_+12); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(b_+12, b_+14);
  else { CYC(b_+12, b_+18); alu_cpl(gb); A = alu_inc8(gb, A); E = 0; }
  B = A;
  A = C;
  alu_sub(gb, B);
  alu_ccf(gb);
  if (!(F & FC)) { CYCT(b_+18, b_+23); return; }
  CYC(b_+18, b_+23);
  C = A;
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  E |= 0x20;
  CYC(b_+23, b_+31); A = mem_rd(gb, w1Link_xh);
  CYC(b_+31, b_+32); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(b_+32, b_+34);
  else { CYC(b_+32, b_+38); alu_cpl(gb); A = alu_inc8(gb, A); E |= 0x40; }
  alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+38, b_+40); return; }
  CYC(b_+38, b_+40);
  alu_cp(gb, B);
  if (F & FC) CYCT(b_+40, b_+43);
  else { CYC(b_+40, b_+45); E = alu_swap(gb, E); }
  A = E;
  alu_and(gb, 0x06);
  alu_scf(gb);
  CYC(b_+45, b_+50);
}

void objectCheckLinkWithinDistance_hook(GB *gb) {
  link_within_distance(gb);
  ret_effect(gb);
}

// speed

static void add_speed_component(GB *gb) {
  BASE(objectApplyComponentSpeed);
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E);
  CYC(b_+16, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21);
}

void objectApplyComponentSpeed_hook(GB *gb) {
  BASE(objectApplyComponentSpeed);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_Y);
  L = A;
  alu_add(gb, 0x06);
  E = A;
  H = D;
  CYC(b_+2, b_+12);
  add_speed_component(gb);
  CYC(b_+12, b_+13);
  E = alu_inc8(gb, E);
  add_speed_component(gb);
  ret_effect(gb);
}

static void velocity_invalid(GB *gb) {
  BASE(getPositionOffsetForVelocity);
  SET_HL(wTmpcec0 + 3);
  alu_xor(gb, A);
  CYC(b_+64, b_+69); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+69, b_+70); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+70, b_+71); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  CYC(b_+72, b_+73);
}

static void position_offset_for_velocity(GB *gb) {
  BASE(getPositionOffsetForVelocity);
  alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(b_+0, b_+4); velocity_invalid(gb); return; }
  CYC(b_+0, b_+4);
  B = alu_swap(gb, B);
  if (F & FZ) { CYCT(b_+4, b_+8); velocity_invalid(gb); return; }
  CYC(b_+4, b_+8);
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
  CYC(b_+8, b_+28); A = H8(hRomBank);
  CYC(b_+28, b_+29); push_effect(gb, AF);
  A = 0x03;
  CYC(b_+29, b_+33); H8(hRomBank) = A;
  CYC(b_+33, b_+36); mem_wr(gb, MBC_ROM_BANK, A);
  SET_BC(wTmpcec0);
  CYC(b_+36, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+40, b_+41); mem_wr(gb, BC, A);
  C = alu_inc8(gb, C);
  CYC(b_+41, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+43, b_+44); mem_wr(gb, BC, A);
  C = alu_inc8(gb, C);
  A = 0x0e;
  CYC(b_+44, b_+48);
  add_a_to_hl(gb);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+50); mem_wr(gb, BC, A);
  C = alu_inc8(gb, C);
  CYC(b_+50, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+52, b_+53); mem_wr(gb, BC, A);
  CYC(b_+53, b_+54); SET_AF(pop_effect(gb));
  CYC(b_+54, b_+56); H8(hRomBank) = A;
  CYC(b_+56, b_+59); mem_wr(gb, MBC_ROM_BANK, A);
  SET_HL(wTmpcec0);
  alu_or(gb, H);
  CYC(b_+59, b_+64);
}

void getPositionOffsetForVelocity_hook(GB *gb) {
  position_offset_for_velocity(gb);
  ret_effect(gb);
}

static void apply_given_speed(GB *gb) {
  BASE(objectApplyGivenSpeed);
  CYC(b_+0, b_+3);
  position_offset_for_velocity(gb);
  if (F & FZ) { CYCT(b_+3, b_+4); return; }
  E = alu_inc8(gb, E);
  CYC(b_+3, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+8, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+13, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E); L = alu_inc8(gb, L);
  CYC(b_+18, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24);
}

void objectApplyGivenSpeed_hook(GB *gb) {
  apply_given_speed(gb);
  ret_effect(gb);
}

static void object_apply_speed(GB *gb) {
  BASE(objectApplySpeed);
  H = D;
  CYC(b_+0, b_+3); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ANGLE);
  E = A;
  L = A;
  CYC(b_+3, b_+8); C = mem_rd(gb, HL);
  alu_add(gb, 0x07);
  L = A;
  CYC(b_+8, b_+12); B = mem_rd(gb, HL);
  apply_given_speed(gb);
}

void objectApplySpeed_hook(GB *gb) {
  object_apply_speed(gb);
  ret_effect(gb);
}

// vertical speed

static void update_speed_z_param_c(GB *gb) {
  BASE(objectUpdateSpeedZ_paramC);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_Z);
  E = A;
  alu_add(gb, 0x06);
  L = A;
  H = D;
  CYC(b_+2, b_+12);
  add16_bit_refs(gb);
  alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+12, b_+16);
    alu_xor(gb, A);
    CYC(b_+26, b_+28); mem_wr(gb, DE, A);
    E = alu_dec8(gb, E);
    CYC(b_+28, b_+30); mem_wr(gb, DE, A);
    alu_xor(gb, A);
    CYC(b_+30, b_+32);
    return;
  }
  CYC(b_+12, b_+16);
  L = alu_dec8(gb, L);
  A = C;
  CYC(b_+16, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0;
  CYC(b_+20, b_+23); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  alu_or(gb, D);
  CYC(b_+24, b_+26);
}

void objectUpdateSpeedZ_paramC_hook(GB *gb) {
  update_speed_z_param_c(gb);
  ret_effect(gb);
}

void objectUpdateSpeedZ_hook(GB *gb) {
  BASE(objectUpdateSpeedZ);
  C = A;
  CYC(b_+0, b_+1);
  update_speed_z_param_c(gb);
  ret_effect(gb);
}

void objectUpdateSpeedZ_sidescroll_hook(GB *gb) {
  BASE(objectUpdateSpeedZ_sidescroll);
  B = 0x06;
  CYC(b_+0, b_+2);
  objectUpdateSpeedZ_sidescroll_givenYOffset_hook(gb);
}

void objectSetSpeedZ_hook(GB *gb) {
  BASE(objectSetSpeedZ);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_SPEED_Z);
  L = A;
  H = D;
  CYC(b_+2, b_+7); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+10);
  ret_effect(gb);
}

static void negate_and_halve_speed_z(GB *gb) {
  BASE(objectNegateAndHalveSpeedZ);
  H = D;
  CYC(b_+0, b_+3); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_SPEED_Z);
  L = A;
  CYC(b_+3, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_cpl(gb);
  C = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, HL);
  alu_cpl(gb);
  B = A;
  SET_BC(BC + 1);
  B = alu_sra(gb, B);
  C = alu_rr(gb, C);
  SET_HL(0x10000 - 0x80);
  CYC(b_+10, b_+23);
  compare_hl_to_bc(gb);
  A = alu_inc8(gb, A);
  alu_scf(gb);
  if (F & FZ) { CYCT(b_+23, b_+26); return; }
  CYC(b_+23, b_+28); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_SPEED_Z);
  E = A;
  A = C;
  CYC(b_+28, b_+33); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = B;
  CYC(b_+33, b_+36); mem_wr(gb, DE, A);
  alu_or(gb, C);
  alu_scf(gb);
  if (F & FZ) { CYCT(b_+36, b_+39); return; }
  CYC(b_+36, b_+39);
  alu_xor(gb, A);
  CYC(b_+39, b_+41);
}

void objectNegateAndHalveSpeedZ_hook(GB *gb) {
  negate_and_halve_speed_z(gb);
  ret_effect(gb);
}

void objectUpdateSpeedZAndBounce_hook(GB *gb) {
  BASE(objectUpdateSpeedZAndBounce);
  CYC(b_+0, b_+3);
  update_speed_z_param_c(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  negate_and_halve_speed_z(gb);
  ret_effect(gb);
}

// collision boxes

static void h_check_contains_point(GB *gb) {
  BASE(objectHCheckContainsPoint);
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+0, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_sub(gb, B);
  if (!(F & FC)) CYCT(b_+7, b_+10);
  else { CYC(b_+7, b_+12); alu_cpl(gb); A = alu_inc8(gb, A); }
  B = A;
  L = alu_inc8(gb, L);
  CYC(b_+12, b_+15); A = mem_rd(gb, HL);
  alu_sub(gb, C);
  if (!(F & FC)) CYCT(b_+15, b_+18);
  else { CYC(b_+15, b_+20); alu_cpl(gb); A = alu_inc8(gb, A); }
  C = A;
  A = L;
  alu_add(gb, 0x19);
  L = A;
  A = B;
  CYC(b_+20, b_+27); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+27, b_+28); return; }
  L = alu_inc8(gb, L);
  A = C;
  CYC(b_+27, b_+31); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32);
}

void objectHCheckContainsPoint_hook(GB *gb) {
  h_check_contains_point(gb);
  ret_effect(gb);
}

void objectCheckContainsPoint_hook(GB *gb) {
  BASE(objectCheckContainsPoint);
  H = D;
  CYC(b_+0, b_+3); L = H8(hActiveObjectType);
  CYC(b_+3, b_+6);
  h_check_contains_point(gb);
  ret_effect(gb);
}

void interactionCheckContainsPoint_hook(GB *gb) {
  BASE(interactionCheckContainsPoint);
  H = D;
  L = INTERACTION_BASE;
  CYC(b_+0, b_+3);
  h_check_contains_point(gb);
  ret_effect(gb);
}

static void objects_collided_from_variables(GB *gb) {
  BASE(checkObjectsCollidedFromVariables);
  A = B;
  CYC(b_+0, b_+3); H8(hFF8D) = A;
  A = C;
  CYC(b_+3, b_+6); H8(hFF8C) = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_add(gb, mem_rd(gb, HL));
  B = A;
  CYC(b_+8, b_+11); A = H8(hFF8F);
  C = A;
  CYC(b_+11, b_+14); A = H8(hFF8D);
  alu_sub(gb, C);
  alu_add(gb, B);
  B = alu_sla(gb, B);
  alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+14, b_+20); return; }
  CYC(b_+14, b_+20);
  E = alu_inc8(gb, E);
  SET_HL(HL + 1);
  CYC(b_+20, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
  B = A;
  CYC(b_+24, b_+27); A = H8(hFF8E);
  C = A;
  CYC(b_+27, b_+30); A = H8(hFF8C);
  alu_sub(gb, C);
  alu_add(gb, B);
  B = alu_sla(gb, B);
  alu_cp(gb, B);
  CYC(b_+30, b_+36);
}

void checkObjectsCollidedFromVariables_hook(GB *gb) {
  objects_collided_from_variables(gb);
  ret_effect(gb);
}

static void collided_with_link_from_1c56(GB *gb) {
  BASE(_checkCollidedWithLink);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+16, b_+18); H8(hFF8E) = A;
  L = alu_dec8(gb, L);
  CYC(b_+18, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); H8(hFF8F) = A;
  A = L;
  alu_add(gb, 0x1b);
  E = A;
  CYC(b_+22, b_+29); A = W8(wLinkObjectIndex);
  H = A;
  L = OBJ_YH;
  CYC(b_+29, b_+33); B = mem_rd(gb, HL);
  L = OBJ_XH;
  CYC(b_+33, b_+36); C = mem_rd(gb, HL);
  L = OBJ_COLLISION_RADIUS_Y;
  CYC(b_+36, b_+40);
  objects_collided_from_variables(gb);
}

static void check_collided_with_link(GB *gb) {
  BASE(_checkCollidedWithLink);
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  B = A;
  C = OBJ_ZH;
  CYC(b_+3, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); alu_sub(gb, mem_rd(gb, HL));
  alu_add(gb, 0x07);
  alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(b_+8, b_+13); return; }
  CYC(b_+8, b_+13);
  L = alu_dec8(gb, L);
  L = alu_dec8(gb, L);
  CYC(b_+13, b_+15);
  collided_with_link_from_1c56(gb);
}

void _checkCollidedWithLink_hook(GB *gb) {
  check_collided_with_link(gb);
  ret_effect(gb);
}

static void object_check_collided_with_link(GB *gb) {
  BASE(objectCheckCollidedWithLink);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  L = A;
  H = D;
  CYC(b_+2, b_+6);
  check_collided_with_link(gb);
}

void objectCheckCollidedWithLink_hook(GB *gb) {
  object_check_collided_with_link(gb);
  ret_effect(gb);
}

void objectCheckCollidedWithLink_ignoreZ_hook(GB *gb) {
  BASE(objectCheckCollidedWithLink_ignoreZ);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_XH);
  L = A;
  H = D;
  CYC(b_+2, b_+8);
  collided_with_link_from_1c56(gb);
  ret_effect(gb);
}

static void collided_with_link_not_dead(GB *gb) {
  BASE(objectCheckCollidedWithLink_notDead);
  CYC(b_+0, b_+3); A = W8(wLinkDeathTrigger);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); return; }
  CYC(b_+3, b_+7);
  object_check_collided_with_link(gb);
}

void objectCheckCollidedWithLink_notDead_hook(GB *gb) {
  collided_with_link_not_dead(gb);
  ret_effect(gb);
}

void objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook(GB *gb) {
  BASE(objectCheckCollidedWithLink_notDeadAndNotGrabbing);
  CYC(b_+0, b_+3); A = W8(wLinkGrabState);
  alu_and(gb, 0xbe);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  collided_with_link_not_dead(gb);
  ret_effect(gb);
}

void objectCheckCollidedWithLink_onGround_hook(GB *gb) {
  BASE(objectCheckCollidedWithLink_onGround);
  CYC(b_+0, b_+3); A = W8(wLinkInAir);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+8); A = mem_rd(gb, w1Link_zh);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); ret_effect(gb); return; }
  CYC(b_+8, b_+12);
  collided_with_link_not_dead(gb);
  ret_effect(gb);
}

static void objects_collided(GB *gb) {
  BASE(checkObjectsCollided);
  A = L;
  alu_and(gb, 0xc0);
  L = A;
  uint16_t hl = HL;
  H = D;
  CYC(b_+0, b_+6); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+6, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+14); H8(hFF8F) = A;
  L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+18); H8(hFF8E) = A;
  A = L;
  alu_add(gb, 0x19);
  E = A;
  SET_HL(hl);
  A = L;
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+18, b_+28); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(b_+28, b_+31); C = mem_rd(gb, HL);
  alu_add(gb, 0x1b);
  L = A;
  CYC(b_+31, b_+37);
  objects_collided_from_variables(gb);
}

void checkObjectsCollided_hook(GB *gb) {
  objects_collided(gb);
  ret_effect(gb);
}

static void link_position_plus_direction_offset(GB *gb) {
  BASE(_getLinkPositionPlusDirectionOffset);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_direction);
  SET_HL(linkDirectionOffsets);
  CYC(b_+3, b_+7);
  add_double_index_to_hl(gb);
  SET_DE(w1Link_yh);
  CYC(b_+7, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); H8(hFF8F) = A;
  SET_HL(HL + 1);
  E = OBJ_XH;
  CYC(b_+14, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); H8(hFF8E) = A;
  E = OBJ_ZH;
  CYC(b_+21, b_+24); A = mem_rd(gb, DE);
  alu_sub(gb, 0x03);
  CYC(b_+24, b_+28); H8(hFF91) = A;
  CYC(b_+28, b_+29);
}

void _getLinkPositionPlusDirectionOffset_hook(GB *gb) {
  link_position_plus_direction_offset(gb);
  ret_effect(gb);
}

static void check_collision_with_h_and_d(GB *gb) {
  BASE(_checkCollisionWithHAndD);
  alu_add(gb, OBJ_VAR2A);
  L = A;
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+6); return; }
  CYC(b_+5, b_+6);
  alu_sub(gb, 0x1b);
  L = A;
  CYC(b_+6, b_+11); A = H8(hFF91);
  CYC(b_+11, b_+12); alu_sub(gb, mem_rd(gb, HL));
  alu_add(gb, 0x07);
  alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(b_+12, b_+17); return; }
  CYC(b_+12, b_+17);
  L = alu_dec8(gb, L);
  L = alu_dec8(gb, L);
  CYC(b_+17, b_+20); A = mem_rd(gb, HL); SET_HL(HL - 1);
  L = alu_dec8(gb, L);
  CYC(b_+20, b_+22); B = mem_rd(gb, HL);
  C = A;
  A = L;
  alu_add(gb, 0x1b);
  L = A;
  E = OBJ_COLLISION_RADIUS_Y;
  CYC(b_+22, b_+32);
  objects_collided_from_variables(gb);
}

void _checkCollisionWithHAndD_hook(GB *gb) {
  check_collision_with_h_and_d(gb);
  ret_effect(gb);
}

static void object_h_check_collision_with_link(GB *gb) {
  BASE(objectHCheckCollisionWithLink);
  uint16_t de = DE, hl = HL;
  CYC(b_+0, b_+5);
  link_position_plus_direction_offset(gb);
  SET_HL(hl);
  A = L;
  alu_and(gb, 0xc0);
  CYC(b_+5, b_+12);
  check_collision_with_h_and_d(gb);
  SET_DE(de);
  CYC(b_+12, b_+14);
}

void objectHCheckCollisionWithLink_hook(GB *gb) {
  object_h_check_collision_with_link(gb);
  ret_effect(gb);
}

// Link state checks (all end at checkLinkCollisionsEnabled@noCarry or with scf)

static void link_check_no_carry(GB *gb) {
  BASE(checkLinkCollisionsEnabled);
  alu_xor(gb, A);
  CYC(b_+38, b_+40);
}

static void link_control_checks_tail(GB *gb) {
  BASE(checkLinkCollisionsEnabled);
  CYC(b_+18, b_+21); A = GVW(wLinkDeathTrigger, wMenuDisabled);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+21, b_+24); link_check_no_carry(gb); return; }
  CYC(b_+21, b_+27); A = W8(wcc95);
  alu_rlca(gb);
  if (F & FC) { CYCT(b_+27, b_+30); link_check_no_carry(gb); return; }
  CYC(b_+27, b_+33); A = W8(wLinkInAir);
  alu_rlca(gb);
  if (F & FC) { CYCT(b_+33, b_+36); link_check_no_carry(gb); return; }
  CYC(b_+33, b_+36);
  alu_scf(gb);
  CYC(b_+36, b_+38);
}

void checkLinkID0AndControlNormal_hook(GB *gb) {
  BASE(checkLinkID0AndControlNormal);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_id);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(b_+3, b_+6); alu_xor(gb, A); CYC(b_+6, b_+8); ret_effect(gb); return; }
  CYCT(b_+3, b_+6);
  link_control_checks_tail(gb);
  ret_effect(gb);
}

static void link_collisions_enabled(GB *gb) {
  BASE(checkLinkCollisionsEnabled);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_collisionType);
  alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+3, b_+6); link_check_no_carry(gb); return; }
  CYC(b_+3, b_+9); A = GVW(wDisableLinkCollisionsAndMenu, wLinkDeathTrigger);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); link_check_no_carry(gb); return; }
  CYC(b_+9, b_+15); A = GVW(wMenuDisabled, wDisableLinkCollisionsAndMenu);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); link_check_no_carry(gb); return; }
  CYC(b_+15, b_+18);
  link_control_checks_tail(gb);
}

void checkLinkCollisionsEnabled_hook(GB *gb) {
  link_collisions_enabled(gb);
  ret_effect(gb);
}

static void link_vulnerable(GB *gb) {
  BASE(checkLinkVulnerable);
  SET_HL(w1Link_var2a);
  CYC(b_+0, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); alu_or(gb, mem_rd(gb, HL));
  L = 0x2d;
  CYC(b_+4, b_+7); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+7, b_+10); link_check_no_carry(gb); return; }
  CYC(b_+7, b_+10);
  link_collisions_enabled(gb);
}

void checkLinkVulnerable_hook(GB *gb) {
  link_vulnerable(gb);
  ret_effect(gb);
}

void checkLinkVulnerableAndIDZero_hook(GB *gb) {
  BASE(checkLinkVulnerableAndIDZero);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_id);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(b_+3, b_+6); alu_xor(gb, A); CYC(b_+6, b_+8); ret_effect(gb); return; }
  CYCT(b_+3, b_+6);
  link_vulnerable(gb);
  ret_effect(gb);
}

// simple tile collision (quarter tiles) at the object's position

void objectCheckSimpleCollision_hook(GB *gb) {
  BASE(objectCheckSimpleCollision);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  L = A;
  H = D;
  CYC(b_+2, b_+7); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L);
  L = alu_inc8(gb, L);
  CYC(b_+7, b_+10); C = mem_rd(gb, HL);
  A = B;
  alu_and(gb, 0xf0);
  L = A;
  A = C;
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_or(gb, L);
  L = A;
  H = wRoomCollisions >> 8;
  CYC(b_+10, b_+24); A = mem_rd(gb, HL);
  alu_bit(gb, 3, B);
  if (F & FZ) { CYC(b_+24, b_+28); alu_rrca(gb); alu_rrca(gb); CYC(b_+28, b_+30); }
  else CYCT(b_+24, b_+28);
  alu_bit(gb, 3, C);
  if (F & FZ) { CYC(b_+30, b_+34); alu_rrca(gb); CYC(b_+34, b_+35); }
  else CYCT(b_+30, b_+34);
  alu_and(gb, 0x01);
  CYC(b_+35, b_+38);
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
  BASE(getTileCollisionsAtPosition);
  position_to_collision_addr(gb, b_+0, b_+11);
  H = wRoomCollisions >> 8;
  CYC(b_+11, b_+13); A = mem_rd(gb, HL);
  alu_or(gb, A);
  CYC(b_+13, b_+16);
}

void getTileCollisionsAtPosition_hook(GB *gb) {
  tile_collisions_at_position(gb);
  ret_effect(gb);
}

static void object_get_tile_collisions(GB *gb) {
  object_position_to_bc(gb, SYM(objectGetTileCollisions));
  tile_collisions_at_position(gb);
}

void objectGetTileCollisions_hook(GB *gb) {
  object_get_tile_collisions(gb);
  ret_effect(gb);
}

static void simple_collision(GB *gb) {
  BASE(_simpleCollision);
  alu_bit(gb, 3, B);
  if (F & FZ) { CYC(b_+0, b_+4); alu_rrca(gb); alu_rrca(gb); CYC(b_+4, b_+6); }
  else CYCT(b_+0, b_+4);
  alu_bit(gb, 3, C);
  if (F & FZ) { CYC(b_+6, b_+10); alu_rrca(gb); CYC(b_+10, b_+11); }
  else CYCT(b_+6, b_+10);
  alu_rrca(gb);
  CYC(b_+11, b_+13);
}

void _simpleCollision_hook(GB *gb) {
  simple_collision(gb);
  ret_effect(gb);
}

static void complex_collision(GB *gb) {
  BASE(_complexCollision);
  uint16_t de = DE;
  CYC(b_+0, b_+1);
  alu_and(gb, 0x0f);
  E = A;
  D = 0;
  alu_add_hl(gb, DE);
  CYC(b_+1, b_+7); E = mem_rd(gb, HL);
  alu_cp(gb, 0x08);
  A = B;
  if (!(F & FC)) CYCT(b_+7, b_+13);
  else { CYC(b_+7, b_+14); A = C; }
  alu_rrca(gb);
  alu_and(gb, 0x07);
  SET_HL(bitTable);
  alu_add(gb, L);
  L = A;
  CYC(b_+14, b_+22); A = mem_rd(gb, HL);
  alu_and(gb, E);
  SET_DE(de);
  if (F & FZ) { CYCT(b_+22, b_+26); return; }
  CYC(b_+22, b_+26);
  alu_scf(gb);
  CYC(b_+26, b_+28);
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

void checkGivenCollision_allowHoles_hook(GB *gb) { given_collision(gb, SYM(checkGivenCollision_allowHoles), specialCollisions_allowHoles); ret_effect(gb); }
void checkTileCollision_allowHoles_hook(GB *gb) { tile_collision(gb, SYM(checkTileCollision_allowHoles), specialCollisions_allowHoles); ret_effect(gb); }
void checkTileCollisionAt_allowHoles_hook(GB *gb) {
  position_to_collision_addr(gb, SYM(checkTileCollisionAt_allowHoles), SYM(checkTileCollision_allowHoles));
  tile_collision(gb, SYM(checkTileCollision_allowHoles), specialCollisions_allowHoles);
  ret_effect(gb);
}
void objectCheckTileCollision_allowHoles_hook(GB *gb) {
  object_position_to_bc(gb, SYM(objectCheckTileCollision_allowHoles));
  position_to_collision_addr(gb, SYM(checkTileCollisionAt_allowHoles), SYM(checkTileCollision_allowHoles));
  tile_collision(gb, SYM(checkTileCollision_allowHoles), specialCollisions_allowHoles);
  ret_effect(gb);
}

void checkGivenCollision_disallowHoles_hook(GB *gb) { given_collision(gb, SYM(checkGivenCollision_disallowHoles), specialCollisions_disallowHoles); ret_effect(gb); }
void checkTileCollision_disallowHoles_hook(GB *gb) { tile_collision(gb, SYM(checkTileCollision_disallowHoles), specialCollisions_disallowHoles); ret_effect(gb); }
void checkTileCollisionAt_disallowHoles_hook(GB *gb) {
  position_to_collision_addr(gb, SYM(checkTileCollisionAt_disallowHoles), SYM(checkTileCollision_disallowHoles));
  tile_collision(gb, SYM(checkTileCollision_disallowHoles), specialCollisions_disallowHoles);
  ret_effect(gb);
}
void objectCheckTileCollision_disallowHoles_hook(GB *gb) {
  object_position_to_bc(gb, SYM(objectCheckTileCollision_disallowHoles));
  position_to_collision_addr(gb, SYM(checkTileCollisionAt_disallowHoles), SYM(checkTileCollision_disallowHoles));
  tile_collision(gb, SYM(checkTileCollision_disallowHoles), specialCollisions_disallowHoles);
  ret_effect(gb);
}

void checkCollisionPosition_disallowSmallBridges_hook(GB *gb) {
  tile_collision(gb, SYM(checkCollisionPosition_disallowSmallBridges), specialCollisions_disallowSmallBridges);
  ret_effect(gb);
}

static void retrieve_tile_collision_value(GB *gb) {
  BASE(retrieveTileCollisionValue);
  H = w3TileCollisions >> 8;
  L = A;
  A = 0x03;
  CYC(b_+0, b_+7); mem_wr(gb, IO_SVBK, A);
  CYC(b_+7, b_+8); L = mem_rd(gb, HL);
  alu_xor(gb, A);
  CYC(b_+8, b_+11); mem_wr(gb, IO_SVBK, A);
  A = L;
  CYC(b_+11, b_+13);
}

void retrieveTileCollisionValue_hook(GB *gb) {
  retrieve_tile_collision_value(gb);
  ret_effect(gb);
}

static void backwards_search(GB *gb) {
  BASE(backwardsSearch);
  for (;;) {
    CYC(b_+0, b_+1); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+1, b_+2); return; }
    L = alu_dec8(gb, L);
    if (L) { CYC(b_+1, b_+3); CYCT(b_+3, b_+5); continue; }
    CYC(b_+1, b_+6); alu_cp(gb, mem_rd(gb, HL));
    CYC(b_+6, b_+7);
    return;
  }
}

void backwardsSearch_hook(GB *gb) {
  backwards_search(gb);
  ret_effect(gb);
}

void findTileInRoom_hook(GB *gb) {
  BASE(findTileInRoom);
  H = wRoomLayout >> 8;
  L = 0xbf;
  CYC(b_+0, b_+4);
  backwards_search(gb);
  ret_effect(gb);
}

// hazards (water, holes, lava)

static void object_check_is_over_hazard(GB *gb) {
  BASE(objectCheckIsOverHazard);
  SET_BC(0x0500);
  CYC(b_+0, b_+6);
  object_get_relative_tile(gb);
  CYC(b_+6, b_+9); W8(wObjectTileIndex) = A;
  SET_HL(hazardCollisionTable);
  CYC(b_+9, b_+15);
  lookup_collision_table(gb);
}

void objectCheckIsOverHazard_hook(GB *gb) {
  object_check_is_over_hazard(gb);
  ret_effect(gb);
}

static void object_check_is_on_hazard(GB *gb) {
  BASE(objectCheckIsOnHazard);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  E = A;
  CYC(b_+2, b_+6); A = mem_rd(gb, DE);
  alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+6, b_+9); return; }
  CYC(b_+6, b_+9);
  object_check_is_over_hazard(gb);
}

void objectCheckIsOnHazard_hook(GB *gb) {
  object_check_is_on_hazard(gb);
  ret_effect(gb);
}

void checkLinkIsOverHazard_hook(GB *gb) {
  BASE(checkLinkIsOverHazard);
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  D = A;
  CYC(b_+3, b_+6); H8(hActiveObject) = A;
  alu_xor(gb, A);
  CYC(b_+6, b_+9); H8(hActiveObjectType) = A;
  E = 0x01;
  CYC(b_+9, b_+12); A = mem_rd(gb, DE);
  alu_sub(gb, 0x0c);
  if (F & FZ) { CYCT(b_+12, b_+15); ret_effect(gb); return; }
  uint16_t bc = BC, hl = HL;
  CYC(b_+12, b_+20);
  object_check_is_over_hazard(gb);
  SET_HL(hl);
  SET_BC(bc);
  CYC(b_+20, b_+23);
  ret_effect(gb);
}

void objectCheckTileAtPositionIsWater_hook(GB *gb) {
  BASE(objectCheckTileAtPositionIsWater);
  CYC(b_+0, b_+3);
  object_get_tile_at_position(gb);
  alu_sub(gb, GV(0xf9, 0xfa));
  alu_cp(gb, GV(0x05, 0x04));
  CYC(b_+3, b_+8);
  ret_effect(gb);
}

void checkTileAtPositionIsWater_hook(GB *gb) {
  BASE(checkTileAtPositionIsWater);
  CYC(b_+0, b_+3);
  tile_at_position(gb);
  alu_sub(gb, GV(0xf9, 0xfa));
  alu_cp(gb, GV(0x05, 0x04));
  CYC(b_+3, b_+8);
  ret_effect(gb);
}

// object searches by id

static void find_item_with_id(GB *gb, bool check_first) {
  BASE(findItemWithID);
  for (;;) {
    if (check_first) {
      L = 0x01;
      CYC(b_+2, b_+5); A = mem_rd(gb, HL);
      alu_cp(gb, C);
      if (F & FZ) { CYCT(b_+5, b_+7); return; }
      CYC(b_+5, b_+7);
    }
    check_first = true;
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(SYM(findItemWithID_startingAfterH), (SYM(findItemWithID_startingAfterH) + 6)); continue; }
    CYC(SYM(findItemWithID_startingAfterH), (SYM(findItemWithID_startingAfterH) + 6));
    alu_or(gb, H);
    CYC((SYM(findItemWithID_startingAfterH) + 6), (SYM(findItemWithID_startingAfterH) + 8));
    return;
  }
}

void findItemWithID_hook(GB *gb) {
  BASE(findItemWithID);
  H = w1WeaponItem >> 8;
  CYC(b_+0, b_+2);
  find_item_with_id(gb, true);
  ret_effect(gb);
}

void findItemWithID_startingAfterH_hook(GB *gb) {
  find_item_with_id(gb, false);
  ret_effect(gb);
}

static void find_same_type_object(GB *gb, bool check_first) {
  BASE(objectFindSameTypeObjectWithID);
  for (;;) {
    if (check_first) {
      CYC(b_+9, b_+10); A = mem_rd(gb, HL);
      alu_cp(gb, C);
      if (F & FZ) { CYCT(b_+10, b_+12); return; }
      CYC(b_+10, b_+12);
    }
    check_first = true;
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(SYM(func_228f), (SYM(func_228f) + 6)); continue; }
    CYC(SYM(func_228f), (SYM(func_228f) + 6));
    alu_or(gb, H);
    CYC((SYM(func_228f) + 6), (SYM(func_228f) + 8));
    return;
  }
}

void objectFindSameTypeObjectWithID_hook(GB *gb) {
  BASE(objectFindSameTypeObjectWithID);
  CYC(b_+0, b_+2); A = H8(hActiveObject);
  alu_and(gb, 0xf0);
  H = A;
  CYC(b_+2, b_+7); A = H8(hActiveObjectType);
  A = alu_inc8(gb, A);
  L = A;
  CYC(b_+7, b_+9);
  find_same_type_object(gb, true);
  ret_effect(gb);
}

void func_228f_hook(GB *gb) {
  find_same_type_object(gb, false);
  ret_effect(gb);
}

// priority relative to Link

static void get_priority(GB *gb) {
  BASE(objectSetPriorityRelativeToLink_withTerrainEffects);
  CYC(b_+14, b_+16); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  E = A;
  CYC(b_+16, b_+20); A = mem_rd(gb, DE);
  A = alu_dec8(gb, A);
  B = 0x03;
  alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+20, b_+26); return; }
  CYC(b_+20, b_+26);
  B = alu_dec8(gb, B);
  A = E;
  alu_add(gb, 0xfc);
  E = A;
  CYC(b_+26, b_+32); A = mem_rd(gb, DE);
  E = A;
  CYC(b_+32, b_+36); A = W8(wLinkObjectIndex);
  H = A;
  L = OBJ_YH;
  CYC(b_+36, b_+40); A = mem_rd(gb, HL);
  alu_add(gb, 0x0b);
  alu_cp(gb, E);
  if (!(F & FC)) { CYCT(b_+40, b_+44); return; }
  CYC(b_+40, b_+44);
  B = alu_dec8(gb, B);
  CYC(b_+44, b_+46);
}

static void set_priority_relative_to_link(GB *gb) {
  BASE(objectSetPriorityRelativeToLink_withTerrainEffects);
  CYC(b_+2, b_+5);
  get_priority(gb);
  CYC(b_+5, b_+7); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_VISIBLE);
  E = A;
  A = C;
  alu_or(gb, B);
  CYC(b_+7, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14);
}

void objectSetPriorityRelativeToLink_hook(GB *gb) {
  BASE(objectSetPriorityRelativeToLink);
  C = 0x80;
  CYC(b_+0, b_+4);
  set_priority_relative_to_link(gb);
  ret_effect(gb);
}

void objectSetPriorityRelativeToLink_withTerrainEffects_hook(GB *gb) {
  BASE(objectSetPriorityRelativeToLink_withTerrainEffects);
  C = 0xc0;
  CYC(b_+0, b_+2);
  set_priority_relative_to_link(gb);
  ret_effect(gb);
}

// rupees and text

static void get_rupee_value(GB *gb) {
  BASE(getRupeeValue);
  uint16_t hl = HL;
  alu_cp(gb, 0x14);
  if (F & FC) CYCT(b_+0, b_+5);
  else { CYC(b_+0, b_+7); A = 0x14; }
  SET_HL(rupeeValues);
  CYC(b_+7, b_+11);
  add_double_index_to_hl(gb);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); B = mem_rd(gb, HL);
  C = A;
  SET_HL(hl);
  CYC(b_+13, b_+16);
}

void getRupeeValue_hook(GB *gb) {
  get_rupee_value(gb);
  ret_effect(gb);
}

void cpRupeeValue_hook(GB *gb) {
  BASE(cpRupeeValue);
  SET_HL(wNumRupees);
  CYC(b_+0, b_+6);
  get_rupee_value(gb);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+8, b_+12);
  compare_hl_to_bc(gb);
  A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); alu_xor(gb, A); CYC(b_+17, b_+19); }
  else { CYC(b_+12, b_+15); A = alu_inc8(gb, A); CYC(b_+15, b_+17); }
  ret_effect(gb);
}

void retIfTextIsActive_hook(GB *gb) {
  BASE(retIfTextIsActive);
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+5); SET_AF(pop_effect(gb));
  CYC(b_+5, b_+7);
  ret_effect(gb);
}

// scaled velocity

static void scale_component(GB *gb) {
  BASE(getScaledPositionOffsetForVelocity);
  uint16_t hl = HL;
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  E = 0x08;
  SET_HL(0);
  CYC(b_+12, b_+20); A = H8(hFF8B);
  do {
    alu_add_hl(gb, HL);
    alu_rlca(gb);
    if (F & FC) { CYC(b_+20, b_+24); CYC(b_+24, b_+25); alu_add_hl(gb, BC); }
    else CYCT(b_+20, b_+24);
    E = alu_dec8(gb, E);
    if (E) CYCT(b_+25, b_+28); else CYC(b_+25, b_+28);
  } while (E);
  A = L;
  B = H;
  SET_HL(hl);
  CYC(b_+28, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+32, b_+33); mem_wr(gb, HL, B);
  CYC(b_+33, b_+34);
}

static void scaled_position_offset_for_velocity(GB *gb) {
  BASE(getScaledPositionOffsetForVelocity);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  CYC(b_+2, b_+5);
  position_offset_for_velocity(gb);
  CYC(b_+5, b_+8);
  scale_component(gb);
  CYC(b_+8, b_+9);
  L = alu_inc8(gb, L);
  scale_component(gb);
}

void getScaledPositionOffsetForVelocity_hook(GB *gb) {
  scaled_position_offset_for_velocity(gb);
  ret_effect(gb);
}

void objectSetComponentSpeedByScaledVelocity_hook(GB *gb) {
  BASE(objectSetComponentSpeedByScaledVelocity);
  CYC(b_+0, b_+3);
  scaled_position_offset_for_velocity(gb);
  CYC(b_+3, b_+5); A = H8(hActiveObjectType);
  alu_or(gb, 0x13);
  E = A;
  CYC(b_+5, b_+9); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(b_+10, b_+12); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(b_+13, b_+15); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(b_+16, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void objectSetPositionInCircleArc_hook(GB *gb) {
  BASE(objectSetPositionInCircleArc);
  uint16_t bc = BC;
  H = D;
  L = E;
  CYC(b_+0, b_+4); C = mem_rd(gb, HL);
  B = 0x28;
  CYC(b_+4, b_+9);
  scaled_position_offset_for_velocity(gb);
  SET_BC(bc);
  CYC(b_+9, b_+12); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_YH);
  E = A;
  CYC(b_+12, b_+18); A = mem_rd(gb, wTmpcec0 + 1);
  alu_add(gb, B);
  CYC(b_+18, b_+20); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(b_+20, b_+25); A = mem_rd(gb, wTmpcec0 + 3);
  alu_add(gb, C);
  CYC(b_+25, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28);
  ret_effect(gb);
}

// tile setters

void objectMakeTileSolid_hook(GB *gb) {
  BASE(objectMakeTileSolid);
  CYC(b_+0, b_+3);
  object_get_tile_collisions(gb);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0f);
  CYC(b_+5, b_+6);
  ret_effect(gb);
}

static void set_tile_without_gfx_reload(GB *gb) {
  BASE(setTileWithoutGfxReload);
  B = wRoomLayout >> 8;
  CYC(b_+0, b_+3); mem_wr(gb, BC, A);
  CYC(b_+3, b_+6);
  retrieve_tile_collision_value(gb);
  B = wRoomCollisions >> 8;
  CYC(b_+6, b_+9); mem_wr(gb, BC, A);
  CYC(b_+9, b_+10);
}

void setTileWithoutGfxReload_hook(GB *gb) {
  set_tile_without_gfx_reload(gb);
  ret_effect(gb);
}

static void set_tile_in_room_layout_buffer(GB *gb) {
  BASE(setTileInRoomLayoutBuffer);
  CYC(b_+0, b_+2); uint8_t svbk = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+7); mem_wr(gb, IO_SVBK, 0x03);
  A = B;
  B = w3RoomLayoutBuffer >> 8;
  CYC(b_+7, b_+11); mem_wr(gb, BC, A);
  A = svbk;
  CYC(b_+11, b_+14); mem_wr(gb, IO_SVBK, A);
  CYC(b_+14, b_+15);
}

void setTileInRoomLayoutBuffer_hook(GB *gb) {
  set_tile_in_room_layout_buffer(gb);
  ret_effect(gb);
}

// palette colour components (bank $17), 3 x 5-bit components per colour, $30 colours

static void extract_color_components(GB *gb) {
  BASE(extractColorComponents);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  A = GV(0x17, 0x16);
  CYC(b_+3, b_+7); H8(hRomBank) = A;
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, GV(0x17, 0x16));
  B = 0x30;
  CYC(b_+10, b_+12);
  do {
    CYC(b_+12, b_+13); C = mem_rd(gb, HL);
    SET_HL(HL + 1);
    CYC(b_+13, b_+15); A = mem_rd(gb, HL);
    C = alu_sla(gb, C); alu_rla(gb);
    C = alu_rl(gb, C); alu_rla(gb);
    C = alu_rl(gb, C); alu_rla(gb);
    alu_and(gb, 0x1f);
    CYC(b_+15, b_+27); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+27, b_+29); A = mem_rd(gb, HL); SET_HL(HL - 1);
    alu_rra(gb);
    alu_rra(gb);
    alu_and(gb, 0x1f);
    CYC(b_+29, b_+34); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+34, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_and(gb, 0x1f);
    CYC(b_+36, b_+39); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+39, b_+44); else CYC(b_+39, b_+44);
  } while (B);
  CYC(b_+44, b_+45); SET_AF(pop_effect(gb));
  CYC(b_+45, b_+47); H8(hRomBank) = A;
  CYC(b_+47, b_+50); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+50, b_+51);
}

void extractColorComponents_hook(GB *gb) {
  extract_color_components(gb);
  ret_effect(gb);
}

void enableIntroInputs_hook(GB *gb) {
  BASE(enableIntroInputs);
  CYC(b_+0, b_+2); A = H8(hIntroInputsEnabled);
  alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+5); ret_effect(gb); return; }
  CYC(b_+2, b_+5);
  A = 0x01;
  CYC(b_+5, b_+9); H8(hIntroInputsEnabled) = A;
  CYC(b_+9, b_+10);
  ret_effect(gb);
}

// chests (bank $16 table by group, then position/room/contents records ending with $ff)

void getChestData_hook(GB *gb) {
  BASE(getChestData);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  A = GV(0x16, 0x15);
  CYC(b_+3, b_+7); H8(hRomBank) = A;
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, GV(0x16, 0x15));
  CYC(b_+10, b_+13); A = W8(wActiveGroup);
  SET_HL(chestDataGroupTable_bank16);
  CYC(b_+13, b_+17);
  add_double_index_to_hl(gb);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+19, b_+23); A = W8(wActiveRoom);
  B = A;
  CYC(b_+23, b_+24);
  for (;;) {
    CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    A = alu_inc8(gb, A);
    if (F & FZ) { CYCT(b_+25, b_+29); SET_BC(0x2800); CYC(b_+42, b_+45); break; }
    CYC(b_+25, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_cp(gb, B);
    if (F & FZ) {
      CYCT(b_+30, b_+33);
      CYC(b_+37, b_+38); B = mem_rd(gb, HL);
      SET_HL(HL + 1);
      CYC(b_+38, b_+40); C = mem_rd(gb, HL);
      CYC(b_+40, b_+42);
      break;
    }
    CYC(b_+30, b_+37);
    SET_HL(HL + 2);
  }
  CYC(b_+45, b_+46); SET_AF(pop_effect(gb));
  CYC(b_+46, b_+48); H8(hRomBank) = A;
  CYC(b_+48, b_+51); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+51, b_+52);
  ret_effect(gb);
}

// respawn points

void setDeathRespawnPoint_hook(GB *gb) {
  BASE(setDeathRespawnPoint);
  const struct { uint16_t insn, src; } copies[] = {
    {b_+3, wActiveGroup}, {b_+7, wActiveRoom}, {b_+11, wRoomStateModifier},
    {b_+15, w1Link_direction}, {b_+19, w1Link_yh}, {b_+23, w1Link_xh},
    {b_+27, wRememberedCompanionId}, {b_+31, wRememberedCompanionGroup},
    {b_+35, wRememberedCompanionRoom}, {b_+39, wLinkObjectIndex},
    {b_+44, wRememberedCompanionY}, {b_+48, wRememberedCompanionX},
  };
  SET_HL(wDeathRespawnBuffer);
  CYC(b_+0, b_+3);
  for (int i = 0; i < 12; i++) {
    if (i == 10) { L = alu_inc8(gb, L); CYC(b_+43, b_+44); }
    CYC(copies[i].insn, copies[i].insn + 3); A = mem_rd(gb, copies[i].src);
    CYC(copies[i].insn + 3, copies[i].insn + 4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  }
  CYC(b_+52, b_+53);
  ret_effect(gb);
}

void updateLinkLocalRespawnPosition_hook(GB *gb) {
  BASE(updateLinkLocalRespawnPosition);
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  H = A;
  L = 0x08;
  CYC(b_+3, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+10); W8(wLinkLocalRespawnDir) = A;
  L = OBJ_YH;
  CYC(b_+10, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+16); W8(wLinkLocalRespawnY) = A;
  L = OBJ_XH;
  CYC(b_+16, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+22); W8(wLinkLocalRespawnX) = A;
  CYC(b_+22, b_+23);
  ret_effect(gb);
}

// bank switching around a call: ldh a,(hRomBank) / push af / ld a,n / ldh (hRomBank),a /
// ld ($2222),a ... pop af / ldh (hRomBank),a / ld ($2222),a

static void bank_push(GB *gb, uint16_t a, uint8_t bank) {
  CYC(a, a + 2); A = H8(hRomBank);
  CYC(a + 2, a + 3); push_effect(gb, AF);
  A = bank;
  CYC(a + 3, a + 7); H8(hRomBank) = A;
  CYC(a + 7, a + 10); mem_wr(gb, MBC_ROM_BANK, A);
}

static void bank_pop(GB *gb, uint16_t a) {
  CYC(a, a + 1); SET_AF(pop_effect(gb));
  CYC(a + 1, a + 3); H8(hRomBank) = A;
  CYC(a + 3, a + 6); mem_wr(gb, MBC_ROM_BANK, A);
}

// room flags for broken tiles and unlocked doors

static void set_room_flag(GB *gb) {
  BASE(setRoomFlagsForUnlockedKeyDoor);
  C = A;
  CYC(b_+32, b_+36); A = W8(wDungeonFlagsAddressH);
  B = A;
  CYC(b_+36, b_+38); A = mem_rd(gb, DE);
  L = A;
  CYC(b_+38, b_+40); A = mem_rd(gb, BC);
  alu_or(gb, L);
  CYC(b_+40, b_+42); mem_wr(gb, BC, A);
  CYC(b_+42, b_+43);
}

static void set_room_flags_for_unlocked_key_door(GB *gb) {
  BASE(setRoomFlagsForUnlockedKeyDoor);
  alu_and(gb, 0x0f);
  SET_DE(adjacentRoomsData);
  CYC(b_+0, b_+8);
  add_a_to_de(gb);
  CYC(b_+8, b_+11); A = W8(wDungeonIndex);
  alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+11, b_+15);
    CYC(b_+43, b_+46);
    this_room_flags(gb);
    CYC(b_+46, b_+47); A = mem_rd(gb, DE);
    CYC(b_+47, b_+48); alu_or(gb, mem_rd(gb, HL));
    CYC(b_+48, b_+49); mem_wr(gb, HL, A);
    CYC(b_+49, b_+50);
    return;
  }
  CYC(b_+11, b_+15);
  CALL_ROM(b_+15, ROM_getActiveRoomFromDungeonMapPosition);
  CYC(b_+18, b_+21);
  set_room_flag(gb);
  CYC(b_+21, b_+22); SET_DE(DE + 1);
  CYC(b_+22, b_+25); A = W8(wDungeonMapPosition);
  L = A;
  CYC(b_+25, b_+27); A = mem_rd(gb, DE);
  alu_add(gb, L);
  CYC(b_+27, b_+28);
  CALL_ROM(b_+28, ROM_getRoomInDungeon);
  CYC(b_+31, b_+32); SET_DE(DE + 1);
  set_room_flag(gb);
}

void setRoomFlagsForUnlockedKeyDoor_hook(GB *gb) {
  set_room_flags_for_unlocked_key_door(gb);
  ret_effect(gb);
}

static void set_room_flags_for_unlocked_key_door_overworld(GB *gb) {
  BASE(setRoomFlagsForUnlockedKeyDoor_overworldOnly);
  alu_and(gb, 0x0f);
  SET_HL(adjacentRoomsData);
  CYC(b_+0, b_+6);
  add_a_to_hl(gb);
  CYC(b_+6, b_+9); A = W8(wActiveRoom);
  C = A;
  B = GV(0xc7, 0xc8);
  CYC(b_+9, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+14); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+15); mem_wr(gb, BC, A);
  SET_HL(HL + 1);
  CYC(b_+15, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, C);
  C = A;
  CYC(b_+17, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+21); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+22); mem_wr(gb, BC, A);
  CYC(b_+22, b_+23);
}

void setRoomFlagsForUnlockedKeyDoor_overworldOnly_hook(GB *gb) {
  set_room_flags_for_unlocked_key_door_overworld(gb);
  ret_effect(gb);
}

void updateRoomFlagsForBrokenTile_hook(GB *gb) {
  BASE(updateRoomFlagsForBrokenTile);
  uint16_t af = AF;
  CYC(b_+0, b_+1);
  SET_HL(tileIncreaseGashaMaturityOnBreakTable);
  CYC(b_+1, b_+7);
  lookup_collision_table(gb);
  if (F & FC) { CYCT(b_+7, b_+10); push_effect(gb, b_+10); asm_call(gb, ROM_addToGashaMaturity, b_+10); }
  else CYC(b_+7, b_+10);
  SET_AF(af);
  CYC(b_+10, b_+11);
  SET_HL(tileUpdateRoomFlagsOnBreakTable);
  CYC(b_+11, b_+17);
  lookup_collision_table(gb);
  if (!(F & FC)) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+23); set_room_flags_for_unlocked_key_door(gb); ret_effect(gb); return; }
  CYC(b_+18, b_+23);
  alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+28); set_room_flags_for_unlocked_key_door_overworld(gb); ret_effect(gb); return; }
  CYC(b_+23, b_+28);
  alu_and(gb, 0x0f);
  SET_BC(bitTable);
  alu_add(gb, C);
  C = A;
  CYC(b_+28, b_+38); A = W8(wActiveGroup);
  SET_HL(flagLocationGroupTable);
  CYC(b_+38, b_+42);
  add_a_to_hl(gb);
  CYC(b_+42, b_+43); H = mem_rd(gb, HL);
  CYC(b_+43, b_+46); A = W8(wActiveRoom);
  L = A;
  CYC(b_+46, b_+48); A = mem_rd(gb, BC);
  CYC(b_+48, b_+49); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+51);
  ret_effect(gb);
}

void checkAndUpdateLinkOnChest_hook(GB *gb) {
  BASE(checkAndUpdateLinkOnChest);
  CYC(b_+0, b_+3); A = W8(wLinkOnChest);
  alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+6);
    C = A;
    CYC(b_+24, b_+28); A = W8(wActiveTilePos);
    alu_cp(gb, C);
    if (F & FZ) { CYCT(b_+28, b_+30); ret_effect(gb); return; }
    CYC(b_+28, b_+30);
    B = wRoomLayout >> 8;
    CYC(b_+30, b_+33); A = mem_rd(gb, BC);
    CYC(b_+33, b_+36);
    retrieve_tile_collision_value(gb);
    B = alu_dec8(gb, B);
    CYC(b_+36, b_+38); mem_wr(gb, BC, A);
    alu_xor(gb, A);
    CYC(b_+38, b_+42); W8(wLinkOnChest) = A;
    CYC(b_+42, b_+43);
    ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+9); A = W8(wActiveTileIndex);
  alu_cp(gb, 0xf1);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ret_effect(gb); return; }
  CYC(b_+9, b_+15); A = W8(wActiveTilePos);
  CYC(b_+15, b_+18); W8(wLinkOnChest) = A;
  L = A;
  H = wRoomCollisions >> 8;
  CYC(b_+18, b_+23); mem_wr(gb, HL, 0x00);
  CYC(b_+23, b_+24);
  ret_effect(gb);
}

void showInfoTextForRoller_hook(GB *gb) {
  BASE(showInfoTextForRoller);
  uint16_t sp0_ = gb->sp;
  bank_push(gb, b_+0, 0x06);
  A = 0x09;
  CYC(b_+10, b_+12);
  CALL_C(b_+12, showInfoTextForTile_hook, SYM(showInfoTextForTile), b_+15);
  bank_pop(gb, b_+15);
  CYC(b_+21, b_+22);
  ret_effect(gb);
}

// camera

void updateCamera_hook(GB *gb) {
  BASE(updateCamera);
  CYC(b_+0, b_+3); A = W8(wScreenVariables);
  alu_and(gb, 0x05);
  if (F & FZ) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  bank_push(gb, b_+6, 0x01);
  CALL_ROM(b_+16, ROM_b01_updateCameraPosition);
  CALL_ROM(b_+19, ROM_b01_updateGfxRegs2Scroll);
  CALL_ROM(b_+22, ROM_b01_updateScreenShake);
  bank_pop(gb, b_+25);
  CYC(b_+31, b_+32);
  ret_effect(gb);
}

void resetCamera_hook(GB *gb) {
  BASE(resetCamera);
  bank_push(gb, b_+0, 0x01);
  CALL_ROM(b_+10, ROM_b01_calculateCameraPosition);
  CALL_ROM(b_+13, ROM_b01_updateGfxRegs2Scroll);
  bank_pop(gb, b_+16);
  CYC(b_+22, b_+23);
  ret_effect(gb);
}

void setCameraFocusedObject_hook(GB *gb) {
  BASE(setCameraFocusedObject);
  CYC(b_+0, b_+2); A = H8(hActiveObject);
  CYC(b_+2, b_+5); W8(wCameraFocusedObject) = A;
  CYC(b_+5, b_+7); A = H8(hActiveObjectType);
  CYC(b_+7, b_+10); W8(wCameraFocusedObjectType) = A;
  CYC(b_+10, b_+11);
  ret_effect(gb);
}

void setCameraFocusedObjectToLink_hook(GB *gb) {
  BASE(setCameraFocusedObjectToLink);
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  CYC(b_+3, b_+6); W8(wCameraFocusedObject) = A;
  A = 0x00;
  CYC(b_+6, b_+11); W8(wCameraFocusedObjectType) = A;
  CYC(b_+11, b_+12);
  ret_effect(gb);
}

void reloadTileMap_hook(GB *gb) {
  BASE(reloadTileMap);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  alu_xor(gb, A);
  CYC(b_+3, b_+7); W8(wScreenOffsetY) = A;
  CYC(b_+7, b_+10); W8(wScreenOffsetX) = A;
  A = 0x10;
  CYC(b_+10, b_+12);
  CALL_ROM(b_+12, ROM_loadUncompressedGfxHeader);
  CYC(b_+15, b_+19); H8(hRomBank) = 0x01;
  CYC(b_+19, b_+22); mem_wr(gb, MBC_ROM_BANK, 0x01);
  CALL_ROM(b_+22, ROM_b01_setScreenTransitionState02);
  CALL_ROM(b_+25, ROM_b01_updateGfxRegs2Scroll);
  bank_pop(gb, b_+28);
  CYC(b_+34, b_+35);
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
  wave_scroll_call(gb, SYM(initWaveScrollValues), ROM_b01_initWaveScrollValues_body, false);
  ret_effect(gb);
}

void loadBigBufferScrollValues_hook(GB *gb) {
  wave_scroll_call(gb, SYM(loadBigBufferScrollValues), ROM_b01_loadBigBufferScrollValues_body, true);
  ret_effect(gb);
}

// room collisions from the layout and the bank 3 collision table

static void blank_collisions_horizontally(GB *gb) {
  BASE(loadRoomCollisions);
  A = 0xff;
  B = 0x10;
  CYC(b_+59, b_+63);
  do {
    CYC(b_+63, b_+64); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+64, b_+67); else CYC(b_+64, b_+67);
  } while (B);
  CYC(b_+67, b_+68);
}

static void blank_collisions_vertically(GB *gb) {
  BASE(loadRoomCollisions);
  B = 0x0b;
  C = 0xff;
  CYC(b_+68, b_+72);
  do {
    CYC(b_+72, b_+73); mem_wr(gb, HL, C);
    A = L;
    alu_add(gb, 0x10);
    L = A;
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+73, b_+80); else CYC(b_+73, b_+80);
  } while (B);
  CYC(b_+80, b_+81);
}

static void blank_data_around_collisions(GB *gb) {
  BASE(loadRoomCollisions);
  SET_HL(wRoomCollisions + 0xf0);
  CYC(b_+27, b_+33);
  blank_collisions_horizontally(gb);
  SET_HL(wRoomCollisions + 0x0f);
  CYC(b_+33, b_+39);
  blank_collisions_vertically(gb);
  CYC(b_+39, b_+42); A = W8(wActiveGroup);
  alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(b_+42, b_+46);
    L = 0x80;
    CYC(b_+50, b_+55);
    blank_collisions_horizontally(gb);
    L = 0x0a;
    CYC(b_+55, b_+59);
    blank_collisions_vertically(gb);
  } else {
    CYC(b_+42, b_+46);
    L = 0xb0;
    CYC(b_+46, b_+50);
    blank_collisions_horizontally(gb);
  }
}

static void load_room_collisions(GB *gb) {
  BASE(loadRoomCollisions);
  A = 0x03;
  CYC(b_+0, b_+4); mem_wr(gb, IO_SVBK, A);
  D = w3TileCollisions >> 8;
  SET_HL(wRoomLayout);
  B = 0xb0;
  CYC(b_+4, b_+11);
  do {
    CYC(b_+11, b_+12); A = mem_rd(gb, HL);
    E = A;
    CYC(b_+12, b_+14); A = mem_rd(gb, DE);
    H = alu_dec8(gb, H);
    CYC(b_+14, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
    H = alu_inc8(gb, H);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+16, b_+20); else CYC(b_+16, b_+20);
  } while (B);
  CYC(b_+20, b_+23);
  blank_data_around_collisions(gb);
  alu_xor(gb, A);
  CYC(b_+23, b_+26); mem_wr(gb, IO_SVBK, A);
  CYC(b_+26, b_+27);
}

void loadRoomCollisions_hook(GB *gb) {
  load_room_collisions(gb);
  ret_effect(gb);
}

// treasure and item drops (bank $3f bodies)

void decideItemDrop_hook(GB *gb) {
  BASE(decideItemDrop);
  uint16_t sp0_ = gb->sp;
  C = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, 0x3f);
  CALL_C(b_+11, decideItemDrop_body_hook, ROM_b3f_decideItemDrop_body, b_+14);
  bank_pop(gb, b_+14);
  A = C;
  alu_cp(gb, 0xff);
  CYC(b_+20, b_+24);
  ret_effect(gb);
}

void checkItemDropAvailable_hook(GB *gb) {
  BASE(checkItemDropAvailable);
  uint16_t sp0_ = gb->sp;
  C = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, 0x3f);
  A = C;
  CYC(b_+11, b_+12);
  CALL_C(b_+12, checkItemDropAvailable_body_hook,
      ROM_b3f_checkItemDropAvailable_body, b_+15);
  bank_pop(gb, b_+15);
  A = C;
  alu_cp(gb, 0xff);
  CYC(b_+21, b_+25);
  ret_effect(gb);
}

static void give_treasure(GB *gb, uint16_t sp0_) {
  BASE(giveTreasure);
  B = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, 0x3f);
  CALL_C(b_+11, giveTreasure_body_hook, ROM_b3f_giveTreasure_body, b_+14);
  bank_pop(gb, b_+14);
  A = B;
  alu_or(gb, A);
  CYC(b_+20, b_+23);
}

void giveTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  give_treasure(gb, sp0_);
  ret_effect(gb);
}

void loseTreasure_hook(GB *gb) {
  BASE(loseTreasure);
  uint16_t sp0_ = gb->sp;
  B = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, 0x3f);
  CALL_C(b_+11, loseTreasure_body_hook, ROM_b3f_loseTreasure_body, b_+14);
  bank_pop(gb, b_+14);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

void removeRupeeValue_hook(GB *gb) {
  BASE(removeRupeeValue);
  SET_HL(wNumRupees);
  CYC(b_+0, b_+6);
  get_rupee_value(gb);
  CYC(b_+6, b_+9);
  sub_decimal_from_hl_ref(gb);
  ret_effect(gb);
}

void getRandomRingOfGivenTier_hook(GB *gb) {
  BASE(getRandomRingOfGivenTier);
  bank_push(gb, b_+0, 0x3f);
  B = 0x01;
  A = C;
  alu_cp(gb, 0x04);
  if (F & FZ) CYCT(b_+10, b_+17);
  else { CYC(b_+10, b_+19); B = 0x07; }
  SET_HL(ringTierTable_bank3f);
  CYC(b_+19, b_+23);
  add_double_index_to_hl(gb);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+25); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+25, b_+29);
  A = getRandomNumber(gb);
  alu_and(gb, B);
  C = A;
  B = 0x00;
  alu_add_hl(gb, BC);
  CYC(b_+29, b_+34);
  CYC(b_+34, b_+35); C = mem_rd(gb, HL);
  bank_pop(gb, b_+35);
  A = 0x2d;
  CYC(b_+41, b_+44);
  ret_effect(gb);
}

void refillSeedSatchel_hook(GB *gb) {
  BASE(refillSeedSatchel);
  uint16_t sp0_ = gb->sp;
  E = 0x20;
  CYC(b_+0, b_+2);
  for (;;) {
    A = E;
    CYC(b_+2, b_+3);
    CALL_C(b_+3, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+6);
    if (!(F & FC)) CYCT(b_+6, b_+8);
    else {
      CYC(b_+6, b_+8);
      A = E;
      C = 0x99;
      CYC(b_+8, b_+14);
      give_treasure(gb, sp0_);
    }
    E = alu_inc8(gb, E);
    A = E;
    alu_cp(gb, 0x25);
    if (F & FC) { CYCT(b_+14, b_+20); continue; }
    CYC(b_+14, b_+20);
    break;
  }
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

static void add_to_gasha_maturity(GB *gb) {
  BASE(addToGashaMaturity);
  uint16_t hl = HL;
  SET_HL(wGashaMaturity);
  CYC(b_+0, b_+5); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  if (!(F & FC)) CYCT(b_+6, b_+8);
  else {
    CYC(b_+6, b_+9); uint8_t v = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
    if (v) CYCT(b_+9, b_+11);
    else {
      CYC(b_+9, b_+14); A = 0xff; mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(b_+14, b_+15); mem_wr(gb, HL, A);
    }
  }
  SET_HL(hl);
  CYC(b_+15, b_+17);
}

void addToGashaMaturity_hook(GB *gb) {
  add_to_gasha_maturity(gb);
  ret_effect(gb);
}

// room-specific data: table of per-group pointers to (room, value) lists

void findRoomSpecificData_hook(GB *gb) {
  BASE(findRoomSpecificData);
  E = A;
  CYC(b_+0, b_+4); A = W8(wActiveGroup);
  CYC(b_+4, b_+5);
  add_double_index_to_hl(gb);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+7, b_+8);
  lookup_key(gb);
  ret_effect(gb);
}

// object deletion and replacement

static void remove_from_abutton_list(GB *gb);

static void object_delete_de(GB *gb) {
  BASE(objectDelete_de);
  A = E;
  alu_and(gb, 0xc0);
  E = A;
  L = A;
  H = D;
  B = 0x10;
  alu_xor(gb, A);
  CYC(b_+0, b_+9);
  do {
    CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+13, b_+16); else CYC(b_+13, b_+16);
  } while (B);
  CYC(b_+16, b_+19);
  remove_from_abutton_list(gb);
}

void objectDelete_de_hook(GB *gb) { object_delete_de(gb); ret_effect(gb); }

static void object_delete_use_active_type(GB *gb) {
  BASE(objectDelete_useActiveObjectType);
  CYC(b_+0, b_+2); E = H8(hActiveObjectType);
  CYC(b_+2, b_+3);
  object_delete_de(gb);
}

void objectDelete_useActiveObjectType_hook(GB *gb) { object_delete_use_active_type(gb); ret_effect(gb); }

void objectReplaceWithID_hook(GB *gb) {
  BASE(objectReplaceWithID);
  uint16_t bc0 = BC;
  H = D;
  CYC(b_+0, b_+4); A = H8(hActiveObjectType);
  L = A;
  CYC(b_+4, b_+6); B = mem_rd(gb, HL);
  alu_add(gb, OBJ_YH);
  L = A;
  CYC(b_+6, b_+10); C = mem_rd(gb, HL);
  uint16_t bc1 = BC;
  L = alu_inc8(gb, L); L = alu_inc8(gb, L);
  CYC(b_+10, b_+14); B = mem_rd(gb, HL);
  L = alu_inc8(gb, L); L = alu_inc8(gb, L);
  CYC(b_+14, b_+17); C = mem_rd(gb, HL);
  uint16_t bc2 = BC;
  CYC(b_+17, b_+21);
  object_delete_use_active_type(gb);
  SET_BC(bc2);
  H = D;
  A = H8(hActiveObjectType);
  alu_add(gb, OBJ_ZH);
  L = A;
  CYC(b_+21, b_+29); mem_wr(gb, HL, C);
  L = alu_dec8(gb, L); L = alu_dec8(gb, L);
  CYC(b_+29, b_+32); mem_wr(gb, HL, B);
  SET_BC(bc1);
  L = alu_dec8(gb, L); L = alu_dec8(gb, L);
  CYC(b_+32, b_+36); mem_wr(gb, HL, C);
  A = H8(hActiveObjectType);
  L = A;
  A = B;
  alu_and(gb, 0x03);
  CYC(b_+36, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  SET_BC(bc0);
  CYC(b_+43, b_+45); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+45, b_+47); mem_wr(gb, HL, C);
  CYC(b_+47, b_+48);
  ret_effect(gb);
}

void checkInteractionSubstate_hook(GB *gb) {
  BASE(checkInteractionSubstate);
  E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  alu_or(gb, A);
  CYC(b_+3, b_+5);
  ret_effect(gb);
}

static void set_collide_radii(GB *gb) {
  BASE(objectSetCollideRadii);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_COLLIDE_RADIUS_Y);
  L = A;
  H = D;
  CYC(b_+2, b_+7); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, C);
  CYC(b_+9, b_+10);
}

void objectSetCollideRadii_hook(GB *gb) {
  set_collide_radii(gb);
  ret_effect(gb);
}

void objectSetCollideRadius_hook(GB *gb) {
  BASE(objectSetCollideRadius);
  uint16_t bc = BC;
  B = A;
  C = A;
  CYC(b_+0, b_+6);
  set_collide_radii(gb);
  SET_BC(bc);
  CYC(b_+6, b_+8);
  ret_effect(gb);
}

static void dec_num_enemies(GB *gb) {
  BASE(decNumEnemies);
  SET_HL(wNumEnemies);
  CYC(b_+0, b_+4); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); return; }
  CYC(b_+4, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+7, b_+8);
}

void decNumEnemies_hook(GB *gb) {
  dec_num_enemies(gb);
  ret_effect(gb);
}

void setScreenShakeCounter_hook(GB *gb) {
  BASE(setScreenShakeCounter);
  SET_HL(wScreenShakeCounterY);
  CYC(b_+0, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+6);
  ret_effect(gb);
}

static void object_create_interaction(GB *gb) {
  BASE(objectCreateInteraction);
  CALL_ROM(b_+0, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); return; }
  CYC(b_+3, b_+5); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+5, b_+7); mem_wr(gb, HL, C);
  CYC(b_+7, b_+10);
  object_copy_position(gb);
  alu_xor(gb, A);
  CYC(b_+10, b_+12);
}

void objectCreateInteraction_hook(GB *gb) {
  object_create_interaction(gb);
  ret_effect(gb);
}

void objectCreateInteractionWithSubid00_hook(GB *gb) {
  BASE(objectCreateInteractionWithSubid00);
  C = 0x00;
  CYC(b_+0, b_+2);
  object_create_interaction(gb);
  ret_effect(gb);
}

void objectCreatePuff_hook(GB *gb) {
  BASE(objectCreatePuff);
  B = 0x05;
  CYC(b_+0, b_+2);
  C = 0x00;
  CYC(SYM(objectCreateInteractionWithSubid00), (SYM(objectCreateInteractionWithSubid00) + 2));
  object_create_interaction(gb);
  ret_effect(gb);
}

static void object_create_falling_down_hole(GB *gb) {
  BASE(objectCreateFallingDownHoleInteraction);
  CALL_ROM(b_+0, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); return; }
  CYC(b_+3, b_+6); mem_wr(gb, HL, 0x0f);
  L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+6, b_+10); A = H8(hActiveObjectType);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_add(gb, 0x01);
  E = A;
  CYC(b_+11, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+19);
  object_copy_position(gb);
  alu_xor(gb, A);
  CYC(b_+19, b_+21);
}

void objectCreateFallingDownHoleInteraction_hook(GB *gb) {
  object_create_falling_down_hole(gb);
  ret_effect(gb);
}

void objectFlickerVisibility_hook(GB *gb) {
  BASE(objectFlickerVisibility);
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  alu_and(gb, B);
  if (F & FZ) { CYCT(b_+3, b_+7); obj_visible_bit(gb, SYM(objectSetInvisible), false); }
  else { CYC(b_+3, b_+7); CYC(b_+7, b_+10); obj_visible_bit(gb, SYM(objectSetVisible), true); }
  ret_effect(gb);
}

static void unmark_solid_position(GB *gb) {
  BASE(objectUnmarkSolidPosition);
  CYC(b_+0, b_+3);
  object_get_short_position(gb);
  B = A;
  A = 0x02;
  CYC(b_+3, b_+8); mem_wr(gb, IO_SVBK, A);
  A = B;
  SET_HL(w2SolidObjectPositions);
  CYC(b_+8, b_+15);
  unset_flag(gb);
  A = 0x00;
  CYC(b_+15, b_+19); mem_wr(gb, IO_SVBK, A);
  CYC(b_+19, b_+20);
}

void objectUnmarkSolidPosition_hook(GB *gb) {
  unmark_solid_position(gb);
  ret_effect(gb);
}

// interaction text and scripts

void interactionSetHighTextIndex_hook(GB *gb) {
  BASE(interactionSetHighTextIndex);
  E = 0x73;
  CYC(b_+0, b_+3); mem_wr(gb, DE, A);
  E = 0x70;
  A |= 0x80;
  CYC(b_+3, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9);
  ret_effect(gb);
}

void interactionSetScript_hook(GB *gb) {
  BASE(interactionSetScript);
  E = 0x58;
  A = L;
  CYC(b_+0, b_+4); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+4, b_+7); mem_wr(gb, DE, A);
  H = D;
  L = INTERACTION_BASE + OBJ_COUNTER1;
  alu_xor(gb, A);
  CYC(b_+7, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14);
  ret_effect(gb);
}

// mimic a background tile: oam data from the tile mapping, bg palette copied to spr palette 6

void objectMimicBgTile_hook(GB *gb) {
  BASE(objectMimicBgTile);
  CALL_ROM(b_+0, ROM_getTileMappingData);
  H = D;
  CYC(b_+3, b_+6); A = H8(hActiveObjectType);
  alu_add(gb, 0x1b);
  L = A;
  A = 0x0e;
  CYC(b_+6, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, C);
  A = B;
  alu_and(gb, 0x07);
  alu_swap_a(gb);
  alu_rrca(gb);
  SET_BC(w2TilesetBgPalettes);
  CYC(b_+14, b_+26);
  add_a_to_bc(gb);
  CYC(b_+26, b_+28); uint8_t svbk = mem_rd(gb, IO_SVBK), f = F;
  CYC(b_+28, b_+33); mem_wr(gb, IO_SVBK, 0x02);
  SET_HL(w2TilesetBgPalettes + 0x70);
  E = 0x08;
  CYC(b_+33, b_+38);
  do {
    CYC(b_+38, b_+39); A = mem_rd(gb, BC);
    CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
    C = alu_inc8(gb, C);
    E = alu_dec8(gb, E);
    if (E) CYCT(b_+40, b_+44); else CYC(b_+40, b_+44);
  } while (E);
  SET_HL(hDirtySprPalettes);
  CYC(b_+44, b_+49); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  A = svbk;
  F = f;
  CYC(b_+49, b_+52); mem_wr(gb, IO_SVBK, A);
  CYC(b_+52, b_+53);
  ret_effect(gb);
}

// bank copies

void copy8BytesFromRingMapToCec0_hook(GB *gb) {
  BASE(copy8BytesFromRingMapToCec0);
  bank_push(gb, b_+0, 0x1c);
  SET_DE(wTmpcec0);
  B = 0x08;
  CYC(b_+10, b_+18);
  copyMemory(gb, DE, HL, 8);
  SET_HL(HL + 8);
  SET_DE(DE + 8);
  B = 0;
  bank_pop(gb, b_+18);
  CYC(b_+24, b_+25);
  ret_effect(gb);
}

void copy20BytesFromBank_hook(GB *gb) {
  BASE(copy20BytesFromBank);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  A = B;
  CYC(b_+3, b_+6); H8(hRomBank) = A;
  CYC(b_+6, b_+9); mem_wr(gb, MBC_ROM_BANK, A);
  B = 0x20;
  CYC(b_+9, b_+14);
  copyMemory(gb, DE, HL, 0x20);
  SET_HL(HL + 0x20);
  SET_DE(DE + 0x20);
  B = 0;
  CYC(b_+14, b_+15); SET_AF(pop_effect(gb));
  CYC(b_+15, b_+17); H8(hRomBank) = A;
  CYC(b_+17, b_+20); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

// A-button sensitive object list: (object high byte, variable low byte) pairs, $ccb3-$ccd2

void objectAddToAButtonSensitiveObjectList_hook(GB *gb) {
  BASE(objectAddToAButtonSensitiveObjectList);
  alu_xor(gb, A);
  CYC(b_+0, b_+2); mem_wr(gb, DE, A);
  SET_HL(wAButtonSensitiveObjectList);
  CYC(b_+2, b_+5);
  for (;;) {
    CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+7, b_+9);
      A = E;
      CYC(b_+16, b_+18); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(b_+18, b_+19); mem_wr(gb, HL, D);
      alu_scf(gb);
      CYC(b_+19, b_+21);
      break;
    }
    CYC(b_+7, b_+9);
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, GV(0xd3, 0xea));
    if (F & FC) { CYCT(b_+9, b_+15); continue; }
    CYC(b_+9, b_+16);
    break;
  }
  ret_effect(gb);
}

static void remove_from_abutton_list(GB *gb) {
  BASE(objectRemoveFromAButtonSensitiveObjectList);
  uint16_t de = DE;
  A = E;
  alu_and(gb, 0xc0);
  E = A;
  SET_HL(wAButtonSensitiveObjectList);
  CYC(b_+0, b_+8);
  for (;;) {
    CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_cp(gb, D);
    if (!(F & FZ)) CYCT(b_+9, b_+12);
    else {
      CYC(b_+9, b_+13); A = mem_rd(gb, HL);
      alu_and(gb, 0xc0);
      alu_sub(gb, E);
      if (!(F & FZ)) CYCT(b_+13, b_+18);
      else {
        CYC(b_+13, b_+19); mem_wr(gb, HL, A); SET_HL(HL - 1);
        CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
      }
    }
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, GV(0xd3, 0xea));
    if (F & FC) { CYCT(b_+20, b_+26); continue; }
    CYC(b_+20, b_+28);
    break;
  }
  SET_DE(de);
}

void objectRemoveFromAButtonSensitiveObjectList_hook(GB *gb) {
  remove_from_abutton_list(gb);
  ret_effect(gb);
}

void checkEnemyAndPartCollisionsIfTextInactive_hook(GB *gb) {
  BASE(retIfTextIsActive);
  uint16_t sp0_ = gb->sp;
  CYC(SYM(checkEnemyAndPartCollisionsIfTextInactive), (SYM(checkEnemyAndPartCollisionsIfTextInactive) + 3));
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(b_+3, b_+5); SET_AF(0x1dea); CYC(b_+5, b_+7); ret_effect(gb); return; }
  CYCT(b_+3, b_+5);
  bank_push(gb, (SYM(checkEnemyAndPartCollisionsIfTextInactive) + 3), 0x07);
  CALL_C((SYM(checkEnemyAndPartCollisionsIfTextInactive) + 13), checkEnemyAndPartCollisions_hook,
         ROM_b07_checkEnemyAndPartCollisions, (SYM(checkEnemyAndPartCollisionsIfTextInactive) + 16));
  bank_pop(gb, (SYM(checkEnemyAndPartCollisionsIfTextInactive) + 16));
  CYC((SYM(checkEnemyAndPartCollisionsIfTextInactive) + 22), (SYM(checkEnemyAndPartCollisionsIfTextInactive) + 23));
  ret_effect(gb);
}

// replace an object with a hazard animation and delete it

static void delete_and_scf(GB *gb) {
  BASE(objectReplaceWithSplash);
  CYC(b_+5, b_+8);
  object_delete_use_active_type(gb);
  alu_scf(gb);
  CYC(b_+8, b_+10);
}

static void replace_with_interaction_b(GB *gb) {
  BASE(objectReplaceWithSplash);
  CYC(b_+2, b_+5);
  C = 0x00;
  CYC(SYM(objectCreateInteractionWithSubid00), (SYM(objectCreateInteractionWithSubid00) + 2));
  object_create_interaction(gb);
  delete_and_scf(gb);
}

static void replace_with_splash(GB *gb) {
  BASE(objectReplaceWithSplash);
  B = 0x03;
  CYC(b_+0, b_+2);
  replace_with_interaction_b(gb);
}

static void replace_with_falling_down_hole(GB *gb) {
  BASE(objectReplaceWithFallingDownHoleInteraction);
  CYC(b_+0, b_+3);
  object_create_falling_down_hole(gb);
  CYC(b_+3, b_+5);
  delete_and_scf(gb);
}

void objectReplaceWithSplash_hook(GB *gb) { replace_with_splash(gb); ret_effect(gb); }
void objectReplaceWithFallingDownHoleInteraction_hook(GB *gb) { replace_with_falling_down_hole(gb); ret_effect(gb); }

void objectReplaceWithAnimationIfOnHazard_hook(GB *gb) {
  BASE(objectReplaceWithAnimationIfOnHazard);
  CYC(b_+0, b_+3);
  object_check_is_on_hazard(gb);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  alu_rrca(gb);
  if (F & FC) { CYCT(b_+4, b_+7); replace_with_splash(gb); ret_effect(gb); return; }
  CYC(b_+4, b_+7);
  alu_rrca(gb);
  if (F & FC) { CYCT(b_+7, b_+10); replace_with_falling_down_hole(gb); ret_effect(gb); return; }
  CYC(b_+7, b_+10);
  B = 0x04;
  CYC(b_+10, b_+14);
  replace_with_interaction_b(gb);
  ret_effect(gb);
}

void breakCrackedFloor_hook(GB *gb) {
  BASE(breakCrackedFloor);
  uint16_t bc = BC;
  CYC(b_+0, b_+1);
  CALL_ROM(b_+1, ROM_setTile);
  SET_BC(bc);
  A = 0xb3;
  CYC(b_+4, b_+7);
  CALL_ROM(b_+7, ROM_playSound);
  CALL_ROM(b_+10, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+16); mem_wr(gb, HL, 0x0f);
  L = alu_inc8(gb, L);
  CYC(b_+16, b_+19); mem_wr(gb, HL, 0x80);
  L = 0x4b;
  CYC(b_+19, b_+24);
  set_short_pos_param_c(gb);
  ret_effect(gb);
}

// Link pushed by moving objects (bank 5 body)

static void update_link_position_given_velocity(GB *gb, uint16_t sp0_) {
  BASE(updateLinkPositionGivenVelocity);
  bank_push(gb, b_+0, 0x05);
  uint16_t de = DE;
  CYC(b_+10, b_+14); A = W8(wLinkObjectIndex);
  D = A;
  E = 0x00;
  CYC(b_+14, b_+17);
  CALL_C(b_+17, specialObjectUpdatePositionGivenVelocity_hook,
         ROM_b05_specialObjectUpdatePositionGivenVelocity, b_+20);
  SET_DE(de);
  CYC(b_+20, b_+21);
  bank_pop(gb, b_+21);
  alu_scf(gb);
  CYC(b_+27, b_+29);
}

void updateLinkPositionGivenVelocity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  update_link_position_given_velocity(gb, sp0_);
  ret_effect(gb);
}

void objectPushLinkAwayOnCollision_hook(GB *gb) {
  BASE(objectPushLinkAwayOnCollision);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  H = A;
  L = 0x00;
  CYC(b_+3, b_+9);
  objects_collided(gb);
  if (!(F & FC)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13);
  push_effect(gb, b_+13);
  angle_toward_enemy_target(gb);
  C = A;
  B = 0x28;
  CYC(b_+13, b_+16);
  update_link_position_given_velocity(gb, sp0_);
  ret_effect(gb);
}

// interaction scripts

void interactionRunScript_hook(GB *gb) {
  BASE(interactionRunScript);
  CYC(b_+0, b_+3); A = W8(wLinkDeathTrigger);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+8); A = W8(wTextIsActive);
  alu_add(gb, A);
  if (F & FC) CYCT(b_+8, b_+11);
  else if (!(F & FZ)) { CYC(b_+8, b_+11); CYCT(b_+11, b_+12); ret_effect(gb); return; }
  else CYC(b_+8, b_+12);
  H = D;
  L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+16); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) CYCT(b_+16, b_+19);
  else {
    CYC(b_+16, b_+20); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
    if (v) { CYCT(b_+20, b_+21); ret_effect(gb); return; }
    CYC(b_+20, b_+21);
  }
  L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+21, b_+24); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) CYCT(b_+24, b_+27);
  else {
    CYC(b_+24, b_+28); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
    if (v) { CYCT(b_+28, b_+31); object_apply_speed(gb); }
    else CYC(b_+28, b_+31);
    alu_xor(gb, A);
    CYC(b_+31, b_+33);
    ret_effect(gb);
    return;
  }
  H = D;
  L = 0x58;
  CYC(b_+33, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+37, b_+38); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+38, b_+39);
  CALL_ROM(b_+39, ROM__interactionActuallyRunScript);
  if (F & FC) {
    CYCT(b_+42, b_+44);
    CALL_ROM(b_+49, ROM__interactionSaveScriptAddress);
    alu_scf(gb);
    CYC(b_+52, b_+54);
    ret_effect(gb);
    return;
  }
  CYC(b_+42, b_+44);
  CALL_ROM(b_+44, ROM__interactionSaveScriptAddress);
  alu_xor(gb, A);
  CYC(b_+47, b_+49);
  ret_effect(gb);
}

// keep object h (Link) from passing through object d

static void set_both_object_variables(GB *gb, uint16_t call_addr) {
  BASE(preventObjectHFromPassingObjectD);
  CYC(call_addr, call_addr + 3);
  CYC(b_+95, b_+97); A = H8(hActiveObjectType);
  alu_or(gb, B);
  E = A;
  CYC(b_+97, b_+101); A = H8(hFF8B);
  alu_or(gb, B);
  L = A;
  CYC(b_+101, b_+104);
}

static void check_collision_direction(GB *gb) {
  BASE(preventObjectHFromPassingObjectD);
  CYC(b_+9, b_+12);
  B = OBJ_YH;
  CYC(b_+44, b_+46);
  set_both_object_variables(gb, b_+46);
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(b_+51, b_+53);
  else { CYC(b_+51, b_+55); alu_cpl(gb); A = alu_inc8(gb, A); }
  C = A;
  B = OBJ_COLLIDE_RADIUS_Y;
  CYC(b_+55, b_+58);
  set_both_object_variables(gb, b_+58);
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+63, b_+65); H8(hFF8D) = A;
  alu_sub(gb, C);
  CYC(b_+65, b_+68); H8(hFF8F) = A;
  B = OBJ_XH;
  CYC(b_+68, b_+70);
  set_both_object_variables(gb, b_+70);
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYCT(b_+75, b_+77);
  else { CYC(b_+75, b_+79); alu_cpl(gb); A = alu_inc8(gb, A); }
  C = A;
  B = OBJ_COLLIDE_RADIUS_Y + 1;
  CYC(b_+79, b_+82);
  set_both_object_variables(gb, b_+82);
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+87, b_+89); H8(hFF8C) = A;
  alu_sub(gb, C);
  B = A;
  CYC(b_+89, b_+93); A = H8(hFF8F);
  alu_cp(gb, B);
  CYC(b_+93, b_+95);
}

static void prevent_h_from_passing_d(GB *gb) {
  BASE(preventObjectHFromPassingObjectD);
  A = L;
  alu_and(gb, 0xc0);
  CYC(b_+0, b_+5); H8(hFF8B) = A;
  CYC(b_+5, b_+8);
  objects_collided(gb);
  if (!(F & FC)) { CYCT(b_+8, b_+9); return; }
  CYC(b_+8, b_+9);
  check_collision_direction(gb);
  if (!(F & FC)) {
    CYCT(b_+12, b_+14);
    B = OBJ_XH;
    CYC(b_+21, b_+25); A = H8(hFF8C);
    C = A;
    CYC(b_+25, b_+28);
  } else {
    CYC(b_+12, b_+14);
    B = OBJ_YH;
    CYC(b_+14, b_+18); A = H8(hFF8D);
    C = A;
    CYC(b_+18, b_+21);
  }
  set_both_object_variables(gb, b_+28);
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_sub(gb, mem_rd(gb, HL));
  A = C;
  if (F & FC) CYCT(b_+33, b_+36);
  else { CYC(b_+33, b_+38); alu_cpl(gb); A = alu_inc8(gb, A); }
  B = A;
  CYC(b_+38, b_+40); A = mem_rd(gb, DE);
  alu_add(gb, B);
  CYC(b_+40, b_+42); mem_wr(gb, HL, A);
  alu_scf(gb);
  CYC(b_+42, b_+44);
}

void preventObjectHFromPassingObjectD_hook(GB *gb) {
  prevent_h_from_passing_d(gb);
  ret_effect(gb);
}

static void interaction_delete(GB *gb);

// script helpers

static void script_jump(GB *gb) {
  BASE(scriptFunc_jump);
  A = H;
  alu_cp(gb, 0x80);
  if (F & FC) CYCT(b_+0, b_+5);
  else {
    CYC(b_+0, b_+7); C = H8(hScriptAddressL);
    CYC(b_+7, b_+10); B = H8(hScriptAddressH);
    CYC(b_+10, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_sub(gb, C);
    E = A;
    CYC(b_+12, b_+15); A = mem_rd(gb, HL); SET_HL(HL - 1);
    alu_sbc(gb, B);
    alu_or(gb, A);
    if (!(F & FZ)) CYCT(b_+15, b_+19);
    else { L = E; H = wBigBuffer >> 8; CYC(b_+15, b_+23); return; }
  }
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+25); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+25, b_+28); A = H8(hActiveObject);
  D = A;
  alu_xor(gb, A);
  CYC(b_+28, b_+31);
}

void scriptFunc_jump_hook(GB *gb) {
  script_jump(gb);
  ret_effect(gb);
}

void scriptFunc_jump_scf_hook(GB *gb) {
  BASE(scriptFunc_jump_scf);
  CYC(b_+0, b_+3);
  script_jump(gb);
  alu_scf(gb);
  CYC(b_+3, b_+5);
  ret_effect(gb);
}

void scriptFunc_add3ToHl_hook(GB *gb) {
  BASE(scriptFunc_add3ToHl);
  SET_HL(HL + 3);
  CYC(b_+0, b_+4);
  ret_effect(gb);
}

void scriptFunc_add3ToHl_scf_hook(GB *gb) {
  BASE(scriptFunc_add3ToHl_scf);
  alu_scf(gb);
  CYC(b_+0, b_+1);
  SET_HL(HL + 3);
  CYC(SYM(scriptFunc_add3ToHl), (SYM(scriptFunc_add3ToHl) + 4));
  ret_effect(gb);
}

// interaction animation. _interactionNextAnimationFrame stays transliterated and pops the
// return address itself, so these return whether that happened.

static void next_animation_frame(GB *gb, uint16_t o, uint8_t counter, uint8_t id, uint16_t table, uint8_t oam);

static void interaction_animate(GB *gb) {
  BASE(interactionAnimate);
  H = D;
  L = 0x60;
  CYC(b_+0, b_+4); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(b_+4, b_+5); return; }
  CYC(b_+4, b_+7); A = H8(hRomBank);
  CYC(b_+7, b_+8); push_effect(gb, AF);
  CYC(b_+8, b_+12); H8(hRomBank) = GV(0x16, 0x14);
  CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, GV(0x16, 0x14));
  L = 0x62;
  CYC(b_+15, b_+19);
  next_animation_frame(gb, SYM(_interactionNextAnimationFrame), 0x60, 0x41, interactionAnimationFrameTable_bank16, 0x5e);
}

static void call_interaction_animate(GB *gb, uint16_t a) {
  CYC(a, a + 3);
  interaction_animate(gb);
}

void interactionAnimate_hook(GB *gb) {
  interaction_animate(gb);
  ret_effect(gb);
}

static void interaction_set_animation(GB *gb) {
  BASE(interactionSetAnimation);
  alu_add(gb, A);
  C = A;
  B = 0x00;
  CYC(b_+0, b_+6); A = H8(hRomBank);
  CYC(b_+6, b_+7); push_effect(gb, AF);
  CYC(b_+7, b_+11); H8(hRomBank) = GV(0x16, 0x14);
  CYC(b_+11, b_+14); mem_wr(gb, MBC_ROM_BANK, GV(0x16, 0x14));
  E = INTERACTION_BASE + 0x01;
  CYC(b_+14, b_+17); A = mem_rd(gb, DE);
  SET_HL(interactionAnimationTable_bank16);
  CYC(b_+17, b_+21);
  add_double_index_to_hl(gb);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  CYC(b_+23, b_+25);
  next_animation_frame(gb, SYM(_interactionNextAnimationFrame), 0x60, 0x41, interactionAnimationFrameTable_bank16, 0x5e);
}

void interactionSetAnimation_hook(GB *gb) { interaction_set_animation(gb); ret_effect(gb); }

static void interaction_animate_2_times(GB *gb) {
  BASE(interactionAnimate2Times);
  call_interaction_animate(gb, b_+0);
  CYC(b_+3, b_+6);
  interaction_animate(gb);
}

static void interaction_animate_3_times(GB *gb) {
  call_interaction_animate(gb, SYM(interactionAnimate3Times));
  interaction_animate_2_times(gb);
}

void interactionAnimate2Times_hook(GB *gb) { interaction_animate_2_times(gb); ret_effect(gb); }
void interactionAnimate3Times_hook(GB *gb) { interaction_animate_3_times(gb); ret_effect(gb); }
void interactionAnimate4Times_hook(GB *gb) {
  call_interaction_animate(gb, SYM(interactionAnimate4Times));
  interaction_animate_3_times(gb);
  ret_effect(gb);
}

void interactionAnimateBasedOnSpeed_hook(GB *gb) {
  BASE(interactionAnimateBasedOnSpeed);
  call_interaction_animate(gb, b_+0);
  E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+3, b_+6); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); ret_effect(gb); return; }
  CYC(b_+6, b_+8);
  E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+8, b_+11); A = mem_rd(gb, DE);
  alu_cp(gb, 0x28);
  if (F & FC) { CYCT(b_+11, b_+14); ret_effect(gb); return; }
  CYC(b_+11, b_+14);
  alu_cp(gb, 0x50);
  if (F & FC) { CYCT(b_+14, b_+19); interaction_animate(gb); ret_effect(gb); return; }
  CYC(b_+14, b_+19);
  alu_cp(gb, 0x78);
  if (F & FC) { CYCT(b_+19, b_+24); interaction_animate_2_times(gb); ret_effect(gb); return; }
  CYC(b_+19, b_+24);
  CYC(b_+24, b_+27);
  interaction_animate_3_times(gb);
  ret_effect(gb);
}

// npc behaviour

static void prevent_link_from_passing(GB *gb) {
  BASE(objectPreventLinkFromPassing);
  CYC(b_+0, b_+3); A = W8(wLinkCanPassNpcs);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); return; }
  CYC(b_+3, b_+5);
  L = A;
  CYC(b_+5, b_+9); A = W8(wLinkObjectIndex);
  H = A;
  CYC(b_+9, b_+13);
  prevent_h_from_passing_d(gb);
  uint16_t af = AF;
  SET_HL(w1Companion_id);
  CYC(b_+13, b_+18); A = mem_rd(gb, HL);
  alu_cp(gb, 0x0c);
  if (!(F & FZ)) CYCT(b_+18, b_+22);
  else {
    L = OBJ_STATE;
    CYC(b_+18, b_+25); A = mem_rd(gb, HL);
    alu_cp(gb, 0x02);
    if (!(F & FZ)) CYCT(b_+25, b_+29);
    else {
      CYC(b_+25, b_+32);
      prevent_h_from_passing_d(gb);
      if (!(F & FC)) CYCT(b_+32, b_+34);
      else {
        CYC(b_+32, b_+34);
        A = 0x01;
        CYC(b_+34, b_+39); W8(wDimitriHitNpc) = A;
      }
    }
  }
  SET_AF(af);
  CYC(b_+39, b_+41);
}

void objectPreventLinkFromPassing_hook(GB *gb) {
  prevent_link_from_passing(gb);
  ret_effect(gb);
}

static void push_link_away_and_update_draw_priority(GB *gb) {
  BASE(interactionPushLinkAwayAndUpdateDrawPriority);
  CYC(b_+0, b_+3);
  prevent_link_from_passing(gb);
  CYC(b_+3, b_+6);
  C = 0xc0;
  CYC(SYM(objectSetPriorityRelativeToLink_withTerrainEffects), (SYM(objectSetPriorityRelativeToLink_withTerrainEffects) + 2));
  set_priority_relative_to_link(gb);
}

void interactionPushLinkAwayAndUpdateDrawPriority_hook(GB *gb) {
  push_link_away_and_update_draw_priority(gb);
  ret_effect(gb);
}

static void interaction_animate_as_npc(GB *gb) {
  call_interaction_animate(gb, SYM(interactionAnimateAsNpc));
  push_link_away_and_update_draw_priority(gb);
}

void interactionAnimateAsNpc_hook(GB *gb) {
  interaction_animate_as_npc(gb);
  ret_effect(gb);
}

void npcFaceLinkAndAnimate_hook(GB *gb) {
  BASE(npcFaceLinkAndAnimate);
  E = 0x6c;
  A = 0x01;
  CYC(b_+0, b_+5); mem_wr(gb, DE, A);
  E = 0x6b;
  CYC(b_+5, b_+8); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(b_+8, b_+11);
  else {
    CYC(b_+8, b_+11);
    C = 0x28;
    CYC(b_+11, b_+16);
    link_within_distance(gb);
    if (F & FC) CYCT(b_+16, b_+18);
    else {
      CYC(b_+16, b_+20);
      L = 0x6c;
      CYC(b_+20, b_+21); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
      A = 0x04;
      CYC(b_+21, b_+23);
    }
    B = A;
    alu_add(gb, A);
    alu_add(gb, A);
    H = D;
    L = 0x49;
    CYC(b_+23, b_+30); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+30, b_+32); interaction_animate_as_npc(gb); ret_effect(gb); return; }
    CYC(b_+30, b_+33); mem_wr(gb, HL, A);
    B = alu_srl(gb, B);
    E = 0x77;
    CYC(b_+33, b_+38); A = mem_rd(gb, DE);
    alu_add(gb, B);
    CYC(b_+38, b_+42);
    interaction_set_animation(gb);
    E = 0x6b;
    A = 0x1e;
    CYC(b_+42, b_+46);
  }
  A = alu_dec8(gb, A);
  CYC(b_+46, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50);
  interaction_animate_as_npc(gb);
  ret_effect(gb);
}

void returnIfScrollMode01Unset_hook(GB *gb) {
  BASE(returnIfScrollMode01Unset);
  CYC(b_+0, b_+3); A = W8(wScreenVariables);
  alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6); SET_HL(pop_effect(gb));
  CYC(b_+6, b_+8);
  ret_effect(gb);
}

void interactionDeleteAndRetIfEnabled02_hook(GB *gb) {
  BASE(interactionDeleteAndRetIfEnabled02);
  E = INTERACTION_BASE;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  alu_and(gb, 0x03);
  alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+3, b_+8); ret_effect(gb); return; }
  CYC(b_+3, b_+8); SET_HL(pop_effect(gb));
  CYC(b_+8, b_+12);
  interaction_delete(gb);
  ret_effect(gb);
}

static void angle_to_direction(GB *gb) {
  BASE(convertAngleToDirection);
  alu_add(gb, 0x04);
  alu_add(gb, A);
  alu_swap_a(gb);
  alu_and(gb, 0x03);
  CYC(b_+0, b_+8);
}

void convertAngleToDirection_hook(GB *gb) {
  angle_to_direction(gb);
  ret_effect(gb);
}

static void angle_de_to_direction(GB *gb) {
  BASE(convertAngleDeToDirection);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  angle_to_direction(gb);
}

void convertAngleDeToDirection_hook(GB *gb) {
  angle_de_to_direction(gb);
  ret_effect(gb);
}

void interactionSetAlwaysUpdateBit_hook(GB *gb) {
  BASE(interactionSetAlwaysUpdateBit);
  H = D;
  L = INTERACTION_BASE;
  CYC(b_+0, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+5, b_+6);
  ret_effect(gb);
}

void interactionUnsetAlwaysUpdateBit_hook(GB *gb) {
  BASE(interactionUnsetAlwaysUpdateBit);
  H = D;
  L = INTERACTION_BASE;
  CYC(b_+0, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+5, b_+6);
  ret_effect(gb);
}

void objectCheckLinkPushingAgainstCenter_hook(GB *gb) {
  BASE(objectCheckLinkPushingAgainstCenter);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_id);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+8); A = W8(wLinkAngle);
  alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+8, b_+11); ret_effect(gb); return; }
  CYC(b_+8, b_+14); A = W8(wGameKeysPressed);
  alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); ret_effect(gb); return; }
  CYC(b_+14, b_+17);
  B = 0x04;
  CYC(b_+17, b_+22);
  centered_with_link(gb);
  ret_effect(gb);
}

static void adjacent_tile_is_solid(GB *gb) {
  BASE(interactionCheckAdjacentTileIsSolid_viaDirection);
  SET_HL(adjacentTileDirOffsets);
  CYC(b_+5, b_+9);
  add_a_to_hl(gb);
  CYC(b_+9, b_+12);
  object_get_short_position(gb);
  CYC(b_+12, b_+13); alu_add(gb, mem_rd(gb, HL));
  H = wRoomCollisions >> 8;
  L = A;
  CYC(b_+13, b_+17); A = mem_rd(gb, HL);
  alu_or(gb, A);
  CYC(b_+17, b_+19);
}

void interactionCheckAdjacentTileIsSolid_hook(GB *gb) {
  BASE(interactionCheckAdjacentTileIsSolid);
  E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6);
  angle_de_to_direction(gb);
  CYC(b_+6, b_+8);
  adjacent_tile_is_solid(gb);
  ret_effect(gb);
}

void interactionCheckAdjacentTileIsSolid_viaDirection_hook(GB *gb) {
  BASE(interactionCheckAdjacentTileIsSolid_viaDirection);
  E = INTERACTION_BASE + 0x08;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  A = alu_sra(gb, A);
  CYC(b_+3, b_+5);
  adjacent_tile_is_solid(gb);
  ret_effect(gb);
}

void interactionDecCounter1IfTextNotActive_hook(GB *gb) {
  BASE(interactionDecCounter1IfTextNotActive);
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+8);
  obj_dec_body(gb, SYM(interactionDecCounter1), INTERACTION_BASE + OBJ_COUNTER1);
  ret_effect(gb);
}

void interactionDecCounter1IfPaletteNotFading_hook(GB *gb) {
  BASE(interactionDecCounter1IfPaletteNotFading);
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+8);
  obj_dec_body(gb, SYM(interactionDecCounter1), INTERACTION_BASE + OBJ_COUNTER1);
  ret_effect(gb);
}

static void interaction_h_set_position(GB *gb) {
  BASE(interactionHSetPosition);
  L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+0, b_+3); mem_wr(gb, HL, B);
  L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+3, b_+6); mem_wr(gb, HL, C);
  CYC(b_+6, b_+7);
}

void interactionHSetPosition_hook(GB *gb) {
  interaction_h_set_position(gb);
  ret_effect(gb);
}

void interactionSetPosition_hook(GB *gb) {
  BASE(interactionSetPosition);
  H = D;
  CYC(b_+0, b_+1);
  interaction_h_set_position(gb);
  ret_effect(gb);
}

// floating music note (bank $0b body)

void objectCreateFloatingMusicNote_hook(GB *gb) {
  BASE(objectCreateFloatingMusicNote);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  A = 0x01;
  CYC(b_+2, b_+6); H8(hFF8D) = A;
  bank_push(gb, b_+6, GV(0x0b, 0x0a));
  CALL_ROM(b_+16, ROM_b0b_objectCreateFloatingImage);
  bank_pop(gb, b_+19);
  CYC(b_+25, b_+26);
  ret_effect(gb);
}

// enemy and part animation (the transliterated tails pop the return address)

static void enemy_animate(GB *gb) {
  BASE(enemyAnimate);
  H = D;
  L = 0xa0;
  CYC(b_+0, b_+4); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(b_+4, b_+5); return; }
  CYC(b_+4, b_+7); A = H8(hRomBank);
  CYC(b_+7, b_+8); push_effect(gb, AF);
  CYC(b_+8, b_+12); H8(hRomBank) = GV(0x0d, 0x0c);
  CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, GV(0x0d, 0x0c));
  L = 0xa2;
  CYC(b_+15, b_+19);
  next_animation_frame(gb, SYM(_enemyNextAnimationFrame), 0xa0, 0x81, enemyAnimationFrameTable_bank0d, 0x9e);
}

void enemyAnimate_hook(GB *gb) {
  enemy_animate(gb);
  ret_effect(gb);
}

void enemySetAnimation_hook(GB *gb) {
  BASE(enemySetAnimation);
  alu_add(gb, A);
  C = A;
  B = 0x00;
  CYC(b_+0, b_+6); A = H8(hRomBank);
  CYC(b_+6, b_+7); push_effect(gb, AF);
  CYC(b_+7, b_+11); H8(hRomBank) = GV(0x0d, 0x0c);
  CYC(b_+11, b_+14); mem_wr(gb, MBC_ROM_BANK, GV(0x0d, 0x0c));
  E = 0x81;
  CYC(b_+14, b_+17); A = mem_rd(gb, DE);
  SET_HL(enemyAnimationTable_bank0d);
  CYC(b_+17, b_+21);
  add_double_index_to_hl(gb);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  CYC(b_+23, b_+25);
  next_animation_frame(gb, SYM(_enemyNextAnimationFrame), 0xa0, 0x81, enemyAnimationFrameTable_bank0d, 0x9e);
  ret_effect(gb);
}

void partAnimate_hook(GB *gb) {
  BASE(partAnimate);
  H = D;
  L = 0xe0;
  CYC(b_+0, b_+4); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+9); H8(hRomBank) = GV(0x16, 0x15);
  CYC(b_+9, b_+12); mem_wr(gb, MBC_ROM_BANK, GV(0x16, 0x15));
  L = 0xe2;
  CYC(b_+12, b_+16);
  next_animation_frame(gb, SYM(_partNextAnimationFrame), 0xe0, 0xc1, partAnimationFrameTable_bank16, 0xde);
  ret_effect(gb);
}

void partSetAnimation_hook(GB *gb) {
  BASE(partSetAnimation);
  alu_add(gb, A);
  C = A;
  B = 0x00;
  A = GV(0x16, 0x15);
  CYC(b_+0, b_+8); H8(hRomBank) = A;
  CYC(b_+8, b_+11); mem_wr(gb, MBC_ROM_BANK, A);
  E = 0xc1;
  CYC(b_+11, b_+14); A = mem_rd(gb, DE);
  SET_HL(partAnimationTable_bank16);
  CYC(b_+14, b_+18);
  add_double_index_to_hl(gb);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  CYC(b_+20, b_+22);
  next_animation_frame(gb, SYM(_partNextAnimationFrame), 0xe0, 0xc1, partAnimationFrameTable_bank16, 0xde);
  ret_effect(gb);
}

// enemy death

static void enemy_delete(GB *gb) {
  BASE(enemyDelete);
  E = 0x80;
  CYC(b_+0, b_+5);
  remove_from_abutton_list(gb);
  L = E;
  H = D;
  B = 0x10;
  alu_xor(gb, A);
  CYC(b_+5, b_+10);
  do {
    CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+14, b_+17); else CYC(b_+14, b_+17);
  } while (B);
  CYC(b_+17, b_+18);
}

void enemyDelete_hook(GB *gb) {
  enemy_delete(gb);
  ret_effect(gb);
}

static void enemy_create_death_puff(GB *gb) {
  BASE(enemyDie);
  E = 0xbf;
  CYC(b_+76, b_+79); A = mem_rd(gb, DE);
  alu_rlca(gb);
  if (F & FC) { CYCT(b_+79, b_+83); dec_num_enemies(gb); pop_effect(gb); return; }
  CYC(b_+79, b_+83);
  CALL_ROM(b_+83, ROM_getFreePartSlot);
  if (!(F & FZ)) { CYCT(b_+86, b_+87); pop_effect(gb); return; }
  CYC(b_+86, b_+87);
  E = 0x80;
  CYC(b_+87, b_+90); A = mem_rd(gb, DE);
  alu_and(gb, 0x03);
  L = alu_dec8(gb, L);
  CYC(b_+90, b_+94); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x02);
  L = alu_inc8(gb, L);
  E = 0x81;
  CYC(b_+96, b_+100); A = mem_rd(gb, DE);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  L = 0xed;
  E = 0xad;
  CYC(b_+101, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  CYC(b_+107, b_+110);
  object_copy_position(gb);
  L = 0xc7;
  CYC(b_+110, b_+113); mem_wr(gb, HL, B);
  A = 0x73;
  CYC(b_+113, b_+118);
  asm_call(gb, ROM_playSound, b_+5);
}

static void enemy_die_common(GB *gb) {
  BASE(enemyDie);
  CYC(b_+2, b_+5);
  push_effect(gb, b_+5);
  enemy_create_death_puff(gb);
  alu_bit(gb, 0, B);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); push_effect(gb, b_+10); asm_call(gb, ROM_markEnemyAsKilledInRoom, b_+10); }
  else CYC(b_+7, b_+10);
  A = 0x00;
  CYC(b_+10, b_+12);
  CALL_ROM(b_+12, ROM_checkGlobalFlag);
  if (!(F & FZ)) CYCT(b_+15, b_+17);
  else {
    CYC(b_+15, b_+17);
    L = 0x20;
    CYC(b_+17, b_+22);
    inc_hl_ref16_with_cap(gb);
    CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+23, b_+24); H = mem_rd(gb, HL);
    L = A;
    SET_BC(0x03e8);
    CYC(b_+24, b_+31);
    compare_hl_to_bc(gb);
    alu_rlca(gb);
    A = 0x00;
    CYC(b_+31, b_+34);
    if (!(F & FC)) { CYCT(b_+34, b_+37); push_effect(gb, b_+37); asm_call(gb, ROM_setGlobalFlag, b_+37); }
    else CYC(b_+34, b_+37);
  }
  SET_HL(wMapleKillCounter);
  CYC(b_+37, b_+43);
  inc_hl_ref_with_cap(gb);
  A = 0x3a;
  CYC(b_+43, b_+48);
  cp_active_ring(gb);
  A = 0xff;
  if (F & FZ) CYCT(b_+48, b_+52);
  else { CYC(b_+48, b_+53); alu_xor(gb, A); }
  L = GV(0x4f, 0x4c);
  C = 0x10;
  CYC(b_+53, b_+57);
  do {
    alu_rlca(gb);
    if (F & FC) { CYCT(b_+57, b_+61); inc_hl_ref_with_cap(gb); }
    else CYC(b_+57, b_+61);
    CYC(b_+61, b_+64);
    inc_hl_ref_with_cap(gb);
    L = alu_inc8(gb, L);
    C = alu_dec8(gb, C);
    if (C) CYCT(b_+64, b_+68); else CYC(b_+64, b_+68);
  } while (C);
  A = 0x03;
  CYC(b_+68, b_+73);
  add_to_gasha_maturity(gb);
  CYC(b_+73, b_+76);
  enemy_delete(gb);
}

void enemyDie_hook(GB *gb) {BASE(enemyDie); B = 0x01; CYC(b_+0, b_+2); enemy_die_common(gb); ret_effect(gb); }
void enemyDie_uncounted_hook(GB *gb) {BASE(enemyDie_uncounted); B = 0x00; CYC(b_+0, b_+4); enemy_die_common(gb); ret_effect(gb); }
void enemyDie_withoutItemDrop_hook(GB *gb) {BASE(enemyDie_withoutItemDrop); B = 0x81; CYC(b_+0, b_+4); enemy_die_common(gb); ret_effect(gb); }
void enemyDie_uncounted_withoutItemDrop_hook(GB *gb) {BASE(enemyDie_uncounted_withoutItemDrop); B = 0x80; CYC(b_+0, b_+4); enemy_die_common(gb); ret_effect(gb); }

// energy swirls (bank $11 bodies)

void createEnergySwirlGoingIn_hook(GB *gb) {
  BASE(createEnergySwirlGoingIn);
  L = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, GV(0x11, 0x10));
  CALL_ROM(b_+11, ROM_b11_createEnergySwirlGoingIn_body);
  bank_pop(gb, b_+14);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

void createEnergySwirlGoingOut_hook(GB *gb) {
  BASE(createEnergySwirlGoingOut);
  L = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, GV(0x11, 0x10));
  CALL_ROM(b_+11, ROM_b11_createEnergySwirlGoingOut_body);
  bank_pop(gb, b_+14);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

// Link input and state

void convertLinkAngleToDirectionButtons_hook(GB *gb) {
  BASE(convertLinkAngleToDirectionButtons);
  CYC(b_+0, b_+3); A = W8(wLinkAngle);
  alu_add(gb, A);
  if (F & FC) { CYCT(b_+3, b_+6); alu_xor(gb, A); CYC(b_+17, b_+19); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  alu_add(gb, A);
  alu_swap_a(gb);
  uint16_t hl = HL;
  SET_HL(linkAngleToDirectionButtons);
  CYC(b_+6, b_+14);
  add_a_to_hl(gb);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  SET_HL(hl);
  CYC(b_+15, b_+17);
  ret_effect(gb);
}

static void clear_pegasus_seed_counter(GB *gb) {
  BASE(clearPegasusSeedCounter);
  SET_HL(wPegasusSeedCounter);
  alu_xor(gb, A);
  CYC(b_+0, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+7);
}

void clearPegasusSeedCounter_hook(GB *gb) {
  clear_pegasus_seed_counter(gb);
  ret_effect(gb);
}

void setSimulatedInputAddress_hook(GB *gb) {
  BASE(setSimulatedInputAddress);
  SET_DE(wSimulatedInputBank);
  CYC(b_+0, b_+4); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = L;
  CYC(b_+4, b_+7); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+7, b_+10); mem_wr(gb, DE, A);
  E = 0xc5;
  alu_xor(gb, A);
  CYC(b_+10, b_+14); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(b_+14, b_+16); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  A = alu_inc8(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+22);
  clear_pegasus_seed_counter(gb);
  ret_effect(gb);
}

void getSimulatedInput_hook(GB *gb) {
  BASE(getSimulatedInput);
  CYC(b_+0, b_+3); A = W8(wPaletteThread_mode);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+8); A = W8(wUseSimulatedInput);
  alu_rlca(gb);
  if (F & FC) CYCT(b_+8, b_+11);
  else {
    CYC(b_+8, b_+11);
    SET_HL(wSimulatedInputCounter);
    CYC(b_+11, b_+17);
    dec16_cap(gb, HL);
    if (!(F & FZ)) CYCT(b_+17, b_+19);
    else {
      CYC(b_+17, b_+21); A = H8(hRomBank);
      CYC(b_+21, b_+22); push_effect(gb, AF);
      SET_HL(wSimulatedInputBank);
      CYC(b_+22, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+26, b_+28); H8(hRomBank) = A;
      CYC(b_+28, b_+31); mem_wr(gb, MBC_ROM_BANK, A);
      CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+32, b_+33); H = mem_rd(gb, HL);
      L = A;
      CYC(b_+33, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+35, b_+38); W8(wSimulatedInputCounter) = A;
      CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+39, b_+42); mem_wr(gb, wSimulatedInputCounter + 1, A);
      alu_bit(gb, 7, A);
      if (F & FZ) {
        CYCT(b_+42, b_+46);
        CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+54, b_+57); W8(wSimulatedInputValue) = A;
      } else {
        CYC(b_+42, b_+46);
        A = 0xff;
        CYC(b_+46, b_+51); W8(wUseSimulatedInput) = A;
        CYC(b_+51, b_+53);
      }
      CYC(b_+57, b_+58); SET_AF(pop_effect(gb));
      CYC(b_+58, b_+60); H8(hRomBank) = A;
      CYC(b_+60, b_+63); mem_wr(gb, MBC_ROM_BANK, A);
      A = L;
      CYC(b_+63, b_+67); W8(wSimulatedInputAddressL) = A;
      A = H;
      CYC(b_+67, b_+71); W8(wSimulatedInputAddressH) = A;
    }
  }
  CYC(b_+71, b_+74); A = W8(wSimulatedInputValue);
  CYC(b_+74, b_+75);
  ret_effect(gb);
}

void itemSetState_hook(GB *gb) {
  BASE(itemSetState);
  H = D;
  L = OBJ_STATE;
  CYC(b_+0, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x00);
  CYC(b_+6, b_+7);
  ret_effect(gb);
}

void special_object_set_animation_hook(GB *gb) {
  BASE(specialObjectSetAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  E = 0x30;
  CYC(b_+0, b_+3); mem_wr(gb, DE, A);
  alu_add(gb, A);
  C = A;
  B = 0x00;
  CYC(b_+3, b_+7);
  bank_push(gb, b_+7, 0x06);
  CALL_C(b_+17, specialObjectSetAnimation_body_hook, SYM(specialObjectSetAnimation_body), b_+20);
  bank_pop(gb, b_+20);
  CYC(b_+26, b_+27);
}

void specialObjectSetAnimation_hook(GB *gb) {
  special_object_set_animation_hook(gb);
  ret_effect(gb);
}

void specialObjectAnimate_hook(GB *gb) {
  BASE(specialObjectAnimate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  H = D;
  L = 0x20;
  CYC(b_+0, b_+4); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  bank_push(gb, b_+5, 0x06);
  L = 0x22;
  CYC(b_+15, b_+17);
  CALL_C(b_+17, specialObjectNextAnimationFrame_hook, SYM(specialObjectNextAnimationFrame), b_+20);
  bank_pop(gb, b_+20);
  CYC(b_+26, b_+27);
  ret_effect(gb);
}

void putLinkOnGround_hook(GB *gb) {
  BASE(putLinkOnGround);
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  alu_rrca(gb);
  if (F & FC) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+5);
  uint16_t de = DE;
  alu_xor(gb, A);
  CYC(b_+5, b_+10); W8(wLinkInAir) = A;
  SET_HL(w1Link_speedZ);
  CYC(b_+10, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = OBJ_Z;
  CYC(b_+15, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = 0x01;
  CYC(b_+19, b_+22); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(b_+22, b_+25);
  else {
    CYC(b_+22, b_+25);
    D = H;
    A = 0x10;
    CYC(b_+25, b_+31);
    special_object_set_animation_hook(gb);
  }
  SET_DE(de);
  CYC(b_+31, b_+33);
  ret_effect(gb);
}

static void set_link_force_state_08(GB *gb) {
  BASE(setLinkForceStateToState08_withParam);
  uint16_t hl = HL;
  SET_HL(wcc50);
  CYC(b_+0, b_+5); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08);
  SET_HL(hl);
  CYC(b_+7, b_+9);
}

void setLinkForceStateToState08_withParam_hook(GB *gb) {
  set_link_force_state_08(gb);
  ret_effect(gb);
}

void setLinkForceStateToState08_hook(GB *gb) {
  BASE(setLinkForceStateToState08);
  alu_xor(gb, A);
  CYC(b_+0, b_+1);
  set_link_force_state_08(gb);
  ret_effect(gb);
}

static void clear_link_state_bytes(GB *gb) {
  BASE(setLinkID);
  alu_xor(gb, A);
  CYC(b_+4, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10);
}

void setLinkIDOverride_hook(GB *gb) {
  BASE(setLinkIDOverride);
  alu_or(gb, 0x80);
  CYC(b_+0, b_+5); W8(wLinkIDOverride) = A;
  SET_HL(w1Link_subid);
  CYC(b_+5, b_+10);
  clear_link_state_bytes(gb);
  ret_effect(gb);
}

void setLinkID_hook(GB *gb) {
  BASE(setLinkID);
  SET_HL(w1Link_id);
  CYC(b_+0, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  clear_link_state_bytes(gb);
  ret_effect(gb);
}

void respawnLink_hook(GB *gb) {
  BASE(respawnLink);
  A = 0x02;
  CYC(b_+0, b_+5); W8(wLinkForceState) = A;
  A = 0x02;
  CYC(b_+5, b_+10); W8(wLinkStateParameter) = A;
  alu_or(gb, D);
  CYC(b_+10, b_+12);
  ret_effect(gb);
}

void checkLinkPushingAgainstWall_hook(GB *gb) {
  BASE(checkLinkPushingAgainstWall);
  uint16_t hl = HL;
  CYC(b_+0, b_+4); A = mem_rd(gb, w1Link_direction);
  SET_HL(wallCollisionDirections);
  CYC(b_+4, b_+8);
  add_double_index_to_hl(gb);
  CYC(b_+8, b_+11); A = mem_rd(gb, w1Link_adjacentWallsBitset);
  CYC(b_+11, b_+12); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    SET_HL(HL + 1);
    CYC(b_+13, b_+19); A = W8(wGameKeysPressed);
    CYC(b_+19, b_+20); alu_and(gb, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYC(b_+20, b_+22);
      SET_HL(hl);
      alu_scf(gb);
      CYC(b_+22, b_+25);
      ret_effect(gb);
      return;
    }
    CYCT(b_+20, b_+22);
  } else CYCT(b_+13, b_+15);
  SET_HL(hl);
  alu_xor(gb, A);
  CYC(b_+25, b_+28);
  ret_effect(gb);
}

static void direction_from_angle(GB *gb) {
  BASE(updateLinkDirectionFromAngle);
  CYC(b_+5, b_+6); B = mem_rd(gb, HL);
  CYC(b_+6, b_+9); A = W8(wLinkAngle);
  alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+9, b_+13); return; }
  CYC(b_+9, b_+13);
  alu_and(gb, 0x1c);
  alu_rrca(gb);
  alu_rrca(gb);
  alu_rra(gb);
  if (!(F & FC)) CYCT(b_+13, b_+20);
  else {
    CYC(b_+13, b_+20);
    C = A;
    alu_sub(gb, B);
    A = alu_inc8(gb, A);
    alu_and(gb, 0x02);
    if (F & FZ) { CYCT(b_+20, b_+27); return; }
    CYC(b_+20, b_+28);
    A = C;
  }
  CYC(b_+28, b_+29); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+29, b_+31); return; }
  CYC(b_+29, b_+32); mem_wr(gb, HL, A);
  B = A;
  alu_scf(gb);
  CYC(b_+32, b_+34);
}

static void update_direction_from_angle(GB *gb, uint16_t from, uint16_t target) {
  BASE(updateLinkDirectionFromAngle);
  uint16_t bc = BC, hl = HL;
  SET_HL(target);
  CYC(from, from == (SYM(checkLinkPushingAgainstWall__collisionDirections) + 8) ? (SYM(updateCompanionDirectionFromAngle) + 7) : b_+5);
  direction_from_angle(gb);
  A = B;
  SET_HL(hl);
  SET_BC(bc);
  CYC(b_+34, b_+38);
}

void updateLinkDirectionFromAngle_hook(GB *gb) {
  update_direction_from_angle(gb, SYM(updateLinkDirectionFromAngle), w1Link_direction);
  ret_effect(gb);
}

void updateCompanionDirectionFromAngle_hook(GB *gb) {
  update_direction_from_angle(gb, SYM(updateCompanionDirectionFromAngle), w1Companion_id + 0x07);
  ret_effect(gb);
}

void specialObjectSetCoordinatesToRespawnYX_hook(GB *gb) {
  BASE(specialObjectSetCoordinatesToRespawnYX);
  H = D;
  L = 0x08;
  CYC(b_+0, b_+6); A = W8(wLinkLocalRespawnDir);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0xff;
  CYC(b_+7, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+13); W8(wLinkPathIndex) = A;
  L = OBJ_YH;
  CYC(b_+13, b_+18); A = W8(wLinkLocalRespawnY);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(b_+19, b_+23); A = W8(wLinkLocalRespawnX);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(b_+24, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = 0x2d;
  CYC(b_+27, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+31);
  ret_effect(gb);
}

void resetLinkInvincibility_hook(GB *gb) {
  BASE(resetLinkInvincibility);
  SET_HL(w1Link_oamFlagsBackup);
  CYC(b_+0, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  L = 0x24;
  alu_xor(gb, A);
  CYC(b_+5, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = 0x28;
  CYC(b_+10, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void decPegasusSeedCounter_hook(GB *gb) {
  BASE(decPegasusSeedCounter);
  SET_HL(wPegasusSeedCounter + 1);
  CYC(b_+0, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  L = alu_dec8(gb, L);
  B = 0x00;
  C = 0x07;
  A = 0x11;
  CYC(b_+5, b_+15);
  cp_active_ring(gb);
  if (F & FZ) CYCT(b_+15, b_+17);
  else {
    CYC(b_+15, b_+17);
    C = 0x0f;
    CYC(b_+17, b_+22);
    dec16_cap(gb, HL);
    if (F & FZ) { CYCT(b_+22, b_+23); ret_effect(gb); return; }
    CYC(b_+22, b_+24); A = mem_rd(gb, HL);
    alu_and(gb, C);
    if (!(F & FZ)) CYCT(b_+24, b_+27);
    else { CYC(b_+24, b_+29); B = 0x80; }
  }
  CYC(b_+29, b_+32);
  dec16_cap(gb, HL);
  if (F & FZ) { CYCT(b_+32, b_+33); ret_effect(gb); return; }
  CYC(b_+32, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_and(gb, C);
  if (!(F & FZ)) CYCT(b_+34, b_+37);
  else { CYC(b_+34, b_+39); B = 0x80; }
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  alu_or(gb, B);
  CYC(b_+40, b_+42); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+42, b_+43);
  ret_effect(gb);
}

void checkPegasusSeedCounter_hook(GB *gb) {
  BASE(checkPegasusSeedCounter);
  SET_HL(wPegasusSeedCounter);
  CYC(b_+0, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+6, b_+7);
  ret_effect(gb);
}

// items

void itemDelete_hook(GB *gb) {
  BASE(itemDelete);
  H = D;
  L = 0x00;
  B = 0x10;
  alu_xor(gb, A);
  CYC(b_+0, b_+6);
  do {
    CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+10, b_+13); else CYC(b_+10, b_+13);
  } while (B);
  CYC(b_+13, b_+14);
  ret_effect(gb);
}

void itemUpdateAngle_hook(GB *gb) {
  BASE(itemUpdateAngle);
  H = D;
  L = 0x08;
  CYC(b_+0, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_swap_a(gb);
  alu_rrca(gb);
  CYC(b_+4, b_+8); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+8, b_+9);
  ret_effect(gb);
}

void getFreeItemSlot_hook(GB *gb) {
  BASE(getFreeItemSlot);
  SET_HL(w1WeaponItem + 0x100);
  CYC(b_+0, b_+3);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) { CYCT(b_+4, b_+6); break; }
    CYC(b_+4, b_+6);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xdc);
    if (F & FC) { CYCT(b_+6, b_+12); continue; }
    CYC(b_+6, b_+12);
    alu_or(gb, H);
    CYC(b_+12, b_+14);
    break;
  }
  ret_effect(gb);
}

// dungeons

void setVisitedRoomFlag_hook(GB *gb) {
  BASE(setVisitedRoomFlag);
  CYC(b_+0, b_+3);
  this_room_flags(gb);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+5, b_+6);
  ret_effect(gb);
}

void getThisRoomDungeonProperties_hook(GB *gb) {
  BASE(getThisRoomDungeonProperties);
  bank_push(gb, b_+0, 0x01);
  CYC(b_+10, b_+13); A = W8(wActiveGroup);
  alu_sub(gb, 0x04);
  alu_and(gb, 0x01);
  SET_HL(dungeonRoomPropertiesGroupTable_bank01);
  CYC(b_+13, b_+21);
  add_double_index_to_hl(gb);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+23, b_+27); A = W8(wActiveRoom);
  B = 0x00;
  C = A;
  alu_add_hl(gb, BC);
  CYC(b_+27, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+35); W8(wDungeonRoomProperties) = A;
  bank_pop(gb, b_+35);
  CYC(b_+41, b_+42);
  ret_effect(gb);
}

static void dungeon_layout_address(GB *gb) {
  BASE(getDungeonLayoutAddress);
  uint16_t bc = BC, de = DE;
  CYC(b_+0, b_+5); A = W8(wDungeonFloor);
  C = 0x40;
  CYC(b_+5, b_+10);
  multiply_a_by_c(gb);
  SET_BC(w2DungeonLayout);
  alu_add_hl(gb, BC);
  SET_DE(de);
  SET_BC(bc);
  CYC(b_+10, b_+17);
}

void getDungeonLayoutAddress_hook(GB *gb) {
  dungeon_layout_address(gb);
  ret_effect(gb);
}

static void room_in_dungeon(GB *gb) {
  BASE(getRoomInDungeon);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  A = 0x02;
  CYC(b_+2, b_+6); mem_wr(gb, IO_SVBK, A);
  CYC(b_+6, b_+9);
  dungeon_layout_address(gb);
  CYC(b_+9, b_+11); A = H8(hFF8B);
  CYC(b_+11, b_+12);
  add_a_to_hl(gb);
  CYC(b_+12, b_+13); L = mem_rd(gb, HL);
  alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, IO_SVBK, A);
  A = L;
  CYC(b_+16, b_+18);
}

void getRoomInDungeon_hook(GB *gb) {
  room_in_dungeon(gb);
  ret_effect(gb);
}

void getActiveRoomFromDungeonMapPosition_hook(GB *gb) {
  BASE(getActiveRoomFromDungeonMapPosition);
  CYC(b_+0, b_+3); A = W8(wDungeonMapPosition);
  room_in_dungeon(gb);
  ret_effect(gb);
}

static void free_enemy_slot_uncounted(GB *gb) {
  BASE(getFreeEnemySlot_uncounted);
  SET_HL(ENEMY_SLOTS);
  CYC(b_+0, b_+3);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+4, b_+7);
      A = alu_inc8(gb, A);
      CYC(b_+15, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
      alu_xor(gb, A);
      CYC(b_+17, b_+19);
      return;
    }
    CYC(b_+4, b_+7);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+7, b_+13); continue; }
    CYC(b_+7, b_+13);
    alu_or(gb, H);
    CYC(b_+13, b_+15);
    return;
  }
}

void getFreeEnemySlot_uncounted_hook(GB *gb) {
  free_enemy_slot_uncounted(gb);
  ret_effect(gb);
}

void getFreeEnemySlot_hook(GB *gb) {
  BASE(getFreeEnemySlot);
  CYC(b_+0, b_+3);
  free_enemy_slot_uncounted(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+7); A = W8(wNumEnemies);
  A = alu_inc8(gb, A);
  CYC(b_+7, b_+11); W8(wNumEnemies) = A;
  alu_xor(gb, A);
  CYC(b_+11, b_+13);
  ret_effect(gb);
}

void enemyReplaceWithID_hook(GB *gb) {
  BASE(enemyReplaceWithID);
  H = D;
  uint16_t bc0 = BC;
  L = 0x80;
  CYC(b_+0, b_+5); B = mem_rd(gb, HL);
  L = 0x8b;
  CYC(b_+5, b_+8); C = mem_rd(gb, HL);
  uint16_t bc1 = BC;
  L = 0x8d;
  CYC(b_+8, b_+12); B = mem_rd(gb, HL);
  L = 0x8f;
  CYC(b_+12, b_+15); C = mem_rd(gb, HL);
  uint16_t bc2 = BC;
  CYC(b_+15, b_+19);
  enemy_delete(gb);
  SET_BC(bc2);
  L = 0x8f;
  CYC(b_+19, b_+23); mem_wr(gb, HL, C);
  L = 0x8d;
  CYC(b_+23, b_+26); mem_wr(gb, HL, B);
  SET_BC(bc1);
  L = 0x8b;
  CYC(b_+26, b_+30); mem_wr(gb, HL, C);
  L = 0x80;
  A = B;
  alu_and(gb, 0x73);
  CYC(b_+30, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);
  SET_BC(bc0);
  CYC(b_+36, b_+38); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+38, b_+40); mem_wr(gb, HL, C);
  CYC(b_+40, b_+41);
  ret_effect(gb);
}

void findFreeStaticObjectSlot_hook(GB *gb) {
  BASE(findFreeStaticObjectSlot);
  SET_HL(wStaticObjects);
  B = 0x08;
  CYC(b_+0, b_+5);
  for (;;) {
    CYC(b_+5, b_+6); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) { CYCT(b_+6, b_+8); break; }
    CYC(b_+6, b_+8);
    A = 0x08;
    alu_add(gb, L);
    L = A;
    B = alu_dec8(gb, B);
    if (B) { CYCT(b_+8, b_+15); continue; }
    CYC(b_+8, b_+15);
    alu_or(gb, H);
    CYC(b_+15, b_+17);
    break;
  }
  ret_effect(gb);
}

// interaction extras

void interactionLoadExtraGraphics_hook(GB *gb) {
  BASE(interactionLoadExtraGraphics);
  E = INTERACTION_BASE + 0x01;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(b_+6, b_+9); W8(wLoadedObjectGfxEnd) = A;
  CYC(b_+9, b_+10);
  ret_effect(gb);
}

void interactionGetMiniScript_hook(GB *gb) {
  BASE(interactionGetMiniScript);
  H = D;
  L = 0x58;
  CYC(b_+0, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+5, b_+7);
  ret_effect(gb);
}

// palettes and dungeon setup (bank 3 and bank 1 bodies)

void clearFadingPalettes_hook(GB *gb) {
  BASE(clearFadingPalettes);
  bank_push(gb, b_+0, 0x03);
  CALL_ROM(b_+10, ROM_b03_clearFadingPalettes_body);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void flashScreen_hook(GB *gb) {
  BASE(flashScreen);
  bank_push(gb, b_+0, 0x03);
  CALL_ROM(b_+10, ROM_b03_flashScreen_body);
  B = 0x01;
  if (!(F & FZ)) CYCT(b_+13, b_+17);
  else { CYC(b_+13, b_+18); B = alu_dec8(gb, B); }
  bank_pop(gb, b_+18);
  A = B;
  alu_or(gb, A);
  CYC(b_+24, b_+27);
  ret_effect(gb);
}

void loadDungeonLayout_hook(GB *gb) {
  BASE(loadDungeonLayout);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  alu_and(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  bank_push(gb, b_+6, 0x01);
  CALL_C(b_+16, loadDungeonLayout_b01_hook, SYM(loadDungeonLayout_b01), b_+19);
  bank_pop(gb, b_+19);
  CYC(b_+25, b_+26);
  ret_effect(gb);
}

void initializeDungeonStuff_hook(GB *gb) {
  BASE(initializeDungeonStuff);
  alu_xor(gb, A);
  CYC(b_+0, b_+4); W8(wToggleBlocksState) = A;
  CYC(b_+4, b_+7); W8(wSwitchState) = A;
  CYC(b_+7, b_+10); W8(wSpinnerState) = A;
  CYC(b_+10, b_+13);
  loadStaticObjects_hook(gb);
}

void clearStaticObjects_hook(GB *gb) {
  BASE(clearStaticObjects);
  SET_HL(wStaticObjects);
  B = GV(0x40, 0x80);
  CYC(b_+0, b_+8);
  clearMemory(gb, HL, GV(0x40, 0x80));
  SET_HL(HL + GV(0x40, 0x80));
  B = 0;
  A = 0;
  F = FZ | FN;
  ret_effect(gb);
}

void objectDeleteRelatedObj1AsStaticObject_hook(GB *gb) {
  BASE(objectDeleteRelatedObj1AsStaticObject);
  CYC(b_+0, b_+2); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_RELATED1);
  L = A;
  H = D;
  CYC(b_+2, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); H = mem_rd(gb, HL);
  E = L;
  L = A;
  alu_or(gb, H);
  CYC(b_+8, b_+11);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  alu_xor(gb, A);
  CYC(b_+12, b_+14); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(b_+14, b_+16); mem_wr(gb, DE, A);
  E = 0x08;
  CYC(b_+16, b_+18);
  do {
    CYC(b_+18, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
    E = alu_dec8(gb, E);
    if (E) CYCT(b_+19, b_+22); else CYC(b_+19, b_+22);
  } while (E);
  CYC(b_+22, b_+23);
  ret_effect(gb);
}

// parent items (bank 6 bodies)

static void bank6_function_caller(GB *gb, uint16_t sp0_) {
  BASE(checkUseItems_b00);
  bank_push(gb, b_+2, 0x06);
  CALL_C(b_+12, functionCaller_b06_hook, GV(SYM(functionCaller_b06), 0x4822), b_+15);
  bank_pop(gb, b_+15);
  CYC(b_+21, b_+22);
}

void clearAllParentItems_hook(GB *gb) {
  BASE(clearAllParentItems);
  uint16_t sp0_ = gb->sp;
  C = 0x00;
  CYC(b_+0, b_+4);
  bank6_function_caller(gb, sp0_);
  ret_effect(gb);
}

void updateParentItemButtonAssignment_hook(GB *gb) {
  BASE(updateParentItemButtonAssignment);
  uint16_t sp0_ = gb->sp;
  C = 0x01;
  CYC(b_+0, b_+4);
  bank6_function_caller(gb, sp0_);
  ret_effect(gb);
}

void clearVar3fForParentItems_hook(GB *gb) {
  BASE(clearVar3fForParentItems);
  SET_HL(w1ParentItem2_var3f);
  CYC(b_+0, b_+3);
  for (;;) {
    CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xd6);
    if (F & FC) { CYCT(b_+5, b_+11); continue; }
    CYC(b_+5, b_+12);
    break;
  }
  ret_effect(gb);
}

void linkCreateSplash_hook(GB *gb) {
  BASE(linkCreateSplash);
  B = 0x03;
  CYC(b_+0, b_+5); A = W8(wLinkSwimmingState);
  alu_bit(gb, 6, A);
  if (F & FZ) CYCT(b_+5, b_+9);
  else { CYC(b_+5, b_+10); B = alu_inc8(gb, B); }
  CYC(b_+10, b_+13); A = W8(wTilesetFlags);
  alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+13, b_+18);
    C = 0x00;
    CYC(SYM(objectCreateInteractionWithSubid00), (SYM(objectCreateInteractionWithSubid00) + 2));
    object_create_interaction(gb);
    ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+18);
  CALL_ROM(b_+18, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(b_+21, b_+22); ret_effect(gb); return; }
  CYC(b_+21, b_+23); mem_wr(gb, HL, B);
  SET_BC(0xfd00);
  CYC(b_+23, b_+29);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void clearVariousLinkVariables_hook(GB *gb) {
  BASE(clearVariousLinkVariables);
  alu_xor(gb, A);
  CYC(b_+0, b_+4); mem_wr(gb, w1Link_var36, A);
  CYC(b_+4, b_+7); mem_wr(gb, w1Link_speed, A);
  CYC(b_+7, b_+10); mem_wr(gb, w1Link_var3e, A);
  CYC(b_+10, b_+13); mem_wr(gb, w1Link_var12, A);
  A = alu_dec8(gb, A);
  CYC(b_+13, b_+17); mem_wr(gb, w1Link_angle, A);
  CYC(b_+17, b_+18);
  ret_effect(gb);
}

void dropLinkHeldItem_hook(GB *gb) {
  BASE(dropLinkHeldItem);
  CYC(b_+0, b_+3); A = W8(wAButtonSensitiveObjectListEnd);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(b_+3, b_+6);
  else {
    CYC(b_+3, b_+9); A = W8(wLinkGrabState);
    alu_and(gb, 0x07);
    alu_sub(gb, 0x02);
    alu_cp(gb, 0x02);
    if (!(F & FC)) CYCT(b_+9, b_+17);
    else {
      CYC(b_+9, b_+20);
      SET_HL(w1Link_relatedObj2);
      CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+21, b_+22); H = mem_rd(gb, HL);
      alu_add(gb, 0x04);
      L = A;
      CYC(b_+22, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
      alu_cp(gb, 0x02);
      if (!(F & FZ)) CYCT(b_+26, b_+30);
      else {
        CYC(b_+26, b_+30);
        A = 0x03;
        CYC(b_+30, b_+33); mem_wr(gb, HL, A);
        A = L;
        alu_add(gb, 0x04);
        L = A;
        CYC(b_+33, b_+39); mem_wr(gb, HL, 0xff);
      }
    }
  }
  alu_xor(gb, A);
  CYC(b_+39, b_+43); W8(wLinkGrabState) = A;
  CYC(b_+43, b_+46); W8(wLinkGrabState2) = A;
  CYC(b_+46, b_+47);
  ret_effect(gb);
}

void objectAddToGrabbableObjectBuffer_hook(GB *gb) {
  BASE(objectAddToGrabbableObjectBuffer);
  SET_HL(wGrabbableObjectBuffer);
  CYC(b_+0, b_+3);
  for (;;) {
    L = alu_inc8(gb, L);
    CYC(b_+3, b_+6); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+6, b_+8);
      A = D;
      CYC(b_+15, b_+17); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(b_+17, b_+19); A = H8(hActiveObjectType);
      CYC(b_+19, b_+20); mem_wr(gb, HL, A);
      CYC(b_+20, b_+21);
      break;
    }
    CYC(b_+6, b_+8);
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, GV(0x84, 0x9e));
    if (F & FC) { CYCT(b_+8, b_+14); continue; }
    CYC(b_+8, b_+15);
    break;
  }
  ret_effect(gb);
}

// breakable tiles (bank 6 body); the carry comes back through bit 0 of e across the bank pop

void tryToBreakTile_body_hook(GB *gb);

static void try_to_break_tile(GB *gb, uint16_t sp0_) {
  BASE(tryToBreakTile);
  CYC(b_+0, b_+2); H8(hFF8F) = A;
  bank_push(gb, b_+2, 0x06);
  CALL_C(b_+12, tryToBreakTile_body_hook, ROM_b06_tryToBreakTile_body, b_+15);
  CYC(b_+15, b_+17); E = alu_rl(gb, E);
  bank_pop(gb, b_+17);
  CYC(b_+23, b_+25); E = alu_rr(gb, E);
  CYC(b_+25, b_+26);
}

void tryToBreakTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  try_to_break_tile(gb, sp0_);
  ret_effect(gb);
}

void itemTryToBreakTile_hook(GB *gb) {
  BASE(itemTryToBreakTile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = OBJ_YH;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = OBJ_XH;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  try_to_break_tile(gb, sp0_);
  ret_effect(gb);
}

// global flags

void unsetGlobalFlag_hook(GB *gb) {
  BASE(unsetGlobalFlag);
  SET_HL(wGlobalFlags);
  CYC(b_+0, b_+6);
  unset_flag(gb);
  ret_effect(gb);
}

// palette thread control

static void clear_palette_fade_variables(GB *gb) {
  BASE(clearPaletteFadeVariables);
  alu_xor(gb, A);
  CYC(b_+0, b_+4); W8(wPaletteThread_mode) = A;
  CYC(b_+4, b_+7); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(b_+7, b_+9); H8(hBgPaletteSources) = A;
  CYC(b_+9, b_+11); H8(hSprPaletteSources) = A;
  CYC(b_+11, b_+14); W8(wPaletteThread_updateRate) = A;
  CYC(b_+14, b_+17); W8(wLockBG7Color3ToBlack) = A;
  SET_HL(wDirtyFadeBgPalettes);
  CYC(b_+17, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+25);
}

void clearPaletteFadeVariables_hook(GB *gb) {
  clear_palette_fade_variables(gb);
  ret_effect(gb);
}

void clearPaletteFadeVariablesAndRefreshPalettes_hook(GB *gb) {
  BASE(clearPaletteFadeVariablesAndRefreshPalettes);
  A = 0xff;
  CYC(b_+0, b_+4); H8(hDirtyBgPalettes) = A;
  CYC(b_+4, b_+6); H8(hDirtySprPalettes) = A;
  clear_palette_fade_variables(gb);
  ret_effect(gb);
}

static void make_all_palettes_use_fading(GB *gb) {
  BASE(makeAllPaletteUseFading);
  A = 0xff;
  SET_HL(wDirtyFadeBgPalettes);
  CYC(b_+0, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10);
}

void makeAllPaletteUseFading_hook(GB *gb) {
  make_all_palettes_use_fading(gb);
  ret_effect(gb);
}

static void set_palette_thread_delay(GB *gb) {
  BASE(setPaletteThreadDelay);
  CYC(b_+0, b_+3); W8(wPaletteThread_counterRefill) = A;
  A = 0x01;
  CYC(b_+3, b_+8); W8(wPaletteThread_counter) = A;
  CYC(b_+8, b_+9);
}

void setPaletteThreadDelay_hook(GB *gb) {
  set_palette_thread_delay(gb);
  ret_effect(gb);
}

static void fadeout_white_tail(GB *gb) {
  BASE(fadeoutToWhite);
  CYC(b_+7, b_+10); W8(wPaletteThread_speed) = A;
  alu_xor(gb, A);
  CYC(b_+10, b_+14); mem_wr(gb, wPaletteFadeOffset, A);
  make_all_palettes_use_fading(gb);
}

void fadeoutToWhite_hook(GB *gb) {
  BASE(fadeoutToWhite);
  A = 0x01;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+5, b_+7);
  fadeout_white_tail(gb);
  ret_effect(gb);
}

void fastFadeoutToWhite_hook(GB *gb) {
  BASE(fastFadeoutToWhite);
  A = 0x01;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(b_+5, b_+9);
  fadeout_white_tail(gb);
  ret_effect(gb);
}

void fadeoutToWhiteWithDelay_hook(GB *gb) {
  BASE(fadeoutToWhiteWithDelay);
  CYC(b_+0, b_+3);
  set_palette_thread_delay(gb);
  A = 0x09;
  CYC(b_+3, b_+8); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+8, b_+12);
  fadeout_white_tail(gb);
  ret_effect(gb);
}

static void fadein_white_tail(GB *gb) {
  BASE(fadeinFromWhite);
  CYC(b_+7, b_+10); W8(wPaletteThread_speed) = A;
  A = 0x20;
  CYC(b_+10, b_+15); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(b_+15, b_+18);
  make_all_palettes_use_fading(gb);
}

static void fadein_from_white(GB *gb) {
  BASE(fadeinFromWhite);
  A = 0x02;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+5, b_+7);
  fadein_white_tail(gb);
}

static void fast_fadein_from_white(GB *gb) {
  BASE(fastFadeinFromWhite);
  A = 0x02;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(b_+5, b_+9);
  fadein_white_tail(gb);
}

void fadeinFromWhite_hook(GB *gb) { fadein_from_white(gb); ret_effect(gb); }
void fastFadeinFromWhite_hook(GB *gb) { fast_fadein_from_white(gb); ret_effect(gb); }

void fadeinFromWhiteWithDelay_hook(GB *gb) {
  BASE(fadeinFromWhiteWithDelay);
  CYC(b_+0, b_+3);
  set_palette_thread_delay(gb);
  A = 0x0a;
  CYC(b_+3, b_+8); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+8, b_+12);
  fadein_white_tail(gb);
  ret_effect(gb);
}

static void fadeout_black_tail(GB *gb) {
  BASE(fadeoutToBlack);
  CYC(b_+7, b_+10); W8(wPaletteThread_speed) = A;
  alu_xor(gb, A);
  CYC(b_+10, b_+14); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(b_+14, b_+17);
  make_all_palettes_use_fading(gb);
}

void fadeoutToBlack_hook(GB *gb) {
  BASE(fadeoutToBlack);
  A = 0x03;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+5, b_+7);
  fadeout_black_tail(gb);
  ret_effect(gb);
}

void fastFadeoutToBlack_hook(GB *gb) {
  BASE(fastFadeoutToBlack);
  A = 0x03;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(b_+5, b_+9);
  fadeout_black_tail(gb);
  ret_effect(gb);
}

void fadeoutToBlackWithDelay_hook(GB *gb) {
  BASE(fadeoutToBlackWithDelay);
  CYC(b_+0, b_+3);
  set_palette_thread_delay(gb);
  A = 0x0b;
  CYC(b_+3, b_+8); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+8, b_+12);
  fadeout_black_tail(gb);
  ret_effect(gb);
}

static void fadein_black_tail(GB *gb) {
  BASE(fadeinFromBlack);
  CYC(b_+7, b_+10); W8(wPaletteThread_speed) = A;
  A = 0xe0;
  CYC(b_+10, b_+15); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(b_+15, b_+18);
  make_all_palettes_use_fading(gb);
}

void fadeinFromBlack_hook(GB *gb) {
  BASE(fadeinFromBlack);
  A = 0x04;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+5, b_+7);
  fadein_black_tail(gb);
  ret_effect(gb);
}

void fastFadeinFromBlack_hook(GB *gb) {
  BASE(fastFadeinFromBlack);
  A = 0x04;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(b_+5, b_+9);
  fadein_black_tail(gb);
  ret_effect(gb);
}

void fadeinFromBlackWithDelay_hook(GB *gb) {
  BASE(fadeinFromBlackWithDelay);
  CYC(b_+0, b_+3);
  set_palette_thread_delay(gb);
  A = 0x0c;
  CYC(b_+3, b_+8); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+8, b_+12);
  fadein_black_tail(gb);
  ret_effect(gb);
}

static void set_darkening_variables(GB *gb) {
  BASE(_setDarkeningVariables);
  CYC(b_+0, b_+3); W8(wPaletteThread_speed) = A;
  CYC(b_+3, b_+6); A = W8(wPaletteThread_parameter);
  CYC(b_+6, b_+9); mem_wr(gb, wPaletteFadeOffset, A);
  A = B;
  CYC(b_+9, b_+13); W8(wPaletteThread_parameter) = A;
  A = 0xfc;
  SET_HL(wDirtyFadeBgPalettes);
  CYC(b_+13, b_+19); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x00);
  L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x00);
  CYC(b_+25, b_+26);
}

void _setDarkeningVariables_hook(GB *gb) {
  set_darkening_variables(gb);
  ret_effect(gb);
}

static void darken_room_helper(GB *gb) {
  BASE(_darkenRoomHelper);
  A = 0x05;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+5, b_+7);
  set_darkening_variables(gb);
}

void _darkenRoomHelper_hook(GB *gb) { darken_room_helper(gb); ret_effect(gb); }
void darkenRoom_hook(GB *gb) {BASE(darkenRoom); B = 0xf0; CYC(b_+0, b_+2); darken_room_helper(gb); ret_effect(gb); }
void darkenRoomLightly_hook(GB *gb) {BASE(darkenRoomLightly); B = 0xf7; CYC(b_+0, b_+4); darken_room_helper(gb); ret_effect(gb); }

void darkenRoomWithSpeed_hook(GB *gb) {
  BASE(darkenRoomWithSpeed);
  B = 0xf0;
  CYC(b_+0, b_+5);
  set_darkening_variables(gb);
  A = 0x05;
  CYC(b_+5, b_+10); W8(wPaletteThread_mode) = A;
  CYC(b_+10, b_+11);
  ret_effect(gb);
}

static void brighten_room_helper(GB *gb) {
  BASE(_brightenRoomHelper);
  CYC(b_+0, b_+3);
  set_darkening_variables(gb);
  A = 0x06;
  CYC(b_+3, b_+8); W8(wPaletteThread_mode) = A;
  CYC(b_+8, b_+9);
}

void _brightenRoomHelper_hook(GB *gb) { brighten_room_helper(gb); ret_effect(gb); }
void brightenRoom_hook(GB *gb) {BASE(brightenRoom); B = 0x00; A = 0x01; CYC(b_+0, b_+4); brighten_room_helper(gb); ret_effect(gb); }
void brightenRoomLightly_hook(GB *gb) {BASE(brightenRoomLightly); B = 0xf7; A = 0x01; CYC(b_+0, b_+6); brighten_room_helper(gb); ret_effect(gb); }
void brightenRoomWithSpeed_hook(GB *gb) {BASE(brightenRoomWithSpeed); B = 0x00; CYC(b_+0, b_+4); brighten_room_helper(gb); ret_effect(gb); }

static void fadein_to_room_tail(GB *gb) {
  BASE(fastFadeinFromWhiteToRoom);
  A = 0x07;
  CYC(b_+8, b_+13); W8(wPaletteThread_mode) = A;
  CYC(b_+13, b_+14);
}

void fastFadeinFromWhiteToRoom_hook(GB *gb) {
  BASE(fastFadeinFromWhiteToRoom);
  CYC(b_+0, b_+3);
  fast_fadein_from_white(gb);
  A = 0x1e;
  CYC(b_+3, b_+8); mem_wr(gb, wPaletteFadeOffset, A);
  fadein_to_room_tail(gb);
  ret_effect(gb);
}

void fadeinFromWhiteToRoom_hook(GB *gb) {
  BASE(fadeinFromWhiteToRoom);
  CYC(b_+0, b_+3);
  fadein_from_white(gb);
  CYC(b_+3, b_+5);
  fadein_to_room_tail(gb);
  ret_effect(gb);
}

static void start_fade_between_two_palettes(GB *gb) {
  BASE(startFadeBetweenTwoPalettes);
  A = 0x08;
  CYC(b_+0, b_+5); W8(wPaletteThread_mode) = A;
  A = 0x20;
  CYC(b_+5, b_+10); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(b_+10, b_+11);
}

void startFadeBetweenTwoPalettes_hook(GB *gb) {
  start_fade_between_two_palettes(gb);
  ret_effect(gb);
}

// enemies and object tables

void setEnemyTargetToLinkPosition_hook(GB *gb) {
  BASE(setEnemyTargetToLinkPosition);
  CYC(b_+0, b_+3); A = W8(wLinkObjectIndex);
  H = A;
  L = OBJ_YH;
  CYC(b_+3, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+9); H8(hEnemyTargetY) = A;
  L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); H8(hEnemyTargetX) = A;
  CYC(b_+13, b_+16); A = W8(wScentSeedActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); ret_effect(gb); return; }
  CYC(b_+16, b_+18);
  L = OBJ_YH;
  CYC(b_+18, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+23); H8(hFFB2) = A;
  L = alu_inc8(gb, L);
  CYC(b_+23, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); H8(hFFB3) = A;
  CYC(b_+27, b_+28);
  ret_effect(gb);
}

void getEntryFromObjectTable2_hook(GB *gb) {
  BASE(getEntryFromObjectTable2);
  bank_push(gb, b_+0, 0x12);
  A = B;
  SET_HL(objectTable2_bank12);
  CYC(b_+10, b_+15);
  add_double_index_to_hl(gb);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+17, b_+18);
  bank_pop(gb, b_+18);
  CYC(b_+24, b_+25);
  ret_effect(gb);
}

void checkDungeonUsesToggleBlocks_hook(GB *gb) {
  BASE(checkDungeonUsesToggleBlocks);
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  SET_HL(dungeonsUsingToggleBlocks);
  CYC(b_+6, b_+12);
  check_flag(gb);
  ret_effect(gb);
}

void getIndexOfGashaSpotInRoom_hook(GB *gb) {
  BASE(getIndexOfGashaSpotInRoom);
  C = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, GV(0x02, 0x04));
  A = C;
  CYC(b_+11, b_+12);
  CALL_ROM(b_+12, ROM_b02_getIndexOfGashaSpotInRoom_body);
  SET_BC(AF);
  CYC(b_+15, b_+17);
  bank_pop(gb, b_+17);
  CYC(b_+23, b_+24);
  ret_effect(gb);
}

void getBlackTowerProgress_hook(GB *gb) {
  BASE(getBlackTowerProgress);
  uint16_t bc = BC;
  C = 0x02;
  CYC(b_+0, b_+6); A = mem_rd(gb, wGroup0RoomFlags + 0x90);
  alu_bit(gb, 6, A);
  if (!(F & FZ)) CYCT(b_+6, b_+10);
  else {
    C = alu_dec8(gb, C);
    CYC(b_+6, b_+14); A = mem_rd(gb, wGroup0RoomFlags + 0xba);
    alu_bit(gb, 6, A);
    if (!(F & FZ)) CYCT(b_+14, b_+18);
    else { CYC(b_+14, b_+19); C = alu_dec8(gb, C); }
  }
  A = C;
  SET_BC(bc);
  CYC(b_+19, b_+22);
  ret_effect(gb);
}

// tile mapping and the changed-tile queue

static void set_hl_to_tile_mapping_data(GB *gb) {
  BASE(setHlToTileMappingDataPlusATimes8);
  CYC(b_+0, b_+3);
  multiply_a_by_8(gb);
  SET_HL(w3TileMappingData);
  alu_add_hl(gb, BC);
  CYC(b_+3, b_+8);
}

void setHlToTileMappingDataPlusATimes8_hook(GB *gb) {
  set_hl_to_tile_mapping_data(gb);
  ret_effect(gb);
}

void getTileMappingData_hook(GB *gb) {
  BASE(getTileMappingData);
  C = A;
  CYC(b_+0, b_+3); uint8_t svbk = mem_rd(gb, IO_SVBK), f = F;
  CYC(b_+3, b_+8); mem_wr(gb, IO_SVBK, 0x03);
  A = C;
  CYC(b_+8, b_+12);
  set_hl_to_tile_mapping_data(gb);
  uint16_t de = DE;
  SET_DE(wTmpcec0);
  B = 0x08;
  CYC(b_+12, b_+21);
  copyMemory(gb, DE, HL, 8);
  SET_HL(HL + 8);
  SET_DE(de);
  CYC(b_+21, b_+25); A = mem_rd(gb, wTmpcec0 + 4);
  B = A;
  CYC(b_+25, b_+29); A = mem_rd(gb, wTmpcec0);
  C = A;
  A = svbk;
  F = f;
  CYC(b_+29, b_+33); mem_wr(gb, IO_SVBK, A);
  CYC(b_+33, b_+34);
  ret_effect(gb);
}

static void set_tile(GB *gb) {
  BASE(setTile);
  B = A;
  CYC(b_+0, b_+4); A = W8(wChangedTileQueueTail);
  A = alu_inc8(gb, A);
  alu_and(gb, 0x1f);
  E = A;
  CYC(b_+4, b_+11); A = W8(wChangedTileQueueHead);
  alu_cp(gb, E);
  if (F & FZ) { CYCT(b_+11, b_+13); return; }
  CYC(b_+11, b_+13);
  A = E;
  CYC(b_+13, b_+17); W8(wChangedTileQueueTail) = A;
  CYC(b_+17, b_+19); uint8_t svbk = mem_rd(gb, IO_SVBK), f = F;
  CYC(b_+19, b_+24); mem_wr(gb, IO_SVBK, 0x02);
  A = E;
  alu_add(gb, A);
  SET_HL(w2ChangedTileQueue);
  CYC(b_+24, b_+30);
  add_a_to_hl(gb);
  CYC(b_+30, b_+31); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); mem_wr(gb, HL, C);
  A = B;
  CYC(b_+33, b_+37);
  set_tile_without_gfx_reload(gb);
  A = svbk;
  F = f;
  CYC(b_+37, b_+40); mem_wr(gb, IO_SVBK, A);
  alu_or(gb, H);
  CYC(b_+40, b_+42);
}

void setTile_hook(GB *gb) {
  set_tile(gb);
  ret_effect(gb);
}

void setTileInAllBuffers_hook(GB *gb) {
  BASE(setTileInAllBuffers);
  E = A;
  B = A;
  CYC(b_+0, b_+5);
  set_tile_in_room_layout_buffer(gb);
  A = E;
  CYC(b_+5, b_+9);
  set_tile(gb);
  ret_effect(gb);
}

static void set_interleaved_tile(GB *gb, uint16_t sp0_) {
  BASE(setInterleavedTile);
  uint16_t de = DE;
  E = A;
  CYC(b_+0, b_+4); C = mem_rd(gb, IO_SVBK);
  CYC(b_+4, b_+7); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(b_+7, b_+13); H8(hRomBank) = 0x04;
  CYC(b_+13, b_+16); mem_wr(gb, MBC_ROM_BANK, 0x04);
  A = E;
  CYC(b_+16, b_+17);
  CALL_C(b_+17, setInterleavedTile_body_hook, SYM(setInterleavedTile_body), b_+20);
  SET_BC(bc);
  A = B;
  CYC(b_+20, b_+24); H8(hRomBank) = A;
  CYC(b_+24, b_+27); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(b_+27, b_+30); mem_wr(gb, IO_SVBK, A);
  SET_DE(de);
  CYC(b_+30, b_+32);
}

void setInterleavedTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  set_interleaved_tile(gb, sp0_);
  ret_effect(gb);
}

// interaction slots

static void get_free_interaction_slot(GB *gb) {
  BASE(getFreeInteractionSlot);
  SET_HL(wInteractionSlots);
  CYC(b_+0, b_+3);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+4, b_+7);
      CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      L = alu_inc8(gb, L);
      alu_xor(gb, A);
      CYC(b_+16, b_+19);
      break;
    }
    CYC(b_+4, b_+7);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+7, b_+13); continue; }
    CYC(b_+7, b_+13);
    alu_or(gb, H);
    CYC(b_+13, b_+15);
    break;
  }
}

void getFreeInteractionSlot_hook(GB *gb) {
  get_free_interaction_slot(gb);
  ret_effect(gb);
}

static void interaction_delete(GB *gb) {
  BASE(interactionDelete);
  H = D;
  L = INTERACTION_BASE;
  B = 0x10;
  alu_xor(gb, A);
  CYC(b_+0, b_+6);
  do {
    CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+10, b_+13); else CYC(b_+10, b_+13);
  } while (B);
  CYC(b_+13, b_+14);
}

void interactionDelete_hook(GB *gb) {
  interaction_delete(gb);
  ret_effect(gb);
}

void interactionDeleteAndUnmarkSolidPosition_hook(GB *gb) {
  BASE(interactionDeleteAndUnmarkSolidPosition);
  CYC(b_+0, b_+3);
  unmark_solid_position(gb);
  interaction_delete(gb);
  ret_effect(gb);
}

void checkNpcShouldExistAtGameStage_hook(GB *gb) {
  BASE(checkNpcShouldExistAtGameStage);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  bank_push(gb, b_+2, 0x09);
  CYC(b_+12, b_+14); A = H8(hFF8B);
  CALL_ROM(b_+14, ROM_b09_checkNpcShouldExistAtGameStage_body);
  C = 0x00;
  if (F & FZ) CYCT(b_+17, b_+21);
  else { CYC(b_+17, b_+22); C = alu_inc8(gb, C); }
  bank_pop(gb, b_+22);
  A = C;
  alu_or(gb, A);
  CYC(b_+28, b_+31);
  ret_effect(gb);
}

void interactionSetSimpleScript_hook(GB *gb) {
  BASE(interactionSetSimpleScript);
  E = 0x58;
  A = L;
  CYC(b_+0, b_+4); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+4, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8);
  ret_effect(gb);
}

static void generate_vram_tiles_with_room_changes(GB *gb, uint16_t sp0_);

// screen music and room packs (bank 4 tables)

static void load_screen_music(GB *gb) {
  BASE(loadScreenMusic);
  bank_push(gb, b_+0, 0x04);
  CYC(b_+10, b_+13); A = W8(wActiveGroup);
  SET_HL(musicAssignmentGroupTable_bank04);
  CYC(b_+13, b_+17);
  add_double_index_to_hl(gb);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+19, b_+23); A = W8(wActiveRoom);
  CYC(b_+23, b_+24);
  add_a_to_hl(gb);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+28); W8(wActiveMusic2) = A;
  CYC(b_+28, b_+31); A = W8(wActiveGroup);
  alu_cp(gb, 0x02);
  if (!(F & FC)) CYCT(b_+31, b_+35);
  else {
    CYC(b_+31, b_+35);
    B = A;
    CYC(b_+35, b_+39); A = W8(wActiveRoom);
    C = A;
    SET_HL(roomPackData_bank04);
    alu_add_hl(gb, BC);
    CYC(b_+39, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+45, b_+48); W8(wLoadingRoomPack) = A;
  }
  bank_pop(gb, b_+48);
  CYC(b_+54, b_+55);
}

void loadScreenMusic_hook(GB *gb) {
  load_screen_music(gb);
  ret_effect(gb);
}

void applyWarpDest_hook(GB *gb) {
  BASE(applyWarpDest);
  bank_push(gb, b_+0, 0x04);
  CALL_ROM(b_+10, ROM_b04_applyWarpDest_b04);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

static void load_screen_music_and_set_room_pack(GB *gb) {
  BASE(loadScreenMusicAndSetRoomPack);
  CYC(b_+0, b_+3);
  load_screen_music(gb);
  CYC(b_+3, b_+6); A = W8(wActiveRoom);
  CYC(b_+6, b_+9); W8(wLoadingRoom) = A;
  CYC(b_+9, b_+12); A = W8(wActiveGroup);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); return; }
  CYC(b_+12, b_+17); A = W8(wLoadingRoomPack);
  alu_and(gb, 0x7f);
  CYC(b_+17, b_+22); W8(wRoomPack) = A;
  CYC(b_+22, b_+23);
}

void loadScreenMusicAndSetRoomPack_hook(GB *gb) {
  load_screen_music_and_set_room_pack(gb);
  ret_effect(gb);
}

void dismountCompanionAndSetRememberedPositionToScreenCenter_hook(GB *gb) {
  BASE(dismountCompanionAndSetRememberedPositionToScreenCenter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, 0x05);
  SET_DE(w1Companion);
  A = E;
  CYC(b_+10, b_+16); H8(hActiveObjectType) = A;
  A = D;
  CYC(b_+16, b_+19); H8(hActiveObject) = A;
  CALL_C(b_+19, companionDismount_hook, SYM(companionDismount), b_+22);
  CALL_C(b_+22, saveLinkLocalRespawnAndCompanionPosition_hook, SYM(saveLinkLocalRespawnAndCompanionPosition), b_+25);
  A = 0x38;
  CYC(b_+25, b_+30); W8(wRememberedCompanionY) = A;
  A = 0x50;
  CYC(b_+30, b_+35); W8(wRememberedCompanionX) = A;
  bank_pop(gb, b_+35);
  CYC(b_+41, b_+42);
  ret_effect(gb);
}

// memory clears

static void clear_block(GB *gb, uint16_t dst, unsigned n) {
  clearMemory(gb, dst, n);
  SET_HL((uint16_t)(dst + n));
  B = 0;
  A = 0;
  F = FZ | FN;
}

static void clear_wram_bank1(GB *gb) {
  BASE(clearWramBank1);
  alu_xor(gb, A);
  CYC(b_+0, b_+3); mem_wr(gb, IO_SVBK, A);
  SET_HL(w1Link);
  SET_BC(0x1000);
  CYC(b_+3, b_+12);
  clearMemoryBc(gb, HL, 0x1000);
  SET_HL(HL + 0x1000);
  SET_BC(0);
  A = 0;
  E = 0;
  F = FZ;
}

void clearWramBank1_hook(GB *gb) {
  clear_wram_bank1(gb);
  ret_effect(gb);
}

static void clear_screen_variables(GB *gb) {
  BASE(clearScreenVariables);
  SET_HL(wScreenVariables);
  B = 0x30;
  CYC(b_+0, b_+8);
  clear_block(gb, HL, 0x30);
  A = 0xff;
  CYC(b_+8, b_+13); W8(wLoadedTilesetUniqueGfx) = A;
  CYC(b_+13, b_+16); W8(wLoadedTilesetLayout) = A;
  CYC(b_+16, b_+19); W8(wLoadedTilesetAnimation) = A;
  CYC(b_+19, b_+20);
}

void clearScreenVariables_hook(GB *gb) {
  clear_screen_variables(gb);
  ret_effect(gb);
}

void clearScreenVariablesAndWramBank1_hook(GB *gb) {
  BASE(clearScreenVariablesAndWramBank1);
  CYC(b_+0, b_+3);
  clear_wram_bank1(gb);
  clear_screen_variables(gb);
  ret_effect(gb);
}

void clearLinkObject_hook(GB *gb) {
  BASE(clearLinkObject);
  SET_HL(w1Link);
  B = 0x40;
  CYC(b_+0, b_+8);
  clear_block(gb, HL, 0x40);
  ret_effect(gb);
}

static void clear_reserved_interaction1(GB *gb) {
  BASE(clearReservedInteraction1);
  SET_HL(w1ReservedInteraction0 + 0x100);
  B = 0x40;
  CYC(b_+0, b_+8);
  clear_block(gb, HL, 0x40);
}

void clearReservedInteraction1_hook(GB *gb) {
  clear_reserved_interaction1(gb);
  ret_effect(gb);
}

void clearReservedInteraction0_hook(GB *gb) {
  BASE(clearReservedInteraction0);
  SET_HL(w1ReservedInteraction0);
  B = 0x40;
  CYC(b_+0, b_+8);
  clear_block(gb, HL, 0x40);
  clear_reserved_interaction1(gb);
  ret_effect(gb);
}

static void clear_object_slots(GB *gb, uint16_t base, uint16_t first) {
  SET_DE(first);
  CYC(base, base + 3);
  for (;;) {
    H = D;
    L = E;
    B = 0x40;
    CYC(base + 3, base + 10);
    clear_block(gb, HL, 0x40);
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(base + 10, base + 16); continue; }
    CYC(base + 10, base + 17);
    break;
  }
}

void clearDynamicInteractions_hook(GB *gb) { clear_object_slots(gb, SYM(clearDynamicInteractions), wInteractionSlots); ret_effect(gb); }
void clearItems_hook(GB *gb) { clear_object_slots(gb, SYM(clearItems), w1WeaponItem); ret_effect(gb); }
void clearEnemies_hook(GB *gb) { clear_object_slots(gb, SYM(clearEnemies), ENEMY_SLOTS); ret_effect(gb); }
void clearParts_hook(GB *gb) { clear_object_slots(gb, SYM(clearParts), PART_SLOTS); ret_effect(gb); }

// tileset animation data (bank 4 tables)

static void load_animation_pointer(GB *gb) {
  BASE(loadAnimationData);
  uint16_t hl = HL;
  CYC(b_+63, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+66); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+66, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  SET_DE(DE + 1);
  A = L;
  CYC(b_+69, b_+72); mem_wr(gb, DE, A);
  SET_DE(DE + 1);
  A = H;
  CYC(b_+72, b_+75); mem_wr(gb, DE, A);
  SET_HL((uint16_t)(hl + 2));
  CYC(b_+75, b_+79);
}

static void load_animation_data(GB *gb) {
  BASE(loadAnimationData);
  B = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, 0x04);
  A = B;
  SET_HL(animationGroupTable_bank04);
  CYC(b_+11, b_+16);
  add_double_index_to_hl(gb);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+18, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+23); W8(wAnimationState) = A;
  uint16_t de = DE;
  SET_DE(wAnimationCounter1);
  CYC(b_+23, b_+30);
  load_animation_pointer(gb);
  SET_DE(wAnimationCounter2);
  CYC(b_+30, b_+36);
  load_animation_pointer(gb);
  SET_DE(wAnimationCounter3);
  CYC(b_+36, b_+42);
  load_animation_pointer(gb);
  SET_DE(wAnimationCounter4);
  CYC(b_+42, b_+48);
  load_animation_pointer(gb);
  SET_DE(de);
  CYC(b_+48, b_+49);
  bank_pop(gb, b_+49);
  alu_xor(gb, A);
  CYC(b_+55, b_+59); W8(wAnimationQueueHead) = A;
  CYC(b_+59, b_+62); W8(wAnimationQueueTail) = A;
  CYC(b_+62, b_+63);
}

void loadAnimationData_hook(GB *gb) {
  load_animation_data(gb);
  ret_effect(gb);
}

void forceLoadRoom_hook(GB *gb) {
  BASE(forceLoadRoom);
  uint16_t sp0_ = gb->sp;
  alu_and(gb, 0x03);
  CYC(b_+0, b_+5); W8(wRoomStateModifier) = A;
  A = B;
  CYC(b_+5, b_+9); W8(wActiveGroup) = A;
  A = C;
  CYC(b_+9, b_+13); W8(wActiveRoom) = A;
  CYC(b_+13, b_+16);
  load_screen_music_and_set_room_pack(gb);
  CALL_ROM(b_+16, ROM_loadTilesetData);
  CALL_ROM(b_+19, ROM_loadTilesetGraphics);
  CALL_ROM(b_+22, ROM_loadTilesetAndRoomLayout);
  CYC(b_+25, b_+28);
  generate_vram_tiles_with_room_changes(gb, sp0_);
  ret_effect(gb);
}

// unique graphics headers (bank 4 table)

void loadUniqueGfxHeader_hook(GB *gb) {
  BASE(loadUniqueGfxHeader);
  alu_and(gb, 0x7f);
  B = A;
  CYC(b_+0, b_+3);
  bank_push(gb, b_+3, 0x04);
  A = B;
  SET_HL(uniqueGfxHeaderTable_bank04);
  CYC(b_+13, b_+18);
  add_double_index_to_hl(gb);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+22); mem_wr(gb, wUniqueGfxHeaderAddress, A);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+26); mem_wr(gb, wUniqueGfxHeaderAddress + 1, A);
  bank_pop(gb, b_+26);
  CYC(b_+32, b_+33);
  ret_effect(gb);
}

void updateTilesetUniqueGfx_hook(GB *gb) {
  BASE(updateTilesetUniqueGfx);
  CYC(b_+0, b_+3); A = W8(wTilesetUniqueGfx);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+5);
  B = A;
  CYC(b_+5, b_+9); A = W8(wLoadedTilesetUniqueGfx);
  alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+9, b_+11); ret_effect(gb); return; }
  CYC(b_+9, b_+13); A = H8(hRomBank);
  CYC(b_+13, b_+14); push_effect(gb, AF);
  SET_HL(wUniqueGfxHeaderAddress);
  CYC(b_+14, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+19, b_+24); H8(hRomBank) = 0x04;
  CYC(b_+24, b_+27); mem_wr(gb, MBC_ROM_BANK, 0x04);
  CALL_ROM(b_+27, ROM_loadUniqueGfxHeaderEntry);
  C = A;
  A = L;
  CYC(b_+30, b_+35); mem_wr(gb, wUniqueGfxHeaderAddress, A);
  A = H;
  CYC(b_+35, b_+39); mem_wr(gb, wUniqueGfxHeaderAddress + 1, A);
  CYC(b_+39, b_+40); SET_AF(pop_effect(gb));
  CYC(b_+40, b_+42); H8(hRomBank) = A;
  CYC(b_+42, b_+45); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  alu_add(gb, A);
  CYC(b_+45, b_+48);
  ret_effect(gb);
}

// unique gfx header entries (bank 4 headers, decompressed into w3VramTiles)

static void load_unique_gfx_header_entry(GB *gb) {
  BASE(loadUniqueGfxHeaderEntry);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+1, b_+4);
    uint16_t hl = HL;
    CYC(b_+60, b_+62); A = mem_rd(gb, HL);
    alu_and(gb, 0x7f);
    CYC(b_+62, b_+64);
    CALL_ROM(b_+64, ROM_loadPaletteHeader);
    SET_HL(hl);
    CYC(b_+67, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+69, b_+70);
    return;
  }
  CYC(b_+1, b_+4);
  C = A;
  CYC(b_+4, b_+7); H8(hFF8C) = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(b_+8, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+10, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  D = A;
  CYC(b_+12, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(b_+14, b_+16); A = mem_rd(gb, HL);
  alu_and(gb, 0x7f);
  CYC(b_+16, b_+20); H8(hFF8D) = A;
  uint16_t hl = HL, de = DE;
  L = C;
  H = B;
  B = A;
  CYC(b_+20, b_+27); A = H8(hFF8C);
  C = A;
  SET_DE(w3VramTiles + 7);
  CYC(b_+27, b_+31);
  CALL_ROM(b_+31, ROM_decompressGraphics);
  SET_DE(de);
  SET_HL(w3VramTiles);
  C = 0x07;
  CYC(b_+34, b_+42); A = H8(hFF8D);
  B = A;
  CYC(b_+42, b_+43);
  CALL_ROM(b_+43, ROM_queueDmaTransfer);
  SET_HL(hl);
  A = 0x00;
  CYC(b_+46, b_+51); mem_wr(gb, IO_SVBK, A);
  A = 0x04;
  CYC(b_+51, b_+55); H8(hRomBank) = A;
  CYC(b_+55, b_+58); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+60);
}

void loadUniqueGfxHeaderEntry_hook(GB *gb) {
  load_unique_gfx_header_entry(gb);
  ret_effect(gb);
}

void uniqueGfxFunc_380b_hook(GB *gb) {
  BASE(uniqueGfxFunc_380b);
  B = A;
  CYC(b_+0, b_+1);
  bank_push(gb, b_+1, 0x04);
  A = B;
  SET_HL(uniqueGfxHeaderTable_bank04);
  CYC(b_+11, b_+16);
  add_double_index_to_hl(gb);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+18, b_+22);
  load_unique_gfx_header_entry(gb);
  bank_pop(gb, b_+22);
  CYC(b_+28, b_+29);
  ret_effect(gb);
}

static void generate_vram_tiles_with_room_changes(GB *gb, uint16_t sp0_) {
  BASE(generateVramTilesWithRoomChanges);
  CYC(b_+0, b_+2); C = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+5); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(b_+5, b_+11); H8(hRomBank) = 0x04;
  CYC(b_+11, b_+14); mem_wr(gb, MBC_ROM_BANK, 0x04);
  CALL_C(b_+14, generateW3VramTilesAndAttributes_hook, SYM(generateW3VramTilesAndAttributes), b_+17);
  SET_HL(ROM_b02_applyRoomSpecificTileChangesAfterGfxLoad);
  E = 0x02;
  CYC(b_+17, b_+22);
  CALL_ROM(b_+22, ROM_interBankCall);
  SET_BC(bc);
  A = B;
  CYC(b_+25, b_+29); H8(hRomBank) = A;
  CYC(b_+29, b_+32); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(b_+32, b_+35); mem_wr(gb, IO_SVBK, A);
  CYC(b_+35, b_+36);
}

void generateVramTilesWithRoomChanges_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  generate_vram_tiles_with_room_changes(gb, sp0_);
  ret_effect(gb);
}

// simple scripts (bank $0c): commands 0 to 4 through the rst $00 jump table at $3dd7

static void simple_script_run_command(GB *gb, uint16_t sp0_) {
  BASE(interactionRunSimpleScript);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); push_effect(gb, HL);
  CYC(b_+46, b_+47); push_effect(gb, simpleScriptCommandTable);
  CYC(0x0000, 0x0001); alu_add(gb, A);
  CYC(0x0001, 0x0002); SET_HL(pop_effect(gb));
  CYC(0x0002, 0x0004); alu_add(gb, L); L = A;
  if (!(F & FC)) CYCT(0x0004, 0x0006);
  else { CYC(0x0004, 0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007, 0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0008, 0x0009); H = mem_rd(gb, HL);
  L = A;
  CYC(0x0009, 0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+57) {
    CYC(b_+57, b_+58); SET_HL(pop_effect(gb));
    CYC(b_+58, b_+59);
    break;
    }
    else if (jt_ == b_+59) {
    CYC(b_+59, b_+60); SET_HL(pop_effect(gb));
    CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = INTERACTION_BASE + OBJ_COUNTER1;
    CYC(b_+61, b_+64); mem_wr(gb, DE, A);
    alu_xor(gb, A);
    CYC(b_+64, b_+66);
    break;
    }
    else if (jt_ == b_+66) {
{
    CYC(b_+66, b_+67); SET_HL(pop_effect(gb));
    CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    CYC(b_+68, b_+69);
    CALL_ROM(b_+69, ROM_playSound);
    SET_HL(hl);
    CYC(b_+72, b_+74);
    break;
  }
    }
    else if (jt_ == b_+74) {
{
    CYC(b_+74, b_+75); SET_HL(pop_effect(gb));
    CYC(b_+75, b_+76); A = mem_rd(gb, HL); SET_HL(HL + 1);
    C = A;
    CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    CYC(b_+77, b_+82);
    set_tile(gb);
    SET_HL(hl);
    alu_scf(gb);
    CYC(b_+82, b_+85);
    break;
  }
    }
    else {
{
    CYC(b_+85, b_+86); SET_HL(pop_effect(gb));
    CYC(b_+86, b_+87); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+87, b_+89); H8(hFF8C) = A;
    CYC(b_+89, b_+90); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+90, b_+92); H8(hFF8F) = A;
    CYC(b_+92, b_+93); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+93, b_+95); H8(hFF8E) = A;
    CYC(b_+95, b_+96); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    CYC(b_+96, b_+100);
    set_interleaved_tile(gb, sp0_);
    SET_HL(hl);
    alu_scf(gb);
    CYC(b_+100, b_+103);
    break;
  }
    }
  } while (0);
}

void interactionRunSimpleScript_hook(GB *gb) {
  BASE(interactionRunSimpleScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, GV(0x0c, 0x14));
  H = D;
  L = 0x58;
  CYC(b_+10, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+15, b_+16);
  for (;;) {
    CYC(b_+16, b_+17); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+17, b_+20);
      bank_pop(gb, b_+36);
      alu_scf(gb);
      CYC(b_+42, b_+44);
      break;
    }
    CYC(b_+17, b_+20);
    CYC(b_+20, b_+23);
    simple_script_run_command(gb, sp0_);
    if (F & FC) { CYCT(b_+23, b_+25); continue; }
    CYC(b_+23, b_+25);
    CYC(b_+25, b_+28);
    E = 0x58;
    A = L;
    CYC(SYM(interactionSetSimpleScript), (SYM(interactionSetSimpleScript) + 4)); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    A = H;
    CYC((SYM(interactionSetSimpleScript) + 4), (SYM(interactionSetSimpleScript) + 7)); mem_wr(gb, DE, A);
    CYC((SYM(interactionSetSimpleScript) + 7), (SYM(interactionSetSimpleScript) + 8));
    bank_pop(gb, b_+28);
    alu_xor(gb, A);
    CYC(b_+34, b_+36);
    break;
  }
  ret_effect(gb);
}

// decorative interactions

void objectCreateSparkle_hook(GB *gb) {
  BASE(objectCreateSparkle);
  CYC(b_+0, b_+3);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+6); mem_wr(gb, HL, 0x84);
  L = alu_inc8(gb, L);
  CYC(b_+6, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+12);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void objectCreateSparkleMovingUp_hook(GB *gb) {
  BASE(objectCreateSparkleMovingUp);
  CYC(b_+0, b_+3);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+6); mem_wr(gb, HL, 0x84);
  L = alu_inc8(gb, L);
  CYC(b_+6, b_+9); mem_wr(gb, HL, 0x02);
  L = 0x50;
  CYC(b_+9, b_+13); mem_wr(gb, HL, 0x80);
  L = alu_inc8(gb, L);
  CYC(b_+13, b_+16); mem_wr(gb, HL, 0xff);
  CYC(b_+16, b_+19);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void objectCreateRedBlueOrb_hook(GB *gb) {
  BASE(objectCreateRedBlueOrb);
  CYC(b_+0, b_+3);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+6); mem_wr(gb, HL, 0x84);
  L = alu_inc8(gb, L);
  CYC(b_+6, b_+9); mem_wr(gb, HL, 0x04);
  CYC(b_+9, b_+12);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void incMakuTreeState_hook(GB *gb) {
  BASE(incMakuTreeState);
  CYC(b_+0, b_+3); A = W8(wMakuTreeState);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  alu_cp(gb, 0x11);
  if (F & FC) CYCT(b_+4, b_+8);
  else { CYC(b_+4, b_+10); A = 0x10; }
  CYC(b_+10, b_+13); W8(wMakuTreeState) = A;
  CYC(b_+13, b_+14);
  ret_effect(gb);
}

void setLinkDirection_hook(GB *gb) {
  BASE(setLinkDirection);
  B = A;
  CYC(b_+0, b_+4); A = W8(wLinkObjectIndex);
  H = A;
  L = 0x08;
  CYC(b_+4, b_+8); mem_wr(gb, HL, B);
  H = w1Link >> 8;
  CYC(b_+8, b_+11); mem_wr(gb, HL, B);
  CYC(b_+11, b_+12);
  ret_effect(gb);
}

void interactionFunc_3e6d_hook(GB *gb) {
  BASE(interactionFunc_3e6d);
  uint16_t de = DE;
  L = 0x43;
  CYC(b_+0, b_+4); E = mem_rd(gb, HL);
  bank_push(gb, b_+4, GV(0x16, 0x14));
  A = E;
  SET_HL(creditsSpriteTable_bank16);
  CYC(b_+14, b_+19);
  add_double_index_to_hl(gb);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+21, b_+22);
  CALL_ROM(b_+22, ROM_addSpritesToOam_withOffset);
  bank_pop(gb, b_+25);
  SET_DE(de);
  CYC(b_+31, b_+33);
  ret_effect(gb);
}

// parts

void getFreePartSlot_hook(GB *gb) {
  BASE(getFreePartSlot);
  SET_HL(PART_SLOTS);
  CYC(b_+0, b_+3);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+4, b_+7);
      A = alu_inc8(gb, A);
      CYC(b_+15, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
      alu_xor(gb, A);
      CYC(b_+17, b_+19);
      break;
    }
    CYC(b_+4, b_+7);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+7, b_+13); continue; }
    CYC(b_+7, b_+13);
    alu_or(gb, H);
    CYC(b_+13, b_+15);
    break;
  }
  ret_effect(gb);
}

void partDelete_hook(GB *gb) {
  BASE(partDelete);
  H = D;
  L = 0xc0;
  B = 0x10;
  alu_xor(gb, A);
  CYC(b_+0, b_+6);
  do {
    CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+10, b_+13); else CYC(b_+10, b_+13);
  } while (B);
  CYC(b_+13, b_+14);
  ret_effect(gb);
}

void checkLinkCanSurface_hook(GB *gb) {
  BASE(checkLinkCanSurface);
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  SET_HL(ROM_b12_checkLinkCanSurface_isUnderwater);
  E = 0x12;
  CYC(b_+6, b_+11);
  CALL_ROM(b_+11, ROM_interBankCall);
  C = alu_srl(gb, C);
  CYC(b_+14, b_+17);
  ret_effect(gb);
}

void copy256BytesFromBank_hook(GB *gb) {
  BASE(copy256BytesFromBank);
  A = E;
  CYC(b_+0, b_+3); mem_wr(gb, IO_SVBK, A);
  A = C;
  CYC(b_+3, b_+6); H8(hRomBank) = A;
  CYC(b_+6, b_+9); mem_wr(gb, MBC_ROM_BANK, A);
  E = 0x00;
  B = 0x00;
  CYC(b_+9, b_+16);
  copy_loop(gb, SYM(copyMemory), DE, HL, 256);
  copy_done(gb, HL, 256);
}

// movement scripts (bank $0e bodies)

void objectLoadMovementScript_hook(GB *gb) {
  BASE(objectLoadMovementScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, GV(0x0e, 0x0d));
  CALL_C(b_+10, objectLoadMovementScript_body_hook, SYM(objectLoadMovementScript_body), b_+13);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void objectRunMovementScript_hook(GB *gb) {
  BASE(objectRunMovementScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, GV(0x0e, 0x0d));
  CALL_C(b_+10, objectRunMovementScript_body_hook, SYM(objectRunMovementScript_body), b_+13);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void decCbb3_hook(GB *gb) {
  BASE(decCbb3);
  SET_HL(wFakeResetMenu);
  CYC(b_+0, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

void incCbc1_hook(GB *gb) {
  BASE(incCbc1);
  SET_HL(wGenericCutscene_cbc1);
  CYC(b_+0, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

void incCbc2_hook(GB *gb) {
  BASE(incCbc2);
  SET_HL(wGenericCutscene_cbc2);
  CYC(b_+0, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

static void add_sprites_from_bank(GB *gb, uint16_t a, uint16_t target) {
  CYC(a, a + 2); A = H8(hRomBank);
  CYC(a + 2, a + 3); push_effect(gb, AF);
  A = E;
  CYC(a + 3, a + 6); H8(hRomBank) = A;
  CYC(a + 6, a + 9); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_ROM(a + 9, target);
  CYC(a + 12, a + 13); SET_AF(pop_effect(gb));
  CYC(a + 13, a + 15); H8(hRomBank) = A;
  CYC(a + 15, a + 18); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(a + 18, a + 19);
}

void addSpritesFromBankToOam_withOffset_hook(GB *gb) {
  add_sprites_from_bank(gb, SYM(addSpritesFromBankToOam_withOffset), ROM_addSpritesToOam_withOffset);
  ret_effect(gb);
}

void addSpritesFromBankToOam_hook(GB *gb) {
  add_sprites_from_bank(gb, SYM(addSpritesFromBankToOam), ROM_addSpritesToOam);
  ret_effect(gb);
}

// input

void pollInput_hook(GB *gb) {
  BASE(pollInput);
  C = 0x00;
  A = 0x20;
  CYC(b_+0, b_+5); mem_wr(gb, IO_P1, A);
  CYC(b_+5, b_+6); A = mem_rd(gb, IO_P1);
  CYC(b_+6, b_+7); A = mem_rd(gb, IO_P1);
  CYC(b_+7, b_+8); A = mem_rd(gb, IO_P1);
  B = A;
  A = 0x10;
  CYC(b_+8, b_+12); mem_wr(gb, IO_P1, A);
  A = B;
  alu_and(gb, 0x0f);
  alu_swap_a(gb);
  B = A;
  SET_HL(wKeysPressed);
  CYC(b_+12, b_+22); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_cpl(gb);
  CYC(b_+23, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+26); A = mem_rd(gb, IO_P1);
  CYC(b_+26, b_+27); A = mem_rd(gb, IO_P1);
  alu_and(gb, 0x0f);
  alu_or(gb, B);
  alu_cpl(gb);
  CYC(b_+27, b_+32); B = mem_rd(gb, HL);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_and(gb, B);
  CYC(b_+33, b_+35); mem_wr(gb, HL, A);
  A = 0x30;
  CYC(b_+35, b_+38); mem_wr(gb, IO_P1, A);
  CYC(b_+38, b_+39);
  ret_effect(gb);
}

void getInputWithAutofire_hook(GB *gb) {
  BASE(getInputWithAutofire);
  uint16_t hl = HL, bc = BC;
  CYC(b_+0, b_+5); A = W8(wKeysPressed);
  alu_and(gb, 0xf0);
  B = A;
  SET_HL(wAutoFireKeysPressed);
  CYC(b_+5, b_+12); A = mem_rd(gb, HL);
  alu_and(gb, B);
  A = B;
  CYC(b_+12, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  bool just_pressed;
  if (F & FZ) {
    CYCT(b_+15, b_+17);
    alu_xor(gb, A);
    CYC(b_+37, b_+39); mem_wr(gb, HL, A);
    just_pressed = true;
  } else {
    CYC(b_+15, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+18, b_+19); A = mem_rd(gb, HL);
    alu_cp(gb, 0x28);
    if (F & FC) { CYCT(b_+19, b_+23); just_pressed = true; }
    else {
      CYC(b_+19, b_+23);
      alu_and(gb, 0x1f);
      alu_or(gb, 0x80);
      CYC(b_+23, b_+28); mem_wr(gb, HL, A);
      alu_and(gb, 0x03);
      if (!(F & FZ)) { CYCT(b_+28, b_+32); just_pressed = true; }
      else {
        CYC(b_+28, b_+35); A = W8(wKeysPressed);
        CYC(b_+35, b_+37);
        just_pressed = false;
      }
    }
  }
  if (just_pressed) { CYC(b_+39, b_+42); A = W8(wKeysJustPressed); }
  SET_BC(bc);
  SET_HL(hl);
  CYC(b_+42, b_+45);
  ret_effect(gb);
}

// graphics register states

void loadGfxRegisterStateIndex_hook(GB *gb) {
  BASE(loadGfxRegisterStateIndex);
  L = A;
  alu_add(gb, A);
  alu_add(gb, L);
  alu_add(gb, A);
  SET_HL(gfxRegisterStates);
  CYC(b_+0, b_+8);
  add_double_index_to_hl(gb);
  B = 0x0c;
  SET_DE(wGfxRegs1);
  CYC(b_+8, b_+13);
  do {
    CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+14, b_+15); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    B = alu_dec8(gb, B);
    if (B) CYCT(b_+15, b_+19); else CYC(b_+15, b_+19);
  } while (B);
  CYC(b_+19, b_+22); A = W8(wGfxRegs1);
  CYC(b_+22, b_+25); W8(wGfxRegsFinal) = A;
  CYC(b_+25, b_+27); mem_wr(gb, IO_LCDC, A);
  CYC(b_+27, b_+28);
  ret_effect(gb);
}

// DMA transfers: queued for VBlank while the LCD is on, done at once (GDMA) while it is off

static void queue_dma_transfer(GB *gb) {
  BASE(queueDmaTransfer);
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_LCDC);
  alu_rlca(gb);
  if (F & FC) {
    CYC(b_+2, b_+5);
    uint16_t de = DE, hl = HL;
    H = 0xc4;
    CYC(b_+5, b_+11); A = H8(hVBlankFunctionQueueTail);
    L = A;
    CYC(b_+11, b_+15); A = mem_rd(gb, vblankDmaFunctionOffset);
    CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = C;
    CYC(b_+16, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
    SET_DE(hl);
    A = D;
    CYC(b_+18, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = E;
    CYC(b_+21, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
    SET_DE(de);
    A = E;
    CYC(b_+23, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = D;
    CYC(b_+26, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = E;
    CYC(b_+28, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = B;
    CYC(b_+30, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = L;
    CYC(b_+32, b_+35); H8(hVBlankFunctionQueueTail) = A;
    alu_scf(gb);
    CYC(b_+35, b_+37);
    return;
  }
  CYCT(b_+2, b_+5);
  CYC(b_+37, b_+39); A = H8(hRomBank);
  CYC(b_+39, b_+40); push_effect(gb, AF);
  CYC(b_+40, b_+42); A = mem_rd(gb, IO_SVBK);
  CYC(b_+42, b_+43); push_effect(gb, AF);
  CYC(b_+43, b_+44); push_effect(gb, DE);
  CYC(b_+44, b_+45); push_effect(gb, HL);
  A = C;
  CYC(b_+45, b_+48); mem_wr(gb, IO_SVBK, A);
  CYC(b_+48, b_+50); H8(hRomBank) = A;
  CYC(b_+50, b_+53); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+53, b_+54); SET_DE(pop_effect(gb));
  SET_HL(IO_HDMA1);
  CYC(b_+54, b_+58); mem_wr(gb, HL, D);
  L = alu_inc8(gb, L);
  CYC(b_+58, b_+60); mem_wr(gb, HL, E);
  L = alu_inc8(gb, L);
  CYC(b_+60, b_+62); SET_DE(pop_effect(gb));
  A = E;
  CYC(b_+62, b_+65); mem_wr(gb, IO_VBK, A);
  CYC(b_+65, b_+66); mem_wr(gb, HL, D);
  L = alu_inc8(gb, L);
  CYC(b_+66, b_+68); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, HL, B);
  CYC(b_+69, b_+70); SET_AF(pop_effect(gb));
  CYC(b_+70, b_+72); mem_wr(gb, IO_SVBK, A);
  CYC(b_+72, b_+73); SET_AF(pop_effect(gb));
  CYC(b_+73, b_+75); H8(hRomBank) = A;
  CYC(b_+75, b_+78); mem_wr(gb, MBC_ROM_BANK, A);
  alu_xor(gb, A);
  CYC(b_+78, b_+80);
}

void queueDmaTransfer_hook(GB *gb) {
  queue_dma_transfer(gb);
  ret_effect(gb);
}

// gfx headers (bank 1 tables): entries of bank, dest, src, size; bit 7 of the size byte
// continues. Uncompressed headers go through the DMA queue, compressed ones through the
// decompressor.

static void load_gfx_header(GB *gb, uint16_t o, uint16_t table, bool compressed) {
  E = A;
  CYC(o + 0x00, o + 0x03); C = mem_rd(gb, IO_SVBK);
  CYC(o + 0x03, o + 0x06); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(o + 0x06, o + 0x0c); H8(hRomBank) = 0x01;
  CYC(o + 0x0c, o + 0x0f); mem_wr(gb, MBC_ROM_BANK, 0x01);
  A = E;
  SET_HL(table);
  CYC(o + 0x0f, o + 0x14);
  add_double_index_to_hl(gb);
  CYC(o + 0x14, o + 0x15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(o + 0x15, o + 0x16); H = mem_rd(gb, HL);
  L = A;
  CYC(o + 0x16, o + 0x17);
  for (;;) {
    CYC(o + 0x17, o + 0x18); A = mem_rd(gb, HL); SET_HL(HL + 1);
    C = A;
    CYC(o + 0x18, o + 0x1a); A = mem_rd(gb, HL); SET_HL(HL + 1);
    D = A;
    CYC(o + 0x1a, o + 0x1c); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    uint16_t dest = DE;
    CYC(o + 0x1c, o + 0x1f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    D = A;
    CYC(o + 0x1f, o + 0x21); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    CYC(o + 0x21, o + 0x23); A = mem_rd(gb, HL);
    alu_and(gb, 0x7f);
    B = A;
    A = L;
    CYC(o + 0x23, o + 0x29); H8(hFF90) = A;
    A = H;
    CYC(o + 0x29, o + 0x2c); H8(hFF91) = A;
    SET_HL(dest);
    CYC(o + 0x2c, o + 0x30);
    if (compressed) { push_effect(gb, (uint16_t)(o + 0x30)); asm_call(gb, ROM_decompressGraphics, (uint16_t)(o + 0x30)); }
    else queue_dma_transfer(gb);
    CYC(o + 0x30, o + 0x34); H8(hRomBank) = 0x01;
    CYC(o + 0x34, o + 0x37); mem_wr(gb, MBC_ROM_BANK, 0x01);
    CYC(o + 0x37, o + 0x39); A = H8(hFF90);
    L = A;
    CYC(o + 0x39, o + 0x3c); A = H8(hFF91);
    H = A;
    CYC(o + 0x3c, o + 0x3e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, A);
    if (F & FC) { CYCT(o + 0x3e, o + 0x41); continue; }
    CYC(o + 0x3e, o + 0x41);
    break;
  }
  SET_BC(bc);
  A = B;
  CYC(o + 0x41, o + 0x45); H8(hRomBank) = A;
  CYC(o + 0x45, o + 0x48); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(o + 0x48, o + 0x4b); mem_wr(gb, IO_SVBK, A);
  CYC(o + 0x4b, o + 0x4c);
}

void loadUncompressedGfxHeader_hook(GB *gb) {
  load_gfx_header(gb, SYM(loadUncompressedGfxHeader), uncmpGfxHeaderTable_bank01, false);
  ret_effect(gb);
}

void loadGfxHeader_hook(GB *gb) {
  load_gfx_header(gb, SYM(loadGfxHeader), gfxHeaderTable_bank01, true);
  ret_effect(gb);
}

// tilesets (bank 1 tables); loadTilesetHlpr stays transliterated

void loadTileset_hook(GB *gb) {
  BASE(loadTileset);
  E = A;
  CYC(b_+0, b_+3); C = mem_rd(gb, IO_SVBK);
  CYC(b_+3, b_+6); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(b_+6, b_+12); H8(hRomBank) = 0x01;
  CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, 0x01);
  A = E;
  SET_HL(tilesetHeaderTable_bank01);
  CYC(b_+15, b_+20);
  add_double_index_to_hl(gb);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+22, b_+23);
  for (;;) {
    CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    SET_HL(tilesetGfxHeaderTable_bank01);
    CYC(b_+24, b_+29);
    add_double_index_to_hl(gb);
    CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+30, b_+31); H = mem_rd(gb, HL);
    L = A;
    CYC(b_+31, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+33, b_+35); H8(hFF8F) = A;
    CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+36, b_+38); H8(hFF91) = A;
    CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+39, b_+41); H8(hFF90) = A;
    SET_HL(hl);
    CYC(b_+41, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+43, b_+45); H8(hFF8E) = A;
    CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
    D = A;
    CYC(b_+46, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    uint16_t src = DE;
    CYC(b_+48, b_+51); A = mem_rd(gb, HL); SET_HL(HL + 1);
    D = A;
    CYC(b_+51, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    CYC(b_+53, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_and(gb, 0x7f);
    CYC(b_+55, b_+59); H8(hFF8D) = A;
    CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+60, b_+62); H8(hFF8C) = A;
    A = H;
    CYC(b_+62, b_+65); H8(hFF93) = A;
    A = L;
    CYC(b_+65, b_+68); H8(hFF92) = A;
    SET_HL(src);
    CYC(b_+68, b_+69);
    CALL_ROM(b_+69, ROM_loadTilesetHlpr);
    CYC(b_+72, b_+76); H8(hRomBank) = 0x01;
    CYC(b_+76, b_+79); mem_wr(gb, MBC_ROM_BANK, 0x01);
    CYC(b_+79, b_+81); A = H8(hFF93);
    H = A;
    CYC(b_+81, b_+84); A = H8(hFF92);
    L = A;
    CYC(b_+84, b_+86); A = mem_rd(gb, HL); SET_HL(HL + 1);
    SET_HL(HL + 1);
    alu_add(gb, A);
    if (F & FC) { CYCT(b_+86, b_+90); continue; }
    CYC(b_+86, b_+90);
    break;
  }
  SET_BC(bc);
  A = B;
  CYC(b_+90, b_+94); H8(hRomBank) = A;
  CYC(b_+94, b_+97); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(b_+97, b_+100); mem_wr(gb, IO_SVBK, A);
  CYC(b_+100, b_+101);
  ret_effect(gb);
}

// animation frame advance, shared by interactions, enemies and parts: the animation data
// pointer (counter + 1..3) walks frames of (duration, oam index, flags); $ff loops back.

static void next_animation_frame(GB *gb, uint16_t o, uint8_t counter, uint8_t id, uint16_t table, uint8_t oam) {
  BASE(_interactionNextAnimationFrame);
  CYC(o, o + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(o + 1, o + 2); H = mem_rd(gb, HL);
  L = A;
  CYC(o + 2, o + 4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_cp(gb, 0xff);
  if (!(F & FZ)) CYCT(o + 4, o + 8);
  else {
    CYC(o + 4, o + 10);
    B = A;
    C = mem_rd(gb, HL);
    alu_add_hl(gb, BC);
    CYC(o + 10, o + 12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  }
  E = counter;
  CYC(o + 12, o + 15); mem_wr(gb, DE, A);
  CYC(o + 15, o + 16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  B = 0x00;
  E = alu_inc8(gb, E);
  CYC(o + 16, o + 21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(o + 21, o + 22); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = L;
  CYC(o + 22, o + 25); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(o + 25, o + 28); mem_wr(gb, DE, A);
  E = id;
  CYC(o + 28, o + 31); A = mem_rd(gb, DE);
  SET_HL(table);
  CYC(o + 31, o + 35);
  add_double_index_to_hl(gb);
  CYC(o + 35, o + 36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(o + 36, o + 37); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  E = oam;
  CYC(o + 37, o + 42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(o + 42, o + 43); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  CYC(o + 43, o + 45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_and(gb, 0x3f);
  if (o == b_+0) {
    alu_or(gb, 0x40);
    CYC(b_+45, b_+50); mem_wr(gb, DE, A);
    CYC(b_+50, b_+51); SET_AF(pop_effect(gb));
    CYC(b_+51, b_+53); H8(hRomBank) = A;
    CYC(b_+53, b_+56); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(b_+56, b_+57);
  } else if (o == SYM(_enemyNextAnimationFrame)) {
    CYC((SYM(_enemyNextAnimationFrame) + 45), (SYM(_enemyNextAnimationFrame) + 48)); mem_wr(gb, DE, A);
    CYC((SYM(_enemyNextAnimationFrame) + 48), (SYM(_enemyNextAnimationFrame) + 49)); SET_AF(pop_effect(gb));
    CYC((SYM(_enemyNextAnimationFrame) + 49), (SYM(_enemyNextAnimationFrame) + 51)); H8(hRomBank) = A;
    CYC((SYM(_enemyNextAnimationFrame) + 51), (SYM(_enemyNextAnimationFrame) + 54)); mem_wr(gb, MBC_ROM_BANK, A);
    CYC((SYM(_enemyNextAnimationFrame) + 54), (SYM(_enemyNextAnimationFrame) + 55));
  } else {
    alu_or(gb, 0x40);
    CYC((SYM(_partNextAnimationFrame) + 45), (SYM(_partNextAnimationFrame) + 50)); mem_wr(gb, DE, A);
    A = 0x11;
    CYC((SYM(_partNextAnimationFrame) + 50), (SYM(_partNextAnimationFrame) + 54)); H8(hRomBank) = A;
    CYC((SYM(_partNextAnimationFrame) + 54), (SYM(_partNextAnimationFrame) + 57)); mem_wr(gb, MBC_ROM_BANK, A);
    CYC((SYM(_partNextAnimationFrame) + 57), (SYM(_partNextAnimationFrame) + 58));
  }
}

void _interactionNextAnimationFrame_hook(GB *gb) { next_animation_frame(gb, SYM(_interactionNextAnimationFrame), 0x60, 0x41, interactionAnimationFrameTable_bank16, 0x5e); ret_effect(gb); }
void _enemyNextAnimationFrame_hook(GB *gb) { next_animation_frame(gb, SYM(_enemyNextAnimationFrame), 0xa0, 0x81, enemyAnimationFrameTable_bank0d, 0x9e); ret_effect(gb); }
void _partNextAnimationFrame_hook(GB *gb) { next_animation_frame(gb, SYM(_partNextAnimationFrame), 0xe0, 0xc1, partAnimationFrameTable_bank16, 0xde); ret_effect(gb); }

// sound driver control and the timer it runs from

static void disable_timer(GB *gb) {
  BASE(disableTimer);
  SET_HL(hFFB7);
  CYC(b_+0, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  alu_xor(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, IO_TAC, A);
  CYC(b_+8, b_+9);
}

void disableTimer_hook(GB *gb) {
  disable_timer(gb);
  ret_effect(gb);
}

static void enable_timer(GB *gb) {
  BASE(enableTimer);
  alu_xor(gb, A);
  CYC(b_+0, b_+3); mem_wr(gb, IO_TAC, A);
  A = 0xa0;
  CYC(b_+3, b_+7); H8(hMusicQueueTail) = A;
  CYC(b_+7, b_+9); H8(hMusicQueueHead) = A;
  CYC(b_+9, b_+11); A = mem_rd(gb, IO_KEY1);
  alu_rlca(gb);
  A = 0x77;
  if (F & FC) CYCT(b_+11, b_+16);
  else { CYC(b_+11, b_+18); A = 0xbb; }
  SET_HL(IO_TIMA);
  CYC(b_+18, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(b_+23, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  SET_HL(hFFB7);
  CYC(b_+27, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(b_+32, b_+33);
}

void enableTimer_hook(GB *gb) {
  enable_timer(gb);
  ret_effect(gb);
}

static void start_sound(GB *gb) {
  BASE(_startSound);
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); A = H8(hRomBank);
  CYC(b_+3, b_+4); push_effect(gb, AF);
  CYC(b_+4, b_+7);
  disable_timer(gb);
  A = 0x39;
  CYC(b_+7, b_+11); H8(hSoundDataBaseBank) = A;
  CYC(b_+11, b_+13); H8(hSoundDataBaseBank2) = A;
  CYC(b_+13, b_+15); H8(hRomBank) = A;
  CYC(b_+15, b_+18); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_ROM(b_+18, ROM_jpBc);
  CYC(b_+21, b_+24);
  enable_timer(gb);
  CYC(b_+24, b_+25); SET_AF(pop_effect(gb));
  CYC(b_+25, b_+27); H8(hRomBank) = A;
  CYC(b_+27, b_+30); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+30, b_+31); SET_DE(pop_effect(gb));
  CYC(b_+31, b_+32);
}

void _startSound_hook(GB *gb) {
  start_sound(gb);
  ret_effect(gb);
}

void restartSound_hook(GB *gb) {
  BASE(restartSound);
  SET_BC(0x4009);
  CYC(b_+0, b_+5);
  start_sound(gb);
  ret_effect(gb);
}

// sprites into wOam: each entry y, x, tile, attributes with a y/x offset in bc

static void add_sprites_to_oam_with_offset(GB *gb) {
  BASE(addSpritesToOam_withOffset);
  CYC(b_+0, b_+2); A = H8(hOamTail);
  alu_cp(gb, 0xa0);
  CYC(b_+2, b_+4);
  if (!(F & FC)) { CYCT(b_+4, b_+5); return; }
  CYC(b_+4, b_+5);
  E = A;
  D = wOam >> 8;
  CYC(b_+5, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); return; }
  CYC(b_+9, b_+11);
  for (;;) {
    CYC(b_+11, b_+13); H8(hFF8B) = A;
    CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, B);
    alu_cp(gb, 0xa0);
    bool skip = false;
    if (!(F & FC)) {
      CYCT(b_+14, b_+19);
      CYC(b_+49, b_+50); SET_HL(HL + 1);
      skip = true;
    } else {
      CYC(b_+14, b_+20); mem_wr(gb, DE, A);
      CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
      alu_add(gb, C);
      alu_cp(gb, 0xa8);
      if (!(F & FC)) { CYCT(b_+21, b_+26); skip = true; }
    }
    if (skip) {
      SET_HL(HL + 2);
      A = 0xe0;
      CYC(b_+50, b_+55); mem_wr(gb, DE, A);
      CYC(b_+55, b_+57);
    } else {
      CYC(b_+21, b_+26);
      E = alu_inc8(gb, E);
      CYC(b_+26, b_+28); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      CYC(b_+28, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+30, b_+31); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      CYC(b_+31, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+33, b_+34); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      A = E;
      alu_cp(gb, 0xa0);
      if (!(F & FC)) { CYCT(b_+34, b_+40); break; }
      CYC(b_+34, b_+40);
    }
    CYC(b_+40, b_+42); A = H8(hFF8B);
    A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+42, b_+45); continue; }
    CYC(b_+42, b_+46);
    A = E;
    break;
  }
  CYC(b_+46, b_+48); H8(hOamTail) = A;
  CYC(b_+48, b_+49);
}

void addSpritesToOam_withOffset_hook(GB *gb) {
  add_sprites_to_oam_with_offset(gb);
  ret_effect(gb);
}

void addSpritesToOam_hook(GB *gb) {
  BASE(addSpritesToOam);
  SET_BC(0);
  CYC(b_+0, b_+3);
  add_sprites_to_oam_with_offset(gb);
  ret_effect(gb);
}

// draw queue: objects sorted into four priority lists in wObjectsToDraw

static void object_queue_draw(GB *gb) {
  BASE(objectQueueDraw);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+1, b_+3); return; }
  CYC(b_+1, b_+3);
  A = E;
  alu_or(gb, OBJ_VISIBLE);
  L = A;
  H = D;
  CYC(b_+3, b_+9); A = mem_rd(gb, HL);
  alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+9, b_+12); return; }
  CYC(b_+9, b_+12);
  alu_and(gb, 0x03);
  H = A;
  alu_add(gb, GV(0xa1, 0x9f));
  C = A;
  CYC(b_+12, b_+19); A = mem_rd(gb, IO_P1 | C);
  alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+19, b_+22); return; }
  CYC(b_+19, b_+22);
  A = alu_inc8(gb, A);
  CYC(b_+22, b_+24); mem_wr(gb, IO_P1 | C, A);
  A = alu_dec8(gb, A);
  H = alu_swap(gb, H);
  alu_add(gb, H);
  alu_add(gb, A);
  L = A;
  H = wObjectsToDraw >> 8;
  A = B;
  CYC(b_+24, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+34, b_+35); mem_wr(gb, HL, D);
  CYC(b_+35, b_+36);
}

void objectQueueDraw_hook(GB *gb) {
  object_queue_draw(gb);
  ret_effect(gb);
}

static void queue_draw_objects(GB *gb) {
  BASE(queueDrawEverything);
  for (;;) {
    CYC(b_+38, b_+41);
    object_queue_draw(gb);
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+41, b_+47); continue; }
    CYC(b_+41, b_+48);
    break;
  }
}

static void queue_draw_everything(GB *gb) {
  BASE(queueDrawEverything);
  SET_HL(hTerrainEffectsBufferUsedSize);
  alu_xor(gb, A);
  CYC(b_+0, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  SET_DE(w1WeaponItem);
  B = 0x0b;
  CYC(b_+9, b_+17);
  queue_draw_objects(gb);
  SET_DE(ENEMY_SLOTS);
  B = 0x8b;
  CYC(b_+17, b_+25);
  queue_draw_objects(gb);
  SET_DE(PART_SLOTS);
  B = 0xcb;
  CYC(b_+25, b_+33);
  queue_draw_objects(gb);
  SET_DE(w1ReservedInteraction0);
  B = 0x4b;
  CYC(b_+33, b_+38);
  queue_draw_objects(gb);
}

void queueDrawEverything_hook(GB *gb) {
  queue_draw_everything(gb);
  ret_effect(gb);
}

// tileset animation and room entry

void loadTilesetAnimation_hook(GB *gb) {
  BASE(loadTilesetAnimation);
  CYC(b_+0, b_+3); A = W8(wLoadedTilesetAnimation);
  B = A;
  CYC(b_+3, b_+7); A = W8(wTilesetAnimation);
  alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+7, b_+9); ret_effect(gb); return; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); W8(wLoadedTilesetAnimation) = A;
  CYC(b_+12, b_+15);
  load_animation_data(gb);
  ret_effect(gb);
}

void func_1135_hook(GB *gb) {
  BASE(func_1135);
  alu_xor(gb, A);
  CYC(b_+0, b_+4); W8(wDeathRespawnBuffer_rememberedCompanionGroup) = A;
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

void func_131f_hook(GB *gb) {
  BASE(func_131f);
  uint16_t sp0_ = gb->sp;
  alu_xor(gb, A);
  CYC(b_+0, b_+4); W8(wScreenOffsetY) = A;
  CYC(b_+4, b_+7); W8(wScreenOffsetX) = A;
  bank_push(gb, b_+7, 0x01);
  CALL_ROM(b_+17, ROM_b01_initializeRoomBoundaryAndLoadAnimations);
  CALL_ROM(b_+20, ROM_b01_setScreenTransitionState02);
  CALL_ROM(b_+23, ROM_loadTilesetAndRoomLayout);
  CYC(b_+26, b_+29); A = W8(wcddf);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+29, b_+32);
    CYC(b_+55, b_+58);
    load_room_collisions(gb);
    CYC(b_+58, b_+61);
    generate_vram_tiles_with_room_changes(gb, sp0_);
    A = 0x10;
    CYC(b_+61, b_+66);
    load_gfx_header(gb, SYM(loadUncompressedGfxHeader), uncmpGfxHeaderTable_bank01, false);
  } else {
    CYC(b_+29, b_+32);
    SET_HL(ROM_b04_func_04_6ed1);
    E = 0x04;
    CYC(b_+32, b_+37);
    CALL_ROM(b_+37, ROM_interBankCall);
    SET_HL(ROM_b04_func_04_6f31);
    E = 0x04;
    CYC(b_+40, b_+45);
    CALL_ROM(b_+45, ROM_interBankCall);
    A = 0x30;
    CYC(b_+48, b_+53);
    load_gfx_header(gb, SYM(loadUncompressedGfxHeader), uncmpGfxHeaderTable_bank01, false);
    CYC(b_+53, b_+55);
  }
  CYC(b_+66, b_+69); A = W8(wTilesetPalette);
  CYC(b_+69, b_+72); W8(wLoadedTilesetPalette) = A;
  CYC(b_+72, b_+75); A = W8(wTilesetUniqueGfx);
  CYC(b_+75, b_+78); W8(wLoadedTilesetUniqueGfx) = A;
  bank_pop(gb, b_+78);
  CYC(b_+84, b_+85);
  ret_effect(gb);
}

void func_1383_hook(GB *gb) {
  BASE(func_1383);
  CYC(b_+0, b_+1);
  ret_effect(gb);
}

void func_13c6_hook(GB *gb) {
  BASE(func_13c6);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  A = 0x02;
  CYC(b_+3, b_+7); mem_wr(gb, IO_SVBK, A);
  CYC(b_+7, b_+8); push_effect(gb, DE);
  CYC(b_+8, b_+9); push_effect(gb, BC);
  SET_DE(w2ColorComponentBuffer1);
  CYC(b_+9, b_+15);
  extract_color_components(gb);
  CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  SET_DE(w2ColorComponentBuffer2);
  CYC(b_+16, b_+22);
  extract_color_components(gb);
  CYC(b_+22, b_+23); SET_DE(pop_effect(gb));
  CYC(b_+23, b_+24); SET_AF(pop_effect(gb));
  CYC(b_+24, b_+26); H8(hRomBank) = A;
  CYC(b_+26, b_+29); mem_wr(gb, MBC_ROM_BANK, A);
  alu_xor(gb, A);
  CYC(b_+29, b_+32); mem_wr(gb, IO_SVBK, A);
  CYC(b_+32, b_+35);
  start_fade_between_two_palettes(gb);
  ret_effect(gb);
}

// text data reads through WRAM bank 7 bank variables

void readByteFromW7ActiveBank_hook(GB *gb) {
  BASE(readByteFromW7ActiveBank);
  uint16_t bc = BC;
  CYC(b_+0, b_+4); A = mem_rd(gb, w7ActiveBank);
  CYC(b_+4, b_+6); H8(hRomBank) = A;
  CYC(b_+6, b_+9); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  A = 0x3f;
  CYC(b_+10, b_+14); H8(hRomBank) = A;
  CYC(b_+14, b_+17); mem_wr(gb, MBC_ROM_BANK, A);
  A = B;
  SET_BC(bc);
  CYC(b_+17, b_+20);
  ret_effect(gb);
}

void readByteFromW7TextTableBank_hook(GB *gb) {
  BASE(readByteFromW7TextTableBank);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+6); A = mem_rd(gb, w7TextTableBank);
  alu_bit(gb, 7, H);
  if (F & FZ) CYCT(b_+6, b_+10);
  else {
    CYC(b_+6, b_+15);
    H = (uint8_t)((H & 0x7f) | 0x40);
    A = alu_inc8(gb, A);
  }
  CYC(b_+15, b_+17); H8(hRomBank) = A;
  CYC(b_+17, b_+20); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+23); H8(hFF8B) = A;
  CYC(b_+23, b_+24); SET_AF(pop_effect(gb));
  CYC(b_+24, b_+26); H8(hRomBank) = A;
  CYC(b_+26, b_+29); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+29, b_+31); A = H8(hFF8B);
  CYC(b_+31, b_+32);
  ret_effect(gb);
}

// secrets and menus (bank 2 and 3 bodies)

void secretFunctionCaller_hook(GB *gb) {
  BASE(secretFunctionCaller);
  bank_push(gb, b_+0, 0x03);
  CALL_ROM(b_+10, ROM_b03_secretFunctionCaller_body);
  bank_pop(gb, b_+13);
  A = B;
  alu_or(gb, A);
  CYC(b_+19, b_+22);
  ret_effect(gb);
}

static void open_menu_tail(GB *gb, uint16_t sp0_) {
  BASE(copyW4PaletteDataToW2TilesetBgPalettes);
  L = A;
  CYC(b_+2, b_+5); C = mem_rd(gb, IO_SVBK);
  CYC(b_+5, b_+8); B = H8(hRomBank);
  CYC(b_+8, b_+10); push_effect(gb, BC);
  A = 0x02;
  CYC(b_+10, b_+14); H8(hRomBank) = A;
  CYC(b_+14, b_+17); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+17, runBank2Function_hook, ROM_b02_runBank2Function, b_+20);
  CYC(b_+20, b_+21); SET_BC(pop_effect(gb));
  A = B;
  CYC(b_+21, b_+24); H8(hRomBank) = A;
  CYC(b_+24, b_+27); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(b_+27, b_+30); mem_wr(gb, IO_SVBK, A);
  CYC(b_+30, b_+31);
}

void openMenu_hook(GB *gb) {
  BASE(openMenu);
  H = 0x06;
  CYC(b_+0, b_+4);
  open_menu_tail(gb, gb->sp);
  ret_effect(gb);
}

void openSecretInputMenu_hook(GB *gb) {
  BASE(openSecretInputMenu);
  CYC(b_+0, b_+3); W8(wSecretInputType) = A;
  A = 0x01;
  CYC(b_+3, b_+8); W8(wTextInputResult) = A;
  A = 0x06;
  CYC(b_+8, b_+13);
  H = 0x06;
  CYC(SYM(openMenu), (SYM(openMenu) + 4));
  open_menu_tail(gb, gb->sp);
  ret_effect(gb);
}

void updateMenus_hook(GB *gb) {
  BASE(updateMenus);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+5); B = H8(hRomBank);
  CYC(b_+5, b_+7); push_effect(gb, BC);
  A = 0x02;
  CYC(b_+7, b_+11); H8(hRomBank) = A;
  CYC(b_+11, b_+14); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+14, b2_updateMenus_hook, ROM_b02_updateMenus, b_+17);
  CYC(b_+17, b_+18); SET_BC(pop_effect(gb));
  A = B;
  CYC(b_+18, b_+21); H8(hRomBank) = A;
  CYC(b_+21, b_+24); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(b_+24, b_+27); mem_wr(gb, IO_SVBK, A);
  CYC(b_+27, b_+30); A = W8(wOpenedMenuType);
  alu_or(gb, A);
  CYC(b_+30, b_+32);
  ret_effect(gb);
}

// bank $3f object graphics loaders: the callees may switch threads, so the saved bank lives on
// the real stack and the interpreter can finish the tail if the C frame is unwound

static void bank3f_push(GB *gb, uint16_t a) {
  CYC(a, a + 2); A = H8(hRomBank);
  CYC(a + 2, a + 3); push_effect(gb, AF);
  A = 0x3f;
  CYC(a + 3, a + 7); H8(hRomBank) = A;
  CYC(a + 7, a + 10); mem_wr(gb, MBC_ROM_BANK, A);
}

static void bank_pop_af(GB *gb, uint16_t a) {
  CYC(a, a + 1); SET_AF(pop_effect(gb));
  CYC(a + 1, a + 3); H8(hRomBank) = A;
  CYC(a + 3, a + 6); mem_wr(gb, MBC_ROM_BANK, A);
}

void interactionInitGraphics_hook(GB *gb) {
  BASE(interactionInitGraphics);
  uint16_t sp0_ = gb->sp;
  bank3f_push(gb, b_+0);
  CALL_C(b_+10, interactionLoadGraphics_hook,
      ROM_b3f_interactionLoadGraphics, b_+13);
  C = A;
  CYC(b_+13, b_+14);
  bank_pop_af(gb, b_+14);
  A = C;
  CYC(b_+20, b_+24);
  interaction_set_animation(gb);
  ret_effect(gb);
}

static void refresh_object_gfx(GB *gb, uint16_t sp0_) {
  BASE(refreshObjectGfx);
  bank3f_push(gb, b_+0);
  CALL_C(b_+10, refreshObjectGfx_body_hook, ROM_b3f_refreshObjectGfx_body, b_+13);
  alu_xor(gb, A);
  CYC(b_+13, b_+17); W8(wLoadedTreeGfxIndex) = A;
  bank_pop_af(gb, b_+17);
  CYC(b_+23, b_+24);
}

void refreshObjectGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  refresh_object_gfx(gb, sp0_);
  ret_effect(gb);
}

void refreshLoadedTreeGfx_hook(GB *gb) {
  BASE(refreshLoadedTreeGfx);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLoadedTreeGfxIndex);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+5);
  refresh_object_gfx(gb, sp0_);
  ret_effect(gb);
}

void loadObjectGfxHeaderToSlot4_hook(GB *gb) {
  BASE(loadObjectGfxHeaderToSlot4);
  uint16_t sp0_ = gb->sp;
  bank3f_push(gb, b_+0);
  CALL_C(b_+10, loadObjectGfxHeaderToSlot4_body_hook,
      ROM_b3f_loadObjectGfxHeaderToSlot4_body, b_+13);
  bank_pop_af(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void loadTreeGfx_hook(GB *gb) {
  BASE(loadTreeGfx);
  uint16_t sp0_ = gb->sp;
  E = A;
  CYC(b_+0, b_+1);
  bank3f_push(gb, b_+1);
  CALL_C(b_+11, loadTreeGfx_body_hook, ROM_b3f_loadTreeGfx_body, b_+14);
  bank_pop_af(gb, b_+14);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

// $20 tiles of object gfx from the 3-byte pointer at hl, through the DMA buffers at 4:dc00/4:de00

static void load_object_gfx2(GB *gb) {
  BASE(loadObjectGfx2);
  C = A;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); L = mem_rd(gb, HL);
  alu_and(gb, 0x7f);
  H = A;
  CYC(b_+2, b_+9); A = mem_rd(gb, wcc20);
  alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+12);
    A = D;
    alu_or(gb, 0xd0);
    D = A;
    A = 0x05;
    alu_add(gb, E);
    E = A;
    B = 0x1f;
    CYC(b_+55, b_+65);
    CALL_ROM(b_+65, ROM_decompressGraphics);
    A = 0x01;
    CYC(b_+68, b_+72); mem_wr(gb, IO_SVBK, A);
    A = 0x3f;
    CYC(b_+72, b_+76); H8(hRomBank) = A;
    CYC(b_+76, b_+79); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(b_+79, b_+80);
    return;
  }
  CYC(b_+9, b_+13); push_effect(gb, DE);
  CYC(b_+13, b_+16); A = W8(wcc07);
  alu_xor(gb, 0xff);
  CYC(b_+16, b_+21); W8(wcc07) = A;
  SET_DE(w4GfxBuf1 + 4);
  if (!(F & FZ)) CYCT(b_+21, b_+26);
  else { CYC(b_+21, b_+29); SET_DE(w4GfxBuf2 + 4); }
  CYC(b_+29, b_+30); push_effect(gb, DE);
  B = 0x1f;
  CYC(b_+30, b_+32);
  CALL_ROM(b_+32, ROM_decompressGraphics);
  CYC(b_+35, b_+36); SET_HL(pop_effect(gb));
  CYC(b_+36, b_+37); SET_DE(pop_effect(gb));
  C = 0x04;
  A = 0x01;
  CYC(b_+37, b_+43); mem_wr(gb, IO_SVBK, A);
  A = 0x3f;
  CYC(b_+43, b_+47); H8(hRomBank) = A;
  CYC(b_+47, b_+50); mem_wr(gb, MBC_ROM_BANK, A);
  B = 0x1f;
  CYC(b_+50, b_+55);
  queue_dma_transfer(gb);
}

void loadObjectGfx2_hook(GB *gb) {
  load_object_gfx2(gb);
  ret_effect(gb);
}

void loadObjectGfx_hook(GB *gb) {
  BASE(loadObjectGfx);
  D = B;
  E = 0x00;
  CYC(b_+0, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  load_object_gfx2(gb);
  ret_effect(gb);
}

// small state setters

void clearFollowingLinkObject_hook(GB *gb) {
  BASE(clearFollowingLinkObject);
  SET_HL(wFollowingLinkObjectType);
  alu_xor(gb, A);
  CYC(b_+0, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+7);
  ret_effect(gb);
}

void setWarpDestVariables_hook(GB *gb) {
  BASE(setWarpDestVariables);
  uint16_t de = DE;
  SET_DE(wWarpDestVariables);
  B = 0x05;
  CYC(b_+0, b_+9);
  copyMemory(gb, DE, HL, 5);
  A = mem_rd(gb, HL + 4);
  B = 0;
  F = (F & FC) | FZ | FN;
  SET_HL(HL + 5);
  SET_DE(de);
  CYC(b_+9, b_+11);
  ret_effect(gb);
}

void setInstrumentsDisabledCounterAndScrollMode_hook(GB *gb) {
  BASE(setInstrumentsDisabledCounterAndScrollMode);
  A = 0x08;
  CYC(b_+0, b_+5); W8(wInstrumentsDisabledCounter) = A;
  A = 0x01;
  CYC(b_+5, b_+10); W8(wScrollMode) = A;
  CYC(b_+10, b_+11);
  ret_effect(gb);
}

// text thread control: the tails hand off to the thread kernel

void _label_00_203_hook(GB *gb);
void _label_00_204_hook(GB *gb);

void stopTextThread_hook(GB *gb) {
  BASE(stopTextThread);
  alu_xor(gb, A);
  CYC(b_+0, b_+4); W8(wTextIsActive) = A;
  CYC(b_+4, b_+7); W8(wTextboxFlags) = A;
  A = 0xf0;
  CYC(b_+7, b_+12);
  hook_handoff(gb, ROM_threadStop);
}

static void show_text_tail(GB *gb) {
  BASE(_label_00_204);
  CYC(b_+0, b_+3); A = W8(wTextboxFlags);
  alu_or(gb, L);
  CYC(b_+3, b_+7); W8(wTextboxFlags) = A;
  A = B;
  alu_add(gb, 0x04);
  B = A;
  SET_HL(wTextDisplayMode);
  CYC(b_+7, b_+15); mem_wr(gb, HL, E);
  L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  A = B;
  CYC(b_+17, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xff);
  L = alu_inc8(gb, L);
  CYC(b_+23, b_+26); mem_wr(gb, HL, 0x02);
  L = alu_inc8(gb, L);
  CYC(b_+26, b_+29); mem_wr(gb, HL, 0x98);
  A = 0x01;
  CYC(b_+29, b_+34); W8(wTextIsActive) = A;
  CYC(b_+34, b_+37); SET_BC(SYM(textThreadStart));
  CYC(b_+37, b_+39); A = 0xf0;
  CYC(b_+39, b_+42);
  threadRestart_hook(gb);
}

void _label_00_203_hook(GB *gb) {
  BASE(_label_00_203);
  CYC(b_+0, b_+2); E = 0x00;
  show_text_tail(gb);
}

void _label_00_204_hook(GB *gb) {
  show_text_tail(gb);
}

void showText_hook(GB *gb) {
  BASE(showText);
  L = 0x00;
  CYC(b_+0, b_+2);
  _label_00_203_hook(gb);
}

void showTextNonExitable_hook(GB *gb) {
  BASE(showTextNonExitable);
  L = 0x02;
  CYCT(b_+0, b_+4);
  _label_00_203_hook(gb);
}

void showTextOnInventoryMenu_hook(GB *gb) {
  BASE(showTextOnInventoryMenu);
  CYC(b_+0, b_+3); A = W8(wTextboxFlags);
  A |= 0x01;
  CYC(b_+3, b_+8); W8(wTextboxFlags) = A;
  L = 0x00;
  E = 0x02;
  CYCT(b_+8, b_+14);
  _label_00_204_hook(gb);
}

void clearAllItemsAndPutLinkOnGround_hook(GB *gb) {
  BASE(clearAllItemsAndPutLinkOnGround);
  uint16_t de = DE;
  CYC(b_+0, b_+1);
  CALL_ROM(b_+1, ROM_clearAllParentItems);
  CALL_ROM(b_+4, ROM_dropLinkHeldItem);
  alu_xor(gb, A);
  CYC(b_+7, b_+11); W8(wIsSeedShooterInUse) = A;
  SET_DE(w1WeaponItem);
  CYC(b_+11, b_+14);
  for (;;) {
    H = D;
    L = 0x01;
    CYC(b_+14, b_+18); A = mem_rd(gb, HL);
    alu_cp(gb, 0x18);
    if (!(F & FZ)) {
      CYCT(b_+18, b_+22);
      L = E;
      B = 0x40;
      CYC(b_+34, b_+37);
      CALL_ROM(b_+37, ROM_clearMemory);
    } else {
      CYC(b_+18, b_+22);
      L = 0x2f;
      CYC(b_+22, b_+26); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
      CYC(b_+26, b_+28); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
      L = 0x1a;
      CYC(b_+28, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
      CYCT(b_+32, b_+34);
    }
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+40, b_+46); continue; }
    CYC(b_+40, b_+46);
    break;
  }
  SET_DE(de);
  CYC(b_+46, b_+50);
  hook_handoff(gb, ROM_putLinkOnGround);
}

// font tile copies

void copyTextCharacterGfx_hook(GB *gb) {
  BASE(copyTextCharacterGfx);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); push_effect(gb, BC);
  SET_HL(gfx_font_start_bank1c);
  alu_bit(gb, 0, C);
  if (!(F & FZ)) CYCT(b_+2, b_+9);
  else {
    CYC(b_+2, b_+12);
    SET_HL(gfx_font_jp_bank1c);
    alu_cp(gb, 0x0e);
    if (!(F & FC)) CYCT(b_+12, b_+16);
    else { CYC(b_+12, b_+18); A = 0x20; }
  }
  CYC(b_+18, b_+21);
  multiply_a_by_16(gb);
  alu_add_hl(gb, BC);
  CYC(b_+21, b_+24); A = H8(hRomBank);
  CYC(b_+24, b_+25); push_effect(gb, AF);
  A = 0x1c;
  CYC(b_+25, b_+29); H8(hRomBank) = A;
  CYC(b_+29, b_+32); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+32, b_+35); A = W8(wFileSelect_fontXor);
  C = A;
  B = 0x10;
  CYC(b_+35, b_+38);
  for (;;) {
    CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_xor(gb, C);
    CYC(b_+39, b_+41); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    CYC(b_+41, b_+43); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    B = alu_dec8(gb, B);
    if (B) { CYCT(b_+43, b_+47); continue; }
    CYC(b_+43, b_+47);
    break;
  }
  CYC(b_+47, b_+48); SET_AF(pop_effect(gb));
  CYC(b_+48, b_+50); H8(hRomBank) = A;
  CYC(b_+50, b_+53); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+53, b_+54); SET_BC(pop_effect(gb));
  CYC(b_+54, b_+55); SET_HL(pop_effect(gb));
  CYC(b_+55, b_+56);
  ret_effect(gb);
}

// retrieveTextCharacter@func_18fd: copies one 1bpp font tile at hl to bc as 2bpp in the colour
// wTextGfxColorIndex selects. gfx_font_start+$140, the heart, is $4860 in both games and is
// always drawn in colour 1.
static void retrieve_text_character_tile(GB *gb) {
  BASE(retrieveTextCharacter__func_18fd);
  E = 0x10;
  A = H;
  alu_cp(gb, 0x48);
  CYC(b_+0, b_+5);
  if (!(F & FZ)) CYCT(b_+5, b_+7);
  else {
    CYC(b_+5, b_+7);
    A = L;
    alu_cp(gb, 0x60);
    CYC(b_+7, b_+10);
    if (F & FZ) { CYCT(b_+10, b_+12); goto color1; }
    CYC(b_+10, b_+12);
  }
  CYC(b_+12, b_+15); A = mem_rd(gb, wTextGfxColorIndex);
  alu_and(gb, 0x0f);
  alu_or(gb, A);
  CYC(b_+15, b_+18);
  if (F & FZ) { CYCT(b_+18, b_+20); goto color0; }
  CYC(b_+18, b_+20);
  A = alu_dec8(gb, A);
  CYC(b_+20, b_+21);
  if (F & FZ) { CYCT(b_+21, b_+23); goto color1; }
  CYC(b_+21, b_+23);
  A = alu_dec8(gb, A);
  CYC(b_+23, b_+24);
  if (F & FZ) { CYCT(b_+24, b_+26); goto color2; }
  CYC(b_+24, b_+26);
  E = 0x20;
  CYC(b_+26, b_+28);
  for (;;) {
    CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+29, b_+30); mem_wr(gb, BC, A);
    SET_BC(BC + 1);
    E = alu_dec8(gb, E);
    CYC(b_+30, b_+32);
    if (!(F & FZ)) { CYCT(b_+32, b_+34); continue; }
    CYC(b_+32, b_+34);
    break;
  }
  CYC(b_+34, b_+37); A = mem_rd(gb, wTextGfxColorIndex);
  alu_and(gb, 0xf0);
  alu_swap_a(gb);
  CYC(b_+37, b_+41);
  CYC(b_+41, b_+44); mem_wr(gb, wTextGfxColorIndex, A);
  CYC(b_+44, b_+45);
  return;
color0:
  for (;;) {
    CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+46, b_+47); mem_wr(gb, BC, A);
    C = alu_inc8(gb, C);
    CYC(b_+47, b_+48);
    CYC(b_+48, b_+49); mem_wr(gb, BC, A);
    SET_BC(BC + 1);
    E = alu_dec8(gb, E);
    CYC(b_+49, b_+51);
    if (!(F & FZ)) { CYCT(b_+51, b_+53); continue; }
    CYC(b_+51, b_+53);
    break;
  }
  CYC(b_+53, b_+54);
  return;
color1:
  for (;;) {
    A = 0xff;
    CYC(b_+54, b_+56);
    CYC(b_+56, b_+57); mem_wr(gb, BC, A);
    C = alu_inc8(gb, C);
    CYC(b_+57, b_+58);
    CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+59, b_+60); mem_wr(gb, BC, A);
    SET_BC(BC + 1);
    E = alu_dec8(gb, E);
    CYC(b_+60, b_+62);
    if (!(F & FZ)) { CYCT(b_+62, b_+64); continue; }
    CYC(b_+62, b_+64);
    break;
  }
  CYC(b_+64, b_+65);
  return;
color2:
  for (;;) {
    CYC(b_+65, b_+66); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+66, b_+67); mem_wr(gb, BC, A);
    C = alu_inc8(gb, C);
    A = 0xff;
    CYC(b_+67, b_+70);
    CYC(b_+70, b_+71); mem_wr(gb, BC, A);
    SET_BC(BC + 1);
    E = alu_dec8(gb, E);
    CYC(b_+71, b_+73);
    if (!(F & FZ)) { CYCT(b_+73, b_+75); continue; }
    CYC(b_+73, b_+75);
    break;
  }
  CYC(b_+75, b_+76);
}

void retrieveTextCharacter_hook(GB *gb) {
  BASE(retrieveTextCharacter);
  uint16_t hl = HL, de = DE, bc = BC;
  CYC(b_+0, b_+6);
  multiply_a_by_16(gb);
  CYC(b_+6, b_+9); A = mem_rd(gb, w7TextGfxSource);
  SET_HL(textGfxSourceTable);
  CYC(b_+9, b_+13);
  add_double_index_to_hl(gb);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  SET_BC(bc);
  A = 0x1c;
  CYC(b_+15, b_+22); H8(hRomBank) = A;
  CYC(b_+22, b_+25); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+25, b_+28);
  retrieve_text_character_tile(gb);
  A = 0x3f;
  CYC(b_+28, b_+32); H8(hRomBank) = A;
  CYC(b_+32, b_+35); mem_wr(gb, MBC_ROM_BANK, A);
  alu_xor(gb, A);
  CYC(b_+35, b_+39); mem_wr(gb, w7TextGfxSource, A);
  SET_DE(de);
  SET_HL(hl);
  CYC(b_+39, b_+42);
  ret_effect(gb);
}

// indirect jumps and the cross-bank call trampoline

void jpHl_hook(GB *gb) {
  CYC(0x00a0, 0x00a1);
  hook_handoff(gb, HL);
}

void jpBc_hook(GB *gb) {
  BASE(jpBc);
  L = C;
  H = B;
  CYC(b_+0, b_+3);
  hook_handoff(gb, HL);
}

void interBankCall_hook(GB *gb) {
  CYC(0x008a, 0x008d); A = H8(hRomBank);
  CYC(0x008d, 0x008e); push_effect(gb, AF);
  A = E;
  CYC(0x008e, 0x0092); H8(hRomBank) = A;
  CYC(0x0092, 0x0095); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_ROM(0x0095, ROM_jpHl);
  CYC(0x0098, 0x0099); SET_AF(pop_effect(gb));
  CYC(0x0099, 0x009c); H8(hRomBank) = A;
  CYC(0x009c, 0x009f); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x009f, 0x00a0);
  ret_effect(gb);
}

// interaction state compares and group-table lookups

void cpInteractionState_hook(GB *gb) {
  BASE(cpInteractionState);
  H = D;
  L = INTERACTION_BASE | OBJ_STATE;
  CYC(b_+0, b_+4); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

void cpInteractionSubstate_hook(GB *gb) {
  BASE(cpInteractionSubstate);
  H = D;
  L = INTERACTION_BASE | OBJ_SUBSTATE;
  CYC(b_+0, b_+4); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

void findByteInGroupTable_hook(GB *gb) {
  BASE(findByteInGroupTable);
  E = A;
  CYC(b_+0, b_+4); A = W8(wActiveGroup);
  CYC(b_+4, b_+5);
  add_double_index_to_hl(gb);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+7, b_+8);
  find_byte_at_hl(gb);
  ret_effect(gb);
}

// angle from (hFF8F,hFF8E) towards (b,c), both offset by 8, as one of 32 directions

static void object_get_relative_angle_with_temp_vars(GB *gb) {
  BASE(objectGetRelativeAngleWithTempVars);
  E = 0x08;
  A = B;
  alu_add(gb, E);
  B = A;
  A = C;
  alu_add(gb, E);
  C = A;
  E = 0x00;
  CYC(b_+0, b_+12); A = H8(hFF8F);
  alu_add(gb, 0x08);
  alu_sub(gb, B);
  if (!(F & FC)) CYCT(b_+12, b_+17);
  else {
    CYC(b_+12, b_+21);
    alu_cpl(gb);
    A = alu_inc8(gb, A);
    E = 0x04;
  }
  H = A;
  CYC(b_+21, b_+24); A = H8(hFF8E);
  alu_add(gb, 0x08);
  alu_sub(gb, C);
  if (!(F & FC)) CYCT(b_+24, b_+29);
  else {
    CYC(b_+24, b_+33);
    alu_cpl(gb);
    A = alu_inc8(gb, A);
    E = alu_inc8(gb, E);
    E = alu_inc8(gb, E);
  }
  alu_cp(gb, H);
  if (!(F & FC)) CYCT(b_+33, b_+36);
  else {
    CYC(b_+33, b_+40);
    E = alu_inc8(gb, E);
    L = A;
    A = H;
    H = L;
  }
  C = E;
  B = 0x00;
  A = alu_srl(gb, A);
  A = alu_srl(gb, A);
  A = alu_srl(gb, A);
  alu_add(gb, A);
  L = A;
  alu_cp(gb, H);
  if (!(F & FC)) CYCT(b_+40, b_+54);
  else {
    CYC(b_+40, b_+54);
    B = alu_inc8(gb, B);
    alu_add(gb, L);
    alu_cp(gb, H);
    if (!(F & FC)) CYCT(b_+54, b_+59);
    else {
      CYC(b_+54, b_+59);
      B = alu_inc8(gb, B);
      alu_add(gb, L);
      alu_cp(gb, H);
      if (!(F & FC)) CYCT(b_+59, b_+64);
      else {
        CYC(b_+59, b_+64);
        B = alu_inc8(gb, B);
        alu_add(gb, L);
        alu_cp(gb, H);
        if (!(F & FC)) CYCT(b_+64, b_+69);
        else { CYC(b_+64, b_+70); B = alu_inc8(gb, B); }
      }
    }
  }
  A = C;
  alu_add(gb, A);
  alu_add(gb, A);
  alu_add(gb, A);
  alu_add(gb, B);
  C = A;
  B = 0x00;
  SET_HL(ROM_pushDirectionData);
  alu_add_hl(gb, BC);
  CYC(b_+70, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+84);
}

void objectGetRelativeAngleWithTempVars_hook(GB *gb) {
  object_get_relative_angle_with_temp_vars(gb);
  ret_effect(gb);
}

void hObjectCheckCollidedWithLink_hook(GB *gb) {
  BASE(hObjectCheckCollidedWithLink);
  uint16_t de = DE;
  D = H;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, 0x0f);
  L = A;
  CYC(b_+0, b_+11);
  check_collided_with_link(gb);
  SET_DE(de);
  CYC(b_+11, b_+13);
  ret_effect(gb);
}

void func_1c84_hook(GB *gb) {
  BASE(func_1c84);
  CYC(b_+0, b_+3); A = W8(w1ReservedItemC);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+5);
  object_h_check_collision_with_link(gb);
  ret_effect(gb);
}

// status bar and other bank 2 menu functions, dispatched through runBank2Function

void checkReloadStatusBarGraphics_hook(GB *gb) {
  BASE(checkReloadStatusBarGraphics);
  SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+0, b_+4); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); ret_effect(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  alu_rrca(gb);
  A = 0x02;
  if (F & FC) CYCT(b_+8, b_+13);
  else { CYC(b_+8, b_+15); A = 0x03; }
  CYC(b_+15, b_+18);
  load_gfx_header(gb, SYM(loadUncompressedGfxHeader), uncmpGfxHeaderTable_bank01, false);
  ret_effect(gb);
}

static void bank2_menu_function(GB *gb, uint16_t a, uint8_t h, uint16_t sp0_) {
  H = h;
  CYC(a, a + 4);
  open_menu_tail(gb, sp0_);
  ret_effect(gb);
}

void loadCommonGraphics_hook(GB *gb) { bank2_menu_function(gb, SYM(loadCommonGraphics), 0x00, gb->sp); }
void updateStatusBar_hook(GB *gb) { bank2_menu_function(gb, SYM(updateStatusBar), 0x01, gb->sp); }
void hideStatusBar_hook(GB *gb) { bank2_menu_function(gb, SYM(hideStatusBar), 0x02, gb->sp); }
void showStatusBar_hook(GB *gb) { bank2_menu_function(gb, SYM(showStatusBar), 0x03, gb->sp); }
void saveGraphicsOnEnterMenu_hook(GB *gb) { bank2_menu_function(gb, SYM(saveGraphicsOnEnterMenu), 0x04, gb->sp); }
void reloadGraphicsOnExitMenu_hook(GB *gb) { bank2_menu_function(gb, SYM(reloadGraphicsOnExitMenu), 0x05, gb->sp); }
void copyW2TilesetBgPalettesToW4PaletteData_hook(GB *gb) { bank2_menu_function(gb, SYM(copyW2TilesetBgPalettesToW4PaletteData), 0x07, gb->sp); }

void copyW4PaletteDataToW2TilesetBgPalettes_hook(GB *gb) {
  BASE(copyW4PaletteDataToW2TilesetBgPalettes);
  H = 0x08;
  CYC(b_+0, b_+2);
  open_menu_tail(gb, gb->sp);
  ret_effect(gb);
}

void getRoomDungeonProperties_hook(GB *gb) {
  BASE(getRoomDungeonProperties);
  bank_push(gb, b_+0, 0x01);
  CYC(b_+10, b_+13); A = W8(wActiveGroup);
  alu_and(gb, 0x01);
  SET_HL(dungeonRoomPropertiesGroupTable_bank01);
  CYC(b_+13, b_+19);
  add_double_index_to_hl(gb);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); H = mem_rd(gb, HL);
  L = A;
  A = B;
  CYC(b_+21, b_+24);
  add_a_to_hl(gb);
  CYC(b_+24, b_+25); B = mem_rd(gb, HL);
  bank_pop(gb, b_+25);
  CYC(b_+31, b_+32);
  ret_effect(gb);
}

// interaction scripts, treasures and floating images

void interactionFunc_278b_hook(GB *gb) {
  BASE(interactionFunc_278b);
  L = 0x58;
  CYC(b_+0, b_+3); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  CYC(b_+3, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6);
  ret_effect(gb);
}

void interactionSetMiniScript_hook(GB *gb) {
  BASE(interactionSetMiniScript);
  E = 0x58;
  A = L;
  CYC(b_+0, b_+4); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+4, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8);
  ret_effect(gb);
}

void objectOscillateZ_hook(GB *gb) {
  BASE(objectOscillateZ);
  bank_push(gb, b_+0, GV(0x09, 0x08));
  CALL_ROM(b_+10, ROM_b09_objectOscillateZ_body);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

static void create_ring_treasure(GB *gb) {
  BASE(createRingTreasure);
  CYC(b_+0, b_+3);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60);
  L = alu_inc8(gb, L);
  CYC(b_+6, b_+9); mem_wr(gb, HL, 0x2d);
  L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); mem_wr(gb, HL, C);
  L = 0x78;
  B |= 0x40;
  CYC(b_+11, b_+16); mem_wr(gb, HL, B);
  alu_xor(gb, A);
  CYC(b_+16, b_+18);
}

void createRingTreasure_hook(GB *gb) {
  create_ring_treasure(gb);
  ret_effect(gb);
}

void giveRingToLink_hook(GB *gb) {
  BASE(giveRingToLink);
  CYC(b_+0, b_+3);
  create_ring_treasure(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  uint16_t de = DE;
  SET_DE(w1Link_yh);
  CYC(b_+4, b_+11);
  copy_pos_raw(gb);
  SET_DE(de);
  alu_xor(gb, A);
  CYC(b_+11, b_+14);
  ret_effect(gb);
}

void createTreasure_hook(GB *gb) {
  BASE(createTreasure);
  CYC(b_+0, b_+3);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60);
  L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, C);
  alu_xor(gb, A);
  CYC(b_+10, b_+12);
  ret_effect(gb);
}

void objectCreateExclamationMark_hook(GB *gb) {
  BASE(objectCreateExclamationMark);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  bank_push(gb, b_+2, GV(0x0b, 0x0a));
  CYC(b_+12, b_+14); A = H8(hFF8B);
  CALL_ROM(b_+14, ROM_b0b_objectCreateExclamationMark_body);
  bank_pop(gb, b_+17);
  CYC(b_+23, b_+24);
  ret_effect(gb);
}

void objectCreateFloatingSnore_hook(GB *gb) {
  BASE(objectCreateFloatingMusicNote);
  CYC(SYM(objectCreateFloatingSnore), (SYM(objectCreateFloatingSnore) + 2)); H8(hFF8B) = A;
  A = 0x00;
  CYC((SYM(objectCreateFloatingSnore) + 2), (SYM(objectCreateFloatingSnore) + 6));
  CYC(b_+4, b_+6); H8(hFF8D) = A;
  bank_push(gb, b_+6, 0x0b);
  CALL_ROM(b_+16, ROM_b0b_objectCreateFloatingImage);
  bank_pop(gb, b_+19);
  CYC(b_+25, b_+26);
  ret_effect(gb);
}

void checkGlobalFlag_hook(GB *gb) {
  BASE(checkGlobalFlag);
  SET_HL(wGlobalFlags);
  CYC(b_+0, b_+6);
  check_flag(gb);
  ret_effect(gb);
}

void setGlobalFlag_hook(GB *gb) {
  BASE(setGlobalFlag);
  SET_HL(wGlobalFlags);
  CYC(b_+0, b_+6);
  set_flag(gb);
  ret_effect(gb);
}

static void bank12_table_pointer(GB *gb, uint16_t a, uint16_t table) {
  bank_push(gb, a, 0x12);
  A = B;
  SET_HL(table);
  CYC(a + 10, a + 15);
  add_double_index_to_hl(gb);
  CYC(a + 15, a + 16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(a + 16, a + 17); H = mem_rd(gb, HL);
  L = A;
  CYC(a + 17, a + 18);
  bank_pop(gb, a + 18);
  CYC(a + 24, a + 25);
  ret_effect(gb);
}

void getWildTokayObjectDataIndex_hook(GB *gb) { bank12_table_pointer(gb, SYM(getWildTokayObjectDataIndex), wildTokayObjectTable_bank12); }
void getEntryFromObjectTable1_hook(GB *gb) { bank12_table_pointer(gb, SYM(getEntryFromObjectTable1), objectTable1_bank12); }

void enemyCodeNil_hook(GB *gb) {
  BASE(enemyCodeNil);
  CYC(b_+0, b_+1);
  ret_effect(gb);
}

static void bank2_function_caller(GB *gb, uint16_t a, uint8_t h, uint16_t sp0_) {
  BASE(checkSpawnTimeportalInteraction_b00);
  H = h;
  if (a != b_+0) CYC(a, a + 4); else CYC(a, a + 2);
  L = A;
  CYC(b_+2, b_+3);
  bank_push(gb, b_+3, 0x02);
  CALL_C(b_+13, functionCaller_b02_hook, SYM(functionCaller_b02), b_+16);
  C = alu_rl(gb, C);
  CYC(b_+16, b_+18);
  bank_pop(gb, b_+18);
  C = alu_srl(gb, C);
  CYC(b_+24, b_+27);
  ret_effect(gb);
}

void func_3211_hook(GB *gb) { bank2_function_caller(gb, SYM(func_3211), 0x03, gb->sp); }
void clearEnemiesKilledList_b00_hook(GB *gb) { bank2_function_caller(gb, SYM(clearEnemiesKilledList_b00), 0x00, gb->sp); }
void addRoomToEnemiesKilledList_b00_hook(GB *gb) { bank2_function_caller(gb, SYM(addRoomToEnemiesKilledList_b00), 0x01, gb->sp); }
void markEnemyAsKilledInRoom_b00_hook(GB *gb) { bank2_function_caller(gb, SYM(markEnemyAsKilledInRoom_b00), 0x02, gb->sp); }
void generateRandomBuffer_b00_hook(GB *gb) { bank2_function_caller(gb, SYM(generateRandomBuffer_b00), 0x04, gb->sp); }
void getRandomPositionForEnemy_b00_hook(GB *gb) { bank2_function_caller(gb, SYM(getRandomPositionForEnemy_b00), 0x05, gb->sp); }
void checkSpawnTimeportalInteraction_b00_hook(GB *gb) { bank2_function_caller(gb, SYM(checkSpawnTimeportalInteraction_b00), 0x06, gb->sp); }

// Link grabbing and A-button interaction with nearby objects

void checkGrabbableObjects_hook(GB *gb) {
  BASE(checkGrabbableObjects);
  CYC(b_+0, b_+3); A = W8(w1ReservedItemC);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); ret_effect(gb); return; }
  CYC(b_+3, b_+5);
  uint16_t de = DE;
  CYC(b_+5, b_+9);
  link_position_plus_direction_offset(gb);
  SET_HL(wGrabbableObjectBuffer);
  CYC(b_+9, b_+12);
  for (;;) {
    L = alu_inc8(gb, L);
    CYC(b_+12, b_+15); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYC(b_+15, b_+18); uint16_t hl = HL;
      L = alu_dec8(gb, L);
      CYC(b_+18, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+20, b_+21); H = mem_rd(gb, HL);
      CYC(b_+21, b_+24);
      check_collision_with_h_and_d(gb);
      if (F & FC) {
        CYCT(b_+24, b_+26);
        CYC(b_+36, b_+37);
        E = 0x19;
        A = H;
        CYC(b_+37, b_+41); mem_wr(gb, DE, A);
        E = alu_dec8(gb, E);
        A = L;
        alu_and(gb, 0xc0);
        CYC(b_+41, b_+46); mem_wr(gb, DE, A);
        L = A;
        CYC(b_+46, b_+49); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
        alu_add(gb, 0x04);
        L = A;
        CYC(b_+49, b_+54); mem_wr(gb, HL, 0x02);
        L = alu_inc8(gb, L);
        CYC(b_+54, b_+57); mem_wr(gb, HL, 0x00);
        SET_DE(de);
        alu_scf(gb);
        CYC(b_+57, b_+60);
        ret_effect(gb);
        return;
      }
      CYC(b_+24, b_+27); SET_HL(hl);
    } else CYCT(b_+15, b_+17);
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, GV(0x84, 0x9e));
    if (F & FC) { CYCT(b_+27, b_+33); continue; }
    CYC(b_+27, b_+33);
    break;
  }
  SET_DE(de);
  alu_xor(gb, A);
  CYC(b_+33, b_+36);
  ret_effect(gb);
}

void linkInteractWithAButtonSensitiveObjects_hook(GB *gb) {
  BASE(linkInteractWithAButtonSensitiveObjects);
  CYC(b_+0, b_+3); A = W8(wGameKeysJustPressed);
  alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+3, b_+6); ret_effect(gb); return; }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); A = W8(wAButtonSensitiveObjectListEnd);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(b_+9, b_+12);
  else {
    CYC(b_+9, b_+15); A = W8(wLinkGrabState);
    alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+15, b_+17); ret_effect(gb); return; }
    CYC(b_+15, b_+17);
  }
  uint16_t de = DE;
  E = 0x08;
  CYC(b_+17, b_+21); A = mem_rd(gb, DE);
  SET_HL(linkInteractPositionOffsets);
  CYC(b_+21, b_+25);
  add_double_index_to_hl(gb);
  E = 0x0b;
  CYC(b_+25, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+31); H8(hFF8D) = A;
  SET_HL(HL + 1);
  E = 0x0d;
  CYC(b_+31, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+36, b_+38); H8(hFF8C) = A;
  SET_DE(wAButtonSensitiveObjectList);
  CYC(b_+38, b_+41);
  bool found = false;
  for (;;) {
    CYC(b_+41, b_+42); A = mem_rd(gb, DE);
    H = A;
    E = alu_inc8(gb, E);
    CYC(b_+42, b_+45); A = mem_rd(gb, DE);
    L = A;
    alu_or(gb, H);
    if (F & FZ) CYCT(b_+45, b_+49);
    else {
      CYC(b_+45, b_+50); uint16_t hl = HL;
      CYC(b_+50, b_+52); A = H8(hFF8D);
      B = A;
      CYC(b_+52, b_+55); A = H8(hFF8C);
      C = A;
      CYC(b_+55, b_+59);
      h_check_contains_point(gb);
      CYC(b_+59, b_+60); SET_HL(hl);
      if (!(F & FC)) CYCT(b_+60, b_+62);
      else {
        CYC(b_+60, b_+62);
        CYC(b_+62, b_+64); alu_bit(gb, 0, mem_rd(gb, HL));
        if (F & FZ) { CYCT(b_+64, b_+66); found = true; break; }
        CYC(b_+64, b_+66);
      }
    }
    E = alu_inc8(gb, E);
    A = E;
    alu_cp(gb, GV(0xd3, 0xea));
    if (F & FC) { CYCT(b_+66, b_+72); continue; }
    CYC(b_+66, b_+72);
    break;
  }
  if (!found) {
    SET_DE(de);
    CYC(b_+72, b_+74);
    ret_effect(gb);
    return;
  }
  CYC(b_+74, b_+76); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  SET_HL(w1Link_invincibilityCounter);
  CYC(b_+76, b_+80); A = mem_rd(gb, HL);
  alu_or(gb, A);
  A = 0xfc;
  bool write = true;
  if (F & FZ) CYCT(b_+80, b_+85);
  else {
    CYC(b_+80, b_+85);
    CYC(b_+85, b_+87); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(b_+87, b_+89);
      CYC(b_+96, b_+97); alu_cp(gb, mem_rd(gb, HL));
      if (!(F & FC)) { CYCT(b_+97, b_+99); write = false; }
      else CYC(b_+97, b_+99);
    } else {
      CYC(b_+87, b_+89);
      A = 0x04;
      CYC(b_+89, b_+92); alu_cp(gb, mem_rd(gb, HL));
      if (F & FC) { CYCT(b_+92, b_+94); write = false; }
      else { CYC(b_+92, b_+94); CYC(b_+94, b_+96); }
    }
  }
  if (write) { CYC(b_+99, b_+100); mem_wr(gb, HL, A); }
  A = 0x08;
  CYC(b_+100, b_+105); W8(wDisableRingTransformations) = A;
  A = 0x80;
  CYC(b_+105, b_+110); W8(wForceLinkPushAnimation) = A;
  SET_HL(wLinkTurningDisabled);
  CYC(b_+110, b_+115); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  alu_scf(gb);
  SET_DE(de);
  CYC(b_+115, b_+118);
  ret_effect(gb);
}

// per-frame object update loops

static void switch_bank(GB *gb, uint16_t a, uint8_t bank) {
  A = bank;
  CYC(a, a + 4); H8(hRomBank) = A;
  CYC(a + 4, a + 7); mem_wr(gb, MBC_ROM_BANK, A);
}

static void update_enemies_if_state_is_zero(GB *gb) {
  BASE(_updateEnemiesIfStateIsZero);
  A = 0x80;
  CYC(b_+0, b_+4); H8(hActiveObjectType) = A;
  D = 0xd0;
  A = D;
  CYC(b_+4, b_+7);
  for (;;) {
    CYC(b_+7, b_+9); H8(hActiveObject) = A;
    H = D;
    L = 0x80;
    CYC(b_+9, b_+13); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) CYCT(b_+13, b_+16);
    else {
      CYC(b_+13, b_+16);
      L = 0x84;
      CYC(b_+16, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+19, b_+20); alu_or(gb, mem_rd(gb, HL));
      if (F & FZ) CALL_ROM_CC(b_+20, ROM_updateEnemy);
      else CYC(b_+20, b_+23);
      E = 0x9b;
      CYC(b_+23, b_+26); A = mem_rd(gb, DE);
      E = alu_inc8(gb, E);
      CYC(b_+26, b_+28); mem_wr(gb, DE, A);
    }
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+28, b_+34); continue; }
    CYC(b_+28, b_+35);
    break;
  }
}

void _updateEnemiesIfStateIsZero_hook(GB *gb) {
  update_enemies_if_state_is_zero(gb);
  ret_effect(gb);
}

void updateEnemies_hook(GB *gb) {
  BASE(updateEnemies);
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+3, b_+7); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+3, b_+7);
  CYC(b_+7, b_+10); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); A = W8(wDisabledObjects);
  alu_and(gb, 0x84);
  if (!(F & FZ)) { CYCT(b_+16, b_+20); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+16, b_+20);
  CYC(b_+20, b_+23); A = W8(wPaletteThread_mode);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+26); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+23, b_+26);
  A = 0x80;
  CYC(b_+26, b_+30); H8(hActiveObjectType) = A;
  D = 0xd0;
  A = D;
  CYC(b_+30, b_+33);
  for (;;) {
    CYC(b_+33, b_+35); H8(hActiveObject) = A;
    E = 0x80;
    CYC(b_+35, b_+38); A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(b_+38, b_+41);
    else {
      CYC(b_+38, b_+41);
      CALL_ROM(b_+41, ROM_updateEnemy);
      H = D;
      L = 0xaa;
      CYC(b_+44, b_+49); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
      L = alu_inc8(gb, L);
      CYC(b_+49, b_+51); A = mem_rd(gb, HL);
      alu_or(gb, A);
      bool reset = true;
      if (F & FZ) CYCT(b_+51, b_+54);
      else {
        CYC(b_+51, b_+54);
        alu_rlca(gb);
        if (F & FC) {
          CYCT(b_+54, b_+57);
          CYC(b_+86, b_+87); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
        } else {
          CYC(b_+54, b_+57);
          CYC(b_+57, b_+58); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
          if (F & FZ) CYCT(b_+58, b_+60);
          else {
            CYC(b_+58, b_+60);
            CYC(b_+60, b_+63); A = W8(wFrameCounter);
            alu_bit(gb, 2, A);
            if (!(F & FZ)) CYCT(b_+63, b_+67);
            else {
              CYC(b_+63, b_+67);
              B = 0x05;
              L = 0x9b;
              CYC(b_+67, b_+72); A = mem_rd(gb, HL); SET_HL(HL + 1);
              alu_and(gb, 0x07);
              alu_cp(gb, B);
              if (!(F & FZ)) CYCT(b_+72, b_+77);
              else { CYC(b_+72, b_+79); B = 0x02; }
              CYC(b_+79, b_+80); A = mem_rd(gb, HL);
              alu_and(gb, 0xf8);
              alu_or(gb, B);
              CYC(b_+80, b_+84); mem_wr(gb, HL, A);
              CYC(b_+84, b_+86);
              reset = false;
            }
          }
        }
      }
      if (reset) {
        L = 0x9b;
        CYC(b_+87, b_+90); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+90, b_+91); mem_wr(gb, HL, A);
      }
    }
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+91, b_+97); continue; }
    CYC(b_+91, b_+98);
    break;
  }
  ret_effect(gb);
}

void updateEnemy_hook(GB *gb) {
  BASE(updateEnemy);
  CALL_ROM(b_+0, ROM_enemyStandardUpdate);
  E = 0x81;
  CYC(b_+3, b_+6); A = mem_rd(gb, DE);
  B = 0x0f;
  alu_cp(gb, 0x70);
  if (!(F & FC)) CYCT(b_+6, b_+12);
  else {
    CYC(b_+6, b_+12);
    B = alu_dec8(gb, B);
    alu_cp(gb, 0x30);
    if (!(F & FC)) CYCT(b_+12, b_+17);
    else {
      CYC(b_+12, b_+17);
      B = alu_dec8(gb, B);
      alu_cp(gb, 0x08);
      if (!(F & FC)) CYCT(b_+17, b_+22);
      else { CYC(b_+17, b_+24); B = 0x10; }
    }
  }
  alu_add(gb, A);
  alu_add(gb, 0x34);
  L = A;
  A = 0x00;
  alu_adc(gb, 0x2f);
  H = A;
  CYC(b_+24, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+35); H = mem_rd(gb, HL);
  L = A;
  A = B;
  CYC(b_+35, b_+39); H8(hRomBank) = A;
  CYC(b_+39, b_+42); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  alu_or(gb, A);
  CYC(b_+42, b_+45);
  hook_handoff(gb, HL);
}

static void update_interactions_if_state_is_zero(GB *gb) {
  BASE(_updateInteractionsIfStateIsZero);
  A = 0x40;
  CYC(b_+0, b_+4); H8(hActiveObjectType) = A;
  A = 0xd0;
  CYC(b_+4, b_+6);
  for (;;) {
    CYC(b_+6, b_+8); H8(hActiveObject) = A;
    D = A;
    E = 0x40;
    CYC(b_+8, b_+12); A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(b_+12, b_+15);
    else {
      CYC(b_+12, b_+15);
      alu_rlca(gb);
      bool call = true;
      if (F & FC) CYCT(b_+15, b_+18);
      else {
        CYC(b_+15, b_+18);
        E = 0x44;
        CYC(b_+18, b_+21); A = mem_rd(gb, DE);
        alu_or(gb, A);
        if (!(F & FZ)) { CYCT(b_+21, b_+24); call = false; }
        else CYC(b_+21, b_+24);
      }
      if (call) CALL_ROM(b_+24, ROM_updateInteraction);
    }
    CYC(b_+27, b_+29); A = H8(hActiveObject);
    A = alu_inc8(gb, A);
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+29, b_+34); continue; }
    CYC(b_+29, b_+35);
    break;
  }
}

void _updateInteractionsIfStateIsZero_hook(GB *gb) {
  update_interactions_if_state_is_zero(gb);
  ret_effect(gb);
}

void updateInteractions_hook(GB *gb) {
  BASE(updateInteractions);
  CYC(b_+0, b_+3); A = W8(wScrollMode);
  alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+3, b_+7); update_interactions_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+3, b_+7);
  CYC(b_+7, b_+10); A = W8(wDisabledObjects);
  alu_and(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+10, b_+14); update_interactions_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+10, b_+14);
  CYC(b_+14, b_+17); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+20); update_interactions_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(b_+17, b_+20);
  A = 0x40;
  CYC(b_+20, b_+24); H8(hActiveObjectType) = A;
  A = 0xd0;
  CYC(b_+24, b_+26);
  for (;;) {
    CYC(b_+26, b_+28); H8(hActiveObject) = A;
    D = A;
    E = 0x40;
    CYC(b_+28, b_+32); A = mem_rd(gb, DE);
    alu_or(gb, A);
    CYC(b_+32, b_+33);
    if (!(F & FZ)) CALL_ROM_CC(b_+33, ROM_updateInteraction);
    else CYC(b_+33, b_+36);
    CYC(b_+36, b_+38); A = H8(hActiveObject);
    A = alu_inc8(gb, A);
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+38, b_+43); continue; }
    CYC(b_+38, b_+44);
    break;
  }
  ret_effect(gb);
}

void updateInteraction_hook(GB *gb) {
  BASE(updateInteraction);
  E = 0x41;
  CYC(b_+0, b_+3); A = mem_rd(gb, DE);
  B = 0x08;
  alu_cp(gb, 0x3e);
  if (F & FC) CYCT(b_+3, b_+9);
  else {
    CYC(b_+3, b_+9);
    B = alu_inc8(gb, B);
    alu_cp(gb, 0x67);
    if (F & FC) CYCT(b_+9, b_+14);
    else {
      CYC(b_+9, b_+14);
      B = alu_inc8(gb, B);
      alu_cp(gb, 0x98);
      if (F & FC) CYCT(b_+14, b_+19);
      else {
        CYC(b_+14, b_+19);
        B = alu_inc8(gb, B);
        alu_cp(gb, 0xdc);
        if (F & FC) CYCT(b_+19, b_+24);
        else { CYC(b_+19, b_+26); B = 0x10; }
      }
    }
  }
  A = B;
  CYC(b_+26, b_+29); H8(hRomBank) = A;
  CYC(b_+29, b_+32); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  SET_HL(interactionCodeTable);
  CYC(b_+33, b_+37);
  add_double_index_to_hl(gb);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+39); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+39, b_+41);
  hook_handoff(gb, HL);
}

void load_link_and_companion_animation_frame_hook(GB *gb) {
  BASE(loadLinkAndCompanionAnimationFrame);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, 0x06);
  CALL_C(b_+10, loadLinkAndCompanionAnimationFrame_body_hook, SYM(loadLinkAndCompanionAnimationFrame_body), b_+13);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
}

void loadLinkAndCompanionAnimationFrame_hook(GB *gb) {
  load_link_and_companion_animation_frame_hook(gb);
  ret_effect(gb);
}

static void update_all_objects_after_bank6_selected(GB *gb, uint16_t sp0_);

void updateAllObjects_hook(GB *gb) {
  BASE(updateAllObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  switch_bank(gb, b_+3, 0x05);
  CALL_C(b_+10, updateSpecialObjects_hook, SYM(updateSpecialObjects), b_+13);
  switch_bank(gb, b_+13, 0x07);
  CALL_C(b_+20, updateItems_hook, SYM(updateItems), b_+23);
  CALL_ROM(b_+23, ROM_setEnemyTargetToLinkPosition);
  switch_bank(gb, b_+26, 0x00);
  CALL_ROM(b_+33, ROM_updateEnemies);
  switch_bank(gb, b_+36, GV(0x11, 0x10));
  CALL_ROM(b_+43, ROM_b11_updateParts);
  switch_bank(gb, b_+46, 0x00);
  CALL_ROM(b_+53, ROM_updateInteractions);
  switch_bank(gb, b_+56, 0x01);
  CALL_C(b_+63, func_4000_b01_hook, ROM_b01_func_4000, b_+66);
  switch_bank(gb, b_+66, 0x05);
  CYC(b_+73, b_+76); A = W8(wLinkObjectIndex);
  alu_rrca(gb);
  CYC(b_+76, b_+77);
  if (F & FC) CALL_C_CC(b_+77, func_410d_hook, SYM(func_410d), b_+80);
  else CYC(b_+77, b_+80);
  CYC(b_+80, b_+82); A = 0x06;
  update_all_objects_after_bank6_selected(gb, sp0_);
}

static void update_all_objects_after_bank6_selected(GB *gb, uint16_t sp0_) {
  BASE(updateAllObjects);
  CYC(b_+82, b_+84); H8(hRomBank) = A;
  CYC(b_+84, b_+87); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+87, b_+90); A = W8(wLinkGrabState);
  alu_rlca(gb);
  CYC(b_+90, b_+91);
  if (F & FC) CALL_ROM_CC(b_+91, ROM_b06_updateGrabbedObjectPosition);
  else CYC(b_+91, b_+94);
  CYC(b_+94, b_+97);
  load_link_and_companion_animation_frame_hook(gb);
  switch_bank(gb, b_+97, 0x07);
  CALL_C(b_+104, updateItemsPost_hook, SYM(updateItemsPost), b_+107);
  switch_bank(gb, b_+107, 0x01);
  CALL_ROM(b_+114, ROM_b01_checkUpdateFollowingLinkObject);
  switch_bank(gb, b_+117, 0x00);
  CALL_ROM(b_+124, ROM_updateCamera);
  switch_bank(gb, b_+127, 0x04);
  CALL_C(b_+134, updateChangedTileQueue_hook, SYM(updateChangedTileQueue), b_+137);
  switch_bank(gb, b_+137, 0x04);
  CALL_ROM(b_+144, ROM_b04_updateAnimations);
  alu_xor(gb, A);
  CYC(b_+147, b_+151); W8(wc4b6) = A;
  CYC(b_+151, b_+152); SET_AF(pop_effect(gb));
  CYC(b_+152, b_+154); H8(hRomBank) = A;
  CYC(b_+154, b_+157); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+157, b_+158);
  ret_effect(gb);
}

void updateAllObjects__jump34ad_hook(GB *gb) {
  update_all_objects_after_bank6_selected(gb, gb->sp);
}

void updateSpecialObjectsAndInteractions_hook(GB *gb) {
  BASE(updateSpecialObjectsAndInteractions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  switch_bank(gb, b_+3, 0x05);
  CALL_C(b_+10, updateSpecialObjects_hook, SYM(updateSpecialObjects), b_+13);
  switch_bank(gb, b_+13, 0x00);
  CALL_ROM(b_+20, ROM_updateInteractions);
  CYC(b_+23, b_+26);
  load_link_and_companion_animation_frame_hook(gb);
  alu_xor(gb, A);
  CYC(b_+26, b_+30); W8(wc4b6) = A;
  CYC(b_+30, b_+31); SET_AF(pop_effect(gb));
  CYC(b_+31, b_+33); H8(hRomBank) = A;
  CYC(b_+33, b_+36); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+36, b_+37);
  ret_effect(gb);
}

void updateInteractionsAndDrawAllSprites_hook(GB *gb) {
  BASE(updateInteractionsAndDrawAllSprites);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  switch_bank(gb, b_+3, 0x00);
  CALL_ROM(b_+10, ROM_updateInteractions);
  CALL_ROM(b_+13, ROM_drawAllSprites);
  alu_xor(gb, A);
  CYC(b_+16, b_+20); W8(wc4b6) = A;
  CYC(b_+20, b_+21); SET_AF(pop_effect(gb));
  CYC(b_+21, b_+23); H8(hRomBank) = A;
  CYC(b_+23, b_+26); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+26, b_+27);
  ret_effect(gb);
}

void func_3539_hook(GB *gb) {
  BASE(func_3539);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  switch_bank(gb, b_+3, 0x05);
  CALL_C(b_+10, updateSpecialObjects_hook, SYM(updateSpecialObjects), b_+13);
  switch_bank(gb, b_+13, 0x07);
  CALL_C(b_+20, updateItems_hook, SYM(updateItems), b_+23);
  switch_bank(gb, b_+23, 0x00);
  CALL_ROM(b_+30, ROM_updateEnemies);
  switch_bank(gb, b_+33, 0x11);
  CALL_ROM(b_+40, ROM_b11_updateParts);
  switch_bank(gb, b_+43, 0x00);
  CALL_ROM(b_+50, ROM_updateInteractions);
  switch_bank(gb, b_+53, 0x07);
  CALL_C(b_+60, updateItemsPost_hook, SYM(updateItemsPost), b_+63);
  switch_bank(gb, b_+63, 0x00);
  CYC(b_+70, b_+73);
  load_link_and_companion_animation_frame_hook(gb);
  switch_bank(gb, b_+73, 0x04);
  CALL_ROM(b_+80, ROM_b04_updateAnimations);
  alu_xor(gb, A);
  CYC(b_+83, b_+87); W8(wc4b6) = A;
  CYC(b_+87, b_+88); SET_AF(pop_effect(gb));
  CYC(b_+88, b_+90); H8(hRomBank) = A;
  CYC(b_+90, b_+93); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+93, b_+94);
  ret_effect(gb);
}

// tileset and room loading

// loadTilesetLayout@helper: hl points at a tile mapping index in w3TileMappingIndices, de at the
// destination in w3TileMappingData; copies the four tile indices and four attributes.
static void load_tileset_layout_helper(GB *gb) {
  BASE(loadTilesetLayout__helper);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  SET_HL(SYM(tileMappingTable));
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  CYC(b_+5, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  B = A;
  CYC(b_+14, b_+19);
  CYC(b_+19, b_+20); push_effect(gb, HL);
  SET_HL(SYM(tileMappingIndexDataPointer));
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+25); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  B = 0x04;
  CYC(b_+25, b_+32);
  CALL_C(b_+32, copyMemory_hook, SYM(copyMemory), b_+35);
  CYC(b_+35, b_+36); SET_HL(pop_effect(gb));
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_and(gb, 0x0f);
  B = A;
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+41); C = mem_rd(gb, HL);
  SET_HL(SYM(tileMappingAttributeDataPointer));
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  alu_add_hl(gb, BC);
  B = 0x04;
  CYC(b_+46, b_+53);
  CALL_C(b_+53, copyMemory_hook, SYM(copyMemory), b_+56);
  CYC(b_+56, b_+57); SET_HL(pop_effect(gb));
  CYC(b_+57, b_+58);
}

void loadTilesetLayout_hook(GB *gb) {
  BASE(loadTilesetLayout);
  CYC(b_+0, b_+3); A = W8(wTilesetLayout);
  CALL_ROM(b_+3, ROM_loadTileset);
  switch_bank(gb, b_+6, 0x18);
  A = 0x03;
  CYC(b_+13, b_+17); mem_wr(gb, IO_SVBK, A);
  SET_HL(w3TileMappingIndices);
  SET_DE(w3TileMappingData);
  B = 0x00;
  CYC(b_+17, b_+25);
  for (;;) {
    CYC(b_+25, b_+26); push_effect(gb, BC);
    CYC(b_+26, b_+29);
    load_tileset_layout_helper(gb);
    CYC(b_+29, b_+30); SET_BC(pop_effect(gb));
    B = alu_dec8(gb, B);
    if (B) { CYCT(b_+30, b_+33); continue; }
    CYC(b_+30, b_+33);
    break;
  }
  SET_HL(ROM_b04_setPastCliffPalettesToRed);
  E = 0x04;
  CYC(b_+33, b_+41);
  interBankCall_hook(gb);
}

static void load_tileset_unique_gfx(GB *gb) {
  BASE(loadTilesetUniqueGfx);
  switch_bank(gb, b_+0, 0x04);
  CYC(b_+7, b_+10); A = W8(wTilesetUniqueGfx);
  alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(b_+10, b_+13); return; }
  CYC(b_+10, b_+13);
  SET_HL(uniqueGfxHeaderTable_bank04);
  CYC(b_+13, b_+17);
  add_double_index_to_hl(gb);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+19, b_+20);
  for (;;) {
    CYC(b_+20, b_+23);
    load_unique_gfx_header_entry(gb);
    alu_add(gb, A);
    if (F & FC) { CYCT(b_+23, b_+26); continue; }
    CYC(b_+23, b_+27);
    break;
  }
}

void loadTilesetUniqueGfx_hook(GB *gb) {
  load_tileset_unique_gfx(gb);
  ret_effect(gb);
}

void loadTilesetGraphics_hook(GB *gb) {
  BASE(loadTilesetGraphics);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+6); A = W8(wTilesetGfx);
  CYC(b_+6, b_+9);
  load_gfx_header(gb, SYM(loadGfxHeader), gfxHeaderTable_bank01, true);
  CYC(b_+9, b_+12); A = W8(wTilesetPalette);
  CALL_ROM(b_+12, ROM_loadPaletteHeader);
  CYC(b_+15, b_+18);
  load_tileset_unique_gfx(gb);
  switch_bank(gb, b_+18, 0x04);
  CALL_ROM(b_+25, ROM_b04_initializeAnimations);
  SET_HL(ROM_b02_func_02_7a77);
  E = 0x02;
  CYC(b_+28, b_+33);
  CALL_ROM(b_+33, ROM_interBankCall);
  SET_HL(ROM_b02_checkLoadPastSignAndChestGfx);
  E = 0x02;
  CYC(b_+36, b_+41);
  CALL_ROM(b_+41, ROM_interBankCall);
  CYC(b_+44, b_+47); A = W8(wTilesetUniqueGfx);
  CYC(b_+47, b_+50); W8(wLoadedTilesetUniqueGfx) = A;
  CYC(b_+50, b_+53); A = W8(wTilesetPalette);
  CYC(b_+53, b_+56); W8(wLoadedTilesetPalette) = A;
  CYC(b_+56, b_+59); A = W8(wTilesetAnimation);
  CYC(b_+59, b_+62); W8(wLoadedTilesetAnimation) = A;
  CYC(b_+62, b_+63); SET_AF(pop_effect(gb));
  CYC(b_+63, b_+65); H8(hRomBank) = A;
  CYC(b_+65, b_+68); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+68, b_+69);
  ret_effect(gb);
}

void loadTilesetData_hook(GB *gb) {
  BASE(loadTilesetData);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  switch_bank(gb, b_+3, 0x04);
  CALL_ROM(b_+10, ROM_b04_loadTilesetData_body);
  SET_HL(ROM_b02_updateTilesetFlagsForIndoorRoomInAltWorld);
  E = 0x02;
  CYC(b_+13, b_+18);
  CALL_ROM(b_+18, ROM_interBankCall);
  CYC(b_+21, b_+22); SET_AF(pop_effect(gb));
  CYC(b_+22, b_+24); H8(hRomBank) = A;
  CYC(b_+24, b_+27); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+27, b_+28);
  ret_effect(gb);
}

void loadTilesetAndRoomLayout_hook(GB *gb) {
  BASE(loadTilesetAndRoomLayout);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+6); A = W8(wLoadedTilesetLayout);
  B = A;
  CYC(b_+6, b_+10); A = W8(wTilesetLayout);
  alu_cp(gb, B);
  CYC(b_+10, b_+14); W8(wLoadedTilesetLayout) = A;
  if (!(F & FZ)) CALL_ROM_CC(b_+14, ROM_loadTilesetLayout);
  else CYC(b_+14, b_+17);
  CALL_ROM(b_+17, ROM_loadRoomLayout);
  switch_bank(gb, b_+20, 0x04);
  CALL_ROM(b_+27, ROM_b04_applyAllTileSubstitutions);
  A = 0x03;
  CYC(b_+30, b_+34); mem_wr(gb, IO_SVBK, A);
  SET_HL(w3RoomLayoutBuffer);
  SET_DE(wRoomLayout);
  B = 0xc0;
  CYC(b_+34, b_+42);
  CALL_ROM(b_+42, ROM_copyMemoryReverse);
  alu_xor(gb, A);
  CYC(b_+45, b_+48); mem_wr(gb, IO_SVBK, A);
  CYC(b_+48, b_+49); SET_AF(pop_effect(gb));
  CYC(b_+49, b_+51); H8(hRomBank) = A;
  CYC(b_+51, b_+54); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+54, b_+55);
  ret_effect(gb);
}

static void inter_bank_call(GB *gb, uint16_t a, uint8_t bank, uint16_t target) {
  SET_HL(target);
  E = bank;
  CYC(a, a + 5);
  CALL_ROM(a + 5, ROM_interBankCall);
}

void initializeRoom_hook(GB *gb) {
  BASE(initializeRoom);
  inter_bank_call(gb, b_+0, 0x01, ROM_b01_clearSolidObjectPositions);
  CYC(b_+8, b_+11); A = W8(wSentBackByStrangeForce);
  A = alu_dec8(gb, A);
  if (F & FZ) {
    CYC(b_+11, b_+14);
    B = 0x7c;
    CYC(b_+14, b_+19);
    objectCreateInteractionWithSubid00_hook(gb);
    return;
  }
  CYCT(b_+11, b_+14);
  inter_bank_call(gb, b_+19, 0x02, ROM_b02_calculateRoomStateModifier);
  CALL_ROM(b_+27, ROM_refreshObjectGfx);
  inter_bank_call(gb, b_+30, 0x12, ROM_b12_runRoomSpecificCode);
  inter_bank_call(gb, b_+38, 0x02, ROM_b02_createSeaEffectsPartIfApplicable);
  inter_bank_call(gb, b_+46, 0x01, ROM_b01_checkLoadPirateShip);
  bank_push(gb, b_+54, 0x02);
  CALL_ROM(b_+64, ROM_checkSpawnTimeportalInteraction);
  CYC(b_+67, b_+70); A = W8(wcc05);
  alu_bit(gb, 2, A);
  CYC(b_+70, b_+72);
  if (!(F & FZ)) CALL_ROM_CC(b_+72, ROM_b02_loadRememberedCompanion);
  else CYC(b_+72, b_+75);
  CYC(b_+75, b_+78); A = W8(wcc05);
  alu_bit(gb, 3, A);
  CYC(b_+78, b_+80);
  if (!(F & FZ)) CALL_ROM_CC(b_+80, ROM_b02_checkAndSpawnMaple);
  else CYC(b_+80, b_+83);
  switch_bank(gb, b_+83, 0x12);
  CYC(b_+90, b_+93); A = W8(wcc05);
  alu_bit(gb, 0, A);
  CYC(b_+93, b_+95);
  if (!(F & FZ)) CALL_ROM_CC(b_+95, ROM_b12_parseObjectData);
  else CYC(b_+95, b_+98);
  switch_bank(gb, b_+98, 0x16);
  CALL_ROM(b_+105, ROM_b16_parseStaticObjects);
  bank_pop(gb, b_+108);
  CYC(b_+114, b_+115);
  ret_effect(gb);
}

void loadStaticObjects_hook(GB *gb) {
  BASE(loadStaticObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, GV(0x16, 0x15));
  CYC(b_+10, b_+11); push_effect(gb, DE);
  CALL_C(b_+11, loadStaticObjects_body_hook, SYM(loadStaticObjects_body), b_+14);
  CYC(b_+14, b_+15); SET_DE(pop_effect(gb));
  bank_pop(gb, b_+15);
  CYC(b_+21, b_+22);
  ret_effect(gb);
}

void objectSaveAsStaticObject_hook(GB *gb) {
  BASE(objectSaveAsStaticObject);
  CYC(b_+0, b_+1); mem_wr(gb, HL, A);
  CYC(b_+1, b_+3); A = H8(hActiveObjectType);
  alu_add(gb, 0x16);
  E = A;
  A = L;
  CYC(b_+3, b_+8); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+8, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); A = W8(wActiveRoom);
  SET_HL(HL + 1);
  CYC(b_+14, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+18); A = H8(hActiveObjectType);
  A = alu_inc8(gb, A);
  E = A;
  CYC(b_+18, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E);
  CYC(b_+22, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = E;
  alu_add(gb, 0x09);
  E = A;
  CYC(b_+25, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(b_+31, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+35, b_+36);
  ret_effect(gb);
}

void objectMarkSolidPosition_hook(GB *gb) {
  BASE(objectMarkSolidPosition);
  CYC(b_+0, b_+3);
  object_get_short_position(gb);
  B = A;
  A = 0x02;
  CYC(b_+3, b_+8); mem_wr(gb, IO_SVBK, A);
  A = B;
  SET_HL(w2SolidObjectPositions);
  CYC(b_+8, b_+15);
  set_flag(gb);
  A = 0x00;
  CYC(b_+15, b_+19); mem_wr(gb, IO_SVBK, A);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

// bank-switched wrappers around single bodies

static void banked_body_call(GB *gb, uint16_t a, uint8_t bank, uint16_t target) {
  bank_push(gb, a, bank);
  CALL_ROM(a + 10, target);
  bank_pop(gb, a + 13);
  CYC(a + 19, a + 20);
  ret_effect(gb);
}

void disableLcdAndLoadRoom_hook(GB *gb) {
  BASE(disableLcdAndLoadRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, 0x03);
  CALL_C(b_+10, disableLcdAndLoadRoom_body_hook, SYM(disableLcdAndLoadRoom_body), b_+13);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20); ret_effect(gb);
}
void playWaveSoundAtRandomIntervals_hook(GB *gb) { banked_body_call(gb, SYM(playWaveSoundAtRandomIntervals), 0x10, ROM_b10_playWaveSoundAtRandomIntervals_body); }
void func_3ed0_hook(GB *gb) { banked_body_call(gb, SYM(func_3ed0), 0x03, ROM_b03_func_03_7841); }
void func_3ee4_hook(GB *gb) { banked_body_call(gb, SYM(func_3ee4), 0x03, ROM_b03_func_03_7849); }
void endgameCutsceneHandler_hook(GB *gb) { banked_body_call(gb, SYM(endgameCutsceneHandler), 0x03, ROM_b03_endgameCutsceneHandler_body); }

void fileSelect_redrawDecorations_hook(GB *gb) {
  BASE(fileSelect_redrawDecorations);
  bank_push(gb, b_+0, 0x02);
  CALL_ROM(b_+10, ROM_b02_fileSelect_redrawDecorationsAndSetWramBank4);
  bank_pop(gb, b_+13);
  alu_xor(gb, A);
  CYC(b_+19, b_+22); mem_wr(gb, IO_SVBK, A);
  CYC(b_+22, b_+23);
  ret_effect(gb);
}

void func_2d48_hook(GB *gb) {
  BASE(func_2d48);
  bank_push(gb, b_+0, GV(0x3f, 0x03));
  A = B;
  SET_HL(data_5951_bank3f);
  CYC(b_+10, b_+15);
  add_a_to_hl(gb);
  CYC(b_+15, b_+16); B = mem_rd(gb, HL);
  bank_pop(gb, b_+16);
  CYC(b_+22, b_+23);
  ret_effect(gb);
}

void func_32fc_hook(GB *gb) {
  BASE(func_32fc);
  CYC(b_+0, b_+3);
  set_palette_thread_delay(gb);
  A = 0x0d;
  B = 0xf0;
  CYC(b_+3, b_+10); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+10, b_+14);
  set_darkening_variables(gb);
  ret_effect(gb);
}

void func_333e_hook(GB *gb) {
  BASE(func_333e);
  CYC(b_+0, b_+3);
  set_palette_thread_delay(gb);
  A = 0x0e;
  B = 0x00;
  CYC(b_+3, b_+10); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(b_+10, b_+14);
  set_darkening_variables(gb);
  ret_effect(gb);
}

// sound queue entry and music volume

void playSound_b00_hook(GB *gb) {
  BASE(playSound_b00);
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+0, b_+2); ret_effect(gb); return; }
  CYC(b_+0, b_+2);
  H = A;
  CYC(b_+2, b_+5); A = H8(hFFB7);
  alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); ret_effect(gb); return; }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); A = H8(hMusicQueueTail);
  L = A;
  A = H;
  H = wMusicQueue >> 8;
  CYC(b_+10, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = L;
  alu_and(gb, 0xaf);
  CYC(b_+15, b_+20); H8(hMusicQueueTail) = A;
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

void setMusicVolume_hook(GB *gb) {
  BASE(setMusicVolume);
  alu_or(gb, 0x80);
  CYC(b_+0, b_+4); H8(hMusicVolume) = A;
  CYC(b_+4, b_+5);
  ret_effect(gb);
}

// sequential ROM reads that cross into the next bank, and the graphics decompressor

static void adjust_hl_sequential(GB *gb) {
  BASE(_adjustHLSequential);
  A = H;
  alu_cp(gb, 0x80);
  if (!(F & FZ)) CYCT(b_+0, b_+5);
  else {
    CYC(b_+0, b_+5);
    H = 0x40;
    CYC(b_+5, b_+9); A = H8(hRomBank);
    A = alu_inc8(gb, A);
    CYC(b_+9, b_+12); H8(hRomBank) = A;
    CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, A);
  }
  A = B;
  alu_or(gb, C);
  CYC(b_+15, b_+18);
}

void _adjustHLSequential_hook(GB *gb) {
  adjust_hl_sequential(gb);
  ret_effect(gb);
}

static void copy_byte_sequential(GB *gb) {
  BASE(copyByteSequential);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); mem_wr(gb, DE, A);
  SET_DE(DE + 1);
  SET_BC(BC - 1);
  CYC(b_+2, b_+4);
  adjust_hl_sequential(gb);
}

void copyByteSequential_hook(GB *gb) {
  copy_byte_sequential(gb);
  ret_effect(gb);
}

static void decompress_block(GB *gb) {
  BASE(func_069c);
  CYC(b_+0, b_+3);
  read_byte_sequential(gb);
  C = A;
  CYC(b_+3, b_+7);
  read_byte_sequential(gb);
  CYC(b_+7, b_+9); H8(hFF8A) = A;
  alu_or(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+12);
    CYC(SYM(_label_00_050), (SYM(_label_00_050) + 3));
    read_byte_sequential(gb);
    CYC((SYM(_label_00_050) + 3), (SYM(_label_00_050) + 5)); H8(hFF8B) = A;
    B = 0x08;
    CYC((SYM(_label_00_050) + 5), (SYM(_label_00_050) + 7));
    for (;;) {
      C = alu_rl(gb, C);
      if (F & FC) { CYCT(SYM(_label_00_051), (SYM(_label_00_051) + 4)); CYC(SYM(_label_00_052), (SYM(_label_00_052) + 2)); A = H8(hFF8B); }
      else { CYC(SYM(_label_00_051), (SYM(_label_00_051) + 4)); CYC((SYM(_label_00_051) + 4), (SYM(_label_00_051) + 7)); read_byte_sequential(gb); CYC((SYM(_label_00_051) + 7), (SYM(_label_00_051) + 9)); }
      CYC(SYM(_label_00_053), (SYM(_label_00_053) + 1)); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      B = alu_dec8(gb, B);
      if (B) { CYCT((SYM(_label_00_053) + 1), (SYM(_label_00_053) + 5)); continue; }
      CYC((SYM(_label_00_053) + 1), (SYM(_label_00_053) + 5));
      break;
    }
    CYC((SYM(_label_00_053) + 5), (SYM(_label_00_053) + 7)); A = H8(hFF8A);
    C = A;
    B = 0x08;
    CYC((SYM(_label_00_053) + 7), (SYM(_label_00_053) + 10));
    for (;;) {
      C = alu_rl(gb, C);
      if (F & FC) { CYCT(SYM(_label_00_054), (SYM(_label_00_054) + 4)); CYC(SYM(_label_00_055), (SYM(_label_00_055) + 2)); A = H8(hFF8B); }
      else { CYC(SYM(_label_00_054), (SYM(_label_00_054) + 4)); CYC((SYM(_label_00_054) + 4), (SYM(_label_00_054) + 7)); read_byte_sequential(gb); CYC((SYM(_label_00_054) + 7), (SYM(_label_00_054) + 9)); }
      CYC(SYM(_label_00_056), (SYM(_label_00_056) + 1)); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      B = alu_dec8(gb, B);
      if (B) { CYCT((SYM(_label_00_056) + 1), (SYM(_label_00_056) + 5)); continue; }
      CYC((SYM(_label_00_056) + 1), (SYM(_label_00_056) + 6));
      return;
    }
  }
  CYC(b_+9, b_+12);
  B = 0x10;
  CYC(b_+12, b_+14);
  for (;;) {
    CYC(SYM(_label_00_049), (SYM(_label_00_049) + 3));
    read_byte_sequential(gb);
    CYC((SYM(_label_00_049) + 3), (SYM(_label_00_049) + 4)); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    B = alu_dec8(gb, B);
    if (B) { CYCT((SYM(_label_00_049) + 4), (SYM(_label_00_049) + 8)); continue; }
    CYC((SYM(_label_00_049) + 4), (SYM(_label_00_049) + 9));
    return;
  }
}

void func_069c_hook(GB *gb) {
  decompress_block(gb);
  ret_effect(gb);
}

static void copy_uncompressed(GB *gb) {
  BASE(func_06e0);
  for (;;) {
    C = 0x10;
    CYC(b_+0, b_+2);
    for (;;) {
      CYC(b_+2, b_+5);
      read_byte_sequential(gb);
      CYC(b_+5, b_+6); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      C = alu_dec8(gb, C);
      if (C) { CYCT(b_+6, b_+10); continue; }
      CYC(b_+6, b_+10);
      break;
    }
    B = alu_dec8(gb, B);
    if (B) { CYCT(b_+10, b_+13); continue; }
    CYC(b_+10, b_+14);
    return;
  }
}

void func_06e0_hook(GB *gb) {
  copy_uncompressed(gb);
  ret_effect(gb);
}

static void decompress_lz(GB *gb) {
  BASE(_label_00_068);
  CYC(SYM(_label_00_061), (SYM(_label_00_061) + 2)); H8(hFF8E) = A;
  B = alu_swap(gb, B);
  A = B;
  alu_and(gb, 0xf0);
  C = A;
  alu_xor(gb, B);
  B = A;
  A = 0x01;
  CYC((SYM(_label_00_061) + 2), (SYM(_label_00_061) + 14)); H8(hFF8B) = A;
  for (;;) {
    CYC(SYM(_label_00_062), (SYM(_label_00_062) + 2)); A = H8(hFF8B);
    A = alu_dec8(gb, A);
    CYC((SYM(_label_00_062) + 2), (SYM(_label_00_062) + 5)); H8(hFF8B) = A;
    if (!(F & FZ)) CYCT((SYM(_label_00_062) + 5), (SYM(_label_00_062) + 7));
    else {
      CYC((SYM(_label_00_062) + 5), (SYM(_label_00_062) + 7));
      A = 0x08;
      CYC((SYM(_label_00_062) + 7), (SYM(_label_00_062) + 11)); H8(hFF8B) = A;
      CYC((SYM(_label_00_062) + 11), (SYM(_label_00_062) + 12)); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC((SYM(_label_00_062) + 12), (SYM(_label_00_062) + 14)); H8(hFF8A) = A;
      CYC((SYM(_label_00_062) + 14), (SYM(_label_00_062) + 17));
      adjust_hl_sequential(gb);
    }
    CYC(SYM(_label_00_063), (SYM(_label_00_063) + 2)); A = H8(hFF8A);
    alu_add(gb, A);
    CYC((SYM(_label_00_063) + 2), (SYM(_label_00_063) + 5)); H8(hFF8A) = A;
    if (!(F & FC)) {
      CYC((SYM(_label_00_063) + 5), (SYM(_label_00_063) + 7));
      CYC((SYM(_label_00_063) + 7), (SYM(_label_00_063) + 10));
      copy_byte_sequential(gb);
      if (!(F & FZ)) { CYCT((SYM(_label_00_063) + 10), (SYM(_label_00_063) + 12)); continue; }
      CYC((SYM(_label_00_063) + 10), (SYM(_label_00_063) + 13));
      return;
    }
    CYCT((SYM(_label_00_063) + 5), (SYM(_label_00_063) + 7));
    CYC(SYM(_label_00_064), (SYM(_label_00_064) + 2)); A = H8(hFF8E);
    alu_or(gb, A);
    bool no_length = false;
    if (!(F & FZ)) {
      CYCT((SYM(_label_00_064) + 2), (SYM(_label_00_064) + 5));
      CYC(SYM(_label_00_065), (SYM(_label_00_065) + 1)); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC((SYM(_label_00_065) + 1), (SYM(_label_00_065) + 3)); H8(hFF92) = A;
      CYC((SYM(_label_00_065) + 3), (SYM(_label_00_065) + 6));
      adjust_hl_sequential(gb);
      CYC((SYM(_label_00_065) + 6), (SYM(_label_00_065) + 7)); A = mem_rd(gb, HL);
      alu_and(gb, 0x07);
      CYC((SYM(_label_00_065) + 7), (SYM(_label_00_065) + 11)); H8(hFF93) = A;
      CYC((SYM(_label_00_065) + 11), (SYM(_label_00_065) + 12)); alu_xor(gb, mem_rd(gb, HL));
      if (F & FZ) { CYCT((SYM(_label_00_065) + 12), (SYM(_label_00_065) + 14)); no_length = true; }
      else {
        CYC((SYM(_label_00_065) + 12), (SYM(_label_00_065) + 14));
        alu_rrca(gb);
        alu_rrca(gb);
        alu_rrca(gb);
        alu_add(gb, 0x02);
        CYC((SYM(_label_00_065) + 14), (SYM(_label_00_065) + 21));
      }
    } else {
      CYC((SYM(_label_00_064) + 2), (SYM(_label_00_064) + 5));
      CYC((SYM(_label_00_064) + 5), (SYM(_label_00_064) + 6)); A = mem_rd(gb, HL);
      alu_and(gb, 0x1f);
      CYC((SYM(_label_00_064) + 6), (SYM(_label_00_064) + 10)); H8(hFF92) = A;
      CYC((SYM(_label_00_064) + 10), (SYM(_label_00_064) + 11)); alu_xor(gb, mem_rd(gb, HL));
      if (F & FZ) { CYCT((SYM(_label_00_064) + 11), (SYM(_label_00_064) + 13)); no_length = true; }
      else {
        CYC((SYM(_label_00_064) + 11), (SYM(_label_00_064) + 13));
        A = alu_swap(gb, A);
        alu_rrca(gb);
        A = alu_inc8(gb, A);
        CYC((SYM(_label_00_064) + 13), (SYM(_label_00_064) + 19));
      }
    }
    if (no_length) {
      SET_HL(HL + 1);
      CYC(SYM(_label_00_066), (SYM(_label_00_066) + 4));
      adjust_hl_sequential(gb);
      CYC((SYM(_label_00_066) + 4), (SYM(_label_00_066) + 5)); A = mem_rd(gb, HL);
    }
    CYC(SYM(_label_00_067), (SYM(_label_00_067) + 2)); H8(hFF8F) = A;
    SET_HL(HL + 1);
    CYC((SYM(_label_00_067) + 2), (SYM(_label_00_067) + 6));
    adjust_hl_sequential(gb);
    CYC((SYM(_label_00_067) + 6), (SYM(_label_00_067) + 7)); push_effect(gb, HL);
    CYC((SYM(_label_00_067) + 7), (SYM(_label_00_067) + 9)); A = H8(hFF92);
    alu_cpl(gb);
    L = A;
    CYC((SYM(_label_00_067) + 9), (SYM(_label_00_067) + 13)); A = H8(hFF93);
    alu_cpl(gb);
    H = A;
    alu_add_hl(gb, DE);
    CYC((SYM(_label_00_067) + 13), (SYM(_label_00_067) + 16));
    for (;;) {
      CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+1, b_+2); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      SET_BC(BC - 1);
      A = B;
      alu_or(gb, C);
      if (F & FZ) {
        CYCT(b_+2, b_+8);
        CYC(SYM(_label_00_069), (SYM(_label_00_069) + 1)); SET_HL(pop_effect(gb));
        CYC((SYM(_label_00_069) + 1), (SYM(_label_00_069) + 2));
        return;
      }
      CYC(b_+2, b_+8);
      CYC(b_+8, b_+10); A = H8(hFF8F);
      A = alu_dec8(gb, A);
      CYC(b_+10, b_+13); H8(hFF8F) = A;
      if (!(F & FZ)) { CYCT(b_+13, b_+15); continue; }
      CYC(b_+13, b_+15);
      CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
      CYC(b_+16, b_+18);
      break;
    }
  }
}

void decompressGraphics_hook(GB *gb) {
  BASE(decompressGraphics);
  A = E;
  alu_and(gb, 0x0f);
  CYC(b_+0, b_+5); mem_wr(gb, IO_VBK, A);
  CYC(b_+5, b_+7); mem_wr(gb, IO_SVBK, A);
  alu_xor(gb, E);
  E = A;
  A = C;
  alu_and(gb, 0x3f);
  CYC(b_+7, b_+14); H8(hRomBank) = A;
  CYC(b_+14, b_+17); mem_wr(gb, MBC_ROM_BANK, A);
  B = alu_inc8(gb, B);
  A = C;
  alu_and(gb, 0xc0);
  if (F & FZ) { CYCT(b_+17, b_+24); copy_uncompressed(gb); ret_effect(gb); return; }
  CYC(b_+17, b_+24);
  alu_cp(gb, 0xc0);
  if (F & FZ) { CYCT(b_+24, b_+28); A = 0xff; CYC(SYM(_label_00_059), (SYM(_label_00_059) + 4)); decompress_lz(gb); ret_effect(gb); return; }
  CYC(b_+24, b_+28);
  alu_cp(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+28, b_+32);
    alu_xor(gb, A);
    CYC(SYM(_label_00_060), (SYM(_label_00_060) + 3)); H8(hFF93) = A;
    decompress_lz(gb);
    ret_effect(gb);
    return;
  }
  CYC(b_+28, b_+32);
  A = B;
  CYC(b_+32, b_+33);
  for (;;) {
    CYC(b_+33, b_+34); push_effect(gb, AF);
    CYC(b_+34, b_+37);
    decompress_block(gb);
    CYC(b_+37, b_+38); SET_AF(pop_effect(gb));
    A = alu_dec8(gb, A);
    if (A) { CYCT(b_+38, b_+41); continue; }
    CYC(b_+38, b_+42);
    break;
  }
  ret_effect(gb);
}

static bool decompress_call_hook(GB *gb, void (*fn)(GB *), uint16_t target,
                                 uint16_t ra, uint16_t sp0_) {
  push_effect(gb, (uint16_t)(ra));
  uint16_t sp_ = cpu_sp(gb);
  if (hook_in_verify || !hook_enabled_at(gb, target)) asm_call(gb, target, ra);
  else {
    fn(gb);
    if (!(cpu_pc(gb) == ra && cpu_sp(gb) == (uint16_t)(sp_ + 2))) {
      hook_continue(gb, cpu_pc(gb), sp0_);
      return true;
    }
  }
  return false;
}

#define CALL_C_FRAGMENT(a, fn, target, ra) do { \
  CYC((a), (a) + 3); \
  if (decompress_call_hook(gb, fn, (target), (ra), sp0_)) return true; \
} while (0)

static bool decompress_label_054(GB *gb, uint16_t sp0_);
static bool decompress_label_056(GB *gb, uint16_t sp0_);
static bool decompress_label_062(GB *gb, uint16_t sp0_);
static bool decompress_label_063(GB *gb, uint16_t sp0_);
static bool decompress_label_064(GB *gb, uint16_t sp0_);
static bool decompress_label_065(GB *gb, uint16_t sp0_);
static bool decompress_label_066(GB *gb, uint16_t sp0_);
static bool decompress_label_067(GB *gb, uint16_t sp0_);
static bool decompress_label_068(GB *gb, uint16_t sp0_);

static bool decompress_label_049(GB *gb, uint16_t sp0_) {
  BASE(_label_00_049);
  for (;;) {
    CALL_C_FRAGMENT(b_+0, readByteSequential_hook, SYM(readByteSequential), b_+3);
    CYC(b_+3, b_+4); mem_wr(gb, DE, A);
    CYC(b_+4, b_+5); SET_DE(DE + 1);
    CYC(b_+5, b_+6); B = alu_dec8(gb, B);
    if (B) { CYCT(b_+6, b_+8); continue; }
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9);
    return false;
  }
}

static bool decompress_label_053(GB *gb, uint16_t sp0_) {
  BASE(_label_00_053);
  for (;;) {
    CYC(b_+0, b_+1); mem_wr(gb, DE, A);
    CYC(b_+1, b_+2); SET_DE(DE + 1);
    CYC(b_+2, b_+3); B = alu_dec8(gb, B);
    if (B) {
      CYCT(b_+3, b_+5);
      CYC(SYM(_label_00_051), (SYM(_label_00_051) + 2)); C = alu_rl(gb, C);
      if (F & FC) { CYCT((SYM(_label_00_051) + 2), (SYM(_label_00_051) + 4)); CYC(SYM(_label_00_052), (SYM(_label_00_052) + 2)); A = H8(hFF8B); }
      else { CYC((SYM(_label_00_051) + 2), (SYM(_label_00_051) + 4)); CALL_C_FRAGMENT((SYM(_label_00_051) + 4), readByteSequential_hook, SYM(readByteSequential), (SYM(_label_00_051) + 7)); CYC((SYM(_label_00_051) + 7), (SYM(_label_00_051) + 9)); }
      continue;
    }
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+7); A = H8(hFF8A);
    CYC(b_+7, b_+8); C = A;
    CYC(b_+8, b_+10); B = 0x08;
    return decompress_label_054(gb, sp0_);
  }
}

static bool decompress_label_051(GB *gb, uint16_t sp0_) {
  BASE(_label_00_051);
  CYC(b_+0, b_+2); C = alu_rl(gb, C);
  if (F & FC) { CYCT(b_+2, b_+4); CYC(SYM(_label_00_052), (SYM(_label_00_052) + 2)); A = H8(hFF8B); }
  else { CYC(b_+2, b_+4); CALL_C_FRAGMENT(b_+4, readByteSequential_hook, SYM(readByteSequential), b_+7); CYC(b_+7, b_+9); }
  return decompress_label_053(gb, sp0_);
}

static bool decompress_label_050(GB *gb, uint16_t sp0_) {
  BASE(_label_00_050);
  CALL_C_FRAGMENT(b_+0, readByteSequential_hook, SYM(readByteSequential), b_+3);
  CYC(b_+3, b_+5); H8(hFF8B) = A;
  CYC(b_+5, b_+7); B = 0x08;
  return decompress_label_051(gb, sp0_);
}

static bool decompress_label_054(GB *gb, uint16_t sp0_) {
  BASE(_label_00_054);
  CYC(b_+0, b_+2); C = alu_rl(gb, C);
  if (F & FC) { CYCT(b_+2, b_+4); CYC(SYM(_label_00_055), (SYM(_label_00_055) + 2)); A = H8(hFF8B); }
  else { CYC(b_+2, b_+4); CALL_C_FRAGMENT(b_+4, readByteSequential_hook, SYM(readByteSequential), b_+7); CYC(b_+7, b_+9); }
  return decompress_label_056(gb, sp0_);
}

static bool decompress_label_056(GB *gb, uint16_t sp0_) {
  BASE(_label_00_056);
  CYC(b_+0, b_+1); mem_wr(gb, DE, A);
  CYC(b_+1, b_+2); SET_DE(DE + 1);
  CYC(b_+2, b_+3); B = alu_dec8(gb, B);
  if (B) { CYCT(b_+3, b_+5); return decompress_label_054(gb, sp0_); }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6);
  return false;
}

static bool decompress_label_069(GB *gb, uint16_t sp0_) {
  BASE(_label_00_069);
  (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2);
  return false;
}

static bool decompress_label_068(GB *gb, uint16_t sp0_) {
  BASE(_label_00_068);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+1, b_+2); mem_wr(gb, DE, A);
    CYC(b_+2, b_+3); SET_DE(DE + 1);
    CYC(b_+3, b_+4); SET_BC(BC - 1);
    CYC(b_+4, b_+5); A = B;
    CYC(b_+5, b_+6); alu_or(gb, C);
    if (F & FZ) {
      CYCT(b_+6, b_+8);
      return decompress_label_069(gb, sp0_);
    }
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+10); A = H8(hFF8F);
    CYC(b_+10, b_+11); A = alu_dec8(gb, A);
    CYC(b_+11, b_+13); H8(hFF8F) = A;
    if (!(F & FZ)) { CYCT(b_+13, b_+15); continue; }
    CYC(b_+13, b_+15);
    CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
    CYC(b_+16, b_+18);
    return decompress_label_062(gb, sp0_);
  }
}

static bool decompress_label_067(GB *gb, uint16_t sp0_) {
  BASE(_label_00_067);
  CYC(b_+0, b_+2); H8(hFF8F) = A;
  CYC(b_+2, b_+3); SET_HL(HL + 1);
  CALL_C_FRAGMENT(b_+3, _adjustHLSequential_hook, SYM(_adjustHLSequential), b_+6);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+9); A = H8(hFF92);
  CYC(b_+9, b_+10); alu_cpl(gb);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+13); A = H8(hFF93);
  CYC(b_+13, b_+14); alu_cpl(gb);
  CYC(b_+14, b_+15); H = A;
  CYC(b_+15, b_+16); alu_add_hl(gb, DE);
  return decompress_label_068(gb, sp0_);
}

static bool decompress_label_066(GB *gb, uint16_t sp0_) {
  BASE(_label_00_066);
  CYC(b_+0, b_+1); SET_HL(HL + 1);
  CALL_C_FRAGMENT(b_+1, _adjustHLSequential_hook, SYM(_adjustHLSequential), b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  return decompress_label_067(gb, sp0_);
}

static bool decompress_label_065(GB *gb, uint16_t sp0_) {
  BASE(_label_00_065);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+3); H8(hFF92) = A;
  CALL_C_FRAGMENT(b_+3, _adjustHLSequential_hook, SYM(_adjustHLSequential), b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_and(gb, 0x07);
  CYC(b_+9, b_+11); H8(hFF93) = A;
  CYC(b_+11, b_+12); alu_xor(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+12, b_+14); return decompress_label_066(gb, sp0_); }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+16); alu_rrca(gb);
  CYC(b_+16, b_+17); alu_rrca(gb);
  CYC(b_+17, b_+19); alu_add(gb, 0x02);
  CYC(b_+19, b_+21);
  return decompress_label_067(gb, sp0_);
}

static bool decompress_label_064(GB *gb, uint16_t sp0_) {
  BASE(_label_00_064);
  CYC(b_+0, b_+2); A = H8(hFF8E);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); return decompress_label_065(gb, sp0_); }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x1f);
  CYC(b_+8, b_+10); H8(hFF92) = A;
  CYC(b_+10, b_+11); alu_xor(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+11, b_+13); return decompress_label_066(gb, sp0_); }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = alu_swap(gb, A);
  CYC(b_+15, b_+16); alu_rrca(gb);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CYC(b_+17, b_+19);
  return decompress_label_067(gb, sp0_);
}

static bool decompress_label_063(GB *gb, uint16_t sp0_) {
  BASE(_label_00_063);
  CYC(b_+0, b_+2); A = H8(hFF8A);
  CYC(b_+2, b_+3); alu_add(gb, A);
  CYC(b_+3, b_+5); H8(hFF8A) = A;
  if (!(F & FC)) {
    CYC(b_+5, b_+7);
    CALL_C_FRAGMENT(b_+7, copyByteSequential_hook, SYM(copyByteSequential), b_+10);
    if (!(F & FZ)) { CYCT(b_+10, b_+12); return decompress_label_062(gb, sp0_); }
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+13);
    return false;
  }
  CYCT(b_+5, b_+7);
  return decompress_label_064(gb, sp0_);
}

static bool decompress_label_062(GB *gb, uint16_t sp0_) {
  BASE(_label_00_062);
  for (;;) {
    CYC(b_+0, b_+2); A = H8(hFF8B);
    CYC(b_+2, b_+3); A = alu_dec8(gb, A);
    CYC(b_+3, b_+5); H8(hFF8B) = A;
    if (!(F & FZ)) { CYCT(b_+5, b_+7); return decompress_label_063(gb, sp0_); }
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+9); A = 0x08;
    CYC(b_+9, b_+11); H8(hFF8B) = A;
    CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+12, b_+14); H8(hFF8A) = A;
    CALL_C_FRAGMENT(b_+14, _adjustHLSequential_hook, SYM(_adjustHLSequential), SYM(_label_00_063));
    return decompress_label_063(gb, sp0_);
  }
}

static bool decompress_label_061(GB *gb, uint16_t sp0_) {
  BASE(_label_00_061);
  CYC(b_+0, b_+2); H8(hFF8E) = A;
  CYC(b_+2, b_+4); B = alu_swap(gb, B);
  CYC(b_+4, b_+5); A = B;
  CYC(b_+5, b_+7); alu_and(gb, 0xf0);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); alu_xor(gb, B);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+14); H8(hFF8B) = A;
  return decompress_label_062(gb, sp0_);
}

#define DECOMPRESS_HOOK(fn) do { \
  if (fn(gb, sp0_)) return; \
  ret_effect(gb); \
} while (0)

void _label_00_049_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_049); }
void _label_00_050_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_050); }
void _label_00_051_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_051); }
void _label_00_052_hook(GB *gb) {BASE(_label_00_052); uint16_t sp0_ = gb->sp; CYC(b_+0, b_+2); A = H8(hFF8B); DECOMPRESS_HOOK(decompress_label_053); }
void _label_00_053_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_053); }
void _label_00_054_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_054); }
void _label_00_055_hook(GB *gb) {BASE(_label_00_055); uint16_t sp0_ = gb->sp; CYC(b_+0, b_+2); A = H8(hFF8B); DECOMPRESS_HOOK(decompress_label_056); }
void _label_00_056_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_056); }
void _label_00_059_hook(GB *gb) {BASE(_label_00_059); uint16_t sp0_ = gb->sp; CYC(b_+0, b_+2); A = 0xff; CYC(b_+2, b_+4); DECOMPRESS_HOOK(decompress_label_061); }
void _label_00_060_hook(GB *gb) {BASE(_label_00_060); uint16_t sp0_ = gb->sp; CYC(b_+0, b_+1); alu_xor(gb, A); CYC(b_+1, b_+3); H8(hFF93) = A; DECOMPRESS_HOOK(decompress_label_061); }
void _label_00_061_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_061); }
void _label_00_062_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_062); }
void _label_00_063_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_063); }
void _label_00_064_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_064); }
void _label_00_065_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_065); }
void _label_00_066_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_066); }
void _label_00_067_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_067); }
void _label_00_068_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_068); }
void _label_00_069_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_069); }

#undef DECOMPRESS_HOOK
#undef CALL_C_FRAGMENT

// palette headers: entries of (flags, count, source pointer) copied into w2 palette buffers

void loadPaletteHeader_hook(GB *gb) {
  BASE(loadPaletteHeader);
  CYC(b_+0, b_+1); push_effect(gb, DE);
  L = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, IO_SVBK);
  C = A;
  CYC(b_+4, b_+7); A = H8(hRomBank);
  B = A;
  CYC(b_+7, b_+9); push_effect(gb, BC);
  A = 0x02;
  CYC(b_+9, b_+13); mem_wr(gb, IO_SVBK, A);
  A = 0x01;
  CYC(b_+13, b_+17); H8(hRomBank) = A;
  CYC(b_+17, b_+20); mem_wr(gb, MBC_ROM_BANK, A);
  A = L;
  SET_HL(paletteHeaderTable_bank01);
  CYC(b_+20, b_+25);
  add_double_index_to_hl(gb);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+27, b_+28);
  for (;;) {
    A = 0x01;
    CYC(b_+28, b_+32); H8(hRomBank) = A;
    CYC(b_+32, b_+35); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(b_+35, b_+36); A = mem_rd(gb, HL);
    alu_and(gb, 0x07);
    A = alu_inc8(gb, A);
    B = A;
    CYC(b_+36, b_+41); A = mem_rd(gb, HL);
    alu_rlca(gb);
    A = alu_swap(gb, A);
    alu_and(gb, 0x07);
    SET_DE(bitTable);
    alu_add(gb, E);
    E = A;
    CYC(b_+41, b_+52); A = mem_rd(gb, DE);
    C = A;
    alu_xor(gb, A);
    CYC(b_+52, b_+54);
    for (;;) {
      alu_or(gb, C);
      B = alu_dec8(gb, B);
      if (F & FZ) { CYCT(b_+54, b_+58); break; }
      CYC(b_+54, b_+58);
      alu_rlca(gb);
      CYC(b_+58, b_+61);
    }
    B = A;
    C = GV(0xa6, 0xa4);
    CYC(b_+61, b_+66); alu_bit(gb, 6, mem_rd(gb, HL));
    if (F & FZ) CYCT(b_+66, b_+68);
    else { CYC(b_+66, b_+70); C = GV(0xa7, 0xa5); }
    CYC(b_+70, b_+71); A = mem_rd(gb, 0xff00 | C);
    alu_or(gb, B);
    CYC(b_+71, b_+73); mem_wr(gb, 0xff00 | C, A);
    CYC(b_+73, b_+74); A = mem_rd(gb, HL);
    alu_and(gb, 0x78);
    alu_add(gb, 0x80);
    E = A;
    D = 0xde;
    CYC(b_+74, b_+82); A = mem_rd(gb, HL);
    alu_and(gb, 0x07);
    A = alu_inc8(gb, A);
    B = A;
    CYC(b_+82, b_+87); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_rlca(gb);
    CYC(b_+87, b_+89); A = mem_rd(gb, HL); SET_HL(HL + 1);
    C = A;
    CYC(b_+89, b_+91); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+91, b_+92); push_effect(gb, HL);
    L = C;
    H = A;
    A = GV(0x17, 0x16);
    CYC(b_+92, b_+98); H8(hRomBank) = A;
    CYC(b_+98, b_+101); mem_wr(gb, MBC_ROM_BANK, A);
    for (;;) {
      C = 0x08;
      CYC(b_+101, b_+103);
      for (;;) {
        CYC(b_+103, b_+104); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+104, b_+105); mem_wr(gb, DE, A);
        E = alu_inc8(gb, E);
        C = alu_dec8(gb, C);
        if (C) { CYCT(b_+105, b_+109); continue; }
        CYC(b_+105, b_+109);
        break;
      }
      B = alu_dec8(gb, B);
      if (B) { CYCT(b_+109, b_+112); continue; }
      CYC(b_+109, b_+112);
      break;
    }
    CYC(b_+112, b_+113); SET_HL(pop_effect(gb));
    if (F & FC) { CYCT(b_+113, b_+115); continue; }
    CYC(b_+113, b_+115);
    break;
  }
  CYC(b_+115, b_+116); SET_BC(pop_effect(gb));
  A = B;
  CYC(b_+116, b_+119); H8(hRomBank) = A;
  CYC(b_+119, b_+122); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(b_+122, b_+125); mem_wr(gb, IO_SVBK, A);
  CYC(b_+125, b_+126); SET_DE(pop_effect(gb));
  CYC(b_+126, b_+127);
  ret_effect(gb);
}

// tileset graphics helper: bit-packed literal/back-reference stream into VRAM

void loadTilesetHlpr_hook(GB *gb) {
  BASE(loadTilesetHlpr);
  A = E;
  alu_and(gb, 0x0f);
  CYC(b_+0, b_+5); mem_wr(gb, IO_VBK, A);
  CYC(b_+5, b_+7); mem_wr(gb, IO_SVBK, A);
  alu_xor(gb, E);
  E = A;
  CYC(b_+7, b_+9);
  for (;;) {
    CYC(b_+9, b_+11); A = H8(hFF8E);
    CYC(b_+11, b_+13); H8(hRomBank) = A;
    CYC(b_+13, b_+16); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+17, b_+19); H8(hFF8B) = A;
    B = 0x08;
    CYC(b_+19, b_+21);
    for (;;) {
      CYC(b_+21, b_+23); A = H8(hFF8E);
      CYC(b_+23, b_+25); H8(hRomBank) = A;
      CYC(b_+25, b_+28); mem_wr(gb, MBC_ROM_BANK, A);
      CYC(b_+28, b_+30); A = H8(hFF8B);
      alu_rrca(gb);
      CYC(b_+30, b_+33); H8(hFF8B) = A;
      if (F & FC) {
        CYCT(b_+33, b_+35);
        CYC(b_+47, b_+48); push_effect(gb, BC);
        CYC(b_+48, b_+50); A = H8(hFF8F);
        alu_bit(gb, 7, A);
        if (!(F & FZ)) {
          CYCT(b_+50, b_+54);
          CYC(b_+74, b_+75); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(b_+75, b_+77); H8(hFF8A) = A;
          CYC(b_+77, b_+78); A = mem_rd(gb, HL); SET_HL(HL + 1);
          C = A;
          CYC(b_+78, b_+80); A = mem_rd(gb, HL); SET_HL(HL + 1);
          B = A;
          CYC(b_+80, b_+81);
        } else {
          CYC(b_+50, b_+54);
          CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
          C = A;
          CYC(b_+55, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(b_+57, b_+59); H8(hFF8A) = A;
          alu_and(gb, 0x0f);
          B = A;
          CYC(b_+59, b_+64); A = H8(hFF8A);
          A = alu_swap(gb, A);
          alu_and(gb, 0x0f);
          alu_add(gb, 0x03);
          CYC(b_+64, b_+72); H8(hFF8A) = A;
          CYC(b_+72, b_+74);
        }
        CYC(b_+81, b_+82); push_effect(gb, HL);
        SET_HL(hFF90);
        CYC(b_+82, b_+86); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+86, b_+87); H = mem_rd(gb, HL);
        L = A;
        alu_add_hl(gb, BC);
        CYC(b_+87, b_+91); A = H8(hFF8A);
        B = A;
        CYC(b_+91, b_+94); A = H8(hFF8F);
        alu_and(gb, 0x3f);
        CYC(b_+94, b_+98); H8(hRomBank) = A;
        CYC(b_+98, b_+101); mem_wr(gb, MBC_ROM_BANK, A);
        for (;;) {
          CYC(b_+101, b_+102); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(b_+102, b_+103); mem_wr(gb, DE, A);
          SET_DE(DE + 1);
          CYC(b_+103, b_+107);
          dec16_ff8c(gb);
          if (F & FZ) {
            CYCT(b_+107, b_+109);
            CYC(b_+119, b_+120); SET_HL(pop_effect(gb));
            CYC(b_+120, b_+121); SET_BC(pop_effect(gb));
            CYC(b_+121, b_+122);
            ret_effect(gb);
            return;
          }
          CYC(b_+107, b_+109);
          B = alu_dec8(gb, B);
          if (B) { CYCT(b_+109, b_+112); continue; }
          CYC(b_+109, b_+112);
          break;
        }
        CYC(b_+112, b_+113); SET_HL(pop_effect(gb));
        CYC(b_+113, b_+114); SET_BC(pop_effect(gb));
        B = alu_dec8(gb, B);
        if (B) { CYCT(b_+114, b_+117); continue; }
        CYC(b_+114, b_+117);
        CYC(b_+117, b_+119);
        break;
      }
      CYC(b_+33, b_+35);
      CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+36, b_+37); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      CYC(b_+37, b_+41);
      dec16_ff8c(gb);
      if (F & FZ) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
      CYC(b_+41, b_+42);
      B = alu_dec8(gb, B);
      if (B) { CYCT(b_+42, b_+45); continue; }
      CYC(b_+42, b_+45);
      CYC(b_+45, b_+47);
      break;
    }
  }
}

// sprite drawing: objects queued by priority, terrain effects, then wOam is padded out

static void draw_sprite_list(GB *gb) {
  BASE(func_0eda);
  switch_bank(gb, b_+0, GV(0x14, 0x13));
  CYC(b_+7, b_+9); A = H8(hOamTail);
  E = A;
  CYC(b_+9, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  alu_add(gb, A);
  alu_add(gb, A);
  alu_add(gb, E);
  alu_cp(gb, 0xa1);
  if (!(F & FC)) { CYCT(b_+11, b_+19); CYC(b_+45, b_+46); return; }
  CYC(b_+11, b_+19);
  D = wOam >> 8;
  CYC(b_+19, b_+21);
  for (;;) {
    CYC(b_+21, b_+23); A = H8(hFF8C);
    CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+24, b_+25); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(b_+25, b_+29); A = H8(hFF8D);
    CYC(b_+29, b_+30); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+30, b_+31); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(b_+31, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+34, b_+35); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+35, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+37, b_+38); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    C = alu_dec8(gb, C);
    if (C) { CYCT(b_+38, b_+42); continue; }
    CYC(b_+38, b_+42);
    break;
  }
  A = E;
  CYC(b_+42, b_+45); H8(hOamTail) = A;
  CYC(b_+45, b_+46);
}

void func_0eda_hook(GB *gb) {
  draw_sprite_list(gb);
  ret_effect(gb);
}

// drawAllSpritesUnconditionally@drawObject: hl points into wObjectsToDraw; wRamFunction is one
// of the two _getObjectPositionOnScreen variants. The OAM data bank is BASE_OAM_DATA_BANK in the
// disassembly, the bank that holds the item OAM data in both games.
static void draw_object(GB *gb) {
  BASE(drawAllSpritesUnconditionally__drawObject);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  L = alu_inc8(gb, L);
  CYC(b_+1, b_+3); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+3, b_+4);
  CALL_C(b_+4, wRamFunction, wRamFunctionAddr, b_+7);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto ret; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+12); H8(hFF8F) = A;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+15); H8(hFF8E) = A;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); H = mem_rd(gb, HL);
  L = A;
  A = H;
  alu_and(gb, 0xc0);
  alu_rlca(gb);
  alu_rlca(gb);
  alu_add(gb, SYMBANK(itemOamData4cf40));
  CYC(b_+17, b_+25);
  CYC(b_+25, b_+27); H8(hRomBank) = A;
  CYC(b_+27, b_+30); mem_wr(gb, MBC_ROM_BANK, A);
  H = (uint8_t)((H | 0x40) & 0x7f);
  CYC(b_+30, b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_or(gb, A);
  CYC(b_+35, b_+36);
  if (F & FZ) { CYCT(b_+36, b_+38); goto ret; }
  CYC(b_+36, b_+38);
  C = A;
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); A = H8(hOamTail);
  E = A;
  A = wOamEnd & 0xff;
  alu_sub(gb, E);
  CYC(b_+41, b_+45);
  if (F & FZ) { CYCT(b_+45, b_+47); goto ret; }
  CYC(b_+45, b_+47);
  alu_rrca(gb);
  alu_rrca(gb);
  B = A;
  D = wOam >> 8;
  CYC(b_+47, b_+52);
  for (;;) {
    CYC(b_+52, b_+54); A = H8(hFF8C);
    CYC(b_+54, b_+55); alu_add(gb, mem_rd(gb, HL));
    SET_HL(HL + 1);
    alu_cp(gb, 0xa0);
    CYC(b_+55, b_+58);
    if (!(F & FC)) { CYCT(b_+58, b_+60); goto skipSprite; }
    CYC(b_+58, b_+60);
    CYC(b_+60, b_+61); mem_wr(gb, DE, A);
    CYC(b_+61, b_+63); A = H8(hFF8D);
    CYC(b_+63, b_+64); alu_add(gb, mem_rd(gb, HL));
    alu_cp(gb, 0xa8);
    CYC(b_+64, b_+66);
    if (!(F & FC)) { CYCT(b_+66, b_+68); goto skipSprite; }
    CYC(b_+66, b_+68);
    E = alu_inc8(gb, E);
    CYC(b_+68, b_+69);
    CYC(b_+69, b_+70); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(b_+70, b_+72);
    CYC(b_+72, b_+74); A = H8(hFF8E);
    CYC(b_+74, b_+75); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+75, b_+76); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(b_+76, b_+78);
    CYC(b_+78, b_+80); A = H8(hFF8F);
    CYC(b_+80, b_+81); alu_xor(gb, mem_rd(gb, HL));
    CYC(b_+81, b_+82); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    B = alu_dec8(gb, B);
    CYC(b_+82, b_+85);
    if (F & FZ) { CYCT(b_+85, b_+87); break; }
    CYC(b_+85, b_+87);
    C = alu_dec8(gb, C);
    CYC(b_+87, b_+88);
    if (!(F & FZ)) { CYCT(b_+88, b_+90); continue; }
    CYC(b_+88, b_+90);
    break;
  skipSprite:
    SET_HL(HL + 3);
    C = alu_dec8(gb, C);
    CYC(b_+97, b_+101);
    if (!(F & FZ)) { CYCT(b_+101, b_+103); continue; }
    CYC(b_+101, b_+103);
    CYCT(b_+103, b_+105);
    break;
  }
  A = E;
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+93); H8(hOamTail) = A;
ret:
  CYC(b_+93, b_+94); SET_HL(pop_effect(gb));
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x00);
  CYC(b_+96, b_+97);
}

static void draw_object_terrain_effects(GB *gb) {
  BASE(_drawObjectTerrainEffects);
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); return; }
  CYC(b_+3, b_+6);
  A = B;
  alu_cp(gb, 0x97);
  if (!(F & FC)) { CYCT(b_+6, b_+10); return; }
  CYC(b_+6, b_+10);
  alu_bit(gb, 7, E);
  if (F & FZ) {
    CYCT(b_+10, b_+14);
    CYC(b_+47, b_+50); A = W8(wScrollMode);
    alu_cp(gb, 0x08);
    if (F & FZ) { CYCT(b_+50, b_+53); return; }
    CYC(b_+50, b_+53);
    CYC(b_+53, b_+54); push_effect(gb, HL);
    A = L;
    alu_and(gb, 0xc0);
    alu_add(gb, OBJ_YH);
    L = A;
    CYC(b_+54, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
    B = A;
    alu_add(gb, 0x05);
    alu_and(gb, 0xf0);
    C = A;
    L = alu_inc8(gb, L);
    CYC(b_+61, b_+69); L = mem_rd(gb, HL);
    A = L;
    alu_xor(gb, B);
    H = A;
    A = L;
    alu_and(gb, 0xf0);
    A = alu_swap(gb, A);
    alu_or(gb, C);
    C = A;
    B = wRoomLayout >> 8;
    CYC(b_+69, b_+82); A = mem_rd(gb, BC);
    alu_cp(gb, 0xf8);
    if (F & FZ) {
      CYCT(b_+82, b_+86);
      alu_bit(gb, 2, H);
      CYC(b_+99, b_+104); A = W8(wGrassAnimationModifier);
      if (F & FZ) CYCT(b_+104, b_+106);
      else { CYC(b_+104, b_+108); alu_add(gb, 0x24); }
      C = A;
      B = 0x00;
      SET_HL(greenGrassAnimationFrame0_bank14);
      alu_add_hl(gb, BC);
      CYC(b_+108, b_+115);
    } else {
      CYC(b_+82, b_+86);
      alu_cp(gb, 0xf9);
      if (!(F & FZ)) {
        CYCT(b_+86, b_+90);
        CYC(b_+120, b_+121); SET_HL(pop_effect(gb));
        CYC(b_+121, b_+122);
        return;
      }
      CYC(b_+86, b_+90);
      E = alu_inc8(gb, E);
      SET_HL(wPuddleAnimationPointer);
      CYC(b_+90, b_+95); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+95, b_+96); H = mem_rd(gb, HL);
      L = A;
      CYC(b_+96, b_+99);
    }
    CYC(b_+115, b_+116); push_effect(gb, DE);
    CYC(b_+116, b_+119);
    draw_sprite_list(gb);
    CYC(b_+119, b_+120); SET_DE(pop_effect(gb));
    CYC(b_+120, b_+121); SET_HL(pop_effect(gb));
    CYC(b_+121, b_+122);
    return;
  }
  CYC(b_+10, b_+14);
  CYC(b_+14, b_+17); A = W8(wFrameCounter);
  alu_xor(gb, H);
  alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+17, b_+20); return; }
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+21); push_effect(gb, HL);
  CYC(b_+21, b_+23); A = H8(hTerrainEffectsBufferUsedSize);
  alu_add(gb, 0xc0);
  L = A;
  H = wTerrainEffectsBuffer >> 8;
  CYC(b_+23, b_+30); A = H8(hFF8C);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+33); A = H8(hFF8D);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x00;
  CYC(b_+34, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x40;
  CYC(b_+37, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = L;
  alu_sub(gb, 0xc0);
  CYC(b_+40, b_+45); H8(hTerrainEffectsBufferUsedSize) = A;
  CYC(b_+45, b_+46); SET_HL(pop_effect(gb));
  CYC(b_+46, b_+47);
}

void _drawObjectTerrainEffects_hook(GB *gb) {
  draw_object_terrain_effects(gb);
  ret_effect(gb);
}

static void object_position_on_screen_tail(GB *gb) {
  BASE(_getObjectPositionOnScreen);
  CYC(b_+16, b_+18); H8(hFF8D) = A;
  L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); E = mem_rd(gb, HL);
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_VISIBLE);
  L = A;
  CYC(b_+20, b_+27); A = mem_rd(gb, HL);
  alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+27, b_+29); return; }
  CYC(b_+27, b_+29);
  alu_rlca(gb);
  CYC(b_+29, b_+30);
  if (F & FC) { CYCT(b_+30, b_+33); draw_object_terrain_effects(gb); }
  else CYC(b_+30, b_+33);
  A = D;
  alu_add(gb, E);
  CYC(b_+33, b_+37); H8(hFF8C) = A;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, 0x1c);
  L = A;
  alu_scf(gb);
  CYC(b_+37, b_+45);
}

void _getObjectPositionOnScreen_hook(GB *gb) {
  BASE(_getObjectPositionOnScreen);
  CYC(b_+0, b_+2); A = H8(hCameraX);
  C = A;
  CYC(b_+2, b_+5); A = H8(hCameraY);
  B = A;
  CYC(b_+5, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_sub(gb, B);
  alu_add(gb, 0x10);
  CYC(b_+7, b_+12); H8(hFF8C) = A;
  D = A;
  L = alu_inc8(gb, L);
  CYC(b_+12, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_sub(gb, C);
  CYC(b_+15, b_+16);
  object_position_on_screen_tail(gb);
  ret_effect(gb);
}

static void draw_all_sprites_unconditionally(GB *gb) {
  BASE(drawAllSpritesUnconditionally);
  CYC(b_+0, b_+2); A = H8(hRomBank);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+6);
  queue_draw_everything(gb);
  CYC(b_+6, b_+9); A = W8(wLinkRaisedFloorOffset);
  SET_HL(w1Link_yh);
  CYC(b_+9, b_+13); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  SET_DE(w1Link);
  B = 0x0b;
  CYC(b_+14, b_+22); A = W8(wTextboxFlags);
  alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYC(b_+22, b_+26);
    CYC(b_+26, b_+29);
    object_queue_draw(gb);
    CYC(b_+29, b_+31);
  } else {
    CYCT(b_+22, b_+26);
    for (;;) {
      CYC(b_+31, b_+34);
      object_queue_draw(gb);
      D = alu_inc8(gb, D);
      A = D;
      alu_cp(gb, 0xd6);
      if (F & FC) { CYCT(b_+34, b_+40); continue; }
      CYC(b_+34, b_+40);
      break;
    }
  }
  switch_bank(gb, b_+40, 0x14);
  CYC(b_+47, b_+50); A = W8(wFrameCounter);
  alu_add(gb, A);
  A = alu_swap(gb, A);
  alu_and(gb, 0x03);
  SET_HL(puddleAnimationFrames_bank14);
  CYC(b_+50, b_+59);
  add_double_index_to_hl(gb);
  CYC(b_+59, b_+60); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+60, b_+63); W8(wPuddleAnimationPointer) = A;
  CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+64, b_+67); mem_wr(gb, wPuddleAnimationPointer + 1, A);
  SET_HL(wRamFunctionAddr);
  A = 0xc3;
  CYC(b_+67, b_+73); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x82;
  CYC(b_+73, b_+76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x0f);
  CYC(b_+78, b_+81); A = W8(wScrollMode);
  alu_cp(gb, 0x08);
  if (!(F & FZ)) CYCT(b_+81, b_+85);
  else {
    CYC(b_+81, b_+85);
    A = 0x0f;
    CYC(b_+85, b_+88); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(b_+88, b_+90); mem_wr(gb, HL, 0xc1);
    alu_xor(gb, A);
    B = A;
    A = alu_inc8(gb, A);
    CYC(b_+90, b_+95); H8(hFF8A) = A;
    CYC(b_+95, b_+98); A = W8(wRoomIsLarge);
    alu_or(gb, A);
    if (F & FZ) CYCT(b_+98, b_+101);
    else { CYC(b_+98, b_+103); A = 0x04; }
    C = A;
    CYC(b_+103, b_+107); A = W8(wScreenTransitionDirection);
    alu_add(gb, C);
    alu_add(gb, A);
    alu_add(gb, A);
    C = A;
    SET_HL(data_1058);
    alu_add_hl(gb, BC);
    CYC(b_+107, b_+116); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+116, b_+118); H8(hFF90) = A;
    CYC(b_+118, b_+119); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+119, b_+121); H8(hFF91) = A;
    CYC(b_+121, b_+122); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+122, b_+124); H8(hFF92) = A;
    CYC(b_+124, b_+125); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+125, b_+127); H8(hFF93) = A;
  }
  SET_HL(wObjectsToDraw);
  CYC(b_+127, b_+130);
  for (;;) {
    CYC(b_+130, b_+131); A = mem_rd(gb, HL);
    alu_or(gb, A);
    CYC(b_+131, b_+132);
    if (!(F & FZ)) { CYCT(b_+132, b_+135); draw_object(gb); }
    else CYC(b_+132, b_+135);
    L = alu_inc8(gb, L);
    L = alu_inc8(gb, L);
    alu_bit(gb, 7, L);
    if (F & FZ) { CYCT(b_+135, b_+141); continue; }
    CYC(b_+135, b_+141);
    break;
  }
  SET_HL(wTerrainEffectsBuffer);
  CYC(b_+141, b_+146); A = H8(hTerrainEffectsBufferUsedSize);
  alu_rrca(gb);
  A = alu_srl(gb, A);
  B = A;
  if (F & FZ) CYCT(b_+146, b_+152);
  else {
    CYC(b_+146, b_+152);
    for (;;) {
      CYC(b_+152, b_+153); push_effect(gb, BC);
      CYC(b_+153, b_+154); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+154, b_+156); H8(hFF8C) = A;
      CYC(b_+156, b_+157); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+157, b_+159); H8(hFF8D) = A;
      CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+160, b_+161); push_effect(gb, HL);
      CYC(b_+161, b_+162); H = mem_rd(gb, HL);
      L = A;
      CYC(b_+162, b_+166);
      draw_sprite_list(gb);
      CYC(b_+166, b_+167); SET_HL(pop_effect(gb));
      L = alu_inc8(gb, L);
      CYC(b_+167, b_+169); SET_BC(pop_effect(gb));
      B = alu_dec8(gb, B);
      if (B) { CYCT(b_+169, b_+172); continue; }
      CYC(b_+169, b_+172);
      break;
    }
  }
  CYC(b_+172, b_+174); A = H8(hOamTail);
  alu_cp(gb, 0xa0);
  if (!(F & FC)) CYCT(b_+174, b_+178);
  else {
    CYC(b_+174, b_+178);
    H = wOam >> 8;
    B = 0xe0;
    CYC(b_+178, b_+182);
    for (;;) {
      L = A;
      CYC(b_+182, b_+184); mem_wr(gb, HL, B);
      alu_add(gb, 0x04);
      alu_cp(gb, 0xa0);
      if (F & FC) { CYCT(b_+184, b_+190); continue; }
      CYC(b_+184, b_+190);
      break;
    }
  }
  CYC(b_+190, b_+193); A = W8(wLinkRaisedFloorOffset);
  alu_cpl(gb);
  A = alu_inc8(gb, A);
  SET_HL(w1Link_yh);
  CYC(b_+193, b_+199); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+199, b_+200); mem_wr(gb, HL, A);
  CYC(b_+200, b_+201); SET_AF(pop_effect(gb));
  CYC(b_+201, b_+203); H8(hRomBank) = A;
  CYC(b_+203, b_+206); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+206, b_+207);
}

void drawAllSpritesUnconditionally_hook(GB *gb) {
  draw_all_sprites_unconditionally(gb);
  ret_effect(gb);
}

void drawAllSprites_hook(GB *gb) {
  BASE(drawAllSprites);
  SET_HL(wc4b6);
  CYC(b_+0, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xff);
  draw_all_sprites_unconditionally(gb);
  ret_effect(gb);
}

// dirty palette upload from the VBlank handler: bit n of the dirty mask selects palette n

static void write_palette_regs(GB *gb) {
  BASE(updateDirtyPalettes);
  for (;;) {
    D = alu_srl(gb, D);
    if (!(F & FC)) {
      CYCT(b_+28, b_+32);
      if (F & FZ) { CYCT(b_+56, b_+57); return; }
      CYC(b_+56, b_+57);
      A = L;
      alu_add(gb, 0x08);
      L = A;
      CYC(b_+57, b_+63);
      continue;
    }
    CYC(b_+28, b_+32);
    A = L;
    alu_or(gb, 0x80);
    CYC(b_+32, b_+36); mem_wr(gb, 0xff00 | C, A);
    C = alu_inc8(gb, C);
    CYC(b_+36, b_+37);
    for (uint16_t a = b_+37; a < b_+53; a += 2) {
      CYC(a, a + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(a + 1, a + 2); mem_wr(gb, 0xff00 | C, A);
    }
    C = alu_dec8(gb, C);
    CYC(b_+53, b_+56);
  }
}

void updateDirtyPalettes_hook(GB *gb) {
  BASE(updateDirtyPalettes);
  A = 0x02;
  CYC(b_+0, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+6); A = H8(hDirtyBgPalettes);
  D = A;
  alu_xor(gb, A);
  CYC(b_+6, b_+10); H8(hDirtyBgPalettes) = A;
  C = 0x68;
  SET_HL(w2BgPalettesBuffer);
  CYC(b_+10, b_+18); push_effect(gb, b_+18);
  write_palette_regs(gb);
  pop_effect(gb);
  CYC(b_+18, b_+20); A = H8(hDirtySprPalettes);
  D = A;
  alu_xor(gb, A);
  CYC(b_+20, b_+24); H8(hDirtySprPalettes) = A;
  C = 0x6a;
  L = 0x40;
  CYC(b_+24, b_+28);
  write_palette_regs(gb);
  ret_effect(gb);
}

// the script engine's asm-call commands and the loader for scripts copied to wBigBuffer

void _interactionActuallyRunScript_hook(GB *gb) {
  BASE(_interactionActuallyRunScript);
  bank_push(gb, b_+0, GV(0x0c, 0x0b));
  for (;;) {
    CYC(b_+10, b_+11); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+11, b_+14);
      bank_pop(gb, b_+27);
      alu_scf(gb);
      CYC(b_+33, b_+35);
      ret_effect(gb);
      return;
    }
    CYC(b_+11, b_+14);
    CALL_ROM(b_+14, ROM_b0c_runScriptCommand);
    if (F & FC) { CYCT(b_+17, b_+19); continue; }
    CYC(b_+17, b_+19);
    break;
  }
  bank_pop(gb, b_+19);
  alu_xor(gb, A);
  CYC(b_+25, b_+27);
  ret_effect(gb);
}

void _interactionSaveScriptAddress_hook(GB *gb) {
  BASE(_interactionSaveScriptAddress);
  E = 0x58;
  A = L;
  CYC(b_+0, b_+4); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(b_+4, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8);
  ret_effect(gb);
}

void _scriptCmd_asmRetFunc_hook(GB *gb) {
  BASE(_scriptCmd_asmRetFunc);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_AF(pop_effect(gb));
  CYC(b_+2, b_+4); H8(hRomBank) = A;
  CYC(b_+4, b_+7); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+7, b_+9); A = H8(hActiveObject);
  D = A;
  alu_scf(gb);
  CYC(b_+9, b_+12);
  ret_effect(gb);
}

static void script_setup_asm_call(GB *gb) {
  BASE(_scriptFunc_setupAsmCall);
  SET_HL(HL + 1);
  D = 0x15;
  CYC(b_+0, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+4, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(b_+6, b_+8);
}

void _scriptFunc_setupAsmCall_hook(GB *gb) {
  script_setup_asm_call(gb);
  ret_effect(gb);
}

static void script_asm_call_tail(GB *gb) {
  BASE(scriptCmd_asmCallWithParam);
  CYC(b_+6, b_+8); A = H8(hRomBank);
  CYC(b_+8, b_+9); push_effect(gb, AF);
  A = D;
  CYC(b_+9, b_+12); H8(hRomBank) = A;
  CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+15, b_+16); push_effect(gb, HL);
  SET_HL(ROM__scriptCmd_asmRetFunc);
  CYC(b_+16, b_+20); push_effect(gb, HL);
  CYC(b_+20, b_+22); A = H8(hActiveObject);
  D = A;
  H = B;
  L = C;
  A = E;
  CYC(b_+22, b_+27);
  hook_handoff(gb, HL);
}

void scriptCmd_asmCall_hook(GB *gb) {
  BASE(scriptCmd_asmCall);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4);
  script_setup_asm_call(gb);
  CYC(b_+4, b_+6);
  script_asm_call_tail(gb);
}

void scriptCmd_asmCallWithParam_hook(GB *gb) {
  BASE(scriptCmd_asmCallWithParam);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4);
  script_setup_asm_call(gb);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(b_+5, b_+6);
  script_asm_call_tail(gb);
}

void scriptCmd_loadScript_hook(GB *gb) {
  BASE(scriptCmd_loadScript);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  SET_HL(HL + 1);
  CYC(b_+1, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(b_+3, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+5, b_+8); H8(hScriptAddressL) = A;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(b_+9, b_+12); H8(hScriptAddressH) = A;
  CYC(b_+12, b_+14); A = H8(hRomBank);
  CYC(b_+14, b_+15); push_effect(gb, AF);
  A = E;
  CYC(b_+15, b_+18); H8(hRomBank) = A;
  CYC(b_+18, b_+21); mem_wr(gb, MBC_ROM_BANK, A);
  H = B;
  L = C;
  SET_DE(wBigBuffer);
  B = 0x00;
  CYC(b_+21, b_+31);
  copyMemory(gb, DE, HL, 256);
  SET_HL(HL + 256);
  SET_DE(DE + 256);
  CYC(b_+31, b_+32); SET_AF(pop_effect(gb));
  CYC(b_+32, b_+34); H8(hRomBank) = A;
  CYC(b_+34, b_+37); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+37, b_+39); A = H8(hActiveObject);
  D = A;
  SET_HL(wBigBuffer);
  alu_scf(gb);
  CYC(b_+39, b_+45);
  ret_effect(gb);
}

// room layout: the rst $00 jump table picks the large or small room decoder

static uint16_t rst_jump_table(GB *gb) {
  CYC(0x0000, 0x0001); alu_add(gb, A);
  CYC(0x0001, 0x0002); SET_HL(pop_effect(gb));
  CYC(0x0002, 0x0003); alu_add(gb, L);
  CYC(0x0003, 0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004, 0x0006);
  else {
    CYC(0x0004, 0x0006);
    CYC(0x0006, 0x0007); H = alu_inc8(gb, H);
  }
  CYC(0x0007, 0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0008, 0x0009); H = mem_rd(gb, HL);
  CYC(0x0009, 0x000a); L = A;
  CYC(0x000a, 0x000b);
  return HL;
}

void linkState07__substate0_hook(GB *gb) {
  BASE(linkState07);
  uint16_t sp0_ = gb->sp;
  CALL_ROM(b_+10, SYM(linkCancelAllItemUsageAndClearAdjacentWallsBitset));
  CALL_C(b_+13, itemIncSubstate_hook, SYM(itemIncSubstate), b_+16);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+19); L = 0x24;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CALL_C(b_+20, clearVariousLinkVariables_hook, SYM(clearVariousLinkVariables), b_+23);
  CYC(b_+23, b_+25); A = 0x80;
  CYC(b_+25, b_+28); W8(wLinkInAir) = A;
  CYC(b_+28, b_+30); A = 0x03;
  CYC(b_+30, b_+33);
  specialObjectSetAnimation_hook(gb);
}

void linkState07__substate2_hook(GB *gb) {
  BASE(linkState07);
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37); W8(wLinkInAir) = A;
  CYC(b_+37, b_+39); A = 0x05;
  CYC(b_+39, b_+42); W8(wWarpTransition) = A;
  CYC(b_+42, b_+44); E = 0x0b;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); alu_add(gb, 0x04);
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); A = 0x0a;
  CYC(b_+50, b_+53);
  linkSetState_hook(gb);
}

void linkState07_hook(GB *gb) {
  BASE(linkState07);
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (rst_jump_table(gb));
    if (jt_ == b_+10) { linkState07__substate0_hook(gb); return; }
    else if (jt_ == SYM(specialObjectAnimate)) { specialObjectAnimate_hook(gb); return; }
    else if (jt_ == b_+33) { linkState07__substate2_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

// loadRoomLayout's local blocks. The Ages instruction stream; loadRoomLayout is not hooked under
// Seasons, whose @loadLayoutData differs.

// @loadLargeRoomLayoutHlpr: bc is a chunk descriptor; returns hl = its data and b = its length.
static void room_layout_large_hlpr(GB *gb) {
  BASE(loadRoomLayout__loadLargeRoomLayoutHlpr);
  D = B;
  A = B;
  alu_and(gb, 0x0f);
  B = A;
  CYC(b_+0, b_+5);
  CYC(b_+5, b_+7); A = H8(hFF8F);
  H = A;
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = H8(hFF8E);
  L = A;
  alu_add_hl(gb, BC);
  A = D;
  alu_swap_a(gb);
  alu_and(gb, 0x0f);
  alu_add(gb, 0x03);
  B = A;
  CYC(b_+10, b_+21);
}

// @checkDeNextLayoutRow: keeps de inside the small room's 10-wide rows.
static void room_layout_check_de_next_row(GB *gb) {
  BASE(loadRoomLayout__checkDeNextLayoutRow);
  A = E;
  alu_and(gb, 0x0f);
  alu_cp(gb, 0x0a);
  CYC(b_+0, b_+5);
  if (F & FC) { CYCT(b_+5, b_+6); return; }
  CYC(b_+5, b_+6);
  A = 0x06;
  alu_add(gb, E);
  E = A;
  CYC(b_+6, b_+11);
}

// @layoutCopyBytes: copies b bytes from hl to de with row wrapping.
static void room_layout_copy_bytes(GB *gb) {
  BASE(loadRoomLayout__layoutCopyBytes);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+1, b_+2); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+2, b_+6);
    room_layout_check_de_next_row(gb);
    B = alu_dec8(gb, B);
    CYC(b_+6, b_+7);
    if (B) { CYCT(b_+7, b_+9); continue; }
    CYC(b_+7, b_+9);
    break;
  }
  CYC(b_+9, b_+10);
}

// @decompressLayoutHelper: eight bytes, each either the next byte at hl or the repeat byte in
// hFF8B, chosen by the bits of c.
static void room_layout_decompress_helper(GB *gb) {
  BASE(loadRoomLayout__decompressLayoutHelper);
  B = 0x08;
  CYC(b_+0, b_+2);
  for (;;) {
    C = alu_srl(gb, C);
    CYC(b_+2, b_+4);
    if (F & FC) {
      CYCT(b_+4, b_+6);
      CYC(b_+9, b_+11); A = H8(hFF8B);
    } else {
      CYC(b_+4, b_+6);
      CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYCT(b_+7, b_+9);
    }
    CYC(b_+11, b_+12); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+12, b_+16);
    room_layout_check_de_next_row(gb);
    B = alu_dec8(gb, B);
    CYC(b_+16, b_+17);
    if (B) { CYCT(b_+17, b_+19); continue; }
    CYC(b_+17, b_+19);
    break;
  }
  CYC(b_+19, b_+20);
}

// @decompressLayoutMode2Helper: a 16-bit repeat mask, the repeat byte, then literal bytes.
static void room_layout_mode2_helper(GB *gb) {
  BASE(loadRoomLayout__decompressLayoutMode2Helper);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+5); H8(hFF8A) = A;
  alu_or(gb, C);
  B = 0x10;
  CYC(b_+5, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); room_layout_copy_bytes(gb); return; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+13); H8(hFF8B) = A;
  CYC(b_+13, b_+16);
  room_layout_decompress_helper(gb);
  CYC(b_+16, b_+18); A = H8(hFF8A);
  C = A;
  CYC(b_+18, b_+19);
  CYCT(b_+19, b_+21);
  room_layout_decompress_helper(gb);
}

// @decompressLayoutMode1Helper: an 8-bit repeat mask, the repeat byte, then literal bytes.
static void room_layout_mode1_helper(GB *gb) {
  BASE(loadRoomLayout__decompressLayoutMode1Helper);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  alu_or(gb, A);
  B = 0x08;
  CYC(b_+1, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); room_layout_copy_bytes(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+10); H8(hFF8B) = A;
  CYCT(b_+10, b_+12);
  room_layout_decompress_helper(gb);
}

// @loadLayoutData: reads the $b0 layout bytes at bank hFF8C:hl into wRoomCollisions, walking
// into the next bank when hl crosses $8000; returns hl = wRoomCollisions.
static void room_layout_load_data(GB *gb) {
  BASE(loadRoomLayout__loadLayoutData);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); A = H8(hFF8C);
  E = A;
  CYC(b_+3, b_+4);
  for (;;) {
    alu_bit(gb, 7, H);
    CYC(b_+4, b_+6);
    if (F & FZ) { CYCT(b_+6, b_+8); break; }
    CYC(b_+6, b_+8);
    A = H;
    alu_sub(gb, 0x40);
    H = A;
    E = alu_inc8(gb, E);
    CYC(b_+8, b_+13);
    CYCT(b_+13, b_+15);
  }
  A = E;
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); H8(hRomBank) = A;
  CYC(b_+18, b_+21); mem_wr(gb, MBC_ROM_BANK, A);
  B = 0xb0;
  SET_DE(wRoomCollisions);
  CYC(b_+21, b_+26);
  for (;;) {
    CALL_C(b_+26, readByteSequential_hook, SYM(readByteSequential), b_+29);
    CYC(b_+29, b_+30); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    B = alu_dec8(gb, B);
    CYC(b_+30, b_+32);
    if (B) { CYCT(b_+32, b_+34); continue; }
    CYC(b_+32, b_+34);
    break;
  }
  SET_HL(wRoomCollisions);
  CYC(b_+34, b_+37);
  CYC(b_+37, b_+38); SET_DE(pop_effect(gb));
  CYC(b_+38, b_+39);
}

static void load_large_room_layout(GB *gb) {
  BASE(loadRoomLayout);
  CYC(b_+76, b_+78); A = H8(hFF8F);
  H = A;
  CYC(b_+78, b_+81); A = H8(hFF8E);
  L = A;
  SET_BC(0x1000);
  alu_add_hl(gb, BC);
  CYC(b_+81, b_+88); A = H8(hFF8D);
  CYC(b_+88, b_+90); H8(hRomBank) = A;
  CYC(b_+90, b_+93); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+93, b_+96); A = W8(wLoadingRoom);
  CYC(b_+96, b_+97);
  add_double_index_to_hl(gb);
  CYC(b_+97, b_+98); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+98, b_+99); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+99, b_+100); SET_BC(pop_effect(gb));
  alu_add_hl(gb, BC);
  SET_BC(0xfe00);
  alu_add_hl(gb, BC);
  CYC(b_+100, b_+106);
  CYC(b_+106, b_+109);
  room_layout_load_data(gb);
  SET_DE(wRoomLayout);
  CYC(b_+109, b_+112);
  for (;;) {
    CYC(b_+112, b_+113); A = mem_rd(gb, HL); SET_HL(HL + 1);
    B = 0x08;
    CYC(b_+113, b_+115);
    for (;;) {
      alu_rrca(gb);
      CYC(b_+115, b_+118); H8(hFF8B) = A;
      if (F & FC) {
        CYCT(b_+118, b_+120);
        CYC(b_+134, b_+135); push_effect(gb, BC);
        CYC(b_+135, b_+136); A = mem_rd(gb, HL); SET_HL(HL + 1);
        C = A;
        CYC(b_+136, b_+138); A = mem_rd(gb, HL); SET_HL(HL + 1);
        B = A;
        CYC(b_+138, b_+140); push_effect(gb, HL);
        CYC(b_+140, b_+143);
        room_layout_large_hlpr(gb);
        D = wRoomLayout >> 8;
        CYC(b_+143, b_+147); A = H8(hFF8D);
        CYC(b_+147, b_+149); H8(hRomBank) = A;
        CYC(b_+149, b_+152); mem_wr(gb, MBC_ROM_BANK, A);
        for (;;) {
          CYC(b_+152, b_+153); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(b_+153, b_+154); mem_wr(gb, DE, A);
          E = alu_inc8(gb, E);
          A = E;
          alu_cp(gb, 0xb0);
          if (F & FZ) {
            CYCT(b_+154, b_+160);
            CYC(b_+167, b_+168); SET_HL(pop_effect(gb));
            CYC(b_+168, b_+169); SET_BC(pop_effect(gb));
            CYC(b_+169, b_+170);
            return;
          }
          CYC(b_+154, b_+160);
          B = alu_dec8(gb, B);
          if (B) { CYCT(b_+160, b_+163); continue; }
          CYC(b_+160, b_+163);
          break;
        }
        CYC(b_+163, b_+164); SET_HL(pop_effect(gb));
        CYC(b_+164, b_+165); SET_BC(pop_effect(gb));
        CYC(b_+165, b_+167);
      } else {
        CYC(b_+118, b_+120);
        CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+121, b_+122); mem_wr(gb, DE, A);
        E = alu_inc8(gb, E);
        A = E;
        alu_cp(gb, 0xb0);
        if (F & FZ) { CYCT(b_+122, b_+127); return; }
        CYC(b_+122, b_+127);
      }
      CYC(b_+127, b_+129); A = H8(hFF8B);
      B = alu_dec8(gb, B);
      if (B) { CYCT(b_+129, b_+132); continue; }
      CYC(b_+129, b_+132);
      CYC(b_+132, b_+134);
      break;
    }
  }
}

static void decompress_layout_rows(GB *gb, uint16_t a, uint8_t rows, void (*helper)(GB *)) {
  SET_DE(wRoomLayout);
  A = rows;
  CYC(a, a + 5);
  for (;;) {
    CYC(a + 5, a + 6); push_effect(gb, AF);
    CYC(a + 6, a + 9);
    helper(gb);
    CYC(a + 9, a + 10); SET_AF(pop_effect(gb));
    A = alu_dec8(gb, A);
    if (A) { CYCT(a + 10, a + 13); continue; }
    CYC(a + 10, a + 14);
    return;
  }
}

static void load_small_room_layout(GB *gb) {
  BASE(loadRoomLayout);
  CYC(b_+170, b_+172); A = H8(hFF8D);
  CYC(b_+172, b_+174); H8(hRomBank) = A;
  CYC(b_+174, b_+177); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+177, b_+179); A = H8(hFF8E);
  L = A;
  CYC(b_+179, b_+182); A = H8(hFF8F);
  H = A;
  CYC(b_+182, b_+186); A = W8(wLoadingRoom);
  CYC(b_+186, b_+187);
  add_double_index_to_hl(gb);
  CYC(b_+187, b_+188); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+188, b_+190); A = mem_rd(gb, HL);
  E = A;
  alu_and(gb, 0x3f);
  B = A;
  CYC(b_+190, b_+195); SET_HL(pop_effect(gb));
  alu_add_hl(gb, BC);
  CYC(b_+195, b_+196);
  CYC(b_+196, b_+199);
  room_layout_load_data(gb);
  alu_bit(gb, 7, E);
  if (!(F & FZ)) { CYCT(b_+199, b_+203); decompress_layout_rows(gb, b_+229, 0x05, room_layout_mode2_helper); return; }
  CYC(b_+199, b_+203);
  alu_bit(gb, 6, E);
  if (!(F & FZ)) { CYCT(b_+203, b_+207); decompress_layout_rows(gb, b_+264, 0x0a, room_layout_mode1_helper); return; }
  CYC(b_+203, b_+207);
  SET_DE(wRoomLayout);
  SET_BC(0x0a08);
  CYC(b_+207, b_+213);
  for (;;) {
    CYC(b_+213, b_+214); push_effect(gb, BC);
    for (;;) {
      CYC(b_+214, b_+215); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+215, b_+216); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      B = alu_dec8(gb, B);
      if (B) { CYCT(b_+216, b_+220); continue; }
      CYC(b_+216, b_+220);
      break;
    }
    A = E;
    alu_add(gb, 0x06);
    E = A;
    CYC(b_+220, b_+225); SET_BC(pop_effect(gb));
    C = alu_dec8(gb, C);
    if (C) { CYCT(b_+225, b_+228); continue; }
    CYC(b_+225, b_+229);
    return;
  }
}

void loadRoomLayout_hook(GB *gb) {
  BASE(loadRoomLayout);
  SET_HL(wRoomLayout);
  B = 0xc0;
  CYC(b_+0, b_+5);
  CALL_ROM(b_+5, ROM_clearMemory);
  switch_bank(gb, b_+8, 0x04);
  CYC(b_+15, b_+18); A = W8(wTilesetLayoutGroup);
  alu_add(gb, A);
  alu_add(gb, A);
  SET_HL(roomLayoutGroupTable_bank04);
  CYC(b_+18, b_+24);
  add_double_index_to_hl(gb);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(b_+25, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+29); H8(hFF8D) = A;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+32); H8(hFF8E) = A;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+33, b_+35); H8(hFF8F) = A;
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+38); H8(hFF8C) = A;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+39, b_+40); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+40, b_+43); A = H8(hFF8C);
  CYC(b_+43, b_+45); H8(hRomBank) = A;
  CYC(b_+45, b_+48); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+48, b_+49); push_effect(gb, HL);
  A = B;
  CYC(b_+49, b_+51); push_effect(gb, b_+51);
  uint16_t target = rst_jump_table(gb);
  if (target == b_+76) load_large_room_layout(gb);
  else if (target == b_+170) load_small_room_layout(gb);
  else { hook_handoff(gb, target); return; }
  ret_effect(gb);
}

// object position on screen while a screen transition scrolls: camera in hFF90-hFF93

void _label_00_152_hook(GB *gb);

void _getObjectPositionOnScreen_duringScreenTransition_hook(GB *gb) {
  BASE(_getObjectPositionOnScreen_duringScreenTransition);
  D = H;
  A = L;
  alu_and(gb, 0xc0);
  E = A;
  CYC(b_+0, b_+6); A = mem_rd(gb, DE);
  alu_and(gb, 0x03);
  alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+6, b_+12);
    _label_00_152_hook(gb);
    return;
  }
  CYC(b_+6, b_+12);
  D = 0x00;
  CYC(b_+12, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, 0x10);
  C = A;
  A = D;
  alu_adc(gb, A);
  B = A;
  L = alu_inc8(gb, L);
  CYC(b_+15, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(b_+23, b_+25); push_effect(gb, HL);
  A = L;
  alu_and(gb, 0xc0);
  L = A;
  CYC(b_+25, b_+31); A = H8(hFF8A);
  CYC(b_+31, b_+32); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+32, b_+34);
  else {
    CYC(b_+32, b_+34);
    SET_HL(hFF90);
    CYC(b_+34, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, C);
    C = A;
    CYC(b_+38, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_adc(gb, B);
    B = A;
    CYC(b_+41, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, E);
    E = A;
    CYC(b_+44, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_adc(gb, D);
    D = A;
    CYC(b_+47, b_+49);
  }
  SET_HL(hCameraY);
  A = C;
  CYC(b_+49, b_+54); alu_sub(gb, mem_rd(gb, HL));
  C = A;
  L = alu_inc8(gb, L);
  A = B;
  CYC(b_+54, b_+58); alu_sbc(gb, mem_rd(gb, HL));
  B = A;
  bool draw = true;
  if (F & FZ) {
    CYCT(b_+58, b_+61);
    A = C;
    alu_cp(gb, 0xb0);
    if (!(F & FC)) { CYCT(b_+71, b_+76); draw = false; }
    else CYC(b_+71, b_+76);
  } else {
    CYC(b_+58, b_+61);
    A = alu_inc8(gb, A);
    if (!(F & FZ)) { CYCT(b_+61, b_+64); draw = false; }
    else {
      CYC(b_+61, b_+64);
      A = C;
      alu_cp(gb, 0xe0);
      if (F & FC) { CYCT(b_+64, b_+69); draw = false; }
      else { CYC(b_+64, b_+69); CYC(b_+69, b_+71); }
    }
  }
  if (draw) {
    CYC(b_+76, b_+78); H8(hFF8C) = A;
    B = A;
    L = alu_inc8(gb, L);
    A = E;
    CYC(b_+78, b_+82); alu_sub(gb, mem_rd(gb, HL));
    E = A;
    L = alu_inc8(gb, L);
    A = D;
    CYC(b_+82, b_+86); alu_sbc(gb, mem_rd(gb, HL));
    D = A;
    if (F & FZ) {
      CYCT(b_+86, b_+89);
      A = E;
      alu_cp(gb, 0xb8);
      if (!(F & FC)) { CYCT(b_+99, b_+104); draw = false; }
      else CYC(b_+99, b_+104);
    } else {
      CYC(b_+86, b_+89);
      A = alu_inc8(gb, A);
      if (!(F & FZ)) { CYCT(b_+89, b_+92); draw = false; }
      else {
        CYC(b_+89, b_+92);
        A = E;
        alu_cp(gb, 0xe8);
        if (F & FC) { CYCT(b_+92, b_+97); draw = false; }
        else { CYC(b_+92, b_+97); CYC(b_+97, b_+99); }
      }
    }
  }
  if (draw) {
    CYC(b_+104, b_+106); H8(hFF8D) = A;
    D = B;
    CYC(b_+106, b_+108); SET_HL(pop_effect(gb));
    L = alu_inc8(gb, L);
    CYC(b_+108, b_+110); E = mem_rd(gb, HL);
    A = L;
    alu_and(gb, 0xc0);
    alu_add(gb, OBJ_VISIBLE);
    L = A;
    CYC(b_+110, b_+117); A = mem_rd(gb, HL);
    alu_rlca(gb);
    if (!(F & FC)) { CYCT(b_+117, b_+119); ret_effect(gb); return; }
    CYC(b_+117, b_+119);
    alu_rlca(gb);
    CYC(b_+119, b_+120);
    if (F & FC) { CYCT(b_+120, b_+123); draw_object_terrain_effects(gb); }
    else CYC(b_+120, b_+123);
    A = D;
    alu_add(gb, E);
    CYC(b_+123, b_+127); H8(hFF8C) = A;
    A = L;
    alu_and(gb, 0xc0);
    alu_add(gb, 0x1c);
    L = A;
    alu_scf(gb);
    CYC(b_+127, b_+135);
    ret_effect(gb);
    return;
  }
  CYC(b_+135, b_+136); SET_HL(pop_effect(gb));
  A = L;
  alu_and(gb, 0xc0);
  L = A;
  CYC(b_+136, b_+142); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+142, b_+144);
  else {
    CYC(b_+142, b_+144);
    alu_or(gb, OBJ_VISIBLE);
    L = A;
    CYC(b_+144, b_+149); mem_wr(gb, HL, 0x00);
  }
  alu_xor(gb, A);
  CYC(b_+149, b_+151);
  ret_effect(gb);
}

void _label_00_152_hook(GB *gb) {
  BASE(_label_00_152);
  CYC(b_+0, b_+2); A = H8(hCameraX);
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+5); A = H8(hCameraY);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); alu_sub(gb, B);
  CYC(b_+8, b_+10); alu_add(gb, 0x10);
  CYC(b_+10, b_+12); H8(hFF8C) = A;
  CYC(b_+12, b_+13); D = A;
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); alu_sub(gb, C);
  CYC(b_+16, b_+18);
  object_position_on_screen_tail(gb);
  ret_effect(gb);
}

// angle towards an object read from (de)

void getRelativeAngle_hook(GB *gb) {
  BASE(getRelativeAngle);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); H8(hFF8F) = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(b_+3, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); H8(hFF8E) = A;
  object_get_relative_angle_with_temp_vars(gb);
  ret_effect(gb);
}

// sidescrolling gravity with a landing check against the tile below

void objectUpdateSpeedZ_sidescroll_givenYOffset_hook(GB *gb) {
  BASE(objectUpdateSpeedZ_sidescroll_givenYOffset);
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  CYC(b_+2, b_+4); A = H8(hActiveObjectType);
  alu_add(gb, 0x15);
  L = A;
  H = D;
  CYC(b_+4, b_+10); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYC(b_+10, b_+12);
    alu_add(gb, 0xf6);
    L = A;
    CYC(b_+12, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, B);
    B = A;
    L = alu_inc8(gb, L);
    CYC(b_+16, b_+20); A = mem_rd(gb, HL);
    alu_sub(gb, 0x04);
    C = A;
    CYC(b_+20, b_+23);
    CALL_ROM(b_+23, ROM_checkTileCollisionAt_allowHoles);
    if (F & FC) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
    CYC(b_+26, b_+27);
    A = C;
    alu_add(gb, 0x07);
    C = A;
    CYC(b_+27, b_+31);
    CALL_ROM(b_+31, ROM_checkTileCollisionAt_allowHoles);
    if (F & FC) { CYCT(b_+34, b_+35); ret_effect(gb); return; }
    CYC(b_+34, b_+35);
  } else CYCT(b_+10, b_+12);
  CYC(b_+35, b_+37); A = H8(hActiveObjectType);
  alu_add(gb, 0x0a);
  E = A;
  alu_add(gb, 0x0a);
  L = A;
  H = D;
  CYC(b_+37, b_+44);
  CALL_ROM(b_+44, ROM_add16BitRefs);
  L = alu_dec8(gb, L);
  CYC(b_+47, b_+50); A = H8(hFF8B);
  CYC(b_+50, b_+51); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x00;
  CYC(b_+52, b_+55); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  alu_or(gb, D);
  CYC(b_+56, b_+58);
  ret_effect(gb);
}

// bank-switched wrappers: treasure display, object gfx, damage, files, cutscene objects

void interactWithTileBeforeLink_b00_hook(GB *gb) {
  BASE(interactWithTileBeforeLink_b00);
  uint16_t sp0_ = gb->sp;
  bank_push(gb, b_+0, 0x06);
  CALL_C(b_+10, interactWithTileBeforeLink_b06_hook, SYM(interactWithTileBeforeLink_b06), b_+13);
  C = alu_rl(gb, C);
  CYC(b_+13, b_+15);
  bank_pop(gb, b_+15);
  C = alu_srl(gb, C);
  CYC(b_+21, b_+24);
  ret_effect(gb);
}

void reloadObjectGfx_b00_hook(GB *gb) {
  BASE(reloadObjectGfx_b00);
  uint16_t sp0_ = gb->sp;
  bank3f_push(gb, b_+0);
  CALL_C(b_+10, reloadObjectGfx_b3f_hook, ROM_b3f_reloadObjectGfx, b_+13);
  bank_pop_af(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void loadWeaponGfx_b00_hook(GB *gb) {
  BASE(loadWeaponGfx_b00);
  uint16_t sp0_ = gb->sp;
  E = A;
  CYC(b_+0, b_+1);
  bank3f_push(gb, b_+1);
  CALL_C(b_+11, loadWeaponGfx_b3f_hook, ROM_b3f_loadWeaponGfx, b_+14);
  bank_pop_af(gb, b_+14);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

void loadTreasureDisplayData_b00_hook(GB *gb) {
  BASE(loadTreasureDisplayData_b00);
  uint16_t sp0_ = gb->sp;
  L = A;
  CYC(b_+0, b_+1);
  bank3f_push(gb, b_+1);
  CALL_C(b_+11, loadTreasureDisplayData_b3f_hook, ROM_b3f_loadTreasureDisplayData, b_+14);
  bank_pop_af(gb, b_+14);
  CYC(b_+20, b_+21);
  ret_effect(gb);
}

void checkTreasureObtained_hook(GB *gb) {
  BASE(checkTreasureObtained);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2);
  L = A;
  alu_or(gb, A);
  if (F & FZ) CYCT(b_+2, b_+5);
  else {
    CYC(b_+2, b_+5);
    bank_push(gb, b_+5, 0x3f);
    CALL_C(b_+15, checkTreasureObtained_body_hook,
        ROM_b3f_checkTreasureObtained_body, b_+18);
    bank_pop(gb, b_+18);
    A = L;
    H = alu_srl(gb, H);
    CYC(b_+24, b_+27);
  }
  CYC(b_+27, b_+28); SET_HL(pop_effect(gb));
  CYC(b_+28, b_+29);
  ret_effect(gb);
}

void makeActiveObjectFollowLink_b00_hook(GB *gb) { banked_body_call(gb, SYM(makeActiveObjectFollowLink_b00), 0x01, ROM_b01_makeActiveObjectFollowLink); }
void specialObjectCode_companionCutscene_b00_hook(GB *gb) {
  BASE(specialObjectCode_companionCutscene_b00);
  uint16_t sp0_ = gb->sp;
  bank_push(gb, b_+0, 0x06);
  CALL_C(b_+10, specialObjectCode_companionCutscene_b06_hook,
         ROM_b06_specialObjectCode_companionCutscene, b_+13);
  bank_pop(gb, b_+13);
  CYC(b_+19, b_+20);
  ret_effect(gb);
}

void specialObjectCode_linkInCutscene_b00_hook(GB *gb) {
  BASE(specialObjectCode_linkInCutscene_b00);
  SET_HL(ROM_b06_specialObjectCode_linkInCutscene);
  E = 0x06;
  CYC(b_+0, b_+8);
  interBankCall_hook(gb);
}

void linkApplyDamage_b00_hook(GB *gb) {
  BASE(linkApplyDamage_b00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); A = H8(hRomBank);
  CYC(b_+3, b_+4); push_effect(gb, AF);
  D = w1Link >> 8;
  A = 0x05;
  CYC(b_+4, b_+10); H8(hRomBank) = A;
  CYC(b_+10, b_+13); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+13, linkApplyDamage_b5_hook, SYM(linkApplyDamage_b5), b_+16);
  bank_pop(gb, b_+16);
  CYC(b_+22, b_+23); SET_DE(pop_effect(gb));
  CYC(b_+23, b_+24);
  ret_effect(gb);
}

void checkUseItems_b00_hook(GB *gb) {
  BASE(checkUseItems_b00);
  uint16_t sp0_ = gb->sp;
  C = 0x02;
  CYC(b_+0, b_+2);
  bank6_function_caller(gb, sp0_);
  ret_effect(gb);
}

void parseGivenObjectData_b00_hook(GB *gb) {
  BASE(parseGivenObjectData_b00);
  uint16_t sp0_ = gb->sp;
  bank_push(gb, b_+0, GV(0x12, 0x11));
  CYC(b_+10, b_+11); push_effect(gb, DE);
  D = H;
  E = L;
  CYC(b_+11, b_+13);
  CALL_C(b_+13, parseGivenObjectData_b12_hook, ROM_b12_parseGivenObjectData, b_+16);
  CYC(b_+16, b_+17); SET_DE(pop_effect(gb));
  bank_pop(gb, b_+17);
  CYC(b_+23, b_+24);
  ret_effect(gb);
}

void checkObjectIsCloseToPosition_b00_hook(GB *gb) {
  BASE(checkObjectIsCloseToPosition_b00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); H8(hFF8B) = A;
  bank_push(gb, b_+2, 0x08);
  CALL_C(b_+12, checkObjectIsCloseToPosition_b08_hook, ROM_b08_checkObjectIsCloseToPosition, b_+15);
  B = 0x00;
  if (!(F & FC)) CYCT(b_+15, b_+19);
  else { CYC(b_+15, b_+20); B = alu_inc8(gb, B); }
  bank_pop(gb, b_+20);
  A = B;
  alu_or(gb, A);
  if (F & FZ) { CYCT(b_+26, b_+29); ret_effect(gb); return; }
  CYC(b_+26, b_+29);
  alu_scf(gb);
  CYC(b_+29, b_+31);
  ret_effect(gb);
}

static void file_management(GB *gb, uint16_t a, uint8_t op) {
  BASE(eraseFile_b00);
  C = op;
  CYC(a, a + 2);
  if (a != b_+0) CYC(a + 2, a + 4);
  bank_push(gb, b_+2, 0x07);
  CALL_ROM(b_+12, ROM_b07_fileManagementFunction);
  C = A;
  CYC(b_+15, b_+16);
  bank_pop(gb, b_+16);
  A = C;
  CYC(b_+22, b_+24);
  ret_effect(gb);
}

void initializeFile_b00_hook(GB *gb) { file_management(gb, SYM(initializeFile_b00), 0x00); }
void saveFile_b00_hook(GB *gb) { file_management(gb, SYM(saveFile_b00), 0x01); }
void loadFile_b00_hook(GB *gb) { file_management(gb, SYM(loadFile_b00), 0x02); }
void eraseFile_b00_hook(GB *gb) { file_management(gb, SYM(eraseFile_b00), 0x03); }

void initSound_b00_hook(GB *gb) {
  BASE(initSound_b00);
  SET_BC(0x4000);
  CYC(b_+0, b_+3);
  start_sound(gb);
  ret_effect(gb);
}

static void intro_cinematic_after_call(GB *gb, uint16_t sp0_) {
  BASE(intro_cinematic);
  switch_bank(gb, b_+13, 0x05);
  CALL_C(b_+20, updateSpecialObjects_hook, SYM(updateSpecialObjects), b_+23);
  CALL_C(b_+23, loadLinkAndCompanionAnimationFrame_hook, SYM(loadLinkAndCompanionAnimationFrame), b_+26);
  switch_bank(gb, b_+26, 0x04);
  CALL_C(b_+33, updateAnimations_hook, SYM(updateAnimations), b_+36);
  CALL_C(b_+36, updateInteractionsAndDrawAllSprites_hook, SYM(updateInteractionsAndDrawAllSprites), b_+39);
  bank_pop(gb, b_+39);
  CYC(b_+45, b_+46);
  ret_effect(gb);
}

void intro_cinematic_hook(GB *gb) {
  BASE(intro_cinematic);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, b_+0, 0x03);
  CALL_C(b_+10, runIntroCinematic_hook, SYM(runIntroCinematic), b_+13);
  intro_cinematic_after_call(gb, sp0_);
}

void intro_cinematic__afterCall2d27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  intro_cinematic_after_call(gb, sp0_);
}

// the enemy state machine's common prologue: returns the case index in c

void enemyStandardUpdate_hook(GB *gb) {
  BASE(enemyStandardUpdate);
  H = D;
  L = 0x84;
  CYC(b_+0, b_+4); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+7);
    inter_bank_call(gb, b_+33, 0x3f, ROM_b3f_enemyLoadGraphicsAndProperties);
    CALL_ROM(b_+41, ROM_getRandomNumber_noPreserveVars);
    E = 0xbd;
    CYC(b_+44, b_+47); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    A = 0x01;
    CYC(b_+47, b_+51); mem_wr(gb, DE, A);
    CYC(b_+51, b_+53);
    C = 0x00;
    CYC(b_+30, b_+33);
    ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+7);
  L = 0xaa;
  CYC(b_+7, b_+11); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+11, b_+13); C = 0x04; CYC(b_+53, b_+56); ret_effect(gb); return; }
  CYC(b_+11, b_+13);
  E = 0xad;
  CYC(b_+13, b_+16); A = mem_rd(gb, DE);
  alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+20);
    L = E;
    CYC(b_+56, b_+58); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    C = 0x05;
    CYC(b_+58, b_+61);
    ret_effect(gb);
    return;
  }
  CYC(b_+16, b_+20);
  L = alu_dec8(gb, L);
  CYC(b_+20, b_+22); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+22, b_+25);
    L = 0xbf;
    CYC(b_+61, b_+65); alu_bit(gb, 1, mem_rd(gb, HL));
    if (!(F & FZ)) { CYCT(b_+65, b_+67); C = 0x00; CYC(b_+30, b_+33); ret_effect(gb); return; }
    CYC(b_+65, b_+67);
    C = 0x03;
    CYC(b_+67, b_+70);
    ret_effect(gb);
    return;
  }
  CYC(b_+22, b_+25);
  E = alu_inc8(gb, E);
  CYC(b_+25, b_+27); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (F & FZ) { CYC(b_+27, b_+30); C = 0x00; CYC(b_+30, b_+33); ret_effect(gb); return; }
  CYCT(b_+27, b_+30);
  CYC(b_+70, b_+73); A = W8(wFrameCounter);
  alu_rrca(gb);
  if (F & FC) {
    CYC(b_+73, b_+76);
    L = E;
    CYC(b_+76, b_+78); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+78, b_+79); A = mem_rd(gb, HL);
    alu_cp(gb, 0x1e);
    if (F & FC) {
      CYC(b_+79, b_+83);
      alu_rrca(gb);
      if (F & FC) {
        CYC(b_+83, b_+86);
        L = 0x8d;
        CYC(b_+86, b_+89); A = mem_rd(gb, HL);
        alu_xor(gb, 0x01);
        CYC(b_+89, b_+92); mem_wr(gb, HL, A);
      } else CYCT(b_+83, b_+86);
    } else CYCT(b_+79, b_+83);
  } else CYCT(b_+73, b_+76);
  L = 0x84;
  CYC(b_+92, b_+95); A = mem_rd(gb, HL);
  alu_cp(gb, 0x08);
  if (F & FC) CYCT(b_+95, b_+99);
  else {
    CYC(b_+95, b_+99);
    L = 0x8f;
    CYC(b_+99, b_+102); A = mem_rd(gb, HL);
    A = alu_dec8(gb, A);
    alu_cp(gb, 0x08);
    if (F & FC) CYCT(b_+102, b_+107);
    else {
      CYC(b_+102, b_+107);
      C = 0x20;
      CYC(b_+107, b_+109);
      CALL_ROM(b_+109, ROM_objectUpdateSpeedZAndBounce);
      if (!(F & FC)) { CYCT(b_+112, b_+114); C = 0x02; CYC(b_+120, b_+123); ret_effect(gb); return; }
      CYC(b_+112, b_+114);
      H = D;
      CYC(b_+114, b_+115);
    }
  }
  L = 0x94;
  alu_xor(gb, A);
  CYC(b_+115, b_+119); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A);
  C = 0x02;
  CYC(b_+120, b_+123);
  ret_effect(gb);
}

void timerInterrupt_hook(GB *gb);
void serialInterrupt_hook(GB *gb);void vblankInterrupt_hook(GB *gb);
void lcdInterrupt_hook(GB *gb);
void resumeThreadInAFrames_hook(GB *gb);
static void thread_state_flag(GB *gb, uint16_t a, bool set) {
  CYC(a, a + 1); push_effect(gb, HL);
  L = A;
  H = wThreadStateBuffer >> 8;
  CYC(a + 1, a + 4);
  uint8_t v = mem_rd(gb, HL);
  CYC(a + 4, a + 6); mem_wr(gb, HL, set ? (uint8_t)(v | 0x80) : (uint8_t)(v & 0x7f));
  CYC(a + 6, a + 7); SET_HL(pop_effect(gb));
  CYC(a + 7, a + 8);
  ret_effect(gb);
}

void threadFunc_088b_hook(GB *gb) { thread_state_flag(gb, SYM(threadFunc_088b), true); }
void threadFunc_0893_hook(GB *gb) { thread_state_flag(gb, SYM(threadFunc_0893), false); }

void threadStop_hook(GB *gb) {
  BASE(threadStop);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  L = A;
  H = wThreadStateBuffer >> 8;
  CYC(b_+1, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x00);
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb));
  CYC(b_+7, b_+8);
  ret_effect(gb);
}

static void copy_initial_thread_state(GB *gb, uint16_t loop) {
  for (;;) {
    CYC(loop, loop + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(loop + 1, loop + 2); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(loop + 2, loop + 6); continue; }
    CYC(loop + 2, loop + 6);
    break;
  }
}

static void set_thread_restart_address(GB *gb, uint16_t a) {
  L = C;
  H = D;
  CYC(a, a + 2);
  CYC(a + 2, a + 3); SET_BC(pop_effect(gb));
  CYC(a + 3, a + 4); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  CYC(a + 4, a + 5);
  CYC(a + 5, a + 6); mem_wr(gb, HL, B);
}

void threadRestart_hook(GB *gb) {
  BASE(threadRestart);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, BC);
  E = A;
  alu_add(gb, 0x04);
  C = A;
  D = 0x00;
  SET_HL(ROM_initialThreadStatesBase);
  alu_add_hl(gb, DE);
  D = wThreadStateBuffer >> 8;
  B = 0x08;
  CYC(b_+3, b_+17);
  copy_initial_thread_state(gb, b_+17);
  set_thread_restart_address(gb, b_+23);
  CYC(b_+29, b_+30); SET_DE(pop_effect(gb));
  CYC(b_+30, b_+31); SET_HL(pop_effect(gb));
  CYC(b_+31, b_+32);
  ret_effect(gb);
}

void restartThisThread_hook(GB *gb) {
  BASE(restartThisThread);
  CYC(b_+0, b_+1); push_effect(gb, BC);
  A = H8(hActiveThread);
  E = A;
  alu_add(gb, 0x04);
  C = A;
  D = 0x00;
  SET_HL(ROM_initialThreadStatesBase);
  alu_add_hl(gb, DE);
  D = wThreadStateBuffer >> 8;
  B = 0x08;
  CYC(b_+1, b_+17);
  copy_initial_thread_state(gb, b_+17);
  set_thread_restart_address(gb, b_+23);
  CYCT(b_+29, b_+31);
  _nextThread_hook(gb);
}

void stubThreadStart_hook(GB *gb) {
  BASE(stubThreadStart);
  A = H8(hActiveThread);
  L = A;
  H = wThreadStateBuffer >> 8;
  CYC(b_+0, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x00);
  CYCT(b_+7, b_+9);
  _nextThread_hook(gb);
}

void resumeThreadNextFrameAndSaveBank_hook(GB *gb) {
  BASE(resumeThreadNextFrameAndSaveBank);
  A = 0x01;
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); push_effect(gb, BC);
  B = A;
  A = H8(hRomBank);
  C = A;
  A = B;
  CYC(b_+3, b_+8);
  CYC(b_+8, b_+11); push_effect(gb, b_+11);
  resumeThreadInAFrames_hook(gb);
  A = C;
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); H8(hRomBank) = A;
  CYC(b_+14, b_+17); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+17, b_+18); SET_BC(pop_effect(gb));
  CYC(b_+18, b_+19);
  ret_effect(gb);
}

void resumeThreadNextFrame_hook(GB *gb) {
  BASE(resumeThreadNextFrame);
  A = 0x01;
  CYC(b_+0, b_+2);
  resumeThreadInAFrames_hook(gb);
}

void resumeThreadInAFrames_hook(GB *gb) {
  BASE(resumeThreadInAFrames);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, BC);
  B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = mem_rd(gb, hActiveThread);
  L = A;
  CYC(b_+6, b_+7);
  H = wThreadStateBuffer >> 8;
  CYC(b_+7, b_+9);
  A = 0x01;
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(b_+13, b_+14);
  burn_store_sp(gb, b_+14, hFF92);
  CYC(b_+17, b_+19); A = mem_rd(gb, hFF92);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+20, b_+22); A = mem_rd(gb, hFF93);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  _nextThread_hook(gb);
}

void writeToSC_hook(GB *gb) {
  BASE(writeToSC);
  CYC(b_+0, b_+1); push_effect(gb, AF);
  alu_and(gb, 0x01);
  CYC(b_+1, b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, IO_SC, A);
  CYC(b_+5, b_+6); SET_AF(pop_effect(gb));
  CYC(b_+6, b_+8); mem_wr(gb, IO_SC, A);
  CYC(b_+8, b_+9);
  ret_effect(gb);
}

static void serial_reti(GB *gb, uint16_t a) {
  CYC(a, a + 1); SET_AF(pop_effect(gb));
  CYC(a + 1, a + 2);
  reti_effect(gb);
}

void serialInterrupt_hook(GB *gb) {
  BASE(serialInterrupt);
  A = H8(hSerialInterruptBehaviour);
  alu_or(gb, A);
  CYC(b_+0, b_+3);
  if (!(F & FZ)) {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+7); A = mem_rd(gb, IO_SB);
    CYC(b_+7, b_+9); H8(hSerialByte) = A;
    alu_xor(gb, A);
    CYC(b_+9, b_+10);
    CYC(b_+10, b_+12); mem_wr(gb, IO_SB, A);
    A = alu_inc8(gb, A);
    CYC(b_+12, b_+13);
    CYC(b_+13, b_+15); H8(hReceivedSerialByte) = A;
    serial_reti(gb, b_+15);
    return;
  }
  CYCT(b_+3, b_+5);
  CYC(b_+17, b_+19); A = mem_rd(gb, IO_SB);
  alu_cp(gb, 0xe1);
  CYC(b_+19, b_+21);
  if (F & FZ) CYCT(b_+21, b_+23);
  else {
    CYC(b_+21, b_+23);
    alu_cp(gb, 0xe0);
    CYC(b_+23, b_+25);
    if (!(F & FZ)) {
      CYCT(b_+25, b_+27);
      A = 0xe1;
      CYC(b_+34, b_+36);
      CYC(b_+36, b_+38); mem_wr(gb, IO_SB, A);
      A = 0x80;
      CYC(b_+38, b_+40);
      CALL_ROM(b_+40, ROM_writeToSC);
      serial_reti(gb, b_+43);
      return;
    }
    CYC(b_+25, b_+27);
  }
  CYC(b_+27, b_+29); H8(hSerialInterruptBehaviour) = A;
  alu_xor(gb, A);
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); mem_wr(gb, IO_SB, A);
  serial_reti(gb, b_+32);
}

void serialFunc_0c73_hook(GB *gb) {
  BASE(serialFunc_0c73);
  alu_xor(gb, A);
  CYC(b_+0, b_+1);
  CYC(b_+1, b_+3); H8(hFFBD) = A;
  A = 0xe0;
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, IO_SB, A);
  A = 0x81;
  CYC(b_+7, b_+9);
  CYCT(b_+9, b_+11);
  writeToSC_hook(gb);
}

void disableSerialPort_hook(GB *gb) {
  BASE(disableSerialPort);
  alu_xor(gb, A);
  CYC(b_+0, b_+1);
  CYC(b_+1, b_+3); H8(hSerialInterruptBehaviour) = A;
  CYC(b_+3, b_+5); mem_wr(gb, IO_SB, A);
  CYCT(b_+5, b_+7);
  writeToSC_hook(gb);
}

void serialFunc_0c85_hook(GB *gb) {
  BASE(serialFunc_0c85);
  SET_HL(ROM_b16_serialFunc_44ac);
  E = GV(0x16, 0x15);
  CYC(b_+0, b_+5);
  CYC(b_+5, b_+8);
  interBankCall_hook(gb);
}

void serialFunc_0c8d_hook(GB *gb) {
  BASE(serialFunc_0c8d);
  CYC(b_+0, b_+1); push_effect(gb, DE);
  SET_HL(ROM_b16_serialFunc_4000);
  E = GV(0x16, 0x15);
  CYC(b_+1, b_+6);
  CALL_ROM(b_+6, ROM_interBankCall);
  CYC(b_+9, b_+10); SET_DE(pop_effect(gb));
  CYC(b_+10, b_+11);
  ret_effect(gb);
}

static void timer_interrupt_end(GB *gb) {
  BASE(timerInterrupt);
  CYC(b_+82, b_+83); SET_HL(pop_effect(gb));
  CYC(b_+83, b_+84); SET_DE(pop_effect(gb));
  CYC(b_+84, b_+85); SET_BC(pop_effect(gb));
  CYC(b_+85, b_+86); SET_AF(pop_effect(gb));
  CYC(b_+86, b_+87);
  reti_effect(gb);
}

void timerInterrupt_hook(GB *gb) {
  BASE(timerInterrupt);
  SET_HL(hFFB7);
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); timer_interrupt_end(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+9, b_+11); timer_interrupt_end(gb); return; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x01));
  L = alu_inc8(gb, L);
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+19); mem_wr(gb, HL, 0x07);
    CYC(b_+19, b_+21); A = mem_rd(gb, IO_TMA);
    A = alu_dec8(gb, A);
    CYC(b_+21, b_+22);
    CYC(b_+22, b_+24); mem_wr(gb, IO_TIMA, A);
  } else CYCT(b_+15, b_+17);
  A = 0x39;
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+29); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+29, b_+31); A = H8(hMusicVolume);
  alu_bit(gb, 7, A);
  CYC(b_+31, b_+33);
  if (F & FZ) CYCT(b_+33, b_+35);
  else {
    CYC(b_+33, b_+35);
    alu_and(gb, 0x03);
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+39); H8(hMusicVolume) = A;
    CALL_ROM(b_+39, ROM_b39_updateMusicVolume);
  }
  CYC(b_+42, b_+44); A = H8(hMusicQueueTail);
  B = A;
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); A = H8(hMusicQueueHead);
  alu_cp(gb, B);
  CYC(b_+47, b_+48);
  if (F & FZ) CYCT(b_+48, b_+50);
  else {
    CYC(b_+48, b_+50);
    H = wMusicQueue >> 8;
    CYC(b_+50, b_+52);
    for (;;) {
      L = A;
      CYC(b_+52, b_+53);
      CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+54, b_+55); push_effect(gb, BC);
      CYC(b_+55, b_+56); push_effect(gb, HL);
      CALL_ROM(b_+56, ROM_b39_playSound);
      CYC(b_+59, b_+60); SET_HL(pop_effect(gb));
      CYC(b_+60, b_+61); SET_BC(pop_effect(gb));
      A = L;
      alu_and(gb, 0xaf);
      alu_cp(gb, B);
      CYC(b_+61, b_+65);
      if (F & FZ) { CYC(b_+65, b_+67); break; }
      CYCT(b_+65, b_+67);
    }
    CYC(b_+67, b_+69); H8(hMusicQueueHead) = A;
  }
  CALL_ROM(b_+69, ROM_b39_updateSound);
  SET_HL(hFFB7);
  CYC(b_+72, b_+75);
  CYC(b_+75, b_+77); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0xfe));
  CYC(b_+77, b_+79); A = H8(hRomBank);
  CYC(b_+79, b_+82); mem_wr(gb, MBC_ROM_BANK, A);
  timer_interrupt_end(gb);
}

void vblankVector_hook(GB *gb) {
  CYC(0x0040, 0x0041); push_effect(gb, AF);
  CYC(0x0041, 0x0042); push_effect(gb, BC);
  CYC(0x0042, 0x0043); push_effect(gb, DE);
  CYC(0x0043, 0x0044); push_effect(gb, HL);
  CYC(0x0044, 0x0047);
  vblankInterrupt_hook(gb);
}

void lcdVector_hook(GB *gb) {
  CYC(0x0048, 0x0049); push_effect(gb, AF);
  CYC(0x0049, 0x004a); push_effect(gb, HL);
  CYC(0x004a, 0x004d);
  lcdInterrupt_hook(gb);
}

void timerVector_hook(GB *gb) {
  CYC(0x0050, 0x0051);
  gb->ime_delay = true; gb->ime_writes++;
  CYC(0x0051, 0x0052); push_effect(gb, AF);
  CYC(0x0052, 0x0053); push_effect(gb, BC);
  CYC(0x0053, 0x0054); push_effect(gb, DE);
  CYC(0x0054, 0x0055); push_effect(gb, HL);
  CYC(0x0055, 0x0058);
  timerInterrupt_hook(gb);
}

void serialVector_hook(GB *gb) {
  CYC(0x0058, 0x0059); push_effect(gb, AF);
  CYC(0x0059, 0x005c);
  serialInterrupt_hook(gb);
}

void vblankFunctionRet_hook(GB *gb);

static void vblank_queue_dispatch(GB *gb, uint16_t sp0) {
  BASE(runVBlankFunctions);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  C = A;
  B = 0x00;
  SET_HL(ROM_vblankFunctionsStart);
  CYC(b_+5, b_+11);
  alu_add_hl(gb, BC);
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13);
  hook_continue(gb, HL, sp0);
}

void runVBlankFunctions_hook(GB *gb) {
  BASE(runVBlankFunctions);
  uint16_t sp0 = gb->sp;
  SET_HL(wVBlankFunctionQueue);
  CYC(b_+0, b_+3);
  vblank_queue_dispatch(gb, sp0);
}

void vblankFunctionRet_hook(GB *gb) {
  BASE(vblankFunctionRet);
  uint16_t sp0 = gb->sp;
  CYC(b_+0, b_+2); A = H8(hVBlankFunctionQueueTail);
  alu_cp(gb, L);
  CYC(b_+2, b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); vblank_queue_dispatch(gb, sp0); return; }
  CYC(b_+3, b_+5);
  alu_xor(gb, A);
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); H8(hVBlankFunctionQueueTail) = A;
  CYC(b_+8, b_+9);
  ret_effect(gb);
}

static void vblank_copy_block(GB *gb, uint16_t a) {
  CYC(a, a + 1); SET_HL(pop_effect(gb));
  CYC(a + 1, a + 2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(a + 2, a + 4); mem_wr(gb, IO_VBK, A);
  CYC(a + 4, a + 5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(a + 5, a + 6);
  CYC(a + 6, a + 7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  D = A;
  CYC(a + 7, a + 8);
  CYC(a + 8, a + 9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(a + 9, a + 10);
  for (;;) {
    CYC(a + 10, a + 11); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(a + 11, a + 12); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    CYC(a + 12, a + 13);
    B = alu_dec8(gb, B);
    CYC(a + 13, a + 14);
    if (!(F & FZ)) { CYCT(a + 14, a + 16); continue; }
    CYC(a + 14, a + 16);
    break;
  }
  CYCT(a + 16, a + 18);
  vblankFunctionRet_hook(gb);
}

void vblankFunctionsStart_hook(GB *gb) { vblank_copy_block(gb, SYM(vblankFunctionsStart)); }
void vblankFunction0ad9_hook(GB *gb) { vblank_copy_block(gb, SYM(vblankFunction0ad9)); }

void vblankRunBank4Function_hook(GB *gb) {
  BASE(vblankRunBank4Function);
  A = 0x04;
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+5); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+5, b_+8);
  vblankRunBank4Function_b04_hook(gb);
}

void vblankFunction0aa8_hook(GB *gb) {
  BASE(vblankFunction0aa8);
  uint16_t sp0 = gb->sp;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  L = C;
  H = A;
  SET_BC(b_+12);
  CYC(b_+5, b_+10);
  CYC(b_+10, b_+11); push_effect(gb, BC);
  CYC(b_+11, b_+12);
  hook_continue(gb, HL, sp0);
}

static void vblank_write4_bytes(GB *gb) {
  BASE(vblankCopyTileFunction);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34);
  ret_effect(gb);
}

void vblankCopyTileFunction_hook(GB *gb) {
  BASE(vblankCopyTileFunction);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  SET_DE(ROM_vblankFunctionRet);
  CYC(b_+1, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, DE);
  alu_xor(gb, A);
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, IO_VBK, A);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  D = A;
  CYC(b_+11, b_+12);
  C = E;
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); push_effect(gb, b_+16);
  vblank_write4_bytes(gb);
  E = C;
  CYC(b_+16, b_+17);
  A = 0x01;
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, IO_VBK, A);
  vblank_write4_bytes(gb);
}

void vblankDmaFunction_hook(GB *gb) {
  BASE(vblankDmaFunction);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+10); mem_wr(gb, IO_HDMA1, A);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+13); mem_wr(gb, IO_HDMA2, A);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+16); mem_wr(gb, IO_VBK, A);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+19); mem_wr(gb, IO_HDMA3, A);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+22); mem_wr(gb, IO_HDMA4, A);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+25); mem_wr(gb, IO_HDMA5, A);
  CYC(b_+25, b_+28);
  vblankFunctionRet_hook(gb);
}

void vblankInterrupt_hook(GB *gb) {
  BASE(vblankInterrupt);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = H8(hNextLcdInterruptBehaviour);
  CYC(b_+2, b_+4); H8(hLcdInterruptBehaviour) = A;
  alu_xor(gb, A);
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); H8(hLcdInterruptCounter) = A;
  SET_HL(hFFB7);
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  SET_HL(wGfxRegsFinal);
  CYC(b_+12, b_+15);
  static const uint16_t regs[] = {IO_LCDC, IO_SCY, IO_SCX, IO_WY, IO_WX, IO_LYC};
  uint16_t a = b_+15;
  for (int i = 0; i < 6; i++) {
    CYC(a, a + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(a + 1, a + 3); mem_wr(gb, regs[i], A);
    a += 3;
  }
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) CYCT(b_+34, b_+36);
  else {
    CYC(b_+34, b_+36);
    SET_DE(wGfxRegs2);
    CYC(b_+36, b_+39);
    L = wGfxRegs3 & 0xff;
    CYC(b_+39, b_+41);
    for (int i = 0; i < 6; i++) {
      uint16_t b = (uint16_t)(b_+41 + i * 3);
      CYC(b, b + 1); A = mem_rd(gb, DE);
      CYC(b + 1, b + 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
      if (i < 5) { E = alu_inc8(gb, E); CYC(b + 2, b + 3); }
    }
    CYC(b_+58, b_+60); A = mem_rd(gb, IO_VBK);
    B = A;
    CYC(b_+60, b_+61);
    CYC(b_+61, b_+63); A = mem_rd(gb, IO_SVBK);
    C = A;
    CYC(b_+63, b_+64);
    CYC(b_+64, b_+65); push_effect(gb, BC);
    CYC(b_+65, b_+67); A = H8(hVBlankFunctionQueueTail);
    alu_or(gb, A);
    CYC(b_+67, b_+68);
    if (!(F & FZ)) CALL_C_CC(b_+68, runVBlankFunctions_hook, ROM_runVBlankFunctions, b_+71);
    else CYC(b_+68, b_+71);
    CALL_C(b_+71, updateDirtyPalettes_hook, ROM_updateDirtyPalettes, b_+74);
    CYC(b_+74, b_+75); gb->ime = false; gb->ime_delay = false; gb->ime_writes++;
    CALL_C(b_+75, hramOamDmaFunction_hook, ROM_hramOamDmaFunction, b_+78);
    CYC(b_+78, b_+79); SET_BC(pop_effect(gb));
    A = C;
    CYC(b_+79, b_+80);
    CYC(b_+80, b_+82); mem_wr(gb, IO_SVBK, A);
    A = B;
    CYC(b_+82, b_+83);
    CYC(b_+83, b_+85); mem_wr(gb, IO_VBK, A);
    SET_HL(wGfxRegs6_LCDC);
    CYC(b_+85, b_+88);
    static const uint16_t final[] = {wGfxRegs7_LCDC, wGfxRegs7_SCY, wGfxRegs7_SCX};
    for (int i = 0; i < 3; i++) {
      uint16_t b = (uint16_t)(b_+88 + i * 4);
      CYC(b, b + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b + 1, b + 4); mem_wr(gb, final[i], A);
    }
  }
  SET_HL(hFFB7);
  CYC(b_+100, b_+103);
  CYC(b_+103, b_+105); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
  CYC(b_+105, b_+107); A = H8(hRomBank);
  CYC(b_+107, b_+109); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+109, b_+111);
  else {
    CYC(b_+109, b_+111);
    CYC(b_+111, b_+113); A = H8(hSoundDataBaseBank2);
  }
  CYC(b_+113, b_+116); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(b_+116, b_+117); SET_HL(pop_effect(gb));
  CYC(b_+117, b_+118); SET_DE(pop_effect(gb));
  CYC(b_+118, b_+119); SET_BC(pop_effect(gb));
  CYC(b_+119, b_+120); SET_AF(pop_effect(gb));
  CYC(b_+120, b_+121);
  reti_effect(gb);
}

void _lcdInterruptEnd_hook(GB *gb) {
  BASE(_lcdInterruptEnd);
  CYC(b_+0, b_+1); SET_BC(pop_effect(gb));
  CYC(b_+1, b_+2); SET_HL(pop_effect(gb));
  CYC(b_+2, b_+3); SET_AF(pop_effect(gb));
  CYC(b_+3, b_+4);
  reti_effect(gb);
}

void lcdInterrupt_clearLYC_hook(GB *gb) {
  BASE(lcdInterrupt_clearLYC);
  A = 0xc7;
  CYC(b_+0, b_+2); mem_wr(gb, IO_LYC, A);
  CYC(b_+2, b_+4);
  _lcdInterruptEnd_hook(gb);
}

void lcdInterrupt_clearWXY_hook(GB *gb) {
  BASE(lcdInterrupt_clearWXY);
  A = 0xc7;
  CYC(b_+0, b_+2); mem_wr(gb, IO_WY, A);
  CYC(b_+2, b_+4); mem_wr(gb, IO_WX, A);
  CYC(b_+4, b_+6);
  lcdInterrupt_clearLYC_hook(gb);
}

void lcdInterrupt_setLcdcToA7_hook(GB *gb) {
  BASE(lcdInterrupt_setLcdcToA7);
  for (;;) {
    CYC(b_+0, b_+2); A = mem_rd(gb, IO_STAT);
    CYC(b_+2, b_+3); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(b_+3, b_+5); continue; }
    CYC(b_+3, b_+5);
    break;
  }
  A = 0xa7;
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, IO_LCDC, A);
  CYCT(b_+9, b_+11);
  lcdInterrupt_clearLYC_hook(gb);
}

void lcdInterrupt_ringMenu_hook(GB *gb) {
  BASE(lcdInterrupt_ringMenu);
  for (;;) {
    CYC(b_+0, b_+2); A = mem_rd(gb, IO_STAT);
    CYC(b_+2, b_+3); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(b_+3, b_+5); continue; }
    CYC(b_+3, b_+5);
    break;
  }
  CYC(b_+5, b_+7); mem_wr(gb, IO_SCX, A);
  A = 0x87;
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, IO_LCDC, A);
  CYC(b_+11, b_+13); A = H8(hLcdInterruptCounter);
  A = alu_dec8(gb, A);
  CYC(b_+13, b_+14);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
  } else {
    CYC(b_+14, b_+16);
    A = mem_rd(gb, wRingMenu_mode);
    CYC(b_+16, b_+19);
    alu_or(gb, A);
    CYC(b_+19, b_+20);
    if (F & FZ) CYCT(b_+20, b_+22);
    else {
      CYC(b_+20, b_+22);
      A = 0x87;
      CYC(b_+22, b_+24);
      CYC(b_+24, b_+26); mem_wr(gb, IO_LYC, A);
    }
    A = 0x02;
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+30); H8(hLcdInterruptCounter) = A;
    CYCT(b_+30, b_+32);
    _lcdInterruptEnd_hook(gb);
    return;
  }
  A = 0x80;
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, IO_SCY, A);
  CYCT(b_+36, b_+38);
  lcdInterrupt_clearWXY_hook(gb);
}

void lcdInterrupt_0bea_hook(GB *gb) {
  BASE(lcdInterrupt_0bea);
  for (;;) {
    CYC(b_+0, b_+2); A = mem_rd(gb, IO_STAT);
    CYC(b_+2, b_+3); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(b_+3, b_+5); continue; }
    CYC(b_+3, b_+5);
    break;
  }
  SET_HL(wGfxRegs7);
  CYC(b_+5, b_+8);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, IO_LCDC, A);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, IO_SCY, A);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, IO_SCX, A);
  CYCT(b_+17, b_+19);
  lcdInterrupt_clearLYC_hook(gb);
}

static void lcd_interrupt_not_status_bar(GB *gb) {
  BASE(lcdInterrupt);
  A = H8(hLcdInterruptBehaviour);
  CYC(b_+80, b_+82);
  alu_cp(gb, 0x07);
  CYC(b_+82, b_+84);
  if (!(F & FC)) { CYCT(b_+84, b_+86); lcdInterrupt_clearLYC_hook(gb); return; }
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+87);
  push_effect(gb, b_+87);
  uint16_t target = rst_jump_table(gb);
  if (target == ROM_lcdInterrupt_clearLYC) lcdInterrupt_clearLYC_hook(gb);
  else if (target == ROM_lcdInterrupt_setLcdcToA7) lcdInterrupt_setLcdcToA7_hook(gb);
  else if (target == ROM_lcdInterrupt_clearWXY) lcdInterrupt_clearWXY_hook(gb);
  else if (target == ROM_lcdInterrupt_ringMenu) lcdInterrupt_ringMenu_hook(gb);
  else if (target == ROM_lcdInterrupt_0bea) lcdInterrupt_0bea_hook(gb);
  else hook_handoff(gb, target);
}

void lcdInterrupt_hook(GB *gb) {
  BASE(lcdInterrupt);
  A = H8(hLcdInterruptBehaviour);
  CYC(b_+0, b_+2);
  alu_cp(gb, 0x02);
  CYC(b_+2, b_+4);
  if (F & FC) {
    CYC(b_+4, b_+6);
    alu_or(gb, A);
    A = mem_rd(gb, IO_LY);
    CYC(b_+6, b_+9);
    L = A;
    H = wBigBuffer >> 8;
    CYC(b_+9, b_+12);
    A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+12, b_+13);
    if (F & FZ) {
      CYC(b_+13, b_+15);
      CYC(b_+15, b_+17); mem_wr(gb, IO_SCX, A);
      CYCT(b_+17, b_+19);
    } else {
      CYCT(b_+13, b_+15);
      CYC(b_+19, b_+21); mem_wr(gb, IO_SCY, A);
    }
    A = L;
    CYC(b_+21, b_+22);
    alu_cp(gb, 0x90);
    CYC(b_+22, b_+24);
    if (F & FC) { CYC(b_+24, b_+26); CYC(b_+26, b_+28); mem_wr(gb, IO_LYC, A); }
    else { CYCT(b_+24, b_+26); }
    CYC(b_+28, b_+29); SET_HL(pop_effect(gb));
    CYC(b_+29, b_+30); SET_AF(pop_effect(gb));
    CYC(b_+30, b_+31);
    reti_effect(gb);
    return;
  }
  CYCT(b_+4, b_+6);
  CYC(b_+31, b_+32); push_effect(gb, BC);
  C = 0x03;
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); A = H8(hLcdInterruptCounter);
  alu_or(gb, A);
  CYC(b_+36, b_+37);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); lcd_interrupt_not_status_bar(gb); return; }
  CYC(b_+37, b_+39);
  SET_HL(wGfxRegs3);
  CYC(b_+39, b_+42);
  for (;;) {
    CYC(b_+42, b_+44); A = mem_rd(gb, IO_STAT);
    CYC(b_+44, b_+45); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(b_+45, b_+47); continue; }
    CYC(b_+45, b_+47);
    break;
  }
  static const uint16_t regs[] = {IO_LCDC, IO_SCY, IO_SCX, IO_WY, IO_WX, IO_LYC};
  uint16_t a = b_+47;
  for (int i = 0; i < 6; i++) {
    CYC(a, a + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(a + 1, a + 3); mem_wr(gb, regs[i], A);
    a += 3;
  }
  CYC(b_+65, b_+67); A = H8(hLcdInterruptBehaviour);
  alu_cp(gb, 0x02);
  CYC(b_+67, b_+69);
  if (!(F & FZ)) CYCT(b_+69, b_+71);
  else {
    CYC(b_+69, b_+71);
    alu_xor(gb, A);
    CYC(b_+71, b_+72);
    CYC(b_+72, b_+74); H8(hLcdInterruptBehaviour) = A;
  }
  A = 0x01;
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); H8(hLcdInterruptCounter) = A;
  CYCT(b_+78, b_+80);
  _lcdInterruptEnd_hook(gb);
}

void begin_hook(GB *gb) {
  BASE(begin);
  CYC(b_+0, b_+1);
  CYC(b_+1, b_+2); gb->ime = false; gb->ime_delay = false; gb->ime_writes++;
  alu_cp(gb, 0x11);
  CYC(b_+2, b_+4);
  A = 0x00;
  CYC(b_+4, b_+6);
  if (F & FZ) {
    CYC(b_+6, b_+8);
    A = alu_inc8(gb, A);
    CYC(b_+8, b_+9);
    alu_bit(gb, 0, B);
    CYC(b_+9, b_+11);
    if (F & FZ) CYCT(b_+11, b_+13);
    else {
      CYC(b_+11, b_+13);
      A = 0xff;
      CYC(b_+13, b_+15);
    }
  } else CYCT(b_+6, b_+8);
  CYC(b_+15, b_+17); H8(hGameboyType) = A;
  A = 0x37;
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); H8(hRng1) = A;
  A = 0x0d;
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); H8(hRng2) = A;
  resetGame_hook(gb);
}

void resetGame__afterSp016c_hook(GB *gb) {
  BASE(resetGame);
  A = 0x03;
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); H8(hRomBank) = A;
  CYC(b_+7, b_+10); mem_wr(gb, MBC_ROM_BANK, A);
  CYCT(b_+10, b_+13);
  init_hook(gb);
}

void resetGame_hook(GB *gb) {
  BASE(resetGame);
  CYC(b_+0, b_+3); gb->sp = wMainStackTop; gb->sp_loads++;
  hook_handoff(gb, b_+3);
}

void _nextThread__afterSp091a_hook(GB *gb) {
  BASE(_nextThread);
  H = wThreadStateBuffer >> 8;
  CYC(b_+3, b_+5);
  A = 0x01;
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, IO_SVBK, A);
  CYCT(b_+9, b_+11);
  _mainLoop_nextThread_hook(gb);
}

void _nextThread_hook(GB *gb) {
  BASE(_nextThread);
  CYC(b_+0, b_+3); gb->sp = wMainStackTop; gb->sp_loads++;
  hook_handoff(gb, b_+3);
}

void startGame__afterSp0925_hook(GB *gb) {
  BASE(startGame);
  SET_HL(SYM(_initialThreadStates));
  CYC(b_+3, b_+6);
  SET_DE(wThreadStateBuffer);
  CYC(b_+6, b_+9);
  B = 0x20;
  CYC(b_+9, b_+11);
  for (;;) {
    CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+12, b_+13); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+13, b_+14);
    B = alu_dec8(gb, B);
    CYC(b_+14, b_+15);
    if (!(F & FZ)) { CYCT(b_+15, b_+17); continue; }
    CYC(b_+15, b_+17);
    break;
  }
  _mainLoop_hook(gb);
}

void startGame_hook(GB *gb) {
  BASE(startGame);
  CYC(b_+0, b_+3); gb->sp = wMainStackTop; gb->sp_loads++;
  hook_handoff(gb, b_+3);
}

void _mainLoop_hook(GB *gb) {
  BASE(_mainLoop);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, pollInput_hook, ROM_pollInput, b_+3);
  A = mem_rd(gb, hIntroInputsEnabled);
  CYC(b_+3, b_+5);
  alu_add(gb, A);
  CYC(b_+5, b_+6);
  if (F & FZ) CYCT(b_+6, b_+8);
  else {
    CYC(b_+6, b_+8);
    A = mem_rd(gb, wKeysPressed);
    CYC(b_+8, b_+11);
    alu_sub(gb, (JOY_A | JOY_B | JOY_START | JOY_SELECT));
    CYC(b_+11, b_+13);
    if (F & FZ) { CYCT(b_+13, b_+16); resetGame_hook(gb); return; }
    CYC(b_+13, b_+16);
  }
  A = 0x10;
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); H8(hOamTail) = A;
  H = wThreadStateBuffer >> 8;
  CYC(b_+20, b_+22);
  A = wThreadStateBuffer & 0xff;
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); H8(hActiveThread) = A;
  L = A;
  CYC(b_+26, b_+27);
  A = mem_rd(gb, HL);
  CYC(b_+27, b_+28);
  A = alu_dec8(gb, A);
  CYC(b_+28, b_+29);
  if (F & FZ) { CYCT(b_+29, b_+31); _countdownToRunThread_hook(gb); return; }
  CYC(b_+29, b_+31);
  A = alu_dec8(gb, A);
  CYC(b_+31, b_+32);
  if (F & FZ) { CYCT(b_+32, b_+34); _initializeThread_hook(gb); return; }
  CYC(b_+32, b_+34);
  _mainLoop_nextThread_hook(gb);
}

void _mainLoop_nextThread_hook(GB *gb) {
  BASE(_mainLoop_nextThread);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+2); A = mem_rd(gb, hActiveThread);
    alu_add(gb, 0x08);
    CYC(b_+2, b_+4);
    CYC(b_+4, b_+6); H8(hActiveThread) = A;
    alu_cp(gb, (wThreadStateBuffer + 4 * 8) & 0xff);
    CYC(b_+6, b_+8);
    if (!(F & FZ)) {
      CYCT(b_+8, b_+10);
      L = A;
      CYC((SYM(_mainLoop) + 26), (SYM(_mainLoop) + 27));
      A = mem_rd(gb, HL);
      CYC((SYM(_mainLoop) + 27), (SYM(_mainLoop) + 28));
      A = alu_dec8(gb, A);
      CYC((SYM(_mainLoop) + 28), (SYM(_mainLoop) + 29));
      if (F & FZ) { CYCT((SYM(_mainLoop) + 29), (SYM(_mainLoop) + 31)); _countdownToRunThread_hook(gb); return; }
      CYC((SYM(_mainLoop) + 29), (SYM(_mainLoop) + 31));
      A = alu_dec8(gb, A);
      CYC((SYM(_mainLoop) + 31), (SYM(_mainLoop) + 32));
      if (F & FZ) { CYCT((SYM(_mainLoop) + 32), (SYM(_mainLoop) + 34)); _initializeThread_hook(gb); return; }
      CYC((SYM(_mainLoop) + 32), (SYM(_mainLoop) + 34));
      continue;
    }
    CYC(b_+8, b_+10);
    break;
  }
  A = 0x3f;
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); H8(hRomBank) = A;
  CYC(b_+14, b_+17); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+17, refreshDirtyPalettes_hook, ROM_refreshDirtyPalettes, b_+20);
  alu_xor(gb, A);
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, IO_SVBK, A);
  SET_HL(wc49e);
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  SET_HL(wGfxRegs1);
  CYC(b_+27, b_+30);
  SET_DE(wGfxRegsFinal);
  CYC(b_+30, b_+33);
  B = 0x06;
  CYC(b_+33, b_+35);
  for (;;) {
    CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+36, b_+37); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(b_+37, b_+38);
    B = alu_dec8(gb, B);
    CYC(b_+38, b_+39);
    if (!(F & FZ)) { CYCT(b_+39, b_+41); continue; }
    CYC(b_+39, b_+41);
    break;
  }
  SET_HL(wVBlankChecker);
  CYC(b_+41, b_+44);
  A = 0xff;
  CYC(b_+44, b_+46); mem_wr(gb, HL, A);
  for (;;) {
    int halt_r;
    do { I(b_+46, 1); halt_r = hook_halt(gb, b_+47); } while (halt_r == 1);
    if (halt_r < 0) { hook_handoff(gb, b_+47); return; }
    CYC(b_+47, b_+48);
    alu_bit(gb, 7, mem_rd(gb, HL));
    CYC(b_+48, b_+50);
    if (!(F & FZ)) { CYCT(b_+50, b_+52); continue; }
    CYC(b_+50, b_+52);
    break;
  }
  CYCT(b_+52, b_+54);
  _mainLoop_hook(gb);
}

void _countdownToRunThread__afterSp0998_hook(GB *gb) {
  BASE(_countdownToRunThread);
  CYC(b_+13, b_+14); SET_BC(pop_effect(gb));
  CYC(b_+14, b_+15); SET_DE(pop_effect(gb));
  CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  CYC(b_+16, b_+17);
  ret_effect(gb);
}

void _countdownToRunThread_hook(GB *gb) {
  BASE(_countdownToRunThread);
  L = alu_inc8(gb, L);
  CYC(b_+0, b_+1);
  uint8_t v = alu_dec8(gb, mem_rd(gb, HL));
  CYC(b_+1, b_+2); mem_wr(gb, HL, v);
  if (F & FZ) CYC(b_+2, b_+4);
  else { CYCT(b_+2, b_+4); _mainLoop_nextThread_hook(gb); return; }
  L = alu_dec8(gb, L);
  CYC(b_+4, b_+5);
  A = 0x03;
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(b_+8, b_+9);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10);
  H = mem_rd(gb, HL);
  CYC(b_+10, b_+11);
  L = A;
  CYC(b_+11, b_+12);
  gb->sp = HL; gb->sp_loads++;
  CYC(b_+12, b_+13);
  hook_handoff(gb, b_+13);
}

void _initializeThread__afterSp09aa_hook(GB *gb) {
  BASE(_initializeThread);
  CYC(b_+14, b_+15); push_effect(gb, BC);
  CYC(b_+15, b_+16);
  ret_effect(gb);
}

void _initializeThread_hook(GB *gb) {
  BASE(_initializeThread);
  A = 0x03;
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(b_+3, b_+4);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5);
  E = A;
  CYC(b_+5, b_+6);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7);
  D = A;
  CYC(b_+7, b_+8);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9);
  B = mem_rd(gb, HL);
  CYC(b_+9, b_+10);
  C = A;
  CYC(b_+10, b_+11);
  L = E;
  CYC(b_+11, b_+12);
  H = D;
  CYC(b_+12, b_+13);
  gb->sp = HL; gb->sp_loads++;
  CYC(b_+13, b_+14);
  hook_handoff(gb, b_+14);
}

void textThreadStart_hook(GB *gb) {
  BASE(textThreadStart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  A = mem_rd(gb, wScrollMode);
  CYC(b_+0, b_+3);
  alu_or(gb, A);
  CYC(b_+3, b_+4);
  if (F & FZ) CYCT(b_+4, b_+6);
  else {
    CYC(b_+4, b_+6);
    alu_and(gb, 0x01);
    CYC(b_+6, b_+8);
    if (!(F & FZ)) { CYCT(b_+8, b_+10); }
    else {
      CYC(b_+8, b_+10);
      alu_xor(gb, A);
      CYC(b_+10, b_+11);
      CYC(b_+11, b_+14); mem_wr(gb, wTextIsActive, A);
      CYC(b_+14, b_+17); mem_wr(gb, wTextboxFlags, A);
      CYCT(b_+17, b_+20);
      stubThreadStart_hook(gb);
      return;
    }
  }
  A = 0x3f;
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); H8(hRomBank) = A;
  CYC(b_+24, b_+27); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(b_+27, initTextbox_hook, ROM_initTextbox, b_+30);
  for (;;) {
    A = 0x3f;
    CYC(b_+30, b_+32);
    CYC(b_+32, b_+34); H8(hRomBank) = A;
    CYC(b_+34, b_+37); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(b_+37, updateTextbox_hook, ROM_updateTextbox, b_+40);
    CALL_C(b_+40, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, b_+43);
    CYCT(b_+43, b_+45);
  }
}

void fileSelectThreadStart_hook(GB *gb) {
  BASE(fileSelectThreadStart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  SET_HL(wFileSelect_mode);
  CYC(b_+0, b_+3);
  B = 0x10;
  CYC(b_+3, b_+5);
  CALL_C(b_+5, clearMemory_hook, ROM_clearMemory, b_+8);
  for (;;) {
    A = 0x02;
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); H8(hRomBank) = A;
    CYC(b_+12, b_+15); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(b_+15, b2_fileSelectScreen_hook, ROM_b2_fileSelectScreen, b_+18);
    CALL_C(b_+18, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, b_+21);
    CYCT(b_+21, b_+23);
  }
}

void thread_1b10_hook(GB *gb) {
  BASE(thread_1b10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  SET_HL(wSaveQuitMenu_state);
  CYC(b_+0, b_+3);
  B = 0x10;
  CYC(b_+3, b_+5);
  CALL_C(b_+5, clearMemory_hook, ROM_clearMemory, b_+8);
  A = 0x01;
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); mem_wr(gb, wSaveQuitMenu_gameOver, A);
  for (;;) {
    A = 0x02;
    CYC(b_+13, b_+15);
    CYC(b_+15, b_+17); H8(hRomBank) = A;
    CYC(b_+17, b_+20); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(b_+20, runSaveAndQuitMenu_hook, ROM_runSaveAndQuitMenu, b_+23);
    CALL_C(b_+23, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, b_+26);
    CYCT(b_+26, b_+28);
  }
}

void introThreadStart_hook(GB *gb) {
  BASE(introThreadStart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    SET_HL(wIntro_frameCounter);
    CYC(b_+0, b_+3);
    CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    A = 0x03;
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+8); H8(hRomBank) = A;
    CYC(b_+8, b_+11); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(b_+11, runIntro_hook, ROM_runIntro, b_+14);
    CALL_C(b_+14, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, b_+17);
    CYCT(b_+17, b_+19);
  }
}

void paletteFadeThreadStart_hook(GB *gb) {
  BASE(paletteFadeThreadStart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    A = 0x02;
    CYC(b_+0, b_+2);
    CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
    A = 0x01;
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+8); H8(hRomBank) = A;
    CYC(b_+8, b_+11); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(b_+11, paletteFadeHandler_hook, ROM_paletteFadeHandler, b_+14);
    CALL_C(b_+14, checkLockBG7Color3ToBlack_hook, ROM_checkLockBG7Color3ToBlack, b_+17);
    A = mem_rd(gb, wPaletteThread_updateRate);
    CYC(b_+17, b_+20);
    alu_or(gb, A);
    CYC(b_+20, b_+21);
    if (!(F & FZ)) CYCT(b_+21, b_+23);
    else {
      CYC(b_+21, b_+23);
      A = alu_inc8(gb, A);
      CYC(b_+23, b_+24);
    }
    CALL_C(b_+24, resumeThreadInAFrames_hook, ROM_resumeThreadInAFrames, b_+27);
    CYCT(b_+27, b_+29);
  }
}

void mainThreadStart_hook(GB *gb) {
  BASE(mainThreadStart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, restartSound_hook, ROM_restartSound, b_+3);
  CALL_C(b_+3, stopTextThread_hook, ROM_stopTextThread, b_+6);
  for (;;) {
    SET_HL(wPlaytimeCounter);
    CYC(b_+6, b_+9);
    CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+10, b_+11);
    CYC(b_+11, b_+14); mem_wr(gb, wFrameCounter, A);
    if (!(F & FZ)) {
      CYCT(b_+14, b_+16);
    } else {
      CYC(b_+14, b_+16);
      mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+16, b_+17);
      if (!(F & FZ)) {
        CYCT(b_+17, b_+19);
      } else {
        CYC(b_+17, b_+19);
        L = alu_inc8(gb, L);
        CYC(b_+19, b_+20);
        mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
        CYC(b_+20, b_+21);
        if (!(F & FZ)) {
          CYCT(b_+21, b_+23);
        } else {
          CYC(b_+21, b_+23);
          L = alu_inc8(gb, L);
          CYC(b_+23, b_+24);
          mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
          CYC(b_+24, b_+25);
        }
      }
    }
    A = 0x01;
    CYC(b_+25, b_+27);
    CYC(b_+27, b_+29); H8(hRomBank) = A;
    CYC(b_+29, b_+32); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(b_+32, runGameLogic_hook, ROM_runGameLogic, b_+35);
    CALL_C(b_+35, drawAllSprites_hook, ROM_drawAllSprites, b_+38);
    CALL_C(b_+38, checkReloadStatusBarGraphics_hook, ROM_checkReloadStatusBarGraphics, b_+41);
    CALL_C(b_+41, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, b_+44);
    CYCT(b_+44, b_+46);
  }
}

void hramOamDmaFunction_hook(GB *gb) {
  I(hOamFunc, 2); A = 0xcb;
  I((hOamFunc + 2), 3); mem_wr(gb, IO_DMA, A);
  I((hOamFunc + 4), 2); A = 0x28;
  for (;;) {
    I((hOamFunc + 6), 1); A = alu_dec8(gb, A);
    if (!(F & FZ)) { I((hOamFunc + 7), 3); continue; }
    I((hOamFunc + 7), 2);
    break;
  }
  RET((hOamFunc + 9)); return;
}

void wMusicReadFunction_hook(GB *gb) {
  I(wMusicReadFunction, 3); H8(hSoundDataBaseBank2) = A;
  I((wMusicReadFunction + 2), 4); mem_wr(gb, 0x2000, A);
  I((wMusicReadFunction + 5), 2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  I((wMusicReadFunction + 6), 1); C = A;
  I((wMusicReadFunction + 7), 3); A = H8(hSoundDataBaseBank);
  I((wMusicReadFunction + 9), 3); H8(hSoundDataBaseBank2) = A;
  I((wMusicReadFunction + 11), 4); mem_wr(gb, 0x2000, A);
  I((wMusicReadFunction + 14), 1); A = C;
  RET((wMusicReadFunction + 15)); return;
}
