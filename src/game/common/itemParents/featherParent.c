#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t feather_parent_jump_table(GB *gb) {
  burn_rom(gb,0,0,1,false);alu_add(gb,A);burn_rom(gb,0,1,2,false);SET_HL(pop_effect(gb));burn_rom(gb,0,2,3,false);alu_add(gb,L);burn_rom(gb,0,3,4,false);L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);return HL;
}

void parentItemCode_feather_hook(GB *gb) {
  uint16_t sp0_=gb->sp;
  CYC(0x52f3,0x52f5);E=4;CYC(0x52f5,0x52f6);A=mem_rd(gb,DE);CYC(0x52f6,0x52f7);push_effect(gb,0x52f7);
  switch(feather_parent_jump_table(gb)){case 0x52fb:break;case 0x5355:goto state1;default:hook_continue(gb,HL,sp0_);return;}
  CALL_C(0x52fb,isLinkUnderwater_hook,0x54d2,0x52fe); if(!(F&FZ)){CYCT(0x52fe,0x5300);goto clear;} CYC(0x52fe,0x5300);
  CYC(0x5300,0x5303);A=W8(w1ParentItem2_id);CYC(0x5303,0x5305);alu_cp(gb,0x0a);if(F&FZ){CYCT(0x5305,0x5307);goto clear;}CYC(0x5305,0x5307);
  CYC(0x5307,0x530a);A=W8(wLinkObjectIndex);CYC(0x530a,0x530b);alu_rrca(gb);if(F&FC){CYCT(0x530b,0x530d);goto clear;}CYC(0x530b,0x530d);
  CYC(0x530d,0x5310);A=W8(wLinkGrabState);CYC(0x5310,0x5311);alu_or(gb,A);if(!(F&FZ)){CYCT(0x5311,0x5313);goto clear;}CYC(0x5311,0x5313);
  CALL_C(0x5313,isLinkInHole_hook,0x54d8,0x5316);if(F&FC){CYCT(0x5316,0x5318);goto clear;}CYC(0x5316,0x5318);
  CYC(0x5318,0x531b);SET_HL(wLinkSwimmingState);CYC(0x531b,0x531c);A=mem_rd(gb,HL);SET_HL(HL+1);CYC(0x531c,0x531d);alu_or(gb,mem_rd(gb,HL));if(!(F&FZ)){CYCT(0x531d,0x531f);goto clear;}CYC(0x531d,0x531f);
  CYC(0x531f,0x5322);A=W8(wLinkInAir);CYC(0x5322,0x5323);alu_add(gb,A);if(F&FC){CYCT(0x5323,0x5325);goto clear;}CYC(0x5323,0x5325);
  CYC(0x5325,0x5326);alu_add(gb,A);if(F&FC){CYCT(0x5326,0x5328);goto state1;}CYC(0x5326,0x5328);if(!(F&FZ)){CYCT(0x5328,0x532a);goto clear;}CYC(0x5328,0x532a);
  CYC(0x532a,0x532d);A=W8(w1Link_zh);CYC(0x532d,0x532e);alu_or(gb,A);if(!(F&FZ)){CYCT(0x532e,0x5330);goto clear;}CYC(0x532e,0x5330);
  CYC(0x5330,0x5333);SET_BC(0xfe20);CYC(0x5333,0x5336);A=W8(wActiveGroup);CYC(0x5336,0x5338);alu_cp(gb,6);
  if(F&FC)CYCT(0x5338,0x533a);else{CYC(0x5338,0x533a);CYC(0x533a,0x533d);SET_BC(0xfdd0);}
  CYC(0x533d,0x5340);SET_HL(w1Link_speedZ);CYC(0x5340,0x5341);mem_wr(gb,HL,C);CYC(0x5341,0x5342);L=alu_inc8(gb,L);CYC(0x5342,0x5343);mem_wr(gb,HL,B);
  CYC(0x5343,0x5345);A=1;CYC(0x5345,0x5347);A=1;CYC(0x5347,0x534a);W8(wLinkInAir)=A;
  if(!(F&FZ)){CYCT(0x534a,0x534c);goto clear;}CYC(0x534a,0x534c);
  CYC(0x534c,0x534e);E=4;CYC(0x534e,0x5350);A=1;CYC(0x5350,0x5351);mem_wr(gb,DE,A);CYC(0x5351,0x5352);ret_effect(gb);return;
clear:
  CYC(0x5352,0x5355);clearParentItem_hook(gb);
  return;
state1:
  CYC(0x5355,0x5358);clearParentItem_hook(gb);
}
