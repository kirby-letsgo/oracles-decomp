#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void child_addValueToChildStatus_hook(GB *gb);
void child_checkHasRupees_hook(GB *gb);
void child_setStage8ResponseToSelectedTextOption_hook(GB *gb);
void child_setStage8Response_hook(GB *gb);
void child_playMusic_hook(GB *gb);
void child_giveHeartRefill_hook(GB *gb);
void child_giveOneHeart_hook(GB *gb);
void child_giveRupees_hook(GB *gb);

void child_addValueToChildStatus_hook(GB *gb) {
  CYC(0x5457, 0x545a); SET_HL(0xc60f);
  CYC(0x545a, 0x545b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x545b, 0x545c); mem_wr(gb, HL, A);
  RET(0x545c);
}

void child_checkHasRupees_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x545d, cpRupeeValue_hook, 0x1765, 0x5460);
  CYC(0x5460, 0x5462); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x5462, 0x5463); mem_wr(gb, DE, A);
  RET(0x5463);
}

void child_setStage8ResponseToSelectedTextOption_hook(GB *gb) {
  CYC(0x5464, 0x5467); SET_HL(0xcba5);
  CYC(0x5467, 0x5468); alu_add(gb, mem_rd(gb, HL));
  child_setStage8Response_hook(gb);
}

void child_setStage8Response_hook(GB *gb) {
  CYC(0x5468, 0x546b); mem_wr(gb, 0xc6e3, A);
  RET(0x546b);
}

void child_playMusic_hook(GB *gb) {
  CYC(0x546c, 0x546f); A = mem_rd(gb, 0xc6e3);
  CYC(0x546f, 0x5470); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5470, 0x5472);
    goto playHigh;
  }
  CYC(0x5470, 0x5472);
  CYC(0x5472, 0x5474); A = 0x38;
  CYC(0x5474, 0x5477); playSound_b00_hook(gb);
  return;

playHigh:
  CYC(0x5477, 0x5479); A = 0x4a;
  CYC(0x5479, 0x547c); playSound_b00_hook(gb);
}

static void child_giveHealthFromC(GB *gb) {
  CYC(0x5482, 0x5484); A = 0x29;
  CYC(0x5484, 0x5487); giveTreasure_hook(gb);
}

void child_giveHeartRefill_hook(GB *gb) {
  CYC(0x547c, 0x547e); C = 0x40;
  CYC(0x547e, 0x5480);
  child_giveHealthFromC(gb);
}

void child_giveOneHeart_hook(GB *gb) {
  CYC(0x5480, 0x5482); C = 0x04;
  child_giveHealthFromC(gb);
}

void child_giveRupees_hook(GB *gb) {
  CYC(0x5487, 0x5488); C = A;
  CYC(0x5488, 0x548a); A = 0x28;
  CYC(0x548a, 0x548d); giveTreasure_hook(gb);
}
