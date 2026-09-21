#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void oldMan_takeRupees_hook(GB *gb);
void oldMan_giveRupees_hook(GB *gb);
void oldManGiveShieldUpgradeToLink_hook(GB *gb);
void oldManWarpLinkToLibrary_hook(GB *gb);
void oldManSetAnimationToVar38_hook(GB *gb);
void label_15_097_hook(GB *gb);

static void oldMan_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void oldMan_takeRupees_hook(GB *gb) {
  BASE(oldMan_takeRupees);
  CYC(b_+0, b_+3); SET_HL(wNumRupees);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  if (F & FZ) {
    RET_TAKEN(b_+8); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); A = 0x01;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+18); SET_HL(SYM(oldMan_rupeeValues));
  CYC(b_+18, b_+19); oldMan_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+23); removeRupeeValue_hook(gb);
}

void oldMan_giveRupees_hook(GB *gb) {
  BASE(oldMan_giveRupees);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(oldMan_rupeeValues));
  CYC(b_+6, b_+7); oldMan_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); C = mem_rd(gb, HL);
  CYC(b_+8, b_+10); A = 0x28;
  CYC(b_+10, b_+13); giveTreasure_hook(gb);
}

void oldManGiveShieldUpgradeToLink_hook(GB *gb) {
  BASE(oldManGiveShieldUpgradeToLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto afterShieldCheck; } // jr c
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = W8(wShieldLevel);
afterShieldCheck:
  CYC(b_+10, b_+12); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+12, b_+14); goto haveSubidLevel; } // jr c
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); A = 0x02;
haveSubidLevel:
  CYC(b_+16, b_+17); C = A;
  CALL_C(b_+17, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x60);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x01);
  CYC(b_+26, b_+27); L = alu_inc8(gb, L);
  CYC(b_+27, b_+28); mem_wr(gb, HL, C);
  CYC(b_+28, b_+29); push_effect(gb, DE);
  CYC(b_+29, b_+32); SET_DE(w1Link_yh);
  CALL_C(b_+32, objectCopyPosition_rawAddress_hook, SYM(objectCopyPosition_rawAddress), b_+35);
  CYC(b_+35, b_+36); SET_DE(pop_effect(gb));
  RET(b_+36); return; // ret
}

void oldManWarpLinkToLibrary_hook(GB *gb) {
  BASE(oldManWarpLinkToLibrary);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+11);
  CALL_C(b_+3, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+6);
  CYC(b_+6, b_+8); A = 0x8d;
  CYC(b_+8, b_+11); TAIL(playSound_b00); // jp
}

void oldManSetAnimationToVar38_hook(GB *gb) {
  BASE(oldManSetAnimationToVar38);
  CYC(b_+0, b_+2); E = 0x78;
  TAIL(label_15_097);
}

void label_15_097_hook(GB *gb) {
  BASE(label_15_097);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+4); TAIL(interactionSetAnimation); // jp
}
