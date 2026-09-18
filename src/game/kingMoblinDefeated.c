#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode72_jump_table(GB *gb) {
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

static void interactionCode72_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_KING_MOBLIN_DEFEATED
void interactionCode72_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cbc, 0x5cbe); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5cbe, 0x5cbf); A = mem_rd(gb, DE);
  CYC(0x5cbf, 0x5cc1); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(0x5cc1, 0x5cc2); push_effect(gb, 0x5cc2);
    uint16_t target = interactionCode72_jump_table(gb);
    if (target == 0x5d3a) goto subid1;
    if (target == 0x5d4c) goto subid2;
  }

  // Subid 0: King moblin / "parent" for other subids
  CYC(0x5cc8, 0x5cc9); A = mem_rd(gb, DE);
  CYC(0x5cc9, 0x5cca); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5cca, 0x5ccc); goto subid0State0; } // jr z
  CYC(0x5cca, 0x5ccc);

  // interactionCode72@subid0State1
  CALL_C(0x5ccc, interactionRunScript_hook, 0x2552, 0x5ccf);
  if (!(F & FC)) { CYCT(0x5ccf, 0x5cd2); interactionAnimate_hook(gb); return; } // jp nc
  CYC(0x5ccf, 0x5cd2);
  CALL_C(0x5cd2, getFreeInteractionSlot_hook, 0x3aef, 0x5cd5);
  if (!(F & FZ)) { RET_TAKEN(0x5cd5); return; } // ret nz
  CYC(0x5cd5, 0x5cd6);
  // Spawn instance of this object with subid 2
  CYC(0x5cd6, 0x5cd8); mem_wr(gb, HL, 0x72); // INTERAC_KING_MOBLIN_DEFEATED
  CYC(0x5cd8, 0x5cd9); L = alu_inc8(gb, L);
  CYC(0x5cd9, 0x5cdb); mem_wr(gb, HL, 0x02);
  CYC(0x5cdb, 0x5cdd); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5cdd, 0x5cdf); mem_wr(gb, HL, 0x68);
  CYC(0x5cdf, 0x5ce2); interactionDelete_hook(gb); return; // jp

subid0State0:
  CALL_C(0x5ce2, getThisRoomFlags_hook, 0x197d, 0x5ce5);
  CYC(0x5ce5, 0x5ce7); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x5ce7, 0x5cea); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5ce7, 0x5cea);
  CYC(0x5cea, 0x5cec); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(0x5cec, checkGlobalFlag_hook, 0x31f3, 0x5cef);
  if (F & FZ) { CYCT(0x5cef, 0x5cf2); interactionDelete_hook(gb); return; } // jp z
  CYC(0x5cef, 0x5cf2);
  CALL_C(0x5cf2, setDeathRespawnPoint_hook, 0x1100, 0x5cf5);
  CYC(0x5cf5, 0x5cf7); A = 0x80;
  CYC(0x5cf7, 0x5cfa); W8(wDisabledObjects) = A;
  CYC(0x5cfa, 0x5cfd); W8(wMenuDisabled) = A;

  CYC(0x5cfd, 0x5d00); push_effect(gb, 0x5d00); goto spawnSubservientMoblin;
after1_spawnSubservientMoblin:
  CYC(0x5d00, 0x5d02); mem_wr(gb, HL, 0x38);
  CYC(0x5d02, 0x5d05); push_effect(gb, 0x5d05); goto spawnSubservientMoblin;
after2_spawnSubservientMoblin:
  CYC(0x5d05, 0x5d07); mem_wr(gb, HL, 0x78);
  CYC(0x5d07, 0x5d0a); SET_HL(0xcfd0);
  CYC(0x5d0a, 0x5d0c); B = 0x04;
  CALL_C(0x5d0c, clearMemory_hook, 0x046f, 0x5d0f);
  CYC(0x5d0f, 0x5d11); A = 0x02;
  CALL_C(0x5d11, fadeinFromWhiteWithDelay_hook, 0x3284, 0x5d14);
  CYC(0x5d14, 0x5d17); SET_HL(0x7512); // mainScripts.kingMoblinDefeated_kingScript

setScriptAndInitStuff:
  CALL_C(0x5d17, interactionSetScript_hook, 0x2544, 0x5d1a);
  CALL_C(0x5d1a, interactionInitGraphics_hook, 0x15fb, 0x5d1d);
  CALL_C(0x5d1d, interactionIncState_hook, 0x23e0, 0x5d20);
  CYC(0x5d20, 0x5d22); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5d22, 0x5d24); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x5d24, 0x5d26); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5d26, 0x5d28); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x5d28, 0x5d2b); objectSetVisible82_hook(gb); return; // jp

// Spawn an instance of subid 1, the normal moblins
spawnSubservientMoblin:
  CALL_C(0x5d2b, getFreeInteractionSlot_hook, 0x3aef, 0x5d2e);
  if (!(F & FZ)) {
    RET_TAKEN(0x5d2e);
    if (gb->pc == 0x5d00 && gb->sp == sp0_) goto after1_spawnSubservientMoblin;
    if (gb->pc == 0x5d05 && gb->sp == sp0_) goto after2_spawnSubservientMoblin;
    return;
  } // ret nz
  CYC(0x5d2e, 0x5d2f);
  CYC(0x5d2f, 0x5d31); mem_wr(gb, HL, 0x72); // INTERAC_KING_MOBLIN_DEFEATED
  CYC(0x5d31, 0x5d32); L = alu_inc8(gb, L);
  CYC(0x5d32, 0x5d33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x5d33, 0x5d35); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x5d35, 0x5d37); mem_wr(gb, HL, 0x68);
  CYC(0x5d37, 0x5d39); L = INTERACTION_BASE + OBJ_XH;
  RET(0x5d39);
  if (gb->pc == 0x5d00 && gb->sp == sp0_) goto after1_spawnSubservientMoblin;
  if (gb->pc == 0x5d05 && gb->sp == sp0_) goto after2_spawnSubservientMoblin;
  return; // ret

subid1:
  CYC(0x5d3a, 0x5d3b); A = mem_rd(gb, DE);
  CYC(0x5d3b, 0x5d3c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5d3c, 0x5d3e); goto subid1State0; } // jr z
  CYC(0x5d3c, 0x5d3e);

runScriptAndAnimate:
  CALL_C(0x5d3e, interactionRunScript_hook, 0x2552, 0x5d41);
  if (!(F & FC)) { CYCT(0x5d41, 0x5d44); interactionAnimate_hook(gb); return; } // jp nc
  CYC(0x5d41, 0x5d44);
  CYC(0x5d44, 0x5d47); interactionDelete_hook(gb); return; // jp

subid1State0:
  CYC(0x5d47, 0x5d4a); SET_HL(0x7521); // mainScripts.kingMoblinDefeated_helperMoblinScript
  CYC(0x5d4a, 0x5d4c); goto setScriptAndInitStuff; // jr

// Subid 2: Gorons who approach after he leaves; var03 is the index
subid2:
  CYC(0x5d4c, 0x5d4d); A = mem_rd(gb, DE);
  CYC(0x5d4d, 0x5d4e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5d4e, 0x5d50); goto runScriptAndAnimate; } // jr nz
  CYC(0x5d4e, 0x5d50);
  CALL_C(0x5d50, interactionInitGraphics_hook, 0x15fb, 0x5d53);
  CALL_C(0x5d53, interactionIncState_hook, 0x23e0, 0x5d56);
  CYC(0x5d56, 0x5d58); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5d58, 0x5d5a); mem_wr(gb, HL, 0x14); // SPEED_80
  // Load script
  CYC(0x5d5a, 0x5d5c); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5d5c, 0x5d5d); A = mem_rd(gb, DE);
  CYC(0x5d5d, 0x5d60); SET_HL(0x5d9d); // @scriptTable
  CYC(0x5d60, 0x5d61); interactionCode72_add_double_index(gb, 0x5d61);
  CYC(0x5d61, 0x5d62); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5d62, 0x5d63); H = mem_rd(gb, HL);
  CYC(0x5d63, 0x5d64); L = A;
  CALL_C(0x5d64, interactionSetScript_hook, 0x2544, 0x5d67);
  CALL_C(0x5d67, objectSetVisible82_hook, 0x1e69, 0x5d6a);
  // Load data from table
  CYC(0x5d6a, 0x5d6c); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5d6c, 0x5d6d); A = mem_rd(gb, DE);
  CYC(0x5d6d, 0x5d6e); alu_add(gb, A); // add a
  CYC(0x5d6e, 0x5d71); SET_HL(0x5da5); // @goronData
  CYC(0x5d71, 0x5d72); interactionCode72_add_double_index(gb, 0x5d72);
  CYC(0x5d72, 0x5d74); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5d74, 0x5d75); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5d75, 0x5d76); mem_wr(gb, DE, A);
  CYC(0x5d76, 0x5d78); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5d78, 0x5d79); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5d79, 0x5d7a); mem_wr(gb, DE, A);
  CYC(0x5d7a, 0x5d7c); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5d7c, 0x5d7d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5d7d, 0x5d7e); mem_wr(gb, DE, A);
  CYC(0x5d7e, 0x5d7f); A = mem_rd(gb, HL);
  CALL_C(0x5d7f, interactionSetAnimation_hook, 0x262e, 0x5d82);
  // If [var03] == 0, spawn the other gorons
  CYC(0x5d82, 0x5d84); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5d84, 0x5d85); A = mem_rd(gb, DE);
  CYC(0x5d85, 0x5d86); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5d86); return; } // ret nz
  CYC(0x5d86, 0x5d87);
  CYC(0x5d87, 0x5d89); B = 0x01;

  CYC(0x5d89, 0x5d8c); push_effect(gb, 0x5d8c); goto spawnGoronInstance;
after1_spawnGoronInstance:
  CYC(0x5d8c, 0x5d8d); B = alu_inc8(gb, B);
  CYC(0x5d8d, 0x5d90); push_effect(gb, 0x5d90); goto spawnGoronInstance;
after2_spawnGoronInstance:
  CYC(0x5d90, 0x5d91); B = alu_inc8(gb, B);

spawnGoronInstance:
  CALL_C(0x5d91, getFreeInteractionSlot_hook, 0x3aef, 0x5d94);
  if (!(F & FZ)) {
    RET_TAKEN(0x5d94);
    if (gb->pc == 0x5d8c && gb->sp == sp0_) goto after1_spawnGoronInstance;
    if (gb->pc == 0x5d90 && gb->sp == sp0_) goto after2_spawnGoronInstance;
    return;
  } // ret nz
  CYC(0x5d94, 0x5d95);
  CYC(0x5d95, 0x5d97); mem_wr(gb, HL, 0x72); // INTERAC_KING_MOBLIN_DEFEATED
  CYC(0x5d97, 0x5d98); L = alu_inc8(gb, L);
  CYC(0x5d98, 0x5d9a); mem_wr(gb, HL, 0x02);
  CYC(0x5d9a, 0x5d9b); L = alu_inc8(gb, L);
  CYC(0x5d9b, 0x5d9c); mem_wr(gb, HL, B);
  RET(0x5d9c);
  if (gb->pc == 0x5d8c && gb->sp == sp0_) goto after1_spawnGoronInstance;
  if (gb->pc == 0x5d90 && gb->sp == sp0_) goto after2_spawnGoronInstance;
  return; // ret
}
