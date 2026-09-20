#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode77_jump_table(GB *gb) {
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

// INTERAC_SMALL_KEY_ON_ENEMY
void interactionCode77_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6017, 0x6019); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6019, 0x601a); A = mem_rd(gb, DE);
  {
    CYC(0x601a, 0x601b); push_effect(gb, 0x601b);
    uint16_t target = interactionCode77_jump_table(gb);
    if (target == 0x6056) goto state1;
    if (target == 0x606d) goto state2;
  }

  // state0
  CALL_C(0x6021, getThisRoomFlags_hook, 0x197d, 0x6024);
  CYC(0x6024, 0x6026); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(0x6026, 0x6029); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x6026, 0x6029);
  CYC(0x6029, 0x602b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x602b, 0x602c); A = mem_rd(gb, DE);
  CYC(0x602c, 0x602d); B = A;
  CYC(0x602d, 0x6030); SET_HL(ENEMY_SLOTS + OBJ_ID); // FIRST_ENEMY_INDEX, Enemy.id

nextEnemy:
  CYC(0x6030, 0x6031); A = mem_rd(gb, HL);
  CYC(0x6031, 0x6032); alu_cp(gb, B);
  if (F & FZ) { CYCT(0x6032, 0x6034); goto foundMatch; } // jr z
  CYC(0x6032, 0x6034);
  CYC(0x6034, 0x6035); H = alu_inc8(gb, H);
  CYC(0x6035, 0x6036); A = H;
  CYC(0x6036, 0x6038); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  // BUG: original game checks carry here, so this only works if the enemy is in the first slot.
  if (F & FC) { CYCT(0x6038, 0x603b); interactionDelete_hook(gb); return; } // jp c
  CYC(0x6038, 0x603b);
  CYC(0x603b, 0x603d); goto nextEnemy; // jr

foundMatch:
  CYC(0x603d, 0x603e); L = alu_dec8(gb, L);
  CYC(0x603e, 0x603f); A = L;
  CYC(0x603f, 0x6041); E = INTERACTION_BASE + OBJ_RELATED2; // Interaction.relatedObj2 low byte
  CYC(0x6041, 0x6042); mem_wr(gb, DE, A);
  CYC(0x6042, 0x6043); A = H;
  CYC(0x6043, 0x6044); E = alu_inc8(gb, E);
  CYC(0x6044, 0x6045); mem_wr(gb, DE, A);
  CALL_C(0x6045, interactionInitGraphics_hook, 0x15fb, 0x6048); // SWITCHES THREADS
  CALL_C(0x6048, objectSetVisible80_hook, 0x1e57, 0x604b);
  CALL_C(0x604b, interactionIncState_hook, 0x23e0, 0x604e);

takeRelatedObj2Position:
  CYC(0x604e, 0x6050); A = OBJ_Y; // Object.y
  CALL_C(0x6050, objectGetRelatedObject2Var_hook, 0x2164, 0x6053);
  CYC(0x6053, 0x6056); objectTakePosition_hook(gb);
  if (gb->pc == 0x6063 && gb->sp == sp0_) goto afterTakeRelatedObj2Position;
  return; // jp

state1:
  CYC(0x6056, 0x6058); A = OBJ_ENABLED; // Object.enabled
  CALL_C(0x6058, objectGetRelatedObject2Var_hook, 0x2164, 0x605b);
  CYC(0x605b, 0x605c); A = mem_rd(gb, HL);
  CYC(0x605c, 0x605d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x605d, 0x6060); interactionIncState_hook(gb); return; } // jp z
  CYC(0x605d, 0x6060);
  CYC(0x6060, 0x6063); push_effect(gb, 0x6063); goto takeRelatedObj2Position;

afterTakeRelatedObj2Position:
  CYC(0x6063, 0x6065); A = OBJ_VISIBLE; // Object.visible
  CALL_C(0x6065, objectGetRelatedObject2Var_hook, 0x2164, 0x6068);
  CYC(0x6068, 0x606a); B = 0x01;
  CYC(0x606a, 0x606d); objectFlickerVisibility_hook(gb); return; // jp

state2:
  CALL_C(0x606d, objectSetVisible_hook, 0x1e84, 0x6070);
  CYC(0x6070, 0x6072); C = 0x20;
  CALL_C(0x6072, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6075);
  if (!(F & FZ)) { RET_TAKEN(0x6075); return; } // ret nz
  CYC(0x6075, 0x6076);
  CYC(0x6076, 0x6079); SET_BC(0x3000); // TREASURE_SMALL_KEY, $00
  CALL_C(0x6079, createTreasure_hook, 0x27d4, 0x607c);
  CALL_C(0x607c, objectCopyPosition_hook, 0x2242, 0x607f);
  CYC(0x607f, 0x6082); interactionDelete_hook(gb); return; // jp
}
