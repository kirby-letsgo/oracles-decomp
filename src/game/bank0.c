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

static void multiply_a_by_c(GB *gb) {
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
}

void multiplyAByC_hook(GB *gb) {
  multiply_a_by_c(gb);
  ret_effect(gb);
}

static void multiply_a_by_16(GB *gb) {
  uint8_t a = A;
  C = (uint8_t)(a << 4);
  A = B = a >> 4;
  F = (A == 0 ? FZ : 0) | FH;
  CYC(0x01ac, 0x01b7);
}

void multiplyABy16_hook(GB *gb) {
  multiply_a_by_16(gb);
  ret_effect(gb);
}

static void multiply_a_by_8(GB *gb) {
  uint8_t a = A;
  C = (uint8_t)(a << 3);
  A = B = a >> 5;
  F = (A == 0 ? FZ : 0) | FH;
  CYC(0x01b7, 0x01c3);
}

void multiplyABy8_hook(GB *gb) {
  multiply_a_by_8(gb);
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

static void check_flag(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x0205, 0x020a);
  flag_hlpr(gb);
  CYC(0x020a, 0x020b); alu_and(gb, mem_rd(gb, HL));
  SET_BC(bc); SET_HL(hl);
  CYC(0x020b, 0x020e);
}

void checkFlag_hook(GB *gb) {
  check_flag(gb);
  ret_effect(gb);
}

static void set_flag(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x020e, 0x0213);
  flag_hlpr(gb);
  CYC(0x0213, 0x0214); alu_or(gb, mem_rd(gb, HL));
  CYC(0x0214, 0x0215); mem_wr(gb, HL, A);
  SET_BC(bc); SET_HL(hl);
  CYC(0x0215, 0x0218);
}

void setFlag_hook(GB *gb) {
  set_flag(gb);
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

static void dec16_ff8c(GB *gb) {
  uint16_t hl = HL;
  CYC(0x0878, 0x087f);
  dec16_cap(gb, hFF8C);
  SET_HL(hl);
  CYC(0x087f, 0x0881);
}

void dec16_ff8c_hook(GB *gb) {
  dec16_ff8c(gb);
  ret_effect(gb);
}

static void inc_hl_ref_with_cap(GB *gb) {
  CYC(0x0245, 0x0246); uint8_t v = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) CYCT(0x0246, 0x0247);
  else { CYC(0x0246, 0x0249); mem_wr(gb, HL, 0xff); CYC(0x0249, 0x024a); }
}

void incHlRefWithCap_hook(GB *gb) {
  inc_hl_ref_with_cap(gb);
  ret_effect(gb);
}

static void inc_hl_ref16_with_cap(GB *gb) {
  CYC(0x024a, 0x024b); uint8_t lo = alu_inc8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, lo);
  if (lo) { CYCT(0x024b, 0x024c); return; }
  CYC(0x024b, 0x024e); uint8_t hi = alu_inc8(gb, mem_rd(gb, HL + 1)); mem_wr(gb, HL + 1, hi);
  if (hi) { CYC(0x024e, 0x0252); return; }
  CYCT(0x024e, 0x0250);
  CYC(0x0252, 0x0256); mem_wr(gb, HL + 1, 0xff);
  CYC(0x0256, 0x0257); mem_wr(gb, HL, 0xff);
  CYC(0x0257, 0x0259);
}

void incHlRef16WithCap_hook(GB *gb) {
  inc_hl_ref16_with_cap(gb);
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

static void read_byte_sequential(GB *gb) {
  CYC(0x0788, 0x0789); uint8_t v = mem_rd(gb, HL);
  SET_HL(HL + 1);
  alu_bit(gb, 7, H);
  if (!(H & 0x80)) { CYCT(0x0789, 0x078c); A = v; return; }
  CYC(0x0789, 0x078f); H = 0x40;
  CYC(0x078f, 0x0791); uint8_t bank = (uint8_t)(H8(hRomBank) + 1);
  CYC(0x0791, 0x0794); H8(hRomBank) = bank;
  CYC(0x0794, 0x0797); mem_wr(gb, MBC_ROM_BANK, bank);
  CYC(0x0797, 0x0799);
  A = v;
}

void readByteSequential_hook(GB *gb) {
  read_byte_sequential(gb);
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

static void cp_active_ring(GB *gb) {
  CYC(0x23b0, 0x23b5); alu_cp(gb, W8(wActiveRing));
  CYC(0x23b5, 0x23b7);
}

void cpActiveRing_hook(GB *gb) {
  cp_active_ring(gb);
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

static void copy_position_with_offset(GB *gb) {
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
}

void objectCopyPositionWithOffset_hook(GB *gb) {
  copy_position_with_offset(gb);
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

// angles

static void object_get_relative_angle(GB *gb) {
  CYC(0x1ea4, 0x1ea6); A = H8(hActiveObjectType);
  alu_or(gb, OBJ_YH);
  E = A;
  CYC(0x1ea6, 0x1ea9);
  getRelativeAngle_hook(gb);
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
  CYC(0x2049, 0x205d); A = H8(hRomBank);
  CYC(0x205d, 0x205e); push_effect(gb, AF);
  A = 0x03;
  CYC(0x205e, 0x2062); H8(hRomBank) = A;
  CYC(0x2062, 0x2065); mem_wr(gb, MBC_ROM_BANK, A);
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
  CYC(0x2076, 0x2077); SET_AF(pop_effect(gb));
  CYC(0x2077, 0x2079); H8(hRomBank) = A;
  CYC(0x2079, 0x207c); mem_wr(gb, MBC_ROM_BANK, A);
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
  objectUpdateSpeedZ_sidescroll_givenYOffset_hook(gb);
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

static void object_h_check_collision_with_link(GB *gb) {
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
}

void objectHCheckCollisionWithLink_hook(GB *gb) {
  object_h_check_collision_with_link(gb);
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

static void set_tile_without_gfx_reload(GB *gb) {
  B = wRoomLayout >> 8;
  CYC(0x141c, 0x141f); mem_wr(gb, BC, A);
  CYC(0x141f, 0x1422);
  retrieve_tile_collision_value(gb);
  B = wRoomCollisions >> 8;
  CYC(0x1422, 0x1425); mem_wr(gb, BC, A);
  CYC(0x1425, 0x1426);
}

void setTileWithoutGfxReload_hook(GB *gb) {
  set_tile_without_gfx_reload(gb);
  ret_effect(gb);
}

static void set_tile_in_room_layout_buffer(GB *gb) {
  CYC(0x1426, 0x1428); uint8_t svbk = mem_rd(gb, IO_SVBK);
  CYC(0x1428, 0x142d); mem_wr(gb, IO_SVBK, 0x03);
  A = B;
  B = w3RoomLayoutBuffer >> 8;
  CYC(0x142d, 0x1431); mem_wr(gb, BC, A);
  A = svbk;
  CYC(0x1431, 0x1434); mem_wr(gb, IO_SVBK, A);
  CYC(0x1434, 0x1435);
}

void setTileInRoomLayoutBuffer_hook(GB *gb) {
  set_tile_in_room_layout_buffer(gb);
  ret_effect(gb);
}

// palette colour components (bank $17), 3 x 5-bit components per colour, $30 colours

static void extract_color_components(GB *gb) {
  CYC(0x13e9, 0x13eb); A = H8(hRomBank);
  CYC(0x13eb, 0x13ec); push_effect(gb, AF);
  A = 0x17;
  CYC(0x13ec, 0x13f0); H8(hRomBank) = A;
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
  CYC(0x1415, 0x1416); SET_AF(pop_effect(gb));
  CYC(0x1416, 0x1418); H8(hRomBank) = A;
  CYC(0x1418, 0x141b); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x141b, 0x141c);
}

void extractColorComponents_hook(GB *gb) {
  extract_color_components(gb);
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
  CYC(0x10cc, 0x10ce); A = H8(hRomBank);
  CYC(0x10ce, 0x10cf); push_effect(gb, AF);
  A = 0x16;
  CYC(0x10cf, 0x10d3); H8(hRomBank) = A;
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
  CYC(0x10f9, 0x10fa); SET_AF(pop_effect(gb));
  CYC(0x10fa, 0x10fc); H8(hRomBank) = A;
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
  uint16_t sp0_ = gb->sp;
  bank_push(gb, 0x1298, 0x06);
  A = 0x09;
  CYC(0x12a2, 0x12a4);
  CALL_C(0x12a4, showInfoTextForTile_hook, 0x42fb, 0x12a7);
  bank_pop(gb, 0x12a7);
  CYC(0x12ad, 0x12ae);
  ret_effect(gb);
}

// camera

void updateCamera_hook(GB *gb) {
  CYC(0x12ae, 0x12b1); A = W8(wScreenVariables);
  alu_and(gb, 0x05);
  if (F & FZ) { CYCT(0x12b1, 0x12b4); ret_effect(gb); return; }
  CYC(0x12b1, 0x12b4);
  bank_push(gb, 0x12b4, 0x01);
  CALL_ROM(0x12be, ROM_b01_updateCameraPosition);
  CALL_ROM(0x12c1, ROM_b01_updateGfxRegs2Scroll);
  CALL_ROM(0x12c4, ROM_b01_updateScreenShake);
  bank_pop(gb, 0x12c7);
  CYC(0x12cd, 0x12ce);
  ret_effect(gb);
}

void resetCamera_hook(GB *gb) {
  bank_push(gb, 0x12ce, 0x01);
  CALL_ROM(0x12d8, ROM_b01_calculateCameraPosition);
  CALL_ROM(0x12db, ROM_b01_updateGfxRegs2Scroll);
  bank_pop(gb, 0x12de);
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
  CYC(0x12fc, 0x12fe); A = H8(hRomBank);
  CYC(0x12fe, 0x12ff); push_effect(gb, AF);
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
  bank_pop(gb, 0x1318);
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

static void load_room_collisions(GB *gb) {
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
}

void loadRoomCollisions_hook(GB *gb) {
  load_room_collisions(gb);
  ret_effect(gb);
}

// treasure and item drops (bank $3f bodies)

void decideItemDrop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  C = A;
  CYC(0x16eb, 0x16ec);
  bank_push(gb, 0x16ec, 0x3f);
  CALL_C(0x16f6, decideItemDrop_body_hook, ROM_b3f_decideItemDrop_body, 0x16f9);
  bank_pop(gb, 0x16f9);
  A = C;
  alu_cp(gb, 0xff);
  CYC(0x16ff, 0x1703);
  ret_effect(gb);
}

void checkItemDropAvailable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  C = A;
  CYC(0x1703, 0x1704);
  bank_push(gb, 0x1704, 0x3f);
  A = C;
  CYC(0x170e, 0x170f);
  CALL_C(0x170f, checkItemDropAvailable_body_hook,
      ROM_b3f_checkItemDropAvailable_body, 0x1712);
  bank_pop(gb, 0x1712);
  A = C;
  alu_cp(gb, 0xff);
  CYC(0x1718, 0x171c);
  ret_effect(gb);
}

static void give_treasure(GB *gb, uint16_t sp0_) {
  B = A;
  CYC(0x171c, 0x171d);
  bank_push(gb, 0x171d, 0x3f);
  CALL_C(0x1727, giveTreasure_body_hook, ROM_b3f_giveTreasure_body, 0x172a);
  bank_pop(gb, 0x172a);
  A = B;
  alu_or(gb, A);
  CYC(0x1730, 0x1733);
}

void giveTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  give_treasure(gb, sp0_);
  ret_effect(gb);
}

void loseTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  B = A;
  CYC(0x1733, 0x1734);
  bank_push(gb, 0x1734, 0x3f);
  CALL_C(0x173e, loseTreasure_body_hook, ROM_b3f_loseTreasure_body, 0x1741);
  bank_pop(gb, 0x1741);
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
  bank_push(gb, 0x17e0, 0x3f);
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
  bank_pop(gb, 0x1803);
  A = 0x2d;
  CYC(0x1809, 0x180c);
  ret_effect(gb);
}

void refillSeedSatchel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  E = 0x20;
  CYC(0x180c, 0x180e);
  for (;;) {
    A = E;
    CYC(0x180e, 0x180f);
    CALL_C(0x180f, checkTreasureObtained_hook, 0x1748, 0x1812);
    if (!(F & FC)) CYCT(0x1812, 0x1814);
    else {
      CYC(0x1812, 0x1814);
      A = E;
      C = 0x99;
      CYC(0x1814, 0x181a);
      give_treasure(gb, sp0_);
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

static void add_to_gasha_maturity(GB *gb) {
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
}

void addToGashaMaturity_hook(GB *gb) {
  add_to_gasha_maturity(gb);
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

static void dec_num_enemies(GB *gb) {
  SET_HL(wNumEnemies);
  CYC(0x24b3, 0x24b7); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x24b7, 0x24b9); return; }
  CYC(0x24b7, 0x24ba); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x24ba, 0x24bb);
}

void decNumEnemies_hook(GB *gb) {
  dec_num_enemies(gb);
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

static void unmark_solid_position(GB *gb) {
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
}

void objectUnmarkSolidPosition_hook(GB *gb) {
  unmark_solid_position(gb);
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
  bank_push(gb, 0x1af7, 0x1c);
  SET_DE(wTmpcec0);
  B = 0x08;
  CYC(0x1b01, 0x1b09);
  copyMemory(gb, DE, HL, 8);
  SET_HL(HL + 8);
  SET_DE(DE + 8);
  B = 0;
  bank_pop(gb, 0x1b09);
  CYC(0x1b0f, 0x1b10);
  ret_effect(gb);
}

void copy20BytesFromBank_hook(GB *gb) {
  CYC(0x1a83, 0x1a85); A = H8(hRomBank);
  CYC(0x1a85, 0x1a86); push_effect(gb, AF);
  A = B;
  CYC(0x1a86, 0x1a89); H8(hRomBank) = A;
  CYC(0x1a89, 0x1a8c); mem_wr(gb, MBC_ROM_BANK, A);
  B = 0x20;
  CYC(0x1a8c, 0x1a91);
  copyMemory(gb, DE, HL, 0x20);
  SET_HL(HL + 0x20);
  SET_DE(DE + 0x20);
  B = 0;
  CYC(0x1a91, 0x1a92); SET_AF(pop_effect(gb));
  CYC(0x1a92, 0x1a94); H8(hRomBank) = A;
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
  uint16_t sp0_ = gb->sp;
  CYC(0x1de7, 0x1dea);
  CYC(0x1859, 0x185c); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYC(0x185c, 0x185e); SET_AF(0x1dea); CYC(0x185e, 0x1860); ret_effect(gb); return; }
  CYCT(0x185c, 0x185e);
  bank_push(gb, 0x1dea, 0x07);
  CALL_C(0x1df4, checkEnemyAndPartCollisions_hook,
         ROM_b07_checkEnemyAndPartCollisions, 0x1df7);
  bank_pop(gb, 0x1df7);
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

static void update_link_position_given_velocity(GB *gb, uint16_t sp0_) {
  bank_push(gb, 0x231e, 0x05);
  uint16_t de = DE;
  CYC(0x2328, 0x232c); A = W8(wLinkObjectIndex);
  D = A;
  E = 0x00;
  CYC(0x232c, 0x232f);
  CALL_C(0x232f, specialObjectUpdatePositionGivenVelocity_hook,
         ROM_b05_specialObjectUpdatePositionGivenVelocity, 0x2332);
  SET_DE(de);
  CYC(0x2332, 0x2333);
  bank_pop(gb, 0x2333);
  alu_scf(gb);
  CYC(0x2339, 0x233b);
}

void updateLinkPositionGivenVelocity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  update_link_position_given_velocity(gb, sp0_);
  ret_effect(gb);
}

void objectPushLinkAwayOnCollision_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
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
  update_link_position_given_velocity(gb, sp0_);
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

static void interaction_delete(GB *gb);

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

static void next_animation_frame(GB *gb, uint16_t o, uint8_t counter, uint8_t id, uint16_t table, uint8_t oam);

static void interaction_animate(GB *gb) {
  H = D;
  L = 0x60;
  CYC(0x261b, 0x261f); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(0x261f, 0x2620); return; }
  CYC(0x261f, 0x2622); A = H8(hRomBank);
  CYC(0x2622, 0x2623); push_effect(gb, AF);
  CYC(0x2623, 0x2627); H8(hRomBank) = 0x16;
  CYC(0x2627, 0x262a); mem_wr(gb, MBC_ROM_BANK, 0x16);
  L = 0x62;
  CYC(0x262a, 0x262e);
  next_animation_frame(gb, 0x2647, 0x60, 0x41, interactionAnimationFrameTable_bank16, 0x5e);
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
  next_animation_frame(gb, 0x2647, 0x60, 0x41, interactionAnimationFrameTable_bank16, 0x5e);
}

void interactionSetAnimation_hook(GB *gb) { interaction_set_animation(gb); ret_effect(gb); }

static void interaction_animate_2_times(GB *gb) {
  call_interaction_animate(gb, 0x2752);
  CYC(0x2755, 0x2758);
  interaction_animate(gb);
}

static void interaction_animate_3_times(GB *gb) {
  call_interaction_animate(gb, 0x274f);
  interaction_animate_2_times(gb);
}

void interactionAnimate2Times_hook(GB *gb) { interaction_animate_2_times(gb); ret_effect(gb); }
void interactionAnimate3Times_hook(GB *gb) { interaction_animate_3_times(gb); ret_effect(gb); }
void interactionAnimate4Times_hook(GB *gb) {
  call_interaction_animate(gb, 0x274c);
  interaction_animate_3_times(gb);
  ret_effect(gb);
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
  if (F & FC) { CYCT(0x2766, 0x276b); interaction_animate(gb); ret_effect(gb); return; }
  CYC(0x2766, 0x276b);
  alu_cp(gb, 0x78);
  if (F & FC) { CYCT(0x276b, 0x2770); interaction_animate_2_times(gb); ret_effect(gb); return; }
  CYC(0x276b, 0x2770);
  CYC(0x2770, 0x2773);
  interaction_animate_3_times(gb);
  ret_effect(gb);
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
  interaction_delete(gb);
  ret_effect(gb);
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

// floating music note (bank $0b body)

void objectCreateFloatingMusicNote_hook(GB *gb) {
  CYC(0x27fe, 0x2800); H8(hFF8B) = A;
  A = 0x01;
  CYC(0x2800, 0x2804); H8(hFF8D) = A;
  bank_push(gb, 0x2804, 0x0b);
  CALL_ROM(0x280e, ROM_b0b_objectCreateFloatingImage);
  bank_pop(gb, 0x2811);
  CYC(0x2817, 0x2818);
  ret_effect(gb);
}

// enemy and part animation (the transliterated tails pop the return address)

static void enemy_animate(GB *gb) {
  H = D;
  L = 0xa0;
  CYC(0x2818, 0x281c); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(0x281c, 0x281d); return; }
  CYC(0x281c, 0x281f); A = H8(hRomBank);
  CYC(0x281f, 0x2820); push_effect(gb, AF);
  CYC(0x2820, 0x2824); H8(hRomBank) = 0x0d;
  CYC(0x2824, 0x2827); mem_wr(gb, MBC_ROM_BANK, 0x0d);
  L = 0xa2;
  CYC(0x2827, 0x282b);
  next_animation_frame(gb, 0x2844, 0xa0, 0x81, enemyAnimationFrameTable_bank0d, 0x9e);
}

void enemyAnimate_hook(GB *gb) {
  enemy_animate(gb);
  ret_effect(gb);
}

void enemySetAnimation_hook(GB *gb) {
  alu_add(gb, A);
  C = A;
  B = 0x00;
  CYC(0x282b, 0x2831); A = H8(hRomBank);
  CYC(0x2831, 0x2832); push_effect(gb, AF);
  CYC(0x2832, 0x2836); H8(hRomBank) = 0x0d;
  CYC(0x2836, 0x2839); mem_wr(gb, MBC_ROM_BANK, 0x0d);
  E = 0x81;
  CYC(0x2839, 0x283c); A = mem_rd(gb, DE);
  SET_HL(enemyAnimationTable_bank0d);
  CYC(0x283c, 0x2840);
  add_double_index_to_hl(gb);
  CYC(0x2840, 0x2841); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2841, 0x2842); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  CYC(0x2842, 0x2844);
  next_animation_frame(gb, 0x2844, 0xa0, 0x81, enemyAnimationFrameTable_bank0d, 0x9e);
  ret_effect(gb);
}

void partAnimate_hook(GB *gb) {
  H = D;
  L = 0xe0;
  CYC(0x2978, 0x297c); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(0x297c, 0x297d); ret_effect(gb); return; }
  CYC(0x297c, 0x2981); H8(hRomBank) = 0x16;
  CYC(0x2981, 0x2984); mem_wr(gb, MBC_ROM_BANK, 0x16);
  L = 0xe2;
  CYC(0x2984, 0x2988);
  next_animation_frame(gb, 0x299e, 0xe0, 0xc1, partAnimationFrameTable_bank16, 0xde);
  ret_effect(gb);
}

void partSetAnimation_hook(GB *gb) {
  alu_add(gb, A);
  C = A;
  B = 0x00;
  A = 0x16;
  CYC(0x2988, 0x2990); H8(hRomBank) = A;
  CYC(0x2990, 0x2993); mem_wr(gb, MBC_ROM_BANK, A);
  E = 0xc1;
  CYC(0x2993, 0x2996); A = mem_rd(gb, DE);
  SET_HL(partAnimationTable_bank16);
  CYC(0x2996, 0x299a);
  add_double_index_to_hl(gb);
  CYC(0x299a, 0x299b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x299b, 0x299c); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  CYC(0x299c, 0x299e);
  next_animation_frame(gb, 0x299e, 0xe0, 0xc1, partAnimationFrameTable_bank16, 0xde);
  ret_effect(gb);
}

// enemy death

static void enemy_delete(GB *gb) {
  E = 0x80;
  CYC(0x2e47, 0x2e4c);
  remove_from_abutton_list(gb);
  L = E;
  H = D;
  B = 0x10;
  alu_xor(gb, A);
  CYC(0x2e4c, 0x2e51);
  do {
    CYC(0x2e51, 0x2e52); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x2e52, 0x2e53); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x2e53, 0x2e54); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x2e54, 0x2e55); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x2e55, 0x2e58); else CYC(0x2e55, 0x2e58);
  } while (B);
  CYC(0x2e58, 0x2e59);
}

void enemyDelete_hook(GB *gb) {
  enemy_delete(gb);
  ret_effect(gb);
}

static void enemy_create_death_puff(GB *gb) {
  E = 0xbf;
  CYC(0x28d3, 0x28d6); A = mem_rd(gb, DE);
  alu_rlca(gb);
  if (F & FC) { CYCT(0x28d6, 0x28da); dec_num_enemies(gb); pop_effect(gb); return; }
  CYC(0x28d6, 0x28da);
  CALL_ROM(0x28da, ROM_getFreePartSlot);
  if (!(F & FZ)) { CYCT(0x28dd, 0x28de); pop_effect(gb); return; }
  CYC(0x28dd, 0x28de);
  E = 0x80;
  CYC(0x28de, 0x28e1); A = mem_rd(gb, DE);
  alu_and(gb, 0x03);
  L = alu_dec8(gb, L);
  CYC(0x28e1, 0x28e5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x28e5, 0x28e7); mem_wr(gb, HL, 0x02);
  L = alu_inc8(gb, L);
  E = 0x81;
  CYC(0x28e7, 0x28eb); A = mem_rd(gb, DE);
  CYC(0x28eb, 0x28ec); mem_wr(gb, HL, A);
  L = 0xed;
  E = 0xad;
  CYC(0x28ec, 0x28f1); A = mem_rd(gb, DE);
  CYC(0x28f1, 0x28f2); mem_wr(gb, HL, A);
  CYC(0x28f2, 0x28f5);
  object_copy_position(gb);
  L = 0xc7;
  CYC(0x28f5, 0x28f8); mem_wr(gb, HL, B);
  A = 0x73;
  CYC(0x28f8, 0x28fd);
  asm_call(gb, ROM_playSound, 0x288c);
}

static void enemy_die_common(GB *gb) {
  CYC(0x2889, 0x288c);
  push_effect(gb, 0x288c);
  enemy_create_death_puff(gb);
  alu_bit(gb, 0, B);
  if (!(F & FZ)) { CYCT(0x288e, 0x2891); push_effect(gb, 0x2891); asm_call(gb, ROM_markEnemyAsKilledInRoom, 0x2891); }
  else CYC(0x288e, 0x2891);
  A = 0x00;
  CYC(0x2891, 0x2893);
  CALL_ROM(0x2893, ROM_checkGlobalFlag);
  if (!(F & FZ)) CYCT(0x2896, 0x2898);
  else {
    CYC(0x2896, 0x2898);
    L = 0x20;
    CYC(0x2898, 0x289d);
    inc_hl_ref16_with_cap(gb);
    CYC(0x289d, 0x289e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x289e, 0x289f); H = mem_rd(gb, HL);
    L = A;
    SET_BC(0x03e8);
    CYC(0x289f, 0x28a6);
    compare_hl_to_bc(gb);
    alu_rlca(gb);
    A = 0x00;
    CYC(0x28a6, 0x28a9);
    if (!(F & FC)) { CYCT(0x28a9, 0x28ac); push_effect(gb, 0x28ac); asm_call(gb, ROM_setGlobalFlag, 0x28ac); }
    else CYC(0x28a9, 0x28ac);
  }
  SET_HL(wMapleKillCounter);
  CYC(0x28ac, 0x28b2);
  inc_hl_ref_with_cap(gb);
  A = 0x3a;
  CYC(0x28b2, 0x28b7);
  cp_active_ring(gb);
  A = 0xff;
  if (F & FZ) CYCT(0x28b7, 0x28bb);
  else { CYC(0x28b7, 0x28bc); alu_xor(gb, A); }
  L = 0x4f;
  C = 0x10;
  CYC(0x28bc, 0x28c0);
  do {
    alu_rlca(gb);
    if (F & FC) { CYCT(0x28c0, 0x28c4); inc_hl_ref_with_cap(gb); }
    else CYC(0x28c0, 0x28c4);
    CYC(0x28c4, 0x28c7);
    inc_hl_ref_with_cap(gb);
    L = alu_inc8(gb, L);
    C = alu_dec8(gb, C);
    if (C) CYCT(0x28c7, 0x28cb); else CYC(0x28c7, 0x28cb);
  } while (C);
  A = 0x03;
  CYC(0x28cb, 0x28d0);
  add_to_gasha_maturity(gb);
  CYC(0x28d0, 0x28d3);
  enemy_delete(gb);
}

void enemyDie_hook(GB *gb) { B = 0x01; CYC(0x2887, 0x2889); enemy_die_common(gb); ret_effect(gb); }
void enemyDie_uncounted_hook(GB *gb) { B = 0x00; CYC(0x2883, 0x2887); enemy_die_common(gb); ret_effect(gb); }
void enemyDie_withoutItemDrop_hook(GB *gb) { B = 0x81; CYC(0x287f, 0x2883); enemy_die_common(gb); ret_effect(gb); }
void enemyDie_uncounted_withoutItemDrop_hook(GB *gb) { B = 0x80; CYC(0x287b, 0x287f); enemy_die_common(gb); ret_effect(gb); }

// energy swirls (bank $11 bodies)

void createEnergySwirlGoingIn_hook(GB *gb) {
  L = A;
  CYC(0x29d8, 0x29d9);
  bank_push(gb, 0x29d9, 0x11);
  CALL_ROM(0x29e3, ROM_b11_createEnergySwirlGoingIn_body);
  bank_pop(gb, 0x29e6);
  CYC(0x29ec, 0x29ed);
  ret_effect(gb);
}

void createEnergySwirlGoingOut_hook(GB *gb) {
  L = A;
  CYC(0x29ed, 0x29ee);
  bank_push(gb, 0x29ee, 0x11);
  CALL_ROM(0x29f8, ROM_b11_createEnergySwirlGoingOut_body);
  bank_pop(gb, 0x29fb);
  CYC(0x2a01, 0x2a02);
  ret_effect(gb);
}

// Link input and state

void convertLinkAngleToDirectionButtons_hook(GB *gb) {
  CYC(0x2a02, 0x2a05); A = W8(wLinkAngle);
  alu_add(gb, A);
  if (F & FC) { CYCT(0x2a05, 0x2a08); alu_xor(gb, A); CYC(0x2a13, 0x2a15); ret_effect(gb); return; }
  CYC(0x2a05, 0x2a08);
  alu_add(gb, A);
  alu_swap_a(gb);
  uint16_t hl = HL;
  SET_HL(linkAngleToDirectionButtons);
  CYC(0x2a08, 0x2a10);
  add_a_to_hl(gb);
  CYC(0x2a10, 0x2a11); A = mem_rd(gb, HL);
  SET_HL(hl);
  CYC(0x2a11, 0x2a13);
  ret_effect(gb);
}

static void clear_pegasus_seed_counter(GB *gb) {
  SET_HL(wPegasusSeedCounter);
  alu_xor(gb, A);
  CYC(0x2a85, 0x2a8a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2a8a, 0x2a8b); mem_wr(gb, HL, A);
  CYC(0x2a8b, 0x2a8c);
}

void clearPegasusSeedCounter_hook(GB *gb) {
  clear_pegasus_seed_counter(gb);
  ret_effect(gb);
}

void setSimulatedInputAddress_hook(GB *gb) {
  SET_DE(wSimulatedInputBank);
  CYC(0x2a1d, 0x2a21); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = L;
  CYC(0x2a21, 0x2a24); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(0x2a24, 0x2a27); mem_wr(gb, DE, A);
  E = 0xc5;
  alu_xor(gb, A);
  CYC(0x2a27, 0x2a2b); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(0x2a2b, 0x2a2d); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  A = alu_inc8(gb, A);
  CYC(0x2a2d, 0x2a30); mem_wr(gb, DE, A);
  CYC(0x2a30, 0x2a33);
  clear_pegasus_seed_counter(gb);
  ret_effect(gb);
}

void getSimulatedInput_hook(GB *gb) {
  CYC(0x2a33, 0x2a36); A = W8(wPaletteThread_mode);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x2a36, 0x2a38); ret_effect(gb); return; }
  CYC(0x2a36, 0x2a3b); A = W8(wUseSimulatedInput);
  alu_rlca(gb);
  if (F & FC) CYCT(0x2a3b, 0x2a3e);
  else {
    CYC(0x2a3b, 0x2a3e);
    SET_HL(wSimulatedInputCounter);
    CYC(0x2a3e, 0x2a44);
    dec16_cap(gb, HL);
    if (!(F & FZ)) CYCT(0x2a44, 0x2a46);
    else {
      CYC(0x2a44, 0x2a48); A = H8(hRomBank);
      CYC(0x2a48, 0x2a49); push_effect(gb, AF);
      SET_HL(wSimulatedInputBank);
      CYC(0x2a49, 0x2a4d); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x2a4d, 0x2a4f); H8(hRomBank) = A;
      CYC(0x2a4f, 0x2a52); mem_wr(gb, MBC_ROM_BANK, A);
      CYC(0x2a52, 0x2a53); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x2a53, 0x2a54); H = mem_rd(gb, HL);
      L = A;
      CYC(0x2a54, 0x2a56); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x2a56, 0x2a59); W8(wSimulatedInputCounter) = A;
      CYC(0x2a59, 0x2a5a); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x2a5a, 0x2a5d); mem_wr(gb, wSimulatedInputCounter + 1, A);
      alu_bit(gb, 7, A);
      if (F & FZ) {
        CYCT(0x2a5d, 0x2a61);
        CYC(0x2a68, 0x2a69); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x2a69, 0x2a6c); W8(wSimulatedInputValue) = A;
      } else {
        CYC(0x2a5d, 0x2a61);
        A = 0xff;
        CYC(0x2a61, 0x2a66); W8(wUseSimulatedInput) = A;
        CYC(0x2a66, 0x2a68);
      }
      CYC(0x2a6c, 0x2a6d); SET_AF(pop_effect(gb));
      CYC(0x2a6d, 0x2a6f); H8(hRomBank) = A;
      CYC(0x2a6f, 0x2a72); mem_wr(gb, MBC_ROM_BANK, A);
      A = L;
      CYC(0x2a72, 0x2a76); W8(wSimulatedInputAddressL) = A;
      A = H;
      CYC(0x2a76, 0x2a7a); W8(wSimulatedInputAddressH) = A;
    }
  }
  CYC(0x2a7a, 0x2a7d); A = W8(wSimulatedInputValue);
  CYC(0x2a7d, 0x2a7e);
  ret_effect(gb);
}

void itemSetState_hook(GB *gb) {
  H = D;
  L = OBJ_STATE;
  CYC(0x2a7e, 0x2a82); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2a82, 0x2a84); mem_wr(gb, HL, 0x00);
  CYC(0x2a84, 0x2a85);
  ret_effect(gb);
}

void special_object_set_animation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  E = 0x30;
  CYC(0x2b0a, 0x2b0d); mem_wr(gb, DE, A);
  alu_add(gb, A);
  C = A;
  B = 0x00;
  CYC(0x2b0d, 0x2b11);
  bank_push(gb, 0x2b11, 0x06);
  CALL_C(0x2b1b, specialObjectSetAnimation_body_hook, 0x4427, 0x2b1e);
  bank_pop(gb, 0x2b1e);
  CYC(0x2b24, 0x2b25);
}

void specialObjectSetAnimation_hook(GB *gb) {
  special_object_set_animation_hook(gb);
  ret_effect(gb);
}

void specialObjectAnimate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  H = D;
  L = 0x20;
  CYC(0x2aef, 0x2af3); uint8_t v = alu_dec8(gb, mem_rd(gb, HL)); mem_wr(gb, HL, v);
  if (v) { CYCT(0x2af3, 0x2af4); ret_effect(gb); return; }
  CYC(0x2af3, 0x2af4);
  bank_push(gb, 0x2af4, 0x06);
  L = 0x22;
  CYC(0x2afe, 0x2b00);
  CALL_C(0x2b00, specialObjectNextAnimationFrame_hook, 0x4432, 0x2b03);
  bank_pop(gb, 0x2b03);
  CYC(0x2b09, 0x2b0a);
  ret_effect(gb);
}

void putLinkOnGround_hook(GB *gb) {
  CYC(0x2a8c, 0x2a8f); A = W8(wLinkObjectIndex);
  alu_rrca(gb);
  if (F & FC) { CYCT(0x2a8f, 0x2a91); ret_effect(gb); return; }
  CYC(0x2a8f, 0x2a91);
  uint16_t de = DE;
  alu_xor(gb, A);
  CYC(0x2a91, 0x2a96); W8(wLinkInAir) = A;
  SET_HL(w1Link_speedZ);
  CYC(0x2a96, 0x2a9a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2a9a, 0x2a9b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = OBJ_Z;
  CYC(0x2a9b, 0x2a9e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2a9e, 0x2a9f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = 0x01;
  CYC(0x2a9f, 0x2aa2); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(0x2aa2, 0x2aa5);
  else {
    CYC(0x2aa2, 0x2aa5);
    D = H;
    A = 0x10;
    CYC(0x2aa5, 0x2aab);
    special_object_set_animation_hook(gb);
  }
  SET_DE(de);
  CYC(0x2aab, 0x2aad);
  ret_effect(gb);
}

static void set_link_force_state_08(GB *gb) {
  uint16_t hl = HL;
  SET_HL(wcc50);
  CYC(0x2aae, 0x2ab3); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x2ab3, 0x2ab5); mem_wr(gb, HL, 0x08);
  SET_HL(hl);
  CYC(0x2ab5, 0x2ab7);
}

void setLinkForceStateToState08_withParam_hook(GB *gb) {
  set_link_force_state_08(gb);
  ret_effect(gb);
}

void setLinkForceStateToState08_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x2aad, 0x2aae);
  set_link_force_state_08(gb);
  ret_effect(gb);
}

static void clear_link_state_bytes(GB *gb) {
  alu_xor(gb, A);
  CYC(0x2add, 0x2adf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2adf, 0x2ae0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2ae0, 0x2ae1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2ae1, 0x2ae2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2ae2, 0x2ae3);
}

void setLinkIDOverride_hook(GB *gb) {
  alu_or(gb, 0x80);
  CYC(0x2acf, 0x2ad4); W8(wLinkIDOverride) = A;
  SET_HL(w1Link_subid);
  CYC(0x2ad4, 0x2ad9);
  clear_link_state_bytes(gb);
  ret_effect(gb);
}

void setLinkID_hook(GB *gb) {
  SET_HL(w1Link_id);
  CYC(0x2ad9, 0x2add); mem_wr(gb, HL, A); SET_HL(HL + 1);
  clear_link_state_bytes(gb);
  ret_effect(gb);
}

void respawnLink_hook(GB *gb) {
  A = 0x02;
  CYC(0x2ae3, 0x2ae8); W8(wLinkForceState) = A;
  A = 0x02;
  CYC(0x2ae8, 0x2aed); W8(wLinkStateParameter) = A;
  alu_or(gb, D);
  CYC(0x2aed, 0x2aef);
  ret_effect(gb);
}

void checkLinkPushingAgainstWall_hook(GB *gb) {
  uint16_t hl = HL;
  CYC(0x2b39, 0x2b3d); A = mem_rd(gb, w1Link_direction);
  SET_HL(wallCollisionDirections);
  CYC(0x2b3d, 0x2b41);
  add_double_index_to_hl(gb);
  CYC(0x2b41, 0x2b44); A = mem_rd(gb, w1Link_adjacentWallsBitset);
  CYC(0x2b44, 0x2b45); alu_and(gb, mem_rd(gb, HL));
  CYC(0x2b45, 0x2b46); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    SET_HL(HL + 1);
    CYC(0x2b46, 0x2b4c); A = W8(wGameKeysPressed);
    CYC(0x2b4c, 0x2b4d); alu_and(gb, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYC(0x2b4d, 0x2b4f);
      SET_HL(hl);
      alu_scf(gb);
      CYC(0x2b4f, 0x2b52);
      ret_effect(gb);
      return;
    }
    CYCT(0x2b4d, 0x2b4f);
  } else CYCT(0x2b46, 0x2b48);
  SET_HL(hl);
  alu_xor(gb, A);
  CYC(0x2b52, 0x2b55);
  ret_effect(gb);
}

static void direction_from_angle(GB *gb) {
  CYC(0x2b69, 0x2b6a); B = mem_rd(gb, HL);
  CYC(0x2b6a, 0x2b6d); A = W8(wLinkAngle);
  alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x2b6d, 0x2b71); return; }
  CYC(0x2b6d, 0x2b71);
  alu_and(gb, 0x1c);
  alu_rrca(gb);
  alu_rrca(gb);
  alu_rra(gb);
  if (!(F & FC)) CYCT(0x2b71, 0x2b78);
  else {
    CYC(0x2b71, 0x2b78);
    C = A;
    alu_sub(gb, B);
    A = alu_inc8(gb, A);
    alu_and(gb, 0x02);
    if (F & FZ) { CYCT(0x2b78, 0x2b7f); return; }
    CYC(0x2b78, 0x2b80);
    A = C;
  }
  CYC(0x2b80, 0x2b81); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x2b81, 0x2b83); return; }
  CYC(0x2b81, 0x2b84); mem_wr(gb, HL, A);
  B = A;
  alu_scf(gb);
  CYC(0x2b84, 0x2b86);
}

static void update_direction_from_angle(GB *gb, uint16_t from, uint16_t target) {
  uint16_t bc = BC, hl = HL;
  SET_HL(target);
  CYC(from, from == 0x2b5d ? 0x2b64 : 0x2b69);
  direction_from_angle(gb);
  A = B;
  SET_HL(hl);
  SET_BC(bc);
  CYC(0x2b86, 0x2b8a);
}

void updateLinkDirectionFromAngle_hook(GB *gb) {
  update_direction_from_angle(gb, 0x2b64, w1Link_direction);
  ret_effect(gb);
}

void updateCompanionDirectionFromAngle_hook(GB *gb) {
  update_direction_from_angle(gb, 0x2b5d, w1Companion_id + 0x07);
  ret_effect(gb);
}

void specialObjectSetCoordinatesToRespawnYX_hook(GB *gb) {
  H = D;
  L = 0x08;
  CYC(0x2b8a, 0x2b90); A = W8(wLinkLocalRespawnDir);
  CYC(0x2b90, 0x2b91); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0xff;
  CYC(0x2b91, 0x2b94); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2b94, 0x2b97); W8(wLinkPathIndex) = A;
  L = OBJ_YH;
  CYC(0x2b97, 0x2b9c); A = W8(wLinkLocalRespawnY);
  CYC(0x2b9c, 0x2b9d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(0x2b9d, 0x2ba1); A = W8(wLinkLocalRespawnX);
  CYC(0x2ba1, 0x2ba2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(0x2ba2, 0x2ba4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2ba4, 0x2ba5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = 0x2d;
  CYC(0x2ba5, 0x2ba8); mem_wr(gb, HL, A);
  CYC(0x2ba8, 0x2ba9);
  ret_effect(gb);
}

void resetLinkInvincibility_hook(GB *gb) {
  SET_HL(w1Link_oamFlagsBackup);
  CYC(0x2ba9, 0x2bad); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2bad, 0x2bae); mem_wr(gb, HL, A);
  L = 0x24;
  alu_xor(gb, A);
  CYC(0x2bae, 0x2bb2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2bb2, 0x2bb3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = 0x28;
  CYC(0x2bb3, 0x2bb6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(0x2bb6, 0x2bb8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2bb8, 0x2bb9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2bb9, 0x2bba); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2bba, 0x2bbb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2bbb, 0x2bbc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2bbc, 0x2bbd);
  ret_effect(gb);
}

void decPegasusSeedCounter_hook(GB *gb) {
  SET_HL(wPegasusSeedCounter + 1);
  CYC(0x2bbd, 0x2bc2); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  L = alu_dec8(gb, L);
  B = 0x00;
  C = 0x07;
  A = 0x11;
  CYC(0x2bc2, 0x2bcc);
  cp_active_ring(gb);
  if (F & FZ) CYCT(0x2bcc, 0x2bce);
  else {
    CYC(0x2bcc, 0x2bce);
    C = 0x0f;
    CYC(0x2bce, 0x2bd3);
    dec16_cap(gb, HL);
    if (F & FZ) { CYCT(0x2bd3, 0x2bd4); ret_effect(gb); return; }
    CYC(0x2bd3, 0x2bd5); A = mem_rd(gb, HL);
    alu_and(gb, C);
    if (!(F & FZ)) CYCT(0x2bd5, 0x2bd8);
    else { CYC(0x2bd5, 0x2bda); B = 0x80; }
  }
  CYC(0x2bda, 0x2bdd);
  dec16_cap(gb, HL);
  if (F & FZ) { CYCT(0x2bdd, 0x2bde); ret_effect(gb); return; }
  CYC(0x2bdd, 0x2bdf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_and(gb, C);
  if (!(F & FZ)) CYCT(0x2bdf, 0x2be2);
  else { CYC(0x2bdf, 0x2be4); B = 0x80; }
  CYC(0x2be4, 0x2be5); A = mem_rd(gb, HL);
  alu_or(gb, B);
  CYC(0x2be5, 0x2be7); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x2be7, 0x2be8);
  ret_effect(gb);
}

void checkPegasusSeedCounter_hook(GB *gb) {
  SET_HL(wPegasusSeedCounter);
  CYC(0x2be8, 0x2bec); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2bec, 0x2bed); alu_or(gb, mem_rd(gb, HL));
  CYC(0x2bed, 0x2bee); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x2bee, 0x2bef);
  ret_effect(gb);
}

// items

void itemDelete_hook(GB *gb) {
  H = D;
  L = 0x00;
  B = 0x10;
  alu_xor(gb, A);
  CYC(0x2ce2, 0x2ce8);
  do {
    CYC(0x2ce8, 0x2ce9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x2ce9, 0x2cea); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x2cea, 0x2ceb); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x2ceb, 0x2cec); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x2cec, 0x2cef); else CYC(0x2cec, 0x2cef);
  } while (B);
  CYC(0x2cef, 0x2cf0);
  ret_effect(gb);
}

void itemUpdateAngle_hook(GB *gb) {
  H = D;
  L = 0x08;
  CYC(0x2cf0, 0x2cf4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_swap_a(gb);
  alu_rrca(gb);
  CYC(0x2cf4, 0x2cf8); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x2cf8, 0x2cf9);
  ret_effect(gb);
}

void getFreeItemSlot_hook(GB *gb) {
  SET_HL(w1WeaponItem + 0x100);
  CYC(0x2cf9, 0x2cfc);
  for (;;) {
    CYC(0x2cfc, 0x2cfd); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) { CYCT(0x2cfd, 0x2cff); break; }
    CYC(0x2cfd, 0x2cff);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xdc);
    if (F & FC) { CYCT(0x2cff, 0x2d05); continue; }
    CYC(0x2cff, 0x2d05);
    alu_or(gb, H);
    CYC(0x2d05, 0x2d07);
    break;
  }
  ret_effect(gb);
}

// dungeons

void setVisitedRoomFlag_hook(GB *gb) {
  CYC(0x2dd1, 0x2dd4);
  this_room_flags(gb);
  CYC(0x2dd4, 0x2dd6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x2dd6, 0x2dd7);
  ret_effect(gb);
}

void getThisRoomDungeonProperties_hook(GB *gb) {
  bank_push(gb, 0x2dd7, 0x01);
  CYC(0x2de1, 0x2de4); A = W8(wActiveGroup);
  alu_sub(gb, 0x04);
  alu_and(gb, 0x01);
  SET_HL(dungeonRoomPropertiesGroupTable_bank01);
  CYC(0x2de4, 0x2dec);
  add_double_index_to_hl(gb);
  CYC(0x2dec, 0x2ded); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2ded, 0x2dee); H = mem_rd(gb, HL);
  L = A;
  CYC(0x2dee, 0x2df2); A = W8(wActiveRoom);
  B = 0x00;
  C = A;
  alu_add_hl(gb, BC);
  CYC(0x2df2, 0x2df7); A = mem_rd(gb, HL);
  CYC(0x2df7, 0x2dfa); W8(wDungeonRoomProperties) = A;
  bank_pop(gb, 0x2dfa);
  CYC(0x2e00, 0x2e01);
  ret_effect(gb);
}

static void dungeon_layout_address(GB *gb) {
  uint16_t bc = BC, de = DE;
  CYC(0x2e01, 0x2e06); A = W8(wDungeonFloor);
  C = 0x40;
  CYC(0x2e06, 0x2e0b);
  multiply_a_by_c(gb);
  SET_BC(w2DungeonLayout);
  alu_add_hl(gb, BC);
  SET_DE(de);
  SET_BC(bc);
  CYC(0x2e0b, 0x2e12);
}

void getDungeonLayoutAddress_hook(GB *gb) {
  dungeon_layout_address(gb);
  ret_effect(gb);
}

static void room_in_dungeon(GB *gb) {
  CYC(0x2e15, 0x2e17); H8(hFF8B) = A;
  A = 0x02;
  CYC(0x2e17, 0x2e1b); mem_wr(gb, IO_SVBK, A);
  CYC(0x2e1b, 0x2e1e);
  dungeon_layout_address(gb);
  CYC(0x2e1e, 0x2e20); A = H8(hFF8B);
  CYC(0x2e20, 0x2e21);
  add_a_to_hl(gb);
  CYC(0x2e21, 0x2e22); L = mem_rd(gb, HL);
  alu_xor(gb, A);
  CYC(0x2e22, 0x2e25); mem_wr(gb, IO_SVBK, A);
  A = L;
  CYC(0x2e25, 0x2e27);
}

void getRoomInDungeon_hook(GB *gb) {
  room_in_dungeon(gb);
  ret_effect(gb);
}

void getActiveRoomFromDungeonMapPosition_hook(GB *gb) {
  CYC(0x2e12, 0x2e15); A = W8(wDungeonMapPosition);
  room_in_dungeon(gb);
  ret_effect(gb);
}

static void free_enemy_slot_uncounted(GB *gb) {
  SET_HL(ENEMY_SLOTS);
  CYC(0x2e34, 0x2e37);
  for (;;) {
    CYC(0x2e37, 0x2e38); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x2e38, 0x2e3b);
      A = alu_inc8(gb, A);
      CYC(0x2e43, 0x2e45); mem_wr(gb, HL, A); SET_HL(HL + 1);
      alu_xor(gb, A);
      CYC(0x2e45, 0x2e47);
      return;
    }
    CYC(0x2e38, 0x2e3b);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x2e3b, 0x2e41); continue; }
    CYC(0x2e3b, 0x2e41);
    alu_or(gb, H);
    CYC(0x2e41, 0x2e43);
    return;
  }
}

void getFreeEnemySlot_uncounted_hook(GB *gb) {
  free_enemy_slot_uncounted(gb);
  ret_effect(gb);
}

void getFreeEnemySlot_hook(GB *gb) {
  CYC(0x2e27, 0x2e2a);
  free_enemy_slot_uncounted(gb);
  if (!(F & FZ)) { CYCT(0x2e2a, 0x2e2b); ret_effect(gb); return; }
  CYC(0x2e2a, 0x2e2e); A = W8(wNumEnemies);
  A = alu_inc8(gb, A);
  CYC(0x2e2e, 0x2e32); W8(wNumEnemies) = A;
  alu_xor(gb, A);
  CYC(0x2e32, 0x2e34);
  ret_effect(gb);
}

void enemyReplaceWithID_hook(GB *gb) {
  H = D;
  uint16_t bc0 = BC;
  L = 0x80;
  CYC(0x2e59, 0x2e5e); B = mem_rd(gb, HL);
  L = 0x8b;
  CYC(0x2e5e, 0x2e61); C = mem_rd(gb, HL);
  uint16_t bc1 = BC;
  L = 0x8d;
  CYC(0x2e61, 0x2e65); B = mem_rd(gb, HL);
  L = 0x8f;
  CYC(0x2e65, 0x2e68); C = mem_rd(gb, HL);
  uint16_t bc2 = BC;
  CYC(0x2e68, 0x2e6c);
  enemy_delete(gb);
  SET_BC(bc2);
  L = 0x8f;
  CYC(0x2e6c, 0x2e70); mem_wr(gb, HL, C);
  L = 0x8d;
  CYC(0x2e70, 0x2e73); mem_wr(gb, HL, B);
  SET_BC(bc1);
  L = 0x8b;
  CYC(0x2e73, 0x2e77); mem_wr(gb, HL, C);
  L = 0x80;
  A = B;
  alu_and(gb, 0x73);
  CYC(0x2e77, 0x2e7d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  SET_BC(bc0);
  CYC(0x2e7d, 0x2e7f); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x2e7f, 0x2e81); mem_wr(gb, HL, C);
  CYC(0x2e81, 0x2e82);
  ret_effect(gb);
}

void findFreeStaticObjectSlot_hook(GB *gb) {
  SET_HL(wStaticObjects);
  B = 0x08;
  CYC(0x31a7, 0x31ac);
  for (;;) {
    CYC(0x31ac, 0x31ad); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) { CYCT(0x31ad, 0x31af); break; }
    CYC(0x31ad, 0x31af);
    A = 0x08;
    alu_add(gb, L);
    L = A;
    B = alu_dec8(gb, B);
    if (B) { CYCT(0x31af, 0x31b6); continue; }
    CYC(0x31af, 0x31b6);
    alu_or(gb, H);
    CYC(0x31b6, 0x31b8);
    break;
  }
  ret_effect(gb);
}

// interaction extras

void interactionLoadExtraGraphics_hook(GB *gb) {
  E = INTERACTION_BASE + 0x01;
  CYC(0x2781, 0x2784); A = mem_rd(gb, DE);
  CYC(0x2784, 0x2787); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(0x2787, 0x278a); W8(wLoadedObjectGfxEnd) = A;
  CYC(0x278a, 0x278b);
  ret_effect(gb);
}

void interactionGetMiniScript_hook(GB *gb) {
  H = D;
  L = 0x58;
  CYC(0x2791, 0x2795); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2795, 0x2796); H = mem_rd(gb, HL);
  L = A;
  CYC(0x2796, 0x2798);
  ret_effect(gb);
}

// palettes and dungeon setup (bank 3 and bank 1 bodies)

void clearFadingPalettes_hook(GB *gb) {
  bank_push(gb, 0x2d5f, 0x03);
  CALL_ROM(0x2d69, ROM_b03_clearFadingPalettes_body);
  bank_pop(gb, 0x2d6c);
  CYC(0x2d72, 0x2d73);
  ret_effect(gb);
}

void flashScreen_hook(GB *gb) {
  bank_push(gb, 0x2d73, 0x03);
  CALL_ROM(0x2d7d, ROM_b03_flashScreen_body);
  B = 0x01;
  if (!(F & FZ)) CYCT(0x2d80, 0x2d84);
  else { CYC(0x2d80, 0x2d85); B = alu_dec8(gb, B); }
  bank_pop(gb, 0x2d85);
  A = B;
  alu_or(gb, A);
  CYC(0x2d8b, 0x2d8e);
  ret_effect(gb);
}

void loadDungeonLayout_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x2daa, 0x2dad); A = W8(wTilesetFlags);
  alu_and(gb, 0x08);
  if (F & FZ) { CYCT(0x2dad, 0x2db0); ret_effect(gb); return; }
  CYC(0x2dad, 0x2db0);
  bank_push(gb, 0x2db0, 0x01);
  CALL_C(0x2dba, loadDungeonLayout_b01_hook, 0x564e, 0x2dbd);
  bank_pop(gb, 0x2dbd);
  CYC(0x2dc3, 0x2dc4);
  ret_effect(gb);
}

void initializeDungeonStuff_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x2dc4, 0x2dc8); W8(wToggleBlocksState) = A;
  CYC(0x2dc8, 0x2dcb); W8(wSwitchState) = A;
  CYC(0x2dcb, 0x2dce); W8(wSpinnerState) = A;
  CYC(0x2dce, 0x2dd1);
  loadStaticObjects_hook(gb);
}

void clearStaticObjects_hook(GB *gb) {
  SET_HL(wStaticObjects);
  B = 0x40;
  CYC(0x319f, 0x31a7);
  clearMemory(gb, HL, 0x40);
  SET_HL(HL + 0x40);
  B = 0;
  A = 0;
  F = FZ | FN;
  ret_effect(gb);
}

void objectDeleteRelatedObj1AsStaticObject_hook(GB *gb) {
  CYC(0x31b8, 0x31ba); A = H8(hActiveObjectType);
  alu_add(gb, OBJ_RELATED1);
  L = A;
  H = D;
  CYC(0x31ba, 0x31bf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x31bf, 0x31c0); H = mem_rd(gb, HL);
  E = L;
  L = A;
  alu_or(gb, H);
  CYC(0x31c0, 0x31c3);
  if (F & FZ) { CYCT(0x31c3, 0x31c4); ret_effect(gb); return; }
  CYC(0x31c3, 0x31c4);
  alu_xor(gb, A);
  CYC(0x31c4, 0x31c6); mem_wr(gb, DE, A);
  E = alu_dec8(gb, E);
  CYC(0x31c6, 0x31c8); mem_wr(gb, DE, A);
  E = 0x08;
  CYC(0x31c8, 0x31ca);
  do {
    CYC(0x31ca, 0x31cb); mem_wr(gb, HL, A); SET_HL(HL + 1);
    E = alu_dec8(gb, E);
    if (E) CYCT(0x31cb, 0x31ce); else CYC(0x31cb, 0x31ce);
  } while (E);
  CYC(0x31ce, 0x31cf);
  ret_effect(gb);
}

// parent items (bank 6 bodies)

static void bank6_function_caller(GB *gb, uint16_t sp0_) {
  bank_push(gb, 0x2c1a, 0x06);
  CALL_C(0x2c24, functionCaller_b06_hook, 0x4870, 0x2c27);
  bank_pop(gb, 0x2c27);
  CYC(0x2c2d, 0x2c2e);
}

void clearAllParentItems_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  C = 0x00;
  CYC(0x2c10, 0x2c14);
  bank6_function_caller(gb, sp0_);
  ret_effect(gb);
}

void updateParentItemButtonAssignment_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  C = 0x01;
  CYC(0x2c14, 0x2c18);
  bank6_function_caller(gb, sp0_);
  ret_effect(gb);
}

void clearVar3fForParentItems_hook(GB *gb) {
  SET_HL(w1ParentItem2_var3f);
  CYC(0x2c72, 0x2c75);
  for (;;) {
    CYC(0x2c75, 0x2c77); mem_wr(gb, HL, 0x00);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xd6);
    if (F & FC) { CYCT(0x2c77, 0x2c7d); continue; }
    CYC(0x2c77, 0x2c7e);
    break;
  }
  ret_effect(gb);
}

void linkCreateSplash_hook(GB *gb) {
  B = 0x03;
  CYC(0x2c7e, 0x2c83); A = W8(wLinkSwimmingState);
  alu_bit(gb, 6, A);
  if (F & FZ) CYCT(0x2c83, 0x2c87);
  else { CYC(0x2c83, 0x2c88); B = alu_inc8(gb, B); }
  CYC(0x2c88, 0x2c8b); A = W8(wTilesetFlags);
  alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x2c8b, 0x2c90);
    C = 0x00;
    CYC(0x24c3, 0x24c5);
    object_create_interaction(gb);
    ret_effect(gb);
    return;
  }
  CYC(0x2c8b, 0x2c90);
  CALL_ROM(0x2c90, ROM_getFreeInteractionSlot);
  if (!(F & FZ)) { CYCT(0x2c93, 0x2c94); ret_effect(gb); return; }
  CYC(0x2c93, 0x2c95); mem_wr(gb, HL, B);
  SET_BC(0xfd00);
  CYC(0x2c95, 0x2c9b);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void clearVariousLinkVariables_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x2c9b, 0x2c9f); mem_wr(gb, w1Link_var36, A);
  CYC(0x2c9f, 0x2ca2); mem_wr(gb, w1Link_speed, A);
  CYC(0x2ca2, 0x2ca5); mem_wr(gb, w1Link_var3e, A);
  CYC(0x2ca5, 0x2ca8); mem_wr(gb, w1Link_var12, A);
  A = alu_dec8(gb, A);
  CYC(0x2ca8, 0x2cac); mem_wr(gb, w1Link_angle, A);
  CYC(0x2cac, 0x2cad);
  ret_effect(gb);
}

void dropLinkHeldItem_hook(GB *gb) {
  CYC(0x2c43, 0x2c46); A = W8(wAButtonSensitiveObjectListEnd);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(0x2c46, 0x2c49);
  else {
    CYC(0x2c46, 0x2c4c); A = W8(wLinkGrabState);
    alu_and(gb, 0x07);
    alu_sub(gb, 0x02);
    alu_cp(gb, 0x02);
    if (!(F & FC)) CYCT(0x2c4c, 0x2c54);
    else {
      CYC(0x2c4c, 0x2c57);
      SET_HL(w1Link_relatedObj2);
      CYC(0x2c57, 0x2c58); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x2c58, 0x2c59); H = mem_rd(gb, HL);
      alu_add(gb, 0x04);
      L = A;
      CYC(0x2c59, 0x2c5d); A = mem_rd(gb, HL); SET_HL(HL + 1);
      alu_cp(gb, 0x02);
      if (!(F & FZ)) CYCT(0x2c5d, 0x2c61);
      else {
        CYC(0x2c5d, 0x2c61);
        A = 0x03;
        CYC(0x2c61, 0x2c64); mem_wr(gb, HL, A);
        A = L;
        alu_add(gb, 0x04);
        L = A;
        CYC(0x2c64, 0x2c6a); mem_wr(gb, HL, 0xff);
      }
    }
  }
  alu_xor(gb, A);
  CYC(0x2c6a, 0x2c6e); W8(wLinkGrabState) = A;
  CYC(0x2c6e, 0x2c71); W8(wLinkGrabState2) = A;
  CYC(0x2c71, 0x2c72);
  ret_effect(gb);
}

void objectAddToGrabbableObjectBuffer_hook(GB *gb) {
  SET_HL(wGrabbableObjectBuffer);
  CYC(0x2c2e, 0x2c31);
  for (;;) {
    L = alu_inc8(gb, L);
    CYC(0x2c31, 0x2c34); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x2c34, 0x2c36);
      A = D;
      CYC(0x2c3d, 0x2c3f); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(0x2c3f, 0x2c41); A = H8(hActiveObjectType);
      CYC(0x2c41, 0x2c42); mem_wr(gb, HL, A);
      CYC(0x2c42, 0x2c43);
      break;
    }
    CYC(0x2c34, 0x2c36);
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, 0x84);
    if (F & FC) { CYCT(0x2c36, 0x2c3c); continue; }
    CYC(0x2c36, 0x2c3d);
    break;
  }
  ret_effect(gb);
}

// breakable tiles (bank 6 body); the carry comes back through bit 0 of e across the bank pop

void tryToBreakTile_body_hook(GB *gb);

static void try_to_break_tile(GB *gb, uint16_t sp0_) {
  CYC(0x2bf6, 0x2bf8); H8(hFF8F) = A;
  bank_push(gb, 0x2bf8, 0x06);
  CALL_C(0x2c02, tryToBreakTile_body_hook, ROM_b06_tryToBreakTile_body, 0x2c05);
  CYC(0x2c05, 0x2c07); E = alu_rl(gb, E);
  bank_pop(gb, 0x2c07);
  CYC(0x2c0d, 0x2c0f); E = alu_rr(gb, E);
  CYC(0x2c0f, 0x2c10);
}

void tryToBreakTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  try_to_break_tile(gb, sp0_);
  ret_effect(gb);
}

void itemTryToBreakTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x2bef, 0x2bf0); H = D;
  CYC(0x2bf0, 0x2bf2); L = OBJ_YH;
  CYC(0x2bf2, 0x2bf3); B = mem_rd(gb, HL);
  CYC(0x2bf3, 0x2bf5); L = OBJ_XH;
  CYC(0x2bf5, 0x2bf6); C = mem_rd(gb, HL);
  try_to_break_tile(gb, sp0_);
  ret_effect(gb);
}

// global flags

void unsetGlobalFlag_hook(GB *gb) {
  SET_HL(wGlobalFlags);
  CYC(0x31ff, 0x3205);
  unset_flag(gb);
  ret_effect(gb);
}

// palette thread control

static void clear_palette_fade_variables(GB *gb) {
  alu_xor(gb, A);
  CYC(0x323e, 0x3242); W8(wPaletteThread_mode) = A;
  CYC(0x3242, 0x3245); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(0x3245, 0x3247); H8(hBgPaletteSources) = A;
  CYC(0x3247, 0x3249); H8(hSprPaletteSources) = A;
  CYC(0x3249, 0x324c); W8(wPaletteThread_updateRate) = A;
  CYC(0x324c, 0x324f); W8(wLockBG7Color3ToBlack) = A;
  SET_HL(wDirtyFadeBgPalettes);
  CYC(0x324f, 0x3253); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3253, 0x3254); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3254, 0x3255); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3255, 0x3256); mem_wr(gb, HL, A);
  CYC(0x3256, 0x3257);
}

void clearPaletteFadeVariables_hook(GB *gb) {
  clear_palette_fade_variables(gb);
  ret_effect(gb);
}

void clearPaletteFadeVariablesAndRefreshPalettes_hook(GB *gb) {
  A = 0xff;
  CYC(0x3238, 0x323c); H8(hDirtyBgPalettes) = A;
  CYC(0x323c, 0x323e); H8(hDirtySprPalettes) = A;
  clear_palette_fade_variables(gb);
  ret_effect(gb);
}

static void make_all_palettes_use_fading(GB *gb) {
  A = 0xff;
  SET_HL(wDirtyFadeBgPalettes);
  CYC(0x327a, 0x3280); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3280, 0x3281); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3281, 0x3282); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3282, 0x3283); mem_wr(gb, HL, A);
  CYC(0x3283, 0x3284);
}

void makeAllPaletteUseFading_hook(GB *gb) {
  make_all_palettes_use_fading(gb);
  ret_effect(gb);
}

static void set_palette_thread_delay(GB *gb) {
  CYC(0x337b, 0x337e); W8(wPaletteThread_counterRefill) = A;
  A = 0x01;
  CYC(0x337e, 0x3383); W8(wPaletteThread_counter) = A;
  CYC(0x3383, 0x3384);
}

void setPaletteThreadDelay_hook(GB *gb) {
  set_palette_thread_delay(gb);
  ret_effect(gb);
}

static void fadeout_white_tail(GB *gb) {
  CYC(0x3273, 0x3276); W8(wPaletteThread_speed) = A;
  alu_xor(gb, A);
  CYC(0x3276, 0x327a); mem_wr(gb, wPaletteFadeOffset, A);
  make_all_palettes_use_fading(gb);
}

void fadeoutToWhite_hook(GB *gb) {
  A = 0x01;
  CYC(0x326c, 0x3271); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x3271, 0x3273);
  fadeout_white_tail(gb);
  ret_effect(gb);
}

void fastFadeoutToWhite_hook(GB *gb) {
  A = 0x01;
  CYC(0x3263, 0x3268); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(0x3268, 0x326c);
  fadeout_white_tail(gb);
  ret_effect(gb);
}

void fadeoutToWhiteWithDelay_hook(GB *gb) {
  CYC(0x3257, 0x325a);
  set_palette_thread_delay(gb);
  A = 0x09;
  CYC(0x325a, 0x325f); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x325f, 0x3263);
  fadeout_white_tail(gb);
  ret_effect(gb);
}

static void fadein_white_tail(GB *gb) {
  CYC(0x32a0, 0x32a3); W8(wPaletteThread_speed) = A;
  A = 0x20;
  CYC(0x32a3, 0x32a8); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(0x32a8, 0x32ab);
  make_all_palettes_use_fading(gb);
}

static void fadein_from_white(GB *gb) {
  A = 0x02;
  CYC(0x3299, 0x329e); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x329e, 0x32a0);
  fadein_white_tail(gb);
}

static void fast_fadein_from_white(GB *gb) {
  A = 0x02;
  CYC(0x3290, 0x3295); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(0x3295, 0x3299);
  fadein_white_tail(gb);
}

void fadeinFromWhite_hook(GB *gb) { fadein_from_white(gb); ret_effect(gb); }
void fastFadeinFromWhite_hook(GB *gb) { fast_fadein_from_white(gb); ret_effect(gb); }

void fadeinFromWhiteWithDelay_hook(GB *gb) {
  CYC(0x3284, 0x3287);
  set_palette_thread_delay(gb);
  A = 0x0a;
  CYC(0x3287, 0x328c); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x328c, 0x3290);
  fadein_white_tail(gb);
  ret_effect(gb);
}

static void fadeout_black_tail(GB *gb) {
  CYC(0x32c7, 0x32ca); W8(wPaletteThread_speed) = A;
  alu_xor(gb, A);
  CYC(0x32ca, 0x32ce); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(0x32ce, 0x32d1);
  make_all_palettes_use_fading(gb);
}

void fadeoutToBlack_hook(GB *gb) {
  A = 0x03;
  CYC(0x32c0, 0x32c5); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x32c5, 0x32c7);
  fadeout_black_tail(gb);
  ret_effect(gb);
}

void fastFadeoutToBlack_hook(GB *gb) {
  A = 0x03;
  CYC(0x32b7, 0x32bc); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(0x32bc, 0x32c0);
  fadeout_black_tail(gb);
  ret_effect(gb);
}

void fadeoutToBlackWithDelay_hook(GB *gb) {
  CYC(0x32ab, 0x32ae);
  set_palette_thread_delay(gb);
  A = 0x0b;
  CYC(0x32ae, 0x32b3); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x32b3, 0x32b7);
  fadeout_black_tail(gb);
  ret_effect(gb);
}

static void fadein_black_tail(GB *gb) {
  CYC(0x32ed, 0x32f0); W8(wPaletteThread_speed) = A;
  A = 0xe0;
  CYC(0x32f0, 0x32f5); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(0x32f5, 0x32f8);
  make_all_palettes_use_fading(gb);
}

void fadeinFromBlack_hook(GB *gb) {
  A = 0x04;
  CYC(0x32e6, 0x32eb); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x32eb, 0x32ed);
  fadein_black_tail(gb);
  ret_effect(gb);
}

void fastFadeinFromBlack_hook(GB *gb) {
  A = 0x04;
  CYC(0x32dd, 0x32e2); W8(wPaletteThread_mode) = A;
  A = 0x03;
  CYC(0x32e2, 0x32e6);
  fadein_black_tail(gb);
  ret_effect(gb);
}

void fadeinFromBlackWithDelay_hook(GB *gb) {
  CYC(0x32d1, 0x32d4);
  set_palette_thread_delay(gb);
  A = 0x0c;
  CYC(0x32d4, 0x32d9); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x32d9, 0x32dd);
  fadein_black_tail(gb);
  ret_effect(gb);
}

static void set_darkening_variables(GB *gb) {
  CYC(0x331e, 0x3321); W8(wPaletteThread_speed) = A;
  CYC(0x3321, 0x3324); A = W8(wPaletteThread_parameter);
  CYC(0x3324, 0x3327); mem_wr(gb, wPaletteFadeOffset, A);
  A = B;
  CYC(0x3327, 0x332b); W8(wPaletteThread_parameter) = A;
  A = 0xfc;
  SET_HL(wDirtyFadeBgPalettes);
  CYC(0x332b, 0x3331); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3331, 0x3333); mem_wr(gb, HL, 0x00);
  L = alu_inc8(gb, L);
  CYC(0x3333, 0x3335); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x3335, 0x3337); mem_wr(gb, HL, 0x00);
  CYC(0x3337, 0x3338);
}

void _setDarkeningVariables_hook(GB *gb) {
  set_darkening_variables(gb);
  ret_effect(gb);
}

static void darken_room_helper(GB *gb) {
  A = 0x05;
  CYC(0x3317, 0x331c); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x331c, 0x331e);
  set_darkening_variables(gb);
}

void _darkenRoomHelper_hook(GB *gb) { darken_room_helper(gb); ret_effect(gb); }
void darkenRoom_hook(GB *gb) { B = 0xf0; CYC(0x3315, 0x3317); darken_room_helper(gb); ret_effect(gb); }
void darkenRoomLightly_hook(GB *gb) { B = 0xf7; CYC(0x32f8, 0x32fc); darken_room_helper(gb); ret_effect(gb); }

void darkenRoomWithSpeed_hook(GB *gb) {
  B = 0xf0;
  CYC(0x330a, 0x330f);
  set_darkening_variables(gb);
  A = 0x05;
  CYC(0x330f, 0x3314); W8(wPaletteThread_mode) = A;
  CYC(0x3314, 0x3315);
  ret_effect(gb);
}

static void brighten_room_helper(GB *gb) {
  CYC(0x3354, 0x3357);
  set_darkening_variables(gb);
  A = 0x06;
  CYC(0x3357, 0x335c); W8(wPaletteThread_mode) = A;
  CYC(0x335c, 0x335d);
}

void _brightenRoomHelper_hook(GB *gb) { brighten_room_helper(gb); ret_effect(gb); }
void brightenRoom_hook(GB *gb) { B = 0x00; A = 0x01; CYC(0x3350, 0x3354); brighten_room_helper(gb); ret_effect(gb); }
void brightenRoomLightly_hook(GB *gb) { B = 0xf7; A = 0x01; CYC(0x3338, 0x333e); brighten_room_helper(gb); ret_effect(gb); }
void brightenRoomWithSpeed_hook(GB *gb) { B = 0x00; CYC(0x334c, 0x3350); brighten_room_helper(gb); ret_effect(gb); }

static void fadein_to_room_tail(GB *gb) {
  A = 0x07;
  CYC(0x3365, 0x336a); W8(wPaletteThread_mode) = A;
  CYC(0x336a, 0x336b);
}

void fastFadeinFromWhiteToRoom_hook(GB *gb) {
  CYC(0x335d, 0x3360);
  fast_fadein_from_white(gb);
  A = 0x1e;
  CYC(0x3360, 0x3365); mem_wr(gb, wPaletteFadeOffset, A);
  fadein_to_room_tail(gb);
  ret_effect(gb);
}

void fadeinFromWhiteToRoom_hook(GB *gb) {
  CYC(0x336b, 0x336e);
  fadein_from_white(gb);
  CYC(0x336e, 0x3370);
  fadein_to_room_tail(gb);
  ret_effect(gb);
}

static void start_fade_between_two_palettes(GB *gb) {
  A = 0x08;
  CYC(0x3370, 0x3375); W8(wPaletteThread_mode) = A;
  A = 0x20;
  CYC(0x3375, 0x337a); mem_wr(gb, wPaletteFadeOffset, A);
  CYC(0x337a, 0x337b);
}

void startFadeBetweenTwoPalettes_hook(GB *gb) {
  start_fade_between_two_palettes(gb);
  ret_effect(gb);
}

// enemies and object tables

void setEnemyTargetToLinkPosition_hook(GB *gb) {
  CYC(0x3616, 0x3619); A = W8(wLinkObjectIndex);
  H = A;
  L = OBJ_YH;
  CYC(0x3619, 0x361d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x361d, 0x361f); H8(hEnemyTargetY) = A;
  L = alu_inc8(gb, L);
  CYC(0x361f, 0x3621); A = mem_rd(gb, HL);
  CYC(0x3621, 0x3623); H8(hEnemyTargetX) = A;
  CYC(0x3623, 0x3626); A = W8(wScentSeedActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x3626, 0x3628); ret_effect(gb); return; }
  CYC(0x3626, 0x3628);
  L = OBJ_YH;
  CYC(0x3628, 0x362b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x362b, 0x362d); H8(hFFB2) = A;
  L = alu_inc8(gb, L);
  CYC(0x362d, 0x362f); A = mem_rd(gb, HL);
  CYC(0x362f, 0x3631); H8(hFFB3) = A;
  CYC(0x3631, 0x3632);
  ret_effect(gb);
}

void getEntryFromObjectTable2_hook(GB *gb) {
  bank_push(gb, 0x3632, 0x12);
  A = B;
  SET_HL(objectTable2_bank12);
  CYC(0x363c, 0x3641);
  add_double_index_to_hl(gb);
  CYC(0x3641, 0x3642); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3642, 0x3643); H = mem_rd(gb, HL);
  L = A;
  CYC(0x3643, 0x3644);
  bank_pop(gb, 0x3644);
  CYC(0x364a, 0x364b);
  ret_effect(gb);
}

void checkDungeonUsesToggleBlocks_hook(GB *gb) {
  CYC(0x364b, 0x364e); A = W8(wDungeonIndex);
  alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x364e, 0x3651); ret_effect(gb); return; }
  CYC(0x364e, 0x3651);
  SET_HL(dungeonsUsingToggleBlocks);
  CYC(0x3651, 0x3657);
  check_flag(gb);
  ret_effect(gb);
}

void getIndexOfGashaSpotInRoom_hook(GB *gb) {
  C = A;
  CYC(0x36a8, 0x36a9);
  bank_push(gb, 0x36a9, 0x02);
  A = C;
  CYC(0x36b3, 0x36b4);
  CALL_ROM(0x36b4, ROM_b02_getIndexOfGashaSpotInRoom_body);
  SET_BC(AF);
  CYC(0x36b7, 0x36b9);
  bank_pop(gb, 0x36b9);
  CYC(0x36bf, 0x36c0);
  ret_effect(gb);
}

void getBlackTowerProgress_hook(GB *gb) {
  uint16_t bc = BC;
  C = 0x02;
  CYC(0x36c0, 0x36c6); A = mem_rd(gb, wGroup0RoomFlags + 0x90);
  alu_bit(gb, 6, A);
  if (!(F & FZ)) CYCT(0x36c6, 0x36ca);
  else {
    C = alu_dec8(gb, C);
    CYC(0x36c6, 0x36ce); A = mem_rd(gb, wGroup0RoomFlags + 0xba);
    alu_bit(gb, 6, A);
    if (!(F & FZ)) CYCT(0x36ce, 0x36d2);
    else { CYC(0x36ce, 0x36d3); C = alu_dec8(gb, C); }
  }
  A = C;
  SET_BC(bc);
  CYC(0x36d3, 0x36d6);
  ret_effect(gb);
}

// tile mapping and the changed-tile queue

static void set_hl_to_tile_mapping_data(GB *gb) {
  CYC(0x3a94, 0x3a97);
  multiply_a_by_8(gb);
  SET_HL(w3TileMappingData);
  alu_add_hl(gb, BC);
  CYC(0x3a97, 0x3a9c);
}

void setHlToTileMappingDataPlusATimes8_hook(GB *gb) {
  set_hl_to_tile_mapping_data(gb);
  ret_effect(gb);
}

void getTileMappingData_hook(GB *gb) {
  C = A;
  CYC(0x3a72, 0x3a75); uint8_t svbk = mem_rd(gb, IO_SVBK), f = F;
  CYC(0x3a75, 0x3a7a); mem_wr(gb, IO_SVBK, 0x03);
  A = C;
  CYC(0x3a7a, 0x3a7e);
  set_hl_to_tile_mapping_data(gb);
  uint16_t de = DE;
  SET_DE(wTmpcec0);
  B = 0x08;
  CYC(0x3a7e, 0x3a87);
  copyMemory(gb, DE, HL, 8);
  SET_HL(HL + 8);
  SET_DE(de);
  CYC(0x3a87, 0x3a8b); A = mem_rd(gb, wTmpcec0 + 4);
  B = A;
  CYC(0x3a8b, 0x3a8f); A = mem_rd(gb, wTmpcec0);
  C = A;
  A = svbk;
  F = f;
  CYC(0x3a8f, 0x3a93); mem_wr(gb, IO_SVBK, A);
  CYC(0x3a93, 0x3a94);
  ret_effect(gb);
}

static void set_tile(GB *gb) {
  B = A;
  CYC(0x3a9c, 0x3aa0); A = W8(wChangedTileQueueTail);
  A = alu_inc8(gb, A);
  alu_and(gb, 0x1f);
  E = A;
  CYC(0x3aa0, 0x3aa7); A = W8(wChangedTileQueueHead);
  alu_cp(gb, E);
  if (F & FZ) { CYCT(0x3aa7, 0x3aa9); return; }
  CYC(0x3aa7, 0x3aa9);
  A = E;
  CYC(0x3aa9, 0x3aad); W8(wChangedTileQueueTail) = A;
  CYC(0x3aad, 0x3aaf); uint8_t svbk = mem_rd(gb, IO_SVBK), f = F;
  CYC(0x3aaf, 0x3ab4); mem_wr(gb, IO_SVBK, 0x02);
  A = E;
  alu_add(gb, A);
  SET_HL(w2ChangedTileQueue);
  CYC(0x3ab4, 0x3aba);
  add_a_to_hl(gb);
  CYC(0x3aba, 0x3abb); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x3abb, 0x3abd); mem_wr(gb, HL, C);
  A = B;
  CYC(0x3abd, 0x3ac1);
  set_tile_without_gfx_reload(gb);
  A = svbk;
  F = f;
  CYC(0x3ac1, 0x3ac4); mem_wr(gb, IO_SVBK, A);
  alu_or(gb, H);
  CYC(0x3ac4, 0x3ac6);
}

void setTile_hook(GB *gb) {
  set_tile(gb);
  ret_effect(gb);
}

void setTileInAllBuffers_hook(GB *gb) {
  E = A;
  B = A;
  CYC(0x3ac6, 0x3acb);
  set_tile_in_room_layout_buffer(gb);
  A = E;
  CYC(0x3acb, 0x3acf);
  set_tile(gb);
  ret_effect(gb);
}

static void set_interleaved_tile(GB *gb, uint16_t sp0_) {
  uint16_t de = DE;
  E = A;
  CYC(0x3acf, 0x3ad3); C = mem_rd(gb, IO_SVBK);
  CYC(0x3ad3, 0x3ad6); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(0x3ad6, 0x3adc); H8(hRomBank) = 0x04;
  CYC(0x3adc, 0x3adf); mem_wr(gb, MBC_ROM_BANK, 0x04);
  A = E;
  CYC(0x3adf, 0x3ae0);
  CALL_C(0x3ae0, setInterleavedTile_body_hook, 0x6cb3, 0x3ae3);
  SET_BC(bc);
  A = B;
  CYC(0x3ae3, 0x3ae7); H8(hRomBank) = A;
  CYC(0x3ae7, 0x3aea); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(0x3aea, 0x3aed); mem_wr(gb, IO_SVBK, A);
  SET_DE(de);
  CYC(0x3aed, 0x3aef);
}

void setInterleavedTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  set_interleaved_tile(gb, sp0_);
  ret_effect(gb);
}

// interaction slots

static void get_free_interaction_slot(GB *gb) {
  SET_HL(wInteractionSlots);
  CYC(0x3aef, 0x3af2);
  for (;;) {
    CYC(0x3af2, 0x3af3); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x3af3, 0x3af6);
      CYC(0x3afe, 0x3aff); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      L = alu_inc8(gb, L);
      alu_xor(gb, A);
      CYC(0x3aff, 0x3b02);
      break;
    }
    CYC(0x3af3, 0x3af6);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x3af6, 0x3afc); continue; }
    CYC(0x3af6, 0x3afc);
    alu_or(gb, H);
    CYC(0x3afc, 0x3afe);
    break;
  }
}

void getFreeInteractionSlot_hook(GB *gb) {
  get_free_interaction_slot(gb);
  ret_effect(gb);
}

static void interaction_delete(GB *gb) {
  H = D;
  L = INTERACTION_BASE;
  B = 0x10;
  alu_xor(gb, A);
  CYC(0x3b05, 0x3b0b);
  do {
    CYC(0x3b0b, 0x3b0c); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x3b0c, 0x3b0d); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x3b0d, 0x3b0e); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x3b0e, 0x3b0f); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x3b0f, 0x3b12); else CYC(0x3b0f, 0x3b12);
  } while (B);
  CYC(0x3b12, 0x3b13);
}

void interactionDelete_hook(GB *gb) {
  interaction_delete(gb);
  ret_effect(gb);
}

void interactionDeleteAndUnmarkSolidPosition_hook(GB *gb) {
  CYC(0x3b02, 0x3b05);
  unmark_solid_position(gb);
  interaction_delete(gb);
  ret_effect(gb);
}

void checkNpcShouldExistAtGameStage_hook(GB *gb) {
  CYC(0x3d78, 0x3d7a); H8(hFF8B) = A;
  bank_push(gb, 0x3d7a, 0x09);
  CYC(0x3d84, 0x3d86); A = H8(hFF8B);
  CALL_ROM(0x3d86, ROM_b09_checkNpcShouldExistAtGameStage_body);
  C = 0x00;
  if (F & FZ) CYCT(0x3d89, 0x3d8d);
  else { CYC(0x3d89, 0x3d8e); C = alu_inc8(gb, C); }
  bank_pop(gb, 0x3d8e);
  A = C;
  alu_or(gb, A);
  CYC(0x3d94, 0x3d97);
  ret_effect(gb);
}

void interactionSetSimpleScript_hook(GB *gb) {
  E = 0x58;
  A = L;
  CYC(0x3da0, 0x3da4); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(0x3da4, 0x3da7); mem_wr(gb, DE, A);
  CYC(0x3da7, 0x3da8);
  ret_effect(gb);
}

static void generate_vram_tiles_with_room_changes(GB *gb, uint16_t sp0_);

// screen music and room packs (bank 4 tables)

static void load_screen_music(GB *gb) {
  bank_push(gb, 0x33cf, 0x04);
  CYC(0x33d9, 0x33dc); A = W8(wActiveGroup);
  SET_HL(musicAssignmentGroupTable_bank04);
  CYC(0x33dc, 0x33e0);
  add_double_index_to_hl(gb);
  CYC(0x33e0, 0x33e1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x33e1, 0x33e2); H = mem_rd(gb, HL);
  L = A;
  CYC(0x33e2, 0x33e6); A = W8(wActiveRoom);
  CYC(0x33e6, 0x33e7);
  add_a_to_hl(gb);
  CYC(0x33e7, 0x33e8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x33e8, 0x33eb); W8(wActiveMusic2) = A;
  CYC(0x33eb, 0x33ee); A = W8(wActiveGroup);
  alu_cp(gb, 0x02);
  if (!(F & FC)) CYCT(0x33ee, 0x33f2);
  else {
    CYC(0x33ee, 0x33f2);
    B = A;
    CYC(0x33f2, 0x33f6); A = W8(wActiveRoom);
    C = A;
    SET_HL(roomPackData_bank04);
    alu_add_hl(gb, BC);
    CYC(0x33f6, 0x33fc); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x33fc, 0x33ff); W8(wLoadingRoomPack) = A;
  }
  bank_pop(gb, 0x33ff);
  CYC(0x3405, 0x3406);
}

void loadScreenMusic_hook(GB *gb) {
  load_screen_music(gb);
  ret_effect(gb);
}

void applyWarpDest_hook(GB *gb) {
  bank_push(gb, 0x3406, 0x04);
  CALL_ROM(0x3410, ROM_b04_applyWarpDest_b04);
  bank_pop(gb, 0x3413);
  CYC(0x3419, 0x341a);
  ret_effect(gb);
}

static void load_screen_music_and_set_room_pack(GB *gb) {
  CYC(0x341a, 0x341d);
  load_screen_music(gb);
  CYC(0x341d, 0x3420); A = W8(wActiveRoom);
  CYC(0x3420, 0x3423); W8(wLoadingRoom) = A;
  CYC(0x3423, 0x3426); A = W8(wActiveGroup);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x3426, 0x3428); return; }
  CYC(0x3426, 0x342b); A = W8(wLoadingRoomPack);
  alu_and(gb, 0x7f);
  CYC(0x342b, 0x3430); W8(wRoomPack) = A;
  CYC(0x3430, 0x3431);
}

void loadScreenMusicAndSetRoomPack_hook(GB *gb) {
  load_screen_music_and_set_room_pack(gb);
  ret_effect(gb);
}

void dismountCompanionAndSetRememberedPositionToScreenCenter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x3431, 0x05);
  SET_DE(w1Companion);
  A = E;
  CYC(0x343b, 0x3441); H8(hActiveObjectType) = A;
  A = D;
  CYC(0x3441, 0x3444); H8(hActiveObject) = A;
  CALL_C(0x3444, companionDismount_hook, 0x4630, 0x3447);
  CALL_C(0x3447, saveLinkLocalRespawnAndCompanionPosition_hook, 0x467c, 0x344a);
  A = 0x38;
  CYC(0x344a, 0x344f); W8(wRememberedCompanionY) = A;
  A = 0x50;
  CYC(0x344f, 0x3454); W8(wRememberedCompanionX) = A;
  bank_pop(gb, 0x3454);
  CYC(0x345a, 0x345b);
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
  alu_xor(gb, A);
  CYC(0x3597, 0x359a); mem_wr(gb, IO_SVBK, A);
  SET_HL(w1Link);
  SET_BC(0x1000);
  CYC(0x359a, 0x35a3);
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
  SET_HL(wScreenVariables);
  B = 0x30;
  CYC(0x35a6, 0x35ae);
  clear_block(gb, HL, 0x30);
  A = 0xff;
  CYC(0x35ae, 0x35b3); W8(wLoadedTilesetUniqueGfx) = A;
  CYC(0x35b3, 0x35b6); W8(wLoadedTilesetLayout) = A;
  CYC(0x35b6, 0x35b9); W8(wLoadedTilesetAnimation) = A;
  CYC(0x35b9, 0x35ba);
}

void clearScreenVariables_hook(GB *gb) {
  clear_screen_variables(gb);
  ret_effect(gb);
}

void clearScreenVariablesAndWramBank1_hook(GB *gb) {
  CYC(0x35a3, 0x35a6);
  clear_wram_bank1(gb);
  clear_screen_variables(gb);
  ret_effect(gb);
}

void clearLinkObject_hook(GB *gb) {
  SET_HL(w1Link);
  B = 0x40;
  CYC(0x35ba, 0x35c2);
  clear_block(gb, HL, 0x40);
  ret_effect(gb);
}

static void clear_reserved_interaction1(GB *gb) {
  SET_HL(w1ReservedInteraction0 + 0x100);
  B = 0x40;
  CYC(0x35ca, 0x35d2);
  clear_block(gb, HL, 0x40);
}

void clearReservedInteraction1_hook(GB *gb) {
  clear_reserved_interaction1(gb);
  ret_effect(gb);
}

void clearReservedInteraction0_hook(GB *gb) {
  SET_HL(w1ReservedInteraction0);
  B = 0x40;
  CYC(0x35c2, 0x35ca);
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

void clearDynamicInteractions_hook(GB *gb) { clear_object_slots(gb, 0x35d2, wInteractionSlots); ret_effect(gb); }
void clearItems_hook(GB *gb) { clear_object_slots(gb, 0x35e3, w1WeaponItem); ret_effect(gb); }
void clearEnemies_hook(GB *gb) { clear_object_slots(gb, 0x35f4, ENEMY_SLOTS); ret_effect(gb); }
void clearParts_hook(GB *gb) { clear_object_slots(gb, 0x3605, PART_SLOTS); ret_effect(gb); }

// tileset animation data (bank 4 tables)

static void load_animation_pointer(GB *gb) {
  uint16_t hl = HL;
  CYC(0x3698, 0x369a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x369a, 0x369b); H = mem_rd(gb, HL);
  L = A;
  CYC(0x369b, 0x369d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x369d, 0x369e); mem_wr(gb, DE, A);
  SET_DE(DE + 1);
  A = L;
  CYC(0x369e, 0x36a1); mem_wr(gb, DE, A);
  SET_DE(DE + 1);
  A = H;
  CYC(0x36a1, 0x36a4); mem_wr(gb, DE, A);
  SET_HL((uint16_t)(hl + 2));
  CYC(0x36a4, 0x36a8);
}

static void load_animation_data(GB *gb) {
  B = A;
  CYC(0x3659, 0x365a);
  bank_push(gb, 0x365a, 0x04);
  A = B;
  SET_HL(animationGroupTable_bank04);
  CYC(0x3664, 0x3669);
  add_double_index_to_hl(gb);
  CYC(0x3669, 0x366a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x366a, 0x366b); H = mem_rd(gb, HL);
  L = A;
  CYC(0x366b, 0x366d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x366d, 0x3670); W8(wAnimationState) = A;
  uint16_t de = DE;
  SET_DE(wAnimationCounter1);
  CYC(0x3670, 0x3677);
  load_animation_pointer(gb);
  SET_DE(wAnimationCounter2);
  CYC(0x3677, 0x367d);
  load_animation_pointer(gb);
  SET_DE(wAnimationCounter3);
  CYC(0x367d, 0x3683);
  load_animation_pointer(gb);
  SET_DE(wAnimationCounter4);
  CYC(0x3683, 0x3689);
  load_animation_pointer(gb);
  SET_DE(de);
  CYC(0x3689, 0x368a);
  bank_pop(gb, 0x368a);
  alu_xor(gb, A);
  CYC(0x3690, 0x3694); W8(wAnimationQueueHead) = A;
  CYC(0x3694, 0x3697); W8(wAnimationQueueTail) = A;
  CYC(0x3697, 0x3698);
}

void loadAnimationData_hook(GB *gb) {
  load_animation_data(gb);
  ret_effect(gb);
}

void forceLoadRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  alu_and(gb, 0x03);
  CYC(0x36f6, 0x36fb); W8(wRoomStateModifier) = A;
  A = B;
  CYC(0x36fb, 0x36ff); W8(wActiveGroup) = A;
  A = C;
  CYC(0x36ff, 0x3703); W8(wActiveRoom) = A;
  CYC(0x3703, 0x3706);
  load_screen_music_and_set_room_pack(gb);
  CALL_ROM(0x3706, ROM_loadTilesetData);
  CALL_ROM(0x3709, ROM_loadTilesetGraphics);
  CALL_ROM(0x370c, ROM_loadTilesetAndRoomLayout);
  CYC(0x370f, 0x3712);
  generate_vram_tiles_with_room_changes(gb, sp0_);
  ret_effect(gb);
}

// unique graphics headers (bank 4 table)

void loadUniqueGfxHeader_hook(GB *gb) {
  alu_and(gb, 0x7f);
  B = A;
  CYC(0x3775, 0x3778);
  bank_push(gb, 0x3778, 0x04);
  A = B;
  SET_HL(uniqueGfxHeaderTable_bank04);
  CYC(0x3782, 0x3787);
  add_double_index_to_hl(gb);
  CYC(0x3787, 0x3788); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3788, 0x378b); mem_wr(gb, wUniqueGfxHeaderAddress, A);
  CYC(0x378b, 0x378c); A = mem_rd(gb, HL);
  CYC(0x378c, 0x378f); mem_wr(gb, wUniqueGfxHeaderAddress + 1, A);
  bank_pop(gb, 0x378f);
  CYC(0x3795, 0x3796);
  ret_effect(gb);
}

void updateTilesetUniqueGfx_hook(GB *gb) {
  CYC(0x37db, 0x37de); A = W8(wTilesetUniqueGfx);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x37de, 0x37e0); ret_effect(gb); return; }
  CYC(0x37de, 0x37e0);
  B = A;
  CYC(0x37e0, 0x37e4); A = W8(wLoadedTilesetUniqueGfx);
  alu_cp(gb, B);
  if (F & FZ) { CYCT(0x37e4, 0x37e6); ret_effect(gb); return; }
  CYC(0x37e4, 0x37e8); A = H8(hRomBank);
  CYC(0x37e8, 0x37e9); push_effect(gb, AF);
  SET_HL(wUniqueGfxHeaderAddress);
  CYC(0x37e9, 0x37ed); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x37ed, 0x37ee); H = mem_rd(gb, HL);
  L = A;
  CYC(0x37ee, 0x37f3); H8(hRomBank) = 0x04;
  CYC(0x37f3, 0x37f6); mem_wr(gb, MBC_ROM_BANK, 0x04);
  CALL_ROM(0x37f6, ROM_loadUniqueGfxHeaderEntry);
  C = A;
  A = L;
  CYC(0x37f9, 0x37fe); mem_wr(gb, wUniqueGfxHeaderAddress, A);
  A = H;
  CYC(0x37fe, 0x3802); mem_wr(gb, wUniqueGfxHeaderAddress + 1, A);
  CYC(0x3802, 0x3803); SET_AF(pop_effect(gb));
  CYC(0x3803, 0x3805); H8(hRomBank) = A;
  CYC(0x3805, 0x3808); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  alu_add(gb, A);
  CYC(0x3808, 0x380b);
  ret_effect(gb);
}

// unique gfx header entries (bank 4 headers, decompressed into w3VramTiles)

static void load_unique_gfx_header_entry(GB *gb) {
  CYC(0x3843, 0x3844); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x3844, 0x3847);
    uint16_t hl = HL;
    CYC(0x387f, 0x3881); A = mem_rd(gb, HL);
    alu_and(gb, 0x7f);
    CYC(0x3881, 0x3883);
    CALL_ROM(0x3883, ROM_loadPaletteHeader);
    SET_HL(hl);
    CYC(0x3886, 0x3888); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x3888, 0x3889);
    return;
  }
  CYC(0x3844, 0x3847);
  C = A;
  CYC(0x3847, 0x384a); H8(hFF8C) = A;
  CYC(0x384a, 0x384b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(0x384b, 0x384d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(0x384d, 0x384f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  D = A;
  CYC(0x384f, 0x3851); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(0x3851, 0x3853); A = mem_rd(gb, HL);
  alu_and(gb, 0x7f);
  CYC(0x3853, 0x3857); H8(hFF8D) = A;
  uint16_t hl = HL, de = DE;
  L = C;
  H = B;
  B = A;
  CYC(0x3857, 0x385e); A = H8(hFF8C);
  C = A;
  SET_DE(w3VramTiles + 7);
  CYC(0x385e, 0x3862);
  CALL_ROM(0x3862, ROM_decompressGraphics);
  SET_DE(de);
  SET_HL(w3VramTiles);
  C = 0x07;
  CYC(0x3865, 0x386d); A = H8(hFF8D);
  B = A;
  CYC(0x386d, 0x386e);
  CALL_ROM(0x386e, ROM_queueDmaTransfer);
  SET_HL(hl);
  A = 0x00;
  CYC(0x3871, 0x3876); mem_wr(gb, IO_SVBK, A);
  A = 0x04;
  CYC(0x3876, 0x387a); H8(hRomBank) = A;
  CYC(0x387a, 0x387d); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x387d, 0x387e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x387e, 0x387f);
}

void loadUniqueGfxHeaderEntry_hook(GB *gb) {
  load_unique_gfx_header_entry(gb);
  ret_effect(gb);
}

void uniqueGfxFunc_380b_hook(GB *gb) {
  B = A;
  CYC(0x380b, 0x380c);
  bank_push(gb, 0x380c, 0x04);
  A = B;
  SET_HL(uniqueGfxHeaderTable_bank04);
  CYC(0x3816, 0x381b);
  add_double_index_to_hl(gb);
  CYC(0x381b, 0x381c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x381c, 0x381d); H = mem_rd(gb, HL);
  L = A;
  CYC(0x381d, 0x3821);
  load_unique_gfx_header_entry(gb);
  bank_pop(gb, 0x3821);
  CYC(0x3827, 0x3828);
  ret_effect(gb);
}

static void generate_vram_tiles_with_room_changes(GB *gb, uint16_t sp0_) {
  CYC(0x3a4e, 0x3a50); C = mem_rd(gb, IO_SVBK);
  CYC(0x3a50, 0x3a53); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(0x3a53, 0x3a59); H8(hRomBank) = 0x04;
  CYC(0x3a59, 0x3a5c); mem_wr(gb, MBC_ROM_BANK, 0x04);
  CALL_C(0x3a5c, generateW3VramTilesAndAttributes_hook, 0x6bf1, 0x3a5f);
  SET_HL(ROM_b02_applyRoomSpecificTileChangesAfterGfxLoad);
  E = 0x02;
  CYC(0x3a5f, 0x3a64);
  CALL_ROM(0x3a64, ROM_interBankCall);
  SET_BC(bc);
  A = B;
  CYC(0x3a67, 0x3a6b); H8(hRomBank) = A;
  CYC(0x3a6b, 0x3a6e); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(0x3a6e, 0x3a71); mem_wr(gb, IO_SVBK, A);
  CYC(0x3a71, 0x3a72);
}

void generateVramTilesWithRoomChanges_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  generate_vram_tiles_with_room_changes(gb, sp0_);
  ret_effect(gb);
}

// simple scripts (bank $0c): commands 0 to 4 through the rst $00 jump table at $3dd7

static void simple_script_run_command(GB *gb, uint16_t sp0_) {
  CYC(0x3dd4, 0x3dd5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3dd5, 0x3dd6); push_effect(gb, HL);
  CYC(0x3dd6, 0x3dd7); push_effect(gb, simpleScriptCommandTable);
  CYC(0x0000, 0x0001); alu_add(gb, A);
  CYC(0x0001, 0x0002); SET_HL(pop_effect(gb));
  CYC(0x0002, 0x0004); alu_add(gb, L); L = A;
  if (!(F & FC)) CYCT(0x0004, 0x0006);
  else { CYC(0x0004, 0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007, 0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0008, 0x0009); H = mem_rd(gb, HL);
  L = A;
  CYC(0x0009, 0x000b);
  switch (HL) {
  case 0x3de1:
    CYC(0x3de1, 0x3de2); SET_HL(pop_effect(gb));
    CYC(0x3de2, 0x3de3);
    break;
  case 0x3de3:
    CYC(0x3de3, 0x3de4); SET_HL(pop_effect(gb));
    CYC(0x3de4, 0x3de5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = INTERACTION_BASE + OBJ_COUNTER1;
    CYC(0x3de5, 0x3de8); mem_wr(gb, DE, A);
    alu_xor(gb, A);
    CYC(0x3de8, 0x3dea);
    break;
  case 0x3dea: {
    CYC(0x3dea, 0x3deb); SET_HL(pop_effect(gb));
    CYC(0x3deb, 0x3dec); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    CYC(0x3dec, 0x3ded);
    CALL_ROM(0x3ded, ROM_playSound);
    SET_HL(hl);
    CYC(0x3df0, 0x3df2);
    break;
  }
  case 0x3df2: {
    CYC(0x3df2, 0x3df3); SET_HL(pop_effect(gb));
    CYC(0x3df3, 0x3df4); A = mem_rd(gb, HL); SET_HL(HL + 1);
    C = A;
    CYC(0x3df4, 0x3df5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    CYC(0x3df5, 0x3dfa);
    set_tile(gb);
    SET_HL(hl);
    alu_scf(gb);
    CYC(0x3dfa, 0x3dfd);
    break;
  }
  default: {
    CYC(0x3dfd, 0x3dfe); SET_HL(pop_effect(gb));
    CYC(0x3dfe, 0x3dff); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x3dff, 0x3e01); H8(hFF8C) = A;
    CYC(0x3e01, 0x3e02); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x3e02, 0x3e04); H8(hFF8F) = A;
    CYC(0x3e04, 0x3e05); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x3e05, 0x3e07); H8(hFF8E) = A;
    CYC(0x3e07, 0x3e08); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    CYC(0x3e08, 0x3e0c);
    set_interleaved_tile(gb, sp0_);
    SET_HL(hl);
    alu_scf(gb);
    CYC(0x3e0c, 0x3e0f);
    break;
  }
  }
}

void interactionRunSimpleScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x3da8, 0x0c);
  H = D;
  L = 0x58;
  CYC(0x3db2, 0x3db6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3db6, 0x3db7); H = mem_rd(gb, HL);
  L = A;
  CYC(0x3db7, 0x3db8);
  for (;;) {
    CYC(0x3db8, 0x3db9); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x3db9, 0x3dbc);
      bank_pop(gb, 0x3dcc);
      alu_scf(gb);
      CYC(0x3dd2, 0x3dd4);
      break;
    }
    CYC(0x3db9, 0x3dbc);
    CYC(0x3dbc, 0x3dbf);
    simple_script_run_command(gb, sp0_);
    if (F & FC) { CYCT(0x3dbf, 0x3dc1); continue; }
    CYC(0x3dbf, 0x3dc1);
    CYC(0x3dc1, 0x3dc4);
    E = 0x58;
    A = L;
    CYC(0x3da0, 0x3da4); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    A = H;
    CYC(0x3da4, 0x3da7); mem_wr(gb, DE, A);
    CYC(0x3da7, 0x3da8);
    bank_pop(gb, 0x3dc4);
    alu_xor(gb, A);
    CYC(0x3dca, 0x3dcc);
    break;
  }
  ret_effect(gb);
}

// decorative interactions

void objectCreateSparkle_hook(GB *gb) {
  CYC(0x3e28, 0x3e2b);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(0x3e2b, 0x3e2c); ret_effect(gb); return; }
  CYC(0x3e2b, 0x3e2e); mem_wr(gb, HL, 0x84);
  L = alu_inc8(gb, L);
  CYC(0x3e2e, 0x3e31); mem_wr(gb, HL, 0x00);
  CYC(0x3e31, 0x3e34);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void objectCreateSparkleMovingUp_hook(GB *gb) {
  CYC(0x3e34, 0x3e37);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(0x3e37, 0x3e38); ret_effect(gb); return; }
  CYC(0x3e37, 0x3e3a); mem_wr(gb, HL, 0x84);
  L = alu_inc8(gb, L);
  CYC(0x3e3a, 0x3e3d); mem_wr(gb, HL, 0x02);
  L = 0x50;
  CYC(0x3e3d, 0x3e41); mem_wr(gb, HL, 0x80);
  L = alu_inc8(gb, L);
  CYC(0x3e41, 0x3e44); mem_wr(gb, HL, 0xff);
  CYC(0x3e44, 0x3e47);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void objectCreateRedBlueOrb_hook(GB *gb) {
  CYC(0x3e47, 0x3e4a);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(0x3e4a, 0x3e4b); ret_effect(gb); return; }
  CYC(0x3e4a, 0x3e4d); mem_wr(gb, HL, 0x84);
  L = alu_inc8(gb, L);
  CYC(0x3e4d, 0x3e50); mem_wr(gb, HL, 0x04);
  CYC(0x3e50, 0x3e53);
  copy_position_with_offset(gb);
  ret_effect(gb);
}

void incMakuTreeState_hook(GB *gb) {
  CYC(0x3e53, 0x3e56); A = W8(wMakuTreeState);
  CYC(0x3e56, 0x3e57); A = alu_inc8(gb, A);
  alu_cp(gb, 0x11);
  if (F & FC) CYCT(0x3e57, 0x3e5b);
  else { CYC(0x3e57, 0x3e5d); A = 0x10; }
  CYC(0x3e5d, 0x3e60); W8(wMakuTreeState) = A;
  CYC(0x3e60, 0x3e61);
  ret_effect(gb);
}

void setLinkDirection_hook(GB *gb) {
  B = A;
  CYC(0x3e61, 0x3e65); A = W8(wLinkObjectIndex);
  H = A;
  L = 0x08;
  CYC(0x3e65, 0x3e69); mem_wr(gb, HL, B);
  H = w1Link >> 8;
  CYC(0x3e69, 0x3e6c); mem_wr(gb, HL, B);
  CYC(0x3e6c, 0x3e6d);
  ret_effect(gb);
}

void interactionFunc_3e6d_hook(GB *gb) {
  uint16_t de = DE;
  L = 0x43;
  CYC(0x3e6d, 0x3e71); E = mem_rd(gb, HL);
  bank_push(gb, 0x3e71, 0x16);
  A = E;
  SET_HL(creditsSpriteTable_bank16);
  CYC(0x3e7b, 0x3e80);
  add_double_index_to_hl(gb);
  CYC(0x3e80, 0x3e81); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3e81, 0x3e82); H = mem_rd(gb, HL);
  L = A;
  CYC(0x3e82, 0x3e83);
  CALL_ROM(0x3e83, ROM_addSpritesToOam_withOffset);
  bank_pop(gb, 0x3e86);
  SET_DE(de);
  CYC(0x3e8c, 0x3e8e);
  ret_effect(gb);
}

// parts

void getFreePartSlot_hook(GB *gb) {
  SET_HL(PART_SLOTS);
  CYC(0x3e8e, 0x3e91);
  for (;;) {
    CYC(0x3e91, 0x3e92); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x3e92, 0x3e95);
      A = alu_inc8(gb, A);
      CYC(0x3e9d, 0x3e9f); mem_wr(gb, HL, A); SET_HL(HL + 1);
      alu_xor(gb, A);
      CYC(0x3e9f, 0x3ea1);
      break;
    }
    CYC(0x3e92, 0x3e95);
    H = alu_inc8(gb, H);
    A = H;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x3e95, 0x3e9b); continue; }
    CYC(0x3e95, 0x3e9b);
    alu_or(gb, H);
    CYC(0x3e9b, 0x3e9d);
    break;
  }
  ret_effect(gb);
}

void partDelete_hook(GB *gb) {
  H = D;
  L = 0xc0;
  B = 0x10;
  alu_xor(gb, A);
  CYC(0x3ea1, 0x3ea7);
  do {
    CYC(0x3ea7, 0x3ea8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x3ea8, 0x3ea9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x3ea9, 0x3eaa); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x3eaa, 0x3eab); mem_wr(gb, HL, A); SET_HL(HL + 1);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x3eab, 0x3eae); else CYC(0x3eab, 0x3eae);
  } while (B);
  CYC(0x3eae, 0x3eaf);
  ret_effect(gb);
}

void checkLinkCanSurface_hook(GB *gb) {
  CYC(0x3eaf, 0x3eb2); A = W8(wTilesetFlags);
  alu_and(gb, 0x40);
  if (F & FZ) { CYCT(0x3eb2, 0x3eb5); ret_effect(gb); return; }
  CYC(0x3eb2, 0x3eb5);
  SET_HL(ROM_b12_checkLinkCanSurface_isUnderwater);
  E = 0x12;
  CYC(0x3eb5, 0x3eba);
  CALL_ROM(0x3eba, ROM_interBankCall);
  C = alu_srl(gb, C);
  CYC(0x3ebd, 0x3ec0);
  ret_effect(gb);
}

void copy256BytesFromBank_hook(GB *gb) {
  A = E;
  CYC(0x3ec0, 0x3ec3); mem_wr(gb, IO_SVBK, A);
  A = C;
  CYC(0x3ec3, 0x3ec6); H8(hRomBank) = A;
  CYC(0x3ec6, 0x3ec9); mem_wr(gb, MBC_ROM_BANK, A);
  E = 0x00;
  B = 0x00;
  CYC(0x3ec9, 0x3ed0);
  copy_loop(gb, 0x0486, DE, HL, 256);
  copy_done(gb, HL, 256);
}

// movement scripts (bank $0e bodies)

void objectLoadMovementScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x3035, 0x0e);
  CALL_C(0x303f, objectLoadMovementScript_body_hook, 0x6b2d, 0x3042);
  bank_pop(gb, 0x3042);
  CYC(0x3048, 0x3049);
  ret_effect(gb);
}

void objectRunMovementScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x3049, 0x0e);
  CALL_C(0x3053, objectRunMovementScript_body_hook, 0x6b4c, 0x3056);
  bank_pop(gb, 0x3056);
  CYC(0x305c, 0x305d);
  ret_effect(gb);
}

void decCbb3_hook(GB *gb) {
  SET_HL(wFakeResetMenu);
  CYC(0x305d, 0x3061); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x3061, 0x3062);
  ret_effect(gb);
}

void incCbc1_hook(GB *gb) {
  SET_HL(wGenericCutscene_cbc1);
  CYC(0x3062, 0x3066); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x3066, 0x3067);
  ret_effect(gb);
}

void incCbc2_hook(GB *gb) {
  SET_HL(wGenericCutscene_cbc2);
  CYC(0x3067, 0x306b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x306b, 0x306c);
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
  add_sprites_from_bank(gb, 0x30d8, ROM_addSpritesToOam_withOffset);
  ret_effect(gb);
}

void addSpritesFromBankToOam_hook(GB *gb) {
  add_sprites_from_bank(gb, 0x30eb, ROM_addSpritesToOam);
  ret_effect(gb);
}

// input

void pollInput_hook(GB *gb) {
  C = 0x00;
  A = 0x20;
  CYC(0x026d, 0x0272); mem_wr(gb, IO_P1, A);
  CYC(0x0272, 0x0273); A = mem_rd(gb, IO_P1);
  CYC(0x0273, 0x0274); A = mem_rd(gb, IO_P1);
  CYC(0x0274, 0x0275); A = mem_rd(gb, IO_P1);
  B = A;
  A = 0x10;
  CYC(0x0275, 0x0279); mem_wr(gb, IO_P1, A);
  A = B;
  alu_and(gb, 0x0f);
  alu_swap_a(gb);
  B = A;
  SET_HL(wKeysPressed);
  CYC(0x0279, 0x0283); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x0283, 0x0284); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_cpl(gb);
  CYC(0x0284, 0x0286); mem_wr(gb, HL, A);
  CYC(0x0286, 0x0287); A = mem_rd(gb, IO_P1);
  CYC(0x0287, 0x0288); A = mem_rd(gb, IO_P1);
  alu_and(gb, 0x0f);
  alu_or(gb, B);
  alu_cpl(gb);
  CYC(0x0288, 0x028d); B = mem_rd(gb, HL);
  CYC(0x028d, 0x028e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_and(gb, B);
  CYC(0x028e, 0x0290); mem_wr(gb, HL, A);
  A = 0x30;
  CYC(0x0290, 0x0293); mem_wr(gb, IO_P1, A);
  CYC(0x0293, 0x0294);
  ret_effect(gb);
}

void getInputWithAutofire_hook(GB *gb) {
  uint16_t hl = HL, bc = BC;
  CYC(0x0294, 0x0299); A = W8(wKeysPressed);
  alu_and(gb, 0xf0);
  B = A;
  SET_HL(wAutoFireKeysPressed);
  CYC(0x0299, 0x02a0); A = mem_rd(gb, HL);
  alu_and(gb, B);
  A = B;
  CYC(0x02a0, 0x02a3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  bool just_pressed;
  if (F & FZ) {
    CYCT(0x02a3, 0x02a5);
    alu_xor(gb, A);
    CYC(0x02b9, 0x02bb); mem_wr(gb, HL, A);
    just_pressed = true;
  } else {
    CYC(0x02a3, 0x02a6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x02a6, 0x02a7); A = mem_rd(gb, HL);
    alu_cp(gb, 0x28);
    if (F & FC) { CYCT(0x02a7, 0x02ab); just_pressed = true; }
    else {
      CYC(0x02a7, 0x02ab);
      alu_and(gb, 0x1f);
      alu_or(gb, 0x80);
      CYC(0x02ab, 0x02b0); mem_wr(gb, HL, A);
      alu_and(gb, 0x03);
      if (!(F & FZ)) { CYCT(0x02b0, 0x02b4); just_pressed = true; }
      else {
        CYC(0x02b0, 0x02b7); A = W8(wKeysPressed);
        CYC(0x02b7, 0x02b9);
        just_pressed = false;
      }
    }
  }
  if (just_pressed) { CYC(0x02bb, 0x02be); A = W8(wKeysJustPressed); }
  SET_BC(bc);
  SET_HL(hl);
  CYC(0x02be, 0x02c1);
  ret_effect(gb);
}

// graphics register states

void loadGfxRegisterStateIndex_hook(GB *gb) {
  L = A;
  alu_add(gb, A);
  alu_add(gb, L);
  alu_add(gb, A);
  SET_HL(gfxRegisterStates);
  CYC(0x02ea, 0x02f2);
  add_double_index_to_hl(gb);
  B = 0x0c;
  SET_DE(wGfxRegs1);
  CYC(0x02f2, 0x02f7);
  do {
    CYC(0x02f7, 0x02f8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x02f8, 0x02f9); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    B = alu_dec8(gb, B);
    if (B) CYCT(0x02f9, 0x02fd); else CYC(0x02f9, 0x02fd);
  } while (B);
  CYC(0x02fd, 0x0300); A = W8(wGfxRegs1);
  CYC(0x0300, 0x0303); W8(wGfxRegsFinal) = A;
  CYC(0x0303, 0x0305); mem_wr(gb, IO_LCDC, A);
  CYC(0x0305, 0x0306);
  ret_effect(gb);
}

// DMA transfers: queued for VBlank while the LCD is on, done at once (GDMA) while it is off

static void queue_dma_transfer(GB *gb) {
  CYC(0x058a, 0x058c); A = mem_rd(gb, IO_LCDC);
  alu_rlca(gb);
  if (F & FC) {
    CYC(0x058c, 0x058f);
    uint16_t de = DE, hl = HL;
    H = 0xc4;
    CYC(0x058f, 0x0595); A = H8(hVBlankFunctionQueueTail);
    L = A;
    CYC(0x0595, 0x0599); A = mem_rd(gb, vblankDmaFunctionOffset);
    CYC(0x0599, 0x059a); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = C;
    CYC(0x059a, 0x059c); mem_wr(gb, HL, A); SET_HL(HL + 1);
    SET_DE(hl);
    A = D;
    CYC(0x059c, 0x059f); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = E;
    CYC(0x059f, 0x05a1); mem_wr(gb, HL, A); SET_HL(HL + 1);
    SET_DE(de);
    A = E;
    CYC(0x05a1, 0x05a4); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = D;
    CYC(0x05a4, 0x05a6); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = E;
    CYC(0x05a6, 0x05a8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = B;
    CYC(0x05a8, 0x05aa); mem_wr(gb, HL, A); SET_HL(HL + 1);
    A = L;
    CYC(0x05aa, 0x05ad); H8(hVBlankFunctionQueueTail) = A;
    alu_scf(gb);
    CYC(0x05ad, 0x05af);
    return;
  }
  CYCT(0x058c, 0x058f);
  CYC(0x05af, 0x05b1); A = H8(hRomBank);
  CYC(0x05b1, 0x05b2); push_effect(gb, AF);
  CYC(0x05b2, 0x05b4); A = mem_rd(gb, IO_SVBK);
  CYC(0x05b4, 0x05b5); push_effect(gb, AF);
  CYC(0x05b5, 0x05b6); push_effect(gb, DE);
  CYC(0x05b6, 0x05b7); push_effect(gb, HL);
  A = C;
  CYC(0x05b7, 0x05ba); mem_wr(gb, IO_SVBK, A);
  CYC(0x05ba, 0x05bc); H8(hRomBank) = A;
  CYC(0x05bc, 0x05bf); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x05bf, 0x05c0); SET_DE(pop_effect(gb));
  SET_HL(IO_HDMA1);
  CYC(0x05c0, 0x05c4); mem_wr(gb, HL, D);
  L = alu_inc8(gb, L);
  CYC(0x05c4, 0x05c6); mem_wr(gb, HL, E);
  L = alu_inc8(gb, L);
  CYC(0x05c6, 0x05c8); SET_DE(pop_effect(gb));
  A = E;
  CYC(0x05c8, 0x05cb); mem_wr(gb, IO_VBK, A);
  CYC(0x05cb, 0x05cc); mem_wr(gb, HL, D);
  L = alu_inc8(gb, L);
  CYC(0x05cc, 0x05ce); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x05ce, 0x05cf); mem_wr(gb, HL, B);
  CYC(0x05cf, 0x05d0); SET_AF(pop_effect(gb));
  CYC(0x05d0, 0x05d2); mem_wr(gb, IO_SVBK, A);
  CYC(0x05d2, 0x05d3); SET_AF(pop_effect(gb));
  CYC(0x05d3, 0x05d5); H8(hRomBank) = A;
  CYC(0x05d5, 0x05d8); mem_wr(gb, MBC_ROM_BANK, A);
  alu_xor(gb, A);
  CYC(0x05d8, 0x05da);
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
  load_gfx_header(gb, 0x05da, uncmpGfxHeaderTable_bank01, false);
  ret_effect(gb);
}

void loadGfxHeader_hook(GB *gb) {
  load_gfx_header(gb, 0x0626, gfxHeaderTable_bank01, true);
  ret_effect(gb);
}

// tilesets (bank 1 tables); loadTilesetHlpr stays transliterated

void loadTileset_hook(GB *gb) {
  E = A;
  CYC(0x0799, 0x079c); C = mem_rd(gb, IO_SVBK);
  CYC(0x079c, 0x079f); B = H8(hRomBank);
  uint16_t bc = BC;
  CYC(0x079f, 0x07a5); H8(hRomBank) = 0x01;
  CYC(0x07a5, 0x07a8); mem_wr(gb, MBC_ROM_BANK, 0x01);
  A = E;
  SET_HL(tilesetHeaderTable_bank01);
  CYC(0x07a8, 0x07ad);
  add_double_index_to_hl(gb);
  CYC(0x07ad, 0x07ae); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x07ae, 0x07af); H = mem_rd(gb, HL);
  L = A;
  CYC(0x07af, 0x07b0);
  for (;;) {
    CYC(0x07b0, 0x07b1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    uint16_t hl = HL;
    SET_HL(tilesetGfxHeaderTable_bank01);
    CYC(0x07b1, 0x07b6);
    add_double_index_to_hl(gb);
    CYC(0x07b6, 0x07b7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x07b7, 0x07b8); H = mem_rd(gb, HL);
    L = A;
    CYC(0x07b8, 0x07ba); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x07ba, 0x07bc); H8(hFF8F) = A;
    CYC(0x07bc, 0x07bd); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x07bd, 0x07bf); H8(hFF91) = A;
    CYC(0x07bf, 0x07c0); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x07c0, 0x07c2); H8(hFF90) = A;
    SET_HL(hl);
    CYC(0x07c2, 0x07c4); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x07c4, 0x07c6); H8(hFF8E) = A;
    CYC(0x07c6, 0x07c7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    D = A;
    CYC(0x07c7, 0x07c9); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    uint16_t src = DE;
    CYC(0x07c9, 0x07cc); A = mem_rd(gb, HL); SET_HL(HL + 1);
    D = A;
    CYC(0x07cc, 0x07ce); A = mem_rd(gb, HL); SET_HL(HL + 1);
    E = A;
    CYC(0x07ce, 0x07d0); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_and(gb, 0x7f);
    CYC(0x07d0, 0x07d4); H8(hFF8D) = A;
    CYC(0x07d4, 0x07d5); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(0x07d5, 0x07d7); H8(hFF8C) = A;
    A = H;
    CYC(0x07d7, 0x07da); H8(hFF93) = A;
    A = L;
    CYC(0x07da, 0x07dd); H8(hFF92) = A;
    SET_HL(src);
    CYC(0x07dd, 0x07de);
    CALL_ROM(0x07de, ROM_loadTilesetHlpr);
    CYC(0x07e1, 0x07e5); H8(hRomBank) = 0x01;
    CYC(0x07e5, 0x07e8); mem_wr(gb, MBC_ROM_BANK, 0x01);
    CYC(0x07e8, 0x07ea); A = H8(hFF93);
    H = A;
    CYC(0x07ea, 0x07ed); A = H8(hFF92);
    L = A;
    CYC(0x07ed, 0x07ef); A = mem_rd(gb, HL); SET_HL(HL + 1);
    SET_HL(HL + 1);
    alu_add(gb, A);
    if (F & FC) { CYCT(0x07ef, 0x07f3); continue; }
    CYC(0x07ef, 0x07f3);
    break;
  }
  SET_BC(bc);
  A = B;
  CYC(0x07f3, 0x07f7); H8(hRomBank) = A;
  CYC(0x07f7, 0x07fa); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(0x07fa, 0x07fd); mem_wr(gb, IO_SVBK, A);
  CYC(0x07fd, 0x07fe);
  ret_effect(gb);
}

// animation frame advance, shared by interactions, enemies and parts: the animation data
// pointer (counter + 1..3) walks frames of (duration, oam index, flags); $ff loops back.

static void next_animation_frame(GB *gb, uint16_t o, uint8_t counter, uint8_t id, uint16_t table, uint8_t oam) {
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
  if (o == 0x2647) {
    alu_or(gb, 0x40);
    CYC(0x2674, 0x2679); mem_wr(gb, DE, A);
    CYC(0x2679, 0x267a); SET_AF(pop_effect(gb));
    CYC(0x267a, 0x267c); H8(hRomBank) = A;
    CYC(0x267c, 0x267f); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(0x267f, 0x2680);
  } else if (o == 0x2844) {
    CYC(0x2871, 0x2874); mem_wr(gb, DE, A);
    CYC(0x2874, 0x2875); SET_AF(pop_effect(gb));
    CYC(0x2875, 0x2877); H8(hRomBank) = A;
    CYC(0x2877, 0x287a); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(0x287a, 0x287b);
  } else {
    alu_or(gb, 0x40);
    CYC(0x29cb, 0x29d0); mem_wr(gb, DE, A);
    A = 0x11;
    CYC(0x29d0, 0x29d4); H8(hRomBank) = A;
    CYC(0x29d4, 0x29d7); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(0x29d7, 0x29d8);
  }
}

void _interactionNextAnimationFrame_hook(GB *gb) { next_animation_frame(gb, 0x2647, 0x60, 0x41, interactionAnimationFrameTable_bank16, 0x5e); ret_effect(gb); }
void _enemyNextAnimationFrame_hook(GB *gb) { next_animation_frame(gb, 0x2844, 0xa0, 0x81, enemyAnimationFrameTable_bank0d, 0x9e); ret_effect(gb); }
void _partNextAnimationFrame_hook(GB *gb) { next_animation_frame(gb, 0x299e, 0xe0, 0xc1, partAnimationFrameTable_bank16, 0xde); ret_effect(gb); }

// sound driver control and the timer it runs from

static void disable_timer(GB *gb) {
  SET_HL(hFFB7);
  CYC(0x0cdd, 0x0ce2); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  alu_xor(gb, A);
  CYC(0x0ce2, 0x0ce5); mem_wr(gb, IO_TAC, A);
  CYC(0x0ce5, 0x0ce6);
}

void disableTimer_hook(GB *gb) {
  disable_timer(gb);
  ret_effect(gb);
}

static void enable_timer(GB *gb) {
  alu_xor(gb, A);
  CYC(0x0ce6, 0x0ce9); mem_wr(gb, IO_TAC, A);
  A = 0xa0;
  CYC(0x0ce9, 0x0ced); H8(hMusicQueueTail) = A;
  CYC(0x0ced, 0x0cef); H8(hMusicQueueHead) = A;
  CYC(0x0cef, 0x0cf1); A = mem_rd(gb, IO_KEY1);
  alu_rlca(gb);
  A = 0x77;
  if (F & FC) CYCT(0x0cf1, 0x0cf6);
  else { CYC(0x0cf1, 0x0cf8); A = 0xbb; }
  SET_HL(IO_TIMA);
  CYC(0x0cf8, 0x0cfc); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x0cfc, 0x0cfd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  alu_xor(gb, A);
  CYC(0x0cfd, 0x0cff); mem_wr(gb, HL, A);
  CYC(0x0cff, 0x0d01); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  SET_HL(hFFB7);
  CYC(0x0d01, 0x0d06); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfe);
  CYC(0x0d06, 0x0d07);
}

void enableTimer_hook(GB *gb) {
  enable_timer(gb);
  ret_effect(gb);
}

static void start_sound(GB *gb) {
  CYC(0x0cba, 0x0cbb); push_effect(gb, DE);
  CYC(0x0cbb, 0x0cbd); A = H8(hRomBank);
  CYC(0x0cbd, 0x0cbe); push_effect(gb, AF);
  CYC(0x0cbe, 0x0cc1);
  disable_timer(gb);
  A = 0x39;
  CYC(0x0cc1, 0x0cc5); H8(hSoundDataBaseBank) = A;
  CYC(0x0cc5, 0x0cc7); H8(hSoundDataBaseBank2) = A;
  CYC(0x0cc7, 0x0cc9); H8(hRomBank) = A;
  CYC(0x0cc9, 0x0ccc); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_ROM(0x0ccc, ROM_jpBc);
  CYC(0x0ccf, 0x0cd2);
  enable_timer(gb);
  CYC(0x0cd2, 0x0cd3); SET_AF(pop_effect(gb));
  CYC(0x0cd3, 0x0cd5); H8(hRomBank) = A;
  CYC(0x0cd5, 0x0cd8); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x0cd8, 0x0cd9); SET_DE(pop_effect(gb));
  CYC(0x0cd9, 0x0cda);
}

void _startSound_hook(GB *gb) {
  start_sound(gb);
  ret_effect(gb);
}

void restartSound_hook(GB *gb) {
  SET_BC(0x4009);
  CYC(0x0cb2, 0x0cb7);
  start_sound(gb);
  ret_effect(gb);
}

// sprites into wOam: each entry y, x, tile, attributes with a y/x offset in bc

static void add_sprites_to_oam_with_offset(GB *gb) {
  CYC(0x0d61, 0x0d63); A = H8(hOamTail);
  alu_cp(gb, 0xa0);
  CYC(0x0d63, 0x0d65);
  if (!(F & FC)) { CYCT(0x0d65, 0x0d66); return; }
  CYC(0x0d65, 0x0d66);
  E = A;
  D = wOam >> 8;
  CYC(0x0d66, 0x0d6a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x0d6a, 0x0d6c); return; }
  CYC(0x0d6a, 0x0d6c);
  for (;;) {
    CYC(0x0d6c, 0x0d6e); H8(hFF8B) = A;
    CYC(0x0d6e, 0x0d6f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, B);
    alu_cp(gb, 0xa0);
    bool skip = false;
    if (!(F & FC)) {
      CYCT(0x0d6f, 0x0d74);
      CYC(0x0d92, 0x0d93); SET_HL(HL + 1);
      skip = true;
    } else {
      CYC(0x0d6f, 0x0d75); mem_wr(gb, DE, A);
      CYC(0x0d75, 0x0d76); A = mem_rd(gb, HL); SET_HL(HL + 1);
      alu_add(gb, C);
      alu_cp(gb, 0xa8);
      if (!(F & FC)) { CYCT(0x0d76, 0x0d7b); skip = true; }
    }
    if (skip) {
      SET_HL(HL + 2);
      A = 0xe0;
      CYC(0x0d93, 0x0d98); mem_wr(gb, DE, A);
      CYC(0x0d98, 0x0d9a);
    } else {
      CYC(0x0d76, 0x0d7b);
      E = alu_inc8(gb, E);
      CYC(0x0d7b, 0x0d7d); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      CYC(0x0d7d, 0x0d7f); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0d7f, 0x0d80); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      CYC(0x0d80, 0x0d82); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0d82, 0x0d83); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      A = E;
      alu_cp(gb, 0xa0);
      if (!(F & FC)) { CYCT(0x0d83, 0x0d89); break; }
      CYC(0x0d83, 0x0d89);
    }
    CYC(0x0d89, 0x0d8b); A = H8(hFF8B);
    A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(0x0d8b, 0x0d8e); continue; }
    CYC(0x0d8b, 0x0d8f);
    A = E;
    break;
  }
  CYC(0x0d8f, 0x0d91); H8(hOamTail) = A;
  CYC(0x0d91, 0x0d92);
}

void addSpritesToOam_withOffset_hook(GB *gb) {
  add_sprites_to_oam_with_offset(gb);
  ret_effect(gb);
}

void addSpritesToOam_hook(GB *gb) {
  SET_BC(0);
  CYC(0x0d5e, 0x0d61);
  add_sprites_to_oam_with_offset(gb);
  ret_effect(gb);
}

// draw queue: objects sorted into four priority lists in wObjectsToDraw

static void object_queue_draw(GB *gb) {
  CYC(0x10a8, 0x10a9); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x10a9, 0x10ab); return; }
  CYC(0x10a9, 0x10ab);
  A = E;
  alu_or(gb, OBJ_VISIBLE);
  L = A;
  H = D;
  CYC(0x10ab, 0x10b1); A = mem_rd(gb, HL);
  alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x10b1, 0x10b4); return; }
  CYC(0x10b1, 0x10b4);
  alu_and(gb, 0x03);
  H = A;
  alu_add(gb, 0xa1);
  C = A;
  CYC(0x10b4, 0x10bb); A = mem_rd(gb, IO_P1 | C);
  alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x10bb, 0x10be); return; }
  CYC(0x10bb, 0x10be);
  A = alu_inc8(gb, A);
  CYC(0x10be, 0x10c0); mem_wr(gb, IO_P1 | C, A);
  A = alu_dec8(gb, A);
  H = alu_swap(gb, H);
  alu_add(gb, H);
  alu_add(gb, A);
  L = A;
  H = wObjectsToDraw >> 8;
  A = B;
  CYC(0x10c0, 0x10ca); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x10ca, 0x10cb); mem_wr(gb, HL, D);
  CYC(0x10cb, 0x10cc);
}

void objectQueueDraw_hook(GB *gb) {
  object_queue_draw(gb);
  ret_effect(gb);
}

static void queue_draw_objects(GB *gb) {
  for (;;) {
    CYC(0x109e, 0x10a1);
    object_queue_draw(gb);
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x10a1, 0x10a7); continue; }
    CYC(0x10a1, 0x10a8);
    break;
  }
}

static void queue_draw_everything(GB *gb) {
  SET_HL(hTerrainEffectsBufferUsedSize);
  alu_xor(gb, A);
  CYC(0x1078, 0x107d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x107d, 0x107e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x107e, 0x107f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x107f, 0x1080); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x1080, 0x1081); mem_wr(gb, HL, A); SET_HL(HL + 1);
  SET_DE(w1WeaponItem);
  B = 0x0b;
  CYC(0x1081, 0x1089);
  queue_draw_objects(gb);
  SET_DE(ENEMY_SLOTS);
  B = 0x8b;
  CYC(0x1089, 0x1091);
  queue_draw_objects(gb);
  SET_DE(PART_SLOTS);
  B = 0xcb;
  CYC(0x1091, 0x1099);
  queue_draw_objects(gb);
  SET_DE(w1ReservedInteraction0);
  B = 0x4b;
  CYC(0x1099, 0x109e);
  queue_draw_objects(gb);
}

void queueDrawEverything_hook(GB *gb) {
  queue_draw_everything(gb);
  ret_effect(gb);
}

// tileset animation and room entry

void loadTilesetAnimation_hook(GB *gb) {
  CYC(0x1374, 0x1377); A = W8(wLoadedTilesetAnimation);
  B = A;
  CYC(0x1377, 0x137b); A = W8(wTilesetAnimation);
  alu_cp(gb, B);
  if (F & FZ) { CYCT(0x137b, 0x137d); ret_effect(gb); return; }
  CYC(0x137b, 0x137d);
  CYC(0x137d, 0x1380); W8(wLoadedTilesetAnimation) = A;
  CYC(0x1380, 0x1383);
  load_animation_data(gb);
  ret_effect(gb);
}

void func_1135_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x1135, 0x1139); W8(wDeathRespawnBuffer_rememberedCompanionGroup) = A;
  CYC(0x1139, 0x113a);
  ret_effect(gb);
}

void func_131f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  alu_xor(gb, A);
  CYC(0x131f, 0x1323); W8(wScreenOffsetY) = A;
  CYC(0x1323, 0x1326); W8(wScreenOffsetX) = A;
  bank_push(gb, 0x1326, 0x01);
  CALL_ROM(0x1330, ROM_b01_initializeRoomBoundaryAndLoadAnimations);
  CALL_ROM(0x1333, ROM_b01_setScreenTransitionState02);
  CALL_ROM(0x1336, ROM_loadTilesetAndRoomLayout);
  CYC(0x1339, 0x133c); A = W8(wcddf);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x133c, 0x133f);
    CYC(0x1356, 0x1359);
    load_room_collisions(gb);
    CYC(0x1359, 0x135c);
    generate_vram_tiles_with_room_changes(gb, sp0_);
    A = 0x10;
    CYC(0x135c, 0x1361);
    load_gfx_header(gb, 0x05da, uncmpGfxHeaderTable_bank01, false);
  } else {
    CYC(0x133c, 0x133f);
    SET_HL(ROM_b04_func_04_6ed1);
    E = 0x04;
    CYC(0x133f, 0x1344);
    CALL_ROM(0x1344, ROM_interBankCall);
    SET_HL(ROM_b04_func_04_6f31);
    E = 0x04;
    CYC(0x1347, 0x134c);
    CALL_ROM(0x134c, ROM_interBankCall);
    A = 0x30;
    CYC(0x134f, 0x1354);
    load_gfx_header(gb, 0x05da, uncmpGfxHeaderTable_bank01, false);
    CYC(0x1354, 0x1356);
  }
  CYC(0x1361, 0x1364); A = W8(wTilesetPalette);
  CYC(0x1364, 0x1367); W8(wLoadedTilesetPalette) = A;
  CYC(0x1367, 0x136a); A = W8(wTilesetUniqueGfx);
  CYC(0x136a, 0x136d); W8(wLoadedTilesetUniqueGfx) = A;
  bank_pop(gb, 0x136d);
  CYC(0x1373, 0x1374);
  ret_effect(gb);
}

void func_1383_hook(GB *gb) {
  CYC(0x1383, 0x1384);
  ret_effect(gb);
}

void func_13c6_hook(GB *gb) {
  CYC(0x13c6, 0x13c8); A = H8(hRomBank);
  CYC(0x13c8, 0x13c9); push_effect(gb, AF);
  A = 0x02;
  CYC(0x13c9, 0x13cd); mem_wr(gb, IO_SVBK, A);
  CYC(0x13cd, 0x13ce); push_effect(gb, DE);
  CYC(0x13ce, 0x13cf); push_effect(gb, BC);
  SET_DE(w2ColorComponentBuffer1);
  CYC(0x13cf, 0x13d5);
  extract_color_components(gb);
  CYC(0x13d5, 0x13d6); SET_HL(pop_effect(gb));
  SET_DE(w2ColorComponentBuffer2);
  CYC(0x13d6, 0x13dc);
  extract_color_components(gb);
  CYC(0x13dc, 0x13dd); SET_DE(pop_effect(gb));
  CYC(0x13dd, 0x13de); SET_AF(pop_effect(gb));
  CYC(0x13de, 0x13e0); H8(hRomBank) = A;
  CYC(0x13e0, 0x13e3); mem_wr(gb, MBC_ROM_BANK, A);
  alu_xor(gb, A);
  CYC(0x13e3, 0x13e6); mem_wr(gb, IO_SVBK, A);
  CYC(0x13e6, 0x13e9);
  start_fade_between_two_palettes(gb);
  ret_effect(gb);
}

// text data reads through WRAM bank 7 bank variables

void readByteFromW7ActiveBank_hook(GB *gb) {
  uint16_t bc = BC;
  CYC(0x1949, 0x194d); A = mem_rd(gb, w7ActiveBank);
  CYC(0x194d, 0x194f); H8(hRomBank) = A;
  CYC(0x194f, 0x1952); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x1952, 0x1953); B = mem_rd(gb, HL);
  A = 0x3f;
  CYC(0x1953, 0x1957); H8(hRomBank) = A;
  CYC(0x1957, 0x195a); mem_wr(gb, MBC_ROM_BANK, A);
  A = B;
  SET_BC(bc);
  CYC(0x195a, 0x195d);
  ret_effect(gb);
}

void readByteFromW7TextTableBank_hook(GB *gb) {
  CYC(0x195d, 0x195f); A = H8(hRomBank);
  CYC(0x195f, 0x1960); push_effect(gb, AF);
  CYC(0x1960, 0x1963); A = mem_rd(gb, w7TextTableBank);
  alu_bit(gb, 7, H);
  if (F & FZ) CYCT(0x1963, 0x1967);
  else {
    CYC(0x1963, 0x196c);
    H = (uint8_t)((H & 0x7f) | 0x40);
    A = alu_inc8(gb, A);
  }
  CYC(0x196c, 0x196e); H8(hRomBank) = A;
  CYC(0x196e, 0x1971); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x1971, 0x1972); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1972, 0x1974); H8(hFF8B) = A;
  CYC(0x1974, 0x1975); SET_AF(pop_effect(gb));
  CYC(0x1975, 0x1977); H8(hRomBank) = A;
  CYC(0x1977, 0x197a); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x197a, 0x197c); A = H8(hFF8B);
  CYC(0x197c, 0x197d);
  ret_effect(gb);
}

// secrets and menus (bank 2 and 3 bodies)

void secretFunctionCaller_hook(GB *gb) {
  bank_push(gb, 0x1a2e, 0x03);
  CALL_ROM(0x1a38, ROM_b03_secretFunctionCaller_body);
  bank_pop(gb, 0x1a3b);
  A = B;
  alu_or(gb, A);
  CYC(0x1a41, 0x1a44);
  ret_effect(gb);
}

static void open_menu_tail(GB *gb, uint16_t sp0_) {
  L = A;
  CYC(0x1aba, 0x1abd); C = mem_rd(gb, IO_SVBK);
  CYC(0x1abd, 0x1ac0); B = H8(hRomBank);
  CYC(0x1ac0, 0x1ac2); push_effect(gb, BC);
  A = 0x02;
  CYC(0x1ac2, 0x1ac6); H8(hRomBank) = A;
  CYC(0x1ac6, 0x1ac9); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(0x1ac9, runBank2Function_hook, ROM_b02_runBank2Function, 0x1acc);
  CYC(0x1acc, 0x1acd); SET_BC(pop_effect(gb));
  A = B;
  CYC(0x1acd, 0x1ad0); H8(hRomBank) = A;
  CYC(0x1ad0, 0x1ad3); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(0x1ad3, 0x1ad6); mem_wr(gb, IO_SVBK, A);
  CYC(0x1ad6, 0x1ad7);
}

void openMenu_hook(GB *gb) {
  H = 0x06;
  CYC(0x1ab0, 0x1ab4);
  open_menu_tail(gb, gb->sp);
  ret_effect(gb);
}

void openSecretInputMenu_hook(GB *gb) {
  CYC(0x1a44, 0x1a47); W8(wSecretInputType) = A;
  A = 0x01;
  CYC(0x1a47, 0x1a4c); W8(wTextInputResult) = A;
  A = 0x06;
  CYC(0x1a4c, 0x1a51);
  H = 0x06;
  CYC(0x1ab0, 0x1ab4);
  open_menu_tail(gb, gb->sp);
  ret_effect(gb);
}

void updateMenus_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x1a51, 0x1a53); C = mem_rd(gb, IO_SVBK);
  CYC(0x1a53, 0x1a56); B = H8(hRomBank);
  CYC(0x1a56, 0x1a58); push_effect(gb, BC);
  A = 0x02;
  CYC(0x1a58, 0x1a5c); H8(hRomBank) = A;
  CYC(0x1a5c, 0x1a5f); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(0x1a5f, b2_updateMenus_hook, ROM_b02_updateMenus, 0x1a62);
  CYC(0x1a62, 0x1a63); SET_BC(pop_effect(gb));
  A = B;
  CYC(0x1a63, 0x1a66); H8(hRomBank) = A;
  CYC(0x1a66, 0x1a69); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(0x1a69, 0x1a6c); mem_wr(gb, IO_SVBK, A);
  CYC(0x1a6c, 0x1a6f); A = W8(wOpenedMenuType);
  alu_or(gb, A);
  CYC(0x1a6f, 0x1a71);
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
  uint16_t sp0_ = gb->sp;
  bank3f_push(gb, 0x15fb);
  CALL_C(0x1605, interactionLoadGraphics_hook,
      ROM_b3f_interactionLoadGraphics, 0x1608);
  C = A;
  CYC(0x1608, 0x1609);
  bank_pop_af(gb, 0x1609);
  A = C;
  CYC(0x160f, 0x1613);
  interaction_set_animation(gb);
  ret_effect(gb);
}

static void refresh_object_gfx(GB *gb, uint16_t sp0_) {
  bank3f_push(gb, 0x1618);
  CALL_C(0x1622, refreshObjectGfx_body_hook, ROM_b3f_refreshObjectGfx_body, 0x1625);
  alu_xor(gb, A);
  CYC(0x1625, 0x1629); W8(wLoadedTreeGfxIndex) = A;
  bank_pop_af(gb, 0x1629);
  CYC(0x162f, 0x1630);
}

void refreshObjectGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  refresh_object_gfx(gb, sp0_);
  ret_effect(gb);
}

void refreshLoadedTreeGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x1613, 0x1616); A = W8(wLoadedTreeGfxIndex);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x1616, 0x1618); ret_effect(gb); return; }
  CYC(0x1616, 0x1618);
  refresh_object_gfx(gb, sp0_);
  ret_effect(gb);
}

void loadObjectGfxHeaderToSlot4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  bank3f_push(gb, 0x1644);
  CALL_C(0x164e, loadObjectGfxHeaderToSlot4_body_hook,
      ROM_b3f_loadObjectGfxHeaderToSlot4_body, 0x1651);
  bank_pop_af(gb, 0x1651);
  CYC(0x1657, 0x1658);
  ret_effect(gb);
}

void loadTreeGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  E = A;
  CYC(0x1658, 0x1659);
  bank3f_push(gb, 0x1659);
  CALL_C(0x1663, loadTreeGfx_body_hook, ROM_b3f_loadTreeGfx_body, 0x1666);
  bank_pop_af(gb, 0x1666);
  CYC(0x166c, 0x166d);
  ret_effect(gb);
}

// $20 tiles of object gfx from the 3-byte pointer at hl, through the DMA buffers at 4:dc00/4:de00

static void load_object_gfx2(GB *gb) {
  C = A;
  CYC(0x1686, 0x1687); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1687, 0x1688); L = mem_rd(gb, HL);
  alu_and(gb, 0x7f);
  H = A;
  CYC(0x1688, 0x168f); A = mem_rd(gb, wcc20);
  alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x168f, 0x1692);
    A = D;
    alu_or(gb, 0xd0);
    D = A;
    A = 0x05;
    alu_add(gb, E);
    E = A;
    B = 0x1f;
    CYC(0x16bd, 0x16c7);
    CALL_ROM(0x16c7, ROM_decompressGraphics);
    A = 0x01;
    CYC(0x16ca, 0x16ce); mem_wr(gb, IO_SVBK, A);
    A = 0x3f;
    CYC(0x16ce, 0x16d2); H8(hRomBank) = A;
    CYC(0x16d2, 0x16d5); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(0x16d5, 0x16d6);
    return;
  }
  CYC(0x168f, 0x1693); push_effect(gb, DE);
  CYC(0x1693, 0x1696); A = W8(wcc07);
  alu_xor(gb, 0xff);
  CYC(0x1696, 0x169b); W8(wcc07) = A;
  SET_DE(w4GfxBuf1 + 4);
  if (!(F & FZ)) CYCT(0x169b, 0x16a0);
  else { CYC(0x169b, 0x16a3); SET_DE(w4GfxBuf2 + 4); }
  CYC(0x16a3, 0x16a4); push_effect(gb, DE);
  B = 0x1f;
  CYC(0x16a4, 0x16a6);
  CALL_ROM(0x16a6, ROM_decompressGraphics);
  CYC(0x16a9, 0x16aa); SET_HL(pop_effect(gb));
  CYC(0x16aa, 0x16ab); SET_DE(pop_effect(gb));
  C = 0x04;
  A = 0x01;
  CYC(0x16ab, 0x16b1); mem_wr(gb, IO_SVBK, A);
  A = 0x3f;
  CYC(0x16b1, 0x16b5); H8(hRomBank) = A;
  CYC(0x16b5, 0x16b8); mem_wr(gb, MBC_ROM_BANK, A);
  B = 0x1f;
  CYC(0x16b8, 0x16bd);
  queue_dma_transfer(gb);
}

void loadObjectGfx2_hook(GB *gb) {
  load_object_gfx2(gb);
  ret_effect(gb);
}

void loadObjectGfx_hook(GB *gb) {
  D = B;
  E = 0x00;
  CYC(0x1682, 0x1686); A = mem_rd(gb, HL); SET_HL(HL + 1);
  load_object_gfx2(gb);
  ret_effect(gb);
}

// small state setters

void clearFollowingLinkObject_hook(GB *gb) {
  SET_HL(wFollowingLinkObjectType);
  alu_xor(gb, A);
  CYC(0x1846, 0x184b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x184b, 0x184c); mem_wr(gb, HL, A);
  CYC(0x184c, 0x184d);
  ret_effect(gb);
}

void setWarpDestVariables_hook(GB *gb) {
  uint16_t de = DE;
  SET_DE(wWarpDestVariables);
  B = 0x05;
  CYC(0x1997, 0x19a0);
  copyMemory(gb, DE, HL, 5);
  SET_HL(HL + 5);
  SET_DE(de);
  CYC(0x19a0, 0x19a2);
  ret_effect(gb);
}

void setInstrumentsDisabledCounterAndScrollMode_hook(GB *gb) {
  A = 0x08;
  CYC(0x19a2, 0x19a7); W8(wInstrumentsDisabledCounter) = A;
  A = 0x01;
  CYC(0x19a7, 0x19ac); W8(wScrollMode) = A;
  CYC(0x19ac, 0x19ad);
  ret_effect(gb);
}

// text thread control: the tails hand off to the thread kernel

void _label_00_203_hook(GB *gb);
void _label_00_204_hook(GB *gb);

void stopTextThread_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x184d, 0x1851); W8(wTextIsActive) = A;
  CYC(0x1851, 0x1854); W8(wTextboxFlags) = A;
  A = 0xf0;
  CYC(0x1854, 0x1859);
  hook_handoff(gb, ROM_threadStop);
}

static void show_text_tail(GB *gb) {
  CYC(0x1876, 0x1879); A = W8(wTextboxFlags);
  alu_or(gb, L);
  CYC(0x1879, 0x187d); W8(wTextboxFlags) = A;
  A = B;
  alu_add(gb, 0x04);
  B = A;
  SET_HL(wTextDisplayMode);
  CYC(0x187d, 0x1885); mem_wr(gb, HL, E);
  L = alu_inc8(gb, L);
  CYC(0x1885, 0x1887); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  A = B;
  CYC(0x1887, 0x188a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x188a, 0x188b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x188b, 0x188d); mem_wr(gb, HL, 0xff);
  L = alu_inc8(gb, L);
  CYC(0x188d, 0x1890); mem_wr(gb, HL, 0x02);
  L = alu_inc8(gb, L);
  CYC(0x1890, 0x1893); mem_wr(gb, HL, 0x98);
  A = 0x01;
  CYC(0x1893, 0x1898); W8(wTextIsActive) = A;
  CYC(0x1898, 0x189b); SET_BC(0x18a0);
  CYC(0x189b, 0x189d); A = 0xf0;
  CYC(0x189d, 0x18a0);
  threadRestart_hook(gb);
}

void _label_00_203_hook(GB *gb) {
  CYC(0x1874, 0x1876); E = 0x00;
  show_text_tail(gb);
}

void _label_00_204_hook(GB *gb) {
  show_text_tail(gb);
}

void showText_hook(GB *gb) {
  L = 0x00;
  CYC(0x1872, 0x1874);
  _label_00_203_hook(gb);
}

void showTextNonExitable_hook(GB *gb) {
  L = 0x02;
  CYCT(0x186e, 0x1872);
  _label_00_203_hook(gb);
}

void showTextOnInventoryMenu_hook(GB *gb) {
  CYC(0x1860, 0x1863); A = W8(wTextboxFlags);
  A |= 0x01;
  CYC(0x1863, 0x1868); W8(wTextboxFlags) = A;
  L = 0x00;
  E = 0x02;
  CYCT(0x1868, 0x186e);
  _label_00_204_hook(gb);
}

void clearAllItemsAndPutLinkOnGround_hook(GB *gb) {
  uint16_t de = DE;
  CYC(0x19ad, 0x19ae);
  CALL_ROM(0x19ae, ROM_clearAllParentItems);
  CALL_ROM(0x19b1, ROM_dropLinkHeldItem);
  alu_xor(gb, A);
  CYC(0x19b4, 0x19b8); W8(wIsSeedShooterInUse) = A;
  SET_DE(w1WeaponItem);
  CYC(0x19b8, 0x19bb);
  for (;;) {
    H = D;
    L = 0x01;
    CYC(0x19bb, 0x19bf); A = mem_rd(gb, HL);
    alu_cp(gb, 0x18);
    if (!(F & FZ)) {
      CYCT(0x19bf, 0x19c3);
      L = E;
      B = 0x40;
      CYC(0x19cf, 0x19d2);
      CALL_ROM(0x19d2, ROM_clearMemory);
    } else {
      CYC(0x19bf, 0x19c3);
      L = 0x2f;
      CYC(0x19c3, 0x19c7); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
      CYC(0x19c7, 0x19c9); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
      L = 0x1a;
      CYC(0x19c9, 0x19cd); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
      CYCT(0x19cd, 0x19cf);
    }
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x19d5, 0x19db); continue; }
    CYC(0x19d5, 0x19db);
    break;
  }
  SET_DE(de);
  CYC(0x19db, 0x19df);
  hook_handoff(gb, ROM_putLinkOnGround);
}

// font tile copies

void copyTextCharacterGfx_hook(GB *gb) {
  CYC(0x19df, 0x19e0); push_effect(gb, HL);
  CYC(0x19e0, 0x19e1); push_effect(gb, BC);
  SET_HL(gfx_font_start_bank1c);
  alu_bit(gb, 0, C);
  if (!(F & FZ)) CYCT(0x19e1, 0x19e8);
  else {
    CYC(0x19e1, 0x19eb);
    SET_HL(gfx_font_jp_bank1c);
    alu_cp(gb, 0x0e);
    if (!(F & FC)) CYCT(0x19eb, 0x19ef);
    else { CYC(0x19eb, 0x19f1); A = 0x20; }
  }
  CYC(0x19f1, 0x19f4);
  multiply_a_by_16(gb);
  alu_add_hl(gb, BC);
  CYC(0x19f4, 0x19f7); A = H8(hRomBank);
  CYC(0x19f7, 0x19f8); push_effect(gb, AF);
  A = 0x1c;
  CYC(0x19f8, 0x19fc); H8(hRomBank) = A;
  CYC(0x19fc, 0x19ff); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x19ff, 0x1a02); A = W8(wFileSelect_fontXor);
  C = A;
  B = 0x10;
  CYC(0x1a02, 0x1a05);
  for (;;) {
    CYC(0x1a05, 0x1a06); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_xor(gb, C);
    CYC(0x1a06, 0x1a08); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    CYC(0x1a08, 0x1a0a); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    B = alu_dec8(gb, B);
    if (B) { CYCT(0x1a0a, 0x1a0e); continue; }
    CYC(0x1a0a, 0x1a0e);
    break;
  }
  CYC(0x1a0e, 0x1a0f); SET_AF(pop_effect(gb));
  CYC(0x1a0f, 0x1a11); H8(hRomBank) = A;
  CYC(0x1a11, 0x1a14); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x1a14, 0x1a15); SET_BC(pop_effect(gb));
  CYC(0x1a15, 0x1a16); SET_HL(pop_effect(gb));
  CYC(0x1a16, 0x1a17);
  ret_effect(gb);
}

void retrieveTextCharacter_hook(GB *gb) {
  uint16_t hl = HL, de = DE, bc = BC;
  CYC(0x18cd, 0x18d3);
  multiply_a_by_16(gb);
  CYC(0x18d3, 0x18d6); A = mem_rd(gb, w7TextGfxSource);
  SET_HL(textGfxSourceTable);
  CYC(0x18d6, 0x18da);
  add_double_index_to_hl(gb);
  CYC(0x18da, 0x18db); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x18db, 0x18dc); H = mem_rd(gb, HL);
  L = A;
  alu_add_hl(gb, BC);
  SET_BC(bc);
  A = 0x1c;
  CYC(0x18dc, 0x18e3); H8(hRomBank) = A;
  CYC(0x18e3, 0x18e6); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_ROM(0x18e6, ROM_retrieveTextCharacter_func_18fd);
  A = 0x3f;
  CYC(0x18e9, 0x18ed); H8(hRomBank) = A;
  CYC(0x18ed, 0x18f0); mem_wr(gb, MBC_ROM_BANK, A);
  alu_xor(gb, A);
  CYC(0x18f0, 0x18f4); mem_wr(gb, w7TextGfxSource, A);
  SET_DE(de);
  SET_HL(hl);
  CYC(0x18f4, 0x18f7);
  ret_effect(gb);
}

// indirect jumps and the cross-bank call trampoline

void jpHl_hook(GB *gb) {
  CYC(0x00a0, 0x00a1);
  hook_handoff(gb, HL);
}

void jpBc_hook(GB *gb) {
  L = C;
  H = B;
  CYC(0x0cda, 0x0cdd);
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
  H = D;
  L = INTERACTION_BASE | OBJ_STATE;
  CYC(0x23f4, 0x23f8); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x23f8, 0x23f9);
  ret_effect(gb);
}

void cpInteractionSubstate_hook(GB *gb) {
  H = D;
  L = INTERACTION_BASE | OBJ_SUBSTATE;
  CYC(0x23f9, 0x23fd); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x23fd, 0x23fe);
  ret_effect(gb);
}

void findByteInGroupTable_hook(GB *gb) {
  E = A;
  CYC(0x1e0f, 0x1e13); A = W8(wActiveGroup);
  CYC(0x1e13, 0x1e14);
  add_double_index_to_hl(gb);
  CYC(0x1e14, 0x1e15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1e15, 0x1e16); H = mem_rd(gb, HL);
  L = A;
  CYC(0x1e16, 0x1e17);
  find_byte_at_hl(gb);
  ret_effect(gb);
}

// angle from (hFF8F,hFF8E) towards (b,c), both offset by 8, as one of 32 directions

static void object_get_relative_angle_with_temp_vars(GB *gb) {
  E = 0x08;
  A = B;
  alu_add(gb, E);
  B = A;
  A = C;
  alu_add(gb, E);
  C = A;
  E = 0x00;
  CYC(0x1eb1, 0x1ebd); A = H8(hFF8F);
  alu_add(gb, 0x08);
  alu_sub(gb, B);
  if (!(F & FC)) CYCT(0x1ebd, 0x1ec2);
  else {
    CYC(0x1ebd, 0x1ec6);
    alu_cpl(gb);
    A = alu_inc8(gb, A);
    E = 0x04;
  }
  H = A;
  CYC(0x1ec6, 0x1ec9); A = H8(hFF8E);
  alu_add(gb, 0x08);
  alu_sub(gb, C);
  if (!(F & FC)) CYCT(0x1ec9, 0x1ece);
  else {
    CYC(0x1ec9, 0x1ed2);
    alu_cpl(gb);
    A = alu_inc8(gb, A);
    E = alu_inc8(gb, E);
    E = alu_inc8(gb, E);
  }
  alu_cp(gb, H);
  if (!(F & FC)) CYCT(0x1ed2, 0x1ed5);
  else {
    CYC(0x1ed2, 0x1ed9);
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
  if (!(F & FC)) CYCT(0x1ed9, 0x1ee7);
  else {
    CYC(0x1ed9, 0x1ee7);
    B = alu_inc8(gb, B);
    alu_add(gb, L);
    alu_cp(gb, H);
    if (!(F & FC)) CYCT(0x1ee7, 0x1eec);
    else {
      CYC(0x1ee7, 0x1eec);
      B = alu_inc8(gb, B);
      alu_add(gb, L);
      alu_cp(gb, H);
      if (!(F & FC)) CYCT(0x1eec, 0x1ef1);
      else {
        CYC(0x1eec, 0x1ef1);
        B = alu_inc8(gb, B);
        alu_add(gb, L);
        alu_cp(gb, H);
        if (!(F & FC)) CYCT(0x1ef1, 0x1ef6);
        else { CYC(0x1ef1, 0x1ef7); B = alu_inc8(gb, B); }
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
  CYC(0x1ef7, 0x1f04); A = mem_rd(gb, HL);
  CYC(0x1f04, 0x1f05);
}

void objectGetRelativeAngleWithTempVars_hook(GB *gb) {
  object_get_relative_angle_with_temp_vars(gb);
  ret_effect(gb);
}

void hObjectCheckCollidedWithLink_hook(GB *gb) {
  uint16_t de = DE;
  D = H;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, 0x0f);
  L = A;
  CYC(0x1c77, 0x1c82);
  check_collided_with_link(gb);
  SET_DE(de);
  CYC(0x1c82, 0x1c84);
  ret_effect(gb);
}

void func_1c84_hook(GB *gb) {
  CYC(0x1c84, 0x1c87); A = W8(w1ReservedItemC);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1c87, 0x1c89); ret_effect(gb); return; }
  CYC(0x1c87, 0x1c89);
  object_h_check_collision_with_link(gb);
  ret_effect(gb);
}

// status bar and other bank 2 menu functions, dispatched through runBank2Function

void checkReloadStatusBarGraphics_hook(GB *gb) {
  SET_HL(wStatusBarNeedsRefresh);
  CYC(0x1a71, 0x1a75); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x1a75, 0x1a77); ret_effect(gb); return; }
  CYC(0x1a75, 0x1a77);
  CYC(0x1a77, 0x1a79); mem_wr(gb, HL, 0x00);
  alu_rrca(gb);
  A = 0x02;
  if (F & FC) CYCT(0x1a79, 0x1a7e);
  else { CYC(0x1a79, 0x1a80); A = 0x03; }
  CYC(0x1a80, 0x1a83);
  load_gfx_header(gb, 0x05da, uncmpGfxHeaderTable_bank01, false);
  ret_effect(gb);
}

static void bank2_menu_function(GB *gb, uint16_t a, uint8_t h, uint16_t sp0_) {
  H = h;
  CYC(a, a + 4);
  open_menu_tail(gb, sp0_);
  ret_effect(gb);
}

void loadCommonGraphics_hook(GB *gb) { bank2_menu_function(gb, 0x1a98, 0x00, gb->sp); }
void updateStatusBar_hook(GB *gb) { bank2_menu_function(gb, 0x1a9c, 0x01, gb->sp); }
void hideStatusBar_hook(GB *gb) { bank2_menu_function(gb, 0x1aa0, 0x02, gb->sp); }
void showStatusBar_hook(GB *gb) { bank2_menu_function(gb, 0x1aa4, 0x03, gb->sp); }
void saveGraphicsOnEnterMenu_hook(GB *gb) { bank2_menu_function(gb, 0x1aa8, 0x04, gb->sp); }
void reloadGraphicsOnExitMenu_hook(GB *gb) { bank2_menu_function(gb, 0x1aac, 0x05, gb->sp); }
void copyW2TilesetBgPalettesToW4PaletteData_hook(GB *gb) { bank2_menu_function(gb, 0x1ab4, 0x07, gb->sp); }

void copyW4PaletteDataToW2TilesetBgPalettes_hook(GB *gb) {
  H = 0x08;
  CYC(0x1ab8, 0x1aba);
  open_menu_tail(gb, gb->sp);
  ret_effect(gb);
}

void getRoomDungeonProperties_hook(GB *gb) {
  bank_push(gb, 0x1ad7, 0x01);
  CYC(0x1ae1, 0x1ae4); A = W8(wActiveGroup);
  alu_and(gb, 0x01);
  SET_HL(dungeonRoomPropertiesGroupTable_bank01);
  CYC(0x1ae4, 0x1aea);
  add_double_index_to_hl(gb);
  CYC(0x1aea, 0x1aeb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x1aeb, 0x1aec); H = mem_rd(gb, HL);
  L = A;
  A = B;
  CYC(0x1aec, 0x1aef);
  add_a_to_hl(gb);
  CYC(0x1aef, 0x1af0); B = mem_rd(gb, HL);
  bank_pop(gb, 0x1af0);
  CYC(0x1af6, 0x1af7);
  ret_effect(gb);
}

// interaction scripts, treasures and floating images

void interactionFunc_278b_hook(GB *gb) {
  L = 0x58;
  CYC(0x278b, 0x278e); mem_wr(gb, HL, C);
  L = alu_inc8(gb, L);
  CYC(0x278e, 0x2790); mem_wr(gb, HL, B);
  CYC(0x2790, 0x2791);
  ret_effect(gb);
}

void interactionSetMiniScript_hook(GB *gb) {
  E = 0x58;
  A = L;
  CYC(0x2798, 0x279c); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(0x279c, 0x279f); mem_wr(gb, DE, A);
  CYC(0x279f, 0x27a0);
  ret_effect(gb);
}

void objectOscillateZ_hook(GB *gb) {
  bank_push(gb, 0x27a0, 0x09);
  CALL_ROM(0x27aa, ROM_b09_objectOscillateZ_body);
  bank_pop(gb, 0x27ad);
  CYC(0x27b3, 0x27b4);
  ret_effect(gb);
}

static void create_ring_treasure(GB *gb) {
  CYC(0x27c2, 0x27c5);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(0x27c5, 0x27c6); return; }
  CYC(0x27c5, 0x27c6);
  CYC(0x27c6, 0x27c8); mem_wr(gb, HL, 0x60);
  L = alu_inc8(gb, L);
  CYC(0x27c8, 0x27cb); mem_wr(gb, HL, 0x2d);
  L = alu_inc8(gb, L);
  CYC(0x27cb, 0x27cd); mem_wr(gb, HL, C);
  L = 0x78;
  B |= 0x40;
  CYC(0x27cd, 0x27d2); mem_wr(gb, HL, B);
  alu_xor(gb, A);
  CYC(0x27d2, 0x27d4);
}

void createRingTreasure_hook(GB *gb) {
  create_ring_treasure(gb);
  ret_effect(gb);
}

void giveRingToLink_hook(GB *gb) {
  CYC(0x27b4, 0x27b7);
  create_ring_treasure(gb);
  if (!(F & FZ)) { CYCT(0x27b7, 0x27b8); ret_effect(gb); return; }
  CYC(0x27b7, 0x27b8);
  uint16_t de = DE;
  SET_DE(w1Link_yh);
  CYC(0x27b8, 0x27bf);
  copy_pos_raw(gb);
  SET_DE(de);
  alu_xor(gb, A);
  CYC(0x27bf, 0x27c2);
  ret_effect(gb);
}

void createTreasure_hook(GB *gb) {
  CYC(0x27d4, 0x27d7);
  get_free_interaction_slot(gb);
  if (!(F & FZ)) { CYCT(0x27d7, 0x27d8); ret_effect(gb); return; }
  CYC(0x27d7, 0x27d8);
  CYC(0x27d8, 0x27da); mem_wr(gb, HL, 0x60);
  L = alu_inc8(gb, L);
  CYC(0x27da, 0x27dc); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x27dc, 0x27de); mem_wr(gb, HL, C);
  alu_xor(gb, A);
  CYC(0x27de, 0x27e0);
  ret_effect(gb);
}

void objectCreateExclamationMark_hook(GB *gb) {
  CYC(0x27e0, 0x27e2); H8(hFF8B) = A;
  bank_push(gb, 0x27e2, 0x0b);
  CYC(0x27ec, 0x27ee); A = H8(hFF8B);
  CALL_ROM(0x27ee, ROM_b0b_objectCreateExclamationMark_body);
  bank_pop(gb, 0x27f1);
  CYC(0x27f7, 0x27f8);
  ret_effect(gb);
}

void objectCreateFloatingSnore_hook(GB *gb) {
  CYC(0x27f8, 0x27fa); H8(hFF8B) = A;
  A = 0x00;
  CYC(0x27fa, 0x27fe);
  CYC(0x2802, 0x2804); H8(hFF8D) = A;
  bank_push(gb, 0x2804, 0x0b);
  CALL_ROM(0x280e, ROM_b0b_objectCreateFloatingImage);
  bank_pop(gb, 0x2811);
  CYC(0x2817, 0x2818);
  ret_effect(gb);
}

void checkGlobalFlag_hook(GB *gb) {
  SET_HL(wGlobalFlags);
  CYC(0x31f3, 0x31f9);
  check_flag(gb);
  ret_effect(gb);
}

void setGlobalFlag_hook(GB *gb) {
  SET_HL(wGlobalFlags);
  CYC(0x31f9, 0x31ff);
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

void getWildTokayObjectDataIndex_hook(GB *gb) { bank12_table_pointer(gb, 0x3e0f, wildTokayObjectTable_bank12); }
void getEntryFromObjectTable1_hook(GB *gb) { bank12_table_pointer(gb, 0x3080, objectTable1_bank12); }

void enemyCodeNil_hook(GB *gb) {
  CYC(0x3034, 0x3035);
  ret_effect(gb);
}

static void bank2_function_caller(GB *gb, uint16_t a, uint8_t h, uint16_t sp0_) {
  H = h;
  if (a != 0x321d) CYC(a, a + 4); else CYC(a, a + 2);
  L = A;
  CYC(0x321f, 0x3220);
  bank_push(gb, 0x3220, 0x02);
  CALL_C(0x322a, functionCaller_b02_hook, 0x77b2, 0x322d);
  C = alu_rl(gb, C);
  CYC(0x322d, 0x322f);
  bank_pop(gb, 0x322f);
  C = alu_srl(gb, C);
  CYC(0x3235, 0x3238);
  ret_effect(gb);
}

void func_3211_hook(GB *gb) { bank2_function_caller(gb, 0x3211, 0x03, gb->sp); }
void clearEnemiesKilledList_b00_hook(GB *gb) { bank2_function_caller(gb, 0x3205, 0x00, gb->sp); }
void addRoomToEnemiesKilledList_b00_hook(GB *gb) { bank2_function_caller(gb, 0x3209, 0x01, gb->sp); }
void markEnemyAsKilledInRoom_b00_hook(GB *gb) { bank2_function_caller(gb, 0x320d, 0x02, gb->sp); }
void generateRandomBuffer_b00_hook(GB *gb) { bank2_function_caller(gb, 0x3215, 0x04, gb->sp); }
void getRandomPositionForEnemy_b00_hook(GB *gb) { bank2_function_caller(gb, 0x3219, 0x05, gb->sp); }
void checkSpawnTimeportalInteraction_b00_hook(GB *gb) { bank2_function_caller(gb, 0x321d, 0x06, gb->sp); }

// Link grabbing and A-button interaction with nearby objects

void checkGrabbableObjects_hook(GB *gb) {
  CYC(0x1c97, 0x1c9a); A = W8(w1ReservedItemC);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x1c9a, 0x1c9c); ret_effect(gb); return; }
  CYC(0x1c9a, 0x1c9c);
  uint16_t de = DE;
  CYC(0x1c9c, 0x1ca0);
  link_position_plus_direction_offset(gb);
  SET_HL(wGrabbableObjectBuffer);
  CYC(0x1ca0, 0x1ca3);
  for (;;) {
    L = alu_inc8(gb, L);
    CYC(0x1ca3, 0x1ca6); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYC(0x1ca6, 0x1ca9); uint16_t hl = HL;
      L = alu_dec8(gb, L);
      CYC(0x1ca9, 0x1cab); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x1cab, 0x1cac); H = mem_rd(gb, HL);
      CYC(0x1cac, 0x1caf);
      check_collision_with_h_and_d(gb);
      if (F & FC) {
        CYCT(0x1caf, 0x1cb1);
        CYC(0x1cbb, 0x1cbc);
        E = 0x19;
        A = H;
        CYC(0x1cbc, 0x1cc0); mem_wr(gb, DE, A);
        E = alu_dec8(gb, E);
        A = L;
        alu_and(gb, 0xc0);
        CYC(0x1cc0, 0x1cc5); mem_wr(gb, DE, A);
        L = A;
        CYC(0x1cc5, 0x1cc8); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
        alu_add(gb, 0x04);
        L = A;
        CYC(0x1cc8, 0x1ccd); mem_wr(gb, HL, 0x02);
        L = alu_inc8(gb, L);
        CYC(0x1ccd, 0x1cd0); mem_wr(gb, HL, 0x00);
        SET_DE(de);
        alu_scf(gb);
        CYC(0x1cd0, 0x1cd3);
        ret_effect(gb);
        return;
      }
      CYC(0x1caf, 0x1cb2); SET_HL(hl);
    } else CYCT(0x1ca6, 0x1ca8);
    L = alu_inc8(gb, L);
    A = L;
    alu_cp(gb, 0x84);
    if (F & FC) { CYCT(0x1cb2, 0x1cb8); continue; }
    CYC(0x1cb2, 0x1cb8);
    break;
  }
  SET_DE(de);
  alu_xor(gb, A);
  CYC(0x1cb8, 0x1cbb);
  ret_effect(gb);
}

void linkInteractWithAButtonSensitiveObjects_hook(GB *gb) {
  CYC(0x1b5d, 0x1b60); A = W8(wGameKeysJustPressed);
  alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x1b60, 0x1b63); ret_effect(gb); return; }
  CYC(0x1b60, 0x1b63);
  CYC(0x1b63, 0x1b66); A = W8(wAButtonSensitiveObjectListEnd);
  alu_or(gb, A);
  if (!(F & FZ)) CYCT(0x1b66, 0x1b69);
  else {
    CYC(0x1b66, 0x1b6c); A = W8(wLinkGrabState);
    alu_or(gb, A);
    if (!(F & FZ)) { CYCT(0x1b6c, 0x1b6e); ret_effect(gb); return; }
    CYC(0x1b6c, 0x1b6e);
  }
  uint16_t de = DE;
  E = 0x08;
  CYC(0x1b6e, 0x1b72); A = mem_rd(gb, DE);
  SET_HL(linkInteractPositionOffsets);
  CYC(0x1b72, 0x1b76);
  add_double_index_to_hl(gb);
  E = 0x0b;
  CYC(0x1b76, 0x1b79); A = mem_rd(gb, DE);
  CYC(0x1b79, 0x1b7a); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1b7a, 0x1b7c); H8(hFF8D) = A;
  SET_HL(HL + 1);
  E = 0x0d;
  CYC(0x1b7c, 0x1b80); A = mem_rd(gb, DE);
  CYC(0x1b80, 0x1b81); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1b81, 0x1b83); H8(hFF8C) = A;
  SET_DE(wAButtonSensitiveObjectList);
  CYC(0x1b83, 0x1b86);
  bool found = false;
  for (;;) {
    CYC(0x1b86, 0x1b87); A = mem_rd(gb, DE);
    H = A;
    E = alu_inc8(gb, E);
    CYC(0x1b87, 0x1b8a); A = mem_rd(gb, DE);
    L = A;
    alu_or(gb, H);
    if (F & FZ) CYCT(0x1b8a, 0x1b8e);
    else {
      CYC(0x1b8a, 0x1b8f); uint16_t hl = HL;
      CYC(0x1b8f, 0x1b91); A = H8(hFF8D);
      B = A;
      CYC(0x1b91, 0x1b94); A = H8(hFF8C);
      C = A;
      CYC(0x1b94, 0x1b98);
      h_check_contains_point(gb);
      CYC(0x1b98, 0x1b99); SET_HL(hl);
      if (!(F & FC)) CYCT(0x1b99, 0x1b9b);
      else {
        CYC(0x1b99, 0x1b9b);
        CYC(0x1b9b, 0x1b9d); alu_bit(gb, 0, mem_rd(gb, HL));
        if (F & FZ) { CYCT(0x1b9d, 0x1b9f); found = true; break; }
        CYC(0x1b9d, 0x1b9f);
      }
    }
    E = alu_inc8(gb, E);
    A = E;
    alu_cp(gb, 0xd3);
    if (F & FC) { CYCT(0x1b9f, 0x1ba5); continue; }
    CYC(0x1b9f, 0x1ba5);
    break;
  }
  if (!found) {
    SET_DE(de);
    CYC(0x1ba5, 0x1ba7);
    ret_effect(gb);
    return;
  }
  CYC(0x1ba7, 0x1ba9); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  SET_HL(w1Link_invincibilityCounter);
  CYC(0x1ba9, 0x1bad); A = mem_rd(gb, HL);
  alu_or(gb, A);
  A = 0xfc;
  bool write = true;
  if (F & FZ) CYCT(0x1bad, 0x1bb2);
  else {
    CYC(0x1bad, 0x1bb2);
    CYC(0x1bb2, 0x1bb4); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(0x1bb4, 0x1bb6);
      CYC(0x1bbd, 0x1bbe); alu_cp(gb, mem_rd(gb, HL));
      if (!(F & FC)) { CYCT(0x1bbe, 0x1bc0); write = false; }
      else CYC(0x1bbe, 0x1bc0);
    } else {
      CYC(0x1bb4, 0x1bb6);
      A = 0x04;
      CYC(0x1bb6, 0x1bb9); alu_cp(gb, mem_rd(gb, HL));
      if (F & FC) { CYCT(0x1bb9, 0x1bbb); write = false; }
      else { CYC(0x1bb9, 0x1bbb); CYC(0x1bbb, 0x1bbd); }
    }
  }
  if (write) { CYC(0x1bc0, 0x1bc1); mem_wr(gb, HL, A); }
  A = 0x08;
  CYC(0x1bc1, 0x1bc6); W8(wDisableRingTransformations) = A;
  A = 0x80;
  CYC(0x1bc6, 0x1bcb); W8(wForceLinkPushAnimation) = A;
  SET_HL(wLinkTurningDisabled);
  CYC(0x1bcb, 0x1bd0); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  alu_scf(gb);
  SET_DE(de);
  CYC(0x1bd0, 0x1bd3);
  ret_effect(gb);
}

// per-frame object update loops

static void switch_bank(GB *gb, uint16_t a, uint8_t bank) {
  A = bank;
  CYC(a, a + 4); H8(hRomBank) = A;
  CYC(a + 4, a + 7); mem_wr(gb, MBC_ROM_BANK, A);
}

static void update_enemies_if_state_is_zero(GB *gb) {
  A = 0x80;
  CYC(0x2e82, 0x2e86); H8(hActiveObjectType) = A;
  D = 0xd0;
  A = D;
  CYC(0x2e86, 0x2e89);
  for (;;) {
    CYC(0x2e89, 0x2e8b); H8(hActiveObject) = A;
    H = D;
    L = 0x80;
    CYC(0x2e8b, 0x2e8f); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) CYCT(0x2e8f, 0x2e92);
    else {
      CYC(0x2e8f, 0x2e92);
      L = 0x84;
      CYC(0x2e92, 0x2e95); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x2e95, 0x2e96); alu_or(gb, mem_rd(gb, HL));
      if (F & FZ) CALL_ROM_CC(0x2e96, ROM_updateEnemy);
      else CYC(0x2e96, 0x2e99);
      E = 0x9b;
      CYC(0x2e99, 0x2e9c); A = mem_rd(gb, DE);
      E = alu_inc8(gb, E);
      CYC(0x2e9c, 0x2e9e); mem_wr(gb, DE, A);
    }
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x2e9e, 0x2ea4); continue; }
    CYC(0x2e9e, 0x2ea5);
    break;
  }
}

void _updateEnemiesIfStateIsZero_hook(GB *gb) {
  update_enemies_if_state_is_zero(gb);
  ret_effect(gb);
}

void updateEnemies_hook(GB *gb) {
  CYC(0x2ea5, 0x2ea8); A = W8(wScrollMode);
  alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x2ea8, 0x2eac); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x2ea8, 0x2eac);
  CYC(0x2eac, 0x2eaf); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x2eaf, 0x2eb2); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x2eaf, 0x2eb2);
  CYC(0x2eb2, 0x2eb5); A = W8(wDisabledObjects);
  alu_and(gb, 0x84);
  if (!(F & FZ)) { CYCT(0x2eb5, 0x2eb9); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x2eb5, 0x2eb9);
  CYC(0x2eb9, 0x2ebc); A = W8(wPaletteThread_mode);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x2ebc, 0x2ebf); update_enemies_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x2ebc, 0x2ebf);
  A = 0x80;
  CYC(0x2ebf, 0x2ec3); H8(hActiveObjectType) = A;
  D = 0xd0;
  A = D;
  CYC(0x2ec3, 0x2ec6);
  for (;;) {
    CYC(0x2ec6, 0x2ec8); H8(hActiveObject) = A;
    E = 0x80;
    CYC(0x2ec8, 0x2ecb); A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(0x2ecb, 0x2ece);
    else {
      CYC(0x2ecb, 0x2ece);
      CALL_ROM(0x2ece, ROM_updateEnemy);
      H = D;
      L = 0xaa;
      CYC(0x2ed1, 0x2ed6); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
      L = alu_inc8(gb, L);
      CYC(0x2ed6, 0x2ed8); A = mem_rd(gb, HL);
      alu_or(gb, A);
      bool reset = true;
      if (F & FZ) CYCT(0x2ed8, 0x2edb);
      else {
        CYC(0x2ed8, 0x2edb);
        alu_rlca(gb);
        if (F & FC) {
          CYCT(0x2edb, 0x2ede);
          CYC(0x2efb, 0x2efc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
        } else {
          CYC(0x2edb, 0x2ede);
          CYC(0x2ede, 0x2edf); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
          if (F & FZ) CYCT(0x2edf, 0x2ee1);
          else {
            CYC(0x2edf, 0x2ee1);
            CYC(0x2ee1, 0x2ee4); A = W8(wFrameCounter);
            alu_bit(gb, 2, A);
            if (!(F & FZ)) CYCT(0x2ee4, 0x2ee8);
            else {
              CYC(0x2ee4, 0x2ee8);
              B = 0x05;
              L = 0x9b;
              CYC(0x2ee8, 0x2eed); A = mem_rd(gb, HL); SET_HL(HL + 1);
              alu_and(gb, 0x07);
              alu_cp(gb, B);
              if (!(F & FZ)) CYCT(0x2eed, 0x2ef2);
              else { CYC(0x2eed, 0x2ef4); B = 0x02; }
              CYC(0x2ef4, 0x2ef5); A = mem_rd(gb, HL);
              alu_and(gb, 0xf8);
              alu_or(gb, B);
              CYC(0x2ef5, 0x2ef9); mem_wr(gb, HL, A);
              CYC(0x2ef9, 0x2efb);
              reset = false;
            }
          }
        }
      }
      if (reset) {
        L = 0x9b;
        CYC(0x2efc, 0x2eff); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x2eff, 0x2f00); mem_wr(gb, HL, A);
      }
    }
    D = alu_inc8(gb, D);
    A = D;
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x2f00, 0x2f06); continue; }
    CYC(0x2f00, 0x2f07);
    break;
  }
  ret_effect(gb);
}

void updateEnemy_hook(GB *gb) {
  CALL_ROM(0x2f07, ROM_enemyStandardUpdate);
  E = 0x81;
  CYC(0x2f0a, 0x2f0d); A = mem_rd(gb, DE);
  B = 0x0f;
  alu_cp(gb, 0x70);
  if (!(F & FC)) CYCT(0x2f0d, 0x2f13);
  else {
    CYC(0x2f0d, 0x2f13);
    B = alu_dec8(gb, B);
    alu_cp(gb, 0x30);
    if (!(F & FC)) CYCT(0x2f13, 0x2f18);
    else {
      CYC(0x2f13, 0x2f18);
      B = alu_dec8(gb, B);
      alu_cp(gb, 0x08);
      if (!(F & FC)) CYCT(0x2f18, 0x2f1d);
      else { CYC(0x2f18, 0x2f1f); B = 0x10; }
    }
  }
  alu_add(gb, A);
  alu_add(gb, 0x34);
  L = A;
  A = 0x00;
  alu_adc(gb, 0x2f);
  H = A;
  CYC(0x2f1f, 0x2f29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x2f29, 0x2f2a); H = mem_rd(gb, HL);
  L = A;
  A = B;
  CYC(0x2f2a, 0x2f2e); H8(hRomBank) = A;
  CYC(0x2f2e, 0x2f31); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  alu_or(gb, A);
  CYC(0x2f31, 0x2f34);
  hook_handoff(gb, HL);
}

static void update_interactions_if_state_is_zero(GB *gb) {
  A = 0x40;
  CYC(0x3b13, 0x3b17); H8(hActiveObjectType) = A;
  A = 0xd0;
  CYC(0x3b17, 0x3b19);
  for (;;) {
    CYC(0x3b19, 0x3b1b); H8(hActiveObject) = A;
    D = A;
    E = 0x40;
    CYC(0x3b1b, 0x3b1f); A = mem_rd(gb, DE);
    alu_or(gb, A);
    if (F & FZ) CYCT(0x3b1f, 0x3b22);
    else {
      CYC(0x3b1f, 0x3b22);
      alu_rlca(gb);
      bool call = true;
      if (F & FC) CYCT(0x3b22, 0x3b25);
      else {
        CYC(0x3b22, 0x3b25);
        E = 0x44;
        CYC(0x3b25, 0x3b28); A = mem_rd(gb, DE);
        alu_or(gb, A);
        if (!(F & FZ)) { CYCT(0x3b28, 0x3b2b); call = false; }
        else CYC(0x3b28, 0x3b2b);
      }
      if (call) CALL_ROM(0x3b2b, ROM_updateInteraction);
    }
    CYC(0x3b2e, 0x3b30); A = H8(hActiveObject);
    A = alu_inc8(gb, A);
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x3b30, 0x3b35); continue; }
    CYC(0x3b30, 0x3b36);
    break;
  }
}

void _updateInteractionsIfStateIsZero_hook(GB *gb) {
  update_interactions_if_state_is_zero(gb);
  ret_effect(gb);
}

void updateInteractions_hook(GB *gb) {
  CYC(0x3b36, 0x3b39); A = W8(wScrollMode);
  alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(0x3b39, 0x3b3d); update_interactions_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x3b39, 0x3b3d);
  CYC(0x3b3d, 0x3b40); A = W8(wDisabledObjects);
  alu_and(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x3b40, 0x3b44); update_interactions_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x3b40, 0x3b44);
  CYC(0x3b44, 0x3b47); A = W8(wTextIsActive);
  alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x3b47, 0x3b4a); update_interactions_if_state_is_zero(gb); ret_effect(gb); return; }
  CYC(0x3b47, 0x3b4a);
  A = 0x40;
  CYC(0x3b4a, 0x3b4e); H8(hActiveObjectType) = A;
  A = 0xd0;
  CYC(0x3b4e, 0x3b50);
  for (;;) {
    CYC(0x3b50, 0x3b52); H8(hActiveObject) = A;
    D = A;
    E = 0x40;
    CYC(0x3b52, 0x3b56); A = mem_rd(gb, DE);
    alu_or(gb, A);
    CYC(0x3b56, 0x3b57);
    if (!(F & FZ)) CALL_ROM_CC(0x3b57, ROM_updateInteraction);
    else CYC(0x3b57, 0x3b5a);
    CYC(0x3b5a, 0x3b5c); A = H8(hActiveObject);
    A = alu_inc8(gb, A);
    alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x3b5c, 0x3b61); continue; }
    CYC(0x3b5c, 0x3b62);
    break;
  }
  ret_effect(gb);
}

void updateInteraction_hook(GB *gb) {
  E = 0x41;
  CYC(0x3b62, 0x3b65); A = mem_rd(gb, DE);
  B = 0x08;
  alu_cp(gb, 0x3e);
  if (F & FC) CYCT(0x3b65, 0x3b6b);
  else {
    CYC(0x3b65, 0x3b6b);
    B = alu_inc8(gb, B);
    alu_cp(gb, 0x67);
    if (F & FC) CYCT(0x3b6b, 0x3b70);
    else {
      CYC(0x3b6b, 0x3b70);
      B = alu_inc8(gb, B);
      alu_cp(gb, 0x98);
      if (F & FC) CYCT(0x3b70, 0x3b75);
      else {
        CYC(0x3b70, 0x3b75);
        B = alu_inc8(gb, B);
        alu_cp(gb, 0xdc);
        if (F & FC) CYCT(0x3b75, 0x3b7a);
        else { CYC(0x3b75, 0x3b7c); B = 0x10; }
      }
    }
  }
  A = B;
  CYC(0x3b7c, 0x3b7f); H8(hRomBank) = A;
  CYC(0x3b7f, 0x3b82); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x3b82, 0x3b83); A = mem_rd(gb, DE);
  SET_HL(interactionCodeTable);
  CYC(0x3b83, 0x3b87);
  add_double_index_to_hl(gb);
  CYC(0x3b87, 0x3b88); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3b88, 0x3b89); H = mem_rd(gb, HL);
  L = A;
  CYC(0x3b89, 0x3b8b);
  hook_handoff(gb, HL);
}

void load_link_and_companion_animation_frame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x2b25, 0x06);
  CALL_C(0x2b2f, loadLinkAndCompanionAnimationFrame_body_hook, 0x44c9, 0x2b32);
  bank_pop(gb, 0x2b32);
  CYC(0x2b38, 0x2b39);
}

void loadLinkAndCompanionAnimationFrame_hook(GB *gb) {
  load_link_and_companion_animation_frame_hook(gb);
  ret_effect(gb);
}

static void update_all_objects_after_bank6_selected(GB *gb, uint16_t sp0_);

void updateAllObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x345b, 0x345d); A = H8(hRomBank);
  CYC(0x345d, 0x345e); push_effect(gb, AF);
  switch_bank(gb, 0x345e, 0x05);
  CALL_C(0x3465, updateSpecialObjects_hook, 0x4000, 0x3468);
  switch_bank(gb, 0x3468, 0x07);
  CALL_C(0x346f, updateItems_hook, 0x4872, 0x3472);
  CALL_ROM(0x3472, ROM_setEnemyTargetToLinkPosition);
  switch_bank(gb, 0x3475, 0x00);
  CALL_ROM(0x347c, ROM_updateEnemies);
  switch_bank(gb, 0x347f, 0x11);
  CALL_ROM(0x3486, ROM_b11_updateParts);
  switch_bank(gb, 0x3489, 0x00);
  CALL_ROM(0x3490, ROM_updateInteractions);
  switch_bank(gb, 0x3493, 0x01);
  CALL_C(0x349a, func_4000_b01_hook, ROM_b01_func_4000, 0x349d);
  switch_bank(gb, 0x349d, 0x05);
  CYC(0x34a4, 0x34a7); A = W8(wLinkObjectIndex);
  alu_rrca(gb);
  CYC(0x34a7, 0x34a8);
  if (F & FC) CALL_C_CC(0x34a8, func_410d_hook, 0x410d, 0x34ab);
  else CYC(0x34a8, 0x34ab);
  CYC(0x34ab, 0x34ad); A = 0x06;
  update_all_objects_after_bank6_selected(gb, sp0_);
}

static void update_all_objects_after_bank6_selected(GB *gb, uint16_t sp0_) {
  CYC(0x34ad, 0x34af); H8(hRomBank) = A;
  CYC(0x34af, 0x34b2); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x34b2, 0x34b5); A = W8(wLinkGrabState);
  alu_rlca(gb);
  CYC(0x34b5, 0x34b6);
  if (F & FC) CALL_ROM_CC(0x34b6, ROM_b06_updateGrabbedObjectPosition);
  else CYC(0x34b6, 0x34b9);
  CYC(0x34b9, 0x34bc);
  load_link_and_companion_animation_frame_hook(gb);
  switch_bank(gb, 0x34bc, 0x07);
  CALL_C(0x34c3, updateItemsPost_hook, 0x491a, 0x34c6);
  switch_bank(gb, 0x34c6, 0x01);
  CALL_ROM(0x34cd, ROM_b01_checkUpdateFollowingLinkObject);
  switch_bank(gb, 0x34d0, 0x00);
  CALL_ROM(0x34d7, ROM_updateCamera);
  switch_bank(gb, 0x34da, 0x04);
  CALL_C(0x34e1, updateChangedTileQueue_hook, 0x6c32, 0x34e4);
  switch_bank(gb, 0x34e4, 0x04);
  CALL_ROM(0x34eb, ROM_b04_updateAnimations);
  alu_xor(gb, A);
  CYC(0x34ee, 0x34f2); W8(wc4b6) = A;
  CYC(0x34f2, 0x34f3); SET_AF(pop_effect(gb));
  CYC(0x34f3, 0x34f5); H8(hRomBank) = A;
  CYC(0x34f5, 0x34f8); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x34f8, 0x34f9);
  ret_effect(gb);
}

void updateAllObjects__jump34ad_hook(GB *gb) {
  update_all_objects_after_bank6_selected(gb, gb->sp);
}

void updateSpecialObjectsAndInteractions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x34f9, 0x34fb); A = H8(hRomBank);
  CYC(0x34fb, 0x34fc); push_effect(gb, AF);
  switch_bank(gb, 0x34fc, 0x05);
  CALL_C(0x3503, updateSpecialObjects_hook, 0x4000, 0x3506);
  switch_bank(gb, 0x3506, 0x00);
  CALL_ROM(0x350d, ROM_updateInteractions);
  CYC(0x3510, 0x3513);
  load_link_and_companion_animation_frame_hook(gb);
  alu_xor(gb, A);
  CYC(0x3513, 0x3517); W8(wc4b6) = A;
  CYC(0x3517, 0x3518); SET_AF(pop_effect(gb));
  CYC(0x3518, 0x351a); H8(hRomBank) = A;
  CYC(0x351a, 0x351d); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x351d, 0x351e);
  ret_effect(gb);
}

void updateInteractionsAndDrawAllSprites_hook(GB *gb) {
  CYC(0x351e, 0x3520); A = H8(hRomBank);
  CYC(0x3520, 0x3521); push_effect(gb, AF);
  switch_bank(gb, 0x3521, 0x00);
  CALL_ROM(0x3528, ROM_updateInteractions);
  CALL_ROM(0x352b, ROM_drawAllSprites);
  alu_xor(gb, A);
  CYC(0x352e, 0x3532); W8(wc4b6) = A;
  CYC(0x3532, 0x3533); SET_AF(pop_effect(gb));
  CYC(0x3533, 0x3535); H8(hRomBank) = A;
  CYC(0x3535, 0x3538); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x3538, 0x3539);
  ret_effect(gb);
}

void func_3539_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x3539, 0x353b); A = H8(hRomBank);
  CYC(0x353b, 0x353c); push_effect(gb, AF);
  switch_bank(gb, 0x353c, 0x05);
  CALL_C(0x3543, updateSpecialObjects_hook, 0x4000, 0x3546);
  switch_bank(gb, 0x3546, 0x07);
  CALL_C(0x354d, updateItems_hook, 0x4872, 0x3550);
  switch_bank(gb, 0x3550, 0x00);
  CALL_ROM(0x3557, ROM_updateEnemies);
  switch_bank(gb, 0x355a, 0x11);
  CALL_ROM(0x3561, ROM_b11_updateParts);
  switch_bank(gb, 0x3564, 0x00);
  CALL_ROM(0x356b, ROM_updateInteractions);
  switch_bank(gb, 0x356e, 0x07);
  CALL_C(0x3575, updateItemsPost_hook, 0x491a, 0x3578);
  switch_bank(gb, 0x3578, 0x00);
  CYC(0x357f, 0x3582);
  load_link_and_companion_animation_frame_hook(gb);
  switch_bank(gb, 0x3582, 0x04);
  CALL_ROM(0x3589, ROM_b04_updateAnimations);
  alu_xor(gb, A);
  CYC(0x358c, 0x3590); W8(wc4b6) = A;
  CYC(0x3590, 0x3591); SET_AF(pop_effect(gb));
  CYC(0x3591, 0x3593); H8(hRomBank) = A;
  CYC(0x3593, 0x3596); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x3596, 0x3597);
  ret_effect(gb);
}

// tileset and room loading

void loadTilesetLayout_hook(GB *gb) {
  CYC(0x3712, 0x3715); A = W8(wTilesetLayout);
  CALL_ROM(0x3715, ROM_loadTileset);
  switch_bank(gb, 0x3718, 0x18);
  A = 0x03;
  CYC(0x371f, 0x3723); mem_wr(gb, IO_SVBK, A);
  SET_HL(w3TileMappingIndices);
  SET_DE(w3TileMappingData);
  B = 0x00;
  CYC(0x3723, 0x372b);
  for (;;) {
    CYC(0x372b, 0x372c); push_effect(gb, BC);
    CALL_ROM(0x372c, ROM_loadTilesetLayout_helper);
    CYC(0x372f, 0x3730); SET_BC(pop_effect(gb));
    B = alu_dec8(gb, B);
    if (B) { CYCT(0x3730, 0x3733); continue; }
    CYC(0x3730, 0x3733);
    break;
  }
  SET_HL(ROM_b04_setPastCliffPalettesToRed);
  E = 0x04;
  CYC(0x3733, 0x373b);
  interBankCall_hook(gb);
}

static void load_tileset_unique_gfx(GB *gb) {
  switch_bank(gb, 0x3828, 0x04);
  CYC(0x382f, 0x3832); A = W8(wTilesetUniqueGfx);
  alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(0x3832, 0x3835); return; }
  CYC(0x3832, 0x3835);
  SET_HL(uniqueGfxHeaderTable_bank04);
  CYC(0x3835, 0x3839);
  add_double_index_to_hl(gb);
  CYC(0x3839, 0x383a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x383a, 0x383b); H = mem_rd(gb, HL);
  L = A;
  CYC(0x383b, 0x383c);
  for (;;) {
    CYC(0x383c, 0x383f);
    load_unique_gfx_header_entry(gb);
    alu_add(gb, A);
    if (F & FC) { CYCT(0x383f, 0x3842); continue; }
    CYC(0x383f, 0x3843);
    break;
  }
}

void loadTilesetUniqueGfx_hook(GB *gb) {
  load_tileset_unique_gfx(gb);
  ret_effect(gb);
}

void loadTilesetGraphics_hook(GB *gb) {
  CYC(0x3796, 0x3798); A = H8(hRomBank);
  CYC(0x3798, 0x3799); push_effect(gb, AF);
  CYC(0x3799, 0x379c); A = W8(wTilesetGfx);
  CYC(0x379c, 0x379f);
  load_gfx_header(gb, 0x0626, gfxHeaderTable_bank01, true);
  CYC(0x379f, 0x37a2); A = W8(wTilesetPalette);
  CALL_ROM(0x37a2, ROM_loadPaletteHeader);
  CYC(0x37a5, 0x37a8);
  load_tileset_unique_gfx(gb);
  switch_bank(gb, 0x37a8, 0x04);
  CALL_ROM(0x37af, ROM_b04_initializeAnimations);
  SET_HL(ROM_b02_func_02_7a77);
  E = 0x02;
  CYC(0x37b2, 0x37b7);
  CALL_ROM(0x37b7, ROM_interBankCall);
  SET_HL(ROM_b02_checkLoadPastSignAndChestGfx);
  E = 0x02;
  CYC(0x37ba, 0x37bf);
  CALL_ROM(0x37bf, ROM_interBankCall);
  CYC(0x37c2, 0x37c5); A = W8(wTilesetUniqueGfx);
  CYC(0x37c5, 0x37c8); W8(wLoadedTilesetUniqueGfx) = A;
  CYC(0x37c8, 0x37cb); A = W8(wTilesetPalette);
  CYC(0x37cb, 0x37ce); W8(wLoadedTilesetPalette) = A;
  CYC(0x37ce, 0x37d1); A = W8(wTilesetAnimation);
  CYC(0x37d1, 0x37d4); W8(wLoadedTilesetAnimation) = A;
  CYC(0x37d4, 0x37d5); SET_AF(pop_effect(gb));
  CYC(0x37d5, 0x37d7); H8(hRomBank) = A;
  CYC(0x37d7, 0x37da); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x37da, 0x37db);
  ret_effect(gb);
}

void loadTilesetData_hook(GB *gb) {
  CYC(0x3889, 0x388b); A = H8(hRomBank);
  CYC(0x388b, 0x388c); push_effect(gb, AF);
  switch_bank(gb, 0x388c, 0x04);
  CALL_ROM(0x3893, ROM_b04_loadTilesetData_body);
  SET_HL(ROM_b02_updateTilesetFlagsForIndoorRoomInAltWorld);
  E = 0x02;
  CYC(0x3896, 0x389b);
  CALL_ROM(0x389b, ROM_interBankCall);
  CYC(0x389e, 0x389f); SET_AF(pop_effect(gb));
  CYC(0x389f, 0x38a1); H8(hRomBank) = A;
  CYC(0x38a1, 0x38a4); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x38a4, 0x38a5);
  ret_effect(gb);
}

void loadTilesetAndRoomLayout_hook(GB *gb) {
  CYC(0x38a5, 0x38a7); A = H8(hRomBank);
  CYC(0x38a7, 0x38a8); push_effect(gb, AF);
  CYC(0x38a8, 0x38ab); A = W8(wLoadedTilesetLayout);
  B = A;
  CYC(0x38ab, 0x38af); A = W8(wTilesetLayout);
  alu_cp(gb, B);
  CYC(0x38af, 0x38b3); W8(wLoadedTilesetLayout) = A;
  if (!(F & FZ)) CALL_ROM_CC(0x38b3, ROM_loadTilesetLayout);
  else CYC(0x38b3, 0x38b6);
  CALL_ROM(0x38b6, ROM_loadRoomLayout);
  switch_bank(gb, 0x38b9, 0x04);
  CALL_ROM(0x38c0, ROM_b04_applyAllTileSubstitutions);
  A = 0x03;
  CYC(0x38c3, 0x38c7); mem_wr(gb, IO_SVBK, A);
  SET_HL(w3RoomLayoutBuffer);
  SET_DE(wRoomLayout);
  B = 0xc0;
  CYC(0x38c7, 0x38cf);
  CALL_ROM(0x38cf, ROM_copyMemoryReverse);
  alu_xor(gb, A);
  CYC(0x38d2, 0x38d5); mem_wr(gb, IO_SVBK, A);
  CYC(0x38d5, 0x38d6); SET_AF(pop_effect(gb));
  CYC(0x38d6, 0x38d8); H8(hRomBank) = A;
  CYC(0x38d8, 0x38db); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x38db, 0x38dc);
  ret_effect(gb);
}

static void inter_bank_call(GB *gb, uint16_t a, uint8_t bank, uint16_t target) {
  SET_HL(target);
  E = bank;
  CYC(a, a + 5);
  CALL_ROM(a + 5, ROM_interBankCall);
}

void initializeRoom_hook(GB *gb) {
  inter_bank_call(gb, 0x30fe, 0x01, ROM_b01_clearSolidObjectPositions);
  CYC(0x3106, 0x3109); A = W8(wSentBackByStrangeForce);
  A = alu_dec8(gb, A);
  if (F & FZ) {
    CYC(0x3109, 0x310c);
    B = 0x7c;
    CYC(0x310c, 0x3111);
    objectCreateInteractionWithSubid00_hook(gb);
    return;
  }
  CYCT(0x3109, 0x310c);
  inter_bank_call(gb, 0x3111, 0x02, ROM_b02_calculateRoomStateModifier);
  CALL_ROM(0x3119, ROM_refreshObjectGfx);
  inter_bank_call(gb, 0x311c, 0x12, ROM_b12_runRoomSpecificCode);
  inter_bank_call(gb, 0x3124, 0x02, ROM_b02_createSeaEffectsPartIfApplicable);
  inter_bank_call(gb, 0x312c, 0x01, ROM_b01_checkLoadPirateShip);
  bank_push(gb, 0x3134, 0x02);
  CALL_ROM(0x313e, ROM_checkSpawnTimeportalInteraction);
  CYC(0x3141, 0x3144); A = W8(wcc05);
  alu_bit(gb, 2, A);
  CYC(0x3144, 0x3146);
  if (!(F & FZ)) CALL_ROM_CC(0x3146, ROM_b02_loadRememberedCompanion);
  else CYC(0x3146, 0x3149);
  CYC(0x3149, 0x314c); A = W8(wcc05);
  alu_bit(gb, 3, A);
  CYC(0x314c, 0x314e);
  if (!(F & FZ)) CALL_ROM_CC(0x314e, ROM_b02_checkAndSpawnMaple);
  else CYC(0x314e, 0x3151);
  switch_bank(gb, 0x3151, 0x12);
  CYC(0x3158, 0x315b); A = W8(wcc05);
  alu_bit(gb, 0, A);
  CYC(0x315b, 0x315d);
  if (!(F & FZ)) CALL_ROM_CC(0x315d, ROM_b12_parseObjectData);
  else CYC(0x315d, 0x3160);
  switch_bank(gb, 0x3160, 0x16);
  CALL_ROM(0x3167, ROM_b16_parseStaticObjects);
  bank_pop(gb, 0x316a);
  CYC(0x3170, 0x3171);
  ret_effect(gb);
}

void loadStaticObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x3189, 0x16);
  CYC(0x3193, 0x3194); push_effect(gb, DE);
  CALL_C(0x3194, loadStaticObjects_body_hook, 0x5085, 0x3197);
  CYC(0x3197, 0x3198); SET_DE(pop_effect(gb));
  bank_pop(gb, 0x3198);
  CYC(0x319e, 0x319f);
  ret_effect(gb);
}

void objectSaveAsStaticObject_hook(GB *gb) {
  CYC(0x31cf, 0x31d0); mem_wr(gb, HL, A);
  CYC(0x31d0, 0x31d2); A = H8(hActiveObjectType);
  alu_add(gb, 0x16);
  E = A;
  A = L;
  CYC(0x31d2, 0x31d7); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(0x31d7, 0x31da); mem_wr(gb, DE, A);
  CYC(0x31da, 0x31dd); A = W8(wActiveRoom);
  SET_HL(HL + 1);
  CYC(0x31dd, 0x31df); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x31df, 0x31e1); A = H8(hActiveObjectType);
  A = alu_inc8(gb, A);
  E = A;
  CYC(0x31e1, 0x31e4); A = mem_rd(gb, DE);
  CYC(0x31e4, 0x31e5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E);
  CYC(0x31e5, 0x31e7); A = mem_rd(gb, DE);
  CYC(0x31e7, 0x31e8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = E;
  alu_add(gb, 0x09);
  E = A;
  CYC(0x31e8, 0x31ed); A = mem_rd(gb, DE);
  CYC(0x31ed, 0x31ee); mem_wr(gb, HL, A); SET_HL(HL + 1);
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(0x31ee, 0x31f1); A = mem_rd(gb, DE);
  CYC(0x31f1, 0x31f2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x31f2, 0x31f3);
  ret_effect(gb);
}

void objectMarkSolidPosition_hook(GB *gb) {
  CYC(0x24f0, 0x24f3);
  object_get_short_position(gb);
  B = A;
  A = 0x02;
  CYC(0x24f3, 0x24f8); mem_wr(gb, IO_SVBK, A);
  A = B;
  SET_HL(w2SolidObjectPositions);
  CYC(0x24f8, 0x24ff);
  set_flag(gb);
  A = 0x00;
  CYC(0x24ff, 0x2503); mem_wr(gb, IO_SVBK, A);
  CYC(0x2503, 0x2504);
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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x30b0, 0x03);
  CALL_C(0x30ba, disableLcdAndLoadRoom_body_hook, 0x5fe5, 0x30bd);
  bank_pop(gb, 0x30bd);
  CYC(0x30c3, 0x30c4); ret_effect(gb);
}
void playWaveSoundAtRandomIntervals_hook(GB *gb) { banked_body_call(gb, 0x30c4, 0x10, ROM_b10_playWaveSoundAtRandomIntervals_body); }
void func_3ed0_hook(GB *gb) { banked_body_call(gb, 0x3ed0, 0x03, ROM_b03_func_03_7841); }
void func_3ee4_hook(GB *gb) { banked_body_call(gb, 0x3ee4, 0x03, ROM_b03_func_03_7849); }
void endgameCutsceneHandler_hook(GB *gb) { banked_body_call(gb, 0x306c, 0x03, ROM_b03_endgameCutsceneHandler_body); }

void fileSelect_redrawDecorations_hook(GB *gb) {
  bank_push(gb, 0x3099, 0x02);
  CALL_ROM(0x30a3, ROM_b02_fileSelect_redrawDecorationsAndSetWramBank4);
  bank_pop(gb, 0x30a6);
  alu_xor(gb, A);
  CYC(0x30ac, 0x30af); mem_wr(gb, IO_SVBK, A);
  CYC(0x30af, 0x30b0);
  ret_effect(gb);
}

void func_2d48_hook(GB *gb) {
  bank_push(gb, 0x2d48, 0x3f);
  A = B;
  SET_HL(data_5951_bank3f);
  CYC(0x2d52, 0x2d57);
  add_a_to_hl(gb);
  CYC(0x2d57, 0x2d58); B = mem_rd(gb, HL);
  bank_pop(gb, 0x2d58);
  CYC(0x2d5e, 0x2d5f);
  ret_effect(gb);
}

void func_32fc_hook(GB *gb) {
  CYC(0x32fc, 0x32ff);
  set_palette_thread_delay(gb);
  A = 0x0d;
  B = 0xf0;
  CYC(0x32ff, 0x3306); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x3306, 0x330a);
  set_darkening_variables(gb);
  ret_effect(gb);
}

void func_333e_hook(GB *gb) {
  CYC(0x333e, 0x3341);
  set_palette_thread_delay(gb);
  A = 0x0e;
  B = 0x00;
  CYC(0x3341, 0x3348); W8(wPaletteThread_mode) = A;
  A = 0x01;
  CYC(0x3348, 0x334c);
  set_darkening_variables(gb);
  ret_effect(gb);
}

// sound queue entry and music volume

void playSound_b00_hook(GB *gb) {
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x0c98, 0x0c9a); ret_effect(gb); return; }
  CYC(0x0c98, 0x0c9a);
  H = A;
  CYC(0x0c9a, 0x0c9d); A = H8(hFFB7);
  alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(0x0c9d, 0x0ca0); ret_effect(gb); return; }
  CYC(0x0c9d, 0x0ca0);
  CYC(0x0ca0, 0x0ca2); A = H8(hMusicQueueTail);
  L = A;
  A = H;
  H = wMusicQueue >> 8;
  CYC(0x0ca2, 0x0ca7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = L;
  alu_and(gb, 0xaf);
  CYC(0x0ca7, 0x0cac); H8(hMusicQueueTail) = A;
  CYC(0x0cac, 0x0cad);
  ret_effect(gb);
}

void setMusicVolume_hook(GB *gb) {
  alu_or(gb, 0x80);
  CYC(0x0cad, 0x0cb1); H8(hMusicVolume) = A;
  CYC(0x0cb1, 0x0cb2);
  ret_effect(gb);
}

// sequential ROM reads that cross into the next bank, and the graphics decompressor

static void adjust_hl_sequential(GB *gb) {
  A = H;
  alu_cp(gb, 0x80);
  if (!(F & FZ)) CYCT(0x0776, 0x077b);
  else {
    CYC(0x0776, 0x077b);
    H = 0x40;
    CYC(0x077b, 0x077f); A = H8(hRomBank);
    A = alu_inc8(gb, A);
    CYC(0x077f, 0x0782); H8(hRomBank) = A;
    CYC(0x0782, 0x0785); mem_wr(gb, MBC_ROM_BANK, A);
  }
  A = B;
  alu_or(gb, C);
  CYC(0x0785, 0x0788);
}

void _adjustHLSequential_hook(GB *gb) {
  adjust_hl_sequential(gb);
  ret_effect(gb);
}

static void copy_byte_sequential(GB *gb) {
  CYC(0x0772, 0x0773); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0773, 0x0774); mem_wr(gb, DE, A);
  SET_DE(DE + 1);
  SET_BC(BC - 1);
  CYC(0x0774, 0x0776);
  adjust_hl_sequential(gb);
}

void copyByteSequential_hook(GB *gb) {
  copy_byte_sequential(gb);
  ret_effect(gb);
}

static void decompress_block(GB *gb) {
  CYC(0x069c, 0x069f);
  read_byte_sequential(gb);
  C = A;
  CYC(0x069f, 0x06a3);
  read_byte_sequential(gb);
  CYC(0x06a3, 0x06a5); H8(hFF8A) = A;
  alu_or(gb, C);
  if (!(F & FZ)) {
    CYCT(0x06a5, 0x06a8);
    CYC(0x06b3, 0x06b6);
    read_byte_sequential(gb);
    CYC(0x06b6, 0x06b8); H8(hFF8B) = A;
    B = 0x08;
    CYC(0x06b8, 0x06ba);
    for (;;) {
      C = alu_rl(gb, C);
      if (F & FC) { CYCT(0x06ba, 0x06be); CYC(0x06c3, 0x06c5); A = H8(hFF8B); }
      else { CYC(0x06ba, 0x06be); CYC(0x06be, 0x06c1); read_byte_sequential(gb); CYC(0x06c1, 0x06c3); }
      CYC(0x06c5, 0x06c6); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x06c6, 0x06ca); continue; }
      CYC(0x06c6, 0x06ca);
      break;
    }
    CYC(0x06ca, 0x06cc); A = H8(hFF8A);
    C = A;
    B = 0x08;
    CYC(0x06cc, 0x06cf);
    for (;;) {
      C = alu_rl(gb, C);
      if (F & FC) { CYCT(0x06cf, 0x06d3); CYC(0x06d8, 0x06da); A = H8(hFF8B); }
      else { CYC(0x06cf, 0x06d3); CYC(0x06d3, 0x06d6); read_byte_sequential(gb); CYC(0x06d6, 0x06d8); }
      CYC(0x06da, 0x06db); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x06db, 0x06df); continue; }
      CYC(0x06db, 0x06e0);
      return;
    }
  }
  CYC(0x06a5, 0x06a8);
  B = 0x10;
  CYC(0x06a8, 0x06aa);
  for (;;) {
    CYC(0x06aa, 0x06ad);
    read_byte_sequential(gb);
    CYC(0x06ad, 0x06ae); mem_wr(gb, DE, A);
    SET_DE(DE + 1);
    B = alu_dec8(gb, B);
    if (B) { CYCT(0x06ae, 0x06b2); continue; }
    CYC(0x06ae, 0x06b3);
    return;
  }
}

void func_069c_hook(GB *gb) {
  decompress_block(gb);
  ret_effect(gb);
}

static void copy_uncompressed(GB *gb) {
  for (;;) {
    C = 0x10;
    CYC(0x06e0, 0x06e2);
    for (;;) {
      CYC(0x06e2, 0x06e5);
      read_byte_sequential(gb);
      CYC(0x06e5, 0x06e6); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      C = alu_dec8(gb, C);
      if (C) { CYCT(0x06e6, 0x06ea); continue; }
      CYC(0x06e6, 0x06ea);
      break;
    }
    B = alu_dec8(gb, B);
    if (B) { CYCT(0x06ea, 0x06ed); continue; }
    CYC(0x06ea, 0x06ee);
    return;
  }
}

void func_06e0_hook(GB *gb) {
  copy_uncompressed(gb);
  ret_effect(gb);
}

static void decompress_lz(GB *gb) {
  CYC(0x06f5, 0x06f7); H8(hFF8E) = A;
  B = alu_swap(gb, B);
  A = B;
  alu_and(gb, 0xf0);
  C = A;
  alu_xor(gb, B);
  B = A;
  A = 0x01;
  CYC(0x06f7, 0x0703); H8(hFF8B) = A;
  for (;;) {
    CYC(0x0703, 0x0705); A = H8(hFF8B);
    A = alu_dec8(gb, A);
    CYC(0x0705, 0x0708); H8(hFF8B) = A;
    if (!(F & FZ)) CYCT(0x0708, 0x070a);
    else {
      CYC(0x0708, 0x070a);
      A = 0x08;
      CYC(0x070a, 0x070e); H8(hFF8B) = A;
      CYC(0x070e, 0x070f); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x070f, 0x0711); H8(hFF8A) = A;
      CYC(0x0711, 0x0714);
      adjust_hl_sequential(gb);
    }
    CYC(0x0714, 0x0716); A = H8(hFF8A);
    alu_add(gb, A);
    CYC(0x0716, 0x0719); H8(hFF8A) = A;
    if (!(F & FC)) {
      CYC(0x0719, 0x071b);
      CYC(0x071b, 0x071e);
      copy_byte_sequential(gb);
      if (!(F & FZ)) { CYCT(0x071e, 0x0720); continue; }
      CYC(0x071e, 0x0721);
      return;
    }
    CYCT(0x0719, 0x071b);
    CYC(0x0721, 0x0723); A = H8(hFF8E);
    alu_or(gb, A);
    bool no_length = false;
    if (!(F & FZ)) {
      CYCT(0x0723, 0x0726);
      CYC(0x0734, 0x0735); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0735, 0x0737); H8(hFF92) = A;
      CYC(0x0737, 0x073a);
      adjust_hl_sequential(gb);
      CYC(0x073a, 0x073b); A = mem_rd(gb, HL);
      alu_and(gb, 0x07);
      CYC(0x073b, 0x073f); H8(hFF93) = A;
      CYC(0x073f, 0x0740); alu_xor(gb, mem_rd(gb, HL));
      if (F & FZ) { CYCT(0x0740, 0x0742); no_length = true; }
      else {
        CYC(0x0740, 0x0742);
        alu_rrca(gb);
        alu_rrca(gb);
        alu_rrca(gb);
        alu_add(gb, 0x02);
        CYC(0x0742, 0x0749);
      }
    } else {
      CYC(0x0723, 0x0726);
      CYC(0x0726, 0x0727); A = mem_rd(gb, HL);
      alu_and(gb, 0x1f);
      CYC(0x0727, 0x072b); H8(hFF92) = A;
      CYC(0x072b, 0x072c); alu_xor(gb, mem_rd(gb, HL));
      if (F & FZ) { CYCT(0x072c, 0x072e); no_length = true; }
      else {
        CYC(0x072c, 0x072e);
        A = alu_swap(gb, A);
        alu_rrca(gb);
        A = alu_inc8(gb, A);
        CYC(0x072e, 0x0734);
      }
    }
    if (no_length) {
      SET_HL(HL + 1);
      CYC(0x0749, 0x074d);
      adjust_hl_sequential(gb);
      CYC(0x074d, 0x074e); A = mem_rd(gb, HL);
    }
    CYC(0x074e, 0x0750); H8(hFF8F) = A;
    SET_HL(HL + 1);
    CYC(0x0750, 0x0754);
    adjust_hl_sequential(gb);
    CYC(0x0754, 0x0755); push_effect(gb, HL);
    CYC(0x0755, 0x0757); A = H8(hFF92);
    alu_cpl(gb);
    L = A;
    CYC(0x0757, 0x075b); A = H8(hFF93);
    alu_cpl(gb);
    H = A;
    alu_add_hl(gb, DE);
    CYC(0x075b, 0x075e);
    for (;;) {
      CYC(0x075e, 0x075f); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x075f, 0x0760); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      SET_BC(BC - 1);
      A = B;
      alu_or(gb, C);
      if (F & FZ) {
        CYCT(0x0760, 0x0766);
        CYC(0x0770, 0x0771); SET_HL(pop_effect(gb));
        CYC(0x0771, 0x0772);
        return;
      }
      CYC(0x0760, 0x0766);
      CYC(0x0766, 0x0768); A = H8(hFF8F);
      A = alu_dec8(gb, A);
      CYC(0x0768, 0x076b); H8(hFF8F) = A;
      if (!(F & FZ)) { CYCT(0x076b, 0x076d); continue; }
      CYC(0x076b, 0x076d);
      CYC(0x076d, 0x076e); SET_HL(pop_effect(gb));
      CYC(0x076e, 0x0770);
      break;
    }
  }
}

void decompressGraphics_hook(GB *gb) {
  A = E;
  alu_and(gb, 0x0f);
  CYC(0x0672, 0x0677); mem_wr(gb, IO_VBK, A);
  CYC(0x0677, 0x0679); mem_wr(gb, IO_SVBK, A);
  alu_xor(gb, E);
  E = A;
  A = C;
  alu_and(gb, 0x3f);
  CYC(0x0679, 0x0680); H8(hRomBank) = A;
  CYC(0x0680, 0x0683); mem_wr(gb, MBC_ROM_BANK, A);
  B = alu_inc8(gb, B);
  A = C;
  alu_and(gb, 0xc0);
  if (F & FZ) { CYCT(0x0683, 0x068a); copy_uncompressed(gb); ret_effect(gb); return; }
  CYC(0x0683, 0x068a);
  alu_cp(gb, 0xc0);
  if (F & FZ) { CYCT(0x068a, 0x068e); A = 0xff; CYC(0x06ee, 0x06f2); decompress_lz(gb); ret_effect(gb); return; }
  CYC(0x068a, 0x068e);
  alu_cp(gb, 0x40);
  if (F & FZ) {
    CYCT(0x068e, 0x0692);
    alu_xor(gb, A);
    CYC(0x06f2, 0x06f5); H8(hFF93) = A;
    decompress_lz(gb);
    ret_effect(gb);
    return;
  }
  CYC(0x068e, 0x0692);
  A = B;
  CYC(0x0692, 0x0693);
  for (;;) {
    CYC(0x0693, 0x0694); push_effect(gb, AF);
    CYC(0x0694, 0x0697);
    decompress_block(gb);
    CYC(0x0697, 0x0698); SET_AF(pop_effect(gb));
    A = alu_dec8(gb, A);
    if (A) { CYCT(0x0698, 0x069b); continue; }
    CYC(0x0698, 0x069c);
    break;
  }
  ret_effect(gb);
}

static bool decompress_call_hook(GB *gb, void (*fn)(GB *), uint16_t target,
                                 uint16_t ra, uint16_t sp0_) {
  push_effect(gb, (uint16_t)(ra));
  uint16_t sp_ = cpu_sp(gb);
  if (hook_in_verify || !hook_enabled_at(target)) asm_call(gb, target, ra);
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
  for (;;) {
    CALL_C_FRAGMENT(0x06aa, readByteSequential_hook, 0x0788, 0x06ad);
    CYC(0x06ad, 0x06ae); mem_wr(gb, DE, A);
    CYC(0x06ae, 0x06af); SET_DE(DE + 1);
    CYC(0x06af, 0x06b0); B = alu_dec8(gb, B);
    if (B) { CYCT(0x06b0, 0x06b2); continue; }
    CYC(0x06b0, 0x06b2);
    CYC(0x06b2, 0x06b3);
    return false;
  }
}

static bool decompress_label_053(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x06c5, 0x06c6); mem_wr(gb, DE, A);
    CYC(0x06c6, 0x06c7); SET_DE(DE + 1);
    CYC(0x06c7, 0x06c8); B = alu_dec8(gb, B);
    if (B) {
      CYCT(0x06c8, 0x06ca);
      CYC(0x06ba, 0x06bc); C = alu_rl(gb, C);
      if (F & FC) { CYCT(0x06bc, 0x06be); CYC(0x06c3, 0x06c5); A = H8(hFF8B); }
      else { CYC(0x06bc, 0x06be); CALL_C_FRAGMENT(0x06be, readByteSequential_hook, 0x0788, 0x06c1); CYC(0x06c1, 0x06c3); }
      continue;
    }
    CYC(0x06c8, 0x06ca);
    CYC(0x06ca, 0x06cc); A = H8(hFF8A);
    CYC(0x06cc, 0x06cd); C = A;
    CYC(0x06cd, 0x06cf); B = 0x08;
    return decompress_label_054(gb, sp0_);
  }
}

static bool decompress_label_051(GB *gb, uint16_t sp0_) {
  CYC(0x06ba, 0x06bc); C = alu_rl(gb, C);
  if (F & FC) { CYCT(0x06bc, 0x06be); CYC(0x06c3, 0x06c5); A = H8(hFF8B); }
  else { CYC(0x06bc, 0x06be); CALL_C_FRAGMENT(0x06be, readByteSequential_hook, 0x0788, 0x06c1); CYC(0x06c1, 0x06c3); }
  return decompress_label_053(gb, sp0_);
}

static bool decompress_label_050(GB *gb, uint16_t sp0_) {
  CALL_C_FRAGMENT(0x06b3, readByteSequential_hook, 0x0788, 0x06b6);
  CYC(0x06b6, 0x06b8); H8(hFF8B) = A;
  CYC(0x06b8, 0x06ba); B = 0x08;
  return decompress_label_051(gb, sp0_);
}

static bool decompress_label_054(GB *gb, uint16_t sp0_) {
  CYC(0x06cf, 0x06d1); C = alu_rl(gb, C);
  if (F & FC) { CYCT(0x06d1, 0x06d3); CYC(0x06d8, 0x06da); A = H8(hFF8B); }
  else { CYC(0x06d1, 0x06d3); CALL_C_FRAGMENT(0x06d3, readByteSequential_hook, 0x0788, 0x06d6); CYC(0x06d6, 0x06d8); }
  return decompress_label_056(gb, sp0_);
}

static bool decompress_label_056(GB *gb, uint16_t sp0_) {
  CYC(0x06da, 0x06db); mem_wr(gb, DE, A);
  CYC(0x06db, 0x06dc); SET_DE(DE + 1);
  CYC(0x06dc, 0x06dd); B = alu_dec8(gb, B);
  if (B) { CYCT(0x06dd, 0x06df); return decompress_label_054(gb, sp0_); }
  CYC(0x06dd, 0x06df);
  CYC(0x06df, 0x06e0);
  return false;
}

static bool decompress_label_069(GB *gb, uint16_t sp0_) {
  (void)sp0_;
  CYC(0x0770, 0x0771); SET_HL(pop_effect(gb));
  CYC(0x0771, 0x0772);
  return false;
}

static bool decompress_label_068(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x075e, 0x075f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x075f, 0x0760); mem_wr(gb, DE, A);
    CYC(0x0760, 0x0761); SET_DE(DE + 1);
    CYC(0x0761, 0x0762); SET_BC(BC - 1);
    CYC(0x0762, 0x0763); A = B;
    CYC(0x0763, 0x0764); alu_or(gb, C);
    if (F & FZ) {
      CYCT(0x0764, 0x0766);
      return decompress_label_069(gb, sp0_);
    }
    CYC(0x0764, 0x0766);
    CYC(0x0766, 0x0768); A = H8(hFF8F);
    CYC(0x0768, 0x0769); A = alu_dec8(gb, A);
    CYC(0x0769, 0x076b); H8(hFF8F) = A;
    if (!(F & FZ)) { CYCT(0x076b, 0x076d); continue; }
    CYC(0x076b, 0x076d);
    CYC(0x076d, 0x076e); SET_HL(pop_effect(gb));
    CYC(0x076e, 0x0770);
    return decompress_label_062(gb, sp0_);
  }
}

static bool decompress_label_067(GB *gb, uint16_t sp0_) {
  CYC(0x074e, 0x0750); H8(hFF8F) = A;
  CYC(0x0750, 0x0751); SET_HL(HL + 1);
  CALL_C_FRAGMENT(0x0751, _adjustHLSequential_hook, 0x0776, 0x0754);
  CYC(0x0754, 0x0755); push_effect(gb, HL);
  CYC(0x0755, 0x0757); A = H8(hFF92);
  CYC(0x0757, 0x0758); alu_cpl(gb);
  CYC(0x0758, 0x0759); L = A;
  CYC(0x0759, 0x075b); A = H8(hFF93);
  CYC(0x075b, 0x075c); alu_cpl(gb);
  CYC(0x075c, 0x075d); H = A;
  CYC(0x075d, 0x075e); alu_add_hl(gb, DE);
  return decompress_label_068(gb, sp0_);
}

static bool decompress_label_066(GB *gb, uint16_t sp0_) {
  CYC(0x0749, 0x074a); SET_HL(HL + 1);
  CALL_C_FRAGMENT(0x074a, _adjustHLSequential_hook, 0x0776, 0x074d);
  CYC(0x074d, 0x074e); A = mem_rd(gb, HL);
  return decompress_label_067(gb, sp0_);
}

static bool decompress_label_065(GB *gb, uint16_t sp0_) {
  CYC(0x0734, 0x0735); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0735, 0x0737); H8(hFF92) = A;
  CALL_C_FRAGMENT(0x0737, _adjustHLSequential_hook, 0x0776, 0x073a);
  CYC(0x073a, 0x073b); A = mem_rd(gb, HL);
  CYC(0x073b, 0x073d); alu_and(gb, 0x07);
  CYC(0x073d, 0x073f); H8(hFF93) = A;
  CYC(0x073f, 0x0740); alu_xor(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x0740, 0x0742); return decompress_label_066(gb, sp0_); }
  CYC(0x0740, 0x0742);
  CYC(0x0742, 0x0743); alu_rrca(gb);
  CYC(0x0743, 0x0744); alu_rrca(gb);
  CYC(0x0744, 0x0745); alu_rrca(gb);
  CYC(0x0745, 0x0747); alu_add(gb, 0x02);
  CYC(0x0747, 0x0749);
  return decompress_label_067(gb, sp0_);
}

static bool decompress_label_064(GB *gb, uint16_t sp0_) {
  CYC(0x0721, 0x0723); A = H8(hFF8E);
  CYC(0x0723, 0x0724); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x0724, 0x0726); return decompress_label_065(gb, sp0_); }
  CYC(0x0724, 0x0726);
  CYC(0x0726, 0x0727); A = mem_rd(gb, HL);
  CYC(0x0727, 0x0729); alu_and(gb, 0x1f);
  CYC(0x0729, 0x072b); H8(hFF92) = A;
  CYC(0x072b, 0x072c); alu_xor(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x072c, 0x072e); return decompress_label_066(gb, sp0_); }
  CYC(0x072c, 0x072e);
  CYC(0x072e, 0x0730); A = alu_swap(gb, A);
  CYC(0x0730, 0x0731); alu_rrca(gb);
  CYC(0x0731, 0x0732); A = alu_inc8(gb, A);
  CYC(0x0732, 0x0734);
  return decompress_label_067(gb, sp0_);
}

static bool decompress_label_063(GB *gb, uint16_t sp0_) {
  CYC(0x0714, 0x0716); A = H8(hFF8A);
  CYC(0x0716, 0x0717); alu_add(gb, A);
  CYC(0x0717, 0x0719); H8(hFF8A) = A;
  if (!(F & FC)) {
    CYC(0x0719, 0x071b);
    CALL_C_FRAGMENT(0x071b, copyByteSequential_hook, 0x0772, 0x071e);
    if (!(F & FZ)) { CYCT(0x071e, 0x0720); return decompress_label_062(gb, sp0_); }
    CYC(0x071e, 0x0720);
    CYC(0x0720, 0x0721);
    return false;
  }
  CYCT(0x0719, 0x071b);
  return decompress_label_064(gb, sp0_);
}

static bool decompress_label_062(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x0703, 0x0705); A = H8(hFF8B);
    CYC(0x0705, 0x0706); A = alu_dec8(gb, A);
    CYC(0x0706, 0x0708); H8(hFF8B) = A;
    if (!(F & FZ)) { CYCT(0x0708, 0x070a); return decompress_label_063(gb, sp0_); }
    CYC(0x0708, 0x070a);
    CYC(0x070a, 0x070c); A = 0x08;
    CYC(0x070c, 0x070e); H8(hFF8B) = A;
    CYC(0x070e, 0x070f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x070f, 0x0711); H8(hFF8A) = A;
    CALL_C_FRAGMENT(0x0711, _adjustHLSequential_hook, 0x0776, 0x0714);
    return decompress_label_063(gb, sp0_);
  }
}

static bool decompress_label_061(GB *gb, uint16_t sp0_) {
  CYC(0x06f5, 0x06f7); H8(hFF8E) = A;
  CYC(0x06f7, 0x06f9); B = alu_swap(gb, B);
  CYC(0x06f9, 0x06fa); A = B;
  CYC(0x06fa, 0x06fc); alu_and(gb, 0xf0);
  CYC(0x06fc, 0x06fd); C = A;
  CYC(0x06fd, 0x06fe); alu_xor(gb, B);
  CYC(0x06fe, 0x06ff); B = A;
  CYC(0x06ff, 0x0701); A = 0x01;
  CYC(0x0701, 0x0703); H8(hFF8B) = A;
  return decompress_label_062(gb, sp0_);
}

#define DECOMPRESS_HOOK(fn) do { \
  if (fn(gb, sp0_)) return; \
  ret_effect(gb); \
} while (0)

void _label_00_049_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_049); }
void _label_00_050_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_050); }
void _label_00_051_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_051); }
void _label_00_052_hook(GB *gb) { uint16_t sp0_ = gb->sp; CYC(0x06c3, 0x06c5); A = H8(hFF8B); DECOMPRESS_HOOK(decompress_label_053); }
void _label_00_053_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_053); }
void _label_00_054_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_054); }
void _label_00_055_hook(GB *gb) { uint16_t sp0_ = gb->sp; CYC(0x06d8, 0x06da); A = H8(hFF8B); DECOMPRESS_HOOK(decompress_label_056); }
void _label_00_056_hook(GB *gb) { uint16_t sp0_ = gb->sp; DECOMPRESS_HOOK(decompress_label_056); }
void _label_00_059_hook(GB *gb) { uint16_t sp0_ = gb->sp; CYC(0x06ee, 0x06f0); A = 0xff; CYC(0x06f0, 0x06f2); DECOMPRESS_HOOK(decompress_label_061); }
void _label_00_060_hook(GB *gb) { uint16_t sp0_ = gb->sp; CYC(0x06f2, 0x06f3); alu_xor(gb, A); CYC(0x06f3, 0x06f5); H8(hFF93) = A; DECOMPRESS_HOOK(decompress_label_061); }
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
  CYC(0x050b, 0x050c); push_effect(gb, DE);
  L = A;
  CYC(0x050c, 0x050f); A = mem_rd(gb, IO_SVBK);
  C = A;
  CYC(0x050f, 0x0512); A = H8(hRomBank);
  B = A;
  CYC(0x0512, 0x0514); push_effect(gb, BC);
  A = 0x02;
  CYC(0x0514, 0x0518); mem_wr(gb, IO_SVBK, A);
  A = 0x01;
  CYC(0x0518, 0x051c); H8(hRomBank) = A;
  CYC(0x051c, 0x051f); mem_wr(gb, MBC_ROM_BANK, A);
  A = L;
  SET_HL(paletteHeaderTable_bank01);
  CYC(0x051f, 0x0524);
  add_double_index_to_hl(gb);
  CYC(0x0524, 0x0525); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0525, 0x0526); H = mem_rd(gb, HL);
  L = A;
  CYC(0x0526, 0x0527);
  for (;;) {
    A = 0x01;
    CYC(0x0527, 0x052b); H8(hRomBank) = A;
    CYC(0x052b, 0x052e); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(0x052e, 0x052f); A = mem_rd(gb, HL);
    alu_and(gb, 0x07);
    A = alu_inc8(gb, A);
    B = A;
    CYC(0x052f, 0x0534); A = mem_rd(gb, HL);
    alu_rlca(gb);
    A = alu_swap(gb, A);
    alu_and(gb, 0x07);
    SET_DE(bitTable);
    alu_add(gb, E);
    E = A;
    CYC(0x0534, 0x053f); A = mem_rd(gb, DE);
    C = A;
    alu_xor(gb, A);
    CYC(0x053f, 0x0541);
    for (;;) {
      alu_or(gb, C);
      B = alu_dec8(gb, B);
      if (F & FZ) { CYCT(0x0541, 0x0545); break; }
      CYC(0x0541, 0x0545);
      alu_rlca(gb);
      CYC(0x0545, 0x0548);
    }
    B = A;
    C = 0xa6;
    CYC(0x0548, 0x054d); alu_bit(gb, 6, mem_rd(gb, HL));
    if (F & FZ) CYCT(0x054d, 0x054f);
    else { CYC(0x054d, 0x0551); C = 0xa7; }
    CYC(0x0551, 0x0552); A = mem_rd(gb, 0xff00 | C);
    alu_or(gb, B);
    CYC(0x0552, 0x0554); mem_wr(gb, 0xff00 | C, A);
    CYC(0x0554, 0x0555); A = mem_rd(gb, HL);
    alu_and(gb, 0x78);
    alu_add(gb, 0x80);
    E = A;
    D = 0xde;
    CYC(0x0555, 0x055d); A = mem_rd(gb, HL);
    alu_and(gb, 0x07);
    A = alu_inc8(gb, A);
    B = A;
    CYC(0x055d, 0x0562); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_rlca(gb);
    CYC(0x0562, 0x0564); A = mem_rd(gb, HL); SET_HL(HL + 1);
    C = A;
    CYC(0x0564, 0x0566); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0566, 0x0567); push_effect(gb, HL);
    L = C;
    H = A;
    A = 0x17;
    CYC(0x0567, 0x056d); H8(hRomBank) = A;
    CYC(0x056d, 0x0570); mem_wr(gb, MBC_ROM_BANK, A);
    for (;;) {
      C = 0x08;
      CYC(0x0570, 0x0572);
      for (;;) {
        CYC(0x0572, 0x0573); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x0573, 0x0574); mem_wr(gb, DE, A);
        E = alu_inc8(gb, E);
        C = alu_dec8(gb, C);
        if (C) { CYCT(0x0574, 0x0578); continue; }
        CYC(0x0574, 0x0578);
        break;
      }
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x0578, 0x057b); continue; }
      CYC(0x0578, 0x057b);
      break;
    }
    CYC(0x057b, 0x057c); SET_HL(pop_effect(gb));
    if (F & FC) { CYCT(0x057c, 0x057e); continue; }
    CYC(0x057c, 0x057e);
    break;
  }
  CYC(0x057e, 0x057f); SET_BC(pop_effect(gb));
  A = B;
  CYC(0x057f, 0x0582); H8(hRomBank) = A;
  CYC(0x0582, 0x0585); mem_wr(gb, MBC_ROM_BANK, A);
  A = C;
  CYC(0x0585, 0x0588); mem_wr(gb, IO_SVBK, A);
  CYC(0x0588, 0x0589); SET_DE(pop_effect(gb));
  CYC(0x0589, 0x058a);
  ret_effect(gb);
}

// tileset graphics helper: bit-packed literal/back-reference stream into VRAM

void loadTilesetHlpr_hook(GB *gb) {
  A = E;
  alu_and(gb, 0x0f);
  CYC(0x07fe, 0x0803); mem_wr(gb, IO_VBK, A);
  CYC(0x0803, 0x0805); mem_wr(gb, IO_SVBK, A);
  alu_xor(gb, E);
  E = A;
  CYC(0x0805, 0x0807);
  for (;;) {
    CYC(0x0807, 0x0809); A = H8(hFF8E);
    CYC(0x0809, 0x080b); H8(hRomBank) = A;
    CYC(0x080b, 0x080e); mem_wr(gb, MBC_ROM_BANK, A);
    CYC(0x080e, 0x080f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x080f, 0x0811); H8(hFF8B) = A;
    B = 0x08;
    CYC(0x0811, 0x0813);
    for (;;) {
      CYC(0x0813, 0x0815); A = H8(hFF8E);
      CYC(0x0815, 0x0817); H8(hRomBank) = A;
      CYC(0x0817, 0x081a); mem_wr(gb, MBC_ROM_BANK, A);
      CYC(0x081a, 0x081c); A = H8(hFF8B);
      alu_rrca(gb);
      CYC(0x081c, 0x081f); H8(hFF8B) = A;
      if (F & FC) {
        CYCT(0x081f, 0x0821);
        CYC(0x082d, 0x082e); push_effect(gb, BC);
        CYC(0x082e, 0x0830); A = H8(hFF8F);
        alu_bit(gb, 7, A);
        if (!(F & FZ)) {
          CYCT(0x0830, 0x0834);
          CYC(0x0848, 0x0849); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(0x0849, 0x084b); H8(hFF8A) = A;
          CYC(0x084b, 0x084c); A = mem_rd(gb, HL); SET_HL(HL + 1);
          C = A;
          CYC(0x084c, 0x084e); A = mem_rd(gb, HL); SET_HL(HL + 1);
          B = A;
          CYC(0x084e, 0x084f);
        } else {
          CYC(0x0830, 0x0834);
          CYC(0x0834, 0x0835); A = mem_rd(gb, HL); SET_HL(HL + 1);
          C = A;
          CYC(0x0835, 0x0837); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(0x0837, 0x0839); H8(hFF8A) = A;
          alu_and(gb, 0x0f);
          B = A;
          CYC(0x0839, 0x083e); A = H8(hFF8A);
          A = alu_swap(gb, A);
          alu_and(gb, 0x0f);
          alu_add(gb, 0x03);
          CYC(0x083e, 0x0846); H8(hFF8A) = A;
          CYC(0x0846, 0x0848);
        }
        CYC(0x084f, 0x0850); push_effect(gb, HL);
        SET_HL(hFF90);
        CYC(0x0850, 0x0854); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x0854, 0x0855); H = mem_rd(gb, HL);
        L = A;
        alu_add_hl(gb, BC);
        CYC(0x0855, 0x0859); A = H8(hFF8A);
        B = A;
        CYC(0x0859, 0x085c); A = H8(hFF8F);
        alu_and(gb, 0x3f);
        CYC(0x085c, 0x0860); H8(hRomBank) = A;
        CYC(0x0860, 0x0863); mem_wr(gb, MBC_ROM_BANK, A);
        for (;;) {
          CYC(0x0863, 0x0864); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(0x0864, 0x0865); mem_wr(gb, DE, A);
          SET_DE(DE + 1);
          CYC(0x0865, 0x0869);
          dec16_ff8c(gb);
          if (F & FZ) {
            CYCT(0x0869, 0x086b);
            CYC(0x0875, 0x0876); SET_HL(pop_effect(gb));
            CYC(0x0876, 0x0877); SET_BC(pop_effect(gb));
            CYC(0x0877, 0x0878);
            ret_effect(gb);
            return;
          }
          CYC(0x0869, 0x086b);
          B = alu_dec8(gb, B);
          if (B) { CYCT(0x086b, 0x086e); continue; }
          CYC(0x086b, 0x086e);
          break;
        }
        CYC(0x086e, 0x086f); SET_HL(pop_effect(gb));
        CYC(0x086f, 0x0870); SET_BC(pop_effect(gb));
        B = alu_dec8(gb, B);
        if (B) { CYCT(0x0870, 0x0873); continue; }
        CYC(0x0870, 0x0873);
        CYC(0x0873, 0x0875);
        break;
      }
      CYC(0x081f, 0x0821);
      CYC(0x0821, 0x0822); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0822, 0x0823); mem_wr(gb, DE, A);
      SET_DE(DE + 1);
      CYC(0x0823, 0x0827);
      dec16_ff8c(gb);
      if (F & FZ) { CYCT(0x0827, 0x0828); ret_effect(gb); return; }
      CYC(0x0827, 0x0828);
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x0828, 0x082b); continue; }
      CYC(0x0828, 0x082b);
      CYC(0x082b, 0x082d);
      break;
    }
  }
}

// sprite drawing: objects queued by priority, terrain effects, then wOam is padded out

static void draw_sprite_list(GB *gb) {
  switch_bank(gb, 0x0eda, 0x14);
  CYC(0x0ee1, 0x0ee3); A = H8(hOamTail);
  E = A;
  CYC(0x0ee3, 0x0ee5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  alu_add(gb, A);
  alu_add(gb, A);
  alu_add(gb, E);
  alu_cp(gb, 0xa1);
  if (!(F & FC)) { CYCT(0x0ee5, 0x0eed); CYC(0x0f07, 0x0f08); return; }
  CYC(0x0ee5, 0x0eed);
  D = wOam >> 8;
  CYC(0x0eed, 0x0eef);
  for (;;) {
    CYC(0x0eef, 0x0ef1); A = H8(hFF8C);
    CYC(0x0ef1, 0x0ef2); alu_add(gb, mem_rd(gb, HL));
    CYC(0x0ef2, 0x0ef3); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(0x0ef3, 0x0ef7); A = H8(hFF8D);
    CYC(0x0ef7, 0x0ef8); alu_add(gb, mem_rd(gb, HL));
    CYC(0x0ef8, 0x0ef9); mem_wr(gb, DE, A);
    SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(0x0ef9, 0x0efc); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0efc, 0x0efd); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(0x0efd, 0x0eff); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0eff, 0x0f00); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    C = alu_dec8(gb, C);
    if (C) { CYCT(0x0f00, 0x0f04); continue; }
    CYC(0x0f00, 0x0f04);
    break;
  }
  A = E;
  CYC(0x0f04, 0x0f07); H8(hOamTail) = A;
  CYC(0x0f07, 0x0f08);
}

void func_0eda_hook(GB *gb) {
  draw_sprite_list(gb);
  ret_effect(gb);
}

static void draw_object_terrain_effects(GB *gb) {
  CYC(0x0f08, 0x0f0b); A = W8(wTilesetFlags);
  alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x0f0b, 0x0f0e); return; }
  CYC(0x0f0b, 0x0f0e);
  A = B;
  alu_cp(gb, 0x97);
  if (!(F & FC)) { CYCT(0x0f0e, 0x0f12); return; }
  CYC(0x0f0e, 0x0f12);
  alu_bit(gb, 7, E);
  if (F & FZ) {
    CYCT(0x0f12, 0x0f16);
    CYC(0x0f37, 0x0f3a); A = W8(wScrollMode);
    alu_cp(gb, 0x08);
    if (F & FZ) { CYCT(0x0f3a, 0x0f3d); return; }
    CYC(0x0f3a, 0x0f3d);
    CYC(0x0f3d, 0x0f3e); push_effect(gb, HL);
    A = L;
    alu_and(gb, 0xc0);
    alu_add(gb, OBJ_YH);
    L = A;
    CYC(0x0f3e, 0x0f45); A = mem_rd(gb, HL); SET_HL(HL + 1);
    B = A;
    alu_add(gb, 0x05);
    alu_and(gb, 0xf0);
    C = A;
    L = alu_inc8(gb, L);
    CYC(0x0f45, 0x0f4d); L = mem_rd(gb, HL);
    A = L;
    alu_xor(gb, B);
    H = A;
    A = L;
    alu_and(gb, 0xf0);
    A = alu_swap(gb, A);
    alu_or(gb, C);
    C = A;
    B = wRoomLayout >> 8;
    CYC(0x0f4d, 0x0f5a); A = mem_rd(gb, BC);
    alu_cp(gb, 0xf8);
    if (F & FZ) {
      CYCT(0x0f5a, 0x0f5e);
      alu_bit(gb, 2, H);
      CYC(0x0f6b, 0x0f70); A = W8(wGrassAnimationModifier);
      if (F & FZ) CYCT(0x0f70, 0x0f72);
      else { CYC(0x0f70, 0x0f74); alu_add(gb, 0x24); }
      C = A;
      B = 0x00;
      SET_HL(greenGrassAnimationFrame0_bank14);
      alu_add_hl(gb, BC);
      CYC(0x0f74, 0x0f7b);
    } else {
      CYC(0x0f5a, 0x0f5e);
      alu_cp(gb, 0xf9);
      if (!(F & FZ)) {
        CYCT(0x0f5e, 0x0f62);
        CYC(0x0f80, 0x0f81); SET_HL(pop_effect(gb));
        CYC(0x0f81, 0x0f82);
        return;
      }
      CYC(0x0f5e, 0x0f62);
      E = alu_inc8(gb, E);
      SET_HL(wPuddleAnimationPointer);
      CYC(0x0f62, 0x0f67); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0f67, 0x0f68); H = mem_rd(gb, HL);
      L = A;
      CYC(0x0f68, 0x0f6b);
    }
    CYC(0x0f7b, 0x0f7c); push_effect(gb, DE);
    CYC(0x0f7c, 0x0f7f);
    draw_sprite_list(gb);
    CYC(0x0f7f, 0x0f80); SET_DE(pop_effect(gb));
    CYC(0x0f80, 0x0f81); SET_HL(pop_effect(gb));
    CYC(0x0f81, 0x0f82);
    return;
  }
  CYC(0x0f12, 0x0f16);
  CYC(0x0f16, 0x0f19); A = W8(wFrameCounter);
  alu_xor(gb, H);
  alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x0f19, 0x0f1c); return; }
  CYC(0x0f19, 0x0f1c);
  CYC(0x0f1c, 0x0f1d); push_effect(gb, HL);
  CYC(0x0f1d, 0x0f1f); A = H8(hTerrainEffectsBufferUsedSize);
  alu_add(gb, 0xc0);
  L = A;
  H = wTerrainEffectsBuffer >> 8;
  CYC(0x0f1f, 0x0f26); A = H8(hFF8C);
  CYC(0x0f26, 0x0f27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x0f27, 0x0f29); A = H8(hFF8D);
  CYC(0x0f29, 0x0f2a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x00;
  CYC(0x0f2a, 0x0f2d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x40;
  CYC(0x0f2d, 0x0f30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = L;
  alu_sub(gb, 0xc0);
  CYC(0x0f30, 0x0f35); H8(hTerrainEffectsBufferUsedSize) = A;
  CYC(0x0f35, 0x0f36); SET_HL(pop_effect(gb));
  CYC(0x0f36, 0x0f37);
}

void _drawObjectTerrainEffects_hook(GB *gb) {
  draw_object_terrain_effects(gb);
  ret_effect(gb);
}

static void object_position_on_screen_tail(GB *gb) {
  CYC(0x0f92, 0x0f94); H8(hFF8D) = A;
  L = alu_inc8(gb, L);
  CYC(0x0f94, 0x0f96); E = mem_rd(gb, HL);
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, OBJ_VISIBLE);
  L = A;
  CYC(0x0f96, 0x0f9d); A = mem_rd(gb, HL);
  alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x0f9d, 0x0f9f); return; }
  CYC(0x0f9d, 0x0f9f);
  alu_rlca(gb);
  CYC(0x0f9f, 0x0fa0);
  if (F & FC) { CYCT(0x0fa0, 0x0fa3); draw_object_terrain_effects(gb); }
  else CYC(0x0fa0, 0x0fa3);
  A = D;
  alu_add(gb, E);
  CYC(0x0fa3, 0x0fa7); H8(hFF8C) = A;
  A = L;
  alu_and(gb, 0xc0);
  alu_add(gb, 0x1c);
  L = A;
  alu_scf(gb);
  CYC(0x0fa7, 0x0faf);
}

void _getObjectPositionOnScreen_hook(GB *gb) {
  CYC(0x0f82, 0x0f84); A = H8(hCameraX);
  C = A;
  CYC(0x0f84, 0x0f87); A = H8(hCameraY);
  B = A;
  CYC(0x0f87, 0x0f89); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_sub(gb, B);
  alu_add(gb, 0x10);
  CYC(0x0f89, 0x0f8e); H8(hFF8C) = A;
  D = A;
  L = alu_inc8(gb, L);
  CYC(0x0f8e, 0x0f91); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_sub(gb, C);
  CYC(0x0f91, 0x0f92);
  object_position_on_screen_tail(gb);
  ret_effect(gb);
}

static void draw_all_sprites_unconditionally(GB *gb) {
  CYC(0x0da2, 0x0da4); A = H8(hRomBank);
  CYC(0x0da4, 0x0da5); push_effect(gb, AF);
  CYC(0x0da5, 0x0da8);
  queue_draw_everything(gb);
  CYC(0x0da8, 0x0dab); A = W8(wLinkRaisedFloorOffset);
  SET_HL(w1Link_yh);
  CYC(0x0dab, 0x0daf); alu_add(gb, mem_rd(gb, HL));
  CYC(0x0daf, 0x0db0); mem_wr(gb, HL, A);
  SET_DE(w1Link);
  B = 0x0b;
  CYC(0x0db0, 0x0db8); A = W8(wTextboxFlags);
  alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYC(0x0db8, 0x0dbc);
    CYC(0x0dbc, 0x0dbf);
    object_queue_draw(gb);
    CYC(0x0dbf, 0x0dc1);
  } else {
    CYCT(0x0db8, 0x0dbc);
    for (;;) {
      CYC(0x0dc1, 0x0dc4);
      object_queue_draw(gb);
      D = alu_inc8(gb, D);
      A = D;
      alu_cp(gb, 0xd6);
      if (F & FC) { CYCT(0x0dc4, 0x0dca); continue; }
      CYC(0x0dc4, 0x0dca);
      break;
    }
  }
  switch_bank(gb, 0x0dca, 0x14);
  CYC(0x0dd1, 0x0dd4); A = W8(wFrameCounter);
  alu_add(gb, A);
  A = alu_swap(gb, A);
  alu_and(gb, 0x03);
  SET_HL(puddleAnimationFrames_bank14);
  CYC(0x0dd4, 0x0ddd);
  add_double_index_to_hl(gb);
  CYC(0x0ddd, 0x0dde); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0dde, 0x0de1); W8(wPuddleAnimationPointer) = A;
  CYC(0x0de1, 0x0de2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0de2, 0x0de5); mem_wr(gb, wPuddleAnimationPointer + 1, A);
  SET_HL(wRamFunctionAddr);
  A = 0xc3;
  CYC(0x0de5, 0x0deb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x82;
  CYC(0x0deb, 0x0dee); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x0dee, 0x0df0); mem_wr(gb, HL, 0x0f);
  CYC(0x0df0, 0x0df3); A = W8(wScrollMode);
  alu_cp(gb, 0x08);
  if (!(F & FZ)) CYCT(0x0df3, 0x0df7);
  else {
    CYC(0x0df3, 0x0df7);
    A = 0x0f;
    CYC(0x0df7, 0x0dfa); mem_wr(gb, HL, A); SET_HL(HL - 1);
    CYC(0x0dfa, 0x0dfc); mem_wr(gb, HL, 0xc1);
    alu_xor(gb, A);
    B = A;
    A = alu_inc8(gb, A);
    CYC(0x0dfc, 0x0e01); H8(hFF8A) = A;
    CYC(0x0e01, 0x0e04); A = W8(wRoomIsLarge);
    alu_or(gb, A);
    if (F & FZ) CYCT(0x0e04, 0x0e07);
    else { CYC(0x0e04, 0x0e09); A = 0x04; }
    C = A;
    CYC(0x0e09, 0x0e0d); A = W8(wScreenTransitionDirection);
    alu_add(gb, C);
    alu_add(gb, A);
    alu_add(gb, A);
    C = A;
    SET_HL(data_1058);
    alu_add_hl(gb, BC);
    CYC(0x0e0d, 0x0e16); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0e16, 0x0e18); H8(hFF90) = A;
    CYC(0x0e18, 0x0e19); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0e19, 0x0e1b); H8(hFF91) = A;
    CYC(0x0e1b, 0x0e1c); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0e1c, 0x0e1e); H8(hFF92) = A;
    CYC(0x0e1e, 0x0e1f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0e1f, 0x0e21); H8(hFF93) = A;
  }
  SET_HL(wObjectsToDraw);
  CYC(0x0e21, 0x0e24);
  for (;;) {
    CYC(0x0e24, 0x0e25); A = mem_rd(gb, HL);
    alu_or(gb, A);
    CYC(0x0e25, 0x0e26);
    if (!(F & FZ)) CALL_ROM_CC(0x0e26, ROM_drawAllSprites_drawObject);
    else CYC(0x0e26, 0x0e29);
    L = alu_inc8(gb, L);
    L = alu_inc8(gb, L);
    alu_bit(gb, 7, L);
    if (F & FZ) { CYCT(0x0e29, 0x0e2f); continue; }
    CYC(0x0e29, 0x0e2f);
    break;
  }
  SET_HL(wTerrainEffectsBuffer);
  CYC(0x0e2f, 0x0e34); A = H8(hTerrainEffectsBufferUsedSize);
  alu_rrca(gb);
  A = alu_srl(gb, A);
  B = A;
  if (F & FZ) CYCT(0x0e34, 0x0e3a);
  else {
    CYC(0x0e34, 0x0e3a);
    for (;;) {
      CYC(0x0e3a, 0x0e3b); push_effect(gb, BC);
      CYC(0x0e3b, 0x0e3c); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0e3c, 0x0e3e); H8(hFF8C) = A;
      CYC(0x0e3e, 0x0e3f); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0e3f, 0x0e41); H8(hFF8D) = A;
      CYC(0x0e41, 0x0e42); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0e42, 0x0e43); push_effect(gb, HL);
      CYC(0x0e43, 0x0e44); H = mem_rd(gb, HL);
      L = A;
      CYC(0x0e44, 0x0e48);
      draw_sprite_list(gb);
      CYC(0x0e48, 0x0e49); SET_HL(pop_effect(gb));
      L = alu_inc8(gb, L);
      CYC(0x0e49, 0x0e4b); SET_BC(pop_effect(gb));
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x0e4b, 0x0e4e); continue; }
      CYC(0x0e4b, 0x0e4e);
      break;
    }
  }
  CYC(0x0e4e, 0x0e50); A = H8(hOamTail);
  alu_cp(gb, 0xa0);
  if (!(F & FC)) CYCT(0x0e50, 0x0e54);
  else {
    CYC(0x0e50, 0x0e54);
    H = wOam >> 8;
    B = 0xe0;
    CYC(0x0e54, 0x0e58);
    for (;;) {
      L = A;
      CYC(0x0e58, 0x0e5a); mem_wr(gb, HL, B);
      alu_add(gb, 0x04);
      alu_cp(gb, 0xa0);
      if (F & FC) { CYCT(0x0e5a, 0x0e60); continue; }
      CYC(0x0e5a, 0x0e60);
      break;
    }
  }
  CYC(0x0e60, 0x0e63); A = W8(wLinkRaisedFloorOffset);
  alu_cpl(gb);
  A = alu_inc8(gb, A);
  SET_HL(w1Link_yh);
  CYC(0x0e63, 0x0e69); alu_add(gb, mem_rd(gb, HL));
  CYC(0x0e69, 0x0e6a); mem_wr(gb, HL, A);
  CYC(0x0e6a, 0x0e6b); SET_AF(pop_effect(gb));
  CYC(0x0e6b, 0x0e6d); H8(hRomBank) = A;
  CYC(0x0e6d, 0x0e70); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x0e70, 0x0e71);
}

void drawAllSpritesUnconditionally_hook(GB *gb) {
  draw_all_sprites_unconditionally(gb);
  ret_effect(gb);
}

void drawAllSprites_hook(GB *gb) {
  SET_HL(wc4b6);
  CYC(0x0d9a, 0x0d9f); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x0d9f, 0x0da0); ret_effect(gb); return; }
  CYC(0x0d9f, 0x0da0);
  CYC(0x0da0, 0x0da2); mem_wr(gb, HL, 0xff);
  draw_all_sprites_unconditionally(gb);
  ret_effect(gb);
}

// dirty palette upload from the VBlank handler: bit n of the dirty mask selects palette n

static void write_palette_regs(GB *gb) {
  for (;;) {
    D = alu_srl(gb, D);
    if (!(F & FC)) {
      CYCT(0x0b23, 0x0b27);
      if (F & FZ) { CYCT(0x0b3f, 0x0b40); return; }
      CYC(0x0b3f, 0x0b40);
      A = L;
      alu_add(gb, 0x08);
      L = A;
      CYC(0x0b40, 0x0b46);
      continue;
    }
    CYC(0x0b23, 0x0b27);
    A = L;
    alu_or(gb, 0x80);
    CYC(0x0b27, 0x0b2b); mem_wr(gb, 0xff00 | C, A);
    C = alu_inc8(gb, C);
    CYC(0x0b2b, 0x0b2c);
    for (uint16_t a = 0x0b2c; a < 0x0b3c; a += 2) {
      CYC(a, a + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(a + 1, a + 2); mem_wr(gb, 0xff00 | C, A);
    }
    C = alu_dec8(gb, C);
    CYC(0x0b3c, 0x0b3f);
  }
}

void updateDirtyPalettes_hook(GB *gb) {
  A = 0x02;
  CYC(0x0b07, 0x0b0b); mem_wr(gb, IO_SVBK, A);
  CYC(0x0b0b, 0x0b0d); A = H8(hDirtyBgPalettes);
  D = A;
  alu_xor(gb, A);
  CYC(0x0b0d, 0x0b11); H8(hDirtyBgPalettes) = A;
  C = 0x68;
  SET_HL(w2BgPalettesBuffer);
  CYC(0x0b11, 0x0b19); push_effect(gb, 0x0b19);
  write_palette_regs(gb);
  pop_effect(gb);
  CYC(0x0b19, 0x0b1b); A = H8(hDirtySprPalettes);
  D = A;
  alu_xor(gb, A);
  CYC(0x0b1b, 0x0b1f); H8(hDirtySprPalettes) = A;
  C = 0x6a;
  L = 0x40;
  CYC(0x0b1f, 0x0b23);
  write_palette_regs(gb);
  ret_effect(gb);
}

// the script engine's asm-call commands and the loader for scripts copied to wBigBuffer

void _interactionActuallyRunScript_hook(GB *gb) {
  bank_push(gb, 0x2518, 0x0c);
  for (;;) {
    CYC(0x2522, 0x2523); A = mem_rd(gb, HL);
    alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x2523, 0x2526);
      bank_pop(gb, 0x2533);
      alu_scf(gb);
      CYC(0x2539, 0x253b);
      ret_effect(gb);
      return;
    }
    CYC(0x2523, 0x2526);
    CALL_ROM(0x2526, ROM_b0c_runScriptCommand);
    if (F & FC) { CYCT(0x2529, 0x252b); continue; }
    CYC(0x2529, 0x252b);
    break;
  }
  bank_pop(gb, 0x252b);
  alu_xor(gb, A);
  CYC(0x2531, 0x2533);
  ret_effect(gb);
}

void _interactionSaveScriptAddress_hook(GB *gb) {
  E = 0x58;
  A = L;
  CYC(0x2588, 0x258c); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  A = H;
  CYC(0x258c, 0x258f); mem_wr(gb, DE, A);
  CYC(0x258f, 0x2590);
  ret_effect(gb);
}

void _scriptCmd_asmRetFunc_hook(GB *gb) {
  CYC(0x25b1, 0x25b2); SET_HL(pop_effect(gb));
  CYC(0x25b2, 0x25b3); SET_AF(pop_effect(gb));
  CYC(0x25b3, 0x25b5); H8(hRomBank) = A;
  CYC(0x25b5, 0x25b8); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x25b8, 0x25ba); A = H8(hActiveObject);
  D = A;
  alu_scf(gb);
  CYC(0x25ba, 0x25bd);
  ret_effect(gb);
}

static void script_setup_asm_call(GB *gb) {
  SET_HL(HL + 1);
  D = 0x15;
  CYC(0x25bd, 0x25c1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(0x25c1, 0x25c3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(0x25c3, 0x25c5);
}

void _scriptFunc_setupAsmCall_hook(GB *gb) {
  script_setup_asm_call(gb);
  ret_effect(gb);
}

static void script_asm_call_tail(GB *gb) {
  CYC(0x259c, 0x259e); A = H8(hRomBank);
  CYC(0x259e, 0x259f); push_effect(gb, AF);
  A = D;
  CYC(0x259f, 0x25a2); H8(hRomBank) = A;
  CYC(0x25a2, 0x25a5); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x25a5, 0x25a6); push_effect(gb, HL);
  SET_HL(ROM__scriptCmd_asmRetFunc);
  CYC(0x25a6, 0x25aa); push_effect(gb, HL);
  CYC(0x25aa, 0x25ac); A = H8(hActiveObject);
  D = A;
  H = B;
  L = C;
  A = E;
  CYC(0x25ac, 0x25b1);
  hook_handoff(gb, HL);
}

void scriptCmd_asmCall_hook(GB *gb) {
  CYC(0x2590, 0x2591); SET_HL(pop_effect(gb));
  CYC(0x2591, 0x2594);
  script_setup_asm_call(gb);
  CYC(0x2594, 0x2596);
  script_asm_call_tail(gb);
}

void scriptCmd_asmCallWithParam_hook(GB *gb) {
  CYC(0x2596, 0x2597); SET_HL(pop_effect(gb));
  CYC(0x2597, 0x259a);
  script_setup_asm_call(gb);
  CYC(0x259a, 0x259b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(0x259b, 0x259c);
  script_asm_call_tail(gb);
}

void scriptCmd_loadScript_hook(GB *gb) {
  CYC(0x25ee, 0x25ef); SET_HL(pop_effect(gb));
  SET_HL(HL + 1);
  CYC(0x25ef, 0x25f1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(0x25f1, 0x25f3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(0x25f3, 0x25f6); H8(hScriptAddressL) = A;
  CYC(0x25f6, 0x25f7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(0x25f7, 0x25fa); H8(hScriptAddressH) = A;
  CYC(0x25fa, 0x25fc); A = H8(hRomBank);
  CYC(0x25fc, 0x25fd); push_effect(gb, AF);
  A = E;
  CYC(0x25fd, 0x2600); H8(hRomBank) = A;
  CYC(0x2600, 0x2603); mem_wr(gb, MBC_ROM_BANK, A);
  H = B;
  L = C;
  SET_DE(wBigBuffer);
  B = 0x00;
  CYC(0x2603, 0x260d);
  copyMemory(gb, DE, HL, 256);
  SET_HL(HL + 256);
  SET_DE(DE + 256);
  CYC(0x260d, 0x260e); SET_AF(pop_effect(gb));
  CYC(0x260e, 0x2610); H8(hRomBank) = A;
  CYC(0x2610, 0x2613); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x2613, 0x2615); A = H8(hActiveObject);
  D = A;
  SET_HL(wBigBuffer);
  alu_scf(gb);
  CYC(0x2615, 0x261b);
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
  uint16_t sp0_ = gb->sp;
  CALL_ROM(0x2cb7, 0x4f45);
  CALL_C(0x2cba, itemIncSubstate_hook, 0x23ef, 0x2cbd);
  CYC(0x2cbd, 0x2cbe); alu_xor(gb, A);
  CYC(0x2cbe, 0x2cc0); L = 0x24;
  CYC(0x2cc0, 0x2cc1); mem_wr(gb, HL, A);
  CALL_C(0x2cc1, clearVariousLinkVariables_hook, 0x2c9b, 0x2cc4);
  CYC(0x2cc4, 0x2cc6); A = 0x80;
  CYC(0x2cc6, 0x2cc9); W8(wLinkInAir) = A;
  CYC(0x2cc9, 0x2ccb); A = 0x03;
  CYC(0x2ccb, 0x2cce);
  specialObjectSetAnimation_hook(gb);
}

void linkState07__substate2_hook(GB *gb) {
  CYC(0x2cce, 0x2ccf); alu_xor(gb, A);
  CYC(0x2ccf, 0x2cd2); W8(wLinkInAir) = A;
  CYC(0x2cd2, 0x2cd4); A = 0x05;
  CYC(0x2cd4, 0x2cd7); W8(wWarpTransition) = A;
  CYC(0x2cd7, 0x2cd9); E = 0x0b;
  CYC(0x2cd9, 0x2cda); A = mem_rd(gb, DE);
  CYC(0x2cda, 0x2cdc); alu_add(gb, 0x04);
  CYC(0x2cdc, 0x2cdd); mem_wr(gb, DE, A);
  CYC(0x2cdd, 0x2cdf); A = 0x0a;
  CYC(0x2cdf, 0x2ce2);
  linkSetState(gb);
}

void linkState07_hook(GB *gb) {
  CYC(0x2cad, 0x2caf); E = 0x05;
  CYC(0x2caf, 0x2cb0); A = mem_rd(gb, DE);
  CYC(0x2cb0, 0x2cb1); push_effect(gb, 0x2cb1);
  switch (rst_jump_table(gb)) {
    case 0x2cb7: linkState07__substate0_hook(gb); return;
    case 0x2aef: specialObjectAnimate_hook(gb); return;
    case 0x2cce: linkState07__substate2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

static void load_large_room_layout(GB *gb) {
  CYC(0x3928, 0x392a); A = H8(hFF8F);
  H = A;
  CYC(0x392a, 0x392d); A = H8(hFF8E);
  L = A;
  SET_BC(0x1000);
  alu_add_hl(gb, BC);
  CYC(0x392d, 0x3934); A = H8(hFF8D);
  CYC(0x3934, 0x3936); H8(hRomBank) = A;
  CYC(0x3936, 0x3939); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x3939, 0x393c); A = W8(wLoadingRoom);
  CYC(0x393c, 0x393d);
  add_double_index_to_hl(gb);
  CYC(0x393d, 0x393e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x393e, 0x393f); H = mem_rd(gb, HL);
  L = A;
  CYC(0x393f, 0x3940); SET_BC(pop_effect(gb));
  alu_add_hl(gb, BC);
  SET_BC(0xfe00);
  alu_add_hl(gb, BC);
  CYC(0x3940, 0x3946);
  CALL_ROM(0x3946, ROM_loadRoomLayout_loadLayoutData);
  SET_DE(wRoomLayout);
  CYC(0x3949, 0x394c);
  for (;;) {
    CYC(0x394c, 0x394d); A = mem_rd(gb, HL); SET_HL(HL + 1);
    B = 0x08;
    CYC(0x394d, 0x394f);
    for (;;) {
      alu_rrca(gb);
      CYC(0x394f, 0x3952); H8(hFF8B) = A;
      if (F & FC) {
        CYCT(0x3952, 0x3954);
        CYC(0x3962, 0x3963); push_effect(gb, BC);
        CYC(0x3963, 0x3964); A = mem_rd(gb, HL); SET_HL(HL + 1);
        C = A;
        CYC(0x3964, 0x3966); A = mem_rd(gb, HL); SET_HL(HL + 1);
        B = A;
        CYC(0x3966, 0x3968); push_effect(gb, HL);
        CALL_ROM(0x3968, ROM_loadRoomLayout_loadLargeRoomLayoutHlpr);
        D = wRoomLayout >> 8;
        CYC(0x396b, 0x396f); A = H8(hFF8D);
        CYC(0x396f, 0x3971); H8(hRomBank) = A;
        CYC(0x3971, 0x3974); mem_wr(gb, MBC_ROM_BANK, A);
        for (;;) {
          CYC(0x3974, 0x3975); A = mem_rd(gb, HL); SET_HL(HL + 1);
          CYC(0x3975, 0x3976); mem_wr(gb, DE, A);
          E = alu_inc8(gb, E);
          A = E;
          alu_cp(gb, 0xb0);
          if (F & FZ) {
            CYCT(0x3976, 0x397c);
            CYC(0x3983, 0x3984); SET_HL(pop_effect(gb));
            CYC(0x3984, 0x3985); SET_BC(pop_effect(gb));
            CYC(0x3985, 0x3986);
            return;
          }
          CYC(0x3976, 0x397c);
          B = alu_dec8(gb, B);
          if (B) { CYCT(0x397c, 0x397f); continue; }
          CYC(0x397c, 0x397f);
          break;
        }
        CYC(0x397f, 0x3980); SET_HL(pop_effect(gb));
        CYC(0x3980, 0x3981); SET_BC(pop_effect(gb));
        CYC(0x3981, 0x3983);
      } else {
        CYC(0x3952, 0x3954);
        CYC(0x3954, 0x3955); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x3955, 0x3956); mem_wr(gb, DE, A);
        E = alu_inc8(gb, E);
        A = E;
        alu_cp(gb, 0xb0);
        if (F & FZ) { CYCT(0x3956, 0x395b); return; }
        CYC(0x3956, 0x395b);
      }
      CYC(0x395b, 0x395d); A = H8(hFF8B);
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x395d, 0x3960); continue; }
      CYC(0x395d, 0x3960);
      CYC(0x3960, 0x3962);
      break;
    }
  }
}

static void decompress_layout_rows(GB *gb, uint16_t a, uint8_t rows, uint16_t helper) {
  SET_DE(wRoomLayout);
  A = rows;
  CYC(a, a + 5);
  for (;;) {
    CYC(a + 5, a + 6); push_effect(gb, AF);
    CALL_ROM(a + 6, helper);
    CYC(a + 9, a + 10); SET_AF(pop_effect(gb));
    A = alu_dec8(gb, A);
    if (A) { CYCT(a + 10, a + 13); continue; }
    CYC(a + 10, a + 14);
    return;
  }
}

static void load_small_room_layout(GB *gb) {
  CYC(0x3986, 0x3988); A = H8(hFF8D);
  CYC(0x3988, 0x398a); H8(hRomBank) = A;
  CYC(0x398a, 0x398d); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x398d, 0x398f); A = H8(hFF8E);
  L = A;
  CYC(0x398f, 0x3992); A = H8(hFF8F);
  H = A;
  CYC(0x3992, 0x3996); A = W8(wLoadingRoom);
  CYC(0x3996, 0x3997);
  add_double_index_to_hl(gb);
  CYC(0x3997, 0x3998); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(0x3998, 0x399a); A = mem_rd(gb, HL);
  E = A;
  alu_and(gb, 0x3f);
  B = A;
  CYC(0x399a, 0x399f); SET_HL(pop_effect(gb));
  alu_add_hl(gb, BC);
  CYC(0x399f, 0x39a0);
  CALL_ROM(0x39a0, ROM_loadRoomLayout_loadLayoutData);
  alu_bit(gb, 7, E);
  if (!(F & FZ)) { CYCT(0x39a3, 0x39a7); decompress_layout_rows(gb, 0x39c1, 0x05, ROM_loadRoomLayout_decompressLayoutMode2Helper); return; }
  CYC(0x39a3, 0x39a7);
  alu_bit(gb, 6, E);
  if (!(F & FZ)) { CYCT(0x39a7, 0x39ab); decompress_layout_rows(gb, 0x39e4, 0x0a, ROM_loadRoomLayout_decompressLayoutMode1Helper); return; }
  CYC(0x39a7, 0x39ab);
  SET_DE(wRoomLayout);
  SET_BC(0x0a08);
  CYC(0x39ab, 0x39b1);
  for (;;) {
    CYC(0x39b1, 0x39b2); push_effect(gb, BC);
    for (;;) {
      CYC(0x39b2, 0x39b3); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x39b3, 0x39b4); mem_wr(gb, DE, A);
      E = alu_inc8(gb, E);
      B = alu_dec8(gb, B);
      if (B) { CYCT(0x39b4, 0x39b8); continue; }
      CYC(0x39b4, 0x39b8);
      break;
    }
    A = E;
    alu_add(gb, 0x06);
    E = A;
    CYC(0x39b8, 0x39bd); SET_BC(pop_effect(gb));
    C = alu_dec8(gb, C);
    if (C) { CYCT(0x39bd, 0x39c0); continue; }
    CYC(0x39bd, 0x39c1);
    return;
  }
}

void loadRoomLayout_hook(GB *gb) {
  SET_HL(wRoomLayout);
  B = 0xc0;
  CYC(0x38dc, 0x38e1);
  CALL_ROM(0x38e1, ROM_clearMemory);
  switch_bank(gb, 0x38e4, 0x04);
  CYC(0x38eb, 0x38ee); A = W8(wTilesetLayoutGroup);
  alu_add(gb, A);
  alu_add(gb, A);
  SET_HL(roomLayoutGroupTable_bank04);
  CYC(0x38ee, 0x38f4);
  add_double_index_to_hl(gb);
  CYC(0x38f4, 0x38f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  B = A;
  CYC(0x38f5, 0x38f7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x38f7, 0x38f9); H8(hFF8D) = A;
  CYC(0x38f9, 0x38fa); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x38fa, 0x38fc); H8(hFF8E) = A;
  CYC(0x38fc, 0x38fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x38fd, 0x38ff); H8(hFF8F) = A;
  CYC(0x38ff, 0x3900); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3900, 0x3902); H8(hFF8C) = A;
  CYC(0x3902, 0x3903); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x3903, 0x3904); H = mem_rd(gb, HL);
  L = A;
  CYC(0x3904, 0x3907); A = H8(hFF8C);
  CYC(0x3907, 0x3909); H8(hRomBank) = A;
  CYC(0x3909, 0x390c); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x390c, 0x390d); push_effect(gb, HL);
  A = B;
  CYC(0x390d, 0x390f); push_effect(gb, 0x390f);
  uint16_t target = rst_jump_table(gb);
  if (target == 0x3928) load_large_room_layout(gb);
  else if (target == 0x3986) load_small_room_layout(gb);
  else { hook_handoff(gb, target); return; }
  ret_effect(gb);
}

// object position on screen while a screen transition scrolls: camera in hFF90-hFF93

void _label_00_152_hook(GB *gb);

void _getObjectPositionOnScreen_duringScreenTransition_hook(GB *gb) {
  D = H;
  A = L;
  alu_and(gb, 0xc0);
  E = A;
  CYC(0x0fc1, 0x0fc7); A = mem_rd(gb, DE);
  alu_and(gb, 0x03);
  alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x0fc7, 0x0fcd);
    _label_00_152_hook(gb);
    return;
  }
  CYC(0x0fc7, 0x0fcd);
  D = 0x00;
  CYC(0x0fcd, 0x0fd0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  alu_add(gb, 0x10);
  C = A;
  A = D;
  alu_adc(gb, A);
  B = A;
  L = alu_inc8(gb, L);
  CYC(0x0fd0, 0x0fd8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(0x0fd8, 0x0fda); push_effect(gb, HL);
  A = L;
  alu_and(gb, 0xc0);
  L = A;
  CYC(0x0fda, 0x0fe0); A = H8(hFF8A);
  CYC(0x0fe0, 0x0fe1); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) CYCT(0x0fe1, 0x0fe3);
  else {
    CYC(0x0fe1, 0x0fe3);
    SET_HL(hFF90);
    CYC(0x0fe3, 0x0fe7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, C);
    C = A;
    CYC(0x0fe7, 0x0fea); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_adc(gb, B);
    B = A;
    CYC(0x0fea, 0x0fed); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, E);
    E = A;
    CYC(0x0fed, 0x0ff0); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_adc(gb, D);
    D = A;
    CYC(0x0ff0, 0x0ff2);
  }
  SET_HL(hCameraY);
  A = C;
  CYC(0x0ff2, 0x0ff7); alu_sub(gb, mem_rd(gb, HL));
  C = A;
  L = alu_inc8(gb, L);
  A = B;
  CYC(0x0ff7, 0x0ffb); alu_sbc(gb, mem_rd(gb, HL));
  B = A;
  bool draw = true;
  if (F & FZ) {
    CYCT(0x0ffb, 0x0ffe);
    A = C;
    alu_cp(gb, 0xb0);
    if (!(F & FC)) { CYCT(0x1008, 0x100d); draw = false; }
    else CYC(0x1008, 0x100d);
  } else {
    CYC(0x0ffb, 0x0ffe);
    A = alu_inc8(gb, A);
    if (!(F & FZ)) { CYCT(0x0ffe, 0x1001); draw = false; }
    else {
      CYC(0x0ffe, 0x1001);
      A = C;
      alu_cp(gb, 0xe0);
      if (F & FC) { CYCT(0x1001, 0x1006); draw = false; }
      else { CYC(0x1001, 0x1006); CYC(0x1006, 0x1008); }
    }
  }
  if (draw) {
    CYC(0x100d, 0x100f); H8(hFF8C) = A;
    B = A;
    L = alu_inc8(gb, L);
    A = E;
    CYC(0x100f, 0x1013); alu_sub(gb, mem_rd(gb, HL));
    E = A;
    L = alu_inc8(gb, L);
    A = D;
    CYC(0x1013, 0x1017); alu_sbc(gb, mem_rd(gb, HL));
    D = A;
    if (F & FZ) {
      CYCT(0x1017, 0x101a);
      A = E;
      alu_cp(gb, 0xb8);
      if (!(F & FC)) { CYCT(0x1024, 0x1029); draw = false; }
      else CYC(0x1024, 0x1029);
    } else {
      CYC(0x1017, 0x101a);
      A = alu_inc8(gb, A);
      if (!(F & FZ)) { CYCT(0x101a, 0x101d); draw = false; }
      else {
        CYC(0x101a, 0x101d);
        A = E;
        alu_cp(gb, 0xe8);
        if (F & FC) { CYCT(0x101d, 0x1022); draw = false; }
        else { CYC(0x101d, 0x1022); CYC(0x1022, 0x1024); }
      }
    }
  }
  if (draw) {
    CYC(0x1029, 0x102b); H8(hFF8D) = A;
    D = B;
    CYC(0x102b, 0x102d); SET_HL(pop_effect(gb));
    L = alu_inc8(gb, L);
    CYC(0x102d, 0x102f); E = mem_rd(gb, HL);
    A = L;
    alu_and(gb, 0xc0);
    alu_add(gb, OBJ_VISIBLE);
    L = A;
    CYC(0x102f, 0x1036); A = mem_rd(gb, HL);
    alu_rlca(gb);
    if (!(F & FC)) { CYCT(0x1036, 0x1038); ret_effect(gb); return; }
    CYC(0x1036, 0x1038);
    alu_rlca(gb);
    CYC(0x1038, 0x1039);
    if (F & FC) { CYCT(0x1039, 0x103c); draw_object_terrain_effects(gb); }
    else CYC(0x1039, 0x103c);
    A = D;
    alu_add(gb, E);
    CYC(0x103c, 0x1040); H8(hFF8C) = A;
    A = L;
    alu_and(gb, 0xc0);
    alu_add(gb, 0x1c);
    L = A;
    alu_scf(gb);
    CYC(0x1040, 0x1048);
    ret_effect(gb);
    return;
  }
  CYC(0x1048, 0x1049); SET_HL(pop_effect(gb));
  A = L;
  alu_and(gb, 0xc0);
  L = A;
  CYC(0x1049, 0x104f); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) CYCT(0x104f, 0x1051);
  else {
    CYC(0x104f, 0x1051);
    alu_or(gb, OBJ_VISIBLE);
    L = A;
    CYC(0x1051, 0x1056); mem_wr(gb, HL, 0x00);
  }
  alu_xor(gb, A);
  CYC(0x1056, 0x1058);
  ret_effect(gb);
}

void _label_00_152_hook(GB *gb) {
  CYC(0x0faf, 0x0fb1); A = H8(hCameraX);
  CYC(0x0fb1, 0x0fb2); C = A;
  CYC(0x0fb2, 0x0fb4); A = H8(hCameraY);
  CYC(0x0fb4, 0x0fb5); B = A;
  CYC(0x0fb5, 0x0fb6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0fb6, 0x0fb7); alu_sub(gb, B);
  CYC(0x0fb7, 0x0fb9); alu_add(gb, 0x10);
  CYC(0x0fb9, 0x0fbb); H8(hFF8C) = A;
  CYC(0x0fbb, 0x0fbc); D = A;
  CYC(0x0fbc, 0x0fbd); L = alu_inc8(gb, L);
  CYC(0x0fbd, 0x0fbe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0fbe, 0x0fbf); alu_sub(gb, C);
  CYC(0x0fbf, 0x0fc1);
  object_position_on_screen_tail(gb);
  ret_effect(gb);
}

// angle towards an object read from (de)

void getRelativeAngle_hook(GB *gb) {
  CYC(0x1ea9, 0x1eaa); A = mem_rd(gb, DE);
  CYC(0x1eaa, 0x1eac); H8(hFF8F) = A;
  E = alu_inc8(gb, E);
  E = alu_inc8(gb, E);
  CYC(0x1eac, 0x1eaf); A = mem_rd(gb, DE);
  CYC(0x1eaf, 0x1eb1); H8(hFF8E) = A;
  object_get_relative_angle_with_temp_vars(gb);
  ret_effect(gb);
}

// sidescrolling gravity with a landing check against the tile below

void objectUpdateSpeedZ_sidescroll_givenYOffset_hook(GB *gb) {
  CYC(0x1f68, 0x1f6a); H8(hFF8B) = A;
  CYC(0x1f6a, 0x1f6c); A = H8(hActiveObjectType);
  alu_add(gb, 0x15);
  L = A;
  H = D;
  CYC(0x1f6c, 0x1f72); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYC(0x1f72, 0x1f74);
    alu_add(gb, 0xf6);
    L = A;
    CYC(0x1f74, 0x1f78); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_add(gb, B);
    B = A;
    L = alu_inc8(gb, L);
    CYC(0x1f78, 0x1f7c); A = mem_rd(gb, HL);
    alu_sub(gb, 0x04);
    C = A;
    CYC(0x1f7c, 0x1f7f);
    CALL_ROM(0x1f7f, ROM_checkTileCollisionAt_allowHoles);
    if (F & FC) { CYCT(0x1f82, 0x1f83); ret_effect(gb); return; }
    CYC(0x1f82, 0x1f83);
    A = C;
    alu_add(gb, 0x07);
    C = A;
    CYC(0x1f83, 0x1f87);
    CALL_ROM(0x1f87, ROM_checkTileCollisionAt_allowHoles);
    if (F & FC) { CYCT(0x1f8a, 0x1f8b); ret_effect(gb); return; }
    CYC(0x1f8a, 0x1f8b);
  } else CYCT(0x1f72, 0x1f74);
  CYC(0x1f8b, 0x1f8d); A = H8(hActiveObjectType);
  alu_add(gb, 0x0a);
  E = A;
  alu_add(gb, 0x0a);
  L = A;
  H = D;
  CYC(0x1f8d, 0x1f94);
  CALL_ROM(0x1f94, ROM_add16BitRefs);
  L = alu_dec8(gb, L);
  CYC(0x1f97, 0x1f9a); A = H8(hFF8B);
  CYC(0x1f9a, 0x1f9b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x1f9b, 0x1f9c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  A = 0x00;
  CYC(0x1f9c, 0x1f9f); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x1f9f, 0x1fa0); mem_wr(gb, HL, A);
  alu_or(gb, D);
  CYC(0x1fa0, 0x1fa2);
  ret_effect(gb);
}

// bank-switched wrappers: treasure display, object gfx, damage, files, cutscene objects

void interactWithTileBeforeLink_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  bank_push(gb, 0x1280, 0x06);
  CALL_C(0x128a, interactWithTileBeforeLink_b06_hook, 0x4000, 0x128d);
  C = alu_rl(gb, C);
  CYC(0x128d, 0x128f);
  bank_pop(gb, 0x128f);
  C = alu_srl(gb, C);
  CYC(0x1295, 0x1298);
  ret_effect(gb);
}

void reloadObjectGfx_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  bank3f_push(gb, 0x1630);
  CALL_C(0x163a, reloadObjectGfx_b3f_hook, ROM_b3f_reloadObjectGfx, 0x163d);
  bank_pop_af(gb, 0x163d);
  CYC(0x1643, 0x1644);
  ret_effect(gb);
}

void loadWeaponGfx_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  E = A;
  CYC(0x166d, 0x166e);
  bank3f_push(gb, 0x166e);
  CALL_C(0x1678, loadWeaponGfx_b3f_hook, ROM_b3f_loadWeaponGfx, 0x167b);
  bank_pop_af(gb, 0x167b);
  CYC(0x1681, 0x1682);
  ret_effect(gb);
}

void loadTreasureDisplayData_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  L = A;
  CYC(0x16d6, 0x16d7);
  bank3f_push(gb, 0x16d7);
  CALL_C(0x16e1, loadTreasureDisplayData_b3f_hook, ROM_b3f_loadTreasureDisplayData, 0x16e4);
  bank_pop_af(gb, 0x16e4);
  CYC(0x16ea, 0x16eb);
  ret_effect(gb);
}

void checkTreasureObtained_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x1748, 0x1749); push_effect(gb, HL);
  CYC(0x1749, 0x174a);
  L = A;
  alu_or(gb, A);
  if (F & FZ) CYCT(0x174a, 0x174d);
  else {
    CYC(0x174a, 0x174d);
    bank_push(gb, 0x174d, 0x3f);
    CALL_C(0x1757, checkTreasureObtained_body_hook,
        ROM_b3f_checkTreasureObtained_body, 0x175a);
    bank_pop(gb, 0x175a);
    A = L;
    H = alu_srl(gb, H);
    CYC(0x1760, 0x1763);
  }
  CYC(0x1763, 0x1764); SET_HL(pop_effect(gb));
  CYC(0x1764, 0x1765);
  ret_effect(gb);
}

void makeActiveObjectFollowLink_b00_hook(GB *gb) { banked_body_call(gb, 0x1832, 0x01, ROM_b01_makeActiveObjectFollowLink); }
void specialObjectCode_companionCutscene_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  bank_push(gb, 0x2d8e, 0x06);
  CALL_C(0x2d98, specialObjectCode_companionCutscene_b06_hook,
         ROM_b06_specialObjectCode_companionCutscene, 0x2d9b);
  bank_pop(gb, 0x2d9b);
  CYC(0x2da1, 0x2da2);
  ret_effect(gb);
}

void specialObjectCode_linkInCutscene_b00_hook(GB *gb) {
  SET_HL(ROM_b06_specialObjectCode_linkInCutscene);
  E = 0x06;
  CYC(0x2da2, 0x2daa);
  interBankCall_hook(gb);
}

void linkApplyDamage_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x2ab7, 0x2ab8); push_effect(gb, DE);
  CYC(0x2ab8, 0x2aba); A = H8(hRomBank);
  CYC(0x2aba, 0x2abb); push_effect(gb, AF);
  D = w1Link >> 8;
  A = 0x05;
  CYC(0x2abb, 0x2ac1); H8(hRomBank) = A;
  CYC(0x2ac1, 0x2ac4); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(0x2ac4, linkApplyDamage_b5_hook, 0x4270, 0x2ac7);
  bank_pop(gb, 0x2ac7);
  CYC(0x2acd, 0x2ace); SET_DE(pop_effect(gb));
  CYC(0x2ace, 0x2acf);
  ret_effect(gb);
}

void checkUseItems_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  C = 0x02;
  CYC(0x2c18, 0x2c1a);
  bank6_function_caller(gb, sp0_);
  ret_effect(gb);
}

void parseGivenObjectData_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  bank_push(gb, 0x3171, 0x12);
  CYC(0x317b, 0x317c); push_effect(gb, DE);
  D = H;
  E = L;
  CYC(0x317c, 0x317e);
  CALL_C(0x317e, parseGivenObjectData_b12_hook, ROM_b12_parseGivenObjectData, 0x3181);
  CYC(0x3181, 0x3182); SET_DE(pop_effect(gb));
  bank_pop(gb, 0x3182);
  CYC(0x3188, 0x3189);
  ret_effect(gb);
}

void checkObjectIsCloseToPosition_b00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x3d59, 0x3d5b); H8(hFF8B) = A;
  bank_push(gb, 0x3d5b, 0x08);
  CALL_C(0x3d65, checkObjectIsCloseToPosition_b08_hook, ROM_b08_checkObjectIsCloseToPosition, 0x3d68);
  B = 0x00;
  if (!(F & FC)) CYCT(0x3d68, 0x3d6c);
  else { CYC(0x3d68, 0x3d6d); B = alu_inc8(gb, B); }
  bank_pop(gb, 0x3d6d);
  A = B;
  alu_or(gb, A);
  if (F & FZ) { CYCT(0x3d73, 0x3d76); ret_effect(gb); return; }
  CYC(0x3d73, 0x3d76);
  alu_scf(gb);
  CYC(0x3d76, 0x3d78);
  ret_effect(gb);
}

static void file_management(GB *gb, uint16_t a, uint8_t op) {
  C = op;
  CYC(a, a + 2);
  if (a != 0x09e0) CYC(a + 2, a + 4);
  bank_push(gb, 0x09e2, 0x07);
  CALL_ROM(0x09ec, ROM_b07_fileManagementFunction);
  C = A;
  CYC(0x09ef, 0x09f0);
  bank_pop(gb, 0x09f0);
  A = C;
  CYC(0x09f6, 0x09f8);
  ret_effect(gb);
}

void initializeFile_b00_hook(GB *gb) { file_management(gb, 0x09d4, 0x00); }
void saveFile_b00_hook(GB *gb) { file_management(gb, 0x09d8, 0x01); }
void loadFile_b00_hook(GB *gb) { file_management(gb, 0x09dc, 0x02); }
void eraseFile_b00_hook(GB *gb) { file_management(gb, 0x09e0, 0x03); }

void initSound_b00_hook(GB *gb) {
  SET_BC(0x4000);
  CYC(0x0cb7, 0x0cba);
  start_sound(gb);
  ret_effect(gb);
}

static void intro_cinematic_after_call(GB *gb, uint16_t sp0_) {
  switch_bank(gb, 0x2d27, 0x05);
  CALL_C(0x2d2e, updateSpecialObjects_hook, 0x4000, 0x2d31);
  CALL_C(0x2d31, loadLinkAndCompanionAnimationFrame_hook, 0x2b25, 0x2d34);
  switch_bank(gb, 0x2d34, 0x04);
  CALL_C(0x2d3b, updateAnimations_hook, 0x5906, 0x2d3e);
  CALL_C(0x2d3e, updateInteractionsAndDrawAllSprites_hook, 0x351e, 0x2d41);
  bank_pop(gb, 0x2d41);
  CYC(0x2d47, 0x2d48);
  ret_effect(gb);
}

void intro_cinematic_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  bank_push(gb, 0x2d1a, 0x03);
  CALL_C(0x2d24, runIntroCinematic_hook, 0x4e20, 0x2d27);
  intro_cinematic_after_call(gb, sp0_);
}

void intro_cinematic__afterCall2d27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  intro_cinematic_after_call(gb, sp0_);
}

// the enemy state machine's common prologue: returns the case index in c

void enemyStandardUpdate_hook(GB *gb) {
  H = D;
  L = 0x84;
  CYC(0x28fd, 0x2901); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x2901, 0x2904);
    inter_bank_call(gb, 0x291e, 0x3f, ROM_b3f_enemyLoadGraphicsAndProperties);
    CALL_ROM(0x2926, ROM_getRandomNumber_noPreserveVars);
    E = 0xbd;
    CYC(0x2929, 0x292c); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    A = 0x01;
    CYC(0x292c, 0x2930); mem_wr(gb, DE, A);
    CYC(0x2930, 0x2932);
    C = 0x00;
    CYC(0x291b, 0x291e);
    ret_effect(gb);
    return;
  }
  CYC(0x2901, 0x2904);
  L = 0xaa;
  CYC(0x2904, 0x2908); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x2908, 0x290a); C = 0x04; CYC(0x2932, 0x2935); ret_effect(gb); return; }
  CYC(0x2908, 0x290a);
  E = 0xad;
  CYC(0x290a, 0x290d); A = mem_rd(gb, DE);
  alu_and(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(0x290d, 0x2911);
    L = E;
    CYC(0x2935, 0x2937); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    C = 0x05;
    CYC(0x2937, 0x293a);
    ret_effect(gb);
    return;
  }
  CYC(0x290d, 0x2911);
  L = alu_dec8(gb, L);
  CYC(0x2911, 0x2913); A = mem_rd(gb, HL);
  alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x2913, 0x2916);
    L = 0xbf;
    CYC(0x293a, 0x293e); alu_bit(gb, 1, mem_rd(gb, HL));
    if (!(F & FZ)) { CYCT(0x293e, 0x2940); C = 0x00; CYC(0x291b, 0x291e); ret_effect(gb); return; }
    CYC(0x293e, 0x2940);
    C = 0x03;
    CYC(0x2940, 0x2943);
    ret_effect(gb);
    return;
  }
  CYC(0x2913, 0x2916);
  E = alu_inc8(gb, E);
  CYC(0x2916, 0x2918); A = mem_rd(gb, DE);
  alu_or(gb, A);
  if (F & FZ) { CYC(0x2918, 0x291b); C = 0x00; CYC(0x291b, 0x291e); ret_effect(gb); return; }
  CYCT(0x2918, 0x291b);
  CYC(0x2943, 0x2946); A = W8(wFrameCounter);
  alu_rrca(gb);
  if (F & FC) {
    CYC(0x2946, 0x2949);
    L = E;
    CYC(0x2949, 0x294b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x294b, 0x294c); A = mem_rd(gb, HL);
    alu_cp(gb, 0x1e);
    if (F & FC) {
      CYC(0x294c, 0x2950);
      alu_rrca(gb);
      if (F & FC) {
        CYC(0x2950, 0x2953);
        L = 0x8d;
        CYC(0x2953, 0x2956); A = mem_rd(gb, HL);
        alu_xor(gb, 0x01);
        CYC(0x2956, 0x2959); mem_wr(gb, HL, A);
      } else CYCT(0x2950, 0x2953);
    } else CYCT(0x294c, 0x2950);
  } else CYCT(0x2946, 0x2949);
  L = 0x84;
  CYC(0x2959, 0x295c); A = mem_rd(gb, HL);
  alu_cp(gb, 0x08);
  if (F & FC) CYCT(0x295c, 0x2960);
  else {
    CYC(0x295c, 0x2960);
    L = 0x8f;
    CYC(0x2960, 0x2963); A = mem_rd(gb, HL);
    A = alu_dec8(gb, A);
    alu_cp(gb, 0x08);
    if (F & FC) CYCT(0x2963, 0x2968);
    else {
      CYC(0x2963, 0x2968);
      C = 0x20;
      CYC(0x2968, 0x296a);
      CALL_ROM(0x296a, ROM_objectUpdateSpeedZAndBounce);
      if (!(F & FC)) { CYCT(0x296d, 0x296f); C = 0x02; CYC(0x2975, 0x2978); ret_effect(gb); return; }
      CYC(0x296d, 0x296f);
      H = D;
      CYC(0x296f, 0x2970);
    }
  }
  L = 0x94;
  alu_xor(gb, A);
  CYC(0x2970, 0x2974); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x2974, 0x2975); mem_wr(gb, HL, A);
  C = 0x02;
  CYC(0x2975, 0x2978);
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

void threadFunc_088b_hook(GB *gb) { thread_state_flag(gb, 0x088b, true); }
void threadFunc_0893_hook(GB *gb) { thread_state_flag(gb, 0x0893, false); }

void threadStop_hook(GB *gb) {
  CYC(0x089b, 0x089c); push_effect(gb, HL);
  L = A;
  H = wThreadStateBuffer >> 8;
  CYC(0x089c, 0x089f);
  CYC(0x089f, 0x08a1); mem_wr(gb, HL, 0x00);
  CYC(0x08a1, 0x08a2); SET_HL(pop_effect(gb));
  CYC(0x08a2, 0x08a3);
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
  CYC(0x08a3, 0x08a4); push_effect(gb, HL);
  CYC(0x08a4, 0x08a5); push_effect(gb, DE);
  CYC(0x08a5, 0x08a6); push_effect(gb, BC);
  E = A;
  alu_add(gb, 0x04);
  C = A;
  D = 0x00;
  SET_HL(ROM_initialThreadStatesBase);
  alu_add_hl(gb, DE);
  D = wThreadStateBuffer >> 8;
  B = 0x08;
  CYC(0x08a6, 0x08b4);
  copy_initial_thread_state(gb, 0x08b4);
  set_thread_restart_address(gb, 0x08ba);
  CYC(0x08c0, 0x08c1); SET_DE(pop_effect(gb));
  CYC(0x08c1, 0x08c2); SET_HL(pop_effect(gb));
  CYC(0x08c2, 0x08c3);
  ret_effect(gb);
}

void restartThisThread_hook(GB *gb) {
  CYC(0x08c3, 0x08c4); push_effect(gb, BC);
  A = H8(hActiveThread);
  E = A;
  alu_add(gb, 0x04);
  C = A;
  D = 0x00;
  SET_HL(ROM_initialThreadStatesBase);
  alu_add_hl(gb, DE);
  D = wThreadStateBuffer >> 8;
  B = 0x08;
  CYC(0x08c4, 0x08d4);
  copy_initial_thread_state(gb, 0x08d4);
  set_thread_restart_address(gb, 0x08da);
  CYCT(0x08e0, 0x08e2);
  _nextThread_hook(gb);
}

void stubThreadStart_hook(GB *gb) {
  A = H8(hActiveThread);
  L = A;
  H = wThreadStateBuffer >> 8;
  CYC(0x08e2, 0x08e7);
  CYC(0x08e7, 0x08e9); mem_wr(gb, HL, 0x00);
  CYCT(0x08e9, 0x08eb);
  _nextThread_hook(gb);
}

void resumeThreadNextFrameAndSaveBank_hook(GB *gb) {
  A = 0x01;
  CYC(0x08eb, 0x08ed);
  CYC(0x08ed, 0x08ee); push_effect(gb, BC);
  B = A;
  A = H8(hRomBank);
  C = A;
  A = B;
  CYC(0x08ee, 0x08f3);
  CYC(0x08f3, 0x08f6); push_effect(gb, 0x08f6);
  resumeThreadInAFrames_hook(gb);
  A = C;
  CYC(0x08f6, 0x08f7);
  CYC(0x08f7, 0x08f9); H8(hRomBank) = A;
  CYC(0x08f9, 0x08fc); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x08fc, 0x08fd); SET_BC(pop_effect(gb));
  CYC(0x08fd, 0x08fe);
  ret_effect(gb);
}

void resumeThreadNextFrame_hook(GB *gb) {
  A = 0x01;
  CYC(0x08fe, 0x0900);
  resumeThreadInAFrames_hook(gb);
}

void resumeThreadInAFrames_hook(GB *gb) {
  CYC(0x0900, 0x0901); push_effect(gb, HL);
  CYC(0x0901, 0x0902); push_effect(gb, DE);
  CYC(0x0902, 0x0903); push_effect(gb, BC);
  B = A;
  CYC(0x0903, 0x0904);
  CYC(0x0904, 0x0906); A = mem_rd(gb, hActiveThread);
  L = A;
  CYC(0x0906, 0x0907);
  H = wThreadStateBuffer >> 8;
  CYC(0x0907, 0x0909);
  A = 0x01;
  CYC(0x0909, 0x090b);
  CYC(0x090b, 0x090c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x090c, 0x090d); mem_wr(gb, HL, B);
  L = alu_inc8(gb, L);
  CYC(0x090d, 0x090e);
  burn_store_sp(gb, 0x090e, hFF92);
  CYC(0x0911, 0x0913); A = mem_rd(gb, hFF92);
  CYC(0x0913, 0x0914); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x0914, 0x0916); A = mem_rd(gb, hFF93);
  CYC(0x0916, 0x0917); mem_wr(gb, HL, A);
  _nextThread_hook(gb);
}

void writeToSC_hook(GB *gb) {
  CYC(0x0c6a, 0x0c6b); push_effect(gb, AF);
  alu_and(gb, 0x01);
  CYC(0x0c6b, 0x0c6d);
  CYC(0x0c6d, 0x0c6f); mem_wr(gb, IO_SC, A);
  CYC(0x0c6f, 0x0c70); SET_AF(pop_effect(gb));
  CYC(0x0c70, 0x0c72); mem_wr(gb, IO_SC, A);
  CYC(0x0c72, 0x0c73);
  ret_effect(gb);
}

static void serial_reti(GB *gb, uint16_t a) {
  CYC(a, a + 1); SET_AF(pop_effect(gb));
  CYC(a + 1, a + 2);
  reti_effect(gb);
}

void serialInterrupt_hook(GB *gb) {
  A = H8(hSerialInterruptBehaviour);
  alu_or(gb, A);
  CYC(0x0c3d, 0x0c40);
  if (!(F & FZ)) {
    CYC(0x0c40, 0x0c42);
    CYC(0x0c42, 0x0c44); A = mem_rd(gb, IO_SB);
    CYC(0x0c44, 0x0c46); H8(hSerialByte) = A;
    alu_xor(gb, A);
    CYC(0x0c46, 0x0c47);
    CYC(0x0c47, 0x0c49); mem_wr(gb, IO_SB, A);
    A = alu_inc8(gb, A);
    CYC(0x0c49, 0x0c4a);
    CYC(0x0c4a, 0x0c4c); H8(hReceivedSerialByte) = A;
    serial_reti(gb, 0x0c4c);
    return;
  }
  CYCT(0x0c40, 0x0c42);
  CYC(0x0c4e, 0x0c50); A = mem_rd(gb, IO_SB);
  alu_cp(gb, 0xe1);
  CYC(0x0c50, 0x0c52);
  if (F & FZ) CYCT(0x0c52, 0x0c54);
  else {
    CYC(0x0c52, 0x0c54);
    alu_cp(gb, 0xe0);
    CYC(0x0c54, 0x0c56);
    if (!(F & FZ)) {
      CYCT(0x0c56, 0x0c58);
      A = 0xe1;
      CYC(0x0c5f, 0x0c61);
      CYC(0x0c61, 0x0c63); mem_wr(gb, IO_SB, A);
      A = 0x80;
      CYC(0x0c63, 0x0c65);
      CALL_ROM(0x0c65, ROM_writeToSC);
      serial_reti(gb, 0x0c68);
      return;
    }
    CYC(0x0c56, 0x0c58);
  }
  CYC(0x0c58, 0x0c5a); H8(hSerialInterruptBehaviour) = A;
  alu_xor(gb, A);
  CYC(0x0c5a, 0x0c5b);
  CYC(0x0c5b, 0x0c5d); mem_wr(gb, IO_SB, A);
  serial_reti(gb, 0x0c5d);
}

void serialFunc_0c73_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x0c73, 0x0c74);
  CYC(0x0c74, 0x0c76); H8(hFFBD) = A;
  A = 0xe0;
  CYC(0x0c76, 0x0c78);
  CYC(0x0c78, 0x0c7a); mem_wr(gb, IO_SB, A);
  A = 0x81;
  CYC(0x0c7a, 0x0c7c);
  CYCT(0x0c7c, 0x0c7e);
  writeToSC_hook(gb);
}

void disableSerialPort_hook(GB *gb) {
  alu_xor(gb, A);
  CYC(0x0c7e, 0x0c7f);
  CYC(0x0c7f, 0x0c81); H8(hSerialInterruptBehaviour) = A;
  CYC(0x0c81, 0x0c83); mem_wr(gb, IO_SB, A);
  CYCT(0x0c83, 0x0c85);
  writeToSC_hook(gb);
}

void serialFunc_0c85_hook(GB *gb) {
  SET_HL(ROM_b16_serialFunc_44ac);
  E = 0x16;
  CYC(0x0c85, 0x0c8a);
  CYC(0x0c8a, 0x0c8d);
  interBankCall_hook(gb);
}

void serialFunc_0c8d_hook(GB *gb) {
  CYC(0x0c8d, 0x0c8e); push_effect(gb, DE);
  SET_HL(ROM_b16_serialFunc_4000);
  E = 0x16;
  CYC(0x0c8e, 0x0c93);
  CALL_ROM(0x0c93, ROM_interBankCall);
  CYC(0x0c96, 0x0c97); SET_DE(pop_effect(gb));
  CYC(0x0c97, 0x0c98);
  ret_effect(gb);
}

static void timer_interrupt_end(GB *gb) {
  CYC(0x0d59, 0x0d5a); SET_HL(pop_effect(gb));
  CYC(0x0d5a, 0x0d5b); SET_DE(pop_effect(gb));
  CYC(0x0d5b, 0x0d5c); SET_BC(pop_effect(gb));
  CYC(0x0d5c, 0x0d5d); SET_AF(pop_effect(gb));
  CYC(0x0d5d, 0x0d5e);
  reti_effect(gb);
}

void timerInterrupt_hook(GB *gb) {
  SET_HL(hFFB7);
  CYC(0x0d07, 0x0d0a);
  CYC(0x0d0a, 0x0d0c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x0d0c, 0x0d0e); timer_interrupt_end(gb); return; }
  CYC(0x0d0c, 0x0d0e);
  CYC(0x0d0e, 0x0d10); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x0d10, 0x0d12); timer_interrupt_end(gb); return; }
  CYC(0x0d10, 0x0d12);
  CYC(0x0d12, 0x0d14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x01));
  L = alu_inc8(gb, L);
  CYC(0x0d14, 0x0d15);
  CYC(0x0d15, 0x0d16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    CYC(0x0d16, 0x0d18);
    CYC(0x0d18, 0x0d1a); mem_wr(gb, HL, 0x07);
    CYC(0x0d1a, 0x0d1c); A = mem_rd(gb, IO_TMA);
    A = alu_dec8(gb, A);
    CYC(0x0d1c, 0x0d1d);
    CYC(0x0d1d, 0x0d1f); mem_wr(gb, IO_TIMA, A);
  } else CYCT(0x0d16, 0x0d18);
  A = 0x39;
  CYC(0x0d1f, 0x0d21);
  CYC(0x0d21, 0x0d24); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x0d24, 0x0d26); A = H8(hMusicVolume);
  alu_bit(gb, 7, A);
  CYC(0x0d26, 0x0d28);
  if (F & FZ) CYCT(0x0d28, 0x0d2a);
  else {
    CYC(0x0d28, 0x0d2a);
    alu_and(gb, 0x03);
    CYC(0x0d2a, 0x0d2c);
    CYC(0x0d2c, 0x0d2e); H8(hMusicVolume) = A;
    CALL_ROM(0x0d2e, ROM_b39_updateMusicVolume);
  }
  CYC(0x0d31, 0x0d33); A = H8(hMusicQueueTail);
  B = A;
  CYC(0x0d33, 0x0d34);
  CYC(0x0d34, 0x0d36); A = H8(hMusicQueueHead);
  alu_cp(gb, B);
  CYC(0x0d36, 0x0d37);
  if (F & FZ) CYCT(0x0d37, 0x0d39);
  else {
    CYC(0x0d37, 0x0d39);
    H = wMusicQueue >> 8;
    CYC(0x0d39, 0x0d3b);
    for (;;) {
      L = A;
      CYC(0x0d3b, 0x0d3c);
      CYC(0x0d3c, 0x0d3d); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x0d3d, 0x0d3e); push_effect(gb, BC);
      CYC(0x0d3e, 0x0d3f); push_effect(gb, HL);
      CALL_ROM(0x0d3f, ROM_b39_playSound);
      CYC(0x0d42, 0x0d43); SET_HL(pop_effect(gb));
      CYC(0x0d43, 0x0d44); SET_BC(pop_effect(gb));
      A = L;
      alu_and(gb, 0xaf);
      alu_cp(gb, B);
      CYC(0x0d44, 0x0d48);
      if (F & FZ) { CYC(0x0d48, 0x0d4a); break; }
      CYCT(0x0d48, 0x0d4a);
    }
    CYC(0x0d4a, 0x0d4c); H8(hMusicQueueHead) = A;
  }
  CALL_ROM(0x0d4c, ROM_b39_updateSound);
  SET_HL(hFFB7);
  CYC(0x0d4f, 0x0d52);
  CYC(0x0d52, 0x0d54); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0xfe));
  CYC(0x0d54, 0x0d56); A = H8(hRomBank);
  CYC(0x0d56, 0x0d59); mem_wr(gb, MBC_ROM_BANK, A);
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
  CYC(0x0a74, 0x0a75); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0a75, 0x0a76); push_effect(gb, HL);
  C = A;
  B = 0x00;
  SET_HL(ROM_vblankFunctionsStart);
  CYC(0x0a76, 0x0a7c);
  alu_add_hl(gb, BC);
  CYC(0x0a7c, 0x0a7d);
  CYC(0x0a7d, 0x0a7e);
  hook_continue(gb, HL, sp0);
}

void runVBlankFunctions_hook(GB *gb) {
  uint16_t sp0 = gb->sp;
  SET_HL(wVBlankFunctionQueue);
  CYC(0x0a71, 0x0a74);
  vblank_queue_dispatch(gb, sp0);
}

void vblankFunctionRet_hook(GB *gb) {
  uint16_t sp0 = gb->sp;
  CYC(0x0a7e, 0x0a80); A = H8(hVBlankFunctionQueueTail);
  alu_cp(gb, L);
  CYC(0x0a80, 0x0a81);
  if (!(F & FZ)) { CYCT(0x0a81, 0x0a83); vblank_queue_dispatch(gb, sp0); return; }
  CYC(0x0a81, 0x0a83);
  alu_xor(gb, A);
  CYC(0x0a83, 0x0a84);
  CYC(0x0a84, 0x0a86); H8(hVBlankFunctionQueueTail) = A;
  CYC(0x0a86, 0x0a87);
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

void vblankFunctionsStart_hook(GB *gb) { vblank_copy_block(gb, 0x0a8e); }
void vblankFunction0ad9_hook(GB *gb) { vblank_copy_block(gb, 0x0ad9); }

void vblankRunBank4Function_hook(GB *gb) {
  A = 0x04;
  CYC(0x0aa0, 0x0aa2);
  CYC(0x0aa2, 0x0aa5); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x0aa5, 0x0aa8);
  vblankRunBank4Function_b04_hook(gb);
}

void vblankFunction0aa8_hook(GB *gb) {
  uint16_t sp0 = gb->sp;
  CYC(0x0aa8, 0x0aa9); SET_HL(pop_effect(gb));
  CYC(0x0aa9, 0x0aaa); A = mem_rd(gb, HL); SET_HL(HL + 1);
  C = A;
  CYC(0x0aaa, 0x0aab);
  CYC(0x0aab, 0x0aac); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0aac, 0x0aad); push_effect(gb, HL);
  L = C;
  H = A;
  SET_BC(0x0ab4);
  CYC(0x0aad, 0x0ab2);
  CYC(0x0ab2, 0x0ab3); push_effect(gb, BC);
  CYC(0x0ab3, 0x0ab4);
  hook_continue(gb, HL, sp0);
}

static void vblank_write4_bytes(GB *gb) {
  CYC(0x0acc, 0x0acd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0acd, 0x0ace); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  CYC(0x0ace, 0x0acf);
  CYC(0x0acf, 0x0ad0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0ad0, 0x0ad1); mem_wr(gb, DE, A);
  CYC(0x0ad1, 0x0ad2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(0x0ad2, 0x0ad3);
  CYC(0x0ad3, 0x0ad4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0ad4, 0x0ad5); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  CYC(0x0ad5, 0x0ad6);
  CYC(0x0ad6, 0x0ad7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0ad7, 0x0ad8); mem_wr(gb, DE, A);
  CYC(0x0ad8, 0x0ad9);
  ret_effect(gb);
}

void vblankCopyTileFunction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x0ab7, 0x0ab8); SET_HL(pop_effect(gb));
  SET_DE(ROM_vblankFunctionRet);
  CYC(0x0ab8, 0x0abb);
  CYC(0x0abb, 0x0abc); push_effect(gb, DE);
  alu_xor(gb, A);
  CYC(0x0abc, 0x0abd);
  CYC(0x0abd, 0x0abf); mem_wr(gb, IO_VBK, A);
  CYC(0x0abf, 0x0ac0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  E = A;
  CYC(0x0ac0, 0x0ac1);
  CYC(0x0ac1, 0x0ac2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  D = A;
  CYC(0x0ac2, 0x0ac3);
  C = E;
  CYC(0x0ac3, 0x0ac4);
  CYC(0x0ac4, 0x0ac7); push_effect(gb, 0x0ac7);
  vblank_write4_bytes(gb);
  E = C;
  CYC(0x0ac7, 0x0ac8);
  A = 0x01;
  CYC(0x0ac8, 0x0aca);
  CYC(0x0aca, 0x0acc); mem_wr(gb, IO_VBK, A);
  vblank_write4_bytes(gb);
}

void vblankDmaFunction_hook(GB *gb) {
  CYC(0x0aeb, 0x0aec); SET_HL(pop_effect(gb));
  CYC(0x0aec, 0x0aed); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0aed, 0x0aef); mem_wr(gb, IO_SVBK, A);
  CYC(0x0aef, 0x0af2); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x0af2, 0x0af3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0af3, 0x0af5); mem_wr(gb, IO_HDMA1, A);
  CYC(0x0af5, 0x0af6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0af6, 0x0af8); mem_wr(gb, IO_HDMA2, A);
  CYC(0x0af8, 0x0af9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0af9, 0x0afb); mem_wr(gb, IO_VBK, A);
  CYC(0x0afb, 0x0afc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0afc, 0x0afe); mem_wr(gb, IO_HDMA3, A);
  CYC(0x0afe, 0x0aff); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0aff, 0x0b01); mem_wr(gb, IO_HDMA4, A);
  CYC(0x0b01, 0x0b02); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0b02, 0x0b04); mem_wr(gb, IO_HDMA5, A);
  CYC(0x0b04, 0x0b07);
  vblankFunctionRet_hook(gb);
}

void vblankInterrupt_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x09f8, 0x09fa); A = H8(hNextLcdInterruptBehaviour);
  CYC(0x09fa, 0x09fc); H8(hLcdInterruptBehaviour) = A;
  alu_xor(gb, A);
  CYC(0x09fc, 0x09fd);
  CYC(0x09fd, 0x09ff); H8(hLcdInterruptCounter) = A;
  SET_HL(hFFB7);
  CYC(0x09ff, 0x0a02);
  CYC(0x0a02, 0x0a04); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  SET_HL(wGfxRegsFinal);
  CYC(0x0a04, 0x0a07);
  static const uint16_t regs[] = {IO_LCDC, IO_SCY, IO_SCX, IO_WY, IO_WX, IO_LYC};
  uint16_t a = 0x0a07;
  for (int i = 0; i < 6; i++) {
    CYC(a, a + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(a + 1, a + 3); mem_wr(gb, regs[i], A);
    a += 3;
  }
  CYC(0x0a19, 0x0a1a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) CYCT(0x0a1a, 0x0a1c);
  else {
    CYC(0x0a1a, 0x0a1c);
    SET_DE(wGfxRegs2);
    CYC(0x0a1c, 0x0a1f);
    L = wGfxRegs3 & 0xff;
    CYC(0x0a1f, 0x0a21);
    for (int i = 0; i < 6; i++) {
      uint16_t b = (uint16_t)(0x0a21 + i * 3);
      CYC(b, b + 1); A = mem_rd(gb, DE);
      CYC(b + 1, b + 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
      if (i < 5) { E = alu_inc8(gb, E); CYC(b + 2, b + 3); }
    }
    CYC(0x0a32, 0x0a34); A = mem_rd(gb, IO_VBK);
    B = A;
    CYC(0x0a34, 0x0a35);
    CYC(0x0a35, 0x0a37); A = mem_rd(gb, IO_SVBK);
    C = A;
    CYC(0x0a37, 0x0a38);
    CYC(0x0a38, 0x0a39); push_effect(gb, BC);
    CYC(0x0a39, 0x0a3b); A = H8(hVBlankFunctionQueueTail);
    alu_or(gb, A);
    CYC(0x0a3b, 0x0a3c);
    if (!(F & FZ)) CALL_C_CC(0x0a3c, runVBlankFunctions_hook, ROM_runVBlankFunctions, 0x0a3f);
    else CYC(0x0a3c, 0x0a3f);
    CALL_C(0x0a3f, updateDirtyPalettes_hook, ROM_updateDirtyPalettes, 0x0a42);
    CYC(0x0a42, 0x0a43); gb->ime = false; gb->ime_delay = false; gb->ime_writes++;
    CALL_C(0x0a43, hramOamDmaFunction_hook, ROM_hramOamDmaFunction, 0x0a46);
    CYC(0x0a46, 0x0a47); SET_BC(pop_effect(gb));
    A = C;
    CYC(0x0a47, 0x0a48);
    CYC(0x0a48, 0x0a4a); mem_wr(gb, IO_SVBK, A);
    A = B;
    CYC(0x0a4a, 0x0a4b);
    CYC(0x0a4b, 0x0a4d); mem_wr(gb, IO_VBK, A);
    SET_HL(wGfxRegs6_LCDC);
    CYC(0x0a4d, 0x0a50);
    static const uint16_t final[] = {wGfxRegs7_LCDC, wGfxRegs7_SCY, wGfxRegs7_SCX};
    for (int i = 0; i < 3; i++) {
      uint16_t b = (uint16_t)(0x0a50 + i * 4);
      CYC(b, b + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b + 1, b + 4); mem_wr(gb, final[i], A);
    }
  }
  SET_HL(hFFB7);
  CYC(0x0a5c, 0x0a5f);
  CYC(0x0a5f, 0x0a61); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
  CYC(0x0a61, 0x0a63); A = H8(hRomBank);
  CYC(0x0a63, 0x0a65); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) CYCT(0x0a65, 0x0a67);
  else {
    CYC(0x0a65, 0x0a67);
    CYC(0x0a67, 0x0a69); A = H8(hSoundDataBaseBank2);
  }
  CYC(0x0a69, 0x0a6c); mem_wr(gb, MBC_ROM_BANK, A);
  CYC(0x0a6c, 0x0a6d); SET_HL(pop_effect(gb));
  CYC(0x0a6d, 0x0a6e); SET_DE(pop_effect(gb));
  CYC(0x0a6e, 0x0a6f); SET_BC(pop_effect(gb));
  CYC(0x0a6f, 0x0a70); SET_AF(pop_effect(gb));
  CYC(0x0a70, 0x0a71);
  reti_effect(gb);
}

void _lcdInterruptEnd_hook(GB *gb) {
  CYC(0x0be6, 0x0be7); SET_BC(pop_effect(gb));
  CYC(0x0be7, 0x0be8); SET_HL(pop_effect(gb));
  CYC(0x0be8, 0x0be9); SET_AF(pop_effect(gb));
  CYC(0x0be9, 0x0bea);
  reti_effect(gb);
}

void lcdInterrupt_clearLYC_hook(GB *gb) {
  A = 0xc7;
  CYC(0x0be2, 0x0be4); mem_wr(gb, IO_LYC, A);
  CYC(0x0be4, 0x0be6);
  _lcdInterruptEnd_hook(gb);
}

void lcdInterrupt_clearWXY_hook(GB *gb) {
  A = 0xc7;
  CYC(0x0bdc, 0x0bde); mem_wr(gb, IO_WY, A);
  CYC(0x0bde, 0x0be0); mem_wr(gb, IO_WX, A);
  CYC(0x0be0, 0x0be2);
  lcdInterrupt_clearLYC_hook(gb);
}

void lcdInterrupt_setLcdcToA7_hook(GB *gb) {
  for (;;) {
    CYC(0x0bab, 0x0bad); A = mem_rd(gb, IO_STAT);
    CYC(0x0bad, 0x0bae); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(0x0bae, 0x0bb0); continue; }
    CYC(0x0bae, 0x0bb0);
    break;
  }
  A = 0xa7;
  CYC(0x0bb0, 0x0bb2);
  CYC(0x0bb2, 0x0bb4); mem_wr(gb, IO_LCDC, A);
  CYCT(0x0bb4, 0x0bb6);
  lcdInterrupt_clearLYC_hook(gb);
}

void lcdInterrupt_ringMenu_hook(GB *gb) {
  for (;;) {
    CYC(0x0bb6, 0x0bb8); A = mem_rd(gb, IO_STAT);
    CYC(0x0bb8, 0x0bb9); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(0x0bb9, 0x0bbb); continue; }
    CYC(0x0bb9, 0x0bbb);
    break;
  }
  CYC(0x0bbb, 0x0bbd); mem_wr(gb, IO_SCX, A);
  A = 0x87;
  CYC(0x0bbd, 0x0bbf);
  CYC(0x0bbf, 0x0bc1); mem_wr(gb, IO_LCDC, A);
  CYC(0x0bc1, 0x0bc3); A = H8(hLcdInterruptCounter);
  A = alu_dec8(gb, A);
  CYC(0x0bc3, 0x0bc4);
  if (!(F & FZ)) {
    CYCT(0x0bc4, 0x0bc6);
  } else {
    CYC(0x0bc4, 0x0bc6);
    A = mem_rd(gb, wRingMenu_mode);
    CYC(0x0bc6, 0x0bc9);
    alu_or(gb, A);
    CYC(0x0bc9, 0x0bca);
    if (F & FZ) CYCT(0x0bca, 0x0bcc);
    else {
      CYC(0x0bca, 0x0bcc);
      A = 0x87;
      CYC(0x0bcc, 0x0bce);
      CYC(0x0bce, 0x0bd0); mem_wr(gb, IO_LYC, A);
    }
    A = 0x02;
    CYC(0x0bd0, 0x0bd2);
    CYC(0x0bd2, 0x0bd4); H8(hLcdInterruptCounter) = A;
    CYCT(0x0bd4, 0x0bd6);
    _lcdInterruptEnd_hook(gb);
    return;
  }
  A = 0x80;
  CYC(0x0bd6, 0x0bd8);
  CYC(0x0bd8, 0x0bda); mem_wr(gb, IO_SCY, A);
  CYCT(0x0bda, 0x0bdc);
  lcdInterrupt_clearWXY_hook(gb);
}

void lcdInterrupt_0bea_hook(GB *gb) {
  for (;;) {
    CYC(0x0bea, 0x0bec); A = mem_rd(gb, IO_STAT);
    CYC(0x0bec, 0x0bed); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(0x0bed, 0x0bef); continue; }
    CYC(0x0bed, 0x0bef);
    break;
  }
  SET_HL(0xc4a5);
  CYC(0x0bef, 0x0bf2);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0bf2, 0x0bf3);
  CYC(0x0bf3, 0x0bf5); mem_wr(gb, IO_LCDC, A);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0bf5, 0x0bf6);
  CYC(0x0bf6, 0x0bf8); mem_wr(gb, IO_SCY, A);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0bf8, 0x0bf9);
  CYC(0x0bf9, 0x0bfb); mem_wr(gb, IO_SCX, A);
  CYCT(0x0bfb, 0x0bfd);
  lcdInterrupt_clearLYC_hook(gb);
}

static void lcd_interrupt_not_status_bar(GB *gb) {
  A = H8(hLcdInterruptBehaviour);
  CYC(0x0b96, 0x0b98);
  alu_cp(gb, 0x07);
  CYC(0x0b98, 0x0b9a);
  if (!(F & FC)) { CYCT(0x0b9a, 0x0b9c); lcdInterrupt_clearLYC_hook(gb); return; }
  CYC(0x0b9a, 0x0b9c);
  CYC(0x0b9c, 0x0b9d);
  push_effect(gb, 0x0b9d);
  uint16_t target = rst_jump_table(gb);
  if (target == ROM_lcdInterrupt_clearLYC) lcdInterrupt_clearLYC_hook(gb);
  else if (target == ROM_lcdInterrupt_setLcdcToA7) lcdInterrupt_setLcdcToA7_hook(gb);
  else if (target == ROM_lcdInterrupt_clearWXY) lcdInterrupt_clearWXY_hook(gb);
  else if (target == ROM_lcdInterrupt_ringMenu) lcdInterrupt_ringMenu_hook(gb);
  else if (target == ROM_lcdInterrupt_0bea) lcdInterrupt_0bea_hook(gb);
  else hook_handoff(gb, target);
}

void lcdInterrupt_hook(GB *gb) {
  A = H8(hLcdInterruptBehaviour);
  CYC(0x0b46, 0x0b48);
  alu_cp(gb, 0x02);
  CYC(0x0b48, 0x0b4a);
  if (F & FC) {
    CYC(0x0b4a, 0x0b4c);
    alu_or(gb, A);
    A = mem_rd(gb, IO_LY);
    CYC(0x0b4c, 0x0b4f);
    L = A;
    H = wBigBuffer >> 8;
    CYC(0x0b4f, 0x0b52);
    A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0b52, 0x0b53);
    if (F & FZ) {
      CYC(0x0b53, 0x0b55);
      CYC(0x0b55, 0x0b57); mem_wr(gb, IO_SCX, A);
      CYCT(0x0b57, 0x0b59);
    } else {
      CYCT(0x0b53, 0x0b55);
      CYC(0x0b59, 0x0b5b); mem_wr(gb, IO_SCY, A);
    }
    A = L;
    CYC(0x0b5b, 0x0b5c);
    alu_cp(gb, 0x90);
    CYC(0x0b5c, 0x0b5e);
    if (F & FC) { CYC(0x0b5e, 0x0b60); CYC(0x0b60, 0x0b62); mem_wr(gb, IO_LYC, A); }
    else { CYCT(0x0b5e, 0x0b60); }
    CYC(0x0b62, 0x0b63); SET_HL(pop_effect(gb));
    CYC(0x0b63, 0x0b64); SET_AF(pop_effect(gb));
    CYC(0x0b64, 0x0b65);
    reti_effect(gb);
    return;
  }
  CYCT(0x0b4a, 0x0b4c);
  CYC(0x0b65, 0x0b66); push_effect(gb, BC);
  C = 0x03;
  CYC(0x0b66, 0x0b68);
  CYC(0x0b68, 0x0b6a); A = H8(hLcdInterruptCounter);
  alu_or(gb, A);
  CYC(0x0b6a, 0x0b6b);
  if (!(F & FZ)) { CYCT(0x0b6b, 0x0b6d); lcd_interrupt_not_status_bar(gb); return; }
  CYC(0x0b6b, 0x0b6d);
  SET_HL(wGfxRegs3);
  CYC(0x0b6d, 0x0b70);
  for (;;) {
    CYC(0x0b70, 0x0b72); A = mem_rd(gb, IO_STAT);
    CYC(0x0b72, 0x0b73); alu_and(gb, C);
    if (!(F & FZ)) { CYCT(0x0b73, 0x0b75); continue; }
    CYC(0x0b73, 0x0b75);
    break;
  }
  static const uint16_t regs[] = {IO_LCDC, IO_SCY, IO_SCX, IO_WY, IO_WX, IO_LYC};
  uint16_t a = 0x0b75;
  for (int i = 0; i < 6; i++) {
    CYC(a, a + 1); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(a + 1, a + 3); mem_wr(gb, regs[i], A);
    a += 3;
  }
  CYC(0x0b87, 0x0b89); A = H8(hLcdInterruptBehaviour);
  alu_cp(gb, 0x02);
  CYC(0x0b89, 0x0b8b);
  if (!(F & FZ)) CYCT(0x0b8b, 0x0b8d);
  else {
    CYC(0x0b8b, 0x0b8d);
    alu_xor(gb, A);
    CYC(0x0b8d, 0x0b8e);
    CYC(0x0b8e, 0x0b90); H8(hLcdInterruptBehaviour) = A;
  }
  A = 0x01;
  CYC(0x0b90, 0x0b92);
  CYC(0x0b92, 0x0b94); H8(hLcdInterruptCounter) = A;
  CYCT(0x0b94, 0x0b96);
  _lcdInterruptEnd_hook(gb);
}

void begin_hook(GB *gb) {
  CYC(0x0150, 0x0151);
  CYC(0x0151, 0x0152); gb->ime = false; gb->ime_delay = false; gb->ime_writes++;
  alu_cp(gb, 0x11);
  CYC(0x0152, 0x0154);
  A = 0x00;
  CYC(0x0154, 0x0156);
  if (F & FZ) {
    CYC(0x0156, 0x0158);
    A = alu_inc8(gb, A);
    CYC(0x0158, 0x0159);
    alu_bit(gb, 0, B);
    CYC(0x0159, 0x015b);
    if (F & FZ) CYCT(0x015b, 0x015d);
    else {
      CYC(0x015b, 0x015d);
      A = 0xff;
      CYC(0x015d, 0x015f);
    }
  } else CYCT(0x0156, 0x0158);
  CYC(0x015f, 0x0161); H8(hGameboyType) = A;
  A = 0x37;
  CYC(0x0161, 0x0163);
  CYC(0x0163, 0x0165); H8(hRng1) = A;
  A = 0x0d;
  CYC(0x0165, 0x0167);
  CYC(0x0167, 0x0169); H8(hRng2) = A;
  resetGame_hook(gb);
}

void resetGame__afterSp016c_hook(GB *gb) {
  A = 0x03;
  CYC(0x016c, 0x016e);
  CYC(0x016e, 0x0170); H8(hRomBank) = A;
  CYC(0x0170, 0x0173); mem_wr(gb, MBC_ROM_BANK, A);
  CYCT(0x0173, 0x0176);
  init_hook(gb);
}

void resetGame_hook(GB *gb) {
  CYC(0x0169, 0x016c); gb->sp = wMainStackTop; gb->sp_loads++;
  hook_handoff(gb, 0x016c);
}

void _nextThread__afterSp091a_hook(GB *gb) {
  H = wThreadStateBuffer >> 8;
  CYC(0x091a, 0x091c);
  A = 0x01;
  CYC(0x091c, 0x091e);
  CYC(0x091e, 0x0920); mem_wr(gb, IO_SVBK, A);
  CYCT(0x0920, 0x0922);
  _mainLoop_nextThread_hook(gb);
}

void _nextThread_hook(GB *gb) {
  CYC(0x0917, 0x091a); gb->sp = wMainStackTop; gb->sp_loads++;
  hook_handoff(gb, 0x091a);
}

void startGame__afterSp0925_hook(GB *gb) {
  SET_HL(0x09ac);
  CYC(0x0925, 0x0928);
  SET_DE(wThreadStateBuffer);
  CYC(0x0928, 0x092b);
  B = 0x20;
  CYC(0x092b, 0x092d);
  for (;;) {
    CYC(0x092d, 0x092e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x092e, 0x092f); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(0x092f, 0x0930);
    B = alu_dec8(gb, B);
    CYC(0x0930, 0x0931);
    if (!(F & FZ)) { CYCT(0x0931, 0x0933); continue; }
    CYC(0x0931, 0x0933);
    break;
  }
  _mainLoop_hook(gb);
}

void startGame_hook(GB *gb) {
  CYC(0x0922, 0x0925); gb->sp = wMainStackTop; gb->sp_loads++;
  hook_handoff(gb, 0x0925);
}

void _mainLoop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x0933, pollInput_hook, ROM_pollInput, 0x0936);
  A = mem_rd(gb, hIntroInputsEnabled);
  CYC(0x0936, 0x0938);
  alu_add(gb, A);
  CYC(0x0938, 0x0939);
  if (F & FZ) CYCT(0x0939, 0x093b);
  else {
    CYC(0x0939, 0x093b);
    A = mem_rd(gb, wKeysPressed);
    CYC(0x093b, 0x093e);
    alu_sub(gb, (JOY_A | JOY_B | JOY_START | JOY_SELECT));
    CYC(0x093e, 0x0940);
    if (F & FZ) { CYCT(0x0940, 0x0943); resetGame_hook(gb); return; }
    CYC(0x0940, 0x0943);
  }
  A = 0x10;
  CYC(0x0943, 0x0945);
  CYC(0x0945, 0x0947); H8(hOamTail) = A;
  H = wThreadStateBuffer >> 8;
  CYC(0x0947, 0x0949);
  A = wThreadStateBuffer & 0xff;
  CYC(0x0949, 0x094b);
  CYC(0x094b, 0x094d); H8(hActiveThread) = A;
  L = A;
  CYC(0x094d, 0x094e);
  A = mem_rd(gb, HL);
  CYC(0x094e, 0x094f);
  A = alu_dec8(gb, A);
  CYC(0x094f, 0x0950);
  if (F & FZ) { CYCT(0x0950, 0x0952); _countdownToRunThread_hook(gb); return; }
  CYC(0x0950, 0x0952);
  A = alu_dec8(gb, A);
  CYC(0x0952, 0x0953);
  if (F & FZ) { CYCT(0x0953, 0x0955); _initializeThread_hook(gb); return; }
  CYC(0x0953, 0x0955);
  _mainLoop_nextThread_hook(gb);
}

void _mainLoop_nextThread_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x0955, 0x0957); A = mem_rd(gb, hActiveThread);
    alu_add(gb, 0x08);
    CYC(0x0957, 0x0959);
    CYC(0x0959, 0x095b); H8(hActiveThread) = A;
    alu_cp(gb, (wThreadStateBuffer + 4 * 8) & 0xff);
    CYC(0x095b, 0x095d);
    if (!(F & FZ)) {
      CYCT(0x095d, 0x095f);
      L = A;
      CYC(0x094d, 0x094e);
      A = mem_rd(gb, HL);
      CYC(0x094e, 0x094f);
      A = alu_dec8(gb, A);
      CYC(0x094f, 0x0950);
      if (F & FZ) { CYCT(0x0950, 0x0952); _countdownToRunThread_hook(gb); return; }
      CYC(0x0950, 0x0952);
      A = alu_dec8(gb, A);
      CYC(0x0952, 0x0953);
      if (F & FZ) { CYCT(0x0953, 0x0955); _initializeThread_hook(gb); return; }
      CYC(0x0953, 0x0955);
      continue;
    }
    CYC(0x095d, 0x095f);
    break;
  }
  A = 0x3f;
  CYC(0x095f, 0x0961);
  CYC(0x0961, 0x0963); H8(hRomBank) = A;
  CYC(0x0963, 0x0966); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(0x0966, refreshDirtyPalettes_hook, ROM_refreshDirtyPalettes, 0x0969);
  alu_xor(gb, A);
  CYC(0x0969, 0x096a);
  CYC(0x096a, 0x096c); mem_wr(gb, IO_SVBK, A);
  SET_HL(0xc49e);
  CYC(0x096c, 0x096f);
  CYC(0x096f, 0x0970); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  SET_HL(wGfxRegs1);
  CYC(0x0970, 0x0973);
  SET_DE(wGfxRegsFinal);
  CYC(0x0973, 0x0976);
  B = 0x06;
  CYC(0x0976, 0x0978);
  for (;;) {
    CYC(0x0978, 0x0979); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x0979, 0x097a); mem_wr(gb, DE, A);
    E = alu_inc8(gb, E);
    CYC(0x097a, 0x097b);
    B = alu_dec8(gb, B);
    CYC(0x097b, 0x097c);
    if (!(F & FZ)) { CYCT(0x097c, 0x097e); continue; }
    CYC(0x097c, 0x097e);
    break;
  }
  SET_HL(wVBlankChecker);
  CYC(0x097e, 0x0981);
  A = 0xff;
  CYC(0x0981, 0x0983); mem_wr(gb, HL, A);
  for (;;) {
    int halt_r;
    do { I(0x0983, 1); halt_r = hook_halt(gb, 0x0984); } while (halt_r == 1);
    if (halt_r < 0) { hook_handoff(gb, 0x0984); return; }
    CYC(0x0984, 0x0985);
    alu_bit(gb, 7, mem_rd(gb, HL));
    CYC(0x0985, 0x0987);
    if (!(F & FZ)) { CYCT(0x0987, 0x0989); continue; }
    CYC(0x0987, 0x0989);
    break;
  }
  CYCT(0x0989, 0x098b);
  _mainLoop_hook(gb);
}

void _countdownToRunThread__afterSp0998_hook(GB *gb) {
  CYC(0x0998, 0x0999); SET_BC(pop_effect(gb));
  CYC(0x0999, 0x099a); SET_DE(pop_effect(gb));
  CYC(0x099a, 0x099b); SET_HL(pop_effect(gb));
  CYC(0x099b, 0x099c);
  ret_effect(gb);
}

void _countdownToRunThread_hook(GB *gb) {
  L = alu_inc8(gb, L);
  CYC(0x098b, 0x098c);
  uint8_t v = alu_dec8(gb, mem_rd(gb, HL));
  CYC(0x098c, 0x098d); mem_wr(gb, HL, v);
  if (F & FZ) CYC(0x098d, 0x098f);
  else { CYCT(0x098d, 0x098f); _mainLoop_nextThread_hook(gb); return; }
  L = alu_dec8(gb, L);
  CYC(0x098f, 0x0990);
  A = 0x03;
  CYC(0x0990, 0x0992);
  CYC(0x0992, 0x0993); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(0x0993, 0x0994);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x0994, 0x0995);
  H = mem_rd(gb, HL);
  CYC(0x0995, 0x0996);
  L = A;
  CYC(0x0996, 0x0997);
  gb->sp = HL; gb->sp_loads++;
  CYC(0x0997, 0x0998);
  hook_handoff(gb, 0x0998);
}

void _initializeThread__afterSp09aa_hook(GB *gb) {
  CYC(0x09aa, 0x09ab); push_effect(gb, BC);
  CYC(0x09ab, 0x09ac);
  ret_effect(gb);
}

void _initializeThread_hook(GB *gb) {
  A = 0x03;
  CYC(0x099c, 0x099e);
  CYC(0x099e, 0x099f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  L = alu_inc8(gb, L);
  CYC(0x099f, 0x09a0);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x09a0, 0x09a1);
  E = A;
  CYC(0x09a1, 0x09a2);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x09a2, 0x09a3);
  D = A;
  CYC(0x09a3, 0x09a4);
  A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x09a4, 0x09a5);
  B = mem_rd(gb, HL);
  CYC(0x09a5, 0x09a6);
  C = A;
  CYC(0x09a6, 0x09a7);
  L = E;
  CYC(0x09a7, 0x09a8);
  H = D;
  CYC(0x09a8, 0x09a9);
  gb->sp = HL; gb->sp_loads++;
  CYC(0x09a9, 0x09aa);
  hook_handoff(gb, 0x09aa);
}

void textThreadStart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  A = mem_rd(gb, wScrollMode);
  CYC(0x18a0, 0x18a3);
  alu_or(gb, A);
  CYC(0x18a3, 0x18a4);
  if (F & FZ) CYCT(0x18a4, 0x18a6);
  else {
    CYC(0x18a4, 0x18a6);
    alu_and(gb, 0x01);
    CYC(0x18a6, 0x18a8);
    if (!(F & FZ)) { CYCT(0x18a8, 0x18aa); }
    else {
      CYC(0x18a8, 0x18aa);
      alu_xor(gb, A);
      CYC(0x18aa, 0x18ab);
      CYC(0x18ab, 0x18ae); mem_wr(gb, wTextIsActive, A);
      CYC(0x18ae, 0x18b1); mem_wr(gb, wTextboxFlags, A);
      CYCT(0x18b1, 0x18b4);
      stubThreadStart_hook(gb);
      return;
    }
  }
  A = 0x3f;
  CYC(0x18b4, 0x18b6);
  CYC(0x18b6, 0x18b8); H8(hRomBank) = A;
  CYC(0x18b8, 0x18bb); mem_wr(gb, MBC_ROM_BANK, A);
  CALL_C(0x18bb, initTextbox_hook, ROM_initTextbox, 0x18be);
  for (;;) {
    A = 0x3f;
    CYC(0x18be, 0x18c0);
    CYC(0x18c0, 0x18c2); H8(hRomBank) = A;
    CYC(0x18c2, 0x18c5); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(0x18c5, updateTextbox_hook, ROM_updateTextbox, 0x18c8);
    CALL_C(0x18c8, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, 0x18cb);
    CYCT(0x18cb, 0x18cd);
  }
}

void fileSelectThreadStart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  SET_HL(wFileSelect_mode);
  CYC(0x1a17, 0x1a1a);
  B = 0x10;
  CYC(0x1a1a, 0x1a1c);
  CALL_C(0x1a1c, clearMemory_hook, ROM_clearMemory, 0x1a1f);
  for (;;) {
    A = 0x02;
    CYC(0x1a1f, 0x1a21);
    CYC(0x1a21, 0x1a23); H8(hRomBank) = A;
    CYC(0x1a23, 0x1a26); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(0x1a26, b2_fileSelectScreen_hook, ROM_b2_fileSelectScreen, 0x1a29);
    CALL_C(0x1a29, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, 0x1a2c);
    CYCT(0x1a2c, 0x1a2e);
  }
}

void thread_1b10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  SET_HL(wSaveQuitMenu_state);
  CYC(0x1b10, 0x1b13);
  B = 0x10;
  CYC(0x1b13, 0x1b15);
  CALL_C(0x1b15, clearMemory_hook, ROM_clearMemory, 0x1b18);
  A = 0x01;
  CYC(0x1b18, 0x1b1a);
  CYC(0x1b1a, 0x1b1d); mem_wr(gb, wSaveQuitMenu_gameOver, A);
  for (;;) {
    A = 0x02;
    CYC(0x1b1d, 0x1b1f);
    CYC(0x1b1f, 0x1b21); H8(hRomBank) = A;
    CYC(0x1b21, 0x1b24); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(0x1b24, runSaveAndQuitMenu_hook, ROM_runSaveAndQuitMenu, 0x1b27);
    CALL_C(0x1b27, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, 0x1b2a);
    CYCT(0x1b2a, 0x1b2c);
  }
}

void introThreadStart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    SET_HL(wIntro_frameCounter);
    CYC(0x2d07, 0x2d0a);
    CYC(0x2d0a, 0x2d0b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    A = 0x03;
    CYC(0x2d0b, 0x2d0d);
    CYC(0x2d0d, 0x2d0f); H8(hRomBank) = A;
    CYC(0x2d0f, 0x2d12); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(0x2d12, runIntro_hook, ROM_runIntro, 0x2d15);
    CALL_C(0x2d15, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, 0x2d18);
    CYCT(0x2d18, 0x2d1a);
  }
}

void paletteFadeThreadStart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    A = 0x02;
    CYC(0x3384, 0x3386);
    CYC(0x3386, 0x3388); mem_wr(gb, IO_SVBK, A);
    A = 0x01;
    CYC(0x3388, 0x338a);
    CYC(0x338a, 0x338c); H8(hRomBank) = A;
    CYC(0x338c, 0x338f); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(0x338f, paletteFadeHandler_hook, ROM_paletteFadeHandler, 0x3392);
    CALL_C(0x3392, checkLockBG7Color3ToBlack_hook, ROM_checkLockBG7Color3ToBlack, 0x3395);
    A = mem_rd(gb, wPaletteThread_updateRate);
    CYC(0x3395, 0x3398);
    alu_or(gb, A);
    CYC(0x3398, 0x3399);
    if (!(F & FZ)) CYCT(0x3399, 0x339b);
    else {
      CYC(0x3399, 0x339b);
      A = alu_inc8(gb, A);
      CYC(0x339b, 0x339c);
    }
    CALL_C(0x339c, resumeThreadInAFrames_hook, ROM_resumeThreadInAFrames, 0x339f);
    CYCT(0x339f, 0x33a1);
  }
}

void mainThreadStart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x33a1, restartSound_hook, ROM_restartSound, 0x33a4);
  CALL_C(0x33a4, stopTextThread_hook, ROM_stopTextThread, 0x33a7);
  for (;;) {
    SET_HL(wPlaytimeCounter);
    CYC(0x33a7, 0x33aa);
    CYC(0x33aa, 0x33ab); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x33ab, 0x33ac);
    CYC(0x33ac, 0x33af); mem_wr(gb, wFrameCounter, A);
    if (!(F & FZ)) {
      CYCT(0x33af, 0x33b1);
    } else {
      CYC(0x33af, 0x33b1);
      mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(0x33b1, 0x33b2);
      if (!(F & FZ)) {
        CYCT(0x33b2, 0x33b4);
      } else {
        CYC(0x33b2, 0x33b4);
        L = alu_inc8(gb, L);
        CYC(0x33b4, 0x33b5);
        mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
        CYC(0x33b5, 0x33b6);
        if (!(F & FZ)) {
          CYCT(0x33b6, 0x33b8);
        } else {
          CYC(0x33b6, 0x33b8);
          L = alu_inc8(gb, L);
          CYC(0x33b8, 0x33b9);
          mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
          CYC(0x33b9, 0x33ba);
        }
      }
    }
    A = 0x01;
    CYC(0x33ba, 0x33bc);
    CYC(0x33bc, 0x33be); H8(hRomBank) = A;
    CYC(0x33be, 0x33c1); mem_wr(gb, MBC_ROM_BANK, A);
    CALL_C(0x33c1, runGameLogic_hook, ROM_runGameLogic, 0x33c4);
    CALL_C(0x33c4, drawAllSprites_hook, ROM_drawAllSprites, 0x33c7);
    CALL_C(0x33c7, checkReloadStatusBarGraphics_hook, ROM_checkReloadStatusBarGraphics, 0x33ca);
    CALL_C(0x33ca, resumeThreadNextFrame_hook, ROM_resumeThreadNextFrame, 0x33cd);
    CYCT(0x33cd, 0x33cf);
  }
}

void hramOamDmaFunction_hook(GB *gb) {
  I(0xff80, 2); A = 0xcb;
  I(0xff82, 3); mem_wr(gb, IO_DMA, A);
  I(0xff84, 2); A = 0x28;
  for (;;) {
    I(0xff86, 1); A = alu_dec8(gb, A);
    if (!(F & FZ)) { I(0xff87, 3); continue; }
    I(0xff87, 2);
    break;
  }
  RET(0xff89); return;
}

void wMusicReadFunction_hook(GB *gb) {
  I(0xc000, 3); H8(hSoundDataBaseBank2) = A;
  I(0xc002, 4); mem_wr(gb, 0x2000, A);
  I(0xc005, 2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  I(0xc006, 1); C = A;
  I(0xc007, 3); A = H8(hSoundDataBaseBank);
  I(0xc009, 3); H8(hSoundDataBaseBank2) = A;
  I(0xc00b, 4); mem_wr(gb, 0x2000, A);
  I(0xc00e, 1); A = C;
  RET(0xc00f); return;
}
