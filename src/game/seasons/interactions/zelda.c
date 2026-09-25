#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/zelda.s.
// INTERAC_ZELDA

static uint16_t zelda_jump_table(GB *gb) {
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

static void zelda_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_ZELDA
void s_interactionCode44_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (zelda_jump_table(gb));
    if (jt_ == SYM(zelda_state0) && hook_is(gb, SYM(zelda_state0), s_zelda_state0_hook)) { s_zelda_state0_hook(gb); return; }
    if (jt_ == SYM(zelda_state1) && hook_is(gb, SYM(zelda_state1), s_zelda_state1_hook)) { s_zelda_state1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_zelda_state0_hook(GB *gb) {
  BASE(zelda_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+10); SET_HL(SYM(table_6ea3));
  CYC(b_+10, b_+11); zelda_add_double_index(gb, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); H = mem_rd(gb, HL);
  CYC(b_+13, b_+14); L = A;
  CALL_C(b_+14, s_interactionSetScript, SYM(interactionSetScript), b_+17);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+19, b_+21); goto subid0; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+23, b_+25); goto subid8; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+27, b_+29); goto subid9; }
  CYC(b_+27, b_+29);
setVisibleInitGraphicsIncState:
  CALL_C(b_+29, s_objectSetVisible82, SYM(objectSetVisible82), b_+32);
  CALL_C(b_+32, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+35);
  CYC(b_+35, b_+37);
  TAIL_S(zelda_state1);
subid0:
  CYC(b_+37, b_+39); A = 0xb0;
  CYC(b_+39, b_+42); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  CYC(b_+42, b_+45); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CALL_C(b_+45, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+48);
  CYC(b_+48, b_+50); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+50, b_+52); goto L_6e3c; }
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+54); A = 0x01;
  CYC(b_+54, b_+57); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+57, b_+60); A = mem_rd(gb, wActiveMusic);
  CYC(b_+60, b_+61); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+61, b_+63); goto L_6e3c; }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); alu_xor(gb, A);
  CYC(b_+64, b_+67); mem_wr(gb, wActiveMusic, A);
  CYC(b_+67, b_+69); A = 0x38;
  CALL_C(b_+69, s_playSound, SYM(playSound_b00), b_+72);
L_6e3c:
  CYC(b_+72, b_+75); SET_HL(wTmpcbb3);
  CYC(b_+75, b_+77); B = 0x10;
  CALL_C(b_+77, s_clearMemory, SYM(clearMemory), b_+80);
  CYC(b_+80, b_+82);
  goto setVisibleInitGraphicsIncState;
subid8:
  CALL_C(b_+82, s_checkGotMakuSeedDidNotSeeZeldaKidnapped_hook, SYM(checkGotMakuSeedDidNotSeeZeldaKidnapped), b_+85);
  CYC(b_+85, b_+87); alu_bit(gb, 7, C);
  if (!(F & FZ)) { CYCT(b_+87, b_+90); TAIL(interactionDelete); }
  CYC(b_+87, b_+90);
  CYC(b_+90, b_+92);
  goto setVisibleInitGraphicsIncState;
subid9:
  CYC(b_+92, b_+94); A = 0x23;
  CALL_C(b_+94, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+97);
  if (F & FZ) { CYCT(b_+97, b_+100); TAIL(interactionDelete); }
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+102); A = 0x1e;
  CALL_C(b_+102, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+105);
  if (!(F & FZ)) { CYCT(b_+105, b_+108); TAIL(interactionDelete); }
  CYC(b_+105, b_+108);
  CYC(b_+108, b_+110);
  goto setVisibleInitGraphicsIncState;
}

void s_zelda_state1_hook(GB *gb) {
  BASE(zelda_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (zelda_jump_table(gb));
    if (jt_ == b_+24) goto animateAndRunScript;
    if (jt_ == b_+30) goto runSubid5;
    if (jt_ == b_+41) goto runSubid6;
    if (jt_ == b_+50) goto faceLinkAndRunScript;
    if (jt_ == b_+56) goto runSubid8;
    HANDOFF(HL);
  } while (0);
animateAndRunScript:
  CALL_C(b_+24, s_interactionRunScript, SYM(interactionRunScript), b_+27);
  CYC(b_+27, b_+30);
  TAIL(interactionAnimate);
runSubid5:
  CALL_C(b_+30, s_interactionRunScript, SYM(interactionRunScript), b_+33);
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+37, b_+40); TAIL(interactionAnimate); }
  CYC(b_+37, b_+40);
  RET(b_+40); return;
runSubid6:
  CALL_C(b_+41, s_interactionRunScript, SYM(interactionRunScript), b_+44);
  if (F & FC) { CYCT(b_+44, b_+47); TAIL(interactionDelete); }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+50);
  TAIL(interactionAnimate);
faceLinkAndRunScript:
  CALL_C(b_+50, s_interactionRunScript, SYM(interactionRunScript), b_+53);
  CYC(b_+53, b_+56);
  TAIL(npcFaceLinkAndAnimate);
runSubid8:
  CYC(b_+56, b_+58); A = 0x26;
  CALL_C(b_+58, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+61);
  if (!(F & FZ)) { CYCT(b_+61, b_+63); goto faceLinkAndRunScript; }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65);
  goto animateAndRunScript;
}

