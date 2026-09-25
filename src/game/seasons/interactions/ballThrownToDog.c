#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/ballThrownToDog.s.
// INTERAC_BALL_THROWN_TO_DOG

static uint16_t ballThrownToDog_jump_table(GB *gb) {
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

static void ballThrownToDog_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode83@func_7aea
static void ballThrownToDog_func_7aea(GB *gb) {
  BASE(interactionCode83);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0xff);
  CYC(b_+52, b_+53); L = alu_inc8(gb, L);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0xfc);
  RET(b_+55); return;
}

// INTERAC_BALL_THROWN_TO_DOG
void s_interactionCode83_hook(GB *gb) {
  BASE(interactionCode83);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); H = D;
  CALL_L(b_+9, ballThrownToDog_func_7aea, b_+12);
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x3c);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x18);
  CALL_C(b_+20, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+23);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_7ae4; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x82);
  CYC(b_+27, b_+29); L = 0x57;
  CYC(b_+29, b_+30); mem_wr(gb, HL, D);
  CYC(b_+30, b_+33); SET_BC(0x00f4);
  CALL_C(b_+33, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+36);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+41, b_+42); mem_wr(gb, HL, A);
L_7ae4:
  CALL_C(b_+42, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+45);
  CYC(b_+45, b_+48);
  TAIL(objectSetVisible82);
func_7aea:
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0xff);
  CYC(b_+52, b_+53); L = alu_inc8(gb, L);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0xfc);
  RET(b_+55); return;
state1:
  CALL_C(b_+56, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+59);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+62, b_+64); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); push_effect(gb, b_+68);
  do { uint16_t jt_ = (ballThrownToDog_jump_table(gb));
    if (jt_ == b_+74) goto state1_susbtate0;
    if (jt_ == b_+99) goto state1_susbtate1;
    if (jt_ == b_+137) goto state1_susbtate2;
    HANDOFF(HL);
  } while (0);
state1_susbtate0:
  CYC(b_+74, b_+77); A = mem_rd(gb, wccd4);
  CYC(b_+77, b_+79); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CALL_C(b_+80, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+83);
  CYC(b_+83, b_+85); alu_and(gb, 0x03);
  CYC(b_+85, b_+88); SET_HL(SYM(table_7b59));
  CYC(b_+88, b_+89); ballThrownToDog_add_double_index(gb, b_+89);
  CYC(b_+89, b_+90); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+94); A = mem_rd(gb, HL);
  CYC(b_+94, b_+95); E = alu_inc8(gb, E);
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  CYC(b_+96, b_+99);
  TAIL(interactionIncSubstate);
state1_susbtate1:
  CYC(b_+99, b_+101); C = 0x20;
  CALL_C(b_+101, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+107); TAIL(objectApplySpeed); }
  CYC(b_+104, b_+107);
  CYC(b_+107, b_+109); L = 0x55;
  CYC(b_+109, b_+110); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+110, b_+112); A = alu_srl(gb, A);
  CYC(b_+112, b_+113); B = A;
  CYC(b_+113, b_+114); A = mem_rd(gb, HL);
  CYC(b_+114, b_+115); alu_rra(gb);
  CYC(b_+115, b_+116); alu_cpl(gb);
  CYC(b_+116, b_+118); alu_add(gb, 0x01);
  CYC(b_+118, b_+119); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+119, b_+120); A = B;
  CYC(b_+120, b_+121); alu_cpl(gb);
  CYC(b_+121, b_+123); alu_adc(gb, 0x00);
  CYC(b_+123, b_+124); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+124, b_+127); SET_BC(hObjectPriority1Counter);
  CYC(b_+127, b_+128); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+128, b_+129); H = mem_rd(gb, HL);
  CYC(b_+129, b_+130); L = A;
  CALL_C(b_+130, s_compareHlToBc, SYM(compareHlToBc), b_+133);
  if (F & FC) { RET_TAKEN(b_+133); return; }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+137);
  TAIL(interactionIncSubstate);
state1_susbtate2:
  CYC(b_+137, b_+140); A = mem_rd(gb, wccd4);
  CYC(b_+140, b_+142); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+142); return; }
  CYC(b_+142, b_+143);
  CYC(b_+143, b_+144); alu_xor(gb, A);
  CYC(b_+144, b_+145); mem_wr(gb, DE, A);
  CYC(b_+145, b_+146); H = D;
  CYC(b_+146, b_+148); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+148, b_+150); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+150, b_+151); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+153); E = alu_inc8(gb, E);
  CYC(b_+153, b_+154); E = alu_inc8(gb, E);
  CYC(b_+154, b_+155); A = mem_rd(gb, HL);
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);
  CYC(b_+156, b_+159);
  goto func_7aea;
}

