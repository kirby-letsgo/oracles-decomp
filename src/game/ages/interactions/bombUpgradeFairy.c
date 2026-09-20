#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode83), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode83), (from), (to), true)

static uint16_t bombUpgradeFairy_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void bombUpgradeFairy_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// INTERAC_BOMB_UPGRADE_FAIRY
void interactionCode83_hook(GB *gb) {
  BASE(interactionCode83);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == SYM(bombUpgradeFairy_subid01)) { bombUpgradeFairy_subid01_hook(gb); return; }
    if (target == SYM(bombUpgradeFairy_subid02)) { bombUpgradeFairy_subid02_hook(gb); return; }
    bombUpgradeFairy_subid00_hook(gb); return; // target == 0x63ba
  }
}

void bombUpgradeFairy_subid00_hook(GB *gb) {
  BASE(bombUpgradeFairy_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == b_+65) goto state1;
    if (target == b_+135) goto state2;
    if (target == b_+184) goto state3;
  }

  // bombUpgradeFairy_subid00@state0
  CYC(b_+10, b_+12); A = 0x1c; // GLOBALFLAG_GOT_BOMB_UPGRADE_FROM_FAIRY
  CALL_C(b_+12, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  CALL_C(b_+18, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+23, b_+26); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+23, b_+26);
  CALL_C(b_+26, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+29);
  CALL_C(b_+29, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+32);
  CALL_C(b_+32, interactionIncState_hook, SYM(interactionIncState), b_+35);
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x12);
  CYC(b_+41, b_+44); SET_HL(wTextNumberSubstitution);
  CYC(b_+44, b_+47); A = W8(wMaxBombs);
  CYC(b_+47, b_+49); alu_cp(gb, 0x10);
  CYC(b_+49, b_+51); A = 0x30;
  if (F & FZ) { CYCT(b_+51, b_+53); goto l63f1; } // jr z
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); A = 0x50;

l63f1:
  CYC(b_+55, b_+56); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+56, b_+57); alu_xor(gb, A);
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+61); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;
  CYC(b_+61, b_+64); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  RET(b_+64); return; // ret

state1:
  // Bombs are hardcoded to set this variable to $01 when it falls into water on this
  // screen. Hold execution until that happens.
  CYC(b_+65, b_+68); A = W8(wTmpcfc0_bombUpgradeCutscene_state);
  CYC(b_+68, b_+69); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; } // ret nz
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73); A = W8(w1Link_zh);
  CYC(b_+73, b_+74); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; } // ret nz
  CYC(b_+74, b_+75);
  // Check that Link's in position
  CYC(b_+75, b_+77); A = H8(hEnemyTargetY);
  CYC(b_+77, b_+79); alu_sub(gb, 0x41);
  CYC(b_+79, b_+81); alu_cp(gb, 0x06);
  if (!(F & FC)) { RET_TAKEN(b_+81); return; } // ret nc
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+84); A = H8(hEnemyTargetX);
  CYC(b_+84, b_+86); alu_sub(gb, 0x58);
  CYC(b_+86, b_+88); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(b_+88); return; } // ret nc
  CYC(b_+88, b_+89);
  CALL_C(b_+89, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+92);
  if (!(F & FC)) { RET_TAKEN(b_+92); return; } // ret nc
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+96); SET_BC((SYM(initializeVramMap1) + 21)); // INTERAC_PUFF, $02
  CALL_C(b_+96, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+99);
  if (!(F & FZ)) { RET_TAKEN(b_+99); return; } // ret nz
  CYC(b_+99, b_+100);
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+102, b_+104); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  CYC(b_+105, b_+106); E = alu_inc8(gb, E);
  CYC(b_+106, b_+107); A = H;
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CALL_C(b_+108, clearAllParentItems_hook, SYM(clearAllParentItems), b_+111);
  CALL_C(b_+111, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+114);
  CYC(b_+114, b_+115); alu_xor(gb, A);
  CYC(b_+115, b_+118); W8(w1Link_direction) = A;
  CYC(b_+118, b_+121); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;
  CYC(b_+121, b_+123); A = 0x80;
  CYC(b_+123, b_+126); W8(wDisabledObjects) = A;
  CYC(b_+126, b_+129); W8(wMenuDisabled) = A;
  CALL_C(b_+129, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+132);
  CYC(b_+132, b_+135); interactionIncState_hook(gb); return; // jp

state2:
  // Wait for signal to spawn in silver and gold bombs?
  CYC(b_+135, b_+137); A = OBJ_ANIM_PARAMETER;
  CALL_C(b_+137, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+140);
  CYC(b_+140, b_+142); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+142); return; } // ret z
  CYC(b_+142, b_+143);
  CALL_C(b_+143, interactionIncState_hook, SYM(interactionIncState), b_+146);
  CYC(b_+146, b_+148); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+148, b_+150); mem_wr(gb, HL, 0x28);
  CYC(b_+150, b_+153); SET_BC(0x840e); // INTERAC_SPARKLE, $0e
  CALL_C(b_+153, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+156);
  CALL_C(b_+156, objectSetVisible81_hook, SYM(objectSetVisible81), b_+159);
  CYC(b_+159, b_+162); SET_HL((SYM(twinrova_takeInvertedPositionFromObject) + 16)); // mainScripts.bombUpgradeFairyScript
  CALL_C(b_+162, interactionSetScript_hook, SYM(interactionSetScript), b_+165);
  CYC(b_+165, b_+167); B = 0x00;
  CYC(b_+167, b_+170); push_effect(gb, b_+170); goto spawnSubid2Instance;
after1_spawnSubid2Instance:
  CYC(b_+170, b_+172); B = 0x01;
  // falls through into spawnSubid2Instance a second time, no push (this call
  // returns through the outer dispatcher's own frame, exiting the hook for real)

spawnSubid2Instance:
  CALL_C(b_+172, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+175);
  if (!(F & FZ)) {
    RET_TAKEN(b_+175);
    if (gb->pc == b_+170 && gb->sp == sp0_) goto after1_spawnSubid2Instance;
    return;
  } // ret nz
  CYC(b_+175, b_+176);
  CYC(b_+176, b_+178); mem_wr(gb, HL, 0x83); // INTERAC_BOMB_UPGRADE_FAIRY
  CYC(b_+178, b_+179); L = alu_inc8(gb, L);
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x02);
  CYC(b_+181, b_+182); L = alu_inc8(gb, L);
  CYC(b_+182, b_+183); mem_wr(gb, HL, B);
  RET(b_+183);
  if (gb->pc == b_+170 && gb->sp == sp0_) goto after1_spawnSubid2Instance;
  return; // ret

state3:
  CALL_C(b_+184, interactionAnimate_hook, SYM(interactionAnimate), b_+187);
  CYC(b_+187, b_+190); A = W8(wTextIsActive);
  CYC(b_+190, b_+191); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+191); return; } // ret nz
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+195); A = W8(wPaletteThread_mode);
  CYC(b_+195, b_+196); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+196); return; } // ret nz
  CYC(b_+196, b_+197);
  CALL_C(b_+197, interactionRunScript_hook, SYM(interactionRunScript), b_+200);
  if (!(F & FC)) { RET_TAKEN(b_+200); return; } // ret nc
  CYC(b_+200, b_+201);
  CYC(b_+201, b_+202); alu_xor(gb, A);
  CYC(b_+202, b_+205); W8(wDisabledObjects) = A;
  CYC(b_+205, b_+208); W8(wMenuDisabled) = A;
  CYC(b_+208, b_+209); A = alu_inc8(gb, A);
  CYC(b_+209, b_+212); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;
  CALL_C(b_+212, objectCreatePuff_hook, SYM(objectCreatePuff), b_+215);
  CYC(b_+215, SYM(bombUpgradeFairy_subid01)); interactionDelete_hook(gb); return; // jp
}

// Bombs that surround Link (depending on his answer)
void bombUpgradeFairy_subid01_hook(GB *gb) {
  BASE(bombUpgradeFairy_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == b_+54) goto subid1state1;
    if (target == b_+66) goto subid1state2;
  }

  // bombUpgradeFairy_subid01@state0
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, interactionIncState_hook, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_add(gb, A); // add a
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+22); SET_HL(b_+42); // @bombPositions
  CYC(b_+22, b_+23); bombUpgradeFairy_addAToHl_from_rst(gb, b_+23);
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+25, b_+27); A = H8(hEnemyTargetY);
  CYC(b_+27, b_+28); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+31, b_+32); SET_HL(HL + 1); // inc hl
  CYC(b_+32, b_+34); A = H8(hEnemyTargetX);
  CYC(b_+34, b_+35); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+39); SET_HL(HL + 1); // inc hl
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  RET(b_+41); return; // ret

subid1state1:
  CALL_C(b_+54, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+57);
  if (!(F & FZ)) { RET_TAKEN(b_+57); return; } // ret nz
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+59); L = E;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(b_+60, objectCreatePuff_hook, SYM(objectCreatePuff), b_+63);
  CYC(b_+63, b_+66); objectSetVisible82_hook(gb); return; // jp

subid1state2:
  CYC(b_+66, b_+69); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+69, b_+70); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+70, b_+73); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+70, b_+73);
  // Flash the bomb between blue and red palettes
  CALL_C(b_+73, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+76);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; } // ret nz
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+85); alu_xor(gb, 0x01);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+86, b_+87); mem_wr(gb, HL, A);
  RET(b_+87); return; // ret
}

// Gold/silver bombs
void bombUpgradeFairy_subid02_hook(GB *gb) {
  BASE(bombUpgradeFairy_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == b_+58) goto subid2state1;
    if (target == b_+72) goto subid2state2;
  }

  // bombUpgradeFairy_subid02@state0
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+13); A = 0x80; // PALH_80
  CALL_C(b_+13, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_or(gb, A);
  CYC(b_+23, b_+25); B = 0x5a;
  if (F & FZ) { CYCT(b_+25, b_+27); goto l650f; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+29, b_+31); A = 0x06;
  CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+35); B = 0x76;

l650f:
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x3c);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+41, b_+42); mem_wr(gb, HL, B);
  CYC(b_+42, b_+45); SET_BC((SYM(initializeVramMap1) + 21)); // INTERAC_PUFF, $02
  CALL_C(b_+45, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+48);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; } // ret nz
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+51, b_+53); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+55); E = alu_inc8(gb, E);
  CYC(b_+55, b_+56); A = H;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  RET(b_+57); return; // ret

subid2state1:
  // Wait for the puff to finish, then make self visible
  CYC(b_+58, b_+60); A = OBJ_ANIM_PARAMETER;
  CALL_C(b_+60, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+63);
  CYC(b_+63, b_+65); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; } // ret nz
  CYC(b_+65, b_+66);
  CALL_C(b_+66, interactionIncState_hook, SYM(interactionIncState), b_+69);
  CYC(b_+69, b_+72); objectSetVisible82_hook(gb); return; // jp

subid2state2:
  CYC(b_+72, b_+75); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+76); return; } // ret z
  CYC(b_+76, b_+77);
  CALL_C(b_+77, objectCreatePuff_hook, SYM(objectCreatePuff), b_+80);
  CYC(b_+80, SYM(interactionCode84)); interactionDelete_hook(gb); return; // jp
}
