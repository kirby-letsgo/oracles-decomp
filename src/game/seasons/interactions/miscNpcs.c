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
  CALL_C(b_+15, s_checkHoronVillageNPCShouldBeSeen, SYM(checkHoronVillageNPCShouldBeSeen), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto sunken_city; }
  CYC(b_+18, b_+20);
  if (!(F & FC)) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CYCT(b_+23, b_+25); goto set_subid;

sunken_city:
  CALL_C(b_+25, s_getSunkenCityNPCVisibleSubId, SYM(getSunkenCityNPCVisibleSubId), b_+28);
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
  CALL_C(b_+51, s_checkMayorsHouseNPCshouldBeSeen, SYM(checkMayorsHouseNPCshouldBeSeen), b_+54);
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
  if (F & FZ) { CYCT(b_+81, b_+84); TAIL_SG(dungeonWiseOldMan_textLookup); }
  CYC(b_+81, b_+84);
  CYC(b_+84, b_+86); alu_cp(gb, 0x2c); // INTERAC_MR_WRITE
  if (F & FZ) { CYCT(b_+86, b_+89); TAIL_SG(mrWrite_spawnLightableTorch); }
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+91); alu_cp(gb, 0x33); // INTERAC_BATHING_SUBROSIANS
  if (F & FZ) CALL_C_CC(b_+91, s_func_572c, SYM(func_572c), b_+94);
  else CYC(b_+91, b_+94);
  CYC(b_+94, b_+96); E = 0x41; // Interaction.id
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+99); alu_cp(gb, 0x36); // INTERAC_MASTER_DIVERS_SON
  if (F & FZ) CALL_C_CC(b_+99, s_func_572c, SYM(func_572c), b_+102);
  else CYC(b_+99, b_+102);
  TAIL_SG(func_5723);
}
