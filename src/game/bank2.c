#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

static void add_double_index_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L); L = A;
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) { burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); return; }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); ret_effect(gb);
}

void loadRememberedCompanion_hook(GB *gb);
void checkAndSpawnMaple_hook(GB *gb);
void stub_02_77f4_hook(GB *gb);
void checkTileValidForEnemySpawn_hook(GB *gb);
void checkPositionValidForEnemySpawn_hook(GB *gb);
void getNextValueFromRandomBuffer_hook(GB *gb);
void getCandidatePositionForEnemy_hook(GB *gb);
void checkEnemyPlacedAtPosition_hook(GB *gb);
void calculateRoomStateModifier_hook(GB *gb);
void createSeaEffectsPartIfApplicable_hook(GB *gb);
void func_02_7a3a_hook(GB *gb);
void dungeonMap_drawItemSprites_hook(GB *gb);
void getNumSmallKeys_hook(GB *gb);
void checkLinkHasBossKey_hook(GB *gb);
void checkLinkHasCompass_hook(GB *gb);
void checkLinkHasMap_hook(GB *gb);
void dungeonMap_drawFloorCursor_hook(GB *gb);
void dungeonMap_drawBossSymbolForFloor_hook(GB *gb);
void dungeonMap_drawLinkIcons_hook(GB *gb);
void dungeonMap_updateCursorFlickerCounter_hook(GB *gb);
void dungeonMap_drawCursor_hook(GB *gb);
void dungeonMap_drawArrows_hook(GB *gb);
void mapGetRoomIndexWithoutUnusedColumns_hook(GB *gb);
void mapMenu_checkCursorRoomVisited_hook(GB *gb);
void mapMenu_checkRoomVisited_hook(GB *gb);
void mapMenu_drawArrow_hook(GB *gb);
void mapMenu_drawCursor_hook(GB *gb);
void mapMenu_drawSpriteAtRoomIndex_hook(GB *gb);
void mapMenu_drawWarpSites_hook(GB *gb);
void mapMenu_drawWarpSites__drawWarpDest_hook(GB *gb);
void mapMenu_drawWarpSites__nextTree_hook(GB *gb);
void getTreeWarpDataIndex_hook(GB *gb);
void getTreeWarpDataForRoom_hook(GB *gb);
void getWarpTreeData_hook(GB *gb);
void getWarpTreeData__ret_hook(GB *gb);

static uint16_t function_caller_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

static void swap_de_hl_memory(GB *gb) {
  CYC(0x7856, 0x7857); A = mem_rd(gb, DE);
  CYC(0x7857, 0x7858); C = A;
  CYC(0x7858, 0x7859); A = mem_rd(gb, HL);
  CYC(0x7859, 0x785a); mem_wr(gb, HL, C);
  CYC(0x785a, 0x785b); mem_wr(gb, DE, A);
  CYC(0x785b, 0x785c); ret_effect(gb);
}

void functionCaller_b02_hook(GB *gb) {
  CYC(0x77b2, 0x77b3); C = L;
  CYC(0x77b3, 0x77b4); A = H;
  CYC(0x77b4, 0x77b5); push_effect(gb, 0x77b5);
  hook_handoff(gb, function_caller_jump_table(gb));
}

void addRoomToEnemiesKilledList_b02_hook(GB *gb) {
  CYC(0x77c3, 0x77c6); SET_HL(0xcdc0);
  CYC(0x77c6, 0x77c9); A = mem_rd(gb, 0xcc30);
  CYC(0x77c9, 0x77cb); B = 0x08;
  for (;;) {
    CYC(0x77cb, 0x77cc); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x77cc, 0x77ce); break; }
    CYC(0x77cc, 0x77ce);
    CYC(0x77ce, 0x77cf); L = alu_inc8(gb, L);
    CYC(0x77cf, 0x77d0); L = alu_inc8(gb, L);
    CYC(0x77d0, 0x77d1); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x77d1, 0x77d3); continue; }
    CYC(0x77d1, 0x77d3);
    CYC(0x77d3, 0x77d6); A = mem_rd(gb, 0xcdd0);
    CYC(0x77d6, 0x77d7); B = A;
    CYC(0x77d7, 0x77d8); A = alu_inc8(gb, A);
    CYC(0x77d8, 0x77d9); A = alu_inc8(gb, A);
    CYC(0x77d9, 0x77db); alu_and(gb, 0x0f);
    CYC(0x77db, 0x77de); mem_wr(gb, 0xcdd0, A);
    CYC(0x77de, 0x77df); A = B;
    CYC(0x77df, 0x77e1); alu_and(gb, 0x0f);
    CYC(0x77e1, 0x77e3); alu_add(gb, 0xc0);
    CYC(0x77e3, 0x77e4); L = A;
    CYC(0x77e4, 0x77e7); A = mem_rd(gb, 0xcc30);
    CYC(0x77e7, 0x77e8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x77e8, 0x77e9); alu_xor(gb, A);
    CYC(0x77e9, 0x77ea); mem_wr(gb, HL, A);
    CYC(0x77ea, 0x77ed); mem_wr(gb, 0xcec9, A);
    CYC(0x77ed, 0x77ee); ret_effect(gb);
    return;
  }
  CYC(0x77ee, 0x77ef); L = alu_inc8(gb, L);
  CYC(0x77ef, 0x77f0); A = mem_rd(gb, HL);
  CYC(0x77f0, 0x77f3); mem_wr(gb, 0xcec9, A);
  CYC(0x77f3, 0x77f4); ret_effect(gb);
}

void markEnemyAsKilledInRoom_b02_hook(GB *gb) {
  CYC(0x77f5, 0x77f8); SET_HL(0xcdc0);
  CYC(0x77f8, 0x77fa); B = 0x08;
  CYC(0x77fa, 0x77fd); A = mem_rd(gb, 0xcc30);
  for (;;) {
    CYC(0x77fd, 0x77fe); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x77fe, 0x7800); break; }
    CYC(0x77fe, 0x7800);
    CYC(0x7800, 0x7801); L = alu_inc8(gb, L);
    CYC(0x7801, 0x7802); L = alu_inc8(gb, L);
    CYC(0x7802, 0x7803); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x7803, 0x7805); continue; }
    CYC(0x7803, 0x7805);
    CYC(0x7805, 0x7806); ret_effect(gb);
    return;
  }
  CYC(0x7806, 0x7807); L = alu_inc8(gb, L);
  CYC(0x7807, 0x7809); E = 0x80;
  CYC(0x7809, 0x780a); A = mem_rd(gb, DE);
  CYC(0x780a, 0x780c); alu_and(gb, 0x70);
  CYC(0x780c, 0x780e); A = alu_swap(gb, A);
  CYC(0x780e, 0x7811); SET_BC(0x00f8);
  CYC(0x7811, 0x7812); alu_add(gb, C);
  CYC(0x7812, 0x7813); C = A;
  CYC(0x7813, 0x7814); A = mem_rd(gb, BC);
  CYC(0x7814, 0x7815); alu_or(gb, mem_rd(gb, HL));
  CYC(0x7815, 0x7816); mem_wr(gb, HL, A);
  CYC(0x7816, 0x7817); ret_effect(gb);
}

void clearEnemiesKilledList_b02_hook(GB *gb) {
  CYC(0x7817, 0x7818); alu_xor(gb, A);
  CYC(0x7818, 0x781b); mem_wr(gb, 0xcdd0, A);
  CYC(0x781b, 0x781e); SET_HL(0xcdc0);
  CYC(0x781e, 0x7820); B = 0x10;
  CYC(0x7820, 0x7823); clearMemory_hook(gb);
}

void generateRandomBuffer_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7823, 0x7824); push_effect(gb, DE);
  CYC(0x7824, 0x7826); A = 0x04;
  CYC(0x7826, 0x7828); mem_wr(gb, 0xff70, A);
  CYC(0x7828, 0x782b); SET_HL(0xd000);
  CYC(0x782b, 0x782d); B = 0;
  do {
    CYC(0x782d, 0x782e); A = B;
    CYC(0x782e, 0x782f); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x782f, 0x7830); B = alu_inc8(gb, B);
    if (!(F & FZ)) CYCT(0x7830, 0x7832); else CYC(0x7830, 0x7832);
  } while (!(F & FZ));
  CYC(0x7832, 0x7835); SET_HL(0xd0ff);
  CYC(0x7835, 0x7836); D = H;
  CALL_C(0x7836, getRandomNumber_hook, 0x043e, 0x7839);
  CYC(0x7839, 0x783a); E = A;
  CYC(0x783a, 0x783d); push_effect(gb, 0x783d); swap_de_hl_memory(gb);
  CYC(0x783d, 0x783f); B = 0xff;
  do {
    CALL_C(0x783f, getRandomNumber_hook, 0x043e, 0x7842);
    CYC(0x7842, 0x7843); C = L;
    CALL_C(0x7843, multiplyAByC_hook, 0x019d, 0x7846);
    CYC(0x7846, 0x7847); E = H;
    CYC(0x7847, 0x7848); L = C;
    CYC(0x7848, 0x784a); H = 0xd0;
    CYC(0x784a, 0x784d); push_effect(gb, 0x784d); swap_de_hl_memory(gb);
    CYC(0x784d, 0x784e); L = alu_dec8(gb, L);
    if (!(F & FZ)) CYCT(0x784e, 0x7850); else CYC(0x784e, 0x7850);
  } while (!(F & FZ));
  CYC(0x7850, 0x7852); A = 0x01;
  CYC(0x7852, 0x7854); mem_wr(gb, 0xff70, A);
  CYC(0x7854, 0x7855); SET_DE(pop_effect(gb));
  CYC(0x7855, 0x7856); ret_effect(gb);
}

void getRandomPositionForEnemy_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x785c, 0x785e); A = 0x40;
  CYC(0x785e, 0x7861); mem_wr(gb, 0xcecf, A);
  for (;;) {
    CYC(0x7861, 0x7864); SET_HL(0xcecf);
    CYC(0x7864, 0x7865); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) { CYCT(0x7865, 0x7867); CYC(0x7880, 0x7881); alu_scf(gb); CYC(0x7881, 0x7882); ret_effect(gb); return; }
    CYC(0x7865, 0x7867);
    CALL_C(0x7867, getCandidatePositionForEnemy_hook, 0x796b, 0x786a);
    CYC(0x786a, 0x786d); mem_wr(gb, 0xcec2, A);
    CYC(0x786d, 0x786e); C = A;
    CALL_C(0x786e, checkPositionValidForEnemySpawn_hook, 0x7894, 0x7871);
    if (F & FC) { CYCT(0x7871, 0x7873); continue; }
    CYC(0x7871, 0x7873);
    CYC(0x7873, 0x7875); A = mem_rd(gb, 0xff8b);
    CYC(0x7875, 0x7877); alu_and(gb, 0x04);
    if (F & FZ) {
      CYC(0x7877, 0x7879);
      CALL_C(0x7879, checkTileValidForEnemySpawn_hook, 0x7882, 0x787c);
      if (F & FC) { CYCT(0x787c, 0x787e); continue; }
      CYC(0x787c, 0x787e);
    } else CYCT(0x7877, 0x7879);
    CYC(0x787e, 0x787f); alu_xor(gb, A);
    CYC(0x787f, 0x7880); ret_effect(gb);
    return;
  }
}

void checkSpawnTimeportalInteraction_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79be, 0x79bf); alu_xor(gb, A);
  CYC(0x79bf, 0x79c2); mem_wr(gb, 0xcddd, A);
  CYC(0x79c2, 0x79c5); SET_HL(0xc63e);
  CYC(0x79c5, 0x79c8); A = mem_rd(gb, 0xcc2d);
  CYC(0x79c8, 0x79c9); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79c9, 0x79ca); ret_effect(gb); return; }
  CYC(0x79c9, 0x79ca);
  CYC(0x79ca, 0x79cb); L = alu_inc8(gb, L);
  CYC(0x79cb, 0x79ce); A = mem_rd(gb, 0xcc30);
  CYC(0x79ce, 0x79cf); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79cf, 0x79d0); ret_effect(gb); return; }
  CYC(0x79cf, 0x79d0);
  CYC(0x79d0, 0x79d1); L = alu_inc8(gb, L);
  CYC(0x79d1, 0x79d2); C = mem_rd(gb, HL);
  CALL_C(0x79d2, getFreeInteractionSlot_hook, 0x3aef, 0x79d5);
  if (!(F & FZ)) { CYCT(0x79d5, 0x79d6); ret_effect(gb); return; }
  CYC(0x79d5, 0x79d6);
  CYC(0x79d6, 0x79d8); mem_wr(gb, HL, 0xde);
  CYC(0x79d8, 0x79da); A = 0x01;
  CYC(0x79da, 0x79dd); mem_wr(gb, 0xcddd, A);
  CYC(0x79dd, 0x79df); L = 0x4b;
  CYC(0x79df, 0x79e2); setShortPosition_paramC_hook(gb);
}

void loadRememberedCompanion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x768a, 0x768d); SET_HL(wRememberedCompanionId);
  CYC(0x768d, 0x768e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x768e, 0x768f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x768f, 0x7690); ret_effect(gb); return; }
  CYC(0x768f, 0x7690);
  CYC(0x7690, 0x7691); C = A;
  CYC(0x7691, 0x7694); A = W8(wActiveGroup);
  CYC(0x7694, 0x7695); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7695, 0x7696); ret_effect(gb); return; }
  CYC(0x7695, 0x7696);
  CYC(0x7696, 0x7697); L = alu_inc8(gb, L);
  CYC(0x7697, 0x769a); A = W8(wActiveRoom);
  CYC(0x769a, 0x769b); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x769b, 0x769c); ret_effect(gb); return; }
  CYC(0x769b, 0x769c);
  CYC(0x769c, 0x769f); A = W8(w1Companion_enabled);
  CYC(0x769f, 0x76a0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76a0, 0x76a1); ret_effect(gb); return; }
  CYC(0x76a0, 0x76a1);
  CYC(0x76a1, 0x76a2); A = C;
  CYC(0x76a2, 0x76a4); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(0x76a4, 0x76a6); goto raft; }
  CYC(0x76a4, 0x76a6);
  CYC(0x76a6, 0x76a9); W8(w1Companion_id) = A;
  CYC(0x76a9, 0x76ab); A = 1;
  CYC(0x76ab, 0x76ae); W8(w1Companion_enabled) = A;
  CYC(0x76ae, 0x76af); L = alu_inc8(gb, L);
  CYC(0x76af, 0x76b0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76b0, 0x76b3); W8(w1Companion_yh) = A;
  CYC(0x76b3, 0x76b4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76b4, 0x76b7); W8(w1Companion_xh) = A;
  CYC(0x76b7, 0x76b8); ret_effect(gb);
  return;
raft:
  CYC(0x76b8, 0x76bb); A = W8(wTilesetFlags);
  CYC(0x76bb, 0x76bd); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x76bd, 0x76be); ret_effect(gb); return; }
  CYC(0x76bd, 0x76be);
  CALL_C(0x76be, getFreeInteractionSlot_hook, ROM_getFreeInteractionSlot, 0x76c1);
  if (!(F & FZ)) { CYCT(0x76c1, 0x76c2); ret_effect(gb); return; }
  CYC(0x76c1, 0x76c2);
  CYC(0x76c2, 0x76c4); mem_wr(gb, HL, 0xe6);
  CYC(0x76c4, 0x76c5); L = alu_inc8(gb, L);
  CYC(0x76c5, 0x76c7); mem_wr(gb, HL, 2);
  CYC(0x76c7, 0x76ca); A = W8(wRememberedCompanionY);
  CYC(0x76ca, 0x76cc); L = 0x4b;
  CYC(0x76cc, 0x76cd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x76cd, 0x76ce); L = alu_inc8(gb, L);
  CYC(0x76ce, 0x76d1); A = W8(wRememberedCompanionX);
  CYC(0x76d1, 0x76d2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x76d2, 0x76d3); ret_effect(gb);
}

void checkAndSpawnMaple_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x76d3, 0x76d4); alu_xor(gb, A);
  CYC(0x76d4, 0x76d7); W8(wIsMaplePresent) = A;
  CYC(0x76d7, 0x76da); A = W8(wcc85);
  CYC(0x76da, 0x76db); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76db, 0x76dc); ret_effect(gb); return; }
  CYC(0x76db, 0x76dc);
  CYC(0x76dc, 0x76df); A = W8(wActiveGroup);
  CYC(0x76df, 0x76e2); SET_HL(0x7792);
  CYC(0x76e2, 0x76e3); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x76e3, 0x76e5); goto start_check; }
  CYC(0x76e3, 0x76e5);
  CYC(0x76e5, 0x76e6); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x76e6, 0x76e7); ret_effect(gb); return; }
  CYC(0x76e6, 0x76e7);
  CYC(0x76e7, 0x76ea); A = W8(w1Companion_enabled);
  CYC(0x76ea, 0x76eb); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x76eb, 0x76ec); ret_effect(gb); return; }
  CYC(0x76eb, 0x76ec);
  CYC(0x76ec, 0x76ef); A = W8(wAnimalCompanion);
  CYC(0x76ef, 0x76f0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x76f0, 0x76f2); goto maple_table; }
  CYC(0x76f0, 0x76f2);
  CYC(0x76f2, 0x76f4); alu_sub(gb, 0x0b);
maple_table:
  CYC(0x76f4, 0x76f7); SET_HL(0x772c);
  CYC(0x76f7, 0x76f8); add_double_index_to_hl(gb, 0x76f8);
  CYC(0x76f8, 0x76f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76f9, 0x76fa); H = mem_rd(gb, HL);
  CYC(0x76fa, 0x76fb); L = A;
start_check:
  CYC(0x76fb, 0x76fe); A = W8(wActiveRoom);
  CALL_C(0x76fe, checkFlag_hook, 0x0205, 0x7701);
  if (!(F & FZ)) { CYCT(0x7701, 0x7702); ret_effect(gb); return; }
  CYC(0x7701, 0x7702);
  CYC(0x7702, 0x7704); A = 0x0f;
  CALL_C(0x7704, cpActiveRing_hook, 0x23b0, 0x7707);
  CYC(0x7707, 0x7709); E = 0x1e;
  if (!(F & FZ)) CYCT(0x7709, 0x770b);
  else { CYC(0x7709, 0x770b); CYC(0x770b, 0x770d); E = alu_srl(gb, E); }
  CYC(0x770d, 0x7710); SET_HL(wMapleKillCounter);
  CYC(0x7710, 0x7711); A = mem_rd(gb, HL);
  CYC(0x7711, 0x7712); alu_cp(gb, E);
  if (F & FC) { CYCT(0x7712, 0x7713); ret_effect(gb); return; }
  CYC(0x7712, 0x7713); CYC(0x7713, 0x7715); mem_wr(gb, HL, 0);
  CYC(0x7715, 0x7718); SET_HL(w1Companion);
  CYC(0x7718, 0x771a); A = 1;
  CYC(0x771a, 0x771d); W8(wcc85) = A;
  CYC(0x771d, 0x7720); W8(wIsMaplePresent) = A;
  CYC(0x7720, 0x7721); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7721, 0x7723); mem_wr(gb, HL, 0x0e);
  CYC(0x7723, 0x7725); L = 0x0b;
  CYC(0x7725, 0x7727); mem_wr(gb, HL, 0x18);
  CYC(0x7727, 0x7729); L = 0x0d;
  CYC(0x7729, 0x772b); mem_wr(gb, HL, 0xb8);
  CYC(0x772b, 0x772c); ret_effect(gb);
}

void stub_02_77f4_hook(GB *gb) {
  CYC(0x77f4, 0x77f5); ret_effect(gb);
}

static void get_absolute_value(GB *gb) {
  CYC(0x7919, 0x791b); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x791b, 0x791c); ret_effect(gb); return; }
  CYC(0x791b, 0x791c); CYC(0x791c, 0x791d); alu_cpl(gb);
  CYC(0x791d, 0x791e); A = alu_inc8(gb, A);
  CYC(0x791e, 0x791f); ret_effect(gb);
}

void checkTileValidForEnemySpawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7882, 0x7884); B = 0xce;
  CYC(0x7884, 0x7885); A = mem_rd(gb, BC);
  CYC(0x7885, 0x7886); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7886, 0x7888); goto invalid; }
  CYC(0x7886, 0x7888);
  CYC(0x7888, 0x788a); B = 0xcf;
  CYC(0x788a, 0x788b); A = mem_rd(gb, BC);
  CYC(0x788b, 0x788e); SET_HL(0x791f);
  CALL_C(0x788e, lookupCollisionTable_hook, 0x1e1f, 0x7891);
  if (!(F & FC)) { CYCT(0x7891, 0x7892); ret_effect(gb); return; }
  CYC(0x7891, 0x7892);
invalid:
  CYC(0x7892, 0x7893); alu_scf(gb);
  CYC(0x7893, 0x7894); ret_effect(gb);
}

static void check_enemy_position_bounds(GB *gb) {
  CYC(0x789b, 0x789e); A = W8(wActiveGroup);
  CYC(0x789e, 0x78a0); alu_and(gb, 4);
  CYC(0x78a0, 0x78a3); SET_HL(0x78c8);
  if (F & FZ) CYCT(0x78a3, 0x78a5);
  else { CYC(0x78a3, 0x78a5); CYC(0x78a5, 0x78a8); SET_HL(0x78d8); }
  CYC(0x78a8, 0x78ab); A = W8(wScreenTransitionDirection);
  CYC(0x78ab, 0x78ac); alu_add(gb, A);
  CYC(0x78ac, 0x78ad); add_double_index_to_hl(gb, 0x78ad);
  CYC(0x78ad, 0x78ae); A = C;
  CYC(0x78ae, 0x78b0); alu_and(gb, 0xf0);
  CYC(0x78b0, 0x78b2); A = alu_swap(gb, A);
  CYC(0x78b2, 0x78b3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x78b3, 0x78b5); goto invalid; }
  CYC(0x78b3, 0x78b5); CYC(0x78b5, 0x78b6); SET_HL(HL + 1);
  CYC(0x78b6, 0x78b7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x78b7, 0x78b9); goto invalid; }
  CYC(0x78b7, 0x78b9); A = C;
  CYC(0x78b9, 0x78ba); alu_and(gb, 0x0f);
  CYC(0x78ba, 0x78bc); SET_HL(HL + 1);
  CYC(0x78bc, 0x78be); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x78be, 0x78c0); goto invalid; }
  CYC(0x78be, 0x78c0); CYC(0x78c0, 0x78c1); SET_HL(HL + 1);
  CYC(0x78c1, 0x78c2); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x78c2, 0x78c4); goto invalid; }
  CYC(0x78c2, 0x78c4); CYC(0x78c4, 0x78c5); alu_xor(gb, A);
  CYC(0x78c5, 0x78c6); ret_effect(gb);
  return;
invalid:
  CYC(0x78c6, 0x78c7); alu_scf(gb);
  CYC(0x78c7, 0x78c8); ret_effect(gb);
}

void checkPositionValidForEnemySpawn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7894, 0x7897); A = W8(wScreenVariables);
  CYC(0x7897, 0x7899); alu_and(gb, 8);
  if (!(F & FZ)) { CYC(0x7899, 0x789b); check_enemy_position_bounds(gb); return; }
  CYCT(0x7899, 0x789b); CYC(0x78e8, 0x78e9); alu_xor(gb, A);
  CYC(0x78e9, 0x78ec); W8(wScreenTransitionDirection) = A;
  CYC(0x78ec, 0x78ef); A = W8(wWarpDestPos);
  CYC(0x78ef, 0x78f0); B = A;
  CYC(0x78f0, 0x78f2); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(0x78f2, 0x78f4); check_enemy_position_bounds(gb); return; }
  CYC(0x78f2, 0x78f4); CYC(0x78f4, 0x78f6); alu_and(gb, 0xf0);
  CYC(0x78f6, 0x78f8); A = alu_swap(gb, A);
  CYC(0x78f8, 0x78f9); H = A;
  CYC(0x78f9, 0x78fa); A = C;
  CYC(0x78fa, 0x78fc); alu_and(gb, 0xf0);
  CYC(0x78fc, 0x78fe); A = alu_swap(gb, A);
  CYC(0x78fe, 0x78ff); alu_sub(gb, H);
  CYC(0x78ff, 0x7902); push_effect(gb, 0x7902); get_absolute_value(gb);
  CYC(0x7902, 0x7904); alu_cp(gb, 3);
  if (F & FC) { CYCT(0x7904, 0x7906); goto check_x; }
  CYC(0x7904, 0x7906); goto valid;
check_x:
  CYC(0x7908, 0x7909); A = B;
  CYC(0x7909, 0x790b); alu_and(gb, 0x0f);
  CYC(0x790b, 0x790c); H = A;
  CYC(0x790c, 0x790d); A = C;
  CYC(0x790d, 0x790f); alu_and(gb, 0x0f);
  CYC(0x790f, 0x7910); alu_sub(gb, H);
  CYC(0x7910, 0x7913); push_effect(gb, 0x7913); get_absolute_value(gb);
  CYC(0x7913, 0x7915); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(0x7915, 0x7917); goto valid; }
  CYC(0x7915, 0x7917); CYC(0x7917, 0x7918); alu_scf(gb);
  CYC(0x7918, 0x7919); ret_effect(gb);
  return;
valid:
  CYC(0x7906, 0x7907); alu_xor(gb, A);
  CYC(0x7907, 0x7908); ret_effect(gb);
}

void getNextValueFromRandomBuffer_hook(GB *gb) {
  CYC(0x7959, 0x795c); SET_HL(wEnemyPlacement_randomBufferIndex);
  CYC(0x795c, 0x795d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x795d, 0x795f); A = 4;
  CYC(0x795f, 0x7961); mem_wr(gb, IO_SVBK, A);
  CYC(0x7961, 0x7962); L = mem_rd(gb, HL);
  CYC(0x7962, 0x7964); H = 0xd0;
  CYC(0x7964, 0x7965); H = mem_rd(gb, HL);
  CYC(0x7965, 0x7967); A = 1;
  CYC(0x7967, 0x7969); mem_wr(gb, IO_SVBK, A);
  CYC(0x7969, 0x796a); A = H;
  CYC(0x796a, 0x796b); ret_effect(gb);
}

void checkEnemyPlacedAtPosition_hook(GB *gb) {
  CYC(0x79a7, 0x79aa); A = W8(wEnemyPlacement_numEnemies);
  CYC(0x79aa, 0x79ab); alu_or(gb, A);
  if (F & FZ) { CYCT(0x79ab, 0x79ac); ret_effect(gb); return; }
  CYC(0x79ab, 0x79ac); CYC(0x79ac, 0x79ad); push_effect(gb, BC);
  CYC(0x79ad, 0x79ae); C = A;
  CYC(0x79ae, 0x79b1); SET_HL(wEnemyPlacement_placedEnemyPositions);
  for (;;) {
    CYC(0x79b1, 0x79b2); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x79b2, 0x79b3); alu_cp(gb, B);
    if (F & FZ) { CYCT(0x79b3, 0x79b5); break; }
    CYC(0x79b3, 0x79b5); CYC(0x79b5, 0x79b6); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x79b6, 0x79b8); continue; }
    CYC(0x79b6, 0x79b8);
    CYC(0x79b8, 0x79b9); SET_BC(pop_effect(gb));
    CYC(0x79b9, 0x79ba); alu_xor(gb, A);
    CYC(0x79ba, 0x79bb); ret_effect(gb);
    return;
  }
  CYC(0x79bb, 0x79bc); SET_BC(pop_effect(gb));
  CYC(0x79bc, 0x79bd); alu_scf(gb);
  CYC(0x79bd, 0x79be); ret_effect(gb);
}

void getCandidatePositionForEnemy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x796b, 0x796e); A = W8(wActiveGroup);
  CYC(0x796e, 0x7970); alu_and(gb, 4);
  if (!(F & FZ)) { CYCT(0x7970, 0x7972); goto dungeon; }
  CYC(0x7970, 0x7972);
overworld:
  CALL_C(0x7972, getNextValueFromRandomBuffer_hook, 0x7959, 0x7975);
  CYC(0x7975, 0x7977); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(0x7977, 0x7979); goto overworld; }
  CYC(0x7977, 0x7979);
  CYC(0x7979, 0x797a); B = A;
  CYC(0x797a, 0x797c); alu_and(gb, 0x0f);
  CYC(0x797c, 0x797e); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x797e, 0x7980); goto overworld; }
  CYC(0x797e, 0x7980);
  CALL_C(0x7980, checkEnemyPlacedAtPosition_hook, 0x79a7, 0x7983);
  if (F & FC) { CYCT(0x7983, 0x7985); goto overworld; }
  CYC(0x7983, 0x7985); CYC(0x7985, 0x7986); A = B;
  CYC(0x7986, 0x7987); ret_effect(gb);
  return;
dungeon:
  CALL_C(0x7987, getNextValueFromRandomBuffer_hook, 0x7959, 0x798a);
  CYC(0x798a, 0x798c); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(0x798c, 0x798e); goto dungeon; }
  CYC(0x798c, 0x798e);
  CYC(0x798e, 0x798f); B = A;
  CYC(0x798f, 0x7991); alu_and(gb, 0xf0);
  if (F & FZ) { CYCT(0x7991, 0x7993); goto dungeon; }
  CYC(0x7991, 0x7993);
  CYC(0x7993, 0x7995); alu_cp(gb, 0xa0);
  if (F & FZ) { CYCT(0x7995, 0x7997); goto dungeon; }
  CYC(0x7995, 0x7997);
  CYC(0x7997, 0x7998); A = B;
  CYC(0x7998, 0x799a); alu_and(gb, 0x0f);
  if (F & FZ) { CYCT(0x799a, 0x799c); goto dungeon; }
  CYC(0x799a, 0x799c);
  CYC(0x799c, 0x799e); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(0x799e, 0x79a0); goto dungeon; }
  CYC(0x799e, 0x79a0);
  CALL_C(0x79a0, checkEnemyPlacedAtPosition_hook, 0x79a7, 0x79a3);
  if (F & FC) { CYCT(0x79a3, 0x79a5); goto dungeon; }
  CYC(0x79a3, 0x79a5); CYC(0x79a5, 0x79a6); A = B;
  CYC(0x79a6, 0x79a7); ret_effect(gb);
}

void calculateRoomStateModifier_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x79e2, 0x79e5); A = W8(wActiveGroup);
  CYC(0x79e5, 0x79e6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x79e6, 0x79e8); goto standard; }
  CYC(0x79e6, 0x79e8);
  CYC(0x79e8, 0x79eb); A = W8(wRoomPack);
  CYC(0x79eb, 0x79ed); alu_cp(gb, 0x7f);
  if (F & FZ) { CYCT(0x79ed, 0x79ef); goto companion_region; }
  CYC(0x79ed, 0x79ef);
standard:
  CYC(0x79ef, 0x79f2); A = W8(wTilesetFlags);
  CYC(0x79f2, 0x79f4); alu_and(gb, 0x40);
  CYC(0x79f4, 0x79f6); B = 0;
  if (F & FZ) CYCT(0x79f6, 0x79f8);
  else { CYC(0x79f6, 0x79f8); CYC(0x79f8, 0x79f9); B = alu_inc8(gb, B); }
  CALL_C(0x79f9, getThisRoomFlags_hook, 0x197d, 0x79fc);
  CYC(0x79fc, 0x79fe); alu_and(gb, 1);
  if (F & FZ) CYCT(0x79fe, 0x7a00);
  else { CYC(0x79fe, 0x7a00); CYC(0x7a00, 0x7a01); B = alu_inc8(gb, B); }
  CYC(0x7a01, 0x7a02); A = B;
  CYC(0x7a02, 0x7a05); W8(wRoomStateModifier) = A;
  CYC(0x7a05, 0x7a06); ret_effect(gb);
  return;
companion_region:
  CYC(0x7a06, 0x7a09); A = W8(wAnimalCompanion);
  CYC(0x7a09, 0x7a0a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7a0a, 0x7a0c); goto standard; }
  CYC(0x7a0a, 0x7a0c);
  CYC(0x7a0c, 0x7a0e); alu_sub(gb, 0x0b);
  CYC(0x7a0e, 0x7a11); W8(wRoomStateModifier) = A;
  CYC(0x7a11, 0x7a12); ret_effect(gb);
}

void createSeaEffectsPartIfApplicable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a12, 0x7a15); A = W8(wActiveCollisions);
  CYC(0x7a15, 0x7a18); SET_HL(0x7a2c);
  CYC(0x7a18, 0x7a19); push_effect(gb, 0x7a19); add_a_to_hl(gb);
  CYC(0x7a19, 0x7a1a); A = mem_rd(gb, HL);
  CYC(0x7a1a, 0x7a1b); push_effect(gb, 0x7a1b); add_a_to_hl(gb);
  for (;;) {
    CYC(0x7a1b, 0x7a1c); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x7a1c, 0x7a1d); alu_or(gb, A);
    if (F & FZ) { CYCT(0x7a1d, 0x7a1e); ret_effect(gb); return; }
    CYC(0x7a1d, 0x7a1e); CYC(0x7a1e, 0x7a1f); push_effect(gb, HL);
    CALL_C(0x7a1f, findTileInRoom_hook, 0x15cc, 0x7a22);
    CYC(0x7a22, 0x7a23); SET_HL(pop_effect(gb));
    if (!(F & FZ)) { CYCT(0x7a23, 0x7a25); continue; }
    CYC(0x7a23, 0x7a25);
    CALL_C(0x7a25, getFreePartSlot_hook, ROM_getFreePartSlot, 0x7a28);
    if (!(F & FZ)) { CYCT(0x7a28, 0x7a29); ret_effect(gb); return; }
    CYC(0x7a28, 0x7a29); CYC(0x7a29, 0x7a2b); mem_wr(gb, HL, 0x2e);
    CYC(0x7a2b, 0x7a2c); ret_effect(gb);
    return;
  }
}

void func_02_7a3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7a3a, 0x7a3d); A = W8(wcddd);
  CYC(0x7a3d, 0x7a3e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7a3e, 0x7a3f); ret_effect(gb); return; }
  CYC(0x7a3e, 0x7a3f);
  CYC(0x7a3f, 0x7a40); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7a40, 0x7a42); goto spawn; }
  CYC(0x7a40, 0x7a42);
  CYC(0x7a42, 0x7a45); W8(wcddd) = A;
  CYC(0x7a45, 0x7a46); ret_effect(gb);
  return;
spawn:
  CALL_C(0x7a46, getFreeInteractionSlot_hook, ROM_getFreeInteractionSlot, 0x7a49);
  if (!(F & FZ)) { CYCT(0x7a49, 0x7a4a); ret_effect(gb); return; }
  CYC(0x7a49, 0x7a4a); CYC(0x7a4a, 0x7a4c); mem_wr(gb, HL, 0xde);
  CYC(0x7a4c, 0x7a4f); A = W8(wPortalPos);
  CYC(0x7a4f, 0x7a51); L = 0x4b;
  CYC(0x7a51, 0x7a54); setShortPosition_hook(gb);
}

void dungeonMap_drawItemSprites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x64da, getNumSmallKeys_hook, 0x651f, 0x64dd);
  CYC(0x64dd, 0x64e0); SET_HL(0x651a);
  if (!(F & FZ)) CALL_C_CC(0x64e0, addSpritesToOam_hook, 0x0d5e, 0x64e3);
  else CYC(0x64e0, 0x64e3);
  CALL_C(0x64e3, checkLinkHasBossKey_hook, 0x6529, 0x64e6);
  CYC(0x64e6, 0x64e9); SET_HL(0x6511);
  if (!(F & FZ)) CALL_C_CC(0x64e9, addSpritesToOam_hook, 0x0d5e, 0x64ec);
  else CYC(0x64e9, 0x64ec);
  CALL_C(0x64ec, checkLinkHasCompass_hook, 0x6532, 0x64ef);
  CYC(0x64ef, 0x64f2); SET_HL(0x6508);
  if (!(F & FZ)) CALL_C_CC(0x64f2, addSpritesToOam_hook, 0x0d5e, 0x64f5);
  else CYC(0x64f2, 0x64f5);
  CALL_C(0x64f5, checkLinkHasMap_hook, 0x653e, 0x64f8);
  CYC(0x64f8, 0x64fb); SET_HL(0x64ff);
  if (!(F & FZ)) CALL_C_CC(0x64fb, addSpritesToOam_hook, 0x0d5e, 0x64fe);
  else CYC(0x64fb, 0x64fe);
  CYC(0x64fe, 0x64ff); ret_effect(gb);
}

void getNumSmallKeys_hook(GB *gb) {
  CYC(0x651f, 0x6522); A = W8(wDungeonIndex);
  CYC(0x6522, 0x6525); SET_HL(wDungeonSmallKeys);
  CYC(0x6525, 0x6526); push_effect(gb, 0x6526); add_a_to_hl(gb);
  CYC(0x6526, 0x6527); A = mem_rd(gb, HL);
  CYC(0x6527, 0x6528); alu_or(gb, A);
  CYC(0x6528, 0x6529); ret_effect(gb);
}

void checkLinkHasBossKey_hook(GB *gb) {
  CYC(0x6529, 0x652c); SET_HL(wDungeonBossKeys);
  CYC(0x652c, 0x652f); A = W8(wDungeonIndex);
  CYC(0x652f, 0x6532); checkFlag_hook(gb);
}

void checkLinkHasCompass_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6532, 0x6533); push_effect(gb, HL);
  CYC(0x6533, 0x6536); SET_HL(wDungeonCompasses);
  CYC(0x6536, 0x6539); A = W8(wDungeonIndex);
  CALL_C(0x6539, checkFlag_hook, 0x0205, 0x653c);
  CYC(0x653c, 0x653d); SET_HL(pop_effect(gb));
  CYC(0x653d, 0x653e); ret_effect(gb);
}

void checkLinkHasMap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x653e, 0x653f); push_effect(gb, HL);
  CYC(0x653f, 0x6542); SET_HL(wDungeonMaps);
  CYC(0x6542, 0x6545); A = W8(wDungeonIndex);
  CALL_C(0x6545, checkFlag_hook, 0x0205, 0x6548);
  CYC(0x6548, 0x6549); SET_HL(pop_effect(gb));
  CYC(0x6549, 0x654a); ret_effect(gb);
}

void dungeonMap_drawFloorCursor_hook(GB *gb) {
  CYC(0x654a, 0x654d); A = W8(wDungeonIndex);
  CYC(0x654d, 0x6550); SET_HL(0x691e);
  CYC(0x6550, 0x6551); add_double_index_to_hl(gb, 0x6551);
  CYC(0x6551, 0x6554); A = W8(wMapMenu_floorIndex);
  CYC(0x6554, 0x6556); A = alu_swap(gb, A);
  CYC(0x6556, 0x6557); alu_rrca(gb);
  CYC(0x6557, 0x6558); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6558, 0x6559); B = A;
  CYC(0x6559, 0x655b); C = 0;
  CYC(0x655b, 0x655e); SET_HL(0x6561);
  CYC(0x655e, 0x6561); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_drawBossSymbolForFloor_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6566, checkLinkHasCompass_hook, 0x6532, 0x6569);
  if (F & FZ) { CYCT(0x6569, 0x656a); ret_effect(gb); return; }
  CYC(0x6569, 0x656a);
  CYC(0x656a, 0x656d); A = W8(wDungeonIndex);
  CYC(0x656d, 0x6570); SET_HL(0x691f);
  CYC(0x6570, 0x6571); add_double_index_to_hl(gb, 0x6571);
  CYC(0x6571, 0x6572); B = mem_rd(gb, HL);
  CYC(0x6572, 0x6574); C = 0;
  CYC(0x6574, 0x6577); SET_HL(0x657a);
  CYC(0x6577, 0x657a); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_drawLinkIcons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x657f, 0x6582); A = W8(wMapMenu_dungeonCursorFlicker);
  CYC(0x6582, 0x6583); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6583, 0x6585); goto floor_list; }
  CYC(0x6583, 0x6585);
  CALL_C(0x6585, dungeonMap_getLinkIconPosition, 0x6756, 0x6588);
  CYC(0x6588, 0x658b); SET_HL(wMapMenu_dungeonScrollY);
  CYC(0x658b, 0x658c); A = B;
  CYC(0x658c, 0x658d); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x658d, 0x658f); alu_cp(gb, 0x12);
  if (!(F & FC)) { CYCT(0x658f, 0x6591); goto floor_list; }
  CYC(0x658f, 0x6591);
  CYC(0x6591, 0x6592); A = alu_inc8(gb, A);
  CYC(0x6592, 0x6594); A = alu_swap(gb, A);
  CYC(0x6594, 0x6595); alu_rrca(gb);
  CYC(0x6595, 0x6596); B = A;
  CYC(0x6596, 0x6598); C = alu_swap(gb, C);
  CYC(0x6598, 0x659a); C = alu_rrc(gb, C);
  CYC(0x659a, 0x659d); SET_HL(0x65bd);
  CALL_C(0x659d, addSpritesToOam_withOffset_hook, 0x0d61, 0x65a0);
floor_list:
  CYC(0x65a0, 0x65a3); A = W8(wDungeonIndex);
  CYC(0x65a3, 0x65a6); SET_HL(0x691e);
  CYC(0x65a6, 0x65a7); add_double_index_to_hl(gb, 0x65a7);
  CYC(0x65a7, 0x65aa); A = W8(wMapMenu_linkFloor);
  CYC(0x65aa, 0x65ab); C = A;
  CYC(0x65ab, 0x65ae); A = W8(wDungeonNumFloors);
  CYC(0x65ae, 0x65af); A = alu_dec8(gb, A);
  CYC(0x65af, 0x65b0); alu_sub(gb, C);
  CYC(0x65b0, 0x65b2); A = alu_swap(gb, A);
  CYC(0x65b2, 0x65b3); alu_rrca(gb);
  CYC(0x65b3, 0x65b4); alu_add(gb, mem_rd(gb, HL));
  CYC(0x65b4, 0x65b5); B = A;
  CYC(0x65b5, 0x65b7); C = 0;
  CYC(0x65b7, 0x65ba); SET_HL(0x65c2);
  CYC(0x65ba, 0x65bd); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_updateCursorFlickerCounter_hook(GB *gb) {
  CYC(0x65c7, 0x65ca); A = W8(wFrameCounter);
  CYC(0x65ca, 0x65cc); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(0x65cc, 0x65cd); ret_effect(gb); return; }
  CYC(0x65cc, 0x65cd);
  CYC(0x65cd, 0x65d0); SET_HL(wMapMenu_dungeonCursorFlicker);
  CYC(0x65d0, 0x65d1); A = mem_rd(gb, HL);
  CYC(0x65d1, 0x65d3); alu_xor(gb, 1);
  CYC(0x65d3, 0x65d4); mem_wr(gb, HL, A);
  CYC(0x65d4, 0x65d5); ret_effect(gb);
}

void dungeonMap_drawCursor_hook(GB *gb) {
  CYC(0x65d5, 0x65d8); A = W8(wSubmenuState);
  CYC(0x65d8, 0x65d9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x65d9, 0x65da); ret_effect(gb); return; }
  CYC(0x65d9, 0x65da);
  CYC(0x65da, 0x65dd); A = W8(wMapMenu_dungeonCursorFlicker);
  CYC(0x65dd, 0x65de); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x65de, 0x65df); ret_effect(gb); return; }
  CYC(0x65de, 0x65df);
  CYC(0x65df, 0x65e2); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(0x65e2, 0x65e4); alu_and(gb, 0xf8);
  CYC(0x65e4, 0x65e5); B = A;
  CYC(0x65e5, 0x65e8); A = W8(wMapMenu_dungeonCursorIndex);
  CYC(0x65e8, 0x65ea); alu_and(gb, 7);
  CYC(0x65ea, 0x65eb); alu_add(gb, A);
  CYC(0x65eb, 0x65ec); alu_add(gb, A);
  CYC(0x65ec, 0x65ed); alu_add(gb, A);
  CYC(0x65ed, 0x65ee); C = A;
  CYC(0x65ee, 0x65f1); SET_HL(0x65f4);
  CYC(0x65f1, 0x65f4); addSpritesToOam_withOffset_hook(gb);
}

void dungeonMap_drawArrows_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65fd, 0x6600); A = W8(wSubmenuState);
  CYC(0x6600, 0x6601); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6601, 0x6602); ret_effect(gb); return; }
  CYC(0x6601, 0x6602);
  CALL_C(0x6602, dungeonMap_checkCanScrollUp, 0x6454, 0x6605);
  if (F & FZ) { CYCT(0x6605, 0x6607); goto down_arrow; }
  CYC(0x6605, 0x6607);
  CYC(0x6607, 0x660a); SET_HL(0x6617);
  CALL_C(0x660a, addSpritesToOam_hook, 0x0d5e, 0x660d);
down_arrow:
  CALL_C(0x660d, dungeonMap_checkCanScrollDown, 0x641b, 0x6610);
  if (F & FZ) { CYCT(0x6610, 0x6611); ret_effect(gb); return; }
  CYC(0x6610, 0x6611);
  CYC(0x6611, 0x6614); SET_HL(0x661c);
  CYC(0x6614, 0x6617); addSpritesToOam_hook(gb);
}

void mapGetRoomIndexWithoutUnusedColumns_hook(GB *gb) {
  CYC(0x6621, 0x6622); push_effect(gb, BC);
  CYC(0x6622, 0x6625); A = W8(wMapMenu_cursorIndex);
  CYC(0x6625, 0x6626); B = A;
  CYC(0x6626, 0x6628); alu_and(gb, 0xf0);
  CYC(0x6628, 0x662a); A = alu_swap(gb, A);
  CYC(0x662a, 0x662b); alu_add(gb, A);
  CYC(0x662b, 0x662c); C = A;
  CYC(0x662c, 0x662d); A = B;
  CYC(0x662d, 0x662e); alu_sub(gb, C);
  CYC(0x662e, 0x662f); B = A;
  CYC(0x662f, 0x6632); A = W8(wTilesetFlags);
  CYC(0x6632, 0x6633); alu_rlca(gb);
  CYC(0x6633, 0x6634); A = B;
  CYC(0x6634, 0x6635); SET_BC(pop_effect(gb));
  CYC(0x6635, 0x6636); ret_effect(gb);
}

void mapMenu_checkCursorRoomVisited_hook(GB *gb) {
  CYC(0x6636, 0x6639); A = W8(wMapMenu_cursorIndex);
  mapMenu_checkRoomVisited_hook(gb);
}

void mapMenu_checkRoomVisited_hook(GB *gb) {
  CYC(0x6639, 0x663a); push_effect(gb, HL);
  CYC(0x663a, 0x663b); H = A;
  CYC(0x663b, 0x663e); A = W8(wMapMenu_mode);
  CYC(0x663e, 0x663f); alu_rrca(gb);
  CYC(0x663f, 0x6640); A = H;
  CYC(0x6640, 0x6643); SET_HL(0xc800);
  if (F & FC) CYCT(0x6643, 0x6645);
  else { CYC(0x6643, 0x6645); CYC(0x6645, 0x6648); SET_HL(0xc700); }
  CYC(0x6648, 0x6649); push_effect(gb, 0x6649); add_a_to_hl(gb);
  CYC(0x6649, 0x664a); A = mem_rd(gb, HL);
  CYC(0x664a, 0x664c); alu_bit(gb, 4, A);
  CYC(0x664c, 0x664d); SET_HL(pop_effect(gb));
  CYC(0x664d, 0x664e); ret_effect(gb);
}

void mapMenu_drawArrow_hook(GB *gb) {
  CYC(0x664e, 0x6651); A = W8(wFrameCounter);
  CYC(0x6651, 0x6653); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x6653, 0x6654); ret_effect(gb); return; }
  CYC(0x6653, 0x6654);
  CYC(0x6654, 0x6657); SET_HL(0x665c);
  CYC(0x6657, 0x665a); A = W8(wMapMenu_currentRoom);
  CYC(0x665a, 0x665c); mapMenu_drawSpriteAtRoomIndex_hook(gb);
}

void mapMenu_drawCursor_hook(GB *gb) {
  CYC(0x6661, 0x6664); SET_HL(0x6669);
  CYC(0x6664, 0x6667); A = W8(wMapMenu_cursorIndex);
  CYC(0x6667, 0x6669); mapMenu_drawSpriteAtRoomIndex_hook(gb);
}

void mapMenu_drawSpriteAtRoomIndex_hook(GB *gb) {
  CYC(0x6672, 0x6673); C = A;
  CYC(0x6673, 0x6676); SET_DE(0x1018);
  CYC(0x6676, 0x6677); A = C;
  CYC(0x6677, 0x6679); alu_and(gb, 0xf0);
  CYC(0x6679, 0x667b); A = alu_srl(gb, A);
  CYC(0x667b, 0x667c); alu_add(gb, D);
  CYC(0x667c, 0x667d); B = A;
  CYC(0x667d, 0x667e); A = C;
  CYC(0x667e, 0x6680); alu_and(gb, 0x0f);
  CYC(0x6680, 0x6681); alu_add(gb, A);
  CYC(0x6681, 0x6682); alu_add(gb, A);
  CYC(0x6682, 0x6683); alu_add(gb, A);
  CYC(0x6683, 0x6684); alu_add(gb, E);
  CYC(0x6684, 0x6685); C = A;
  CYC(0x6685, 0x6688); addSpritesToOam_withOffset_hook(gb);
}

void mapMenu_drawWarpSites_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6688, 0x668b); SET_DE(0x66b9);
  CYC(0x668b, 0x668e); SET_HL(wTmpcec0);
  CYC(0x668e, 0x6690); B = 5;
  CALL_C(0x6690, copyMemoryReverse_hook, 0x047f, 0x6693);
  CYC(0x6693, 0x6696); A = W8(wFrameCounter);
  CYC(0x6696, 0x6698); alu_and(gb, 0x18);
  CYC(0x6698, 0x6699); alu_rrca(gb);
  CYC(0x6699, 0x669a); alu_rrca(gb);
  CYC(0x669a, 0x669c); L = 0xc3;
  CYC(0x669c, 0x669d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x669d, 0x669e); mem_wr(gb, HL, A);
  CYC(0x669e, 0x66a0); C = 0;
  mapMenu_drawWarpSites__drawWarpDest_hook(gb);
}

void mapMenu_drawWarpSites__drawWarpDest_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66a0, 0x66a1); A = C;
  CALL_C(0x66a1, getTreeWarpDataIndex_hook, 0x66be, 0x66a4);
  CYC(0x66a4, 0x66a5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x66a5, 0x66a6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x66a6, 0x66a7); ret_effect(gb); return; }
  CYC(0x66a6, 0x66a7);
  CYC(0x66a7, 0x66a8); push_effect(gb, BC);
  CYC(0x66a8, 0x66a9); C = A;
  CALL_C(0x66a9, mapMenu_checkRoomVisited_hook, 0x6639, 0x66ac);
  if (F & FZ) { CYCT(0x66ac, 0x66ae); mapMenu_drawWarpSites__nextTree_hook(gb); return; }
  CYC(0x66ac, 0x66ae);
  CYC(0x66ae, 0x66af); A = C;
  CYC(0x66af, 0x66b2); SET_HL(wTmpcec0);
  CALL_C(0x66b2, mapMenu_drawSpriteAtRoomIndex_hook, 0x6672, 0x66b5);
  mapMenu_drawWarpSites__nextTree_hook(gb);
}

void mapMenu_drawWarpSites__nextTree_hook(GB *gb) {
  CYC(0x66b5, 0x66b6); SET_BC(pop_effect(gb));
  CYC(0x66b6, 0x66b7); C = alu_inc8(gb, C);
  CYC(0x66b7, 0x66b9); mapMenu_drawWarpSites__drawWarpDest_hook(gb);
}

void getTreeWarpDataIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66be, 0x66bf); C = A;
  CALL_C(0x66bf, getWarpTreeData_hook, 0x66d4, 0x66c2);
  CYC(0x66c2, 0x66c3); alu_add(gb, A);
  CYC(0x66c3, 0x66c4); alu_add(gb, C);
  CYC(0x66c4, 0x66c5); push_effect(gb, 0x66c5); add_a_to_hl(gb);
  CYC(0x66c5, 0x66c6); ret_effect(gb);
}

void getTreeWarpDataForRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x66c6, 0x66c7); C = A;
  CALL_C(0x66c7, getWarpTreeData_hook, 0x66d4, 0x66ca);
  for (;;) {
    CYC(0x66ca, 0x66cb); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x66cb, 0x66cc); alu_or(gb, A);
    CYC(0x66cc, 0x66cd); alu_scf(gb);
    if (F & FZ) { CYCT(0x66cd, 0x66ce); ret_effect(gb); return; }
    CYC(0x66cd, 0x66ce);
    CYC(0x66ce, 0x66cf); alu_cp(gb, C);
    if (F & FZ) { CYCT(0x66cf, 0x66d0); ret_effect(gb); return; }
    CYC(0x66cf, 0x66d0);
    CYC(0x66d0, 0x66d1); SET_HL(HL + 1);
    CYC(0x66d1, 0x66d2); SET_HL(HL + 1);
    CYC(0x66d2, 0x66d4);
  }
}

void getWarpTreeData_hook(GB *gb) {
  CYC(0x66d4, 0x66d5); push_effect(gb, AF);
  CYC(0x66d5, 0x66d8); SET_HL(0x6d1e);
  CYC(0x66d8, 0x66db); A = W8(wTilesetFlags);
  CYC(0x66db, 0x66dc); alu_rlca(gb);
  if (F & FC) { CYCT(0x66dc, 0x66de); getWarpTreeData__ret_hook(gb); return; }
  CYC(0x66dc, 0x66de);
  CYC(0x66de, 0x66e1); SET_HL(0x6d03);
  CYC(0x66e1, 0x66e4); A = mem_rd(gb, 0xc7ac);
  CYC(0x66e4, 0x66e5); alu_rlca(gb);
  if (F & FC) { CYCT(0x66e5, 0x66e7); getWarpTreeData__ret_hook(gb); return; }
  CYC(0x66e5, 0x66e7);
  CYC(0x66e7, 0x66e9); A = 3;
  CYC(0x66e9, 0x66ea); push_effect(gb, 0x66ea); add_a_to_hl(gb);
  getWarpTreeData__ret_hook(gb);
}

void getWarpTreeData__ret_hook(GB *gb) {
  CYC(0x66ea, 0x66eb); SET_AF(pop_effect(gb));
  CYC(0x66eb, 0x66ec); ret_effect(gb);
}
