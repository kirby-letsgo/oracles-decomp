#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

static void nayruSavedCutscene_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void nayruSavedCutscene_createEnergySwirl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d45, 0x6d46); H = D;
  CYC(0x6d46, 0x6d48); L = 0x4b;
  CYC(0x6d48, 0x6d49); B = mem_rd(gb, HL);
  CYC(0x6d49, 0x6d4b); L = 0x4d;
  CYC(0x6d4b, 0x6d4c); C = mem_rd(gb, HL);
  CYC(0x6d4c, 0x6d4e); A = 0xff;
  CYC(0x6d4e, 0x6d51); createEnergySwirlGoingIn_hook(gb);
}

void nayruSavedCutscene_spawnGuardIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d51, 0x6d52); B = A;
  CALL_C(0x6d52, getFreeInteractionSlot_hook, 0x3aef, 0x6d55);
  if (!(F & FZ)) { CYCT(0x6d55, 0x6d56); ret_effect(gb); return; }
  CYC(0x6d55, 0x6d56);
  CYC(0x6d56, 0x6d58); mem_wr(gb, HL, 0x6e);
  CYC(0x6d58, 0x6d59); L = alu_inc8(gb, L);
  CYC(0x6d59, 0x6d5b); mem_wr(gb, HL, 0x04);
  CYC(0x6d5b, 0x6d5c); L = alu_inc8(gb, L);
  CYC(0x6d5c, 0x6d5d); mem_wr(gb, HL, B);
  CYC(0x6d5d, 0x6d5e); ret_effect(gb);
}

void nayruSavedCutscene_setSpeedZIndex_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d5e, 0x6d61); SET_HL(0x6d6a);
  CYC(0x6d61, 0x6d62); nayruSavedCutscene_addDoubleIndexToHl_from_rst(gb, 0x6d62);
  CYC(0x6d62, 0x6d64); E = 0x54;
  CYC(0x6d64, 0x6d65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d65, 0x6d66); mem_wr(gb, DE, A);
  CYC(0x6d66, 0x6d67); E = alu_inc8(gb, E);
  CYC(0x6d67, 0x6d68); A = mem_rd(gb, HL);
  CYC(0x6d68, 0x6d69); mem_wr(gb, DE, A);
  CYC(0x6d69, 0x6d6a); ret_effect(gb);
}

void nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(GB *gb);
void nayruSavedCutscene_setAnimationAtAddress_hook(GB *gb);

void nayruSavedCutscene_loadAngleAndAnimationPreset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d6e, 0x6d71); SET_HL(0x6d7a);
  CYC(0x6d71, 0x6d72); nayruSavedCutscene_addDoubleIndexToHl_from_rst(gb, 0x6d72);
  nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(gb);
}

void nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d72, 0x6d74); E = 0x49;
  CYC(0x6d74, 0x6d75); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d75, 0x6d76); mem_wr(gb, DE, A);
  nayruSavedCutscene_setAnimationAtAddress_hook(gb);
}

void nayruSavedCutscene_setAnimationAtAddress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d76, 0x6d77); A = mem_rd(gb, HL);
  CYC(0x6d77, 0x6d7a); interactionSetAnimation_hook(gb);
}

void nayruSavedCutscene_loadGuardAngleToMoveTowardCenter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d84, 0x6d86); E = 0x50;
  CYC(0x6d86, 0x6d88); A = 0x0a;
  CYC(0x6d88, 0x6d89); mem_wr(gb, DE, A);
  CYC(0x6d89, 0x6d8b); E = 0x43;
  CYC(0x6d8b, 0x6d8c); A = mem_rd(gb, DE);
  CYC(0x6d8c, 0x6d8f); SET_HL(0x6d92);
  CYC(0x6d8f, 0x6d90); nayruSavedCutscene_addDoubleIndexToHl_from_rst(gb, 0x6d90);
  CYC(0x6d90, 0x6d92); nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(gb);
}

static void nayruSavedCutscene_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

void nayruSavedCutscene_loadGuardAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6d9e, 0x6da0); E = 0x43;
  CYC(0x6da0, 0x6da1); A = mem_rd(gb, DE);
  CYC(0x6da1, 0x6da4); SET_HL(0x6da7);
  CYC(0x6da4, 0x6da5); nayruSavedCutscene_addAToHl_from_rst(gb, 0x6da5);
  CYC(0x6da5, 0x6da7); nayruSavedCutscene_setAnimationAtAddress_hook(gb);
}
