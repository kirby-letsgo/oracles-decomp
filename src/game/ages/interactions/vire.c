#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t vire_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void vire_add_a_to_hl(GB *gb, uint16_t ra) {
  push_effect(gb, ra);
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x12, 0x13, false); burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0, 0x14, 0x15, false);
  } else burn_rom(gb, 0, 0x12, 0x13, true);
  ret_effect(gb);
}

void interactionCodeb8_hook(GB *gb) {
  BASE(interactionCodeb8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); do { uint16_t jt_ = (vire_jump_table(gb));
    if (jt_ == SYM(vire_subid0) && hook_enabled_at(gb, SYM(vire_subid0))) { vire_subid0_hook(gb); return; }
    else if (jt_ == SYM(vire_subid1) && hook_enabled_at(gb, SYM(vire_subid1))) { vire_subid1_hook(gb); return; }
    else if (jt_ == SYM(vire_subid2) && hook_enabled_at(gb, SYM(vire_subid2))) { vire_subid2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void vire_subid0_hook(GB *gb) {
  BASE(vire_subid0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto state0; }
  CYC(b_+3, b_+5);
state1:
  CYC(b_+5, b_+7); E = 0x78;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto runScript; }
  CYC(b_+9, b_+11); CALL_C(b_+11, vire_disableObjectsIfLinkIsReady_hook, SYM(vire_disableObjectsIfLinkIsReady), b_+14);
  if (!(F & FC)) { CYC(b_+14, b_+16); goto animate; }
  CYCT(b_+14, b_+16); CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+20); mem_wr(gb, w1Link_direction, A);
runScript:
  CALL_C(b_+20, interactionRunScript_hook, SYM(interactionRunScript), b_+23);
  if (F & FC) { CYC(b_+23, b_+26); vire_deleteAndReturnControl_hook(gb); return; }
  CYC(b_+23, b_+26);
animate:
  CYC(b_+26, b_+29); TAIL(interactionAnimate);
state0:
  CALL_C(b_+29, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+32);
  CYC(b_+32, b_+34); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYC(b_+34, b_+37); interactionDelete_hook(gb); return; }
  CYC(b_+34, b_+37); CYC(b_+37, b_+39); A = 0x39;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+45); SET_HL(SYM(interactionCoded8__scriptTable));
  TAIL(vire_setScript);
}

void vire_setScript_hook(GB *gb) {
  BASE(vire_setScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionSetScript_hook, SYM(interactionSetScript), b_+3);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CALL_C(b_+6, interactionIncState_hook, SYM(interactionIncState), b_+9);
  CYC(b_+9, b_+11); L = 0x50;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x50);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+17); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A);
  CYC(b_+17, b_+20); TAIL(objectSetVisiblec2);
}

void vire_subid1_hook(GB *gb) {
  BASE(vire_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); do { uint16_t jt_ = (vire_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+39) { goto state1; }
    else if (jt_ == b_+81) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+10, b_+13); A = mem_rd(gb, (wGroup5RoomFlags + 231));
  CYC(b_+13, b_+15); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYC(b_+15, b_+18); interactionDelete_hook(gb); return; }
  CALL_C(b_+18, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 6, mem_rd(gb, HL));
  CYC(b_+23, b_+26); SET_HL((SYM(interactionCoded8__subid0Script) + 1));
  if (F & FZ) { CYCT(b_+26, b_+28); vire_setScript_hook(gb); return; }
  CYC(b_+26, b_+28); CYC(b_+28, b_+31); A = mem_rd(gb, wActiveMusic);
  CYC(b_+31, b_+32); alu_or(gb, A); CYC(b_+32, b_+34); A = 0x2d;
  if (!(F & FZ)) CALL_C_CC(b_+34, playSound_b00_hook, SYM(playSound_b00), b_+37); else CYC(b_+34, b_+37);
  CYC(b_+37, b_+39); goto gotoState2;
state1:
  CYC(b_+39, b_+41); E = 0x78; CYC(b_+41, b_+42); A = mem_rd(gb, DE); CYC(b_+42, b_+43); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto runScript; }
  CYC(b_+43, b_+45); CYC(b_+45, b_+48); A = mem_rd(gb, w1Link_yh); CYC(b_+48, b_+50); alu_cp(gb, 0x9b);
  if (!(F & FC)) { CYC(b_+50, b_+53); goto animate1; }
  CYC(b_+50, b_+53); CALL_C(b_+53, vire_disableObjectsIfLinkIsReady_hook, SYM(vire_disableObjectsIfLinkIsReady), b_+56);
  if (!(F & FC)) { CYC(b_+56, b_+59); goto animate1; }
runScript:
  CALL_C(b_+59, interactionRunScript_hook, SYM(interactionRunScript), b_+62);
  if (!(F & FC)) { CYC(b_+62, b_+65); goto animate1; }
  CALL_C(b_+65, objectSetInvisible_hook, SYM(objectSetInvisible), b_+68);
  CALL_C(b_+68, vire_returnControl_hook, SYM(vire_returnControl), b_+71);
gotoState2:
  CYC(b_+71, b_+72); H = D; CYC(b_+72, b_+74); L = 0x44; CYC(b_+74, b_+76); mem_wr(gb, HL, 0x02); CYC(b_+76, b_+78); L = 0x46; CYC(b_+78, b_+80); mem_wr(gb, HL, 0x08); RET(b_+80); return;
animate1:
  CYC(b_+56, b_+60); TAIL(interactionAnimate);
state2:
  CALL_C(b_+81, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+84);
  if (!(F & FZ)) { RET_TAKEN(b_+84); return; }
  CYC(b_+85, b_+88); SET_HL(w1Link_yh); CYC(b_+88, b_+89); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+89, b_+91); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYC(b_+91, b_+93); goto spawn; }
  CYC(b_+91, b_+93); L = alu_inc8(gb, L); CYC(b_+94, b_+95); A = mem_rd(gb, HL); CYC(b_+95, b_+97); alu_cp(gb, 0xa0);
  if (!(F & FC)) { CYC(b_+97, b_+99); vire_setRandomCounter1_hook(gb); return; }
spawn:
  CALL_C(b_+99, getFreePartSlot_hook, SYM(getFreePartSlot), b_+102);
  if (!(F & FZ)) { CYC(b_+102, b_+104); vire_setRandomCounter1_hook(gb); return; }
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x2c); CYC(b_+106, b_+107); L = alu_inc8(gb, L); CYC(b_+107, b_+108); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); vire_setRandomCounter1_hook(gb);
}

void vire_setRandomCounter1_hook(GB *gb) {
  BASE(vire_setRandomCounter1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03); CYC(b_+5, b_+8); SET_HL(b_+14);
  vire_add_a_to_hl(gb, b_+9);
  CYC(b_+9, b_+11); E = 0x46; CYC(b_+11, b_+12); A = mem_rd(gb, HL); CYC(b_+12, b_+13); mem_wr(gb, DE, A); RET(b_+13); return;
}

void vire_subid2_hook(GB *gb) {
  BASE(vire_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44; CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); do { uint16_t jt_ = (vire_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+46) { goto state1; }
    else if (jt_ == b_+132) { goto state2; }
    else if (jt_ == b_+157) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CALL_C(b_+12, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+15); CYC(b_+15, b_+17); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYC(b_+17, b_+20); interactionDelete_hook(gb); return; }
  CYC(b_+20, b_+23); SET_BC(0xad03); CALL_C(b_+23, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+26);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; }
  CYC(b_+27, b_+29); E = 0x56; CYC(b_+29, b_+31); A = 0x40; CYC(b_+31, b_+32); mem_wr(gb, DE, A); CYC(b_+32, b_+33); E = alu_inc8(gb, E); CYC(b_+33, b_+34); A = H; CYC(b_+34, b_+35); mem_wr(gb, DE, A); CYC(b_+35, b_+38); SET_HL((SYM(interactionCoded8__subid0Script) + 20)); CALL_C(b_+38, vire_setScript_hook, SYM(vire_setScript), b_+41);
  CYC(b_+41, b_+43); L = 0x46; CYC(b_+43, b_+45); mem_wr(gb, HL, 0x08); RET(b_+45); return;
state1:
  CYC(b_+46, b_+49); SET_HL(w1Link_yh); CYC(b_+49, b_+50); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+50, b_+52); alu_cp(gb, 0x40);
  if (!(F & FC)) { CYC(b_+52, b_+54); goto game; }
  CYC(b_+54, b_+55); L = alu_inc8(gb, L); CYC(b_+55, b_+56); A = mem_rd(gb, HL); CYC(b_+56, b_+58); alu_cp(gb, 0x58);
  if (!(F & FC)) { CYC(b_+58, b_+60); goto game; }
  CALL_C(b_+60, vire_disableObjectsIfLinkIsReady_hook, SYM(vire_disableObjectsIfLinkIsReady), b_+63); if (!(F & FC)) { CYC(b_+63, b_+65); goto game; }
  CYC(b_+65, b_+67); A = 1; CYC(b_+67, b_+70); mem_wr(gb, wDisabledObjects, A); CYC(b_+70, b_+73); mem_wr(gb, wTmpcfc0_armosStatue_killedArmosPositions, A); CYC(b_+73, b_+75); A = 3; CYC(b_+75, b_+78); mem_wr(gb, w1Link_direction, A); CYC(b_+78, b_+81); interactionIncState_hook(gb); return;
game:
  CYC(b_+81, b_+82); H = D; CYC(b_+82, b_+84); L = 0x47; CYC(b_+84, b_+86); A = mem_rd(gb, HL); CYC(b_+85, b_+86); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+86, b_+88); goto counter; }
  CYC(b_+88, b_+89); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); if (!(F & FZ)) { CYCT(b_+89, b_+91); goto counter; }
  CYC(b_+89, b_+91); CYC(b_+91, b_+93); E = 0x48; CYC(b_+93, b_+94); alu_xor(gb, A); CYC(b_+94, b_+95); mem_wr(gb, DE, A); CALL_C(b_+95, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+98);
counter:
  CALL_C(b_+98, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+101); if (!(F & FZ)) { CYC(b_+101, b_+103); goto animate2; }
  CALL_C(b_+103, getFreePartSlot_hook, SYM(getFreePartSlot), b_+106); if (!(F & FZ)) { CYC(b_+106, b_+108); goto setCounter; }
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x2c); CALL_C(b_+110, objectCopyPosition_hook, SYM(objectCopyPosition), b_+113); CYC(b_+113, b_+115); E = 0x48; CYC(b_+115, b_+117); A = 1; CYC(b_+117, b_+118); mem_wr(gb, DE, A); CALL_C(b_+118, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+121); CYC(b_+121, b_+123); E = 0x47; CYC(b_+123, b_+125); A = 0x18; CYC(b_+125, b_+126); mem_wr(gb, DE, A);
setCounter:
  CALL_C(b_+126, vire_setRandomCounter1_hook, SYM(vire_setRandomCounter1), b_+129);
animate2:
  CYC(b_+129, b_+132); TAIL(interactionAnimate);
state2:
  CALL_C(b_+132, interactionIncState_hook, SYM(interactionIncState), b_+135); CYC(b_+135, b_+137); L = 0x46; CYC(b_+137, b_+138); alu_xor(gb, A); CYC(b_+138, b_+139); mem_wr(gb, HL, A); SET_HL(HL + 1); CYC(b_+139, b_+140); mem_wr(gb, HL, A); CYC(b_+140, b_+142); E = 0x48; CYC(b_+142, b_+143); A = mem_rd(gb, DE); CYC(b_+143, b_+144); A = alu_dec8(gb, A); if (F & FZ) CALL_C_CC(b_+144, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+147); else CYC(b_+144, b_+147); CYC(b_+147, b_+149); A = 0x80; CYC(b_+149, b_+152); mem_wr(gb, wDisabledObjects, A); CYC(b_+152, b_+154); A = 0xf0; CALL_C(b_+154, playSound_b00_hook, SYM(playSound_b00), b_+157); goto state3;
state3:
  CALL_C(b_+157, interactionRunScript_hook, SYM(interactionRunScript), b_+160); if (!(F & FC)) { CYC(b_+160, b_+162); goto animate2; } CYC(b_+162, b_+164); A = 5; CALL_C(b_+164, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+167); CYC(b_+167, b_+168); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(b_+168, b_+171); interactionDelete_hook(gb); return;
}

void vire_disableObjectsIfLinkIsReady_hook(GB *gb) {
  BASE(vire_disableObjectsIfLinkIsReady);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkInAir); CYC(b_+3, b_+4); alu_or(gb, A); if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CALL_C(b_+5, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+8); if (!(F & FC)) { RET_TAKEN(b_+8); return; }
  CYC(b_+9, b_+11); A = 0x80; CYC(b_+11, b_+14); mem_wr(gb, wDisabledObjects, A); CYC(b_+14, b_+17); mem_wr(gb, wMenuDisabled, A); CYC(b_+17, b_+19); E = 0x78; CYC(b_+19, b_+20); mem_wr(gb, DE, A); CALL_C(b_+20, clearAllParentItems_hook, SYM(clearAllParentItems), b_+23); CALL_C(b_+23, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+26); CYC(b_+26, b_+27); alu_scf(gb); RET(b_+27); return;
}

void vire_deleteAndReturnControl_hook(GB *gb) {
  BASE(vire_deleteAndReturnControl);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDelete_hook, SYM(interactionDelete), SYM(vire_returnControl));
  TAIL(vire_returnControl);
}

void vire_returnControl_hook(GB *gb) {
  BASE(vire_returnControl);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A); CYC(b_+1, b_+4); mem_wr(gb, wDisabledObjects, A); CYC(b_+4, b_+7); mem_wr(gb, wMenuDisabled, A); RET(b_+7); return;
}
