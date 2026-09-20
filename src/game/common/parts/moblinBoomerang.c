#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t moblinBoomerang_jump_table(GB *gb) {
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

void func_53f5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x53f5, 0x53f7); A = 0x0b;
  CALL_C(0x53f7, objectGetRelatedObject1Var_hook, 0x2160, 0x53fa);
  CYC(0x53fa, 0x53fb); push_effect(gb, HL);
  CYC(0x53fb, 0x53fc); B = mem_rd(gb, HL);
  CYC(0x53fc, 0x53fe); L = 0x8d;
  CYC(0x53fe, 0x53ff); C = mem_rd(gb, HL);
  CALL_C(0x53ff, objectGetRelativeAngle_hook, 0x1ea4, 0x5402);
  CYC(0x5402, 0x5404); E = 0xc9; // Part.angle
  CYC(0x5404, 0x5405); mem_wr(gb, DE, A);
  CYC(0x5405, 0x5406); SET_HL(pop_effect(gb));
  CYC(0x5406, 0x5408); E = 0xcb; // Part.yh
  CYC(0x5408, 0x5409); A = mem_rd(gb, DE);
  CYC(0x5409, 0x540a); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x540a, 0x540c); alu_add(gb, 0x04);
  CYC(0x540c, 0x540e); alu_cp(gb, 0x09);
  if (!(F & FC)) { RET_TAKEN(0x540e); return; } // ret nc
  CYC(0x540e, 0x540f);
  CYC(0x540f, 0x5411); L = 0x8d;
  CYC(0x5411, 0x5413); E = 0xcd; // Part.xh
  CYC(0x5413, 0x5414); A = mem_rd(gb, DE);
  CYC(0x5414, 0x5415); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5415, 0x5417); alu_add(gb, 0x04);
  CYC(0x5417, 0x5419); alu_cp(gb, 0x09);
  RET(0x5419); return; // ret
}

void func_541a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x541a, 0x541d); A = mem_rd(gb, wFrameCounter);
  CYC(0x541d, 0x541f); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x541f); return; } // ret nz
  CYC(0x541f, 0x5420);
  CYC(0x5420, 0x5422); E = 0xd0; // Part.speed
  CYC(0x5422, 0x5423); A = mem_rd(gb, DE);
  CYC(0x5423, 0x5425); alu_add(gb, 0x05);
  CYC(0x5425, 0x5427); alu_cp(gb, 0x50);
  if (!(F & FC)) { RET_TAKEN(0x5427); return; } // ret nc
  CYC(0x5427, 0x5428);
  CYC(0x5428, 0x5429); mem_wr(gb, DE, A);
  RET(0x5429); return; // ret
}

void func_542a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x542a, 0x542b); H = D;
  CYC(0x542b, 0x542d); L = 0xc7; // Part.counter2
  CYC(0x542d, 0x542e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x542e); return; } // ret nz
  CYC(0x542e, 0x542f);
  CYC(0x542f, 0x5431); mem_wr(gb, HL, 0x06);
  CYC(0x5431, 0x5433); E = 0xd0; // Part.speed
  CYC(0x5433, 0x5434); A = mem_rd(gb, DE);
  CYC(0x5434, 0x5436); alu_sub(gb, 0x05);
  if (F & FC) { RET_TAKEN(0x5436); return; } // ret c
  CYC(0x5436, 0x5437);
  CYC(0x5437, 0x5438); mem_wr(gb, DE, A);
  RET(0x5438); return; // ret
}

void partCode21_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5395, 0x5397); goto normalStatus; } // jr z
  CYC(0x5395, 0x5397);
  CYC(0x5397, 0x5399); E = 0xea; // Part.var2a
  CYC(0x5399, 0x539a); A = mem_rd(gb, DE);
  CYC(0x539a, 0x539c); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x539c, 0x539e); alu_sub(gb, 0x01);
  CYC(0x539e, 0x53a0); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x53a0, 0x53a2); goto normalStatus; } // jr nc
  CYC(0x53a0, 0x53a2);
  CYC(0x53a2, 0x53a4); E = 0xc4; // Part.state
  CYC(0x53a4, 0x53a6); A = 0x02;
  CYC(0x53a6, 0x53a7); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x53a7, 0x53a9); E = 0xd7; // Part.relatedObj1
  CYC(0x53a9, 0x53aa); A = mem_rd(gb, DE);
  CYC(0x53aa, 0x53ab); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x53ab, 0x53ad); goto partDeleteExit; } // jr z
  CYC(0x53ab, 0x53ad);
  CYC(0x53ad, 0x53af); E = 0xc4; // Part.state
  CYC(0x53af, 0x53b0); A = mem_rd(gb, DE);
  {
    CYC(0x53b0, 0x53b1); push_effect(gb, 0x53b1);
    uint16_t target = moblinBoomerang_jump_table(gb);
    if (target == 0x53b7) goto state0;
    if (target == 0x53c8) goto state1;
    goto state2;
  }

state0:
  CYC(0x53b7, 0x53b8); H = D;
  CYC(0x53b8, 0x53b9); L = E;
  CYC(0x53b9, 0x53ba); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53ba, 0x53bc); L = 0xc6; // Part.counter1
  CYC(0x53bc, 0x53be); mem_wr(gb, HL, 0x2d);
  CYC(0x53be, 0x53bf); L = L + 1;
  CYC(0x53bf, 0x53c1); mem_wr(gb, HL, 0x06);
  CYC(0x53c1, 0x53c3); L = 0xd0; // Part.speed
  CYC(0x53c3, 0x53c5); mem_wr(gb, HL, 0x50);
  CYC(0x53c5, 0x53c8); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(0x53c8, objectCheckSimpleCollision_hook, 0x1487, 0x53cb);
  if (!(F & FZ)) { CYCT(0x53cb, 0x53cd); goto func_53ee; } // jr nz
  CYC(0x53cb, 0x53cd);
  CALL_C(0x53cd, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x53d0);
  if (F & FZ) { CYCT(0x53d0, 0x53d2); goto func_53ee; } // jr z
  CYC(0x53d0, 0x53d2);
  CALL_C(0x53d2, func_542a_hook, 0x542a, 0x53d5);

objectApplySpeedExit:
  CALL_C(0x53d5, objectApplySpeed_hook, 0x201d, 0x53d8);

animate:
  CYC(0x53d8, 0x53db); partAnimate_hook(gb); return; // jp

state2:
  CALL_C(0x53db, func_541a_hook, 0x541a, 0x53de);
  CALL_C(0x53de, func_53f5_hook, 0x53f5, 0x53e1);
  if (!(F & FC)) { CYCT(0x53e1, 0x53e3); goto objectApplySpeedExit; } // jr nc
  CYC(0x53e1, 0x53e3);
  CYC(0x53e3, 0x53e5); A = 0x18;
  CALL_C(0x53e5, objectGetRelatedObject1Var_hook, 0x2160, 0x53e8);
  CYC(0x53e8, 0x53e9); alu_xor(gb, A);
  CYC(0x53e9, 0x53ea); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x53ea, 0x53eb); mem_wr(gb, HL, A);

partDeleteExit:
  CYC(0x53eb, 0x53ee); partDelete_hook(gb); return; // jp

func_53ee:
  CYC(0x53ee, 0x53f0); E = 0xc4; // Part.state
  CYC(0x53f0, 0x53f2); A = 0x02;
  CYC(0x53f2, 0x53f3); mem_wr(gb, DE, A);
  CYC(0x53f3, 0x53f5); goto animate; // jr
}
