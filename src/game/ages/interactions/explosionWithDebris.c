#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode99_jump_table(GB *gb) {
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

static void interactionCode99_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_EXPLOSION_WITH_DEBRIS
// ==================================================================================================
void interactionCode99_hook(GB *gb) {
  BASE(interactionCode99);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCode99_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+114) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisible81_hook, SYM(objectSetVisible81), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  CYC(b_+20, b_+21); push_effect(gb, b_+21);
  do { uint16_t jt_ = (interactionCode99_jump_table(gb));
    if (jt_ == b_+27) { goto initSubid00; }
    else if (jt_ == b_+70) { goto initSubid01; }
    else if (jt_ == b_+62) { goto initSubid02; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

initSubid00:
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+31); ret_effect(gb); return; } // ret z
  CYC(b_+30, b_+31);
  CALL_C(b_+31, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+34);
  CYC(b_+34, b_+36); alu_and(gb, 0x03);
  CYC(b_+36, b_+39); SET_HL(b_+179); // @subid0Positions
  CYC(b_+39, b_+40); interactionCode99_add_double_index_from_rst(gb, b_+40);
  CALL_C(b_+40, getRandomNumber_hook, SYM(getRandomNumber), b_+43);
  CYC(b_+43, b_+45); alu_and(gb, 0x07);
  CYC(b_+45, b_+47); alu_sub(gb, 0x03);
  CYC(b_+47, b_+48); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+49); B = A;
  CYC(b_+49, b_+50); SET_HL(HL + 1);
  CALL_C(b_+50, getRandomNumber_hook, SYM(getRandomNumber), b_+53);
  CYC(b_+53, b_+55); alu_and(gb, 0x07);
  CYC(b_+55, b_+57); alu_sub(gb, 0x03);
  CYC(b_+57, b_+58); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+58, b_+59); C = A;
  CYC(b_+59, b_+62); TAIL(interactionSetPosition); // jp

initSubid02:
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+67); A = (uint8_t)(A & ~(1 << 6));
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);

initSubid01:
  CALL_C(b_+70, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+73);
  CYC(b_+73, b_+75); alu_and(gb, 0x03);
  CYC(b_+75, b_+77); alu_add(gb, 0x02);
  CYC(b_+77, b_+78); C = A;
  CYC(b_+78, b_+79); H = D;
  CYC(b_+79, b_+81); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+83); alu_add(gb, A);
  CYC(b_+83, b_+84); alu_add(gb, A);
  CYC(b_+84, b_+85); alu_add(gb, A);
  CYC(b_+85, b_+86); alu_add(gb, C);
  CYC(b_+86, b_+88); alu_and(gb, 0x1f);
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+90, b_+91); mem_wr(gb, HL, A);
  CALL_C(b_+91, getRandomNumber_hook, SYM(getRandomNumber), b_+94);
  CYC(b_+94, b_+96); alu_and(gb, 0x03);
  CYC(b_+96, b_+99); SET_BC(b_+163); // @subid1And2Speeds
  CALL_C(b_+99, addAToBc_hook, 0x006d, b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, BC);
  CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+105, b_+106); mem_wr(gb, HL, A);
  CYC(b_+106, b_+108); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x80);
  CYC(b_+110, b_+111); L = alu_inc8(gb, L);
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0xfe);
  RET(b_+113); return;

state1:
  CYC(b_+114, b_+116); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+116, b_+117); A = mem_rd(gb, DE);
  CYC(b_+117, b_+118); push_effect(gb, b_+118);
  do { uint16_t jt_ = (interactionCode99_jump_table(gb));
    if (jt_ == b_+124) { goto runSubid0; }
    else if (jt_ == b_+167) { goto runSubid1Or2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

runSubid0:
  CALL_C(b_+124, interactionAnimate_hook, SYM(interactionAnimate), b_+127);
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+131, b_+132); ret_effect(gb); return; } // ret z
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+133); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+133, b_+136); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+133, b_+136);
  CYC(b_+136, b_+137); alu_xor(gb, A);
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+140); mem_wr(gb, hFF8B, A);
  CYC(b_+140, b_+142); mem_wr(gb, hFF8D, A);
  CYC(b_+142, b_+144); mem_wr(gb, hFF8C, A);
  CYC(b_+144, b_+146); B = 0x04;

spawnDebrisLoop:
  CALL_C(b_+146, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+149);
  if (!(F & FZ)) { CYCT(b_+149, b_+150); ret_effect(gb); return; } // ret nz
  CYC(b_+149, b_+150);
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x99); // INTERAC_EXPLOSION_WITH_DEBRIS
  CYC(b_+152, b_+153); L = alu_inc8(gb, L);
  CYC(b_+153, b_+154); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+154, b_+155); L = alu_inc8(gb, L);
  CYC(b_+155, b_+156); mem_wr(gb, HL, B);
  CALL_C(b_+156, objectCopyPosition_hook, SYM(objectCopyPosition), b_+159);
  CYC(b_+159, b_+160); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+160, b_+162); goto spawnDebrisLoop; } // jr nz
  CYC(b_+160, b_+162);
  RET(b_+162); return;

runSubid1Or2:
  CALL_C(b_+167, objectApplySpeed_hook, SYM(objectApplySpeed), b_+170);
  CYC(b_+170, b_+172); C = 0x28;
  CALL_C(b_+172, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+175);
  if (F & FZ) { CYCT(b_+175, b_+178); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+175, b_+178);
  RET(b_+178); return;
}
