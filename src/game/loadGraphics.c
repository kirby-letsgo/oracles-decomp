#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4000, 0x4002); A = mem_rd(gb, IO_SVBK);
  CYC(0x4002, 0x4003); push_effect(gb, AF);
  CYC(0x4003, 0x4005); A = 0x02;
  CYC(0x4005, 0x4007); mem_wr(gb, IO_SVBK, A);
  CYC(0x4007, 0x400a); SET_HL(0x409d);
  CYC(0x400a, 0x400d); SET_DE(w2GbaModePaletteData);
  CYC(0x400d, 0x400f); B = 0x80;
  CALL_C(0x400f, copyMemory_hook, 0x0486, 0x4012);
  CYC(0x4012, 0x4013); SET_AF(pop_effect(gb));
  CYC(0x4013, 0x4015); mem_wr(gb, IO_SVBK, A);
  CYC(0x4015, 0x4016); ret_effect(gb);
}

void refreshDirtyPalettes_hook(GB *gb) {
  CYC(0x4016, 0x4018); A = 0x02;
  CYC(0x4018, 0x401a); mem_wr(gb, IO_SVBK, A);
  CYC(0x401a, 0x401c); A = H8(hDirtyBgPalettes);
  CYC(0x401c, 0x401d); D = A;
  CYC(0x401d, 0x401f); A = H8(hBgPaletteSources);
  CYC(0x401f, 0x4020); E = A;
  CYC(0x4020, 0x4022); L = (uint8_t)w2TilesetBgPalettes;
  CYC(0x4022, 0x4025); push_effect(gb, 0x4025);
  refresh_dirty_palettes_refresh(gb);
  CYC(0x4025, 0x4027); A = H8(hDirtySprPalettes);
  CYC(0x4027, 0x4028); D = A;
  CYC(0x4028, 0x402a); A = H8(hSprPaletteSources);
  CYC(0x402a, 0x402b); E = A;
  CYC(0x402b, 0x402d); L = (uint8_t)w2TilesetSprPalettes;
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_refresh(GB *gb) {
  CYC(0x402d, 0x402e); A = D;
  CYC(0x402e, 0x402f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x402f, 0x4030);
    ret_effect(gb);
    return;
  }
  CYC(0x402f, 0x4030);
  CYC(0x4030, 0x4032); D = alu_srl(gb, D);
  if (!(F & FC)) {
    CYCT(0x4032, 0x4034);
    refresh_dirty_palettes_next_palette(gb);
    return;
  }
  CYC(0x4032, 0x4034);
  CYC(0x4034, 0x4036); H = (uint8_t)(w2TilesetBgPalettes >> 8);
  CYC(0x4036, 0x4038); E = alu_srl(gb, E);
  if (!(F & FC)) {
    CYCT(0x4038, 0x403a);
  } else {
    CYC(0x4038, 0x403a);
    CYC(0x403a, 0x403b); H = alu_inc8(gb, H);
  }
  CYC(0x403b, 0x403d); A = H8(hGameboyType);
  CYC(0x403d, 0x403e); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x403e, 0x4040);
    refresh_dirty_palettes_gbc_mode(gb);
    return;
  }
  CYC(0x403e, 0x4040);
  refresh_dirty_palettes_gba_mode(gb);
}

static void refresh_dirty_palettes_gba_mode(GB *gb) {
  CYC(0x4040, 0x4043); push_effect(gb, 0x4043);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(0x4043, 0x4046); push_effect(gb, 0x4046);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(0x4046, 0x4049); push_effect(gb, 0x4049);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(0x4049, 0x404c); push_effect(gb, 0x404c);
  refresh_dirty_palettes_gba_brighten_palette(gb);
  CYC(0x404c, 0x404e);
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_gbc_mode(GB *gb) {
  CYC(0x404e, 0x404f); push_effect(gb, DE);
  CYC(0x404f, 0x4051); B = (uint8_t)(w2BgPalettesBuffer >> 8);
  CYC(0x4051, 0x4052); C = L;
  CYC(0x4052, 0x4054); C = (uint8_t)(C & ~(1 << 7));
  CYC(0x4054, 0x4056); E = 0x08;
  for (;;) {
    CYC(0x4056, 0x4057); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4057, 0x4058); mem_wr(gb, BC, A);
    CYC(0x4058, 0x4059); C = alu_inc8(gb, C);
    CYC(0x4059, 0x405a); E = alu_dec8(gb, E);
    if (F & FZ) {
      CYC(0x405a, 0x405c);
      break;
    }
    CYCT(0x405a, 0x405c);
  }
  CYC(0x405c, 0x405d); SET_DE(pop_effect(gb));
  CYC(0x405d, 0x405f);
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_next_palette(GB *gb) {
  CYC(0x405f, 0x4060); A = L;
  CYC(0x4060, 0x4062); alu_add(gb, 0x08);
  CYC(0x4062, 0x4063); L = A;
  CYC(0x4063, 0x4065); E = alu_srl(gb, E);
  CYC(0x4065, 0x4067);
  refresh_dirty_palettes_refresh(gb);
}

static void refresh_dirty_palettes_gba_brighten_palette(GB *gb) {
  CYC(0x4067, 0x4068); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4068, 0x4069); C = A;
  CYC(0x4069, 0x406b); alu_and(gb, 0xe0);
  CYC(0x406b, 0x406c); B = A;
  CYC(0x406c, 0x406d); A = mem_rd(gb, HL);
  CYC(0x406d, 0x406f); alu_and(gb, 0x03);
  CYC(0x406f, 0x4070); alu_or(gb, B);
  CYC(0x4070, 0x4072); A = alu_swap(gb, A);
  CYC(0x4072, 0x4073); B = A;
  CYC(0x4073, 0x4074); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4074, 0x4076); alu_and(gb, 0x7c);
  CYC(0x4076, 0x4077); alu_rrca(gb);
  CYC(0x4077, 0x4078); alu_rrca(gb);
  CYC(0x4078, 0x4079); push_effect(gb, HL);
  CYC(0x4079, 0x407c); SET_HL(w2GbaModePaletteData + 0x60);
  CYC(0x407c, 0x407d); add_a_to_hl_from_rst(gb, 0x407d);
  CYC(0x407d, 0x407e); A = B;
  CYC(0x407e, 0x407f); B = mem_rd(gb, HL);
  CYC(0x407f, 0x4082); SET_HL(w2GbaModePaletteData + 0x21);
  CYC(0x4082, 0x4083); add_a_to_hl_from_rst(gb, 0x4083);
  CYC(0x4083, 0x4084); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4084, 0x4085); alu_or(gb, B);
  CYC(0x4085, 0x4086); B = A;
  CYC(0x4086, 0x4087); A = C;
  CYC(0x4087, 0x4089); alu_and(gb, 0x1f);
  CYC(0x4089, 0x408a); C = mem_rd(gb, HL);
  CYC(0x408a, 0x408d); SET_HL(w2GbaModePaletteData);
  CYC(0x408d, 0x408e); add_a_to_hl_from_rst(gb, 0x408e);
  CYC(0x408e, 0x408f); A = mem_rd(gb, HL);
  CYC(0x408f, 0x4090); alu_or(gb, C);
  CYC(0x4090, 0x4091); SET_HL(pop_effect(gb));
  CYC(0x4091, 0x4092); C = H;
  CYC(0x4092, 0x4094); L = (uint8_t)(L & ~(1 << 7));
  CYC(0x4094, 0x4096); H = (uint8_t)(w2BgPalettesBuffer >> 8);
  CYC(0x4096, 0x4097); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4097, 0x4098); A = B;
  CYC(0x4098, 0x4099); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4099, 0x409b); L = (uint8_t)(L | (1 << 7));
  CYC(0x409b, 0x409c); H = C;
  CYC(0x409c, 0x409d); ret_effect(gb);
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
  CYC(0x4270, 0x4271); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4271, 0x4272);
    ret_effect(gb);
    return;
  }
  CYC(0x4271, 0x4272);
  CYC(0x4272, 0x4275); SET_HL(wLoadedObjectGfx);
  CYC(0x4275, 0x4277); B = 0x08;
  CYC(0x4277, 0x4278); C = A;

  for (;;) {
    CYC(0x4278, 0x4279); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4279, 0x427a); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(0x427a, 0x427c);
      break;
    }
    CYC(0x427a, 0x427c);
    CYC(0x427c, 0x427d); L = alu_inc8(gb, L);
    CYC(0x427d, 0x427e); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x427e, 0x4280);
      CYC(0x4280, 0x4282); C = 0x01;
      CYC(0x4282, 0x4283); alu_scf(gb);
      CYC(0x4283, 0x4284); ret_effect(gb);
      return;
    }
    CYCT(0x427e, 0x4280);
  }

  CYC(0x4284, 0x4286); mem_wr(gb, HL, 0x01);
  CYC(0x4286, 0x4287); L = alu_dec8(gb, L);
  CYC(0x4287, 0x4288); A = L;
  CYC(0x4288, 0x428a); alu_sub(gb, (uint8_t)wLoadedObjectGfx);
  CYC(0x428a, 0x428c); A = alu_swap(gb, A);
  CYC(0x428c, 0x428d); C = A;
  CYC(0x428d, 0x428e); ret_effect(gb);
}

void updateTileIndexBaseForAllObjects__updateTileIndexBase_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4256, 0x4257); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4257, 0x4258);
    ret_effect(gb);
    return;
  }
  CYC(0x4257, 0x4258);
  CALL_C(0x4258, findIndexInLoadedObjectGfx_hook, 0x4270, 0x425b);
  CYC(0x425b, 0x425d); A = H8(hActiveObjectType);
  CYC(0x425d, 0x425e); E = A;
  CYC(0x425e, 0x425f); A = mem_rd(gb, DE);
  CYC(0x425f, 0x4260); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4260, 0x4261);
    ret_effect(gb);
    return;
  }
  CYC(0x4260, 0x4261);
  CYC(0x4261, 0x4262); A = E;
  CYC(0x4262, 0x4264); alu_add(gb, 0x1c);
  CYC(0x4264, 0x4265); E = A;
  CYC(0x4265, 0x4266); A = mem_rd(gb, DE);
  CYC(0x4266, 0x4268); alu_bit(gb, 3, A);
  if (!(F & FZ)) {
    CYCT(0x4268, 0x4269);
    ret_effect(gb);
    return;
  }
  CYC(0x4268, 0x4269);
  CYC(0x4269, 0x426a); E = alu_inc8(gb, E);
  CYC(0x426a, 0x426b); A = mem_rd(gb, DE);
  CYC(0x426b, 0x426d); alu_and(gb, 0x1f);
  CYC(0x426d, 0x426e); alu_add(gb, C);
  CYC(0x426e, 0x426f); mem_wr(gb, DE, A);
  CYC(0x426f, 0x4270); ret_effect(gb);
}

void findUnusedIndexInLoadedObjectGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x428e, 0x4290); B = 0x08;

  for (;;) {
    CALL_C(0x4290, getAddressOfLoadedObjectGfxIndex_hook, 0x42b3, 0x4293);
    CYC(0x4293, 0x4294); L = alu_inc8(gb, L);
    CYC(0x4294, 0x4295); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(0x4295, 0x4296); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x4296, 0x4298);
      break;
    }
    CYC(0x4296, 0x4298);
    CALL_C(0x4298, incLoadedObjectGfxIndex_hook, 0x42a9, 0x429b);
    CYC(0x429b, 0x429c); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x429c, 0x429e);
      CYC(0x429e, 0x42a0); C = 0x01;
      CYC(0x42a0, 0x42a1); alu_scf(gb);
      CYC(0x42a1, 0x42a2); ret_effect(gb);
      return;
    }
    CYCT(0x429c, 0x429e);
  }

  CYC(0x42a2, 0x42a3); A = L;
  CYC(0x42a3, 0x42a5); alu_sub(gb, (uint8_t)wLoadedObjectGfx);
  CYC(0x42a5, 0x42a7); A = alu_swap(gb, A);
  CYC(0x42a7, 0x42a8); C = A;
  CYC(0x42a8, 0x42a9); ret_effect(gb);
}

void incLoadedObjectGfxIndex_hook(GB *gb) {
  CYC(0x42a9, 0x42ac); A = W8(wLoadedObjectGfxIndex);
  CYC(0x42ac, 0x42ad); A = alu_inc8(gb, A);
  CYC(0x42ad, 0x42af); alu_and(gb, 0x07);
  CYC(0x42af, 0x42b2); W8(wLoadedObjectGfxIndex) = A;
  CYC(0x42b2, 0x42b3); ret_effect(gb);
}

void getAddressOfLoadedObjectGfxIndex_hook(GB *gb) {
  CYC(0x42b3, 0x42b6); A = W8(wLoadedObjectGfxIndex);
  CYC(0x42b6, 0x42b9); SET_HL(wLoadedObjectGfx);
  CYC(0x42b9, 0x42ba); add_double_index_to_hl_from_rst(gb, 0x42ba);
  CYC(0x42ba, 0x42bb); ret_effect(gb);
}

void addIndexToLoadedObjectGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42bb, 0x42bc); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x42bc, 0x42bd);
    ret_effect(gb);
    return;
  }
  CYC(0x42bc, 0x42bd);
  CYC(0x42bd, 0x42be); push_effect(gb, HL);
  CYC(0x42be, 0x42bf); push_effect(gb, BC);
  CYC(0x42bf, 0x42c0); E = A;
  CALL_C(0x42c0, findIndexInLoadedObjectGfx_hook, 0x4270, 0x42c3);
  if (!(F & FC)) {
    CYCT(0x42c3, 0x42c5);
  } else {
    CYC(0x42c3, 0x42c5);
    CALL_C(0x42c5, findUnusedIndexInLoadedObjectGfx_hook, 0x428e, 0x42c8);
    if (!(F & FC)) {
      CALL_C_CC(0x42c8, insertIndexIntoLoadedObjectGfx_hook, 0x42cf, 0x42cb);
    } else {
      CYC(0x42c8, 0x42cb);
    }
  }
  CYC(0x42cb, 0x42cc); A = C;
  CYC(0x42cc, 0x42cd); SET_BC(pop_effect(gb));
  CYC(0x42cd, 0x42ce); SET_HL(pop_effect(gb));
  CYC(0x42ce, 0x42cf); ret_effect(gb);
}

static void insert_index_into_loaded_object_gfx_tail(GB *gb, uint16_t sp0_) {
  CYC(0x4302, 0x4304); D = 0x00;
  CYC(0x4304, 0x4305); alu_add_hl(gb, DE);
  CYC(0x4305, 0x4306); alu_add_hl(gb, DE);
  CYC(0x4306, 0x4307); alu_add_hl(gb, DE);
  CALL_C(0x4307, loadObjectGfx_hook, 0x1682, 0x430a);
  CYC(0x430a, 0x430b); SET_HL(pop_effect(gb));
  CYC(0x430b, 0x430c); SET_DE(pop_effect(gb));
  CYC(0x430c, 0x430d); SET_BC(pop_effect(gb));
  CYC(0x430d, 0x430e); ret_effect(gb);
}

static void insert_index_into_loaded_object_gfx_tree(GB *gb, uint16_t sp0_) {
  CYC(0x42f3, 0x42f5); B = 0x92;
  CYC(0x42f5, 0x42f8); SET_HL(0x5d2a);
  CYC(0x42f8, 0x42fa);
  insert_index_into_loaded_object_gfx_tail(gb, sp0_);
}

static void insert_index_into_loaded_object_gfx_object(GB *gb, uint16_t sp0_) {
  CYC(0x42fa, 0x42fc); alu_sub(gb, 0x08);
  CYC(0x42fc, 0x42fe); alu_or(gb, 0x80);
  CYC(0x42fe, 0x42ff); B = A;
  CYC(0x42ff, 0x4302); SET_HL(0x5a8a);
  insert_index_into_loaded_object_gfx_tail(gb, sp0_);
}

void insertIndexIntoLoadedObjectGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42cf, 0x42d0); A = L;
  CYC(0x42d0, 0x42d2); alu_cp(gb, (uint8_t)wLoadedTreeGfxActive);
  if (F & FC) {
    CYC(0x42d2, 0x42d4);
    CYC(0x42d4, 0x42d5); push_effect(gb, HL);
    CYC(0x42d5, 0x42d8); SET_HL(wLoadedObjectGfx);
    for (;;) {
      CYC(0x42d8, 0x42d9); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(0x42d9, 0x42da); alu_cp(gb, E);
      if (F & FZ) {
        CYC(0x42da, 0x42dc);
        CYC(0x42dc, 0x42dd); alu_xor(gb, A);
        CYC(0x42dd, 0x42de); mem_wr(gb, HL, A); SET_HL(HL - 1);
        CYC(0x42de, 0x42df); mem_wr(gb, HL, A); SET_HL(HL + 1);
      } else {
        CYCT(0x42da, 0x42dc);
      }
      CYC(0x42df, 0x42e0); L = alu_inc8(gb, L);
      CYC(0x42e0, 0x42e1); A = L;
      CYC(0x42e1, 0x42e3); alu_cp(gb, (uint8_t)wLoadedObjectGfxEnd);
      if (!(F & FC)) {
        CYC(0x42e3, 0x42e5);
        break;
      }
      CYCT(0x42e3, 0x42e5);
    }
    CYC(0x42e5, 0x42e6); SET_HL(pop_effect(gb));
  } else {
    CYCT(0x42d2, 0x42d4);
  }
  CYC(0x42e6, 0x42e7); push_effect(gb, BC);
  CYC(0x42e7, 0x42e8); push_effect(gb, DE);
  CYC(0x42e8, 0x42e9); push_effect(gb, HL);
  CYC(0x42e9, 0x42ea); mem_wr(gb, HL, E);
  CYC(0x42ea, 0x42eb); L = alu_inc8(gb, L);
  CYC(0x42eb, 0x42ed); mem_wr(gb, HL, 0x01);
  CYC(0x42ed, 0x42ee); L = alu_dec8(gb, L);
  CYC(0x42ee, 0x42ef); A = L;
  CYC(0x42ef, 0x42f1); alu_cp(gb, (uint8_t)wLoadedTreeGfxActive);
  if (F & FC) {
    CYCT(0x42f1, 0x42f3);
    insert_index_into_loaded_object_gfx_object(gb, sp0_);
  } else {
    CYC(0x42f1, 0x42f3);
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
  CYC(0x4324, 0x4325); SET_HL(pop_effect(gb));
  CYC(0x4325, 0x4326); SET_BC(pop_effect(gb));
  CYC(0x4326, 0x4327); ret_effect(gb);
}

static void mark_loaded_object_gfx_used_found(GB *gb) {
  CYC(0x4322, 0x4324); mem_wr(gb, HL, 0x01);
  mark_loaded_object_gfx_used_end(gb);
}

void markLoadedObjectGfxUsed_hook(GB *gb) {
  CYC(0x430e, 0x430f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x430f, 0x4310);
    ret_effect(gb);
    return;
  }
  CYC(0x430f, 0x4310);
  CYC(0x4310, 0x4311); push_effect(gb, BC);
  CYC(0x4311, 0x4312); push_effect(gb, HL);
  CYC(0x4312, 0x4315); SET_HL(wLoadedObjectGfx);
  CYC(0x4315, 0x4316); C = A;

  for (;;) {
    CYC(0x4316, 0x4317); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4317, 0x4318); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(0x4318, 0x431a);
      mark_loaded_object_gfx_used_found(gb);
      return;
    }
    CYC(0x4318, 0x431a);
    CYC(0x431a, 0x431b); L = alu_inc8(gb, L);
    CYC(0x431b, 0x431c); A = L;
    CYC(0x431c, 0x431e); alu_cp(gb, (uint8_t)wLoadedObjectGfxEnd);
    if (!(F & FC)) {
      CYC(0x431e, 0x4320);
      break;
    }
    CYCT(0x431e, 0x4320);
  }

  CYC(0x4320, 0x4322);
  mark_loaded_object_gfx_used_end(gb);
}

void markLoadedObjectGfxUsed__found_hook(GB *gb) {
  mark_loaded_object_gfx_used_found(gb);
}

void markLoadedObjectGfxUsed__end_hook(GB *gb) {
  mark_loaded_object_gfx_used_end(gb);
}

void markAllLoadedObjectGfxUnused_hook(GB *gb) {
  CYC(0x4327, 0x4328); push_effect(gb, BC);
  CYC(0x4328, 0x4329); push_effect(gb, HL);
  CYC(0x4329, 0x432c); SET_HL(wLoadedObjectGfx);
  CYC(0x432c, 0x432e); B = 0x08;
  CYC(0x432e, 0x432f); alu_xor(gb, A);
  for (;;) {
    CYC(0x432f, 0x4330); L = alu_inc8(gb, L);
    CYC(0x4330, 0x4331); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x4331, 0x4332); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x4332, 0x4334);
      break;
    }
    CYCT(0x4332, 0x4334);
  }
  CYC(0x4334, 0x4335); SET_HL(pop_effect(gb));
  CYC(0x4335, 0x4336); SET_BC(pop_effect(gb));
  CYC(0x4336, 0x4337); ret_effect(gb);
}

void enemyGetObjectGfxIndex_hook(GB *gb) {
  CYC(0x4337, 0x4339); E = 0x81;
  CYC(0x4339, 0x433a); A = mem_rd(gb, DE);
  getObjectGfxIndexForEnemy_hook(gb);
}

void getObjectGfxIndexForEnemy_hook(GB *gb) {
  CYC(0x433a, 0x433b); push_effect(gb, BC);
  CYC(0x433b, 0x433c); alu_add(gb, A);
  CYC(0x433c, 0x433d); C = A;
  CYC(0x433d, 0x433f); B = 0x00;
  CYC(0x433f, 0x4342); SET_HL(0x5d4b);
  CYC(0x4342, 0x4343); alu_add_hl(gb, BC);
  CYC(0x4343, 0x4344); alu_add_hl(gb, BC);
  CYC(0x4344, 0x4345); SET_BC(pop_effect(gb));
  CYC(0x4345, 0x4346); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4346, 0x4347); ret_effect(gb);
}

void partGetObjectGfxIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4347, 0x4348); push_effect(gb, BC);
  CYC(0x4348, 0x434a); E = 0xc1;
  CYC(0x434a, 0x434b); A = mem_rd(gb, DE);
  CALL_C(0x434b, multiplyABy8_hook, 0x01b7, 0x434e);
  CYC(0x434e, 0x4351); SET_HL(0x60cd);
  CYC(0x4351, 0x4352); alu_add_hl(gb, BC);
  CYC(0x4352, 0x4353); SET_BC(pop_effect(gb));
  CYC(0x4353, 0x4354); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4354, 0x4355); ret_effect(gb);
}

void interactionGetObjectGfxIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4355, 0x4356); push_effect(gb, BC);
  CALL_C(0x4356, interactionGetData_hook, 0x4437, 0x4359);
  CYC(0x4359, 0x435a); SET_BC(pop_effect(gb));
  CYC(0x435a, 0x435b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x435b, 0x435c); ret_effect(gb);
}

void itemGetObjectGfxIndex_hook(GB *gb) {
  CYC(0x435c, 0x435e); E = 0x01;
  CYC(0x435e, 0x435f); A = mem_rd(gb, DE);
  CYC(0x435f, 0x4360); L = A;
  CYC(0x4360, 0x4361); alu_add(gb, A);
  CYC(0x4361, 0x4362); alu_add(gb, L);
  CYC(0x4362, 0x4365); SET_HL(0x63a5);
  CYC(0x4365, 0x4366); add_a_to_hl_from_rst(gb, 0x4366);
  CYC(0x4366, 0x4367); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4367, 0x4368); ret_effect(gb);
}

void resumeThreadNextFrameIfLcdIsOn__afterCall4124_hook(GB *gb) {
  CYC(0x4124, 0x4125); ret_effect(gb);
}

void resumeThreadNextFrameIfLcdIsOn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x411d, 0x411f); A = mem_rd(gb, IO_LCDC);
  CYC(0x411f, 0x4120); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x4120, 0x4121); ret_effect(gb); return;
  }
  CYC(0x4120, 0x4121);
  CALL_C(0x4121, resumeThreadNextFrameAndSaveBank_hook, 0x08eb, 0x4124);
  resumeThreadNextFrameIfLcdIsOn__afterCall4124_hook(gb);
}

void reloadObjectGfx_b3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4125, 0x4128); A = W8(wLoadedItemGraphic1);
  CYC(0x4128, 0x4129); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(0x4129, loadUncompressedGfxHeader_hook, 0x05da, 0x412c);
  } else {
    CYC(0x4129, 0x412c);
  }
  CYC(0x412c, 0x412f); A = W8(wLoadedItemGraphic2);
  CYC(0x412f, 0x4130); alu_or(gb, A);
  if (!(F & FZ)) {
    CALL_C_CC(0x4130, loadUncompressedGfxHeader_hook, 0x05da, 0x4133);
  } else {
    CYC(0x4130, 0x4133);
  }
  agesFunc_3f_4133_hook(gb);
}

void agesFunc_3f_4133__afterCall4143_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  for (;;) {
    CYC(0x4143, 0x4144); L = alu_inc8(gb, L);
    CYC(0x4144, 0x4145); mem_wr(gb, HL, D);
    CYC(0x4145, 0x4146); L = alu_inc8(gb, L);
    CYC(0x4146, 0x4147); A = L;
    CYC(0x4147, 0x4149); alu_cp(gb, 0x18);
    if (!(F & FC)) {
      CYC(0x4149, 0x414b);
      break;
    }
    CYCT(0x4149, 0x414b);
    CYC(0x4136, 0x4137); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4137, 0x4138); E = A;
    CYC(0x4138, 0x4139); D = mem_rd(gb, HL);
    CYC(0x4139, 0x413a); L = alu_dec8(gb, L);
    CYC(0x413a, 0x413b); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x413b, 0x413d);
      continue;
    }
    CYC(0x413b, 0x413d);
    CALL_C(0x413d, insertIndexIntoLoadedObjectGfx_hook, 0x42cf, 0x4140);
    CALL_C(0x4140, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x4143);
  }
  CYC(0x414b, 0x414e); SET_HL(wLoadedTreeGfxIndex + 1);
  CYC(0x414e, 0x414f); E = mem_rd(gb, HL);
  CYC(0x414f, 0x4151); mem_wr(gb, HL, 0x00);
  CYC(0x4151, 0x4154); loadTreeGfx_body_hook(gb);
}

void agesFunc_3f_4133_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4133, 0x4136); SET_HL(wLoadedObjectGfx);
  CYC(0x4136, 0x4137); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4137, 0x4138); E = A;
  CYC(0x4138, 0x4139); D = mem_rd(gb, HL);
  CYC(0x4139, 0x413a); L = alu_dec8(gb, L);
  CYC(0x413a, 0x413b); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x413b, 0x413d);
  } else {
    CYC(0x413b, 0x413d);
    CALL_C(0x413d, insertIndexIntoLoadedObjectGfx_hook, 0x42cf, 0x4140);
    CALL_C(0x4140, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x4143);
  }
  agesFunc_3f_4133__afterCall4143_hook(gb);
}

static void refresh_object_gfx_next_extra(GB *gb, uint16_t sp0_) {
  CYC(0x41b6, 0x41b7); A = alu_inc8(gb, A);
  CYC(0x41b7, 0x41b9); alu_and(gb, 0x07);
  CYC(0x41b9, 0x41ba); B = A;
  CYC(0x41ba, 0x41bd); SET_HL(wLoadedObjectGfx + 1);
  CYC(0x41bd, 0x41be); add_double_index_to_hl_from_rst(gb, 0x41be);
  CYC(0x41be, 0x41bf); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x41bf, 0x41c0); D = A;
  CYC(0x41c0, 0x41c1); C = mem_rd(gb, HL);
  CYC(0x41c1, 0x41c2); E = alu_inc8(gb, E);
  CALL_C(0x41c2, insertIndexIntoLoadedObjectGfx_hook, 0x42cf, 0x41c5);
  CYC(0x41c5, 0x41c6); A = D;
  CYC(0x41c6, 0x41c7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x41c7, 0x41c9);
  } else {
    CYC(0x41c7, 0x41c9);
    CYC(0x41c9, 0x41ca); A = C;
    CYC(0x41ca, 0x41cb); push_effect(gb, DE);
    CALL_C(0x41cb, addIndexToLoadedObjectGfx_hook, 0x42bb, 0x41ce);
    CYC(0x41ce, 0x41cf); SET_DE(pop_effect(gb));
  }
  CALL_C(0x41cf, updateTileIndexBaseForAllObjects_hook, 0x4201, 0x41d2);
  refreshObjectGfx_body__afterCall41d2_hook(gb);
}

void refreshObjectGfx_body__afterCall41d2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41d2, 0x41d4); D = 0x00;
  CYC(0x41d4, 0x41d7); SET_HL(0x5a8b);
  CYC(0x41d7, 0x41d8); alu_add_hl(gb, DE);
  CYC(0x41d8, 0x41d9); alu_add_hl(gb, DE);
  CYC(0x41d9, 0x41da); alu_add_hl(gb, DE);
  CYC(0x41da, 0x41dc); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x41dc, 0x41dd); A = B;
  if (F & FZ) {
    CYCT(0x41dd, 0x41df);
    refresh_object_gfx_next_extra(gb, sp0_);
    return;
  }
  CYC(0x41dd, 0x41df);
  CYC(0x41df, 0x41e2); W8(wLoadedObjectGfxIndex) = A;
  CYC(0x41e2, 0x41e3); alu_xor(gb, A);
  CYC(0x41e3, 0x41e6); W8(wEnemyIDToLoadExtraGfx) = A;
  CYC(0x41e6, 0x41e9); W8(wInteractionIDToLoadExtraGfx) = A;
  CYC(0x41e9, 0x41ec); incLoadedObjectGfxIndex_hook(gb);
}

void refreshObjectGfx_body__afterCall41ad_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41ad, 0x41ae); A = E;
  CALL_C(0x41ae, findIndexInLoadedObjectGfx_hook, 0x4270, 0x41b1);
  CYC(0x41b1, 0x41b2); A = L;
  CYC(0x41b2, 0x41b4); alu_sub(gb, 0x08);
  CYC(0x41b4, 0x41b6); A = alu_srl(gb, A);
  refresh_object_gfx_next_extra(gb, sp0_);
}

void refreshObjectGfx_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4154, markAllLoadedObjectGfxUnused_hook, 0x4327, 0x4157);
  CYC(0x4157, 0x4159); D = 0xd0;
  do {
    CALL_C(0x4159, enemyGetObjectGfxIndex_hook, 0x4337, 0x415c);
    CALL_C(0x415c, markLoadedObjectGfxUsed_hook, 0x430e, 0x415f);
    CYC(0x415f, 0x4160); D = alu_inc8(gb, D);
    CYC(0x4160, 0x4161); A = D;
    CYC(0x4161, 0x4163); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x4163, 0x4165); else CYC(0x4163, 0x4165);
  } while (F & FC);
  CYC(0x4165, 0x4167); D = 0xd0;
  do {
    CALL_C(0x4167, partGetObjectGfxIndex_hook, 0x4347, 0x416a);
    CALL_C(0x416a, markLoadedObjectGfxUsed_hook, 0x430e, 0x416d);
    CYC(0x416d, 0x416e); D = alu_inc8(gb, D);
    CYC(0x416e, 0x416f); A = D;
    CYC(0x416f, 0x4171); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x4171, 0x4173); else CYC(0x4171, 0x4173);
  } while (F & FC);
  CYC(0x4173, 0x4175); D = 0xd0;
  do {
    CALL_C(0x4175, interactionGetObjectGfxIndex_hook, 0x4355, 0x4178);
    CALL_C(0x4178, markLoadedObjectGfxUsed_hook, 0x430e, 0x417b);
    CYC(0x417b, 0x417c); D = alu_inc8(gb, D);
    CYC(0x417c, 0x417d); A = D;
    CYC(0x417d, 0x417f); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x417f, 0x4181); else CYC(0x417f, 0x4181);
  } while (F & FC);
  CYC(0x4181, 0x4183); D = 0xd6;
  do {
    CALL_C(0x4183, itemGetObjectGfxIndex_hook, 0x435c, 0x4186);
    CALL_C(0x4186, markLoadedObjectGfxUsed_hook, 0x430e, 0x4189);
    CYC(0x4189, 0x418a); D = alu_inc8(gb, D);
    CYC(0x418a, 0x418b); A = D;
    CYC(0x418b, 0x418d); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x418d, 0x418f); else CYC(0x418d, 0x418f);
  } while (F & FC);
  CYC(0x418f, 0x4192); A = W8(wEnemyIDToLoadExtraGfx);
  CYC(0x4192, 0x4193); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(0x4193, 0x4195);
    CALL_C(0x4195, getObjectGfxIndexForEnemy_hook, 0x433a, 0x4198);
    CYC(0x4198, 0x419a);
  } else {
    CYCT(0x4193, 0x4195);
    CYC(0x419a, 0x419d); SET_HL(wInteractionIDToLoadExtraGfx);
    CYC(0x419d, 0x419e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x419e, 0x419f); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x419f, 0x41a0); ret_effect(gb); return;
    }
    CYC(0x419f, 0x41a0);
    CYC(0x41a0, 0x41a1); E = mem_rd(gb, HL);
    CYC(0x41a1, 0x41a3); mem_wr(gb, HL, 0x00);
    CALL_C(0x41a3, getDataForInteraction_hook, 0x443c, 0x41a6);
    CYC(0x41a6, 0x41a7); A = mem_rd(gb, HL);
  }
  CALL_C(0x41a7, addIndexToLoadedObjectGfx_hook, 0x42bb, 0x41aa);
  CALL_C(0x41aa, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x41ad);
  refreshObjectGfx_body__afterCall41ad_hook(gb);
}

void loadObjectGfxHeaderToSlot4_body__afterCall41f0_hook(GB *gb) {
  CYC(0x41f0, 0x41f1); SET_DE(pop_effect(gb));
  CYC(0x41f1, 0x41f3); A = 0x03;
  CYC(0x41f3, 0x41f5);
  refresh_object_gfx_next_extra(gb, gb->sp);
}

void loadObjectGfxHeaderToSlot4_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41ec, 0x41ed); push_effect(gb, DE);
  CALL_C(0x41ed, refreshObjectGfx_body_hook, 0x4154, 0x41f0);
  loadObjectGfxHeaderToSlot4_body__afterCall41f0_hook(gb);
}

void loadTreeGfx_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41f5, 0x41f8); SET_HL(wLoadedTreeGfxActive);
  CYC(0x41f8, 0x41f9); A = E;
  CYC(0x41f9, 0x41fa); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x41fa, 0x41fb); ret_effect(gb); return;
  }
  CYC(0x41fa, 0x41fb);
  CALL_C(0x41fb, insertIndexIntoLoadedObjectGfx_hook, 0x42cf, 0x41fe);
  CYC(0x41fe, 0x4201); resumeThreadNextFrameIfLcdIsOn_hook(gb);
}

void updateTileIndexBaseForAllObjects__afterCall4252_hook(GB *gb) {
  CYC(0x4252, 0x4253); SET_HL(pop_effect(gb));
  CYC(0x4253, 0x4254); SET_DE(pop_effect(gb));
  CYC(0x4254, 0x4255); SET_BC(pop_effect(gb));
  CYC(0x4255, 0x4256); ret_effect(gb);
}

void updateTileIndexBaseForAllObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4201, 0x4202); push_effect(gb, BC);
  CYC(0x4202, 0x4203); push_effect(gb, DE);
  CYC(0x4203, 0x4204); push_effect(gb, HL);
  CYC(0x4204, 0x4206); A = 0x80;
  CYC(0x4206, 0x4208); H8(hActiveObjectType) = A;
  CYC(0x4208, 0x420a); D = 0xd0;
  do {
    CALL_C(0x420a, enemyGetObjectGfxIndex_hook, 0x4337, 0x420d);
    CALL_C(0x420d, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, 0x4256, 0x4210);
    CYC(0x4210, 0x4211); D = alu_inc8(gb, D);
    CYC(0x4211, 0x4212); A = D;
    CYC(0x4212, 0x4214); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x4214, 0x4216);
    else CYC(0x4214, 0x4216);
  } while (F & FC);
  CYC(0x4216, 0x4218); A = 0xc0;
  CYC(0x4218, 0x421a); H8(hActiveObjectType) = A;
  CYC(0x421a, 0x421c); D = 0xd0;
  do {
    CALL_C(0x421c, partGetObjectGfxIndex_hook, 0x4347, 0x421f);
    CALL_C(0x421f, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, 0x4256, 0x4222);
    CYC(0x4222, 0x4223); D = alu_inc8(gb, D);
    CYC(0x4223, 0x4224); A = D;
    CYC(0x4224, 0x4226); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x4226, 0x4228);
    else CYC(0x4226, 0x4228);
  } while (F & FC);
  CYC(0x4228, 0x422a); A = 0x40;
  CYC(0x422a, 0x422c); H8(hActiveObjectType) = A;
  CYC(0x422c, 0x422e); D = 0xd2;
  do {
    CALL_C(0x422e, interactionGetObjectGfxIndex_hook, 0x4355, 0x4231);
    CALL_C(0x4231, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, 0x4256, 0x4234);
    CYC(0x4234, 0x4235); D = alu_inc8(gb, D);
    CYC(0x4235, 0x4236); A = D;
    CYC(0x4236, 0x4238); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x4238, 0x423a);
    else CYC(0x4238, 0x423a);
  } while (F & FC);
  CYC(0x423a, 0x423c); A = 0x00;
  CYC(0x423c, 0x423e); H8(hActiveObjectType) = A;
  CYC(0x423e, 0x4240); D = 0xd6;
  do {
    CALL_C(0x4240, itemGetObjectGfxIndex_hook, 0x435c, 0x4243);
    CALL_C(0x4243, updateTileIndexBaseForAllObjects__updateTileIndexBase_hook, 0x4256, 0x4246);
    CYC(0x4246, 0x4247); D = alu_inc8(gb, D);
    CYC(0x4247, 0x4248); A = D;
    CYC(0x4248, 0x424a); alu_cp(gb, 0xe0);
    if (F & FC) CYCT(0x424a, 0x424c);
    else CYC(0x424a, 0x424c);
  } while (F & FC);
  CALL_C(0x424c, drawAllSpritesUnconditionally_hook, 0x0da2, 0x424f);
  CALL_C(0x424f, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x4252);
  updateTileIndexBaseForAllObjects__afterCall4252_hook(gb);
}

void enemyLoadGraphicsAndProperties__afterCall4372_hook(GB *gb) {
  CYC(0x4372, 0x4374); E = 0x81;
  CYC(0x4374, 0x4375); A = mem_rd(gb, DE);
  CYC(0x4375, 0x4377); E = 0xa4;
  CYC(0x4377, 0x4379); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4379, 0x437b);
  } else {
    CYC(0x4379, 0x437b);
    CYC(0x437b, 0x437d); A |= 0x80;
  }
  CYC(0x437d, 0x437e); mem_wr(gb, DE, A);
  CYC(0x437e, 0x437f); E = alu_inc8(gb, E);
  CYC(0x437f, 0x4380); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4380, 0x4382); alu_and(gb, 0x7f);
  CYC(0x4382, 0x4383); mem_wr(gb, DE, A);
  CYC(0x4383, 0x4385); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYC(0x4385, 0x4387);
    CYC(0x4387, 0x4388); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4388, 0x438a); alu_and(gb, 0x7f);
    CYC(0x438a, 0x438b); L = mem_rd(gb, HL);
    CYC(0x438b, 0x438c); H = A;
    CYC(0x438c, 0x438e); E = 0x82;
    CYC(0x438e, 0x438f); A = mem_rd(gb, DE);
    CYC(0x438f, 0x4390); B = A;
    CYC(0x4390, 0x4392); E = 0x00;
    for (;;) {
      CYC(0x4392, 0x4394); alu_bit(gb, 7, mem_rd(gb, HL));
      if (F & FZ) {
        CYCT(0x4394, 0x4396);
        break;
      }
      CYC(0x4394, 0x4396);
      CYC(0x4396, 0x4397); A = E;
      CYC(0x4397, 0x4398); alu_cp(gb, B);
      if (F & FZ) {
        CYCT(0x4398, 0x439a);
        break;
      }
      CYC(0x4398, 0x439a);
      CYC(0x439a, 0x439b); SET_HL(HL + 1);
      CYC(0x439b, 0x439c); SET_HL(HL + 1);
      CYC(0x439c, 0x439d); E = alu_inc8(gb, E);
      CYC(0x439d, 0x439f);
    }
  } else {
    CYCT(0x4385, 0x4387);
  }
  CYC(0x439f, 0x43a0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43a0, 0x43a1); push_effect(gb, HL);
  CYC(0x43a1, 0x43a2); alu_add(gb, A);
  CYC(0x43a2, 0x43a5); SET_HL(0x5fb9);
  CYC(0x43a5, 0x43a6); add_double_index_to_hl_from_rst(gb, 0x43a6);
  CYC(0x43a6, 0x43a8); E = 0xa6;
  CYC(0x43a8, 0x43a9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43a9, 0x43aa); mem_wr(gb, DE, A);
  CYC(0x43aa, 0x43ab); E = alu_inc8(gb, E);
  CYC(0x43ab, 0x43ac); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43ac, 0x43ad); mem_wr(gb, DE, A);
  CYC(0x43ad, 0x43ae); E = alu_inc8(gb, E);
  CYC(0x43ae, 0x43af); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43af, 0x43b0); mem_wr(gb, DE, A);
  CYC(0x43b0, 0x43b1); E = alu_inc8(gb, E);
  CYC(0x43b1, 0x43b2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43b2, 0x43b3); mem_wr(gb, DE, A);
  CYC(0x43b3, 0x43b4); SET_HL(pop_effect(gb));
  CYC(0x43b4, 0x43b5); A = mem_rd(gb, HL);
  CYC(0x43b5, 0x43b7); alu_and(gb, 0x0f);
  CYC(0x43b7, 0x43b8); alu_add(gb, A);
  CYC(0x43b8, 0x43b9); alu_add(gb, C);
  CYC(0x43b9, 0x43bb); E = 0x9d;
  CYC(0x43bb, 0x43bc); mem_wr(gb, DE, A);
  CYC(0x43bc, 0x43bd); A = mem_rd(gb, HL);
  CYC(0x43bd, 0x43bf); A = alu_swap(gb, A);
  CYC(0x43bf, 0x43c1); alu_and(gb, 0x0f);
  CYC(0x43c1, 0x43c2); E = alu_dec8(gb, E);
  CYC(0x43c2, 0x43c3); mem_wr(gb, DE, A);
  CYC(0x43c3, 0x43c4); E = alu_dec8(gb, E);
  CYC(0x43c4, 0x43c5); mem_wr(gb, DE, A);
  CYC(0x43c5, 0x43c6); alu_xor(gb, A);
  CYC(0x43c6, 0x43c9); enemySetAnimation_hook(gb);
}

void enemyLoadGraphicsAndProperties_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4368, enemyGetObjectGfxIndex_hook, 0x4337, 0x436b);
  CALL_C(0x436b, addIndexToLoadedObjectGfx_hook, 0x42bb, 0x436e);
  CYC(0x436e, 0x436f); C = A;
  if (F & FC) {
    CALL_C_CC(0x436f, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x4372);
  } else {
    CYC(0x436f, 0x4372);
  }
  enemyLoadGraphicsAndProperties__afterCall4372_hook(gb);
}

void partLoadGraphicsAndProperties__afterCall43d3_hook(GB *gb) {
  CYC(0x43d3, 0x43d5); E = 0xc1;
  CYC(0x43d5, 0x43d6); A = mem_rd(gb, DE);
  CYC(0x43d6, 0x43d8); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x43d8, 0x43da);
  } else {
    CYC(0x43d8, 0x43da);
    CYC(0x43da, 0x43dc); A |= 0x80;
  }
  CYC(0x43dc, 0x43de); E = 0xe4;
  CYC(0x43de, 0x43df); mem_wr(gb, DE, A);
  CYC(0x43df, 0x43e0); E = alu_inc8(gb, E);
  CYC(0x43e0, 0x43e1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43e1, 0x43e3); alu_and(gb, 0x7f);
  CYC(0x43e3, 0x43e4); mem_wr(gb, DE, A);
  CYC(0x43e4, 0x43e5); E = alu_inc8(gb, E);
  CYC(0x43e5, 0x43e6); A = mem_rd(gb, HL);
  CYC(0x43e6, 0x43e8); A = alu_swap(gb, A);
  CYC(0x43e8, 0x43ea); alu_and(gb, 0x0f);
  CYC(0x43ea, 0x43eb); mem_wr(gb, DE, A);
  CYC(0x43eb, 0x43ec); E = alu_inc8(gb, E);
  CYC(0x43ec, 0x43ed); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43ed, 0x43ef); alu_and(gb, 0x0f);
  CYC(0x43ef, 0x43f0); mem_wr(gb, DE, A);
  CYC(0x43f0, 0x43f1); E = alu_inc8(gb, E);
  CYC(0x43f1, 0x43f2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f2, 0x43f3); mem_wr(gb, DE, A);
  CYC(0x43f3, 0x43f4); E = alu_inc8(gb, E);
  CYC(0x43f4, 0x43f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f5, 0x43f6); mem_wr(gb, DE, A);
  CYC(0x43f6, 0x43f8); E = 0xdd;
  CYC(0x43f8, 0x43f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f9, 0x43fa); alu_add(gb, C);
  CYC(0x43fa, 0x43fb); mem_wr(gb, DE, A);
  CYC(0x43fb, 0x43fc); E = alu_dec8(gb, E);
  CYC(0x43fc, 0x43fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43fd, 0x43fe); mem_wr(gb, DE, A);
  CYC(0x43fe, 0x43ff); E = alu_dec8(gb, E);
  CYC(0x43ff, 0x4400); mem_wr(gb, DE, A);
  CYC(0x4400, 0x4401); alu_xor(gb, A);
  CYC(0x4401, 0x4404); partSetAnimation_hook(gb);
}

void partLoadGraphicsAndProperties_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x43c9, partGetObjectGfxIndex_hook, 0x4347, 0x43cc);
  CALL_C(0x43cc, addIndexToLoadedObjectGfx_hook, 0x42bb, 0x43cf);
  CYC(0x43cf, 0x43d0); C = A;
  if (F & FC) {
    CALL_C_CC(0x43d0, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x43d3);
  } else {
    CYC(0x43d0, 0x43d3);
  }
  partLoadGraphicsAndProperties__afterCall43d3_hook(gb);
}

void interactionLoadGraphics__afterCall440e_hook(GB *gb) {
  CYC(0x440e, 0x440f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x440f, 0x4411); alu_and(gb, 0x7f);
  CYC(0x4411, 0x4412); alu_add(gb, C);
  CYC(0x4412, 0x4414); E = 0x5d;
  CYC(0x4414, 0x4415); mem_wr(gb, DE, A);
  CYC(0x4415, 0x4416); A = mem_rd(gb, HL);
  CYC(0x4416, 0x4418); A = alu_swap(gb, A);
  CYC(0x4418, 0x441a); alu_and(gb, 0x0f);
  CYC(0x441a, 0x441b); E = alu_dec8(gb, E);
  CYC(0x441b, 0x441c); mem_wr(gb, DE, A);
  CYC(0x441c, 0x441d); E = alu_dec8(gb, E);
  CYC(0x441d, 0x441e); mem_wr(gb, DE, A);
  CYC(0x441e, 0x441f); A = mem_rd(gb, HL);
  CYC(0x441f, 0x4421); alu_and(gb, 0x0f);
  CYC(0x4421, 0x4422); ret_effect(gb);
}

void interactionLoadGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4404, interactionGetObjectGfxIndex_hook, 0x4355, 0x4407);
  CALL_C(0x4407, addIndexToLoadedObjectGfx_hook, 0x42bb, 0x440a);
  CYC(0x440a, 0x440b); C = A;
  if (F & FC) {
    CALL_C_CC(0x440b, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x440e);
  } else {
    CYC(0x440b, 0x440e);
  }
  interactionLoadGraphics__afterCall440e_hook(gb);
}

void itemLoadGraphics__afterCall442c_hook(GB *gb) {
  CYC(0x442c, 0x442d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x442d, 0x442e); alu_add(gb, C);
  CYC(0x442e, 0x4430); E = 0x1d;
  CYC(0x4430, 0x4431); mem_wr(gb, DE, A);
  CYC(0x4431, 0x4432); A = mem_rd(gb, HL);
  CYC(0x4432, 0x4433); E = alu_dec8(gb, E);
  CYC(0x4433, 0x4434); mem_wr(gb, DE, A);
  CYC(0x4434, 0x4435); E = alu_dec8(gb, E);
  CYC(0x4435, 0x4436); mem_wr(gb, DE, A);
  CYC(0x4436, 0x4437); ret_effect(gb);
}

void itemLoadGraphics_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4422, itemGetObjectGfxIndex_hook, 0x435c, 0x4425);
  CALL_C(0x4425, addIndexToLoadedObjectGfx_hook, 0x42bb, 0x4428);
  CYC(0x4428, 0x4429); C = A;
  if (F & FC) {
    CALL_C_CC(0x4429, resumeThreadNextFrameIfLcdIsOn_hook, 0x411d, 0x442c);
  } else {
    CYC(0x4429, 0x442c);
  }
  itemLoadGraphics__afterCall442c_hook(gb);
}

void interactionGetData_hook(GB *gb) {
  CYC(0x4437, 0x4438); H = D;
  CYC(0x4438, 0x443a); L = 0x41;
  CYC(0x443a, 0x443b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x443b, 0x443c); E = mem_rd(gb, HL);
  getDataForInteraction_hook(gb);
}

void getDataForInteraction_hook(GB *gb) {
  CYC(0x443c, 0x443d); C = A;
  CYC(0x443d, 0x443f); B = 0x00;
  CYC(0x443f, 0x4442); SET_HL(0x6427);
  CYC(0x4442, 0x4443); alu_add_hl(gb, BC);
  CYC(0x4443, 0x4444); alu_add_hl(gb, BC);
  CYC(0x4444, 0x4445); alu_add_hl(gb, BC);
  CYC(0x4445, 0x4446); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x4446, 0x4447); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x4447, 0x4448);
    ret_effect(gb);
    return;
  }
  CYC(0x4447, 0x4448);
  CYC(0x4448, 0x4449); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4449, 0x444a); SET_HL(HL + 1);
  CYC(0x444a, 0x444b); H = mem_rd(gb, HL);
  CYC(0x444b, 0x444c); L = A;
  CYC(0x444c, 0x444e); C = 0x03;
  CYC(0x444e, 0x444f); A = E;
  CYC(0x444f, 0x4450); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4450, 0x4451);
    ret_effect(gb);
    return;
  }
  CYC(0x4450, 0x4451);
  for (;;) {
    CYC(0x4451, 0x4452); SET_HL(HL + 1);
    CYC(0x4452, 0x4454); alu_bit(gb, 7, mem_rd(gb, HL));
    CYC(0x4454, 0x4455); SET_HL(HL - 1);
    if (!(F & FZ)) {
      CYCT(0x4455, 0x4456);
      ret_effect(gb);
      return;
    }
    CYC(0x4455, 0x4456);
    CYC(0x4456, 0x4457); alu_add_hl(gb, BC);
    CYC(0x4457, 0x4458); A = alu_dec8(gb, A);
    if (F & FZ) {
      CYC(0x4458, 0x445a);
      break;
    }
    CYCT(0x4458, 0x445a);
  }
  CYC(0x445a, 0x445b); ret_effect(gb);
}

void loadWeaponGfx_b3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x445b, 0x445e); SET_HL(wLoadedItemGraphic1);
  CYC(0x445e, 0x445f); A = E;
  CYC(0x445f, 0x4461); alu_cp(gb, 0x1a);
  if (!(F & FC)) {
    CYCT(0x4461, 0x4463);
  } else {
    CYC(0x4461, 0x4463);
    CYC(0x4463, 0x4464); L = alu_inc8(gb, L);
  }
  CYC(0x4464, 0x4465); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4465, 0x4466);
    ret_effect(gb);
    return;
  }
  CYC(0x4465, 0x4466);
  CYC(0x4466, 0x4467); mem_wr(gb, HL, A);
  CYC(0x4467, 0x4468); push_effect(gb, DE);
  CALL_C(0x4468, loadUncompressedGfxHeader_hook, 0x05da, 0x446b);
  CYC(0x446b, 0x446c); SET_DE(pop_effect(gb));
  CYC(0x446c, 0x446d); ret_effect(gb);
}
