#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void rickyCutsceneJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d10, 0x6d13); SET_BC(0xfe00);
  CALL_C(0x6d13, objectSetSpeedZ_hook, 0x239d, 0x6d16);
  CYC(0x6d16, 0x6d18); A = 0x02;
  CYC(0x6d18, 0x6d1b); specialObjectSetAnimation_hook(gb);
}

void companionCutsceneInitOam_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d1b, 0x6d1d); A = 0x01;
  CYC(0x6d1d, 0x6d1e); mem_wr(gb, DE, A);
  CYC(0x6d1e, 0x6d21); SET_HL(0x41f7);
  CYC(0x6d21, 0x6d23); E = 0x05;
  CALL_C(0x6d23, interBankCall_hook, 0x008a, 0x6d26);
  CYC(0x6d26, 0x6d29); objectSetVisiblec0_hook(gb);
}

void companionCutsceneFunc_7081_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7081, 0x7083); alu_sub(gb, 0x04);
  CYC(0x7083, 0x7085); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7085, 0x7086); ret_effect(gb); return; }
  CYC(0x7085, 0x7086);
  CYC(0x7086, 0x7088); E = 0x09;
  CALL_C(0x7088, convertAngleDeToDirection_hook, 0x26f8, 0x708b);
  CYC(0x708b, 0x708c); A = alu_dec8(gb, A);
  CYC(0x708c, 0x708e); alu_and(gb, 0x03);
  CYC(0x708e, 0x708f); H = D;
  CYC(0x708f, 0x7091); L = 0x08;
  CYC(0x7091, 0x7092); mem_wr(gb, HL, A);
  CYC(0x7092, 0x7094); L = 0x36;
  CYC(0x7094, 0x7095); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7095, 0x7098); specialObjectSetAnimation_hook(gb);
}

void companionCutsceneDecAngle_hook(GB *gb) {
  CYC(0x7098, 0x709a); E = 0x09;
  CYC(0x709a, 0x709b); A = mem_rd(gb, DE);
  CYC(0x709b, 0x709c); A = alu_dec8(gb, A);
  CYC(0x709c, 0x709e); alu_and(gb, 0x1f);
  CYC(0x709e, 0x709f); mem_wr(gb, DE, A);
  CYC(0x709f, 0x70a0); ret_effect(gb);
}
