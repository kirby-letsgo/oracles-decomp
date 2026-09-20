#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode8a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode8a), (from), (to), true)

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
  BASE(interactionCode8a);
  push_effect(gb, return_address);
  CYC(b_+246, b_+249); SET_HL(wEssencesObtained);
  CYC(b_+249, b_+252); checkFlag_hook(gb); // jp
}

// INTERAC_REMOTE_MAKU_CUTSCENE
void interactionCode8a_hook(GB *gb) {
  BASE(interactionCode8a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode8a_jump_table(gb);
    if (target == b_+8) goto subid0;
    HANDOFF(target);
  }

subid0:
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto state1; } // jr nz
  CYC(b_+11, b_+13);

  // interactionCode8a@state0
  CALL_C(b_+13, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+25); push_effect(gb, b_+25); goto checkConditionsAndSetText;
afterCheckConditionsAndSetText:
  CALL_C(b_+25, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+28);
  CYC(b_+28, b_+30); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+30, b_+33); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+30, b_+33);

  CYC(b_+33, b_+36); push_effect(gb, b_+36); goto loadScript;

state1:
  CALL_C(b_+36, interactionRunScript_hook, SYM(interactionRunScript), b_+39);
  if (F & FC) { CYCT(b_+39, b_+42); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+39, b_+42);
  RET(b_+42); return; // ret

// interactionCode8a@checkConditionsAndSetText: single caller, state0's own return address
// 0x6968, pushed above while sp == sp0_, so its ret's resume check compares against bare
// sp0_. Its jump table dispatches on var03 to twelve sibling val## branches that each either
// jp into @deleteSelfAndReturn -- which pops this call's own pushed return address, discarding
// it, and tail-jumps into interactionDelete, ending the whole hook -- or jp into
// @setTextForScript, whose ret resumes state0 via that same pushed 0x6968.
checkConditionsAndSetText:
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  {
    CYC(b_+46, b_+47); push_effect(gb, b_+47);
    uint16_t target = interactionCode8a_jump_table(gb);
    if (target == b_+71) goto val00;
    if (target == b_+84) goto val01;
    if (target == b_+90) goto val02;
    if (target == b_+104) goto val03;
    if (target == b_+118) goto val04;
    if (target == b_+145) goto val05;
    if (target == b_+159) goto val06;
    if (target == b_+173) goto val07;
    if (target == b_+187) goto val08;
    if (target == b_+201) goto val09;
    if (target == b_+215) goto val0a;
    if (target == b_+229) goto val0b;
    HANDOFF(target);
  }

val00:
  CYC(b_+71, b_+72); alu_xor(gb, A); // xor a
  CYC(b_+72, b_+75); interactionCode8a_checkEssenceObtained(gb, b_+75);
  if (F & FZ) { CYCT(b_+75, b_+78); goto deleteSelfAndReturn; } // jp z
  CYC(b_+75, b_+78);
  CYC(b_+78, b_+81); SET_BC(0x00b0); // ldbc $00, <TX_05b0
  CYC(b_+81, b_+84); goto setTextForScript; // jp

val01:
  CYC(b_+84, b_+87); SET_BC(0x00b1); // ldbc $00, <TX_05b1
  CYC(b_+87, b_+90); goto setTextForScript; // jp

val02:
  CYC(b_+90, b_+92); A = 0x11; // TREASURE_HARP
  CALL_C(b_+92, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+95);
  if (!(F & FC)) { CYCT(b_+95, b_+98); goto deleteSelfAndReturn; } // jp nc
  CYC(b_+95, b_+98);
  CYC(b_+98, b_+101); SET_BC(0x00b2); // ldbc $00, <TX_05b2
  CYC(b_+101, b_+104); goto setTextForScript; // jp

val03:
  CYC(b_+104, b_+106); A = 0x01;
  CYC(b_+106, b_+109); interactionCode8a_checkEssenceObtained(gb, b_+109);
  if (F & FZ) { CYCT(b_+109, b_+112); goto deleteSelfAndReturn; } // jp z
  CYC(b_+109, b_+112);
  CYC(b_+112, b_+115); SET_BC(0x00b3); // ldbc $00, <TX_05b3
  CYC(b_+115, b_+118); goto setTextForScript; // jp

val04:
  CYC(b_+118, b_+120); A = 0x02;
  CYC(b_+120, b_+123); interactionCode8a_checkEssenceObtained(gb, b_+123);
  if (F & FZ) { CYCT(b_+123, b_+126); goto deleteSelfAndReturn; } // jp z
  CYC(b_+123, b_+126);
  CYC(b_+126, b_+129); SET_HL(wGroup1RoomFlags + 0x76); // wPastRoomFlags+$76
  CYC(b_+129, b_+131); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 0)); // set 0,(hl)
  CALL_C(b_+131, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+134);
  CYC(b_+134, b_+136); A = 0x1d; // GLOBALFLAG_CAN_BUY_FLUTE
  if (F & FZ) CALL_C_CC(b_+136, setGlobalFlag_hook, SYM(setGlobalFlag), b_+139); else CYC(b_+136, b_+139); // call z
  CYC(b_+139, b_+142); SET_BC(0x00b4); // ldbc $00, <TX_05b4
  CYC(b_+142, b_+145); goto setTextForScript; // jp

val05:
  CYC(b_+145, b_+147); A = 0x03;
  CYC(b_+147, b_+150); interactionCode8a_checkEssenceObtained(gb, b_+150);
  if (F & FZ) { CYCT(b_+150, b_+153); goto deleteSelfAndReturn; } // jp z
  CYC(b_+150, b_+153);
  CYC(b_+153, b_+156); SET_BC(0x00b5); // ldbc $00, <TX_05b5
  CYC(b_+156, b_+159); goto setTextForScript; // jp

val06:
  CYC(b_+159, b_+161); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+161, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+164);
  if (F & FZ) { CYCT(b_+164, b_+167); goto deleteSelfAndReturn; } // jp z
  CYC(b_+164, b_+167);
  CYC(b_+167, b_+170); SET_BC(0x00b6); // ldbc $00, <TX_05b6
  CYC(b_+170, b_+173); goto setTextForScript; // jp

val07:
  CYC(b_+173, b_+175); A = 0x04;
  CYC(b_+175, b_+178); interactionCode8a_checkEssenceObtained(gb, b_+178);
  if (F & FZ) { CYCT(b_+178, b_+181); goto deleteSelfAndReturn; } // jp z
  CYC(b_+178, b_+181);
  CYC(b_+181, b_+184); SET_BC(0x00b7); // ldbc $00, <TX_05b7
  CYC(b_+184, b_+187); goto setTextForScript; // jp

val08:
  CYC(b_+187, b_+189); A = 0x05;
  CYC(b_+189, b_+192); interactionCode8a_checkEssenceObtained(gb, b_+192);
  if (F & FZ) { CYCT(b_+192, b_+195); goto deleteSelfAndReturn; } // jp z
  CYC(b_+192, b_+195);
  CYC(b_+195, b_+198); SET_BC(0x00b8); // ldbc $00, <TX_05b8
  CYC(b_+198, b_+201); goto setTextForScript; // jp

val09:
  CYC(b_+201, b_+203); A = 0x06;
  CYC(b_+203, b_+206); interactionCode8a_checkEssenceObtained(gb, b_+206);
  if (F & FZ) { CYCT(b_+206, b_+209); goto deleteSelfAndReturn; } // jp z
  CYC(b_+206, b_+209);
  CYC(b_+209, b_+212); SET_BC(0x00b9); // ldbc $00, <TX_05b9
  CYC(b_+212, b_+215); goto setTextForScript; // jp

val0a:
  CYC(b_+215, b_+217); A = 0x07;
  CYC(b_+217, b_+220); interactionCode8a_checkEssenceObtained(gb, b_+220);
  if (F & FZ) { CYCT(b_+220, b_+223); goto deleteSelfAndReturn; } // jp z
  CYC(b_+220, b_+223);
  CYC(b_+223, b_+226); SET_BC(0x00ba); // ldbc $00, <TX_05ba
  CYC(b_+226, b_+229); goto setTextForScript; // jp

val0b:
  CYC(b_+229, b_+232); SET_BC(0x00bb); // ldbc $00, <TX_05bb
  CYC(b_+232, b_+235); goto setTextForScript; // jp

deleteSelfAndReturn:
  CYC(b_+235, b_+236); SET_AF(pop_effect(gb));
  CYC(b_+236, b_+239); interactionDelete_hook(gb); return; // jp

setTextForScript:
  CYC(b_+239, b_+240); H = D;
  CYC(b_+240, b_+242); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+242, b_+243); mem_wr(gb, HL, B);
  CYC(b_+243, b_+244); L = alu_inc8(gb, L);
  CYC(b_+244, b_+245); mem_wr(gb, HL, C);
  RET(b_+245);
  if (gb->pc == b_+25 && gb->sp == sp0_) goto afterCheckConditionsAndSetText;
  return; // ret

// interactionCode8a@loadScript: single caller, state0's own return address 0x6973, pushed
// while sp == sp0_, and that address also happens to be @state1's own entry point in ROM.
loadScript:
  CYC(b_+261, b_+263); A = 0x05; // >TX_0500
  CALL_C(b_+263, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+266);
  CYC(b_+266, b_+268); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+268, b_+269); A = mem_rd(gb, DE);
  CYC(b_+269, b_+272); SET_HL(b_+282); // @scriptTable
  CYC(b_+272, b_+273); interactionCode8a_add_double_index(gb, b_+273);
  CYC(b_+273, b_+274); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+274, b_+275); H = mem_rd(gb, HL);
  CYC(b_+275, b_+276); L = A;
  CALL_C(b_+276, interactionSetScript_hook, SYM(interactionSetScript), b_+279);
  CYC(b_+279, b_+282); interactionIncState_hook(gb);
  if (gb->pc == b_+36 && gb->sp == sp0_) goto state1;
  return; // jp
}
