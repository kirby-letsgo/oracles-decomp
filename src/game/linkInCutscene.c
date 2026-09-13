#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void linkCutscene_initOam_setVisible_incState_hook(GB *gb);
void linkCutscene_updateAngleOnPath_hook(GB *gb);
void linkCutsceneFunc_73e8_hook(GB *gb);

static uint16_t link_cutscene_jump_table(GB *gb) {
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

static void link_cutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void link_cutscene_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x0012, 0x0013, false);
    burn_rom(gb, 0, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

void linkCutsceneRet_hook(GB *gb) {
  CYC(0x7250, 0x7251); ret_effect(gb);
}

void linkCutscene_animateAndDecCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x74a1, specialObjectAnimate_hook, 0x2aef, 0x74a4);
  CYC(0x74a4, 0x74a7); itemDecCounter1_hook(gb);
}

void linkCutscene_oscillateZ_hook(GB *gb) {
  CYC(0x7180, 0x7183); A = W8(wTmpcbb7);
  CYC(0x7183, 0x7185); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x7185, 0x7187);
  } else {
    CYC(0x7185, 0x7187);
    CYC(0x7187, 0x718a); A = W8(wTmpcbb7);
    CYC(0x718a, 0x718c); alu_and(gb, 0x38);
    CYC(0x718c, 0x718e); A = alu_swap(gb, A);
    CYC(0x718e, 0x718f); alu_rlca(gb);
    CYC(0x718f, 0x7190); push_effect(gb, 0x7190); link_cutscene_add_a_to_hl(gb);
    CYC(0x7190, 0x7192); E = 0x0f;
    CYC(0x7192, 0x7193); A = mem_rd(gb, HL);
    CYC(0x7193, 0x7194); B = A;
    CYC(0x7194, 0x7195); A = mem_rd(gb, DE);
    CYC(0x7195, 0x7196); alu_add(gb, B);
    CYC(0x7196, 0x7197); mem_wr(gb, DE, A);
  }
  CYC(0x7197, 0x719a); specialObjectAnimate_hook(gb);
}

void linkCutscene_oscillateZ_1_hook(GB *gb) {
  CYC(0x717d, 0x7180); SET_HL(0x71a2);
  linkCutscene_oscillateZ_hook(gb);
}

void linkCutscene_oscillateZ_2_hook(GB *gb) {
  CYC(0x771a, 0x771d); SET_HL(0x71aa);
  CYC(0x771d, 0x7720); linkCutscene_oscillateZ_hook(gb);
}

void linkCutscene_createGlowingOrb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7167, 0x716a); SET_BC(0x8406);
  CALL_C(0x716a, objectCreateInteraction_hook, 0x24c5, 0x716d);
  if (!(F & FZ)) {
    CYCT(0x716d, 0x716f);
  } else {
    CYC(0x716d, 0x716f);
    CYC(0x716f, 0x7171); L = 0x56;
    CYC(0x7171, 0x7173); A = 0;
    CYC(0x7173, 0x7174); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x7174, 0x7175); mem_wr(gb, HL, D);
  }
  CALL_C(0x7175, itemIncSubstate_hook, 0x23ef, 0x7178);
  CYC(0x7178, 0x717a); A = 0x05;
  CYC(0x717a, 0x717d); specialObjectSetAnimation_hook(gb);
}

void linkCutscene0_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71b2, 0x71b4); E = 0x21;
  CYC(0x71b4, 0x71b5); A = mem_rd(gb, DE);
  CYC(0x71b5, 0x71b6); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x71b6, 0x71b8);
  } else {
    CYC(0x71b6, 0x71b8);
    CYC(0x71b8, 0x71ba); A = 0x07;
    CYC(0x71ba, 0x71bd); W8(wTmpcbb9) = A;
    CYC(0x71bd, 0x71be); ret_effect(gb);
    return;
  }
  CALL_C(0x71be, specialObjectAnimate_hook, 0x2aef, 0x71c1);
  CYC(0x71c1, 0x71c4); A = W8(wTmpcbb7);
  CYC(0x71c4, 0x71c5); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x71c5, 0x71c8);
    objectSetInvisible_hook(gb);
    return;
  }
  CYC(0x71c5, 0x71c8);
  CYC(0x71c8, 0x71cb);
  objectSetVisible_hook(gb);
}

void linkCutscene_cpyTo48_hook(GB *gb) {
  CYC(0x7487, 0x7489); E = 0x0b;
  CYC(0x7489, 0x748a); A = mem_rd(gb, DE);
  CYC(0x748a, 0x748c); alu_cp(gb, 0x48);
  CYC(0x748c, 0x748d); ret_effect(gb);
}

void linkCutscene_cpxTo38_hook(GB *gb) {
  CYC(0x748d, 0x748f); E = 0x0d;
  CYC(0x748f, 0x7490); A = mem_rd(gb, DE);
  CYC(0x7490, 0x7492); alu_cp(gb, 0x38);
  CYC(0x7492, 0x7493); ret_effect(gb);
}

void linkCutscene_initOam_setVisible_incState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7493, 0x7496); SET_HL(0x41f7);
  CYC(0x7496, 0x7498); E = 0x05;
  CALL_C(0x7498, interBankCall_hook, 0x008a, 0x749b);
  CALL_C(0x749b, objectSetVisiblec1_hook, 0x1e3c, 0x749e);
  CYC(0x749e, 0x74a1);
  itemIncState_hook(gb);
}

void linkCutscene_updateAngleOnPath_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  bool loop_after_update = false;

start:
  CYC(0x7720, 0x7722); E = 0x03;
  CYC(0x7722, 0x7723); A = mem_rd(gb, DE);
  CYC(0x7723, 0x7726); SET_HL(0x776f);
  CYC(0x7726, 0x7727); link_cutscene_add_double_index(gb, 0x7727);
  CYC(0x7727, 0x7728); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7728, 0x7729); H = mem_rd(gb, HL);
  CYC(0x7729, 0x772a); L = A;
  CYC(0x772a, 0x772c); E = 0x3d;
  CYC(0x772c, 0x772d); A = mem_rd(gb, DE);
  CYC(0x772d, 0x772e); alu_add(gb, A);
  CYC(0x772e, 0x772f); push_effect(gb, 0x772f); link_cutscene_add_a_to_hl(gb);
  CYC(0x772f, 0x7730); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7730, 0x7732); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x7732, 0x7733); ret_effect(gb); return; }
  CYC(0x7732, 0x7733);
  CYC(0x7733, 0x7734); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7734, 0x7736);
    CYC(0x7744, 0x7746); E = 0x0d;
    CYC(0x7746, 0x7747); A = mem_rd(gb, DE);
    CYC(0x7747, 0x7748); alu_sub(gb, mem_rd(gb, HL));
    CYC(0x7748, 0x774a); B = 0x03;
    if (!(F & FC)) {
      CYCT(0x774a, 0x774c);
    } else {
      CYC(0x774a, 0x774c);
      CYC(0x774c, 0x774e); B = 0x01;
    }
    if (!(F & FZ)) {
      CYCT(0x774e, 0x7750);
      goto update_direction;
    }
    CYC(0x774e, 0x7750);
  } else {
    CYC(0x7734, 0x7736);
    CYC(0x7736, 0x7738); E = 0x0b;
    CYC(0x7738, 0x7739); A = mem_rd(gb, DE);
    CYC(0x7739, 0x773a); alu_sub(gb, mem_rd(gb, HL));
    CYC(0x773a, 0x773c); B = 0x00;
    if (!(F & FC)) {
      CYCT(0x773c, 0x773e);
    } else {
      CYC(0x773c, 0x773e);
      CYC(0x773e, 0x7740); B = 0x02;
    }
    if (!(F & FZ)) {
      CYCT(0x7740, 0x7742);
      goto update_direction;
    }
    CYC(0x7740, 0x7742);
    CYC(0x7742, 0x7744);
  }

  CYC(0x7750, 0x7751); H = D;
  CYC(0x7751, 0x7753); L = 0x3d;
  CYC(0x7753, 0x7754); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7754, 0x7757); push_effect(gb, 0x7757);
  loop_after_update = true;

update_direction:
  CYC(0x7759, 0x775b); E = 0x08;
  CYC(0x775b, 0x775c); A = mem_rd(gb, DE);
  CYC(0x775c, 0x775d); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x775d, 0x775f);
  } else {
    CYC(0x775d, 0x775f);
    CYC(0x775f, 0x7760); A = B;
    CYC(0x7760, 0x7761); mem_wr(gb, DE, A);
    CALL_C(0x7761, specialObjectSetAnimation_hook, 0x2b0a, 0x7764);
    CYC(0x7764, 0x7766); E = 0x08;
    CYC(0x7766, 0x7767); A = mem_rd(gb, DE);
    CYC(0x7767, 0x7769); A = alu_swap(gb, A);
    CYC(0x7769, 0x776a); alu_rrca(gb);
    CYC(0x776a, 0x776c); E = 0x09;
    CYC(0x776c, 0x776d); mem_wr(gb, DE, A);
  }
  CYC(0x776d, 0x776e); alu_or(gb, D);
  CYC(0x776e, 0x776f); ret_effect(gb);
  if (!loop_after_update) return;
  loop_after_update = false;
  CYC(0x7757, 0x7759);
  goto start;
}

void linkCutsceneFunc_73e8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73e8, 0x73eb); A = W8(wFrameCounter);
  CYC(0x73eb, 0x73ed); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x73ed, 0x73ee); ret_effect(gb); return; }
  CYC(0x73ed, 0x73ee);
  CYC(0x73ee, 0x73f1); SET_HL(0x7877);
  CYC(0x73f1, 0x73f3); E = 0x0a;
  CALL_C(0x73f3, interBankCall_hook, 0x008a, 0x73f6);
  CALL_C(0x73f6, objectGetRelativeAngle_hook, 0x1ea4, 0x73f9);
  CALL_C(0x73f9, convertAngleToDirection_hook, 0x26f9, 0x73fc);
  CYC(0x73fc, 0x73fd); H = D;
  CYC(0x73fd, 0x73ff); L = 0x08;
  CYC(0x73ff, 0x7400); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7400, 0x7401); ret_effect(gb); return; }
  CYC(0x7400, 0x7401);
  CYC(0x7401, 0x7402); mem_wr(gb, HL, A);
  CYC(0x7402, 0x7405);
  specialObjectSetAnimation_hook(gb);
}

static void link_cutscene_set_direction_and_animation(GB *gb, uint16_t sp0_) {
  CALL_C(0x72cc, itemIncSubstate_hook, 0x23ef, 0x72cf);
  CYC(0x72cf, 0x72d1); L = 0x09;
  CYC(0x72d1, 0x72d2); mem_wr(gb, HL, B);
  CYC(0x72d2, 0x72d3); A = B;
  CYC(0x72d3, 0x72d5); A = alu_swap(gb, A);
  CYC(0x72d5, 0x72d6); alu_rlca(gb);
  CYC(0x72d6, 0x72d9);
  specialObjectSetAnimation_hook(gb);
}

void linkCutscene3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7322, 0x7324); E = 0x04;
  CYC(0x7324, 0x7325); A = mem_rd(gb, DE);
  CYC(0x7325, 0x7326); push_effect(gb, 0x7326);
  switch (link_cutscene_jump_table(gb)) {
    case 0x732a: goto state0;
    case 0x7332: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x732a, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x732d);
  CYC(0x732d, 0x732f); A = 0x01;
  CYC(0x732f, 0x7332);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CYC(0x7332, 0x7334); E = 0x05;
  CYC(0x7334, 0x7335); A = mem_rd(gb, DE);
  CYC(0x7335, 0x7336); push_effect(gb, 0x7336);
  switch (link_cutscene_jump_table(gb)) {
    case 0x734a: goto substate0;
    case 0x7361: goto substate1;
    case 0x736f: goto substate2;
    case 0x737a: goto substate3;
    case 0x738a: goto substate4;
    case 0x73a9: goto substate5;
    case 0x73b7: goto substate6;
    case 0x73cc: goto substate7;
    case 0x73da: goto substate8;
    case 0x73b6: goto substate9;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x734a, 0x734d); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x734d, 0x734f); alu_cp(gb, 0x09);
  if (!(F & FZ)) { CYCT(0x734f, 0x7350); ret_effect(gb); return; }
  CYC(0x734f, 0x7350);
  CALL_C(0x7350, itemIncSubstate_hook, 0x23ef, 0x7353);
  CYC(0x7353, 0x7355); L = 0x0b;
  CYC(0x7355, 0x7357); A = 0x30;
  CYC(0x7357, 0x7358); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7358, 0x7359); L = alu_inc8(gb, L);
  CYC(0x7359, 0x735b); A = 0x78;
  CYC(0x735b, 0x735c); mem_wr(gb, HL, A);
  CYC(0x735c, 0x735e); A = 0x01;
  CYC(0x735e, 0x7361);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CYC(0x7361, 0x7364); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7364, 0x7366); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(0x7366, 0x7367); ret_effect(gb); return; }
  CYC(0x7366, 0x7367);
  CALL_C(0x7367, itemIncSubstate_hook, 0x23ef, 0x736a);
  CYC(0x736a, 0x736c); L = 0x06;
  CYC(0x736c, 0x736e); mem_wr(gb, HL, 0x1e);
  CYC(0x736e, 0x736f); ret_effect(gb);
  return;

substate2:
  CALL_C(0x736f, itemDecCounter1_hook, 0x23d6, 0x7372);
  if (!(F & FZ)) { CYCT(0x7372, 0x7373); ret_effect(gb); return; }
  CYC(0x7372, 0x7373);
  CALL_C(0x7373, itemIncSubstate_hook, 0x23ef, 0x7376);
  CYC(0x7376, 0x7377); alu_xor(gb, A);
  CYC(0x7377, 0x737a);
  specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CYC(0x737a, 0x737c); B = 0x0e;
  CYC(0x737c, 0x737e); C = 0x02;
  CYC(0x737e, 0x7381); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7381, 0x7382); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x7382, 0x7383); ret_effect(gb); return; }
  CYC(0x7382, 0x7383);
  CALL_C(0x7383, itemIncSubstate_hook, 0x23ef, 0x7386);
  CYC(0x7386, 0x7387); A = C;
  CYC(0x7387, 0x738a);
  specialObjectSetAnimation_hook(gb);
  return;

substate4:
  CYC(0x738a, 0x738d); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x738d, 0x738f); alu_cp(gb, 0x11);
  if (!(F & FZ)) { CYCT(0x738f, 0x7390); ret_effect(gb); return; }
  CYC(0x738f, 0x7390);
  CALL_C(0x7390, itemIncSubstate_hook, 0x23ef, 0x7393);
  CYC(0x7393, 0x7395); L = 0x09;
  CYC(0x7395, 0x7397); mem_wr(gb, HL, 0x18);
  CYC(0x7397, 0x7399); L = 0x10;
  CYC(0x7399, 0x739b); mem_wr(gb, HL, 0x3c);
  CYC(0x739b, 0x739d); L = 0x06;
  CYC(0x739d, 0x739f); mem_wr(gb, HL, 0x16);
  CYC(0x739f, 0x73a1); A = 0x75;
  CALL_C(0x73a1, playSound_b00_hook, 0x0c98, 0x73a4);
  CYC(0x73a4, 0x73a6); A = 0x03;
  CYC(0x73a6, 0x73a9);
  specialObjectSetAnimation_hook(gb);
  return;

substate5:
  CALL_C(0x73a9, linkCutscene_animateAndDecCounter1_hook, 0x74a1, 0x73ac);
  if (!(F & FZ)) {
    CYCT(0x73ac, 0x73af);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x73ac, 0x73af);
  CALL_C(0x73af, itemIncSubstate_hook, 0x23ef, 0x73b2);
  CYC(0x73b2, 0x73b4); L = 0x06;
  CYC(0x73b4, 0x73b6); mem_wr(gb, HL, 0x06);

substate9:
  CYC(0x73b6, 0x73b7); ret_effect(gb);
  return;

substate6:
  CALL_C(0x73b7, itemDecCounter1_hook, 0x23d6, 0x73ba);
  if (!(F & FZ)) { CYCT(0x73ba, 0x73bb); ret_effect(gb); return; }
  CYC(0x73ba, 0x73bb);
  CYC(0x73bb, 0x73bd); mem_wr(gb, HL, 0x08);
  CYC(0x73bd, 0x73bf); L = 0x09;
  CYC(0x73bf, 0x73c1); mem_wr(gb, HL, 0x10);
  CYC(0x73c1, 0x73c3); A = 0x02;
  CYC(0x73c3, 0x73c5); L = 0x08;
  CYC(0x73c5, 0x73c6); mem_wr(gb, HL, A);
  CALL_C(0x73c6, specialObjectSetAnimation_hook, 0x2b0a, 0x73c9);
  CYC(0x73c9, 0x73cc);
  itemIncSubstate_hook(gb);
  return;

substate7:
  CALL_C(0x73cc, linkCutscene_animateAndDecCounter1_hook, 0x74a1, 0x73cf);
  if (!(F & FZ)) {
    CYCT(0x73cf, 0x73d2);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x73cf, 0x73d2);
  CYC(0x73d2, 0x73d4); A = 0x75;
  CALL_C(0x73d4, playSound_b00_hook, 0x0c98, 0x73d7);
  CYC(0x73d7, 0x73da);
  itemIncSubstate_hook(gb);
  return;

substate8:
  CYC(0x73da, 0x73dd); A = W8(wTmpcfc0_genericCutscene_cfd2);
  CYC(0x73dd, 0x73de); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x73de, 0x73e0);
    linkCutsceneFunc_73e8_hook(gb);
    return;
  }
  CYC(0x73de, 0x73e0);
  CYC(0x73e0, 0x73e2); A = 0x03;
  CALL_C(0x73e2, specialObjectSetAnimation_hook, 0x2b0a, 0x73e5);
  CYC(0x73e5, 0x73e8);
  itemIncSubstate_hook(gb);
}

void linkCutscene4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7405, 0x7407); E = 0x04;
  CYC(0x7407, 0x7408); A = mem_rd(gb, DE);
  CYC(0x7408, 0x7409); push_effect(gb, 0x7409);
  switch (link_cutscene_jump_table(gb)) {
    case 0x740d: goto state0;
    case 0x741d: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x740d, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x7410);
  CYC(0x7410, 0x7412); L = 0x0b;
  CYC(0x7412, 0x7414); A = 0x38;
  CYC(0x7414, 0x7415); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7415, 0x7416); L = alu_inc8(gb, L);
  CYC(0x7416, 0x7418); A = 0x58;
  CYC(0x7418, 0x7419); mem_wr(gb, HL, A);
  CYC(0x7419, 0x741a); alu_xor(gb, A);
  CYC(0x741a, 0x741d);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CYC(0x741d, 0x741f); E = 0x05;
  CYC(0x741f, 0x7420); A = mem_rd(gb, DE);
  CYC(0x7420, 0x7421); push_effect(gb, 0x7421);
  switch (link_cutscene_jump_table(gb)) {
    case 0x742d: goto substate0;
    case 0x7436: goto substate1;
    case 0x7446: goto substate2;
    case 0x7455: goto substate3;
    case 0x7460: goto substate4;
    case 0x746b: goto substate5;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x742d, 0x7430); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7430, 0x7432); alu_cp(gb, 0x1f);
  if (!(F & FZ)) { CYCT(0x7432, 0x7433); ret_effect(gb); return; }
  CYC(0x7432, 0x7433);
  CYC(0x7433, 0x7436);
  itemIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x7436, 0x7439); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7439, 0x743b); alu_cp(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x743b, 0x743e);
    linkCutsceneFunc_73e8_hook(gb);
    return;
  }
  CYC(0x743b, 0x743e);
  CALL_C(0x743e, itemIncSubstate_hook, 0x23ef, 0x7441);
  CYC(0x7441, 0x7443); L = 0x06;
  CYC(0x7443, 0x7445); mem_wr(gb, HL, 0x50);
  CYC(0x7445, 0x7446); ret_effect(gb);
  return;

substate2:
  CALL_C(0x7446, itemDecCounter1_hook, 0x23d6, 0x7449);
  if (!(F & FZ)) { CYCT(0x7449, 0x744a); ret_effect(gb); return; }
  CYC(0x7449, 0x744a);
  CYC(0x744a, 0x744c); mem_wr(gb, HL, 0x30);
  CYC(0x744c, 0x744e); L = 0x10;
  CYC(0x744e, 0x7450); mem_wr(gb, HL, 0x28);
  CYC(0x7450, 0x7452); B = 0x10;
  CYC(0x7452, 0x7455);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  return;

substate3:
  CALL_C(0x7455, linkCutscene_animateAndDecCounter1_hook, 0x74a1, 0x7458);
  if (!(F & FZ)) {
    CYCT(0x7458, 0x745b);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x7458, 0x745b);
  CYC(0x745b, 0x745d); mem_wr(gb, HL, 0x08);
  CYC(0x745d, 0x7460);
  itemIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(0x7460, itemDecCounter1_hook, 0x23d6, 0x7463);
  if (!(F & FZ)) { CYCT(0x7463, 0x7464); ret_effect(gb); return; }
  CYC(0x7463, 0x7464);
  CYC(0x7464, 0x7466); mem_wr(gb, HL, 0x10);
  CYC(0x7466, 0x7468); B = 0x18;
  CYC(0x7468, 0x746b);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  return;

substate5:
  CALL_C(0x746b, linkCutscene_animateAndDecCounter1_hook, 0x74a1, 0x746e);
  if (!(F & FZ)) {
    CYCT(0x746e, 0x7471);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x746e, 0x7471);
  CYC(0x7471, 0x7473); A = 0x21;
  CYC(0x7473, 0x7476); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(0x7476, 0x7478); A = 0x81;
  CYC(0x7478, 0x747b); W8(wMenuDisabled) = A;
  CYC(0x747b, 0x747e); W8(wDisabledObjects) = A;
  CYC(0x747e, 0x7480); E = 0x08;
  CYC(0x7480, 0x7482); A = 0x03;
  CYC(0x7482, 0x7483); mem_wr(gb, DE, A);
  CYC(0x7483, 0x7484); alu_xor(gb, A);
  CYC(0x7484, 0x7487);
  setLinkIDOverride_hook(gb);
}
