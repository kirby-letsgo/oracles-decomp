#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

// INTERAC_BUBBLE
//
// interactionCode91@checkDelete is called from two places, both at the top-level entry stack
// depth (no outer push before either), so both its callers and all four of its exits compare
// against sp0_ directly.
void interactionCode91_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x499c, 0x499e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x499e, 0x499f); A = mem_rd(gb, DE);
  CYC(0x499f, 0x49a0); alu_or(gb, A);
  CYC(0x49a0, 0x49a2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x49a2, 0x49a3); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(0x49a3, 0x49a6); goto subid01; } // jp nz
  CYC(0x49a3, 0x49a6);

  // interactionCode91@subid00
  CYC(0x49a6, 0x49a7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x49a7, 0x49a9); goto subid00_state0; } // jr z
  CYC(0x49a7, 0x49a9);

  // interactionCode91@subid00@state1
  CYC(0x49a9, 0x49ac); push_effect(gb, 0x49ac); goto checkDelete;
afterCheckDelete1:
  if (F & FC) { CYCT(0x49ac, 0x49af); interactionDelete_hook(gb); return; } // jp c
  CYC(0x49ac, 0x49af);
  CALL_C(0x49af, objectApplySpeed_hook, 0x201d, 0x49b2);
  CYC(0x49b2, 0x49b4); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x49b4, 0x49b5); A = mem_rd(gb, DE);
  CYC(0x49b5, 0x49b7); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(0x49b7, 0x49ba); interactionDelete_hook(gb); return; } // jp nc
  CYC(0x49b7, 0x49ba);
  CALL_C(0x49ba, interactionDecCounter1_hook, 0x23cc, 0x49bd);
  if (!(F & FZ)) { RET_TAKEN(0x49bd); return; } // ret nz
  CYC(0x49bd, 0x49be);
  CYC(0x49be, 0x49c0); mem_wr(gb, HL, 0x04);
  CYC(0x49c0, 0x49c2); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x49c2, 0x49c3); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x49c3, 0x49c5); goto afterVar31Dec; } // jr nz
  CYC(0x49c3, 0x49c5);
  CYC(0x49c5, 0x49c7); mem_wr(gb, HL, 0x08);
  CYC(0x49c7, 0x49c9); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x49c9, 0x49ca); A = mem_rd(gb, HL);
  CYC(0x49ca, 0x49cb); alu_cpl(gb);
  CYC(0x49cb, 0x49cc); A = alu_inc8(gb, A);
  CYC(0x49cc, 0x49cd); mem_wr(gb, HL, A);

afterVar31Dec:
  CYC(0x49cd, 0x49cf); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x49cf, 0x49d0); A = mem_rd(gb, DE);
  CYC(0x49d0, 0x49d2); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x49d2, 0x49d3); alu_add(gb, mem_rd(gb, HL));
  CYC(0x49d3, 0x49d5); alu_and(gb, 0x1f);
  CYC(0x49d5, 0x49d6); mem_wr(gb, DE, A);
  RET(0x49d6); return; // ret

subid00_state0:
  CYC(0x49d7, 0x49da); push_effect(gb, 0x49da); goto checkDelete;
afterCheckDelete2:
  if (F & FC) { CYCT(0x49da, 0x49dd); interactionDelete_hook(gb); return; } // jp c
  CYC(0x49da, 0x49dd);
  CALL_C(0x49dd, interactionInitGraphics_hook, 0x15fb, 0x49e0);
  CALL_C(0x49e0, interactionIncState_hook, 0x23e0, 0x49e3);
  CYC(0x49e3, 0x49e5); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x49e5, 0x49e7); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x49e7, 0x49e9); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x49e9, 0x49eb); A = 0x04;
  CYC(0x49eb, 0x49ec); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x49ec, 0x49ee); mem_wr(gb, HL, 180);
  CYC(0x49ee, 0x49f0); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x49f0, 0x49f1); A = alu_inc8(gb, A);
  CYC(0x49f1, 0x49f2); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CALL_C(0x49f2, getRandomNumber_hook, 0x043e, 0x49f5);
  CYC(0x49f5, 0x49f7); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x49f7, 0x49f9); goto afterRandDec; } // jr nz
  CYC(0x49f7, 0x49f9);
  CYC(0x49f9, 0x49fa); A = alu_dec8(gb, A);

afterRandDec:
  CYC(0x49fa, 0x49fb); mem_wr(gb, HL, A);
  CYC(0x49fb, 0x49fe); A = W8(wTilesetFlags);
  CYC(0x49fe, 0x4a00); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (!(F & FZ)) { CYCT(0x4a00, 0x4a03); objectSetVisible83_hook(gb); return; } // jp nz
  CYC(0x4a00, 0x4a03);

randomNumberFrom0To4:
  CALL_C(0x4a03, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4a06);
  CYC(0x4a06, 0x4a08); alu_and(gb, 0x07);
  CYC(0x4a08, 0x4a0a); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x4a0a, 0x4a0c); goto randomNumberFrom0To4; } // jr nc
  CYC(0x4a0a, 0x4a0c);
  CYC(0x4a0c, 0x4a0e); alu_sub(gb, 0x02);
  CYC(0x4a0e, 0x4a10); alu_and(gb, 0x1f);
  CYC(0x4a10, 0x4a12); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4a12, 0x4a13); mem_wr(gb, DE, A);
  CYC(0x4a13, 0x4a16); objectSetVisible81_hook(gb); return; // jp

subid01:
  CYC(0x4a16, 0x4a17); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4a17, 0x4a19); goto subid01_state0; } // jr z
  CYC(0x4a17, 0x4a19);

  // interactionCode91@subid01@state1
  CYC(0x4a19, 0x4a1b); A = 0x24; // Object.collisionType
  CALL_C(0x4a1b, objectGetRelatedObject1Var_hook, 0x2160, 0x4a1e);
  CYC(0x4a1e, 0x4a20); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4a20, 0x4a23); interactionDelete_hook(gb); return; } // jp z
  CYC(0x4a20, 0x4a23);
  CALL_C(0x4a23, objectTakePosition_hook, 0x2274, 0x4a26);
  CALL_C(0x4a26, interactionDecCounter1_hook, 0x23cc, 0x4a29);
  if (!(F & FZ)) { RET_TAKEN(0x4a29); return; } // ret nz
  CYC(0x4a29, 0x4a2a);
  CYC(0x4a2a, 0x4a2c); mem_wr(gb, HL, 90);
  CYC(0x4a2c, 0x4a2e); B = 0x91; // INTERAC_BUBBLE
  CYC(0x4a2e, 0x4a31); objectCreateInteractionWithSubid00_hook(gb); return; // jp

subid01_state0:
  CALL_C(0x4a31, interactionIncState_hook, 0x23e0, 0x4a34);
  CYC(0x4a34, 0x4a36); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4a36, 0x4a38); mem_wr(gb, HL, 30);
  RET(0x4a38); return; // ret

checkDelete:
  CYC(0x4a39, 0x4a3c); A = W8(wTilesetFlags);
  CYC(0x4a3c, 0x4a3e); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (!(F & FZ)) { CYCT(0x4a3e, 0x4a41); goto checkDelete_sidescrolling; } // jp nz
  CYC(0x4a3e, 0x4a41);

  // interactionCode91@checkDelete@topDown
  CALL_C(0x4a41, interactionDecCounter2_hook, 0x23d1, 0x4a44);
  CYC(0x4a44, 0x4a45); A = mem_rd(gb, HL);
  CYC(0x4a45, 0x4a47); alu_cp(gb, 60);
  if (!(F & FC)) {
    RET_TAKEN(0x4a47);
    if (gb->pc == 0x49ac && gb->sp == sp0_) goto afterCheckDelete1;
    if (gb->pc == 0x49da && gb->sp == sp0_) goto afterCheckDelete2;
    return;
  } // ret nc
  CYC(0x4a47, 0x4a48);
  CYC(0x4a48, 0x4a49); alu_or(gb, A);
  CYC(0x4a49, 0x4a4a); alu_scf(gb);
  if (F & FZ) {
    RET_TAKEN(0x4a4a);
    if (gb->pc == 0x49ac && gb->sp == sp0_) goto afterCheckDelete1;
    if (gb->pc == 0x49da && gb->sp == sp0_) goto afterCheckDelete2;
    return;
  } // ret z
  CYC(0x4a4a, 0x4a4b);
  CYC(0x4a4b, 0x4a4d); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x4a4d, 0x4a4e); A = mem_rd(gb, HL);
  CYC(0x4a4e, 0x4a50); alu_xor(gb, 0x80);
  CYC(0x4a50, 0x4a51); mem_wr(gb, HL, A);
  RET(0x4a51);
  if (gb->pc == 0x49ac && gb->sp == sp0_) goto afterCheckDelete1;
  if (gb->pc == 0x49da && gb->sp == sp0_) goto afterCheckDelete2;
  return; // ret

checkDelete_sidescrolling:
  CALL_C(0x4a52, objectGetTileAtPosition_hook, 0x1444, 0x4a55);
  CYC(0x4a55, 0x4a58); SET_HL(hazardCollisionTable);
  CALL_C(0x4a58, lookupCollisionTable_hook, 0x1e1f, 0x4a5b);
  CYC(0x4a5b, 0x4a5c); alu_ccf(gb);
  RET(0x4a5c);
  if (gb->pc == 0x49ac && gb->sp == sp0_) goto afterCheckDelete1;
  if (gb->pc == 0x49da && gb->sp == sp0_) goto afterCheckDelete2;
  return; // ret
}
