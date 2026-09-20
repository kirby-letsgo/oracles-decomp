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
  CYC(b_+0,b_+2);E=4;CYC(b_+2,b_+3);A=mem_rd(gb,DE);CYC(b_+3,b_+4);push_effect(gb,b_+4);
  do { uint16_t jt_ = (feather_parent_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+98) { goto state1; }
    else { hook_continue(gb,HL,sp0_);return; }
  } while (0);
  CALL_C(b_+8,isLinkUnderwater_hook,SYM(isLinkUnderwater),b_+11); if(!(F&FZ)){CYCT(b_+11,b_+13);goto clear;} CYC(b_+11,b_+13);
  CYC(b_+13,b_+16);A=W8(w1ParentItem2_id);CYC(b_+16,b_+18);alu_cp(gb,0x0a);if(F&FZ){CYCT(b_+18,b_+20);goto clear;}CYC(b_+18,b_+20);
  CYC(b_+20,b_+23);A=W8(wLinkObjectIndex);CYC(b_+23,b_+24);alu_rrca(gb);if(F&FC){CYCT(b_+24,b_+26);goto clear;}CYC(b_+24,b_+26);
  CYC(b_+26,b_+29);A=W8(wLinkGrabState);CYC(b_+29,b_+30);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+30,b_+32);goto clear;}CYC(b_+30,b_+32);
  CALL_C(b_+32,isLinkInHole_hook,SYM(isLinkInHole),b_+35);if(F&FC){CYCT(b_+35,b_+37);goto clear;}CYC(b_+35,b_+37);
  CYC(b_+37,b_+40);SET_HL(wLinkSwimmingState);CYC(b_+40,b_+41);A=mem_rd(gb,HL);SET_HL(HL+1);CYC(b_+41,b_+42);alu_or(gb,mem_rd(gb,HL));if(!(F&FZ)){CYCT(b_+42,b_+44);goto clear;}CYC(b_+42,b_+44);
  CYC(b_+44,b_+47);A=W8(wLinkInAir);CYC(b_+47,b_+48);alu_add(gb,A);if(F&FC){CYCT(b_+48,b_+50);goto clear;}CYC(b_+48,b_+50);
  CYC(b_+50,b_+51);alu_add(gb,A);if(F&FC){CYCT(b_+51,b_+53);goto state1;}CYC(b_+51,b_+53);if(!(F&FZ)){CYCT(b_+53,b_+55);goto clear;}CYC(b_+53,b_+55);
  CYC(b_+55,b_+58);A=W8(w1Link_zh);CYC(b_+58,b_+59);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+59,b_+61);goto clear;}CYC(b_+59,b_+61);
  CYC(b_+61,b_+64);SET_BC(0xfe20);CYC(b_+64,b_+67);A=W8(wActiveGroup);CYC(b_+67,b_+69);alu_cp(gb,6);
  if(F&FC)CYCT(b_+69,b_+71);else{CYC(b_+69,b_+71);CYC(b_+71,b_+74);SET_BC(0xfdd0);}
  CYC(b_+74,b_+77);SET_HL(w1Link_speedZ);CYC(b_+77,b_+78);mem_wr(gb,HL,C);CYC(b_+78,b_+79);L=alu_inc8(gb,L);CYC(b_+79,b_+80);mem_wr(gb,HL,B);
  CYC(b_+80,b_+82);A=1;CYC(b_+82,b_+84);A=1;CYC(b_+84,b_+87);W8(wLinkInAir)=A;
  if(!(F&FZ)){CYCT(b_+87,b_+89);goto clear;}CYC(b_+87,b_+89);
  CYC(b_+89,b_+91);E=4;CYC(b_+91,b_+93);A=1;CYC(b_+93,b_+94);mem_wr(gb,DE,A);CYC(b_+94,b_+95);ret_effect(gb);return;
clear:
  CYC(b_+95,b_+98);clearParentItem_hook(gb);
  return;
state1:
  CYC(b_+98,b_+101);clearParentItem_hook(gb);
}
