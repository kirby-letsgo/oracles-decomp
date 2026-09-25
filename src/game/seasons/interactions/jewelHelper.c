#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/jewelHelper.s.
// INTERAC_JEWEL_HELPER

static uint16_t jewelHelper_jump_table(GB *gb) {
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

static void jewelHelper_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode90@spawnGatePuffs
static void jewelHelper_spawnGatePuffs(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+622, b_+625); SET_BC(b_+654 /* @puffPositions */);
  CALL_C(b_+625, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+628);
  CYC(b_+628, b_+630); A = 0x04;
L_5207:
  CYC(b_+630, b_+632); mem_wr(gb, hFF8B, A);
  CALL_C(b_+632, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+635);
  if (!(F & FZ)) { RET_TAKEN(b_+635); return; }
  CYC(b_+635, b_+636);
  CYC(b_+636, b_+638); mem_wr(gb, HL, 0x05);
  CYC(b_+638, b_+640); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+640, b_+641); A = mem_rd(gb, BC);
  CYC(b_+641, b_+642); mem_wr(gb, HL, A);
  CYC(b_+642, b_+643); SET_BC(BC + 1);
  CYC(b_+643, b_+645); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+645, b_+646); A = mem_rd(gb, BC);
  CYC(b_+646, b_+647); mem_wr(gb, HL, A);
  CYC(b_+647, b_+648); SET_BC(BC + 1);
  CYC(b_+648, b_+650); A = mem_rd(gb, hFF8B);
  CYC(b_+650, b_+651); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+651, b_+653); goto L_5207; }
  CYC(b_+651, b_+653);
  RET(b_+653); return;
}

// interactionCode90@spawnJewelGraphic
static void jewelHelper_spawnJewelGraphic(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+613, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+616);
  if (!(F & FZ)) { RET_TAKEN(b_+616); return; }
  CYC(b_+616, b_+617);
  CYC(b_+617, b_+619); mem_wr(gb, HL, 0x92);
  CYC(b_+619, b_+620); L = alu_inc8(gb, L);
  CYC(b_+620, b_+621); mem_wr(gb, HL, C);
  RET(b_+621); return;
}

// interactionCode90@insertJewel
static void jewelHelper_insertJewel(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+604, b_+606); alu_sub(gb, 0x4c);
  CYC(b_+606, b_+607); C = A;
  CYC(b_+607, b_+610); SET_HL(wInsertedJewels);
  CALL_C(b_+610, s_setFlag, SYM(setFlag), b_+613);
  CALL_C(b_+613, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+616);
  if (!(F & FZ)) { RET_TAKEN(b_+616); return; }
  CYC(b_+616, b_+617);
  CYC(b_+617, b_+619); mem_wr(gb, HL, 0x92);
  CYC(b_+619, b_+620); L = alu_inc8(gb, L);
  CYC(b_+620, b_+621); mem_wr(gb, HL, C);
  RET(b_+621); return;
}

// interactionCode90@checkJewelInserted
static void jewelHelper_checkJewelInserted(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+556, s_checkLinkID0AndControlNormal_hook, SYM(checkLinkID0AndControlNormal), b_+559);
  if (!(F & FC)) { RET_TAKEN(b_+559); return; }
  CYC(b_+559, b_+560);
  CYC(b_+560, b_+563); SET_HL(w1Link_direction);
  CYC(b_+563, b_+564); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+564, b_+565); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+565); return; }
  CYC(b_+565, b_+566);
  CYC(b_+566, b_+568); L = 0x0b;
  CYC(b_+568, b_+570); A = 0x36;
  CYC(b_+570, b_+571); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+571, b_+573); alu_cp(gb, 0x15);
  if (!(F & FC)) { RET_TAKEN(b_+573); return; }
  CYC(b_+573, b_+574);
  CYC(b_+574, b_+576); L = 0x0d;
  CYC(b_+576, b_+577); C = mem_rd(gb, HL);
  CYC(b_+577, b_+580); SET_HL(b_+594);
nextJewel:
  CYC(b_+580, b_+581); SET_HL(HL + 1);
  CYC(b_+581, b_+582); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+582, b_+583); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+583); return; }
  CYC(b_+583, b_+584);
  CYC(b_+584, b_+586); alu_add(gb, 0x01);
  CYC(b_+586, b_+587); alu_sub(gb, C);
  CYC(b_+587, b_+589); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+589, b_+591); goto nextJewel; }
  CYC(b_+589, b_+591);
  CYC(b_+591, b_+592); A = mem_rd(gb, HL);
  CYC(b_+592, b_+595);
  TAIL(checkTreasureObtained);
}

// interactionCode90@spawnJewelGraphics
static void jewelHelper_spawnJewelGraphics(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+533, b_+535); C = 0x00;
spawnJewelGraphics_next:
  CYC(b_+535, b_+538); SET_HL(wInsertedJewels);
  CYC(b_+538, b_+539); A = C;
  CALL_C(b_+539, s_checkFlag, SYM(checkFlag), b_+542);
  if (F & FZ) { CYCT(b_+542, b_+544); goto L_51b6; }
  CYC(b_+542, b_+544);
  CYC(b_+544, b_+545); push_effect(gb, BC);
  CALL_L(b_+545, jewelHelper_spawnJewelGraphic, b_+548);
  CYC(b_+548, b_+549); SET_BC(pop_effect(gb));
L_51b6:
  CYC(b_+549, b_+550); C = alu_inc8(gb, C);
  CYC(b_+550, b_+551); A = C;
  CYC(b_+551, b_+553); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+553, b_+555); goto spawnJewelGraphics_next; }
  CYC(b_+553, b_+555);
  RET(b_+555); return;
}

// interactionCode90@shakeScreen
static void jewelHelper_shakeScreen(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+336, b_+338); A = 0x06;
  CALL_C(b_+338, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+341);
  CYC(b_+341, b_+343); A = 0x70;
  CYC(b_+343, b_+346);
  TAIL(playSound_b00);
}

// INTERAC_JEWEL_HELPER
void s_interactionCode90_hook(GB *gb) {
  BASE(interactionCode90);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (jewelHelper_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+169) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+17); SET_HL(b_+670 /* @scriptTable */);
  CYC(b_+17, b_+18); jewelHelper_add_double_index(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+20); H = mem_rd(gb, HL);
  CYC(b_+20, b_+21); L = A;
  CALL_C(b_+21, s_interactionSetScript, SYM(interactionSetScript), b_+24);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); push_effect(gb, b_+28);
  do { uint16_t jt_ = (jewelHelper_jump_table(gb));
    if (jt_ == b_+44) goto subid0Init;
    if (jt_ == b_+56) goto subid1Init;
    if (jt_ == b_+77) goto subid2Init;
    if (jt_ == b_+69) goto subid3Init;
    if (jt_ == b_+75) goto subid4Init;
    if (jt_ == b_+100) goto subid6Init;
    if (jt_ == b_+135) goto subid7Init;
    HANDOFF(HL);
  } while (0);
subid0Init:
  CALL_L(b_+44, jewelHelper_spawnJewelGraphics, b_+47);
  CALL_C(b_+47, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+50);
  CYC(b_+50, b_+52); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+52, b_+55); TAIL(interactionDelete); }
  CYC(b_+52, b_+55);
  RET(b_+55); return;
subid1Init:
  CALL_C(b_+56, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+59);
  if (F & FZ) { CYCT(b_+59, b_+61); goto label_0a_130; }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+66); alu_sub(gb, 0x08);
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
label_0a_130:
  CYC(b_+67, b_+69);
  goto state1;
subid3Init:
  CALL_C(b_+69, s_interactionRunScript, SYM(interactionRunScript), b_+72);
  CALL_C(b_+72, s_interactionRunScript, SYM(interactionRunScript), b_+75);
subid4Init:
  CYC(b_+75, b_+77);
  goto state1;
subid2Init:
  CALL_C(b_+77, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+80);
  CYC(b_+80, b_+82); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+82, b_+84); goto label_0a_131; }
  CYC(b_+82, b_+84);
  RET(b_+84); return;
label_0a_131:
  CALL_C(b_+85, s_getFreePartSlot, SYM(getFreePartSlot), b_+88);
  if (!(F & FZ)) { RET_TAKEN(b_+88); return; }
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x06);
  CYC(b_+91, b_+93); L = PART_BASE + OBJ_YH;
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x78);
  CYC(b_+95, b_+97); L = PART_BASE + OBJ_XH;
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x78);
  RET(b_+99); return;
subid6Init:
  CALL_C(b_+100, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+103);
  CYC(b_+103, b_+105); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+105, b_+108); TAIL(interactionDelete); }
  CYC(b_+105, b_+108);
  CALL_C(b_+108, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+111);
  if (!(F & FZ)) { CYCT(b_+111, b_+113); goto label_0a_132; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+115); A = 0x34;
  CYC(b_+115, b_+118); mem_wr(gb, wChestContentsOverride, A);
  CYC(b_+118, b_+120); A = 0x01;
  CYC(b_+120, b_+123); mem_wr(gb, wChestContentsOverride + 0x01, A);
  CYC(b_+123, b_+126);
  TAIL(interactionDelete);
label_0a_132:
  CYC(b_+126, b_+127); alu_xor(gb, A);
  CYC(b_+127, b_+130); mem_wr(gb, wcca2, A);
  CYC(b_+130, b_+131); A = alu_inc8(gb, A);
  CYC(b_+131, b_+134); mem_wr(gb, wcca1, A);
  RET(b_+134); return;
subid7Init:
  CALL_C(b_+135, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+138);
  if (F & FZ) { CYCT(b_+138, b_+141); TAIL(interactionDelete); }
  CYC(b_+138, b_+141);
  CALL_C(b_+141, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+144);
  CYC(b_+144, b_+146); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+146, b_+149); TAIL(interactionDelete); }
  CYC(b_+146, b_+149);
  CYC(b_+149, b_+151); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+151, b_+154); TAIL(interactionDelete); }
  CYC(b_+151, b_+154);
  CALL_C(b_+154, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+157);
  if (!(F & FZ)) { RET_TAKEN(b_+157); return; }
  CYC(b_+157, b_+158);
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x60);
  CYC(b_+160, b_+161); L = alu_inc8(gb, L);
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0x4d);
  CYC(b_+163, b_+164); L = alu_inc8(gb, L);
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x01);
  CYC(b_+166, b_+169);
  TAIL(objectCopyPosition);
state1:
  CYC(b_+169, b_+171); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+173); push_effect(gb, b_+173);
  do { uint16_t jt_ = (jewelHelper_jump_table(gb));
    if (jt_ == b_+196) goto subid0State1;
    if (jt_ == b_+189) goto runScript;
    if (jt_ == b_+427) goto subid6State1;
    if (jt_ == b_+519) goto subid7State1;
    HANDOFF(HL);
  } while (0);
runScript:
  CALL_C(b_+189, s_interactionRunScript, SYM(interactionRunScript), b_+192);
  if (F & FC) { CYCT(b_+192, b_+195); TAIL(interactionDelete); }
  CYC(b_+192, b_+195);
  RET(b_+195); return;
subid0State1:
  CYC(b_+196, b_+198); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CYC(b_+199, b_+200); push_effect(gb, b_+200);
  do { uint16_t jt_ = (jewelHelper_jump_table(gb));
    if (jt_ == b_+212) goto subid0Substate0;
    if (jt_ == b_+253) goto subid0Substate1;
    if (jt_ == b_+284) goto subid0Substate2;
    if (jt_ == b_+291) goto subid0Substate3;
    if (jt_ == b_+346) goto subid0Substate4;
    if (jt_ == b_+408) goto subid0Substate5;
    HANDOFF(HL);
  } while (0);
subid0Substate0:
  CALL_L(b_+212, jewelHelper_checkJewelInserted, b_+215);
  if (!(F & FC)) { RET_TAKEN(b_+215); return; }
  CYC(b_+215, b_+216);
  CYC(b_+216, b_+217); A = mem_rd(gb, HL);
  CALL_C(b_+217, s_loseTreasure, SYM(loseTreasure), b_+220);
  CYC(b_+220, b_+221); A = mem_rd(gb, HL);
  CALL_L(b_+221, jewelHelper_insertJewel, b_+224);
  CYC(b_+224, b_+226); A = 0x81;
  CYC(b_+226, b_+229); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+229, b_+232); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+232, b_+234); A = 0x4d;
  CALL_C(b_+234, s_playSound, SYM(playSound_b00), b_+237);
  CALL_C(b_+237, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+240);
  CYC(b_+240, b_+241); alu_xor(gb, A);
  CYC(b_+241, b_+244); mem_wr(gb, w1Link_direction, A);
  CALL_C(b_+244, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+247);
  CYC(b_+247, b_+250); SET_HL(SYM(jewelHelperScript_insertedJewel));
  CALL_C(b_+250, s_interactionSetScript, SYM(interactionSetScript), b_+253);
subid0Substate1:
  CALL_C(b_+253, s_interactionRunScript, SYM(interactionRunScript), b_+256);
  if (!(F & FC)) { RET_TAKEN(b_+256); return; }
  CYC(b_+256, b_+257);
  CYC(b_+257, b_+260); A = mem_rd(gb, wInsertedJewels);
  CYC(b_+260, b_+262); alu_cp(gb, 0x0f);
  if (F & FZ) { CYCT(b_+262, b_+264); goto insertedAllJewels; }
  CYC(b_+262, b_+264);
  CYC(b_+264, b_+265); alu_xor(gb, A);
  CYC(b_+265, b_+267); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+267, b_+268); mem_wr(gb, DE, A);
  CYC(b_+268, b_+271); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+271, b_+274); mem_wr(gb, wDisabledObjects, A);
  RET(b_+274); return;
insertedAllJewels:
  CALL_C(b_+275, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+278);
  CYC(b_+278, b_+281); SET_HL(SYM(jewelHelperScript_insertedAllJewels));
  CALL_C(b_+281, s_interactionSetScript, SYM(interactionSetScript), b_+284);
subid0Substate2:
  CALL_C(b_+284, s_interactionRunScript, SYM(interactionRunScript), b_+287);
  if (!(F & FC)) { RET_TAKEN(b_+287); return; }
  CYC(b_+287, b_+288);
  CYC(b_+288, b_+291);
  TAIL(interactionIncSubstate);
subid0Substate3:
  CYC(b_+291, b_+294); SET_HL(b_+392 /* @gateOpenTiles */);
  CYC(b_+294, b_+296); B = 0x04;
L_50b9:
  CYC(b_+296, b_+297); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+297, b_+299); mem_wr(gb, hFF8C, A);
  CYC(b_+299, b_+300); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+300, b_+302); mem_wr(gb, hFF8F, A);
  CYC(b_+302, b_+303); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+303, b_+305); mem_wr(gb, hFF8E, A);
  CYC(b_+305, b_+306); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+306, b_+307); push_effect(gb, HL);
  CYC(b_+307, b_+308); push_effect(gb, BC);
  CALL_C(b_+308, s_setInterleavedTile, SYM(setInterleavedTile), b_+311);
  CYC(b_+311, b_+312); SET_BC(pop_effect(gb));
  CYC(b_+312, b_+313); SET_HL(pop_effect(gb));
  CYC(b_+313, b_+314); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+314, b_+316); goto L_50b9; }
  CYC(b_+314, b_+316);
  CYC(b_+316, b_+318); A = mem_rd(gb, hActiveObject);
  CYC(b_+318, b_+319); D = A;
  CALL_C(b_+319, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+322);
  CYC(b_+322, b_+324); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+324, b_+326); mem_wr(gb, HL, 0x1e);
  CYC(b_+326, b_+328); A = 0x00;
  CALL_L(b_+328, jewelHelper_spawnGatePuffs, b_+331);
  CYC(b_+331, b_+333); A = 0x73;
  CALL_C(b_+333, s_playSound, SYM(playSound_b00), b_+336);
  CYC(b_+336, b_+338); A = 0x06;
  CALL_C(b_+338, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+341);
  CYC(b_+341, b_+343); A = 0x70;
  CYC(b_+343, b_+346);
  TAIL(playSound_b00);
subid0Substate4:
  CALL_C(b_+346, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+349);
  if (!(F & FZ)) { RET_TAKEN(b_+349); return; }
  CYC(b_+349, b_+350);
  CYC(b_+350, b_+353); SET_HL(b_+392 /* @gateOpenTiles */);
  CYC(b_+353, b_+355); B = 0x04;
L_50f4:
  CYC(b_+355, b_+356); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+356, b_+357); C = A;
  CYC(b_+357, b_+358); A = mem_rd(gb, HL);
  CYC(b_+358, b_+359); push_effect(gb, HL);
  CYC(b_+359, b_+360); push_effect(gb, BC);
  CALL_C(b_+360, s_setTile, SYM(setTile), b_+363);
  CYC(b_+363, b_+364); SET_BC(pop_effect(gb));
  CYC(b_+364, b_+365); SET_HL(pop_effect(gb));
  CYC(b_+365, b_+366); SET_HL(HL + 1);
  CYC(b_+366, b_+367); SET_HL(HL + 1);
  CYC(b_+367, b_+368); SET_HL(HL + 1);
  CYC(b_+368, b_+369); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+369, b_+371); goto L_50f4; }
  CYC(b_+369, b_+371);
  CALL_L(b_+371, jewelHelper_shakeScreen, b_+374);
  CYC(b_+374, b_+376); A = 0x04;
  CALL_L(b_+376, jewelHelper_spawnGatePuffs, b_+379);
  CYC(b_+379, b_+381); A = 0x73;
  CALL_C(b_+381, s_playSound, SYM(playSound_b00), b_+384);
  CALL_C(b_+384, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+387);
  CYC(b_+387, b_+389); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+389, b_+391); mem_wr(gb, HL, 0x3c);
  RET(b_+391); return;

subid0Substate5:
  CALL_C(b_+408, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+411);
  if (!(F & FZ)) { RET_TAKEN(b_+411); return; }
  CYC(b_+411, b_+412);
  CYC(b_+412, b_+413); alu_xor(gb, A);
  CYC(b_+413, b_+416); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+416, b_+419); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+419, b_+421); A = 0x4d;
  CALL_C(b_+421, s_playSound, SYM(playSound_b00), b_+424);
  CYC(b_+424, b_+427);
  TAIL(interactionDelete);
subid6State1:
  CYC(b_+427, b_+429); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+429, b_+430); A = mem_rd(gb, DE);
  CYC(b_+430, b_+431); push_effect(gb, b_+431);
  do { uint16_t jt_ = (jewelHelper_jump_table(gb));
    if (jt_ == b_+437) goto subid6State1_substate0;
    if (jt_ == b_+465) goto subid6State1_substate1;
    if (jt_ == b_+495) goto subid6State1_substate2;
    HANDOFF(HL);
  } while (0);
subid6State1_substate0:
  CYC(b_+437, b_+440); A = mem_rd(gb, wcca2);
  CYC(b_+440, b_+441); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+441); return; }
  CYC(b_+441, b_+442);
  CYC(b_+442, b_+445); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+445, b_+446); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+446); return; }
  CYC(b_+446, b_+447);
  CALL_C(b_+447, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+450);
  CYC(b_+450, b_+452); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+452, b_+454); mem_wr(gb, HL, 0x1e);
  CYC(b_+454, b_+456); A = 0x80;
  CYC(b_+456, b_+459); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+459, b_+461); A = 0x81;
  CYC(b_+461, b_+464); mem_wr(gb, wDisabledObjects, A);
  RET(b_+464); return;
subid6State1_substate1:
  CALL_C(b_+465, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+468);
  if (!(F & FZ)) { RET_TAKEN(b_+468); return; }
  CYC(b_+468, b_+469);
  CYC(b_+469, b_+472); SET_BC(0x0580);
  CALL_C(b_+472, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+475);
  if (!(F & FZ)) { RET_TAKEN(b_+475); return; }
  CYC(b_+475, b_+476);
  CYC(b_+476, b_+478); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+478, b_+479); A = mem_rd(gb, HL);
  CYC(b_+479, b_+481); alu_sub(gb, 0x04);
  CYC(b_+481, b_+482); mem_wr(gb, HL, A);
  CYC(b_+482, b_+484); A = 0x85;
  CALL_C(b_+484, s_playSound, SYM(playSound_b00), b_+487);
  CALL_C(b_+487, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+490);
  CYC(b_+490, b_+492); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+492, b_+494); mem_wr(gb, HL, 0x10);
  RET(b_+494); return;
subid6State1_substate2:
  CALL_C(b_+495, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+498);
  if (!(F & FZ)) { RET_TAKEN(b_+498); return; }
  CYC(b_+498, b_+499);
  CYC(b_+499, b_+501); B = 0xe3;
  CALL_C(b_+501, s_objectCreateInteractionWithSubid00, SYM(objectCreateInteractionWithSubid00), b_+504);
  if (!(F & FZ)) { RET_TAKEN(b_+504); return; }
  CYC(b_+504, b_+505);
  CYC(b_+505, b_+507); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+507, b_+508); A = mem_rd(gb, HL);
  CYC(b_+508, b_+510); alu_sub(gb, 0x04);
  CYC(b_+510, b_+511); mem_wr(gb, HL, A);
  CALL_C(b_+511, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+514);
  CYC(b_+514, b_+516); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+516, b_+519);
  TAIL(interactionDelete);
subid7State1:
  CYC(b_+519, b_+521); A = 0x4d;
  CALL_C(b_+521, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+524);
  if (!(F & FC)) { RET_TAKEN(b_+524); return; }
  CYC(b_+524, b_+525);
  CALL_C(b_+525, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+528);
  CYC(b_+528, b_+530); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+530, b_+533);
  TAIL(interactionDelete);
}
