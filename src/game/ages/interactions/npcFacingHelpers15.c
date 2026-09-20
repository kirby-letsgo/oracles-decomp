#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(turnToFaceSomething), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(turnToFaceSomething), (from), (to), true)

void turnToFaceSomething_hook(GB *gb);
void turnToFaceSomethingAtInterval_hook(GB *gb);
void setLinkAnimation_hook(GB *gb);

void turnToFaceSomething_hook(GB *gb) {
  BASE(turnToFaceSomething);
  CYC(b_+0, SYM(turnToFaceSomethingAtInterval)); A = 0x0f;
  turnToFaceSomethingAtInterval_hook(gb);
}

void turnToFaceSomethingAtInterval_hook(GB *gb) {
  BASE(turnToFaceSomethingAtInterval);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wFrameCounter);
  CYC(b_+4, b_+5); alu_and(gb, B);
  if (!(F & FZ)) {
    RET_TAKEN(b_+5); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL((SYM(symmetryNpcSubid8And9Script__almostDoneTalking_b15) + 3));
  CYC(b_+9, b_+11); E = 0x0a;
  CALL_C(b_+11, interBankCall_hook, 0x008a, b_+14);
  CALL_C(b_+14, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+17);
  CALL_C(b_+17, convertAngleToDirection_hook, SYM(convertAngleToDirection), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    RET_TAKEN(b_+24); return;
  }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, SYM(setLinkAnimation)); interactionSetAnimation_hook(gb);
}

void setLinkAnimation_hook(GB *gb) {
  BASE(setLinkAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); D = 0xd0;
  CALL_C(b_+3, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+6);
  CYC(b_+6, b_+7); SET_DE(pop_effect(gb));
  RET(b_+7);
}
