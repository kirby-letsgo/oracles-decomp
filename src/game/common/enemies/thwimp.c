#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t enemyCode2e_jump_table(GB *gb) {
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

// enemyCode2e@state8: an @-local reached both by the jump table below (tail transfer) and by a
// genuine `call` from @stateB (which pushes its own return address first). Declared as a bare
// _hook (not static) only because it needs CALL_C/sp0_ internally -- never registered on its
// own, since registering enemyCode2e collapses this whole routine's @-locals out of the
// generated fallback (rule 6).
void enemyCode2e_state8_hook(GB *gb) {
  BASE(enemyCode2e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+53, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+57); ret_effect(gb); return; } // ret nz
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+58); L = E;
  CYC(b_+58, b_+59); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+59, b_+60); alu_xor(gb, A);
  RET(b_+60); return;
}

// ==================================================================================================
// ENEMY_THWIMP
//
// Variables:
//   var30: Original y-position (where it returns to after stomping)
//
// All states below (aside from @state8 above) are @-local to enemyCode2e in the disassembly (no
// separate top-level symbols), so they live inline in this one hook.
// ==================================================================================================
void enemyCode2e_hook(GB *gb) {
  BASE(enemyCode2e);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+5); ret_effect(gb); return; } // ret c
  CYC(b_+4, b_+5);

normalStatus:
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  {
    uint16_t target = enemyCode2e_jump_table(gb);
    if (target == b_+53) { enemyCode2e_state8_hook(gb); return; }
    if (target == b_+61) goto state9;
    if (target == b_+83) goto stateA;
    if (target == b_+109) goto stateB;
    if (target == b_+116) goto stateC;
    if (target == b_+35) goto state_uninitialized;
    RET(b_+52); return; // states 1-7 all target 0x66ae, a bare `ret`
  }

state_uninitialized:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+40); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+49, b_+52); TAIL(ecom_setSpeedAndState8AndVisible_b0d); // jp

  // Waiting for Link to approach
state9:
  CYC(b_+61, b_+62); H = D;
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+64, b_+66); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+66, b_+67); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+67, b_+69); alu_add(gb, 0x0a);
  CYC(b_+69, b_+71); alu_cp(gb, 0x15);
  if (!(F & FC)) { CYCT(b_+71, b_+72); ret_effect(gb); return; } // ret nc
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+73); L = E;
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+74, b_+76); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+76, b_+77); alu_xor(gb, A);
  CYC(b_+77, b_+78); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+80); A = alu_inc8(gb, A);
  CYC(b_+80, b_+83); TAIL(enemySetAnimation); // jp

  // Falling down
stateA:
  CYC(b_+83, b_+85); A = 0x40;
  CALL_C(b_+85, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+88);
  if (F & FC) { CYCT(b_+88, b_+90); goto landed; } // jr c
  CYC(b_+88, b_+90);
  // Cap speedZ to $0200 (ish... doesn't fix the low byte)
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+93); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+93, b_+94); ret_effect(gb); return; } // ret c
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x02);
  RET(b_+96); return;

landed:
  CALL_C(b_+97, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+100);
  CYC(b_+100, b_+102); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 45);
  CYC(b_+104, b_+106); A = 0x50; // SND_CLINK
  CYC(b_+106, b_+109); TAIL(playSound_b00); // jp

  // Just landed. Wait for [counter1] frames
stateB:
  CYC(b_+109, b_+112); push_effect(gb, b_+112); enemyCode2e_state8_hook(gb);
  if (!(F & FZ)) { CYCT(b_+112, b_+113); ret_effect(gb); return; } // ret nz
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+116); TAIL(enemySetAnimation); // jp

  // Moving back up at constant speed
stateC:
  CYC(b_+116, b_+117); H = D;
  CYC(b_+117, b_+119); L = ENEMY_BASE + OBJ_Y;
  CYC(b_+119, b_+120); A = mem_rd(gb, HL);
  CYC(b_+120, b_+122); alu_sub(gb, 0x80);
  CYC(b_+122, b_+123); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+123, b_+124); A = mem_rd(gb, HL);
  CYC(b_+124, b_+126); alu_sbc(gb, 0x00);
  CYC(b_+126, b_+127); mem_wr(gb, HL, A);
  CYC(b_+127, b_+129); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+131, b_+132); ret_effect(gb); return; } // ret nz
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+134); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+134, b_+136); mem_wr(gb, HL, 24);
  CYC(b_+136, b_+138); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x08);
  RET(b_+140); return;
}
