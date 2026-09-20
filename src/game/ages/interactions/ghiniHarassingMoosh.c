#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode73_jump_table(GB *gb) {
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

static void interactionCode73_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_GHINI_HARASSING_MOOSH
void interactionCode73_hook(GB *gb) {
  BASE(interactionCode73);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto checkState; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto checkState; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+15); A = W8(wScrollMode);
  CYC(b_+15, b_+17); alu_and(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } // ret nz
  CYC(b_+17, b_+18);

checkState:
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  {
    CYC(b_+21, b_+22); push_effect(gb, b_+22);
    uint16_t target = interactionCode73_jump_table(gb);
    if (target == b_+76) goto state1;
  }

  CYC(b_+26, b_+28); A = 0x01;
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  // Delete self if they shouldn't be here right now
  CYC(b_+29, b_+32); A = W8(wEssencesObtained);
  CYC(b_+32, b_+34); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+34, b_+36); goto deleteThis; } // jr z
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, wGroup1RoomFlags + 0x79); // wPastRoomFlags+$79
  CYC(b_+39, b_+41); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+41, b_+43); goto deleteThis; } // jr z
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); A = W8(wMooshState);
  CYC(b_+46, b_+48); alu_and(gb, 0x60);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto deleteThis; } // jr nz
  CYC(b_+48, b_+50);
  CALL_C(b_+50, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+53);
  CALL_C(b_+53, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+56);
  CYC(b_+56, b_+58); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0xfe); // -2
  // Load script
  CYC(b_+60, b_+62); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+66); SET_HL(b_+110); // @scriptTable
  CYC(b_+66, b_+67); interactionCode73_add_double_index(gb, b_+67);
  CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+68, b_+69); H = mem_rd(gb, HL);
  CYC(b_+69, b_+70); L = A;
  CALL_C(b_+70, interactionSetScript_hook, SYM(interactionSetScript), b_+73);
  CYC(b_+73, b_+76); objectSetVisiblec0_hook(gb); return; // jp

state1:
  CALL_C(b_+76, interactionAnimate_hook, SYM(interactionAnimate), b_+79);
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+83, b_+85); goto l5e1c; } // jr z
  CYC(b_+83, b_+85);
  // While the ghini is moving, make them "rotate" in position.
  CALL_C(b_+85, objectApplySpeed_hook, SYM(objectApplySpeed), b_+88);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); A = alu_dec8(gb, A);
  CYC(b_+92, b_+94); alu_and(gb, 0x1f);
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  CYC(b_+95, b_+97); alu_cp(gb, 0x18);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto l5e1c; } // jr nz
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+102, b_+103); mem_wr(gb, DE, A);

l5e1c:
  CALL_C(b_+103, interactionRunScript_hook, SYM(interactionRunScript), b_+106);
  if (!(F & FC)) { RET_TAKEN(b_+106); return; } // ret nc
  CYC(b_+106, b_+107);

deleteThis:
  CYC(b_+107, b_+110); interactionDelete_hook(gb); return; // jp
}
