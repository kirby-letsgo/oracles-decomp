#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/makuTree.s.
// INTERAC_MAKU_TREE
// Variables:
// ws_cc39: Maku tree stage
// wc6e5: ???
// ws_c6e0: ???

static uint16_t makuTree_jump_table(GB *gb) {
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

// INTERAC_MAKU_TREE
// Variables:
// ws_cc39: Maku tree stage
// wc6e5: ???
// ws_c6e0: ???
void s_interactionCode87_hook(GB *gb) {
  BASE(interactionCode87);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (makuTree_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+110) goto state1;
    if (jt_ == b_+113) goto state2;
    if (jt_ == b_+116) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (makuTree_jump_table(gb));
    if (jt_ == b_+22) goto subid0;
    if (jt_ == b_+67) goto subid1;
    if (jt_ == b_+87) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+22, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+25);
  CALL_C(b_+25, s_objectSetVisible83, SYM(objectSetVisible83), b_+28);
  CALL_C(b_+28, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+31);
  CALL_C(b_+31, s_makuTree_setAppropriateStage_hook, SYM(makuTree_setAppropriateStage), b_+34);
  CALL_C(b_+34, s_makuTree_spawnGnarledKey_hook, SYM(makuTree_spawnGnarledKey), b_+37);
  CYC(b_+37, b_+40); SET_HL(SYM(script710b));
  CALL_C(b_+40, s_interactionSetScript, SYM(interactionSetScript), b_+43);
  CYC(b_+43, b_+46); A = mem_rd(gb, ws_cc39);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+47, b_+49); goto L_7d49; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); A = 0x01;
  CYC(b_+51, b_+53);
  goto L_7d4b;
L_7d49:
  CYC(b_+53, b_+55); A = 0x02;
L_7d4b:
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CALL_C(b_+58, s_interactionRunScript, SYM(interactionRunScript), b_+61);
  CALL_C(b_+61, s_interactionRunScript, SYM(interactionRunScript), b_+64);
  CYC(b_+64, b_+67);
  TAIL(interactionRunScript);
subid1:
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+69, b_+71); A = 0x02;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CALL_C(b_+72, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+75);
  CALL_C(b_+75, s_objectSetVisible83, SYM(objectSetVisible83), b_+78);
  CYC(b_+78, b_+81); SET_HL(SYM(script7255));
  CALL_C(b_+81, s_interactionSetScript, SYM(interactionSetScript), b_+84);
  CYC(b_+84, b_+87);
  TAIL(interactionRunScript);
subid2:
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+89, b_+91); A = 0x02;
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  CALL_C(b_+92, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+95);
  CALL_C(b_+95, s_objectSetVisible83, SYM(objectSetVisible83), b_+98);
  CALL_C(b_+98, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+101);
  CYC(b_+101, b_+104); SET_HL(SYM(script7261));
  CALL_C(b_+104, s_interactionSetScript, SYM(interactionSetScript), b_+107);
  CYC(b_+107, b_+110);
  TAIL(interactionRunScript);
state1:
  CALL_C(b_+110, s_makuTree_setRoomFlag40OnGnarledKeyGet_hook, SYM(makuTree_setRoomFlag40OnGnarledKeyGet), b_+113);
state2:
  CALL_C(b_+113, s_interactionRunScript, SYM(interactionRunScript), b_+116);
state3:
  CYC(b_+116, b_+119);
  TAIL(interactionAnimate);
}

// makuTree_setAppropriateStage@setStage
static void makuTree_setStage(GB *gb) {
  BASE(makuTree_setAppropriateStage);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+103, b_+106); mem_wr(gb, ws_cc39, A);
  RET(b_+106); return;
}

// This label in used directly from bank 0.
void s_makuTree_setAppropriateStage_hook(GB *gb) {
  BASE(makuTree_setAppropriateStage);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x28;
  CALL_C(b_+2, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); goto setStageToLast; }
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); A = 0x40;
  CALL_C(b_+10, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+13);
  if (F & FC) { CYCT(b_+13, b_+15); goto L_7d9b; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
L_7d9b:
  CYC(b_+16, b_+18); alu_cp(gb, 0x17);
  if (F & FZ) { CYCT(b_+18, b_+20); goto highestEssenceIs5Except4; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_cp(gb, 0x1f);
  if (F & FZ) { CYCT(b_+22, b_+24); goto highestEssenceIs5; }
  CYC(b_+22, b_+24);
  CALL_C(b_+24, s_getHighestSetBit, SYM(getHighestSetBit), b_+27);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto L_7da9; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
L_7da9:
  CALL_L(b_+30, makuTree_setStage, b_+33);
  CYC(b_+33, b_+35); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+35, b_+37); goto highestEssenceIs1; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+39, b_+41); goto highestEssenceIs8; }
  CYC(b_+39, b_+41);
  RET(b_+41); return;
highestEssenceIs1:
  CYC(b_+42, b_+44); A = 0x01;
  CYC(b_+44, b_+46); B = 0x2a;
  CALL_C(b_+46, s_getRoomFlags, SYM(getRoomFlags), b_+49);
  CYC(b_+49, b_+51); alu_and(gb, 0x40);
  if (F & FZ) { RET_TAKEN(b_+51); return; }
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); A = 0x09;
  CYC(b_+54, b_+56);
  goto setStage;
highestEssenceIs5Except4:
  CYC(b_+56, b_+58); A = 0x27;
  CALL_C(b_+58, s_setGlobalFlag, SYM(setGlobalFlag), b_+61);
  CYC(b_+61, b_+63); A = 0x0a;
  CYC(b_+63, b_+65);
  goto setStage;
highestEssenceIs5:
  CYC(b_+65, b_+67); A = 0x27;
  CALL_C(b_+67, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+72); goto L_7dd7; }
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); A = 0x05;
  CYC(b_+74, b_+76);
  goto setStage;
L_7dd7:
  CYC(b_+76, b_+78); A = 0x0b;
  CYC(b_+78, b_+80);
  goto setStage;
highestEssenceIs8:
  CYC(b_+80, b_+83); A = mem_rd(gb, wc6e5);
  CYC(b_+83, b_+85); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+85, b_+87); goto all8Essences; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); A = 0x19;
  CALL_C(b_+89, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+92);
  if (F & FZ) { RET_TAKEN(b_+92); return; }
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+95); A = 0x0c;
  CYC(b_+95, b_+97);
  goto setStage;
all8Essences:
  CYC(b_+97, b_+99); A = 0x0d;
  CYC(b_+99, b_+101);
  goto setStage;
setStageToLast:
  CYC(b_+101, b_+103); A = 0x0e;
setStage:
  CYC(b_+103, b_+106); mem_wr(gb, ws_cc39, A);
  RET(b_+106); return;
}

void s_makuTree_setRoomFlag40OnGnarledKeyGet_hook(GB *gb) {
  BASE(makuTree_setRoomFlag40OnGnarledKeyGet);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x40);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x42;
  CALL_C(b_+8, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+11);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  RET(b_+14); return;
}

void s_makuTree_spawnGnarledKey_hook(GB *gb) {
  BASE(makuTree_spawnGnarledKey);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 6, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x60);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x42);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x01);
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+23, b_+25); A = 0x58;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+26, b_+29); A = mem_rd(gb, ws_c6e0);
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  RET(b_+32); return;
}

