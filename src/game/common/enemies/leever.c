#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode0b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode0b), (from), (to), true)

static uint16_t enemyCode0b_jump_table(GB *gb) {
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

static void enemyCode0b_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// 0d:48f3, called from enemyCode0b@state8 (subid00 & subid01).
void enemyCode0b_chooseSpawnPosition_hook(GB *gb) {
  BASE(enemyCode0b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+359, b_+360); A = B;
  CYC(b_+360, b_+361); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+361, b_+363); goto chooseRandomSpot; } // jr nz
  CYC(b_+361, b_+363);
  CYC(b_+363, b_+366); SET_DE(w1Link_yh); // w1Link.yh
  CALL_C(b_+366, getShortPositionFromDE_hook, SYM(getShortPositionFromDE), b_+369);
  CYC(b_+369, b_+370); C = A;
  CYC(b_+370, b_+372); E = 0x08; // <w1Link.direction
  CYC(b_+372, b_+373); A = mem_rd(gb, DE);
  CYC(b_+373, b_+374); alu_rlca(gb);
  CYC(b_+374, b_+375); alu_rlca(gb);
  CYC(b_+375, b_+378); SET_HL(b_+411); // @@linkRelativeOffsets
  CYC(b_+378, b_+379); enemyCode0b_addAToHl_from_rst(gb, b_+379);
  CYC(b_+379, b_+382); A = mem_rd(gb, wFrameCounter);
  CYC(b_+382, b_+384); alu_and(gb, 0x03);
  CYC(b_+384, b_+385); enemyCode0b_addAToHl_from_rst(gb, b_+385);
  CYC(b_+385, b_+387); A = H8(hActiveObject);
  CYC(b_+387, b_+388); D = A;
  CYC(b_+388, b_+389); A = C;
  CYC(b_+389, b_+390); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+390, b_+391); C = A;
  CYC(b_+391, b_+393); alu_and(gb, 0xf0);
  CYC(b_+393, b_+395); alu_cp(gb, 0x80); // SMALL_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(b_+395, b_+397); goto invalid; } // jr nc
  CYC(b_+395, b_+397);
  CYC(b_+397, b_+398); A = C;
  CYC(b_+398, b_+400); alu_and(gb, 0x0f);
  CYC(b_+400, b_+402); alu_cp(gb, 0x0a); // SMALL_ROOM_WIDTH
  if (!(F & FC)) { CYCT(b_+402, b_+404); goto invalid; } // jr nc
  CYC(b_+402, b_+404);
  CYC(b_+404, b_+406); B = 0xce; // >wRoomCollisions
  CYC(b_+406, b_+407); A = mem_rd(gb, BC);
  CYC(b_+407, b_+408); alu_or(gb, A);
  RET(b_+408); return; // ret

invalid:
  CYC(b_+409, b_+410); alu_or(gb, D);
  RET(b_+410); return; // ret

chooseRandomSpot:
  CALL_C(b_+427, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+430);
  CYC(b_+430, b_+432); alu_and(gb, 0x77);
  CYC(b_+432, b_+433); C = A;
  CYC(b_+433, b_+435); B = 0xce; // >wRoomCollisions
  CYC(b_+435, b_+436); A = mem_rd(gb, BC);
  CYC(b_+436, b_+437); alu_or(gb, A);
  RET(b_+437); return; // ret
}

// 0d:4942, called from enemyCode0b's uninitialized/subid02 states.
void enemyCode0b_setRandomCounter1_hook(GB *gb) {
  BASE(enemyCode0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+438, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+441);
  CYC(b_+441, b_+443); alu_and(gb, 0x03);
  CYC(b_+443, b_+446); SET_HL(b_+452); // @counter1Vals
  CYC(b_+446, b_+447); enemyCode0b_addAToHl_from_rst(gb, b_+447);
  CYC(b_+447, b_+449); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+449, b_+450); A = mem_rd(gb, HL);
  CYC(b_+450, b_+451); mem_wr(gb, DE, A);
  RET(b_+451); return; // ret
}

// 0d:4954, called after emerging fully in the chase states.
void enemyCode0b_setRandomHighCounter1_hook(GB *gb) {
  BASE(enemyCode0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+456, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+459);
  CYC(b_+459, b_+461); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+461, b_+463); alu_and(gb, 0x38);
  CYC(b_+463, b_+465); alu_add(gb, 0x70);
  CYC(b_+465, b_+466); mem_wr(gb, DE, A);
  RET(b_+466); return; // ret
}

// 0d:495f, called from subid02's chase state.
void enemyCode0b_nudgeTowardsLink_hook(GB *gb) {
  BASE(enemyCode0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+467, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+470);
  if (!(F & FZ)) { RET_TAKEN(b_+470); return; } // ret nz
  CYC(b_+470, b_+471);
  CYC(b_+471, b_+473); mem_wr(gb, HL, 0x06);
  CALL_C(b_+473, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+476);
  CYC(b_+476, SYM(enemyCode0c)); objectNudgeAngleTowards_hook(gb); return; // jp
}

// ==================================================================================================
// ENEMY_LEEVER
// ==================================================================================================
void enemyCode0b_hook(GB *gb) {
  BASE(enemyCode0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto dead; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+11, b_+14);
  RET(b_+14); return; // ret

dead:
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto die; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); B = 0x0b; // ENEMY_LEEVER
  CALL_C(b_+24, ecom_spawnEnemyWithSubid01_b0d_hook, SYM(ecom_spawnEnemyWithSubid01_b0d), b_+27);
  if (!(F & FZ)) { RET_TAKEN(b_+27); return; } // ret nz
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+29, b_+31); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);

die:
  CYC(b_+39, b_+42); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(b_+42, ecom_getSubidAndCpStateTo08_b0d_hook, SYM(ecom_getSubidAndCpStateTo08_b0d), b_+45);
  if (!(F & FC)) { CYCT(b_+45, b_+47); goto normalState; } // jr nc
  CYC(b_+45, b_+47);
  {
    CYC(b_+47, b_+48); push_effect(gb, b_+48);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == b_+72) goto state_uninitialized;
    if (target == b_+104) goto state_stub;
    if (target == b_+78) goto state_switchHook;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+64, b_+65); A = B;
  {
    CYC(b_+65, b_+66); push_effect(gb, b_+66);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == b_+105) goto normalState_subid00;
    if (target == b_+203) goto normalState_subid01;
    if (target == b_+235) goto normalState_subid02;
    HANDOFF(target);
  }

state_uninitialized:
  CALL_C(b_+72, enemyCode0b_setRandomCounter1_hook, b_+438, b_+75);
  CYC(b_+75, b_+78); ecom_setSpeedAndState8_b0d_hook(gb); return; // jp

state_switchHook:
  CYC(b_+78, b_+79); E = alu_inc8(gb, E);
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  {
    CYC(b_+80, b_+81); push_effect(gb, b_+81);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+89) goto switchHook_substate1;
    if (target == b_+90) goto switchHook_substate3;
    HANDOFF(target);
  }

switchHook_substate1:
  RET(b_+89); return; // ret

switchHook_substate3:
  CYC(b_+90, b_+92); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+96); SET_HL(b_+101); // @@destStates
  CYC(b_+96, b_+97); enemyCode0b_addAToHl_from_rst(gb, b_+97);
  CYC(b_+97, b_+98); B = mem_rd(gb, HL);
  CYC(b_+98, b_+101); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp

state_stub:
  RET(b_+104); return; // ret

normalState_subid00:
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+108); alu_sub(gb, 0x08);
  {
    CYC(b_+108, b_+109); push_effect(gb, b_+109);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == b_+117) goto state8;
    if (target == b_+139) goto state9;
    if (target == b_+165) goto subid00_stateA;
    if (target == b_+187) goto stateB;
    HANDOFF(target);
  }

state8:
  CALL_C(b_+117, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+120);
  if (!(F & FZ)) { RET_TAKEN(b_+120); return; } // ret nz
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+122, enemyCode0b_chooseSpawnPosition_hook, b_+359, b_+125);
  if (!(F & FZ)) { RET_TAKEN(b_+125); return; } // ret nz
  CYC(b_+125, b_+126);
  CALL_C(b_+126, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+129);
  CYC(b_+129, b_+131); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+131, b_+132); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+132, b_+133); alu_xor(gb, A);
  CALL_C(b_+133, enemySetAnimation_hook, SYM(enemySetAnimation), b_+136);
  CYC(b_+136, b_+139); objectSetVisiblec2_hook(gb); return; // jp

state9:
  CYC(b_+139, b_+140); H = D;
  CYC(b_+140, b_+142); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+142, b_+143); A = mem_rd(gb, HL);
  CYC(b_+143, b_+144); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+144, b_+146); goto animate; } // jr nz
  CYC(b_+144, b_+146);
  CYC(b_+146, b_+147); L = E;
  CYC(b_+147, b_+148); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+148, b_+150); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+150, b_+152); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x14); // SPEED_80
  CALL_C(b_+156, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+159);
  CALL_C(b_+159, enemyCode0b_setRandomHighCounter1_hook, b_+456, b_+162);

animate:
  CYC(b_+162, b_+165); enemyAnimate_hook(gb); return; // jp

subid00_stateA:
  CALL_C(b_+165, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+168);
  if (!(F & FZ)) { CYCT(b_+168, b_+171); goto updatePosition; } // jp nz
  CYC(b_+168, b_+171);

backIntoGround:
  CALL_C(b_+171, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+174);
  CYC(b_+174, b_+176); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+176, b_+178); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+178, b_+180); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+182, b_+184); A = 0x02;
  CYC(b_+184, b_+187); enemySetAnimation_hook(gb); return; // jp

stateB:
  CYC(b_+187, b_+188); H = D;
  CYC(b_+188, b_+190); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+190, b_+191); A = mem_rd(gb, HL);
  CYC(b_+191, b_+192); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+192, b_+194); goto animate; } // jr nz
  CYC(b_+192, b_+194);
  CYC(b_+194, b_+195); L = E;
  CYC(b_+195, b_+197); mem_wr(gb, HL, 0x08);
  CALL_C(b_+197, enemyCode0b_setRandomCounter1_hook, b_+438, b_+200);
  CYC(b_+200, b_+203); objectSetInvisible_hook(gb); return; // jp

normalState_subid01:
  CYC(b_+203, b_+204); A = mem_rd(gb, DE);
  CYC(b_+204, b_+206); alu_sub(gb, 0x08);
  {
    CYC(b_+206, b_+207); push_effect(gb, b_+207);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == b_+117) goto state8;
    if (target == b_+139) goto state9;
    if (target == b_+215) goto subid01_stateA;
    if (target == b_+187) goto stateB;
    HANDOFF(target);
  }

subid01_stateA:
  CALL_C(b_+215, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+218);
  if (F & FZ) { CYCT(b_+218, b_+221); goto backIntoGround; } // jp z
  CYC(b_+218, b_+221);
  CALL_C(b_+221, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+224);
  CYC(b_+224, b_+226); alu_cp(gb, 0x14);
  if (!(F & FC)) { CYCT(b_+226, b_+229); goto updatePosition; } // jp nc
  CYC(b_+226, b_+229);
  CALL_C(b_+229, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+232);
  CYC(b_+232, b_+235); goto updatePosition; // jp

normalState_subid02:
  CYC(b_+235, b_+236); A = mem_rd(gb, DE);
  CYC(b_+236, b_+238); alu_sub(gb, 0x08);
  {
    CYC(b_+238, b_+239); push_effect(gb, b_+239);
    uint16_t target = enemyCode0b_jump_table(gb);
    if (target == b_+249) goto subid02_state8;
    if (target == b_+271) goto subid02_state9;
    if (target == b_+287) goto subid02_stateA;
    if (target == b_+313) goto subid02_stateB;
    if (target == b_+328) goto subid02_stateC;
    HANDOFF(target);
  }

subid02_state8:
  CYC(b_+249, b_+250); H = D;
  CYC(b_+250, b_+251); L = E;
  CYC(b_+251, b_+252); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+252, b_+254); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+254, b_+255); A = mem_rd(gb, HL);
  CYC(b_+255, b_+257); alu_and(gb, 0x30);
  CYC(b_+257, b_+259); alu_add(gb, 0x60);
  CYC(b_+259, b_+260); mem_wr(gb, HL, A);
  CYC(b_+260, b_+262); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+262, b_+264); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+264, b_+265); A = mem_rd(gb, DE);
  CYC(b_+265, b_+266); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+266, b_+268); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+268, b_+269); A = mem_rd(gb, DE);
  CYC(b_+269, b_+270); mem_wr(gb, HL, A);
  RET(b_+270); return; // ret

subid02_state9:
  CALL_C(b_+271, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+274);
  if (!(F & FZ)) { RET_TAKEN(b_+274); return; } // ret nz
  CYC(b_+274, b_+275);
  CYC(b_+275, b_+276); L = alu_inc8(gb, L);
  CYC(b_+276, b_+278); mem_wr(gb, HL, 0x06); // [counter2] = 6
  CYC(b_+278, b_+279); L = E;
  CYC(b_+279, b_+280); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+280, b_+281); alu_xor(gb, A);
  CALL_C(b_+281, enemySetAnimation_hook, SYM(enemySetAnimation), b_+284);
  CYC(b_+284, b_+287); objectSetVisiblec2_hook(gb); return; // jp

subid02_stateA:
  CYC(b_+287, b_+289); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+289, b_+290); A = mem_rd(gb, DE);
  CYC(b_+290, b_+291); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+291, b_+293); goto animate2; } // jr nz
  CYC(b_+291, b_+293);
  CYC(b_+293, b_+294); H = D;
  CYC(b_+294, b_+296); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+296, b_+297); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+297, b_+299); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+299, b_+301); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+301, b_+303); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+303, b_+305); mem_wr(gb, HL, 0x19); // SPEED_a0
  CALL_C(b_+305, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+308);
  CALL_C(b_+308, enemyCode0b_setRandomHighCounter1_hook, b_+456, b_+311);
  CYC(b_+311, b_+313); goto animate2; // jr

subid02_stateB:
  CALL_C(b_+313, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+316);
  if (F & FZ) { CYCT(b_+316, b_+319); goto backIntoGround; } // jp z
  CYC(b_+316, b_+319);
  CALL_C(b_+319, enemyCode0b_nudgeTowardsLink_hook, b_+467, b_+322);
  CALL_C(b_+322, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+325);

animate2:
  CYC(b_+325, b_+328); enemyAnimate_hook(gb); return; // jp

subid02_stateC:
  CYC(b_+328, b_+330); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+330, b_+331); A = mem_rd(gb, DE);
  CYC(b_+331, b_+332); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+332, b_+334); goto animate2; } // jr nz
  CYC(b_+332, b_+334);
  CYC(b_+334, b_+336); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+336, b_+338); A = 0x09;
  CYC(b_+338, b_+339); mem_wr(gb, DE, A);
  CALL_C(b_+339, enemyCode0b_setRandomCounter1_hook, b_+438, b_+342);
  CYC(b_+342, b_+345); objectSetInvisible_hook(gb); return; // jp

updatePosition:
  CYC(b_+345, b_+347); A = 0x01;
  CALL_C(b_+347, ecom_getTopDownAdjacentWallsBitset_b0d_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+350);
  if (!(F & FZ)) { CYCT(b_+350, b_+353); goto backIntoGround; } // jp nz
  CYC(b_+350, b_+353);
  CALL_C(b_+353, objectApplySpeed_hook, SYM(objectApplySpeed), b_+356);
  CYC(b_+356, b_+359); enemyAnimate_hook(gb); return; // jp
}
