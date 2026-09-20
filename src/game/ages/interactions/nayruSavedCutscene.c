#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t nayruSavedCutscene_jump_table(GB *gb) {
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

static void nayruSavedCutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Nayru waking up after being freed from possession
void interaction6e_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54da, 0x54db); A = mem_rd(gb, DE);
  {
    CYC(0x54db, 0x54dc); push_effect(gb, 0x54dc);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == 0x552c) goto state1;
    if (target == 0x553a) goto state2;
  }

  CALL_C(0x54e2, interactionInitGraphics_hook, 0x15fb, 0x54e5);
  CALL_C(0x54e5, interactionIncState_hook, 0x23e0, 0x54e8);
  CYC(0x54e8, 0x54ea); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x54ea, 0x54ec); mem_wr(gb, HL, 0x58);
  CYC(0x54ec, 0x54ee); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x54ee, 0x54f0); mem_wr(gb, HL, 0x78);
  CYC(0x54f0, 0x54f2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x54f2, 0x54f4); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(0x54f4, 0x54f6); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x54f6, 0x54f8); A = 0x01;
  CYC(0x54f8, 0x54f9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x54f9, 0x54fa); mem_wr(gb, HL, A);
  CYC(0x54fa, 0x54fd); W8(wLoadedTreeGfxIndex) = A;
  CYC(0x54fd, 0x5500); SET_HL(0xd008); // w1Link.direction
  CYC(0x5500, 0x5502); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(0x5502, 0x5504); L = 0x0b; // <w1Link.yh
  CYC(0x5504, 0x5506); mem_wr(gb, HL, 0x64);
  CYC(0x5506, 0x5508); L = 0x0d; // <w1Link.xh
  CYC(0x5508, 0x550a); mem_wr(gb, HL, 0x78);
  CYC(0x550a, 0x550d); SET_HL(0xcfd0); // wTmpcfc0.genericCutscene.cfd0
  CYC(0x550d, 0x550f); B = 0x10;
  CALL_C(0x550f, clearMemory_hook, 0x046f, 0x5512);
  CALL_C(0x5512, setCameraFocusedObjectToLink_hook, 0x12f0, 0x5515);
  CALL_C(0x5515, resetCamera_hook, 0x12ce, 0x5518);
  CYC(0x5518, 0x551a); A = H8(hActiveObject);
  CYC(0x551a, 0x551b); D = A;
  CALL_C(0x551b, fadeinFromWhite_hook, 0x3299, 0x551e);
  CYC(0x551e, 0x5520); A = 0x0a;
  CALL_C(0x5520, interactionSetAnimation_hook, 0x262e, 0x5523);
  CALL_C(0x5523, objectSetVisible82_hook, 0x1e69, 0x5526);
  CYC(0x5526, 0x5529); SET_HL(0x72ca); // mainScripts.interaction6e_subid00Script
  CYC(0x5529, 0x552c); interactionSetScript_hook(gb); return; // jp

state1:
  CALL_C(0x552c, interactionRunScript_hook, 0x2552, 0x552f);
  if (!(F & FC)) { CYCT(0x552f, 0x5532); interactionAnimate_hook(gb); return; } // jp nc
  CYC(0x552f, 0x5532);
  CALL_C(0x5532, interactionIncState_hook, 0x23e0, 0x5535);
  CYC(0x5535, 0x5537); A = 0x04;
  CYC(0x5537, 0x553a); fadeoutToWhiteWithDelay_hook(gb); return; // jp

state2:
  CYC(0x553a, 0x553d); A = W8(wPaletteThread_mode);
  CYC(0x553d, 0x553e); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x553e); return; } // ret nz
  CYC(0x553e, 0x553f);
  CYC(0x553f, 0x5541); A = 0x19; // GLOBALFLAG_BEAT_POSSESSED_NAYRU
  CALL_C(0x5541, setGlobalFlag_hook, 0x31f9, 0x5544);
  CYC(0x5544, 0x5546); A = 0x0c; // CUTSCENE_NAYRU_WARP_TO_MAKU_TREE
  CYC(0x5546, 0x5549); W8(wCutsceneTrigger) = A;
  CYC(0x5549, 0x554c); interactionDelete_hook(gb); return; // jp
}

// Queen Ambi
void interaction6e_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x554c, 0x554d); A = mem_rd(gb, DE);
  {
    CYC(0x554d, 0x554e); push_effect(gb, 0x554e);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == 0x5571) goto state1;
    if (target == 0x558a) goto state2;
    if (target == 0x55a6) goto state3;
    if (target == 0x55de) goto state4;
  }

  CALL_C(0x5558, interactionInitGraphics_hook, 0x15fb, 0x555b);
  CALL_C(0x555b, interactionIncState_hook, 0x23e0, 0x555e);
  CYC(0x555e, 0x5560); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5560, 0x5562); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x5562, 0x5564); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x5564, 0x5566); A = 0x01;
  CYC(0x5566, 0x5567); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5567, 0x5568); mem_wr(gb, HL, A);
  CALL_C(0x5568, objectSetVisiblec2_hook, 0x1e45, 0x556b);
  CYC(0x556b, 0x556e); SET_HL(0x7302); // mainScripts.interaction6e_subid01Script_part1
  CYC(0x556e, 0x5571); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(0x5571, 0x5573); C = 0x30;
  CALL_C(0x5573, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5576);
  if (!(F & FZ)) { RET_TAKEN(0x5576); return; } // ret nz
  CYC(0x5576, 0x5577);
  CALL_C(0x5577, interactionRunScript_hook, 0x2552, 0x557a);
  if (!(F & FC)) { CYCT(0x557a, 0x557c); goto animate; } // jr nc
  CYC(0x557a, 0x557c);
  CALL_C(0x557c, interactionIncState_hook, 0x23e0, 0x557f);
  CYC(0x557f, 0x5581); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5581, 0x5583); mem_wr(gb, HL, 0xf4); // 244
  // Use "direction" variable temporarily as "animation"
  CYC(0x5583, 0x5585); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x5585, 0x5587); mem_wr(gb, HL, 0x05);

animate:
  CYC(0x5587, 0x558a); interactionAnimate_hook(gb); return; // jp

state2:
  CALL_C(0x558a, interactionDecCounter1_hook, 0x23cc, 0x558d);
  if (F & FZ) { CYCT(0x558d, 0x558f); goto l559f; } // jr z
  CYC(0x558d, 0x558f);
  CYC(0x558f, 0x5590); A = mem_rd(gb, HL);
  CYC(0x5590, 0x5592); alu_cp(gb, 0xc1);
  if (!(F & FC)) { CYCT(0x5592, 0x5594); goto animate; } // jr nc
  CYC(0x5592, 0x5594);
  CYC(0x5594, 0x5596); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x5596); return; } // ret nz
  CYC(0x5596, 0x5597);
  CYC(0x5597, 0x5599); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x5599, 0x559a); A = mem_rd(gb, HL);
  CYC(0x559a, 0x559c); alu_xor(gb, 0x02);
  CYC(0x559c, 0x559d); mem_wr(gb, HL, A);
  CYC(0x559d, 0x559f); goto setAnimation; // jr

l559f:
  CYC(0x559f, 0x55a0); L = E;
  CYC(0x55a0, 0x55a1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x55a1, 0x55a3); A = 0x06;

setAnimation:
  CYC(0x55a3, 0x55a6); interactionSetAnimation_hook(gb); return; // jp

state3:
  CYC(0x55a6, 0x55a9); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x55a9, 0x55ab); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(0x55ab, 0x55ad); goto l55bc; } // jr z
  CYC(0x55ab, 0x55ad);
  CYC(0x55ad, 0x55b0); A = W8(wFrameCounter);
  CYC(0x55b0, 0x55b1); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x55b1); return; } // ret c
  CYC(0x55b1, 0x55b2);
  CYC(0x55b2, 0x55b4); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x55b4, 0x55b5); A = mem_rd(gb, DE);
  CYC(0x55b5, 0x55b6); A = alu_inc8(gb, A);
  CYC(0x55b6, 0x55b8); alu_and(gb, 0x01);
  CYC(0x55b8, 0x55ba); alu_add(gb, 0x78);
  CYC(0x55ba, 0x55bb); mem_wr(gb, DE, A);
  RET(0x55bb); return; // ret

l55bc:
  CALL_C(0x55bc, interactionIncState_hook, 0x23e0, 0x55bf);
  CYC(0x55bf, 0x55c1); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x55c1, 0x55c3); mem_wr(gb, HL, 0x78);
  CYC(0x55c3, 0x55c5); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x55c5, 0x55c7); A = 0x06;
  CYC(0x55c7, 0x55c8); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x55c8, 0x55c9); mem_wr(gb, HL, A);
  CYC(0x55c9, 0x55cc); SET_HL(0x7328); // mainScripts.interaction6e_subid01Script_part2
  CALL_C(0x55cc, interactionSetScript_hook, 0x2544, 0x55cf);
  CYC(0x55cf, 0x55d1); A = 0xd2; // SND_LIGHTNING
  CALL_C(0x55d1, playSound_b00_hook, 0x0c98, 0x55d4);
  CYC(0x55d4, 0x55d6); A = 0x21; // MUS_DISASTER
  CALL_C(0x55d6, playSound_b00_hook, 0x0c98, 0x55d9);
  CYC(0x55d9, 0x55db); A = 0x04;
  CYC(0x55db, 0x55de); fadeinFromWhiteWithDelay_hook(gb); return; // jp

state4:
  CYC(0x55de, 0x55e1); A = W8(wPaletteThread_mode);
  CYC(0x55e1, 0x55e2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x55e2, 0x55e4); goto l55ed; } // jr nz
  CYC(0x55e2, 0x55e4);
  CYC(0x55e4, 0x55e6); C = 0x30;
  CALL_C(0x55e6, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x55e9);
  if (!(F & FZ)) { RET_TAKEN(0x55e9); return; } // ret nz
  CYC(0x55e9, 0x55ea);
  CALL_C(0x55ea, interactionRunScript_hook, 0x2552, 0x55ed);

l55ed:
  CYC(0x55ed, 0x55f0); interactionAnimate_hook(gb); return; // jp
}

// Ghost Veran
void interaction6e_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x55f0, 0x55f1); A = mem_rd(gb, DE);
  {
    CYC(0x55f1, 0x55f2); push_effect(gb, 0x55f2);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == 0x561a) goto state1;
    if (target == 0x5649) goto state2;
    if (target == 0x567e) goto state3;
  }

  CALL_C(0x55fa, interactionInitGraphics_hook, 0x15fb, 0x55fd);
  CALL_C(0x55fd, interactionIncState_hook, 0x23e0, 0x5600);
  CYC(0x5600, 0x5602); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5602, 0x5604); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x5604, 0x5606); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5606, 0x5608); mem_wr(gb, HL, 0x0e);
  CYC(0x5608, 0x560a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x560a, 0x560c); mem_wr(gb, HL, 0x48);
  CYC(0x560c, 0x560f); SET_BC(0x560c); // TX_560c
  CALL_C(0x560f, showText_hook, 0x1872, 0x5612);
  CYC(0x5612, 0x5614); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(0x5614, playSound_b00_hook, 0x0c98, 0x5617);
  CYC(0x5617, 0x561a); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(0x561a, 0x561c); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x561c, 0x561d); A = mem_rd(gb, DE);
  CYC(0x561d, 0x561f); alu_cp(gb, 0x48);
  CYC(0x561f, 0x5621); A = 0xa4; // SND_BEAM
  if (F & FZ) { CALL_C_CC(0x5621, playSound_b00_hook, 0x0c98, 0x5624); } else { CYC(0x5621, 0x5624); } // call z
  CALL_C(0x5624, interactionDecCounter1_hook, 0x23cc, 0x5627);
  if (!(F & FZ)) { CYCT(0x5627, 0x5629); goto applySpeedAndAnimate; } // jr nz
  CYC(0x5627, 0x5629);
  CYC(0x5629, 0x562b); mem_wr(gb, HL, 0xac);
  CYC(0x562b, 0x562d); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x562d, 0x562e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x562e, 0x5630); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5630, 0x5632); mem_wr(gb, HL, 0x16);
  // Link moves up, while facing down
  CYC(0x5632, 0x5635); SET_HL(0xd008); // w1Link.direction
  CYC(0x5635, 0x5637); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(0x5637, 0x5638); L = alu_inc8(gb, L);
  CYC(0x5638, 0x563a); mem_wr(gb, HL, 0x00); // ANGLE_UP
  CYC(0x563a, 0x563c); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(0x563c, 0x563f); W8(wLinkForceState) = A;
  CYC(0x563f, 0x5641); A = 0x04;
  CYC(0x5641, 0x5644); W8(wLinkStateParameter) = A;
  CYC(0x5644, 0x5646); A = 0xc9; // SND_CIRCLING
  CALL_C(0x5646, playSound_b00_hook, 0x0c98, 0x5649);

state2:
  CALL_C(0x5649, interactionDecCounter1_hook, 0x23cc, 0x564c);
  if (F & FZ) { CYCT(0x564c, 0x564e); goto beginPossessingAmbi; } // jr z
  CYC(0x564c, 0x564e);
  CYC(0x564e, 0x564f); A = mem_rd(gb, HL);
  CYC(0x564f, 0x5650); push_effect(gb, AF);
  CYC(0x5650, 0x5652); alu_cp(gb, 0x56);
  CYC(0x5652, 0x5654); A = 0xc9; // SND_CIRCLING
  if (F & FZ) { CALL_C_CC(0x5654, playSound_b00_hook, 0x0c98, 0x5657); } else { CYC(0x5654, 0x5657); } // call z
  CYC(0x5657, 0x5658); SET_AF(pop_effect(gb));
  CYC(0x5658, 0x5659); alu_rrca(gb);
  CYC(0x5659, 0x565b); E = INTERACTION_BASE + OBJ_ANGLE;
  if (!(F & FC)) { CYCT(0x565b, 0x565d); goto l5662; } // jr nc
  CYC(0x565b, 0x565d);
  CYC(0x565d, 0x565e); A = mem_rd(gb, DE);
  CYC(0x565e, 0x565f); A = alu_dec8(gb, A);
  CYC(0x565f, 0x5661); alu_and(gb, 0x1f);
  CYC(0x5661, 0x5662); mem_wr(gb, DE, A);

l5662:
  CYC(0x5662, 0x5664); A = 0x10;
  CYC(0x5664, 0x5667); SET_BC(0x7e78);
  CALL_C(0x5667, objectSetPositionInCircleArc_hook, 0x210e, 0x566a);
  CYC(0x566a, 0x566d); interactionAnimate_hook(gb); return; // jp

beginPossessingAmbi:
  CYC(0x566d, 0x566f); mem_wr(gb, HL, 0x50);
  CYC(0x566f, 0x5670); L = E;
  CYC(0x5670, 0x5671); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x5671, 0x5673); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5673, 0x5675); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(0x5675, 0x5677); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5677, 0x5679); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x5679, 0x567b); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(0x567b, playSound_b00_hook, 0x0c98, 0x567e);

state3:
  CALL_C(0x567e, interactionDecCounter1_hook, 0x23cc, 0x5681);
  if (!(F & FZ)) { CYCT(0x5681, 0x5683); goto l568d; } // jr nz
  CYC(0x5681, 0x5683);
  CYC(0x5683, 0x5685); A = 0x07;
  CYC(0x5685, 0x5688); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(0x5688, 0x568a); A = 0x04;
  CYC(0x568a, 0x568d); interactionDelete_hook(gb); return; // jp

l568d:
  CYC(0x568d, 0x568f); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x568f, 0x5690); A = mem_rd(gb, HL);
  CYC(0x5690, 0x5692); alu_xor(gb, 0x80);
  CYC(0x5692, 0x5693); mem_wr(gb, HL, A);

applySpeedAndAnimate:
  CALL_C(0x5693, objectApplySpeed_hook, 0x201d, 0x5696);
  CYC(0x5696, 0x5699); interactionAnimate_hook(gb); return; // jp
}

void interaction6e_runScriptAndAnimate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x569d, interactionRunScript_hook, 0x2552, 0x56a0);
  CYC(0x56a0, 0x56a3); interactionAnimate_hook(gb); return; // jp
}

void interaction6e_initRalph_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x56a3, interactionInitGraphics_hook, 0x15fb, 0x56a6);
  CALL_C(0x56a6, interactionIncState_hook, 0x23e0, 0x56a9);
  CYC(0x56a9, 0x56ab); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x56ab, 0x56ad); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(0x56ad, objectSetVisible82_hook, 0x1e69, 0x56b0);
  CYC(0x56b0, 0x56b3); SET_HL(0x7386); // mainScripts.interaction6e_subid03Script
  CYC(0x56b3, 0x56b6); interactionSetScript_hook(gb); return; // jp
}

// Ralph
void interaction6e_subid03_hook(GB *gb) {
  CYC(0x5699, 0x569a); A = mem_rd(gb, DE);
  CYC(0x569a, 0x569b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x569b, 0x569d); interaction6e_initRalph_hook(gb); return; } // jr z
  CYC(0x569b, 0x569d);
  interaction6e_runScriptAndAnimate_hook(gb); return;
}

// Guards that run into the room
void interaction6e_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56b6, 0x56b7); A = mem_rd(gb, DE);
  CYC(0x56b7, 0x56b8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x56b8, 0x56ba); interaction6e_runScriptAndAnimate_hook(gb); return; } // jr nz
  CYC(0x56b8, 0x56ba);
  CALL_C(0x56ba, interactionInitGraphics_hook, 0x15fb, 0x56bd);
  CALL_C(0x56bd, interactionIncState_hook, 0x23e0, 0x56c0);
  CYC(0x56c0, 0x56c2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x56c2, 0x56c4); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x56c4, 0x56c6); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x56c6, 0x56c8); mem_wr(gb, HL, 0xb0);
  CYC(0x56c8, 0x56ca); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x56ca, 0x56cc); mem_wr(gb, HL, 0x78);
  CALL_C(0x56cc, objectSetVisible82_hook, 0x1e69, 0x56cf);
  CYC(0x56cf, 0x56d1); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x56d1, 0x56d2); A = mem_rd(gb, DE);
  CYC(0x56d2, 0x56d5); SET_HL(0x56dc); // @scriptTable
  CYC(0x56d5, 0x56d6); nayruSavedCutscene_add_double_index(gb, 0x56d6);
  CYC(0x56d6, 0x56d7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x56d7, 0x56d8); H = mem_rd(gb, HL);
  CYC(0x56d8, 0x56d9); L = A;
  CYC(0x56d9, 0x56dc); interactionSetScript_hook(gb); return; // jp
}

// INTERAC_NAYRU_SAVED_CUTSCENE
void interactionCode6e_hook(GB *gb) {
  CYC(0x54ca, 0x54cc); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x54cc, 0x54cd); A = mem_rd(gb, DE);
  CYC(0x54cd, 0x54cf); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(0x54cf, 0x54d0); push_effect(gb, 0x54d0);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == 0x54da) { interaction6e_subid00_hook(gb); return; }
    if (target == 0x554c) { interaction6e_subid01_hook(gb); return; }
    if (target == 0x55f0) { interaction6e_subid02_hook(gb); return; }
    if (target == 0x5699) { interaction6e_subid03_hook(gb); return; }
    interaction6e_subid04_hook(gb); return; // target == 0x56b6
  }
}
