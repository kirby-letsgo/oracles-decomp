#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t pumpkinHeadProjectile_jump_table(GB *gb) {
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

static void pumpkinHeadProjectile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_PUMPKIN_HEAD_PROJECTILE
void partCode42_hook(GB *gb) {
  BASE(partCode42);
  uint16_t sp0_ = gb->sp;
  if (!(F & FZ)) { CYCT(b_+0, b_+3); partDelete_hook(gb); return; } // jp nz
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+5); E = 0xc4; // Part.state
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  {
    CYC(b_+6, b_+7); push_effect(gb, b_+7);
    uint16_t target = pumpkinHeadProjectile_jump_table(gb);
    if (target == b_+114) goto state1;
    if (target == b_+124) goto state2;
    goto state0;
  }

state0:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+18); L = 0xc6; // Part.counter1
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x08);
  CYC(b_+20, b_+22); L = 0xd0; // Part.speed
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x3c); // SPEED_0f0
  CYC(b_+24, b_+26); E = 0xcb; // Part.yh
  CYC(b_+26, b_+28); L = 0xcf; // Part.zh
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x00);
  CYC(b_+33, b_+35); E = 0xc2; // Part.subid
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+39); SET_BC(b_+103); // table_7421
  CALL_C(b_+39, addAToBc_hook, 0x006d, b_+42);
  CYC(b_+42, b_+44); L = 0xc9; // Part.angle
  CYC(b_+44, b_+45); A = mem_rd(gb, BC);
  CYC(b_+45, b_+46); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+46, b_+48); alu_and(gb, 0x1f);
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_7417; } // jr nz
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); alu_rrca(gb);
  CYC(b_+55, b_+56); alu_rrca(gb);
  CYC(b_+56, b_+59); SET_HL(b_+106); // table_7424
  CYC(b_+59, b_+60); pumpkinHeadProjectile_addAToHl_from_rst(gb, b_+60);
  CYC(b_+60, b_+62); E = 0xcb; // Part.yh
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+64); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CYC(b_+65, b_+67); E = 0xcd; // Part.xh
  CYC(b_+67, b_+68); SET_HL(HL + 1); // inc hl
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+73); B = 0x02;

L_7403:
  CALL_C(b_+73, getFreePartSlot_hook, SYM(getFreePartSlot), b_+76);
  if (!(F & FZ)) { CYCT(b_+76, b_+78); goto L_7417; } // jr nz
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x42); // PART_PUMPKIN_HEAD_PROJECTILE
  CYC(b_+80, b_+81); L = alu_inc8(gb, L);
  CYC(b_+81, b_+82); mem_wr(gb, HL, B);
  CYC(b_+82, b_+84); L = 0xc9; // Part.angle
  CYC(b_+84, b_+85); E = L;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); mem_wr(gb, HL, A);
  CALL_C(b_+87, objectCopyPosition_hook, SYM(objectCopyPosition), b_+90);
  CYC(b_+90, b_+91); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+91, b_+93); goto L_7403; } // jr nz
  CYC(b_+91, b_+93);

L_7417:
  CYC(b_+93, b_+95); E = 0xc9; // Part.angle
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+97); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+97, b_+100); objectSetVisible82_hook(gb); return; } // jp z
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+103); objectSetVisible81_hook(gb); return; // jp

state1:
  CALL_C(b_+114, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+117);
  if (!(F & FZ)) { CYCT(b_+117, b_+119); goto state2; } // jr nz
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+120); L = E;
  CYC(b_+120, b_+121); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+121, objectSetVisible82_hook, SYM(objectSetVisible82), b_+124);

state2:
  CALL_C(b_+124, partAnimate_hook, SYM(partAnimate), b_+127);
  CALL_C(b_+127, objectApplySpeed_hook, SYM(objectApplySpeed), b_+130);
  CALL_C(b_+130, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+133);
  if (!(F & FC)) { RET_TAKEN(b_+133); return; } // ret nc
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+137); partDelete_hook(gb); return; // jp
}
