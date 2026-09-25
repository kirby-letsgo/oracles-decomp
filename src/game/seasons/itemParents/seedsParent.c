#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/itemParents/seedsParent.s.

static uint16_t seedsParent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// ITEM_SLINGSHOT ($13)
// @snaddr{4d25}
void s_parentItemCode_slingshot_hook(GB *gb) {
  BASE(parentItemCode_slingshot);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (seedsParent_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == SYM(parentItemGenericState1) && hook_is(gb, SYM(parentItemGenericState1), s_parentItemGenericState1)) { s_parentItemGenericState1(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+11); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+15); A = mem_rd(gb, wIsSeedShooterInUse);
  CYC(b_+15, b_+16); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); TAIL(clearParentItem); }
  CYC(b_+16, b_+19);
  CALL_C(b_+19, s_updateLinkDirectionFromAngle, SYM(updateLinkDirectionFromAngle), b_+22);
  CYC(b_+22, b_+24); C = 0x01;
  CYC(b_+24, b_+27); A = mem_rd(gb, wSlingshotLevel);
  CYC(b_+27, b_+29); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto L_4d46; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); C = 0x03;
L_4d46:
  CALL_C(b_+33, s_getNumFreeItemSlots, SYM(getNumFreeItemSlots), b_+36);
  CYC(b_+36, b_+37); alu_cp(gb, C);
  if (F & FC) { CYCT(b_+37, b_+40); TAIL(clearParentItem); }
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+42); A = 0x01;
  CALL_C(b_+42, s_clearSelfIfNoSeeds, SYM(clearSelfIfNoSeeds), b_+45);
  CYC(b_+45, b_+46); push_effect(gb, BC);
  CALL_C(b_+46, s_parentItemLoadAnimationAndIncState, SYM(parentItemLoadAnimationAndIncState), b_+49);
  CALL_C(b_+49, s_itemCreateChild, SYM(itemCreateChild), b_+52);
  CYC(b_+52, b_+53); SET_BC(pop_effect(gb));
spawnSeed:
  CYC(b_+53, b_+55); E = 0x19;
  CYC(b_+55, b_+57); A = 0xd0;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+59); push_effect(gb, BC);
  CYC(b_+59, b_+61); E = 0x01;
  CALL_C(b_+61, s_itemCreateChildWithID, SYM(itemCreateChildWithID), b_+64);
  CYC(b_+64, b_+65); SET_BC(pop_effect(gb));
  CYC(b_+65, b_+66); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+66, b_+68); goto spawnSeed; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+69); A = B;
  CYC(b_+69, b_+72);
  TAIL(decNumActiveSeeds);
}

