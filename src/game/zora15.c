#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void zora_createExclamationMark_hook(GB *gb) {
  CYC(0x76de, 0x76e1); SET_BC(0xf200);
  CYC(0x76e1, 0x76e3); A = 0x1e;
  CYC(0x76e3, 0x76e6); objectCreateExclamationMark_hook(gb);
}

void zora_beginJump_hook(GB *gb) {
  CYC(0x76e6, 0x76e9); SET_BC(0xff00);
  CYC(0x76e9, 0x76ec); objectSetSpeedZ_hook(gb);
}

void zora_makeLinkFaceDown_hook(GB *gb) {
  CYC(0x76ec, 0x76ee); A = 0x02;
  CYC(0x76ee, 0x76f1); mem_wr(gb, w1Link_direction, A);
  CYC(0x76f1, 0x76f4); clearAllParentItems_hook(gb);
}

void zora_moveToLinksXPosition_hook(GB *gb) {
  CYC(0x76f4, 0x76f7); A = mem_rd(gb, w1Link_xh);
  CYC(0x76f7, 0x76f8); B = A;
  CYC(0x76f8, 0x76fa); E = 0x4d;
  CYC(0x76fa, 0x76fb); A = mem_rd(gb, DE);
  CYC(0x76fb, 0x76fc); alu_sub(gb, B);
  CYC(0x76fc, 0x76fe); E = 0x47;
  CYC(0x76fe, 0x76ff); mem_wr(gb, DE, A);
  CYC(0x76ff, 0x7700); ret_effect(gb);
}

void zora_waitForLinkToMoveDown_hook(GB *gb) {
  CYC(0x7700, 0x7703); A = mem_rd(gb, w1Link_yh);
  CYC(0x7703, 0x7705); alu_cp(gb, 0x18);
  CYC(0x7705, 0x7707); A = 0x01;
  if (F & FC) { CYCT(0x7707, 0x7709); goto store; }
  CYC(0x7707, 0x7709);
  CYC(0x7709, 0x770a); A = alu_dec8(gb, A);
store:
  CYC(0x770a, 0x770d); mem_wr(gb, 0xcfc1, A);
  CYC(0x770d, 0x770e); ret_effect(gb);
}

void zora_checkIsLinkedGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x770e, checkIsLinkedGame_hook, 0x1992, 0x7711);
  CYC(0x7711, 0x7713); A = 0x01;
  if (!(F & FZ)) { CYCT(0x7713, 0x7715); goto store; }
  CYC(0x7713, 0x7715);
  CYC(0x7715, 0x7716); A = alu_dec8(gb, A);
store:
  CYC(0x7716, 0x7719); mem_wr(gb, 0xcfc1, A);
  CYC(0x7719, 0x771a); ret_effect(gb);
}

void zora_createExclamationMarkToTheRight_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x771a, 0x771c); A = 0x50;
  CALL_C(0x771c, playSound_b00_hook, 0x0c98, 0x771f);
  CYC(0x771f, 0x7721); A = 0x2d;
  CYC(0x7721, 0x7724); SET_BC(0xf808);
  CYC(0x7724, 0x7727); objectCreateExclamationMark_hook(gb);
}

static void zora_setLinkDirection(GB *gb) {
  CYC(0x7735, 0x7738); mem_wr(gb, w1Link_direction, A);
  CYC(0x7738, 0x7739); ret_effect(gb);
}

void zora_setLinkDirectionUp_hook(GB *gb) {
  CYC(0x7727, 0x7729); A = 0x00;
  CYC(0x7729, 0x772b); zora_setLinkDirection(gb);
}

void zora_setLinkDirectionRight_hook(GB *gb) {
  CYC(0x772b, 0x772d); A = 0x01;
  CYC(0x772d, 0x772f); zora_setLinkDirection(gb);
}

void zora_setLinkDirectionDown_hook(GB *gb) {
  CYC(0x772f, 0x7731); A = 0x02;
  CYC(0x7731, 0x7733); zora_setLinkDirection(gb);
}

void zora_setLinkDirectionLeft_hook(GB *gb) {
  CYC(0x7733, 0x7735); A = 0x03;
  zora_setLinkDirection(gb);
}
