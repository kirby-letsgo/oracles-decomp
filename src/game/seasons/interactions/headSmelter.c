#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/headSmelter.s.
// INTERAC_HEAD_SMELTER

static uint16_t headSmelter_jump_table(GB *gb) {
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

// INTERAC_HEAD_SMELTER
void s_interactionCode53_hook(GB *gb) {
  BASE(interactionCode53);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (headSmelter_jump_table(gb));
    if (jt_ == b_+8) goto subid0;
    if (jt_ == b_+62) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (headSmelter_jump_table(gb));
    if (jt_ == b_+18) goto subid0_state0;
    if (jt_ == b_+43) goto subid0_state1;
    if (jt_ == b_+52) goto subid0_state2;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CALL_C(b_+18, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+23, b_+26); TAIL(interactionDelete); }
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CALL_C(b_+31, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+34);
  CYC(b_+34, b_+37); SET_HL(SYM(headSmelterAtTempleScript));
  CALL_C(b_+37, s_interactionSetScript, SYM(interactionSetScript), b_+40);
  CYC(b_+40, b_+43);
  TAIL(objectSetVisible82);
subid0_state1:
  CALL_C(b_+43, s_interactionRunScript, SYM(interactionRunScript), b_+46);
  CALL_C(b_+46, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+49);
  CYC(b_+49, b_+52);
  TAIL(npcFaceLinkAndAnimate);
subid0_state2:
  CALL_C(b_+52, s_interactionAnimate, SYM(interactionAnimate), b_+55);
  CALL_C(b_+55, s_interactionRunScript, SYM(interactionRunScript), b_+58);
  if (F & FC) { CYCT(b_+58, b_+61); TAIL(interactionDelete); }
  CYC(b_+58, b_+61);
  RET(b_+61); return;
subid1:
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); push_effect(gb, b_+66);
  do { uint16_t jt_ = (headSmelter_jump_table(gb));
    if (jt_ == b_+74) goto subid1_state0;
    if (jt_ == b_+43) goto subid0_state1;
    if (jt_ == b_+52) goto subid0_state2;
    if (jt_ == b_+99) goto subid1_state3;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+74, b_+76); A = 0x0d;
  CALL_C(b_+76, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+79);
  if (F & FZ) { CYCT(b_+79, b_+82); TAIL(interactionDelete); }
  CYC(b_+79, b_+82);
  CYC(b_+82, b_+84); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+84, b_+86); A = 0x01;
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CALL_C(b_+87, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+90);
  CYC(b_+90, b_+93); SET_HL(SYM(headSmelterAtFurnaceScript));
  CALL_C(b_+93, s_interactionSetScript, SYM(interactionSetScript), b_+96);
  CYC(b_+96, b_+99);
  TAIL(objectSetVisible82);
subid1_state3:
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CYC(b_+100, b_+103); mem_wr(gb, wTmpcfc0, A);
  CALL_C(b_+103, s_interactionRunScript, SYM(interactionRunScript), b_+106);
  CYC(b_+106, b_+108); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);
  CYC(b_+109, b_+110); push_effect(gb, b_+110);
  do { uint16_t jt_ = (headSmelter_jump_table(gb));
    if (jt_ == b_+118) goto subid1_state3_substate0;
    if (jt_ == b_+153) goto subid1_state3_substate2;
    HANDOFF(HL);
  } while (0);
subid1_state3_substate0:
  CALL_C(b_+118, s_interactionAnimate, SYM(interactionAnimate), b_+121);
  CYC(b_+121, b_+124); A = mem_rd(gb, wTmpcfc0);
  CALL_C(b_+124, s_getHighestSetBit, SYM(getHighestSetBit), b_+127);
  if (!(F & FC)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+130); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+130, b_+132); goto L_7bb4; }
  CYC(b_+130, b_+132);
  CYC(b_+132, b_+134); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+134, b_+136); A = 0x01;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  RET(b_+137); return;
L_7bb4:
  CYC(b_+138, b_+139); B = A;
  CYC(b_+139, b_+140); B = alu_inc8(gb, B);
  CYC(b_+140, b_+141); H = D;
  CYC(b_+141, b_+143); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+143, b_+144); mem_wr(gb, HL, B);
  CYC(b_+144, b_+146); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+146, b_+148); mem_wr(gb, HL, 0x08);
  CYC(b_+148, b_+150); alu_add(gb, 0x04);
  CYC(b_+150, b_+153);
  TAIL(interactionSetAnimation);
subid1_state3_substate2:
  CALL_C(b_+153, s_interactionAnimate, SYM(interactionAnimate), b_+156);
  CYC(b_+156, b_+157); H = D;
  CYC(b_+157, b_+159); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+161, b_+163); goto L_7bd3; }
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x00);
  CYC(b_+165, b_+167); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+167, b_+168); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+168, b_+169); mem_wr(gb, HL, A);
L_7bd3:
  CYC(b_+169, b_+171); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+171, b_+172); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+172); return; }
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+175); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0x01);
  RET(b_+177); return;
}
