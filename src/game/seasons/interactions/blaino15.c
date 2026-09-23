#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_BLAINO_SCRIPT), bank 0x15: the
// helpers of Blaino's boxing minigame script.

// var38 = the rupee value to bet (higher after cheating), var37 = whether Link can afford it.
void s_blainoScript_saveVariables_hook(GB *gb) {
  BASE(blainoScript_saveVariables);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x1a; // GLOBALFLAG_CHEATED_BLAINO
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+7); A = 0x04;
  if (F & FZ) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x05;
store:
  CYC(b_+11, b_+13); E = 0x78; // Interaction.var38
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CALL_C(b_+14, cpRupeeValue_hook, SYM(cpRupeeValue), b_+17);
  CYC(b_+17, b_+19); E = 0x77; // Interaction.var37
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x00;
  CYC(b_+22, b_+25); mem_wr(gb, wShopHaveEnoughRupees + 1, A);
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+28); E = 0x71; // Interaction.var31
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = 0x44; // Interaction.state
  CYC(b_+31, b_+33); A = 0x01;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return;
}

void s_blainoScript_takeRupees_hook(GB *gb) {
  BASE(blainoScript_takeRupees);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x78; // Interaction.var38
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6);
  TAIL(removeRupeeValue);
}

void s_blainoScript_adjustRupeesInText_hook(GB *gb) {
  BASE(blainoScript_adjustRupeesInText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x78; // Interaction.var38
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CALL_C(b_+3, getRupeeValue_hook, SYM(getRupeeValue), b_+6);
  CYC(b_+6, b_+9); SET_HL(wTextNumberSubstitution);
  CYC(b_+9, b_+10); mem_wr(gb, HL, C);
  CYC(b_+10, b_+11); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, B);
  RET(b_+12); return;
}

void s_blainoScript_give30Rupees_hook(GB *gb) {
  BASE(blainoScript_give30Rupees);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x07; // RUPEEVAL_030
  CYC(b_+2, b_+4); A = 0x28; // TREASURE_RUPEES
  CYC(b_+4, b_+7);
  TAIL(giveTreasure);
}

void s_blainoScript_clearItemsAndPegasusSeeds_hook(GB *gb) {
  BASE(blainoScript_clearItemsAndPegasusSeeds);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearPegasusSeedCounter_hook, SYM(clearPegasusSeedCounter), b_+3);
  CALL_C(b_+3, clearAllParentItems_hook, SYM(clearAllParentItems), b_+6);
  CALL_C(b_+6, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+9);
  CYC(b_+9, b_+12);
  TAIL(clearItems);
}

// Link facing left at ($40,$60) on the ground, in the forced state.
void s_blainoScript_setLinkPositionAndState_hook(GB *gb) {
  BASE(blainoScript_setLinkPositionAndState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link_direction);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x03); // DIR_LEFT
  CYC(b_+8, b_+10); L = 0x0b; // w1Link.yh
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x40);
  CYC(b_+12, b_+14); L = 0x0d; // w1Link.xh
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x60);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+19); L = 0x0f; // w1Link.zh
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+23); mem_wr(gb, wLinkInAir, A);
  RET(b_+23); return;
}

// Puts the Blaino interaction (slot in var39) in state 2 and spawns the Blaino enemy at
// ($40,$40) linked to it.
void s_blainoScript_spawnBlainoEnemy_hook(GB *gb) {
  BASE(blainoScript_spawnBlainoEnemy);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x79; // Interaction.var39
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = 0x44; // Interaction.state
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x02);
  CALL_C(b_+8, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x54); // ENEMY_BLAINO
  CYC(b_+14, b_+16); L = 0x8b; // Enemy.yh
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x40);
  CYC(b_+18, b_+20); L = 0x8d; // Enemy.xh
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x40);
  CYC(b_+22, b_+24); E = 0x56; // Interaction.relatedObj1
  CYC(b_+24, b_+26); A = 0x80;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);
  CYC(b_+28, b_+29); A = H;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  RET(b_+30); return;
}

// Clears the enemies, spawns Blaino at ($40,$40) and moves this object onto him.
void s_blainoScript_setBlainoPosition_hook(GB *gb) {
  BASE(blainoScript_setBlainoPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CALL_C(b_+1, clearEnemies_hook, SYM(clearEnemies), b_+4);
  CYC(b_+4, b_+5); SET_DE(pop_effect(gb));
  CYC(b_+5, b_+8); SET_BC(0x4040);
  CALL_C(b_+8, s_spawnBlainoAtPosition_hook, SYM(spawnBlainoAtPosition), b_+11);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); L = 0x4b; // Interaction.yh
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  CYC(b_+15, b_+17); L = 0x4d; // Interaction.xh
  CYC(b_+17, b_+18); C = mem_rd(gb, HL);
  CYC(b_+18, b_+20); E = 0x4b; // Interaction.yh
  CYC(b_+20, b_+21); A = B;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = 0x4d; // Interaction.xh
  CYC(b_+24, b_+25); A = C;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return;
}

void s_blainoScript_spawnBlaino_hook(GB *gb) {
  BASE(blainoScript_spawnBlaino);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x4050);
  TAIL_S(spawnBlainoAtPosition);
}

// The Blaino interaction at (b,c); its slot goes in var39. Z on success.
void s_spawnBlainoAtPosition_hook(GB *gb) {
  BASE(spawnBlainoAtPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x72); // INTERAC_BLAINO
  CYC(b_+6, b_+8); L = 0x4b; // Interaction.yh
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+11); L = 0x4d; // Interaction.xh
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+14); E = 0x79; // Interaction.var39
  CYC(b_+14, b_+15); A = H;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  RET(b_+17); return;
}

// putAwayLinksItems@saveItemToB: b = the old equipped item at hl, replaced by a.
static void blaino_save_item(GB *gb) {
  BASE(putAwayLinksItems);
  CYC(b_+44, b_+45); B = mem_rd(gb, HL);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  RET(b_+46); return;
}

// putAwayLinksItems@storeItemInInventory: b into the first free inventory storage slot, whose
// low address byte goes to (de).
static void blaino_store_item(GB *gb) {
  BASE(putAwayLinksItems);
  CYC(b_+47, b_+48); push_effect(gb, DE);
  CYC(b_+48, b_+50); D = 0xcf;
  CYC(b_+50, b_+52); L = 0x82; // wInventoryStorage
  for (;;) {
    CYC(b_+52, b_+53); A = mem_rd(gb, HL);
    CYC(b_+53, b_+54); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+54, b_+56); break; }
    CYC(b_+54, b_+56);
    CYC(b_+56, b_+57); L = alu_inc8(gb, L);
    CYCT(b_+57, b_+59);
  }
  CYC(b_+59, b_+60); mem_wr(gb, HL, B);
  CYC(b_+60, b_+61); A = L;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+63); SET_DE(pop_effect(gb));
  RET(b_+63); return;
}

// Replaces the equipped B and A items with a's low and high nibbles, storing the old ones in
// the inventory (their slots in wTmpcfc0+$1f/$1e), and disables the active ring.
void s_putAwayLinksItems_hook(GB *gb) {
  BASE(putAwayLinksItems);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, hFF8B, A);
  CYC(b_+2, b_+4); A = 0xff;
  CYC(b_+4, b_+7); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+7, b_+10); SET_HL(wInventoryB);
  CYC(b_+10, b_+12); E = 0xdf;
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8B);
  CYC(b_+14, b_+16); alu_and(gb, 0x0f);
  CALL_L(b_+16, blaino_save_item, b_+19);
  CALL_L(b_+19, blaino_store_item, b_+22);
  CYC(b_+22, b_+24); L = 0x81; // wInventoryA
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF8B);
  CYC(b_+26, b_+28); A = alu_swap(gb, A);
  CYC(b_+28, b_+30); alu_and(gb, 0x0f);
  CYC(b_+30, b_+32); E = 0xde;
  CALL_L(b_+32, blaino_save_item, b_+35);
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+38); alu_cp(gb, 0x0c); // ITEM_BIGGORON_SWORD
  if (!(F & FZ)) CALL_L_CC(b_+38, blaino_store_item, b_+41);
  else CYC(b_+38, b_+41);
  CYC(b_+41, b_+44);
  TAIL(disableActiveRing);
}

// seasonsFunc_15_5cf7 from its +24: re-enable the active ring.
static void blaino_enable_ring(GB *gb) {
  BASE(seasonsFunc_15_5cf7);
  CYC(b_+24, b_+27);
  TAIL(enableActiveRing);
}

// Restores Link's items unless the shop check left 3 in wShopHaveEnoughRupees.
void s_seasonsFunc_15_5cf0_hook(GB *gb) {
  BASE(seasonsFunc_15_5cf0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wShopHaveEnoughRupees);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+5, b_+7); blaino_enable_ring(gb); return; }
  CYC(b_+5, b_+7);
  TAIL_S(seasonsFunc_15_5cf7);
}

// Moves the stored items (slots in wTmpcfc0+$1f/$1e) back to the B and A items.
void s_seasonsFunc_15_5cf7_hook(GB *gb) {
  BASE(seasonsFunc_15_5cf7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); A = 0xff;
  CYC(b_+3, b_+6); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+6, b_+8); H = 0xc6; // wc600Block
  CYC(b_+8, b_+11); SET_DE(wTmpcfc0 + 0x1f);
  CYC(b_+11, b_+13); C = 0x80; // wInventoryB
  CALL_C(b_+13, s_seasonsFunc_15_5d12_hook, SYM(seasonsFunc_15_5d12), b_+16);
  CYC(b_+16, b_+18); E = 0xde;
  CYC(b_+18, b_+20); C = 0x81; // wInventoryA
  CALL_C(b_+20, s_seasonsFunc_15_5d12_hook, SYM(seasonsFunc_15_5d12), b_+23);
  CYC(b_+23, b_+24); SET_DE(pop_effect(gb));
  blaino_enable_ring(gb); return;
}

// Moves the item in the storage slot named by (de) to the equipped slot c (both in $c6xx);
// the Biggoron sword takes both slots.
void s_seasonsFunc_15_5d12_hook(GB *gb) {
  BASE(seasonsFunc_15_5d12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+2); return; }
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+4); L = A;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x00);
  CYC(b_+7, b_+8); L = C;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0x0c); // ITEM_BIGGORON_SWORD
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  RET(b_+13); return;
}
