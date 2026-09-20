#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7ee5, objectSetCollideRadius_hook, 0x24a1, 0x7ee8);
  CYC(0x7ee8, 0x7eeb); SET_HL(0xd00b);  // ld hl,w1Link.yh
  CYC(0x7eeb, 0x7eec); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ldi a,(hl)
  CYC(0x7eec, 0x7eee); alu_add(gb, 0x05);  // add $05
  CYC(0x7eee, 0x7eef); B = A;  // ld b,a
  CYC(0x7eef, 0x7ef0); L = alu_inc8(gb, L);  // inc l
  CYC(0x7ef0, 0x7ef1); C = mem_rd(gb, HL);  // ld c,(hl)
  CYC(0x7ef1, 0x7ef5); interactionCheckContainsPoint_hook(gb); return;  // jp interactionCheckContainsPoint
}

void interactionCodee6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e51, 0x7e53); E = INTERACTION_BASE + OBJ_STATE;  // ld e,Interaction.state
  CYC(0x7e53, 0x7e54); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x7e54, 0x7e55); push_effect(gb, 0x7e55);
  switch (raftInteraction_jump_table(gb)) {
    case 0x7e5b: goto state0;
    case 0x7e9f: goto state1;
    case 0x3b05: interactionDelete_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x7e5b, 0x7e5d); E = INTERACTION_BASE + OBJ_SUBID;  // ld e,Interaction.subid
  CYC(0x7e5d, 0x7e5e); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x7e5e, 0x7e5f); push_effect(gb, 0x7e5f);
  switch (raftInteraction_jump_table(gb)) {
    case 0x7e65: goto subid0;
    case 0x7e6d: goto subid1;
    case 0x7e87: goto subid2;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid0:
  CYC(0x7e65, 0x7e68); A = mem_rd(gb, 0xc647);  // ld a,(wDimitriState)
  CYC(0x7e68, 0x7e6a); alu_bit(gb, 6, A);  // bit 6,a
  if ((F & FZ)) { CYCT(0x7e6a, 0x7e6d); interactionDelete_hook(gb); return; } CYC(0x7e6a, 0x7e6d);  // jp z,interactionDelete
subid1:
  CYC(0x7e6d, 0x7e6f); A = 0x26;  // ld a,GLOBALFLAG_RAFTON_CHANGED_ROOMS
  CALL_C(0x7e6f, checkGlobalFlag_hook, 0x31f3, 0x7e72);
  if ((F & FZ)) { CYCT(0x7e72, 0x7e75); interactionDelete_hook(gb); return; } CYC(0x7e72, 0x7e75);  // jp z,interactionDelete
  CYC(0x7e75, 0x7e78); A = mem_rd(gb, 0xd101);  // ld a,(w1Companion.id)
  CYC(0x7e78, 0x7e7a); alu_cp(gb, 0x13);  // cp SPECIALOBJECT_RAFT
  if ((F & FZ)) { CYCT(0x7e7a, 0x7e7d); interactionDelete_hook(gb); return; } CYC(0x7e7a, 0x7e7d);  // jp z,interactionDelete
  CYC(0x7e7d, 0x7e7f); C = 0xe6;  // ld c,INTERAC_RAFT
  CALL_C(0x7e7f, objectFindSameTypeObjectWithID_hook, 0x22c8, 0x7e82);
  CYC(0x7e82, 0x7e83); A = H;  // ld a,h
  CYC(0x7e83, 0x7e84); alu_cp(gb, D);  // cp d
  if (!(F & FZ)) { CYCT(0x7e84, 0x7e87); interactionDelete_hook(gb); return; } CYC(0x7e84, 0x7e87);  // jp nz,interactionDelete
subid2:
  CYC(0x7e87, 0x7e88); push_effect(gb, DE);  // push de
  CYC(0x7e88, 0x7e8a); A = 0x3b;  // ld a,UNCMP_GFXH_AGES_3b
  CALL_C(0x7e8a, loadUncompressedGfxHeader_hook, 0x05da, 0x7e8d);
  CYC(0x7e8d, 0x7e8e); SET_DE(pop_effect(gb));  // pop de
  CALL_C(0x7e8e, interactionInitGraphics_hook, 0x15fb, 0x7e91);
  CALL_C(0x7e91, interactionIncState_hook, 0x23e0, 0x7e94);
  CYC(0x7e94, 0x7e96); E = INTERACTION_BASE + OBJ_DIRECTION;  // ld e,Interaction.direction
  CYC(0x7e96, 0x7e97); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x7e97, 0x7e99); alu_and(gb, 0x01);  // and $01
  CALL_C(0x7e99, interactionSetAnimation_hook, 0x262e, 0x7e9c);
  CYC(0x7e9c, 0x7e9f); objectSetVisible83_hook(gb); return;  // jp objectSetVisible83

state1:
  CALL_C(0x7e9f, interactionAnimate_hook, 0x261b, 0x7ea2);
  CYC(0x7ea2, 0x7ea4); A = 0x09;  // ld a,$09
  CYC(0x7ea4, 0x7ea7); push_effect(gb, 0x7ea7); interactionCodee6__checkLinkWithinRange_hook(gb);  // call @checkLinkWithinRange
  if (!(F & FC)) { RET_TAKEN(0x7ea7); return; } CYC(0x7ea7, 0x7ea8);  // ret nc
  CYC(0x7ea8, 0x7eab); A = mem_rd(gb, 0xcc5c);  // ld a,(wLinkInAir)
  CYC(0x7eab, 0x7eac); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x7eac, 0x7eae); goto mountedRaft; } CYC(0x7eac, 0x7eae);  // jr z,@mountedRaft
  CYC(0x7eae, 0x7eb1); SET_HL(0xd00f);  // ld hl,w1Link.zh
  CYC(0x7eb1, 0x7eb2); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7eb2, 0x7eb4); alu_cp(gb, 0xfd);  // cp $fd
  if ((F & FC)) { RET_TAKEN(0x7eb4); return; } CYC(0x7eb4, 0x7eb5);  // ret c
  CYC(0x7eb5, 0x7eb7); L = 0x15;  // ld l,<w1Link.speedZ+1
  CYC(0x7eb7, 0x7eba); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(0x7eb9); return; } CYC(0x7eb9, 0x7eba);  // ret nz
mountedRaft:
  CYC(0x7eba, 0x7ebb); A = D;  // ld a,d
  CYC(0x7ebb, 0x7ebe); mem_wr(gb, 0xcc96, A);  // ld (wLinkRidingObject),a
  CYC(0x7ebe, 0x7ec0); A = 0x05;  // ld a,$05
  CYC(0x7ec0, 0x7ec3); mem_wr(gb, 0xcc6b, A);  // ld (wInstrumentsDisabledCounter),a
  CYC(0x7ec3, 0x7ec6); push_effect(gb, 0x7ec6); interactionCodee6__checkLinkWithinRange_hook(gb);  // call @checkLinkWithinRange
  if (!(F & FC)) { RET_TAKEN(0x7ec6); return; } CYC(0x7ec6, 0x7ec7);  // ret nc
  CYC(0x7ec7, 0x7eca); A = mem_rd(gb, 0xd001);  // ld a,(w1Link.id)
  CYC(0x7eca, 0x7ecb); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x7ecb, 0x7ecd); goto afterOverride; } CYC(0x7ecb, 0x7ecd);  // jr z,++
  CYC(0x7ecd, 0x7ece); alu_xor(gb, A);  // xor a
  CALL_C(0x7ece, setLinkIDOverride_hook, 0x2acf, 0x7ed1);
afterOverride:
  CYC(0x7ed1, 0x7ed4); SET_HL(0xd100);  // ld hl,w1Companion.enabled
  CYC(0x7ed4, 0x7ed6); mem_wr(gb, HL, 0x03);  // ld (hl),$03
  CYC(0x7ed6, 0x7ed7); L = alu_inc8(gb, L);  // inc l
  CYC(0x7ed7, 0x7ed9); mem_wr(gb, HL, 0x13);  // ld (hl),SPECIALOBJECT_RAFT
  CYC(0x7ed9, 0x7edb); E = INTERACTION_BASE + OBJ_DIRECTION;  // ld e,Interaction.direction
  CYC(0x7edb, 0x7edd); L = 0x08;  // ld l,<w1Link.direction
  CYC(0x7edd, 0x7ede); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x7ede, 0x7edf); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CALL_C(0x7edf, objectCopyPosition_hook, 0x2242, 0x7ee2);
  CYC(0x7ee2, 0x7ee5); interactionIncState_hook(gb); return;  // jp interactionIncState
}
