#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void goron_setLinkPositionAndDirection_hook(GB *gb);
void goron_findInteractionWithID_hook(GB *gb);

static void goronTargetCarts_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void goronTargetCarts_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void goron_targetCarts_setLinkPositionToCartPlatform_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6338, 0x633a); A = 0x00;
  CYC(0x633a, 0x633d); SET_BC(0x8838);
  CYC(0x633d, 0x633f); goron_setLinkPositionAndDirection_hook(gb);
}

void goron_targetCarts_setLinkPositionAfterGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x633f, 0x6341); A = 0x01;
  CYC(0x6341, 0x6344); SET_BC(0x78a8);
  CYC(0x6344, 0x6346); goron_setLinkPositionAndDirection_hook(gb);
}

void goron_targetCarts_spawnPrize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6698, getThisRoomFlags_hook, 0x197d, 0x669b);
  CYC(0x669b, 0x669d); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x669d, 0x669f); goto alreadyGotBrisket; }
  CYC(0x669d, 0x669f);
  CYC(0x669f, 0x66a0); alu_xor(gb, A);
  CYC(0x66a0, 0x66a2); goto spawnPrize;

alreadyGotBrisket:
  CALL_C(0x66a2, getRandomNumber_hook, 0x043e, 0x66a5);
  CYC(0x66a5, 0x66a7); alu_and(gb, 0x0f);
  CYC(0x66a7, 0x66aa); SET_HL(0x66d8);
  CYC(0x66aa, 0x66ab); goronTargetCarts_addAToHl_from_rst(gb, 0x66ab);
  CYC(0x66ab, 0x66ac); A = mem_rd(gb, HL);
  CYC(0x66ac, 0x66ae); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x66ae, 0x66b0); goto spawnPrize; }
  CYC(0x66ae, 0x66b0);
  CYC(0x66b0, 0x66b2); A = 0x06;
  CALL_C(0x66b2, checkTreasureObtained_hook, 0x1748, 0x66b5);
  CYC(0x66b5, 0x66b7); A = 0x04;
  if (!(F & FC)) { CYCT(0x66b7, 0x66b9); goto spawnPrize; }
  CYC(0x66b7, 0x66b9);
  CYC(0x66b9, 0x66bb); A = 0x03;

spawnPrize:
  CYC(0x66bb, 0x66be); mem_wr(gb, 0xcfd6, A);
  CYC(0x66be, 0x66c1); SET_HL(0x66e8);
  CYC(0x66c1, 0x66c2); goronTargetCarts_addDoubleIndexToHl_from_rst(gb, 0x66c2);
  CYC(0x66c2, 0x66c3); B = mem_rd(gb, HL);
  CYC(0x66c3, 0x66c4); L = alu_inc8(gb, L);
  CYC(0x66c4, 0x66c5); C = mem_rd(gb, HL);
  CALL_C(0x66c5, getFreeInteractionSlot_hook, 0x3aef, 0x66c8);
  if (!(F & FZ)) { CYCT(0x66c8, 0x66c9); ret_effect(gb); return; }
  CYC(0x66c8, 0x66c9);
  CYC(0x66c9, 0x66cb); mem_wr(gb, HL, 0x60);
  CYC(0x66cb, 0x66cc); L = alu_inc8(gb, L);
  CYC(0x66cc, 0x66cd); mem_wr(gb, HL, B);
  CYC(0x66cd, 0x66ce); L = alu_inc8(gb, L);
  CYC(0x66ce, 0x66cf); mem_wr(gb, HL, C);
  CYC(0x66cf, 0x66d1); L = 0x4b;
  CYC(0x66d1, 0x66d3); mem_wr(gb, HL, 0x78);
  CYC(0x66d3, 0x66d5); L = 0x4d;
  CYC(0x66d5, 0x66d7); mem_wr(gb, HL, 0x78);
  CYC(0x66d7, 0x66d8); ret_effect(gb);
}

void goron_targetCarts_deleteMinecartAndClearStaticObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6758, 0x675a); B = 0x16;
  CALL_C(0x675a, goron_findInteractionWithID_hook, 0x6767, 0x675d);
  CYC(0x675d, 0x675e); push_effect(gb, DE);
  CYC(0x675e, 0x675f); E = L;
  CYC(0x675f, 0x6760); D = H;
  CALL_C(0x6760, objectDelete_de_hook, 0x21e3, 0x6763);
  CYC(0x6763, 0x6764); SET_DE(pop_effect(gb));
  CYC(0x6764, 0x6767); clearStaticObjects_hook(gb);
}

void goron_targetCarts_deleteCrystals_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x677d, 0x6780); SET_HL(0xd080);
loop:
  CYC(0x6780, 0x6781); A = mem_rd(gb, HL);
  CYC(0x6781, 0x6782); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6782, 0x6784); goto nextEnemy; }
  CYC(0x6782, 0x6784);
  CYC(0x6784, 0x6785); L = alu_inc8(gb, L);
  CYC(0x6785, 0x6786); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6786, 0x6788); alu_cp(gb, 0x63);
  if (!(F & FZ)) { CYCT(0x6788, 0x678a); goto nextEnemy; }
  CYC(0x6788, 0x678a);
  CYC(0x678a, 0x678b); push_effect(gb, DE);
  CYC(0x678b, 0x678c); push_effect(gb, HL);
  CYC(0x678c, 0x678d); E = L;
  CYC(0x678d, 0x678e); D = H;
  CALL_C(0x678e, objectDelete_de_hook, 0x21e3, 0x6791);
  CYC(0x6791, 0x6794); SET_HL(0xcdd1);
  CYC(0x6794, 0x6795); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6795, 0x6796); SET_HL(pop_effect(gb));
  CYC(0x6796, 0x6797); SET_DE(pop_effect(gb));

nextEnemy:
  CYC(0x6797, 0x6798); H = alu_inc8(gb, H);
  CYC(0x6798, 0x6799); A = H;
  CYC(0x6799, 0x679b); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x679b, 0x679d); goto loop; }
  CYC(0x679b, 0x679d);
  CYC(0x679d, 0x679e); ret_effect(gb);
}

void goron_targetCarts_setPlayingFlag_hook(GB *gb);
void goron_targetCarts_clearPlayingFlag_hook(GB *gb);

void goron_targetCarts_beginGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x679e, 0x679f); alu_xor(gb, A);
  CYC(0x679f, 0x67a2); mem_wr(gb, 0xcfdb, A);
  CYC(0x67a2, 0x67a5); mem_wr(gb, 0xcfdd, A);
  CYC(0x67a5, 0x67a8); mem_wr(gb, 0xcfde, A);
  CYC(0x67a8, 0x67ab); mem_wr(gb, 0xcfdc, A);
  CYC(0x67ab, 0x67ae); goron_targetCarts_setPlayingFlag_hook(gb);
}

void goron_targetCarts_endGame_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67ae, 0x67b1); goron_targetCarts_clearPlayingFlag_hook(gb);
}

void goron_targetCarts_setPlayingFlag_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x67b1, getThisRoomFlags_hook, 0x197d, 0x67b4);
  CYC(0x67b4, 0x67b6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x67b6, 0x67b7); ret_effect(gb);
}

void goron_targetCarts_clearPlayingFlag_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x67b7, getThisRoomFlags_hook, 0x197d, 0x67ba);
  CYC(0x67ba, 0x67bc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x67bc, 0x67bd); ret_effect(gb);
}

void goron_targetCarts_setupNumTargetsHitText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67cc, 0x67cf); A = mem_rd(gb, 0xcfde);
  CYC(0x67cf, 0x67d1); alu_add(gb, 0x00);
  CYC(0x67d1, 0x67d2); alu_daa(gb);
  CYC(0x67d2, 0x67d5); SET_HL(0xcba8);
  CYC(0x67d5, 0x67d6); mem_wr(gb, HL, A);
  CYC(0x67d6, 0x67d7); SET_HL(HL + 1);
  CYC(0x67d7, 0x67d9); mem_wr(gb, HL, 0x00);
  CYC(0x67d9, 0x67da); ret_effect(gb);
}

void goron_targetCarts_checkHitAllTargets_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67da, 0x67dd); A = mem_rd(gb, 0xcfde);
  CYC(0x67dd, 0x67df); alu_cp(gb, 0x0c);
  CYC(0x67df, 0x67e2); writeFlagsTocddb_hook(gb);
}

void goron_targetCarts_checkHit9OrMoreTargets_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67e2, 0x67e5); A = mem_rd(gb, 0xcfde);
  CYC(0x67e5, 0x67e7); alu_cp(gb, 0x09);
  CYC(0x67e7, 0x67e8); alu_ccf(gb);
  CYC(0x67e8, 0x67eb); writeFlagsTocddb_hook(gb);
}

void goron_targetCarts_configureInventory_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67eb, 0x67ee); SET_BC(0xc688);
  CYC(0x67ee, 0x67f1); SET_HL(0xcfd7);
  CYC(0x67f1, 0x67f2); A = mem_rd(gb, BC);
  CYC(0x67f2, 0x67f3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x67f3, 0x67f6); A = mem_rd(gb, 0xc689);
  CYC(0x67f6, 0x67f8); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x67f8, 0x67fa); goto equipToA; }
  CYC(0x67f8, 0x67fa);

equipToB:
  CYC(0x67fa, 0x67fb); alu_xor(gb, A);
  CYC(0x67fb, 0x67fc); mem_wr(gb, BC, A);
  CYC(0x67fc, 0x67fd); C = alu_inc8(gb, C);
  CYC(0x67fd, 0x67fe); A = mem_rd(gb, BC);
  CYC(0x67fe, 0x67ff); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x67ff, 0x6801); A = 0x0f;
  CYC(0x6801, 0x6802); mem_wr(gb, BC, A);
  CYC(0x6802, 0x6804); goto setupSeedShooter;

equipToA:
  CYC(0x6804, 0x6806); A = 0x0f;
  CYC(0x6806, 0x6807); mem_wr(gb, BC, A);
  CYC(0x6807, 0x6808); C = alu_inc8(gb, C);
  CYC(0x6808, 0x6809); A = mem_rd(gb, BC);
  CYC(0x6809, 0x680a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x680a, 0x680b); alu_xor(gb, A);
  CYC(0x680b, 0x680c); mem_wr(gb, BC, A);

setupSeedShooter:
  CYC(0x680c, 0x680e); C = 0xba;
  CYC(0x680e, 0x680f); A = mem_rd(gb, BC);
  CYC(0x680f, 0x6810); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6810, 0x6812); A = 0x99;
  CYC(0x6812, 0x6813); mem_wr(gb, BC, A);
  CYC(0x6813, 0x6815); C = 0xc5;
  CYC(0x6815, 0x6816); A = mem_rd(gb, BC);
  CYC(0x6816, 0x6817); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6817, 0x6819); A = 0x01;
  CYC(0x6819, 0x681a); mem_wr(gb, BC, A);
  CYC(0x681a, 0x681c); A = 0xff;
  CYC(0x681c, 0x681f); mem_wr(gb, 0xcbe9, A);
  CYC(0x681f, 0x6820); ret_effect(gb);
}

void goron_targetCarts_restoreInventory_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6820, 0x6823); SET_BC(0xc688);
  CYC(0x6823, 0x6826); SET_HL(0xcfd7);
  CYC(0x6826, 0x6827); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6827, 0x6828); mem_wr(gb, BC, A);
  CYC(0x6828, 0x6829); C = alu_inc8(gb, C);
  CYC(0x6829, 0x682a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x682a, 0x682b); mem_wr(gb, BC, A);
  CYC(0x682b, 0x682d); C = 0xba;
  CYC(0x682d, 0x682e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x682e, 0x682f); mem_wr(gb, BC, A);
  CYC(0x682f, 0x6831); C = 0xc5;
  CYC(0x6831, 0x6832); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6832, 0x6833); mem_wr(gb, BC, A);
  CYC(0x6833, 0x6835); A = 0xff;
  CYC(0x6835, 0x6838); mem_wr(gb, 0xcbe9, A);
  CYC(0x6838, 0x6839); ret_effect(gb);
}

void goron_targetCarts_loadCrystals_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6839, getThisRoomFlags_hook, 0x197d, 0x683c);
  CYC(0x683c, 0x683e); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(0x683e, 0x6840); A = 0x00;
  if (F & FZ) { CYCT(0x6840, 0x6842); goto L_6848; }
  CYC(0x6840, 0x6842);
  CALL_C(0x6842, getRandomNumber_hook, 0x043e, 0x6845);
  CYC(0x6845, 0x6847); alu_and(gb, 0x01);
  CYC(0x6847, 0x6848); A = alu_inc8(gb, A);

L_6848:
  CYC(0x6848, 0x684b); mem_wr(gb, 0xcfd4, A);
  CYC(0x684b, 0x684e); SET_HL(0x7870);
  CYC(0x684e, 0x6851); parseGivenObjectData_b00_hook(gb);
}

void goron_targetCarts_reloadCrystalsInFirstRoom_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6851, 0x6852); alu_xor(gb, A);

loop:
  CYC(0x6852, 0x6854); hram_wr(gb, 0x8b, A);
  CYC(0x6854, 0x6857); SET_HL(0xcfdd);
  CALL_C(0x6857, checkFlag_hook, 0x0205, 0x685a);
  if (!(F & FZ)) { CYCT(0x685a, 0x685c); goto nextCrystal; }
  CYC(0x685a, 0x685c);
  CALL_C(0x685c, getFreeEnemySlot_hook, 0x2e27, 0x685f);
  CYC(0x685f, 0x6861); A = hram_rd(gb, 0x8b);
  CYC(0x6861, 0x6863); mem_wr(gb, HL, 0x63);
  CYC(0x6863, 0x6864); L = alu_inc8(gb, L);
  CYC(0x6864, 0x6865); mem_wr(gb, HL, A);

nextCrystal:
  CYC(0x6865, 0x6867); A = hram_rd(gb, 0x8b);
  CYC(0x6867, 0x6868); A = alu_inc8(gb, A);
  CYC(0x6868, 0x686a); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(0x686a, 0x686c); goto loop; }
  CYC(0x686a, 0x686c); ret_effect(gb);
}
