#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/zeldaKidnappedRoom.s.
// INTERAC_ZELDA_KIDNAPPED_ROOM

static uint16_t zeldaKidnappedRoom_jump_table(GB *gb) {
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

// INTERAC_ZELDA_KIDNAPPED_ROOM
void s_interactionCodec3_hook(GB *gb) {
  BASE(interactionCodec3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (zeldaKidnappedRoom_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+52) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto substate1; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0x22;
  CALL_C(b_+15, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+18);
  if (F & FZ) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); A = 0x23;
  CALL_C(b_+23, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+26);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); TAIL(interactionDelete); }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+32); SET_HL(SYM(zeldaKidnappedRoom_loadZeldaAndMoblins));
  CYC(b_+32, b_+34); E = 0x15;
  CALL_C(b_+34, s_interBankCall, SYM(interBankCall), b_+37);
  CYC(b_+37, b_+40);
  TAIL(interactionIncSubstate);
substate1:
  CALL_C(b_+40, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+43);
  CALL_C(b_+43, s_interactionIncState, SYM(interactionIncState), b_+46);
  CYC(b_+46, b_+49); SET_HL(SYM(ZeldaBeingKidnappedScript));
  CALL_C(b_+49, s_interactionSetScript, SYM(interactionSetScript), b_+52);
state1:
  CYC(b_+52, b_+55);
  TAIL(interactionRunScript);
}
