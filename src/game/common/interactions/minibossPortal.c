#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode7e_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void interactionCode7e_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void interactionCode7e_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_MINIBOSS_PORTAL
void interactionCode7e_hook(GB *gb) {
  BASE(interactionCode7e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+O(8)) goto subid00;
    if (target == b_+O(228)) goto subid01;
    if (game_seasons && target == b_+S(261)) goto subid02;
    hook_continue(gb, target, sp0_); return;
  }

subid00:
  CYC(b_+O(8), b_+OE(10)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(10), b_+OE(11)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(11), b_+OE(12)); push_effect(gb, b_+OE(12));
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+O(20)) goto minibossState0;
    if (target == b_+O(70)) goto state1;
    if (target == b_+O(122)) goto state2;
    if (target == b_+O(135)) goto minibossState3;
    hook_continue(gb, target, sp0_); return;
  }

minibossState0:
  CYC(b_+O(20), b_+OE(23)); A = W8(wDungeonIndex);
  CYC(b_+O(23), b_+OE(26)); SET_HL(b_+O(181)); // interactionCode7e@dungeonRoomTable
  CYC(b_+O(26), b_+OE(27)); interactionCode7e_addDoubleIndexToHl_from_rst(gb, b_+OE(27));
  CYC(b_+O(27), b_+OE(28)); C = mem_rd(gb, HL);
  CYC(b_+O(28), b_+OE(31)); A = W8(wActiveGroup);
  CYC(b_+O(31), b_+OE(34)); SET_HL(SYM(flagLocationGroupTable)); // flagLocationGroupTable
  CYC(b_+O(34), b_+OE(35)); interactionCode7e_addAToHl_from_rst(gb, b_+OE(35));
  CYC(b_+O(35), b_+OE(36)); H = mem_rd(gb, HL);
  CYC(b_+O(36), b_+OE(37)); L = C;
  CYC(b_+O(37), b_+OE(38)); A = mem_rd(gb, HL);
  CYC(b_+O(38), b_+OE(40)); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+O(40), b_+OE(43)); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+O(40), b_+OE(43));
  CYC(b_+O(43), b_+OE(45)); C = 0x57;
  CALL_C(b_+O(45), objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+OE(48));

commonState0:
  CALL_C(b_+O(48), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(51));
  CYC(b_+O(51), b_+OE(53)); A = 0x03;
  CALL_C(b_+O(53), objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+OE(56));
  CALL_C(b_+O(56), objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+OE(59));
  CYC(b_+O(59), b_+OE(61)); A = 0x01;
  if (!(F & FC)) { CYCT(b_+O(61), b_+OE(63)); goto afterIncA; } // jr nc
  CYC(b_+O(61), b_+OE(63));
  CYC(b_+O(63), b_+OE(64)); A = alu_inc8(gb, A);

afterIncA:
  CYC(b_+O(64), b_+OE(66)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(66), b_+OE(67)); mem_wr(gb, DE, A);
  CYC(b_+O(67), b_+OE(70)); TAIL(objectSetVisible83); // jp

state1:
  CALL_C(b_+O(70), interactionAnimate_hook, SYM(interactionAnimate), b_+OE(73));
  CALL_C(b_+O(73), objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+OE(76));
  if (!(F & FC)) { RET_TAKEN(b_+O(76)); return; } // ret nc
  CYC(b_+O(76), b_+OE(77));
  CYC(b_+O(77), b_+OE(80)); A = W8(w1Link_id);
  CYC(b_+O(80), b_+OE(81)); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+O(81), checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+OE(84)); else CYC(b_+O(81), b_+OE(84)); // call z
  if (!(F & FC)) { RET_TAKEN(b_+O(84)); return; } // ret nc
  CYC(b_+O(84), b_+OE(85));
  CALL_C(b_+O(85), resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+OE(88));
  CYC(b_+O(88), b_+OE(90)); A = 0x03;
  CYC(b_+O(90), b_+OE(92)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(92), b_+OE(93)); mem_wr(gb, DE, A);
  CYC(b_+O(93), b_+OE(96)); W8(wLinkCanPassNpcs) = A;
  CYC(b_+O(96), b_+OE(98)); A = 0x30;
  CYC(b_+O(98), b_+OE(100)); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(100), b_+OE(101)); mem_wr(gb, DE, A);
  CALL_C(b_+O(101), setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+OE(104));
  CYC(b_+O(104), b_+OE(107)); SET_HL(w1Link_visible);
  CYC(b_+O(107), b_+OE(109)); mem_wr(gb, HL, 0x82);
  CALL_C(b_+O(109), objectCopyPosition_hook, SYM(objectCopyPosition), b_+OE(112));
  CYC(b_+O(112), b_+OE(114)); A = 0x01;
  CYC(b_+O(114), b_+OE(117)); W8(wDisabledObjects) = A;
  CYC(b_+O(117), b_+OE(119)); A = 0x8d; // SND_TELEPORT
  CYC(b_+O(119), b_+OE(122)); TAIL(playSound_b00); // jp

state2:
  CALL_C(b_+O(122), interactionAnimate_hook, SYM(interactionAnimate), b_+OE(125));
  CALL_C(b_+O(125), objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+OE(128));
  if (F & FC) { RET_TAKEN(b_+O(128)); return; } // ret c
  CYC(b_+O(128), b_+OE(129));
  CYC(b_+O(129), b_+OE(131)); A = 0x01;
  CYC(b_+O(131), b_+OE(133)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(133), b_+OE(134)); mem_wr(gb, DE, A);
  RET(b_+O(134)); return; // ret

minibossState3:
  CYC(b_+O(135), b_+OE(138)); SET_HL(w1Link);
  CALL_C(b_+O(138), objectCopyPosition_hook, SYM(objectCopyPosition), b_+OE(141));
  CYC(b_+O(141), b_+OE(144)); push_effect(gb, b_+OE(144)); goto spinLink;
afterSpin1:
  if (!(F & FZ)) { RET_TAKEN(b_+O(144)); return; } // ret nz
  CYC(b_+O(144), b_+OE(145));
  CYC(b_+O(145), b_+OE(148)); A = W8(wDungeonIndex);
  CYC(b_+O(148), b_+OE(151)); SET_HL(b_+O(181)); // interactionCode7e@dungeonRoomTable
  CYC(b_+O(151), b_+OE(152)); interactionCode7e_addDoubleIndexToHl_from_rst(gb, b_+OE(152));
  CYC(b_+O(152), b_+OE(153)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(153), b_+OE(154)); C = mem_rd(gb, HL);
  CYC(b_+O(154), b_+OE(155)); B = A;
  CYC(b_+O(155), b_+OE(158)); SET_HL(wWarpDestGroup);
  CYC(b_+O(158), b_+OE(161)); A = W8(wActiveGroup);
  CYC(b_+O(161), b_+OE(163)); alu_or(gb, 0x80);
  CYC(b_+O(163), b_+OE(164)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(164), b_+OE(167)); A = W8(wActiveRoom);
  CYC(b_+O(167), b_+OE(168)); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+O(168), b_+OE(170)); goto afterRoomCheck; } // jr nz
  CYC(b_+O(168), b_+OE(170));
  CYC(b_+O(170), b_+OE(171)); B = C;

afterRoomCheck:
  CYC(b_+O(171), b_+OE(172)); A = B;
  CYC(b_+O(172), b_+OE(173)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(173), b_+OE(174)); alu_xor(gb, A);
  CYC(b_+O(174), b_+OE(175)); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+O(175), b_+OE(177)); mem_wr(gb, HL, 0x57);
  CYC(b_+O(177), b_+OE(178)); L = alu_inc8(gb, L);
  CYC(b_+O(178), b_+OE(180)); mem_wr(gb, HL, 0x03);
  RET(b_+O(180)); return; // ret

subid01:
  CYC(b_+O(228), b_+OE(230)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(230), b_+OE(231)); A = mem_rd(gb, DE);
  {
    CYC(b_+O(231), b_+OE(232)); push_effect(gb, b_+OE(232));
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+O(240)) goto herosCaveState0;
    if (target == b_+O(70)) goto state1;
    if (target == b_+O(122)) goto state2;
    if (target == b_+O(270)) goto herosCaveState3;
    hook_continue(gb, target, sp0_); return;
  }

herosCaveState0:
  if (!game_seasons) {
    CALL_C(b_+240, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+243);
    CYC(b_+243, b_+245); E = INTERACTION_BASE + OBJ_XH;
    CYC(b_+245, b_+246); A = mem_rd(gb, DE);
    CYC(b_+246, b_+248); E = INTERACTION_BASE + OBJ_VAR03;
    CYC(b_+248, b_+249); mem_wr(gb, DE, A);
    CYC(b_+249, b_+251); alu_bit(gb, 7, A);
    if (F & FZ) { CYCT(b_+251, b_+253); goto afterRoomFlagCheck; } // jr z
    CYC(b_+251, b_+253);
    CALL_C(b_+253, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+256);
    CYC(b_+256, b_+258); alu_and(gb, 0x20);
    if (F & FZ) { RET_TAKEN(b_+258); return; } // ret z
    CYC(b_+258, b_+259);

afterRoomFlagCheck:
    CYC(b_+259, b_+260); H = D;
    CYC(b_+260, b_+262); E = INTERACTION_BASE + OBJ_YH;
    CYC(b_+262, b_+263); L = E;
    CYC(b_+263, b_+264); A = mem_rd(gb, DE);
    CALL_C(b_+264, setShortPosition_hook, SYM(setShortPosition), b_+267);
  } else {
    CYC(b_+S(242), b_+S(245)); A = W8(wc64a);
    CYC(b_+S(245), b_+S(246)); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+S(246), b_+S(249)); TAIL(interactionDelete); }
    CYC(b_+S(246), b_+S(249));
  }
  CYC(b_+O(267), b_+OE(270)); goto commonState0; // jp

herosCaveState3:
  CYC(b_+O(270), b_+OE(273)); push_effect(gb, b_+OE(273)); goto spinLink;
afterSpin2:
  if (!(F & FZ)) { RET_TAKEN(b_+O(273)); return; } // ret nz
  CYC(b_+O(273), b_+OE(274));
  if (game_seasons) {
    CYC(b_+S(256), b_+S(259)); A = W8(wc64a);
    CYC(b_+S(259), b_+S(261)); goto initHerosCaveWarp;
  }
  if (!game_seasons) {
    CYC(b_+274, b_+276); E = INTERACTION_BASE + OBJ_VAR03;
    CYC(b_+276, b_+277); A = mem_rd(gb, DE);
    CYC(b_+277, b_+279); alu_and(gb, 0x0f);
    CYC(b_+279, b_+282); push_effect(gb, b_+282); goto initHerosCaveWarp;
afterInitWarp:
    CYC(b_+282, b_+284); A = 0x84;
    CYC(b_+284, b_+287); W8(wWarpDestGroup) = A;
    RET(b_+287); return; // ret
  }

// @subid02, Seasons only: the portal in the second half of Hero's Cave.
subid02:
  CYC(b_+S(261), b_+S(263)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+S(263), b_+S(264)); A = mem_rd(gb, DE);
  {
    CYC(b_+S(264), b_+S(265)); push_effect(gb, b_+S(265));
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+S(273)) goto herosCave2State0;
    if (target == b_+O(70)) goto state1;
    if (target == b_+O(122)) goto state2;
    if (target == b_+S(284)) goto herosCave2State3;
    hook_continue(gb, target, sp0_); return;
  }

herosCave2State0:
  CALL_C(b_+S(273), getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+S(276));
  CYC(b_+S(276), b_+S(278)); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+S(278), b_+S(281)); TAIL(interactionDelete); }
  CYC(b_+S(278), b_+S(281));
  CYC(b_+S(281), b_+S(284)); goto commonState0;

herosCave2State3:
  CYC(b_+S(284), b_+S(287)); push_effect(gb, b_+S(287)); goto spinLink;
afterSpin3:
  if (!(F & FZ)) { RET_TAKEN(b_+S(287)); return; }
  CYC(b_+S(287), b_+S(288));
  CYC(b_+S(288), b_+S(289)); alu_xor(gb, A);
  goto initHerosCaveWarp;

// interactionCode7e@spinLink: reached by two genuine calls, from @minibossState3's return
// address 0x44e2 above and @herosCaveState3's return address 0x4563 above; never separately
// hooked. Has both a literal ret exit and a tail-jump-into-external-hook exit, so every exit
// needs both callers' resume checks.
spinLink:
  CALL_C(b_+O(199), resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+OE(202));
  CALL_C(b_+O(202), interactionAnimate_hook, SYM(interactionAnimate), b_+OE(205));
  CYC(b_+O(205), b_+OE(208)); A = W8(wLinkDeathTrigger);
  CYC(b_+O(208), b_+OE(209)); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(b_+O(209));
    if (gb->pc == b_+O(144) && gb->sp == sp0_) goto afterSpin1;
    if (gb->pc == b_+O(273) && gb->sp == sp0_) goto afterSpin2;
    if (game_seasons && gb->pc == b_+S(287) && gb->sp == sp0_) goto afterSpin3;
    return;
  } // ret nz
  CYC(b_+O(209), b_+OE(210));
  CYC(b_+O(210), b_+OE(213)); A = W8(wFrameCounter);
  CYC(b_+O(213), b_+OE(215)); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+O(215), b_+OE(217)); goto afterDirUpdate; } // jr nz
  CYC(b_+O(215), b_+OE(217));
  CYC(b_+O(217), b_+OE(220)); SET_HL(w1Link_direction);
  CYC(b_+O(220), b_+OE(221)); A = mem_rd(gb, HL);
  CYC(b_+O(221), b_+OE(222)); A = alu_inc8(gb, A);
  CYC(b_+O(222), b_+OE(224)); alu_and(gb, 0x03);
  CYC(b_+O(224), b_+OE(225)); mem_wr(gb, HL, A);

afterDirUpdate:
  CYC(b_+O(225), b_+OE(228)); interactionDecCounter1_hook(gb); // jp
  if (gb->pc == b_+O(144) && gb->sp == sp0_) goto afterSpin1;
  if (gb->pc == b_+O(273) && gb->sp == sp0_) goto afterSpin2;
  if (game_seasons && gb->pc == b_+S(287) && gb->sp == sp0_) goto afterSpin3;
  return;

// interactionCode7e@initHerosCaveWarp: reached by one genuine call, from @herosCaveState3's
// return address 0x456c above; never separately hooked.
initHerosCaveWarp:
  CYC(b_+O(288), b_+OE(291)); SET_HL(b_+O(315)); // interactionCode7e@herosCaveWarps
  CYC(b_+O(291), b_+OE(292)); interactionCode7e_addDoubleIndexToHl_from_rst(gb, b_+OE(292));
  CYC(b_+O(292), b_+OE(293)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(293), b_+OE(296)); W8(wWarpDestRoom) = A;
  CYC(b_+O(296), b_+OE(297)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(297), b_+OE(300)); W8(wWarpDestPos) = A;
  CYC(b_+O(300), b_+OE(302)); A = 0x85;
  CYC(b_+O(302), b_+OE(305)); W8(wWarpDestGroup) = A;
  CYC(b_+O(305), b_+OE(306)); alu_xor(gb, A);
  CYC(b_+O(306), b_+OE(309)); W8(wWarpTransition) = A;
  CYC(b_+O(309), b_+OE(311)); A = 0x03;
  CYC(b_+O(311), b_+OE(314)); W8(wWarpTransition2) = A;
  RET(b_+O(314));
  if (!game_seasons && gb->pc == b_+282 && gb->sp == sp0_) goto afterInitWarp;
  return;
}
