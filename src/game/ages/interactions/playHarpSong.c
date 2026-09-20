#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from,to) burn_rom(gb,0x0b,(from),(to),false)
#define CYCT(from,to) burn_rom(gb,0x0b,(from),(to),true)

static uint16_t harp_jump_table(GB *gb) {
  burn_rom(gb,0,0,1,false); alu_add(gb,A); burn_rom(gb,0,1,2,false); SET_HL(pop_effect(gb)); burn_rom(gb,0,2,3,false); alu_add(gb,L); burn_rom(gb,0,3,4,false); L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);return HL;
}
static void harp_add_a(GB *gb,uint16_t ra){push_effect(gb,ra);burn_rom(gb,0,0x10,0x11,false);alu_add(gb,L);burn_rom(gb,0,0x11,0x12,false);L=A;if(!(F&FC)){burn_rom(gb,0,0x12,0x13,true);ret_effect(gb);return;}burn_rom(gb,0,0x12,0x13,false);burn_rom(gb,0,0x13,0x14,false);H=alu_inc8(gb,H);burn_rom(gb,0,0x14,0x15,false);ret_effect(gb);}
void interactionCodec5_hook(GB *gb){uint16_t sp0_=gb->sp;CYC(0x7446,0x7448);E=INTERACTION_BASE+OBJ_STATE;CYC(0x7448,0x7449);A=mem_rd(gb,DE);CYC(0x7449,0x744a);push_effect(gb,0x744a);switch(harp_jump_table(gb)){
case 0x7458:CALL_C(0x7458,setLinkForceStateToState08_hook,0x2aad,0x745b);CYC(0x745b,0x745e);SET_HL(w1Link_yh);CALL_C(0x745e,objectTakePosition_hook,0x2274,0x7461);CYC(0x7461,0x7463);E=0x46;CYC(0x7463,0x7465);A=4;CYC(0x7465,0x7466);mem_wr(gb,DE,A);CYC(0x7466,0x7469);interactionIncState_hook(gb);return;
case 0x7469:CALL_C(0x7469,interactionDecCounter1_hook,0x23cc,0x746c);if(!(F&FZ)){CYCT(0x746c,0x746d);ret_effect(gb);return;}CYC(0x746c,0x746d);CYC(0x746d,0x746f);mem_wr(gb,HL,52);CYC(0x746f,0x7471);A=0x1e;CYC(0x7471,0x7474);W8(wcc50)=A;CALL_C(0x7474,interactionIncState_hook,0x23e0,0x7477);CYC(0x7477,0x7479);E=0x42;CYC(0x7479,0x747a);A=mem_rd(gb,DE);CYC(0x747a,0x747d);SET_HL(0x7482);CYC(0x747d,0x747e);harp_add_a(gb,0x747e);CYC(0x747e,0x747f);A=mem_rd(gb,HL);CYC(0x747f,0x7482);playSound_b00_hook(gb);return;
case 0x7485:case 0x74a9:{bool right=HL==0x74a9;uint16_t a=right?0x74a9:0x7485;CYC(a,a+3);A=W8(wFrameCounter);CYC(a+3,a+5);alu_and(gb,0x1f);if(!(F&FZ)){CYCT(a+5,right?0x7493:0x748c);goto common;}CYC(a+5,right?0x7493:0x748c);CYC(right?0x74b0:0x748c,right?0x74b2:0x748d);A=right?1:0;CYC(right?0x74b2:0x748d,right?0x74b5:0x7490);SET_BC(right?0xf808:0xf8f8);CALL_C(right?0x74b5:0x7490,objectCreateFloatingMusicNote_hook,0x27fe,right?0x74b8:0x7493);if(right)CYC(0x74b8,0x7493);common:CYC(0x7493,0x7494);push_effect(gb,DE);CYC(0x7494,0x7497);SET_DE(w1Link);CYC(0x7497,0x749a);SET_HL(0x2aef);CYC(0x749a,0x749c);E=0;CALL_C(0x749c,interBankCall_hook,0x008a,0x749f);CYC(0x749f,0x74a0);SET_DE(pop_effect(gb));CALL_C(0x74a0,interactionDecCounter1_hook,0x23cc,0x74a3);if(!(F&FZ)){CYCT(0x74a3,0x74a4);ret_effect(gb);return;}CYC(0x74a3,0x74a4);CYC(0x74a4,0x74a6);mem_wr(gb,HL,52);CYC(0x74a6,0x74a9);interactionIncState_hook(gb);return;}
case 0x74ba:CYC(0x74ba,0x74bd);SET_HL(wTmpcfc0_genericCutscene_state);CYC(0x74bd,0x74bf);mem_wr(gb,HL,mem_rd(gb,HL)|0x80);CYC(0x74bf,0x74c1);A=0x10;CYC(0x74c1,0x74c4);W8(wcc50)=A;CYC(0x74c4,0x74c7);interactionDelete_hook(gb);return;default:hook_continue(gb,HL,sp0_);return;}}
