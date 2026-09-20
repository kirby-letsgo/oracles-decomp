#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(nayruSavedCutscene_createEnergySwirl), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(nayruSavedCutscene_createEnergySwirl), (from), (to), true)

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
  BASE(nayruSavedCutscene_createEnergySwirl);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x4b;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = 0x4d;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+9); A = 0xff;
  CYC(b_+9, SYM(nayruSavedCutscene_spawnGuardIndex)); createEnergySwirlGoingIn_hook(gb);
}

void nayruSavedCutscene_spawnGuardIndex_hook(GB *gb) {
  BASE(nayruSavedCutscene_spawnGuardIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+4);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x6e);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x04);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); mem_wr(gb, HL, B);
  CYC(b_+12, SYM(nayruSavedCutscene_setSpeedZIndex)); ret_effect(gb);
}

void nayruSavedCutscene_setSpeedZIndex_hook(GB *gb) {
  BASE(nayruSavedCutscene_setSpeedZIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+12);
  CYC(b_+3, b_+4); nayruSavedCutscene_addDoubleIndexToHl_from_rst(gb, b_+4);
  CYC(b_+4, b_+6); E = 0x54;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); E = alu_inc8(gb, E);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

void nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(GB *gb);
void nayruSavedCutscene_setAnimationAtAddress_hook(GB *gb);

void nayruSavedCutscene_loadAngleAndAnimationPreset_hook(GB *gb) {
  BASE(nayruSavedCutscene_loadAngleAndAnimationPreset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(nayruSavedCutscene_angleAndAnimationPresets));
  CYC(b_+3, SYM(nayruSavedCutscene_setAngleAndAnimationAtAddress)); nayruSavedCutscene_addDoubleIndexToHl_from_rst(gb, SYM(nayruSavedCutscene_setAngleAndAnimationAtAddress));
  nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(gb);
}

void nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(GB *gb) {
  BASE(nayruSavedCutscene_setAngleAndAnimationAtAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x49;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, SYM(nayruSavedCutscene_setAnimationAtAddress)); mem_wr(gb, DE, A);
  nayruSavedCutscene_setAnimationAtAddress_hook(gb);
}

void nayruSavedCutscene_setAnimationAtAddress_hook(GB *gb) {
  BASE(nayruSavedCutscene_setAnimationAtAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, SYM(nayruSavedCutscene_angleAndAnimationPresets)); interactionSetAnimation_hook(gb);
}

void nayruSavedCutscene_loadGuardAngleToMoveTowardCenter_hook(GB *gb) {
  BASE(nayruSavedCutscene_loadGuardAngleToMoveTowardCenter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x50;
  CYC(b_+2, b_+4); A = 0x0a;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = 0x43;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+11); SET_HL(b_+14);
  CYC(b_+11, b_+12); nayruSavedCutscene_addDoubleIndexToHl_from_rst(gb, b_+12);
  CYC(b_+12, b_+14); nayruSavedCutscene_setAngleAndAnimationAtAddress_hook(gb);
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
  BASE(nayruSavedCutscene_loadGuardAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+9);
  CYC(b_+6, b_+7); nayruSavedCutscene_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); nayruSavedCutscene_setAnimationAtAddress_hook(gb);
}
