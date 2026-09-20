#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(creditsTextHorizontal_6537), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(creditsTextHorizontal_6537), (from), (to), true)

static uint16_t interactionCodeae_jump_table(GB *gb) {
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

static void interactionCodeae_addAToHl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void interactionCodeae_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Mutually chained via straight fallthrough (creditsTextHorizontal_6537 -> _6554 -> _6559) and,
// on the "still empty" path, _6559 tail-jumps back into _6537. All three are bare-global.
void creditsTextHorizontal_6537_hook(GB *gb);
void creditsTextHorizontal_6554_hook(GB *gb);
void creditsTextHorizontal_6559_hook(GB *gb);

// 0b:6537, called from creditsTextHorizontal_6559 (tail `jp`) and interactionCodeae@substate0.
void creditsTextHorizontal_6537_hook(GB *gb) {
  BASE(creditsTextHorizontal_6537);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto l_6550; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0xae); // INTERAC_CREDITS_TEXT_HORIZONTAL
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [child.subid]
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x01); // [child.var03]
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+16, b_+17); E = L;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); E = alu_inc8(gb, E);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+20, b_+21); A = mem_rd(gb, DE); // [counter2]
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CALL_C(b_+22, objectCopyPosition_hook, SYM(objectCopyPosition), b_+25);

l_6550:
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+28, SYM(creditsTextHorizontal_6554)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  creditsTextHorizontal_6554_hook(gb); return; // falls through
}

// 0b:6554, called from interactionCodeae@subsubstate1; falls into (and out of)
// creditsTextHorizontal_6537.
void creditsTextHorizontal_6554_hook(GB *gb) {
  BASE(creditsTextHorizontal_6554);
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+3, b_+4); H = mem_rd(gb, HL);
  CYC(b_+4, SYM(creditsTextHorizontal_6559)); L = A;
  creditsTextHorizontal_6559_hook(gb); return; // falls through
}

// 0b:6559, called from interactionCodeae@state0.
// @param[in] hl  Script pointer.
void creditsTextHorizontal_6559_hook(GB *gb) {
  BASE(creditsTextHorizontal_6559);
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+1, b_+3); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+6, b_+7); mem_wr(gb, DE, A); // [var31]
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+17, b_+18); A = L;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = H;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+27); ret_effect(gb); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+28); E = alu_dec8(gb, E);
  CYC(b_+28, b_+29); A = mem_rd(gb, DE); // [var30]
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+31); ret_effect(gb); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, SYM(horizontalCreditsText_var03Nonzero)); creditsTextHorizontal_6537_hook(gb); return; // jp
}

// 0b:657b, called from interactionCodeae@state1 (tail `jp`).
void horizontalCreditsText_var03Nonzero_hook(GB *gb) {
  BASE(horizontalCreditsText_var03Nonzero);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  do { uint16_t jt_ = (interactionCodeae_jump_table(gb));
    if (jt_ == b_+13) { goto substate0; }
    else if (jt_ == b_+42) { goto substate1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto applySpeed; } // jr nz
  CYC(b_+17, b_+19);
  CALL_C(b_+19, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+22);
  CYC(b_+22, b_+24); B = 0xa0;
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+30); goto l_659b; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); B = 0x50;

l_659b:
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+34, b_+35); mem_wr(gb, HL, B);
  RET(b_+35); return;

applySpeed:
  CALL_C(b_+36, objectApplySpeed_hook, SYM(objectApplySpeed), b_+39);
  CYC(b_+39, b_+42); objectApplySpeed_hook(gb); return; // jp

substate1:
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+46, b_+47); ret_effect(gb); return; } // ret z
  CYC(b_+46, b_+47);
  CALL_C(b_+47, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+50);
  if (F & FZ) { CYCT(b_+50, b_+53); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+50, b_+53);
  RET(b_+53); return;
}

// ==================================================================================================
// INTERAC_CREDITS_TEXT_HORIZONTAL
//
// Variables:
//   var03: ?
//   var30: ?
//   var31: ?
//   var32: ?
//   var33: ?
// ==================================================================================================
void interactionCodeae_hook(GB *gb) {
  BASE(interactionCodeae);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodeae_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+87) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state]
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto var03Nonzero; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+24); SET_HL(SYM(horizontalCreditsText_scriptTable)); // horizontalCreditsText_scriptTable
  CYC(b_+24, b_+25); interactionCodeae_addDoubleIndex(gb, b_+25);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+26, b_+27); H = mem_rd(gb, HL);
  CYC(b_+27, b_+28); L = A;
  CALL_C(b_+28, creditsTextHorizontal_6559_hook, SYM(creditsTextHorizontal_6559), b_+31);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+37); SET_HL(SYM(horizontalCreditsText_65b1)); // horizontalCreditsText_65b1
  CYC(b_+37, b_+38); interactionCodeae_addDoubleIndex(gb, b_+38);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  RET(b_+46); return;

var03Nonzero:
  CALL_C(b_+47, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+50);
  CYC(b_+50, b_+51); H = D;
  CYC(b_+51, b_+53); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x14);
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+59, b_+61); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CALL_C(b_+62, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+65);
  CYC(b_+65, b_+66); H = D;
  CYC(b_+66, b_+68); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+70); alu_or(gb, A);
  CYC(b_+70, b_+73); SET_BC(0xf018);
  if (F & FZ) { CYCT(b_+73, b_+75); goto l_642b; } // jr z
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+78); SET_BC(0x0008);

l_642b:
  CYC(b_+78, b_+80); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+80, b_+81); mem_wr(gb, HL, B);
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+83, b_+84); mem_wr(gb, HL, C);
  CYC(b_+84, b_+87); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(b_+87, b_+89); A = 0x01;
  CYC(b_+89, b_+90); mem_wr(gb, DE, A); // [state]
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+94); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+94, b_+97); horizontalCreditsText_var03Nonzero_hook(gb); return; } // jp nz
  CYC(b_+94, b_+97);
  CYC(b_+97, b_+100); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+100, b_+101); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+101, b_+102); ret_effect(gb); return; } // ret nz
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+104); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+104, b_+105); A = mem_rd(gb, DE);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  do { uint16_t jt_ = (interactionCodeae_jump_table(gb));
    if (jt_ == b_+112) { goto substate0; }
    else if (jt_ == b_+143) { goto substate1; }
    else if (jt_ == b_+334) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

substate0:
  CYC(b_+112, b_+113); H = D;
  CYC(b_+113, b_+115); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CALL_C(b_+115, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+118);
  if (!(F & FZ)) { CYCT(b_+118, b_+119); ret_effect(gb); return; } // ret nz
  CYC(b_+118, b_+119);
  CALL_C(b_+119, creditsTextHorizontal_6537_hook, SYM(creditsTextHorizontal_6537), b_+122);
  // falls through into @func_6457

func_6457:
  CYC(b_+122, b_+124); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+126); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+126, b_+127); ret_effect(gb); return; } // ret nc
  CYC(b_+126, b_+127);
  CYC(b_+127, b_+129); B = 0x01;
  CYC(b_+129, b_+130); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+130, b_+132); goto l_6463; } // jr nc
  CYC(b_+130, b_+132);
  CYC(b_+132, b_+134); B = 0x02;

l_6463:
  CYC(b_+134, b_+135); H = D;
  CYC(b_+135, b_+137); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+137, b_+139); mem_wr(gb, HL, 0xb4); // 180
  CYC(b_+139, b_+141); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+141, b_+142); mem_wr(gb, HL, B);
  RET(b_+142); return;

substate1:
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+147); push_effect(gb, b_+147);
  do { uint16_t jt_ = (interactionCodeae_jump_table(gb));
    if (jt_ == b_+155) { goto subsubstate0; }
    else if (jt_ == b_+164) { goto subsubstate1; }
    else if (jt_ == b_+242) { goto subsubstate2; }
    else if (jt_ == b_+254) { goto subsubstate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subsubstate0:
  CALL_C(b_+155, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+158);
  if (!(F & FZ)) { CYCT(b_+158, b_+159); ret_effect(gb); return; } // ret nz
  CYC(b_+158, b_+159);
  CYC(b_+159, b_+160); H = D;
  CYC(b_+160, b_+162); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+162, b_+163); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+163); return;

subsubstate1:
  CYC(b_+164, b_+167); A = mem_rd(gb, wFrameCounter);
  CYC(b_+167, b_+169); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+169, b_+170); ret_effect(gb); return; } // ret nz
  CYC(b_+169, b_+170);
  CYC(b_+170, b_+171); H = D;
  CYC(b_+171, b_+173); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+173, b_+174); A = mem_rd(gb, HL);
  CYC(b_+174, b_+176); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+176, b_+178); goto label_0b_234; } // jr nz
  CYC(b_+176, b_+178);
  CYC(b_+178, b_+180); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+180, b_+181); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+181, b_+183); L = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+183, b_+184); A = mem_rd(gb, HL);
  CYC(b_+184, b_+186); alu_sub(gb, 0x03);
  CYC(b_+186, b_+187); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+187, b_+188); A = mem_rd(gb, HL);
  CYC(b_+188, b_+190); alu_sbc(gb, 0x00);
  CYC(b_+190, b_+191); mem_wr(gb, HL, A);
  CALL_C(b_+191, creditsTextHorizontal_6554_hook, SYM(creditsTextHorizontal_6554), b_+194);
  CYC(b_+194, b_+195); H = D;
  CYC(b_+195, b_+197); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+197, b_+199); mem_wr(gb, HL, 0x1e);
  RET(b_+199); return;

label_0b_234:
  CYC(b_+200, b_+202); A = hram_rd(gb, R_SVBK);
  CYC(b_+202, b_+203); push_effect(gb, AF); // push af
  CYC(b_+203, b_+205); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+205, b_+206); A = mem_rd(gb, HL);
  CYC(b_+206, b_+207); B = A;
  CYC(b_+207, b_+209); A = 0x04; // :w4TileMap
  CYC(b_+209, b_+211); hram_wr(gb, R_SVBK, A);
  CYC(b_+211, b_+212); A = B;
  CYC(b_+212, b_+215); SET_HL(w4TileMap);
  CYC(b_+215, b_+216); interactionCodeae_addDoubleIndex(gb, b_+216);
  CYC(b_+216, b_+218); B = 0x30;

loop:
  CYC(b_+218, b_+219); alu_xor(gb, A);
  CYC(b_+219, b_+220); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+220, b_+221); mem_wr(gb, HL, A);
  CYC(b_+221, b_+223); A = 0x1f;
  CYC(b_+223, b_+224); interactionCodeae_addAToHl(gb, b_+224);
  CYC(b_+224, b_+225); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+225, b_+227); goto loop; } // jr nz
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+228); push_effect(gb, DE); // push de
  CYC(b_+228, b_+230); A = 0x09; // UNCMP_GFXH_09
  CALL_C(b_+230, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+233);
  CYC(b_+233, b_+234); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+234, b_+235); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+235, b_+237); hram_wr(gb, R_SVBK, A);
  CYC(b_+237, b_+238); H = D;
  CYC(b_+238, b_+240); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+240, b_+241); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+241); return;

subsubstate2:
  CALL_C(b_+242, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+245);
  if (!(F & FZ)) { CYCT(b_+245, b_+246); ret_effect(gb); return; } // ret nz
  CYC(b_+245, b_+246);
  CYC(b_+246, b_+248); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+248, b_+249); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+249, b_+251); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+251, b_+253); mem_wr(gb, HL, 0x10);
  RET(b_+253); return;

subsubstate3:
  CYC(b_+254, b_+257); A = mem_rd(gb, wFrameCounter);
  CYC(b_+257, b_+259); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+259, b_+260); ret_effect(gb); return; } // ret nz
  CYC(b_+259, b_+260);
  CALL_C(b_+260, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+263);
  if (!(F & FZ)) { CYCT(b_+263, b_+265); goto label_0b_236; } // jr nz
  CYC(b_+263, b_+265);
  CYC(b_+265, b_+266); alu_xor(gb, A);
  CYC(b_+266, b_+268); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+268, b_+269); mem_wr(gb, HL, A);
  CYC(b_+269, b_+271); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+271, b_+272); mem_wr(gb, HL, A);
  CYC(b_+272, b_+275); goto func_6457; // jp

label_0b_236:
  CYC(b_+275, b_+276); push_effect(gb, DE); // push de
  CYC(b_+276, b_+278); A = hram_rd(gb, R_SVBK);
  CYC(b_+278, b_+279); push_effect(gb, AF); // push af
  CYC(b_+279, b_+280); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+280, b_+281); B = A;
  CYC(b_+281, b_+282); A = B;
  CYC(b_+282, b_+285); SET_HL(w4TileMap);
  CYC(b_+285, b_+286); interactionCodeae_addDoubleIndex(gb, b_+286);
  CYC(b_+286, b_+287); A = B;
  CYC(b_+287, b_+290); SET_DE(w3VramTiles);
  CALL_C(b_+290, addDoubleIndexToDe_hook, 0x0072, b_+293);
  CYC(b_+293, b_+295); B = 0x30;

tileLoop:
  CYC(b_+295, b_+296); push_effect(gb, BC); // push bc
  CYC(b_+296, b_+298); A = 0x03; // :w3VramTiles
  CYC(b_+298, b_+300); hram_wr(gb, R_SVBK, A);
  CYC(b_+300, b_+301); A = mem_rd(gb, DE);
  CYC(b_+301, b_+302); B = A;
  CYC(b_+302, b_+303); SET_DE(DE + 1); // inc de
  CYC(b_+303, b_+304); A = mem_rd(gb, DE);
  CYC(b_+304, b_+305); C = A;
  CYC(b_+305, b_+307); A = 0x04; // :w4TileMap
  CYC(b_+307, b_+309); hram_wr(gb, R_SVBK, A);
  CYC(b_+309, b_+310); mem_wr(gb, HL, B);
  CYC(b_+310, b_+311); SET_HL(HL + 1); // inc hl
  CYC(b_+311, b_+312); mem_wr(gb, HL, C);
  CYC(b_+312, b_+314); A = 0x1f;
  CYC(b_+314, b_+315); C = A;
  CYC(b_+315, b_+316); interactionCodeae_addAToHl(gb, b_+316);
  CYC(b_+316, b_+317); A = C;
  CALL_C(b_+317, addAToDe_hook, 0x0068, b_+320);
  CYC(b_+320, b_+321); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+321, b_+322); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+322, b_+324); goto tileLoop; } // jr nz
  CYC(b_+322, b_+324);
  CYC(b_+324, b_+326); A = 0x09; // UNCMP_GFXH_09
  CALL_C(b_+326, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+329);
  CYC(b_+329, b_+330); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+330, b_+332); hram_wr(gb, R_SVBK, A);
  CYC(b_+332, b_+333); SET_DE(pop_effect(gb)); // pop de
  RET(b_+333); return;

substate2:
  CALL_C(b_+334, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+337);
  if (!(F & FZ)) { CYCT(b_+337, b_+338); ret_effect(gb); return; } // ret nz
  CYC(b_+337, b_+338);
  CYC(b_+338, b_+341); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+341, b_+343); mem_wr(gb, HL, 0xff);
  CYC(b_+343, SYM(creditsTextHorizontal_6537)); interactionDelete_hook(gb); return; // jp
}
