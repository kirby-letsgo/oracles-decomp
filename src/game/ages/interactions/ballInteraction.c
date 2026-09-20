#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t ballInteraction_jump_table(GB *gb) {
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

// INTERAC_BALL
void interactionCode95_hook(GB *gb) {
  BASE(interactionCode95);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = ballInteraction_jump_table(gb);
    if (target == b_+21) goto state1;
  }

  // interactionCode95@state0
  CALL_C(b_+8, interactionIncState_hook, SYM(interactionIncState), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+15, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+18);
  CYC(b_+18, b_+21); objectSetVisible80_hook(gb); return; // jp

state1:
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  {
    CYC(b_+24, b_+25); push_effect(gb, b_+25);
    uint16_t target = ballInteraction_jump_table(gb);
    if (target == b_+72) goto substate1;
    if (target == b_+136) goto substate2;
  }

  // interactionCode95@substate0
  CYC(b_+31, b_+34); A = W8(wTmpcfc0_genericCutscene_cfd3);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+35); return; } // ret z
  CYC(b_+35, b_+36);
  CALL_C(b_+36, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+39);
  CYC(b_+39, b_+41); B = 0x08; // ANGLE_RIGHT
  CYC(b_+41, b_+42); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+42, b_+44); goto afterAngle; } // jr z
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); B = 0x18; // ANGLE_LEFT

afterAngle:
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+48, b_+49); mem_wr(gb, HL, B);
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CYC(b_+52, b_+54); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto setSpeedZ; } // jr nz
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); SET_BC((SYM(interaction6b_subid0d__state1) + 4));
  CALL_C(b_+59, interactionHSetPosition_hook, SYM(interactionHSetPosition), b_+62);
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0xfa); // -6

setSpeedZ:
  CYC(b_+66, b_+69); SET_BC(0xfe40); // -0x1c0
  CYC(b_+69, b_+72); objectSetSpeedZ_hook(gb); return; // jp

substate1:
  CALL_C(b_+72, objectApplySpeed_hook, SYM(objectApplySpeed), b_+75);
  CYC(b_+75, b_+77); C = 0x20;
  CALL_C(b_+77, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; } // ret nz
  CYC(b_+80, b_+81);

  // Ball has landed
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+86); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+86, b_+88); goto subid2; } // jr z
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+89); A = alu_dec8(gb, A);
  CYC(b_+89, b_+92); SET_BC((SYM(interactionCode91__checkDelete) + 3));
  if (F & FZ) { CYCT(b_+92, b_+94); goto ballLanded; } // jr z
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+96); C = 0x75;

ballLanded:
  CYC(b_+96, b_+97); alu_xor(gb, A);
  CYC(b_+97, b_+100); W8(wTmpcfc0_genericCutscene_cfd3) = A;
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+102, b_+103); mem_wr(gb, DE, A);
  CYC(b_+103, b_+106); interactionSetPosition_hook(gb); return; // jp

subid2:
  // [speedZ] = -[speedZ]/2
  CYC(b_+106, b_+108); L = INTERACTION_BASE + OBJ_SPEED_Z + 1; // Interaction.speedZ+1
  CYC(b_+108, b_+109); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+109, b_+111); A = alu_srl(gb, A);
  CYC(b_+111, b_+112); B = A;
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CYC(b_+113, b_+114); alu_rra(gb);
  CYC(b_+114, b_+115); alu_cpl(gb);
  CYC(b_+115, b_+117); alu_add(gb, 0x01);
  CYC(b_+117, b_+118); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+118, b_+119); A = B;
  CYC(b_+119, b_+120); alu_cpl(gb);
  CYC(b_+120, b_+122); alu_adc(gb, 0x00);
  CYC(b_+122, b_+123); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a

  // Go to substate 2 (stop doing anything) if the ball's Z speed has gone too low
  CYC(b_+123, b_+126); SET_BC(hOamFunc);
  CYC(b_+126, b_+127); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+127, b_+128); H = mem_rd(gb, HL);
  CYC(b_+128, b_+129); L = A;
  CALL_C(b_+129, compareHlToBc_hook, SYM(compareHlToBc), b_+132);
  if (F & FC) { RET_TAKEN(b_+132); return; } // ret c
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+136); interactionIncSubstate_hook(gb); return; // jp

substate2:
  RET(b_+136); return; // ret
}
