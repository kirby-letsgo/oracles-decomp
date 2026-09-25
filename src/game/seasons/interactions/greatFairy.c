#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/greatFairy.s.
// INTERAC_GREAT_FAIRY

static uint16_t greatFairy_jump_table(GB *gb) {
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

static void greatFairy_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_GREAT_FAIRY
void s_interactionCoded5_hook(GB *gb) {
  BASE(interactionCoded5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (greatFairy_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+67) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+15, b_+17); goto subid0; }
  CYC(b_+15, b_+17);
  CALL_C(b_+17, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+25);
  goto subid1;
subid0:
  CYC(b_+25, b_+27); A = 0x28;
  CALL_C(b_+27, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+30);
  if (F & FZ) { CYCT(b_+30, b_+33); TAIL(interactionDelete); }
  CYC(b_+30, b_+33);
subid1:
  CALL_C(b_+33, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+36);
  CALL_C(b_+36, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+39);
  CYC(b_+39, b_+40); H = D;
  CYC(b_+40, b_+41); L = E;
  CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0xf0);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x36);
  CYC(b_+50, b_+52); A = 0x41;
  CALL_C(b_+52, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+55);
  CYC(b_+55, b_+56); alu_xor(gb, A);
  CYC(b_+56, b_+59); mem_wr(gb, wActiveMusic, A);
  CYC(b_+59, b_+61); A = 0x0f;
  CALL_C(b_+61, s_playSound, SYM(playSound_b00), b_+64);
  CYC(b_+64, b_+67);
  TAIL(objectCreatePuff);
state1:
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); push_effect(gb, b_+71);
  do { uint16_t jt_ = (greatFairy_jump_table(gb));
    if (jt_ == b_+75) goto substate0;
    if (jt_ == b_+120) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+78); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+78, b_+79); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+82, b_+83); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+83, b_+84); alu_xor(gb, A);
  CALL_C(b_+84, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+87);
  CALL_C(b_+87, s_objectSetVisiblec2, SYM(objectSetVisiblec2), b_+90);
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+92, b_+94); A = 0x07;
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  CYC(b_+95, b_+97); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+99); alu_or(gb, A);
  CYC(b_+99, b_+102); SET_HL((SYM(ganon_decideNextMove) + 43) /* @choice0 */);
  if (!(F & FZ)) { CYCT(b_+102, b_+104); goto setScript; }
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); A = 0x60;
  CALL_C(b_+106, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+109);
  CYC(b_+109, b_+112); SET_HL(SYM(templeGreatFairyScript_beginningSecret));
  if (F & FZ) { CYCT(b_+112, b_+114); goto setScript; }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+117); SET_HL(b_+16);
setScript:
  CYC(b_+117, b_+120);
  TAIL(interactionSetScript);
substate1:
  CALL_C(b_+120, s_interactionRunScript, SYM(interactionRunScript), b_+123);
  CALL_C(b_+123, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+126);
  CYC(b_+126, b_+129); A = mem_rd(gb, wFrameCounter);
  CYC(b_+129, b_+131); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+131); return; }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+135); A = mem_rd(gb, wFrameCounter);
  CYC(b_+135, b_+137); alu_and(gb, 0x38);
  CYC(b_+137, b_+139); A = alu_swap(gb, A);
  CYC(b_+139, b_+140); alu_rlca(gb);
  CYC(b_+140, b_+143); SET_HL(SYM(table_7f1f));
  CYC(b_+143, b_+144); greatFairy_add_a_to_hl(gb, b_+144);
  CYC(b_+144, b_+146); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+146, b_+147); A = mem_rd(gb, DE);
  CYC(b_+147, b_+148); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+148, b_+149); mem_wr(gb, DE, A);
  RET(b_+149); return;
}
