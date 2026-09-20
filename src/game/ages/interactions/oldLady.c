#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode3d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode3d), (from), (to), true)

// oldLadyScriptTable: .dw mainScripts.oldLadySubid{0,1,2,3}Script (bank $0c pointers).
#define oldLadyScriptTable_bank08 SYM(oldLadyScriptTable)

// mainScripts.linkedGameNpcScript (bank $0c) and agesInteractionsBank09.getGameProgress_1
// (bank $09), referenced by address only.
#define linkedGameNpcScript_bank0c SYM(linkedGameNpcScript)
#define getGameProgress_1_bank09 SYM(getGameProgress_1)

static uint16_t oldLady_jumpTable(GB *gb) {
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

static void oldLady_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode3d@loadScript: pick the script from oldLadyScriptTable by subid. Reached
// from the init table, by `jr`/`jp` from @initSubid0, @initSubid2 and @runSubid3@substate1.
static void oldLady_loadScript(GB *gb) {
  BASE(interactionCode3d);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+73); SET_HL(oldLadyScriptTable_bank08);
  CYC(b_+73, b_+74); oldLady_addDoubleIndex(gb, b_+74);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+75, b_+76); H = mem_rd(gb, HL);
  CYC(b_+76, b_+77); L = A;
  CYC(b_+77, b_+80); interactionSetScript_hook(gb);
}

// interactionCode3d@state1: subid dispatch. Reached from the state table and by `jr` from
// @initSubid4/5 (while @initSubid's return address is still on the stack).
static void oldLady_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3d);
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); push_effect(gb, b_+131);
  do { uint16_t jt_ = (oldLady_jumpTable(gb));
    if (jt_ == b_+143) { goto runSubid0; }
    else if (jt_ == b_+156) { goto runSubid1; }
    else if (jt_ == b_+222) { goto runSubid2; }
    else if (jt_ == b_+228) { goto runSubid3; }
    else if (jt_ == b_+281) { goto runSubid4; }
    else { HANDOFF(HL); }
  } while (0);

runSubid0:
  // NPC with a grandson that is stone for part of the game
  CALL_C(b_+143, interactionRunScript_hook, SYM(interactionRunScript), b_+146);
  CYC(b_+146, b_+148); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+148, b_+149); A = mem_rd(gb, DE);
  CYC(b_+149, b_+150); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+150, b_+153); interactionAnimateAsNpc_hook(gb); return;
  }
  CYC(b_+150, b_+153);
  CYC(b_+153, b_+156); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid1:
  // Cutscene where her grandson gets turned to stone
  CYC(b_+156, b_+158); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+160); push_effect(gb, b_+160);
  do { uint16_t jt_ = (oldLady_jumpTable(gb));
    if (jt_ == b_+168) { goto subid1_substate0; }
    else if (jt_ == b_+192) { goto subid1_substate1; }
    else if (jt_ == b_+201) { goto subid1_substate2; }
    else if (jt_ == b_+212) { goto subid1_substate3; }
    else { HANDOFF(HL); }
  } while (0);

subid1_substate0:
  CALL_C(b_+168, interactionAnimate_hook, SYM(interactionAnimate), b_+171);
  CALL_C(b_+171, interactionRunScript_hook, SYM(interactionRunScript), b_+174);
  if (!(F & FC)) {
    CYCT(b_+174, b_+176); goto subid1_substate0_scriptRunning;
  }
  CYC(b_+174, b_+176);
  // Script ended
  CALL_C(b_+176, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+179);
  CYC(b_+179, b_+181); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+181, b_+183); mem_wr(gb, HL, 60);
  CYC(b_+183, b_+184); ret_effect(gb);
  return;
subid1_substate0_scriptRunning:
  CYC(b_+184, b_+186); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+186, b_+187); A = mem_rd(gb, DE);
  CYC(b_+187, b_+188); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+188, b_+191); interactionAnimate2Times_hook(gb); return;
  }
  CYC(b_+188, b_+191);
  CYC(b_+191, b_+192); ret_effect(gb);
  return;

subid1_substate1:
  CALL_C(b_+192, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+195);
  if (!(F & FZ)) {
    CYCT(b_+195, b_+196); ret_effect(gb); return;
  }
  CYC(b_+195, b_+196);
  CYC(b_+196, b_+198); mem_wr(gb, HL, 20);
  CYC(b_+198, b_+201); interactionIncSubstate_hook(gb);
  return;

subid1_substate2:
  CALL_C(b_+201, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+204);
  if (!(F & FZ)) {
    CYCT(b_+204, b_+207); interactionAnimate3Times_hook(gb); return;
  }
  CYC(b_+204, b_+207);
  CYC(b_+207, b_+209); mem_wr(gb, HL, 60);
  CYC(b_+209, b_+212); interactionIncSubstate_hook(gb);
  return;

subid1_substate3:
  CALL_C(b_+212, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+215);
  if (!(F & FZ)) {
    CYCT(b_+215, b_+216); ret_effect(gb); return;
  }
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+218); A = 0xff;
  CYC(b_+218, b_+221); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  CYC(b_+221, b_+222); ret_effect(gb);
  return;

runSubid2:
  // NPC in present, screen left from bipin&blossom's house
  CALL_C(b_+222, interactionRunScript_hook, SYM(interactionRunScript), b_+225);
  CYC(b_+225, b_+228); npcFaceLinkAndAnimate_hook(gb);
  return;

runSubid3:
  // Cutscene where her grandson is restored from stone
  CYC(b_+228, b_+230); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+230, b_+231); A = mem_rd(gb, DE);
  CYC(b_+231, b_+232); push_effect(gb, b_+232);
  do { uint16_t jt_ = (oldLady_jumpTable(gb));
    if (jt_ == b_+238) { goto subid3_substate0; }
    else if (jt_ == b_+248) { goto subid3_substate1; }
    else if (jt_ == b_+264) { goto subid3_substate2; }
    else { HANDOFF(HL); }
  } while (0);

subid3_substate0:
  CALL_C(b_+238, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+241);
  if (!(F & FZ)) {
    CYCT(b_+241, b_+242); ret_effect(gb); return;
  }
  CYC(b_+241, b_+242);
  CALL_C(b_+242, startJump_hook, SYM(startJump), b_+245);
  CYC(b_+245, b_+248); interactionIncSubstate_hook(gb);
  return;

subid3_substate1:
  CYC(b_+248, b_+250); C = 0x20;
  CALL_C(b_+250, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+253);
  if (!(F & FZ)) {
    CYCT(b_+253, b_+254); ret_effect(gb); return;
  }
  CYC(b_+253, b_+254);
  CALL_C(b_+254, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+257);
  CYC(b_+257, b_+259); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+259, b_+261); mem_wr(gb, HL, 0xb4);
  CYC(b_+261, b_+264); oldLady_loadScript(gb);
  return;

subid3_substate2:
  CYC(b_+264, b_+265); H = D;
  CYC(b_+265, b_+267); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+267, b_+268); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+268, b_+270);
  } else {
    CYC(b_+268, b_+270);
    CYC(b_+270, b_+272); A = 0xff;
    CYC(b_+272, b_+275); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  }
  CALL_C(b_+275, interactionRunScript_hook, SYM(interactionRunScript), b_+278);
  CYC(b_+278, b_+281); interactionAnimateBasedOnSpeed_hook(gb);
  return;

runSubid4:
  // Linked game NPC (subids 4 and 5)
  CALL_C(b_+281, interactionRunScript_hook, SYM(interactionRunScript), b_+284);
  if (F & FC) {
    CYCT(b_+284, b_+287); interactionDelete_hook(gb); return;
  }
  CYC(b_+284, b_+287);
  CYC(b_+287, SYM(oldLadyScriptTable)); npcFaceLinkAndAnimate_hook(gb);
}

// interactionCode3d@initSubid: per-subid initialization. Reached only by a static `call`
// from @state0.
static void oldLady_initSubid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode3d);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (oldLady_jumpTable(gb));
    if (jt_ == b_+44) { goto initSubid0; }
    else if (jt_ == b_+67) { oldLady_loadScript(gb); return; }
    else if (jt_ == b_+80) { goto initSubid2; }
    else if (jt_ == b_+97) { goto initSubid3; }
    else if (jt_ == b_+107) { goto initSubid4; }
    else if (jt_ == b_+111) { goto initSubid5; }
    else { HANDOFF(HL); }
  } while (0);

initSubid0:
  CYC(b_+44, b_+46); A = 0x03;
  CALL_C(b_+46, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+49);
  // Check whether her grandson is stone
  CYC(b_+49, b_+51); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(b_+51, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+54);
  if (F & FZ) {
    CYCT(b_+54, b_+56); oldLady_loadScript(gb); return;
  }
  CYC(b_+54, b_+56);
  // Set var03 to nonzero if her grandson is stone, also change her position
  CYC(b_+56, b_+58); A = 0x01;
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+64); SET_BC((SYM(interactionCode15__subid00) + 9));
  CALL_C(b_+64, interactionSetPosition_hook, SYM(interactionSetPosition), b_+67);
  oldLady_loadScript(gb);
  return;

initSubid2:
  // This NPC only exists between saving Nayru and beating d7?
  // callab agesInteractionsBank09.getGameProgress_1
  CYC(b_+80, b_+83); SET_HL(getGameProgress_1_bank09);
  CYC(b_+83, b_+85); E = 0x09;
  CALL_C(b_+85, interBankCall_hook, 0x008a, b_+88);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+92, b_+95); interactionDelete_hook(gb); return;
  }
  CYC(b_+92, b_+95);
  CYC(b_+95, b_+97); oldLady_loadScript(gb);
  return;

initSubid3:
  CYC(b_+97, b_+99); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+99, b_+101); A = 220;
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  CYC(b_+102, b_+104); A = 0x03;
  CYC(b_+104, b_+107); interactionSetAnimation_hook(gb);
  return;

initSubid4:
  CYC(b_+107, b_+109); A = 0x00;
  CYC(b_+109, b_+111); goto linkedGameNpc;

initSubid5:
  CYC(b_+111, b_+113); A = 0x09;

linkedGameNpc:
  CYC(b_+113, b_+115); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+115, b_+116); mem_wr(gb, DE, A);
  CYC(b_+116, b_+119); SET_HL(linkedGameNpcScript_bank0c);
  CALL_C(b_+119, interactionSetScript_hook, SYM(interactionSetScript), b_+122);
  CALL_C(b_+122, interactionRunScript_hook, SYM(interactionRunScript), b_+125);
  CYC(b_+125, b_+127); oldLady_state1(gb, sp0_);
}

// INTERAC_OLD_LADY
void interactionCode3d_hook(GB *gb) {
  BASE(interactionCode3d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (oldLady_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+127) { oldLady_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); oldLady_initSubid(gb, sp0_);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+24, b_+27); objectMarkSolidPosition_hook(gb); return;
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}
