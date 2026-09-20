#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode52), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode52), (from), (to), true)

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
  BASE(interactionCode52);
  CALL_C(b_+202, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+205);
  CYC(b_+205, b_+207); E = 0x42;
  CYC(b_+207, b_+208); A = mem_rd(gb, DE);
  CYC(b_+208, b_+211); SET_HL(b_+221);
  CYC(b_+211, b_+212); oldMan_add_double_index(gb, b_+212);
  CYC(b_+212, b_+213); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+213, b_+214); H = mem_rd(gb, HL);
  CYC(b_+214, b_+215); L = A;
  CALL_C(b_+215, interactionSetScript_hook, SYM(interactionSetScript), b_+218);
  CYC(b_+218, b_+221); interactionIncState_hook(gb);
}

void interactionCode52_hook(GB *gb) {
  BASE(interactionCode52);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (oldMan_jump_table(gb));
    if (jt_ == b_+18) { goto subid0; }
    else if (jt_ == b_+35) { goto subid1; }
    else if (jt_ == b_+50) { goto subid2; }
    else if (jt_ == b_+89) { goto generic; }
    else { HANDOFF(HL); }
  } while (0);
subid0:
  CALL_C(b_+18, checkInteractionState_hook, SYM(checkInteractionState), b_+21);
  if (F & FZ) { CYC(b_+21, b_+23); CYC(b_+23, b_+26); push_effect(gb, b_+26); oldMan_load_script(gb, sp0_); }
  else CYCT(b_+21, b_+23);
  CALL_C(b_+26, interactionRunScript_hook, SYM(interactionRunScript), b_+29);
  if (F & FC) { CYCT(b_+29, b_+32); interactionDelete_hook(gb); return; }
  CYC(b_+29, b_+32); CYC(b_+32, b_+35); npcFaceLinkAndAnimate_hook(gb); return;
subid1:
  CALL_C(b_+35, checkInteractionState_hook, SYM(checkInteractionState), b_+38);
  if (F & FZ) { CYCT(b_+38, b_+41); push_effect(gb, b_+41); oldMan_load_script(gb, sp0_); }
  else CYC(b_+38, b_+41);
  CALL_C(b_+41, interactionRunScript_hook, SYM(interactionRunScript), b_+44);
  if (F & FC) { CYCT(b_+44, b_+47); interactionDelete_hook(gb); return; }
  CYC(b_+44, b_+47); CYC(b_+47, b_+50); interactionAnimateAsNpc_hook(gb); return;
subid2:
  CALL_C(b_+50, checkInteractionState_hook, SYM(checkInteractionState), b_+53);
  if (F & FZ) {
    CYC(b_+53, b_+55); CYC(b_+55, b_+57); A = 0x14;
    CALL_C(b_+57, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+60);
    if (!(F & FZ)) { CYCT(b_+60, b_+63); interactionDelete_hook(gb); return; }
    CYC(b_+60, b_+63); CYC(b_+63, b_+66); push_effect(gb, b_+66); oldMan_load_script(gb, sp0_);
  } else CYCT(b_+53, b_+55);
  CALL_C(b_+66, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+69);
  CALL_C(b_+69, interactionRunScript_hook, SYM(interactionRunScript), b_+72);
  if (!(F & FC)) { CYCT(b_+72, b_+73); ret_effect(gb); return; }
  CYC(b_+72, b_+73); CYC(b_+73, b_+75); A = 0x8d;
  CALL_C(b_+75, playSound_b00_hook, SYM(playSound_b00), b_+78);
  CYC(b_+78, b_+81); SET_HL(b_+84);
  CYC(b_+81, b_+84); setWarpDestVariables_hook(gb); return;
generic:
  CALL_C(b_+89, checkInteractionState_hook, SYM(checkInteractionState), b_+92);
  if (!(F & FZ)) { CYCT(b_+92, b_+94); goto generic_state1; }
  CYC(b_+92, b_+94);
  CALL_C(b_+100, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+103);
  CALL_C(b_+103, interactionIncState_hook, SYM(interactionIncState), b_+106);
  CYC(b_+106, b_+108); L = 0x73; CYC(b_+108, b_+110); mem_wr(gb, HL, 0x33);
  CYC(b_+110, b_+112); L = 0x67; CYC(b_+112, b_+114); mem_wr(gb, HL, 6);
  CYC(b_+114, b_+116); L = 0x48; CYC(b_+116, b_+117); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+117, b_+119); A = 0x30; CALL_C(b_+119, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+122);
  CYC(b_+122, b_+124); B = 0;
  if (F & FZ) CYCT(b_+124, b_+126); else { CYC(b_+124, b_+126); CYC(b_+126, b_+127); B = alu_inc8(gb, B); }
  CYC(b_+127, b_+129); E = 0x42; CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+132); alu_sub(gb, 3); CYC(b_+132, b_+133); C = A;
  CYC(b_+133, b_+134); alu_add(gb, A); CYC(b_+134, b_+135); alu_add(gb, B);
  CYC(b_+135, b_+138); SET_HL(b_+188); CYC(b_+138, b_+139); oldMan_add_double_index(gb, b_+139);
  CYC(b_+139, b_+141); E = 0x72; CYC(b_+141, b_+142); A = mem_rd(gb, HL); CYC(b_+142, b_+143); mem_wr(gb, DE, A);
  CYC(b_+143, b_+144); A = C; CYC(b_+144, b_+145); alu_add(gb, A); CYC(b_+145, b_+146); alu_add(gb, C);
  CYC(b_+146, b_+149); SET_HL(b_+176); CYC(b_+149, b_+150); oldMan_add_double_index(gb, b_+150);
  CYC(b_+150, b_+152); E = 0x66; CYC(b_+152, b_+153); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+153, b_+154); mem_wr(gb, DE, A);
  CYC(b_+154, b_+156); E = 0x5b; CYC(b_+156, b_+157); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(b_+157, b_+158); mem_wr(gb, DE, A);
  CYC(b_+158, b_+159); E = alu_inc8(gb, E); CYC(b_+159, b_+160); mem_wr(gb, DE, A);
  CYC(b_+160, b_+162); E = 0x78; CYC(b_+162, b_+163); A = mem_rd(gb, HL); CYC(b_+163, b_+164); mem_wr(gb, DE, A);
  CALL_C(b_+164, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+167); CALL_C(b_+167, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+170);
  CYC(b_+170, b_+173); SET_HL((SYM(impaNpc_subid01) + 19)); CYC(b_+173, b_+176); interactionSetScript_hook(gb); return;
generic_state1:
  CALL_C(b_+94, interactionRunScript_hook, SYM(interactionRunScript), b_+97); CYC(b_+97, b_+100); interactionAnimateAsNpc_hook(gb);
}
