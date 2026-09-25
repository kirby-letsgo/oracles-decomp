#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/sunkenCityNpcs.s.
// INTERAC_FLOODED_HOUSE_GIRL
// INTERAC_MASTER_DIVERS_WIFE
// INTERAC_MASTER_DIVER

static void sunkenCityNpcs_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_FLOODED_HOUSE_GIRL
// INTERAC_MASTER_DIVERS_WIFE
// INTERAC_MASTER_DIVER
void s_interactionCode8a_hook(GB *gb) {
  BASE(interactionCode8a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x8d);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_4bf9; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x40;
  CALL_C(b_+17, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+20);
  if (!(F & FC)) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CALL_C(b_+23, s_getHighestSetBit, SYM(getHighestSetBit), b_+26);
  CYC(b_+26, b_+28); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+28, b_+31); TAIL(interactionDelete); }
  CYC(b_+28, b_+31);
L_4bf9:
  CALL_C(b_+31, s_getSunkenCityNPCVisibleSubId_caller_hook, SYM(getSunkenCityNPCVisibleSubId_caller), b_+34);
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+38, b_+41); TAIL(interactionDelete); }
  CYC(b_+38, b_+41);
  CYC(b_+41, b_+43); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto npcShouldAppear; }
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+50); alu_cp(gb, 0x8b);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto npcShouldAppear; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); A = 0x16;
  CALL_C(b_+54, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+57);
  CYC(b_+57, b_+59); B = 0x5d;
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto wifeShouldAppear; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); B = 0xb6;
wifeShouldAppear:
  CYC(b_+63, b_+66); A = mem_rd(gb, wActiveRoom);
  CYC(b_+66, b_+67); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+67, b_+70); TAIL(interactionDelete); }
  CYC(b_+67, b_+70);
npcShouldAppear:
  CALL_C(b_+70, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+73);
  CYC(b_+73, b_+75); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+75, b_+77); A = 0x04;
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+84); SET_HL(b_+114 /* @floodedHouseGirlScripts */);
  CYC(b_+84, b_+86); alu_cp(gb, 0x8a);
  if (F & FZ) { CYCT(b_+86, b_+88); goto setScript; }
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+91); SET_HL(b_+124 /* @masterDiversWifeScripts */);
  CYC(b_+91, b_+93); alu_cp(gb, 0x8b);
  if (F & FZ) { CYCT(b_+93, b_+95); goto setScript; }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+98); SET_HL(b_+134 /* @masterDiverScripts */);
setScript:
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+102); sunkenCityNpcs_add_double_index(gb, b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+103, b_+104); H = mem_rd(gb, HL);
  CYC(b_+104, b_+105); L = A;
  CALL_C(b_+105, s_interactionSetScript, SYM(interactionSetScript), b_+108);
state1:
  CALL_C(b_+108, s_interactionRunScript, SYM(interactionRunScript), b_+111);
  CYC(b_+111, b_+114);
  TAIL(interactionAnimateAsNpc);
}

