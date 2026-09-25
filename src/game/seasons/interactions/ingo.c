#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/ingo.s.
// INTERAC_INGO

static uint16_t ingo_jump_table(GB *gb) {
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

// interactionCode57@func_7d5a
static void ingo_func_7d5a(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+56, b_+59); A = mem_rd(gb, w1Link_yh);
  CYC(b_+59, b_+61); alu_sub(gb, 0x20);
  if (!(F & FC)) { RET_TAKEN(b_+61); return; }
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); A = 0x22;
  CYC(b_+64, b_+67); mem_wr(gb, w1Link_yh, A);
  CYC(b_+67, b_+70); A = mem_rd(gb, wLinkInAir);
  CYC(b_+70, b_+71); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+74); A = 0x80;
  CYC(b_+74, b_+77); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+77, b_+79); A = 0x01;
  CYC(b_+79, b_+82); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+82, b_+85); SET_HL(SYM(ingoScript_LinkApproachingVases));
  CALL_C(b_+85, s_interactionSetScript, SYM(interactionSetScript), b_+88);
  CYC(b_+88, b_+89); H = D;
  CYC(b_+89, b_+91); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x02);
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x00);
  CYC(b_+96, b_+97); SET_BC(pop_effect(gb));
  RET(b_+97); return;
}

// INTERAC_INGO
void s_interactionCode57_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ingo_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+40) goto state1;
    if (jt_ == b_+49) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+10, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+13);
  CALL_C(b_+13, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+16);
L_7d32:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x01);
  CYC(b_+21, b_+23); A = 0x0b;
  CALL_C(b_+23, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+26);
  CYC(b_+26, b_+29); SET_HL(SYM(ingoScript_tradingVase));
  CALL_C(b_+29, s_interactionSetScript, SYM(interactionSetScript), b_+32);
  CYC(b_+32, b_+34); A = 0x02;
  CALL_C(b_+34, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+37);
  CYC(b_+37, b_+40);
  goto preventLinkFromPassing;
state1:
  CALL_L(b_+40, ingo_func_7d5a, b_+43);
  CALL_C(b_+43, s_interactionRunScript, SYM(interactionRunScript), b_+46);
  CYC(b_+46, b_+49);
  TAIL(npcFaceLinkAndAnimate);
state2:
  CALL_C(b_+49, s_interactionRunScript, SYM(interactionRunScript), b_+52);
  if (F & FC) { CYCT(b_+52, b_+54); goto L_7d32; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56);
  goto func7d84;

func7d84:
  CALL_C(b_+98, s_interactionAnimate, SYM(interactionAnimate), b_+101);
  CYC(b_+101, b_+103); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+105, b_+107); goto preventLinkFromPassing; }
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+112); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+112, b_+114); goto preventLinkFromPassing; }
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+116); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+116, b_+118); A = 0x01;
  CYC(b_+118, b_+119); mem_wr(gb, DE, A);
preventLinkFromPassing:
  CALL_C(b_+119, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+122);
  CYC(b_+122, b_+125);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}
