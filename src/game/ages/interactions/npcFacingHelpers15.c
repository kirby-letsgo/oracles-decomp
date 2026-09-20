#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void turnToFaceSomething_hook(GB *gb);
void turnToFaceSomethingAtInterval_hook(GB *gb);
void setLinkAnimation_hook(GB *gb);

void turnToFaceSomething_hook(GB *gb) {
  CYC(0x5613, 0x5615); A = 0x0f;
  turnToFaceSomethingAtInterval_hook(gb);
}

void turnToFaceSomethingAtInterval_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5615, 0x5616); B = A;
  CYC(0x5616, 0x5619); A = mem_rd(gb, 0xcc00);
  CYC(0x5619, 0x561a); alu_and(gb, B);
  if (!(F & FZ)) {
    RET_TAKEN(0x561a); return;
  }
  CYC(0x561a, 0x561b);
  CYC(0x561b, 0x561e); SET_HL(0x7877);
  CYC(0x561e, 0x5620); E = 0x0a;
  CALL_C(0x5620, interBankCall_hook, 0x008a, 0x5623);
  CALL_C(0x5623, objectGetRelativeAngle_hook, 0x1ea4, 0x5626);
  CALL_C(0x5626, convertAngleToDirection_hook, 0x26f9, 0x5629);
  CYC(0x5629, 0x562a); H = D;
  CYC(0x562a, 0x562c); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x562c, 0x562d); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    RET_TAKEN(0x562d); return;
  }
  CYC(0x562d, 0x562e);
  CYC(0x562e, 0x562f); mem_wr(gb, HL, A);
  CYC(0x562f, 0x5632); interactionSetAnimation_hook(gb);
}

void setLinkAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5632, 0x5633); push_effect(gb, DE);
  CYC(0x5633, 0x5635); D = 0xd0;
  CALL_C(0x5635, specialObjectSetAnimation_hook, 0x2b0a, 0x5638);
  CYC(0x5638, 0x5639); SET_DE(pop_effect(gb));
  RET(0x5639);
}
