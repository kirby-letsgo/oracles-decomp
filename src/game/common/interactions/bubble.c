#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode91), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode91), (from), (to), true)

// INTERAC_BUBBLE
//
// interactionCode91@checkDelete is called from two places, both at the top-level entry stack
// depth (no outer push before either), so both its callers and all four of its exits compare
// against sp0_ directly.
void interactionCode91_hook(GB *gb) {
  BASE(interactionCode91);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); goto subid01; } // jp nz
  CYC(b_+7, b_+10);

  // interactionCode91@subid00
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto subid00_state0; } // jr z
  CYC(b_+11, b_+13);

  // interactionCode91@subid00@state1
  CYC(b_+13, b_+16); push_effect(gb, b_+16); goto checkDelete;
afterCheckDelete1:
  if (F & FC) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+16, b_+19);
  CALL_C(b_+19, objectApplySpeed_hook, SYM(objectApplySpeed), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_cp(gb, 0xf0);
  if (!(F & FC)) { CYCT(b_+27, b_+30); interactionDelete_hook(gb); return; } // jp nc
  CYC(b_+27, b_+30);
  CALL_C(b_+30, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x04);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto afterVar31Dec; } // jr nz
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x08);
  CYC(b_+43, b_+45); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+47); alu_cpl(gb);
  CYC(b_+47, b_+48); A = alu_inc8(gb, A);
  CYC(b_+48, b_+49); mem_wr(gb, HL, A);

afterVar31Dec:
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+54); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+54, b_+55); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+55, b_+57); alu_and(gb, 0x1f);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  RET(b_+58); return; // ret

subid00_state0:
  CYC(b_+59, b_+62); push_effect(gb, b_+62); goto checkDelete;
afterCheckDelete2:
  if (F & FC) { CYCT(b_+62, b_+65); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+62, b_+65);
  CALL_C(b_+65, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+68);
  CALL_C(b_+68, interactionIncState_hook, SYM(interactionIncState), b_+71);
  CYC(b_+71, b_+73); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+77, b_+79); A = 0x04;
  CYC(b_+79, b_+80); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+80, b_+82); mem_wr(gb, HL, 180);
  CYC(b_+82, b_+84); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+84, b_+85); A = alu_inc8(gb, A);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CALL_C(b_+86, getRandomNumber_hook, SYM(getRandomNumber), b_+89);
  CYC(b_+89, b_+91); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+91, b_+93); goto afterRandDec; } // jr nz
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+94); A = alu_dec8(gb, A);

afterRandDec:
  CYC(b_+94, b_+95); mem_wr(gb, HL, A);
  CYC(b_+95, b_+98); A = W8(wTilesetFlags);
  CYC(b_+98, b_+100); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (!(F & FZ)) { CYCT(b_+100, b_+103); objectSetVisible83_hook(gb); return; } // jp nz
  CYC(b_+100, b_+103);

randomNumberFrom0To4:
  CALL_C(b_+103, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+106);
  CYC(b_+106, b_+108); alu_and(gb, 0x07);
  CYC(b_+108, b_+110); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+110, b_+112); goto randomNumberFrom0To4; } // jr nc
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); alu_sub(gb, 0x02);
  CYC(b_+114, b_+116); alu_and(gb, 0x1f);
  CYC(b_+116, b_+118); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+118, b_+119); mem_wr(gb, DE, A);
  CYC(b_+119, b_+122); objectSetVisible81_hook(gb); return; // jp

subid01:
  CYC(b_+122, b_+123); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+123, b_+125); goto subid01_state0; } // jr z
  CYC(b_+123, b_+125);

  // interactionCode91@subid01@state1
  CYC(b_+125, b_+127); A = 0x24; // Object.collisionType
  CALL_C(b_+127, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+130);
  CYC(b_+130, b_+132); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+132, b_+135); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+132, b_+135);
  CALL_C(b_+135, objectTakePosition_hook, SYM(objectTakePosition), b_+138);
  CALL_C(b_+138, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+141);
  if (!(F & FZ)) { RET_TAKEN(b_+141); return; } // ret nz
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+144); mem_wr(gb, HL, 90);
  CYC(b_+144, b_+146); B = 0x91; // INTERAC_BUBBLE
  CYC(b_+146, b_+149); objectCreateInteractionWithSubid00_hook(gb); return; // jp

subid01_state0:
  CALL_C(b_+149, interactionIncState_hook, SYM(interactionIncState), b_+152);
  CYC(b_+152, b_+154); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 30);
  RET(b_+156); return; // ret

checkDelete:
  CYC(b_+157, b_+160); A = W8(wTilesetFlags);
  CYC(b_+160, b_+162); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (!(F & FZ)) { CYCT(b_+162, b_+165); goto checkDelete_sidescrolling; } // jp nz
  CYC(b_+162, b_+165);

  // interactionCode91@checkDelete@topDown
  CALL_C(b_+165, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+168);
  CYC(b_+168, b_+169); A = mem_rd(gb, HL);
  CYC(b_+169, b_+171); alu_cp(gb, 60);
  if (!(F & FC)) {
    RET_TAKEN(b_+171);
    if (gb->pc == b_+16 && gb->sp == sp0_) goto afterCheckDelete1;
    if (gb->pc == b_+62 && gb->sp == sp0_) goto afterCheckDelete2;
    return;
  } // ret nc
  CYC(b_+171, b_+172);
  CYC(b_+172, b_+173); alu_or(gb, A);
  CYC(b_+173, b_+174); alu_scf(gb);
  if (F & FZ) {
    RET_TAKEN(b_+174);
    if (gb->pc == b_+16 && gb->sp == sp0_) goto afterCheckDelete1;
    if (gb->pc == b_+62 && gb->sp == sp0_) goto afterCheckDelete2;
    return;
  } // ret z
  CYC(b_+174, b_+175);
  CYC(b_+175, b_+177); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+177, b_+178); A = mem_rd(gb, HL);
  CYC(b_+178, b_+180); alu_xor(gb, 0x80);
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  RET(b_+181);
  if (gb->pc == b_+16 && gb->sp == sp0_) goto afterCheckDelete1;
  if (gb->pc == b_+62 && gb->sp == sp0_) goto afterCheckDelete2;
  return; // ret

checkDelete_sidescrolling:
  CALL_C(b_+182, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+185);
  CYC(b_+185, b_+188); SET_HL(hazardCollisionTable);
  CALL_C(b_+188, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+191);
  CYC(b_+191, b_+192); alu_ccf(gb);
  RET(b_+192);
  if (gb->pc == b_+16 && gb->sp == sp0_) goto afterCheckDelete1;
  if (gb->pc == b_+62 && gb->sp == sp0_) goto afterCheckDelete2;
  return; // ret
}
