#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t breakTileDebris_jumpTable(GB *gb) {
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

static void breakTileDebris_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode0c@doSpecializedInitialization: init for interactions 0 (underwater bush
// breaking) and $0a (shovel debris). Reached only by a static `call` from interactionCode00
// itself; not independently hookable.
static void breakTileDebris_doSpecializedInitialization(GB *gb, uint16_t sp0_) {
  BASE(interactionCode00);
  CYC(b_+126, b_+128); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+130, b_+132);
  } else {
    CYC(b_+130, b_+132);
    CYC(b_+132, b_+134); alu_cp(gb, 0x0a);
    if (F & FZ) {
      CYC(b_+134, b_+135);
    } else {
      CYCT(b_+134, b_+135); ret_effect(gb); return;
    }
    // @interac0A
    CYC(b_+135, b_+138); SET_BC(0xfdc0);
    CALL_C(b_+138, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+141);
    CYC(b_+141, b_+143); E = INTERACTION_BASE + OBJ_DIRECTION;
    CYC(b_+143, b_+144); A = mem_rd(gb, DE);
    CYC(b_+144, b_+147); interactionSetAnimation_hook(gb);
    return;
  }
  // @interac00
  CYC(b_+147, b_+150); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+150, b_+152); alu_and(gb, 0x40);
  if (F & FZ) {
    CYCT(b_+152, b_+154); goto notUnderwater;
  }
  CYC(b_+152, b_+154);
  CYC(b_+154, b_+156); A = 0x0e;
  CYC(b_+156, b_+158); goto stored;
notUnderwater:
  CYC(b_+158, b_+161); A = mem_rd(gb, wGrassAnimationModifier);
  CYC(b_+161, b_+163); alu_and(gb, 0x03);
  CYC(b_+163, b_+165); alu_or(gb, 0x08);
stored:
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  CYC(b_+168, b_+169); E = alu_inc8(gb, E);
  CYC(b_+169, b_+170); mem_wr(gb, DE, A);
  CYC(b_+170, b_+171); ret_effect(gb);
}

void interactionCode00_hook(GB *gb) {
  BASE(interactionCode00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (breakTileDebris_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+81) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x14);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+23); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) {
    CALL_C_CC(b_+23, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+26);
  } else {
    CYC(b_+23, b_+26);
  }
  CYC(b_+26, b_+29); push_effect(gb, b_+29); breakTileDebris_doSpecializedInitialization(gb, sp0_);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+35); SET_HL(b_+55);
  CYC(b_+35, b_+36); breakTileDebris_addDoubleIndex(gb, b_+36);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_rlca(gb);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); E = mem_rd(gb, HL);
  if (!(F & FC)) {
    CALL_C_CC(b_+42, playSound_b00_hook, SYM(playSound_b00), b_+45);
  } else {
    CYC(b_+42, b_+45);
  }
  CYC(b_+45, b_+46); A = E;
  CYC(b_+46, b_+47); push_effect(gb, b_+47);
  do { uint16_t jt_ = (breakTileDebris_jumpTable(gb));
    if (jt_ == SYM(objectSetVisible80) && hook_enabled_at(gb, SYM(objectSetVisible80))) { objectSetVisible80_hook(gb); return; }
    else if (jt_ == SYM(objectSetVisible81) && hook_enabled_at(gb, SYM(objectSetVisible81))) { objectSetVisible81_hook(gb); return; }
    else if (jt_ == SYM(objectSetVisible82) && hook_enabled_at(gb, SYM(objectSetVisible82))) { objectSetVisible82_hook(gb); return; }
    else if (jt_ == SYM(objectSetVisible83) && hook_enabled_at(gb, SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state1:
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+84, b_+86); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYC(b_+86, b_+89);
  } else {
    CYCT(b_+86, b_+89); TAIL(interactionDelete);
  }
  CYC(b_+89, b_+91); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+91, b_+93); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+93, b_+95); goto checkId;
  }
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+98); A = mem_rd(gb, wFrameCounter);
  CYC(b_+98, b_+99); alu_xor(gb, D);
  CYC(b_+99, b_+100); alu_rrca(gb);
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+102, b_+104); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  if (!(F & FC)) {
    CYCT(b_+104, b_+106); goto checkId;
  }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+108); mem_wr(gb, HL, mem_rd(gb, HL) & ~(1 << 7));

checkId:
  CYC(b_+108, b_+110); E = INTERACTION_BASE + OBJ_ID;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+113); alu_cp(gb, 0x0a);
  if (F & FZ) {
    CYC(b_+113, b_+115);
    CYC(b_+115, b_+117); C = 0x60;
    CALL_C(b_+117, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+120);
    CALL_C(b_+120, objectApplySpeed_hook, SYM(objectApplySpeed), b_+123);
  } else {
    CYCT(b_+113, b_+115);
  }
  CYC(b_+123, b_+126); TAIL(interactionAnimate);
}
