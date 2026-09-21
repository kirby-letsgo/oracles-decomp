#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/interactions/eraOrSeasonInfo.s (INTERAC_ERA_OR_SEASON_INFO), bank $10.

static uint16_t interactionCodee0_jump_table(GB *gb) {
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

void interactionCodee0_hook(GB *gb) {
  BASE(interactionCodee0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(2)); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (interactionCodee0_jump_table(gb));
    if (jt_ == b_+O(12)) { goto state0; }
    else if (jt_ == b_+O(41)) { goto state1; }
    else if (jt_ == b_+O(58)) { goto state2; }
    else if (jt_ == b_+O(69)) { goto state3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CYC(b_+O(12), b_+OE(14)); A = 0x01;
  CYC(b_+O(14), b_+OE(15)); mem_wr(gb, DE, A);
  if (game_seasons) {       // the season of the room pack being entered, or of the current room
    CYC(b_+S(15), b_+S(18)); A = W8(wLoadingRoomPack);
    CYC(b_+S(18), b_+S(19)); A = alu_inc8(gb, A);
    if (F & FZ) CYCT(b_+S(19), b_+S(21));
    else { CYC(b_+S(19), b_+S(21)); CYC(b_+S(21), b_+S(24)); A = W8(wRoomStateModifier); }
  } else {                  // present or past
    CYC(b_+15, b_+18); A = W8(wTilesetFlags);
    CYC(b_+18, b_+20); alu_and(gb, 0x80);
    CYC(b_+20, b_+21); alu_rlca(gb);
  }
  CYC(b_+O(21), b_+OE(23)); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+O(23), b_+OE(24)); mem_wr(gb, DE, A);
  CALL_C(b_+O(24), interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+OE(27));
  CALL_C(b_+O(27), interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+OE(30));
  CYC(b_+O(30), b_+OE(32)); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+O(32), b_+OE(34)); mem_wr(gb, HL, 0x0a);
  CYC(b_+O(34), b_+OE(36)); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(36), b_+OE(38)); mem_wr(gb, HL, 0xb0);
  CYC(b_+O(38), b_+OE(41));
  TAIL(objectSetVisible80);
state1:
  CYC(b_+O(41), b_+OE(42)); H = D;
  CYC(b_+O(42), b_+OE(44)); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(44), b_+OE(45)); A = mem_rd(gb, HL);
  CYC(b_+O(45), b_+OE(47)); alu_sub(gb, 0x04);
  CYC(b_+O(47), b_+OE(48)); mem_wr(gb, HL, A);
  CYC(b_+O(48), b_+OE(50)); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(b_+O(50)); return; }
  CYC(b_+O(50), b_+OE(51));
  CYC(b_+O(51), b_+OE(52)); L = E;
  CYC(b_+O(52), b_+OE(53)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(53), b_+OE(55)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(55), b_+OE(57)); mem_wr(gb, HL, 0x28);
  RET(b_+O(57)); return;
state2:
  CALL_C(b_+O(58), interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+OE(61));
  if (!(F & FZ)) { RET_TAKEN(b_+O(61)); return; }
  CYC(b_+O(61), b_+OE(62));
  CYC(b_+O(62), b_+OE(63)); L = E;
  CYC(b_+O(63), b_+OE(64)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(64), b_+OE(66)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(66), b_+OE(68)); mem_wr(gb, HL, 0x06);
  RET(b_+O(68)); return;
state3:
  CYC(b_+O(69), b_+OE(70)); H = D;
  CYC(b_+O(70), b_+OE(72)); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+O(72), b_+OE(73)); A = mem_rd(gb, HL);
  CYC(b_+O(73), b_+OE(75)); alu_sub(gb, 0x06);
  CYC(b_+O(75), b_+OE(76)); mem_wr(gb, HL, A);
  CYC(b_+O(76), b_+OE(78)); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+O(78), b_+OE(79)); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+O(79)); return; }
  CYC(b_+O(79), b_+OE(80));
  CYC(b_+O(80), b_+OE(83));
  TAIL(interactionDelete);
}
