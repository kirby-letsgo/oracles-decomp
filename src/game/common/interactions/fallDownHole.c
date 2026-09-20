#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode0f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode0f), (from), (to), true)

// interactionCode0f@specialHoleRooms: lookupKey table (room, group pairs, $00 terminated).
#define specialHoleRooms_bank08 SYM(interactionCode0f__specialHoleRooms)

static uint16_t fallDownHole_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// interactionCode0f@checkUpdateHoleEvent: in a few special rooms, record the id of the
// object that fell in the hole into the wTmpcfc0.fallDownHoleEvent buffer. Reached only by
// a static `call` from interactionCode0f state 0; not independently hookable.
static void fallDownHole_checkUpdateHoleEvent(GB *gb, uint16_t sp0_) {
  BASE(interactionCode0f);
  CYC(b_+115, b_+118); A = mem_rd(gb, wActiveRoom);
  CYC(b_+118, b_+119); E = A;
  CYC(b_+119, b_+122); SET_HL(specialHoleRooms_bank08);
  CALL_C(b_+122, lookupKey_hook, SYM(lookupKey), b_+125);
  if (!(F & FC)) {
    CYCT(b_+125, b_+126); ret_effect(gb); return;
  }
  CYC(b_+125, b_+126);
  CYC(b_+126, b_+127); B = A;
  CYC(b_+127, b_+130); A = mem_rd(gb, wActiveGroup);
  CYC(b_+130, b_+131); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+131, b_+132); ret_effect(gb); return;
  }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+135); SET_HL(wTmpcfc0_fallDownHoleEvent_cfd8);
  CYC(b_+135, b_+137); B = 0x04;
  for (;;) {
    CYC(b_+137, b_+138); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+138, b_+140); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYC(b_+140, b_+142);
      CYC(b_+142, b_+144); E = INTERACTION_BASE + OBJ_COUNTER2;
      CYC(b_+144, b_+145); A = mem_rd(gb, DE);
      CYC(b_+145, b_+146); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(b_+146, b_+147); E = alu_dec8(gb, E);
      CYC(b_+147, b_+148); A = mem_rd(gb, DE);
      CYC(b_+148, b_+149); mem_wr(gb, HL, A);
      CYC(b_+149, b_+150); ret_effect(gb);
      return;
    }
    CYCT(b_+140, b_+142);
    CYC(b_+150, b_+151); L = alu_inc8(gb, L);
    CYC(b_+151, b_+152); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(b_+152, b_+154);
      break;
    }
    CYCT(b_+152, b_+154);
  }
  CYC(b_+154, b_+155); ret_effect(gb);
}

void interactionCode0f_hook(GB *gb) {
  BASE(interactionCode0f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (fallDownHole_jumpTable(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+51) { goto state1; }
    else if (jt_ == b_+99) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13);
  CALL_C(b_+13, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x0f);
  CYC(b_+28, b_+29); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(b_+29, b_+31); goto fallDownHole;
  }
  CYC(b_+29, b_+31);
  // @dust
  CALL_C(b_+31, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+34);
  CYC(b_+34, b_+37); objectSetVisible80_hook(gb);
  return;

fallDownHole:
  CYC(b_+37, b_+38); E = alu_inc8(gb, E);
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_rlca(gb);
  CYC(b_+40, b_+42); A = 0x59;
  if (!(F & FC)) {
    CALL_C_CC(b_+42, playSound_b00_hook, SYM(playSound_b00), b_+45);
  } else {
    CYC(b_+42, b_+45);
  }
  CYC(b_+45, b_+48); push_effect(gb, b_+48); fallDownHole_checkUpdateHoleEvent(gb, sp0_);
  CYC(b_+48, b_+51); objectSetVisible83_hook(gb);
  return;

state1:
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+54, b_+56); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+56, b_+58); goto delete;
  }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+61, b_+63); H8(hFF8F) = A;
  CYC(b_+63, b_+65); alu_add(gb, 0x05);
  CYC(b_+65, b_+67); alu_and(gb, 0xf0);
  CYC(b_+67, b_+69); alu_add(gb, 0x08);
  CYC(b_+69, b_+70); B = A;
  CYC(b_+70, b_+71); L = alu_inc8(gb, L);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL);
  CYC(b_+72, b_+74); H8(hFF8E) = A;
  CYC(b_+74, b_+76); alu_and(gb, 0xf0);
  CYC(b_+76, b_+78); alu_add(gb, 0x08);
  CYC(b_+78, b_+79); C = A;
  CYC(b_+79, b_+80); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+80, b_+82); goto moveTowardsCenter;
  }
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+84); A = H8(hFF8F);
  CYC(b_+84, b_+85); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+85, b_+87); goto animate;
  }
  CYC(b_+85, b_+87);

moveTowardsCenter:
  CALL_C(b_+87, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+90);
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CALL_C(b_+93, objectApplySpeed_hook, SYM(objectApplySpeed), b_+96);

animate:
  CYC(b_+96, b_+99); interactionAnimate_hook(gb);
  return;

state2:
  CYC(b_+99, b_+100); H = D;
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_xor(gb, 0x80);
  CYC(b_+105, b_+106); mem_wr(gb, HL, A);
  CYC(b_+106, b_+108); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+108, b_+110); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+110, b_+112); goto animate;
  }
  CYC(b_+110, b_+112);

delete:
  CYC(b_+112, b_+115); interactionDelete_hook(gb);
}

void clearFallDownHoleEventBuffer_hook(GB *gb) {
  BASE(clearFallDownHoleEventBuffer);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_fallDownHoleEvent_cfd8);
  CYC(b_+3, b_+5); B = 0x08;
  CYC(b_+5, b_+7); A = 0xff;
  CYC(b_+7, SYM(interactionCode10)); fillMemory_hook(gb);
}
