#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(applyAllTileSubstitutions), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(applyAllTileSubstitutions), (from), (to), true)

void replaceBreakableTileOverPortal_hook(GB *gb);
void removeBreakableTileForTimeWarp_hook(GB *gb);
void replaceBreakableTileOverLinkTimeWarpingIn_hook(GB *gb);
void replacePollutionWithWaterIfPollutionFixed_hook(GB *gb);
void replaceTiles_hook(GB *gb);
void applyStandardTileSubstitutions_hook(GB *gb);
void applyStandardTileSubstitutions__locFunc_hook(GB *gb);

static void tile_substitutions_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void applyAllTileSubstitutions_hook(GB *gb) {
  BASE(applyAllTileSubstitutions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, replacePollutionWithWaterIfPollutionFixed_hook, SYM(replacePollutionWithWaterIfPollutionFixed), b_+3);
  CALL_C(b_+3, applySingleTileChanges_hook, SYM(applySingleTileChanges), b_+6);
  CALL_C(b_+6, applyStandardTileSubstitutions_hook, SYM(applyStandardTileSubstitutions), b_+9);
  CALL_C(b_+9, replaceOpenedChest_hook, SYM(replaceOpenedChest), b_+12);
  CYC(b_+12, b_+15); A = W8(wActiveGroup);
  CYC(b_+15, b_+17); alu_and(gb, 0x06);
  CYC(b_+17, b_+19); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
  } else {
    CYC(b_+19, b_+21);
    CALL_C(b_+21, replaceShutterForLinkEntering_hook, SYM(replaceShutterForLinkEntering), b_+24);
    CALL_C(b_+24, replaceSwitchTiles_hook, SYM(replaceSwitchTiles), b_+27);
    CALL_C(b_+27, replaceToggleBlocks_hook, SYM(replaceToggleBlocks), b_+30);
    CALL_C(b_+30, replaceJabuTilesIfUnderwater_hook, SYM(replaceJabuTilesIfUnderwater), b_+33);
  }
  CALL_C(b_+33, applyRoomSpecificTileChanges_hook, SYM(applyRoomSpecificTileChanges), b_+36);
  CYC(b_+36, b_+39); A = W8(wActiveGroup);
  CYC(b_+39, b_+41); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(b_+41, b_+42); ret_effect(gb);
    return;
  }
  CYC(b_+41, b_+42);
  CALL_C(b_+42, replaceBreakableTileOverPortal_hook, SYM(replaceBreakableTileOverPortal), b_+45);
  CALL_C(b_+45, replaceBreakableTileOverLinkTimeWarpingIn_hook, SYM(replaceBreakableTileOverLinkTimeWarpingIn), b_+48);
  CYC(b_+48, b_+51); A = W8(wLinkTimeWarpTile);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+52, b_+53); ret_effect(gb);
    return;
  }
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+54); C = A;
  CYC(b_+54, b_+55); C = alu_dec8(gb, C);
  CYC(b_+55, b_+57); B = wRoomLayout >> 8;
  CYC(b_+57, b_+58); A = mem_rd(gb, BC);
  CYC(b_+58, b_+59); E = A;
  CYC(b_+59, b_+62); SET_HL(SYM(timewarpReturnTileReplacementDict));
  CALL_C(b_+62, lookupKey_hook, SYM(lookupKey), b_+65);
  if (!(F & FC)) {
    CYCT(b_+65, b_+66); ret_effect(gb);
    return;
  }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+67); mem_wr(gb, BC, A);
  CYC(b_+67, SYM(timewarpReturnTileReplacementDict)); ret_effect(gb);
}

void replaceBreakableTileOverPortal_hook(GB *gb) {
  BASE(replaceBreakableTileOverPortal);
  CYC(b_+0, b_+3); SET_HL(wPortalGroup);
  CYC(b_+3, b_+6); A = W8(wActiveGroup);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+12); A = W8(wActiveRoom);
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+13, b_+14); ret_effect(gb);
    return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, SYM(removeBreakableTileForTimeWarp)); C = mem_rd(gb, HL);
  removeBreakableTileForTimeWarp_hook(gb);
}

void removeBreakableTileForTimeWarp_hook(GB *gb) {
  BASE(removeBreakableTileForTimeWarp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = wRoomLayout >> 8;
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+4); E = A;
  CYC(b_+4, b_+7); SET_HL(SYM(timewarpEntryTileReplacementDict));
  CALL_C(b_+7, lookupKey_hook, SYM(lookupKey), b_+10);
  if (!(F & FC)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, BC, A);
  CYC(b_+12, SYM(timewarpEntryTileReplacementDict)); ret_effect(gb);
}

void replaceBreakableTileOverLinkTimeWarpingIn_hook(GB *gb) {
  BASE(replaceBreakableTileOverLinkTimeWarpingIn);
  CYC(b_+0, b_+3); A = W8(wWarpTransition);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = W8(wWarpDestPos);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, SYM(replacePollutionWithWaterIfPollutionFixed)); removeBreakableTileForTimeWarp_hook(gb);
}

void replacePollutionWithWaterIfPollutionFixed_hook(GB *gb) {
  BASE(replacePollutionWithWaterIfPollutionFixed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x30;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wTilesetFlags);
  CYC(b_+9, b_+11); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(b_+11, b_+12); ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_DE(b_+24);
  CYC(b_+15, b_+17); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+22); SET_DE(b_+27);
  }
  CYC(b_+22, b_+24); replaceTiles_hook(gb);
}

void replaceTiles_hook(GB *gb) {
  BASE(replaceTiles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, DE);
    CYC(b_+1, b_+2); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+2, b_+3); ret_effect(gb);
      return;
    }
    CYC(b_+2, b_+3);
    CYC(b_+3, b_+4); B = A;
    CYC(b_+4, b_+5); SET_DE(DE + 1);
    CYC(b_+5, b_+6); A = mem_rd(gb, DE);
    CYC(b_+6, b_+7); SET_DE(DE + 1);
    CALL_C(b_+7, findTileInRoom_hook, SYM(findTileInRoom), b_+10);
    if (!(F & FZ)) {
      CYCT(b_+10, b_+12);
      continue;
    }
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+13); mem_wr(gb, HL, B);
    CYC(b_+13, b_+14); C = A;
    CYC(b_+14, b_+15); A = L;
    CYC(b_+15, b_+16); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+16, b_+18);
      continue;
    }
    CYC(b_+16, b_+18);
    for (;;) {
      CYC(b_+18, b_+19); L = alu_dec8(gb, L);
      CYC(b_+19, b_+20); A = C;
      CALL_C(b_+20, backwardsSearch_hook, SYM(backwardsSearch), b_+23);
      if (!(F & FZ)) {
        CYCT(b_+23, b_+25);
        break;
      }
      CYC(b_+23, b_+25);
      CYC(b_+25, b_+26); mem_wr(gb, HL, B);
      CYC(b_+26, b_+27); C = A;
      CYC(b_+27, b_+28); A = L;
      CYC(b_+28, b_+29); alu_or(gb, A);
      if (F & FZ) {
        CYCT(b_+29, b_+31);
        break;
      }
      CYC(b_+29, b_+31);
      CYC(b_+31, SYM(applyStandardTileSubstitutions));
    }
  }
}

void applyStandardTileSubstitutions_hook(GB *gb) {
  BASE(applyStandardTileSubstitutions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); hram_wr(gb, 0x8b, A);
  CYC(b_+5, b_+8); SET_HL(SYM(standardTileSubstitutions));
  CYC(b_+8, b_+10); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(b_+10, applyStandardTileSubstitutions__locFunc_hook, b_+51, b_+13);
  else CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); SET_HL(SYM(standardTileSubstitutions__bit1));
  CYC(b_+16, b_+18); A = hram_rd(gb, 0x8b);
  CYC(b_+18, b_+20); alu_bit(gb, 1, A);
  if (!(F & FZ)) CALL_C_CC(b_+20, applyStandardTileSubstitutions__locFunc_hook, b_+51, b_+23);
  else CYC(b_+20, b_+23);
  CYC(b_+23, b_+26); SET_HL(SYM(standardTileSubstitutions__bit2));
  CYC(b_+26, b_+28); A = hram_rd(gb, 0x8b);
  CYC(b_+28, b_+30); alu_bit(gb, 2, A);
  if (!(F & FZ)) CALL_C_CC(b_+30, applyStandardTileSubstitutions__locFunc_hook, b_+51, b_+33);
  else CYC(b_+30, b_+33);
  CYC(b_+33, b_+36); SET_HL(SYM(standardTileSubstitutions__bit3));
  CYC(b_+36, b_+38); A = hram_rd(gb, 0x8b);
  CYC(b_+38, b_+40); alu_bit(gb, 3, A);
  if (!(F & FZ)) CALL_C_CC(b_+40, applyStandardTileSubstitutions__locFunc_hook, b_+51, b_+43);
  else CYC(b_+40, b_+43);
  CYC(b_+43, b_+46); SET_HL(SYM(standardTileSubstitutions__bit7));
  CYC(b_+46, b_+48); A = hram_rd(gb, 0x8b);
  CYC(b_+48, b_+50); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+50, b_+51); ret_effect(gb);
    return;
  }
  CYC(b_+50, b_+51);
  applyStandardTileSubstitutions__locFunc_hook(gb);
}

void applyStandardTileSubstitutions__locFunc_hook(GB *gb) {
  BASE(applyStandardTileSubstitutions);
  CYC(b_+51, b_+54); A = W8(wActiveCollisions);
  CYC(b_+54, b_+55); tile_substitutions_add_double_index(gb, b_+55);
  CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+56, b_+57); H = mem_rd(gb, HL);
  CYC(b_+57, b_+58); L = A;
  CYC(b_+58, b_+59); E = L;
  CYC(b_+59, b_+60); D = H;
  CYC(b_+60, SYM(standardTileSubstitutions)); replaceTiles_hook(gb);
}
