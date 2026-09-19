#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6644, 0x6646); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6646, 0x6647); A = mem_rd(gb, DE);
  CYC(0x6647, 0x6648); push_effect(gb, 0x6648);
  switch (interactionCodeaf_jump_table(gb)) {
    case 0x664c: goto state0;
    case 0x6661: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x664c, 0x664e); A = 0x01;
  CYC(0x664e, 0x664f); mem_wr(gb, DE, A); // [state]
  CYC(0x664f, 0x6651); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6651, 0x6652); A = mem_rd(gb, DE);
  CYC(0x6652, 0x6653); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6653, 0x6655); goto l_665b; } // jr nz
  CYC(0x6653, 0x6655);
  CYC(0x6655, 0x6658); SET_HL(0x66bc); // @data_66bc
  CYC(0x6658, 0x665b); goto storeVar30Value; // jp

l_665b:
  CYC(0x665b, 0x665c); H = D;
  CYC(0x665c, 0x665e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x665e, 0x6660); mem_wr(gb, HL, 0x14); // SPEED_80
  RET(0x6660); return;

state1:
  CYC(0x6661, 0x6663); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6663, 0x6664); A = mem_rd(gb, DE);
  CYC(0x6664, 0x6665); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6665, 0x6667); goto subid1; } // jr nz
  CYC(0x6665, 0x6667);
  CYC(0x6667, 0x666a); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x666a, 0x666b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x666b, 0x666c); ret_effect(gb); return; } // ret nz
  CYC(0x666b, 0x666c);
  CYC(0x666c, 0x666d); H = D;
  CYC(0x666d, 0x666f); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CALL_C(0x666f, decHlRef16WithCap_hook, 0x0237, 0x6672);
  if (!(F & FZ)) { CYCT(0x6672, 0x6673); ret_effect(gb); return; } // ret nz
  CYC(0x6672, 0x6673);
  CYC(0x6673, 0x6676); push_effect(gb, 0x6676);

  // interactionCodeaf@spawnChild, inlined (single caller, reached by a genuine `call` right
  // above): falls straight through into the shared @storeVar30Value tail below, whose real
  // "ret" is checked exactly like CALL_C would, since this call site has more work to do
  // afterward (unlike @state0's own tail `jp storeVar30Value`, a plain same-function jump).
  CALL_C(0x6683, getFreeInteractionSlot_hook, 0x3aef, 0x6686);
  if (!(F & FZ)) { CYCT(0x6686, 0x6688); goto l_6695; } // jr nz
  CYC(0x6686, 0x6688);
  CYC(0x6688, 0x668a); mem_wr(gb, HL, 0xaf); // INTERAC_CREDITS_TEXT_VERTICAL
  CYC(0x668a, 0x668b); L = alu_inc8(gb, L);
  CYC(0x668b, 0x668d); mem_wr(gb, HL, 0x01); // [child.subid] = 1
  CYC(0x668d, 0x668e); L = alu_inc8(gb, L);
  CYC(0x668e, 0x6690); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6690, 0x6691); A = mem_rd(gb, DE);
  CYC(0x6691, 0x6692); mem_wr(gb, HL, A); // [child.var03]
  CALL_C(0x6692, objectCopyPosition_hook, 0x2242, 0x6695);

l_6695:
  CYC(0x6695, 0x6696); H = D;
  CYC(0x6696, 0x6698); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6698, 0x6699); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6699, 0x669a); A = mem_rd(gb, HL);
  CYC(0x669a, 0x669d); SET_HL(0x66bc); // @data_66bc
  CYC(0x669d, 0x669e); interactionCodeaf_addDoubleIndex(gb, 0x669e);
  // falls through into @storeVar30Value

storeVar30Value:
  CYC(0x669e, 0x669f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x669f, 0x66a1); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x66a1, 0x66a2); mem_wr(gb, DE, A);
  CYC(0x66a2, 0x66a3); E = alu_inc8(gb, E);
  CYC(0x66a3, 0x66a4); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x66a4, 0x66a5); mem_wr(gb, DE, A);
  RET(0x66a5);
  // @state0 reaches here via a plain tail `jp` with nothing of its own left to do, so its
  // "ret" really does end the whole hook (gb->pc/sp already correctly resolved to the true
  // caller). Only the @spawnChild call site above still has C code to run afterward.
  if (!(gb->pc == 0x6676 && gb->sp == sp0_)) return;

  CYC(0x6676, 0x6678); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6678, 0x6679); A = mem_rd(gb, DE);
  CYC(0x6679, 0x667a); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x667a, 0x667b); ret_effect(gb); return; } // ret nz
  CYC(0x667a, 0x667b);
  CYC(0x667b, 0x667e); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x667e, 0x6680); mem_wr(gb, HL, 0xff);
  CYC(0x6680, 0x6683); interactionDelete_hook(gb); return; // jp

subid1:
  CYC(0x66a6, 0x66a9); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x66a9, 0x66aa); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x66aa, 0x66ab); ret_effect(gb); return; } // ret nz
  CYC(0x66aa, 0x66ab);
  CALL_C(0x66ab, objectApplySpeed_hook, 0x201d, 0x66ae);
  CYC(0x66ae, 0x66af); H = D;
  CYC(0x66af, 0x66b1); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x66b1, 0x66b2); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x66b2, 0x66b3); B = A;
  CYC(0x66b3, 0x66b4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x66b4, 0x66b7); interactionDelete_hook(gb); return; } // jp z
  CYC(0x66b4, 0x66b7);
  CYC(0x66b7, 0x66b8); L = alu_inc8(gb, L);
  CYC(0x66b8, 0x66b9); C = mem_rd(gb, HL); // [xh]
  CYC(0x66b9, 0x66bc); interactionFunc_3e6d_hook(gb); return; // jp
}
