#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6a4c, 0x6a4e); A = 1;
  CYC(0x6a4e, 0x6a4f); mem_wr(gb, DE, A);
  CALL_C(0x6a4f, getThisRoomFlags_hook, 0x197d, 0x6a52);
  CYC(0x6a52, 0x6a54); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x6a54, 0x6a57); interactionDelete_hook(gb); return; }
  CYC(0x6a54, 0x6a57);
  CYC(0x6a57, 0x6a59); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CALL_C(0x6a59, setDeathRespawnPoint_hook, 0x1100, 0x6a5c);
  CYC(0x6a5c, 0x6a5d); alu_xor(gb, A);
  CYC(0x6a5d, 0x6a60); mem_wr(gb, 0xcba0, A);
  CYC(0x6a60, 0x6a62); A = 0x78;
  CYC(0x6a62, 0x6a64); E = 0x46;
  CYC(0x6a64, 0x6a65); mem_wr(gb, DE, A);
  CYC(0x6a65, 0x6a68); SET_BC(0x5878);
  CYC(0x6a68, 0x6a6b); createEnergySwirlGoingIn_hook(gb); return;
}

void interactionCodeb5__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6a75, interactionDecCounter1_hook, 0x23cc, 0x6a78);
  if (!(F & FZ)) { CYCT(0x6a78, 0x6a79); ret_effect(gb); return; }
  CYC(0x6a78, 0x6a79);
  CALL_C(0x6a79, interactionIncSubstate_hook, 0x23e5, 0x6a7c);
  CYC(0x6a7c, 0x6a7e); L = 0x46;
  CYC(0x6a7e, 0x6a80); mem_wr(gb, HL, 0x08);
  CYC(0x6a80, 0x6a83); SET_HL(0xcbb3);
  CYC(0x6a83, 0x6a85); mem_wr(gb, HL, 0);
  CYC(0x6a85, 0x6a88); SET_HL(0xcbba);
  CYC(0x6a88, 0x6a8a); mem_wr(gb, HL, 0xff);
  CYC(0x6a8a, 0x6a8b); ret_effect(gb);
}

void interactionCodeb5__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a8b, 0x6a8d); E = 0x46;
  CYC(0x6a8d, 0x6a8e); A = mem_rd(gb, DE);
  CYC(0x6a8e, 0x6a8f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6a8f, 0x6a91); goto after_force_state; }
  CYC(0x6a8f, 0x6a91);
  CALL_C(0x6a91, setLinkForceStateToState08_hook, 0x2aad, 0x6a94);
  CYC(0x6a94, 0x6a97); SET_HL(0xd01a);
  CYC(0x6a97, 0x6a99); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
after_force_state:
  CALL_C(0x6a99, interactionDecCounter1_hook, 0x23cc, 0x6a9c);
  CYC(0x6a9c, 0x6a9f); SET_HL(0xcbb3);
  CYC(0x6a9f, 0x6aa1); B = 1;
  CALL_C(0x6aa1, flashScreen_hook, 0x2d73, 0x6aa4);
  if (F & FZ) { CYCT(0x6aa4, 0x6aa5); ret_effect(gb); return; }
  CYC(0x6aa4, 0x6aa5);
  CALL_C(0x6aa5, interactionIncSubstate_hook, 0x23e5, 0x6aa8);
  CYC(0x6aa8, 0x6aaa); A = 3;
  CYC(0x6aaa, 0x6aae); fadeinFromWhiteWithDelay_hook(gb); return;
}

void interactionCodeb5__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6aad, 0x6ab0); A = mem_rd(gb, 0xc4ab);
  CYC(0x6ab0, 0x6ab1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6ab1, 0x6ab2); ret_effect(gb); return; }
  CYC(0x6ab1, 0x6ab2);
  CYC(0x6ab2, 0x6ab3); alu_xor(gb, A);
  CYC(0x6ab3, 0x6ab6); mem_wr(gb, 0xcc8a, A);
  CYC(0x6ab6, 0x6ab9); mem_wr(gb, 0xcc02, A);
  CYC(0x6ab9, 0x6abc); mem_wr(gb, 0xcbc3, A);
  CYC(0x6abc, 0x6abf); interactionDelete_hook(gb); return;
}

void interactionCodeb5__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a6b, 0x6a6d); E = 0x45;
  CYC(0x6a6d, 0x6a6f); A = mem_rd(gb, DE);
  CYC(0x6a6e, 0x6a6f); push_effect(gb, 0x6a6f);
  switch (final_dungeon_energy_jump_table(gb)) {
    case 0x6a75: interactionCodeb5__substate0_hook(gb); return;
    case 0x6a8b: interactionCodeb5__substate1_hook(gb); return;
    case 0x6aad: interactionCodeb5__substate2_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void interactionCodeb5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a44, 0x6a46); E = 0x44;
  CYC(0x6a46, 0x6a48); A = mem_rd(gb, DE);
  CYC(0x6a47, 0x6a48); push_effect(gb, 0x6a48);
  switch (final_dungeon_energy_jump_table(gb)) {
    case 0x6a4c: interactionCodeb5__state0_hook(gb); return;
    case 0x6a6b: interactionCodeb5__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
