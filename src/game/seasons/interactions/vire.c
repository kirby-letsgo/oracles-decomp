#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/vire.s.
// INTERAC_VIRE

static uint16_t vire_jump_table(GB *gb) {
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

// INTERAC_VIRE
void s_interactionCodee3_hook(GB *gb) {
  BASE(interactionCodee3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (vire_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+42) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+17);
  CYC(b_+17, b_+20); SET_BC(0xfe00);
  CALL_C(b_+20, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+23);
  CYC(b_+23, b_+26); SET_HL(SYM(vireScript));
  CALL_C(b_+26, s_interactionSetScript, SYM(interactionSetScript), b_+29);
  CYC(b_+29, b_+31); A = 0xbb;
  CALL_C(b_+31, s_playSound, SYM(playSound_b00), b_+34);
  CYC(b_+34, b_+36); A = 0x00;
  CALL_C(b_+36, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+39);
  CYC(b_+39, b_+42);
  TAIL(interactionAnimateAsNpc);
state1:
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); push_effect(gb, b_+46);
  do { uint16_t jt_ = (vire_jump_table(gb));
    if (jt_ == b_+54) goto substate0;
    if (jt_ == b_+86) goto substate1;
    if (jt_ == b_+116) goto substate2;
    if (jt_ == b_+148) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+54, s_func_6ede_hook, SYM(func_6ede), b_+57);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+62); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+62, b_+64); goto L_6e6b; }
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+67);
  TAIL(interactionAnimateAsNpc);
L_6e6b:
  CALL_C(b_+67, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+70);
  CYC(b_+70, b_+72); A = 0x39;
  CYC(b_+72, b_+75); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+75, s_playSound, SYM(playSound_b00), b_+78);
  CYC(b_+78, b_+80); A = 0x01;
  CALL_C(b_+80, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+83);
  CYC(b_+83, b_+86);
  TAIL(interactionAnimateAsNpc);
substate1:
  CYC(b_+86, b_+89); SET_HL(SYM(seasonsFunc_0a_71ce));
  CYC(b_+89, b_+91); E = 0x0a;
  CALL_C(b_+91, s_interBankCall, SYM(interBankCall), b_+94);
  CALL_C(b_+94, s_interactionRunScript, SYM(interactionRunScript), b_+97);
  if (F & FC) { CYCT(b_+97, b_+99); goto L_6e8e; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+102);
  TAIL(interactionAnimateAsNpc);
L_6e8e:
  CALL_C(b_+102, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+105);
  CYC(b_+105, b_+107); A = 0x74;
  CALL_C(b_+107, s_playSound, SYM(playSound_b00), b_+110);
  CYC(b_+110, b_+113); SET_BC(0xfc00);
  CALL_C(b_+113, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+116);
substate2:
  CALL_C(b_+116, s_func_6ede_hook, SYM(func_6ede), b_+119);
  CYC(b_+119, b_+121); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+121, b_+122); A = mem_rd(gb, DE);
  CYC(b_+122, b_+124); alu_cp(gb, 0xb0);
  if (F & FC) { CYCT(b_+124, b_+126); goto L_6ea9; }
  CYC(b_+124, b_+126);
  CYC(b_+126, b_+129);
  TAIL(interactionAnimateAsNpc);
L_6ea9:
  CYC(b_+129, b_+132); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+132, b_+135); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+135, s_playSound, SYM(playSound_b00), b_+138);
  CYC(b_+138, b_+139); alu_xor(gb, A);
  CYC(b_+139, b_+142); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+142, b_+145); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+145, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+148);
substate3:
  CALL_C(b_+148, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+151);
  if (!(F & FZ)) { RET_TAKEN(b_+151); return; }
  CYC(b_+151, b_+152);
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x90);
  CYC(b_+154, b_+155); L = alu_inc8(gb, L);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x07);
  CYC(b_+157, b_+159); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+159, b_+161); mem_wr(gb, HL, 0x7c);
  CYC(b_+161, b_+163); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x78);
  CALL_C(b_+165, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+168);
  if (!(F & FZ)) { CYCT(b_+168, b_+170); goto L_6edb; }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+172); mem_wr(gb, HL, 0x05);
  CYC(b_+172, b_+174); A = 0x78;
  CYC(b_+174, b_+176); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+176, b_+177); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+177, b_+178); L = alu_inc8(gb, L);
  CYC(b_+178, b_+179); mem_wr(gb, HL, A);
L_6edb:
  CYC(b_+179, b_+182);
  TAIL(interactionDelete);
}

// objectApplyComponentSpeed@addSpeedComponent
static void vire_addSpeedComponent(GB *gb) {
  BASE(objectApplyComponentSpeed);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+15, b_+16); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_adc(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1);
  RET(b_+20); return;
}

void s_func_6ede_hook(GB *gb) {
  BASE(func_6ede);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = mem_rd(gb, hActiveObjectType);
  CYC(b_+2, b_+4); alu_add(gb, 0x0e);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+7); alu_add(gb, 0x06);
  CYC(b_+7, b_+8); E = A;
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+12);
  vire_addSpeedComponent(gb); return;
}

