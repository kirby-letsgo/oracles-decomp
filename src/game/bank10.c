#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

void agesFunc_10_70f6__func_71fd_hook(GB *gb);
void agesFunc_10_70f6__func_7174_hook(GB *gb);
void agesFunc_10_7298__substate3_hook(GB *gb);
void agesFunc_10_7298__func_7407_hook(GB *gb);
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

void agesFunc_10_70f6__substate1_hook(GB *gb) {
  CYC(0x7146, 0x7149); A = mem_rd(gb, 0xcfdf);
  CYC(0x7149, 0x714a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x714a, 0x714b); ret_effect(gb);
    return;
  }
  CYC(0x714a, 0x714b);
  CYC(0x714b, 0x714e); SET_HL(wTmpcbb3);
  CYC(0x714e, 0x7150); mem_wr(gb, HL, 0xe0);
  CYC(0x7150, 0x7151); SET_HL(HL + 1);
  CYC(0x7151, 0x7153); mem_wr(gb, HL, 0x01);
  CYC(0x7153, 0x7156); incCbc2_hook(gb);
}

void agesFunc_10_70f6__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7156, 0x7159); SET_HL(wTmpcbb3);
  CALL_C(0x7159, decHlRef16WithCap_hook, 0x0237, 0x715c);
  if (!(F & FZ)) {
    CYCT(0x715c, 0x715d); ret_effect(gb);
    return;
  }
  CYC(0x715c, 0x715d);
  CALL_C(0x715d, checkIsLinkedGame_hook, 0x1992, 0x7160);
  if (!(F & FZ)) {
    CYCT(0x7160, 0x7162);
    agesFunc_10_70f6__func_7174_hook(gb);
    return;
  }
  CYC(0x7160, 0x7162);
  CYC(0x7162, 0x7165); SET_HL(0x6086);
  CYC(0x7165, 0x7167); E = 0x03;
  CALL_C(0x7167, interBankCall_hook, 0x008a, 0x716a);
  CYC(0x716a, 0x716c); A = 0x03;
  CYC(0x716c, 0x716f); mem_wr(gb, 0xcbc1, A);
  CYC(0x716f, 0x7171); A = 0x04;
  CYC(0x7171, 0x7174); fadeoutToWhiteWithDelay_hook(gb);
}

void agesFunc_10_70f6__func_7174_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7174, 0x7176); A = 0x04;
  CYC(0x7176, 0x7179); mem_wr(gb, wTmpcbb3, A);
  CYC(0x7179, 0x717c); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x717c, 0x717e); hram_wr(gb, 0xaa, A);
  CYC(0x717e, 0x7180); A = 0x01;
  CALL_C(0x7180, loadUncompressedGfxHeader_hook, 0x05da, 0x7183);
  CYC(0x7183, 0x7185); A = 0x0b;
  CALL_C(0x7185, loadPaletteHeader_hook, 0x050b, 0x7188);
  CYC(0x7188, 0x718a); B = 0x03;
  for (;;) {
    CALL_C(0x718a, getFreeInteractionSlot_hook, 0x3aef, 0x718d);
    if (!(F & FZ)) {
      CYCT(0x718d, 0x718f);
      break;
    }
    CYC(0x718d, 0x718f);
    CYC(0x718f, 0x7191); mem_wr(gb, HL, 0x4a);
    CYC(0x7191, 0x7192); L = alu_inc8(gb, L);
    CYC(0x7192, 0x7194); mem_wr(gb, HL, 0x09);
    CYC(0x7194, 0x7195); L = alu_inc8(gb, L);
    CYC(0x7195, 0x7196); B = alu_dec8(gb, B);
    CYC(0x7196, 0x7197); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(0x7197, 0x7199);
      continue;
    }
    CYC(0x7197, 0x7199);
    break;
  }
  CYC(0x7199, 0x719c); incCbc2_hook(gb);
}

void agesFunc_10_70f6__substate3_hook(GB *gb) {
  CYC(0x719c, 0x719f); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x719f, 0x71a0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x71a0, 0x71a2);
    agesFunc_10_70f6__func_71aa_hook(gb);
    return;
  }
  CYC(0x71a0, 0x71a2);
  CYC(0x71a2, 0x71a4); A = 0x78;
  CYC(0x71a4, 0x71a7); mem_wr(gb, wTmpcbb3, A);
  CYC(0x71a7, 0x71aa); incCbc2_hook(gb);
}

void agesFunc_10_70f6__substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x71c4, 0x71c7); SET_HL(wTmpcbb3);
  CYC(0x71c7, 0x71c9); B = 0x01;
  CALL_C(0x71c9, flashScreen_hook, 0x2d73, 0x71cc);
  if (F & FZ) {
    CYCT(0x71cc, 0x71cd); ret_effect(gb);
    return;
  }
  CYC(0x71cc, 0x71cd);
  CALL_C(0x71cd, disableLcd_hook, 0x02c1, 0x71d0);
  CYC(0x71d0, 0x71d2); A = 0x9a;
  CALL_C(0x71d2, loadGfxHeader_hook, 0x0626, 0x71d5);
  CYC(0x71d5, 0x71d7); A = 0x9f;
  CALL_C(0x71d7, loadPaletteHeader_hook, 0x050b, 0x71da);
  CALL_C(0x71da, clearDynamicInteractions_hook, 0x35d2, 0x71dd);
  CYC(0x71dd, 0x71df); B = 0x03;
  for (;;) {
    CALL_C(0x71df, getFreeInteractionSlot_hook, 0x3aef, 0x71e2);
    if (!(F & FZ)) {
      CYCT(0x71e2, 0x71e4);
      break;
    }
    CYC(0x71e2, 0x71e4);
    CYC(0x71e4, 0x71e6); mem_wr(gb, HL, 0xcf);
    CYC(0x71e6, 0x71e7); L = alu_inc8(gb, L);
    CYC(0x71e7, 0x71e8); B = alu_dec8(gb, B);
    CYC(0x71e8, 0x71e9); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(0x71e9, 0x71eb);
      continue;
    }
    CYC(0x71e9, 0x71eb);
    break;
  }
  CYC(0x71eb, 0x71ed); A = 0x04;
  CALL_C(0x71ed, loadGfxRegisterStateIndex_hook, 0x02ea, 0x71f0);
  CYC(0x71f0, 0x71f2); A = 0x04;
  CALL_C(0x71f2, fadeinFromWhiteWithDelay_hook, 0x3284, 0x71f5);
  CALL_C(0x71f5, incCbc2_hook, 0x3067, 0x71f8);
  CYC(0x71f8, 0x71fa); A = 0xf0;
  CYC(0x71fa, 0x71fd); mem_wr(gb, wTmpcbb3, A);
  agesFunc_10_70f6__func_71fd_hook(gb);
}

void agesFunc_10_70f6__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7251, 0x7254); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(0x7254, 0x7256); alu_cp(gb, 0x98);
  if (!(F & FZ)) {
    CYCT(0x7256, 0x7258);
    agesFunc_10_70f6__func_7262_hook(gb);
    return;
  }
  CYC(0x7256, 0x7258);
  CYC(0x7258, 0x725a); A = 0xf0;
  CYC(0x725a, 0x725d); mem_wr(gb, wTmpcbb3, A);
  CALL_C(0x725d, incCbc2_hook, 0x3067, 0x7260);
  CYC(0x7260, 0x7262);
  CYC(0x727c, 0x727f);
  agesFunc_10_70f6__func_71fd_hook(gb);
}

void agesFunc_10_70f6__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x727f, agesFunc_10_70f6__func_71fd_hook, 0x71fd, 0x7282);
  CALL_C(0x7282, decCbb3_hook, 0x305d, 0x7285);
  if (!(F & FZ)) {
    CYCT(0x7285, 0x7286); ret_effect(gb);
    return;
  }
  CYC(0x7285, 0x7286);
  CYC(0x7286, 0x7289); SET_HL(0x6086);
  CYC(0x7289, 0x728b); E = 0x03;
  CALL_C(0x728b, interBankCall_hook, 0x008a, 0x728e);
  CYC(0x728e, 0x7290); A = 0x03;
  CYC(0x7290, 0x7293); mem_wr(gb, 0xcbc1, A);
  CYC(0x7293, 0x7295); A = 0x04;
  CYC(0x7295, 0x7298); fadeoutToWhiteWithDelay_hook(gb);
}

void agesFunc_10_7298__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7313, 0x7316); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7316, 0x7317); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7317, 0x7318); ret_effect(gb);
    return;
  }
  CYC(0x7317, 0x7318);
  CALL_C(0x7318, incCbc2_hook, 0x3067, 0x731b);
  agesFunc_10_7298__func_731b_hook(gb);
}

void agesFunc_10_7298__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7341, agesFunc_10_7298__func_731b_hook, 0x731b, 0x7344);
  CALL_C(0x7344, decCbb3_hook, 0x305d, 0x7347);
  if (!(F & FZ)) {
    CYCT(0x7347, 0x7348); ret_effect(gb);
    return;
  }
  CYC(0x7347, 0x7348);
  CALL_C(0x7348, incCbc2_hook, 0x3067, 0x734b);
  agesFunc_10_7298__substate3_hook(gb);
}

void agesFunc_10_7298__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x734b, agesFunc_10_7298__func_731b_hook, 0x731b, 0x734e);
  CYC(0x734e, 0x7351); SET_HL(wFileIsLinkedGame);
  CYC(0x7351, 0x7352); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7352, 0x7353); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7353, 0x7355); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x7355, 0x7356); ret_effect(gb);
    return;
  }
  CYC(0x7355, 0x7356);
  CYC(0x7356, 0x7359); A = mem_rd(gb, wKeysJustPressed);
  CYC(0x7359, 0x735b); alu_and(gb, 0x0b);
  if (F & FZ) {
    CYCT(0x735b, 0x735c); ret_effect(gb);
    return;
  }
  CYC(0x735b, 0x735c);
  CALL_C(0x735c, incCbc2_hook, 0x3067, 0x735f);
  CYC(0x735f, 0x7362); fadeoutToWhite_hook(gb);
}

void agesFunc_10_7298__substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x73c1, fileSelect_redrawDecorations_hook, 0x3099, 0x73c4);
  CYC(0x73c4, 0x73c7); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x73c7, 0x73c8); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x73c8, 0x73c9); ret_effect(gb);
    return;
  }
  CYC(0x73c8, 0x73c9);
  CALL_C(0x73c9, decCbb3_hook, 0x305d, 0x73cc);
  if (!(F & FZ)) {
    CYCT(0x73cc, 0x73cd); ret_effect(gb);
    return;
  }
  CYC(0x73cc, 0x73cd);
  CYC(0x73cd, 0x73d0); SET_HL(wTmpcbb3);
  CYC(0x73d0, 0x73d2); B = 0x3c;
  CALL_C(0x73d2, checkIsLinkedGame_hook, 0x1992, 0x73d5);
  if (F & FZ) {
    CYCT(0x73d5, 0x73d7);
  } else {
    CYC(0x73d5, 0x73d7);
    CYC(0x73d7, 0x73d9); B = 0xb4;
  }
  CYC(0x73d9, 0x73da); mem_wr(gb, HL, B);
  CYC(0x73da, 0x73dd); incCbc2_hook(gb);
}

void agesFunc_10_7298__substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x73dd, fileSelect_redrawDecorations_hook, 0x3099, 0x73e0);
  CALL_C(0x73e0, decCbb3_hook, 0x305d, 0x73e3);
  if (!(F & FZ)) {
    CYCT(0x73e3, 0x73e4); ret_effect(gb);
    return;
  }
  CYC(0x73e3, 0x73e4);
  CALL_C(0x73e4, checkIsLinkedGame_hook, 0x1992, 0x73e7);
  if (!(F & FZ)) {
    CYCT(0x73e7, 0x73e9);
  } else {
    CYC(0x73e7, 0x73e9);
    CALL_C(0x73e9, getFreeInteractionSlot_hook, 0x3aef, 0x73ec);
    if (!(F & FZ)) {
      CYCT(0x73ec, 0x73ee);
    } else {
      CYC(0x73ec, 0x73ee);
      CYC(0x73ee, 0x73f0); mem_wr(gb, HL, 0xd1);
      CYC(0x73f0, 0x73f1); alu_xor(gb, A);
      CYC(0x73f1, 0x73f4); mem_wr(gb, 0xcfde, A);
    }
  }
  CYC(0x73f4, 0x73f7); incCbc2_hook(gb);
}

void agesFunc_10_7298__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x73f7, fileSelect_redrawDecorations_hook, 0x3099, 0x73fa);
  CALL_C(0x73fa, checkIsLinkedGame_hook, 0x1992, 0x73fd);
  if (F & FZ) {
    CYCT(0x73fd, 0x73ff);
    agesFunc_10_7298__func_7407_hook(gb);
    return;
  }
  CYC(0x73fd, 0x73ff);
  CYC(0x73ff, 0x7402); A = mem_rd(gb, wKeysJustPressed);
  CYC(0x7402, 0x7404); alu_and(gb, 0x01);
  if (F & FZ) {
    CYC(0x7404, 0x7406);
    CYC(0x7406, 0x7407); ret_effect(gb);
    return;
  }
  CYCT(0x7404, 0x7406);
  CALL_C(0x740c, incCbc2_hook, 0x3067, 0x740f);
  CYC(0x740f, 0x7411); A = 0xfa;
  CALL_C(0x7411, playSound_b00_hook, 0x0c98, 0x7414);
  CYC(0x7414, 0x7417); fadeoutToWhite_hook(gb);
}

void agesFunc_10_7298__func_7407_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7407, 0x740a); A = mem_rd(gb, 0xcfde);
  CYC(0x740a, 0x740b); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x740b, 0x740c); ret_effect(gb);
    return;
  }
  CYC(0x740b, 0x740c);
  CALL_C(0x740c, incCbc2_hook, 0x3067, 0x740f);
  CYC(0x740f, 0x7411); A = 0xfa;
  CALL_C(0x7411, playSound_b00_hook, 0x0c98, 0x7414);
  CYC(0x7414, 0x7417); fadeoutToWhite_hook(gb);
}

void agesFunc_10_7298__substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7440, agesFunc_10_7298__func_7450_hook, 0x7450, 0x7443);
  CYC(0x7443, 0x7446); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7446, 0x7447); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7447, 0x7448); ret_effect(gb);
    return;
  }
  CYC(0x7447, 0x7448);
  CYC(0x7448, 0x744b); SET_HL(wTmpcbb3);
  CYC(0x744b, 0x744d); mem_wr(gb, HL, 0xb4);
  CYC(0x744d, 0x7450); incCbc2_hook(gb);
}

void agesFunc_10_7298__substateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x745e, agesFunc_10_7298__func_7450_hook, 0x7450, 0x7461);
  CYC(0x7461, 0x7464); SET_HL(wTmpcbb3);
  CYC(0x7464, 0x7465); A = mem_rd(gb, HL);
  CYC(0x7465, 0x7466); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7466, 0x7468);
    agesFunc_10_7298__func_746a_hook(gb);
    return;
  }
  CYC(0x7466, 0x7468);
  CYC(0x7468, 0x7469); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7469, 0x746a); ret_effect(gb);
}
