#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/75.s.
// INTERAC_INTRO_SPRITE

static uint16_t i75_jump_table(GB *gb) {
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

// INTERAC_INTRO_SPRITE
void s_interactionCode75_hook(GB *gb) {
  BASE(interactionCode75);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i75_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+61) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_interactionIncState, SYM(interactionIncState), b_+11);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto notSubdId0; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); SET_HL(SYM(script6f48));
  CALL_C(b_+23, s_interactionSetScript, SYM(interactionSetScript), b_+26);
  CYC(b_+26, b_+29);
  TAIL(objectSetVisible82);
notSubdId0:
  CYC(b_+29, b_+30); H = D;
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x70);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x80);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x18);
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x05);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+51, b_+54); TAIL(objectSetVisible83); }
  CYC(b_+51, b_+54);
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x05);
  CYC(b_+58, b_+61);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); push_effect(gb, b_+65);
  do { uint16_t jt_ = (i75_jump_table(gb));
    if (jt_ == b_+71) goto subid0;
    if (jt_ == b_+102) goto subid1;
    if (jt_ == b_+132) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+71, s_interactionRunScript, SYM(interactionRunScript), b_+74);
  if (F & FC) { CYCT(b_+74, b_+77); TAIL(interactionDelete); }
  CYC(b_+74, b_+77);
  CALL_C(b_+77, s_interactionAnimate, SYM(interactionAnimate), b_+80);
  CYC(b_+80, b_+81); H = D;
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x00);
  CYC(b_+88, b_+89); A = alu_dec8(gb, A);
  CYC(b_+89, b_+91); alu_add(gb, 0x30);
  CYC(b_+91, b_+92); push_effect(gb, DE);
  CALL_C(b_+92, s_loadGfxHeader, SYM(loadGfxHeader), b_+95);
  CYC(b_+95, b_+97); A = 0x0c;
  CALL_C(b_+97, s_loadUncompressedGfxHeader, SYM(loadUncompressedGfxHeader), b_+100);
  CYC(b_+100, b_+101); SET_DE(pop_effect(gb));
  RET(b_+101); return;
subid1:
  CALL_C(b_+102, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+105);
  if (!(F & FZ)) { CYCT(b_+105, b_+107); goto subid2; }
  CYC(b_+105, b_+107);
  CALL_C(b_+107, s_interactionAnimate, SYM(interactionAnimate), b_+110);
  CYC(b_+110, b_+111); H = D;
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+113, b_+114); A = mem_rd(gb, HL);
  CYC(b_+114, b_+115); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+115, b_+117); goto subid2; }
  CYC(b_+115, b_+117);
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x00);
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+122, b_+124); goto subid2; }
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+126, b_+127); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+127, b_+129); A = 0x04;
  CALL_C(b_+129, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+132);
subid2:
  CYC(b_+132, b_+135); SET_HL(wTmpcbb6);
  CYC(b_+135, b_+136); A = mem_rd(gb, HL);
  CYC(b_+136, b_+137); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+137); return; }
  CYC(b_+137, b_+138);
  CYC(b_+138, b_+141);
  TAIL(objectApplySpeed);
}
