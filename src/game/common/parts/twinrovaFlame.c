#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t twinrovaFlame_jump_table(GB *gb) {
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

void partCode4c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x59ee, 0x59f0); goto normalStatus; } // jr z
  CYC(0x59ee, 0x59f0);
  CYC(0x59f0, 0x59f2); E = 0xea; // Part.var2a
  CYC(0x59f2, 0x59f3); A = mem_rd(gb, DE);
  CYC(0x59f3, 0x59f5); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x59f5, 0x59f8); partDelete_hook(gb); return; } // jp nz
  CYC(0x59f5, 0x59f8);

normalStatus:
  CYC(0x59f8, 0x59fa); E = 0xc2; // Part.subid
  CYC(0x59fa, 0x59fb); A = mem_rd(gb, DE);
  CYC(0x59fb, 0x59fc); alu_or(gb, A);
  CYC(0x59fc, 0x59fe); E = 0xc4; // Part.state
  CYC(0x59fe, 0x59ff); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(0x59ff, 0x5a01); goto subid0; } // jr z
  CYC(0x59ff, 0x5a01);
  CYC(0x5a01, 0x5a02); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5a02, 0x5a04); goto L_5a11; } // jr z
  CYC(0x5a02, 0x5a04);
  CALL_C(0x5a04, partAnimate_hook, 0x2978, 0x5a07);
  CALL_C(0x5a07, objectApplySpeed_hook, 0x201d, 0x5a0a);
  CALL_C(0x5a0a, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5a0d);
  if (!(F & FZ)) { RET_TAKEN(0x5a0d); return; } // ret nz
  CYC(0x5a0d, 0x5a0e);
  CYC(0x5a0e, 0x5a11); partDelete_hook(gb); return; // jp

L_5a11:
  CYC(0x5a11, 0x5a12); H = D;
  CYC(0x5a12, 0x5a13); L = E;
  CYC(0x5a13, 0x5a14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5a14, 0x5a16); L = 0xd0; // Part.speed
  CYC(0x5a16, 0x5a18); mem_wr(gb, HL, 0x50);
  CYC(0x5a18, 0x5a1a); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x5a1a, 0x5a1c); A = 0x05;
  CYC(0x5a1c, 0x5a1d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5a1d, 0x5a1e); mem_wr(gb, HL, A);
  CYC(0x5a1e, 0x5a20); L = 0xe6; // Part.collisionRadiusY
  CYC(0x5a20, 0x5a22); A = 0x02;
  CYC(0x5a22, 0x5a23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5a23, 0x5a24); mem_wr(gb, HL, A);
  CYC(0x5a24, 0x5a26); A = 0xbb; // SND_BEAM2
  CALL_C(0x5a26, playSound_b00_hook, 0x0c98, 0x5a29);
  CYC(0x5a29, 0x5a2b); A = 0x01;
  CALL_C(0x5a2b, partSetAnimation_hook, 0x2988, 0x5a2e);
  CYC(0x5a2e, 0x5a31); objectSetVisible82_hook(gb); return; // jp

subid0:
  CYC(0x5a31, 0x5a32); push_effect(gb, 0x5a32);
  {
    uint16_t target = twinrovaFlame_jump_table(gb);
    if (target == 0x5a38) goto state0;
    if (target == 0x5a46) goto state1;
    goto state2;
  }

state0:
  CYC(0x5a38, 0x5a39); H = D;
  CYC(0x5a39, 0x5a3a); L = E;
  CYC(0x5a3a, 0x5a3b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5a3b, 0x5a3d); L = 0xd0; // Part.speed
  CYC(0x5a3d, 0x5a3f); mem_wr(gb, HL, 0x46);
  CYC(0x5a3f, 0x5a41); L = 0xc6; // Part.counter1
  CYC(0x5a41, 0x5a43); mem_wr(gb, HL, 0x1e);
  CYC(0x5a43, 0x5a46); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x5a46, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5a49);
  if (!(F & FZ)) { CYCT(0x5a49, 0x5a4c); partAnimate_hook(gb); return; } // jp nz
  CYC(0x5a49, 0x5a4c);
  CYC(0x5a4c, 0x5a4d); L = E;
  CYC(0x5a4d, 0x5a4e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x5a4e, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5a51);
  CYC(0x5a51, 0x5a53); E = 0xc9; // Part.angle
  CYC(0x5a53, 0x5a54); mem_wr(gb, DE, A);

state2:
  CALL_C(0x5a54, partAnimate_hook, 0x2978, 0x5a57);
  CALL_C(0x5a57, objectApplySpeed_hook, 0x201d, 0x5a5a);
  CALL_C(0x5a5a, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5a5d);
  if (!(F & FC)) { RET_TAKEN(0x5a5d); return; } // ret nc
  CYC(0x5a5d, 0x5a5e);
  CALL_C(0x5a5e, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5a61);
  CYC(0x5a61, 0x5a63); alu_sub(gb, 0x02);
  CYC(0x5a63, 0x5a65); alu_and(gb, 0x1f);
  CYC(0x5a65, 0x5a66); C = A;
  CYC(0x5a66, 0x5a68); B = 0x03;

L_5a68:
  CALL_C(0x5a68, getFreePartSlot_hook, 0x3e8e, 0x5a6b);
  if (!(F & FZ)) { CYCT(0x5a6b, 0x5a6d); goto L_5a77; } // jr nz
  CYC(0x5a6b, 0x5a6d);
  CYC(0x5a6d, 0x5a6f); mem_wr(gb, HL, 0x4c); // PART_TWINROVA_FLAME
  CYC(0x5a6f, 0x5a70); L = alu_inc8(gb, L);
  CYC(0x5a70, 0x5a71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5a71, 0x5a73); L = 0xc9; // Part.angle
  CYC(0x5a73, 0x5a74); mem_wr(gb, HL, C);
  CALL_C(0x5a74, objectCopyPosition_hook, 0x2242, 0x5a77);

L_5a77:
  CYC(0x5a77, 0x5a78); A = C;
  CYC(0x5a78, 0x5a7a); alu_add(gb, 0x02);
  CYC(0x5a7a, 0x5a7c); alu_and(gb, 0x1f);
  CYC(0x5a7c, 0x5a7d); C = A;
  CYC(0x5a7d, 0x5a7e); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x5a7e, 0x5a80); goto L_5a68; } // jr nz
  CYC(0x5a7e, 0x5a80);
  CALL_C(0x5a80, objectCreatePuff_hook, 0x24c1, 0x5a83);
  CYC(0x5a83, 0x5a86); partDelete_hook(gb); return; // jp
}
