#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCode9c_jump_table(GB *gb) {
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

// 0b:5462, called once from interactionCode9c@subid0State0.
void interactionCode9c_choosePresentKingZoraScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5462, 0x5464); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(0x5464, checkGlobalFlag_hook, 0x31f3, 0x5467);
  if (F & FZ) { CYCT(0x5467, 0x5469); goto pollutionNotFixed; } // jr z
  CYC(0x5467, 0x5469);
  CYC(0x5469, 0x546b); A = 0x40; // TREASURE_ESSENCE
  CALL_C(0x546b, checkTreasureObtained_hook, 0x1748, 0x546e);
  CYC(0x546e, 0x5470); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(0x5470, 0x5472); goto justCleanedWater; } // jr z
  CYC(0x5470, 0x5472);
  CYC(0x5472, 0x5474); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(0x5474, checkGlobalFlag_hook, 0x31f3, 0x5477);
  CYC(0x5477, 0x547a); SET_HL(0x7aff); // mainScripts.kingZoraScript_present_afterD7
  if (F & FZ) { CYCT(0x547a, 0x547b); ret_effect(gb); return; } // ret z
  CYC(0x547a, 0x547b);
  CYC(0x547b, 0x547d); A = 0x05; // TREASURE_SWORD
  CALL_C(0x547d, checkTreasureObtained_hook, 0x1748, 0x5480);
  CYC(0x5480, 0x5482); alu_and(gb, 0x01);
  CYC(0x5482, 0x5484); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5484, 0x5485); mem_wr(gb, DE, A);
  CYC(0x5485, 0x5488); SET_HL(0x7b05); // mainScripts.kingZoraScript_present_postGame
  RET(0x5488); return;

pollutionNotFixed:
  CYC(0x5489, 0x548b); A = 0x46; // TREASURE_LIBRARY_KEY
  CALL_C(0x548b, checkTreasureObtained_hook, 0x1748, 0x548e);
  CYC(0x548e, 0x5491); SET_HL(0x7aeb); // mainScripts.kingZoraScript_present_acceptedTask
  if (F & FC) { CYCT(0x5491, 0x5492); ret_effect(gb); return; } // ret c
  CYC(0x5491, 0x5492);
  CALL_C(0x5492, getThisRoomFlags_hook, 0x197d, 0x5495);
  CYC(0x5495, 0x5497); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(0x5497, 0x549a); SET_HL(0x7aca); // mainScripts.kingZoraScript_present_firstTime
  if (F & FZ) { CYCT(0x549a, 0x549b); ret_effect(gb); return; } // ret z
  CYC(0x549a, 0x549b);
  CYC(0x549b, 0x549e); SET_HL(0x7ad7); // mainScripts.kingZoraScript_present_giveKey
  RET(0x549e); return;

justCleanedWater:
  CYC(0x549f, 0x54a1); A = 0x31; // GLOBALFLAG_GOT_PERMISSION_TO_ENTER_JABU
  CALL_C(0x54a1, checkGlobalFlag_hook, 0x31f3, 0x54a4);
  CYC(0x54a4, 0x54a7); SET_HL(0x7af1); // mainScripts.kingZoraScript_present_justCleanedWater
  if (F & FZ) { CYCT(0x54a7, 0x54a8); ret_effect(gb); return; } // ret z
  CYC(0x54a7, 0x54a8);
  CYC(0x54a8, 0x54ab); SET_HL(0x7af9); // mainScripts.kingZoraScript_present_cleanedWater
  RET(0x54ab); return;
}

// 0b:54ac, called once from interactionCode9c@subid1.
void interactionCode9c_choosePastKingZoraScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54ac, 0x54ae); A = 0x27; // GLOBALFLAG_KING_ZORA_CURED
  CALL_C(0x54ae, checkGlobalFlag_hook, 0x31f3, 0x54b1);
  if (F & FZ) { CYCT(0x54b1, 0x54b3); goto notCured; } // jr z
  CYC(0x54b1, 0x54b3);
  CYC(0x54b3, 0x54b5); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(0x54b5, checkGlobalFlag_hook, 0x31f3, 0x54b8);
  CYC(0x54b8, 0x54bb); SET_HL(0x7b8b); // mainScripts.kingZoraScript_past_justCured
  if (F & FZ) { CYCT(0x54bb, 0x54bc); ret_effect(gb); return; } // ret z
  CYC(0x54bb, 0x54bc);
  CYC(0x54bc, 0x54be); A = 0x40; // TREASURE_ESSENCE
  CALL_C(0x54be, checkTreasureObtained_hook, 0x1748, 0x54c1);
  CYC(0x54c1, 0x54c3); alu_bit(gb, 6, A);
  CYC(0x54c3, 0x54c6); SET_HL(0x7b91); // mainScripts.kingZoraScript_past_cleanedWater
  if (F & FZ) { CYCT(0x54c6, 0x54c7); ret_effect(gb); return; } // ret z
  CYC(0x54c6, 0x54c7);
  CYC(0x54c7, 0x54ca); SET_HL(0x7b97); // mainScripts.kingZoraScript_past_afterD7
  RET(0x54ca); return;

notCured:
  CYC(0x54cb, 0x54cd); A = 0x2f; // TREASURE_POTION
  CALL_C(0x54cd, checkTreasureObtained_hook, 0x1748, 0x54d0);
  CYC(0x54d0, 0x54d3); SET_HL(0x7b59); // mainScripts.kingZoraScript_past_dontHavePotion
  if (!(F & FC)) { CYCT(0x54d3, 0x54d4); ret_effect(gb); return; } // ret nc
  CYC(0x54d3, 0x54d4);
  CYC(0x54d4, 0x54d7); SET_HL(0x7b5f); // mainScripts.kingZoraScript_past_havePotion
  RET(0x54d7); return;
}

// ==================================================================================================
// INTERAC_KING_ZORA
// ==================================================================================================
void interactionCode9c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5407, 0x5409); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5409, 0x540a); A = mem_rd(gb, DE);
  CYC(0x540a, 0x540c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x540c, 0x540d); push_effect(gb, 0x540d);
  switch (interactionCode9c_jump_table(gb)) {
    case 0x5413: goto subid0;
    case 0x5441: goto subid1;
    case 0x544a: goto subid2;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0:
  CYC(0x5413, 0x5414); A = mem_rd(gb, DE);
  CYC(0x5414, 0x5415); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5415, 0x5417); goto subid0State0; } // jr z
  CYC(0x5415, 0x5417);

state1:
  CALL_C(0x5417, interactionRunScript_hook, 0x2552, 0x541a);
  CYC(0x541a, 0x541d); interactionAnimate_hook(gb); return; // jp

subid0State0:
  CYC(0x541d, 0x541f); A = 0x27; // GLOBALFLAG_KING_ZORA_CURED
  CALL_C(0x541f, checkGlobalFlag_hook, 0x31f3, 0x5422);
  if (F & FZ) { CYCT(0x5422, 0x5425); interactionDelete_hook(gb); return; } // jp z
  CYC(0x5422, 0x5425);
  CALL_C(0x5425, interactionCode9c_choosePresentKingZoraScript_hook, 0x5462, 0x5428);

setScriptAndInit:
  CALL_C(0x5428, interactionSetScript_hook, 0x2544, 0x542b);
  CYC(0x542b, 0x542d); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x542d, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x5430);
  CALL_C(0x5430, interactionInitGraphics_hook, 0x15fb, 0x5433);
  CALL_C(0x5433, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5436);
  CALL_C(0x5436, interactionIncState_hook, 0x23e0, 0x5439);
  CYC(0x5439, 0x543b); A = 0x0a;
  CALL_C(0x543b, objectSetCollideRadius_hook, 0x24a1, 0x543e);
  CYC(0x543e, 0x5441); objectSetVisible82_hook(gb); return; // jp

subid1:
  CYC(0x5441, 0x5442); A = mem_rd(gb, DE);
  CYC(0x5442, 0x5443); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5443, 0x5445); goto state1; } // jr nz
  CYC(0x5443, 0x5445);
  CALL_C(0x5445, interactionCode9c_choosePastKingZoraScript_hook, 0x54ac, 0x5448);
  CYC(0x5448, 0x544a); goto setScriptAndInit; // jr

subid2:
  CYC(0x544a, 0x544b); A = mem_rd(gb, DE);
  CYC(0x544b, 0x544c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x544c, 0x544e); goto subid2State0; } // jr z
  CYC(0x544c, 0x544e);

// subid2State1
  CALL_C(0x544e, interactionDecCounter1_hook, 0x23cc, 0x5451);
  if (!(F & FZ)) { CYCT(0x5451, 0x5452); ret_effect(gb); return; } // ret nz
  CYC(0x5451, 0x5452);
  CYC(0x5452, 0x5455); interactionDelete_hook(gb); return; // jp

subid2State0:
  CALL_C(0x5455, interactionInitGraphics_hook, 0x15fb, 0x5458);
  CALL_C(0x5458, interactionIncState_hook, 0x23e0, 0x545b);
  CYC(0x545b, 0x545d); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x545d, 0x545f); mem_wr(gb, HL, 0x24);
  CYC(0x545f, 0x5462); objectSetVisible81_hook(gb); return; // jp
}
