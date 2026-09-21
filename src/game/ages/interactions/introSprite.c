#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode75_jump_table(GB *gb) {
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

// INTERAC_INTRO_SPRITE
void interactionCode75_hook(GB *gb) {
  BASE(interactionCode75);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode75_jump_table(gb);
    if (target == b_+92) goto state1;
  }

  // interactionCode75@state0
  CALL_C(b_+8, interactionIncState_hook, SYM(interactionIncState), b_+11);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisible82_hook, SYM(objectSetVisible82), b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, DE);
  {
    CYC(b_+20, b_+21); push_effect(gb, b_+21);
    uint16_t target = interactionCode75_jump_table(gb);
    if (target == b_+36) goto subid1Init;
    if (target == b_+55) goto subid2Init;
    if (target == b_+61) goto subid3Init;
    if (target == b_+76) goto subid4Init;
    if (target == b_+82) goto subid6Init;
  }

  // interactionCode75@subid0Init
  // interactionCode75@subid5Init
  RET(b_+35); return;

subid1Init:
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x05);

initSpeedToScrollLeft:
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+45, b_+47); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+49, b_+52); SET_BC((SYM(miscPuzzles_dropSmallKeyHere) + 11));
  CYC(b_+52, b_+55); TAIL(interactionSetPosition); // jp

subid2Init:
  CALL_C(b_+55, objectSetVisible83_hook, SYM(objectSetVisible83), b_+58);
  CYC(b_+58, b_+59); H = D;
  CYC(b_+59, b_+61); goto initSpeedToScrollLeft; // jr

subid3Init:
  CYC(b_+61, b_+64); SET_BC((SYM(companionCallableRooms) + 8));
  CALL_C(b_+64, interactionSetPosition_hook, SYM(interactionSetPosition), b_+67);
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x19);
  CYC(b_+71, b_+73); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x0a); // SPEED_40
  RET(b_+75); return;

subid4Init:
  CYC(b_+76, b_+79); SET_BC(0x1838);
  CYC(b_+79, b_+82); TAIL(interactionSetPosition); // jp

subid6Init:
  CYC(b_+82, b_+83); H = D;
  CYC(b_+83, b_+85); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x1a);
  CYC(b_+87, b_+89); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x0f); // SPEED_60
  RET(b_+91); return;

state1:
  CYC(b_+92, b_+94); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  {
    CYC(b_+95, b_+96); push_effect(gb, b_+96);
    uint16_t target = interactionCode75_jump_table(gb);
    if (target == b_+120) goto runSubid1;
    if (target == b_+150) goto updateSpeed;
    if (target == b_+159) goto runSubid3;
    if (target == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; }
    if (target == b_+171) goto runSubid6;
  }

  // interactionCode75@runSubid0
  // interactionCode75@runSubid5
  CYC(b_+110, b_+113); A = W8(wIntro_cbba);
  CYC(b_+113, b_+114); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+114, b_+117); interactionAnimate_hook(gb); return; } // jp z
  CYC(b_+114, b_+117);
  CYC(b_+117, b_+120); TAIL(interactionDelete); // jp

runSubid1:
  CALL_C(b_+120, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+123);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); goto updateSpeed; } // jr nz
  CYC(b_+123, b_+125);
  CALL_C(b_+125, interactionAnimate_hook, SYM(interactionAnimate), b_+128);
  CYC(b_+128, b_+129); H = D;
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+133); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+133, b_+135); goto updateSpeed; } // jr z
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x00);
  CYC(b_+137, b_+139); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+139, b_+140); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+140, b_+142); goto updateSpeed; } // jr nz
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+144); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+144, b_+145); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+145, b_+147); A = 0x04;
  CALL_C(b_+147, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+150);

updateSpeed:
  CYC(b_+150, b_+153); SET_HL(wIntro_cbb6);
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+155); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+155); return; } // ret z
  CYC(b_+155, b_+156);
  CYC(b_+156, b_+159); TAIL(objectApplySpeed); // jp

runSubid3:
  CALL_C(b_+159, interactionAnimate_hook, SYM(interactionAnimate), b_+162);
  CYC(b_+162, b_+165); A = W8(wIntro_frameCounter);
  CYC(b_+165, b_+167); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+167); return; } // ret nz
  CYC(b_+167, b_+168);
  CYC(b_+168, b_+171); TAIL(objectApplySpeed); // jp

runSubid6:
  CYC(b_+171, b_+174); A = W8(wTmpcbba);
  CYC(b_+174, b_+175); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+175, b_+178); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+175, b_+178);
  CYC(b_+178, b_+181); A = W8(wPaletteThread_mode);
  CYC(b_+181, b_+182); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+182); return; } // ret nz
  CYC(b_+182, b_+183);
  CALL_C(b_+183, interactionAnimate_hook, SYM(interactionAnimate), b_+186);
  CYC(b_+186, b_+189); TAIL(objectApplySpeed); // jp
}
