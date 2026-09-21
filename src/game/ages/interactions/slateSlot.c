#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodedb_jump_table(GB *gb) {
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

// interactionCodedb@resetCounter (0b:7f7e): always ends in a real "ret", so it must pop via
// ret_effect() -- reached both via genuine `call nc,` (through CALL_C_CC, which pushes a matching
// return address first) from @state1's two sites, and via plain fallthrough / `jr nc,` tail-jumps
// from @state1's "not enough slates" path and from @placeSlate (no push there, so ret_effect()
// pops whatever the real stack already holds -- the original caller of the whole hook).
static void interactionCodedb_resetCounter(GB *gb) {
  BASE(interactionCodedb);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+67, b_+69); A = 0x0a;
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  RET(b_+70); return;
}

// ==================================================================================================
// INTERAC_SLATE_SLOT
//
// Variables:
//   var3f: Counter to push against this object until the slate will be placed
// ==================================================================================================
void interactionCodedb_hook(GB *gb) {
  BASE(interactionCodedb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodedb_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+35) { goto state1; }
    else if (jt_ == b_+93) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  // Check if slate already placed
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+16); SET_BC(0x00f8); // bitTable
  CYC(b_+16, b_+17); alu_add(gb, C);
  CYC(b_+17, b_+18); C = A;
  CALL_C(b_+18, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, BC);
  CYC(b_+22, b_+23); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+23, b_+26); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+29); SET_HL(b_+29); // mainScripts.slateSlotScript
  CALL_C(b_+29, interactionSetScript_hook, SYM(interactionSetScript), b_+32);
  CYC(b_+32, b_+35); TAIL(interactionIncState); // jp

state1:
  CALL_C(b_+35, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+38);
  if (!(F & FC)) CALL_C_CC(b_+38, interactionCodedb_resetCounter, b_+65, b_+41); else CYC(b_+38, b_+41); // call nc
  CALL_C(b_+41, objectCheckLinkPushingAgainstCenter_hook, SYM(objectCheckLinkPushingAgainstCenter), b_+44);
  if (!(F & FC)) CALL_C_CC(b_+44, interactionCodedb_resetCounter, b_+65, b_+47); else CYC(b_+44, b_+47); // call nc
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto state2; } // jr nz
  CYC(b_+51, b_+53);

  // Time to place the slate, if available
  CYC(b_+53, b_+56); A = mem_rd(gb, wNumSlates);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+57, b_+59); goto placeSlate; } // jr nz
  CYC(b_+57, b_+59);

  // Not enough slates
  CYC(b_+59, b_+62); SET_BC((SYM(interactionCode9a__initialize) + 15)); // TX_5111
  CALL_C(b_+62, showText_hook, SYM(showText), b_+65);
  interactionCodedb_resetCounter(gb); return; // falls through into @resetCounter

placeSlate:
  CALL_C(b_+71, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+74);
  if (!(F & FC)) { CYCT(b_+74, b_+76); interactionCodedb_resetCounter(gb); return; } // jr nc
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+78, b_+81); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+81, b_+84); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+84, b_+87); SET_HL(b_+32); // mainScripts.slateSlotScript_placeSlate
  CALL_C(b_+87, interactionSetScript_hook, SYM(interactionSetScript), b_+90);
  CALL_C(b_+90, interactionIncState_hook, SYM(interactionIncState), b_+93);
  // falls through into @state2

state2:
  CALL_C(b_+93, interactionRunScript_hook, SYM(interactionRunScript), b_+96);
  if (!(F & FC)) { CYCT(b_+96, b_+97); ret_effect(gb); return; } // ret nc
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+100); TAIL(interactionDelete); // jp
}
