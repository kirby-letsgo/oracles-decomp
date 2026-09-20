#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void setTrigger2IfTriggers0And1Set_hook(GB *gb);
void makeTorchesLightable_hook(GB *gb);
void func_4f5d_hook(GB *gb);
void spawnBridge_hook(GB *gb);
void mermaidsCave_spawnBridge_room38_hook(GB *gb);
void herosCave_spawnBridge_roomc9_hook(GB *gb);
void ancientTomb_startWallRetractionCutscene_hook(GB *gb);
void moonlitGrotto_enableControlAfterBreakingCrystal_hook(GB *gb);
void label_15_031_hook(GB *gb);

void setTrigger2IfTriggers0And1Set_hook(GB *gb) {
  BASE(setTrigger2IfTriggers0And1Set);
  CYC(b_+0, b_+3); SET_HL(wActiveTriggers);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto notSet;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 2));
  RET(b_+12);
  return;

notSet:
  CYC(b_+13, b_+15); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 2));
  RET(b_+15);
}

void makeTorchesLightable_hook(GB *gb) {
  BASE(makeTorchesLightable);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    RET_TAKEN(b_+3); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xc7);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08);
  CYC(b_+9, b_+11); L = 0x4b;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x06);
  CYC(b_+13, b_+15); L = 0x4d;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x10);
  RET(b_+17);
}

void func_4f5d_hook(GB *gb) {
  BASE(func_4f5d);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  CYC(b_+5, b_+7); A = 0x4d;
  CYC(b_+7, b_+10); playSound_b00_hook(gb);
}

void spawnBridge_hook(GB *gb) {
  BASE(spawnBridge);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) {
    RET_TAKEN(b_+3); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0c);
  CYC(b_+6, b_+8); L = 0xc7;
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+11); L = 0xc9;
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+14); L = 0xcb;
  CYC(b_+14, b_+15); mem_wr(gb, HL, E);
  RET(b_+15);
}

void mermaidsCave_spawnBridge_room38_hook(GB *gb) {
  BASE(mermaidsCave_spawnBridge_room38);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(b_+5, b_+7); A = 0x4d;
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  CYC(b_+10, b_+13); SET_BC(0x0800);
  CYC(b_+13, b_+15); E = 0x69;
  CYC(b_+15, b_+18); spawnBridge_hook(gb);
}

void herosCave_spawnBridge_roomc9_hook(GB *gb) {
  BASE(herosCave_spawnBridge_roomc9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(b_+5, b_+7); A = 0x4d;
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  CYC(b_+10, b_+13); SET_BC(0x0803);
  CYC(b_+13, b_+15); E = 0x2a;
  CYC(b_+15, b_+18); spawnBridge_hook(gb);
}

void ancientTomb_startWallRetractionCutscene_hook(GB *gb) {
  BASE(ancientTomb_startWallRetractionCutscene);
  CYC(b_+0, b_+2); A = 0x0b;
  CYC(b_+2, b_+5); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+5, b_+8); resetLinkInvincibility_hook(gb);
}

void moonlitGrotto_enableControlAfterBreakingCrystal_hook(GB *gb) {
  BASE(moonlitGrotto_enableControlAfterBreakingCrystal);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+4, b_+7); mem_wr(gb, wMenuDisabled, A);
  label_15_031_hook(gb);
}

void label_15_031_hook(GB *gb) {
  BASE(label_15_031);
  CYC(b_+0, b_+3); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+3, b_+6); mem_wr(gb, wDisableWarpTiles, A);
  RET(b_+6);
}
