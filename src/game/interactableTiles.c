#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(resetPushingAgainstTileCounter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x14;
  CYC(b_+2, b_+5); W8(wPushingAgainstTileCounter) = A;
  CYC(b_+5, b_+6); ret_effect(gb);
}

void decPushingAgainstTileCounter_hook(GB *gb) {
  BASE(decPushingAgainstTileCounter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wPushingAgainstTileCounter);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); ret_effect(gb);
}

void specialObjectGetTileInFront_hook(GB *gb) {
  BASE(specialObjectGetTileInFront);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x08;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(nextTileOffsets));
  CYC(b_+6, b_+7); add_double_index_to_hl_from_rst(gb, SYM(specialObjectGetTileAtOffset));
  specialObjectGetTileAtOffset_hook(gb);
}

void specialObjectGetTileAtOffset_hook(GB *gb) {
  BASE(specialObjectGetTileAtOffset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x0b;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+6); alu_and(gb, 0xf0);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+8); SET_HL(HL + 1);
  CYC(b_+8, b_+10); E = 0x0d;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+14); A = alu_swap(gb, A);
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
  CYC(b_+16, b_+17); alu_or(gb, C);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); B = 0xcf;
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void checkTileAfterNext_hook(GB *gb) {
  BASE(checkTileAfterNext);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkPushingDirection);
  CYC(b_+3, b_+6); SET_HL(b_+18);
  CYC(b_+6, b_+7); add_double_index_to_hl_from_rst(gb, b_+7);
  CALL_C(b_+7, specialObjectGetTileAtOffset_hook, SYM(specialObjectGetTileAtOffset), b_+10);
  CYC(b_+10, b_+12); B = 0xce;
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); alu_scf(gb);
  CYC(b_+17, b_+18); ret_effect(gb);
}

void jumpToShowInfoText_hook(GB *gb) {
  BASE(jumpToShowInfoText);
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+5); showInfoTextForTile_hook(gb);
}

void createKeySpriteInteraction_hook(GB *gb) {
  BASE(createKeySpriteInteraction);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x17);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); A = H8(hFF8B);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+12); A = H8(hFF8D);
  CYC(b_+12, b_+14); L = 0x4b;
  CYC(b_+14, b_+17); setShortPosition_hook(gb);
}

void nextToSubrosiaKeydoor_hook(GB *gb) {
  BASE(nextToSubrosiaKeydoor);
  CYC(b_+0, b_+1); alu_scf(gb);
  CYC(b_+1, b_+2); ret_effect(gb);
}

void nextToGhiniSpawner_hook(GB *gb) {
  BASE(nextToGhiniSpawner);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wIsMaplePresent);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, specialObjectCheckPushingAgainstTile_hook, SYM(specialObjectCheckPushingAgainstTile), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+11); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+8, b_+11);
  CALL_C(b_+11, decPushingAgainstTileCounter_hook, SYM(decPushingAgainstTileCounter), b_+14);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+15); ret_effect(gb); return;
  }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = H8(hFF8D);
  CYC(b_+17, b_+18); L = A;
  CYC(b_+18, b_+20); H = 0xcf;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x00);
  CALL_C(b_+22, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+25);
  CALL_C(b_+25, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+28);
  if (!(F & FZ)) {
    CYCT(b_+28, b_+29); ret_effect(gb); return;
  }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x17);
  CYC(b_+31, b_+32); L = alu_inc8(gb, L);
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+33, b_+35); L = 0x8b;
  CYC(b_+35, b_+36); mem_wr(gb, HL, B);
  CYC(b_+36, b_+38); L = 0x8d;
  CYC(b_+38, b_+39); mem_wr(gb, HL, C);
  CYC(b_+39, b_+40); ret_effect(gb);
}

void nextToTileWithInfoText_hook(GB *gb) {
  BASE(nextToTileWithInfoText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectCheckPushingAgainstTile_hook, SYM(specialObjectCheckPushingAgainstTile), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+6); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, decPushingAgainstTileCounter_hook, SYM(decPushingAgainstTileCounter), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, resetPushingAgainstTileCounter_hook, SYM(resetPushingAgainstTileCounter), b_+13);
  CYC(b_+13, b_+14); A = B;
  CYC(b_+14, b_+16); A = alu_swap(gb, A);
  CYC(b_+16, b_+18); alu_and(gb, 0x0f);
  CYC(b_+18, b_+19); interactable_tiles_jump_table_from_rst(gb, b_+19);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+29) {
      CYC(b_+29, b_+31); A = 0x16;
      CALL_C(b_+31, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+34);
      CYC(b_+34, b_+35); alu_ccf(gb);
      if (!(F & FC)) {
        CYCT(b_+35, b_+36); ret_effect(gb); return;
      }
      CYC(b_+35, b_+36);
      CYC(b_+36, b_+38); A = 0x03;
      CYC(b_+38, b_+40); showInfoTextForTile_hook(gb); return;
    }
    else if (jt_ == b_+40) {
      CYC(b_+40, b_+42); A = 0x05;
      CYC(b_+42, b_+44); showInfoTextForTile_hook(gb); return;
    }
    else if (jt_ == b_+44) {
      CYC(b_+44, b_+46); A = 0x06;
      CYC(b_+46, b_+48); showInfoTextForTile_hook(gb); return;
    }
    else if (jt_ == b_+48) {
      CYC(b_+48, b_+50); A = 0x07;
      CYC(b_+50, b_+52); showInfoTextForTile_hook(gb); return;
    }
    else if (jt_ == b_+52) {
      CYC(b_+52, b_+54); A = 0x04;
      CYC(b_+54, b_+56); showInfoTextForTile_hook(gb); return;
    }
  } while (0);
  hook_continue(gb, HL, sp0_);
}

void showInfoTextForTile_hook(GB *gb) {
  BASE(showInfoTextForTile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(b_+26);
  CYC(b_+3, b_+4); add_double_index_to_hl_from_rst(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CALL_C(b_+7, resetPushingAgainstTileCounter_hook, SYM(resetPushingAgainstTileCounter), b_+10);
  CYC(b_+10, b_+13); SET_HL(wInformativeTextsShown);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, B);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); B = 0x51;
  CALL_C(b_+21, showText_hook, SYM(showText), b_+24);
  CYC(b_+24, b_+25); alu_scf(gb);
  CYC(b_+25, b_+26); ret_effect(gb);
}

static void special_object_check_position_component(GB *gb) {
  BASE(specialObjectCheckPushingAgainstTile);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_and(gb, 0x0f);
  CYC(b_+34, b_+36); alu_sub(gb, 0x03);
  CYC(b_+36, b_+38); alu_cp(gb, 0x0b);
  CYC(b_+38, b_+39); ret_effect(gb);
}

static void special_object_check_position(GB *gb) {
  BASE(specialObjectCheckPushingAgainstTile);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0x0b;
  CYC(b_+25, b_+28); push_effect(gb, b_+28);
  special_object_check_position_component(gb);
  if (F & FC) {
    CYCT(b_+28, b_+29); ret_effect(gb); return;
  }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+31); L = 0x0d;
  special_object_check_position_component(gb);
}

void specialObjectCheckPushingAgainstTile_hook(GB *gb) {
  BASE(specialObjectCheckPushingAgainstTile);
  CYC(b_+0, b_+3); A = W8(wLinkPushingDirection);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+4, b_+6); goto not_pushing;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wLinkAngle);
  CYC(b_+9, b_+11); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13); goto not_pushing;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); push_effect(gb, b_+16);
  special_object_check_position(gb);
  if (!(F & FC)) {
    CYCT(b_+16, b_+18); goto not_pushing;
  }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_or(gb, D);
  CYC(b_+19, b_+20); ret_effect(gb); return;

not_pushing:
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void checkAndDecKeyCount_hook(GB *gb) {
  BASE(checkAndDecKeyCount);
  CYC(b_+0, b_+3); A = W8(wDungeonIndex);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+9); alu_cp(gb, 0x40);
  CYC(b_+9, b_+11); H = 0xc6;
  CYC(b_+11, b_+14); A = W8(wDungeonIndex);
  if (!(F & FC)) {
    CYCT(b_+14, b_+16);
    CYC(b_+30, b_+32); L = 0x82;
    CYC(b_+32, b_+35); checkFlag_hook(gb); return;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); alu_add(gb, 0x72);
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+21, b_+22); ret_effect(gb); return;
  }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+23, b_+26); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+26, b_+28); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+28, b_+29); alu_or(gb, H);
  CYC(b_+29, b_+30); ret_effect(gb);
}

static void facing_bottom_abort(GB *gb) {
  BASE(checkFacingBottomOfTile);
  CYC(b_+21, b_+22); SET_AF(pop_effect(gb));
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void checkFacingBottomOfTile_hook(GB *gb) {
  BASE(checkFacingBottomOfTile);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_direction);
  CYC(b_+3, b_+6); SET_HL(b_+17);
  CYC(b_+6, b_+7); add_a_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_adjacentWallsBitset);
  CYC(b_+10, b_+11); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14); facing_bottom_abort(gb); return;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 0xc0);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void checkFacingBottomOfTileAndPressedA_hook(GB *gb) {
  BASE(checkFacingBottomOfTileAndPressedA);
  CYC(b_+0, b_+3); A = W8(wGameKeysJustPressed);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+5, b_+7); facing_bottom_abort(gb); return;
  }
  CYC(b_+5, b_+7);
  checkFacingBottomOfTile_hook(gb);
}

void nextToChestTile_hook(GB *gb) {
  BASE(nextToChestTile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkFacingBottomOfTileAndPressedA_hook, SYM(checkFacingBottomOfTileAndPressedA), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5);
  } else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+8); SET_BC(0x510d);
    CALL_C(b_+8, showText_hook, SYM(showText), b_+11);
    CYC(b_+11, b_+12); alu_scf(gb);
    CYC(b_+12, b_+13); ret_effect(gb); return;
  }
  CYC(b_+13, b_+16); A = W8(wInShop);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+17, b_+19); goto open_chest;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = W8(wcca1);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+23, b_+25); goto open_chest;
  }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); A = W8(wcca2);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+30); ret_effect(gb); return;
  }
  CYC(b_+29, b_+30);

open_chest:
  CYC(b_+30, b_+31); A = C;
  CYC(b_+31, b_+34); W8(wcca2) = A;
  CYC(b_+34, b_+36); A = 0xf0;
  CALL_C(b_+36, setTile_hook, SYM(setTile), b_+39);
  CYC(b_+39, b_+41); A = 0x6c;
  CALL_C(b_+41, playSound_b00_hook, SYM(playSound_b00), b_+44);
  CYC(b_+44, b_+47); A = W8(wInShop);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+49); ret_effect(gb); return;
  }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+52); A = W8(wcca1);
  CYC(b_+52, b_+53); alu_or(gb, A);
  CYC(b_+53, b_+54); alu_scf(gb);
  if (!(F & FZ)) {
    CYCT(b_+54, b_+55); ret_effect(gb); return;
  }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+58); SET_HL(w1ReservedInteraction0);
  CYC(b_+58, b_+60); B = 0x40;
  CALL_C(b_+60, clearMemory_hook, SYM(clearMemory), b_+63);
  CYC(b_+63, b_+66); A = W8(wChestContentsOverride);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+67, b_+69);
    CALL_C(b_+76, getChestData_hook, SYM(getChestData), b_+79);
  } else {
    CYC(b_+67, b_+69);
    CYC(b_+69, b_+70); B = A;
    CYC(b_+70, b_+73); A = mem_rd(gb, wChestContentsOverride + 1);
    CYC(b_+73, b_+74); C = A;
    CYC(b_+74, b_+76);
  }
  CYC(b_+79, b_+80); A = B;
  CYC(b_+80, b_+81); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+81, b_+83); goto record_opened;
  }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); A = 0x83;
  CYC(b_+85, b_+88); W8(wDisabledObjects) = A;
  CYC(b_+88, b_+91); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+91, b_+94); SET_HL(w1ReservedInteraction0_enabled);
  CYC(b_+94, b_+96); A = 0x81;
  CYC(b_+96, b_+97); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x60);
  CYC(b_+99, b_+100); L = alu_inc8(gb, L);
  CYC(b_+100, b_+101); mem_wr(gb, HL, B);
  CYC(b_+101, b_+102); L = alu_inc8(gb, L);
  CYC(b_+102, b_+103); mem_wr(gb, HL, C);
  CYC(b_+103, b_+105); L = 0x4b;
  CYC(b_+105, b_+108); A = W8(wcca2);
  CYC(b_+108, b_+109); B = A;
  CYC(b_+109, b_+111); alu_and(gb, 0xf0);
  CYC(b_+111, b_+112); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+112, b_+113); L = alu_inc8(gb, L);
  CYC(b_+113, b_+114); A = B;
  CYC(b_+114, b_+116); A = alu_swap(gb, A);
  CYC(b_+116, b_+118); alu_and(gb, 0xf0);
  CYC(b_+118, b_+120); alu_or(gb, 0x08);
  CYC(b_+120, b_+121); mem_wr(gb, HL, A);

record_opened:
  CALL_C(b_+121, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+124);
  CYC(b_+124, b_+126); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+126, b_+127); alu_xor(gb, A);
  CYC(b_+127, b_+130); W8(wChestContentsOverride) = A;
  CYC(b_+130, b_+133); mem_wr(gb, wChestContentsOverride + 1, A);
  CYC(b_+133, b_+134); alu_scf(gb);
  CYC(b_+134, b_+135); ret_effect(gb);
}

void nextToSignTile_hook(GB *gb) {
  BASE(nextToSignTile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkFacingBottomOfTileAndPressedA_hook, SYM(checkFacingBottomOfTileAndPressedA), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x510e);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+8); goto show_text;
  }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); A = W8(wActiveGroup);
  CYC(b_+11, b_+14); SET_HL(SYM(signTextGroupTable));
  CYC(b_+14, b_+15); add_double_index_to_hl_from_rst(gb, b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); H = mem_rd(gb, HL);
  CYC(b_+17, b_+18); L = A;
  CYC(b_+18, b_+21); A = W8(wActiveRoom);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+24); A = H8(hFF8D);
  CYC(b_+24, b_+25); C = A;
  for (;;) {
    CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+26, b_+27); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+27, b_+29);
      CYC(b_+51, b_+54); SET_BC(0x0901);
      goto show_text;
    }
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+30); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+30, b_+32);
      CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+37, b_+38); alu_cp(gb, B);
      if (F & FZ) {
        CYCT(b_+38, b_+40);
        CYC(b_+43, b_+44); C = mem_rd(gb, HL);
        CYC(b_+44, b_+46); B = 0x2e;
        CALL_C(b_+46, showText_hook, SYM(showText), b_+49);
        CYC(b_+49, b_+50); alu_scf(gb);
        CYC(b_+50, b_+51); ret_effect(gb); return;
      }
      CYC(b_+38, b_+40);
      CYC(b_+40, b_+41); SET_HL(HL + 1);
      CYC(b_+41, b_+43);
    } else {
      CYC(b_+30, b_+32);
      CYC(b_+32, b_+33); SET_HL(HL + 1);
      CYC(b_+33, b_+34); SET_HL(HL + 1);
      CYC(b_+34, b_+36);
    }
  }

show_text:
  CALL_C(b_+54, showText_hook, SYM(showText), b_+57);
  CYC(b_+57, b_+58); alu_scf(gb);
  CYC(b_+58, b_+59); ret_effect(gb);
}

void nextToPushableBlock_hook(GB *gb) {
  BASE(nextToPushableBlock);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, specialObjectCheckPushingAgainstTile_hook, SYM(specialObjectCheckPushingAgainstTile), b_+9);
  if (F & FZ) {
    CYCT(b_+9, b_+12); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+9, b_+12);
  CALL_C(b_+12, decPushingAgainstTileCounter_hook, SYM(decPushingAgainstTileCounter), b_+15);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); alu_bit(gb, 6, B);
  if (F & FZ) {
    CYCT(b_+18, b_+20); goto check_direction;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); A = 0x16;
  CALL_C(b_+22, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+25);
  CYC(b_+25, b_+27); A = 0x03;
  if (!(F & FC)) {
    CYCT(b_+27, b_+30); showInfoTextForTile_hook(gb); return;
  }
  CYC(b_+27, b_+30);

check_direction:
  CYC(b_+30, b_+32); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+34); goto check_destination;
  }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+37); A = alu_swap(gb, A);
  CYC(b_+37, b_+39); alu_and(gb, 0x03);
  CYC(b_+39, b_+40); L = A;
  CYC(b_+40, b_+43); A = W8(wLinkPushingDirection);
  CYC(b_+43, b_+44); alu_cp(gb, L);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+46); goto end;
  }
  CYC(b_+44, b_+46);

check_destination:
  CALL_C(b_+46, checkTileAfterNext_hook, SYM(checkTileAfterNext), b_+49);
  if (!(F & FC)) {
    CYCT(b_+49, b_+51); goto end;
  }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); A = H8(hFF8B);
  CYC(b_+53, b_+55); alu_cp(gb, 0xda);
  if (F & FZ) {
    CYCT(b_+55, b_+57); goto somaria_block;
  }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+60); SET_HL(w1ReservedInteraction1_enabled);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+62); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+62, b_+64); goto end;
  }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x01);
  CYC(b_+66, b_+67); L = alu_inc8(gb, L);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x14);
  CYC(b_+69, b_+72); A = W8(wLinkPushingDirection);
  CYC(b_+72, b_+74); A = alu_swap(gb, A);
  CYC(b_+74, b_+75); alu_rrca(gb);
  CYC(b_+75, b_+77); L = 0x49;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); A = H8(hFF8D);
  CYC(b_+80, b_+82); L = 0x70;
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
  CYC(b_+83, b_+85); L = 0x4b;
  CALL_C(b_+85, setShortPosition_hook, SYM(setShortPosition), b_+88);
  CYC(b_+88, b_+90); L = 0x4b;
  CYC(b_+90, b_+91); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+92, b_+94); A = H8(hFF8B);
  CYC(b_+94, b_+96); alu_cp(gb, 0xd9);
  if (!(F & FZ)) {
    CYCT(b_+96, b_+98); goto end;
  }
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+101); A = W8(wTilesetFlags);
  CYC(b_+101, b_+103); alu_and(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+103, b_+105); goto end;
  }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+108); W8(wDisabledObjects) = A;
  goto end;

somaria_block:
  CYC(b_+112, b_+114); C = 0x18;
  CALL_C(b_+114, findItemWithID_hook, SYM(findItemWithID), b_+117);
  if (!(F & FZ)) {
    CYCT(b_+117, b_+119); goto end;
  }
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+121); L = 0x2f;
  CYC(b_+121, b_+123); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+123, b_+126); A = W8(wLinkPushingDirection);
  CYC(b_+126, b_+128); L = 0x08;
  CYC(b_+128, b_+129); mem_wr(gb, HL, A);
  CYC(b_+129, b_+131);

end:
  CYC(b_+108, b_+109); alu_xor(gb, A);
  CYC(b_+109, b_+112); resetPushingAgainstTileCounter_hook(gb);
}

void nextToKeyBlock_hook(GB *gb) {
  BASE(nextToKeyBlock);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectCheckPushingAgainstTile_hook, SYM(specialObjectCheckPushingAgainstTile), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+6); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+3, b_+6);
  CALL_C(b_+6, decPushingAgainstTileCounter_hook, SYM(decPushingAgainstTileCounter), b_+9);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CALL_C(b_+10, checkAndDecKeyCount_hook, SYM(checkAndDecKeyCount), b_+13);
  CYC(b_+13, b_+15); A = 0x02;
  if (F & FZ) {
    CYCT(b_+15, b_+18); showInfoTextForTile_hook(gb); return;
  }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, createKeySpriteInteraction_hook, SYM(createKeySpriteInteraction), b_+21);
  CYC(b_+21, b_+23); A = 0xa0;
  CALL_C(b_+23, setTile_hook, SYM(setTile), b_+26);
  CYC(b_+26, b_+28); A = 0x6c;
  CALL_C(b_+28, playSound_b00_hook, SYM(playSound_b00), b_+31);
  CALL_C(b_+31, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+36, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+39);
  if (!(F & FZ)) {
    CYCT(b_+39, b_+41); goto finish;
  }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x05);
  CYC(b_+43, b_+45); L = 0x4b;
  CYC(b_+45, b_+47); A = H8(hFF8D);
  CALL_C(b_+47, setShortPosition_hook, SYM(setShortPosition), b_+50);

finish:
  CYC(b_+50, b_+51); alu_xor(gb, A);
  CYC(b_+51, b_+53); resetPushingAgainstTileCounter_hook(gb);
}

void nextToKeyDoor_hook(GB *gb) {
  BASE(nextToKeyDoor);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectCheckPushingAgainstTile_hook, SYM(specialObjectCheckPushingAgainstTile), b_+3);
  if (F & FZ) {
    CYCT(b_+3, b_+5); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, decPushingAgainstTileCounter_hook, SYM(decPushingAgainstTileCounter), b_+8);
  if (F & FZ) {
    CYCT(b_+8, b_+10); goto check_key;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);

check_key:
  CALL_C(b_+12, checkAndDecKeyCount_hook, SYM(checkAndDecKeyCount), b_+15);
  if (F & FZ) {
    CYCT(b_+15, b_+17); goto no_key;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); SET_HL(w1ReservedInteraction0_enabled);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+22, b_+24); goto finish;
  }
  CYC(b_+22, b_+24);
  CALL_C(b_+24, createKeySpriteInteraction_hook, SYM(createKeySpriteInteraction), b_+27);
  CYC(b_+27, b_+30); SET_HL(w1ReservedInteraction0_enabled);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x1e);
  CYC(b_+35, b_+37); A = H8(hFF8D);
  CYC(b_+37, b_+39); L = 0x4b;
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); L = 0x49;
  CYC(b_+42, b_+43); A = B;
  CYC(b_+43, b_+45); A = alu_swap(gb, A);
  CYC(b_+45, b_+47); alu_and(gb, 0x0f);
  CYC(b_+47, b_+48); alu_add(gb, A);
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);
  CYC(b_+49, b_+50); push_effect(gb, DE);
  CYC(b_+50, b_+51); alu_add(gb, A);
  CALL_C(b_+51, setRoomFlagsForUnlockedKeyDoor_hook, SYM(setRoomFlagsForUnlockedKeyDoor), b_+54);
  CYC(b_+54, b_+55); SET_DE(pop_effect(gb));

finish:
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, b_+58); resetPushingAgainstTileCounter_hook(gb);
  return;

no_key:
  CYC(b_+58, b_+59); A = B;
  CYC(b_+59, b_+61); alu_cp(gb, 0x40);
  CYC(b_+61, b_+63); A = 0x01;
  if (!(F & FC)) {
    CYCT(b_+63, b_+66); showInfoTextForTile_hook(gb); return;
  }
  CYC(b_+63, b_+66);
  CYC(b_+66, b_+67); alu_xor(gb, A);
  CYC(b_+67, b_+70); showInfoTextForTile_hook(gb);
}

void nextToOverworldKeyhole_hook(GB *gb) {
  BASE(nextToOverworldKeyhole);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, specialObjectCheckPushingAgainstTile_hook, SYM(specialObjectCheckPushingAgainstTile), b_+9);
  if (F & FZ) {
    CYCT(b_+9, b_+11); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+9, b_+11);
  CALL_C(b_+11, checkFacingBottomOfTile_hook, SYM(checkFacingBottomOfTile), b_+14);
  if (F & FZ) {
    CYCT(b_+14, b_+16);
  } else {
    CYC(b_+14, b_+16);
    CYC(b_+16, b_+17); alu_xor(gb, A);
    CYC(b_+17, b_+18); ret_effect(gb); return;
  }
  CALL_C(b_+18, decPushingAgainstTileCounter_hook, SYM(decPushingAgainstTileCounter), b_+21);
  if (F & FZ) {
    CYCT(b_+21, b_+23);
  } else {
    CYC(b_+21, b_+23);
    CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) {
      CYCT(b_+24, b_+25); ret_effect(gb); return;
    }
    CYC(b_+24, b_+25);
  }
  CYC(b_+25, b_+28); A = W8(wActiveRoom);
  CYC(b_+28, b_+31); SET_HL(b_+80);
  CALL_C(b_+31, findRoomSpecificData_hook, SYM(findRoomSpecificData), b_+34);
  CYC(b_+34, b_+35); B = A;
  if (!(F & FC)) {
    CYCT(b_+35, b_+37); jumpToShowInfoText_hook(gb); return;
  }
  CYC(b_+35, b_+37);
  CALL_C(b_+37, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+40);
  if (!(F & FC)) {
    CYCT(b_+40, b_+42); jumpToShowInfoText_hook(gb); return;
  }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); A = 0x6c;
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
  CALL_C(b_+47, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+50);
  CYC(b_+50, b_+52); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+52, b_+55); SET_HL(wTmpcfc0);
  CYC(b_+55, b_+57); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CALL_C(b_+57, createKeySpriteInteraction_hook, SYM(createKeySpriteInteraction), b_+60);
  CYC(b_+60, b_+62); L = 0x41;
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+63, b_+64); A = B;
  CYC(b_+64, b_+66); alu_sub(gb, 0x42);
  CYC(b_+66, b_+68); L = 0x42;
  CYC(b_+68, b_+69); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+69, b_+70); mem_wr(gb, HL, A);
  CYC(b_+70, b_+72); A = 0x81;
  CYC(b_+72, b_+75); W8(wDisabledObjects) = A;
  CYC(b_+75, b_+78); W8(wMenuDisabled) = A;
  CYC(b_+78, b_+79); alu_scf(gb);
  CYC(b_+79, b_+80); ret_effect(gb);
}

void interactWithTileBeforeLink_b06_hook(GB *gb) {
  BASE(interactWithTileBeforeLink_b06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLinkGrabState);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, specialObjectGetTileInFront_hook, SYM(specialObjectGetTileInFront), b_+8);
  CYC(b_+8, b_+9); E = A;
  CYC(b_+9, b_+11); H8(hFF8B) = A;
  CYC(b_+11, b_+12); A = C;
  CYC(b_+12, b_+14); H8(hFF8D) = A;
  CYC(b_+14, b_+17); SET_HL(SYM(interactableTilesTable));
  CALL_C(b_+17, lookupCollisionTable_paramE_hook, SYM(lookupCollisionTable_paramE), b_+20);
  if (!(F & FC)) {
    CYCT(b_+20, b_+23); resetPushingAgainstTileCounter_hook(gb); return;
  }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+26); alu_and(gb, 0x0f);
  CYC(b_+26, b_+27); interactable_tiles_jump_table_from_rst(gb, b_+27);
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(nextToPushableBlock)) { nextToPushableBlock_hook(gb); return; }
    else if (jt_ == SYM(nextToKeyBlock)) { nextToKeyBlock_hook(gb); return; }
    else if (jt_ == SYM(nextToKeyDoor)) { nextToKeyDoor_hook(gb); return; }
    else if (jt_ == SYM(nextToTileWithInfoText)) { nextToTileWithInfoText_hook(gb); return; }
    else if (jt_ == SYM(nextToChestTile)) { nextToChestTile_hook(gb); return; }
    else if (jt_ == SYM(nextToSignTile)) { nextToSignTile_hook(gb); return; }
    else if (jt_ == SYM(nextToOverworldKeyhole)) { nextToOverworldKeyhole_hook(gb); return; }
    else if (jt_ == SYM(nextToSubrosiaKeydoor)) { nextToSubrosiaKeydoor_hook(gb); return; }
    else if (jt_ == SYM(nextToGhiniSpawner)) { nextToGhiniSpawner_hook(gb); return; }
  } while (0);
  hook_continue(gb, HL, sp0_);
}
