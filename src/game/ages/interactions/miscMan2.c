#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t bank09_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

void interactionCode44_hook(GB *gb);
void interactionCode44__subid0_hook(GB *gb);
void interactionCode44__subid0__initialized_hook(GB *gb);
void interactionCode44__subid1_hook(GB *gb);
void interactionCode44__afterCall54b3_hook(GB *gb);
void interactionCode44__subid2_hook(GB *gb);
void interactionCode44__subid3__initialized_hook(GB *gb);
void interactionCode44__subid4_hook(GB *gb);
void interactionCode44__subid4__initialized_hook(GB *gb);
void interactionCode44__initGraphicsAndIncState_hook(GB *gb);
void interactionCode44__initGraphicsIncStateAndLoadScript_hook(GB *gb);
void getGameProgress_1_hook(GB *gb);
void getGameProgress_1__noEssences_hook(GB *gb);
void getGameProgress_2_hook(GB *gb);
void getGameProgress_2__noEssences_hook(GB *gb);
void unusedFunc5598_hook(GB *gb);
void checkNpcShouldExistAtGameStage_body_hook(GB *gb);

void interactionCode44_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+14) { goto L_5492; }
    else if (jt_ == b_+39) { goto L_54ab; }
    else if (jt_ == b_+53) { goto L_54b9; }
    else if (jt_ == b_+91) { goto L_54df; }
    else { HANDOFF(HL); }
  } while (0);
L_5492:
  CALL_C(b_+14, checkInteractionState_hook, SYM(checkInteractionState), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_54a2; } CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 0x14;
  CALL_C(b_+21, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+24);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+24, b_+27);
  CALL_L(b_+27, interactionCode44__initGraphicsIncStateAndLoadScript_hook, b_+30);
L_54a2:
  CALL_C(b_+30, interactionRunScript_hook, SYM(interactionRunScript), b_+33);
  if ((F & FC)) { CYCT(b_+33, b_+36); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+33, b_+36);
  CYC(b_+36, b_+39); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
L_54ab:
  CALL_C(b_+39, checkInteractionState_hook, SYM(checkInteractionState), b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto L_54b3; } CYC(b_+42, b_+44);
  CALL_L(b_+44, interactionCode44__initGraphicsIncStateAndLoadScript_hook, b_+47);
L_54b3:
  CALL_C(b_+47, interactionRunScript_hook, SYM(interactionRunScript), b_+50);
  CYC(b_+50, b_+53); if (hook_enabled_at(gb, SYM(interactionAnimateBasedOnSpeed))) { interactionAnimateBasedOnSpeed_hook(gb); return; } HANDOFF(SYM(interactionAnimateBasedOnSpeed));
L_54b9:
  CALL_C(b_+53, checkInteractionState_hook, SYM(checkInteractionState), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto L_54d9; } CYC(b_+56, b_+58);
  CALL_C(b_+58, getGameProgress_1_hook, SYM(getGameProgress_1), b_+61);
  CYC(b_+61, b_+63); C = 0x02;
  CYC(b_+63, b_+65); A = 0x06;
  CALL_C(b_+65, checkNpcShouldExistAtGameStage_body_hook, SYM(checkNpcShouldExistAtGameStage_body), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+71); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+68, b_+71);
  CYC(b_+71, b_+72); A = B;
  CYC(b_+72, b_+75); SET_HL(SYM(lynnaMan2ScriptTable));
  CYC(b_+75, b_+76); push_effect(gb, b_+76);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+77, b_+78); H = mem_rd(gb, HL);
  CYC(b_+78, b_+79); L = A;
  CALL_C(b_+79, interactionSetScript_hook, SYM(interactionSetScript), b_+82);
  CALL_L(b_+82, interactionCode44__initGraphicsAndIncState_hook, b_+85);
L_54d9:
  CALL_C(b_+85, interactionRunScript_hook, SYM(interactionRunScript), b_+88);
  CYC(b_+88, b_+91); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
L_54df:
  CALL_C(b_+91, checkInteractionState_hook, SYM(checkInteractionState), b_+94);
  if (!(F & FZ)) { CYCT(b_+94, b_+96); goto L_5506; } CYC(b_+94, b_+96);
  CALL_C(b_+96, getGameProgress_2_hook, SYM(getGameProgress_2), b_+99);
  CYC(b_+99, b_+100); A = B;
  CYC(b_+100, b_+102); alu_cp(gb, 0x03);
  if ((F & FZ)) { CYCT(b_+102, b_+105); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+102, b_+105);
  CYC(b_+105, b_+107); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+107, b_+109); goto L_54f9; } CYC(b_+107, b_+109);
  CYC(b_+109, b_+112); SET_BC(SYM(interactionCode48__initSubid0d));
  CALL_C(b_+112, interactionSetPosition_hook, SYM(interactionSetPosition), b_+115);
  CYC(b_+115, b_+117); A = 0x06;
L_54f9:
  CYC(b_+117, b_+120); SET_HL(SYM(pastHoboScriptTable));
  CYC(b_+120, b_+121); push_effect(gb, b_+121);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+121, b_+122); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+122, b_+123); H = mem_rd(gb, HL);
  CYC(b_+123, b_+124); L = A;
  CALL_C(b_+124, interactionSetScript_hook, SYM(interactionSetScript), b_+127);
  CALL_L(b_+127, interactionCode44__initGraphicsAndIncState_hook, b_+130);
L_5506:
  CALL_C(b_+130, interactionRunScript_hook, SYM(interactionRunScript), b_+133);
  CYC(b_+133, b_+136); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__subid0_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5492:
  CALL_C(b_+14, checkInteractionState_hook, SYM(checkInteractionState), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_54a2; } CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); A = 0x14;
  CALL_C(b_+21, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+24);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+24, b_+27);
  CALL_L(b_+27, interactionCode44__initGraphicsIncStateAndLoadScript_hook, b_+30);
L_54a2:
  CALL_C(b_+30, interactionRunScript_hook, SYM(interactionRunScript), b_+33);
  if ((F & FC)) { CYCT(b_+33, b_+36); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+33, b_+36);
  CYC(b_+36, b_+39); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__subid0__initialized_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54a2:
  CALL_C(b_+30, interactionRunScript_hook, SYM(interactionRunScript), b_+33);
  if ((F & FC)) { CYCT(b_+33, b_+36); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+33, b_+36);
  CYC(b_+36, b_+39); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__subid1_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54ab:
  CALL_C(b_+39, checkInteractionState_hook, SYM(checkInteractionState), b_+42);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto L_54b3; } CYC(b_+42, b_+44);
  CALL_L(b_+44, interactionCode44__initGraphicsIncStateAndLoadScript_hook, b_+47);
L_54b3:
  CALL_C(b_+47, interactionRunScript_hook, SYM(interactionRunScript), b_+50);
  CYC(b_+50, b_+53); if (hook_enabled_at(gb, SYM(interactionAnimateBasedOnSpeed))) { interactionAnimateBasedOnSpeed_hook(gb); return; } HANDOFF(SYM(interactionAnimateBasedOnSpeed));
}

void interactionCode44__afterCall54b3_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54b3:
  CALL_C(b_+47, interactionRunScript_hook, SYM(interactionRunScript), b_+50);
  CYC(b_+50, b_+53); if (hook_enabled_at(gb, SYM(interactionAnimateBasedOnSpeed))) { interactionAnimateBasedOnSpeed_hook(gb); return; } HANDOFF(SYM(interactionAnimateBasedOnSpeed));
}

void interactionCode44__subid2_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54b9:
  CALL_C(b_+53, checkInteractionState_hook, SYM(checkInteractionState), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto L_54d9; } CYC(b_+56, b_+58);
  CALL_C(b_+58, getGameProgress_1_hook, SYM(getGameProgress_1), b_+61);
  CYC(b_+61, b_+63); C = 0x02;
  CYC(b_+63, b_+65); A = 0x06;
  CALL_C(b_+65, checkNpcShouldExistAtGameStage_body_hook, SYM(checkNpcShouldExistAtGameStage_body), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+71); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+68, b_+71);
  CYC(b_+71, b_+72); A = B;
  CYC(b_+72, b_+75); SET_HL(SYM(lynnaMan2ScriptTable));
  CYC(b_+75, b_+76); push_effect(gb, b_+76);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+77, b_+78); H = mem_rd(gb, HL);
  CYC(b_+78, b_+79); L = A;
  CALL_C(b_+79, interactionSetScript_hook, SYM(interactionSetScript), b_+82);
  CALL_L(b_+82, interactionCode44__initGraphicsAndIncState_hook, b_+85);
L_54d9:
  CALL_C(b_+85, interactionRunScript_hook, SYM(interactionRunScript), b_+88);
  CYC(b_+88, b_+91); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__subid3__initialized_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54d9:
  CALL_C(b_+85, interactionRunScript_hook, SYM(interactionRunScript), b_+88);
  CYC(b_+88, b_+91); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__subid4_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_54df:
  CALL_C(b_+91, checkInteractionState_hook, SYM(checkInteractionState), b_+94);
  if (!(F & FZ)) { CYCT(b_+94, b_+96); goto L_5506; } CYC(b_+94, b_+96);
  CALL_C(b_+96, getGameProgress_2_hook, SYM(getGameProgress_2), b_+99);
  CYC(b_+99, b_+100); A = B;
  CYC(b_+100, b_+102); alu_cp(gb, 0x03);
  if ((F & FZ)) { CYCT(b_+102, b_+105); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+102, b_+105);
  CYC(b_+105, b_+107); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+107, b_+109); goto L_54f9; } CYC(b_+107, b_+109);
  CYC(b_+109, b_+112); SET_BC(SYM(interactionCode48__initSubid0d));
  CALL_C(b_+112, interactionSetPosition_hook, SYM(interactionSetPosition), b_+115);
  CYC(b_+115, b_+117); A = 0x06;
L_54f9:
  CYC(b_+117, b_+120); SET_HL(SYM(pastHoboScriptTable));
  CYC(b_+120, b_+121); push_effect(gb, b_+121);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+121, b_+122); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+122, b_+123); H = mem_rd(gb, HL);
  CYC(b_+123, b_+124); L = A;
  CALL_C(b_+124, interactionSetScript_hook, SYM(interactionSetScript), b_+127);
  CALL_L(b_+127, interactionCode44__initGraphicsAndIncState_hook, b_+130);
L_5506:
  CALL_C(b_+130, interactionRunScript_hook, SYM(interactionRunScript), b_+133);
  CYC(b_+133, b_+136); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__subid4__initialized_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5506:
  CALL_C(b_+130, interactionRunScript_hook, SYM(interactionRunScript), b_+133);
  CYC(b_+133, b_+136); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode44__initGraphicsAndIncState_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_550c:
  CALL_C(b_+136, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+139);
  CALL_C(b_+139, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+142);
  CYC(b_+142, b_+145); if (hook_enabled_at(gb, SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}

void interactionCode44__initGraphicsIncStateAndLoadScript_hook(GB *gb) {
  BASE(interactionCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5515:
  CALL_C(b_+145, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+148);
  CALL_C(b_+148, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+151);
  CYC(b_+151, b_+153); E = 0x42;
  CYC(b_+153, b_+154); A = mem_rd(gb, DE);
  CYC(b_+154, b_+157); SET_HL(SYM(miscMan2ScriptTable));
  CYC(b_+157, b_+158); push_effect(gb, b_+158);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+158, b_+159); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+159, b_+160); H = mem_rd(gb, HL);
  CYC(b_+160, b_+161); L = A;
  CALL_C(b_+161, interactionSetScript_hook, SYM(interactionSetScript), b_+164);
  CYC(b_+164, b_+167); if (hook_enabled_at(gb, SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}

void getGameProgress_1_hook(GB *gb) {
  BASE(getGameProgress_1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x05;
  CYC(b_+2, b_+4); A = 0x14;
  CALL_C(b_+4, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+7);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; } CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); B = alu_dec8(gb, B);
  CYC(b_+9, b_+11); A = 0x13;
  CALL_C(b_+11, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+14);
  if (!(F & FZ)) { CYCT(b_+14, b_+15); ret_effect(gb); return; } CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); A = 0x40;
  CALL_C(b_+17, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+20);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto L_5556; } CYC(b_+20, b_+22);
  CALL_C(b_+22, getHighestSetBit_hook, SYM(getHighestSetBit), b_+25);
  CYC(b_+25, b_+26); C = A;
  CYC(b_+26, b_+28); B = 0x03;
  CYC(b_+28, b_+30); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+30, b_+31); ret_effect(gb); return; } CYC(b_+30, b_+31);
  CYC(b_+31, b_+32); B = alu_dec8(gb, B);
  CYC(b_+32, b_+34); A = 0x11;
  CALL_C(b_+34, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+37);
  if (!(F & FZ)) { CYCT(b_+37, b_+38); ret_effect(gb); return; } CYC(b_+37, b_+38);
  CYC(b_+38, b_+39); B = alu_dec8(gb, B);
  CYC(b_+39, b_+40); A = C;
  CYC(b_+40, b_+42); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+42, b_+43); ret_effect(gb); return; } CYC(b_+42, b_+43);
L_5556:
  CYC(b_+43, b_+45); B = 0x00;
  CYC(b_+45, b_+46); ret_effect(gb); return;
}

void getGameProgress_1__noEssences_hook(GB *gb) {
  BASE(getGameProgress_1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5556:
  CYC(b_+43, b_+45); B = 0x00;
  CYC(b_+45, b_+46); ret_effect(gb); return;
}

void getGameProgress_2_hook(GB *gb) {
  BASE(getGameProgress_2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x07;
  CYC(b_+2, b_+4); A = 0x14;
  CALL_C(b_+4, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+7);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; } CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); B = alu_dec8(gb, B);
  CALL_C(b_+9, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+12);
  if ((F & FZ)) { CYCT(b_+12, b_+14); goto L_556d; } CYC(b_+12, b_+14);
  CYC(b_+14, b_+17); SET_HL((wGroup4RoomFlags + 252));
  CYC(b_+17, b_+19); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+19, b_+20); ret_effect(gb); return; } CYC(b_+19, b_+20);
L_556d:
  CYC(b_+20, b_+21); B = alu_dec8(gb, B);
  CYC(b_+21, b_+23); A = 0x13;
  CALL_C(b_+23, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+27); ret_effect(gb); return; } CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); A = 0x40;
  CALL_C(b_+29, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+32);
  if (!(F & FC)) { CYCT(b_+32, b_+34); goto L_5595; } CYC(b_+32, b_+34);
  CALL_C(b_+34, getHighestSetBit_hook, SYM(getHighestSetBit), b_+37);
  CYC(b_+37, b_+38); C = A;
  CYC(b_+38, b_+40); B = 0x04;
  CYC(b_+40, b_+42); alu_cp(gb, 0x06);
  if (!(F & FC)) { CYCT(b_+42, b_+43); ret_effect(gb); return; } CYC(b_+42, b_+43);
  CYC(b_+43, b_+44); B = alu_dec8(gb, B);
  CYC(b_+44, b_+46); A = 0x11;
  CALL_C(b_+46, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+50); ret_effect(gb); return; } CYC(b_+49, b_+50);
  CYC(b_+50, b_+51); B = alu_dec8(gb, B);
  CYC(b_+51, b_+52); A = C;
  CYC(b_+52, b_+54); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+54, b_+55); ret_effect(gb); return; } CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); B = alu_dec8(gb, B);
  CYC(b_+56, b_+57); A = C;
  CYC(b_+57, b_+59); alu_cp(gb, 0x01);
  if (!(F & FC)) { CYCT(b_+59, b_+60); ret_effect(gb); return; } CYC(b_+59, b_+60);
L_5595:
  CYC(b_+60, b_+62); B = 0x00;
  CYC(b_+62, b_+63); ret_effect(gb); return;
}

void getGameProgress_2__noEssences_hook(GB *gb) {
  BASE(getGameProgress_2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5595:
  CYC(b_+60, b_+62); B = 0x00;
  CYC(b_+62, b_+63); ret_effect(gb); return;
}

void unusedFunc5598_hook(GB *gb) {
  BASE(unusedFunc5598);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+4); SET_HL(SYM(lynnaMan2ScriptTable));
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); H = mem_rd(gb, HL);
  CYC(b_+7, b_+8); L = A;
  CALL_C(b_+8, interactionSetScript_hook, SYM(interactionSetScript), b_+11);
  CYC(b_+11, b_+14); if (hook_enabled_at(gb, SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}

void checkNpcShouldExistAtGameStage_body_hook(GB *gb) {
  BASE(checkNpcShouldExistAtGameStage_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+26);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+9); E = 0x42;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_sub(gb, C);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); H = mem_rd(gb, HL);
  CYC(b_+14, b_+15); L = A;
L_55b5:
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); alu_cp(gb, B);
  if ((F & FZ)) { CYCT(b_+17, b_+18); ret_effect(gb); return; } CYC(b_+17, b_+18);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  if ((F & FZ)) { CYCT(b_+19, b_+21); goto L_55bd; } CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); goto L_55b5;
L_55bd:
  CYC(b_+23, b_+25); alu_or(gb, 0x01);
  CYC(b_+25, b_+26); ret_effect(gb); return;
}
