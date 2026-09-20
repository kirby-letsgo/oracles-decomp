#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodeb5), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodeb5), (from), (to), true)

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
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+14);
  CYC(b_+14, b_+16); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CALL_C(b_+21, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+28); mem_wr(gb, wOamEnd, A);
  CYC(b_+28, b_+30); A = 0x78;
  CYC(b_+30, b_+32); E = 0x46;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+36); SET_BC((SYM(interactionCodea1__state9) + 11));
  CYC(b_+36, b_+39); createEnergySwirlGoingIn_hook(gb); return;
}

void interactionCodeb5__substate0_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+49, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+52);
  if (!(F & FZ)) { CYCT(b_+52, b_+53); ret_effect(gb); return; }
  CYC(b_+52, b_+53);
  CALL_C(b_+53, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+56);
  CYC(b_+56, b_+58); L = 0x46;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x08);
  CYC(b_+60, b_+63); SET_HL(wFakeResetMenu);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0);
  CYC(b_+65, b_+68); SET_HL(wFileSelect_fontXor);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0xff);
  CYC(b_+70, b_+71); ret_effect(gb);
}

void interactionCodeb5__substate1_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+71, b_+73); E = 0x46;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+75, b_+77); goto after_force_state; }
  CYC(b_+75, b_+77);
  CALL_C(b_+77, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+80);
  CYC(b_+80, b_+83); SET_HL(w1Link_visible);
  CYC(b_+83, b_+85); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
after_force_state:
  CALL_C(b_+85, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+88);
  CYC(b_+88, b_+91); SET_HL(wFakeResetMenu);
  CYC(b_+91, b_+93); B = 1;
  CALL_C(b_+93, flashScreen_hook, SYM(flashScreen), b_+96);
  if (F & FZ) { CYCT(b_+96, b_+97); ret_effect(gb); return; }
  CYC(b_+96, b_+97);
  CALL_C(b_+97, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+100);
  CYC(b_+100, b_+102); A = 3;
  CYC(b_+102, b_+106); fadeinFromWhiteWithDelay_hook(gb); return;
}

void interactionCodeb5__substate2_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+105, b_+108); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+108, b_+109); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+109, b_+110); ret_effect(gb); return; }
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+111); alu_xor(gb, A);
  CYC(b_+111, b_+114); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+114, b_+117); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+117, b_+120); mem_wr(gb, wMenuUnionEnd, A);
  CYC(b_+120, SYM(interactionCodeb8)); interactionDelete_hook(gb); return;
}

void interactionCodeb5__state1_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+39, b_+41); E = 0x45;
  CYC(b_+41, b_+43); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); push_effect(gb, b_+43);
  do { uint16_t jt_ = (final_dungeon_energy_jump_table(gb));
    if (jt_ == b_+49) { interactionCodeb5__substate0_hook(gb); return; }
    else if (jt_ == b_+71) { interactionCodeb5__substate1_hook(gb); return; }
    else if (jt_ == b_+105) { interactionCodeb5__substate2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void interactionCodeb5_hook(GB *gb) {
  BASE(interactionCodeb5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+4); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (final_dungeon_energy_jump_table(gb));
    if (jt_ == b_+8) { interactionCodeb5__state0_hook(gb); return; }
    else if (jt_ == b_+39) { interactionCodeb5__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
