#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

#define linkCutscene0OscillationData_bank06 0x719a

void linkCutscene_initOam_setVisible_incState_hook(GB *gb);
void linkCutscene_updateAngleOnPath_hook(GB *gb);
void linkCutsceneFunc_73e8_hook(GB *gb);
void linkCutscene0_hook(GB *gb);

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

void linkCutscene0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70be, 0x70c0); E = 0x04;
  CYC(0x70c0, 0x70c1); A = mem_rd(gb, DE);
  CYC(0x70c1, 0x70c2); push_effect(gb, 0x70c2);
  switch (link_cutscene_jump_table(gb)) {
    case 0x70c6: break;
    case 0x70d0: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

  CALL_C(0x70c6, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x70c9);
  CALL_C(0x70c9, objectSetVisible81_hook, 0x1e60, 0x70cc);
  CYC(0x70cc, 0x70cd); alu_xor(gb, A);
  CALL_C(0x70cd, specialObjectSetAnimation_hook, 0x2b0a, 0x70d0);

state1:
  CYC(0x70d0, 0x70d2); E = 0x05;
  CYC(0x70d2, 0x70d3); A = mem_rd(gb, DE);
  CYC(0x70d3, 0x70d4); push_effect(gb, 0x70d4);
  switch (link_cutscene_jump_table(gb)) {
    case 0x70e2: goto substate0;
    case 0x710e: goto substate1;
    case 0x7125: goto substate2;
    case 0x713b: goto substate3;
    case 0x7152: goto substate4;
    case 0x7160: goto substate5;
    case 0x71b2: linkCutscene0_substate6_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x70e2, 0x70e5); A = W8(wLinkAngle);
  CYC(0x70e5, 0x70e6); alu_rlca(gb);
  CYC(0x70e6, 0x70e8); A = 0x00;
  if (F & FC) {
    CYCT(0x70e8, 0x70eb);
    specialObjectSetAnimation_hook(gb);
    return;
  }
  CYC(0x70e8, 0x70eb);
  CYC(0x70eb, 0x70ec); H = D;
  CYC(0x70ec, 0x70ee); L = 0x0b;
  CYC(0x70ee, 0x70f1); A = W8(wGameKeysPressed);
  CYC(0x70f1, 0x70f3); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYC(0x70f3, 0x70f5);
    CYC(0x70f5, 0x70f6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  } else {
    CYCT(0x70f3, 0x70f5);
  }
  CYC(0x70f6, 0x70f8); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYC(0x70f8, 0x70fa);
    CYC(0x70fa, 0x70fb); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  } else {
    CYCT(0x70f8, 0x70fa);
  }
  CYC(0x70fb, 0x70fc); A = mem_rd(gb, HL);
  CYC(0x70fc, 0x70fe); alu_cp(gb, 0x40);
  if (!(F & FC)) {
    CYCT(0x70fe, 0x7101);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x70fe, 0x7101);
  CYC(0x7101, 0x7103); A = 0x01;
  CYC(0x7103, 0x7106); W8(wTmpcbb9) = A;
  CYC(0x7106, 0x7108); A = 0x77;
  CALL_C(0x7108, playSound_b00_hook, 0x0c98, 0x710b);
  CYC(0x710b, 0x710e);
  itemIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x710e, 0x7111); A = W8(wTmpcbb9);
  CYC(0x7111, 0x7113); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x7113, 0x7114); ret_effect(gb); return; }
  CYC(0x7113, 0x7114);
  CALL_C(0x7114, itemIncSubstate_hook, 0x23ef, 0x7117);
  CYC(0x7117, 0x7119); B = 0x04;
  CALL_C(0x7119, func_2d48_hook, 0x2d48, 0x711c);
  CYC(0x711c, 0x711d); A = B;
  CYC(0x711d, 0x711f); E = 0x06;
  CYC(0x711f, 0x7120); mem_wr(gb, DE, A);
  CYC(0x7120, 0x7122); A = 0x04;
  CYC(0x7122, 0x7125);
  specialObjectSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(0x7125, itemDecCounter1_hook, 0x23d6, 0x7128);
  if (!(F & FZ)) {
    CYCT(0x7128, 0x712b);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x7128, 0x712b);
  CYC(0x712b, 0x712d); L = 0x10;
  CYC(0x712d, 0x712f); mem_wr(gb, HL, 0x05);
  CYC(0x712f, 0x7131); B = 0x05;
  CALL_C(0x7131, func_2d48_hook, 0x2d48, 0x7134);
  CYC(0x7134, 0x7135); A = B;
  CYC(0x7135, 0x7137); E = 0x06;
  CYC(0x7137, 0x7138); mem_wr(gb, DE, A);
  CYC(0x7138, 0x713b);
  itemIncSubstate_hook(gb);
  return;

substate3:
  CALL_C(0x713b, itemDecCounter1_hook, 0x23d6, 0x713e);
  if (!(F & FZ)) {
    CYCT(0x713e, 0x7141);
    goto oscillate;
  }
  CYC(0x713e, 0x7141);
  CALL_C(0x7141, itemIncSubstate_hook, 0x23ef, 0x7144);
  CYC(0x7144, 0x7146); B = 0x07;
  CALL_C(0x7146, func_2d48_hook, 0x2d48, 0x7149);
  CYC(0x7149, 0x714a); A = B;
  CYC(0x714a, 0x714c); E = 0x06;
  CYC(0x714c, 0x714d); mem_wr(gb, DE, A);
oscillate:
  CYC(0x714d, 0x7150); SET_HL(linkCutscene0OscillationData_bank06);
  CYC(0x7150, 0x7152);
  linkCutscene_oscillateZ_hook(gb);
  return;

substate4:
  CALL_C(0x7152, itemDecCounter1_hook, 0x23d6, 0x7155);
  if (!(F & FZ)) {
    CYCT(0x7155, 0x7158);
    linkCutscene_oscillateZ_1_hook(gb);
    return;
  }
  CYC(0x7155, 0x7158);
  CYC(0x7158, 0x715a); A = 0x03;
  CYC(0x715a, 0x715d); W8(wTmpcbb9) = A;
  CALL_C(0x715d, itemIncSubstate_hook, 0x23ef, 0x7160);

substate5:
  CYC(0x7160, 0x7163); A = W8(wTmpcbb9);
  CYC(0x7163, 0x7165); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x7165, 0x7167);
    linkCutscene_oscillateZ_1_hook(gb);
    return;
  }
  CYC(0x7165, 0x7167);
  linkCutscene_createGlowingOrb_hook(gb);
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

void linkCutscene1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71cb, 0x71cd); E = 0x04;
  CYC(0x71cd, 0x71ce); A = mem_rd(gb, DE);
  CYC(0x71ce, 0x71cf); push_effect(gb, 0x71cf);
  switch (link_cutscene_jump_table(gb)) {
    case 0x71d3: goto state0;
    case 0x71e2: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x71d3, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x71d6);
  CYC(0x71d6, 0x71d8); E = 0x06;
  CYC(0x71d8, 0x71da); A = 0x78;
  CYC(0x71da, 0x71db); mem_wr(gb, DE, A);
  CYC(0x71db, 0x71dc); alu_xor(gb, A);
  CYC(0x71dc, 0x71de); E = 0x08;
  CYC(0x71de, 0x71df); mem_wr(gb, DE, A);
  CALL_C(0x71df, specialObjectSetAnimation_hook, 0x2b0a, 0x71e2);

state1:
  CYC(0x71e2, 0x71e4); E = 0x05;
  CYC(0x71e4, 0x71e5); A = mem_rd(gb, DE);
  CYC(0x71e5, 0x71e6); push_effect(gb, 0x71e6);
  switch (link_cutscene_jump_table(gb)) {
    case 0x71f0: goto substate0;
    case 0x7213: goto substate1;
    case 0x7228: goto substate2;
    case 0x7239: goto substate3;
    case 0x7250: linkCutsceneRet_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x71f0, itemDecCounter1_hook, 0x23d6, 0x71f3);
  if (!(F & FZ)) { CYCT(0x71f3, 0x71f4); ret_effect(gb); return; }
  CYC(0x71f3, 0x71f4);
  CALL_C(0x71f4, itemIncSubstate_hook, 0x23ef, 0x71f7);
  CYC(0x71f7, 0x71f9); L = 0x10;
  CYC(0x71f9, 0x71fb); mem_wr(gb, HL, 0x28);
  CYC(0x71fb, 0x71fd); L = 0x0d;
  CYC(0x71fd, 0x71fe); A = mem_rd(gb, HL);
  CYC(0x71fe, 0x7200); alu_cp(gb, 0x48);
  CYC(0x7200, 0x7202); A = 0x00;
  if (F & FZ) {
    CYCT(0x7202, 0x7204);
  } else {
    CYC(0x7202, 0x7204);
    CYC(0x7204, 0x7206); A = 0x18;
    if (!(F & FC)) {
      CYCT(0x7206, 0x7208);
    } else {
      CYC(0x7206, 0x7208);
      CYC(0x7208, 0x720a); A = 0x08;
    }
  }
  CYC(0x720a, 0x720c); L = 0x09;
  CYC(0x720c, 0x720d); mem_wr(gb, HL, A);
  CYC(0x720d, 0x720f); A = alu_swap(gb, A);
  CYC(0x720f, 0x7210); alu_rlca(gb);
  CYC(0x7210, 0x7213);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CYC(0x7213, 0x7215); E = 0x0d;
  CYC(0x7215, 0x7216); A = mem_rd(gb, DE);
  CYC(0x7216, 0x7218); alu_cp(gb, 0x48);
  if (!(F & FZ)) {
    CYCT(0x7218, 0x721a);
    CALL_C(0x7222, objectApplySpeed_hook, 0x201d, 0x7225);
    CYC(0x7225, 0x7228);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x7218, 0x721a);
  CALL_C(0x721a, itemIncSubstate_hook, 0x23ef, 0x721d);
  CYC(0x721d, 0x721f); L = 0x06;
  CYC(0x721f, 0x7221); mem_wr(gb, HL, 0x04);
  CYC(0x7221, 0x7222); ret_effect(gb);
  return;

substate2:
  CALL_C(0x7228, itemDecCounter1_hook, 0x23d6, 0x722b);
  if (!(F & FZ)) { CYCT(0x722b, 0x722c); ret_effect(gb); return; }
  CYC(0x722b, 0x722c);
  CYC(0x722c, 0x722e); mem_wr(gb, HL, 0x2e);
  CALL_C(0x722e, itemIncSubstate_hook, 0x23ef, 0x7231);
  CYC(0x7231, 0x7233); L = 0x09;
  CYC(0x7233, 0x7235); mem_wr(gb, HL, 0x00);
  CYC(0x7235, 0x7236); alu_xor(gb, A);
  CYC(0x7236, 0x7239);
  specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CALL_C(0x7239, specialObjectAnimate_hook, 0x2aef, 0x723c);
  CALL_C(0x723c, objectApplySpeed_hook, 0x201d, 0x723f);
  CALL_C(0x723f, itemDecCounter1_hook, 0x23d6, 0x7242);
  if (!(F & FZ)) { CYCT(0x7242, 0x7243); ret_effect(gb); return; }
  CYC(0x7242, 0x7243);
  CYC(0x7243, 0x7246); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7246, 0x7248); mem_wr(gb, HL, 0x01);
  CYC(0x7248, 0x724a); A = 0x50;
  CALL_C(0x724a, playSound_b00_hook, 0x0c98, 0x724d);
  CYC(0x724d, 0x7250);
  itemIncSubstate_hook(gb);
}

void linkCutscene2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7251, 0x7253); E = 0x04;
  CYC(0x7253, 0x7254); A = mem_rd(gb, DE);
  CYC(0x7254, 0x7255); push_effect(gb, 0x7255);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7259: goto state0;
    case 0x726b: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x7259, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x725c);
  CYC(0x725c, 0x725f); SET_BC(0x3838);
  CALL_C(0x725f, objectGetRelativeAngle_hook, 0x1ea4, 0x7262);
  CYC(0x7262, 0x7264); E = 0x09;
  CYC(0x7264, 0x7265); mem_wr(gb, DE, A);
  CALL_C(0x7265, convertAngleDeToDirection_hook, 0x26f8, 0x7268);
  CYC(0x7268, 0x726b);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CYC(0x726b, 0x726d); E = 0x05;
  CYC(0x726d, 0x726e); A = mem_rd(gb, DE);
  CYC(0x726e, 0x726f); push_effect(gb, 0x726f);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7281: goto substate0;
    case 0x72b5: goto substate1;
    case 0x72c6: goto substate2;
    case 0x72d9: goto substate3;
    case 0x72ef: goto substate4;
    case 0x7300: goto substate5;
    case 0x7305: goto substate6;
    case 0x7310: goto substate7;
    case 0x7321: goto substate8;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7281, 0x7284); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7284, 0x7286); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x7286, 0x7287); ret_effect(gb); return; }
  CYC(0x7286, 0x7287);
  CALL_C(0x7287, itemIncSubstate_hook, 0x23ef, 0x728a);
  CYC(0x728a, 0x728c); L = 0x0b;
  CYC(0x728c, 0x728d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x728d, 0x728f); alu_cp(gb, 0x48);
  CYC(0x728f, 0x7291); A = 0x18;
  CYC(0x7291, 0x7293); B = 0x04;
  if (F & FZ) {
    CYCT(0x7293, 0x7295);
  } else {
    CYC(0x7293, 0x7295);
    CYC(0x7295, 0x7297); A = 0x10;
    if (F & FC) {
      CYCT(0x7297, 0x7299);
    } else {
      CYC(0x7297, 0x7299);
      CYC(0x7299, 0x729a); L = alu_inc8(gb, L);
      CYC(0x729a, 0x729c); B = 0x01;
      CYC(0x729c, 0x729d); A = mem_rd(gb, HL);
      CYC(0x729d, 0x729f); alu_cp(gb, 0x38);
      CYC(0x729f, 0x72a1); A = 0x00;
      if (F & FZ) {
        CYCT(0x72a1, 0x72a3);
      } else {
        CYC(0x72a1, 0x72a3);
        CYC(0x72a3, 0x72a5); A = 0x18;
        if (!(F & FC)) {
          CYCT(0x72a5, 0x72a7);
        } else {
          CYC(0x72a5, 0x72a7);
          CYC(0x72a7, 0x72a9); A = 0x08;
        }
      }
    }
  }
  CYC(0x72a9, 0x72ab); L = 0x05;
  CYC(0x72ab, 0x72ac); mem_wr(gb, HL, B);
  CYC(0x72ac, 0x72ae); L = 0x09;
  CYC(0x72ae, 0x72af); mem_wr(gb, HL, A);
  CYC(0x72af, 0x72b1); A = alu_swap(gb, A);
  CYC(0x72b1, 0x72b2); alu_rlca(gb);
  CYC(0x72b2, 0x72b5);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x72b5, specialObjectAnimate_hook, 0x2aef, 0x72b8);
  CALL_C(0x72b8, linkCutscene_cpxTo38_hook, 0x748d, 0x72bb);
  if (!(F & FZ)) {
    CYCT(0x72bb, 0x72be);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x72bb, 0x72be);
  CALL_C(0x72be, itemIncSubstate_hook, 0x23ef, 0x72c1);
  CYC(0x72c1, 0x72c3); L = 0x06;
  CYC(0x72c3, 0x72c5); mem_wr(gb, HL, 0x08);
  CYC(0x72c5, 0x72c6); ret_effect(gb);
  return;

substate2:
  CYC(0x72c6, 0x72c8); B = 0x00;
  goto label_72c8;

substate3:
  CALL_C(0x72d9, specialObjectAnimate_hook, 0x2aef, 0x72dc);
  CALL_C(0x72dc, linkCutscene_cpyTo48_hook, 0x7487, 0x72df);
  if (!(F & FZ)) {
    CYCT(0x72df, 0x72e2);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x72df, 0x72e2);
  goto goto_state7;

substate4:
  CALL_C(0x72ef, specialObjectAnimate_hook, 0x2aef, 0x72f2);
  CALL_C(0x72f2, linkCutscene_cpyTo48_hook, 0x7487, 0x72f5);
  if (!(F & FZ)) {
    CYCT(0x72f5, 0x72f8);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x72f5, 0x72f8);
  CALL_C(0x72f8, itemIncSubstate_hook, 0x23ef, 0x72fb);
  CYC(0x72fb, 0x72fd); L = 0x06;
  CYC(0x72fd, 0x72ff); mem_wr(gb, HL, 0x08);
  CYC(0x72ff, 0x7300); ret_effect(gb);
  return;

substate5:
  CYC(0x7300, 0x7302); B = 0x18;
  CYC(0x7302, 0x7305);
  goto label_72c8;

substate6:
  CALL_C(0x7305, specialObjectAnimate_hook, 0x2aef, 0x7308);
  CALL_C(0x7308, linkCutscene_cpxTo38_hook, 0x748d, 0x730b);
  if (!(F & FZ)) {
    CYCT(0x730b, 0x730e);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x730b, 0x730e);
  CYC(0x730e, 0x7310);
  goto goto_state7;

substate7:
  CALL_C(0x7310, itemDecCounter1_hook, 0x23d6, 0x7313);
  if (!(F & FZ)) { CYCT(0x7313, 0x7314); ret_effect(gb); return; }
  CYC(0x7313, 0x7314);
  CYC(0x7314, 0x7316); mem_wr(gb, HL, 0x10);
  CYC(0x7316, 0x7318); B = 0x00;
  CYC(0x7318, 0x731b); push_effect(gb, 0x731b);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  CYC(0x731b, 0x731e); SET_HL(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x731e, 0x7320); mem_wr(gb, HL, 0x03);
  CYC(0x7320, 0x7321); ret_effect(gb);
  return;

substate8:
  CYC(0x7321, 0x7322); ret_effect(gb);
  return;

label_72c8:
  CALL_C(0x72c8, itemDecCounter1_hook, 0x23d6, 0x72cb);
  if (!(F & FZ)) { CYCT(0x72cb, 0x72cc); ret_effect(gb); return; }
  CYC(0x72cb, 0x72cc);
  link_cutscene_set_direction_and_animation(gb, sp0_);
  return;

goto_state7:
  CYC(0x72e2, 0x72e3); H = D;
  CYC(0x72e3, 0x72e5); L = 0x05;
  CYC(0x72e5, 0x72e7); mem_wr(gb, HL, 0x07);
  CYC(0x72e7, 0x72e9); L = 0x06;
  CYC(0x72e9, 0x72eb); mem_wr(gb, HL, 0x3c);
  CYC(0x72eb, 0x72ec); alu_xor(gb, A);
  CYC(0x72ec, 0x72ef);
  specialObjectSetAnimation_hook(gb);
}

void linkCutscene5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74a7, 0x74a9); E = 0x04;
  CYC(0x74a9, 0x74aa); A = mem_rd(gb, DE);
  CYC(0x74aa, 0x74ab); push_effect(gb, 0x74ab);
  switch (link_cutscene_jump_table(gb)) {
    case 0x74af: goto state0;
    case 0x74be: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x74af, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x74b2);
  CYC(0x74b2, 0x74b4); L = 0x10;
  CYC(0x74b4, 0x74b6); mem_wr(gb, HL, 0x28);
  CYC(0x74b6, 0x74b8); L = 0x3d;
  CYC(0x74b8, 0x74ba); mem_wr(gb, HL, 0x00);
  CYC(0x74ba, 0x74bc); L = 0x08;
  CYC(0x74bc, 0x74be); mem_wr(gb, HL, 0xff);

state1:
  CALL_C(0x74be, linkCutscene_updateAngleOnPath_hook, 0x7720, 0x74c1);
  if (F & FZ) {
    CYCT(0x74c1, 0x74c3);
  } else {
    CYC(0x74c1, 0x74c3);
    CALL_C(0x74c3, specialObjectAnimate_hook, 0x2aef, 0x74c6);
    CYC(0x74c6, 0x74c9);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x74c9, 0x74cb); A = 0x00;
  CYC(0x74cb, 0x74ce);
  setLinkIDOverride_hook(gb);
}

void linkCutscene6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74ce, 0x74d0); E = 0x04;
  CYC(0x74d0, 0x74d1); A = mem_rd(gb, DE);
  CYC(0x74d1, 0x74d2); push_effect(gb, 0x74d2);
  switch (link_cutscene_jump_table(gb)) {
    case 0x74d6: goto state0;
    case 0x74ec: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x74d6, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x74d9);
  CYC(0x74d9, 0x74db); L = 0x10;
  CYC(0x74db, 0x74dd); mem_wr(gb, HL, 0x14);
  CYC(0x74dd, 0x74df); B = 0x16;
  CYC(0x74df, 0x74e1); L = 0x09;
  CYC(0x74e1, 0x74e2); A = mem_rd(gb, HL);
  CYC(0x74e2, 0x74e4); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(0x74e4, 0x74e6);
  } else {
    CYC(0x74e4, 0x74e6);
    CYC(0x74e6, 0x74e8); B = 0x15;
  }
  CYC(0x74e8, 0x74e9); A = B;
  CALL_C(0x74e9, specialObjectSetAnimation_hook, 0x2b0a, 0x74ec);

state1:
  CYC(0x74ec, 0x74ee); E = 0x05;
  CYC(0x74ee, 0x74ef); A = mem_rd(gb, DE);
  CYC(0x74ef, 0x74f0); push_effect(gb, 0x74f0);
  switch (link_cutscene_jump_table(gb)) {
    case 0x74f6: goto substate0;
    case 0x7504: goto substate1;
    case 0x7512: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x74f6, specialObjectAnimate_hook, 0x2aef, 0x74f9);
  CALL_C(0x74f9, getThisRoomFlags_hook, 0x197d, 0x74fc);
  CYC(0x74fc, 0x74fe); alu_and(gb, 0xc0);
  if (F & FZ) {
    CYCT(0x74fe, 0x7501);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x74fe, 0x7501);
  CYC(0x7501, 0x7504);
  itemIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x7504, 0x7507); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7507, 0x7509); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7509, 0x750a); ret_effect(gb); return; }
  CYC(0x7509, 0x750a);
  CALL_C(0x750a, itemIncSubstate_hook, 0x23ef, 0x750d);
  CYC(0x750d, 0x750f); A = 0x02;
  CYC(0x750f, 0x7512);
  specialObjectSetAnimation_hook(gb);
  return;

substate2:
  CYC(0x7512, 0x7513); ret_effect(gb);
}

void linkCutscene7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7513, 0x7515); E = 0x04;
  CYC(0x7515, 0x7516); A = mem_rd(gb, DE);
  CYC(0x7516, 0x7517); push_effect(gb, 0x7517);
  switch (link_cutscene_jump_table(gb)) {
    case 0x751b: goto state0;
    case 0x7527: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x751b, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x751e);
  CYC(0x751e, 0x7520); L = 0x06;
  CYC(0x7520, 0x7522); mem_wr(gb, HL, 0xf0);
  CYC(0x7522, 0x7524); A = 0x14;
  CYC(0x7524, 0x7527);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CALL_C(0x7527, specialObjectAnimate_hook, 0x2aef, 0x752a);
  CALL_C(0x752a, itemDecCounter1_hook, 0x23d6, 0x752d);
  if (!(F & FZ)) { CYCT(0x752d, 0x752e); ret_effect(gb); return; }
  CYC(0x752d, 0x752e);
  CYC(0x752e, 0x752f); alu_xor(gb, A);
  CALL_C(0x752f, setLinkIDOverride_hook, 0x2acf, 0x7532);
  CYC(0x7532, 0x7534); L = 0x08;
  CYC(0x7534, 0x7536); mem_wr(gb, HL, 0x02);
  CYC(0x7536, 0x7538); A = 0x01;
  CYC(0x7538, 0x753b); W8(wUseSimulatedInput) = A;
  CYC(0x753b, 0x753e); W8(wMenuDisabled) = A;
  CYC(0x753e, 0x753f); ret_effect(gb);
}

void linkCutscene8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x753f, 0x7541); E = 0x04;
  CYC(0x7541, 0x7542); A = mem_rd(gb, DE);
  CYC(0x7542, 0x7543); push_effect(gb, 0x7543);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7547: goto state0;
    case 0x755a: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x7547, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x754a);
  CYC(0x754a, 0x754c); L = 0x0b;
  CYC(0x754c, 0x754e); mem_wr(gb, HL, 0x68);
  CYC(0x754e, 0x7550); L = 0x0d;
  CYC(0x7550, 0x7552); mem_wr(gb, HL, 0x50);
  CYC(0x7552, 0x7554); A = 0x00;
  CALL_C(0x7554, specialObjectSetAnimation_hook, 0x2b0a, 0x7557);
  CYC(0x7557, 0x755a);
  objectSetInvisible_hook(gb);
  return;

state1:
  CYC(0x755a, 0x755c); E = 0x05;
  CYC(0x755c, 0x755d); A = mem_rd(gb, DE);
  CYC(0x755d, 0x755e); push_effect(gb, 0x755e);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7562: goto substate0;
    case 0x7575: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7562, 0x7565); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7565, 0x7567); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x7567, 0x7569);
  } else {
    CYC(0x7567, 0x7569);
    CYC(0x7569, 0x756c); A = W8(wTmpcfc0_genericCutscene_cfd0);
    CYC(0x756c, 0x756e); alu_cp(gb, 0x01);
    if (!(F & FZ)) { CYCT(0x756e, 0x756f); ret_effect(gb); return; }
    CYC(0x756e, 0x756f);
  }
  CALL_C(0x756f, itemIncSubstate_hook, 0x23ef, 0x7572);
  CYC(0x7572, 0x7575);
  objectSetVisiblec2_hook(gb);
  return;

substate1:
  CYC(0x7575, 0x7576); ret_effect(gb);
}

void linkCutscene9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7576, 0x7578); E = 0x04;
  CYC(0x7578, 0x7579); A = mem_rd(gb, DE);
  CYC(0x7579, 0x757a); push_effect(gb, 0x757a);
  switch (link_cutscene_jump_table(gb)) {
    case 0x757e: goto state0;
    case 0x7589: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x757e, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x7581);
  CYC(0x7581, 0x7583); A = 0x02;
  CALL_C(0x7583, specialObjectSetAnimation_hook, 0x2b0a, 0x7586);
  CYC(0x7586, 0x7589);
  objectSetInvisible_hook(gb);
  return;

state1:
  CYC(0x7589, 0x758b); E = 0x05;
  CYC(0x758b, 0x758c); A = mem_rd(gb, DE);
  CYC(0x758c, 0x758d); push_effect(gb, 0x758d);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7597: goto substate0;
    case 0x75a3: goto substate1;
    case 0x75ac: goto substate2;
    case 0x75c2: goto substate3;
    case 0x75d4: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7597, 0x759a); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x759a, 0x759c); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x759c, 0x759d); ret_effect(gb); return; }
  CYC(0x759c, 0x759d);
  CALL_C(0x759d, itemIncSubstate_hook, 0x23ef, 0x75a0);
  CYC(0x75a0, 0x75a3);
  objectSetVisible82_hook(gb);
  return;

substate1:
  CYC(0x75a3, 0x75a6); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x75a6, 0x75a8); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x75a8, 0x75a9); ret_effect(gb); return; }
  CYC(0x75a8, 0x75a9);
  CALL_C(0x75a9, itemIncSubstate_hook, 0x23ef, 0x75ac);

substate2:
  CYC(0x75ac, 0x75af); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x75af, 0x75b1); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x75b1, 0x75b4);
    linkCutsceneFunc_73e8_hook(gb);
    return;
  }
  CYC(0x75b1, 0x75b4);
  CALL_C(0x75b4, itemIncSubstate_hook, 0x23ef, 0x75b7);
  CYC(0x75b7, 0x75ba); SET_BC(0xfe40);
  CALL_C(0x75ba, objectSetSpeedZ_hook, 0x239d, 0x75bd);
  CYC(0x75bd, 0x75bf); A = 0x0d;
  CYC(0x75bf, 0x75c2);
  specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CYC(0x75c2, 0x75c4); C = 0x20;
  CALL_C(0x75c4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x75c7);
  if (!(F & FZ)) { CYCT(0x75c7, 0x75c8); ret_effect(gb); return; }
  CYC(0x75c7, 0x75c8);
  CALL_C(0x75c8, itemIncSubstate_hook, 0x23ef, 0x75cb);
  CYC(0x75cb, 0x75cd); L = 0x06;
  CYC(0x75cd, 0x75cf); mem_wr(gb, HL, 0x78);
  CYC(0x75cf, 0x75d1); L = 0x20;
  CYC(0x75d1, 0x75d3); mem_wr(gb, HL, 0x01);
  CYC(0x75d3, 0x75d4); ret_effect(gb);
  return;

substate4:
  CALL_C(0x75d4, itemDecCounter1_hook, 0x23d6, 0x75d7);
  if (!(F & FZ)) {
    CYCT(0x75d7, 0x75da);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x75d7, 0x75da);
  CYC(0x75da, 0x75dd); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x75dd, 0x75df); mem_wr(gb, HL, 0xff);
  CYC(0x75df, 0x75e0); ret_effect(gb);
}

static void link_cutscene_check_shield_equipped(GB *gb) {
  CYC(0x75fa, 0x75fd); SET_HL(wInventoryB);
  CYC(0x75fd, 0x75ff); A = 0x01;
  CYC(0x75ff, 0x7600); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7600, 0x7601); ret_effect(gb); return; }
  CYC(0x7600, 0x7601);
  CYC(0x7601, 0x7602); L = alu_inc8(gb, L);
  CYC(0x7602, 0x7603); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x7603, 0x7604); ret_effect(gb);
}

void linkCutsceneA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75e0, 0x75e2); E = 0x04;
  CYC(0x75e2, 0x75e3); A = mem_rd(gb, DE);
  CYC(0x75e3, 0x75e4); push_effect(gb, 0x75e4);
  switch (link_cutscene_jump_table(gb)) {
    case 0x75e8: goto state0;
    case 0x7604: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x75e8, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x75eb);
  CALL_C(0x75eb, objectSetInvisible_hook, 0x1e7b, 0x75ee);
  CYC(0x75ee, 0x75f1); push_effect(gb, 0x75f1);
  link_cutscene_check_shield_equipped(gb);
  CYC(0x75f1, 0x75f3); A = 0x0b;
  if (!(F & FZ)) {
    CYCT(0x75f3, 0x75f5);
  } else {
    CYC(0x75f3, 0x75f5);
    CYC(0x75f5, 0x75f7); A = 0x0f;
  }
  CYC(0x75f7, 0x75fa);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CYC(0x7604, 0x7606); E = 0x05;
  CYC(0x7606, 0x7607); A = mem_rd(gb, DE);
  CYC(0x7607, 0x7608); push_effect(gb, 0x7608);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7610: goto substate0;
    case 0x761c: goto substate1;
    case 0x7630: goto substate2;
    case 0x7658: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7610, 0x7613); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x7613, 0x7615); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x7615, 0x7616); ret_effect(gb); return; }
  CYC(0x7615, 0x7616);
  CALL_C(0x7616, itemIncSubstate_hook, 0x23ef, 0x7619);
  CYC(0x7619, 0x761c);
  objectSetVisible82_hook(gb);
  return;

substate1:
  CYC(0x761c, 0x761f); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x761f, 0x7621); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7621, 0x7622); ret_effect(gb); return; }
  CYC(0x7621, 0x7622);
  CALL_C(0x7622, itemIncSubstate_hook, 0x23ef, 0x7625);
  CYC(0x7625, 0x7628); push_effect(gb, 0x7628);
  link_cutscene_check_shield_equipped(gb);
  CYC(0x7628, 0x762a); A = 0x10;
  if (!(F & FZ)) {
    CYCT(0x762a, 0x762c);
  } else {
    CYC(0x762a, 0x762c);
    CYC(0x762c, 0x762d); A = alu_inc8(gb, A);
  }
  CYC(0x762d, 0x7630);
  specialObjectSetAnimation_hook(gb);
  return;

substate2:
  CYC(0x7630, 0x7633); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x7633, 0x7635); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x7635, 0x7636); ret_effect(gb); return; }
  CYC(0x7635, 0x7636);
  CALL_C(0x7636, itemIncSubstate_hook, 0x23ef, 0x7639);
  CYC(0x7639, 0x763b); L = 0x06;
  CYC(0x763b, 0x763d); mem_wr(gb, HL, 0x68);
  CYC(0x763d, 0x763e); L = alu_inc8(gb, L);
  CYC(0x763e, 0x7640); mem_wr(gb, HL, 0x01);
  CYC(0x7640, 0x7642); B = 0x02;

spawn_heart:
  CALL_C(0x7642, getFreeInteractionSlot_hook, 0x3aef, 0x7645);
  if (!(F & FZ)) {
    CYCT(0x7645, 0x7647);
    goto set_animation;
  }
  CYC(0x7645, 0x7647);
  CYC(0x7647, 0x7649); mem_wr(gb, HL, 0xb7);
  CYC(0x7649, 0x764a); L = alu_inc8(gb, L);
  CYC(0x764a, 0x764b); A = B;
  CYC(0x764b, 0x764c); A = alu_dec8(gb, A);
  CYC(0x764c, 0x764d); mem_wr(gb, HL, A);
  CALL_C(0x764d, objectCopyPosition_hook, 0x2242, 0x7650);
  CYC(0x7650, 0x7651); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x7651, 0x7653);
    goto spawn_heart;
  }
  CYC(0x7651, 0x7653);

set_animation:
  CYC(0x7653, 0x7655); A = 0x12;
  CYC(0x7655, 0x7658);
  specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CALL_C(0x7658, specialObjectAnimate_hook, 0x2aef, 0x765b);
  CYC(0x765b, 0x765c); H = D;
  CYC(0x765c, 0x765e); L = 0x06;
  CALL_C(0x765e, decHlRef16WithCap_hook, 0x0237, 0x7661);
  if (!(F & FZ)) { CYCT(0x7661, 0x7662); ret_effect(gb); return; }
  CYC(0x7661, 0x7662);
  CYC(0x7662, 0x7665); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x7665, 0x7667); mem_wr(gb, HL, 0x09);
  CYC(0x7667, 0x7668); ret_effect(gb);
}

void linkCutsceneB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7668, 0x766a); E = 0x04;
  CYC(0x766a, 0x766b); A = mem_rd(gb, DE);
  CYC(0x766b, 0x766c); push_effect(gb, 0x766c);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7670: goto state0;
    case 0x769c: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x7670, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x7673);
  CALL_C(0x7673, objectSetVisible81_hook, 0x1e60, 0x7676);
  CYC(0x7676, 0x7678); L = 0x06;
  CYC(0x7678, 0x767a); mem_wr(gb, HL, 0x2c);
  CYC(0x767a, 0x767b); SET_HL(HL + 1);
  CYC(0x767b, 0x767d); mem_wr(gb, HL, 0x01);
  CYC(0x767d, 0x767f); L = 0x0b;
  CYC(0x767f, 0x7681); mem_wr(gb, HL, 0xd0);
  CYC(0x7681, 0x7683); L = 0x0d;
  CYC(0x7683, 0x7685); mem_wr(gb, HL, 0x50);
  CYC(0x7685, 0x7687); A = 0x08;
  CALL_C(0x7687, specialObjectSetAnimation_hook, 0x2b0a, 0x768a);
  CYC(0x768a, 0x768b); alu_xor(gb, A);
  CYC(0x768b, 0x768e); W8(wTmpcbb9) = A;
  CYC(0x768e, 0x7691); SET_BC(0x840d);
  CALL_C(0x7691, objectCreateInteraction_hook, 0x24c5, 0x7694);
  if (!(F & FZ)) {
    CYCT(0x7694, 0x7696);
  } else {
    CYC(0x7694, 0x7696);
    CYC(0x7696, 0x7698); L = 0x56;
    CYC(0x7698, 0x769a); A = 0x00;
    CYC(0x769a, 0x769b); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x769b, 0x769c); mem_wr(gb, HL, D);
  }

state1:
  CYC(0x769c, 0x769f); A = W8(wFrameCounter);
  CYC(0x769f, 0x76a2); W8(wTmpcbb7) = A;
  CYC(0x76a2, 0x76a4); E = 0x05;
  CYC(0x76a4, 0x76a5); A = mem_rd(gb, DE);
  CYC(0x76a5, 0x76a6); push_effect(gb, 0x76a6);
  switch (link_cutscene_jump_table(gb)) {
    case 0x76ae: goto substate0;
    case 0x76bd: goto substate1;
    case 0x76cd: goto substate2;
    case 0x76e5: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x76ae, linkCutscene_oscillateZ_2_hook, 0x771a, 0x76b1);
  CYC(0x76b1, 0x76b4); SET_HL(w1Link_counter1);
  CALL_C(0x76b4, decHlRef16WithCap_hook, 0x0237, 0x76b7);
  if (!(F & FZ)) { CYCT(0x76b7, 0x76b8); ret_effect(gb); return; }
  CYC(0x76b7, 0x76b8);
  CYC(0x76b8, 0x76ba); mem_wr(gb, HL, 0x3c);
  CYC(0x76ba, 0x76bd);
  itemIncSubstate_hook(gb);
  return;

substate1:
  CALL_C(0x76bd, linkCutscene_oscillateZ_2_hook, 0x771a, 0x76c0);
  CALL_C(0x76c0, itemDecCounter1_hook, 0x23d6, 0x76c3);
  if (!(F & FZ)) { CYCT(0x76c3, 0x76c4); ret_effect(gb); return; }
  CYC(0x76c3, 0x76c4);
  CALL_C(0x76c4, itemIncSubstate_hook, 0x23ef, 0x76c7);
  CYC(0x76c7, 0x76ca); SET_BC(0x1213);
  CYC(0x76ca, 0x76cd);
  showText_hook(gb);
  return;

substate2:
  CYC(0x76cd, 0x76d0); SET_HL(0x71a2);
  CALL_C(0x76d0, linkCutscene_oscillateZ_hook, 0x7180, 0x76d3);
  CYC(0x76d3, 0x76d6); A = W8(wTextIsActive);
  CYC(0x76d6, 0x76d7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76d7, 0x76d8); ret_effect(gb); return; }
  CYC(0x76d7, 0x76d8);
  CYC(0x76d8, 0x76da); A = 0x06;
  CYC(0x76da, 0x76dd); W8(wTmpcbb9) = A;
  CYC(0x76dd, 0x76df); A = 0x91;
  CALL_C(0x76df, playSound_b00_hook, 0x0c98, 0x76e2);
  CYC(0x76e2, 0x76e5);
  linkCutscene_createGlowingOrb_hook(gb);
  return;

substate3:
  CYC(0x76e5, 0x76e7); E = 0x21;
  CYC(0x76e7, 0x76e8); A = mem_rd(gb, DE);
  CYC(0x76e8, 0x76e9); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x76e9, 0x76eb);
  } else {
    CYC(0x76e9, 0x76eb);
    CYC(0x76eb, 0x76ed); A = 0x07;
    CYC(0x76ed, 0x76f0); W8(wTmpcbb9) = A;
    CYC(0x76f0, 0x76f1); ret_effect(gb);
    return;
  }
  CALL_C(0x76f1, specialObjectAnimate_hook, 0x2aef, 0x76f4);
  CYC(0x76f4, 0x76f7); A = W8(wFrameCounter);
  CYC(0x76f7, 0x76f8); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x76f8, 0x76fb);
    objectSetInvisible_hook(gb);
    return;
  }
  CYC(0x76f8, 0x76fb);
  CYC(0x76fb, 0x76fe);
  objectSetVisible_hook(gb);
}

void linkCutsceneC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76fe, 0x7700); E = 0x04;
  CYC(0x7700, 0x7701); A = mem_rd(gb, DE);
  CYC(0x7701, 0x7702); push_effect(gb, 0x7702);
  switch (link_cutscene_jump_table(gb)) {
    case 0x7706: goto state0;
    case 0x7250: linkCutsceneRet_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x7706, linkCutscene_initOam_setVisible_incState_hook, 0x7493, 0x7709);
  CYC(0x7709, 0x770c); SET_BC(0xf804);
  CYC(0x770c, 0x770e); A = 0xff;
  CALL_C(0x770e, objectCreateExclamationMark_hook, 0x27e0, 0x7711);
  CYC(0x7711, 0x7713); L = 0x42;
  CYC(0x7713, 0x7715); mem_wr(gb, HL, 0x01);
  CYC(0x7715, 0x7717); A = 0x06;
  CYC(0x7717, 0x771a);
  specialObjectSetAnimation_hook(gb);
}

void specialObjectCode_linkInCutscene_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70a0, 0x70a2); E = 0x02;
  CYC(0x70a2, 0x70a3); A = mem_rd(gb, DE);
  CYC(0x70a3, 0x70a4); push_effect(gb, 0x70a4);
  switch (link_cutscene_jump_table(gb)) {
    case 0x70be: linkCutscene0_hook(gb); return;
    case 0x71cb: linkCutscene1_hook(gb); return;
    case 0x7251: linkCutscene2_hook(gb); return;
    case 0x7322: linkCutscene3_hook(gb); return;
    case 0x7405: linkCutscene4_hook(gb); return;
    case 0x74a7: linkCutscene5_hook(gb); return;
    case 0x74ce: linkCutscene6_hook(gb); return;
    case 0x7513: linkCutscene7_hook(gb); return;
    case 0x753f: linkCutscene8_hook(gb); return;
    case 0x7576: linkCutscene9_hook(gb); return;
    case 0x75e0: linkCutsceneA_hook(gb); return;
    case 0x7668: linkCutsceneB_hook(gb); return;
    case 0x76fe: linkCutsceneC_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
