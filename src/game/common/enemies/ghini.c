#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void ghini_updateMovement_hook(GB *gb);
void ghini_updateAnimationFromAngle_hook(GB *gb);
void ghini_chooseTargetPosition_hook(GB *gb);
void ghini_updateMovementAndSetSpeedFromCounter1_hook(GB *gb);
void ghini_subid00_hook(GB *gb);
void ghini_subid01_hook(GB *gb);
void ghini_subid02_hook(GB *gb);

static uint16_t ghini_jump_table(GB *gb) {
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

static void ghini_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_GHINI
// ==================================================================================================
void enemyCode17_hook(GB *gb) {
  BASE(enemyCode17);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(b_+4, b_+6); goto stunned; } // jr c
  CYC(b_+4, b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto dead; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockbackNoSolidity_b0d_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  RET(b_+12); return; // ret

stunned:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } // ret nz
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+20, b_+22); A = 0xfe;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  RET(b_+23); return; // ret

dead:
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+28); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+28, b_+31); enemyDie_hook(gb); return; } // jp z
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+34); SET_HL((w1ReservedInteraction0_var3f + 2)); // FIRST_ENEMY_INDEX, Enemy.id

nextGhini:
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); alu_cp(gb, 0x17); // ENEMY_GHINI
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto nextGhiniStep; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+41, b_+42); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto nextGhiniStep; } // jr nz
  CYC(b_+42, b_+44);
  CALL_C(b_+44, ecom_killObjectH_b0d_hook, SYM(ecom_killObjectH_b0d), b_+47);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_ID;

nextGhiniStep:
  CYC(b_+49, b_+50); H = alu_inc8(gb, H);
  CYC(b_+50, b_+51); A = H;
  CYC(b_+51, b_+53); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+53, b_+55); goto nextGhini; } // jr c
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+58); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+58, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+61);
  if (!(F & FC)) { CYCT(b_+61, b_+63); goto normalState; } // jr nc
  CYC(b_+61, b_+63);
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = ghini_jump_table(gb);
    if (target == b_+88) goto state_uninitialized;
    if (target == b_+116) goto state_stub;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+80, b_+81); A = B;
  {
    CYC(b_+81, b_+82); push_effect(gb, b_+82);
    uint16_t target = ghini_jump_table(gb);
    if (target == SYM(ghini_subid00)) { ghini_subid00_hook(gb); return; }
    if (target == SYM(ghini_subid01)) { ghini_subid01_hook(gb); return; }
    if (target == SYM(ghini_subid02)) { ghini_subid02_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+88, b_+90); A = 0x14; // SPEED_80
  CALL_C(b_+90, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+93);
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0xfe);
  CYC(b_+97, b_+98); A = B;
  CYC(b_+98, b_+99); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+99, b_+101); goto afterSubid1Setup; } // jr nz
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+103); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x3c);
  CYC(b_+105, b_+107); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0x10);
  CYC(b_+109, b_+111); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+111, b_+113); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

afterSubid1Setup:
  CYC(b_+113, b_+116); objectSetVisiblec1_hook(gb); return; // jp

state_stub:
  RET(b_+116); return; // ret
}

// 0d:54b2, bare global; normal ghini.
void ghini_subid00_hook(GB *gb) {
  BASE(ghini_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = ghini_jump_table(gb);
    if (target == b_+8) goto state8;
    if (target == b_+30) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(b_+8, b_+11); SET_BC(0x187f);
  CALL_C(b_+11, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); A = 0x30;
  CYC(b_+19, b_+20); alu_add(gb, C);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); mem_wr(gb, HL, B);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+30); ghini_updateAnimationFromAngle_hook(gb); return; // jp

state9:
  CALL_C(b_+30, ghini_updateMovement_hook, SYM(ghini_updateMovement), b_+33);
  CALL_C(b_+33, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+36);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto animate; } // jr nz
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

animate:
  CYC(b_+41, b_+44); enemyAnimate_hook(gb); return; // jp
}

// 0d:54de, bare global; takes a second to spawn in, and killing one of subid 1 makes all
// other die too.
void ghini_subid01_hook(GB *gb) {
  BASE(ghini_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = ghini_jump_table(gb);
    if (target == b_+14) goto state8;
    if (target == b_+39) goto state9;
    if (target == b_+88) goto stateA;
    if (target == b_+105) goto stateB;
    if (target == b_+124) goto stateC;
    HANDOFF(target);
  }

state8:
  CALL_C(b_+14, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+17);
  if (F & FZ) { CYCT(b_+17, b_+19); goto makeVisible; } // jr z
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } // ret nz
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+26); ecom_flickerVisibility_b0d_hook(gb); return; // jp

makeVisible:
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+28, b_+30); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+32, b_+34); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+34, b_+37); push_effect(gb, b_+37); goto gotoStateC;
state8_resume: // resumes at 0x5503, itself a distinct "jr $5544" right after the call above
  CYC(b_+37, b_+39); goto animate; // jr

state9:
  CALL_C(b_+39, ghini_updateMovement_hook, SYM(ghini_updateMovement), b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wFrameCounter);
  CYC(b_+45, b_+46); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto animate; } // jr nc
  CYC(b_+46, b_+48);
  CALL_C(b_+48, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+51);
  if (F & FZ) { CYCT(b_+51, b_+53); goto incState; } // jr z
  CYC(b_+51, b_+53);
  CALL_C(b_+53, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+56);
  CYC(b_+56, b_+58); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(b_+58, b_+60); goto animate; } // jr nc
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+63); SET_BC(0x1f1f);
  CALL_C(b_+63, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+66);
  CYC(b_+66, b_+67); alu_or(gb, B);
  CYC(b_+67, b_+68); A = C;
  if (F & FZ) { CALL_C(b_+68, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+71); } else { CYC(b_+68, b_+71); } // call z
  CYC(b_+71, b_+73); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CALL_C(b_+74, ghini_updateAnimationFromAngle_hook, SYM(ghini_updateAnimationFromAngle), b_+77);
  CYC(b_+77, b_+79); goto animate; // jr

incState:
  CALL_C(b_+79, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+82);
  CYC(b_+82, b_+84); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x00);
  CYC(b_+86, b_+88); goto animate; // jr

stateA:
  CYC(b_+88, b_+89); H = D;
  CYC(b_+89, b_+91); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); alu_cp(gb, 0x80);
  if (F & FC) { CYCT(b_+95, b_+98); ghini_updateMovementAndSetSpeedFromCounter1_hook(gb); return; } // jp c
  CYC(b_+95, b_+98);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x80);
  CYC(b_+100, b_+101); L = E;
  CYC(b_+101, b_+102); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b

animate:
  CYC(b_+102, b_+105); enemyAnimate_hook(gb);
  if (gb->pc == b_+37 && gb->sp == sp0_) goto state8_resume;
  return; // jp

stateB:
  CALL_C(b_+105, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+108);
  if (!(F & FZ)) { CYCT(b_+108, b_+110); goto animate; } // jr nz
  CYC(b_+108, b_+110);

gotoStateC:
  CYC(b_+110, b_+112); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x0c);
  CYC(b_+114, b_+116); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x7f);
  CYC(b_+118, b_+120); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+120, b_+122); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+122, b_+124); goto animate; // jr

stateC:
  CALL_C(b_+124, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+127);
  if (!(F & FZ)) { CYCT(b_+127, b_+130); ghini_updateMovementAndSetSpeedFromCounter1_hook(gb); return; } // jp nz
  CYC(b_+127, b_+130);
  CYC(b_+130, b_+131); L = E;
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x09); // [state] = 9
  CALL_C(b_+133, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+136);
  CYC(b_+136, b_+138); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+138, b_+140); alu_and(gb, 0x7f);
  CYC(b_+140, b_+142); alu_add(gb, 0x7f);
  CYC(b_+142, b_+143); mem_wr(gb, DE, A);
  CYC(b_+143, b_+145); goto animate; // jr
}

// 0d:556f, bare global.
void ghini_subid02_hook(GB *gb) {
  BASE(ghini_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = ghini_jump_table(gb);
    if (target == b_+12) goto state8;
    if (target == b_+26) goto state9;
    if (target == b_+46) goto stateA;
    if (target == b_+85) goto stateB;
    HANDOFF(target);
  }

state8:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x24);
  CALL_C(b_+23, ghini_chooseTargetPosition_hook, SYM(ghini_chooseTargetPosition), b_+26);

state9:
  CALL_C(b_+26, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+29);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto checkSpeedBump; } // jr nz
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); L = E;
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+33, b_+35); goto stateA; // jr

checkSpeedBump:
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+38); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto stateA; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_add(gb, 0x05); // SPEED_20
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);

stateA:
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+49); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(b_+49, ecom_readPositionVars_b0d_hook, SYM(ecom_readPositionVars_b0d), b_+52);
  CYC(b_+52, b_+53); alu_sub(gb, C);
  CYC(b_+53, b_+54); A = alu_inc8(gb, A);
  CYC(b_+54, b_+56); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+56, b_+58); goto moveTowardTarget; } // jr nc
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); A = H8(hFF8F);
  CYC(b_+60, b_+61); alu_sub(gb, B);
  CYC(b_+61, b_+62); A = alu_inc8(gb, A);
  CYC(b_+62, b_+64); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto moveTowardTarget; } // jr nc
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x0b);
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x1c);
  CYC(b_+74, b_+76); goto stateB; // jr

moveTowardTarget:
  CALL_C(b_+76, ecom_moveTowardPosition_b0d_hook, SYM(ecom_moveTowardPosition_b0d), b_+79);
  CALL_C(b_+79, ghini_updateAnimationFromAngle_hook, SYM(ghini_updateAnimationFromAngle), b_+82);

animate:
  CYC(b_+82, b_+85); enemyAnimate_hook(gb); return; // jp

stateB:
  CALL_C(b_+85, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+88);
  if (F & FZ) { CYCT(b_+88, b_+90); goto gotoState8; } // jr z
  CYC(b_+88, b_+90);
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+93); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto applySpeed; } // jr nz
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+100); alu_sub(gb, 0x05); // SPEED_20
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);

applySpeed:
  CALL_C(b_+101, objectApplySpeed_hook, SYM(objectApplySpeed), b_+104);
  CYC(b_+104, b_+106); goto animate; // jr

gotoState8:
  CYC(b_+106, b_+107); L = E;
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0x08);
  CYC(b_+109, b_+12); goto state8; // jr
}

// 0d:55de, bare global; called via tail jump only (never a real call), so it never needs
// to resume the caller — it always ends by handing off to enemyAnimate.
void ghini_updateMovementAndSetSpeedFromCounter1_hook(GB *gb) {
  BASE(ghini_updateMovementAndSetSpeedFromCounter1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ghini_updateMovement_hook, SYM(ghini_updateMovement), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); B = 0x00;
  CYC(b_+8, b_+10); alu_cp(gb, 0x2a);
  if (F & FC) { CYCT(b_+10, b_+12); goto haveIndex; } // jr c
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); B = alu_inc8(gb, B);
  CYC(b_+13, b_+15); alu_cp(gb, 0x54);
  if (F & FC) { CYCT(b_+15, b_+17); goto haveIndex; } // jr c
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); B = alu_inc8(gb, B);

haveIndex:
  CYC(b_+18, b_+19); A = B;
  CYC(b_+19, b_+22); SET_HL(b_+29); // @speeds (data: SPEED_80, SPEED_40, SPEED_20)
  CYC(b_+22, b_+23); ghini_addAToHl_from_rst(gb, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30); enemyAnimate_hook(gb); return; // jr to the trampoline at 0d:55c1, which is just enemyAnimate
}

// 0d:55fe, bare global.
void ghini_updateMovement_hook(GB *gb) {
  BASE(ghini_updateMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_bounceOffScreenBoundary_b0d_hook, SYM(ecom_bounceOffScreenBoundary_b0d), b_+6);
  if (F & FZ) { RET_TAKEN(b_+6); return; } // ret z
  CYC(b_+6, b_+7);
  ghini_updateAnimationFromAngle_hook(gb); return; // fallthrough
}

// 0d:5605, bare global.
void ghini_updateAnimationFromAngle_hook(GB *gb) {
  BASE(ghini_updateAnimationFromAngle);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+4, b_+6); alu_cp(gb, 0x10);
  CYC(b_+6, b_+8); A = 0x01;
  if (F & FC) { CYCT(b_+8, b_+10); goto compare; } // jr c
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);

compare:
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+12); return; } // ret z
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+17); enemySetAnimation_hook(gb); return; // jp
}

// 0d:5616, bare global; sets var30/var31 to target position for subid 2.
void ghini_chooseTargetPosition_hook(GB *gb) {
  BASE(ghini_chooseTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC((SYM(enemy27Animations) + 2));
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+6);
  CYC(b_+6, b_+7); A = B;
  CYC(b_+7, b_+9); alu_sub(gb, 0x20);
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto haveYOffset; } // jr nc
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);

haveYOffset:
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+16); SET_HL(wRoomEdgeY);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+17, b_+19); A = alu_srl(gb, A);
  CYC(b_+19, b_+20); alu_add(gb, B);
  CYC(b_+20, b_+22); alu_sub(gb, 0x28);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); A = alu_srl(gb, A);
  CYC(b_+26, b_+27); alu_add(gb, C);
  CYC(b_+27, b_+29); alu_sub(gb, 0x38);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+33, b_+34); mem_wr(gb, HL, B);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); mem_wr(gb, HL, C);
  RET(b_+36); return; // ret
}
