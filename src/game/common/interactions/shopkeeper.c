#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t shopkeeper_jump_table(GB *gb) {
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

void checkReloadShopItemTiles_hook(GB *gb);
void interactionCode46_hook(GB *gb);
void shopkeeperState0_hook(GB *gb);
void shopkeeperState1_hook(GB *gb);
void shopkeeperState2_hook(GB *gb);
void shopkeeperState3_hook(GB *gb);
void shopkeeperState5_hook(GB *gb);
void shopkeeperState6_hook(GB *gb);
void shopkeeperLoadScript_hook(GB *gb);
void shopkeeperPromptChestGame_hook(GB *gb);
void shopkeeperGotoState1_hook(GB *gb);
void shopkeeperGetItemPrice_hook(GB *gb);
void shopkeeperCloseOpenedChest_hook(GB *gb);
void shopkeeperCheckLinkHasItemAlready_hook(GB *gb);
void shopkeeperCheckAllItemsBought_hook(GB *gb);
void shopkeeperTurnToFaceLink_hook(GB *gb);

void checkReloadShopItemTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4000, 0x4003); A = mem_rd(gb, 0xcd00);
  CYC(0x4003, 0x4005); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4005, 0x4006); ret_effect(gb); return; } CYC(0x4005, 0x4006);
  CYC(0x4006, 0x4009); SET_HL(0xccd3);
  CYC(0x4009, 0x400b); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x400b, 0x400c); ret_effect(gb); return; } CYC(0x400b, 0x400c);
  CYC(0x400c, 0x400e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 2)));
  CYC(0x400e, 0x400f); push_effect(gb, DE);
  CYC(0x400f, 0x4011); A = 0x11;
  CALL_C(0x4011, loadUncompressedGfxHeader_hook, 0x05da, 0x4014);
  CYC(0x4014, 0x4015); SET_DE(pop_effect(gb));
  CYC(0x4015, 0x4016); ret_effect(gb);
}

void interactionCode46_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4016, checkReloadShopItemTiles_hook, 0x4000, 0x4019);
  CYC(0x4019, 0x401c); push_effect(gb, 0x401c);
  CYC(0x401f, 0x4021); E = 0x44;
  CYC(0x4021, 0x4022); A = mem_rd(gb, DE);
  CYC(0x4022, 0x4023); push_effect(gb, 0x4023); SET_HL(shopkeeper_jump_table(gb));
  switch (HL) {
    case 0x4031: shopkeeperState0_hook(gb); break;
    case 0x4072: shopkeeperState1_hook(gb); break;
    case 0x40fb: shopkeeperState2_hook(gb); break;
    case 0x4148: shopkeeperState3_hook(gb); break;
    case 0x41ae: shopkeeperState5_hook(gb); break;
    case 0x40d8: shopkeeperState6_hook(gb); break;
    default: hook_continue(gb, HL, gb->sp); return;
  }
  CYC(0x401c, 0x401f); interactionAnimateAsNpc_hook(gb);
}

void shopkeeperState6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x40d8, 0x40da); E = 0x71;
  CYC(0x40da, 0x40db); A = mem_rd(gb, DE);
  CYC(0x40db, 0x40dc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x40dc, 0x40de); goto L_40e9; } CYC(0x40dc, 0x40de);
  CYC(0x40de, 0x40e1); SET_HL(0xd00d);
  CYC(0x40e1, 0x40e3); E = 0x4d;
  CYC(0x40e3, 0x40e4); A = mem_rd(gb, DE);
  CYC(0x40e4, 0x40e5); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x40e5, 0x40e6); ret_effect(gb); return; } CYC(0x40e5, 0x40e6);
  CYC(0x40e6, 0x40e9); shopkeeperGotoState1_hook(gb); return;
L_40e9:
  CYC(0x40e9, 0x40ea); alu_xor(gb, A);
  CYC(0x40ea, 0x40eb); mem_wr(gb, DE, A);
  CALL_C(0x40eb, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x40ee);
  CYC(0x40ee, 0x40f0); A = 0x81;
  CYC(0x40f0, 0x40f3); mem_wr(gb, 0xcc8a, A);
  CYC(0x40f3, 0x40f5); E = 0x44;
  CYC(0x40f5, 0x40f7); A = 2;
  CYC(0x40f7, 0x40f8); mem_wr(gb, DE, A);
  CYC(0x40f8, 0x40fb); shopkeeperTurnToFaceLink_hook(gb);
}

void shopkeeperState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x40fb, 0x40fd); E = 0x42;
  CYC(0x40fd, 0x40fe); A = mem_rd(gb, DE);
  CYC(0x40fe, 0x4100); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x4100, 0x4102); shopkeeperPromptChestGame_hook(gb); return; } CYC(0x4100, 0x4102);
  CYC(0x4102, 0x4105); A = mem_rd(gb, 0xcc5a);
  CYC(0x4105, 0x4106); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4106, 0x4108); goto L_4124; } CYC(0x4106, 0x4108);
  CYC(0x4108, 0x410b); A = mem_rd(gb, 0xd019);
  CYC(0x410b, 0x410c); H = A;
  CYC(0x410c, 0x410e); E = 0x7b;
  CYC(0x410e, 0x410f); mem_wr(gb, DE, A);
  CYC(0x410f, 0x4111); L = 0x42;
  CYC(0x4111, 0x4112); A = mem_rd(gb, HL);
  CYC(0x4112, 0x4114); E = 0x77;
  CYC(0x4114, 0x4115); mem_wr(gb, DE, A);
  CALL_C(0x4115, shopkeeperGetItemPrice_hook, 0x424e, 0x4118);
  CYC(0x4118, 0x411a); E = 0x77;
  CYC(0x411a, 0x411b); A = mem_rd(gb, DE);
  CALL_C(0x411b, shopkeeperCheckLinkHasItemAlready_hook, 0x426f, 0x411e);
  CYC(0x411e, 0x4121); SET_HL(0x47c3);
  CYC(0x4121, 0x4124); shopkeeperLoadScript_hook(gb); return;
L_4124:
  CALL_C(0x4124, shopkeeperCheckAllItemsBought_hook, 0x42ae, 0x4127);
  if (!(F & FZ)) { CYCT(0x4127, 0x4129); shopkeeperLoadScript_hook(gb); return; } CYC(0x4127, 0x4129);
  CYC(0x4129, 0x412b); E = 0x42;
  CYC(0x412b, 0x412c); A = mem_rd(gb, DE);
  CYC(0x412c, 0x412e); alu_cp(gb, 2);
  CYC(0x412e, 0x4131); SET_HL(0x47ba);
  if (!(F & FZ)) { CYCT(0x4131, 0x4133); shopkeeperLoadScript_hook(gb); return; } CYC(0x4131, 0x4133);
  CYC(0x4133, 0x4136); SET_HL(0x47bd);
  shopkeeperLoadScript_hook(gb);
}

void shopkeeperLoadScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4136, 0x4138); E = 0x44;
  CYC(0x4138, 0x413a); A = 4;
  CYC(0x413a, 0x413b); mem_wr(gb, DE, A);
  CYC(0x413b, 0x413e); interactionSetScript_hook(gb);
}

void shopkeeperPromptChestGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x413e, 0x4140); A = 0x0c;
  CALL_C(0x4140, shopkeeperGetItemPrice_hook, 0x424e, 0x4143);
  CYC(0x4143, 0x4146); SET_HL(0x48e9);
  CYC(0x4146, 0x4148); shopkeeperLoadScript_hook(gb);
}

void shopkeeperState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4148, 0x414a); E = 0x42;
  CYC(0x414a, 0x414b); A = mem_rd(gb, DE);
  CYC(0x414b, 0x414d); alu_and(gb, 0x80);
  CYC(0x414d, 0x414f); A = 0x0c;
  if (!(F & FZ)) CALL_C_CC(0x414f, shopkeeperGetItemPrice_hook, 0x424e, 0x4152); else CYC(0x414f, 0x4152);
  CALL_C(0x4152, interactionRunScript_hook, 0x2552, 0x4155);
  if (!(F & FC)) { CYCT(0x4155, 0x4156); ret_effect(gb); return; } CYC(0x4155, 0x4156);
  CYC(0x4156, 0x4157); alu_xor(gb, A);
  CYC(0x4157, 0x415a); mem_wr(gb, 0xcc8a, A);
  CYC(0x415a, 0x415c); E = 0x7f;
  CYC(0x415c, 0x415d); A = mem_rd(gb, DE);
  CYC(0x415d, 0x415e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x415e, 0x4160); goto L_4173; } CYC(0x415e, 0x4160);
  CYC(0x4160, 0x4161); C = A;
  CYC(0x4161, 0x4162); alu_xor(gb, A);
  CYC(0x4162, 0x4163); mem_wr(gb, DE, A);
  CALL_C(0x4163, getRandomRingOfGivenTier_hook, 0x17e0, 0x4166);
  CYC(0x4166, 0x4167); B = C;
  CYC(0x4167, 0x4169); C = 0;
  CALL_C(0x4169, giveRingToLink_hook, 0x27b4, 0x416c);
  CYC(0x416c, 0x416e); A = 1;
  CYC(0x416e, 0x4171); mem_wr(gb, 0xcc8a, A);
  CYC(0x4171, 0x4173); shopkeeperGotoState1_hook(gb); return;
L_4173:
  CYC(0x4173, 0x4175); E = 0x7a;
  CYC(0x4175, 0x4176); A = mem_rd(gb, DE);
  CYC(0x4176, 0x4177); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4177, 0x4179); shopkeeperGotoState1_hook(gb); return; } CYC(0x4177, 0x4179);
  CYC(0x4179, 0x417a); A = alu_inc8(gb, A);
  CYC(0x417a, 0x417c); C = 4;
  if (F & FZ) { CYCT(0x417c, 0x417e); goto L_4185; } CYC(0x417c, 0x417e);
  CYC(0x417e, 0x4180); C = 3;
  CYC(0x4180, 0x4182); A = 0x81;
  CYC(0x4182, 0x4185); mem_wr(gb, 0xcc8a, A);
L_4185:
  CYC(0x4185, 0x4186); alu_xor(gb, A);
  CYC(0x4186, 0x4187); mem_wr(gb, DE, A);
  CYC(0x4187, 0x4189); E = 0x7b;
  CYC(0x4189, 0x418a); A = mem_rd(gb, DE);
  CYC(0x418a, 0x418b); H = A;
  CYC(0x418b, 0x418d); L = 0x44;
  CYC(0x418d, 0x418e); mem_wr(gb, HL, C);
  CALL_C(0x418e, dropLinkHeldItem_hook, 0x2c43, 0x4191);
  shopkeeperGotoState1_hook(gb);
}

void shopkeeperGotoState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4191, 0x4193); E = 0x44;
  CYC(0x4193, 0x4195); A = 1;
  CYC(0x4195, 0x4196); mem_wr(gb, DE, A);
  CYC(0x4196, 0x4199); SET_BC(0x0614);
  CALL_C(0x4199, objectSetCollideRadii_hook, 0x24a9, 0x419c);
  CYC(0x419c, 0x419e); E = 0x42;
  CYC(0x419e, 0x419f); A = mem_rd(gb, DE);
  CYC(0x419f, 0x41a0); alu_or(gb, A);
  CYC(0x41a0, 0x41a2); A = 3;
  if (F & FZ) { CYCT(0x41a2, 0x41a4); goto L_41a6; } CYC(0x41a2, 0x41a4);
  CYC(0x41a4, 0x41a6); A = 1;
L_41a6:
  CALL_C(0x41a6, interactionSetAnimation_hook, 0x262e, 0x41a9);
  CYC(0x41a9, 0x41ab); E = 0x71;
  CYC(0x41ab, 0x41ae); objectAddToAButtonSensitiveObjectList_hook(gb);
}

void shopkeeperState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x41ae, 0x41b0); E = 0x45;
  CYC(0x41b0, 0x41b1); A = mem_rd(gb, DE);
  CYC(0x41b1, 0x41b2); push_effect(gb, 0x41b2); SET_HL(shopkeeper_jump_table(gb));
  switch (HL) { case 0x41ba: goto L_41ba; case 0x41d0: goto L_41d0; case 0x4231: goto L_4231; case 0x4246: goto L_4246; default: HANDOFF(HL); }
L_41ba:
  CYC(0x41ba, 0x41bc); A = 1;
  CYC(0x41bc, 0x41bd); mem_wr(gb, DE, A);
  CALL_C(0x41bd, getRandomNumber_hook, 0x043e, 0x41c0);
  CYC(0x41c0, 0x41c2); alu_and(gb, 1);
  CYC(0x41c2, 0x41c4); E = 0x79;
  CYC(0x41c4, 0x41c5); mem_wr(gb, DE, A);
  CALL_C(0x41c5, shopkeeperCloseOpenedChest_hook, 0x4263, 0x41c8);
  CYC(0x41c8, 0x41c9); alu_xor(gb, A);
  CYC(0x41c9, 0x41cc); mem_wr(gb, 0xcca2, A);
  CYC(0x41cc, 0x41ce); E = 0x7f;
  CYC(0x41ce, 0x41cf); mem_wr(gb, DE, A);
  CYC(0x41cf, 0x41d0); ret_effect(gb); return;
L_41d0:
  CYC(0x41d0, 0x41d2); E = 0x71;
  CYC(0x41d2, 0x41d3); A = mem_rd(gb, DE);
  CYC(0x41d3, 0x41d4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x41d4, 0x41d6); goto L_41de; } CYC(0x41d4, 0x41d6);
  CYC(0x41d6, 0x41d7); alu_xor(gb, A);
  CYC(0x41d7, 0x41d8); mem_wr(gb, DE, A);
  CYC(0x41d8, 0x41db); SET_HL(0x499d);
  CYC(0x41db, 0x41de); shopkeeperLoadScript_hook(gb); return;
L_41de:
  CYC(0x41de, 0x41e1); A = mem_rd(gb, 0xcca2);
  CYC(0x41e1, 0x41e2); alu_or(gb, A);
  if (F & FZ) { CYCT(0x41e2, 0x41e3); ret_effect(gb); return; } CYC(0x41e2, 0x41e3);
  CYC(0x41e3, 0x41e5); E = 0x45;
  CYC(0x41e5, 0x41e6); alu_xor(gb, A);
  CYC(0x41e6, 0x41e7); mem_wr(gb, DE, A);
  CYC(0x41e7, 0x41e9); A = 0xf1;
  CALL_C(0x41e9, findTileInRoom_hook, 0x15cc, 0x41ec);
  CYC(0x41ec, 0x41ef); A = mem_rd(gb, 0xcca2);
  CYC(0x41ef, 0x41f0); alu_sub(gb, L);
  CYC(0x41f0, 0x41f1); alu_rlca(gb);
  CYC(0x41f1, 0x41f3); alu_xor(gb, 1);
  CYC(0x41f3, 0x41f5); alu_and(gb, 1);
  CYC(0x41f5, 0x41f6); H = D;
  CYC(0x41f6, 0x41f8); L = 0x79;
  CYC(0x41f8, 0x41f9); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x41f9, 0x41fb); L = 0x7c;
  if (!(F & FZ)) { CYCT(0x41fb, 0x41fd); goto L_4204; } CYC(0x41fb, 0x41fd);
  CYC(0x41fd, 0x41fe); mem_wr(gb, HL, A);
  CYC(0x41fe, 0x4201); SET_HL(0x493c);
  CYC(0x4201, 0x4204); shopkeeperLoadScript_hook(gb); return;
L_4204:
  CYC(0x4204, 0x4205); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4205, 0x4206); mem_wr(gb, HL, A);
  CALL_C(0x4206, getFreeInteractionSlot_hook, 0x3aef, 0x4209);
  CYC(0x4209, 0x420b); mem_wr(gb, HL, 0x60);
  CYC(0x420b, 0x420d); L = 0x42;
  CYC(0x420d, 0x420f); mem_wr(gb, HL, 0x28);
  CYC(0x420f, 0x4210); L = alu_inc8(gb, L);
  CYC(0x4210, 0x4212); mem_wr(gb, HL, 8);
  CYC(0x4212, 0x4214); L = 0x71;
  CYC(0x4214, 0x4216); mem_wr(gb, HL, 3);
  CYC(0x4216, 0x4218); L = 0x79;
  CYC(0x4218, 0x421a); mem_wr(gb, HL, 1);
  CYC(0x421a, 0x421c); E = 0x79;
  CYC(0x421c, 0x421d); A = mem_rd(gb, DE);
  CYC(0x421d, 0x4220); SET_BC(0x42da);
  CALL_C(0x4220, addAToBc_hook, 0x006d, 0x4223);
  CYC(0x4223, 0x4225); L = 0x4b;
  CYC(0x4225, 0x4227); mem_wr(gb, HL, 0x20);
  CYC(0x4227, 0x4229); L = 0x4d;
  CYC(0x4229, 0x422a); A = mem_rd(gb, BC);
  CYC(0x422a, 0x422b); mem_wr(gb, HL, A);
  CYC(0x422b, 0x422e); SET_HL(0x4950);
  CYC(0x422e, 0x4231); shopkeeperLoadScript_hook(gb); return;
L_4231:
  CYC(0x4231, 0x4233); E = 0x49;
  CYC(0x4233, 0x4234); A = mem_rd(gb, DE);
  CYC(0x4234, 0x4236); A = alu_swap(gb, A);
  CYC(0x4236, 0x4238); alu_and(gb, 1);
  CYC(0x4238, 0x4239); H = D;
  CYC(0x4239, 0x423b); L = 0x79;
  CYC(0x423b, 0x423c); alu_xor(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x423c, 0x423e); goto L_4246; } CYC(0x423c, 0x423e);
  CALL_C(0x423e, shopkeeperCloseOpenedChest_hook, 0x4263, 0x4241);
  CYC(0x4241, 0x4243); E = 0x45;
  CYC(0x4243, 0x4245); A = 3;
  CYC(0x4245, 0x4246); mem_wr(gb, DE, A);
L_4246:
  CALL_C(0x4246, interactionRunScript_hook, 0x2552, 0x4249);
  if (!(F & FC)) { CYCT(0x4249, 0x424a); ret_effect(gb); return; } CYC(0x4249, 0x424a);
  CYC(0x424a, 0x424c); E = 0x45;
  CYC(0x424c, 0x424d); alu_xor(gb, A);
  CYC(0x424d, 0x424e); mem_wr(gb, DE, A);
  shopkeeperGetItemPrice_hook(gb);
}

void shopkeeperGetItemPrice_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x424e, 0x4251); SET_HL(0x44ba);
  CYC(0x4251, 0x4252); push_effect(gb, 0x4252);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) { burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); }
  else { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb); }
  CYC(0x4252, 0x4253); A = mem_rd(gb, HL);
  CALL_C(0x4253, cpRupeeValue_hook, 0x1765, 0x4256);
  CYC(0x4256, 0x4259); mem_wr(gb, 0xccd5, A);
  CYC(0x4259, 0x425c); mem_wr(gb, 0xcbad, A);
  CYC(0x425c, 0x425f); SET_HL(0xcba8);
  CYC(0x425f, 0x4260); mem_wr(gb, HL, C);
  CYC(0x4260, 0x4261); L = alu_inc8(gb, L);
  CYC(0x4261, 0x4262); mem_wr(gb, HL, B);
  CYC(0x4262, 0x4263); ret_effect(gb);
}

void shopkeeperCloseOpenedChest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4263, 0x4266); A = mem_rd(gb, 0xcca2);
  CYC(0x4266, 0x4268); alu_bit(gb, 7, A);
  CYC(0x4268, 0x4269); C = A;
  CYC(0x4269, 0x426b); A = 0xf1;
  if (F & FZ) { CYCT(0x426b, 0x426e); setTile_hook(gb); return; } CYC(0x426b, 0x426e);
  CYC(0x426e, 0x426f); ret_effect(gb);
}

void shopkeeperCheckLinkHasItemAlready_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x426f, 0x4270); B = A;
  CYC(0x4270, 0x4271); alu_xor(gb, A);
  CYC(0x4271, 0x4273); E = 0x78;
  CYC(0x4273, 0x4274); mem_wr(gb, DE, A);
  CYC(0x4274, 0x4276); E = 0x42;
  CYC(0x4276, 0x4277); A = mem_rd(gb, DE);
  CYC(0x4277, 0x4279); E = 0x78;
  CYC(0x4279, 0x427a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x427a, 0x427b); ret_effect(gb); return; } CYC(0x427a, 0x427b);
  CYC(0x427b, 0x427d); H = 0xc6;
  CYC(0x427d, 0x427e); A = B;
  CYC(0x427e, 0x4280); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(0x4280, 0x4281); ret_effect(gb); return; } CYC(0x4280, 0x4281);
  CYC(0x4281, 0x4283); alu_cp(gb, 3);
  if (F & FZ) { CYCT(0x4283, 0x4285); goto L_42a0; } CYC(0x4283, 0x4285);
  CYC(0x4285, 0x4287); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(0x4287, 0x4289); goto L_42a0; } CYC(0x4287, 0x4289);
  CYC(0x4289, 0x428b); alu_cp(gb, 0x12);
  if (F & FZ) { CYCT(0x428b, 0x428d); goto L_42a0; } CYC(0x428b, 0x428d);
  CYC(0x428d, 0x428f); alu_cp(gb, 0x0d);
  if (F & FZ) { CYCT(0x428f, 0x4291); goto L_42a4; } CYC(0x428f, 0x4291);
  CYC(0x4291, 0x4293); L = 0xb0;
  CYC(0x4293, 0x4295); alu_cp(gb, 4);
  if (F & FZ) { CYCT(0x4295, 0x4297); goto L_4299; } CYC(0x4295, 0x4297);
  CYC(0x4297, 0x4299); L = 0xaa;
L_4299:
  CYC(0x4299, 0x429a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x429a, 0x429b); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x429b, 0x429c); ret_effect(gb); return; } CYC(0x429b, 0x429c);
L_429c:
  CYC(0x429c, 0x429e); A = 1;
  CYC(0x429e, 0x429f); mem_wr(gb, DE, A);
  CYC(0x429f, 0x42a0); ret_effect(gb); return;
L_42a0:
  CYC(0x42a0, 0x42a2); A = 1;
  CYC(0x42a2, 0x42a4); goto L_42a6;
L_42a4:
  CYC(0x42a4, 0x42a6); A = 0x0e;
L_42a6:
  CALL_C(0x42a6, checkTreasureObtained_hook, 0x1748, 0x42a9);
  CYC(0x42a9, 0x42ab); E = 0x78;
  if (!(F & FC)) { CYCT(0x42ab, 0x42ac); ret_effect(gb); return; } CYC(0x42ab, 0x42ac);
  CYC(0x42ac, 0x42ae); goto L_429c;
}

void shopkeeperCheckAllItemsBought_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42ae, 0x42b1); SET_HL(0xd240);
L_42b1:
  CYC(0x42b1, 0x42b3); L = 0x40;
  CYC(0x42b3, 0x42b4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42b4, 0x42b5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x42b5, 0x42b7); goto L_42bb; } CYC(0x42b5, 0x42b7);
  CYC(0x42b7, 0x42b8); A = mem_rd(gb, HL);
  CYC(0x42b8, 0x42ba); alu_cp(gb, 0x47);
  if (F & FZ) { CYCT(0x42ba, 0x42bb); ret_effect(gb); return; } CYC(0x42ba, 0x42bb);
L_42bb:
  CYC(0x42bb, 0x42bc); H = alu_inc8(gb, H);
  CYC(0x42bc, 0x42bd); A = H;
  CYC(0x42bd, 0x42bf); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x42bf, 0x42c1); goto L_42b1; } CYC(0x42bf, 0x42c1);
  CYC(0x42c1, 0x42c4); SET_HL(0x47c0);
  CYC(0x42c4, 0x42c5); alu_or(gb, D);
  CYC(0x42c5, 0x42c6); ret_effect(gb);
}

void shopkeeperTurnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x42c6, objectGetAngleTowardLink_hook, 0x1e9c, 0x42c9);
  CYC(0x42c9, 0x42cb); E = 0x49;
  CYC(0x42cb, 0x42cc); mem_wr(gb, DE, A);
  CALL_C(0x42cc, convertAngleDeToDirection_hook, 0x26f8, 0x42cf);
  CYC(0x42cf, 0x42d0); E = alu_dec8(gb, E);
  CYC(0x42d0, 0x42d1); mem_wr(gb, DE, A);
  CYC(0x42d1, 0x42d4); interactionSetAnimation_hook(gb);
}

void shopkeeperState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4031, 0x4033); A = 1;
  CYC(0x4033, 0x4034); mem_wr(gb, DE, A);
  CYC(0x4034, 0x4036); E = 0x40;
  CYC(0x4036, 0x4037); A = mem_rd(gb, DE);
  CYC(0x4037, 0x4039); alu_or(gb, 0x80);
  CYC(0x4039, 0x403a); mem_wr(gb, DE, A);
  CYC(0x403a, 0x403c); A = 0x80;
  CYC(0x403c, 0x403f); mem_wr(gb, 0xcca2, A);
  CALL_C(0x403f, interactionInitGraphics_hook, 0x15fb, 0x4042);
  CYC(0x4042, 0x4044); E = 0x49;
  CYC(0x4044, 0x4046); A = 4;
  CYC(0x4046, 0x4047); mem_wr(gb, DE, A);
  CYC(0x4047, 0x404a); SET_BC(0x0614);
  CALL_C(0x404a, objectSetCollideRadii_hook, 0x24a9, 0x404d);
  CYC(0x404d, 0x404f); L = 0x42;
  CYC(0x404f, 0x4050); A = mem_rd(gb, HL);
  CYC(0x4050, 0x4052); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(0x4052, 0x4054); goto L_405f; } CYC(0x4052, 0x4054);
  CYC(0x4054, 0x4057); A = mem_rd(gb, 0xc642);
  CYC(0x4057, 0x4059); alu_and(gb, 0x0f);
  CYC(0x4059, 0x405b); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x405b, 0x405d); goto L_405f; } CYC(0x405b, 0x405d);
  CYC(0x405d, 0x405f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
L_405f:
  CYC(0x405f, 0x4061); E = 0x42;
  CYC(0x4061, 0x4062); A = mem_rd(gb, DE);
  CYC(0x4062, 0x4063); alu_or(gb, A);
  CYC(0x4063, 0x4065); A = 3;
  if (F & FZ) CALL_C_CC(0x4065, interactionSetAnimation_hook, 0x262e, 0x4068); else CYC(0x4065, 0x4068);
  CYC(0x4068, 0x406a); A = 0x0e;
  CALL_C(0x406a, interactionSetHighTextIndex_hook, 0x253b, 0x406d);
  CYC(0x406d, 0x406f); E = 0x71;
  CYC(0x406f, 0x4072); objectAddToAButtonSensitiveObjectList_hook(gb);
}

void shopkeeperState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4072, retIfTextIsActive_hook, 0x1859, 0x4075);
  CYC(0x4075, 0x4077); E = 0x71;
  CYC(0x4077, 0x4078); A = mem_rd(gb, DE);
  CYC(0x4078, 0x4079); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4079, 0x407b); goto L_40c5; } CYC(0x4079, 0x407b);
  CYC(0x407b, 0x407d); E = 0x42;
  CYC(0x407d, 0x407e); A = mem_rd(gb, DE);
  CYC(0x407e, 0x407f); alu_or(gb, A);
  CYC(0x407f, 0x4082); SET_HL(0xd00d);
  if (!(F & FZ)) { CYCT(0x4082, 0x4084); goto L_4084; } CYC(0x4082, 0x4084);
L_4084:
  CYC(0x4084, 0x4086); L = 0x0b;
  CYC(0x4086, 0x4088); E = 0x42;
  CYC(0x4088, 0x4089); A = mem_rd(gb, DE);
  CYC(0x4089, 0x408b); alu_and(gb, 1);
  CYC(0x408b, 0x408d); C = 0x69;
  CYC(0x408d, 0x408e); B = mem_rd(gb, HL);
  CYC(0x408e, 0x4090); A = 0x69;
  if (F & FZ) { CYCT(0x4090, 0x4092); goto L_4097; } CYC(0x4090, 0x4092);
  CYC(0x4092, 0x4094); B = 0x27;
  CYC(0x4094, 0x4095); C = mem_rd(gb, HL);
  CYC(0x4095, 0x4097); A = 0x27;
L_4097:
  CYC(0x4097, 0x4098); L = A;
  CYC(0x4098, 0x4099); A = C;
  CYC(0x4099, 0x409a); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x409a, 0x409c); goto L_40bf; } CYC(0x409a, 0x409c);
  CYC(0x409c, 0x409f); A = mem_rd(gb, 0xcc5a);
  CYC(0x409f, 0x40a0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x40a0, 0x40a2); goto L_40bf; } CYC(0x40a0, 0x40a2);
  CYC(0x40a2, 0x40a4); A = 0x81;
  CYC(0x40a4, 0x40a7); mem_wr(gb, 0xcc8a, A);
  CYC(0x40a7, 0x40a8); A = L;
  CYC(0x40a8, 0x40ab); SET_HL(0xd00b);
  CYC(0x40ab, 0x40ac); mem_wr(gb, HL, A);
  CYC(0x40ac, 0x40af); SET_BC(0x0606);
  CALL_C(0x40af, objectSetCollideRadii_hook, 0x24a9, 0x40b2);
  CYC(0x40b2, 0x40b4); E = 0x42;
  CYC(0x40b4, 0x40b5); A = mem_rd(gb, DE);
  CYC(0x40b5, 0x40b8); SET_HL(0x42d4);
  CYC(0x40b8, 0x40b9); push_effect(gb, 0x40b9);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
  CYC(0x40b9, 0x40ba); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40ba, 0x40bb); H = mem_rd(gb, HL);
  CYC(0x40bb, 0x40bc); L = A;
  CYC(0x40bc, 0x40bf); shopkeeperLoadScript_hook(gb); return;
L_40bf:
  CYC(0x40bf, 0x40c2); SET_BC(0x0614);
  CYC(0x40c2, 0x40c5); objectSetCollideRadii_hook(gb); return;
L_40c5:
  CYC(0x40c5, 0x40c6); alu_xor(gb, A);
  CYC(0x40c6, 0x40c7); mem_wr(gb, DE, A);
  CALL_C(0x40c7, objectRemoveFromAButtonSensitiveObjectList_hook, 0x1b41, 0x40ca);
  CALL_C(0x40ca, shopkeeperTurnToFaceLink_hook, 0x42c6, 0x40cd);
  CYC(0x40cd, 0x40cf); A = 0x81;
  CYC(0x40cf, 0x40d2); mem_wr(gb, 0xcc8a, A);
  CYC(0x40d2, 0x40d4); E = 0x44;
  CYC(0x40d4, 0x40d6); A = 2;
  CYC(0x40d6, 0x40d7); mem_wr(gb, DE, A);
  CYC(0x40d7, 0x40d8); ret_effect(gb);
}
