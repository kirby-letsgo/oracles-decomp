#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode14), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode14), (from), (to), true)

#define pushableTilePropertiesTable_bank08 SYM(pushableTilePropertiesTable)

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
  BASE(interactionCode14);
  CYC(b_+154, b_+157); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+157, b_+159); alu_and(gb, 0x18);
  if (F & FZ) {
    CYCT(b_+159, b_+160); ret_effect(gb); return;
  }
  CYC(b_+159, b_+160);
  CALL_C(b_+160, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+163);
  CYC(b_+163, b_+164); C = A;
  CYC(b_+164, b_+166); B = wRoomLayout >> 8;
  CYC(b_+166, b_+167); A = mem_rd(gb, BC);
  CYC(b_+167, b_+169); alu_cp(gb, TILEINDEX_BUTTON);
  CYC(b_+169, b_+171); A = 0xfe;
  if (F & FZ) {
    CYCT(b_+171, b_+173);
  } else {
    CYC(b_+171, b_+173);
    CYC(b_+173, b_+174); alu_xor(gb, A);
  }
  CYC(b_+174, b_+176); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+176, b_+177); mem_wr(gb, DE, A);
  CYC(b_+177, b_+178); ret_effect(gb);
}

// interactionCode14@replaceTileUnderneathBlock: restore the ground tile at var30 (from the room
// layout buffer if non-solid, else var32). Tails into setTile, whose ret pops our return address.
static void pushblock_replaceTileUnderneathBlock(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  CYC(b_+181, b_+182); C = A;
  CALL_C(b_+182, getTileIndexFromRoomLayoutBuffer_paramC_hook, SYM(getTileIndexFromRoomLayoutBuffer_paramC), b_+185);
  if (!(F & FC)) {
    CYCT(b_+185, b_+188); setTile_hook(gb); return;
  }
  CYC(b_+185, b_+188);
  CYC(b_+188, b_+190); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+190, b_+191); A = mem_rd(gb, DE);
  CYC(b_+191, b_+194); setTile_hook(gb);
}

// interactionCode14@checkRotatingCubePermitsPushing: carry set means this block may not be pushed.
static void pushblock_checkRotatingCubePermitsPushing(GB *gb) {
  BASE(interactionCode14);
  CYC(b_+194, b_+197); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+197, b_+198); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+198, b_+199); ret_effect(gb); return;
  }
  CYC(b_+198, b_+199);
  CYC(b_+199, b_+202); A = mem_rd(gb, wRotatingCubeColor);
  CYC(b_+202, b_+204); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+204, b_+206); goto deny;
  }
  CYC(b_+204, b_+206);
  CYC(b_+206, b_+208); alu_and(gb, 0x7f);
  CYC(b_+208, b_+209); B = A;
  CYC(b_+209, b_+211); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(b_+211, b_+212); A = mem_rd(gb, DE);
  CYC(b_+212, b_+214); alu_sub(gb, TILEINDEX_RED_PUSHABLE_BLOCK);
  CYC(b_+214, b_+215); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+215, b_+216); ret_effect(gb); return;
  }
  CYC(b_+215, b_+216);
deny:
  CYC(b_+216, b_+217); alu_scf(gb);
  CYC(b_+217, b_+218); ret_effect(gb);
}

// interactionCode14@loadPushableTileProperties: look up var31 in the pushable tile table for the
// active collision set and copy its 4 bytes into var31-var34.
static void pushblock_loadPushableTileProperties(GB *gb) {
  BASE(interactionCode14);
  CYC(b_+218, b_+221); A = mem_rd(gb, wActiveCollisions);
  CYC(b_+221, b_+224); SET_HL(pushableTilePropertiesTable_bank08);
  CYC(b_+224, b_+225); push_effect(gb, b_+225); pushblock_addAToHl(gb);
  CYC(b_+225, b_+226); A = mem_rd(gb, HL);
  CYC(b_+226, b_+227); push_effect(gb, b_+227); pushblock_addAToHl(gb);
  CYC(b_+227, b_+229); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(b_+229, b_+230); A = mem_rd(gb, DE);
  CYC(b_+230, b_+231); B = A;
search:
  CYC(b_+231, b_+232); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+232, b_+233); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+233, b_+234); ret_effect(gb); return;
  }
  CYC(b_+233, b_+234);
  CYC(b_+234, b_+235); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+235, b_+237); goto match;
  }
  CYC(b_+235, b_+237);
  CYC(b_+237, b_+238); SET_HL(HL + 1);
  CYC(b_+238, b_+239); SET_HL(HL + 1);
  CYC(b_+239, b_+240); SET_HL(HL + 1);
  CYC(b_+240, b_+242); goto search;
match:
  CYC(b_+242, b_+243); mem_wr(gb, DE, A);
  CYC(b_+243, b_+244); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+244, b_+245); E = alu_inc8(gb, E);
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CYC(b_+246, b_+247); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+247, b_+248); E = alu_inc8(gb, E);
  CYC(b_+248, b_+249); mem_wr(gb, DE, A);
  CYC(b_+249, b_+250); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+250, b_+251); E = alu_inc8(gb, E);
  CYC(b_+251, b_+252); mem_wr(gb, DE, A);
  CYC(b_+252, SYM(pushableTilePropertiesTable)); ret_effect(gb);
}

// interactionCode14@state1 (also reached by fallthrough from state0): move the block, and once
// counter1 expires (@func_449d) settle it and delete self.
static void pushblock_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+96, b_+99); push_effect(gb, b_+99); pushblock_updateZPositionForButton(gb, sp0_);
  CALL_C(b_+99, objectApplySpeed_hook, SYM(objectApplySpeed), b_+102);
  CALL_C(b_+102, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+105);
  CALL_C(b_+105, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+108);
  if (!(F & FZ)) {
    CYCT(b_+108, b_+109); ret_effect(gb); return;
  }
  CYC(b_+108, b_+109);
  CALL_C(b_+109, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+112);
  if (F & FC) {
    CYCT(b_+112, b_+115); interactionDelete_hook(gb); return;
  }
  CYC(b_+112, b_+115);
  CALL_C(b_+115, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+118);
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+120, b_+121); mem_wr(gb, DE, A);
  CYC(b_+121, b_+123); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+125); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+125, b_+127); goto checkSound;
  }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+128); B = A;
  CYC(b_+128, b_+130); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); C = A;
  CYC(b_+132, b_+133); A = B;
  CALL_C(b_+133, setTile_hook, SYM(setTile), b_+136);
checkSound:
  CYC(b_+136, b_+138); E = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+138, b_+139); A = mem_rd(gb, DE);
  CYC(b_+139, b_+140); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+140, b_+142); goto delete;
  }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+146); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+146, b_+148); A = 0x4d;
  CALL_C(b_+148, playSound_b00_hook, SYM(playSound_b00), b_+151);
delete:
  CYC(b_+151, b_+154); interactionDelete_hook(gb);
}

// interactionCode14@state0: block just pushed. Falls through into state1.
static void pushblock_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode14);
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR30;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); B = wRoomLayout >> 8;
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_VAR31;
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CALL_C(b_+24, objectMimicBgTile_hook, SYM(objectMimicBgTile), b_+27);
  CYC(b_+27, b_+30); push_effect(gb, b_+30); pushblock_checkRotatingCubePermitsPushing(gb);
  if (F & FC) {
    CYCT(b_+30, b_+33); interactionDelete_hook(gb); return;
  }
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+35); A = 0x06;
  CALL_C(b_+35, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+38);
  CYC(b_+38, b_+41); push_effect(gb, b_+41); pushblock_loadPushableTileProperties(gb);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+44, b_+46); alu_bit(gb, 2, mem_rd(gb, HL));
  CYC(b_+46, b_+48); A = 0x01;
  if (!(F & FZ)) {
    CALL_C_CC(b_+48, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+51);
  } else {
    CYC(b_+48, b_+51);
  }
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+55); SET_BC((SYM(setTileWithoutGfxReload) + 4));
  CYC(b_+55, b_+58); A = mem_rd(gb, wBraceletLevel);
  CYC(b_+58, b_+60); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+60, b_+62); goto setSpeed;
  }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_VAR34;
  CYC(b_+64, b_+66); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+66, b_+68); goto setSpeed;
  }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+71); SET_BC((SYM(findByteInGroupTable) + 6));
setSpeed:
  CYC(b_+71, b_+73); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+73, b_+74); mem_wr(gb, HL, B);
  CYC(b_+74, b_+76); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+76, b_+77); mem_wr(gb, HL, C);
  CYC(b_+77, b_+79); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+79, b_+80); A = mem_rd(gb, HL);
  CYC(b_+80, b_+82); alu_or(gb, 0x80);
  CYC(b_+82, b_+85); mem_wr(gb, wBlockPushAngle, A);
  CYC(b_+85, b_+88); push_effect(gb, b_+88); pushblock_replaceTileUnderneathBlock(gb, sp0_);
  CALL_C(b_+88, objectSetVisible82_hook, SYM(objectSetVisible82), b_+91);
  CYC(b_+91, b_+93); A = 0x71;
  CALL_C(b_+93, playSound_b00_hook, SYM(playSound_b00), b_+96);
  pushblock_state1(gb, sp0_);
}

void interactionCode14_hook(GB *gb) {
  BASE(interactionCode14);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pushblock_jumpTable(gb));
    if (jt_ == b_+8) { pushblock_state0(gb, sp0_); return; }
    else if (jt_ == b_+96) { pushblock_state1(gb, sp0_); return; }
    else { HANDOFF(HL); }
  } while (0);
}
