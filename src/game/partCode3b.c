#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6e76, 0x6e78); E = 0xc2; // Part.subid
  CYC(0x6e78, 0x6e79); A = mem_rd(gb, DE);
  CYC(0x6e79, 0x6e7b); E = 0xc4; // Part.state
  CYC(0x6e7b, 0x6e7c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6e7c, 0x6e7e); goto subid0; } // jr z
  CYC(0x6e7c, 0x6e7e);
  CYC(0x6e7e, 0x6e81); goto subid1; // jp

subid0:
  CYC(0x6e81, 0x6e82); A = mem_rd(gb, DE);
  CYC(0x6e82, 0x6e83); push_effect(gb, 0x6e83);
  {
    uint16_t target = partCode3b_jump_table(gb);
    if (target == 0x6eae) goto subid0_state1;
    if (target == 0x6ed3) goto state2;
    goto subid0_state0;
  }

subid0_state0:
  CYC(0x6e89, 0x6e8a); H = D;
  CYC(0x6e8a, 0x6e8b); L = E;
  CYC(0x6e8b, 0x6e8c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6e8c, 0x6e8e); L = 0xd5; // Part.speedZ+1
  CYC(0x6e8e, 0x6e90); mem_wr(gb, HL, 0x02);
  CYC(0x6e90, 0x6e92); L = 0xcb; // Part.yh
  CYC(0x6e92, 0x6e94); A = mem_rd(gb, 0xffaa); // hCameraY
  CYC(0x6e94, 0x6e95); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6e95, 0x6e96); L = alu_inc8(gb, L);
  CYC(0x6e96, 0x6e97); A = mem_rd(gb, HL);
  CYC(0x6e97, 0x6e98); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6e98, 0x6e9a); goto after_random; } // jr nz
  CYC(0x6e98, 0x6e9a);
  CALL_C(0x6e9a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6e9d);
  CYC(0x6e9d, 0x6e9f); alu_and(gb, 0x7c);
  CYC(0x6e9f, 0x6ea0); B = A;
  CYC(0x6ea0, 0x6ea2); A = mem_rd(gb, 0xffac); // hCameraX
  CYC(0x6ea2, 0x6ea3); alu_add(gb, B);
  CYC(0x6ea3, 0x6ea5); E = 0xcd; // Part.xh
  CYC(0x6ea5, 0x6ea6); mem_wr(gb, DE, A);

after_random:
  CALL_C(0x6ea6, objectSetVisible82_hook, 0x1e69, 0x6ea9);
  CYC(0x6ea9, 0x6eab); A = 0x59; // SND_FALLINHOLE
  CYC(0x6eab, 0x6eae); playSound_b00_hook(gb); return; // jp

subid0_state1:
  CYC(0x6eae, 0x6eb0); A = 0x20;
  CALL_C(0x6eb0, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x6eb3);
  if (F & FC) { CYCT(0x6eb3, 0x6eb5); goto func_6ebd; } // jr c
  CYC(0x6eb3, 0x6eb5);
  CYC(0x6eb5, 0x6eb6); A = mem_rd(gb, DE);
  CYC(0x6eb6, 0x6eb8); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(0x6eb8, 0x6eba); goto animate; } // jr c
  CYC(0x6eb8, 0x6eba);
  CYC(0x6eba, 0x6ebd); partDelete_hook(gb); return; // jp

func_6ebd:
  CYC(0x6ebd, 0x6ebe); H = D;
  CYC(0x6ebe, 0x6ec0); L = 0xc4; // Part.state
  CYC(0x6ec0, 0x6ec1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ec1, 0x6ec3); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x6ec3, 0x6ec5); A = 0x0b;
  CYC(0x6ec5, 0x6ec6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6ec6, 0x6ec7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6ec7, 0x6ec9); mem_wr(gb, HL, 0x02);
  CYC(0x6ec9, 0x6ecb); A = 0x01;
  CALL_C(0x6ecb, partSetAnimation_hook, 0x2988, 0x6ece);
  CYC(0x6ece, 0x6ed0); A = 0xa5; // SND_BREAK_ROCK
  CYC(0x6ed0, 0x6ed3); playSound_b00_hook(gb); return; // jp

state2:
  CYC(0x6ed3, 0x6ed5); E = 0xe1; // Part.animParameter
  CYC(0x6ed5, 0x6ed6); A = mem_rd(gb, DE);
  CYC(0x6ed6, 0x6ed8); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6ed8, 0x6edb); partDelete_hook(gb); return; } // jp nz
  CYC(0x6ed8, 0x6edb);
  CYC(0x6edb, 0x6ede); SET_HL(0x6ee9); // table_6ee9
  CYC(0x6ede, 0x6edf); partCode3b_addAToHl_from_rst(gb, 0x6edf);
  CYC(0x6edf, 0x6ee1); E = 0xe6; // Part.collisionRadiusY
  CYC(0x6ee1, 0x6ee2); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6ee2, 0x6ee3); mem_wr(gb, DE, A);
  CYC(0x6ee3, 0x6ee4); E = alu_inc8(gb, E);
  CYC(0x6ee4, 0x6ee5); A = mem_rd(gb, HL);
  CYC(0x6ee5, 0x6ee6); mem_wr(gb, DE, A);

animate:
  CYC(0x6ee6, 0x6ee9); partAnimate_hook(gb); return; // jp

subid1:
  CYC(0x6ef3, 0x6ef4); A = mem_rd(gb, DE);
  CYC(0x6ef4, 0x6ef5); push_effect(gb, 0x6ef5);
  {
    uint16_t target = partCode3b_jump_table(gb);
    if (target == 0x6f23) goto subid1_state1;
    if (target == 0x6ed3) goto state2;
    goto subid1_state0;
  }

subid1_state0:
  CYC(0x6efb, 0x6efc); H = D;
  CYC(0x6efc, 0x6efd); L = E;
  CYC(0x6efd, 0x6efe); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6efe, 0x6f00); L = 0xd5; // Part.speedZ+1
  CYC(0x6f00, 0x6f02); mem_wr(gb, HL, 0x02);
  CYC(0x6f02, 0x6f04); L = 0xcb; // Part.yh
  CYC(0x6f04, 0x6f05); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6f05, 0x6f06); L = alu_inc8(gb, L);
  CYC(0x6f06, 0x6f07); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6f07, 0x6f09); goto setVisiblec2; } // jr nz
  CYC(0x6f07, 0x6f09);
  CALL_C(0x6f09, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6f0c);
  CYC(0x6f0c, 0x6f0e); alu_and(gb, 0x7c);
  CYC(0x6f0e, 0x6f0f); B = A;
  CYC(0x6f0f, 0x6f11); A = mem_rd(gb, 0xff95); // hRng2
  CYC(0x6f11, 0x6f13); alu_and(gb, 0x7c);
  CYC(0x6f13, 0x6f14); C = A;
  CYC(0x6f14, 0x6f16); E = 0xcb; // Part.yh
  CYC(0x6f16, 0x6f18); A = mem_rd(gb, 0xffaa); // hCameraY
  CYC(0x6f18, 0x6f19); alu_add(gb, B);
  CYC(0x6f19, 0x6f1a); mem_wr(gb, DE, A);
  CYC(0x6f1a, 0x6f1c); E = 0xcd; // Part.xh
  CYC(0x6f1c, 0x6f1e); A = mem_rd(gb, 0xffac); // hCameraX
  CYC(0x6f1e, 0x6f1f); alu_add(gb, C);
  CYC(0x6f1f, 0x6f20); mem_wr(gb, DE, A);

setVisiblec2:
  CYC(0x6f20, 0x6f23); objectSetVisiblec2_hook(gb); return; // jp

subid1_state1:
  CYC(0x6f23, 0x6f25); C = 0x20;
  CALL_C(0x6f25, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6f28);
  if (!(F & FZ)) { CYCT(0x6f28, 0x6f2a); goto animate; } // jr nz
  CYC(0x6f28, 0x6f2a);
  CYC(0x6f2a, 0x6f2c); goto func_6ebd; // jr
}
