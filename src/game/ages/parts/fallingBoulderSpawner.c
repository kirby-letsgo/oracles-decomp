#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t fallingBoulderSpawner_jump_table(GB *gb) {
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

static void fallingBoulderSpawner_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_FALLING_BOULDER_SPAWNER
void partCode45_hook(GB *gb) {
  BASE(partCode45);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = fallingBoulderSpawner_jump_table(gb);
    if (target == b_+50) goto state1;
    if (target == b_+91) goto state2;
    goto state0;
  }

state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = 0xd0; // Part.speed
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x32);
  CYC(b_+17, b_+19); L = 0xcb; // Part.yh
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(b_+22, b_+24); goto L_754a; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); alu_add(gb, 0x04);

L_754a:
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+27, b_+29); E = 0xf0; // Part.var30
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); E = alu_inc8(gb, E);
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+36); E = 0xc2; // Part.subid
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+40); SET_HL(b_+46); // table_755e (initialTimeToAppear)
  CYC(b_+40, b_+41); fallingBoulderSpawner_addAToHl_from_rst(gb, b_+41);
  CYC(b_+41, b_+43); E = 0xc6; // Part.counter1
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  RET(b_+45); return; // ret

state1:
  CALL_C(b_+50, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+53);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; } // ret nz
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+55); L = E;
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+56, b_+58); L = 0xe4; // Part.collisionType
  CYC(b_+58, b_+60); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

bounceRandomlyDownwards:
  CYC(b_+60, b_+62); L = 0xd4; // Part.speedZ
  CYC(b_+62, b_+64); A = 0x60;
  CYC(b_+64, b_+65); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0xfe);

L_7573:
  CALL_C(b_+67, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+70);
  CYC(b_+70, b_+72); alu_and(gb, 0x07);
  CYC(b_+72, b_+74); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+74, b_+76); goto L_7573; } // jr nc
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); alu_sub(gb, 0x03);
  CYC(b_+78, b_+80); alu_add(gb, 0x10);
  CYC(b_+80, b_+82); E = 0xc9; // Part.angle
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CALL_C(b_+83, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+86);
  CYC(b_+86, b_+88); A = 0xb3; // SND_RUMBLE
  CYC(b_+88, b_+91); playSound_b00_hook(gb); // jp (own burn; local, inlined)
  if (gb->pc == b_+99 && gb->sp == sp0_) goto state2_afterBounceCall;
  return;

state2:
  CYC(b_+91, b_+93); C = 0x20;
  CALL_C(b_+93, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+96);
  if (F & FZ) { CYCT(b_+96, b_+99); push_effect(gb, b_+99); goto bounceRandomlyDownwards; } // call z (own burn; local, inlined)
  CYC(b_+96, b_+99);

state2_afterBounceCall:
  CALL_C(b_+99, objectApplySpeed_hook, SYM(objectApplySpeed), b_+102);
  CYC(b_+102, b_+104); E = 0xcb; // Part.yh
  CYC(b_+104, b_+105); A = mem_rd(gb, DE);
  CYC(b_+105, b_+107); alu_cp(gb, 0x88);
  if (F & FC) { CYCT(b_+107, b_+110); partAnimate_hook(gb); return; } // jp c
  CYC(b_+107, b_+110);
  CYC(b_+110, b_+111); H = D;
  CYC(b_+111, b_+113); L = 0xc4; // Part.state
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+116); L = 0xe4; // Part.collisionType
  CYC(b_+116, b_+118); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+118, b_+120); L = 0xc6; // Part.counter1
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0xb4);
  CYC(b_+122, b_+124); E = 0xf0; // Part.var30
  CYC(b_+124, b_+126); L = 0xcb; // Part.yh
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+128); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+128, b_+129); E = alu_inc8(gb, E);
  CYC(b_+129, b_+130); L = alu_inc8(gb, L);
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); mem_wr(gb, HL, A);
  CYC(b_+132, b_+135); TAIL(objectSetInvisible); // jp
}
