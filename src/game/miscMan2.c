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

void interactionCode44_hook(GB *gb);
void interactionCode44__subid0_hook(GB *gb);
void interactionCode44__subid0__initialized_hook(GB *gb);
void interactionCode44__subid1_hook(GB *gb);
void interactionCode44__afterCall54b3_hook(GB *gb);
void interactionCode44__subid2_hook(GB *gb);
void interactionCode44__subid3__initialized_hook(GB *gb);
void interactionCode44__subid4_hook(GB *gb);
void interactionCode44__subid4__initialized_hook(GB *gb);
void interactionCode44__initGraphicsAndIncState_hook(GB *gb);
void interactionCode44__initGraphicsIncStateAndLoadScript_hook(GB *gb);
void getGameProgress_1_hook(GB *gb);
void getGameProgress_1__noEssences_hook(GB *gb);
void getGameProgress_2_hook(GB *gb);
void getGameProgress_2__noEssences_hook(GB *gb);
void unusedFunc5598_hook(GB *gb);
void checkNpcShouldExistAtGameStage_body_hook(GB *gb);

void interactionCode44_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5484, 0x5486); E = 0x42;
  CYC(0x5486, 0x5487); A = mem_rd(gb, DE);
  CYC(0x5487, 0x5488); push_effect(gb, 0x5488); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x5492: goto L_5492; case 0x54ab: goto L_54ab; case 0x54b9: goto L_54b9; case 0x54df: goto L_54df; default: HANDOFF(HL); }
L_5492:
  CALL_C(0x5492, checkInteractionState_hook, 0x23fe, 0x5495);
  if (!(F & FZ)) { CYCT(0x5495, 0x5497); goto L_54a2; } CYC(0x5495, 0x5497);
  CYC(0x5497, 0x5499); A = 0x14;
  CALL_C(0x5499, checkGlobalFlag_hook, 0x31f3, 0x549c);
  if (!(F & FZ)) { CYCT(0x549c, 0x549f); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x549c, 0x549f);
  CALL_C(0x549f, interactionCode44__initGraphicsIncStateAndLoadScript_hook, 0x5515, 0x54a2);
L_54a2:
  CALL_C(0x54a2, interactionRunScript_hook, 0x2552, 0x54a5);
  if ((F & FC)) { CYCT(0x54a5, 0x54a8); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54a5, 0x54a8);
  CYC(0x54a8, 0x54ab); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
L_54ab:
  CALL_C(0x54ab, checkInteractionState_hook, 0x23fe, 0x54ae);
  if (!(F & FZ)) { CYCT(0x54ae, 0x54b0); goto L_54b3; } CYC(0x54ae, 0x54b0);
  CALL_C(0x54b0, interactionCode44__initGraphicsIncStateAndLoadScript_hook, 0x5515, 0x54b3);
L_54b3:
  CALL_C(0x54b3, interactionRunScript_hook, 0x2552, 0x54b6);
  CYC(0x54b6, 0x54b9); if (hook_enabled_at(0x2758)) { interactionAnimateBasedOnSpeed_hook(gb); return; } HANDOFF(0x2758);
L_54b9:
  CALL_C(0x54b9, checkInteractionState_hook, 0x23fe, 0x54bc);
  if (!(F & FZ)) { CYCT(0x54bc, 0x54be); goto L_54d9; } CYC(0x54bc, 0x54be);
  CALL_C(0x54be, getGameProgress_1_hook, 0x552b, 0x54c1);
  CYC(0x54c1, 0x54c3); C = 0x02;
  CYC(0x54c3, 0x54c5); A = 0x06;
  CALL_C(0x54c5, checkNpcShouldExistAtGameStage_body_hook, 0x55a6, 0x54c8);
  if (!(F & FZ)) { CYCT(0x54c8, 0x54cb); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54c8, 0x54cb);
  CYC(0x54cb, 0x54cc); A = B;
  CYC(0x54cc, 0x54cf); SET_HL(0x562a);
  CYC(0x54cf, 0x54d0); push_effect(gb, 0x54d0);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x54d0, 0x54d1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54d1, 0x54d2); H = mem_rd(gb, HL);
  CYC(0x54d2, 0x54d3); L = A;
  CALL_C(0x54d3, interactionSetScript_hook, 0x2544, 0x54d6);
  CALL_C(0x54d6, interactionCode44__initGraphicsAndIncState_hook, 0x550c, 0x54d9);
L_54d9:
  CALL_C(0x54d9, interactionRunScript_hook, 0x2552, 0x54dc);
  CYC(0x54dc, 0x54df); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
L_54df:
  CALL_C(0x54df, checkInteractionState_hook, 0x23fe, 0x54e2);
  if (!(F & FZ)) { CYCT(0x54e2, 0x54e4); goto L_5506; } CYC(0x54e2, 0x54e4);
  CALL_C(0x54e4, getGameProgress_2_hook, 0x5559, 0x54e7);
  CYC(0x54e7, 0x54e8); A = B;
  CYC(0x54e8, 0x54ea); alu_cp(gb, 0x03);
  if ((F & FZ)) { CYCT(0x54ea, 0x54ed); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54ea, 0x54ed);
  CYC(0x54ed, 0x54ef); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(0x54ef, 0x54f1); goto L_54f9; } CYC(0x54ef, 0x54f1);
  CYC(0x54f1, 0x54f4); SET_BC(0x5878);
  CALL_C(0x54f4, interactionSetPosition_hook, 0x2773, 0x54f7);
  CYC(0x54f7, 0x54f9); A = 0x06;
L_54f9:
  CYC(0x54f9, 0x54fc); SET_HL(0x5636);
  CYC(0x54fc, 0x54fd); push_effect(gb, 0x54fd);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x54fd, 0x54fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54fe, 0x54ff); H = mem_rd(gb, HL);
  CYC(0x54ff, 0x5500); L = A;
  CALL_C(0x5500, interactionSetScript_hook, 0x2544, 0x5503);
  CALL_C(0x5503, interactionCode44__initGraphicsAndIncState_hook, 0x550c, 0x5506);
L_5506:
  CALL_C(0x5506, interactionRunScript_hook, 0x2552, 0x5509);
  CYC(0x5509, 0x550c); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5492:
  CALL_C(0x5492, checkInteractionState_hook, 0x23fe, 0x5495);
  if (!(F & FZ)) { CYCT(0x5495, 0x5497); goto L_54a2; } CYC(0x5495, 0x5497);
  CYC(0x5497, 0x5499); A = 0x14;
  CALL_C(0x5499, checkGlobalFlag_hook, 0x31f3, 0x549c);
  if (!(F & FZ)) { CYCT(0x549c, 0x549f); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x549c, 0x549f);
  CALL_C(0x549f, interactionCode44__initGraphicsIncStateAndLoadScript_hook, 0x5515, 0x54a2);
L_54a2:
  CALL_C(0x54a2, interactionRunScript_hook, 0x2552, 0x54a5);
  if ((F & FC)) { CYCT(0x54a5, 0x54a8); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54a5, 0x54a8);
  CYC(0x54a8, 0x54ab); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__subid0__initialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54a2:
  CALL_C(0x54a2, interactionRunScript_hook, 0x2552, 0x54a5);
  if ((F & FC)) { CYCT(0x54a5, 0x54a8); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54a5, 0x54a8);
  CYC(0x54a8, 0x54ab); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54ab:
  CALL_C(0x54ab, checkInteractionState_hook, 0x23fe, 0x54ae);
  if (!(F & FZ)) { CYCT(0x54ae, 0x54b0); goto L_54b3; } CYC(0x54ae, 0x54b0);
  CALL_C(0x54b0, interactionCode44__initGraphicsIncStateAndLoadScript_hook, 0x5515, 0x54b3);
L_54b3:
  CALL_C(0x54b3, interactionRunScript_hook, 0x2552, 0x54b6);
  CYC(0x54b6, 0x54b9); if (hook_enabled_at(0x2758)) { interactionAnimateBasedOnSpeed_hook(gb); return; } HANDOFF(0x2758);
}

void interactionCode44__afterCall54b3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54b3:
  CALL_C(0x54b3, interactionRunScript_hook, 0x2552, 0x54b6);
  CYC(0x54b6, 0x54b9); if (hook_enabled_at(0x2758)) { interactionAnimateBasedOnSpeed_hook(gb); return; } HANDOFF(0x2758);
}

void interactionCode44__subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54b9:
  CALL_C(0x54b9, checkInteractionState_hook, 0x23fe, 0x54bc);
  if (!(F & FZ)) { CYCT(0x54bc, 0x54be); goto L_54d9; } CYC(0x54bc, 0x54be);
  CALL_C(0x54be, getGameProgress_1_hook, 0x552b, 0x54c1);
  CYC(0x54c1, 0x54c3); C = 0x02;
  CYC(0x54c3, 0x54c5); A = 0x06;
  CALL_C(0x54c5, checkNpcShouldExistAtGameStage_body_hook, 0x55a6, 0x54c8);
  if (!(F & FZ)) { CYCT(0x54c8, 0x54cb); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54c8, 0x54cb);
  CYC(0x54cb, 0x54cc); A = B;
  CYC(0x54cc, 0x54cf); SET_HL(0x562a);
  CYC(0x54cf, 0x54d0); push_effect(gb, 0x54d0);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x54d0, 0x54d1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54d1, 0x54d2); H = mem_rd(gb, HL);
  CYC(0x54d2, 0x54d3); L = A;
  CALL_C(0x54d3, interactionSetScript_hook, 0x2544, 0x54d6);
  CALL_C(0x54d6, interactionCode44__initGraphicsAndIncState_hook, 0x550c, 0x54d9);
L_54d9:
  CALL_C(0x54d9, interactionRunScript_hook, 0x2552, 0x54dc);
  CYC(0x54dc, 0x54df); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__subid3__initialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54d9:
  CALL_C(0x54d9, interactionRunScript_hook, 0x2552, 0x54dc);
  CYC(0x54dc, 0x54df); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__subid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54df:
  CALL_C(0x54df, checkInteractionState_hook, 0x23fe, 0x54e2);
  if (!(F & FZ)) { CYCT(0x54e2, 0x54e4); goto L_5506; } CYC(0x54e2, 0x54e4);
  CALL_C(0x54e4, getGameProgress_2_hook, 0x5559, 0x54e7);
  CYC(0x54e7, 0x54e8); A = B;
  CYC(0x54e8, 0x54ea); alu_cp(gb, 0x03);
  if ((F & FZ)) { CYCT(0x54ea, 0x54ed); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x54ea, 0x54ed);
  CYC(0x54ed, 0x54ef); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(0x54ef, 0x54f1); goto L_54f9; } CYC(0x54ef, 0x54f1);
  CYC(0x54f1, 0x54f4); SET_BC(0x5878);
  CALL_C(0x54f4, interactionSetPosition_hook, 0x2773, 0x54f7);
  CYC(0x54f7, 0x54f9); A = 0x06;
L_54f9:
  CYC(0x54f9, 0x54fc); SET_HL(0x5636);
  CYC(0x54fc, 0x54fd); push_effect(gb, 0x54fd);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x54fd, 0x54fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x54fe, 0x54ff); H = mem_rd(gb, HL);
  CYC(0x54ff, 0x5500); L = A;
  CALL_C(0x5500, interactionSetScript_hook, 0x2544, 0x5503);
  CALL_C(0x5503, interactionCode44__initGraphicsAndIncState_hook, 0x550c, 0x5506);
L_5506:
  CALL_C(0x5506, interactionRunScript_hook, 0x2552, 0x5509);
  CYC(0x5509, 0x550c); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__subid4__initialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5506:
  CALL_C(0x5506, interactionRunScript_hook, 0x2552, 0x5509);
  CYC(0x5509, 0x550c); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode44__initGraphicsAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_550c:
  CALL_C(0x550c, interactionInitGraphics_hook, 0x15fb, 0x550f);
  CALL_C(0x550f, objectMarkSolidPosition_hook, 0x24f0, 0x5512);
  CYC(0x5512, 0x5515); if (hook_enabled_at(0x23e0)) { interactionIncState_hook(gb); return; } HANDOFF(0x23e0);
}

void interactionCode44__initGraphicsIncStateAndLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5515:
  CALL_C(0x5515, interactionInitGraphics_hook, 0x15fb, 0x5518);
  CALL_C(0x5518, objectMarkSolidPosition_hook, 0x24f0, 0x551b);
  CYC(0x551b, 0x551d); E = 0x42;
  CYC(0x551d, 0x551e); A = mem_rd(gb, DE);
  CYC(0x551e, 0x5521); SET_HL(0x5620);
  CYC(0x5521, 0x5522); push_effect(gb, 0x5522);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x5522, 0x5523); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5523, 0x5524); H = mem_rd(gb, HL);
  CYC(0x5524, 0x5525); L = A;
  CALL_C(0x5525, interactionSetScript_hook, 0x2544, 0x5528);
  CYC(0x5528, 0x552b); if (hook_enabled_at(0x23e0)) { interactionIncState_hook(gb); return; } HANDOFF(0x23e0);
}

void getGameProgress_1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x552b, 0x552d); B = 0x05;
  CYC(0x552d, 0x552f); A = 0x14;
  CALL_C(0x552f, checkGlobalFlag_hook, 0x31f3, 0x5532);
  if (!(F & FZ)) { CYCT(0x5532, 0x5533); ret_effect(gb); return; } CYC(0x5532, 0x5533);
  CYC(0x5533, 0x5534); B = alu_dec8(gb, B);
  CYC(0x5534, 0x5536); A = 0x13;
  CALL_C(0x5536, checkGlobalFlag_hook, 0x31f3, 0x5539);
  if (!(F & FZ)) { CYCT(0x5539, 0x553a); ret_effect(gb); return; } CYC(0x5539, 0x553a);
  CYC(0x553a, 0x553c); A = 0x40;
  CALL_C(0x553c, checkTreasureObtained_hook, 0x1748, 0x553f);
  if (!(F & FC)) { CYCT(0x553f, 0x5541); goto L_5556; } CYC(0x553f, 0x5541);
  CALL_C(0x5541, getHighestSetBit_hook, 0x01ea, 0x5544);
  CYC(0x5544, 0x5545); C = A;
  CYC(0x5545, 0x5547); B = 0x03;
  CYC(0x5547, 0x5549); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x5549, 0x554a); ret_effect(gb); return; } CYC(0x5549, 0x554a);
  CYC(0x554a, 0x554b); B = alu_dec8(gb, B);
  CYC(0x554b, 0x554d); A = 0x11;
  CALL_C(0x554d, checkGlobalFlag_hook, 0x31f3, 0x5550);
  if (!(F & FZ)) { CYCT(0x5550, 0x5551); ret_effect(gb); return; } CYC(0x5550, 0x5551);
  CYC(0x5551, 0x5552); B = alu_dec8(gb, B);
  CYC(0x5552, 0x5553); A = C;
  CYC(0x5553, 0x5555); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x5555, 0x5556); ret_effect(gb); return; } CYC(0x5555, 0x5556);
L_5556:
  CYC(0x5556, 0x5558); B = 0x00;
  CYC(0x5558, 0x5559); ret_effect(gb); return;
}

void getGameProgress_1__noEssences_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5556:
  CYC(0x5556, 0x5558); B = 0x00;
  CYC(0x5558, 0x5559); ret_effect(gb); return;
}

void getGameProgress_2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5559, 0x555b); B = 0x07;
  CYC(0x555b, 0x555d); A = 0x14;
  CALL_C(0x555d, checkGlobalFlag_hook, 0x31f3, 0x5560);
  if (!(F & FZ)) { CYCT(0x5560, 0x5561); ret_effect(gb); return; } CYC(0x5560, 0x5561);
  CYC(0x5561, 0x5562); B = alu_dec8(gb, B);
  CALL_C(0x5562, checkIsLinkedGame_hook, 0x1992, 0x5565);
  if ((F & FZ)) { CYCT(0x5565, 0x5567); goto L_556d; } CYC(0x5565, 0x5567);
  CYC(0x5567, 0x556a); SET_HL(0xc9fc);
  CYC(0x556a, 0x556c); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x556c, 0x556d); ret_effect(gb); return; } CYC(0x556c, 0x556d);
L_556d:
  CYC(0x556d, 0x556e); B = alu_dec8(gb, B);
  CYC(0x556e, 0x5570); A = 0x13;
  CALL_C(0x5570, checkGlobalFlag_hook, 0x31f3, 0x5573);
  if (!(F & FZ)) { CYCT(0x5573, 0x5574); ret_effect(gb); return; } CYC(0x5573, 0x5574);
  CYC(0x5574, 0x5576); A = 0x40;
  CALL_C(0x5576, checkTreasureObtained_hook, 0x1748, 0x5579);
  if (!(F & FC)) { CYCT(0x5579, 0x557b); goto L_5595; } CYC(0x5579, 0x557b);
  CALL_C(0x557b, getHighestSetBit_hook, 0x01ea, 0x557e);
  CYC(0x557e, 0x557f); C = A;
  CYC(0x557f, 0x5581); B = 0x04;
  CYC(0x5581, 0x5583); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x5583, 0x5584); ret_effect(gb); return; } CYC(0x5583, 0x5584);
  CYC(0x5584, 0x5585); B = alu_dec8(gb, B);
  CYC(0x5585, 0x5587); A = 0x11;
  CALL_C(0x5587, checkGlobalFlag_hook, 0x31f3, 0x558a);
  if (!(F & FZ)) { CYCT(0x558a, 0x558b); ret_effect(gb); return; } CYC(0x558a, 0x558b);
  CYC(0x558b, 0x558c); B = alu_dec8(gb, B);
  CYC(0x558c, 0x558d); A = C;
  CYC(0x558d, 0x558f); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x558f, 0x5590); ret_effect(gb); return; } CYC(0x558f, 0x5590);
  CYC(0x5590, 0x5591); B = alu_dec8(gb, B);
  CYC(0x5591, 0x5592); A = C;
  CYC(0x5592, 0x5594); alu_cp(gb, 0x01);
  if (!(F & FC)) { CYCT(0x5594, 0x5595); ret_effect(gb); return; } CYC(0x5594, 0x5595);
L_5595:
  CYC(0x5595, 0x5597); B = 0x00;
  CYC(0x5597, 0x5598); ret_effect(gb); return;
}

void getGameProgress_2__noEssences_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5595:
  CYC(0x5595, 0x5597); B = 0x00;
  CYC(0x5597, 0x5598); ret_effect(gb); return;
}

void unusedFunc5598_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5598, 0x5599); A = B;
  CYC(0x5599, 0x559c); SET_HL(0x562a);
  CYC(0x559c, 0x559d); push_effect(gb, 0x559d);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x559d, 0x559e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x559e, 0x559f); H = mem_rd(gb, HL);
  CYC(0x559f, 0x55a0); L = A;
  CALL_C(0x55a0, interactionSetScript_hook, 0x2544, 0x55a3);
  CYC(0x55a3, 0x55a6); if (hook_enabled_at(0x23e0)) { interactionIncState_hook(gb); return; } HANDOFF(0x23e0);
}

void checkNpcShouldExistAtGameStage_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55a6, 0x55a9); SET_HL(0x55c0);
  CYC(0x55a9, 0x55aa); push_effect(gb, 0x55aa);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x55aa, 0x55ab); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x55ab, 0x55ac); H = mem_rd(gb, HL);
  CYC(0x55ac, 0x55ad); L = A;
  CYC(0x55ad, 0x55af); E = 0x42;
  CYC(0x55af, 0x55b0); A = mem_rd(gb, DE);
  CYC(0x55b0, 0x55b1); alu_sub(gb, C);
  CYC(0x55b1, 0x55b2); push_effect(gb, 0x55b2);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x55b2, 0x55b3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x55b3, 0x55b4); H = mem_rd(gb, HL);
  CYC(0x55b4, 0x55b5); L = A;
L_55b5:
  CYC(0x55b5, 0x55b6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x55b6, 0x55b7); alu_cp(gb, B);
  if ((F & FZ)) { CYCT(0x55b7, 0x55b8); ret_effect(gb); return; } CYC(0x55b7, 0x55b8);
  CYC(0x55b8, 0x55b9); A = alu_inc8(gb, A);
  if ((F & FZ)) { CYCT(0x55b9, 0x55bb); goto L_55bd; } CYC(0x55b9, 0x55bb);
  CYC(0x55bb, 0x55bd); goto L_55b5;
L_55bd:
  CYC(0x55bd, 0x55bf); alu_or(gb, 0x01);
  CYC(0x55bf, 0x55c0); ret_effect(gb); return;
}
