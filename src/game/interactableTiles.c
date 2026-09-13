#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void specialObjectGetTileAtOffset_hook(GB *gb);
void showInfoTextForTile_hook(GB *gb);
void specialObjectCheckPushingAgainstTile_hook(GB *gb);

static void add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void interactable_tiles_jump_table_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
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
}

void resetPushingAgainstTileCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x420c, 0x420e); A = 0x14;
  CYC(0x420e, 0x4211); W8(wPushingAgainstTileCounter) = A;
  CYC(0x4211, 0x4212); ret_effect(gb);
}

void decPushingAgainstTileCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4212, 0x4215); SET_HL(wPushingAgainstTileCounter);
  CYC(0x4215, 0x4216); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4216, 0x4217); ret_effect(gb);
}

void specialObjectGetTileInFront_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4373, 0x4375); E = 0x08;
  CYC(0x4375, 0x4376); A = mem_rd(gb, DE);
  CYC(0x4376, 0x4379); SET_HL(0x4390);
  CYC(0x4379, 0x437a); add_double_index_to_hl_from_rst(gb, 0x437a);
  specialObjectGetTileAtOffset_hook(gb);
}

void specialObjectGetTileAtOffset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x437a, 0x437c); E = 0x0b;
  CYC(0x437c, 0x437d); A = mem_rd(gb, DE);
  CYC(0x437d, 0x437e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x437e, 0x4380); alu_and(gb, 0xf0);
  CYC(0x4380, 0x4381); C = A;
  CYC(0x4381, 0x4382); SET_HL(HL + 1);
  CYC(0x4382, 0x4384); E = 0x0d;
  CYC(0x4384, 0x4385); A = mem_rd(gb, DE);
  CYC(0x4385, 0x4386); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4386, 0x4388); A = alu_swap(gb, A);
  CYC(0x4388, 0x438a); alu_and(gb, 0x0f);
  CYC(0x438a, 0x438b); alu_or(gb, C);
  CYC(0x438b, 0x438c); C = A;
  CYC(0x438c, 0x438e); B = 0xcf;
  CYC(0x438e, 0x438f); A = mem_rd(gb, BC);
  CYC(0x438f, 0x4390); ret_effect(gb);
}

void checkTileAfterNext_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4398, 0x439b); A = W8(wLinkPushingDirection);
  CYC(0x439b, 0x439e); SET_HL(0x43aa);
  CYC(0x439e, 0x439f); add_double_index_to_hl_from_rst(gb, 0x439f);
  CALL_C(0x439f, specialObjectGetTileAtOffset_hook, 0x437a, 0x43a2);
  CYC(0x43a2, 0x43a4); B = 0xce;
  CYC(0x43a4, 0x43a5); A = mem_rd(gb, BC);
  CYC(0x43a5, 0x43a7); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x43a7, 0x43a8); ret_effect(gb); return;
  }
  CYC(0x43a7, 0x43a8);
  CYC(0x43a8, 0x43a9); alu_scf(gb);
  CYC(0x43a9, 0x43aa); ret_effect(gb);
}

void jumpToShowInfoText_hook(GB *gb) {
  CYC(0x4283, 0x4285); A = 0x08;
  CYC(0x4285, 0x4288); showInfoTextForTile_hook(gb);
}

void createKeySpriteInteraction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4288, getFreeInteractionSlot_hook, 0x3aef, 0x428b);
  if (!(F & FZ)) {
    CYCT(0x428b, 0x428c); ret_effect(gb); return;
  }
  CYC(0x428b, 0x428c);
  CYC(0x428c, 0x428e); mem_wr(gb, HL, 0x17);
  CYC(0x428e, 0x428f); L = alu_inc8(gb, L);
  CYC(0x428f, 0x4291); A = H8(hFF8B);
  CYC(0x4291, 0x4292); mem_wr(gb, HL, A);
  CYC(0x4292, 0x4294); A = H8(hFF8D);
  CYC(0x4294, 0x4296); L = 0x4b;
  CYC(0x4296, 0x4299); setShortPosition_hook(gb);
}

void nextToSubrosiaKeydoor_hook(GB *gb) {
  CYC(0x4299, 0x429a); alu_scf(gb);
  CYC(0x429a, 0x429b); ret_effect(gb);
}

void nextToGhiniSpawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x429b, 0x429e); A = W8(wIsMaplePresent);
  CYC(0x429e, 0x429f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x429f, 0x42a0); ret_effect(gb); return;
  }
  CYC(0x429f, 0x42a0);
  CALL_C(0x42a0, specialObjectCheckPushingAgainstTile_hook, 0x4329, 0x42a3);
  if (F & FZ) {
    CYCT(0x42a3, 0x42a6); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x42a3, 0x42a6);
  CALL_C(0x42a6, decPushingAgainstTileCounter_hook, 0x4212, 0x42a9);
  if (!(F & FZ)) {
    CYCT(0x42a9, 0x42aa); ret_effect(gb); return;
  }
  CYC(0x42a9, 0x42aa);
  CYC(0x42aa, 0x42ac); A = H8(hFF8D);
  CYC(0x42ac, 0x42ad); L = A;
  CYC(0x42ad, 0x42af); H = 0xcf;
  CYC(0x42af, 0x42b1); mem_wr(gb, HL, 0x00);
  CALL_C(0x42b1, convertShortToLongPosition_hook, 0x20cb, 0x42b4);
  CALL_C(0x42b4, getFreeEnemySlot_hook, 0x2e27, 0x42b7);
  if (!(F & FZ)) {
    CYCT(0x42b7, 0x42b8); ret_effect(gb); return;
  }
  CYC(0x42b7, 0x42b8);
  CYC(0x42b8, 0x42ba); mem_wr(gb, HL, 0x17);
  CYC(0x42ba, 0x42bb); L = alu_inc8(gb, L);
  CYC(0x42bb, 0x42bc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x42bc, 0x42be); L = 0x8b;
  CYC(0x42be, 0x42bf); mem_wr(gb, HL, B);
  CYC(0x42bf, 0x42c1); L = 0x8d;
  CYC(0x42c1, 0x42c2); mem_wr(gb, HL, C);
  CYC(0x42c2, 0x42c3); ret_effect(gb);
}

void nextToTileWithInfoText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x42c3, specialObjectCheckPushingAgainstTile_hook, 0x4329, 0x42c6);
  if (F & FZ) {
    CYCT(0x42c6, 0x42c9); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x42c6, 0x42c9);
  CALL_C(0x42c9, decPushingAgainstTileCounter_hook, 0x4212, 0x42cc);
  if (!(F & FZ)) {
    CYCT(0x42cc, 0x42cd); ret_effect(gb); return;
  }
  CYC(0x42cc, 0x42cd);
  CALL_C(0x42cd, resetPushingAgainstTileCounter_hook, 0x420c, 0x42d0);
  CYC(0x42d0, 0x42d1); A = B;
  CYC(0x42d1, 0x42d3); A = alu_swap(gb, A);
  CYC(0x42d3, 0x42d5); alu_and(gb, 0x0f);
  CYC(0x42d5, 0x42d6); interactable_tiles_jump_table_from_rst(gb, 0x42d6);
  switch (HL) {
    case 0x42e0:
      CYC(0x42e0, 0x42e2); A = 0x16;
      CALL_C(0x42e2, checkTreasureObtained_hook, 0x1748, 0x42e5);
      CYC(0x42e5, 0x42e6); alu_ccf(gb);
      if (!(F & FC)) {
        CYCT(0x42e6, 0x42e7); ret_effect(gb); return;
      }
      CYC(0x42e6, 0x42e7);
      CYC(0x42e7, 0x42e9); A = 0x03;
      CYC(0x42e9, 0x42eb); showInfoTextForTile_hook(gb); return;
    case 0x42eb:
      CYC(0x42eb, 0x42ed); A = 0x05;
      CYC(0x42ed, 0x42ef); showInfoTextForTile_hook(gb); return;
    case 0x42ef:
      CYC(0x42ef, 0x42f1); A = 0x06;
      CYC(0x42f1, 0x42f3); showInfoTextForTile_hook(gb); return;
    case 0x42f3:
      CYC(0x42f3, 0x42f5); A = 0x07;
      CYC(0x42f5, 0x42f7); showInfoTextForTile_hook(gb); return;
    case 0x42f7:
      CYC(0x42f7, 0x42f9); A = 0x04;
      CYC(0x42f9, 0x42fb); showInfoTextForTile_hook(gb); return;
  }
  hook_continue(gb, HL, sp0_);
}

void showInfoTextForTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42fb, 0x42fe); SET_HL(0x4315);
  CYC(0x42fe, 0x42ff); add_double_index_to_hl_from_rst(gb, 0x42ff);
  CYC(0x42ff, 0x4300); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4300, 0x4301); B = A;
  CYC(0x4301, 0x4302); C = mem_rd(gb, HL);
  CALL_C(0x4302, resetPushingAgainstTileCounter_hook, 0x420c, 0x4305);
  CYC(0x4305, 0x4308); SET_HL(wInformativeTextsShown);
  CYC(0x4308, 0x4309); A = mem_rd(gb, HL);
  CYC(0x4309, 0x430a); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(0x430a, 0x430b); ret_effect(gb); return;
  }
  CYC(0x430a, 0x430b);
  CYC(0x430b, 0x430c); A = mem_rd(gb, HL);
  CYC(0x430c, 0x430d); alu_or(gb, B);
  CYC(0x430d, 0x430e); mem_wr(gb, HL, A);
  CYC(0x430e, 0x4310); B = 0x51;
  CALL_C(0x4310, showText_hook, 0x1872, 0x4313);
  CYC(0x4313, 0x4314); alu_scf(gb);
  CYC(0x4314, 0x4315); ret_effect(gb);
}

static void special_object_check_position_component(GB *gb) {
  CYC(0x4348, 0x4349); A = mem_rd(gb, HL);
  CYC(0x4349, 0x434b); alu_and(gb, 0x0f);
  CYC(0x434b, 0x434d); alu_sub(gb, 0x03);
  CYC(0x434d, 0x434f); alu_cp(gb, 0x0b);
  CYC(0x434f, 0x4350); ret_effect(gb);
}

static void special_object_check_position(GB *gb) {
  CYC(0x433f, 0x4340); H = D;
  CYC(0x4340, 0x4342); L = 0x0b;
  CYC(0x4342, 0x4345); push_effect(gb, 0x4345);
  special_object_check_position_component(gb);
  if (F & FC) {
    CYCT(0x4345, 0x4346); ret_effect(gb); return;
  }
  CYC(0x4345, 0x4346);
  CYC(0x4346, 0x4348); L = 0x0d;
  special_object_check_position_component(gb);
}

void specialObjectCheckPushingAgainstTile_hook(GB *gb) {
  CYC(0x4329, 0x432c); A = W8(wLinkPushingDirection);
  CYC(0x432c, 0x432d); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x432d, 0x432f); goto not_pushing;
  }
  CYC(0x432d, 0x432f);
  CYC(0x432f, 0x4332); A = W8(wLinkAngle);
  CYC(0x4332, 0x4334); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x4334, 0x4336); goto not_pushing;
  }
  CYC(0x4334, 0x4336);
  CYC(0x4336, 0x4339); push_effect(gb, 0x4339);
  special_object_check_position(gb);
  if (!(F & FC)) {
    CYCT(0x4339, 0x433b); goto not_pushing;
  }
  CYC(0x4339, 0x433b);
  CYC(0x433b, 0x433c); alu_or(gb, D);
  CYC(0x433c, 0x433d); ret_effect(gb); return;

not_pushing:
  CYC(0x433d, 0x433e); alu_xor(gb, A);
  CYC(0x433e, 0x433f); ret_effect(gb);
}

void checkAndDecKeyCount_hook(GB *gb) {
  CYC(0x4350, 0x4353); A = W8(wDungeonIndex);
  CYC(0x4353, 0x4355); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4355, 0x4356); ret_effect(gb); return;
  }
  CYC(0x4355, 0x4356);
  CYC(0x4356, 0x4357); A = B;
  CYC(0x4357, 0x4359); alu_cp(gb, 0x40);
  CYC(0x4359, 0x435b); H = 0xc6;
  CYC(0x435b, 0x435e); A = W8(wDungeonIndex);
  if (!(F & FC)) {
    CYCT(0x435e, 0x4360);
    CYC(0x436e, 0x4370); L = 0x82;
    CYC(0x4370, 0x4373); checkFlag_hook(gb); return;
  }
  CYC(0x435e, 0x4360);
  CYC(0x4360, 0x4362); alu_add(gb, 0x72);
  CYC(0x4362, 0x4363); L = A;
  CYC(0x4363, 0x4364); A = mem_rd(gb, HL);
  CYC(0x4364, 0x4365); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4365, 0x4366); ret_effect(gb); return;
  }
  CYC(0x4365, 0x4366);
  CYC(0x4366, 0x4367); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4367, 0x436a); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x436a, 0x436c); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x436c, 0x436d); alu_or(gb, H);
  CYC(0x436d, 0x436e); ret_effect(gb);
}
