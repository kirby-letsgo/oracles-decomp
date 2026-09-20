#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode68), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode68), (from), (to), true)

static uint16_t interactionCode68_jump_table(GB *gb) {
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

static void interactionCode68_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_ROSA
//
// interactionCode68@initGraphicsAndIncState (0x4ce6-0x4cee) is dead code in the ROM -- no call,
// jump, or fallthrough reaches it from anywhere in this routine -- so its bytes are omitted here.
//
// initGraphicsAndLoadScript and loadScriptFromTableAndInitGraphics are each called once (from
// subid00@state0 and subid01@state0), then jr into loadScriptAndIncState / loadScriptFromTable-
// AndIncState at the same stack depth as their caller. Both of those tail-jump into
// interactionIncState_hook without ever executing their own ret, so it is that external hook's
// own return that eventually pops the still-outstanding outer call and resumes this function --
// handled below with the same tail-jump-then-check pattern used for external hooks elsewhere.
// getScript is called from both of those (loadScriptAndIncState, loadScriptFromTableAndIncState),
// each one level deeper than the top level because of that same still-outstanding outer call, so
// its own two resume checks compare against sp0_-2, not bare sp0_.
void interactionCode68_hook(GB *gb) {
  BASE(interactionCode68);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode68_jump_table(gb);
    if (target == b_+8) goto subid00;
    goto subid01;
  }

subid00:
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto subid00_state1; } // jr nz
  CYC(b_+11, b_+13);

  // interactionCode68@subid00@state0
  CALL_C(b_+13, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+16);
  if (F & FZ) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+22); A = W8(wEssencesObtained);
  CYC(b_+22, b_+24); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+30); push_effect(gb, b_+30); goto initGraphicsAndLoadScript;
afterInitGraphicsAndLoadScript:
  CALL_C(b_+30, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+33);
  CALL_C(b_+33, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+36);
  CYC(b_+36, b_+38); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto subid00_alreadyGaveShovel; } // jr nz
  CYC(b_+38, b_+40);
  CALL_C(b_+40, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+43);
  if (!(F & FZ)) { RET_TAKEN(b_+43); return; } // ret nz
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x6b); // INTERAC_MISCELLANEOUS_1
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x09);
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_RELATED1 + 1; // Interaction.relatedObj1+1
  CYC(b_+51, b_+52); A = D;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  RET(b_+53); return; // ret

subid00_alreadyGaveShovel:
  CYC(b_+54, b_+57); SET_HL((SYM(miscPuzzles_subid0e__state1) + 1)); // mainScripts.rosa_subid00Script_alreadyGaveShovel
  CYC(b_+57, b_+60); interactionSetScript_hook(gb); return; // jp

subid00_state1:
  CALL_C(b_+60, interactionRunScript_hook, SYM(interactionRunScript), b_+63);
  CYC(b_+63, b_+65); A = 0x15; // TREASURE_SHOVEL
  CALL_C(b_+65, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+68);
  if (F & FC) { CYCT(b_+68, b_+71); npcFaceLinkAndAnimate_hook(gb); return; } // jp c
  CYC(b_+68, b_+71);
  CYC(b_+71, b_+74); interactionAnimateAsNpc_hook(gb); return; // jp

subid01:
  CALL_C(b_+74, checkInteractionState_hook, SYM(checkInteractionState), b_+77);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto subid01_state1; } // jr nz
  CYC(b_+77, b_+79);

  // interactionCode68@subid01@state0
  CYC(b_+79, b_+82); push_effect(gb, b_+82); goto loadScriptFromTableAndInitGraphics;
afterLoadScriptFromTableAndInitGraphics:
  CYC(b_+82, b_+84); L = INTERACTION_BASE + 0x37; // Interaction.var37
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x04);
  CALL_C(b_+86, interactionRunScript_hook, SYM(interactionRunScript), b_+89);

subid01_state1:
  CALL_C(b_+89, interactionRunScript_hook, SYM(interactionRunScript), b_+92);
  if (F & FC) { CYCT(b_+92, b_+95); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+92, b_+95);
  CYC(b_+95, b_+98); npcFaceLinkAndAnimate_hook(gb); return; // jp

initGraphicsAndLoadScript:
  CALL_C(b_+107, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+110);
  CALL_C(b_+110, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+113);
  CYC(b_+113, b_+115); goto loadScriptAndIncState;

loadScriptFromTableAndInitGraphics:
  CALL_C(b_+115, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+118);
  CALL_C(b_+118, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+121);
  CYC(b_+121, b_+123); goto loadScriptFromTableAndIncState;

loadScriptAndIncState:
  CYC(b_+123, b_+126); push_effect(gb, b_+126); goto getScript;
afterGetScript1:
  CALL_C(b_+126, interactionSetScript_hook, SYM(interactionSetScript), b_+129);
  CYC(b_+129, b_+132); interactionIncState_hook(gb);
  if (gb->pc == b_+30 && gb->sp == sp0_) goto afterInitGraphicsAndLoadScript;
  return; // jp

loadScriptFromTableAndIncState:
  CYC(b_+132, b_+135); push_effect(gb, b_+135); goto getScript;
afterGetScript2:
  CYC(b_+135, b_+136); E = alu_inc8(gb, E); // Interaction.var03
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+138); interactionCode68_addDoubleIndexToHl_from_rst(gb, b_+138);
  CYC(b_+138, b_+139); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+139, b_+140); H = mem_rd(gb, HL);
  CYC(b_+140, b_+141); L = A;
  CALL_C(b_+141, interactionSetScript_hook, SYM(interactionSetScript), b_+144);
  CYC(b_+144, b_+147); interactionIncState_hook(gb);
  if (gb->pc == b_+82 && gb->sp == sp0_) goto afterLoadScriptFromTableAndInitGraphics;
  return; // jp

getScript:
  CYC(b_+147, b_+149); A = 0x1c; // >TX_1c00
  CALL_C(b_+149, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+152);
  CYC(b_+152, b_+154); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+158); SET_HL(b_+163); // interactionCode68@scriptTable
  CYC(b_+158, b_+159); interactionCode68_addDoubleIndexToHl_from_rst(gb, b_+159);
  CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+160, b_+161); H = mem_rd(gb, HL);
  CYC(b_+161, b_+162); L = A;
  RET(b_+162);
  if (gb->pc == b_+126 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterGetScript1;
  if (gb->pc == b_+135 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterGetScript2;
  return; // ret
}
