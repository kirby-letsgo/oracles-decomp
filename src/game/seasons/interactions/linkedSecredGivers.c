#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/linkedSecredGivers.s.
// INTERAC_LINKED_SECRET_GIVERS

static uint16_t linkedSecredGivers_jump_table(GB *gb) {
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

static void linkedSecredGivers_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCodedb@func_662f
static void linkedSecredGivers_func_662f(GB *gb) {
  BASE(interactionCodedb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+53); push_effect(gb, b_+53);
  do { uint16_t jt_ = (linkedSecredGivers_jump_table(gb));
    if (jt_ == b_+59) goto subid0;
    if (jt_ == b_+65) goto subid1;
    if (jt_ == b_+75) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+59, b_+61); A = 0x2d;
  CALL_C(b_+61, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+64);
  RET(b_+64); return;
subid1:
  CYC(b_+65, b_+67); A = 0x00;
  CYC(b_+67, b_+69); B = 0x81;
  CALL_C(b_+69, s_getRoomFlags, SYM(getRoomFlags), b_+72);
  CYC(b_+72, b_+74); alu_bit(gb, 7, A);
  RET(b_+74); return;
subid2:
  CYC(b_+75, b_+77); A = 0x40;
  CALL_C(b_+77, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+80);
  if (!(F & FC)) { CYCT(b_+80, b_+82); goto L_665a; }
  CYC(b_+80, b_+82);
  CALL_C(b_+82, s_getHighestSetBit, SYM(getHighestSetBit), b_+85);
  CYC(b_+85, b_+87); alu_cp(gb, 0x01);
  if (F & FC) { CYCT(b_+87, b_+89); goto L_665a; }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); alu_or(gb, 0x01);
  RET(b_+91); return;
L_665a:
  CYC(b_+92, b_+93); alu_xor(gb, A);
  RET(b_+93); return;
}

// INTERAC_LINKED_SECRET_GIVERS
void s_interactionCodedb_hook(GB *gb) {
  BASE(interactionCodedb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkedSecredGivers_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+94) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+14);
  if (F & FZ) { CYCT(b_+14, b_+17); TAIL(interactionDelete); }
  CYC(b_+14, b_+17);
  CALL_L(b_+17, linkedSecredGivers_func_662f, b_+20);
  if (F & FZ) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+29); SET_HL(b_+46 /* @table_662c */);
  CYC(b_+29, b_+30); linkedSecredGivers_add_a_to_hl(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+37); SET_HL(SYM(linkedGameNpcScript));
  CALL_C(b_+37, s_interactionSetScript, SYM(interactionSetScript), b_+40);
  CALL_C(b_+40, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+43);
  CYC(b_+43, b_+46);
  TAIL(objectSetVisiblec2);

state1:
  CALL_C(b_+94, s_interactionRunScript, SYM(interactionRunScript), b_+97);
  CYC(b_+97, b_+99); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+99, b_+100); A = mem_rd(gb, DE);
  CYC(b_+100, b_+101); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+101, b_+104); TAIL(npcFaceLinkAndAnimate); }
  CYC(b_+101, b_+104);
  CYC(b_+104, b_+107);
  TAIL(interactionAnimateAsNpc);
}

