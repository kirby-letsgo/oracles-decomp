#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  CYC(0x4f3b, 0x4f3e); SET_HL(wActiveTriggers);
  CYC(0x4f3e, 0x4f3f); A = mem_rd(gb, HL);
  CYC(0x4f3f, 0x4f41); alu_and(gb, 0x03);
  CYC(0x4f41, 0x4f43); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x4f43, 0x4f45);
    goto notSet;
  }
  CYC(0x4f43, 0x4f45);
  CYC(0x4f45, 0x4f47); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 2));
  RET(0x4f47);
  return;

notSet:
  CYC(0x4f48, 0x4f4a); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 2));
  RET(0x4f4a);
}

void makeTorchesLightable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f4b, getFreeInteractionSlot_hook, 0x3aef, 0x4f4e);
  if (!(F & FZ)) {
    RET_TAKEN(0x4f4e); return;
  }
  CYC(0x4f4e, 0x4f4f);
  CYC(0x4f4f, 0x4f51); mem_wr(gb, HL, 0xc7);
  CYC(0x4f51, 0x4f52); L = alu_inc8(gb, L);
  CYC(0x4f52, 0x4f54); mem_wr(gb, HL, 0x08);
  CYC(0x4f54, 0x4f56); L = 0x4b;
  CYC(0x4f56, 0x4f58); mem_wr(gb, HL, 0x06);
  CYC(0x4f58, 0x4f5a); L = 0x4d;
  CYC(0x4f5a, 0x4f5c); mem_wr(gb, HL, 0x10);
  RET(0x4f5c);
}

void func_4f5d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f5d, getThisRoomFlags_hook, 0x197d, 0x4f60);
  CYC(0x4f60, 0x4f62); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  CYC(0x4f62, 0x4f64); A = 0x4d;
  CYC(0x4f64, 0x4f67); playSound_b00_hook(gb);
}

void spawnBridge_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f67, getFreePartSlot_hook, 0x3e8e, 0x4f6a);
  if (!(F & FZ)) {
    RET_TAKEN(0x4f6a); return;
  }
  CYC(0x4f6a, 0x4f6b);
  CYC(0x4f6b, 0x4f6d); mem_wr(gb, HL, 0x0c);
  CYC(0x4f6d, 0x4f6f); L = 0xc7;
  CYC(0x4f6f, 0x4f70); mem_wr(gb, HL, B);
  CYC(0x4f70, 0x4f72); L = 0xc9;
  CYC(0x4f72, 0x4f73); mem_wr(gb, HL, C);
  CYC(0x4f73, 0x4f75); L = 0xcb;
  CYC(0x4f75, 0x4f76); mem_wr(gb, HL, E);
  RET(0x4f76);
}

void mermaidsCave_spawnBridge_room38_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f77, getThisRoomFlags_hook, 0x197d, 0x4f7a);
  CYC(0x4f7a, 0x4f7c); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(0x4f7c, 0x4f7e); A = 0x4d;
  CALL_C(0x4f7e, playSound_b00_hook, 0x0c98, 0x4f81);
  CYC(0x4f81, 0x4f84); SET_BC(0x0800);
  CYC(0x4f84, 0x4f86); E = 0x69;
  CYC(0x4f86, 0x4f89); spawnBridge_hook(gb);
}

void herosCave_spawnBridge_roomc9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f89, getThisRoomFlags_hook, 0x197d, 0x4f8c);
  CYC(0x4f8c, 0x4f8e); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));
  CYC(0x4f8e, 0x4f90); A = 0x4d;
  CALL_C(0x4f90, playSound_b00_hook, 0x0c98, 0x4f93);
  CYC(0x4f93, 0x4f96); SET_BC(0x0803);
  CYC(0x4f96, 0x4f98); E = 0x2a;
  CYC(0x4f98, 0x4f9b); spawnBridge_hook(gb);
}

void ancientTomb_startWallRetractionCutscene_hook(GB *gb) {
  CYC(0x4f9b, 0x4f9d); A = 0x0b;
  CYC(0x4f9d, 0x4fa0); mem_wr(gb, 0xcc04, A);
  CYC(0x4fa0, 0x4fa3); resetLinkInvincibility_hook(gb);
}

void moonlitGrotto_enableControlAfterBreakingCrystal_hook(GB *gb) {
  CYC(0x4fa3, 0x4fa4); alu_xor(gb, A);
  CYC(0x4fa4, 0x4fa7); mem_wr(gb, 0xcc8a, A);
  CYC(0x4fa7, 0x4faa); mem_wr(gb, 0xcc02, A);
  label_15_031_hook(gb);
}

void label_15_031_hook(GB *gb) {
  CYC(0x4faa, 0x4fad); mem_wr(gb, 0xcc91, A);
  CYC(0x4fad, 0x4fb0); mem_wr(gb, 0xcc90, A);
  RET(0x4fb0);
}
