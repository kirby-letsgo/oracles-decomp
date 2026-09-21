#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode9c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+91, b_+93); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(b_+93, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+96);
  if (F & FZ) { CYCT(b_+96, b_+98); goto pollutionNotFixed; } // jr z
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+100); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+100, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+103);
  CYC(b_+103, b_+105); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+105, b_+107); goto justCleanedWater; } // jr z
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); A = 0x14; // GLOBALFLAG_FINISHEDGAME
  CALL_C(b_+109, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+112);
  CYC(b_+112, b_+115); SET_HL((SYM(interactiond7_makuSeed__state3Substate9) + 1)); // mainScripts.kingZoraScript_present_afterD7
  if (F & FZ) { CYCT(b_+115, b_+116); ret_effect(gb); return; } // ret z
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+118); A = 0x05; // TREASURE_SWORD
  CALL_C(b_+118, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+121);
  CYC(b_+121, b_+123); alu_and(gb, 0x01);
  CYC(b_+123, b_+125); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+129); SET_HL((SYM(interactiond7_makuSeed__state3Substate9) + 7)); // mainScripts.kingZoraScript_present_postGame
  RET(b_+129); return;

pollutionNotFixed:
  CYC(b_+130, b_+132); A = 0x46; // TREASURE_LIBRARY_KEY
  CALL_C(b_+132, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+135);
  CYC(b_+135, b_+138); SET_HL((SYM(interactiond7_makuSeed__essenceRotationCommon) + 7)); // mainScripts.kingZoraScript_present_acceptedTask
  if (F & FC) { CYCT(b_+138, b_+139); ret_effect(gb); return; } // ret c
  CYC(b_+138, b_+139);
  CALL_C(b_+139, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+142);
  CYC(b_+142, b_+144); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(b_+144, b_+147); SET_HL((SYM(interactiond7_makuSeed__state3Substate2) + 17)); // mainScripts.kingZoraScript_present_firstTime
  if (F & FZ) { CYCT(b_+147, b_+148); ret_effect(gb); return; } // ret z
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+151); SET_HL((SYM(interactiond7_makuSeed__state3Substate3) + 11)); // mainScripts.kingZoraScript_present_giveKey
  RET(b_+151); return;

justCleanedWater:
  CYC(b_+152, b_+154); A = 0x31; // GLOBALFLAG_GOT_PERMISSION_TO_ENTER_JABU
  CALL_C(b_+154, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+157);
  CYC(b_+157, b_+160); SET_HL((SYM(interactiond7_makuSeed__state3Substate8) + 4)); // mainScripts.kingZoraScript_present_justCleanedWater
  if (F & FZ) { CYCT(b_+160, b_+161); ret_effect(gb); return; } // ret z
  CYC(b_+160, b_+161);
  CYC(b_+161, b_+164); SET_HL((SYM(interactiond7_makuSeed__state3Substate8) + 12)); // mainScripts.kingZoraScript_present_cleanedWater
  RET(b_+164); return;
}

// 0b:54ac, called once from interactionCode9c@subid1.
void interactionCode9c_choosePastKingZoraScript_hook(GB *gb) {
  BASE(interactionCode9c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+165, b_+167); A = 0x27; // GLOBALFLAG_KING_ZORA_CURED
  CALL_C(b_+167, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+170);
  if (F & FZ) { CYCT(b_+170, b_+172); goto notCured; } // jr z
  CYC(b_+170, b_+172);
  CYC(b_+172, b_+174); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(b_+174, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+177);
  CYC(b_+177, b_+180); SET_HL((SYM(interactiond7_makuSeed__state4Substate3) + 21)); // mainScripts.kingZoraScript_past_justCured
  if (F & FZ) { CYCT(b_+180, b_+181); ret_effect(gb); return; } // ret z
  CYC(b_+180, b_+181);
  CYC(b_+181, b_+183); A = 0x40; // TREASURE_ESSENCE
  CALL_C(b_+183, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+186);
  CYC(b_+186, b_+188); alu_bit(gb, 6, A);
  CYC(b_+188, b_+191); SET_HL((SYM(interactiond7_makuSeed__state4Substate3__unlinkedGame) + 2)); // mainScripts.kingZoraScript_past_cleanedWater
  if (F & FZ) { CYCT(b_+191, b_+192); ret_effect(gb); return; } // ret z
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+195); SET_HL((SYM(interactiond7_makuSeed__state4Substate3__unlinkedGame) + 8)); // mainScripts.kingZoraScript_past_afterD7
  RET(b_+195); return;

notCured:
  CYC(b_+196, b_+198); A = 0x2f; // TREASURE_POTION
  CALL_C(b_+198, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+201);
  CYC(b_+201, b_+204); SET_HL((SYM(interactiond7_makuSeed__tileReplacements) + 35)); // mainScripts.kingZoraScript_past_dontHavePotion
  if (!(F & FC)) { CYCT(b_+204, b_+205); ret_effect(gb); return; } // ret nc
  CYC(b_+204, b_+205);
  CYC(b_+205, b_+208); SET_HL((SYM(interactiond7_makuSeed__state4Substate1) + 4)); // mainScripts.kingZoraScript_past_havePotion
  RET(b_+208); return;
}

// ==================================================================================================
// INTERAC_KING_ZORA
// ==================================================================================================
void interactionCode9c_hook(GB *gb) {
  BASE(interactionCode9c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (interactionCode9c_jump_table(gb));
    if (jt_ == b_+12) { goto subid0; }
    else if (jt_ == b_+58) { goto subid1; }
    else if (jt_ == b_+67) { goto subid2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0:
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto subid0State0; } // jr z
  CYC(b_+14, b_+16);

state1:
  CALL_C(b_+16, interactionRunScript_hook, SYM(interactionRunScript), b_+19);
  CYC(b_+19, b_+22); TAIL(interactionAnimate); // jp

subid0State0:
  CYC(b_+22, b_+24); A = 0x27; // GLOBALFLAG_KING_ZORA_CURED
  CALL_C(b_+24, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+27);
  if (F & FZ) { CYCT(b_+27, b_+30); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+27, b_+30);
  CALL_C(b_+30, interactionCode9c_choosePresentKingZoraScript_hook, b_+91, b_+33);

setScriptAndInit:
  CALL_C(b_+33, interactionSetScript_hook, SYM(interactionSetScript), b_+36);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+38, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+41);
  CALL_C(b_+41, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+44);
  CALL_C(b_+44, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+47);
  CALL_C(b_+47, interactionIncState_hook, SYM(interactionIncState), b_+50);
  CYC(b_+50, b_+52); A = 0x0a;
  CALL_C(b_+52, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+55);
  CYC(b_+55, b_+58); TAIL(objectSetVisible82); // jp

subid1:
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto state1; } // jr nz
  CYC(b_+60, b_+62);
  CALL_C(b_+62, interactionCode9c_choosePastKingZoraScript_hook, b_+165, b_+65);
  CYC(b_+65, b_+67); goto setScriptAndInit; // jr

subid2:
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+69, b_+71); goto subid2State0; } // jr z
  CYC(b_+69, b_+71);

// subid2State1
  CALL_C(b_+71, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+74);
  if (!(F & FZ)) { CYCT(b_+74, b_+75); ret_effect(gb); return; } // ret nz
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+78); TAIL(interactionDelete); // jp

subid2State0:
  CALL_C(b_+78, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+81);
  CALL_C(b_+81, interactionIncState_hook, SYM(interactionIncState), b_+84);
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x24);
  CYC(b_+88, b_+91); TAIL(objectSetVisible81); // jp
}
