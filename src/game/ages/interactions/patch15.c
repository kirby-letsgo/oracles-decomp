#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void patch_setStairTile_hook(GB *gb);

void patch_jump_hook(GB *gb) {
  CYC(0x7498, 0x7499); H = D;
  CYC(0x7499, 0x749b); L = 0x54;
  CYC(0x749b, 0x749d); A = 0x80;
  CYC(0x749d, 0x749e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x749e, 0x74a0); mem_wr(gb, HL, 0xfe);
  CYC(0x74a0, 0x74a2); A = 0x01;
  CYC(0x74a2, 0x74a5); mem_wr(gb, wDisabledObjects, A);
  CYC(0x74a5, 0x74a8); mem_wr(gb, wMenuDisabled, A);
  CYC(0x74a8, 0x74ab); mem_wr(gb, wOamEnd, A);
  CYC(0x74ab, 0x74ad); A = 0x8f;
  CYC(0x74ad, 0x74b0); playSound_b00_hook(gb);
}

void patch_updateTextSubstitution_hook(GB *gb) {
  CYC(0x74b0, 0x74b3); A = mem_rd(gb, wTmpcfc0_patchMinigame_itemNameText);
  CYC(0x74b3, 0x74b6); mem_wr(gb, wTextSubstitutions, A);
  CYC(0x74b6, 0x74b7); ret_effect(gb);
}

void patch_restoreControlAndStairs_hook(GB *gb) {
  CYC(0x74b7, 0x74b8); alu_xor(gb, A);
  CYC(0x74b8, 0x74bb); mem_wr(gb, wDisabledObjects, A);
  CYC(0x74bb, 0x74be); mem_wr(gb, wMenuDisabled, A);
  CYC(0x74be, 0x74c0); A = 0x44;
  patch_setStairTile_hook(gb);
}

void patch_setStairTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74c0, 0x74c2); C = 0x49;
  CALL_C(0x74c2, setTile_hook, 0x3a9c, 0x74c5);
  CALL_C(0x74c5, getFreeInteractionSlot_hook, 0x3aef, 0x74c8);
  if (!(F & FZ)) { RET_TAKEN(0x74c8); return; }
  CYC(0x74c8, 0x74c9);
  CYC(0x74c9, 0x74cb); mem_wr(gb, HL, 0x05);
  CYC(0x74cb, 0x74cd); L = 0x4b;
  CYC(0x74cd, 0x74cf); mem_wr(gb, HL, 0x48);
  CYC(0x74cf, 0x74d1); L = 0x4d;
  CYC(0x74d1, 0x74d3); mem_wr(gb, HL, 0x98);
  CYC(0x74d3, 0x74d4); ret_effect(gb);
}

void patch_moveLinkPositionAtMinigameEnd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74d4, 0x74d5); push_effect(gb, DE);
  CALL_C(0x74d5, clearAllItemsAndPutLinkOnGround_hook, 0x19ad, 0x74d8);
  CYC(0x74d8, 0x74d9); SET_DE(pop_effect(gb));
  CALL_C(0x74d9, setLinkForceStateToState08_hook, 0x2aad, 0x74dc);
  CALL_C(0x74dc, resetLinkInvincibility_hook, 0x2ba9, 0x74df);
  CYC(0x74df, 0x74e1); L = 0x0b;
  CYC(0x74e1, 0x74e3); mem_wr(gb, HL, 0x48);
  CYC(0x74e3, 0x74e5); L = 0x0d;
  CYC(0x74e5, 0x74e7); mem_wr(gb, HL, 0x78);
  CYC(0x74e7, 0x74e9); L = 0x08;
  CYC(0x74e9, 0x74ea); mem_wr(gb, HL, A);
  CYC(0x74ea, 0x74eb); A = alu_inc8(gb, A);
  CYC(0x74eb, 0x74ee); mem_wr(gb, 0xcfd6, A);
  CYC(0x74ee, 0x74f1); resetCamera_hook(gb);
}

void patch_turnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x74f1, objectGetAngleTowardLink_hook, 0x1e9c, 0x74f4);
  CYC(0x74f4, 0x74f6); alu_add(gb, 0x04);
  CYC(0x74f6, 0x74f8); alu_and(gb, 0x18);
  CYC(0x74f8, 0x74fa); alu_swap_a(gb);
  CYC(0x74fa, 0x74fb); alu_rlca(gb);
  CYC(0x74fb, 0x74fd); E = 0x48;
  CYC(0x74fd, 0x74fe); mem_wr(gb, HL, A);
  CYC(0x74fe, 0x7501); interactionSetAnimation_hook(gb);
}
