#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/88.s, bank $09.

// Stores the script pointer hl in var18/19 and loads counter1/counter2 from its first two
// bytes; a first byte of $ff hands over to seasonsFunc_09_7f17.
void s_seasonsFunc_09_7f01_hook(GB *gb) {
  BASE(seasonsFunc_09_7f01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x58; // Interaction.var18
  CYC(b_+2, b_+3); A = H;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = L;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); TAIL_S(seasonsFunc_09_7f17); }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); E = 0x46; // Interaction.counter1
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+19); E = 0x70; // Interaction.var30
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return;
}

static void i88_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void i88_d_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_88
void s_interactionCode88_hook(GB *gb) {
  BASE(interactionCode88);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto nonZeroState; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+18); alu_or(gb, 0x80);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+22);
  CALL_C(b_+22, s_objectSetVisible82, SYM(objectSetVisible82), b_+25);
  CALL_C(b_+25, s_objectSetInvisible, SYM(objectSetInvisible), b_+28);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+32, b_+34); goto L_7e4d; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+37); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+37, b_+38); alu_cpl(gb);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
L_7e4d:
  CYC(b_+39, b_+41); alu_add(gb, 0x28);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_7e61; }
  CYC(b_+48, b_+50);
  CALL_C(b_+50, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+53);
  CYC(b_+53, b_+56); SET_HL(SYM(seasonsTable_09_7f33));
  CYC(b_+56, b_+59);
  TAIL_S(seasonsFunc_09_7f01);
L_7e61:
  CYC(b_+59, b_+61); A = 0x30;
  CALL_C(b_+61, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+67); TAIL(interactionDelete); }
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+69, b_+71); A = 0x3c;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  RET(b_+72); return;
nonZeroState:
  CYC(b_+73, b_+75); A = 0x0a;
  CALL_C(b_+75, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+80); goto L_7e7d; }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+83); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+84, b_+87); TAIL(interactionDelete); }
  CYC(b_+84, b_+87);
L_7e7d:
  CALL_C(b_+87, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+90);
  if (!(F & FZ)) { CYCT(b_+90, b_+92); goto L_7ea2; }
  CYC(b_+90, b_+92);
  CALL_C(b_+92, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+95);
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+100, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+103);
  CYC(b_+103, b_+105); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+105); return; }
  CYC(b_+105, b_+106);
  CALL_C(b_+106, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+109);
  CALL_C(b_+109, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+112);
  CYC(b_+112, b_+114); alu_and(gb, 0x03);
  CYC(b_+114, b_+117); SET_HL(SYM(seasonsTable_09_7f2b));
  CYC(b_+117, b_+118); i88_d_add_double_index(gb, b_+118);
  CYC(b_+118, b_+119); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+119, b_+120); H = mem_rd(gb, HL);
  CYC(b_+120, b_+121); L = A;
  CYC(b_+121, b_+124);
  TAIL_S(seasonsFunc_09_7f01);
L_7ea2:
  CYC(b_+124, b_+126); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+128); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); TAIL_S(seasonsFunc_09_7ee2); }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+132); A = 0x01;
  CYC(b_+132, b_+133); mem_wr(gb, DE, A);
  CYC(b_+133, b_+135); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+135, b_+136); A = mem_rd(gb, DE);
  CYC(b_+136, b_+139); SET_HL(SYM(seasonsTable_09_7f28));
  CYC(b_+139, b_+140); i88_d_add_a_to_hl(gb, b_+140);
  CYC(b_+140, b_+141); A = mem_rd(gb, HL);
  CALL_C(b_+141, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+144);
  CYC(b_+144, b_+146); A = 0xff;
  CYC(b_+146, b_+149); mem_wr(gb, wLoadedTilesetPalette, A);
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+151); alu_or(gb, A);
  CYC(b_+151, b_+153); A = 0xd2;
  if (!(F & FZ)) CALL_C_CC(b_+153, s_playSound, SYM(playSound_b00), b_+156);
  else CYC(b_+153, b_+156);
  CYC(b_+156, b_+157); A = mem_rd(gb, DE);
  CYC(b_+157, b_+159); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+159, b_+161); goto L_7ecc; }
  CYC(b_+159, b_+161);
  CALL_C(b_+161, s_objectSetInvisible, SYM(objectSetInvisible), b_+164);
  CYC(b_+164, b_+166);
  TAIL_S(seasonsFunc_09_7ee2);
L_7ecc:
  CALL_C(b_+166, s_getRandomNumber, SYM(getRandomNumber), b_+169);
  CYC(b_+169, b_+171); alu_and(gb, 0x01);
  CYC(b_+171, b_+172); B = A;
  CYC(b_+172, b_+174); A = 0x13;
  if (F & FZ) { CYCT(b_+174, b_+176); goto L_7ed8; }
  CYC(b_+174, b_+176);
  CYC(b_+176, b_+178); A = 0x8d;
L_7ed8:
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+180, b_+181); mem_wr(gb, DE, A);
  CYC(b_+181, b_+182); A = B;
  CALL_C(b_+182, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+185);
  CALL_C(b_+185, s_objectSetVisible, SYM(objectSetVisible), b_+188);
  s_seasonsFunc_09_7ee2_hook(gb); return; // falls through
}

void s_seasonsFunc_09_7ee2_hook(GB *gb) {
  BASE(seasonsFunc_09_7ee2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_7ef5; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_interactionAnimate, SYM(interactionAnimate), b_+10);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto L_7ef5; }
  CYC(b_+14, b_+16);
  CALL_C(b_+16, s_objectSetInvisible, SYM(objectSetInvisible), b_+19);
L_7ef5:
  CALL_C(b_+19, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+22);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); L = mem_rd(gb, HL);
  CYC(b_+28, b_+29); H = A;
  CYC(b_+29, b_+30); SET_HL(HL + 1);
  CYC(b_+30, b_+31); SET_HL(HL + 1);
  s_seasonsFunc_09_7f01_hook(gb); return; // falls through
}

void s_seasonsFunc_09_7f17_hook(GB *gb) {
  BASE(seasonsFunc_09_7f17);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+8); TAIL(interactionDelete); }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x3c);
  RET(b_+16); return;
}

