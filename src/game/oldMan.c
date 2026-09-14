#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t oldMan_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void oldMan_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0, 0x18, 0x19, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x19, 0x1a, false); C = A;
  burn_rom(gb, 0, 0x1a, 0x1c, false); B = 0;
  burn_rom(gb, 0, 0x1c, 0x1d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1d, 0x1e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1e, 0x1f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x1f, 0x20, false); ret_effect(gb);
}

static void oldMan_load_script(GB *gb, uint16_t sp0_) {
  CALL_C(0x66a3, interactionInitGraphics_hook, 0x15fb, 0x66a6);
  CYC(0x66a6, 0x66a8); E = 0x42;
  CYC(0x66a8, 0x66a9); A = mem_rd(gb, DE);
  CYC(0x66a9, 0x66ac); SET_HL(0x66b6);
  CYC(0x66ac, 0x66ad); oldMan_add_double_index(gb, 0x66ad);
  CYC(0x66ad, 0x66ae); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x66ae, 0x66af); H = mem_rd(gb, HL);
  CYC(0x66af, 0x66b0); L = A;
  CALL_C(0x66b0, interactionSetScript_hook, 0x2544, 0x66b3);
  CYC(0x66b3, 0x66b6); interactionIncState_hook(gb);
}

void interactionCode52_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x65d9, 0x65db); E = 0x42;
  CYC(0x65db, 0x65dc); A = mem_rd(gb, DE);
  CYC(0x65dc, 0x65dd); push_effect(gb, 0x65dd);
  switch (oldMan_jump_table(gb)) {
    case 0x65eb: goto subid0;
    case 0x65fc: goto subid1;
    case 0x660b: goto subid2;
    case 0x6632: goto generic;
    default: HANDOFF(HL);
  }
subid0:
  CALL_C(0x65eb, checkInteractionState_hook, 0x23fe, 0x65ee);
  if (F & FZ) { CYC(0x65ee, 0x65f0); CYC(0x65f0, 0x65f3); push_effect(gb, 0x65f3); oldMan_load_script(gb, sp0_); }
  else CYCT(0x65ee, 0x65f0);
  CALL_C(0x65f3, interactionRunScript_hook, 0x2552, 0x65f6);
  if (F & FC) { CYCT(0x65f6, 0x65f9); interactionDelete_hook(gb); return; }
  CYC(0x65f6, 0x65f9); CYC(0x65f9, 0x65fc); npcFaceLinkAndAnimate_hook(gb); return;
subid1:
  CALL_C(0x65fc, checkInteractionState_hook, 0x23fe, 0x65ff);
  if (F & FZ) { CYCT(0x65ff, 0x6602); push_effect(gb, 0x6602); oldMan_load_script(gb, sp0_); }
  else CYC(0x65ff, 0x6602);
  CALL_C(0x6602, interactionRunScript_hook, 0x2552, 0x6605);
  if (F & FC) { CYCT(0x6605, 0x6608); interactionDelete_hook(gb); return; }
  CYC(0x6605, 0x6608); CYC(0x6608, 0x660b); interactionAnimateAsNpc_hook(gb); return;
subid2:
  CALL_C(0x660b, checkInteractionState_hook, 0x23fe, 0x660e);
  if (F & FZ) {
    CYC(0x660e, 0x6610); CYC(0x6610, 0x6612); A = 0x14;
    CALL_C(0x6612, checkGlobalFlag_hook, 0x31f3, 0x6615);
    if (!(F & FZ)) { CYCT(0x6615, 0x6618); interactionDelete_hook(gb); return; }
    CYC(0x6615, 0x6618); CYC(0x6618, 0x661b); push_effect(gb, 0x661b); oldMan_load_script(gb, sp0_);
  } else CYCT(0x660e, 0x6610);
  CALL_C(0x661b, interactionAnimateAsNpc_hook, 0x26db, 0x661e);
  CALL_C(0x661e, interactionRunScript_hook, 0x2552, 0x6621);
  if (!(F & FC)) { CYCT(0x6621, 0x6622); ret_effect(gb); return; }
  CYC(0x6621, 0x6622); CYC(0x6622, 0x6624); A = 0x8d;
  CALL_C(0x6624, playSound_b00_hook, 0x0c98, 0x6627);
  CYC(0x6627, 0x662a); SET_HL(0x662d);
  CYC(0x662a, 0x662d); setWarpDestVariables_hook(gb); return;
generic:
  CALL_C(0x6632, checkInteractionState_hook, 0x23fe, 0x6635);
  if (!(F & FZ)) { CYCT(0x6635, 0x6637); goto generic_state1; }
  CYC(0x6635, 0x6637);
  CALL_C(0x663d, interactionInitGraphics_hook, 0x15fb, 0x6640);
  CALL_C(0x6640, interactionIncState_hook, 0x23e0, 0x6643);
  CYC(0x6643, 0x6645); L = 0x73; CYC(0x6645, 0x6647); mem_wr(gb, HL, 0x33);
  CYC(0x6647, 0x6649); L = 0x67; CYC(0x6649, 0x664b); mem_wr(gb, HL, 6);
  CYC(0x664b, 0x664d); L = 0x48; CYC(0x664d, 0x664e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x664e, 0x6650); A = 0x30; CALL_C(0x6650, checkGlobalFlag_hook, 0x31f3, 0x6653);
  CYC(0x6653, 0x6655); B = 0;
  if (F & FZ) CYCT(0x6655, 0x6657); else { CYC(0x6655, 0x6657); CYC(0x6657, 0x6658); B = alu_inc8(gb, B); }
  CYC(0x6658, 0x665a); E = 0x42; CYC(0x665a, 0x665b); A = mem_rd(gb, DE);
  CYC(0x665b, 0x665d); alu_sub(gb, 3); CYC(0x665d, 0x665e); C = A;
  CYC(0x665e, 0x665f); alu_add(gb, A); CYC(0x665f, 0x6660); alu_add(gb, B);
  CYC(0x6660, 0x6663); SET_HL(0x6695); CYC(0x6663, 0x6664); oldMan_add_double_index(gb, 0x6664);
  CYC(0x6664, 0x6666); E = 0x72; CYC(0x6666, 0x6667); A = mem_rd(gb, HL); CYC(0x6667, 0x6668); mem_wr(gb, DE, A);
  CYC(0x6668, 0x6669); A = C; CYC(0x6669, 0x666a); alu_add(gb, A); CYC(0x666a, 0x666b); alu_add(gb, C);
  CYC(0x666b, 0x666e); SET_HL(0x6689); CYC(0x666e, 0x666f); oldMan_add_double_index(gb, 0x666f);
  CYC(0x666f, 0x6671); E = 0x66; CYC(0x6671, 0x6672); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6672, 0x6673); mem_wr(gb, DE, A);
  CYC(0x6673, 0x6675); E = 0x5b; CYC(0x6675, 0x6676); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x6676, 0x6677); mem_wr(gb, DE, A);
  CYC(0x6677, 0x6678); E = alu_inc8(gb, E); CYC(0x6678, 0x6679); mem_wr(gb, DE, A);
  CYC(0x6679, 0x667b); E = 0x78; CYC(0x667b, 0x667c); A = mem_rd(gb, HL); CYC(0x667c, 0x667d); mem_wr(gb, DE, A);
  CALL_C(0x667d, interactionSetAnimation_hook, 0x262e, 0x6680); CALL_C(0x6680, objectSetVisiblec2_hook, 0x1e45, 0x6683);
  CYC(0x6683, 0x6686); SET_HL(0x64fc); CYC(0x6686, 0x6689); interactionSetScript_hook(gb); return;
generic_state1:
  CALL_C(0x6637, interactionRunScript_hook, 0x2552, 0x663a); CYC(0x663a, 0x663d); interactionAnimateAsNpc_hook(gb);
}
