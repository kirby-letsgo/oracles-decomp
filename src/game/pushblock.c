#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define pushableTilePropertiesTable_bank08 0x452d

#define OBJ_VAR30 0x30
#define OBJ_VAR31 0x31

#define TILEINDEX_BUTTON 0x0c
#define TILEINDEX_RED_PUSHABLE_BLOCK 0x2c

static uint16_t pushblock_jumpTable(GB *gb) {
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

static void pushblock_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

// interactionCode14@updateZPositionForButton: raise z by 2 pixels when sitting on an unpressed button.
static void pushblock_updateZPositionForButton(GB *gb, uint16_t sp0_) {
  CYC(0x44ca, 0x44cd); A = mem_rd(gb, wTilesetFlags);
  CYC(0x44cd, 0x44cf); alu_and(gb, 0x18);
  if (F & FZ) {
    CYCT(0x44cf, 0x44d0); ret_effect(gb); return;
  }
  CYC(0x44cf, 0x44d0);
  CALL_C(0x44d0, objectGetShortPosition_hook, 0x2096, 0x44d3);
  CYC(0x44d3, 0x44d4); C = A;
  CYC(0x44d4, 0x44d6); B = wRoomLayout >> 8;
  CYC(0x44d6, 0x44d7); A = mem_rd(gb, BC);
  CYC(0x44d7, 0x44d9); alu_cp(gb, TILEINDEX_BUTTON);
  CYC(0x44d9, 0x44db); A = 0xfe;
  if (F & FZ) {
    CYCT(0x44db, 0x44dd);
  } else {
    CYC(0x44db, 0x44dd);
    CYC(0x44dd, 0x44de); alu_xor(gb, A);
  }
  CYC(0x44de, 0x44e0); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x44e0, 0x44e1); mem_wr(gb, DE, A);
  CYC(0x44e1, 0x44e2); ret_effect(gb);
}

// interactionCode14@replaceTileUnderneathBlock: restore the ground tile at var30 (from the room
// layout buffer if non-solid, else var32). Tails into setTile, whose ret pops our return address.
static void pushblock_replaceTileUnderneathBlock(GB *gb, uint16_t sp0_) {
  CYC(0x44e2, 0x44e4); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x44e4, 0x44e5); A = mem_rd(gb, DE);
  CYC(0x44e5, 0x44e6); C = A;
  CALL_C(0x44e6, getTileIndexFromRoomLayoutBuffer_paramC_hook, 0x15d8, 0x44e9);
  if (!(F & FC)) {
    CYCT(0x44e9, 0x44ec); setTile_hook(gb); return;
  }
  CYC(0x44e9, 0x44ec);
  CYC(0x44ec, 0x44ee); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x44ee, 0x44ef); A = mem_rd(gb, DE);
  CYC(0x44ef, 0x44f2); setTile_hook(gb);
}

// interactionCode14@checkRotatingCubePermitsPushing: carry set means this block may not be pushed.
static void pushblock_checkRotatingCubePermitsPushing(GB *gb) {
  CYC(0x44f2, 0x44f5); A = mem_rd(gb, wRotatingCubePos);
  CYC(0x44f5, 0x44f6); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x44f6, 0x44f7); ret_effect(gb); return;
  }
  CYC(0x44f6, 0x44f7);
  CYC(0x44f7, 0x44fa); A = mem_rd(gb, wRotatingCubeColor);
  CYC(0x44fa, 0x44fc); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x44fc, 0x44fe); goto deny;
  }
  CYC(0x44fc, 0x44fe);
  CYC(0x44fe, 0x4500); alu_and(gb, 0x7f);
  CYC(0x4500, 0x4501); B = A;
  CYC(0x4501, 0x4503); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(0x4503, 0x4504); A = mem_rd(gb, DE);
  CYC(0x4504, 0x4506); alu_sub(gb, TILEINDEX_RED_PUSHABLE_BLOCK);
  CYC(0x4506, 0x4507); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4507, 0x4508); ret_effect(gb); return;
  }
  CYC(0x4507, 0x4508);
deny:
  CYC(0x4508, 0x4509); alu_scf(gb);
  CYC(0x4509, 0x450a); ret_effect(gb);
}

// interactionCode14@loadPushableTileProperties: look up var31 in the pushable tile table for the
// active collision set and copy its 4 bytes into var31-var34.
static void pushblock_loadPushableTileProperties(GB *gb) {
  CYC(0x450a, 0x450d); A = mem_rd(gb, wActiveCollisions);
  CYC(0x450d, 0x4510); SET_HL(pushableTilePropertiesTable_bank08);
  CYC(0x4510, 0x4511); push_effect(gb, 0x4511); pushblock_addAToHl(gb);
  CYC(0x4511, 0x4512); A = mem_rd(gb, HL);
  CYC(0x4512, 0x4513); push_effect(gb, 0x4513); pushblock_addAToHl(gb);
  CYC(0x4513, 0x4515); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(0x4515, 0x4516); A = mem_rd(gb, DE);
  CYC(0x4516, 0x4517); B = A;
search:
  CYC(0x4517, 0x4518); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4518, 0x4519); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4519, 0x451a); ret_effect(gb); return;
  }
  CYC(0x4519, 0x451a);
  CYC(0x451a, 0x451b); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x451b, 0x451d); goto match;
  }
  CYC(0x451b, 0x451d);
  CYC(0x451d, 0x451e); SET_HL(HL + 1);
  CYC(0x451e, 0x451f); SET_HL(HL + 1);
  CYC(0x451f, 0x4520); SET_HL(HL + 1);
  CYC(0x4520, 0x4522); goto search;
match:
  CYC(0x4522, 0x4523); mem_wr(gb, DE, A);
  CYC(0x4523, 0x4524); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4524, 0x4525); E = alu_inc8(gb, E);
  CYC(0x4525, 0x4526); mem_wr(gb, DE, A);
  CYC(0x4526, 0x4527); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4527, 0x4528); E = alu_inc8(gb, E);
  CYC(0x4528, 0x4529); mem_wr(gb, DE, A);
  CYC(0x4529, 0x452a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x452a, 0x452b); E = alu_inc8(gb, E);
  CYC(0x452b, 0x452c); mem_wr(gb, DE, A);
  CYC(0x452c, 0x452d); ret_effect(gb);
}

// interactionCode14@state1 (also reached by fallthrough from state0): move the block, and once
// counter1 expires (@func_449d) settle it and delete self.
static void pushblock_state1(GB *gb, uint16_t sp0_) {
  CYC(0x4490, 0x4493); push_effect(gb, 0x4493); pushblock_updateZPositionForButton(gb, sp0_);
  CALL_C(0x4493, objectApplySpeed_hook, 0x201d, 0x4496);
  CALL_C(0x4496, objectPreventLinkFromPassing_hook, 0x2680, 0x4499);
  CALL_C(0x4499, interactionDecCounter1_hook, 0x23cc, 0x449c);
  if (!(F & FZ)) {
    CYCT(0x449c, 0x449d); ret_effect(gb); return;
  }
  CYC(0x449c, 0x449d);
  CALL_C(0x449d, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x44a0);
  if (F & FC) {
    CYCT(0x44a0, 0x44a3); interactionDelete_hook(gb); return;
  }
  CYC(0x44a0, 0x44a3);
  CALL_C(0x44a3, objectGetShortPosition_hook, 0x2096, 0x44a6);
  CYC(0x44a6, 0x44a8); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x44a8, 0x44a9); mem_wr(gb, DE, A);
  CYC(0x44a9, 0x44ab); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x44ab, 0x44ac); A = mem_rd(gb, DE);
  CYC(0x44ac, 0x44ad); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x44ad, 0x44af); goto checkSound;
  }
  CYC(0x44ad, 0x44af);
  CYC(0x44af, 0x44b0); B = A;
  CYC(0x44b0, 0x44b2); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x44b2, 0x44b3); A = mem_rd(gb, DE);
  CYC(0x44b3, 0x44b4); C = A;
  CYC(0x44b4, 0x44b5); A = B;
  CALL_C(0x44b5, setTile_hook, 0x3a9c, 0x44b8);
checkSound:
  CYC(0x44b8, 0x44ba); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x44ba, 0x44bb); A = mem_rd(gb, DE);
  CYC(0x44bb, 0x44bc); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x44bc, 0x44be); goto delete;
  }
  CYC(0x44bc, 0x44be);
  CYC(0x44be, 0x44bf); alu_xor(gb, A);
  CYC(0x44bf, 0x44c2); mem_wr(gb, wDisabledObjects, A);
  CYC(0x44c2, 0x44c4); A = 0x4d;
  CALL_C(0x44c4, playSound_b00_hook, 0x0c98, 0x44c7);
delete:
  CYC(0x44c7, 0x44ca); interactionDelete_hook(gb);
}

// interactionCode14@state0: block just pushed. Falls through into state1.
static void pushblock_state0(GB *gb, uint16_t sp0_) {
  CYC(0x4438, 0x443a); A = 0x01;
  CYC(0x443a, 0x443b); mem_wr(gb, DE, A);
  CALL_C(0x443b, interactionInitGraphics_hook, 0x15fb, 0x443e);
  CYC(0x443e, 0x4440); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(0x4440, 0x4441); A = mem_rd(gb, DE);
  CYC(0x4441, 0x4442); C = A;
  CYC(0x4442, 0x4444); B = wRoomLayout >> 8;
  CYC(0x4444, 0x4445); A = mem_rd(gb, BC);
  CYC(0x4445, 0x4447); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(0x4447, 0x4448); mem_wr(gb, DE, A);
  CALL_C(0x4448, objectMimicBgTile_hook, 0x233b, 0x444b);
  CYC(0x444b, 0x444e); push_effect(gb, 0x444e); pushblock_checkRotatingCubePermitsPushing(gb);
  if (F & FC) {
    CYCT(0x444e, 0x4451); interactionDelete_hook(gb); return;
  }
  CYC(0x444e, 0x4451);
  CYC(0x4451, 0x4453); A = 0x06;
  CALL_C(0x4453, objectSetCollideRadius_hook, 0x24a1, 0x4456);
  CYC(0x4456, 0x4459); push_effect(gb, 0x4459); pushblock_loadPushableTileProperties(gb);
  CYC(0x4459, 0x445a); H = D;
  CYC(0x445a, 0x445c); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x445c, 0x445e); alu_bit(gb, 2, mem_rd(gb, HL));
  CYC(0x445e, 0x4460); A = 0x01;
  if (!(F & FZ)) {
    CALL_C_CC(0x4460, interactionSetAnimation_hook, 0x262e, 0x4463);
  } else {
    CYC(0x4460, 0x4463);
  }
  CYC(0x4463, 0x4464); H = D;
  CYC(0x4464, 0x4467); SET_BC(0x1420);
  CYC(0x4467, 0x446a); A = mem_rd(gb, wBraceletLevel);
  CYC(0x446a, 0x446c); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x446c, 0x446e); goto setSpeed;
  }
  CYC(0x446c, 0x446e);
  CYC(0x446e, 0x4470); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(0x4470, 0x4472); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4472, 0x4474); goto setSpeed;
  }
  CYC(0x4472, 0x4474);
  CYC(0x4474, 0x4477); SET_BC(0x1e15);
setSpeed:
  CYC(0x4477, 0x4479); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4479, 0x447a); mem_wr(gb, HL, B);
  CYC(0x447a, 0x447c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x447c, 0x447d); mem_wr(gb, HL, C);
  CYC(0x447d, 0x447f); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x447f, 0x4480); A = mem_rd(gb, HL);
  CYC(0x4480, 0x4482); alu_or(gb, 0x80);
  CYC(0x4482, 0x4485); mem_wr(gb, wBlockPushAngle, A);
  CYC(0x4485, 0x4488); push_effect(gb, 0x4488); pushblock_replaceTileUnderneathBlock(gb, sp0_);
  CALL_C(0x4488, objectSetVisible82_hook, 0x1e69, 0x448b);
  CYC(0x448b, 0x448d); A = 0x71;
  CALL_C(0x448d, playSound_b00_hook, 0x0c98, 0x4490);
  pushblock_state1(gb, sp0_);
}

void interactionCode14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4430, 0x4432); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4432, 0x4433); A = mem_rd(gb, DE);
  CYC(0x4433, 0x4434); push_effect(gb, 0x4434);
  switch (pushblock_jumpTable(gb)) {
    case 0x4438: pushblock_state0(gb, sp0_); return;
    case 0x4490: pushblock_state1(gb, sp0_); return;
    default: HANDOFF(HL);
  }
}
