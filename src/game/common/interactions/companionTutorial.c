#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCoded0_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded0_addAToHl(GB *gb, uint16_t return_address) {
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

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCoded0_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCoded0@cpYToCompanion (0b:4bee): true local, called (real `call`) twice from
// @setFlagAndDeleteWhenCompanionIsAbove and @setFlagAndDeleteWhenCompanionIsBelowOrRight.
static void interactionCoded0_cpYToCompanion(GB *gb) {
  BASE(interactionCoded0);
  CYC(b_+O(186), b_+OE(188)); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(188), b_+OE(189)); A = mem_rd(gb, DE);
  CYC(b_+O(189), b_+OE(192)); SET_HL(w1Companion_yh);
  CYC(b_+O(192), b_+OE(193)); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+O(193)); return;
}

// interactionCoded0@checkLinkInXRange (0b:4bf6): true local, called (real `call`) once from
// @setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange.
// @param[out] zflag z if Link is within a certain range of X-positions for certain rooms?
static void interactionCoded0_checkLinkInXRange(GB *gb) {
  BASE(interactionCoded0);
  CYC(b_+O(194), b_+OE(197)); A = mem_rd(gb, wActiveRoom);
  CYC(b_+O(197), b_+OE(200)); SET_HL(b_+O(228)); // @rooms
  CYC(b_+O(200), b_+OE(202)); B = 0x00;

roomLoop:
  CYC(b_+O(202), b_+OE(203)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(203), b_+OE(205)); goto foundRoom; } // jr z
  CYC(b_+O(203), b_+OE(205));
  CYC(b_+O(205), b_+OE(206)); B = alu_inc8(gb, B);
  CYC(b_+O(206), b_+OE(207)); SET_HL(HL + 1);
  CYC(b_+O(207), b_+OE(209)); goto roomLoop; // jr $4bfe

foundRoom:
  CYC(b_+O(209), b_+OE(210)); A = B;
  CYC(b_+O(210), b_+OE(213)); SET_HL(b_+O(231)); // @xRanges
  CYC(b_+O(213), b_+OE(214)); interactionCoded0_addDoubleIndex(gb, b_+O(214));
  CYC(b_+O(214), b_+OE(217)); A = mem_rd(gb, w1Link_xh);
  CYC(b_+O(217), b_+OE(218)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+O(218), b_+OE(220)); goto notInRange; } // jr c
  CYC(b_+O(218), b_+OE(220));
  CYC(b_+O(220), b_+OE(221)); SET_HL(HL + 1);
  CYC(b_+O(221), b_+OE(222)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+O(222), b_+OE(224)); goto notInRange; } // jr nc
  CYC(b_+O(222), b_+OE(224));
  CYC(b_+O(224), b_+OE(225)); alu_xor(gb, A);
  RET(b_+O(225)); return;

notInRange:
  CYC(b_+O(226), b_+OE(227)); alu_or(gb, D);
  RET(b_+O(227)); return;
}

// ==================================================================================================
// INTERAC_COMPANION_TUTORIAL
//
// @state2's inner jump table has 6 entries for the ages build; two of them (index 2 and 5) both
// target @setFlagAndDeleteWhenCompanionIsBelow, which collapses to a single switch case here.
// @setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero is dead in the ages build (no jump-table
// entry reaches it) but its bytes are still present in ROM, falling straight through into
// @setFlagAndDeleteWhenCompanionIsAbove; ported below for completeness even though unreachable.
// ==================================================================================================
void interactionCoded0_hook(GB *gb) {
  BASE(interactionCoded0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (interactionCoded0_jump_table(gb));
    if (jt_ == b_+O(10)) { goto state0; }
    else if (jt_ == b_+O(14)) { goto state1; }
    else if (jt_ == b_+O(99)) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+O(10), b_+OE(12)); A = 0x01;
  CYC(b_+O(12), b_+OE(13)); mem_wr(gb, DE, A);
  RET(b_+O(13)); return;

state1:
  CYC(b_+O(14), b_+OE(16)); A = 0x02;
  CYC(b_+O(16), b_+OE(17)); mem_wr(gb, DE, A);
  CYC(b_+O(17), b_+OE(20)); A = mem_rd(gb, w1Companion_enabled);
  CYC(b_+O(20), b_+OE(21)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(21), b_+OE(23)); goto deleteIfSubid2Or5; } // jr z
  CYC(b_+O(21), b_+OE(23));

  // Verify that the correct companion is on-screen, otherwise delete self
  CYC(b_+O(23), b_+OE(25)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(25), b_+OE(26)); A = mem_rd(gb, DE);
  CYC(b_+O(26), b_+OE(28)); A = alu_srl(gb, A);
  CYC(b_+O(28), b_+OE(30)); alu_add(gb, 0x0b); // SPECIALOBJECT_FIRST_COMPANION
  CYC(b_+O(30), b_+OE(32)); alu_cp(gb, 0x0e); // SPECIALOBJECT_LAST_COMPANION+1
  if (F & FC) { CYCT(b_+O(32), b_+OE(34)); goto afterMoosh; } // jr c
  CYC(b_+O(32), b_+OE(34));
  CYC(b_+O(34), b_+OE(36)); A = 0x0d; // SPECIALOBJECT_MOOSH

afterMoosh:
  CYC(b_+O(36), b_+OE(39)); SET_HL(w1Companion_id);
  CYC(b_+O(39), b_+OE(40)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(40), b_+OE(42)); goto delete; } // jr nz

  // Delete self if tutorial text was already shown
  CYC(b_+O(40), b_+OE(42));
  CYC(b_+O(42), b_+OE(43)); A = mem_rd(gb, DE);
  CYC(b_+O(43), b_+OE(46)); SET_HL(b_+(game_seasons ? S(222) : 249)); // @flagNumbers
  CYC(b_+O(46), b_+OE(47)); interactionCoded0_addAToHl(gb, b_+O(47));
  CYC(b_+O(47), b_+OE(48)); A = mem_rd(gb, HL);
  CYC(b_+O(48), b_+OE(51)); SET_HL(wCompanionTutorialTextShown);
  CALL_C(b_+O(51), checkFlag_hook, SYM(checkFlag), b_+OE(54));
  if (!(F & FZ)) { CYCT(b_+O(54), b_+OE(56)); goto delete; } // jr nz
  CYC(b_+O(54), b_+OE(56));

  // Check whether to dismount? (subid 2 only)
  CYC(b_+O(56), b_+OE(57)); A = mem_rd(gb, DE);
  CYC(b_+O(57), b_+OE(59)); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+O(59), b_+OE(61)); goto afterDismountCheck; } // jr nz
  CYC(b_+O(59), b_+OE(61));
  CYC(b_+O(61), b_+OE(64)); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+O(64), b_+OE(65)); alu_rra(gb);
  CYC(b_+O(65), b_+OE(66)); A = mem_rd(gb, DE);
  if (!(F & FC)) { CYCT(b_+O(66), b_+OE(68)); goto afterDismountCheck; } // jr nc
  CYC(b_+O(66), b_+OE(68));
  CYC(b_+O(68), b_+OE(71)); mem_wr(gb, wForceCompanionDismount, A);

afterDismountCheck:
  CYC(b_+O(71), b_+OE(74)); SET_HL(b_+(game_seasons ? S(208) : 237)); // @tutorialTextToShow
  CYC(b_+O(74), b_+OE(75)); interactionCoded0_addDoubleIndex(gb, b_+O(75));
  CYC(b_+O(75), b_+OE(76)); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+O(76), b_+OE(77)); C = A;
  CYC(b_+O(77), b_+OE(78)); B = mem_rd(gb, HL);
  CYC(b_+O(78), b_+OE(81)); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+O(81), b_+OE(83)); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(b_+O(83), showText_hook, SYM(showText), b_+OE(86)); else CYC(b_+O(83), b_+OE(86)); // call nz

deleteIfSubid2Or5:
  CYC(b_+O(86), b_+OE(88)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(88), b_+OE(89)); A = mem_rd(gb, DE);
  CYC(b_+O(89), b_+OE(91)); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+O(91), b_+OE(93)); goto delete; } // jr z
  CYC(b_+O(91), b_+OE(93));
  CYC(b_+O(93), b_+OE(95)); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+O(95), b_+OE(96)); ret_effect(gb); return; } // ret nz
  CYC(b_+O(95), b_+OE(96));

delete:
  CYC(b_+O(96), b_+OE(99)); TAIL(interactionDelete); // jp

state2:
  CYC(b_+O(99), b_+OE(102)); A = mem_rd(gb, w1Companion_enabled);
  CYC(b_+O(102), b_+OE(103)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(103), b_+OE(104)); ret_effect(gb); return; } // ret z
  CYC(b_+O(103), b_+OE(104));
  CYC(b_+O(104), b_+OE(106)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(106), b_+OE(107)); A = mem_rd(gb, DE);
  CYC(b_+O(107), b_+OE(108)); push_effect(gb, b_+OE(108));
  do { uint16_t jt_ = (interactionCoded0_jump_table(gb));
    if (!game_seasons && jt_ == b_+171) { goto setFlagAndDeleteWhenCompanionIsBelowOrRight; }
    else if (jt_ == b_+O(135)) { goto setFlagAndDeleteWhenCompanionIsAbove; }
    else if (jt_ == b_+O(120)) { goto setFlagAndDeleteWhenCompanionIsBelow; }
    else if (!game_seasons && jt_ == b_+155) { goto setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange; }
    else if (!game_seasons && jt_ == b_+161) { goto setFlagAndDeleteWhenCompanionIsLeft; }
    else if (game_seasons && jt_ == b_+O(130)) { goto setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero; }
    else if (game_seasons && jt_ == b_+O(153)) { goto goToDelete; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

setFlagAndDeleteWhenCompanionIsBelow:
  CYC(b_+O(120), b_+OE(122)); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(122), b_+OE(123)); A = mem_rd(gb, DE);
  CYC(b_+O(123), b_+OE(126)); SET_HL(w1Companion_yh);
  CYC(b_+O(126), b_+OE(127)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+O(127), b_+OE(128)); ret_effect(gb); return; } // ret nc
  CYC(b_+O(127), b_+OE(128));
  CYC(b_+O(128), b_+OE(130)); goto setFlagAndDelete; // jr $4bbf

  // Only Seasons' table reaches this; in Ages it is dead code.
setFlagAndDeleteWhenCompanionIsAboveAndVar38NonZero:
  CYC(b_+O(130), b_+OE(133)); A = mem_rd(gb, w1Companion_var38);
  CYC(b_+O(133), b_+OE(134)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(134), b_+OE(135)); ret_effect(gb); return; } // ret z
  CYC(b_+O(134), b_+OE(135));

setFlagAndDeleteWhenCompanionIsAbove:
  CALL_L(b_+O(135), interactionCoded0_cpYToCompanion, b_+OE(138));
  if (F & FC) { CYCT(b_+O(138), b_+OE(139)); ret_effect(gb); return; } // ret c
  CYC(b_+O(138), b_+OE(139));

setFlagAndDelete:
  CYC(b_+O(139), b_+OE(141)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(141), b_+OE(142)); A = mem_rd(gb, DE);
  CYC(b_+O(142), b_+OE(145)); SET_HL(b_+(game_seasons ? S(222) : 249)); // @flagNumbers
  CYC(b_+O(145), b_+OE(146)); interactionCoded0_addAToHl(gb, b_+O(146));
  CYC(b_+O(146), b_+OE(147)); A = mem_rd(gb, HL);
  CYC(b_+O(147), b_+OE(150)); SET_HL(wCompanionTutorialTextShown);
  CALL_C(b_+O(150), setFlag_hook, SYM(setFlag), b_+OE(153));

goToDelete:
  CYC(b_+O(153), b_+OE(155)); goto delete; // jr $4b94

  if (!game_seasons) {
setFlagAndDeleteWhenCompanionIsAboveAndLinkInXRange:
    CALL_L(b_+155, interactionCoded0_checkLinkInXRange, b_+158);
    if (!(F & FZ)) { CYCT(b_+158, b_+159); ret_effect(gb); return; } // ret nz
    CYC(b_+158, b_+159);
    CYC(b_+159, b_+161); goto setFlagAndDeleteWhenCompanionIsAbove; // jr $4bbb

setFlagAndDeleteWhenCompanionIsLeft:
    CYC(b_+161, b_+163); E = INTERACTION_BASE + OBJ_XH;
    CYC(b_+163, b_+164); A = mem_rd(gb, DE);
    CYC(b_+164, b_+167); SET_HL(w1Companion_xh);
    CYC(b_+167, b_+168); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FC)) { CYCT(b_+168, b_+169); ret_effect(gb); return; } // ret nc
    CYC(b_+168, b_+169);
    CYC(b_+169, b_+171); goto setFlagAndDelete; // jr $4bbf

setFlagAndDeleteWhenCompanionIsBelowOrRight:
    CALL_L(b_+171, interactionCoded0_cpYToCompanion, b_+174);
    if (F & FC) { CYCT(b_+174, b_+176); goto setFlagAndDelete; } // jr c
    CYC(b_+174, b_+176);
    CYC(b_+176, b_+178); E = INTERACTION_BASE + OBJ_XH;
    CYC(b_+178, b_+179); A = mem_rd(gb, DE);
    CYC(b_+179, b_+182); SET_HL(w1Companion_xh);
    CYC(b_+182, b_+183); alu_cp(gb, mem_rd(gb, HL));
    if (F & FC) { CYCT(b_+183, b_+184); ret_effect(gb); return; } // ret c
    CYC(b_+183, b_+184);
    CYC(b_+184, b_+186); goto setFlagAndDelete; // jr $4bbf
  }
}
