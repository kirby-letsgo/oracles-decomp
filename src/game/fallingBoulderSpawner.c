#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t fallingBoulderSpawner_jump_table(GB *gb) {
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

static void fallingBoulderSpawner_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// PART_FALLING_BOULDER_SPAWNER
void partCode45_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7530, 0x7532); E = 0xc4; // Part.state
  CYC(0x7532, 0x7533); A = mem_rd(gb, DE);
  {
    CYC(0x7533, 0x7534); push_effect(gb, 0x7534);
    uint16_t target = fallingBoulderSpawner_jump_table(gb);
    if (target == 0x7562) goto state1;
    if (target == 0x758b) goto state2;
    goto state0;
  }

state0:
  CYC(0x753a, 0x753b); H = D;
  CYC(0x753b, 0x753c); L = E;
  CYC(0x753c, 0x753d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x753d, 0x753f); L = 0xd0; // Part.speed
  CYC(0x753f, 0x7541); mem_wr(gb, HL, 0x32);
  CYC(0x7541, 0x7543); L = 0xcb; // Part.yh
  CYC(0x7543, 0x7544); A = mem_rd(gb, HL);
  CYC(0x7544, 0x7546); alu_sub(gb, 0x08);
  if (F & FZ) { CYCT(0x7546, 0x7548); goto L_754a; } // jr z
  CYC(0x7546, 0x7548);
  CYC(0x7548, 0x754a); alu_add(gb, 0x04);

L_754a:
  CYC(0x754a, 0x754b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x754b, 0x754d); E = 0xf0; // Part.var30
  CYC(0x754d, 0x754e); mem_wr(gb, DE, A);
  CYC(0x754e, 0x754f); E = alu_inc8(gb, E);
  CYC(0x754f, 0x7550); L = alu_inc8(gb, L);
  CYC(0x7550, 0x7551); A = mem_rd(gb, HL);
  CYC(0x7551, 0x7552); mem_wr(gb, DE, A);
  CYC(0x7552, 0x7554); E = 0xc2; // Part.subid
  CYC(0x7554, 0x7555); A = mem_rd(gb, DE);
  CYC(0x7555, 0x7558); SET_HL(0x755e); // table_755e (initialTimeToAppear)
  CYC(0x7558, 0x7559); fallingBoulderSpawner_addAToHl_from_rst(gb, 0x7559);
  CYC(0x7559, 0x755b); E = 0xc6; // Part.counter1
  CYC(0x755b, 0x755c); A = mem_rd(gb, HL);
  CYC(0x755c, 0x755d); mem_wr(gb, DE, A);
  RET(0x755d); return; // ret

state1:
  CALL_C(0x7562, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7565);
  if (!(F & FZ)) { RET_TAKEN(0x7565); return; } // ret nz
  CYC(0x7565, 0x7566);
  CYC(0x7566, 0x7567); L = E;
  CYC(0x7567, 0x7568); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7568, 0x756a); L = 0xe4; // Part.collisionType
  CYC(0x756a, 0x756c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

bounceRandomlyDownwards:
  CYC(0x756c, 0x756e); L = 0xd4; // Part.speedZ
  CYC(0x756e, 0x7570); A = 0x60;
  CYC(0x7570, 0x7571); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7571, 0x7573); mem_wr(gb, HL, 0xfe);

L_7573:
  CALL_C(0x7573, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7576);
  CYC(0x7576, 0x7578); alu_and(gb, 0x07);
  CYC(0x7578, 0x757a); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(0x757a, 0x757c); goto L_7573; } // jr nc
  CYC(0x757a, 0x757c);
  CYC(0x757c, 0x757e); alu_sub(gb, 0x03);
  CYC(0x757e, 0x7580); alu_add(gb, 0x10);
  CYC(0x7580, 0x7582); E = 0xc9; // Part.angle
  CYC(0x7582, 0x7583); mem_wr(gb, DE, A);
  CALL_C(0x7583, objectSetVisiblec1_hook, 0x1e3c, 0x7586);
  CYC(0x7586, 0x7588); A = 0xb3; // SND_RUMBLE
  CYC(0x7588, 0x758b); playSound_b00_hook(gb); // jp (own burn; local, inlined)
  if (gb->pc == 0x7593 && gb->sp == sp0_) goto state2_afterBounceCall;
  return;

state2:
  CYC(0x758b, 0x758d); C = 0x20;
  CALL_C(0x758d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7590);
  if (F & FZ) { CYCT(0x7590, 0x7593); push_effect(gb, 0x7593); goto bounceRandomlyDownwards; } // call z (own burn; local, inlined)
  CYC(0x7590, 0x7593);

state2_afterBounceCall:
  CALL_C(0x7593, objectApplySpeed_hook, 0x201d, 0x7596);
  CYC(0x7596, 0x7598); E = 0xcb; // Part.yh
  CYC(0x7598, 0x7599); A = mem_rd(gb, DE);
  CYC(0x7599, 0x759b); alu_cp(gb, 0x88);
  if (F & FC) { CYCT(0x759b, 0x759e); partAnimate_hook(gb); return; } // jp c
  CYC(0x759b, 0x759e);
  CYC(0x759e, 0x759f); H = D;
  CYC(0x759f, 0x75a1); L = 0xc4; // Part.state
  CYC(0x75a1, 0x75a2); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x75a2, 0x75a4); L = 0xe4; // Part.collisionType
  CYC(0x75a4, 0x75a6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x75a6, 0x75a8); L = 0xc6; // Part.counter1
  CYC(0x75a8, 0x75aa); mem_wr(gb, HL, 0xb4);
  CYC(0x75aa, 0x75ac); E = 0xf0; // Part.var30
  CYC(0x75ac, 0x75ae); L = 0xcb; // Part.yh
  CYC(0x75ae, 0x75af); A = mem_rd(gb, DE);
  CYC(0x75af, 0x75b0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x75b0, 0x75b1); E = alu_inc8(gb, E);
  CYC(0x75b1, 0x75b2); L = alu_inc8(gb, L);
  CYC(0x75b2, 0x75b3); A = mem_rd(gb, DE);
  CYC(0x75b3, 0x75b4); mem_wr(gb, HL, A);
  CYC(0x75b4, 0x75b7); objectSetInvisible_hook(gb); return; // jp
}
