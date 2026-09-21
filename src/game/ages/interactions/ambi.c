#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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

static void ambi_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode4d_hook(GB *gb);
void ambi_state1_hook(GB *gb);
void ambi_updateAnimationAndRunScript_hook(GB *gb);
void ambi_runSubid01_hook(GB *gb);
void ambi_ret_hook(GB *gb);
void ambi_runSubid02_hook(GB *gb);
void ambi_runSubid03_hook(GB *gb);
void ambi_runSubid04_hook(GB *gb);
void ambi_runSubid05_hook(GB *gb);
void ambi_runSubid06_hook(GB *gb);
void ambi_runSubid08_hook(GB *gb);
void ambi_runSubid0a_hook(GB *gb);
void ambi_loadScript_hook(GB *gb);

static void ambi_init_subid(GB *gb, uint16_t sp0_) {
  BASE(interactionCode4d);
  CYC(b_+28, b_+30); E = 0x42;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+54) { goto subid1; }
    else if (jt_ == b_+72) { goto subid3; }
    else if (jt_ == b_+80) { goto subid4; }
    else if (jt_ == b_+91) { goto subid0; }
    else if (jt_ == b_+97) { goto subid2; }
    else if (jt_ == b_+100) { goto subid5; }
    else if (jt_ == b_+114) { goto subid6; }
    else if (jt_ == b_+195) { goto subid7; }
    else if (jt_ == b_+198) { goto subida; }
    else { HANDOFF(HL); }
  } while (0);

subid1:
  CYC(b_+54, b_+57); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+57, b_+59); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+59, b_+62); ambi_loadScript_hook(gb); return; }
  CYC(b_+59, b_+62);
  CALL_C(b_+62, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+66); ret_effect(gb); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+69); SET_HL((SYM(subrosian_loadScript) + 6));
  CYC(b_+69, b_+72); interactionSetScript_hook(gb);
  return;

subid3:
  CALL_C(b_+72, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+75);
  CYC(b_+75, b_+77); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+77, b_+80); interactionDelete_hook(gb); return; }
  CYC(b_+77, b_+80);
subid4:
  CYC(b_+80, b_+83); SET_HL((SYM(dog_setTargetPositionIndex__map3) + 4));
  CYC(b_+83, b_+85); E = 0x08;
  CALL_C(b_+85, interBankCall_hook, 0x008a, b_+88);
  CYC(b_+88, b_+91); ambi_loadScript_hook(gb);
  return;

subid0:
  CALL_C(b_+91, soldierCheckBeatD6_hook, SYM(soldierCheckBeatD6), b_+94);
  if (!(F & FC)) { CYCT(b_+94, b_+97); interactionDelete_hook(gb); return; }
  CYC(b_+94, b_+97);
subid2:
  CYC(b_+97, b_+100); ambi_loadScript_hook(gb);
  return;

subid5:
  CYC(b_+100, b_+103); SET_HL((SYM(dog_setTargetPositionIndex__map3) + 4));
  CYC(b_+103, b_+105); E = 0x08;
  CALL_C(b_+105, interBankCall_hook, 0x008a, b_+108);
  CALL_C(b_+108, objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+111);
  CYC(b_+111, b_+114); ambi_loadScript_hook(gb);
  return;

subid6:
  CALL_C(b_+114, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+117);
  CYC(b_+117, b_+119); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+119, b_+122); interactionDelete_hook(gb); return; }
  CYC(b_+119, b_+122);
  CYC(b_+122, b_+124); A = 0x85;
  CALL_C(b_+124, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+127);
  CYC(b_+127, b_+128); H = D;
  CYC(b_+128, b_+130); L = 0x5c;
  CYC(b_+130, b_+132); A = 0x06;
  CYC(b_+132, b_+133); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+133, b_+134); mem_wr(gb, HL, A);
  CYC(b_+134, b_+136); A = 0x01;
  CYC(b_+136, b_+139); W8(wNumEnemies) = A;
  CALL_C(b_+139, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+142);
  if (F & FZ) { CYCT(b_+142, b_+144); goto spawn_ghost_veran; }
  CYC(b_+142, b_+144);
  CYC(b_+144, b_+146); E = 0x44;
  CYC(b_+146, b_+147); alu_xor(gb, A);
  CYC(b_+147, b_+148); mem_wr(gb, DE, A);
  CYC(b_+148, b_+149); ret_effect(gb);
  return;

spawn_ghost_veran:
  CYC(b_+149, b_+151); mem_wr(gb, HL, 0x3e);
  CYC(b_+151, b_+152); L = alu_inc8(gb, L);
  CYC(b_+152, b_+153); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+153, b_+156); SET_BC(0xf000);
  CALL_C(b_+156, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+159);
  CYC(b_+159, b_+161); A = 0xf0;
  CALL_C(b_+161, playSound_b00_hook, SYM(playSound_b00), b_+164);
  CYC(b_+164, b_+167); SET_HL(w1Link_direction);
  CYC(b_+167, b_+168); alu_xor(gb, A);
  CYC(b_+168, b_+169); W8(w1Link_direction) = A;
  CYC(b_+169, b_+170); W8(w1Link_angle) = A;
  CYC(b_+170, b_+173); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+173, b_+176); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(b_+176, b_+177); A = alu_dec8(gb, A);
  CYC(b_+177, b_+180); W8(wActiveMusic) = A;
  CYC(b_+180, b_+183); SET_HL(wcc93);
  CYC(b_+183, b_+185); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+185, b_+187); A = 0x0b;
  CYC(b_+187, b_+190); W8(wLinkForceState) = A;
  CYC(b_+190, b_+192); A = 0x16;
  CYC(b_+192, b_+195); W8(wLinkStateParameter) = A;
subid7:
  CYC(b_+195, b_+198); ambi_loadScript_hook(gb);
  return;

subida:
  CALL_C(b_+198, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+201);
  if (F & FZ) { CYCT(b_+201, b_+204); interactionDelete_hook(gb); return; }
  CYC(b_+201, b_+204);
  CYC(b_+204, b_+207); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+207, b_+209); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+209, b_+212); interactionDelete_hook(gb); return; }
  CYC(b_+209, b_+212);
  CYC(b_+212, b_+215); ambi_loadScript_hook(gb);
}

void interactionCode4d_hook(GB *gb) {
  BASE(interactionCode4d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == SYM(ambi_state1) && hook_is(gb, SYM(ambi_state1), ambi_state1_hook)) { ambi_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+20); push_effect(gb, b_+20); ambi_init_subid(gb, sp0_);
  CYC(b_+20, b_+22); E = 0x40;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+24, b_+27); objectMarkSolidPosition_hook(gb); return; }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+28); ret_effect(gb);
}

void ambi_state1_hook(GB *gb) {
  BASE(ambi_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == SYM(ambi_updateAnimationAndRunScript) && hook_is(gb, SYM(ambi_updateAnimationAndRunScript), ambi_updateAnimationAndRunScript_hook)) { ambi_updateAnimationAndRunScript_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid01) && hook_is(gb, SYM(ambi_runSubid01), ambi_runSubid01_hook)) { ambi_runSubid01_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid02) && hook_is(gb, SYM(ambi_runSubid02), ambi_runSubid02_hook)) { ambi_runSubid02_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid03) && hook_is(gb, SYM(ambi_runSubid03), ambi_runSubid03_hook)) { ambi_runSubid03_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid04) && hook_is(gb, SYM(ambi_runSubid04), ambi_runSubid04_hook)) { ambi_runSubid04_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid05) && hook_is(gb, SYM(ambi_runSubid05), ambi_runSubid05_hook)) { ambi_runSubid05_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid06) && hook_is(gb, SYM(ambi_runSubid06), ambi_runSubid06_hook)) { ambi_runSubid06_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid08) && hook_is(gb, SYM(ambi_runSubid08), ambi_runSubid08_hook)) { ambi_runSubid08_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), interactionAnimate_hook)) { interactionAnimate_hook(gb); return; }
    else if (jt_ == SYM(ambi_runSubid0a) && hook_is(gb, SYM(ambi_runSubid0a), ambi_runSubid0a_hook)) { ambi_runSubid0a_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void ambi_updateAnimationAndRunScript_hook(GB *gb) {
  BASE(ambi_updateAnimationAndRunScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionRunScript);
}

void ambi_runSubid01_hook(GB *gb) {
  BASE(ambi_runSubid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto update_substate; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+8, b_+10); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(b_+10, b_+13); goto update_substate; }
  CYC(b_+10, b_+13);
  CALL_C(b_+13, interactionAnimate_hook, SYM(interactionAnimate), b_+16);
  CYC(b_+16, b_+19); SET_HL((SYM(checkNpcShouldExistAtGameStage_body__data5__subid2) + 3));
  CYC(b_+19, b_+21); E = 0x15;
  CYC(b_+21, b_+24); interBankCall_hook(gb);
  return;

update_substate:
  CYC(b_+24, b_+26); E = 0x45;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+34) { goto substate0; }
    else if (jt_ == b_+52) { goto substate1; }
    else if (jt_ == SYM(ambi_updateAnimationAndRunScript) && hook_is(gb, SYM(ambi_updateAnimationAndRunScript), ambi_updateAnimationAndRunScript_hook)) { ambi_updateAnimationAndRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+34, b_+37); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+37, b_+39); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); ambi_updateAnimationAndRunScript_hook(gb); return; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+44); SET_HL((SYM(interactionCode64__state0) + 5));
  CYC(b_+44, b_+46); E = 0x08;
  CALL_C(b_+46, interBankCall_hook, 0x008a, b_+49);
  CYC(b_+49, b_+52); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+52, b_+54); C = 0x20;
  CALL_C(b_+54, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+57);
  if (!(F & FZ)) { CYCT(b_+57, b_+58); ret_effect(gb); return; }
  CYC(b_+57, b_+58);
  CALL_C(b_+58, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+61);
  CYC(b_+61, b_+63); L = 0x7e;
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  TAIL(ambi_ret);
}

void ambi_ret_hook(GB *gb) {
  BASE(ambi_ret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); ret_effect(gb);
}

void ambi_runSubid02_hook(GB *gb) {
  BASE(ambi_runSubid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+17) { goto substate1; }
    else if (jt_ == SYM(interactionAnimateBasedOnSpeed) && hook_is(gb, SYM(interactionAnimateBasedOnSpeed), interactionAnimateBasedOnSpeed_hook)) { interactionAnimateBasedOnSpeed_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+10, ambi_updateAnimationAndRunScript_hook, SYM(ambi_updateAnimationAndRunScript), b_+13);
  if (!(F & FC)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+17); interactionIncSubstate_hook(gb);
  return;

substate1:
  CALL_C(b_+17, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+20);
  CALL_C(b_+20, objectApplySpeed_hook, SYM(objectApplySpeed), b_+23);
  CYC(b_+23, b_+26); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+26, b_+28); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+28, b_+29); ret_effect(gb); return; }
  CYC(b_+28, b_+29);
  CALL_C(b_+29, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+32);
  CYC(b_+32, b_+35); SET_BC((SYM(soldierSubid04Substate1) + 5));
  CYC(b_+35, b_+38); TAIL(interactionSetPosition);
}

void ambi_runSubid03_hook(GB *gb) {
  BASE(ambi_runSubid03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+14) { goto substate0; }
    else if (jt_ == b_+41) { goto animate; }
    else if (jt_ == b_+47) { goto substate1; }
    else if (jt_ == b_+62) { goto substate2; }
    else if (jt_ == b_+85) { goto substate3; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+14, b_+17); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+17, b_+19); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto animate; }
  CYC(b_+19, b_+21);
  CALL_C(b_+21, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+24);
  CYC(b_+24, b_+26); L = 0x46;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x14);
  CALL_C(b_+28, getFreePartSlot_hook, SYM(getFreePartSlot), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+32); ret_effect(gb); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x27);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+38, b_+41); objectCopyPosition_hook(gb);
  return;

animate:
  CALL_C(b_+41, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+44);
  CYC(b_+44, b_+47); interactionRunScript_hook(gb);
  return;

substate1:
  CALL_C(b_+47, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+50);
  if (!(F & FZ)) { CYCT(b_+50, b_+51); ret_effect(gb); return; }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CYC(b_+52, b_+55); W8(wTmpcbb3) = A;
  CYC(b_+55, b_+56); A = alu_dec8(gb, A);
  CYC(b_+56, b_+59); W8(wTmpcbba) = A;
  CYC(b_+59, b_+62); interactionIncSubstate_hook(gb);
  return;

substate2:
  CYC(b_+62, b_+65); SET_HL(wTmpcbb3);
  CYC(b_+65, b_+67); B = 2;
  CALL_C(b_+67, flashScreen_hook, SYM(flashScreen), b_+70);
  if (F & FZ) { CYCT(b_+70, b_+71); ret_effect(gb); return; }
  CYC(b_+70, b_+71);
  CALL_C(b_+71, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+74);
  CYC(b_+74, b_+77); SET_BC(0x8408);
  CALL_C(b_+77, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+80);
  CYC(b_+80, b_+82); A = 2;
  CYC(b_+82, b_+85); fadeinFromWhiteWithDelay_hook(gb);
  return;

substate3:
  CYC(b_+85, b_+88); A = W8(wPaletteThread_mode);
  CYC(b_+88, b_+89); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+89, b_+90); ret_effect(gb); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); A = 2;
  CYC(b_+92, b_+95); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(b_+95, b_+98); TAIL(interactionIncSubstate);
}

void ambi_runSubid04_hook(GB *gb) {
  BASE(ambi_runSubid04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ambi_jump_table(gb));
    if (jt_ == b_+10) { goto substate0; }
    else if (jt_ == b_+25) { goto substate1; }
    else if (jt_ == SYM(interactionAnimate) && hook_is(gb, SYM(interactionAnimate), interactionAnimate_hook)) { interactionAnimate_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+10, ambi_updateAnimationAndRunScript_hook, SYM(ambi_updateAnimationAndRunScript), b_+13);
  if (!(F & FC)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); W8(wTmpcbb3) = A;
  CYC(b_+18, b_+19); A = alu_dec8(gb, A);
  CYC(b_+19, b_+22); W8(wTmpcbba) = A;
  CYC(b_+22, b_+25); interactionIncSubstate_hook(gb);
  return;

substate1:
  CYC(b_+25, b_+28); SET_HL(wTmpcbb3);
  CYC(b_+28, b_+30); B = 2;
  CALL_C(b_+30, flashScreen_hook, SYM(flashScreen), b_+33);
  if (F & FZ) { CYCT(b_+33, b_+34); ret_effect(gb); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); A = 3;
  CYC(b_+36, b_+39); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(b_+39, b_+42); TAIL(interactionIncSubstate);
}

void ambi_runSubid05_hook(GB *gb) {
  BASE(ambi_runSubid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+9); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+9, b_+11); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+11, b_+14); interactionAnimate_hook(gb); return; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void ambi_runSubid06_hook(GB *gb) {
  BASE(ambi_runSubid06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+6); interactionAnimate_hook(gb); return; }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); A = 1;
  CYC(b_+8, b_+11); W8(wLoadedTreeGfxIndex) = A;
  CYC(b_+11, b_+14); TAIL(interactionDelete);
}

void ambi_runSubid08_hook(GB *gb) {
  BASE(ambi_runSubid08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ambi_updateAnimationAndRunScript_hook, SYM(ambi_updateAnimationAndRunScript), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 1;
  CYC(b_+6, b_+9); W8(wTmpcbb8) = A;
  CYC(b_+9, b_+11); A = 8;
  CYC(b_+11, b_+14); W8(wCutsceneTrigger) = A;
  CYC(b_+14, b_+17); TAIL(interactionDelete);
}

void ambi_runSubid0a_hook(GB *gb) {
  BASE(ambi_runSubid0a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, npcFaceLinkAndAnimate_hook, SYM(npcFaceLinkAndAnimate), b_+3);
  CYC(b_+3, b_+6); TAIL(interactionRunScript);
}

void ambi_loadScript_hook(GB *gb) {
  BASE(ambi_loadScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+13);
  CYC(b_+6, b_+7); ambi_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); TAIL(interactionSetScript);
}
