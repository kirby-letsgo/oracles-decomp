#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+442,dropLinkHeldItem_hook,SYM(dropLinkHeldItem),b_+445);
  CYC(b_+445,b_+448);clearParentItem_hook(gb);
}

static bool bracelet_delete_if_invalid(GB *gb, uint16_t ret) {
  BASE(parentItemCode_bracelet);
  CYC(ret-3,ret);push_effect(gb,ret);
  CYC(b_+430,b_+433);A=W8(wLinkSwimmingState);CYC(b_+433,b_+434);alu_or(gb,A);
  if(!(F&FZ))CYCT(b_+434,b_+436);else{
    CYC(b_+434,b_+436);CYC(b_+436,b_+439);A=W8(wLinkGrabState);CYC(b_+439,b_+440);alu_or(gb,A);
    if(!(F&FZ)){CYCT(b_+440,b_+441);ret_effect(gb);return false;}CYC(b_+440,b_+441);
  }
  CYC(b_+441,b_+442);SET_AF(pop_effect(gb));bracelet_drop_hook(gb);return true;
}

static void bracelet_check_wall(GB *gb, uint16_t call, uint16_t ret) {
  BASE(parentItemCode_bracelet);
  CYC(call,ret);push_effect(gb,ret);
  CYC(b_+448,b_+451);A=W8(w1Link_direction);CYC(b_+451,b_+452);B=A;CYC(b_+452,b_+453);alu_add(gb,A);CYC(b_+453,b_+454);alu_add(gb,B);
  CYC(b_+454,b_+457);SET_HL(b_+478);CYC(b_+457,b_+458);push_effect(gb,b_+458);bombs_bracelet_add_a_to_hl(gb);
  CYC(b_+458,b_+461);A=W8(w1Link_adjacentWallsBitset);CYC(b_+461,b_+462);alu_and(gb,mem_rd(gb,HL));CYC(b_+462,b_+463);alu_cp(gb,mem_rd(gb,HL));
  if(!(F&FZ)){CYCT(b_+463,b_+464);ret_effect(gb);return;}CYC(b_+463,b_+464);
  CYC(b_+464,b_+465);SET_HL(HL+1);CYC(b_+465,b_+468);A=W8(w1Link_yh);CYC(b_+468,b_+469);alu_add(gb,mem_rd(gb,HL));CYC(b_+469,b_+470);B=A;
  CYC(b_+470,b_+471);SET_HL(HL+1);CYC(b_+471,b_+474);A=W8(w1Link_xh);CYC(b_+474,b_+475);alu_add(gb,mem_rd(gb,HL));CYC(b_+475,b_+476);C=A;CYC(b_+476,b_+477);alu_xor(gb,A);CYC(b_+477,b_+478);ret_effect(gb);
}

void makeLinkPickupObjectH_hook(GB *gb) {
  BASE(makeLinkPickupObjectH);
  CYC(b_+0,b_+2);L=0;CYC(b_+2,b_+4);mem_wr(gb,HL,mem_rd(gb,HL)|(1<<1));CYC(b_+4,b_+6);L=5;CYC(b_+6,b_+7);alu_xor(gb,A);
  CYC(b_+7,b_+8);mem_wr(gb,HL,A);SET_HL(HL-1);CYC(b_+8,b_+10);mem_wr(gb,HL,2);CYC(b_+10,b_+13);W8(w1Link_relatedObj2)=A;CYC(b_+13,b_+14);A=H;CYC(b_+14,b_+17);mem_wr(gb,w1Link_relatedObj2+1,A);CYC(b_+17,b_+18);alu_or(gb,A);CYC(b_+18,b_+19);ret_effect(gb);
}

void try_pickup_object_hook(GB *gb) {
  BASE(tryPickupBombs);
  uint16_t sp0_ = gb->sp;
  CYC(b_+24,b_+26);L=0x2f;CYC(b_+26,b_+27);A=mem_rd(gb,HL);CYC(b_+27,b_+29);alu_and(gb,0xb0);
  if(!(F&FZ)){CYCT(b_+29,b_+31);goto fail;}CYC(b_+29,b_+31);
  CALL_C(b_+31,objectHCheckCollisionWithLink_hook,SYM(objectHCheckCollisionWithLink),b_+34);
  if(F&FC){CYCT(b_+34,b_+36);makeLinkPickupObjectH_hook(gb);return;}CYC(b_+34,b_+36);
fail:CYC(b_+36,b_+37);alu_xor(gb,A);CYC(b_+37,b_+38);ret_effect(gb);
}

void tryPickupBombs_hook(GB *gb) {
  BASE(tryPickupBombs);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0,b_+3);A=W8(wLinkUsingItem1);CYC(b_+3,b_+4);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+4,b_+6);goto fail;}CYC(b_+4,b_+6);
  CYC(b_+6,b_+8);C=3;CALL_C(b_+8,findItemWithID_hook,SYM(findItemWithID),b_+11);if(!(F&FZ)){CYCT(b_+11,b_+13);goto fail;}CYC(b_+11,b_+13);
  CYC(b_+13,b_+16);push_effect(gb,b_+16);try_pickup_object_hook(gb);if(!(F&FZ)){CYCT(b_+16,b_+17);ret_effect(gb);return;}CYC(b_+16,b_+17);
  CYC(b_+17,b_+19);C=3;CALL_C(b_+19,findItemWithID_startingAfterH_hook,SYM(findItemWithID_startingAfterH),b_+22);if(!(F&FZ)){CYCT(b_+22,b_+24);goto fail;}CYC(b_+22,b_+24);
  try_pickup_object_hook(gb);return;
fail:CYC(b_+36,b_+37);alu_xor(gb,A);CYC(b_+37,b_+38);ret_effect(gb);
}

void bracelet_begin_pickup_hook(GB *gb);

void bracelet_begin_pickup_animated_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  CYC(b_+162,b_+164);A=0x15;CALL_C(b_+164,specialObjectSetAnimationWithLinkData_hook,SYM(specialObjectSetAnimationWithLinkData),b_+167);
  bracelet_begin_pickup_hook(gb);
}

void bracelet_begin_pickup_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+167,itemDisableLinkMovement_hook,SYM(itemDisableLinkMovement),b_+170);CALL_C(b_+170,itemDisableLinkTurning_hook,SYM(itemDisableLinkTurning),b_+173);
  CYC(b_+173,b_+175);A=0xc2;CYC(b_+175,b_+178);W8(wLinkGrabState)=A;CYC(b_+178,b_+179);alu_xor(gb,A);CYC(b_+179,b_+182);W8(wLinkGrabState2)=A;
  CYC(b_+182,b_+185);SET_HL(w1Link_collisionType);CYC(b_+185,b_+187);mem_wr(gb,HL,mem_rd(gb,HL)&~(1<<7));CYC(b_+187,b_+189);A=2;CYC(b_+189,b_+191);E=4;CYC(b_+191,b_+192);mem_wr(gb,DE,A);
  CYC(b_+192,b_+194);E=0x3f;CYC(b_+194,b_+196);A=0x0f;CYC(b_+196,b_+197);mem_wr(gb,DE,A);CYC(b_+197,b_+199);A=0x9c;CYC(b_+199,b_+202);playSound_b00_hook(gb);
}

void bracelet_state2_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  if(bracelet_delete_if_invalid(gb,b_+209))return;
  CALL_C(b_+209,specialObjectAnimate_optimized_hook,SYM(specialObjectAnimate_optimized),b_+212);CYC(b_+212,b_+215);A=W8(wLinkGrabState2);CYC(b_+215,b_+216);alu_rlca(gb);
  if(F&FC){CYC(b_+216,b_+218);CYC(b_+218,b_+220);A=0x83;CYC(b_+220,b_+223);W8(wLinkGrabState)=A;CYC(b_+223,b_+225);E=4;CYC(b_+225,b_+227);A=5;CYC(b_+227,b_+228);mem_wr(gb,DE,A);CYC(b_+228,b_+230);A=0x13;CYC(b_+230,b_+233);specialObjectSetAnimationWithLinkData_hook(gb);return;}CYCT(b_+216,b_+218);
  CYC(b_+233,b_+234);H=D;CYC(b_+234,b_+236);L=0x21;CYC(b_+236,b_+238);alu_bit(gb,7,mem_rd(gb,HL));
  if(F&FZ){CYC(b_+238,b_+240);CYC(b_+240,b_+243);A=W8(wLinkGrabState2);CYC(b_+243,b_+245);alu_and(gb,0xf0);CYC(b_+245,b_+246);alu_add(gb,mem_rd(gb,HL));CYC(b_+246,b_+249);W8(wLinkGrabState2)=A;CYC(b_+249,b_+250);ret_effect(gb);return;}CYCT(b_+238,b_+240);
  CYC(b_+250,b_+252);A=0x83;CYC(b_+252,b_+255);W8(wLinkGrabState)=A;CYC(b_+255,b_+257);L=4;CYC(b_+257,b_+258);mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(b_+258,b_+260);L=0x3f;CYC(b_+260,b_+262);mem_wr(gb,HL,0);
  CYC(b_+262,b_+265);SET_HL(w1Link_collisionType);CYC(b_+265,b_+267);mem_wr(gb,HL,mem_rd(gb,HL)|(1<<7));CALL_C(b_+267,itemEnableLinkTurning_hook,SYM(itemEnableLinkTurning),b_+270);CYC(b_+270,b_+273);itemEnableLinkMovement_hook(gb);
}

void bracelet_state3_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  if(bracelet_delete_if_invalid(gb,b_+276))return;
  CYC(b_+276,b_+279);A=W8(wLinkInAir);CYC(b_+279,b_+280);alu_rlca(gb);if(F&FC){CYCT(b_+280,b_+281);ret_effect(gb);return;}CYC(b_+280,b_+281);
  CYC(b_+281,b_+284);A=W8(wcc67);CYC(b_+284,b_+285);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+285,b_+286);ret_effect(gb);return;}CYC(b_+285,b_+286);
  CYC(b_+286,b_+289);A=W8(w1Link_var2a);CYC(b_+289,b_+290);alu_or(gb,A);if(!(F&FZ))CYCT(b_+290,b_+292);else{CYC(b_+290,b_+292);CYC(b_+292,b_+295);A=W8(wGameKeysJustPressed);CYC(b_+295,b_+297);alu_and(gb,3);if(F&FZ){CYCT(b_+297,b_+298);ret_effect(gb);return;}CYC(b_+297,b_+298);CALL_C(b_+298,updateLinkDirectionFromAngle_hook,SYM(updateLinkDirectionFromAngle),b_+301);}
  CYC(b_+301,b_+304);SET_HL(w1Link_relatedObj2);CYC(b_+304,b_+305);alu_xor(gb,A);CYC(b_+305,b_+306);C=mem_rd(gb,HL);CYC(b_+306,b_+307);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+307,b_+308);B=mem_rd(gb,HL);CYC(b_+308,b_+309);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+309,b_+310);A=C;CYC(b_+310,b_+312);alu_add(gb,5);CYC(b_+312,b_+313);L=A;CYC(b_+313,b_+314);H=B;CYC(b_+314,b_+316);mem_wr(gb,HL,2);
  CYC(b_+316,b_+318);E=0x37;CYC(b_+318,b_+319);A=mem_rd(gb,DE);CYC(b_+319,b_+320);alu_or(gb,A);if(!(F&FZ))CYCT(b_+320,b_+322);else{CYC(b_+320,b_+322);CYC(b_+322,b_+323);A=C;CYC(b_+323,b_+324);alu_or(gb,A);bool create=!(F&FZ);if(create)CYCT(b_+324,b_+326);else{CYC(b_+324,b_+326);CYC(b_+326,b_+327);A=B;CYC(b_+327,b_+329);alu_cp(gb,0xd7);if(!(F&FC)){CYCT(b_+329,b_+331);create=false;}else{CYC(b_+329,b_+331);create=true;}}if(create){CYC(b_+331,b_+332);push_effect(gb,DE);CYC(b_+332,b_+335);SET_HL(w1ReservedItemC);CYC(b_+335,b_+336);mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(b_+336,b_+337);L=alu_inc8(gb,L);CYC(b_+337,b_+339);A=0x16;CYC(b_+339,b_+340);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+340,b_+342);L=0x18;CYC(b_+342,b_+343);A=C;CYC(b_+343,b_+344);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+344,b_+345);mem_wr(gb,HL,B);CYC(b_+345,b_+347);alu_add(gb,0x0b);CYC(b_+347,b_+348);E=A;CYC(b_+348,b_+349);D=B;CALL_C(b_+349,objectCopyPosition_rawAddress_hook,SYM(objectCopyPosition_rawAddress),b_+352);CYC(b_+352,b_+353);SET_DE(pop_effect(gb));}}
  CYC(b_+353,b_+356);A=W8(wLinkAngle);CYC(b_+356,b_+357);alu_rlca(gb);if(F&FC)CYCT(b_+357,b_+359);else{CYC(b_+357,b_+359);CYC(b_+359,b_+362);A=W8(w1Link_direction);CYC(b_+362,b_+364);A=alu_swap(gb,A);CYC(b_+364,b_+365);alu_rrca(gb);}
  CYC(b_+365,b_+367);L=9;CYC(b_+367,b_+368);mem_wr(gb,HL,A);CYC(b_+368,b_+370);L=0x38;CYC(b_+370,b_+373);A=W8(wLinkGrabState2);CYC(b_+373,b_+374);mem_wr(gb,HL,A);CYC(b_+374,b_+375);alu_xor(gb,A);CYC(b_+375,b_+378);W8(wLinkGrabState2)=A;CYC(b_+378,b_+381);W8(wLinkGrabState)=A;
  CYC(b_+381,b_+382);H=D;CYC(b_+382,b_+384);L=4;CYC(b_+384,b_+385);mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(b_+385,b_+387);L=0x3f;CYC(b_+387,b_+389);mem_wr(gb,HL,0x0f);CYC(b_+389,b_+391);C=0x16;
  CYC(b_+391,b_+394);A=W8(w1Companion_id);CYC(b_+394,b_+396);alu_cp(gb,0x0a);if(!(F&FZ))CYCT(b_+396,b_+398);else{CYC(b_+396,b_+398);CYC(b_+398,b_+401);A=W8(wLinkObjectIndex);CYC(b_+401,b_+402);alu_rrca(gb);if(!(F&FC))CYCT(b_+402,b_+404);else{CYC(b_+402,b_+404);CYC(b_+404,b_+406);C=0x25;}}
  CYC(b_+406,b_+407);A=C;CALL_C(b_+407,specialObjectSetAnimationWithLinkData_hook,SYM(specialObjectSetAnimationWithLinkData),b_+410);CALL_C(b_+410,itemDisableLinkMovement_hook,SYM(itemDisableLinkMovement),b_+413);CALL_C(b_+413,itemDisableLinkTurning_hook,SYM(itemDisableLinkTurning),b_+416);CYC(b_+416,b_+418);A=0x51;CYC(b_+418,b_+421);playSound_b00_hook(gb);
}

void bracelet_state4_hook(GB *gb){BASE(parentItemCode_bracelet);CYC(b_+421,b_+423);E=0x21;CYC(b_+423,b_+424);A=mem_rd(gb,DE);CYC(b_+424,b_+425);alu_rlca(gb);if(!(F&FC)){CYCT(b_+425,b_+428);specialObjectAnimate_optimized_hook(gb);return;}CYC(b_+425,b_+428);CYC(b_+428,b_+430);bracelet_drop_hook(gb);}
void bracelet_state5_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;CALL_C(b_+490,parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+493);if(F&FZ){CYCT(b_+493,b_+496);bracelet_drop_hook(gb);return;}CYC(b_+493,b_+496);if(bracelet_delete_if_invalid(gb,b_+499))return;CYC(b_+499,b_+502);A=W8(w1Link_knockbackCounter);CYC(b_+502,b_+503);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+503,b_+506);bracelet_drop_hook(gb);return;}CYC(b_+503,b_+506);CYC(b_+506,b_+509);A=W8(w1Link_direction);CYC(b_+509,b_+512);SET_HL(b_+202);CYC(b_+512,b_+513);push_effect(gb,b_+513);bombs_bracelet_add_a_to_hl(gb);CYC(b_+513,b_+516);A=W8(wGameKeysPressed);CYC(b_+516,b_+517);alu_and(gb,mem_rd(gb,HL));CYC(b_+517,b_+519);A=0x13;if(F&FZ){CYCT(b_+519,b_+522);specialObjectSetAnimationWithLinkData_hook(gb);}else{CYC(b_+519,b_+522);CYC(b_+522,b_+525);specialObjectAnimate_optimized_hook(gb);}}

void bracelet_state0_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;CALL_C(b_+16,checkLinkOnGround_hook,SYM(checkLinkOnGround),b_+19);if(!(F&FZ)){CYCT(b_+19,b_+22);clearParentItem_hook(gb);return;}CYC(b_+19,b_+22);CYC(b_+22,b_+25);A=W8(w1ReservedItemC);CYC(b_+25,b_+26);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+26,b_+29);clearParentItem_hook(gb);return;}CYC(b_+26,b_+29);CALL_C(b_+29,parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+32);if(F&FZ){CYCT(b_+32,b_+35);bracelet_drop_hook(gb);return;}CYC(b_+32,b_+35);CYC(b_+35,b_+38);A=W8(wLinkUsingItem1);CYC(b_+38,b_+39);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+39,b_+41);goto nothing;}CYC(b_+39,b_+41);CALL_C(b_+41,checkGrabbableObjects_hook,SYM(checkGrabbableObjects),b_+44);if(F&FC){CYCT(b_+44,b_+46);bracelet_begin_pickup_animated_hook(gb);return;}CYC(b_+44,b_+46);CALL_C(b_+46,tryPickupBombs_hook,SYM(tryPickupBombs),b_+49);if(!(F&FZ)){CYCT(b_+49,b_+51);bracelet_begin_pickup_animated_hook(gb);return;}CYC(b_+49,b_+51);bracelet_check_wall(gb,b_+51,b_+54);if(!(F&FZ)){CYCT(b_+54,b_+56);goto nothing;}CYC(b_+54,b_+56);CYC(b_+56,b_+58);A=0x41;CYC(b_+58,b_+61);W8(wLinkGrabState)=A;CYC(b_+61,b_+64);parentItemLoadAnimationAndIncState_hook(gb);return;nothing:CYC(b_+64,b_+67);A=W8(w1Link_direction);CYC(b_+67,b_+69);alu_or(gb,0x80);CYC(b_+69,b_+72);W8(wBraceletGrabbingNothing)=A;CYC(b_+72,b_+73);ret_effect(gb);}

void bracelet_state1_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;if(bracelet_delete_if_invalid(gb,b_+76))return;CYC(b_+76,b_+79);A=W8(w1Link_knockbackCounter);CYC(b_+79,b_+80);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+80,b_+83);bracelet_drop_hook(gb);return;}CYC(b_+80,b_+83);CALL_C(b_+83,parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+86);if(F&FZ){CYCT(b_+86,b_+89);bracelet_drop_hook(gb);return;}CYC(b_+86,b_+89);CYC(b_+89,b_+92);A=W8(wLinkInAir);CYC(b_+92,b_+93);alu_or(gb,A);if(!(F&FZ)){CYCT(b_+93,b_+96);bracelet_drop_hook(gb);return;}CYC(b_+93,b_+96);bracelet_check_wall(gb,b_+96,b_+99);if(!(F&FZ)){CYCT(b_+99,b_+102);bracelet_drop_hook(gb);return;}CYC(b_+99,b_+102);CYC(b_+102,b_+105);A=W8(w1Link_direction);CYC(b_+105,b_+108);SET_HL(b_+202);CYC(b_+108,b_+109);push_effect(gb,b_+109);bombs_bracelet_add_a_to_hl(gb);CALL_C(b_+109,andHlWithGameKeysPressed_hook,SYM(andHlWithGameKeysPressed),b_+112);CYC(b_+112,b_+114);A=0x14;if(F&FZ){CYCT(b_+114,b_+117);specialObjectSetAnimationWithLinkData_hook(gb);return;}CYC(b_+114,b_+117);CALL_C(b_+117,specialObjectAnimate_optimized_hook,SYM(specialObjectAnimate_optimized),b_+120);CYC(b_+120,b_+122);E=0x21;CYC(b_+122,b_+123);A=mem_rd(gb,DE);CYC(b_+123,b_+124);alu_rlca(gb);if(!(F&FC)){CYCT(b_+124,b_+125);ret_effect(gb);return;}CYC(b_+124,b_+125);bracelet_check_wall(gb,b_+125,b_+128);if(!(F&FZ)){CYCT(b_+128,b_+131);bracelet_drop_hook(gb);return;}CYC(b_+128,b_+131);CYC(b_+131,b_+132);alu_xor(gb,A);CALL_C(b_+132,tryToBreakTile_hook,SYM(tryToBreakTile),b_+135);if(!(F&FC)){CYCT(b_+135,b_+136);ret_effect(gb);return;}CYC(b_+135,b_+136);CYC(b_+136,b_+139);SET_HL(w1ReservedItemC);CYC(b_+139,b_+141);A=3;CYC(b_+141,b_+142);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+142,b_+144);mem_wr(gb,HL,0x16);CYC(b_+144,b_+145);L=alu_inc8(gb,L);CYC(b_+145,b_+147);A=mem_rd(gb,hFF92);CYC(b_+147,b_+148);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+148,b_+150);E=0x37;CYC(b_+150,b_+151);mem_wr(gb,DE,A);CYC(b_+151,b_+153);A=mem_rd(gb,hFF8E);CYC(b_+153,b_+154);mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+154,b_+155);alu_xor(gb,A);CYC(b_+155,b_+158);W8(w1Link_relatedObj2)=A;CYC(b_+158,b_+159);A=H;CYC(b_+159,b_+162);mem_wr(gb,w1Link_relatedObj2+1,A);bracelet_begin_pickup_animated_hook(gb);}

void parentItemCode_bracelet_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;CYC(b_+0,b_+2);E=4;CYC(b_+2,b_+3);A=mem_rd(gb,DE);CYC(b_+3,b_+4);push_effect(gb,b_+4);do { uint16_t jt_ = (bombs_bracelet_jump_table(gb));
  if (jt_ == b_+16) { bracelet_state0_hook(gb);return; }
  else if (jt_ == b_+73) { bracelet_state1_hook(gb);return; }
  else if (jt_ == b_+206) { bracelet_state2_hook(gb);return; }
  else if (jt_ == b_+273) { bracelet_state3_hook(gb);return; }
  else if (jt_ == b_+421) { bracelet_state4_hook(gb);return; }
  else if (jt_ == b_+490) { bracelet_state5_hook(gb);return; }
  else { hook_continue(gb,HL,sp0_); }
} while (0);}

void parentItemCode_bombchu_hook(GB *gb) {
  BASE(parentItemCode_bombchu);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  uint16_t target = bombs_bracelet_jump_table(gb);
  if (target != b_+O(8)) { hook_continue(gb, target, sp0_); return; }
  if (!game_seasons) {      // no bombchus underwater or on the Dimitri raft
    CALL_C(b_+8, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+11);
    if (!(F & FZ)) { CYCT(b_+11, b_+14); TAIL(clearParentItem); }
    CYC(b_+11, b_+14);
    CYC(b_+14, b_+17); A = W8(w1Companion_id);
    CYC(b_+17, b_+19); alu_cp(gb, 0x13);
    if (F & FZ) { CYCT(b_+19, b_+22); TAIL(clearParentItem); }
    CYC(b_+19, b_+22);
  }
  CYC(b_+O(22), b_+OE(25)); A = W8(wLinkSwimmingState);
  CYC(b_+O(25), b_+OE(26)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(26), b_+OE(29)); TAIL(clearParentItem); }
  CYC(b_+O(26), b_+OE(29));
  CYC(b_+O(29), b_+OE(32)); A = W8(wNumBombchus);
  CYC(b_+O(32), b_+OE(33)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(33), b_+OE(36)); TAIL(clearParentItem); }
  CYC(b_+O(33), b_+OE(36));
  CALL_C(b_+O(36), parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+OE(39));
  CYC(b_+O(39), b_+OE(41)); E = 0x01;
  CYC(b_+O(41), b_+OE(44)); TAIL(itemCreateChildAndDeleteOnFailure);
}

void parentItemCode_bomb_hook(GB *gb){BASE(parentItemCode_bomb);uint16_t sp0_=gb->sp;CYC(b_+0,b_+2);E=4;CYC(b_+2,b_+3);A=mem_rd(gb,DE);CYC(b_+3,b_+4);push_effect(gb,b_+4);do { uint16_t jt_ = (bombs_bracelet_jump_table(gb));
  if (jt_ == b_+14) { break; }
  else if (jt_ == SYM(parentItemCode_bracelet__state2)) { bracelet_state2_hook(gb);return; }
  else if (jt_ == SYM(parentItemCode_bracelet__state3)) { bracelet_state3_hook(gb);return; }
  else if (jt_ == SYM(parentItemCode_bracelet__state4)) { bracelet_state4_hook(gb);return; }
  else { hook_continue(gb,HL,sp0_);return; }
} while (0);CALL_C(b_+14,isLinkUnderwater_hook,SYM(isLinkUnderwater),b_+17);if(!(F&FZ)){CYCT(b_+17,b_+20);clearParentItem_hook(gb);return;}CYC(b_+17,b_+20);CYC(b_+20,b_+23);A=W8(w1Companion_id);CYC(b_+23,b_+25);alu_cp(gb,0x13);if(F&FZ)CYCT(b_+25,b_+27);else{CYC(b_+25,b_+27);CYC(b_+27,b_+30);A=W8(wLinkObjectIndex);CYC(b_+30,b_+31);alu_rrca(gb);if(F&FC){CYCT(b_+31,b_+34);clearParentItem_hook(gb);return;}CYC(b_+31,b_+34);}CYC(b_+34,b_+37);A=W8(wLinkSwimmingState);CYC(b_+37,b_+38);B=A;CYC(b_+38,b_+41);A=W8(wLinkInAir);CYC(b_+41,b_+42);alu_or(gb,B);if(!(F&FZ)){CYCT(b_+42,b_+45);clearParentItem_hook(gb);return;}CYC(b_+42,b_+45);CALL_C(b_+45,tryPickupBombs_hook,SYM(tryPickupBombs),b_+48);if(!(F&FZ)){CYCT(b_+48,b_+51);bracelet_begin_pickup_animated_hook(gb);return;}CYC(b_+48,b_+51);CYC(b_+51,b_+54);A=W8(wNumBombs);CYC(b_+54,b_+55);alu_or(gb,A);if(F&FZ){CYCT(b_+55,b_+58);clearParentItem_hook(gb);return;}CYC(b_+55,b_+58);CALL_C(b_+58,parentItemLoadAnimationAndIncState_hook,SYM(parentItemLoadAnimationAndIncState),b_+61);CYC(b_+61,b_+63);E=1;CYC(b_+63,b_+65);A=0x19;CALL_C(b_+65,cpActiveRing_hook,SYM(cpActiveRing),b_+68);if(!(F&FZ))CYCT(b_+68,b_+70);else{CYC(b_+68,b_+70);CYC(b_+70,b_+71);E=alu_inc8(gb,E);}CALL_C(b_+71,itemCreateChild_hook,SYM(itemCreateChild),b_+74);if(F&FC){CYCT(b_+74,b_+77);clearParentItem_hook(gb);return;}CYC(b_+74,b_+77);CALL_C(b_+77,makeLinkPickupObjectH_hook,SYM(makeLinkPickupObjectH),b_+80);CYC(b_+80,b_+83);bracelet_begin_pickup_hook(gb);}
