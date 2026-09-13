#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

void warpTransition0_hook(GB *gb);
void warpTransitionA_hook(GB *gb);
void warpTransition1_hook(GB *gb);
void warpUpdateRespawnPoint_hook(GB *gb);
void warpTransition_setLinkFacingDir_hook(GB *gb);

void warpTransition0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a4c, warpTransition_setLinkFacingDir_hook, 0x4a77, 0x4a4f);
  warpTransitionA_hook(gb);
}

void warpTransitionA_hook(GB *gb) {
  CYC(0x4a4f, 0x4a52);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransitionE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a52, objectCenterOnTile_hook, 0x20db, 0x4a55);
  CYC(0x4a55, 0x4a56); A = mem_rd(gb, HL);
  CYC(0x4a56, 0x4a58); alu_and(gb, 0xf0);
  CYC(0x4a58, 0x4a59); mem_wr(gb, HL, A);
  warpTransition1_hook(gb);
}

void warpTransition1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a59, warpTransition_setLinkFacingDir_hook, 0x4a77, 0x4a5c);
  warpUpdateRespawnPoint_hook(gb);
}

void warpUpdateRespawnPoint_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a5c, 0x4a5f); A = W8(wActiveGroup);
  CYC(0x4a5f, 0x4a61); alu_cp(gb, 0x06);
  if (!(F & FC)) {
    CYCT(0x4a61, 0x4a63);
    warpTransition0_hook(gb);
    return;
  }
  CYC(0x4a61, 0x4a63);
  CALL_C(0x4a63, setDeathRespawnPoint_hook, 0x1100, 0x4a66);
  CALL_C(0x4a66, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4a69);
  CYC(0x4a69, 0x4a6c);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransitionC_hook(GB *gb) {
  CYC(0x4a6c, 0x4a6f); A = W8(wcc50);
  CYC(0x4a6f, 0x4a71); alu_and(gb, 0x03);
  CYC(0x4a71, 0x4a73); E = (uint8_t)w1Link_direction;
  CYC(0x4a73, 0x4a74); mem_wr(gb, DE, A);
  CYC(0x4a74, 0x4a77);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransition_setLinkFacingDir_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a77, objectGetTileAtPosition_hook, 0x1444, 0x4a7a);
  CYC(0x4a7a, 0x4a7d); SET_HL(0x4a88);
  CALL_C(0x4a7d, lookupCollisionTable_hook, 0x1e1f, 0x4a80);
  if (F & FC) {
    CYCT(0x4a80, 0x4a82);
  } else {
    CYC(0x4a80, 0x4a82);
    CYC(0x4a82, 0x4a84); A = 0x02;
  }
  CYC(0x4a84, 0x4a86); E = (uint8_t)w1Link_direction;
  CYC(0x4a86, 0x4a87); mem_wr(gb, DE, A);
  CYC(0x4a87, 0x4a88); ret_effect(gb);
}
