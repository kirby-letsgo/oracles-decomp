#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

#define linkSpeedTable_bank05 0x5d3b
#define specialObjectVelocityCollisionBits_bank05 0x5def
#define specialObjectAdjacentWallsRemapData_bank05 0x5e8d
#define adjacentWallOverworldOffsets_bank05 0x5ed2
#define adjacentWallSidescrollOffsets_bank05 0x5ee2
#define adjacentWallSpecialCollisions_bank05 0x5f1e
#define slideAngleTable_bank00 0x2461

void warpTransition0_hook(GB *gb);
void warpTransitionA_hook(GB *gb);
void warpTransition1_hook(GB *gb);
void warpUpdateRespawnPoint_hook(GB *gb);
void warpTransition_setLinkFacingDir_hook(GB *gb);
void warpTransition5_00_hook(GB *gb);
void warpTransition5_01_hook(GB *gb);
void warpTransition5_02_hook(GB *gb);
void linkIncrementDirectionOnOddFrames_hook(GB *gb);
void linkIncrementDirection_hook(GB *gb);
void warpTransition9_hook(GB *gb);
void warpTransitionB_hook(GB *gb);
void warpTransitionF_hook(GB *gb);
void warpTransition6_hook(GB *gb);
void warpTransition3_hook(GB *gb);
void linkState01_hook(GB *gb);
void linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook(GB *gb);
void linkCancelAllItemUsage_hook(GB *gb);
void linkState0e_hook(GB *gb);
void linkState0f_hook(GB *gb);
void linkState0b_hook(GB *gb);
void setLinkStateToDead_hook(GB *gb);
void linkState03_hook(GB *gb);
void specialObjectCode_link_hook(GB *gb);
void linkState02_hook(GB *gb);
void linkState0c_hook(GB *gb);
void linkState13_hook(GB *gb);
void linkState14_hook(GB *gb);
void linkState0d_hook(GB *gb);
void linkState05_hook(GB *gb);
void linkState06_hook(GB *gb);
void linkState09_hook(GB *gb);
void linkState11_hook(GB *gb);
void calculateAdjacentWallsBitset_hook(GB *gb);
void specialObjectUpdatePositionGivenVelocity_hook(GB *gb);
void specialObjectUpdateAdjacentWallsBitset_hook(GB *gb);
void updateLinkSpeed_withParam_hook(GB *gb);
void linkUpdateMovement_hook(GB *gb);
void linkUpdateKnockback_hook(GB *gb);
void updateHeartRingCounter_hook(GB *gb);
void animateLinkWalking_hook(GB *gb);
void linkUpdateInAir_hook(GB *gb);
void checkForUnderwaterTransition_hook(GB *gb);
void checkLinkForceState_hook(GB *gb);
void checkLinkPushingAgainstBed_hook(GB *gb);
void specialObjectUpdatePosition_hook(GB *gb);
void animateLinkStanding_hook(GB *gb);
void linkResetSpeed_hook(GB *gb);
void updateLinkSpeed_standard_hook(GB *gb);
void checkLinkJumpingOffCliff_hook(GB *gb);
void linkSetState_hook(GB *gb);
void func_5631_hook(GB *gb);
void linkUpdateSwimming_hook(GB *gb);
void overworldSwimmingState1_hook(GB *gb);
void forceDrownLink_hook(GB *gb);
void checkSwimmingOverSeawater_hook(GB *gb);
void overworldSwimmingState2_hook(GB *gb);
void overworldSwimmingState3_hook(GB *gb);
void linkUpdateDrowning_hook(GB *gb);
void linkSetSwimmingSpeed_hook(GB *gb);
void linkSetSwimmingSpeedTmp_hook(GB *gb);
void linkUpdateFlippersSpeed_hook(GB *gb);
void linkUpdateDiving_hook(GB *gb);
void func_5933_hook(GB *gb);
void linkUpdateVelocity_hook(GB *gb);
void initLinkState_hook(GB *gb);
void specialObjectSetPositionToVar38IfSet_hook(GB *gb);

static void link_add_a_to_hl(GB *gb, uint16_t return_address);
void linkUpdateVelocity_mermaidSuit_hook(GB *gb);

static void link_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t link_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}
void linkUpdateKnockback_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d5b, 0x5d5d); E = 0x04;
  CYC(0x5d5d, 0x5d5e); A = mem_rd(gb, DE);
  CYC(0x5d5e, 0x5d60); alu_cp(gb, 0x02);
  if ((F & FZ)) { CYCT(0x5d60, 0x5d62); goto L_5d89; } CYC(0x5d60, 0x5d62);
  CYC(0x5d62, 0x5d65); A = mem_rd(gb, 0xcc5c);
  CYC(0x5d65, 0x5d66); alu_rlca(gb);
  if ((F & FC)) { CYCT(0x5d66, 0x5d68); goto L_5d89; } CYC(0x5d66, 0x5d68);
  CYC(0x5d68, 0x5d6a); C = 0x01;
  CYC(0x5d6a, 0x5d6b); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x5d6b, 0x5d6d); goto L_5d6e; } CYC(0x5d6b, 0x5d6d);
  CYC(0x5d6d, 0x5d6e); C = alu_inc8(gb, C);
L_5d6e:
  CYC(0x5d6e, 0x5d6f); H = D;
  CYC(0x5d6f, 0x5d71); L = 0x2d;
  CYC(0x5d71, 0x5d72); A = mem_rd(gb, HL);
  CYC(0x5d72, 0x5d73); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x5d73, 0x5d74); ret_effect(gb); return; } CYC(0x5d73, 0x5d74);
  CYC(0x5d74, 0x5d75); alu_sub(gb, C);
  if ((F & FC)) { CYCT(0x5d75, 0x5d77); goto L_5d89; } CYC(0x5d75, 0x5d77);
  CYC(0x5d77, 0x5d78); mem_wr(gb, HL, A);
  CYC(0x5d78, 0x5d7a); L = 0x2c;
  CALL_C(0x5d7a, linkAdjustGivenAngleInSidescrollingArea_hook, 0x4432, 0x5d7d);
  CYC(0x5d7d, 0x5d7e); A = mem_rd(gb, DE);
  CYC(0x5d7e, 0x5d7f); C = A;
  CYC(0x5d7f, 0x5d81); B = 0x32;
  CYC(0x5d81, 0x5d84); SET_HL(0xcc95);
  CYC(0x5d84, 0x5d86); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 5)));
  CYC(0x5d86, 0x5d89); specialObjectUpdatePositionGivenVelocity_hook(gb); return;
L_5d89:
  CYC(0x5d89, 0x5d8b); E = 0x2d;
  CYC(0x5d8b, 0x5d8c); alu_xor(gb, A);
  CYC(0x5d8c, 0x5d8d); mem_wr(gb, DE, A);
  CYC(0x5d8d, 0x5d8e); ret_effect(gb); return;
}

void updateHeartRingCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x563e, 0x563f); E = A;
  CYC(0x563f, 0x5642); A = mem_rd(gb, 0xc6cb);
  CYC(0x5642, 0x5645); SET_BC(0x0208);
  CYC(0x5645, 0x5647); alu_cp(gb, 0x13);
  if ((F & FZ)) { CYCT(0x5647, 0x5649); goto L_5650; } CYC(0x5647, 0x5649);
  CYC(0x5649, 0x564b); alu_cp(gb, 0x14);
  if (!(F & FZ)) { CYCT(0x564b, 0x564d); goto L_5678; } CYC(0x564b, 0x564d);
  CYC(0x564d, 0x5650); SET_BC(0x0310);
L_5650:
  CYC(0x5650, 0x5651); A = E;
  CYC(0x5651, 0x5652); alu_or(gb, C);
  CYC(0x5652, 0x5653); C = A;
  CYC(0x5653, 0x5654); push_effect(gb, DE);
  CYC(0x5654, 0x5657); SET_DE(0xcec1);
  CYC(0x5657, 0x565a); SET_HL(0xcc53);
  CYC(0x565a, 0x565c); C = alu_srl(gb, C);
  if ((F & FC)) { CALL_ROM_CC(0x565c, 0x5680); } else CYC(0x565c, 0x565f);
  CYC(0x565f, 0x5661); E = 0xc3;
  CYC(0x5661, 0x5663); L = 0x53;
  CYC(0x5663, 0x5665); C = alu_srl(gb, C);
  if ((F & FC)) { CALL_ROM_CC(0x5665, 0x5680); } else CYC(0x5665, 0x5668);
  CYC(0x5668, 0x5669); SET_DE(pop_effect(gb));
  CYC(0x5669, 0x566c); A = mem_rd(gb, 0xcc55);
  CYC(0x566c, 0x566d); alu_cp(gb, B);
  if ((F & FC)) { CYCT(0x566d, 0x566e); ret_effect(gb); return; } CYC(0x566d, 0x566e);
  CYC(0x566e, 0x5671); SET_HL(0xc6aa);
  CYC(0x5671, 0x5672); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5672, 0x5673); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x5673, 0x5675); A = 0x29;
  if ((F & FC)) { CALL_C_CC(0x5675, giveTreasure_hook, 0x171c, 0x5678); } else CYC(0x5675, 0x5678);
L_5678:
  CYC(0x5678, 0x567b); SET_HL(0xcc53);
  CYC(0x567b, 0x567c); alu_xor(gb, A);
  CYC(0x567c, 0x567d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x567d, 0x567e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x567e, 0x567f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x567f, 0x5680); ret_effect(gb); return;
}

void animateLinkWalking_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5cc3, checkPegasusSeedCounter_hook, 0x2be8, 0x5cc6);
  if ((F & FZ)) { CYCT(0x5cc6, 0x5cc8); goto L_5cda; } CYC(0x5cc6, 0x5cc8);
  CYC(0x5cc8, 0x5cc9); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x5cc9, 0x5ccb); goto L_5cda; } CYC(0x5cc9, 0x5ccb);
  CYC(0x5ccb, 0x5cce); SET_HL(0xdf00);
  CYC(0x5cce, 0x5cd0); A = 0x03;
  CYC(0x5cd0, 0x5cd1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5cd1, 0x5cd3); mem_wr(gb, HL, 0x1a);
  CYC(0x5cd3, 0x5cd4); L = alu_inc8(gb, L);
  CYC(0x5cd4, 0x5cd5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5cd5, 0x5cd7); A = 0xa3;
  CALL_C(0x5cd7, playSound_b00_hook, 0x0c98, 0x5cda);
L_5cda:
  CYC(0x5cda, 0x5cdb); H = D;
  CYC(0x5cdb, 0x5cdd); A = 0x10;
  CYC(0x5cdd, 0x5cdf); L = 0x30;
  CYC(0x5cdf, 0x5ce0); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5ce0, 0x5ce3); specialObjectSetAnimation_hook(gb); return; }
  CYC(0x5ce0, 0x5ce3);
  CYC(0x5ce3, 0x5ce6); specialObjectAnimate_hook(gb); return;
}

void checkForUnderwaterTransition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x516c, 0x516f); A = mem_rd(gb, 0xcc91);
  CYC(0x516f, 0x5170); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5170, 0x5171); ret_effect(gb); return; } CYC(0x5170, 0x5171);
  CYC(0x5171, 0x5174); A = mem_rd(gb, 0xcc34);
  CYC(0x5174, 0x5176); alu_and(gb, 0x40);
  if ((F & FZ)) { CYCT(0x5176, 0x5177); ret_effect(gb); return; } CYC(0x5176, 0x5177);
  CYC(0x5177, 0x517a); A = mem_rd(gb, 0xcc2a);
  CYC(0x517a, 0x517c); alu_and(gb, 0x02);
  if ((F & FZ)) { CYCT(0x517c, 0x517d); ret_effect(gb); return; } CYC(0x517c, 0x517d);
  CYC(0x517d, 0x5180); A = mem_rd(gb, 0xcc99);
  CYC(0x5180, 0x5181); L = A;
  CYC(0x5181, 0x5183); H = 0xcf;
  CYC(0x5183, 0x5184); A = mem_rd(gb, HL);
  CYC(0x5184, 0x5187); SET_HL(0x7c6e);
  CALL_C(0x5187, lookupCollisionTable_hook, 0x1e1f, 0x518a);
  CYC(0x518a, 0x518c); alu_cp(gb, 0x18);
  if ((F & FZ)) { CYCT(0x518c, 0x518d); ret_effect(gb); return; } CYC(0x518c, 0x518d);
  CYC(0x518d, 0x518f); alu_cp(gb, 0x02);
  if ((F & FZ)) { CYCT(0x518f, 0x5191); goto L_51b2; } CYC(0x518f, 0x5191);
  CALL_C(0x5191, checkLinkCanSurface_hook, 0x3eaf, 0x5194);
  if (!(F & FC)) { CYCT(0x5194, 0x5195); ret_effect(gb); return; } CYC(0x5194, 0x5195);
  CYC(0x5195, 0x5196); SET_AF(pop_effect(gb));
  CYC(0x5196, 0x5199); A = mem_rd(gb, 0xcc34);
  CYC(0x5199, 0x519b); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x519b, 0x519d); goto L_51a4; } CYC(0x519b, 0x519d);
  CYC(0x519d, 0x519f); C = 0xfe;
  CYC(0x519f, 0x51a2); A = mem_rd(gb, 0xcc30);
  CYC(0x51a2, 0x51a4); goto L_51cf;
L_51a4:
  CYC(0x51a4, 0x51a7); A = mem_rd(gb, 0xcc3b);
  CYC(0x51a7, 0x51a8); A = alu_inc8(gb, A);
  CYC(0x51a8, 0x51ab); mem_wr(gb, 0xcc3b, A);
  CALL_C(0x51ab, getActiveRoomFromDungeonMapPosition_hook, 0x2e12, 0x51ae);
  CYC(0x51ae, 0x51b0); C = 0x00;
  CYC(0x51b0, 0x51b2); goto L_51cf;
L_51b2:
  CYC(0x51b2, 0x51b3); SET_AF(pop_effect(gb));
  CYC(0x51b3, 0x51b6); A = mem_rd(gb, 0xcc34);
  CYC(0x51b6, 0x51b8); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x51b8, 0x51ba); goto L_51c1; } CYC(0x51b8, 0x51ba);
  CYC(0x51ba, 0x51bc); C = 0x02;
  CYC(0x51bc, 0x51bf); A = mem_rd(gb, 0xcc30);
  CYC(0x51bf, 0x51c1); goto L_51cf;
L_51c1:
  CYC(0x51c1, 0x51c4); A = mem_rd(gb, 0xcc3b);
  CYC(0x51c4, 0x51c5); A = alu_dec8(gb, A);
  CYC(0x51c5, 0x51c8); mem_wr(gb, 0xcc3b, A);
  CALL_C(0x51c8, getActiveRoomFromDungeonMapPosition_hook, 0x2e12, 0x51cb);
  CYC(0x51cb, 0x51cd); C = 0x00;
  CYC(0x51cd, 0x51cf); goto L_51cf;
L_51cf:
  CYC(0x51cf, 0x51d2); mem_wr(gb, 0xcc48, A);
  CYC(0x51d2, 0x51d5); A = mem_rd(gb, 0xcc2d);
  CYC(0x51d5, 0x51d6); alu_add(gb, C);
  CYC(0x51d6, 0x51d8); alu_or(gb, 0x80);
  CYC(0x51d8, 0x51db); mem_wr(gb, 0xcc47, A);
  CYC(0x51db, 0x51de); A = mem_rd(gb, 0xcc99);
  CYC(0x51de, 0x51e1); mem_wr(gb, 0xcc4a, A);
  CYC(0x51e1, 0x51e3); A = 0x00;
  CYC(0x51e3, 0x51e6); mem_wr(gb, 0xcc49, A);
  CYC(0x51e6, 0x51e8); A = 0x03;
  CYC(0x51e8, 0x51eb); mem_wr(gb, 0xcc4b, A);
  CYC(0x51eb, 0x51ec); ret_effect(gb); return;
}

void checkLinkForceState_hook(GB *gb) {
  CYC(0x54c0, 0x54c3); SET_HL(0xcc4f);
  CYC(0x54c3, 0x54c4); A = mem_rd(gb, HL);
  CYC(0x54c4, 0x54c5); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x54c5, 0x54c6); ret_effect(gb); return; } CYC(0x54c5, 0x54c6);
  CYC(0x54c6, 0x54c8); mem_wr(gb, HL, 0x00);
  CYC(0x54c8, 0x54c9); SET_HL(pop_effect(gb));
  linkSetState_hook(gb); return;
}

void checkLinkPushingAgainstBed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5faf, 0x5fb2); SET_HL(0xccd7);
  CYC(0x5fb2, 0x5fb4); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5fb4, 0x5fb5); ret_effect(gb); return; } CYC(0x5fb4, 0x5fb5);
  CYC(0x5fb5, 0x5fb8); A = mem_rd(gb, 0xcc2d);
  CYC(0x5fb8, 0x5fba); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x5fba, 0x5fbb); ret_effect(gb); return; } CYC(0x5fba, 0x5fbb);
  CYC(0x5fbb, 0x5fbe); SET_BC(0x9e17);
  CYC(0x5fbe, 0x5fc0); L = 0x01;
  CYC(0x5fc0, 0x5fc3); A = mem_rd(gb, 0xcc30);
  CYC(0x5fc3, 0x5fc4); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x5fc4, 0x5fc5); ret_effect(gb); return; } CYC(0x5fc4, 0x5fc5);
  CYC(0x5fc5, 0x5fc8); A = mem_rd(gb, 0xcc99);
  CYC(0x5fc8, 0x5fc9); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x5fc9, 0x5fca); ret_effect(gb); return; } CYC(0x5fc9, 0x5fca);
  CYC(0x5fca, 0x5fcc); E = 0x08;
  CYC(0x5fcc, 0x5fcd); A = mem_rd(gb, DE);
  CYC(0x5fcd, 0x5fce); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(0x5fce, 0x5fcf); ret_effect(gb); return; } CYC(0x5fce, 0x5fcf);
  CALL_C(0x5fcf, checkLinkPushingAgainstWall_hook, 0x2b39, 0x5fd2);
  if ((F & FZ)) { CYCT(0x5fd2, 0x5fd3); ret_effect(gb); return; } CYC(0x5fd2, 0x5fd3);
  CYC(0x5fd3, 0x5fd6); SET_HL(0xccd4);
  CYC(0x5fd6, 0x5fd7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5fd7, 0x5fd8); A = mem_rd(gb, HL);
  CYC(0x5fd8, 0x5fda); alu_cp(gb, 0x5a);
  if ((F & FC)) { CYCT(0x5fda, 0x5fdb); ret_effect(gb); return; } CYC(0x5fda, 0x5fdb);
  CYC(0x5fdb, 0x5fdc); SET_HL(pop_effect(gb));
  CYC(0x5fdc, 0x5fdf); SET_HL(0xccd7);
  CYC(0x5fdf, 0x5fe1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
  CYC(0x5fe1, 0x5fe3); A = 0x05;
  CYC(0x5fe3, 0x5fe6); linkSetState_hook(gb); return;
}

void specialObjectUpdatePosition_hook(GB *gb) {
  CYC(0x5d97, 0x5d99); E = 0x10;
  CYC(0x5d99, 0x5d9a); A = mem_rd(gb, DE);
  CYC(0x5d9a, 0x5d9b); B = A;
  CYC(0x5d9b, 0x5d9d); E = 0x09;
  CYC(0x5d9d, 0x5d9e); A = mem_rd(gb, DE);
  CYC(0x5d9e, 0x5d9f); C = A;
  specialObjectUpdatePositionGivenVelocity_hook(gb);
}

void animateLinkStanding_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cb5, 0x5cb7); E = 0x30;
  CYC(0x5cb7, 0x5cb8); A = mem_rd(gb, DE);
  CYC(0x5cb8, 0x5cba); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x5cba, 0x5cbc); goto L_5cc1; } CYC(0x5cba, 0x5cbc);
  CALL_C(0x5cbc, checkPegasusSeedCounter_hook, 0x2be8, 0x5cbf);
  if (!(F & FZ)) { CYCT(0x5cbf, 0x5cc1); animateLinkWalking_hook(gb); return; } CYC(0x5cbf, 0x5cc1);
L_5cc1:
  CYC(0x5cc1, 0x5cc2); alu_xor(gb, A);
  CYC(0x5cc2, 0x5cc3); mem_wr(gb, DE, A);
  animateLinkWalking_hook(gb); return;
}

void linkResetSpeed_hook(GB *gb) {
  CYC(0x562c, 0x562e); E = 0x10;
  CYC(0x562e, 0x562f); alu_xor(gb, A);
  CYC(0x562f, 0x5630); mem_wr(gb, DE, A);
  CYC(0x5630, 0x5631); ret_effect(gb); return;
}

void updateLinkSpeed_standard_hook(GB *gb) {
  CYC(0x5ce6, 0x5ce8); C = 0x00;
  updateLinkSpeed_withParam_hook(gb);
}

void linkSetState_hook(GB *gb) {
  CYC(0x54c9, 0x54ca); H = D;
  CYC(0x54ca, 0x54cc); L = 0x04;
  CYC(0x54cc, 0x54cd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x54cd, 0x54cf); mem_wr(gb, HL, 0x00);
  CYC(0x54cf, 0x54d1); alu_cp(gb, 0x0a);
  if ((F & FZ)) { CYCT(0x54d1, 0x54d3); goto L_54da; } CYC(0x54d1, 0x54d3);
  CYC(0x54d3, 0x54d5); alu_cp(gb, 0x0c);
  if ((F & FZ)) { CYCT(0x54d5, 0x54d7); goto L_54da; } CYC(0x54d5, 0x54d7);
  CYC(0x54d7, 0x54d9); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(0x54d9, 0x54da); ret_effect(gb); return; } CYC(0x54d9, 0x54da);
L_54da:
  CYC(0x54da, 0x54dd); specialObjectCode_link_hook(gb); return;
}

void func_5631_hook(GB *gb) {
  CYC(0x5631, 0x5634); SET_HL(0xcc9e);
  CYC(0x5634, 0x5636); alu_bit(gb, 6, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5636, 0x5637); ret_effect(gb); return; } CYC(0x5636, 0x5637);
  CYC(0x5637, 0x5639); E = 0x2c;
  CYC(0x5639, 0x563a); A = mem_rd(gb, DE);
  CYC(0x563a, 0x563c); E = 0x09;
  CYC(0x563c, 0x563d); mem_wr(gb, DE, A);
  CYC(0x563d, 0x563e); ret_effect(gb); return;
}

void overworldSwimmingState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56ad, linkCancelAllItemUsage_hook, 0x4f49, 0x56b0);
  CALL_C(0x56b0, linkSetSwimmingSpeed_hook, 0x5778, 0x56b3);
  CYC(0x56b3, 0x56b5); L = 0x2f;
  CYC(0x56b5, 0x56b7); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(0x56b7, 0x56b9); L = 0x06;
  CYC(0x56b9, 0x56bb); mem_wr(gb, HL, 0x0a);
  if ((F & FZ)) { CYCT(0x56bb, 0x56bd); goto L_56bf; } CYC(0x56bb, 0x56bd);
  CYC(0x56bd, 0x56bf); mem_wr(gb, HL, 0x02);
L_56bf:
  CYC(0x56bf, 0x56c2); A = mem_rd(gb, 0xcc5d);
  CYC(0x56c2, 0x56c4); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x56c4, 0x56c6); goto L_56d8; } CYC(0x56c4, 0x56c6);
  CALL_C(0x56c6, checkSwimmingOverSeawater_hook, 0x570f, 0x56c9);
  if ((F & FZ)) { CYCT(0x56c9, 0x56cb); goto L_56d4; } CYC(0x56c9, 0x56cb);
  CYC(0x56cb, 0x56cd); A = 0x2e;
  CALL_C(0x56cd, checkTreasureObtained_hook, 0x1748, 0x56d0);
  CYC(0x56d0, 0x56d2); B = 0x0b;
  if ((F & FC)) { CYCT(0x56d2, 0x56d4); goto L_56f8; } CYC(0x56d2, 0x56d4);
L_56d4:
  CYC(0x56d4, 0x56d6); C = 0x88;
  CYC(0x56d6, 0x56d8); goto L_56da;
L_56d8:
  CYC(0x56d8, 0x56da); C = 0x78;
L_56da:
  CYC(0x56da, 0x56dc); A = 0x02;
  CYC(0x56dc, 0x56df); mem_wr(gb, 0xcc4f, A);
  CYC(0x56df, 0x56e1); A = 0x04;
  CYC(0x56e1, 0x56e4); mem_wr(gb, 0xcc51, A);
  CYC(0x56e4, 0x56e6); A = 0x80;
  CYC(0x56e6, 0x56e9); mem_wr(gb, 0xcc92, A);
  CYC(0x56e9, 0x56ea); H = D;
  CYC(0x56ea, 0x56ec); L = 0x2b;
  CYC(0x56ec, 0x56ed); mem_wr(gb, HL, C);
  CYC(0x56ed, 0x56ef); L = 0x24;
  CYC(0x56ef, 0x56f1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x56f1, 0x56f3); A = 0x5f;
  CALL_C(0x56f3, playSound_b00_hook, 0x0c98, 0x56f6);
  CYC(0x56f6, 0x56f8); B = 0x0a;
L_56f8:
  CYC(0x56f8, 0x56fb); SET_HL(0xcc5d);
  CYC(0x56fb, 0x56fc); A = mem_rd(gb, HL);
  CYC(0x56fc, 0x56fe); alu_and(gb, 0xf0);
  CYC(0x56fe, 0x5700); alu_or(gb, 0x02);
  CYC(0x5700, 0x5701); mem_wr(gb, HL, A);
  CYC(0x5701, 0x5702); A = B;
  CALL_C(0x5702, specialObjectSetAnimation_hook, 0x2b0a, 0x5705);
  CYC(0x5705, 0x5708); linkCreateSplash_hook(gb); return;
}

void forceDrownLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goto L_5708;
L_56d8:
  CYC(0x56d8, 0x56da); C = 0x78;
  CYC(0x56da, 0x56dc); A = 0x02;
  CYC(0x56dc, 0x56df); mem_wr(gb, 0xcc4f, A);
  CYC(0x56df, 0x56e1); A = 0x04;
  CYC(0x56e1, 0x56e4); mem_wr(gb, 0xcc51, A);
  CYC(0x56e4, 0x56e6); A = 0x80;
  CYC(0x56e6, 0x56e9); mem_wr(gb, 0xcc92, A);
  CYC(0x56e9, 0x56ea); H = D;
  CYC(0x56ea, 0x56ec); L = 0x2b;
  CYC(0x56ec, 0x56ed); mem_wr(gb, HL, C);
  CYC(0x56ed, 0x56ef); L = 0x24;
  CYC(0x56ef, 0x56f1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x56f1, 0x56f3); A = 0x5f;
  CALL_C(0x56f3, playSound_b00_hook, 0x0c98, 0x56f6);
  CYC(0x56f6, 0x56f8); B = 0x0a;
  CYC(0x56f8, 0x56fb); SET_HL(0xcc5d);
  CYC(0x56fb, 0x56fc); A = mem_rd(gb, HL);
  CYC(0x56fc, 0x56fe); alu_and(gb, 0xf0);
  CYC(0x56fe, 0x5700); alu_or(gb, 0x02);
  CYC(0x5700, 0x5701); mem_wr(gb, HL, A);
  CYC(0x5701, 0x5702); A = B;
  CALL_C(0x5702, specialObjectSetAnimation_hook, 0x2b0a, 0x5705);
  CYC(0x5705, 0x5708); linkCreateSplash_hook(gb); return;
L_5708:
  CYC(0x5708, 0x570b); SET_HL(0xcc5d);
  CYC(0x570b, 0x570d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x570d, 0x570f); goto L_56d8;
}

void checkSwimmingOverSeawater_hook(GB *gb) {
  CYC(0x570f, 0x5712); A = mem_rd(gb, 0xd02f);
  CYC(0x5712, 0x5714); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x5714, 0x5715); ret_effect(gb); return; } CYC(0x5714, 0x5715);
  CYC(0x5715, 0x5718); A = mem_rd(gb, 0xcc9c);
  CYC(0x5718, 0x571a); alu_sub(gb, 0x17);
  CYC(0x571a, 0x571b); ret_effect(gb); return;
}

void overworldSwimmingState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x571b, itemDecCounter1_hook, 0x23d6, 0x571e);
  if (!(F & FZ)) { CYCT(0x571e, 0x5721); specialObjectUpdatePosition_hook(gb); return; } CYC(0x571e, 0x5721);
  CYC(0x5721, 0x5724); SET_HL(0xcc5d);
  CYC(0x5724, 0x5725); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  overworldSwimmingState3_hook(gb); return;
}

void linkUpdateDrowning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5759, 0x575b); A = 0x80;
  CYC(0x575b, 0x575e); mem_wr(gb, 0xcc92, A);
  CALL_C(0x575e, specialObjectAnimate_hook, 0x2aef, 0x5761);
  CYC(0x5761, 0x5762); H = D;
  CYC(0x5762, 0x5763); alu_xor(gb, A);
  CYC(0x5763, 0x5765); L = 0x24;
  CYC(0x5765, 0x5766); mem_wr(gb, HL, A);
  CYC(0x5766, 0x5768); L = 0x21;
  CYC(0x5768, 0x576a); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x576a, 0x576b); ret_effect(gb); return; } CYC(0x576a, 0x576b);
  CYC(0x576b, 0x576e); mem_wr(gb, 0xcc5d, A);
  CYC(0x576e, 0x5770); A = 0x02;
  CYC(0x5770, 0x5773); mem_wr(gb, 0xcc51, A);
  CYC(0x5773, 0x5775); A = 0x02;
  CYC(0x5775, 0x5778); linkSetState_hook(gb); return;
}

void linkSetSwimmingSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5778, 0x577a); A = 0x15;
  CALL_C(0x577a, cpActiveRing_hook, 0x23b0, 0x577d);
  CYC(0x577d, 0x577f); A = 0x23;
  if ((F & FZ)) { CYCT(0x577f, 0x5781); goto L_5783; } CYC(0x577f, 0x5781);
  CYC(0x5781, 0x5783); A = 0x14;
L_5783:
  CYC(0x5783, 0x5784); H = D;
  CYC(0x5784, 0x5786); L = 0x10;
  CYC(0x5786, 0x5787); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5787, 0x5788); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5788, 0x5789); L = alu_inc8(gb, L);
  CYC(0x5789, 0x578b); A = 0x03;
  CYC(0x578b, 0x578c); mem_wr(gb, HL, A);
  CYC(0x578c, 0x578e); L = 0x35;
  CYC(0x578e, 0x5790); mem_wr(gb, HL, 0x00);
  CYC(0x5790, 0x5791); ret_effect(gb); return;
}

void linkSetSwimmingSpeedTmp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5791, 0x5793); A = 0x15;
  CALL_C(0x5793, cpActiveRing_hook, 0x23b0, 0x5796);
  CYC(0x5796, 0x5798); A = 0x23;
  if ((F & FZ)) { CYCT(0x5798, 0x579a); goto L_579c; } CYC(0x5798, 0x579a);
  CYC(0x579a, 0x579c); A = 0x14;
L_579c:
  CYC(0x579c, 0x579e); E = 0x11;
  CYC(0x579e, 0x579f); mem_wr(gb, DE, A);
  CYC(0x579f, 0x57a0); ret_effect(gb); return;
}

void linkUpdateDiving_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goto L_5810;
L_51b2:
  CYC(0x51b2, 0x51b3); SET_AF(pop_effect(gb));
  CYC(0x51b3, 0x51b6); A = mem_rd(gb, 0xcc34);
  CYC(0x51b6, 0x51b8); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x51b8, 0x51ba); goto L_51c1; } CYC(0x51b8, 0x51ba);
  CYC(0x51ba, 0x51bc); C = 0x02;
  CYC(0x51bc, 0x51bf); A = mem_rd(gb, 0xcc30);
  CYC(0x51bf, 0x51c1); goto L_51cf;
L_51c1:
  CYC(0x51c1, 0x51c4); A = mem_rd(gb, 0xcc3b);
  CYC(0x51c4, 0x51c5); A = alu_dec8(gb, A);
  CYC(0x51c5, 0x51c8); mem_wr(gb, 0xcc3b, A);
  CALL_C(0x51c8, getActiveRoomFromDungeonMapPosition_hook, 0x2e12, 0x51cb);
  CYC(0x51cb, 0x51cd); C = 0x00;
  CYC(0x51cd, 0x51cf); goto L_51cf;
L_51cf:
  CYC(0x51cf, 0x51d2); mem_wr(gb, 0xcc48, A);
  CYC(0x51d2, 0x51d5); A = mem_rd(gb, 0xcc2d);
  CYC(0x51d5, 0x51d6); alu_add(gb, C);
  CYC(0x51d6, 0x51d8); alu_or(gb, 0x80);
  CYC(0x51d8, 0x51db); mem_wr(gb, 0xcc47, A);
  CYC(0x51db, 0x51de); A = mem_rd(gb, 0xcc99);
  CYC(0x51de, 0x51e1); mem_wr(gb, 0xcc4a, A);
  CYC(0x51e1, 0x51e3); A = 0x00;
  CYC(0x51e3, 0x51e6); mem_wr(gb, 0xcc49, A);
  CYC(0x51e6, 0x51e8); A = 0x03;
  CYC(0x51e8, 0x51eb); mem_wr(gb, 0xcc4b, A);
  CYC(0x51eb, 0x51ec); ret_effect(gb); return;
L_5810:
  CALL_C(0x5810, specialObjectAnimate_hook, 0x2aef, 0x5813);
  CYC(0x5813, 0x5816); SET_HL(0xcc5d);
  CYC(0x5816, 0x5818); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5818, 0x581a); goto L_582c; } CYC(0x5818, 0x581a);
  CYC(0x581a, 0x581d); A = mem_rd(gb, 0xcc91);
  CYC(0x581d, 0x581e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x581e, 0x5820); goto L_582c; } CYC(0x581e, 0x5820);
  CYC(0x5820, 0x5823); A = mem_rd(gb, 0xcc99);
  CYC(0x5823, 0x5824); C = A;
  CYC(0x5824, 0x5826); B = 0xcf;
  CYC(0x5826, 0x5827); A = mem_rd(gb, BC);
  CYC(0x5827, 0x5829); alu_cp(gb, 0xfc);
  if ((F & FZ)) { CYCT(0x5829, 0x582c); goto L_51b2; } CYC(0x5829, 0x582c);
L_582c:
  CYC(0x582c, 0x582f); A = mem_rd(gb, 0xcc2a);
  CYC(0x582f, 0x5831); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x5831, 0x5833); goto L_5841; } CYC(0x5831, 0x5833);
  CYC(0x5833, 0x5835); A = 0x3c;
  CALL_C(0x5835, cpActiveRing_hook, 0x23b0, 0x5838);
  if ((F & FZ)) { CYCT(0x5838, 0x5839); ret_effect(gb); return; } CYC(0x5838, 0x5839);
  CYC(0x5839, 0x583b); E = 0x07;
  CYC(0x583b, 0x583c); A = mem_rd(gb, DE);
  CYC(0x583c, 0x583d); A = alu_dec8(gb, A);
  CYC(0x583d, 0x583e); mem_wr(gb, DE, A);
  if ((F & FZ)) { CYCT(0x583e, 0x5840); goto L_5845; } CYC(0x583e, 0x5840);
  CYC(0x5840, 0x5841); ret_effect(gb); return;
L_5841:
  CYC(0x5841, 0x5843); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5843, 0x5845); goto L_584c; } CYC(0x5843, 0x5845);
L_5845:
  CYC(0x5845, 0x5847); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x5847, 0x5849); A = 0x0b;
  CYC(0x5849, 0x584c); specialObjectSetAnimation_hook(gb); return;
L_584c:
  CYC(0x584c, 0x584e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x584e, 0x5850); E = 0x07;
  CYC(0x5850, 0x5852); A = 0x78;
  CYC(0x5852, 0x5853); mem_wr(gb, DE, A);
  CALL_C(0x5853, linkCreateSplash_hook, 0x2c7e, 0x5856);
  CYC(0x5856, 0x5858); A = 0x0c;
  CYC(0x5858, 0x585b); specialObjectSetAnimation_hook(gb); return;
}

void func_5933_hook(GB *gb) {
  CYC(0x5933, 0x5934); E = A;
  CYC(0x5934, 0x5935); H = D;
  CYC(0x5935, 0x5937); L = 0x09;
  CYC(0x5937, 0x5939); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5939, 0x593b); goto L_593d; } CYC(0x5939, 0x593b);
  CYC(0x593b, 0x593c); mem_wr(gb, HL, E);
  CYC(0x593c, 0x593d); ret_effect(gb); return;
L_593d:
  CYC(0x593d, 0x593f); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x593f, 0x5941); goto L_595b; } CYC(0x593f, 0x5941);
  CYC(0x5941, 0x5942); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x5942, 0x5944); alu_add(gb, 0x04);
  CYC(0x5944, 0x5946); alu_and(gb, 0x1f);
  CYC(0x5946, 0x5948); alu_cp(gb, 0x09);
  if ((F & FC)) { CYCT(0x5948, 0x594a); goto L_597c; } CYC(0x5948, 0x594a);
  CYC(0x594a, 0x594c); alu_sub(gb, 0x10);
  CYC(0x594c, 0x594e); alu_cp(gb, 0x09);
  if ((F & FC)) { CYCT(0x594e, 0x5950); goto L_5967; } CYC(0x594e, 0x5950);
  CYC(0x5950, 0x5953); SET_BC(0x0100);
  CYC(0x5953, 0x5955); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x5955, 0x5957); goto L_598d; } CYC(0x5955, 0x5957);
  CYC(0x5957, 0x5959); B = 0xff;
  CYC(0x5959, 0x595b); goto L_598d;
L_595b:
  CYC(0x595b, 0x595e); SET_BC(0x00fb);
  CYC(0x595e, 0x5961); A = mem_rd(gb, 0xcc5c);
  CYC(0x5961, 0x5962); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x5962, 0x5964); goto L_598d; } CYC(0x5962, 0x5964);
  CYC(0x5964, 0x5965); C = B;
  CYC(0x5965, 0x5967); goto L_598d;
L_5967:
  CYC(0x5967, 0x596a); SET_BC(0x01fb);
  CYC(0x596a, 0x596c); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(0x596c, 0x596e); goto L_598d; } CYC(0x596c, 0x596e);
  CYC(0x596e, 0x5970); B = 0xff;
  CYC(0x5970, 0x5972); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x5972, 0x5974); goto L_598d; } CYC(0x5972, 0x5974);
  CYC(0x5974, 0x5975); A = E;
  CYC(0x5975, 0x5977); alu_xor(gb, 0x10);
  CYC(0x5977, 0x5978); mem_wr(gb, HL, A);
  CYC(0x5978, 0x597a); B = 0x00;
  CYC(0x597a, 0x597c); goto L_598d;
L_597c:
  CYC(0x597c, 0x597f); SET_BC(0xff05);
  CYC(0x597f, 0x5981); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(0x5981, 0x5983); goto L_598d; } CYC(0x5981, 0x5983);
  CYC(0x5983, 0x5985); B = 0x01;
  CYC(0x5985, 0x5987); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(0x5987, 0x5989); goto L_598d; } CYC(0x5987, 0x5989);
  CYC(0x5989, 0x598a); A = E;
  CYC(0x598a, 0x598b); mem_wr(gb, HL, A);
  CYC(0x598b, 0x598d); B = 0x00;
L_598d:
  CYC(0x598d, 0x598f); L = 0x12;
  CYC(0x598f, 0x5990); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5990, 0x5991); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5991, 0x5992); alu_cp(gb, mem_rd(gb, HL));
  if ((F & FC)) { CYCT(0x5992, 0x5993); ret_effect(gb); return; } CYC(0x5992, 0x5993);
  CYC(0x5993, 0x5994); L = alu_dec8(gb, L);
  CYC(0x5994, 0x5996); mem_wr(gb, HL, 0x00);
  CYC(0x5996, 0x5998); L = 0x09;
  CYC(0x5998, 0x5999); A = mem_rd(gb, HL);
  CYC(0x5999, 0x599a); alu_add(gb, B);
  CYC(0x599a, 0x599c); alu_and(gb, 0x1f);
  CYC(0x599c, 0x599d); mem_wr(gb, HL, A);
  CYC(0x599d, 0x599f); L = 0x11;
  CYC(0x599f, 0x59a0); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x59a0, 0x59a1); B = A;
  CYC(0x59a1, 0x59a2); A = mem_rd(gb, HL);
  CYC(0x59a2, 0x59a3); alu_add(gb, C);
  if ((F & FZ)) { CYCT(0x59a3, 0x59a5); goto L_59af; } CYC(0x59a3, 0x59a5);
  CYC(0x59a5, 0x59a7); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x59a7, 0x59a9); goto L_59af; } CYC(0x59a7, 0x59a9);
  CYC(0x59a9, 0x59aa); alu_cp(gb, B);
  if ((F & FC)) { CYCT(0x59aa, 0x59ac); goto L_59ad; } CYC(0x59aa, 0x59ac);
  CYC(0x59ac, 0x59ad); A = B;
L_59ad:
  CYC(0x59ad, 0x59ae); mem_wr(gb, HL, A);
  CYC(0x59ae, 0x59af); ret_effect(gb); return;
L_59af:
  CYC(0x59af, 0x59b1); L = 0x10;
  CYC(0x59b1, 0x59b2); alu_xor(gb, A);
  CYC(0x59b2, 0x59b3); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x59b3, 0x59b4); L = alu_inc8(gb, L);
  CYC(0x59b4, 0x59b5); mem_wr(gb, HL, L);
  CYC(0x59b5, 0x59b6); A = alu_dec8(gb, A);
  CYC(0x59b6, 0x59b8); L = 0x09;
  CYC(0x59b8, 0x59b9); mem_wr(gb, HL, A);
  CYC(0x59b9, 0x59ba); ret_effect(gb); return;
}

void initLinkState_hook(GB *gb) {
  CYC(0x5ca1, 0x5ca2); H = D;
  CYC(0x5ca2, 0x5ca4); L = 0x04;
  CYC(0x5ca4, 0x5ca6); mem_wr(gb, HL, 0x01);
  CYC(0x5ca6, 0x5ca7); L = alu_inc8(gb, L);
  CYC(0x5ca7, 0x5ca9); mem_wr(gb, HL, 0x00);
  CYC(0x5ca9, 0x5cab); L = 0x35;
  CYC(0x5cab, 0x5cad); mem_wr(gb, HL, 0x00);
  CYC(0x5cad, 0x5cae); ret_effect(gb); return;
}

void overworldSwimmingState3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  goto L_5725;
L_56d4:
  CYC(0x56d4, 0x56d6); C = 0x88;
  CYC(0x56d6, 0x56d8); goto L_56da;
L_56da:
  CYC(0x56da, 0x56dc); A = 0x02;
  CYC(0x56dc, 0x56df); mem_wr(gb, 0xcc4f, A);
  CYC(0x56df, 0x56e1); A = 0x04;
  CYC(0x56e1, 0x56e4); mem_wr(gb, 0xcc51, A);
  CYC(0x56e4, 0x56e6); A = 0x80;
  CYC(0x56e6, 0x56e9); mem_wr(gb, 0xcc92, A);
  CYC(0x56e9, 0x56ea); H = D;
  CYC(0x56ea, 0x56ec); L = 0x2b;
  CYC(0x56ec, 0x56ed); mem_wr(gb, HL, C);
  CYC(0x56ed, 0x56ef); L = 0x24;
  CYC(0x56ef, 0x56f1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x56f1, 0x56f3); A = 0x5f;
  CALL_C(0x56f3, playSound_b00_hook, 0x0c98, 0x56f6);
  CYC(0x56f6, 0x56f8); B = 0x0a;
  CYC(0x56f8, 0x56fb); SET_HL(0xcc5d);
  CYC(0x56fb, 0x56fc); A = mem_rd(gb, HL);
  CYC(0x56fc, 0x56fe); alu_and(gb, 0xf0);
  CYC(0x56fe, 0x5700); alu_or(gb, 0x02);
  CYC(0x5700, 0x5701); mem_wr(gb, HL, A);
  CYC(0x5701, 0x5702); A = B;
  CALL_C(0x5702, specialObjectSetAnimation_hook, 0x2b0a, 0x5705);
  CYC(0x5705, 0x5708); linkCreateSplash_hook(gb); return;
L_5725:
  CALL_C(0x5725, checkSwimmingOverSeawater_hook, 0x570f, 0x5728);
  if ((F & FZ)) { CYCT(0x5728, 0x572a); goto L_56d4; } CYC(0x5728, 0x572a);
  CALL_C(0x572a, linkUpdateDiving_hook, 0x5810, 0x572d);
  CALL_C(0x572d, objectSetVisiblec1_hook, 0x1e3c, 0x5730);
  CYC(0x5730, 0x5731); H = D;
  CYC(0x5731, 0x5733); L = 0x24;
  CYC(0x5733, 0x5735); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5735, 0x5738); A = mem_rd(gb, 0xcc5d);
  CYC(0x5738, 0x5739); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x5739, 0x573b); goto L_5740; } CYC(0x5739, 0x573b);
  CYC(0x573b, 0x573d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(0x573d, objectSetVisiblec3_hook, 0x1e4e, 0x5740);
L_5740:
  CALL_C(0x5740, updateLinkDirectionFromAngle_hook, 0x2b64, 0x5743);
  CYC(0x5743, 0x5744); H = D;
  CYC(0x5744, 0x5746); L = 0x2f;
  CYC(0x5746, 0x5748); alu_bit(gb, 6, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5748, 0x574a); goto L_5750; } CYC(0x5748, 0x574a);
  CALL_C(0x574a, linkUpdateVelocity_mermaidSuit_hook, 0x58f7, 0x574d);
  CYC(0x574d, 0x5750); specialObjectUpdatePosition_hook(gb); return;
L_5750:
  CALL_C(0x5750, linkUpdateFlippersSpeed_hook, 0x57a0, 0x5753);
  CALL_C(0x5753, func_5933_hook, 0x5933, 0x5756);
  CYC(0x5756, 0x5759); specialObjectUpdatePosition_hook(gb); return;
}

void linkUpdateVelocity_mermaidSuit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x58f7, 0x58f9); C = 0x98;
  CALL_C(0x58f9, updateLinkSpeed_withParam_hook, 0x5ce8, 0x58fc);
  CYC(0x58fc, 0x58ff); A = mem_rd(gb, 0xc6cb);
  CYC(0x58ff, 0x5901); alu_cp(gb, 0x15);
  if (!(F & FZ)) { CYCT(0x5901, 0x5903); goto L_5908; } CYC(0x5901, 0x5903);
  CYC(0x5903, 0x5905); E = 0x11;
  CYC(0x5905, 0x5907); A = 0x37;
  CYC(0x5907, 0x5908); mem_wr(gb, DE, A);
L_5908:
  CYC(0x5908, 0x5909); H = D;
  CYC(0x5909, 0x590c); A = mem_rd(gb, 0xcc61);
  CYC(0x590c, 0x590d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x590d, 0x590f); goto L_5916; } CYC(0x590d, 0x590f);
  CYC(0x590f, 0x5912); A = mem_rd(gb, 0xcc2a);
  CYC(0x5912, 0x5914); alu_and(gb, 0xf0);
  if (!(F & FZ)) { CYCT(0x5914, 0x5916); goto L_5922; } CYC(0x5914, 0x5916);
L_5916:
  CYC(0x5916, 0x5918); L = 0x3e;
  CYC(0x5918, 0x5919); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5919, 0x591b); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x591b, 0x591d); goto L_592c; } CYC(0x591b, 0x591d);
  CYC(0x591d, 0x591f); A = 0xff;
  CYC(0x591f, 0x5920); mem_wr(gb, HL, A);
  CYC(0x5920, 0x5922); func_5933_hook(gb); return;
L_5922:
  CYC(0x5922, 0x5924); A = 0x87;
  CALL_C(0x5924, playSound_b00_hook, 0x0c98, 0x5927);
  CYC(0x5927, 0x5928); H = D;
  CYC(0x5928, 0x592a); L = 0x3e;
  CYC(0x592a, 0x592c); mem_wr(gb, HL, 0x04);
L_592c:
  CYC(0x592c, 0x592e); L = 0x12;
  CYC(0x592e, 0x5930); mem_wr(gb, HL, 0x14);
  CYC(0x5930, 0x5933); A = mem_rd(gb, 0xcc2b);
  func_5933_hook(gb); return;
}

void linkUpdateVelocity_hook(GB *gb) {
  CYC(0x58f0, 0x58f3); A = mem_rd(gb, 0xcc34);
  CYC(0x58f3, 0x58f5); alu_and(gb, 0x40);
  if ((F & FZ)) { CYCT(0x58f5, 0x58f7); goto L_5930; } CYC(0x58f5, 0x58f7);
  linkUpdateVelocity_mermaidSuit_hook(gb); return;
L_5930:
  CYC(0x5930, 0x5933); A = mem_rd(gb, 0xcc2b);
  func_5933_hook(gb); return;
}

void linkUpdateInAir_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5af3, 0x5af6); A = mem_rd(gb, 0xcc5c);
  CYC(0x5af6, 0x5af8); alu_and(gb, 0x0f);
  CYC(0x5af8, 0x5af9); push_effect(gb, 0x5af9);
  switch (link_jump_table(gb)) { case 0x5aff: goto L_5aff; case 0x5b0c: goto L_5b0c; case 0x5b3a: goto L_5b3a; default: hook_continue(gb, HL, sp0_); return; }
L_5aff:
  CYC(0x5aff, 0x5b00); H = D;
  CYC(0x5b00, 0x5b02); L = 0x0f;
  CYC(0x5b02, 0x5b04); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5b04, 0x5b05); ret_effect(gb); return; } CYC(0x5b04, 0x5b05);
  CYC(0x5b05, 0x5b07); A = 0x02;
  CYC(0x5b07, 0x5b0a); mem_wr(gb, 0xcc5c, A);
  CYC(0x5b0a, 0x5b0c); goto L_5b2d;
L_5b0c:
  CYC(0x5b0c, 0x5b0f); SET_HL(0xcc5c);
  CYC(0x5b0f, 0x5b10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5b10, 0x5b12); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5b12, 0x5b14); goto L_5b28; } CYC(0x5b12, 0x5b14);
  CYC(0x5b14, 0x5b17); SET_HL(0xcc9e);
  CYC(0x5b17, 0x5b19); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5b19, 0x5b1b); goto L_5b28; } CYC(0x5b19, 0x5b1b);
  CYC(0x5b1b, 0x5b1d); L = 0x9c;
  CYC(0x5b1d, 0x5b1f); mem_wr(gb, HL, 0x00);
  CALL_C(0x5b1f, updateLinkSpeed_standard_hook, 0x5ce6, 0x5b22);
  CYC(0x5b22, 0x5b25); A = mem_rd(gb, 0xcc2b);
  CYC(0x5b25, 0x5b27); E = 0x09;
  CYC(0x5b27, 0x5b28); mem_wr(gb, DE, A);
L_5b28:
  CYC(0x5b28, 0x5b2a); A = 0x53;
  CALL_C(0x5b2a, playSound_b00_hook, 0x0c98, 0x5b2d);
L_5b2d:
  CYC(0x5b2d, 0x5b30); A = mem_rd(gb, 0xcc5a);
  CYC(0x5b30, 0x5b31); C = A;
  CYC(0x5b31, 0x5b34); A = mem_rd(gb, 0xcc60);
  CYC(0x5b34, 0x5b35); alu_or(gb, C);
  CYC(0x5b35, 0x5b37); A = 0x18;
  if ((F & FZ)) { CALL_C_CC(0x5b37, specialObjectSetAnimation_hook, 0x2b0a, 0x5b3a); } else CYC(0x5b37, 0x5b3a);
L_5b3a:
  CYC(0x5b3a, 0x5b3b); alu_xor(gb, A);
  CYC(0x5b3b, 0x5b3d); E = 0x12;
  CYC(0x5b3d, 0x5b3e); mem_wr(gb, DE, A);
  CYC(0x5b3e, 0x5b3f); E = alu_inc8(gb, E);
  CYC(0x5b3f, 0x5b40); mem_wr(gb, DE, A);
  CYC(0x5b40, 0x5b43); SET_HL(0xcc5c);
  CYC(0x5b43, 0x5b45); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(0x5b45, 0x5b47); goto L_5b4a; } CYC(0x5b45, 0x5b47);
  CYC(0x5b47, 0x5b49); E = 0x33;
  CYC(0x5b49, 0x5b4a); mem_wr(gb, DE, A);
L_5b4a:
  CYC(0x5b4a, 0x5b4c); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(0x5b4c, 0x5b4e); C = 0x20;
  if ((F & FZ)) { CYCT(0x5b4e, 0x5b50); goto L_5b52; } CYC(0x5b4e, 0x5b50);
  CYC(0x5b50, 0x5b52); C = 0x0a;
L_5b52:
  CALL_C(0x5b52, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5b55);
  CYC(0x5b55, 0x5b57); L = 0x15;
  if ((F & FZ)) { CYCT(0x5b57, 0x5b59); goto L_5b66; } CYC(0x5b57, 0x5b59);
  CYC(0x5b59, 0x5b5a); A = mem_rd(gb, HL);
  CYC(0x5b5a, 0x5b5c); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x5b5c, 0x5b5d); ret_effect(gb); return; } CYC(0x5b5c, 0x5b5d);
  CYC(0x5b5d, 0x5b5f); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(0x5b5f, 0x5b60); ret_effect(gb); return; } CYC(0x5b5f, 0x5b60);
  CYC(0x5b60, 0x5b62); mem_wr(gb, HL, 0x03);
  CYC(0x5b62, 0x5b63); L = alu_dec8(gb, L);
  CYC(0x5b63, 0x5b65); mem_wr(gb, HL, 0x00);
  CYC(0x5b65, 0x5b66); ret_effect(gb); return;
L_5b66:
  CYC(0x5b66, 0x5b67); alu_xor(gb, A);
  CYC(0x5b67, 0x5b68); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5b68, 0x5b69); mem_wr(gb, HL, A);
  CYC(0x5b69, 0x5b6c); mem_wr(gb, 0xcc5c, A);
  CYC(0x5b6c, 0x5b6e); E = 0x36;
  CYC(0x5b6e, 0x5b6f); mem_wr(gb, DE, A);
  CALL_C(0x5b6f, animateLinkStanding_hook, 0x5cb5, 0x5b72);
  CALL_C(0x5b72, specialObjectSetPositionToVar38IfSet_hook, 0x6016, 0x5b75);
  CALL_C(0x5b75, linkApplyTileTypes_hook, 0x42b7, 0x5b78);
  CYC(0x5b78, 0x5b7b); A = mem_rd(gb, 0xcc9c);
  CYC(0x5b7b, 0x5b7c); A = alu_dec8(gb, A);
  CYC(0x5b7c, 0x5b7e); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x5b7e, 0x5b80); goto L_5b85; } CYC(0x5b7e, 0x5b80);
  CYC(0x5b80, 0x5b82); A = 0x04;
  CYC(0x5b82, 0x5b85); mem_wr(gb, 0xcc9b, A);
L_5b85:
  CYC(0x5b85, 0x5b87); A = 0xa3;
  CALL_C(0x5b87, playSound_b00_hook, 0x0c98, 0x5b8a);
  CALL_C(0x5b8a, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x5b8d);
  CYC(0x5b8d, 0x5b90); initLinkState_hook(gb); return;
}

void linkUpdateSwimming_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5698, 0x569b); A = mem_rd(gb, 0xcc5d);
  CYC(0x569b, 0x569d); alu_and(gb, 0x0f);
  CYC(0x569d, 0x56a0); SET_HL(0xcc95);
  CYC(0x56a0, 0x56a2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CYC(0x56a2, 0x56a3); push_effect(gb, 0x56a3);
  switch (link_jump_table(gb)) { case 0x5ca1: initLinkState_hook(gb); return; case 0x56ad: overworldSwimmingState1_hook(gb); return; case 0x571b: overworldSwimmingState2_hook(gb); return; case 0x5725: overworldSwimmingState3_hook(gb); return; case 0x5759: linkUpdateDrowning_hook(gb); return; default: hook_continue(gb, HL, sp0_); return; }
}

void linkUpdateFlippersSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57a0, 0x57a2); E = 0x35;
  CYC(0x57a2, 0x57a3); A = mem_rd(gb, DE);
  CYC(0x57a3, 0x57a4); push_effect(gb, 0x57a4);
  switch (link_jump_table(gb)) { case 0x57aa: goto L_57aa; case 0x57d5: goto L_57d5; case 0x57da: goto L_57da; default: hook_continue(gb, HL, sp0_); return; }
L_57aa:
  CYC(0x57aa, 0x57ad); A = mem_rd(gb, 0xcc2a);
  CYC(0x57ad, 0x57af); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x57af, 0x57b1); goto L_57b8; } CYC(0x57af, 0x57b1);
  CALL_C(0x57b1, linkSetSwimmingSpeedTmp_hook, 0x5791, 0x57b4);
  CYC(0x57b4, 0x57b7); A = mem_rd(gb, 0xcc2b);
  CYC(0x57b7, 0x57b8); ret_effect(gb); return;
L_57b8:
  CYC(0x57b8, 0x57ba); A = 0x01;
  CYC(0x57ba, 0x57bb); mem_wr(gb, DE, A);
  CYC(0x57bb, 0x57bd); A = 0x08;
L_57bd:
  CYC(0x57bd, 0x57be); push_effect(gb, AF);
  CYC(0x57be, 0x57c0); E = 0x08;
  CYC(0x57c0, 0x57c1); A = mem_rd(gb, DE);
  CYC(0x57c1, 0x57c2); alu_add(gb, A);
  CYC(0x57c2, 0x57c3); alu_add(gb, A);
  CYC(0x57c3, 0x57c4); alu_add(gb, A);
  CALL_C(0x57c4, func_5933_hook, 0x5933, 0x57c7);
  CYC(0x57c7, 0x57c8); SET_AF(pop_effect(gb));
  CYC(0x57c8, 0x57c9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x57c9, 0x57cb); goto L_57bd; } CYC(0x57c9, 0x57cb);
  CYC(0x57cb, 0x57cd); E = 0x06;
  CYC(0x57cd, 0x57cf); A = 0x0d;
  CYC(0x57cf, 0x57d0); mem_wr(gb, DE, A);
  CYC(0x57d0, 0x57d2); A = 0x88;
  CALL_C(0x57d2, playSound_b00_hook, 0x0c98, 0x57d5);
L_57d5:
  CYC(0x57d5, 0x57d8); SET_BC(0x0105);
  CYC(0x57d8, 0x57da); goto L_57dd;
L_57da:
  CYC(0x57da, 0x57dd); SET_BC(0xfffb);
L_57dd:
  CALL_C(0x57dd, itemDecCounter1_hook, 0x23d6, 0x57e0);
  if ((F & FZ)) { CYCT(0x57e0, 0x57e2); goto L_57e9; } CYC(0x57e0, 0x57e2);
  CYC(0x57e2, 0x57e3); A = mem_rd(gb, HL);
  CYC(0x57e3, 0x57e5); alu_and(gb, 0x03);
  if ((F & FZ)) { CYCT(0x57e5, 0x57e7); goto L_57f9; } CYC(0x57e5, 0x57e7);
  CYC(0x57e7, 0x57e9); goto L_5803;
L_57e9:
  CYC(0x57e9, 0x57eb); L = 0x35;
  CYC(0x57eb, 0x57ec); B = alu_inc8(gb, B);
  CYC(0x57ec, 0x57ed); mem_wr(gb, HL, B);
  if (!(F & FZ)) { CYCT(0x57ed, 0x57ef); goto L_57f4; } CYC(0x57ed, 0x57ef);
  CALL_C(0x57ef, linkSetSwimmingSpeed_hook, 0x5778, 0x57f2);
  CYC(0x57f2, 0x57f4); goto L_5803;
L_57f4:
  CYC(0x57f4, 0x57f6); L = 0x06;
  CYC(0x57f6, 0x57f8); A = 0x0c;
  CYC(0x57f8, 0x57f9); mem_wr(gb, HL, A);
L_57f9:
  CYC(0x57f9, 0x57fb); L = 0x11;
  CYC(0x57fb, 0x57fc); A = mem_rd(gb, HL);
  CYC(0x57fc, 0x57fd); alu_add(gb, C);
  CYC(0x57fd, 0x57ff); alu_bit(gb, 7, A);
  if ((F & FZ)) { CYCT(0x57ff, 0x5801); goto L_5802; } CYC(0x57ff, 0x5801);
  CYC(0x5801, 0x5802); alu_xor(gb, A);
L_5802:
  CYC(0x5802, 0x5803); mem_wr(gb, HL, A);
L_5803:
  CYC(0x5803, 0x5806); A = mem_rd(gb, 0xcc2b);
  CYC(0x5806, 0x5808); alu_bit(gb, 7, A);
  if ((F & FZ)) { CYCT(0x5808, 0x5809); ret_effect(gb); return; } CYC(0x5808, 0x5809);
  CYC(0x5809, 0x580b); E = 0x08;
  CYC(0x580b, 0x580c); A = mem_rd(gb, DE);
  CYC(0x580c, 0x580e); A = alu_swap(gb, A);
  CYC(0x580e, 0x580f); alu_rrca(gb);
  CYC(0x580f, 0x5810); ret_effect(gb); return;
}

void checkLinkJumpingOffCliff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6034, 0x6037); A = mem_rd(gb, 0xcc2b);
  CYC(0x6037, 0x6038); C = A;
  CYC(0x6038, 0x603a); alu_and(gb, 0xe7);
  if (!(F & FZ)) { CYCT(0x603a, 0x603b); ret_effect(gb); return; } CYC(0x603a, 0x603b);
  CYC(0x603b, 0x603c); H = D;
  CYC(0x603c, 0x603e); L = 0x09;
  CYC(0x603e, 0x603f); alu_xor(gb, C);
  CYC(0x603f, 0x6040); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6040, 0x6041); ret_effect(gb); return; } CYC(0x6040, 0x6041);
  CYC(0x6041, 0x6042); alu_add(gb, A);
  CYC(0x6042, 0x6044); A = alu_swap(gb, A);
  CYC(0x6044, 0x6045); C = A;
  CYC(0x6045, 0x6046); alu_add(gb, A);
  CYC(0x6046, 0x6047); alu_add(gb, A);
  CYC(0x6047, 0x6048); alu_add(gb, C);
  CYC(0x6048, 0x604b); SET_HL(0x6091);
  CYC(0x604b, 0x604c); link_add_a_to_hl(gb, 0x604c);
  CYC(0x604c, 0x604e); E = 0x33;
  CYC(0x604e, 0x604f); A = mem_rd(gb, DE);
  CYC(0x604f, 0x6050); alu_and(gb, mem_rd(gb, HL));
  CYC(0x6050, 0x6051); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x6051, 0x6052); ret_effect(gb); return; } CYC(0x6051, 0x6052);
  CALL_ROM(0x6052, 0x6076);
  if (!(F & FC)) { CYCT(0x6055, 0x6056); ret_effect(gb); return; } CYC(0x6055, 0x6056);
  CALL_ROM(0x6056, 0x6076);
  if (!(F & FC)) { CYCT(0x6059, 0x605a); ret_effect(gb); return; } CYC(0x6059, 0x605a);
  CYC(0x605a, 0x605c); A = 0x81;
  CYC(0x605c, 0x605f); mem_wr(gb, 0xcc5c, A);
  CYC(0x605f, 0x6062); SET_BC(0xfe40);
  CALL_C(0x6062, objectSetSpeedZ_hook, 0x239d, 0x6065);
  CYC(0x6065, 0x6067); L = 0x2d;
  CYC(0x6067, 0x6069); mem_wr(gb, HL, 0x00);
  CYC(0x6069, 0x606a); SET_HL(pop_effect(gb));
  CYC(0x606a, 0x606c); A = 0x12;
  CALL_C(0x606c, linkSetState_hook, 0x54c9, 0x606f);
  CYC(0x606f, 0x6071); linkState12(gb); return;
}

void specialObjectSetPositionToVar38IfSet_hook(GB *gb) {
  CYC(0x6016, 0x6018); E = 0x37;
  CYC(0x6018, 0x6019); A = mem_rd(gb, DE);
  CYC(0x6019, 0x601a); alu_or(gb, A);
  if ((F & FZ)) { CYCT(0x601a, 0x601b); ret_effect(gb); return; } CYC(0x601a, 0x601b);
  CYC(0x601b, 0x601e); SET_HL(0x6010);
  CYC(0x601e, 0x601f); link_add_double_index(gb, 0x601f);
  CYC(0x601f, 0x6020); E = alu_inc8(gb, E);
  CYC(0x6020, 0x6021); A = mem_rd(gb, DE);
  CYC(0x6021, 0x6022); C = A;
  CYC(0x6022, 0x6024); alu_and(gb, 0xf0);
  CYC(0x6024, 0x6025); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6025, 0x6027); E = 0x0b;
  CYC(0x6027, 0x6028); mem_wr(gb, DE, A);
  CYC(0x6028, 0x6029); SET_HL(HL + 1);
  CYC(0x6029, 0x602a); A = C;
  CYC(0x602a, 0x602c); alu_and(gb, 0x0f);
  CYC(0x602c, 0x602e); A = alu_swap(gb, A);
  CYC(0x602e, 0x602f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x602f, 0x6031); E = 0x0d;
  CYC(0x6031, 0x6032); mem_wr(gb, DE, A);
  CYC(0x6032, 0x6034); clearVar37AndVar38(gb); return;
}

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

void warpTransition2_hook(GB *gb) {
  CYC(0x4a9b, 0x4a9d); A = 0x03;
  CYC(0x4a9d, 0x4aa0); W8(wWarpTransition2) = A;
  CYC(0x4aa0, 0x4aa2); A = 0x6e;
  CYC(0x4aa2, 0x4aa5);
  playSound_b00_hook(gb);
}

void warpTransition4_hook(GB *gb) {
  CYC(0x4b68, 0x4b6b); A = W8(wWarpTransition);
  CYC(0x4b6b, 0x4b6c); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x4b6c, 0x4b6f);
    warpTransition0_hook(gb);
    return;
  }
  CYC(0x4b6c, 0x4b6f);
  CYC(0x4b6f, 0x4b71); A = 0x01;
  CYC(0x4b71, 0x4b74); W8(wWarpTransition2) = A;
  CYC(0x4b74, 0x4b76); A = 0x6e;
  CYC(0x4b76, 0x4b79);
  playSound_b00_hook(gb);
}

void warpTransition5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b79, 0x4b7b); E = 0x05;
  CYC(0x4b7b, 0x4b7c); A = mem_rd(gb, DE);
  CYC(0x4b7c, 0x4b7d); push_effect(gb, 0x4b7d);
  switch (link_jump_table(gb)) {
    case 0x4b83: warpTransition5_00_hook(gb); return;
    case 0x4ba1: warpTransition5_01_hook(gb); return;
    case 0x4c19: warpTransition5_02_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void warpTransition5_00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b83, 0x4b85); A = 0x01;
  CYC(0x4b85, 0x4b86); mem_wr(gb, DE, A);
  CYC(0x4b86, 0x4b89); SET_BC(0x0020);
  CALL_C(0x4b89, objectSetSpeedZ_hook, 0x239d, 0x4b8c);
  CALL_C(0x4b8c, objectGetZAboveScreen_hook, 0x2172, 0x4b8f);
  CYC(0x4b8f, 0x4b91); L = (uint8_t)w1Link_zh;
  CYC(0x4b91, 0x4b92); mem_wr(gb, HL, A);
  CYC(0x4b92, 0x4b94); L = (uint8_t)w1Link_yh;
  CYC(0x4b94, 0x4b95); A = mem_rd(gb, HL);
  CYC(0x4b95, 0x4b97); alu_sub(gb, 0x04);
  CYC(0x4b97, 0x4b98); mem_wr(gb, HL, A);
  CYC(0x4b98, 0x4b9a); L = (uint8_t)w1Link_direction;
  CYC(0x4b9a, 0x4b9c); mem_wr(gb, HL, 0x02);
  CYC(0x4b9c, 0x4b9e); A = 0x04;
  CYC(0x4b9e, 0x4ba1);
  specialObjectSetAnimation_hook(gb);
}

static void warp_transition7_link_collapsed(GB *gb, uint16_t sp0_) {
  CALL_C(0x4c05, itemIncSubstate_hook, 0x23ef, 0x4c08);
  CYC(0x4c08, 0x4c0a); L = (uint8_t)w1Link_counter1;
  CYC(0x4c0a, 0x4c0c); mem_wr(gb, HL, 0x1e);
  CYC(0x4c0c, 0x4c0e); A = 0x02;
  CALL_C(0x4c0e, specialObjectSetAnimation_hook, 0x2b0a, 0x4c11);
  CYC(0x4c11, 0x4c13); A = 0x87;
  CYC(0x4c13, 0x4c16);
  playSound_b00_hook(gb);
}

void warpTransition5_01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ba1, specialObjectAnimate_hook, 0x2aef, 0x4ba4);
  CYC(0x4ba4, 0x4ba6); C = 0x20;
  CALL_C(0x4ba6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4ba9);
  if (!(F & FZ)) { CYCT(0x4ba9, 0x4baa); ret_effect(gb); return; }
  CYC(0x4ba9, 0x4baa);
  CYC(0x4baa, 0x4bad); SET_HL(0x2408);
  CALL_C(0x4bad, lookupCollisionTable_hook, 0x1e1f, 0x4bb0);
  if (!(F & FC)) {
    CYCT(0x4bb0, 0x4bb3);
    warp_transition7_link_collapsed(gb, sp0_);
    return;
  }
  CYC(0x4bb0, 0x4bb3);
  CYC(0x4bb3, 0x4bb6);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransition7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4bb6, 0x4bb8); E = 0x05;
  CYC(0x4bb8, 0x4bb9); A = mem_rd(gb, DE);
  CYC(0x4bb9, 0x4bba); push_effect(gb, 0x4bba);
  switch (link_jump_table(gb)) {
    case 0x4bc2: goto state0;
    case 0x4be3: goto state1;
    case 0x4bfc: goto state2;
    case 0x4c16: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4bc2, 0x4bc4); A = 0x01;
  CYC(0x4bc4, 0x4bc5); mem_wr(gb, DE, A);
  CYC(0x4bc5, 0x4bc6); H = D;
  CYC(0x4bc6, 0x4bc8); L = (uint8_t)w1Link_direction;
  CYC(0x4bc8, 0x4bca); mem_wr(gb, HL, 0x02);
  CYC(0x4bca, 0x4bcb); L = alu_inc8(gb, L);
  CYC(0x4bcb, 0x4bcd); mem_wr(gb, HL, 0x10);
  CYC(0x4bcd, 0x4bcf); L = (uint8_t)w1Link_speed;
  CYC(0x4bcf, 0x4bd1); mem_wr(gb, HL, 0x28);
  CYC(0x4bd1, 0x4bd3); L = (uint8_t)w1Link_visible;
  CYC(0x4bd3, 0x4bd5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(0x4bd5, 0x4bd7); L = (uint8_t)w1Link_counter1;
  CYC(0x4bd7, 0x4bd9); mem_wr(gb, HL, 0x78);
  CYC(0x4bd9, 0x4bdb); A = 0x04;
  CALL_C(0x4bdb, specialObjectSetAnimation_hook, 0x2b0a, 0x4bde);
  CYC(0x4bde, 0x4be0); A = 0x65;
  CYC(0x4be0, 0x4be3);
  playSound_b00_hook(gb);
  return;

state1:
  CALL_C(0x4be3, itemDecCounter1_hook, 0x23d6, 0x4be6);
  if (!(F & FZ)) { CYCT(0x4be6, 0x4be7); ret_effect(gb); return; }
  CYC(0x4be6, 0x4be7);
  CYC(0x4be7, 0x4be9); L = 0x05;
  CYC(0x4be9, 0x4bea); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4bea, 0x4bec); L = (uint8_t)w1Link_visible;
  CYC(0x4bec, 0x4bee); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x4bee, 0x4bf0); L = (uint8_t)w1Link_counter1;
  CYC(0x4bf0, 0x4bf2); mem_wr(gb, HL, 0x30);
  CYC(0x4bf2, 0x4bf4); A = 0x10;
  CALL_C(0x4bf4, setScreenShakeCounter_hook, 0x24bb, 0x4bf7);
  CYC(0x4bf7, 0x4bf9); A = 0x85;
  CYC(0x4bf9, 0x4bfc);
  playSound_b00_hook(gb);
  return;

state2:
  CALL_C(0x4bfc, specialObjectAnimate_hook, 0x2aef, 0x4bff);
  CALL_C(0x4bff, itemDecCounter1_hook, 0x23d6, 0x4c02);
  if (!(F & FZ)) {
    CYCT(0x4c02, 0x4c05);
    specialObjectUpdatePosition_hook(gb);
    return;
  }
  CYC(0x4c02, 0x4c05);
  warp_transition7_link_collapsed(gb, sp0_);
  return;

state3:
  CALL_C(0x4c16, setDeathRespawnPoint_hook, 0x1100, 0x4c19);
  warpTransition5_02_hook(gb);
}

void warpTransition5_02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4c19, itemDecCounter1_hook, 0x23d6, 0x4c1c);
  if (!(F & FZ)) { CYCT(0x4c1c, 0x4c1d); ret_effect(gb); return; }
  CYC(0x4c1c, 0x4c1d);
  CYC(0x4c1d, 0x4c20);
  initLinkStateAndAnimateStanding(gb);
}

void linkIncrementDirectionOnOddFrames_hook(GB *gb) {
  CYC(0x4c20, 0x4c23); A = W8(wFrameCounter);
  CYC(0x4c23, 0x4c24); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4c24, 0x4c25); ret_effect(gb); return; }
  CYC(0x4c24, 0x4c25);
  linkIncrementDirection_hook(gb);
}

void linkIncrementDirection_hook(GB *gb) {
  CYC(0x4c25, 0x4c27); E = (uint8_t)w1Link_direction;
  CYC(0x4c27, 0x4c28); A = mem_rd(gb, DE);
  CYC(0x4c28, 0x4c29); A = alu_inc8(gb, A);
  CYC(0x4c29, 0x4c2b); alu_and(gb, 0x03);
  CYC(0x4c2b, 0x4c2c); mem_wr(gb, DE, A);
  CYC(0x4c2c, 0x4c2d); ret_effect(gb);
}

void warpTransition8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c2d, 0x4c2f); E = 0x05;
  CYC(0x4c2f, 0x4c30); A = mem_rd(gb, DE);
  CYC(0x4c30, 0x4c31); push_effect(gb, 0x4c31);
  switch (link_jump_table(gb)) {
    case 0x4c41: goto state0;
    case 0x4c6b: goto state1;
    case 0x4c8a: goto state2;
    case 0x4ca4: goto state3;
    case 0x4cb2: goto state4;
    case 0x4cc3: goto state5;
    case 0x4cd2: goto state6;
    case 0x4cec: goto state7;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x4c41, 0x4c43); A = 0x01;
  CYC(0x4c43, 0x4c44); mem_wr(gb, DE, A);
  CYC(0x4c44, 0x4c46); A = 0xff;
  CYC(0x4c46, 0x4c49); W8(wDisabledObjects) = A;
  CYC(0x4c49, 0x4c4b); A = 0x80;
  CYC(0x4c4b, 0x4c4e); W8(wMenuDisabled) = A;
  CYC(0x4c4e, 0x4c50); A = 0x15;
  CYC(0x4c50, 0x4c53); W8(wCutsceneTrigger) = A;
  CYC(0x4c53, 0x4c56); SET_BC(0xff60);
  CALL_C(0x4c56, objectSetSpeedZ_hook, 0x239d, 0x4c59);
  CYC(0x4c59, 0x4c5b); L = 0x06;
  CYC(0x4c5b, 0x4c5d); mem_wr(gb, HL, 0x30);
  CALL_C(0x4c5d, linkCancelAllItemUsage_hook, 0x4f49, 0x4c60);
  CALL_C(0x4c60, restartSound_hook, 0x0cb2, 0x4c63);
  CYC(0x4c63, 0x4c65); A = 0xb4;
  CALL_C(0x4c65, playSound_b00_hook, 0x0c98, 0x4c68);
  CYC(0x4c68, 0x4c6b);
  objectCenterOnTile_hook(gb);
  return;

state1:
  CYC(0x4c6b, 0x4c6d); C = 0x02;
  CALL_C(0x4c6d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4c70);
  CYC(0x4c70, 0x4c73); A = W8(wFrameCounter);
  CYC(0x4c73, 0x4c75); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x4c75, 0x4c77);
  } else {
    CYC(0x4c75, 0x4c77);
    CYC(0x4c77, 0x4c7a); SET_HL(wTmpcbbc);
    CYC(0x4c7a, 0x4c7b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(0x4c7b, 0x4c7e); A = W8(wFrameCounter);
  CYC(0x4c7e, 0x4c80); alu_and(gb, 0x03);
  if (F & FZ) CALL_C_CC(0x4c80, linkIncrementDirection_hook, 0x4c25, 0x4c83);
  else CYC(0x4c80, 0x4c83);
  CALL_C(0x4c83, itemDecCounter1_hook, 0x23d6, 0x4c86);
  if (!(F & FZ)) { CYCT(0x4c86, 0x4c87); ret_effect(gb); return; }
  CYC(0x4c86, 0x4c87);
  CYC(0x4c87, 0x4c8a);
  itemIncSubstate_hook(gb);
  return;

state2:
  CYC(0x4c8a, 0x4c8c); C = 0x02;
  CALL_C(0x4c8c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4c8f);
  CALL_C(0x4c8f, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4c92);
  CYC(0x4c92, 0x4c93); H = D;
  CYC(0x4c93, 0x4c95); L = 0x15;
  CYC(0x4c95, 0x4c97); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4c97, 0x4c98); ret_effect(gb); return; }
  CYC(0x4c97, 0x4c98);
  CYC(0x4c98, 0x4c9a); L = 0x06;
  CYC(0x4c9a, 0x4c9c); mem_wr(gb, HL, 0x28);
  CYC(0x4c9c, 0x4c9e); A = 0x02;
  CALL_C(0x4c9e, fadeoutToWhiteWithDelay_hook, 0x3257, 0x4ca1);
  CYC(0x4ca1, 0x4ca4);
  itemIncSubstate_hook(gb);
  return;

state3:
  CALL_C(0x4ca4, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4ca7);
  CALL_C(0x4ca7, itemDecCounter1_hook, 0x23d6, 0x4caa);
  if (!(F & FZ)) { CYCT(0x4caa, 0x4cab); ret_effect(gb); return; }
  CYC(0x4caa, 0x4cab);
  CYC(0x4cab, 0x4cae); SET_HL(wTmpcbb3);
  CYC(0x4cae, 0x4caf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4caf, 0x4cb2);
  itemIncSubstate_hook(gb);
  return;

state4:
  CALL_C(0x4cb2, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4cb5);
  CYC(0x4cb5, 0x4cb8); A = W8(wCutsceneState);
  CYC(0x4cb8, 0x4cba); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x4cba, 0x4cbb); ret_effect(gb); return; }
  CYC(0x4cba, 0x4cbb);
  CALL_C(0x4cbb, itemIncSubstate_hook, 0x23ef, 0x4cbe);
  CYC(0x4cbe, 0x4cc0); L = 0x06;
  CYC(0x4cc0, 0x4cc2); mem_wr(gb, HL, 0x28);
  CYC(0x4cc2, 0x4cc3); ret_effect(gb);
  return;

state5:
  CYC(0x4cc3, 0x4cc5); C = 0x02;
  CALL_C(0x4cc5, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4cc8);
  CALL_C(0x4cc8, linkIncrementDirectionOnOddFrames_hook, 0x4c20, 0x4ccb);
  CALL_C(0x4ccb, itemDecCounter1_hook, 0x23d6, 0x4cce);
  if (!(F & FZ)) { CYCT(0x4cce, 0x4ccf); ret_effect(gb); return; }
  CYC(0x4cce, 0x4ccf);
  CYC(0x4ccf, 0x4cd2);
  itemIncSubstate_hook(gb);
  return;

state6:
  CYC(0x4cd2, 0x4cd4); C = 0x02;
  CALL_C(0x4cd4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4cd7);
  CYC(0x4cd7, 0x4cda); A = W8(wFrameCounter);
  CYC(0x4cda, 0x4cdc); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x4cdc, 0x4cdd); ret_effect(gb); return; }
  CYC(0x4cdc, 0x4cdd);
  CALL_C(0x4cdd, linkIncrementDirection_hook, 0x4c25, 0x4ce0);
  CYC(0x4ce0, 0x4ce3); SET_HL(wTmpcbbc);
  CYC(0x4ce3, 0x4ce4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x4ce4, 0x4ce5); ret_effect(gb); return; }
  CYC(0x4ce4, 0x4ce5);
  CYC(0x4ce5, 0x4ce8); SET_HL(wTmpcbb3);
  CYC(0x4ce8, 0x4ce9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4ce9, 0x4cec);
  itemIncSubstate_hook(gb);
  return;

state7:
  CYC(0x4cec, 0x4cef); A = W8(wDisabledObjects);
  CYC(0x4cef, 0x4cf1); alu_and(gb, 0x81);
  if (F & FZ) {
    CYCT(0x4cf1, 0x4cf3);
    goto finish;
  }
  CYC(0x4cf1, 0x4cf3);
  CYC(0x4cf3, 0x4cf6); A = W8(wFrameCounter);
  CYC(0x4cf6, 0x4cf8); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x4cf8, 0x4cf9); ret_effect(gb); return; }
  CYC(0x4cf8, 0x4cf9);
  CYC(0x4cf9, 0x4cfc);
  linkIncrementDirection_hook(gb);
  return;

finish:
  CYC(0x4cfc, 0x4cfe); E = (uint8_t)w1Link_direction;
  CYC(0x4cfe, 0x4cff); A = mem_rd(gb, DE);
  CYC(0x4cff, 0x4d01); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x4d01, 0x4d04);
    linkIncrementDirection_hook(gb);
    return;
  }
  CYC(0x4d01, 0x4d04);
  CYC(0x4d04, 0x4d07); A = W8(wActiveMusic2);
  CYC(0x4d07, 0x4d0a); W8(wActiveMusic) = A;
  CALL_C(0x4d0a, playSound_b00_hook, 0x0c98, 0x4d0d);
  CALL_C(0x4d0d, setDeathRespawnPoint_hook, 0x1100, 0x4d10);
  CALL_C(0x4d10, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4d13);
  CALL_C(0x4d13, resetLinkInvincibility_hook, 0x2ba9, 0x4d16);
  CYC(0x4d16, 0x4d19);
  initLinkStateAndAnimateStanding(gb);
}

void linkState0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a26, 0x4a29); A = W8(wWarpTransition);
  CYC(0x4a29, 0x4a2b); alu_and(gb, 0x0f);
  CYC(0x4a2b, 0x4a2c); push_effect(gb, 0x4a2c);
  switch (link_jump_table(gb)) {
    case 0x4a4c: warpTransition0_hook(gb); return;
    case 0x4a59: warpTransition1_hook(gb); return;
    case 0x4a9b: warpTransition2_hook(gb); return;
    case 0x4aa5: warpTransition3_hook(gb); return;
    case 0x4b68: warpTransition4_hook(gb); return;
    case 0x4b79: warpTransition5_hook(gb); return;
    case 0x4d89: warpTransition6_hook(gb); return;
    case 0x4a4f: warpTransitionA_hook(gb); return;
    case 0x4c2d: warpTransition8_hook(gb); return;
    case 0x4d19: warpTransition9_hook(gb); return;
    case 0x4d47: warpTransitionB_hook(gb); return;
    case 0x4a6c: warpTransitionC_hook(gb); return;
    case 0x4a52: warpTransitionE_hook(gb); return;
    case 0x4d83: warpTransitionF_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void linkState00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x49e4, clearAllParentItems_hook, 0x2c10, 0x49e7);
  CALL_C(0x49e7, specialObjectSetOamVariables_hook, 0x41f7, 0x49ea);
  CYC(0x49ea, 0x49ec); A = 0x10;
  CALL_C(0x49ec, specialObjectSetAnimation_hook, 0x2b0a, 0x49ef);
  CYC(0x49ef, 0x49f0); H = D;
  CYC(0x49f0, 0x49f2); L = 0x24;
  CYC(0x49f2, 0x49f4); A = 0x80;
  CYC(0x49f4, 0x49f5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49f5, 0x49f6); L = alu_inc8(gb, L);
  CYC(0x49f6, 0x49f8); A = 0x06;
  CYC(0x49f8, 0x49f9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49f9, 0x49fa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49fa, 0x49fc); L = 0x29;
  CYC(0x49fc, 0x49fe); mem_wr(gb, HL, 0x01);
  CYC(0x49fe, 0x4a01); A = W8(wLinkForceState);
  CYC(0x4a01, 0x4a03); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(0x4a03, 0x4a05);
  } else {
    CYC(0x4a03, 0x4a05);
    CYC(0x4a05, 0x4a08); A = W8(wDisableRingTransformations);
    CYC(0x4a08, 0x4a09); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(0x4a09, 0x4a0b);
    } else {
      CYC(0x4a09, 0x4a0b);
      CALL_C(0x4a0b, objectGetTileCollisions_hook, 0x14ad, 0x4a0e);
      CYC(0x4a0e, 0x4a10); alu_cp(gb, 0x0f);
      if (!(F & FZ)) {
        CYCT(0x4a10, 0x4a12);
      } else {
        CYC(0x4a10, 0x4a12);
        CYC(0x4a12, 0x4a15); SET_HL(wLastAnimalMountPointY);
        CYC(0x4a15, 0x4a16); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(0x4a16, 0x4a18); E = (uint8_t)w1Link_yh;
        CYC(0x4a18, 0x4a19); mem_wr(gb, DE, A);
        CYC(0x4a19, 0x4a1a); A = mem_rd(gb, HL);
        CYC(0x4a1a, 0x4a1c); E = (uint8_t)w1Link_xh;
        CYC(0x4a1c, 0x4a1d); mem_wr(gb, DE, A);
      }
    }
  }
  CALL_C(0x4a1d, objectSetVisiblec1_hook, 0x1e3c, 0x4a20);
  CALL_C(0x4a20, checkLinkForceState_hook, 0x54c0, 0x4a23);
  CYC(0x4a23, 0x4a26);
  initLinkStateAndAnimateStanding(gb);
}

void linkState08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f0c, 0x4f0e); E = 0x05;
  CYC(0x4f0e, 0x4f0f); A = mem_rd(gb, DE);
  CYC(0x4f0f, 0x4f10); push_effect(gb, 0x4f10);
  switch (link_jump_table(gb)) {
    case 0x4f14: goto substate0;
    case 0x4f27: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4f14, 0x4f16); A = 0x01;
  CYC(0x4f16, 0x4f17); mem_wr(gb, DE, A);
  CYC(0x4f17, 0x4f1a); SET_HL(wcc50);
  CYC(0x4f1a, 0x4f1b); A = mem_rd(gb, HL);
  CYC(0x4f1b, 0x4f1d); mem_wr(gb, HL, 0x00);
  CYC(0x4f1d, 0x4f1e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f1e, 0x4f1f); ret_effect(gb); return; }
  CYC(0x4f1e, 0x4f1f);
  CALL_C(0x4f1f, linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook, 0x4f45, 0x4f22);
  CYC(0x4f22, 0x4f24); A = 0x10;
  CYC(0x4f24, 0x4f27);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x4f27, checkLinkForceState_hook, 0x54c0, 0x4f2a);
  CYC(0x4f2a, 0x4f2d); SET_HL(wcc50);
  CYC(0x4f2d, 0x4f2e); A = mem_rd(gb, HL);
  CYC(0x4f2e, 0x4f2f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(0x4f2f, 0x4f31);
    CYC(0x4f31, 0x4f33); mem_wr(gb, HL, 0x00);
    CALL_C(0x4f33, specialObjectSetAnimation_hook, 0x2b0a, 0x4f36);
  } else {
    CYCT(0x4f2f, 0x4f31);
  }
  CYC(0x4f36, 0x4f39); A = W8(wcc63);
  CYC(0x4f39, 0x4f3a); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x4f3a, checkUseItems_b00_hook, 0x2c18, 0x4f3d);
  else CYC(0x4f3a, 0x4f3d);
  CYC(0x4f3d, 0x4f40); A = W8(wDisabledObjects);
  CYC(0x4f40, 0x4f41); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f41, 0x4f42); ret_effect(gb); return; }
  CYC(0x4f41, 0x4f42);
  CYC(0x4f42, 0x4f45);
  initLinkStateAndAnimateStanding(gb);
}

void linkState04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4ff6, 0x4ff8); E = 0x05;
  CYC(0x4ff8, 0x4ff9); A = mem_rd(gb, DE);
  CYC(0x4ff9, 0x4ffa); push_effect(gb, 0x4ffa);
  switch (link_jump_table(gb)) {
    case 0x4ffe: goto substate0;
    case 0x5014: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4ffe, 0x5000); A = 0x01;
  CYC(0x5000, 0x5001); mem_wr(gb, DE, A);
  CALL_C(0x5001, linkCancelAllItemUsage_hook, 0x4f49, 0x5004);
  CYC(0x5004, 0x5006); E = 0x30;
  CYC(0x5006, 0x5007); A = mem_rd(gb, DE);
  CYC(0x5007, 0x500a); W8(wcc52) = A;
  CYC(0x500a, 0x500d); A = W8(wcc50);
  CYC(0x500d, 0x500f); alu_and(gb, 0x0f);
  CYC(0x500f, 0x5011); alu_add(gb, 0x0e);
  CYC(0x5011, 0x5014);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x5014, retIfTextIsActive_hook, 0x1859, 0x5017);
  CYC(0x5017, 0x501a); A = W8(wcc50);
  CYC(0x501a, 0x501b); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x501b, 0x501d);
  } else {
    CYC(0x501b, 0x501d);
    CYC(0x501d, 0x5020); A = W8(wDisabledObjects);
    CYC(0x5020, 0x5022); alu_and(gb, 0x81);
    if (!(F & FZ)) { CYCT(0x5022, 0x5023); ret_effect(gb); return; }
    CYC(0x5022, 0x5023);
  }
  CYC(0x5023, 0x5025); E = 0x04;
  CYC(0x5025, 0x5027); A = 0x01;
  CYC(0x5027, 0x5028); mem_wr(gb, DE, A);
  CYC(0x5028, 0x502b); A = W8(wcc52);
  CYC(0x502b, 0x502e);
  specialObjectSetAnimation_hook(gb);
}

void warpTransition9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d19, 0x4d1b); E = 0x05;
  CYC(0x4d1b, 0x4d1c); A = mem_rd(gb, DE);
  CYC(0x4d1c, 0x4d1d); push_effect(gb, 0x4d1d);
  switch (link_jump_table(gb)) {
    case 0x4d21: goto substate0;
    case 0x4d3a: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4d21, itemIncSubstate_hook, 0x23ef, 0x4d24);
  CYC(0x4d24, 0x4d26); L = 0x0b;
  CYC(0x4d26, 0x4d28); A = 0x08;
  CYC(0x4d28, 0x4d29); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4d29, 0x4d2a); mem_wr(gb, HL, A);
  CALL_C(0x4d2a, objectCenterOnTile_hook, 0x20db, 0x4d2d);
  CALL_C(0x4d2d, clearAllParentItems_hook, 0x2c10, 0x4d30);
  CYC(0x4d30, 0x4d32); A = 0x0d;
  CALL_C(0x4d32, specialObjectSetAnimation_hook, 0x2b0a, 0x4d35);
  CYC(0x4d35, 0x4d37); A = 0x65;
  CYC(0x4d37, 0x4d3a);
  playSound_b00_hook(gb);
  return;

substate1:
  CYC(0x4d3a, 0x4d3c); E = 0x21;
  CYC(0x4d3c, 0x4d3d); A = mem_rd(gb, DE);
  CYC(0x4d3d, 0x4d3e); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x4d3e, 0x4d41); specialObjectAnimate_hook(gb); return; }
  CYC(0x4d3e, 0x4d41);
  CYC(0x4d41, 0x4d43); A = 0x03;
  CYC(0x4d43, 0x4d46); W8(wWarpTransition2) = A;
  CYC(0x4d46, 0x4d47); ret_effect(gb);
}

void warpTransitionB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d47, 0x4d49); E = 0x05;
  CYC(0x4d49, 0x4d4a); A = mem_rd(gb, DE);
  CYC(0x4d4a, 0x4d4b); push_effect(gb, 0x4d4b);
  switch (link_jump_table(gb)) {
    case 0x4d51: goto state0;
    case 0x4d63: goto state1;
    case 0x4d77: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x4d51, itemIncSubstate_hook, 0x23ef, 0x4d54);
  CALL_C(0x4d54, objectGetZAboveScreen_hook, 0x2172, 0x4d57);
  CYC(0x4d57, 0x4d59); L = 0x0f;
  CYC(0x4d59, 0x4d5a); mem_wr(gb, HL, A);
  CYC(0x4d5a, 0x4d5c); L = 0x08;
  CYC(0x4d5c, 0x4d5e); mem_wr(gb, HL, 0x02);
  CYC(0x4d5e, 0x4d60); A = 0x04;
  CYC(0x4d60, 0x4d63);
  specialObjectSetAnimation_hook(gb);
  return;

state1:
  CALL_C(0x4d63, specialObjectAnimate_hook, 0x2aef, 0x4d66);
  CYC(0x4d66, 0x4d68); C = 0x0c;
  CALL_C(0x4d68, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4d6b);
  if (!(F & FZ)) { CYCT(0x4d6b, 0x4d6c); ret_effect(gb); return; }
  CYC(0x4d6b, 0x4d6c);
  CALL_C(0x4d6c, itemIncSubstate_hook, 0x23ef, 0x4d6f);
  CALL_C(0x4d6f, animateLinkStanding_hook, 0x5cb5, 0x4d72);
  CYC(0x4d72, 0x4d74); A = 0x87;
  CYC(0x4d74, 0x4d77);
  playSound_b00_hook(gb);
  return;

state2:
  CYC(0x4d77, 0x4d7a); A = W8(wDisabledObjects);
  CYC(0x4d7a, 0x4d7c); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(0x4d7c, 0x4d7d); ret_effect(gb); return; }
  CYC(0x4d7c, 0x4d7d);
  CALL_C(0x4d7d, objectSetVisiblec2_hook, 0x1e45, 0x4d80);
  CYC(0x4d80, 0x4d83);
  initLinkStateAndAnimateStanding(gb);
}

void warpTransitionF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d83, checkLinkForceState_hook, 0x54c0, 0x4d86);
  CYC(0x4d86, 0x4d89);
  objectSetInvisible_hook(gb);
}

void linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook(GB *gb) {
  CYC(0x4f45, 0x4f47); E = 0x33;
  CYC(0x4f47, 0x4f48); alu_xor(gb, A);
  CYC(0x4f48, 0x4f49); mem_wr(gb, DE, A);
  linkCancelAllItemUsage_hook(gb);
}

void linkCancelAllItemUsage_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f49, dropLinkHeldItem_hook, 0x2c43, 0x4f4c);
  CYC(0x4f4c, 0x4f4f);
  clearAllParentItems_hook(gb);
}

void linkState0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f4f, 0x4f51); E = 0x05;
  CYC(0x4f51, 0x4f52); A = mem_rd(gb, DE);
  CYC(0x4f52, 0x4f53); push_effect(gb, 0x4f53);
  switch (link_jump_table(gb)) {
    case 0x4f59: goto substate0;
    case 0x4f62: goto substate1;
    case 0x4f6c: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4f59, itemIncSubstate_hook, 0x23ef, 0x4f5c);
  CYC(0x4f5c, 0x4f5e); E = 0x37;
  CYC(0x4f5e, 0x4f61); A = W8(wActiveRoom);
  CYC(0x4f61, 0x4f62); mem_wr(gb, DE, A);

substate1:
  CALL_C(0x4f62, objectCheckWithinScreenBoundary_hook, 0x2184, 0x4f65);
  if (F & FC) { CYCT(0x4f65, 0x4f66); ret_effect(gb); return; }
  CYC(0x4f65, 0x4f66);
  CALL_C(0x4f66, itemIncSubstate_hook, 0x23ef, 0x4f69);
  CALL_C(0x4f69, objectSetInvisible_hook, 0x1e7b, 0x4f6c);

substate2:
  CYC(0x4f6c, 0x4f6d); H = D;
  CYC(0x4f6d, 0x4f6f); L = 0x37;
  CYC(0x4f6f, 0x4f72); A = W8(wActiveRoom);
  CYC(0x4f72, 0x4f73); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4f73, 0x4f74); ret_effect(gb); return; }
  CYC(0x4f73, 0x4f74);
  CALL_C(0x4f74, objectCheckWithinScreenBoundary_hook, 0x2184, 0x4f77);
  if (!(F & FC)) { CYCT(0x4f77, 0x4f78); ret_effect(gb); return; }
  CYC(0x4f77, 0x4f78);
  CYC(0x4f78, 0x4f7a); E = 0x05;
  CYC(0x4f7a, 0x4f7c); A = 0x01;
  CYC(0x4f7c, 0x4f7d); mem_wr(gb, DE, A);
  CYC(0x4f7d, 0x4f80);
  objectSetVisiblec2_hook(gb);
}

void linkState0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f80, 0x4f83); A = W8(wTextIsActive);
  CYC(0x4f83, 0x4f84); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4f84, 0x4f85); ret_effect(gb); return; }
  CYC(0x4f84, 0x4f85);
  CYC(0x4f85, 0x4f87); E = 0x05;
  CYC(0x4f87, 0x4f88); A = mem_rd(gb, DE);
  CYC(0x4f88, 0x4f89); push_effect(gb, 0x4f89);
  switch (link_jump_table(gb)) {
    case 0x4f8f: goto substate0;
    case 0x4fb4: goto substate1;
    case 0x4fc0: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4f8f, itemIncSubstate_hook, 0x23ef, 0x4f92);
  CYC(0x4f92, 0x4f93); L = alu_inc8(gb, L);
  CYC(0x4f93, 0x4f95); mem_wr(gb, HL, 0x14);
  CYC(0x4f95, 0x4f97); L = 0x09;
  CYC(0x4f97, 0x4f99); mem_wr(gb, HL, 0x10);
  CYC(0x4f99, 0x4f9b); L = 0x0b;
  CYC(0x4f9b, 0x4f9d); mem_wr(gb, HL, 0x38);
  CYC(0x4f9d, 0x4f9f); L = 0x0d;
  CYC(0x4f9f, 0x4fa1); mem_wr(gb, HL, 0x50);
  CYC(0x4fa1, 0x4fa3); L = 0x10;
  CYC(0x4fa3, 0x4fa5); mem_wr(gb, HL, 0x28);
  CYC(0x4fa5, 0x4fa7); L = 0x14;
  CYC(0x4fa7, 0x4fa9); A = 0x80;
  CYC(0x4fa9, 0x4faa); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4faa, 0x4fac); mem_wr(gb, HL, 0xfe);
  CYC(0x4fac, 0x4fae); A = 0x02;
  CALL_C(0x4fae, specialObjectSetAnimation_hook, 0x2b0a, 0x4fb1);
  CYC(0x4fb1, 0x4fb4);
  objectSetVisiblec2_hook(gb);
  return;

substate1:
  CALL_C(0x4fb4, objectApplySpeed_hook, 0x201d, 0x4fb7);
  CYC(0x4fb7, 0x4fb9); C = 0x20;
  CALL_C(0x4fb9, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4fbc);
  if (!(F & FC)) { CYCT(0x4fbc, 0x4fbd); ret_effect(gb); return; }
  CYC(0x4fbc, 0x4fbd);
  CYC(0x4fbd, 0x4fc0);
  itemIncSubstate_hook(gb);
  return;

substate2:
  CALL_C(0x4fc0, itemDecCounter1_hook, 0x23d6, 0x4fc3);
  if (!(F & FZ)) { CYCT(0x4fc3, 0x4fc4); ret_effect(gb); return; }
  CYC(0x4fc3, 0x4fc4);
  CYC(0x4fc4, 0x4fc7);
  initLinkStateAndAnimateStanding(gb);
}

void linkState0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fc7, 0x4fc9); E = 0x05;
  CYC(0x4fc9, 0x4fca); A = mem_rd(gb, DE);
  CYC(0x4fca, 0x4fcb); push_effect(gb, 0x4fcb);
  switch (link_jump_table(gb)) {
    case 0x4fcf: goto substate0;
    case 0x4fe6: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x4fcf, 0x4fd1); A = 0x01;
  CYC(0x4fd1, 0x4fd2); mem_wr(gb, DE, A);
  CYC(0x4fd2, 0x4fd4); E = 0x06;
  CYC(0x4fd4, 0x4fd7); A = W8(wLinkStateParameter);
  CYC(0x4fd7, 0x4fd8); mem_wr(gb, DE, A);
  CALL_C(0x4fd8, clearPegasusSeedCounter_hook, 0x2a85, 0x4fdb);
  CALL_C(0x4fdb, linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook, 0x4f45, 0x4fde);
  CALL_C(0x4fde, updateLinkSpeed_standard_hook, 0x5ce6, 0x4fe1);
  CYC(0x4fe1, 0x4fe3); A = 0x10;
  CALL_C(0x4fe3, specialObjectSetAnimation_hook, 0x2b0a, 0x4fe6);

substate1:
  CALL_C(0x4fe6, specialObjectAnimate_hook, 0x2aef, 0x4fe9);
  CALL_C(0x4fe9, itemDecCounter1_hook, 0x23d6, 0x4fec);
  CYC(0x4fec, 0x4fee); L = 0x33;
  CYC(0x4fee, 0x4ff0); mem_wr(gb, HL, 0x00);
  if (!(F & FZ)) {
    CYCT(0x4ff0, 0x4ff3);
    specialObjectUpdatePosition_hook(gb);
    return;
  }
  CYC(0x4ff0, 0x4ff3);
  CYC(0x4ff3, 0x4ff6);
  initLinkStateAndAnimateStanding(gb);
}

void setLinkStateToDead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x502e, 0x5030); A = 0x03;
  CALL_C(0x5030, linkSetState_hook, 0x54c9, 0x5033);
  linkState03_hook(gb);
}

void linkState03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5033, 0x5034); alu_xor(gb, A);
  CYC(0x5034, 0x5037); W8(wLinkHealth) = A;
  CYC(0x5037, 0x5039); E = 0x05;
  CYC(0x5039, 0x503a); A = mem_rd(gb, DE);
  CYC(0x503a, 0x503b); push_effect(gb, 0x503b);
  switch (link_jump_table(gb)) {
    case 0x503f: goto substate0;
    case 0x505e: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x503f, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x5042);
  CYC(0x5042, 0x5044); E = 0x2d;
  CYC(0x5044, 0x5045); A = mem_rd(gb, DE);
  CYC(0x5045, 0x5046); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5046, 0x5049);
    linkUpdateKnockback_hook(gb);
    return;
  }
  CYC(0x5046, 0x5049);
  CYC(0x5049, 0x504a); H = D;
  CYC(0x504a, 0x504c); L = 0x05;
  CYC(0x504c, 0x504d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x504d, 0x504f); L = 0x06;
  CYC(0x504f, 0x5051); mem_wr(gb, HL, 0x04);
  CALL_C(0x5051, linkCancelAllItemUsage_hook, 0x4f49, 0x5054);
  CYC(0x5054, 0x5056); A = 0x01;
  CALL_C(0x5056, specialObjectSetAnimation_hook, 0x2b0a, 0x5059);
  CYC(0x5059, 0x505b); A = 0x64;
  CYC(0x505b, 0x505e);
  playSound_b00_hook(gb);
  return;

substate1:
  CALL_C(0x505e, resetLinkInvincibility_hook, 0x2ba9, 0x5061);
  CALL_C(0x5061, specialObjectAnimate_hook, 0x2aef, 0x5064);
  CYC(0x5064, 0x5065); H = D;
  CYC(0x5065, 0x5067); L = 0x21;
  CYC(0x5067, 0x5068); A = mem_rd(gb, HL);
  CYC(0x5068, 0x5069); alu_add(gb, A);
  if (!(F & FZ)) { CYCT(0x5069, 0x506b); goto trigger_game_over; }
  CYC(0x5069, 0x506b);
  if (!(F & FC)) { CYCT(0x506b, 0x506c); ret_effect(gb); return; }
  CYC(0x506b, 0x506c);
  CYC(0x506c, 0x506e); L = 0x06;
  CYC(0x506e, 0x506f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x506f, 0x5070); ret_effect(gb); return; }
  CYC(0x506f, 0x5070);
  CYC(0x5070, 0x5072); A = 0x02;
  CYC(0x5072, 0x5075);
  specialObjectSetAnimation_hook(gb);
  return;

trigger_game_over:
  CYC(0x5075, 0x5077); A = 0xff;
  CYC(0x5077, 0x507a); W8(wGameOverScreenTrigger) = A;
  CYC(0x507a, 0x507b); ret_effect(gb);
}

static void warp_transition3_enter_from_middle_bottom(GB *gb, uint16_t sp0_) {
  CYC(0x4b21, 0x4b23); A = 0x01;
  CYC(0x4b23, 0x4b26); W8(wMenuDisabled) = A;
  CYC(0x4b26, 0x4b28); L = 0x06;
  CYC(0x4b28, 0x4b2a); mem_wr(gb, HL, 0x1c);
  CYC(0x4b2a, 0x4b2d); A = W8(wWarpTransition);
  CYC(0x4b2d, 0x4b2f); alu_and(gb, 0x40);
  CYC(0x4b2f, 0x4b31); A = alu_swap(gb, A);
  CYC(0x4b31, 0x4b32); B = A;
  CYC(0x4b32, 0x4b35); A = W8(wActiveGroup);
  CYC(0x4b35, 0x4b37); alu_and(gb, 0x04);
  CYC(0x4b37, 0x4b38); alu_rrca(gb);
  CYC(0x4b38, 0x4b39); alu_or(gb, B);
  CYC(0x4b39, 0x4b3c); SET_BC(0x4b60);
  CALL_C(0x4b3c, addAToBc_hook, 0x006d, 0x4b3f);
  CYC(0x4b3f, 0x4b41); L = 0x0b;
  CYC(0x4b41, 0x4b42); A = mem_rd(gb, BC);
  CYC(0x4b42, 0x4b43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4b43, 0x4b44); SET_BC(BC + 1);
  CYC(0x4b44, 0x4b45); L = alu_inc8(gb, L);
  CYC(0x4b45, 0x4b46); A = mem_rd(gb, BC);
  CYC(0x4b46, 0x4b47); mem_wr(gb, HL, A);
  CYC(0x4b47, 0x4b48); ret_effect(gb);
}

void warpTransition3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4aa5, 0x4aa7); E = 0x05;
  CYC(0x4aa7, 0x4aa8); A = mem_rd(gb, DE);
  CYC(0x4aa8, 0x4aa9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4aa9, 0x4aab); goto each_frame; }
  CYC(0x4aa9, 0x4aab);
  CYC(0x4aab, 0x4aac); H = D;
  CYC(0x4aac, 0x4aad); L = E;
  CYC(0x4aad, 0x4aae); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4aae, 0x4ab0); L = 0x06;
  CYC(0x4ab0, 0x4ab2); mem_wr(gb, HL, 0x10);
  CYC(0x4ab2, 0x4ab5); A = W8(wWarpTransition);
  CYC(0x4ab5, 0x4ab7); alu_and(gb, 0x40);
  CYC(0x4ab7, 0x4ab9); A = alu_swap(gb, A);
  CYC(0x4ab9, 0x4aba); alu_rrca(gb);
  CYC(0x4aba, 0x4abd); SET_BC(0x4ad8);
  CALL_C(0x4abd, addAToBc_hook, 0x006d, 0x4ac0);
  CYC(0x4ac0, 0x4ac2); L = 0x08;
  CYC(0x4ac2, 0x4ac3); A = mem_rd(gb, BC);
  CYC(0x4ac3, 0x4ac4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ac4, 0x4ac5); SET_BC(BC + 1);
  CYC(0x4ac5, 0x4ac6); A = mem_rd(gb, BC);
  CYC(0x4ac6, 0x4ac7); mem_wr(gb, HL, A);
  CALL_C(0x4ac7, updateLinkSpeed_standard_hook, 0x5ce6, 0x4aca);
  CALL_C(0x4aca, animateLinkStanding_hook, 0x5cb5, 0x4acd);
  CYC(0x4acd, 0x4ad0); A = W8(wWarpTransition);
  CYC(0x4ad0, 0x4ad1); alu_rlca(gb);
  if (F & FC) { CYCT(0x4ad1, 0x4ad3); goto destination_init; }
  CYC(0x4ad1, 0x4ad3);
  CYC(0x4ad3, 0x4ad5); A = 0x6e;
  CYC(0x4ad5, 0x4ad8);
  playSound_b00_hook(gb);
  return;

each_frame:
  CYC(0x4adc, 0x4adf); A = W8(wScrollMode);
  CYC(0x4adf, 0x4ae1); alu_and(gb, 0x0a);
  if (!(F & FZ)) { CYCT(0x4ae1, 0x4ae2); ret_effect(gb); return; }
  CYC(0x4ae1, 0x4ae2);
  CYC(0x4ae2, 0x4ae4); A = 0x00;
  CYC(0x4ae4, 0x4ae7); W8(wScrollMode) = A;
  CALL_C(0x4ae7, specialObjectAnimate_hook, 0x2aef, 0x4aea);
  CALL_C(0x4aea, itemDecCounter1_hook, 0x23d6, 0x4aed);
  if (!(F & FZ)) {
    CYCT(0x4aed, 0x4af0);
    specialObjectUpdatePosition_hook(gb);
    return;
  }
  CYC(0x4aed, 0x4af0);
  CYC(0x4af0, 0x4af2); A = 0x01;
  CYC(0x4af2, 0x4af5); W8(wScrollMode) = A;
  CYC(0x4af5, 0x4af6); alu_xor(gb, A);
  CYC(0x4af6, 0x4af9); W8(wMenuDisabled) = A;
  CYC(0x4af9, 0x4afc); A = W8(wWarpTransition);
  CYC(0x4afc, 0x4afe); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x4afe, 0x4b01);
    warpUpdateRespawnPoint_hook(gb);
    return;
  }
  CYC(0x4afe, 0x4b01);
  CYC(0x4b01, 0x4b03); A = alu_swap(gb, A);
  CYC(0x4b03, 0x4b05); alu_and(gb, 0x03);
  CYC(0x4b05, 0x4b08); W8(wWarpTransition2) = A;
  CYC(0x4b08, 0x4b09); ret_effect(gb);
  return;

destination_init:
  CYC(0x4b09, 0x4b0a); H = D;
  CYC(0x4b0a, 0x4b0d); A = W8(wWarpDestPos);
  CYC(0x4b0d, 0x4b0f); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(0x4b0f, 0x4b11);
    warp_transition3_enter_from_middle_bottom(gb, sp0_);
    return;
  }
  CYC(0x4b0f, 0x4b11);
  CYC(0x4b11, 0x4b13); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(0x4b13, 0x4b15);
    goto enter_from_bottom;
  }
  CYC(0x4b13, 0x4b15);
  CYC(0x4b15, 0x4b17); L = 0x0b;
  CALL_C(0x4b17, setShortPosition_hook, 0x20b8, 0x4b1a);
  CYC(0x4b1a, 0x4b1c); L = 0x06;
  CYC(0x4b1c, 0x4b1e); mem_wr(gb, HL, 0x1c);
  CYC(0x4b1e, 0x4b21);
  initLinkStateAndAnimateStanding(gb);
  return;

enter_from_bottom:
  CYC(0x4b48, 0x4b4b); push_effect(gb, 0x4b4b);
  warp_transition3_enter_from_middle_bottom(gb, sp0_);
  CYC(0x4b4b, 0x4b4e); A = W8(wWarpDestPos);
  CYC(0x4b4e, 0x4b50); A = alu_swap(gb, A);
  CYC(0x4b50, 0x4b52); alu_and(gb, 0xf0);
  CYC(0x4b52, 0x4b53); B = A;
  CYC(0x4b53, 0x4b56); A = W8(wActiveGroup);
  CYC(0x4b56, 0x4b58); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(0x4b58, 0x4b5a);
  } else {
    CYC(0x4b58, 0x4b5a);
    CYC(0x4b5a, 0x4b5b); alu_rlca(gb);
  }
  CYC(0x4b5b, 0x4b5c); alu_or(gb, B);
  CYC(0x4b5c, 0x4b5e); L = 0x0d;
  CYC(0x4b5e, 0x4b5f); mem_wr(gb, HL, A);
  CYC(0x4b5f, 0x4b60); ret_effect(gb);
}

static void warp_transition6_flicker_visibility_and_dec_counter1(GB *gb,
                                                                  uint16_t sp0_) {
  CYC(0x4d9d, 0x4d9f); B = 0x03;
  CALL_C(0x4d9f, objectFlickerVisibility_hook, 0x24e6, 0x4da2);
  CYC(0x4da2, 0x4da5);
  itemDecCounter1_hook(gb);
}

static void warp_transition6_create_destination_timewarp_animation(GB *gb,
                                                                    uint16_t sp0_) {
  CALL_C(0x4da5, getFreeInteractionSlot_hook, 0x3aef, 0x4da8);
  if (!(F & FZ)) { CYCT(0x4da8, 0x4da9); ret_effect(gb); return; }
  CYC(0x4da8, 0x4da9);
  CYC(0x4da9, 0x4dab); mem_wr(gb, HL, 0xdd);
  CYC(0x4dab, 0x4dac); L = alu_inc8(gb, L);
  CYC(0x4dac, 0x4dad); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4dad, 0x4db0); A = W8(wcc50);
  CYC(0x4db0, 0x4db1); L = alu_inc8(gb, L);
  CYC(0x4db1, 0x4db2); mem_wr(gb, HL, A);
  CYC(0x4db2, 0x4db3); ret_effect(gb);
}

static void warp_transition6_center_link_on_doorway(GB *gb, uint16_t sp0_) {
  CALL_C(0x4db3, objectGetTileAtPosition_hook, 0x1444, 0x4db6);
  CYC(0x4db6, 0x4db7); push_effect(gb, HL);
  CYC(0x4db7, 0x4db8); A = E;
  CYC(0x4db8, 0x4dbb); SET_HL(0x4de7);
  CALL_C(0x4dbb, findByteAtHl_hook, 0x1e17, 0x4dbe);
  CYC(0x4dbe, 0x4dbf); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(0x4dbf, 0x4dc0); ret_effect(gb); return; }
  CYC(0x4dbf, 0x4dc0);
  CYC(0x4dc0, 0x4dc1); push_effect(gb, HL);
  CYC(0x4dc1, 0x4dc2); L = alu_dec8(gb, L);
  CYC(0x4dc2, 0x4dc3); E = mem_rd(gb, HL);
  CYC(0x4dc3, 0x4dc6); SET_HL(0x4de7);
  CALL_C(0x4dc6, findByteAtHl_hook, 0x1e17, 0x4dc9);
  CYC(0x4dc9, 0x4dca); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(0x4dca, 0x4dcc); goto check_right_tile; }
  CYC(0x4dca, 0x4dcc);
  CYC(0x4dcc, 0x4dce); E = 0x0d;
  CYC(0x4dce, 0x4dcf); A = mem_rd(gb, DE);
  CYC(0x4dcf, 0x4dd1); alu_and(gb, 0xf0);
  CYC(0x4dd1, 0x4dd2); mem_wr(gb, DE, A);
  CYC(0x4dd2, 0x4dd3); ret_effect(gb);
  return;

check_right_tile:
  CYC(0x4dd3, 0x4dd4); L = alu_inc8(gb, L);
  CYC(0x4dd4, 0x4dd5); E = mem_rd(gb, HL);
  CYC(0x4dd5, 0x4dd8); SET_HL(0x4de7);
  CALL_C(0x4dd8, findByteAtHl_hook, 0x1e17, 0x4ddb);
  if (!(F & FC)) { CYCT(0x4ddb, 0x4ddc); ret_effect(gb); return; }
  CYC(0x4ddb, 0x4ddc);
  CYC(0x4ddc, 0x4dde); E = 0x0d;
  CYC(0x4dde, 0x4ddf); A = mem_rd(gb, DE);
  CYC(0x4ddf, 0x4de1); alu_add(gb, 0x08);
  CYC(0x4de1, 0x4de2); mem_wr(gb, DE, A);
  CYC(0x4de2, 0x4de5); SET_HL(wEnteredWarpPosition);
  CYC(0x4de5, 0x4de6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4de6, 0x4de7); ret_effect(gb);
}

void warpTransition6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d89, 0x4d8b); E = 0x05;
  CYC(0x4d8b, 0x4d8c); A = mem_rd(gb, DE);
  CYC(0x4d8c, 0x4d8d); push_effect(gb, 0x4d8d);
  switch (link_jump_table(gb)) {
    case 0x4def: goto substate0;
    case 0x4e07: goto substate1;
    case 0x4e3c: goto substate2;
    case 0x4e4d: goto substate3;
    case 0x4ea3: goto substate4;
    case 0x4eab: goto substate5;
    case 0x4eb7: goto substate6;
    case 0x4ec3: goto substate7;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x4def, itemIncSubstate_hook, 0x23ef, 0x4df2);
  CYC(0x4df2, 0x4df4); L = 0x06;
  CYC(0x4df4, 0x4df6); mem_wr(gb, HL, 0x1e);
  CYC(0x4df6, 0x4df8); L = 0x08;
  CYC(0x4df8, 0x4dfa); mem_wr(gb, HL, 0x02);
  CYC(0x4dfa, 0x4dfb); A = D;
  CYC(0x4dfb, 0x4dfe); W8(wLinkCanPassNpcs) = A;
  CYC(0x4dfe, 0x4e01); W8(wMenuDisabled) = A;
  CYC(0x4e01, 0x4e04); push_effect(gb, 0x4e04);
  warp_transition6_center_link_on_doorway(gb, sp0_);
  CYC(0x4e04, 0x4e07);
  objectSetInvisible_hook(gb);
  return;

substate1:
  CYC(0x4e07, 0x4e0a); A = W8(wPaletteThread_mode);
  CYC(0x4e0a, 0x4e0b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4e0b, 0x4e0c); ret_effect(gb); return; }
  CYC(0x4e0b, 0x4e0c);
  CALL_C(0x4e0c, itemDecCounter1_hook, 0x23d6, 0x4e0f);
  if (!(F & FZ)) { CYCT(0x4e0f, 0x4e10); ret_effect(gb); return; }
  CYC(0x4e0f, 0x4e10);
  CYC(0x4e10, 0x4e12); mem_wr(gb, HL, 0x10);
  CYC(0x4e12, 0x4e15); push_effect(gb, 0x4e15);
  warp_transition6_create_destination_timewarp_animation(gb, sp0_);
  CYC(0x4e15, 0x4e18); A = W8(wSentBackByStrangeForce);
  CYC(0x4e18, 0x4e19); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4e19, 0x4e1b); goto warp_failed; }
  CYC(0x4e19, 0x4e1b);
  CYC(0x4e1b, 0x4e1e); SET_HL(0x62e9);
  CYC(0x4e1e, 0x4e20); E = 0x01;
  CALL_C(0x4e20, interBankCall_hook, 0x008a, 0x4e23);
  CYC(0x4e23, 0x4e25); C = alu_srl(gb, C);
  if (F & FC) { CYCT(0x4e25, 0x4e27); goto warp_failed; }
  CYC(0x4e25, 0x4e27);
  CYC(0x4e27, 0x4e2a); SET_HL(0x62c4);
  CYC(0x4e2a, 0x4e2c); E = 0x01;
  CALL_C(0x4e2c, interBankCall_hook, 0x008a, 0x4e2f);
  CYC(0x4e2f, 0x4e31); C = alu_srl(gb, C);
  if (F & FC) { CYCT(0x4e31, 0x4e33); goto warp_failed; }
  CYC(0x4e31, 0x4e33);
  CYC(0x4e33, 0x4e36);
  itemIncSubstate_hook(gb);
  return;

warp_failed:
  CYC(0x4e36, 0x4e38); E = 0x05;
  CYC(0x4e38, 0x4e3a); A = 0x04;
  CYC(0x4e3a, 0x4e3b); mem_wr(gb, DE, A);
  CYC(0x4e3b, 0x4e3c); ret_effect(gb);
  return;

substate2:
  CALL_C(0x4e3c, itemDecCounter1_hook, 0x23d6, 0x4e3f);
  if (!(F & FZ)) { CYCT(0x4e3f, 0x4e40); ret_effect(gb); return; }
  CYC(0x4e3f, 0x4e40);
  CYC(0x4e40, 0x4e42); mem_wr(gb, HL, 0x1e);

make_link_visible_and_play_sound:
  CYC(0x4e42, 0x4e44); A = 0xd4;
  CALL_C(0x4e44, playSound_b00_hook, 0x0c98, 0x4e47);
  CALL_C(0x4e47, objectSetVisiblec0_hook, 0x1e33, 0x4e4a);
  CYC(0x4e4a, 0x4e4d);
  itemIncSubstate_hook(gb);
  return;

substate3:
  CYC(0x4e4d, 0x4e50); push_effect(gb, 0x4e50);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(0x4e50, 0x4e51); ret_effect(gb); return; }
  CYC(0x4e50, 0x4e51);
  CYC(0x4e51, 0x4e54); A = W8(wLinkTimeWarpTile);
  CYC(0x4e54, 0x4e55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4e55, 0x4e57); goto maybe_show_strange_force_text; }
  CYC(0x4e55, 0x4e57);
  CYC(0x4e57, 0x4e5a); SET_HL(wPortalGroup);
  CYC(0x4e5a, 0x4e5d); A = W8(wActiveGroup);
  CYC(0x4e5d, 0x4e5e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e5e, 0x4e61); A = W8(wActiveRoom);
  CYC(0x4e61, 0x4e62); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e62, 0x4e65); A = W8(wWarpDestPos);
  CYC(0x4e65, 0x4e66); mem_wr(gb, HL, A);
  CYC(0x4e66, 0x4e67); C = A;
  CALL_C(0x4e67, getFreeInteractionSlot_hook, 0x3aef, 0x4e6a);
  if (!(F & FZ)) { CYCT(0x4e6a, 0x4e6c); goto maybe_show_strange_force_text; }
  CYC(0x4e6a, 0x4e6c);
  CYC(0x4e6c, 0x4e6e); mem_wr(gb, HL, 0xde);
  CYC(0x4e6e, 0x4e70); L = 0x4b;
  CALL_C(0x4e70, setShortPosition_paramC_hook, 0x20b9, 0x4e73);

maybe_show_strange_force_text:
  CYC(0x4e73, 0x4e76); A = W8(wSentBackByStrangeForce);
  CYC(0x4e76, 0x4e77); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4e77, 0x4e79);
  } else {
    CYC(0x4e77, 0x4e79);
    CYC(0x4e79, 0x4e7c); SET_BC(0x5112);
    CALL_C(0x4e7c, showText_hook, 0x1872, 0x4e7f);
  }
  CYC(0x4e7f, 0x4e80); alu_xor(gb, A);
  CYC(0x4e80, 0x4e83); W8(wLinkTimeWarpTile) = A;
  CYC(0x4e83, 0x4e86); W8(wWarpTransition) = A;
  CYC(0x4e86, 0x4e89); W8(wLinkCanPassNpcs) = A;
  CYC(0x4e89, 0x4e8c); W8(wMenuDisabled) = A;
  CYC(0x4e8c, 0x4e8f); W8(wSentBackByStrangeForce) = A;
  CYC(0x4e8f, 0x4e92); W8(wcddf) = A;
  CYC(0x4e92, 0x4e95); W8(wcde0) = A;
  CYC(0x4e95, 0x4e97); E = 0x2b;
  CYC(0x4e97, 0x4e99); A = 0x88;
  CYC(0x4e99, 0x4e9a); mem_wr(gb, DE, A);
  CALL_C(0x4e9a, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4e9d);
  CALL_C(0x4e9d, objectSetVisiblec2_hook, 0x1e45, 0x4ea0);
  CYC(0x4ea0, 0x4ea3);
  initLinkStateAndAnimateStanding(gb);
  return;

substate4:
  CALL_C(0x4ea3, itemDecCounter1_hook, 0x23d6, 0x4ea6);
  if (!(F & FZ)) { CYCT(0x4ea6, 0x4ea7); ret_effect(gb); return; }
  CYC(0x4ea6, 0x4ea7);
  CYC(0x4ea7, 0x4ea9); mem_wr(gb, HL, 0x78);
  CYCT(0x4ea9, 0x4eab);
  goto make_link_visible_and_play_sound;

substate5:
  CYC(0x4eab, 0x4eae); push_effect(gb, 0x4eae);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(0x4eae, 0x4eaf); ret_effect(gb); return; }
  CYC(0x4eae, 0x4eaf);
  CYC(0x4eaf, 0x4eb1); mem_wr(gb, HL, 0x10);
  CYC(0x4eb1, 0x4eb4); push_effect(gb, 0x4eb4);
  warp_transition6_create_destination_timewarp_animation(gb, sp0_);
  CYC(0x4eb4, 0x4eb7);
  itemIncSubstate_hook(gb);
  return;

substate6:
  CYC(0x4eb7, 0x4eba); push_effect(gb, 0x4eba);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(0x4eba, 0x4ebb); ret_effect(gb); return; }
  CYC(0x4eba, 0x4ebb);
  CYC(0x4ebb, 0x4ebd); mem_wr(gb, HL, 0x14);
  CALL_C(0x4ebd, objectSetInvisible_hook, 0x1e7b, 0x4ec0);
  CYC(0x4ec0, 0x4ec3);
  itemIncSubstate_hook(gb);
  return;

substate7:
  CALL_C(0x4ec3, itemDecCounter1_hook, 0x23d6, 0x4ec6);
  if (!(F & FZ)) { CYCT(0x4ec6, 0x4ec7); ret_effect(gb); return; }
  CYC(0x4ec6, 0x4ec7);
  CALL_C(0x4ec7, objectGetTileAtPosition_hook, 0x1444, 0x4eca);
  CYC(0x4eca, 0x4ecb); C = L;
  CYC(0x4ecb, 0x4ece); SET_HL(wWarpDestGroup);
  CYC(0x4ece, 0x4ed1); A = W8(wActiveGroup);
  CYC(0x4ed1, 0x4ed3); alu_xor(gb, 0x01);
  CYC(0x4ed3, 0x4ed5); alu_or(gb, 0x80);
  CYC(0x4ed5, 0x4ed6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4ed6, 0x4ed9); A = W8(wActiveRoom);
  CYC(0x4ed9, 0x4eda); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4eda, 0x4edc); A = 0x06;
  CYC(0x4edc, 0x4edd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4edd, 0x4ede); A = C;
  CYC(0x4ede, 0x4edf); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4edf, 0x4ee0); A = alu_inc8(gb, A);
  CYC(0x4ee0, 0x4ee3); W8(wLinkTimeWarpTile) = A;
  CYC(0x4ee3, 0x4ee6); W8(wcddf) = A;
  CYC(0x4ee6, 0x4ee8); A = 0x03;
  CYC(0x4ee8, 0x4ee9); mem_wr(gb, HL, A);
  CYC(0x4ee9, 0x4eea); alu_xor(gb, A);
  CYC(0x4eea, 0x4eed); W8(wScrollMode) = A;
  CYC(0x4eed, 0x4ef0); SET_HL(wSentBackByStrangeForce);
  CYC(0x4ef0, 0x4ef1); A = mem_rd(gb, HL);
  CYC(0x4ef1, 0x4ef2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4ef2, 0x4ef4);
  } else {
    CYC(0x4ef2, 0x4ef4);
    CYC(0x4ef4, 0x4ef5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(0x4ef5, 0x4ef8); A = W8(wLinkStateParameter);
  CYC(0x4ef8, 0x4efa); alu_bit(gb, 4, A);
  if (F & FZ) {
    CYC(0x4efa, 0x4efc);
    CALL_C(0x4efc, getThisRoomFlags_hook, 0x197d, 0x4eff);
    CYC(0x4eff, 0x4f01); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  } else {
    CYCT(0x4efa, 0x4efc);
  }
  CYC(0x4f01, 0x4f03); A = 0xd4;
  CALL_C(0x4f03, playSound_b00_hook, 0x0c98, 0x4f06);
  CYC(0x4f06, 0x4f09); SET_DE(w1Link);
  CYC(0x4f09, 0x4f0c);
  objectDelete_de_hook(gb);
}

void linkState01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54dd, 0x54df); A = 0x80;
  CYC(0x54df, 0x54e2); W8(wForceLinkPushAnimation) = A;
  CYC(0x54e2, 0x54e5); A = W8(wPaletteThread_mode);
  CYC(0x54e5, 0x54e6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x54e6, 0x54e7); ret_effect(gb); return; }
  CYC(0x54e6, 0x54e7);
  CYC(0x54e7, 0x54ea); A = W8(wScrollMode);
  CYC(0x54ea, 0x54ec); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x54ec, 0x54ed); ret_effect(gb); return; }
  CYC(0x54ec, 0x54ed);
  CALL_C(0x54ed, updateLinkDamageTaken_hook, 0x4268, 0x54f0);
  CYC(0x54f0, 0x54f3); A = W8(wLinkDeathTrigger);
  CYC(0x54f3, 0x54f4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x54f4, 0x54f7);
    setLinkStateToDead_hook(gb);
    return;
  }
  CYC(0x54f4, 0x54f7);
  CALL_C(0x54f7, checkLinkForceState_hook, 0x54c0, 0x54fa);
  CALL_C(0x54fa, retIfTextIsActive_hook, 0x1859, 0x54fd);
  CYC(0x54fd, 0x5500); A = W8(wDisabledObjects);
  CYC(0x5500, 0x5502); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(0x5502, 0x5503); ret_effect(gb); return; }
  CYC(0x5502, 0x5503);
  CALL_C(0x5503, decPegasusSeedCounter_hook, 0x2bbd, 0x5506);
  CYC(0x5506, 0x5509); A = W8(w1Companion_id);
  CYC(0x5509, 0x550b); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(0x550b, 0x550d); goto interactions_done; }
  CYC(0x550b, 0x550d);
  CYC(0x550d, 0x550f); alu_cp(gb, 0x13);
  if (F & FZ) { CYCT(0x550f, 0x5511); goto interactions_done; }
  CYC(0x550f, 0x5511);
  CYC(0x5511, 0x5514); A = W8(wLinkObjectIndex);
  CYC(0x5514, 0x5515); alu_rrca(gb);
  if (F & FC) { CYCT(0x5515, 0x5516); ret_effect(gb); return; }
  CYC(0x5515, 0x5516);
  CYC(0x5516, 0x5519); A = W8(wLinkPlayingInstrument);
  CYC(0x5519, 0x551a); B = A;
  CYC(0x551a, 0x551d); A = W8(wLinkInAir);
  CYC(0x551d, 0x551e); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(0x551e, 0x5520); goto interactions_done; }
  CYC(0x551e, 0x5520);
  CYC(0x5520, 0x5522); E = 0x2d;
  CYC(0x5522, 0x5523); A = mem_rd(gb, DE);
  CYC(0x5523, 0x5524); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5524, 0x5526); goto interactions_done; }
  CYC(0x5524, 0x5526);
  CALL_C(0x5526, linkInteractWithAButtonSensitiveObjects_hook, 0x1b5d, 0x5529);
  if (F & FC) { CYCT(0x5529, 0x552a); ret_effect(gb); return; }
  CYC(0x5529, 0x552a);
  CALL_C(0x552a, interactWithTileBeforeLink_b00_hook, 0x1280, 0x552d);
  if (F & FC) { CYCT(0x552d, 0x552e); ret_effect(gb); return; }
  CYC(0x552d, 0x552e);

interactions_done:
  CYC(0x552e, 0x552f); alu_xor(gb, A);
  CYC(0x552f, 0x5532); W8(wForceLinkPushAnimation) = A;
  CYC(0x5532, 0x5535); W8(wLinkPlayingInstrument) = A;
  CYC(0x5535, 0x5538); A = W8(wTilesetFlags);
  CYC(0x5538, 0x553a); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x553a, 0x553d);
    linkState01_sidescroll(gb);
    return;
  }
  CYC(0x553a, 0x553d);
  CALL_C(0x553d, linkApplyTileTypes_hook, 0x42b7, 0x5540);
  CALL_C(0x5540, checkAndUpdateLinkOnChest_hook, 0x1255, 0x5543);
  CALL_C(0x5543, checkUseItems_b00_hook, 0x2c18, 0x5546);
  CYC(0x5546, 0x5549); A = W8(wLinkPlayingInstrument);
  CYC(0x5549, 0x554a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x554a, 0x554b); ret_effect(gb); return; }
  CYC(0x554a, 0x554b);
  CALL_C(0x554b, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x554e);
  CALL_C(0x554e, linkUpdateKnockback_hook, 0x5d5b, 0x5551);
  CYC(0x5551, 0x5554); A = W8(wLinkSwimmingState);
  CYC(0x5554, 0x5556); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x5556, 0x5558); goto update_in_air; }
  CYC(0x5556, 0x5558);
  CYC(0x5558, 0x555b); A = W8(wMagnetGloveState);
  CYC(0x555b, 0x555d); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x555d, 0x555f); goto update_in_air; }
  CYC(0x555d, 0x555f);
  CYC(0x555f, 0x5562); A = W8(wLinkInAir);
  CYC(0x5562, 0x5563); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5563, 0x5565); goto update_in_air; }
  CYC(0x5563, 0x5565);
  CYC(0x5565, 0x5568); A = W8(wLinkGrabState);
  CYC(0x5568, 0x5569); C = A;
  CYC(0x5569, 0x556c); A = W8(wLinkImmobilized);
  CYC(0x556c, 0x556d); alu_or(gb, C);
  if (!(F & FZ)) { CYCT(0x556d, 0x556f); goto update_in_air; }
  CYC(0x556d, 0x556f);
  CALL_C(0x556f, checkLinkPushingAgainstBed_hook, 0x5faf, 0x5572);
  CALL_C(0x5572, checkLinkJumpingOffCliff_hook, 0x6034, 0x5575);

update_in_air:
  CALL_C(0x5575, linkUpdateInAir_hook, 0x5af3, 0x5578);
  CYC(0x5578, 0x557b); A = W8(wLinkInAir);
  CYC(0x557b, 0x557c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x557c, 0x557e); goto not_in_air; }
  CYC(0x557c, 0x557e);
  CYC(0x557e, 0x5580); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x5580, 0x5582);
  } else {
    CYC(0x5580, 0x5582);
    CYC(0x5582, 0x5584); E = 0x15;
    CYC(0x5584, 0x5585); A = mem_rd(gb, DE);
    CYC(0x5585, 0x5587); alu_bit(gb, 7, A);
    if (F & FZ) CALL_C_CC(0x5587, linkUpdateVelocity_hook, 0x58f0, 0x558a);
    else CYC(0x5587, 0x558a);
  }
  CYC(0x558a, 0x558d); SET_HL(wcc95);
  CYC(0x558d, 0x558f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CALL_C(0x558f, specialObjectSetAngleRelativeToVar38, 0x5ff3, 0x5592);
  CALL_C(0x5592, specialObjectUpdatePosition_hook, 0x5d97, 0x5595);
  CYC(0x5595, 0x5598);
  specialObjectAnimate_hook(gb);
  return;

not_in_air:
  CYC(0x5598, 0x559b); A = W8(wMagnetGloveState);
  CYC(0x559b, 0x559d); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x559d, 0x55a0);
    animateLinkStanding_hook(gb);
    return;
  }
  CYC(0x559d, 0x55a0);
  CYC(0x55a0, 0x55a2); E = 0x2d;
  CYC(0x55a2, 0x55a3); A = mem_rd(gb, DE);
  CYC(0x55a3, 0x55a4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55a4, 0x55a7);
    func_5631_hook(gb);
    return;
  }
  CYC(0x55a4, 0x55a7);
  CYC(0x55a7, 0x55a8); H = D;
  CYC(0x55a8, 0x55aa); L = 0x24;
  CYC(0x55aa, 0x55ac); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x55ac, 0x55af); A = W8(wLinkSwimmingState);
  CYC(0x55af, 0x55b0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55b0, 0x55b3);
    linkUpdateSwimming_hook(gb);
    return;
  }
  CYC(0x55b0, 0x55b3);
  CALL_C(0x55b3, objectSetVisiblec1_hook, 0x1e3c, 0x55b6);
  CYC(0x55b6, 0x55b9); A = W8(wLinkObjectIndex);
  CYC(0x55b9, 0x55ba); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x55ba, 0x55bc); goto check_underwater_transition; }
  CYC(0x55ba, 0x55bc);
  CYC(0x55bc, 0x55bf); A = W8(w1Companion_id);
  CYC(0x55bf, 0x55c1); alu_or(gb, 0x13);
  if (F & FZ) { CYCT(0x55c1, 0x55c3); goto update_direction_if_not_using_item; }
  CYC(0x55c1, 0x55c3);
  CYCT(0x55c3, 0x55c5);
  goto update_direction;

check_underwater_transition:
  CALL_C(0x55c5, checkForUnderwaterTransition_hook, 0x516c, 0x55c8);
  CYC(0x55c8, 0x55cb); SET_HL(0x462d);
  CYC(0x55cb, 0x55cd); E = 0x06;
  CALL_C(0x55cd, interBankCall_hook, 0x008a, 0x55d0);
  CYC(0x55d0, 0x55d1); A = B;
  CYC(0x55d1, 0x55d2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55d2, 0x55d5);
    setLinkIDOverride_hook(gb);
    return;
  }
  CYC(0x55d2, 0x55d5);
  CYC(0x55d5, 0x55d6); H = D;
  CYC(0x55d6, 0x55d8); L = 0x2f;
  CYC(0x55d8, 0x55da); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x55da, 0x55dc); goto ordinary_movement; }
  CYC(0x55da, 0x55dc);
  CALL_C(0x55dc, linkUpdateVelocity_mermaidSuit_hook, 0x58f7, 0x55df);
  CYCT(0x55df, 0x55e1);
  goto velocity_updated;

ordinary_movement:
  CYC(0x55e1, 0x55e4); A = W8(wLinkGrabState);
  CYC(0x55e4, 0x55e6); alu_and(gb, 0x0f);
  CYC(0x55e6, 0x55e7); A = alu_dec8(gb, A);
  CYC(0x55e7, 0x55e9); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x55e9, 0x55eb); goto normal_movement; }
  CYC(0x55e9, 0x55eb);
  CYC(0x55eb, 0x55ee); SET_HL(wIsTileSlippery);
  CYC(0x55ee, 0x55f0); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x55f0, 0x55f2); goto normal_movement; }
  CYC(0x55f0, 0x55f2);
  CYC(0x55f2, 0x55f4); C = 0x88;
  CALL_C(0x55f4, updateLinkSpeed_withParam_hook, 0x5ce8, 0x55f7);
  CALL_C(0x55f7, linkUpdateVelocity_hook, 0x58f0, 0x55fa);

velocity_updated:
  CYC(0x55fa, 0x55fd); A = W8(wLinkAngle);
  CYC(0x55fd, 0x55fe); alu_rlca(gb);
  CYC(0x55fe, 0x5600); C = 0x02;
  if (F & FC) { CYCT(0x5600, 0x5602); goto update_movement; }
  CYC(0x5600, 0x5602);
  CYCT(0x5602, 0x5604);
  goto walking;

normal_movement:
  CYC(0x5604, 0x5607); A = W8(wcc95);
  CYC(0x5607, 0x5608); B = A;
  CYC(0x5608, 0x560a); E = 0x09;
  CYC(0x560a, 0x560d); A = W8(wLinkAngle);
  CYC(0x560d, 0x560e); mem_wr(gb, DE, A);
  CYC(0x560e, 0x560f); alu_or(gb, B);
  CYC(0x560f, 0x5610); alu_rlca(gb);
  CYC(0x5610, 0x5612); C = 0x00;
  if (F & FC) { CYCT(0x5612, 0x5614); goto update_movement; }
  CYC(0x5612, 0x5614);
  CYC(0x5614, 0x5616); C = 0x01;
  CYC(0x5616, 0x5619); A = W8(wLinkImmobilized);
  CYC(0x5619, 0x561a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x561a, 0x561c); goto update_movement; }
  CYC(0x561a, 0x561c);
  CALL_C(0x561c, updateLinkSpeed_standard_hook, 0x5ce6, 0x561f);

walking:
  CYC(0x561f, 0x5621); C = 0x07;

update_movement:
  CALL_C(0x5621, linkUpdateMovement_hook, 0x5ad0, 0x5624);

update_direction_if_not_using_item:
  CYC(0x5624, 0x5627); A = W8(wLinkTurningDisabled);
  CYC(0x5627, 0x5628); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5628, 0x5629); ret_effect(gb); return; }
  CYC(0x5628, 0x5629);

update_direction:
  CYC(0x5629, 0x562c);
  updateLinkDirectionFromAngle_hook(gb);
}

void specialObjectCode_link_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49b6, 0x49b8); E = 0x04;
  CYC(0x49b8, 0x49b9); A = mem_rd(gb, DE);
  CYC(0x49b9, 0x49ba); push_effect(gb, 0x49ba);
  switch (link_jump_table(gb)) {
    case 0x49e4: linkState00_hook(gb); return;
    case 0x54dd: linkState01_hook(gb); return;
    case 0x507b: linkState02_hook(gb); return;
    case 0x5033: linkState03_hook(gb); return;
    case 0x4ff6: linkState04_hook(gb); return;
    case 0x52fb: linkState05_hook(gb); return;
    case 0x5377: linkState06_hook(gb); return;
    case 0x2cad: linkState07_hook(gb); return;
    case 0x4f0c: linkState08_hook(gb); return;
    case 0x53ea: linkState09_hook(gb); return;
    case 0x4a00: linkState0a_hook(gb); return;
    case 0x4fc7: linkState0b_hook(gb); return;
    case 0x51ec: linkState0c_hook(gb); return;
    case 0x529e: linkState0d_hook(gb); return;
    case 0x4f4f: linkState0e_hook(gb); return;
    case 0x4f80: linkState0f_hook(gb); return;
    case 0x5467: linkState11_hook(gb); return;
    case 0x60a5: linkState12(gb); return;
    case 0x5227: linkState13_hook(gb); return;
    case 0x527f: linkState14_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void linkState02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x507b, 0x507d); A = 0xff;
  CYC(0x507d, 0x5080); W8(wGameKeysPressed) = A;
  CYC(0x5080, 0x5082); A = 0x80;
  CYC(0x5082, 0x5085); W8(wForceLinkPushAnimation) = A;
  CYC(0x5085, 0x5087); E = 0x05;
  CYC(0x5087, 0x5088); A = mem_rd(gb, DE);
  CYC(0x5088, 0x5089); push_effect(gb, 0x5089);
  switch (link_jump_table(gb)) {
    case 0x5095: goto substate0;
    case 0x50cc: goto substate1;
    case 0x510f: goto substate2;
    case 0x514a: goto substate3;
    case 0x5155: goto substate4;
    case 0x5163: goto substate5;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x5095, linkCancelAllItemUsage_hook, 0x4f49, 0x5098);
  CYC(0x5098, 0x509b); A = W8(wLinkStateParameter);
  CYC(0x509b, 0x509c); push_effect(gb, 0x509c);
  switch (link_jump_table(gb)) {
    case 0x50b0: goto parameter_fall_down_hole;
    case 0x50b3: goto parameter_fall_without_centering;
    case 0x50f9: goto respawn;
    case 0x50b8: goto parameter_3;
    case 0x50a6: goto parameter_drown;
    default: hook_continue(gb, HL, sp0_); return;
  }

parameter_drown:
  CYC(0x50a6, 0x50a8); E = 0x05;
  CYC(0x50a8, 0x50aa); A = 0x05;
  CYC(0x50aa, 0x50ab); mem_wr(gb, DE, A);
  CYC(0x50ab, 0x50ad); A = 0x0a;
  CYC(0x50ad, 0x50b0);
  specialObjectSetAnimation_hook(gb);
  return;

parameter_fall_down_hole:
  CALL_C(0x50b0, objectCenterOnTile_hook, 0x20db, 0x50b3);

parameter_fall_without_centering:
  CALL_C(0x50b3, itemIncSubstate_hook, 0x23ef, 0x50b6);
  CYC(0x50b6, 0x50b8);
  goto start_falling;

parameter_3:
  CYC(0x50b8, 0x50ba); E = 0x05;
  CYC(0x50ba, 0x50bc); A = 0x04;
  CYC(0x50bc, 0x50bd); mem_wr(gb, DE, A);

start_falling:
  CYC(0x50bd, 0x50be); H = D;
  CYC(0x50be, 0x50c0); L = 0x24;
  CYC(0x50c0, 0x50c2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x50c2, 0x50c4); A = 0x0d;
  CALL_C(0x50c4, specialObjectSetAnimation_hook, 0x2b0a, 0x50c7);
  CYC(0x50c7, 0x50c9); A = 0x65;
  CYC(0x50c9, 0x50cc);
  playSound_b00_hook(gb);
  return;

substate1:
  CYC(0x50cc, 0x50cd); H = D;
  CYC(0x50cd, 0x50cf); L = 0x21;
  CYC(0x50cf, 0x50d1); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x50d1, 0x50d4);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x50d1, 0x50d4);
  CYC(0x50d4, 0x50d7); A = W8(wActiveTileType);
  CYC(0x50d7, 0x50d9); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x50d9, 0x50db); goto respawn; }
  CYC(0x50d9, 0x50db);
  CYC(0x50db, 0x50de); A = W8(wActiveGroup);
  CYC(0x50de, 0x50e0); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x50e0, 0x50e2); goto ordinary_hole_warp; }
  CYC(0x50e0, 0x50e2);
  CYC(0x50e2, 0x50e5); A = W8(wActiveRoom);
  CYC(0x50e5, 0x50e7); alu_cp(gb, 0x9f);
  if (!(F & FZ)) { CYCT(0x50e7, 0x50e9); goto ordinary_hole_warp; }
  CYC(0x50e7, 0x50e9);
  CYC(0x50e9, 0x50ec); SET_HL(0x7d92);
  CYC(0x50ec, 0x50ee); E = 0x01;
  CYC(0x50ee, 0x50f1);
  interBankCall_hook(gb);
  return;

ordinary_hole_warp:
  CYC(0x50f1, 0x50f4); SET_HL(0x4ad2);
  CYC(0x50f4, 0x50f6); E = 0x01;
  CYC(0x50f6, 0x50f9);
  interBankCall_hook(gb);
  return;

respawn:
  CALL_C(0x50f9, specialObjectSetCoordinatesToRespawnYX_hook, 0x2b8a, 0x50fc);
  CYC(0x50fc, 0x50fe); L = 0x05;
  CYC(0x50fe, 0x5100); A = 0x02;
  CYC(0x5100, 0x5101); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5101, 0x5102); mem_wr(gb, HL, A);
  CALL_C(0x5102, specialObjectTryToBreakTile_source05_hook, 0x49a8, 0x5105);
  CALL_C(0x5105, objectGetTileAtPosition_hook, 0x1444, 0x5108);
  CYC(0x5108, 0x5109); A = L;
  CYC(0x5109, 0x510c); W8(wEnteredWarpPosition) = A;
  CYC(0x510c, 0x510f);
  objectSetInvisible_hook(gb);
  return;

substate2:
  CYC(0x510f, 0x5110); H = D;
  CYC(0x5110, 0x5112); L = 0x06;
  CYC(0x5112, 0x5115); A = W8(wScrollMode);
  CYC(0x5115, 0x5117); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYC(0x5117, 0x5119);
    CYC(0x5119, 0x511b); mem_wr(gb, HL, 0x04);
    CYC(0x511b, 0x511c); ret_effect(gb);
    return;
  }
  CYCT(0x5117, 0x5119);
  CYC(0x511c, 0x511d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x511d, 0x511e); ret_effect(gb); return; }
  CYC(0x511d, 0x511e);
  CYC(0x511e, 0x511f); alu_xor(gb, A);
  CYC(0x511f, 0x5122); W8(wLinkInAir) = A;
  CYC(0x5122, 0x5125); W8(wLinkSwimmingState) = A;
  CYC(0x5125, 0x5127); A = 0x1c;
  CALL_C(0x5127, cpActiveRing_hook, 0x23b0, 0x512a);
  CYC(0x512a, 0x512c); A = 0xfc;
  if (!(F & FZ)) {
    CYCT(0x512c, 0x512e);
  } else {
    CYC(0x512c, 0x512e);
    CYC(0x512e, 0x5130); A = alu_sra(gb, A);
  }
  CALL_C(0x5130, itemIncSubstate_hook, 0x23ef, 0x5133);
  CYC(0x5133, 0x5135); L = 0x25;
  CYC(0x5135, 0x5136); mem_wr(gb, HL, A);
  CYC(0x5136, 0x5138); L = 0x2b;
  CYC(0x5138, 0x513a); mem_wr(gb, HL, 0x3c);
  CYC(0x513a, 0x513c); L = 0x06;
  CYC(0x513c, 0x513e); mem_wr(gb, HL, 0x10);
  CALL_C(0x513e, linkApplyDamage_b00_hook, 0x2ab7, 0x5141);
  CALL_C(0x5141, objectSetVisiblec1_hook, 0x1e3c, 0x5144);
  CALL_C(0x5144, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x5147);
  CYC(0x5147, 0x514a);
  animateLinkStanding_hook(gb);
  return;

substate3:
  CALL_C(0x514a, itemDecCounter1_hook, 0x23d6, 0x514d);
  if (!(F & FZ)) { CYCT(0x514d, 0x514e); ret_effect(gb); return; }
  CYC(0x514d, 0x514e);
  CYC(0x514e, 0x5150); L = 0x24;
  CYC(0x5150, 0x5152); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5152, 0x5155);
  initLinkStateAndAnimateStanding(gb);
  return;

substate4:
  CYC(0x5155, 0x5156); H = D;
  CYC(0x5156, 0x5158); L = 0x21;
  CYC(0x5158, 0x515a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x515a, 0x515d);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x515a, 0x515d);
  CALL_C(0x515d, objectSetInvisible_hook, 0x1e7b, 0x5160);
  CYC(0x5160, 0x5163);
  checkLinkForceState_hook(gb);
  return;

substate5:
  CYC(0x5163, 0x5165); E = 0x21;
  CYC(0x5165, 0x5166); A = mem_rd(gb, DE);
  CYC(0x5166, 0x5167); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x5167, 0x516a);
    specialObjectAnimate_hook(gb);
    return;
  }
  CYC(0x5167, 0x516a);
  CYC(0x516a, 0x516c);
  goto respawn;
}

void linkState0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x51ec, 0x51ee); E = 0x05;
  CYC(0x51ee, 0x51ef); A = mem_rd(gb, DE);
  CYC(0x51ef, 0x51f0); push_effect(gb, 0x51f0);
  switch (link_jump_table(gb)) {
    case 0x51f6: goto substate0;
    case 0x5226: goto done;
    case 0x520d: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x51f6, 0x51f8); A = 0x01;
  CYC(0x51f8, 0x51f9); mem_wr(gb, DE, A);
  CYC(0x51f9, 0x51fc); W8(wWarpsDisabled) = A;
  CYC(0x51fc, 0x51fd); alu_xor(gb, A);
  CYC(0x51fd, 0x51ff); E = 0x24;
  CYC(0x51ff, 0x5200); mem_wr(gb, DE, A);
  CYC(0x5200, 0x5202); A = 0x00;
  CYC(0x5202, 0x5205); W8(wScrollMode) = A;
  CALL_C(0x5205, linkCancelAllItemUsage_hook, 0x4f49, 0x5208);
  CYC(0x5208, 0x520a); A = 0x67;
  CYC(0x520a, 0x520d);
  playSound_b00_hook(gb);
  return;

substate2:
  CYC(0x520d, 0x520e); alu_xor(gb, A);
  CYC(0x520e, 0x5211); W8(wWarpsDisabled) = A;
  CYC(0x5211, 0x5214); SET_HL(wWarpDestGroup);
  CYC(0x5214, 0x5217); A = W8(wActiveGroup);
  CYC(0x5217, 0x5219); alu_or(gb, 0x80);
  CYC(0x5219, 0x521a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x521a, 0x521d); A = W8(wDungeonWallmasterDestRoom);
  CYC(0x521d, 0x521e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x521e, 0x5220); A = 0x05;
  CYC(0x5220, 0x5221); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5221, 0x5223); A = 0x87;
  CYC(0x5223, 0x5224); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5224, 0x5226); mem_wr(gb, HL, 0x03);

done:
  CYC(0x5226, 0x5227); ret_effect(gb);
}

static void link_state13_substate1(GB *gb, uint16_t sp0_) {
  CYC(0x524a, 0x524c); C = 0x40;
  CALL_C(0x524c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x524f);
  CYC(0x524f, 0x5252); A = W8(wcc50);
  CYC(0x5252, 0x5253); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5253, 0x5255);
    goto update_counter;
  }
  CYC(0x5253, 0x5255);
  CALL_C(0x5255, updateLinkDirectionFromAngle_hook, 0x2b64, 0x5258);
  CYC(0x5258, 0x525a); L = 0x2a;
  CYC(0x525a, 0x525b); A = mem_rd(gb, HL);
  CYC(0x525b, 0x525c); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x525c, 0x525e); goto restore; }
  CYC(0x525c, 0x525e);

update_counter:
  CYC(0x525e, 0x5260); C = 0x01;
  CYC(0x5260, 0x5263); A = W8(wGameKeysJustPressed);
  CYC(0x5263, 0x5264); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5264, 0x5266);
  } else {
    CYC(0x5264, 0x5266);
    CYC(0x5266, 0x5268); C = 0x04;
  }
  CYC(0x5268, 0x526a); L = 0x06;
  CYC(0x526a, 0x526b); A = mem_rd(gb, HL);
  CYC(0x526b, 0x526c); alu_sub(gb, C);
  CYC(0x526c, 0x526d); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(0x526d, 0x526e); ret_effect(gb); return; }
  CYC(0x526d, 0x526e);

restore:
  CYC(0x526e, 0x5270); L = 0x1b;
  CYC(0x5270, 0x5272); A = 0x08;
  CYC(0x5272, 0x5273); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5273, 0x5274); mem_wr(gb, HL, A);
  CYC(0x5274, 0x5276); L = 0x2d;
  CYC(0x5276, 0x5278); mem_wr(gb, HL, 0x00);
  CYC(0x5278, 0x5279); alu_xor(gb, A);
  CYC(0x5279, 0x527c); W8(wLinkForceState) = A;
  CYC(0x527c, 0x527f);
  initLinkStateAndAnimateStanding(gb);
}

void linkState13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5227, 0x5229); A = 0x80;
  CYC(0x5229, 0x522c); W8(wForceLinkPushAnimation) = A;
  CYC(0x522c, 0x522e); E = 0x05;
  CYC(0x522e, 0x522f); A = mem_rd(gb, DE);
  CYC(0x522f, 0x5230); push_effect(gb, 0x5230);
  switch (link_jump_table(gb)) {
    case 0x5234: break;
    case 0x524a: link_state13_substate1(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5234, itemIncSubstate_hook, 0x23ef, 0x5237);
  CYC(0x5237, 0x5238); L = alu_inc8(gb, L);
  CYC(0x5238, 0x523a); mem_wr(gb, HL, 0xb4);
  CYC(0x523a, 0x523c); L = 0x1b;
  CYC(0x523c, 0x523e); A = 0x0f;
  CYC(0x523e, 0x523f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x523f, 0x5240); mem_wr(gb, HL, A);
  CYC(0x5240, 0x5242); A = 0x7f;
  CALL_C(0x5242, loadPaletteHeader_hook, 0x050b, 0x5245);
  CYC(0x5245, 0x5246); alu_xor(gb, A);
  CYC(0x5246, 0x5249); W8(wcc50) = A;
  CYC(0x5249, 0x524a); ret_effect(gb);
}

void linkState14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x527f, 0x5281); E = 0x05;
  CYC(0x5281, 0x5282); A = mem_rd(gb, DE);
  CYC(0x5282, 0x5283); push_effect(gb, 0x5283);
  switch (link_jump_table(gb)) {
    case 0x5287: break;
    case 0x524a: link_state13_substate1(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
  CALL_C(0x5287, itemIncSubstate_hook, 0x23ef, 0x528a);
  CYC(0x528a, 0x528c); L = 0x06;
  CYC(0x528c, 0x528e); mem_wr(gb, HL, 0xf0);
  CALL_C(0x528e, linkCancelAllItemUsage_hook, 0x4f49, 0x5291);
  CYC(0x5291, 0x5294); A = W8(wcc50);
  CYC(0x5294, 0x5295); alu_or(gb, A);
  CYC(0x5295, 0x5297); A = 0x02;
  if (F & FZ) {
    CYCT(0x5297, 0x5299);
  } else {
    CYC(0x5297, 0x5299);
    CYC(0x5299, 0x529b); A = 0x10;
  }
  CYC(0x529b, 0x529e);
  specialObjectSetAnimation_hook(gb);
}

void linkState0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x529e, 0x52a0); A = 0x80;
  CYC(0x52a0, 0x52a3); W8(wcc92) = A;
  CYC(0x52a3, 0x52a5); E = 0x05;
  CYC(0x52a5, 0x52a6); A = mem_rd(gb, DE);
  CYC(0x52a6, 0x52a7); push_effect(gb, 0x52a7);
  switch (link_jump_table(gb)) {
    case 0x52b1: goto substate0;
    case 0x4268: updateLinkDamageTaken_hook(gb); return;
    case 0x52be: goto substate2;
    case 0x52dc: goto substate3;
    case 0x52ef: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x52b1, 0x52b3); A = 0x01;
  CYC(0x52b3, 0x52b4); mem_wr(gb, DE, A);
  CYC(0x52b4, 0x52b7); W8(wWarpsDisabled) = A;
  CYC(0x52b7, 0x52b9); E = 0x30;
  CYC(0x52b9, 0x52ba); alu_xor(gb, A);
  CYC(0x52ba, 0x52bb); mem_wr(gb, DE, A);
  CYC(0x52bb, 0x52be);
  linkCancelAllItemUsage_hook(gb);
  return;

substate2:
  CYC(0x52be, 0x52c0); A = 0x03;
  CYC(0x52c0, 0x52c1); mem_wr(gb, DE, A);
  CYC(0x52c1, 0x52c2); H = D;
  CYC(0x52c2, 0x52c4); L = 0x06;
  CYC(0x52c4, 0x52c6); mem_wr(gb, HL, 0x1e);
  CYC(0x52c6, 0x52c8); L = 0x14;
  CYC(0x52c8, 0x52ca); A = 0x20;
  CYC(0x52ca, 0x52cb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x52cb, 0x52cd); mem_wr(gb, HL, 0xfe);
  CYC(0x52cd, 0x52cf); L = 0x08;
  CYC(0x52cf, 0x52d0); alu_xor(gb, A);
  CYC(0x52d0, 0x52d1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x52d1, 0x52d3); mem_wr(gb, HL, 0x10);
  CYC(0x52d3, 0x52d5); L = 0x10;
  CYC(0x52d5, 0x52d7); mem_wr(gb, HL, 0x3c);
  CYC(0x52d7, 0x52d9); A = 0x03;
  CYC(0x52d9, 0x52dc);
  specialObjectSetAnimation_hook(gb);
  return;

substate3:
  CALL_C(0x52dc, itemDecCounter1_hook, 0x23d6, 0x52df);
  if (F & FZ) { CYCT(0x52df, 0x52e1); goto restore_normal; }
  CYC(0x52df, 0x52e1);
  CYC(0x52e1, 0x52e3); C = 0x20;
  CALL_C(0x52e3, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x52e6);
  CALL_C(0x52e6, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x52e9);
  CALL_C(0x52e9, specialObjectUpdatePosition_hook, 0x5d97, 0x52ec);
  CYC(0x52ec, 0x52ef);
  specialObjectAnimate_hook(gb);
  return;

substate4:
  CYC(0x52ef, 0x52f0); H = D;
  CYC(0x52f0, 0x52f2); L = 0x2b;
  CYC(0x52f2, 0x52f4); mem_wr(gb, HL, 0x94);

restore_normal:
  CYC(0x52f4, 0x52f5); alu_xor(gb, A);
  CYC(0x52f5, 0x52f8); W8(wWarpsDisabled) = A;
  CYC(0x52f8, 0x52fb);
  initLinkStateAndAnimateStanding(gb);
}

void linkState05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x52fb, 0x52fd); E = 0x05;
  CYC(0x52fd, 0x52fe); A = mem_rd(gb, DE);
  CYC(0x52fe, 0x52ff); push_effect(gb, 0x52ff);
  switch (link_jump_table(gb)) {
    case 0x5305: goto substate0;
    case 0x5323: goto substate1;
    case 0x5338: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x5305, itemIncSubstate_hook, 0x23ef, 0x5308);
  CYC(0x5308, 0x530a); L = 0x10;
  CYC(0x530a, 0x530c); mem_wr(gb, HL, 0x14);
  CYC(0x530c, 0x530e); L = 0x18;
  CYC(0x530e, 0x5310); A = 0x02;
  CALL_C(0x5310, specialObjectSetVar37AndVar38, 0x5fed, 0x5313);
  CYC(0x5313, 0x5316); SET_BC(0xfe80);
  CALL_C(0x5316, objectSetSpeedZ_hook, 0x239d, 0x5319);
  CYC(0x5319, 0x531b); A = 0x81;
  CYC(0x531b, 0x531e); W8(wLinkInAir) = A;
  CYC(0x531e, 0x5320); A = 0x2f;
  CYC(0x5320, 0x5323);
  specialObjectSetAnimation_hook(gb);
  return;

substate1:
  CALL_C(0x5323, specialObjectAnimate_hook, 0x2aef, 0x5326);
  CALL_C(0x5326, specialObjectSetAngleRelativeToVar38, 0x5ff3, 0x5329);
  CALL_C(0x5329, objectApplySpeed_hook, 0x201d, 0x532c);
  CYC(0x532c, 0x532e); C = 0x20;
  CALL_C(0x532e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5331);
  if (!(F & FZ)) { CYCT(0x5331, 0x5332); ret_effect(gb); return; }
  CYC(0x5331, 0x5332);
  CALL_C(0x5332, itemIncSubstate_hook, 0x23ef, 0x5335);
  CYC(0x5335, 0x5338);
  specialObjectSetPositionToVar38IfSet_hook(gb);
  return;

substate2:
  CALL_C(0x5338, specialObjectAnimate_hook, 0x2aef, 0x533b);
  CYC(0x533b, 0x533c); H = D;
  CYC(0x533c, 0x533e); L = 0x21;
  CYC(0x533e, 0x533f); A = mem_rd(gb, HL);
  CYC(0x533f, 0x5341); mem_wr(gb, HL, 0x00);
  CYC(0x5341, 0x5342); push_effect(gb, 0x5342);
  switch (link_jump_table(gb)) {
    case 0x534c: goto anim_parameter1;
    case 0x5355: goto anim_parameter2;
    case 0x535a: goto anim_parameter0;
    case 0x535b: goto anim_parameter3;
    case 0x535e: goto anim_parameter4;
    default: hook_continue(gb, HL, sp0_); return;
  }

anim_parameter1:
  CALL_C(0x534c, darkenRoomLightly_hook, 0x32f8, 0x534f);
  CYC(0x534f, 0x5351); A = 0x06;
  CYC(0x5351, 0x5354); W8(wPaletteThread_updateRate) = A;
  CYC(0x5354, 0x5355); ret_effect(gb);
  return;

anim_parameter2:
  CYC(0x5355, 0x5358); SET_HL(wLinkMaxHealth);
  CYC(0x5358, 0x5359); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5359, 0x535a); mem_wr(gb, HL, A);

anim_parameter0:
  CYC(0x535a, 0x535b); ret_effect(gb);
  return;

anim_parameter3:
  CYC(0x535b, 0x535e);
  brightenRoom_hook(gb);
  return;

anim_parameter4:
  CYC(0x535e, 0x5361); SET_BC(0xfe80);
  CALL_C(0x5361, objectSetSpeedZ_hook, 0x239d, 0x5364);
  CYC(0x5364, 0x5366); L = 0x08;
  CYC(0x5366, 0x5368); mem_wr(gb, HL, 0x03);
  CYC(0x5368, 0x5369); L = alu_inc8(gb, L);
  CYC(0x5369, 0x536b); mem_wr(gb, HL, 0x18);
  CYC(0x536b, 0x536d); L = 0x10;
  CYC(0x536d, 0x536f); mem_wr(gb, HL, 0x14);
  CYC(0x536f, 0x5371); A = 0x81;
  CYC(0x5371, 0x5374); W8(wLinkInAir) = A;
  CYC(0x5374, 0x5377);
  initLinkStateAndAnimateStanding(gb);
}

void linkState06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5377, 0x5379); E = 0x05;
  CYC(0x5379, 0x537a); A = mem_rd(gb, DE);
  CYC(0x537a, 0x537b); push_effect(gb, 0x537b);
  switch (link_jump_table(gb)) {
    case 0x5383: goto substate0;
    case 0x539d: goto substate1;
    case 0x53b0: goto substate2;
    case 0x53d1: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x5383, 0x5385); A = 0x01;
  CYC(0x5385, 0x5386); mem_wr(gb, DE, A);
  CYC(0x5386, 0x5387); H = D;
  CYC(0x5387, 0x5389); L = 0x06;
  CYC(0x5389, 0x538b); mem_wr(gb, HL, 0x08);
  CYC(0x538b, 0x538d); L = 0x10;
  CYC(0x538d, 0x538f); mem_wr(gb, HL, 0x50);
  CYC(0x538f, 0x5391); L = 0x09;
  CYC(0x5391, 0x5393); mem_wr(gb, HL, 0x00);
  CYC(0x5393, 0x5395); A = 0x81;
  CYC(0x5395, 0x5398); W8(wLinkInAir) = A;
  CYC(0x5398, 0x539a); A = 0x53;
  CALL_C(0x539a, playSound_b00_hook, 0x0c98, 0x539d);

substate1:
  CALL_C(0x539d, specialObjectUpdatePositionWithoutTileEdgeAdjust, 0x5d8e, 0x53a0);
  CALL_C(0x53a0, itemDecCounter1_hook, 0x23d6, 0x53a3);
  if (!(F & FZ)) { CYCT(0x53a3, 0x53a4); ret_effect(gb); return; }
  CYC(0x53a3, 0x53a4);
  CYC(0x53a4, 0x53a6); L = 0x05;
  CYC(0x53a6, 0x53a7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53a7, 0x53a9); L = 0x08;
  CYC(0x53a9, 0x53ab); mem_wr(gb, HL, 0x00);
  CYC(0x53ab, 0x53ad); A = 0x04;
  CALL_C(0x53ad, specialObjectSetAnimation_hook, 0x2b0a, 0x53b0);

substate2:
  CALL_C(0x53b0, specialObjectAnimate_hook, 0x2aef, 0x53b3);
  CYC(0x53b3, 0x53b6); A = W8(wScrollMode);
  CYC(0x53b6, 0x53b8); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x53b8, 0x53b9); ret_effect(gb); return; }
  CYC(0x53b8, 0x53b9);
  CALL_C(0x53b9, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x53bc);
  if (F & FC) {
    CYCT(0x53bc, 0x53bf);
    specialObjectUpdatePositionWithoutTileEdgeAdjust(gb);
    return;
  }
  CYC(0x53bc, 0x53bf);
  CYC(0x53bf, 0x53c2); SET_BC(0xfe00);
  CALL_C(0x53c2, objectSetSpeedZ_hook, 0x239d, 0x53c5);
  CYC(0x53c5, 0x53c7); L = 0x05;
  CYC(0x53c7, 0x53c8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x53c8, 0x53ca); L = 0x10;
  CYC(0x53ca, 0x53cc); mem_wr(gb, HL, 0x0a);
  CYC(0x53cc, 0x53ce); A = 0x18;
  CALL_C(0x53ce, specialObjectSetAnimation_hook, 0x2b0a, 0x53d1);

substate3:
  CALL_C(0x53d1, specialObjectAnimate_hook, 0x2aef, 0x53d4);
  CALL_C(0x53d4, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x53d7);
  CALL_C(0x53d7, specialObjectUpdatePosition_hook, 0x5d97, 0x53da);
  CYC(0x53da, 0x53dc); C = 0x18;
  CALL_C(0x53dc, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x53df);
  if (!(F & FZ)) { CYCT(0x53df, 0x53e0); ret_effect(gb); return; }
  CYC(0x53df, 0x53e0);
  CYC(0x53e0, 0x53e1); alu_xor(gb, A);
  CYC(0x53e1, 0x53e4); W8(wLinkInAir) = A;
  CYC(0x53e4, 0x53e7); W8(wWarpsDisabled) = A;
  CYC(0x53e7, 0x53ea);
  initLinkStateAndAnimateStanding(gb);
}

void linkState09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x53ea, 0x53ec); E = 0x05;
  CYC(0x53ec, 0x53ed); A = mem_rd(gb, DE);
  CYC(0x53ed, 0x53ee); push_effect(gb, 0x53ee);
  switch (link_jump_table(gb)) {
    case 0x53fa: goto substate0;
    case 0x540c: goto substate1;
    case 0x5419: goto substate2;
    case 0x5426: goto substate3;
    case 0x543e: goto substate4;
    case 0x5454: goto substate5;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CALL_C(0x53fa, itemIncSubstate_hook, 0x23ef, 0x53fd);
  CYC(0x53fd, 0x53ff); L = 0x10;
  CYC(0x53ff, 0x5401); mem_wr(gb, HL, 0x28);
  CYC(0x5401, 0x5403); L = 0x08;
  CYC(0x5403, 0x5405); mem_wr(gb, HL, 0x03);
  CYC(0x5405, 0x5406); L = alu_inc8(gb, L);
  CYC(0x5406, 0x5408); mem_wr(gb, HL, 0x08);
  CYC(0x5408, 0x540a); L = 0x06;
  CYC(0x540a, 0x540c); mem_wr(gb, HL, 0x0c);

substate1:
  CALL_C(0x540c, itemDecCounter1_hook, 0x23d6, 0x540f);
  if (!(F & FZ)) { CYCT(0x540f, 0x5411); goto animate; }
  CYC(0x540f, 0x5411);
  CYC(0x5411, 0x5413); mem_wr(gb, HL, 0x0c);
  CYC(0x5413, 0x5414); L = E;
  CYC(0x5414, 0x5415); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5415, 0x5417); L = 0x09;
  CYC(0x5417, 0x5419); mem_wr(gb, HL, 0x18);

substate2:
  CALL_C(0x5419, itemDecCounter1_hook, 0x23d6, 0x541c);
  if (!(F & FZ)) { CYCT(0x541c, 0x541e); goto animate; }
  CYC(0x541c, 0x541e);
  CYC(0x541e, 0x5420); mem_wr(gb, HL, 0x32);
  CYC(0x5420, 0x5421); L = E;
  CYC(0x5421, 0x5422); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5422, 0x5424); L = 0x08;
  CYC(0x5424, 0x5426); mem_wr(gb, HL, 0x02);

substate3:
  CALL_C(0x5426, itemDecCounter1_hook, 0x23d6, 0x5429);
  if (!(F & FZ)) { CYCT(0x5429, 0x542a); ret_effect(gb); return; }
  CYC(0x5429, 0x542a);
  CYC(0x542a, 0x542b); L = E;
  CYC(0x542b, 0x542c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x542c, 0x542e); L = 0x08;
  CYC(0x542e, 0x5430); mem_wr(gb, HL, 0x00);
  CYC(0x5430, 0x5431); L = alu_inc8(gb, L);
  CYC(0x5431, 0x5433); mem_wr(gb, HL, 0x10);
  CYC(0x5433, 0x5435); L = 0x06;
  CYC(0x5435, 0x5437); A = 0x1e;
  CYC(0x5437, 0x5438); mem_wr(gb, HL, A);
  CYC(0x5438, 0x543b); SET_BC(0xf4f8);
  CYC(0x543b, 0x543e);
  objectCreateExclamationMark_hook(gb);
  return;

substate4:
  CALL_C(0x543e, itemDecCounter1_hook, 0x23d6, 0x5441);
  if (!(F & FZ)) { CYCT(0x5441, 0x5442); ret_effect(gb); return; }
  CYC(0x5441, 0x5442);
  CYC(0x5442, 0x5443); L = E;
  CYC(0x5443, 0x5444); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5444, 0x5447); SET_BC(0xfe80);
  CALL_C(0x5447, objectSetSpeedZ_hook, 0x239d, 0x544a);
  CYC(0x544a, 0x544c); A = 0x18;
  CALL_C(0x544c, specialObjectSetAnimation_hook, 0x2b0a, 0x544f);
  CYC(0x544f, 0x5451); A = 0x53;
  CYC(0x5451, 0x5454);
  playSound_b00_hook(gb);
  return;

substate5:
  CYC(0x5454, 0x5456); C = 0x18;
  CALL_C(0x5456, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5459);
  if (!(F & FZ)) { CYCT(0x5459, 0x545b); goto animate; }
  CYC(0x5459, 0x545b);
  CYC(0x545b, 0x545d); L = 0x05;
  CYC(0x545d, 0x545e); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x545e, 0x5460); mem_wr(gb, HL, 0x08);
  CYC(0x5460, 0x5461); ret_effect(gb);
  return;

animate:
  CALL_C(0x5461, specialObjectAnimate_hook, 0x2aef, 0x5464);
  CYC(0x5464, 0x5467);
  specialObjectUpdatePositionWithoutTileEdgeAdjust(gb);
}

void linkState11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5467, 0x5469); E = 0x05;
  CYC(0x5469, 0x546a); A = mem_rd(gb, DE);
  CYC(0x546a, 0x546b); push_effect(gb, 0x546b);
  switch (link_jump_table(gb)) {
    case 0x5471: goto substate0;
    case 0x548d: goto substate1;
    case 0x549c: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x5471, 0x5473); A = 0x01;
  CYC(0x5473, 0x5474); mem_wr(gb, DE, A);
  CALL_C(0x5474, linkCancelAllItemUsage_hook, 0x4f49, 0x5477);
  CYC(0x5477, 0x5478); alu_xor(gb, A);
  CYC(0x5478, 0x547a); E = 0x24;
  CYC(0x547a, 0x547b); mem_wr(gb, DE, A);
  CYC(0x547b, 0x547d); A = 0x4e;
  CALL_C(0x547d, playSound_b00_hook, 0x0c98, 0x5480);
  CYC(0x5480, 0x5483); A = W8(wcc50);
  CYC(0x5483, 0x5485); alu_and(gb, 0x7f);
  CYC(0x5485, 0x5487); A = 0x06;
  if (F & FZ) {
    CYCT(0x5487, 0x5489);
  } else {
    CYC(0x5487, 0x5489);
    CYC(0x5489, 0x548a); A = alu_inc8(gb, A);
  }
  CALL_C(0x548a, specialObjectSetAnimation_hook, 0x2b0a, 0x548d);

substate1:
  CALL_C(0x548d, specialObjectAnimate_hook, 0x2aef, 0x5490);
  CYC(0x5490, 0x5492); E = 0x21;
  CYC(0x5492, 0x5493); A = mem_rd(gb, DE);
  CYC(0x5493, 0x5494); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x5494, 0x5495); ret_effect(gb); return; }
  CYC(0x5494, 0x5495);
  CALL_C(0x5495, itemIncSubstate_hook, 0x23ef, 0x5498);
  CYC(0x5498, 0x549a); L = 0x06;
  CYC(0x549a, 0x549c); mem_wr(gb, HL, 0x14);

substate2:
  CALL_C(0x549c, specialObjectAnimate_hook, 0x2aef, 0x549f);
  CYC(0x549f, 0x54a2); A = W8(wFrameCounter);
  CYC(0x54a2, 0x54a3); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x54a3, 0x54a6);
    objectSetInvisible_hook(gb);
    return;
  }
  CYC(0x54a3, 0x54a6);
  CALL_C(0x54a6, objectSetVisible_hook, 0x1e84, 0x54a9);
  CALL_C(0x54a9, itemDecCounter1_hook, 0x23d6, 0x54ac);
  if (!(F & FZ)) { CYCT(0x54ac, 0x54ad); ret_effect(gb); return; }
  CYC(0x54ac, 0x54ad);
  CYC(0x54ad, 0x54b0); A = W8(wcc50);
  CYC(0x54b0, 0x54b2); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x54b2, 0x54b4); goto force_state; }
  CYC(0x54b2, 0x54b4);
  CALL_C(0x54b4, respawnLink_hook, 0x2ae3, 0x54b7);
  CYC(0x54b7, 0x54b9);
  checkLinkForceState_hook(gb);
  return;

force_state:
  CYC(0x54b9, 0x54bb); A = 0x03;
  CYC(0x54bb, 0x54be); W8(wLinkForceState) = A;
  CYC(0x54be, 0x54c0);
  checkLinkForceState_hook(gb);
}

static void link_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void link_check_tile_collision_at_allow_raised_floor(GB *gb) {
  CYC(0x5ef2, 0x5ef3); A = B;
  CYC(0x5ef3, 0x5ef5); alu_and(gb, 0xf0);
  CYC(0x5ef5, 0x5ef6); L = A;
  CYC(0x5ef6, 0x5ef7); A = C;
  CYC(0x5ef7, 0x5ef9); A = alu_swap(gb, A);
  CYC(0x5ef9, 0x5efb); alu_and(gb, 0x0f);
  CYC(0x5efb, 0x5efc); alu_or(gb, L);
  CYC(0x5efc, 0x5efd); L = A;
  CYC(0x5efd, 0x5eff); H = (uint8_t)(wRoomCollisions >> 8);
  CYC(0x5eff, 0x5f00); A = mem_rd(gb, HL);
  CYC(0x5f00, 0x5f02); alu_cp(gb, 0x10);
  if (F & FC) {
    CYCT(0x5f02, 0x5f04);
    goto simple_collision;
  }
  CYC(0x5f02, 0x5f04);
  CYC(0x5f04, 0x5f06); alu_and(gb, 0x0f);
  CYC(0x5f06, 0x5f09); SET_HL(adjacentWallSpecialCollisions_bank05);
  CYC(0x5f09, 0x5f0a); link_add_a_to_hl(gb, 0x5f0a);
  CYC(0x5f0a, 0x5f0b); E = mem_rd(gb, HL);
  CYC(0x5f0b, 0x5f0d); alu_cp(gb, 0x08);
  CYC(0x5f0d, 0x5f0e); A = B;
  if (!(F & FC)) {
    CYCT(0x5f0e, 0x5f10);
  } else {
    CYC(0x5f0e, 0x5f10);
    CYC(0x5f10, 0x5f11); A = C;
  }
  CYC(0x5f11, 0x5f12); alu_rrca(gb);
  CYC(0x5f12, 0x5f14); alu_and(gb, 0x07);
  CYC(0x5f14, 0x5f17); SET_HL(bitTable);
  CYC(0x5f17, 0x5f18); alu_add(gb, L);
  CYC(0x5f18, 0x5f19); L = A;
  CYC(0x5f19, 0x5f1a); A = mem_rd(gb, HL);
  CYC(0x5f1a, 0x5f1b); alu_and(gb, E);
  if (F & FZ) { CYCT(0x5f1b, 0x5f1c); ret_effect(gb); return; }
  CYC(0x5f1b, 0x5f1c);
  CYC(0x5f1c, 0x5f1d); alu_scf(gb);
  CYC(0x5f1d, 0x5f1e); ret_effect(gb); return;

simple_collision:
  CYC(0x5f2e, 0x5f30); alu_bit(gb, 3, B);
  if (!(F & FZ)) {
    CYCT(0x5f30, 0x5f32);
  } else {
    CYC(0x5f30, 0x5f32);
    CYC(0x5f32, 0x5f33); alu_rrca(gb);
    CYC(0x5f33, 0x5f34); alu_rrca(gb);
  }
  CYC(0x5f34, 0x5f36); alu_bit(gb, 3, C);
  if (!(F & FZ)) {
    CYCT(0x5f36, 0x5f38);
  } else {
    CYC(0x5f36, 0x5f38);
    CYC(0x5f38, 0x5f39); alu_rrca(gb);
  }
  CYC(0x5f39, 0x5f3a); alu_rrca(gb);
  CYC(0x5f3a, 0x5f3b); ret_effect(gb);
}

void calculateAdjacentWallsBitset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ea3, 0x5ea5); A = 0x01;
  CYC(0x5ea5, 0x5ea7); H8(hFF8B) = A;
  CYC(0x5ea7, 0x5eaa); SET_HL(adjacentWallOverworldOffsets_bank05);
  CYC(0x5eaa, 0x5ead); A = W8(wTilesetFlags);
  CYC(0x5ead, 0x5eaf); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(0x5eaf, 0x5eb1);
  } else {
    CYC(0x5eaf, 0x5eb1);
    CYC(0x5eb1, 0x5eb4); SET_HL(adjacentWallSidescrollOffsets_bank05);
  }

  for (;;) {
    CYC(0x5eb4, 0x5eb5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5eb5, 0x5eb6); alu_add(gb, B);
    CYC(0x5eb6, 0x5eb7); B = A;
    CYC(0x5eb7, 0x5eb8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5eb8, 0x5eb9); alu_add(gb, C);
    CYC(0x5eb9, 0x5eba); C = A;
    CYC(0x5eba, 0x5ebb); push_effect(gb, HL);
    CYC(0x5ebb, 0x5ebe); A = W8(wLinkRaisedFloorOffset);
    CYC(0x5ebe, 0x5ebf); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x5ebf, 0x5ec1);
      CALL_C(0x5ec6, checkTileCollisionAt_allowHoles_hook, 0x14d1, 0x5ec9);
    } else {
      CYC(0x5ebf, 0x5ec1);
      CYC(0x5ec1, 0x5ec4); push_effect(gb, 0x5ec4);
      link_check_tile_collision_at_allow_raised_floor(gb);
      CYC(0x5ec4, 0x5ec6);
    }
    CYC(0x5ec9, 0x5eca); SET_HL(pop_effect(gb));
    CYC(0x5eca, 0x5ecc); A = H8(hFF8B);
    CYC(0x5ecc, 0x5ecd); alu_rla(gb);
    CYC(0x5ecd, 0x5ecf); H8(hFF8B) = A;
    if (!(F & FC)) {
      CYCT(0x5ecf, 0x5ed1);
      continue;
    }
    CYC(0x5ecf, 0x5ed1);
    CYC(0x5ed1, 0x5ed2); ret_effect(gb);
    return;
  }
}

static void special_object_tile_edge_adjust(GB *gb) {
  CYC(0x5e0f, 0x5e10); A = C;
  CYC(0x5e10, 0x5e13); SET_HL(slideAngleTable_bank00);
  CYC(0x5e13, 0x5e14); link_add_a_to_hl(gb, 0x5e14);
  CYC(0x5e14, 0x5e15); A = mem_rd(gb, HL);
  CYC(0x5e15, 0x5e17); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x5e17, 0x5e18); ret_effect(gb); return; }
  CYC(0x5e17, 0x5e18);
  CYC(0x5e18, 0x5e19); A = mem_rd(gb, HL);
  CYC(0x5e19, 0x5e1a); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x5e1a, 0x5e1c);
    goto bit7;
  }
  CYC(0x5e1a, 0x5e1c);
  CYC(0x5e1c, 0x5e1d); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x5e1d, 0x5e1f);
    goto bit6;
  }
  CYC(0x5e1d, 0x5e1f);
  CYC(0x5e1f, 0x5e20); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x5e20, 0x5e22);
    goto bit5;
  }
  CYC(0x5e20, 0x5e22);
  CYC(0x5e22, 0x5e23); A = E;
  CYC(0x5e23, 0x5e25); alu_and(gb, 0xcc);
  CYC(0x5e25, 0x5e27); alu_cp(gb, 0x04);
  CYC(0x5e27, 0x5e29); A = 0x00;
  if (F & FZ) { CYCT(0x5e29, 0x5e2a); ret_effect(gb); return; }
  CYC(0x5e29, 0x5e2a);
  CYC(0x5e2a, 0x5e2b); A = E;
  CYC(0x5e2b, 0x5e2d); alu_and(gb, 0x3c);
  CYC(0x5e2d, 0x5e2f); alu_cp(gb, 0x08);
  CYC(0x5e2f, 0x5e31); A = 0x10;
  CYC(0x5e31, 0x5e32); ret_effect(gb); return;

bit5:
  CYC(0x5e32, 0x5e33); A = E;
  CYC(0x5e33, 0x5e35); alu_and(gb, 0xc3);
  CYC(0x5e35, 0x5e37); alu_cp(gb, 0x01);
  CYC(0x5e37, 0x5e39); A = 0x00;
  if (F & FZ) { CYCT(0x5e39, 0x5e3a); ret_effect(gb); return; }
  CYC(0x5e39, 0x5e3a);
  CYC(0x5e3a, 0x5e3b); A = E;
  CYC(0x5e3b, 0x5e3d); alu_and(gb, 0x33);
  CYC(0x5e3d, 0x5e3f); alu_cp(gb, 0x02);
  CYC(0x5e3f, 0x5e41); A = 0x10;
  CYC(0x5e41, 0x5e42); ret_effect(gb); return;

bit7:
  CYC(0x5e42, 0x5e43); A = E;
  CYC(0x5e43, 0x5e45); alu_and(gb, 0xc3);
  CYC(0x5e45, 0x5e47); alu_cp(gb, 0x80);
  CYC(0x5e47, 0x5e49); A = 0x08;
  if (F & FZ) { CYCT(0x5e49, 0x5e4a); ret_effect(gb); return; }
  CYC(0x5e49, 0x5e4a);
  CYC(0x5e4a, 0x5e4b); A = E;
  CYC(0x5e4b, 0x5e4d); alu_and(gb, 0xcc);
  CYC(0x5e4d, 0x5e4f); alu_cp(gb, 0x40);
  CYC(0x5e4f, 0x5e51); A = 0x18;
  CYC(0x5e51, 0x5e52); ret_effect(gb); return;

bit6:
  CYC(0x5e52, 0x5e53); A = E;
  CYC(0x5e53, 0x5e55); alu_and(gb, 0x33);
  CYC(0x5e55, 0x5e57); alu_cp(gb, 0x20);
  CYC(0x5e57, 0x5e59); A = 0x08;
  if (F & FZ) { CYCT(0x5e59, 0x5e5a); ret_effect(gb); return; }
  CYC(0x5e59, 0x5e5a);
  CYC(0x5e5a, 0x5e5b); A = E;
  CYC(0x5e5b, 0x5e5d); alu_and(gb, 0x3c);
  CYC(0x5e5d, 0x5e5f); alu_cp(gb, 0x10);
  CYC(0x5e5f, 0x5e61); A = 0x18;
  CYC(0x5e61, 0x5e62); ret_effect(gb);
}

void specialObjectUpdatePositionGivenVelocity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d9f, 0x5da1); alu_bit(gb, 7, C);
  if (!(F & FZ)) {
    CYCT(0x5da1, 0x5da3);
    CYC(0x5dec, 0x5ded); alu_xor(gb, A);
    CYC(0x5ded, 0x5dee); C = A;
    CYC(0x5dee, 0x5def); ret_effect(gb);
    return;
  }
  CYC(0x5da1, 0x5da3);
  CYC(0x5da3, 0x5da5); E = 0x33;
  CYC(0x5da5, 0x5da6); A = mem_rd(gb, DE);
  CYC(0x5da6, 0x5da7); E = A;
  CYC(0x5da7, 0x5daa); push_effect(gb, 0x5daa);
  special_object_tile_edge_adjust(gb);
  if (!(F & FZ)) {
    CYCT(0x5daa, 0x5dac);
  } else {
    CYC(0x5daa, 0x5dac);
    CYC(0x5dac, 0x5dad); C = A;
    CYC(0x5dad, 0x5daf); E = 0x00;
  }
  CYC(0x5daf, 0x5db0); A = C;
  CYC(0x5db0, 0x5db3); SET_HL(specialObjectVelocityCollisionBits_bank05);
  CYC(0x5db3, 0x5db4); link_add_a_to_hl(gb, 0x5db4);
  CYC(0x5db4, 0x5db5); A = E;
  CYC(0x5db5, 0x5db6); alu_and(gb, mem_rd(gb, HL));
  CYC(0x5db6, 0x5db7); E = A;
  CALL_C(0x5db7, getPositionOffsetForVelocity_hook, 0x2041, 0x5dba);
  CYC(0x5dba, 0x5dbc); C = 0x00;
  CYC(0x5dbc, 0x5dbd); B = E;
  CYC(0x5dbd, 0x5dbe); A = B;
  CYC(0x5dbe, 0x5dc0); alu_and(gb, 0xf0);
  if (!(F & FZ)) {
    CYCT(0x5dc0, 0x5dc2);
    goto horizontal;
  }
  CYC(0x5dc0, 0x5dc2);
  CYC(0x5dc2, 0x5dc3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5dc3, 0x5dc4); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5dc4, 0x5dc6);
    goto horizontal;
  }
  CYC(0x5dc4, 0x5dc6);
  CYC(0x5dc6, 0x5dc7); L = alu_dec8(gb, L);
  CYC(0x5dc7, 0x5dc9); E = 0x0a;
  CYC(0x5dc9, 0x5dca); A = mem_rd(gb, DE);
  CYC(0x5dca, 0x5dcb); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5dcb, 0x5dcc); mem_wr(gb, DE, A);
  CYC(0x5dcc, 0x5dcd); E = alu_inc8(gb, E);
  CYC(0x5dcd, 0x5dce); L = alu_inc8(gb, L);
  CYC(0x5dce, 0x5dcf); A = mem_rd(gb, DE);
  CYC(0x5dcf, 0x5dd0); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x5dd0, 0x5dd1); mem_wr(gb, DE, A);
  CYC(0x5dd1, 0x5dd2); C = alu_inc8(gb, C);

horizontal:
  CYC(0x5dd2, 0x5dd3); A = B;
  CYC(0x5dd3, 0x5dd5); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x5dd5, 0x5dd7);
    goto done;
  }
  CYC(0x5dd5, 0x5dd7);
  CYC(0x5dd7, 0x5dd9); L = (uint8_t)(wTmpcec0 + 3);
  CYC(0x5dd9, 0x5dda); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5dda, 0x5ddb); alu_or(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5ddb, 0x5ddd);
    goto done;
  }
  CYC(0x5ddb, 0x5ddd);
  CYC(0x5ddd, 0x5ddf); E = 0x0c;
  CYC(0x5ddf, 0x5de0); A = mem_rd(gb, DE);
  CYC(0x5de0, 0x5de1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5de1, 0x5de2); mem_wr(gb, DE, A);
  CYC(0x5de2, 0x5de3); L = alu_inc8(gb, L);
  CYC(0x5de3, 0x5de4); E = alu_inc8(gb, E);
  CYC(0x5de4, 0x5de5); A = mem_rd(gb, DE);
  CYC(0x5de5, 0x5de6); alu_adc(gb, mem_rd(gb, HL));
  CYC(0x5de6, 0x5de7); mem_wr(gb, DE, A);
  CYC(0x5de7, 0x5de9); C = (uint8_t)(C | (1 << 1));

done:
  CYC(0x5de9, 0x5dea); A = C;
  CYC(0x5dea, 0x5deb); alu_or(gb, A);
  CYC(0x5deb, 0x5dec); ret_effect(gb);
}

void specialObjectUpdateAdjacentWallsBitset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5e62, 0x5e64); E = 0x33;
  CYC(0x5e64, 0x5e65); alu_xor(gb, A);
  CYC(0x5e65, 0x5e66); mem_wr(gb, DE, A);
  CYC(0x5e66, 0x5e69); A = W8(wLinkObjectIndex);
  CYC(0x5e69, 0x5e6a); alu_rrca(gb);
  if (F & FC) { CYCT(0x5e6a, 0x5e6b); ret_effect(gb); return; }
  CYC(0x5e6a, 0x5e6b);
  CYC(0x5e6b, 0x5e6c); H = D;
  CYC(0x5e6c, 0x5e6e); L = 0x0b;
  CYC(0x5e6e, 0x5e6f); B = mem_rd(gb, HL);
  CYC(0x5e6f, 0x5e71); L = 0x0d;
  CYC(0x5e71, 0x5e72); C = mem_rd(gb, HL);
  CALL_C(0x5e72, calculateAdjacentWallsBitset_hook, 0x5ea3, 0x5e75);
  CYC(0x5e75, 0x5e76); B = A;
  CYC(0x5e76, 0x5e79); SET_HL(specialObjectAdjacentWallsRemapData_bank05 - 1);

  for (;;) {
    CYC(0x5e79, 0x5e7a); SET_HL(HL + 1);
    CYC(0x5e7a, 0x5e7b); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5e7b, 0x5e7c); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x5e7c, 0x5e7e);
      CYC(0x5e88, 0x5e89); A = B;
      CYC(0x5e89, 0x5e8b); E = 0x33;
      CYC(0x5e8b, 0x5e8c); mem_wr(gb, DE, A);
      CYC(0x5e8c, 0x5e8d); ret_effect(gb);
      return;
    }
    CYC(0x5e7c, 0x5e7e);
    CYC(0x5e7e, 0x5e7f); alu_cp(gb, B);
    if (!(F & FZ)) {
      CYCT(0x5e7f, 0x5e81);
      continue;
    }
    CYC(0x5e7f, 0x5e81);
    CYC(0x5e81, 0x5e82); A = mem_rd(gb, HL);
    CYC(0x5e82, 0x5e84); H8(hFF8B) = A;
    CYC(0x5e84, 0x5e86); E = 0x33;
    CYC(0x5e86, 0x5e87); mem_wr(gb, DE, A);
    CYC(0x5e87, 0x5e88); ret_effect(gb);
    return;
  }
}

void updateLinkSpeed_withParam_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ce8, 0x5cea); E = 0x36;
  CYC(0x5cea, 0x5ceb); A = mem_rd(gb, DE);
  CYC(0x5ceb, 0x5cec); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x5cec, 0x5cee);
    goto determine_speed;
  }
  CYC(0x5cec, 0x5cee);
  CYC(0x5cee, 0x5cef); A = C;
  CYC(0x5cef, 0x5cf0); mem_wr(gb, DE, A);
  CYC(0x5cf0, 0x5cf2); alu_and(gb, 0x7f);
  CYC(0x5cf2, 0x5cf5); SET_HL(linkSpeedTable_bank05);
  CYC(0x5cf5, 0x5cf6); link_add_a_to_hl(gb, 0x5cf6);
  CYC(0x5cf6, 0x5cf8); E = 0x10;
  CYC(0x5cf8, 0x5cf9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5cf9, 0x5cfa); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5cfa, 0x5cfc);
  } else {
    CYC(0x5cfa, 0x5cfc);
    CYC(0x5cfc, 0x5cfd); mem_wr(gb, DE, A);
  }
  CYC(0x5cfd, 0x5cfe); alu_xor(gb, A);
  CYC(0x5cfe, 0x5d00); E = 0x12;
  CYC(0x5d00, 0x5d01); mem_wr(gb, DE, A);
  CYC(0x5d01, 0x5d02); E = alu_inc8(gb, E);
  CYC(0x5d02, 0x5d03); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5d03, 0x5d04); mem_wr(gb, DE, A);

determine_speed:
  CYC(0x5d04, 0x5d06); B = 0x02;
  CYC(0x5d06, 0x5d08); E = 0x00;
  CYC(0x5d08, 0x5d0b); A = W8(wActiveTileType);
  CYC(0x5d0b, 0x5d0d); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x5d0d, 0x5d0f);
    goto select_speed;
  }
  CYC(0x5d0d, 0x5d0f);
  CYC(0x5d0f, 0x5d11); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x5d11, 0x5d13);
    goto select_speed;
  }
  CYC(0x5d11, 0x5d13);
  CYC(0x5d13, 0x5d15); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(0x5d15, 0x5d17);
    goto check_pegasus;
  }
  CYC(0x5d15, 0x5d17);
  CYC(0x5d17, 0x5d18); B = alu_inc8(gb, B);
  CYC(0x5d18, 0x5d1a); alu_cp(gb, 0x06);
  if (F & FZ) {
    CYCT(0x5d1a, 0x5d1c);
    goto check_pegasus;
  }
  CYC(0x5d1a, 0x5d1c);
  CYC(0x5d1c, 0x5d1e); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(0x5d1e, 0x5d20);
    goto check_pegasus;
  }
  CYC(0x5d1e, 0x5d20);
  CYC(0x5d20, 0x5d21); B = alu_inc8(gb, B);

check_pegasus:
  CALL_C(0x5d21, checkPegasusSeedCounter_hook, 0x2be8, 0x5d24);
  if (F & FZ) {
    CYCT(0x5d24, 0x5d26);
  } else {
    CYC(0x5d24, 0x5d26);
    CYC(0x5d26, 0x5d28); E = 0x03;
  }

select_speed:
  CYC(0x5d28, 0x5d29); A = E;
  CYC(0x5d29, 0x5d2a); alu_add(gb, B);
  CYC(0x5d2a, 0x5d2b); alu_add(gb, C);
  CYC(0x5d2b, 0x5d2d); alu_and(gb, 0x7f);
  CYC(0x5d2d, 0x5d30); SET_HL(linkSpeedTable_bank05);
  CYC(0x5d30, 0x5d31); link_add_a_to_hl(gb, 0x5d31);
  CYC(0x5d31, 0x5d32); A = mem_rd(gb, HL);
  CYC(0x5d32, 0x5d33); H = D;
  CYC(0x5d33, 0x5d35); L = 0x11;
  CYC(0x5d35, 0x5d36); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x5d36, 0x5d38); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(0x5d38, 0x5d39); ret_effect(gb); return; }
  CYC(0x5d38, 0x5d39);
  CYC(0x5d39, 0x5d3a); mem_wr(gb, HL, A);
  CYC(0x5d3a, 0x5d3b); ret_effect(gb);
}

void linkUpdateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5ad0, 0x5ad1); A = C;
  CYC(0x5ad1, 0x5ad2); alu_rrca(gb);
  CYC(0x5ad2, 0x5ad3); push_effect(gb, AF);
  if (F & FC) {
    CYCT(0x5ad3, 0x5ad5);
    CALL_C(0x5ada, animateLinkWalking_hook, 0x5cc3, 0x5add);
  } else {
    CYC(0x5ad3, 0x5ad5);
    CALL_C(0x5ad5, animateLinkStanding_hook, 0x5cb5, 0x5ad8);
    CYC(0x5ad8, 0x5ada);
  }
  CYC(0x5add, 0x5ade); SET_AF(pop_effect(gb));
  CYC(0x5ade, 0x5adf); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x5adf, 0x5ae1);
    goto reset_speed;
  }
  CYC(0x5adf, 0x5ae1);
  CYC(0x5ae1, 0x5ae2); push_effect(gb, AF);
  CALL_C(0x5ae2, specialObjectUpdatePosition_hook, 0x5d97, 0x5ae5);
  if (F & FZ) {
    CYCT(0x5ae5, 0x5ae7);
    CYC(0x5aef, 0x5af0); SET_AF(pop_effect(gb));
    goto reset_speed;
  }
  CYC(0x5ae5, 0x5ae7);
  CYC(0x5ae7, 0x5ae8); C = A;
  CYC(0x5ae8, 0x5ae9); SET_AF(pop_effect(gb));
  CYC(0x5ae9, 0x5aea); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x5aea, 0x5aeb); ret_effect(gb); return; }
  CYC(0x5aea, 0x5aeb);
  CYC(0x5aeb, 0x5aec); A = C;
  CYC(0x5aec, 0x5aef);
  updateHeartRingCounter_hook(gb);
  return;

reset_speed:
  CYC(0x5af0, 0x5af3);
  linkResetSpeed_hook(gb);
}
