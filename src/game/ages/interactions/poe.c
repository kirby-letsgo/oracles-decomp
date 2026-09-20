#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionCode59__afterCall6c0b_hook(GB *gb);

static uint16_t poe_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A); burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L); burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL); burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A; burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

static void poe_add_double_index(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC); burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A; burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0; burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC); burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb)); burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCode59__loadScriptAndInitGraphics_hook(GB *gb) {
  BASE(interactionCode59);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+108, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+111);
  interactionCode59__afterCall6c0b_hook(gb);
}

void interactionCode59__afterCall6c0b_hook(GB *gb) {
  BASE(interactionCode59);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+111, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+114);
  CYC(b_+114, b_+116); E = OBJ_SUBID; CYC(b_+116, b_+117); A = mem_rd(gb, DE); CYC(b_+117, b_+120); SET_HL(b_+130); CYC(b_+120, b_+121); poe_add_double_index(gb, b_+121);
  CYC(b_+121, b_+122); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+122, b_+123); H = mem_rd(gb, HL); CYC(b_+123, b_+124); L = A; CALL_C(b_+124, interactionSetScript_hook, SYM(interactionSetScript), b_+127); CYC(b_+127, b_+130); interactionIncState_hook(gb);
}

void interactionCode59__afterCall6be7_hook(GB *gb) {
  BASE(interactionCode59);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+75, interactionRunScript_hook, SYM(interactionRunScript), b_+78); if (F & FC) { CYCT(b_+78, b_+81); interactionDelete_hook(gb); return; } CYC(b_+78, b_+81);
  CYC(b_+81, b_+83); E = OBJ_VAR3E; CYC(b_+83, b_+84); A = mem_rd(gb, DE); CYC(b_+84, b_+85); alu_or(gb, A); if (!(F & FZ)) { CYCT(b_+85, b_+86); ret_effect(gb); return; } CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); E = OBJ_VAR3F; CYC(b_+88, b_+89); A = mem_rd(gb, DE); CYC(b_+89, b_+90); alu_or(gb, A); if (F & FZ) { CYCT(b_+90, b_+93); npcFaceLinkAndAnimate_hook(gb); return; } CYC(b_+90, b_+93); CALL_C(b_+93, interactionAnimate_hook, SYM(interactionAnimate), b_+96); CYC(b_+96, b_+99); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void interactionCode59_hook(GB *gb) {
  BASE(interactionCode59);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) {
    CYC(b_+3, b_+5); CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_VAR03; CYC(b_+7, b_+8); A = mem_rd(gb, DE); CYC(b_+8, b_+9); push_effect(gb, b_+9);
    do { uint16_t jt_ = (poe_jump_table(gb));
      if (jt_ == b_+15) { CALL_C(b_+15, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+18); CYC(b_+18, b_+20); alu_bit(gb, 6, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(b_+20, b_+23); interactionDelete_hook(gb); return; } CYC(b_+20, b_+23); CYC(b_+23, b_+26); SET_HL(wGroup0RoomFlags + 0x2e); CYC(b_+26, b_+28); alu_bit(gb, 6, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(b_+28, b_+31); interactionDelete_hook(gb); return; } CYC(b_+28, b_+31); CYC(b_+31, b_+33); break; }
      else if (jt_ == b_+33) { CALL_C(b_+33, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+36); CYC(b_+36, b_+38); alu_bit(gb, 5, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(b_+38, b_+41); interactionDelete_hook(gb); return; } CYC(b_+38, b_+41); CYC(b_+41, b_+43); alu_bit(gb, 6, mem_rd(gb, HL)); if (F & FZ) { CYCT(b_+43, b_+46); interactionDelete_hook(gb); return; } CYC(b_+43, b_+46); CYC(b_+46, b_+49); SET_HL(wGroup0RoomFlags + 0x2e); CYC(b_+49, b_+51); alu_bit(gb, 6, mem_rd(gb, HL)); if (F & FZ) { CYCT(b_+51, b_+54); interactionDelete_hook(gb); return; } CYC(b_+51, b_+54); CYC(b_+54, b_+56); break; }
      else if (jt_ == b_+56) { CYC(b_+56, b_+59); SET_HL(wGroup0RoomFlags + 0x7c); CYC(b_+59, b_+61); alu_bit(gb, 6, mem_rd(gb, HL)); if (F & FZ) { CYCT(b_+61, b_+64); interactionDelete_hook(gb); return; } CYC(b_+61, b_+64); CALL_C(b_+64, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+67); CYC(b_+67, b_+69); alu_bit(gb, 6, mem_rd(gb, HL)); if (!(F & FZ)) { CYCT(b_+69, b_+72); interactionDelete_hook(gb); return; } CYC(b_+69, b_+72); break; }
      else { HANDOFF(HL); }
    } while (0);
    CALL_C(b_+72, interactionCode59__loadScriptAndInitGraphics_hook, b_+108, b_+75);
  } else CYCT(b_+3, b_+5);
  interactionCode59__afterCall6be7_hook(gb);
}
