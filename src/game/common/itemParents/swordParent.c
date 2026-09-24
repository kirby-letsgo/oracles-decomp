#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t sword_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

enum sword_poke_result {
  SWORD_POKE_ABORT,
  SWORD_POKE_NORMAL,
  SWORD_POKE_TRIGGER,
};

static void sword_parent_check_and_ret_for_poke(GB *gb, uint16_t sp0_, enum sword_poke_result *result) {
  BASE(parentItemCode_sword);
  *result = SWORD_POKE_ABORT;
  CYC(b_+O(389), b_+OE(390)); alu_xor(gb, A);
  CYC(b_+O(390), b_+OE(392)); E = 0x02;
  CYC(b_+O(392), b_+OE(393)); mem_wr(gb, DE, A);
  CYC(b_+O(393), b_+OE(396)); A = W8(w1WeaponItem_var2a);
  CYC(b_+O(396), b_+OE(398)); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+O(398), b_+OE(400)); goto set_subid; }
  CYC(b_+O(398), b_+OE(400));
  CYC(b_+O(400), b_+OE(401)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(401), b_+OE(403)); goto escape; }
  CYC(b_+O(401), b_+OE(403));
  CALL_C(b_+O(403), checkLinkPushingAgainstWall_hook, SYM(checkLinkPushingAgainstWall), b_+OE(406));
  if (!(F & FC)) {
    CYCT(b_+O(406), b_+OE(407));
    ret_effect(gb);
    *result = SWORD_POKE_NORMAL;
    return;
  }
  CYC(b_+O(406), b_+OE(407));

set_subid:
  CYC(b_+O(407), b_+OE(409)); E = 0x02;
  CYC(b_+O(409), b_+OE(411)); A = 0x01;
  CYC(b_+O(411), b_+OE(412)); mem_wr(gb, DE, A);

escape:
  CYC(b_+O(412), b_+OE(413)); SET_HL(pop_effect(gb));
  CYC(b_+O(413), b_+OE(414)); alu_xor(gb, A);
  CYC(b_+O(414), b_+OE(417)); W8(w1WeaponItem_collisionType) = A;
  *result = SWORD_POKE_TRIGGER;
}

static void sword_parent_create_beam(GB *gb, uint16_t sp0_) {
  BASE(parentItemCode_sword);
  CYC(b_+O(468), b_+OE(471)); SET_BC(0x2700);
  CYC(b_+O(471), b_+OE(473)); E = 0x01;
  CALL_C(b_+O(473), getFreeItemSlotWithObjectCap_hook, SYM(getFreeItemSlotWithObjectCap), b_+OE(476));
  if (F & FC) { CYCT(b_+O(476), b_+OE(477)); ret_effect(gb); return; }
  CYC(b_+O(476), b_+OE(477));
  CYC(b_+O(477), b_+OE(478)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(478), b_+OE(479)); L = alu_inc8(gb, L);
  CYC(b_+O(479), b_+OE(480)); A = B;
  CYC(b_+O(480), b_+OE(481)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(481), b_+OE(482)); A = C;
  CYC(b_+O(482), b_+OE(483)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(483), b_+OE(484)); push_effect(gb, DE);
  CYC(b_+O(484), b_+OE(487)); SET_DE(w1Link_direction);
  CYC(b_+O(487), b_+OE(489)); L = 0x08;
  CYC(b_+O(489), b_+OE(491)); B = 0x08;
  CALL_C(b_+O(491), copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+OE(494));
  CYC(b_+O(494), b_+OE(495)); SET_DE(pop_effect(gb));
  CYC(b_+O(495), b_+OE(496)); F = (F & FZ) | FC;
  CYC(b_+O(496), b_+OE(497)); ret_effect(gb);
}

void parentItemCode_sword_hook(GB *gb) {
  BASE(parentItemCode_sword);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), clearParentItemIfCantUseSword_hook, SYM(clearParentItemIfCantUseSword), b_+OE(3));
  CYC(b_+O(3), b_+OE(5)); E = 0x04;
  CYC(b_+O(5), b_+OE(6)); A = mem_rd(gb, DE);
  CYC(b_+O(6), b_+OE(7)); push_effect(gb, b_+OE(7));
  do { uint16_t jt_ = (sword_parent_jump_table(gb));
    if (jt_ == b_+O(21)) { goto state0; }
    else if (jt_ == b_+O(73)) { goto state1; }
    else if (jt_ == b_+O(195)) { goto state2; }
    else if (jt_ == b_+O(258)) { goto state3; }
    else if (jt_ == b_+O(331)) { goto state4; }
    else if (jt_ == b_+O(354)) { goto state5; }
    else if (jt_ == b_+O(119)) { goto state6; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+O(21), b_+OE(24)); SET_HL(wcc63);
  CYC(b_+O(24), b_+OE(26)); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) CYCT(b_+O(26), b_+OE(28));
  else {
    CYC(b_+O(26), b_+OE(28));
    CYC(b_+O(28), b_+OE(30)); mem_wr(gb, HL, 0x00);
    CALL_C(b_+O(30), updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+OE(33));
    CYC(b_+O(33), b_+OE(36)); A = W8(wLinkHealth);
    CYC(b_+O(36), b_+OE(38)); alu_cp(gb, 0x05);
    if (F & FC) CYCT(b_+O(38), b_+OE(40));
    else {
      CYC(b_+O(38), b_+OE(40));
      CYC(b_+O(40), b_+OE(42)); A = 0x32;
      CALL_C(b_+O(42), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(45));
      if (!(F & FZ)) CYCT(b_+O(45), b_+OE(47));
      else {
        CYC(b_+O(45), b_+OE(47));
        CYC(b_+O(47), b_+OE(49)); E = 0x3a;
        CYC(b_+O(49), b_+OE(51)); A = 0xf8;
        CYC(b_+O(51), b_+OE(52)); mem_wr(gb, DE, A);
      }
    }
  }
  CYC(b_+O(52), b_+OE(55)); SET_HL(w1WeaponItem_enabled);
  CYC(b_+O(55), b_+OE(56)); A = mem_rd(gb, HL);
  CYC(b_+O(56), b_+OE(57)); alu_or(gb, A);
  CYC(b_+O(57), b_+OE(59)); B = 0x40;
  if (!(F & FZ)) CALL_C_CC(b_+O(59), clearMemory_hook, SYM(clearMemory), b_+OE(62));
  else CYC(b_+O(59), b_+OE(62));
  CYC(b_+O(62), b_+OE(63)); H = D;
  CYC(b_+O(63), b_+OE(65)); L = 0x00;
  CYC(b_+O(65), b_+OE(67)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+O(67), parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+OE(70));
  CYC(b_+O(70), b_+OE(73)); TAIL(itemCreateChild);

state1:
  CYC(b_+O(73), b_+OE(76)); A = W8(wcc63);
  CYC(b_+O(76), b_+OE(77)); alu_rlca(gb);
  if (F & FC) { CYCT(b_+O(77), b_+OE(80)); goto spin_start; }
  CYC(b_+O(77), b_+OE(80));
  CALL_C(b_+O(80), specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+OE(83));
  CYC(b_+O(83), b_+OE(84)); H = D;
  CYC(b_+O(84), b_+OE(86)); E = 0x21;
  CYC(b_+O(86), b_+OE(87)); A = mem_rd(gb, DE);
  CYC(b_+O(87), b_+OE(88)); alu_or(gb, A);
  if (F & FZ) CYCT(b_+O(88), b_+OE(90));
  else {
    CYC(b_+O(88), b_+OE(90));
    CYC(b_+O(90), b_+OE(92)); L = 0x3a;
    CYC(b_+O(92), b_+OE(94)); alu_bit(gb, 7, mem_rd(gb, HL));
    if (!(F & FZ)) CYCT(b_+O(94), b_+OE(96));
    else {
      CYC(b_+O(94), b_+OE(96));
      CYC(b_+O(96), b_+OE(98)); L = 0x00;
      CYC(b_+O(98), b_+OE(100)); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
    }
  }
  CYC(b_+O(100), b_+OE(101)); L = E;
  CYC(b_+O(101), b_+OE(103)); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+O(103), b_+OE(105)); goto state6; }
  CYC(b_+O(103), b_+OE(105));
  CYC(b_+O(105), b_+OE(107)); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+O(107), b_+OE(108)); ret_effect(gb); return; }
  CYC(b_+O(107), b_+OE(108));
  CYC(b_+O(108), b_+OE(110)); mem_wr(gb, HL, mem_rd(gb, HL) & 0xdf);
  CYC(b_+O(110), b_+OE(113)); A = W8(wSwordLevel);
  CYC(b_+O(113), b_+OE(115)); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+O(115), b_+OE(118)); goto check_create_beam; }
  CYC(b_+O(115), b_+OE(118));
  CYC(b_+O(118), b_+OE(119)); ret_effect(gb); return;

state6:
  CYC(b_+O(119), b_+OE(122)); A = W8(w1WeaponItem_var2a);
  CYC(b_+O(122), b_+OE(123)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(123), b_+OE(126)); goto enemy_contact; }
  CYC(b_+O(123), b_+OE(126));
  CYC(b_+O(126), b_+OE(129)); A = W8(wLinkObjectIndex);
  CYC(b_+O(129), b_+OE(130)); alu_rrca(gb);
  if (F & FC) { CYCT(b_+O(130), b_+OE(133)); goto delete_self; }
  CYC(b_+O(130), b_+OE(133));
  CALL_C(b_+O(133), parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+OE(136));
  if (F & FZ) { CYCT(b_+O(136), b_+OE(139)); goto delete_self; }
  CYC(b_+O(136), b_+OE(139));
  CYC(b_+O(139), b_+OE(141)); A = 0x01;
  CYC(b_+O(141), b_+OE(144)); W8(wcc63) = A;
  CYC(b_+O(144), b_+OE(145)); A = alu_inc8(gb, A);
  CYC(b_+O(145), b_+OE(148)); W8(w1WeaponItem_state) = A;
  CYC(b_+O(148), b_+OE(150)); A = 0x89;
  CYC(b_+O(150), b_+OE(153)); W8(w1WeaponItem_collisionType) = A;
  CYC(b_+O(153), b_+OE(155)); L = 0x04;
  CYC(b_+O(155), b_+OE(157)); mem_wr(gb, HL, 0x02);
  CYC(b_+O(157), b_+OE(158)); L = alu_inc8(gb, L);
  CYC(b_+O(158), b_+OE(159)); alu_xor(gb, A);
  CYC(b_+O(159), b_+OE(160)); mem_wr(gb, HL, A);
  CYC(b_+O(160), b_+OE(162)); L = 0x3a;
  CYC(b_+O(162), b_+OE(163)); mem_wr(gb, HL, A);
  CYC(b_+O(163), b_+OE(165)); L = 0x3f;
  CYC(b_+O(165), b_+OE(166)); mem_wr(gb, HL, A);
  CYC(b_+O(166), b_+OE(168)); L = 0x06;
  CYC(b_+O(168), b_+OE(170)); mem_wr(gb, HL, 0x28);
  CYC(b_+O(170), b_+OE(173)); TAIL(itemEnableLinkMovement);

enemy_contact:
  CYC(b_+O(173), b_+OE(175)); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+O(175), b_+OE(178)); goto delete_self; }
  CYC(b_+O(175), b_+OE(178));
  CYC(b_+O(178), b_+OE(180)); E = 0x3a;
  CYC(b_+O(180), b_+OE(181)); A = mem_rd(gb, DE);
  CYC(b_+O(181), b_+OE(182)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(182), b_+OE(185)); goto delete_self; }
  CYC(b_+O(182), b_+OE(185));
  CYC(b_+O(185), b_+OE(188)); SET_HL(w1Link_damageToApply);
  CYC(b_+O(188), b_+OE(189)); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+O(189), b_+OE(190)); mem_wr(gb, HL, A);
  CYC(b_+O(190), b_+OE(191)); alu_xor(gb, A);
  CYC(b_+O(191), b_+OE(192)); mem_wr(gb, DE, A);
  CYC(b_+O(192), b_+OE(195)); goto delete_self;

state2:
  CYC(b_+O(195), b_+OE(198)); A = W8(wLinkObjectIndex);
  CYC(b_+O(198), b_+OE(199)); alu_rrca(gb);
  if (F & FC) { CYCT(b_+O(199), b_+OE(202)); goto delete_self; }
  CYC(b_+O(199), b_+OE(202));
  CALL_C(b_+O(202), parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+OE(205));
  if (F & FZ) { CYCT(b_+O(205), b_+OE(208)); goto delete_self; }
  CYC(b_+O(205), b_+OE(208));
  CYC(b_+O(208), b_+OE(211)); push_effect(gb, b_+OE(211));
  enum sword_poke_result poke_result;
  sword_parent_check_and_ret_for_poke(gb, sp0_, &poke_result);
  if (poke_result == SWORD_POKE_ABORT) return;
  if (poke_result == SWORD_POKE_TRIGGER) goto trigger_poke;
  CYC(b_+O(211), b_+OE(213)); A = 0x16;
  CALL_C(b_+O(213), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(216));
  CYC(b_+O(216), b_+OE(218)); C = 0x01;
  if (!(F & FZ)) CYCT(b_+O(218), b_+OE(220));
  else { CYC(b_+O(218), b_+OE(220)); CYC(b_+O(220), b_+OE(222)); C = 0x04; }
  CYC(b_+O(222), b_+OE(224)); L = 0x06;
  CYC(b_+O(224), b_+OE(225)); A = mem_rd(gb, HL);
  CYC(b_+O(225), b_+OE(226)); alu_sub(gb, C);
  CYC(b_+O(226), b_+OE(227)); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+O(227), b_+OE(228)); ret_effect(gb); return; }
  CYC(b_+O(227), b_+OE(228));
  CYC(b_+O(228), b_+OE(230)); A = 0x31;
  CALL_C(b_+O(230), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(233));
  if (!(F & FZ)) { CYCT(b_+O(233), b_+OE(235)); goto charge_without_beam; }
  CYC(b_+O(233), b_+OE(235));
  CYC(b_+O(235), b_+OE(238)); push_effect(gb, b_+OE(238)); sword_parent_create_beam(gb, sp0_);
  CYC(b_+O(238), b_+OE(241)); goto trigger_poke;

charge_without_beam:
  CYC(b_+O(241), b_+OE(243)); L = 0x04;
  CYC(b_+O(243), b_+OE(244)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(244), b_+OE(246)); L = 0x00;
  CYC(b_+O(246), b_+OE(248)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+O(248), b_+OE(250)); A = 0x03;
  CYC(b_+O(250), b_+OE(253)); W8(w1WeaponItem_state) = A;
  CYC(b_+O(253), b_+OE(255)); A = 0x4f;
  CYC(b_+O(255), b_+OE(258)); TAIL(playSound_b00);

state3:
  CYC(b_+O(258), b_+OE(261)); push_effect(gb, b_+OE(261));
  sword_parent_check_and_ret_for_poke(gb, sp0_, &poke_result);
  if (poke_result == SWORD_POKE_ABORT) return;
  if (poke_result == SWORD_POKE_TRIGGER) goto trigger_poke;
  CALL_C(b_+O(261), parentItemCheckButtonPressed_hook, SYM(parentItemCheckButtonPressed), b_+OE(264));
  if (!(F & FZ)) { CYCT(b_+O(264), b_+OE(265)); ret_effect(gb); return; }
  CYC(b_+O(264), b_+OE(265));

spin_start:
  CYC(b_+O(265), b_+OE(266)); H = D;
  CYC(b_+O(266), b_+OE(268)); A = 0x02;
  CYC(b_+O(268), b_+OE(271)); W8(wcc63) = A;
  CYC(b_+O(271), b_+OE(273)); L = 0x04;
  CYC(b_+O(273), b_+OE(275)); mem_wr(gb, HL, 0x04);
  CYC(b_+O(275), b_+OE(277)); A = 0x2f;
  CALL_C(b_+O(277), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(280));
  CYC(b_+O(280), b_+OE(282)); A = 0x05;
  if (!(F & FZ)) CYCT(b_+O(282), b_+OE(284));
  else { CYC(b_+O(282), b_+OE(284)); CYC(b_+O(284), b_+OE(286)); A = 0x09; }
  CYC(b_+O(286), b_+OE(288)); L = 0x06;
  CYC(b_+O(288), b_+OE(289)); mem_wr(gb, HL, A);
  CYC(b_+O(289), b_+OE(291)); L = 0x3f;
  CYC(b_+O(291), b_+OE(293)); mem_wr(gb, HL, 0x0f);
  if (!game_seasons) {
    CALL_C(b_+293, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+296);
    CYC(b_+296, b_+298); C = 0x28;
    if (F & FZ) CYCT(b_+298, b_+300);
    else { CYC(b_+298, b_+300); CYC(b_+300, b_+302); C = 0x30; }
  }
  CYC(b_+O(302), b_+OE(305)); A = W8(w1Link_direction);
  if (!game_seasons) { CYC(b_+305, b_+306); alu_add(gb, C); }
  else { CYC(b_+S(296), b_+S(298)); alu_add(gb, 0x28); }
  CALL_C(b_+O(306), specialObjectSetAnimationWithLinkData_hook, SYM(specialObjectSetAnimationWithLinkData), b_+OE(309));
  CYC(b_+O(309), b_+OE(310)); H = D;
  CYC(b_+O(310), b_+OE(312)); L = 0x21;
  CYC(b_+O(312), b_+OE(314)); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+O(314), b_+OE(317)); SET_HL(w1WeaponItem_state);
  CYC(b_+O(317), b_+OE(319)); mem_wr(gb, HL, 0x04);
  CYC(b_+O(319), b_+OE(321)); L = 0x3a;
  CYC(b_+O(321), b_+OE(323)); mem_wr(gb, HL, alu_sla(gb, mem_rd(gb, HL)));
  CALL_C(b_+O(323), itemDisableLinkMovement_hook, SYM(itemDisableLinkMovement), b_+OE(326));
  CYC(b_+O(326), b_+OE(328)); A = 0x6b;
  CYC(b_+O(328), b_+OE(331)); TAIL(playSound_b00);

state4:
  CALL_C(b_+O(331), specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+OE(334));
  CYC(b_+O(334), b_+OE(335)); H = D;
  CYC(b_+O(335), b_+OE(337)); L = 0x21;
  CYC(b_+O(337), b_+OE(339)); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(339), b_+OE(340)); ret_effect(gb); return; }
  CYC(b_+O(339), b_+OE(340));
  CYC(b_+O(340), b_+OE(342)); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+O(342), b_+OE(344)); L = 0x06;
  CYC(b_+O(344), b_+OE(345)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+O(345), b_+OE(346)); ret_effect(gb); return; }
  CYC(b_+O(345), b_+OE(346));
  CYC(b_+O(346), b_+OE(348)); A = 0x05;
  CYC(b_+O(348), b_+OE(351)); W8(w1WeaponItem_state) = A;
  CYC(b_+O(351), b_+OE(354)); goto delete_self;

state5:
  CALL_C(b_+O(354), specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+OE(357));
  CYC(b_+O(357), b_+OE(358)); H = D;
  CYC(b_+O(358), b_+OE(360)); L = 0x21;
  CYC(b_+O(360), b_+OE(362)); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(362), b_+OE(363)); ret_effect(gb); return; }
  CYC(b_+O(362), b_+OE(363));
  CYC(b_+O(363), b_+OE(365)); L = 0x02;
  CYC(b_+O(365), b_+OE(366)); A = mem_rd(gb, HL);
  CYC(b_+O(366), b_+OE(367)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(367), b_+OE(369)); goto delete_self; }
  CYC(b_+O(367), b_+OE(369));
  CYC(b_+O(369), b_+OE(371)); A = 0x06;
  CYC(b_+O(371), b_+OE(374)); W8(w1WeaponItem_state) = A;
  CYC(b_+O(374), b_+OE(376)); L = 0x04;
  CYC(b_+O(376), b_+OE(377)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(377), b_+OE(378)); alu_xor(gb, A);
  CYC(b_+O(378), b_+OE(381)); W8(w1WeaponItem_var2a) = A;
  CYC(b_+O(381), b_+OE(382)); ret_effect(gb); return;

delete_self:
  CYC(b_+O(382), b_+OE(383)); alu_xor(gb, A);
  CYC(b_+O(383), b_+OE(386)); W8(wcc63) = A;
  CYC(b_+O(386), b_+OE(389)); TAIL(clearParentItem);

trigger_poke:
  CYC(b_+O(417), b_+OE(418)); H = D;
  CYC(b_+O(418), b_+OE(420)); L = 0x3f;
  CYC(b_+O(420), b_+OE(422)); mem_wr(gb, HL, 0x08);
  CYC(b_+O(422), b_+OE(424)); L = 0x04;
  CYC(b_+O(424), b_+OE(426)); mem_wr(gb, HL, 0x05);
  CALL_C(b_+O(426), itemDisableLinkMovement_hook, SYM(itemDisableLinkMovement), b_+OE(429));
  if (!game_seasons) {
    CALL_C(b_+429, isLinkUnderwater_hook, SYM(isLinkUnderwater), b_+432);
    CYC(b_+432, b_+434); A = 0x1f;
    if (F & FZ) CYCT(b_+434, b_+436);
    else { CYC(b_+434, b_+436); CYC(b_+436, b_+438); A = 0x2c; }
  } else {
    CYC(b_+O(436), b_+OE(438)); A = 0x1f;
  }
  CYC(b_+O(438), b_+OE(441)); TAIL(specialObjectSetAnimationWithLinkData);

check_create_beam:
  CYC(b_+O(441), b_+OE(443)); C = 0x08;
  CYC(b_+O(443), b_+OE(445)); A = 0x17;
  CALL_C(b_+O(445), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(448));
  if (F & FZ) CYCT(b_+O(448), b_+OE(450));
  else {
    CYC(b_+O(448), b_+OE(450));
    CYC(b_+O(450), b_+OE(452)); C = 0x0c;
    CYC(b_+O(452), b_+OE(454)); A = 0x18;
    CALL_C(b_+O(454), cpActiveRing_hook, SYM(cpActiveRing), b_+OE(457));
    if (F & FZ) CYCT(b_+O(457), b_+OE(459));
    else { CYC(b_+O(457), b_+OE(459)); CYC(b_+O(459), b_+OE(461)); C = 0x00; }
  }
  CYC(b_+O(461), b_+OE(464)); SET_HL(wLinkHealth);
  CYC(b_+O(464), b_+OE(465)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(465), b_+OE(466)); alu_add(gb, C);
  CYC(b_+O(466), b_+OE(467)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+O(467), b_+OE(468)); ret_effect(gb); return; }
  CYC(b_+O(467), b_+OE(468));
  sword_parent_create_beam(gb, sp0_);
}
