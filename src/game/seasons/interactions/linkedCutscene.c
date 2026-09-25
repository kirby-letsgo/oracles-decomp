#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/linkedCutscene.s.
// INTERAC_LINKED_CUTSCENE

static uint16_t linkedCutscene_jump_table(GB *gb) {
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

static void linkedCutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_LINKED_CUTSCENE
void s_interactionCodeb3_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkedCutscene_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+144) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (linkedCutscene_jump_table(gb));
    if (jt_ == b_+22) goto subid0;
    if (jt_ == b_+38) goto subid1;
    if (jt_ == b_+62) goto subid2;
    if (jt_ == b_+78) goto subid3;
    if (jt_ == b_+102) goto subid4;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+22, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); TAIL(interactionDelete); }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); A = 0x1c;
  CALL_C(b_+30, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+33);
  if (!(F & FZ)) { CYCT(b_+33, b_+36); TAIL(interactionDelete); }
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+38);
  goto postCutscene;
subid1:
  CALL_C(b_+38, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+41);
  if (!(F & FZ)) { CYCT(b_+41, b_+44); TAIL(interactionDelete); }
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+46); A = 0x1d;
  CALL_C(b_+46, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+52); TAIL(interactionDelete); }
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+54); A = 0x36;
  CALL_C(b_+54, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+57);
  if (!(F & FC)) { CYCT(b_+57, b_+60); TAIL(interactionDelete); }
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+62);
  goto postCutscene;
subid2:
  CALL_C(b_+62, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+65);
  if (F & FZ) { CYCT(b_+65, b_+68); TAIL(interactionDelete); }
  CYC(b_+65, b_+68);
  CYC(b_+68, b_+70); A = 0x1e;
  CALL_C(b_+70, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+73);
  if (!(F & FZ)) { CYCT(b_+73, b_+76); TAIL(interactionDelete); }
  CYC(b_+73, b_+76);
  CYC(b_+76, b_+78);
  goto postCutscene;
subid3:
  CALL_C(b_+78, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+81);
  if (F & FZ) { CYCT(b_+81, b_+84); TAIL(interactionDelete); }
  CYC(b_+81, b_+84);
  CYC(b_+84, b_+86); A = 0x1f;
  CALL_C(b_+86, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+89);
  if (!(F & FZ)) { CYCT(b_+89, b_+92); TAIL(interactionDelete); }
  CYC(b_+89, b_+92);
  CYC(b_+92, b_+94); A = 0x36;
  CALL_C(b_+94, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+97);
  if (!(F & FC)) { CYCT(b_+97, b_+100); TAIL(interactionDelete); }
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+102);
  goto postCutscene;
subid4:
  CALL_C(b_+102, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+105);
  if (F & FZ) { CYCT(b_+105, b_+108); TAIL(interactionDelete); }
  CYC(b_+105, b_+108);
  CYC(b_+108, b_+110); A = 0x20;
  CALL_C(b_+110, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+113);
  if (!(F & FZ)) { CYCT(b_+113, b_+116); TAIL(interactionDelete); }
  CYC(b_+113, b_+116);
postCutscene:
  CYC(b_+116, b_+117); H = D;
  CYC(b_+117, b_+119); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x01);
  CYC(b_+121, b_+124); SET_HL(wTmpcfc0);
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0x00);
  CYC(b_+126, b_+128); A = 0x50;
  CALL_C(b_+128, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+131);
  CYC(b_+131, b_+133); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+137); SET_HL(b_+151 /* @scriptTable */);
  CYC(b_+137, b_+138); linkedCutscene_add_double_index(gb, b_+138);
  CYC(b_+138, b_+139); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+139, b_+140); H = mem_rd(gb, HL);
  CYC(b_+140, b_+141); L = A;
  CALL_C(b_+141, s_interactionSetScript, SYM(interactionSetScript), b_+144);
state1:
  CALL_C(b_+144, s_interactionRunScript, SYM(interactionRunScript), b_+147);
  if (F & FC) { CYCT(b_+147, b_+150); TAIL(interactionDelete); }
  CYC(b_+147, b_+150);
  RET(b_+150); return;
}

