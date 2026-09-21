#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t owlStatue_jump_table(GB *gb) {
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

static void owlStatue_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode13_hook(GB *gb) {
  BASE(partCode13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+O(0), b_+OE(2)); goto normalStatus; } // jr z
  CYC(b_+O(0), b_+OE(2));
  CYC(b_+O(2), b_+OE(4)); E = 0xea; // Part.var2a
  CYC(b_+O(4), b_+OE(5)); A = mem_rd(gb, DE);
  CYC(b_+O(5), b_+OE(7)); alu_cp(gb, 0x9a);
  if (!(F & FZ)) { CYCT(b_+O(7), b_+OE(9)); goto normalStatus; } // jr nz
  CYC(b_+O(7), b_+OE(9));
  CYC(b_+O(9), b_+OE(10)); H = D;
  CYC(b_+O(10), b_+OE(12)); L = 0xc4; // Part.state
  CYC(b_+O(12), b_+OE(13)); A = mem_rd(gb, HL);
  CYC(b_+O(13), b_+OE(15)); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+O(15), b_+OE(17)); goto normalStatus; } // jr nc
  CYC(b_+O(15), b_+OE(17));
  CYC(b_+O(17), b_+OE(18)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(18), b_+OE(20)); L = 0xc6; // Part.counter1
  CYC(b_+O(20), b_+OE(22)); mem_wr(gb, HL, 0x32);

normalStatus:
  CYC(b_+O(22), b_+OE(24)); E = 0xc4; // Part.state
  CYC(b_+O(24), b_+OE(25)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(25), b_+OE(26)); push_effect(gb, b_+OE(26));
    uint16_t target = owlStatue_jump_table(gb);
    if (target == b_+O(34)) goto state0;
    if (target == b_+O(51)) goto stateStub;
    if (target == b_+O(52)) goto state2;
    goto state3;
  }

state0:
  CYC(b_+O(34), b_+OE(35)); H = D;
  CYC(b_+O(35), b_+OE(36)); L = E;
  CYC(b_+O(36), b_+OE(37)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(37), b_+OE(39)); L = 0xff; // Part.var3f
  CYC(b_+O(39), b_+OE(41)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CALL_C(b_+O(41), objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+OE(44));
  CYC(b_+O(44), b_+OE(46)); H = 0xcf; // >wRoomLayout
  CYC(b_+O(46), b_+OE(48)); mem_wr(gb, HL, 0x00);
  CYC(b_+O(48), b_+OE(51)); TAIL(objectSetVisible83); // jp

stateStub:
  RET(b_+O(51)); return; // ret

state2:
  CALL_C(b_+O(52), partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+OE(55));
  if (!(F & FZ)) { CYCT(b_+O(55), b_+OE(57)); goto L_4cd6; } // jr nz
  CYC(b_+O(55), b_+OE(57));
  CYC(b_+O(57), b_+OE(59)); mem_wr(gb, HL, 0x1e);
  CYC(b_+O(59), b_+OE(60)); L = E;
  CYC(b_+O(60), b_+OE(61)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(61), b_+OE(63)); A = 0x01;
  CYC(b_+O(63), b_+OE(66)); TAIL(partSetAnimation); // jp

L_4cd6:
  CYC(b_+O(66), b_+OE(67)); A = mem_rd(gb, HL);
  CYC(b_+O(67), b_+OE(69)); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+O(69)); return; } // ret nz
  CYC(b_+O(69), b_+OE(70));
  CYC(b_+O(70), b_+OE(71)); A = mem_rd(gb, HL);
  CYC(b_+O(71), b_+OE(72)); alu_rrca(gb);
  CYC(b_+O(72), b_+OE(73)); alu_rrca(gb);
  CYC(b_+O(73), b_+OE(75)); alu_sub(gb, 0x02);
  CYC(b_+O(75), b_+OE(78)); SET_HL(b_+O(91)); // @owlStatueSparkleOffset
  CYC(b_+O(78), b_+OE(79)); owlStatue_addAToHl_from_rst(gb, b_+OE(79));
  CYC(b_+O(79), b_+OE(80)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(80), b_+OE(81)); B = A;
  CYC(b_+O(81), b_+OE(82)); C = mem_rd(gb, HL);
  CALL_C(b_+O(82), getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+OE(85));
  if (!(F & FZ)) { RET_TAKEN(b_+O(85)); return; } // ret nz
  CYC(b_+O(85), b_+OE(86));
  CYC(b_+O(86), b_+OE(88)); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  if (game_seasons) {
    CYC(b_+S(88), b_+S(89)); L = alu_inc8(gb, L);
    CYC(b_+S(89), b_+S(91)); mem_wr(gb, HL, 0x05);
  }
  CYC(b_+O(88), b_+OE(91)); TAIL(objectCopyPositionWithOffset); // jp

state3:
  CALL_C(b_+O(103), partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+OE(106));
  if (!(F & FZ)) { CYCT(b_+O(106), b_+OE(108)); goto L_4d07; } // jr nz
  CYC(b_+O(106), b_+OE(108));
  CYC(b_+O(108), b_+OE(109)); L = E;
  CYC(b_+O(109), b_+OE(111)); mem_wr(gb, HL, 0x01);
  CYC(b_+O(111), b_+OE(112)); alu_xor(gb, A);
  CYC(b_+O(112), b_+OE(115)); TAIL(partSetAnimation); // jp

L_4d07:
  CYC(b_+O(115), b_+OE(116)); A = mem_rd(gb, HL);
  CYC(b_+O(116), b_+OE(118)); alu_cp(gb, 0x16);
  if (!(F & FZ)) { RET_TAKEN(b_+O(118)); return; } // ret nz
  CYC(b_+O(118), b_+OE(119));
  CYC(b_+O(119), b_+OE(121)); L = 0xc2; // Part.subid
  CYC(b_+O(121), b_+OE(122)); C = mem_rd(gb, HL);
  CYC(b_+O(122), b_+OE(124)); B = 0x39;
  CYC(b_+O(124), b_+OE(127)); TAIL(showText); // jp
}
