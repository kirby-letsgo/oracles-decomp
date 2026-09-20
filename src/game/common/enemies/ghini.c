#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x543d, 0x543f); goto normalStatus; } // jr z
  CYC(0x543d, 0x543f);
  CYC(0x543f, 0x5441); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x5441, 0x5443); goto stunned; } // jr c
  CYC(0x5441, 0x5443);
  if (F & FZ) { CYCT(0x5443, 0x5445); goto dead; } // jr z
  CYC(0x5443, 0x5445);
  CYC(0x5445, 0x5446); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5446, 0x5449); ecom_updateKnockbackNoSolidity_b0d_hook(gb); return; } // jp nz
  CYC(0x5446, 0x5449);
  RET(0x5449); return; // ret

stunned:
  CYC(0x544a, 0x544c); E = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x544c, 0x544d); A = mem_rd(gb, DE);
  CYC(0x544d, 0x544e); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x544e); return; } // ret nz
  CYC(0x544e, 0x544f);
  CYC(0x544f, 0x5451); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x5451, 0x5453); A = 0xfe;
  CYC(0x5453, 0x5454); mem_wr(gb, DE, A);
  RET(0x5454); return; // ret

dead:
  CYC(0x5455, 0x5457); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5457, 0x5458); A = mem_rd(gb, DE);
  CYC(0x5458, 0x5459); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x5459, 0x545c); enemyDie_hook(gb); return; } // jp z
  CYC(0x5459, 0x545c);
  CYC(0x545c, 0x545f); SET_HL(0xd081); // FIRST_ENEMY_INDEX, Enemy.id

nextGhini:
  CYC(0x545f, 0x5460); A = mem_rd(gb, HL);
  CYC(0x5460, 0x5462); alu_cp(gb, 0x17); // ENEMY_GHINI
  if (!(F & FZ)) { CYCT(0x5462, 0x5464); goto nextGhiniStep; } // jr nz
  CYC(0x5462, 0x5464);
  CYC(0x5464, 0x5465); L = alu_inc8(gb, L);
  CYC(0x5465, 0x5466); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5466, 0x5467); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5467, 0x5469); goto nextGhiniStep; } // jr nz
  CYC(0x5467, 0x5469);
  CALL_C(0x5469, ecom_killObjectH_b0d_hook, 0x445e, 0x546c);
  CYC(0x546c, 0x546e); L = ENEMY_BASE + OBJ_ID;

nextGhiniStep:
  CYC(0x546e, 0x546f); H = alu_inc8(gb, H);
  CYC(0x546f, 0x5470); A = H;
  CYC(0x5470, 0x5472); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x5472, 0x5474); goto nextGhini; } // jr c
  CYC(0x5472, 0x5474);
  CYC(0x5474, 0x5477); enemyDie_hook(gb); return; // jp

normalStatus:
  CALL_C(0x5477, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x547a);
  if (!(F & FC)) { CYCT(0x547a, 0x547c); goto normalState; } // jr nc
  CYC(0x547a, 0x547c);
  {
    CYC(0x547c, 0x547d); push_effect(gb, 0x547d);
    uint16_t target = ghini_jump_table(gb);
    if (target == 0x5495) goto state_uninitialized;
    if (target == 0x54b1) goto state_stub;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x548d, 0x548e); A = B;
  {
    CYC(0x548e, 0x548f); push_effect(gb, 0x548f);
    uint16_t target = ghini_jump_table(gb);
    if (target == 0x54b2) { ghini_subid00_hook(gb); return; }
    if (target == 0x54de) { ghini_subid01_hook(gb); return; }
    if (target == 0x556f) { ghini_subid02_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x5495, 0x5497); A = 0x14; // SPEED_80
  CALL_C(0x5497, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x549a);
  CYC(0x549a, 0x549c); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x549c, 0x549e); mem_wr(gb, HL, 0xfe);
  CYC(0x549e, 0x549f); A = B;
  CYC(0x549f, 0x54a0); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x54a0, 0x54a2); goto afterSubid1Setup; } // jr nz
  CYC(0x54a0, 0x54a2);
  CYC(0x54a2, 0x54a4); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x54a4, 0x54a6); mem_wr(gb, HL, 0x3c);
  CYC(0x54a6, 0x54a8); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x54a8, 0x54aa); mem_wr(gb, HL, 0x10);
  CYC(0x54aa, 0x54ac); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x54ac, 0x54ae); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)

afterSubid1Setup:
  CYC(0x54ae, 0x54b1); objectSetVisiblec1_hook(gb); return; // jp

state_stub:
  RET(0x54b1); return; // ret
}

// 0d:54b2, bare global; normal ghini.
void ghini_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54b2, 0x54b3); A = mem_rd(gb, DE);
  CYC(0x54b3, 0x54b5); alu_sub(gb, 0x08);
  {
    CYC(0x54b5, 0x54b6); push_effect(gb, 0x54b6);
    uint16_t target = ghini_jump_table(gb);
    if (target == 0x54ba) goto state8;
    if (target == 0x54d0) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(0x54ba, 0x54bd); SET_BC(0x187f);
  CALL_C(0x54bd, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x54c0);
  CYC(0x54c0, 0x54c1); H = D;
  CYC(0x54c1, 0x54c3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x54c3, 0x54c5); A = 0x30;
  CYC(0x54c5, 0x54c6); alu_add(gb, C);
  CYC(0x54c6, 0x54c7); mem_wr(gb, HL, A);
  CYC(0x54c7, 0x54c9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x54c9, 0x54ca); mem_wr(gb, HL, B);
  CYC(0x54ca, 0x54cc); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x54cc, 0x54cd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x54cd, 0x54d0); ghini_updateAnimationFromAngle_hook(gb); return; // jp

state9:
  CALL_C(0x54d0, ghini_updateMovement_hook, 0x55fe, 0x54d3);
  CALL_C(0x54d3, ecom_decCounter1_b0d_hook, 0x439a, 0x54d6);
  if (!(F & FZ)) { CYCT(0x54d6, 0x54d8); goto animate; } // jr nz
  CYC(0x54d6, 0x54d8);
  CYC(0x54d8, 0x54da); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x54da, 0x54db); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));

animate:
  CYC(0x54db, 0x54de); enemyAnimate_hook(gb); return; // jp
}

// 0d:54de, bare global; takes a second to spawn in, and killing one of subid 1 makes all
// other die too.
void ghini_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54de, 0x54df); A = mem_rd(gb, DE);
  CYC(0x54df, 0x54e1); alu_sub(gb, 0x08);
  {
    CYC(0x54e1, 0x54e2); push_effect(gb, 0x54e2);
    uint16_t target = ghini_jump_table(gb);
    if (target == 0x54ec) goto state8;
    if (target == 0x5505) goto state9;
    if (target == 0x5536) goto stateA;
    if (target == 0x5547) goto stateB;
    if (target == 0x555a) goto stateC;
    HANDOFF(target);
  }

state8:
  CALL_C(0x54ec, ecom_decCounter1_b0d_hook, 0x439a, 0x54ef);
  if (F & FZ) { CYCT(0x54ef, 0x54f1); goto makeVisible; } // jr z
  CYC(0x54ef, 0x54f1);
  CYC(0x54f1, 0x54f2); A = mem_rd(gb, HL);
  CYC(0x54f2, 0x54f4); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x54f4); return; } // ret nz
  CYC(0x54f4, 0x54f5);
  CYC(0x54f5, 0x54f8); ecom_flickerVisibility_b0d_hook(gb); return; // jp

makeVisible:
  CYC(0x54f8, 0x54fa); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(0x54fa, 0x54fc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x54fc, 0x54fe); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x54fe, 0x5500); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5500, 0x5503); push_effect(gb, 0x5503); goto gotoStateC;
state8_resume: // resumes at 0x5503, itself a distinct "jr $5544" right after the call above
  CYC(0x5503, 0x5505); goto animate; // jr

state9:
  CALL_C(0x5505, ghini_updateMovement_hook, 0x55fe, 0x5508);
  CYC(0x5508, 0x550b); A = mem_rd(gb, wFrameCounter);
  CYC(0x550b, 0x550c); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x550c, 0x550e); goto animate; } // jr nc
  CYC(0x550c, 0x550e);
  CALL_C(0x550e, ecom_decCounter1_b0d_hook, 0x439a, 0x5511);
  if (F & FZ) { CYCT(0x5511, 0x5513); goto incState; } // jr z
  CYC(0x5511, 0x5513);
  CALL_C(0x5513, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5516);
  CYC(0x5516, 0x5518); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(0x5518, 0x551a); goto animate; } // jr nc
  CYC(0x5518, 0x551a);
  CYC(0x551a, 0x551d); SET_BC(0x1f1f);
  CALL_C(0x551d, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5520);
  CYC(0x5520, 0x5521); alu_or(gb, B);
  CYC(0x5521, 0x5522); A = C;
  if (F & FZ) { CALL_C(0x5522, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5525); } else { CYC(0x5522, 0x5525); } // call z
  CYC(0x5525, 0x5527); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5527, 0x5528); mem_wr(gb, DE, A);
  CALL_C(0x5528, ghini_updateAnimationFromAngle_hook, 0x5605, 0x552b);
  CYC(0x552b, 0x552d); goto animate; // jr

incState:
  CALL_C(0x552d, ecom_incState_b0d_hook, 0x4000, 0x5530);
  CYC(0x5530, 0x5532); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5532, 0x5534); mem_wr(gb, HL, 0x00);
  CYC(0x5534, 0x5536); goto animate; // jr

stateA:
  CYC(0x5536, 0x5537); H = D;
  CYC(0x5537, 0x5539); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5539, 0x553a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x553a, 0x553b); A = mem_rd(gb, HL);
  CYC(0x553b, 0x553d); alu_cp(gb, 0x80);
  if (F & FC) { CYCT(0x553d, 0x5540); ghini_updateMovementAndSetSpeedFromCounter1_hook(gb); return; } // jp c
  CYC(0x553d, 0x5540);
  CYC(0x5540, 0x5542); mem_wr(gb, HL, 0x80);
  CYC(0x5542, 0x5543); L = E;
  CYC(0x5543, 0x5544); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b

animate:
  CYC(0x5544, 0x5547); enemyAnimate_hook(gb);
  if (gb->pc == 0x5503 && gb->sp == sp0_) goto state8_resume;
  return; // jp

stateB:
  CALL_C(0x5547, ecom_decCounter1_b0d_hook, 0x439a, 0x554a);
  if (!(F & FZ)) { CYCT(0x554a, 0x554c); goto animate; } // jr nz
  CYC(0x554a, 0x554c);

gotoStateC:
  CYC(0x554c, 0x554e); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x554e, 0x5550); mem_wr(gb, HL, 0x0c);
  CYC(0x5550, 0x5552); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5552, 0x5554); mem_wr(gb, HL, 0x7f);
  CYC(0x5554, 0x5556); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5556, 0x5558); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x5558, 0x555a); goto animate; // jr

stateC:
  CALL_C(0x555a, ecom_decCounter1_b0d_hook, 0x439a, 0x555d);
  if (!(F & FZ)) { CYCT(0x555d, 0x5560); ghini_updateMovementAndSetSpeedFromCounter1_hook(gb); return; } // jp nz
  CYC(0x555d, 0x5560);
  CYC(0x5560, 0x5561); L = E;
  CYC(0x5561, 0x5563); mem_wr(gb, HL, 0x09); // [state] = 9
  CALL_C(0x5563, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5566);
  CYC(0x5566, 0x5568); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5568, 0x556a); alu_and(gb, 0x7f);
  CYC(0x556a, 0x556c); alu_add(gb, 0x7f);
  CYC(0x556c, 0x556d); mem_wr(gb, DE, A);
  CYC(0x556d, 0x556f); goto animate; // jr
}

// 0d:556f, bare global.
void ghini_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x556f, 0x5570); A = mem_rd(gb, DE);
  CYC(0x5570, 0x5572); alu_sub(gb, 0x08);
  {
    CYC(0x5572, 0x5573); push_effect(gb, 0x5573);
    uint16_t target = ghini_jump_table(gb);
    if (target == 0x557b) goto state8;
    if (target == 0x5589) goto state9;
    if (target == 0x559d) goto stateA;
    if (target == 0x55c4) goto stateB;
    HANDOFF(target);
  }

state8:
  CYC(0x557b, 0x557c); H = D;
  CYC(0x557c, 0x557d); L = E;
  CYC(0x557d, 0x557e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x557e, 0x5580); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5580, 0x5582); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(0x5582, 0x5584); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5584, 0x5586); mem_wr(gb, HL, 0x24);
  CALL_C(0x5586, ghini_chooseTargetPosition_hook, 0x5616, 0x5589);

state9:
  CALL_C(0x5589, ecom_decCounter1_b0d_hook, 0x439a, 0x558c);
  if (!(F & FZ)) { CYCT(0x558c, 0x558e); goto checkSpeedBump; } // jr nz
  CYC(0x558c, 0x558e);
  CYC(0x558e, 0x558f); L = E;
  CYC(0x558f, 0x5590); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x5590, 0x5592); goto stateA; // jr

checkSpeedBump:
  CYC(0x5592, 0x5593); A = mem_rd(gb, HL);
  CYC(0x5593, 0x5595); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x5595, 0x5597); goto stateA; } // jr nz
  CYC(0x5595, 0x5597);
  CYC(0x5597, 0x5599); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5599, 0x559a); A = mem_rd(gb, HL);
  CYC(0x559a, 0x559c); alu_add(gb, 0x05); // SPEED_20
  CYC(0x559c, 0x559d); mem_wr(gb, HL, A);

stateA:
  CYC(0x559d, 0x559e); H = D;
  CYC(0x559e, 0x55a0); L = ENEMY_BASE + 0x30; // Enemy.var30
  CALL_C(0x55a0, ecom_readPositionVars_b0d_hook, 0x4439, 0x55a3);
  CYC(0x55a3, 0x55a4); alu_sub(gb, C);
  CYC(0x55a4, 0x55a5); A = alu_inc8(gb, A);
  CYC(0x55a5, 0x55a7); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x55a7, 0x55a9); goto moveTowardTarget; } // jr nc
  CYC(0x55a7, 0x55a9);
  CYC(0x55a9, 0x55ab); A = H8(hFF8F);
  CYC(0x55ab, 0x55ac); alu_sub(gb, B);
  CYC(0x55ac, 0x55ad); A = alu_inc8(gb, A);
  CYC(0x55ad, 0x55af); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x55af, 0x55b1); goto moveTowardTarget; } // jr nc
  CYC(0x55af, 0x55b1);
  CYC(0x55b1, 0x55b3); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x55b3, 0x55b5); mem_wr(gb, HL, 0x0b);
  CYC(0x55b5, 0x55b7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x55b7, 0x55b9); mem_wr(gb, HL, 0x1c);
  CYC(0x55b9, 0x55bb); goto stateB; // jr

moveTowardTarget:
  CALL_C(0x55bb, ecom_moveTowardPosition_b0d_hook, 0x4430, 0x55be);
  CALL_C(0x55be, ghini_updateAnimationFromAngle_hook, 0x5605, 0x55c1);

animate:
  CYC(0x55c1, 0x55c4); enemyAnimate_hook(gb); return; // jp

stateB:
  CALL_C(0x55c4, ecom_decCounter1_b0d_hook, 0x439a, 0x55c7);
  if (F & FZ) { CYCT(0x55c7, 0x55c9); goto gotoState8; } // jr z
  CYC(0x55c7, 0x55c9);
  CYC(0x55c9, 0x55ca); A = mem_rd(gb, HL);
  CYC(0x55ca, 0x55cc); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x55cc, 0x55ce); goto applySpeed; } // jr nz
  CYC(0x55cc, 0x55ce);
  CYC(0x55ce, 0x55d0); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x55d0, 0x55d1); A = mem_rd(gb, HL);
  CYC(0x55d1, 0x55d3); alu_sub(gb, 0x05); // SPEED_20
  CYC(0x55d3, 0x55d4); mem_wr(gb, HL, A);

applySpeed:
  CALL_C(0x55d4, objectApplySpeed_hook, 0x201d, 0x55d7);
  CYC(0x55d7, 0x55d9); goto animate; // jr

gotoState8:
  CYC(0x55d9, 0x55da); L = E;
  CYC(0x55da, 0x55dc); mem_wr(gb, HL, 0x08);
  CYC(0x55dc, 0x557b); goto state8; // jr
}

// 0d:55de, bare global; called via tail jump only (never a real call), so it never needs
// to resume the caller — it always ends by handing off to enemyAnimate.
void ghini_updateMovementAndSetSpeedFromCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x55de, ghini_updateMovement_hook, 0x55fe, 0x55e1);
  CYC(0x55e1, 0x55e3); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x55e3, 0x55e4); A = mem_rd(gb, DE);
  CYC(0x55e4, 0x55e6); B = 0x00;
  CYC(0x55e6, 0x55e8); alu_cp(gb, 0x2a);
  if (F & FC) { CYCT(0x55e8, 0x55ea); goto haveIndex; } // jr c
  CYC(0x55e8, 0x55ea);
  CYC(0x55ea, 0x55eb); B = alu_inc8(gb, B);
  CYC(0x55eb, 0x55ed); alu_cp(gb, 0x54);
  if (F & FC) { CYCT(0x55ed, 0x55ef); goto haveIndex; } // jr c
  CYC(0x55ed, 0x55ef);
  CYC(0x55ef, 0x55f0); B = alu_inc8(gb, B);

haveIndex:
  CYC(0x55f0, 0x55f1); A = B;
  CYC(0x55f1, 0x55f4); SET_HL(0x55fb); // @speeds (data: SPEED_80, SPEED_40, SPEED_20)
  CYC(0x55f4, 0x55f5); ghini_addAToHl_from_rst(gb, 0x55f5);
  CYC(0x55f5, 0x55f7); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x55f7, 0x55f8); A = mem_rd(gb, HL);
  CYC(0x55f8, 0x55f9); mem_wr(gb, DE, A);
  CYC(0x55f9, 0x55fc); enemyAnimate_hook(gb); return; // jr to the trampoline at 0d:55c1, which is just enemyAnimate
}

// 0d:55fe, bare global.
void ghini_updateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x55fe, objectApplySpeed_hook, 0x201d, 0x5601);
  CALL_C(0x5601, ecom_bounceOffScreenBoundary_b0d_hook, 0x42e5, 0x5604);
  if (F & FZ) { RET_TAKEN(0x5604); return; } // ret z
  CYC(0x5604, 0x5605);
  ghini_updateAnimationFromAngle_hook(gb); return; // fallthrough
}

// 0d:5605, bare global.
void ghini_updateAnimationFromAngle_hook(GB *gb) {
  CYC(0x5605, 0x5606); H = D;
  CYC(0x5606, 0x5608); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5608, 0x5609); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5609, 0x560b); alu_cp(gb, 0x10);
  CYC(0x560b, 0x560d); A = 0x01;
  if (F & FC) { CYCT(0x560d, 0x560f); goto compare; } // jr c
  CYC(0x560d, 0x560f);
  CYC(0x560f, 0x5610); A = alu_dec8(gb, A);

compare:
  CYC(0x5610, 0x5611); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5611); return; } // ret z
  CYC(0x5611, 0x5612);
  CYC(0x5612, 0x5613); mem_wr(gb, HL, A);
  CYC(0x5613, 0x5616); enemySetAnimation_hook(gb); return; // jp
}

// 0d:5616, bare global; sets var30/var31 to target position for subid 2.
void ghini_chooseTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5616, 0x5619); SET_BC(0x7070);
  CALL_C(0x5619, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x561c);
  CYC(0x561c, 0x561d); A = B;
  CYC(0x561d, 0x561f); alu_sub(gb, 0x20);
  if (!(F & FC)) { CYCT(0x561f, 0x5621); goto haveYOffset; } // jr nc
  CYC(0x561f, 0x5621);
  CYC(0x5621, 0x5622); alu_xor(gb, A);

haveYOffset:
  CYC(0x5622, 0x5623); B = A;
  CYC(0x5623, 0x5626); SET_HL(wRoomEdgeY);
  CYC(0x5626, 0x5627); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5627, 0x5629); A = alu_srl(gb, A);
  CYC(0x5629, 0x562a); alu_add(gb, B);
  CYC(0x562a, 0x562c); alu_sub(gb, 0x28);
  CYC(0x562c, 0x562d); B = A;
  CYC(0x562d, 0x562e); A = mem_rd(gb, HL);
  CYC(0x562e, 0x5630); A = alu_srl(gb, A);
  CYC(0x5630, 0x5631); alu_add(gb, C);
  CYC(0x5631, 0x5633); alu_sub(gb, 0x38);
  CYC(0x5633, 0x5634); C = A;
  CYC(0x5634, 0x5635); H = D;
  CYC(0x5635, 0x5637); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5637, 0x5638); mem_wr(gb, HL, B);
  CYC(0x5638, 0x5639); L = alu_inc8(gb, L);
  CYC(0x5639, 0x563a); mem_wr(gb, HL, C);
  RET(0x563a); return; // ret
}
