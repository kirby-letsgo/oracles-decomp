#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode39), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode39), (from), (to), true)

static uint16_t headThwompFireball_jump_table(GB *gb) {
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

static void headThwompFireball_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_HEAD_THWOMP_FIREBALL
void partCode39_hook(GB *gb) {
  BASE(partCode39);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = headThwompFireball_jump_table(gb);
    if (target == b_+59) goto state1;
    if (target == b_+123) goto state2;
    goto state0;
  }

state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = 0xc6; // Part.counter1
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x1e);
  CYC(b_+17, b_+19); L = 0xd4; // Part.speedZ
  CYC(b_+19, b_+21); A = 0x20;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0xfc);
  CALL_C(b_+24, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+27);
  CYC(b_+27, b_+29); alu_and(gb, 0x10);
  CYC(b_+29, b_+31); alu_add(gb, 0x08);
  CYC(b_+31, b_+33); E = 0xc9; // Part.angle
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CALL_C(b_+34, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+37);
  CYC(b_+37, b_+39); alu_and(gb, 0x03);
  CYC(b_+39, b_+42); SET_HL(b_+55); // table_6c66
  CYC(b_+42, b_+43); headThwompFireball_addAToHl_from_rst(gb, b_+43);
  CYC(b_+43, b_+45); E = 0xd0; // Part.speed
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CALL_C(b_+47, objectSetVisible82_hook, SYM(objectSetVisible82), b_+50);
  CYC(b_+50, b_+52); A = 0x59; // SND_FALLINHOLE
  CYC(b_+52, b_+55); playSound_b00_hook(gb); return; // jp

state1:
  CALL_C(b_+59, objectApplySpeed_hook, SYM(objectApplySpeed), b_+62);
  CYC(b_+62, b_+63); H = D;
  CYC(b_+63, b_+65); L = 0xd4; // Part.speedZ
  CYC(b_+65, b_+67); E = 0xca;
  CALL_C(b_+67, add16BitRefs_hook, SYM(add16BitRefs), b_+70);
  CYC(b_+70, b_+71); L = alu_dec8(gb, L);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); alu_add(gb, 0x20);
  CYC(b_+74, b_+75); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+78); alu_adc(gb, 0x00);
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CALL_C(b_+79, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+82);
  if (!(F & FZ)) { CYCT(b_+82, b_+84); goto animate; } // jr nz
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+87); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(b_+87, b_+90); partDelete_hook(gb); return; } // jp nc
  CYC(b_+87, b_+90);
  CYC(b_+90, b_+92); alu_add(gb, 0x06);
  CYC(b_+92, b_+93); B = A;
  CYC(b_+93, b_+95); L = 0xcd; // Part.xh
  CYC(b_+95, b_+96); C = mem_rd(gb, HL);
  CALL_C(b_+96, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+99);
  if (!(F & FC)) { CYCT(b_+99, b_+101); goto animate; } // jr nc
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+102); H = D;
  CYC(b_+102, b_+104); L = 0xc4; // Part.state
  CYC(b_+104, b_+105); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+105, b_+107); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+107, b_+109); A = 0x0b;
  CYC(b_+109, b_+110); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+110, b_+111); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x26);
  CYC(b_+113, b_+115); A = 0x01;
  CALL_C(b_+115, partSetAnimation_hook, SYM(partSetAnimation), b_+118);
  CYC(b_+118, b_+120); A = 0xa5; // SND_BREAK_ROCK
  CYC(b_+120, b_+123); playSound_b00_hook(gb); return; // jp

state2:
  CYC(b_+123, b_+125); E = 0xe1; // Part.animParameter
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+128); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+128, b_+131); partDelete_hook(gb); return; } // jp nz
  CYC(b_+128, b_+131);
  CYC(b_+131, b_+134); SET_HL(b_+145); // table_6cc0
  CYC(b_+134, b_+135); headThwompFireball_addAToHl_from_rst(gb, b_+135);
  CYC(b_+135, b_+137); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+137, b_+138); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+138, b_+139); mem_wr(gb, DE, A);
  CYC(b_+139, b_+140); E = alu_inc8(gb, E);
  CYC(b_+140, b_+141); A = mem_rd(gb, HL);
  CYC(b_+141, b_+142); mem_wr(gb, DE, A);

animate:
  CYC(b_+142, b_+145); partAnimate_hook(gb); return; // jp
}
