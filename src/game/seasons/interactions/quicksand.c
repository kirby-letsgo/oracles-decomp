#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/quicksand.s.
// INTERAC_QUICKSAND

static uint16_t quicksand_jump_table(GB *gb) {
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

static void quicksand_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCode5e@checkLinkWithinAPartOfQuicksand
static void quicksand_checkLinkWithinAPartOfQuicksand(GB *gb) {
  BASE(interactionCode5e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+195, b_+196); H = D;
  CYC(b_+196, b_+198); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+198, b_+199); mem_wr(gb, HL, B);
  CYC(b_+199, b_+200); L = alu_inc8(gb, L);
  CYC(b_+200, b_+201); mem_wr(gb, HL, B);
  CYC(b_+201, b_+204); A = mem_rd(gb, w1Link_yh);
  CYC(b_+204, b_+205); alu_add(gb, C);
  CYC(b_+205, b_+206); B = A;
  CYC(b_+206, b_+209); A = mem_rd(gb, w1Link_xh);
  CYC(b_+209, b_+210); C = A;
  CYC(b_+210, b_+213);
  TAIL(interactionCheckContainsPoint);
}

// INTERAC_QUICKSAND
void s_interactionCode5e_hook(GB *gb) {
  BASE(interactionCode5e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (quicksand_jump_table(gb));
    if (jt_ == b_+13) goto state0;
    if (jt_ == b_+16) goto state1;
    if (jt_ == b_+121) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
state1:
  CYC(b_+16, b_+18); A = 0x21;
  CALL_C(b_+18, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+21);
  CALL_C(b_+21, s__findItemDropAddress_hook, SYM(_findItemDropAddress), b_+24);
  CALL_C(b_+24, s__findPirateSkullAddress_hook, SYM(_findPirateSkullAddress), b_+27);
  CALL_C(b_+27, s__findBombOrScentSeedAddress_hook, SYM(_findBombOrScentSeedAddress), b_+30);
  CYC(b_+30, b_+33); A = mem_rd(gb, w1Link_state);
  CYC(b_+33, b_+35); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; }
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+39); A = mem_rd(gb, w1Link_zh);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+40); return; }
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+44); SET_BC(0x2105);
  CALL_L(b_+44, quicksand_checkLinkWithinAPartOfQuicksand, b_+47);
  if (!(F & FC)) { RET_TAKEN(b_+47); return; }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0x23;
  CALL_C(b_+50, s_cpActiveRing, SYM(cpActiveRing), b_+53);
  if (F & FZ) { CYCT(b_+53, b_+55); goto L_4c10; }
  CYC(b_+53, b_+55);
  CALL_C(b_+55, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+58);
  CYC(b_+58, b_+60); alu_xor(gb, 0x10);
  CYC(b_+60, b_+61); C = A;
  CYC(b_+61, b_+63); B = 0x14;
  CALL_C(b_+63, s_updateLinkPositionGivenVelocity, SYM(updateLinkPositionGivenVelocity), b_+66);
L_4c10:
  CALL_C(b_+66, s__matchSkullNumberWithSubid_hook, SYM(_matchSkullNumberWithSubid), b_+69);
  CYC(b_+69, b_+72); SET_BC(0x0300);
  CALL_L(b_+72, quicksand_checkLinkWithinAPartOfQuicksand, b_+75);
  if (!(F & FC)) { RET_TAKEN(b_+75); return; }
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+78); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+80); alu_or(gb, A);
  CYC(b_+80, b_+82); A = 0x01;
  if (F & FZ) { CYCT(b_+82, b_+84); goto respawnLink; }
  CYC(b_+82, b_+84);
  CALL_C(b_+84, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+87);
  CALL_C(b_+87, s_clearAllParentItems, SYM(clearAllParentItems), b_+90);
  CYC(b_+90, b_+91); H = D;
  CYC(b_+91, b_+93); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x02);
  CYC(b_+95, b_+98); A = mem_rd(gb, wPirateSkullRandomNumber);
  CYC(b_+98, b_+100); alu_and(gb, 0x7f);
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+102, b_+103); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x3c);
  CYC(b_+105, b_+107); A = 0x03;
respawnLink:
  CYC(b_+107, b_+110); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+110, b_+112); A = 0x02;
  CYC(b_+112, b_+115); mem_wr(gb, wLinkForceState, A);
  CYC(b_+115, b_+118); SET_HL(w1Link_yh);
  CYC(b_+118, b_+121);
  TAIL(objectCopyPosition);
state2:
  CYC(b_+121, b_+122); alu_xor(gb, A);
  CYC(b_+122, b_+125); mem_wr(gb, wPirateSkullRandomNumber, A);
  CALL_C(b_+125, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+128);
  if (!(F & FZ)) { RET_TAKEN(b_+128); return; }
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+131); C = 0x03;
  CYC(b_+131, b_+133); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+133, b_+134); A = mem_rd(gb, HL);
  CYC(b_+134, b_+136); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+136, b_+138); goto L_4c65; }
  CYC(b_+136, b_+138);
  CYC(b_+138, b_+139); C = alu_dec8(gb, C);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+143, b_+145); goto L_4c65; }
  CYC(b_+143, b_+145);
  CYC(b_+145, b_+148); A = mem_rd(gb, wFrameCounter);
  CYC(b_+148, b_+150); alu_and(gb, 0x01);
  CYC(b_+150, b_+151); C = A;
L_4c65:
  CYC(b_+151, b_+152); A = C;
  CYC(b_+152, b_+153); alu_add(gb, A);
  CYC(b_+153, b_+154); alu_add(gb, C);
  CYC(b_+154, b_+157); SET_HL(b_+183 /* @warpDestLocations */);
  CYC(b_+157, b_+158); quicksand_add_a_to_hl(gb, b_+158);
  CYC(b_+158, b_+159); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+159, b_+162); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+162, b_+163); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+163, b_+166); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+166, b_+167); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+167, b_+170); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+170, b_+172); A = 0x05;
  CYC(b_+172, b_+175); mem_wr(gb, wWarpTransition, A);
  CYC(b_+175, b_+177); A = 0x03;
  CYC(b_+177, b_+180); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+180, b_+183);
  TAIL(interactionDelete);
}

// Set bit 7 of wPirateSkullRandomNumber if that value and subid match
void s__matchSkullNumberWithSubid_hook(GB *gb) {
  BASE(_matchSkullNumberWithSubid);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wPirateSkullRandomNumber);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+13); return;
}

// Checks for Pirate Skull, Bomb, Used Scent Seed, or Item Drop to pull into the center
void s__findPirateSkullAddress_hook(GB *gb) {
  BASE(_findPirateSkullAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x4d;
  CALL_C(b_+2, s_objectFindSameTypeObjectWithID, SYM(objectFindSameTypeObjectWithID), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+10, b_+12);
  TAIL_S(_moveObjectIfGrounded);
}

void s__findItemDropAddress_hook(GB *gb) {
  BASE(_findItemDropAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); H = 0xd0;
L_4cbf:
  CYC(b_+2, b_+4); L = PART_BASE + OBJ_ID;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_cp(gb, 0x01);
  if (F & FZ) CALL_C_CC(b_+7, s__objectIsPart_hook, SYM(_objectIsPart), b_+10);
  else CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); H = alu_inc8(gb, H);
  CYC(b_+11, b_+12); A = H;
  CYC(b_+12, b_+14); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+14, b_+16); goto L_4cbf; }
  CYC(b_+14, b_+16);
  RET(b_+16); return;
}

// Object is a part
void s__objectIsPart_hook(GB *gb) {
  BASE(_objectIsPart);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = PART_BASE + OBJ_ZH;
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR31;
  s__moveObjectIfGrounded_hook(gb); return; // falls through
}

// Param     hl      Object.zh
// Param     e       Object's yh variable to tell it to move toward quicksand
void s__moveObjectIfGrounded_hook(GB *gb) {
  BASE(_moveObjectIfGrounded);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+1, b_+2); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+2); return; }
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+4); L = alu_dec8(gb, L);
  CYC(b_+4, b_+5); C = mem_rd(gb, HL);
  CYC(b_+5, b_+6); L = alu_dec8(gb, L);
  CYC(b_+6, b_+7); L = alu_dec8(gb, L);
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CYC(b_+8, b_+9); L = E;
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CALL_C(b_+10, s_interactionCheckContainsPoint, SYM(interactionCheckContainsPoint), b_+13);
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  if (!(F & FC)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, s_objectGetPosition, SYM(objectGetPosition), b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, HL, B);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+21); mem_wr(gb, HL, C);
  RET(b_+21); return;
}

void s__findBombOrScentSeedAddress_hook(GB *gb) {
  BASE(_findBombOrScentSeedAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x03;
  CALL_C(b_+2, s_findItemWithID, SYM(findItemWithID), b_+5);
  if (F & FZ) CALL_C_CC(b_+5, s__objectIsItem_hook, SYM(_objectIsItem), b_+8);
  else CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); C = 0x03;
  CALL_C(b_+10, s_findItemWithID_startingAfterH, SYM(findItemWithID_startingAfterH), b_+13);
  if (F & FZ) CALL_C_CC(b_+13, s__objectIsItem_hook, SYM(_objectIsItem), b_+16);
  else CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); C = 0x21;
  CALL_C(b_+18, s_findItemWithID, SYM(findItemWithID), b_+21);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  s__objectIsItem_hook(gb); return; // falls through
}

// Object is an item
void s__objectIsItem_hook(GB *gb) {
  BASE(_objectIsItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x0f;
  CYC(b_+2, b_+4); E = 0x31;
  CYC(b_+4, b_+6);
  TAIL_S(_moveObjectIfGrounded);
}
