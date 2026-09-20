#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(initGbaModePaletteData), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(initGbaModePaletteData), (from), (to), true)

void getAddressOfLoadedObjectGfxIndex_hook(GB *gb);
void incLoadedObjectGfxIndex_hook(GB *gb);
void getObjectGfxIndexForEnemy_hook(GB *gb);
void insertIndexIntoLoadedObjectGfx_hook(GB *gb);
void interactionGetData_hook(GB *gb);
void getDataForInteraction_hook(GB *gb);
static void refresh_dirty_palettes_refresh(GB *gb);
static void refresh_dirty_palettes_gba_mode(GB *gb);
static void refresh_dirty_palettes_gbc_mode(GB *gb);
static void refresh_dirty_palettes_next_palette(GB *gb);
static void refresh_dirty_palettes_gba_brighten_palette(GB *gb);
static void add_a_to_hl_from_rst(GB *gb, uint16_t return_address);
static void refresh_object_gfx_next_extra(GB *gb, uint16_t sp0_);
void refreshObjectGfx_body__afterCall41d2_hook(GB *gb);

void initGbaModePaletteData_hook(GB *gb) {
  BASE(initGbaModePaletteData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_SVBK);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+5); A = 0x02;
  CYC(b_+5, b_+7); mem_wr(gb, IO_SVBK, A);
  CYC(b_+7, b_+10); SET_HL(SYM(gbaModePaletteData));
  CYC(b_+10, b_+13); SET_DE(w2GbaModePaletteData);
  CYC(b_+13, b_+15); B = 0x80;
  CALL_C(b_+15, copyMemory_hook, SYM(copyMemory), b_+18);
  CYC(b_+18, b_+19); SET_AF(pop_effect(gb));
  CYC(b_+19, b_+21); mem_wr(gb, IO_SVBK, A);
  CYC(b_+21, SYM(refreshDirtyPalettes)); ret_effect(gb);
}

void refreshDirtyPalettes_hook(GB *gb) {
  BASE(refreshDirtyPalettes);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+6); A = H8(hDirtyBgPalettes);
  CYC(b_+6, b_+7); D = A;
  CYC(b_+7, b_+9); A = H8(hBgPaletteSources);
  CYC(b_+9, b_+10); E = A;
  CYC(b_+10, b_+12); L = (uint8_t)w2TilesetBgPalettes;
  CYC(b_+12, b_+15); push_effect(gb, b_+15);
  refresh_dirty_palettes_refresh(gb);
  CYC(b_+15, b_+17); A = H8(hDirtySprPalettes);
  CYC(b_+17, b_+18); D = A;
  CYC(b_+18, b_+20); A = H8(hSprPaletteSources);
  CYC(b_+20, b_+21); E = A;
  CYC(b_+21, b_+23); L = (uint8_t)w2TilesetSprPalettes;
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_refresh(GB *gb) {
  BASE(refreshDirtyPalettes);
  CYC(b_+23, b_+24); A = D;
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+25, b_+26);
    ret_effect(gb);
    return;
  }
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); D = alu_srl(gb, D);
  if (!(F & FC)) {
    CYCT(b_+28, b_+30);
    refresh_dirty_palettes_next_palette(gb);
    return;
  }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); H = (uint8_t)(w2TilesetBgPalettes >> 8);
  CYC(b_+32, b_+34); E = alu_srl(gb, E);
  if (!(F & FC)) {
    CYCT(b_+34, b_+36);
  } else {
    CYC(b_+34, b_+36);
    CYC(b_+36, b_+37); H = alu_inc8(gb, H);
  }
  CYC(b_+37, b_+39); A = H8(hGameboyType);
  CYC(b_+39, b_+40); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+40, b_+42);
    refresh_dirty_palettes_gbc_mode(gb);
    return;
  }
  CYC(b_+40, b_+42);
  refresh_dirty_palettes_gba_mode(gb);
}

static void refresh_dirty_palettes_gba_mode(GB *gb) {
  BASE(refreshDirtyPalettes);
  CYC(b_+42, b_+45); push_effect(gb, b_+45);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(b_+45, b_+48); push_effect(gb, b_+48);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(b_+48, b_+51); push_effect(gb, b_+51);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(b_+51, b_+54); push_effect(gb, b_+54);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(b_+54, b_+56);
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_gbc_mode(GB *gb) {
  BASE(refreshDirtyPalettes);
  CYC(b_+56, b_+57); push_effect(gb, DE);
  CYC(b_+57, b_+59); B = (uint8_t)(w2BgPalettesBuffer >> 8);
  CYC(b_+59, b_+60); C = L;
  CYC(b_+60, b_+62); C = (uint8_t)(C & ~(1 << 7));
  CYC(b_+62, b_+64); E = 0x08;
  for (;;) {
    CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+65, b_+66); mem_wr(gb, BC, A);
    CYC(b_+66, b_+67); C = alu_inc8(gb, C);
    CYC(b_+67, b_+68); E = alu_dec8(gb, E);
    if (F & FZ) {
      CYC(b_+68, b_+70);
      break;
    }
    CYCT(b_+68, b_+70);
  }
  CYC(b_+70, b_+71); SET_DE(pop_effect(gb));
  CYC(b_+71, b_+73);
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_next_palette(GB *gb) {
  BASE(refreshDirtyPalettes);
  CYC(b_+73, b_+74); A = L;
  CYC(b_+74, b_+76); alu_add(gb, 0x08);
  CYC(b_+76, b_+77); L = A;
  CYC(b_+77, b_+79); E = alu_srl(gb, E);
  CYC(b_+79, b_+81);
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_gba_brighten_palette(GB *gb) {
  BASE(refreshDirtyPalettes);
  CYC(b_+81, b_+82); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+82, b_+83); C = A;
  CYC(b_+83, b_+85); alu_and(gb, 0xe0);
  CYC(b_+85, b_+86); B = A;
  CYC(b_+86, b_+87); A = mem_rd(gb, HL);
  CYC(b_+87, b_+89); alu_and(gb, 0x03);
  CYC(b_+89, b_+90); alu_or(gb, B);
  CYC(b_+90, b_+92); A = alu_swap(gb, A);
  CYC(b_+92, b_+93); B = A;
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+94, b_+96); alu_and(gb, 0x7c);
  CYC(b_+96, b_+97); alu_rrca(gb);
  CYC(b_+97, b_+98); alu_rrca(gb);
  CYC(b_+98, b_+99); push_effect(gb, HL);
  CYC(b_+99, b_+102); SET_HL(w2GbaModePaletteData + 0x60);
  CYC(b_+102, b_+103); add_a_to_hl_from_rst(gb, b_+103);
  CYC(b_+103, b_+104); A = B;
  CYC(b_+104, b_+105); B = mem_rd(gb, HL);
  CYC(b_+105, b_+108); SET_HL(w2GbaModePaletteData + 0x21);
  CYC(b_+108, b_+109); add_a_to_hl_from_rst(gb, b_+109);
  CYC(b_+109, b_+110); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+110, b_+111); alu_or(gb, B);
  CYC(b_+111, b_+112); B = A;
  CYC(b_+112, b_+113); A = C;
  CYC(b_+113, b_+115); alu_and(gb, 0x1f);
  CYC(b_+115, b_+116); C = mem_rd(gb, HL);
  CYC(b_+116, b_+119); SET_HL(w2GbaModePaletteData);
  CYC(b_+119, b_+120); add_a_to_hl_from_rst(gb, b_+120);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL);
  CYC(b_+121, b_+122); alu_or(gb, C);
  CYC(b_+122, b_+123); SET_HL(pop_effect(gb));
  CYC(b_+123, b_+124); C = H;
  CYC(b_+124, b_+126); L = (uint8_t)(L & ~(1 << 7));
  CYC(b_+126, b_+128); H = (uint8_t)(w2BgPalettesBuffer >> 8);
  CYC(b_+128, b_+129); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+129, b_+130); A = B;
  CYC(b_+130, b_+131); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+131, b_+133); L = (uint8_t)(L | (1 << 7));
  CYC(b_+133, b_+134); H = C;
  CYC(b_+134, SYM(gbaModePaletteData)); ret_effect(gb);
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

void findIndexInLoadedObjectGfx_hook(GB *gb) {
  BASE(findIndexInLoadedObjectGfx);
  CYC(b_+0, b_+1); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+1, b_+2);
    ret_effect(gb);
    return;
  }
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+5); SET_HL(wLoadedObjectGfx);
  CYC(b_+5, b_+7); B = 0x08;
  CYC(b_+7, b_+8); C = A;

  for (;;) {
    CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+9, b_+10); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+10, b_+12);
      break;
    }
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+13); L = alu_inc8(gb, L);
    CYC(b_+13, b_+14); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(b_+14, b_+16);
      CYC(b_+16, b_+18); C = 0x01;
      CYC(b_+18, b_+19); alu_scf(gb);
      CYC(b_+19, b_+20); ret_effect(gb);
      return;
    }
    CYCT(b_+14, b_+16);
  }

  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x01);
  CYC(b_+22, b_+23); L = alu_dec8(gb, L);
  CYC(b_+23, b_+24); A = L;
  CYC(b_+24, b_+26); alu_sub(gb, (uint8_t)wLoadedObjectGfx);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+29); C = A;
  CYC(b_+29, SYM(findUnusedIndexInLoadedObjectGfx)); ret_effect(gb);
}

void updateTileIndexBaseForAllObjects__updateTileIndexBase_hook(GB *gb) {
  BASE(updateTileIndexBaseForAllObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+86, b_+87);
    ret_effect(gb);
    return;
  }
  CYC(b_+86, b_+87);
  CALL_C(b_+87, findIndexInLoadedObjectGfx_hook, SYM(findIndexInLoadedObjectGfx), b_+90);
  CYC(b_+90, b_+92); A = H8(hActiveObjectType);
  CYC(b_+92, b_+93); E = A;
  CYC(b_+93, b_+94); A = mem_rd(gb, DE);
  CYC(b_+94, b_+95); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+95, b_+96);
    ret_effect(gb);
    return;
  }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+97); A = E;
  CYC(b_+97, b_+99); alu_add(gb, 0x1c);
  CYC(b_+99, b_+100); E = A;
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+103); alu_bit(gb, 3, A);
  if (!(F & FZ)) {
    CYCT(b_+103, b_+104);
    ret_effect(gb);
    return;
  }
  CYC(b_+103, b_+104);
  CYC(b_+104, b_+105); E = alu_inc8(gb, E);
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+108); alu_and(gb, 0x1f);
  CYC(b_+108, b_+109); alu_add(gb, C);
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CYC(b_+110, SYM(findIndexInLoadedObjectGfx)); ret_effect(gb);
}

void findUnusedIndexInLoadedObjectGfx_hook(GB *gb) {
  BASE(findUnusedIndexInLoadedObjectGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x08;

  for (;;) {
    CALL_C(b_+2, getAddressOfLoadedObjectGfxIndex_hook, SYM(getAddressOfLoadedObjectGfxIndex), b_+5);
    CYC(b_+5, b_+6); L = alu_inc8(gb, L);
    CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+7, b_+8); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+8, b_+10);
      break;
    }
    CYC(b_+8, b_+10);
    CALL_C(b_+10, incLoadedObjectGfxIndex_hook, SYM(incLoadedObjectGfxIndex), b_+13);
    CYC(b_+13, b_+14); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(b_+14, b_+16);
      CYC(b_+16, b_+18); C = 0x01;
      CYC(b_+18, b_+19); alu_scf(gb);
      CYC(b_+19, b_+20); ret_effect(gb);
      return;
    }
    CYCT(b_+14, b_+16);
  }

  CYC(b_+20, b_+21); A = L;
  CYC(b_+21, b_+23); alu_sub(gb, (uint8_t)wLoadedObjectGfx);
  CYC(b_+23, b_+25); A = alu_swap(gb, A);
  CYC(b_+25, b_+26); C = A;
  CYC(b_+26, SYM(incLoadedObjectGfxIndex)); ret_effect(gb);
}

void incLoadedObjectGfxIndex_hook(GB *gb) {
  BASE(incLoadedObjectGfxIndex);
  CYC(b_+0, b_+3); A = W8(wLoadedObjectGfxIndex);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x07);
  CYC(b_+6, b_+9); W8(wLoadedObjectGfxIndex) = A;
  CYC(b_+9, SYM(getAddressOfLoadedObjectGfxIndex)); ret_effect(gb);
}

void getAddressOfLoadedObjectGfxIndex_hook(GB *gb) {
  BASE(getAddressOfLoadedObjectGfxIndex);
  CYC(b_+0, b_+3); A = W8(wLoadedObjectGfxIndex);
  CYC(b_+3, b_+6); SET_HL(wLoadedObjectGfx);
  CYC(b_+6, b_+7); add_double_index_to_hl_from_rst(gb, b_+7);
  CYC(b_+7, SYM(addIndexToLoadedObjectGfx)); ret_effect(gb);
}

void addIndexToLoadedObjectGfx_hook(GB *gb) {
  BASE(addIndexToLoadedObjectGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+1, b_+2);
    ret_effect(gb);
    return;
  }
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+4); push_effect(gb, BC);
  CYC(b_+4, b_+5); E = A;
  CALL_C(b_+5, findIndexInLoadedObjectGfx_hook, SYM(findIndexInLoadedObjectGfx), b_+8);
  if (!(F & FC)) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CALL_C(b_+10, findUnusedIndexInLoadedObjectGfx_hook, SYM(findUnusedIndexInLoadedObjectGfx), b_+13);
    if (!(F & FC)) {
      CALL_C_CC(b_+13, insertIndexIntoLoadedObjectGfx_hook, SYM(insertIndexIntoLoadedObjectGfx), b_+16);
    } else {
      CYC(b_+13, b_+16);
    }
  }
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+18); SET_BC(pop_effect(gb));
  CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
  CYC(b_+19, SYM(insertIndexIntoLoadedObjectGfx)); ret_effect(gb);
}

static void insert_index_into_loaded_object_gfx_tail(GB *gb, uint16_t sp0_) {
  BASE(insertIndexIntoLoadedObjectGfx);
  CYC(b_+51, b_+53); D = 0x00;
  CYC(b_+53, b_+54); alu_add_hl(gb, DE);
  CYC(b_+54, b_+55); alu_add_hl(gb, DE);
  CYC(b_+55, b_+56); alu_add_hl(gb, DE);
  CALL_C(b_+56, loadObjectGfx_hook, SYM(loadObjectGfx), b_+59);
  CYC(b_+59, b_+60); SET_HL(pop_effect(gb));
  CYC(b_+60, b_+61); SET_DE(pop_effect(gb));
  CYC(b_+61, b_+62); SET_BC(pop_effect(gb));
  CYC(b_+62, SYM(markLoadedObjectGfxUsed)); ret_effect(gb);
}

static void insert_index_into_loaded_object_gfx_tree(GB *gb, uint16_t sp0_) {
  BASE(insertIndexIntoLoadedObjectGfx);
  CYC(b_+36, b_+38); B = 0x92;
  CYC(b_+38, b_+41); SET_HL(SYM(treeGfxHeaderTable));
  CYC(b_+41, b_+43);
  insert_index_into_loaded_object_gfx_tail(gb, sp0_);
}

static void insert_index_into_loaded_object_gfx_object(GB *gb, uint16_t sp0_) {
  BASE(insertIndexIntoLoadedObjectGfx);
  CYC(b_+43, b_+45); alu_sub(gb, 0x08);
  CYC(b_+45, b_+47); alu_or(gb, 0x80);
  CYC(b_+47, b_+48); B = A;
  CYC(b_+48, b_+51); SET_HL(SYM(objectGfxHeaderTable));
  insert_index_into_loaded_object_gfx_tail(gb, sp0_);
}

void insertIndexIntoLoadedObjectGfx_hook(GB *gb) {
  BASE(insertIndexIntoLoadedObjectGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+3); alu_cp(gb, (uint8_t)wLoadedTreeGfxActive);
  if (F & FC) {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+6); push_effect(gb, HL);
    CYC(b_+6, b_+9); SET_HL(wLoadedObjectGfx);
    for (;;) {
      CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+10, b_+11); alu_cp(gb, E);
      if (F & FZ) {
        CYC(b_+11, b_+13);
        CYC(b_+13, b_+14); alu_xor(gb, A);
        CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL - 1);
        CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
      } else {
        CYCT(b_+11, b_+13);
      }
      CYC(b_+16, b_+17); L = alu_inc8(gb, L);
      CYC(b_+17, b_+18); A = L;
      CYC(b_+18, b_+20); alu_cp(gb, (uint8_t)wLoadedObjectGfxEnd);
      if (!(F & FC)) {
        CYC(b_+20, b_+22);
        break;
      }
      CYCT(b_+20, b_+22);
    }
    CYC(b_+22, b_+23); SET_HL(pop_effect(gb));
  } else {
    CYCT(b_+3, b_+5);
  }
  CYC(b_+23, b_+24); push_effect(gb, BC);
  CYC(b_+24, b_+25); push_effect(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, HL);
  CYC(b_+26, b_+27); mem_wr(gb, HL, E);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x01);
  CYC(b_+30, b_+31); L = alu_dec8(gb, L);
  CYC(b_+31, b_+32); A = L;
  CYC(b_+32, b_+34); alu_cp(gb, (uint8_t)wLoadedTreeGfxActive);
  if (F & FC) {
    CYCT(b_+34, b_+36);
    insert_index_into_loaded_object_gfx_object(gb, sp0_);
  } else {
    CYC(b_+34, b_+36);
    insert_index_into_loaded_object_gfx_tree(gb, sp0_);
  }
}

void insertIndexIntoLoadedObjectGfx__tree_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  insert_index_into_loaded_object_gfx_tree(gb, sp0_);
}

void insertIndexIntoLoadedObjectGfx__object_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  insert_index_into_loaded_object_gfx_object(gb, sp0_);
}

static void mark_loaded_object_gfx_used_end(GB *gb) {
  BASE(markLoadedObjectGfxUsed);
  CYC(b_+22, b_+23); SET_HL(pop_effect(gb));
  CYC(b_+23, b_+24); SET_BC(pop_effect(gb));
  CYC(b_+24, SYM(markAllLoadedObjectGfxUnused)); ret_effect(gb);
}

static void mark_loaded_object_gfx_used_found(GB *gb) {
  BASE(markLoadedObjectGfxUsed);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x01);
  mark_loaded_object_gfx_used_end(gb);
}

void markLoadedObjectGfxUsed_hook(GB *gb) {
  BASE(markLoadedObjectGfxUsed);
  CYC(b_+0, b_+1); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+1, b_+2);
    ret_effect(gb);
    return;
  }
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); push_effect(gb, BC);
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CYC(b_+4, b_+7); SET_HL(wLoadedObjectGfx);
  CYC(b_+7, b_+8); C = A;

  for (;;) {
    CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+9, b_+10); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+10, b_+12);
      mark_loaded_object_gfx_used_found(gb);
      return;
    }
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+13); L = alu_inc8(gb, L);
    CYC(b_+13, b_+14); A = L;
    CYC(b_+14, b_+16); alu_cp(gb, (uint8_t)wLoadedObjectGfxEnd);
    if (!(F & FC)) {
      CYC(b_+16, b_+18);
      break;
    }
    CYCT(b_+16, b_+18);
  }

  CYC(b_+18, b_+20);
  mark_loaded_object_gfx_used_end(gb);
}

void markLoadedObjectGfxUsed__found_hook(GB *gb) {
  mark_loaded_object_gfx_used_found(gb);
}

void markLoadedObjectGfxUsed__end_hook(GB *gb) {
  mark_loaded_object_gfx_used_end(gb);
}

void markAllLoadedObjectGfxUnused_hook(GB *gb) {
  BASE(markAllLoadedObjectGfxUnused);
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+5); SET_HL(wLoadedObjectGfx);
  CYC(b_+5, b_+7); B = 0x08;
  CYC(b_+7, b_+8); alu_xor(gb, A);
  for (;;) {
    CYC(b_+8, b_+9); L = alu_inc8(gb, L);
    CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+10, b_+11); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(b_+11, b_+13);
      break;
    }
    CYCT(b_+11, b_+13);
  }
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+15); SET_BC(pop_effect(gb));
  CYC(b_+15, SYM(enemyGetObjectGfxIndex)); ret_effect(gb);
}

void enemyGetObjectGfxIndex_hook(GB *gb) {
  BASE(enemyGetObjectGfxIndex);
  CYC(b_+0, b_+2); E = 0x81;
  CYC(b_+2, SYM(getObjectGfxIndexForEnemy)); A = mem_rd(gb, DE);
  getObjectGfxIndexForEnemy_hook(gb);
}

void getObjectGfxIndexForEnemy_hook(GB *gb) {
  BASE(getObjectGfxIndexForEnemy);
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); alu_add(gb, A);
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+5); B = 0x00;
  CYC(b_+5, b_+8); SET_HL(SYM(enemyData));
  CYC(b_+8, b_+9); alu_add_hl(gb, BC);
  CYC(b_+9, b_+10); alu_add_hl(gb, BC);
  CYC(b_+10, b_+11); SET_BC(pop_effect(gb));
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, SYM(partGetObjectGfxIndex)); ret_effect(gb);
}

void partGetObjectGfxIndex_hook(GB *gb) {
  BASE(partGetObjectGfxIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+3); E = 0xc1;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CALL_C(b_+4, multiplyABy8_hook, SYM(multiplyABy8), b_+7);
  CYC(b_+7, b_+10); SET_HL(SYM(partData));
  CYC(b_+10, b_+11); alu_add_hl(gb, BC);
  CYC(b_+11, b_+12); SET_BC(pop_effect(gb));
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, SYM(interactionGetObjectGfxIndex)); ret_effect(gb);
}

void interactionGetObjectGfxIndex_hook(GB *gb) {
  BASE(interactionGetObjectGfxIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CALL_C(b_+1, interactionGetData_hook, SYM(interactionGetData), b_+4);
  CYC(b_+4, b_+5); SET_BC(pop_effect(gb));
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, SYM(itemGetObjectGfxIndex)); ret_effect(gb);
}

void itemGetObjectGfxIndex_hook(GB *gb) {
  BASE(itemGetObjectGfxIndex);
  CYC(b_+0, b_+2); E = 0x01;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); L = A;
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); alu_add(gb, L);
  CYC(b_+6, b_+9); SET_HL(SYM(itemData));
  CYC(b_+9, b_+10); add_a_to_hl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, SYM(enemyLoadGraphicsAndProperties)); ret_effect(gb);
}

void resumeThreadNextFrameIfLcdIsOn__afterCall4124_hook(GB *gb) {
  BASE(resumeThreadNextFrameIfLcdIsOn);
  CYC(b_+7, SYM(reloadObjectGfx_b3f)); ret_effect(gb);
}

void resumeThreadNextFrameIfLcdIsOn_hook(GB *gb) {
  BASE(resumeThreadNextFrameIfLcdIsOn);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = mem_rd(gb, IO_LCDC);
  CYC(b_+2, b_+3); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, resumeThreadNextFrameAndSaveBank_hook, SYM(resumeThreadNextFrameAndSaveBank), b_+7);
  resumeThreadNextFrameIfLcdIsOn__afterCall4124_hook(gb);
}

void reloadObjectGfx_b3f_hook(GB *gb) {
  BASE(reloadObjectGfx_b3f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wLoadedItemGraphic1);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(b_+4, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+7);
  } else {
    CYC(b_+4, b_+7);
  }
  CYC(b_+7, b_+10); A = W8(wLoadedItemGraphic2);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(b_+11, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), SYM(agesFunc_3f_4133));
  } else {
    CYC(b_+11, SYM(agesFunc_3f_4133));
  }
  agesFunc_3f_4133_hook(gb);
}

void agesFunc_3f_4133__afterCall4143_hook(GB *gb) {
  BASE(agesFunc_3f_4133);
  uint16_t sp0_ = gb->sp;
  for (;;) {
    CYC(b_+16, b_+17); L = alu_inc8(gb, L);
    CYC(b_+17, b_+18); mem_wr(gb, HL, D);
    CYC(b_+18, b_+19); L = alu_inc8(gb, L);
    CYC(b_+19, b_+20); A = L;
    CYC(b_+20, b_+22); alu_cp(gb, 0x18);
    if (!(F & FC)) {
      CYC(b_+22, b_+24);
      break;
    }
    CYCT(b_+22, b_+24);
    CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+4, b_+5); E = A;
    CYC(b_+5, b_+6); D = mem_rd(gb, HL);
    CYC(b_+6, b_+7); L = alu_dec8(gb, L);
    CYC(b_+7, b_+8); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+8, b_+10);
      continue;
    }
    CYC(b_+8, b_+10);
    CALL_C(b_+10, insertIndexIntoLoadedObjectGfx_hook, SYM(insertIndexIntoLoadedObjectGfx), b_+13);
    CALL_C(b_+13, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+16);
  }
  CYC(b_+24, b_+27); SET_HL(wLoadedTreeGfxIndex + 1);
  CYC(b_+27, b_+28); E = mem_rd(gb, HL);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x00);
  CYC(b_+30, SYM(refreshObjectGfx_body)); loadTreeGfx_body_hook(gb);
}

void agesFunc_3f_4133_hook(GB *gb) {
  BASE(agesFunc_3f_4133);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wLoadedObjectGfx);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); E = A;
  CYC(b_+5, b_+6); D = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = alu_dec8(gb, L);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CALL_C(b_+10, insertIndexIntoLoadedObjectGfx_hook, SYM(insertIndexIntoLoadedObjectGfx), b_+13);
    CALL_C(b_+13, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+16);
  }
  agesFunc_3f_4133__afterCall4143_hook(gb);
}

static void refresh_object_gfx_next_extra(GB *gb, uint16_t sp0_) {
  BASE(refreshObjectGfx_body);
  CYC(b_+98, b_+99); A = alu_inc8(gb, A);
  CYC(b_+99, b_+101); alu_and(gb, 0x07);
  CYC(b_+101, b_+102); B = A;
  CYC(b_+102, b_+105); SET_HL(wLoadedObjectGfx + 1);
  CYC(b_+105, b_+106); add_double_index_to_hl_from_rst(gb, b_+106);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+107, b_+108); D = A;
  CYC(b_+108, b_+109); C = mem_rd(gb, HL);
  CYC(b_+109, b_+110); E = alu_inc8(gb, E);
  CALL_C(b_+110, insertIndexIntoLoadedObjectGfx_hook, SYM(insertIndexIntoLoadedObjectGfx), b_+113);
  CYC(b_+113, b_+114); A = D;
  CYC(b_+114, b_+115); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+115, b_+117);
  } else {
    CYC(b_+115, b_+117);
    CYC(b_+117, b_+118); A = C;
    CYC(b_+118, b_+119); push_effect(gb, DE);
    CALL_C(b_+119, addIndexToLoadedObjectGfx_hook, SYM(addIndexToLoadedObjectGfx), b_+122);
    CYC(b_+122, b_+123); SET_DE(pop_effect(gb));
  }
  CALL_C(b_+123, updateTileIndexBaseForAllObjects_hook, SYM(updateTileIndexBaseForAllObjects), b_+126);
  refreshObjectGfx_body__afterCall41d2_hook(gb);
}

void refreshObjectGfx_body__afterCall41d2_hook(GB *gb) {
  BASE(refreshObjectGfx_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+126, b_+128); D = 0x00;
  CYC(b_+128, b_+131); SET_HL((SYM(objectGfxHeaderTable) + 1));
  CYC(b_+131, b_+132); alu_add_hl(gb, DE);
  CYC(b_+132, b_+133); alu_add_hl(gb, DE);
  CYC(b_+133, b_+134); alu_add_hl(gb, DE);
  CYC(b_+134, b_+136); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+136, b_+137); A = B;
  if (F & FZ) {
    CYCT(b_+137, b_+139);
    refresh_object_gfx_next_extra(gb, sp0_);
    return;
  }
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+142); W8(wLoadedObjectGfxIndex) = A;
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+146); W8(wEnemyIDToLoadExtraGfx) = A;
  CYC(b_+146, b_+149); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(b_+149, SYM(loadObjectGfxHeaderToSlot4_body)); incLoadedObjectGfxIndex_hook(gb);
}

void refreshObjectGfx_body__afterCall41ad_hook(GB *gb) {
  BASE(refreshObjectGfx_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+89, b_+90); A = E;
  CALL_C(b_+90, findIndexInLoadedObjectGfx_hook, SYM(findIndexInLoadedObjectGfx), b_+93);
  CYC(b_+93, b_+94); A = L;
  CYC(b_+94, b_+96); alu_sub(gb, 0x08);
  CYC(b_+96, b_+98); A = alu_srl(gb, A);
  refresh_object_gfx_next_extra(gb, sp0_);
}

void refreshObjectGfx_body_hook(GB *gb) {
  BASE(refreshObjectGfx_body);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, markAllLoadedObjectGfxUnused_hook, SYM(markAllLoadedObjectGfxUnused), b_+3);
  CYC(b_+3, b_+5); D = 0xd0;
  do {
    CALL_C(b_+5, enemyGetObjectGfxIndex_hook, SYM(enemyGetObjectGfxIndex), b_+8);
    CALL_C(b_+8, markLoadedObjectGfxUsed_hook, SYM(markLoadedObjectGfxUsed), b_+11);
    CYC(b_+11, b_+12); D = alu_inc8(gb, D);
    CYC(b_+12, b_+13); A = D;
    CYC(b_+13, b_+15); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+15, b_+17); else CYC(b_+15, b_+17);
  } while (F & FC);
  CYC(b_+17, b_+19); D = 0xd0;
  do {
    CALL_C(b_+19, partGetObjectGfxIndex_hook, SYM(partGetObjectGfxIndex), b_+22);
    CALL_C(b_+22, markLoadedObjectGfxUsed_hook, SYM(markLoadedObjectGfxUsed), b_+25);
    CYC(b_+25, b_+26); D = alu_inc8(gb, D);
    CYC(b_+26, b_+27); A = D;
    CYC(b_+27, b_+29); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+29, b_+31); else CYC(b_+29, b_+31);
  } while (F & FC);
  CYC(b_+31, b_+33); D = 0xd0;
  do {
    CALL_C(b_+33, interactionGetObjectGfxIndex_hook, SYM(interactionGetObjectGfxIndex), b_+36);
    CALL_C(b_+36, markLoadedObjectGfxUsed_hook, SYM(markLoadedObjectGfxUsed), b_+39);
    CYC(b_+39, b_+40); D = alu_inc8(gb, D);
    CYC(b_+40, b_+41); A = D;
    CYC(b_+41, b_+43); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+43, b_+45); else CYC(b_+43, b_+45);
  } while (F & FC);
  CYC(b_+45, b_+47); D = 0xd6;
  do {
    CALL_C(b_+47, itemGetObjectGfxIndex_hook, SYM(itemGetObjectGfxIndex), b_+50);
    CALL_C(b_+50, markLoadedObjectGfxUsed_hook, SYM(markLoadedObjectGfxUsed), b_+53);
    CYC(b_+53, b_+54); D = alu_inc8(gb, D);
    CYC(b_+54, b_+55); A = D;
    CYC(b_+55, b_+57); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+57, b_+59); else CYC(b_+57, b_+59);
  } while (F & FC);
  CYC(b_+59, b_+62); A = W8(wEnemyIDToLoadExtraGfx);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(b_+63, b_+65);
    CALL_C(b_+65, getObjectGfxIndexForEnemy_hook, SYM(getObjectGfxIndexForEnemy), b_+68);
    CYC(b_+68, b_+70);
  } else {
    CYCT(b_+63, b_+65);
    CYC(b_+70, b_+73); SET_HL(wInteractionIDToLoadExtraGfx);
    CYC(b_+73, b_+74); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+74, b_+75); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+75, b_+76); ret_effect(gb); return;
    }
    CYC(b_+75, b_+76);
    CYC(b_+76, b_+77); E = mem_rd(gb, HL);
    CYC(b_+77, b_+79); mem_wr(gb, HL, 0x00);
    CALL_C(b_+79, getDataForInteraction_hook, SYM(getDataForInteraction), b_+82);
    CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  }
  CALL_C(b_+83, addIndexToLoadedObjectGfx_hook, SYM(addIndexToLoadedObjectGfx), b_+86);
  CALL_C(b_+86, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+89);
  refreshObjectGfx_body__afterCall41ad_hook(gb);
}

void loadObjectGfxHeaderToSlot4_body__afterCall41f0_hook(GB *gb) {
  BASE(loadObjectGfxHeaderToSlot4_body);
  CYC(b_+4, b_+5); SET_DE(pop_effect(gb));
  CYC(b_+5, b_+7); A = 0x03;
  CYC(b_+7, SYM(loadTreeGfx_body));
  refresh_object_gfx_next_extra(gb, gb->sp);
}

void loadObjectGfxHeaderToSlot4_body_hook(GB *gb) {
  BASE(loadObjectGfxHeaderToSlot4_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CALL_C(b_+1, refreshObjectGfx_body_hook, SYM(refreshObjectGfx_body), b_+4);
  loadObjectGfxHeaderToSlot4_body__afterCall41f0_hook(gb);
}

void loadTreeGfx_body_hook(GB *gb) {
  BASE(loadTreeGfx_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wLoadedTreeGfxActive);
  CYC(b_+3, b_+4); A = E;
  CYC(b_+4, b_+5); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, insertIndexIntoLoadedObjectGfx_hook, SYM(insertIndexIntoLoadedObjectGfx), b_+9);
  CYC(b_+9, SYM(updateTileIndexBaseForAllObjects)); resumeThreadNextFrameIfLcdIsOn_hook(gb);
}

void updateTileIndexBaseForAllObjects__afterCall4252_hook(GB *gb) {
  BASE(updateTileIndexBaseForAllObjects);
  CYC(b_+81, b_+82); SET_HL(pop_effect(gb));
  CYC(b_+82, b_+83); SET_DE(pop_effect(gb));
  CYC(b_+83, b_+84); SET_BC(pop_effect(gb));
  CYC(b_+84, b_+85); ret_effect(gb);
}

void updateTileIndexBaseForAllObjects_hook(GB *gb) {
  BASE(updateTileIndexBaseForAllObjects);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+5); A = 0x80;
  CYC(b_+5, b_+7); H8(hActiveObjectType) = A;
  CYC(b_+7, b_+9); D = 0xd0;
  do {
    CALL_C(b_+9, enemyGetObjectGfxIndex_hook, SYM(enemyGetObjectGfxIndex), b_+12);
    CALL_C(b_+12, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, b_+85, b_+15);
    CYC(b_+15, b_+16); D = alu_inc8(gb, D);
    CYC(b_+16, b_+17); A = D;
    CYC(b_+17, b_+19); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+19, b_+21);
    else CYC(b_+19, b_+21);
  } while (F & FC);
  CYC(b_+21, b_+23); A = 0xc0;
  CYC(b_+23, b_+25); H8(hActiveObjectType) = A;
  CYC(b_+25, b_+27); D = 0xd0;
  do {
    CALL_C(b_+27, partGetObjectGfxIndex_hook, SYM(partGetObjectGfxIndex), b_+30);
    CALL_C(b_+30, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, b_+85, b_+33);
    CYC(b_+33, b_+34); D = alu_inc8(gb, D);
    CYC(b_+34, b_+35); A = D;
    CYC(b_+35, b_+37); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+37, b_+39);
    else CYC(b_+37, b_+39);
  } while (F & FC);
  CYC(b_+39, b_+41); A = 0x40;
  CYC(b_+41, b_+43); H8(hActiveObjectType) = A;
  CYC(b_+43, b_+45); D = 0xd2;
  do {
    CALL_C(b_+45, interactionGetObjectGfxIndex_hook, SYM(interactionGetObjectGfxIndex), b_+48);
    CALL_C(b_+48, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, b_+85, b_+51);
    CYC(b_+51, b_+52); D = alu_inc8(gb, D);
    CYC(b_+52, b_+53); A = D;
    CYC(b_+53, b_+55); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+55, b_+57);
    else CYC(b_+55, b_+57);
  } while (F & FC);
  CYC(b_+57, b_+59); A = 0x00;
  CYC(b_+59, b_+61); H8(hActiveObjectType) = A;
  CYC(b_+61, b_+63); D = 0xd6;
  do {
    CALL_C(b_+63, itemGetObjectGfxIndex_hook, SYM(itemGetObjectGfxIndex), b_+66);
    CALL_C(b_+66, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, b_+85, b_+69);
    CYC(b_+69, b_+70); D = alu_inc8(gb, D);
    CYC(b_+70, b_+71); A = D;
    CYC(b_+71, b_+73); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(b_+73, b_+75);
    else CYC(b_+73, b_+75);
  } while (F & FC);
  CALL_C(b_+75, drawAllSpritesUnconditionally_hook, SYM(drawAllSpritesUnconditionally), b_+78);
  CALL_C(b_+78, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+81);
  updateTileIndexBaseForAllObjects__afterCall4252_hook(gb);
}

void enemyLoadGraphicsAndProperties__afterCall4372_hook(GB *gb) {
  BASE(enemyLoadGraphicsAndProperties);
  CYC(b_+10, b_+12); E = 0x81;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); E = 0xa4;
  CYC(b_+15, b_+17); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+21); A |= 0x80;
  }
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+26); alu_and(gb, 0x7f);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+32, b_+34); alu_and(gb, 0x7f);
    CYC(b_+34, b_+35); L = mem_rd(gb, HL);
    CYC(b_+35, b_+36); H = A;
    CYC(b_+36, b_+38); E = 0x82;
    CYC(b_+38, b_+39); A = mem_rd(gb, DE);
    CYC(b_+39, b_+40); B = A;
    CYC(b_+40, b_+42); E = 0x00;
    for (;;) {
      CYC(b_+42, b_+44); alu_bit(gb, 7, mem_rd(gb, HL));
      if (F & FZ) {
        CYCT(b_+44, b_+46);
        break;
      }
      CYC(b_+44, b_+46);
      CYC(b_+46, b_+47); A = E;
      CYC(b_+47, b_+48); alu_cp(gb, B);
      if (F & FZ) {
        CYCT(b_+48, b_+50);
        break;
      }
      CYC(b_+48, b_+50);
      CYC(b_+50, b_+51); SET_HL(HL + 1);
      CYC(b_+51, b_+52); SET_HL(HL + 1);
      CYC(b_+52, b_+53); E = alu_inc8(gb, E);
      CYC(b_+53, b_+55);
    }
  } else {
    CYCT(b_+29, b_+31);
  }
  CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+56, b_+57); push_effect(gb, HL);
  CYC(b_+57, b_+58); alu_add(gb, A);
  CYC(b_+58, b_+61); SET_HL(SYM(extraEnemyData));
  CYC(b_+61, b_+62); add_double_index_to_hl_from_rst(gb, b_+62);
  CYC(b_+62, b_+64); E = 0xa6;
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+67); E = alu_inc8(gb, E);
  CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+70); E = alu_inc8(gb, E);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+73); E = alu_inc8(gb, E);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); SET_HL(pop_effect(gb));
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_and(gb, 0x0f);
  CYC(b_+79, b_+80); alu_add(gb, A);
  CYC(b_+80, b_+81); alu_add(gb, C);
  CYC(b_+81, b_+83); E = 0x9d;
  CYC(b_+83, b_+84); mem_wr(gb, DE, A);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); A = alu_swap(gb, A);
  CYC(b_+87, b_+89); alu_and(gb, 0x0f);
  CYC(b_+89, b_+90); E = alu_dec8(gb, E);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+92); E = alu_dec8(gb, E);
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, SYM(partLoadGraphicsAndProperties)); enemySetAnimation_hook(gb);
}

void enemyLoadGraphicsAndProperties_hook(GB *gb) {
  BASE(enemyLoadGraphicsAndProperties);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyGetObjectGfxIndex_hook, SYM(enemyGetObjectGfxIndex), b_+3);
  CALL_C(b_+3, addIndexToLoadedObjectGfx_hook, SYM(addIndexToLoadedObjectGfx), b_+6);
  CYC(b_+6, b_+7); C = A;
  if (F & FC) {
    CALL_C_CC(b_+7, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  enemyLoadGraphicsAndProperties__afterCall4372_hook(gb);
}

void partLoadGraphicsAndProperties__afterCall43d3_hook(GB *gb) {
  BASE(partLoadGraphicsAndProperties);
  CYC(b_+10, b_+12); E = 0xc1;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+15, b_+17);
  } else {
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+19); A |= 0x80;
  }
  CYC(b_+19, b_+21); E = 0xe4;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+26); alu_and(gb, 0x7f);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); A = alu_swap(gb, A);
  CYC(b_+31, b_+33); alu_and(gb, 0x0f);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); E = alu_inc8(gb, E);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+38); alu_and(gb, 0x0f);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); E = alu_inc8(gb, E);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); E = alu_inc8(gb, E);
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+47); E = 0xdd;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+48, b_+49); alu_add(gb, C);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+51); E = alu_dec8(gb, E);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+54); E = alu_dec8(gb, E);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, SYM(interactionLoadGraphics)); partSetAnimation_hook(gb);
}

void partLoadGraphicsAndProperties_hook(GB *gb) {
  BASE(partLoadGraphicsAndProperties);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, partGetObjectGfxIndex_hook, SYM(partGetObjectGfxIndex), b_+3);
  CALL_C(b_+3, addIndexToLoadedObjectGfx_hook, SYM(addIndexToLoadedObjectGfx), b_+6);
  CYC(b_+6, b_+7); C = A;
  if (F & FC) {
    CALL_C_CC(b_+7, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  partLoadGraphicsAndProperties__afterCall43d3_hook(gb);
}

void interactionLoadGraphics__afterCall440e_hook(GB *gb) {
  BASE(interactionLoadGraphics);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+13); alu_and(gb, 0x7f);
  CYC(b_+13, b_+14); alu_add(gb, C);
  CYC(b_+14, b_+16); E = 0x5d;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+22); alu_and(gb, 0x0f);
  CYC(b_+22, b_+23); E = alu_dec8(gb, E);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+25); E = alu_dec8(gb, E);
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_and(gb, 0x0f);
  CYC(b_+29, SYM(itemLoadGraphics)); ret_effect(gb);
}

void interactionLoadGraphics_hook(GB *gb) {
  BASE(interactionLoadGraphics);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionGetObjectGfxIndex_hook, SYM(interactionGetObjectGfxIndex), b_+3);
  CALL_C(b_+3, addIndexToLoadedObjectGfx_hook, SYM(addIndexToLoadedObjectGfx), b_+6);
  CYC(b_+6, b_+7); C = A;
  if (F & FC) {
    CALL_C_CC(b_+7, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  interactionLoadGraphics__afterCall440e_hook(gb);
}

void itemLoadGraphics__afterCall442c_hook(GB *gb) {
  BASE(itemLoadGraphics);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); alu_add(gb, C);
  CYC(b_+12, b_+14); E = 0x1d;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); E = alu_dec8(gb, E);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); E = alu_dec8(gb, E);
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, SYM(interactionGetData)); ret_effect(gb);
}

void itemLoadGraphics_hook(GB *gb) {
  BASE(itemLoadGraphics);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemGetObjectGfxIndex_hook, SYM(itemGetObjectGfxIndex), b_+3);
  CALL_C(b_+3, addIndexToLoadedObjectGfx_hook, SYM(addIndexToLoadedObjectGfx), b_+6);
  CYC(b_+6, b_+7); C = A;
  if (F & FC) {
    CALL_C_CC(b_+7, resumeThreadNextFrameIfLcdIsOn_hook, SYM(resumeThreadNextFrameIfLcdIsOn), b_+10);
  } else {
    CYC(b_+7, b_+10);
  }
  itemLoadGraphics__afterCall442c_hook(gb);
}

void interactionGetData_hook(GB *gb) {
  BASE(interactionGetData);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x41;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, SYM(getDataForInteraction)); E = mem_rd(gb, HL);
  getDataForInteraction_hook(gb);
}

void getDataForInteraction_hook(GB *gb) {
  BASE(getDataForInteraction);
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+3); B = 0x00;
  CYC(b_+3, b_+6); SET_HL((SYM(interactionData) + 1));
  CYC(b_+6, b_+7); alu_add_hl(gb, BC);
  CYC(b_+7, b_+8); alu_add_hl(gb, BC);
  CYC(b_+8, b_+9); alu_add_hl(gb, BC);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+11, b_+12);
    ret_effect(gb);
    return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+18); C = 0x03;
  CYC(b_+18, b_+19); A = E;
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+20, b_+21);
    ret_effect(gb);
    return;
  }
  CYC(b_+20, b_+21);
  for (;;) {
    CYC(b_+21, b_+22); SET_HL(HL + 1);
    CYC(b_+22, b_+24); alu_bit(gb, 7, mem_rd(gb, HL));
    CYC(b_+24, b_+25); SET_HL(HL - 1);
    if (!(F & FZ)) {
      CYCT(b_+25, b_+26);
      ret_effect(gb);
      return;
    }
    CYC(b_+25, b_+26);
    CYC(b_+26, b_+27); alu_add_hl(gb, BC);
    CYC(b_+27, b_+28); A = alu_dec8(gb, A);
    if (F & FZ) {
      CYC(b_+28, b_+30);
      break;
    }
    CYCT(b_+28, b_+30);
  }
  CYC(b_+30, SYM(loadWeaponGfx_b3f)); ret_effect(gb);
}

void loadWeaponGfx_b3f_hook(GB *gb) {
  BASE(loadWeaponGfx_b3f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wLoadedItemGraphic1);
  CYC(b_+3, b_+4); A = E;
  CYC(b_+4, b_+6); alu_cp(gb, 0x1a);
  if (!(F & FC)) {
    CYCT(b_+6, b_+8);
  } else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  }
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+10, b_+11);
    ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+13); push_effect(gb, DE);
  CALL_C(b_+13, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+16);
  CYC(b_+16, b_+17); SET_DE(pop_effect(gb));
  CYC(b_+17, SYM(checkTreasureObtained_body)); ret_effect(gb);
}
