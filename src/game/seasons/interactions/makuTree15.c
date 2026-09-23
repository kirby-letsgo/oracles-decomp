#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_MAKU_TREE), bank 0x15.

// Sets wTmpcfc0 when Link's sword (not a companion) hits the gate.
void s_makuTree_checkGateHit_hook(GB *gb) {
  BASE(makuTree_checkGateHit);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1WeaponItem_id);
  CYC(b_+3, b_+5); alu_cp(gb, 0x05); // ITEM_SWORD
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wcc63);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CALL_C(b_+11, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+14);
  if (!(F & FC)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wTmpcfc0, A);
  RET(b_+20); return;
}

static void maku_add_a_to_hl(GB *gb, uint16_t return_address) {
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

void s_makuTree_setMakuMapText_hook(GB *gb) {
  BASE(makuTree_setMakuMapText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wMakuMapTextPresent);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  RET(b_+4); return;
}

// makuTree_showText from its +3: TX_17xx with the low byte in c.
static void maku_show_text_tail(GB *gb) {
  BASE(makuTree_showText);
  CYC(b_+3, b_+5); B = 0x17;
  CYC(b_+5, b_+8);
  TAIL(showText);
}

void s_makuTree_showTextBasedOnVar_hook(GB *gb) {
  BASE(makuTree_showTextBasedOnVar);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYCT(b_+1, b_+3); maku_show_text_tail(gb); return;
}

void s_makuTree_showTextAndSetMapTextBasedOnStage_hook(GB *gb) {
  BASE(makuTree_showTextAndSetMapTextBasedOnStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_makuTree_setMapTextBasedOnStage_hook, SYM(makuTree_setMapTextBasedOnStage), b_+3);
  CYCT(b_+3, b_+5); maku_show_text_tail(gb); return;
}

void s_makuTree_showTextAndSetMapText_hook(GB *gb) {
  BASE(makuTree_showTextAndSetMapText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_makuTree_setMapText_hook, SYM(makuTree_setMapText), b_+3);
  CYCT(b_+3, b_+5); maku_show_text_tail(gb); return;
}

// The map text for the current stage (ws_cc39) from makuTreeTextIndices.
void s_makuTree_setMapTextBasedOnStage_hook(GB *gb) {
  BASE(makuTree_setMapTextBasedOnStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, ws_cc39);
  CYC(b_+3, b_+6); SET_HL(SYM(makuTreeTextIndices));
  CYC(b_+6, b_+7); maku_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  TAIL_S(makuTree_setMapText);
}

void s_makuTree_setMapText_hook(GB *gb) {
  BASE(makuTree_setMapText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_makuTree_add1bToLowTextIfLinked_hook, SYM(makuTree_add1bToLowTextIfLinked), b_+3);
  CYC(b_+3, b_+6); SET_HL(wMakuMapTextPresent);
  CYC(b_+6, b_+7); mem_wr(gb, HL, C);
  RET(b_+7); return;
}

// c = a, plus $1b in a linked game.
void s_makuTree_add1bToLowTextIfLinked_hook(GB *gb) {
  BASE(makuTree_add1bToLowTextIfLinked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CALL_C(b_+1, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+4);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = C;
  CYC(b_+6, b_+8); alu_add(gb, 0x1b);
  CYC(b_+8, b_+9); C = A;
  RET(b_+9); return;
}

// var37 = a, with the bubble spawned when a is 0; then animation a.
void s_makuTree_storeIntoVar37SpawnBubbleIf0_hook(GB *gb) {
  BASE(makuTree_storeIntoVar37SpawnBubbleIf0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto store; }
  CYC(b_+2, b_+4);
  CALL_C(b_+4, s_makuTree_spawnBubble_hook, SYM(makuTree_spawnBubble), b_+7);
  CYC(b_+7, b_+9); A = 0x00;
store:
  CYC(b_+9, b_+11); E = 0x77; // Interaction.var37
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15);
  TAIL(interactionSetAnimation);
}

// The gnarled key at y $60 and the x nearest Link ($40, $50 or $60), remembered in ws_c6e0.
void s_makuTree_dropGnarledKey_hook(GB *gb) {
  BASE(makuTree_dropGnarledKey);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60); // INTERAC_TREASURE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x42); // TREASURE_GNARLED_KEY
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+14); L = 0x4b; // Interaction.yh
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x60);
  CYC(b_+16, b_+19); A = mem_rd(gb, w1Link_xh);
  CYC(b_+19, b_+21); B = 0x50;
  CYC(b_+21, b_+23); alu_cp(gb, 0x64);
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto place; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0x3c);
  if (F & FC) { CYCT(b_+27, b_+29); goto place; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+31); B = 0x40;
  CYC(b_+31, b_+33); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+33, b_+35); goto place; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); B = 0x60;
place:
  CYC(b_+37, b_+39); L = 0x4d; // Interaction.xh
  CYC(b_+39, b_+40); mem_wr(gb, HL, B);
  CYC(b_+40, b_+41); A = B;
  CYC(b_+41, b_+44); mem_wr(gb, ws_c6e0, A);
  RET(b_+44); return;
}

// The Maku tree bubble enemy (subid = this object's), linked both ways; clears wTmpcfc0 bit 7.
void s_makuTree_spawnBubble_hook(GB *gb) {
  BASE(makuTree_spawnBubble);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x56); // ENEMY_MAKU_TREE_BUBBLE
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); E = 0x42; // Interaction.subid
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); L = 0x98; // Enemy.relatedObj2
  CYC(b_+13, b_+15); A = 0x40;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); mem_wr(gb, HL, D);
  CYC(b_+17, b_+19); E = 0x56; // Interaction.relatedObj1
  CYC(b_+19, b_+21); A = 0x80;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); E = alu_inc8(gb, E);
  CYC(b_+23, b_+24); A = H;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); SET_HL(wTmpcfc0);
  CYC(b_+28, b_+30); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  RET(b_+30); return;
}

void s_makuTree_dropMakuSeed_hook(GB *gb) {
  BASE(makuTree_dropMakuSeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x9301); // INTERAC_MAKU_SEED, subid 1
  CYC(b_+3, b_+6);
  TAIL(objectCreateInteraction);
}

void s_makuTree_OnoxTauntingAfterMakuSeedGet_hook(GB *gb) {
  BASE(makuTree_OnoxTauntingAfterMakuSeedGet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0e; // CUTSCENE_S_ONOX_TAUNTING
  CYC(b_+2, b_+5); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+5, b_+7); A = 0x19; // GLOBALFLAG_GOT_MAKU_SEED
  CYC(b_+7, b_+10);
  TAIL(setGlobalFlag);
}

void s_makuTree_disableEverythingIfUnlinked_hook(GB *gb) {
  BASE(makuTree_disableEverythingIfUnlinked);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+8, b_+11); mem_wr(gb, wDisabledObjects, A);
  RET(b_+11); return;
}

// Unless warping in, Link facing up at ($68,$50) on the ground in the forced state.
void s_seasonsFunc_15_619a_hook(GB *gb) {
  BASE(seasonsFunc_15_619a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wWarpDestPos);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CALL_C(b_+5, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+8);
  CYC(b_+8, b_+11); SET_HL(w1Link_direction);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(b_+13, b_+15); L = 0x0b; // w1Link.yh
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x68);
  CYC(b_+17, b_+19); L = 0x0d; // w1Link.xh
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x50);
  CYC(b_+21, b_+23); L = 0x0f; // w1Link.zh
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x00);
  RET(b_+25); return;
}
