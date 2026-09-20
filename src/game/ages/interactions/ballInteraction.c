#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x7c81, 0x7c83); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7c83, 0x7c84); A = mem_rd(gb, DE);
  {
    CYC(0x7c84, 0x7c85); push_effect(gb, 0x7c85);
    uint16_t target = ballInteraction_jump_table(gb);
    if (target == 0x7c96) goto state1;
  }

  // interactionCode95@state0
  CALL_C(0x7c89, interactionIncState_hook, 0x23e0, 0x7c8c);
  CYC(0x7c8c, 0x7c8e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7c8e, 0x7c90); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x7c90, interactionInitGraphics_hook, 0x15fb, 0x7c93);
  CYC(0x7c93, 0x7c96); objectSetVisible80_hook(gb); return; // jp

state1:
  CYC(0x7c96, 0x7c98); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7c98, 0x7c99); A = mem_rd(gb, DE);
  {
    CYC(0x7c99, 0x7c9a); push_effect(gb, 0x7c9a);
    uint16_t target = ballInteraction_jump_table(gb);
    if (target == 0x7cc9) goto substate1;
    if (target == 0x7d09) goto substate2;
  }

  // interactionCode95@substate0
  CYC(0x7ca0, 0x7ca3); A = W8(wTmpcfc0_genericCutscene_cfd3);
  CYC(0x7ca3, 0x7ca4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7ca4); return; } // ret z
  CYC(0x7ca4, 0x7ca5);
  CALL_C(0x7ca5, interactionIncSubstate_hook, 0x23e5, 0x7ca8);
  CYC(0x7ca8, 0x7caa); B = 0x08; // ANGLE_RIGHT
  CYC(0x7caa, 0x7cab); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7cab, 0x7cad); goto afterAngle; } // jr z
  CYC(0x7cab, 0x7cad);
  CYC(0x7cad, 0x7caf); B = 0x18; // ANGLE_LEFT

afterAngle:
  CYC(0x7caf, 0x7cb1); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7cb1, 0x7cb2); mem_wr(gb, HL, B);
  CYC(0x7cb2, 0x7cb4); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7cb4, 0x7cb5); mem_wr(gb, HL, A);
  CYC(0x7cb5, 0x7cb7); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x7cb7, 0x7cb9); goto setSpeedZ; } // jr nz
  CYC(0x7cb7, 0x7cb9);
  CYC(0x7cb9, 0x7cbc); SET_BC(0x5075);
  CALL_C(0x7cbc, interactionHSetPosition_hook, 0x2774, 0x7cbf);
  CYC(0x7cbf, 0x7cc1); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x7cc1, 0x7cc3); mem_wr(gb, HL, 0xfa); // -6

setSpeedZ:
  CYC(0x7cc3, 0x7cc6); SET_BC(0xfe40); // -0x1c0
  CYC(0x7cc6, 0x7cc9); objectSetSpeedZ_hook(gb); return; // jp

substate1:
  CALL_C(0x7cc9, objectApplySpeed_hook, 0x201d, 0x7ccc);
  CYC(0x7ccc, 0x7cce); C = 0x20;
  CALL_C(0x7cce, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7cd1);
  if (!(F & FZ)) { RET_TAKEN(0x7cd1); return; } // ret nz
  CYC(0x7cd1, 0x7cd2);

  // Ball has landed
  CYC(0x7cd2, 0x7cd4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7cd4, 0x7cd5); A = mem_rd(gb, DE);
  CYC(0x7cd5, 0x7cd7); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x7cd7, 0x7cd9); goto subid2; } // jr z
  CYC(0x7cd7, 0x7cd9);
  CYC(0x7cd9, 0x7cda); A = alu_dec8(gb, A);
  CYC(0x7cda, 0x7cdd); SET_BC(0x4a3c);
  if (F & FZ) { CYCT(0x7cdd, 0x7cdf); goto ballLanded; } // jr z
  CYC(0x7cdd, 0x7cdf);
  CYC(0x7cdf, 0x7ce1); C = 0x75;

ballLanded:
  CYC(0x7ce1, 0x7ce2); alu_xor(gb, A);
  CYC(0x7ce2, 0x7ce5); W8(wTmpcfc0_genericCutscene_cfd3) = A;
  CYC(0x7ce5, 0x7ce7); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7ce7, 0x7ce8); mem_wr(gb, DE, A);
  CYC(0x7ce8, 0x7ceb); interactionSetPosition_hook(gb); return; // jp

subid2:
  // [speedZ] = -[speedZ]/2
  CYC(0x7ceb, 0x7ced); L = INTERACTION_BASE + OBJ_SPEED_Z + 1; // Interaction.speedZ+1
  CYC(0x7ced, 0x7cee); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7cee, 0x7cf0); A = alu_srl(gb, A);
  CYC(0x7cf0, 0x7cf1); B = A;
  CYC(0x7cf1, 0x7cf2); A = mem_rd(gb, HL);
  CYC(0x7cf2, 0x7cf3); alu_rra(gb);
  CYC(0x7cf3, 0x7cf4); alu_cpl(gb);
  CYC(0x7cf4, 0x7cf6); alu_add(gb, 0x01);
  CYC(0x7cf6, 0x7cf7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7cf7, 0x7cf8); A = B;
  CYC(0x7cf8, 0x7cf9); alu_cpl(gb);
  CYC(0x7cf9, 0x7cfb); alu_adc(gb, 0x00);
  CYC(0x7cfb, 0x7cfc); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a

  // Go to substate 2 (stop doing anything) if the ball's Z speed has gone too low
  CYC(0x7cfc, 0x7cff); SET_BC(0xff80);
  CYC(0x7cff, 0x7d00); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7d00, 0x7d01); H = mem_rd(gb, HL);
  CYC(0x7d01, 0x7d02); L = A;
  CALL_C(0x7d02, compareHlToBc_hook, 0x01d6, 0x7d05);
  if (F & FC) { RET_TAKEN(0x7d05); return; } // ret c
  CYC(0x7d05, 0x7d06);
  CYC(0x7d06, 0x7d09); interactionIncSubstate_hook(gb); return; // jp

substate2:
  RET(0x7d09); return; // ret
}
