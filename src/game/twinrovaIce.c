#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void twinrovaIce_bounceOffShield_hook(GB *gb);

static uint16_t twinrovaIce_jump_table(GB *gb) {
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

static void twinrovaIce_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_TWINROVA_ICE
//
// Variables:
//   var3e: ?
// ==================================================================================================
void enemyCode5d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x690f, 0x6911); goto normalStatus; } // jr z
  CYC(0x690f, 0x6911);
  CYC(0x6911, 0x6913); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6913); return; } // ret c
  CYC(0x6913, 0x6914);
  CYC(0x6914, 0x6916); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x6916, 0x6917); A = mem_rd(gb, DE);
  CYC(0x6917, 0x6919); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { CYCT(0x6919, 0x691b); goto normalStatus; } // jr z
  CYC(0x6919, 0x691b);
  CYC(0x691b, 0x691d); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x691d, 0x691f); alu_sub(gb, 0x02); // ITEMCOLLISION_L2_SHIELD
  CYC(0x691f, 0x6921); alu_cp(gb, 0x02); // ITEMCOLLISION_L3_SHIELD-ITEMCOLLISION_L2_SHIELD+1
  if (F & FC) CALL_C_CC(0x6921, twinrovaIce_bounceOffShield_hook, 0x6972, 0x6924); else CYC(0x6921, 0x6924); // call c
  CALL_C(0x6924, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, 0x43ab, 0x6927);

normalStatus:
  CYC(0x6927, 0x6929); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6929, 0x692a); A = mem_rd(gb, DE);
  {
    CYC(0x692a, 0x692b); push_effect(gb, 0x692b);
    uint16_t target = twinrovaIce_jump_table(gb);
    if (target == 0x6931) goto state0;
    if (target == 0x6948) goto state1;
    if (target == 0x6950) goto state2;
    HANDOFF(target);
  }

state0:
  CYC(0x6931, 0x6932); H = D;
  CYC(0x6932, 0x6933); L = E;
  CYC(0x6933, 0x6934); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6934, 0x6936); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x6936, 0x6938); mem_wr(gb, HL, 0x46); // SPEED_1c0
  CYC(0x6938, 0x693a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x693a, 0x693c); mem_wr(gb, HL, 120);
  CYC(0x693c, 0x693e); L = ENEMY_BASE + 0x3e; // Enemy.var3e
  CYC(0x693e, 0x6940); mem_wr(gb, HL, 0x08);
  CYC(0x6940, 0x6942); A = 0x98; // SND_POOF
  CALL_C(0x6942, playSound_b00_hook, 0x0c98, 0x6945);
  CYC(0x6945, 0x6948); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x6948, ecom_decCounter1_b0e_hook, 0x439a, 0x694b);
  if (!(F & FZ)) { CYCT(0x694b, 0x694e); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x694b, 0x694e);
  CYC(0x694e, 0x694f); L = E;
  CYC(0x694f, 0x6950); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

state2:
  CYC(0x6950, 0x6952); A = OBJ_HEALTH; // Object.health
  CALL_C(0x6952, objectGetRelatedObject1Var_hook, 0x2160, 0x6955);
  CYC(0x6955, 0x6956); A = mem_rd(gb, HL);
  CYC(0x6956, 0x6957); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6957, 0x6959); goto delete; } // jr z
  CYC(0x6957, 0x6959);
  CYC(0x6959, 0x695b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x695b, 0x695c); A = mem_rd(gb, HL);
  CYC(0x695c, 0x695e); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(0x695e, 0x6960); goto delete; } // jr z
  CYC(0x695e, 0x6960);
  CALL_C(0x6960, objectApplySpeed_hook, 0x201d, 0x6963);
  CALL_C(0x6963, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x6966);
  if (F & FZ) { RET_TAKEN(0x6966); return; } // ret z
  CYC(0x6966, 0x6967);
  CYC(0x6967, 0x6969); A = 0x50; // SND_CLINK
  CYC(0x6969, 0x696c); playSound_b00_hook(gb); return; // jp

delete:
  CALL_C(0x696c, objectCreatePuff_hook, 0x24c1, 0x696f);
  CYC(0x696f, 0x6972); enemyDelete_hook(gb); return; // jp
}

// 0e:6972, bare global; called from enemyCode5d. This doesn't appear to do anything other
// than make a sound, because the angle is immediately overwritten after this is called.
void twinrovaIce_bounceOffShield_hook(GB *gb) {
  CYC(0x6972, 0x6975); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(0x6975, 0x6977); A = alu_swap(gb, A);
  CYC(0x6977, 0x6978); B = A;
  CYC(0x6978, 0x697a); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x697a, 0x697b); A = mem_rd(gb, DE);
  CYC(0x697b, 0x697c); alu_add(gb, B);
  CYC(0x697c, 0x697f); SET_HL(0x6989); // @bounceTable
  CYC(0x697f, 0x6980); twinrovaIce_addAToHl_from_rst(gb, 0x6980);
  CYC(0x6980, 0x6982); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6982, 0x6983); A = mem_rd(gb, HL);
  CYC(0x6983, 0x6984); mem_wr(gb, DE, A);
  CYC(0x6984, 0x6986); A = 0x50; // SND_CLINK
  CYC(0x6986, 0x6989); playSound_b00_hook(gb); return; // jp
}
