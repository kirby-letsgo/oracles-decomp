#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

void agesFunc_10_70f6__func_71fd_hook(GB *gb);
void agesFunc_10_7298__playWaveSoundAtRandomIntervals_body_hook(GB *gb);

void agesFunc_10_70f6__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x71b8, decCbb3_hook, 0x305d, 0x71bb);
  if (!(F & FZ)) {
    CYCT(0x71bb, 0x71bc); ret_effect(gb);
    return;
  }
  CYC(0x71bb, 0x71bc);
  CYC(0x71bc, 0x71be); A = 0xff;
  CYC(0x71be, 0x71c1); mem_wr(gb, wTmpcbba, A);
  CYC(0x71c1, 0x71c4); incCbc2_hook(gb);
}

void agesFunc_10_70f6__func_71aa_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x71aa, decCbb3_hook, 0x305d, 0x71ad);
  if (!(F & FZ)) {
    CYCT(0x71ad, 0x71ae); ret_effect(gb);
    return;
  }
  CYC(0x71ad, 0x71ae);
  CYC(0x71ae, 0x71b0); mem_wr(gb, HL, 0x04);
  CYC(0x71b0, 0x71b3); SET_HL(wGfxRegs1_SCY);
  CYC(0x71b3, 0x71b4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x71b4, 0x71b5); A = mem_rd(gb, HL);
  CYC(0x71b5, 0x71b7); hram_wr(gb, 0xaa, A);
  CYC(0x71b7, 0x71b8); ret_effect(gb);
}

void agesFunc_10_70f6__func_71fd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x71fd, 0x71fe); alu_xor(gb, A);
  CYC(0x71fe, 0x7200); hram_wr(gb, 0x9f, A);
  CYC(0x7200, 0x7203); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x7203, 0x7205); alu_cp(gb, 0x60);
  if (!(F & FC)) {
    CYCT(0x7205, 0x7207);
  } else {
    CYC(0x7205, 0x7207);
    CYC(0x7207, 0x7208); alu_cpl(gb);
    CYC(0x7208, 0x7209); A = alu_inc8(gb, A);
    CYC(0x7209, 0x720a); B = A;
    CYC(0x720a, 0x720d); A = mem_rd(gb, wFrameCounter);
    CYC(0x720d, 0x720f); alu_and(gb, 0x01);
    if (!(F & FZ)) {
      CYCT(0x720f, 0x7211);
    } else {
      CYC(0x720f, 0x7211);
      CYC(0x7211, 0x7212); C = A;
      CYC(0x7212, 0x7215); SET_HL(0x4ed8);
      CYC(0x7215, 0x7217); E = 0x16;
      CALL_C(0x7217, addSpritesFromBankToOam_withOffset_hook, 0x30d8, 0x721a);
    }
  }
  CYC(0x721a, 0x721d); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x721d, 0x721e); alu_cpl(gb);
  CYC(0x721e, 0x721f); A = alu_inc8(gb, A);
  CYC(0x721f, 0x7221); B = 0xc7;
  CYC(0x7221, 0x7222); alu_add(gb, B);
  CYC(0x7222, 0x7223); B = A;
  CYC(0x7223, 0x7225); C = 0x38;
  CYC(0x7225, 0x7228); SET_HL(0x4f21);
  CYC(0x7228, 0x722a); E = 0x16;
  CYC(0x722a, 0x722b); push_effect(gb, BC);
  CALL_C(0x722b, addSpritesFromBankToOam_withOffset_hook, 0x30d8, 0x722e);
  CYC(0x722e, 0x722f); SET_BC(pop_effect(gb));
  CYC(0x722f, 0x7232); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x7232, 0x7234); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(0x7234, 0x7235); ret_effect(gb);
    return;
  }
  CYC(0x7234, 0x7235);
  CYC(0x7235, 0x7238); SET_HL(0x4f56);
  CYC(0x7238, 0x723a); E = 0x16;
  CYC(0x723a, 0x723d); addSpritesFromBankToOam_withOffset_hook(gb);
}

void agesFunc_10_70f6__substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x723d, agesFunc_10_70f6__func_71fd_hook, 0x71fd, 0x7240);
  CYC(0x7240, 0x7243); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7243, 0x7244); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7244, 0x7245); ret_effect(gb);
    return;
  }
  CYC(0x7244, 0x7245);
  CALL_C(0x7245, decCbb3_hook, 0x305d, 0x7248);
  if (!(F & FZ)) {
    CYCT(0x7248, 0x7249); ret_effect(gb);
    return;
  }
  CYC(0x7248, 0x7249);
  CYC(0x7249, 0x724b); A = 0x04;
  CYC(0x724b, 0x724e); mem_wr(gb, wTmpcbb3, A);
  CYC(0x724e, 0x7251); incCbc2_hook(gb);
}

void agesFunc_10_70f6__func_7262_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7262, decCbb3_hook, 0x305d, 0x7265);
  if (!(F & FZ)) {
    CYCT(0x7265, 0x7267);
  } else {
    CYC(0x7265, 0x7267);
    CYC(0x7267, 0x7269); mem_wr(gb, HL, 0x04);
    CYC(0x7269, 0x726c); SET_HL(wGfxRegs1_SCY);
    CYC(0x726c, 0x726d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x726d, 0x726e); A = mem_rd(gb, HL);
    CYC(0x726e, 0x7270); hram_wr(gb, 0xaa, A);
    CYC(0x7270, 0x7272); alu_cp(gb, 0x60);
    if (!(F & FZ)) {
      CYCT(0x7272, 0x7274);
    } else {
      CYC(0x7272, 0x7274);
      CALL_C(0x7274, clearDynamicInteractions_hook, 0x35d2, 0x7277);
      CYC(0x7277, 0x7279); A = 0x2c;
      CALL_C(0x7279, loadUncompressedGfxHeader_hook, 0x05da, 0x727c);
    }
  }
  CYC(0x727c, 0x727f); agesFunc_10_70f6__func_71fd_hook(gb);
}

void agesFunc_10_7298__func_731b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x731b, checkIsLinkedGame_hook, 0x1992, 0x731e);
  if (F & FZ) {
    CYCT(0x731e, 0x731f); ret_effect(gb);
    return;
  }
  CYC(0x731e, 0x731f);
  CYC(0x731f, 0x7322); SET_HL(wTmpcbb4);
  CYC(0x7322, 0x7323); A = mem_rd(gb, HL);
  CYC(0x7323, 0x7324); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7324, 0x7326);
    agesFunc_10_7298__playWaveSoundAtRandomIntervals_body_hook(gb);
    return;
  }
  CYC(0x7324, 0x7326);
  CYC(0x7326, 0x7327); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7327, 0x7328); ret_effect(gb);
}

void agesFunc_10_7298__playWaveSoundAtRandomIntervals_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7328, 0x7329); push_effect(gb, HL);
  CYC(0x7329, 0x732b); A = 0xaa;
  CALL_C(0x732b, playSound_b00_hook, 0x0c98, 0x732e);
  CYC(0x732e, 0x732f); SET_HL(pop_effect(gb));
  CALL_C(0x732f, getRandomNumber_hook, 0x043e, 0x7332);
  CYC(0x7332, 0x7334); alu_and(gb, 0x03);
  CYC(0x7334, 0x7337); SET_BC(0x733d);
  CALL_C(0x7337, addAToBc_hook, 0x006d, 0x733a);
  CYC(0x733a, 0x733b); A = mem_rd(gb, BC);
  CYC(0x733b, 0x733c); mem_wr(gb, HL, A);
  CYC(0x733c, 0x733d); ret_effect(gb);
}

void agesFunc_10_7298__func_7450_hook(GB *gb) {
  CYC(0x7450, 0x7453); SET_HL(0x4fec);
  CYC(0x7453, 0x7455); E = 0x16;
  CYC(0x7455, 0x7458); SET_BC(0x3038);
  CYC(0x7458, 0x7459); alu_xor(gb, A);
  CYC(0x7459, 0x745b); hram_wr(gb, 0x9f, A);
  CYC(0x745b, 0x745e); addSpritesFromBankToOam_withOffset_hook(gb);
}

void agesFunc_10_7298__func_746a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x746a, 0x746d); A = mem_rd(gb, wKeysJustPressed);
  CYC(0x746d, 0x746f); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x746f, 0x7470); ret_effect(gb);
    return;
  }
  CYC(0x746f, 0x7470);
  CALL_C(0x7470, incCbc2_hook, 0x3067, 0x7473);
  CYC(0x7473, 0x7476); fadeoutToWhite_hook(gb);
}
