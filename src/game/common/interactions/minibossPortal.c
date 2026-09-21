#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode7e);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+8) goto subid00;
    goto subid01;
  }

subid00:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  {
    CYC(b_+11, b_+12); push_effect(gb, b_+12);
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+20) goto minibossState0;
    if (target == b_+70) goto state1;
    if (target == b_+122) goto state2;
    goto minibossState3;
  }

minibossState0:
  CYC(b_+20, b_+23); A = W8(wDungeonIndex);
  CYC(b_+23, b_+26); SET_HL(b_+181); // interactionCode7e@dungeonRoomTable
  CYC(b_+26, b_+27); interactionCode7e_addDoubleIndexToHl_from_rst(gb, b_+27);
  CYC(b_+27, b_+28); C = mem_rd(gb, HL);
  CYC(b_+28, b_+31); A = W8(wActiveGroup);
  CYC(b_+31, b_+34); SET_HL(SYM(flagLocationGroupTable)); // flagLocationGroupTable
  CYC(b_+34, b_+35); interactionCode7e_addAToHl_from_rst(gb, b_+35);
  CYC(b_+35, b_+36); H = mem_rd(gb, HL);
  CYC(b_+36, b_+37); L = C;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+40); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(b_+40, b_+43); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+40, b_+43);
  CYC(b_+43, b_+45); C = 0x57;
  CALL_C(b_+45, objectSetShortPosition_hook, SYM(objectSetShortPosition), b_+48);

commonState0:
  CALL_C(b_+48, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+51);
  CYC(b_+51, b_+53); A = 0x03;
  CALL_C(b_+53, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+56);
  CALL_C(b_+56, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+59);
  CYC(b_+59, b_+61); A = 0x01;
  if (!(F & FC)) { CYCT(b_+61, b_+63); goto afterIncA; } // jr nc
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+64); A = alu_inc8(gb, A);

afterIncA:
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+70); TAIL(objectSetVisible83); // jp

state1:
  CALL_C(b_+70, interactionAnimate_hook, SYM(interactionAnimate), b_+73);
  CALL_C(b_+73, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+76);
  if (!(F & FC)) { RET_TAKEN(b_+76); return; } // ret nc
  CYC(b_+76, b_+77);
  CYC(b_+77, b_+80); A = W8(w1Link_id);
  CYC(b_+80, b_+81); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+81, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+84); else CYC(b_+81, b_+84); // call z
  if (!(F & FC)) { RET_TAKEN(b_+84); return; } // ret nc
  CYC(b_+84, b_+85);
  CALL_C(b_+85, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+88);
  CYC(b_+88, b_+90); A = 0x03;
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+96); W8(wLinkCanPassNpcs) = A;
  CYC(b_+96, b_+98); A = 0x30;
  CYC(b_+98, b_+100); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CALL_C(b_+101, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+104);
  CYC(b_+104, b_+107); SET_HL(w1Link_visible);
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0x82);
  CALL_C(b_+109, objectCopyPosition_hook, SYM(objectCopyPosition), b_+112);
  CYC(b_+112, b_+114); A = 0x01;
  CYC(b_+114, b_+117); W8(wDisabledObjects) = A;
  CYC(b_+117, b_+119); A = 0x8d; // SND_TELEPORT
  CYC(b_+119, b_+122); TAIL(playSound_b00); // jp

state2:
  CALL_C(b_+122, interactionAnimate_hook, SYM(interactionAnimate), b_+125);
  CALL_C(b_+125, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+128);
  if (F & FC) { RET_TAKEN(b_+128); return; } // ret c
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+131); A = 0x01;
  CYC(b_+131, b_+133); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);
  RET(b_+134); return; // ret

minibossState3:
  CYC(b_+135, b_+138); SET_HL(w1Link);
  CALL_C(b_+138, objectCopyPosition_hook, SYM(objectCopyPosition), b_+141);
  CYC(b_+141, b_+144); push_effect(gb, b_+144); goto spinLink;
afterSpin1:
  if (!(F & FZ)) { RET_TAKEN(b_+144); return; } // ret nz
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+148); A = W8(wDungeonIndex);
  CYC(b_+148, b_+151); SET_HL(b_+181); // interactionCode7e@dungeonRoomTable
  CYC(b_+151, b_+152); interactionCode7e_addDoubleIndexToHl_from_rst(gb, b_+152);
  CYC(b_+152, b_+153); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+153, b_+154); C = mem_rd(gb, HL);
  CYC(b_+154, b_+155); B = A;
  CYC(b_+155, b_+158); SET_HL(wWarpDestGroup);
  CYC(b_+158, b_+161); A = W8(wActiveGroup);
  CYC(b_+161, b_+163); alu_or(gb, 0x80);
  CYC(b_+163, b_+164); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+164, b_+167); A = W8(wActiveRoom);
  CYC(b_+167, b_+168); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+168, b_+170); goto afterRoomCheck; } // jr nz
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+171); B = C;

afterRoomCheck:
  CYC(b_+171, b_+172); A = B;
  CYC(b_+172, b_+173); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+173, b_+174); alu_xor(gb, A);
  CYC(b_+174, b_+175); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0x57);
  CYC(b_+177, b_+178); L = alu_inc8(gb, L);
  CYC(b_+178, b_+180); mem_wr(gb, HL, 0x03);
  RET(b_+180); return; // ret

subid01:
  CYC(b_+228, b_+230); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+230, b_+231); A = mem_rd(gb, DE);
  {
    CYC(b_+231, b_+232); push_effect(gb, b_+232);
    uint16_t target = interactionCode7e_jump_table(gb);
    if (target == b_+240) goto herosCaveState0;
    if (target == b_+70) goto state1;
    if (target == b_+122) goto state2;
    goto herosCaveState3;
  }

herosCaveState0:
  CALL_C(b_+240, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+243);
  CYC(b_+243, b_+245); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+245, b_+246); A = mem_rd(gb, DE);
  CYC(b_+246, b_+248); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+248, b_+249); mem_wr(gb, DE, A);
  CYC(b_+249, b_+251); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+251, b_+253); goto afterRoomFlagCheck; } // jr z
  CYC(b_+251, b_+253);
  CALL_C(b_+253, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+256);
  CYC(b_+256, b_+258); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(b_+258); return; } // ret z
  CYC(b_+258, b_+259);

afterRoomFlagCheck:
  CYC(b_+259, b_+260); H = D;
  CYC(b_+260, b_+262); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+262, b_+263); L = E;
  CYC(b_+263, b_+264); A = mem_rd(gb, DE);
  CALL_C(b_+264, setShortPosition_hook, SYM(setShortPosition), b_+267);
  CYC(b_+267, b_+270); goto commonState0; // jp

herosCaveState3:
  CYC(b_+270, b_+273); push_effect(gb, b_+273); goto spinLink;
afterSpin2:
  if (!(F & FZ)) { RET_TAKEN(b_+273); return; } // ret nz
  CYC(b_+273, b_+274);
  CYC(b_+274, b_+276); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+276, b_+277); A = mem_rd(gb, DE);
  CYC(b_+277, b_+279); alu_and(gb, 0x0f);
  CYC(b_+279, b_+282); push_effect(gb, b_+282); goto initHerosCaveWarp;
afterInitWarp:
  CYC(b_+282, b_+284); A = 0x84;
  CYC(b_+284, b_+287); W8(wWarpDestGroup) = A;
  RET(b_+287); return; // ret

// interactionCode7e@spinLink: reached by two genuine calls, from @minibossState3's return
// address 0x44e2 above and @herosCaveState3's return address 0x4563 above; never separately
// hooked. Has both a literal ret exit and a tail-jump-into-external-hook exit, so every exit
// needs both callers' resume checks.
spinLink:
  CALL_C(b_+199, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+202);
  CALL_C(b_+202, interactionAnimate_hook, SYM(interactionAnimate), b_+205);
  CYC(b_+205, b_+208); A = W8(wLinkDeathTrigger);
  CYC(b_+208, b_+209); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(b_+209);
    if (gb->pc == b_+144 && gb->sp == sp0_) goto afterSpin1;
    if (gb->pc == b_+273 && gb->sp == sp0_) goto afterSpin2;
    return;
  } // ret nz
  CYC(b_+209, b_+210);
  CYC(b_+210, b_+213); A = W8(wFrameCounter);
  CYC(b_+213, b_+215); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+215, b_+217); goto afterDirUpdate; } // jr nz
  CYC(b_+215, b_+217);
  CYC(b_+217, b_+220); SET_HL(w1Link_direction);
  CYC(b_+220, b_+221); A = mem_rd(gb, HL);
  CYC(b_+221, b_+222); A = alu_inc8(gb, A);
  CYC(b_+222, b_+224); alu_and(gb, 0x03);
  CYC(b_+224, b_+225); mem_wr(gb, HL, A);

afterDirUpdate:
  CYC(b_+225, b_+228); interactionDecCounter1_hook(gb); // jp
  if (gb->pc == b_+144 && gb->sp == sp0_) goto afterSpin1;
  if (gb->pc == b_+273 && gb->sp == sp0_) goto afterSpin2;
  return;

// interactionCode7e@initHerosCaveWarp: reached by one genuine call, from @herosCaveState3's
// return address 0x456c above; never separately hooked.
initHerosCaveWarp:
  CYC(b_+288, b_+291); SET_HL(b_+315); // interactionCode7e@herosCaveWarps
  CYC(b_+291, b_+292); interactionCode7e_addDoubleIndexToHl_from_rst(gb, b_+292);
  CYC(b_+292, b_+293); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+293, b_+296); W8(wWarpDestRoom) = A;
  CYC(b_+296, b_+297); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+297, b_+300); W8(wWarpDestPos) = A;
  CYC(b_+300, b_+302); A = 0x85;
  CYC(b_+302, b_+305); W8(wWarpDestGroup) = A;
  CYC(b_+305, b_+306); alu_xor(gb, A);
  CYC(b_+306, b_+309); W8(wWarpTransition) = A;
  CYC(b_+309, b_+311); A = 0x03;
  CYC(b_+311, b_+314); W8(wWarpTransition2) = A;
  RET(b_+314);
  if (gb->pc == b_+282 && gb->sp == sp0_) goto afterInitWarp;
  return;
}
