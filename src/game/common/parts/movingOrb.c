#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t movingOrb_jump_table(GB *gb) {
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

void partCode0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x47d3, 0x47d5); alu_cp(gb, 0x01); // PARTSTATUS_JUST_HIT
  if (!(F & FZ)) { CYCT(0x47d5, 0x47d7); goto normalStatus; } // jr nz
  CYC(0x47d5, 0x47d7);

  CYC(0x47d7, 0x47d8); H = D;
  CYC(0x47d8, 0x47da); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x47da, 0x47db); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x47db, 0x47dc); mem_wr(gb, HL, A); // [oamFlags]
  CYC(0x47dc, 0x47de); L = 0xc3; // Part.var03
  CYC(0x47de, 0x47e1); A = mem_rd(gb, 0xcdd2); // wToggleBlocksState
  CYC(0x47e1, 0x47e2); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x47e2, 0x47e5); mem_wr(gb, 0xcdd2, A); // wToggleBlocksState
  CYC(0x47e5, 0x47e7); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x47e7, 0x47e8); A = mem_rd(gb, HL);
  CYC(0x47e8, 0x47e9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x47e9, 0x47eb); goto L_47ed; } // jr nz
  CYC(0x47e9, 0x47eb);
  CYC(0x47eb, 0x47ed); A = 0x02;

L_47ed:
  CYC(0x47ed, 0x47ee); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x47ee, 0x47ef); mem_wr(gb, HL, A);
  CYC(0x47ef, 0x47f1); A = 0x7e; // SND_SWITCH
  CALL_C(0x47f1, playSound_b00_hook, 0x0c98, 0x47f4);

normalStatus:
  CYC(0x47f4, 0x47f6); E = 0xc4; // Part.state
  CYC(0x47f6, 0x47f7); A = mem_rd(gb, DE);
  CYC(0x47f7, 0x47f9); alu_sub(gb, 0x08);
  if (F & FC) { CYCT(0x47f9, 0x47fb); goto state0To7; } // jr c
  CYC(0x47f9, 0x47fb);
  {
    CYC(0x47fb, 0x47fc); push_effect(gb, 0x47fc);
    uint16_t target = movingOrb_jump_table(gb);
    if (target == 0x4820) goto state8_up;
    if (target == 0x482c) goto state9_right;
    if (target == 0x4838) goto stateA_down;
    if (target == 0x4844) goto stateB_left;
    goto stateC_waiting;
  }

state0To7:
  CYC(0x4806, 0x4809); SET_HL(0x6b22); // bank0e.orbMovementScript
  CALL_C(0x4809, objectLoadMovementScript_hook, 0x3035, 0x480c);
  CYC(0x480c, 0x480d); H = D;
  CYC(0x480d, 0x480f); L = 0xc3; // Part.var03
  CYC(0x480f, 0x4811); B = 0x01;
  CYC(0x4811, 0x4814); A = mem_rd(gb, 0xcdd2); // wToggleBlocksState
  CYC(0x4814, 0x4815); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4815, 0x4817); goto L_4818; } // jr z
  CYC(0x4815, 0x4817);
  CYC(0x4817, 0x4818); B = alu_inc8(gb, B);

L_4818:
  CYC(0x4818, 0x4819); A = B;
  CYC(0x4819, 0x481b); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x481b, 0x481c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x481c, 0x481d); mem_wr(gb, HL, A);
  CYC(0x481d, 0x4820); objectSetVisible82_hook(gb); return; // jp

state8_up:
  CYC(0x4820, 0x4821); H = D;
  CYC(0x4821, 0x4823); E = 0xf2; // Part.var32
  CYC(0x4823, 0x4824); A = mem_rd(gb, DE);
  CYC(0x4824, 0x4826); L = 0xcb; // Part.yh
  CYC(0x4826, 0x4827); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x4827, 0x482a); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x4827, 0x482a);
  CYC(0x482a, 0x482c); goto runMovementScript; // jr

state9_right:
  CYC(0x482c, 0x482d); H = D;
  CYC(0x482d, 0x482f); E = 0xcd; // Part.xh
  CYC(0x482f, 0x4830); A = mem_rd(gb, DE);
  CYC(0x4830, 0x4832); L = 0xf3; // Part.var33
  CYC(0x4832, 0x4833); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x4833, 0x4836); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x4833, 0x4836);
  CYC(0x4836, 0x4838); goto runMovementScript; // jr

stateA_down:
  CYC(0x4838, 0x4839); H = D;
  CYC(0x4839, 0x483b); E = 0xcb; // Part.yh
  CYC(0x483b, 0x483c); A = mem_rd(gb, DE);
  CYC(0x483c, 0x483e); L = 0xf2; // Part.var32
  CYC(0x483e, 0x483f); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x483f, 0x4842); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x483f, 0x4842);
  CYC(0x4842, 0x4844); goto runMovementScript; // jr

stateB_left:
  CYC(0x4844, 0x4845); H = D;
  CYC(0x4845, 0x4847); E = 0xf3; // Part.var33
  CYC(0x4847, 0x4848); A = mem_rd(gb, DE);
  CYC(0x4848, 0x484a); L = 0xcd; // Part.xh
  CYC(0x484a, 0x484b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x484b, 0x484e); objectApplySpeed_hook(gb); return; } // jp c
  CYC(0x484b, 0x484e);

runMovementScript:
  CYC(0x484e, 0x484f); A = mem_rd(gb, DE);
  CYC(0x484f, 0x4850); mem_wr(gb, HL, A);
  CYC(0x4850, 0x4853); objectRunMovementScript_hook(gb); return; // jp

stateC_waiting:
  CYC(0x4853, 0x4854); H = D;
  CYC(0x4854, 0x4856); L = 0xc6; // Part.counter1
  CYC(0x4856, 0x4857); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x4857); return; } // ret nz
  CYC(0x4857, 0x4858);
  CYC(0x4858, 0x485b); objectRunMovementScript_hook(gb); return; // jp
}
