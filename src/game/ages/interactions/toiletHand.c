#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t toilet_hand_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A); burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb)); burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L); burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL); burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A; burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void toilet_hand_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void toilet_hand_load_script(GB *gb, uint16_t sp0_) {
  BASE(interactionCode5b);
  CYC(b_+106, b_+108); E = OBJ_SUBID; CYC(b_+108, b_+109); A = mem_rd(gb, DE); CYC(b_+109, b_+112); SET_HL(b_+184); CYC(b_+112, b_+113); toilet_hand_add_double_index(gb, b_+113); CYC(b_+113, b_+114); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+114, b_+115); H = mem_rd(gb, HL); CYC(b_+115, b_+116); L = A; CYC(b_+116, b_+119); interactionSetScript_hook(gb);
}

static void toilet_hand_load_script_and_init_graphics(GB *gb, uint16_t sp0_) {
  BASE(interactionCode5b);
  CALL_C(b_+95, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+98); CYC(b_+98, b_+100); A = 0x0b; CALL_C(b_+100, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+103); CALL_C(b_+103, interactionIncState_hook, SYM(interactionIncState), b_+106); toilet_hand_load_script(gb, sp0_);
}

static void toilet_hand_respond_to_object_in_hole(GB *gb, uint16_t sp0_) {
  BASE(interactionCode5b);
  CYC(b_+119, b_+122); A = W8(wTextIsActive); CYC(b_+122, b_+123); alu_or(gb, A); if (!(F & FZ)) { CYCT(b_+123, b_+124); ret_effect(gb); return; } CYC(b_+123, b_+124);
  CYC(b_+124, b_+127); A = mem_rd(gb, wTmpcfc0_fallDownHoleEvent + 0x18); CYC(b_+127, b_+128); A = alu_inc8(gb, A); CYC(b_+128, b_+129); E = A; CYC(b_+129, b_+132); SET_HL(b_+155); CALL_C(b_+132, lookupKey_hook, SYM(lookupKey), b_+135); if (!(F & FC)) { CYCT(b_+135, b_+136); ret_effect(gb); return; } CYC(b_+135, b_+136);
  CYC(b_+136, b_+139); SET_HL(b_+160); CYC(b_+139, b_+140); toilet_hand_add_double_index(gb, b_+140); CYC(b_+140, b_+141); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+141, b_+142); H = mem_rd(gb, HL); CYC(b_+142, b_+143); L = A;
  CYC(b_+143, b_+146); A = mem_rd(gb, wTmpcfc0_fallDownHoleEvent + 0x19); CYC(b_+146, b_+147); E = A; CALL_C(b_+147, lookupKey_hook, SYM(lookupKey), b_+150); if (!(F & FC)) { CYCT(b_+150, b_+151); ret_effect(gb); return; } CYC(b_+150, b_+151); CYC(b_+151, b_+153); E = OBJ_VAR38; CYC(b_+153, b_+154); mem_wr(gb, DE, A); CYC(b_+154, b_+155); ret_effect(gb);
}

void interactionCode5b_hook(GB *gb) {
  BASE(interactionCode5b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = OBJ_STATE; CYC(b_+2, b_+3); A = mem_rd(gb, DE); CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (toilet_hand_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+24) { goto state1; }
    else if (jt_ == b_+50) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);
state0:
  CYC(b_+10, b_+13); push_effect(gb, b_+13); toilet_hand_load_script_and_init_graphics(gb, sp0_); CALL_C(b_+13, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+16); CYC(b_+16, b_+19); SET_HL((SYM(shopkeeperState3) + 3)); CYC(b_+19, b_+21); E = 0x08; CALL_C(b_+21, interBankCall_hook, 0x008a, b_+24);
state1:
  CYC(b_+24, b_+27); push_effect(gb, b_+27); toilet_hand_respond_to_object_in_hole(gb, sp0_); if (F & FC) goto dropped;
  CALL_C(b_+29, interactionRunScript_hook, SYM(interactionRunScript), b_+32); CYC(b_+32, b_+33); H = D; CYC(b_+33, b_+35); L = OBJ_VISIBLE; CYC(b_+35, b_+37); alu_bit(gb, 7, mem_rd(gb, HL)); if (F & FZ) { CYCT(b_+37, b_+38); ret_effect(gb); return; } CYC(b_+37, b_+38); CYC(b_+38, b_+41); interactionAnimateAsNpc_hook(gb); return;
dropped:
  CYC(b_+41, b_+44); SET_HL((SYM(interactionCode53__initGraphicsLoadScriptAndIncState) + 6)); CALL_C(b_+44, interactionSetScript_hook, SYM(interactionSetScript), b_+47); CYC(b_+47, b_+50); interactionIncState_hook(gb); return;
state2:
  CYC(b_+50, b_+53); A = W8(wTextIsActive); CYC(b_+53, b_+54); alu_or(gb, A); if (!(F & FZ)) { CYCT(b_+54, b_+55); ret_effect(gb); return; } CYC(b_+54, b_+55); CALL_C(b_+55, interactionRunScript_hook, SYM(interactionRunScript), b_+58); if (F & FC) goto script_ended; CYC(b_+58, b_+60); CYC(b_+60, b_+61); H = D; CYC(b_+61, b_+63); L = OBJ_VISIBLE; CYC(b_+63, b_+65); alu_bit(gb, 7, mem_rd(gb, HL)); if (F & FZ) { CYCT(b_+65, b_+66); ret_effect(gb); return; } CYC(b_+65, b_+66); CALL_C(b_+66, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+69); CYC(b_+69, b_+72); interactionAnimate_hook(gb); return;
script_ended:
  CYCT(b_+58, b_+60); CYC(b_+72, b_+75); push_effect(gb, b_+75); toilet_hand_load_script(gb, sp0_); CYC(b_+75, b_+78); SET_HL((SYM(shopkeeperState3) + 3)); CYC(b_+78, b_+80); E = 0x08; CALL_C(b_+80, interBankCall_hook, 0x008a, b_+83); CYC(b_+83, b_+85); E = OBJ_STATE; CYC(b_+85, b_+87); A = 1; CYC(b_+87, b_+88); mem_wr(gb, DE, A); CYC(b_+88, b_+89); ret_effect(gb);
}
