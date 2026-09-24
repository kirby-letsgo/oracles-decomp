#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/mittensAndOwner.s.
// INTERAC_MITTENS
// INTERAC_MITTENS_OWNER

static uint16_t mittensAndOwner_jump_table(GB *gb) {
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

// interactionCode26@func_5a78
static void mittensAndOwner_func_5a78(GB *gb) {
  BASE(interactionCode25);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x02);
  CYC(b_+111, b_+114); SET_HL(SYM(mittensOwnerScript));
  CYC(b_+114, b_+117);
  TAIL(interactionSetScript);
}

// INTERAC_MITTENS
// INTERAC_MITTENS_OWNER
void s_interactionCode25_hook(GB *gb) {
  BASE(interactionCode25);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mittensAndOwner_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+117) goto state1;
    if (jt_ == b_+133) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+10, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+13);
  CYC(b_+13, b_+15); A = 0x0b;
  CALL_C(b_+15, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+18);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_cp(gb, 0x26);
  if (F & FZ) { CYCT(b_+23, b_+25); goto mittensOwner; }
  CYC(b_+23, b_+25);
  CALL_C(b_+25, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+28);
  CYC(b_+28, b_+30); alu_and(gb, 0x40);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto state0_savedMittens; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+36, b_+39); TAIL(interactionDelete); }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+41);
  goto incStateSetScript;
state0_savedMittens:
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+42, b_+45); TAIL(interactionDelete); }
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+47);
  goto incStateSetScript;
mittensOwner:
  CALL_C(b_+47, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+50);
  CYC(b_+50, b_+52); alu_and(gb, 0x40);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto mittensOwner_savedMittens; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+58); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+58, b_+61); TAIL(interactionDelete); }
  CYC(b_+58, b_+61);
  CALL_L(b_+61, mittensAndOwner_func_5a78, b_+64);
  CYC(b_+64, b_+66); A = 0x00;
  CALL_C(b_+66, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+69);
  CYC(b_+69, b_+72);
  goto animate;
mittensOwner_savedMittens:
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+73, b_+76); TAIL(interactionDelete); }
  CYC(b_+73, b_+76);
  CALL_L(b_+76, mittensAndOwner_func_5a78, b_+79);
  CYC(b_+79, b_+81); A = 0x02;
  CALL_C(b_+81, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+84);
  CYC(b_+84, b_+87);
  goto animate;
incStateSetScript:
  CYC(b_+87, b_+88); H = D;
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x01);
  CYC(b_+92, b_+95); SET_HL(SYM(mittensScript));
  CALL_C(b_+95, s_interactionSetScript, SYM(interactionSetScript), b_+98);
  CYC(b_+98, b_+100); A = 0x02;
  CALL_C(b_+100, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+103);
  CYC(b_+103, b_+106);
  goto animate;

state1:
  CALL_C(b_+117, s_interactionRunScript, SYM(interactionRunScript), b_+120);
  CYC(b_+120, b_+123); A = mem_rd(gb, wccd4);
  CYC(b_+123, b_+124); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+124, b_+127); TAIL_SG(npcFaceLinkAndAnimate); }
  CYC(b_+124, b_+127);
  CALL_C(b_+127, s_func_5a99_hook, SYM(func_5a99), b_+130);
  CYC(b_+130, b_+133);
  goto animate;
state2:
  CALL_C(b_+133, s_interactionRunScript, SYM(interactionRunScript), b_+136);
animate:
  CYC(b_+136, b_+139);
  TAIL(interactionAnimateAsNpc);
}

void s_func_5a99_hook(GB *gb) {
  BASE(func_5a99);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mittensAndOwner_jump_table(gb));
    if (jt_ == b_+12) goto var38_00;
    if (jt_ == b_+41) goto var38_01;
    if (jt_ == b_+59) goto var38_02;
    if (jt_ == b_+85) goto var38_03;
    HANDOFF(HL);
  } while (0);
var38_00:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x01);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x08);
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x28);
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x00);
  CYC(b_+29, b_+30); SET_HL(HL + 1);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xfe);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x04);
  CYC(b_+36, b_+38); A = 0x07;
  CYC(b_+38, b_+41);
  TAIL(interactionSetAnimation);
var38_01:
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+51); A = 0x08;
  CALL_C(b_+51, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+54);
  CYC(b_+54, b_+56); A = 0x53;
  CYC(b_+56, b_+59);
  TAIL(playSound_b00);
var38_02:
  CYC(b_+59, b_+61); C = 0x28;
  CALL_C(b_+61, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+67); TAIL(objectApplySpeed); }
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+68); H = D;
  CYC(b_+68, b_+70); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+70, b_+71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+71, b_+73); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x04);
  CYC(b_+75, b_+77); A = 0x07;
  CALL_C(b_+77, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+80);
  CYC(b_+80, b_+82); A = 0x57;
  CYC(b_+82, b_+85);
  TAIL(playSound_b00);
var38_03:
  CYC(b_+85, b_+86); H = D;
  CYC(b_+86, b_+88); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+88, b_+89); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+91); alu_xor(gb, A);
  CYC(b_+91, b_+94); mem_wr(gb, wccd4, A);
  CYC(b_+94, b_+96); A = 0x02;
  CYC(b_+96, b_+99);
  TAIL(interactionSetAnimation);
}

