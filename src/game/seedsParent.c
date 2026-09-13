#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t seeds_parent_jump_table(GB *gb) {
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

static void seeds_parent_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    ret_effect(gb);
  }
}

void parentItemGenericState1_hook(GB *gb) {
  CYC(0x4f95, 0x4f97); E = 0x21;
  CYC(0x4f97, 0x4f98); A = mem_rd(gb, DE);
  CYC(0x4f98, 0x4f99); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x4f99, 0x4f9c); specialObjectAnimate_optimized_hook(gb); }
  else { CYC(0x4f99, 0x4f9c); CYC(0x4f9c, 0x4f9f); clearParentItem_hook(gb); }
}

void clearSelfIfNoSeeds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f82, 0x4f85); SET_HL(wSatchelSelectedSeeds);
  CYC(0x4f85, 0x4f86); push_effect(gb, 0x4f86); seeds_parent_add_a_to_hl(gb);
  CYC(0x4f86, 0x4f87); A = mem_rd(gb, HL);
  CYC(0x4f87, 0x4f88); B = A;
  CYC(0x4f88, 0x4f8a); B |= 0x20;
  CYC(0x4f8a, 0x4f8d); SET_HL(wNumEmberSeeds);
  CYC(0x4f8d, 0x4f8e); push_effect(gb, 0x4f8e); seeds_parent_add_a_to_hl(gb);
  CYC(0x4f8e, 0x4f8f); A = mem_rd(gb, HL);
  CYC(0x4f8f, 0x4f90); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f90, 0x4f91); ret_effect(gb); return; }
  CYC(0x4f90, 0x4f91);
  CYC(0x4f91, 0x4f92); SET_HL(pop_effect(gb));
  CYC(0x4f92, 0x4f95); clearParentItem_hook(gb);
}

void parentItemCode_satchel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f2d, 0x4f2f); E = 0x04;
  CYC(0x4f2f, 0x4f30); A = mem_rd(gb, DE);
  CYC(0x4f30, 0x4f31); push_effect(gb, 0x4f31);
  switch (seeds_parent_jump_table(gb)) {
    case 0x4f35: goto state0;
    case 0x4f95: parentItemGenericState1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4f35, 0x4f38); A = W8(w1Companion_id);
  CYC(0x4f38, 0x4f3a); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(0x4f3a, 0x4f3d); goto clear; }
  CYC(0x4f3a, 0x4f3d);
  CALL_C(0x4f3d, isLinkUnderwater_hook, 0x54d2, 0x4f40);
  if (!(F & FZ)) { CYCT(0x4f40, 0x4f43); goto clear; }
  CYC(0x4f40, 0x4f43);
  CYC(0x4f43, 0x4f46); A = W8(wLinkSwimmingState);
  CYC(0x4f46, 0x4f47); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f47, 0x4f4a); goto clear; }
  CYC(0x4f47, 0x4f4a);
  CALL_C(0x4f4a, clearSelfIfNoSeeds_hook, 0x4f82, 0x4f4d);
  CYC(0x4f4d, 0x4f4e); A = B;
  CYC(0x4f4e, 0x4f50); alu_cp(gb, 0x22);
  if (F & FZ) { CYCT(0x4f50, 0x4f52); goto pegasus; }
  CYC(0x4f50, 0x4f52);
  CYC(0x4f52, 0x4f53); push_effect(gb, BC);
  CALL_C(0x4f53, parentItemLoadAnimationAndIncState_hook, 0x5378, 0x4f56);
  CYC(0x4f56, 0x4f57); SET_BC(pop_effect(gb));
  CYC(0x4f57, 0x4f58); push_effect(gb, BC);
  CYC(0x4f58, 0x4f5a); C = 0x00;
  CYC(0x4f5a, 0x4f5c); E = 0x01;
  CALL_C(0x4f5c, itemCreateChildWithID_hook, 0x53e3, 0x4f5f);
  CYC(0x4f5f, 0x4f60); SET_BC(pop_effect(gb));
  if (F & FC) { CYCT(0x4f60, 0x4f63); goto clear; }
  CYC(0x4f60, 0x4f63);
  CYC(0x4f63, 0x4f64); A = B;
  CYC(0x4f64, 0x4f67); decNumActiveSeeds_hook(gb); return;

pegasus:
  CYC(0x4f67, 0x4f6a); SET_HL(wPegasusSeedCounter);
  CYC(0x4f6a, 0x4f6b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f6b, 0x4f6c); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4f6c, 0x4f6e); goto clear; }
  CYC(0x4f6c, 0x4f6e);
  CYC(0x4f6e, 0x4f70); A = 0x03;
  CYC(0x4f70, 0x4f71); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4f71, 0x4f73); mem_wr(gb, HL, 0xc0);
  CYC(0x4f73, 0x4f74); A = B;
  CALL_C(0x4f74, decNumActiveSeeds_hook, 0x17bb, 0x4f77);
  CYC(0x4f77, 0x4f7a); SET_HL(w1ReservedItemF);
  CYC(0x4f7a, 0x4f7c); A = 0x03;
  CYC(0x4f7c, 0x4f7d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4f7d, 0x4f7f); mem_wr(gb, HL, 0x1a);

clear:
  CYC(0x4f7f, 0x4f82); clearParentItem_hook(gb);
}
