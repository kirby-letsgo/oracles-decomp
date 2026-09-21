#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t other_swords_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void other_swords_state1(GB *gb) {
  BASE(parentItemCode_punch);
  CYC(b_+O(70), b_+OE(72)); E = 0x21;
  CYC(b_+O(72), b_+OE(73)); A = mem_rd(gb, DE);
  CYC(b_+O(73), b_+OE(74)); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+O(74), b_+OE(77)); specialObjectAnimate_optimized_hook(gb); }
  else { CYC(b_+O(74), b_+OE(77)); CYC(b_+O(77), b_+OE(80)); clearParentItem_hook(gb); }
}

void parentItemCode_foolsOre_hook(GB *gb) {
  BASE(parentItemCode_foolsOre);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (other_swords_jump_table(gb));
    if (jt_ == b_+O(8)) {
      CYC(b_+O(8), b_+OE(10)); E = 0x00;
      CYC(b_+O(10), b_+OE(12)); A = 0xff;
      CYC(b_+O(12), b_+OE(13)); mem_wr(gb, DE, A);
      CALL_C(b_+O(13), updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+OE(16));
      CALL_C(b_+O(16), parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+OE(19));
      CYC(b_+O(19), b_+OE(22)); TAIL(itemCreateChild);
    }
    else if (jt_ == SYM(parentItemCode_punch__state1)) { other_swords_state1(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void parentItemCode_rodOfSeasons_hook(GB *gb) {
  BASE(parentItemCode_rodOfSeasons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearParentItemIfCantUseSword_hook, SYM(clearParentItemIfCantUseSword), b_+3);
  CALL_C(b_+3, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+9); clearParentItem_hook(gb); return; }
  CYC(b_+6, b_+9);
  TAIL(parentItemCode_foolsOre);
}

void parentItemCode_punch_hook(GB *gb) {
  BASE(parentItemCode_punch);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (other_swords_jump_table(gb));
    if (jt_ == b_+O(8)) { goto state0; }
    else if (jt_ == b_+O(70)) { other_swords_state1(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+O(8), b_+OE(10)); E = 0x00;
  CYC(b_+O(10), b_+OE(12)); A = 0xff;
  CYC(b_+O(12), b_+OE(13)); mem_wr(gb, DE, A);
  CALL_C(b_+O(13), updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+OE(16));
  CALL_C(b_+O(16), parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+OE(19));
  CALL_C(b_+O(19), itemCreateChild_hook, SYM(itemCreateChild), b_+OE(22));
  CYC(b_+O(22), b_+OE(25)); A = W8(wActiveRing);
  CYC(b_+O(25), b_+OE(27)); alu_cp(gb, 0x0b);
  if (game_seasons) {
    if (!(F & FZ)) { CYCT(b_+S(27), b_+S(28)); ret_effect(gb); return; }
    CYC(b_+S(27), b_+S(28));
  } else {
    if (F & FZ) { CYCT(b_+27, b_+29); goto experts_ring; }
    CYC(b_+27, b_+29);
    CALL_C(b_+29, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+32);
    if (F & FZ) { CYCT(b_+32, b_+33); ret_effect(gb); return; }
    CYC(b_+32, b_+33);
    CYC(b_+33, b_+35); A = 0x37;
    CYC(b_+35, b_+38); TAIL(specialObjectSetAnimationWithLinkData);
  }

experts_ring:
  CYC(b_+O(38), b_+OE(40)); L = 0x02;
  CYC(b_+O(40), b_+OE(41)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(41), b_+OE(43)); C = 0x34;
  CYC(b_+O(43), b_+OE(46)); A = W8(wLinkObjectIndex);
  CYC(b_+O(46), b_+OE(47)); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+O(47), b_+OE(49)); goto not_riding; }
  CYC(b_+O(47), b_+OE(49));
  if (!game_seasons) {
    CYC(b_+49, b_+52); A = W8(w1Companion_id);
    CYC(b_+52, b_+54); alu_cp(gb, 0x13);
    if (F & FZ) { CYCT(b_+54, b_+56); goto set_animation; }
    CYC(b_+54, b_+56);
  }
  CYC(b_+O(56), b_+OE(57)); C = alu_inc8(gb, C);
  if (!game_seasons) CYC(b_+57, b_+59);
  goto set_animation;

not_riding:
  if (!game_seasons) {
    CALL_C(b_+59, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+62);
    if (F & FZ) CYCT(b_+62, b_+64);
    else { CYC(b_+62, b_+64); CYC(b_+64, b_+66); C = 0x36; }
  }

set_animation:
  CYC(b_+O(66), b_+OE(67)); A = C;
  CYC(b_+O(67), b_+OE(70)); TAIL(specialObjectSetAnimationWithLinkData);
}
