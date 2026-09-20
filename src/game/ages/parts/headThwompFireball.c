#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6c2f, 0x6c31); E = 0xc4; // Part.state
  CYC(0x6c31, 0x6c32); A = mem_rd(gb, DE);
  {
    CYC(0x6c32, 0x6c33); push_effect(gb, 0x6c33);
    uint16_t target = headThwompFireball_jump_table(gb);
    if (target == 0x6c6a) goto state1;
    if (target == 0x6caa) goto state2;
    goto state0;
  }

state0:
  CYC(0x6c39, 0x6c3a); H = D;
  CYC(0x6c3a, 0x6c3b); L = E;
  CYC(0x6c3b, 0x6c3c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6c3c, 0x6c3e); L = 0xc6; // Part.counter1
  CYC(0x6c3e, 0x6c40); mem_wr(gb, HL, 0x1e);
  CYC(0x6c40, 0x6c42); L = 0xd4; // Part.speedZ
  CYC(0x6c42, 0x6c44); A = 0x20;
  CYC(0x6c44, 0x6c45); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c45, 0x6c47); mem_wr(gb, HL, 0xfc);
  CALL_C(0x6c47, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c4a);
  CYC(0x6c4a, 0x6c4c); alu_and(gb, 0x10);
  CYC(0x6c4c, 0x6c4e); alu_add(gb, 0x08);
  CYC(0x6c4e, 0x6c50); E = 0xc9; // Part.angle
  CYC(0x6c50, 0x6c51); mem_wr(gb, DE, A);
  CALL_C(0x6c51, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6c54);
  CYC(0x6c54, 0x6c56); alu_and(gb, 0x03);
  CYC(0x6c56, 0x6c59); SET_HL(0x6c66); // table_6c66
  CYC(0x6c59, 0x6c5a); headThwompFireball_addAToHl_from_rst(gb, 0x6c5a);
  CYC(0x6c5a, 0x6c5c); E = 0xd0; // Part.speed
  CYC(0x6c5c, 0x6c5d); A = mem_rd(gb, HL);
  CYC(0x6c5d, 0x6c5e); mem_wr(gb, DE, A);
  CALL_C(0x6c5e, objectSetVisible82_hook, 0x1e69, 0x6c61);
  CYC(0x6c61, 0x6c63); A = 0x59; // SND_FALLINHOLE
  CYC(0x6c63, 0x6c66); playSound_b00_hook(gb); return; // jp

state1:
  CALL_C(0x6c6a, objectApplySpeed_hook, 0x201d, 0x6c6d);
  CYC(0x6c6d, 0x6c6e); H = D;
  CYC(0x6c6e, 0x6c70); L = 0xd4; // Part.speedZ
  CYC(0x6c70, 0x6c72); E = 0xca;
  CALL_C(0x6c72, add16BitRefs_hook, 0x23a7, 0x6c75);
  CYC(0x6c75, 0x6c76); L = alu_dec8(gb, L);
  CYC(0x6c76, 0x6c77); A = mem_rd(gb, HL);
  CYC(0x6c77, 0x6c79); alu_add(gb, 0x20);
  CYC(0x6c79, 0x6c7a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c7a, 0x6c7b); A = mem_rd(gb, HL);
  CYC(0x6c7b, 0x6c7d); alu_adc(gb, 0x00);
  CYC(0x6c7d, 0x6c7e); mem_wr(gb, HL, A);
  CALL_C(0x6c7e, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6c81);
  if (!(F & FZ)) { CYCT(0x6c81, 0x6c83); goto animate; } // jr nz
  CYC(0x6c81, 0x6c83);
  CYC(0x6c83, 0x6c84); A = mem_rd(gb, DE);
  CYC(0x6c84, 0x6c86); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(0x6c86, 0x6c89); partDelete_hook(gb); return; } // jp nc
  CYC(0x6c86, 0x6c89);
  CYC(0x6c89, 0x6c8b); alu_add(gb, 0x06);
  CYC(0x6c8b, 0x6c8c); B = A;
  CYC(0x6c8c, 0x6c8e); L = 0xcd; // Part.xh
  CYC(0x6c8e, 0x6c8f); C = mem_rd(gb, HL);
  CALL_C(0x6c8f, checkTileCollisionAt_allowHoles_hook, 0x14d1, 0x6c92);
  if (!(F & FC)) { CYCT(0x6c92, 0x6c94); goto animate; } // jr nc
  CYC(0x6c92, 0x6c94);
  CYC(0x6c94, 0x6c95); H = D;
  CYC(0x6c95, 0x6c97); L = 0xc4; // Part.state
  CYC(0x6c97, 0x6c98); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6c98, 0x6c9a); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x6c9a, 0x6c9c); A = 0x0b;
  CYC(0x6c9c, 0x6c9d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c9d, 0x6c9e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c9e, 0x6ca0); mem_wr(gb, HL, 0x26);
  CYC(0x6ca0, 0x6ca2); A = 0x01;
  CALL_C(0x6ca2, partSetAnimation_hook, 0x2988, 0x6ca5);
  CYC(0x6ca5, 0x6ca7); A = 0xa5; // SND_BREAK_ROCK
  CYC(0x6ca7, 0x6caa); playSound_b00_hook(gb); return; // jp

state2:
  CYC(0x6caa, 0x6cac); E = 0xe1; // Part.animParameter
  CYC(0x6cac, 0x6cad); A = mem_rd(gb, DE);
  CYC(0x6cad, 0x6caf); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6caf, 0x6cb2); partDelete_hook(gb); return; } // jp nz
  CYC(0x6caf, 0x6cb2);
  CYC(0x6cb2, 0x6cb5); SET_HL(0x6cc0); // table_6cc0
  CYC(0x6cb5, 0x6cb6); headThwompFireball_addAToHl_from_rst(gb, 0x6cb6);
  CYC(0x6cb6, 0x6cb8); E = 0xe6; // Part.collisionRadiusY
  CYC(0x6cb8, 0x6cb9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6cb9, 0x6cba); mem_wr(gb, DE, A);
  CYC(0x6cba, 0x6cbb); E = alu_inc8(gb, E);
  CYC(0x6cbb, 0x6cbc); A = mem_rd(gb, HL);
  CYC(0x6cbc, 0x6cbd); mem_wr(gb, DE, A);

animate:
  CYC(0x6cbd, 0x6cc0); partAnimate_hook(gb); return; // jp
}
