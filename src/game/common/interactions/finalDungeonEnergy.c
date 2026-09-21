#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t final_dungeon_energy_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCodeb5__state0_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(8), b_+OE(10)); A = 1;
  CYC(b_+O(10), b_+OE(11)); mem_wr(gb, DE, A);
  CALL_C(b_+O(11), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(14));
  CYC(b_+O(14), b_+OE(16)); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+O(16), b_+OE(19)); interactionDelete_hook(gb); return; }
  CYC(b_+O(16), b_+OE(19));
  CYC(b_+O(19), b_+OE(21)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CALL_C(b_+O(21), setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+OE(24));
  if (game_seasons) {
    CYC(b_+S(24), b_+S(26)); A = 0x09;
    CYC(b_+S(26), b_+S(29)); W8(wc6e5) = A;
  }
  CYC(b_+O(24), b_+OE(25)); alu_xor(gb, A);
  CYC(b_+O(25), b_+OE(28)); mem_wr(gb, wOamEnd, A);
  CYC(b_+O(28), b_+OE(30)); A = 0x78;
  CYC(b_+O(30), b_+OE(32)); E = 0x46;
  CYC(b_+O(32), b_+OE(33)); mem_wr(gb, DE, A);
  CYC(b_+O(33), b_+OE(36)); SET_BC(0x5878);
  CYC(b_+O(36), b_+OE(39)); TAIL(createEnergySwirlGoingIn);
}

void interactionCodeb5__substate0_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(49), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(52));
  if (!(F & FZ)) { CYCT(b_+O(52), b_+OE(53)); ret_effect(gb); return; }
  CYC(b_+O(52), b_+OE(53));
  CALL_C(b_+O(53), interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+OE(56));
  CYC(b_+O(56), b_+OE(58)); L = 0x46;
  CYC(b_+O(58), b_+OE(60)); mem_wr(gb, HL, 0x08);
  CYC(b_+O(60), b_+OE(63)); SET_HL(wFakeResetMenu);
  CYC(b_+O(63), b_+OE(65)); mem_wr(gb, HL, 0);
  CYC(b_+O(65), b_+OE(68)); SET_HL(wFileSelect_fontXor);
  CYC(b_+O(68), b_+OE(70)); mem_wr(gb, HL, 0xff);
  CYC(b_+O(70), b_+OE(71)); ret_effect(gb);
}

void interactionCodeb5__substate1_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(71), b_+OE(73)); E = 0x46;
  CYC(b_+O(73), b_+OE(74)); A = mem_rd(gb, DE);
  CYC(b_+O(74), b_+OE(75)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(75), b_+OE(77)); goto after_force_state; }
  CYC(b_+O(75), b_+OE(77));
  CALL_C(b_+O(77), setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+OE(80));
  CYC(b_+O(80), b_+OE(83)); SET_HL(w1Link_visible);
  CYC(b_+O(83), b_+OE(85)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
after_force_state:
  CALL_C(b_+O(85), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(88));
  CYC(b_+O(88), b_+OE(91)); SET_HL(wFakeResetMenu);
  CYC(b_+O(91), b_+OE(93)); B = 1;
  CALL_C(b_+O(93), flashScreen_hook, SYM(flashScreen), b_+OE(96));
  if (F & FZ) { CYCT(b_+O(96), b_+OE(97)); ret_effect(gb); return; }
  CYC(b_+O(96), b_+OE(97));
  CALL_C(b_+O(97), interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+OE(100));
  CYC(b_+O(100), b_+OE(102)); A = 3;
  CYC(b_+O(102), b_+OE(105)); TAIL(fadeinFromWhiteWithDelay);
}

void interactionCodeb5__substate2_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(105), b_+OE(108)); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+O(108), b_+OE(109)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(109), b_+OE(110)); ret_effect(gb); return; }
  CYC(b_+O(109), b_+OE(110));
  CYC(b_+O(110), b_+OE(111)); alu_xor(gb, A);
  CYC(b_+O(111), b_+OE(114)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(114), b_+OE(117)); mem_wr(gb, wMenuDisabled, A);
  if (!game_seasons) { CYC(b_+117, b_+120); mem_wr(gb, wMenuUnionEnd, A); }
  CYC(b_+O(120), b_+OE(123)); TAIL(interactionDelete);
}

void interactionCodeb5__state1_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(39), b_+OE(41)); E = 0x45;
  CYC(b_+O(41), b_+OE(43)); A = mem_rd(gb, DE);
  CYC(b_+O(42), b_+OE(43)); push_effect(gb, b_+OE(43));
  do { uint16_t jt_ = (final_dungeon_energy_jump_table(gb));
    if (jt_ == b_+O(49)) { interactionCodeb5__substate0_hook(gb); return; }
    else if (jt_ == b_+O(71)) { interactionCodeb5__substate1_hook(gb); return; }
    else if (jt_ == b_+O(105)) { interactionCodeb5__substate2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void interactionCodeb5_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x44;
  CYC(b_+O(2), b_+OE(4)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (final_dungeon_energy_jump_table(gb));
    if (jt_ == b_+O(8)) { interactionCodeb5__state0_hook(gb); return; }
    else if (jt_ == b_+O(39)) { interactionCodeb5__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
