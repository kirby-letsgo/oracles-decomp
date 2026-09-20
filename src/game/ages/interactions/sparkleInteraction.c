#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode84), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode84), (from), (to), true)

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
  BASE(interactionCode84);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);
  CALL_C(b_+5, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+8);
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  {
    CYC(b_+17, b_+18); push_effect(gb, b_+18);
    uint16_t target = sparkleInteraction_jump_table(gb);
    if (target == b_+59) goto initSubid00;
    if (target == b_+65) goto lowDrawPriority;
    if (target == b_+68) goto highDrawPriority;
    if (target == b_+71) goto initSubid0b;
    if (target == b_+82) goto initSubid0c;
    // target == 0x6571 falls straight through to initSubid0a
  }

// interactionCode84@initSubid0a
  CYC(b_+50, b_+51); H = D;
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_SPEED; // Interaction.speed
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto initSubid00; } // jr nz
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x78);

initSubid00:
  CYC(b_+59, b_+60); E = alu_inc8(gb, E); // inc e
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+62, b_+65); objectSetVisible81_hook(gb); return; } // jp nz
  CYC(b_+62, b_+65);

lowDrawPriority:
  CYC(b_+65, b_+68); objectSetVisible82_hook(gb); return; // jp

highDrawPriority:
  CYC(b_+68, b_+71); objectSetVisible80_hook(gb); return; // jp

initSubid0b:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_SPEED; // Interaction.speedY low byte
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0xc0);
  CYC(b_+76, b_+77); L = alu_inc8(gb, L); // Interaction.speedY high byte
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0xff);
  CYC(b_+79, b_+82); objectSetVisible81_hook(gb); return; // jp

initSubid0c:
  CYC(b_+82, b_+84); A = OBJ_ID; // Object.id
  CALL_C(b_+84, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+87);
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+93); goto lowDrawPriority; // jr

state1:
  CYC(b_+93, b_+95); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  {
    CYC(b_+96, b_+97); push_effect(gb, b_+97);
    uint16_t target = sparkleInteraction_jump_table(gb);
    if (target == b_+132) goto runSubid00;
    if (target == b_+143) goto animateAndFlickerAndDeleteWhenCounter1Zero;
    if (target == b_+162) goto runSubid05;
    if (target == b_+193) goto runSubid06;
    if (target == b_+175) goto runSubid07;
    if (target == b_+149) goto animateAndFlicker;
    if (target == b_+217) goto runSubid0a;
    if (target == b_+229) goto runSubid0c;
    if (target == b_+254) goto runSubid0d;
    if (target == b_+183) goto runSubid0e;
    // target == 0x65c0 falls straight through to runSubid02
  }

// interactionCode84@runSubid02
  CALL_C(b_+129, objectApplyComponentSpeed_hook, SYM(objectApplyComponentSpeed), b_+132);

runSubid00:
  CYC(b_+132, b_+134); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+134, b_+135); A = mem_rd(gb, DE);
  CYC(b_+135, b_+137); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+137, b_+140); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+137, b_+140);
  CYC(b_+140, b_+143); interactionAnimate_hook(gb); return; // jp

animateAndFlickerAndDeleteWhenCounter1Zero:
  CALL_C(b_+143, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+146);
  if (F & FZ) { CYCT(b_+146, b_+149); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+146, b_+149);

animateAndFlicker:
  CALL_C(b_+149, interactionAnimate_hook, SYM(interactionAnimate), b_+152);
  CYC(b_+152, b_+155); A = W8(wFrameCounter);

flicker:
  CYC(b_+155, b_+156); alu_rrca(gb);
  if (F & FC) { CYCT(b_+156, b_+159); objectSetInvisible_hook(gb); return; } // jp c
  CYC(b_+156, b_+159);
  CYC(b_+159, b_+162); objectSetVisible_hook(gb); return; // jp

runSubid05:
  CYC(b_+162, b_+164); A = OBJ_YH; // Object.yh
  CALL_C(b_+164, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+167);
  CYC(b_+167, b_+170); SET_BC((SYM(loadTilesetHlpr) + 2));
  CALL_C(b_+170, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+173);
  CYC(b_+173, b_+175); goto animateAndFlickerAndDeleteWhenCounter1Zero; // jr

runSubid07:
  CYC(b_+175, b_+177); A = OBJ_YH; // Object.yh
  CALL_C(b_+177, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+180);
  CALL_C(b_+180, objectTakePosition_hook, SYM(objectTakePosition), b_+183);

runSubid0e:
  CYC(b_+183, b_+186); A = W8(wTmpcfc0_bombUpgradeCutscene_state);
  CYC(b_+186, b_+188); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+188, b_+191); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+188, b_+191);
  CYC(b_+191, b_+193); goto animateAndFlicker; // jr

runSubid06:
  CYC(b_+193, b_+196); A = W8(wTmpcbb9);
  CYC(b_+196, b_+198); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+198, b_+201); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+198, b_+201);

animateFlickerAndTakeRelatedObj1Position:
  CALL_C(b_+201, interactionAnimate_hook, SYM(interactionAnimate), b_+204);
  CYC(b_+204, b_+206); A = OBJ_YH; // Object.yh
  CALL_C(b_+206, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+209);
  CALL_C(b_+209, objectTakePosition_hook, SYM(objectTakePosition), b_+212);
  CYC(b_+212, b_+215); A = W8(wIntro_frameCounter);
  CYC(b_+215, b_+217); goto flicker; // jr

runSubid0a:
  CALL_C(b_+217, objectApplySpeed_hook, SYM(objectApplySpeed), b_+220);
  CALL_C(b_+220, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+223);
  if (F & FC) { CYCT(b_+223, b_+226); interactionAnimate_hook(gb); return; } // jp c
  CYC(b_+223, b_+226);
  CYC(b_+226, b_+229); interactionDelete_hook(gb); return; // jp

runSubid0c:
  CYC(b_+229, b_+231); A = OBJ_ID; // Object.id
  CALL_C(b_+231, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+234);
  CYC(b_+234, b_+236); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+236, b_+237); A = mem_rd(gb, DE);
  CYC(b_+237, b_+238); alu_cp(gb, mem_rd(gb, HL)); // cp (hl)
  if (!(F & FZ)) { CYCT(b_+238, b_+241); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+238, b_+241);
  CALL_C(b_+241, objectTakePosition_hook, SYM(objectTakePosition), b_+244);
  CYC(b_+244, b_+247); A = W8(wTmpcfc0);
  CYC(b_+247, b_+249); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+249, b_+252); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+249, b_+252);
  CYC(b_+252, b_+254); goto animateAndFlicker; // jr

runSubid0d:
  CYC(b_+254, b_+257); A = W8(wTmpcbb9);
  CYC(b_+257, b_+259); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+259, b_+262); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+259, b_+262);
  CYC(b_+262, SYM(interactionCode86)); goto animateFlickerAndTakeRelatedObj1Position; // jr
}
