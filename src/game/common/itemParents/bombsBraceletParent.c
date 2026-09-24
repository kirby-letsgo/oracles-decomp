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
  CALL_C(b_+O(442),dropLinkHeldItem_hook,SYM(dropLinkHeldItem),b_+OE(445));
  CYC(b_+O(445),b_+OE(448));clearParentItem_hook(gb);
}

static bool bracelet_delete_if_invalid(GB *gb, uint16_t ret) {
  BASE(parentItemCode_bracelet);
  CYC(ret-3,ret);push_effect(gb,ret);
  CYC(b_+O(430),b_+OE(433));A=W8(wLinkSwimmingState);CYC(b_+O(433),b_+OE(434));alu_or(gb,A);
  if(!(F&FZ))CYCT(b_+O(434),b_+OE(436));else{
    CYC(b_+O(434),b_+OE(436));CYC(b_+O(436),b_+OE(439));A=W8(wLinkGrabState);CYC(b_+O(439),b_+OE(440));alu_or(gb,A);
    if(!(F&FZ)){CYCT(b_+O(440),b_+OE(441));ret_effect(gb);return false;}CYC(b_+O(440),b_+OE(441));
  }
  CYC(b_+O(441),b_+OE(442));SET_AF(pop_effect(gb));bracelet_drop_hook(gb);return true;
}

static void bracelet_check_wall(GB *gb, uint16_t call, uint16_t ret) {
  BASE(parentItemCode_bracelet);
  CYC(call,ret);push_effect(gb,ret);
  CYC(b_+O(448),b_+OE(451));A=W8(w1Link_direction);CYC(b_+O(451),b_+OE(452));B=A;CYC(b_+O(452),b_+OE(453));alu_add(gb,A);CYC(b_+O(453),b_+OE(454));alu_add(gb,B);
  CYC(b_+O(454),b_+OE(457));SET_HL(b_+O(478));CYC(b_+O(457),b_+OE(458));push_effect(gb,b_+O(458));bombs_bracelet_add_a_to_hl(gb);
  CYC(b_+O(458),b_+OE(461));A=W8(w1Link_adjacentWallsBitset);CYC(b_+O(461),b_+OE(462));alu_and(gb,mem_rd(gb,HL));CYC(b_+O(462),b_+OE(463));alu_cp(gb,mem_rd(gb,HL));
  if(!(F&FZ)){CYCT(b_+O(463),b_+OE(464));ret_effect(gb);return;}CYC(b_+O(463),b_+OE(464));
  CYC(b_+O(464),b_+OE(465));SET_HL(HL+1);CYC(b_+O(465),b_+OE(468));A=W8(w1Link_yh);CYC(b_+O(468),b_+OE(469));alu_add(gb,mem_rd(gb,HL));CYC(b_+O(469),b_+OE(470));B=A;
  CYC(b_+O(470),b_+OE(471));SET_HL(HL+1);CYC(b_+O(471),b_+OE(474));A=W8(w1Link_xh);CYC(b_+O(474),b_+OE(475));alu_add(gb,mem_rd(gb,HL));CYC(b_+O(475),b_+OE(476));C=A;CYC(b_+O(476),b_+OE(477));alu_xor(gb,A);CYC(b_+O(477),b_+OE(478));ret_effect(gb);
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
  CYC(b_+O(162),b_+OE(164));A=0x15;CALL_C(b_+O(164),specialObjectSetAnimationWithLinkData_hook,SYM(specialObjectSetAnimationWithLinkData),b_+OE(167));
  bracelet_begin_pickup_hook(gb);
}

void bracelet_begin_pickup_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(167),itemDisableLinkMovement_hook,SYM(itemDisableLinkMovement),b_+OE(170));CALL_C(b_+O(170),itemDisableLinkTurning_hook,SYM(itemDisableLinkTurning),b_+OE(173));
  CYC(b_+O(173),b_+OE(175));A=0xc2;CYC(b_+O(175),b_+OE(178));W8(wLinkGrabState)=A;CYC(b_+O(178),b_+OE(179));alu_xor(gb,A);CYC(b_+O(179),b_+OE(182));W8(wLinkGrabState2)=A;
  CYC(b_+O(182),b_+OE(185));SET_HL(w1Link_collisionType);CYC(b_+O(185),b_+OE(187));mem_wr(gb,HL,mem_rd(gb,HL)&~(1<<7));CYC(b_+O(187),b_+OE(189));A=2;CYC(b_+O(189),b_+OE(191));E=4;CYC(b_+O(191),b_+OE(192));mem_wr(gb,DE,A);
  CYC(b_+O(192),b_+OE(194));E=0x3f;CYC(b_+O(194),b_+OE(196));A=0x0f;CYC(b_+O(196),b_+OE(197));mem_wr(gb,DE,A);CYC(b_+O(197),b_+OE(199));A=0x9c;CYC(b_+O(199),b_+OE(202));playSound_b00_hook(gb);
}

void bracelet_state2_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  if(bracelet_delete_if_invalid(gb,b_+O(209)))return;
  CALL_C(b_+O(209),specialObjectAnimate_optimized_hook,SYM(specialObjectAnimate_optimized),b_+OE(212));CYC(b_+O(212),b_+OE(215));A=W8(wLinkGrabState2);CYC(b_+O(215),b_+OE(216));alu_rlca(gb);
  if(F&FC){CYC(b_+O(216),b_+OE(218));CYC(b_+O(218),b_+OE(220));A=0x83;CYC(b_+O(220),b_+OE(223));W8(wLinkGrabState)=A;CYC(b_+O(223),b_+OE(225));E=4;CYC(b_+O(225),b_+OE(227));A=5;CYC(b_+O(227),b_+OE(228));mem_wr(gb,DE,A);CYC(b_+O(228),b_+OE(230));A=0x13;CYC(b_+O(230),b_+OE(233));specialObjectSetAnimationWithLinkData_hook(gb);return;}CYCT(b_+O(216),b_+OE(218));
  CYC(b_+O(233),b_+OE(234));H=D;CYC(b_+O(234),b_+OE(236));L=0x21;CYC(b_+O(236),b_+OE(238));alu_bit(gb,7,mem_rd(gb,HL));
  if(F&FZ){CYC(b_+O(238),b_+OE(240));CYC(b_+O(240),b_+OE(243));A=W8(wLinkGrabState2);CYC(b_+O(243),b_+OE(245));alu_and(gb,0xf0);CYC(b_+O(245),b_+OE(246));alu_add(gb,mem_rd(gb,HL));CYC(b_+O(246),b_+OE(249));W8(wLinkGrabState2)=A;CYC(b_+O(249),b_+OE(250));ret_effect(gb);return;}CYCT(b_+O(238),b_+OE(240));
  CYC(b_+O(250),b_+OE(252));A=0x83;CYC(b_+O(252),b_+OE(255));W8(wLinkGrabState)=A;CYC(b_+O(255),b_+OE(257));L=4;CYC(b_+O(257),b_+OE(258));mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(b_+O(258),b_+OE(260));L=0x3f;CYC(b_+O(260),b_+OE(262));mem_wr(gb,HL,0);
  CYC(b_+O(262),b_+OE(265));SET_HL(w1Link_collisionType);CYC(b_+O(265),b_+OE(267));mem_wr(gb,HL,mem_rd(gb,HL)|(1<<7));CALL_C(b_+O(267),itemEnableLinkTurning_hook,SYM(itemEnableLinkTurning),b_+OE(270));CYC(b_+O(270),b_+OE(273));itemEnableLinkMovement_hook(gb);
}

void bracelet_state3_hook(GB *gb) {
  BASE(parentItemCode_bracelet);
  uint16_t sp0_ = gb->sp;
  if(bracelet_delete_if_invalid(gb,b_+O(276)))return;
  CYC(b_+O(276),b_+OE(279));A=W8(wLinkInAir);CYC(b_+O(279),b_+OE(280));alu_rlca(gb);if(F&FC){CYCT(b_+O(280),b_+OE(281));ret_effect(gb);return;}CYC(b_+O(280),b_+OE(281));
  CYC(b_+O(281),b_+OE(284));A=W8(wcc67);CYC(b_+O(284),b_+OE(285));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(285),b_+OE(286));ret_effect(gb);return;}CYC(b_+O(285),b_+OE(286));
  CYC(b_+O(286),b_+OE(289));A=W8(w1Link_var2a);CYC(b_+O(289),b_+OE(290));alu_or(gb,A);if(!(F&FZ))CYCT(b_+O(290),b_+OE(292));else{CYC(b_+O(290),b_+OE(292));CYC(b_+O(292),b_+OE(295));A=W8(wGameKeysJustPressed);CYC(b_+O(295),b_+OE(297));alu_and(gb,3);if(F&FZ){CYCT(b_+O(297),b_+OE(298));ret_effect(gb);return;}CYC(b_+O(297),b_+OE(298));CALL_C(b_+O(298),updateLinkDirectionFromAngle_hook,SYM(updateLinkDirectionFromAngle),b_+OE(301));}
  CYC(b_+O(301),b_+OE(304));SET_HL(w1Link_relatedObj2);CYC(b_+O(304),b_+OE(305));alu_xor(gb,A);CYC(b_+O(305),b_+OE(306));C=mem_rd(gb,HL);CYC(b_+O(306),b_+OE(307));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(307),b_+OE(308));B=mem_rd(gb,HL);CYC(b_+O(308),b_+OE(309));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(309),b_+OE(310));A=C;CYC(b_+O(310),b_+OE(312));alu_add(gb,5);CYC(b_+O(312),b_+OE(313));L=A;CYC(b_+O(313),b_+OE(314));H=B;CYC(b_+O(314),b_+OE(316));mem_wr(gb,HL,2);
  CYC(b_+O(316),b_+OE(318));E=0x37;CYC(b_+O(318),b_+OE(319));A=mem_rd(gb,DE);CYC(b_+O(319),b_+OE(320));alu_or(gb,A);if(!(F&FZ))CYCT(b_+O(320),b_+OE(322));else{CYC(b_+O(320),b_+OE(322));CYC(b_+O(322),b_+OE(323));A=C;CYC(b_+O(323),b_+OE(324));alu_or(gb,A);bool create=!(F&FZ);if(create)CYCT(b_+O(324),b_+OE(326));else{CYC(b_+O(324),b_+OE(326));CYC(b_+O(326),b_+OE(327));A=B;CYC(b_+O(327),b_+OE(329));alu_cp(gb,0xd7);if(!(F&FC)){CYCT(b_+O(329),b_+OE(331));create=false;}else{CYC(b_+O(329),b_+OE(331));create=true;}}if(create){CYC(b_+O(331),b_+OE(332));push_effect(gb,DE);CYC(b_+O(332),b_+OE(335));SET_HL(w1ReservedItemC);CYC(b_+O(335),b_+OE(336));mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(b_+O(336),b_+OE(337));L=alu_inc8(gb,L);CYC(b_+O(337),b_+OE(339));A=0x16;CYC(b_+O(339),b_+OE(340));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(340),b_+OE(342));L=0x18;CYC(b_+O(342),b_+OE(343));A=C;CYC(b_+O(343),b_+OE(344));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(344),b_+OE(345));mem_wr(gb,HL,B);CYC(b_+O(345),b_+OE(347));alu_add(gb,0x0b);CYC(b_+O(347),b_+OE(348));E=A;CYC(b_+O(348),b_+OE(349));D=B;CALL_C(b_+O(349),objectCopyPosition_rawAddress_hook,SYM(objectCopyPosition_rawAddress),b_+OE(352));CYC(b_+O(352),b_+OE(353));SET_DE(pop_effect(gb));}}
  CYC(b_+O(353),b_+OE(356));A=W8(wLinkAngle);CYC(b_+O(356),b_+OE(357));alu_rlca(gb);if(F&FC)CYCT(b_+O(357),b_+OE(359));else{CYC(b_+O(357),b_+OE(359));CYC(b_+O(359),b_+OE(362));A=W8(w1Link_direction);CYC(b_+O(362),b_+OE(364));A=alu_swap(gb,A);CYC(b_+O(364),b_+OE(365));alu_rrca(gb);}
  CYC(b_+O(365),b_+OE(367));L=9;CYC(b_+O(367),b_+OE(368));mem_wr(gb,HL,A);CYC(b_+O(368),b_+OE(370));L=0x38;CYC(b_+O(370),b_+OE(373));A=W8(wLinkGrabState2);CYC(b_+O(373),b_+OE(374));mem_wr(gb,HL,A);CYC(b_+O(374),b_+OE(375));alu_xor(gb,A);CYC(b_+O(375),b_+OE(378));W8(wLinkGrabState2)=A;CYC(b_+O(378),b_+OE(381));W8(wLinkGrabState)=A;
  CYC(b_+O(381),b_+OE(382));H=D;CYC(b_+O(382),b_+OE(384));L=4;CYC(b_+O(384),b_+OE(385));mem_wr(gb,HL,alu_inc8(gb,mem_rd(gb,HL)));CYC(b_+O(385),b_+OE(387));L=0x3f;CYC(b_+O(387),b_+OE(389));mem_wr(gb,HL,0x0f);CYC(b_+O(389),b_+OE(391));C=0x16;
  
  if (!game_seasons) {
    CYC(b_+391, b_+394); A = W8(w1Companion_id);
    CYC(b_+394, b_+396); alu_cp(gb, 0x0a);
    if (!(F & FZ)) { CYCT(b_+396, b_+398); goto companion_done; }
    CYC(b_+396, b_+398);
  }
  CYC(b_+O(398), b_+OE(401)); A = W8(wLinkObjectIndex);
  CYC(b_+O(401), b_+OE(402)); alu_rrca(gb);
  if (!(F & FC)) CYCT(b_+O(402), b_+OE(404));
  else { CYC(b_+O(402), b_+OE(404)); CYC(b_+O(404), b_+OE(406)); C = 0x25; }
companion_done:
  
  CYC(b_+O(406),b_+OE(407));A=C;CALL_C(b_+O(407),specialObjectSetAnimationWithLinkData_hook,SYM(specialObjectSetAnimationWithLinkData),b_+OE(410));CALL_C(b_+O(410),itemDisableLinkMovement_hook,SYM(itemDisableLinkMovement),b_+OE(413));CALL_C(b_+O(413),itemDisableLinkTurning_hook,SYM(itemDisableLinkTurning),b_+OE(416));CYC(b_+O(416),b_+OE(418));A=0x51;CYC(b_+O(418),b_+OE(421));playSound_b00_hook(gb);
}

void bracelet_state4_hook(GB *gb){BASE(parentItemCode_bracelet);CYC(b_+O(421),b_+OE(423));E=0x21;CYC(b_+O(423),b_+OE(424));A=mem_rd(gb,DE);CYC(b_+O(424),b_+OE(425));alu_rlca(gb);if(!(F&FC)){CYCT(b_+O(425),b_+OE(428));specialObjectAnimate_optimized_hook(gb);return;}CYC(b_+O(425),b_+OE(428));CYC(b_+O(428),b_+OE(430));bracelet_drop_hook(gb);}
void bracelet_state5_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;CALL_C(b_+O(490),parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+OE(493));if(F&FZ){CYCT(b_+O(493),b_+OE(496));bracelet_drop_hook(gb);return;}CYC(b_+O(493),b_+OE(496));if(bracelet_delete_if_invalid(gb,b_+O(499)))return;CYC(b_+O(499),b_+OE(502));A=W8(w1Link_knockbackCounter);CYC(b_+O(502),b_+OE(503));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(503),b_+OE(506));bracelet_drop_hook(gb);return;}CYC(b_+O(503),b_+OE(506));CYC(b_+O(506),b_+OE(509));A=W8(w1Link_direction);CYC(b_+O(509),b_+OE(512));SET_HL(b_+O(202));CYC(b_+O(512),b_+OE(513));push_effect(gb,b_+O(513));bombs_bracelet_add_a_to_hl(gb);CYC(b_+O(513),b_+OE(516));A=W8(wGameKeysPressed);CYC(b_+O(516),b_+OE(517));alu_and(gb,mem_rd(gb,HL));CYC(b_+O(517),b_+OE(519));A=0x13;if(F&FZ){CYCT(b_+O(519),b_+OE(522));specialObjectSetAnimationWithLinkData_hook(gb);}else{CYC(b_+O(519),b_+OE(522));CYC(b_+O(522),b_+OE(525));specialObjectAnimate_optimized_hook(gb);}}

void bracelet_state0_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;CALL_C(b_+O(16),checkLinkOnGround_hook,SYM(checkLinkOnGround),b_+OE(19));if(!(F&FZ)){CYCT(b_+O(19),b_+OE(22));clearParentItem_hook(gb);return;}CYC(b_+O(19),b_+OE(22));
  if (game_seasons) {
    CYC(b_+S(22), b_+S(25)); A = W8(wActiveTileType);
    CYC(b_+S(25), b_+S(27)); alu_cp(gb, 0x08);
    if (F & FZ) { CYCT(b_+S(27), b_+S(30)); clearParentItem_hook(gb); return; }
    CYC(b_+S(27), b_+S(30));
  }
  CYC(b_+O(22),b_+OE(25));A=W8(w1ReservedItemC);CYC(b_+O(25),b_+OE(26));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(26),b_+OE(29));clearParentItem_hook(gb);return;}CYC(b_+O(26),b_+OE(29));CALL_C(b_+O(29),parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+OE(32));if(F&FZ){CYCT(b_+O(32),b_+OE(35));bracelet_drop_hook(gb);return;}CYC(b_+O(32),b_+OE(35));CYC(b_+O(35),b_+OE(38));A=W8(wLinkUsingItem1);CYC(b_+O(38),b_+OE(39));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(39),b_+OE(41));goto nothing;}CYC(b_+O(39),b_+OE(41));CALL_C(b_+O(41),checkGrabbableObjects_hook,SYM(checkGrabbableObjects),b_+OE(44));if(F&FC){CYCT(b_+O(44),b_+OE(46));bracelet_begin_pickup_animated_hook(gb);return;}CYC(b_+O(44),b_+OE(46));CALL_C(b_+O(46),tryPickupBombs_hook,SYM(tryPickupBombs),b_+OE(49));if(!(F&FZ)){CYCT(b_+O(49),b_+OE(51));bracelet_begin_pickup_animated_hook(gb);return;}CYC(b_+O(49),b_+OE(51));bracelet_check_wall(gb,b_+O(51),b_+O(54));if(!(F&FZ)){CYCT(b_+O(54),b_+OE(56));goto nothing;}CYC(b_+O(54),b_+OE(56));CYC(b_+O(56),b_+OE(58));A=0x41;CYC(b_+O(58),b_+OE(61));W8(wLinkGrabState)=A;CYC(b_+O(61),b_+OE(64));parentItemLoadAnimationAndIncState_hook(gb);return;nothing:CYC(b_+O(64),b_+OE(67));A=W8(w1Link_direction);CYC(b_+O(67),b_+OE(69));alu_or(gb,0x80);CYC(b_+O(69),b_+OE(72));W8(wBraceletGrabbingNothing)=A;CYC(b_+O(72),b_+OE(73));ret_effect(gb);}

void bracelet_state1_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;if(bracelet_delete_if_invalid(gb,b_+O(76)))return;CYC(b_+O(76),b_+OE(79));A=W8(w1Link_knockbackCounter);CYC(b_+O(79),b_+OE(80));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(80),b_+OE(83));bracelet_drop_hook(gb);return;}CYC(b_+O(80),b_+OE(83));CALL_C(b_+O(83),parentItemCheckButtonPressed_hook,SYM(parentItemCheckButtonPressed),b_+OE(86));if(F&FZ){CYCT(b_+O(86),b_+OE(89));bracelet_drop_hook(gb);return;}CYC(b_+O(86),b_+OE(89));CYC(b_+O(89),b_+OE(92));A=W8(wLinkInAir);CYC(b_+O(92),b_+OE(93));alu_or(gb,A);if(!(F&FZ)){CYCT(b_+O(93),b_+OE(96));bracelet_drop_hook(gb);return;}CYC(b_+O(93),b_+OE(96));bracelet_check_wall(gb,b_+O(96),b_+O(99));if(!(F&FZ)){CYCT(b_+O(99),b_+OE(102));bracelet_drop_hook(gb);return;}CYC(b_+O(99),b_+OE(102));CYC(b_+O(102),b_+OE(105));A=W8(w1Link_direction);CYC(b_+O(105),b_+OE(108));SET_HL(b_+O(202));CYC(b_+O(108),b_+OE(109));push_effect(gb,b_+O(109));bombs_bracelet_add_a_to_hl(gb);CALL_C(b_+O(109),andHlWithGameKeysPressed_hook,SYM(andHlWithGameKeysPressed),b_+OE(112));CYC(b_+O(112),b_+OE(114));A=0x14;if(F&FZ){CYCT(b_+O(114),b_+OE(117));specialObjectSetAnimationWithLinkData_hook(gb);return;}CYC(b_+O(114),b_+OE(117));CALL_C(b_+O(117),specialObjectAnimate_optimized_hook,SYM(specialObjectAnimate_optimized),b_+OE(120));CYC(b_+O(120),b_+OE(122));E=0x21;CYC(b_+O(122),b_+OE(123));A=mem_rd(gb,DE);CYC(b_+O(123),b_+OE(124));alu_rlca(gb);if(!(F&FC)){CYCT(b_+O(124),b_+OE(125));ret_effect(gb);return;}CYC(b_+O(124),b_+OE(125));bracelet_check_wall(gb,b_+O(125),b_+O(128));if(!(F&FZ)){CYCT(b_+O(128),b_+OE(131));bracelet_drop_hook(gb);return;}CYC(b_+O(128),b_+OE(131));CYC(b_+O(131),b_+OE(132));alu_xor(gb,A);CALL_C(b_+O(132),tryToBreakTile_hook,SYM(tryToBreakTile),b_+OE(135));if(!(F&FC)){CYCT(b_+O(135),b_+OE(136));ret_effect(gb);return;}CYC(b_+O(135),b_+OE(136));CYC(b_+O(136),b_+OE(139));SET_HL(w1ReservedItemC);CYC(b_+O(139),b_+OE(141));A=3;CYC(b_+O(141),b_+OE(142));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(142),b_+OE(144));mem_wr(gb,HL,0x16);CYC(b_+O(144),b_+OE(145));L=alu_inc8(gb,L);CYC(b_+O(145),b_+OE(147));A=mem_rd(gb,hFF92);CYC(b_+O(147),b_+OE(148));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(148),b_+OE(150));E=0x37;CYC(b_+O(150),b_+OE(151));mem_wr(gb,DE,A);CYC(b_+O(151),b_+OE(153));A=mem_rd(gb,hFF8E);CYC(b_+O(153),b_+OE(154));mem_wr(gb,HL,A);SET_HL(HL+1);CYC(b_+O(154),b_+OE(155));alu_xor(gb,A);CYC(b_+O(155),b_+OE(158));W8(w1Link_relatedObj2)=A;CYC(b_+O(158),b_+OE(159));A=H;CYC(b_+O(159),b_+OE(162));mem_wr(gb,w1Link_relatedObj2+1,A);bracelet_begin_pickup_animated_hook(gb);}

void parentItemCode_bracelet_hook(GB *gb){BASE(parentItemCode_bracelet);uint16_t sp0_=gb->sp;CYC(b_+O(0),b_+OE(2));E=4;CYC(b_+O(2),b_+OE(3));A=mem_rd(gb,DE);CYC(b_+O(3),b_+OE(4));push_effect(gb,b_+O(4));do { uint16_t jt_ = (bombs_bracelet_jump_table(gb));
  if (jt_ == b_+O(16)) { bracelet_state0_hook(gb);return; }
  else if (jt_ == b_+O(73)) { bracelet_state1_hook(gb);return; }
  else if (jt_ == b_+O(206)) { bracelet_state2_hook(gb);return; }
  else if (jt_ == b_+O(273)) { bracelet_state3_hook(gb);return; }
  else if (jt_ == b_+O(421)) { bracelet_state4_hook(gb);return; }
  else if (jt_ == b_+O(490)) { bracelet_state5_hook(gb);return; }
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

void parentItemCode_bomb_hook(GB *gb){BASE(parentItemCode_bomb);uint16_t sp0_=gb->sp;CYC(b_+O(0),b_+OE(2));E=4;CYC(b_+O(2),b_+OE(3));A=mem_rd(gb,DE);CYC(b_+O(3),b_+OE(4));push_effect(gb,b_+O(4));do { uint16_t jt_ = (bombs_bracelet_jump_table(gb));
  if (jt_ == b_+O(14)) { break; }
  else if (jt_ == SYM(parentItemCode_bracelet__state2)) { bracelet_state2_hook(gb);return; }
  else if (jt_ == SYM(parentItemCode_bracelet__state3)) { bracelet_state3_hook(gb);return; }
  else if (jt_ == SYM(parentItemCode_bracelet__state4)) { bracelet_state4_hook(gb);return; }
  else { hook_continue(gb,HL,sp0_);return; }
} while (0);
  if (!game_seasons) {
    CALL_C(b_+14, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+17);
    if (!(F & FZ)) { CYCT(b_+17, b_+20); clearParentItem_hook(gb); return; }
    CYC(b_+17, b_+20);
    CYC(b_+20, b_+23); A = W8(w1Companion_id);
    CYC(b_+23, b_+25); alu_cp(gb, 0x13);
    if (F & FZ) { CYCT(b_+25, b_+27); goto check_swimming; }
    CYC(b_+25, b_+27);
  }
  CYC(b_+O(27), b_+OE(30)); A = W8(wLinkObjectIndex);
  CYC(b_+O(30), b_+OE(31)); alu_rrca(gb);
  if (F & FC) { CYCT(b_+O(31), b_+OE(34)); clearParentItem_hook(gb); return; }
  CYC(b_+O(31), b_+OE(34));
check_swimming:
  CYC(b_+O(34),b_+OE(37));A=W8(wLinkSwimmingState);CYC(b_+O(37),b_+OE(38));B=A;CYC(b_+O(38),b_+OE(41));A=W8(wLinkInAir);CYC(b_+O(41),b_+OE(42));alu_or(gb,B);if(!(F&FZ)){CYCT(b_+O(42),b_+OE(45));clearParentItem_hook(gb);return;}CYC(b_+O(42),b_+OE(45));CALL_C(b_+O(45),tryPickupBombs_hook,SYM(tryPickupBombs),b_+OE(48));if(!(F&FZ)){CYCT(b_+O(48),b_+OE(51));bracelet_begin_pickup_animated_hook(gb);return;}CYC(b_+O(48),b_+OE(51));CYC(b_+O(51),b_+OE(54));A=W8(wNumBombs);CYC(b_+O(54),b_+OE(55));alu_or(gb,A);if(F&FZ){CYCT(b_+O(55),b_+OE(58));clearParentItem_hook(gb);return;}CYC(b_+O(55),b_+OE(58));CALL_C(b_+O(58),parentItemLoadAnimationAndIncState_hook,SYM(parentItemLoadAnimationAndIncState),b_+OE(61));CYC(b_+O(61),b_+OE(63));E=1;CYC(b_+O(63),b_+OE(65));A=0x19;CALL_C(b_+O(65),cpActiveRing_hook,SYM(cpActiveRing),b_+OE(68));if(!(F&FZ))CYCT(b_+O(68),b_+OE(70));else{CYC(b_+O(68),b_+OE(70));CYC(b_+O(70),b_+OE(71));E=alu_inc8(gb,E);}CALL_C(b_+O(71),itemCreateChild_hook,SYM(itemCreateChild),b_+OE(74));if(F&FC){CYCT(b_+O(74),b_+OE(77));clearParentItem_hook(gb);return;}CYC(b_+O(74),b_+OE(77));CALL_C(b_+O(77),makeLinkPickupObjectH_hook,SYM(makeLinkPickupObjectH),b_+OE(80));CYC(b_+O(80),b_+OE(83));bracelet_begin_pickup_hook(gb);}
