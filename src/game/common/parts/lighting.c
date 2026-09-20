#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t lighting_jump_table(GB *gb) {
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

static void lighting_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode27_hook(GB *gb) {
  BASE(partCode27);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = lighting_jump_table(gb);
    if (target == b_+10) goto state0;
    if (target == b_+45) goto state1;
    goto state2;
  }

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+16);
  CYC(b_+16, b_+18); E = 0xf0; // Part.var30
  CYC(b_+18, b_+20); alu_and(gb, 0x06);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = 0xcf; // Part.zh
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xc0);
  CYC(b_+26, b_+28); L = 0xd7; // Part.relatedObj1+1
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+30); return; } // ret z
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); L = 0xc6; // Part.counter1
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x1e);
  CYC(b_+35, b_+37); L = 0xcb; // Part.yh
  CYC(b_+37, b_+39); A = H8(hEnemyTargetY);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+43); A = H8(hEnemyTargetX);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  RET(b_+44); return; // ret

state1:
  CALL_C(b_+45, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+48);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; } // ret nz
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+50); L = E;
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+51, b_+53); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+53, playSound_b00_hook, SYM(playSound_b00), b_+56);
  CYC(b_+56, b_+59); objectSetVisible81_hook(gb); return; // jp

state2:
  CALL_C(b_+59, partAnimate_hook, SYM(partAnimate), b_+62);
  CYC(b_+62, b_+64); E = 0xe1; // Part.animParameter
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+66, b_+69); partDelete_hook(gb); return; } // jp z
  CYC(b_+66, b_+69);
  CYC(b_+69, b_+72); push_effect(gb, b_+72); goto func_55a6; // call $55a6 (own burn; local, inlined)

resume_559b:
  CYC(b_+72, b_+74); E = 0xc3; // Part.var03
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+76); return; } // ret z
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); A = 0xff;
  CYC(b_+79, b_+82); mem_wr(gb, wTmpcfc0_fairyHideAndSeek_cfd2, A);
  RET(b_+82); return; // ret

func_55a6:
  CYC(b_+83, b_+85); E = 0xe1; // Part.animParameter
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+88); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+88, b_+91); push_effect(gb, b_+91); goto func_55e7; } // call nz (taken; local, inlined)
  CYC(b_+88, b_+91);

resume_55ae:
  CYC(b_+91, b_+93); E = 0xe1; // Part.animParameter
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+96); alu_and(gb, 0x0e);
  CYC(b_+96, b_+99); SET_HL(b_+135); // table_55da
  CYC(b_+99, b_+100); lighting_addAToHl_from_rst(gb, b_+100);
  CYC(b_+100, b_+102); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+102, b_+103); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+105); E = alu_inc8(gb, E);
  CYC(b_+105, b_+106); A = mem_rd(gb, HL);
  CYC(b_+106, b_+107); mem_wr(gb, DE, A);
  CYC(b_+107, b_+109); E = 0xe1; // Part.animParameter
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+112); alu_and(gb, 0x70);
  CYC(b_+112, b_+114); A = alu_swap(gb, A);
  CYC(b_+114, b_+117); SET_HL(b_+143); // table_55e2
  CYC(b_+117, b_+118); lighting_addAToHl_from_rst(gb, b_+118);
  CYC(b_+118, b_+120); E = 0xcf; // Part.zh
  CYC(b_+120, b_+121); A = mem_rd(gb, HL);
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
  CYC(b_+122, b_+124); E = 0xe1; // Part.animParameter
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+127); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+127, b_+128); ret_effect(gb); goto resume_559b; } // ret z (taken; pops to caller)
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+129); A = alu_dec8(gb, A);
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  CYC(b_+130, b_+132); A = 0x06;
  CYC(b_+132, b_+135); setScreenShakeCounter_hook(gb); goto resume_559b; // jp (tail call; its own ret pops to caller)

func_55e7:
  CYC(b_+148, b_+150); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+150, b_+151); mem_wr(gb, DE, A);
  CYC(b_+151, b_+153); alu_and(gb, 0x0e);
  CYC(b_+153, b_+155); alu_sub(gb, 0x02);
  CYC(b_+155, b_+156); B = A;
  CYC(b_+156, b_+158); E = 0xf0; // Part.var30
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+160); alu_add(gb, B);
  CYC(b_+160, b_+163); SET_HL(b_+176); // table_5603
  CYC(b_+163, b_+164); lighting_addAToHl_from_rst(gb, b_+164);
  CYC(b_+164, b_+165); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+165, b_+166); C = mem_rd(gb, HL);
  CYC(b_+166, b_+167); B = A;
  CALL_C(b_+167, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+170);
  if (!(F & FZ)) { CYCT(b_+170, b_+171); ret_effect(gb); goto resume_55ae; } // ret nz (taken; pops to caller)
  CYC(b_+170, b_+171);
  CYC(b_+171, b_+173); mem_wr(gb, HL, 0x08);
  CYC(b_+173, b_+176); objectCopyPositionWithOffset_hook(gb); goto resume_55ae; // jp (tail call; its own ret pops to caller)
}
