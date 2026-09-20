#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode7f_jump_table(GB *gb) {
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

static void interaction7f_subid00_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void interaction7f_subid00_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void interaction7f_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x476f, checkInteractionState_hook, 0x23fe, 0x4772);
  if (!(F & FZ)) { CYCT(0x4772, 0x4775); objectPreventLinkFromPassing_hook(gb); return; } // jp nz
  CYC(0x4772, 0x4775);
  CYC(0x4775, 0x4777); A = 0x01;
  CYC(0x4777, 0x4778); mem_wr(gb, DE, A);
  CYC(0x4778, 0x477b); SET_BC(0x060a);
  CALL_C(0x477b, objectSetCollideRadii_hook, 0x24a9, 0x477e);
  CALL_C(0x477e, objectGetTileAtPosition_hook, 0x1444, 0x4781);
  CYC(0x4781, 0x4782); H = alu_dec8(gb, H);
  CYC(0x4782, 0x4784); mem_wr(gb, HL, 0x0f);
  CALL_C(0x4784, interactionInitGraphics_hook, 0x15fb, 0x4787);
  CYC(0x4787, 0x478a); objectSetVisible83_hook(gb); return; // jp
}

// interaction7f_subid02@copyEssencePosition: reached by one genuine call, from @state1's
// return address 0x479b below; never separately hooked.
void interaction7f_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x478a, checkInteractionState_hook, 0x23fe, 0x478d);
  if (!(F & FZ)) { CYCT(0x478d, 0x478f); goto state1; } // jr nz
  CYC(0x478d, 0x478f);

  // interaction7f_subid02@state0
  CYC(0x478f, 0x4791); A = 0x01;
  CYC(0x4791, 0x4792); mem_wr(gb, DE, A);
  CALL_C(0x4792, interactionInitGraphics_hook, 0x15fb, 0x4795);
  CYC(0x4795, 0x4798); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(0x4798, 0x479b); push_effect(gb, 0x479b); goto copyEssencePosition;
afterCopy:
  CALL_C(0x479b, interactionAnimate_hook, 0x261b, 0x479e);
  CYC(0x479e, 0x479f); H = D;
  CYC(0x479f, 0x47a1); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x47a1, 0x47a2); A = mem_rd(gb, HL);
  CYC(0x47a2, 0x47a3); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x47a3); return; } // ret z
  CYC(0x47a3, 0x47a4);
  CYC(0x47a4, 0x47a6); mem_wr(gb, HL, 0x00);
  CYC(0x47a6, 0x47a8); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x47a8, 0x47aa); A = 0x80;
  CYC(0x47aa, 0x47ab); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x47ab, 0x47ac); mem_wr(gb, HL, A);
  RET(0x47ac); return; // ret

copyEssencePosition:
  CYC(0x47ad, 0x47af); A = 0x00; // Object.enabled
  CALL_C(0x47af, objectGetRelatedObject1Var_hook, 0x2160, 0x47b2);
  CYC(0x47b2, 0x47b5); objectTakePosition_hook(gb);
  if (gb->pc == 0x479b && gb->sp == sp0_) goto afterCopy;
  return;
}

void interaction7f_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x45ac, 0x45ae); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x45ae, 0x45af); A = mem_rd(gb, DE);
  {
    CYC(0x45af, 0x45b0); push_effect(gb, 0x45b0);
    uint16_t target = interactionCode7f_jump_table(gb);
    if (target == 0x45c0) goto state0;
    if (target == 0x4638) goto state1;
    if (target == 0x46a3) goto state2;
    if (target == 0x46b8) goto state3;
    if (target == 0x46cb) goto state4;
    if (target == 0x4710) goto state5;
    if (target == 0x471c) goto state6;
    goto state7;
  }

state0:
  CYC(0x45c0, 0x45c2); A = 0x01;
  CYC(0x45c2, 0x45c3); mem_wr(gb, DE, A);
  CALL_C(0x45c3, interactionInitGraphics_hook, 0x15fb, 0x45c6);
  CYC(0x45c6, 0x45c8); A = 0x04;
  CALL_C(0x45c8, objectSetCollideRadius_hook, 0x24a1, 0x45cb);
  CYC(0x45cb, 0x45ce); SET_BC(0x7f01); // INTERAC_ESSENCE, $01
  CALL_C(0x45ce, objectCreateInteraction_hook, 0x24c5, 0x45d1);
  CALL_C(0x45d1, getThisRoomFlags_hook, 0x197d, 0x45d4);
  CYC(0x45d4, 0x45d6); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(0x45d6, 0x45d9); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x45d6, 0x45d9);
  CYC(0x45d9, 0x45dc); SET_HL(w1ReservedInteraction1);
  CYC(0x45dc, 0x45de); B = 0x40;
  CALL_C(0x45de, clearMemory_hook, 0x046f, 0x45e1);
  CYC(0x45e1, 0x45e4); SET_HL(w1ReservedInteraction1);
  CYC(0x45e4, 0x45e6); mem_wr(gb, HL, 0x81);
  CYC(0x45e6, 0x45e7); L = alu_inc8(gb, L);
  CYC(0x45e7, 0x45e9); mem_wr(gb, HL, 0x7f); // INTERAC_ESSENCE
  CYC(0x45e9, 0x45ea); L = alu_inc8(gb, L);
  CYC(0x45ea, 0x45ec); mem_wr(gb, HL, 0x02);
  CALL_C(0x45ec, objectCopyPosition_hook, 0x2242, 0x45ef);
  CYC(0x45ef, 0x45f1); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x45f1, 0x45f3); A = H8(hActiveObjectType);
  CYC(0x45f3, 0x45f4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x45f4, 0x45f6); A = H8(hActiveObject);
  CYC(0x45f6, 0x45f7); mem_wr(gb, HL, A);
  CYC(0x45f7, 0x45f8); H = D;
  CYC(0x45f8, 0x45fa); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x45fa, 0x45fc); mem_wr(gb, HL, (uint8_t)-0x10);
  CYC(0x45fc, 0x45ff); A = W8(wDungeonIndex);
  CYC(0x45ff, 0x4600); A = alu_dec8(gb, A);
  CYC(0x4600, 0x4602); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(0x4602, 0x4604); goto afterDungeonOverride; } // jr nz
  CYC(0x4602, 0x4604);
  CYC(0x4604, 0x4606); A = 0x05;

afterDungeonOverride:
  CYC(0x4606, 0x4608); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4608, 0x4609); mem_wr(gb, HL, A);
  CYC(0x4609, 0x460a); B = A;
  CYC(0x460a, 0x460b); alu_add(gb, A);
  CYC(0x460b, 0x460c); alu_add(gb, B);
  CYC(0x460c, 0x460f); SET_HL(0x4620); // interaction7f_subid00@essenceOamData
  CYC(0x460f, 0x4610); interaction7f_subid00_addAToHl_from_rst(gb, 0x4610);
  CYC(0x4610, 0x4612); E = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(0x4612, 0x4613); A = mem_rd(gb, DE);
  CYC(0x4613, 0x4614); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4614, 0x4615); SET_HL(HL + 1); // inc hl
  CYC(0x4615, 0x4616); mem_wr(gb, DE, A);
  CYC(0x4616, 0x4617); E = alu_dec8(gb, E);
  CYC(0x4617, 0x4618); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4618, 0x4619); mem_wr(gb, DE, A);
  CYC(0x4619, 0x461a); A = mem_rd(gb, HL);
  CALL_C(0x461a, interactionSetAnimation_hook, 0x262e, 0x461d);
  CYC(0x461d, 0x4620); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(0x4638, 0x463b); A = W8(wFrameCounter);
  CYC(0x463b, 0x463d); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x463d); return; } // ret nz
  CYC(0x463d, 0x463e);
  CYC(0x463e, 0x463f); H = D;
  CYC(0x463f, 0x4641); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4641, 0x4642); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4642, 0x4643); A = mem_rd(gb, HL);
  CYC(0x4643, 0x4645); alu_and(gb, 0x0f);
  CYC(0x4645, 0x4648); SET_HL(0x4693); // interaction7f_subid00@essenceFloatOffsets
  CYC(0x4648, 0x4649); interaction7f_subid00_addAToHl_from_rst(gb, 0x4649);
  CYC(0x4649, 0x464a); A = mem_rd(gb, HL);
  CYC(0x464a, 0x464c); alu_add(gb, 0xf0);
  CYC(0x464c, 0x464e); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x464e, 0x464f); mem_wr(gb, DE, A);
  CYC(0x464f, 0x4652); A = W8(wLinkInAir);
  CYC(0x4652, 0x4653); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4653); return; } // ret nz
  CYC(0x4653, 0x4654);
  CYC(0x4654, 0x4657); A = W8(wLinkGrabState);
  CYC(0x4657, 0x4658); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4658); return; } // ret nz
  CYC(0x4658, 0x4659);
  CYC(0x4659, 0x465b); B = 0x04;
  CALL_C(0x465b, objectCheckCenteredWithLink_hook, 0x1fee, 0x465e);
  if (!(F & FC)) { RET_TAKEN(0x465e); return; } // ret nc
  CYC(0x465e, 0x465f);
  CYC(0x465f, 0x4661); C = 0x14;
  CALL_C(0x4661, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x4664);
  if (!(F & FC)) { RET_TAKEN(0x4664); return; } // ret nc
  CYC(0x4664, 0x4665);
  CYC(0x4665, 0x4667); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(0x4667); return; } // ret nz
  CYC(0x4667, 0x4668);
  CALL_C(0x4668, clearAllParentItems_hook, 0x2c10, 0x466b);
  CYC(0x466b, 0x466d); A = 0x81;
  CYC(0x466d, 0x4670); W8(wDisabledObjects) = A;
  CYC(0x4670, 0x4673); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x4673, 0x4676); SET_HL(w1Link_direction);
  CYC(0x4676, 0x4678); mem_wr(gb, HL, 0x00); // DIR_UP
  CALL_C(0x4678, objectGetAngleTowardLink_hook, 0x1e9c, 0x467b);
  CYC(0x467b, 0x467c); H = D;
  CYC(0x467c, 0x467e); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x467e, 0x467f); mem_wr(gb, HL, A);
  CYC(0x467f, 0x4681); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4681, 0x4683); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x4683, 0x4685); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4685, 0x4686); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x4686, darkenRoom_hook, 0x3315, 0x4689);
  CYC(0x4689, 0x468b); A = 0x77; // SND_DROPESSENCE
  CALL_C(0x468b, playSound_b00_hook, 0x0c98, 0x468e);
  CYC(0x468e, 0x4690); A = 0xfc; // SNDCTRL_SLOW_FADEOUT
  CYC(0x4690, 0x4693); playSound_b00_hook(gb); return; // jp

state2:
  CALL_C(0x46a3, objectGetAngleTowardLink_hook, 0x1e9c, 0x46a6);
  CYC(0x46a6, 0x46a8); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x46a8, 0x46a9); mem_wr(gb, DE, A);
  CALL_C(0x46a9, objectApplySpeed_hook, 0x201d, 0x46ac);
  CALL_C(0x46ac, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x46af);
  if (!(F & FC)) { RET_TAKEN(0x46af); return; } // ret nc
  CYC(0x46af, 0x46b0);
  CYC(0x46b0, 0x46b2); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(0x46b2, 0x46b4); A = 0x06;
  CYC(0x46b4, 0x46b5); mem_wr(gb, DE, A);
  CYC(0x46b5, 0x46b8); interactionIncState_hook(gb); return; // jp

state3:
  CYC(0x46b8, 0x46ba); C = 0x08;
  CALL_C(0x46ba, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x46bd);
  if (F & FZ) { CYCT(0x46bd, 0x46bf); goto afterSpeedZCheck; } // jr z
  CYC(0x46bd, 0x46bf);
  CALL_C(0x46bf, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x46c2);
  if (!(F & FC)) { RET_TAKEN(0x46c2); return; } // ret nc
  CYC(0x46c2, 0x46c3);

afterSpeedZCheck:
  CYC(0x46c3, 0x46c4); H = D;
  CYC(0x46c4, 0x46c6); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x46c6, 0x46c8); mem_wr(gb, HL, 30);
  CYC(0x46c8, 0x46cb); interactionIncState_hook(gb); return; // jp

state4:
  CALL_C(0x46cb, interactionDecCounter1_hook, 0x23cc, 0x46ce);
  if (!(F & FZ)) { RET_TAKEN(0x46ce); return; } // ret nz
  CYC(0x46ce, 0x46cf);
  CYC(0x46cf, 0x46d1); A = 0x04; // LINK_STATE_04
  CYC(0x46d1, 0x46d4); W8(wLinkForceState) = A;
  CYC(0x46d4, 0x46d6); A = 0x01;
  CYC(0x46d6, 0x46d9); W8(wcc50) = A;
  CALL_C(0x46d9, interactionIncState_hook, 0x23e0, 0x46dc);
  CYC(0x46dc, 0x46df); A = W8(w1Link_yh);
  CYC(0x46df, 0x46e1); alu_sub(gb, 0x0e);
  CYC(0x46e1, 0x46e3); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x46e3, 0x46e4); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x46e4, 0x46e5); L = alu_inc8(gb, L);
  CYC(0x46e5, 0x46e8); A = W8(w1Link_xh);
  CYC(0x46e8, 0x46e9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x46e9, 0x46ea); L = alu_inc8(gb, L);
  CYC(0x46ea, 0x46eb); alu_xor(gb, A);
  CYC(0x46eb, 0x46ec); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x46ec, 0x46ed); mem_wr(gb, HL, A);
  CYC(0x46ed, 0x46ef); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x46ef, 0x46f0); A = mem_rd(gb, HL);
  CYC(0x46f0, 0x46f3); SET_HL(0x4708); // interaction7f_subid00@getEssenceTextTable
  CYC(0x46f3, 0x46f4); interaction7f_subid00_addAToHl_from_rst(gb, 0x46f4);
  CYC(0x46f4, 0x46f6); B = 0x00; // >TX_0000
  CYC(0x46f6, 0x46f7); C = mem_rd(gb, HL);
  CALL_C(0x46f7, showText_hook, 0x1872, 0x46fa);
  CALL_C(0x46fa, getThisRoomFlags_hook, 0x197d, 0x46fd);
  CYC(0x46fd, 0x46ff); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 5)); // ROOMFLAG_BIT_ITEM
  CYC(0x46ff, 0x4701); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4701, 0x4702); A = mem_rd(gb, DE);
  CYC(0x4702, 0x4703); C = A;
  CYC(0x4703, 0x4705); A = 0x40; // TREASURE_ESSENCE
  CYC(0x4705, 0x4708); giveTreasure_hook(gb); return; // jp

state5:
  CALL_C(0x4710, retIfTextIsActive_hook, 0x1859, 0x4713);
  CALL_C(0x4713, interactionIncState_hook, 0x23e0, 0x4716);
  CYC(0x4716, 0x4719); SET_HL(0x49c8); // mainScripts.essenceScript_essenceGetCutscene
  CYC(0x4719, 0x471c); interactionSetScript_hook(gb); return; // jp

state6:
  CALL_C(0x471c, interactionRunScript_hook, 0x2552, 0x471f);
  if (!(F & FC)) { RET_TAKEN(0x471f); return; } // ret nc
  CYC(0x471f, 0x4720);
  CALL_C(0x4720, interactionIncState_hook, 0x23e0, 0x4723);
  CYC(0x4723, 0x4725); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4725, 0x4727); mem_wr(gb, HL, 30);

state7:
  CALL_C(0x4727, interactionDecCounter1_hook, 0x23cc, 0x472a);
  if (!(F & FZ)) { RET_TAKEN(0x472a); return; } // ret nz
  CYC(0x472a, 0x472b);
  CYC(0x472b, 0x472d); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x472d, 0x472e); A = mem_rd(gb, HL);
  CYC(0x472e, 0x472f); alu_add(gb, A);
  CYC(0x472f, 0x4732); SET_HL(0x474f); // interaction7f_subid00@essenceWarps
  CYC(0x4732, 0x4733); interaction7f_subid00_addDoubleIndexToHl_from_rst(gb, 0x4733);
  CYC(0x4733, 0x4734); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4734, 0x4737); W8(wWarpDestGroup) = A;
  CYC(0x4737, 0x4738); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4738, 0x473b); W8(wWarpDestRoom) = A;
  CYC(0x473b, 0x473c); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x473c, 0x473f); W8(wWarpDestPos) = A;
  CYC(0x473f, 0x4740); A = mem_rd(gb, HL);
  CYC(0x4740, 0x4743); W8(wWarpTransition) = A;
  CYC(0x4743, 0x4745); A = 0x83;
  CYC(0x4745, 0x4748); W8(wWarpTransition2) = A;
  CYC(0x4748, 0x4749); alu_xor(gb, A);
  CYC(0x4749, 0x474c); W8(wActiveMusic) = A;
  CYC(0x474c, 0x474f); clearStaticObjects_hook(gb); return; // jp
}

// INTERAC_ESSENCE
void interactionCode7f_hook(GB *gb) {
  CYC(0x459d, 0x45a0); A = W8(wLinkDeathTrigger);
  CYC(0x45a0, 0x45a1); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x45a1); return; } // ret nz
  CYC(0x45a1, 0x45a2);
  CYC(0x45a2, 0x45a4); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x45a4, 0x45a5); A = mem_rd(gb, DE);
  {
    CYC(0x45a5, 0x45a6); push_effect(gb, 0x45a6);
    uint16_t target = interactionCode7f_jump_table(gb);
    if (target == 0x45ac) { interaction7f_subid00_hook(gb); return; }
    if (target == 0x476f) { interaction7f_subid01_hook(gb); return; }
    interaction7f_subid02_hook(gb); return;
  }
}
