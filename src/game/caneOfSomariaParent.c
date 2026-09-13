#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void parentItemCode_caneOfSomaria_hook(GB *gb) {
  uint16_t sp0_=gb->sp;
  CYC(0x4b67,0x4b69); E=4; CYC(0x4b69,0x4b6a); A=mem_rd(gb,DE); CYC(0x4b6a,0x4b6b); push_effect(gb,0x4b6b);
  burn_rom(gb,0,0,1,false); alu_add(gb,A); burn_rom(gb,0,1,2,false); SET_HL(pop_effect(gb)); burn_rom(gb,0,2,3,false); alu_add(gb,L); burn_rom(gb,0,3,4,false); L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);
  if(HL==0x4b6f){CALL_C(0x4b6f,updateLinkDirectionFromAngle_hook,0x2b64,0x4b72);CALL_C(0x4b72,parentItemLoadAnimationAndIncState_hook,0x5378,0x4b75);CYC(0x4b75,0x4b78);itemCreateChild_hook(gb);return;}
  if(HL!=0x4b78){hook_continue(gb,HL,sp0_);return;}
  CYC(0x4b78,0x4b7a);E=0x21;CYC(0x4b7a,0x4b7b);A=mem_rd(gb,DE);CYC(0x4b7b,0x4b7c);alu_rlca(gb);
  if(!(F&FC)){CYCT(0x4b7c,0x4b7f);specialObjectAnimate_optimized_hook(gb);}else{CYC(0x4b7c,0x4b7f);CYC(0x4b7f,0x4b82);clearParentItem_hook(gb);}
}
