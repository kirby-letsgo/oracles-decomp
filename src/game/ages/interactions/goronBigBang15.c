#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(goron_bigBang_initLinkPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); SET_BC((SYM(group2ObjectDataTable) + 277));
  TAIL(goron_setLinkPositionAndDirection);
}

void goron_bigBang_spawnPrize_hook(GB *gb) {
  BASE(goron_bigBang_spawnPrize);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto alreadyGotMermaidKey; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+10); goto checkSpawnPrize;

alreadyGotMermaidKey:
  CALL_C(b_+10, getRandomNumber_hook, SYM(getRandomNumber), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  CYC(b_+15, b_+18); SET_HL(b_+64);
  CYC(b_+18, b_+19); goronBigBang_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);

checkSpawnPrize:
  CYC(b_+20, b_+22); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto spawnPrize; }
  CYC(b_+22, b_+24);
  CALL_C(b_+24, getRandomNumber_hook, SYM(getRandomNumber), b_+27);
  CYC(b_+27, b_+29); alu_and(gb, 0x01);
  CYC(b_+29, b_+31); alu_add(gb, 0x04);

spawnPrize:
  CYC(b_+31, b_+34); mem_wr(gb, wTmpcfc0_bigBangGame_prizeIndex, A);
  CYC(b_+34, b_+37); SET_HL(b_+80);
  CYC(b_+37, b_+38); goronBigBang_addDoubleIndexToHl_from_rst(gb, b_+38);
  CYC(b_+38, b_+39); B = mem_rd(gb, HL);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+41); C = mem_rd(gb, HL);
  CALL_C(b_+41, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x60);
  CYC(b_+47, b_+48); L = alu_inc8(gb, L);
  CYC(b_+48, b_+49); mem_wr(gb, HL, B);
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+51); mem_wr(gb, HL, C);
  CYC(b_+51, b_+53); L = 0x4b;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x38);
  CYC(b_+55, b_+57); L = 0x4d;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x50);
  CYC(b_+59, b_+61); L = 0x4f;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0xf0);
  CYC(b_+63, b_+64); ret_effect(gb);
}

void goron_bigBang_hideSelf_hook(GB *gb) {
  BASE(goron_bigBang_hideSelf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7e;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+8); L = 0x66;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+13); objectSetInvisible_hook(gb);
}

void goron_bigBang_unhideSelf_hook(GB *gb) {
  BASE(goron_bigBang_unhideSelf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7e;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+7); A = 0x06;
  CYC(b_+7, b_+9); L = 0x66;
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+14); objectSetVisible_hook(gb);
}

void goron_bigBang_checkLinkHitByBomb_hook(GB *gb) {
  BASE(goron_bigBang_checkLinkHitByBomb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_invincibilityCounter);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CALL_C(b_+4, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+7);
  CYC(b_+7, b_+8); alu_cpl(gb);
  CYC(b_+8, b_+11); mem_wr(gb, wcddb, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void goron_bigBang_createBombSpawner_hook(GB *gb) {
  BASE(goron_bigBang_createBombSpawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x49);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0xff);
  CYC(b_+9, b_+10); ret_effect(gb);
}

void goron_createBombFlowerSprite_hook(GB *gb) {
  BASE(goron_createBombFlowerSprite);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x60);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x49);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CYC(b_+12, b_+14); L = 0x4b;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x60);
  CYC(b_+16, b_+18); L = 0x4d;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x38);
  CYC(b_+20, b_+21); ret_effect(gb);
}

void goron_createExplosionIndex_hook(GB *gb) {
  BASE(goron_createExplosionIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+22);
  CALL_C(b_+3, addDoubleIndexToBc_hook, 0x007e, b_+6);
  CALL_C(b_+6, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x56);
  CYC(b_+12, b_+14); L = 0x4b;
  CYC(b_+14, b_+15); A = mem_rd(gb, BC);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); SET_BC(BC + 1);
  CYC(b_+17, b_+19); L = 0x4d;
  CYC(b_+19, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void goron_countdownToNextExplosionGroup_hook(GB *gb) {
  BASE(goron_countdownToNextExplosionGroup);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7a;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); L = 0x7b;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+11); alu_and(gb, 0x07);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+14); mem_wr(gb, hFF8B, A);
  CYC(b_+14, b_+17); SET_BC(b_+53);
  CALL_C(b_+17, addAToBc_hook, 0x006d, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+23); L = 0x7a;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF8B);
  CYC(b_+26, b_+27); alu_add(gb, A);
  CYC(b_+27, b_+30); SET_BC(b_+61);
  CALL_C(b_+30, addDoubleIndexToBc_hook, 0x007e, b_+33);
  CYC(b_+33, b_+35); A = 0x04;

next:
  CYC(b_+35, b_+37); mem_wr(gb, hFF8D, A);
  CYC(b_+37, b_+38); A = mem_rd(gb, BC);
  CYC(b_+38, b_+40); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+40, b_+41); ret_effect(gb); return; }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); push_effect(gb, BC);
  CALL_C(b_+42, goron_createExplosionIndex_hook, SYM(goron_createExplosionIndex), b_+45);
  CYC(b_+45, b_+46); SET_BC(pop_effect(gb));
  CYC(b_+46, b_+47); SET_BC(BC + 1);
  CYC(b_+47, b_+49); A = mem_rd(gb, hFF8D);
  CYC(b_+49, b_+50); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto next; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); ret_effect(gb);
}

void goron_bigBang_loadRoomLayout_hook(GB *gb);

void goron_bigBang_loadMinigameLayout1_topHalf_hook(GB *gb) {
  BASE(goron_bigBang_loadMinigameLayout1_topHalf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goron_bigBang_minigameLayout1_topHalf));
  CYC(b_+3, b_+5); C = 0x11;
  CYC(b_+5, b_+7); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadMinigameLayout1_bottomHalf_hook(GB *gb) {
  BASE(goron_bigBang_loadMinigameLayout1_bottomHalf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goron_bigBang_minigameLayout1_bottomHalf));
  CYC(b_+3, b_+5); C = 0x41;
  CYC(b_+5, b_+7); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadMinigameLayout2_topHalf_hook(GB *gb) {
  BASE(goron_bigBang_loadMinigameLayout2_topHalf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goron_bigBang_minigameLayout2_topHalf));
  CYC(b_+3, b_+5); C = 0x11;
  CYC(b_+5, b_+7); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadMinigameLayout2_bottomHalf_hook(GB *gb) {
  BASE(goron_bigBang_loadMinigameLayout2_bottomHalf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goron_bigBang_minigameLayout2_bottomHalf));
  CYC(b_+3, b_+5); C = 0x41;
  CYC(b_+5, b_+7); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadNormalRoomLayout_topHalf_hook(GB *gb) {
  BASE(goron_bigBang_loadNormalRoomLayout_topHalf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goron_bigBang_normalRoomLayout));
  CYC(b_+3, b_+5); C = 0x11;
  CYC(b_+5, b_+7); goron_bigBang_loadRoomLayout_hook(gb);
}

void goron_bigBang_loadNormalRoomLayout_bottomHalf_hook(GB *gb) {
  BASE(goron_bigBang_loadNormalRoomLayout_bottomHalf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goron_bigBang_normalRoomLayout));
  CYC(b_+3, b_+5); C = 0x41;
  TAIL(goron_bigBang_loadRoomLayout);
}

void goron_bigBang_loadRoomLayout_hook(GB *gb) {
  BASE(goron_bigBang_loadRoomLayout);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;

nextRow:
  CYC(b_+2, b_+4); mem_wr(gb, hFF93, A);
  CYC(b_+4, b_+6); A = 0x08;

nextColumn:
  CYC(b_+6, b_+8); mem_wr(gb, hFF92, A);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CALL_C(b_+10, setTile_hook, SYM(setTile), b_+13);
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+15); C = alu_inc8(gb, C);
  CYC(b_+15, b_+17); A = mem_rd(gb, hFF92);
  CYC(b_+17, b_+18); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto nextColumn; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); A = C;
  CYC(b_+21, b_+23); alu_add(gb, 0x08);
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+26); A = mem_rd(gb, hFF93);
  CYC(b_+26, b_+27); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto nextRow; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); ret_effect(gb);
}

void goron_bigBang_blockOrRestoreExit_hook(GB *gb) {
  BASE(goron_bigBang_blockOrRestoreExit);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+19);
  CYC(b_+3, b_+4); goronBigBang_addAToHl_from_rst(gb, b_+4);
  CYC(b_+4, b_+6); C = 0x73;

next:
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CALL_C(b_+8, setTile_hook, SYM(setTile), b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); C = alu_inc8(gb, C);
  CYC(b_+13, b_+14); A = C;
  CYC(b_+14, b_+16); alu_cp(gb, 0x77);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto next; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); ret_effect(gb);
}
