#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode4c), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode4c), (from), (to), true)

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
  BASE(partCode4c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); partDelete_hook(gb); return; } // jp nz
  CYC(b_+7, b_+10);

normalStatus:
  CYC(b_+10, b_+12); E = 0xc2; // Part.subid
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  CYC(b_+14, b_+16); E = 0xc4; // Part.state
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+17, b_+19); goto subid0; } // jr z
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+20, b_+22); goto L_5a11; } // jr z
  CYC(b_+20, b_+22);
  CALL_C(b_+22, partAnimate_hook, SYM(partAnimate), b_+25);
  CALL_C(b_+25, objectApplySpeed_hook, SYM(objectApplySpeed), b_+28);
  CALL_C(b_+28, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+31);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; } // ret nz
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+35); partDelete_hook(gb); return; // jp

L_5a11:
  CYC(b_+35, b_+36); H = D;
  CYC(b_+36, b_+37); L = E;
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+38, b_+40); L = 0xd0; // Part.speed
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x50);
  CYC(b_+42, b_+44); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+44, b_+46); A = 0x05;
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+50); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+50, b_+52); A = 0x02;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  CYC(b_+54, b_+56); A = 0xbb; // SND_BEAM2
  CALL_C(b_+56, playSound_b00_hook, SYM(playSound_b00), b_+59);
  CYC(b_+59, b_+61); A = 0x01;
  CALL_C(b_+61, partSetAnimation_hook, SYM(partSetAnimation), b_+64);
  CYC(b_+64, b_+67); objectSetVisible82_hook(gb); return; // jp

subid0:
  CYC(b_+67, b_+68); push_effect(gb, b_+68);
  {
    uint16_t target = twinrovaFlame_jump_table(gb);
    if (target == b_+74) goto state0;
    if (target == b_+88) goto state1;
    goto state2;
  }

state0:
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+76); L = E;
  CYC(b_+76, b_+77); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+77, b_+79); L = 0xd0; // Part.speed
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x46);
  CYC(b_+81, b_+83); L = 0xc6; // Part.counter1
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x1e);
  CYC(b_+85, b_+88); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+88, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+91);
  if (!(F & FZ)) { CYCT(b_+91, b_+94); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+91, b_+94);
  CYC(b_+94, b_+95); L = E;
  CYC(b_+95, b_+96); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+96, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+99);
  CYC(b_+99, b_+101); E = 0xc9; // Part.angle
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);

state2:
  CALL_C(b_+102, partAnimate_hook, SYM(partAnimate), b_+105);
  CALL_C(b_+105, objectApplySpeed_hook, SYM(objectApplySpeed), b_+108);
  CALL_C(b_+108, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+111);
  if (!(F & FC)) { RET_TAKEN(b_+111); return; } // ret nc
  CYC(b_+111, b_+112);
  CALL_C(b_+112, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+115);
  CYC(b_+115, b_+117); alu_sub(gb, 0x02);
  CYC(b_+117, b_+119); alu_and(gb, 0x1f);
  CYC(b_+119, b_+120); C = A;
  CYC(b_+120, b_+122); B = 0x03;

L_5a68:
  CALL_C(b_+122, getFreePartSlot_hook, SYM(getFreePartSlot), b_+125);
  if (!(F & FZ)) { CYCT(b_+125, b_+127); goto L_5a77; } // jr nz
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x4c); // PART_TWINROVA_FLAME
  CYC(b_+129, b_+130); L = alu_inc8(gb, L);
  CYC(b_+130, b_+131); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+131, b_+133); L = 0xc9; // Part.angle
  CYC(b_+133, b_+134); mem_wr(gb, HL, C);
  CALL_C(b_+134, objectCopyPosition_hook, SYM(objectCopyPosition), b_+137);

L_5a77:
  CYC(b_+137, b_+138); A = C;
  CYC(b_+138, b_+140); alu_add(gb, 0x02);
  CYC(b_+140, b_+142); alu_and(gb, 0x1f);
  CYC(b_+142, b_+143); C = A;
  CYC(b_+143, b_+144); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+144, b_+146); goto L_5a68; } // jr nz
  CYC(b_+144, b_+146);
  CALL_C(b_+146, objectCreatePuff_hook, SYM(objectCreatePuff), b_+149);
  CYC(b_+149, SYM(partCode4e)); partDelete_hook(gb); return; // jp
}
