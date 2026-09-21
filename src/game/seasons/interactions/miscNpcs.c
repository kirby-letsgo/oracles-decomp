#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscNpcs.s, bank $08.

// The shared "run the script, then animate" state of the Horon Village NPCs: an NPC whose
// Interaction.var03 has bit 7 set animates on its own, the others face Link.
void s_miscNPC_state1_hook(GB *gb) {
  BASE(miscNPC_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = 0x43; // Interaction.var03
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); TAIL(interactionAnimateAsNpc); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL(npcFaceLinkAndAnimate);
}

static void misc_npc_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Init of every NPC that shares this code (ids $24..). Bit 7 of the subid moves to var03; the
// Horon Village and Sunken City checks may delete the NPC or pick its subid; the script comes
// from miscNPC_scriptTable[id-$24][subid]; a few ids get an extra setup call.
void s_miscNPC_state0_hook(GB *gb) {
  BASE(miscNPC_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = 0x42; // Interaction.subid
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+9); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto visibility; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+12, b_+14); alu_and(gb, 0x7f);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);

visibility:
  CALL_C(b_+15, s_checkHoronVillageNPCShouldBeSeen_hook, SYM(checkHoronVillageNPCShouldBeSeen), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto sunken_city; }
  CYC(b_+18, b_+20);
  if (!(F & FC)) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CYCT(b_+23, b_+25); goto set_subid;

sunken_city:
  CALL_C(b_+25, s_getSunkenCityNPCVisibleSubId_hook, SYM(getSunkenCityNPCVisibleSubId), b_+28);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto graphics; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); E = 0x42; // Interaction.subid
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+34, b_+37); TAIL(interactionDelete); }
  CYC(b_+34, b_+37);

set_subid:
  CYC(b_+37, b_+39); E = 0x42; // Interaction.subid
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);

graphics:
  CALL_C(b_+41, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+44);
  CYC(b_+44, b_+46); E = 0x41; // Interaction.id
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+49); alu_cp(gb, 0x24); // INTERAC_MAYORS_HOUSE_NPC
  if (!(F & FZ)) { CYCT(b_+49, b_+51); goto script; }
  CYC(b_+49, b_+51);
  CALL_C(b_+51, s_checkMayorsHouseNPCshouldBeSeen_hook, SYM(checkMayorsHouseNPCshouldBeSeen), b_+54);
  if (F & FZ) { CYCT(b_+54, b_+57); TAIL(interactionDelete); }
  CYC(b_+54, b_+57);

script:
  CYC(b_+57, b_+59); alu_sub(gb, 0x24);
  CYC(b_+59, b_+62); SET_HL(SYM(miscNPC_scriptTable));
  CYC(b_+62, b_+63); misc_npc_add_double_index(gb, b_+63);
  CYC(b_+63, b_+64); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+64, b_+65); H = mem_rd(gb, HL);
  CYC(b_+65, b_+66); L = A;
  CYC(b_+66, b_+68); E = 0x42; // Interaction.subid
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); misc_npc_add_double_index(gb, b_+70);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+71, b_+72); H = mem_rd(gb, HL);
  CYC(b_+72, b_+73); L = A;
  CALL_C(b_+73, interactionSetScript_hook, SYM(interactionSetScript), b_+76);
  CYC(b_+76, b_+78); E = 0x41; // Interaction.id
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+81); alu_cp(gb, 0x38); // INTERAC_DUNGEON_WISE_OLD_MAN
  if (F & FZ) { CYCT(b_+81, b_+84); TAIL_S(dungeonWiseOldMan_textLookup); }
  CYC(b_+81, b_+84);
  CYC(b_+84, b_+86); alu_cp(gb, 0x2c); // INTERAC_MR_WRITE
  if (F & FZ) { CYCT(b_+86, b_+89); TAIL_S(mrWrite_spawnLightableTorch); }
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+91); alu_cp(gb, 0x33); // INTERAC_BATHING_SUBROSIANS
  if (F & FZ) CALL_C_CC(b_+91, s_func_572c_hook, SYM(func_572c), b_+94);
  else CYC(b_+91, b_+94);
  CYC(b_+94, b_+96); E = 0x41; // Interaction.id
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+99); alu_cp(gb, 0x36); // INTERAC_MASTER_DIVERS_SON
  if (F & FZ) CALL_C_CC(b_+99, s_func_572c_hook, SYM(func_572c), b_+102);
  else CYC(b_+99, b_+102);
  TAIL_S(func_5723);
}

static void misc_npc_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// Clears var38/var39 and animates as an NPC (the common end of state 0).
void s_func_5723_hook(GB *gb) {
  BASE(func_5723);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+2); H = D;
  CYC(b_+2, b_+4); L = 0x78; // Interaction.var38
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+9);
  TAIL(interactionAnimateAsNpc);
}

// Runs the script twice (the bathing Subrosians and the master diver's son skip their first
// script frame).
void s_func_572c_hook(GB *gb) {
  BASE(func_572c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6);
  TAIL(interactionRunScript);
}

// Mr. Write's torch becomes lightable until room flag bit 6 is set.
void s_mrWrite_spawnLightableTorch_hook(GB *gb) {
  BASE(mrWrite_spawnLightableTorch);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+5, b_+7); goto spawn; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10);
  TAIL_S(func_5723);

spawn:
  CALL_C(b_+10, getFreePartSlot_hook, SYM(getFreePartSlot), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto done; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x06); // PART_LIGHTABLE_TORCH
  CYC(b_+17, b_+19); L = 0xcb; // Part.yh
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x38);
  CYC(b_+21, b_+23); L = 0xcd; // Part.xh
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x68);
done:
  CYC(b_+25, b_+28);
  TAIL_S(func_5723);
}

// Subid 0 of the dungeon wise old man picks his text (TX_33xx) by dungeon index.
void s_dungeonWiseOldMan_textLookup_hook(GB *gb) {
  BASE(dungeonWiseOldMan_textLookup);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42; // Interaction.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto ret; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  CYC(b_+10, b_+12); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+12, b_+14); goto lookup; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
lookup:
  CYC(b_+15, b_+18); SET_HL(b_+30); // @textLookup
  CYC(b_+18, b_+19); misc_npc_add_a_to_hl(gb, b_+19);
  CYC(b_+19, b_+21); E = 0x72; // Interaction.textID
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+26); A = 0x33;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
ret:
  CYC(b_+27, b_+30);
  TAIL_S(func_5723);
}

// Z set if the mayor's-house NPC should not be seen: the mayor (subid 3) leaves once the
// villagers have been seen in a linked game or the game is finished; the woman (id xor 1)
// appears then instead.
void s_checkMayorsHouseNPCshouldBeSeen_hook(GB *gb) {
  BASE(checkMayorsHouseNPCshouldBeSeen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42; // Interaction.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CALL_C(b_+4, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+7);
  if (F & FZ) { CYCT(b_+7, b_+9); goto unlinked; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x1e; // GLOBALFLAG_ZELDA_VILLAGERS_SEEN
  CALL_C(b_+11, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+14);
  if (F & FZ) { CYCT(b_+14, b_+16); goto xor01_if_mayor; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = 0x1f; // GLOBALFLAG_ZELDA_KIDNAPPED_SEEN
  CALL_C(b_+18, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+21);
  if (F & FZ) { CYCT(b_+21, b_+23); goto xor_a; }
  CYC(b_+21, b_+23);
  CYCT(b_+23, b_+25); goto xor01_if_mayor;

unlinked:
  CYC(b_+25, b_+27); A = 0x28; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+27, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+30);
  if (F & FZ) { CYCT(b_+30, b_+32); goto xor01_if_mayor; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+35); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto xor_a; }
  CYC(b_+35, b_+37);
  CYCT(b_+37, b_+39); goto xor01;

xor01_if_mayor:
  CYC(b_+39, b_+40); A = B;
  CYC(b_+40, b_+42); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+42, b_+44); goto xor_a; }
  CYC(b_+42, b_+44);
xor01:
  CYC(b_+44, b_+46); E = 0x41; // Interaction.id
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+49); alu_xor(gb, 0x01);
  RET(b_+49); return;

xor_a:
  CYC(b_+50, b_+51); alu_xor(gb, A);
  RET(b_+51); return;
}

// @main, also called from other banks: b indexes conditionalHoronNPCLookupTable, then
// the subid picks a zero-terminated list of stages (+1) at which the NPC is seen; carry set
// when the current stage is listed.
void s_checkHoronVillageNPCShouldBeSeen_body__main_hook(GB *gb) {
  BASE(checkHoronVillageNPCShouldBeSeen_body);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13); SET_HL(SYM(conditionalHoronNPCLookupTable));
  CYC(b_+13, b_+14); misc_npc_add_double_index(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+18); push_effect(gb, HL);
  CALL_C(b_+18, s_checkNPCStage_hook, SYM(checkNPCStage), b_+21);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+24); E = 0x42; // Interaction.subid
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); misc_npc_add_double_index(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); H = mem_rd(gb, HL);
  CYC(b_+28, b_+29); L = A;
  for (;;) {
    CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+30, b_+31); alu_or(gb, A);
    if (F & FZ) { RET_TAKEN(b_+31); return; }
    CYC(b_+31, b_+32);
    CYC(b_+32, b_+33); A = alu_dec8(gb, A);
    CYC(b_+33, b_+34); alu_cp(gb, B);
    if (!(F & FZ)) { CYCT(b_+34, b_+36); continue; }
    CYC(b_+34, b_+36);
    break;
  }
  CYC(b_+36, b_+37); alu_scf(gb);
  RET(b_+37); return;
}

// checkHoronVillageNPCShouldBeSeen_body@scf
static void horon_npc_scf(GB *gb) {
  BASE(checkHoronVillageNPCShouldBeSeen_body);
  CYC(b_+36, b_+37); alu_scf(gb);
  RET(b_+37); return;
}

// The fickle lady (b=0) and fickle man (b=1) go straight to the lookup; the man's subid 6 is
// always seen; other ids continue in the body.
void s_checkHoronVillageNPCShouldBeSeen_hook(GB *gb) {
  BASE(checkHoronVillageNPCShouldBeSeen);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x41; // Interaction.id
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); B = 0x00;
  CYC(b_+5, b_+7); alu_cp(gb, 0x2d); // INTERAC_FICKLE_LADY
  if (F & FZ) { CYCT(b_+7, b_+9); s_checkHoronVillageNPCShouldBeSeen_body__main_hook(gb); return; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CYC(b_+10, b_+12); alu_cp(gb, 0x37); // INTERAC_FICKLE_MAN
  if (!(F & FZ)) { CYCT(b_+12, b_+14); TAIL_S(checkHoronVillageNPCShouldBeSeen_body); }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); E = 0x42; // Interaction.subid
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); s_checkHoronVillageNPCShouldBeSeen_body__main_hook(gb); return; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); B = 0x0b;
  CYCT(b_+23, b_+25); horon_npc_scf(gb); return;
}

// Ids $3c (b=2) and $3d (b=3) are conditional; anything else returns NZ (not conditional).
void s_checkHoronVillageNPCShouldBeSeen_body_hook(GB *gb) {
  BASE(checkHoronVillageNPCShouldBeSeen_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = alu_inc8(gb, B);
  CYC(b_+1, b_+3); alu_cp(gb, 0x3c);
  if (F & FZ) { CYCT(b_+3, b_+5); s_checkHoronVillageNPCShouldBeSeen_body__main_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); B = alu_inc8(gb, B);
  CYC(b_+6, b_+8); alu_cp(gb, 0x3d);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  s_checkHoronVillageNPCShouldBeSeen_body__main_hook(gb); return;
}

// b = the game's stage for the conditional NPCs: 0 no essences and Maku tree not met, 1 met,
// 2 first essence, 3 second (and Zelda saved from Vire if linked), 4 five essences, 5 eighth,
// 6 Zelda's villagers seen, 7 Maku seed, 8 Zelda kidnapped, 9 second essence but Zelda not yet
// saved (linked), $0a game finished. A is cleared.
void s_checkNPCStage_hook(GB *gb) {
  BASE(checkNPCStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x28; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  CYC(b_+5, b_+7); B = 0x0a;
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto clear_a; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+11, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+14);
  if (F & FC) { CYCT(b_+14, b_+16); goto essence_gotten; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = 0x18; // GLOBALFLAG_GNARLED_KEY_GIVEN
  CALL_C(b_+18, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+21);
  CYC(b_+21, b_+23); B = 0x01;
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto clear_a; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); B = 0x00;
clear_a:
  CYC(b_+27, b_+28); alu_xor(gb, A);
  RET(b_+28); return;

essence_gotten:
  CYC(b_+29, b_+30); C = A;
  CALL_C(b_+30, getNumSetBits_hook, SYM(getNumSetBits), b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, hFF8B, A);
  CYC(b_+35, b_+36); A = C;
  CALL_C(b_+36, getHighestSetBit_hook, SYM(getHighestSetBit), b_+39);
  CYC(b_+39, b_+40); C = A;
  CALL_C(b_+40, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+43);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto linked_game; }
  CYC(b_+43, b_+45);

regular_check:
  CYC(b_+45, b_+46); A = C;
  CYC(b_+46, b_+48); B = 0x05;
  CYC(b_+48, b_+50); alu_cp(gb, 0x07);
  if (!(F & FC)) { RET_TAKEN(b_+50); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); B = alu_dec8(gb, B);
  CYC(b_+52, b_+54); A = mem_rd(gb, hFF8B);
  CYC(b_+54, b_+56); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+56); return; }
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+58); A = C;
  CYC(b_+58, b_+59); B = alu_dec8(gb, B);
  CYC(b_+59, b_+61); alu_cp(gb, 0x01);
  if (!(F & FC)) { RET_TAKEN(b_+61); return; }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+63); B = alu_dec8(gb, B);
  RET(b_+63); return;

linked_game:
  CYC(b_+64, b_+66); A = 0x1f; // GLOBALFLAG_ZELDA_KIDNAPPED_SEEN
  CALL_C(b_+66, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+69);
  CYC(b_+69, b_+71); B = 0x08;
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+74); A = 0x19; // GLOBALFLAG_GOT_MAKU_SEED
  CALL_C(b_+74, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+77);
  CYC(b_+77, b_+79); B = 0x07;
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); A = 0x1e; // GLOBALFLAG_ZELDA_VILLAGERS_SEEN
  CALL_C(b_+82, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+85);
  CYC(b_+85, b_+87); B = 0x06;
  if (!(F & FZ)) { RET_TAKEN(b_+87); return; }
  CYC(b_+87, b_+88);
  CYC(b_+88, b_+89); A = C;
  CYC(b_+89, b_+91); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+91, b_+93); goto regular_check; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); A = mem_rd(gb, hFF8B);
  CYC(b_+95, b_+97); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+97, b_+99); goto regular_check; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); B = 0x09;
  CYC(b_+101, b_+103); A = 0x23; // GLOBALFLAG_ZELDA_SAVED_FROM_VIRE
  CALL_C(b_+103, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+106);
  if (F & FZ) { RET_TAKEN(b_+106); return; }
  CYC(b_+106, b_+107);
  CYC(b_+107, b_+109); B = 0x03;
  RET(b_+109); return;
}

// For the master diver's son ($36) and treasure hunter ($39): b = 4 game finished, 3 Zelda
// kidnapped seen, 2 eighth essence, 1 fourth essence, 0 none; A cleared. Other ids return
// NZ with a = $ff.
void s_getSunkenCityNPCVisibleSubId_hook(GB *gb) {
  BASE(getSunkenCityNPCVisibleSubId);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x41; // Interaction.id
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x36); // INTERAC_MASTER_DIVERS_SON
  if (F & FZ) { CYCT(b_+5, b_+7); s_getSunkenCityNPCVisibleSubId__main_hook(gb); return; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x39); // INTERAC_TREASURE_HUNTER
  if (F & FZ) { CYCT(b_+9, b_+11); s_getSunkenCityNPCVisibleSubId__main_hook(gb); return; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0xff;
  RET(b_+13); return;
}

// @main, also called from other banks (the stage check without the id test).
void s_getSunkenCityNPCVisibleSubId__main_hook(GB *gb) {
  BASE(getSunkenCityNPCVisibleSubId);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+14, b_+16); A = 0x28; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+16, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+19);
  CYC(b_+19, b_+21); B = 0x04;
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto clear_a; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x1f; // GLOBALFLAG_ZELDA_KIDNAPPED_SEEN
  CALL_C(b_+25, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+28);
  CYC(b_+28, b_+30); B = 0x03;
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto clear_a; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+34, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+37);
  CYC(b_+37, b_+39); B = 0x00;
  if (!(F & FC)) { CYCT(b_+39, b_+41); goto clear_a; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); C = A;
  CALL_C(b_+42, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+45);
  if (F & FZ) CYCT(b_+45, b_+47);
  else CYC(b_+45, b_+47);
  CYC(b_+47, b_+48); A = C;
  CALL_C(b_+48, getHighestSetBit_hook, SYM(getHighestSetBit), b_+51);
  CYC(b_+51, b_+53); B = 0x02;
  CYC(b_+53, b_+55); alu_cp(gb, 0x07);
  if (!(F & FC)) { RET_TAKEN(b_+55); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+57); B = alu_dec8(gb, B);
  CYC(b_+57, b_+58); A = C;
  CYC(b_+58, b_+60); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto clear_a; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); B = alu_dec8(gb, B);
clear_a:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;
}
