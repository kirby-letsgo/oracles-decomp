#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactiondc_removeGraveyardGateTiles1), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactiondc_removeGraveyardGateTiles1), (from), (to), true)

void interactiondc_7bde_hook(GB *gb);
void interactiondc_spawnPuff_hook(GB *gb);

void interactiondc_removeGraveyardGateTiles1_hook(GB *gb) {
  BASE(interactiondc_removeGraveyardGateTiles1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0a;
  CALL_C(b_+2, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+5);
  CYC(b_+5, b_+7); A = 0x3a;
  CYC(b_+7, b_+9); C = 0x34;
  CALL_C(b_+9, setTile_hook, SYM(setTile), b_+12);
  CYC(b_+12, b_+14); A = 0x3a;
  CYC(b_+14, b_+16); C = 0x44;
  CALL_C(b_+16, setTile_hook, SYM(setTile), b_+19);
  CYC(b_+19, b_+22); SET_HL(b_+46);
  CALL_C(b_+22, interactiondc_7bde_hook, SYM(interactiondc_7bde), b_+25);
  CALL_C(b_+25, interactiondc_7bde_hook, SYM(interactiondc_7bde), b_+28);
  CALL_C(b_+28, interactiondc_7bde_hook, SYM(interactiondc_7bde), b_+31);
  CALL_C(b_+31, interactiondc_7bde_hook, SYM(interactiondc_7bde), b_+34);
  CYC(b_+34, b_+37); SET_BC((SYM(group2ObjectDataTable) + 261));
  CALL_C(b_+37, interactiondc_spawnPuff_hook, SYM(interactiondc_spawnPuff), b_+40);
  CYC(b_+40, b_+43); SET_BC((SYM(group2ObjectDataTable) + 277));
  CYC(b_+43, b_+46); interactiondc_spawnPuff_hook(gb);
}

void interactiondc_removeGraveyardGateTiles2_hook(GB *gb) {
  BASE(interactiondc_removeGraveyardGateTiles2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x0a;
  CALL_C(b_+2, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+5);
  CYC(b_+5, b_+7); A = 0x3a;
  CYC(b_+7, b_+9); C = 0x33;
  CALL_C(b_+9, setTile_hook, SYM(setTile), b_+12);
  CYC(b_+12, b_+14); A = 0x3a;
  CYC(b_+14, b_+16); C = 0x35;
  CALL_C(b_+16, setTile_hook, SYM(setTile), b_+19);
  CYC(b_+19, b_+21); A = 0x3a;
  CYC(b_+21, b_+23); C = 0x43;
  CALL_C(b_+23, setTile_hook, SYM(setTile), b_+26);
  CYC(b_+26, b_+28); A = 0x3a;
  CYC(b_+28, b_+30); C = 0x45;
  CALL_C(b_+30, setTile_hook, SYM(setTile), b_+33);
  CYC(b_+33, b_+36); SET_BC((SYM(group2ObjectDataTable) + 245));
  CALL_C(b_+36, interactiondc_spawnPuff_hook, SYM(interactiondc_spawnPuff), b_+39);
  CYC(b_+39, b_+42); SET_BC((SYM(group2ObjectDataTable) + 293));
  CYC(b_+42, SYM(interactiondc_7bde)); interactiondc_spawnPuff_hook(gb);
}

void interactiondc_7bde_hook(GB *gb) {
  BASE(interactiondc_7bde);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+3); hram_wr(gb, 0x8c, A);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); hram_wr(gb, 0x8f, A);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+9); hram_wr(gb, 0x8e, A);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); push_effect(gb, HL);
  CALL_C(b_+11, setInterleavedTile_hook, SYM(setInterleavedTile), b_+14);
  CYC(b_+14, b_+15); SET_HL(pop_effect(gb));
  CYC(b_+15, SYM(interactiondc_spawnPuff)); ret_effect(gb);
}

void interactiondc_spawnPuff_hook(GB *gb) {
  BASE(interactiondc_spawnPuff);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x05);
  CYC(b_+6, b_+8); L = 0x4b;
  CYC(b_+8, b_+9); mem_wr(gb, HL, B);
  CYC(b_+9, b_+11); L = 0x4d;
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+13); ret_effect(gb);
}
