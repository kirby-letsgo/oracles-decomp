#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t fallingFire_jump_table(GB *gb) {
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

static void fallingFire_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_5535_hook(GB *gb) {
  BASE(func_5535);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xe4; // Part.collisionType
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+7, b_+10); TAIL(objectSetVisible81); // jp
}

void func_553f_hook(GB *gb) {
  BASE(func_553f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x87;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+10); SET_HL(SYM(table_554f)); // table_554f
  CYC(b_+10, b_+11); fallingFire_addAToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+13); E = 0xc6; // Part.counter1
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return; // ret
}

void partCode23_hook(GB *gb) {
  BASE(partCode23);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = 0xc4; // Part.state
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = fallingFire_jump_table(gb);
    if (target == b_+12) goto subid0;
    if (target == b_+27) goto subid1;
    goto subid2;
  }

subid0:
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto func_54f6; } // jr z
  CYC(b_+14, b_+16);
  CALL_C(b_+16, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); mem_wr(gb, HL, GV(0x78, 0x3c));
  CYC(b_+22, b_+24); goto L_5504; // jr

func_54f6:
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return; // ret

subid1:
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+29, b_+31); goto func_54f6; } // jr z
  CYC(b_+29, b_+31);
  CALL_C(b_+31, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+34);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; } // ret nz
  CYC(b_+34, b_+35);
  CALL_C(b_+35, func_553f_hook, SYM(func_553f), b_+38);

L_5504:
  CALL_C(b_+38, getFreePartSlot_hook, SYM(getFreePartSlot), b_+41);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; } // ret nz
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x23); // PART_FALLING_FIRE
  CYC(b_+44, b_+45); L = alu_inc8(gb, L);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x02);
  CYC(b_+47, b_+49); L = 0xf0; // Part.var30
  CYC(b_+49, b_+50); E = L;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CYC(b_+52, b_+55); TAIL(objectCopyPosition); // jp

subid2:
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+57, b_+59); func_5535_hook(gb); return; } // jr z
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = 0xcb; // Part.yh
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+65); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(b_+65, b_+68); partDelete_hook(gb); return; } // jp nc
  CYC(b_+65, b_+68);
  CYC(b_+68, b_+70); L = 0xd0; // Part.speed
  CYC(b_+70, b_+72); E = 0xca;
  CALL_C(b_+72, add16BitRefs_hook, SYM(add16BitRefs), b_+75);
  CYC(b_+75, b_+76); L = alu_dec8(gb, L);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_add(gb, 0x10);
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+83); alu_adc(gb, 0x00);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A);
  CYC(b_+84, b_+87); TAIL(partAnimate); // jp
}
