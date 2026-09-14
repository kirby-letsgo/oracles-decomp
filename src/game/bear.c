#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t bank09_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

void interactionCode5d_hook(GB *gb);
void bear_state0_hook(GB *gb);
void bear_state0__afterCall6d4b_hook(GB *gb);
void bear_state0__initSubid_hook(GB *gb);
void bear_state0__initSubid00_hook(GB *gb);
void bear_state0__initSubid01_hook(GB *gb);
void bear_state0__initSubid02_hook(GB *gb);
void bear_state0__var03IsNonzero_hook(GB *gb);
void bear_state0__chooseTextID_hook(GB *gb);
void bear_state0__textIDs_hook(GB *gb);
void bear_state1_hook(GB *gb);
void bear_state1__runSubid00_hook(GB *gb);
void bear_state1__substate0_hook(GB *gb);
void bear_state1__substate1_hook(GB *gb);
void bear_state1__substate2_hook(GB *gb);
void bear_state1__substate3_hook(GB *gb);
void bear_state1__substate4_hook(GB *gb);
void bear_state1__runSubid02_hook(GB *gb);

void interactionCode5d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d3d, 0x6d3f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6d3f, 0x6d40); A = mem_rd(gb, DE);
  CYC(0x6d40, 0x6d41); push_effect(gb, 0x6d41); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x6d45: bear_state0_hook(gb); return;
    case 0x6dcb: bear_state1_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void bear_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d45, 0x6d47); A = 0x01;
  CYC(0x6d47, 0x6d48); mem_wr(gb, DE, A);
  CALL_C(0x6d48, interactionInitGraphics_hook, 0x15fb, 0x6d4b);
L_6d4b:
  CALL_C(0x6d4b, objectSetVisiblec2_hook, 0x1e45, 0x6d4e);
  CALL_C(0x6d4e, bear_state0__initSubid_hook, 0x6d59, 0x6d51);
  CYC(0x6d51, 0x6d53); E = 0x40;
  CYC(0x6d53, 0x6d54); A = mem_rd(gb, DE);
  CYC(0x6d54, 0x6d55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d55, 0x6d58); if (hook_enabled_at(0x24f0)) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(0x24f0); } CYC(0x6d55, 0x6d58);
  CYC(0x6d58, 0x6d59); ret_effect(gb); return;
}

void bear_state0__afterCall6d4b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d4b:
  CALL_C(0x6d4b, objectSetVisiblec2_hook, 0x1e45, 0x6d4e);
  CALL_C(0x6d4e, bear_state0__initSubid_hook, 0x6d59, 0x6d51);
  CYC(0x6d51, 0x6d53); E = 0x40;
  CYC(0x6d53, 0x6d54); A = mem_rd(gb, DE);
  CYC(0x6d54, 0x6d55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d55, 0x6d58); if (hook_enabled_at(0x24f0)) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(0x24f0); } CYC(0x6d55, 0x6d58);
  CYC(0x6d58, 0x6d59); ret_effect(gb); return;
}

void bear_state0__initSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d59:
  CYC(0x6d59, 0x6d5b); E = 0x42;
  CYC(0x6d5b, 0x6d5c); A = mem_rd(gb, DE);
  CYC(0x6d5c, 0x6d5d); push_effect(gb, 0x6d5d); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x6d63: goto L_6d63; case 0x6d76: goto L_6d76; case 0x6d77: goto L_6d77; default: HANDOFF(HL); }
L_6d63:
  CALL_C(0x6d63, getThisRoomFlags_hook, 0x197d, 0x6d66);
  CYC(0x6d66, 0x6d68); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6d68, 0x6d6a); goto L_6d70; } CYC(0x6d68, 0x6d6a);
  CYC(0x6d6a, 0x6d6c); E = 0x4b;
  CYC(0x6d6c, 0x6d6d); A = mem_rd(gb, DE);
  CYC(0x6d6d, 0x6d6f); alu_add(gb, 0x10);
  CYC(0x6d6f, 0x6d70); mem_wr(gb, DE, A);
L_6d70:
  CYC(0x6d70, 0x6d73); SET_HL(0x6724);
  CYC(0x6d73, 0x6d76); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
L_6d76:
  CYC(0x6d76, 0x6d77); ret_effect(gb); return;
L_6d77:
  CYC(0x6d77, 0x6d79); E = 0x43;
  CYC(0x6d79, 0x6d7a); A = mem_rd(gb, DE);
  CYC(0x6d7a, 0x6d7b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d7b, 0x6d7d); goto L_6da8; } CYC(0x6d7b, 0x6d7d);
  CYC(0x6d7d, 0x6d7f); A = 0x0a;
  CALL_C(0x6d7f, checkGlobalFlag_hook, 0x31f3, 0x6d82);
  if ((F & FZ)) { CYCT(0x6d82, 0x6d85); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6d82, 0x6d85);
  CYC(0x6d85, 0x6d88); SET_HL(0x7804);
  CALL_C(0x6d88, parseGivenObjectData_b00_hook, 0x3171, 0x6d8b);
  CYC(0x6d8b, 0x6d8d); A = 0x14;
  CALL_C(0x6d8d, checkGlobalFlag_hook, 0x31f3, 0x6d90);
  if (!(F & FZ)) { CYCT(0x6d90, 0x6d93); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6d90, 0x6d93);
  CYC(0x6d93, 0x6d95); A = 0x12;
  CALL_C(0x6d95, checkGlobalFlag_hook, 0x31f3, 0x6d98);
  if ((F & FZ)) { CYCT(0x6d98, 0x6d9b); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6d98, 0x6d9b);
  CYC(0x6d9b, 0x6d9d); A = 0x11;
  CALL_C(0x6d9d, checkGlobalFlag_hook, 0x31f3, 0x6da0);
  CYC(0x6da0, 0x6da2); A = 0x00;
  if ((F & FZ)) { CYCT(0x6da2, 0x6da5); goto L_6da6; } CYC(0x6da2, 0x6da5);
  CYC(0x6da5, 0x6da6); A = alu_inc8(gb, A);
L_6da6:
  CYC(0x6da6, 0x6da8); goto L_6db2;
L_6da8:
  CYC(0x6da8, 0x6daa); A = 0x14;
  CALL_C(0x6daa, checkGlobalFlag_hook, 0x31f3, 0x6dad);
  if ((F & FZ)) { CYCT(0x6dad, 0x6db0); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6dad, 0x6db0);
  CYC(0x6db0, 0x6db2); A = 0x02;
L_6db2:
  CALL_C(0x6db2, bear_state0__chooseTextID_hook, 0x6dbb, 0x6db5);
  CYC(0x6db5, 0x6db8); SET_HL(0x675a);
  CYC(0x6db8, 0x6dbb); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
}

void bear_state0__initSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d63:
  CALL_C(0x6d63, getThisRoomFlags_hook, 0x197d, 0x6d66);
  CYC(0x6d66, 0x6d68); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x6d68, 0x6d6a); goto L_6d70; } CYC(0x6d68, 0x6d6a);
  CYC(0x6d6a, 0x6d6c); E = 0x4b;
  CYC(0x6d6c, 0x6d6d); A = mem_rd(gb, DE);
  CYC(0x6d6d, 0x6d6f); alu_add(gb, 0x10);
  CYC(0x6d6f, 0x6d70); mem_wr(gb, DE, A);
L_6d70:
  CYC(0x6d70, 0x6d73); SET_HL(0x6724);
  CYC(0x6d73, 0x6d76); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
}

void bear_state0__initSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d76:
  CYC(0x6d76, 0x6d77); ret_effect(gb); return;
}

void bear_state0__initSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d77:
  CYC(0x6d77, 0x6d79); E = 0x43;
  CYC(0x6d79, 0x6d7a); A = mem_rd(gb, DE);
  CYC(0x6d7a, 0x6d7b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d7b, 0x6d7d); goto L_6da8; } CYC(0x6d7b, 0x6d7d);
  CYC(0x6d7d, 0x6d7f); A = 0x0a;
  CALL_C(0x6d7f, checkGlobalFlag_hook, 0x31f3, 0x6d82);
  if ((F & FZ)) { CYCT(0x6d82, 0x6d85); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6d82, 0x6d85);
  CYC(0x6d85, 0x6d88); SET_HL(0x7804);
  CALL_C(0x6d88, parseGivenObjectData_b00_hook, 0x3171, 0x6d8b);
  CYC(0x6d8b, 0x6d8d); A = 0x14;
  CALL_C(0x6d8d, checkGlobalFlag_hook, 0x31f3, 0x6d90);
  if (!(F & FZ)) { CYCT(0x6d90, 0x6d93); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6d90, 0x6d93);
  CYC(0x6d93, 0x6d95); A = 0x12;
  CALL_C(0x6d95, checkGlobalFlag_hook, 0x31f3, 0x6d98);
  if ((F & FZ)) { CYCT(0x6d98, 0x6d9b); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6d98, 0x6d9b);
  CYC(0x6d9b, 0x6d9d); A = 0x11;
  CALL_C(0x6d9d, checkGlobalFlag_hook, 0x31f3, 0x6da0);
  CYC(0x6da0, 0x6da2); A = 0x00;
  if ((F & FZ)) { CYCT(0x6da2, 0x6da5); goto L_6da6; } CYC(0x6da2, 0x6da5);
  CYC(0x6da5, 0x6da6); A = alu_inc8(gb, A);
L_6da6:
  CYC(0x6da6, 0x6da8); goto L_6db2;
L_6da8:
  CYC(0x6da8, 0x6daa); A = 0x14;
  CALL_C(0x6daa, checkGlobalFlag_hook, 0x31f3, 0x6dad);
  if ((F & FZ)) { CYCT(0x6dad, 0x6db0); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6dad, 0x6db0);
  CYC(0x6db0, 0x6db2); A = 0x02;
L_6db2:
  CALL_C(0x6db2, bear_state0__chooseTextID_hook, 0x6dbb, 0x6db5);
  CYC(0x6db5, 0x6db8); SET_HL(0x675a);
  CYC(0x6db8, 0x6dbb); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
}

void bear_state0__var03IsNonzero_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6da8:
  CYC(0x6da8, 0x6daa); A = 0x14;
  CALL_C(0x6daa, checkGlobalFlag_hook, 0x31f3, 0x6dad);
  if ((F & FZ)) { CYCT(0x6dad, 0x6db0); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6dad, 0x6db0);
  CYC(0x6db0, 0x6db2); A = 0x02;
  CALL_C(0x6db2, bear_state0__chooseTextID_hook, 0x6dbb, 0x6db5);
  CYC(0x6db5, 0x6db8); SET_HL(0x675a);
  CYC(0x6db8, 0x6dbb); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
}

void bear_state0__chooseTextID_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6dbb:
  CYC(0x6dbb, 0x6dbe); SET_HL(0x6dc8);
  CYC(0x6dbe, 0x6dbf); push_effect(gb, 0x6dbf);
  CYC(0x0010, 0x0011); alu_add(gb, L); CYC(0x0011, 0x0012); L = A;
  if (!(F & FC)) { CYCT(0x0012, 0x0013); ret_effect(gb); }
  else { CYC(0x0012, 0x0013); CYC(0x0013, 0x0014); H = alu_inc8(gb, H); CYC(0x0014, 0x0015); ret_effect(gb); }
  CYC(0x6dbf, 0x6dc0); A = mem_rd(gb, HL);
  CYC(0x6dc0, 0x6dc2); E = 0x72;
  CYC(0x6dc2, 0x6dc3); mem_wr(gb, DE, A);
  CYC(0x6dc3, 0x6dc5); A = 0x57;
  CYC(0x6dc5, 0x6dc6); E = alu_inc8(gb, E);
  CYC(0x6dc6, 0x6dc7); mem_wr(gb, DE, A);
  CYC(0x6dc7, 0x6dc8); ret_effect(gb); return;
}

void bear_state0__textIDs_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6dc8:
  CYC(0x6dc8, 0x6dc9); mem_wr(gb, DE, A);
  CYC(0x6dc9, 0x6dca); SET_DE(DE + 1);
  CYC(0x6dca, 0x6dcb); D = alu_inc8(gb, D);
  bear_state1_hook(gb); return;
}

void bear_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6dcb, 0x6dcd); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6dcd, 0x6dce); A = mem_rd(gb, DE);
  CYC(0x6dce, 0x6dcf); push_effect(gb, 0x6dcf); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x6dd5: bear_state1__runSubid00_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    case 0x6e4c: bear_state1__runSubid02_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void bear_state1__runSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6dd5:
  CALL_C(0x6dd5, interactionAnimateAsNpc_hook, 0x26db, 0x6dd8);
  CYC(0x6dd8, 0x6dda); E = 0x45;
  CYC(0x6dda, 0x6ddb); A = mem_rd(gb, DE);
  CYC(0x6ddb, 0x6ddc); push_effect(gb, 0x6ddc); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x6de6: goto L_6de6; case 0x6e08: goto L_6e08; case 0x6e19: goto L_6e19; case 0x6e2a: goto L_6e2a; case 0x6e40: goto L_6e40; default: HANDOFF(HL); }
L_6de6:
  CALL_C(0x6de6, interactionRunScript_hook, 0x2552, 0x6de9);
  CYC(0x6de9, 0x6dec); SET_HL(0xd00d);
  CYC(0x6dec, 0x6ded); A = mem_rd(gb, HL);
  CYC(0x6ded, 0x6def); alu_cp(gb, 0x60);
  if ((F & FC)) { CYCT(0x6def, 0x6df0); ret_effect(gb); return; } CYC(0x6def, 0x6df0);
  CYC(0x6df0, 0x6df2); L = 0x0b;
  CYC(0x6df2, 0x6df3); A = mem_rd(gb, HL);
  CYC(0x6df3, 0x6df5); alu_cp(gb, 0x3e);
  if (!(F & FC)) { CYCT(0x6df5, 0x6df6); ret_effect(gb); return; } CYC(0x6df5, 0x6df6);
  CYC(0x6df6, 0x6df8); A = 0x08;
  CALL_C(0x6df8, setLinkIDOverride_hook, 0x2acf, 0x6dfb);
  CYC(0x6dfb, 0x6dfd); L = 0x02;
  CYC(0x6dfd, 0x6dff); mem_wr(gb, HL, 0x03);
  CYC(0x6dff, 0x6e02); SET_HL(0x6750);
  CALL_C(0x6e02, interactionSetScript_hook, 0x2544, 0x6e05);
  CALL_C(0x6e05, interactionIncSubstate_hook, 0x23e5, 0x6e08);
L_6e08:
  CALL_C(0x6e08, interactionRunScript_hook, 0x2552, 0x6e0b);
  CYC(0x6e0b, 0x6e0e); A = mem_rd(gb, 0xcfd0);
  CYC(0x6e0e, 0x6e10); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x6e10, 0x6e11); ret_effect(gb); return; } CYC(0x6e10, 0x6e11);
  CALL_C(0x6e11, interactionIncSubstate_hook, 0x23e5, 0x6e14);
  CYC(0x6e14, 0x6e16); A = 0x02;
  CYC(0x6e16, 0x6e19); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
L_6e19:
  CALL_C(0x6e19, interactionAnimate_hook, 0x261b, 0x6e1c);
  CYC(0x6e1c, 0x6e1f); A = mem_rd(gb, 0xcfd0);
  CYC(0x6e1f, 0x6e21); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x6e21, 0x6e22); ret_effect(gb); return; } CYC(0x6e21, 0x6e22);
  CALL_C(0x6e22, interactionIncSubstate_hook, 0x23e5, 0x6e25);
  CYC(0x6e25, 0x6e27); L = 0x46;
  CYC(0x6e27, 0x6e29); mem_wr(gb, HL, 0x28);
  CYC(0x6e29, 0x6e2a); ret_effect(gb); return;
L_6e2a:
  CALL_C(0x6e2a, interactionDecCounter1_hook, 0x23cc, 0x6e2d);
  if (!(F & FZ)) { CYCT(0x6e2d, 0x6e30); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b); } CYC(0x6e2d, 0x6e30);
  CALL_C(0x6e30, interactionIncSubstate_hook, 0x23e5, 0x6e33);
  CYC(0x6e33, 0x6e35); L = 0x49;
  CYC(0x6e35, 0x6e37); mem_wr(gb, HL, 0x02);
  CYC(0x6e37, 0x6e39); L = 0x50;
  CYC(0x6e39, 0x6e3b); mem_wr(gb, HL, 0x28);
  CYC(0x6e3b, 0x6e3d); A = 0x01;
  CYC(0x6e3d, 0x6e40); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
L_6e40:
  CALL_C(0x6e40, objectCheckWithinScreenBoundary_hook, 0x2184, 0x6e43);
  if (!(F & FC)) { CYCT(0x6e43, 0x6e46); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6e43, 0x6e46);
  CALL_C(0x6e46, objectApplySpeed_hook, 0x201d, 0x6e49);
  CYC(0x6e49, 0x6e4c); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
}

void bear_state1__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6de6:
  CALL_C(0x6de6, interactionRunScript_hook, 0x2552, 0x6de9);
  CYC(0x6de9, 0x6dec); SET_HL(0xd00d);
  CYC(0x6dec, 0x6ded); A = mem_rd(gb, HL);
  CYC(0x6ded, 0x6def); alu_cp(gb, 0x60);
  if ((F & FC)) { CYCT(0x6def, 0x6df0); ret_effect(gb); return; } CYC(0x6def, 0x6df0);
  CYC(0x6df0, 0x6df2); L = 0x0b;
  CYC(0x6df2, 0x6df3); A = mem_rd(gb, HL);
  CYC(0x6df3, 0x6df5); alu_cp(gb, 0x3e);
  if (!(F & FC)) { CYCT(0x6df5, 0x6df6); ret_effect(gb); return; } CYC(0x6df5, 0x6df6);
  CYC(0x6df6, 0x6df8); A = 0x08;
  CALL_C(0x6df8, setLinkIDOverride_hook, 0x2acf, 0x6dfb);
  CYC(0x6dfb, 0x6dfd); L = 0x02;
  CYC(0x6dfd, 0x6dff); mem_wr(gb, HL, 0x03);
  CYC(0x6dff, 0x6e02); SET_HL(0x6750);
  CALL_C(0x6e02, interactionSetScript_hook, 0x2544, 0x6e05);
  CALL_C(0x6e05, interactionIncSubstate_hook, 0x23e5, 0x6e08);
L_6e08:
  CALL_C(0x6e08, interactionRunScript_hook, 0x2552, 0x6e0b);
  CYC(0x6e0b, 0x6e0e); A = mem_rd(gb, 0xcfd0);
  CYC(0x6e0e, 0x6e10); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x6e10, 0x6e11); ret_effect(gb); return; } CYC(0x6e10, 0x6e11);
  CALL_C(0x6e11, interactionIncSubstate_hook, 0x23e5, 0x6e14);
  CYC(0x6e14, 0x6e16); A = 0x02;
  CYC(0x6e16, 0x6e19); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
}

void bear_state1__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e08:
  CALL_C(0x6e08, interactionRunScript_hook, 0x2552, 0x6e0b);
  CYC(0x6e0b, 0x6e0e); A = mem_rd(gb, 0xcfd0);
  CYC(0x6e0e, 0x6e10); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x6e10, 0x6e11); ret_effect(gb); return; } CYC(0x6e10, 0x6e11);
  CALL_C(0x6e11, interactionIncSubstate_hook, 0x23e5, 0x6e14);
  CYC(0x6e14, 0x6e16); A = 0x02;
  CYC(0x6e16, 0x6e19); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
}

void bear_state1__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e19:
  CALL_C(0x6e19, interactionAnimate_hook, 0x261b, 0x6e1c);
  CYC(0x6e1c, 0x6e1f); A = mem_rd(gb, 0xcfd0);
  CYC(0x6e1f, 0x6e21); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x6e21, 0x6e22); ret_effect(gb); return; } CYC(0x6e21, 0x6e22);
  CALL_C(0x6e22, interactionIncSubstate_hook, 0x23e5, 0x6e25);
  CYC(0x6e25, 0x6e27); L = 0x46;
  CYC(0x6e27, 0x6e29); mem_wr(gb, HL, 0x28);
  CYC(0x6e29, 0x6e2a); ret_effect(gb); return;
}

void bear_state1__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e2a:
  CALL_C(0x6e2a, interactionDecCounter1_hook, 0x23cc, 0x6e2d);
  if (!(F & FZ)) { CYCT(0x6e2d, 0x6e30); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b); } CYC(0x6e2d, 0x6e30);
  CALL_C(0x6e30, interactionIncSubstate_hook, 0x23e5, 0x6e33);
  CYC(0x6e33, 0x6e35); L = 0x49;
  CYC(0x6e35, 0x6e37); mem_wr(gb, HL, 0x02);
  CYC(0x6e37, 0x6e39); L = 0x50;
  CYC(0x6e39, 0x6e3b); mem_wr(gb, HL, 0x28);
  CYC(0x6e3b, 0x6e3d); A = 0x01;
  CYC(0x6e3d, 0x6e40); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
}

void bear_state1__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e40:
  CALL_C(0x6e40, objectCheckWithinScreenBoundary_hook, 0x2184, 0x6e43);
  if (!(F & FC)) { CYCT(0x6e43, 0x6e46); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6e43, 0x6e46);
  CALL_C(0x6e46, objectApplySpeed_hook, 0x201d, 0x6e49);
  CYC(0x6e49, 0x6e4c); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
}

void bear_state1__runSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e4c:
  CALL_C(0x6e4c, interactionRunScript_hook, 0x2552, 0x6e4f);
  if ((F & FC)) { CYCT(0x6e4f, 0x6e52); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6e4f, 0x6e52);
  CYC(0x6e52, 0x6e55); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}
