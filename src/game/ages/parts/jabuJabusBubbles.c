#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t jabuJabusBubbles_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void jabuJabusBubbles_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void partCode16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f59, 0x5f5b); E = 0xc4; // Part.state
  CYC(0x5f5b, 0x5f5c); A = mem_rd(gb, DE);
  {
    CYC(0x5f5c, 0x5f5d); push_effect(gb, 0x5f5d);
    uint16_t target = jabuJabusBubbles_jump_table(gb);
    if (target == 0x5f65) goto state0;
    if (target == 0x5fab) goto state1;
    if (target == 0x5fbc) goto state2;
    goto state3;
  }

state0:
  CYC(0x5f65, 0x5f66); H = D;
  CYC(0x5f66, 0x5f67); L = E;
  CYC(0x5f67, 0x5f68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5f68, 0x5f6a); L = 0xc0;
  CYC(0x5f6a, 0x5f6c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5f6c, 0x5f6e); L = 0xcd; // Part.xh
  CYC(0x5f6e, 0x5f6f); A = mem_rd(gb, HL);
  CYC(0x5f6f, 0x5f71); alu_cp(gb, 0x50);
  CYC(0x5f71, 0x5f74); SET_BC(0xff80);
  if (F & FC) { CYCT(0x5f74, 0x5f76); goto L_5f79; } // jr c
  CYC(0x5f74, 0x5f76);
  CYC(0x5f76, 0x5f79); SET_BC(0x0080);

L_5f79:
  CYC(0x5f79, 0x5f7b); L = 0xd2;
  CYC(0x5f7b, 0x5f7c); mem_wr(gb, HL, C);
  CYC(0x5f7c, 0x5f7d); L = alu_inc8(gb, L);
  CYC(0x5f7d, 0x5f7e); mem_wr(gb, HL, B);
  CALL_C(0x5f7e, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5f81);
  CYC(0x5f81, 0x5f82); B = A;
  CYC(0x5f82, 0x5f84); alu_and(gb, 0x07);
  CYC(0x5f84, 0x5f86); E = 0xc6; // Part.counter1
  CYC(0x5f86, 0x5f87); mem_wr(gb, DE, A);
  CYC(0x5f87, 0x5f88); A = B;
  CYC(0x5f88, 0x5f8a); alu_and(gb, 0x18);
  CYC(0x5f8a, 0x5f8c); A = alu_swap(gb, A);
  CYC(0x5f8c, 0x5f8d); alu_rlca(gb);
  CYC(0x5f8d, 0x5f90); SET_HL(0x5fa7); // table_5fa7
  CYC(0x5f90, 0x5f91); jabuJabusBubbles_addAToHl_from_rst(gb, 0x5f91);
  CYC(0x5f91, 0x5f93); E = 0xd0; // Part.speed
  CYC(0x5f93, 0x5f94); A = mem_rd(gb, HL);
  CYC(0x5f94, 0x5f95); mem_wr(gb, DE, A);
  CYC(0x5f95, 0x5f96); A = B;
  CYC(0x5f96, 0x5f98); alu_and(gb, 0xe0);
  CYC(0x5f98, 0x5f9a); A = alu_swap(gb, A);
  CYC(0x5f9a, 0x5f9b); alu_add(gb, A);
  CYC(0x5f9b, 0x5f9d); E = 0xc7; // Part.counter2
  CYC(0x5f9d, 0x5f9e); mem_wr(gb, DE, A);
  CYC(0x5f9e, 0x5fa0); E = 0xc2; // Part.subid
  CYC(0x5fa0, 0x5fa1); A = mem_rd(gb, DE);
  CALL_C(0x5fa1, partSetAnimation_hook, 0x2988, 0x5fa4);
  CYC(0x5fa4, 0x5fa7); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x5fab, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5fae);
  if (!(F & FZ)) { CYCT(0x5fae, 0x5fb0); goto L_5fb5; } // jr nz
  CYC(0x5fae, 0x5fb0);
  CYC(0x5fb0, 0x5fb1); L = alu_inc8(gb, L);
  CYC(0x5fb1, 0x5fb2); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5fb2, 0x5fb3); mem_wr(gb, HL, A);
  CYC(0x5fb3, 0x5fb4); L = E;
  CYC(0x5fb4, 0x5fb5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

L_5fb5:
  CYC(0x5fb5, 0x5fb7); L = 0xda; // Part.visible
  CYC(0x5fb7, 0x5fb8); A = mem_rd(gb, HL);
  CYC(0x5fb8, 0x5fba); alu_xor(gb, 0x80);
  CYC(0x5fba, 0x5fbb); mem_wr(gb, HL, A);
  RET(0x5fbb); return; // ret

state2:
  CALL_C(0x5fbc, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5fbf);
  if (!(F & FZ)) { CYCT(0x5fbf, 0x5fc1); goto L_5fc3; } // jr nz
  CYC(0x5fbf, 0x5fc1);
  CYC(0x5fc1, 0x5fc2); L = E;
  CYC(0x5fc2, 0x5fc3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

L_5fc3:
  CYC(0x5fc3, 0x5fc5); L = 0xd2;
  CYC(0x5fc5, 0x5fc6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fc6, 0x5fc7); B = mem_rd(gb, HL);
  CYC(0x5fc7, 0x5fc8); C = A;
  CYC(0x5fc8, 0x5fca); L = 0xcc;
  CYC(0x5fca, 0x5fcb); E = L;
  CYC(0x5fcb, 0x5fcc); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5fcc, 0x5fcd); H = mem_rd(gb, HL);
  CYC(0x5fcd, 0x5fce); L = A;
  CYC(0x5fce, 0x5fcf); alu_add_hl(gb, BC);
  CYC(0x5fcf, 0x5fd0); A = L;
  CYC(0x5fd0, 0x5fd1); mem_wr(gb, DE, A);
  CYC(0x5fd1, 0x5fd2); E = alu_inc8(gb, E);
  CYC(0x5fd2, 0x5fd3); A = H;
  CYC(0x5fd3, 0x5fd4); mem_wr(gb, DE, A);

state3:
  CALL_C(0x5fd4, objectApplySpeed_hook, 0x201d, 0x5fd7);
  CYC(0x5fd7, 0x5fd9); E = 0xcb; // Part.yh
  CYC(0x5fd9, 0x5fda); A = mem_rd(gb, DE);
  CYC(0x5fda, 0x5fdc); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(0x5fdc, 0x5fdf); partDelete_hook(gb); return; } // jp nc
  CYC(0x5fdc, 0x5fdf);
  CYC(0x5fdf, 0x5fe0); H = D;
  CYC(0x5fe0, 0x5fe2); L = 0xda; // Part.visible
  CYC(0x5fe2, 0x5fe3); A = mem_rd(gb, HL);
  CYC(0x5fe3, 0x5fe5); alu_xor(gb, 0x80);
  CYC(0x5fe5, 0x5fe6); mem_wr(gb, HL, A);
  CYC(0x5fe6, 0x5fe8); L = 0xc7; // Part.counter2
  CYC(0x5fe8, 0x5fe9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5fe9, 0x5feb); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x5feb); return; } // ret nz
  CYC(0x5feb, 0x5fec);
  CYC(0x5fec, 0x5fee); L = 0xd0; // Part.speed
  CYC(0x5fee, 0x5fef); A = mem_rd(gb, HL);
  CYC(0x5fef, 0x5ff1); alu_sub(gb, 0x05);
  CYC(0x5ff1, 0x5ff3); alu_cp(gb, 0x13);
  if (F & FC) { RET_TAKEN(0x5ff3); return; } // ret c
  CYC(0x5ff3, 0x5ff4);
  CYC(0x5ff4, 0x5ff5); mem_wr(gb, HL, A);
  RET(0x5ff5); return; // ret
}
