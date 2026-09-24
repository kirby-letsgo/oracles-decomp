#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/unicornsCave4ChestPuzzle.s.
// INTERAC_D5_4_CHEST_PUZZLE

static uint16_t unicornsCave4ChestPuzzle_jump_table(GB *gb) {
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

// interactionCode62@subid0@func_4fa5
static void unicornsCave4ChestPuzzle_subid0_func_4fa5(GB *gb) {
  BASE(interactionCode62);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+87, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+90);
  if (!(F & FZ)) { RET_TAKEN(b_+90); return; }
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x62);
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x01);
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+98, b_+99); mem_wr(gb, HL, B);
  CYC(b_+99, b_+101); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+101, b_+102); mem_wr(gb, HL, C);
  RET(b_+102); return;
}

// INTERAC_D5_4_CHEST_PUZZLE
void s_interactionCode62_hook(GB *gb) {
  BASE(interactionCode62);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (unicornsCave4ChestPuzzle_jump_table(gb));
    if (jt_ == b_+8) goto subid0;
    if (jt_ == b_+103) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (unicornsCave4ChestPuzzle_jump_table(gb));
    if (jt_ == b_+20) goto subid0_state0;
    if (jt_ == b_+43) goto subid0_state1;
    if (jt_ == b_+103) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CALL_C(b_+20, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+23);
  CYC(b_+23, b_+25); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+25, b_+28); TAIL(interactionDelete); }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+30, b_+32); A = 0x01;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+36); mem_wr(gb, wcca1, A);
  CYC(b_+36, b_+37); alu_xor(gb, A);
  CYC(b_+37, b_+40); mem_wr(gb, wTmpcfc0 + 0x18, A);
  CYC(b_+40, b_+43); mem_wr(gb, wTmpcfc0 + 0x19, A);
subid0_state1:
  CYC(b_+43, b_+46); A = mem_rd(gb, wNumEnemies);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+47); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+51, b_+52); B = A;
  CYC(b_+52, b_+54); C = 0x00;
  CALL_L(b_+54, unicornsCave4ChestPuzzle_subid0_func_4fa5, b_+57);
  CYC(b_+57, b_+60); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+60, b_+61); B = A;
  CYC(b_+61, b_+63); C = 0x01;
  CALL_L(b_+63, unicornsCave4ChestPuzzle_subid0_func_4fa5, b_+66);
  CYC(b_+66, b_+69); A = mem_rd(gb, wTmpcfc0 + 0x12);
  CYC(b_+69, b_+70); B = A;
  CYC(b_+70, b_+72); C = 0x02;
  CALL_L(b_+72, unicornsCave4ChestPuzzle_subid0_func_4fa5, b_+75);
  CYC(b_+75, b_+78); A = mem_rd(gb, wTmpcfc0 + 0x13);
  CYC(b_+78, b_+79); B = A;
  CYC(b_+79, b_+81); C = 0x03;
  CALL_L(b_+81, unicornsCave4ChestPuzzle_subid0_func_4fa5, b_+84);
  CYC(b_+84, b_+87);
  TAIL(interactionDelete);

subid1:
  CYC(b_+103, b_+105); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+107); push_effect(gb, b_+107);
  do { uint16_t jt_ = (unicornsCave4ChestPuzzle_jump_table(gb));
    if (jt_ == b_+117) goto subid1_state0;
    if (jt_ == b_+143) goto subid1_state1;
    if (jt_ == b_+158) goto subid1_state2;
    if (jt_ == b_+261) goto subid1_state3;
    if (jt_ == b_+268) goto subid1_state4;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+117, b_+119); A = 0x01;
  CYC(b_+119, b_+120); mem_wr(gb, DE, A);
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+122, b_+123); A = mem_rd(gb, DE);
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+126, s_setShortPosition, SYM(setShortPosition), b_+129);
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x04);
  CYC(b_+133, b_+134); L = alu_inc8(gb, L);
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x06);
  CYC(b_+136, b_+138); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+140, s_objectCreatePuff, SYM(objectCreatePuff), b_+143);
subid1_state1:
  CALL_C(b_+143, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+146);
  if (!(F & FZ)) { RET_TAKEN(b_+146); return; }
  CYC(b_+146, b_+147);
  CYC(b_+147, b_+149); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+149, b_+150); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+150, b_+152); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+152, b_+153); C = mem_rd(gb, HL);
  CYC(b_+153, b_+155); A = 0xf1;
  CALL_C(b_+155, s_setTile, SYM(setTile), b_+158);
subid1_state2:
  CYC(b_+158, b_+161); A = mem_rd(gb, wTmpcfc0 + 0x19);
  CYC(b_+161, b_+162); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+162, b_+165); TAIL_S(func_5076); }
  CYC(b_+162, b_+165);
  CALL_C(b_+165, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+168);
  CYC(b_+168, b_+171); A = mem_rd(gb, wcca2);
  CYC(b_+171, b_+172); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+172); return; }
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+174); B = A;
  CYC(b_+174, b_+176); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+178); return; }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+182); A = mem_rd(gb, wTmpcfc0 + 0x18);
  CYC(b_+182, b_+183); B = A;
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+187, b_+189); goto subid1_func_5040; }
  CYC(b_+187, b_+189);
  CYC(b_+189, b_+190); A = alu_inc8(gb, A);
  CYC(b_+190, b_+193); mem_wr(gb, wTmpcfc0 + 0x18, A);
  CYC(b_+193, b_+196); SET_HL(w1ReservedInteraction0);
  CYC(b_+196, b_+198); B = 0x40;
  CALL_C(b_+198, s_clearMemory, SYM(clearMemory), b_+201);
  CYC(b_+201, b_+204); SET_HL(w1ReservedInteraction0);
  CYC(b_+204, b_+205); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+205, b_+206); L = alu_inc8(gb, L);
  CYC(b_+206, b_+208); mem_wr(gb, HL, 0x60);
  CYC(b_+208, b_+209); L = alu_inc8(gb, L);
  CYC(b_+209, b_+212); A = mem_rd(gb, wTmpcfc0 + 0x18);
  CYC(b_+212, b_+213); A = alu_dec8(gb, A);
  CYC(b_+213, b_+216); SET_BC(b_+253 /* @subid1@table_504b */);
  CALL_C(b_+216, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+219);
  CYC(b_+219, b_+220); A = mem_rd(gb, BC);
  CYC(b_+220, b_+221); mem_wr(gb, HL, A);
  CYC(b_+221, b_+222); L = alu_inc8(gb, L);
  CYC(b_+222, b_+223); SET_BC(BC + 1);
  CYC(b_+223, b_+224); A = mem_rd(gb, BC);
  CYC(b_+224, b_+225); mem_wr(gb, HL, A);
  CYC(b_+225, b_+228); SET_BC(0xf800);
  CALL_C(b_+228, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+231);
  CYC(b_+231, b_+233); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+233, b_+235); A = 0x03;
  CYC(b_+235, b_+236); mem_wr(gb, DE, A);
  CYC(b_+236, b_+238); A = 0x81;
  CYC(b_+238, b_+241); mem_wr(gb, wDisabledObjects, A);
  RET(b_+241); return;
subid1_func_5040:
  CYC(b_+242, b_+244); A = 0x5a;
  CALL_C(b_+244, s_playSound, SYM(playSound_b00), b_+247);
  CYC(b_+247, b_+249); A = 0x01;
  CYC(b_+249, b_+252); mem_wr(gb, wTmpcfc0 + 0x19, A);
  RET(b_+252); return;

subid1_state3:
  CYC(b_+261, b_+264); A = mem_rd(gb, wTmpcfc0 + 0x19);
  CYC(b_+264, b_+265); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+265, b_+267); TAIL_S(func_5076); }
  CYC(b_+265, b_+267);
  RET(b_+267); return;
subid1_state4:
  CALL_C(b_+268, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+271);
  if (!(F & FZ)) { RET_TAKEN(b_+271); return; }
  CYC(b_+271, b_+272);
  CALL_C(b_+272, s_objectCreatePuff, SYM(objectCreatePuff), b_+275);
  CALL_C(b_+275, s_getFreeEnemySlot, SYM(getFreeEnemySlot), b_+278);
  if (!(F & FZ)) { RET_TAKEN(b_+278); return; }
  CYC(b_+278, b_+279);
  CYC(b_+279, b_+281); mem_wr(gb, HL, 0x19);
  CALL_C(b_+281, s_objectCopyPosition, SYM(objectCopyPosition), b_+284);
  CYC(b_+284, b_+286); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+286, b_+287); A = mem_rd(gb, DE);
  CYC(b_+287, b_+288); C = A;
  CYC(b_+288, b_+290); A = 0xa0;
  CALL_C(b_+290, s_setTile, SYM(setTile), b_+293);
  CYC(b_+293, b_+296);
  TAIL(interactionDelete);
}

void s_func_5076_hook(GB *gb) {
  BASE(func_5076);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x04;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); A = 0x3c;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10); return;
}

