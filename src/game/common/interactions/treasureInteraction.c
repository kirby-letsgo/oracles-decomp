#include "game/game.h"
#include "game/gen.h"
#undef CYC
#undef CYCT
#define CYC(a,b) burn_rom(gb,bk_,(a),(b),false)
#define CYCT(a,b) burn_rom(gb,bk_,(a),(b),true)

void interactionCode60__giveTreasure_hook(GB *gb);
void interactionCode60__checkLinkTouched_hook(GB *gb);
void interactionCode60__setVisibleIfWithinScreenBoundary_hook(GB *gb);
void interactionCode60__setLinkAnimationAndDeleteIfTextClosed_hook(GB *gb);
void interactionCode60_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0),b_+OE(2)); E = 0x44;
  CYC(b_+O(2),b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3),b_+O(3)+1);push_effect(gb,b_+O(4));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(14)) { goto L_4981; }
    else if (jt_ == b_+O(54)) { goto L_49a9; }
    else if (jt_ == b_+O(514)) { goto L_4b75; }
    else if (jt_ == b_+O(524)) { goto L_4b7f; }
    else { HANDOFF(HL); }
  } while (0);
L_4981:
  CYC(b_+O(14),b_+OE(16)); A = 0x01;
  CYC(b_+O(16),b_+OE(17)); mem_wr(gb, DE, A);
  CYC(b_+O(17),b_+OE(20)); SET_HL(SYM(interactionLoadTreasureData));
  CYC(b_+O(20),b_+OE(22)); E = GV(0x16, 0x15);
  CALL_C(b_+O(22), interBankCall_hook, 0x008a, b_+OE(25));
  CYC(b_+O(25),b_+OE(27)); A = 0x06;
  CALL_C(b_+O(27), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(30));
  CYC(b_+O(30),b_+OE(32)); L = 0x78;
  CYC(b_+O(32),b_+OE(33)); A = mem_rd(gb, HL);
  CYC(b_+O(33),b_+OE(34)); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+O(34),b_+OE(36)); goto L_499e; } CYC(b_+O(34),b_+OE(36));
  CYC(b_+O(36),b_+OE(38)); alu_cp(gb, 0xff);
  if ((F & FZ)) { CYCT(b_+O(38),b_+OE(40)); goto L_499e; } CYC(b_+O(38),b_+OE(40));
  CYC(b_+O(40),b_+OE(42)); L = 0x74;
  CYC(b_+O(42),b_+OE(43)); mem_wr(gb, HL, A);
L_499e:
  CALL_C(b_+O(43), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(46));
L_49a1:
  CYC(b_+O(46),b_+OE(48)); E = 0x71;
  CYC(b_+O(48),b_+OE(49)); A = mem_rd(gb, DE);
  CYC(b_+O(49),b_+OE(50)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(50),b_+O(50)+1);ret_effect(gb); return; } CYC(b_+O(50),b_+OE(51));
  CYC(b_+O(51),b_+OE(54)); if (hook_is(gb, SYM(objectSetVisiblec2), objectSetVisiblec2_hook)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_49a9:
  CYC(b_+O(54),b_+OE(56)); E = 0x71;
  CYC(b_+O(56),b_+OE(57)); A = mem_rd(gb, DE);
  CYC(b_+O(57),b_+O(57)+1);push_effect(gb,b_+O(58));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(72)) { goto L_49bb; }
    else if (jt_ == b_+O(89)) { goto L_49cc; }
    else if (jt_ == b_+O(113)) { goto L_49e4; }
    else if (jt_ == b_+O(260)) { goto L_4a77; }
    else if (jt_ == b_+O(336)) { goto L_4ac3; }
    else if (jt_ == b_+O(399)) { goto L_4b02; }
    else if (jt_ == b_+O(425)) { goto L_4b1c; }
    else { HANDOFF(HL); }
  } while (0);
L_49bb:
  CYC(b_+O(72),b_+OE(73)); H = D;
  CYC(b_+O(73),b_+OE(75)); L = 0x44;
  CYC(b_+O(75),b_+OE(77)); mem_wr(gb, HL, 0x02);
  CYC(b_+O(77),b_+OE(78)); L = alu_inc8(gb, L);
  CYC(b_+O(78),b_+OE(80)); mem_wr(gb, HL, 0x00);
  CALL_L(b_+O(80), interactionCode60__checkLinkTouched_hook, b_+OE(83));
  if ((F & FC)) { CYCT(b_+O(83),b_+OE(86)); goto L_4a50; } CYC(b_+O(83),b_+OE(86));
  CYC(b_+O(86),b_+OE(89)); if (hook_is(gb, SYM(objectSetVisiblec2), objectSetVisiblec2_hook)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_49cc:
  CYC(b_+O(89),b_+OE(91)); E = 0x45;
  CYC(b_+O(91),b_+OE(92)); A = mem_rd(gb, DE);
  CYC(b_+O(92),b_+OE(93)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(93),b_+OE(95)); goto L_49de; } CYC(b_+O(93),b_+OE(95));
  CYC(b_+O(95),b_+OE(97)); A = 0x01;
  CYC(b_+O(97),b_+OE(98)); mem_wr(gb, DE, A);
  CYC(b_+O(98),b_+OE(100)); E = 0x46;
  CYC(b_+O(100),b_+OE(102)); A = 0x1e;
  CYC(b_+O(102),b_+OE(103)); mem_wr(gb, DE, A);
  CALL_C(b_+O(103), objectCreatePuff_hook, SYM(objectCreatePuff), b_+OE(106));
  if (!(F & FZ)) { CYCT(b_+O(106),b_+O(106)+1);ret_effect(gb); return; } CYC(b_+O(106),b_+OE(107));
L_49de:
  CALL_C(b_+O(107), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(110));
  if (!(F & FZ)) { CYCT(b_+O(110),b_+O(110)+1);ret_effect(gb); return; } CYC(b_+O(110),b_+OE(111));
  CYC(b_+O(111),b_+OE(113)); goto L_49bb;
L_49e4:
  CYC(b_+O(113),b_+OE(115)); E = 0x45;
  CYC(b_+O(115),b_+OE(116)); A = mem_rd(gb, DE);
  CYC(b_+O(116),b_+O(116)+1);push_effect(gb,b_+O(117));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(123)) { goto L_49ee; }
    else if (jt_ == b_+O(136)) { goto L_49fb; }
    else if (jt_ == b_+O(161)) { goto L_4a14; }
    else { HANDOFF(HL); }
  } while (0);
L_49ee:
  CYC(b_+O(123),b_+OE(125)); A = 0x01;
  CYC(b_+O(125),b_+OE(126)); mem_wr(gb, DE, A);
  CYC(b_+O(126),b_+OE(127)); H = D;
  CYC(b_+O(127),b_+OE(129)); L = 0x46;
  CYC(b_+O(129),b_+OE(131)); mem_wr(gb, HL, 0x28);
  CYC(b_+O(131),b_+OE(133)); A = 0x4d;
  CYC(b_+O(133),b_+OE(136)); if (hook_is(gb, SYM(playSound_b00), playSound_b00_hook)) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));
L_49fb:
  CALL_C(b_+O(136), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(139));
  if (!(F & FZ)) { CYCT(b_+O(139),b_+OE(140));ret_effect(gb); return; } CYC(b_+O(139),b_+OE(140));
  CYC(b_+O(140),b_+OE(142)); mem_wr(gb, HL, 0x02);
  CYC(b_+O(142),b_+OE(143)); L = alu_inc8(gb, L);
  CYC(b_+O(143),b_+OE(145)); mem_wr(gb, HL, 0x02);
  CYC(b_+O(145),b_+OE(147)); L = 0x45;
  CYC(b_+O(147),b_+OE(148)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+O(148), objectGetZAboveScreen_hook, SYM(objectGetZAboveScreen), b_+OE(151));
  CYC(b_+O(151),b_+OE(152)); H = D;
  CYC(b_+O(152),b_+OE(154)); L = 0x4f;
  CYC(b_+O(154),b_+OE(155)); mem_wr(gb, HL, A);
  CALL_C(b_+O(155), objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+OE(158));
  CYC(b_+O(158),b_+OE(161)); goto L_4a47;
L_4a14:
  CALL_L(b_+O(161), interactionCode60__checkLinkTouched_hook, b_+OE(164));
  if ((F & FC)) { CYCT(b_+O(164),b_+OE(166)); goto L_4a50; } CYC(b_+O(164),b_+OE(166));
  CALL_L(b_+O(166), interactionCode60__setVisibleIfWithinScreenBoundary_hook, b_+OE(169));
  CYC(b_+O(169),b_+OE(171)); C = 0x10;
  CALL_C(b_+O(171), objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+OE(174));
  if (!(F & FZ)) { CYCT(b_+O(174),b_+O(174)+1);ret_effect(gb); return; } CYC(b_+O(174),b_+OE(175));
  CALL_C(b_+O(175), objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+OE(178));
  if (!(F & FC)) { CYCT(b_+O(178),b_+OE(180)); goto L_4a2d; } CYC(b_+O(178),b_+OE(180));
  CYC(b_+O(180),b_+OE(181)); A = alu_dec8(gb, A);
  if ((F & FZ)) { CYCT(b_+O(181),b_+OE(183)); goto L_4a62; } CYC(b_+O(181),b_+OE(183));
  CYC(b_+O(183),b_+OE(186)); if (hook_is(gb, SYM(objectReplaceWithFallingDownHoleInteraction), objectReplaceWithFallingDownHoleInteraction_hook)) { objectReplaceWithFallingDownHoleInteraction_hook(gb); return; } HANDOFF(SYM(objectReplaceWithFallingDownHoleInteraction));
L_4a2d:
  if (game_seasons) {       // the chest chime only for treasure $30 (a small key)
    CYC(b_+S(186), b_+S(188)); E = 0x70;
    CYC(b_+S(188), b_+S(189)); A = mem_rd(gb, DE);
    CYC(b_+S(189), b_+S(191)); alu_cp(gb, 0x30);
    CYC(b_+S(191), b_+S(193)); A = 0x77;
    if (F & FZ) CALL_C_CC(b_+S(193), playSound_b00_hook, SYM(playSound_b00), b_+S(196));
    else CYC(b_+S(193), b_+S(196));
  } else {
    CYC(b_+186,b_+188); A = 0x77;
    CALL_C(b_+188, playSound_b00_hook, SYM(playSound_b00), b_+191);
  }
  CALL_C(b_+O(191), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(194));
  if ((F & FZ)) { CYCT(b_+O(194),b_+OE(196)); goto L_4a3d; } CYC(b_+O(194),b_+OE(196));
  CYC(b_+O(196),b_+OE(199)); SET_BC(0xff56);
  CYC(b_+O(199),b_+OE(202)); if (hook_is(gb, SYM(objectSetSpeedZ), objectSetSpeedZ_hook)) { objectSetSpeedZ_hook(gb); return; } HANDOFF(SYM(objectSetSpeedZ));
L_4a3d:
  CALL_C(b_+O(202), objectSetVisible_hook, SYM(objectSetVisible), b_+OE(205));
  CALL_C(b_+O(205), objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+OE(208));
  CYC(b_+O(208),b_+OE(210)); A = 0x02;
  CYC(b_+O(210),b_+OE(212)); goto L_4a55;
L_4a47:
  CALL_C(b_+O(212), objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+OE(215));
  if (!(F & FC)) { CYCT(b_+O(215),b_+OE(218)); if (hook_is(gb, SYM(objectSetInvisible), objectSetInvisible_hook)) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible)); } CYC(b_+O(215),b_+OE(218));
  CYC(b_+O(218),b_+OE(221)); if (hook_is(gb, SYM(objectSetVisible), objectSetVisible_hook)) { objectSetVisible_hook(gb); return; } HANDOFF(SYM(objectSetVisible));
L_4a50:
  CALL_L(b_+O(221), interactionCode60__giveTreasure_hook, b_+OE(224));
  CYC(b_+O(224),b_+OE(226)); A = 0x03;
L_4a55:
  CYC(b_+O(226),b_+OE(227)); H = D;
  CYC(b_+O(227),b_+OE(229)); L = 0x44;
  CYC(b_+O(229),b_+OE(230)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(230),b_+OE(231)); alu_xor(gb, A);
  CYC(b_+O(231),b_+OE(232)); mem_wr(gb, HL, A);
  CYC(b_+O(232),b_+OE(234)); L = 0x4e;
  CYC(b_+O(234),b_+OE(235)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(235),b_+OE(236)); mem_wr(gb, HL, A);
  CYC(b_+O(236),b_+OE(239)); if (hook_is(gb, SYM(interactionSetAlwaysUpdateBit), interactionSetAlwaysUpdateBit_hook)) { interactionSetAlwaysUpdateBit_hook(gb); return; } HANDOFF(SYM(interactionSetAlwaysUpdateBit));
L_4a62:
  CYC(b_+O(239),b_+OE(240)); H = D;
  CYC(b_+O(240),b_+OE(242)); L = 0x70;
  CYC(b_+O(242),b_+OE(243)); A = mem_rd(gb, HL);
  CYC(b_+O(243),b_+OE(245)); L = 0x42;
  CYC(b_+O(245),b_+OE(246)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(246),b_+OE(247)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(247),b_+OE(248)); L = alu_inc8(gb, L);
  CYC(b_+O(248),b_+OE(249)); alu_xor(gb, A);
  CYC(b_+O(249),b_+OE(250)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(250),b_+OE(251)); mem_wr(gb, HL, A);
  CYC(b_+O(251),b_+OE(253)); L = 0x5a;
  CYC(b_+O(253),b_+OE(255)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+O(255),b_+OE(257)); B = 0x03;
  CYC(b_+O(257),b_+OE(260)); if (hook_is(gb, SYM(objectCreateInteractionWithSubid00), objectCreateInteractionWithSubid00_hook)) { objectCreateInteractionWithSubid00_hook(gb); return; } HANDOFF(SYM(objectCreateInteractionWithSubid00));
L_4a77:
  CYC(b_+O(260),b_+OE(262)); A = 0x80;
  CYC(b_+O(262),b_+OE(265)); mem_wr(gb, wForceLinkPushAnimation, A);
  CYC(b_+O(265),b_+OE(267)); E = 0x45;
  CYC(b_+O(267),b_+OE(268)); A = mem_rd(gb, DE);
  CYC(b_+O(268),b_+O(268)+1);push_effect(gb,b_+O(269));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(275)) { goto L_4a86; }
    else if (jt_ == b_+O(295)) { goto L_4a9a; }
    else if (jt_ == b_+O(317)) { goto L_4ab0; }
    else { HANDOFF(HL); }
  } while (0);
L_4a86:
  CYC(b_+O(275),b_+OE(277)); A = 0x01;
  CYC(b_+O(277),b_+OE(278)); mem_wr(gb, DE, A);
  CYC(b_+O(278),b_+OE(281)); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+O(281), interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+OE(284));
  CYC(b_+O(284),b_+OE(286)); L = 0x50;
  CYC(b_+O(286),b_+OE(288)); mem_wr(gb, HL, 0x0a);
  CYC(b_+O(288),b_+OE(290)); L = 0x46;
  CYC(b_+O(290),b_+OE(292)); mem_wr(gb, HL, 0x20);
  CYC(b_+O(292),b_+OE(295)); if (hook_is(gb, SYM(objectSetVisible80), objectSetVisible80_hook)) { objectSetVisible80_hook(gb); return; } HANDOFF(SYM(objectSetVisible80));
L_4a9a:
  CALL_C(b_+O(295), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(298));
  CALL_C(b_+O(298), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(301));
  if (!(F & FZ)) { CYCT(b_+O(301),b_+O(301)+1);ret_effect(gb); return; } CYC(b_+O(301),b_+OE(302));
  CYC(b_+O(302),b_+OE(304)); L = 0x45;
  CYC(b_+O(304),b_+OE(305)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(305),b_+OE(307)); L = 0x79;
  CYC(b_+O(307),b_+OE(308)); A = mem_rd(gb, HL);
  CYC(b_+O(308),b_+OE(309)); alu_or(gb, A);
  if ((F & FZ)) { CALL_L_CC(b_+O(309), interactionCode60__giveTreasure_hook, b_+OE(312)); } else CYC(b_+O(309),b_+OE(312));
  CYC(b_+O(312),b_+OE(314)); A = 0x4c;
  CALL_C(b_+O(314), playSound_b00_hook, SYM(playSound_b00), b_+OE(317));
L_4ab0:
  CYC(b_+O(317),b_+OE(320)); A = mem_rd(gb, wOamEnd);
  CYC(b_+O(320),b_+OE(322)); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+O(322),b_+O(322)+1);ret_effect(gb); return; } CYC(b_+O(322),b_+OE(323));
  CYC(b_+O(323),b_+OE(324)); alu_xor(gb, A);
  CYC(b_+O(324),b_+OE(327)); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+O(327),b_+OE(329)); E = 0x79;
  CYC(b_+O(329),b_+OE(330)); A = mem_rd(gb, DE);
  CYC(b_+O(330),b_+OE(333)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(333),b_+OE(336)); if (hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
L_4ac3:
  CYC(b_+O(336),b_+OE(338)); E = 0x45;
  CYC(b_+O(338),b_+OE(339)); A = mem_rd(gb, DE);
  CYC(b_+O(339),b_+O(339)+1);push_effect(gb,b_+O(340));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(346)) { goto L_4acd; }
    else if (jt_ == b_+O(359)) { goto L_4ada; }
    else if (jt_ == b_+O(383)) { goto L_4af2; }
    else { HANDOFF(HL); }
  } while (0);
L_4acd:
  CYC(b_+O(346),b_+OE(348)); A = 0x01;
  CYC(b_+O(348),b_+OE(349)); mem_wr(gb, DE, A);
  CYC(b_+O(349),b_+OE(352)); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+O(352), interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+OE(355));
  CYC(b_+O(355),b_+OE(357)); L = 0x46;
  CYC(b_+O(357),b_+OE(359)); mem_wr(gb, HL, 0x0f);
L_4ada:
  CALL_C(b_+O(359), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(362));
  if (!(F & FZ)) { CYCT(b_+O(362),b_+O(362)+1);ret_effect(gb); return; } CYC(b_+O(362),b_+OE(363));
  CALL_C(b_+O(363), interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+OE(366));
  CALL_C(b_+O(366), objectSetVisible80_hook, SYM(objectSetVisible80), b_+OE(369));
  CALL_L(b_+O(369), interactionCode60__giveTreasure_hook, b_+OE(372));
  CYC(b_+O(372),b_+OE(375)); SET_BC(0x8100);
  CALL_L(b_+O(375), interactionCode60__setLinkAnimationAndDeleteIfTextClosed_hook, b_+OE(378));
  CYC(b_+O(378),b_+OE(380)); A = 0x4c;
  CYC(b_+O(380),b_+OE(383)); if (hook_is(gb, SYM(playSound_b00), playSound_b00_hook)) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));
L_4af2:
  CYC(b_+O(383),b_+OE(386)); A = mem_rd(gb, wOamEnd);
  CYC(b_+O(386),b_+OE(388)); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+O(388),b_+O(388)+1);ret_effect(gb); return; } CYC(b_+O(388),b_+OE(389));
  CYC(b_+O(389),b_+OE(390)); alu_xor(gb, A);
  CYC(b_+O(390),b_+OE(393)); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+O(393),b_+OE(396)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(396),b_+OE(399)); if (hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
L_4b02:
  CALL_L(b_+O(399), interactionCode60__checkLinkTouched_hook, b_+OE(402));
  if (!(F & FC)) { CYCT(b_+O(402),b_+O(402)+1);ret_effect(gb); return; } CYC(b_+O(402),b_+OE(403));
  CYC(b_+O(403),b_+OE(406)); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+O(406),b_+OE(408)); alu_bit(gb, 7, A);
  if ((F & FZ)) { CYCT(b_+O(408),b_+O(408)+1);ret_effect(gb); return; } CYC(b_+O(408),b_+OE(409));
  CALL_C(b_+O(409), objectSetVisible82_hook, SYM(objectSetVisible82), b_+OE(412));
  CALL_L(b_+O(412), interactionCode60__giveTreasure_hook, b_+OE(415));
  CYC(b_+O(415),b_+OE(417)); A = 0x4c;
  CALL_C(b_+O(417), playSound_b00_hook, SYM(playSound_b00), b_+OE(420));
  CYC(b_+O(420),b_+OE(422)); A = 0x03;
  CYC(b_+O(422),b_+OE(425)); goto L_4a55;
L_4b1c:
  CYC(b_+O(425),b_+OE(427)); E = 0x45;
  CYC(b_+O(427),b_+OE(428)); A = mem_rd(gb, DE);
  CYC(b_+O(428),b_+O(428)+1);push_effect(gb,b_+O(429));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(435)) { goto L_4b26; }
    else if (jt_ == b_+O(445)) { goto L_4b30; }
    else if (jt_ == b_+O(483)) { goto L_4b56; }
    else { HANDOFF(HL); }
  } while (0);
L_4b26:
  CYC(b_+O(435),b_+OE(437)); A = 0x01;
  CYC(b_+O(437),b_+OE(438)); mem_wr(gb, DE, A);
  CALL_C(b_+O(438), objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+OE(441));
  CYC(b_+O(441),b_+OE(444)); mem_wr(gb, wccaa, A);
  CYC(b_+O(444),b_+O(444)+1);ret_effect(gb); return;
L_4b30:
  CYC(b_+O(445),b_+OE(448)); A = mem_rd(gb, wScreenVariables);
  CYC(b_+O(448),b_+OE(450)); alu_and(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+O(450),b_+OE(453)); if (hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+O(450),b_+OE(453));
  CYC(b_+O(453),b_+OE(456)); A = mem_rd(gb, wccaa);
  CYC(b_+O(456),b_+OE(457)); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(457),b_+O(457)+1);ret_effect(gb); return; } CYC(b_+O(457),b_+OE(458));
  CYC(b_+O(458),b_+OE(461)); SET_BC(0xff00);
  CALL_C(b_+O(461), objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+OE(464));
  CYC(b_+O(464),b_+OE(466)); L = 0x45;
  CYC(b_+O(466),b_+OE(467)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(467),b_+OE(470)); A = mem_rd(gb, w1Link_direction);
  CYC(b_+O(470),b_+OE(472)); A = alu_swap(gb, A);
  CYC(b_+O(472),b_+OE(473)); alu_rrca(gb);
  CYC(b_+O(473),b_+OE(475)); L = 0x49;
  CYC(b_+O(475),b_+OE(476)); mem_wr(gb, HL, A);
  CYC(b_+O(476),b_+OE(478)); L = 0x50;
  CYC(b_+O(478),b_+OE(480)); mem_wr(gb, HL, 0x14);
  CYC(b_+O(480),b_+OE(483)); if (hook_is(gb, SYM(objectSetVisiblec2), objectSetVisiblec2_hook)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_4b56:
  CALL_C(b_+O(483), objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+OE(486));
  if (!(F & FC)) { CALL_C_CC(b_+O(486), objectApplySpeed_hook, SYM(objectApplySpeed), b_+OE(489)); } else CYC(b_+O(486),b_+OE(489));
  CYC(b_+O(489),b_+OE(491)); C = 0x10;
  CALL_C(b_+O(491), objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+OE(494));
  if (!(F & FZ)) { CYCT(b_+O(494),b_+O(494)+1);ret_effect(gb); return; } CYC(b_+O(494),b_+OE(495));
  CYC(b_+O(495),b_+O(495)+1);push_effect(gb,AF);
  CALL_C(b_+O(496), objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+OE(499));
  CYC(b_+O(499),b_+O(499)+1);SET_BC(pop_effect(gb));
  if ((F & FC)) { CYCT(b_+O(500),b_+OE(503)); if (hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+O(500),b_+OE(503));
  CYC(b_+O(503),b_+OE(505)); A = 0x77;
  CALL_C(b_+O(505), playSound_b00_hook, SYM(playSound_b00), b_+OE(508));
  CYC(b_+O(508),b_+OE(510)); alu_bit(gb, 4, C);
  if ((F & FZ)) { CYCT(b_+O(510),b_+O(510)+1);ret_effect(gb); return; } CYC(b_+O(510),b_+OE(511));
  CYC(b_+O(511),b_+OE(514)); goto L_4a3d;
L_4b75:
  CALL_C(b_+O(514), returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+OE(517));
  CALL_L(b_+O(517), interactionCode60__checkLinkTouched_hook, b_+OE(520));
  if (!(F & FC)) { CYCT(b_+O(520),b_+O(520)+1);ret_effect(gb); return; } CYC(b_+O(520),b_+OE(521));
  CYC(b_+O(521),b_+OE(524)); goto L_4a50;
L_4b7f:
  CYC(b_+O(524),b_+OE(526)); E = 0x72;
  CYC(b_+O(526),b_+OE(527)); A = mem_rd(gb, DE);
  CYC(b_+O(527),b_+O(527)+1);push_effect(gb,b_+O(528));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(540)) { goto L_4b8f; }
    else if (jt_ == b_+O(545)) { goto L_4b94; }
    else if (jt_ == b_+O(602)) { goto L_4bcd; }
    else { HANDOFF(HL); }
  } while (0);
L_4b8f:
  CYC(b_+O(540),b_+OE(543)); SET_BC(0x80fc);
  CYC(b_+O(543),b_+OE(545)); goto L_4b97;
L_4b94:
  CYC(b_+O(545),b_+OE(548)); SET_BC(0x8100);
L_4b97:
  CYC(b_+O(548),b_+OE(550)); E = 0x45;
  CYC(b_+O(550),b_+OE(551)); A = mem_rd(gb, DE);
  CYC(b_+O(551),b_+OE(552)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(552),b_+OE(554)); goto L_4bbd; } CYC(b_+O(552),b_+OE(554));
  CYC(b_+O(554),b_+OE(555)); A = alu_inc8(gb, A);
  CYC(b_+O(555),b_+OE(556)); mem_wr(gb, DE, A);
L_4b9f:
  CYC(b_+O(556),b_+OE(558)); A = 0x04;
  CYC(b_+O(558),b_+OE(561)); mem_wr(gb, wLinkForceState, A);
  CYC(b_+O(561),b_+OE(562)); A = B;
  CYC(b_+O(562),b_+OE(565)); mem_wr(gb, wcc50, A);
  CYC(b_+O(565),b_+OE(568)); SET_HL(wDisabledObjects);
  CYC(b_+O(568),b_+OE(570)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(b_+O(570),b_+OE(573)); SET_HL(w1Link);
  CYC(b_+O(573),b_+OE(575)); B = 0xf2;
  CALL_C(b_+O(575), objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+OE(578));
  CALL_C(b_+O(578), objectSetVisible80_hook, SYM(objectSetVisible80), b_+OE(581));
  CYC(b_+O(581),b_+OE(583)); A = 0x4c;
  CALL_C(b_+O(583), playSound_b00_hook, SYM(playSound_b00), b_+OE(586));
L_4bbd:
  CALL_C(b_+O(586), retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+OE(589));
  CYC(b_+O(589),b_+OE(592)); SET_HL(wDisabledObjects);
  CYC(b_+O(592),b_+OE(594)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  CYC(b_+O(594),b_+OE(596)); A = 0x0f;
  CYC(b_+O(596),b_+OE(599)); mem_wr(gb, wInstrumentsDisabledCounter, A);
  CYC(b_+O(599),b_+OE(602)); if (hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
L_4bcd:
  CYC(b_+O(602),b_+OE(604)); A = 0x78;
  CYC(b_+O(604),b_+OE(607)); mem_wr(gb, wInstrumentsDisabledCounter, A);
  CYC(b_+O(607),b_+OE(609)); E = 0x45;
  CYC(b_+O(609),b_+OE(610)); A = mem_rd(gb, DE);
  CYC(b_+O(610),b_+O(610)+1);push_effect(gb,b_+O(611));
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+O(619)) { goto L_4bde; }
    else if (jt_ == b_+O(642)) { goto L_4bf5; }
    else if (jt_ == b_+O(655)) { goto L_4c02; }
    else if (jt_ == b_+O(695)) { goto L_4c2a; }
    else { HANDOFF(HL); }
  } while (0);
L_4bde:
  CYC(b_+O(619),b_+OE(621)); A = 0x01;
  CYC(b_+O(621),b_+OE(622)); mem_wr(gb, DE, A);
  CYC(b_+O(622),b_+OE(623)); E = alu_inc8(gb, E);
  CYC(b_+O(623),b_+OE(625)); A = 0x04;
  CYC(b_+O(625),b_+OE(626)); mem_wr(gb, DE, A);
  CYC(b_+O(626),b_+OE(628)); A = 0x81;
  CYC(b_+O(628),b_+OE(631)); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+O(631),b_+OE(633)); A = 0xff;
  CALL_C(b_+O(633), setLinkForceStateToState08_withParam_hook, SYM(setLinkForceStateToState08_withParam), b_+OE(636));
  CYC(b_+O(636),b_+OE(639)); SET_HL(wLinkForceState);
  CYC(b_+O(639),b_+OE(642)); if (hook_is(gb, SYM(objectSetInvisible), objectSetInvisible_hook)) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));
L_4bf5:
  CALL_C(b_+O(642), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(645));
  if (!(F & FZ)) { CYCT(b_+O(645),b_+O(645)+1);ret_effect(gb); return; } CYC(b_+O(645),b_+OE(646));
  CYC(b_+O(646),b_+OE(648)); L = 0x45;
  CYC(b_+O(648),b_+OE(649)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(649),b_+OE(651)); A = 0xff;
  CYC(b_+O(651),b_+OE(654)); mem_wr(gb, wcc63, A);
  CYC(b_+O(654),b_+O(654)+1);ret_effect(gb); return;
L_4c02:
  CYC(b_+O(655),b_+OE(658)); A = mem_rd(gb, wcc63);
  CYC(b_+O(658),b_+OE(659)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(659),b_+O(659)+1);ret_effect(gb); return; } CYC(b_+O(659),b_+OE(660));
  CYC(b_+O(660),b_+OE(662)); A = 0x0e;
  CYC(b_+O(662),b_+OE(665)); mem_wr(gb, wcc50, A);
  CYC(b_+O(665),b_+OE(667)); E = 0x4b;
  CYC(b_+O(667),b_+OE(670)); A = mem_rd(gb, w1Link_yh);
  CYC(b_+O(670),b_+OE(672)); alu_sub(gb, 0x0e);
  CYC(b_+O(672),b_+OE(673)); mem_wr(gb, DE, A);
  CYC(b_+O(673),b_+OE(675)); E = 0x4d;
  CYC(b_+O(675),b_+OE(678)); A = mem_rd(gb, w1Link_xh);
  CYC(b_+O(678),b_+OE(680)); alu_sub(gb, 0x04);
  CYC(b_+O(680),b_+OE(681)); mem_wr(gb, DE, A);
  CALL_C(b_+O(681), objectSetVisible_hook, SYM(objectSetVisible), b_+OE(684));
  CALL_C(b_+O(684), objectSetVisible80_hook, SYM(objectSetVisible80), b_+OE(687));
  CALL_C(b_+O(687), interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+OE(690));
  CYC(b_+O(690),b_+OE(692)); A = 0xab;
  CYC(b_+O(692),b_+OE(695)); if (hook_is(gb, SYM(playSound_b00), playSound_b00_hook)) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));
L_4c2a:
  CYC(b_+O(695),b_+OE(698)); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+O(698),b_+OE(699)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(699),b_+OE(700));ret_effect(gb); return; } CYC(b_+O(699),b_+OE(700));
  CYC(b_+O(700),b_+OE(703)); if (hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
}

void interactionCode60__giveTreasure_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4c32:
  CYC(b_+O(703),b_+OE(705)); E = 0x74;
  CYC(b_+O(705),b_+OE(706)); A = mem_rd(gb, DE);
  CYC(b_+O(706),b_+OE(707)); C = A;
  CYC(b_+O(707),b_+OE(709)); E = 0x70;
  CYC(b_+O(709),b_+OE(710)); A = mem_rd(gb, DE);
  CYC(b_+O(710),b_+OE(711)); B = A;
  CYC(b_+O(711),b_+OE(713)); alu_cp(gb, 0x37);
  if (!(F & FZ)) { CYCT(b_+O(713),b_+OE(715)); goto L_4c4d; } CYC(b_+O(713),b_+OE(715));
  CYC(b_+O(715),b_+OE(717)); A = 0x26;
  CALL_C(b_+O(717), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(720));
  if ((F & FZ)) { CYCT(b_+O(720),b_+OE(722)); goto L_4c4c; } CYC(b_+O(720),b_+OE(722));
  CYC(b_+O(722),b_+OE(724)); A = 0x27;
  CALL_C(b_+O(724), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(727));
  if (!(F & FZ)) { CYCT(b_+O(727),b_+OE(729)); goto L_4c4d; } CYC(b_+O(727),b_+OE(729));
L_4c4c:
  CYC(b_+O(729),b_+OE(730)); C = alu_inc8(gb, C);
L_4c4d:
  CYC(b_+O(730),b_+OE(731)); A = B;
  CALL_C(b_+O(731), giveTreasure_hook, SYM(giveTreasure), b_+OE(734));
  CYC(b_+O(734),b_+OE(735)); B = A;
  CYC(b_+O(735),b_+OE(737)); E = 0x72;
  CYC(b_+O(737),b_+OE(738)); A = mem_rd(gb, DE);
  CYC(b_+O(738),b_+OE(740)); alu_cp(gb, 0x03);
  if ((F & FZ)) { CYCT(b_+O(740),b_+OE(742)); goto L_4c5d; } CYC(b_+O(740),b_+OE(742));
  CYC(b_+O(742),b_+OE(743)); A = B;
  CALL_C(b_+O(743), playSound_b00_hook, SYM(playSound_b00), b_+OE(746));
L_4c5d:
  CYC(b_+O(746),b_+OE(748)); E = 0x75;
  CYC(b_+O(748),b_+OE(749)); A = mem_rd(gb, DE);
  CYC(b_+O(749),b_+OE(751)); alu_cp(gb, 0xff);
  if ((F & FZ)) { CYCT(b_+O(751),b_+OE(753)); goto L_4c7d; } CYC(b_+O(751),b_+OE(753));
  CYC(b_+O(753),b_+OE(754)); C = A;
  CYC(b_+O(754),b_+OE(756)); B = 0x00;
  CALL_C(b_+O(756), showText_hook, SYM(showText), b_+OE(759));
  CYC(b_+O(759),b_+OE(761)); A = mem_rd(gb, hCameraY);
  CYC(b_+O(761),b_+OE(762)); B = A;
  CYC(b_+O(762),b_+OE(765)); A = mem_rd(gb, w1Link_yh);
  CYC(b_+O(765),b_+OE(766)); alu_sub(gb, B);
  CYC(b_+O(766),b_+OE(768)); alu_sub(gb, 0x10);
  CYC(b_+O(768),b_+OE(770)); alu_cp(gb, 0x48);
  CYC(b_+O(770),b_+OE(772)); A = 0x02;
  if ((F & FC)) { CYCT(b_+O(772),b_+OE(774)); goto L_4c7a; } CYC(b_+O(772),b_+OE(774));
  CYC(b_+O(774),b_+OE(775)); alu_xor(gb, A);
L_4c7a:
  CYC(b_+O(775),b_+OE(778)); mem_wr(gb, wTextboxPosition, A);
L_4c7d:
  CYC(b_+O(778),b_+OE(780)); E = 0x73;
  CYC(b_+O(780),b_+OE(781)); A = mem_rd(gb, DE);
  CYC(b_+O(781),b_+OE(782)); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+O(782),b_+O(782)+1);ret_effect(gb); return; } CYC(b_+O(782),b_+OE(783));
  CALL_C(b_+O(783), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+OE(786));
  CYC(b_+O(786),b_+OE(788)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CYC(b_+O(788),b_+O(788)+1);ret_effect(gb); return;
}

void interactionCode60__checkLinkTouched_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4c88:
  CYC(b_+O(789),b_+OE(792)); A = mem_rd(gb, wLinkForceState);
  CYC(b_+O(792),b_+OE(793)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(793),b_+O(793)+1);ret_effect(gb); return; } CYC(b_+O(793),b_+OE(794));
  CYC(b_+O(794),b_+OE(797)); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+O(797),b_+OE(798)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(798),b_+O(798)+1);ret_effect(gb); return; } CYC(b_+O(798),b_+OE(799));
  CYC(b_+O(799),b_+OE(802)); A = mem_rd(gb, w1Link_state);
  CYC(b_+O(802),b_+OE(804)); alu_cp(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+O(804),b_+OE(806)); goto L_4c9d; } CYC(b_+O(804),b_+OE(806));
  CYC(b_+O(806),b_+OE(808)); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+O(808),b_+OE(810)); goto L_4cac; } CYC(b_+O(808),b_+OE(810));
L_4c9d:
  CYC(b_+O(810),b_+OE(813)); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+O(813),b_+OE(814)); alu_rrca(gb);
  if ((F & FC)) { CYCT(b_+O(814),b_+OE(816)); goto L_4cac; } CYC(b_+O(814),b_+OE(816));
  CYC(b_+O(816),b_+OE(818)); E = 0x6a;
  CYC(b_+O(818),b_+OE(819)); A = mem_rd(gb, DE);
  CYC(b_+O(819),b_+OE(820)); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+O(820),b_+OE(823)); if (hook_is(gb, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook)) { objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook(gb); return; } HANDOFF(SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing)); } CYC(b_+O(820),b_+OE(823));
  CYC(b_+O(823),b_+OE(824)); alu_scf(gb);
  CYC(b_+O(824),b_+O(824)+1);ret_effect(gb); return;
L_4cac:
  CYC(b_+O(825),b_+OE(826)); alu_xor(gb, A);
  CYC(b_+O(826),b_+O(826)+1);ret_effect(gb); return;
}

void interactionCode60__setVisibleIfWithinScreenBoundary_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+O(212), objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+OE(215));
  if (!(F & FC)) { CYCT(b_+O(215), b_+OE(218)); objectSetInvisible_hook(gb); return; }
  CYC(b_+O(215), b_+OE(218));
  CYC(b_+O(218), b_+OE(221)); TAIL(objectSetVisible);
}

void interactionCode60__setLinkAnimationAndDeleteIfTextClosed_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(556), b_+OE(558)); A = 0x04;
  CYC(b_+O(558), b_+OE(561)); mem_wr(gb, wLinkForceState, A);
  CYC(b_+O(561), b_+OE(562)); A = B;
  CYC(b_+O(562), b_+OE(565)); mem_wr(gb, wcc50, A);
  CYC(b_+O(565), b_+OE(568)); SET_HL(wDisabledObjects);
  CYC(b_+O(568), b_+OE(570)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 1));
  CYC(b_+O(570), b_+OE(573)); SET_HL(w1Link);
  CYC(b_+O(573), b_+OE(575)); B = 0xf2;
  CALL_C(b_+O(575), objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+OE(578));
  CALL_C(b_+O(578), objectSetVisible80_hook, SYM(objectSetVisible80), b_+OE(581));
  CYC(b_+O(581), b_+OE(583)); A = 0x4c;
  CALL_C(b_+O(583), playSound_b00_hook, SYM(playSound_b00), b_+OE(586));
  CALL_C(b_+O(586), retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+OE(589));
  CYC(b_+O(589), b_+OE(592)); SET_HL(wDisabledObjects);
  CYC(b_+O(592), b_+OE(594)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~1));
  CYC(b_+O(594), b_+OE(596)); A = 0x0f;
  CYC(b_+O(596), b_+OE(599)); mem_wr(gb, wInstrumentsDisabledCounter, A);
  CYC(b_+O(599), b_+OE(602)); TAIL(interactionDelete);
}
