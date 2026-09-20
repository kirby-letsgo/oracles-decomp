#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t partCode3e_jump_table(GB *gb) {
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

// PART_3e: tracks the enemy index of every loaded Ambi Guard in Part.var30-3f
void partCode3e_hook(GB *gb) {
  BASE(partCode3e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = partCode3e_jump_table(gb);
    if (target == b_+35) goto state1;
    if (target == b_+60) goto state2;
    if (target == b_+94) goto state3;
    goto state0;
  }

state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); E = 0xf0; // Part.var30
  CYC(b_+17, b_+20); SET_HL((w1ReservedInteraction0_var3f + 2)); // FIRST_ENEMY_INDEX, Enemy.id

L_7101:
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x54); // ENEMY_AMBI_GUARD
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_7109; } // jr nz
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = H;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);

L_7109:
  CYC(b_+28, b_+29); H = alu_inc8(gb, H);
  CYC(b_+29, b_+30); A = H;
  CYC(b_+30, b_+32); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+32, b_+34); goto L_7101; } // jr c
  CYC(b_+32, b_+34);
  RET(b_+34); return; // ret

state1:
  CYC(b_+35, b_+38); SET_HL(w4Filler5);

L_7113:
  CYC(b_+38, b_+40); L = 0x24; // Object.collisionType
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+43); alu_cp(gb, 0x98);
  if (F & FZ) { CYCT(b_+43, b_+45); goto L_7121; } // jr z
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+46); H = alu_inc8(gb, H);
  CYC(b_+46, b_+47); A = H;
  CYC(b_+47, b_+49); alu_cp(gb, 0xdc);
  if (F & FC) { CYCT(b_+49, b_+51); goto L_7113; } // jr c
  CYC(b_+49, b_+51);
  RET(b_+51); return; // ret

L_7121:
  CYC(b_+52, b_+54); A = 0x02;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = 0xd9; // Part.relatedObj2+1
  CYC(b_+57, b_+58); A = H;
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  RET(b_+59); return; // ret

state2:
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = 0xc4; // Part.state
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+64, b_+66); L = 0xc6; // Part.counter1
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x3c);
  CYC(b_+68, b_+70); L = 0xd9; // Part.relatedObj2+1
  CYC(b_+70, b_+71); B = mem_rd(gb, HL);
  CYC(b_+71, b_+73); E = 0xf0; // Part.var30

L_7136:
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+75); return; } // ret z
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+77); H = A;
  CYC(b_+77, b_+79); L = 0xba; // Enemy.var3a
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0xff);
  CYC(b_+81, b_+83); L = 0x98; // Enemy.relatedObj2
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x00);
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);
  CYC(b_+86, b_+87); mem_wr(gb, HL, B);
  CYC(b_+87, b_+88); E = alu_inc8(gb, E);
  CYC(b_+88, b_+89); A = E;
  CYC(b_+89, b_+91); alu_cp(gb, 0xf4); // Part.var34
  if (F & FC) { CYCT(b_+91, b_+93); goto L_7136; } // jr c
  CYC(b_+91, b_+93);
  RET(b_+93); return; // ret

state3:
  CALL_C(b_+94, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+97);
  if (!(F & FZ)) { RET_TAKEN(b_+97); return; } // ret nz
  CYC(b_+97, b_+98);
  CYC(b_+98, b_+99); L = E;
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x01);
  RET(b_+101); return; // ret
}
