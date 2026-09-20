#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void objectOscillateZ_body_hook(GB *gb) {
  CYC(0x492d, 0x4930); A = mem_rd(gb, 0xcc00);
  CYC(0x4930, 0x4932); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x4932, 0x4933); ret_effect(gb); return;
  }
  CYC(0x4932, 0x4933);
  CYC(0x4933, 0x4936); A = mem_rd(gb, 0xcc00);
  CYC(0x4936, 0x4938); alu_and(gb, 0x38);
  CYC(0x4938, 0x493a); A = alu_swap(gb, A);
  CYC(0x493a, 0x493b); alu_rlca(gb);
  CYC(0x493b, 0x493e); SET_HL(0x4948);
  CYC(0x493e, 0x493f); push_effect(gb, 0x493f);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  }
  CYC(0x493f, 0x4941); A = mem_rd(gb, 0xffae);
  CYC(0x4941, 0x4943); alu_add(gb, 0x0f);
  CYC(0x4943, 0x4944); E = A;
  CYC(0x4944, 0x4945); A = mem_rd(gb, DE);
  CYC(0x4945, 0x4946); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4946, 0x4947); mem_wr(gb, DE, A);
  CYC(0x4947, 0x4948); ret_effect(gb);
}

static uint16_t seasonsFairyJumpTable(GB *gb) {
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

static void seasonsFairyCreatePuff(GB *gb) {
  CYC(0x4807, 0x480a); objectCreatePuff_hook(gb);
}

static void seasonsFairyCreateSparkle0(GB *gb) {
  CYC(0x480a, 0x480d); SET_BC(0x8400);
  CYC(0x480d, 0x480f);
  CYC(0x481e, 0x4821); objectCreateInteraction_hook(gb);
}

static void seasonsFairyUpdateAnimation(GB *gb) {
  CYC(0x48b0, 0x48b3); interactionAnimate_hook(gb);
}

static void seasonsFairyCreateSparkle1(GB *gb) {
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(0x480f, 0x4812); SET_BC(0x8407);
  CALL_C(0x4812, objectCreateInteraction_hook, 0x24c5, 0x4815);
  CYC(0x4815, 0x4817); E = 0x46;
  CYC(0x4817, 0x4818); A = mem_rd(gb, DE);
  CYC(0x4818, 0x4819); L = E;
  CYC(0x4819, 0x481a); mem_wr(gb, HL, A);
  CYC(0x481a, 0x481b); ret_effect(gb);
}

static void seasonsFairyCreateSparkle2(GB *gb) {
  CYC(0x481b, 0x481e); SET_BC(0x8401);
  CYC(0x481e, 0x4821); objectCreateInteraction_hook(gb);
}

static void seasonsFairyFunc48eb(GB *gb) {
  CYC(0x48eb, 0x48ec); H = D;
  CYC(0x48ec, 0x48ee); L = 0x4b;
  CYC(0x48ee, 0x48f0); E = 0x78;
  CYC(0x48f0, 0x48f1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x48f1, 0x48f2); mem_wr(gb, DE, A);
  CYC(0x48f2, 0x48f3); B = A;
  CYC(0x48f3, 0x48f4); L = alu_inc8(gb, L);
  CYC(0x48f4, 0x48f5); E = alu_inc8(gb, E);
  CYC(0x48f5, 0x48f6); A = mem_rd(gb, HL);
  CYC(0x48f6, 0x48f7); mem_wr(gb, DE, A);
  CYC(0x48f7, 0x48f8); C = A;
  CYC(0x48f8, 0x48f9); ret_effect(gb);
}

static void seasonsFairyFunc48d0(GB *gb) {
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(0x48d0, 0x48d2); L = 0x4b;
  CYC(0x48d2, 0x48d4); E = 0x78;
  CYC(0x48d4, 0x48d5); A = mem_rd(gb, DE);
  CYC(0x48d5, 0x48d6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x48d6, 0x48d7); L = alu_inc8(gb, L);
  CYC(0x48d7, 0x48d8); E = alu_inc8(gb, E);
  CYC(0x48d8, 0x48d9); A = mem_rd(gb, DE);
  CYC(0x48d9, 0x48da); mem_wr(gb, HL, A);
  CYC(0x48da, 0x48dd); A = mem_rd(gb, w1Link_yh);
  CYC(0x48dd, 0x48de); B = A;
  CYC(0x48de, 0x48e1); A = mem_rd(gb, w1Link_xh);
  CYC(0x48e1, 0x48e2); C = A;
  CALL_C(0x48e2, objectGetRelativeAngle_hook, 0x1ea4, 0x48e5);
  CYC(0x48e5, 0x48e7); E = 0x49;
  CYC(0x48e7, 0x48e8); mem_wr(gb, DE, A);
  CALL_C(0x48e8, objectApplySpeed_hook, 0x201d, 0x48eb);
  seasonsFairyFunc48eb(gb);
}

static void seasonsFairyFunc48f9(GB *gb) {
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(0x48f9, 0x48fb); E = 0x7a;
  CYC(0x48fb, 0x48fc); A = mem_rd(gb, DE);
  CYC(0x48fc, 0x48fd); alu_or(gb, A);
  if (F & FZ) { CYCT(0x48fd, 0x48fe); ret_effect(gb); return; }
  CYC(0x48fd, 0x48fe);
  CALL_C(0x48fe, interactionDecCounter1_hook, 0x23cc, 0x4901);
  if (!(F & FZ)) { CYCT(0x4901, 0x4902); ret_effect(gb); return; }
  CYC(0x4901, 0x4902);
  CYC(0x4902, 0x4904); mem_wr(gb, HL, 0x04);
  CYC(0x4904, 0x4905); L = E;
  CYC(0x4905, 0x4906); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4906, 0x4907); ret_effect(gb);
}

static void seasonsFairyFunc4907(GB *gb) {
  CYC(0x4907, 0x490a); A = mem_rd(gb, wFrameCounter);
  CYC(0x490a, 0x490b); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x490b, 0x490c); ret_effect(gb); return; }
  CYC(0x490b, 0x490c);
  CYC(0x490c, 0x490e); E = 0x7b;
  CYC(0x490e, 0x490f); A = mem_rd(gb, DE);
  CYC(0x490f, 0x4910); A = alu_inc8(gb, A);
  CYC(0x4910, 0x4912); alu_and(gb, 0x1f);
  CYC(0x4912, 0x4913); mem_wr(gb, DE, A);
  CYC(0x4913, 0x4914); ret_effect(gb);
}

static void seasonsFairyCheckLinkIsClose(GB *gb) {
  CYC(0x4914, 0x4915); H = D;
  CYC(0x4915, 0x4917); L = 0x4b;
  CYC(0x4917, 0x491a); A = mem_rd(gb, w1Link_yh);
  CYC(0x491a, 0x491c); alu_add(gb, 0xf0);
  CYC(0x491c, 0x491d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x491d, 0x491f); alu_add(gb, 0x04);
  CYC(0x491f, 0x4921); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x4921, 0x4922); ret_effect(gb); return; }
  CYC(0x4921, 0x4922);
  CYC(0x4922, 0x4924); L = 0x4d;
  CYC(0x4924, 0x4927); A = mem_rd(gb, w1Link_xh);
  CYC(0x4927, 0x4928); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4928, 0x492a); alu_add(gb, 0x02);
  CYC(0x492a, 0x492c); alu_cp(gb, 0x05);
  CYC(0x492c, 0x492d); ret_effect(gb);
}

void interactionCode50_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x479b, 0x479d); E = 0x44;
  CYC(0x479d, 0x479e); A = mem_rd(gb, DE);
  CYC(0x479e, 0x479f); push_effect(gb, 0x479f);
  switch (seasonsFairyJumpTable(gb)) {
    case 0x47ab: goto state0;
    case 0x4821: goto state1;
    case 0x4865: goto state2;
    case 0x488a: goto state3;
    case 0x48b3: goto state4;
    case 0x48c0: goto state5;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x47ab, 0x47ad); E = 0x45;
  CYC(0x47ad, 0x47ae); A = mem_rd(gb, DE);
  CYC(0x47ae, 0x47af); push_effect(gb, 0x47af);
  switch (seasonsFairyJumpTable(gb)) {
    case 0x47b5: goto substate0;
    case 0x47cc: goto substate1;
    case 0x47da: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x47b5, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x47b8);
  CYC(0x47b8, 0x47ba); L = 0x45;
  CYC(0x47ba, 0x47bc); mem_wr(gb, HL, 0x01);
  CYC(0x47bc, 0x47be); L = 0x46;
  CYC(0x47be, 0x47c0); mem_wr(gb, HL, 0x01);
  CYC(0x47c0, 0x47c2); L = 0x4f;
  CYC(0x47c2, 0x47c4); mem_wr(gb, HL, 0x00);
  CYC(0x47c4, 0x47c6); A = 0x0f;
  CYC(0x47c6, 0x47c9); mem_wr(gb, 0xcc35, A);
  CYC(0x47c9, 0x47cc); playSound_b00_hook(gb); return;

substate1:
  CALL_C(0x47cc, interactionDecCounter1_hook, 0x23cc, 0x47cf);
  if (!(F & FZ)) { CYCT(0x47cf, 0x47d0); ret_effect(gb); return; }
  CYC(0x47cf, 0x47d0);
  CYC(0x47d0, 0x47d2); L = 0x45;
  CYC(0x47d2, 0x47d4); mem_wr(gb, HL, 0x02);
  CYC(0x47d4, 0x47d6); L = 0x46;
  CYC(0x47d6, 0x47d8); mem_wr(gb, HL, 0x10);
  CYC(0x47d8, 0x47da); goto create_puff;

substate2:
  CALL_C(0x47da, interactionDecCounter1_hook, 0x23cc, 0x47dd);
  if (!(F & FZ)) { CYCT(0x47dd, 0x47de); ret_effect(gb); return; }
  CYC(0x47dd, 0x47de);
  CALL_C(0x47de, interactionInitGraphics_hook, 0x15fb, 0x47e1);
  CALL_C(0x47e1, objectSetVisible80_hook, 0x1e57, 0x47e4);
  CYC(0x47e4, 0x47e5); H = D;
  CYC(0x47e5, 0x47e7); L = 0x44;
  CYC(0x47e7, 0x47e9); mem_wr(gb, HL, 0x01);
  CYC(0x47e9, 0x47eb); L = 0x45;
  CYC(0x47eb, 0x47ed); mem_wr(gb, HL, 0x00);
  CYC(0x47ed, 0x47ef); L = 0x43;
  CYC(0x47ef, 0x47f0); A = mem_rd(gb, HL);
  CYC(0x47f0, 0x47f1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x47f1, 0x47f3); goto sparkle1; }
  CYC(0x47f1, 0x47f3);
  CYC(0x47f3, 0x47f5); L = 0x46;
  CYC(0x47f5, 0x47f7); mem_wr(gb, HL, 0x78);
  CYC(0x47f7, 0x47fa); push_effect(gb, 0x47fa); seasonsFairyCreateSparkle0(gb);
  CYC(0x47fa, 0x47fd); goto update_animation;

sparkle1:
  CYC(0x47fd, 0x47ff); L = 0x46;
  CYC(0x47ff, 0x4801); mem_wr(gb, HL, 0x3c);
  CYC(0x4801, 0x4804); push_effect(gb, 0x4804); seasonsFairyCreateSparkle1(gb);
  CYC(0x4804, 0x4807); goto update_animation;

create_puff:
  CYC(0x4807, 0x480a); objectCreatePuff_hook(gb); return;

state1:
  CALL_C(0x4821, objectOscillateZ_body_hook, 0x492d, 0x4824);
  CALL_C(0x4824, interactionDecCounter1_hook, 0x23cc, 0x4827);
  if (F & FZ) { CYCT(0x4827, 0x4829); goto state1_expired; }
  CYC(0x4827, 0x4829);
  CYC(0x4829, 0x482c); push_effect(gb, 0x482c); seasonsFairyUpdateAnimation(gb);
  CYC(0x482c, 0x482f); A = mem_rd(gb, wFrameCounter);
  CYC(0x482f, 0x4830); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4830, 0x4833); objectSetInvisible_hook(gb); return; }
  CYC(0x4830, 0x4833);
  CYC(0x4833, 0x4836); objectSetVisible_hook(gb); return;

state1_expired:
  CYC(0x4836, 0x4838); L = 0x43;
  CYC(0x4838, 0x4839); A = mem_rd(gb, HL);
  CYC(0x4839, 0x483a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x483a, 0x483c); goto state1_normal; }
  CYC(0x483a, 0x483c);
  CYC(0x483c, 0x483e); L = 0x44;
  CYC(0x483e, 0x4840); mem_wr(gb, HL, 0x05);
  CYC(0x4840, 0x4843); SET_HL(0xcfc0);
  CYC(0x4843, 0x4845); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CALL_C(0x4845, objectSetVisible_hook, 0x1e84, 0x4848);
  CYC(0x4848, 0x484a); goto update_animation;

state1_normal:
  CYC(0x484a, 0x484c); L = 0x44;
  CYC(0x484c, 0x484d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x484d, 0x484f); L = 0x4f;
  CYC(0x484f, 0x4851); mem_wr(gb, HL, 0x00);
  CYC(0x4851, 0x4853); L = 0x7a;
  CYC(0x4853, 0x4855); mem_wr(gb, HL, 0x30);
  CYC(0x4855, 0x4857); L = 0x49;
  CYC(0x4857, 0x4859); mem_wr(gb, HL, 0x00);
  CYC(0x4859, 0x485b); L = 0x50;
  CYC(0x485b, 0x485d); mem_wr(gb, HL, 0x14);
  CALL_C(0x485d, objectSetVisible_hook, 0x1e84, 0x4860);
  CYC(0x4860, 0x4862); A = 0x4f;
  CALL_C(0x4862, playSound_b00_hook, 0x0c98, 0x4865);

state2:
  CALL_C(0x4865, objectApplySpeed_hook, 0x201d, 0x4868);
  CYC(0x4868, 0x4869); H = D;
  CYC(0x4869, 0x486b); L = 0x4b;
  CYC(0x486b, 0x486c); A = mem_rd(gb, HL);
  CYC(0x486c, 0x486e); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x486e, 0x4870); goto update_animation; }
  CYC(0x486e, 0x4870);
  CYC(0x4870, 0x4872); L = 0x44;
  CYC(0x4872, 0x4873); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4873, 0x4875); L = 0x46;
  CYC(0x4875, 0x4877); mem_wr(gb, HL, 0x04);
  CYC(0x4877, 0x4879); L = 0x7b;
  CYC(0x4879, 0x487b); mem_wr(gb, HL, 0x00);
  CYC(0x487b, 0x487e); A = mem_rd(gb, w1Link_yh);
  CYC(0x487e, 0x4880); L = 0x4b;
  CYC(0x4880, 0x4881); mem_wr(gb, HL, A);
  CYC(0x4881, 0x4884); A = mem_rd(gb, w1Link_xh);
  CYC(0x4884, 0x4886); L = 0x4d;
  CYC(0x4886, 0x4887); mem_wr(gb, HL, A);
  CYC(0x4887, 0x488a); push_effect(gb, 0x488a); seasonsFairyFunc48eb(gb);

state3:
  CYC(0x488a, 0x488d); push_effect(gb, 0x488d); seasonsFairyCheckLinkIsClose(gb);
  if (F & FC) { CYCT(0x488d, 0x488f); goto link_close; }
  CYC(0x488d, 0x488f);
  CYC(0x488f, 0x4892); push_effect(gb, 0x4892); seasonsFairyFunc48d0(gb);
  CYC(0x4892, 0x4895); push_effect(gb, 0x4895); seasonsFairyFunc48f9(gb);
  CYC(0x4895, 0x4896); A = mem_rd(gb, DE);
  CYC(0x4896, 0x4898); E = 0x7b;
  CALL_C(0x4898, objectSetPositionInCircleArc_hook, 0x210e, 0x489b);
  CYC(0x489b, 0x489e); push_effect(gb, 0x489e); seasonsFairyFunc4907(gb);
  CYC(0x489e, 0x48a1); A = mem_rd(gb, wFrameCounter);
  CYC(0x48a1, 0x48a3); alu_and(gb, 0x07);
  if (F & FZ) { CYCT(0x48a3, 0x48a6); push_effect(gb, 0x48a6); seasonsFairyCreateSparkle2(gb); }
  else CYC(0x48a3, 0x48a6);
  CYC(0x48a6, 0x48a8); goto update_animation;

link_close:
  CYC(0x48a8, 0x48aa); L = 0x44;
  CYC(0x48aa, 0x48ab); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x48ab, 0x48ae); SET_HL(0xcfc0);
  CYC(0x48ae, 0x48b0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));

update_animation:
  seasonsFairyUpdateAnimation(gb); return;

state4:
  CALL_C(0x48b3, objectOscillateZ_body_hook, 0x492d, 0x48b6);
  CYC(0x48b6, 0x48b9); A = mem_rd(gb, 0xcfc0);
  CYC(0x48b9, 0x48bb); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x48bb, 0x48be); interactionDelete_hook(gb); return; }
  CYC(0x48bb, 0x48be);
  CYC(0x48be, 0x48c0); goto update_animation;

state5:
  CALL_C(0x48c0, objectOscillateZ_body_hook, 0x492d, 0x48c3);
  CYC(0x48c3, 0x48c6); A = mem_rd(gb, 0xcfc0);
  CYC(0x48c6, 0x48c8); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x48c8, 0x48ca); goto update_animation; }
  CYC(0x48c8, 0x48ca);
  CYC(0x48ca, 0x48cd); push_effect(gb, 0x48cd); seasonsFairyCreatePuff(gb);
  CYC(0x48cd, 0x48d0); interactionDelete_hook(gb);
}
