#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t ambi_jump_table(GB *gb) {
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

static void ambi_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode4d_hook(GB *gb);
void ambi_state1_hook(GB *gb);
void ambi_updateAnimationAndRunScript_hook(GB *gb);
void ambi_runSubid01_hook(GB *gb);
void ambi_ret_hook(GB *gb);
void ambi_runSubid02_hook(GB *gb);
void ambi_runSubid03_hook(GB *gb);
void ambi_runSubid04_hook(GB *gb);
void ambi_runSubid05_hook(GB *gb);
void ambi_runSubid06_hook(GB *gb);
void ambi_runSubid08_hook(GB *gb);
void ambi_runSubid0a_hook(GB *gb);
void ambi_loadScript_hook(GB *gb);

static void ambi_init_subid(GB *gb, uint16_t sp0_) {
  CYC(0x612a, 0x612c); E = 0x42;
  CYC(0x612c, 0x612d); A = mem_rd(gb, DE);
  CYC(0x612d, 0x612e); push_effect(gb, 0x612e);
  switch (ambi_jump_table(gb)) {
    case 0x6144: goto subid1;
    case 0x6156: goto subid3;
    case 0x615e: goto subid4;
    case 0x6169: goto subid0;
    case 0x616f: goto subid2;
    case 0x6172: goto subid5;
    case 0x6180: goto subid6;
    case 0x61d1: goto subid7;
    case 0x61d4: goto subida;
    default: HANDOFF(HL);
  }

subid1:
  CYC(0x6144, 0x6147); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x6147, 0x6149); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(0x6149, 0x614c); ambi_loadScript_hook(gb); return; }
  CYC(0x6149, 0x614c);
  CALL_C(0x614c, checkIsLinkedGame_hook, 0x1992, 0x614f);
  if (!(F & FZ)) { CYCT(0x614f, 0x6150); ret_effect(gb); return; }
  CYC(0x614f, 0x6150);
  CYC(0x6150, 0x6153); SET_HL(0x6402);
  CYC(0x6153, 0x6156); interactionSetScript_hook(gb);
  return;

subid3:
  CALL_C(0x6156, getThisRoomFlags_hook, 0x197d, 0x6159);
  CYC(0x6159, 0x615b); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x615b, 0x615e); interactionDelete_hook(gb); return; }
  CYC(0x615b, 0x615e);
subid4:
  CYC(0x615e, 0x6161); SET_HL(0x68e4);
  CYC(0x6161, 0x6163); E = 0x08;
  CALL_C(0x6163, interBankCall_hook, 0x008a, 0x6166);
  CYC(0x6166, 0x6169); ambi_loadScript_hook(gb);
  return;

subid0:
  CALL_C(0x6169, soldierCheckBeatD6_hook, 0x51f8, 0x616c);
  if (!(F & FC)) { CYCT(0x616c, 0x616f); interactionDelete_hook(gb); return; }
  CYC(0x616c, 0x616f);
subid2:
  CYC(0x616f, 0x6172); ambi_loadScript_hook(gb);
  return;

subid5:
  CYC(0x6172, 0x6175); SET_HL(0x68e4);
  CYC(0x6175, 0x6177); E = 0x08;
  CALL_C(0x6177, interBankCall_hook, 0x008a, 0x617a);
  CALL_C(0x617a, objectSetVisiblec3_hook, 0x1e4e, 0x617d);
  CYC(0x617d, 0x6180); ambi_loadScript_hook(gb);
  return;

subid6:
  CALL_C(0x6180, getThisRoomFlags_hook, 0x197d, 0x6183);
  CYC(0x6183, 0x6185); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6185, 0x6188); interactionDelete_hook(gb); return; }
  CYC(0x6185, 0x6188);
  CYC(0x6188, 0x618a); A = 0x85;
  CALL_C(0x618a, loadPaletteHeader_hook, 0x050b, 0x618d);
  CYC(0x618d, 0x618e); H = D;
  CYC(0x618e, 0x6190); L = 0x5c;
  CYC(0x6190, 0x6192); A = 0x06;
  CYC(0x6192, 0x6193); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6193, 0x6194); mem_wr(gb, HL, A);
  CYC(0x6194, 0x6196); A = 0x01;
  CYC(0x6196, 0x6199); W8(wNumEnemies) = A;
  CALL_C(0x6199, getFreeInteractionSlot_hook, 0x3aef, 0x619c);
  if (F & FZ) { CYCT(0x619c, 0x619e); goto spawn_ghost_veran; }
  CYC(0x619c, 0x619e);
  CYC(0x619e, 0x61a0); E = 0x44;
  CYC(0x61a0, 0x61a1); alu_xor(gb, A);
  CYC(0x61a1, 0x61a2); mem_wr(gb, DE, A);
  CYC(0x61a2, 0x61a3); ret_effect(gb);
  return;

spawn_ghost_veran:
  CYC(0x61a3, 0x61a5); mem_wr(gb, HL, 0x3e);
  CYC(0x61a5, 0x61a6); L = alu_inc8(gb, L);
  CYC(0x61a6, 0x61a7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x61a7, 0x61aa); SET_BC(0xf000);
  CALL_C(0x61aa, objectCopyPositionWithOffset_hook, 0x225a, 0x61ad);
  CYC(0x61ad, 0x61af); A = 0xf0;
  CALL_C(0x61af, playSound_b00_hook, 0x0c98, 0x61b2);
  CYC(0x61b2, 0x61b5); SET_HL(w1Link_direction);
  CYC(0x61b5, 0x61b6); alu_xor(gb, A);
  CYC(0x61b6, 0x61b7); W8(w1Link_direction) = A;
  CYC(0x61b7, 0x61b8); W8(w1Link_angle) = A;
  CYC(0x61b8, 0x61bb); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x61bb, 0x61be); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(0x61be, 0x61bf); A = alu_dec8(gb, A);
  CYC(0x61bf, 0x61c2); W8(wActiveMusic) = A;
  CYC(0x61c2, 0x61c5); SET_HL(wcc93);
  CYC(0x61c5, 0x61c7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x61c7, 0x61c9); A = 0x0b;
  CYC(0x61c9, 0x61cc); W8(wLinkForceState) = A;
  CYC(0x61cc, 0x61ce); A = 0x16;
  CYC(0x61ce, 0x61d1); W8(wLinkStateParameter) = A;
subid7:
  CYC(0x61d1, 0x61d4); ambi_loadScript_hook(gb);
  return;

subida:
  CALL_C(0x61d4, checkIsLinkedGame_hook, 0x1992, 0x61d7);
  if (F & FZ) { CYCT(0x61d7, 0x61da); interactionDelete_hook(gb); return; }
  CYC(0x61d7, 0x61da);
  CYC(0x61da, 0x61dd); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x61dd, 0x61df); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x61df, 0x61e2); interactionDelete_hook(gb); return; }
  CYC(0x61df, 0x61e2);
  CYC(0x61e2, 0x61e5); ambi_loadScript_hook(gb);
}

void interactionCode4d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x610e, 0x6110); E = 0x44;
  CYC(0x6110, 0x6111); A = mem_rd(gb, DE);
  CYC(0x6111, 0x6112); push_effect(gb, 0x6112);
  switch (ambi_jump_table(gb)) {
    case 0x6116: goto state0;
    case 0x61e5: ambi_state1_hook(gb); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x6116, 0x6118); A = 1;
  CYC(0x6118, 0x6119); mem_wr(gb, DE, A);
  CALL_C(0x6119, interactionInitGraphics_hook, 0x15fb, 0x611c);
  CALL_C(0x611c, objectSetVisiblec2_hook, 0x1e45, 0x611f);
  CYC(0x611f, 0x6122); push_effect(gb, 0x6122); ambi_init_subid(gb, sp0_);
  CYC(0x6122, 0x6124); E = 0x40;
  CYC(0x6124, 0x6125); A = mem_rd(gb, DE);
  CYC(0x6125, 0x6126); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6126, 0x6129); objectMarkSolidPosition_hook(gb); return; }
  CYC(0x6126, 0x6129);
  CYC(0x6129, 0x612a); ret_effect(gb);
}

void ambi_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x61e5, 0x61e7); E = 0x42;
  CYC(0x61e7, 0x61e8); A = mem_rd(gb, DE);
  CYC(0x61e8, 0x61e9); push_effect(gb, 0x61e9);
  switch (ambi_jump_table(gb)) {
    case 0x61ff: ambi_updateAnimationAndRunScript_hook(gb); return;
    case 0x6205: ambi_runSubid01_hook(gb); return;
    case 0x6246: ambi_runSubid02_hook(gb); return;
    case 0x626c: ambi_runSubid03_hook(gb); return;
    case 0x62ce: ambi_runSubid04_hook(gb); return;
    case 0x62f8: ambi_runSubid05_hook(gb); return;
    case 0x630c: ambi_runSubid06_hook(gb); return;
    case 0x631a: ambi_runSubid08_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x632b: ambi_runSubid0a_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void ambi_updateAnimationAndRunScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x61ff, interactionAnimate_hook, 0x261b, 0x6202);
  CYC(0x6202, 0x6205); interactionRunScript_hook(gb);
}

void ambi_runSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6205, checkIsLinkedGame_hook, 0x1992, 0x6208);
  if (F & FZ) { CYCT(0x6208, 0x620a); goto update_substate; }
  CYC(0x6208, 0x620a);
  CYC(0x620a, 0x620d); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x620d, 0x620f); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(0x620f, 0x6212); goto update_substate; }
  CYC(0x620f, 0x6212);
  CALL_C(0x6212, interactionAnimate_hook, 0x261b, 0x6215);
  CYC(0x6215, 0x6218); SET_HL(0x5613);
  CYC(0x6218, 0x621a); E = 0x15;
  CYC(0x621a, 0x621d); interBankCall_hook(gb);
  return;

update_substate:
  CYC(0x621d, 0x621f); E = 0x45;
  CYC(0x621f, 0x6220); A = mem_rd(gb, DE);
  CYC(0x6220, 0x6221); push_effect(gb, 0x6221);
  switch (ambi_jump_table(gb)) {
    case 0x6227: goto substate0;
    case 0x6239: goto substate1;
    case 0x61ff: ambi_updateAnimationAndRunScript_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x6227, 0x622a); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x622a, 0x622c); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x622c, 0x622e); ambi_updateAnimationAndRunScript_hook(gb); return; }
  CYC(0x622c, 0x622e);
  CYC(0x622e, 0x6231); SET_HL(0x73db);
  CYC(0x6231, 0x6233); E = 0x08;
  CALL_C(0x6233, interBankCall_hook, 0x008a, 0x6236);
  CYC(0x6236, 0x6239); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x6239, 0x623b); C = 0x20;
  CALL_C(0x623b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x623e);
  if (!(F & FZ)) { CYCT(0x623e, 0x623f); ret_effect(gb); return; }
  CYC(0x623e, 0x623f);
  CALL_C(0x623f, interactionIncSubstate_hook, 0x23e5, 0x6242);
  CYC(0x6242, 0x6244); L = 0x7e;
  CYC(0x6244, 0x6245); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  ambi_ret_hook(gb);
}

void ambi_ret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6245, 0x6246); ret_effect(gb);
}

void ambi_runSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6246, 0x6248); E = 0x45;
  CYC(0x6248, 0x6249); A = mem_rd(gb, DE);
  CYC(0x6249, 0x624a); push_effect(gb, 0x624a);
  switch (ambi_jump_table(gb)) {
    case 0x6250: goto substate0;
    case 0x6257: goto substate1;
    case 0x2758: interactionAnimateBasedOnSpeed_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x6250, ambi_updateAnimationAndRunScript_hook, 0x61ff, 0x6253);
  if (!(F & FC)) { CYCT(0x6253, 0x6254); ret_effect(gb); return; }
  CYC(0x6253, 0x6254);
  CYC(0x6254, 0x6257); interactionIncSubstate_hook(gb);
  return;

substate1:
  CALL_C(0x6257, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x625a);
  CALL_C(0x625a, objectApplySpeed_hook, 0x201d, 0x625d);
  CYC(0x625d, 0x6260); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x6260, 0x6262); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(0x6262, 0x6263); ret_effect(gb); return; }
  CYC(0x6262, 0x6263);
  CALL_C(0x6263, interactionIncSubstate_hook, 0x23e5, 0x6266);
  CYC(0x6266, 0x6269); SET_BC(0x5040);
  CYC(0x6269, 0x626c); interactionSetPosition_hook(gb);
}

void ambi_runSubid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x626c, 0x626e); E = 0x45;
  CYC(0x626e, 0x626f); A = mem_rd(gb, DE);
  CYC(0x626f, 0x6270); push_effect(gb, 0x6270);
  switch (ambi_jump_table(gb)) {
    case 0x627a: goto substate0;
    case 0x6295: goto animate;
    case 0x629b: goto substate1;
    case 0x62aa: goto substate2;
    case 0x62c1: goto substate3;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x627a, 0x627d); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x627d, 0x627f); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x627f, 0x6281); goto animate; }
  CYC(0x627f, 0x6281);
  CALL_C(0x6281, interactionIncSubstate_hook, 0x23e5, 0x6284);
  CYC(0x6284, 0x6286); L = 0x46;
  CYC(0x6286, 0x6288); mem_wr(gb, HL, 0x14);
  CALL_C(0x6288, getFreePartSlot_hook, 0x3e8e, 0x628b);
  if (!(F & FZ)) { CYCT(0x628b, 0x628c); ret_effect(gb); return; }
  CYC(0x628b, 0x628c);
  CYC(0x628c, 0x628e); mem_wr(gb, HL, 0x27);
  CYC(0x628e, 0x628f); L = alu_inc8(gb, L);
  CYC(0x628f, 0x6290); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6290, 0x6291); L = alu_inc8(gb, L);
  CYC(0x6291, 0x6292); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6292, 0x6295); objectCopyPosition_hook(gb);
  return;

animate:
  CALL_C(0x6295, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x6298);
  CYC(0x6298, 0x629b); interactionRunScript_hook(gb);
  return;

substate1:
  CALL_C(0x629b, interactionDecCounter1_hook, 0x23cc, 0x629e);
  if (!(F & FZ)) { CYCT(0x629e, 0x629f); ret_effect(gb); return; }
  CYC(0x629e, 0x629f);
  CYC(0x629f, 0x62a0); alu_xor(gb, A);
  CYC(0x62a0, 0x62a3); W8(wTmpcbb3) = A;
  CYC(0x62a3, 0x62a4); A = alu_dec8(gb, A);
  CYC(0x62a4, 0x62a7); W8(wTmpcbba) = A;
  CYC(0x62a7, 0x62aa); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(0x62aa, 0x62ad); SET_HL(wTmpcbb3);
  CYC(0x62ad, 0x62af); B = 2;
  CALL_C(0x62af, flashScreen_hook, 0x2d73, 0x62b2);
  if (F & FZ) { CYCT(0x62b2, 0x62b3); ret_effect(gb); return; }
  CYC(0x62b2, 0x62b3);
  CALL_C(0x62b3, interactionIncSubstate_hook, 0x23e5, 0x62b6);
  CYC(0x62b6, 0x62b9); SET_BC(0x8408);
  CALL_C(0x62b9, objectCreateInteraction_hook, 0x24c5, 0x62bc);
  CYC(0x62bc, 0x62be); A = 2;
  CYC(0x62be, 0x62c1); fadeinFromWhiteWithDelay_hook(gb);
  return;

substate3:
  CYC(0x62c1, 0x62c4); A = W8(wPaletteThread_mode);
  CYC(0x62c4, 0x62c5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x62c5, 0x62c6); ret_effect(gb); return; }
  CYC(0x62c5, 0x62c6);
  CYC(0x62c6, 0x62c8); A = 2;
  CYC(0x62c8, 0x62cb); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(0x62cb, 0x62ce); interactionIncSubstate_hook(gb);
}

void ambi_runSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62ce, 0x62d0); E = 0x45;
  CYC(0x62d0, 0x62d1); A = mem_rd(gb, DE);
  CYC(0x62d1, 0x62d2); push_effect(gb, 0x62d2);
  switch (ambi_jump_table(gb)) {
    case 0x62d8: goto substate0;
    case 0x62e7: goto substate1;
    case 0x261b: interactionAnimate_hook(gb); return;
    default: HANDOFF(HL);
  }

substate0:
  CALL_C(0x62d8, ambi_updateAnimationAndRunScript_hook, 0x61ff, 0x62db);
  if (!(F & FC)) { CYCT(0x62db, 0x62dc); ret_effect(gb); return; }
  CYC(0x62db, 0x62dc);
  CYC(0x62dc, 0x62dd); alu_xor(gb, A);
  CYC(0x62dd, 0x62e0); W8(wTmpcbb3) = A;
  CYC(0x62e0, 0x62e1); A = alu_dec8(gb, A);
  CYC(0x62e1, 0x62e4); W8(wTmpcbba) = A;
  CYC(0x62e4, 0x62e7); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(0x62e7, 0x62ea); SET_HL(wTmpcbb3);
  CYC(0x62ea, 0x62ec); B = 2;
  CALL_C(0x62ec, flashScreen_hook, 0x2d73, 0x62ef);
  if (F & FZ) { CYCT(0x62ef, 0x62f0); ret_effect(gb); return; }
  CYC(0x62ef, 0x62f0);
  CYC(0x62f0, 0x62f2); A = 3;
  CYC(0x62f2, 0x62f5); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(0x62f5, 0x62f8); interactionIncSubstate_hook(gb);
}

void ambi_runSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x62f8, interactionRunScript_hook, 0x2552, 0x62fb);
  if (F & FC) { CYCT(0x62fb, 0x62fe); interactionDelete_hook(gb); return; }
  CYC(0x62fb, 0x62fe);
  CYC(0x62fe, 0x6301); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x6301, 0x6303); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x6303, 0x6306); interactionAnimate_hook(gb); return; }
  CYC(0x6303, 0x6306);
  CYC(0x6306, 0x6307); ret_effect(gb);
}

void ambi_runSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x630c, interactionRunScript_hook, 0x2552, 0x630f);
  if (!(F & FC)) { CYCT(0x630f, 0x6312); interactionAnimate_hook(gb); return; }
  CYC(0x630f, 0x6312);
  CYC(0x6312, 0x6314); A = 1;
  CYC(0x6314, 0x6317); W8(wLoadedTreeGfxIndex) = A;
  CYC(0x6317, 0x631a); interactionDelete_hook(gb);
}

void ambi_runSubid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x631a, ambi_updateAnimationAndRunScript_hook, 0x61ff, 0x631d);
  if (!(F & FC)) { CYCT(0x631d, 0x631e); ret_effect(gb); return; }
  CYC(0x631d, 0x631e);
  CYC(0x631e, 0x6320); A = 1;
  CYC(0x6320, 0x6323); W8(wTmpcbb8) = A;
  CYC(0x6323, 0x6325); A = 8;
  CYC(0x6325, 0x6328); W8(wCutsceneTrigger) = A;
  CYC(0x6328, 0x632b); interactionDelete_hook(gb);
}

void ambi_runSubid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x632b, npcFaceLinkAndAnimate_hook, 0x26a9, 0x632e);
  CYC(0x632e, 0x6331); interactionRunScript_hook(gb);
}

void ambi_loadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6331, 0x6333); E = 0x42;
  CYC(0x6333, 0x6334); A = mem_rd(gb, DE);
  CYC(0x6334, 0x6337); SET_HL(0x633e);
  CYC(0x6337, 0x6338); ambi_add_double_index(gb, 0x6338);
  CYC(0x6338, 0x6339); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6339, 0x633a); H = mem_rd(gb, HL);
  CYC(0x633a, 0x633b); L = A;
  CYC(0x633b, 0x633e); interactionSetScript_hook(gb);
}
