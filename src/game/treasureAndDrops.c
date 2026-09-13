#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

void loseTreasure_helper_hook(GB *gb);
void realignUnappraisedRings__findNextFilledSlot_hook(GB *gb);
void addTreasureToInventory__addToInventory_hook(GB *gb);
void loadTreasureDisplayData__getTableIndices_b3f_hook(GB *gb);
void checkItemDropAvailable_body_hook(GB *gb);
void giveTreasure_body__findItemInTable_hook(GB *gb);
void giveTreasure_body__giveTreasure_hook(GB *gb);
void giveTreasure_body__applyParameter_hook(GB *gb);
void giveTreasure_body__modee_hook(GB *gb);
void giveTreasure_body__modef_hook(GB *gb);
void giveTreasure_body__mode9_hook(GB *gb);
void giveTreasure_body__removeOneDuplicateRing_hook(GB *gb);

static void treasure_add_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
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

static void treasure_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t treasure_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void giveTreasure_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x44c8, 0x44c9); push_effect(gb, HL);
  CYC(0x44c9, 0x44ca); push_effect(gb, DE);
  CYC(0x44ca, 0x44cb); A = B;
  CYC(0x44cb, 0x44cd); H8(hFF8B) = A;
  CYC(0x44cd, 0x44ce); push_effect(gb, BC);
  CYC(0x44ce, 0x44d1); SET_HL(0x4547);
  CALL_C(0x44d1, giveTreasure_body__findItemInTable_hook, 0x44f0, 0x44d4);
  if (F & FZ) {
    CYCT(0x44d4, 0x44d6);
  } else {
    CYC(0x44d4, 0x44d6);
    CALL_C(0x44d6, loseTreasure_helper_hook, 0x44a8, 0x44d9);
    CYC(0x44d9, 0x44da); A = C;
    CALL_C(0x44da, loseTreasure_helper_hook, 0x44a8, 0x44dd);
  }
  CYC(0x44dd, 0x44de); SET_BC(pop_effect(gb));
  CYC(0x44de, 0x44df); A = B;
  CALL_C(0x44df, giveTreasure_body__giveTreasure_hook, 0x4501, 0x44e2);
  CYC(0x44e2, 0x44e3); push_effect(gb, AF);
  CYC(0x44e3, 0x44e6); SET_HL(0x453a);
  CALL_C(0x44e6, giveTreasure_body__findItemInTable_hook, 0x44f0, 0x44e9);
  if (!(F & FZ)) {
    CALL_C_CC(0x44e9, giveTreasure_body__giveTreasure_hook, 0x4501, 0x44ec);
  } else {
    CYC(0x44e9, 0x44ec);
  }
  CYC(0x44ec, 0x44ed); SET_BC(pop_effect(gb));
  CYC(0x44ed, 0x44ee); SET_DE(pop_effect(gb));
  CYC(0x44ee, 0x44ef); SET_HL(pop_effect(gb));
  CYC(0x44ef, 0x44f0); ret_effect(gb);
}

void giveTreasure_body__findItemInTable_hook(GB *gb) {
  CYC(0x44f0, 0x44f2); A = H8(hFF8B);
  CYC(0x44f2, 0x44f3); C = A;
  for (;;) {
    CYC(0x44f3, 0x44f4); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x44f4, 0x44f5); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(0x44f5, 0x44f7);
      break;
    }
    CYC(0x44f5, 0x44f7);
    CYC(0x44f7, 0x44f8); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x44f8, 0x44f9); ret_effect(gb); return;
    }
    CYC(0x44f8, 0x44f9);
    CYC(0x44f9, 0x44fa); SET_HL(HL + 1);
    CYC(0x44fa, 0x44fb); SET_HL(HL + 1);
    CYC(0x44fb, 0x44fd);
  }
  CYC(0x44fd, 0x44fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x44fe, 0x44ff); C = mem_rd(gb, HL);
  CYC(0x44ff, 0x4500); alu_or(gb, A);
  CYC(0x4500, 0x4501); ret_effect(gb);
}

void giveTreasure_body__giveTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4501, 0x4503); H8(hFF8B) = A;
  CALL_C(0x4503, checkIncreaseGashaMaturityForGettingTreasure_hook, 0x4ad6, 0x4506);
  CALL_C(0x4506, addTreasureToInventory_hook, 0x46b6, 0x4509);
  CYC(0x4509, 0x450c); SET_HL(wObtainedTreasureFlags);
  CYC(0x450c, 0x450e); A = H8(hFF8B);
  CALL_C(0x450e, setFlag_hook, 0x020e, 0x4511);
  CYC(0x4511, 0x4512); push_effect(gb, BC);
  CYC(0x4512, 0x4514); A = H8(hFF8B);
  CYC(0x4514, 0x4515); C = A;
  CYC(0x4515, 0x4517); B = 0x00;
  CYC(0x4517, 0x451a); SET_HL(0x6c09);
  CYC(0x451a, 0x451b); alu_add_hl(gb, BC);
  CYC(0x451b, 0x451c); alu_add_hl(gb, BC);
  CYC(0x451c, 0x451d); alu_add_hl(gb, BC);
  CYC(0x451d, 0x451e); SET_BC(pop_effect(gb));
  CYC(0x451e, 0x4520); D = 0xc6;
  CYC(0x4520, 0x4521); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4521, 0x4522); E = A;
  CYC(0x4522, 0x4523); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4523, 0x4525);
  } else {
    CYC(0x4523, 0x4525);
    CYC(0x4525, 0x4527); E = (uint8_t)wShortSecretIndex;
  }
  CYC(0x4527, 0x4528); A = mem_rd(gb, HL);
  CYC(0x4528, 0x452a); alu_and(gb, 0x0f);
  CYC(0x452a, 0x452b); push_effect(gb, HL);
  CALL_C(0x452b, giveTreasure_body__applyParameter_hook, 0x4548, 0x452e);
  CYC(0x452e, 0x452f); SET_HL(pop_effect(gb));
  CYC(0x452f, 0x4531); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x4531, 0x4532); SET_HL(HL + 1);
  CYC(0x4532, 0x4533); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(0x4533, 0x4535);
  } else {
    CYC(0x4533, 0x4535);
    CALL_C(0x4535, playSound_b00_hook, 0x0c98, 0x4538);
    CYC(0x4538, 0x4539); alu_xor(gb, A);
  }
  CYC(0x4539, 0x453a); ret_effect(gb);
}

void giveTreasure_body__applyParameter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4548, 0x4549); push_effect(gb, 0x4549);
  switch (treasure_jump_table(gb)) {
    case 0x4539: CYC(0x4539, 0x453a); ret_effect(gb); return;
    case 0x4569: giveTreasure_body__modeb_hook(gb); return;
    case 0x4570: giveTreasure_body__mode8_hook(gb); return;
    case 0x457b: giveTreasure_body__mode5_hook(gb); return;
    case 0x457e: giveTreasure_body__mode6_hook(gb); return;
    case 0x4582: giveTreasure_body__mode1_hook(gb); return;
    case 0x4588: giveTreasure_body__mode2_hook(gb); return;
    case 0x458c: giveTreasure_body__mode3_hook(gb); return;
    case 0x458e: giveTreasure_body__mode4_hook(gb); return;
    case 0x4597: giveTreasure_body__mode7_hook(gb); return;
    case 0x45a4: giveTreasure_body__modea_hook(gb); return;
    case 0x45a8: giveTreasure_body__modec_hook(gb); return;
    case 0x45bc: giveTreasure_body__moded_hook(gb); return;
    case 0x45c7: giveTreasure_body__modee_hook(gb); return;
    case 0x45fe: giveTreasure_body__modef_hook(gb); return;
    case 0x4614: giveTreasure_body__mode9_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void checkTreasureObtained_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x446d, 0x446e); A = L;
  CYC(0x446e, 0x4470); alu_cp(gb, 0x60);
  if (!(F & FC)) {
    CYCT(0x4470, 0x4472);
    CYC(0x4494, 0x4496); alu_and(gb, 0x07);
    CYC(0x4496, 0x4499); SET_HL(wUpgradesObtained);
    CALL_C(0x4499, checkFlag_hook, 0x0205, 0x449c);
    if (!(F & FZ)) {
      CYCT(0x449c, 0x449e);
      CYC(0x4491, 0x4493); H = 0x01;
      CYC(0x4493, 0x4494); ret_effect(gb); return;
    }
    CYC(0x449c, 0x449e);
    CYC(0x449e, 0x44a0); H = 0x00;
    CYC(0x44a0, 0x44a1); ret_effect(gb); return;
  }
  CYC(0x4470, 0x4472);
  CYC(0x4472, 0x4474); H8(hFF8B) = A;
  CYC(0x4474, 0x4477); SET_HL(wObtainedTreasureFlags);
  CALL_C(0x4477, checkFlag_hook, 0x0205, 0x447a);
  if (F & FZ) {
    CYCT(0x447a, 0x447c);
    CYC(0x449e, 0x44a0); H = 0x00;
    CYC(0x44a0, 0x44a1); ret_effect(gb); return;
  }
  CYC(0x447a, 0x447c);
  CYC(0x447c, 0x447d); push_effect(gb, BC);
  CYC(0x447d, 0x447f); A = H8(hFF8B);
  CYC(0x447f, 0x4480); C = A;
  CYC(0x4480, 0x4482); B = 0x00;
  CYC(0x4482, 0x4485); SET_HL(0x6c09);
  CYC(0x4485, 0x4486); alu_add_hl(gb, BC);
  CYC(0x4486, 0x4487); alu_add_hl(gb, BC);
  CYC(0x4487, 0x4488); alu_add_hl(gb, BC);
  CYC(0x4488, 0x4489); SET_BC(pop_effect(gb));
  CYC(0x4489, 0x448a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x448a, 0x448b); L = A;
  CYC(0x448b, 0x448c); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x448c, 0x448e);
  } else {
    CYC(0x448c, 0x448e);
    CYC(0x448e, 0x4490); H = 0xc6;
    CYC(0x4490, 0x4491); L = mem_rd(gb, HL);
  }
  CYC(0x4491, 0x4493); H = 0x01;
  CYC(0x4493, 0x4494); ret_effect(gb);
}

void loseTreasure_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44a1, 0x44a2); push_effect(gb, HL);
  CYC(0x44a2, 0x44a3); A = B;
  CALL_C(0x44a3, loseTreasure_helper_hook, 0x44a8, 0x44a6);
  CYC(0x44a6, 0x44a7); SET_HL(pop_effect(gb));
  CYC(0x44a7, 0x44a8); ret_effect(gb);
}

void loseTreasure_helper_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44a8, 0x44a9); B = A;
  CYC(0x44a9, 0x44ac); SET_HL(wObtainedTreasureFlags);
  CALL_C(0x44ac, unsetFlag_hook, 0x0218, 0x44af);
  CYC(0x44af, 0x44b0); A = B;
  CYC(0x44b0, 0x44b2); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(0x44b2, 0x44b3); ret_effect(gb); return;
  }
  CYC(0x44b2, 0x44b3);
  CYC(0x44b3, 0x44b6); SET_HL(wInventoryB);
  CYC(0x44b6, 0x44b8); B = 0x12;
  for (;;) {
    CYC(0x44b8, 0x44b9); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x44b9, 0x44bb);
      break;
    }
    CYC(0x44b9, 0x44bb);
    CYC(0x44bb, 0x44bc); L = alu_inc8(gb, L);
    CYC(0x44bc, 0x44bd); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x44bd, 0x44bf);
      CYC(0x44bf, 0x44c0); ret_effect(gb); return;
    }
    CYCT(0x44bd, 0x44bf);
  }
  CYC(0x44c0, 0x44c2); mem_wr(gb, HL, 0x00);
  CYC(0x44c2, 0x44c5); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x44c5, 0x44c7); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x44c7, 0x44c8); ret_effect(gb);
}

void getNumUnappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4697, 0x4698); push_effect(gb, DE);
  CYC(0x4698, 0x469b); SET_HL(wUnappraisedRings);
  CYC(0x469b, 0x469e); SET_DE(0x4000);
  do {
    CYC(0x469e, 0x469f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x469f, 0x46a1); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(0x46a1, 0x46a3);
    } else {
      CYC(0x46a1, 0x46a3);
      CYC(0x46a3, 0x46a4); E = alu_inc8(gb, E);
    }
    CYC(0x46a4, 0x46a5); D = alu_dec8(gb, D);
    if (!(F & FZ)) CYCT(0x46a5, 0x46a7);
    else CYC(0x46a5, 0x46a7);
  } while (!(F & FZ));
  CYC(0x46a7, 0x46a8); push_effect(gb, BC);
  CYC(0x46a8, 0x46a9); A = E;
  CALL_C(0x46a9, hexToDec_hook, 0x0259, 0x46ac);
  CYC(0x46ac, 0x46ae); C = alu_swap(gb, C);
  CYC(0x46ae, 0x46af); alu_or(gb, C);
  CYC(0x46af, 0x46b2); W8(wNumUnappraisedRingsBcd) = A;
  CYC(0x46b2, 0x46b3); SET_BC(pop_effect(gb));
  CYC(0x46b3, 0x46b4); B = E;
  CYC(0x46b4, 0x46b5); SET_DE(pop_effect(gb));
  CYC(0x46b5, 0x46b6); ret_effect(gb);
}

void realignUnappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x466f, 0x4672); SET_HL(wUnappraisedRings);
  for (;;) {
    CYC(0x4672, 0x4673); A = mem_rd(gb, HL);
    CYC(0x4673, 0x4675); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYC(0x4675, 0x4677);
      CYC(0x4677, 0x4678); push_effect(gb, HL);
      CALL_C(0x4678, realignUnappraisedRings__findNextFilledSlot_hook, 0x4687, 0x467b);
      CYC(0x467b, 0x467c); SET_HL(pop_effect(gb));
      if (!(F & FC)) {
        CYCT(0x467c, 0x467e);
        break;
      }
      CYC(0x467c, 0x467e);
      CYC(0x467e, 0x467f); mem_wr(gb, HL, A);
    } else {
      CYCT(0x4675, 0x4677);
    }
    CYC(0x467f, 0x4680); L = alu_inc8(gb, L);
    CYC(0x4680, 0x4681); A = L;
    CYC(0x4681, 0x4683); alu_cp(gb, (uint8_t)wUnappraisedRingsEnd);
    if (F & FZ) {
      CYC(0x4683, 0x4685);
      break;
    }
    CYCT(0x4683, 0x4685);
  }
  CYC(0x4685, 0x4687);
  getNumUnappraisedRings_hook(gb);
}

void realignUnappraisedRings__findNextFilledSlot_hook(GB *gb) {
  for (;;) {
    CYC(0x4687, 0x4688); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4688, 0x468a); alu_cp(gb, 0xff);
    if (!(F & FZ)) {
      CYCT(0x468a, 0x468c);
      break;
    }
    CYC(0x468a, 0x468c);
    CYC(0x468c, 0x468d); A = L;
    CYC(0x468d, 0x468f); alu_cp(gb, (uint8_t)wUnappraisedRingsEnd);
    if (F & FZ) {
      CYC(0x468f, 0x4691);
      CYC(0x4691, 0x4692); ret_effect(gb);
      return;
    }
    CYCT(0x468f, 0x4691);
  }
  CYC(0x4692, 0x4693); SET_HL(HL - 1);
  CYC(0x4693, 0x4695); mem_wr(gb, HL, 0xff);
  CYC(0x4695, 0x4696); alu_scf(gb);
  CYC(0x4696, 0x4697); ret_effect(gb);
}

void addTreasureToInventory_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46b6, 0x46b8); A = H8(hFF8B);
  CYC(0x46b8, 0x46ba); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(0x46ba, 0x46bb); ret_effect(gb);
    return;
  }
  CYC(0x46ba, 0x46bb);
  CYC(0x46bb, 0x46bc); push_effect(gb, BC);
  CALL_C(0x46bc, addTreasureToInventory__addToInventory_hook, 0x46dc, 0x46bf);
  CYC(0x46bf, 0x46c0); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x46c0, 0x46c1); ret_effect(gb);
    return;
  }
  CYC(0x46c0, 0x46c1);
  if (F & FZ) {
    CYCT(0x46c1, 0x46c4);
    setStatusBarNeedsRefreshBit1_hook(gb);
    return;
  }
  CYC(0x46c1, 0x46c4);
  CYC(0x46c4, 0x46c5); push_effect(gb, BC);
  CYC(0x46c5, 0x46c6); alu_cpl(gb);
  CYC(0x46c6, 0x46c8); alu_add(gb, 0x88);
  CYC(0x46c8, 0x46c9); L = A;
  CYC(0x46c9, 0x46cb); A = H8(hFF8B);
  CYC(0x46cb, 0x46cc); C = A;
  CYC(0x46cc, 0x46ce); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYC(0x46ce, 0x46d0);
    CYC(0x46d0, 0x46d1); A = mem_rd(gb, HL);
    CYC(0x46d1, 0x46d2); mem_wr(gb, HL, C);
    CALL_C(0x46d2, addTreasureToInventory__addToInventory_hook, 0x46dc, 0x46d5);
  } else {
    CYCT(0x46ce, 0x46d0);
  }
  CYC(0x46d5, 0x46d8); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x46d8, 0x46da); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x46da, 0x46db); SET_BC(pop_effect(gb));
  CYC(0x46db, 0x46dc); ret_effect(gb);
}

void addTreasureToInventory__addToInventory_hook(GB *gb) {
  CYC(0x46dc, 0x46dd); C = A;
  CYC(0x46dd, 0x46e0); SET_HL(wInventoryB);
  CYC(0x46e0, 0x46e2); B = 0x12;
  for (;;) {
    CYC(0x46e2, 0x46e3); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x46e3, 0x46e4); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(0x46e4, 0x46e6);
      break;
    }
    CYC(0x46e4, 0x46e6);
    CYC(0x46e6, 0x46e7); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x46e7, 0x46e9);
      CYC(0x46e9, 0x46ea); B = alu_dec8(gb, B);
      CYC(0x46ea, 0x46ec); L = (uint8_t)wInventoryB;
      do {
        CYC(0x46ec, 0x46ed); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x46ed, 0x46ee); alu_or(gb, A);
        if (!(F & FZ)) CYCT(0x46ee, 0x46f0);
        else CYC(0x46ee, 0x46f0);
      } while (!(F & FZ));
      break;
    }
    CYCT(0x46e7, 0x46e9);
  }
  CYC(0x46f0, 0x46f1); L = alu_dec8(gb, L);
  CYC(0x46f1, 0x46f2); mem_wr(gb, HL, C);
  CYC(0x46f2, 0x46f3); A = L;
  CYC(0x46f3, 0x46f5); alu_sub(gb, 0x8a);
  CYC(0x46f5, 0x46f7); alu_bit(gb, 7, B);
  CYC(0x46f7, 0x46f8); ret_effect(gb);
}

void loadTreasureDisplayData_b3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x46f8, 0x46f9); A = L;
  CYC(0x46f9, 0x46fa); push_effect(gb, DE);
  CALL_C(0x46fa, loadTreasureDisplayData__getTableIndices_b3f_hook, 0x472b, 0x46fd);
  CYC(0x46fd, 0x46fe); push_effect(gb, BC);
  CYC(0x46fe, 0x4701); SET_HL(0x0000);
  CYC(0x4701, 0x4702); A = D;
  CYC(0x4702, 0x4703); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4703, 0x4705);
  } else {
    CYC(0x4703, 0x4705);
    CYC(0x4705, 0x4706); alu_cpl(gb);
    CYC(0x4706, 0x4707); A = alu_inc8(gb, A);
    CYC(0x4707, 0x4708); L = A;
    CYC(0x4708, 0x470a); H = 0xff;
    CYC(0x470a, 0x470b); A = D;
    CALL_C(0x470b, multiplyABy8_hook, 0x01b7, 0x470e);
    CYC(0x470e, 0x470f); alu_add_hl(gb, BC);
  }
  CYC(0x470f, 0x4710); push_effect(gb, HL);
  CYC(0x4710, 0x4711); A = E;
  CYC(0x4711, 0x4714); SET_HL(0x6d62);
  CYC(0x4714, 0x4715); treasure_add_double_index_to_hl_from_rst(gb, 0x4715);
  CYC(0x4715, 0x4716); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4716, 0x4717); H = mem_rd(gb, HL);
  CYC(0x4717, 0x4718); L = A;
  CYC(0x4718, 0x4719); SET_BC(pop_effect(gb));
  CYC(0x4719, 0x471a); alu_add_hl(gb, BC);
  CYC(0x471a, 0x471d); SET_DE(wTmpcec0);
  CYC(0x471d, 0x471f); B = 0x07;
  do {
    CYC(0x471f, 0x4720); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4720, 0x4721); mem_wr(gb, DE, A);
    CYC(0x4721, 0x4722); E = alu_inc8(gb, E);
    CYC(0x4722, 0x4723); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x4723, 0x4725);
    else CYC(0x4723, 0x4725);
  } while (!(F & FZ));
  CYC(0x4725, 0x4728); SET_HL(wTmpcec0);
  CYC(0x4728, 0x4729); SET_BC(pop_effect(gb));
  CYC(0x4729, 0x472a); SET_DE(pop_effect(gb));
  CYC(0x472a, 0x472b); ret_effect(gb);
}

void loadTreasureDisplayData__getTableIndices_b3f_hook(GB *gb) {
  CYC(0x472b, 0x472c); D = A;
  CYC(0x472c, 0x472f); SET_HL(0x6d41);
  for (;;) {
    CYC(0x472f, 0x4730); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4730, 0x4731); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x4731, 0x4733);
      break;
    }
    CYC(0x4731, 0x4733);
    CYC(0x4733, 0x4734); alu_cp(gb, D);
    if (F & FZ) {
      CYCT(0x4734, 0x4736);
      break;
    }
    CYC(0x4734, 0x4736);
    CYC(0x4736, 0x4737); SET_HL(HL + 1);
    CYC(0x4737, 0x4738); SET_HL(HL + 1);
    CYC(0x4738, 0x473a);
  }
  CYC(0x473a, 0x473b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x473b, 0x473c); E = mem_rd(gb, HL);
  CYC(0x473c, 0x473d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x473d, 0x473f);
  } else {
    CYC(0x473d, 0x473f);
    CYC(0x473f, 0x4740); L = A;
    CYC(0x4740, 0x4742); H = 0xc6;
    CYC(0x4742, 0x4743); D = mem_rd(gb, HL);
  }
  CYC(0x4743, 0x4744); ret_effect(gb);
}

void decideItemDrop_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4744, 0x4745); A = C;
  CYC(0x4745, 0x4746); alu_or(gb, A);
  CYC(0x4746, 0x4748); A |= 0x80;
  if (!(F & FZ)) {
    CYCT(0x4748, 0x474a);
  } else {
    CYC(0x4748, 0x474a);
    CYC(0x474a, 0x474c); A = H8(hActiveObjectType);
    CYC(0x474c, 0x474e); alu_add(gb, 0x02);
    CYC(0x474e, 0x474f); E = A;
    CYC(0x474f, 0x4750); A = mem_rd(gb, DE);
  }
  CYC(0x4750, 0x4753); SET_HL(0x4a46);
  CYC(0x4753, 0x4754); treasure_add_index_to_hl_from_rst(gb, 0x4754);
  CYC(0x4754, 0x4755); A = mem_rd(gb, HL);
  CYC(0x4755, 0x4756); C = A;
  CYC(0x4756, 0x4758); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4758, 0x475a);
    goto unavailable;
  }
  CYC(0x4758, 0x475a);
  CYC(0x475a, 0x475c); A = alu_swap(gb, A);
  CYC(0x475c, 0x475d); alu_rrca(gb);
  CYC(0x475d, 0x475f); alu_and(gb, 0x07);
  CYC(0x475f, 0x4762); SET_HL(0x47fe);
  CYC(0x4762, 0x4763); treasure_add_double_index_to_hl_from_rst(gb, 0x4763);
  CYC(0x4763, 0x4764); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4764, 0x4765); H = mem_rd(gb, HL);
  CYC(0x4765, 0x4766); L = A;
  CALL_C(0x4766, getRandomNumber_hook, 0x043e, 0x4769);
  CYC(0x4769, 0x476b); alu_and(gb, 0x3f);
  CALL_C(0x476b, checkFlag_hook, 0x0205, 0x476e);
  if (F & FZ) {
    CYCT(0x476e, 0x4770);
    goto unavailable;
  }
  CYC(0x476e, 0x4770);
  CYC(0x4770, 0x4771); A = C;
  CYC(0x4771, 0x4773); alu_and(gb, 0x1f);
  CYC(0x4773, 0x4776); SET_HL(0x47be);
  CYC(0x4776, 0x4777); treasure_add_double_index_to_hl_from_rst(gb, 0x4777);
  CYC(0x4777, 0x4778); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4778, 0x4779); H = mem_rd(gb, HL);
  CYC(0x4779, 0x477a); L = A;
  CALL_C(0x477a, getRandomNumber_hook, 0x043e, 0x477d);
  CYC(0x477d, 0x477f); alu_and(gb, 0x1f);
  CYC(0x477f, 0x4780); treasure_add_index_to_hl_from_rst(gb, 0x4780);
  CYC(0x4780, 0x4781); A = mem_rd(gb, HL);
  CYC(0x4781, 0x4782); C = A;
  checkItemDropAvailable_body_hook(gb);
  return;

unavailable:
  CYC(0x478f, 0x4791); C = 0xff;
  CYC(0x4791, 0x4792); ret_effect(gb);
}

void checkItemDropAvailable_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4782, 0x4783); A = C;
  CYC(0x4783, 0x4786); SET_HL(0x47de);
  CYC(0x4786, 0x4787); treasure_add_double_index_to_hl_from_rst(gb, 0x4787);
  CYC(0x4787, 0x4788); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4788, 0x4789); B = mem_rd(gb, HL);
  CYC(0x4789, 0x478a); L = A;
  CYC(0x478a, 0x478c); H = 0xc6;
  CYC(0x478c, 0x478d); A = mem_rd(gb, HL);
  CYC(0x478d, 0x478e); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(0x478e, 0x478f); ret_effect(gb); return;
  }
  CYC(0x478e, 0x478f);
  CYC(0x478f, 0x4791); C = 0xff;
  CYC(0x4791, 0x4792); ret_effect(gb);
}

void giveTreasure_body__modeb_hook(GB *gb) {
  CYC(0x4569, 0x456a); A = C;
  CYC(0x456a, 0x456d); SET_HL(wUpgradesObtained);
  CYC(0x456d, 0x4570); setFlag_hook(gb);
}

void giveTreasure_body__mode8_hook(GB *gb) {
  CYC(0x4570, 0x4571); A = mem_rd(gb, DE);
  CYC(0x4571, 0x4572); alu_cp(gb, C);
  if (!(F & FC)) {
    CYCT(0x4572, 0x4573); ret_effect(gb); return;
  }
  CYC(0x4572, 0x4573);
  CYC(0x4573, 0x4574); A = C;
  CYC(0x4574, 0x4575); mem_wr(gb, DE, A);
  CYC(0x4575, 0x4578); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x4578, 0x457a); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x457a, 0x457b); ret_effect(gb);
}

void giveTreasure_body__mode5_hook(GB *gb) {
  CYC(0x457b, 0x457c); A = C;
  CYC(0x457c, 0x457d); mem_wr(gb, DE, A);
  CYC(0x457d, 0x457e); ret_effect(gb);
}

void giveTreasure_body__mode1_hook(GB *gb) {
  CYC(0x4582, 0x4583); A = C;
  CYC(0x4583, 0x4584); H = D;
  CYC(0x4584, 0x4585); L = E;
  CYC(0x4585, 0x4588); setFlag_hook(gb);
}

void giveTreasure_body__mode6_hook(GB *gb) {
  CYC(0x457e, 0x4581); A = W8(wDungeonIndex);
  CYC(0x4581, 0x4582); C = A;
  giveTreasure_body__mode1_hook(gb);
}

void giveTreasure_body__mode2_hook(GB *gb) {
  CYC(0x4588, 0x4589); A = mem_rd(gb, DE);
  CYC(0x4589, 0x458a); A = alu_inc8(gb, A);
  CYC(0x458a, 0x458b); mem_wr(gb, DE, A);
  CYC(0x458b, 0x458c); ret_effect(gb);
}

void giveTreasure_body__mode4_hook(GB *gb) {
  CYC(0x458e, 0x458f); A = mem_rd(gb, DE);
  CYC(0x458f, 0x4590); alu_add(gb, C);
  CYC(0x4590, 0x4591); alu_daa(gb);
  if (!(F & FC)) {
    CYCT(0x4591, 0x4593);
  } else {
    CYC(0x4591, 0x4593);
    CYC(0x4593, 0x4595); A = 0x99;
  }
  CYC(0x4595, 0x4596); mem_wr(gb, DE, A);
  CYC(0x4596, 0x4597); ret_effect(gb);
}

void giveTreasure_body__mode3_hook(GB *gb) {
  CYC(0x458c, 0x458e); C = 0x01;
  giveTreasure_body__mode4_hook(gb);
}

void giveTreasure_body__mode7_hook(GB *gb) {
  CYC(0x4597, 0x459a); A = W8(wDungeonIndex);
  CYC(0x459a, 0x459b); alu_add(gb, E);
  CYC(0x459b, 0x459c); L = A;
  CYC(0x459c, 0x459d); H = D;
  CYC(0x459d, 0x459e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x459e, 0x45a1); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x45a1, 0x45a3); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(0x45a3, 0x45a4); ret_effect(gb);
}

void giveTreasure_body__modea_hook(GB *gb) {
  CYC(0x45a4, 0x45a5); A = mem_rd(gb, DE);
  CYC(0x45a5, 0x45a6); alu_add(gb, C);
  CYC(0x45a6, 0x45a7); mem_wr(gb, DE, A);
  CYC(0x45a7, 0x45a8); ret_effect(gb);
}

void giveTreasure_body__modec_hook(GB *gb) {
  CYC(0x45a8, 0x45a9); H = D;
  CYC(0x45a9, 0x45aa); L = E;
  CYC(0x45aa, 0x45ac); A = (uint8_t)wLinkHealth;
  CYC(0x45ac, 0x45ad); alu_cp(gb, E);
  CYC(0x45ad, 0x45ae); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(0x45ae, 0x45b0);
  } else {
    CYC(0x45ae, 0x45b0);
    CYC(0x45b0, 0x45b1); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYC(0x45b1, 0x45b3);
      CYC(0x45b3, 0x45b5); A = 0x57;
      CYC(0x45b5, 0x45b8); playSound_b00_hook(gb);
      return;
    }
    CYCT(0x45b1, 0x45b3);
  }
  CYC(0x45b8, 0x45b9); alu_add(gb, C);
  CYC(0x45b9, 0x45ba); mem_wr(gb, DE, A);
  CYC(0x45ba, 0x45bc);
  CYC(0x45c2, 0x45c3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x45c3, 0x45c4); ret_effect(gb); return;
  }
  CYC(0x45c3, 0x45c4);
  CYC(0x45c4, 0x45c5); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x45c5, 0x45c6); mem_wr(gb, HL, A);
  CYC(0x45c6, 0x45c7); ret_effect(gb);
}

void giveTreasure_body__moded_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x45bc, giveTreasure_body__mode4_hook, 0x458e, 0x45bf);
  CYC(0x45bf, 0x45c0); H = D;
  CYC(0x45c0, 0x45c1); L = E;
  CYC(0x45c1, 0x45c2); L = alu_inc8(gb, L);
  CYC(0x45c2, 0x45c3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x45c3, 0x45c4); ret_effect(gb); return;
  }
  CYC(0x45c3, 0x45c4);
  CYC(0x45c4, 0x45c5); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x45c5, 0x45c6); mem_wr(gb, HL, A);
  CYC(0x45c6, 0x45c7); ret_effect(gb);
}

void giveTreasure_body__modee_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x45c7, 0x45c8); A = C;
  CALL_C(0x45c8, getRupeeValue_hook, 0x1781, 0x45cb);
  CYC(0x45cb, 0x45cc); A = E;
  CYC(0x45cc, 0x45ce); alu_cp(gb, (uint8_t)wNumRupees);
  if (!(F & FZ)) {
    CYCT(0x45ce, 0x45d0);
  } else {
    CYC(0x45ce, 0x45d0);
    CYC(0x45d0, 0x45d2); A = 0x01;
    CALL_C(0x45d2, checkGlobalFlag_hook, 0x31f3, 0x45d5);
    if (!(F & FZ)) {
      CYCT(0x45d5, 0x45d7);
    } else {
      CYC(0x45d5, 0x45d7);
      CYC(0x45d7, 0x45d8); H = D;
      CYC(0x45d8, 0x45da); L = (uint8_t)wTotalRupeesCollected;
      CALL_C(0x45da, addDecimalToHlRef_hook, 0x0181, 0x45dd);
      if (!(F & FC)) {
        CYCT(0x45dd, 0x45df);
      } else {
        CYC(0x45dd, 0x45df);
        CYC(0x45df, 0x45e1); A = 0x01;
        CALL_C(0x45e1, setGlobalFlag_hook, 0x31f9, 0x45e4);
      }
    }
  }
  CYC(0x45e4, 0x45e5); H = D;
  CYC(0x45e5, 0x45e6); L = E;
  CALL_C(0x45e6, addDecimalToHlRef_hook, 0x0181, 0x45e9);
  CYC(0x45e9, 0x45ea); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x45ea, 0x45eb); H = mem_rd(gb, HL);
  CYC(0x45eb, 0x45ec); L = A;
  CYC(0x45ec, 0x45ef); SET_BC(0x0999);
  CALL_C(0x45ef, compareHlToBc_hook, 0x01d6, 0x45f2);
  CYC(0x45f2, 0x45f3); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x45f3, 0x45f4); ret_effect(gb); return;
  }
  CYC(0x45f3, 0x45f4);
  CYC(0x45f4, 0x45f5); A = C;
  CYC(0x45f5, 0x45f6); mem_wr(gb, DE, A);
  CYC(0x45f6, 0x45f7); E = alu_inc8(gb, E);
  CYC(0x45f7, 0x45f8); A = B;
  CYC(0x45f8, 0x45f9); mem_wr(gb, DE, A);
  CYC(0x45f9, 0x45fb); A = 0x61;
  CYC(0x45fb, 0x45fe); playSound_b00_hook(gb);
}

void giveTreasure_body__modef_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x45fe, giveTreasure_body__mode4_hook, 0x458e, 0x4601);
  CALL_C(0x4601, setStatusBarNeedsRefreshBit1_hook, 0x17d8, 0x4604);
  CYC(0x4604, 0x4607); A = W8(wSeedSatchelLevel);
  CYC(0x4607, 0x460a); SET_HL(0x4610);
  CYC(0x460a, 0x460b); treasure_add_index_to_hl_from_rst(gb, 0x460b);
  CYC(0x460b, 0x460c); A = mem_rd(gb, DE);
  CYC(0x460c, 0x460d); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(0x460d, 0x460e); ret_effect(gb); return;
  }
  CYC(0x460d, 0x460e);
  CYC(0x460e, 0x460f); A = mem_rd(gb, HL);
  CYC(0x460f, 0x4610); mem_wr(gb, DE, A);
  CYC(0x4610, 0x4611); ret_effect(gb);
}

void giveTreasure_body__mode9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4614, 0x4616); C |= 0x40;
  CALL_C(0x4616, realignUnappraisedRings_hook, 0x466f, 0x4619);
  CYC(0x4619, 0x461b); alu_cp(gb, 0x64);
  if (F & FC) {
    CYCT(0x461b, 0x461d);
  } else {
    CYC(0x461b, 0x461d);
    CALL_C(0x461d, giveTreasure_body__removeOneDuplicateRing_hook, 0x4629, 0x4620);
    CALL_C(0x4620, realignUnappraisedRings_hook, 0x466f, 0x4623);
  }
  CYC(0x4623, 0x4624); A = C;
  CYC(0x4624, 0x4627); mem_wr(gb, wUnappraisedRingsEnd - 1, A);
  CYC(0x4627, 0x4629); realignUnappraisedRings_hook(gb);
}

void giveTreasure_body__removeOneDuplicateRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4629, 0x462b); A = hram_rd(gb, R_SVBK);
  CYC(0x462b, 0x462c); push_effect(gb, AF);
  CYC(0x462c, 0x462e); A = 0x04;
  CYC(0x462e, 0x4630); hram_wr(gb, R_SVBK, A);
  CYC(0x4630, 0x4633); SET_HL(w4TmpRingBuffer);
  CYC(0x4633, 0x4635); B = 0x40;
  CALL_C(0x4635, clearMemory_hook, 0x046f, 0x4638);
  CYC(0x4638, 0x463b); SET_DE(wUnappraisedRings);
  CYC(0x463b, 0x463d); B = 0x40;
  do {
    CYC(0x463d, 0x463e); A = mem_rd(gb, DE);
    CYC(0x463e, 0x4640); alu_and(gb, 0x3f);
    CYC(0x4640, 0x4643); SET_HL(w4TmpRingBuffer);
    CYC(0x4643, 0x4644); treasure_add_index_to_hl_from_rst(gb, 0x4644);
    CYC(0x4644, 0x4645); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(0x4645, 0x4646); E = alu_inc8(gb, E);
    CYC(0x4646, 0x4647); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x4647, 0x4649);
    else CYC(0x4647, 0x4649);
  } while (!(F & FZ));
  CYC(0x4649, 0x464c); SET_HL(w4TmpRingBuffer);
  CYC(0x464c, 0x464f); SET_DE(0x0000);
  CYC(0x464f, 0x4651); B = 0x40;
  do {
    CYC(0x4651, 0x4652); A = mem_rd(gb, HL);
    CYC(0x4652, 0x4653); alu_cp(gb, D);
    if (F & FC) {
      CYCT(0x4653, 0x4655);
    } else {
      CYC(0x4653, 0x4655);
      CYC(0x4655, 0x4656); D = A;
      CYC(0x4656, 0x4657); E = L;
    }
    CYC(0x4657, 0x4658); L = alu_inc8(gb, L);
    CYC(0x4658, 0x4659); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(0x4659, 0x465b);
    else CYC(0x4659, 0x465b);
  } while (!(F & FZ));
  CYC(0x465b, 0x465c); A = E;
  CYC(0x465c, 0x465e); alu_sub(gb, (uint8_t)w4TmpRingBuffer);
  CYC(0x465e, 0x4660); alu_or(gb, 0x40);
  CYC(0x4660, 0x4661); E = A;
  CYC(0x4661, 0x4662); SET_AF(pop_effect(gb));
  CYC(0x4662, 0x4664); hram_wr(gb, R_SVBK, A);
  CYC(0x4664, 0x4667); SET_HL(wUnappraisedRingsEnd - 1);
  do {
    CYC(0x4667, 0x4668); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(0x4668, 0x4669); alu_cp(gb, E);
    if (!(F & FZ)) CYCT(0x4669, 0x466b);
    else CYC(0x4669, 0x466b);
  } while (!(F & FZ));
  CYC(0x466b, 0x466c); SET_HL(HL + 1);
  CYC(0x466c, 0x466e); mem_wr(gb, HL, 0xff);
  CYC(0x466e, 0x466f); ret_effect(gb);
}

void checkIncreaseGashaMaturityForGettingTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ad6, 0x4ad7); push_effect(gb, BC);
  CYC(0x4ad7, 0x4ad8); B = A;
  CYC(0x4ad8, 0x4adb); SET_HL(0x4aed);
  for (;;) {
    CYC(0x4adb, 0x4adc); SET_HL(HL + 1);
    CYC(0x4adc, 0x4add); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4add, 0x4ade); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x4ade, 0x4ae0);
      break;
    }
    CYC(0x4ade, 0x4ae0);
    CYC(0x4ae0, 0x4ae1); alu_cp(gb, B);
    if (!(F & FZ)) {
      CYCT(0x4ae1, 0x4ae3);
      continue;
    }
    CYC(0x4ae1, 0x4ae3);
    CYC(0x4ae3, 0x4ae5); alu_cp(gb, 0x29);
    CYC(0x4ae5, 0x4ae6); A = C;
    if (F & FZ) {
      CYCT(0x4ae6, 0x4ae8);
    } else {
      CYC(0x4ae6, 0x4ae8);
      CYC(0x4ae8, 0x4ae9); A = mem_rd(gb, HL);
    }
    CALL_C(0x4ae9, addToGashaMaturity_hook, 0x1821, 0x4aec);
    break;
  }
  CYC(0x4aec, 0x4aed); SET_BC(pop_effect(gb));
  CYC(0x4aed, 0x4aee); ret_effect(gb);
}
