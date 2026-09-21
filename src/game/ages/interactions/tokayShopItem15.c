#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(tokayShopItem_giveFeatherAndLoseShovel);
  CYC(b_+0, b_+2); C = 0x02;
  CYC(b_+2, b_+4); A = 0x15;
  CYC(b_+4, b_+6); tokayShopItem_giveAndLoseTreasure_hook(gb);
}

void tokayShopItem_giveBraceletAndLoseShovel_hook(GB *gb) {
  BASE(tokayShopItem_giveBraceletAndLoseShovel);
  CYC(b_+0, b_+2); C = 0x03;
  CYC(b_+2, b_+4); A = 0x15;
  CYC(b_+4, b_+6); tokayShopItem_giveAndLoseTreasure_hook(gb);
}

static void tokayShopItem_finishGiveShovel(GB *gb) {
  BASE(tokayShopItem_giveShovelAndLoseBracelet);
  CYC(b_+2, b_+4); E = 0x7c;
  CYC(b_+4, b_+6); A = 0x15;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); C = 0x02;
  CYC(b_+9, b_+10); A = B;
  tokayShopItem_giveAndLoseTreasure_hook(gb);
}

void tokayShopItem_giveShovelAndLoseFeather_hook(GB *gb) {
  BASE(tokayShopItem_giveShovelAndLoseFeather);
  CYC(b_+0, b_+2); B = 0x17;
  CYC(b_+2, b_+4); tokayShopItem_finishGiveShovel(gb);
}

void tokayShopItem_giveShovelAndLoseBracelet_hook(GB *gb) {
  BASE(tokayShopItem_giveShovelAndLoseBracelet);
  CYC(b_+0, b_+2); B = 0x16;
  tokayShopItem_finishGiveShovel(gb);
}

void tokayShopItem_giveAndLoseTreasure_hook(GB *gb) {
  BASE(tokayShopItem_giveAndLoseTreasure);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x7b;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, tokayShopItem_createTreasureAtLink_hook, SYM(tokayShopItem_createTreasureAtLink), b_+6);
  CYC(b_+6, b_+8); E = 0x7b;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CALL_C(b_+9, loseTreasure_hook, SYM(loseTreasure), b_+12);
  CYC(b_+12, b_+13); ret_effect(gb);
}

void tokayShopItem_giveShieldToLink_hook(GB *gb) {
  BASE(tokayShopItem_giveShieldToLink);
  CYC(b_+0, b_+2); E = 0x7c;
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = 0x42;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_sub(gb, 0x04);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+13); tokayShopItem_createTreasureAtLink_hook(gb);
}

void tokayShopItem_giveBraceletToLink_hook(GB *gb) {
  BASE(tokayShopItem_giveBraceletToLink);
  CYC(b_+0, b_+2); C = 0x03;
  CYC(b_+2, b_+4); tokayShopItem_createTreasureAtLink_hook(gb);
}

void tokayShopItem_giveFeatherToLink_hook(GB *gb) {
  BASE(tokayShopItem_giveFeatherToLink);
  CYC(b_+0, b_+2); C = 0x02;
  TAIL(tokayShopItem_createTreasureAtLink);
}

void tokayShopItem_createTreasureAtLink_hook(GB *gb) {
  BASE(tokayShopItem_createTreasureAtLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x7c;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, createTreasure_hook, SYM(createTreasure), b_+7);
  CYC(b_+7, b_+9); L = 0x4b;
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link_yh);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_xh);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

static void tokayShopItem_loseSeedsTail(GB *gb) {
  BASE(tokayShopItem_lose10MysterySeeds);
  CYC(b_+2, b_+4); H = 0xc6;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_sub(gb, 0x10);
  CYC(b_+7, b_+8); alu_daa(gb);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); A = 0xff;
  CYC(b_+11, b_+14); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void tokayShopItem_lose10ScentSeeds_hook(GB *gb) {
  BASE(tokayShopItem_lose10ScentSeeds);
  CYC(b_+0, b_+2); L = 0xba;
  CYC(b_+2, b_+4); tokayShopItem_loseSeedsTail(gb);
}

void tokayShopItem_lose10MysterySeeds_hook(GB *gb) {
  BASE(tokayShopItem_lose10MysterySeeds);
  CYC(b_+0, b_+2); L = 0xbd;
  tokayShopItem_loseSeedsTail(gb);
}
