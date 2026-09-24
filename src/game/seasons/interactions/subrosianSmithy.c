#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosianSmithy.s.
// INTERAC_SUBROSIAN_SMITHY

static uint16_t subrosianSmithy_jump_table(GB *gb) {
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

// interactionCodea4@func_6418
static void subrosianSmithy_func_6418(GB *gb) {
  BASE(interactionCodea4);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+46, b_+48); A = 0x4a;
  CALL_C(b_+48, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+51);
  if (!(F & FC)) { CYCT(b_+51, b_+53); goto L_6424; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+54); alu_or(gb, A);
  CYC(b_+54, b_+56); A = 0x01;
  if (F & FZ) { CYCT(b_+56, b_+58); TAIL_S(smithyLoadIntoVar3f); }
  CYC(b_+56, b_+58);
L_6424:
  CYC(b_+58, b_+60); A = 0x52;
  CALL_C(b_+60, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+63);
  if (!(F & FC)) { CYCT(b_+63, b_+65); goto L_642f; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); A = 0x02;
  CYC(b_+67, b_+69);
  TAIL_S(smithyLoadIntoVar3f);
L_642f:
  CYC(b_+69, b_+71); A = 0x28;
  CALL_C(b_+71, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+74);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto L_643a; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); A = 0x00;
  CYC(b_+78, b_+80);
  TAIL_S(smithyLoadIntoVar3f);
L_643a:
  CYC(b_+80, b_+82); A = 0x03;
  s_smithyLoadIntoVar3f_hook(gb); return; // falls through
}

// INTERAC_SUBROSIAN_SMITHY
void s_interactionCodea4_hook(GB *gb) {
  BASE(interactionCodea4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianSmithy_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+40) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, s_interactionIncState, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x04);
  CYC(b_+18, b_+20); A = 0x3b;
  CALL_C(b_+20, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+23);
  CALL_L(b_+23, subrosianSmithy_func_6418, b_+26);
  CYC(b_+26, b_+29); SET_HL(SYM(subrosianSmithyScript));
  CALL_C(b_+29, s_interactionSetScript, SYM(interactionSetScript), b_+32);
  CALL_C(b_+32, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+35);
  CYC(b_+35, b_+37); A = 0x02;
  CALL_C(b_+37, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+40);
state1:
  CALL_C(b_+40, s_interactionRunScript, SYM(interactionRunScript), b_+43);
  CYC(b_+43, b_+46);
  TAIL(interactionAnimateAsNpc);
}

void s_smithyLoadIntoVar3f_hook(GB *gb) {
  BASE(smithyLoadIntoVar3f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  RET(b_+3); return;
}

