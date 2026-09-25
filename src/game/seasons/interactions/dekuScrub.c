#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/dekuScrub.s.
// INTERAC_DEKU_SCRUB

static uint16_t dekuScrub_jump_table(GB *gb) {
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

static void dekuScrub_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_DEKU_SCRUB
void s_interactionCoded6_hook(GB *gb) {
  BASE(interactionCoded6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dekuScrub_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+80) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+17); SET_HL(b_+77 /* @table_7f74 */);
  CYC(b_+17, b_+18); dekuScrub_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+21, b_+22); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+22, b_+25); TAIL(interactionDelete); }
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+27); A = 0x86;
  CALL_C(b_+27, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+30);
  CALL_C(b_+30, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+33);
  CALL_C(b_+33, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+36);
  CYC(b_+36, b_+38); A = 0x4c;
  CALL_C(b_+38, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+41);
  CYC(b_+41, b_+43); A = 0x28;
  CALL_C(b_+43, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+46);
  CYC(b_+46, b_+49); SET_HL(SYM(dekuScrubScript_notFinishedGame));
  if (F & FZ) { CYCT(b_+49, b_+51); goto setScript; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); A = 0x61;
  CALL_C(b_+53, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+56);
  CYC(b_+56, b_+59); SET_HL(SYM(dekuScrubScript_doneSecret));
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto setScript; }
  CYC(b_+59, b_+61);
  CALL_C(b_+61, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+64);
  CYC(b_+64, b_+66); alu_bit(gb, 7, A);
  CYC(b_+66, b_+69); SET_HL((SYM(interactionCoded5) + 120) /* @substate1 */);
  if (!(F & FZ)) { CYCT(b_+69, b_+71); goto setScript; }
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+74); SET_HL(SYM(dekuScrubScript_beginningSecret));
setScript:
  CYC(b_+74, b_+77);
  TAIL(interactionSetScript);

state1:
  CALL_C(b_+80, s_interactionRunScript, SYM(interactionRunScript), b_+83);
  CALL_C(b_+83, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+86);
  CYC(b_+86, b_+88); C = 0x20;
  CALL_C(b_+88, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+91);
  CYC(b_+91, b_+92); H = D;
  CYC(b_+92, b_+94); L = INTERACTION_BASE + OBJ_VAR37;
  if (F & FC) { CYCT(b_+94, b_+96); goto L_7f91; }
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+98); return; }
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  CYC(b_+101, b_+103); A = 0x03;
  CYC(b_+103, b_+106);
  TAIL(interactionSetAnimation);
L_7f91:
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+108); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+108); return; }
  CYC(b_+108, b_+109);
  CYC(b_+109, b_+110); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+110, b_+112); A = 0x01;
  CYC(b_+112, b_+115);
  TAIL(interactionSetAnimation);
}
