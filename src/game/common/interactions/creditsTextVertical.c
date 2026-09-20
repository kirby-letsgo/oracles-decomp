#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodeaf_jump_table(GB *gb) {
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

static void interactionCodeaf_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_CREDITS_TEXT_VERTICAL
//
// Variables:
//   var30/var31: 16-bit counter?
// ==================================================================================================
void interactionCodeaf_hook(GB *gb) {
  BASE(interactionCodeaf);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodeaf_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+29) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state]
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto l_665b; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); SET_HL(b_+120); // @data_66bc
  CYC(b_+20, b_+23); goto storeVar30Value; // jp

l_665b:
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x14); // SPEED_80
  RET(b_+28); return;

state1:
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto subid1; } // jr nz
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+40); ret_effect(gb); return; } // ret nz
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CALL_C(b_+43, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+46);
  if (!(F & FZ)) { CYCT(b_+46, b_+47); ret_effect(gb); return; } // ret nz
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+50); push_effect(gb, b_+50);

  // interactionCodeaf@spawnChild, inlined (single caller, reached by a genuine `call` right
  // above): falls straight through into the shared @storeVar30Value tail below, whose real
  // "ret" is checked exactly like CALL_C would, since this call site has more work to do
  // afterward (unlike @state0's own tail `jp storeVar30Value`, a plain same-function jump).
  CALL_C(b_+63, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+66);
  if (!(F & FZ)) { CYCT(b_+66, b_+68); goto l_6695; } // jr nz
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0xaf); // INTERAC_CREDITS_TEXT_VERTICAL
  CYC(b_+70, b_+71); L = alu_inc8(gb, L);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x01); // [child.subid] = 1
  CYC(b_+73, b_+74); L = alu_inc8(gb, L);
  CYC(b_+74, b_+76); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); // [child.var03]
  CALL_C(b_+78, objectCopyPosition_hook, SYM(objectCopyPosition), b_+81);

l_6695:
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+89); SET_HL(b_+120); // @data_66bc
  CYC(b_+89, b_+90); interactionCodeaf_addDoubleIndex(gb, b_+90);
  // falls through into @storeVar30Value

storeVar30Value:
  CYC(b_+90, b_+91); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+91, b_+93); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+95); E = alu_inc8(gb, E);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  RET(b_+97);
  // @state0 reaches here via a plain tail `jp` with nothing of its own left to do, so its
  // "ret" really does end the whole hook (gb->pc/sp already correctly resolved to the true
  // caller). Only the @spawnChild call site above still has C code to run afterward.
  if (!(gb->pc == b_+50 && gb->sp == sp0_)) return;

  CYC(b_+50, b_+52); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+54, b_+55); ret_effect(gb); return; } // ret nz
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+58); SET_HL(GV(wTmpcfc0_genericCutscene_cfdf, wTmpcfc0_wildTokay_cfde));
  CYC(b_+58, b_+60); mem_wr(gb, HL, GV(0xff, 0x01));
  CYC(b_+60, b_+63); interactionDelete_hook(gb); return; // jp

subid1:
  CYC(b_+98, b_+101); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+101, b_+102); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+102, b_+103); ret_effect(gb); return; } // ret nz
  CYC(b_+102, b_+103);
  CALL_C(b_+103, objectApplySpeed_hook, SYM(objectApplySpeed), b_+106);
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+109, b_+110); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+110, b_+111); B = A;
  CYC(b_+111, b_+112); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+112, b_+115); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+112, b_+115);
  CYC(b_+115, b_+116); L = alu_inc8(gb, L);
  CYC(b_+116, b_+117); C = mem_rd(gb, HL); // [xh]
  CYC(b_+117, b_+120); interactionFunc_3e6d_hook(gb); return; // jp
}
