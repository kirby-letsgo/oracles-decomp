#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void cutscene02__func_7c86_hook(GB *gb);
void cutscene02__func_7c8e_hook(GB *gb);
void cutscene02__handleRaisingFloorsCutscene_hook(GB *gb);
void cutscene02__state0_hook(GB *gb);
void cutscene02__state1_hook(GB *gb);
void cutscene02__state2_hook(GB *gb);
void cutscene02__func_7ced_hook(GB *gb);
void cutscene02__loopEnd_hook(GB *gb);

static void cutscenes2_jump_table_from_rst(GB *gb, uint16_t return_address) {
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

static void cutscene02_increment_state(GB *gb) {
  CYC(0x7cb8, 0x7cbb); SET_HL(wCutsceneState);
  CYC(0x7cbb, 0x7cbc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7cbc, 0x7cbd); ret_effect(gb);
}

static void cutscene02_loop_end_body(GB *gb, uint16_t sp0_) {
  CYC(0x7d2d, 0x7d30); SET_HL(w3RoomLayoutBuffer + 0xaf);
  CYC(0x7d30, 0x7d33); SET_DE(wRoomLayout + 0xaf);
  for (;;) {
    CYC(0x7d33, 0x7d34); A = mem_rd(gb, HL);
    CYC(0x7d34, 0x7d36); B = 0x00;
    CYC(0x7d36, 0x7d38); alu_cp(gb, 0x0e);
    if (F & FZ) {
      CYCT(0x7d38, 0x7d3a);
      goto replace_tile;
    }
    CYC(0x7d38, 0x7d3a);
    CYC(0x7d3a, 0x7d3b); B = alu_inc8(gb, B);
    CYC(0x7d3b, 0x7d3d); alu_cp(gb, 0x0f);
    if (F & FZ) {
      CYCT(0x7d3d, 0x7d3f);
      goto replace_tile;
    }
    CYC(0x7d3d, 0x7d3f);
    CYC(0x7d3f, 0x7d40); B = alu_inc8(gb, B);
    CYC(0x7d40, 0x7d42); alu_cp(gb, 0x28);
    if (F & FZ) {
      CYCT(0x7d42, 0x7d44);
      goto replace_tile;
    }
    CYC(0x7d42, 0x7d44);
    CYC(0x7d44, 0x7d45); B = alu_inc8(gb, B);
    CYC(0x7d45, 0x7d47); alu_cp(gb, 0x29);
    if (F & FZ) {
      CYCT(0x7d47, 0x7d49);
      goto replace_tile;
    }
    CYC(0x7d47, 0x7d49);
continue_loop:
    CYC(0x7d49, 0x7d4a); E = alu_dec8(gb, E);
    CYC(0x7d4a, 0x7d4b); L = alu_dec8(gb, L);
    if (!(F & FZ)) {
      CYCT(0x7d4b, 0x7d4d);
      continue;
    }
    CYC(0x7d4b, 0x7d4d);
    CYC(0x7d4d, 0x7d4e); alu_xor(gb, A);
    CYC(0x7d4e, 0x7d50); hram_wr(gb, 0x70, A);
    CYC(0x7d50, 0x7d51); ret_effect(gb);
    return;
replace_tile:
    CYC(0x7d51, 0x7d52); A = B;
    CYC(0x7d52, 0x7d55); SET_BC(0x7d63);
    CALL_C(0x7d55, addDoubleIndexToBc_hook, 0x007e, 0x7d58);
    CYC(0x7d58, 0x7d59); A = mem_rd(gb, BC);
    CYC(0x7d59, 0x7d5a); SET_BC(BC + 1);
    CYC(0x7d5a, 0x7d5b); mem_wr(gb, DE, A);
    CYC(0x7d5b, 0x7d5c); mem_wr(gb, HL, A);
    CYC(0x7d5c, 0x7d5d); A = mem_rd(gb, BC);
    CYC(0x7d5d, 0x7d5e); SET_BC(BC + 1);
    CYC(0x7d5e, 0x7d5f); D = alu_dec8(gb, D);
    CYC(0x7d5f, 0x7d60); mem_wr(gb, DE, A);
    CYC(0x7d60, 0x7d61); D = alu_inc8(gb, D);
    CYC(0x7d61, 0x7d63);
    goto continue_loop;
  }
}

void cutscene02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7c80, cutscene02__handleRaisingFloorsCutscene_hook, 0x7c93, 0x7c83);
  CYC(0x7c83, 0x7c86); updateAllObjects_hook(gb);
}

void cutscene02__func_7c86_hook(GB *gb) {
  CYC(0x7c86, 0x7c89); SET_HL(wTmpcbb4);
  CYC(0x7c89, 0x7c8a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x7c8a, 0x7c8b); ret_effect(gb);
    return;
  }
  CYC(0x7c8a, 0x7c8b);
  CYC(0x7c8b, 0x7c8d); mem_wr(gb, HL, 0x1e);
  CYC(0x7c8d, 0x7c8e); ret_effect(gb);
}

void cutscene02__func_7c8e_hook(GB *gb) {
  CYC(0x7c8e, 0x7c91); SET_HL(wTmpcbb3);
  CYC(0x7c91, 0x7c92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c92, 0x7c93); ret_effect(gb);
}

void cutscene02__handleRaisingFloorsCutscene_hook(GB *gb) {
  CYC(0x7c93, 0x7c96); A = mem_rd(gb, wCutsceneState);
  CYC(0x7c96, 0x7c97); cutscenes2_jump_table_from_rst(gb, 0x7c97);
  switch (HL) {
    case 0x7c9d: cutscene02__state0_hook(gb); return;
    case 0x7cbd: cutscene02__state1_hook(gb); return;
    case 0x7cc9: cutscene02__state2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void cutscene02__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c9d, 0x7ca0); SET_HL(wTmpcbb3);
  CYC(0x7ca0, 0x7ca2); B = 0x10;
  CALL_C(0x7ca2, clearMemory_hook, 0x046f, 0x7ca5);
  CYC(0x7ca5, 0x7ca8); A = mem_rd(gb, wDisabledObjects);
  CYC(0x7ca8, 0x7cab); mem_wr(gb, wTmpcbb7, A);
  CYC(0x7cab, 0x7cad); A = 0xff;
  CYC(0x7cad, 0x7cb0); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7cb0, 0x7cb3); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(0x7cb3, 0x7cb5); A = 0x06;
  CYC(0x7cb5, 0x7cb8); mem_wr(gb, wTmpcbb4, A);
  cutscene02_increment_state(gb);
}

void cutscene02__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7cbd, 0x7cbf); A = 0x70;
  CALL_C(0x7cbf, playSound_b00_hook, 0x0c98, 0x7cc2);
  CYC(0x7cc2, 0x7cc4); A = 0x3e;
  CALL_C(0x7cc4, loadUncompressedGfxHeader_hook, 0x05da, 0x7cc7);
  CYC(0x7cc7, 0x7cc9);
  cutscene02_increment_state(gb);
}

void cutscene02__state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7cc9, cutscene02__func_7c86_hook, 0x7c86, 0x7ccc);
  if (!(F & FZ)) {
    CYCT(0x7ccc, 0x7ccd); ret_effect(gb);
    return;
  }
  CYC(0x7ccc, 0x7ccd);
  CALL_C(0x7ccd, cutscene02__func_7ced_hook, 0x7ced, 0x7cd0);
  CYC(0x7cd0, 0x7cd3); SET_HL(0x7a77);
  CYC(0x7cd3, 0x7cd5); E = 0x02;
  CALL_C(0x7cd5, interBankCall_hook, 0x008a, 0x7cd8);
  CYC(0x7cd8, 0x7cd9); alu_xor(gb, A);
  CYC(0x7cd9, 0x7cdc); mem_wr(gb, wCutsceneState, A);
  CYC(0x7cdc, 0x7cdf); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(0x7cdf, 0x7ce2); A = mem_rd(gb, wTmpcbb7);
  CYC(0x7ce2, 0x7ce5); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7ce5, 0x7ce7); A = 0x01;
  CYC(0x7ce7, 0x7cea); mem_wr(gb, 0xc2ef, A);
  CYC(0x7cea, 0x7ced); updateLastToggleBlocksState_hook(gb);
}

void cutscene02__func_7ced_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7ced, 0x7cef); A = 0x03;
  CYC(0x7cef, 0x7cf1); hram_wr(gb, 0x70, A);
  CYC(0x7cf1, 0x7cf3); A = 0x10;
  CALL_C(0x7cf3, findTileInRoom_hook, 0x15cc, 0x7cf6);
  if (!(F & FZ)) {
    CYCT(0x7cf6, 0x7cf8);
    cutscene02__loopEnd_hook(gb);
    return;
  }
  CYC(0x7cf6, 0x7cf8);
  for (;;) {
    CYC(0x7cf8, 0x7cfa); H = 0xdf;
    CYC(0x7cfa, 0x7cfb); A = mem_rd(gb, HL);
    CYC(0x7cfb, 0x7cfd); alu_sub(gb, 0x28);
    CYC(0x7cfd, 0x7cff); alu_cp(gb, 0x02);
    if (!(F & FC)) {
      CYCT(0x7cff, 0x7d01);
      goto search_previous;
    }
    CYC(0x7cff, 0x7d01);
    CYC(0x7d01, 0x7d02); A = mem_rd(gb, HL);
    CYC(0x7d02, 0x7d04); alu_sub(gb, 0x28);
    CYC(0x7d04, 0x7d06); alu_add(gb, 0x0e);
    CYC(0x7d06, 0x7d07); C = L;
    CYC(0x7d07, 0x7d08); push_effect(gb, HL);
    CALL_C(0x7d08, setTile_hook, 0x3a9c, 0x7d0b);
    CYC(0x7d0b, 0x7d0c); alu_xor(gb, A);
    CYC(0x7d0c, 0x7d0e); hram_wr(gb, 0x70, A);
    CALL_C(0x7d0e, getFreeInteractionSlot_hook, 0x3aef, 0x7d11);
    if (!(F & FZ)) {
      CYCT(0x7d11, 0x7d13);
      goto restore_tile_pointer;
    }
    CYC(0x7d11, 0x7d13);
    CYC(0x7d13, 0x7d15); mem_wr(gb, HL, 0x06);
    CYC(0x7d15, 0x7d17); L = 0x4b;
    CALL_C(0x7d17, setShortPosition_paramC_hook, 0x20b9, 0x7d1a);
restore_tile_pointer:
    CYC(0x7d1a, 0x7d1b); SET_HL(pop_effect(gb));
    CYC(0x7d1b, 0x7d1d); A = 0x03;
    CYC(0x7d1d, 0x7d1f); hram_wr(gb, 0x70, A);
    CYC(0x7d1f, 0x7d21); B = 0xcf;
    CYC(0x7d21, 0x7d22); A = mem_rd(gb, HL);
    CYC(0x7d22, 0x7d23); mem_wr(gb, BC, A);
search_previous:
    CYC(0x7d23, 0x7d25); H = 0xcf;
    CYC(0x7d25, 0x7d26); L = alu_dec8(gb, L);
    CYC(0x7d26, 0x7d28); A = 0x10;
    CALL_C(0x7d28, backwardsSearch_hook, 0x15d0, 0x7d2b);
    if (F & FZ) {
      CYCT(0x7d2b, 0x7d2d);
      continue;
    }
    CYC(0x7d2b, 0x7d2d);
    cutscene02_loop_end_body(gb, sp0_);
    return;
  }
}

void cutscene02__loopEnd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  cutscene02_loop_end_body(gb, sp0_);
}

void cutscene0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d6b, 0x7d6e); SET_HL(0x701d);
  CYC(0x7d6e, 0x7d70); E = 0x03;
  CALL_C(0x7d70, interBankCall_hook, 0x008a, 0x7d73);
  CYC(0x7d73, 0x7d76);
  updateAllObjects_hook(gb);
}

void cutscene1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d76, 0x7d79); SET_HL(0x7168);
  CYC(0x7d79, 0x7d7b); E = 0x03;
  CALL_C(0x7d7b, interBankCall_hook, 0x008a, 0x7d7e);
  CYC(0x7d7e, 0x7d81);
  updateAllObjects_hook(gb);
}

void cutscene1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d81, 0x7d83); A = hram_rd(gb, 0x70);
  CYC(0x7d83, 0x7d84); push_effect(gb, AF);
  CYC(0x7d84, 0x7d87); SET_HL(0x7244);
  CYC(0x7d87, 0x7d89); E = 0x03;
  CALL_C(0x7d89, interBankCall_hook, 0x008a, 0x7d8c);
  CYC(0x7d8c, 0x7d8d); SET_AF(pop_effect(gb));
  CYC(0x7d8d, 0x7d8f); hram_wr(gb, 0x70, A);
  CYC(0x7d8f, 0x7d92);
  updateAllObjects_hook(gb);
}

void warpToMoblinKeepUnderground_hook(GB *gb) {
  CYC(0x7d92, 0x7d95); SET_HL(0x7d98);
  CYC(0x7d95, 0x7d98);
  setWarpDestVariables_hook(gb);
}

void cutscene1c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7d9d, 0x7da0); SET_HL(0x7493);
  CYC(0x7da0, 0x7da2); E = 0x03;
  CALL_C(0x7da2, interBankCall_hook, 0x008a, 0x7da5);
  CALL_C(0x7da5, updateAllObjects_hook, 0x345b, 0x7da8);
  CYC(0x7da8, 0x7dab);
  updateStatusBar_hook(gb);
}

void cutscene1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7dab, 0x7dae); SET_HL(0x7565);
  CYC(0x7dae, 0x7db0); E = 0x03;
  CALL_C(0x7db0, interBankCall_hook, 0x008a, 0x7db3);
  CYC(0x7db3, 0x7db6); SET_HL(0x6282);
  CYC(0x7db6, 0x7db8); E = 0x01;
  CALL_C(0x7db8, interBankCall_hook, 0x008a, 0x7dbb);
  CYC(0x7dbb, 0x7dbe);
  updateAllObjects_hook(gb);
}

void cutscene1e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7dbe, 0x7dc1); SET_HL(0x7619);
  CYC(0x7dc1, 0x7dc3); E = 0x03;
  CALL_C(0x7dc3, interBankCall_hook, 0x008a, 0x7dc6);
  CALL_C(0x7dc6, updateStatusBar_hook, 0x1a9c, 0x7dc9);
  CYC(0x7dc9, 0x7dcc);
  updateAllObjects_hook(gb);
}
