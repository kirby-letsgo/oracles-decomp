#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hOamTail, A);
  CYC(b_+3, b_+6); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (bank10_jump_table(gb));
    if (jt_ == b_+26) { agesFunc_10_70f6__substate0_hook(gb); return; }
    else if (jt_ == b_+80) { agesFunc_10_70f6__substate1_hook(gb); return; }
    else if (jt_ == b_+96) { agesFunc_10_70f6__substate2_hook(gb); return; }
    else if (jt_ == b_+166) { agesFunc_10_70f6__substate3_hook(gb); return; }
    else if (jt_ == b_+194) { agesFunc_10_70f6__substate4_hook(gb); return; }
    else if (jt_ == b_+206) { agesFunc_10_70f6__substate5_hook(gb); return; }
    else if (jt_ == b_+327) { agesFunc_10_70f6__substate6_hook(gb); return; }
    else if (jt_ == b_+347) { agesFunc_10_70f6__substate7_hook(gb); return; }
    else if (jt_ == b_+393) { agesFunc_10_70f6__substate8_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void agesFunc_10_7298_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_DE(wGenericCutscene_cbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (bank10_jump_table(gb));
    if (jt_ == b_+29) { agesFunc_10_7298__substate0_hook(gb); return; }
    else if (jt_ == b_+123) { agesFunc_10_7298__substate1_hook(gb); return; }
    else if (jt_ == b_+169) { agesFunc_10_7298__substate2_hook(gb); return; }
    else if (jt_ == b_+179) { agesFunc_10_7298__substate3_hook(gb); return; }
    else if (jt_ == b_+202) { agesFunc_10_7298__substate4_hook(gb); return; }
    else if (jt_ == b_+297) { agesFunc_10_7298__substate5_hook(gb); return; }
    else if (jt_ == b_+325) { agesFunc_10_7298__substate6_hook(gb); return; }
    else if (jt_ == b_+351) { agesFunc_10_7298__substate7_hook(gb); return; }
    else if (jt_ == b_+383) { agesFunc_10_7298__substate8_hook(gb); return; }
    else if (jt_ == b_+424) { agesFunc_10_7298__substate9_hook(gb); return; }
    else if (jt_ == b_+454) { agesFunc_10_7298__substateA_hook(gb); return; }
    else if (jt_ == b_+478) { agesFunc_10_7298__substateB_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void agesFunc_10_70f6__substate4_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+194, decCbb3_hook, SYM(decCbb3), b_+197);
  if (!(F & FZ)) {
    CYCT(b_+197, b_+198); ret_effect(gb);
    return;
  }
  CYC(b_+197, b_+198);
  CYC(b_+198, b_+200); A = 0xff;
  CYC(b_+200, b_+203); mem_wr(gb, wTmpcbba, A);
  CYC(b_+203, b_+206); incCbc2_hook(gb);
}

void agesFunc_10_70f6__func_71aa_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+180, decCbb3_hook, SYM(decCbb3), b_+183);
  if (!(F & FZ)) {
    CYCT(b_+183, b_+184); ret_effect(gb);
    return;
  }
  CYC(b_+183, b_+184);
  CYC(b_+184, b_+186); mem_wr(gb, HL, 0x04);
  CYC(b_+186, b_+189); SET_HL(wGfxRegs1_SCY);
  CYC(b_+189, b_+190); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+190, b_+191); A = mem_rd(gb, HL);
  CYC(b_+191, b_+193); mem_wr(gb, hCameraY, A);
  CYC(b_+193, b_+194); ret_effect(gb);
}

void agesFunc_10_70f6__func_71fd_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+263, b_+264); alu_xor(gb, A);
  CYC(b_+264, b_+266); mem_wr(gb, hOamTail, A);
  CYC(b_+266, b_+269); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+269, b_+271); alu_cp(gb, 0x60);
  if (!(F & FC)) {
    CYCT(b_+271, b_+273);
  } else {
    CYC(b_+271, b_+273);
    CYC(b_+273, b_+274); alu_cpl(gb);
    CYC(b_+274, b_+275); A = alu_inc8(gb, A);
    CYC(b_+275, b_+276); B = A;
    CYC(b_+276, b_+279); A = mem_rd(gb, wFrameCounter);
    CYC(b_+279, b_+281); alu_and(gb, 0x01);
    if (!(F & FZ)) {
      CYCT(b_+281, b_+283);
    } else {
      CYC(b_+281, b_+283);
      CYC(b_+283, b_+284); C = A;
      CYC(b_+284, b_+287); SET_HL((SYM(twinrova_updateAnimationFromAngle) + 6));
      CYC(b_+287, b_+289); E = 0x16;
      CALL_C(b_+289, addSpritesFromBankToOam_withOffset_hook, SYM(addSpritesFromBankToOam_withOffset), b_+292);
    }
  }
  CYC(b_+292, b_+295); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+295, b_+296); alu_cpl(gb);
  CYC(b_+296, b_+297); A = alu_inc8(gb, A);
  CYC(b_+297, b_+299); B = 0xc7;
  CYC(b_+299, b_+300); alu_add(gb, B);
  CYC(b_+300, b_+301); B = A;
  CYC(b_+301, b_+303); C = 0x38;
  CYC(b_+303, b_+306); SET_HL((SYM(twinrova_subid1_updateTargetPosition) + 16));
  CYC(b_+306, b_+308); E = 0x16;
  CYC(b_+308, b_+309); push_effect(gb, BC);
  CALL_C(b_+309, addSpritesFromBankToOam_withOffset_hook, SYM(addSpritesFromBankToOam_withOffset), b_+312);
  CYC(b_+312, b_+313); SET_BC(pop_effect(gb));
  CYC(b_+313, b_+316); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+316, b_+318); alu_cp(gb, 0x60);
  if (F & FC) {
    CYCT(b_+318, b_+319); ret_effect(gb);
    return;
  }
  CYC(b_+318, b_+319);
  CYC(b_+319, b_+322); SET_HL((SYM(twinrova_chooseObjectToAttack) + 9));
  CYC(b_+322, b_+324); E = 0x16;
  CYC(b_+324, b_+327); addSpritesFromBankToOam_withOffset_hook(gb);
}

void agesFunc_10_70f6__substate6_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+327, agesFunc_10_70f6__func_71fd_hook, b_+263, b_+330);
  CYC(b_+330, b_+333); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+333, b_+334); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+334, b_+335); ret_effect(gb);
    return;
  }
  CYC(b_+334, b_+335);
  CALL_C(b_+335, decCbb3_hook, SYM(decCbb3), b_+338);
  if (!(F & FZ)) {
    CYCT(b_+338, b_+339); ret_effect(gb);
    return;
  }
  CYC(b_+338, b_+339);
  CYC(b_+339, b_+341); A = 0x04;
  CYC(b_+341, b_+344); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+344, b_+347); incCbc2_hook(gb);
}

void agesFunc_10_70f6__func_7262_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+364, decCbb3_hook, SYM(decCbb3), b_+367);
  if (!(F & FZ)) {
    CYCT(b_+367, b_+369);
  } else {
    CYC(b_+367, b_+369);
    CYC(b_+369, b_+371); mem_wr(gb, HL, 0x04);
    CYC(b_+371, b_+374); SET_HL(wGfxRegs1_SCY);
    CYC(b_+374, b_+375); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+375, b_+376); A = mem_rd(gb, HL);
    CYC(b_+376, b_+378); mem_wr(gb, hCameraY, A);
    CYC(b_+378, b_+380); alu_cp(gb, 0x60);
    if (!(F & FZ)) {
      CYCT(b_+380, b_+382);
    } else {
      CYC(b_+380, b_+382);
      CALL_C(b_+382, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+385);
      CYC(b_+385, b_+387); A = 0x2c;
      CALL_C(b_+387, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+390);
    }
  }
  CYC(b_+390, b_+393); agesFunc_10_70f6__func_71fd_hook(gb);
}

void agesFunc_10_7298__func_731b_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+131, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+134);
  if (F & FZ) {
    CYCT(b_+134, b_+135); ret_effect(gb);
    return;
  }
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+138); SET_HL(wTmpcbb4);
  CYC(b_+138, b_+139); A = mem_rd(gb, HL);
  CYC(b_+139, b_+140); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+140, b_+142);
    TAIL(agesFunc_10_7298__playWaveSoundAtRandomIntervals_body);
  }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+143); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+143, b_+144); ret_effect(gb);
}

void agesFunc_10_7298__playWaveSoundAtRandomIntervals_body_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+144, b_+145); push_effect(gb, HL);
  CYC(b_+145, b_+147); A = 0xaa;
  CALL_C(b_+147, playSound_b00_hook, SYM(playSound_b00), b_+150);
  CYC(b_+150, b_+151); SET_HL(pop_effect(gb));
  CALL_C(b_+151, getRandomNumber_hook, SYM(getRandomNumber), b_+154);
  CYC(b_+154, b_+156); alu_and(gb, 0x03);
  CYC(b_+156, b_+159); SET_BC(b_+165);
  CALL_C(b_+159, addAToBc_hook, 0x006d, b_+162);
  CYC(b_+162, b_+163); A = mem_rd(gb, BC);
  CYC(b_+163, b_+164); mem_wr(gb, HL, A);
  CYC(b_+164, b_+165); ret_effect(gb);
}

void agesFunc_10_7298__func_7450_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  CYC(b_+440, b_+443); SET_HL((SYM(twinrova_subid0_targetPositions__pattern3) + 12));
  CYC(b_+443, b_+445); E = 0x16;
  CYC(b_+445, b_+448); SET_BC(0x3038);
  CYC(b_+448, b_+449); alu_xor(gb, A);
  CYC(b_+449, b_+451); mem_wr(gb, hOamTail, A);
  CYC(b_+451, b_+454); addSpritesFromBankToOam_withOffset_hook(gb);
}

void agesFunc_10_7298__func_746a_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+466, b_+469); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+469, b_+471); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+471, b_+472); ret_effect(gb);
    return;
  }
  CYC(b_+471, b_+472);
  CALL_C(b_+472, incCbc2_hook, SYM(incCbc2), b_+475);
  CYC(b_+475, b_+478); fadeoutToWhite_hook(gb);
}

void agesFunc_10_70f6__substate1_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  CYC(b_+80, b_+83); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+84, b_+85); ret_effect(gb);
    return;
  }
  CYC(b_+84, b_+85);
  CYC(b_+85, b_+88); SET_HL(wTmpcbb3);
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0xe0);
  CYC(b_+90, b_+91); SET_HL(HL + 1);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x01);
  CYC(b_+93, b_+96); incCbc2_hook(gb);
}

void agesFunc_10_70f6__substate2_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+96, b_+99); SET_HL(wTmpcbb3);
  CALL_C(b_+99, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+102);
  if (!(F & FZ)) {
    CYCT(b_+102, b_+103); ret_effect(gb);
    return;
  }
  CYC(b_+102, b_+103);
  CALL_C(b_+103, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+106);
  if (!(F & FZ)) {
    CYCT(b_+106, b_+108);
    TAIL(agesFunc_10_70f6__func_7174);
  }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+111); SET_HL((SYM(ramrockArm_state0__initSubid0) + 23));
  CYC(b_+111, b_+113); E = 0x03;
  CALL_C(b_+113, interBankCall_hook, 0x008a, b_+116);
  CYC(b_+116, b_+118); A = 0x03;
  CYC(b_+118, b_+121); mem_wr(gb, wGenericCutscene_cbc1, A);
  CYC(b_+121, b_+123); A = 0x04;
  CYC(b_+123, b_+126); fadeoutToWhiteWithDelay_hook(gb);
}

void agesFunc_10_70f6__func_7174_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+126, b_+128); A = 0x04;
  CYC(b_+128, b_+131); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+131, b_+134); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+134, b_+136); mem_wr(gb, hCameraY, A);
  CYC(b_+136, b_+138); A = 0x01;
  CALL_C(b_+138, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+141);
  CYC(b_+141, b_+143); A = 0x0b;
  CALL_C(b_+143, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+146);
  CYC(b_+146, b_+148); B = 0x03;
  for (;;) {
    CALL_C(b_+148, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+151);
    if (!(F & FZ)) {
      CYCT(b_+151, b_+153);
      break;
    }
    CYC(b_+151, b_+153);
    CYC(b_+153, b_+155); mem_wr(gb, HL, 0x4a);
    CYC(b_+155, b_+156); L = alu_inc8(gb, L);
    CYC(b_+156, b_+158); mem_wr(gb, HL, 0x09);
    CYC(b_+158, b_+159); L = alu_inc8(gb, L);
    CYC(b_+159, b_+160); B = alu_dec8(gb, B);
    CYC(b_+160, b_+161); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(b_+161, b_+163);
      continue;
    }
    CYC(b_+161, b_+163);
    break;
  }
  CYC(b_+163, b_+166); incCbc2_hook(gb);
}

void agesFunc_10_70f6__substate3_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  CYC(b_+166, b_+169); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+169, b_+170); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+170, b_+172);
    TAIL(agesFunc_10_70f6__func_71aa);
  }
  CYC(b_+170, b_+172);
  CYC(b_+172, b_+174); A = 0x78;
  CYC(b_+174, b_+177); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+177, b_+180); incCbc2_hook(gb);
}

void agesFunc_10_70f6__substate5_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+206, b_+209); SET_HL(wTmpcbb3);
  CYC(b_+209, b_+211); B = 0x01;
  CALL_C(b_+211, flashScreen_hook, SYM(flashScreen), b_+214);
  if (F & FZ) {
    CYCT(b_+214, b_+215); ret_effect(gb);
    return;
  }
  CYC(b_+214, b_+215);
  CALL_C(b_+215, disableLcd_hook, SYM(disableLcd), b_+218);
  CYC(b_+218, b_+220); A = 0x9a;
  CALL_C(b_+220, loadGfxHeader_hook, SYM(loadGfxHeader), b_+223);
  CYC(b_+223, b_+225); A = 0x9f;
  CALL_C(b_+225, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+228);
  CALL_C(b_+228, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+231);
  CYC(b_+231, b_+233); B = 0x03;
  for (;;) {
    CALL_C(b_+233, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+236);
    if (!(F & FZ)) {
      CYCT(b_+236, b_+238);
      break;
    }
    CYC(b_+236, b_+238);
    CYC(b_+238, b_+240); mem_wr(gb, HL, 0xcf);
    CYC(b_+240, b_+241); L = alu_inc8(gb, L);
    CYC(b_+241, b_+242); B = alu_dec8(gb, B);
    CYC(b_+242, b_+243); mem_wr(gb, HL, B);
    if (!(F & FZ)) {
      CYCT(b_+243, b_+245);
      continue;
    }
    CYC(b_+243, b_+245);
    break;
  }
  CYC(b_+245, b_+247); A = 0x04;
  CALL_C(b_+247, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+250);
  CYC(b_+250, b_+252); A = 0x04;
  CALL_C(b_+252, fadeinFromWhiteWithDelay_hook, SYM(fadeinFromWhiteWithDelay), b_+255);
  CALL_C(b_+255, incCbc2_hook, SYM(incCbc2), b_+258);
  CYC(b_+258, b_+260); A = 0xf0;
  CYC(b_+260, b_+263); mem_wr(gb, wTmpcbb3, A);
  TAIL(agesFunc_10_70f6__func_71fd);
}

void agesFunc_10_70f6__substate7_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+347, b_+350); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+350, b_+352); alu_cp(gb, 0x98);
  if (!(F & FZ)) {
    CYCT(b_+352, b_+354);
    TAIL(agesFunc_10_70f6__func_7262);
  }
  CYC(b_+352, b_+354);
  CYC(b_+354, b_+356); A = 0xf0;
  CYC(b_+356, b_+359); mem_wr(gb, wTmpcbb3, A);
  CALL_C(b_+359, incCbc2_hook, SYM(incCbc2), b_+362);
  CYC(b_+362, b_+364);
  CYC(b_+390, b_+393);
  TAIL(agesFunc_10_70f6__func_71fd);
}

void agesFunc_10_70f6__substate8_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+393, agesFunc_10_70f6__func_71fd_hook, b_+263, b_+396);
  CALL_C(b_+396, decCbb3_hook, SYM(decCbb3), b_+399);
  if (!(F & FZ)) {
    CYCT(b_+399, b_+400); ret_effect(gb);
    return;
  }
  CYC(b_+399, b_+400);
  CYC(b_+400, b_+403); SET_HL((SYM(ramrockArm_state0__initSubid0) + 23));
  CYC(b_+403, b_+405); E = 0x03;
  CALL_C(b_+405, interBankCall_hook, 0x008a, b_+408);
  CYC(b_+408, b_+410); A = 0x03;
  CYC(b_+410, b_+413); mem_wr(gb, wGenericCutscene_cbc1, A);
  CYC(b_+413, b_+415); A = 0x04;
  CYC(b_+415, b_+418); fadeoutToWhiteWithDelay_hook(gb);
}

void agesFunc_10_7298__substate1_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+123, b_+126); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+126, b_+127); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+127, b_+128); ret_effect(gb);
    return;
  }
  CYC(b_+127, b_+128);
  CALL_C(b_+128, incCbc2_hook, SYM(incCbc2), b_+131);
  TAIL(agesFunc_10_7298__func_731b);
}

void agesFunc_10_7298__substate2_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+169, agesFunc_10_7298__func_731b_hook, b_+131, b_+172);
  CALL_C(b_+172, decCbb3_hook, SYM(decCbb3), b_+175);
  if (!(F & FZ)) {
    CYCT(b_+175, b_+176); ret_effect(gb);
    return;
  }
  CYC(b_+175, b_+176);
  CALL_C(b_+176, incCbc2_hook, SYM(incCbc2), b_+179);
  TAIL(agesFunc_10_7298__substate3);
}

void agesFunc_10_7298__substate3_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+179, agesFunc_10_7298__func_731b_hook, b_+131, b_+182);
  CYC(b_+182, b_+185); SET_HL(wFileIsLinkedGame);
  CYC(b_+185, b_+186); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+186, b_+187); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+187, b_+189); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+189, b_+190); ret_effect(gb);
    return;
  }
  CYC(b_+189, b_+190);
  CYC(b_+190, b_+193); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+193, b_+195); alu_and(gb, 0x0b);
  if (F & FZ) {
    CYCT(b_+195, b_+196); ret_effect(gb);
    return;
  }
  CYC(b_+195, b_+196);
  CALL_C(b_+196, incCbc2_hook, SYM(incCbc2), b_+199);
  CYC(b_+199, b_+202); fadeoutToWhite_hook(gb);
}

void agesFunc_10_7298__substate5_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+297, fileSelect_redrawDecorations_hook, SYM(fileSelect_redrawDecorations), b_+300);
  CYC(b_+300, b_+303); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+303, b_+304); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+304, b_+305); ret_effect(gb);
    return;
  }
  CYC(b_+304, b_+305);
  CALL_C(b_+305, decCbb3_hook, SYM(decCbb3), b_+308);
  if (!(F & FZ)) {
    CYCT(b_+308, b_+309); ret_effect(gb);
    return;
  }
  CYC(b_+308, b_+309);
  CYC(b_+309, b_+312); SET_HL(wTmpcbb3);
  CYC(b_+312, b_+314); B = 0x3c;
  CALL_C(b_+314, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+317);
  if (F & FZ) {
    CYCT(b_+317, b_+319);
  } else {
    CYC(b_+317, b_+319);
    CYC(b_+319, b_+321); B = 0xb4;
  }
  CYC(b_+321, b_+322); mem_wr(gb, HL, B);
  CYC(b_+322, b_+325); incCbc2_hook(gb);
}

void agesFunc_10_7298__substate6_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+325, fileSelect_redrawDecorations_hook, SYM(fileSelect_redrawDecorations), b_+328);
  CALL_C(b_+328, decCbb3_hook, SYM(decCbb3), b_+331);
  if (!(F & FZ)) {
    CYCT(b_+331, b_+332); ret_effect(gb);
    return;
  }
  CYC(b_+331, b_+332);
  CALL_C(b_+332, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+335);
  if (!(F & FZ)) {
    CYCT(b_+335, b_+337);
  } else {
    CYC(b_+335, b_+337);
    CALL_C(b_+337, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+340);
    if (!(F & FZ)) {
      CYCT(b_+340, b_+342);
    } else {
      CYC(b_+340, b_+342);
      CYC(b_+342, b_+344); mem_wr(gb, HL, 0xd1);
      CYC(b_+344, b_+345); alu_xor(gb, A);
      CYC(b_+345, b_+348); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
    }
  }
  CYC(b_+348, b_+351); incCbc2_hook(gb);
}

void agesFunc_10_7298__substate7_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+351, fileSelect_redrawDecorations_hook, SYM(fileSelect_redrawDecorations), b_+354);
  CALL_C(b_+354, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+357);
  if (F & FZ) {
    CYCT(b_+357, b_+359);
    TAIL(agesFunc_10_7298__func_7407);
  }
  CYC(b_+357, b_+359);
  CYC(b_+359, b_+362); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+362, b_+364); alu_and(gb, 0x01);
  if (F & FZ) {
    CYC(b_+364, b_+366);
    CYC(b_+366, b_+367); ret_effect(gb);
    return;
  }
  CYCT(b_+364, b_+366);
  CALL_C(b_+372, incCbc2_hook, SYM(incCbc2), b_+375);
  CYC(b_+375, b_+377); A = 0xfa;
  CALL_C(b_+377, playSound_b00_hook, SYM(playSound_b00), b_+380);
  CYC(b_+380, b_+383); fadeoutToWhite_hook(gb);
}

void agesFunc_10_7298__func_7407_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+367, b_+370); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfde);
  CYC(b_+370, b_+371); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+371, b_+372); ret_effect(gb);
    return;
  }
  CYC(b_+371, b_+372);
  CALL_C(b_+372, incCbc2_hook, SYM(incCbc2), b_+375);
  CYC(b_+375, b_+377); A = 0xfa;
  CALL_C(b_+377, playSound_b00_hook, SYM(playSound_b00), b_+380);
  CYC(b_+380, b_+383); fadeoutToWhite_hook(gb);
}

void agesFunc_10_7298__substate9_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+424, agesFunc_10_7298__func_7450_hook, b_+440, b_+427);
  CYC(b_+427, b_+430); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+430, b_+431); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+431, b_+432); ret_effect(gb);
    return;
  }
  CYC(b_+431, b_+432);
  CYC(b_+432, b_+435); SET_HL(wTmpcbb3);
  CYC(b_+435, b_+437); mem_wr(gb, HL, 0xb4);
  CYC(b_+437, b_+440); incCbc2_hook(gb);
}

void agesFunc_10_7298__substateA_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+454, agesFunc_10_7298__func_7450_hook, b_+440, b_+457);
  CYC(b_+457, b_+460); SET_HL(wTmpcbb3);
  CYC(b_+460, b_+461); A = mem_rd(gb, HL);
  CYC(b_+461, b_+462); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+462, b_+464);
    TAIL(agesFunc_10_7298__func_746a);
  }
  CYC(b_+462, b_+464);
  CYC(b_+464, b_+465); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+465, b_+466); ret_effect(gb);
}

void agesFunc_10_7298__substate8_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+383, fileSelect_redrawDecorations_hook, SYM(fileSelect_redrawDecorations), b_+386);
  CYC(b_+386, b_+389); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+389, b_+390); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+390, b_+391); ret_effect(gb);
    return;
  }
  CYC(b_+390, b_+391);
  CALL_C(b_+391, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+394);
  if (!(F & FZ)) {
    CYCT(b_+394, b_+397); resetGame_hook(gb);
    return;
  }
  CYC(b_+394, b_+397);
  CALL_C(b_+397, disableLcd_hook, SYM(disableLcd), b_+400);
  CALL_C(b_+400, clearOam_hook, SYM(clearOam), b_+403);
  CALL_C(b_+403, incCbc2_hook, SYM(incCbc2), b_+406);
  CYC(b_+406, b_+408); A = 0x96;
  CALL_C(b_+408, loadGfxHeader_hook, SYM(loadGfxHeader), b_+411);
  CYC(b_+411, b_+413); A = 0xa7;
  CALL_C(b_+413, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+416);
  CALL_C(b_+416, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+419);
  CYC(b_+419, b_+421); A = 0x04;
  CYC(b_+421, b_+424); loadGfxRegisterStateIndex_hook(gb);
}

void agesFunc_10_7298__substateB_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+478, agesFunc_10_7298__func_7450_hook, b_+440, b_+481);
  CYC(b_+481, b_+484); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+484, b_+485); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+485, b_+486); ret_effect(gb);
    return;
  }
  CYC(b_+485, b_+486);
  CYC(b_+486, b_+489); resetGame_hook(gb);
}

void agesFunc_10_70f6__substate0_hook(GB *gb) {
  BASE(agesFunc_10_70f6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+26, b_+29); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+31); ret_effect(gb); return; }
  CYC(b_+30, b_+31);
  CALL_C(b_+31, incCbc2_hook, SYM(incCbc2), b_+34);
  CALL_C(b_+34, disableLcd_hook, SYM(disableLcd), b_+37);
  CALL_C(b_+37, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+40);
  CALL_C(b_+40, clearOam_hook, SYM(clearOam), b_+43);
  CYC(b_+43, b_+44); alu_xor(gb, A);
  CYC(b_+44, b_+47); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A);
  CYC(b_+47, b_+49); A = 0x95;
  CALL_C(b_+49, loadGfxHeader_hook, SYM(loadGfxHeader), b_+52);
  CYC(b_+52, b_+54); A = 0xa0;
  CALL_C(b_+54, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+57);
  CYC(b_+57, b_+59); A = 0x09;
  CALL_C(b_+59, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+62);
  CALL_C(b_+62, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+65);
  CALL_C(b_+65, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+69); ret_effect(gb); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0xaf);
  CYC(b_+71, b_+73); L = 0x4b;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0xe8);
  CYC(b_+75, b_+76); L = alu_inc8(gb, L);
  CYC(b_+76, b_+77); L = alu_inc8(gb, L);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x50);
  CYC(b_+79, b_+80); ret_effect(gb);
}

void agesFunc_10_7298__substate0_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+29, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+32);
  if (!(F & FZ)) CALL_C_CC(b_+32, agesFunc_10_70f6__func_71fd_hook, SYM(agesFunc_10_70f6__func_71fd), b_+35);
  else CYC(b_+32, b_+35);
  CYC(b_+35, b_+38); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+40); ret_effect(gb); return; }
  CYC(b_+39, b_+40);
  CALL_C(b_+40, disableLcd_hook, SYM(disableLcd), b_+43);
  CALL_C(b_+43, incCbc2_hook, SYM(incCbc2), b_+46);
  CYC(b_+46, b_+49); SET_HL((SYM(ramrockArm_subid0) + 14)); CYC(b_+49, b_+51); E = 0x03;
  CALL_C(b_+51, interBankCall_hook, 0x008a, b_+54);
  CALL_C(b_+54, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+57);
  CALL_C(b_+57, clearOam_hook, SYM(clearOam), b_+60);
  CALL_C(b_+60, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+63);
  if (F & FZ) { CYCT(b_+63, b_+66); CYC(b_+84, b_+86); A = 0x98; CALL_C(b_+86, loadGfxHeader_hook, SYM(loadGfxHeader), b_+89); CYC(b_+89, b_+91); A = 0xa9; CALL_C(b_+91, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+94); }
  else { CYC(b_+63, b_+66); CYC(b_+66, b_+68); A = 0x99; CALL_C(b_+68, loadGfxHeader_hook, SYM(loadGfxHeader), b_+71); CYC(b_+71, b_+73); A = 0xaa; CALL_C(b_+73, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+76); CYC(b_+76, b_+79); SET_HL((SYM(ganon_stateC_substate4__nextSubstate) + 1)); CALL_C(b_+79, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+82); CYC(b_+82, b_+84); }
  CYC(b_+94, b_+96); A = 0x04; CALL_C(b_+96, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+99);
  CYC(b_+99, b_+100); alu_xor(gb, A); CYC(b_+100, b_+103); SET_HL(hCameraY);
  CYC(b_+103, b_+104); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(b_+105, b_+106); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(b_+106, b_+107); mem_wr(gb, HL, A); CYC(b_+107, b_+110); SET_HL(wTmpcbb3); CYC(b_+110, b_+112); mem_wr(gb, HL, 0xf0); CYC(b_+112, b_+113); mem_wr(gb, HL, A);
  CYC(b_+113, b_+115); A = 0xfb; CALL_C(b_+115, playSound_b00_hook, SYM(playSound_b00), b_+118); CYC(b_+118, b_+120); A = 0x04; CYC(b_+120, b_+123); fadeinFromWhiteWithDelay_hook(gb);
}

void agesFunc_10_7298__substate4_hook(GB *gb) {
  BASE(agesFunc_10_7298);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+202, b_+205); A = mem_rd(gb, wPaletteThread_mode); CYC(b_+205, b_+206); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+206, b_+207); ret_effect(gb); return; }
  CYC(b_+206, b_+207); CALL_C(b_+207, incCbc2_hook, SYM(incCbc2), b_+210); CALL_C(b_+210, disableLcd_hook, SYM(disableLcd), b_+213);
  CYC(b_+213, b_+216); SET_HL((SYM(mergedTwinrova_lavaRoom_stateE__doneAttack) + 19)); CYC(b_+216, b_+218); E = 0x03; CALL_C(b_+218, interBankCall_hook, 0x008a, b_+221);
  CYC(b_+221, b_+223); A = 0xff; CYC(b_+223, b_+226); mem_wr(gb, wTmpcbba, A); CYC(b_+226, b_+228); A = hram_rd(gb, 0x70); CYC(b_+228, b_+229); push_effect(gb, AF);
  CYC(b_+229, b_+231); A = 0x07; CYC(b_+231, b_+233); hram_wr(gb, 0x70, A); CYC(b_+233, b_+236); SET_HL(w7SecretText1); CYC(b_+236, b_+239); SET_DE(w2WaveScrollValues); CYC(b_+239, b_+242); SET_BC(0x1800);
  do { CYC(b_+242, b_+243); A = mem_rd(gb, HL); SET_HL(HL + 1); CALL_C(b_+243, copyTextCharacterGfx_hook, SYM(copyTextCharacterGfx), b_+246); CYC(b_+246, b_+247); B = alu_dec8(gb, B); if (!(F & FZ)) { CYCT(b_+247, b_+249); } else { CYC(b_+247, b_+249); break; } } while (true);
  CYC(b_+249, b_+250); SET_AF(pop_effect(gb)); CYC(b_+250, b_+252); hram_wr(gb, 0x70, A); CYC(b_+252, b_+254); A = 0x97; CALL_C(b_+254, loadGfxHeader_hook, SYM(loadGfxHeader), b_+257); CYC(b_+257, b_+259); A = 0x05; CALL_C(b_+259, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+262); CYC(b_+262, b_+264); A = 0x2b; CALL_C(b_+264, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+267); CALL_C(b_+267, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+270); CYC(b_+270, b_+272); A = 0x06; if (!(F & FZ)) CALL_C_CC(b_+272, loadGfxHeader_hook, SYM(loadGfxHeader), b_+275); else CYC(b_+272, b_+275); CALL_C(b_+275, clearDynamicInteractions_hook, SYM(clearDynamicInteractions), b_+278); CALL_C(b_+278, clearOam_hook, SYM(clearOam), b_+281); CYC(b_+281, b_+283); A = 0x04; CALL_C(b_+283, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+286); CYC(b_+286, b_+289); SET_HL(wTmpcbb3); CYC(b_+289, b_+291); mem_wr(gb, HL, 0x3c); CALL_C(b_+291, fileSelect_redrawDecorations_hook, SYM(fileSelect_redrawDecorations), b_+294); CYC(b_+294, b_+297); fadeinFromWhite_hook(gb);
}
