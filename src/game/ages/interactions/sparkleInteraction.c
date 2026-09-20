#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t sparkleInteraction_jump_table(GB *gb) {
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

// INTERAC_SPARKLE
void interactionCode84_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x653f, checkInteractionState_hook, 0x23fe, 0x6542);
  if (!(F & FZ)) { CYCT(0x6542, 0x6544); goto state1; } // jr nz
  CYC(0x6542, 0x6544);
  CALL_C(0x6544, interactionInitGraphics_hook, 0x15fb, 0x6547);
  CALL_C(0x6547, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x654a);
  CYC(0x654a, 0x654c); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x654c, 0x654d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x654d, 0x654f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x654f, 0x6550); A = mem_rd(gb, DE);
  {
    CYC(0x6550, 0x6551); push_effect(gb, 0x6551);
    uint16_t target = sparkleInteraction_jump_table(gb);
    if (target == 0x657a) goto initSubid00;
    if (target == 0x6580) goto lowDrawPriority;
    if (target == 0x6583) goto highDrawPriority;
    if (target == 0x6586) goto initSubid0b;
    if (target == 0x6591) goto initSubid0c;
    // target == 0x6571 falls straight through to initSubid0a
  }

// interactionCode84@initSubid0a
  CYC(0x6571, 0x6572); H = D;
  CYC(0x6572, 0x6574); L = INTERACTION_BASE + OBJ_SPEED; // Interaction.speed
  CYC(0x6574, 0x6575); A = mem_rd(gb, HL);
  CYC(0x6575, 0x6576); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6576, 0x6578); goto initSubid00; } // jr nz
  CYC(0x6576, 0x6578);
  CYC(0x6578, 0x657a); mem_wr(gb, HL, 0x78);

initSubid00:
  CYC(0x657a, 0x657b); E = alu_inc8(gb, E); // inc e
  CYC(0x657b, 0x657c); A = mem_rd(gb, DE);
  CYC(0x657c, 0x657d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x657d, 0x6580); objectSetVisible81_hook(gb); return; } // jp nz
  CYC(0x657d, 0x6580);

lowDrawPriority:
  CYC(0x6580, 0x6583); objectSetVisible82_hook(gb); return; // jp

highDrawPriority:
  CYC(0x6583, 0x6586); objectSetVisible80_hook(gb); return; // jp

initSubid0b:
  CYC(0x6586, 0x6587); H = D;
  CYC(0x6587, 0x6589); L = INTERACTION_BASE + OBJ_SPEED; // Interaction.speedY low byte
  CYC(0x6589, 0x658b); mem_wr(gb, HL, 0xc0);
  CYC(0x658b, 0x658c); L = alu_inc8(gb, L); // Interaction.speedY high byte
  CYC(0x658c, 0x658e); mem_wr(gb, HL, 0xff);
  CYC(0x658e, 0x6591); objectSetVisible81_hook(gb); return; // jp

initSubid0c:
  CYC(0x6591, 0x6593); A = OBJ_ID; // Object.id
  CALL_C(0x6593, objectGetRelatedObject1Var_hook, 0x2160, 0x6596);
  CYC(0x6596, 0x6598); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6598, 0x6599); A = mem_rd(gb, HL);
  CYC(0x6599, 0x659a); mem_wr(gb, DE, A);
  CYC(0x659a, 0x659c); goto lowDrawPriority; // jr

state1:
  CYC(0x659c, 0x659e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x659e, 0x659f); A = mem_rd(gb, DE);
  {
    CYC(0x659f, 0x65a0); push_effect(gb, 0x65a0);
    uint16_t target = sparkleInteraction_jump_table(gb);
    if (target == 0x65c3) goto runSubid00;
    if (target == 0x65ce) goto animateAndFlickerAndDeleteWhenCounter1Zero;
    if (target == 0x65e1) goto runSubid05;
    if (target == 0x6600) goto runSubid06;
    if (target == 0x65ee) goto runSubid07;
    if (target == 0x65d4) goto animateAndFlicker;
    if (target == 0x6618) goto runSubid0a;
    if (target == 0x6624) goto runSubid0c;
    if (target == 0x663d) goto runSubid0d;
    if (target == 0x65f6) goto runSubid0e;
    // target == 0x65c0 falls straight through to runSubid02
  }

// interactionCode84@runSubid02
  CALL_C(0x65c0, objectApplyComponentSpeed_hook, 0x2008, 0x65c3);

runSubid00:
  CYC(0x65c3, 0x65c5); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x65c5, 0x65c6); A = mem_rd(gb, DE);
  CYC(0x65c6, 0x65c8); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x65c8, 0x65cb); interactionDelete_hook(gb); return; } // jp z
  CYC(0x65c8, 0x65cb);
  CYC(0x65cb, 0x65ce); interactionAnimate_hook(gb); return; // jp

animateAndFlickerAndDeleteWhenCounter1Zero:
  CALL_C(0x65ce, interactionDecCounter1_hook, 0x23cc, 0x65d1);
  if (F & FZ) { CYCT(0x65d1, 0x65d4); interactionDelete_hook(gb); return; } // jp z
  CYC(0x65d1, 0x65d4);

animateAndFlicker:
  CALL_C(0x65d4, interactionAnimate_hook, 0x261b, 0x65d7);
  CYC(0x65d7, 0x65da); A = W8(wFrameCounter);

flicker:
  CYC(0x65da, 0x65db); alu_rrca(gb);
  if (F & FC) { CYCT(0x65db, 0x65de); objectSetInvisible_hook(gb); return; } // jp c
  CYC(0x65db, 0x65de);
  CYC(0x65de, 0x65e1); objectSetVisible_hook(gb); return; // jp

runSubid05:
  CYC(0x65e1, 0x65e3); A = OBJ_YH; // Object.yh
  CALL_C(0x65e3, objectGetRelatedObject1Var_hook, 0x2160, 0x65e6);
  CYC(0x65e6, 0x65e9); SET_BC(0x0800);
  CALL_C(0x65e9, objectTakePositionWithOffset_hook, 0x2277, 0x65ec);
  CYC(0x65ec, 0x65ee); goto animateAndFlickerAndDeleteWhenCounter1Zero; // jr

runSubid07:
  CYC(0x65ee, 0x65f0); A = OBJ_YH; // Object.yh
  CALL_C(0x65f0, objectGetRelatedObject1Var_hook, 0x2160, 0x65f3);
  CALL_C(0x65f3, objectTakePosition_hook, 0x2274, 0x65f6);

runSubid0e:
  CYC(0x65f6, 0x65f9); A = W8(wTmpcfc0_bombUpgradeCutscene_state);
  CYC(0x65f9, 0x65fb); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x65fb, 0x65fe); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x65fb, 0x65fe);
  CYC(0x65fe, 0x6600); goto animateAndFlicker; // jr

runSubid06:
  CYC(0x6600, 0x6603); A = W8(wTmpcbb9);
  CYC(0x6603, 0x6605); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x6605, 0x6608); interactionDelete_hook(gb); return; } // jp z
  CYC(0x6605, 0x6608);

animateFlickerAndTakeRelatedObj1Position:
  CALL_C(0x6608, interactionAnimate_hook, 0x261b, 0x660b);
  CYC(0x660b, 0x660d); A = OBJ_YH; // Object.yh
  CALL_C(0x660d, objectGetRelatedObject1Var_hook, 0x2160, 0x6610);
  CALL_C(0x6610, objectTakePosition_hook, 0x2274, 0x6613);
  CYC(0x6613, 0x6616); A = W8(wIntro_frameCounter);
  CYC(0x6616, 0x6618); goto flicker; // jr

runSubid0a:
  CALL_C(0x6618, objectApplySpeed_hook, 0x201d, 0x661b);
  CALL_C(0x661b, objectCheckWithinScreenBoundary_hook, 0x2184, 0x661e);
  if (F & FC) { CYCT(0x661e, 0x6621); interactionAnimate_hook(gb); return; } // jp c
  CYC(0x661e, 0x6621);
  CYC(0x6621, 0x6624); interactionDelete_hook(gb); return; // jp

runSubid0c:
  CYC(0x6624, 0x6626); A = OBJ_ID; // Object.id
  CALL_C(0x6626, objectGetRelatedObject1Var_hook, 0x2160, 0x6629);
  CYC(0x6629, 0x662b); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x662b, 0x662c); A = mem_rd(gb, DE);
  CYC(0x662c, 0x662d); alu_cp(gb, mem_rd(gb, HL)); // cp (hl)
  if (!(F & FZ)) { CYCT(0x662d, 0x6630); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x662d, 0x6630);
  CALL_C(0x6630, objectTakePosition_hook, 0x2274, 0x6633);
  CYC(0x6633, 0x6636); A = W8(wTmpcfc0);
  CYC(0x6636, 0x6638); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6638, 0x663b); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x6638, 0x663b);
  CYC(0x663b, 0x663d); goto animateAndFlicker; // jr

runSubid0d:
  CYC(0x663d, 0x6640); A = W8(wTmpcbb9);
  CYC(0x6640, 0x6642); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x6642, 0x6645); interactionDelete_hook(gb); return; } // jp z
  CYC(0x6642, 0x6645);
  CYC(0x6645, 0x6647); goto animateFlickerAndTakeRelatedObj1Position; // jr
}
