#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(possessedNayru_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = possessedNayru_jump_table(gb);
    if (target == b_+10) goto state0;
    if (target == b_+92) goto state1;
    if (target == b_+99) goto state2;
    goto state3;
  }

state0:
  CYC(b_+10, b_+12); A = 0x19; // GLOBALFLAG_BEAT_POSSESSED_NAYRU
  CALL_C(b_+12, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); A = 0x85; // PALH_85
  CALL_C(b_+20, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+23);
  CYC(b_+23, b_+25); A = 0x18; // GLOBALFLAG_BEGAN_POSSESSED_NAYRU_FIGHT
  CALL_C(b_+25, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+28);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto state2; } // jr nz
  CYC(b_+28, b_+30);
  CALL_C(b_+30, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x6d); // INTERAC_POSSESSED_NAYRU
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x02);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+41, b_+43); mem_wr(gb, HL, INTERACTION_BASE); // Interaction.start
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); mem_wr(gb, HL, D);
  CALL_C(b_+45, objectCopyPosition_hook, SYM(objectCopyPosition), b_+48);
  CALL_C(b_+48, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+51);
  CYC(b_+51, b_+53); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+53, b_+56); W8(wLinkForceState) = A;
  CYC(b_+56, b_+58); A = 0x0e;
  CYC(b_+58, b_+61); W8(wLinkStateParameter) = A;
  CYC(b_+61, b_+64); SET_HL(w1Link_direction); // w1Link.direction
  CYC(b_+64, b_+67); A = W8(wScreenTransitionDirection);
  CYC(b_+67, b_+68); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+68, b_+70); alu_swap_a(gb);
  CYC(b_+70, b_+71); alu_rrca(gb);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  CYC(b_+72, b_+74); A = 0x01;
  CYC(b_+74, b_+77); W8(wDisabledObjects) = A;
  CYC(b_+77, b_+80); W8(wMenuDisabled) = A;
  CALL_C(b_+80, interactionIncState_hook, SYM(interactionIncState), b_+83);
  CALL_C(b_+83, objectSetVisible82_hook, SYM(objectSetVisible82), b_+86);
  CYC(b_+86, b_+89); SET_HL((SYM(miscPuzzles_subid19__state4) + 27)); // mainScripts.possessedNayru_beginFightScript
  CYC(b_+89, b_+92); TAIL(interactionSetScript); // jp

state1:
  CALL_C(b_+92, interactionRunScript_hook, SYM(interactionRunScript), b_+95);
  if (!(F & FC)) { RET_TAKEN(b_+95); return; } // ret nc
  CYC(b_+95, b_+96);
  CALL_C(b_+96, interactionIncState_hook, SYM(interactionIncState), b_+99);

state2:
  CALL_C(b_+99, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+102);
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; } // ret nz
  CYC(b_+102, b_+103);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x61); // ENEMY_VERAN_POSSESSION_BOSS
  CALL_C(b_+105, objectCopyPosition_hook, SYM(objectCopyPosition), b_+108);
  CYC(b_+108, b_+109); H = D;
  CYC(b_+109, b_+111); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x03);
  RET(b_+113); return; // ret

state3:
  CYC(b_+114, b_+116); A = 0x18; // GLOBALFLAG_BEGAN_POSSESSED_NAYRU_FIGHT
  CALL_C(b_+116, setGlobalFlag_hook, SYM(setGlobalFlag), b_+119);
  CYC(b_+119, b_+120); alu_xor(gb, A);
  CYC(b_+120, b_+123); W8(wDisabledObjects) = A;
  CYC(b_+123, b_+126); W8(wMenuDisabled) = A;
  CYC(b_+126, b_+127); A = alu_inc8(gb, A);
  CYC(b_+127, b_+130); W8(wLoadedTreeGfxIndex) = A;
  CYC(b_+130, b_+133); TAIL(interactionDelete); // jp
}

// possessedNayru_ghost: also relies on E already holding INTERACTION_BASE + OBJ_STATE, set by
// interactionCode6d's dispatcher.
void possessedNayru_ghost_hook(GB *gb) {
  BASE(possessedNayru_ghost);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = possessedNayru_jump_table(gb);
    if (target == b_+8) goto state0;
    if (target == b_+19) goto state1;
    goto state2;
  }

state0:
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, interactionIncState_hook, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xfc); // -4
  RET(b_+18); return; // ret

state1:
  CYC(b_+19, b_+21); A = 0x37; // Object.var37
  CALL_C(b_+21, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+26); return; } // ret z
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+28, interactionIncState_hook, SYM(interactionIncState), b_+31);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(b_+35, objectSetVisible81_hook, SYM(objectSetVisible81), b_+38);
  CYC(b_+38, b_+41); SET_HL((SYM(miscPuzzles_subid1a) + 1)); // mainScripts.possessedNayru_veranGhostScript
  CYC(b_+41, b_+44); TAIL(interactionSetScript); // jp

state2:
  CALL_C(b_+44, interactionRunScript_hook, SYM(interactionRunScript), b_+47);
  if (!(F & FC)) { CYCT(b_+47, b_+50); interactionAnimate_hook(gb); return; } // jp nc
  CYC(b_+47, b_+50);
  CYC(b_+50, b_+52); A = 0x37; // Object.var37
  CALL_C(b_+52, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+55);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x00);
  CYC(b_+57, b_+60); TAIL(interactionDelete); // jp
}

// INTERAC_POSSESSED_NAYRU
void interactionCode6d_hook(GB *gb) {
  BASE(interactionCode6d);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = possessedNayru_jump_table(gb);
    if (target == SYM(possessedNayru_subid00)) { possessedNayru_subid00_hook(gb); return; }
    TAIL(possessedNayru_ghost); // target == 0x548e (subid01 and subid02 alias here)
  }
}
