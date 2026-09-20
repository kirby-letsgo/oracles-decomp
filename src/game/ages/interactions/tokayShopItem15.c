#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void tokayShopItem_giveFeatherAndLoseShovel_hook(GB *gb);
void tokayShopItem_giveBraceletAndLoseShovel_hook(GB *gb);
void tokayShopItem_giveShovelAndLoseFeather_hook(GB *gb);
void tokayShopItem_giveShovelAndLoseBracelet_hook(GB *gb);
void tokayShopItem_giveAndLoseTreasure_hook(GB *gb);
void tokayShopItem_giveShieldToLink_hook(GB *gb);
void tokayShopItem_giveBraceletToLink_hook(GB *gb);
void tokayShopItem_giveFeatherToLink_hook(GB *gb);
void tokayShopItem_createTreasureAtLink_hook(GB *gb);
void tokayShopItem_lose10ScentSeeds_hook(GB *gb);
void tokayShopItem_lose10MysterySeeds_hook(GB *gb);

void tokayShopItem_giveFeatherAndLoseShovel_hook(GB *gb) {
  CYC(0x6f3d, 0x6f3f); C = 0x02;
  CYC(0x6f3f, 0x6f41); A = 0x15;
  CYC(0x6f41, 0x6f43); tokayShopItem_giveAndLoseTreasure_hook(gb);
}

void tokayShopItem_giveBraceletAndLoseShovel_hook(GB *gb) {
  CYC(0x6f43, 0x6f45); C = 0x03;
  CYC(0x6f45, 0x6f47); A = 0x15;
  CYC(0x6f47, 0x6f49); tokayShopItem_giveAndLoseTreasure_hook(gb);
}

static void tokayShopItem_finishGiveShovel(GB *gb) {
  CYC(0x6f4f, 0x6f51); E = 0x7c;
  CYC(0x6f51, 0x6f53); A = 0x15;
  CYC(0x6f53, 0x6f54); mem_wr(gb, DE, A);
  CYC(0x6f54, 0x6f56); C = 0x02;
  CYC(0x6f56, 0x6f57); A = B;
  tokayShopItem_giveAndLoseTreasure_hook(gb);
}

void tokayShopItem_giveShovelAndLoseFeather_hook(GB *gb) {
  CYC(0x6f49, 0x6f4b); B = 0x17;
  CYC(0x6f4b, 0x6f4d); tokayShopItem_finishGiveShovel(gb);
}

void tokayShopItem_giveShovelAndLoseBracelet_hook(GB *gb) {
  CYC(0x6f4d, 0x6f4f); B = 0x16;
  tokayShopItem_finishGiveShovel(gb);
}

void tokayShopItem_giveAndLoseTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f57, 0x6f59); E = 0x7b;
  CYC(0x6f59, 0x6f5a); mem_wr(gb, DE, A);
  CALL_C(0x6f5a, tokayShopItem_createTreasureAtLink_hook, 0x6f77, 0x6f5d);
  CYC(0x6f5d, 0x6f5f); E = 0x7b;
  CYC(0x6f5f, 0x6f60); A = mem_rd(gb, DE);
  CALL_C(0x6f60, loseTreasure_hook, 0x1733, 0x6f63);
  CYC(0x6f63, 0x6f64); ret_effect(gb);
}

void tokayShopItem_giveShieldToLink_hook(GB *gb) {
  CYC(0x6f64, 0x6f66); E = 0x7c;
  CYC(0x6f66, 0x6f68); A = 0x01;
  CYC(0x6f68, 0x6f69); mem_wr(gb, DE, A);
  CYC(0x6f69, 0x6f6b); E = 0x42;
  CYC(0x6f6b, 0x6f6c); A = mem_rd(gb, DE);
  CYC(0x6f6c, 0x6f6e); alu_sub(gb, 0x04);
  CYC(0x6f6e, 0x6f6f); C = A;
  CYC(0x6f6f, 0x6f71); tokayShopItem_createTreasureAtLink_hook(gb);
}

void tokayShopItem_giveBraceletToLink_hook(GB *gb) {
  CYC(0x6f71, 0x6f73); C = 0x03;
  CYC(0x6f73, 0x6f75); tokayShopItem_createTreasureAtLink_hook(gb);
}

void tokayShopItem_giveFeatherToLink_hook(GB *gb) {
  CYC(0x6f75, 0x6f77); C = 0x02;
  tokayShopItem_createTreasureAtLink_hook(gb);
}

void tokayShopItem_createTreasureAtLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6f77, 0x6f79); E = 0x7c;
  CYC(0x6f79, 0x6f7a); A = mem_rd(gb, DE);
  CYC(0x6f7a, 0x6f7b); B = A;
  CALL_C(0x6f7b, createTreasure_hook, 0x27d4, 0x6f7e);
  CYC(0x6f7e, 0x6f80); L = 0x4b;
  CYC(0x6f80, 0x6f83); A = mem_rd(gb, w1Link_yh);
  CYC(0x6f83, 0x6f84); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6f84, 0x6f85); L = alu_inc8(gb, L);
  CYC(0x6f85, 0x6f88); A = mem_rd(gb, w1Link_xh);
  CYC(0x6f88, 0x6f89); mem_wr(gb, HL, A);
  CYC(0x6f89, 0x6f8a); ret_effect(gb);
}

static void tokayShopItem_loseSeedsTail(GB *gb) {
  CYC(0x6f90, 0x6f92); H = 0xc6;
  CYC(0x6f92, 0x6f93); A = mem_rd(gb, HL);
  CYC(0x6f93, 0x6f95); alu_sub(gb, 0x10);
  CYC(0x6f95, 0x6f96); alu_daa(gb);
  CYC(0x6f96, 0x6f97); mem_wr(gb, HL, A);
  CYC(0x6f97, 0x6f99); A = 0xff;
  CYC(0x6f99, 0x6f9c); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(0x6f9c, 0x6f9d); ret_effect(gb);
}

void tokayShopItem_lose10ScentSeeds_hook(GB *gb) {
  CYC(0x6f8a, 0x6f8c); L = 0xba;
  CYC(0x6f8c, 0x6f8e); tokayShopItem_loseSeedsTail(gb);
}

void tokayShopItem_lose10MysterySeeds_hook(GB *gb) {
  CYC(0x6f8e, 0x6f90); L = 0xbd;
  tokayShopItem_loseSeedsTail(gb);
}
