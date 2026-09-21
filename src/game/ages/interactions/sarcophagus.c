#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t sarcophagus_jump_table(GB *gb) {
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

// INTERAC_SARCOPHAGUS
void interactionCode82_hook(GB *gb) {
  BASE(interactionCode82);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = sarcophagus_jump_table(gb);
    if (target == b_+65) goto state1;
    if (target == b_+74) goto state2;
    if (target == b_+166) goto state3;
  }

  // interactionCode82@state0
  CALL_C(b_+12, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+15);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); goto breakOut; } // jp nz
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto afterRoomFlagCheck; } // jr z
  CYC(b_+24, b_+26);
  CALL_C(b_+26, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+29);
  CYC(b_+29, b_+31); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+34); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+31, b_+34);

afterRoomFlagCheck:
  CALL_C(b_+34, interactionIncState_hook, SYM(interactionIncState), b_+37);
  CYC(b_+37, b_+39); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x10);
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x08);
  CALL_C(b_+45, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+48);
  CYC(b_+48, b_+50); H = wRoomLayout >> 8;
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x00);
  CYC(b_+52, b_+53); A = L;
  CYC(b_+53, b_+55); alu_sub(gb, 0x10);
  CYC(b_+55, b_+56); L = A;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x00);
  CYC(b_+58, b_+60); H = wRoomCollisions >> 8;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x0f);
  CYC(b_+62, b_+65); TAIL(objectSetVisible83); // jp

state1:
  CYC(b_+65, b_+68); A = W8(wBraceletLevel);
  CYC(b_+68, b_+70); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+70); return; } // ret c
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+74); TAIL(objectAddToGrabbableObjectBuffer); // jp

state2:
  CYC(b_+74, b_+75); E = alu_inc8(gb, E);
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  {
    CYC(b_+76, b_+77); push_effect(gb, b_+77);
    uint16_t target = sarcophagus_jump_table(gb);
    if (target == b_+131) goto substate1Holding;
    if (target == b_+132) goto substate2JustReleased;
    if (target == b_+142) goto breakOut;
  }

  // interactionCode82@substate0_justGrabbed
  CALL_C(b_+85, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+88);
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+92); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+92, b_+94); goto placeGrabbedTile; } // jr z
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+95); A = alu_dec8(gb, A);
  CYC(b_+95, b_+97); A = 0x4d; // SND_SOLVEPUZZLE
  if (F & FZ) CALL_C_CC(b_+97, playSound_b00_hook, SYM(playSound_b00), b_+100); else CYC(b_+97, b_+100);
  CALL_C(b_+100, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+103);
  CYC(b_+103, b_+105); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)

placeGrabbedTile:
  CALL_C(b_+105, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+108);
  CYC(b_+108, b_+109); push_effect(gb, AF);
  CALL_C(b_+109, getTileIndexFromRoomLayoutBuffer_hook, SYM(getTileIndexFromRoomLayoutBuffer), b_+112);
  CALL_C(b_+112, setTile_hook, SYM(setTile), b_+115);
  CYC(b_+115, b_+116); SET_AF(pop_effect(gb));
  CYC(b_+116, b_+118); alu_sub(gb, 0x10);
  CALL_C(b_+118, getTileIndexFromRoomLayoutBuffer_hook, SYM(getTileIndexFromRoomLayoutBuffer), b_+121);
  CALL_C(b_+121, setTile_hook, SYM(setTile), b_+124);
  CYC(b_+124, b_+125); alu_xor(gb, A);
  CYC(b_+125, b_+128); W8(wLinkGrabState2) = A;
  CYC(b_+128, b_+131); TAIL(objectSetVisiblec1); // jp

substate1Holding:
  RET(b_+131); return; // ret

substate2JustReleased:
  CYC(b_+132, b_+133); H = D;
  CYC(b_+133, b_+135); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+135, b_+137); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CYC(b_+137, b_+139); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+139, b_+141); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+141); return; } // ret nz
  CYC(b_+141, b_+142);

breakOut:
  CYC(b_+142, b_+143); H = D;
  CYC(b_+143, b_+145); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x03);
  CYC(b_+147, b_+149); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+149, b_+151); mem_wr(gb, HL, 0x02);
  CYC(b_+151, b_+153); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+153, b_+155); A = 0x0c;
  CYC(b_+155, b_+156); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+156, b_+157); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+157, b_+159); mem_wr(gb, HL, 0x40); // [oamTileIndexBase] = $40
  CALL_C(b_+159, objectSetVisible83_hook, SYM(objectSetVisible83), b_+162);
  CYC(b_+162, b_+163); alu_xor(gb, A);
  CYC(b_+163, b_+166); TAIL(interactionSetAnimation); // jp

state3:
  CALL_C(b_+166, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+169);
  CYC(b_+169, b_+171); A = 0x73; // SND_KILLENEMY
  if (F & FZ) CALL_C_CC(b_+171, playSound_b00_hook, SYM(playSound_b00), b_+174); else CYC(b_+171, b_+174);
  CYC(b_+174, b_+176); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+178); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+178, b_+181); interactionAnimate_hook(gb); return; } // jp nz
  CYC(b_+178, b_+181);
  CYC(b_+181, b_+184); TAIL(interactionDelete); // jp
}
