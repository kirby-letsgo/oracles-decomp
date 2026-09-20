#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

#define dimitriCutsceneData_bank06 0x6f4b
#define mapleCutsceneData_bank06 0x7015

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6d10, 0x6d13); SET_BC(0xfe00);
  CALL_C(0x6d13, objectSetSpeedZ_hook, 0x239d, 0x6d16);
  CYC(0x6d16, 0x6d18); A = 0x02;
  CYC(0x6d18, 0x6d1b); specialObjectSetAnimation_hook(gb);
}

void companionCutsceneInitOam_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d1b, 0x6d1d); A = 0x01;
  CYC(0x6d1d, 0x6d1e); mem_wr(gb, DE, A);
  CYC(0x6d1e, 0x6d21); SET_HL(0x41f7);
  CYC(0x6d21, 0x6d23); E = 0x05;
  CALL_C(0x6d23, interBankCall_hook, 0x008a, 0x6d26);
  CYC(0x6d26, 0x6d29); objectSetVisiblec0_hook(gb);
}

void companionCutsceneFunc_7081_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7081, 0x7083); alu_sub(gb, 0x04);
  CYC(0x7083, 0x7085); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7085, 0x7086); ret_effect(gb); return; }
  CYC(0x7085, 0x7086);
  CYC(0x7086, 0x7088); E = 0x09;
  CALL_C(0x7088, convertAngleDeToDirection_hook, 0x26f8, 0x708b);
  CYC(0x708b, 0x708c); A = alu_dec8(gb, A);
  CYC(0x708c, 0x708e); alu_and(gb, 0x03);
  CYC(0x708e, 0x708f); H = D;
  CYC(0x708f, 0x7091); L = 0x08;
  CYC(0x7091, 0x7092); mem_wr(gb, HL, A);
  CYC(0x7092, 0x7094); L = 0x36;
  CYC(0x7094, 0x7095); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7095, 0x7098); specialObjectSetAnimation_hook(gb);
}

void companionCutsceneDecAngle_hook(GB *gb) {
  CYC(0x7098, 0x709a); E = 0x09;
  CYC(0x709a, 0x709b); A = mem_rd(gb, DE);
  CYC(0x709b, 0x709c); A = alu_dec8(gb, A);
  CYC(0x709c, 0x709e); alu_and(gb, 0x1f);
  CYC(0x709e, 0x709f); mem_wr(gb, DE, A);
  CYC(0x709f, 0x70a0); ret_effect(gb);
}

void specialObjectCode_companionCutscene_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6cec, 0x6cef); SET_HL(w1Companion_id);
  CYC(0x6cef, 0x6cf0); A = mem_rd(gb, HL);
  CYC(0x6cf0, 0x6cf2); alu_sub(gb, 0x0f);
  CYC(0x6cf2, 0x6cf3); push_effect(gb, 0x6cf3);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6cfb: specialObjectCode_rickyCutscene_hook(gb); return;
    case 0x6efa: specialObjectCode_dimitriCutscene_hook(gb); return;
    case 0x6e54: specialObjectCode_mooshCutscene_hook(gb); return;
    case 0x6fe4: specialObjectCode_mapleCutscene_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void specialObjectCode_rickyCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6cfb, 0x6cfd); E = 0x04;
  CYC(0x6cfd, 0x6cfe); A = mem_rd(gb, DE);
  CYC(0x6cfe, 0x6cff); A = mem_rd(gb, DE);
  CYC(0x6cff, 0x6d00); push_effect(gb, 0x6d00);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6d04: break;
    case 0x6d29: rickyCutscene_state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x6d04, companionCutsceneInitOam_hook, 0x6d1b, 0x6d07);
  CYC(0x6d07, 0x6d08); H = D;
  CYC(0x6d08, 0x6d0a); L = 0x10;
  CYC(0x6d0a, 0x6d0c); mem_wr(gb, HL, 0x50);
  CYC(0x6d0c, 0x6d0e); L = 0x09;
  CYC(0x6d0e, 0x6d10); mem_wr(gb, HL, 0x08);
  rickyCutsceneJump_hook(gb);
}

void rickyCutscene_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6d29, 0x6d2b); E = 0x02;
  CYC(0x6d2b, 0x6d2c); A = mem_rd(gb, DE);
  CYC(0x6d2c, 0x6d2d); push_effect(gb, 0x6d2d);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6d31: CYC(0x6d31, 0x6d32); ret_effect(gb); return;
    case 0x6d32: break;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CYC(0x6d32, 0x6d34); E = 0x05;
  CYC(0x6d34, 0x6d35); A = mem_rd(gb, DE);
  CYC(0x6d35, 0x6d36); push_effect(gb, 0x6d36);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6d4c: goto substate0;
    case 0x6d4f: goto substate1;
    case 0x6d82: goto substate2;
    case 0x6d8c: goto substate3;
    case 0x6d9c: goto substate4;
    case 0x6dc4: goto substate5;
    case 0x6dd4: goto substate6;
    case 0x6dfe: goto substate7;
    case 0x6e13: goto substate8;
    case 0x6e29: goto substate9;
    case 0x6e4a: goto substate_a;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x6d4c, 0x6d4e); L = 0x05;
  CYC(0x6d4e, 0x6d4f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

substate1:
  CALL_C(0x6d4f, objectApplySpeed_hook, 0x201d, 0x6d52);
  CYC(0x6d52, 0x6d54); E = 0x0d;
  CYC(0x6d54, 0x6d55); A = mem_rd(gb, DE);
  CYC(0x6d55, 0x6d57); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6d57, 0x6d59); goto update_z; }
  CYC(0x6d57, 0x6d59);
  CYC(0x6d59, 0x6d5c); SET_HL(w1Link_xh);
  CYC(0x6d5c, 0x6d5d); B = mem_rd(gb, HL);
  CYC(0x6d5d, 0x6d5f); alu_add(gb, 0x18);
  CYC(0x6d5f, 0x6d60); alu_cp(gb, B);
  if (F & FC) { CYCT(0x6d60, 0x6d62); goto update_z; }
  CYC(0x6d60, 0x6d62);
  CALL_C(0x6d62, itemIncSubstate_hook, 0x23ef, 0x6d65);
  CYC(0x6d65, 0x6d66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d66, 0x6d68); L = 0x0e;
  CYC(0x6d68, 0x6d69); alu_xor(gb, A);
  CYC(0x6d69, 0x6d6a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d6a, 0x6d6b); mem_wr(gb, HL, A);
  CYC(0x6d6b, 0x6d6d); L = 0x06;
  CYC(0x6d6d, 0x6d6f); mem_wr(gb, HL, 0x3c);
  CYC(0x6d6f, 0x6d72); specialObjectAnimate_hook(gb); return;

update_z:
  CYC(0x6d72, 0x6d74); C = 0x40;
  CALL_C(0x6d74, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6d77);
  if (!(F & FZ)) { CYCT(0x6d77, 0x6d78); ret_effect(gb); return; }
  CYC(0x6d77, 0x6d78);
  CALL_C(0x6d78, itemIncSubstate_hook, 0x23ef, 0x6d7b);
  CYC(0x6d7b, 0x6d7d); L = 0x06;
  CYC(0x6d7d, 0x6d7f); mem_wr(gb, HL, 0x08);
  CYC(0x6d7f, 0x6d82); specialObjectAnimate_hook(gb); return;

substate2:
  CALL_C(0x6d82, itemDecCounter1_hook, 0x23d6, 0x6d85);
  if (!(F & FZ)) { CYCT(0x6d85, 0x6d86); ret_effect(gb); return; }
  CYC(0x6d85, 0x6d86);
  CYC(0x6d86, 0x6d88); L = 0x05;
  CYC(0x6d88, 0x6d89); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6d89, 0x6d8c); rickyCutsceneJump_hook(gb); return;

substate3:
  CALL_C(0x6d8c, itemDecCounter1_hook, 0x23d6, 0x6d8f);
  if (!(F & FZ)) { CYCT(0x6d8f, 0x6d90); ret_effect(gb); return; }
  CYC(0x6d8f, 0x6d90);
  CYC(0x6d90, 0x6d92); L = 0x05;
  CYC(0x6d92, 0x6d93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d93, 0x6d95); L = 0x06;
  CYC(0x6d95, 0x6d97); mem_wr(gb, HL, 0x5a);
  CYC(0x6d97, 0x6d99); A = 0x14;
  CYC(0x6d99, 0x6d9c); specialObjectSetAnimation_hook(gb); return;

substate4:
  CALL_C(0x6d9c, specialObjectAnimate_hook, 0x2aef, 0x6d9f);
  CALL_C(0x6d9f, itemDecCounter1_hook, 0x23d6, 0x6da2);
  if (!(F & FZ)) { CYCT(0x6da2, 0x6da3); ret_effect(gb); return; }
  CYC(0x6da2, 0x6da3);
  CYC(0x6da3, 0x6da5); L = 0x05;
  CYC(0x6da5, 0x6da6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6da6, 0x6da8); L = 0x06;
  CYC(0x6da8, 0x6daa); mem_wr(gb, HL, 0x0c);
  CYC(0x6daa, 0x6dac); A = 0x1f;
  CALL_C(0x6dac, specialObjectSetAnimation_hook, 0x2b0a, 0x6daf);
  CALL_C(0x6daf, getFreeInteractionSlot_hook, 0x3aef, 0x6db2);
  if (!(F & FZ)) { CYCT(0x6db2, 0x6db3); ret_effect(gb); return; }
  CYC(0x6db2, 0x6db3);
  CYC(0x6db3, 0x6db5); mem_wr(gb, HL, 0x07);
  CYC(0x6db5, 0x6db8); SET_BC(0xf812);
  CYC(0x6db8, 0x6dbb); objectCopyPositionWithOffset_hook(gb); return;

substate5:
  CALL_C(0x6dc4, itemDecCounter1_hook, 0x23d6, 0x6dc7);
  if (!(F & FZ)) { CYCT(0x6dc7, 0x6dc8); ret_effect(gb); return; }
  CYC(0x6dc7, 0x6dc8);
  CYC(0x6dc8, 0x6dca); L = 0x05;
  CYC(0x6dca, 0x6dcb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6dcb, 0x6dcd); L = 0x06;
  CYC(0x6dcd, 0x6dcf); mem_wr(gb, HL, 0x3c);
  CYC(0x6dcf, 0x6dd1); A = 0x1e;
  CYC(0x6dd1, 0x6dd4); specialObjectSetAnimation_hook(gb); return;

substate6:
  CALL_C(0x6dd4, itemDecCounter1_hook, 0x23d6, 0x6dd7);
  if (!(F & FZ)) { CYCT(0x6dd7, 0x6dd8); ret_effect(gb); return; }
  CYC(0x6dd7, 0x6dd8);
  CYC(0x6dd8, 0x6dda); L = 0x05;
  CYC(0x6dda, 0x6ddb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ddb, 0x6ddc); L = alu_inc8(gb, L);
  CYC(0x6ddc, 0x6dde); mem_wr(gb, HL, 0x1e);
  CYC(0x6dde, 0x6de1); SET_HL(wActiveRing);
  CYC(0x6de1, 0x6de3); mem_wr(gb, HL, 0xff);
  CYC(0x6de3, 0x6de5); A = 0x81;
  CYC(0x6de5, 0x6de8); W8(wLinkInAir) = A;
  CYC(0x6de8, 0x6deb); SET_HL(w1Link_speed);
  CYC(0x6deb, 0x6ded); mem_wr(gb, HL, 0x14);
  CYC(0x6ded, 0x6def); L = 0x14;
  CYC(0x6def, 0x6df1); mem_wr(gb, HL, 0x00);
  CYC(0x6df1, 0x6df2); L = alu_inc8(gb, L);
  CYC(0x6df2, 0x6df4); mem_wr(gb, HL, 0xfe);
  CYC(0x6df4, 0x6df6); A = 0x18;
  CYC(0x6df6, 0x6df9); W8(w1Link_angle) = A;
  CYC(0x6df9, 0x6dfb); A = 0x53;
  CYC(0x6dfb, 0x6dfe); playSound_b00_hook(gb); return;

substate7:
  CALL_C(0x6dfe, itemDecCounter1_hook, 0x23d6, 0x6e01);
  if (!(F & FZ)) { CYCT(0x6e01, 0x6e02); ret_effect(gb); return; }
  CYC(0x6e01, 0x6e02);
  CYC(0x6e02, 0x6e04); L = 0x05;
  CYC(0x6e04, 0x6e05); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6e05, 0x6e07); L = 0x06;
  CYC(0x6e07, 0x6e09); mem_wr(gb, HL, 0x14);
  CYC(0x6e09, 0x6e0a); alu_xor(gb, A);
  CYC(0x6e0a, 0x6e0d); SET_HL(w1Link_visible);
  CYC(0x6e0d, 0x6e0e); mem_wr(gb, HL, A);
  CYC(0x6e0e, 0x6e0f); A = alu_inc8(gb, A);
  CYC(0x6e0f, 0x6e12); W8(wDisabledObjects) = A;
  CYC(0x6e12, 0x6e13); ret_effect(gb); return;

substate8:
  CALL_C(0x6e13, itemDecCounter1_hook, 0x23d6, 0x6e16);
  if (!(F & FZ)) { CYCT(0x6e16, 0x6e17); ret_effect(gb); return; }
  CYC(0x6e16, 0x6e17);
  CYC(0x6e17, 0x6e19); L = 0x05;
  CYC(0x6e19, 0x6e1a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6e1a, 0x6e1c); L = 0x09;
  CYC(0x6e1c, 0x6e1e); mem_wr(gb, HL, 0x18);

jump:
  CYC(0x6e1e, 0x6e20); A = 0x1c;
  CALL_C(0x6e20, specialObjectSetAnimation_hook, 0x2b0a, 0x6e23);
  CYC(0x6e23, 0x6e26); SET_BC(0xfe00);
  CYC(0x6e26, 0x6e29); objectSetSpeedZ_hook(gb); return;

substate9:
  CALL_C(0x6e29, objectApplySpeed_hook, 0x201d, 0x6e2c);
  CYC(0x6e2c, 0x6e2e); E = 0x0d;
  CYC(0x6e2e, 0x6e2f); A = mem_rd(gb, DE);
  CYC(0x6e2f, 0x6e31); alu_sub(gb, 0x10);
  CYC(0x6e31, 0x6e32); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x6e32, 0x6e34); goto substate9_update_z; }
  CYC(0x6e32, 0x6e34);
  CYC(0x6e34, 0x6e37); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x6e37, 0x6e39); mem_wr(gb, HL, 0x01);
  CYC(0x6e39, 0x6e3a); ret_effect(gb); return;

substate9_update_z:
  CYC(0x6e3a, 0x6e3c); C = 0x40;
  CALL_C(0x6e3c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6e3f);
  if (!(F & FZ)) { CYCT(0x6e3f, 0x6e40); ret_effect(gb); return; }
  CYC(0x6e3f, 0x6e40);
  CALL_C(0x6e40, itemIncSubstate_hook, 0x23ef, 0x6e43);
  CYC(0x6e43, 0x6e45); L = 0x06;
  CYC(0x6e45, 0x6e47); mem_wr(gb, HL, 0x08);
  CYC(0x6e47, 0x6e4a); specialObjectAnimate_hook(gb); return;

substate_a:
  CALL_C(0x6e4a, itemDecCounter1_hook, 0x23d6, 0x6e4d);
  if (!(F & FZ)) { CYCT(0x6e4d, 0x6e4e); ret_effect(gb); return; }
  CYC(0x6e4d, 0x6e4e);
  CYC(0x6e4e, 0x6e50); L = 0x05;
  CYC(0x6e50, 0x6e51); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6e51, 0x6e54); goto jump;
}

void specialObjectCode_mooshCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6e54, 0x6e56); E = 0x04;
  CYC(0x6e56, 0x6e57); A = mem_rd(gb, DE);
  CYC(0x6e57, 0x6e58); push_effect(gb, 0x6e58);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6e5c: break;
    case 0x6e86: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x6e5c, companionCutsceneInitOam_hook, 0x6d1b, 0x6e5f);
  CYC(0x6e5f, 0x6e60); H = D;
  CYC(0x6e60, 0x6e62); L = 0x06;
  CYC(0x6e62, 0x6e64); mem_wr(gb, HL, 0x5a);
  CYC(0x6e64, 0x6e66); L = 0x10;
  CYC(0x6e66, 0x6e68); mem_wr(gb, HL, 0x37);
  CYC(0x6e68, 0x6e6a); L = 0x36;
  CYC(0x6e6a, 0x6e6c); mem_wr(gb, HL, 0x05);
  CYC(0x6e6c, 0x6e6e); L = 0x09;
  CYC(0x6e6e, 0x6e70); mem_wr(gb, HL, 0x10);
  CYC(0x6e70, 0x6e72); L = 0x0e;
  CYC(0x6e72, 0x6e74); mem_wr(gb, HL, 0xff);
  CYC(0x6e74, 0x6e75); L = alu_inc8(gb, L);
  CYC(0x6e75, 0x6e77); mem_wr(gb, HL, 0xe0);
  CALL_C(0x6e77, getFreeInteractionSlot_hook, 0x3aef, 0x6e7a);
  if (!(F & FZ)) {
    CYCT(0x6e7a, 0x6e7c);
  } else {
    CYC(0x6e7a, 0x6e7c);
    CYC(0x6e7c, 0x6e7e); mem_wr(gb, HL, 0xc0);
    CYC(0x6e7e, 0x6e80); L = 0x57;
    CYC(0x6e80, 0x6e81); mem_wr(gb, HL, D);
  }
  CYC(0x6e81, 0x6e83); A = 0x07;
  CYC(0x6e83, 0x6e86); specialObjectSetAnimation_hook(gb); return;

state1:
  CYC(0x6e86, 0x6e88); E = 0x05;
  CYC(0x6e88, 0x6e89); A = mem_rd(gb, DE);
  CYC(0x6e89, 0x6e8a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6e8a, 0x6e8c);
  } else {
    CYC(0x6e8a, 0x6e8c);
    CALL_C(0x6e8c, specialObjectAnimate_hook, 0x2aef, 0x6e8f);
    CALL_C(0x6e8f, objectApplySpeed_hook, 0x201d, 0x6e92);
  }
  CYC(0x6e92, 0x6e94); E = 0x05;
  CYC(0x6e94, 0x6e95); A = mem_rd(gb, DE);
  CYC(0x6e95, 0x6e96); push_effect(gb, 0x6e96);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6ea0: goto substate0;
    case 0x6eaa: goto substate1;
    case 0x6eb6: goto substate2;
    case 0x6ece: goto substate3;
    case 0x6ede: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x6ea0, itemDecCounter1_hook, 0x23d6, 0x6ea3);
  if (!(F & FZ)) { CYCT(0x6ea3, 0x6ea4); ret_effect(gb); return; }
  CYC(0x6ea3, 0x6ea4);
  CYC(0x6ea4, 0x6ea6); mem_wr(gb, HL, 0x48);
  CYC(0x6ea6, 0x6ea8); L = 0x05;
  CYC(0x6ea8, 0x6ea9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ea9, 0x6eaa); ret_effect(gb); return;

substate1:
  CALL_C(0x6eaa, itemDecCounter1_hook, 0x23d6, 0x6ead);
  if (!(F & FZ)) { CYCT(0x6ead, 0x6eae); ret_effect(gb); return; }
  CYC(0x6ead, 0x6eae);
  CYC(0x6eae, 0x6eb0); mem_wr(gb, HL, 0x06);
  CYC(0x6eb0, 0x6eb2); L = 0x05;
  CYC(0x6eb2, 0x6eb3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6eb3, 0x6eb6); companionCutsceneFunc_7081_hook(gb); return;

substate2:
  CYC(0x6eb6, 0x6eb7); H = D;
  CYC(0x6eb7, 0x6eb9); L = 0x09;
  CYC(0x6eb9, 0x6eba); A = mem_rd(gb, HL);
  CYC(0x6eba, 0x6ebc); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(0x6ebc, 0x6ebe); goto turn; }
  CYC(0x6ebc, 0x6ebe);
  CYC(0x6ebe, 0x6ec0); L = 0x05;
  CYC(0x6ec0, 0x6ec1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ec1, 0x6ec2); ret_effect(gb); return;

turn:
  CYC(0x6ec2, 0x6ec4); L = 0x06;
  CYC(0x6ec4, 0x6ec5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x6ec5, 0x6ec6); ret_effect(gb); return; }
  CYC(0x6ec5, 0x6ec6);
  CALL_C(0x6ec6, companionCutsceneDecAngle_hook, 0x7098, 0x6ec9);
  CYC(0x6ec9, 0x6ecb); mem_wr(gb, HL, 0x06);
  CYC(0x6ecb, 0x6ece); companionCutsceneFunc_7081_hook(gb); return;

substate3:
  CYC(0x6ece, 0x6ecf); H = D;
  CYC(0x6ecf, 0x6ed1); L = 0x09;
  CYC(0x6ed1, 0x6ed2); A = mem_rd(gb, HL);
  CYC(0x6ed2, 0x6ed4); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x6ed4, 0x6ed6); goto turn; }
  CYC(0x6ed4, 0x6ed6);
  CYC(0x6ed6, 0x6ed8); L = 0x05;
  CYC(0x6ed8, 0x6ed9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6ed9, 0x6edb); A = 0x07;
  CYC(0x6edb, 0x6ede); specialObjectSetAnimation_hook(gb); return;

substate4:
  CYC(0x6ede, 0x6ee0); E = 0x0b;
  CYC(0x6ee0, 0x6ee1); A = mem_rd(gb, DE);
  CYC(0x6ee1, 0x6ee3); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(0x6ee3, 0x6ee4); ret_effect(gb); return; }
  CYC(0x6ee3, 0x6ee4);
  CYC(0x6ee4, 0x6ee7); SET_HL(w1Companion_id);
  CYC(0x6ee7, 0x6ee9); B = 0x3f;
  CALL_C(0x6ee9, clearMemory_hook, 0x046f, 0x6eec);
  CYC(0x6eec, 0x6eef); SET_HL(w1Companion_id);
  CYC(0x6eef, 0x6ef1); mem_wr(gb, HL, 0x10);
  CYC(0x6ef1, 0x6ef3); L = 0x0b;
  CYC(0x6ef3, 0x6ef5); mem_wr(gb, HL, 0xe8);
  CYC(0x6ef5, 0x6ef6); L = alu_inc8(gb, L);
  CYC(0x6ef6, 0x6ef7); L = alu_inc8(gb, L);
  CYC(0x6ef7, 0x6ef9); mem_wr(gb, HL, 0x28);
  CYC(0x6ef9, 0x6efa); ret_effect(gb);
}

void specialObjectCode_dimitriCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6efa, 0x6efc); E = 0x04;
  CYC(0x6efc, 0x6efd); A = mem_rd(gb, DE);
  CYC(0x6efd, 0x6efe); push_effect(gb, 0x6efe);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6f02: break;
    case 0x6f16: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x6f02, companionCutsceneInitOam_hook, 0x6d1b, 0x6f05);
  CYC(0x6f05, 0x6f06); H = D;
  CYC(0x6f06, 0x6f08); L = 0x10;
  CYC(0x6f08, 0x6f0a); mem_wr(gb, HL, 0x28);
  CYC(0x6f0a, 0x6f0c); L = 0x0e;
  CYC(0x6f0c, 0x6f0e); mem_wr(gb, HL, 0xe0);
  CYC(0x6f0e, 0x6f0f); L = alu_inc8(gb, L);
  CYC(0x6f0f, 0x6f11); mem_wr(gb, HL, 0xff);
  CYC(0x6f11, 0x6f13); A = 0x19;
  CYC(0x6f13, 0x6f16); specialObjectSetAnimation_hook(gb); return;

state1:
  CYC(0x6f16, 0x6f18); E = 0x05;
  CYC(0x6f18, 0x6f19); A = mem_rd(gb, DE);
  CYC(0x6f19, 0x6f1a); push_effect(gb, 0x6f1a);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6f28: goto substate0;
    case 0x6f54: goto substate1;
    case 0x6f77: goto substate2;
    case 0x6f87: goto substate3;
    case 0x6f93: goto substate4;
    case 0x6faa: goto substate5;
    case 0x6fb6: goto substate6;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x6f28, 0x6f29); H = D;
  CYC(0x6f29, 0x6f2b); L = 0x05;
  CYC(0x6f2b, 0x6f2c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f2c, 0x6f2e); L = 0x07;
  CYC(0x6f2e, 0x6f2f); A = mem_rd(gb, HL);
  CYC(0x6f2f, 0x6f31); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x6f31, 0x6f33);
  } else {
    CYC(0x6f31, 0x6f33);
    CYC(0x6f33, 0x6f34); push_effect(gb, AF);
    CYC(0x6f34, 0x6f36); A = 0x1a;
    CALL_C(0x6f36, specialObjectSetAnimation_hook, 0x2b0a, 0x6f39);
    CYC(0x6f39, 0x6f3a); SET_AF(pop_effect(gb));
  }
  CYC(0x6f3a, 0x6f3b); B = A;
  CYC(0x6f3b, 0x6f3c); alu_add(gb, A);
  CYC(0x6f3c, 0x6f3d); alu_add(gb, B);
  CYC(0x6f3d, 0x6f40); SET_HL(dimitriCutsceneData_bank06);
  CYC(0x6f40, 0x6f41); push_effect(gb, 0x6f41); companion_cutscene_add_a_to_hl(gb);
  CYC(0x6f41, 0x6f42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6f42, 0x6f44); E = 0x09;
  CYC(0x6f44, 0x6f45); mem_wr(gb, DE, A);
  CYC(0x6f45, 0x6f46); C = mem_rd(gb, HL);
  CYC(0x6f46, 0x6f47); SET_HL(HL + 1);
  CYC(0x6f47, 0x6f48); B = mem_rd(gb, HL);
  CYC(0x6f48, 0x6f4b); objectSetSpeedZ_hook(gb); return;

substate1:
  CALL_C(0x6f54, specialObjectAnimate_hook, 0x2aef, 0x6f57);
  CALL_C(0x6f57, objectApplySpeed_hook, 0x201d, 0x6f5a);
  CYC(0x6f5a, 0x6f5c); C = 0x18;
  CALL_C(0x6f5c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6f5f);
  if (!(F & FZ)) { CYCT(0x6f5f, 0x6f60); ret_effect(gb); return; }
  CYC(0x6f5f, 0x6f60);
  CYC(0x6f60, 0x6f61); H = D;
  CYC(0x6f61, 0x6f63); L = 0x07;
  CYC(0x6f63, 0x6f64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f64, 0x6f65); A = mem_rd(gb, HL);
  CYC(0x6f65, 0x6f67); L = 0x05;
  CYC(0x6f67, 0x6f69); alu_cp(gb, 0x03);
  if (F & FZ) {
    CYCT(0x6f69, 0x6f6b);
    CYC(0x6f71, 0x6f72); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x6f72, 0x6f74); L = 0x06;
    CYC(0x6f74, 0x6f76); mem_wr(gb, HL, 0x06);
    CYC(0x6f76, 0x6f77); ret_effect(gb); return;
  }
  CYC(0x6f69, 0x6f6b);
  CYC(0x6f6b, 0x6f6c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6f6c, 0x6f6e); L = 0x06;
  CYC(0x6f6e, 0x6f70); mem_wr(gb, HL, 0x08);
  CYC(0x6f70, 0x6f71); ret_effect(gb); return;

substate2:
  CALL_C(0x6f77, itemDecCounter1_hook, 0x23d6, 0x6f7a);
  if (!(F & FZ)) { CYCT(0x6f7a, 0x6f7b); ret_effect(gb); return; }
  CYC(0x6f7a, 0x6f7b);
  CYC(0x6f7b, 0x6f7d); L = 0x05;
  CYC(0x6f7d, 0x6f7e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f7e, 0x6f80); L = 0x06;
  CYC(0x6f80, 0x6f82); mem_wr(gb, HL, 0x14);
  CYC(0x6f82, 0x6f84); A = 0x27;
  CYC(0x6f84, 0x6f87); specialObjectSetAnimation_hook(gb); return;

substate3:
  CALL_C(0x6f87, itemDecCounter1_hook, 0x23d6, 0x6f8a);
  if (!(F & FZ)) { CYCT(0x6f8a, 0x6f8b); ret_effect(gb); return; }
  CYC(0x6f8a, 0x6f8b);
  CYC(0x6f8b, 0x6f8d); L = 0x05;
  CYC(0x6f8d, 0x6f8e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f8e, 0x6f90); L = 0x06;
  CYC(0x6f90, 0x6f92); mem_wr(gb, HL, 0x78);
  CYC(0x6f92, 0x6f93); ret_effect(gb); return;

substate4:
  CALL_C(0x6f93, specialObjectAnimate_hook, 0x2aef, 0x6f96);
  CALL_C(0x6f96, itemDecCounter1_hook, 0x23d6, 0x6f99);
  if (!(F & FZ)) { CYCT(0x6f99, 0x6f9a); ret_effect(gb); return; }
  CYC(0x6f99, 0x6f9a);
  CYC(0x6f9a, 0x6f9c); L = 0x05;
  CYC(0x6f9c, 0x6f9d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6f9d, 0x6f9f); L = 0x06;
  CYC(0x6f9f, 0x6fa1); mem_wr(gb, HL, 0x3c);
  CYC(0x6fa1, 0x6fa3); L = 0x09;
  CYC(0x6fa3, 0x6fa5); mem_wr(gb, HL, 0x0b);
  CYC(0x6fa5, 0x6fa7); L = 0x10;
  CYC(0x6fa7, 0x6fa9); mem_wr(gb, HL, 0x14);
  CYC(0x6fa9, 0x6faa); ret_effect(gb); return;

substate5:
  CALL_C(0x6faa, itemDecCounter1_hook, 0x23d6, 0x6fad);
  if (!(F & FZ)) { CYCT(0x6fad, 0x6fae); ret_effect(gb); return; }
  CYC(0x6fad, 0x6fae);
  CYC(0x6fae, 0x6fb0); L = 0x05;
  CYC(0x6fb0, 0x6fb1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6fb1, 0x6fb3); A = 0x26;
  CYC(0x6fb3, 0x6fb6); specialObjectSetAnimation_hook(gb); return;

substate6:
  CALL_C(0x6fb6, specialObjectAnimate_hook, 0x2aef, 0x6fb9);
  CALL_C(0x6fb9, objectApplySpeed_hook, 0x201d, 0x6fbc);
  CYC(0x6fbc, 0x6fbe); E = 0x0d;
  CYC(0x6fbe, 0x6fbf); A = mem_rd(gb, DE);
  CYC(0x6fbf, 0x6fc1); alu_cp(gb, 0x78);
  if (F & FZ) {
    CYC(0x6fc1, 0x6fc3);
    CYC(0x6fc3, 0x6fc5); A = 0x05;
    CYC(0x6fc5, 0x6fc8); specialObjectSetAnimation_hook(gb); return;
  }
  CYCT(0x6fc1, 0x6fc3);
  CYC(0x6fc8, 0x6fca); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(0x6fca, 0x6fcb); ret_effect(gb); return; }
  CYC(0x6fca, 0x6fcb);
  CYC(0x6fcb, 0x6fce); SET_HL(w1Companion_id);
  CYC(0x6fce, 0x6fd0); B = 0x3f;
  CALL_C(0x6fd0, clearMemory_hook, 0x046f, 0x6fd3);
  CYC(0x6fd3, 0x6fd6); SET_HL(w1Companion_id);
  CYC(0x6fd6, 0x6fd8); mem_wr(gb, HL, 0x0f);
  CYC(0x6fd8, 0x6fd9); L = alu_inc8(gb, L);
  CYC(0x6fd9, 0x6fdb); mem_wr(gb, HL, 0x01);
  CYC(0x6fdb, 0x6fdd); L = 0x0b;
  CYC(0x6fdd, 0x6fdf); mem_wr(gb, HL, 0x48);
  CYC(0x6fdf, 0x6fe0); L = alu_inc8(gb, L);
  CYC(0x6fe0, 0x6fe1); L = alu_inc8(gb, L);
  CYC(0x6fe1, 0x6fe3); mem_wr(gb, HL, 0xd8);
  CYC(0x6fe3, 0x6fe4); ret_effect(gb);
}

void specialObjectCode_mapleCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6fe4, 0x6fe6); E = 0x04;
  CYC(0x6fe6, 0x6fe7); A = mem_rd(gb, DE);
  CYC(0x6fe7, 0x6fe8); push_effect(gb, 0x6fe8);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x6fec: break;
    case 0x7025: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x6fec, companionCutsceneInitOam_hook, 0x6d1b, 0x6fef);
  CYC(0x6fef, 0x6ff0); H = D;
  CYC(0x6ff0, 0x6ff2); L = 0x0f;
  CYC(0x6ff2, 0x6ff4); mem_wr(gb, HL, 0xf0);
  CYC(0x6ff4, 0x6ff6); L = 0x09;
  CYC(0x6ff6, 0x6ff8); mem_wr(gb, HL, 0x08);
  CYC(0x6ff8, 0x6ffa); L = 0x06;
  CYC(0x6ffa, 0x6ffc); mem_wr(gb, HL, 0x5a);
  CYC(0x6ffc, 0x6ffd); ret_effect(gb); return;

init_position:
  CYC(0x6ffd, 0x6fff); L = 0x07;
  CYC(0x6fff, 0x7000); A = mem_rd(gb, HL);
  CYC(0x7000, 0x7001); alu_add(gb, A);
  CYC(0x7001, 0x7004); SET_HL(mapleCutsceneData_bank06);
  CYC(0x7004, 0x7005); companion_cutscene_add_double_index(gb, 0x7005);
  CYC(0x7005, 0x7006); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7006, 0x7008); E = 0x10;
  CYC(0x7008, 0x7009); mem_wr(gb, DE, A);
  CYC(0x7009, 0x700a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x700a, 0x700c); E = 0x06;
  CYC(0x700c, 0x700d); mem_wr(gb, DE, A);
  CYC(0x700d, 0x700e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x700e, 0x7010); E = 0x0b;
  CYC(0x7010, 0x7011); mem_wr(gb, DE, A);
  CYC(0x7011, 0x7012); A = mem_rd(gb, HL);
  CYC(0x7012, 0x7015); specialObjectSetAnimation_hook(gb); return;

state1:
  CALL_C(0x7025, specialObjectAnimate_hook, 0x2aef, 0x7028);
  CALL_C(0x7028, objectOscillateZ_hook, 0x27a0, 0x702b);
  CYC(0x702b, 0x702d); E = 0x05;
  CYC(0x702d, 0x702e); A = mem_rd(gb, DE);
  CYC(0x702e, 0x702f); push_effect(gb, 0x702f);
  switch (companion_cutscene_jump_table(gb)) {
    case 0x7037: goto substate0;
    case 0x7044: goto substate1;
    case 0x7051: goto substate2;
    case 0x7071: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x7037, 0x703a); A = W8(wPaletteThread_mode);
  CYC(0x703a, 0x703b); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(0x703b, itemDecCounter1_hook, 0x23d6, 0x703e);
  else CYC(0x703b, 0x703e);
  if (!(F & FZ)) { CYCT(0x703e, 0x703f); ret_effect(gb); return; }
  CYC(0x703e, 0x703f);
  CALL_C(0x703f, itemIncSubstate_hook, 0x23ef, 0x7042);
  CYC(0x7042, 0x7044); goto init_position;

substate1:
  CALL_C(0x7044, itemDecCounter1_hook, 0x23d6, 0x7047);
  if (!(F & FZ)) { CYCT(0x7047, 0x704a); objectApplySpeed_hook(gb); return; }
  CYC(0x7047, 0x704a);
  CYC(0x704a, 0x704c); mem_wr(gb, HL, 0x5a);
  CYC(0x704c, 0x704d); L = alu_inc8(gb, L);
  CYC(0x704d, 0x704e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x704e, 0x7051); itemIncSubstate_hook(gb); return;

substate2:
  CALL_C(0x7051, itemDecCounter1_hook, 0x23d6, 0x7054);
  if (!(F & FZ)) { CYCT(0x7054, 0x7055); ret_effect(gb); return; }
  CYC(0x7054, 0x7055);
  CYC(0x7055, 0x7056); L = alu_inc8(gb, L);
  CYC(0x7056, 0x7057); A = mem_rd(gb, HL);
  CYC(0x7057, 0x7059); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x7059, 0x705b);
    CYC(0x7066, 0x7068); L = 0x05;
    CYC(0x7068, 0x7069); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(0x7069, 0x706b); L = 0x09;
    CYC(0x706b, 0x706c); A = mem_rd(gb, HL);
    CYC(0x706c, 0x706e); alu_xor(gb, 0x10);
    CYC(0x706e, 0x706f); mem_wr(gb, HL, A);
    CYC(0x706f, 0x7071); goto init_position;
  }
  CYC(0x7059, 0x705b);
  CYC(0x705b, 0x705c); L = alu_dec8(gb, L);
  CYC(0x705c, 0x705e); mem_wr(gb, HL, 0x1e);
  CALL_C(0x705e, itemIncSubstate_hook, 0x23ef, 0x7061);
  CYC(0x7061, 0x7063); A = 0x07;
  CYC(0x7063, 0x7066); specialObjectSetAnimation_hook(gb); return;

substate3:
  CALL_C(0x7071, itemDecCounter1_hook, 0x23d6, 0x7074);
  if (F & FZ) {
    CYCT(0x7074, 0x7076);
    CYC(0x707b, 0x707d); A = 0xff;
    CYC(0x707d, 0x7080); W8(wTmpcfc0_genericCutscene_cfdf) = A;
    CYC(0x7080, 0x7081); ret_effect(gb); return;
  }
  CYC(0x7074, 0x7076);
  CYC(0x7076, 0x7078); C = 0x02;
  CYC(0x7078, 0x707b); objectUpdateSpeedZ_paramC_hook(gb);
}
