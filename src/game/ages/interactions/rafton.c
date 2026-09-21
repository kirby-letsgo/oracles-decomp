#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode69_jump_table(GB *gb) {
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

static void interactionCode69_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// INTERAC_RAFTON
//
// Every internal transfer in this routine is a jr/jp/jump-table dispatch, never a call, so
// nothing here ever nests below the top level -- no push_effect or resume-check machinery needed.
void interactionCode69_hook(GB *gb) {
  BASE(interactionCode69);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode69_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+14);
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+16, b_+19);
  CALL_C(b_+19, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+22);
  CALL_C(b_+22, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+25);
  CYC(b_+25, b_+27); A = 0x27; // >TX_2700
  CALL_C(b_+27, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+30);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  {
    CYC(b_+33, b_+34); push_effect(gb, b_+34);
    uint16_t target = interactionCode69_jump_table(gb);
    if (target == b_+38) goto initSubid00;
    goto initSubid01;
  }

initSubid00:
  CYC(b_+38, b_+40); A = 0x26; // GLOBALFLAG_RAFTON_CHANGED_ROOMS
  CALL_C(b_+40, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+43);
  if (!(F & FZ)) { CYCT(b_+43, b_+46); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+48); C = 0x04;
  CYC(b_+48, b_+50); A = 0x54; // TREASURE_ISLAND_CHART
  CALL_C(b_+50, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+53);
  if (F & FC) { CYCT(b_+53, b_+55); goto setBehaviour; } // jr c
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+56); C = alu_dec8(gb, C);
  CYC(b_+56, b_+58); A = 0x15; // GLOBALFLAG_GAVE_ROPE_TO_RAFTON
  CALL_C(b_+58, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+61);
  if (!(F & FZ)) { CYCT(b_+61, b_+63); goto setBehaviour; } // jr nz
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); C = alu_dec8(gb, C);
  CYC(b_+64, b_+66); A = 0x52; // TREASURE_CHEVAL_ROPE
  CALL_C(b_+66, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+69);
  if (F & FC) { CYCT(b_+69, b_+71); goto setBehaviour; } // jr c
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+72); C = alu_dec8(gb, C);
  CYC(b_+72, b_+75); A = W8(wEssencesObtained);
  CYC(b_+75, b_+77); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto setBehaviour; } // jr nz
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); C = alu_dec8(gb, C);

setBehaviour:
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+83); L = INTERACTION_BASE + 0x38; // Interaction.var38
  CYC(b_+83, b_+84); mem_wr(gb, HL, C);
  CYC(b_+84, b_+127); goto loadScript; // jr

initSubid01:
  CYC(b_+86, b_+88); A = 0x26; // GLOBALFLAG_RAFTON_CHANGED_ROOMS
  CALL_C(b_+88, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+91);
  if (F & FZ) { CYCT(b_+91, b_+94); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+91, b_+94);
  CYC(b_+94, b_+127); goto loadScript; // jr

state1:
  CYC(b_+96, b_+98); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+98, b_+99); A = mem_rd(gb, DE);
  {
    CYC(b_+99, b_+100); push_effect(gb, b_+100);
    uint16_t target = interactionCode69_jump_table(gb);
    if (target == b_+104) goto runSubid00;
    goto runSubid01;
  }

runSubid00:
  CALL_C(b_+104, interactionRunScript_hook, SYM(interactionRunScript), b_+107);
  if (F & FC) { CYCT(b_+107, b_+110); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+107, b_+110);
  CYC(b_+110, b_+112); E = INTERACTION_BASE + 0x38; // Interaction.var38
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+115); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+115, b_+118); interactionAnimateBasedOnSpeed_hook(gb); return; } // jp z
  CYC(b_+115, b_+118);
  CYC(b_+118, b_+121); TAIL(interactionAnimateAsNpc); // jp

runSubid01:
  CALL_C(b_+121, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+124);
  CYC(b_+124, b_+127); TAIL(interactionRunScript); // jp

loadScript:
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+133); SET_HL(b_+140); // interactionCode69@scriptTable
  CYC(b_+133, b_+134); interactionCode69_addDoubleIndexToHl_from_rst(gb, b_+134);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+135, b_+136); H = mem_rd(gb, HL);
  CYC(b_+136, b_+137); L = A;
  CYC(b_+137, b_+140); TAIL(interactionSetScript); // jp
}
