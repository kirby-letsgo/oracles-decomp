#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/ages/interactions/raft.s (INTERAC_RAFT), bank $10.

static uint16_t raftInteraction_jump_table(GB *gb) {
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

// 10:7ee5
void interactionCodee6__checkLinkWithinRange_hook(GB *gb) {
  BASE(interactionCodee6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+148, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+151);
  CYC(b_+151, b_+154); SET_HL(w1Link_yh);  // ld hl,w1Link.yh
  CYC(b_+154, b_+155); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(b_+155, b_+157); alu_add(gb, 0x05);  // add $05
  CYC(b_+157, b_+158); B = A;  // ld b,a
  CYC(b_+158, b_+159); L = alu_inc8(gb, L);  // inc l
  CYC(b_+159, b_+160); C = mem_rd(gb, HL);  // ld c,(hl)
  CYC(b_+160, b_+164); TAIL(interactionCheckContainsPoint);  // jp interactionCheckContainsPoint
}

void interactionCodee6_hook(GB *gb) {
  BASE(interactionCodee6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;  // ld e,Interaction.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (raftInteraction_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+78) { goto state1; }
    else if (jt_ == SYM(interactionDelete) && hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;  // ld e,Interaction.subid
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (raftInteraction_jump_table(gb));
    if (jt_ == b_+20) { goto subid0; }
    else if (jt_ == b_+28) { goto subid1; }
    else if (jt_ == b_+54) { goto subid2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

subid0:
  CYC(b_+20, b_+23); A = mem_rd(gb, wDimitriState);  // ld a,(wDimitriState)
  CYC(b_+23, b_+25); alu_bit(gb, 6, A);  // bit 6,a
  if ((F & FZ)) { CYCT(b_+25, b_+28); interactionDelete_hook(gb); return; } CYC(b_+25, b_+28);  // jp z,interactionDelete
subid1:
  CYC(b_+28, b_+30); A = 0x26;  // ld a,GLOBALFLAG_RAFTON_CHANGED_ROOMS
  CALL_C(b_+30, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+33);
  if ((F & FZ)) { CYCT(b_+33, b_+36); interactionDelete_hook(gb); return; } CYC(b_+33, b_+36);  // jp z,interactionDelete
  CYC(b_+36, b_+39); A = mem_rd(gb, w1Companion_id);  // ld a,(w1Companion.id)
  CYC(b_+39, b_+41); alu_cp(gb, 0x13);  // cp SPECIALOBJECT_RAFT
  if ((F & FZ)) { CYCT(b_+41, b_+44); interactionDelete_hook(gb); return; } CYC(b_+41, b_+44);  // jp z,interactionDelete
  CYC(b_+44, b_+46); C = 0xe6;  // ld c,INTERAC_RAFT
  CALL_C(b_+46, objectFindSameTypeObjectWithID_hook, SYM(objectFindSameTypeObjectWithID), b_+49);
  CYC(b_+49, b_+50); A = H;  // ld a,h
  CYC(b_+50, b_+51); alu_cp(gb, D);  // cp d
  if (!(F & FZ)) { CYCT(b_+51, b_+54); interactionDelete_hook(gb); return; } CYC(b_+51, b_+54);  // jp nz,interactionDelete
subid2:
  CYC(b_+54, b_+55); push_effect(gb, DE);  // push de
  CYC(b_+55, b_+57); A = 0x3b;  // ld a,UNCMP_GFXH_AGES_3b
  CALL_C(b_+57, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+60);
  CYC(b_+60, b_+61); SET_DE(pop_effect(gb));  // pop de
  CALL_C(b_+61, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+64);
  CALL_C(b_+64, interactionIncState_hook, SYM(interactionIncState), b_+67);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_DIRECTION;  // ld e,Interaction.direction
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+70, b_+72); alu_and(gb, 0x01);  // and $01
  CALL_C(b_+72, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+75);
  CYC(b_+75, b_+78); TAIL(objectSetVisible83);  // jp objectSetVisible83

state1:
  CALL_C(b_+78, interactionAnimate_hook, SYM(interactionAnimate), b_+81);
  CYC(b_+81, b_+83); A = 0x09;  // ld a,$09
  CYC(b_+83, b_+86); push_effect(gb, b_+86); interactionCodee6__checkLinkWithinRange_hook(gb);  // call @checkLinkWithinRange
  if (!(F & FC)) { RET_TAKEN(b_+86); return; } CYC(b_+86, b_+87);  // ret nc
  CYC(b_+87, b_+90); A = mem_rd(gb, wLinkInAir);  // ld a,(wLinkInAir)
  CYC(b_+90, b_+91); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+91, b_+93); goto mountedRaft; } CYC(b_+91, b_+93);  // jr z,@mountedRaft
  CYC(b_+93, b_+96); SET_HL(w1Link_zh);  // ld hl,w1Link.zh
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+97, b_+99); alu_cp(gb, 0xfd);  // cp $fd
  if ((F & FC)) { RET_TAKEN(b_+99); return; } CYC(b_+99, b_+100);  // ret c
  CYC(b_+100, b_+102); L = 0x15;  // ld l,<w1Link.speedZ+1
  CYC(b_+102, b_+105); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(b_+104); return; } CYC(b_+104, b_+105);  // ret nz
mountedRaft:
  CYC(b_+105, b_+106); A = D;  // ld a,d
  CYC(b_+106, b_+109); mem_wr(gb, wLinkRidingObject, A);  // ld (wLinkRidingObject),a
  CYC(b_+109, b_+111); A = 0x05;  // ld a,$05
  CYC(b_+111, b_+114); mem_wr(gb, wInstrumentsDisabledCounter, A);  // ld (wInstrumentsDisabledCounter),a
  CYC(b_+114, b_+117); push_effect(gb, b_+117); interactionCodee6__checkLinkWithinRange_hook(gb);  // call @checkLinkWithinRange
  if (!(F & FC)) { RET_TAKEN(b_+117); return; } CYC(b_+117, b_+118);  // ret nc
  CYC(b_+118, b_+121); A = mem_rd(gb, w1Link_id);  // ld a,(w1Link.id)
  CYC(b_+121, b_+122); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+122, b_+124); goto afterOverride; } CYC(b_+122, b_+124);  // jr z,++
  CYC(b_+124, b_+125); alu_xor(gb, A);  // xor a
  CALL_C(b_+125, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+128);
afterOverride:
  CYC(b_+128, b_+131); SET_HL(w1Companion);  // ld hl,w1Companion.enabled
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x03);  // ld (hl),$03
  CYC(b_+133, b_+134); L = alu_inc8(gb, L);  // inc l
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x13);  // ld (hl),SPECIALOBJECT_RAFT
  CYC(b_+136, b_+138); E = INTERACTION_BASE + OBJ_DIRECTION;  // ld e,Interaction.direction
  CYC(b_+138, b_+140); L = 0x08;  // ld l,<w1Link.direction
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+141, b_+142); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CALL_C(b_+142, objectCopyPosition_hook, SYM(objectCopyPosition), b_+145);
  CYC(b_+145, b_+148); TAIL(interactionIncState);  // jp interactionIncState
}
