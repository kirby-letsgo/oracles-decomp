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

void interactionCode43_hook(GB *gb);
void interactionCode43__subid0_hook(GB *gb);
void interactionCode43__afterCall5348_hook(GB *gb);
void interactionCode43__subid1_hook(GB *gb);
void interactionCode43__afterCall5378_hook(GB *gb);
void interactionCode43__label_09_117_hook(GB *gb);
void interactionCode43__subid3_hook(GB *gb);
void interactionCode43__afterCall538b_hook(GB *gb);
void interactionCode43__subid4_hook(GB *gb);
void interactionCode43__afterCall53a1_hook(GB *gb);
void interactionCode43__subid5_hook(GB *gb);
void interactionCode43__afterCall53b0_hook(GB *gb);
void interactionCode43__subid5__initialized_hook(GB *gb);
void interactionCode43__subid5__substate0_hook(GB *gb);
void interactionCode43__subid5__substate1_hook(GB *gb);
void interactionCode43__subid5__substate2_hook(GB *gb);
void interactionCode43__subid6_hook(GB *gb);
void interactionCode43__afterCall5410_hook(GB *gb);
void interactionCode43__subid6__initAndLoadScript_hook(GB *gb);
void interactionCode43__initialized_hook(GB *gb);
void interactionCode43__subid7_hook(GB *gb);
void interactionCode43__afterCall543b_hook(GB *gb);
void interactionCode43__initGraphicsAndIncState_hook(GB *gb);
void interactionCode43__initGraphicsIncStateAndLoadScript_hook(GB *gb);

void interactionCode43_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5310, 0x5312); E = 0x42;
  CYC(0x5312, 0x5313); A = mem_rd(gb, DE);
  CYC(0x5313, 0x5314); push_effect(gb, 0x5314); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x5324: goto L_5324; case 0x534e: goto L_534e; case 0x5383: goto L_5383; case 0x539a: goto L_539a; case 0x53a8: goto L_53a8; case 0x53fa: goto L_53fa; case 0x5431: goto L_5431; default: HANDOFF(HL); }
L_5324:
  CYC(0x5324, 0x5326); A = 0x14;
  CALL_C(0x5326, checkGlobalFlag_hook, 0x31f3, 0x5329);
  if (!(F & FZ)) { CYCT(0x5329, 0x532c); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x5329, 0x532c);
  CYC(0x532c, 0x532e); A = 0x0b;
  CALL_C(0x532e, checkGlobalFlag_hook, 0x31f3, 0x5331);
  CYC(0x5331, 0x5333); E = 0x43;
  CYC(0x5333, 0x5334); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(0x5334, 0x5336); goto L_533c; } CYC(0x5334, 0x5336);
  CYC(0x5336, 0x5337); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5337, 0x533a); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x5337, 0x533a);
  CYC(0x533a, 0x533c); goto L_5340;
L_533c:
  CYC(0x533c, 0x533d); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x533d, 0x5340); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x533d, 0x5340);
L_5340:
  CALL_C(0x5340, checkInteractionState_hook, 0x23fe, 0x5343);
  if (!(F & FZ)) { CYCT(0x5343, 0x5345); goto L_5348; } CYC(0x5343, 0x5345);
  CALL_C(0x5345, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x5348);
L_5348:
  CALL_C(0x5348, interactionRunScript_hook, 0x2552, 0x534b);
  CYC(0x534b, 0x534e); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
L_534e:
  CALL_C(0x534e, checkInteractionState_hook, 0x23fe, 0x5351);
  if (!(F & FZ)) { CYCT(0x5351, 0x5353); goto L_537d; } CYC(0x5351, 0x5353);
  CYC(0x5353, 0x5356); SET_HL(0x5559);
  CYC(0x5356, 0x5358); E = 0x09;
  CALL_C(0x5358, interBankCall_hook, 0x008a, 0x535b);
  CYC(0x535b, 0x535d); C = 0x01;
  CYC(0x535d, 0x535f); A = 0x05;
  CALL_C(0x535f, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x5362);
  if (!(F & FZ)) { CYCT(0x5362, 0x5365); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x5362, 0x5365);
  CYC(0x5365, 0x5366); A = B;
  CYC(0x5366, 0x5369); SET_HL(0x5474);
  CYC(0x5369, 0x536a); push_effect(gb, 0x536a);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x536a, 0x536b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x536b, 0x536c); H = mem_rd(gb, HL);
  CYC(0x536c, 0x536d); L = A;
  CALL_C(0x536d, interactionSetScript_hook, 0x2544, 0x5370);
  CYC(0x5370, 0x5372); A = 0x17;
  CALL_C(0x5372, interactionSetHighTextIndex_hook, 0x253b, 0x5375);
  CALL_C(0x5375, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x5378);
L_5378:
  CYC(0x5378, 0x537a); A = 0x03;
  CYC(0x537a, 0x537c); E = 0x5c;
  CYC(0x537c, 0x537d); mem_wr(gb, DE, A);
L_537d:
  CALL_C(0x537d, interactionRunScript_hook, 0x2552, 0x5380);
  CYC(0x5380, 0x5383); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
L_5383:
  CALL_C(0x5383, checkInteractionState_hook, 0x23fe, 0x5386);
  if (!(F & FZ)) { CYCT(0x5386, 0x5388); goto L_538e; } CYC(0x5386, 0x5388);
  CALL_C(0x5388, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x538b);
L_538b:
  CYC(0x538b, 0x538e); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
L_538e:
  CALL_C(0x538e, interactionRunScript_hook, 0x2552, 0x5391);
  CYC(0x5391, 0x5394); A = mem_rd(gb, 0xcfd1);
  CYC(0x5394, 0x5396); alu_cp(gb, 0x02);
  if ((F & FC)) { CYCT(0x5396, 0x5399); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b); } CYC(0x5396, 0x5399);
  CYC(0x5399, 0x539a); ret_effect(gb); return;
L_539a:
  CALL_C(0x539a, checkInteractionState_hook, 0x23fe, 0x539d);
  if (!(F & FZ)) { CYCT(0x539d, 0x539e); ret_effect(gb); return; } CYC(0x539d, 0x539e);
  CALL_C(0x539e, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x53a1);
L_53a1:
  CYC(0x53a1, 0x53a3); L = 0x5c;
  CYC(0x53a3, 0x53a5); mem_wr(gb, HL, 0x06);
  CYC(0x53a5, 0x53a8); if (hook_enabled_at(0x1e69)) { objectSetVisible82_hook(gb); return; } HANDOFF(0x1e69);
L_53a8:
  CALL_C(0x53a8, checkInteractionState_hook, 0x23fe, 0x53ab);
  if (!(F & FZ)) { CYCT(0x53ab, 0x53ad); goto L_53b7; } CYC(0x53ab, 0x53ad);
  CALL_C(0x53ad, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x53b0);
L_53b0:
  CYC(0x53b0, 0x53b2); L = 0x5c;
  CYC(0x53b2, 0x53b4); mem_wr(gb, HL, 0x06);
  CYC(0x53b4, 0x53b7); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
L_53b7:
  CYC(0x53b7, 0x53b9); E = 0x45;
  CYC(0x53b9, 0x53ba); A = mem_rd(gb, DE);
  CYC(0x53ba, 0x53bb); push_effect(gb, 0x53bb); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x53c1: goto L_53c1; case 0x53cf: goto L_53cf; case 0x53eb: goto L_53eb; default: HANDOFF(HL); }
L_53c1:
  CYC(0x53c1, 0x53c4); A = mem_rd(gb, 0xcfd1);
  CYC(0x53c4, 0x53c6); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x53c6, 0x53c7); ret_effect(gb); return; } CYC(0x53c6, 0x53c7);
  CYC(0x53c7, 0x53ca); SET_HL(0x7b54);
  CYC(0x53ca, 0x53cc); E = 0x08;
  CYC(0x53cc, 0x53cf); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53cf:
  CALL_C(0x53cf, interactionDecCounter1_hook, 0x23cc, 0x53d2);
  if ((F & FZ)) { CYCT(0x53d2, 0x53d4); goto L_53dc; } CYC(0x53d2, 0x53d4);
  CYC(0x53d4, 0x53d7); SET_HL(0x7b70);
  CYC(0x53d7, 0x53d9); E = 0x08;
  CYC(0x53d9, 0x53dc); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(0x53dc, interactionIncSubstate_hook, 0x23e5, 0x53df);
  CYC(0x53df, 0x53e1); L = 0x5c;
  CYC(0x53e1, 0x53e3); mem_wr(gb, HL, 0x02);
  CYC(0x53e3, 0x53e5); L = 0x46;
  CYC(0x53e5, 0x53e7); mem_wr(gb, HL, 0xa4);
  CYC(0x53e7, 0x53e8); L = alu_inc8(gb, L);
  CYC(0x53e8, 0x53ea); mem_wr(gb, HL, 0x01);
  CYC(0x53ea, 0x53eb); ret_effect(gb); return;
L_53eb:
  CALL_C(0x53eb, interactionAnimate_hook, 0x261b, 0x53ee);
  CYC(0x53ee, 0x53f0); L = 0x46;
  CALL_C(0x53f0, decHlRef16WithCap_hook, 0x0237, 0x53f3);
  if (!(F & FZ)) { CYCT(0x53f3, 0x53f4); ret_effect(gb); return; } CYC(0x53f3, 0x53f4);
  CYC(0x53f4, 0x53f6); A = 0xff;
  CYC(0x53f6, 0x53f9); mem_wr(gb, 0xcfdf, A);
  CYC(0x53f9, 0x53fa); ret_effect(gb); return;
L_53fa:
  CALL_C(0x53fa, checkInteractionState_hook, 0x23fe, 0x53fd);
  if (!(F & FZ)) { CYCT(0x53fd, 0x53ff); goto L_5421; } CYC(0x53fd, 0x53ff);
  CYC(0x53ff, 0x5402); SET_HL(0xc9fc);
  CYC(0x5402, 0x5404); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5404, 0x5406); goto L_541e; } CYC(0x5404, 0x5406);
  CYC(0x5406, 0x5409); A = mem_rd(gb, 0xc6bf);
  CYC(0x5409, 0x540b); alu_bit(gb, 6, A);
  if ((F & FZ)) { CYCT(0x540b, 0x540d); goto L_541e; } CYC(0x540b, 0x540d);
  CALL_C(0x540d, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x5410);
L_5410:
  CYC(0x5410, 0x5412); L = 0x5c;
  CYC(0x5412, 0x5414); mem_wr(gb, HL, 0x06);
  CYC(0x5414, 0x5416); L = 0x43;
  CYC(0x5416, 0x5417); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5417, 0x5419); A = 0x06;
  CALL_C(0x5419, objectSetCollideRadius_hook, 0x24a1, 0x541c);
  CYC(0x541c, 0x541e); goto L_5421;
L_541e:
  CALL_C(0x541e, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x5421);
L_5421:
  CYC(0x5421, 0x5423); E = 0x43;
  CYC(0x5423, 0x5424); A = mem_rd(gb, DE);
  CYC(0x5424, 0x5425); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5425, 0x5428); if (hook_enabled_at(0x26de)) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(0x26de); } CYC(0x5425, 0x5428);
  CALL_C(0x5428, interactionRunScript_hook, 0x2552, 0x542b);
  if ((F & FC)) { CYCT(0x542b, 0x542e); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x542b, 0x542e);
  CYC(0x542e, 0x5431); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
L_5431:
  CALL_C(0x5431, checkInteractionState_hook, 0x23fe, 0x5434);
  if ((F & FZ)) { CYCT(0x5434, 0x5437); goto L_5438; } CYC(0x5434, 0x5437);
  CYC(0x5437, 0x5438); ret_effect(gb); return;
L_5438:
  CALL_C(0x5438, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x543b);
L_543b:
  CYC(0x543b, 0x543d); L = 0x5c;
  CYC(0x543d, 0x543f); mem_wr(gb, HL, 0x06);
  CYC(0x543f, 0x5442); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
}

void interactionCode43__subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5324:
  CYC(0x5324, 0x5326); A = 0x14;
  CALL_C(0x5326, checkGlobalFlag_hook, 0x31f3, 0x5329);
  if (!(F & FZ)) { CYCT(0x5329, 0x532c); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x5329, 0x532c);
  CYC(0x532c, 0x532e); A = 0x0b;
  CALL_C(0x532e, checkGlobalFlag_hook, 0x31f3, 0x5331);
  CYC(0x5331, 0x5333); E = 0x43;
  CYC(0x5333, 0x5334); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(0x5334, 0x5336); goto L_533c; } CYC(0x5334, 0x5336);
  CYC(0x5336, 0x5337); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5337, 0x533a); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x5337, 0x533a);
  CYC(0x533a, 0x533c); goto L_5340;
L_533c:
  CYC(0x533c, 0x533d); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x533d, 0x5340); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x533d, 0x5340);
L_5340:
  CALL_C(0x5340, checkInteractionState_hook, 0x23fe, 0x5343);
  if (!(F & FZ)) { CYCT(0x5343, 0x5345); goto L_5348; } CYC(0x5343, 0x5345);
  CALL_C(0x5345, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x5348);
L_5348:
  CALL_C(0x5348, interactionRunScript_hook, 0x2552, 0x534b);
  CYC(0x534b, 0x534e); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__afterCall5348_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5348:
  CALL_C(0x5348, interactionRunScript_hook, 0x2552, 0x534b);
  CYC(0x534b, 0x534e); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_534e:
  CALL_C(0x534e, checkInteractionState_hook, 0x23fe, 0x5351);
  if (!(F & FZ)) { CYCT(0x5351, 0x5353); goto L_537d; } CYC(0x5351, 0x5353);
  CYC(0x5353, 0x5356); SET_HL(0x5559);
  CYC(0x5356, 0x5358); E = 0x09;
  CALL_C(0x5358, interBankCall_hook, 0x008a, 0x535b);
  CYC(0x535b, 0x535d); C = 0x01;
  CYC(0x535d, 0x535f); A = 0x05;
  CALL_C(0x535f, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x5362);
  if (!(F & FZ)) { CYCT(0x5362, 0x5365); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x5362, 0x5365);
  CYC(0x5365, 0x5366); A = B;
  CYC(0x5366, 0x5369); SET_HL(0x5474);
  CYC(0x5369, 0x536a); push_effect(gb, 0x536a);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x536a, 0x536b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x536b, 0x536c); H = mem_rd(gb, HL);
  CYC(0x536c, 0x536d); L = A;
  CALL_C(0x536d, interactionSetScript_hook, 0x2544, 0x5370);
  CYC(0x5370, 0x5372); A = 0x17;
  CALL_C(0x5372, interactionSetHighTextIndex_hook, 0x253b, 0x5375);
  CALL_C(0x5375, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x5378);
L_5378:
  CYC(0x5378, 0x537a); A = 0x03;
  CYC(0x537a, 0x537c); E = 0x5c;
  CYC(0x537c, 0x537d); mem_wr(gb, DE, A);
L_537d:
  CALL_C(0x537d, interactionRunScript_hook, 0x2552, 0x5380);
  CYC(0x5380, 0x5383); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__afterCall5378_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5378:
  CYC(0x5378, 0x537a); A = 0x03;
  CYC(0x537a, 0x537c); E = 0x5c;
  CYC(0x537c, 0x537d); mem_wr(gb, DE, A);
L_537d:
  CALL_C(0x537d, interactionRunScript_hook, 0x2552, 0x5380);
  CYC(0x5380, 0x5383); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__label_09_117_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_537d:
  CALL_C(0x537d, interactionRunScript_hook, 0x2552, 0x5380);
  CYC(0x5380, 0x5383); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__subid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5383:
  CALL_C(0x5383, checkInteractionState_hook, 0x23fe, 0x5386);
  if (!(F & FZ)) { CYCT(0x5386, 0x5388); goto L_538e; } CYC(0x5386, 0x5388);
  CALL_C(0x5388, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x538b);
L_538b:
  CYC(0x538b, 0x538e); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
L_538e:
  CALL_C(0x538e, interactionRunScript_hook, 0x2552, 0x5391);
  CYC(0x5391, 0x5394); A = mem_rd(gb, 0xcfd1);
  CYC(0x5394, 0x5396); alu_cp(gb, 0x02);
  if ((F & FC)) { CYCT(0x5396, 0x5399); if (hook_enabled_at(0x261b)) { interactionAnimate_hook(gb); return; } HANDOFF(0x261b); } CYC(0x5396, 0x5399);
  CYC(0x5399, 0x539a); ret_effect(gb); return;
}

void interactionCode43__afterCall538b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_538b:
  CYC(0x538b, 0x538e); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
}

void interactionCode43__subid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_539a:
  CALL_C(0x539a, checkInteractionState_hook, 0x23fe, 0x539d);
  if (!(F & FZ)) { CYCT(0x539d, 0x539e); ret_effect(gb); return; } CYC(0x539d, 0x539e);
  CALL_C(0x539e, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x53a1);
L_53a1:
  CYC(0x53a1, 0x53a3); L = 0x5c;
  CYC(0x53a3, 0x53a5); mem_wr(gb, HL, 0x06);
  CYC(0x53a5, 0x53a8); if (hook_enabled_at(0x1e69)) { objectSetVisible82_hook(gb); return; } HANDOFF(0x1e69);
}

void interactionCode43__afterCall53a1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53a1:
  CYC(0x53a1, 0x53a3); L = 0x5c;
  CYC(0x53a3, 0x53a5); mem_wr(gb, HL, 0x06);
  CYC(0x53a5, 0x53a8); if (hook_enabled_at(0x1e69)) { objectSetVisible82_hook(gb); return; } HANDOFF(0x1e69);
}

void interactionCode43__subid5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53a8:
  CALL_C(0x53a8, checkInteractionState_hook, 0x23fe, 0x53ab);
  if (!(F & FZ)) { CYCT(0x53ab, 0x53ad); goto L_53b7; } CYC(0x53ab, 0x53ad);
  CALL_C(0x53ad, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x53b0);
L_53b0:
  CYC(0x53b0, 0x53b2); L = 0x5c;
  CYC(0x53b2, 0x53b4); mem_wr(gb, HL, 0x06);
  CYC(0x53b4, 0x53b7); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
L_53b7:
  CYC(0x53b7, 0x53b9); E = 0x45;
  CYC(0x53b9, 0x53ba); A = mem_rd(gb, DE);
  CYC(0x53ba, 0x53bb); push_effect(gb, 0x53bb); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x53c1: goto L_53c1; case 0x53cf: goto L_53cf; case 0x53eb: goto L_53eb; default: HANDOFF(HL); }
L_53c1:
  CYC(0x53c1, 0x53c4); A = mem_rd(gb, 0xcfd1);
  CYC(0x53c4, 0x53c6); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x53c6, 0x53c7); ret_effect(gb); return; } CYC(0x53c6, 0x53c7);
  CYC(0x53c7, 0x53ca); SET_HL(0x7b54);
  CYC(0x53ca, 0x53cc); E = 0x08;
  CYC(0x53cc, 0x53cf); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53cf:
  CALL_C(0x53cf, interactionDecCounter1_hook, 0x23cc, 0x53d2);
  if ((F & FZ)) { CYCT(0x53d2, 0x53d4); goto L_53dc; } CYC(0x53d2, 0x53d4);
  CYC(0x53d4, 0x53d7); SET_HL(0x7b70);
  CYC(0x53d7, 0x53d9); E = 0x08;
  CYC(0x53d9, 0x53dc); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(0x53dc, interactionIncSubstate_hook, 0x23e5, 0x53df);
  CYC(0x53df, 0x53e1); L = 0x5c;
  CYC(0x53e1, 0x53e3); mem_wr(gb, HL, 0x02);
  CYC(0x53e3, 0x53e5); L = 0x46;
  CYC(0x53e5, 0x53e7); mem_wr(gb, HL, 0xa4);
  CYC(0x53e7, 0x53e8); L = alu_inc8(gb, L);
  CYC(0x53e8, 0x53ea); mem_wr(gb, HL, 0x01);
  CYC(0x53ea, 0x53eb); ret_effect(gb); return;
L_53eb:
  CALL_C(0x53eb, interactionAnimate_hook, 0x261b, 0x53ee);
  CYC(0x53ee, 0x53f0); L = 0x46;
  CALL_C(0x53f0, decHlRef16WithCap_hook, 0x0237, 0x53f3);
  if (!(F & FZ)) { CYCT(0x53f3, 0x53f4); ret_effect(gb); return; } CYC(0x53f3, 0x53f4);
  CYC(0x53f4, 0x53f6); A = 0xff;
  CYC(0x53f6, 0x53f9); mem_wr(gb, 0xcfdf, A);
  CYC(0x53f9, 0x53fa); ret_effect(gb); return;
}

void interactionCode43__afterCall53b0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53b0:
  CYC(0x53b0, 0x53b2); L = 0x5c;
  CYC(0x53b2, 0x53b4); mem_wr(gb, HL, 0x06);
  CYC(0x53b4, 0x53b7); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
}

void interactionCode43__subid5__initialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53b7:
  CYC(0x53b7, 0x53b9); E = 0x45;
  CYC(0x53b9, 0x53ba); A = mem_rd(gb, DE);
  CYC(0x53ba, 0x53bb); push_effect(gb, 0x53bb); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x53c1: goto L_53c1; case 0x53cf: goto L_53cf; case 0x53eb: goto L_53eb; default: HANDOFF(HL); }
L_53c1:
  CYC(0x53c1, 0x53c4); A = mem_rd(gb, 0xcfd1);
  CYC(0x53c4, 0x53c6); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x53c6, 0x53c7); ret_effect(gb); return; } CYC(0x53c6, 0x53c7);
  CYC(0x53c7, 0x53ca); SET_HL(0x7b54);
  CYC(0x53ca, 0x53cc); E = 0x08;
  CYC(0x53cc, 0x53cf); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53cf:
  CALL_C(0x53cf, interactionDecCounter1_hook, 0x23cc, 0x53d2);
  if ((F & FZ)) { CYCT(0x53d2, 0x53d4); goto L_53dc; } CYC(0x53d2, 0x53d4);
  CYC(0x53d4, 0x53d7); SET_HL(0x7b70);
  CYC(0x53d7, 0x53d9); E = 0x08;
  CYC(0x53d9, 0x53dc); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(0x53dc, interactionIncSubstate_hook, 0x23e5, 0x53df);
  CYC(0x53df, 0x53e1); L = 0x5c;
  CYC(0x53e1, 0x53e3); mem_wr(gb, HL, 0x02);
  CYC(0x53e3, 0x53e5); L = 0x46;
  CYC(0x53e5, 0x53e7); mem_wr(gb, HL, 0xa4);
  CYC(0x53e7, 0x53e8); L = alu_inc8(gb, L);
  CYC(0x53e8, 0x53ea); mem_wr(gb, HL, 0x01);
  CYC(0x53ea, 0x53eb); ret_effect(gb); return;
L_53eb:
  CALL_C(0x53eb, interactionAnimate_hook, 0x261b, 0x53ee);
  CYC(0x53ee, 0x53f0); L = 0x46;
  CALL_C(0x53f0, decHlRef16WithCap_hook, 0x0237, 0x53f3);
  if (!(F & FZ)) { CYCT(0x53f3, 0x53f4); ret_effect(gb); return; } CYC(0x53f3, 0x53f4);
  CYC(0x53f4, 0x53f6); A = 0xff;
  CYC(0x53f6, 0x53f9); mem_wr(gb, 0xcfdf, A);
  CYC(0x53f9, 0x53fa); ret_effect(gb); return;
}

void interactionCode43__subid5__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53c1:
  CYC(0x53c1, 0x53c4); A = mem_rd(gb, 0xcfd1);
  CYC(0x53c4, 0x53c6); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x53c6, 0x53c7); ret_effect(gb); return; } CYC(0x53c6, 0x53c7);
  CYC(0x53c7, 0x53ca); SET_HL(0x7b54);
  CYC(0x53ca, 0x53cc); E = 0x08;
  CYC(0x53cc, 0x53cf); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
}

void interactionCode43__subid5__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53cf:
  CALL_C(0x53cf, interactionDecCounter1_hook, 0x23cc, 0x53d2);
  if ((F & FZ)) { CYCT(0x53d2, 0x53d4); goto L_53dc; } CYC(0x53d2, 0x53d4);
  CYC(0x53d4, 0x53d7); SET_HL(0x7b70);
  CYC(0x53d7, 0x53d9); E = 0x08;
  CYC(0x53d9, 0x53dc); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(0x53dc, interactionIncSubstate_hook, 0x23e5, 0x53df);
  CYC(0x53df, 0x53e1); L = 0x5c;
  CYC(0x53e1, 0x53e3); mem_wr(gb, HL, 0x02);
  CYC(0x53e3, 0x53e5); L = 0x46;
  CYC(0x53e5, 0x53e7); mem_wr(gb, HL, 0xa4);
  CYC(0x53e7, 0x53e8); L = alu_inc8(gb, L);
  CYC(0x53e8, 0x53ea); mem_wr(gb, HL, 0x01);
  CYC(0x53ea, 0x53eb); ret_effect(gb); return;
}

void interactionCode43__subid5__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53eb:
  CALL_C(0x53eb, interactionAnimate_hook, 0x261b, 0x53ee);
  CYC(0x53ee, 0x53f0); L = 0x46;
  CALL_C(0x53f0, decHlRef16WithCap_hook, 0x0237, 0x53f3);
  if (!(F & FZ)) { CYCT(0x53f3, 0x53f4); ret_effect(gb); return; } CYC(0x53f3, 0x53f4);
  CYC(0x53f4, 0x53f6); A = 0xff;
  CYC(0x53f6, 0x53f9); mem_wr(gb, 0xcfdf, A);
  CYC(0x53f9, 0x53fa); ret_effect(gb); return;
}

void interactionCode43__subid6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53fa:
  CALL_C(0x53fa, checkInteractionState_hook, 0x23fe, 0x53fd);
  if (!(F & FZ)) { CYCT(0x53fd, 0x53ff); goto L_5421; } CYC(0x53fd, 0x53ff);
  CYC(0x53ff, 0x5402); SET_HL(0xc9fc);
  CYC(0x5402, 0x5404); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5404, 0x5406); goto L_541e; } CYC(0x5404, 0x5406);
  CYC(0x5406, 0x5409); A = mem_rd(gb, 0xc6bf);
  CYC(0x5409, 0x540b); alu_bit(gb, 6, A);
  if ((F & FZ)) { CYCT(0x540b, 0x540d); goto L_541e; } CYC(0x540b, 0x540d);
  CALL_C(0x540d, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x5410);
L_5410:
  CYC(0x5410, 0x5412); L = 0x5c;
  CYC(0x5412, 0x5414); mem_wr(gb, HL, 0x06);
  CYC(0x5414, 0x5416); L = 0x43;
  CYC(0x5416, 0x5417); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5417, 0x5419); A = 0x06;
  CALL_C(0x5419, objectSetCollideRadius_hook, 0x24a1, 0x541c);
  CYC(0x541c, 0x541e); goto L_5421;
L_541e:
  CALL_C(0x541e, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x5421);
L_5421:
  CYC(0x5421, 0x5423); E = 0x43;
  CYC(0x5423, 0x5424); A = mem_rd(gb, DE);
  CYC(0x5424, 0x5425); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5425, 0x5428); if (hook_enabled_at(0x26de)) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(0x26de); } CYC(0x5425, 0x5428);
  CALL_C(0x5428, interactionRunScript_hook, 0x2552, 0x542b);
  if ((F & FC)) { CYCT(0x542b, 0x542e); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x542b, 0x542e);
  CYC(0x542e, 0x5431); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__afterCall5410_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5410:
  CYC(0x5410, 0x5412); L = 0x5c;
  CYC(0x5412, 0x5414); mem_wr(gb, HL, 0x06);
  CYC(0x5414, 0x5416); L = 0x43;
  CYC(0x5416, 0x5417); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5417, 0x5419); A = 0x06;
  CALL_C(0x5419, objectSetCollideRadius_hook, 0x24a1, 0x541c);
  CYC(0x541c, 0x541e); goto L_5421;
L_5421:
  CYC(0x5421, 0x5423); E = 0x43;
  CYC(0x5423, 0x5424); A = mem_rd(gb, DE);
  CYC(0x5424, 0x5425); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5425, 0x5428); if (hook_enabled_at(0x26de)) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(0x26de); } CYC(0x5425, 0x5428);
  CALL_C(0x5428, interactionRunScript_hook, 0x2552, 0x542b);
  if ((F & FC)) { CYCT(0x542b, 0x542e); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x542b, 0x542e);
  CYC(0x542e, 0x5431); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__subid6__initAndLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_541e:
  CALL_C(0x541e, interactionCode43__initGraphicsIncStateAndLoadScript_hook, 0x544b, 0x5421);
L_5421:
  CYC(0x5421, 0x5423); E = 0x43;
  CYC(0x5423, 0x5424); A = mem_rd(gb, DE);
  CYC(0x5424, 0x5425); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5425, 0x5428); if (hook_enabled_at(0x26de)) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(0x26de); } CYC(0x5425, 0x5428);
  CALL_C(0x5428, interactionRunScript_hook, 0x2552, 0x542b);
  if ((F & FC)) { CYCT(0x542b, 0x542e); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x542b, 0x542e);
  CYC(0x542e, 0x5431); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__initialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5421:
  CYC(0x5421, 0x5423); E = 0x43;
  CYC(0x5423, 0x5424); A = mem_rd(gb, DE);
  CYC(0x5424, 0x5425); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5425, 0x5428); if (hook_enabled_at(0x26de)) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(0x26de); } CYC(0x5425, 0x5428);
  CALL_C(0x5428, interactionRunScript_hook, 0x2552, 0x542b);
  if ((F & FC)) { CYCT(0x542b, 0x542e); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x542b, 0x542e);
  CYC(0x542e, 0x5431); if (hook_enabled_at(0x26db)) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(0x26db);
}

void interactionCode43__subid7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5431:
  CALL_C(0x5431, checkInteractionState_hook, 0x23fe, 0x5434);
  if ((F & FZ)) { CYCT(0x5434, 0x5437); goto L_5438; } CYC(0x5434, 0x5437);
  CYC(0x5437, 0x5438); ret_effect(gb); return;
L_5438:
  CALL_C(0x5438, interactionCode43__initGraphicsAndIncState_hook, 0x5442, 0x543b);
L_543b:
  CYC(0x543b, 0x543d); L = 0x5c;
  CYC(0x543d, 0x543f); mem_wr(gb, HL, 0x06);
  CYC(0x543f, 0x5442); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
}

void interactionCode43__afterCall543b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_543b:
  CYC(0x543b, 0x543d); L = 0x5c;
  CYC(0x543d, 0x543f); mem_wr(gb, HL, 0x06);
  CYC(0x543f, 0x5442); if (hook_enabled_at(0x1e45)) { objectSetVisiblec2_hook(gb); return; } HANDOFF(0x1e45);
}

void interactionCode43__initGraphicsAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5442:
  CALL_C(0x5442, interactionInitGraphics_hook, 0x15fb, 0x5445);
  CALL_C(0x5445, objectMarkSolidPosition_hook, 0x24f0, 0x5448);
  CYC(0x5448, 0x544b); if (hook_enabled_at(0x23e0)) { interactionIncState_hook(gb); return; } HANDOFF(0x23e0);
}

void interactionCode43__initGraphicsIncStateAndLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_544b:
  CALL_C(0x544b, interactionInitGraphics_hook, 0x15fb, 0x544e);
  CALL_C(0x544e, objectMarkSolidPosition_hook, 0x24f0, 0x5451);
  CYC(0x5451, 0x5453); A = 0x17;
  CALL_C(0x5453, interactionSetHighTextIndex_hook, 0x253b, 0x5456);
  CYC(0x5456, 0x5458); E = 0x42;
  CYC(0x5458, 0x5459); A = mem_rd(gb, DE);
  CYC(0x5459, 0x545c); SET_HL(0x5466);
  CYC(0x545c, 0x545d); push_effect(gb, 0x545d);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(0x545d, 0x545e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x545e, 0x545f); H = mem_rd(gb, HL);
  CYC(0x545f, 0x5460); L = A;
  CALL_C(0x5460, interactionSetScript_hook, 0x2544, 0x5463);
  CYC(0x5463, 0x5466); if (hook_enabled_at(0x23e0)) { interactionIncState_hook(gb); return; } HANDOFF(0x23e0);
}
