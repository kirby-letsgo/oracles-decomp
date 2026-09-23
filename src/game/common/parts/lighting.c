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
  CYC(b_+O(0), b_+OE(2)); E = 0xc4; // Part.state
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
    uint16_t target = lighting_jump_table(gb);
    if (target == b_+O(10)) goto state0;
    if (target == b_+O(45)) goto state1;
    if (target == b_+O(59)) goto state2;
    hook_continue(gb, target, sp0_); return;
  }

state0:
  CYC(b_+O(10), b_+OE(12)); A = 0x01;
  CYC(b_+O(12), b_+OE(13)); mem_wr(gb, DE, A);
  CALL_C(b_+O(13), getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+OE(16));
  CYC(b_+O(16), b_+OE(18)); E = 0xf0; // Part.var30
  CYC(b_+O(18), b_+OE(20)); alu_and(gb, 0x06);
  CYC(b_+O(20), b_+OE(21)); mem_wr(gb, DE, A);
  CYC(b_+O(21), b_+OE(22)); H = D;
  CYC(b_+O(22), b_+OE(24)); L = 0xcf; // Part.zh
  CYC(b_+O(24), b_+OE(26)); mem_wr(gb, HL, 0xc0);
  CYC(b_+O(26), b_+OE(28)); L = 0xd7; // Part.relatedObj1+1
  CYC(b_+O(28), b_+OE(29)); A = mem_rd(gb, HL);
  CYC(b_+O(29), b_+OE(30)); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+O(30)); return; } // ret z
  CYC(b_+O(30), b_+OE(31));
  CYC(b_+O(31), b_+OE(33)); L = 0xc6; // Part.counter1
  CYC(b_+O(33), b_+OE(35)); mem_wr(gb, HL, 0x1e);
  CYC(b_+O(35), b_+OE(37)); L = 0xcb; // Part.yh
  CYC(b_+O(37), b_+OE(39)); A = H8(hEnemyTargetY);
  CYC(b_+O(39), b_+OE(40)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(40), b_+OE(41)); L = alu_inc8(gb, L);
  CYC(b_+O(41), b_+OE(43)); A = H8(hEnemyTargetX);
  CYC(b_+O(43), b_+OE(44)); mem_wr(gb, HL, A);
  RET(b_+O(44)); return; // ret

state1:
  CALL_C(b_+O(45), partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+OE(48));
  if (!(F & FZ)) { RET_TAKEN(b_+O(48)); return; } // ret nz
  CYC(b_+O(48), b_+OE(49));
  CYC(b_+O(49), b_+OE(50)); L = E;
  CYC(b_+O(50), b_+OE(51)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(51), b_+OE(53)); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+O(53), playSound_b00_hook, SYM(playSound_b00), b_+OE(56));
  CYC(b_+O(56), b_+OE(59)); TAIL(objectSetVisible81); // jp

state2:
  CALL_C(b_+O(59), partAnimate_hook, SYM(partAnimate), b_+OE(62));
  CYC(b_+O(62), b_+OE(64)); E = 0xe1; // Part.animParameter
  CYC(b_+O(64), b_+OE(65)); A = mem_rd(gb, DE);
  CYC(b_+O(65), b_+OE(66)); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+O(66), b_+OE(69)); partDelete_hook(gb); return; } // jp z
  CYC(b_+O(66), b_+OE(69));
  CYC(b_+O(69), b_+OE(72)); push_effect(gb, b_+OE(72)); goto func_55a6; // call $55a6 (own burn; local, inlined)

resume_559b:
  CYC(b_+O(72), b_+OE(74)); E = 0xc3; // Part.var03
  CYC(b_+O(74), b_+OE(75)); A = mem_rd(gb, DE);
  CYC(b_+O(75), b_+OE(76)); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+O(76)); return; } // ret z
  CYC(b_+O(76), b_+OE(77));
  if (game_seasons) {
    CYC(b_+S(77), b_+S(78)); B = A;
    CYC(b_+S(78), b_+S(81)); A = W8(wTmpcfc0_goronDance_danceAnimation);
    CYC(b_+S(81), b_+S(82)); alu_or(gb, B);
    CYC(b_+S(82), b_+S(85)); W8(wTmpcfc0_goronDance_danceAnimation) = A;
    RET(b_+S(85)); return;
  } else {
    CYC(b_+77, b_+79); A = 0xff;
    CYC(b_+79, b_+82); W8(wTmpcfc0_goronDance_danceAnimation) = A;
    RET(b_+82); return; // ret
  }

func_55a6:
  CYC(b_+O(83), b_+OE(85)); E = 0xe1; // Part.animParameter
  CYC(b_+O(85), b_+OE(86)); A = mem_rd(gb, DE);
  CYC(b_+O(86), b_+OE(88)); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+O(88), b_+OE(91)); push_effect(gb, b_+OE(91)); goto func_55e7; } // call nz (taken; local, inlined)
  CYC(b_+O(88), b_+OE(91));

resume_55ae:
  CYC(b_+O(91), b_+OE(93)); E = 0xe1; // Part.animParameter
  CYC(b_+O(93), b_+OE(94)); A = mem_rd(gb, DE);
  CYC(b_+O(94), b_+OE(96)); alu_and(gb, 0x0e);
  CYC(b_+O(96), b_+OE(99)); SET_HL(b_+O(135)); // table_55da
  CYC(b_+O(99), b_+OE(100)); lighting_addAToHl_from_rst(gb, b_+OE(100));
  CYC(b_+O(100), b_+OE(102)); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+O(102), b_+OE(103)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(103), b_+OE(104)); mem_wr(gb, DE, A);
  CYC(b_+O(104), b_+OE(105)); E = alu_inc8(gb, E);
  CYC(b_+O(105), b_+OE(106)); A = mem_rd(gb, HL);
  CYC(b_+O(106), b_+OE(107)); mem_wr(gb, DE, A);
  CYC(b_+O(107), b_+OE(109)); E = 0xe1; // Part.animParameter
  CYC(b_+O(109), b_+OE(110)); A = mem_rd(gb, DE);
  CYC(b_+O(110), b_+OE(112)); alu_and(gb, 0x70);
  CYC(b_+O(112), b_+OE(114)); A = alu_swap(gb, A);
  CYC(b_+O(114), b_+OE(117)); SET_HL(b_+O(143)); // table_55e2
  CYC(b_+O(117), b_+OE(118)); lighting_addAToHl_from_rst(gb, b_+OE(118));
  CYC(b_+O(118), b_+OE(120)); E = 0xcf; // Part.zh
  CYC(b_+O(120), b_+OE(121)); A = mem_rd(gb, HL);
  CYC(b_+O(121), b_+OE(122)); mem_wr(gb, DE, A);
  CYC(b_+O(122), b_+OE(124)); E = 0xe1; // Part.animParameter
  CYC(b_+O(124), b_+OE(125)); A = mem_rd(gb, DE);
  CYC(b_+O(125), b_+OE(127)); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+O(127), b_+OE(128)); ret_effect(gb); goto resume_559b; } // ret z (taken; pops to caller)
  CYC(b_+O(127), b_+OE(128));
  CYC(b_+O(128), b_+OE(129)); A = alu_dec8(gb, A);
  CYC(b_+O(129), b_+OE(130)); mem_wr(gb, DE, A);
  CYC(b_+O(130), b_+OE(132)); A = 0x06;
  CYC(b_+O(132), b_+OE(135)); setScreenShakeCounter_hook(gb); goto resume_559b; // jp (tail call; its own ret pops to caller)

func_55e7:
  CYC(b_+O(148), b_+OE(150)); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+O(150), b_+OE(151)); mem_wr(gb, DE, A);
  CYC(b_+O(151), b_+OE(153)); alu_and(gb, 0x0e);
  CYC(b_+O(153), b_+OE(155)); alu_sub(gb, 0x02);
  CYC(b_+O(155), b_+OE(156)); B = A;
  CYC(b_+O(156), b_+OE(158)); E = 0xf0; // Part.var30
  CYC(b_+O(158), b_+OE(159)); A = mem_rd(gb, DE);
  CYC(b_+O(159), b_+OE(160)); alu_add(gb, B);
  CYC(b_+O(160), b_+OE(163)); SET_HL(b_+O(176)); // table_5603
  CYC(b_+O(163), b_+OE(164)); lighting_addAToHl_from_rst(gb, b_+OE(164));
  CYC(b_+O(164), b_+OE(165)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(165), b_+OE(166)); C = mem_rd(gb, HL);
  CYC(b_+O(166), b_+OE(167)); B = A;
  CALL_C(b_+O(167), getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+OE(170));
  if (!(F & FZ)) { CYCT(b_+O(170), b_+OE(171)); ret_effect(gb); goto resume_55ae; } // ret nz (taken; pops to caller)
  CYC(b_+O(170), b_+OE(171));
  CYC(b_+O(171), b_+OE(173)); mem_wr(gb, HL, 0x08);
  CYC(b_+O(173), b_+OE(176)); objectCopyPositionWithOffset_hook(gb); goto resume_55ae; // jp (tail call; its own ret pops to caller)
}
