#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void func_5d41_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d41, refreshLoadedTreeGfx_hook, 0x1613, 0x5d44);
  CYC(0x5d44, 0x5d47); A = mem_rd(gb, wWarpTransition2);
  CYC(0x5d47, 0x5d48); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5d48, 0x5d4b);
    applyWarpTransition2_hook(gb);
    return;
  }
  CYC(0x5d48, 0x5d4b);
  CYC(0x5d4b, 0x5d4e);
  updateAllObjects_hook(gb);
}

void cutscene16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5de4, updateMenus_hook, 0x1a51, 0x5de7);
  if (!(F & FZ)) {
    CYCT(0x5de7, 0x5de8); ret_effect(gb);
    return;
  }
  CYC(0x5de7, 0x5de8);
  CYC(0x5de8, 0x5deb); SET_HL(wWarpTransition2);
  CYC(0x5deb, 0x5dec); A = mem_rd(gb, HL);
  CYC(0x5dec, 0x5dee); mem_wr(gb, HL, 0x00);
  CYC(0x5dee, 0x5def); A = alu_inc8(gb, A);
  CYC(0x5def, 0x5df1); A = 0x03;
  if (!(F & FZ)) {
    CYCT(0x5df1, 0x5df3);
  } else {
    CYC(0x5df1, 0x5df3);
    CALL_C(0x5df3, updateAllObjects_hook, 0x345b, 0x5df6);
    CYC(0x5df6, 0x5df8); A = 0x01;
  }
  CYC(0x5df8, 0x5dfb); mem_wr(gb, 0xc2ef, A);
  CYC(0x5dfb, 0x5dfc); alu_xor(gb, A);
  CYC(0x5dfc, 0x5dff); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5dff, 0x5e02); mem_wr(gb, wLinkCanPassNpcs, A);
  CYC(0x5e02, 0x5e05); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(0x5e05, 0x5e06); ret_effect(gb);
}

void triggerFadeoutTransition_hook(GB *gb) {
  CYC(0x5e06, 0x5e08); A = 0x05;
  CYC(0x5e08, 0x5e0b); mem_wr(gb, 0xc2ef, A);
  CYC(0x5e0b, 0x5e0e);
  fadeoutToWhite_hook(gb);
}

void applyWarpTransition2_hook(GB *gb) {
  CYC(0x5e0e, 0x5e11); SET_HL(wWarpTransition2);
  CYC(0x5e11, 0x5e12); A = mem_rd(gb, HL);
  CYC(0x5e12, 0x5e13); B = A;
  CYC(0x5e13, 0x5e15); mem_wr(gb, HL, 0x00);
  CYC(0x5e15, 0x5e17); alu_and(gb, 0x0f);
  CYC(0x5e17, 0x5e19); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(0x5e19, 0x5e1b);
  } else {
    CYC(0x5e19, 0x5e1b);
    CYC(0x5e1b, 0x5e1d); A = 0x01;
    CYC(0x5e1d, 0x5e20); mem_wr(gb, 0xc2ee, A);
    CYC(0x5e20, 0x5e21); alu_xor(gb, A);
    CYC(0x5e21, 0x5e24); mem_wr(gb, 0xc2ef, A);
    CYC(0x5e24, 0x5e25); ret_effect(gb);
    return;
  }
  CYC(0x5e25, 0x5e28); A = mem_rd(gb, wActiveRoom);
  CYC(0x5e28, 0x5e2a); alu_cp(gb, 0xd1);
  if (!(F & FZ)) {
    CYCT(0x5e2a, 0x5e2c);
  } else {
    CYC(0x5e2a, 0x5e2c);
    CYC(0x5e2c, 0x5e2d); B = alu_inc8(gb, B);
  }
  CYC(0x5e2d, 0x5e2e); A = B;
  CYC(0x5e2e, 0x5e30); alu_and(gb, 0x0f);
  CYC(0x5e30, 0x5e33); mem_wr(gb, 0xc2ef, A);
  CYC(0x5e33, 0x5e35); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(0x5e35, 0x5e38);
    fadeoutToWhite_hook(gb);
    return;
  }
  CYC(0x5e35, 0x5e38);
  CYC(0x5e38, 0x5e3a); A = 0x04;
  CYC(0x5e3a, 0x5e3d);
  fadeoutToWhiteWithDelay_hook(gb);
}

void setCutsceneIndexIfCutsceneTriggerSet_hook(GB *gb) {
  CYC(0x5e3d, 0x5e40); A = mem_rd(gb, wCutsceneTrigger);
  CYC(0x5e40, 0x5e42); alu_and(gb, 0x7f);
  CYC(0x5e42, 0x5e45); mem_wr(gb, 0xc2ef, A);
  CYC(0x5e45, 0x5e46); alu_xor(gb, A);
  CYC(0x5e46, 0x5e49); mem_wr(gb, wCutsceneTrigger, A);
  CYC(0x5e49, 0x5e4c); mem_wr(gb, wCutsceneState, A);
  CYC(0x5e4c, 0x5e4d); ret_effect(gb);
}

void checkDisplayEraOrSeasonInfo_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e7d, 0x5e7f); A = 0x16;
  CALL_C(0x5e7f, checkGlobalFlag_hook, 0x31f3, 0x5e82);
  if (F & FZ) {
    CYCT(0x5e82, 0x5e84);
  } else {
    CYC(0x5e82, 0x5e84);
    CYC(0x5e84, 0x5e86); A = 0x16;
    CYC(0x5e86, 0x5e89);
    unsetGlobalFlag_hook(gb);
    return;
  }
  CYC(0x5e89, 0x5e8c); A = mem_rd(gb, wSentBackByStrangeForce);
  CYC(0x5e8c, 0x5e8d); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x5e8d, 0x5e8e); ret_effect(gb);
    return;
  }
  CYC(0x5e8d, 0x5e8e);
  CYC(0x5e8e, 0x5e91); A = mem_rd(gb, wTilesetFlags);
  CYC(0x5e91, 0x5e93); alu_bit(gb, 4, A);
  if (!(F & FZ)) {
    CYCT(0x5e93, 0x5e94); ret_effect(gb);
    return;
  }
  CYC(0x5e93, 0x5e94);
  CYC(0x5e94, 0x5e96); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(0x5e96, 0x5e97); ret_effect(gb);
    return;
  }
  CYC(0x5e96, 0x5e97);
  CALL_C(0x5e97, getFreeInteractionSlot_hook, 0x3aef, 0x5e9a);
  if (!(F & FZ)) {
    CYCT(0x5e9a, 0x5e9b); ret_effect(gb);
    return;
  }
  CYC(0x5e9a, 0x5e9b);
  CYC(0x5e9b, 0x5e9d); mem_wr(gb, HL, 0xe0);
  CYC(0x5e9d, 0x5e9e); ret_effect(gb);
}

void updateGrassAnimationModifier_hook(GB *gb) {
  CYC(0x5e9e, 0x5ea0); A = 0x00;
  CYC(0x5ea0, 0x5ea3); mem_wr(gb, wGrassAnimationModifier, A);
  CYC(0x5ea3, 0x5ea4); ret_effect(gb);
}
