#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t wooden_tunnel_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false); return HL;
}

void wooden_tunnel_set_solidity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4040, objectGetShortPosition_hook, 0x2096, 0x4043);
  CYC(0x4043, 0x4045); H = 0xce;
  CYC(0x4045, 0x4046); L = A;
  CYC(0x4046, 0x4047); mem_wr(gb, HL, C);
  CYC(0x4047, 0x4048); ret_effect(gb);
}

void interactionCode98__setSolidity_hook(GB *gb) { wooden_tunnel_set_solidity_hook(gb); }
void interactionCode98__makeSolid_hook(GB *gb) {
  CYC(0x403e, 0x4040); C = 0x0f;
  wooden_tunnel_set_solidity_hook(gb);
}

void interactionCode98__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4008, 0x400a); A = 1;
  CYC(0x400a, 0x400b); mem_wr(gb, DE, A);
  CALL_C(0x400b, interactionInitGraphics_hook, 0x15fb, 0x400e);
  CYC(0x400e, 0x4010); A = 7;
  CALL_C(0x4010, objectSetCollideRadius_hook, 0x24a1, 0x4013);
  CYC(0x4013, 0x4015); E = 0x42;
  CYC(0x4015, 0x4016); A = mem_rd(gb, DE);
  CYC(0x4016, 0x4018);
  CALL_C(0x4018, interactionSetAnimation_hook, 0x262e, 0x401b);
  CYC(0x401b, 0x401e); objectSetVisible81_hook(gb);
}

void interactionCode98__state1_hook(GB *gb) {
  CYC(0x401e, 0x4021); A = mem_rd(gb, wLinkGrabState);
  CYC(0x4021, 0x4022); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4022, 0x4024); interactionCode98__makeSolid_hook(gb); return; }
  CYC(0x4022, 0x4024); CYC(0x4024, 0x4027); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x4027, 0x4029); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x4029, 0x402b); interactionCode98__makeSolid_hook(gb); return; }
  CYC(0x4029, 0x402b); CYC(0x402b, 0x402e); A = mem_rd(gb, w1ReservedItemC_enabled);
  CYC(0x402e, 0x402f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x402f, 0x4031); interactionCode98__makeSolid_hook(gb); return; }
  CYC(0x402f, 0x4031); CYC(0x4031, 0x4033); E = 0x42;
  CYC(0x4033, 0x4034); A = mem_rd(gb, DE);
  CYC(0x4034, 0x4036); alu_cp(gb, 2);
  CYC(0x4036, 0x4038); C = 0x11;
  if (F & FC) { CYCT(0x4038, 0x403a); wooden_tunnel_set_solidity_hook(gb); return; }
  CYC(0x4038, 0x403a); CYC(0x403a, 0x403c); C = 0x19;
  CYC(0x403c, 0x403e); wooden_tunnel_set_solidity_hook(gb);
}

void interactionCode98_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4000, 0x4002); E = 0x44;
  CYC(0x4002, 0x4003); A = mem_rd(gb, DE);
  CYC(0x4003, 0x4004); push_effect(gb, 0x4004);
  switch (wooden_tunnel_jump_table(gb)) {
    case 0x4008: interactionCode98__state0_hook(gb); return;
    case 0x401e: interactionCode98__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
