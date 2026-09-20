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

void interactionCode5d_hook(GB *gb);
void bear_state0_hook(GB *gb);
void bear_state0__afterCall6d4b_hook(GB *gb);
void bear_state0__initSubid_hook(GB *gb);
void bear_state0__initSubid00_hook(GB *gb);
void bear_state0__initSubid01_hook(GB *gb);
void bear_state0__initSubid02_hook(GB *gb);
void bear_state0__var03IsNonzero_hook(GB *gb);
void bear_state0__chooseTextID_hook(GB *gb);
void bear_state0__textIDs_hook(GB *gb);
void bear_state1_hook(GB *gb);
void bear_state1__runSubid00_hook(GB *gb);
void bear_state1__substate0_hook(GB *gb);
void bear_state1__substate1_hook(GB *gb);
void bear_state1__substate2_hook(GB *gb);
void bear_state1__substate3_hook(GB *gb);
void bear_state1__substate4_hook(GB *gb);
void bear_state1__runSubid02_hook(GB *gb);

void interactionCode5d_hook(GB *gb) {
  BASE(interactionCode5d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == SYM(bear_state0)) { bear_state0_hook(gb); return; }
    else if (jt_ == SYM(bear_state1)) { bear_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void bear_state0_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
L_6d4b:
  CALL_C(b_+6, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+9);
  CALL_C(b_+9, bear_state0__initSubid_hook, b_+20, b_+12);
  CYC(b_+12, b_+14); E = 0x40;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); if (hook_enabled_at(gb, SYM(objectMarkSolidPosition))) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(SYM(objectMarkSolidPosition)); } CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); ret_effect(gb); return;
}

void bear_state0__afterCall6d4b_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d4b:
  CALL_C(b_+6, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+9);
  CALL_C(b_+9, bear_state0__initSubid_hook, b_+20, b_+12);
  CYC(b_+12, b_+14); E = 0x40;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); if (hook_enabled_at(gb, SYM(objectMarkSolidPosition))) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(SYM(objectMarkSolidPosition)); } CYC(b_+16, b_+19);
  CYC(b_+19, b_+20); ret_effect(gb); return;
}

void bear_state0__initSubid_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d59:
  CYC(b_+20, b_+22); E = 0x42;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+30) { goto L_6d63; }
    else if (jt_ == b_+49) { goto L_6d76; }
    else if (jt_ == b_+50) { goto L_6d77; }
    else { HANDOFF(HL); }
  } while (0);
L_6d63:
  CALL_C(b_+30, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+33);
  CYC(b_+33, b_+35); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_6d70; } CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); E = 0x4b;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_add(gb, 0x10);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
L_6d70:
  CYC(b_+43, b_+46); SET_HL((SYM(dog_subid00__dontDelete) + 19));
  CYC(b_+46, b_+49); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_6d76:
  CYC(b_+49, b_+50); ret_effect(gb); return;
L_6d77:
  CYC(b_+50, b_+52); E = 0x43;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto L_6da8; } CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); A = 0x0a;
  CALL_C(b_+58, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+61);
  if ((F & FZ)) { CYCT(b_+61, b_+64); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+61, b_+64);
  CYC(b_+64, b_+67); SET_HL((SYM(goronSubid02__landed) + 10));
  CALL_C(b_+67, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+70);
  CYC(b_+70, b_+72); A = 0x14;
  CALL_C(b_+72, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+75);
  if (!(F & FZ)) { CYCT(b_+75, b_+78); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+75, b_+78);
  CYC(b_+78, b_+80); A = 0x12;
  CALL_C(b_+80, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+83);
  if ((F & FZ)) { CYCT(b_+83, b_+86); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+83, b_+86);
  CYC(b_+86, b_+88); A = 0x11;
  CALL_C(b_+88, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+91);
  CYC(b_+91, b_+93); A = 0x00;
  if ((F & FZ)) { CYCT(b_+93, b_+96); goto L_6da6; } CYC(b_+93, b_+96);
  CYC(b_+96, b_+97); A = alu_inc8(gb, A);
L_6da6:
  CYC(b_+97, b_+99); goto L_6db2;
L_6da8:
  CYC(b_+99, b_+101); A = 0x14;
  CALL_C(b_+101, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+104);
  if ((F & FZ)) { CYCT(b_+104, b_+107); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+104, b_+107);
  CYC(b_+107, b_+109); A = 0x02;
L_6db2:
  CALL_C(b_+109, bear_state0__chooseTextID_hook, b_+118, b_+112);
  CYC(b_+112, b_+115); SET_HL((SYM(dog_subid01__state0) + 31));
  CYC(b_+115, b_+118); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void bear_state0__initSubid00_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d63:
  CALL_C(b_+30, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+33);
  CYC(b_+33, b_+35); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_6d70; } CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); E = 0x4b;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_add(gb, 0x10);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
L_6d70:
  CYC(b_+43, b_+46); SET_HL((SYM(dog_subid00__dontDelete) + 19));
  CYC(b_+46, b_+49); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void bear_state0__initSubid01_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d76:
  CYC(b_+49, b_+50); ret_effect(gb); return;
}

void bear_state0__initSubid02_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6d77:
  CYC(b_+50, b_+52); E = 0x43;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto L_6da8; } CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); A = 0x0a;
  CALL_C(b_+58, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+61);
  if ((F & FZ)) { CYCT(b_+61, b_+64); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+61, b_+64);
  CYC(b_+64, b_+67); SET_HL((SYM(goronSubid02__landed) + 10));
  CALL_C(b_+67, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+70);
  CYC(b_+70, b_+72); A = 0x14;
  CALL_C(b_+72, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+75);
  if (!(F & FZ)) { CYCT(b_+75, b_+78); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+75, b_+78);
  CYC(b_+78, b_+80); A = 0x12;
  CALL_C(b_+80, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+83);
  if ((F & FZ)) { CYCT(b_+83, b_+86); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+83, b_+86);
  CYC(b_+86, b_+88); A = 0x11;
  CALL_C(b_+88, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+91);
  CYC(b_+91, b_+93); A = 0x00;
  if ((F & FZ)) { CYCT(b_+93, b_+96); goto L_6da6; } CYC(b_+93, b_+96);
  CYC(b_+96, b_+97); A = alu_inc8(gb, A);
L_6da6:
  CYC(b_+97, b_+99); goto L_6db2;
L_6da8:
  CYC(b_+99, b_+101); A = 0x14;
  CALL_C(b_+101, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+104);
  if ((F & FZ)) { CYCT(b_+104, b_+107); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+104, b_+107);
  CYC(b_+107, b_+109); A = 0x02;
L_6db2:
  CALL_C(b_+109, bear_state0__chooseTextID_hook, b_+118, b_+112);
  CYC(b_+112, b_+115); SET_HL((SYM(dog_subid01__state0) + 31));
  CYC(b_+115, b_+118); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void bear_state0__var03IsNonzero_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6da8:
  CYC(b_+99, b_+101); A = 0x14;
  CALL_C(b_+101, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+104);
  if ((F & FZ)) { CYCT(b_+104, b_+107); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+104, b_+107);
  CYC(b_+107, b_+109); A = 0x02;
  CALL_C(b_+109, bear_state0__chooseTextID_hook, b_+118, b_+112);
  CYC(b_+112, b_+115); SET_HL((SYM(dog_subid01__state0) + 31));
  CYC(b_+115, b_+118); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void bear_state0__chooseTextID_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6dbb:
  CYC(b_+118, b_+121); SET_HL(b_+131);
  CYC(b_+121, b_+122); push_effect(gb, b_+122);
  CYC(0x0010, 0x0011); alu_add(gb, L); CYC(0x0011, 0x0012); L = A;
  if (!(F & FC)) { CYCT(0x0012, 0x0013); ret_effect(gb); }
  else { CYC(0x0012, 0x0013); CYC(0x0013, 0x0014); H = alu_inc8(gb, H); CYC(0x0014, 0x0015); ret_effect(gb); }
  CYC(b_+122, b_+123); A = mem_rd(gb, HL);
  CYC(b_+123, b_+125); E = 0x72;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+128); A = 0x57;
  CYC(b_+128, b_+129); E = alu_inc8(gb, E);
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  CYC(b_+130, b_+131); ret_effect(gb); return;
}

void bear_state0__textIDs_hook(GB *gb) {
  BASE(bear_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6dc8:
  CYC(b_+131, b_+132); mem_wr(gb, DE, A);
  CYC(b_+132, b_+133); SET_DE(DE + 1);
  CYC(b_+133, b_+134); D = alu_inc8(gb, D);
  bear_state1_hook(gb); return;
}

void bear_state1_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+10) { bear_state1__runSubid00_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; }
    else if (jt_ == b_+129) { bear_state1__runSubid02_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void bear_state1__runSubid00_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6dd5:
  CALL_C(b_+10, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+13);
  CYC(b_+13, b_+15); E = 0x45;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+27) { goto L_6de6; }
    else if (jt_ == b_+61) { goto L_6e08; }
    else if (jt_ == b_+78) { goto L_6e19; }
    else if (jt_ == b_+95) { goto L_6e2a; }
    else if (jt_ == b_+117) { goto L_6e40; }
    else { HANDOFF(HL); }
  } while (0);
L_6de6:
  CALL_C(b_+27, interactionRunScript_hook, SYM(interactionRunScript), b_+30);
  CYC(b_+30, b_+33); SET_HL(w1Link_xh);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_cp(gb, 0x60);
  if ((F & FC)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); L = 0x0b;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0x3e);
  if (!(F & FC)) { CYCT(b_+42, b_+43); ret_effect(gb); return; } CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); A = 0x08;
  CALL_C(b_+45, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+48);
  CYC(b_+48, b_+50); L = 0x02;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x03);
  CYC(b_+52, b_+55); SET_HL((SYM(dog_subid01__state0) + 21));
  CALL_C(b_+55, interactionSetScript_hook, SYM(interactionSetScript), b_+58);
  CALL_C(b_+58, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+61);
L_6e08:
  CALL_C(b_+61, interactionRunScript_hook, SYM(interactionRunScript), b_+64);
  CYC(b_+64, b_+67); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+67, b_+69); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+69, b_+70); ret_effect(gb); return; } CYC(b_+69, b_+70);
  CALL_C(b_+70, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+73);
  CYC(b_+73, b_+75); A = 0x02;
  CYC(b_+75, b_+78); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
L_6e19:
  CALL_C(b_+78, interactionAnimate_hook, SYM(interactionAnimate), b_+81);
  CYC(b_+81, b_+84); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+84, b_+86); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+86, b_+87); ret_effect(gb); return; } CYC(b_+86, b_+87);
  CALL_C(b_+87, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+90);
  CYC(b_+90, b_+92); L = 0x46;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x28);
  CYC(b_+94, b_+95); ret_effect(gb); return;
L_6e2a:
  CALL_C(b_+95, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+101); if (hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; } HANDOFF(SYM(interactionAnimate)); } CYC(b_+98, b_+101);
  CALL_C(b_+101, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+104);
  CYC(b_+104, b_+106); L = 0x49;
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x02);
  CYC(b_+108, b_+110); L = 0x50;
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x28);
  CYC(b_+112, b_+114); A = 0x01;
  CYC(b_+114, b_+117); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
L_6e40:
  CALL_C(b_+117, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+120);
  if (!(F & FC)) { CYCT(b_+120, b_+123); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+120, b_+123);
  CALL_C(b_+123, objectApplySpeed_hook, SYM(objectApplySpeed), b_+126);
  CYC(b_+126, b_+129); if (hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; } HANDOFF(SYM(interactionAnimate));
}

void bear_state1__substate0_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6de6:
  CALL_C(b_+27, interactionRunScript_hook, SYM(interactionRunScript), b_+30);
  CYC(b_+30, b_+33); SET_HL(w1Link_xh);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_cp(gb, 0x60);
  if ((F & FC)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); L = 0x0b;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0x3e);
  if (!(F & FC)) { CYCT(b_+42, b_+43); ret_effect(gb); return; } CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); A = 0x08;
  CALL_C(b_+45, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+48);
  CYC(b_+48, b_+50); L = 0x02;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x03);
  CYC(b_+52, b_+55); SET_HL((SYM(dog_subid01__state0) + 21));
  CALL_C(b_+55, interactionSetScript_hook, SYM(interactionSetScript), b_+58);
  CALL_C(b_+58, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+61);
L_6e08:
  CALL_C(b_+61, interactionRunScript_hook, SYM(interactionRunScript), b_+64);
  CYC(b_+64, b_+67); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+67, b_+69); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+69, b_+70); ret_effect(gb); return; } CYC(b_+69, b_+70);
  CALL_C(b_+70, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+73);
  CYC(b_+73, b_+75); A = 0x02;
  CYC(b_+75, b_+78); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
}

void bear_state1__substate1_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e08:
  CALL_C(b_+61, interactionRunScript_hook, SYM(interactionRunScript), b_+64);
  CYC(b_+64, b_+67); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+67, b_+69); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+69, b_+70); ret_effect(gb); return; } CYC(b_+69, b_+70);
  CALL_C(b_+70, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+73);
  CYC(b_+73, b_+75); A = 0x02;
  CYC(b_+75, b_+78); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
}

void bear_state1__substate2_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e19:
  CALL_C(b_+78, interactionAnimate_hook, SYM(interactionAnimate), b_+81);
  CYC(b_+81, b_+84); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+84, b_+86); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+86, b_+87); ret_effect(gb); return; } CYC(b_+86, b_+87);
  CALL_C(b_+87, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+90);
  CYC(b_+90, b_+92); L = 0x46;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x28);
  CYC(b_+94, b_+95); ret_effect(gb); return;
}

void bear_state1__substate3_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e2a:
  CALL_C(b_+95, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+101); if (hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; } HANDOFF(SYM(interactionAnimate)); } CYC(b_+98, b_+101);
  CALL_C(b_+101, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+104);
  CYC(b_+104, b_+106); L = 0x49;
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x02);
  CYC(b_+108, b_+110); L = 0x50;
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x28);
  CYC(b_+112, b_+114); A = 0x01;
  CYC(b_+114, b_+117); if (hook_enabled_at(gb, SYM(interactionSetAnimation))) { interactionSetAnimation_hook(gb); return; } HANDOFF(SYM(interactionSetAnimation));
}

void bear_state1__substate4_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e40:
  CALL_C(b_+117, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+120);
  if (!(F & FC)) { CYCT(b_+120, b_+123); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+120, b_+123);
  CALL_C(b_+123, objectApplySpeed_hook, SYM(objectApplySpeed), b_+126);
  CYC(b_+126, b_+129); if (hook_enabled_at(gb, SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; } HANDOFF(SYM(interactionAnimate));
}

void bear_state1__runSubid02_hook(GB *gb) {
  BASE(bear_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6e4c:
  CALL_C(b_+129, interactionRunScript_hook, SYM(interactionRunScript), b_+132);
  if ((F & FC)) { CYCT(b_+132, b_+135); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+132, b_+135);
  CYC(b_+135, b_+138); if (hook_enabled_at(gb, SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}
