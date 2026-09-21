#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t partCode3b_jump_table(GB *gb) {
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

static void partCode3b_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_3b: head thwomp (purple face) boulder
void partCode3b_hook(GB *gb) {
  BASE(partCode3b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = 0xc4; // Part.state
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto subid0; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); goto subid1; // jp

subid0:
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  {
    uint16_t target = partCode3b_jump_table(gb);
    if (target == b_+56) goto subid0_state1;
    if (target == b_+93) goto state2;
    goto subid0_state0;
  }

subid0_state0:
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+24); L = 0xd5; // Part.speedZ+1
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x02);
  CYC(b_+26, b_+28); L = 0xcb; // Part.yh
  CYC(b_+28, b_+30); A = mem_rd(gb, hCameraY); // hCameraY
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto after_random; } // jr nz
  CYC(b_+34, b_+36);
  CALL_C(b_+36, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+39);
  CYC(b_+39, b_+41); alu_and(gb, 0x7c);
  CYC(b_+41, b_+42); B = A;
  CYC(b_+42, b_+44); A = mem_rd(gb, hCameraX); // hCameraX
  CYC(b_+44, b_+45); alu_add(gb, B);
  CYC(b_+45, b_+47); E = 0xcd; // Part.xh
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);

after_random:
  CALL_C(b_+48, objectSetVisible82_hook, SYM(objectSetVisible82), b_+51);
  CYC(b_+51, b_+53); A = 0x59; // SND_FALLINHOLE
  CYC(b_+53, b_+56); TAIL(playSound_b00); // jp

subid0_state1:
  CYC(b_+56, b_+58); A = 0x20;
  CALL_C(b_+58, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+61);
  if (F & FC) { CYCT(b_+61, b_+63); goto func_6ebd; } // jr c
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+66); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+66, b_+68); goto animate; } // jr c
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+71); TAIL(partDelete); // jp

func_6ebd:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = 0xc4; // Part.state
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+77, b_+79); A = 0x0b;
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x02);
  CYC(b_+83, b_+85); A = 0x01;
  CALL_C(b_+85, partSetAnimation_hook, SYM(partSetAnimation), b_+88);
  CYC(b_+88, b_+90); A = 0xa5; // SND_BREAK_ROCK
  CYC(b_+90, b_+93); TAIL(playSound_b00); // jp

state2:
  CYC(b_+93, b_+95); E = 0xe1; // Part.animParameter
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+98); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+98, b_+101); partDelete_hook(gb); return; } // jp nz
  CYC(b_+98, b_+101);
  CYC(b_+101, b_+104); SET_HL(b_+115); // table_6ee9
  CYC(b_+104, b_+105); partCode3b_addAToHl_from_rst(gb, b_+105);
  CYC(b_+105, b_+107); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+107, b_+108); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+112); mem_wr(gb, DE, A);

animate:
  CYC(b_+112, b_+115); TAIL(partAnimate); // jp

subid1:
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+127); push_effect(gb, b_+127);
  {
    uint16_t target = partCode3b_jump_table(gb);
    if (target == b_+173) goto subid1_state1;
    if (target == b_+93) goto state2;
    goto subid1_state0;
  }

subid1_state0:
  CYC(b_+133, b_+134); H = D;
  CYC(b_+134, b_+135); L = E;
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+138); L = 0xd5; // Part.speedZ+1
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x02);
  CYC(b_+140, b_+142); L = 0xcb; // Part.yh
  CYC(b_+142, b_+143); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+143, b_+144); L = alu_inc8(gb, L);
  CYC(b_+144, b_+145); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+145, b_+147); goto setVisiblec2; } // jr nz
  CYC(b_+145, b_+147);
  CALL_C(b_+147, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+150);
  CYC(b_+150, b_+152); alu_and(gb, 0x7c);
  CYC(b_+152, b_+153); B = A;
  CYC(b_+153, b_+155); A = mem_rd(gb, hRng2); // hRng2
  CYC(b_+155, b_+157); alu_and(gb, 0x7c);
  CYC(b_+157, b_+158); C = A;
  CYC(b_+158, b_+160); E = 0xcb; // Part.yh
  CYC(b_+160, b_+162); A = mem_rd(gb, hCameraY); // hCameraY
  CYC(b_+162, b_+163); alu_add(gb, B);
  CYC(b_+163, b_+164); mem_wr(gb, DE, A);
  CYC(b_+164, b_+166); E = 0xcd; // Part.xh
  CYC(b_+166, b_+168); A = mem_rd(gb, hCameraX); // hCameraX
  CYC(b_+168, b_+169); alu_add(gb, C);
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);

setVisiblec2:
  CYC(b_+170, b_+173); TAIL(objectSetVisiblec2); // jp

subid1_state1:
  CYC(b_+173, b_+175); C = 0x20;
  CALL_C(b_+175, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+178);
  if (!(F & FZ)) { CYCT(b_+178, b_+180); goto animate; } // jr nz
  CYC(b_+178, b_+180);
  CYC(b_+180, b_+182); goto func_6ebd; // jr
}
