#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void checkTilesetOverride_hook(GB *gb);

static void load_tileset_add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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
  pop_effect(gb);
}

static void load_tileset_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void load_tileset_data(GB *gb, uint16_t sp0_) {
  BASE(loadTilesetData_body);
  CYC(b_+26, b_+28); alu_and(gb, 0x80);
  CYC(b_+28, b_+30); H8(hFF8B) = A;
  CYC(b_+30, b_+32); A = H8(hFF8D);
  CYC(b_+32, b_+34); alu_and(gb, 0x7f);
  CALL_C(b_+34, multiplyABy8_hook, SYM(multiplyABy8), b_+37);
  CYC(b_+37, b_+40); SET_HL(SYM(tilesetData));
  CYC(b_+40, b_+41); alu_add_hl(gb, BC);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+42, b_+43); E = A;
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+44, b_+47); W8(wTilesetFlags) = A;
  CYC(b_+47, b_+49); alu_bit(gb, 3, A);
  if (F & FZ) {
    CYCT(b_+49, b_+51);
    CYC(b_+59, b_+61); A = 0xff;
    CYC(b_+61, b_+64); W8(wDungeonIndex) = A;
  } else {
    CYC(b_+49, b_+51);
    CYC(b_+51, b_+52); A = E;
    CYC(b_+52, b_+54); alu_and(gb, 0x0f);
    CYC(b_+54, b_+57); W8(wDungeonIndex) = A;
    CYC(b_+57, b_+59);
  }
  CYC(b_+64, b_+65); A = E;
  CYC(b_+65, b_+67); A = alu_swap(gb, A);
  CYC(b_+67, b_+69); alu_and(gb, 0x07);
  CYC(b_+69, b_+72); W8(wActiveCollisions) = A;
  CYC(b_+72, b_+74); B = 0x06;
  CYC(b_+74, b_+77); SET_DE(wTilesetUniqueGfx);
  do {
    CYC(b_+77, b_+78); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+78, b_+79); mem_wr(gb, DE, A);
    CYC(b_+79, b_+80); E = alu_inc8(gb, E);
    CYC(b_+80, b_+81); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+81, b_+83);
    else CYC(b_+81, b_+83);
  } while (!(F & FZ));
  CYC(b_+83, b_+85); E = 0x20;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); B = A;
  CYC(b_+87, b_+89); A = H8(hFF8B);
  CYC(b_+89, b_+90); alu_or(gb, B);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+92); ret_effect(gb);
}

void loadTilesetData_body_hook(GB *gb) {
  BASE(loadTilesetData_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getAdjustedRoomGroup_hook, SYM(getAdjustedRoomGroup), b_+3);
  CYC(b_+3, b_+6); SET_HL(SYM(roomTilesetsGroupTable));
  CYC(b_+6, b_+7); load_tileset_add_double_index_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); A = W8(wActiveRoom);
  CYC(b_+13, b_+14); load_tileset_add_a_to_hl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); H8(hFF8D) = A;
  CYC(b_+17, b_+20); push_effect(gb, b_+20);
  load_tileset_data(gb, sp0_);
  CALL_C(b_+20, checkTilesetOverride_hook, SYM(checkTilesetOverride), b_+23);
  if (!(F & FC)) {
    CYCT(b_+23, b_+24); ret_effect(gb);
    return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); A = H8(hFF8D);
  load_tileset_data(gb, sp0_);
}

void getAdjustedRoomGroup_hook(GB *gb) {
  BASE(getAdjustedRoomGroup);
  uint16_t sp0_ = gb->sp; (void)sp0_;

  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); alu_cp(gb, 0x02);
  if (!(F & FC)) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);

  CALL_C(b_+7, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+10);
  CYC(b_+10, b_+11); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+11, b_+13);
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+15); B |= 0x02;
  }
  CYC(b_+15, b_+16); A = B;
  CYC(b_+16, b_+17); ret_effect(gb);
}

// checkTilesetOverride@checkMakuTreeSaved: carry set when room $38 of group 0 must use the
// saved-Maku-Tree tileset (hFF8D bumped by two).
static void check_tileset_override_maku_tree_saved(GB *gb) {
  BASE(checkTilesetOverride__checkMakuTreeSaved);
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveGroup);
  alu_or(gb, A);
  CYC(b_+3, b_+4);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wActiveRoom);
  alu_cp(gb, 0x38);
  CYC(b_+8, b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto noChange; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = mem_rd(gb, wGroup1RoomFlags + 0x48);
  alu_and(gb, 0x01);
  CYC(b_+15, b_+17);
  if (F & FZ) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
  SET_HL(hFF8D);
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  alu_scf(gb);
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); ret_effect(gb);
  return;
noChange:
  alu_xor(gb, A);
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+27); ret_effect(gb);
}

// checkTilesetOverride@checkJabuFlooded: carry set when the current Jabu-Jabu floor is under
// water for the current water level (hFF8D bumped by one).
static void check_tileset_override_jabu_flooded(GB *gb) {
  BASE(checkTilesetOverride__checkJabuFlooded);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonIndex);
  alu_cp(gb, 0x07);
  CYC(b_+3, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto noChange; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wTilesetFlags);
  alu_and(gb, 0x20);
  CYC(b_+10, b_+12);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto noChange; }
  CYC(b_+12, b_+14);
  A = 0x11;
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); mem_wr(gb, wDungeonFirstLayout, A);
  SET_HL(SYM(findActiveRoomInDungeonLayoutWithPointlessBankSwitch));
  E = SYMBANK(findActiveRoomInDungeonLayoutWithPointlessBankSwitch);
  CYC(b_+19, b_+24);
  CALL_C(b_+24, interBankCall_hook, ROM_interBankCall, b_+27);
  CYC(b_+27, b_+30); A = mem_rd(gb, wJabuWaterLevel);
  alu_and(gb, 0x07);
  SET_HL(b_+56);
  CYC(b_+30, b_+35);
  CYC(b_+35, b_+36); load_tileset_add_a_to_hl_from_rst(gb, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, wDungeonFloor);
  SET_BC(bitTable);
  alu_add(gb, C);
  C = A;
  CYC(b_+39, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, BC);
  CYC(b_+45, b_+46); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+46, b_+47); ret_effect(gb); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+49); A = H8(hFF8D);
  A = alu_inc8(gb, A);
  CYC(b_+49, b_+50);
  CYC(b_+50, b_+52); H8(hFF8D) = A;
  alu_scf(gb);
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+54); ret_effect(gb);
  return;
noChange:
  alu_xor(gb, A);
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); ret_effect(gb);
}

void checkTilesetOverride_hook(GB *gb) {
  BASE(checkTilesetOverride);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  check_tileset_override_maku_tree_saved(gb);
  if (F & FC) {
    CYCT(b_+3, b_+4); ret_effect(gb);
    return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); push_effect(gb, b_+7);
  check_tileset_override_jabu_flooded(gb);
  if (F & FC) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = W8(wActiveGroup);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14);
    goto no_change;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); A = W8(wLoadingRoomPack);
  CYC(b_+17, b_+19); alu_cp(gb, 0x7f);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto no_change;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); A = W8(wAnimalCompanion);
  CYC(b_+24, b_+26); alu_sub(gb, 0x0b);
  if (F & FZ) {
    CYCT(b_+26, b_+28);
    goto no_change;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+31); A = H8(hFF8D);
  CYC(b_+31, b_+32); alu_add(gb, B);
  CYC(b_+32, b_+34); H8(hFF8D) = A;
  CYC(b_+34, b_+35); alu_scf(gb);
  CYC(b_+35, b_+36); ret_effect(gb);
  return;

no_change:
  CYC(b_+36, b_+37); alu_xor(gb, A);
  CYC(b_+37, b_+38); ret_effect(gb);
}

static void add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L); L = A;
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void setPastCliffPalettesToRed_hook(GB *gb) {
  BASE(setPastCliffPalettesToRed);
  CYC(b_+0, b_+3); A = W8(wActiveCollisions);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto done;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = W8(wTilesetFlags);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
    goto done;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+16); A = W8(wActiveRoom);
  CYC(b_+16, b_+18); alu_cp(gb, 0x38);
  if (F & FZ) {
    CYCT(b_+18, b_+19); ret_effect(gb);
    return;
  }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); A = 0x03;
  CYC(b_+21, b_+23); mem_wr(gb, 0xff70, A);
  CYC(b_+23, b_+26); SET_HL(w3TileMappingData + 0x204);
  CYC(b_+26, b_+28); D = 0x06;
  for (;;) {
    CYC(b_+28, b_+30); B = 0x04;
    for (;;) {
      CYC(b_+30, b_+31); A = mem_rd(gb, HL);
      CYC(b_+31, b_+33); alu_and(gb, 0x07);
      CYC(b_+33, b_+34); alu_cp(gb, D);
      if (!(F & FZ)) {
        CYCT(b_+34, b_+36);
      } else {
        CYC(b_+34, b_+36);
        CYC(b_+36, b_+37); A = mem_rd(gb, HL);
        CYC(b_+37, b_+39); alu_and(gb, 0xf8);
        CYC(b_+39, b_+40); mem_wr(gb, HL, A);
      }
      CYC(b_+40, b_+41); SET_HL(HL + 1);
      CYC(b_+41, b_+42); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(b_+42, b_+44);
        continue;
      }
      CYC(b_+42, b_+44);
      break;
    }
    CYC(b_+44, b_+46); A = 0x04;
    CYC(b_+46, b_+47); push_effect(gb, b_+47);
    add_a_to_hl(gb);
    CYC(b_+47, b_+48); A = H;
    CYC(b_+48, b_+50); alu_cp(gb, 0xd4);
    if (F & FC) {
      CYCT(b_+50, b_+52);
      continue;
    }
    CYC(b_+50, b_+52);
    break;
  }
done:
  CYC(b_+52, b_+53); alu_xor(gb, A);
  CYC(b_+53, b_+55); mem_wr(gb, 0xff70, A);
  CYC(b_+55, b_+56); ret_effect(gb);
}

void func_04_6e9b_hook(GB *gb) {
  BASE(func_04_6e9b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, 0xff70, A);
  CYC(b_+4, b_+7); SET_HL(wRoomLayout);
  CYC(b_+7, b_+10); SET_DE(w2TmpGfxBuffer);
  CYC(b_+10, b_+12); B = 0xc0;
  CALL_C(b_+12, copyMemory_hook, SYM(copyMemory), b_+15);
  CYC(b_+15, b_+18); SET_HL(wRoomCollisions);
  CYC(b_+18, b_+21); SET_DE(w2TmpGfxBuffer + 0x100);
  CYC(b_+21, b_+23); B = 0xc0;
  CALL_C(b_+23, copyMemory_hook, SYM(copyMemory), b_+26);
  CYC(b_+26, b_+29); SET_HL(w3RoomLayoutBuffer);
  CYC(b_+29, b_+32); SET_DE(w2TmpGfxBuffer + 0x200);
  CYC(b_+32, b_+34); B = 0xc0;
  for (;;) {
    CYC(b_+34, b_+36); A = 0x03;
    CYC(b_+36, b_+38); mem_wr(gb, 0xff70, A);
    CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+39, b_+40); C = A;
    CYC(b_+40, b_+42); A = 0x02;
    CYC(b_+42, b_+44); mem_wr(gb, 0xff70, A);
    CYC(b_+44, b_+45); A = C;
    CYC(b_+45, b_+46); mem_wr(gb, DE, A);
    CYC(b_+46, b_+47); SET_DE(DE + 1);
    CYC(b_+47, b_+48); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+48, b_+50);
      continue;
    }
    CYC(b_+48, b_+50);
    break;
  }
  CYC(b_+50, b_+51); alu_xor(gb, A);
  CYC(b_+51, b_+53); mem_wr(gb, 0xff70, A);
  CYC(b_+53, b_+54); ret_effect(gb);
}

void func_04_6ed1_hook(GB *gb) {
  BASE(func_04_6ed1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, 0xff70, A);
  CYC(b_+4, b_+7); SET_HL(wRoomLayout);
  CYC(b_+7, b_+10); SET_DE(w2TmpGfxBuffer);
  CYC(b_+10, b_+12); B = 0xc0;
  CALL_C(b_+12, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+15);
  CYC(b_+15, b_+18); SET_HL(wRoomCollisions);
  CYC(b_+18, b_+21); SET_DE(w2TmpGfxBuffer + 0x100);
  CYC(b_+21, b_+23); B = 0xc0;
  CALL_C(b_+23, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+26);
  CYC(b_+26, b_+29); SET_HL(w3RoomLayoutBuffer);
  CYC(b_+29, b_+32); SET_DE(w2TmpGfxBuffer + 0x200);
  CYC(b_+32, b_+34); B = 0xc0;
  for (;;) {
    CYC(b_+34, b_+36); A = 0x02;
    CYC(b_+36, b_+38); mem_wr(gb, 0xff70, A);
    CYC(b_+38, b_+39); A = mem_rd(gb, DE);
    CYC(b_+39, b_+40); SET_DE(DE + 1);
    CYC(b_+40, b_+41); C = A;
    CYC(b_+41, b_+43); A = 0x03;
    CYC(b_+43, b_+45); mem_wr(gb, 0xff70, A);
    CYC(b_+45, b_+46); A = C;
    CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+47, b_+48); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+48, b_+50);
      continue;
    }
    CYC(b_+48, b_+50);
    break;
  }
  CYC(b_+50, b_+51); alu_xor(gb, A);
  CYC(b_+51, b_+53); mem_wr(gb, 0xff70, A);
  CYC(b_+53, b_+54); ret_effect(gb);
}

static void func_04_6f07_locfunc(GB *gb) {
  BASE(func_04_6f07);
  for (;;) {
    CYC(b_+21, b_+23); A = 0x03;
    CYC(b_+23, b_+25); mem_wr(gb, 0xff70, A);
    CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+26, b_+28); H8(hFF8B) = A;
    CYC(b_+28, b_+30); A = 0x06;
    CYC(b_+30, b_+32); mem_wr(gb, 0xff70, A);
    CYC(b_+32, b_+34); A = H8(hFF8B);
    CYC(b_+34, b_+35); mem_wr(gb, DE, A);
    CYC(b_+35, b_+36); SET_DE(DE + 1);
    CYC(b_+36, b_+37); SET_BC(BC - 1);
    CYC(b_+37, b_+38); A = B;
    CYC(b_+38, b_+39); alu_or(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+39, b_+41);
      continue;
    }
    CYC(b_+39, b_+41);
    break;
  }
  CYC(b_+41, b_+42); ret_effect(gb);
}

void func_04_6f07_hook(GB *gb) {
  BASE(func_04_6f07);
  CYC(b_+0, b_+3); SET_HL(w3VramTiles);
  CYC(b_+3, b_+6); SET_DE(w6TileBuffer);
  CYC(b_+6, b_+9); SET_BC(0x0200);
  CYC(b_+9, b_+12); push_effect(gb, b_+12);
  func_04_6f07_locfunc(gb);
  CYC(b_+12, b_+15); SET_HL(w3VramAttributes);
  CYC(b_+15, b_+18); SET_DE(w6AttributeBuffer);
  CYC(b_+18, b_+21); SET_BC(0x0200);
  func_04_6f07_locfunc(gb);
}

static void func_04_6f31_locfunc(GB *gb) {
  BASE(func_04_6f31);
  for (;;) {
    CYC(b_+21, b_+23); A = 0x06;
    CYC(b_+23, b_+25); mem_wr(gb, 0xff70, A);
    CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+26, b_+28); H8(hFF8B) = A;
    CYC(b_+28, b_+30); A = 0x03;
    CYC(b_+30, b_+32); mem_wr(gb, 0xff70, A);
    CYC(b_+32, b_+34); A = H8(hFF8B);
    CYC(b_+34, b_+35); mem_wr(gb, DE, A);
    CYC(b_+35, b_+36); SET_DE(DE + 1);
    CYC(b_+36, b_+37); SET_BC(BC - 1);
    CYC(b_+37, b_+38); A = B;
    CYC(b_+38, b_+39); alu_or(gb, C);
    if (!(F & FZ)) {
      CYCT(b_+39, b_+41);
      continue;
    }
    CYC(b_+39, b_+41);
    break;
  }
  CYC(b_+41, b_+42); ret_effect(gb);
}

void func_04_6f31_hook(GB *gb) {
  BASE(func_04_6f31);
  CYC(b_+0, b_+3); SET_HL(w6TileBuffer);
  CYC(b_+3, b_+6); SET_DE(w3VramTiles);
  CYC(b_+6, b_+9); SET_BC(0x0200);
  CYC(b_+9, b_+12); push_effect(gb, b_+12);
  func_04_6f31_locfunc(gb);
  CYC(b_+12, b_+15); SET_HL(w6AttributeBuffer);
  CYC(b_+15, b_+18); SET_DE(w3VramAttributes);
  CYC(b_+18, b_+21); SET_BC(0x0200);
  func_04_6f31_locfunc(gb);
}
