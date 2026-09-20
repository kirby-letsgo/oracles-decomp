#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  CYC(0x4121, 0x4124); A = mem_rd(gb, 0xcc39);
  CYC(0x4124, 0x4125); B = A;
  CYC(0x4125, 0x4126); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4126, 0x4128);
    goto inDungeon;
  }
  CYC(0x4126, 0x4128);
  CYC(0x4128, 0x412b); SET_HL(0x41be);
  CYC(0x412b, 0x412d);
  goto loadScript;

inDungeon:
  CYC(0x412d, 0x412e); A = B;
  CYC(0x412e, 0x4131); SET_HL(0x41be);
  CYC(0x4131, 0x4132); movingPlatform_addDoubleIndexToHl_from_rst(gb, 0x4132);
  CYC(0x4132, 0x4133); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4133, 0x4134); H = mem_rd(gb, HL);
  CYC(0x4134, 0x4135); L = A;

loadScript:
  CYC(0x4135, 0x4137); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x4137, 0x4138); A = mem_rd(gb, DE);
  CYC(0x4138, 0x4139); movingPlatform_addDoubleIndexToHl_from_rst(gb, 0x4139);
  CYC(0x4139, 0x413a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x413a, 0x413b); H = mem_rd(gb, HL);
  CYC(0x413b, 0x413c); L = A;
  CYC(0x413c, 0x413e);
  movingPlatform_setScript_hook(gb);
}

void movingPlatform_runScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x413e, 0x4140); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(0x4140, 0x4141); A = mem_rd(gb, DE);
  CYC(0x4141, 0x4142); L = A;
  CYC(0x4142, 0x4143); E = alu_inc8(gb, E);
  CYC(0x4143, 0x4144); A = mem_rd(gb, DE);
  CYC(0x4144, 0x4145); H = A;

nextOpcode:
  CYC(0x4145, 0x4146); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4146, 0x4147); push_effect(gb, HL);
  CYC(0x4147, 0x4148); push_effect(gb, 0x4148);
  switch (movingPlatform_jumpTable(gb)) {
    case 0x4160: goto opcode00;
    case 0x416b: goto opcode01;
    case 0x4177: goto opcode02;
    case 0x417e: goto opcode03;
    case 0x4185: goto opcode04;
    case 0x418d: goto opcode05;
    case 0x41a3: goto opcode08;
    case 0x41a7: goto opcode09;
    case 0x41ab: goto opcode0a;
    case 0x41af: goto opcode0b;
    default: HANDOFF(HL);
  }

opcode00:
  CYC(0x4160, 0x4161); SET_HL(pop_effect(gb));
  CYC(0x4161, 0x4162); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4162, 0x4164); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4164, 0x4165); mem_wr(gb, DE, A);
  CYC(0x4165, 0x4167); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4167, 0x4168); alu_xor(gb, A);
  CYC(0x4168, 0x4169); mem_wr(gb, DE, A);
  CYC(0x4169, 0x416b);
  movingPlatform_setScript_hook(gb);
  return;

opcode01:
  CYC(0x416b, 0x416c); SET_HL(pop_effect(gb));
  CYC(0x416c, 0x416d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x416d, 0x416f); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x416f, 0x4170); mem_wr(gb, DE, A);
  CYC(0x4170, 0x4172); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x4172, 0x4174); A = 0x01;
  CYC(0x4174, 0x4175); mem_wr(gb, DE, A);
  CYC(0x4175, 0x4177);
  movingPlatform_setScript_hook(gb);
  return;

opcode02:
  CYC(0x4177, 0x4178); SET_HL(pop_effect(gb));
  CYC(0x4178, 0x4179); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4179, 0x417b); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x417b, 0x417c); mem_wr(gb, DE, A);
  CYC(0x417c, 0x417e);
  goto nextOpcode;

opcode03:
  CYC(0x417e, 0x417f); SET_HL(pop_effect(gb));
  CYC(0x417f, 0x4180); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4180, 0x4182); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4182, 0x4183); mem_wr(gb, DE, A);
  CYC(0x4183, 0x4185);
  goto nextOpcode;

opcode04:
  CYC(0x4185, 0x4186); SET_HL(pop_effect(gb));
  CYC(0x4186, 0x4187); A = mem_rd(gb, HL);
  CALL_C(0x4187, s8ToS16_hook, 0x01cd, 0x418a);
  CYC(0x418a, 0x418b); alu_add_hl(gb, BC);
  CYC(0x418b, 0x418d);
  goto nextOpcode;

opcode05:
  CYC(0x418d, 0x418e); SET_HL(pop_effect(gb));
  CYC(0x418e, 0x4191); A = mem_rd(gb, 0xcc96);
  CYC(0x4191, 0x4192); alu_cp(gb, D);
  if (!(F & FZ)) {
    CYCT(0x4192, 0x4194);
    goto opcode05_linkNotOn;
  }
  CYC(0x4192, 0x4194);
  CYC(0x4194, 0x4195); SET_HL(HL + 1);
  CYC(0x4195, 0x4197);
  goto nextOpcode;

opcode05_linkNotOn:
  CYC(0x4197, 0x4198); SET_HL(HL - 1);
  CYC(0x4198, 0x419a); A = 0x01;
  CYC(0x419a, 0x419c); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x419c, 0x419d); mem_wr(gb, DE, A);
  CYC(0x419d, 0x419e); alu_xor(gb, A);
  CYC(0x419e, 0x41a0); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x41a0, 0x41a1); mem_wr(gb, DE, A);
  CYC(0x41a1, 0x41a3);
  movingPlatform_setScript_hook(gb);
  return;

opcode08:
  CYC(0x41a3, 0x41a5); A = 0x00;
  CYC(0x41a5, 0x41a7);
  goto moveAtAngle;

opcode09:
  CYC(0x41a7, 0x41a9); A = 0x08;
  CYC(0x41a9, 0x41ab);
  goto moveAtAngle;

opcode0a:
  CYC(0x41ab, 0x41ad); A = 0x10;
  CYC(0x41ad, 0x41af);
  goto moveAtAngle;

opcode0b:
  CYC(0x41af, 0x41b1); A = 0x18;

moveAtAngle:
  CYC(0x41b1, 0x41b3); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x41b3, 0x41b4); mem_wr(gb, DE, A);
  CYC(0x41b4, 0x41b6);
  goto opcode01;
}

void movingPlatform_setScript_hook(GB *gb) {
  CYC(0x41b6, 0x41b8); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(0x41b8, 0x41b9); A = L;
  CYC(0x41b9, 0x41ba); mem_wr(gb, DE, A);
  CYC(0x41ba, 0x41bb); E = alu_inc8(gb, E);
  CYC(0x41bb, 0x41bc); A = H;
  CYC(0x41bc, 0x41bd); mem_wr(gb, DE, A);
  RET(0x41bd);
}
