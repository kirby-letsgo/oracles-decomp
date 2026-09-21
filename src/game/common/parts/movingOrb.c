#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCode0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0x01); // PARTSTATUS_JUST_HIT
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto normalStatus; } // jr nz
  CYC(b_+2, b_+4);

  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); // [oamFlags]
  CYC(b_+9, b_+11); L = 0xc3; // Part.var03
  CYC(b_+11, b_+14); A = mem_rd(gb, wToggleBlocksState); // wToggleBlocksState
  CYC(b_+14, b_+15); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+18); mem_wr(gb, wToggleBlocksState, A); // wToggleBlocksState
  CYC(b_+18, b_+20); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto L_47ed; } // jr nz
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); A = 0x02;

L_47ed:
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+30); A = 0x7e; // SND_SWITCH
  CALL_C(b_+30, playSound_b00_hook, SYM(playSound_b00), b_+33);

normalStatus:
  CYC(b_+33, b_+35); E = 0xc4; // Part.state
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_sub(gb, 0x08);
  if (F & FC) { CYCT(b_+38, b_+40); goto state0To7; } // jr c
  CYC(b_+38, b_+40);
  {
    CYC(b_+40, b_+41); push_effect(gb, b_+41);
    uint16_t target = movingOrb_jump_table(gb);
    if (target == b_+77) goto state8_up;
    if (target == b_+89) goto state9_right;
    if (target == b_+101) goto stateA_down;
    if (target == b_+113) goto stateB_left;
    goto stateC_waiting;
  }

state0To7:
  CYC(b_+51, b_+54); SET_HL(GV((SYM(partCode38__state2) + 22), 0x6b30)); // bank0e.orbMovementScript
  CALL_C(b_+54, objectLoadMovementScript_hook, SYM(objectLoadMovementScript), b_+57);
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = 0xc3; // Part.var03
  CYC(b_+60, b_+62); B = 0x01;
  CYC(b_+62, b_+65); A = mem_rd(gb, wToggleBlocksState); // wToggleBlocksState
  CYC(b_+65, b_+66); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+66, b_+68); goto L_4818; } // jr z
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+69); B = alu_inc8(gb, B);

L_4818:
  CYC(b_+69, b_+70); A = B;
  CYC(b_+70, b_+72); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+72, b_+73); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+73, b_+74); mem_wr(gb, HL, A);
  CYC(b_+74, b_+77); TAIL(objectSetVisible82); // jp

state8_up:
  CYC(b_+77, b_+78); H = D;
  CYC(b_+78, b_+80); E = 0xf2; // Part.var32
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+83); L = 0xcb; // Part.yh
  CYC(b_+83, b_+84); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+84, b_+87); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+84, b_+87);
  CYC(b_+87, b_+89); goto runMovementScript; // jr

state9_right:
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); E = 0xcd; // Part.xh
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+95); L = 0xf3; // Part.var33
  CYC(b_+95, b_+96); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+96, b_+99); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+96, b_+99);
  CYC(b_+99, b_+101); goto runMovementScript; // jr

stateA_down:
  CYC(b_+101, b_+102); H = D;
  CYC(b_+102, b_+104); E = 0xcb; // Part.yh
  CYC(b_+104, b_+105); A = mem_rd(gb, DE);
  CYC(b_+105, b_+107); L = 0xf2; // Part.var32
  CYC(b_+107, b_+108); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+108, b_+111); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+108, b_+111);
  CYC(b_+111, b_+113); goto runMovementScript; // jr

stateB_left:
  CYC(b_+113, b_+114); H = D;
  CYC(b_+114, b_+116); E = 0xf3; // Part.var33
  CYC(b_+116, b_+117); A = mem_rd(gb, DE);
  CYC(b_+117, b_+119); L = 0xcd; // Part.xh
  CYC(b_+119, b_+120); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+120, b_+123); objectApplySpeed_hook(gb); return; } // jp c
  CYC(b_+120, b_+123);

runMovementScript:
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); mem_wr(gb, HL, A);
  CYC(b_+125, b_+128); TAIL(objectRunMovementScript); // jp

stateC_waiting:
  CYC(b_+128, b_+129); H = D;
  CYC(b_+129, b_+131); L = 0xc6; // Part.counter1
  CYC(b_+131, b_+132); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+132); return; } // ret nz
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+136); TAIL(objectRunMovementScript); // jp
}
