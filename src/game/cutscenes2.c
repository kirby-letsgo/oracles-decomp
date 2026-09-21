#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(cutscene02);
  CYC(b_+56, b_+59); SET_HL(wCutsceneState);
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+61); ret_effect(gb);
}

static void cutscene02_loop_end_body(GB *gb, uint16_t sp0_) {
  BASE(cutscene02);
  CYC(b_+173, b_+176); SET_HL(w3RoomLayoutBuffer + 0xaf);
  CYC(b_+176, b_+179); SET_DE(wRoomLayout + 0xaf);
  for (;;) {
    CYC(b_+179, b_+180); A = mem_rd(gb, HL);
    CYC(b_+180, b_+182); B = 0x00;
    CYC(b_+182, b_+184); alu_cp(gb, 0x0e);
    if (F & FZ) {
      CYCT(b_+184, b_+186);
      goto replace_tile;
    }
    CYC(b_+184, b_+186);
    CYC(b_+186, b_+187); B = alu_inc8(gb, B);
    CYC(b_+187, b_+189); alu_cp(gb, 0x0f);
    if (F & FZ) {
      CYCT(b_+189, b_+191);
      goto replace_tile;
    }
    CYC(b_+189, b_+191);
    CYC(b_+191, b_+192); B = alu_inc8(gb, B);
    CYC(b_+192, b_+194); alu_cp(gb, 0x28);
    if (F & FZ) {
      CYCT(b_+194, b_+196);
      goto replace_tile;
    }
    CYC(b_+194, b_+196);
    CYC(b_+196, b_+197); B = alu_inc8(gb, B);
    CYC(b_+197, b_+199); alu_cp(gb, 0x29);
    if (F & FZ) {
      CYCT(b_+199, b_+201);
      goto replace_tile;
    }
    CYC(b_+199, b_+201);
continue_loop:
    CYC(b_+201, b_+202); E = alu_dec8(gb, E);
    CYC(b_+202, b_+203); L = alu_dec8(gb, L);
    if (!(F & FZ)) {
      CYCT(b_+203, b_+205);
      continue;
    }
    CYC(b_+203, b_+205);
    CYC(b_+205, b_+206); alu_xor(gb, A);
    CYC(b_+206, b_+208); hram_wr(gb, 0x70, A);
    CYC(b_+208, b_+209); ret_effect(gb);
    return;
replace_tile:
    CYC(b_+209, b_+210); A = B;
    CYC(b_+210, b_+213); SET_BC(b_+227);
    CALL_C(b_+213, addDoubleIndexToBc_hook, 0x007e, b_+216);
    CYC(b_+216, b_+217); A = mem_rd(gb, BC);
    CYC(b_+217, b_+218); SET_BC(BC + 1);
    CYC(b_+218, b_+219); mem_wr(gb, DE, A);
    CYC(b_+219, b_+220); mem_wr(gb, HL, A);
    CYC(b_+220, b_+221); A = mem_rd(gb, BC);
    CYC(b_+221, b_+222); SET_BC(BC + 1);
    CYC(b_+222, b_+223); D = alu_dec8(gb, D);
    CYC(b_+223, b_+224); mem_wr(gb, DE, A);
    CYC(b_+224, b_+225); D = alu_inc8(gb, D);
    CYC(b_+225, b_+227);
    goto continue_loop;
  }
}

void cutscene02_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, cutscene02__handleRaisingFloorsCutscene_hook, b_+19, b_+3);
  CYC(b_+3, b_+6); updateAllObjects_hook(gb);
}

void cutscene02__func_7c86_hook(GB *gb) {
  BASE(cutscene02);
  CYC(b_+6, b_+9); SET_HL(wTmpcbb4);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x1e);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void cutscene02__func_7c8e_hook(GB *gb) {
  BASE(cutscene02);
  CYC(b_+14, b_+17); SET_HL(wTmpcbb3);
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+18, b_+19); ret_effect(gb);
}

void cutscene02__handleRaisingFloorsCutscene_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+19, b_+22); A = mem_rd(gb, wCutsceneState);
  CYC(b_+22, b_+23); cutscenes2_jump_table_from_rst(gb, b_+23);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+29) { cutscene02__state0_hook(gb); return; }
    else if (jt_ == b_+61) { cutscene02__state1_hook(gb); return; }
    else if (jt_ == b_+73) { cutscene02__state2_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void cutscene02__state0_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+29, b_+32); SET_HL(wTmpcbb3);
  CYC(b_+32, b_+34); B = 0x10;
  CALL_C(b_+34, clearMemory_hook, SYM(clearMemory), b_+37);
  CYC(b_+37, b_+40); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+40, b_+43); mem_wr(gb, wTmpcbb7, A);
  CYC(b_+43, b_+45); A = 0xff;
  CYC(b_+45, b_+48); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+48, b_+51); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+51, b_+53); A = 0x06;
  CYC(b_+53, b_+56); mem_wr(gb, wTmpcbb4, A);
  cutscene02_increment_state(gb);
}

void cutscene02__state1_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+61, b_+63); A = 0x70;
  CALL_C(b_+63, playSound_b00_hook, SYM(playSound_b00), b_+66);
  CYC(b_+66, b_+68); A = 0x3e;
  CALL_C(b_+68, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+71);
  CYC(b_+71, b_+73);
  cutscene02_increment_state(gb);
}

void cutscene02__state2_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+73, cutscene02__func_7c86_hook, b_+6, b_+76);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+77); ret_effect(gb);
    return;
  }
  CYC(b_+76, b_+77);
  CALL_C(b_+77, cutscene02__func_7ced_hook, b_+109, b_+80);
  CYC(b_+80, b_+83); SET_HL((SYM(tilesetLayoutGroup17) + 9));
  CYC(b_+83, b_+85); E = 0x02;
  CALL_C(b_+85, interBankCall_hook, 0x008a, b_+88);
  CYC(b_+88, b_+89); alu_xor(gb, A);
  CYC(b_+89, b_+92); mem_wr(gb, wCutsceneState, A);
  CYC(b_+92, b_+95); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+95, b_+98); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+98, b_+101); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+101, b_+103); A = 0x01;
  CYC(b_+103, b_+106); mem_wr(gb, (wThreadStateBuffer + 15), A);
  CYC(b_+106, b_+109); updateLastToggleBlocksState_hook(gb);
}

void cutscene02__func_7ced_hook(GB *gb) {
  BASE(cutscene02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+109, b_+111); A = 0x03;
  CYC(b_+111, b_+113); hram_wr(gb, 0x70, A);
  CYC(b_+113, b_+115); A = 0x10;
  CALL_C(b_+115, findTileInRoom_hook, SYM(findTileInRoom), b_+118);
  if (!(F & FZ)) {
    CYCT(b_+118, b_+120);
    TAIL(cutscene02__loopEnd);
  }
  CYC(b_+118, b_+120);
  for (;;) {
    CYC(b_+120, b_+122); H = 0xdf;
    CYC(b_+122, b_+123); A = mem_rd(gb, HL);
    CYC(b_+123, b_+125); alu_sub(gb, 0x28);
    CYC(b_+125, b_+127); alu_cp(gb, 0x02);
    if (!(F & FC)) {
      CYCT(b_+127, b_+129);
      goto search_previous;
    }
    CYC(b_+127, b_+129);
    CYC(b_+129, b_+130); A = mem_rd(gb, HL);
    CYC(b_+130, b_+132); alu_sub(gb, 0x28);
    CYC(b_+132, b_+134); alu_add(gb, 0x0e);
    CYC(b_+134, b_+135); C = L;
    CYC(b_+135, b_+136); push_effect(gb, HL);
    CALL_C(b_+136, setTile_hook, SYM(setTile), b_+139);
    CYC(b_+139, b_+140); alu_xor(gb, A);
    CYC(b_+140, b_+142); hram_wr(gb, 0x70, A);
    CALL_C(b_+142, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+145);
    if (!(F & FZ)) {
      CYCT(b_+145, b_+147);
      goto restore_tile_pointer;
    }
    CYC(b_+145, b_+147);
    CYC(b_+147, b_+149); mem_wr(gb, HL, 0x06);
    CYC(b_+149, b_+151); L = 0x4b;
    CALL_C(b_+151, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+154);
restore_tile_pointer:
    CYC(b_+154, b_+155); SET_HL(pop_effect(gb));
    CYC(b_+155, b_+157); A = 0x03;
    CYC(b_+157, b_+159); hram_wr(gb, 0x70, A);
    CYC(b_+159, b_+161); B = 0xcf;
    CYC(b_+161, b_+162); A = mem_rd(gb, HL);
    CYC(b_+162, b_+163); mem_wr(gb, BC, A);
search_previous:
    CYC(b_+163, b_+165); H = 0xcf;
    CYC(b_+165, b_+166); L = alu_dec8(gb, L);
    CYC(b_+166, b_+168); A = 0x10;
    CALL_C(b_+168, backwardsSearch_hook, SYM(backwardsSearch), b_+171);
    if (F & FZ) {
      CYCT(b_+171, b_+173);
      continue;
    }
    CYC(b_+171, b_+173);
    cutscene02_loop_end_body(gb, sp0_);
    return;
  }
}

void cutscene02__loopEnd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  cutscene02_loop_end_body(gb, sp0_);
}

void cutscene0b_hook(GB *gb) {
  BASE(cutscene0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(gfxHeader62) + 17));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CYC(b_+8, b_+11);
  updateAllObjects_hook(gb);
}

void cutscene1a_hook(GB *gb) {
  BASE(cutscene1a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(gfxHeader7e) + 12));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CYC(b_+8, b_+11);
  updateAllObjects_hook(gb);
}

void cutscene1b_hook(GB *gb) {
  BASE(cutscene1b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = hram_rd(gb, 0x70);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  CYC(b_+3, b_+6); SET_HL((SYM(gfxHeader88) + 46));
  CYC(b_+6, b_+8); E = 0x03;
  CALL_C(b_+8, interBankCall_hook, 0x008a, b_+11);
  CYC(b_+11, b_+12); SET_AF(pop_effect(gb));
  CYC(b_+12, b_+14); hram_wr(gb, 0x70, A);
  CYC(b_+14, b_+17);
  updateAllObjects_hook(gb);
}

void warpToMoblinKeepUnderground_hook(GB *gb) {
  BASE(warpToMoblinKeepUnderground);
  CYC(b_+0, b_+3); SET_HL(b_+6);
  CYC(b_+3, b_+6);
  setWarpDestVariables_hook(gb);
}

void cutscene1c_hook(GB *gb) {
  BASE(cutscene1c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(gfxHeader97) + 13));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_C(b_+8, updateAllObjects_hook, SYM(updateAllObjects), b_+11);
  CYC(b_+11, b_+14);
  updateStatusBar_hook(gb);
}

void cutscene1d_hook(GB *gb) {
  BASE(cutscene1d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(gfxHeader9b) + 55));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CYC(b_+8, b_+11); SET_HL(SYM(checkUpdateUnderwaterWaves));
  CYC(b_+11, b_+13); E = 0x01;
  CALL_C(b_+13, interBankCall_hook, 0x008a, b_+16);
  CYC(b_+16, b_+19);
  updateAllObjects_hook(gb);
}

void cutscene1e_hook(GB *gb) {
  BASE(cutscene1e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(gfxHeader9e) + 43));
  CYC(b_+3, b_+5); E = 0x03;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CALL_C(b_+8, updateStatusBar_hook, SYM(updateStatusBar), b_+11);
  CYC(b_+11, b_+14);
  updateAllObjects_hook(gb);
}
