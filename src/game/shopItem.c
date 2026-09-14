#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t shopItem_jump_table(GB *gb) {
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
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void shopItem_addAToHl(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

static void shopItem_addDoubleIndex(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void shopItemDrawDigit(GB *gb) {
  CYC(0x4487, 0x4489); alu_and(gb, 0x0f);
  CYC(0x4489, 0x448a); alu_add(gb, D);
  CYC(0x448a, 0x448b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x448b, 0x448c); mem_wr(gb, HL, E);
  CYC(0x448c, 0x448d); L = alu_inc8(gb, L);
  CYC(0x448d, 0x448e); ret_effect(gb);
}

static void shopItemUpdateRupeeDisplay_tail(GB *gb) {
  CYC(0x43e6, 0x43e8); A = mem_rd(gb, IO_SVBK);
  CYC(0x43e8, 0x43e9); push_effect(gb, AF);
  CYC(0x43e9, 0x43eb); A = 3;
  CYC(0x43eb, 0x43ed); mem_wr(gb, IO_SVBK, A);
  CYC(0x43ed, 0x43ee); push_effect(gb, DE);
  CYC(0x43ee, 0x43ef); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43ef, 0x43f0); E = A;
  CYC(0x43f0, 0x43f1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f1, 0x43f2); D = A;
  CYC(0x43f2, 0x43f3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f3, 0x43f4); B = A;
L_43f4:
  CYC(0x43f4, 0x43f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f5, 0x43f6); mem_wr(gb, DE, A);
  CYC(0x43f6, 0x43f8); D = (uint8_t)(D | 4);
  CYC(0x43f8, 0x43f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f9, 0x43fa); mem_wr(gb, DE, A);
  CYC(0x43fa, 0x43fc); D = (uint8_t)(D & ~4);
  CYC(0x43fc, 0x43fd); SET_DE(DE + 1);
  CYC(0x43fd, 0x43fe); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x43fe, 0x4400); goto L_43f4; } CYC(0x43fe, 0x4400);
  CYC(0x4400, 0x4401); SET_DE(pop_effect(gb));
  CYC(0x4401, 0x4402); SET_AF(pop_effect(gb));
  CYC(0x4402, 0x4404); mem_wr(gb, IO_SVBK, A);
  CYC(0x4404, 0x4407); SET_HL(wInShop);
  CYC(0x4407, 0x4409); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 4));
  CYC(0x4409, 0x440a); ret_effect(gb);
}

void interactionCode47_hook(GB *gb);
void shopItemState0_hook(GB *gb);
void shopItemState5_hook(GB *gb);
void shopItemPopStackAndDeleteSelf_hook(GB *gb);
void shopItemState2_hook(GB *gb);
void shopItemState4_hook(GB *gb);
void shopItemClearRupeeDisplay_hook(GB *gb);
void shopItemUpdateRupeeDisplay_hook(GB *gb);
void shopItemGetTilesForRupeeDisplay_hook(GB *gb);
void shopItemCheckGrabbed_hook(GB *gb);

void interactionCode47_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42dc, 0x42de); E = 0x44;
  CYC(0x42de, 0x42df); A = mem_rd(gb, DE);
  CYC(0x42df, 0x42e0); push_effect(gb, 0x42e0); SET_HL(shopItem_jump_table(gb));
  switch (HL) {
    case 0x42ec: shopItemState0_hook(gb); return;
    case 0x439b: shopItemState2_hook(gb); return;
    case 0x43b4: shopItemState4_hook(gb); return;
    case 0x438d: shopItemState5_hook(gb); return;
    case 0x2c2e: objectAddToGrabbableObjectBuffer_hook(gb); return;
    case 0x440a: shopItemState3(gb); return;
    default: HANDOFF(HL);
  }
}

void shopItemState0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42ec, 0x42ef); A = mem_rd(gb, 0xccd3);
  CYC(0x42ef, 0x42f1); alu_and(gb, 2);
  if (F & FZ) { CYCT(0x42f1, 0x42f2); ret_effect(gb); return; } CYC(0x42f1, 0x42f2);
  CYC(0x42f2, 0x42f4); A = 1;
  CYC(0x42f4, 0x42f5); mem_wr(gb, DE, A);
  CYC(0x42f5, 0x42f7); E = 0x42;
  CYC(0x42f7, 0x42f8); A = mem_rd(gb, DE);
  CYC(0x42f8, 0x42fa); alu_cp(gb, 0);
  if (!(F & FZ)) { CYCT(0x42fa, 0x42fc); goto L_430c; } CYC(0x42fa, 0x42fc);
  CYC(0x42fc, 0x42fe); A = 0x2c;
  CALL_C(0x42fe, checkTreasureObtained_hook, 0x1748, 0x4301);
  if (!(F & FC)) { CYCT(0x4301, 0x4303); goto L_430c; } CYC(0x4301, 0x4303);
  CYC(0x4303, 0x4306); A = mem_rd(gb, 0xc6cc);
  CYC(0x4306, 0x4307); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4307, 0x4309); goto L_430c; } CYC(0x4307, 0x4309);
  CYC(0x4309, 0x430b); A = 0x14;
  CYC(0x430b, 0x430c); mem_wr(gb, DE, A);
L_430c:
  CYC(0x430c, 0x430d); A = mem_rd(gb, DE);
  CYC(0x430d, 0x430f); alu_cp(gb, 4);
  if (!(F & FZ)) { CYCT(0x430f, 0x4311); goto L_431b; } CYC(0x430f, 0x4311);
  CYC(0x4311, 0x4313); A = 3;
  CALL_C(0x4313, checkTreasureObtained_hook, 0x1748, 0x4316);
  if (!(F & FC)) { CYCT(0x4316, 0x4319); shopItemPopStackAndDeleteSelf_hook(gb); return; } CYC(0x4316, 0x4319);
  CYC(0x4319, 0x431b); goto L_4327;
L_431b:
  CYC(0x431b, 0x431d); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(0x431d, 0x431f); goto L_4327; } CYC(0x431d, 0x431f);
  CALL_C(0x431f, checkIsLinkedGame_hook, 0x1992, 0x4322);
  if (F & FZ) { CYCT(0x4322, 0x4324); goto L_4327; } CYC(0x4322, 0x4324);
  CYC(0x4324, 0x4326); A = 0x13;
  CYC(0x4326, 0x4327); mem_wr(gb, DE, A);
L_4327:
  CYC(0x4327, 0x4329); A = 0x0e;
  CALL_C(0x4329, checkTreasureObtained_hook, 0x1748, 0x432c);
  if (F & FC) { CYCT(0x432c, 0x432e); goto L_4339; } CYC(0x432c, 0x432e);
  CYC(0x432e, 0x4330); A = 0x1d;
  CALL_C(0x4330, checkGlobalFlag_hook, 0x31f3, 0x4333);
  if (F & FZ) { CYCT(0x4333, 0x4335); goto L_4339; } CYC(0x4333, 0x4335);
  CYC(0x4335, 0x4337); C = 8;
  CYC(0x4337, 0x4339); goto L_433b;
L_4339:
  CYC(0x4339, 0x433b); C = 0;
L_433b:
  CYC(0x433b, 0x433e); A = mem_rd(gb, 0xc643);
  CYC(0x433e, 0x4340); alu_and(gb, 0xf7);
  CYC(0x4340, 0x4341); alu_or(gb, C);
  CYC(0x4341, 0x4344); mem_wr(gb, 0xc643, A);
  CYC(0x4344, 0x4346); A = 0x0d;
  CALL_C(0x4346, checkTreasureObtained_hook, 0x1748, 0x4349);
  CYC(0x4349, 0x434b); C = 0x10;
  if (F & FC) { CYCT(0x434b, 0x434d); goto L_434f; } CYC(0x434b, 0x434d);
  CYC(0x434d, 0x434f); C = 0x20;
L_434f:
  CYC(0x434f, 0x4352); A = mem_rd(gb, 0xc643);
  CYC(0x4352, 0x4354); alu_and(gb, 0xcf);
  CYC(0x4354, 0x4355); alu_or(gb, C);
  CYC(0x4355, 0x4358); mem_wr(gb, 0xc643, A);
L_4358:
  CYC(0x4358, 0x435a); E = 0x42;
  CYC(0x435a, 0x435b); A = mem_rd(gb, DE);
  CYC(0x435b, 0x435c); alu_add(gb, A);
  CYC(0x435c, 0x435f); SET_HL(0x4523);
  CYC(0x435f, 0x4360); shopItem_addDoubleIndex(gb, 0x4360);
  CYC(0x4360, 0x4361); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4361, 0x4362); C = A;
  CYC(0x4362, 0x4364); B = 0xc6;
  CYC(0x4364, 0x4365); A = mem_rd(gb, BC);
  CYC(0x4365, 0x4366); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4366, 0x4368); goto L_4376; } CYC(0x4366, 0x4368);
  CYC(0x4368, 0x4369); SET_HL(HL + 1);
  CYC(0x4369, 0x436a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x436a, 0x436c); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x436c, 0x436e); shopItemPopStackAndDeleteSelf_hook(gb); return; } CYC(0x436c, 0x436e);
  CYC(0x436e, 0x436f); mem_wr(gb, DE, A);
  CYC(0x436f, 0x4371); E = 0x4d;
  CYC(0x4371, 0x4372); A = mem_rd(gb, DE);
  CYC(0x4372, 0x4373); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4373, 0x4374); mem_wr(gb, DE, A);
  CYC(0x4374, 0x4376); goto L_4358;
L_4376:
  CALL_C(0x4376, interactionInitGraphics_hook, 0x15fb, 0x4379);
  CYC(0x4379, 0x437b); A = 7;
  CALL_C(0x437b, objectSetCollideRadius_hook, 0x24a1, 0x437e);
  CYC(0x437e, 0x4380); L = 0x70;
  CYC(0x4380, 0x4382); E = 0x4b;
  CYC(0x4382, 0x4383); A = mem_rd(gb, DE);
  CYC(0x4383, 0x4384); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4384, 0x4386); E = 0x4d;
  CYC(0x4386, 0x4387); A = mem_rd(gb, DE);
  CYC(0x4387, 0x4388); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CALL_C(0x4388, objectSetVisible83_hook, 0x1e72, 0x438b);
  CYC(0x438b, 0x438d); shopItemUpdateRupeeDisplay_hook(gb);
}

void shopItemState5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x438d, retIfTextIsActive_hook, 0x1859, 0x4390);
  CYC(0x4390, 0x4391); alu_xor(gb, A);
  CYC(0x4391, 0x4394); mem_wr(gb, 0xcc8a, A);
  CYC(0x4394, 0x4397); mem_wr(gb, 0xcc02, A);
  shopItemPopStackAndDeleteSelf_hook(gb);
}

void shopItemPopStackAndDeleteSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4397, 0x4398); SET_AF(pop_effect(gb));
  CYC(0x4398, 0x439b); interactionDelete_hook(gb);
}

void shopItemState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x439b, 0x439d); E = 0x45;
  CYC(0x439d, 0x439e); A = mem_rd(gb, DE);
  CYC(0x439e, 0x439f); push_effect(gb, 0x439f); SET_HL(shopItem_jump_table(gb));
  switch (HL) {
    case 0x43a3: goto L_43a3;
    case 0x43b0: goto L_43b0;
    default: HANDOFF(HL);
  }
L_43a3:
  CYC(0x43a3, 0x43a5); A = 1;
  CYC(0x43a5, 0x43a6); mem_wr(gb, DE, A);
  CYC(0x43a6, 0x43a8); A = 8;
  CYC(0x43a8, 0x43ab); mem_wr(gb, 0xcc5b, A);
  CALL_C(0x43ab, objectSetVisible80_hook, 0x1e57, 0x43ae);
  CYC(0x43ae, 0x43b0); shopItemClearRupeeDisplay_hook(gb); return;
L_43b0:
  CALL_C(0x43b0, shopItemCheckGrabbed_hook, 0x44d0, 0x43b3);
  if (!(F & FZ)) { CYCT(0x43b3, 0x43b4); ret_effect(gb); return; } CYC(0x43b3, 0x43b4);
  shopItemState4_hook(gb);
}

void shopItemState4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43b4, 0x43b5); H = D;
  CYC(0x43b5, 0x43b7); E = 0x4b;
  CYC(0x43b7, 0x43b9); L = 0x70;
  CYC(0x43b9, 0x43ba); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43ba, 0x43bb); mem_wr(gb, DE, A);
  CYC(0x43bb, 0x43bd); E = 0x4d;
  CYC(0x43bd, 0x43be); A = mem_rd(gb, HL);
  CYC(0x43be, 0x43bf); mem_wr(gb, DE, A);
  CYC(0x43bf, 0x43c1); L = 0x4f;
  CYC(0x43c1, 0x43c3); mem_wr(gb, HL, 0);
  CYC(0x43c3, 0x43c5); L = 0x44;
  CYC(0x43c5, 0x43c7); mem_wr(gb, HL, 1);
  CALL_C(0x43c7, shopItemUpdateRupeeDisplay_hook, 0x43e2, 0x43ca);
  CALL_C(0x43ca, objectSetVisible83_hook, 0x1e72, 0x43cd);
  CYC(0x43cd, 0x43d0); dropLinkHeldItem_hook(gb);
}

void shopItemClearRupeeDisplay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x43d0, shopItemGetTilesForRupeeDisplay_hook, 0x4447, 0x43d3);
  if (!(F & FC)) { CYCT(0x43d3, 0x43d4); ret_effect(gb); return; } CYC(0x43d3, 0x43d4);
  CYC(0x43d4, 0x43d5); push_effect(gb, HL);
  CYC(0x43d5, 0x43d7); A = 3;
  CYC(0x43d7, 0x43d8); shopItem_addAToHl(gb, 0x43d8);
  CYC(0x43d8, 0x43da); A = 0x20;
  CYC(0x43da, 0x43db); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43db, 0x43dc); L = alu_inc8(gb, L);
  CYC(0x43dc, 0x43dd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43dd, 0x43de); L = alu_inc8(gb, L);
  CYC(0x43de, 0x43df); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43df, 0x43e0); SET_HL(pop_effect(gb));
  CYC(0x43e0, 0x43e2); shopItemUpdateRupeeDisplay_tail(gb);
}

void shopItemUpdateRupeeDisplay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x43e2, shopItemGetTilesForRupeeDisplay_hook, 0x4447, 0x43e5);
  if (!(F & FC)) { CYCT(0x43e5, 0x43e6); ret_effect(gb); return; } CYC(0x43e5, 0x43e6);
  shopItemUpdateRupeeDisplay_tail(gb);
}

void shopItemGetTilesForRupeeDisplay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4447, 0x4449); E = 0x42;
  CYC(0x4449, 0x444a); A = mem_rd(gb, DE);
  CYC(0x444a, 0x444b); C = A;
  CYC(0x444b, 0x444e); SET_HL(0x448e);
  CYC(0x444e, 0x444f); shopItem_addDoubleIndex(gb, 0x444f);
  CYC(0x444f, 0x4450); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4450, 0x4452); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x4452, 0x4453); ret_effect(gb); return; } CYC(0x4452, 0x4453);
  CYC(0x4453, 0x4454); push_effect(gb, DE);
  CYC(0x4454, 0x4455); E = A;
  CYC(0x4455, 0x4456); D = mem_rd(gb, HL);
  CYC(0x4456, 0x4457); A = C;
  CYC(0x4457, 0x445a); SET_HL(0x44ba);
  CYC(0x445a, 0x445b); shopItem_addAToHl(gb, 0x445b);
  CYC(0x445b, 0x445c); A = mem_rd(gb, HL);
  CALL_C(0x445c, getRupeeValue_hook, 0x1781, 0x445f);
  CYC(0x445f, 0x4462); SET_HL(0xcec0);
  CYC(0x4462, 0x4463); mem_wr(gb, HL, E);
  CYC(0x4463, 0x4464); L = alu_inc8(gb, L);
  CYC(0x4464, 0x4465); mem_wr(gb, HL, D);
  CYC(0x4465, 0x4466); L = alu_inc8(gb, L);
  CYC(0x4466, 0x4468); E = 6;
  CYC(0x4468, 0x446a); D = 0x30;
  CYC(0x446a, 0x446c); A = 2;
  CYC(0x446c, 0x446d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x446d, 0x446e); A = B;
  CYC(0x446e, 0x446f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x446f, 0x4471); goto L_4477; } CYC(0x446f, 0x4471);
  CYC(0x4471, 0x4472); L = alu_dec8(gb, L);
  CYC(0x4472, 0x4473); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4473, 0x4474); L = alu_inc8(gb, L);
  CYC(0x4474, 0x4477); push_effect(gb, 0x4477); shopItemDrawDigit(gb);
L_4477:
  CYC(0x4477, 0x4478); A = C;
  CYC(0x4478, 0x447a); A = alu_swap(gb, A);
  CYC(0x447a, 0x447d); push_effect(gb, 0x447d); shopItemDrawDigit(gb);
  CYC(0x447d, 0x447e); A = C;
  CYC(0x447e, 0x4481); push_effect(gb, 0x4481); shopItemDrawDigit(gb);
  CYC(0x4481, 0x4484); SET_HL(0xcec0);
  CYC(0x4484, 0x4485); SET_DE(pop_effect(gb));
  CYC(0x4485, 0x4486); alu_scf(gb);
  CYC(0x4486, 0x4487); ret_effect(gb);
}

void shopItemCheckGrabbed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44d0, 0x44d3); A = mem_rd(gb, 0xcc2a);
  CYC(0x44d3, 0x44d5); alu_and(gb, 3);
  if (F & FZ) { CYCT(0x44d5, 0x44d7); goto L_44f5; } CYC(0x44d5, 0x44d7);
  CYC(0x44d7, 0x44d9); E = 0x71;
  CYC(0x44d9, 0x44da); A = mem_rd(gb, DE);
  CYC(0x44da, 0x44dc); alu_sub(gb, 0x0d);
  CYC(0x44dc, 0x44dd); B = A;
  CYC(0x44dd, 0x44df); alu_add(gb, 0x1a);
  CYC(0x44df, 0x44e2); SET_HL(0xd00d);
  CYC(0x44e2, 0x44e3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x44e3, 0x44e5); goto L_44f5; } CYC(0x44e3, 0x44e5);
  CYC(0x44e5, 0x44e6); A = B;
  CYC(0x44e6, 0x44e7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x44e7, 0x44e9); goto L_44f5; } CYC(0x44e7, 0x44e9);
  CYC(0x44e9, 0x44eb); L = 0x0b;
  CYC(0x44eb, 0x44ec); A = mem_rd(gb, HL);
  CYC(0x44ec, 0x44ee); alu_cp(gb, 0x3d);
  if (!(F & FC)) { CYCT(0x44ee, 0x44f0); goto L_44f5; } CYC(0x44ee, 0x44f0);
  CYC(0x44f0, 0x44f2); L = 8;
  CYC(0x44f2, 0x44f3); A = mem_rd(gb, HL);
  CYC(0x44f3, 0x44f4); alu_or(gb, A);
  CYC(0x44f4, 0x44f5); ret_effect(gb); return;
L_44f5:
  CYC(0x44f5, 0x44f6); alu_or(gb, D);
  CYC(0x44f6, 0x44f7); ret_effect(gb);
}
