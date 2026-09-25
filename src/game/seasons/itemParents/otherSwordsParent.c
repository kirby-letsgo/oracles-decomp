#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/itemParents/otherSwordsParent.s.

static uint16_t otherSwordsParent_jump_table(GB *gb) {
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

// parentItemCode_punch@state1
static void otherSwordsParent_state1(GB *gb) {
  BASE(parentItemCode_punch);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+44, b_+46); E = 0x21;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+48, b_+51); TAIL(specialObjectAnimate_optimized); }
  CYC(b_+48, b_+51);
  CYC(b_+51, b_+54);
  TAIL(clearParentItem);
}

// parentItemCode_foolsOre@rod_state0
static void otherSwordsParent_rod_state0(GB *gb) {
  BASE(parentItemCode_foolsOre);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+8, b_+11); A = mem_rd(gb, wActiveTileType);
  CYC(b_+11, b_+13); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_4a62; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); E = 0x00;
  CYC(b_+17, b_+19); A = 0xff;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
L_4a62:
  CALL_C(b_+20, s_updateLinkDirectionFromAngle, SYM(updateLinkDirectionFromAngle), b_+23);
  CALL_C(b_+23, s_parentItemLoadAnimationAndIncState, SYM(parentItemLoadAnimationAndIncState), b_+26);
  CYC(b_+26, b_+29);
  TAIL(itemCreateChild);
}

// ITEM_ROD_OF_SEASONS ($07)
void s_parentItemCode_rodOfSeasons_hook(GB *gb) {
  BASE(parentItemCode_rodOfSeasons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_clearParentItemIfCantUseSword, SYM(clearParentItemIfCantUseSword), b_+3);
  CYC(b_+3, b_+5); E = 0x04;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (otherSwordsParent_jump_table(gb));
    if (jt_ == SYM(parentItemCode_foolsOre__rod_state0)) { otherSwordsParent_rod_state0(gb); return; }
    if (jt_ == SYM(parentItemCode_punch__state1)) { otherSwordsParent_state1(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// ITEM_BIGGORON_SWORD ($0c)
void s_parentItemCode_biggoronSword_hook(GB *gb) {
  BASE(parentItemCode_biggoronSword);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_clearParentItemIfCantUseSword, SYM(clearParentItemIfCantUseSword), b_+3);
  s_parentItemCode_foolsOre(gb); return; // falls through
}
