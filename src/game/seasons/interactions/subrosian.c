#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosian.s.
// INTERAC_SUBROSIAN

static uint16_t subrosian_jump_table(GB *gb) {
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

static void subrosian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode30@func_600e
static void subrosian_func_600e(GB *gb) {
  BASE(interactionCode30);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+134, b_+137); SET_HL(wTmpcfc0);
  CYC(b_+137, b_+139); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  RET(b_+139); return;
}

// INTERAC_SUBROSIAN
void s_interactionCode30_hook(GB *gb) {
  BASE(interactionCode30);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosian_jump_table(gb));
    if (jt_ == b_+16) goto state0;
    if (jt_ == b_+82) goto state1;
    if (jt_ == b_+107) goto state2;
    if (jt_ == b_+140) goto state3;
    if (jt_ == b_+151) goto state4;
    if (jt_ == b_+163) goto state5;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+22);
  CYC(b_+22, b_+23); H = D;
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x00);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0xff);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_cp(gb, 0x25);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto L_5fc1; }
  CYC(b_+36, b_+38);
  CALL_C(b_+38, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+41);
  if (F & FZ) { CYCT(b_+41, b_+44); TAIL(interactionDelete); }
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+46); A = 0x0d;
  CALL_C(b_+46, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+49);
  if (F & FZ) { CYCT(b_+49, b_+52); TAIL(interactionDelete); }
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+54, b_+56); A = 0x03;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
L_5fc1:
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+63); SET_HL(SYM(table_607f));
  CYC(b_+63, b_+64); subrosian_add_double_index(gb, b_+64);
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+66); H = mem_rd(gb, HL);
  CYC(b_+66, b_+67); L = A;
  CALL_C(b_+67, s_interactionSetScript, SYM(interactionSetScript), b_+70);
  CALL_C(b_+70, s_interactionRunScript, SYM(interactionRunScript), b_+73);
  CALL_C(b_+73, s_interactionRunScript, SYM(interactionRunScript), b_+76);
  if (F & FC) { CYCT(b_+76, b_+79); TAIL(interactionDelete); }
  CYC(b_+76, b_+79);
  CYC(b_+79, b_+82);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+82, b_+85); A = mem_rd(gb, wActiveGroup);
  CYC(b_+85, b_+86); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); goto L_5fe5; }
  CYC(b_+86, b_+88);
  CALL_C(b_+88, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+91);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x00);
L_5fe5:
  CYC(b_+93, b_+95); C = 0x20;
  CALL_C(b_+95, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+98);
  CALL_C(b_+98, s_interactionRunScript, SYM(interactionRunScript), b_+101);
  if (F & FC) { CYCT(b_+101, b_+104); TAIL(interactionDelete); }
  CYC(b_+101, b_+104);
  CYC(b_+104, b_+107);
  TAIL(npcFaceLinkAndAnimate);
state2:
  CALL_C(b_+107, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+110);
  if (F & FC) CALL_L_CC(b_+110, subrosian_func_600e, b_+113);
  else CYC(b_+110, b_+113);
animateAndRunScript:
  CALL_C(b_+113, s_interactionAnimate, SYM(interactionAnimate), b_+116);
  CALL_C(b_+116, s_interactionAnimate, SYM(interactionAnimate), b_+119);
  CALL_C(b_+119, s_interactionRunScript, SYM(interactionRunScript), b_+122);
  CYC(b_+122, b_+124); C = 0x60;
  CALL_C(b_+124, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+127);
  if (!(F & FZ)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+131); SET_BC(0xfe00);
  CYC(b_+131, b_+134);
  TAIL(objectSetSpeedZ);

state3:
  CALL_C(b_+140, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+143);
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+145, b_+146); mem_wr(gb, DE, A);
  CALL_C(b_+146, s_objectApplySpeed, SYM(objectApplySpeed), b_+149);
  CYC(b_+149, b_+151);
  goto animateAndRunScript;
state4:
  CYC(b_+151, b_+153); C = 0x20;
  CALL_C(b_+153, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+156);
  CALL_C(b_+156, s_interactionRunScript, SYM(interactionRunScript), b_+159);
  if (F & FC) { CYCT(b_+159, b_+162); TAIL(interactionDelete); }
  CYC(b_+159, b_+162);
  RET(b_+162); return;
state5:
  CALL_C(b_+163, s_interactionRunScript, SYM(interactionRunScript), b_+166);
  CYC(b_+166, b_+168); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+168, b_+169); A = mem_rd(gb, DE);
  CYC(b_+169, b_+170); push_effect(gb, b_+170);
  do { uint16_t jt_ = (subrosian_jump_table(gb));
    if (jt_ == b_+178) goto substate0;
    if (jt_ == b_+206) goto substate1;
    if (jt_ == b_+220) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+178, b_+181); A = mem_rd(gb, wTmpcfc0);
  CALL_C(b_+181, s_getHighestSetBit, SYM(getHighestSetBit), b_+184);
  if (!(F & FC)) { RET_TAKEN(b_+184); return; }
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+187); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+187, b_+189); goto L_604b; }
  CYC(b_+187, b_+189);
  CYC(b_+189, b_+191); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+191, b_+193); A = 0x04;
  CYC(b_+193, b_+194); mem_wr(gb, DE, A);
  RET(b_+194); return;
L_604b:
  CYC(b_+195, b_+196); B = A;
  CYC(b_+196, b_+197); A = alu_inc8(gb, A);
  CYC(b_+197, b_+199); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+199, b_+200); mem_wr(gb, DE, A);
  CYC(b_+200, b_+201); A = B;
  CYC(b_+201, b_+203); alu_add(gb, 0x04);
  CYC(b_+203, b_+206);
  TAIL(interactionSetAnimation);
substate1:
  CALL_C(b_+206, s_interactionAnimate, SYM(interactionAnimate), b_+209);
  CYC(b_+209, b_+212); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+213); return; }
  CYC(b_+213, b_+214);
  CYC(b_+214, b_+216); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+216, b_+217); alu_xor(gb, A);
  CYC(b_+217, b_+218); mem_wr(gb, DE, A);
  CYC(b_+218, b_+220);
  goto substate0;
substate2:
  CALL_C(b_+220, s_interactionAnimate, SYM(interactionAnimate), b_+223);
  CYC(b_+223, b_+224); H = D;
  CYC(b_+224, b_+226); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+226, b_+227); A = mem_rd(gb, HL);
  CYC(b_+227, b_+228); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+228, b_+230); goto L_6074; }
  CYC(b_+228, b_+230);
  CYC(b_+230, b_+232); mem_wr(gb, HL, 0x00);
  CYC(b_+232, b_+234); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+234, b_+235); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+235, b_+236); mem_wr(gb, HL, A);
L_6074:
  CYC(b_+236, b_+239); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+239, b_+240); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+240); return; }
  CYC(b_+240, b_+241);
  CYC(b_+241, b_+243); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+243, b_+245); mem_wr(gb, HL, 0x00);
  CYC(b_+245, b_+247);
  goto substate0;
}
