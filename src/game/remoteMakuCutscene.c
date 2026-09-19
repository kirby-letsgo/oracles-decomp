#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode8a_jump_table(GB *gb) {
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

static void interactionCode8a_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode8a@checkEssenceObtained: a shared local reached by `call` from eight sibling
// branches inside @checkConditionsAndSetText, itself already one manual call deep at every one
// of those call sites (state0 -> checkConditionsAndSetText is still on the stack). It never
// executes its own ret -- it tail-jumps straight into the already-hooked checkFlag -- so no
// pc/sp resume check is needed here: push_effect below models the real `call` push, and
// checkFlag_hook's own ret pops that exact value back off regardless of what else sits deeper
// on the stack, letting each caller continue in a straight line right after this call returns.
static void interactionCode8a_checkEssenceObtained(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x6a45, 0x6a48); SET_HL(wEssencesObtained);
  CYC(0x6a48, 0x6a4b); checkFlag_hook(gb); // jp
}

// INTERAC_REMOTE_MAKU_CUTSCENE
void interactionCode8a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x694f, 0x6951); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6951, 0x6952); A = mem_rd(gb, DE);
  {
    CYC(0x6952, 0x6953); push_effect(gb, 0x6953);
    uint16_t target = interactionCode8a_jump_table(gb);
    if (target == 0x6957) goto subid0;
    HANDOFF(target);
  }

subid0:
  CALL_C(0x6957, checkInteractionState_hook, 0x23fe, 0x695a);
  if (!(F & FZ)) { CYCT(0x695a, 0x695c); goto state1; } // jr nz
  CYC(0x695a, 0x695c);

  // interactionCode8a@state0
  CALL_C(0x695c, returnIfScrollMode01Unset_hook, 0x26e4, 0x695f);
  CYC(0x695f, 0x6961); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6961, 0x6962); A = mem_rd(gb, DE);
  CYC(0x6962, 0x6964); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x6964, 0x6965); mem_wr(gb, DE, A);
  CYC(0x6965, 0x6968); push_effect(gb, 0x6968); goto checkConditionsAndSetText;
afterCheckConditionsAndSetText:
  CALL_C(0x6968, getThisRoomFlags_hook, 0x197d, 0x696b);
  CYC(0x696b, 0x696d); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x696d, 0x6970); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x696d, 0x6970);

  CYC(0x6970, 0x6973); push_effect(gb, 0x6973); goto loadScript;

state1:
  CALL_C(0x6973, interactionRunScript_hook, 0x2552, 0x6976);
  if (F & FC) { CYCT(0x6976, 0x6979); interactionDelete_hook(gb); return; } // jp c
  CYC(0x6976, 0x6979);
  RET(0x6979); return; // ret

// interactionCode8a@checkConditionsAndSetText: single caller, state0's own return address
// 0x6968, pushed above while sp == sp0_, so its ret's resume check compares against bare
// sp0_. Its jump table dispatches on var03 to twelve sibling val## branches that each either
// jp into @deleteSelfAndReturn -- which pops this call's own pushed return address, discarding
// it, and tail-jumps into interactionDelete, ending the whole hook -- or jp into
// @setTextForScript, whose ret resumes state0 via that same pushed 0x6968.
checkConditionsAndSetText:
  CYC(0x697a, 0x697c); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x697c, 0x697d); A = mem_rd(gb, DE);
  {
    CYC(0x697d, 0x697e); push_effect(gb, 0x697e);
    uint16_t target = interactionCode8a_jump_table(gb);
    if (target == 0x6996) goto val00;
    if (target == 0x69a3) goto val01;
    if (target == 0x69a9) goto val02;
    if (target == 0x69b7) goto val03;
    if (target == 0x69c5) goto val04;
    if (target == 0x69e0) goto val05;
    if (target == 0x69ee) goto val06;
    if (target == 0x69fc) goto val07;
    if (target == 0x6a0a) goto val08;
    if (target == 0x6a18) goto val09;
    if (target == 0x6a26) goto val0a;
    if (target == 0x6a34) goto val0b;
    HANDOFF(target);
  }

val00:
  CYC(0x6996, 0x6997); alu_xor(gb, A); // xor a
  CYC(0x6997, 0x699a); interactionCode8a_checkEssenceObtained(gb, 0x699a);
  if (F & FZ) { CYCT(0x699a, 0x699d); goto deleteSelfAndReturn; } // jp z
  CYC(0x699a, 0x699d);
  CYC(0x699d, 0x69a0); SET_BC(0x00b0); // ldbc $00, <TX_05b0
  CYC(0x69a0, 0x69a3); goto setTextForScript; // jp

val01:
  CYC(0x69a3, 0x69a6); SET_BC(0x00b1); // ldbc $00, <TX_05b1
  CYC(0x69a6, 0x69a9); goto setTextForScript; // jp

val02:
  CYC(0x69a9, 0x69ab); A = 0x11; // TREASURE_HARP
  CALL_C(0x69ab, checkTreasureObtained_hook, 0x1748, 0x69ae);
  if (!(F & FC)) { CYCT(0x69ae, 0x69b1); goto deleteSelfAndReturn; } // jp nc
  CYC(0x69ae, 0x69b1);
  CYC(0x69b1, 0x69b4); SET_BC(0x00b2); // ldbc $00, <TX_05b2
  CYC(0x69b4, 0x69b7); goto setTextForScript; // jp

val03:
  CYC(0x69b7, 0x69b9); A = 0x01;
  CYC(0x69b9, 0x69bc); interactionCode8a_checkEssenceObtained(gb, 0x69bc);
  if (F & FZ) { CYCT(0x69bc, 0x69bf); goto deleteSelfAndReturn; } // jp z
  CYC(0x69bc, 0x69bf);
  CYC(0x69bf, 0x69c2); SET_BC(0x00b3); // ldbc $00, <TX_05b3
  CYC(0x69c2, 0x69c5); goto setTextForScript; // jp

val04:
  CYC(0x69c5, 0x69c7); A = 0x02;
  CYC(0x69c7, 0x69ca); interactionCode8a_checkEssenceObtained(gb, 0x69ca);
  if (F & FZ) { CYCT(0x69ca, 0x69cd); goto deleteSelfAndReturn; } // jp z
  CYC(0x69ca, 0x69cd);
  CYC(0x69cd, 0x69d0); SET_HL(wGroup1RoomFlags + 0x76); // wPastRoomFlags+$76
  CYC(0x69d0, 0x69d2); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 0)); // set 0,(hl)
  CALL_C(0x69d2, checkIsLinkedGame_hook, 0x1992, 0x69d5);
  CYC(0x69d5, 0x69d7); A = 0x1d; // GLOBALFLAG_CAN_BUY_FLUTE
  if (F & FZ) CALL_C_CC(0x69d7, setGlobalFlag_hook, 0x31f9, 0x69da); else CYC(0x69d7, 0x69da); // call z
  CYC(0x69da, 0x69dd); SET_BC(0x00b4); // ldbc $00, <TX_05b4
  CYC(0x69dd, 0x69e0); goto setTextForScript; // jp

val05:
  CYC(0x69e0, 0x69e2); A = 0x03;
  CYC(0x69e2, 0x69e5); interactionCode8a_checkEssenceObtained(gb, 0x69e5);
  if (F & FZ) { CYCT(0x69e5, 0x69e8); goto deleteSelfAndReturn; } // jp z
  CYC(0x69e5, 0x69e8);
  CYC(0x69e8, 0x69eb); SET_BC(0x00b5); // ldbc $00, <TX_05b5
  CYC(0x69eb, 0x69ee); goto setTextForScript; // jp

val06:
  CYC(0x69ee, 0x69f0); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(0x69f0, checkGlobalFlag_hook, 0x31f3, 0x69f3);
  if (F & FZ) { CYCT(0x69f3, 0x69f6); goto deleteSelfAndReturn; } // jp z
  CYC(0x69f3, 0x69f6);
  CYC(0x69f6, 0x69f9); SET_BC(0x00b6); // ldbc $00, <TX_05b6
  CYC(0x69f9, 0x69fc); goto setTextForScript; // jp

val07:
  CYC(0x69fc, 0x69fe); A = 0x04;
  CYC(0x69fe, 0x6a01); interactionCode8a_checkEssenceObtained(gb, 0x6a01);
  if (F & FZ) { CYCT(0x6a01, 0x6a04); goto deleteSelfAndReturn; } // jp z
  CYC(0x6a01, 0x6a04);
  CYC(0x6a04, 0x6a07); SET_BC(0x00b7); // ldbc $00, <TX_05b7
  CYC(0x6a07, 0x6a0a); goto setTextForScript; // jp

val08:
  CYC(0x6a0a, 0x6a0c); A = 0x05;
  CYC(0x6a0c, 0x6a0f); interactionCode8a_checkEssenceObtained(gb, 0x6a0f);
  if (F & FZ) { CYCT(0x6a0f, 0x6a12); goto deleteSelfAndReturn; } // jp z
  CYC(0x6a0f, 0x6a12);
  CYC(0x6a12, 0x6a15); SET_BC(0x00b8); // ldbc $00, <TX_05b8
  CYC(0x6a15, 0x6a18); goto setTextForScript; // jp

val09:
  CYC(0x6a18, 0x6a1a); A = 0x06;
  CYC(0x6a1a, 0x6a1d); interactionCode8a_checkEssenceObtained(gb, 0x6a1d);
  if (F & FZ) { CYCT(0x6a1d, 0x6a20); goto deleteSelfAndReturn; } // jp z
  CYC(0x6a1d, 0x6a20);
  CYC(0x6a20, 0x6a23); SET_BC(0x00b9); // ldbc $00, <TX_05b9
  CYC(0x6a23, 0x6a26); goto setTextForScript; // jp

val0a:
  CYC(0x6a26, 0x6a28); A = 0x07;
  CYC(0x6a28, 0x6a2b); interactionCode8a_checkEssenceObtained(gb, 0x6a2b);
  if (F & FZ) { CYCT(0x6a2b, 0x6a2e); goto deleteSelfAndReturn; } // jp z
  CYC(0x6a2b, 0x6a2e);
  CYC(0x6a2e, 0x6a31); SET_BC(0x00ba); // ldbc $00, <TX_05ba
  CYC(0x6a31, 0x6a34); goto setTextForScript; // jp

val0b:
  CYC(0x6a34, 0x6a37); SET_BC(0x00bb); // ldbc $00, <TX_05bb
  CYC(0x6a37, 0x6a3a); goto setTextForScript; // jp

deleteSelfAndReturn:
  CYC(0x6a3a, 0x6a3b); SET_AF(pop_effect(gb));
  CYC(0x6a3b, 0x6a3e); interactionDelete_hook(gb); return; // jp

setTextForScript:
  CYC(0x6a3e, 0x6a3f); H = D;
  CYC(0x6a3f, 0x6a41); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6a41, 0x6a42); mem_wr(gb, HL, B);
  CYC(0x6a42, 0x6a43); L = alu_inc8(gb, L);
  CYC(0x6a43, 0x6a44); mem_wr(gb, HL, C);
  RET(0x6a44);
  if (gb->pc == 0x6968 && gb->sp == sp0_) goto afterCheckConditionsAndSetText;
  return; // ret

// interactionCode8a@loadScript: single caller, state0's own return address 0x6973, pushed
// while sp == sp0_, and that address also happens to be @state1's own entry point in ROM.
loadScript:
  CYC(0x6a54, 0x6a56); A = 0x05; // >TX_0500
  CALL_C(0x6a56, interactionSetHighTextIndex_hook, 0x253b, 0x6a59);
  CYC(0x6a59, 0x6a5b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6a5b, 0x6a5c); A = mem_rd(gb, DE);
  CYC(0x6a5c, 0x6a5f); SET_HL(0x6a69); // @scriptTable
  CYC(0x6a5f, 0x6a60); interactionCode8a_add_double_index(gb, 0x6a60);
  CYC(0x6a60, 0x6a61); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6a61, 0x6a62); H = mem_rd(gb, HL);
  CYC(0x6a62, 0x6a63); L = A;
  CALL_C(0x6a63, interactionSetScript_hook, 0x2544, 0x6a66);
  CYC(0x6a66, 0x6a69); interactionIncState_hook(gb);
  if (gb->pc == 0x6973 && gb->sp == sp0_) goto state1;
  return; // jp
}
