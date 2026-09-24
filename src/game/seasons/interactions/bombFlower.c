#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/bombFlower.s.
// INTERAC_BOMB_FLOWER

static uint16_t bombFlower_jump_table(GB *gb) {
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

void s_bomb_flower_subid0_hook(GB *gb) {
  BASE(bomb_flower_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bombFlower_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+41) goto state1;
    if (jt_ == b_+52) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+16);
  CYC(b_+16, b_+18); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto L_436e; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); A = 0x49;
  CALL_C(b_+22, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+25);
  if (F & FC) { CYCT(b_+25, b_+27); goto L_436e; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = 0x04;
  CALL_C(b_+29, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+32);
  CALL_C(b_+32, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+35);
  CYC(b_+35, b_+38);
  TAIL(objectSetVisible82);
L_436e:
  CYC(b_+38, b_+41);
  TAIL(interactionDelete);
state1:
  CALL_C(b_+41, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+44);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x00);
  CALL_C(b_+46, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+49);
  CYC(b_+49, b_+52);
  TAIL(objectAddToGrabbableObjectBuffer);
state2:
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); push_effect(gb, b_+56);
  do { uint16_t jt_ = (bombFlower_jump_table(gb));
    if (jt_ == b_+64) goto substate0;
    if (jt_ == b_+91) goto substate1;
    if (jt_ == b_+144) goto substate2;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+64, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+67);
  CYC(b_+67, b_+69); A = 0x1c;
  CYC(b_+69, b_+72); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+72, b_+73); alu_xor(gb, A);
  CYC(b_+73, b_+76); mem_wr(gb, wLinkGrabState2, A);
  CALL_C(b_+76, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+79);
  CALL_C(b_+79, s_objectSetVisible81, SYM(objectSetVisible81), b_+82);
  CALL_C(b_+82, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+85);
  CYC(b_+85, b_+86); C = A;
  CYC(b_+86, b_+88); A = 0xe1;
  CYC(b_+88, b_+91);
  TAIL(setTile);
substate1:
  CYC(b_+91, b_+94); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+94, b_+96); alu_cp(gb, 0x83);
  if (!(F & FZ)) { RET_TAKEN(b_+96); return; }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+100); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+100, b_+101); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+101); return; }
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+104); A = 0x81;
  CYC(b_+104, b_+107); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+107, b_+110); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+110, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+113);
  CYC(b_+113, b_+115); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+115, b_+117); A = 0x02;
  CYC(b_+117, b_+118); mem_wr(gb, DE, A);
  CALL_C(b_+118, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+121);
  CYC(b_+121, b_+123); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+123, b_+125); A = 0x04;
  CYC(b_+125, b_+128); mem_wr(gb, wLinkForceState, A);
  CYC(b_+128, b_+130); A = 0x01;
  CYC(b_+130, b_+133); mem_wr(gb, wcc50, A);
  CYC(b_+133, b_+136); SET_BC(0x003c);
  CALL_C(b_+136, s_showText, SYM(showText), b_+139);
  CYC(b_+139, b_+141); A = 0x49;
  CYC(b_+141, b_+144);
  TAIL(giveTreasure);
substate2:
  CALL_C(b_+144, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+147);
  CYC(b_+147, b_+148); alu_xor(gb, A);
  CYC(b_+148, b_+151); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+151, b_+154); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+154, s_updateLinkLocalRespawnPosition, SYM(updateLinkLocalRespawnPosition), b_+157);
  CYC(b_+157, b_+160);
  TAIL(interactionDelete);
}

void s_bomb_flower_subid1_hook(GB *gb) {
  BASE(bomb_flower_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bombFlower_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+34) goto state1;
    if (jt_ == b_+31) goto state2;
    if (jt_ == b_+40) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18); SET_HL(SYM(bombflower_unblockAutumnTemple));
  CALL_C(b_+18, s_interactionSetScript, SYM(interactionSetScript), b_+21);
  CALL_C(b_+21, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CALL_C(b_+25, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+28);
  CYC(b_+28, b_+31);
  TAIL(objectSetVisible82);
state2:
  CALL_C(b_+31, s_interactionAnimate, SYM(interactionAnimate), b_+34);
state1:
  CALL_C(b_+34, s_interactionAnimate, SYM(interactionAnimate), b_+37);
  CYC(b_+37, b_+40);
  TAIL(interactionRunScript);
state3:
  CALL_C(b_+40, s_objectSetInvisible, SYM(objectSetInvisible), b_+43);
  CYC(b_+43, b_+46);
  TAIL(interactionRunScript);
}

