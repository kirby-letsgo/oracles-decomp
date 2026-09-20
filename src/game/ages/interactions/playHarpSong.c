#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from,to) burn_rom(gb,bk_,(from),(to),false)
#define CYCT(from,to) burn_rom(gb,bk_,(from),(to),true)

static uint16_t harp_jump_table(GB *gb) {
  burn_rom(gb,0,0,1,false); alu_add(gb,A); burn_rom(gb,0,1,2,false); SET_HL(pop_effect(gb)); burn_rom(gb,0,2,3,false); alu_add(gb,L); burn_rom(gb,0,3,4,false); L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);return HL;
}
static void harp_add_a(GB *gb,uint16_t ra){push_effect(gb,ra);burn_rom(gb,0,0x10,0x11,false);alu_add(gb,L);burn_rom(gb,0,0x11,0x12,false);L=A;if(!(F&FC)){burn_rom(gb,0,0x12,0x13,true);ret_effect(gb);return;}burn_rom(gb,0,0x12,0x13,false);burn_rom(gb,0,0x13,0x14,false);H=alu_inc8(gb,H);burn_rom(gb,0,0x14,0x15,false);ret_effect(gb);}
void interactionCodec5_hook(GB *gb){BASE(interactionCodec5);uint16_t sp0_=gb->sp;CYC(b_+0,b_+2);E=INTERACTION_BASE+OBJ_STATE;CYC(b_+2,b_+3);A=mem_rd(gb,DE);CYC(b_+3,b_+4);push_effect(gb,b_+4);do { uint16_t jt_ = (harp_jump_table(gb));
  if (jt_ == b_+18) { CALL_C(b_+18,setLinkForceStateToState08_hook,SYM(setLinkForceStateToState08),b_+21);CYC(b_+21,b_+24);SET_HL(w1Link_yh);CALL_C(b_+24,objectTakePosition_hook,SYM(objectTakePosition),b_+27);CYC(b_+27,b_+29);E=0x46;CYC(b_+29,b_+31);A=4;CYC(b_+31,b_+32);mem_wr(gb,DE,A);CYC(b_+32,b_+35);interactionIncState_hook(gb);return; }
  else if (jt_ == b_+35) { CALL_C(b_+35,interactionDecCounter1_hook,SYM(interactionDecCounter1),b_+38);if(!(F&FZ)){CYCT(b_+38,b_+39);ret_effect(gb);return;}CYC(b_+38,b_+39);CYC(b_+39,b_+41);mem_wr(gb,HL,52);CYC(b_+41,b_+43);A=0x1e;CYC(b_+43,b_+46);W8(wcc50)=A;CALL_C(b_+46,interactionIncState_hook,SYM(interactionIncState),b_+49);CYC(b_+49,b_+51);E=0x42;CYC(b_+51,b_+52);A=mem_rd(gb,DE);CYC(b_+52,b_+55);SET_HL(b_+60);CYC(b_+55,b_+56);harp_add_a(gb,b_+56);CYC(b_+56,b_+57);A=mem_rd(gb,HL);CYC(b_+57,b_+60);playSound_b00_hook(gb);return; }
  else if (jt_ == b_+63 || jt_ == b_+99) { {bool right=HL==b_+99;uint16_t a=right?b_+99:b_+63;CYC(a,a+3);A=W8(wFrameCounter);CYC(a+3,a+5);alu_and(gb,0x1f);if(!(F&FZ)){CYCT(a+5,right?b_+77:b_+70);goto common;}CYC(a+5,right?b_+77:b_+70);CYC(right?b_+106:b_+70,right?b_+108:b_+71);A=right?1:0;CYC(right?b_+108:b_+71,right?b_+111:b_+74);SET_BC(right?0xf808:0xf8f8);CALL_C(right?b_+111:b_+74,objectCreateFloatingMusicNote_hook,SYM(objectCreateFloatingMusicNote),right?b_+114:b_+77);if(right)CYC(b_+114,b_+77);common:CYC(b_+77,b_+78);push_effect(gb,DE);CYC(b_+78,b_+81);SET_DE(w1Link);CYC(b_+81,b_+84);SET_HL(SYM(specialObjectAnimate));CYC(b_+84,b_+86);E=0;CALL_C(b_+86,interBankCall_hook,0x008a,b_+89);CYC(b_+89,b_+90);SET_DE(pop_effect(gb));CALL_C(b_+90,interactionDecCounter1_hook,SYM(interactionDecCounter1),b_+93);if(!(F&FZ)){CYCT(b_+93,b_+94);ret_effect(gb);return;}CYC(b_+93,b_+94);CYC(b_+94,b_+96);mem_wr(gb,HL,52);CYC(b_+96,b_+99);interactionIncState_hook(gb);return;} }
  else if (jt_ == b_+116) { CYC(b_+116,b_+119);SET_HL(wTmpcfc0_genericCutscene_state);CYC(b_+119,b_+121);mem_wr(gb,HL,mem_rd(gb,HL)|0x80);CYC(b_+121,b_+123);A=0x10;CYC(b_+123,b_+126);W8(wcc50)=A;CYC(b_+126,b_+129);interactionDelete_hook(gb);return; }
  else { hook_continue(gb,HL,sp0_);return; }
} while (0);}
