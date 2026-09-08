#include "game/asm.h"
#include "game/game.h"

// 00:0068
void addAToDe(GB *gb) {
  I(0x0068, 1); alu_add(gb, E);
  I(0x0069, 1); E = A;
  if (!(F & FC)) { RET_TAKEN(0x006a); return; }
  I(0x006a, 2);
  I(0x006b, 1); D = alu_inc8(gb, D);
  RET(0x006c);
}

// 00:006d
void addAToBc(GB *gb) {
  I(0x006d, 1); alu_add(gb, C);
  I(0x006e, 1); C = A;
  if (!(F & FC)) { RET_TAKEN(0x006f); return; }
  I(0x006f, 2);
  I(0x0070, 1); B = alu_inc8(gb, B);
  RET(0x0071);
}

// 00:0072
void addDoubleIndexToDe(GB *gb) {
  PUSH(0x0072, HL);
  I(0x0073, 1); alu_add(gb, A);
  I(0x0074, 1); L = A;
  I(0x0075, 2); A = 0;
  I(0x0077, 1); alu_adc(gb, A);
  I(0x0078, 1); H = A;
  I(0x0079, 2); alu_add_hl(gb, DE);
  I(0x007a, 1); E = L;
  I(0x007b, 1); D = H;
  SET_HL(POP(0x007c));
  RET(0x007d);
}

// 00:007e
void addDoubleIndexToBc(GB *gb) {
  PUSH(0x007e, HL);
  I(0x007f, 1); alu_add(gb, A);
  I(0x0080, 1); L = A;
  I(0x0081, 2); A = 0;
  I(0x0083, 1); alu_adc(gb, A);
  I(0x0084, 1); H = A;
  I(0x0085, 2); alu_add_hl(gb, BC);
  I(0x0086, 1); C = L;
  I(0x0087, 1); B = H;
  SET_HL(POP(0x0088));
  RET(0x0089);
}

// 00:0176
void getNumSetBits(GB *gb) {
  I(0x0176, 2); B = 0;
  for (;;) {
    I(0x0178, 1); alu_add(gb, A);
    if (F & FC) { I(0x0179, 2); I(0x017b, 1); B = alu_inc8(gb, B); } else I(0x0179, 3);
    I(0x017c, 1); alu_or(gb, A);
    if (A == 0) { I(0x017d, 2); break; }
    I(0x017d, 3);
  }
  I(0x017f, 1); A = B;
  RET(0x0180);
}

// 00:0181
void addDecimalToHlRef(GB *gb) {
  I(0x0181, 2); A = mem_rd(gb, HL);
  I(0x0182, 1); alu_add(gb, C);
  I(0x0183, 1); alu_daa(gb);
  I(0x0184, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  I(0x0185, 2); A = mem_rd(gb, HL);
  I(0x0186, 1); alu_adc(gb, B);
  I(0x0187, 1); alu_daa(gb);
  I(0x0188, 2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  if (!(F & FC)) { RET_TAKEN(0x0189); return; }
  I(0x0189, 2);
  I(0x018a, 2); A = 0x63;
  I(0x018c, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  I(0x018d, 2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  RET(0x018e);
}

// 00:018f
void subDecimalFromHlRef(GB *gb) {
  I(0x018f, 2); A = mem_rd(gb, HL);
  I(0x0190, 1); alu_sub(gb, C);
  I(0x0191, 1); alu_daa(gb);
  I(0x0192, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  I(0x0193, 2); A = mem_rd(gb, HL);
  I(0x0194, 1); alu_sbc(gb, B);
  I(0x0195, 1); alu_daa(gb);
  I(0x0196, 2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  if (!(F & FC)) { RET_TAKEN(0x0197); return; }
  I(0x0197, 2);
  I(0x0198, 1); alu_xor(gb, A);
  I(0x0199, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  I(0x019a, 2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  I(0x019b, 1); alu_scf(gb);
  RET(0x019c);
}

// 00:019d
void multiplyAByC(GB *gb) {
  I(0x019d, 2); E = 8;
  I(0x019f, 2); B = 0;
  I(0x01a1, 1); L = B;
  I(0x01a2, 1); H = B;
  for (;;) {
    I(0x01a3, 2); alu_add_hl(gb, HL);
    I(0x01a4, 1); alu_add(gb, A);
    if (F & FC) { I(0x01a5, 2); I(0x01a7, 2); alu_add_hl(gb, BC); } else I(0x01a5, 3);
    I(0x01a8, 1); E = alu_dec8(gb, E);
    if (E == 0) { I(0x01a9, 2); break; }
    I(0x01a9, 3);
  }
  RET(0x01ab);
}

// 00:01ac
void multiplyABy16(GB *gb) {
  I(0x01ac, 2); alu_swap_a(gb);
  I(0x01ae, 1); B = A;
  I(0x01af, 2); alu_and(gb, 0xf0);
  I(0x01b1, 1); C = A;
  I(0x01b2, 1); A = B;
  I(0x01b3, 2); alu_and(gb, 0x0f);
  I(0x01b5, 1); B = A;
  RET(0x01b6);
}

// 00:01b7
void multiplyABy8(GB *gb) {
  I(0x01b7, 2); alu_swap_a(gb);
  I(0x01b9, 1); alu_rrca(gb);
  I(0x01ba, 1); B = A;
  I(0x01bb, 2); alu_and(gb, 0xf8);
  I(0x01bd, 1); C = A;
  I(0x01be, 1); A = B;
  I(0x01bf, 2); alu_and(gb, 0x07);
  I(0x01c1, 1); B = A;
  RET(0x01c2);
}

// 00:01c3
void multiplyABy4(GB *gb) {
  I(0x01c3, 2); B = 0;
  I(0x01c5, 1); alu_add(gb, A);
  I(0x01c6, 2); B = alu_rl(gb, B);
  I(0x01c8, 1); alu_add(gb, A);
  I(0x01c9, 2); B = alu_rl(gb, B);
  I(0x01cb, 1); C = A;
  RET(0x01cc);
}

// 00:01cd
void s8ToS16(GB *gb) {
  I(0x01cd, 2); B = 0xff;
  I(0x01cf, 2); alu_bit(gb, 7, A);
  if (F & FZ) { I(0x01d1, 2); I(0x01d3, 1); B = alu_inc8(gb, B); } else I(0x01d1, 3);
  I(0x01d4, 1); C = A;
  RET(0x01d5);
}

// 00:01d6
void compareHlToBc(GB *gb) {
  I(0x01d6, 1); A = H;
  I(0x01d7, 1); alu_cp(gb, B);
  if (F & FC) { I(0x01d8, 3); goto less; }
  I(0x01d8, 2);
  if (!(F & FZ)) { I(0x01da, 3); goto greater; }
  I(0x01da, 2);
  I(0x01dc, 1); A = L;
  I(0x01dd, 1); alu_cp(gb, C);
  if (F & FC) { I(0x01de, 3); goto less; }
  I(0x01de, 2);
  if (!(F & FZ)) { I(0x01e0, 3); goto greater; }
  I(0x01e0, 2);
  I(0x01e2, 1); alu_xor(gb, A);
  RET(0x01e3); return;
less:
  I(0x01e4, 2); A = 0xff;
  RET(0x01e6); return;
greater:
  I(0x01e7, 2); A = 0x01;
  RET(0x01e9);
}

// 00:01ea
void getHighestSetBit(GB *gb) {
  I(0x01ea, 1); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x01eb); return; }
  I(0x01eb, 2);
  PUSH(0x01ec, BC);
  I(0x01ed, 2); C = 0xff;
  for (;;) {
    I(0x01ef, 1); C = alu_inc8(gb, C);
    I(0x01f0, 2); A = alu_srl(gb, A);
    if (A == 0) { I(0x01f2, 2); break; }
    I(0x01f2, 3);
  }
  I(0x01f4, 1); A = C;
  SET_BC(POP(0x01f5));
  I(0x01f6, 1); alu_scf(gb);
  RET(0x01f7);
}

// 00:01f8
void getLowestSetBit(GB *gb) {
  I(0x01f8, 1); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x01f9); return; }
  I(0x01f9, 2);
  PUSH(0x01fa, BC);
  I(0x01fb, 2); C = 0x08;
  for (;;) {
    I(0x01fd, 1); C = alu_dec8(gb, C);
    I(0x01fe, 1); alu_add(gb, A);
    if (A == 0) { I(0x01ff, 2); break; }
    I(0x01ff, 3);
  }
  I(0x0201, 1); A = C;
  SET_BC(POP(0x0202));
  I(0x0203, 1); alu_scf(gb);
  RET(0x0204);
}

#define BIT_TABLE 0x00f8

// 00:0223 (_flagHlpr)
static void flagHlpr(GB *gb) {
  I(0x0223, 1); B = A;
  I(0x0224, 2); alu_and(gb, 0xf8);
  I(0x0226, 1); alu_rlca(gb);
  I(0x0227, 2); alu_swap_a(gb);
  I(0x0229, 1); C = A;
  I(0x022a, 1); A = B;
  I(0x022b, 2); B = 0;
  I(0x022d, 2); alu_add_hl(gb, BC);
  I(0x022e, 2); alu_and(gb, 0x07);
  I(0x0230, 3); SET_BC(BIT_TABLE);
  I(0x0233, 1); alu_add(gb, C);
  I(0x0234, 1); C = A;
  I(0x0235, 2); A = mem_rd(gb, BC);
  RET(0x0236);
}

// 00:0205
void checkFlag(GB *gb) {
  PUSH(0x0205, HL);
  PUSH(0x0206, BC);
  CALL(0x0207, flagHlpr, 0x020a);
  I(0x020a, 2); alu_and(gb, mem_rd(gb, HL));
  SET_BC(POP(0x020b));
  SET_HL(POP(0x020c));
  RET(0x020d);
}

// 00:020e
void setFlag(GB *gb) {
  PUSH(0x020e, HL);
  PUSH(0x020f, BC);
  CALL(0x0210, flagHlpr, 0x0213);
  I(0x0213, 2); alu_or(gb, mem_rd(gb, HL));
  I(0x0214, 2); mem_wr(gb, HL, A);
  SET_BC(POP(0x0215));
  SET_HL(POP(0x0216));
  RET(0x0217);
}

// 00:0218
void unsetFlag(GB *gb) {
  PUSH(0x0218, HL);
  PUSH(0x0219, BC);
  CALL(0x021a, flagHlpr, 0x021d);
  I(0x021d, 1); alu_cpl(gb);
  I(0x021e, 2); alu_and(gb, mem_rd(gb, HL));
  I(0x021f, 2); mem_wr(gb, HL, A);
  SET_BC(POP(0x0220));
  SET_HL(POP(0x0221));
  RET(0x0222);
}

// 00:0237
void decHlRef16WithCap(GB *gb) {
  I(0x0237, 2); SET_HL(HL + 1);
  I(0x0238, 2); A = mem_rd(gb, HL); SET_HL(HL - 1);
  I(0x0239, 2); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x023a); return; }
  I(0x023a, 2);
  I(0x023b, 2); A = mem_rd(gb, HL);
  I(0x023c, 2); alu_sub(gb, 1);
  I(0x023e, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  I(0x023f, 2); A = mem_rd(gb, HL);
  I(0x0240, 2); alu_sbc(gb, 0);
  I(0x0242, 2); mem_wr(gb, HL, A); SET_HL(HL - 1);
  I(0x0243, 2); alu_or(gb, mem_rd(gb, HL));
  RET(0x0244);
}

// 00:0245
void incHlRefWithCap(GB *gb) {
  I(0x0245, 3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x0246); return; }
  I(0x0246, 2);
  I(0x0247, 3); mem_wr(gb, HL, 0xff);
  RET(0x0249);
}

// 00:024a
void incHlRef16WithCap(GB *gb) {
  I(0x024a, 3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x024b); return; }
  I(0x024b, 2);
  I(0x024c, 2); SET_HL(HL + 1);
  I(0x024d, 3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (F & FZ) {
    I(0x024e, 3);
    PUSH(0x0252, AF);
    I(0x0253, 2); A = 0xff;
    I(0x0255, 2); mem_wr(gb, HL, A); SET_HL(HL - 1);
    I(0x0256, 2); mem_wr(gb, HL, A);
    SET_AF(POP(0x0257));
    RET(0x0258);
    return;
  }
  I(0x024e, 2);
  I(0x0250, 2); SET_HL(HL - 1);
  RET(0x0251);
}

// 00:0259
void hexToDec(GB *gb) {
  I(0x0259, 3); SET_BC(0);
  for (;;) {
    I(0x025c, 2); alu_cp(gb, 100);
    if (F & FC) { I(0x025e, 3); break; }
    I(0x025e, 2);
    I(0x0260, 2); alu_sub(gb, 100);
    I(0x0262, 1); B = alu_inc8(gb, B);
    I(0x0263, 3);
  }
  for (;;) {
    I(0x0265, 2); alu_cp(gb, 10);
    if (F & FC) { RET_TAKEN(0x0267); return; }
    I(0x0267, 2);
    I(0x0268, 2); alu_sub(gb, 10);
    I(0x026a, 1); C = alu_inc8(gb, C);
    I(0x026b, 3);
  }
}

#define H_RNG1 0x94
#define H_RNG2 0x95

// 00:043e
void getRandomNumber(GB *gb) {
  PUSH(0x043e, HL);
  PUSH(0x043f, BC);
  I(0x0440, 3); A = hram_rd(gb, H_RNG1);
  I(0x0442, 1); L = A;
  I(0x0443, 1); C = A;
  I(0x0444, 3); A = hram_rd(gb, H_RNG2);
  I(0x0446, 1); H = A;
  I(0x0447, 1); B = A;
  I(0x0448, 2); alu_add_hl(gb, HL);
  I(0x0449, 2); alu_add_hl(gb, BC);
  I(0x044a, 1); A = H;
  I(0x044b, 3); hram_wr(gb, H_RNG2, A);
  I(0x044d, 1); alu_add(gb, C);
  I(0x044e, 3); hram_wr(gb, H_RNG1, A);
  SET_BC(POP(0x0450));
  SET_HL(POP(0x0451));
  RET(0x0452);
}

// 00:0453
void getRandomNumber_noPreserveVars(GB *gb) {
  I(0x0453, 3); A = hram_rd(gb, H_RNG1);
  I(0x0455, 1); L = A;
  I(0x0456, 1); C = A;
  I(0x0457, 3); A = hram_rd(gb, H_RNG2);
  I(0x0459, 1); H = A;
  I(0x045a, 1); B = A;
  I(0x045b, 2); alu_add_hl(gb, HL);
  I(0x045c, 2); alu_add_hl(gb, BC);
  I(0x045d, 1); A = H;
  I(0x045e, 3); hram_wr(gb, H_RNG2, A);
  I(0x0460, 1); alu_add(gb, C);
  I(0x0461, 3); hram_wr(gb, H_RNG1, A);
  RET(0x0463);
}

// 00:0464
void getRandomIndexFromProbabilityDistribution(GB *gb) {
  I(0x0464, 2); B = 0;
  CALL(0x0466, getRandomNumber, 0x0469);
  for (;;) {
    I(0x0469, 2); alu_sub(gb, mem_rd(gb, HL));
    if (F & FC) { RET_TAKEN(0x046a); return; }
    I(0x046a, 2);
    I(0x046b, 2); SET_HL(HL + 1);
    I(0x046c, 1); B = alu_inc8(gb, B);
    I(0x046d, 3);
  }
}

// 00:046f
void clearMemory(GB *gb) {
  I(0x046f, 1); alu_xor(gb, A);
  fillMemory(gb);
}

// 00:0470
void fillMemory(GB *gb) {
  for (;;) {
    I(0x0470, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    I(0x0471, 1); B = alu_dec8(gb, B);
    if (B == 0) { I(0x0472, 2); break; }
    I(0x0472, 3);
  }
  RET(0x0474);
}

// 00:0475
void clearMemoryBc(GB *gb) {
  I(0x0475, 1); alu_xor(gb, A);
  fillMemoryBc(gb);
}

// 00:0476
void fillMemoryBc(GB *gb) {
  I(0x0476, 1); E = A;
  for (;;) {
    I(0x0477, 1); A = E;
    I(0x0478, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    I(0x0479, 2); SET_BC(BC - 1);
    I(0x047a, 1); A = C;
    I(0x047b, 1); alu_or(gb, B);
    if (A == 0) { I(0x047c, 2); break; }
    I(0x047c, 3);
  }
  RET(0x047e);
}

// 00:047f
void copyMemoryReverse(GB *gb) {
  for (;;) {
    I(0x047f, 2); A = mem_rd(gb, DE);
    I(0x0480, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    I(0x0481, 2); SET_DE(DE + 1);
    I(0x0482, 1); B = alu_dec8(gb, B);
    if (B == 0) { I(0x0483, 2); break; }
    I(0x0483, 3);
  }
  RET(0x0485);
}

// 00:0486
void copyMemory(GB *gb) {
  for (;;) {
    I(0x0486, 2); A = mem_rd(gb, HL); SET_HL(HL + 1);
    I(0x0487, 2); mem_wr(gb, DE, A);
    I(0x0488, 2); SET_DE(DE + 1);
    I(0x0489, 1); B = alu_dec8(gb, B);
    if (B == 0) { I(0x048a, 2); break; }
    I(0x048a, 3);
  }
  RET(0x048c);
}

// 00:048d
void copyMemoryBcReverse(GB *gb) {
  for (;;) {
    I(0x048d, 2); A = mem_rd(gb, DE);
    I(0x048e, 2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    I(0x048f, 2); SET_DE(DE + 1);
    I(0x0490, 2); SET_BC(BC - 1);
    I(0x0491, 1); A = B;
    I(0x0492, 1); alu_or(gb, C);
    if (A == 0) { I(0x0493, 2); break; }
    I(0x0493, 3);
  }
  RET(0x0495);
}

// 00:0496
void copyMemoryBc(GB *gb) {
  for (;;) {
    I(0x0496, 2); A = mem_rd(gb, HL); SET_HL(HL + 1);
    I(0x0497, 2); mem_wr(gb, DE, A);
    I(0x0498, 2); SET_DE(DE + 1);
    I(0x0499, 2); SET_BC(BC - 1);
    I(0x049a, 1); A = B;
    I(0x049b, 1); alu_or(gb, C);
    if (A == 0) { I(0x049c, 2); break; }
    I(0x049c, 3);
  }
  RET(0x049e);
}
