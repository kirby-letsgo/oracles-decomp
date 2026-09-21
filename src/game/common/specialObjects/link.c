#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define linkSpeedTable_bank05 SYM(updateLinkSpeed_withParam__speedTable)
#define specialObjectVelocityCollisionBits_bank05 SYM(specialObjectUpdatePositionGivenVelocity__bitsToCheck)
#define specialObjectAdjacentWallsRemapData_bank05 SYM(specialObjectUpdateAdjacentWallsBitset__data)
#define adjacentWallOverworldOffsets_bank05 SYM(calculateAdjacentWallsBitset__overworldOffsets)
#define adjacentWallSidescrollOffsets_bank05 SYM(calculateAdjacentWallsBitset__sidescrollOffsets)
#define adjacentWallSpecialCollisions_bank05 SYM(calculateAdjacentWallsBitset__specialCollisions)
#define slideAngleTable_bank00 SYM(slideAngleTable)

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
void linkUpdateSwimming_sidescroll_hook(GB *gb);
void linkState01_sidescroll_hook(GB *gb);
void linkUpdateInAir_sidescroll_hook(GB *gb);
void initLinkStateAndAnimateStanding_hook(GB *gb);
void specialObjectUpdatePositionWithoutTileEdgeAdjust_hook(GB *gb);
void checkPositionSurroundedByWalls_hook(GB *gb);
void clearLinkImmobilizedBit4_hook(GB *gb);
void setLinkImmobilizedBit4_hook(GB *gb);
void linkPullIntoHole_hook(GB *gb);
void clearVar37AndVar38_hook(GB *gb);
void specialObjectSetVar37AndVar38_hook(GB *gb);
void specialObjectSetAngleRelativeToVar38_hook(GB *gb);
void linkState12_hook(GB *gb);

static void link_add_a_to_hl(GB *gb, uint16_t return_address);
void linkUpdateVelocity_mermaidSuit_hook(GB *gb);
void link_special_object_update_position_continuation_hook(GB *gb);
void link_special_object_update_position_after_collision_hook(GB *gb);
void link_state12_get_length_of_cliff_hook(GB *gb);

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
  BASE(linkUpdateKnockback);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto L_5d89; } CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); A = mem_rd(gb, wLinkInAir);
  CYC(b_+10, b_+11); alu_rlca(gb);
  if ((F & FC)) { CYCT(b_+11, b_+13); goto L_5d89; } CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); C = 0x01;
  CYC(b_+15, b_+16); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+16, b_+18); goto L_5d6e; } CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); C = alu_inc8(gb, C);
L_5d6e:
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = 0x2d;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+24, b_+25); ret_effect(gb); return; } CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); alu_sub(gb, C);
  if ((F & FC)) { CYCT(b_+26, b_+28); goto L_5d89; } CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CYC(b_+29, b_+31); L = 0x2c;
  CALL_C(b_+31, linkAdjustGivenAngleInSidescrollingArea_hook, SYM(linkAdjustGivenAngleInSidescrollingArea), b_+34);
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+38); B = 0x32;
  CYC(b_+38, b_+41); SET_HL(wcc95);
  CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 5)));
  CYC(b_+43, b_+46); TAIL(specialObjectUpdatePositionGivenVelocity);
L_5d89:
  CYC(b_+46, b_+48); E = 0x2d;
  CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+51); ret_effect(gb); return;
}

// updateHeartRingCounter@addOffsetsToCounter: adds or subtracts (bit 7 of the first byte at de)
// the 16-bit offset at de-1..de to the counter at hl.
static void heart_ring_add_offsets_to_counter(GB *gb) {
  BASE(updateHeartRingCounter__addOffsetsToCounter);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  E = alu_dec8(gb, E);
  alu_rlca(gb);
  CYC(b_+1, b_+3);
  if (!(F & FC)) {
    CYCT(b_+3, b_+5);
    CYC(b_+14, b_+15); A = mem_rd(gb, DE);
    CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(b_+17, b_+18);
    CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  } else {
    CYC(b_+3, b_+5);
    CYC(b_+5, b_+6); A = mem_rd(gb, DE);
    alu_cpl(gb);
    CYC(b_+6, b_+7);
    CYC(b_+7, b_+8); alu_adc(gb, mem_rd(gb, HL));
    CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
    E = alu_inc8(gb, E);
    CYC(b_+9, b_+10);
    CYC(b_+10, b_+11); A = mem_rd(gb, DE);
    alu_cpl(gb);
    CYC(b_+11, b_+12);
    CYCT(b_+12, b_+14);
  }
  CYC(b_+19, b_+20); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  if (!(F & FC)) { CYCT(b_+21, b_+22); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+23, b_+24);
}

void updateHeartRingCounter_hook(GB *gb) {
  BASE(updateHeartRingCounter);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wActiveRing);
  CYC(b_+4, b_+7); SET_BC(0x0208);
  CYC(b_+7, b_+9); alu_cp(gb, 0x13);
  if ((F & FZ)) { CYCT(b_+9, b_+11); goto L_5650; } CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x14);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_5678; } CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_BC(0x0310);
L_5650:
  CYC(b_+18, b_+19); A = E;
  CYC(b_+19, b_+20); alu_or(gb, C);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+22); push_effect(gb, DE);
  CYC(b_+22, b_+25); SET_DE(wEnemyPlacement_numEnemies);
  CYC(b_+25, b_+28); SET_HL(wHeartRingCounter);
  CYC(b_+28, b_+30); C = alu_srl(gb, C);
  if ((F & FC)) { CYCT(b_+30, b_+33); heart_ring_add_offsets_to_counter(gb); } else CYC(b_+30, b_+33);
  CYC(b_+33, b_+35); E = 0xc3;
  CYC(b_+35, b_+37); L = GV(0x53, 0x6e);
  CYC(b_+37, b_+39); C = alu_srl(gb, C);
  if ((F & FC)) { CYCT(b_+39, b_+42); heart_ring_add_offsets_to_counter(gb); } else CYC(b_+39, b_+42);
  CYC(b_+42, b_+43); SET_DE(pop_effect(gb));
  CYC(b_+43, b_+46); A = mem_rd(gb, (wHeartRingCounter + 2));
  CYC(b_+46, b_+47); alu_cp(gb, B);
  if ((F & FC)) { CYCT(b_+47, b_+48); ret_effect(gb); return; } CYC(b_+47, b_+48);
  CYC(b_+48, b_+51); SET_HL(wLinkHealth);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+52, b_+53); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+53, b_+55); A = 0x29;
  if ((F & FC)) { CALL_C_CC(b_+55, giveTreasure_hook, SYM(giveTreasure), b_+58); } else CYC(b_+55, b_+58);
L_5678:
  CYC(b_+58, b_+61); SET_HL(wHeartRingCounter);
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+63, b_+64); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+65, b_+66); ret_effect(gb); return;
}

void animateLinkWalking_hook(GB *gb) {
  BASE(animateLinkWalking);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkPegasusSeedCounter_hook, SYM(checkPegasusSeedCounter), b_+3);
  if ((F & FZ)) { CYCT(b_+3, b_+5); goto L_5cda; } CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+6, b_+8); goto L_5cda; } CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); SET_HL(w1ReservedItemF);
  CYC(b_+11, b_+13); A = 0x03;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x1a);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+18, b_+20); A = 0xa3;
  CALL_C(b_+20, playSound_b00_hook, SYM(playSound_b00), b_+23);
L_5cda:
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); A = 0x10;
  CYC(b_+26, b_+28); L = 0x30;
  CYC(b_+28, b_+29); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+29, b_+32); specialObjectSetAnimation_hook(gb); return; }
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+35); TAIL(specialObjectAnimate);
}

void checkForUnderwaterTransition_hook(GB *gb) {
  BASE(checkForUnderwaterTransition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wDisableScreenTransitions);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+8, b_+10); alu_and(gb, 0x40);
  if ((F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; } CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+14, b_+16); alu_and(gb, 0x02);
  if ((F & FZ)) { CYCT(b_+16, b_+17); ret_effect(gb); return; } CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+20, b_+21); L = A;
  CYC(b_+21, b_+23); H = 0xcf;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+27); SET_HL(SYM(tileTypesTable));
  CALL_C(b_+27, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+30);
  CYC(b_+30, b_+32); alu_cp(gb, 0x18);
  if ((F & FZ)) { CYCT(b_+32, b_+33); ret_effect(gb); return; } CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0x02);
  if ((F & FZ)) { CYCT(b_+35, b_+37); goto L_51b2; } CYC(b_+35, b_+37);
  CALL_C(b_+37, checkLinkCanSurface_hook, SYM(checkLinkCanSurface), b_+40);
  if (!(F & FC)) { CYCT(b_+40, b_+41); ret_effect(gb); return; } CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); SET_AF(pop_effect(gb));
  CYC(b_+42, b_+45); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+45, b_+47); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+47, b_+49); goto L_51a4; } CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); C = 0xfe;
  CYC(b_+51, b_+54); A = mem_rd(gb, wActiveRoom);
  CYC(b_+54, b_+56); goto L_51cf;
L_51a4:
  CYC(b_+56, b_+59); A = mem_rd(gb, wDungeonFloor);
  CYC(b_+59, b_+60); A = alu_inc8(gb, A);
  CYC(b_+60, b_+63); mem_wr(gb, wDungeonFloor, A);
  CALL_C(b_+63, getActiveRoomFromDungeonMapPosition_hook, SYM(getActiveRoomFromDungeonMapPosition), b_+66);
  CYC(b_+66, b_+68); C = 0x00;
  CYC(b_+68, b_+70); goto L_51cf;
L_51b2:
  CYC(b_+70, b_+71); SET_AF(pop_effect(gb));
  CYC(b_+71, b_+74); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+74, b_+76); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+76, b_+78); goto L_51c1; } CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); C = 0x02;
  CYC(b_+80, b_+83); A = mem_rd(gb, wActiveRoom);
  CYC(b_+83, b_+85); goto L_51cf;
L_51c1:
  CYC(b_+85, b_+88); A = mem_rd(gb, wDungeonFloor);
  CYC(b_+88, b_+89); A = alu_dec8(gb, A);
  CYC(b_+89, b_+92); mem_wr(gb, wDungeonFloor, A);
  CALL_C(b_+92, getActiveRoomFromDungeonMapPosition_hook, SYM(getActiveRoomFromDungeonMapPosition), b_+95);
  CYC(b_+95, b_+97); C = 0x00;
  CYC(b_+97, b_+99); goto L_51cf;
L_51cf:
  CYC(b_+99, b_+102); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+102, b_+105); A = mem_rd(gb, wActiveGroup);
  CYC(b_+105, b_+106); alu_add(gb, C);
  CYC(b_+106, b_+108); alu_or(gb, 0x80);
  CYC(b_+108, b_+111); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+111, b_+114); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+114, b_+117); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+117, b_+119); A = 0x00;
  CYC(b_+119, b_+122); mem_wr(gb, wWarpTransition, A);
  CYC(b_+122, b_+124); A = 0x03;
  CYC(b_+124, b_+127); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+127, b_+128); ret_effect(gb); return;
}

void checkLinkForceState_hook(GB *gb) {
  BASE(checkLinkForceState);
  CYC(b_+0, b_+3); SET_HL(wLinkForceState);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+9); SET_HL(pop_effect(gb));
  TAIL(linkSetState);
}

void checkLinkPushingAgainstBed_hook(GB *gb) {
  BASE(checkLinkPushingAgainstBed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wInformativeTextsShown);
  CYC(b_+3, b_+5); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wActiveGroup);
  CYC(b_+9, b_+11); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+11, b_+12); ret_effect(gb); return; } CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_BC(GV(0x9e17, 0x8214));
  CYC(b_+15, b_+17); L = GV(0x01, 0x03);
  CYC(b_+17, b_+20); A = mem_rd(gb, wActiveRoom);
  CYC(b_+20, b_+21); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+21, b_+22); ret_effect(gb); return; } CYC(b_+21, b_+22);
  CYC(b_+22, b_+25); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+25, b_+26); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+26, b_+27); ret_effect(gb); return; } CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); E = 0x08;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_cp(gb, L);
  if (!(F & FZ)) { CYCT(b_+31, b_+32); ret_effect(gb); return; } CYC(b_+31, b_+32);
  CALL_C(b_+32, checkLinkPushingAgainstWall_hook, SYM(checkLinkPushingAgainstWall), b_+35);
  if ((F & FZ)) { CYCT(b_+35, b_+36); ret_effect(gb); return; } CYC(b_+35, b_+36);
  CYC(b_+36, b_+39); SET_HL(wLinkPushingAgainstBedCounter);
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);
  CYC(b_+41, b_+43); alu_cp(gb, 0x5a);
  if ((F & FC)) { CYCT(b_+43, b_+44); ret_effect(gb); return; } CYC(b_+43, b_+44);
  CYC(b_+44, b_+45); SET_HL(pop_effect(gb));
  CYC(b_+45, b_+48); SET_HL(wInformativeTextsShown);
  CYC(b_+48, b_+50); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
  CYC(b_+50, b_+52); A = 0x05;
  CYC(b_+52, b_+55); TAIL(linkSetState);
}

void specialObjectUpdatePosition_hook(GB *gb) {
  BASE(specialObjectUpdatePosition);
  CYC(b_+0, b_+2); E = 0x10;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = 0x09;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); C = A;
  TAIL(specialObjectUpdatePositionGivenVelocity);
}

void animateLinkStanding_hook(GB *gb) {
  BASE(animateLinkStanding);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x30;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_5cc1; } CYC(b_+5, b_+7);
  CALL_C(b_+7, checkPegasusSeedCounter_hook, SYM(checkPegasusSeedCounter), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); animateLinkWalking_hook(gb); return; } CYC(b_+10, b_+12);
L_5cc1:
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  TAIL(animateLinkWalking);
}

void linkResetSpeed_hook(GB *gb) {
  BASE(linkResetSpeed);
  CYC(b_+0, b_+2); E = 0x10;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); ret_effect(gb); return;
}

void updateLinkSpeed_standard_hook(GB *gb) {
  BASE(updateLinkSpeed_standard);
  CYC(b_+0, b_+2); C = 0x00;
  TAIL(updateLinkSpeed_withParam);
}

void linkSetState_hook(GB *gb) {
  BASE(linkSetState);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x04;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x00);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0a);
  if ((F & FZ)) { CYCT(b_+8, b_+10); goto L_54da; } CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x0c);
  if ((F & FZ)) { CYCT(b_+12, b_+14); goto L_54da; } CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+16, b_+17); ret_effect(gb); return; } CYC(b_+16, b_+17);
L_54da:
  CYC(b_+17, b_+20); TAIL(specialObjectCode_link);
}

void func_5631_hook(GB *gb) {
  BASE(func_5631);
  CYC(b_+0, b_+3); SET_HL(wIsTileSlippery);
  CYC(b_+3, b_+5); alu_bit(gb, 6, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x2c;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); E = 0x09;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); ret_effect(gb); return;
}

void overworldSwimmingState1_hook(GB *gb) {
  BASE(overworldSwimmingState1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+3);
  CALL_C(b_+3, linkSetSwimmingSpeed_hook, SYM(linkSetSwimmingSpeed), b_+6);
  CYC(b_+6, b_+8); L = 0x2f;
  CYC(b_+8, b_+10); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(b_+10, b_+12); L = 0x06;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x0a);
  if ((F & FZ)) { CYCT(b_+14, b_+16); goto L_56bf; } CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x02);
L_56bf:
  CYC(b_+18, b_+21); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+21, b_+23); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_56d8; } CYC(b_+23, b_+25);
  CALL_C(b_+25, checkSwimmingOverSeawater_hook, SYM(checkSwimmingOverSeawater), b_+28);
  if ((F & FZ)) { CYCT(b_+28, b_+30); goto L_56d4; } CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0x2e;
  CALL_C(b_+32, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+35);
  CYC(b_+35, b_+37); B = 0x0b;
  if ((F & FC)) { CYCT(b_+37, b_+39); goto L_56f8; } CYC(b_+37, b_+39);
L_56d4:
  CYC(b_+39, b_+41); C = 0x88;
  CYC(b_+41, b_+43); goto L_56da;
L_56d8:
  CYC(b_+43, b_+45); C = 0x78;
L_56da:
  CYC(b_+45, b_+47); A = 0x02;
  CYC(b_+47, b_+50); mem_wr(gb, wLinkForceState, A);
  CYC(b_+50, b_+52); A = 0x04;
  CYC(b_+52, b_+55); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+55, b_+57); A = 0x80;
  CYC(b_+57, b_+60); mem_wr(gb, wcc92, A);
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = 0x2b;
  CYC(b_+63, b_+64); mem_wr(gb, HL, C);
  CYC(b_+64, b_+66); L = 0x24;
  CYC(b_+66, b_+68); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+68, b_+70); A = 0x5f;
  CALL_C(b_+70, playSound_b00_hook, SYM(playSound_b00), b_+73);
  CYC(b_+73, b_+75); B = 0x0a;
L_56f8:
  CYC(b_+75, b_+78); SET_HL(wLinkSwimmingState);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+81); alu_and(gb, 0xf0);
  CYC(b_+81, b_+83); alu_or(gb, 0x02);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A);
  CYC(b_+84, b_+85); A = B;
  CALL_C(b_+85, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+88);
  CYC(b_+88, b_+91); TAIL(linkCreateSplash);
}

void forceDrownLink_hook(GB *gb) {
  BASE(overworldSwimmingState1);
  uint16_t sp0_ = gb->sp;
  goto L_5708;
L_56d8:
  CYC(b_+43, b_+45); C = 0x78;
  CYC(b_+45, b_+47); A = 0x02;
  CYC(b_+47, b_+50); mem_wr(gb, wLinkForceState, A);
  CYC(b_+50, b_+52); A = 0x04;
  CYC(b_+52, b_+55); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+55, b_+57); A = 0x80;
  CYC(b_+57, b_+60); mem_wr(gb, wcc92, A);
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = 0x2b;
  CYC(b_+63, b_+64); mem_wr(gb, HL, C);
  CYC(b_+64, b_+66); L = 0x24;
  CYC(b_+66, b_+68); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+68, b_+70); A = 0x5f;
  CALL_C(b_+70, playSound_b00_hook, SYM(playSound_b00), b_+73);
  CYC(b_+73, b_+75); B = 0x0a;
  CYC(b_+75, b_+78); SET_HL(wLinkSwimmingState);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+81); alu_and(gb, 0xf0);
  CYC(b_+81, b_+83); alu_or(gb, 0x02);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A);
  CYC(b_+84, b_+85); A = B;
  CALL_C(b_+85, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+88);
  CYC(b_+88, b_+91); TAIL(linkCreateSplash);
L_5708:
  CYC(SYM(forceDrownLink), (SYM(forceDrownLink) + 3)); SET_HL(wLinkSwimmingState);
  CYC((SYM(forceDrownLink) + 3), (SYM(forceDrownLink) + 5)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC((SYM(forceDrownLink) + 5), (SYM(forceDrownLink) + 7)); goto L_56d8;
}

void checkSwimmingOverSeawater_hook(GB *gb) {
  BASE(checkSwimmingOverSeawater);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_var2f);
  CYC(b_+3, b_+5); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wActiveTileType);
  CYC(b_+9, b_+11); alu_sub(gb, 0x17);
  CYC(b_+11, b_+12); ret_effect(gb); return;
}

void overworldSwimmingState2_hook(GB *gb) {
  BASE(overworldSwimmingState2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemDecCounter1_hook, SYM(itemDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); specialObjectUpdatePosition_hook(gb); return; } CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); SET_HL(wLinkSwimmingState);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(overworldSwimmingState3);
}

void linkUpdateDrowning_hook(GB *gb) {
  BASE(linkUpdateDrowning);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x80;
  CYC(b_+2, b_+5); mem_wr(gb, wcc92, A);
  CALL_C(b_+5, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+8);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+10); alu_xor(gb, A);
  CYC(b_+10, b_+12); L = 0x24;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x21;
  CYC(b_+15, b_+17); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+17, b_+18); ret_effect(gb); return; } CYC(b_+17, b_+18);
  CYC(b_+18, b_+21); mem_wr(gb, wLinkSwimmingState, A);
  CYC(b_+21, b_+23); A = 0x02;
  CYC(b_+23, b_+26); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+26, b_+28); A = 0x02;
  CYC(b_+28, b_+31); TAIL(linkSetState);
}

void linkSetSwimmingSpeed_hook(GB *gb) {
  BASE(linkSetSwimmingSpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x15;
  CALL_C(b_+2, cpActiveRing_hook, SYM(cpActiveRing), b_+5);
  CYC(b_+5, b_+7); A = 0x23;
  if ((F & FZ)) { CYCT(b_+7, b_+9); goto L_5783; } CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x14;
L_5783:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x10;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); A = 0x03;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = 0x35;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x00);
  CYC(b_+24, b_+25); ret_effect(gb); return;
}

void linkSetSwimmingSpeedTmp_hook(GB *gb) {
  BASE(linkSetSwimmingSpeedTmp);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x15;
  CALL_C(b_+2, cpActiveRing_hook, SYM(cpActiveRing), b_+5);
  CYC(b_+5, b_+7); A = 0x23;
  if ((F & FZ)) { CYCT(b_+7, b_+9); goto L_579c; } CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x14;
L_579c:
  CYC(b_+11, b_+13); E = 0x11;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); ret_effect(gb); return;
}

void linkUpdateDiving_hook(GB *gb) {
  BASE(linkUpdateDiving);
  uint16_t sp0_ = gb->sp;
  goto L_5810;
L_51b2:
  CYC(SYM(checkForUnderwaterTransition__levelDown), (SYM(checkForUnderwaterTransition__levelDown) + 1)); SET_AF(pop_effect(gb));
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 1), (SYM(checkForUnderwaterTransition__levelDown) + 4)); A = mem_rd(gb, wTilesetFlags);
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 4), (SYM(checkForUnderwaterTransition__levelDown) + 6)); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT((SYM(checkForUnderwaterTransition__levelDown) + 6), (SYM(checkForUnderwaterTransition__levelDown) + 8)); goto L_51c1; } CYC((SYM(checkForUnderwaterTransition__levelDown) + 6), (SYM(checkForUnderwaterTransition__levelDown) + 8));
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 8), (SYM(checkForUnderwaterTransition__levelDown) + 10)); C = 0x02;
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 10), (SYM(checkForUnderwaterTransition__levelDown) + 13)); A = mem_rd(gb, wActiveRoom);
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 13), (SYM(checkForUnderwaterTransition__levelDown) + 15)); goto L_51cf;
L_51c1:
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 15), (SYM(checkForUnderwaterTransition__levelDown) + 18)); A = mem_rd(gb, wDungeonFloor);
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 18), (SYM(checkForUnderwaterTransition__levelDown) + 19)); A = alu_dec8(gb, A);
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 19), (SYM(checkForUnderwaterTransition__levelDown) + 22)); mem_wr(gb, wDungeonFloor, A);
  CALL_C((SYM(checkForUnderwaterTransition__levelDown) + 22), getActiveRoomFromDungeonMapPosition_hook, SYM(getActiveRoomFromDungeonMapPosition), (SYM(checkForUnderwaterTransition__levelDown) + 25));
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 25), (SYM(checkForUnderwaterTransition__levelDown) + 27)); C = 0x00;
  CYC((SYM(checkForUnderwaterTransition__levelDown) + 27), (SYM(checkForUnderwaterTransition__levelDown) + 29)); goto L_51cf;
L_51cf:
  CYC(SYM(checkForUnderwaterTransition__initializeWarp), (SYM(checkForUnderwaterTransition__initializeWarp) + 3)); mem_wr(gb, wWarpDestRoom, A);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 3), (SYM(checkForUnderwaterTransition__initializeWarp) + 6)); A = mem_rd(gb, wActiveGroup);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 6), (SYM(checkForUnderwaterTransition__initializeWarp) + 7)); alu_add(gb, C);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 7), (SYM(checkForUnderwaterTransition__initializeWarp) + 9)); alu_or(gb, 0x80);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 9), (SYM(checkForUnderwaterTransition__initializeWarp) + 12)); mem_wr(gb, wWarpDestGroup, A);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 12), (SYM(checkForUnderwaterTransition__initializeWarp) + 15)); A = mem_rd(gb, wActiveTilePos);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 15), (SYM(checkForUnderwaterTransition__initializeWarp) + 18)); mem_wr(gb, wWarpDestPos, A);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 18), (SYM(checkForUnderwaterTransition__initializeWarp) + 20)); A = 0x00;
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 20), (SYM(checkForUnderwaterTransition__initializeWarp) + 23)); mem_wr(gb, wWarpTransition, A);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 23), (SYM(checkForUnderwaterTransition__initializeWarp) + 25)); A = 0x03;
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 25), (SYM(checkForUnderwaterTransition__initializeWarp) + 28)); mem_wr(gb, wWarpTransition2, A);
  CYC((SYM(checkForUnderwaterTransition__initializeWarp) + 28), (SYM(checkForUnderwaterTransition__initializeWarp) + 29)); ret_effect(gb); return;
L_5810:
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+6); SET_HL(wLinkSwimmingState);
  CYC(b_+6, b_+8); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+8, b_+10); goto L_582c; } CYC(b_+8, b_+10);
  CYC(b_+10, b_+13); A = mem_rd(gb, wDisableScreenTransitions);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto L_582c; } CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+22); B = 0xcf;
  CYC(b_+22, b_+23); A = mem_rd(gb, BC);
  CYC(b_+23, b_+25); alu_cp(gb, 0xfc);
  if ((F & FZ)) { CYCT(b_+25, b_+28); goto L_51b2; } CYC(b_+25, b_+28);
L_582c:
  CYC(b_+28, b_+31); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+31, b_+33); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_5841; } CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = 0x3c;
  CALL_C(b_+37, cpActiveRing_hook, SYM(cpActiveRing), b_+40);
  if ((F & FZ)) { CYCT(b_+40, b_+41); ret_effect(gb); return; } CYC(b_+40, b_+41);
  CYC(b_+41, b_+43); E = 0x07;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); A = alu_dec8(gb, A);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  if ((F & FZ)) { CYCT(b_+46, b_+48); goto L_5845; } CYC(b_+46, b_+48);
  CYC(b_+48, b_+49); ret_effect(gb); return;
L_5841:
  CYC(b_+49, b_+51); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+51, b_+53); goto L_584c; } CYC(b_+51, b_+53);
L_5845:
  CYC(b_+53, b_+55); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+55, b_+57); A = 0x0b;
  CYC(b_+57, b_+60); TAIL(specialObjectSetAnimation);
L_584c:
  CYC(b_+60, b_+62); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+62, b_+64); E = 0x07;
  CYC(b_+64, b_+66); A = 0x78;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CALL_C(b_+67, linkCreateSplash_hook, SYM(linkCreateSplash), b_+70);
  CYC(b_+70, b_+72); A = 0x0c;
  CYC(b_+72, b_+75); TAIL(specialObjectSetAnimation);
}

void func_5933_hook(GB *gb) {
  BASE(func_5933);
  CYC(b_+0, b_+1); E = A;
  CYC(b_+1, b_+2); H = D;
  CYC(b_+2, b_+4); L = 0x09;
  CYC(b_+4, b_+6); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+6, b_+8); goto L_593d; } CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); mem_wr(gb, HL, E);
  CYC(b_+9, b_+10); ret_effect(gb); return;
L_593d:
  CYC(b_+10, b_+12); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto L_595b; } CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+19); alu_and(gb, 0x1f);
  CYC(b_+19, b_+21); alu_cp(gb, 0x09);
  if ((F & FC)) { CYCT(b_+21, b_+23); goto L_597c; } CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); alu_sub(gb, 0x10);
  CYC(b_+25, b_+27); alu_cp(gb, 0x09);
  if ((F & FC)) { CYCT(b_+27, b_+29); goto L_5967; } CYC(b_+27, b_+29);
  CYC(b_+29, b_+32); SET_BC(0x0100);
  CYC(b_+32, b_+34); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto L_598d; } CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); B = 0xff;
  CYC(b_+38, b_+40); goto L_598d;
L_595b:
  CYC(b_+40, b_+43); SET_BC(0x00fb);
  CYC(b_+43, b_+46); A = mem_rd(gb, wLinkInAir);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+47, b_+49); goto L_598d; } CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); C = B;
  CYC(b_+50, b_+52); goto L_598d;
L_5967:
  CYC(b_+52, b_+55); SET_BC(0x01fb);
  CYC(b_+55, b_+57); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(b_+57, b_+59); goto L_598d; } CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); B = 0xff;
  CYC(b_+61, b_+63); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+63, b_+65); goto L_598d; } CYC(b_+63, b_+65);
  CYC(b_+65, b_+66); A = E;
  CYC(b_+66, b_+68); alu_xor(gb, 0x10);
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);
  CYC(b_+69, b_+71); B = 0x00;
  CYC(b_+71, b_+73); goto L_598d;
L_597c:
  CYC(b_+73, b_+76); SET_BC(0xff05);
  CYC(b_+76, b_+78); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(b_+78, b_+80); goto L_598d; } CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); B = 0x01;
  CYC(b_+82, b_+84); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+84, b_+86); goto L_598d; } CYC(b_+84, b_+86);
  CYC(b_+86, b_+87); A = E;
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);
  CYC(b_+88, b_+90); B = 0x00;
L_598d:
  CYC(b_+90, b_+92); L = 0x12;
  CYC(b_+92, b_+93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+94, b_+95); alu_cp(gb, mem_rd(gb, HL));
  if ((F & FC)) { CYCT(b_+95, b_+96); ret_effect(gb); return; } CYC(b_+95, b_+96);
  CYC(b_+96, b_+97); L = alu_dec8(gb, L);
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x00);
  CYC(b_+99, b_+101); L = 0x09;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+103); alu_add(gb, B);
  CYC(b_+103, b_+105); alu_and(gb, 0x1f);
  CYC(b_+105, b_+106); mem_wr(gb, HL, A);
  CYC(b_+106, b_+108); L = 0x11;
  CYC(b_+108, b_+109); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+109, b_+110); B = A;
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+112); alu_add(gb, C);
  if ((F & FZ)) { CYCT(b_+112, b_+114); goto L_59af; } CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+116, b_+118); goto L_59af; } CYC(b_+116, b_+118);
  CYC(b_+118, b_+119); alu_cp(gb, B);
  if ((F & FC)) { CYCT(b_+119, b_+121); goto L_59ad; } CYC(b_+119, b_+121);
  CYC(b_+121, b_+122); A = B;
L_59ad:
  CYC(b_+122, b_+123); mem_wr(gb, HL, A);
  CYC(b_+123, b_+124); ret_effect(gb); return;
L_59af:
  CYC(b_+124, b_+126); L = 0x10;
  CYC(b_+126, b_+127); alu_xor(gb, A);
  CYC(b_+127, b_+128); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+128, b_+129); L = alu_inc8(gb, L);
  CYC(b_+129, b_+130); mem_wr(gb, HL, L);
  CYC(b_+130, b_+131); A = alu_dec8(gb, A);
  CYC(b_+131, b_+133); L = 0x09;
  CYC(b_+133, b_+134); mem_wr(gb, HL, A);
  CYC(b_+134, b_+135); ret_effect(gb); return;
}

void initLinkState_hook(GB *gb) {
  BASE(initLinkState);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x04;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x00);
  CYC(b_+8, b_+10); L = 0x35;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+13); ret_effect(gb); return;
}

void overworldSwimmingState3_hook(GB *gb) {
  BASE(overworldSwimmingState3);
  uint16_t sp0_ = gb->sp;
  goto L_5725;
L_56d4:
  CYC(SYM(overworldSwimmingState1__drown), (SYM(overworldSwimmingState1__drown) + 2)); C = 0x88;
  CYC((SYM(overworldSwimmingState1__drown) + 2), (SYM(overworldSwimmingState1__drown) + 4)); goto L_56da;
L_56da:
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 2), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 4)); A = 0x02;
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 4), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 7)); mem_wr(gb, wLinkForceState, A);
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 7), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 9)); A = 0x04;
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 9), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 12)); mem_wr(gb, wLinkStateParameter, A);
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 12), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 14)); A = 0x80;
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 14), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 17)); mem_wr(gb, wcc92, A);
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 17), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 18)); H = D;
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 18), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 20)); L = 0x2b;
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 20), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 21)); mem_wr(gb, HL, C);
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 21), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 23)); L = 0x24;
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 23), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 25)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 25), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 27)); A = 0x5f;
  CALL_C((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 27), playSound_b00_hook, SYM(playSound_b00), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 30));
  CYC((SYM(overworldSwimmingState1__drownWithLessInvincibility) + 30), (SYM(overworldSwimmingState1__drownWithLessInvincibility) + 32)); B = 0x0a;
  CYC(SYM(overworldSwimmingState1__splashAndSetAnimation), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 3)); SET_HL(wLinkSwimmingState);
  CYC((SYM(overworldSwimmingState1__splashAndSetAnimation) + 3), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 4)); A = mem_rd(gb, HL);
  CYC((SYM(overworldSwimmingState1__splashAndSetAnimation) + 4), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 6)); alu_and(gb, 0xf0);
  CYC((SYM(overworldSwimmingState1__splashAndSetAnimation) + 6), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 8)); alu_or(gb, 0x02);
  CYC((SYM(overworldSwimmingState1__splashAndSetAnimation) + 8), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 9)); mem_wr(gb, HL, A);
  CYC((SYM(overworldSwimmingState1__splashAndSetAnimation) + 9), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 10)); A = B;
  CALL_C((SYM(overworldSwimmingState1__splashAndSetAnimation) + 10), specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 13));
  CYC((SYM(overworldSwimmingState1__splashAndSetAnimation) + 13), (SYM(overworldSwimmingState1__splashAndSetAnimation) + 16)); TAIL(linkCreateSplash);
L_5725:
  CALL_C(b_+0, checkSwimmingOverSeawater_hook, SYM(checkSwimmingOverSeawater), b_+3);
  if ((F & FZ)) { CYCT(b_+3, b_+5); goto L_56d4; } CYC(b_+3, b_+5);
  CALL_C(b_+5, linkUpdateDiving_hook, SYM(linkUpdateDiving), b_+8);
  CALL_C(b_+8, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x24;
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+16, b_+19); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+19, b_+20); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto L_5740; } CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(b_+24, objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+27);
L_5740:
  CALL_C(b_+27, updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+30);
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = 0x2f;
  CYC(b_+33, b_+35); alu_bit(gb, 6, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+35, b_+37); goto L_5750; } CYC(b_+35, b_+37);
  CALL_C(b_+37, linkUpdateVelocity_mermaidSuit_hook, SYM(linkUpdateVelocity__mermaidSuit), b_+40);
  CYC(b_+40, b_+43); TAIL(specialObjectUpdatePosition);
L_5750:
  CALL_C(b_+43, linkUpdateFlippersSpeed_hook, SYM(linkUpdateFlippersSpeed), b_+46);
  CALL_C(b_+46, func_5933_hook, SYM(func_5933), b_+49);
  CYC(b_+49, b_+52); TAIL(specialObjectUpdatePosition);
}

void linkUpdateVelocity_mermaidSuit_hook(GB *gb) {
  BASE(linkUpdateVelocity);
  uint16_t sp0_ = gb->sp;
  CYC(b_+7, b_+9); C = 0x98;
  CALL_C(b_+9, updateLinkSpeed_withParam_hook, SYM(updateLinkSpeed_withParam), b_+12);
  CYC(b_+12, b_+15); A = mem_rd(gb, wActiveRing);
  CYC(b_+15, b_+17); alu_cp(gb, 0x15);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_5908; } CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); E = 0x11;
  CYC(b_+21, b_+23); A = 0x37;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
L_5908:
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+28); A = mem_rd(gb, wLinkImmobilized);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto L_5916; } CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+34, b_+36); alu_and(gb, 0xf0);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto L_5922; } CYC(b_+36, b_+38);
L_5916:
  CYC(b_+38, b_+40); L = 0x3e;
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+43); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+43, b_+45); goto L_592c; } CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); A = 0xff;
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+50); TAIL(func_5933);
L_5922:
  CYC(b_+50, b_+52); A = 0x87;
  CALL_C(b_+52, playSound_b00_hook, SYM(playSound_b00), b_+55);
  CYC(b_+55, b_+56); H = D;
  CYC(b_+56, b_+58); L = 0x3e;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x04);
L_592c:
  CYC(b_+60, b_+62); L = 0x12;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x14);
  CYC(b_+64, b_+67); A = mem_rd(gb, wLinkAngle);
  TAIL(func_5933);
}

void linkUpdateVelocity_hook(GB *gb) {
  BASE(linkUpdateVelocity);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto L_5930; } CYC(b_+5, b_+7);
  linkUpdateVelocity_mermaidSuit_hook(gb); return;
L_5930:
  CYC(b_+64, b_+67); A = mem_rd(gb, wLinkAngle);
  TAIL(func_5933);
}

void linkUpdateInAir_hook(GB *gb) {
  BASE(linkUpdateInAir);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkInAir);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+12) { goto L_5aff; }
    else if (jt_ == b_+25) { goto L_5b0c; }
    else if (jt_ == b_+71) { goto L_5b3a; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
L_5aff:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x0f;
  CYC(b_+15, b_+17); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+17, b_+18); ret_effect(gb); return; } CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); A = 0x02;
  CYC(b_+20, b_+23); mem_wr(gb, wLinkInAir, A);
  CYC(b_+23, b_+25); goto L_5b2d;
L_5b0c:
  CYC(b_+25, b_+28); SET_HL(wLinkInAir);
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+29, b_+31); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto L_5b28; } CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); SET_HL(wIsTileSlippery);
  CYC(b_+36, b_+38); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto L_5b28; } CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); L = GV(0x9c, 0xb6);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x00);
  CALL_C(b_+44, updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+47);
  CYC(b_+47, b_+50); A = mem_rd(gb, wLinkAngle);
  CYC(b_+50, b_+52); E = 0x09;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
L_5b28:
  CYC(b_+53, b_+55); A = 0x53;
  CALL_C(b_+55, playSound_b00_hook, SYM(playSound_b00), b_+58);
L_5b2d:
  CYC(b_+58, b_+61); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+61, b_+62); C = A;
  CYC(b_+62, b_+65); A = mem_rd(gb, wLinkTurningDisabled);
  CYC(b_+65, b_+66); alu_or(gb, C);
  CYC(b_+66, b_+68); A = 0x18;
  if ((F & FZ)) { CALL_C_CC(b_+68, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+71); } else CYC(b_+68, b_+71);
L_5b3a:
  CYC(b_+71, b_+72); alu_xor(gb, A);
  CYC(b_+72, b_+74); E = 0x12;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); E = alu_inc8(gb, E);
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CYC(b_+77, b_+80); SET_HL(wLinkInAir);
  CYC(b_+80, b_+82); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+82, b_+84); goto L_5b4a; } CYC(b_+82, b_+84);
  CYC(b_+84, b_+86); E = 0x33;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
L_5b4a:
  CYC(b_+87, b_+89); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(b_+89, b_+91); C = 0x20;
  if ((F & FZ)) { CYCT(b_+91, b_+93); goto L_5b52; } CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); C = 0x0a;
L_5b52:
  CALL_C(b_+95, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+98);
  CYC(b_+98, b_+100); L = 0x15;
  if ((F & FZ)) { CYCT(b_+100, b_+102); goto L_5b66; } CYC(b_+100, b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+105, b_+106); ret_effect(gb); return; } CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(b_+108, b_+109); ret_effect(gb); return; } CYC(b_+108, b_+109);
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x03);
  CYC(b_+111, b_+112); L = alu_dec8(gb, L);
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x00);
  CYC(b_+114, b_+115); ret_effect(gb); return;
L_5b66:
  CYC(b_+115, b_+116); alu_xor(gb, A);
  CYC(b_+116, b_+117); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+117, b_+118); mem_wr(gb, HL, A);
  CYC(b_+118, b_+121); mem_wr(gb, wLinkInAir, A);
  CYC(b_+121, b_+123); E = 0x36;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  CALL_C(b_+124, animateLinkStanding_hook, SYM(animateLinkStanding), b_+127);
  CALL_C(b_+127, specialObjectSetPositionToVar38IfSet_hook, SYM(specialObjectSetPositionToVar38IfSet), b_+130);
  CALL_C(b_+130, linkApplyTileTypes_hook, SYM(linkApplyTileTypes), b_+133);
  CYC(b_+133, b_+136); A = mem_rd(gb, wActiveTileType);
  CYC(b_+136, b_+137); A = alu_dec8(gb, A);
  CYC(b_+137, b_+139); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+139, b_+141); goto L_5b85; } CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); A = 0x04;
  CYC(b_+143, b_+146); mem_wr(gb, wStandingOnTileCounter, A);
L_5b85:
  CYC(b_+146, b_+148); A = 0xa3;
  CALL_C(b_+148, playSound_b00_hook, SYM(playSound_b00), b_+151);
  CALL_C(b_+151, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+154);
  CYC(b_+154, b_+157); TAIL(initLinkState);
}

void linkUpdateSwimming_hook(GB *gb) {
  BASE(linkUpdateSwimming);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+8); SET_HL(wcc95);
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == SYM(initLinkState) && hook_is(gb, SYM(initLinkState), initLinkState_hook)) { initLinkState_hook(gb); return; }
    else if (jt_ == SYM(overworldSwimmingState1) && hook_is(gb, SYM(overworldSwimmingState1), overworldSwimmingState1_hook)) { overworldSwimmingState1_hook(gb); return; }
    else if (jt_ == SYM(overworldSwimmingState2) && hook_is(gb, SYM(overworldSwimmingState2), overworldSwimmingState2_hook)) { overworldSwimmingState2_hook(gb); return; }
    else if (jt_ == SYM(overworldSwimmingState3) && hook_is(gb, SYM(overworldSwimmingState3), overworldSwimmingState3_hook)) { overworldSwimmingState3_hook(gb); return; }
    else if (jt_ == SYM(linkUpdateDrowning) && hook_is(gb, SYM(linkUpdateDrowning), linkUpdateDrowning_hook)) { linkUpdateDrowning_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void linkUpdateFlippersSpeed_hook(GB *gb) {
  BASE(linkUpdateFlippersSpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x35;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+10) { goto L_57aa; }
    else if (jt_ == b_+53) { goto L_57d5; }
    else if (jt_ == b_+58) { goto L_57da; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
L_57aa:
  CYC(b_+10, b_+13); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+13, b_+15); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_57b8; } CYC(b_+15, b_+17);
  CALL_C(b_+17, linkSetSwimmingSpeedTmp_hook, SYM(linkSetSwimmingSpeedTmp), b_+20);
  CYC(b_+20, b_+23); A = mem_rd(gb, wLinkAngle);
  CYC(b_+23, b_+24); ret_effect(gb); return;
L_57b8:
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); A = 0x08;
L_57bd:
  CYC(b_+29, b_+30); push_effect(gb, AF);
  CYC(b_+30, b_+32); E = 0x08;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); alu_add(gb, A);
  CYC(b_+34, b_+35); alu_add(gb, A);
  CYC(b_+35, b_+36); alu_add(gb, A);
  CALL_C(b_+36, func_5933_hook, SYM(func_5933), b_+39);
  CYC(b_+39, b_+40); SET_AF(pop_effect(gb));
  CYC(b_+40, b_+41); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto L_57bd; } CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); E = 0x06;
  CYC(b_+45, b_+47); A = 0x0d;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); A = 0x88;
  CALL_C(b_+50, playSound_b00_hook, SYM(playSound_b00), b_+53);
L_57d5:
  CYC(b_+53, b_+56); SET_BC(0x0105);
  CYC(b_+56, b_+58); goto L_57dd;
L_57da:
  CYC(b_+58, b_+61); SET_BC((hSoundData3 + 9));
L_57dd:
  CALL_C(b_+61, itemDecCounter1_hook, SYM(itemDecCounter1), b_+64);
  if ((F & FZ)) { CYCT(b_+64, b_+66); goto L_57e9; } CYC(b_+64, b_+66);
  CYC(b_+66, b_+67); A = mem_rd(gb, HL);
  CYC(b_+67, b_+69); alu_and(gb, 0x03);
  if ((F & FZ)) { CYCT(b_+69, b_+71); goto L_57f9; } CYC(b_+69, b_+71);
  CYC(b_+71, b_+73); goto L_5803;
L_57e9:
  CYC(b_+73, b_+75); L = 0x35;
  CYC(b_+75, b_+76); B = alu_inc8(gb, B);
  CYC(b_+76, b_+77); mem_wr(gb, HL, B);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto L_57f4; } CYC(b_+77, b_+79);
  CALL_C(b_+79, linkSetSwimmingSpeed_hook, SYM(linkSetSwimmingSpeed), b_+82);
  CYC(b_+82, b_+84); goto L_5803;
L_57f4:
  CYC(b_+84, b_+86); L = 0x06;
  CYC(b_+86, b_+88); A = 0x0c;
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);
L_57f9:
  CYC(b_+89, b_+91); L = 0x11;
  CYC(b_+91, b_+92); A = mem_rd(gb, HL);
  CYC(b_+92, b_+93); alu_add(gb, C);
  CYC(b_+93, b_+95); alu_bit(gb, 7, A);
  if ((F & FZ)) { CYCT(b_+95, b_+97); goto L_5802; } CYC(b_+95, b_+97);
  CYC(b_+97, b_+98); alu_xor(gb, A);
L_5802:
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
L_5803:
  CYC(b_+99, b_+102); A = mem_rd(gb, wLinkAngle);
  CYC(b_+102, b_+104); alu_bit(gb, 7, A);
  if ((F & FZ)) { CYCT(b_+104, b_+105); ret_effect(gb); return; } CYC(b_+104, b_+105);
  CYC(b_+105, b_+107); E = 0x08;
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+110); A = alu_swap(gb, A);
  CYC(b_+110, b_+111); alu_rrca(gb);
  CYC(b_+111, b_+112); ret_effect(gb); return;
}

// checkLinkJumpingOffCliff@checkCliffTile: hl points at a (y,x) offset pair; carry set when the
// tile there is a cliff facing Link's direction.
static void link_check_cliff_tile(GB *gb) {
  BASE(checkLinkJumpingOffCliff__checkCliffTile);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  SET_HL(HL + 1);
  CYC(b_+0, b_+1);
  CYC(b_+1, b_+2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+2, b_+3); C = mem_rd(gb, HL);
  B = A;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  CALL_C(b_+5, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+8);
  CYC(b_+8, b_+10); H8(hFF8B) = A;
  SET_HL(SYM(cliffTilesTable));
  CYC(b_+10, b_+13);
  CALL_C(b_+13, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+16);
  CYC(b_+16, b_+17); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(b_+17, b_+18); return; }
  CYC(b_+17, b_+18);
  C = A;
  E = 0x09;
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  alu_cp(gb, C);
  alu_scf(gb);
  CYC(b_+22, b_+24);
  if (F & FZ) { CYCT(b_+24, b_+25); return; }
  CYC(b_+24, b_+25);
  alu_xor(gb, A);
  CYC(b_+25, b_+27);
}

void checkLinkJumpingOffCliff_hook(GB *gb) {
  BASE(checkLinkJumpingOffCliff);
  uint16_t sp0_ = gb->sp;
  if (game_seasons) {       // no cliff jumps from a hole tile
    CYC(b_+S(0), b_+S(3)); A = W8(wActiveTileType);
    CYC(b_+S(3), b_+S(5)); alu_cp(gb, 0x08);
    if (F & FZ) { CYCT(b_+S(5), b_+S(6)); ret_effect(gb); return; }
    CYC(b_+S(5), b_+S(6));
  }
  CYC(b_+O(0), b_+OE(3)); A = mem_rd(gb, wLinkAngle);
  CYC(b_+O(3), b_+OE(4)); C = A;
  CYC(b_+O(4), b_+OE(6)); alu_and(gb, 0xe7);
  if (!(F & FZ)) { CYCT(b_+O(6), b_+OE(7)); ret_effect(gb); return; } CYC(b_+O(6), b_+OE(7));
  CYC(b_+O(7), b_+OE(8)); H = D;
  CYC(b_+O(8), b_+OE(10)); L = 0x09;
  CYC(b_+O(10), b_+OE(11)); alu_xor(gb, C);
  CYC(b_+O(11), b_+OE(12)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(12), b_+OE(13)); ret_effect(gb); return; } CYC(b_+O(12), b_+OE(13));
  CYC(b_+O(13), b_+OE(14)); alu_add(gb, A);
  CYC(b_+O(14), b_+OE(16)); A = alu_swap(gb, A);
  CYC(b_+O(16), b_+OE(17)); C = A;
  CYC(b_+O(17), b_+OE(18)); alu_add(gb, A);
  CYC(b_+O(18), b_+OE(19)); alu_add(gb, A);
  CYC(b_+O(19), b_+OE(20)); alu_add(gb, C);
  CYC(b_+O(20), b_+OE(23)); SET_HL(b_+O(93));
  CYC(b_+O(23), b_+OE(24)); link_add_a_to_hl(gb, b_+O(24));
  CYC(b_+O(24), b_+OE(26)); E = 0x33;
  CYC(b_+O(26), b_+OE(27)); A = mem_rd(gb, DE);
  CYC(b_+O(27), b_+OE(28)); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+O(28), b_+OE(29)); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+O(29), b_+OE(30)); ret_effect(gb); return; } CYC(b_+O(29), b_+OE(30));
  CYC(b_+O(30), b_+OE(33));
  link_check_cliff_tile(gb);
  if (!(F & FC)) { CYCT(b_+O(33), b_+OE(34)); ret_effect(gb); return; } CYC(b_+O(33), b_+OE(34));
  CYC(b_+O(34), b_+OE(37));
  link_check_cliff_tile(gb);
  if (!(F & FC)) { CYCT(b_+O(37), b_+OE(38)); ret_effect(gb); return; } CYC(b_+O(37), b_+OE(38));
  CYC(b_+O(38), b_+OE(40)); A = 0x81;
  CYC(b_+O(40), b_+OE(43)); mem_wr(gb, wLinkInAir, A);
  CYC(b_+O(43), b_+OE(46)); SET_BC(0xfe40);
  CALL_C(b_+O(46), objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+OE(49));
  CYC(b_+O(49), b_+OE(51)); L = 0x2d;
  CYC(b_+O(51), b_+OE(53)); mem_wr(gb, HL, 0x00);
  if (game_seasons) {       // jumping off a cliff onto a tree stump: land at once
    CYC(b_+S(59), b_+S(61)); A = H8(hFF8B);
    CYC(b_+S(61), b_+S(63)); alu_cp(gb, 0x05);
    if (F & FZ) { CYCT(b_+S(63), b_+S(65)); goto stump; }
    CYC(b_+S(63), b_+S(65));
    CYC(b_+S(65), b_+S(67)); alu_cp(gb, 0x06);
    if (F & FZ) {
      CYCT(b_+S(67), b_+S(69));
stump:
      CYC(b_+S(77), b_+S(79)); L = 0x10;
      CYC(b_+S(79), b_+S(81)); mem_wr(gb, HL, 0x32);
      RET(b_+S(81)); return;
    }
    CYC(b_+S(67), b_+S(69));
  }
  CYC(b_+O(53), b_+OE(54)); SET_HL(pop_effect(gb));
  CYC(b_+O(54), b_+OE(56)); A = 0x12;
  CALL_C(b_+O(56), linkSetState_hook, SYM(linkSetState), b_+OE(59));
  CYC(b_+O(59), b_+OE(61)); TAIL(linkState12);
}

void specialObjectSetPositionToVar38IfSet_hook(GB *gb) {
  BASE(specialObjectSetPositionToVar38IfSet);
  CYC(b_+0, b_+2); E = 0x37;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL((SYM(specialObjectSetAngleRelativeToVar38) + 29));
  CYC(b_+8, b_+9); link_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); alu_and(gb, 0xf0);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+17); E = 0x0b;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+20); A = C;
  CYC(b_+20, b_+22); alu_and(gb, 0x0f);
  CYC(b_+22, b_+24); A = alu_swap(gb, A);
  CYC(b_+24, b_+25); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+25, b_+27); E = 0x0d;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); TAIL(clearVar37AndVar38);
}

void initLinkStateAndAnimateStanding_hook(GB *gb) {
  BASE(initLinkStateAndAnimateStanding);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, initLinkState_hook, SYM(initLinkState), b_+3);
  CYC(b_+3, b_+5); L = 0x1a;
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  TAIL(animateLinkStanding);
}

void specialObjectUpdatePositionWithoutTileEdgeAdjust_hook(GB *gb) {
  BASE(specialObjectUpdatePositionWithoutTileEdgeAdjust);
  CYC(b_+0, b_+2); E = 0x10;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); E = 0x09;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9);
  link_special_object_update_position_continuation_hook(gb);
}

void link_special_object_update_position_continuation_hook(GB *gb) {
  BASE(specialObjectUpdatePositionGivenVelocity);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+16); E = 0x00;
  link_special_object_update_position_after_collision_hook(gb);
}

void link_special_object_update_position_after_collision_hook(GB *gb) {
  BASE(specialObjectUpdatePositionGivenVelocity);
  uint16_t sp0_ = gb->sp;
  CYC(b_+16, b_+17); A = C;
  CYC(b_+17, b_+20); SET_HL(specialObjectVelocityCollisionBits_bank05);
  CYC(b_+20, b_+21); link_add_a_to_hl(gb, b_+21);
  CYC(b_+21, b_+22); A = E;
  CYC(b_+22, b_+23); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); E = A;
  CALL_C(b_+24, getPositionOffsetForVelocity_hook, SYM(getPositionOffsetForVelocity), b_+27);
  CYC(b_+27, b_+29); C = 0x00;
  CYC(b_+29, b_+30); B = E;
  CYC(b_+30, b_+31); A = B;
  CYC(b_+31, b_+33); alu_and(gb, 0xf0);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_5dd2; } CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); alu_or(gb, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+37, b_+39); goto L_5dd2; } CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); L = alu_dec8(gb, L);
  CYC(b_+40, b_+42); E = 0x0a;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+46); E = alu_inc8(gb, E);
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+51); C = alu_inc8(gb, C);
L_5dd2:
  CYC(b_+51, b_+52); A = B;
  CYC(b_+52, b_+54); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto L_5de9; } CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); L = (uint8_t)(wTmpcec0 + 3);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+59, b_+60); alu_or(gb, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+60, b_+62); goto L_5de9; } CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); E = 0x0c;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+68); L = alu_inc8(gb, L);
  CYC(b_+68, b_+69); E = alu_inc8(gb, E);
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+74); C = (uint8_t)(C | (1 << 1));
L_5de9:
  CYC(b_+74, b_+75); A = C;
  CYC(b_+75, b_+76); alu_or(gb, A);
  CYC(b_+76, b_+77); ret_effect(gb);
}

void checkPositionSurroundedByWalls_hook(GB *gb) {
  BASE(checkPositionSurroundedByWalls);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, calculateAdjacentWallsBitset_hook, SYM(calculateAdjacentWallsBitset), b_+3);
L_5e95:
  CYC(b_+3, b_+5); B = 0x80;
  CYC(b_+5, b_+7); alu_cp(gb, 0xff);
  if ((F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; } CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); alu_rra(gb);
  CYC(b_+9, b_+11); B = alu_rl(gb, B);
  CYC(b_+11, b_+12); alu_rra(gb);
  CYC(b_+12, b_+14); B = alu_rl(gb, B);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto L_5e95; } CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); ret_effect(gb); return;
}

void clearLinkImmobilizedBit4_hook(GB *gb) {
  BASE(clearLinkImmobilizedBit4);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); SET_HL(wLinkImmobilized);
  CYC(b_+4, b_+6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb));
  CYC(b_+7, b_+8); ret_effect(gb); return;
}

void setLinkImmobilizedBit4_hook(GB *gb) {
  BASE(setLinkImmobilizedBit4);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); SET_HL(wLinkImmobilized);
  CYC(b_+4, b_+6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4)));
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb));
  CYC(b_+7, b_+8); ret_effect(gb); return;
}

void linkPullIntoHole_hook(GB *gb) {
  BASE(linkPullIntoHole);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = 0x2d;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = 0x04;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0x02);
  if ((F & FZ)) { CYCT(b_+10, b_+11); ret_effect(gb); return; } CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wStandingOnTileCounter);
  CYC(b_+14, b_+16); alu_cp(gb, 0x10);
  if (!(F & FC)) { CALL_C_CC(b_+16, setLinkImmobilizedBit4_hook, SYM(setLinkImmobilizedBit4), b_+19); } else CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); alu_and(gb, 0x03);
  if ((F & FZ)) { CYCT(b_+21, b_+23); goto L_5f66; } CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = alu_dec8(gb, A);
  if ((F & FZ)) { CYCT(b_+24, b_+26); goto L_5f74; } CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); ret_effect(gb); return;
L_5f66:
  CYC(b_+27, b_+29); L = 0x0b;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_add(gb, 0x05);
  CYC(b_+32, b_+34); alu_and(gb, 0xf0);
  CYC(b_+34, b_+36); alu_add(gb, 0x08);
  CYC(b_+36, b_+37); alu_sub(gb, mem_rd(gb, HL));
  if ((F & FC)) { CYCT(b_+37, b_+39); goto L_5f82; } CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); goto L_5f7e;
L_5f74:
  CYC(b_+41, b_+43); L = 0x0d;
  CYC(b_+43, b_+44); A = mem_rd(gb, HL);
  CYC(b_+44, b_+46); alu_and(gb, 0xf0);
  CYC(b_+46, b_+48); alu_add(gb, 0x08);
  CYC(b_+48, b_+49); alu_sub(gb, mem_rd(gb, HL));
  if ((F & FC)) { CYCT(b_+49, b_+51); goto L_5f82; } CYC(b_+49, b_+51);
L_5f7e:
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+53); A = alu_inc8(gb, A);
  CYC(b_+53, b_+55); goto L_5f84;
L_5f82:
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); A = alu_dec8(gb, A);
L_5f84:
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); L = 0x0b;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+61, b_+63); alu_and(gb, 0x0f);
  CYC(b_+63, b_+65); alu_sub(gb, 0x07);
  CYC(b_+65, b_+67); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+67, b_+68); ret_effect(gb); return; } CYC(b_+67, b_+68);
  CYC(b_+68, b_+69); L = alu_inc8(gb, L);
  CYC(b_+69, b_+70); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+70, b_+72); alu_and(gb, 0x0f);
  CYC(b_+72, b_+74); alu_sub(gb, 0x07);
  CYC(b_+74, b_+76); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+76, b_+77); ret_effect(gb); return; } CYC(b_+76, b_+77);
  CALL_C(b_+77, clearAllParentItems_hook, SYM(clearAllParentItems), b_+80);
  CYC(b_+80, b_+82); E = 0x2d;
  CYC(b_+82, b_+83); alu_xor(gb, A);
  CYC(b_+83, b_+84); mem_wr(gb, DE, A);
  CYC(b_+84, b_+87); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+87, b_+89); E = 0x01;
  CYC(b_+89, b_+90); A = mem_rd(gb, DE);
  CYC(b_+90, b_+91); alu_or(gb, A);
  CYC(b_+91, b_+93); A = 0x02;
  if ((F & FZ)) { CYCT(b_+93, b_+96); linkSetState_hook(gb); return; } CYC(b_+93, b_+96);
  CYC(b_+96, b_+99); mem_wr(gb, wLinkForceState, A);
  CYC(b_+99, b_+100); ret_effect(gb); return;
}

void clearVar37AndVar38_hook(GB *gb) {
  BASE(clearVar37AndVar38);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = 0x37;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+7); ret_effect(gb); return;
}

void specialObjectSetVar37AndVar38_hook(GB *gb) {
  BASE(specialObjectSetVar37AndVar38);
  CYC(b_+0, b_+2); E = 0x37;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+4); E = alu_inc8(gb, E);
  CYC(b_+4, b_+5); A = L;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  TAIL(specialObjectSetAngleRelativeToVar38);
}

void specialObjectSetAngleRelativeToVar38_hook(GB *gb) {
  BASE(specialObjectSetAngleRelativeToVar38);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x37;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); SET_HL(b_+29);
  CYC(b_+8, b_+9); link_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); E = alu_inc8(gb, E);
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); alu_and(gb, 0xf0);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+17); SET_HL(HL + 1);
  CYC(b_+17, b_+18); A = C;
  CYC(b_+18, b_+20); alu_and(gb, 0x0f);
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); C = A;
  CALL_C(b_+24, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+27);
  CYC(b_+27, b_+29); E = 0x09;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); ret_effect(gb); return;
}

void linkUpdateSwimming_sidescroll_hook(GB *gb) {
  BASE(linkUpdateSwimming_sidescroll);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto L_5870; } CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(wcc95);
  CYC(b_+10, b_+12); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+21) { goto L_5870; }
    else if (jt_ == b_+24) { goto L_5873; }
    else if (jt_ == b_+69) { goto L_58a0; }
    else if (jt_ == SYM(linkUpdateDrowning) && hook_is(gb, SYM(linkUpdateDrowning), linkUpdateDrowning_hook)) { linkUpdateDrowning_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
L_5870:
  CYC(b_+21, b_+24); TAIL(initLinkState);
L_5873:
  CALL_C(b_+24, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+27);
  CYC(b_+27, b_+30); SET_HL(wLinkSwimmingState);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+31, linkSetSwimmingSpeed_hook, SYM(linkSetSwimmingSpeed), b_+34);
  CALL_C(b_+34, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+37);
  CYC(b_+37, b_+39); A = 0x2e;
  CALL_C(b_+39, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+42);
  if (!(F & FC)) { CYCT(b_+42, b_+44); goto L_5896; } CYC(b_+42, b_+44);
  CYC(b_+44, b_+47); SET_HL(w1Link_var2f);
  CYC(b_+47, b_+49); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(b_+49, b_+51); A = 0x17;
  if ((F & FZ)) { CYCT(b_+51, b_+53); goto L_589d; } CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+55, b_+57); A = 0x11;
  CYC(b_+57, b_+59); goto L_589d;
L_5896:
  CYC(b_+59, b_+61); A = 0x03;
  CYC(b_+61, b_+64); mem_wr(gb, wLinkSwimmingState, A);
  CYC(b_+64, b_+66); A = 0x0a;
L_589d:
  CYC(b_+66, b_+69); TAIL(specialObjectSetAnimation);
L_58a0:
  CYC(b_+69, b_+70); alu_xor(gb, A);
  CYC(b_+70, b_+73); mem_wr(gb, wLinkInAir, A);
  CYC(b_+73, b_+74); H = D;
  CYC(b_+74, b_+76); L = 0x24;
  CYC(b_+76, b_+78); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+78, b_+81); A = mem_rd(gb, wLinkImmobilized);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+82, b_+84); goto L_58d8; } CYC(b_+82, b_+84);
  CYC(b_+84, b_+86); L = 0x08;
  CYC(b_+86, b_+89); A = mem_rd(gb, wLinkAngle);
  CYC(b_+89, b_+90); alu_add(gb, A);
  if ((F & FC)) { CYCT(b_+90, b_+92); goto L_58c2; } CYC(b_+90, b_+92);
  CYC(b_+92, b_+93); C = A;
  CYC(b_+93, b_+95); alu_and(gb, 0x18);
  if ((F & FZ)) { CYCT(b_+95, b_+97); goto L_58c2; } CYC(b_+95, b_+97);
  CYC(b_+97, b_+98); A = C;
  CYC(b_+98, b_+100); A = alu_swap(gb, A);
  CYC(b_+100, b_+102); alu_and(gb, 0x03);
  CYC(b_+102, b_+103); mem_wr(gb, HL, A);
L_58c2:
  CYC(b_+103, b_+105); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(b_+105, b_+107); L = 0x2f;
  CYC(b_+107, b_+109); alu_bit(gb, 6, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+109, b_+111); goto L_58cf; } CYC(b_+109, b_+111);
  CALL_C(b_+111, linkUpdateVelocity_mermaidSuit_hook, SYM(linkUpdateVelocity__mermaidSuit), b_+114);
  CYC(b_+114, b_+116); goto L_58d5;
L_58cf:
  CALL_C(b_+116, linkUpdateFlippersSpeed_hook, SYM(linkUpdateFlippersSpeed), b_+119);
  CALL_C(b_+119, func_5933_hook, SYM(func_5933), b_+122);
L_58d5:
  CALL_C(b_+122, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+125);
L_58d8:
  CYC(b_+125, b_+126); H = D;
  CYC(b_+126, b_+128); L = 0x07;
  CYC(b_+128, b_+129); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+129, b_+131); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+131, b_+133); goto L_58ed; } CYC(b_+131, b_+133);
  CALL_C(b_+133, getRandomNumber_hook, SYM(getRandomNumber), b_+136);
  CYC(b_+136, b_+138); alu_and(gb, 0x1f);
  CYC(b_+138, b_+140); alu_add(gb, 0x32);
  CYC(b_+140, b_+141); mem_wr(gb, HL, A);
  CYC(b_+141, b_+143); B = 0x91;
  CALL_C(b_+143, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+146);
L_58ed:
  CYC(b_+146, b_+149); TAIL(specialObjectAnimate);
}

void linkState01_sidescroll_hook(GB *gb) {
  BASE(linkState01_sidescroll);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, sidescrollUpdateActiveTile_hook, SYM(sidescrollUpdateActiveTile), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wActiveTileType);
  CYC(b_+6, b_+8); alu_bit(gb, 5, A);
  if ((F & FZ)) { CYCT(b_+8, b_+10); goto L_59dc; } CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = 0x2f;
  CYC(b_+13, b_+15); alu_bit(gb, 6, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+15, b_+17); goto L_59cd; } CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
L_59cd:
  CYC(b_+19, b_+22); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_5a00; } CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
  CYC(b_+26, b_+29); mem_wr(gb, wLinkSwimmingState, A);
  CALL_C(b_+29, linkCreateSplash_hook, SYM(linkCreateSplash), b_+32);
  CYC(b_+32, b_+34); goto L_5a00;
L_59dc:
  CYC(b_+34, b_+37); SET_HL(wLinkSwimmingState);
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x00);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+41, b_+43); goto L_5a00; } CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); A = mem_rd(gb, wLastActiveTileType);
  CYC(b_+46, b_+48); alu_cp(gb, 0x30);
  if ((F & FZ)) { CYCT(b_+48, b_+50); goto L_5a00; } CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); A = 0x02;
  CYC(b_+52, b_+55); mem_wr(gb, wLinkInAir, A);
  CALL_C(b_+55, linkCreateSplash_hook, SYM(linkCreateSplash), b_+58);
  CYC(b_+58, b_+61); SET_BC(0xfe60);
  CALL_C(b_+61, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+64);
  CYC(b_+64, b_+67); A = mem_rd(gb, wLinkAngle);
  CYC(b_+67, b_+69); L = 0x09;
  CYC(b_+69, b_+70); mem_wr(gb, HL, A);
L_5a00:
  CALL_C(b_+70, checkUseItems_b00_hook, SYM(checkUseItems_b00), b_+73);
  CYC(b_+73, b_+76); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+77, b_+78); ret_effect(gb); return; } CYC(b_+77, b_+78);
  CALL_C(b_+78, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+81);
  CALL_C(b_+81, linkUpdateKnockback_hook, SYM(linkUpdateKnockback), b_+84);
  CYC(b_+84, b_+87); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+87, b_+88); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+88, b_+91); linkUpdateSwimming_sidescroll_hook(gb); return; } CYC(b_+88, b_+91);
  CYC(b_+91, b_+94); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+94, b_+96); alu_bit(gb, 6, A);
  if ((F & FZ)) { CYCT(b_+96, b_+99); goto L_5a24; } CYC(b_+96, b_+99);
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CYC(b_+100, b_+103); mem_wr(gb, wLinkInAir, A);
  CYC(b_+103, b_+106); TAIL(animateLinkStanding);
L_5a24:
  CALL_C(b_+106, linkUpdateInAir_sidescroll_hook, SYM(linkUpdateInAir_sidescroll), b_+109);
  if ((F & FZ)) { CYCT(b_+109, b_+110); ret_effect(gb); return; } CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); E = 0x2d;
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+114); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+114, b_+115); ret_effect(gb); return; } CYC(b_+114, b_+115);
  CYC(b_+115, b_+118); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+118, b_+120); alu_cp(gb, 0x02);
  if ((F & FZ)) { CALL_C_CC(b_+120, dealSpikeDamageToLink_hook, SYM(dealSpikeDamageToLink), b_+123); } else CYC(b_+120, b_+123);
  CYC(b_+123, b_+126); A = mem_rd(gb, wForceIcePhysics);
  CYC(b_+126, b_+127); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+127, b_+129); goto L_5a42; } CYC(b_+127, b_+129);
  CYC(b_+129, b_+131); E = 0x33;
  CYC(b_+131, b_+132); A = mem_rd(gb, DE);
  CYC(b_+132, b_+134); alu_and(gb, 0x30);
  if (!(F & FZ)) { CYCT(b_+134, b_+136); goto L_5a49; } CYC(b_+134, b_+136);
L_5a42:
  CYC(b_+136, b_+139); A = mem_rd(gb, wLastActiveTileType);
  CYC(b_+139, b_+141); alu_cp(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+141, b_+143); goto L_5a67; } CYC(b_+141, b_+143);
L_5a49:
  CYC(b_+143, b_+145); A = 0x21;
  CALL_C(b_+145, cpActiveRing_hook, SYM(cpActiveRing), b_+148);
  if ((F & FZ)) { CYCT(b_+148, b_+150); goto L_5a67; } CYC(b_+148, b_+150);
  CYC(b_+150, b_+152); C = 0x88;
  CALL_C(b_+152, updateLinkSpeed_withParam_hook, SYM(updateLinkSpeed_withParam), b_+155);
  CYC(b_+155, b_+157); A = 0x06;
  CYC(b_+157, b_+160); mem_wr(gb, wForceIcePhysics, A);
  CALL_C(b_+160, linkUpdateVelocity_hook, SYM(linkUpdateVelocity), b_+163);
  CYC(b_+163, b_+165); C = 0x02;
  CYC(b_+165, b_+168); A = mem_rd(gb, wLinkAngle);
  CYC(b_+168, b_+169); alu_rlca(gb);
  if ((F & FC)) { CYCT(b_+169, b_+171); goto L_5a82; } CYC(b_+169, b_+171);
  CYC(b_+171, b_+173); goto L_5a80;
L_5a67:
  CYC(b_+173, b_+174); alu_xor(gb, A);
  CYC(b_+174, b_+177); mem_wr(gb, wForceIcePhysics, A);
  CYC(b_+177, b_+178); C = A;
  CYC(b_+178, b_+181); A = mem_rd(gb, wLinkAngle);
  CYC(b_+181, b_+183); E = 0x09;
  CYC(b_+183, b_+184); mem_wr(gb, DE, A);
  CYC(b_+184, b_+185); alu_rlca(gb);
  if ((F & FC)) { CYCT(b_+185, b_+187); goto L_5a82; } CYC(b_+185, b_+187);
  CALL_C(b_+187, updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+190);
  CYC(b_+190, b_+192); C = 0x01;
  CYC(b_+192, b_+195); A = mem_rd(gb, wLinkImmobilized);
  CYC(b_+195, b_+196); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+196, b_+198); goto L_5a82; } CYC(b_+196, b_+198);
L_5a80:
  CYC(b_+198, b_+200); C = 0x07;
L_5a82:
  CYC(b_+200, b_+203); SET_HL(wActiveTileType);
  CYC(b_+203, b_+204); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+204, b_+205); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+205, b_+207); alu_and(gb, 0xbf);
  if ((F & FZ)) { CALL_C_CC(b_+207, linkAdjustAngleInSidescrollingArea_hook, SYM(linkAdjustAngleInSidescrollingArea), b_+210); } else CYC(b_+207, b_+210);
  CALL_C(b_+210, linkUpdateMovement_hook, SYM(linkUpdateMovement), b_+213);
  CYC(b_+213, b_+215); E = 0x09;
  CYC(b_+215, b_+216); A = mem_rd(gb, DE);
  CYC(b_+216, b_+218); alu_add(gb, 0x04);
  CYC(b_+218, b_+220); alu_and(gb, 0x1f);
  CYC(b_+220, b_+222); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+222, b_+224); goto L_5ab5; } CYC(b_+222, b_+224);
  CYC(b_+224, b_+227); SET_HL(wActiveTileType);
  CYC(b_+227, b_+228); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+228, b_+229); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+229, b_+231); goto L_5ab5; } CYC(b_+229, b_+231);
  CYC(b_+231, b_+232); A = mem_rd(gb, HL);
  CYC(b_+232, b_+234); alu_cp(gb, 0x90);
  if (!(F & FZ)) { CYCT(b_+234, b_+236); goto L_5ab5; } CYC(b_+234, b_+236);
  CYC(b_+236, b_+238); E = 0x0b;
  CYC(b_+238, b_+239); A = mem_rd(gb, DE);
  CYC(b_+239, b_+241); alu_and(gb, 0x0f);
  CYC(b_+241, b_+243); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+243, b_+245); goto L_5ab5; } CYC(b_+243, b_+245);
  CYC(b_+245, b_+246); A = mem_rd(gb, DE);
  CYC(b_+246, b_+248); alu_and(gb, 0xf0);
  CYC(b_+248, b_+250); alu_add(gb, 0x09);
  CYC(b_+250, b_+251); mem_wr(gb, DE, A);
L_5ab5:
  CYC(b_+251, b_+253); E = 0x33;
  CYC(b_+253, b_+254); A = mem_rd(gb, DE);
  CYC(b_+254, b_+256); alu_and(gb, 0x30);
  if (!(F & FZ)) { CYCT(b_+256, b_+258); goto L_5ac8; } CYC(b_+256, b_+258);
  CYC(b_+258, b_+261); A = mem_rd(gb, wActiveTileType);
  CYC(b_+261, b_+263); alu_bit(gb, 4, A);
  if ((F & FZ)) { CYCT(b_+263, b_+265); goto L_5ac8; } CYC(b_+263, b_+265);
  CYC(b_+265, b_+267); A = 0x01;
  CYC(b_+267, b_+270); mem_wr(gb, wLinkClimbingVine, A);
L_5ac8:
  CYC(b_+270, b_+273); A = mem_rd(gb, wLinkTurningDisabled);
  CYC(b_+273, b_+274); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+274, b_+275); ret_effect(gb); return; } CYC(b_+274, b_+275);
  CYC(b_+275, b_+278); TAIL(updateLinkDirectionFromAngle);
}

void linkUpdateInAir_sidescroll_hook(GB *gb) {
  BASE(linkUpdateInAir_sidescroll);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkInAir);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+12) { goto L_5b9c; }
    else if (jt_ == b_+39) { goto L_5bb7; }
    else if (jt_ == b_+65) { goto L_5bd1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
L_5b9c:
  CYC(b_+12, b_+15); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+17); ret_effect(gb); return; } CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); E = 0x33;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_and(gb, 0x30);
  if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; } CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); SET_HL(wActiveTileType);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+28, b_+30); alu_bit(gb, 4, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+31); ret_effect(gb); return; } CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); H = D;
  CYC(b_+32, b_+34); L = 0x14;
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+37, b_+39); goto L_5bbc;
L_5bb7:
  CYC(b_+39, b_+41); A = 0x53;
  CALL_C(b_+41, playSound_b00_hook, SYM(playSound_b00), b_+44);
L_5bbc:
  CYC(b_+44, b_+47); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+47, b_+48); C = A;
  CYC(b_+48, b_+51); A = mem_rd(gb, wLinkTurningDisabled);
  CYC(b_+51, b_+52); alu_or(gb, C);
  CYC(b_+52, b_+54); A = 0x18;
  if ((F & FZ)) { CALL_C_CC(b_+54, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+57); } else CYC(b_+54, b_+57);
  CYC(b_+57, b_+59); A = 0x02;
  CYC(b_+59, b_+62); mem_wr(gb, wLinkInAir, A);
  CALL_C(b_+62, updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+65);
L_5bd1:
  CYC(b_+65, b_+66); H = D;
  CYC(b_+66, b_+68); L = 0x15;
  CYC(b_+68, b_+70); alu_bit(gb, 7, mem_rd(gb, HL));
  if ((F & FZ)) { CYCT(b_+70, b_+72); goto L_5be1; } CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); E = 0x33;
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+77); alu_and(gb, 0xc0);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto L_5c34; } CYC(b_+77, b_+79);
  CYC(b_+79, b_+81); goto L_5c29;
L_5be1:
  CYC(b_+81, b_+84); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+85, b_+88); goto L_5c95; } CYC(b_+85, b_+88);
  CYC(b_+88, b_+90); E = 0x33;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+93); alu_and(gb, 0x30);
  if (!(F & FZ)) { CYCT(b_+93, b_+96); goto L_5c72; } CYC(b_+93, b_+96);
  CYC(b_+96, b_+99); A = mem_rd(gb, wActiveTileType);
  CYC(b_+99, b_+101); alu_bit(gb, 4, A);
  if ((F & FZ)) { CYCT(b_+101, b_+103); goto L_5bff; } CYC(b_+101, b_+103);
  CYC(b_+103, b_+106); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+106, b_+108); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+108, b_+111); goto L_5c7a; } CYC(b_+108, b_+111);
L_5bff:
  CYC(b_+111, b_+113); E = 0x0b;
  CYC(b_+113, b_+114); A = mem_rd(gb, DE);
  CYC(b_+114, b_+116); alu_bit(gb, 3, A);
  if ((F & FZ)) { CYCT(b_+116, b_+118); goto L_5c0d; } CYC(b_+116, b_+118);
  CYC(b_+118, b_+121); A = mem_rd(gb, wLastActiveTileType);
  CYC(b_+121, b_+123); alu_cp(gb, 0x90);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_5c72; } CYC(b_+123, b_+125);
L_5c0d:
  CYC(b_+125, b_+128); SET_HL(wActiveTileType);
  CYC(b_+128, b_+129); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+129, b_+131); alu_cp(gb, 0x04);
  if ((F & FZ)) { CYCT(b_+131, b_+134); forceDrownLink_hook(gb); return; } CYC(b_+131, b_+134);
  CYC(b_+134, b_+136); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+136, b_+138); goto L_5c26; } CYC(b_+136, b_+138);
  CYC(b_+138, b_+139); A = mem_rd(gb, HL);
  CYC(b_+139, b_+140); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+140, b_+142); goto L_5c26; } CYC(b_+140, b_+142);
  CYC(b_+142, b_+144); A = 0x5f;
  CALL_C(b_+144, playSound_b00_hook, SYM(playSound_b00), b_+147);
  CYC(b_+147, b_+150); TAIL(respawnLink);
L_5c26:
  CALL_C(b_+150, linkUpdateVelocity_hook, SYM(linkUpdateVelocity), b_+153);
L_5c29:
  CYC(b_+153, b_+155); L = 0x0a;
  CYC(b_+155, b_+157); E = 0x14;
  CYC(b_+157, b_+158); A = mem_rd(gb, DE);
  CYC(b_+158, b_+159); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+159, b_+160); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+160, b_+161); E = alu_inc8(gb, E);
  CYC(b_+161, b_+162); A = mem_rd(gb, DE);
  CYC(b_+162, b_+163); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+163, b_+164); mem_wr(gb, HL, A); SET_HL(HL + 1);
L_5c34:
  CYC(b_+164, b_+166); C = 0x24;
  CYC(b_+166, b_+169); A = mem_rd(gb, wLinkInAir);
  CYC(b_+169, b_+171); alu_bit(gb, 5, A);
  if ((F & FZ)) { CYCT(b_+171, b_+173); goto L_5c3f; } CYC(b_+171, b_+173);
  CYC(b_+173, b_+175); C = 0x0e;
L_5c3f:
  CYC(b_+175, b_+177); L = 0x14;
  CYC(b_+177, b_+178); A = mem_rd(gb, HL);
  CYC(b_+178, b_+179); alu_add(gb, C);
  CYC(b_+179, b_+180); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+180, b_+181); A = mem_rd(gb, HL);
  CYC(b_+181, b_+183); alu_adc(gb, 0x00);
  CYC(b_+183, b_+184); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+184, b_+186); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+186, b_+188); goto L_5c54; } CYC(b_+186, b_+188);
  CYC(b_+188, b_+190); alu_cp(gb, 0x03);
  if ((F & FC)) { CYCT(b_+190, b_+192); goto L_5c54; } CYC(b_+190, b_+192);
  CYC(b_+192, b_+193); alu_xor(gb, A);
  CYC(b_+193, b_+194); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+194, b_+196); mem_wr(gb, HL, 0x03);
L_5c54:
  CALL_C(b_+196, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+199);
  CYC(b_+199, b_+201); E = 0x33;
  CYC(b_+201, b_+202); A = mem_rd(gb, DE);
  CYC(b_+202, b_+204); alu_and(gb, 0x30);
  if (!(F & FZ)) { CYCT(b_+204, b_+206); goto L_5c72; } CYC(b_+204, b_+206);
  CALL_C(b_+206, linkAdjustAngleInSidescrollingArea_hook, SYM(linkAdjustAngleInSidescrollingArea), b_+209);
  CALL_C(b_+209, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+212);
  CALL_C(b_+212, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+215);
  CYC(b_+215, b_+217); E = 0x0b;
  CYC(b_+217, b_+218); A = mem_rd(gb, DE);
  CYC(b_+218, b_+220); alu_cp(gb, 0xa9);
  if ((F & FC)) { CYCT(b_+220, b_+222); goto L_5c70; } CYC(b_+220, b_+222);
  CYC(b_+222, b_+224); goto L_5c72;
L_5c70:
  CYC(b_+224, b_+225); alu_xor(gb, A);
  CYC(b_+225, b_+226); ret_effect(gb); return;
L_5c72:
  CYC(b_+226, b_+228); E = 0x0b;
  CYC(b_+228, b_+229); A = mem_rd(gb, DE);
  CYC(b_+229, b_+231); alu_and(gb, 0xf8);
  CYC(b_+231, b_+233); alu_add(gb, 0x01);
  CYC(b_+233, b_+234); mem_wr(gb, DE, A);
L_5c7a:
  CYC(b_+234, b_+235); alu_xor(gb, A);
  CYC(b_+235, b_+237); E = 0x14;
  CYC(b_+237, b_+238); mem_wr(gb, DE, A);
  CYC(b_+238, b_+239); E = alu_inc8(gb, E);
  CYC(b_+239, b_+240); mem_wr(gb, DE, A);
  CYC(b_+240, b_+243); mem_wr(gb, wLinkInAir, A);
  CYC(b_+243, b_+246); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+246, b_+248); alu_cp(gb, 0x02);
  if ((F & FZ)) { CALL_C_CC(b_+248, dealSpikeDamageToLink_hook, SYM(dealSpikeDamageToLink), b_+251); } else CYC(b_+248, b_+251);
  CYC(b_+251, b_+253); A = 0xa3;
  CALL_C(b_+253, playSound_b00_hook, SYM(playSound_b00), b_+256);
  CALL_C(b_+256, animateLinkStanding_hook, SYM(animateLinkStanding), b_+259);
  CYC(b_+259, b_+260); alu_xor(gb, A);
  CYC(b_+260, b_+261); ret_effect(gb); return;
L_5c95:
  CYC(b_+261, b_+263); E = 0x12;
  CYC(b_+263, b_+264); alu_xor(gb, A);
  CYC(b_+264, b_+265); mem_wr(gb, DE, A);
  CYC(b_+265, b_+267); A = 0xff;
  CYC(b_+267, b_+268); mem_wr(gb, DE, A);
  CYC(b_+268, b_+270); E = 0x09;
  CYC(b_+270, b_+271); mem_wr(gb, DE, A);
  CYC(b_+271, b_+273); goto L_5c7a;
}

void linkState12_hook(GB *gb) {
  BASE(linkState12);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+12) { goto L_60b1; }
    else if (jt_ == b_+103) { goto L_610c; }
    else if (jt_ == b_+145) { goto L_6136; }
    else if (jt_ == b_+164) { goto L_6149; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
L_60b1:
  CALL_C(b_+12, itemIncSubstate_hook, SYM(itemIncSubstate), b_+15);
  CYC(b_+15, b_+17); L = 0x2f;
  CYC(b_+17, b_+19); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_60c7; } CYC(b_+19, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+28); A = mem_rd(gb, wLinkTurningDisabled);
  CYC(b_+28, b_+29); alu_or(gb, C);
  CYC(b_+29, b_+31); A = 0x18;
  if ((F & FZ)) { CALL_C_CC(b_+31, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+34); } else CYC(b_+31, b_+34);
L_60c7:
  CYC(b_+34, b_+36); A = 0x53;
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+42); push_effect(gb, b_+42);
  uint16_t local_sp0 = gb->sp;
  link_state12_get_length_of_cliff_hook(gb);
  if (gb->pc != b_+42 || gb->sp != (uint16_t)(local_sp0 + 2)) return;
  if ((F & FZ)) { CYCT(b_+42, b_+44); goto L_60da; } CYC(b_+42, b_+44);
  CYC(b_+44, b_+47); SET_HL(b_+91);
  CYC(b_+47, b_+48); link_add_a_to_hl(gb, b_+48);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); E = 0x10;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+53); ret_effect(gb); return;
L_60da:
  CYC(b_+53, b_+56); A = mem_rd(gb, wScreenTransitionBoundaryY);
  CYC(b_+56, b_+57); B = A;
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = 0x0b;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+62); alu_sub(gb, B);
  CYC(b_+62, b_+63); mem_wr(gb, HL, B);
  CYC(b_+63, b_+65); L = 0x0f;
  CYC(b_+65, b_+66); mem_wr(gb, HL, A);
  CYC(b_+66, b_+68); L = 0x1a;
  CYC(b_+68, b_+70); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 6)));
  CYC(b_+70, b_+72); L = 0x05;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x02);
  CYC(b_+74, b_+75); alu_xor(gb, A);
  CYC(b_+75, b_+77); L = 0x10;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+80); L = 0x14;
  CYC(b_+80, b_+81); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0xff);
  CYC(b_+83, b_+84); A = alu_inc8(gb, A);
  CYC(b_+84, b_+87); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+87, b_+89); L = 0x2f;
  CYC(b_+89, b_+91); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(b_+91, b_+92); ret_effect(gb); return;
L_610c:
  CALL_C(b_+103, objectApplySpeed_hook, SYM(objectApplySpeed), b_+106);
  CYC(b_+106, b_+108); C = 0x20;
  CALL_C(b_+108, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+111);
  if (!(F & FZ)) { CYCT(b_+111, b_+114); specialObjectAnimate_hook(gb); return; } CYC(b_+111, b_+114);
  CYC(b_+114, b_+115); H = D;
  CYC(b_+115, b_+117); L = 0x2f;
  CYC(b_+117, b_+119); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+119, b_+121); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  if (!(F & FZ)) { CALL_C_CC(b_+121, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+124); } else CYC(b_+121, b_+124);
  CALL_C(b_+124, specialObjectTryToBreakTile_source05_hook, SYM(specialObjectTryToBreakTile_source05), b_+127);
  CYC(b_+127, b_+128); alu_xor(gb, A);
  CYC(b_+128, b_+131); mem_wr(gb, wLinkInAir, A);
  CYC(b_+131, b_+134); mem_wr(gb, wLinkSwimmingState, A);
  CYC(b_+134, b_+136); A = 0xa3;
  CALL_C(b_+136, playSound_b00_hook, SYM(playSound_b00), b_+139);
  CALL_C(b_+139, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+142);
  CYC(b_+142, b_+145); TAIL(initLinkStateAndAnimateStanding);
L_6136:
  CYC(b_+145, b_+147); C = 0x20;
  CALL_C(b_+147, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+150);
  if (!(F & FZ)) { CYCT(b_+150, b_+153); specialObjectAnimate_hook(gb); return; } CYC(b_+150, b_+153);
  CYC(b_+153, b_+155); A = 0x82;
  CYC(b_+155, b_+158); mem_wr(gb, wScreenTransitionDirection, A);
  CYC(b_+158, b_+160); E = 0x05;
  CYC(b_+160, b_+162); A = 0x03;
  CYC(b_+162, b_+163); mem_wr(gb, DE, A);
  CYC(b_+163, b_+164); ret_effect(gb); return;
L_6149:
  CYC(b_+164, b_+167); A = mem_rd(gb, wScreenVariables);
  CYC(b_+167, b_+169); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+169, b_+170); ret_effect(gb); return; } CYC(b_+169, b_+170);
  CYC(b_+170, b_+173); push_effect(gb, b_+173);
  uint16_t local_sp1 = gb->sp;
  link_state12_get_length_of_cliff_hook(gb);
  if (gb->pc != b_+173 || gb->sp != (uint16_t)(local_sp1 + 2)) return;
  CYC(b_+173, b_+174); H = D;
  CYC(b_+174, b_+176); L = 0x0b;
  CYC(b_+176, b_+177); A = mem_rd(gb, HL);
  CYC(b_+177, b_+178); alu_sub(gb, B);
  CYC(b_+178, b_+179); mem_wr(gb, HL, B);
  CYC(b_+179, b_+181); L = 0x0f;
  CYC(b_+181, b_+182); mem_wr(gb, HL, A);
  CYC(b_+182, b_+184); L = 0x1a;
  CYC(b_+184, b_+186); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(b_+186, b_+188); L = 0x05;
  CYC(b_+188, b_+190); mem_wr(gb, HL, 0x01);
  CYC(b_+190, b_+191); ret_effect(gb); return;
}

void link_state12_get_length_of_cliff_hook(GB *gb) {
  BASE(linkState12);
  uint16_t sp0_ = gb->sp;
  CYC(b_+191, b_+192); H = D;
  CYC(b_+192, b_+194); L = 0x0b;
  CYC(b_+194, b_+195); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+195, b_+197); alu_add(gb, 0x05);
  CYC(b_+197, b_+198); B = A;
  CYC(b_+198, b_+199); L = alu_inc8(gb, L);
  CYC(b_+199, b_+200); C = mem_rd(gb, HL);
  CYC(b_+200, b_+202); L = 0x09;
  CYC(b_+202, b_+203); A = mem_rd(gb, HL);
  CYC(b_+203, b_+204); alu_add(gb, A);
  CYC(b_+204, b_+206); A = alu_swap(gb, A);
  CYC(b_+206, b_+208); alu_and(gb, 0x03);
  CYC(b_+208, b_+211); SET_HL(b_+274);
  CYC(b_+211, b_+212); link_add_double_index(gb, b_+212);
  CYC(b_+212, b_+213); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+213, b_+215); H8(hFF8D) = A;
  CYC(b_+215, b_+216); A = mem_rd(gb, HL);
  CYC(b_+216, b_+218); H8(hFF8C) = A;
  CYC(b_+218, b_+220); A = 0x01;
  CYC(b_+220, b_+222); H8(hFF8B) = A;
L_6183:
  CYC(b_+222, b_+224); A = H8(hFF8D);
  CYC(b_+224, b_+225); alu_add(gb, B);
  CYC(b_+225, b_+226); B = A;
  CYC(b_+226, b_+228); A = H8(hFF8C);
  CYC(b_+228, b_+229); alu_add(gb, C);
  CYC(b_+229, b_+230); C = A;
  CALL_C(b_+230, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+233);
  if (!(F & FC)) { CYCT(b_+233, b_+235); goto L_61a8; } CYC(b_+233, b_+235);
  CYC(b_+235, b_+237); A = 0x85;
  CALL_C(b_+237, tryToBreakTile_hook, SYM(tryToBreakTile), b_+240);
  if ((F & FC)) { CYCT(b_+240, b_+242); goto L_61ad; } CYC(b_+240, b_+242);
  CYC(b_+242, b_+244); A = H8(hFF92);
  CYC(b_+244, b_+247); SET_HL(SYM(landableTileFromCliffExceptions));
  CALL_C(b_+247, findByteInCollisionTable_hook, SYM(findByteInCollisionTable), b_+250);
  if ((F & FC)) { CYCT(b_+250, b_+252); goto L_61ad; } CYC(b_+250, b_+252);
  CYC(b_+252, b_+254); A = H8(hFF8B);
  CYC(b_+254, b_+255); A = alu_inc8(gb, A);
  CYC(b_+255, b_+257); H8(hFF8B) = A;
  CYC(b_+257, b_+259); goto L_6183;
L_61a8:
  CALL_C(b_+259, getTileAtPosition_hook, SYM(getTileAtPosition), b_+262);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+263, b_+264); ret_effect(gb); return; } CYC(b_+263, b_+264);
L_61ad:
  CYC(b_+264, b_+266); A = H8(hFF8B);
  CYC(b_+266, b_+268); alu_cp(gb, 0x0b);
  if ((F & FC)) { CYCT(b_+268, b_+270); goto L_61b5; } CYC(b_+268, b_+270);
  CYC(b_+270, b_+272); A = 0x0b;
L_61b5:
  CYC(b_+272, b_+273); alu_or(gb, A);
  CYC(b_+273, b_+274); ret_effect(gb); return;
}

void warpTransition0_hook(GB *gb) {
  BASE(warpTransition0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, warpTransition_setLinkFacingDir_hook, SYM(warpTransition_setLinkFacingDir), SYM(warpTransitionA));
  TAIL(warpTransitionA);
}

void warpTransitionA_hook(GB *gb) {
  BASE(warpTransitionA);
  CYC(b_+0, b_+3);
  TAIL(initLinkStateAndAnimateStanding);
}

void warpTransitionE_hook(GB *gb) {
  BASE(warpTransitionE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_and(gb, 0xf0);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  TAIL(warpTransition1);
}

void warpTransition1_hook(GB *gb) {
  BASE(warpTransition1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, warpTransition_setLinkFacingDir_hook, SYM(warpTransition_setLinkFacingDir), SYM(warpUpdateRespawnPoint));
  TAIL(warpUpdateRespawnPoint);
}

void warpUpdateRespawnPoint_hook(GB *gb) {
  BASE(warpUpdateRespawnPoint);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wActiveGroup);
  CYC(b_+3, b_+5); alu_cp(gb, 0x06);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7);
    TAIL(warpTransition0);
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+10);
  CALL_C(b_+10, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+13);
  CYC(b_+13, b_+16);
  TAIL(initLinkStateAndAnimateStanding);
}

void warpTransitionC_hook(GB *gb) {
  BASE(warpTransitionC);
  CYC(b_+0, b_+3); A = W8(wcc50);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+7); E = (uint8_t)w1Link_direction;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+11);
  TAIL(initLinkStateAndAnimateStanding);
}

void warpTransition_setLinkFacingDir_hook(GB *gb) {
  BASE(warpTransition_setLinkFacingDir);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+6); SET_HL(SYM(facingDirAfterWarpTable));
  CALL_C(b_+6, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+9);
  if (F & FC) {
    CYCT(b_+9, b_+11);
  } else {
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+13); A = 0x02;
  }
  CYC(b_+13, b_+15); E = (uint8_t)w1Link_direction;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void warpTransition2_hook(GB *gb) {
  BASE(warpTransition2);
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+5); W8(wWarpTransition2) = A;
  CYC(b_+5, b_+7); A = 0x6e;
  CYC(b_+7, b_+10);
  TAIL(playSound_b00);
}

void warpTransition4_hook(GB *gb) {
  BASE(warpTransition4);
  CYC(b_+0, b_+3); A = W8(wWarpTransition);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+4, b_+7);
    TAIL(warpTransition0);
  }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); A = 0x01;
  CYC(b_+9, b_+12); W8(wWarpTransition2) = A;
  CYC(b_+12, b_+14); A = 0x6e;
  CYC(b_+14, b_+17);
  TAIL(playSound_b00);
}

void warpTransition5_hook(GB *gb) {
  BASE(warpTransition5);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == SYM(warpTransition5_00) && hook_is(gb, SYM(warpTransition5_00), warpTransition5_00_hook)) { warpTransition5_00_hook(gb); return; }
    else if (jt_ == SYM(warpTransition5_01) && hook_is(gb, SYM(warpTransition5_01), warpTransition5_01_hook)) { warpTransition5_01_hook(gb); return; }
    else if (jt_ == SYM(warpTransition5_02) && hook_is(gb, SYM(warpTransition5_02), warpTransition5_02_hook)) { warpTransition5_02_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void warpTransition5_00_hook(GB *gb) {
  BASE(warpTransition5_00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); SET_BC(0x0020);
  CALL_C(b_+6, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+9);
  CALL_C(b_+9, objectGetZAboveScreen_hook, SYM(objectGetZAboveScreen), b_+12);
  CYC(b_+12, b_+14); L = (uint8_t)w1Link_zh;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); L = (uint8_t)w1Link_yh;
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_sub(gb, 0x04);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = (uint8_t)w1Link_direction;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x02);
  CYC(b_+25, b_+27); A = 0x04;
  CYC(b_+27, b_+30);
  TAIL(specialObjectSetAnimation);
}

static void warp_transition7_link_collapsed(GB *gb, uint16_t sp0_) {
  BASE(warpTransition7);
  CALL_C(b_+79, itemIncSubstate_hook, SYM(itemIncSubstate), b_+82);
  CYC(b_+82, b_+84); L = (uint8_t)w1Link_counter1;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x1e);
  CYC(b_+86, b_+88); A = 0x02;
  CALL_C(b_+88, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+91);
  CYC(b_+91, b_+93); A = 0x87;
  CYC(b_+93, b_+96);
  playSound_b00_hook(gb);
}

void warpTransition5_01_hook(GB *gb) {
  BASE(warpTransition5_01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); C = 0x20;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (!(F & FZ)) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_HL(SYM(hazardCollisionTable));
  CALL_C(b_+12, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+15);
  if (!(F & FC)) {
    CYCT(b_+15, b_+18);
    warp_transition7_link_collapsed(gb, sp0_);
    return;
  }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+21);
  TAIL(initLinkStateAndAnimateStanding);
}

void warpTransition7_hook(GB *gb) {
  BASE(warpTransition7);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+45) { goto state1; }
    else if (jt_ == b_+70) { goto state2; }
    else if (jt_ == b_+96) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = (uint8_t)w1Link_direction;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x02);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x10);
  CYC(b_+23, b_+25); L = (uint8_t)w1Link_speed;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x28);
  CYC(b_+27, b_+29); L = (uint8_t)w1Link_visible;
  CYC(b_+29, b_+31); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CYC(b_+31, b_+33); L = (uint8_t)w1Link_counter1;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x78);
  CYC(b_+35, b_+37); A = 0x04;
  CALL_C(b_+37, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+40);
  CYC(b_+40, b_+42); A = 0x65;
  CYC(b_+42, b_+45);
  TAIL(playSound_b00);

state1:
  CALL_C(b_+45, itemDecCounter1_hook, SYM(itemDecCounter1), b_+48);
  if (!(F & FZ)) { CYCT(b_+48, b_+49); ret_effect(gb); return; }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); L = 0x05;
  CYC(b_+51, b_+52); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+52, b_+54); L = (uint8_t)w1Link_visible;
  CYC(b_+54, b_+56); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+56, b_+58); L = (uint8_t)w1Link_counter1;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x30);
  CYC(b_+60, b_+62); A = 0x10;
  CALL_C(b_+62, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+65);
  CYC(b_+65, b_+67); A = 0x85;
  CYC(b_+67, b_+70);
  TAIL(playSound_b00);

state2:
  CALL_C(b_+70, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+73);
  CALL_C(b_+73, itemDecCounter1_hook, SYM(itemDecCounter1), b_+76);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+79);
    TAIL(specialObjectUpdatePosition);
  }
  CYC(b_+76, b_+79);
  warp_transition7_link_collapsed(gb, sp0_);
  return;

state3:
  CALL_C(b_+96, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), SYM(warpTransition5_02));
  TAIL(warpTransition5_02);
}

void warpTransition5_02_hook(GB *gb) {
  BASE(warpTransition5_02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, itemDecCounter1_hook, SYM(itemDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkIncrementDirectionOnOddFrames_hook(GB *gb) {
  BASE(linkIncrementDirectionOnOddFrames);
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  TAIL(linkIncrementDirection);
}

void linkIncrementDirection_hook(GB *gb) {
  BASE(linkIncrementDirection);
  CYC(b_+0, b_+2); E = (uint8_t)w1Link_direction;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x03);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8); ret_effect(gb);
}

void warpTransition8_hook(GB *gb) {
  BASE(warpTransition8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+20) { goto state0; }
    else if (jt_ == b_+62) { goto state1; }
    else if (jt_ == b_+93) { goto state2; }
    else if (jt_ == b_+119) { goto state3; }
    else if (jt_ == b_+133) { goto state4; }
    else if (jt_ == b_+150) { goto state5; }
    else if (jt_ == b_+165) { goto state6; }
    else if (jt_ == b_+191) { goto state7; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); A = 0xff;
  CYC(b_+25, b_+28); W8(wDisabledObjects) = A;
  CYC(b_+28, b_+30); A = 0x80;
  CYC(b_+30, b_+33); W8(wMenuDisabled) = A;
  CYC(b_+33, b_+35); A = 0x15;
  CYC(b_+35, b_+38); W8(wCutsceneTrigger) = A;
  CYC(b_+38, b_+41); SET_BC(0xff60);
  CALL_C(b_+41, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+44);
  CYC(b_+44, b_+46); L = 0x06;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x30);
  CALL_C(b_+48, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+51);
  CALL_C(b_+51, restartSound_hook, SYM(restartSound), b_+54);
  CYC(b_+54, b_+56); A = 0xb4;
  CALL_C(b_+56, playSound_b00_hook, SYM(playSound_b00), b_+59);
  CYC(b_+59, b_+62);
  TAIL(objectCenterOnTile);

state1:
  CYC(b_+62, b_+64); C = 0x02;
  CALL_C(b_+64, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+67);
  CYC(b_+67, b_+70); A = W8(wFrameCounter);
  CYC(b_+70, b_+72); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+74);
  } else {
    CYC(b_+72, b_+74);
    CYC(b_+74, b_+77); SET_HL(wTmpcbbc);
    CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+78, b_+81); A = W8(wFrameCounter);
  CYC(b_+81, b_+83); alu_and(gb, 0x03);
  if (F & FZ) CALL_C_CC(b_+83, linkIncrementDirection_hook, SYM(linkIncrementDirection), b_+86);
  else CYC(b_+83, b_+86);
  CALL_C(b_+86, itemDecCounter1_hook, SYM(itemDecCounter1), b_+89);
  if (!(F & FZ)) { CYCT(b_+89, b_+90); ret_effect(gb); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+93);
  TAIL(itemIncSubstate);

state2:
  CYC(b_+93, b_+95); C = 0x02;
  CALL_C(b_+95, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+98);
  CALL_C(b_+98, linkIncrementDirectionOnOddFrames_hook, SYM(linkIncrementDirectionOnOddFrames), b_+101);
  CYC(b_+101, b_+102); H = D;
  CYC(b_+102, b_+104); L = 0x15;
  CYC(b_+104, b_+106); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+106, b_+107); ret_effect(gb); return; }
  CYC(b_+106, b_+107);
  CYC(b_+107, b_+109); L = 0x06;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x28);
  CYC(b_+111, b_+113); A = 0x02;
  CALL_C(b_+113, fadeoutToWhiteWithDelay_hook, SYM(fadeoutToWhiteWithDelay), b_+116);
  CYC(b_+116, b_+119);
  TAIL(itemIncSubstate);

state3:
  CALL_C(b_+119, linkIncrementDirectionOnOddFrames_hook, SYM(linkIncrementDirectionOnOddFrames), b_+122);
  CALL_C(b_+122, itemDecCounter1_hook, SYM(itemDecCounter1), b_+125);
  if (!(F & FZ)) { CYCT(b_+125, b_+126); ret_effect(gb); return; }
  CYC(b_+125, b_+126);
  CYC(b_+126, b_+129); SET_HL(wTmpcbb3);
  CYC(b_+129, b_+130); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+130, b_+133);
  TAIL(itemIncSubstate);

state4:
  CALL_C(b_+133, linkIncrementDirectionOnOddFrames_hook, SYM(linkIncrementDirectionOnOddFrames), b_+136);
  CYC(b_+136, b_+139); A = W8(wCutsceneState);
  CYC(b_+139, b_+141); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+141, b_+142); ret_effect(gb); return; }
  CYC(b_+141, b_+142);
  CALL_C(b_+142, itemIncSubstate_hook, SYM(itemIncSubstate), b_+145);
  CYC(b_+145, b_+147); L = 0x06;
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x28);
  CYC(b_+149, b_+150); ret_effect(gb);
  return;

state5:
  CYC(b_+150, b_+152); C = 0x02;
  CALL_C(b_+152, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+155);
  CALL_C(b_+155, linkIncrementDirectionOnOddFrames_hook, SYM(linkIncrementDirectionOnOddFrames), b_+158);
  CALL_C(b_+158, itemDecCounter1_hook, SYM(itemDecCounter1), b_+161);
  if (!(F & FZ)) { CYCT(b_+161, b_+162); ret_effect(gb); return; }
  CYC(b_+161, b_+162);
  CYC(b_+162, b_+165);
  TAIL(itemIncSubstate);

state6:
  CYC(b_+165, b_+167); C = 0x02;
  CALL_C(b_+167, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+170);
  CYC(b_+170, b_+173); A = W8(wFrameCounter);
  CYC(b_+173, b_+175); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+175, b_+176); ret_effect(gb); return; }
  CYC(b_+175, b_+176);
  CALL_C(b_+176, linkIncrementDirection_hook, SYM(linkIncrementDirection), b_+179);
  CYC(b_+179, b_+182); SET_HL(wTmpcbbc);
  CYC(b_+182, b_+183); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+183, b_+184); ret_effect(gb); return; }
  CYC(b_+183, b_+184);
  CYC(b_+184, b_+187); SET_HL(wTmpcbb3);
  CYC(b_+187, b_+188); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+188, b_+191);
  TAIL(itemIncSubstate);

state7:
  CYC(b_+191, b_+194); A = W8(wDisabledObjects);
  CYC(b_+194, b_+196); alu_and(gb, 0x81);
  if (F & FZ) {
    CYCT(b_+196, b_+198);
    goto finish;
  }
  CYC(b_+196, b_+198);
  CYC(b_+198, b_+201); A = W8(wFrameCounter);
  CYC(b_+201, b_+203); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+203, b_+204); ret_effect(gb); return; }
  CYC(b_+203, b_+204);
  CYC(b_+204, b_+207);
  TAIL(linkIncrementDirection);

finish:
  CYC(b_+207, b_+209); E = (uint8_t)w1Link_direction;
  CYC(b_+209, b_+210); A = mem_rd(gb, DE);
  CYC(b_+210, b_+212); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+212, b_+215);
    TAIL(linkIncrementDirection);
  }
  CYC(b_+212, b_+215);
  CYC(b_+215, b_+218); A = W8(wActiveMusic2);
  CYC(b_+218, b_+221); W8(wActiveMusic) = A;
  CALL_C(b_+221, playSound_b00_hook, SYM(playSound_b00), b_+224);
  CALL_C(b_+224, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+227);
  CALL_C(b_+227, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+230);
  CALL_C(b_+230, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+233);
  CYC(b_+233, b_+236);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState0a_hook(GB *gb) {
  BASE(linkState0a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wWarpTransition);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == SYM(warpTransition0) && hook_is(gb, SYM(warpTransition0), warpTransition0_hook)) { warpTransition0_hook(gb); return; }
    else if (jt_ == SYM(warpTransition1) && hook_is(gb, SYM(warpTransition1), warpTransition1_hook)) { warpTransition1_hook(gb); return; }
    else if (jt_ == SYM(warpTransition2) && hook_is(gb, SYM(warpTransition2), warpTransition2_hook)) { warpTransition2_hook(gb); return; }
    else if (jt_ == SYM(warpTransition3) && hook_is(gb, SYM(warpTransition3), warpTransition3_hook)) { warpTransition3_hook(gb); return; }
    else if (jt_ == SYM(warpTransition4) && hook_is(gb, SYM(warpTransition4), warpTransition4_hook)) { warpTransition4_hook(gb); return; }
    else if (jt_ == SYM(warpTransition5) && hook_is(gb, SYM(warpTransition5), warpTransition5_hook)) { warpTransition5_hook(gb); return; }
    else if (jt_ == SYM(warpTransition6) && hook_is(gb, SYM(warpTransition6), warpTransition6_hook)) { warpTransition6_hook(gb); return; }
    else if (jt_ == SYM(warpTransitionA) && hook_is(gb, SYM(warpTransitionA), warpTransitionA_hook)) { warpTransitionA_hook(gb); return; }
    else if (jt_ == SYM(warpTransition8) && hook_is(gb, SYM(warpTransition8), warpTransition8_hook)) { warpTransition8_hook(gb); return; }
    else if (jt_ == SYM(warpTransition9) && hook_is(gb, SYM(warpTransition9), warpTransition9_hook)) { warpTransition9_hook(gb); return; }
    else if (jt_ == SYM(warpTransitionB) && hook_is(gb, SYM(warpTransitionB), warpTransitionB_hook)) { warpTransitionB_hook(gb); return; }
    else if (jt_ == SYM(warpTransitionC) && hook_is(gb, SYM(warpTransitionC), warpTransitionC_hook)) { warpTransitionC_hook(gb); return; }
    else if (jt_ == SYM(warpTransitionE) && hook_is(gb, SYM(warpTransitionE), warpTransitionE_hook)) { warpTransitionE_hook(gb); return; }
    else if (jt_ == SYM(warpTransitionF) && hook_is(gb, SYM(warpTransitionF), warpTransitionF_hook)) { warpTransitionF_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void linkState00_hook(GB *gb) {
  BASE(linkState00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, clearAllParentItems_hook, SYM(clearAllParentItems), b_+3);
  CALL_C(b_+3, specialObjectSetOamVariables_hook, SYM(specialObjectSetOamVariables), b_+6);
  CYC(b_+6, b_+8); A = 0x10;
  CALL_C(b_+8, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x24;
  CYC(b_+14, b_+16); A = 0x80;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); A = 0x06;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+22, b_+24); L = 0x29;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x01);
  CYC(b_+26, b_+29); A = W8(wLinkForceState);
  CYC(b_+29, b_+31); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
  } else {
    CYC(b_+31, b_+33);
    CYC(b_+33, b_+36); A = W8(wDisableRingTransformations);
    CYC(b_+36, b_+37); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+37, b_+39);
    } else {
      CYC(b_+37, b_+39);
      CALL_C(b_+39, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+42);
      CYC(b_+42, b_+44); alu_cp(gb, 0x0f);
      if (!(F & FZ)) {
        CYCT(b_+44, b_+46);
      } else {
        CYC(b_+44, b_+46);
        CYC(b_+46, b_+49); SET_HL(wLastAnimalMountPointY);
        CYC(b_+49, b_+50); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+50, b_+52); E = (uint8_t)w1Link_yh;
        CYC(b_+52, b_+53); mem_wr(gb, DE, A);
        CYC(b_+53, b_+54); A = mem_rd(gb, HL);
        CYC(b_+54, b_+56); E = (uint8_t)w1Link_xh;
        CYC(b_+56, b_+57); mem_wr(gb, DE, A);
      }
    }
  }
  CALL_C(b_+57, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+60);
  CALL_C(b_+60, checkLinkForceState_hook, SYM(checkLinkForceState), b_+63);
  CYC(b_+63, b_+66);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState08_hook(GB *gb) {
  BASE(linkState08);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+27) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); SET_HL(wcc50);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x00);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+19); ret_effect(gb); return; }
  CYC(b_+18, b_+19);
  CALL_C(b_+19, linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook, SYM(linkCancelAllItemUsageAndClearAdjacentWallsBitset), b_+22);
  CYC(b_+22, b_+24); A = 0x10;
  CYC(b_+24, b_+27);
  TAIL(specialObjectSetAnimation);

substate1:
  CALL_C(b_+27, checkLinkForceState_hook, SYM(checkLinkForceState), b_+30);
  CYC(b_+30, b_+33); SET_HL(wcc50);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(b_+35, b_+37);
    CYC(b_+37, b_+39); mem_wr(gb, HL, 0x00);
    CALL_C(b_+39, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+42);
  } else {
    CYCT(b_+35, b_+37);
  }
  CYC(b_+42, b_+45); A = W8(wcc63);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+46, checkUseItems_b00_hook, SYM(checkUseItems_b00), b_+49);
  else CYC(b_+46, b_+49);
  CYC(b_+49, b_+52); A = W8(wDisabledObjects);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+57);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState04_hook(GB *gb) {
  BASE(linkState04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+30) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+14);
  CYC(b_+14, b_+16); E = 0x30;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+20); W8(wcc52) = A;
  CYC(b_+20, b_+23); A = W8(wcc50);
  CYC(b_+23, b_+25); alu_and(gb, 0x0f);
  CYC(b_+25, b_+27); alu_add(gb, 0x0e);
  CYC(b_+27, b_+30);
  TAIL(specialObjectSetAnimation);

substate1:
  CALL_C(b_+30, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+33);
  CYC(b_+33, b_+36); A = W8(wcc50);
  CYC(b_+36, b_+37); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+37, b_+39);
  } else {
    CYC(b_+37, b_+39);
    CYC(b_+39, b_+42); A = W8(wDisabledObjects);
    CYC(b_+42, b_+44); alu_and(gb, 0x81);
    if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
    CYC(b_+44, b_+45);
  }
  CYC(b_+45, b_+47); E = 0x04;
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53); A = W8(wcc52);
  CYC(b_+53, b_+56);
  TAIL(specialObjectSetAnimation);
}

void warpTransition9_hook(GB *gb) {
  BASE(warpTransition9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+33) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+8, itemIncSubstate_hook, SYM(itemIncSubstate), b_+11);
  CYC(b_+11, b_+13); L = 0x0b;
  CYC(b_+13, b_+15); A = 0x08;
  CYC(b_+15, b_+16); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CALL_C(b_+17, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+20);
  CALL_C(b_+20, clearAllParentItems_hook, SYM(clearAllParentItems), b_+23);
  CYC(b_+23, b_+25); A = 0x0d;
  CALL_C(b_+25, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+28);
  CYC(b_+28, b_+30); A = 0x65;
  CYC(b_+30, b_+33);
  TAIL(playSound_b00);

substate1:
  CYC(b_+33, b_+35); E = 0x21;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+37, b_+40); specialObjectAnimate_hook(gb); return; }
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+42); A = 0x03;
  CYC(b_+42, b_+45); W8(wWarpTransition2) = A;
  CYC(b_+45, b_+46); ret_effect(gb);
}

void warpTransitionB_hook(GB *gb) {
  BASE(warpTransitionB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+28) { goto state1; }
    else if (jt_ == b_+48) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+10, itemIncSubstate_hook, SYM(itemIncSubstate), b_+13);
  CALL_C(b_+13, objectGetZAboveScreen_hook, SYM(objectGetZAboveScreen), b_+16);
  CYC(b_+16, b_+18); L = 0x0f;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = 0x08;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x02);
  CYC(b_+23, b_+25); A = 0x04;
  CYC(b_+25, b_+28);
  TAIL(specialObjectSetAnimation);

state1:
  CALL_C(b_+28, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+31);
  CYC(b_+31, b_+33); C = 0x0c;
  CALL_C(b_+33, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+36);
  if (!(F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
  CALL_C(b_+37, itemIncSubstate_hook, SYM(itemIncSubstate), b_+40);
  CALL_C(b_+40, animateLinkStanding_hook, SYM(animateLinkStanding), b_+43);
  CYC(b_+43, b_+45); A = 0x87;
  CYC(b_+45, b_+48);
  TAIL(playSound_b00);

state2:
  CYC(b_+48, b_+51); A = W8(wDisabledObjects);
  CYC(b_+51, b_+53); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+57);
  CYC(b_+57, b_+60);
  TAIL(initLinkStateAndAnimateStanding);
}

void warpTransitionF_hook(GB *gb) {
  BASE(warpTransitionF);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkLinkForceState_hook, SYM(checkLinkForceState), b_+3);
  CYC(b_+3, b_+6);
  TAIL(objectSetInvisible);
}

void linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook(GB *gb) {
  BASE(linkCancelAllItemUsageAndClearAdjacentWallsBitset);
  CYC(b_+0, b_+2); E = 0x33;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  TAIL(linkCancelAllItemUsage);
}

void linkCancelAllItemUsage_hook(GB *gb) {
  BASE(linkCancelAllItemUsage);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+3);
  CYC(b_+3, b_+6);
  TAIL(clearAllParentItems);
}

void linkState0e_hook(GB *gb) {
  BASE(linkState0e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+19) { goto substate1; }
    else if (jt_ == b_+29) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+10, itemIncSubstate_hook, SYM(itemIncSubstate), b_+13);
  CYC(b_+13, b_+15); E = 0x37;
  CYC(b_+15, b_+18); A = W8(wActiveRoom);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);

substate1:
  CALL_C(b_+19, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+22);
  if (F & FC) { CYCT(b_+22, b_+23); ret_effect(gb); return; }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, itemIncSubstate_hook, SYM(itemIncSubstate), b_+26);
  CALL_C(b_+26, objectSetInvisible_hook, SYM(objectSetInvisible), b_+29);

substate2:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = 0x37;
  CYC(b_+32, b_+35); A = W8(wActiveRoom);
  CYC(b_+35, b_+36); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; }
  CYC(b_+36, b_+37);
  CALL_C(b_+37, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+40);
  if (!(F & FC)) { CYCT(b_+40, b_+41); ret_effect(gb); return; }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+43); E = 0x05;
  CYC(b_+43, b_+45); A = 0x01;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+49);
  TAIL(objectSetVisiblec2);
}

void linkState0f_hook(GB *gb) {
  BASE(linkState0f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = W8(wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = 0x05;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+15) { goto substate0; }
    else if (jt_ == b_+52) { goto substate1; }
    else if (jt_ == b_+64) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+15, itemIncSubstate_hook, SYM(itemIncSubstate), b_+18);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x14);
  CYC(b_+21, b_+23); L = 0x09;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x10);
  CYC(b_+25, b_+27); L = 0x0b;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x38);
  CYC(b_+29, b_+31); L = 0x0d;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x50);
  CYC(b_+33, b_+35); L = 0x10;
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x28);
  CYC(b_+37, b_+39); L = 0x14;
  CYC(b_+39, b_+41); A = 0x80;
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0xfe);
  CYC(b_+44, b_+46); A = 0x02;
  CALL_C(b_+46, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+49);
  CYC(b_+49, b_+52);
  TAIL(objectSetVisiblec2);

substate1:
  CALL_C(b_+52, objectApplySpeed_hook, SYM(objectApplySpeed), b_+55);
  CYC(b_+55, b_+57); C = 0x20;
  CALL_C(b_+57, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+60);
  if (!(F & FC)) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+64);
  TAIL(itemIncSubstate);

substate2:
  CALL_C(b_+64, itemDecCounter1_hook, SYM(itemDecCounter1), b_+67);
  if (!(F & FZ)) { CYCT(b_+67, b_+68); ret_effect(gb); return; }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+71);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState0b_hook(GB *gb) {
  BASE(linkState0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+8) { goto substate0; }
    else if (jt_ == b_+31) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = 0x06;
  CYC(b_+13, b_+16); A = W8(wLinkStateParameter);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CALL_C(b_+17, clearPegasusSeedCounter_hook, SYM(clearPegasusSeedCounter), b_+20);
  CALL_C(b_+20, linkCancelAllItemUsageAndClearAdjacentWallsBitset_hook, SYM(linkCancelAllItemUsageAndClearAdjacentWallsBitset), b_+23);
  CALL_C(b_+23, updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+26);
  CYC(b_+26, b_+28); A = 0x10;
  CALL_C(b_+28, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+31);

substate1:
  CALL_C(b_+31, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+34);
  CALL_C(b_+34, itemDecCounter1_hook, SYM(itemDecCounter1), b_+37);
  CYC(b_+37, b_+39); L = 0x33;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x00);
  if (!(F & FZ)) {
    CYCT(b_+41, b_+44);
    TAIL(specialObjectUpdatePosition);
  }
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+47);
  TAIL(initLinkStateAndAnimateStanding);
}

void setLinkStateToDead_hook(GB *gb) {
  BASE(setLinkStateToDead);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x03;
  CALL_C(b_+2, linkSetState_hook, SYM(linkSetState), SYM(linkState03));
  TAIL(linkState03);
}

void linkState03_hook(GB *gb) {
  BASE(linkState03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); W8(wLinkHealth) = A;
  CYC(b_+4, b_+6); E = 0x05;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+12) { goto substate0; }
    else if (jt_ == b_+43) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+12, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+15);
  CYC(b_+15, b_+17); E = 0x2d;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+22);
    TAIL(linkUpdateKnockback);
  }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = 0x05;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+28); L = 0x06;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x04);
  CALL_C(b_+30, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+33);
  CYC(b_+33, b_+35); A = 0x01;
  CALL_C(b_+35, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+38);
  CYC(b_+38, b_+40); A = 0x64;
  CYC(b_+40, b_+43);
  TAIL(playSound_b00);

substate1:
  CALL_C(b_+43, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+46);
  CALL_C(b_+46, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+49);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = 0x21;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+54); alu_add(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto trigger_game_over; }
  CYC(b_+54, b_+56);
  if (!(F & FC)) { CYCT(b_+56, b_+57); ret_effect(gb); return; }
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+59); L = 0x06;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); A = 0x02;
  CYC(b_+63, b_+66);
  TAIL(specialObjectSetAnimation);

trigger_game_over:
  CYC(b_+66, b_+68); A = 0xff;
  CYC(b_+68, b_+71); W8(wGameOverScreenTrigger) = A;
  CYC(b_+71, b_+72); ret_effect(gb);
}

static void warp_transition3_enter_from_middle_bottom(GB *gb, uint16_t sp0_) {
  BASE(warpTransition3);
  CYC(b_+124, b_+126); A = 0x01;
  CYC(b_+126, b_+129); W8(wMenuDisabled) = A;
  CYC(b_+129, b_+131); L = 0x06;
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x1c);
  CYC(b_+133, b_+136); A = W8(wWarpTransition);
  CYC(b_+136, b_+138); alu_and(gb, 0x40);
  CYC(b_+138, b_+140); A = alu_swap(gb, A);
  CYC(b_+140, b_+141); B = A;
  CYC(b_+141, b_+144); A = W8(wActiveGroup);
  CYC(b_+144, b_+146); alu_and(gb, 0x04);
  CYC(b_+146, b_+147); alu_rrca(gb);
  CYC(b_+147, b_+148); alu_or(gb, B);
  CYC(b_+148, b_+151); SET_BC(b_+187);
  CALL_C(b_+151, addAToBc_hook, 0x006d, b_+154);
  CYC(b_+154, b_+156); L = 0x0b;
  CYC(b_+156, b_+157); A = mem_rd(gb, BC);
  CYC(b_+157, b_+158); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+158, b_+159); SET_BC(BC + 1);
  CYC(b_+159, b_+160); L = alu_inc8(gb, L);
  CYC(b_+160, b_+161); A = mem_rd(gb, BC);
  CYC(b_+161, b_+162); mem_wr(gb, HL, A);
  CYC(b_+162, b_+163); ret_effect(gb);
}

void warpTransition3_hook(GB *gb) {
  BASE(warpTransition3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto each_frame; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0x06;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10);
  CYC(b_+13, b_+16); A = W8(wWarpTransition);
  CYC(b_+16, b_+18); alu_and(gb, 0x40);
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+21); alu_rrca(gb);
  CYC(b_+21, b_+24); SET_BC(b_+51);
  CALL_C(b_+24, addAToBc_hook, 0x006d, b_+27);
  CYC(b_+27, b_+29); L = 0x08;
  CYC(b_+29, b_+30); A = mem_rd(gb, BC);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+32); SET_BC(BC + 1);
  CYC(b_+32, b_+33); A = mem_rd(gb, BC);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CALL_C(b_+34, updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+37);
  CALL_C(b_+37, animateLinkStanding_hook, SYM(animateLinkStanding), b_+40);
  CYC(b_+40, b_+43); A = W8(wWarpTransition);
  CYC(b_+43, b_+44); alu_rlca(gb);
  if (F & FC) { CYCT(b_+44, b_+46); goto destination_init; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); A = 0x6e;
  CYC(b_+48, b_+51);
  TAIL(playSound_b00);

each_frame:
  CYC(b_+55, b_+58); A = W8(wScrollMode);
  CYC(b_+58, b_+60); alu_and(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+60, b_+61); ret_effect(gb); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); A = 0x00;
  CYC(b_+63, b_+66); W8(wScrollMode) = A;
  CALL_C(b_+66, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+69);
  CALL_C(b_+69, itemDecCounter1_hook, SYM(itemDecCounter1), b_+72);
  if (!(F & FZ)) {
    CYCT(b_+72, b_+75);
    TAIL(specialObjectUpdatePosition);
  }
  CYC(b_+72, b_+75);
  CYC(b_+75, b_+77); A = 0x01;
  CYC(b_+77, b_+80); W8(wScrollMode) = A;
  CYC(b_+80, b_+81); alu_xor(gb, A);
  CYC(b_+81, b_+84); W8(wMenuDisabled) = A;
  CYC(b_+84, b_+87); A = W8(wWarpTransition);
  CYC(b_+87, b_+89); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+89, b_+92);
    TAIL(warpUpdateRespawnPoint);
  }
  CYC(b_+89, b_+92);
  CYC(b_+92, b_+94); A = alu_swap(gb, A);
  CYC(b_+94, b_+96); alu_and(gb, 0x03);
  CYC(b_+96, b_+99); W8(wWarpTransition2) = A;
  CYC(b_+99, b_+100); ret_effect(gb);
  return;

destination_init:
  CYC(b_+100, b_+101); H = D;
  CYC(b_+101, b_+104); A = W8(wWarpDestPos);
  CYC(b_+104, b_+106); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+106, b_+108);
    warp_transition3_enter_from_middle_bottom(gb, sp0_);
    return;
  }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(b_+110, b_+112);
    goto enter_from_bottom;
  }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); L = 0x0b;
  CALL_C(b_+114, setShortPosition_hook, SYM(setShortPosition), b_+117);
  CYC(b_+117, b_+119); L = 0x06;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x1c);
  CYC(b_+121, b_+124);
  TAIL(initLinkStateAndAnimateStanding);

enter_from_bottom:
  CYC(b_+163, b_+166); push_effect(gb, b_+166);
  warp_transition3_enter_from_middle_bottom(gb, sp0_);
  CYC(b_+166, b_+169); A = W8(wWarpDestPos);
  CYC(b_+169, b_+171); A = alu_swap(gb, A);
  CYC(b_+171, b_+173); alu_and(gb, 0xf0);
  CYC(b_+173, b_+174); B = A;
  CYC(b_+174, b_+177); A = W8(wActiveGroup);
  CYC(b_+177, b_+179); alu_and(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+179, b_+181);
  } else {
    CYC(b_+179, b_+181);
    CYC(b_+181, b_+182); alu_rlca(gb);
  }
  CYC(b_+182, b_+183); alu_or(gb, B);
  CYC(b_+183, b_+185); L = 0x0d;
  CYC(b_+185, b_+186); mem_wr(gb, HL, A);
  CYC(b_+186, b_+187); ret_effect(gb);
}

static void warp_transition6_flicker_visibility_and_dec_counter1(GB *gb,
                                                                  uint16_t sp0_) {
  BASE(warpTransition6);
  CYC(b_+20, b_+22); B = 0x03;
  CALL_C(b_+22, objectFlickerVisibility_hook, SYM(objectFlickerVisibility), b_+25);
  CYC(b_+25, b_+28);
  itemDecCounter1_hook(gb);
}

static void warp_transition6_create_destination_timewarp_animation(GB *gb,
                                                                    uint16_t sp0_) {
  BASE(warpTransition6);
  CALL_C(b_+28, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+32); ret_effect(gb); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0xdd);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+36, b_+39); A = W8(wcc50);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+42); ret_effect(gb);
}

static void warp_transition6_center_link_on_doorway(GB *gb, uint16_t sp0_) {
  BASE(warpTransition6);
  CALL_C(b_+42, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+45);
  CYC(b_+45, b_+46); push_effect(gb, HL);
  CYC(b_+46, b_+47); A = E;
  CYC(b_+47, b_+50); SET_HL(b_+94);
  CALL_C(b_+50, findByteAtHl_hook, SYM(findByteAtHl), b_+53);
  CYC(b_+53, b_+54); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(b_+54, b_+55); ret_effect(gb); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); push_effect(gb, HL);
  CYC(b_+56, b_+57); L = alu_dec8(gb, L);
  CYC(b_+57, b_+58); E = mem_rd(gb, HL);
  CYC(b_+58, b_+61); SET_HL(b_+94);
  CALL_C(b_+61, findByteAtHl_hook, SYM(findByteAtHl), b_+64);
  CYC(b_+64, b_+65); SET_HL(pop_effect(gb));
  if (!(F & FC)) { CYCT(b_+65, b_+67); goto check_right_tile; }
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); E = 0x0d;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+72); alu_and(gb, 0xf0);
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+74); ret_effect(gb);
  return;

check_right_tile:
  CYC(b_+74, b_+75); L = alu_inc8(gb, L);
  CYC(b_+75, b_+76); E = mem_rd(gb, HL);
  CYC(b_+76, b_+79); SET_HL(b_+94);
  CALL_C(b_+79, findByteAtHl_hook, SYM(findByteAtHl), b_+82);
  if (!(F & FC)) { CYCT(b_+82, b_+83); ret_effect(gb); return; }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+85); E = 0x0d;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+88); alu_add(gb, 0x08);
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+92); SET_HL(wEnteredWarpPosition);
  CYC(b_+92, b_+93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+93, b_+94); ret_effect(gb);
}

void warpTransition6_hook(GB *gb) {
  BASE(warpTransition6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+102) { goto substate0; }
    else if (jt_ == b_+126) { goto substate1; }
    else if (jt_ == b_+179) { goto substate2; }
    else if (jt_ == b_+196) { goto substate3; }
    else if (jt_ == b_+282) { goto substate4; }
    else if (jt_ == b_+290) { goto substate5; }
    else if (jt_ == b_+302) { goto substate6; }
    else if (jt_ == b_+314) { goto substate7; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+102, itemIncSubstate_hook, SYM(itemIncSubstate), b_+105);
  CYC(b_+105, b_+107); L = 0x06;
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0x1e);
  CYC(b_+109, b_+111); L = 0x08;
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x02);
  CYC(b_+113, b_+114); A = D;
  CYC(b_+114, b_+117); W8(wLinkCanPassNpcs) = A;
  CYC(b_+117, b_+120); W8(wMenuDisabled) = A;
  CYC(b_+120, b_+123); push_effect(gb, b_+123);
  warp_transition6_center_link_on_doorway(gb, sp0_);
  CYC(b_+123, b_+126);
  TAIL(objectSetInvisible);

substate1:
  CYC(b_+126, b_+129); A = W8(wPaletteThread_mode);
  CYC(b_+129, b_+130); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+130, b_+131); ret_effect(gb); return; }
  CYC(b_+130, b_+131);
  CALL_C(b_+131, itemDecCounter1_hook, SYM(itemDecCounter1), b_+134);
  if (!(F & FZ)) { CYCT(b_+134, b_+135); ret_effect(gb); return; }
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x10);
  CYC(b_+137, b_+140); push_effect(gb, b_+140);
  warp_transition6_create_destination_timewarp_animation(gb, sp0_);
  CYC(b_+140, b_+143); A = W8(wSentBackByStrangeForce);
  CYC(b_+143, b_+144); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+144, b_+146); goto warp_failed; }
  CYC(b_+144, b_+146);
  CYC(b_+146, b_+149); SET_HL((SYM(specialObjectCode_transformedLink__animate) + 1));
  CYC(b_+149, b_+151); E = 0x01;
  CALL_C(b_+151, interBankCall_hook, 0x008a, b_+154);
  CYC(b_+154, b_+156); C = alu_srl(gb, C);
  if (F & FC) { CYCT(b_+156, b_+158); goto warp_failed; }
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+161); SET_HL((SYM(specialObjectCode_transformedLink__state1) + 146));
  CYC(b_+161, b_+163); E = 0x01;
  CALL_C(b_+163, interBankCall_hook, 0x008a, b_+166);
  CYC(b_+166, b_+168); C = alu_srl(gb, C);
  if (F & FC) { CYCT(b_+168, b_+170); goto warp_failed; }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+173);
  TAIL(itemIncSubstate);

warp_failed:
  CYC(b_+173, b_+175); E = 0x05;
  CYC(b_+175, b_+177); A = 0x04;
  CYC(b_+177, b_+178); mem_wr(gb, DE, A);
  CYC(b_+178, b_+179); ret_effect(gb);
  return;

substate2:
  CALL_C(b_+179, itemDecCounter1_hook, SYM(itemDecCounter1), b_+182);
  if (!(F & FZ)) { CYCT(b_+182, b_+183); ret_effect(gb); return; }
  CYC(b_+182, b_+183);
  CYC(b_+183, b_+185); mem_wr(gb, HL, 0x1e);

make_link_visible_and_play_sound:
  CYC(b_+185, b_+187); A = 0xd4;
  CALL_C(b_+187, playSound_b00_hook, SYM(playSound_b00), b_+190);
  CALL_C(b_+190, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+193);
  CYC(b_+193, b_+196);
  TAIL(itemIncSubstate);

substate3:
  CYC(b_+196, b_+199); push_effect(gb, b_+199);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(b_+199, b_+200); ret_effect(gb); return; }
  CYC(b_+199, b_+200);
  CYC(b_+200, b_+203); A = W8(wLinkTimeWarpTile);
  CYC(b_+203, b_+204); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+204, b_+206); goto maybe_show_strange_force_text; }
  CYC(b_+204, b_+206);
  CYC(b_+206, b_+209); SET_HL(wPortalGroup);
  CYC(b_+209, b_+212); A = W8(wActiveGroup);
  CYC(b_+212, b_+213); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+213, b_+216); A = W8(wActiveRoom);
  CYC(b_+216, b_+217); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+217, b_+220); A = W8(wWarpDestPos);
  CYC(b_+220, b_+221); mem_wr(gb, HL, A);
  CYC(b_+221, b_+222); C = A;
  CALL_C(b_+222, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+225);
  if (!(F & FZ)) { CYCT(b_+225, b_+227); goto maybe_show_strange_force_text; }
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+229); mem_wr(gb, HL, 0xde);
  CYC(b_+229, b_+231); L = 0x4b;
  CALL_C(b_+231, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+234);

maybe_show_strange_force_text:
  CYC(b_+234, b_+237); A = W8(wSentBackByStrangeForce);
  CYC(b_+237, b_+238); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+238, b_+240);
  } else {
    CYC(b_+238, b_+240);
    CYC(b_+240, b_+243); SET_BC((SYM(linkState02__substate2) + 3));
    CALL_C(b_+243, showText_hook, SYM(showText), b_+246);
  }
  CYC(b_+246, b_+247); alu_xor(gb, A);
  CYC(b_+247, b_+250); W8(wLinkTimeWarpTile) = A;
  CYC(b_+250, b_+253); W8(wWarpTransition) = A;
  CYC(b_+253, b_+256); W8(wLinkCanPassNpcs) = A;
  CYC(b_+256, b_+259); W8(wMenuDisabled) = A;
  CYC(b_+259, b_+262); W8(wSentBackByStrangeForce) = A;
  CYC(b_+262, b_+265); W8(wcddf) = A;
  CYC(b_+265, b_+268); W8(wcde0) = A;
  CYC(b_+268, b_+270); E = 0x2b;
  CYC(b_+270, b_+272); A = 0x88;
  CYC(b_+272, b_+273); mem_wr(gb, DE, A);
  CALL_C(b_+273, updateLinkLocalRespawnPosition_hook, SYM(updateLinkLocalRespawnPosition), b_+276);
  CALL_C(b_+276, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+279);
  CYC(b_+279, b_+282);
  TAIL(initLinkStateAndAnimateStanding);

substate4:
  CALL_C(b_+282, itemDecCounter1_hook, SYM(itemDecCounter1), b_+285);
  if (!(F & FZ)) { CYCT(b_+285, b_+286); ret_effect(gb); return; }
  CYC(b_+285, b_+286);
  CYC(b_+286, b_+288); mem_wr(gb, HL, 0x78);
  CYCT(b_+288, b_+290);
  goto make_link_visible_and_play_sound;

substate5:
  CYC(b_+290, b_+293); push_effect(gb, b_+293);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(b_+293, b_+294); ret_effect(gb); return; }
  CYC(b_+293, b_+294);
  CYC(b_+294, b_+296); mem_wr(gb, HL, 0x10);
  CYC(b_+296, b_+299); push_effect(gb, b_+299);
  warp_transition6_create_destination_timewarp_animation(gb, sp0_);
  CYC(b_+299, b_+302);
  TAIL(itemIncSubstate);

substate6:
  CYC(b_+302, b_+305); push_effect(gb, b_+305);
  warp_transition6_flicker_visibility_and_dec_counter1(gb, sp0_);
  if (!(F & FZ)) { CYCT(b_+305, b_+306); ret_effect(gb); return; }
  CYC(b_+305, b_+306);
  CYC(b_+306, b_+308); mem_wr(gb, HL, 0x14);
  CALL_C(b_+308, objectSetInvisible_hook, SYM(objectSetInvisible), b_+311);
  CYC(b_+311, b_+314);
  TAIL(itemIncSubstate);

substate7:
  CALL_C(b_+314, itemDecCounter1_hook, SYM(itemDecCounter1), b_+317);
  if (!(F & FZ)) { CYCT(b_+317, b_+318); ret_effect(gb); return; }
  CYC(b_+317, b_+318);
  CALL_C(b_+318, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+321);
  CYC(b_+321, b_+322); C = L;
  CYC(b_+322, b_+325); SET_HL(wWarpDestGroup);
  CYC(b_+325, b_+328); A = W8(wActiveGroup);
  CYC(b_+328, b_+330); alu_xor(gb, 0x01);
  CYC(b_+330, b_+332); alu_or(gb, 0x80);
  CYC(b_+332, b_+333); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+333, b_+336); A = W8(wActiveRoom);
  CYC(b_+336, b_+337); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+337, b_+339); A = 0x06;
  CYC(b_+339, b_+340); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+340, b_+341); A = C;
  CYC(b_+341, b_+342); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+342, b_+343); A = alu_inc8(gb, A);
  CYC(b_+343, b_+346); W8(wLinkTimeWarpTile) = A;
  CYC(b_+346, b_+349); W8(wcddf) = A;
  CYC(b_+349, b_+351); A = 0x03;
  CYC(b_+351, b_+352); mem_wr(gb, HL, A);
  CYC(b_+352, b_+353); alu_xor(gb, A);
  CYC(b_+353, b_+356); W8(wScrollMode) = A;
  CYC(b_+356, b_+359); SET_HL(wSentBackByStrangeForce);
  CYC(b_+359, b_+360); A = mem_rd(gb, HL);
  CYC(b_+360, b_+361); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+361, b_+363);
  } else {
    CYC(b_+361, b_+363);
    CYC(b_+363, b_+364); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+364, b_+367); A = W8(wLinkStateParameter);
  CYC(b_+367, b_+369); alu_bit(gb, 4, A);
  if (F & FZ) {
    CYC(b_+369, b_+371);
    CALL_C(b_+371, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+374);
    CYC(b_+374, b_+376); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  } else {
    CYCT(b_+369, b_+371);
  }
  CYC(b_+376, b_+378); A = 0xd4;
  CALL_C(b_+378, playSound_b00_hook, SYM(playSound_b00), b_+381);
  CYC(b_+381, b_+384); SET_DE(w1Link);
  CYC(b_+384, b_+387);
  TAIL(objectDelete_de);
}

void linkState01_hook(GB *gb) {
  BASE(linkState01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); A = 0x80;
  CYC(b_+O(2), b_+OE(5)); W8(wForceLinkPushAnimation) = A;
  CYC(b_+O(5), b_+OE(8)); A = W8(wPaletteThread_mode);
  CYC(b_+O(8), b_+OE(9)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(9), b_+OE(10)); ret_effect(gb); return; }
  CYC(b_+O(9), b_+OE(10));
  CYC(b_+O(10), b_+OE(13)); A = W8(wScrollMode);
  CYC(b_+O(13), b_+OE(15)); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+O(15), b_+OE(16)); ret_effect(gb); return; }
  CYC(b_+O(15), b_+OE(16));
  CALL_C(b_+O(16), updateLinkDamageTaken_hook, SYM(updateLinkDamageTaken), b_+OE(19));
  CYC(b_+O(19), b_+OE(22)); A = W8(wLinkDeathTrigger);
  CYC(b_+O(22), b_+OE(23)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(23), b_+OE(26));
    TAIL(setLinkStateToDead);
  }
  CYC(b_+O(23), b_+OE(26));
  CALL_C(b_+O(26), checkLinkForceState_hook, SYM(checkLinkForceState), b_+OE(29));
  CALL_C(b_+O(29), retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+OE(32));
  CYC(b_+O(32), b_+OE(35)); A = W8(wDisabledObjects);
  CYC(b_+O(35), b_+OE(37)); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(b_+O(37), b_+OE(38)); ret_effect(gb); return; }
  CYC(b_+O(37), b_+OE(38));
  CALL_C(b_+O(38), decPegasusSeedCounter_hook, SYM(decPegasusSeedCounter), b_+OE(41));
  CYC(b_+O(41), b_+OE(44)); A = W8(w1Companion_id);
  if (!game_seasons) {      // Seasons has no Dimitri to check for here
    CYC(b_+44, b_+46); alu_cp(gb, 0x0a);
    if (F & FZ) { CYCT(b_+46, b_+48); goto interactions_done; }
    CYC(b_+46, b_+48);
  }
  CYC(b_+O(48), b_+OE(50)); alu_cp(gb, GV(0x13, 0x0a));
  if (F & FZ) { CYCT(b_+O(50), b_+OE(52)); goto interactions_done; }
  CYC(b_+O(50), b_+OE(52));
  CYC(b_+O(52), b_+OE(55)); A = W8(wLinkObjectIndex);
  CYC(b_+O(55), b_+OE(56)); alu_rrca(gb);
  if (F & FC) { CYCT(b_+O(56), b_+OE(57)); ret_effect(gb); return; }
  CYC(b_+O(56), b_+OE(57));
  CYC(b_+O(57), b_+OE(60)); A = W8(wLinkPlayingInstrument);
  CYC(b_+O(60), b_+OE(61)); B = A;
  CYC(b_+O(61), b_+OE(64)); A = W8(wLinkInAir);
  CYC(b_+O(64), b_+OE(65)); alu_or(gb, B);
  if (!(F & FZ)) { CYCT(b_+O(65), b_+OE(67)); goto interactions_done; }
  CYC(b_+O(65), b_+OE(67));
  CYC(b_+O(67), b_+OE(69)); E = 0x2d;
  CYC(b_+O(69), b_+OE(70)); A = mem_rd(gb, DE);
  CYC(b_+O(70), b_+OE(71)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(71), b_+OE(73)); goto interactions_done; }
  CYC(b_+O(71), b_+OE(73));
  CALL_C(b_+O(73), linkInteractWithAButtonSensitiveObjects_hook, SYM(linkInteractWithAButtonSensitiveObjects), b_+OE(76));
  if (F & FC) { CYCT(b_+O(76), b_+OE(77)); ret_effect(gb); return; }
  CYC(b_+O(76), b_+OE(77));
  CALL_C(b_+O(77), interactWithTileBeforeLink_b00_hook, SYM(interactWithTileBeforeLink_b00), b_+OE(80));
  if (F & FC) { CYCT(b_+O(80), b_+OE(81)); ret_effect(gb); return; }
  CYC(b_+O(80), b_+OE(81));

interactions_done:
  CYC(b_+O(81), b_+OE(82)); alu_xor(gb, A);
  CYC(b_+O(82), b_+OE(85)); W8(wForceLinkPushAnimation) = A;
  CYC(b_+O(85), b_+OE(88)); W8(wLinkPlayingInstrument) = A;
  CYC(b_+O(88), b_+OE(91)); A = W8(wTilesetFlags);
  CYC(b_+O(91), b_+OE(93)); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(b_+O(93), b_+OE(96));
    TAIL(linkState01_sidescroll);
  }
  CYC(b_+O(93), b_+OE(96));
  CALL_C(b_+O(96), linkApplyTileTypes_hook, SYM(linkApplyTileTypes), b_+OE(99));
  CALL_C(b_+O(99), checkAndUpdateLinkOnChest_hook, SYM(checkAndUpdateLinkOnChest), b_+OE(102));
  CALL_C(b_+O(102), checkUseItems_b00_hook, SYM(checkUseItems_b00), b_+OE(105));
  CYC(b_+O(105), b_+OE(108)); A = W8(wLinkPlayingInstrument);
  CYC(b_+O(108), b_+OE(109)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(109), b_+OE(110)); ret_effect(gb); return; }
  CYC(b_+O(109), b_+OE(110));
  CALL_C(b_+O(110), specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+OE(113));
  CALL_C(b_+O(113), linkUpdateKnockback_hook, SYM(linkUpdateKnockback), b_+OE(116));
  CYC(b_+O(116), b_+OE(119)); A = W8(wLinkSwimmingState);
  CYC(b_+O(119), b_+OE(121)); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+O(121), b_+OE(123)); goto update_in_air; }
  CYC(b_+O(121), b_+OE(123));
  CYC(b_+O(123), b_+OE(126)); A = W8(wMagnetGloveState);
  CYC(b_+O(126), b_+OE(128)); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+O(128), b_+OE(130)); goto update_in_air; }
  CYC(b_+O(128), b_+OE(130));
  CYC(b_+O(130), b_+OE(133)); A = W8(wLinkInAir);
  CYC(b_+O(133), b_+OE(134)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(134), b_+OE(136)); goto update_in_air; }
  CYC(b_+O(134), b_+OE(136));
  CYC(b_+O(136), b_+OE(139)); A = W8(wLinkGrabState);
  CYC(b_+O(139), b_+OE(140)); C = A;
  CYC(b_+O(140), b_+OE(143)); A = W8(wLinkImmobilized);
  CYC(b_+O(143), b_+OE(144)); alu_or(gb, C);
  if (!(F & FZ)) { CYCT(b_+O(144), b_+OE(146)); goto update_in_air; }
  CYC(b_+O(144), b_+OE(146));
  CALL_C(b_+O(146), checkLinkPushingAgainstBed_hook, SYM(checkLinkPushingAgainstBed), b_+OE(149));
  if (game_seasons) CALL_ROM(b_+S(145), SYM(checkLinkPushingAgainstTreeStump));
  CALL_C(b_+O(149), checkLinkJumpingOffCliff_hook, SYM(checkLinkJumpingOffCliff), b_+OE(152));

update_in_air:
  CALL_C(b_+O(152), linkUpdateInAir_hook, SYM(linkUpdateInAir), b_+OE(155));
  CYC(b_+O(155), b_+OE(158)); A = W8(wLinkInAir);
  CYC(b_+O(158), b_+OE(159)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(159), b_+OE(161)); goto not_in_air; }
  CYC(b_+O(159), b_+OE(161));
  CYC(b_+O(161), b_+OE(163)); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+O(163), b_+OE(165));
  } else {
    CYC(b_+O(163), b_+OE(165));
    CYC(b_+O(165), b_+OE(167)); E = 0x15;
    CYC(b_+O(167), b_+OE(168)); A = mem_rd(gb, DE);
    CYC(b_+O(168), b_+OE(170)); alu_bit(gb, 7, A);
    if (F & FZ) CALL_C_CC(b_+O(170), linkUpdateVelocity_hook, SYM(linkUpdateVelocity), b_+OE(173));
    else CYC(b_+O(170), b_+OE(173));
  }
  CYC(b_+O(173), b_+OE(176)); SET_HL(wcc95);
  CYC(b_+O(176), b_+OE(178)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CALL_C(b_+O(178), specialObjectSetAngleRelativeToVar38_hook, SYM(specialObjectSetAngleRelativeToVar38), b_+OE(181));
  CALL_C(b_+O(181), specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+OE(184));
  CYC(b_+O(184), b_+OE(187));
  TAIL(specialObjectAnimate);

not_in_air:
  CYC(b_+O(187), b_+OE(190)); A = W8(wMagnetGloveState);
  CYC(b_+O(190), b_+OE(192)); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+O(192), b_+OE(195));
    TAIL(animateLinkStanding);
  }
  CYC(b_+O(192), b_+OE(195));
  CYC(b_+O(195), b_+OE(197)); E = 0x2d;
  CYC(b_+O(197), b_+OE(198)); A = mem_rd(gb, DE);
  CYC(b_+O(198), b_+OE(199)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(199), b_+OE(202));
    TAIL(func_5631);
  }
  CYC(b_+O(199), b_+OE(202));
  CYC(b_+O(202), b_+OE(203)); H = D;
  CYC(b_+O(203), b_+OE(205)); L = 0x24;
  CYC(b_+O(205), b_+OE(207)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+O(207), b_+OE(210)); A = W8(wLinkSwimmingState);
  CYC(b_+O(210), b_+OE(211)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(211), b_+OE(214));
    TAIL(linkUpdateSwimming);
  }
  CYC(b_+O(211), b_+OE(214));
  CALL_C(b_+O(214), objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+OE(217));
  CYC(b_+O(217), b_+OE(220)); A = W8(wLinkObjectIndex);
  CYC(b_+O(220), b_+OE(221)); alu_rrca(gb);
  if (game_seasons) {       // no underwater state or companion text in Seasons: a companion goes to update_direction
    if (F & FC) { CYCT(b_+S(220), b_+S(222)); goto update_direction; }
    CYC(b_+S(220), b_+S(222));
    goto transformed_link;
  }
  if (!game_seasons) {
    if (!(F & FC)) { CYCT(b_+221, b_+223); goto check_underwater_transition; }
    CYC(b_+221, b_+223);
    CYC(b_+223, b_+226); A = W8(w1Companion_id);
    CYC(b_+226, b_+228); alu_or(gb, 0x13);
    if (F & FZ) { CYCT(b_+228, b_+230); goto update_direction_if_not_using_item; }
    CYC(b_+228, b_+230);
    CYCT(b_+230, b_+232);
    goto update_direction;

check_underwater_transition:
    CALL_C(b_+232, checkForUnderwaterTransition_hook, SYM(checkForUnderwaterTransition), b_+235);
  }
transformed_link:
  CYC(b_+O(235), b_+OE(238)); SET_HL(SYM(getTransformedLinkID));
  CYC(b_+O(238), b_+OE(240)); E = 0x06;
  CALL_C(b_+O(240), interBankCall_hook, 0x008a, b_+OE(243));
  CYC(b_+O(243), b_+OE(244)); A = B;
  CYC(b_+O(244), b_+OE(245)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(245), b_+OE(248));
    TAIL(setLinkIDOverride);
  }
  CYC(b_+O(245), b_+OE(248));
  if (!game_seasons) {      // mermaid suit: Ages only
    CYC(b_+248, b_+249); H = D;
    CYC(b_+249, b_+251); L = 0x2f;
    CYC(b_+251, b_+253); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+253, b_+255); goto ordinary_movement; }
    CYC(b_+253, b_+255);
    CALL_C(b_+255, linkUpdateVelocity_mermaidSuit_hook, SYM(linkUpdateVelocity__mermaidSuit), b_+258);
    CYCT(b_+258, b_+260);
    goto velocity_updated;
  }

ordinary_movement:
  CYC(b_+O(260), b_+OE(263)); A = W8(wLinkGrabState);
  CYC(b_+O(263), b_+OE(265)); alu_and(gb, 0x0f);
  CYC(b_+O(265), b_+OE(266)); A = alu_dec8(gb, A);
  CYC(b_+O(266), b_+OE(268)); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+O(268), b_+OE(270)); goto normal_movement; }
  CYC(b_+O(268), b_+OE(270));
  CYC(b_+O(270), b_+OE(273)); SET_HL(wIsTileSlippery);
  CYC(b_+O(273), b_+OE(275)); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+O(275), b_+OE(277)); goto normal_movement; }
  CYC(b_+O(275), b_+OE(277));
  CYC(b_+O(277), b_+OE(279)); C = 0x88;
  CALL_C(b_+O(279), updateLinkSpeed_withParam_hook, SYM(updateLinkSpeed_withParam), b_+OE(282));
  CALL_C(b_+O(282), linkUpdateVelocity_hook, SYM(linkUpdateVelocity), b_+OE(285));

velocity_updated:
  CYC(b_+O(285), b_+OE(288)); A = W8(wLinkAngle);
  CYC(b_+O(288), b_+OE(289)); alu_rlca(gb);
  CYC(b_+O(289), b_+OE(291)); C = 0x02;
  if (F & FC) { CYCT(b_+O(291), b_+OE(293)); goto update_movement; }
  CYC(b_+O(291), b_+OE(293));
  CYCT(b_+O(293), b_+OE(295));
  goto walking;

normal_movement:
  CYC(b_+O(295), b_+OE(298)); A = W8(wcc95);
  CYC(b_+O(298), b_+OE(299)); B = A;
  CYC(b_+O(299), b_+OE(301)); E = 0x09;
  CYC(b_+O(301), b_+OE(304)); A = W8(wLinkAngle);
  CYC(b_+O(304), b_+OE(305)); mem_wr(gb, DE, A);
  CYC(b_+O(305), b_+OE(306)); alu_or(gb, B);
  CYC(b_+O(306), b_+OE(307)); alu_rlca(gb);
  CYC(b_+O(307), b_+OE(309)); C = 0x00;
  if (F & FC) { CYCT(b_+O(309), b_+OE(311)); goto update_movement; }
  CYC(b_+O(309), b_+OE(311));
  CYC(b_+O(311), b_+OE(313)); C = 0x01;
  CYC(b_+O(313), b_+OE(316)); A = W8(wLinkImmobilized);
  CYC(b_+O(316), b_+OE(317)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(317), b_+OE(319)); goto update_movement; }
  CYC(b_+O(317), b_+OE(319));
  CALL_C(b_+O(319), updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+OE(322));

walking:
  CYC(b_+O(322), b_+OE(324)); C = 0x07;

update_movement:
  CALL_C(b_+O(324), linkUpdateMovement_hook, SYM(linkUpdateMovement), b_+OE(327));

update_direction_if_not_using_item:
  CYC(b_+O(327), b_+OE(330)); A = W8(wLinkTurningDisabled);
  CYC(b_+O(330), b_+OE(331)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(331), b_+OE(332)); ret_effect(gb); return; }
  CYC(b_+O(331), b_+OE(332));

update_direction:
  CYC(b_+O(332), b_+OE(335));
  TAIL(updateLinkDirectionFromAngle);
}

void specialObjectCode_link_hook(GB *gb) {
  BASE(specialObjectCode_link);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == SYM(linkState00) && hook_is(gb, SYM(linkState00), linkState00_hook)) { linkState00_hook(gb); return; }
    else if (jt_ == SYM(linkState01) && hook_is(gb, SYM(linkState01), linkState01_hook)) { linkState01_hook(gb); return; }
    else if (jt_ == SYM(linkState02) && hook_is(gb, SYM(linkState02), linkState02_hook)) { linkState02_hook(gb); return; }
    else if (jt_ == SYM(linkState03) && hook_is(gb, SYM(linkState03), linkState03_hook)) { linkState03_hook(gb); return; }
    else if (jt_ == SYM(linkState04) && hook_is(gb, SYM(linkState04), linkState04_hook)) { linkState04_hook(gb); return; }
    else if (jt_ == SYM(linkState05) && hook_is(gb, SYM(linkState05), linkState05_hook)) { linkState05_hook(gb); return; }
    else if (jt_ == SYM(linkState06) && hook_is(gb, SYM(linkState06), linkState06_hook)) { linkState06_hook(gb); return; }
    else if (jt_ == SYM(linkState07) && hook_is(gb, SYM(linkState07), linkState07_hook)) { linkState07_hook(gb); return; }
    else if (jt_ == SYM(linkState08) && hook_is(gb, SYM(linkState08), linkState08_hook)) { linkState08_hook(gb); return; }
    else if (jt_ == SYM(linkState09) && hook_is(gb, SYM(linkState09), linkState09_hook)) { linkState09_hook(gb); return; }
    else if (jt_ == (SYM(linkState00) + 28)) { linkState0a_hook(gb); return; }
    else if (jt_ == SYM(linkState0b) && hook_is(gb, SYM(linkState0b), linkState0b_hook)) { linkState0b_hook(gb); return; }
    else if (jt_ == SYM(linkState0c) && hook_is(gb, SYM(linkState0c), linkState0c_hook)) { linkState0c_hook(gb); return; }
    else if (jt_ == SYM(linkState0d) && hook_is(gb, SYM(linkState0d), linkState0d_hook)) { linkState0d_hook(gb); return; }
    else if (jt_ == SYM(linkState0e) && hook_is(gb, SYM(linkState0e), linkState0e_hook)) { linkState0e_hook(gb); return; }
    else if (jt_ == SYM(linkState0f) && hook_is(gb, SYM(linkState0f), linkState0f_hook)) { linkState0f_hook(gb); return; }
    else if (jt_ == SYM(linkState11) && hook_is(gb, SYM(linkState11), linkState11_hook)) { linkState11_hook(gb); return; }
    else if (jt_ == SYM(linkState12) && hook_is(gb, SYM(linkState12), linkState12_hook)) { linkState12_hook(gb); return; }
    else if (jt_ == SYM(linkState13) && hook_is(gb, SYM(linkState13), linkState13_hook)) { linkState13_hook(gb); return; }
    else if (jt_ == SYM(linkState14) && hook_is(gb, SYM(linkState14), linkState14_hook)) { linkState14_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void linkState02_hook(GB *gb) {
  BASE(linkState02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0xff;
  CYC(b_+2, b_+5); W8(wGameKeysPressed) = A;
  CYC(b_+5, b_+7); A = 0x80;
  CYC(b_+7, b_+10); W8(wForceLinkPushAnimation) = A;
  CYC(b_+10, b_+12); E = 0x05;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+26) { goto substate0; }
    else if (jt_ == b_+81) { goto substate1; }
    else if (jt_ == b_+148) { goto substate2; }
    else if (jt_ == b_+207) { goto substate3; }
    else if (jt_ == b_+218) { goto substate4; }
    else if (jt_ == b_+232) { goto substate5; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+26, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+29);
  CYC(b_+29, b_+32); A = W8(wLinkStateParameter);
  CYC(b_+32, b_+33); push_effect(gb, b_+33);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+53) { goto parameter_fall_down_hole; }
    else if (jt_ == b_+56) { goto parameter_fall_without_centering; }
    else if (jt_ == b_+126) { goto respawn; }
    else if (jt_ == b_+61) { goto parameter_3; }
    else if (jt_ == b_+43) { goto parameter_drown; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

parameter_drown:
  CYC(b_+43, b_+45); E = 0x05;
  CYC(b_+45, b_+47); A = 0x05;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); A = 0x0a;
  CYC(b_+50, b_+53);
  TAIL(specialObjectSetAnimation);

parameter_fall_down_hole:
  CALL_C(b_+53, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+56);

parameter_fall_without_centering:
  CALL_C(b_+56, itemIncSubstate_hook, SYM(itemIncSubstate), b_+59);
  CYC(b_+59, b_+61);
  goto start_falling;

parameter_3:
  CYC(b_+61, b_+63); E = 0x05;
  CYC(b_+63, b_+65); A = 0x04;
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);

start_falling:
  CYC(b_+66, b_+67); H = D;
  CYC(b_+67, b_+69); L = 0x24;
  CYC(b_+69, b_+71); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+71, b_+73); A = 0x0d;
  CALL_C(b_+73, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+76);
  CYC(b_+76, b_+78); A = 0x65;
  CYC(b_+78, b_+81);
  TAIL(playSound_b00);

substate1:
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = 0x21;
  CYC(b_+84, b_+86); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+86, b_+89);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+92); A = W8(wActiveTileType);
  CYC(b_+92, b_+94); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+94, b_+96); goto respawn; }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+99); A = W8(wActiveGroup);
  CYC(b_+99, b_+101); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+101, b_+103); goto ordinary_hole_warp; }
  CYC(b_+101, b_+103);
  CYC(b_+103, b_+106); A = W8(wActiveRoom);
  CYC(b_+106, b_+108); alu_cp(gb, 0x9f);
  if (!(F & FZ)) { CYCT(b_+108, b_+110); goto ordinary_hole_warp; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+113); SET_HL((SYM(cliffTilesTable__sidescrolling) + 94));
  CYC(b_+113, b_+115); E = 0x01;
  CYC(b_+115, b_+118);
  interBankCall_hook(gb);
  return;

ordinary_hole_warp:
  CYC(b_+118, b_+121); SET_HL((SYM(warpTransition3) + 45));
  CYC(b_+121, b_+123); E = 0x01;
  CYC(b_+123, b_+126);
  interBankCall_hook(gb);
  return;

respawn:
  CALL_C(b_+126, specialObjectSetCoordinatesToRespawnYX_hook, SYM(specialObjectSetCoordinatesToRespawnYX), b_+129);
  CYC(b_+129, b_+131); L = 0x05;
  CYC(b_+131, b_+133); A = 0x02;
  CYC(b_+133, b_+134); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+134, b_+135); mem_wr(gb, HL, A);
  CALL_C(b_+135, specialObjectTryToBreakTile_source05_hook, SYM(specialObjectTryToBreakTile_source05), b_+138);
  CALL_C(b_+138, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+141);
  CYC(b_+141, b_+142); A = L;
  CYC(b_+142, b_+145); W8(wEnteredWarpPosition) = A;
  CYC(b_+145, b_+148);
  TAIL(objectSetInvisible);

substate2:
  CYC(b_+148, b_+149); H = D;
  CYC(b_+149, b_+151); L = 0x06;
  CYC(b_+151, b_+154); A = W8(wScrollMode);
  CYC(b_+154, b_+156); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYC(b_+156, b_+158);
    CYC(b_+158, b_+160); mem_wr(gb, HL, 0x04);
    CYC(b_+160, b_+161); ret_effect(gb);
    return;
  }
  CYCT(b_+156, b_+158);
  CYC(b_+161, b_+162); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+162, b_+163); ret_effect(gb); return; }
  CYC(b_+162, b_+163);
  CYC(b_+163, b_+164); alu_xor(gb, A);
  CYC(b_+164, b_+167); W8(wLinkInAir) = A;
  CYC(b_+167, b_+170); W8(wLinkSwimmingState) = A;
  CYC(b_+170, b_+172); A = 0x1c;
  CALL_C(b_+172, cpActiveRing_hook, SYM(cpActiveRing), b_+175);
  CYC(b_+175, b_+177); A = 0xfc;
  if (!(F & FZ)) {
    CYCT(b_+177, b_+179);
  } else {
    CYC(b_+177, b_+179);
    CYC(b_+179, b_+181); A = alu_sra(gb, A);
  }
  CALL_C(b_+181, itemIncSubstate_hook, SYM(itemIncSubstate), b_+184);
  CYC(b_+184, b_+186); L = 0x25;
  CYC(b_+186, b_+187); mem_wr(gb, HL, A);
  CYC(b_+187, b_+189); L = 0x2b;
  CYC(b_+189, b_+191); mem_wr(gb, HL, 0x3c);
  CYC(b_+191, b_+193); L = 0x06;
  CYC(b_+193, b_+195); mem_wr(gb, HL, 0x10);
  CALL_C(b_+195, linkApplyDamage_b00_hook, SYM(linkApplyDamage_b00), b_+198);
  CALL_C(b_+198, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+201);
  CALL_C(b_+201, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+204);
  CYC(b_+204, b_+207);
  TAIL(animateLinkStanding);

substate3:
  CALL_C(b_+207, itemDecCounter1_hook, SYM(itemDecCounter1), b_+210);
  if (!(F & FZ)) { CYCT(b_+210, b_+211); ret_effect(gb); return; }
  CYC(b_+210, b_+211);
  CYC(b_+211, b_+213); L = 0x24;
  CYC(b_+213, b_+215); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+215, b_+218);
  TAIL(initLinkStateAndAnimateStanding);

substate4:
  CYC(b_+218, b_+219); H = D;
  CYC(b_+219, b_+221); L = 0x21;
  CYC(b_+221, b_+223); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+223, b_+226);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+223, b_+226);
  CALL_C(b_+226, objectSetInvisible_hook, SYM(objectSetInvisible), b_+229);
  CYC(b_+229, b_+232);
  TAIL(checkLinkForceState);

substate5:
  CYC(b_+232, b_+234); E = 0x21;
  CYC(b_+234, b_+235); A = mem_rd(gb, DE);
  CYC(b_+235, b_+236); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+236, b_+239);
    TAIL(specialObjectAnimate);
  }
  CYC(b_+236, b_+239);
  CYC(b_+239, b_+241);
  goto respawn;
}

void linkState0c_hook(GB *gb) {
  BASE(linkState0c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+58) { goto done; }
    else if (jt_ == b_+33) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); W8(wWarpsDisabled) = A;
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+19); E = 0x24;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x00;
  CYC(b_+22, b_+25); W8(wScrollMode) = A;
  CALL_C(b_+25, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+28);
  CYC(b_+28, b_+30); A = 0x67;
  CYC(b_+30, b_+33);
  TAIL(playSound_b00);

substate2:
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37); W8(wWarpsDisabled) = A;
  CYC(b_+37, b_+40); SET_HL(wWarpDestGroup);
  CYC(b_+40, b_+43); A = W8(wActiveGroup);
  CYC(b_+43, b_+45); alu_or(gb, 0x80);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+46, b_+49); A = W8(wDungeonWallmasterDestRoom);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+52); A = 0x05;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+55); A = 0x87;
  CYC(b_+55, b_+56); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x03);

done:
  CYC(b_+58, b_+59); ret_effect(gb);
}

static void link_state13_substate1(GB *gb, uint16_t sp0_) {
  BASE(linkState13);
  CYC(b_+35, b_+37); C = 0x40;
  CALL_C(b_+37, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+40);
  CYC(b_+40, b_+43); A = W8(wcc50);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+44, b_+46);
    goto update_counter;
  }
  CYC(b_+44, b_+46);
  CALL_C(b_+46, updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+49);
  CYC(b_+49, b_+51); L = 0x2a;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+53); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto restore; }
  CYC(b_+53, b_+55);

update_counter:
  CYC(b_+55, b_+57); C = 0x01;
  CYC(b_+57, b_+60); A = W8(wGameKeysJustPressed);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+61, b_+63);
  } else {
    CYC(b_+61, b_+63);
    CYC(b_+63, b_+65); C = 0x04;
  }
  CYC(b_+65, b_+67); L = 0x06;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); alu_sub(gb, C);
  CYC(b_+69, b_+70); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+70, b_+71); ret_effect(gb); return; }
  CYC(b_+70, b_+71);

restore:
  CYC(b_+71, b_+73); L = 0x1b;
  CYC(b_+73, b_+75); A = 0x08;
  CYC(b_+75, b_+76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+76, b_+77); mem_wr(gb, HL, A);
  CYC(b_+77, b_+79); L = 0x2d;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x00);
  CYC(b_+81, b_+82); alu_xor(gb, A);
  CYC(b_+82, b_+85); W8(wLinkForceState) = A;
  CYC(b_+85, b_+88);
  initLinkStateAndAnimateStanding_hook(gb);
}

void linkState13_hook(GB *gb) {
  BASE(linkState13);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x80;
  CYC(b_+2, b_+5); W8(wForceLinkPushAnimation) = A;
  CYC(b_+5, b_+7); E = 0x05;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+13) { break; }
    else if (jt_ == b_+35) { link_state13_substate1(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+13, itemIncSubstate_hook, SYM(itemIncSubstate), b_+16);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0xb4);
  CYC(b_+19, b_+21); L = 0x1b;
  CYC(b_+21, b_+23); A = 0x0f;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); A = 0x7f;
  CALL_C(b_+27, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+30);
  CYC(b_+30, b_+31); alu_xor(gb, A);
  CYC(b_+31, b_+34); W8(wcc50) = A;
  CYC(b_+34, b_+35); ret_effect(gb);
}

void linkState14_hook(GB *gb) {
  BASE(linkState14);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == SYM(linkState13__substate1)) { link_state13_substate1(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
  CALL_C(b_+8, itemIncSubstate_hook, SYM(itemIncSubstate), b_+11);
  CYC(b_+11, b_+13); L = 0x06;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+15, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+18);
  CYC(b_+18, b_+21); A = W8(wcc50);
  CYC(b_+21, b_+22); alu_or(gb, A);
  CYC(b_+22, b_+24); A = 0x02;
  if (F & FZ) {
    CYCT(b_+24, b_+26);
  } else {
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+28); A = 0x10;
  }
  CYC(b_+28, b_+31);
  TAIL(specialObjectSetAnimation);
}

void linkState0d_hook(GB *gb) {
  BASE(linkState0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x80;
  CYC(b_+2, b_+5); W8(wcc92) = A;
  CYC(b_+5, b_+7); E = 0x05;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+19) { goto substate0; }
    else if (jt_ == SYM(updateLinkDamageTaken) && hook_is(gb, SYM(updateLinkDamageTaken), updateLinkDamageTaken_hook)) { updateLinkDamageTaken_hook(gb); return; }
    else if (jt_ == b_+32) { goto substate2; }
    else if (jt_ == b_+62) { goto substate3; }
    else if (jt_ == b_+81) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+25); W8(wWarpsDisabled) = A;
  CYC(b_+25, b_+27); E = 0x30;
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+32);
  TAIL(linkCancelAllItemUsage);

substate2:
  CYC(b_+32, b_+34); A = 0x03;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+38); L = 0x06;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x1e);
  CYC(b_+40, b_+42); L = 0x14;
  CYC(b_+42, b_+44); A = 0x20;
  CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0xfe);
  CYC(b_+47, b_+49); L = 0x08;
  CYC(b_+49, b_+50); alu_xor(gb, A);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x10);
  CYC(b_+53, b_+55); L = 0x10;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x3c);
  CYC(b_+57, b_+59); A = 0x03;
  CYC(b_+59, b_+62);
  TAIL(specialObjectSetAnimation);

substate3:
  CALL_C(b_+62, itemDecCounter1_hook, SYM(itemDecCounter1), b_+65);
  if (F & FZ) { CYCT(b_+65, b_+67); goto restore_normal; }
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); C = 0x20;
  CALL_C(b_+69, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+72);
  CALL_C(b_+72, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+75);
  CALL_C(b_+75, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+78);
  CYC(b_+78, b_+81);
  TAIL(specialObjectAnimate);

substate4:
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = 0x2b;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x94);

restore_normal:
  CYC(b_+86, b_+87); alu_xor(gb, A);
  CYC(b_+87, b_+90); W8(wWarpsDisabled) = A;
  CYC(b_+90, b_+93);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState05_hook(GB *gb) {
  BASE(linkState05);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+40) { goto substate1; }
    else if (jt_ == b_+61) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+10, itemIncSubstate_hook, SYM(itemIncSubstate), b_+13);
  CYC(b_+13, b_+15); L = 0x10;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x14);
  CYC(b_+17, b_+19); L = GV(0x18, 0x13);
  CYC(b_+19, b_+21); A = 0x02;
  CALL_C(b_+21, specialObjectSetVar37AndVar38_hook, SYM(specialObjectSetVar37AndVar38), b_+24);
  CYC(b_+24, b_+27); SET_BC(0xfe80);
  CALL_C(b_+27, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+30);
  CYC(b_+30, b_+32); A = 0x81;
  CYC(b_+32, b_+35); W8(wLinkInAir) = A;
  CYC(b_+35, b_+37); A = 0x2f;
  CYC(b_+37, b_+40);
  TAIL(specialObjectSetAnimation);

substate1:
  CALL_C(b_+40, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+43);
  CALL_C(b_+43, specialObjectSetAngleRelativeToVar38_hook, SYM(specialObjectSetAngleRelativeToVar38), b_+46);
  CALL_C(b_+46, objectApplySpeed_hook, SYM(objectApplySpeed), b_+49);
  CYC(b_+49, b_+51); C = 0x20;
  CALL_C(b_+51, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+55); ret_effect(gb); return; }
  CYC(b_+54, b_+55);
  CALL_C(b_+55, itemIncSubstate_hook, SYM(itemIncSubstate), b_+58);
  CYC(b_+58, b_+61);
  TAIL(specialObjectSetPositionToVar38IfSet);

substate2:
  CALL_C(b_+61, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+64);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = 0x21;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x00);
  CYC(b_+70, b_+71); push_effect(gb, b_+71);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+81) { goto anim_parameter1; }
    else if (jt_ == b_+90) { goto anim_parameter2; }
    else if (jt_ == b_+95) { goto anim_parameter0; }
    else if (jt_ == b_+96) { goto anim_parameter3; }
    else if (jt_ == b_+99) { goto anim_parameter4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

anim_parameter1:
  CALL_C(b_+81, darkenRoomLightly_hook, SYM(darkenRoomLightly), b_+84);
  CYC(b_+84, b_+86); A = 0x06;
  CYC(b_+86, b_+89); W8(wPaletteThread_updateRate) = A;
  CYC(b_+89, b_+90); ret_effect(gb);
  return;

anim_parameter2:
  CYC(b_+90, b_+93); SET_HL(wLinkMaxHealth);
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+94, b_+95); mem_wr(gb, HL, A);

anim_parameter0:
  CYC(b_+95, b_+96); ret_effect(gb);
  return;

anim_parameter3:
  CYC(b_+96, b_+99);
  TAIL(brightenRoom);

anim_parameter4:
  CYC(b_+99, b_+102); SET_BC(0xfe80);
  CALL_C(b_+102, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+105);
  CYC(b_+105, b_+107); L = 0x08;
  CYC(b_+107, b_+109); mem_wr(gb, HL, GV(0x03, 0x01));
  CYC(b_+109, b_+110); L = alu_inc8(gb, L);
  CYC(b_+110, b_+112); mem_wr(gb, HL, GV(0x18, 0x08));
  CYC(b_+112, b_+114); L = 0x10;
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x14);
  CYC(b_+116, b_+118); A = 0x81;
  CYC(b_+118, b_+121); W8(wLinkInAir) = A;
  CYC(b_+121, b_+124);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState06_hook(GB *gb) {
  BASE(linkState06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+12) { goto substate0; }
    else if (jt_ == b_+38) { goto substate1; }
    else if (jt_ == b_+57) { goto substate2; }
    else if (jt_ == b_+90) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = 0x06;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x08);
  CYC(b_+20, b_+22); L = 0x10;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x50);
  CYC(b_+24, b_+26); L = 0x09;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);
  CYC(b_+28, b_+30); A = 0x81;
  CYC(b_+30, b_+33); W8(wLinkInAir) = A;
  CYC(b_+33, b_+35); A = 0x53;
  CALL_C(b_+35, playSound_b00_hook, SYM(playSound_b00), b_+38);

substate1:
  CALL_C(b_+38, specialObjectUpdatePositionWithoutTileEdgeAdjust_hook, SYM(specialObjectUpdatePositionWithoutTileEdgeAdjust), b_+41);
  CALL_C(b_+41, itemDecCounter1_hook, SYM(itemDecCounter1), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+45); ret_effect(gb); return; }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); L = 0x05;
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+48, b_+50); L = 0x08;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x00);
  CYC(b_+52, b_+54); A = 0x04;
  CALL_C(b_+54, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+57);

substate2:
  CALL_C(b_+57, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+60);
  CYC(b_+60, b_+63); A = W8(wScrollMode);
  CYC(b_+63, b_+65); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+65, b_+66); ret_effect(gb); return; }
  CYC(b_+65, b_+66);
  CALL_C(b_+66, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+69);
  if (F & FC) {
    CYCT(b_+69, b_+72);
    TAIL(specialObjectUpdatePositionWithoutTileEdgeAdjust);
  }
  CYC(b_+69, b_+72);
  CYC(b_+72, b_+75); SET_BC(0xfe00);
  CALL_C(b_+75, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+78);
  CYC(b_+78, b_+80); L = 0x05;
  CYC(b_+80, b_+81); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+81, b_+83); L = 0x10;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x0a);
  CYC(b_+85, b_+87); A = 0x18;
  CALL_C(b_+87, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+90);

substate3:
  CALL_C(b_+90, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+93);
  CALL_C(b_+93, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+96);
  CALL_C(b_+96, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+99);
  CYC(b_+99, b_+101); C = 0x18;
  CALL_C(b_+101, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+105); ret_effect(gb); return; }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+106); alu_xor(gb, A);
  CYC(b_+106, b_+109); W8(wLinkInAir) = A;
  CYC(b_+109, b_+112); W8(wWarpsDisabled) = A;
  CYC(b_+112, b_+115);
  TAIL(initLinkStateAndAnimateStanding);
}

void linkState09_hook(GB *gb) {
  BASE(linkState09);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+16) { goto substate0; }
    else if (jt_ == b_+34) { goto substate1; }
    else if (jt_ == b_+47) { goto substate2; }
    else if (jt_ == b_+60) { goto substate3; }
    else if (jt_ == b_+84) { goto substate4; }
    else if (jt_ == b_+106) { goto substate5; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CALL_C(b_+16, itemIncSubstate_hook, SYM(itemIncSubstate), b_+19);
  CYC(b_+19, b_+21); L = 0x10;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x28);
  CYC(b_+23, b_+25); L = 0x08;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x03);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x08);
  CYC(b_+30, b_+32); L = 0x06;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x0c);

substate1:
  CALL_C(b_+34, itemDecCounter1_hook, SYM(itemDecCounter1), b_+37);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto animate; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0c);
  CYC(b_+41, b_+42); L = E;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+43, b_+45); L = 0x09;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x18);

substate2:
  CALL_C(b_+47, itemDecCounter1_hook, SYM(itemDecCounter1), b_+50);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto animate; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x32);
  CYC(b_+54, b_+55); L = E;
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+56, b_+58); L = 0x08;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x02);

substate3:
  CALL_C(b_+60, itemDecCounter1_hook, SYM(itemDecCounter1), b_+63);
  if (!(F & FZ)) { CYCT(b_+63, b_+64); ret_effect(gb); return; }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+65); L = E;
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+66, b_+68); L = 0x08;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x00);
  CYC(b_+70, b_+71); L = alu_inc8(gb, L);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x10);
  CYC(b_+73, b_+75); L = 0x06;
  CYC(b_+75, b_+77); A = 0x1e;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A);
  CYC(b_+78, b_+81); SET_BC(0xf4f8);
  CYC(b_+81, b_+84);
  TAIL(objectCreateExclamationMark);

substate4:
  CALL_C(b_+84, itemDecCounter1_hook, SYM(itemDecCounter1), b_+87);
  if (!(F & FZ)) { CYCT(b_+87, b_+88); ret_effect(gb); return; }
  CYC(b_+87, b_+88);
  CYC(b_+88, b_+89); L = E;
  CYC(b_+89, b_+90); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+90, b_+93); SET_BC(0xfe80);
  CALL_C(b_+93, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+96);
  CYC(b_+96, b_+98); A = 0x18;
  CALL_C(b_+98, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+101);
  CYC(b_+101, b_+103); A = 0x53;
  CYC(b_+103, b_+106);
  TAIL(playSound_b00);

substate5:
  CYC(b_+106, b_+108); C = 0x18;
  CALL_C(b_+108, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+111);
  if (!(F & FZ)) { CYCT(b_+111, b_+113); goto animate; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+115); L = 0x05;
  CYC(b_+115, b_+116); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x08);
  CYC(b_+118, b_+119); ret_effect(gb);
  return;

animate:
  CALL_C(b_+119, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+122);
  CYC(b_+122, b_+125);
  TAIL(specialObjectUpdatePositionWithoutTileEdgeAdjust);
}

void linkState11_hook(GB *gb) {
  BASE(linkState11);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x05;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (link_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+38) { goto substate1; }
    else if (jt_ == b_+53) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, linkCancelAllItemUsage_hook, SYM(linkCancelAllItemUsage), b_+16);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+19); E = 0x24;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x4e;
  CALL_C(b_+22, playSound_b00_hook, SYM(playSound_b00), b_+25);
  CYC(b_+25, b_+28); A = W8(wcc50);
  CYC(b_+28, b_+30); alu_and(gb, 0x7f);
  CYC(b_+30, b_+32); A = 0x06;
  if (F & FZ) {
    CYCT(b_+32, b_+34);
  } else {
    CYC(b_+32, b_+34);
    CYC(b_+34, b_+35); A = alu_inc8(gb, A);
  }
  CALL_C(b_+35, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+38);

substate1:
  CALL_C(b_+38, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+41);
  CYC(b_+41, b_+43); E = 0x21;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+45, b_+46); ret_effect(gb); return; }
  CYC(b_+45, b_+46);
  CALL_C(b_+46, itemIncSubstate_hook, SYM(itemIncSubstate), b_+49);
  CYC(b_+49, b_+51); L = 0x06;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x14);

substate2:
  CALL_C(b_+53, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+56);
  CYC(b_+56, b_+59); A = W8(wFrameCounter);
  CYC(b_+59, b_+60); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+60, b_+63);
    TAIL(objectSetInvisible);
  }
  CYC(b_+60, b_+63);
  CALL_C(b_+63, objectSetVisible_hook, SYM(objectSetVisible), b_+66);
  CALL_C(b_+66, itemDecCounter1_hook, SYM(itemDecCounter1), b_+69);
  if (!(F & FZ)) { CYCT(b_+69, b_+70); ret_effect(gb); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73); A = W8(wcc50);
  CYC(b_+73, b_+75); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+75, b_+77); goto force_state; }
  CYC(b_+75, b_+77);
  CALL_C(b_+77, respawnLink_hook, SYM(respawnLink), b_+80);
  CYC(b_+80, b_+82);
  TAIL(checkLinkForceState);

force_state:
  CYC(b_+82, b_+84); A = 0x03;
  CYC(b_+84, b_+87); W8(wLinkForceState) = A;
  CYC(b_+87, b_+89);
  TAIL(checkLinkForceState);
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
  BASE(calculateAdjacentWallsBitset);
  AGES_ONLY();
  CYC(b_+79, b_+80); A = B;
  CYC(b_+80, b_+82); alu_and(gb, 0xf0);
  CYC(b_+82, b_+83); L = A;
  CYC(b_+83, b_+84); A = C;
  CYC(b_+84, b_+86); A = alu_swap(gb, A);
  CYC(b_+86, b_+88); alu_and(gb, 0x0f);
  CYC(b_+88, b_+89); alu_or(gb, L);
  CYC(b_+89, b_+90); L = A;
  CYC(b_+90, b_+92); H = (uint8_t)(wRoomCollisions >> 8);
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); alu_cp(gb, 0x10);
  if (F & FC) {
    CYCT(b_+95, b_+97);
    goto simple_collision;
  }
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); alu_and(gb, 0x0f);
  CYC(b_+99, b_+102); SET_HL(adjacentWallSpecialCollisions_bank05);
  CYC(b_+102, b_+103); link_add_a_to_hl(gb, b_+103);
  CYC(b_+103, b_+104); E = mem_rd(gb, HL);
  CYC(b_+104, b_+106); alu_cp(gb, 0x08);
  CYC(b_+106, b_+107); A = B;
  if (!(F & FC)) {
    CYCT(b_+107, b_+109);
  } else {
    CYC(b_+107, b_+109);
    CYC(b_+109, b_+110); A = C;
  }
  CYC(b_+110, b_+111); alu_rrca(gb);
  CYC(b_+111, b_+113); alu_and(gb, 0x07);
  CYC(b_+113, b_+116); SET_HL(bitTable);
  CYC(b_+116, b_+117); alu_add(gb, L);
  CYC(b_+117, b_+118); L = A;
  CYC(b_+118, b_+119); A = mem_rd(gb, HL);
  CYC(b_+119, b_+120); alu_and(gb, E);
  if (F & FZ) { CYCT(b_+120, b_+121); ret_effect(gb); return; }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+122); alu_scf(gb);
  CYC(b_+122, b_+123); ret_effect(gb); return;

simple_collision:
  CYC(b_+139, b_+141); alu_bit(gb, 3, B);
  if (!(F & FZ)) {
    CYCT(b_+141, b_+143);
  } else {
    CYC(b_+141, b_+143);
    CYC(b_+143, b_+144); alu_rrca(gb);
    CYC(b_+144, b_+145); alu_rrca(gb);
  }
  CYC(b_+145, b_+147); alu_bit(gb, 3, C);
  if (!(F & FZ)) {
    CYCT(b_+147, b_+149);
  } else {
    CYC(b_+147, b_+149);
    CYC(b_+149, b_+150); alu_rrca(gb);
  }
  CYC(b_+150, b_+151); alu_rrca(gb);
  CYC(b_+151, b_+152); ret_effect(gb);
}

void calculateAdjacentWallsBitset_hook(GB *gb) {
  BASE(calculateAdjacentWallsBitset);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); A = 0x01;
  CYC(b_+O(2), b_+OE(4)); H8(hFF8B) = A;
  CYC(b_+O(4), b_+OE(7)); SET_HL(adjacentWallOverworldOffsets_bank05);
  CYC(b_+O(7), b_+OE(10)); A = W8(wTilesetFlags);
  CYC(b_+O(10), b_+OE(12)); alu_and(gb, 0x20);
  if (F & FZ) {
    CYCT(b_+O(12), b_+OE(14));
  } else {
    CYC(b_+O(12), b_+OE(14));
    CYC(b_+O(14), b_+OE(17)); SET_HL(adjacentWallSidescrollOffsets_bank05);
  }

  for (;;) {
    CYC(b_+O(17), b_+OE(18)); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+O(18), b_+OE(19)); alu_add(gb, B);
    CYC(b_+O(19), b_+OE(20)); B = A;
    CYC(b_+O(20), b_+OE(21)); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+O(21), b_+OE(22)); alu_add(gb, C);
    CYC(b_+O(22), b_+OE(23)); C = A;
    CYC(b_+O(23), b_+OE(24)); push_effect(gb, HL);
    if (!game_seasons) {    // raised floors: Ages only
      CYC(b_+24, b_+27); A = W8(wLinkRaisedFloorOffset);
      CYC(b_+27, b_+28); alu_or(gb, A);
      if (F & FZ) {
        CYCT(b_+28, b_+30);
        CALL_C(b_+35, checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+38);
      } else {
        CYC(b_+28, b_+30);
        CYC(b_+30, b_+33); push_effect(gb, b_+33);
        link_check_tile_collision_at_allow_raised_floor(gb);
        CYC(b_+33, b_+35);
      }
    } else {
      CALL_C(b_+O(35), checkTileCollisionAt_allowHoles_hook, SYM(checkTileCollisionAt_allowHoles), b_+OE(38));
    }
    CYC(b_+O(38), b_+OE(39)); SET_HL(pop_effect(gb));
    CYC(b_+O(39), b_+OE(41)); A = H8(hFF8B);
    CYC(b_+O(41), b_+OE(42)); alu_rla(gb);
    CYC(b_+O(42), b_+OE(44)); H8(hFF8B) = A;
    if (!(F & FC)) {
      CYCT(b_+O(44), b_+OE(46));
      continue;
    }
    CYC(b_+O(44), b_+OE(46));
    CYC(b_+O(46), b_+OE(47)); ret_effect(gb);
    return;
  }
}

static void special_object_tile_edge_adjust(GB *gb) {
  BASE(specialObjectUpdatePositionGivenVelocity);
  CYC(b_+112, b_+113); A = C;
  CYC(b_+113, b_+116); SET_HL(slideAngleTable_bank00);
  CYC(b_+116, b_+117); link_add_a_to_hl(gb, b_+117);
  CYC(b_+117, b_+118); A = mem_rd(gb, HL);
  CYC(b_+118, b_+120); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+120, b_+121); ret_effect(gb); return; }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+123, b_+125);
    goto bit7;
  }
  CYC(b_+123, b_+125);
  CYC(b_+125, b_+126); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+126, b_+128);
    goto bit6;
  }
  CYC(b_+126, b_+128);
  CYC(b_+128, b_+129); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+129, b_+131);
    goto bit5;
  }
  CYC(b_+129, b_+131);
  CYC(b_+131, b_+132); A = E;
  CYC(b_+132, b_+134); alu_and(gb, 0xcc);
  CYC(b_+134, b_+136); alu_cp(gb, 0x04);
  CYC(b_+136, b_+138); A = 0x00;
  if (F & FZ) { CYCT(b_+138, b_+139); ret_effect(gb); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+140); A = E;
  CYC(b_+140, b_+142); alu_and(gb, 0x3c);
  CYC(b_+142, b_+144); alu_cp(gb, 0x08);
  CYC(b_+144, b_+146); A = 0x10;
  CYC(b_+146, b_+147); ret_effect(gb); return;

bit5:
  CYC(b_+147, b_+148); A = E;
  CYC(b_+148, b_+150); alu_and(gb, 0xc3);
  CYC(b_+150, b_+152); alu_cp(gb, 0x01);
  CYC(b_+152, b_+154); A = 0x00;
  if (F & FZ) { CYCT(b_+154, b_+155); ret_effect(gb); return; }
  CYC(b_+154, b_+155);
  CYC(b_+155, b_+156); A = E;
  CYC(b_+156, b_+158); alu_and(gb, 0x33);
  CYC(b_+158, b_+160); alu_cp(gb, 0x02);
  CYC(b_+160, b_+162); A = 0x10;
  CYC(b_+162, b_+163); ret_effect(gb); return;

bit7:
  CYC(b_+163, b_+164); A = E;
  CYC(b_+164, b_+166); alu_and(gb, 0xc3);
  CYC(b_+166, b_+168); alu_cp(gb, 0x80);
  CYC(b_+168, b_+170); A = 0x08;
  if (F & FZ) { CYCT(b_+170, b_+171); ret_effect(gb); return; }
  CYC(b_+170, b_+171);
  CYC(b_+171, b_+172); A = E;
  CYC(b_+172, b_+174); alu_and(gb, 0xcc);
  CYC(b_+174, b_+176); alu_cp(gb, 0x40);
  CYC(b_+176, b_+178); A = 0x18;
  CYC(b_+178, b_+179); ret_effect(gb); return;

bit6:
  CYC(b_+179, b_+180); A = E;
  CYC(b_+180, b_+182); alu_and(gb, 0x33);
  CYC(b_+182, b_+184); alu_cp(gb, 0x20);
  CYC(b_+184, b_+186); A = 0x08;
  if (F & FZ) { CYCT(b_+186, b_+187); ret_effect(gb); return; }
  CYC(b_+186, b_+187);
  CYC(b_+187, b_+188); A = E;
  CYC(b_+188, b_+190); alu_and(gb, 0x3c);
  CYC(b_+190, b_+192); alu_cp(gb, 0x10);
  CYC(b_+192, b_+194); A = 0x18;
  CYC(b_+194, b_+195); ret_effect(gb);
}

void specialObjectUpdatePositionGivenVelocity_hook(GB *gb) {
  BASE(specialObjectUpdatePositionGivenVelocity);
  CYC(b_+0, b_+2); alu_bit(gb, 7, C);
  if (!(F & FZ)) {
    CYCT(b_+2, b_+4);
    CYC(b_+77, b_+78); alu_xor(gb, A);
    CYC(b_+78, b_+79); C = A;
    CYC(b_+79, b_+80); ret_effect(gb);
    return;
  }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); E = 0x33;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+11); push_effect(gb, b_+11);
  special_object_tile_edge_adjust(gb);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    link_special_object_update_position_after_collision_hook(gb);
    return;
  } else {
    CYC(b_+11, b_+13);
    link_special_object_update_position_continuation_hook(gb);
    return;
  }
}

void specialObjectUpdateAdjacentWallsBitset_hook(GB *gb) {
  BASE(specialObjectUpdateAdjacentWallsBitset);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x33;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+7); A = W8(wLinkObjectIndex);
  CYC(b_+7, b_+8); alu_rrca(gb);
  if (F & FC) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  if (game_seasons) {       // a hole tile counts as walls all round; no remap table
    CYC(b_+S(9), b_+S(12)); A = W8(wActiveTileType);
    CYC(b_+S(12), b_+S(14)); alu_sub(gb, 0x08);
    if (F & FZ) {
      CYC(b_+S(14), b_+S(16));
      CYC(b_+S(16), b_+S(17)); A = alu_dec8(gb, A);
      CYCT(b_+S(17), b_+S(19));
    } else {
      CYCT(b_+S(14), b_+S(16));
      CYC(b_+S(19), b_+S(20)); H = D;
      CYC(b_+S(20), b_+S(22)); L = 0x0b;
      CYC(b_+S(22), b_+S(23)); B = mem_rd(gb, HL);
      CYC(b_+S(23), b_+S(25)); L = 0x0d;
      CYC(b_+S(25), b_+S(26)); C = mem_rd(gb, HL);
      CALL_C(b_+S(26), calculateAdjacentWallsBitset_hook, SYM(calculateAdjacentWallsBitset), b_+S(29));
    }
    CYC(b_+S(29), b_+S(31)); E = 0x33;
    CYC(b_+S(31), b_+S(32)); mem_wr(gb, DE, A);
    RET(b_+S(32)); return;
  }
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0x0b;
  CYC(b_+12, b_+13); B = mem_rd(gb, HL);
  CYC(b_+13, b_+15); L = 0x0d;
  CYC(b_+15, b_+16); C = mem_rd(gb, HL);
  CALL_C(b_+16, calculateAdjacentWallsBitset_hook, SYM(calculateAdjacentWallsBitset), b_+19);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+23); SET_HL(specialObjectAdjacentWallsRemapData_bank05 - 1);

  for (;;) {
    CYC(b_+23, b_+24); SET_HL(HL + 1);
    CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+25, b_+26); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+26, b_+28);
      CYC(b_+38, b_+39); A = B;
      CYC(b_+39, b_+41); E = 0x33;
      CYC(b_+41, b_+42); mem_wr(gb, DE, A);
      CYC(b_+42, b_+43); ret_effect(gb);
      return;
    }
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+29); alu_cp(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+29, b_+31);
      continue;
    }
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+32); A = mem_rd(gb, HL);
    CYC(b_+32, b_+34); H8(hFF8B) = A;
    CYC(b_+34, b_+36); E = 0x33;
    CYC(b_+36, b_+37); mem_wr(gb, DE, A);
    CYC(b_+37, b_+38); ret_effect(gb);
    return;
  }
}

void updateLinkSpeed_withParam_hook(GB *gb) {
  BASE(updateLinkSpeed_withParam);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x36;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(b_+4, b_+6);
    goto determine_speed;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = C;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); alu_and(gb, 0x7f);
  CYC(b_+10, b_+13); SET_HL(linkSpeedTable_bank05);
  CYC(b_+13, b_+14); link_add_a_to_hl(gb, b_+14);
  CYC(b_+14, b_+16); E = 0x10;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+18, b_+20);
  } else {
    CYC(b_+18, b_+20);
    CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  }
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+24); E = 0x12;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);

determine_speed:
  CYC(b_+28, b_+30); B = 0x02;
  CYC(b_+30, b_+32); E = 0x00;
  CYC(b_+32, b_+35); A = W8(wActiveTileType);
  CYC(b_+35, b_+37); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+37, b_+39);
    goto select_speed;
  }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+41, b_+43);
    goto select_speed;
  }
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+45, b_+47);
    goto check_pegasus;
  }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+48); B = alu_inc8(gb, B);
  CYC(b_+48, b_+50); alu_cp(gb, 0x06);
  if (F & FZ) {
    CYCT(b_+50, b_+52);
    goto check_pegasus;
  }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); alu_cp(gb, 0x04);
  if (F & FZ) {
    CYCT(b_+54, b_+56);
    goto check_pegasus;
  }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+57); B = alu_inc8(gb, B);

check_pegasus:
  CALL_C(b_+57, checkPegasusSeedCounter_hook, SYM(checkPegasusSeedCounter), b_+60);
  if (F & FZ) {
    CYCT(b_+60, b_+62);
  } else {
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+64); E = 0x03;
  }

select_speed:
  CYC(b_+64, b_+65); A = E;
  CYC(b_+65, b_+66); alu_add(gb, B);
  CYC(b_+66, b_+67); alu_add(gb, C);
  CYC(b_+67, b_+69); alu_and(gb, 0x7f);
  CYC(b_+69, b_+72); SET_HL(linkSpeedTable_bank05);
  CYC(b_+72, b_+73); link_add_a_to_hl(gb, b_+73);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = 0x11;
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+78, b_+80); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(b_+80, b_+81); ret_effect(gb); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+82); mem_wr(gb, HL, A);
  CYC(b_+82, b_+83); ret_effect(gb);
}

void linkUpdateMovement_hook(GB *gb) {
  BASE(linkUpdateMovement);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+2); alu_rrca(gb);
  CYC(b_+2, b_+3); push_effect(gb, AF);
  if (F & FC) {
    CYCT(b_+3, b_+5);
    CALL_C(b_+10, animateLinkWalking_hook, SYM(animateLinkWalking), b_+13);
  } else {
    CYC(b_+3, b_+5);
    CALL_C(b_+5, animateLinkStanding_hook, SYM(animateLinkStanding), b_+8);
    CYC(b_+8, b_+10);
  }
  CYC(b_+13, b_+14); SET_AF(pop_effect(gb));
  CYC(b_+14, b_+15); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+15, b_+17);
    goto reset_speed;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); push_effect(gb, AF);
  CALL_C(b_+18, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+21);
  if (F & FZ) {
    CYCT(b_+21, b_+23);
    CYC(b_+31, b_+32); SET_AF(pop_effect(gb));
    goto reset_speed;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+25); SET_AF(pop_effect(gb));
  CYC(b_+25, b_+26); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); A = C;
  CYC(b_+28, b_+31);
  TAIL(updateHeartRingCounter);

reset_speed:
  CYC(b_+32, b_+35);
  TAIL(linkResetSpeed);
}
