#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(parentItemCode_caneOfSomaria), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(parentItemCode_caneOfSomaria), (from), (to), true)

void parentItemCode_caneOfSomaria_hook(GB *gb) {
  BASE(parentItemCode_caneOfSomaria);
  uint16_t sp0_=gb->sp;
  CYC(b_+0,b_+2); E=4; CYC(b_+2,b_+3); A=mem_rd(gb,DE); CYC(b_+3,b_+4); push_effect(gb,b_+4);
  burn_rom(gb,0,0,1,false); alu_add(gb,A); burn_rom(gb,0,1,2,false); SET_HL(pop_effect(gb)); burn_rom(gb,0,2,3,false); alu_add(gb,L); burn_rom(gb,0,3,4,false); L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);
  if(HL==b_+8){CALL_C(b_+8,updateLinkDirectionFromAngle_hook,SYM(updateLinkDirectionFromAngle),b_+11);CALL_C(b_+11,parentItemLoadAnimationAndIncState_hook,SYM(parentItemLoadAnimationAndIncState),b_+14);CYC(b_+14,b_+17);itemCreateChild_hook(gb);return;}
  if(HL!=b_+17){hook_continue(gb,HL,sp0_);return;}
  CYC(b_+17,b_+19);E=0x21;CYC(b_+19,b_+20);A=mem_rd(gb,DE);CYC(b_+20,b_+21);alu_rlca(gb);
  if(!(F&FC)){CYCT(b_+21,b_+24);specialObjectAnimate_optimized_hook(gb);}else{CYC(b_+21,b_+24);CYC(b_+24,SYM(parentItemCode_sword));clearParentItem_hook(gb);}
}
