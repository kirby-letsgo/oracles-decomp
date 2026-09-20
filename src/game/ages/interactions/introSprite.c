#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x5e45, 0x5e47); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5e47, 0x5e48); A = mem_rd(gb, DE);
  {
    CYC(0x5e48, 0x5e49); push_effect(gb, 0x5e49);
    uint16_t target = interactionCode75_jump_table(gb);
    if (target == 0x5ea1) goto state1;
  }

  // interactionCode75@state0
  CALL_C(0x5e4d, interactionIncState_hook, 0x23e0, 0x5e50);
  CALL_C(0x5e50, interactionInitGraphics_hook, 0x15fb, 0x5e53);
  CALL_C(0x5e53, objectSetVisible82_hook, 0x1e69, 0x5e56);
  CYC(0x5e56, 0x5e58); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5e58, 0x5e59); A = mem_rd(gb, DE);
  {
    CYC(0x5e59, 0x5e5a); push_effect(gb, 0x5e5a);
    uint16_t target = interactionCode75_jump_table(gb);
    if (target == 0x5e69) goto subid1Init;
    if (target == 0x5e7c) goto subid2Init;
    if (target == 0x5e82) goto subid3Init;
    if (target == 0x5e91) goto subid4Init;
    if (target == 0x5e97) goto subid6Init;
  }

  // interactionCode75@subid0Init
  // interactionCode75@subid5Init
  RET(0x5e68); return;

subid1Init:
  CYC(0x5e69, 0x5e6a); H = D;
  CYC(0x5e6a, 0x5e6c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5e6c, 0x5e6e); mem_wr(gb, HL, 0x05);

initSpeedToScrollLeft:
  CYC(0x5e6e, 0x5e70); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5e70, 0x5e72); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(0x5e72, 0x5e74); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5e74, 0x5e76); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x5e76, 0x5e79); SET_BC(0x7080);
  CYC(0x5e79, 0x5e7c); interactionSetPosition_hook(gb); return; // jp

subid2Init:
  CALL_C(0x5e7c, objectSetVisible83_hook, 0x1e72, 0x5e7f);
  CYC(0x5e7f, 0x5e80); H = D;
  CYC(0x5e80, 0x5e82); goto initSpeedToScrollLeft; // jr

subid3Init:
  CYC(0x5e82, 0x5e85); SET_BC(0x4c6c);
  CALL_C(0x5e85, interactionSetPosition_hook, 0x2773, 0x5e88);
  CYC(0x5e88, 0x5e8a); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5e8a, 0x5e8c); mem_wr(gb, HL, 0x19);
  CYC(0x5e8c, 0x5e8e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5e8e, 0x5e90); mem_wr(gb, HL, 0x0a); // SPEED_40
  RET(0x5e90); return;

subid4Init:
  CYC(0x5e91, 0x5e94); SET_BC(0x1838);
  CYC(0x5e94, 0x5e97); interactionSetPosition_hook(gb); return; // jp

subid6Init:
  CYC(0x5e97, 0x5e98); H = D;
  CYC(0x5e98, 0x5e9a); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5e9a, 0x5e9c); mem_wr(gb, HL, 0x1a);
  CYC(0x5e9c, 0x5e9e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5e9e, 0x5ea0); mem_wr(gb, HL, 0x0f); // SPEED_60
  RET(0x5ea0); return;

state1:
  CYC(0x5ea1, 0x5ea3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5ea3, 0x5ea4); A = mem_rd(gb, DE);
  {
    CYC(0x5ea4, 0x5ea5); push_effect(gb, 0x5ea5);
    uint16_t target = interactionCode75_jump_table(gb);
    if (target == 0x5ebd) goto runSubid1;
    if (target == 0x5edb) goto updateSpeed;
    if (target == 0x5ee4) goto runSubid3;
    if (target == 0x261b) { interactionAnimate_hook(gb); return; }
    if (target == 0x5ef0) goto runSubid6;
  }

  // interactionCode75@runSubid0
  // interactionCode75@runSubid5
  CYC(0x5eb3, 0x5eb6); A = W8(wIntro_cbba);
  CYC(0x5eb6, 0x5eb7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5eb7, 0x5eba); interactionAnimate_hook(gb); return; } // jp z
  CYC(0x5eb7, 0x5eba);
  CYC(0x5eba, 0x5ebd); interactionDelete_hook(gb); return; // jp

runSubid1:
  CALL_C(0x5ebd, checkInteractionSubstate_hook, 0x2403, 0x5ec0);
  if (!(F & FZ)) { CYCT(0x5ec0, 0x5ec2); goto updateSpeed; } // jr nz
  CYC(0x5ec0, 0x5ec2);
  CALL_C(0x5ec2, interactionAnimate_hook, 0x261b, 0x5ec5);
  CYC(0x5ec5, 0x5ec6); H = D;
  CYC(0x5ec6, 0x5ec8); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5ec8, 0x5ec9); A = mem_rd(gb, HL);
  CYC(0x5ec9, 0x5eca); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5eca, 0x5ecc); goto updateSpeed; } // jr z
  CYC(0x5eca, 0x5ecc);
  CYC(0x5ecc, 0x5ece); mem_wr(gb, HL, 0x00);
  CYC(0x5ece, 0x5ed0); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5ed0, 0x5ed1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x5ed1, 0x5ed3); goto updateSpeed; } // jr nz
  CYC(0x5ed1, 0x5ed3);
  CYC(0x5ed3, 0x5ed5); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5ed5, 0x5ed6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x5ed6, 0x5ed8); A = 0x04;
  CALL_C(0x5ed8, interactionSetAnimation_hook, 0x262e, 0x5edb);

updateSpeed:
  CYC(0x5edb, 0x5ede); SET_HL(wIntro_cbb6);
  CYC(0x5ede, 0x5edf); A = mem_rd(gb, HL);
  CYC(0x5edf, 0x5ee0); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5ee0); return; } // ret z
  CYC(0x5ee0, 0x5ee1);
  CYC(0x5ee1, 0x5ee4); objectApplySpeed_hook(gb); return; // jp

runSubid3:
  CALL_C(0x5ee4, interactionAnimate_hook, 0x261b, 0x5ee7);
  CYC(0x5ee7, 0x5eea); A = W8(wIntro_frameCounter);
  CYC(0x5eea, 0x5eec); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x5eec); return; } // ret nz
  CYC(0x5eec, 0x5eed);
  CYC(0x5eed, 0x5ef0); objectApplySpeed_hook(gb); return; // jp

runSubid6:
  CYC(0x5ef0, 0x5ef3); A = W8(wTmpcbba);
  CYC(0x5ef3, 0x5ef4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5ef4, 0x5ef7); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x5ef4, 0x5ef7);
  CYC(0x5ef7, 0x5efa); A = W8(wPaletteThread_mode);
  CYC(0x5efa, 0x5efb); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5efb); return; } // ret nz
  CYC(0x5efb, 0x5efc);
  CALL_C(0x5efc, interactionAnimate_hook, 0x261b, 0x5eff);
  CYC(0x5eff, 0x5f02); objectApplySpeed_hook(gb); return; // jp
}
