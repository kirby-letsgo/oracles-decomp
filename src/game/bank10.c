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
void agesFunc_10_7298__substate8_hook(GB *gb);
void agesFunc_10_7298__substateB_hook(GB *gb);

static uint16_t bank10_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void agesFunc_10_70f6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70f6, 0x70f7); alu_xor(gb, A);
  CYC(0x70f7, 0x70f9); hram_wr(gb, 0x9f, A);
  CYC(0x70f9, 0x70fc); SET_DE(0xcbc2);
  CYC(0x70fc, 0x70fd); A = mem_rd(gb, DE);
  CYC(0x70fd, 0x70fe); push_effect(gb, 0x70fe);
  switch (bank10_jump_table(gb)) {
    case 0x7110: agesFunc_10_70f6__substate0_hook(gb); return;
    case 0x7146: agesFunc_10_70f6__substate1_hook(gb); return;
    case 0x7156: agesFunc_10_70f6__substate2_hook(gb); return;
    case 0x719c: agesFunc_10_70f6__substate3_hook(gb); return;
    case 0x71b8: agesFunc_10_70f6__substate4_hook(gb); return;
    case 0x71c4: agesFunc_10_70f6__substate5_hook(gb); return;
    case 0x723d: agesFunc_10_70f6__substate6_hook(gb); return;
    case 0x7251: agesFunc_10_70f6__substate7_hook(gb); return;
    case 0x727f: agesFunc_10_70f6__substate8_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void agesFunc_10_7298_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7298, 0x729b); SET_DE(0xcbc2);
  CYC(0x729b, 0x729c); A = mem_rd(gb, DE);
  CYC(0x729c, 0x729d); push_effect(gb, 0x729d);
  switch (bank10_jump_table(gb)) {
    case 0x72b5: agesFunc_10_7298__substate0_hook(gb); return;
    case 0x7313: agesFunc_10_7298__substate1_hook(gb); return;
    case 0x7341: agesFunc_10_7298__substate2_hook(gb); return;
    case 0x734b: agesFunc_10_7298__substate3_hook(gb); return;
    case 0x7362: agesFunc_10_7298__substate4_hook(gb); return;
    case 0x73c1: agesFunc_10_7298__substate5_hook(gb); return;
    case 0x73dd: agesFunc_10_7298__substate6_hook(gb); return;
    case 0x73f7: agesFunc_10_7298__substate7_hook(gb); return;
    case 0x7417: agesFunc_10_7298__substate8_hook(gb); return;
    case 0x7440: agesFunc_10_7298__substate9_hook(gb); return;
    case 0x745e: agesFunc_10_7298__substateA_hook(gb); return;
    case 0x7476: agesFunc_10_7298__substateB_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

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

void agesFunc_10_7298__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7417, fileSelect_redrawDecorations_hook, 0x3099, 0x741a);
  CYC(0x741a, 0x741d); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x741d, 0x741e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x741e, 0x741f); ret_effect(gb);
    return;
  }
  CYC(0x741e, 0x741f);
  CALL_C(0x741f, checkIsLinkedGame_hook, 0x1992, 0x7422);
  if (!(F & FZ)) {
    CYCT(0x7422, 0x7425); resetGame_hook(gb);
    return;
  }
  CYC(0x7422, 0x7425);
  CALL_C(0x7425, disableLcd_hook, 0x02c1, 0x7428);
  CALL_C(0x7428, clearOam_hook, 0x049f, 0x742b);
  CALL_C(0x742b, incCbc2_hook, 0x3067, 0x742e);
  CYC(0x742e, 0x7430); A = 0x96;
  CALL_C(0x7430, loadGfxHeader_hook, 0x0626, 0x7433);
  CYC(0x7433, 0x7435); A = 0xa7;
  CALL_C(0x7435, loadPaletteHeader_hook, 0x050b, 0x7438);
  CALL_C(0x7438, fadeinFromWhite_hook, 0x3299, 0x743b);
  CYC(0x743b, 0x743d); A = 0x04;
  CYC(0x743d, 0x7440); loadGfxRegisterStateIndex_hook(gb);
}

void agesFunc_10_7298__substateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7476, agesFunc_10_7298__func_7450_hook, 0x7450, 0x7479);
  CYC(0x7479, 0x747c); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x747c, 0x747d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x747d, 0x747e); ret_effect(gb);
    return;
  }
  CYC(0x747d, 0x747e);
  CYC(0x747e, 0x7481); resetGame_hook(gb);
}

void agesFunc_10_70f6__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7110, 0x7113); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7113, 0x7114); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7114, 0x7115); ret_effect(gb); return; }
  CYC(0x7114, 0x7115);
  CALL_C(0x7115, incCbc2_hook, 0x3067, 0x7118);
  CALL_C(0x7118, disableLcd_hook, 0x02c1, 0x711b);
  CALL_C(0x711b, clearDynamicInteractions_hook, 0x35d2, 0x711e);
  CALL_C(0x711e, clearOam_hook, 0x049f, 0x7121);
  CYC(0x7121, 0x7122); alu_xor(gb, A);
  CYC(0x7122, 0x7125); mem_wr(gb, 0xcfde, A);
  CYC(0x7125, 0x7127); A = 0x95;
  CALL_C(0x7127, loadGfxHeader_hook, 0x0626, 0x712a);
  CYC(0x712a, 0x712c); A = 0xa0;
  CALL_C(0x712c, loadPaletteHeader_hook, 0x050b, 0x712f);
  CYC(0x712f, 0x7131); A = 0x09;
  CALL_C(0x7131, loadGfxRegisterStateIndex_hook, 0x02ea, 0x7134);
  CALL_C(0x7134, fadeinFromWhite_hook, 0x3299, 0x7137);
  CALL_C(0x7137, getFreeInteractionSlot_hook, 0x3aef, 0x713a);
  if (!(F & FZ)) { CYCT(0x713a, 0x713b); ret_effect(gb); return; }
  CYC(0x713a, 0x713b);
  CYC(0x713b, 0x713d); mem_wr(gb, HL, 0xaf);
  CYC(0x713d, 0x713f); L = 0x4b;
  CYC(0x713f, 0x7141); mem_wr(gb, HL, 0xe8);
  CYC(0x7141, 0x7142); L = alu_inc8(gb, L);
  CYC(0x7142, 0x7143); L = alu_inc8(gb, L);
  CYC(0x7143, 0x7145); mem_wr(gb, HL, 0x50);
  CYC(0x7145, 0x7146); ret_effect(gb);
}

void agesFunc_10_7298__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x72b5, checkIsLinkedGame_hook, 0x1992, 0x72b8);
  if (!(F & FZ)) CALL_C_CC(0x72b8, agesFunc_10_70f6__func_71fd_hook, 0x71fd, 0x72bb);
  else CYC(0x72b8, 0x72bb);
  CYC(0x72bb, 0x72be); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x72be, 0x72bf); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x72bf, 0x72c0); ret_effect(gb); return; }
  CYC(0x72bf, 0x72c0);
  CALL_C(0x72c0, disableLcd_hook, 0x02c1, 0x72c3);
  CALL_C(0x72c3, incCbc2_hook, 0x3067, 0x72c6);
  CYC(0x72c6, 0x72c9); SET_HL(0x60f1); CYC(0x72c9, 0x72cb); E = 0x03;
  CALL_C(0x72cb, interBankCall_hook, 0x008a, 0x72ce);
  CALL_C(0x72ce, clearDynamicInteractions_hook, 0x35d2, 0x72d1);
  CALL_C(0x72d1, clearOam_hook, 0x049f, 0x72d4);
  CALL_C(0x72d4, checkIsLinkedGame_hook, 0x1992, 0x72d7);
  if (F & FZ) { CYCT(0x72d7, 0x72da); CYC(0x72ec, 0x72ee); A = 0x98; CALL_C(0x72ee, loadGfxHeader_hook, 0x0626, 0x72f1); CYC(0x72f1, 0x72f3); A = 0xa9; CALL_C(0x72f3, loadPaletteHeader_hook, 0x050b, 0x72f6); }
  else { CYC(0x72d7, 0x72da); CYC(0x72da, 0x72dc); A = 0x99; CALL_C(0x72dc, loadGfxHeader_hook, 0x0626, 0x72df); CYC(0x72df, 0x72e1); A = 0xaa; CALL_C(0x72e1, loadPaletteHeader_hook, 0x050b, 0x72e4); CYC(0x72e4, 0x72e7); SET_HL(0x5574); CALL_C(0x72e7, parseGivenObjectData_b00_hook, 0x3171, 0x72ea); CYC(0x72ea, 0x72ec); }
  CYC(0x72f6, 0x72f8); A = 0x04; CALL_C(0x72f8, loadGfxRegisterStateIndex_hook, 0x02ea, 0x72fb);
  CYC(0x72fb, 0x72fc); alu_xor(gb, A); CYC(0x72fc, 0x72ff); SET_HL(0xffaa);
  CYC(0x72ff, 0x7300); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(0x7300, 0x7301); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(0x7301, 0x7302); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(0x7302, 0x7303); mem_wr(gb, HL, A); CYC(0x7303, 0x7306); SET_HL(wTmpcbb3); CYC(0x7306, 0x7308); mem_wr(gb, HL, 0xf0); CYC(0x7308, 0x7309); mem_wr(gb, HL, A);
  CYC(0x7309, 0x730b); A = 0xfb; CALL_C(0x730b, playSound_b00_hook, 0x0c98, 0x730e); CYC(0x730e, 0x7310); A = 0x04; CYC(0x7310, 0x7313); fadeinFromWhiteWithDelay_hook(gb);
}

void agesFunc_10_7298__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7362, 0x7365); A = mem_rd(gb, wPaletteThread_mode); CYC(0x7365, 0x7366); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7366, 0x7367); ret_effect(gb); return; }
  CYC(0x7366, 0x7367); CALL_C(0x7367, incCbc2_hook, 0x3067, 0x736a); CALL_C(0x736a, disableLcd_hook, 0x02c1, 0x736d);
  CYC(0x736d, 0x7370); SET_HL(0x481b); CYC(0x7370, 0x7372); E = 0x03; CALL_C(0x7372, interBankCall_hook, 0x008a, 0x7375);
  CYC(0x7375, 0x7377); A = 0xff; CYC(0x7377, 0x737a); mem_wr(gb, wTmpcbba, A); CYC(0x737a, 0x737c); A = hram_rd(gb, 0x70); CYC(0x737c, 0x737d); push_effect(gb, AF);
  CYC(0x737d, 0x737f); A = 0x07; CYC(0x737f, 0x7381); hram_wr(gb, 0x70, A); CYC(0x7381, 0x7384); SET_HL(0xd460); CYC(0x7384, 0x7387); SET_DE(0xd800); CYC(0x7387, 0x738a); SET_BC(0x1800);
  do { CYC(0x738a, 0x738b); A = mem_rd(gb, HL); SET_HL(HL + 1); CALL_C(0x738b, copyTextCharacterGfx_hook, 0x19df, 0x738e); CYC(0x738e, 0x738f); B = alu_dec8(gb, B); if (!(F & FZ)) { CYCT(0x738f, 0x7391); } else { CYC(0x738f, 0x7391); break; } } while (true);
  CYC(0x7391, 0x7392); SET_AF(pop_effect(gb)); CYC(0x7392, 0x7394); hram_wr(gb, 0x70, A); CYC(0x7394, 0x7396); A = 0x97; CALL_C(0x7396, loadGfxHeader_hook, 0x0626, 0x7399); CYC(0x7399, 0x739b); A = 0x05; CALL_C(0x739b, loadPaletteHeader_hook, 0x050b, 0x739e); CYC(0x739e, 0x73a0); A = 0x2b; CALL_C(0x73a0, loadUncompressedGfxHeader_hook, 0x05da, 0x73a3); CALL_C(0x73a3, checkIsLinkedGame_hook, 0x1992, 0x73a6); CYC(0x73a6, 0x73a8); A = 0x06; if (!(F & FZ)) CALL_C_CC(0x73a8, loadGfxHeader_hook, 0x0626, 0x73ab); else CYC(0x73a8, 0x73ab); CALL_C(0x73ab, clearDynamicInteractions_hook, 0x35d2, 0x73ae); CALL_C(0x73ae, clearOam_hook, 0x049f, 0x73b1); CYC(0x73b1, 0x73b3); A = 0x04; CALL_C(0x73b3, loadGfxRegisterStateIndex_hook, 0x02ea, 0x73b6); CYC(0x73b6, 0x73b9); SET_HL(wTmpcbb3); CYC(0x73b9, 0x73bb); mem_wr(gb, HL, 0x3c); CALL_C(0x73bb, fileSelect_redrawDecorations_hook, 0x3099, 0x73be); CYC(0x73be, 0x73c1); fadeinFromWhite_hook(gb);
}
