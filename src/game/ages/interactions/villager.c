#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Script pointer tables (bank $0c mainScripts.*), indexed by subid or game progress.
#define interactionCode3a_scriptTable_bank08 0x76dc
#define interactionCode3a_subid03ScriptTable_bank08 0x76f8
#define interactionCode3a_subid4And5ScriptTable_bank08 0x7704
#define interactionCode3a_subid6And7ScriptTable_bank08 0x7710
#define interactionCode3a_subid08ScriptTable_bank08 0x7720

// Cross-bank targets referenced by address only.
#define getGameProgress_1_bank09 0x552b
#define getGameProgress_2_bank09 0x5559
#define interactionOscillateXRandomly_bank08 0x5d87
#define setCounter1To120AndPlaySoundEffectAndIncSubstate_bank08 0x7b54
#define villagerSubid02Script_part2_bank0c 0x5b54

static uint16_t villager_jumpTable(GB *gb) {
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

static void villager_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @loadScript: pick the script for this subid from @scriptTable. Reached by static jumps
// and one static `call` (@initSubid0d) from interactionCode3a itself.
static void villager_loadScript(GB *gb) {
  CYC(0x76cf, 0x76d1); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x76d1, 0x76d2); A = mem_rd(gb, DE);
  CYC(0x76d2, 0x76d5); SET_HL(interactionCode3a_scriptTable_bank08);
  CYC(0x76d5, 0x76d6); villager_addDoubleIndex(gb, 0x76d6);
  CYC(0x76d6, 0x76d7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76d7, 0x76d8); H = mem_rd(gb, HL);
  CYC(0x76d8, 0x76d9); L = A;
  CYC(0x76d9, 0x76dc); interactionSetScript_hook(gb);
}

// @saveXAndLoadScript: remember xh in var3d, then fall into @initSubid01 (jp @loadScript).
// Reached by fallthrough from @initSubid02 and a static `call` from @runSubid02@substate1.
static void villager_saveXAndLoadScript(GB *gb) {
  CYC(0x7496, 0x7498); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x7498, 0x7499); A = mem_rd(gb, DE);
  CYC(0x7499, 0x749b); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x749b, 0x749c); mem_wr(gb, DE, A);
  // @initSubid01
  CYC(0x749c, 0x749f); villager_loadScript(gb);
}

// @loadStoneAnimation: stone palette, radius and animation. H must already be D.
// Reached by `jr` (@initSubid0a), fallthrough (@initSubid0c) and `call` (@initSubid0b).
static void villager_loadStoneAnimation(GB *gb, uint16_t sp0_) {
  CYC(0x752d, 0x752f); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x752f, 0x7531); mem_wr(gb, HL, 0x06);
  CYC(0x7531, 0x7533); A = 0x06;
  CALL_C(0x7533, objectSetCollideRadius_hook, 0x24a1, 0x7536);
  CYC(0x7536, 0x7538); A = 0x0d;
  CYC(0x7538, 0x753b); interactionSetAnimation_hook(gb);
}

// @state1: per-subid update. Also reached by fallthrough/jr from @initSubid0b/0d/0e.
static void villager_state1(GB *gb, uint16_t sp0_) {
  CYC(0x7558, 0x755a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x755a, 0x755b); A = mem_rd(gb, DE);
  CYC(0x755b, 0x755c); push_effect(gb, 0x755c);
  switch (villager_jumpTable(gb)) {
    case 0x757a: goto runSubid00;
    case 0x75c8: goto runSubid01;
    case 0x75ce: goto runSubid02;
    case 0x762f: goto runScriptAndFaceLink;
    case 0x7635: goto runSubid09;
    case 0x7658: goto plainRet;
    case 0x7659: goto runSubid0b;
    case 0x76a1: goto runSubid0c;
    case 0x76c2: goto runSubid0d;
    case 0x76ce: goto runSubid0e;
    default: HANDOFF(HL);
  }

runSubid00:
  // Cutscene where guy is struck by lightning in intro
  CYC(0x757a, 0x757c); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x757c, 0x757d); A = mem_rd(gb, DE);
  CYC(0x757d, 0x757e); push_effect(gb, 0x757e);
  switch (villager_jumpTable(gb)) {
    case 0x7584: goto subid00_substate0;
    case 0x759a: goto subid00_substate1;
    case 0x75bc: goto subid00_substate2;
    default: HANDOFF(HL);
  }

subid00_substate0:
  CYC(0x7584, 0x7587); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x7587, 0x7589); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7589, 0x758c); interactionAnimate_hook(gb); return;
  }
  CYC(0x7589, 0x758c);
  CALL_C(0x758c, interactionIncSubstate_hook, 0x23e5, 0x758f);
  CYC(0x758f, 0x7591); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7591, 0x7593); mem_wr(gb, HL, 0x3c);
  CYC(0x7593, 0x7595); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x7595, 0x7596); A = mem_rd(gb, HL);
  CYC(0x7596, 0x7598); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7598, 0x7599); mem_wr(gb, HL, A);
  CYC(0x7599, 0x759a); ret_effect(gb);
  return;

subid00_substate1:
  // callab interactionOscillateXRandomly
  CYC(0x759a, 0x759d); SET_HL(interactionOscillateXRandomly_bank08);
  CYC(0x759d, 0x759f); E = 0x08;
  CALL_C(0x759f, interBankCall_hook, 0x008a, 0x75a2);
  CYC(0x75a2, 0x75a5); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x75a5, 0x75a7); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x75a7, 0x75a8); ret_effect(gb); return;
  }
  CYC(0x75a7, 0x75a8);
  CALL_C(0x75a8, interactionIncSubstate_hook, 0x23e5, 0x75ab);
  CYC(0x75ab, 0x75ad); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x75ad, 0x75af); mem_wr(gb, HL, 0x10);
  CALL_C(0x75af, getFreePartSlot_hook, 0x3e8e, 0x75b2);
  if (!(F & FZ)) {
    CYCT(0x75b2, 0x75b3); ret_effect(gb); return;
  }
  CYC(0x75b2, 0x75b3);
  CYC(0x75b3, 0x75b5); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(0x75b5, 0x75b6); L = alu_inc8(gb, L);
  CYC(0x75b6, 0x75b7); mem_wr(gb, HL, E);
  CYC(0x75b7, 0x75b8); L = alu_inc8(gb, L);
  CYC(0x75b8, 0x75b9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x75b9, 0x75bc); objectCopyPosition_hook(gb);
  return;

subid00_substate2:
  CALL_C(0x75bc, interactionDecCounter1_hook, 0x23cc, 0x75bf);
  if (!(F & FZ)) {
    CYCT(0x75bf, 0x75c0); ret_effect(gb); return;
  }
  CYC(0x75bf, 0x75c0);
  CYC(0x75c0, 0x75c2); A = 0x04;
  CYC(0x75c2, 0x75c5); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(0x75c5, 0x75c8); interactionDelete_hook(gb);
  return;

runSubid01:
  // Past villager?
  CALL_C(0x75c8, interactionRunScript_hook, 0x2552, 0x75cb);
  CYC(0x75cb, 0x75ce); interactionAnimateAsNpc_hook(gb);
  return;

runSubid02:
  // Construction worker blocking path to upper part of black tower.
  CYC(0x75ce, 0x75d0); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x75d0, 0x75d1); A = mem_rd(gb, DE);
  CYC(0x75d1, 0x75d2); push_effect(gb, 0x75d2);
  switch (villager_jumpTable(gb)) {
    case 0x75d6: goto subid02_substate0;
    case 0x7615: goto subid02_substate1;
    default: HANDOFF(HL);
  }

subid02_substate0:
  CALL_C(0x75d6, npcFaceLinkAndAnimate_hook, 0x26a9, 0x75d9);
  CALL_C(0x75d9, interactionRunScript_hook, 0x2552, 0x75dc);
  CYC(0x75dc, 0x75df); SET_BC(0x0503);
  CALL_C(0x75df, objectSetCollideRadii_hook, 0x24a9, 0x75e2);
  // Temporarily overwrite the worker's X position to check for "collision" at the
  // position he's left open. His position will be reverted before returning.
  CYC(0x75e2, 0x75e4); B = 0x11;
  CYC(0x75e4, 0x75e6); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x75e6, 0x75e7); A = mem_rd(gb, DE);
  CYC(0x75e7, 0x75e8); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x75e8, 0x75ea);
  } else {
    CYC(0x75e8, 0x75ea);
    CYC(0x75ea, 0x75ec); B = 0xef;
  }
  CYC(0x75ec, 0x75ee); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x75ee, 0x75ef); A = mem_rd(gb, DE);
  CYC(0x75ef, 0x75f0); alu_add(gb, B);
  CYC(0x75f0, 0x75f1); mem_wr(gb, DE, A);
  CYC(0x75f1, 0x75f2); push_effect(gb, BC);
  CALL_C(0x75f2, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x75f5);
  CYC(0x75f5, 0x75f6); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x75f6, 0x75f8); goto subid02_restorePosition;
  }
  CYC(0x75f6, 0x75f8);
  // Link tried to approach; move over to block his path
  CALL_C(0x75f8, interactionIncSubstate_hook, 0x23e5, 0x75fb);
  CYC(0x75fb, 0x75fe); SET_HL(villagerSubid02Script_part2_bank0c);
  CALL_C(0x75fe, interactionSetScript_hook, 0x2544, 0x7601);
subid02_restorePosition:
  CYC(0x7601, 0x7604); SET_HL(w1Link_yh);
  CYC(0x7604, 0x7606); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x7606, 0x7607); A = mem_rd(gb, HL);
  CYC(0x7607, 0x7608); mem_wr(gb, DE, A);
  CYC(0x7608, 0x760b); SET_BC(0x0606);
  CALL_C(0x760b, objectSetCollideRadii_hook, 0x24a9, 0x760e);
  CYC(0x760e, 0x7610); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7610, 0x7611); A = mem_rd(gb, DE);
  CYC(0x7611, 0x7613); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x7613, 0x7614); mem_wr(gb, DE, A);
  CYC(0x7614, 0x7615); ret_effect(gb);
  return;

subid02_substate1:
  CALL_C(0x7615, interactionAnimateAsNpc_hook, 0x26db, 0x7618);
  CALL_C(0x7618, interactionRunScript_hook, 0x2552, 0x761b);
  if (!(F & FC)) {
    CYCT(0x761b, 0x761e); interactionAnimateBasedOnSpeed_hook(gb); return;
  }
  CYC(0x761b, 0x761e);
  CYC(0x761e, 0x7621); push_effect(gb, 0x7621); villager_saveXAndLoadScript(gb);
  CYC(0x7621, 0x7622); H = D;
  CYC(0x7622, 0x7624); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7624, 0x7625); A = mem_rd(gb, HL);
  CYC(0x7625, 0x7627); alu_xor(gb, 0x01);
  CYC(0x7627, 0x7628); mem_wr(gb, HL, A);
  CYC(0x7628, 0x762a); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x762a, 0x762c); mem_wr(gb, HL, 0x00);
  CYC(0x762c, 0x762f); villager_loadScript(gb);
  return;

runScriptAndFaceLink:
  CALL_C(0x762f, interactionRunScript_hook, 0x2552, 0x7632);
  CYC(0x7632, 0x7635); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid09:
  CYC(0x7635, 0x7637); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7637, 0x7638); A = mem_rd(gb, DE);
  CYC(0x7638, 0x7639); push_effect(gb, 0x7639);
  switch (villager_jumpTable(gb)) {
    case 0x763f: goto subid09_substate0;
    case 0x764b: goto subid09_substate1;
    case 0x7658: goto plainRet;
    default: HANDOFF(HL);
  }

subid09_substate0:
  CALL_C(0x763f, interactionRunScript_hook, 0x2552, 0x7642);
  CYC(0x7642, 0x7645); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x7645, 0x7647); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x7647, 0x7648); ret_effect(gb); return;
  }
  CYC(0x7647, 0x7648);
  CYC(0x7648, 0x764b); interactionIncSubstate_hook(gb);
  return;

subid09_substate1:
  CYC(0x764b, 0x764e); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(0x764e, 0x7650); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7650, 0x7651); ret_effect(gb); return;
  }
  CYC(0x7650, 0x7651);
  CALL_C(0x7651, interactionIncSubstate_hook, 0x23e5, 0x7654);
  CYC(0x7654, 0x7656); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7656, 0x7658); mem_wr(gb, HL, 0x06);

plainRet:
  // @ret
  CYC(0x7658, 0x7659); ret_effect(gb);
  return;

runSubid0b:
  // Villager being restored from stone, resumes playing catch
  CYC(0x7659, 0x765b); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x765b, 0x765c); A = mem_rd(gb, DE);
  CYC(0x765c, 0x765d); push_effect(gb, 0x765d);
  switch (villager_jumpTable(gb)) {
    case 0x7663: goto subid0b_substate0;
    case 0x7679: goto subid0b_substate1;
    case 0x7697: goto subid0b_substate2;
    default: HANDOFF(HL);
  }

subid0b_substate0:
  CALL_C(0x7663, interactionDecCounter1IfPaletteNotFading_hook, 0x2744, 0x7666);
  if (!(F & FZ)) {
    CYCT(0x7666, 0x7667); ret_effect(gb); return;
  }
  CYC(0x7666, 0x7667);
  CYC(0x7667, 0x7669); A = 0x01;
  CYC(0x7669, 0x766c); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(0x766c, 0x766e); A = 0xc1; // SND_RESTORE
  CALL_C(0x766e, playSound_b00_hook, 0x0c98, 0x7671);
  // jpab setCounter1To120AndPlaySoundEffectAndIncSubstate
  CYC(0x7671, 0x7674); SET_HL(setCounter1To120AndPlaySoundEffectAndIncSubstate_bank08);
  CYC(0x7674, 0x7676); E = 0x08;
  CYC(0x7676, 0x7679); interBankCall_hook(gb);
  return;

subid0b_substate1:
  CALL_C(0x7679, interactionDecCounter1_hook, 0x23cc, 0x767c);
  if (!(F & FZ)) {
    CYCT(0x767c, 0x767e); goto subid0b_flickerPalette;
  }
  CYC(0x767c, 0x767e);
  CALL_C(0x767e, interactionIncSubstate_hook, 0x23e5, 0x7681);
  CYC(0x7681, 0x7683); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7683, 0x7685); mem_wr(gb, HL, 0x01);
  CYC(0x7685, 0x7688); villager_loadScript(gb);
  return;
subid0b_flickerPalette:
  // Flicker palette every 8 frames
  CYC(0x7688, 0x768b); A = mem_rd(gb, wFrameCounter);
  CYC(0x768b, 0x768d); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(0x768d, 0x768e); ret_effect(gb); return;
  }
  CYC(0x768d, 0x768e);
  CYC(0x768e, 0x7690); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7690, 0x7691); A = mem_rd(gb, DE);
  CYC(0x7691, 0x7692); A = alu_dec8(gb, A);
  CYC(0x7692, 0x7694); alu_xor(gb, 0x05);
  CYC(0x7694, 0x7695); A = alu_inc8(gb, A);
  CYC(0x7695, 0x7696); mem_wr(gb, DE, A);
  CYC(0x7696, 0x7697); ret_effect(gb);
  return;

subid0b_substate2:
  CYC(0x7697, 0x7699); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x7699, 0x769a); A = mem_rd(gb, DE);
  CYC(0x769a, 0x769b); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x769b, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x769e);
  } else {
    CYC(0x769b, 0x769e);
  }
  CYC(0x769e, 0x76a1); interactionRunScript_hook(gb);
  return;

runSubid0c:
  // Villager playing catch with son
  CALL_C(0x76a1, interactionPushLinkAwayAndUpdateDrawPriority_hook, 0x26de, 0x76a4);
  CYC(0x76a4, 0x76a6); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x76a6, 0x76a7); A = mem_rd(gb, DE);
  CYC(0x76a7, 0x76a8); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x76a8, 0x76a9); ret_effect(gb); return;
  }
  CYC(0x76a8, 0x76a9);
  CALL_C(0x76a9, interactionRunScript_hook, 0x2552, 0x76ac);
  // If you press the A button, show text
  CYC(0x76ac, 0x76ae); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x76ae, 0x76af); A = mem_rd(gb, DE);
  CYC(0x76af, 0x76b0); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x76b0, 0x76b1); ret_effect(gb); return;
  }
  CYC(0x76b0, 0x76b1);
  CYC(0x76b1, 0x76b2); alu_xor(gb, A);
  CYC(0x76b2, 0x76b3); mem_wr(gb, DE, A);
  CYC(0x76b3, 0x76b6); SET_BC(0x1442); // TX_1442
  CYC(0x76b6, 0x76b9); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x76b9, 0x76bb); alu_bit(gb, 7, mem_rd(gb, HL)); // Has Veran been beaten?
  if (F & FZ) {
    CYCT(0x76bb, 0x76bd);
  } else {
    CYC(0x76bb, 0x76bd);
    CYC(0x76bd, 0x76bf); C = 0x43; // <TX_1443
  }
  CYC(0x76bf, 0x76c2); showText_hook(gb);
  return;

runSubid0d:
  // Cutscene when you first enter the past
  CALL_C(0x76c2, interactionRunScript_hook, 0x2552, 0x76c5);
  if (F & FC) {
    CYCT(0x76c5, 0x76c8); interactionDelete_hook(gb); return;
  }
  CYC(0x76c5, 0x76c8);
  CALL_C(0x76c8, interactionAnimateBasedOnSpeed_hook, 0x2758, 0x76cb);
  CYC(0x76cb, 0x76ce); interactionPushLinkAwayAndUpdateDrawPriority_hook(gb);
  return;

runSubid0e:
  // Stone villager? Not much to do.
  CYC(0x76ce, 0x76cf); ret_effect(gb);
}

// @initSubid: per-subid initialization. Reached only by a static `call` from
// interactionCode3a@state0 (NOT HOOKED).
static void villager_initSubid(GB *gb, uint16_t sp0_) {
  CYC(0x7465, 0x7467); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7467, 0x7468); A = mem_rd(gb, DE);
  CYC(0x7468, 0x7469); push_effect(gb, 0x7469);
  switch (villager_jumpTable(gb)) {
    case 0x7487: goto initSubid00;
    case 0x749c: goto initSubid01;
    case 0x748c: goto initSubid02;
    case 0x749f: goto initSubid03;
    case 0x74b2: goto initSubid04And05;
    case 0x74d4: goto initSubid06And07;
    case 0x74f1: goto initSubid08;
    case 0x7513: goto initAnimationAndLoadScript;
    case 0x7510: goto initSubid0a;
    case 0x753b: goto initSubid0b;
    case 0x751b: goto initSubid0c;
    case 0x7546: goto initSubid0d;
    case 0x754b: goto initSubid0e;
    default: HANDOFF(HL);
  }

initSubid00:
  CYC(0x7487, 0x7489); A = 0x03;
  CYC(0x7489, 0x748c); interactionSetAnimation_hook(gb);
  return;

initSubid02:
  CYC(0x748c, 0x748e); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x748e, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x7491);
  CYC(0x7491, 0x7493); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7493, 0x7495); A = 0x28; // SPEED_100
  CYC(0x7495, 0x7496); mem_wr(gb, DE, A);
  villager_saveXAndLoadScript(gb);
  return;

initSubid01:
  CYC(0x749c, 0x749f); villager_loadScript(gb);
  return;

initSubid03:
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(0x749f, 0x74a2); SET_HL(getGameProgress_1_bank09);
  CYC(0x74a2, 0x74a4); E = 0x09;
  CALL_C(0x74a4, interBankCall_hook, 0x008a, 0x74a7);
  CYC(0x74a7, 0x74a8); A = B;
  CYC(0x74a8, 0x74ab); SET_HL(interactionCode3a_subid03ScriptTable_bank08);
  CYC(0x74ab, 0x74ac); villager_addDoubleIndex(gb, 0x74ac);
  CYC(0x74ac, 0x74ad); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74ad, 0x74ae); H = mem_rd(gb, HL);
  CYC(0x74ae, 0x74af); L = A;
  CYC(0x74af, 0x74b2); interactionSetScript_hook(gb);
  return;

initSubid04And05:
  CYC(0x74b2, 0x74b4); A = 0x02;
  CYC(0x74b4, 0x74b6); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x74b6, 0x74b7); mem_wr(gb, DE, A);
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(0x74b7, 0x74ba); SET_HL(getGameProgress_1_bank09);
  CYC(0x74ba, 0x74bc); E = 0x09;
  CALL_C(0x74bc, interBankCall_hook, 0x008a, 0x74bf);
  CYC(0x74bf, 0x74c1); C = 0x04;
  CYC(0x74c1, 0x74c3); A = 0x03;
  CALL_C(0x74c3, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x74c6);
  if (!(F & FZ)) {
    CYCT(0x74c6, 0x74c9); interactionDelete_hook(gb); return;
  }
  CYC(0x74c6, 0x74c9);
  CYC(0x74c9, 0x74ca); A = B;
  CYC(0x74ca, 0x74cd); SET_HL(interactionCode3a_subid4And5ScriptTable_bank08);
  CYC(0x74cd, 0x74ce); villager_addDoubleIndex(gb, 0x74ce);
  CYC(0x74ce, 0x74cf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74cf, 0x74d0); H = mem_rd(gb, HL);
  CYC(0x74d0, 0x74d1); L = A;
  CYC(0x74d1, 0x74d4); interactionSetScript_hook(gb);
  return;

initSubid06And07:
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(0x74d4, 0x74d7); SET_HL(getGameProgress_2_bank09);
  CYC(0x74d7, 0x74d9); E = 0x09;
  CALL_C(0x74d9, interBankCall_hook, 0x008a, 0x74dc);
  CYC(0x74dc, 0x74de); C = 0x06;
  CYC(0x74de, 0x74e0); A = 0x04;
  CALL_C(0x74e0, checkNpcShouldExistAtGameStage_hook, 0x3d78, 0x74e3);
  if (!(F & FZ)) {
    CYCT(0x74e3, 0x74e6); interactionDelete_hook(gb); return;
  }
  CYC(0x74e3, 0x74e6);
  CYC(0x74e6, 0x74e7); A = B;
  CYC(0x74e7, 0x74ea); SET_HL(interactionCode3a_subid6And7ScriptTable_bank08);
  CYC(0x74ea, 0x74eb); villager_addDoubleIndex(gb, 0x74eb);
  CYC(0x74eb, 0x74ec); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x74ec, 0x74ed); H = mem_rd(gb, HL);
  CYC(0x74ed, 0x74ee); L = A;
  CYC(0x74ee, 0x74f1); interactionSetScript_hook(gb);
  return;

initSubid08:
  CYC(0x74f1, 0x74f3); A = 0x03;
  CYC(0x74f3, 0x74f5); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x74f5, 0x74f6); mem_wr(gb, DE, A);
  // Delete if you haven't beaten d7 yet? (callab agesInteractionsBank09.getGameProgress_2)
  CYC(0x74f6, 0x74f9); SET_HL(getGameProgress_2_bank09);
  CYC(0x74f9, 0x74fb); E = 0x09;
  CALL_C(0x74fb, interBankCall_hook, 0x008a, 0x74fe);
  CYC(0x74fe, 0x74ff); A = B;
  CYC(0x74ff, 0x7501); alu_cp(gb, 0x04);
  if (F & FC) {
    CYCT(0x7501, 0x7504); interactionDelete_hook(gb); return;
  }
  CYC(0x7501, 0x7504);
  CYC(0x7504, 0x7506); alu_sub(gb, 0x04);
  CYC(0x7506, 0x7509); SET_HL(interactionCode3a_subid08ScriptTable_bank08);
  CYC(0x7509, 0x750a); villager_addDoubleIndex(gb, 0x750a);
  CYC(0x750a, 0x750b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x750b, 0x750c); H = mem_rd(gb, HL);
  CYC(0x750c, 0x750d); L = A;
  CYC(0x750d, 0x7510); interactionSetScript_hook(gb);
  return;

initSubid0a:
  CYC(0x7510, 0x7511); H = D;
  CYC(0x7511, 0x7513); villager_loadStoneAnimation(gb, sp0_);
  return;

initAnimationAndLoadScript:
  CYC(0x7513, 0x7515); A = 0x01;
  CALL_C(0x7515, interactionSetAnimation_hook, 0x262e, 0x7518);
  CYC(0x7518, 0x751b); villager_loadScript(gb);
  return;

initSubid0c:
  // Check whether the villager should be stone right now: have we beaten Veran?
  CYC(0x751b, 0x751e); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x751e, 0x7520); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x7520, 0x7522); goto initAnimationAndLoadScript;
  }
  CYC(0x7520, 0x7522);
  CYC(0x7522, 0x7525); A = mem_rd(gb, wEssencesObtained);
  CYC(0x7525, 0x7527); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(0x7527, 0x7529); goto initAnimationAndLoadScript;
  }
  CYC(0x7527, 0x7529);
  CYC(0x7529, 0x752a); H = D;
  CYC(0x752a, 0x752c); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x752c, 0x752d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  villager_loadStoneAnimation(gb, sp0_);
  return;

initSubid0b:
  CYC(0x753b, 0x753c); H = D;
  CYC(0x753c, 0x753f); push_effect(gb, 0x753f); villager_loadStoneAnimation(gb, sp0_);
  CYC(0x753f, 0x7541); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7541, 0x7543); A = 0x3c;
  CYC(0x7543, 0x7544); mem_wr(gb, DE, A);
  CYC(0x7544, 0x7546); villager_state1(gb, sp0_);
  return;

initSubid0d:
  CYC(0x7546, 0x7549); push_effect(gb, 0x7549); villager_loadScript(gb);
  CYC(0x7549, 0x754b); villager_state1(gb, sp0_);
  return;

initSubid0e:
  CALL_C(0x754b, loadStoneNpcPalette_hook, 0x7de5, 0x754e);
  CYC(0x754e, 0x754f); H = D;
  CYC(0x754f, 0x7551); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7551, 0x7553); mem_wr(gb, HL, 0x06);
  CYC(0x7553, 0x7555); A = 0x0d;
  CALL_C(0x7555, interactionSetAnimation_hook, 0x262e, 0x7558);
  villager_state1(gb, sp0_);
}

// INTERAC_MALE_VILLAGER: assorted male NPCs, subid-dispatched.
//   var03: nonzero if he's turned to stone
//   var39: for some subids, animations only update when var39 is zero
//   var3d: saved X position
void interactionCode3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7449, 0x744b); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x744b, 0x744c); A = mem_rd(gb, DE);
  CYC(0x744c, 0x744d); push_effect(gb, 0x744d);
  switch (villager_jumpTable(gb)) {
    case 0x7451: goto state0;
    case 0x7558: villager_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x7451, 0x7453); A = 0x01;
  CYC(0x7453, 0x7454); mem_wr(gb, DE, A);
  CALL_C(0x7454, interactionInitGraphics_hook, 0x15fb, 0x7457);
  CALL_C(0x7457, objectSetVisiblec2_hook, 0x1e45, 0x745a);
  CYC(0x745a, 0x745d); push_effect(gb, 0x745d); villager_initSubid(gb, sp0_);
  CYC(0x745d, 0x745f); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x745f, 0x7460); A = mem_rd(gb, DE);
  CYC(0x7460, 0x7461); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7461, 0x7464); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(0x7461, 0x7464);
  CYC(0x7464, 0x7465); ret_effect(gb);
}
