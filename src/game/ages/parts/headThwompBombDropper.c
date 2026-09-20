#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static void headThwompBombDropper_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_HEAD_THWOMP_BOMB_DROPPER
void partCode40_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72dd, 0x72df); A = 0x01; // Object.id
  CALL_C(0x72df, objectGetRelatedObject1Var_hook, 0x2160, 0x72e2);
  CYC(0x72e2, 0x72e3); A = mem_rd(gb, HL);
  CYC(0x72e3, 0x72e5); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x72e5, 0x72e8); partDelete_hook(gb); return; } // jp nz
  CYC(0x72e5, 0x72e8);
  CYC(0x72e8, 0x72ea); E = 0xc4; // Part.state
  CYC(0x72ea, 0x72eb); A = mem_rd(gb, DE);
  CYC(0x72eb, 0x72ec); alu_or(gb, A);
  if (F & FZ) { CYCT(0x72ec, 0x72ee); goto state0; } // jr z
  CYC(0x72ec, 0x72ee);
  CYC(0x72ee, 0x72f0); A = 0x20;
  CALL_C(0x72f0, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x72f3);
  if (F & FC) { CYCT(0x72f3, 0x72f6); partDelete_hook(gb); return; } // jp c
  CYC(0x72f3, 0x72f6);
  CALL_C(0x72f6, objectApplySpeed_hook, 0x201d, 0x72f9);
  CYC(0x72f9, 0x72fb); A = 0x00; // Object.state
  CALL_C(0x72fb, objectGetRelatedObject1Var_hook, 0x2160, 0x72fe);
  CYC(0x72fe, 0x7301); objectCopyPosition_hook(gb); return; // jp

state0:
  CYC(0x7301, 0x7302); H = D;
  CYC(0x7302, 0x7303); L = E;
  CYC(0x7303, 0x7304); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x7304, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7307);
  CYC(0x7307, 0x7308); B = A;
  CYC(0x7308, 0x730a); alu_and(gb, 0x03);
  CYC(0x730a, 0x730d); SET_HL(0x732c); // table_732c (speedVals)
  CYC(0x730d, 0x730e); headThwompBombDropper_addAToHl_from_rst(gb, 0x730e);
  CYC(0x730e, 0x7310); E = 0xd0; // Part.speed
  CYC(0x7310, 0x7311); A = mem_rd(gb, HL);
  CYC(0x7311, 0x7312); mem_wr(gb, DE, A);
  CYC(0x7312, 0x7313); A = B;
  CYC(0x7313, 0x7315); alu_and(gb, 0x60);
  CYC(0x7315, 0x7317); A = alu_swap(gb, A);
  CYC(0x7317, 0x731a); SET_HL(0x7330); // table_7330 (speedZVals)
  CYC(0x731a, 0x731b); headThwompBombDropper_addAToHl_from_rst(gb, 0x731b);
  CYC(0x731b, 0x731d); E = 0xd4; // Part.speedZ
  CYC(0x731d, 0x731e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x731e, 0x731f); mem_wr(gb, DE, A);
  CYC(0x731f, 0x7320); E = alu_inc8(gb, E);
  CYC(0x7320, 0x7321); A = mem_rd(gb, HL);
  CYC(0x7321, 0x7322); mem_wr(gb, DE, A);
  CYC(0x7322, 0x7324); A = mem_rd(gb, 0xff95); // hRng2
  CYC(0x7324, 0x7326); alu_and(gb, 0x10);
  CYC(0x7326, 0x7328); alu_add(gb, 0x08);
  CYC(0x7328, 0x732a); E = 0xc9; // Part.angle
  CYC(0x732a, 0x732b); mem_wr(gb, DE, A);
  RET(0x732b); return; // ret
}
