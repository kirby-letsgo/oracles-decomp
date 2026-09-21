#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodea6_jump_table(GB *gb) {
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

// 0b:5dbd, called once from interactionCodea6@state0.
void interactionCodea6_createSparkle_hook(GB *gb) {
  BASE(interactionCodea6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+140, b_+143); SET_BC(0x840f); // INTERAC_SPARKLE, $0f
  CALL_C(b_+143, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+146);
  if (!(F & FZ)) { CYCT(b_+146, b_+147); ret_effect(gb); return; } // ret nz
  CYC(b_+146, b_+147);
  CYC(b_+147, b_+149); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+149, b_+151); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+151, b_+152); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+152, b_+153); mem_wr(gb, HL, D);
  RET(b_+153); return;
}

// 0b:5d87. Marked "Unused function?" in the disassembly source -- callers: 0, unreachable from
// interactionCodea6 or anywhere else, but ported here for completeness since it's part of the
// same source file.
void interactionCodea6_func_5d87_hook(GB *gb) {
  BASE(interactionCodea6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+86, b_+89); SET_BC(0x840b); // INTERAC_SPARKLE, $0b
  CALL_C(b_+89, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+92);
  if (!(F & FZ)) { CYCT(b_+92, b_+93); ret_effect(gb); return; } // ret nz
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+95); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0xc2); // 194
  CALL_C(b_+97, objectCopyPosition_hook, SYM(objectCopyPosition), b_+100);
  CALL_C(b_+100, getRandomNumber_hook, SYM(getRandomNumber), b_+103);
  CYC(b_+103, b_+105); alu_and(gb, 0x07);
  CYC(b_+105, b_+106); alu_add(gb, A);
  CYC(b_+106, b_+109); SET_BC(b_+124); // @offsets
  CALL_C(b_+109, addAToBc_hook, 0x006d, b_+112);
  CYC(b_+112, b_+113); A = mem_rd(gb, BC);
  CYC(b_+113, b_+115); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+115, b_+116); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+116, b_+117); mem_wr(gb, HL, A);
  CYC(b_+117, b_+118); SET_BC(BC + 1); // inc bc
  CYC(b_+118, b_+119); A = mem_rd(gb, BC);
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+121, b_+122); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+122, b_+123); mem_wr(gb, HL, A);
  RET(b_+123); return;

  // @offsets (0b:5dad): pure ROM data (8 signed (dy,dx) byte pairs), referenced only via
  // SET_BC(0x5dad) above.
}

// ==================================================================================================
// INTERAC_MAKU_SEED
//
// Variables:
//   var38: ?
// ==================================================================================================
void interactionCodea6_hook(GB *gb) {
  BASE(interactionCodea6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodea6_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+47) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, interactionIncState_hook, SYM(interactionIncState), b_+11);
  CYC(b_+11, b_+13); A = 0xab; // PALH_ab
  CALL_C(b_+13, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+16);
  CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
  CYC(b_+19, b_+22); SET_HL(w1Link_yh);
  CYC(b_+22, b_+23); B = mem_rd(gb, HL);
  CYC(b_+23, b_+25); L = (uint8_t)w1Link_xh;
  CYC(b_+25, b_+26); C = mem_rd(gb, HL);
  CALL_C(b_+26, interactionSetPosition_hook, SYM(interactionSetPosition), b_+29);
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x8b);
  CYC(b_+33, b_+36); A = mem_rd(gb, wFrameCounter);
  CYC(b_+36, b_+37); alu_cpl(gb);
  CYC(b_+37, b_+38); A = alu_inc8(gb, A);
  CYC(b_+38, b_+40); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CALL_C(b_+41, objectSetVisible82_hook, SYM(objectSetVisible82), b_+44);
  CALL_L(b_+44, interactionCodea6_createSparkle_hook, b_+47);
  // falls through into @state1 (also a direct jump-table target)

state1:
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+50, b_+51); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+51, b_+53); alu_cp(gb, 0xf3);
  if (F & FC) { CYCT(b_+53, b_+55); goto l_5d70; } // jr c
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); A = 0x01;
  CYC(b_+57, b_+60); mem_wr(gb, wTmpcfc0_genericCutscene_state, A);
  CYC(b_+60, b_+63); TAIL(interactionDelete); // jp

l_5d70:
  CYC(b_+63, b_+66); SET_BC(0x0080);
  CYC(b_+66, b_+67); A = C;
  CYC(b_+67, b_+68); alu_add(gb, mem_rd(gb, HL)); // [zh]
  CYC(b_+68, b_+69); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+69, b_+70); A = B;
  CYC(b_+70, b_+71); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+72); mem_wr(gb, HL, A);
  CYC(b_+72, b_+75); A = mem_rd(gb, wFrameCounter);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+77, b_+78); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+78, b_+80); alu_and(gb, 0x3f);
  CYC(b_+80, b_+82); A = 0x83; // SND_MAGIC_POWDER
  if (F & FZ) { CALL_C_CC(b_+82, playSound_b00_hook, SYM(playSound_b00), b_+85); } else { CYC(b_+82, b_+85); } // call z
  RET(b_+85); return;
}
