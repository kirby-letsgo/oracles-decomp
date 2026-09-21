#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void octogonDepthCharge_subid1_hook(GB *gb);
void octogonDepthCharge_subid0_hook(GB *gb);

static uint16_t octogonDepthCharge_jump_table(GB *gb) {
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

static void octogonDepthCharge_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_OCTOGON_DEPTH_CHARGE
void partCode48_hook(GB *gb) {
  BASE(partCode48);
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xc2; // Part.subid
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+9); partDelete_hook(gb); return; } // jp nz
  CYC(b_+6, b_+9);

normalStatus:
  CYC(b_+9, b_+11); E = 0xc2; // Part.subid
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); alu_or(gb, A);
  CYC(b_+13, b_+15); E = 0xc4; // Part.state
  if (F & FZ) { CYCT(b_+15, b_+17); octogonDepthCharge_subid0_hook(gb); return; } // jr z
  CYC(b_+15, b_+17);
  TAIL(octogonDepthCharge_subid1);
}

// Small (split) projectile
void octogonDepthCharge_subid1_hook(GB *gb) {
  BASE(octogonDepthCharge_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+2, b_+4); goto state0; } // jr z
  CYC(b_+2, b_+4);

state1:
  CALL_C(b_+4, objectApplySpeed_hook, SYM(objectApplySpeed), b_+7);
  CALL_C(b_+7, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); partAnimate_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); TAIL(partDelete); // jp

state0:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+18); L = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+21, b_+23); A = 0x02;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); L = 0xd0; // Part.speed
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+29, b_+31); A = 0x01;
  CALL_C(b_+31, partSetAnimation_hook, SYM(partSetAnimation), b_+34);
  CYC(b_+34, b_+37); TAIL(objectSetVisible82); // jp
}

// Large projectile, before being split into 4 smaller ones (subid 1)
void octogonDepthCharge_subid0_hook(GB *gb) {
  BASE(octogonDepthCharge_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = octogonDepthCharge_jump_table(gb);
    if (target == b_+85) goto state1;
    if (target == b_+130) goto state2;
    if (target == b_+143) goto state3;
    goto state0;
  }

state0:
  CYC(b_+10, b_+12); A = 0x1a; // Object.visible
  CALL_C(b_+12, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = 0xc4; // Part.state
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+21); alu_rlca(gb);
  if (F & FC) { CYCT(b_+21, b_+23); goto aboveWater; } // jr c
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+24, b_+26); L = 0xc6; // Part.counter1
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+29); L = 0xcf; // Part.zh
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0xb8);
  CYC(b_+31, b_+33); L = 0xf0; // Part.var30
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x10);
  CALL_C(b_+35, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+38);
  CYC(b_+38, b_+40); alu_and(gb, 0x06);
  CYC(b_+40, b_+43); SET_HL(b_+59); // table_76b3 (positionCandidates)
  CYC(b_+43, b_+44); octogonDepthCharge_addAToHl_from_rst(gb, b_+44);
  CYC(b_+44, b_+46); E = 0xcb; // Part.yh
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); E = 0xcd; // Part.xh
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+54); A = 0x87; // SND_SPLASH
  CALL_C(b_+54, playSound_b00_hook, SYM(playSound_b00), b_+57);
  CYC(b_+57, b_+59); goto setVisible81; // jr

aboveWater:
  CYC(b_+67, b_+69); L = 0xf0; // Part.var30
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x20);
  CYC(b_+71, b_+73); L = 0xcb; // Part.yh
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+76); alu_sub(gb, 0x10);
  CYC(b_+76, b_+77); mem_wr(gb, HL, A);
  CYC(b_+77, b_+79); A = 0x85; // SND_SCENT_SEED
  CALL_C(b_+79, playSound_b00_hook, SYM(playSound_b00), b_+82);

setVisible81:
  CYC(b_+82, b_+85); TAIL(objectSetVisible81); // jp

state1:
  CYC(b_+85, b_+86); H = D;
  CYC(b_+86, b_+88); L = 0xcf; // Part.zh
  CYC(b_+88, b_+89); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+89, b_+90); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+93); alu_cp(gb, 0xd0);
  if (!(F & FC)) { CYCT(b_+93, b_+95); goto animate; } // jr nc
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); alu_cp(gb, 0xb8);
  if (!(F & FC)) { CYCT(b_+97, b_+99); goto flickerVisibility; } // jr nc
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+100); L = E;
  CYC(b_+100, b_+101); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+101, b_+103); L = 0xc6; // Part.counter1
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+105, b_+107); L = 0xe4; // Part.collisionType
  CYC(b_+107, b_+109); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+109, b_+111); L = 0xcb; // Part.yh
  CYC(b_+111, b_+113); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+117); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+117, b_+118); mem_wr(gb, HL, A);
  CYC(b_+118, b_+121); TAIL(objectSetInvisible); // jp

flickerVisibility:
  CYC(b_+121, b_+123); L = 0xda; // Part.visible
  CYC(b_+123, b_+124); A = mem_rd(gb, HL);
  CYC(b_+124, b_+126); alu_xor(gb, 0x80);
  CYC(b_+126, b_+127); mem_wr(gb, HL, A);

animate:
  CYC(b_+127, b_+130); TAIL(partAnimate); // jp

state2:
  CALL_C(b_+130, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+133);
  if (!(F & FZ)) { RET_TAKEN(b_+133); return; } // ret nz
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+135); L = E;
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+138); L = 0xe4; // Part.collisionType
  CYC(b_+138, b_+140); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+140, b_+143); TAIL(objectSetVisiblec1); // jp

state3:
  CYC(b_+143, b_+145); E = 0xf0; // Part.var30
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CALL_C(b_+146, objectUpdateSpeedZ_hook, SYM(objectUpdateSpeedZ), b_+149);
  if (!(F & FZ)) { CYCT(b_+149, b_+151); goto animate; } // jr nz
  CYC(b_+149, b_+151);
  CALL_C(b_+151, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+154);
  CYC(b_+154, b_+156); alu_and(gb, 0x04);
  CYC(b_+156, b_+157); B = A;
  CYC(b_+157, b_+159); C = 0x04;

spawnNext:
  CALL_C(b_+159, getFreePartSlot_hook, SYM(getFreePartSlot), b_+162);
  if (!(F & FZ)) { CYCT(b_+162, b_+164); goto L_772a; } // jr nz
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x48); // PART_OCTOGON_DEPTH_CHARGE
  CYC(b_+166, b_+167); L = alu_inc8(gb, L);
  CYC(b_+167, b_+168); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // subid = 1
  CYC(b_+168, b_+170); L = 0xc9; // Part.angle
  CYC(b_+170, b_+171); mem_wr(gb, HL, B);
  CALL_C(b_+171, objectCopyPosition_hook, SYM(objectCopyPosition), b_+174);
  CYC(b_+174, b_+175); A = B;
  CYC(b_+175, b_+177); alu_add(gb, 0x08);
  CYC(b_+177, b_+178); B = A;

L_772a:
  CYC(b_+178, b_+179); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+179, b_+181); goto spawnNext; } // jr nz
  CYC(b_+179, b_+181);
  CYC(b_+181, b_+183); A = 0x68; // SND_UNKNOWN3
  CALL_C(b_+183, playSound_b00_hook, SYM(playSound_b00), b_+186);
  CYC(b_+186, b_+189); TAIL(partDelete); // jp
}
