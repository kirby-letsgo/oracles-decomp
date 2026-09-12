#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x04, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x04, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5fef, replacePollutionWithWaterIfPollutionFixed_hook, 0x6078, 0x5ff2);
  CALL_C(0x5ff2, applySingleTileChanges, 0x62de, 0x5ff5);
  CALL_C(0x5ff5, applyStandardTileSubstitutions_hook, 0x60b7, 0x5ff8);
  CALL_C(0x5ff8, replaceOpenedChest, 0x626f, 0x5ffb);
  CYC(0x5ffb, 0x5ffe); A = W8(wActiveGroup);
  CYC(0x5ffe, 0x6000); alu_and(gb, 0x06);
  CYC(0x6000, 0x6002); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x6002, 0x6004);
  } else {
    CYC(0x6002, 0x6004);
    CALL_C(0x6004, replaceShutterForLinkEntering, 0x61d8, 0x6007);
    CALL_C(0x6007, replaceSwitchTiles, 0x627e, 0x600a);
    CALL_C(0x600a, replaceToggleBlocks, 0x617c, 0x600d);
    CALL_C(0x600d, replaceJabuTilesIfUnderwater, 0x61a1, 0x6010);
  }
  CALL_C(0x6010, applyRoomSpecificTileChanges_hook, 0x642c, 0x6013);
  CYC(0x6013, 0x6016); A = W8(wActiveGroup);
  CYC(0x6016, 0x6018); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(0x6018, 0x6019); ret_effect(gb);
    return;
  }
  CYC(0x6018, 0x6019);
  CALL_C(0x6019, replaceBreakableTileOverPortal_hook, 0x6046, 0x601c);
  CALL_C(0x601c, replaceBreakableTileOverLinkTimeWarpingIn_hook, 0x606a, 0x601f);
  CYC(0x601f, 0x6022); A = W8(wLinkTimeWarpTile);
  CYC(0x6022, 0x6023); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6023, 0x6024); ret_effect(gb);
    return;
  }
  CYC(0x6023, 0x6024);
  CYC(0x6024, 0x6025); C = A;
  CYC(0x6025, 0x6026); C = alu_dec8(gb, C);
  CYC(0x6026, 0x6028); B = wRoomLayout >> 8;
  CYC(0x6028, 0x6029); A = mem_rd(gb, BC);
  CYC(0x6029, 0x602a); E = A;
  CYC(0x602a, 0x602d); SET_HL(0x6033);
  CALL_C(0x602d, lookupKey_hook, 0x1e06, 0x6030);
  if (!(F & FC)) {
    CYCT(0x6030, 0x6031); ret_effect(gb);
    return;
  }
  CYC(0x6030, 0x6031);
  CYC(0x6031, 0x6032); mem_wr(gb, BC, A);
  CYC(0x6032, 0x6033); ret_effect(gb);
}

void replaceBreakableTileOverPortal_hook(GB *gb) {
  CYC(0x6046, 0x6049); SET_HL(wPortalGroup);
  CYC(0x6049, 0x604c); A = W8(wActiveGroup);
  CYC(0x604c, 0x604d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x604d, 0x604e); ret_effect(gb);
    return;
  }
  CYC(0x604d, 0x604e);
  CYC(0x604e, 0x604f); L = alu_inc8(gb, L);
  CYC(0x604f, 0x6052); A = W8(wActiveRoom);
  CYC(0x6052, 0x6053); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x6053, 0x6054); ret_effect(gb);
    return;
  }
  CYC(0x6053, 0x6054);
  CYC(0x6054, 0x6055); L = alu_inc8(gb, L);
  CYC(0x6055, 0x6056); C = mem_rd(gb, HL);
  removeBreakableTileForTimeWarp_hook(gb);
}

void removeBreakableTileForTimeWarp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6056, 0x6058); B = wRoomLayout >> 8;
  CYC(0x6058, 0x6059); A = mem_rd(gb, BC);
  CYC(0x6059, 0x605a); E = A;
  CYC(0x605a, 0x605d); SET_HL(0x6063);
  CALL_C(0x605d, lookupKey_hook, 0x1e06, 0x6060);
  if (!(F & FC)) {
    CYCT(0x6060, 0x6061); ret_effect(gb);
    return;
  }
  CYC(0x6060, 0x6061);
  CYC(0x6061, 0x6062); mem_wr(gb, BC, A);
  CYC(0x6062, 0x6063); ret_effect(gb);
}

void replaceBreakableTileOverLinkTimeWarpingIn_hook(GB *gb) {
  CYC(0x606a, 0x606d); A = W8(wWarpTransition);
  CYC(0x606d, 0x606f); alu_and(gb, 0x0f);
  CYC(0x606f, 0x6071); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x6071, 0x6072); ret_effect(gb);
    return;
  }
  CYC(0x6071, 0x6072);
  CYC(0x6072, 0x6075); A = W8(wWarpDestPos);
  CYC(0x6075, 0x6076); C = A;
  CYC(0x6076, 0x6078); removeBreakableTileForTimeWarp_hook(gb);
}

void replacePollutionWithWaterIfPollutionFixed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6078, 0x607a); A = 0x30;
  CALL_C(0x607a, checkGlobalFlag_hook, 0x31f3, 0x607d);
  if (F & FZ) {
    CYCT(0x607d, 0x607e); ret_effect(gb);
    return;
  }
  CYC(0x607d, 0x607e);
  CYC(0x607e, 0x6081); A = W8(wTilesetFlags);
  CYC(0x6081, 0x6083); alu_bit(gb, 0, A);
  if (F & FZ) {
    CYCT(0x6083, 0x6084); ret_effect(gb);
    return;
  }
  CYC(0x6083, 0x6084);
  CYC(0x6084, 0x6087); SET_DE(0x6090);
  CYC(0x6087, 0x6089); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(0x6089, 0x608b);
  } else {
    CYC(0x6089, 0x608b);
    CYC(0x608b, 0x608e); SET_DE(0x6093);
  }
  CYC(0x608e, 0x6090); replaceTiles_hook(gb);
}

void replaceTiles_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x6096, 0x6097); A = mem_rd(gb, DE);
    CYC(0x6097, 0x6098); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x6098, 0x6099); ret_effect(gb);
      return;
    }
    CYC(0x6098, 0x6099);
    CYC(0x6099, 0x609a); B = A;
    CYC(0x609a, 0x609b); SET_DE(DE + 1);
    CYC(0x609b, 0x609c); A = mem_rd(gb, DE);
    CYC(0x609c, 0x609d); SET_DE(DE + 1);
    CALL_C(0x609d, findTileInRoom_hook, 0x15cc, 0x60a0);
    if (!(F & FZ)) {
      CYCT(0x60a0, 0x60a2);
      continue;
    }
    CYC(0x60a0, 0x60a2);
    CYC(0x60a2, 0x60a3); mem_wr(gb, HL, B);
    CYC(0x60a3, 0x60a4); C = A;
    CYC(0x60a4, 0x60a5); A = L;
    CYC(0x60a5, 0x60a6); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x60a6, 0x60a8);
      continue;
    }
    CYC(0x60a6, 0x60a8);
    for (;;) {
      CYC(0x60a8, 0x60a9); L = alu_dec8(gb, L);
      CYC(0x60a9, 0x60aa); A = C;
      CALL_C(0x60aa, backwardsSearch_hook, 0x15d0, 0x60ad);
      if (!(F & FZ)) {
        CYCT(0x60ad, 0x60af);
        break;
      }
      CYC(0x60ad, 0x60af);
      CYC(0x60af, 0x60b0); mem_wr(gb, HL, B);
      CYC(0x60b0, 0x60b1); C = A;
      CYC(0x60b1, 0x60b2); A = L;
      CYC(0x60b2, 0x60b3); alu_or(gb, A);
      if (F & FZ) {
        CYCT(0x60b3, 0x60b5);
        break;
      }
      CYC(0x60b3, 0x60b5);
      CYC(0x60b5, 0x60b7);
    }
  }
}

void applyStandardTileSubstitutions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x60b7, getThisRoomFlags_hook, 0x197d, 0x60ba);
  CYC(0x60ba, 0x60bc); hram_wr(gb, 0x8b, A);
  CYC(0x60bc, 0x60bf); SET_HL(0x60f5);
  CYC(0x60bf, 0x60c1); alu_bit(gb, 0, A);
  if (!(F & FZ)) CALL_C_CC(0x60c1, applyStandardTileSubstitutions__locFunc_hook, 0x60ea, 0x60c4);
  else CYC(0x60c1, 0x60c4);
  CYC(0x60c4, 0x60c7); SET_HL(0x6101);
  CYC(0x60c7, 0x60c9); A = hram_rd(gb, 0x8b);
  CYC(0x60c9, 0x60cb); alu_bit(gb, 1, A);
  if (!(F & FZ)) CALL_C_CC(0x60cb, applyStandardTileSubstitutions__locFunc_hook, 0x60ea, 0x60ce);
  else CYC(0x60cb, 0x60ce);
  CYC(0x60ce, 0x60d1); SET_HL(0x610d);
  CYC(0x60d1, 0x60d3); A = hram_rd(gb, 0x8b);
  CYC(0x60d3, 0x60d5); alu_bit(gb, 2, A);
  if (!(F & FZ)) CALL_C_CC(0x60d5, applyStandardTileSubstitutions__locFunc_hook, 0x60ea, 0x60d8);
  else CYC(0x60d5, 0x60d8);
  CYC(0x60d8, 0x60db); SET_HL(0x6119);
  CYC(0x60db, 0x60dd); A = hram_rd(gb, 0x8b);
  CYC(0x60dd, 0x60df); alu_bit(gb, 3, A);
  if (!(F & FZ)) CALL_C_CC(0x60df, applyStandardTileSubstitutions__locFunc_hook, 0x60ea, 0x60e2);
  else CYC(0x60df, 0x60e2);
  CYC(0x60e2, 0x60e5); SET_HL(0x6125);
  CYC(0x60e5, 0x60e7); A = hram_rd(gb, 0x8b);
  CYC(0x60e7, 0x60e9); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x60e9, 0x60ea); ret_effect(gb);
    return;
  }
  CYC(0x60e9, 0x60ea);
  applyStandardTileSubstitutions__locFunc_hook(gb);
}

void applyStandardTileSubstitutions__locFunc_hook(GB *gb) {
  CYC(0x60ea, 0x60ed); A = W8(wActiveCollisions);
  CYC(0x60ed, 0x60ee); tile_substitutions_add_double_index(gb, 0x60ee);
  CYC(0x60ee, 0x60ef); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x60ef, 0x60f0); H = mem_rd(gb, HL);
  CYC(0x60f0, 0x60f1); L = A;
  CYC(0x60f1, 0x60f2); E = L;
  CYC(0x60f2, 0x60f3); D = H;
  CYC(0x60f3, 0x60f5); replaceTiles_hook(gb);
}
