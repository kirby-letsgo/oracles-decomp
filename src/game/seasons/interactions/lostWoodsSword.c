#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/lostWoodsSword.s.
// INTERAC_LOST_WOODS_SWORD

static uint16_t lostWoodsSword_jump_table(GB *gb) {
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

// INTERAC_LOST_WOODS_SWORD
void s_interactionCode59_hook(GB *gb) {
  BASE(interactionCode59);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (lostWoodsSword_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+62) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); A = 0x05;
  CALL_C(b_+18, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto L_7e6c; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+25, b_+28); TAIL(interactionDelete); }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); alu_sub(gb, 0x01);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
L_7e6c:
  CALL_C(b_+33, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+36);
  CALL_C(b_+36, s_interactionIncState, SYM(interactionIncState), b_+39);
  CALL_C(b_+39, s_objectSetVisible, SYM(objectSetVisible), b_+42);
  CALL_C(b_+42, s_objectSetVisible80, SYM(objectSetVisible80), b_+45);
  CYC(b_+45, b_+48); SET_HL(SYM(lostWoodsSwordScript));
  CALL_C(b_+48, s_interactionSetScript, SYM(interactionSetScript), b_+51);
  CYC(b_+51, b_+53); A = 0x4d;
  CALL_C(b_+53, s_playSound, SYM(playSound_b00), b_+56);
  CYC(b_+56, b_+59); SET_BC(0x8404);
  CYC(b_+59, b_+62);
  TAIL(objectCreateInteraction);
state1:
  CALL_C(b_+62, s_interactionRunScript, SYM(interactionRunScript), b_+65);
  if (F & FC) { CYCT(b_+65, b_+68); TAIL(interactionDelete); }
  CYC(b_+65, b_+68);
  RET(b_+68); return;
}
