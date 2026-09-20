#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void patch_setStairTile_hook(GB *gb);

void patch_jump_hook(GB *gb) {
  BASE(patch_jump);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x54;
  CYC(b_+3, b_+5); A = 0x80;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xfe);
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+13); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+13, b_+16); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+16, b_+19); mem_wr(gb, wOamEnd, A);
  CYC(b_+19, b_+21); A = 0x8f;
  CYC(b_+21, b_+24); playSound_b00_hook(gb);
}

void patch_updateTextSubstitution_hook(GB *gb) {
  BASE(patch_updateTextSubstitution);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_patchMinigame_itemNameText);
  CYC(b_+3, b_+6); mem_wr(gb, wTextSubstitutions, A);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void patch_restoreControlAndStairs_hook(GB *gb) {
  BASE(patch_restoreControlAndStairs);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+4, b_+7); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+7, b_+9); A = 0x44;
  patch_setStairTile_hook(gb);
}

void patch_setStairTile_hook(GB *gb) {
  BASE(patch_setStairTile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x49;
  CALL_C(b_+2, setTile_hook, SYM(setTile), b_+5);
  CALL_C(b_+5, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x05);
  CYC(b_+11, b_+13); L = 0x4b;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x48);
  CYC(b_+15, b_+17); L = 0x4d;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x98);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void patch_moveLinkPositionAtMinigameEnd_hook(GB *gb) {
  BASE(patch_moveLinkPositionAtMinigameEnd);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CALL_C(b_+1, clearAllItemsAndPutLinkOnGround_hook, SYM(clearAllItemsAndPutLinkOnGround), b_+4);
  CYC(b_+4, b_+5); SET_DE(pop_effect(gb));
  CALL_C(b_+5, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+8);
  CALL_C(b_+8, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+11);
  CYC(b_+11, b_+13); L = 0x0b;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x48);
  CYC(b_+15, b_+17); L = 0x0d;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x78);
  CYC(b_+19, b_+21); L = 0x08;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  CYC(b_+23, b_+26); mem_wr(gb, wTmpcfc0_bigBangGame_prizeIndex, A);
  CYC(b_+26, b_+29); resetCamera_hook(gb);
}

void patch_turnToFaceLink_hook(GB *gb) {
  BASE(patch_turnToFaceLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); alu_swap_a(gb);
  CYC(b_+9, b_+10); alu_rlca(gb);
  CYC(b_+10, b_+12); E = 0x48;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16); interactionSetAnimation_hook(gb);
}
