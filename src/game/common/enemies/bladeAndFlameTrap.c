#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void bladeTrap_subid00_hook(GB *gb);
void bladeTrap_subid01_hook(GB *gb);
void bladeTrap_subid03_hook(GB *gb);
void bladeTrap_subid05_hook(GB *gb);
void bladeTrap_updateAngle_hook(GB *gb);
void bladeTrap_initCircular_hook(GB *gb);
void bladeTrap_checkObstructionsToTarget_hook(GB *gb);
void bladeTrap_checkLinkAligned_hook(GB *gb);
void bladeTrap_incAngle_hook(GB *gb);
void bladeTrap_decAngle_hook(GB *gb);

static uint16_t bladeTrap_jump_table(GB *gb) {
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

static void bladeTrap_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_BLADE_TRAP
// ENEMY_FLAME_TRAP
// ==================================================================================================
void enemyCode0e_hook(GB *gb) {
  BASE(enemyCode0e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+1); return; } // ret z
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CALL_C(b_+4, enemyAnimate_hook, SYM(enemyAnimate), b_+7);
  CALL_C(b_+7, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+10);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto normalState; } // jr nc
  CYC(b_+10, b_+12);
  {
    CYC(b_+12, b_+13); push_effect(gb, b_+13);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == b_+43) goto state_uninitialized;
    if (target == b_+72) goto state_stub;
    HANDOFF(target);
  }

normalState:
  CYC(b_+29, b_+30); A = B;
  {
    CYC(b_+30, b_+31); push_effect(gb, b_+31);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == SYM(bladeTrap_subid00)) { bladeTrap_subid00_hook(gb); return; }
    if (target == SYM(bladeTrap_subid01)) { bladeTrap_subid01_hook(gb); return; }
    if (target == SYM(bladeTrap_subid03)) { bladeTrap_subid03_hook(gb); return; }
    if (target == SYM(bladeTrap_subid05)) { bladeTrap_subid05_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+43, b_+44); A = B;
  CYC(b_+44, b_+46); alu_sub(gb, 0x03);
  CYC(b_+46, b_+48); alu_cp(gb, 0x02);
  if (F & FC) { CALL_C_CC(b_+48, bladeTrap_initCircular_hook, SYM(bladeTrap_initCircular), b_+51); } else { CYC(b_+48, b_+51); } // call c
  CYC(b_+51, b_+53); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); alu_or(gb, A);
  CYC(b_+55, b_+57); A = 0x08;
  if (!(F & FZ)) { CYCT(b_+57, b_+59); goto setVar3e; } // jr nz
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); A = 0x01;
  CALL_C(b_+61, enemySetAnimation_hook, SYM(enemySetAnimation), b_+64);
  CYC(b_+64, b_+66); A = 0x01;

setVar3e:
  CYC(b_+66, b_+68); E = ENEMY_BASE + 0x3e; // Enemy.var3e
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+72); TAIL(ecom_setSpeedAndState8AndVisible_b0d); // jp

state_stub:
  RET(b_+72); return; // ret
}

// 0d:4be7, bare global; red, spinning trap.
void bladeTrap_subid00_hook(GB *gb) {
  BASE(bladeTrap_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == b_+12) goto state8;
    if (target == b_+24) goto state9;
    if (target == b_+53) goto stateA;
    if (target == b_+72) goto stateB;
    HANDOFF(target);
  }

state8:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+24); TAIL(enemySetAnimation); // jp

state9:
  CYC(b_+24, b_+26); B = 0x0e;
  CALL_C(b_+26, bladeTrap_checkLinkAligned_hook, SYM(bladeTrap_checkLinkAligned), b_+29);
  if (!(F & FC)) { RET_TAKEN(b_+29); return; } // ret nc
  CYC(b_+29, b_+30);
  CALL_C(b_+30, bladeTrap_checkObstructionsToTarget_hook, SYM(bladeTrap_checkObstructionsToTarget), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x0a);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x18);
  CYC(b_+43, b_+45); A = 0x71; // SND_MOVEBLOCK
  CALL_C(b_+45, playSound_b00_hook, SYM(playSound_b00), b_+48);
  CYC(b_+48, b_+50); A = 0x02;
  CYC(b_+50, b_+53); TAIL(enemySetAnimation); // jp

stateA:
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); alu_rrca(gb);
  if (F & FC) { CALL_C_CC(b_+57, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+60); } else { CYC(b_+57, b_+60); } // call c
  CALL_C(b_+60, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+63);
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto animate; } // jr nz
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x0b);

animate:
  CYC(b_+69, b_+72); TAIL(enemyAnimate); // jp

stateB:
  CALL_C(b_+72, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+75);
  if (!(F & FZ)) { CYCT(b_+75, b_+77); goto animate; } // jr nz
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+79); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+79, b_+81); A = 0x09;
  CYC(b_+81, b_+82); mem_wr(gb, DE, A);
  CYC(b_+82, b_+84); A = 0x01;
  CYC(b_+84, b_+87); TAIL(enemySetAnimation); // jp
}

// 0d:4c3e, bare global; bladeTrap_subid02 aliases the same address (blue/gold blade traps).
void bladeTrap_subid01_hook(GB *gb) {
  BASE(bladeTrap_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == b_+14) goto state8;
    if (target == b_+30) goto state9;
    if (target == b_+60) goto stateA;
    if (target == b_+105) goto stateB;
    if (target == b_+117) goto stateC;
    HANDOFF(target);
  }

state8:
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); A = alu_dec8(gb, A);
  CYC(b_+21, b_+23); A = 0x3c; // SPEED_180
  if (F & FZ) { CYCT(b_+23, b_+25); goto setVar30; } // jr z
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); A = 0x78; // SPEED_300

setVar30:
  CYC(b_+27, b_+29); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);

state9:
  CYC(b_+30, b_+32); B = 0x0d;
  CALL_C(b_+32, bladeTrap_checkLinkAligned_hook, SYM(bladeTrap_checkLinkAligned), b_+35);
  if (!(F & FC)) { RET_TAKEN(b_+35); return; } // ret nc
  CYC(b_+35, b_+36);
  CALL_C(b_+36, bladeTrap_checkObstructionsToTarget_hook, SYM(bladeTrap_checkObstructionsToTarget), b_+39);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; } // ret nz
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); A = 0x01;
  CALL_C(b_+42, ecom_getTopDownAdjacentWallsBitset_b0d_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+45);
  if (!(F & FZ)) { RET_TAKEN(b_+45); return; } // ret nz
  CYC(b_+45, b_+46);
  CALL_C(b_+46, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+49);
  CYC(b_+49, b_+51); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A);
  CYC(b_+55, b_+57); A = 0x75; // SND_UNKNOWN5
  CYC(b_+57, b_+60); TAIL(playSound_b00); // jp

stateA:
  CALL_C(b_+60, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+63);
  CYC(b_+63, b_+64); H = D;
  if (F & FZ) { CYCT(b_+64, b_+66); goto beginRetracting; } // jr z
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+68, b_+70); alu_bit(gb, 3, mem_rd(gb, HL));
  CYC(b_+70, b_+72); B = 0x58; // (LARGE_ROOM_HEIGHT/2 << 4) + 8
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_YH;
  if (F & FZ) { CYCT(b_+74, b_+76); goto checkOffset; } // jr z
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); B = 0x78; // (LARGE_ROOM_WIDTH/2 << 4) + 8
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_XH;

checkOffset:
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_sub(gb, B);
  CYC(b_+82, b_+84); alu_add(gb, 0x07);
  CYC(b_+84, b_+86); alu_cp(gb, 0x0f);
  if (!(F & FC)) { RET_TAKEN(b_+86); return; } // ret nc
  CYC(b_+86, b_+87);

beginRetracting:
  CYC(b_+87, b_+89); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL);
  CYC(b_+90, b_+92); alu_xor(gb, 0x10);
  CYC(b_+92, b_+93); mem_wr(gb, HL, A);
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+99, b_+100); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+100, b_+102); A = 0x50; // SND_CLINK
  CYC(b_+102, b_+105); TAIL(playSound_b00); // jp

stateB:
  CALL_C(b_+105, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+108);
  if (!(F & FZ)) { RET_TAKEN(b_+108); return; } // ret nz
  CYC(b_+108, b_+109);
  CALL_C(b_+109, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+112);
  CYC(b_+112, b_+114); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+114, b_+116); mem_wr(gb, HL, 0x10);
  RET(b_+116); return; // ret

stateC:
  CALL_C(b_+117, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+120);
  if (!(F & FZ)) { RET_TAKEN(b_+120); return; } // ret nz
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x09);
  RET(b_+125); return; // ret
}

// 0d:4cbc, bare global; bladeTrap_subid04 aliases the same address (circular blade traps).
void bladeTrap_subid03_hook(GB *gb) {
  BASE(bladeTrap_subid03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == b_+6) goto state8;
    HANDOFF(target);
  }

state8:
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x01);
  if (F & FZ) { CALL_C_CC(b_+11, bladeTrap_updateAngle_hook, SYM(bladeTrap_updateAngle), b_+14); } else { CYC(b_+11, b_+14); } // call z
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+24); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+27); TAIL(objectSetPositionInCircleArc); // jp
}

// 0d:4cd7, bare global; unlimited range green blade.
void bladeTrap_subid05_hook(GB *gb) {
  BASE(bladeTrap_subid05);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = bladeTrap_jump_table(gb);
    if (target == b_+14) goto state8;
    if (target == b_+21) goto state9;
    if (target == b_+52) goto stateA;
    if (target == b_+74) goto stateB;
    if (target == b_+86) goto stateC;
    HANDOFF(target);
  }

state8:
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+16); L = E;
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x50); // SPEED_200

state9:
  CYC(b_+21, b_+23); B = 0x0e;
  CALL_C(b_+23, bladeTrap_checkLinkAligned_hook, SYM(bladeTrap_checkLinkAligned), b_+26);
  if (!(F & FC)) { RET_TAKEN(b_+26); return; } // ret nc
  CYC(b_+26, b_+27);
  CALL_C(b_+27, bladeTrap_checkObstructionsToTarget_hook, SYM(bladeTrap_checkObstructionsToTarget), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); A = 0x01;
  CALL_C(b_+33, ecom_getTopDownAdjacentWallsBitset_b0d_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; } // ret nz
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+40); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+47, b_+49); A = 0x75; // SND_UNKNOWN5
  CYC(b_+49, b_+52); TAIL(playSound_b00); // jp

stateA:
  CALL_C(b_+52, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+55);
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; } // ret nz
  CYC(b_+55, b_+56);
  CALL_C(b_+56, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+59);
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CYC(b_+62, b_+64); alu_xor(gb, 0x10);
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+69, b_+71); A = 0x50; // SND_CLINK
  CYC(b_+71, b_+74); TAIL(playSound_b00); // jp

stateB:
  CALL_C(b_+74, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+77);
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; } // ret nz
  CYC(b_+77, b_+78);
  CALL_C(b_+78, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+81);
  CYC(b_+81, b_+83); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x10);
  RET(b_+85); return; // ret

stateC:
  CALL_C(b_+86, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+89);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; } // ret nz
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x09);
  RET(b_+94); return; // ret
}

// 0d:4d36, bare global.
void bladeTrap_updateAngle_hook(GB *gb) {
  BASE(bladeTrap_updateAngle);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  if (!(F & FZ)) { CYCT(b_+7, b_+10); bladeTrap_decAngle_hook(gb); return; } // jp nz
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+13); TAIL(bladeTrap_incAngle); // jp
}

// 0d:4d43, bare global.
void bladeTrap_initCircular_hook(GB *gb) {
  BASE(bladeTrap_initCircular);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+14); alu_and(gb, 0xf0);
  CYC(b_+14, b_+16); alu_add(gb, 0x08);
  CYC(b_+16, b_+18); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+21); A = C;
  CYC(b_+21, b_+23); alu_and(gb, 0x0f);
  CYC(b_+23, b_+25); A = alu_swap(gb, A);
  CYC(b_+25, b_+27); alu_add(gb, 0x08);
  CYC(b_+27, b_+29); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); C = A;
  CYC(b_+31, b_+33); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+36); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+39, b_+42); TAIL(objectSetPositionInCircleArc); // jp
}

// 0d:4dbf, called once from bladeTrap_checkObstructionsToTarget.
static void bladeTrap_checkObstructionsToTarget_getNumTilesToTarget(GB *gb) {
  BASE(bladeTrap_checkObstructionsToTarget);
  CYC(b_+74, b_+75); E = B;
  CYC(b_+75, b_+77); A = H8(hEnemyTargetY);
  CYC(b_+77, b_+79); alu_bit(gb, 3, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+79, b_+81); goto sub; } // jr z
  CYC(b_+79, b_+81);
  CYC(b_+81, b_+82); E = C;
  CYC(b_+82, b_+84); A = H8(hEnemyTargetX);

sub:
  CYC(b_+84, b_+85); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(b_+85, b_+87); goto afterAbs; } // jr nc
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+88); alu_cpl(gb);
  CYC(b_+88, b_+89); A = alu_inc8(gb, A);

afterAbs:
  CYC(b_+89, b_+91); A = alu_swap(gb, A);
  CYC(b_+91, b_+93); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto storeResult; } // jr nz
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+96); A = alu_inc8(gb, A);

storeResult:
  CYC(b_+96, b_+98); mem_wr(gb, hFF8B, A);
  RET(b_+98); return; // ret
}

// 0d:4dad, called once from bladeTrap_checkObstructionsToTarget's tile-check loop.
static void bladeTrap_checkObstructionsToTarget_checkNextTileSolid(GB *gb) {
  BASE(bladeTrap_checkObstructionsToTarget);
  CYC(b_+56, b_+57); A = B;
  CYC(b_+57, b_+58); alu_add(gb, H);
  CYC(b_+58, b_+59); B = A;
  CYC(b_+59, b_+61); alu_and(gb, 0xf0);
  CYC(b_+61, b_+62); E = A;
  CYC(b_+62, b_+63); A = C;
  CYC(b_+63, b_+64); alu_add(gb, L);
  CYC(b_+64, b_+65); C = A;
  CYC(b_+65, b_+67); alu_and(gb, 0xf0);
  CYC(b_+67, b_+69); A = alu_swap(gb, A);
  CYC(b_+69, b_+70); alu_or(gb, E);
  CYC(b_+70, b_+71); E = A;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+73); alu_or(gb, A);
  RET(b_+73); return; // ret
}

// 0d:4d75, bare global.
void bladeTrap_checkObstructionsToTarget_hook(GB *gb) {
  BASE(bladeTrap_checkObstructionsToTarget);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  CYC(b_+7, b_+9); A = H8(hEnemyTargetX);
  CYC(b_+9, b_+10); alu_sub(gb, C);
  CYC(b_+10, b_+12); alu_add(gb, 0x04);
  CYC(b_+12, b_+14); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto getDirectionOffset; } // jr nc
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); A = H8(hEnemyTargetY);
  CYC(b_+18, b_+19); alu_sub(gb, B);
  CYC(b_+19, b_+21); alu_add(gb, 0x04);
  CYC(b_+21, b_+23); alu_cp(gb, 0x09);
  if (F & FC) { RET_TAKEN(b_+23); return; } // ret c
  CYC(b_+23, b_+24);

getDirectionOffset:
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_ANGLE;
  CALL_C(b_+26, bladeTrap_checkObstructionsToTarget_getNumTilesToTarget, b_+74, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); alu_rrca(gb);
  CYC(b_+31, b_+32); alu_rrca(gb);
  CYC(b_+32, b_+35); SET_HL(SYM(bladeTrap_directionOffsets)); // bladeTrap_directionOffsets (data)
  CYC(b_+35, b_+36); bladeTrap_addAToHl_from_rst(gb, b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+37, b_+38); L = mem_rd(gb, HL);
  CYC(b_+38, b_+39); H = A;
  PUSH(b_+39, DE);
  CYC(b_+40, b_+42); D = 0xce; // >wRoomCollisions

checkNextTile:
  CALL_C(b_+42, bladeTrap_checkObstructionsToTarget_checkNextTileSolid, b_+56, b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto doneChecking; } // jr nz
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); A = mem_rd(gb, hFF8B);
  CYC(b_+49, b_+50); A = alu_dec8(gb, A);
  CYC(b_+50, b_+52); mem_wr(gb, hFF8B, A);
  if (!(F & FZ)) { CYCT(b_+52, b_+54); goto checkNextTile; } // jr nz
  CYC(b_+52, b_+54);

doneChecking:
  SET_DE(POP(b_+54));
  RET(b_+55); return; // ret
}

// 0d:4dd8, bare global.
void bladeTrap_checkLinkAligned_hook(GB *gb) {
  BASE(bladeTrap_checkLinkAligned);
  CYC(b_+0, b_+1); C = B;
  CYC(b_+1, b_+3); C = alu_sla(gb, C);
  CYC(b_+3, b_+4); C = alu_inc8(gb, C);
  CYC(b_+4, b_+6); E = 0x00;
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+11); A = H8(hEnemyTargetX);
  CYC(b_+11, b_+12); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+14); alu_cp(gb, C);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+16, b_+18); A = H8(hEnemyTargetY);
  if (F & FC) { CYCT(b_+18, b_+20); goto inRange; } // jr c
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); E = 0x18;
  CYC(b_+22, b_+23); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+23, b_+24); alu_add(gb, B);
  CYC(b_+24, b_+25); alu_cp(gb, C);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+27, b_+29); A = H8(hEnemyTargetX);
  if (!(F & FC)) { RET_TAKEN(b_+29); return; } // ret nc
  CYC(b_+29, b_+30);

inRange:
  CYC(b_+30, b_+31); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); A = E;
  if (F & FC) { CYCT(b_+32, b_+34); goto setAngle; } // jr c
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); alu_xor(gb, 0x10);

setAngle:
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+40); alu_scf(gb);
  RET(b_+40); return; // ret
}

// 0d:4e01, bare global.
void bladeTrap_incAngle_hook(GB *gb) {
  BASE(bladeTrap_decAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(SYM(bladeTrap_incAngle), (SYM(bladeTrap_incAngle) + 1)); A = mem_rd(gb, DE);
  CYC((SYM(bladeTrap_incAngle) + 1), (SYM(bladeTrap_incAngle) + 2)); A = alu_inc8(gb, A);
  CYC((SYM(bladeTrap_incAngle) + 2), b_+2); goto andAndStore; // jr

andAndStore:
  CYC(b_+2, b_+4); alu_and(gb, 0x1f);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  RET(b_+5); return; // ret
}

// 0d:4e05, bare global.
void bladeTrap_decAngle_hook(GB *gb) {
  BASE(bladeTrap_decAngle);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); A = alu_dec8(gb, A);
  CYC(b_+2, b_+4); alu_and(gb, 0x1f);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  RET(b_+5); return; // ret
}
