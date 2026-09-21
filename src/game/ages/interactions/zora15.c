#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void zora_createExclamationMark_hook(GB *gb) {
  BASE(zora_createExclamationMark);
  CYC(b_+0, b_+3); SET_BC(0xf200);
  CYC(b_+3, b_+5); A = 0x1e;
  CYC(b_+5, b_+8); TAIL(objectCreateExclamationMark);
}

void zora_beginJump_hook(GB *gb) {
  BASE(zora_beginJump);
  CYC(b_+0, b_+3); SET_BC(0xff00);
  CYC(b_+3, b_+6); TAIL(objectSetSpeedZ);
}

void zora_makeLinkFaceDown_hook(GB *gb) {
  BASE(zora_makeLinkFaceDown);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); mem_wr(gb, w1Link_direction, A);
  CYC(b_+5, b_+8); TAIL(clearAllParentItems);
}

void zora_moveToLinksXPosition_hook(GB *gb) {
  BASE(zora_moveToLinksXPosition);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_xh);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = 0x4d;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_sub(gb, B);
  CYC(b_+8, b_+10); E = 0x47;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void zora_waitForLinkToMoveDown_hook(GB *gb) {
  BASE(zora_waitForLinkToMoveDown);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+5); alu_cp(gb, 0x18);
  CYC(b_+5, b_+7); A = 0x01;
  if (F & FC) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
store:
  CYC(b_+10, b_+13); mem_wr(gb, wTmpcfc0_bigBangGame_filler1, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void zora_checkIsLinkedGame_hook(GB *gb) {
  BASE(zora_checkIsLinkedGame);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  CYC(b_+3, b_+5); A = 0x01;
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto store; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
store:
  CYC(b_+8, b_+11); mem_wr(gb, wTmpcfc0_bigBangGame_filler1, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void zora_createExclamationMarkToTheRight_hook(GB *gb) {
  BASE(zora_createExclamationMarkToTheRight);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x50;
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CYC(b_+5, b_+7); A = 0x2d;
  CYC(b_+7, b_+10); SET_BC(0xf808);
  CYC(b_+10, b_+13); TAIL(objectCreateExclamationMark);
}

static void zora_setLinkDirection(GB *gb) {
  BASE(zora_setLinkDirectionLeft);
  CYC(b_+2, b_+5); mem_wr(gb, w1Link_direction, A);
  CYC(b_+5, b_+6); ret_effect(gb);
}

void zora_setLinkDirectionUp_hook(GB *gb) {
  BASE(zora_setLinkDirectionUp);
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+4); zora_setLinkDirection(gb);
}

void zora_setLinkDirectionRight_hook(GB *gb) {
  BASE(zora_setLinkDirectionRight);
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); zora_setLinkDirection(gb);
}

void zora_setLinkDirectionDown_hook(GB *gb) {
  BASE(zora_setLinkDirectionDown);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); zora_setLinkDirection(gb);
}

void zora_setLinkDirectionLeft_hook(GB *gb) {
  BASE(zora_setLinkDirectionLeft);
  CYC(b_+0, b_+2); A = 0x03;
  zora_setLinkDirection(gb);
}
