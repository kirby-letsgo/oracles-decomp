#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t veranBeeProjectile_jump_table(GB *gb) {
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

// PART_VERAN_BEE_PROJECTILE
void partCode58_hook(GB *gb) {
  BASE(partCode58);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto normalStatus; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0xe4; // Part.collisionType
  CYC(b_+12, b_+14); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+14, b_+16); L = 0xc4; // Part.state
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x03);
  CYC(b_+18, b_+20); L = 0xc6; // Part.counter1
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+22, objectSetInvisible_hook, SYM(objectSetInvisible), b_+25);

normalStatus:
  CYC(b_+25, b_+27); E = 0xc4; // Part.state
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  {
    CYC(b_+28, b_+29); push_effect(gb, b_+29);
    uint16_t target = veranBeeProjectile_jump_table(gb);
    if (target == b_+60) goto state1;
    if (target == b_+78) goto state2;
    if (target == b_+90) goto state3;
    goto state0;
  }

state0:
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+39); L = E;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+40, b_+42); L = 0xc9; // Part.angle
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x10);
  CYC(b_+44, b_+46); L = 0xd0; // Part.speed
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x78);
  CYC(b_+48, b_+50); L = 0xc6; // Part.counter1
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x09);
  CYC(b_+52, b_+54); A = 0xa4; // SND_BEAM
  CALL_C(b_+54, playSound_b00_hook, SYM(playSound_b00), b_+57);
  CALL_C(b_+57, objectSetVisible83_hook, SYM(objectSetVisible83), b_+60);

state1:
  CALL_C(b_+60, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+63);
  if (F & FZ) { CYCT(b_+63, b_+65); goto incState; } // jr z
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); A = 0x0b; // Object.state
  CALL_C(b_+67, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+70);
  CYC(b_+70, b_+73); SET_BC(0x1400);
  CYC(b_+73, b_+76); TAIL(objectTakePositionWithOffset); // jp

incState:
  CYC(b_+76, b_+77); L = E;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(b_+78, objectApplySpeed_hook, SYM(objectApplySpeed), b_+81);
  CYC(b_+81, b_+83); E = 0xcb; // Part.yh
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+86); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(b_+86); return; } // ret c
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+90); TAIL(partDelete); // jp

state3:
  CALL_C(b_+90, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+93);
  if (F & FZ) { CYCT(b_+93, b_+96); partDelete_hook(gb); return; } // jp z
  CYC(b_+93, b_+96);
  CYC(b_+96, b_+99); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+99, b_+100); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+100, b_+102); goto L_7e8a; } // jr z
  CYC(b_+100, b_+102);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_sub(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+105, b_+107); goto L_7e89; } // jr nc
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); A = 0x01;

L_7e89:
  CYC(b_+109, b_+110); mem_wr(gb, HL, A);

L_7e8a:
  CYC(b_+110, b_+113); SET_HL(wccd8);
  CYC(b_+113, b_+115); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(b_+115, b_+118); A = mem_rd(gb, wFrameCounter);
  CYC(b_+118, b_+119); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+119); return; } // ret nc
  CYC(b_+119, b_+120);
  CYC(b_+120, b_+123); SET_HL(wLinkImmobilized);
  CYC(b_+123, b_+125); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  RET(b_+125); return; // ret
}
