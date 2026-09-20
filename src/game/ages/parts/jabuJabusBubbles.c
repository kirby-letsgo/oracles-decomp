#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode16), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode16), (from), (to), true)

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
  BASE(partCode16);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = jabuJabusBubbles_jump_table(gb);
    if (target == b_+12) goto state0;
    if (target == b_+82) goto state1;
    if (target == b_+99) goto state2;
    goto state3;
  }

state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = 0xc0;
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+19, b_+21); L = 0xcd; // Part.xh
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); alu_cp(gb, 0x50);
  CYC(b_+24, b_+27); SET_BC(hOamFunc);
  if (F & FC) { CYCT(b_+27, b_+29); goto L_5f79; } // jr c
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC(0x0080);

L_5f79:
  CYC(b_+32, b_+34); L = 0xd2;
  CYC(b_+34, b_+35); mem_wr(gb, HL, C);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); mem_wr(gb, HL, B);
  CALL_C(b_+37, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+40);
  CYC(b_+40, b_+41); B = A;
  CYC(b_+41, b_+43); alu_and(gb, 0x07);
  CYC(b_+43, b_+45); E = 0xc6; // Part.counter1
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); A = B;
  CYC(b_+47, b_+49); alu_and(gb, 0x18);
  CYC(b_+49, b_+51); A = alu_swap(gb, A);
  CYC(b_+51, b_+52); alu_rlca(gb);
  CYC(b_+52, b_+55); SET_HL(b_+78); // table_5fa7
  CYC(b_+55, b_+56); jabuJabusBubbles_addAToHl_from_rst(gb, b_+56);
  CYC(b_+56, b_+58); E = 0xd0; // Part.speed
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+61); A = B;
  CYC(b_+61, b_+63); alu_and(gb, 0xe0);
  CYC(b_+63, b_+65); A = alu_swap(gb, A);
  CYC(b_+65, b_+66); alu_add(gb, A);
  CYC(b_+66, b_+68); E = 0xc7; // Part.counter2
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+71); E = 0xc2; // Part.subid
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CALL_C(b_+72, partSetAnimation_hook, SYM(partSetAnimation), b_+75);
  CYC(b_+75, b_+78); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+82, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+85);
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto L_5fb5; } // jr nz
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); L = alu_inc8(gb, L);
  CYC(b_+88, b_+89); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+89, b_+90); mem_wr(gb, HL, A);
  CYC(b_+90, b_+91); L = E;
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

L_5fb5:
  CYC(b_+92, b_+94); L = 0xda; // Part.visible
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+97); alu_xor(gb, 0x80);
  CYC(b_+97, b_+98); mem_wr(gb, HL, A);
  RET(b_+98); return; // ret

state2:
  CALL_C(b_+99, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+104); goto L_5fc3; } // jr nz
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+105); L = E;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

L_5fc3:
  CYC(b_+106, b_+108); L = 0xd2;
  CYC(b_+108, b_+109); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+109, b_+110); B = mem_rd(gb, HL);
  CYC(b_+110, b_+111); C = A;
  CYC(b_+111, b_+113); L = 0xcc;
  CYC(b_+113, b_+114); E = L;
  CYC(b_+114, b_+115); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+115, b_+116); H = mem_rd(gb, HL);
  CYC(b_+116, b_+117); L = A;
  CYC(b_+117, b_+118); alu_add_hl(gb, BC);
  CYC(b_+118, b_+119); A = L;
  CYC(b_+119, b_+120); mem_wr(gb, DE, A);
  CYC(b_+120, b_+121); E = alu_inc8(gb, E);
  CYC(b_+121, b_+122); A = H;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);

state3:
  CALL_C(b_+123, objectApplySpeed_hook, SYM(objectApplySpeed), b_+126);
  CYC(b_+126, b_+128); E = 0xcb; // Part.yh
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+131); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+131, b_+134); partDelete_hook(gb); return; } // jp nc
  CYC(b_+131, b_+134);
  CYC(b_+134, b_+135); H = D;
  CYC(b_+135, b_+137); L = 0xda; // Part.visible
  CYC(b_+137, b_+138); A = mem_rd(gb, HL);
  CYC(b_+138, b_+140); alu_xor(gb, 0x80);
  CYC(b_+140, b_+141); mem_wr(gb, HL, A);
  CYC(b_+141, b_+143); L = 0xc7; // Part.counter2
  CYC(b_+143, b_+144); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+144, b_+146); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+146); return; } // ret nz
  CYC(b_+146, b_+147);
  CYC(b_+147, b_+149); L = 0xd0; // Part.speed
  CYC(b_+149, b_+150); A = mem_rd(gb, HL);
  CYC(b_+150, b_+152); alu_sub(gb, 0x05);
  CYC(b_+152, b_+154); alu_cp(gb, 0x13);
  if (F & FC) { RET_TAKEN(b_+154); return; } // ret c
  CYC(b_+154, b_+155);
  CYC(b_+155, b_+156); mem_wr(gb, HL, A);
  RET(b_+156); return; // ret
}
