#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodee1), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodee1), (from), (to), true)

// object_code/ages/interactions/timeportalSpawner.s (INTERAC_TIMEPORTAL_SPAWNER), bank $10.

void markSpotDiscovered_hook(GB *gb);

static uint16_t timeportalSpawner_jump_table(GB *gb) {
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

void interactionCodee1_hook(GB *gb) {
  BASE(interactionCodee1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (timeportalSpawner_jump_table(gb));
    if (jt_ == b_+12) { goto state3; }
    else if (jt_ == b_+59) { goto state0; }
    else if (jt_ == b_+134) { goto state1; }
    else if (jt_ == b_+148) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state3:
  CALL_C(b_+12, objectSetVisible83_hook, SYM(objectSetVisible83), b_+15);
  CYC(b_+15, b_+17); B = 0x01;
  CALL_C(b_+17, objectFlickerVisibility_hook, SYM(objectFlickerVisibility), b_+20);
  CALL_C(b_+20, interactionAnimate_hook, SYM(interactionAnimate), b_+23);
  CYC(b_+23, b_+26); push_effect(gb, b_+26);
  markSpotDiscovered_hook(gb);
  CYC(b_+26, b_+29); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+29, b_+30); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CALL_C(b_+31, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+34);
  if (!(F & FC)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CALL_C(b_+35, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+38);
  if (!(F & FC)) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+44); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(b_+44, b_+46);
    goto interBankToBeginTimewarp;
  }
  CYC(b_+44, b_+46);
  CALL_C(b_+46, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
interBankToBeginTimewarp:
  CYC(b_+51, b_+54); SET_HL(SYM(interactionBeginTimewarp));
  CYC(b_+54, b_+56); E = 0x10;
  CYC(b_+56, b_+59);
  interBankCall_hook(gb);
  return;
state0:
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+64); alu_and(gb, 0x0f);
  CYC(b_+64, b_+65); push_effect(gb, b_+65);
  do { uint16_t jt_ = (timeportalSpawner_jump_table(gb));
    if (jt_ == b_+71) { goto subid1Init; }
    else if (jt_ == b_+80) { goto subid2Init; }
    else if (jt_ == b_+92) { goto commonInit; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
subid1Init:
  CYC(b_+71, b_+73); A = 0x12;
  CALL_C(b_+73, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+76);
  if (!(F & FZ)) {
    CYCT(b_+76, b_+78);
    goto commonInit;
  }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80);
  goto setSubidBit7;
subid2Init:
  CYC(b_+80, b_+82); A = 0x19;
  CALL_C(b_+82, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+85);
  if (F & FC) {
    CYCT(b_+85, b_+87);
    goto commonInit;
  }
  CYC(b_+85, b_+87);
setSubidBit7:
  CYC(b_+87, b_+88); H = D;
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+90, b_+92); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
commonInit:
  CALL_C(b_+92, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+95);
  CYC(b_+95, b_+97); alu_cp(gb, 0xd7);
  if (!(F & FZ)) { RET_TAKEN(b_+97); return; }
  CYC(b_+97, b_+98);
  CALL_C(b_+98, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+101);
  CALL_C(b_+101, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+104);
  CYC(b_+104, b_+106); A = 0x02;
  CALL_C(b_+106, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+109);
  CYC(b_+109, b_+111); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+111, b_+112); B = mem_rd(gb, HL);
  CYC(b_+112, b_+114); alu_bit(gb, 6, B);
  if (F & FZ) {
    CYCT(b_+114, b_+116);
    goto afterRoomFlagCheck;
  }
  CYC(b_+114, b_+116);
  CALL_C(b_+116, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+119);
  CYC(b_+119, b_+121); alu_and(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+121, b_+123);
    goto afterRoomFlagCheck;
  }
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+125); B = (uint8_t)(B | (1 << 7));
afterRoomFlagCheck:
  CALL_C(b_+125, interactionIncState_hook, SYM(interactionIncState), b_+128);
  CYC(b_+128, b_+130); alu_bit(gb, 7, B);
  if (F & FZ) { RET_TAKEN(b_+130); return; }
  CYC(b_+130, b_+131);
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x03);
  RET(b_+133); return;
state1:
  CYC(b_+134, b_+137); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+137, b_+138); A = alu_dec8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+138); return; }
  CYC(b_+138, b_+139);
  CALL_C(b_+139, interactionIncState_hook, SYM(interactionIncState), b_+142);
  CALL_C(b_+142, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+145);
  CYC(b_+145, b_+147); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));
  RET(b_+147); return;
state2:
  CYC(b_+148, b_+151); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+151, b_+152); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+152); return; }
  CYC(b_+152, b_+153);
  CYC(b_+153, b_+155); A = 0xf1;
  CALL_C(b_+155, playSound_b00_hook, SYM(playSound_b00), b_+158);
  CYC(b_+158, b_+160); A = 0x8d;
  CALL_C(b_+160, playSound_b00_hook, SYM(playSound_b00), b_+163);
  CYC(b_+163, SYM(interactionCodee3));
  interactionIncState_hook(gb);
}

void markSpotDiscovered_hook(GB *gb) {
  BASE(interactionCodee1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+142, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+145);
  CYC(b_+145, b_+147); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));
  RET(b_+147); return;
}
