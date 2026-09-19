#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodec9_jump_table(GB *gb) {
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

// 0b:761e, tail-jumped into once from interactionCodec9_hook (top-level).
void interactionCodec9_updateAnimation_hook(GB *gb) {
  CYC(0x761e, 0x7620); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7620, 0x7621); A = mem_rd(gb, DE);
  CYC(0x7621, 0x7622); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7622, 0x7623); ret_effect(gb); return; } // ret z
  CYC(0x7622, 0x7623);
  CYC(0x7623, 0x7626); interactionAnimate_hook(gb); return; // jp
}

// 0b:76f0, called (real `call`) from interactionCodec9@state0, and reached by plain fallthrough
// from interactionCodec9_updateHopping_hook.
void interactionCodec9_beginHop_hook(GB *gb) {
  CYC(0x76f0, 0x76f3); SET_BC(0xff40);
  CYC(0x76f3, 0x76f6); objectSetSpeedZ_hook(gb); return; // jp
}

// 0b:76e9, called (real `call`) from interactionCodec9@state1, @state2 and @state3.
void interactionCodec9_updateHopping_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76e9, 0x76eb); C = 0x20;
  CALL_C(0x76eb, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x76ee);
  if (!(F & FZ)) { CYCT(0x76ee, 0x76ef); ret_effect(gb); return; } // ret nz
  CYC(0x76ee, 0x76ef);
  CYC(0x76ef, 0x76f0); H = D;
  interactionCodec9_beginHop_hook(gb); return; // falls through into @beginHop
}

// 0b:76f6, called (real `call`) once from interactionCodec9@state1.
void interactionCodec9_updateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76f6, objectApplySpeed_hook, 0x201d, 0x76f9);
  CYC(0x76f9, 0x76fb); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x76fb, 0x76fc); A = mem_rd(gb, DE);
  CYC(0x76fc, 0x76fe); alu_sub(gb, 0x68);
  CYC(0x76fe, 0x7700); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(0x7700, 0x7701); ret_effect(gb); return; } // ret c
  CYC(0x7700, 0x7701);
  CYC(0x7701, 0x7703); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7703, 0x7704); A = mem_rd(gb, DE);
  CYC(0x7704, 0x7706); alu_xor(gb, 0x10);
  CYC(0x7706, 0x7707); mem_wr(gb, DE, A);
  CYC(0x7707, 0x7709); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x7709, 0x770a); A = mem_rd(gb, DE);
  CYC(0x770a, 0x770c); alu_xor(gb, 0x01);
  CYC(0x770c, 0x770d); mem_wr(gb, DE, A);
  CYC(0x770d, 0x7710); interactionSetAnimation_hook(gb); return; // jp
}

// 0b:768f, real jump-table target reached by `jp` from interactionCodec9@state2, and by
// tail-jump from interactionCodec9_gotoState4_hook / interactionCodec9_func_766f_hook.
void interactionCodec9_setScriptAndGotoState4_hook(GB *gb) {
  CYC(0x768f, 0x7691); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7691, 0x7693); A = 0x04;
  CYC(0x7693, 0x7694); mem_wr(gb, DE, A);
  CYC(0x7694, 0x7697); interactionSetScript_hook(gb); return; // jp
}

// 0b:768c, unreachable in practice (only interactionCodec9_func_766f_hook's dead `jr z` branch
// targets it, and that branch's own tail-jump path bypasses it entirely; zero real callers).
void interactionCodec9_gotoState4_hook(GB *gb) {
  CYC(0x768c, 0x768f); SET_HL(0x7ecd); // script pointer
  interactionCodec9_setScriptAndGotoState4_hook(gb); return;
}

// 0b:766f, unused (zero callers): none of interactionCodec9's states ever jump or call here.
void interactionCodec9_func_766f_hook(GB *gb) {
  CYC(0x766f, 0x7670); alu_xor(gb, A);
  CYC(0x7670, 0x7671); mem_wr(gb, DE, A);
  CYC(0x7671, 0x7673); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7673, 0x7674); mem_wr(gb, DE, A);
  CYC(0x7674, 0x7676); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x7676, 0x7678); A = 0x01;
  CYC(0x7678, 0x7679); mem_wr(gb, DE, A);
  CYC(0x7679, 0x767c); A = mem_rd(gb, wLinkGrabState);
  CYC(0x767c, 0x767d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x767d, 0x767f); interactionCodec9_gotoState4_hook(gb); return; } // jr z
  CYC(0x767d, 0x767f);
  CYC(0x767f, 0x7682); A = mem_rd(gb, w1Link_relatedObj2 + 1);
  CYC(0x7682, 0x7683); H = A;
  CYC(0x7683, 0x7685); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7685, 0x7686); mem_wr(gb, DE, A);
  CYC(0x7686, 0x7689); SET_HL(0x7ecd); // script pointer
  CYC(0x7689, 0x768c); interactionCodec9_setScriptAndGotoState4_hook(gb); return; // jp $768f (skips @gotoState4)
}

// 0b:7610, the state dispatcher. Called (real `call`) once from interactionCodec9_hook.
void interactionCodec9_runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7610, 0x7612); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7612, 0x7613); A = mem_rd(gb, DE);
  CYC(0x7613, 0x7614); push_effect(gb, 0x7614);
  switch (interactionCodec9_jump_table(gb)) {
    case 0x7626: goto state0;
    case 0x7646: goto state1;
    case 0x7697: goto state2;
    case 0x76b1: goto state3;
    case 0x76c9: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x7626, 0x7628); A = 0x01;
  CYC(0x7628, 0x7629); mem_wr(gb, DE, A);
  CALL_C(0x7629, interactionInitGraphics_hook, 0x15fb, 0x762c);
  CYC(0x762c, 0x762d); H = D;
  CYC(0x762d, 0x762f); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x762f, 0x7631); mem_wr(gb, HL, 0x06);
  CYC(0x7631, 0x7632); L = alu_inc8(gb, L);
  CYC(0x7632, 0x7634); mem_wr(gb, HL, 0x06);
  CYC(0x7634, 0x7636); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7636, 0x7638); mem_wr(gb, HL, 0x19);
  CALL_C(0x7638, interactionCodec9_beginHop_hook, 0x76f0, 0x763b);
  CYC(0x763b, 0x763d); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x763d, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x7640);
  CALL_C(0x7640, objectSetVisible80_hook, 0x1e57, 0x7643);
  CYC(0x7643, 0x7646); goto func_7710; // jp

state1:
  CALL_C(0x7646, interactionCodec9_updateHopping_hook, 0x76e9, 0x7649);
  CALL_C(0x7649, interactionCodec9_updateMovement_hook, 0x76f6, 0x764c);
  CYC(0x764c, 0x764f); SET_HL(w1Link_yh);
  CYC(0x764f, 0x7651); C = 0x69;
  CYC(0x7651, 0x7652); B = mem_rd(gb, HL);
  CYC(0x7652, 0x7654); A = 0x69;
  CYC(0x7654, 0x7655); L = A;
  CYC(0x7655, 0x7656); A = C;
  CYC(0x7656, 0x7657); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(0x7657, 0x7658); ret_effect(gb); return; } // ret nc
  CYC(0x7657, 0x7658);
  CYC(0x7658, 0x765b); A = mem_rd(gb, wLinkGrabState);
  CYC(0x765b, 0x765c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x765c, 0x765d); ret_effect(gb); return; } // ret z
  CYC(0x765c, 0x765d);
  CYC(0x765d, 0x765f); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x765f, 0x7661); A = 0x02;
  CYC(0x7661, 0x7662); mem_wr(gb, DE, A);
  CYC(0x7662, 0x7664); A = 0x80;
  CYC(0x7664, 0x7667); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7667, 0x7668); A = L;
  CYC(0x7668, 0x766b); SET_HL(w1Link_yh);
  CYC(0x766b, 0x766c); mem_wr(gb, HL, A);
  CYC(0x766c, 0x766f); goto initState2; // jp

state2:
  CALL_C(0x7697, interactionCodec9_updateHopping_hook, 0x76e9, 0x769a);
  CALL_C(0x769a, objectApplySpeed_hook, 0x201d, 0x769d);
  CYC(0x769d, 0x769f); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x769f, 0x76a0); A = mem_rd(gb, DE);
  CYC(0x76a0, 0x76a2); alu_sub(gb, 0x0c);
  CYC(0x76a2, 0x76a5); SET_HL(w1Link_xh);
  CYC(0x76a5, 0x76a6); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x76a6, 0x76a7); ret_effect(gb); return; } // ret nc
  CYC(0x76a6, 0x76a7);
  CYC(0x76a7, 0x76a9); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x76a9, 0x76aa); alu_xor(gb, A);
  CYC(0x76aa, 0x76ab); mem_wr(gb, DE, A);
  CYC(0x76ab, 0x76ae); SET_HL(0x7ecd); // script pointer
  CYC(0x76ae, 0x76b1); interactionCodec9_setScriptAndGotoState4_hook(gb); return; // jp $768f

state3:
  CALL_C(0x76b1, interactionCodec9_updateHopping_hook, 0x76e9, 0x76b4);
  CALL_C(0x76b4, objectApplySpeed_hook, 0x201d, 0x76b7);
  CYC(0x76b7, 0x76b9); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x76b9, 0x76ba); A = mem_rd(gb, DE);
  CYC(0x76ba, 0x76bc); alu_cp(gb, 0x78);
  if (F & FC) { CYCT(0x76bc, 0x76bd); ret_effect(gb); return; } // ret c
  CYC(0x76bc, 0x76bd);
  CYC(0x76bd, 0x76be); alu_xor(gb, A);
  CYC(0x76be, 0x76c1); mem_wr(gb, wDisabledObjects, A);
  CYC(0x76c1, 0x76c3); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x76c3, 0x76c5); A = 0x01;
  CYC(0x76c5, 0x76c6); mem_wr(gb, DE, A);
  CYC(0x76c6, 0x76c9); goto func_7710; // jp

state4:
  CALL_C(0x76c9, interactionRunScript_hook, 0x2552, 0x76cc);
  if (!(F & FC)) { CYCT(0x76cc, 0x76cd); ret_effect(gb); return; } // ret nc
  CYC(0x76cc, 0x76cd);
  CYC(0x76cd, 0x76cf); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x76cf, 0x76d0); A = mem_rd(gb, DE);
  CYC(0x76d0, 0x76d2); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x76d2, 0x76d4); goto beginMovingBack; } // jr z
  CYC(0x76d2, 0x76d4);
  CYC(0x76d4, 0x76d5); H = D;
  CYC(0x76d5, 0x76d7); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x76d7, 0x76d9); mem_wr(gb, HL, 0x01);
  CYC(0x76d9, 0x76db); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x76db, 0x76dd); mem_wr(gb, HL, 0x00);
  CYC(0x76dd, 0x76df); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x76df, 0x76e1); mem_wr(gb, HL, 0x01);
  CYC(0x76e1, 0x76e2); alu_xor(gb, A);
  CYC(0x76e2, 0x76e5); mem_wr(gb, wDisabledObjects, A);
  RET(0x76e5); return;

beginMovingBack:
  CYC(0x76e6, 0x76e9); goto initState3; // jp

func_7710:
  CYC(0x7710, 0x7711); H = D;
  CYC(0x7711, 0x7713); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x7713, 0x7715); mem_wr(gb, HL, 0x00);
  CYC(0x7715, 0x7717); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7717, 0x7719); mem_wr(gb, HL, 0x14);
  CYC(0x7719, 0x771b); goto l_7724; // jr

initState2:
  CYC(0x771b, 0x771c); H = D;
  CYC(0x771c, 0x771e); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x771e, 0x7720); mem_wr(gb, HL, 0x02);
  CYC(0x7720, 0x7722); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7722, 0x7724); mem_wr(gb, HL, 0x50);
  // falls through into l_7724

l_7724:
  CYC(0x7724, 0x7726); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7726, 0x7728); mem_wr(gb, HL, 0x01);
  CYC(0x7728, 0x772a); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x772a, 0x772c); mem_wr(gb, HL, 0x18);
  CYC(0x772c, 0x772d); alu_xor(gb, A);
  CYC(0x772d, 0x772f); L = INTERACTION_BASE + OBJ_Z;
  CYC(0x772f, 0x7730); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x7730, 0x7731); mem_wr(gb, HL, A); // ld (hl),a
  CYC(0x7731, 0x7733); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x7733, 0x7735); A = 0x00;
  CYC(0x7735, 0x7736); mem_wr(gb, HL, A);
  CYC(0x7736, 0x7739); interactionSetAnimation_hook(gb); return; // jp

initState3:
  CYC(0x7739, 0x773a); H = D;
  CYC(0x773a, 0x773c); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x773c, 0x773e); mem_wr(gb, HL, 0x03);
  CYC(0x773e, 0x7740); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7740, 0x7742); mem_wr(gb, HL, 0x50);
  CYC(0x7742, 0x7744); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x7744, 0x7746); mem_wr(gb, HL, 0x01);
  CYC(0x7746, 0x7748); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7748, 0x774a); mem_wr(gb, HL, 0x08);
  CYC(0x774a, 0x774b); alu_xor(gb, A);
  CYC(0x774b, 0x774d); L = INTERACTION_BASE + OBJ_Z;
  CYC(0x774d, 0x774e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x774e, 0x774f); mem_wr(gb, HL, A); // ld (hl),a
  CYC(0x774f, 0x7751); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x7751, 0x7753); A = 0x01;
  CYC(0x7753, 0x7754); mem_wr(gb, HL, A);
  CYC(0x7754, 0x7757); interactionSetAnimation_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_SYRUP_CUCCO
// ==================================================================================================
void interactionCodec9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x760a, interactionCodec9_runState_hook, 0x7610, 0x760d);
  CYC(0x760d, 0x7610); interactionCodec9_updateAnimation_hook(gb); return; // jp
}
