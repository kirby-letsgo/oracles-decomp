#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t sarcophagus_jump_table(GB *gb) {
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

// INTERAC_SARCOPHAGUS
void interactionCode82_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x62f6, 0x62f8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x62f8, 0x62f9); A = mem_rd(gb, DE);
  {
    CYC(0x62f9, 0x62fa); push_effect(gb, 0x62fa);
    uint16_t target = sarcophagus_jump_table(gb);
    if (target == 0x6337) goto state1;
    if (target == 0x6340) goto state2;
    if (target == 0x639c) goto state3;
  }

  // interactionCode82@state0
  CALL_C(0x6302, interactionInitGraphics_hook, 0x15fb, 0x6305);
  CYC(0x6305, 0x6307); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6307, 0x6308); A = mem_rd(gb, DE);
  CYC(0x6308, 0x630a); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x630a, 0x630d); goto breakOut; } // jp nz
  CYC(0x630a, 0x630d);
  CYC(0x630d, 0x630e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x630e, 0x6310); goto afterRoomFlagCheck; } // jr z
  CYC(0x630e, 0x6310);
  CALL_C(0x6310, getThisRoomFlags_hook, 0x197d, 0x6313);
  CYC(0x6313, 0x6315); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x6315, 0x6318); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x6315, 0x6318);

afterRoomFlagCheck:
  CALL_C(0x6318, interactionIncState_hook, 0x23e0, 0x631b);
  CYC(0x631b, 0x631d); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x631d, 0x631f); mem_wr(gb, HL, 0x10);
  CYC(0x631f, 0x6321); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x6321, 0x6323); mem_wr(gb, HL, 0x08);
  CALL_C(0x6323, objectMakeTileSolid_hook, 0x20b2, 0x6326);
  CYC(0x6326, 0x6328); H = wRoomLayout >> 8;
  CYC(0x6328, 0x632a); mem_wr(gb, HL, 0x00);
  CYC(0x632a, 0x632b); A = L;
  CYC(0x632b, 0x632d); alu_sub(gb, 0x10);
  CYC(0x632d, 0x632e); L = A;
  CYC(0x632e, 0x6330); mem_wr(gb, HL, 0x00);
  CYC(0x6330, 0x6332); H = wRoomCollisions >> 8;
  CYC(0x6332, 0x6334); mem_wr(gb, HL, 0x0f);
  CYC(0x6334, 0x6337); objectSetVisible83_hook(gb); return; // jp

state1:
  CYC(0x6337, 0x633a); A = W8(wBraceletLevel);
  CYC(0x633a, 0x633c); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(0x633c); return; } // ret c
  CYC(0x633c, 0x633d);
  CYC(0x633d, 0x6340); objectAddToGrabbableObjectBuffer_hook(gb); return; // jp

state2:
  CYC(0x6340, 0x6341); E = alu_inc8(gb, E);
  CYC(0x6341, 0x6342); A = mem_rd(gb, DE);
  {
    CYC(0x6342, 0x6343); push_effect(gb, 0x6343);
    uint16_t target = sarcophagus_jump_table(gb);
    if (target == 0x6379) goto substate1Holding;
    if (target == 0x637a) goto substate2JustReleased;
    if (target == 0x6384) goto breakOut;
  }

  // interactionCode82@substate0_justGrabbed
  CALL_C(0x634b, interactionIncSubstate_hook, 0x23e5, 0x634e);
  CYC(0x634e, 0x6350); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6350, 0x6351); A = mem_rd(gb, HL);
  CYC(0x6351, 0x6352); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6352, 0x6354); goto placeGrabbedTile; } // jr z
  CYC(0x6352, 0x6354);
  CYC(0x6354, 0x6355); A = alu_dec8(gb, A);
  CYC(0x6355, 0x6357); A = 0x4d; // SND_SOLVEPUZZLE
  if (F & FZ) CALL_C_CC(0x6357, playSound_b00_hook, 0x0c98, 0x635a); else CYC(0x6357, 0x635a);
  CALL_C(0x635a, getThisRoomFlags_hook, 0x197d, 0x635d);
  CYC(0x635d, 0x635f); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)

placeGrabbedTile:
  CALL_C(0x635f, objectGetShortPosition_hook, 0x2096, 0x6362);
  CYC(0x6362, 0x6363); push_effect(gb, AF);
  CALL_C(0x6363, getTileIndexFromRoomLayoutBuffer_hook, 0x15d7, 0x6366);
  CALL_C(0x6366, setTile_hook, 0x3a9c, 0x6369);
  CYC(0x6369, 0x636a); SET_AF(pop_effect(gb));
  CYC(0x636a, 0x636c); alu_sub(gb, 0x10);
  CALL_C(0x636c, getTileIndexFromRoomLayoutBuffer_hook, 0x15d7, 0x636f);
  CALL_C(0x636f, setTile_hook, 0x3a9c, 0x6372);
  CYC(0x6372, 0x6373); alu_xor(gb, A);
  CYC(0x6373, 0x6376); W8(wLinkGrabState2) = A;
  CYC(0x6376, 0x6379); objectSetVisiblec1_hook(gb); return; // jp

substate1Holding:
  RET(0x6379); return; // ret

substate2JustReleased:
  CYC(0x637a, 0x637b); H = D;
  CYC(0x637b, 0x637d); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(0x637d, 0x637f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1))); // res 1,(hl)
  CYC(0x637f, 0x6381); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x6381, 0x6383); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x6383); return; } // ret nz
  CYC(0x6383, 0x6384);

breakOut:
  CYC(0x6384, 0x6385); H = D;
  CYC(0x6385, 0x6387); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6387, 0x6389); mem_wr(gb, HL, 0x03);
  CYC(0x6389, 0x638b); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x638b, 0x638d); mem_wr(gb, HL, 0x02);
  CYC(0x638d, 0x638f); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x638f, 0x6391); A = 0x0c;
  CYC(0x6391, 0x6392); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6392, 0x6393); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6393, 0x6395); mem_wr(gb, HL, 0x40); // [oamTileIndexBase] = $40
  CALL_C(0x6395, objectSetVisible83_hook, 0x1e72, 0x6398);
  CYC(0x6398, 0x6399); alu_xor(gb, A);
  CYC(0x6399, 0x639c); interactionSetAnimation_hook(gb); return; // jp

state3:
  CALL_C(0x639c, interactionDecCounter1_hook, 0x23cc, 0x639f);
  CYC(0x639f, 0x63a1); A = 0x73; // SND_KILLENEMY
  if (F & FZ) CALL_C_CC(0x63a1, playSound_b00_hook, 0x0c98, 0x63a4); else CYC(0x63a1, 0x63a4);
  CYC(0x63a4, 0x63a6); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x63a6, 0x63a7); A = mem_rd(gb, DE);
  CYC(0x63a7, 0x63a8); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x63a8, 0x63ab); interactionAnimate_hook(gb); return; } // jp nz
  CYC(0x63a8, 0x63ab);
  CYC(0x63ab, 0x63ae); interactionDelete_hook(gb); return; // jp
}
