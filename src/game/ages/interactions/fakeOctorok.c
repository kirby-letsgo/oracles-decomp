#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Each animation faces a different direction (indexed by var03).
#define interactionCode32_animations_bank08 SYM(interactionCode32__animations)
// b0 = counter1, b1 = angle, per var03.
#define impaOctorokCode_countersAndAngles_bank08 SYM(impaOctorokCode__countersAndAngles)
// Three (identical, do-nothing) script pointers indexed by var03.
#define impaOctorokScriptTable_bank08 SYM(impaOctorokScriptTable)
// mainScripts.greatFairyOctorokScript (bank $0c).
#define greatFairyOctorokScript_bank0c SYM(greatFairyOctorokScript)

static uint16_t fakeOctorok_jumpTable(GB *gb) {
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

static void fakeOctorok_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void fakeOctorok_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Subid 0: one of Impa's octoroks. Runs a (do-nothing) script and, once $cfd0 is set,
// walks off-screen and deletes itself.
void impaOctorokCode_hook(GB *gb) {
  BASE(impaOctorokCode);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (fakeOctorok_jumpTable(gb));
    if (jt_ == b_+15) { goto substate0; }
    else if (jt_ == b_+29) { goto substate1; }
    else if (jt_ == b_+70) { goto substate2; }
    else if (jt_ == b_+85) { goto substate3; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+15, b_+18); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+18, b_+20); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(b_+20, b_+21); ret_effect(gb); return;
  }
  CYC(b_+20, b_+21);
  CALL_C(b_+21, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+24);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x14);
  CYC(b_+28, b_+29); ret_effect(gb);
  return;

substate1:
  CALL_C(b_+29, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+32);
  if (!(F & FZ)) {
    CYCT(b_+32, b_+33); ret_effect(gb); return;
  }
  CYC(b_+32, b_+33);
  CALL_C(b_+33, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+36);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(b_+40, b_+42); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+46); SET_BC(impaOctorokCode_countersAndAngles_bank08);
  CALL_C(b_+46, addDoubleIndexToBc_hook, 0x007e, b_+49);
  CYC(b_+49, b_+50); A = mem_rd(gb, BC);
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+54); SET_BC(BC + 1);
  CYC(b_+54, b_+55); A = mem_rd(gb, BC);
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+60); A = alu_swap(gb, A);
  CYC(b_+60, b_+61); alu_rlca(gb);
  CYC(b_+61, b_+64); interactionSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(b_+70, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+73);
  CALL_C(b_+73, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+76);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+77); ret_effect(gb); return;
  }
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+79); A = 0x51; // SND_THROW
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);
  CYC(b_+82, b_+85); interactionIncSubstate_hook(gb);
  return;

substate3:
  CALL_C(b_+85, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+88);
  if (!(F & FC)) {
    CYCT(b_+88, b_+91); TAIL(interactionDelete);
  }
  CYC(b_+88, b_+91);
  CALL_C(b_+91, interactionAnimate2Times_hook, SYM(interactionAnimate2Times), b_+94);
  CYC(b_+94, b_+97); objectApplySpeed_hook(gb);
}

// Subids 1-2: the great fairy disguised as an octorok. Runs the script; once it ends
// (fairy powder used) it sinks into the ground and spawns the real great fairy.
void greatFairyOctorokCode_hook(GB *gb) {
  BASE(greatFairyOctorokCode);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, npcFaceLinkAndAnimate_hook, SYM(npcFaceLinkAndAnimate), b_+3);
  CALL_C(b_+3, interactionRunScript_hook, SYM(interactionRunScript), b_+6);
  if (!(F & FC)) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CALL_C(b_+8, objectUpdateSpeedZ_hook, SYM(objectUpdateSpeedZ), b_+11);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0xf0);
  if (!(F & FZ)) {
    CYCT(b_+16, b_+17); ret_effect(gb); return;
  }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); SET_BC(w1ParentItem5_id); // INTERAC_GREAT_FAIRY, subid 1
  CALL_C(b_+20, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+23);
  CYC(b_+23, b_+25); A = 0x51; // TREASURE_FAIRY_POWDER
  CALL_C(b_+25, loseTreasure_hook, SYM(loseTreasure), b_+28);
  CYC(b_+28, b_+31); interactionDelete_hook(gb);
}

// INTERAC_FAKE_OCTOROK: Impa's octoroks (subid 0) and the great fairy octorok (subids 1-2).
void interactionCode32_hook(GB *gb) {
  BASE(interactionCode32);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (fakeOctorok_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+103) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (fakeOctorok_jumpTable(gb));
    if (jt_ == b_+24) { goto init0; }
    else if (jt_ == b_+97) { goto init1; }
    else if (jt_ == b_+61) { goto init2; }
    else { HANDOFF(HL); }
  } while (0);

init0:
  CALL_C(b_+24, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+27);
  CYC(b_+27, b_+29); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(b_+29, b_+32); TAIL(interactionDelete);
  }
  CYC(b_+29, b_+32);
  CALL_C(b_+32, objectSetVisible82_hook, SYM(objectSetVisible82), b_+35);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); B = A;
  CYC(b_+39, b_+42); SET_HL(impaOctorokScriptTable_bank08);
  CYC(b_+42, b_+43); fakeOctorok_addDoubleIndex(gb, b_+43);
  CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+44, b_+45); H = mem_rd(gb, HL);
  CYC(b_+45, b_+46); L = A;
  CALL_C(b_+46, interactionSetScript_hook, SYM(interactionSetScript), b_+49);
  CYC(b_+49, b_+50); A = B;
  CYC(b_+50, b_+53); SET_HL(interactionCode32_animations_bank08);
  CYC(b_+53, b_+54); push_effect(gb, b_+54); fakeOctorok_addAToHl(gb);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  CYC(b_+55, b_+58); interactionSetAnimation_hook(gb);
  return;

init2:
  CYC(b_+61, b_+63); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(b_+63, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+66);
  if (F & FZ) {
    CYCT(b_+66, b_+68); goto notFixed;
  }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); A = 0x38; // ENEMY_GREAT_FAIRY
  CALL_C(b_+70, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+73);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x38);
  CALL_C(b_+75, objectCopyPosition_hook, SYM(objectCopyPosition), b_+78);
  CYC(b_+78, b_+81); interactionDelete_hook(gb);
  return;
notFixed:
  CYC(b_+81, b_+84); SET_BC(hOamFunc);
  CALL_C(b_+84, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+87);
  CYC(b_+87, b_+89); A = 0x41; // >TX_4100
  CALL_C(b_+89, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+92);
  CYC(b_+92, b_+95); SET_HL(greatFairyOctorokScript_bank0c);
  CYC(b_+95, b_+97);

init1:
  CALL_C(b_+97, interactionSetScript_hook, SYM(interactionSetScript), b_+100);
  CALL_C(b_+100, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+103);

state1:
  CYC(b_+103, b_+105); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+107); push_effect(gb, b_+107);
  do { uint16_t jt_ = (fakeOctorok_jumpTable(gb));
    if (jt_ == SYM(impaOctorokCode) && hook_enabled_at(gb, SYM(impaOctorokCode))) { impaOctorokCode_hook(gb); return; }
    else if (jt_ == SYM(greatFairyOctorokCode) && hook_enabled_at(gb, SYM(greatFairyOctorokCode))) { greatFairyOctorokCode_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}
