#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/sparkle.s.
// INTERAC_SPARKLE

static uint16_t sparkle_jump_table(GB *gb) {
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

// INTERAC_SPARKLE
void s_interactionCode84_hook(GB *gb) {
  BASE(interactionCode84);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+8);
  CALL_C(b_+8, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (sparkle_jump_table(gb));
    if (jt_ == b_+38) goto initSubid00;
    if (jt_ == b_+54) goto mediumDrawPriority;
    if (jt_ == b_+46) goto initSubid02;
    if (jt_ == b_+57) goto initSubid03;
    if (jt_ == b_+68) goto lowDrawPriority;
    if (jt_ == b_+43) goto highDrawPriority;
    if (jt_ == b_+71) goto initSubid08;
    HANDOFF(HL);
  } while (0);
initSubid00:
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x78);
highDrawPriority:
  CYC(b_+43, b_+46);
  TAIL(objectSetVisible82);
initSubid02:
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+49); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x80);
  CYC(b_+51, b_+52); L = alu_inc8(gb, L);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0xff);
mediumDrawPriority:
  CYC(b_+54, b_+57);
  TAIL(objectSetVisible81);
initSubid03:
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0xc0);
  CYC(b_+62, b_+63); L = alu_inc8(gb, L);
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0xff);
  CYC(b_+65, b_+68);
  TAIL(objectSetVisible81);
lowDrawPriority:
  CYC(b_+68, b_+71);
  TAIL(objectSetVisible80);
initSubid08:
  CYC(b_+71, b_+72); H = D;
  CYC(b_+72, b_+74); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0xc2);
  CYC(b_+76, b_+79);
  TAIL(objectSetVisible80);
state1:
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); push_effect(gb, b_+83);
  do { uint16_t jt_ = (sparkle_jump_table(gb));
    if (jt_ == b_+103) goto runSubid00;
    if (jt_ == b_+125) goto runSubid01;
    if (jt_ == b_+122) goto runSubid02;
    if (jt_ == b_+136) goto runSubid04;
    if (jt_ == b_+156) goto runSubid06;
    if (jt_ == b_+180) goto runSubid08;
    if (jt_ == b_+146) goto runSubid09;
    HANDOFF(HL);
  } while (0);
runSubid00:
  CALL_C(b_+103, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+106);
  if (F & FZ) { CYCT(b_+106, b_+109); TAIL(interactionDelete); }
  CYC(b_+106, b_+109);
animateAndFlicker:
  CALL_C(b_+109, s_interactionAnimate, SYM(interactionAnimate), b_+112);
  CYC(b_+112, b_+115); A = mem_rd(gb, wFrameCounter);
flicker4:
  CYC(b_+115, b_+116); alu_rrca(gb);
  if (F & FC) { CYCT(b_+116, b_+119); TAIL(objectSetInvisible); }
  CYC(b_+116, b_+119);
  CYC(b_+119, b_+122);
  TAIL(objectSetVisible);
runSubid02:
  CALL_C(b_+122, s_objectApplyComponentSpeed, SYM(objectApplyComponentSpeed), b_+125);
runSubid01:
  CYC(b_+125, b_+127); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+127, b_+128); A = mem_rd(gb, DE);
  CYC(b_+128, b_+130); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+130, b_+133); TAIL(interactionDelete); }
  CYC(b_+130, b_+133);
  CYC(b_+133, b_+136);
  TAIL(interactionAnimate);
runSubid04:
  CYC(b_+136, b_+139); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+139, b_+141); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+141, b_+144); TAIL(interactionDelete); }
  CYC(b_+141, b_+144);
  CYC(b_+144, b_+146);
  goto animateAndFlicker;
runSubid09:
  CYC(b_+146, b_+149); A = mem_rd(gb, wTmpcbb9);
  CYC(b_+149, b_+151); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+151, b_+154); TAIL(interactionDelete); }
  CYC(b_+151, b_+154);
  CYC(b_+154, b_+156);
  goto animateFlickerAndTakeRelatedObj1Position;
runSubid06:
  CYC(b_+156, b_+159); A = mem_rd(gb, wTmpcbb9);
  CYC(b_+159, b_+161); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+161, b_+164); TAIL(interactionDelete); }
  CYC(b_+161, b_+164);
animateFlickerAndTakeRelatedObj1Position:
  CALL_C(b_+164, s_interactionAnimate, SYM(interactionAnimate), b_+167);
  CYC(b_+167, b_+169); A = 0x0b;
  CALL_C(b_+169, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+172);
  CALL_C(b_+172, s_objectTakePosition, SYM(objectTakePosition), b_+175);
  CYC(b_+175, b_+178); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+178, b_+180);
  goto flicker4;
runSubid08:
  CYC(b_+180, b_+182); A = 0x0b;
  CALL_C(b_+182, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+185);
  CALL_C(b_+185, s_objectTakePosition, SYM(objectTakePosition), b_+188);
  CYC(b_+188, b_+190);
  goto runSubid00;
}

