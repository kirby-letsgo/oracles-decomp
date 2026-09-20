#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t possessedNayru_jump_table(GB *gb) {
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

// possessedNayru_subid00: relies on E already holding INTERACTION_BASE + OBJ_STATE, set by
// interactionCode6d's own dispatcher before tail-jumping here -- this routine never sets E
// itself, matching the ROM's own `ld a,(de)` with no preceding `ld e,...`.
void possessedNayru_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5409, 0x540a); A = mem_rd(gb, DE);
  {
    CYC(0x540a, 0x540b); push_effect(gb, 0x540b);
    uint16_t target = possessedNayru_jump_table(gb);
    if (target == 0x5413) goto state0;
    if (target == 0x5465) goto state1;
    if (target == 0x546c) goto state2;
    goto state3;
  }

state0:
  CYC(0x5413, 0x5415); A = 0x19; // GLOBALFLAG_BEAT_POSSESSED_NAYRU
  CALL_C(0x5415, checkGlobalFlag_hook, 0x31f3, 0x5418);
  if (!(F & FZ)) { CYCT(0x5418, 0x541b); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5418, 0x541b);
  CYC(0x541b, 0x541d); A = 0x85; // PALH_85
  CALL_C(0x541d, loadPaletteHeader_hook, 0x050b, 0x5420);
  CYC(0x5420, 0x5422); A = 0x18; // GLOBALFLAG_BEGAN_POSSESSED_NAYRU_FIGHT
  CALL_C(0x5422, checkGlobalFlag_hook, 0x31f3, 0x5425);
  if (!(F & FZ)) { CYCT(0x5425, 0x5427); goto state2; } // jr nz
  CYC(0x5425, 0x5427);
  CALL_C(0x5427, getFreeInteractionSlot_hook, 0x3aef, 0x542a);
  if (!(F & FZ)) { RET_TAKEN(0x542a); return; } // ret nz
  CYC(0x542a, 0x542b);
  CYC(0x542b, 0x542d); mem_wr(gb, HL, 0x6d); // INTERAC_POSSESSED_NAYRU
  CYC(0x542d, 0x542e); L = alu_inc8(gb, L);
  CYC(0x542e, 0x5430); mem_wr(gb, HL, 0x02);
  CYC(0x5430, 0x5432); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x5432, 0x5434); mem_wr(gb, HL, INTERACTION_BASE); // Interaction.start
  CYC(0x5434, 0x5435); L = alu_inc8(gb, L);
  CYC(0x5435, 0x5436); mem_wr(gb, HL, D);
  CALL_C(0x5436, objectCopyPosition_hook, 0x2242, 0x5439);
  CALL_C(0x5439, interactionInitGraphics_hook, 0x15fb, 0x543c);
  CYC(0x543c, 0x543e); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x543e, 0x5441); W8(wLinkForceState) = A;
  CYC(0x5441, 0x5443); A = 0x0e;
  CYC(0x5443, 0x5446); W8(wLinkStateParameter) = A;
  CYC(0x5446, 0x5449); SET_HL(0xd008); // w1Link.direction
  CYC(0x5449, 0x544c); A = W8(wScreenTransitionDirection);
  CYC(0x544c, 0x544d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x544d, 0x544f); alu_swap_a(gb);
  CYC(0x544f, 0x5450); alu_rrca(gb);
  CYC(0x5450, 0x5451); mem_wr(gb, HL, A);
  CYC(0x5451, 0x5453); A = 0x01;
  CYC(0x5453, 0x5456); W8(wDisabledObjects) = A;
  CYC(0x5456, 0x5459); W8(wMenuDisabled) = A;
  CALL_C(0x5459, interactionIncState_hook, 0x23e0, 0x545c);
  CALL_C(0x545c, objectSetVisible82_hook, 0x1e69, 0x545f);
  CYC(0x545f, 0x5462); SET_HL(0x7299); // mainScripts.possessedNayru_beginFightScript
  CYC(0x5462, 0x5465); interactionSetScript_hook(gb); return; // jp

state1:
  CALL_C(0x5465, interactionRunScript_hook, 0x2552, 0x5468);
  if (!(F & FC)) { RET_TAKEN(0x5468); return; } // ret nc
  CYC(0x5468, 0x5469);
  CALL_C(0x5469, interactionIncState_hook, 0x23e0, 0x546c);

state2:
  CALL_C(0x546c, getFreeEnemySlot_hook, 0x2e27, 0x546f);
  if (!(F & FZ)) { RET_TAKEN(0x546f); return; } // ret nz
  CYC(0x546f, 0x5470);
  CYC(0x5470, 0x5472); mem_wr(gb, HL, 0x61); // ENEMY_VERAN_POSSESSION_BOSS
  CALL_C(0x5472, objectCopyPosition_hook, 0x2242, 0x5475);
  CYC(0x5475, 0x5476); H = D;
  CYC(0x5476, 0x5478); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5478, 0x547a); mem_wr(gb, HL, 0x03);
  RET(0x547a); return; // ret

state3:
  CYC(0x547b, 0x547d); A = 0x18; // GLOBALFLAG_BEGAN_POSSESSED_NAYRU_FIGHT
  CALL_C(0x547d, setGlobalFlag_hook, 0x31f9, 0x5480);
  CYC(0x5480, 0x5481); alu_xor(gb, A);
  CYC(0x5481, 0x5484); W8(wDisabledObjects) = A;
  CYC(0x5484, 0x5487); W8(wMenuDisabled) = A;
  CYC(0x5487, 0x5488); A = alu_inc8(gb, A);
  CYC(0x5488, 0x548b); W8(wLoadedTreeGfxIndex) = A;
  CYC(0x548b, 0x548e); interactionDelete_hook(gb); return; // jp
}

// possessedNayru_ghost: also relies on E already holding INTERACTION_BASE + OBJ_STATE, set by
// interactionCode6d's dispatcher.
void possessedNayru_ghost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x548e, 0x548f); A = mem_rd(gb, DE);
  {
    CYC(0x548f, 0x5490); push_effect(gb, 0x5490);
    uint16_t target = possessedNayru_jump_table(gb);
    if (target == 0x5496) goto state0;
    if (target == 0x54a1) goto state1;
    goto state2;
  }

state0:
  CALL_C(0x5496, interactionInitGraphics_hook, 0x15fb, 0x5499);
  CALL_C(0x5499, interactionIncState_hook, 0x23e0, 0x549c);
  CYC(0x549c, 0x549e); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x549e, 0x54a0); mem_wr(gb, HL, 0xfc); // -4
  RET(0x54a0); return; // ret

state1:
  CYC(0x54a1, 0x54a3); A = 0x37; // Object.var37
  CALL_C(0x54a3, objectGetRelatedObject1Var_hook, 0x2160, 0x54a6);
  CYC(0x54a6, 0x54a7); A = mem_rd(gb, HL);
  CYC(0x54a7, 0x54a8); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x54a8); return; } // ret z
  CYC(0x54a8, 0x54a9);
  CYC(0x54a9, 0x54aa); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x54aa, interactionIncState_hook, 0x23e0, 0x54ad);
  CYC(0x54ad, 0x54af); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x54af, 0x54b1); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(0x54b1, objectSetVisible81_hook, 0x1e60, 0x54b4);
  CYC(0x54b4, 0x54b7); SET_HL(0x72b8); // mainScripts.possessedNayru_veranGhostScript
  CYC(0x54b7, 0x54ba); interactionSetScript_hook(gb); return; // jp

state2:
  CALL_C(0x54ba, interactionRunScript_hook, 0x2552, 0x54bd);
  if (!(F & FC)) { CYCT(0x54bd, 0x54c0); interactionAnimate_hook(gb); return; } // jp nc
  CYC(0x54bd, 0x54c0);
  CYC(0x54c0, 0x54c2); A = 0x37; // Object.var37
  CALL_C(0x54c2, objectGetRelatedObject1Var_hook, 0x2160, 0x54c5);
  CYC(0x54c5, 0x54c7); mem_wr(gb, HL, 0x00);
  CYC(0x54c7, 0x54ca); interactionDelete_hook(gb); return; // jp
}

// INTERAC_POSSESSED_NAYRU
void interactionCode6d_hook(GB *gb) {
  CYC(0x53fd, 0x53ff); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x53ff, 0x5400); A = mem_rd(gb, DE);
  CYC(0x5400, 0x5402); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(0x5402, 0x5403); push_effect(gb, 0x5403);
    uint16_t target = possessedNayru_jump_table(gb);
    if (target == 0x5409) { possessedNayru_subid00_hook(gb); return; }
    possessedNayru_ghost_hook(gb); return; // target == 0x548e (subid01 and subid02 alias here)
  }
}
