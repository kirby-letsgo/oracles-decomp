#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  CYC(0x63ae, 0x63b0); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x63b0, 0x63b1); A = mem_rd(gb, DE);
  CYC(0x63b1, 0x63b3); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(0x63b3, 0x63b4); push_effect(gb, 0x63b4);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == 0x6494) { bombUpgradeFairy_subid01_hook(gb); return; }
    if (target == 0x64ec) { bombUpgradeFairy_subid02_hook(gb); return; }
    bombUpgradeFairy_subid00_hook(gb); return; // target == 0x63ba
  }
}

void bombUpgradeFairy_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x63ba, 0x63bb); A = mem_rd(gb, DE);
  {
    CYC(0x63bb, 0x63bc); push_effect(gb, 0x63bc);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == 0x63fb) goto state1;
    if (target == 0x6441) goto state2;
    if (target == 0x6472) goto state3;
  }

  // bombUpgradeFairy_subid00@state0
  CYC(0x63c4, 0x63c6); A = 0x1c; // GLOBALFLAG_GOT_BOMB_UPGRADE_FROM_FAIRY
  CALL_C(0x63c6, checkGlobalFlag_hook, 0x31f3, 0x63c9);
  if (!(F & FZ)) { CYCT(0x63c9, 0x63cc); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x63c9, 0x63cc);
  CALL_C(0x63cc, getThisRoomFlags_hook, 0x197d, 0x63cf);
  CYC(0x63cf, 0x63d1); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x63d1, 0x63d4); interactionDelete_hook(gb); return; } // jp z
  CYC(0x63d1, 0x63d4);
  CALL_C(0x63d4, interactionInitGraphics_hook, 0x15fb, 0x63d7);
  CALL_C(0x63d7, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x63da);
  CALL_C(0x63da, interactionIncState_hook, 0x23e0, 0x63dd);
  CYC(0x63dd, 0x63df); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x63df, 0x63e0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x63e0, 0x63e1); L = alu_inc8(gb, L);
  CYC(0x63e1, 0x63e3); mem_wr(gb, HL, 0x12);
  CYC(0x63e3, 0x63e6); SET_HL(wTextNumberSubstitution);
  CYC(0x63e6, 0x63e9); A = W8(wMaxBombs);
  CYC(0x63e9, 0x63eb); alu_cp(gb, 0x10);
  CYC(0x63eb, 0x63ed); A = 0x30;
  if (F & FZ) { CYCT(0x63ed, 0x63ef); goto l63f1; } // jr z
  CYC(0x63ed, 0x63ef);
  CYC(0x63ef, 0x63f1); A = 0x50;

l63f1:
  CYC(0x63f1, 0x63f2); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x63f2, 0x63f3); alu_xor(gb, A);
  CYC(0x63f3, 0x63f4); mem_wr(gb, HL, A);
  CYC(0x63f4, 0x63f7); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;
  CYC(0x63f7, 0x63fa); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  RET(0x63fa); return; // ret

state1:
  // Bombs are hardcoded to set this variable to $01 when it falls into water on this
  // screen. Hold execution until that happens.
  CYC(0x63fb, 0x63fe); A = W8(wTmpcfc0_bombUpgradeCutscene_state);
  CYC(0x63fe, 0x63ff); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x63ff); return; } // ret nz
  CYC(0x63ff, 0x6400);
  CYC(0x6400, 0x6403); A = W8(w1Link_zh);
  CYC(0x6403, 0x6404); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6404); return; } // ret nz
  CYC(0x6404, 0x6405);
  // Check that Link's in position
  CYC(0x6405, 0x6407); A = H8(hEnemyTargetY);
  CYC(0x6407, 0x6409); alu_sub(gb, 0x41);
  CYC(0x6409, 0x640b); alu_cp(gb, 0x06);
  if (!(F & FC)) { RET_TAKEN(0x640b); return; } // ret nc
  CYC(0x640b, 0x640c);
  CYC(0x640c, 0x640e); A = H8(hEnemyTargetX);
  CYC(0x640e, 0x6410); alu_sub(gb, 0x58);
  CYC(0x6410, 0x6412); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(0x6412); return; } // ret nc
  CYC(0x6412, 0x6413);
  CALL_C(0x6413, checkLinkVulnerable_hook, 0x1d28, 0x6416);
  if (!(F & FC)) { RET_TAKEN(0x6416); return; } // ret nc
  CYC(0x6416, 0x6417);
  CYC(0x6417, 0x641a); SET_BC(0x0502); // INTERAC_PUFF, $02
  CALL_C(0x641a, objectCreateInteraction_hook, 0x24c5, 0x641d);
  if (!(F & FZ)) { RET_TAKEN(0x641d); return; } // ret nz
  CYC(0x641d, 0x641e);
  CYC(0x641e, 0x6420); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x6420, 0x6422); A = INTERACTION_BASE; // Interaction.start
  CYC(0x6422, 0x6423); mem_wr(gb, DE, A);
  CYC(0x6423, 0x6424); E = alu_inc8(gb, E);
  CYC(0x6424, 0x6425); A = H;
  CYC(0x6425, 0x6426); mem_wr(gb, DE, A);
  CALL_C(0x6426, clearAllParentItems_hook, 0x2c10, 0x6429);
  CALL_C(0x6429, dropLinkHeldItem_hook, 0x2c43, 0x642c);
  CYC(0x642c, 0x642d); alu_xor(gb, A);
  CYC(0x642d, 0x6430); W8(w1Link_direction) = A;
  CYC(0x6430, 0x6433); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;
  CYC(0x6433, 0x6435); A = 0x80;
  CYC(0x6435, 0x6438); W8(wDisabledObjects) = A;
  CYC(0x6438, 0x643b); W8(wMenuDisabled) = A;
  CALL_C(0x643b, setLinkForceStateToState08_hook, 0x2aad, 0x643e);
  CYC(0x643e, 0x6441); interactionIncState_hook(gb); return; // jp

state2:
  // Wait for signal to spawn in silver and gold bombs?
  CYC(0x6441, 0x6443); A = OBJ_ANIM_PARAMETER;
  CALL_C(0x6443, objectGetRelatedObject2Var_hook, 0x2164, 0x6446);
  CYC(0x6446, 0x6448); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6448); return; } // ret z
  CYC(0x6448, 0x6449);
  CALL_C(0x6449, interactionIncState_hook, 0x23e0, 0x644c);
  CYC(0x644c, 0x644e); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x644e, 0x6450); mem_wr(gb, HL, 0x28);
  CYC(0x6450, 0x6453); SET_BC(0x840e); // INTERAC_SPARKLE, $0e
  CALL_C(0x6453, objectCreateInteraction_hook, 0x24c5, 0x6456);
  CALL_C(0x6456, objectSetVisible81_hook, 0x1e60, 0x6459);
  CYC(0x6459, 0x645c); SET_HL(0x76ff); // mainScripts.bombUpgradeFairyScript
  CALL_C(0x645c, interactionSetScript_hook, 0x2544, 0x645f);
  CYC(0x645f, 0x6461); B = 0x00;
  CYC(0x6461, 0x6464); push_effect(gb, 0x6464); goto spawnSubid2Instance;
after1_spawnSubid2Instance:
  CYC(0x6464, 0x6466); B = 0x01;
  // falls through into spawnSubid2Instance a second time, no push (this call
  // returns through the outer dispatcher's own frame, exiting the hook for real)

spawnSubid2Instance:
  CALL_C(0x6466, getFreeInteractionSlot_hook, 0x3aef, 0x6469);
  if (!(F & FZ)) {
    RET_TAKEN(0x6469);
    if (gb->pc == 0x6464 && gb->sp == sp0_) goto after1_spawnSubid2Instance;
    return;
  } // ret nz
  CYC(0x6469, 0x646a);
  CYC(0x646a, 0x646c); mem_wr(gb, HL, 0x83); // INTERAC_BOMB_UPGRADE_FAIRY
  CYC(0x646c, 0x646d); L = alu_inc8(gb, L);
  CYC(0x646d, 0x646f); mem_wr(gb, HL, 0x02);
  CYC(0x646f, 0x6470); L = alu_inc8(gb, L);
  CYC(0x6470, 0x6471); mem_wr(gb, HL, B);
  RET(0x6471);
  if (gb->pc == 0x6464 && gb->sp == sp0_) goto after1_spawnSubid2Instance;
  return; // ret

state3:
  CALL_C(0x6472, interactionAnimate_hook, 0x261b, 0x6475);
  CYC(0x6475, 0x6478); A = W8(wTextIsActive);
  CYC(0x6478, 0x6479); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6479); return; } // ret nz
  CYC(0x6479, 0x647a);
  CYC(0x647a, 0x647d); A = W8(wPaletteThread_mode);
  CYC(0x647d, 0x647e); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x647e); return; } // ret nz
  CYC(0x647e, 0x647f);
  CALL_C(0x647f, interactionRunScript_hook, 0x2552, 0x6482);
  if (!(F & FC)) { RET_TAKEN(0x6482); return; } // ret nc
  CYC(0x6482, 0x6483);
  CYC(0x6483, 0x6484); alu_xor(gb, A);
  CYC(0x6484, 0x6487); W8(wDisabledObjects) = A;
  CYC(0x6487, 0x648a); W8(wMenuDisabled) = A;
  CYC(0x648a, 0x648b); A = alu_inc8(gb, A);
  CYC(0x648b, 0x648e); W8(wTmpcfc0_bombUpgradeCutscene_state) = A;
  CALL_C(0x648e, objectCreatePuff_hook, 0x24c1, 0x6491);
  CYC(0x6491, 0x6494); interactionDelete_hook(gb); return; // jp
}

// Bombs that surround Link (depending on his answer)
void bombUpgradeFairy_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6494, 0x6495); A = mem_rd(gb, DE);
  {
    CYC(0x6495, 0x6496); push_effect(gb, 0x6496);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == 0x64ca) goto subid1state1;
    if (target == 0x64d6) goto subid1state2;
  }

  // bombUpgradeFairy_subid01@state0
  CALL_C(0x649c, interactionInitGraphics_hook, 0x15fb, 0x649f);
  CALL_C(0x649f, interactionIncState_hook, 0x23e0, 0x64a2);
  CYC(0x64a2, 0x64a4); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x64a4, 0x64a5); A = mem_rd(gb, HL);
  CYC(0x64a5, 0x64a6); alu_add(gb, A); // add a
  CYC(0x64a6, 0x64a7); alu_add(gb, mem_rd(gb, HL));
  CYC(0x64a7, 0x64aa); SET_HL(0x64be); // @bombPositions
  CYC(0x64aa, 0x64ab); bombUpgradeFairy_addAToHl_from_rst(gb, 0x64ab);
  CYC(0x64ab, 0x64ad); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x64ad, 0x64af); A = H8(hEnemyTargetY);
  CYC(0x64af, 0x64b0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x64b0, 0x64b1); mem_wr(gb, DE, A);
  CYC(0x64b1, 0x64b3); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x64b3, 0x64b4); SET_HL(HL + 1); // inc hl
  CYC(0x64b4, 0x64b6); A = H8(hEnemyTargetX);
  CYC(0x64b6, 0x64b7); alu_add(gb, mem_rd(gb, HL));
  CYC(0x64b7, 0x64b8); mem_wr(gb, DE, A);
  CYC(0x64b8, 0x64ba); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x64ba, 0x64bb); SET_HL(HL + 1); // inc hl
  CYC(0x64bb, 0x64bc); A = mem_rd(gb, HL);
  CYC(0x64bc, 0x64bd); mem_wr(gb, DE, A);
  RET(0x64bd); return; // ret

subid1state1:
  CALL_C(0x64ca, interactionDecCounter1_hook, 0x23cc, 0x64cd);
  if (!(F & FZ)) { RET_TAKEN(0x64cd); return; } // ret nz
  CYC(0x64cd, 0x64ce);
  CYC(0x64ce, 0x64cf); L = E;
  CYC(0x64cf, 0x64d0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(0x64d0, objectCreatePuff_hook, 0x24c1, 0x64d3);
  CYC(0x64d3, 0x64d6); objectSetVisible82_hook(gb); return; // jp

subid1state2:
  CYC(0x64d6, 0x64d9); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x64d9, 0x64da); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x64da, 0x64dd); interactionDelete_hook(gb); return; } // jp z
  CYC(0x64da, 0x64dd);
  // Flash the bomb between blue and red palettes
  CALL_C(0x64dd, interactionDecCounter1_hook, 0x23cc, 0x64e0);
  CYC(0x64e0, 0x64e1); A = mem_rd(gb, HL);
  CYC(0x64e1, 0x64e3); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x64e3); return; } // ret nz
  CYC(0x64e3, 0x64e4);
  CYC(0x64e4, 0x64e6); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x64e6, 0x64e7); A = mem_rd(gb, HL);
  CYC(0x64e7, 0x64e9); alu_xor(gb, 0x01);
  CYC(0x64e9, 0x64ea); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x64ea, 0x64eb); mem_wr(gb, HL, A);
  RET(0x64eb); return; // ret
}

// Gold/silver bombs
void bombUpgradeFairy_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x64ec, 0x64ed); A = mem_rd(gb, DE);
  {
    CYC(0x64ed, 0x64ee); push_effect(gb, 0x64ee);
    uint16_t target = bombUpgradeFairy_jump_table(gb);
    if (target == 0x6526) goto subid2state1;
    if (target == 0x6534) goto subid2state2;
  }

  // bombUpgradeFairy_subid02@state0
  CALL_C(0x64f4, interactionInitGraphics_hook, 0x15fb, 0x64f7);
  CYC(0x64f7, 0x64f9); A = 0x80; // PALH_80
  CALL_C(0x64f9, loadPaletteHeader_hook, 0x050b, 0x64fc);
  CALL_C(0x64fc, interactionIncState_hook, 0x23e0, 0x64ff);
  CYC(0x64ff, 0x6501); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6501, 0x6502); A = mem_rd(gb, DE);
  CYC(0x6502, 0x6503); alu_or(gb, A);
  CYC(0x6503, 0x6505); B = 0x5a;
  if (F & FZ) { CYCT(0x6505, 0x6507); goto l650f; } // jr z
  CYC(0x6505, 0x6507);
  CYC(0x6507, 0x6509); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x6509, 0x650b); A = 0x06;
  CYC(0x650b, 0x650c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x650c, 0x650d); mem_wr(gb, HL, A);
  CYC(0x650d, 0x650f); B = 0x76;

l650f:
  CYC(0x650f, 0x6511); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6511, 0x6513); mem_wr(gb, HL, 0x3c);
  CYC(0x6513, 0x6515); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x6515, 0x6516); mem_wr(gb, HL, B);
  CYC(0x6516, 0x6519); SET_BC(0x0502); // INTERAC_PUFF, $02
  CALL_C(0x6519, objectCreateInteraction_hook, 0x24c5, 0x651c);
  if (!(F & FZ)) { RET_TAKEN(0x651c); return; } // ret nz
  CYC(0x651c, 0x651d);
  CYC(0x651d, 0x651f); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(0x651f, 0x6521); A = INTERACTION_BASE; // Interaction.start
  CYC(0x6521, 0x6522); mem_wr(gb, DE, A);
  CYC(0x6522, 0x6523); E = alu_inc8(gb, E);
  CYC(0x6523, 0x6524); A = H;
  CYC(0x6524, 0x6525); mem_wr(gb, DE, A);
  RET(0x6525); return; // ret

subid2state1:
  // Wait for the puff to finish, then make self visible
  CYC(0x6526, 0x6528); A = OBJ_ANIM_PARAMETER;
  CALL_C(0x6528, objectGetRelatedObject1Var_hook, 0x2160, 0x652b);
  CYC(0x652b, 0x652d); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x652d); return; } // ret nz
  CYC(0x652d, 0x652e);
  CALL_C(0x652e, interactionIncState_hook, 0x23e0, 0x6531);
  CYC(0x6531, 0x6534); objectSetVisible82_hook(gb); return; // jp

subid2state2:
  CYC(0x6534, 0x6537); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x6537, 0x6538); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6538); return; } // ret z
  CYC(0x6538, 0x6539);
  CALL_C(0x6539, objectCreatePuff_hook, 0x24c1, 0x653c);
  CYC(0x653c, 0x653f); interactionDelete_hook(gb); return; // jp
}
