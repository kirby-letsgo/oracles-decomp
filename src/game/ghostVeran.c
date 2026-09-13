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

void interactionCode3e_hook(GB *gb);
void interactionCode3e__state0_hook(GB *gb);
void interactionCode3e__afterCall4cbc_hook(GB *gb);
void interactionCode3e__subid0Init_hook(GB *gb);
void interactionCode3e__subid1Init_hook(GB *gb);
void interactionCode3e__subid2Init_hook(GB *gb);
void interactionCode3e__state1_hook(GB *gb);
void runVeranGhostSubid0_hook(GB *gb);
void runVeranGhostSubid0__substate0_hook(GB *gb);
void runVeranGhostSubid0__substate1_hook(GB *gb);
void runVeranGhostSubid0__substate2_hook(GB *gb);
void runVeranGhostSubid0__substate3_hook(GB *gb);
void runVeranGhostSubid0__substate4_hook(GB *gb);
void runVeranGhostSubid0__substate6_hook(GB *gb);
void runVeranGhostSubid0__rumbleAndRandomizeX_hook(GB *gb);
void runVeranGhostSubid0__substate7_hook(GB *gb);
void runVeranGhostSubid0__substate8_hook(GB *gb);
void runVeranGhostSubid1_hook(GB *gb);
void runVeranGhostSubid2_hook(GB *gb);
void runVeranGhostSubid2__substate0_hook(GB *gb);
void runVeranGhostSubid2__animate_hook(GB *gb);
void runVeranGhostSubid2__substate1_hook(GB *gb);
void runVeranGhostSubid2__substate2_hook(GB *gb);
void runVeranGhostSubid2__substate3_hook(GB *gb);

void interactionCode3e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4cae, 0x4cb0); E = 0x44;
  CYC(0x4cb0, 0x4cb1); A = mem_rd(gb, DE);
  CYC(0x4cb1, 0x4cb2); push_effect(gb, 0x4cb2); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x4cb6: goto L_4cb6; case 0x4cf0: goto L_4cf0; default: HANDOFF(HL); }
L_4cb6:
  CYC(0x4cb6, 0x4cb8); A = 0x01;
  CYC(0x4cb8, 0x4cb9); mem_wr(gb, DE, A);
  CALL_C(0x4cb9, interactionInitGraphics_hook, 0x15fb, 0x4cbc);
L_4cbc:
  CALL_C(0x4cbc, objectSetVisible83_hook, 0x1e72, 0x4cbf);
  CYC(0x4cbf, 0x4cc1); E = 0x42;
  CYC(0x4cc1, 0x4cc2); A = mem_rd(gb, DE);
  CYC(0x4cc2, 0x4cc3); push_effect(gb, 0x4cc3); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x4cc9: goto L_4cc9; case 0x4cd1: goto L_4cd1; case 0x4ce6: goto L_4ce6; default: HANDOFF(HL); }
L_4cc9:
  CYC(0x4cc9, 0x4ccb); E = 0x46;
  CYC(0x4ccb, 0x4ccd); A = 0x78;
  CYC(0x4ccd, 0x4cce); mem_wr(gb, DE, A);
  CYC(0x4cce, 0x4cd1); if (hook_enabled_at(0x2701)) { interactionSetAlwaysUpdateBit_hook(gb); return; } HANDOFF(0x2701);
L_4cd1:
  CYC(0x4cd1, 0x4cd2); H = D;
  CYC(0x4cd2, 0x4cd4); L = 0x49;
  CYC(0x4cd4, 0x4cd6); mem_wr(gb, HL, 0x10);
  CYC(0x4cd6, 0x4cd8); L = 0x50;
  CYC(0x4cd8, 0x4cda); mem_wr(gb, HL, 0x1e);
  CYC(0x4cda, 0x4cdd); SET_HL(0x5dd0);
  CALL_C(0x4cdd, interactionSetScript_hook, 0x2544, 0x4ce0);
  CALL_C(0x4ce0, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x4ce3);
  CYC(0x4ce3, 0x4ce6); if (hook_enabled_at(0x1e60)) { objectSetVisible81_hook(gb); return; } HANDOFF(0x1e60);
L_4ce6:
  CYC(0x4ce6, 0x4ce8); E = 0x50;
  CYC(0x4ce8, 0x4cea); A = 0x50;
  CYC(0x4cea, 0x4ceb); mem_wr(gb, DE, A);
  CYC(0x4ceb, 0x4ced); A = 0xa4;
  CYC(0x4ced, 0x4cf0); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
L_4cf0:
  CYC(0x4cf0, 0x4cf2); E = 0x42;
  CYC(0x4cf2, 0x4cf3); A = mem_rd(gb, DE);
  CYC(0x4cf3, 0x4cf4); push_effect(gb, 0x4cf4); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x4cfa: runVeranGhostSubid0_hook(gb); return;
    case 0x4dfb: runVeranGhostSubid1_hook(gb); return;
    case 0x4e0a: runVeranGhostSubid2_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void interactionCode3e__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4cb6:
  CYC(0x4cb6, 0x4cb8); A = 0x01;
  CYC(0x4cb8, 0x4cb9); mem_wr(gb, DE, A);
  CALL_C(0x4cb9, interactionInitGraphics_hook, 0x15fb, 0x4cbc);
L_4cbc:
  CALL_C(0x4cbc, objectSetVisible83_hook, 0x1e72, 0x4cbf);
  CYC(0x4cbf, 0x4cc1); E = 0x42;
  CYC(0x4cc1, 0x4cc2); A = mem_rd(gb, DE);
  CYC(0x4cc2, 0x4cc3); push_effect(gb, 0x4cc3); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x4cc9: goto L_4cc9; case 0x4cd1: goto L_4cd1; case 0x4ce6: goto L_4ce6; default: HANDOFF(HL); }
L_4cc9:
  CYC(0x4cc9, 0x4ccb); E = 0x46;
  CYC(0x4ccb, 0x4ccd); A = 0x78;
  CYC(0x4ccd, 0x4cce); mem_wr(gb, DE, A);
  CYC(0x4cce, 0x4cd1); if (hook_enabled_at(0x2701)) { interactionSetAlwaysUpdateBit_hook(gb); return; } HANDOFF(0x2701);
L_4cd1:
  CYC(0x4cd1, 0x4cd2); H = D;
  CYC(0x4cd2, 0x4cd4); L = 0x49;
  CYC(0x4cd4, 0x4cd6); mem_wr(gb, HL, 0x10);
  CYC(0x4cd6, 0x4cd8); L = 0x50;
  CYC(0x4cd8, 0x4cda); mem_wr(gb, HL, 0x1e);
  CYC(0x4cda, 0x4cdd); SET_HL(0x5dd0);
  CALL_C(0x4cdd, interactionSetScript_hook, 0x2544, 0x4ce0);
  CALL_C(0x4ce0, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x4ce3);
  CYC(0x4ce3, 0x4ce6); if (hook_enabled_at(0x1e60)) { objectSetVisible81_hook(gb); return; } HANDOFF(0x1e60);
L_4ce6:
  CYC(0x4ce6, 0x4ce8); E = 0x50;
  CYC(0x4ce8, 0x4cea); A = 0x50;
  CYC(0x4cea, 0x4ceb); mem_wr(gb, DE, A);
  CYC(0x4ceb, 0x4ced); A = 0xa4;
  CYC(0x4ced, 0x4cf0); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
}

void interactionCode3e__afterCall4cbc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4cbc:
  CALL_C(0x4cbc, objectSetVisible83_hook, 0x1e72, 0x4cbf);
  CYC(0x4cbf, 0x4cc1); E = 0x42;
  CYC(0x4cc1, 0x4cc2); A = mem_rd(gb, DE);
  CYC(0x4cc2, 0x4cc3); push_effect(gb, 0x4cc3); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x4cc9: goto L_4cc9; case 0x4cd1: goto L_4cd1; case 0x4ce6: goto L_4ce6; default: HANDOFF(HL); }
L_4cc9:
  CYC(0x4cc9, 0x4ccb); E = 0x46;
  CYC(0x4ccb, 0x4ccd); A = 0x78;
  CYC(0x4ccd, 0x4cce); mem_wr(gb, DE, A);
  CYC(0x4cce, 0x4cd1); if (hook_enabled_at(0x2701)) { interactionSetAlwaysUpdateBit_hook(gb); return; } HANDOFF(0x2701);
L_4cd1:
  CYC(0x4cd1, 0x4cd2); H = D;
  CYC(0x4cd2, 0x4cd4); L = 0x49;
  CYC(0x4cd4, 0x4cd6); mem_wr(gb, HL, 0x10);
  CYC(0x4cd6, 0x4cd8); L = 0x50;
  CYC(0x4cd8, 0x4cda); mem_wr(gb, HL, 0x1e);
  CYC(0x4cda, 0x4cdd); SET_HL(0x5dd0);
  CALL_C(0x4cdd, interactionSetScript_hook, 0x2544, 0x4ce0);
  CALL_C(0x4ce0, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x4ce3);
  CYC(0x4ce3, 0x4ce6); if (hook_enabled_at(0x1e60)) { objectSetVisible81_hook(gb); return; } HANDOFF(0x1e60);
L_4ce6:
  CYC(0x4ce6, 0x4ce8); E = 0x50;
  CYC(0x4ce8, 0x4cea); A = 0x50;
  CYC(0x4cea, 0x4ceb); mem_wr(gb, DE, A);
  CYC(0x4ceb, 0x4ced); A = 0xa4;
  CYC(0x4ced, 0x4cf0); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
}

void interactionCode3e__subid0Init_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4cc9:
  CYC(0x4cc9, 0x4ccb); E = 0x46;
  CYC(0x4ccb, 0x4ccd); A = 0x78;
  CYC(0x4ccd, 0x4cce); mem_wr(gb, DE, A);
  CYC(0x4cce, 0x4cd1); if (hook_enabled_at(0x2701)) { interactionSetAlwaysUpdateBit_hook(gb); return; } HANDOFF(0x2701);
}

void interactionCode3e__subid1Init_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4cd1:
  CYC(0x4cd1, 0x4cd2); H = D;
  CYC(0x4cd2, 0x4cd4); L = 0x49;
  CYC(0x4cd4, 0x4cd6); mem_wr(gb, HL, 0x10);
  CYC(0x4cd6, 0x4cd8); L = 0x50;
  CYC(0x4cd8, 0x4cda); mem_wr(gb, HL, 0x1e);
  CYC(0x4cda, 0x4cdd); SET_HL(0x5dd0);
  CALL_C(0x4cdd, interactionSetScript_hook, 0x2544, 0x4ce0);
  CALL_C(0x4ce0, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x4ce3);
  CYC(0x4ce3, 0x4ce6); if (hook_enabled_at(0x1e60)) { objectSetVisible81_hook(gb); return; } HANDOFF(0x1e60);
}

void interactionCode3e__subid2Init_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4ce6:
  CYC(0x4ce6, 0x4ce8); E = 0x50;
  CYC(0x4ce8, 0x4cea); A = 0x50;
  CYC(0x4cea, 0x4ceb); mem_wr(gb, DE, A);
  CYC(0x4ceb, 0x4ced); A = 0xa4;
  CYC(0x4ced, 0x4cf0); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
}

void interactionCode3e__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4cf0:
  CYC(0x4cf0, 0x4cf2); E = 0x42;
  CYC(0x4cf2, 0x4cf3); A = mem_rd(gb, DE);
  CYC(0x4cf3, 0x4cf4); push_effect(gb, 0x4cf4); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x4cfa: runVeranGhostSubid0_hook(gb); return;
    case 0x4dfb: runVeranGhostSubid1_hook(gb); return;
    case 0x4e0a: runVeranGhostSubid2_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void runVeranGhostSubid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4cfa, 0x4cfc); E = 0x79;
  CYC(0x4cfc, 0x4cfd); A = mem_rd(gb, DE);
  CYC(0x4cfd, 0x4cfe); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(0x4cfe, interactionAnimate_hook, 0x261b, 0x4d01); } else CYC(0x4cfe, 0x4d01);
  CYC(0x4d01, 0x4d03); E = 0x45;
  CYC(0x4d03, 0x4d04); A = mem_rd(gb, DE);
  CYC(0x4d04, 0x4d05); push_effect(gb, 0x4d05); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x4d17: goto L_4d17; case 0x4d3b: goto L_4d3b; case 0x4d4a: goto L_4d4a; case 0x4d80: goto L_4d80; case 0x4d9d: goto L_4d9d; case 0x4da5: goto L_4da5; case 0x4de2: goto L_4de2; case 0x4df4: goto L_4df4; default: HANDOFF(HL); }
L_4d17:
  CALL_C(0x4d17, interactionDecCounter1_hook, 0x23cc, 0x4d1a);
  if (!(F & FZ)) { CYCT(0x4d1a, 0x4d1c); goto L_4d31; } CYC(0x4d1a, 0x4d1c);
  CYC(0x4d1c, 0x4d1e); mem_wr(gb, HL, 0x5a);
  CYC(0x4d1e, 0x4d20); L = 0x49;
  CYC(0x4d20, 0x4d22); mem_wr(gb, HL, 0x00);
  CYC(0x4d22, 0x4d24); L = 0x50;
  CYC(0x4d24, 0x4d26); mem_wr(gb, HL, 0x0a);
  CALL_C(0x4d26, interactionIncSubstate_hook, 0x23e5, 0x4d29);
  CYC(0x4d29, 0x4d2b); A = 0x1d;
  CALL_C(0x4d2b, playSound_b00_hook, 0x0c98, 0x4d2e);
  CYC(0x4d2e, 0x4d31); if (hook_enabled_at(0x1e57)) { objectSetVisible80_hook(gb); return; } HANDOFF(0x1e57);
L_4d31:
  CYC(0x4d31, 0x4d34); A = mem_rd(gb, 0xcc00);
  CYC(0x4d34, 0x4d35); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4d35, 0x4d38); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72); } CYC(0x4d35, 0x4d38);
  CYC(0x4d38, 0x4d3b); if (hook_enabled_at(0x1e57)) { objectSetVisible80_hook(gb); return; } HANDOFF(0x1e57);
L_4d3b:
  CALL_C(0x4d3b, interactionDecCounter1_hook, 0x23cc, 0x4d3e);
  if (!(F & FZ)) { CYCT(0x4d3e, 0x4d41); if (hook_enabled_at(0x201d)) { objectApplySpeed_hook(gb); return; } HANDOFF(0x201d); } CYC(0x4d3e, 0x4d41);
  CALL_C(0x4d41, interactionIncSubstate_hook, 0x23e5, 0x4d44);
  CYC(0x4d44, 0x4d47); SET_HL(0x5db1);
  CYC(0x4d47, 0x4d4a); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
L_4d4a:
  CYC(0x4d4a, 0x4d4d); A = mem_rd(gb, 0xcfd1);
  CYC(0x4d4d, 0x4d4e); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x4d4e, 0x4d50); goto L_4d70; } CYC(0x4d4e, 0x4d50);
  CYC(0x4d50, 0x4d53); SET_BC(0xbb00);
  CALL_C(0x4d53, objectCreateInteraction_hook, 0x24c5, 0x4d56);
  if (!(F & FZ)) { CYCT(0x4d56, 0x4d57); ret_effect(gb); return; } CYC(0x4d56, 0x4d57);
  CYC(0x4d57, 0x4d59); L = 0x56;
  CYC(0x4d59, 0x4d5b); A = 0x40;
  CYC(0x4d5b, 0x4d5c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4d5c, 0x4d5d); mem_wr(gb, HL, D);
L_4d5d:
  CALL_C(0x4d5d, interactionIncSubstate_hook, 0x23e5, 0x4d60);
  CYC(0x4d60, 0x4d62); L = 0x78;
  CYC(0x4d62, 0x4d64); mem_wr(gb, HL, 0x78);
  CYC(0x4d64, 0x4d66); L = 0x79;
  CYC(0x4d66, 0x4d67); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d67, 0x4d68); alu_xor(gb, A);
  CALL_C(0x4d68, interactionSetAnimation_hook, 0x262e, 0x4d6b);
  CYC(0x4d6b, 0x4d6d); A = 0x8d;
  CYC(0x4d6d, 0x4d70); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
L_4d70:
  CALL_C(0x4d70, objectGetPosition_hook, 0x208a, 0x4d73);
  CYC(0x4d73, 0x4d76); SET_HL(0xcfd5);
  CYC(0x4d76, 0x4d77); mem_wr(gb, HL, B);
  CYC(0x4d77, 0x4d78); L = alu_inc8(gb, L);
  CYC(0x4d78, 0x4d7a); E = 0x7d;
  CYC(0x4d7a, 0x4d7b); A = C;
  CYC(0x4d7b, 0x4d7c); mem_wr(gb, DE, A);
  CYC(0x4d7c, 0x4d7d); mem_wr(gb, HL, A);
  CYC(0x4d7d, 0x4d80); if (hook_enabled_at(0x2552)) { interactionRunScript_hook(gb); return; } HANDOFF(0x2552);
L_4d80:
  CYC(0x4d80, 0x4d81); H = D;
  CYC(0x4d81, 0x4d83); L = 0x78;
  CYC(0x4d83, 0x4d84); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4d84, 0x4d86); B = 0x01;
  if (!(F & FZ)) { CYCT(0x4d86, 0x4d89); if (hook_enabled_at(0x24e6)) { objectFlickerVisibility_hook(gb); return; } HANDOFF(0x24e6); } CYC(0x4d86, 0x4d89);
  CYC(0x4d89, 0x4d8b); L = 0x79;
  CYC(0x4d8b, 0x4d8c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x4d8c, interactionIncSubstate_hook, 0x23e5, 0x4d8f);
  CYC(0x4d8f, 0x4d90); A = mem_rd(gb, HL);
  CYC(0x4d90, 0x4d92); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x4d92, 0x4d95); if (hook_enabled_at(0x1e84)) { objectSetVisible_hook(gb); return; } HANDOFF(0x1e84); } CYC(0x4d92, 0x4d95);
  CALL_C(0x4d95, objectSetInvisible_hook, 0x1e7b, 0x4d98);
  CYC(0x4d98, 0x4d9a); A = 0xab;
  CYC(0x4d9a, 0x4d9d); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
L_4d9d:
  CYC(0x4d9d, 0x4da0); A = mem_rd(gb, 0xcfd1);
  CYC(0x4da0, 0x4da2); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4da2, 0x4da3); ret_effect(gb); return; } CYC(0x4da2, 0x4da3);
  CYC(0x4da3, 0x4da5); goto L_4d5d;
L_4da5:
  CYC(0x4da5, 0x4da8); A = mem_rd(gb, 0xcfd0);
  CYC(0x4da8, 0x4daa); alu_cp(gb, 0x12);
  if (!(F & FZ)) { CYCT(0x4daa, 0x4dac); goto L_4db4; } CYC(0x4daa, 0x4dac);
  CYC(0x4dac, 0x4daf); SET_BC(0x0302);
  CALL_C(0x4daf, runVeranGhostSubid0__rumbleAndRandomizeX_hook, 0x4dcb, 0x4db2);
  CYC(0x4db2, 0x4db4); goto L_4dc1;
L_4db4:
  CALL_C(0x4db4, objectGetPosition_hook, 0x208a, 0x4db7);
  CYC(0x4db7, 0x4dba); SET_HL(0xcfd5);
  CYC(0x4dba, 0x4dbb); mem_wr(gb, HL, B);
  CYC(0x4dbb, 0x4dbc); L = alu_inc8(gb, L);
  CYC(0x4dbc, 0x4dbe); E = 0x7d;
  CYC(0x4dbe, 0x4dbf); A = C;
  CYC(0x4dbf, 0x4dc0); mem_wr(gb, DE, A);
  CYC(0x4dc0, 0x4dc1); mem_wr(gb, HL, A);
L_4dc1:
  CALL_C(0x4dc1, interactionRunScript_hook, 0x2552, 0x4dc4);
  if (!(F & FC)) { CYCT(0x4dc4, 0x4dc5); ret_effect(gb); return; } CYC(0x4dc4, 0x4dc5);
  CALL_C(0x4dc5, objectSetInvisible_hook, 0x1e7b, 0x4dc8);
  CYC(0x4dc8, 0x4dcb); if (hook_enabled_at(0x23e5)) { interactionIncSubstate_hook(gb); return; } HANDOFF(0x23e5);
L_4de2:
  CYC(0x4de2, 0x4de5); A = mem_rd(gb, 0xcfd0);
  CYC(0x4de5, 0x4de7); alu_cp(gb, 0x17);
  if (!(F & FZ)) { CYCT(0x4de7, 0x4de8); ret_effect(gb); return; } CYC(0x4de7, 0x4de8);
  CALL_C(0x4de8, interactionIncSubstate_hook, 0x23e5, 0x4deb);
  CYC(0x4deb, 0x4dee); SET_HL(0x5db5);
  CALL_C(0x4dee, interactionSetScript_hook, 0x2544, 0x4df1);
  CALL_C(0x4df1, objectSetVisible80_hook, 0x1e57, 0x4df4);
L_4df4:
  CALL_C(0x4df4, interactionRunScript_hook, 0x2552, 0x4df7);
  if (!(F & FC)) { CYCT(0x4df7, 0x4df8); ret_effect(gb); return; } CYC(0x4df7, 0x4df8);
  CYC(0x4df8, 0x4dfb); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05);
}

void runVeranGhostSubid0__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4d17:
  CALL_C(0x4d17, interactionDecCounter1_hook, 0x23cc, 0x4d1a);
  if (!(F & FZ)) { CYCT(0x4d1a, 0x4d1c); goto L_4d31; } CYC(0x4d1a, 0x4d1c);
  CYC(0x4d1c, 0x4d1e); mem_wr(gb, HL, 0x5a);
  CYC(0x4d1e, 0x4d20); L = 0x49;
  CYC(0x4d20, 0x4d22); mem_wr(gb, HL, 0x00);
  CYC(0x4d22, 0x4d24); L = 0x50;
  CYC(0x4d24, 0x4d26); mem_wr(gb, HL, 0x0a);
  CALL_C(0x4d26, interactionIncSubstate_hook, 0x23e5, 0x4d29);
  CYC(0x4d29, 0x4d2b); A = 0x1d;
  CALL_C(0x4d2b, playSound_b00_hook, 0x0c98, 0x4d2e);
  CYC(0x4d2e, 0x4d31); if (hook_enabled_at(0x1e57)) { objectSetVisible80_hook(gb); return; } HANDOFF(0x1e57);
L_4d31:
  CYC(0x4d31, 0x4d34); A = mem_rd(gb, 0xcc00);
  CYC(0x4d34, 0x4d35); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4d35, 0x4d38); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72); } CYC(0x4d35, 0x4d38);
  CYC(0x4d38, 0x4d3b); if (hook_enabled_at(0x1e57)) { objectSetVisible80_hook(gb); return; } HANDOFF(0x1e57);
}

void runVeranGhostSubid0__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4d3b:
  CALL_C(0x4d3b, interactionDecCounter1_hook, 0x23cc, 0x4d3e);
  if (!(F & FZ)) { CYCT(0x4d3e, 0x4d41); if (hook_enabled_at(0x201d)) { objectApplySpeed_hook(gb); return; } HANDOFF(0x201d); } CYC(0x4d3e, 0x4d41);
  CALL_C(0x4d41, interactionIncSubstate_hook, 0x23e5, 0x4d44);
  CYC(0x4d44, 0x4d47); SET_HL(0x5db1);
  CYC(0x4d47, 0x4d4a); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
}

void runVeranGhostSubid0__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4d4a:
  CYC(0x4d4a, 0x4d4d); A = mem_rd(gb, 0xcfd1);
  CYC(0x4d4d, 0x4d4e); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x4d4e, 0x4d50); goto L_4d70; } CYC(0x4d4e, 0x4d50);
  CYC(0x4d50, 0x4d53); SET_BC(0xbb00);
  CALL_C(0x4d53, objectCreateInteraction_hook, 0x24c5, 0x4d56);
  if (!(F & FZ)) { CYCT(0x4d56, 0x4d57); ret_effect(gb); return; } CYC(0x4d56, 0x4d57);
  CYC(0x4d57, 0x4d59); L = 0x56;
  CYC(0x4d59, 0x4d5b); A = 0x40;
  CYC(0x4d5b, 0x4d5c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4d5c, 0x4d5d); mem_wr(gb, HL, D);
  CALL_C(0x4d5d, interactionIncSubstate_hook, 0x23e5, 0x4d60);
  CYC(0x4d60, 0x4d62); L = 0x78;
  CYC(0x4d62, 0x4d64); mem_wr(gb, HL, 0x78);
  CYC(0x4d64, 0x4d66); L = 0x79;
  CYC(0x4d66, 0x4d67); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d67, 0x4d68); alu_xor(gb, A);
  CALL_C(0x4d68, interactionSetAnimation_hook, 0x262e, 0x4d6b);
  CYC(0x4d6b, 0x4d6d); A = 0x8d;
  CYC(0x4d6d, 0x4d70); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
L_4d70:
  CALL_C(0x4d70, objectGetPosition_hook, 0x208a, 0x4d73);
  CYC(0x4d73, 0x4d76); SET_HL(0xcfd5);
  CYC(0x4d76, 0x4d77); mem_wr(gb, HL, B);
  CYC(0x4d77, 0x4d78); L = alu_inc8(gb, L);
  CYC(0x4d78, 0x4d7a); E = 0x7d;
  CYC(0x4d7a, 0x4d7b); A = C;
  CYC(0x4d7b, 0x4d7c); mem_wr(gb, DE, A);
  CYC(0x4d7c, 0x4d7d); mem_wr(gb, HL, A);
  CYC(0x4d7d, 0x4d80); if (hook_enabled_at(0x2552)) { interactionRunScript_hook(gb); return; } HANDOFF(0x2552);
}

void runVeranGhostSubid0__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4d80:
  CYC(0x4d80, 0x4d81); H = D;
  CYC(0x4d81, 0x4d83); L = 0x78;
  CYC(0x4d83, 0x4d84); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4d84, 0x4d86); B = 0x01;
  if (!(F & FZ)) { CYCT(0x4d86, 0x4d89); if (hook_enabled_at(0x24e6)) { objectFlickerVisibility_hook(gb); return; } HANDOFF(0x24e6); } CYC(0x4d86, 0x4d89);
  CYC(0x4d89, 0x4d8b); L = 0x79;
  CYC(0x4d8b, 0x4d8c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x4d8c, interactionIncSubstate_hook, 0x23e5, 0x4d8f);
  CYC(0x4d8f, 0x4d90); A = mem_rd(gb, HL);
  CYC(0x4d90, 0x4d92); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x4d92, 0x4d95); if (hook_enabled_at(0x1e84)) { objectSetVisible_hook(gb); return; } HANDOFF(0x1e84); } CYC(0x4d92, 0x4d95);
  CALL_C(0x4d95, objectSetInvisible_hook, 0x1e7b, 0x4d98);
  CYC(0x4d98, 0x4d9a); A = 0xab;
  CYC(0x4d9a, 0x4d9d); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
}

void runVeranGhostSubid0__substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  goto L_4d9d;
L_4d5d:
  CALL_C(0x4d5d, interactionIncSubstate_hook, 0x23e5, 0x4d60);
  CYC(0x4d60, 0x4d62); L = 0x78;
  CYC(0x4d62, 0x4d64); mem_wr(gb, HL, 0x78);
  CYC(0x4d64, 0x4d66); L = 0x79;
  CYC(0x4d66, 0x4d67); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d67, 0x4d68); alu_xor(gb, A);
  CALL_C(0x4d68, interactionSetAnimation_hook, 0x262e, 0x4d6b);
  CYC(0x4d6b, 0x4d6d); A = 0x8d;
  CYC(0x4d6d, 0x4d70); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);
L_4d9d:
  CYC(0x4d9d, 0x4da0); A = mem_rd(gb, 0xcfd1);
  CYC(0x4da0, 0x4da2); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4da2, 0x4da3); ret_effect(gb); return; } CYC(0x4da2, 0x4da3);
  CYC(0x4da3, 0x4da5); goto L_4d5d;
}

void runVeranGhostSubid0__substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4da5:
  CYC(0x4da5, 0x4da8); A = mem_rd(gb, 0xcfd0);
  CYC(0x4da8, 0x4daa); alu_cp(gb, 0x12);
  if (!(F & FZ)) { CYCT(0x4daa, 0x4dac); goto L_4db4; } CYC(0x4daa, 0x4dac);
  CYC(0x4dac, 0x4daf); SET_BC(0x0302);
  CALL_C(0x4daf, runVeranGhostSubid0__rumbleAndRandomizeX_hook, 0x4dcb, 0x4db2);
  CYC(0x4db2, 0x4db4); goto L_4dc1;
L_4db4:
  CALL_C(0x4db4, objectGetPosition_hook, 0x208a, 0x4db7);
  CYC(0x4db7, 0x4dba); SET_HL(0xcfd5);
  CYC(0x4dba, 0x4dbb); mem_wr(gb, HL, B);
  CYC(0x4dbb, 0x4dbc); L = alu_inc8(gb, L);
  CYC(0x4dbc, 0x4dbe); E = 0x7d;
  CYC(0x4dbe, 0x4dbf); A = C;
  CYC(0x4dbf, 0x4dc0); mem_wr(gb, DE, A);
  CYC(0x4dc0, 0x4dc1); mem_wr(gb, HL, A);
L_4dc1:
  CALL_C(0x4dc1, interactionRunScript_hook, 0x2552, 0x4dc4);
  if (!(F & FC)) { CYCT(0x4dc4, 0x4dc5); ret_effect(gb); return; } CYC(0x4dc4, 0x4dc5);
  CALL_C(0x4dc5, objectSetInvisible_hook, 0x1e7b, 0x4dc8);
  CYC(0x4dc8, 0x4dcb); if (hook_enabled_at(0x23e5)) { interactionIncSubstate_hook(gb); return; } HANDOFF(0x23e5);
}

void runVeranGhostSubid0__rumbleAndRandomizeX_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4dcb:
  CYC(0x4dcb, 0x4dce); A = mem_rd(gb, 0xcc00);
  CYC(0x4dce, 0x4dd0); alu_and(gb, 0x0f);
  CYC(0x4dd0, 0x4dd2); A = 0xb8;
  if ((F & FZ)) { CALL_C_CC(0x4dd2, playSound_b00_hook, 0x0c98, 0x4dd5); } else CYC(0x4dd2, 0x4dd5);
  CALL_C(0x4dd5, getRandomNumber_hook, 0x043e, 0x4dd8);
  CYC(0x4dd8, 0x4dd9); alu_and(gb, B);
  CYC(0x4dd9, 0x4dda); alu_sub(gb, C);
  CYC(0x4dda, 0x4ddb); H = D;
  CYC(0x4ddb, 0x4ddd); L = 0x7d;
  CYC(0x4ddd, 0x4dde); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4dde, 0x4de0); L = 0x4d;
  CYC(0x4de0, 0x4de1); mem_wr(gb, HL, A);
  CYC(0x4de1, 0x4de2); ret_effect(gb); return;
}

void runVeranGhostSubid0__substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4de2:
  CYC(0x4de2, 0x4de5); A = mem_rd(gb, 0xcfd0);
  CYC(0x4de5, 0x4de7); alu_cp(gb, 0x17);
  if (!(F & FZ)) { CYCT(0x4de7, 0x4de8); ret_effect(gb); return; } CYC(0x4de7, 0x4de8);
  CALL_C(0x4de8, interactionIncSubstate_hook, 0x23e5, 0x4deb);
  CYC(0x4deb, 0x4dee); SET_HL(0x5db5);
  CALL_C(0x4dee, interactionSetScript_hook, 0x2544, 0x4df1);
  CALL_C(0x4df1, objectSetVisible80_hook, 0x1e57, 0x4df4);
L_4df4:
  CALL_C(0x4df4, interactionRunScript_hook, 0x2552, 0x4df7);
  if (!(F & FC)) { CYCT(0x4df7, 0x4df8); ret_effect(gb); return; } CYC(0x4df7, 0x4df8);
  CYC(0x4df8, 0x4dfb); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05);
}

void runVeranGhostSubid0__substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4df4:
  CALL_C(0x4df4, interactionRunScript_hook, 0x2552, 0x4df7);
  if (!(F & FC)) { CYCT(0x4df7, 0x4df8); ret_effect(gb); return; } CYC(0x4df7, 0x4df8);
  CYC(0x4df8, 0x4dfb); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05);
}

void runVeranGhostSubid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4dfb, 0x4dfe); A = mem_rd(gb, 0xcba0);
  CYC(0x4dfe, 0x4dff); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4dff, 0x4e01); goto L_4e07; } CYC(0x4dff, 0x4e01);
  CALL_C(0x4e01, interactionRunScript_hook, 0x2552, 0x4e04);
  if ((F & FC)) { CYCT(0x4e04, 0x4e07); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x4e04, 0x4e07);
L_4e07:
  CYC(0x4e07, 0x4e0a); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
}

void runVeranGhostSubid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4e0a, 0x4e0c); E = 0x45;
  CYC(0x4e0c, 0x4e0d); A = mem_rd(gb, DE);
  CYC(0x4e0d, 0x4e0e); push_effect(gb, 0x4e0e); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x4e16: goto L_4e16; case 0x4e4c: goto L_4e4c; case 0x4e59: goto L_4e59; case 0x4e72: goto L_4e72; default: HANDOFF(HL); }
L_4e16:
  CYC(0x4e16, 0x4e19); SET_BC(0x5878);
  CYC(0x4e19, 0x4e1b); E = 0x4b;
  CYC(0x4e1b, 0x4e1c); A = mem_rd(gb, DE);
  CYC(0x4e1c, 0x4e1e); mem_wr(gb, 0xff8f, A);
  CYC(0x4e1e, 0x4e20); E = 0x4d;
  CYC(0x4e20, 0x4e21); A = mem_rd(gb, DE);
  CYC(0x4e21, 0x4e23); mem_wr(gb, 0xff8e, A);
  CYC(0x4e23, 0x4e24); alu_sub(gb, C);
  CYC(0x4e24, 0x4e25); A = alu_inc8(gb, A);
  CYC(0x4e25, 0x4e27); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x4e27, 0x4e29); goto L_4e40; } CYC(0x4e27, 0x4e29);
  CYC(0x4e29, 0x4e2b); A = mem_rd(gb, 0xff8f);
  CYC(0x4e2b, 0x4e2c); alu_sub(gb, B);
  CYC(0x4e2c, 0x4e2d); A = alu_inc8(gb, A);
  CYC(0x4e2d, 0x4e2f); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x4e2f, 0x4e31); goto L_4e40; } CYC(0x4e2f, 0x4e31);
  CALL_C(0x4e31, interactionIncSubstate_hook, 0x23e5, 0x4e34);
  CYC(0x4e34, 0x4e36); L = 0x4b;
  CYC(0x4e36, 0x4e37); mem_wr(gb, HL, B);
  CYC(0x4e37, 0x4e39); L = 0x4d;
  CYC(0x4e39, 0x4e3a); mem_wr(gb, HL, C);
  CYC(0x4e3a, 0x4e3c); L = 0x46;
  CYC(0x4e3c, 0x4e3e); mem_wr(gb, HL, 0x3c);
  CYC(0x4e3e, 0x4e40); goto L_4e49;
L_4e40:
  CALL_C(0x4e40, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x4e43);
  CYC(0x4e43, 0x4e45); E = 0x49;
  CYC(0x4e45, 0x4e46); mem_wr(gb, DE, A);
  CALL_C(0x4e46, objectApplySpeed_hook, 0x201d, 0x4e49);
L_4e49:
  CYC(0x4e49, 0x4e4c); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
L_4e4c:
  CALL_C(0x4e4c, interactionDecCounter1_hook, 0x23cc, 0x4e4f);
  if (!(F & FZ)) { CYCT(0x4e4f, 0x4e51); goto L_4e49; } CYC(0x4e4f, 0x4e51);
  CYC(0x4e51, 0x4e52); L = E;
  CYC(0x4e52, 0x4e53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4e53, 0x4e56); SET_BC(0x560e);
  CYC(0x4e56, 0x4e59); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);
L_4e59:
  CALL_C(0x4e59, getFreeEnemySlot_hook, 0x2e27, 0x4e5c);
  if (!(F & FZ)) { CYCT(0x4e5c, 0x4e5d); ret_effect(gb); return; } CYC(0x4e5c, 0x4e5d);
  CYC(0x4e5d, 0x4e5f); mem_wr(gb, HL, 0x06);
  CALL_C(0x4e5f, objectCopyPosition_hook, 0x2242, 0x4e62);
  CYC(0x4e62, 0x4e64); E = 0x58;
  CYC(0x4e64, 0x4e66); A = 0x80;
  CYC(0x4e66, 0x4e67); mem_wr(gb, DE, A);
  CYC(0x4e67, 0x4e68); E = alu_inc8(gb, E);
  CYC(0x4e68, 0x4e69); A = H;
  CYC(0x4e69, 0x4e6a); mem_wr(gb, DE, A);
  CALL_C(0x4e6a, interactionIncSubstate_hook, 0x23e5, 0x4e6d);
  CYC(0x4e6d, 0x4e6f); L = 0x46;
  CYC(0x4e6f, 0x4e71); mem_wr(gb, HL, 0x3d);
  CYC(0x4e71, 0x4e72); ret_effect(gb); return;
L_4e72:
  CALL_C(0x4e72, interactionDecCounter1_hook, 0x23cc, 0x4e75);
  if ((F & FZ)) { CYCT(0x4e75, 0x4e77); goto L_4e84; } CYC(0x4e75, 0x4e77);
  CYC(0x4e77, 0x4e79); A = 0x1a;
  CALL_C(0x4e79, objectGetRelatedObject2Var_hook, 0x2164, 0x4e7c);
  CYC(0x4e7c, 0x4e7e); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x4e7e, 0x4e81); if (hook_enabled_at(0x1e69)) { objectSetVisible82_hook(gb); return; } HANDOFF(0x1e69); } CYC(0x4e7e, 0x4e81);
  CYC(0x4e81, 0x4e84); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);
L_4e84:
  CYC(0x4e84, 0x4e86); A = 0x01;
  CYC(0x4e86, 0x4e89); mem_wr(gb, 0xcc18, A);
  CYC(0x4e89, 0x4e8c); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05);
}

void runVeranGhostSubid2__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4e16:
  CYC(0x4e16, 0x4e19); SET_BC(0x5878);
  CYC(0x4e19, 0x4e1b); E = 0x4b;
  CYC(0x4e1b, 0x4e1c); A = mem_rd(gb, DE);
  CYC(0x4e1c, 0x4e1e); mem_wr(gb, 0xff8f, A);
  CYC(0x4e1e, 0x4e20); E = 0x4d;
  CYC(0x4e20, 0x4e21); A = mem_rd(gb, DE);
  CYC(0x4e21, 0x4e23); mem_wr(gb, 0xff8e, A);
  CYC(0x4e23, 0x4e24); alu_sub(gb, C);
  CYC(0x4e24, 0x4e25); A = alu_inc8(gb, A);
  CYC(0x4e25, 0x4e27); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x4e27, 0x4e29); goto L_4e40; } CYC(0x4e27, 0x4e29);
  CYC(0x4e29, 0x4e2b); A = mem_rd(gb, 0xff8f);
  CYC(0x4e2b, 0x4e2c); alu_sub(gb, B);
  CYC(0x4e2c, 0x4e2d); A = alu_inc8(gb, A);
  CYC(0x4e2d, 0x4e2f); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x4e2f, 0x4e31); goto L_4e40; } CYC(0x4e2f, 0x4e31);
  CALL_C(0x4e31, interactionIncSubstate_hook, 0x23e5, 0x4e34);
  CYC(0x4e34, 0x4e36); L = 0x4b;
  CYC(0x4e36, 0x4e37); mem_wr(gb, HL, B);
  CYC(0x4e37, 0x4e39); L = 0x4d;
  CYC(0x4e39, 0x4e3a); mem_wr(gb, HL, C);
  CYC(0x4e3a, 0x4e3c); L = 0x46;
  CYC(0x4e3c, 0x4e3e); mem_wr(gb, HL, 0x3c);
  CYC(0x4e3e, 0x4e40); goto L_4e49;
L_4e40:
  CALL_C(0x4e40, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x4e43);
  CYC(0x4e43, 0x4e45); E = 0x49;
  CYC(0x4e45, 0x4e46); mem_wr(gb, DE, A);
  CALL_C(0x4e46, objectApplySpeed_hook, 0x201d, 0x4e49);
L_4e49:
  CYC(0x4e49, 0x4e4c); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
}

void runVeranGhostSubid2__animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4e49:
  CYC(0x4e49, 0x4e4c); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
}

void runVeranGhostSubid2__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  goto L_4e4c;
L_4e49:
  CYC(0x4e49, 0x4e4c); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b);
L_4e4c:
  CALL_C(0x4e4c, interactionDecCounter1_hook, 0x23cc, 0x4e4f);
  if (!(F & FZ)) { CYCT(0x4e4f, 0x4e51); goto L_4e49; } CYC(0x4e4f, 0x4e51);
  CYC(0x4e51, 0x4e52); L = E;
  CYC(0x4e52, 0x4e53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4e53, 0x4e56); SET_BC(0x560e);
  CYC(0x4e56, 0x4e59); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);
}

void runVeranGhostSubid2__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4e59:
  CALL_C(0x4e59, getFreeEnemySlot_hook, 0x2e27, 0x4e5c);
  if (!(F & FZ)) { CYCT(0x4e5c, 0x4e5d); ret_effect(gb); return; } CYC(0x4e5c, 0x4e5d);
  CYC(0x4e5d, 0x4e5f); mem_wr(gb, HL, 0x06);
  CALL_C(0x4e5f, objectCopyPosition_hook, 0x2242, 0x4e62);
  CYC(0x4e62, 0x4e64); E = 0x58;
  CYC(0x4e64, 0x4e66); A = 0x80;
  CYC(0x4e66, 0x4e67); mem_wr(gb, DE, A);
  CYC(0x4e67, 0x4e68); E = alu_inc8(gb, E);
  CYC(0x4e68, 0x4e69); A = H;
  CYC(0x4e69, 0x4e6a); mem_wr(gb, DE, A);
  CALL_C(0x4e6a, interactionIncSubstate_hook, 0x23e5, 0x4e6d);
  CYC(0x4e6d, 0x4e6f); L = 0x46;
  CYC(0x4e6f, 0x4e71); mem_wr(gb, HL, 0x3d);
  CYC(0x4e71, 0x4e72); ret_effect(gb); return;
}

void runVeranGhostSubid2__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4e72:
  CALL_C(0x4e72, interactionDecCounter1_hook, 0x23cc, 0x4e75);
  if ((F & FZ)) { CYCT(0x4e75, 0x4e77); goto L_4e84; } CYC(0x4e75, 0x4e77);
  CYC(0x4e77, 0x4e79); A = 0x1a;
  CALL_C(0x4e79, objectGetRelatedObject2Var_hook, 0x2164, 0x4e7c);
  CYC(0x4e7c, 0x4e7e); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x4e7e, 0x4e81); if (hook_enabled_at(0x1e69)) { objectSetVisible82_hook(gb); return; } HANDOFF(0x1e69); } CYC(0x4e7e, 0x4e81);
  CYC(0x4e81, 0x4e84); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);
L_4e84:
  CYC(0x4e84, 0x4e86); A = 0x01;
  CYC(0x4e86, 0x4e89); mem_wr(gb, 0xcc18, A);
  CYC(0x4e89, 0x4e8c); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05);
}
