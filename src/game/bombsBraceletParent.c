#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t bombs_bracelet_jump_table(GB *gb) {
  burn_rom(gb,0,0,1,false);alu_add(gb,A);burn_rom(gb,0,1,2,false);SET_HL(pop_effect(gb));burn_rom(gb,0,2,3,false);alu_add(gb,L);burn_rom(gb,0,3,4,false);L=A;
  if(F&FC){burn_rom(gb,0,4,6,false);burn_rom(gb,0,6,7,false);H=alu_inc8(gb,H);}else burn_rom(gb,0,4,6,true);
  burn_rom(gb,0,7,8,false);A=mem_rd(gb,HL);SET_HL(HL+1);burn_rom(gb,0,8,9,false);H=mem_rd(gb,HL);burn_rom(gb,0,9,10,false);L=A;burn_rom(gb,0,10,11,false);return HL;
}

static void bombs_bracelet_add_a_to_hl(GB *gb) {
  burn_rom(gb,0,0x10,0x11,false);alu_add(gb,L);burn_rom(gb,0,0x11,0x12,false);L=A;
  if(F&FC){burn_rom(gb,0,0x12,0x13,false);burn_rom(gb,0,0x13,0x14,false);H=alu_inc8(gb,H);burn_rom(gb,0,0x14,0x15,false);ret_effect(gb);}
  else{burn_rom(gb,0,0x12,0x13,true);ret_effect(gb);}
}

void bracelet_drop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x52a0,dropLinkHeldItem_hook,0x2c43,0x52a3);
  CYC(0x52a3,0x52a6);clearParentItem_hook(gb);
}

static bool bracelet_delete_if_invalid(GB *gb, uint16_t ret) {
  CYC(ret-3,ret);push_effect(gb,ret);
  CYC(0x5294,0x5297);A=W8(wLinkSwimmingState);CYC(0x5297,0x5298);alu_or(gb,A);
  if(!(F&FZ))CYCT(0x5298,0x529a);else{
    CYC(0x5298,0x529a);CYC(0x529a,0x529d);A=W8(wLinkGrabState);CYC(0x529d,0x529e);alu_or(gb,A);
    if(!(F&FZ)){CYCT(0x529e,0x529f);ret_effect(gb);return false;}CYC(0x529e,0x529f);
  }
  CYC(0x529f,0x52a0);SET_AF(pop_effect(gb));bracelet_drop_hook(gb);return true;
}

static void bracelet_check_wall(GB *gb, uint16_t call, uint16_t ret) {
  CYC(call,ret);push_effect(gb,ret);
  CYC(0x52a6,0x52a9);A=W8(w1Link_direction);CYC(0x52a9,0x52aa);B=A;CYC(0x52aa,0x52ab);alu_add(gb,A);CYC(0x52ab,0x52ac);alu_add(gb,B);
  CYC(0x52ac,0x52af);SET_HL(0x52c4);CYC(0x52af,0x52b0);push_effect(gb,0x52b0);bombs_bracelet_add_a_to_hl(gb);
  CYC(0x52b0,0x52b3);A=W8(w1Link_adjacentWallsBitset);CYC(0x52b3,0x52b4);alu_and(gb,mem_rd(gb,HL));CYC(0x52b4,0x52b5);alu_cp(gb,mem_rd(gb,HL));
  if(!(F&FZ)){CYCT(0x52b5,0x52b6);ret_effect(gb);return;}CYC(0x52b5,0x52b6);
  CYC(0x52b6,0x52b7);SET_HL(HL+1);CYC(0x52b7,0x52ba);A=W8(w1Link_yh);CYC(0x52ba,0x52bb);alu_add(gb,mem_rd(gb,HL));CYC(0x52bb,0x52bc);B=A;
  CYC(0x52bc,0x52bd);SET_HL(HL+1);CYC(0x52bd,0x52c0);A=W8(w1Link_xh);CYC(0x52c0,0x52c1);alu_add(gb,mem_rd(gb,HL));CYC(0x52c1,0x52c2);C=A;CYC(0x52c2,0x52c3);alu_xor(gb,A);CYC(0x52c3,0x52c4);ret_effect(gb);
}

void makeLinkPickupObjectH_hook(GB *gb) {
  CYC(0x50d3,0x50d5);L=0;CYC(0x50d5,0x50d7);mem_wr(gb,HL,mem_rd(gb,HL)|(1<<1));CYC(0x50d7,0x50d9);L=5;CYC(0x50d9,0x50da);alu_xor(gb,A);
  CYC(0x50da,0x50db);mem_wr(gb,HL,A);SET_HL(HL-1);CYC(0x50db,0x50dd);mem_wr(gb,HL,2);CYC(0x50dd,0x50e0);W8(w1Link_relatedObj2)=A;CYC(0x50e0,0x50e1);A=H;CYC(0x50e1,0x50e4);mem_wr(gb,w1Link_relatedObj2+1,A);CYC(0x50e4,0x50e5);alu_or(gb,A);CYC(0x50e5,0x50e6);ret_effect(gb);
}

void try_pickup_object_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50c5,0x50c7);L=0x2f;CYC(0x50c7,0x50c8);A=mem_rd(gb,HL);CYC(0x50c8,0x50ca);alu_and(gb,0xb0);
  if(!(F&FZ)){CYCT(0x50ca,0x50cc);goto fail;}CYC(0x50ca,0x50cc);
  CALL_C(0x50cc,objectHCheckCollisionWithLink_hook,0x1c89,0x50cf);
  if(F&FC){CYCT(0x50cf,0x50d1);makeLinkPickupObjectH_hook(gb);return;}CYC(0x50cf,0x50d1);
fail:CYC(0x50d1,0x50d2);alu_xor(gb,A);CYC(0x50d2,0x50d3);ret_effect(gb);
}

void tryPickupBombs_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50ad,0x50b0);A=W8(wLinkUsingItem1);CYC(0x50b0,0x50b1);alu_or(gb,A);if(!(F&FZ)){CYCT(0x50b1,0x50b3);goto fail;}CYC(0x50b1,0x50b3);
  CYC(0x50b3,0x50b5);C=3;CALL_C(0x50b5,findItemWithID_hook,0x22b9,0x50b8);if(!(F&FZ)){CYCT(0x50b8,0x50ba);goto fail;}CYC(0x50b8,0x50ba);
  CYC(0x50ba,0x50bd);push_effect(gb,0x50bd);try_pickup_object_hook(gb);if(!(F&FZ)){CYCT(0x50bd,0x50be);ret_effect(gb);return;}CYC(0x50bd,0x50be);
  CYC(0x50be,0x50c0);C=3;CALL_C(0x50c0,findItemWithID_startingAfterH_hook,0x22c0,0x50c3);if(!(F&FZ)){CYCT(0x50c3,0x50c5);goto fail;}CYC(0x50c3,0x50c5);
  try_pickup_object_hook(gb);return;
fail:CYC(0x50d1,0x50d2);alu_xor(gb,A);CYC(0x50d2,0x50d3);ret_effect(gb);
}

void bracelet_begin_pickup_hook(GB *gb);

void bracelet_begin_pickup_animated_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5188,0x518a);A=0x15;CALL_C(0x518a,specialObjectSetAnimationWithLinkData_hook,0x4412,0x518d);
  bracelet_begin_pickup_hook(gb);
}

void bracelet_begin_pickup_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x518d,itemDisableLinkMovement_hook,0x545d,0x5190);CALL_C(0x5190,itemDisableLinkTurning_hook,0x5470,0x5193);
  CYC(0x5193,0x5195);A=0xc2;CYC(0x5195,0x5198);W8(wLinkGrabState)=A;CYC(0x5198,0x5199);alu_xor(gb,A);CYC(0x5199,0x519c);W8(wLinkGrabState2)=A;
  CYC(0x519c,0x519f);SET_HL(w1Link_collisionType);CYC(0x519f,0x51a1);mem_wr(gb,HL,mem_rd(gb,HL)&~(1<<7));CYC(0x51a1,0x51a3);A=2;CYC(0x51a3,0x51a5);E=4;CYC(0x51a5,0x51a6);mem_wr(gb,DE,A);
  CYC(0x51a6,0x51a8);E=0x3f;CYC(0x51a8,0x51aa);A=0x0f;CYC(0x51aa,0x51ab);mem_wr(gb,DE,A);CYC(0x51ab,0x51ad);A=0x9c;CYC(0x51ad,0x51b0);playSound_b00_hook(gb);
}

void bracelet_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if(bracelet_delete_if_invalid(gb,0x51b7))return;
  CALL_C(0x51b7,specialObjectAnimate_optimized_hook,0x441e,0x51ba);CYC(0x51ba,0x51bd);A=W8(wLinkGrabState2);CYC(0x51bd,0x51be);alu_rlca(gb);
  if(F&FC){CYC(0x51be,0x51c0);CYC(0x51c0,0x51c2);A=0x83;CYC(0x51c2,0x51c5);W8(wLinkGrabState)=A;CYC(0x51c5,0x51c7);E=4;CYC(0x51c7,0x51c9);A=5;CYC(0x51c9,0x51ca);mem_wr(gb,DE,A);CYC(0x51ca,0x51cc);A=0x13;CYC(0x51cc,0x51cf);specialObjectSetAnimationWithLinkData_hook(gb);return;}CYCT(0x51be,0x51c0);
  CYC(0x51cf,0x51d0);H=D;CYC(0x51d0,0x51d2);L=0x21;CYC(0x51d2,0x51d4);alu_bit(gb,7,mem_rd(gb,HL));
  if(F&FZ){CYC(0x51d4,0x51d6);CYC(0x51d6,0x51d9);A=W8(wLinkGrabState2);CYC(0x51d9,0x51db);alu_and(gb,0xf0);CYC(0x51db,0x51dc);alu_add(gb,mem_rd(gb,HL));CYC(0x51dc,0x51df);W8(wLinkGrabState2)=A;CYC(0x51df,0x51e0);ret_effect(gb);return;}CYCT(0x51d4,0x51d6);
  CYC(0x51e0,0x51e2);A=0x83;CYC(0x51e2,0x51e5);W8(wLinkGrabState)=A;CYC(0x51e5,0x51e7);L=4;CYC(0x51e7,0x51e8);mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(0x51e8,0x51ea);L=0x3f;CYC(0x51ea,0x51ec);mem_wr(gb,HL,0);
  CYC(0x51ec,0x51ef);SET_HL(w1Link_collisionType);CYC(0x51ef,0x51f1);mem_wr(gb,HL,mem_rd(gb,HL)|(1<<7));CALL_C(0x51f1,itemEnableLinkTurning_hook,0x5479,0x51f4);CYC(0x51f4,0x51f7);itemEnableLinkMovement_hook(gb);
}

void bracelet_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if(bracelet_delete_if_invalid(gb,0x51fa))return;
  CYC(0x51fa,0x51fd);A=W8(wLinkInAir);CYC(0x51fd,0x51fe);alu_rlca(gb);if(F&FC){CYCT(0x51fe,0x51ff);ret_effect(gb);return;}CYC(0x51fe,0x51ff);
  CYC(0x51ff,0x5202);A=W8(wcc67);CYC(0x5202,0x5203);alu_or(gb,A);if(!(F&FZ)){CYCT(0x5203,0x5204);ret_effect(gb);return;}CYC(0x5203,0x5204);
  CYC(0x5204,0x5207);A=W8(w1Link_var2a);CYC(0x5207,0x5208);alu_or(gb,A);if(!(F&FZ))CYCT(0x5208,0x520a);else{CYC(0x5208,0x520a);CYC(0x520a,0x520d);A=W8(wGameKeysJustPressed);CYC(0x520d,0x520f);alu_and(gb,3);if(F&FZ){CYCT(0x520f,0x5210);ret_effect(gb);return;}CYC(0x520f,0x5210);CALL_C(0x5210,updateLinkDirectionFromAngle_hook,0x2b64,0x5213);}
  CYC(0x5213,0x5216);SET_HL(w1Link_relatedObj2);CYC(0x5216,0x5217);alu_xor(gb,A);CYC(0x5217,0x5218);C=mem_rd(gb,HL);CYC(0x5218,0x5219);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x5219,0x521a);B=mem_rd(gb,HL);CYC(0x521a,0x521b);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x521b,0x521c);A=C;CYC(0x521c,0x521e);alu_add(gb,5);CYC(0x521e,0x521f);L=A;CYC(0x521f,0x5220);H=B;CYC(0x5220,0x5222);mem_wr(gb,HL,2);
  CYC(0x5222,0x5224);E=0x37;CYC(0x5224,0x5225);A=mem_rd(gb,DE);CYC(0x5225,0x5226);alu_or(gb,A);if(!(F&FZ))CYCT(0x5226,0x5228);else{CYC(0x5226,0x5228);CYC(0x5228,0x5229);A=C;CYC(0x5229,0x522a);alu_or(gb,A);bool create=!(F&FZ);if(create)CYCT(0x522a,0x522c);else{CYC(0x522a,0x522c);CYC(0x522c,0x522d);A=B;CYC(0x522d,0x522f);alu_cp(gb,0xd7);if(!(F&FC)){CYCT(0x522f,0x5231);create=false;}else{CYC(0x522f,0x5231);create=true;}}if(create){CYC(0x5231,0x5232);push_effect(gb,DE);CYC(0x5232,0x5235);SET_HL(w1ReservedItemC);CYC(0x5235,0x5236);mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(0x5236,0x5237);L=alu_inc8(gb,L);CYC(0x5237,0x5239);A=0x16;CYC(0x5239,0x523a);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x523a,0x523c);L=0x18;CYC(0x523c,0x523d);A=C;CYC(0x523d,0x523e);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x523e,0x523f);mem_wr(gb,HL,B);CYC(0x523f,0x5241);alu_add(gb,0x0b);CYC(0x5241,0x5242);E=A;CYC(0x5242,0x5243);D=B;CALL_C(0x5243,objectCopyPosition_rawAddress_hook,0x2247,0x5246);CYC(0x5246,0x5247);SET_DE(pop_effect(gb));}}
  CYC(0x5247,0x524a);A=W8(wLinkAngle);CYC(0x524a,0x524b);alu_rlca(gb);if(F&FC)CYCT(0x524b,0x524d);else{CYC(0x524b,0x524d);CYC(0x524d,0x5250);A=W8(w1Link_direction);CYC(0x5250,0x5252);A=alu_swap(gb,A);CYC(0x5252,0x5253);alu_rrca(gb);}
  CYC(0x5253,0x5255);L=9;CYC(0x5255,0x5256);mem_wr(gb,HL,A);CYC(0x5256,0x5258);L=0x38;CYC(0x5258,0x525b);A=W8(wLinkGrabState2);CYC(0x525b,0x525c);mem_wr(gb,HL,A);CYC(0x525c,0x525d);alu_xor(gb,A);CYC(0x525d,0x5260);W8(wLinkGrabState2)=A;CYC(0x5260,0x5263);W8(wLinkGrabState)=A;
  CYC(0x5263,0x5264);H=D;CYC(0x5264,0x5266);L=4;CYC(0x5266,0x5267);mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(0x5267,0x5269);L=0x3f;CYC(0x5269,0x526b);mem_wr(gb,HL,0x0f);CYC(0x526b,0x526d);C=0x16;
  CYC(0x526d,0x5270);A=W8(w1Companion_id);CYC(0x5270,0x5272);alu_cp(gb,0x0a);if(!(F&FZ))CYCT(0x5272,0x5274);else{CYC(0x5272,0x5274);CYC(0x5274,0x5277);A=W8(wLinkObjectIndex);CYC(0x5277,0x5278);alu_rrca(gb);if(!(F&FC))CYCT(0x5278,0x527a);else{CYC(0x5278,0x527a);CYC(0x527a,0x527c);C=0x25;}}
  CYC(0x527c,0x527d);A=C;CALL_C(0x527d,specialObjectSetAnimationWithLinkData_hook,0x4412,0x5280);CALL_C(0x5280,itemDisableLinkMovement_hook,0x545d,0x5283);CALL_C(0x5283,itemDisableLinkTurning_hook,0x5470,0x5286);CYC(0x5286,0x5288);A=0x51;CYC(0x5288,0x528b);playSound_b00_hook(gb);
}

void bracelet_state4_hook(GB *gb){CYC(0x528b,0x528d);E=0x21;CYC(0x528d,0x528e);A=mem_rd(gb,DE);CYC(0x528e,0x528f);alu_rlca(gb);if(!(F&FC)){CYCT(0x528f,0x5292);specialObjectAnimate_optimized_hook(gb);return;}CYC(0x528f,0x5292);CYC(0x5292,0x5294);bracelet_drop_hook(gb);}
void bracelet_state5_hook(GB *gb){uint16_t sp0_=gb->sp;CALL_C(0x52d0,parentItemCheckButtonPressed_hook,0x5496,0x52d3);if(F&FZ){CYCT(0x52d3,0x52d6);bracelet_drop_hook(gb);return;}CYC(0x52d3,0x52d6);if(bracelet_delete_if_invalid(gb,0x52d9))return;CYC(0x52d9,0x52dc);A=W8(w1Link_knockbackCounter);CYC(0x52dc,0x52dd);alu_or(gb,A);if(!(F&FZ)){CYCT(0x52dd,0x52e0);bracelet_drop_hook(gb);return;}CYC(0x52dd,0x52e0);CYC(0x52e0,0x52e3);A=W8(w1Link_direction);CYC(0x52e3,0x52e6);SET_HL(0x51b0);CYC(0x52e6,0x52e7);push_effect(gb,0x52e7);bombs_bracelet_add_a_to_hl(gb);CYC(0x52e7,0x52ea);A=W8(wGameKeysPressed);CYC(0x52ea,0x52eb);alu_and(gb,mem_rd(gb,HL));CYC(0x52eb,0x52ed);A=0x13;if(F&FZ){CYCT(0x52ed,0x52f0);specialObjectSetAnimationWithLinkData_hook(gb);}else{CYC(0x52ed,0x52f0);CYC(0x52f0,0x52f3);specialObjectAnimate_optimized_hook(gb);}}

void bracelet_state0_hook(GB *gb){uint16_t sp0_=gb->sp;CALL_C(0x50f6,checkLinkOnGround_hook,0x54c4,0x50f9);if(!(F&FZ)){CYCT(0x50f9,0x50fc);clearParentItem_hook(gb);return;}CYC(0x50f9,0x50fc);CYC(0x50fc,0x50ff);A=W8(w1ReservedItemC);CYC(0x50ff,0x5100);alu_or(gb,A);if(!(F&FZ)){CYCT(0x5100,0x5103);clearParentItem_hook(gb);return;}CYC(0x5100,0x5103);CALL_C(0x5103,parentItemCheckButtonPressed_hook,0x5496,0x5106);if(F&FZ){CYCT(0x5106,0x5109);bracelet_drop_hook(gb);return;}CYC(0x5106,0x5109);CYC(0x5109,0x510c);A=W8(wLinkUsingItem1);CYC(0x510c,0x510d);alu_or(gb,A);if(!(F&FZ)){CYCT(0x510d,0x510f);goto nothing;}CYC(0x510d,0x510f);CALL_C(0x510f,checkGrabbableObjects_hook,0x1c97,0x5112);if(F&FC){CYCT(0x5112,0x5114);bracelet_begin_pickup_animated_hook(gb);return;}CYC(0x5112,0x5114);CALL_C(0x5114,tryPickupBombs_hook,0x50ad,0x5117);if(!(F&FZ)){CYCT(0x5117,0x5119);bracelet_begin_pickup_animated_hook(gb);return;}CYC(0x5117,0x5119);bracelet_check_wall(gb,0x5119,0x511c);if(!(F&FZ)){CYCT(0x511c,0x511e);goto nothing;}CYC(0x511c,0x511e);CYC(0x511e,0x5120);A=0x41;CYC(0x5120,0x5123);W8(wLinkGrabState)=A;CYC(0x5123,0x5126);parentItemLoadAnimationAndIncState_hook(gb);return;nothing:CYC(0x5126,0x5129);A=W8(w1Link_direction);CYC(0x5129,0x512b);alu_or(gb,0x80);CYC(0x512b,0x512e);W8(wBraceletGrabbingNothing)=A;CYC(0x512e,0x512f);ret_effect(gb);}

void bracelet_state1_hook(GB *gb){uint16_t sp0_=gb->sp;if(bracelet_delete_if_invalid(gb,0x5132))return;CYC(0x5132,0x5135);A=W8(w1Link_knockbackCounter);CYC(0x5135,0x5136);alu_or(gb,A);if(!(F&FZ)){CYCT(0x5136,0x5139);bracelet_drop_hook(gb);return;}CYC(0x5136,0x5139);CALL_C(0x5139,parentItemCheckButtonPressed_hook,0x5496,0x513c);if(F&FZ){CYCT(0x513c,0x513f);bracelet_drop_hook(gb);return;}CYC(0x513c,0x513f);CYC(0x513f,0x5142);A=W8(wLinkInAir);CYC(0x5142,0x5143);alu_or(gb,A);if(!(F&FZ)){CYCT(0x5143,0x5146);bracelet_drop_hook(gb);return;}CYC(0x5143,0x5146);bracelet_check_wall(gb,0x5146,0x5149);if(!(F&FZ)){CYCT(0x5149,0x514c);bracelet_drop_hook(gb);return;}CYC(0x5149,0x514c);CYC(0x514c,0x514f);A=W8(w1Link_direction);CYC(0x514f,0x5152);SET_HL(0x51b0);CYC(0x5152,0x5153);push_effect(gb,0x5153);bombs_bracelet_add_a_to_hl(gb);CALL_C(0x5153,andHlWithGameKeysPressed_hook,0x5499,0x5156);CYC(0x5156,0x5158);A=0x14;if(F&FZ){CYCT(0x5158,0x515b);specialObjectSetAnimationWithLinkData_hook(gb);return;}CYC(0x5158,0x515b);CALL_C(0x515b,specialObjectAnimate_optimized_hook,0x441e,0x515e);CYC(0x515e,0x5160);E=0x21;CYC(0x5160,0x5161);A=mem_rd(gb,DE);CYC(0x5161,0x5162);alu_rlca(gb);if(!(F&FC)){CYCT(0x5162,0x5163);ret_effect(gb);return;}CYC(0x5162,0x5163);bracelet_check_wall(gb,0x5163,0x5166);if(!(F&FZ)){CYCT(0x5166,0x5169);bracelet_drop_hook(gb);return;}CYC(0x5166,0x5169);CYC(0x5169,0x516a);alu_xor(gb,A);CALL_C(0x516a,tryToBreakTile_hook,0x2bf6,0x516d);if(!(F&FC)){CYCT(0x516d,0x516e);ret_effect(gb);return;}CYC(0x516d,0x516e);CYC(0x516e,0x5171);SET_HL(w1ReservedItemC);CYC(0x5171,0x5173);A=3;CYC(0x5173,0x5174);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x5174,0x5176);mem_wr(gb,HL,0x16);CYC(0x5176,0x5177);L=alu_inc8(gb,L);CYC(0x5177,0x5179);A=mem_rd(gb,hFF92);CYC(0x5179,0x517a);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x517a,0x517c);E=0x37;CYC(0x517c,0x517d);mem_wr(gb,DE,A);CYC(0x517d,0x517f);A=mem_rd(gb,hFF8E);CYC(0x517f,0x5180);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(0x5180,0x5181);alu_xor(gb,A);CYC(0x5181,0x5184);W8(w1Link_relatedObj2)=A;CYC(0x5184,0x5185);A=H;CYC(0x5185,0x5188);mem_wr(gb,w1Link_relatedObj2+1,A);bracelet_begin_pickup_animated_hook(gb);}

void parentItemCode_bracelet_hook(GB *gb){uint16_t sp0_=gb->sp;CYC(0x50e6,0x50e8);E=4;CYC(0x50e8,0x50e9);A=mem_rd(gb,DE);CYC(0x50e9,0x50ea);push_effect(gb,0x50ea);switch(bombs_bracelet_jump_table(gb)){case 0x50f6:bracelet_state0_hook(gb);return;case 0x512f:bracelet_state1_hook(gb);return;case 0x51b4:bracelet_state2_hook(gb);return;case 0x51f7:bracelet_state3_hook(gb);return;case 0x528b:bracelet_state4_hook(gb);return;case 0x52d0:bracelet_state5_hook(gb);return;default:hook_continue(gb,HL,sp0_);}}

void parentItemCode_bombchu_hook(GB *gb){uint16_t sp0_=gb->sp;CYC(0x502e,0x5030);E=4;CYC(0x5030,0x5031);A=mem_rd(gb,DE);CYC(0x5031,0x5032);push_effect(gb,0x5032);uint16_t target=bombs_bracelet_jump_table(gb);if(target!=0x5036){hook_continue(gb,target,sp0_);return;}CALL_C(0x5036,isLinkUnderwater_hook,0x54d2,0x5039);if(!(F&FZ)){CYCT(0x5039,0x503c);clearParentItem_hook(gb);return;}CYC(0x5039,0x503c);CYC(0x503c,0x503f);A=W8(w1Companion_id);CYC(0x503f,0x5041);alu_cp(gb,0x13);if(F&FZ){CYCT(0x5041,0x5044);clearParentItem_hook(gb);return;}CYC(0x5041,0x5044);CYC(0x5044,0x5047);A=W8(wLinkSwimmingState);CYC(0x5047,0x5048);alu_or(gb,A);if(!(F&FZ)){CYCT(0x5048,0x504b);clearParentItem_hook(gb);return;}CYC(0x5048,0x504b);CYC(0x504b,0x504e);A=W8(wNumBombchus);CYC(0x504e,0x504f);alu_or(gb,A);if(F&FZ){CYCT(0x504f,0x5052);clearParentItem_hook(gb);return;}CYC(0x504f,0x5052);CALL_C(0x5052,parentItemLoadAnimationAndIncState_hook,0x5378,0x5055);CYC(0x5055,0x5057);E=1;CYC(0x5057,0x505a);itemCreateChildAndDeleteOnFailure_hook(gb);}

void parentItemCode_bomb_hook(GB *gb){uint16_t sp0_=gb->sp;CYC(0x505a,0x505c);E=4;CYC(0x505c,0x505d);A=mem_rd(gb,DE);CYC(0x505d,0x505e);push_effect(gb,0x505e);switch(bombs_bracelet_jump_table(gb)){case 0x5068:break;case 0x51b4:bracelet_state2_hook(gb);return;case 0x51f7:bracelet_state3_hook(gb);return;case 0x528b:bracelet_state4_hook(gb);return;default:hook_continue(gb,HL,sp0_);return;}CALL_C(0x5068,isLinkUnderwater_hook,0x54d2,0x506b);if(!(F&FZ)){CYCT(0x506b,0x506e);clearParentItem_hook(gb);return;}CYC(0x506b,0x506e);CYC(0x506e,0x5071);A=W8(w1Companion_id);CYC(0x5071,0x5073);alu_cp(gb,0x13);if(F&FZ)CYCT(0x5073,0x5075);else{CYC(0x5073,0x5075);CYC(0x5075,0x5078);A=W8(wLinkObjectIndex);CYC(0x5078,0x5079);alu_rrca(gb);if(F&FC){CYCT(0x5079,0x507c);clearParentItem_hook(gb);return;}CYC(0x5079,0x507c);}CYC(0x507c,0x507f);A=W8(wLinkSwimmingState);CYC(0x507f,0x5080);B=A;CYC(0x5080,0x5083);A=W8(wLinkInAir);CYC(0x5083,0x5084);alu_or(gb,B);if(!(F&FZ)){CYCT(0x5084,0x5087);clearParentItem_hook(gb);return;}CYC(0x5084,0x5087);CALL_C(0x5087,tryPickupBombs_hook,0x50ad,0x508a);if(!(F&FZ)){CYCT(0x508a,0x508d);bracelet_begin_pickup_animated_hook(gb);return;}CYC(0x508a,0x508d);CYC(0x508d,0x5090);A=W8(wNumBombs);CYC(0x5090,0x5091);alu_or(gb,A);if(F&FZ){CYCT(0x5091,0x5094);clearParentItem_hook(gb);return;}CYC(0x5091,0x5094);CALL_C(0x5094,parentItemLoadAnimationAndIncState_hook,0x5378,0x5097);CYC(0x5097,0x5099);E=1;CYC(0x5099,0x509b);A=0x19;CALL_C(0x509b,cpActiveRing_hook,0x23b0,0x509e);if(!(F&FZ))CYCT(0x509e,0x50a0);else{CYC(0x509e,0x50a0);CYC(0x50a0,0x50a1);E=alu_inc8(gb,E);}CALL_C(0x50a1,itemCreateChild_hook,0x53dd,0x50a4);if(F&FC){CYCT(0x50a4,0x50a7);clearParentItem_hook(gb);return;}CYC(0x50a4,0x50a7);CALL_C(0x50a7,makeLinkPickupObjectH_hook,0x50d3,0x50aa);CYC(0x50aa,0x50ad);bracelet_begin_pickup_hook(gb);}
