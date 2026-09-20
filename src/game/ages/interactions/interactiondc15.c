#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void interactiondc_7bde_hook(GB *gb);
void interactiondc_spawnPuff_hook(GB *gb);

void interactiondc_removeGraveyardGateTiles1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b73, 0x7b75); A = 0x0a;
  CALL_C(0x7b75, setScreenShakeCounter_hook, 0x24bb, 0x7b78);
  CYC(0x7b78, 0x7b7a); A = 0x3a;
  CYC(0x7b7a, 0x7b7c); C = 0x34;
  CALL_C(0x7b7c, setTile_hook, 0x3a9c, 0x7b7f);
  CYC(0x7b7f, 0x7b81); A = 0x3a;
  CYC(0x7b81, 0x7b83); C = 0x44;
  CALL_C(0x7b83, setTile_hook, 0x3a9c, 0x7b86);
  CYC(0x7b86, 0x7b89); SET_HL(0x7ba1);
  CALL_C(0x7b89, interactiondc_7bde_hook, 0x7bde, 0x7b8c);
  CALL_C(0x7b8c, interactiondc_7bde_hook, 0x7bde, 0x7b8f);
  CALL_C(0x7b8f, interactiondc_7bde_hook, 0x7bde, 0x7b92);
  CALL_C(0x7b92, interactiondc_7bde_hook, 0x7bde, 0x7b95);
  CYC(0x7b95, 0x7b98); SET_BC(0x4840);
  CALL_C(0x7b98, interactiondc_spawnPuff_hook, 0x7bee, 0x7b9b);
  CYC(0x7b9b, 0x7b9e); SET_BC(0x4850);
  CYC(0x7b9e, 0x7ba1); interactiondc_spawnPuff_hook(gb);
}

void interactiondc_removeGraveyardGateTiles2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7bb1, 0x7bb3); A = 0x0a;
  CALL_C(0x7bb3, setScreenShakeCounter_hook, 0x24bb, 0x7bb6);
  CYC(0x7bb6, 0x7bb8); A = 0x3a;
  CYC(0x7bb8, 0x7bba); C = 0x33;
  CALL_C(0x7bba, setTile_hook, 0x3a9c, 0x7bbd);
  CYC(0x7bbd, 0x7bbf); A = 0x3a;
  CYC(0x7bbf, 0x7bc1); C = 0x35;
  CALL_C(0x7bc1, setTile_hook, 0x3a9c, 0x7bc4);
  CYC(0x7bc4, 0x7bc6); A = 0x3a;
  CYC(0x7bc6, 0x7bc8); C = 0x43;
  CALL_C(0x7bc8, setTile_hook, 0x3a9c, 0x7bcb);
  CYC(0x7bcb, 0x7bcd); A = 0x3a;
  CYC(0x7bcd, 0x7bcf); C = 0x45;
  CALL_C(0x7bcf, setTile_hook, 0x3a9c, 0x7bd2);
  CYC(0x7bd2, 0x7bd5); SET_BC(0x4830);
  CALL_C(0x7bd5, interactiondc_spawnPuff_hook, 0x7bee, 0x7bd8);
  CYC(0x7bd8, 0x7bdb); SET_BC(0x4860);
  CYC(0x7bdb, 0x7bde); interactiondc_spawnPuff_hook(gb);
}

void interactiondc_7bde_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7bde, 0x7bdf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7bdf, 0x7be1); hram_wr(gb, 0x8c, A);
  CYC(0x7be1, 0x7be2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7be2, 0x7be4); hram_wr(gb, 0x8f, A);
  CYC(0x7be4, 0x7be5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7be5, 0x7be7); hram_wr(gb, 0x8e, A);
  CYC(0x7be7, 0x7be8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7be8, 0x7be9); push_effect(gb, HL);
  CALL_C(0x7be9, setInterleavedTile_hook, 0x3acf, 0x7bec);
  CYC(0x7bec, 0x7bed); SET_HL(pop_effect(gb));
  CYC(0x7bed, 0x7bee); ret_effect(gb);
}

void interactiondc_spawnPuff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7bee, getFreeInteractionSlot_hook, 0x3aef, 0x7bf1);
  if (!(F & FZ)) { RET_TAKEN(0x7bf1); return; }
  CYC(0x7bf1, 0x7bf2);
  CYC(0x7bf2, 0x7bf4); mem_wr(gb, HL, 0x05);
  CYC(0x7bf4, 0x7bf6); L = 0x4b;
  CYC(0x7bf6, 0x7bf7); mem_wr(gb, HL, B);
  CYC(0x7bf7, 0x7bf9); L = 0x4d;
  CYC(0x7bf9, 0x7bfa); mem_wr(gb, HL, C);
  CYC(0x7bfa, 0x7bfb); ret_effect(gb);
}
