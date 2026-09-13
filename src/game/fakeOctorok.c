#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// Each animation faces a different direction (indexed by var03).
#define interactionCode32_animations_bank08 0x6047
// b0 = counter1, b1 = angle, per var03.
#define impaOctorokCode_countersAndAngles_bank08 0x60be
// Three (identical, do-nothing) script pointers indexed by var03.
#define impaOctorokScriptTable_bank08 0x60df
// mainScripts.greatFairyOctorokScript (bank $0c).
#define greatFairyOctorokScript_bank0c 0x5308

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x607e, interactionAnimate_hook, 0x261b, 0x6081);
  CYC(0x6081, 0x6083); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6083, 0x6084); A = mem_rd(gb, DE);
  CYC(0x6084, 0x6085); push_effect(gb, 0x6085);
  switch (fakeOctorok_jumpTable(gb)) {
    case 0x608d: goto substate0;
    case 0x609b: goto substate1;
    case 0x60c4: goto substate2;
    case 0x60d3: goto substate3;
    default: HANDOFF(HL);
  }

substate0:
  CYC(0x608d, 0x6090); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x6090, 0x6092); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x6092, 0x6093); ret_effect(gb); return;
  }
  CYC(0x6092, 0x6093);
  CALL_C(0x6093, interactionIncSubstate_hook, 0x23e5, 0x6096);
  CYC(0x6096, 0x6098); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6098, 0x609a); mem_wr(gb, HL, 0x14);
  CYC(0x609a, 0x609b); ret_effect(gb);
  return;

substate1:
  CALL_C(0x609b, interactionDecCounter1_hook, 0x23cc, 0x609e);
  if (!(F & FZ)) {
    CYCT(0x609e, 0x609f); ret_effect(gb); return;
  }
  CYC(0x609e, 0x609f);
  CALL_C(0x609f, interactionIncSubstate_hook, 0x23e5, 0x60a2);
  CYC(0x60a2, 0x60a4); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x60a4, 0x60a6); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(0x60a6, 0x60a8); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x60a8, 0x60a9); A = mem_rd(gb, HL);
  CYC(0x60a9, 0x60ac); SET_BC(impaOctorokCode_countersAndAngles_bank08);
  CALL_C(0x60ac, addDoubleIndexToBc_hook, 0x007e, 0x60af);
  CYC(0x60af, 0x60b0); A = mem_rd(gb, BC);
  CYC(0x60b0, 0x60b2); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x60b2, 0x60b3); mem_wr(gb, HL, A);
  CYC(0x60b3, 0x60b4); SET_BC(BC + 1);
  CYC(0x60b4, 0x60b5); A = mem_rd(gb, BC);
  CYC(0x60b5, 0x60b7); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x60b7, 0x60b8); mem_wr(gb, HL, A);
  CYC(0x60b8, 0x60ba); A = alu_swap(gb, A);
  CYC(0x60ba, 0x60bb); alu_rlca(gb);
  CYC(0x60bb, 0x60be); interactionSetAnimation_hook(gb);
  return;

substate2:
  CALL_C(0x60c4, interactionAnimate2Times_hook, 0x2752, 0x60c7);
  CALL_C(0x60c7, interactionDecCounter1_hook, 0x23cc, 0x60ca);
  if (!(F & FZ)) {
    CYCT(0x60ca, 0x60cb); ret_effect(gb); return;
  }
  CYC(0x60ca, 0x60cb);
  CYC(0x60cb, 0x60cd); A = 0x51; // SND_THROW
  CALL_C(0x60cd, playSound_b00_hook, 0x0c98, 0x60d0);
  CYC(0x60d0, 0x60d3); interactionIncSubstate_hook(gb);
  return;

substate3:
  CALL_C(0x60d3, objectCheckWithinScreenBoundary_hook, 0x2184, 0x60d6);
  if (!(F & FC)) {
    CYCT(0x60d6, 0x60d9); interactionDelete_hook(gb); return;
  }
  CYC(0x60d6, 0x60d9);
  CALL_C(0x60d9, interactionAnimate2Times_hook, 0x2752, 0x60dc);
  CYC(0x60dc, 0x60df); objectApplySpeed_hook(gb);
}

// Subids 1-2: the great fairy disguised as an octorok. Runs the script; once it ends
// (fairy powder used) it sinks into the ground and spawns the real great fairy.
void greatFairyOctorokCode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60e5, npcFaceLinkAndAnimate_hook, 0x26a9, 0x60e8);
  CALL_C(0x60e8, interactionRunScript_hook, 0x2552, 0x60eb);
  if (!(F & FC)) {
    CYCT(0x60eb, 0x60ec); ret_effect(gb); return;
  }
  CYC(0x60eb, 0x60ec);
  CYC(0x60ec, 0x60ed); alu_xor(gb, A);
  CALL_C(0x60ed, objectUpdateSpeedZ_hook, 0x1f45, 0x60f0);
  CYC(0x60f0, 0x60f2); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x60f2, 0x60f3); A = mem_rd(gb, DE);
  CYC(0x60f3, 0x60f5); alu_cp(gb, 0xf0);
  if (!(F & FZ)) {
    CYCT(0x60f5, 0x60f6); ret_effect(gb); return;
  }
  CYC(0x60f5, 0x60f6);
  CYC(0x60f6, 0x60f9); SET_BC(0xd501); // INTERAC_GREAT_FAIRY, subid 1
  CALL_C(0x60f9, objectCreateInteraction_hook, 0x24c5, 0x60fc);
  CYC(0x60fc, 0x60fe); A = 0x51; // TREASURE_FAIRY_POWDER
  CALL_C(0x60fe, loseTreasure_hook, 0x1733, 0x6101);
  CYC(0x6101, 0x6104); interactionDelete_hook(gb);
}

// INTERAC_FAKE_OCTOROK: Impa's octoroks (subid 0) and the great fairy octorok (subids 1-2).
void interactionCode32_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x600d, 0x600f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x600f, 0x6010); A = mem_rd(gb, DE);
  CYC(0x6010, 0x6011); push_effect(gb, 0x6011);
  switch (fakeOctorok_jumpTable(gb)) {
    case 0x6015: goto state0;
    case 0x6074: goto state1;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x6015, 0x6017); A = 0x01;
  CYC(0x6017, 0x6018); mem_wr(gb, DE, A);
  CALL_C(0x6018, interactionInitGraphics_hook, 0x15fb, 0x601b);
  CYC(0x601b, 0x601d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x601d, 0x601e); A = mem_rd(gb, DE);
  CYC(0x601e, 0x601f); push_effect(gb, 0x601f);
  switch (fakeOctorok_jumpTable(gb)) {
    case 0x6025: goto init0;
    case 0x606e: goto init1;
    case 0x604a: goto init2;
    default: HANDOFF(HL);
  }

init0:
  CALL_C(0x6025, getThisRoomFlags_hook, 0x197d, 0x6028);
  CYC(0x6028, 0x602a); alu_bit(gb, 6, A);
  if (!(F & FZ)) {
    CYCT(0x602a, 0x602d); interactionDelete_hook(gb); return;
  }
  CYC(0x602a, 0x602d);
  CALL_C(0x602d, objectSetVisible82_hook, 0x1e69, 0x6030);
  CYC(0x6030, 0x6032); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6032, 0x6033); A = mem_rd(gb, DE);
  CYC(0x6033, 0x6034); B = A;
  CYC(0x6034, 0x6037); SET_HL(impaOctorokScriptTable_bank08);
  CYC(0x6037, 0x6038); fakeOctorok_addDoubleIndex(gb, 0x6038);
  CYC(0x6038, 0x6039); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6039, 0x603a); H = mem_rd(gb, HL);
  CYC(0x603a, 0x603b); L = A;
  CALL_C(0x603b, interactionSetScript_hook, 0x2544, 0x603e);
  CYC(0x603e, 0x603f); A = B;
  CYC(0x603f, 0x6042); SET_HL(interactionCode32_animations_bank08);
  CYC(0x6042, 0x6043); push_effect(gb, 0x6043); fakeOctorok_addAToHl(gb);
  CYC(0x6043, 0x6044); A = mem_rd(gb, HL);
  CYC(0x6044, 0x6047); interactionSetAnimation_hook(gb);
  return;

init2:
  CYC(0x604a, 0x604c); A = 0x30; // GLOBALFLAG_WATER_POLLUTION_FIXED
  CALL_C(0x604c, checkGlobalFlag_hook, 0x31f3, 0x604f);
  if (F & FZ) {
    CYCT(0x604f, 0x6051); goto notFixed;
  }
  CYC(0x604f, 0x6051);
  CYC(0x6051, 0x6053); A = 0x38; // ENEMY_GREAT_FAIRY
  CALL_C(0x6053, getFreeEnemySlot_hook, 0x2e27, 0x6056);
  CYC(0x6056, 0x6058); mem_wr(gb, HL, 0x38);
  CALL_C(0x6058, objectCopyPosition_hook, 0x2242, 0x605b);
  CYC(0x605b, 0x605e); interactionDelete_hook(gb);
  return;
notFixed:
  CYC(0x605e, 0x6061); SET_BC(0xff80);
  CALL_C(0x6061, objectSetSpeedZ_hook, 0x239d, 0x6064);
  CYC(0x6064, 0x6066); A = 0x41; // >TX_4100
  CALL_C(0x6066, interactionSetHighTextIndex_hook, 0x253b, 0x6069);
  CYC(0x6069, 0x606c); SET_HL(greatFairyOctorokScript_bank0c);
  CYC(0x606c, 0x606e);

init1:
  CALL_C(0x606e, interactionSetScript_hook, 0x2544, 0x6071);
  CALL_C(0x6071, objectSetVisiblec0_hook, 0x1e33, 0x6074);

state1:
  CYC(0x6074, 0x6076); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6076, 0x6077); A = mem_rd(gb, DE);
  CYC(0x6077, 0x6078); push_effect(gb, 0x6078);
  switch (fakeOctorok_jumpTable(gb)) {
    case 0x607e: impaOctorokCode_hook(gb); return;
    case 0x60e5: greatFairyOctorokCode_hook(gb); return;
    default: HANDOFF(HL);
  }
}
