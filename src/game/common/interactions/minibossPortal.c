#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode7e_jump_table(GB *gb) {
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

static void interactionCode7e_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void interactionCode7e_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_MINIBOSS_PORTAL
void interactionCode7e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4452, 0x4454); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4454, 0x4455); A = mem_rd(gb, DE);
  {
    CYC(0x4455, 0x4456); push_effect(gb, 0x4456);
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == 0x445a) goto subid00;
    goto subid01;
  }

subid00:
  CYC(0x445a, 0x445c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x445c, 0x445d); A = mem_rd(gb, DE);
  {
    CYC(0x445d, 0x445e); push_effect(gb, 0x445e);
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == 0x4466) goto minibossState0;
    if (target == 0x4498) goto state1;
    if (target == 0x44cc) goto state2;
    goto minibossState3;
  }

minibossState0:
  CYC(0x4466, 0x4469); A = W8(wDungeonIndex);
  CYC(0x4469, 0x446c); SET_HL(0x4507); // interactionCode7e@dungeonRoomTable
  CYC(0x446c, 0x446d); interactionCode7e_addDoubleIndexToHl_from_rst(gb, 0x446d);
  CYC(0x446d, 0x446e); C = mem_rd(gb, HL);
  CYC(0x446e, 0x4471); A = W8(wActiveGroup);
  CYC(0x4471, 0x4474); SET_HL(0x09cc); // flagLocationGroupTable
  CYC(0x4474, 0x4475); interactionCode7e_addAToHl_from_rst(gb, 0x4475);
  CYC(0x4475, 0x4476); H = mem_rd(gb, HL);
  CYC(0x4476, 0x4477); L = C;
  CYC(0x4477, 0x4478); A = mem_rd(gb, HL);
  CYC(0x4478, 0x447a); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x447a, 0x447d); interactionDelete_hook(gb); return; } // jp z
  CYC(0x447a, 0x447d);
  CYC(0x447d, 0x447f); C = 0x57;
  CALL_C(0x447f, objectSetShortPosition_hook, 0x20c3, 0x4482);

commonState0:
  CALL_C(0x4482, interactionInitGraphics_hook, 0x15fb, 0x4485);
  CYC(0x4485, 0x4487); A = 0x03;
  CALL_C(0x4487, objectSetCollideRadius_hook, 0x24a1, 0x448a);
  CALL_C(0x448a, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x448d);
  CYC(0x448d, 0x448f); A = 0x01;
  if (!(F & FC)) { CYCT(0x448f, 0x4491); goto afterIncA; } // jr nc
  CYC(0x448f, 0x4491);
  CYC(0x4491, 0x4492); A = alu_inc8(gb, A);

afterIncA:
  CYC(0x4492, 0x4494); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4494, 0x4495); mem_wr(gb, DE, A);
  CYC(0x4495, 0x4498); objectSetVisible83_hook(gb); return; // jp

state1:
  CALL_C(0x4498, interactionAnimate_hook, 0x261b, 0x449b);
  CALL_C(0x449b, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x449e);
  if (!(F & FC)) { RET_TAKEN(0x449e); return; } // ret nc
  CYC(0x449e, 0x449f);
  CYC(0x449f, 0x44a2); A = W8(w1Link_id);
  CYC(0x44a2, 0x44a3); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(0x44a3, checkLinkCollisionsEnabled_hook, 0x1d32, 0x44a6); else CYC(0x44a3, 0x44a6); // call z
  if (!(F & FC)) { RET_TAKEN(0x44a6); return; } // ret nc
  CYC(0x44a6, 0x44a7);
  CALL_C(0x44a7, resetLinkInvincibility_hook, 0x2ba9, 0x44aa);
  CYC(0x44aa, 0x44ac); A = 0x03;
  CYC(0x44ac, 0x44ae); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x44ae, 0x44af); mem_wr(gb, DE, A);
  CYC(0x44af, 0x44b2); W8(wLinkCanPassNpcs) = A;
  CYC(0x44b2, 0x44b4); A = 0x30;
  CYC(0x44b4, 0x44b6); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x44b6, 0x44b7); mem_wr(gb, DE, A);
  CALL_C(0x44b7, setLinkForceStateToState08_hook, 0x2aad, 0x44ba);
  CYC(0x44ba, 0x44bd); SET_HL(w1Link_visible);
  CYC(0x44bd, 0x44bf); mem_wr(gb, HL, 0x82);
  CALL_C(0x44bf, objectCopyPosition_hook, 0x2242, 0x44c2);
  CYC(0x44c2, 0x44c4); A = 0x01;
  CYC(0x44c4, 0x44c7); W8(wDisabledObjects) = A;
  CYC(0x44c7, 0x44c9); A = 0x8d; // SND_TELEPORT
  CYC(0x44c9, 0x44cc); playSound_b00_hook(gb); return; // jp

state2:
  CALL_C(0x44cc, interactionAnimate_hook, 0x261b, 0x44cf);
  CALL_C(0x44cf, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x44d2);
  if (F & FC) { RET_TAKEN(0x44d2); return; } // ret c
  CYC(0x44d2, 0x44d3);
  CYC(0x44d3, 0x44d5); A = 0x01;
  CYC(0x44d5, 0x44d7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x44d7, 0x44d8); mem_wr(gb, DE, A);
  RET(0x44d8); return; // ret

minibossState3:
  CYC(0x44d9, 0x44dc); SET_HL(w1Link);
  CALL_C(0x44dc, objectCopyPosition_hook, 0x2242, 0x44df);
  CYC(0x44df, 0x44e2); push_effect(gb, 0x44e2); goto spinLink;
afterSpin1:
  if (!(F & FZ)) { RET_TAKEN(0x44e2); return; } // ret nz
  CYC(0x44e2, 0x44e3);
  CYC(0x44e3, 0x44e6); A = W8(wDungeonIndex);
  CYC(0x44e6, 0x44e9); SET_HL(0x4507); // interactionCode7e@dungeonRoomTable
  CYC(0x44e9, 0x44ea); interactionCode7e_addDoubleIndexToHl_from_rst(gb, 0x44ea);
  CYC(0x44ea, 0x44eb); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x44eb, 0x44ec); C = mem_rd(gb, HL);
  CYC(0x44ec, 0x44ed); B = A;
  CYC(0x44ed, 0x44f0); SET_HL(wWarpDestGroup);
  CYC(0x44f0, 0x44f3); A = W8(wActiveGroup);
  CYC(0x44f3, 0x44f5); alu_or(gb, 0x80);
  CYC(0x44f5, 0x44f6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x44f6, 0x44f9); A = W8(wActiveRoom);
  CYC(0x44f9, 0x44fa); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x44fa, 0x44fc); goto afterRoomCheck; } // jr nz
  CYC(0x44fa, 0x44fc);
  CYC(0x44fc, 0x44fd); B = C;

afterRoomCheck:
  CYC(0x44fd, 0x44fe); A = B;
  CYC(0x44fe, 0x44ff); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x44ff, 0x4500); alu_xor(gb, A);
  CYC(0x4500, 0x4501); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4501, 0x4503); mem_wr(gb, HL, 0x57);
  CYC(0x4503, 0x4504); L = alu_inc8(gb, L);
  CYC(0x4504, 0x4506); mem_wr(gb, HL, 0x03);
  RET(0x4506); return; // ret

subid01:
  CYC(0x4536, 0x4538); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4538, 0x4539); A = mem_rd(gb, DE);
  {
    CYC(0x4539, 0x453a); push_effect(gb, 0x453a);
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == 0x4542) goto herosCaveState0;
    if (target == 0x4498) goto state1;
    if (target == 0x44cc) goto state2;
    goto herosCaveState3;
  }

herosCaveState0:
  CALL_C(0x4542, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4545);
  CYC(0x4545, 0x4547); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4547, 0x4548); A = mem_rd(gb, DE);
  CYC(0x4548, 0x454a); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x454a, 0x454b); mem_wr(gb, DE, A);
  CYC(0x454b, 0x454d); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x454d, 0x454f); goto afterRoomFlagCheck; } // jr z
  CYC(0x454d, 0x454f);
  CALL_C(0x454f, getThisRoomFlags_hook, 0x197d, 0x4552);
  CYC(0x4552, 0x4554); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(0x4554); return; } // ret z
  CYC(0x4554, 0x4555);

afterRoomFlagCheck:
  CYC(0x4555, 0x4556); H = D;
  CYC(0x4556, 0x4558); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4558, 0x4559); L = E;
  CYC(0x4559, 0x455a); A = mem_rd(gb, DE);
  CALL_C(0x455a, setShortPosition_hook, 0x20b8, 0x455d);
  CYC(0x455d, 0x4560); goto commonState0; // jp

herosCaveState3:
  CYC(0x4560, 0x4563); push_effect(gb, 0x4563); goto spinLink;
afterSpin2:
  if (!(F & FZ)) { RET_TAKEN(0x4563); return; } // ret nz
  CYC(0x4563, 0x4564);
  CYC(0x4564, 0x4566); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4566, 0x4567); A = mem_rd(gb, DE);
  CYC(0x4567, 0x4569); alu_and(gb, 0x0f);
  CYC(0x4569, 0x456c); push_effect(gb, 0x456c); goto initHerosCaveWarp;
afterInitWarp:
  CYC(0x456c, 0x456e); A = 0x84;
  CYC(0x456e, 0x4571); W8(wWarpDestGroup) = A;
  RET(0x4571); return; // ret

// interactionCode7e@spinLink: reached by two genuine calls, from @minibossState3's return
// address 0x44e2 above and @herosCaveState3's return address 0x4563 above; never separately
// hooked. Has both a literal ret exit and a tail-jump-into-external-hook exit, so every exit
// needs both callers' resume checks.
spinLink:
  CALL_C(0x4519, resetLinkInvincibility_hook, 0x2ba9, 0x451c);
  CALL_C(0x451c, interactionAnimate_hook, 0x261b, 0x451f);
  CYC(0x451f, 0x4522); A = W8(wLinkDeathTrigger);
  CYC(0x4522, 0x4523); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(0x4523);
    if (gb->pc == 0x44e2 && gb->sp == sp0_) goto afterSpin1;
    if (gb->pc == 0x4563 && gb->sp == sp0_) goto afterSpin2;
    return;
  } // ret nz
  CYC(0x4523, 0x4524);
  CYC(0x4524, 0x4527); A = W8(wFrameCounter);
  CYC(0x4527, 0x4529); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x4529, 0x452b); goto afterDirUpdate; } // jr nz
  CYC(0x4529, 0x452b);
  CYC(0x452b, 0x452e); SET_HL(w1Link_direction);
  CYC(0x452e, 0x452f); A = mem_rd(gb, HL);
  CYC(0x452f, 0x4530); A = alu_inc8(gb, A);
  CYC(0x4530, 0x4532); alu_and(gb, 0x03);
  CYC(0x4532, 0x4533); mem_wr(gb, HL, A);

afterDirUpdate:
  CYC(0x4533, 0x4536); interactionDecCounter1_hook(gb); // jp
  if (gb->pc == 0x44e2 && gb->sp == sp0_) goto afterSpin1;
  if (gb->pc == 0x4563 && gb->sp == sp0_) goto afterSpin2;
  return;

// interactionCode7e@initHerosCaveWarp: reached by one genuine call, from @herosCaveState3's
// return address 0x456c above; never separately hooked.
initHerosCaveWarp:
  CYC(0x4572, 0x4575); SET_HL(0x458d); // interactionCode7e@herosCaveWarps
  CYC(0x4575, 0x4576); interactionCode7e_addDoubleIndexToHl_from_rst(gb, 0x4576);
  CYC(0x4576, 0x4577); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4577, 0x457a); W8(wWarpDestRoom) = A;
  CYC(0x457a, 0x457b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x457b, 0x457e); W8(wWarpDestPos) = A;
  CYC(0x457e, 0x4580); A = 0x85;
  CYC(0x4580, 0x4583); W8(wWarpDestGroup) = A;
  CYC(0x4583, 0x4584); alu_xor(gb, A);
  CYC(0x4584, 0x4587); W8(wWarpTransition) = A;
  CYC(0x4587, 0x4589); A = 0x03;
  CYC(0x4589, 0x458c); W8(wWarpTransition2) = A;
  RET(0x458c);
  if (gb->pc == 0x456c && gb->sp == sp0_) goto afterInitWarp;
  return;
}
