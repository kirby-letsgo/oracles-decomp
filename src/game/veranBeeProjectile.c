#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x7e1c, 0x7e1e); goto normalStatus; } // jr z
  CYC(0x7e1c, 0x7e1e);
  CYC(0x7e1e, 0x7e20); E = 0xea; // Part.var2a
  CYC(0x7e20, 0x7e21); A = mem_rd(gb, DE);
  CYC(0x7e21, 0x7e23); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x7e23, 0x7e25); goto normalStatus; } // jr nz
  CYC(0x7e23, 0x7e25);
  CYC(0x7e25, 0x7e26); H = D;
  CYC(0x7e26, 0x7e28); L = 0xe4; // Part.collisionType
  CYC(0x7e28, 0x7e2a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7e2a, 0x7e2c); L = 0xc4; // Part.state
  CYC(0x7e2c, 0x7e2e); mem_wr(gb, HL, 0x03);
  CYC(0x7e2e, 0x7e30); L = 0xc6; // Part.counter1
  CYC(0x7e30, 0x7e32); mem_wr(gb, HL, 0xf0);
  CALL_C(0x7e32, objectSetInvisible_hook, 0x1e7b, 0x7e35);

normalStatus:
  CYC(0x7e35, 0x7e37); E = 0xc4; // Part.state
  CYC(0x7e37, 0x7e38); A = mem_rd(gb, DE);
  {
    CYC(0x7e38, 0x7e39); push_effect(gb, 0x7e39);
    uint16_t target = veranBeeProjectile_jump_table(gb);
    if (target == 0x7e58) goto state1;
    if (target == 0x7e6a) goto state2;
    if (target == 0x7e76) goto state3;
    goto state0;
  }

state0:
  CYC(0x7e41, 0x7e42); H = D;
  CYC(0x7e42, 0x7e43); L = E;
  CYC(0x7e43, 0x7e44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7e44, 0x7e46); L = 0xc9; // Part.angle
  CYC(0x7e46, 0x7e48); mem_wr(gb, HL, 0x10);
  CYC(0x7e48, 0x7e4a); L = 0xd0; // Part.speed
  CYC(0x7e4a, 0x7e4c); mem_wr(gb, HL, 0x78);
  CYC(0x7e4c, 0x7e4e); L = 0xc6; // Part.counter1
  CYC(0x7e4e, 0x7e50); mem_wr(gb, HL, 0x09);
  CYC(0x7e50, 0x7e52); A = 0xa4; // SND_BEAM
  CALL_C(0x7e52, playSound_b00_hook, 0x0c98, 0x7e55);
  CALL_C(0x7e55, objectSetVisible83_hook, 0x1e72, 0x7e58);

state1:
  CALL_C(0x7e58, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7e5b);
  if (F & FZ) { CYCT(0x7e5b, 0x7e5d); goto incState; } // jr z
  CYC(0x7e5b, 0x7e5d);
  CYC(0x7e5d, 0x7e5f); A = 0x0b; // Object.state
  CALL_C(0x7e5f, objectGetRelatedObject1Var_hook, 0x2160, 0x7e62);
  CYC(0x7e62, 0x7e65); SET_BC(0x1400);
  CYC(0x7e65, 0x7e68); objectTakePositionWithOffset_hook(gb); return; // jp

incState:
  CYC(0x7e68, 0x7e69); L = E;
  CYC(0x7e69, 0x7e6a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(0x7e6a, objectApplySpeed_hook, 0x201d, 0x7e6d);
  CYC(0x7e6d, 0x7e6f); E = 0xcb; // Part.yh
  CYC(0x7e6f, 0x7e70); A = mem_rd(gb, DE);
  CYC(0x7e70, 0x7e72); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(0x7e72); return; } // ret c
  CYC(0x7e72, 0x7e73);
  CYC(0x7e73, 0x7e76); partDelete_hook(gb); return; // jp

state3:
  CALL_C(0x7e76, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7e79);
  if (F & FZ) { CYCT(0x7e79, 0x7e7c); partDelete_hook(gb); return; } // jp z
  CYC(0x7e79, 0x7e7c);
  CYC(0x7e7c, 0x7e7f); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(0x7e7f, 0x7e80); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7e80, 0x7e82); goto L_7e8a; } // jr z
  CYC(0x7e80, 0x7e82);
  CYC(0x7e82, 0x7e83); A = mem_rd(gb, HL);
  CYC(0x7e83, 0x7e85); alu_sub(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x7e85, 0x7e87); goto L_7e89; } // jr nc
  CYC(0x7e85, 0x7e87);
  CYC(0x7e87, 0x7e89); A = 0x01;

L_7e89:
  CYC(0x7e89, 0x7e8a); mem_wr(gb, HL, A);

L_7e8a:
  CYC(0x7e8a, 0x7e8d); SET_HL(wccd8);
  CYC(0x7e8d, 0x7e8f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(0x7e8f, 0x7e92); A = mem_rd(gb, wFrameCounter);
  CYC(0x7e92, 0x7e93); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x7e93); return; } // ret nc
  CYC(0x7e93, 0x7e94);
  CYC(0x7e94, 0x7e97); SET_HL(wLinkImmobilized);
  CYC(0x7e97, 0x7e99); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  RET(0x7e99); return; // ret
}
