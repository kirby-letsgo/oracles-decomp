#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(rickyCutsceneJump), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(rickyCutsceneJump), (from), (to), true)

#define dimitriCutsceneData_bank06 SYM(specialObjectCode_dimitriCutscene__substate0__data)
#define mapleCutsceneData_bank06 SYM(specialObjectCode_mapleCutscene__initPositionSpeedAnimation__data)

static uint16_t companion_cutscene_jump_table(GB *gb) {
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

static void companion_cutscene_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void companion_cutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void specialObjectCode_rickyCutscene_hook(GB *gb);
void rickyCutscene_state1_hook(GB *gb);
void specialObjectCode_mooshCutscene_hook(GB *gb);
void specialObjectCode_dimitriCutscene_hook(GB *gb);
void specialObjectCode_mapleCutscene_hook(GB *gb);

void rickyCutsceneJump_hook(GB *gb) {
  BASE(rickyCutsceneJump);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xfe00);
  CALL_C(b_+3, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, SYM(companionCutsceneInitOam)); specialObjectSetAnimation_hook(gb);
}

void companionCutsceneInitOam_hook(GB *gb) {
  BASE(companionCutsceneInitOam);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); SET_HL((SYM(nextToKeyDoor) + 49));
  CYC(b_+6, b_+8); E = 0x05;
  CALL_C(b_+8, interBankCall_hook, 0x008a, b_+11);
  CYC(b_+11, SYM(rickyCutscene_state1)); objectSetVisiblec0_hook(gb);
}

void companionCutsceneFunc_7081_hook(GB *gb) {
  BASE(companionCutsceneFunc_7081);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_sub(gb, 0x04);
  CYC(b_+2, b_+4); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = 0x09;
  CALL_C(b_+7, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  CYC(b_+11, b_+13); alu_and(gb, 0x03);
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = 0x08;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = 0x36;
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, SYM(companionCutsceneDecAngle)); specialObjectSetAnimation_hook(gb);
}

void companionCutsceneDecAngle_hook(GB *gb) {
  BASE(companionCutsceneDecAngle);
  CYC(b_+0, b_+2); E = 0x09;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x1f);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, SYM(specialObjectCode_linkInCutscene_b06)); ret_effect(gb);
}

void specialObjectCode_companionCutscene_b06_hook(GB *gb) {
  BASE(specialObjectCode_companionCutscene_b06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Companion_id);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x0f);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == SYM(specialObjectCode_rickyCutscene)) { specialObjectCode_rickyCutscene_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_dimitriCutscene)) { specialObjectCode_dimitriCutscene_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_mooshCutscene)) { specialObjectCode_mooshCutscene_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_mapleCutscene)) { specialObjectCode_mapleCutscene_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void specialObjectCode_rickyCutscene_hook(GB *gb) {
  BASE(specialObjectCode_rickyCutscene);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+9) { break; }
    else if (jt_ == SYM(rickyCutscene_state1)) { rickyCutscene_state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+9, companionCutsceneInitOam_hook, SYM(companionCutsceneInitOam), b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x10;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x50);
  CYC(b_+17, b_+19); L = 0x09;
  CYC(b_+19, SYM(rickyCutsceneJump)); mem_wr(gb, HL, 0x08);
  rickyCutsceneJump_hook(gb);
}

void rickyCutscene_state1_hook(GB *gb) {
  BASE(rickyCutscene_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x02;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+8) { CYC(b_+8, b_+9); ret_effect(gb); return; }
    else if (jt_ == b_+9) { break; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CYC(b_+9, b_+11); E = 0x05;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+35) { goto substate0; }
    else if (jt_ == b_+38) { goto substate1; }
    else if (jt_ == b_+89) { goto substate2; }
    else if (jt_ == b_+99) { goto substate3; }
    else if (jt_ == b_+115) { goto substate4; }
    else if (jt_ == b_+155) { goto substate5; }
    else if (jt_ == b_+171) { goto substate6; }
    else if (jt_ == b_+213) { goto substate7; }
    else if (jt_ == b_+234) { goto substate8; }
    else if (jt_ == b_+256) { goto substate9; }
    else if (jt_ == b_+289) { goto substate_a; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+35, b_+37); L = 0x05;
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

substate1:
  CALL_C(b_+38, objectApplySpeed_hook, SYM(objectApplySpeed), b_+41);
  CYC(b_+41, b_+43); E = 0x0d;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto update_z; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+51); SET_HL(w1Link_xh);
  CYC(b_+51, b_+52); B = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_add(gb, 0x18);
  CYC(b_+54, b_+55); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+55, b_+57); goto update_z; }
  CYC(b_+55, b_+57);
  CALL_C(b_+57, itemIncSubstate_hook, SYM(itemIncSubstate), b_+60);
  CYC(b_+60, b_+61); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+61, b_+63); L = 0x0e;
  CYC(b_+63, b_+64); alu_xor(gb, A);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+65, b_+66); mem_wr(gb, HL, A);
  CYC(b_+66, b_+68); L = 0x06;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x3c);
  CYC(b_+70, b_+73); specialObjectAnimate_hook(gb); return;

update_z:
  CYC(b_+73, b_+75); C = 0x40;
  CALL_C(b_+75, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+79); ret_effect(gb); return; }
  CYC(b_+78, b_+79);
  CALL_C(b_+79, itemIncSubstate_hook, SYM(itemIncSubstate), b_+82);
  CYC(b_+82, b_+84); L = 0x06;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x08);
  CYC(b_+86, b_+89); specialObjectAnimate_hook(gb); return;

substate2:
  CALL_C(b_+89, itemDecCounter1_hook, SYM(itemDecCounter1), b_+92);
  if (!(F & FZ)) { CYCT(b_+92, b_+93); ret_effect(gb); return; }
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+95); L = 0x05;
  CYC(b_+95, b_+96); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+96, b_+99); rickyCutsceneJump_hook(gb); return;

substate3:
  CALL_C(b_+99, itemDecCounter1_hook, SYM(itemDecCounter1), b_+102);
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; }
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+105); L = 0x05;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+108); L = 0x06;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x5a);
  CYC(b_+110, b_+112); A = 0x14;
  CYC(b_+112, b_+115); specialObjectSetAnimation_hook(gb); return;

substate4:
  CALL_C(b_+115, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+118);
  CALL_C(b_+118, itemDecCounter1_hook, SYM(itemDecCounter1), b_+121);
  if (!(F & FZ)) { CYCT(b_+121, b_+122); ret_effect(gb); return; }
  CYC(b_+121, b_+122);
  CYC(b_+122, b_+124); L = 0x05;
  CYC(b_+124, b_+125); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+125, b_+127); L = 0x06;
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x0c);
  CYC(b_+129, b_+131); A = 0x1f;
  CALL_C(b_+131, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+134);
  CALL_C(b_+134, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+137);
  if (!(F & FZ)) { CYCT(b_+137, b_+138); ret_effect(gb); return; }
  CYC(b_+137, b_+138);
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x07);
  CYC(b_+140, b_+143); SET_BC(0xf812);
  CYC(b_+143, b_+146); objectCopyPositionWithOffset_hook(gb); return;

substate5:
  CALL_C(b_+155, itemDecCounter1_hook, SYM(itemDecCounter1), b_+158);
  if (!(F & FZ)) { CYCT(b_+158, b_+159); ret_effect(gb); return; }
  CYC(b_+158, b_+159);
  CYC(b_+159, b_+161); L = 0x05;
  CYC(b_+161, b_+162); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+162, b_+164); L = 0x06;
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x3c);
  CYC(b_+166, b_+168); A = 0x1e;
  CYC(b_+168, b_+171); specialObjectSetAnimation_hook(gb); return;

substate6:
  CALL_C(b_+171, itemDecCounter1_hook, SYM(itemDecCounter1), b_+174);
  if (!(F & FZ)) { CYCT(b_+174, b_+175); ret_effect(gb); return; }
  CYC(b_+174, b_+175);
  CYC(b_+175, b_+177); L = 0x05;
  CYC(b_+177, b_+178); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+178, b_+179); L = alu_inc8(gb, L);
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x1e);
  CYC(b_+181, b_+184); SET_HL(wActiveRing);
  CYC(b_+184, b_+186); mem_wr(gb, HL, 0xff);
  CYC(b_+186, b_+188); A = 0x81;
  CYC(b_+188, b_+191); W8(wLinkInAir) = A;
  CYC(b_+191, b_+194); SET_HL(w1Link_speed);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0x14);
  CYC(b_+196, b_+198); L = 0x14;
  CYC(b_+198, b_+200); mem_wr(gb, HL, 0x00);
  CYC(b_+200, b_+201); L = alu_inc8(gb, L);
  CYC(b_+201, b_+203); mem_wr(gb, HL, 0xfe);
  CYC(b_+203, b_+205); A = 0x18;
  CYC(b_+205, b_+208); W8(w1Link_angle) = A;
  CYC(b_+208, b_+210); A = 0x53;
  CYC(b_+210, b_+213); playSound_b00_hook(gb); return;

substate7:
  CALL_C(b_+213, itemDecCounter1_hook, SYM(itemDecCounter1), b_+216);
  if (!(F & FZ)) { CYCT(b_+216, b_+217); ret_effect(gb); return; }
  CYC(b_+216, b_+217);
  CYC(b_+217, b_+219); L = 0x05;
  CYC(b_+219, b_+220); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+220, b_+222); L = 0x06;
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x14);
  CYC(b_+224, b_+225); alu_xor(gb, A);
  CYC(b_+225, b_+228); SET_HL(w1Link_visible);
  CYC(b_+228, b_+229); mem_wr(gb, HL, A);
  CYC(b_+229, b_+230); A = alu_inc8(gb, A);
  CYC(b_+230, b_+233); W8(wDisabledObjects) = A;
  CYC(b_+233, b_+234); ret_effect(gb); return;

substate8:
  CALL_C(b_+234, itemDecCounter1_hook, SYM(itemDecCounter1), b_+237);
  if (!(F & FZ)) { CYCT(b_+237, b_+238); ret_effect(gb); return; }
  CYC(b_+237, b_+238);
  CYC(b_+238, b_+240); L = 0x05;
  CYC(b_+240, b_+241); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+241, b_+243); L = 0x09;
  CYC(b_+243, b_+245); mem_wr(gb, HL, 0x18);

jump:
  CYC(b_+245, b_+247); A = 0x1c;
  CALL_C(b_+247, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+250);
  CYC(b_+250, b_+253); SET_BC(0xfe00);
  CYC(b_+253, b_+256); objectSetSpeedZ_hook(gb); return;

substate9:
  CALL_C(b_+256, objectApplySpeed_hook, SYM(objectApplySpeed), b_+259);
  CYC(b_+259, b_+261); E = 0x0d;
  CYC(b_+261, b_+262); A = mem_rd(gb, DE);
  CYC(b_+262, b_+264); alu_sub(gb, 0x10);
  CYC(b_+264, b_+265); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+265, b_+267); goto substate9_update_z; }
  CYC(b_+265, b_+267);
  CYC(b_+267, b_+270); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+270, b_+272); mem_wr(gb, HL, 0x01);
  CYC(b_+272, b_+273); ret_effect(gb); return;

substate9_update_z:
  CYC(b_+273, b_+275); C = 0x40;
  CALL_C(b_+275, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+278);
  if (!(F & FZ)) { CYCT(b_+278, b_+279); ret_effect(gb); return; }
  CYC(b_+278, b_+279);
  CALL_C(b_+279, itemIncSubstate_hook, SYM(itemIncSubstate), b_+282);
  CYC(b_+282, b_+284); L = 0x06;
  CYC(b_+284, b_+286); mem_wr(gb, HL, 0x08);
  CYC(b_+286, b_+289); specialObjectAnimate_hook(gb); return;

substate_a:
  CALL_C(b_+289, itemDecCounter1_hook, SYM(itemDecCounter1), b_+292);
  if (!(F & FZ)) { CYCT(b_+292, b_+293); ret_effect(gb); return; }
  CYC(b_+292, b_+293);
  CYC(b_+293, b_+295); L = 0x05;
  CYC(b_+295, b_+296); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+296, SYM(specialObjectCode_mooshCutscene)); goto jump;
}

void specialObjectCode_mooshCutscene_hook(GB *gb) {
  BASE(specialObjectCode_mooshCutscene);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+50) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+8, companionCutsceneInitOam_hook, SYM(companionCutsceneInitOam), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x06;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x5a);
  CYC(b_+16, b_+18); L = 0x10;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x37);
  CYC(b_+20, b_+22); L = 0x36;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x05);
  CYC(b_+24, b_+26); L = 0x09;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x10);
  CYC(b_+28, b_+30); L = 0x0e;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xff);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xe0);
  CALL_C(b_+35, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+38);
  if (!(F & FZ)) {
    CYCT(b_+38, b_+40);
  } else {
    CYC(b_+38, b_+40);
    CYC(b_+40, b_+42); mem_wr(gb, HL, 0xc0);
    CYC(b_+42, b_+44); L = 0x57;
    CYC(b_+44, b_+45); mem_wr(gb, HL, D);
  }
  CYC(b_+45, b_+47); A = 0x07;
  CYC(b_+47, b_+50); specialObjectSetAnimation_hook(gb); return;

state1:
  CYC(b_+50, b_+52); E = 0x05;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+54, b_+56);
  } else {
    CYC(b_+54, b_+56);
    CALL_C(b_+56, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+59);
    CALL_C(b_+59, objectApplySpeed_hook, SYM(objectApplySpeed), b_+62);
  }
  CYC(b_+62, b_+64); E = 0x05;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); push_effect(gb, b_+66);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+76) { goto substate0; }
    else if (jt_ == b_+86) { goto substate1; }
    else if (jt_ == b_+98) { goto substate2; }
    else if (jt_ == b_+122) { goto substate3; }
    else if (jt_ == b_+138) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+76, itemDecCounter1_hook, SYM(itemDecCounter1), b_+79);
  if (!(F & FZ)) { CYCT(b_+79, b_+80); ret_effect(gb); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x48);
  CYC(b_+82, b_+84); L = 0x05;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+85, b_+86); ret_effect(gb); return;

substate1:
  CALL_C(b_+86, itemDecCounter1_hook, SYM(itemDecCounter1), b_+89);
  if (!(F & FZ)) { CYCT(b_+89, b_+90); ret_effect(gb); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x06);
  CYC(b_+92, b_+94); L = 0x05;
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+95, b_+98); companionCutsceneFunc_7081_hook(gb); return;

substate2:
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+101); L = 0x09;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+104); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(b_+104, b_+106); goto turn; }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+108); L = 0x05;
  CYC(b_+108, b_+109); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+109, b_+110); ret_effect(gb); return;

turn:
  CYC(b_+110, b_+112); L = 0x06;
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+113, b_+114); ret_effect(gb); return; }
  CYC(b_+113, b_+114);
  CALL_C(b_+114, companionCutsceneDecAngle_hook, SYM(companionCutsceneDecAngle), b_+117);
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x06);
  CYC(b_+119, b_+122); companionCutsceneFunc_7081_hook(gb); return;

substate3:
  CYC(b_+122, b_+123); H = D;
  CYC(b_+123, b_+125); L = 0x09;
  CYC(b_+125, b_+126); A = mem_rd(gb, HL);
  CYC(b_+126, b_+128); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); goto turn; }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+132); L = 0x05;
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+133, b_+135); A = 0x07;
  CYC(b_+135, b_+138); specialObjectSetAnimation_hook(gb); return;

substate4:
  CYC(b_+138, b_+140); E = 0x0b;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+143); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+143, b_+144); ret_effect(gb); return; }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+147); SET_HL(w1Companion_id);
  CYC(b_+147, b_+149); B = 0x3f;
  CALL_C(b_+149, clearMemory_hook, SYM(clearMemory), b_+152);
  CYC(b_+152, b_+155); SET_HL(w1Companion_id);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x10);
  CYC(b_+157, b_+159); L = 0x0b;
  CYC(b_+159, b_+161); mem_wr(gb, HL, 0xe8);
  CYC(b_+161, b_+162); L = alu_inc8(gb, L);
  CYC(b_+162, b_+163); L = alu_inc8(gb, L);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x28);
  CYC(b_+165, SYM(specialObjectCode_dimitriCutscene)); ret_effect(gb);
}

void specialObjectCode_dimitriCutscene_hook(GB *gb) {
  BASE(specialObjectCode_dimitriCutscene);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+28) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+8, companionCutsceneInitOam_hook, SYM(companionCutsceneInitOam), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x10;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x28);
  CYC(b_+16, b_+18); L = 0x0e;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0xe0);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xff);
  CYC(b_+23, b_+25); A = 0x19;
  CYC(b_+25, b_+28); specialObjectSetAnimation_hook(gb); return;

state1:
  CYC(b_+28, b_+30); E = 0x05;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+46) { goto substate0; }
    else if (jt_ == b_+90) { goto substate1; }
    else if (jt_ == b_+125) { goto substate2; }
    else if (jt_ == b_+141) { goto substate3; }
    else if (jt_ == b_+153) { goto substate4; }
    else if (jt_ == b_+176) { goto substate5; }
    else if (jt_ == b_+188) { goto substate6; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+49); L = 0x05;
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+50, b_+52); L = 0x07;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+55, b_+57);
  } else {
    CYC(b_+55, b_+57);
    CYC(b_+57, b_+58); push_effect(gb, AF);
    CYC(b_+58, b_+60); A = 0x1a;
    CALL_C(b_+60, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+63);
    CYC(b_+63, b_+64); SET_AF(pop_effect(gb));
  }
  CYC(b_+64, b_+65); B = A;
  CYC(b_+65, b_+66); alu_add(gb, A);
  CYC(b_+66, b_+67); alu_add(gb, B);
  CYC(b_+67, b_+70); SET_HL(dimitriCutsceneData_bank06);
  CYC(b_+70, b_+71); push_effect(gb, b_+71); companion_cutscene_add_a_to_hl(gb);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+72, b_+74); E = 0x09;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); C = mem_rd(gb, HL);
  CYC(b_+76, b_+77); SET_HL(HL + 1);
  CYC(b_+77, b_+78); B = mem_rd(gb, HL);
  CYC(b_+78, b_+81); objectSetSpeedZ_hook(gb); return;

substate1:
  CALL_C(b_+90, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+93);
  CALL_C(b_+93, objectApplySpeed_hook, SYM(objectApplySpeed), b_+96);
  CYC(b_+96, b_+98); C = 0x18;
  CALL_C(b_+98, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+101);
  if (!(F & FZ)) { CYCT(b_+101, b_+102); ret_effect(gb); return; }
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+103); H = D;
  CYC(b_+103, b_+105); L = 0x07;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+109); L = 0x05;
  CYC(b_+109, b_+111); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+111, b_+113);
    CYC(b_+119, b_+120); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+120, b_+122); L = 0x06;
    CYC(b_+122, b_+124); mem_wr(gb, HL, 0x06);
    CYC(b_+124, b_+125); ret_effect(gb); return;
  }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+116); L = 0x06;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x08);
  CYC(b_+118, b_+119); ret_effect(gb); return;

substate2:
  CALL_C(b_+125, itemDecCounter1_hook, SYM(itemDecCounter1), b_+128);
  if (!(F & FZ)) { CYCT(b_+128, b_+129); ret_effect(gb); return; }
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+131); L = 0x05;
  CYC(b_+131, b_+132); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+132, b_+134); L = 0x06;
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x14);
  CYC(b_+136, b_+138); A = 0x27;
  CYC(b_+138, b_+141); specialObjectSetAnimation_hook(gb); return;

substate3:
  CALL_C(b_+141, itemDecCounter1_hook, SYM(itemDecCounter1), b_+144);
  if (!(F & FZ)) { CYCT(b_+144, b_+145); ret_effect(gb); return; }
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+147); L = 0x05;
  CYC(b_+147, b_+148); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+148, b_+150); L = 0x06;
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x78);
  CYC(b_+152, b_+153); ret_effect(gb); return;

substate4:
  CALL_C(b_+153, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+156);
  CALL_C(b_+156, itemDecCounter1_hook, SYM(itemDecCounter1), b_+159);
  if (!(F & FZ)) { CYCT(b_+159, b_+160); ret_effect(gb); return; }
  CYC(b_+159, b_+160);
  CYC(b_+160, b_+162); L = 0x05;
  CYC(b_+162, b_+163); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+163, b_+165); L = 0x06;
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x3c);
  CYC(b_+167, b_+169); L = 0x09;
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x0b);
  CYC(b_+171, b_+173); L = 0x10;
  CYC(b_+173, b_+175); mem_wr(gb, HL, 0x14);
  CYC(b_+175, b_+176); ret_effect(gb); return;

substate5:
  CALL_C(b_+176, itemDecCounter1_hook, SYM(itemDecCounter1), b_+179);
  if (!(F & FZ)) { CYCT(b_+179, b_+180); ret_effect(gb); return; }
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+182); L = 0x05;
  CYC(b_+182, b_+183); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+183, b_+185); A = 0x26;
  CYC(b_+185, b_+188); specialObjectSetAnimation_hook(gb); return;

substate6:
  CALL_C(b_+188, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+191);
  CALL_C(b_+191, objectApplySpeed_hook, SYM(objectApplySpeed), b_+194);
  CYC(b_+194, b_+196); E = 0x0d;
  CYC(b_+196, b_+197); A = mem_rd(gb, DE);
  CYC(b_+197, b_+199); alu_cp(gb, 0x78);
  if (F & FZ) {
    CYC(b_+199, b_+201);
    CYC(b_+201, b_+203); A = 0x05;
    CYC(b_+203, b_+206); specialObjectSetAnimation_hook(gb); return;
  }
  CYCT(b_+199, b_+201);
  CYC(b_+206, b_+208); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+208, b_+209); ret_effect(gb); return; }
  CYC(b_+208, b_+209);
  CYC(b_+209, b_+212); SET_HL(w1Companion_id);
  CYC(b_+212, b_+214); B = 0x3f;
  CALL_C(b_+214, clearMemory_hook, SYM(clearMemory), b_+217);
  CYC(b_+217, b_+220); SET_HL(w1Companion_id);
  CYC(b_+220, b_+222); mem_wr(gb, HL, 0x0f);
  CYC(b_+222, b_+223); L = alu_inc8(gb, L);
  CYC(b_+223, b_+225); mem_wr(gb, HL, 0x01);
  CYC(b_+225, b_+227); L = 0x0b;
  CYC(b_+227, b_+229); mem_wr(gb, HL, 0x48);
  CYC(b_+229, b_+230); L = alu_inc8(gb, L);
  CYC(b_+230, b_+231); L = alu_inc8(gb, L);
  CYC(b_+231, b_+233); mem_wr(gb, HL, 0xd8);
  CYC(b_+233, SYM(specialObjectCode_mapleCutscene)); ret_effect(gb);
}

void specialObjectCode_mapleCutscene_hook(GB *gb) {
  BASE(specialObjectCode_mapleCutscene);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+65) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+8, companionCutsceneInitOam_hook, SYM(companionCutsceneInitOam), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x0f;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0xf0);
  CYC(b_+16, b_+18); L = 0x09;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x08);
  CYC(b_+20, b_+22); L = 0x06;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x5a);
  CYC(b_+24, b_+25); ret_effect(gb); return;

init_position:
  CYC(b_+25, b_+27); L = 0x07;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_add(gb, A);
  CYC(b_+29, b_+32); SET_HL(mapleCutsceneData_bank06);
  CYC(b_+32, b_+33); companion_cutscene_add_double_index(gb, b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+36); E = 0x10;
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+38, b_+40); E = 0x06;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+44); E = 0x0b;
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+49); specialObjectSetAnimation_hook(gb); return;

state1:
  CALL_C(b_+65, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+68);
  CALL_C(b_+68, objectOscillateZ_hook, SYM(objectOscillateZ), b_+71);
  CYC(b_+71, b_+73); E = 0x05;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); push_effect(gb, b_+75);
  do { uint16_t jt_ = (companion_cutscene_jump_table(gb));
    if (jt_ == b_+83) { goto substate0; }
    else if (jt_ == b_+96) { goto substate1; }
    else if (jt_ == b_+109) { goto substate2; }
    else if (jt_ == b_+141) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+83, b_+86); A = W8(wPaletteThread_mode);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+87, itemDecCounter1_hook, SYM(itemDecCounter1), b_+90);
  else CYC(b_+87, b_+90);
  if (!(F & FZ)) { CYCT(b_+90, b_+91); ret_effect(gb); return; }
  CYC(b_+90, b_+91);
  CALL_C(b_+91, itemIncSubstate_hook, SYM(itemIncSubstate), b_+94);
  CYC(b_+94, b_+96); goto init_position;

substate1:
  CALL_C(b_+96, itemDecCounter1_hook, SYM(itemDecCounter1), b_+99);
  if (!(F & FZ)) { CYCT(b_+99, b_+102); objectApplySpeed_hook(gb); return; }
  CYC(b_+99, b_+102);
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x5a);
  CYC(b_+104, b_+105); L = alu_inc8(gb, L);
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+109); itemIncSubstate_hook(gb); return;

substate2:
  CALL_C(b_+109, itemDecCounter1_hook, SYM(itemDecCounter1), b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+113); ret_effect(gb); return; }
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+114); L = alu_inc8(gb, L);
  CYC(b_+114, b_+115); A = mem_rd(gb, HL);
  CYC(b_+115, b_+117); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+117, b_+119);
    CYC(b_+130, b_+132); L = 0x05;
    CYC(b_+132, b_+133); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+133, b_+135); L = 0x09;
    CYC(b_+135, b_+136); A = mem_rd(gb, HL);
    CYC(b_+136, b_+138); alu_xor(gb, 0x10);
    CYC(b_+138, b_+139); mem_wr(gb, HL, A);
    CYC(b_+139, b_+141); goto init_position;
  }
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+120); L = alu_dec8(gb, L);
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+122, itemIncSubstate_hook, SYM(itemIncSubstate), b_+125);
  CYC(b_+125, b_+127); A = 0x07;
  CYC(b_+127, b_+130); specialObjectSetAnimation_hook(gb); return;

substate3:
  CALL_C(b_+141, itemDecCounter1_hook, SYM(itemDecCounter1), b_+144);
  if (F & FZ) {
    CYCT(b_+144, b_+146);
    CYC(b_+151, b_+153); A = 0xff;
    CYC(b_+153, b_+156); W8(wTmpcfc0_genericCutscene_cfdf) = A;
    CYC(b_+156, SYM(companionCutsceneFunc_7081)); ret_effect(gb); return;
  }
  CYC(b_+144, b_+146);
  CYC(b_+146, b_+148); C = 0x02;
  CYC(b_+148, b_+151); objectUpdateSpeedZ_paramC_hook(gb);
}
