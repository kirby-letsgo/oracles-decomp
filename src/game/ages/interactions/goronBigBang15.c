#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void goron_setLinkPositionAndDirection_hook(GB *gb);

static void goronBigBang_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void goronBigBang_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void goron_bigBang_initLinkPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6346, 0x6348); A = 0x00;
  CYC(0x6348, 0x634b); SET_BC(0x4850);
  goron_setLinkPositionAndDirection_hook(gb);
}

void goron_bigBang_spawnPrize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x66f2, getThisRoomFlags_hook, 0x197d, 0x66f5);
  CYC(0x66f5, 0x66f7); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x66f7, 0x66f9); goto alreadyGotMermaidKey; }
  CYC(0x66f7, 0x66f9);
  CYC(0x66f9, 0x66fa); alu_xor(gb, A);
  CYC(0x66fa, 0x66fc); goto checkSpawnPrize;

alreadyGotMermaidKey:
  CALL_C(0x66fc, getRandomNumber_hook, 0x043e, 0x66ff);
  CYC(0x66ff, 0x6701); alu_and(gb, 0x0f);
  CYC(0x6701, 0x6704); SET_HL(0x6732);
  CYC(0x6704, 0x6705); goronBigBang_addAToHl_from_rst(gb, 0x6705);
  CYC(0x6705, 0x6706); A = mem_rd(gb, HL);

checkSpawnPrize:
  CYC(0x6706, 0x6708); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x6708, 0x670a); goto spawnPrize; }
  CYC(0x6708, 0x670a);
  CALL_C(0x670a, getRandomNumber_hook, 0x043e, 0x670d);
  CYC(0x670d, 0x670f); alu_and(gb, 0x01);
  CYC(0x670f, 0x6711); alu_add(gb, 0x04);

spawnPrize:
  CYC(0x6711, 0x6714); mem_wr(gb, 0xcfd6, A);
  CYC(0x6714, 0x6717); SET_HL(0x6742);
  CYC(0x6717, 0x6718); goronBigBang_addDoubleIndexToHl_from_rst(gb, 0x6718);
  CYC(0x6718, 0x6719); B = mem_rd(gb, HL);
  CYC(0x6719, 0x671a); L = alu_inc8(gb, L);
  CYC(0x671a, 0x671b); C = mem_rd(gb, HL);
  CALL_C(0x671b, getFreeInteractionSlot_hook, 0x3aef, 0x671e);
  if (!(F & FZ)) { CYCT(0x671e, 0x671f); ret_effect(gb); return; }
  CYC(0x671e, 0x671f);
  CYC(0x671f, 0x6721); mem_wr(gb, HL, 0x60);
  CYC(0x6721, 0x6722); L = alu_inc8(gb, L);
  CYC(0x6722, 0x6723); mem_wr(gb, HL, B);
  CYC(0x6723, 0x6724); L = alu_inc8(gb, L);
  CYC(0x6724, 0x6725); mem_wr(gb, HL, C);
  CYC(0x6725, 0x6727); L = 0x4b;
  CYC(0x6727, 0x6729); mem_wr(gb, HL, 0x38);
  CYC(0x6729, 0x672b); L = 0x4d;
  CYC(0x672b, 0x672d); mem_wr(gb, HL, 0x50);
  CYC(0x672d, 0x672f); L = 0x4f;
  CYC(0x672f, 0x6731); mem_wr(gb, HL, 0xf0);
  CYC(0x6731, 0x6732); ret_effect(gb);
}

void goron_bigBang_hideSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68e3, 0x68e4); H = D;
  CYC(0x68e4, 0x68e6); L = 0x7e;
  CYC(0x68e6, 0x68e8); mem_wr(gb, HL, 0x01);
  CYC(0x68e8, 0x68e9); alu_xor(gb, A);
  CYC(0x68e9, 0x68eb); L = 0x66;
  CYC(0x68eb, 0x68ec); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x68ec, 0x68ed); mem_wr(gb, HL, A);
  CYC(0x68ed, 0x68f0); objectSetInvisible_hook(gb);
}

void goron_bigBang_unhideSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68f0, 0x68f1); H = D;
  CYC(0x68f1, 0x68f3); L = 0x7e;
  CYC(0x68f3, 0x68f5); mem_wr(gb, HL, 0x00);
  CYC(0x68f5, 0x68f7); A = 0x06;
  CYC(0x68f7, 0x68f9); L = 0x66;
  CYC(0x68f9, 0x68fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x68fa, 0x68fb); mem_wr(gb, HL, A);
  CYC(0x68fb, 0x68fe); objectSetVisible_hook(gb);
}

void goron_bigBang_checkLinkHitByBomb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x68fe, 0x6901); A = mem_rd(gb, 0xd02b);
  CYC(0x6901, 0x6902); alu_or(gb, A);
  CALL_C(0x6902, writeFlagsTocddb_hook, 0x5118, 0x6905);
  CYC(0x6905, 0x6906); alu_cpl(gb);
  CYC(0x6906, 0x6909); mem_wr(gb, 0xcddb, A);
  CYC(0x6909, 0x690a); ret_effect(gb);
}

void goron_bigBang_createBombSpawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x690a, getFreePartSlot_hook, 0x3e8e, 0x690d);
  if (!(F & FZ)) { CYCT(0x690d, 0x690e); ret_effect(gb); return; }
  CYC(0x690d, 0x690e);
  CYC(0x690e, 0x6910); mem_wr(gb, HL, 0x49);
  CYC(0x6910, 0x6911); L = alu_inc8(gb, L);
  CYC(0x6911, 0x6913); mem_wr(gb, HL, 0xff);
  CYC(0x6913, 0x6914); ret_effect(gb);
}

void goron_createBombFlowerSprite_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6914, getFreeInteractionSlot_hook, 0x3aef, 0x6917);
  if (!(F & FZ)) { CYCT(0x6917, 0x6918); ret_effect(gb); return; }
  CYC(0x6917, 0x6918);
  CYC(0x6918, 0x691a); mem_wr(gb, HL, 0x60);
  CYC(0x691a, 0x691b); L = alu_inc8(gb, L);
  CYC(0x691b, 0x691d); mem_wr(gb, HL, 0x49);
  CYC(0x691d, 0x691e); L = alu_inc8(gb, L);
  CYC(0x691e, 0x6920); mem_wr(gb, HL, 0x01);
  CYC(0x6920, 0x6922); L = 0x4b;
  CYC(0x6922, 0x6924); mem_wr(gb, HL, 0x60);
  CYC(0x6924, 0x6926); L = 0x4d;
  CYC(0x6926, 0x6928); mem_wr(gb, HL, 0x38);
  CYC(0x6928, 0x6929); ret_effect(gb);
}

void goron_createExplosionIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6986, 0x6989); SET_BC(0x699c);
  CALL_C(0x6989, addDoubleIndexToBc_hook, 0x007e, 0x698c);
  CALL_C(0x698c, getFreeInteractionSlot_hook, 0x3aef, 0x698f);
  if (!(F & FZ)) { CYCT(0x698f, 0x6990); ret_effect(gb); return; }
  CYC(0x698f, 0x6990);
  CYC(0x6990, 0x6992); mem_wr(gb, HL, 0x56);
  CYC(0x6992, 0x6994); L = 0x4b;
  CYC(0x6994, 0x6995); A = mem_rd(gb, BC);
  CYC(0x6995, 0x6996); mem_wr(gb, HL, A);
  CYC(0x6996, 0x6997); SET_BC(BC + 1);
  CYC(0x6997, 0x6999); L = 0x4d;
  CYC(0x6999, 0x699a); A = mem_rd(gb, BC);
  CYC(0x699a, 0x699b); mem_wr(gb, HL, A);
  CYC(0x699b, 0x699c); ret_effect(gb);
}

void goron_countdownToNextExplosionGroup_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6929, 0x692a); H = D;
  CYC(0x692a, 0x692c); L = 0x7a;
  CYC(0x692c, 0x692d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x692d, 0x692e); ret_effect(gb); return; }
  CYC(0x692d, 0x692e);
  CYC(0x692e, 0x6930); L = 0x7b;
  CYC(0x6930, 0x6931); A = mem_rd(gb, HL);
  CYC(0x6931, 0x6932); A = alu_inc8(gb, A);
  CYC(0x6932, 0x6934); alu_and(gb, 0x07);
  CYC(0x6934, 0x6935); mem_wr(gb, HL, A);
  CYC(0x6935, 0x6937); hram_wr(gb, 0x8b, A);
  CYC(0x6937, 0x693a); SET_BC(0x695e);
  CALL_C(0x693a, addAToBc_hook, 0x006d, 0x693d);
  CYC(0x693d, 0x693e); A = mem_rd(gb, BC);
  CYC(0x693e, 0x6940); L = 0x7a;
  CYC(0x6940, 0x6941); mem_wr(gb, HL, A);
  CYC(0x6941, 0x6943); A = hram_rd(gb, 0x8b);
  CYC(0x6943, 0x6944); alu_add(gb, A);
  CYC(0x6944, 0x6947); SET_BC(0x6966);
  CALL_C(0x6947, addDoubleIndexToBc_hook, 0x007e, 0x694a);
  CYC(0x694a, 0x694c); A = 0x04;

next:
  CYC(0x694c, 0x694e); hram_wr(gb, 0x8d, A);
  CYC(0x694e, 0x694f); A = mem_rd(gb, BC);
  CYC(0x694f, 0x6951); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x6951, 0x6952); ret_effect(gb); return; }
  CYC(0x6951, 0x6952);
  CYC(0x6952, 0x6953); push_effect(gb, BC);
  CALL_C(0x6953, goron_createExplosionIndex_hook, 0x6986, 0x6956);
  CYC(0x6956, 0x6957); SET_BC(pop_effect(gb));
  CYC(0x6957, 0x6958); SET_BC(BC + 1);
  CYC(0x6958, 0x695a); A = hram_rd(gb, 0x8d);
  CYC(0x695a, 0x695b); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x695b, 0x695d); goto next; }
  CYC(0x695b, 0x695d);
  CYC(0x695d, 0x695e); ret_effect(gb);
}

void goron_bigBang_loadRoomLayout_hook(GB *gb);

void goron_bigBang_loadMinigameLayout1_topHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69ac, 0x69af); SET_HL(0x69f2);
  CYC(0x69af, 0x69b1); C = 0x11;
  CYC(0x69b1, 0x69b3); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadMinigameLayout1_bottomHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69b3, 0x69b6); SET_HL(0x6a0a);
  CYC(0x69b6, 0x69b8); C = 0x41;
  CYC(0x69b8, 0x69ba); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadMinigameLayout2_topHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69ba, 0x69bd); SET_HL(0x6a22);
  CYC(0x69bd, 0x69bf); C = 0x11;
  CYC(0x69bf, 0x69c1); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadMinigameLayout2_bottomHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69c1, 0x69c4); SET_HL(0x6a3a);
  CYC(0x69c4, 0x69c6); C = 0x41;
  CYC(0x69c6, 0x69c8); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadNormalRoomLayout_topHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69c8, 0x69cb); SET_HL(0x6a52);
  CYC(0x69cb, 0x69cd); C = 0x11;
  CYC(0x69cd, 0x69cf); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadNormalRoomLayout_bottomHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69cf, 0x69d2); SET_HL(0x6a52);
  CYC(0x69d2, 0x69d4); C = 0x41;
  goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadRoomLayout_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x69d4, 0x69d6); A = 0x03;

nextRow:
  CYC(0x69d6, 0x69d8); hram_wr(gb, 0x93, A);
  CYC(0x69d8, 0x69da); A = 0x08;

nextColumn:
  CYC(0x69da, 0x69dc); hram_wr(gb, 0x92, A);
  CYC(0x69dc, 0x69dd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x69dd, 0x69de); push_effect(gb, HL);
  CALL_C(0x69de, setTile_hook, 0x3a9c, 0x69e1);
  CYC(0x69e1, 0x69e2); SET_HL(pop_effect(gb));
  CYC(0x69e2, 0x69e3); C = alu_inc8(gb, C);
  CYC(0x69e3, 0x69e5); A = hram_rd(gb, 0x92);
  CYC(0x69e5, 0x69e6); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x69e6, 0x69e8); goto nextColumn; }
  CYC(0x69e6, 0x69e8);
  CYC(0x69e8, 0x69e9); A = C;
  CYC(0x69e9, 0x69eb); alu_add(gb, 0x08);
  CYC(0x69eb, 0x69ec); C = A;
  CYC(0x69ec, 0x69ee); A = hram_rd(gb, 0x93);
  CYC(0x69ee, 0x69ef); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x69ef, 0x69f1); goto nextRow; }
  CYC(0x69ef, 0x69f1);
  CYC(0x69f1, 0x69f2); ret_effect(gb);
}

void goron_bigBang_blockOrRestoreExit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6a6a, 0x6a6d); SET_HL(0x6a7d);
  CYC(0x6a6d, 0x6a6e); goronBigBang_addAToHl_from_rst(gb, 0x6a6e);
  CYC(0x6a6e, 0x6a70); C = 0x73;

next:
  CYC(0x6a70, 0x6a71); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6a71, 0x6a72); push_effect(gb, HL);
  CALL_C(0x6a72, setTile_hook, 0x3a9c, 0x6a75);
  CYC(0x6a75, 0x6a76); SET_HL(pop_effect(gb));
  CYC(0x6a76, 0x6a77); C = alu_inc8(gb, C);
  CYC(0x6a77, 0x6a78); A = C;
  CYC(0x6a78, 0x6a7a); alu_cp(gb, 0x77);
  if (!(F & FZ)) { CYCT(0x6a7a, 0x6a7c); goto next; }
  CYC(0x6a7a, 0x6a7c);
  CYC(0x6a7c, 0x6a7d); ret_effect(gb);
}
