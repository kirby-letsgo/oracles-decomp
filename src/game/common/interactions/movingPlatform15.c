#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void movingPlatform_loadScript_hook(GB *gb);
void movingPlatform_runScript_hook(GB *gb);
void movingPlatform_setScript_hook(GB *gb);

static void movingPlatform_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t movingPlatform_jumpTable(GB *gb) {
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

void movingPlatform_loadScript_hook(GB *gb) {
  BASE(movingPlatform_loadScript);
  CYC(b_+0, b_+3); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto inDungeon;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+10); SET_HL(GV(SYM(movingPlatform_scriptTable), 0x471d));
  CYC(b_+10, b_+12);
  goto loadScript;

inDungeon:
  CYC(b_+12, b_+13); A = B;
  CYC(b_+13, b_+16); SET_HL(SYM(movingPlatform_scriptTable));
  CYC(b_+16, b_+17); movingPlatform_addDoubleIndexToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  CYC(b_+19, b_+20); L = A;

loadScript:
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); movingPlatform_addDoubleIndexToHl_from_rst(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); H = mem_rd(gb, HL);
  CYC(b_+26, b_+27); L = A;
  CYC(b_+27, b_+29);
  movingPlatform_setScript_hook(gb);
}

void movingPlatform_runScript_hook(GB *gb) {
  BASE(movingPlatform_runScript);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); L = A;
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); H = A;

nextOpcode:
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); push_effect(gb, HL);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (movingPlatform_jumpTable(gb));
    if (jt_ == b_+34) { goto opcode00; }
    else if (jt_ == b_+45) { goto opcode01; }
    else if (jt_ == b_+57) { goto opcode02; }
    else if (jt_ == b_+64) { goto opcode03; }
    else if (jt_ == b_+71) { goto opcode04; }
    else if (jt_ == b_+79) { goto opcode05; }
    else if (jt_ == b_+101) { goto opcode08; }
    else if (jt_ == b_+105) { goto opcode09; }
    else if (jt_ == b_+109) { goto opcode0a; }
    else if (jt_ == b_+113) { goto opcode0b; }
    else { HANDOFF(HL); }
  } while (0);

opcode00:
  CYC(b_+34, b_+35); SET_HL(pop_effect(gb));
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+45);
  TAIL(movingPlatform_setScript);

opcode01:
  CYC(b_+45, b_+46); SET_HL(pop_effect(gb));
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+52, b_+54); A = 0x01;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57);
  TAIL(movingPlatform_setScript);

opcode02:
  CYC(b_+57, b_+58); SET_HL(pop_effect(gb));
  CYC(b_+58, b_+59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+64);
  goto nextOpcode;

opcode03:
  CYC(b_+64, b_+65); SET_HL(pop_effect(gb));
  CYC(b_+65, b_+66); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+66, b_+68); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+71);
  goto nextOpcode;

opcode04:
  CYC(b_+71, b_+72); SET_HL(pop_effect(gb));
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CALL_C(b_+73, s8ToS16_hook, SYM(s8ToS16), b_+76);
  CYC(b_+76, b_+77); alu_add_hl(gb, BC);
  CYC(b_+77, b_+79);
  goto nextOpcode;

opcode05:
  CYC(b_+79, b_+80); SET_HL(pop_effect(gb));
  CYC(b_+80, b_+83); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+83, b_+84); alu_cp(gb, D);
  if (!(F & FZ)) {
    CYCT(b_+84, b_+86);
    goto opcode05_linkNotOn;
  }
  CYC(b_+84, b_+86);
  CYC(b_+86, b_+87); SET_HL(HL + 1);
  CYC(b_+87, b_+89);
  goto nextOpcode;

opcode05_linkNotOn:
  CYC(b_+89, b_+90); SET_HL(HL - 1);
  CYC(b_+90, b_+92); A = 0x01;
  CYC(b_+92, b_+94); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  CYC(b_+95, b_+96); alu_xor(gb, A);
  CYC(b_+96, b_+98); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+101);
  TAIL(movingPlatform_setScript);

opcode08:
  CYC(b_+101, b_+103); A = 0x00;
  CYC(b_+103, b_+105);
  goto moveAtAngle;

opcode09:
  CYC(b_+105, b_+107); A = 0x08;
  CYC(b_+107, b_+109);
  goto moveAtAngle;

opcode0a:
  CYC(b_+109, b_+111); A = 0x10;
  CYC(b_+111, b_+113);
  goto moveAtAngle;

opcode0b:
  CYC(b_+113, b_+115); A = 0x18;

moveAtAngle:
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
  CYC(b_+118, b_+120);
  goto opcode01;
}

void movingPlatform_setScript_hook(GB *gb) {
  BASE(movingPlatform_setScript);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(b_+2, b_+3); A = L;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); E = alu_inc8(gb, E);
  CYC(b_+5, b_+6); A = H;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  RET(b_+7);
}
