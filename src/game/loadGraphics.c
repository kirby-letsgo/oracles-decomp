#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

void getAddressOfLoadedObjectGfxIndex_hook(GB *gb);
void incLoadedObjectGfxIndex_hook(GB *gb);
void getObjectGfxIndexForEnemy_hook(GB *gb);

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
