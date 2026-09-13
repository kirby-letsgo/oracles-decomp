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

static void add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
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

static void facing_bottom_abort(GB *gb) {
  CYC(0x410b, 0x410c); SET_AF(pop_effect(gb));
  CYC(0x410c, 0x410d); alu_xor(gb, A);
  CYC(0x410d, 0x410e); ret_effect(gb);
}

void checkFacingBottomOfTile_hook(GB *gb) {
  CYC(0x40f6, 0x40f9); A = mem_rd(gb, w1Link_direction);
  CYC(0x40f9, 0x40fc); SET_HL(0x4107);
  CYC(0x40fc, 0x40fd); add_a_to_hl_from_rst(gb, 0x40fd);
  CYC(0x40fd, 0x4100); A = mem_rd(gb, w1Link_adjacentWallsBitset);
  CYC(0x4100, 0x4101); alu_and(gb, mem_rd(gb, HL));
  CYC(0x4101, 0x4102); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4102, 0x4104); facing_bottom_abort(gb); return;
  }
  CYC(0x4102, 0x4104);
  CYC(0x4104, 0x4106); alu_cp(gb, 0xc0);
  CYC(0x4106, 0x4107); ret_effect(gb);
}

void checkFacingBottomOfTileAndPressedA_hook(GB *gb) {
  CYC(0x40ef, 0x40f2); A = W8(wGameKeysJustPressed);
  CYC(0x40f2, 0x40f4); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x40f4, 0x40f6); facing_bottom_abort(gb); return;
  }
  CYC(0x40f4, 0x40f6);
  checkFacingBottomOfTile_hook(gb);
}

void nextToChestTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x402d, checkFacingBottomOfTileAndPressedA_hook, 0x40ef, 0x4030);
  if (F & FZ) {
    CYCT(0x4030, 0x4032);
  } else {
    CYC(0x4030, 0x4032);
    CYC(0x4032, 0x4035); SET_BC(0x510d);
    CALL_C(0x4035, showText_hook, 0x1872, 0x4038);
    CYC(0x4038, 0x4039); alu_scf(gb);
    CYC(0x4039, 0x403a); ret_effect(gb); return;
  }
  CYC(0x403a, 0x403d); A = W8(wInShop);
  CYC(0x403d, 0x403e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x403e, 0x4040); goto open_chest;
  }
  CYC(0x403e, 0x4040);
  CYC(0x4040, 0x4043); A = W8(wcca1);
  CYC(0x4043, 0x4044); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4044, 0x4046); goto open_chest;
  }
  CYC(0x4044, 0x4046);
  CYC(0x4046, 0x4049); A = W8(wcca2);
  CYC(0x4049, 0x404a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x404a, 0x404b); ret_effect(gb); return;
  }
  CYC(0x404a, 0x404b);

open_chest:
  CYC(0x404b, 0x404c); A = C;
  CYC(0x404c, 0x404f); W8(wcca2) = A;
  CYC(0x404f, 0x4051); A = 0xf0;
  CALL_C(0x4051, setTile_hook, 0x3a9c, 0x4054);
  CYC(0x4054, 0x4056); A = 0x6c;
  CALL_C(0x4056, playSound_b00_hook, 0x0c98, 0x4059);
  CYC(0x4059, 0x405c); A = W8(wInShop);
  CYC(0x405c, 0x405d); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x405d, 0x405e); ret_effect(gb); return;
  }
  CYC(0x405d, 0x405e);
  CYC(0x405e, 0x4061); A = W8(wcca1);
  CYC(0x4061, 0x4062); alu_or(gb, A);
  CYC(0x4062, 0x4063); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(0x4063, 0x4064); ret_effect(gb); return;
  }
  CYC(0x4063, 0x4064);
  CYC(0x4064, 0x4067); SET_HL(w1ReservedInteraction0);
  CYC(0x4067, 0x4069); B = 0x40;
  CALL_C(0x4069, clearMemory_hook, 0x046f, 0x406c);
  CYC(0x406c, 0x406f); A = W8(wChestContentsOverride);
  CYC(0x406f, 0x4070); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4070, 0x4072);
    CALL_C(0x4079, getChestData_hook, 0x10cc, 0x407c);
  } else {
    CYC(0x4070, 0x4072);
    CYC(0x4072, 0x4073); B = A;
    CYC(0x4073, 0x4076); A = mem_rd(gb, wChestContentsOverride + 1);
    CYC(0x4076, 0x4077); C = A;
    CYC(0x4077, 0x4079);
  }
  CYC(0x407c, 0x407d); A = B;
  CYC(0x407d, 0x407e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x407e, 0x4080); goto record_opened;
  }
  CYC(0x407e, 0x4080);
  CYC(0x4080, 0x4082); A = 0x83;
  CYC(0x4082, 0x4085); W8(wDisabledObjects) = A;
  CYC(0x4085, 0x4088); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x4088, 0x408b); SET_HL(w1ReservedInteraction0_enabled);
  CYC(0x408b, 0x408d); A = 0x81;
  CYC(0x408d, 0x408e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x408e, 0x4090); mem_wr(gb, HL, 0x60);
  CYC(0x4090, 0x4091); L = alu_inc8(gb, L);
  CYC(0x4091, 0x4092); mem_wr(gb, HL, B);
  CYC(0x4092, 0x4093); L = alu_inc8(gb, L);
  CYC(0x4093, 0x4094); mem_wr(gb, HL, C);
  CYC(0x4094, 0x4096); L = 0x4b;
  CYC(0x4096, 0x4099); A = W8(wcca2);
  CYC(0x4099, 0x409a); B = A;
  CYC(0x409a, 0x409c); alu_and(gb, 0xf0);
  CYC(0x409c, 0x409d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x409d, 0x409e); L = alu_inc8(gb, L);
  CYC(0x409e, 0x409f); A = B;
  CYC(0x409f, 0x40a1); A = alu_swap(gb, A);
  CYC(0x40a1, 0x40a3); alu_and(gb, 0xf0);
  CYC(0x40a3, 0x40a5); alu_or(gb, 0x08);
  CYC(0x40a5, 0x40a6); mem_wr(gb, HL, A);

record_opened:
  CALL_C(0x40a6, getThisRoomFlags_hook, 0x197d, 0x40a9);
  CYC(0x40a9, 0x40ab); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(0x40ab, 0x40ac); alu_xor(gb, A);
  CYC(0x40ac, 0x40af); W8(wChestContentsOverride) = A;
  CYC(0x40af, 0x40b2); mem_wr(gb, wChestContentsOverride + 1, A);
  CYC(0x40b2, 0x40b3); alu_scf(gb);
  CYC(0x40b3, 0x40b4); ret_effect(gb);
}

void nextToSignTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x40b4, checkFacingBottomOfTileAndPressedA_hook, 0x40ef, 0x40b7);
  CYC(0x40b7, 0x40ba); SET_BC(0x510e);
  if (!(F & FZ)) {
    CYCT(0x40ba, 0x40bc); goto show_text;
  }
  CYC(0x40ba, 0x40bc);
  CYC(0x40bc, 0x40bf); A = W8(wActiveGroup);
  CYC(0x40bf, 0x40c2); SET_HL(0x7784);
  CYC(0x40c2, 0x40c3); add_double_index_to_hl_from_rst(gb, 0x40c3);
  CYC(0x40c3, 0x40c4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x40c4, 0x40c5); H = mem_rd(gb, HL);
  CYC(0x40c5, 0x40c6); L = A;
  CYC(0x40c6, 0x40c9); A = W8(wActiveRoom);
  CYC(0x40c9, 0x40ca); B = A;
  CYC(0x40ca, 0x40cc); A = H8(hFF8D);
  CYC(0x40cc, 0x40cd); C = A;
  for (;;) {
    CYC(0x40cd, 0x40ce); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x40ce, 0x40cf); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x40cf, 0x40d1);
      CYC(0x40e7, 0x40ea); SET_BC(0x0901);
      goto show_text;
    }
    CYC(0x40cf, 0x40d1);
    CYC(0x40d1, 0x40d2); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(0x40d2, 0x40d4);
      CYC(0x40d8, 0x40d9); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x40d9, 0x40da); alu_cp(gb, B);
      if (F & FZ) {
        CYCT(0x40da, 0x40dc);
        CYC(0x40df, 0x40e0); C = mem_rd(gb, HL);
        CYC(0x40e0, 0x40e2); B = 0x2e;
        CALL_C(0x40e2, showText_hook, 0x1872, 0x40e5);
        CYC(0x40e5, 0x40e6); alu_scf(gb);
        CYC(0x40e6, 0x40e7); ret_effect(gb); return;
      }
      CYC(0x40da, 0x40dc);
      CYC(0x40dc, 0x40dd); SET_HL(HL + 1);
      CYC(0x40dd, 0x40df);
    } else {
      CYC(0x40d2, 0x40d4);
      CYC(0x40d4, 0x40d5); SET_HL(HL + 1);
      CYC(0x40d5, 0x40d6); SET_HL(HL + 1);
      CYC(0x40d6, 0x40d8);
    }
  }

show_text:
  CALL_C(0x40ea, showText_hook, 0x1872, 0x40ed);
  CYC(0x40ed, 0x40ee); alu_scf(gb);
  CYC(0x40ee, 0x40ef); ret_effect(gb);
}

void nextToPushableBlock_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x410e, 0x4111); A = W8(wTilesetFlags);
  CYC(0x4111, 0x4113); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x4113, 0x4114); ret_effect(gb); return;
  }
  CYC(0x4113, 0x4114);
  CALL_C(0x4114, specialObjectCheckPushingAgainstTile_hook, 0x4329, 0x4117);
  if (F & FZ) {
    CYCT(0x4117, 0x411a); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x4117, 0x411a);
  CALL_C(0x411a, decPushingAgainstTileCounter_hook, 0x4212, 0x411d);
  if (!(F & FZ)) {
    CYCT(0x411d, 0x411e); ret_effect(gb); return;
  }
  CYC(0x411d, 0x411e);
  CYC(0x411e, 0x4120); alu_bit(gb, 6, B);
  if (F & FZ) {
    CYCT(0x4120, 0x4122); goto check_direction;
  }
  CYC(0x4120, 0x4122);
  CYC(0x4122, 0x4124); A = 0x16;
  CALL_C(0x4124, checkTreasureObtained_hook, 0x1748, 0x4127);
  CYC(0x4127, 0x4129); A = 0x03;
  if (!(F & FC)) {
    CYCT(0x4129, 0x412c); showInfoTextForTile_hook(gb); return;
  }
  CYC(0x4129, 0x412c);

check_direction:
  CYC(0x412c, 0x412e); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CYCT(0x412e, 0x4130); goto check_destination;
  }
  CYC(0x412e, 0x4130);
  CYC(0x4130, 0x4131); A = B;
  CYC(0x4131, 0x4133); A = alu_swap(gb, A);
  CYC(0x4133, 0x4135); alu_and(gb, 0x03);
  CYC(0x4135, 0x4136); L = A;
  CYC(0x4136, 0x4139); A = W8(wLinkPushingDirection);
  CYC(0x4139, 0x413a); alu_cp(gb, L);
  if (!(F & FZ)) {
    CYCT(0x413a, 0x413c); goto end;
  }
  CYC(0x413a, 0x413c);

check_destination:
  CALL_C(0x413c, checkTileAfterNext_hook, 0x4398, 0x413f);
  if (!(F & FC)) {
    CYCT(0x413f, 0x4141); goto end;
  }
  CYC(0x413f, 0x4141);
  CYC(0x4141, 0x4143); A = H8(hFF8B);
  CYC(0x4143, 0x4145); alu_cp(gb, 0xda);
  if (F & FZ) {
    CYCT(0x4145, 0x4147); goto somaria_block;
  }
  CYC(0x4145, 0x4147);
  CYC(0x4147, 0x414a); SET_HL(w1ReservedInteraction1_enabled);
  CYC(0x414a, 0x414b); A = mem_rd(gb, HL);
  CYC(0x414b, 0x414c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x414c, 0x414e); goto end;
  }
  CYC(0x414c, 0x414e);
  CYC(0x414e, 0x4150); mem_wr(gb, HL, 0x01);
  CYC(0x4150, 0x4151); L = alu_inc8(gb, L);
  CYC(0x4151, 0x4153); mem_wr(gb, HL, 0x14);
  CYC(0x4153, 0x4156); A = W8(wLinkPushingDirection);
  CYC(0x4156, 0x4158); A = alu_swap(gb, A);
  CYC(0x4158, 0x4159); alu_rrca(gb);
  CYC(0x4159, 0x415b); L = 0x49;
  CYC(0x415b, 0x415c); mem_wr(gb, HL, A);
  CYC(0x415c, 0x415e); A = H8(hFF8D);
  CYC(0x415e, 0x4160); L = 0x70;
  CYC(0x4160, 0x4161); mem_wr(gb, HL, A);
  CYC(0x4161, 0x4163); L = 0x4b;
  CALL_C(0x4163, setShortPosition_hook, 0x20b8, 0x4166);
  CYC(0x4166, 0x4168); L = 0x4b;
  CYC(0x4168, 0x4169); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4169, 0x416a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x416a, 0x416c); A = H8(hFF8B);
  CYC(0x416c, 0x416e); alu_cp(gb, 0xd9);
  if (!(F & FZ)) {
    CYCT(0x416e, 0x4170); goto end;
  }
  CYC(0x416e, 0x4170);
  CYC(0x4170, 0x4173); A = W8(wTilesetFlags);
  CYC(0x4173, 0x4175); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(0x4175, 0x4177); goto end;
  }
  CYC(0x4175, 0x4177);
  CYC(0x4177, 0x417a); W8(wDisabledObjects) = A;
  goto end;

somaria_block:
  CYC(0x417e, 0x4180); C = 0x18;
  CALL_C(0x4180, findItemWithID_hook, 0x22b9, 0x4183);
  if (!(F & FZ)) {
    CYCT(0x4183, 0x4185); goto end;
  }
  CYC(0x4183, 0x4185);
  CYC(0x4185, 0x4187); L = 0x2f;
  CYC(0x4187, 0x4189); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x4189, 0x418c); A = W8(wLinkPushingDirection);
  CYC(0x418c, 0x418e); L = 0x08;
  CYC(0x418e, 0x418f); mem_wr(gb, HL, A);
  CYC(0x418f, 0x4191);

end:
  CYC(0x417a, 0x417b); alu_xor(gb, A);
  CYC(0x417b, 0x417e); resetPushingAgainstTileCounter_hook(gb);
}

void nextToKeyBlock_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4191, specialObjectCheckPushingAgainstTile_hook, 0x4329, 0x4194);
  if (F & FZ) {
    CYCT(0x4194, 0x4197); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x4194, 0x4197);
  CALL_C(0x4197, decPushingAgainstTileCounter_hook, 0x4212, 0x419a);
  if (!(F & FZ)) {
    CYCT(0x419a, 0x419b); ret_effect(gb); return;
  }
  CYC(0x419a, 0x419b);
  CALL_C(0x419b, checkAndDecKeyCount_hook, 0x4350, 0x419e);
  CYC(0x419e, 0x41a0); A = 0x02;
  if (F & FZ) {
    CYCT(0x41a0, 0x41a3); showInfoTextForTile_hook(gb); return;
  }
  CYC(0x41a0, 0x41a3);
  CALL_C(0x41a3, createKeySpriteInteraction_hook, 0x4288, 0x41a6);
  CYC(0x41a6, 0x41a8); A = 0xa0;
  CALL_C(0x41a8, setTile_hook, 0x3a9c, 0x41ab);
  CYC(0x41ab, 0x41ad); A = 0x6c;
  CALL_C(0x41ad, playSound_b00_hook, 0x0c98, 0x41b0);
  CALL_C(0x41b0, getThisRoomFlags_hook, 0x197d, 0x41b3);
  CYC(0x41b3, 0x41b5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(0x41b5, getFreeInteractionSlot_hook, 0x3aef, 0x41b8);
  if (!(F & FZ)) {
    CYCT(0x41b8, 0x41ba); goto finish;
  }
  CYC(0x41b8, 0x41ba);
  CYC(0x41ba, 0x41bc); mem_wr(gb, HL, 0x05);
  CYC(0x41bc, 0x41be); L = 0x4b;
  CYC(0x41be, 0x41c0); A = H8(hFF8D);
  CALL_C(0x41c0, setShortPosition_hook, 0x20b8, 0x41c3);

finish:
  CYC(0x41c3, 0x41c4); alu_xor(gb, A);
  CYC(0x41c4, 0x41c6); resetPushingAgainstTileCounter_hook(gb);
}

void nextToKeyDoor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x41c6, specialObjectCheckPushingAgainstTile_hook, 0x4329, 0x41c9);
  if (F & FZ) {
    CYCT(0x41c9, 0x41cb); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x41c9, 0x41cb);
  CALL_C(0x41cb, decPushingAgainstTileCounter_hook, 0x4212, 0x41ce);
  if (F & FZ) {
    CYCT(0x41ce, 0x41d0); goto check_key;
  }
  CYC(0x41ce, 0x41d0);
  CYC(0x41d0, 0x41d1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x41d1, 0x41d2); ret_effect(gb); return;
  }
  CYC(0x41d1, 0x41d2);

check_key:
  CALL_C(0x41d2, checkAndDecKeyCount_hook, 0x4350, 0x41d5);
  if (F & FZ) {
    CYCT(0x41d5, 0x41d7); goto no_key;
  }
  CYC(0x41d5, 0x41d7);
  CYC(0x41d7, 0x41da); SET_HL(w1ReservedInteraction0_enabled);
  CYC(0x41da, 0x41db); A = mem_rd(gb, HL);
  CYC(0x41db, 0x41dc); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x41dc, 0x41de); goto finish;
  }
  CYC(0x41dc, 0x41de);
  CALL_C(0x41de, createKeySpriteInteraction_hook, 0x4288, 0x41e1);
  CYC(0x41e1, 0x41e4); SET_HL(w1ReservedInteraction0_enabled);
  CYC(0x41e4, 0x41e6); mem_wr(gb, HL, 0x01);
  CYC(0x41e6, 0x41e7); L = alu_inc8(gb, L);
  CYC(0x41e7, 0x41e9); mem_wr(gb, HL, 0x1e);
  CYC(0x41e9, 0x41eb); A = H8(hFF8D);
  CYC(0x41eb, 0x41ed); L = 0x4b;
  CYC(0x41ed, 0x41ee); mem_wr(gb, HL, A);
  CYC(0x41ee, 0x41f0); L = 0x49;
  CYC(0x41f0, 0x41f1); A = B;
  CYC(0x41f1, 0x41f3); A = alu_swap(gb, A);
  CYC(0x41f3, 0x41f5); alu_and(gb, 0x0f);
  CYC(0x41f5, 0x41f6); alu_add(gb, A);
  CYC(0x41f6, 0x41f7); mem_wr(gb, HL, A);
  CYC(0x41f7, 0x41f8); push_effect(gb, DE);
  CYC(0x41f8, 0x41f9); alu_add(gb, A);
  CALL_C(0x41f9, setRoomFlagsForUnlockedKeyDoor_hook, 0x11fc, 0x41fc);
  CYC(0x41fc, 0x41fd); SET_DE(pop_effect(gb));

finish:
  CYC(0x41fd, 0x41fe); alu_xor(gb, A);
  CYC(0x41fe, 0x4200); resetPushingAgainstTileCounter_hook(gb);
  return;

no_key:
  CYC(0x4200, 0x4201); A = B;
  CYC(0x4201, 0x4203); alu_cp(gb, 0x40);
  CYC(0x4203, 0x4205); A = 0x01;
  if (!(F & FC)) {
    CYCT(0x4205, 0x4208); showInfoTextForTile_hook(gb); return;
  }
  CYC(0x4205, 0x4208);
  CYC(0x4208, 0x4209); alu_xor(gb, A);
  CYC(0x4209, 0x420c); showInfoTextForTile_hook(gb);
}

void nextToOverworldKeyhole_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4217, getThisRoomFlags_hook, 0x197d, 0x421a);
  CYC(0x421a, 0x421c); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x421c, 0x421d); ret_effect(gb); return;
  }
  CYC(0x421c, 0x421d);
  CALL_C(0x421d, specialObjectCheckPushingAgainstTile_hook, 0x4329, 0x4220);
  if (F & FZ) {
    CYCT(0x4220, 0x4222); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x4220, 0x4222);
  CALL_C(0x4222, checkFacingBottomOfTile_hook, 0x40f6, 0x4225);
  if (F & FZ) {
    CYCT(0x4225, 0x4227);
  } else {
    CYC(0x4225, 0x4227);
    CYC(0x4227, 0x4228); alu_xor(gb, A);
    CYC(0x4228, 0x4229); ret_effect(gb); return;
  }
  CALL_C(0x4229, decPushingAgainstTileCounter_hook, 0x4212, 0x422c);
  if (F & FZ) {
    CYCT(0x422c, 0x422e);
  } else {
    CYC(0x422c, 0x422e);
    CYC(0x422e, 0x422f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) {
      CYCT(0x422f, 0x4230); ret_effect(gb); return;
    }
    CYC(0x422f, 0x4230);
  }
  CYC(0x4230, 0x4233); A = W8(wActiveRoom);
  CYC(0x4233, 0x4236); SET_HL(0x4267);
  CALL_C(0x4236, findRoomSpecificData_hook, 0x1dfe, 0x4239);
  CYC(0x4239, 0x423a); B = A;
  if (!(F & FC)) {
    CYCT(0x423a, 0x423c); jumpToShowInfoText_hook(gb); return;
  }
  CYC(0x423a, 0x423c);
  CALL_C(0x423c, checkTreasureObtained_hook, 0x1748, 0x423f);
  if (!(F & FC)) {
    CYCT(0x423f, 0x4241); jumpToShowInfoText_hook(gb); return;
  }
  CYC(0x423f, 0x4241);
  CYC(0x4241, 0x4243); A = 0x6c;
  CALL_C(0x4243, playSound_b00_hook, 0x0c98, 0x4246);
  CALL_C(0x4246, getThisRoomFlags_hook, 0x197d, 0x4249);
  CYC(0x4249, 0x424b); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x424b, 0x424e); SET_HL(wTmpcfc0);
  CYC(0x424e, 0x4250); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CALL_C(0x4250, createKeySpriteInteraction_hook, 0x4288, 0x4253);
  CYC(0x4253, 0x4255); L = 0x41;
  CYC(0x4255, 0x4256); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4256, 0x4257); A = B;
  CYC(0x4257, 0x4259); alu_sub(gb, 0x42);
  CYC(0x4259, 0x425b); L = 0x42;
  CYC(0x425b, 0x425c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x425c, 0x425d); mem_wr(gb, HL, A);
  CYC(0x425d, 0x425f); A = 0x81;
  CYC(0x425f, 0x4262); W8(wDisabledObjects) = A;
  CYC(0x4262, 0x4265); W8(wMenuDisabled) = A;
  CYC(0x4265, 0x4266); alu_scf(gb);
  CYC(0x4266, 0x4267); ret_effect(gb);
}

void interactWithTileBeforeLink_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4000, 0x4003); A = W8(wLinkGrabState);
  CYC(0x4003, 0x4004); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4004, 0x4005); ret_effect(gb); return;
  }
  CYC(0x4004, 0x4005);
  CALL_C(0x4005, specialObjectGetTileInFront_hook, 0x4373, 0x4008);
  CYC(0x4008, 0x4009); E = A;
  CYC(0x4009, 0x400b); H8(hFF8B) = A;
  CYC(0x400b, 0x400c); A = C;
  CYC(0x400c, 0x400e); H8(hFF8D) = A;
  CYC(0x400e, 0x4011); SET_HL(0x43b2);
  CALL_C(0x4011, lookupCollisionTable_paramE_hook, 0x1e20, 0x4014);
  if (!(F & FC)) {
    CYCT(0x4014, 0x4017); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(0x4014, 0x4017);
  CYC(0x4017, 0x4018); B = A;
  CYC(0x4018, 0x401a); alu_and(gb, 0x0f);
  CYC(0x401a, 0x401b); interactable_tiles_jump_table_from_rst(gb, 0x401b);
  switch (HL) {
    case 0x410e: nextToPushableBlock_hook(gb); return;
    case 0x4191: nextToKeyBlock_hook(gb); return;
    case 0x41c6: nextToKeyDoor_hook(gb); return;
    case 0x42c3: nextToTileWithInfoText_hook(gb); return;
    case 0x402d: nextToChestTile_hook(gb); return;
    case 0x40b4: nextToSignTile_hook(gb); return;
    case 0x4217: nextToOverworldKeyhole_hook(gb); return;
    case 0x4299: nextToSubrosiaKeydoor_hook(gb); return;
    case 0x429b: nextToGhiniSpawner_hook(gb); return;
  }
  hook_continue(gb, HL, sp0_);
}
