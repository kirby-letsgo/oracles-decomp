#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t feather_parent_jump_table(GB *gb) {
  burn_rom(gb,0,0,1,false);alu_add(gb,A);burn_rom(gb,0,1,2,false);SET_HL(pop_effect(gb));burn_rom(gb,0,2,3,false);alu_add(gb,L);burn_rom(gb,0,3,4,false);L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);return HL;
}

void parentItemCode_feather_hook(GB *gb) {
  BASE(parentItemCode_feather);
  uint16_t sp0_=gb->sp;
  CYC(b_+O(0),b_+OE(2));E=4;CYC(b_+O(2),b_+OE(3));A=mem_rd(gb,DE);CYC(b_+O(3),b_+OE(4));push_effect(gb,b_+O(4));
  do { uint16_t jt_ = (feather_parent_jump_table(gb));
    if (jt_ == b_+O(8)) { break; }
    else if (jt_ == b_+(game_seasons ? S(95) : 98)) { goto state1; }
    else { hook_continue(gb,HL,sp0_);return; }
  } while (0);
  if (!game_seasons) {
    CALL_C(b_+8,isLinkUnderwater_hook,SYM(isLinkUnderwater),b_+11); if(!(F&FZ)){CYCT(b_+11,b_+13);goto clear;} CYC(b_+11,b_+13);
    CYC(b_+13,b_+16);A=W8(w1ParentItem2_id);CYC(b_+16,b_+18);alu_cp(gb,0x0a);if(F&FZ){CYCT(b_+18,b_+20);goto clear;}CYC(b_+18,b_+20);
  }
  CYC(b_+O(20),b_+OE(23));A=W8(wLinkObjectIndex);CYC(b_+O(23),b_+OE(24));alu_rrca(gb);if(F&FC){CYCT(b_+O(24),b_+OE(26));goto clear;}CYC(b_+O(24),b_+OE(26));
  CYC(b_+O(26),b_+OE(29));A=W8(wLinkGrabState);CYC(b_+O(29),b_+OE(30));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(30),b_+OE(32));goto clear;}CYC(b_+O(30),b_+OE(32));
  CALL_C(b_+O(32),isLinkInHole_hook,SYM(isLinkInHole),b_+OE(35));if(F&FC){CYCT(b_+O(35),b_+OE(37));goto clear;}CYC(b_+O(35),b_+OE(37));
  CYC(b_+O(37),b_+OE(40));SET_HL(wLinkSwimmingState);CYC(b_+O(40),b_+OE(41));A=mem_rd(gb,HL);SET_HL(HL+1);CYC(b_+O(41),b_+OE(42));alu_or(gb,mem_rd(gb,HL));if(!(F&FZ)){CYCT(b_+O(42),b_+OE(44));goto clear;}CYC(b_+O(42),b_+OE(44));
  CYC(b_+O(44),b_+OE(47));A=W8(wLinkInAir);CYC(b_+O(47),b_+OE(48));alu_add(gb,A);if(F&FC){CYCT(b_+O(48),b_+OE(50));goto clear;}CYC(b_+O(48),b_+OE(50));
  CYC(b_+O(50),b_+OE(51));alu_add(gb,A);if(F&FC){CYCT(b_+O(51),b_+OE(53));goto state1;}CYC(b_+O(51),b_+OE(53));if(!(F&FZ)){CYCT(b_+O(53),b_+OE(55));goto clear;}CYC(b_+O(53),b_+OE(55));
  CYC(b_+O(55),b_+OE(58));A=W8(w1Link_zh);CYC(b_+O(58),b_+OE(59));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(59),b_+OE(61));goto clear;}CYC(b_+O(59),b_+OE(61));
  CYC(b_+O(61),b_+OE(64));SET_BC(0xfe20);CYC(b_+O(64),b_+OE(67));A=W8(wActiveGroup);CYC(b_+O(67),b_+OE(69));alu_cp(gb,6);
  if(F&FC)CYCT(b_+O(69),b_+OE(71));else{CYC(b_+O(69),b_+OE(71));CYC(b_+O(71),b_+OE(74));SET_BC(0xfdd0);}
  CYC(b_+O(74),b_+OE(77));SET_HL(w1Link_speedZ);CYC(b_+O(77),b_+OE(78));mem_wr(gb,HL,C);CYC(b_+O(78),b_+OE(79));L=alu_inc8(gb,L);CYC(b_+O(79),b_+OE(80));mem_wr(gb,HL,B);
  CYC(b_+O(80),b_+OE(82));A=1;
  if (game_seasons) {
    CYC(b_+S(70), b_+S(73)); A = W8(wFeatherLevel);
    CYC(b_+S(73), b_+S(75)); alu_cp(gb, 0x02);
    CYC(b_+S(75), b_+S(77)); A = 0x41;
    if (F & FZ) { CYCT(b_+S(77), b_+S(79)); goto set_in_air; }
    CYC(b_+S(77), b_+S(79));
  }
  CYC(b_+O(82),b_+OE(84));A=1;
set_in_air:
  CYC(b_+O(84),b_+OE(87));W8(wLinkInAir)=A;
  if(!(F&FZ)){CYCT(b_+O(87),b_+OE(89));goto clear;}CYC(b_+O(87),b_+OE(89));
  CYC(b_+O(89),b_+OE(91));E=4;CYC(b_+O(91),b_+OE(93));A=1;CYC(b_+O(93),b_+OE(94));mem_wr(gb,DE,A);CYC(b_+O(94),b_+OE(95));ret_effect(gb);return;
clear:
  CYC(b_+O(95),b_+OE(98));clearParentItem_hook(gb);
  return;
state1:
  if (!game_seasons) { CYC(b_+98,b_+101);clearParentItem_hook(gb); return; }
  CYC(b_+S(95), b_+S(98)); A = W8(wLinkInAir);
  CYC(b_+S(98), b_+S(100)); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+S(100), b_+S(102)); goto clear; }
  CYC(b_+S(100), b_+S(102));
  CALL_C(b_+S(102), parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+S(105));
  if (F & FZ) { CYCT(b_+S(105), b_+S(107)); goto clear; }
  CYC(b_+S(105), b_+S(107));
  CYC(b_+S(107), b_+S(110)); SET_HL(w1Link_speedZ);
  CYC(b_+S(110), b_+S(111)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+S(111), b_+S(112)); H = mem_rd(gb, HL);
  CYC(b_+S(112), b_+S(114)); alu_bit(gb, 7, H);
  if (!(F & FZ)) { RET_TAKEN(b_+S(114)); return; }
  CYC(b_+S(114), b_+S(115));
  CYC(b_+S(115), b_+S(116)); L = A;
  CYC(b_+S(116), b_+S(119)); SET_BC(0x0100);
  CALL_C(b_+S(119), compareHlToBc_hook, SYM(compareHlToBc), b_+S(122));
  CYC(b_+S(122), b_+S(123)); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+S(123)); return; }
  CYC(b_+S(123), b_+S(124));
  CYC(b_+S(124), b_+S(127)); SET_HL(w1Link_speedZ);
  CYC(b_+S(127), b_+S(129)); mem_wr(gb, HL, 0x80);
  CYC(b_+S(129), b_+S(130)); L = alu_inc8(gb, L);
  CYC(b_+S(130), b_+S(132)); mem_wr(gb, HL, 0xff);
  CYC(b_+S(132), b_+S(133)); push_effect(gb, DE);
  CYC(b_+S(133), b_+S(134)); D = H;
  CYC(b_+S(134), b_+S(136)); A = 0x19;
  CALL_C(b_+S(136), specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+S(139));
  CYC(b_+S(139), b_+S(140)); SET_DE(pop_effect(gb));
  CYC(b_+S(140), b_+S(143)); SET_HL(wLinkInAir);
  CYC(b_+S(143), b_+S(145)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+S(145), b_+S(147)); A = 0x51;
  CALL_C(b_+S(147), playSound_b00_hook, SYM(playSound_b00), b_+S(150));
  CYC(b_+S(150), b_+S(153)); clearParentItem_hook(gb);
}
