#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x4d2d, 0x4d2f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4d2f, 0x4d30); A = mem_rd(gb, DE);
  {
    CYC(0x4d30, 0x4d31); push_effect(gb, 0x4d31);
    uint16_t target = interactionCode69_jump_table(gb);
    if (target == 0x4d35) goto state0;
    goto state1;
  }

state0:
  CYC(0x4d35, 0x4d37); A = 0x01;
  CYC(0x4d37, 0x4d38); mem_wr(gb, DE, A);
  CALL_C(0x4d38, getThisRoomFlags_hook, 0x197d, 0x4d3b);
  CYC(0x4d3b, 0x4d3d); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x4d3d, 0x4d40); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x4d3d, 0x4d40);
  CALL_C(0x4d40, interactionInitGraphics_hook, 0x15fb, 0x4d43);
  CALL_C(0x4d43, objectSetVisiblec2_hook, 0x1e45, 0x4d46);
  CYC(0x4d46, 0x4d48); A = 0x27; // >TX_2700
  CALL_C(0x4d48, interactionSetHighTextIndex_hook, 0x253b, 0x4d4b);
  CYC(0x4d4b, 0x4d4d); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4d4d, 0x4d4e); A = mem_rd(gb, DE);
  {
    CYC(0x4d4e, 0x4d4f); push_effect(gb, 0x4d4f);
    uint16_t target = interactionCode69_jump_table(gb);
    if (target == 0x4d53) goto initSubid00;
    goto initSubid01;
  }

initSubid00:
  CYC(0x4d53, 0x4d55); A = 0x26; // GLOBALFLAG_RAFTON_CHANGED_ROOMS
  CALL_C(0x4d55, checkGlobalFlag_hook, 0x31f3, 0x4d58);
  if (!(F & FZ)) { CYCT(0x4d58, 0x4d5b); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x4d58, 0x4d5b);
  CYC(0x4d5b, 0x4d5d); C = 0x04;
  CYC(0x4d5d, 0x4d5f); A = 0x54; // TREASURE_ISLAND_CHART
  CALL_C(0x4d5f, checkTreasureObtained_hook, 0x1748, 0x4d62);
  if (F & FC) { CYCT(0x4d62, 0x4d64); goto setBehaviour; } // jr c
  CYC(0x4d62, 0x4d64);
  CYC(0x4d64, 0x4d65); C = alu_dec8(gb, C);
  CYC(0x4d65, 0x4d67); A = 0x15; // GLOBALFLAG_GAVE_ROPE_TO_RAFTON
  CALL_C(0x4d67, checkGlobalFlag_hook, 0x31f3, 0x4d6a);
  if (!(F & FZ)) { CYCT(0x4d6a, 0x4d6c); goto setBehaviour; } // jr nz
  CYC(0x4d6a, 0x4d6c);
  CYC(0x4d6c, 0x4d6d); C = alu_dec8(gb, C);
  CYC(0x4d6d, 0x4d6f); A = 0x52; // TREASURE_CHEVAL_ROPE
  CALL_C(0x4d6f, checkTreasureObtained_hook, 0x1748, 0x4d72);
  if (F & FC) { CYCT(0x4d72, 0x4d74); goto setBehaviour; } // jr c
  CYC(0x4d72, 0x4d74);
  CYC(0x4d74, 0x4d75); C = alu_dec8(gb, C);
  CYC(0x4d75, 0x4d78); A = W8(wEssencesObtained);
  CYC(0x4d78, 0x4d7a); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x4d7a, 0x4d7c); goto setBehaviour; } // jr nz
  CYC(0x4d7a, 0x4d7c);
  CYC(0x4d7c, 0x4d7d); C = alu_dec8(gb, C);

setBehaviour:
  CYC(0x4d7d, 0x4d7e); H = D;
  CYC(0x4d7e, 0x4d80); L = INTERACTION_BASE + 0x38; // Interaction.var38
  CYC(0x4d80, 0x4d81); mem_wr(gb, HL, C);
  CYC(0x4d81, 0x4dac); goto loadScript; // jr

initSubid01:
  CYC(0x4d83, 0x4d85); A = 0x26; // GLOBALFLAG_RAFTON_CHANGED_ROOMS
  CALL_C(0x4d85, checkGlobalFlag_hook, 0x31f3, 0x4d88);
  if (F & FZ) { CYCT(0x4d88, 0x4d8b); interactionDelete_hook(gb); return; } // jp z
  CYC(0x4d88, 0x4d8b);
  CYC(0x4d8b, 0x4dac); goto loadScript; // jr

state1:
  CYC(0x4d8d, 0x4d8f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4d8f, 0x4d90); A = mem_rd(gb, DE);
  {
    CYC(0x4d90, 0x4d91); push_effect(gb, 0x4d91);
    uint16_t target = interactionCode69_jump_table(gb);
    if (target == 0x4d95) goto runSubid00;
    goto runSubid01;
  }

runSubid00:
  CALL_C(0x4d95, interactionRunScript_hook, 0x2552, 0x4d98);
  if (F & FC) { CYCT(0x4d98, 0x4d9b); interactionDelete_hook(gb); return; } // jp c
  CYC(0x4d98, 0x4d9b);
  CYC(0x4d9b, 0x4d9d); E = INTERACTION_BASE + 0x38; // Interaction.var38
  CYC(0x4d9d, 0x4d9e); A = mem_rd(gb, DE);
  CYC(0x4d9e, 0x4da0); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x4da0, 0x4da3); interactionAnimateBasedOnSpeed_hook(gb); return; } // jp z
  CYC(0x4da0, 0x4da3);
  CYC(0x4da3, 0x4da6); interactionAnimateAsNpc_hook(gb); return; // jp

runSubid01:
  CALL_C(0x4da6, interactionAnimateAsNpc_hook, 0x26db, 0x4da9);
  CYC(0x4da9, 0x4dac); interactionRunScript_hook(gb); return; // jp

loadScript:
  CYC(0x4dac, 0x4dae); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4dae, 0x4daf); A = mem_rd(gb, DE);
  CYC(0x4daf, 0x4db2); SET_HL(0x4db9); // interactionCode69@scriptTable
  CYC(0x4db2, 0x4db3); interactionCode69_addDoubleIndexToHl_from_rst(gb, 0x4db3);
  CYC(0x4db3, 0x4db4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4db4, 0x4db5); H = mem_rd(gb, HL);
  CYC(0x4db5, 0x4db6); L = A;
  CYC(0x4db6, 0x4db9); interactionSetScript_hook(gb); return; // jp
}
