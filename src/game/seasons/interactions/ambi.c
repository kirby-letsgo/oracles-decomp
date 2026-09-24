#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/ambi.s.
// INTERAC_AMBI

static uint16_t ambi_jump_table(GB *gb) {
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

static void ambi_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void ambi_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_AMBI
void s_interactionCodeb8_hook(GB *gb) {
  BASE(interactionCodeb8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+85) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+11);
  if (F & FZ) { CYCT(b_+11, b_+14); TAIL(interactionDelete); }
  CYC(b_+11, b_+14);
  CALL_C(b_+14, s_func_740a_hook, SYM(func_740a), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+22); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+22, b_+24); goto subid3; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); SET_HL(b_+80 /* @var3eVals */);
  CYC(b_+27, b_+28); ambi_add_a_to_hl(gb, b_+28);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+32, b_+35); TAIL(interactionDelete); }
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+37);
  goto L_73d3;
subid3:
  CYC(b_+37, b_+39); A = 0x13;
  CALL_C(b_+39, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+45); TAIL(interactionDelete); }
  CYC(b_+42, b_+45);
L_73d3:
  CALL_C(b_+45, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+48);
  CALL_C(b_+48, s_interactionIncState, SYM(interactionIncState), b_+51);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+57); SET_HL(SYM(table_7432));
  CYC(b_+57, b_+58); ambi_add_double_index(gb, b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+60); H = mem_rd(gb, HL);
  CYC(b_+60, b_+61); L = A;
  CALL_C(b_+61, s_interactionSetScript, SYM(interactionSetScript), b_+64);
  CYC(b_+64, b_+66); A = 0x3a;
  CALL_C(b_+66, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+69);
  CALL_C(b_+69, s_objectSetVisible80, SYM(objectSetVisible80), b_+72);
  CYC(b_+72, b_+74); A = 0x02;
  CALL_C(b_+74, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+77);
  CYC(b_+77, b_+80);
  goto animate;

state1:
  CALL_C(b_+85, s_interactionRunScript, SYM(interactionRunScript), b_+88);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+92, b_+94); goto animate; }
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+97);
  TAIL(npcFaceLinkAndAnimate);
animate:
  CYC(b_+97, b_+100);
  TAIL(interactionAnimate);
}

// Stores into var3e, in this order:
// $03 - if pirates left for the ship
// $02 - if 5th+ essence gotten
// $01 - if 3rd+ essence gotten
// $00 - otherwise
void s_func_740a_hook(GB *gb) {
  BASE(func_740a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x13;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto piratesLeftForShip; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x40;
  CALL_C(b_+9, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+12);
  if (F & FC) { CYCT(b_+12, b_+14); goto haveEssence; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
haveEssence:
  CYC(b_+15, b_+17); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+17, b_+19); goto atLeast5thEssence; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto atLeast3rdEssence; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+26);
  goto storeIntoVar3e;
atLeast3rdEssence:
  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+30);
  goto storeIntoVar3e;
atLeast5thEssence:
  CYC(b_+30, b_+32); A = 0x02;
  CYC(b_+32, b_+34);
  goto storeIntoVar3e;
piratesLeftForShip:
  CYC(b_+34, b_+36); A = 0x03;
storeIntoVar3e:
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  RET(b_+39); return;
}

