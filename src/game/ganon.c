#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/common/enemies/ganon.s (ENEMY_GANON), bank $10.

static void ganon_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void ganon_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t ganon_jump_table(GB *gb) {
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

void ganon_state_uninitialized_hook(GB *gb);
void ganon_state1_hook(GB *gb);
void ganon_state2_hook(GB *gb);
void ganon_state3_hook(GB *gb);
void ganon_state4_hook(GB *gb);
void ganon_state5_hook(GB *gb);
void ganon_state6_hook(GB *gb);
void ganon_state7_hook(GB *gb);
void ganon_state8_hook(GB *gb);
void ganon_state8_substate0_hook(GB *gb);
void ganon_state8_substate1_hook(GB *gb);
void ganon_state8_substate2_hook(GB *gb);
void ganon_state8_substate3_hook(GB *gb);
void ganon_state8_substate4_hook(GB *gb);
void ganon_state8_substate5_hook(GB *gb);
void ganon_state8_substate6_hook(GB *gb);
void ganon_state8_spawnProjectile_hook(GB *gb);
void ganon_state8_substate7_hook(GB *gb);
void ganon_state9_hook(GB *gb);
void ganon_state9_substate0_hook(GB *gb);
void ganon_state9_substate1_hook(GB *gb);
void ganon_state9_substate3_hook(GB *gb);
void ganon_state9_substate4_hook(GB *gb);
void ganon_state9_substate4__spawnProjectile_hook(GB *gb);
void ganon_state9_substate5_hook(GB *gb);
void ganon_state9_substate6_hook(GB *gb);
void ganon_state9_substate7_hook(GB *gb);
void ganon_stateA_hook(GB *gb);
void ganon_stateA_substate1_hook(GB *gb);
void ganon_stateA_substate2_hook(GB *gb);
void ganon_stateA_substate3_hook(GB *gb);
void ganon_stateA_substate4_hook(GB *gb);
void ganon_stateA_substate5_hook(GB *gb);
void ganon_stateA_substate6_hook(GB *gb);
void ganon_stateA_substate7_hook(GB *gb);
void ganon_stateB_hook(GB *gb);
void ganon_stateB_substate1_hook(GB *gb);
void ganon_stateB_substate2_hook(GB *gb);
void ganon_stateB_substate3_hook(GB *gb);
void ganon_stateB_substate4_hook(GB *gb);
void ganon_stateB_substate5_hook(GB *gb);
void ganon_stateB_substate6_hook(GB *gb);
void ganon_stateB_substate7_hook(GB *gb);
void ganon_stateB_substate8_hook(GB *gb);
void ganon_stateB_substate9_hook(GB *gb);
void ganon_stateB_substateA_hook(GB *gb);
void ganon_stateC_hook(GB *gb);
void ganon_stateC_substate1_hook(GB *gb);
void ganon_stateC_substate3_hook(GB *gb);
void ganon_stateC_substate4_hook(GB *gb);
void ganon_stateC_substate5_hook(GB *gb);
void ganon_stateC_substate6_hook(GB *gb);
void ganon_stateC_substate7_hook(GB *gb);
void ganon_stateC_substate8_hook(GB *gb);
void ganon_stateC_substate9_hook(GB *gb);
void ganon_stateC_substateA_hook(GB *gb);
void ganon_stateD_hook(GB *gb);
void ganon_stateD_substate3_hook(GB *gb);
void ganon_stateE_hook(GB *gb);
void ganon_stateE_substate0_hook(GB *gb);
void ganon_stateE_substate1_hook(GB *gb);
void ganon_stateE_substate2_hook(GB *gb);
void ganon_stateE_substate3_hook(GB *gb);
void ganon_loadGfxHeader_hook(GB *gb);
void ganon_updateTeleportAnimationGoingOut_hook(GB *gb);
void ganon_updateFlickeringXPosition_hook(GB *gb);
void ganon_updateTeleportVarsAndPlaySound_hook(GB *gb);
void ganon_updateTeleportAnimationComingIn_hook(GB *gb);
void ganon_finishAttack_hook(GB *gb);
void label_10_135_hook(GB *gb);
void ganon_decideNextMove_hook(GB *gb);
void ganon_decideTeleportLocationAndCounter_hook(GB *gb);
void ganon_spawnPart_hook(GB *gb);
void ganon_makeRoomBoundarySolid_hook(GB *gb);
void ganon_updateSeizurePalette_hook(GB *gb);
void ganon_setTileReplacementMode_hook(GB *gb);

// 10:50a3 (real call from enemyCode04, tail-jumps into whichever state root is selected,
// so this stays a plain non-hooked helper invoked via explicit push_effect).
void enemyCode04__normalStatus_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50a3, 0x50a5); E = 0x84;  // ld e,Enemy.state
  CYC(0x50a5, 0x50a6); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x50a6, 0x50a7); push_effect(gb, 0x50a7);
  switch (ganon_jump_table(gb)) {
    case 0x50c5: ganon_state_uninitialized_hook(gb); return;
    case 0x5152: ganon_state1_hook(gb); return;
    case 0x5166: ganon_state2_hook(gb); return;
    case 0x517f: ganon_state3_hook(gb); return;
    case 0x51a1: ganon_state4_hook(gb); return;
    case 0x51c9: ganon_state5_hook(gb); return;
    case 0x51e8: ganon_state6_hook(gb); return;
    case 0x520b: ganon_state7_hook(gb); return;
    case 0x5216: ganon_state8_hook(gb); return;
    case 0x52b7: ganon_state9_hook(gb); return;
    case 0x5354: ganon_stateA_hook(gb); return;
    case 0x5419: ganon_stateB_hook(gb); return;
    case 0x551b: ganon_stateC_hook(gb); return;
    case 0x564d: ganon_stateD_hook(gb); return;
    case 0x566b: ganon_stateE_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void enemyCode04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(0x505d, 0x505f); goto normalStatus; } CYC(0x505d, 0x505f);  // jr z,@normalStatus
  CYC(0x505f, 0x5061); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(0x5061); return; } CYC(0x5061, 0x5062);  // ret c
  if (!(F & FZ)) { CYCT(0x5062, 0x5064); goto normalStatus; } CYC(0x5062, 0x5064);  // jr nz,@normalStatus
  CALL_C(0x5064, checkLinkVulnerable_hook, 0x1d28, 0x5067);
  if (!(F & FC)) { RET_TAKEN(0x5067); return; } CYC(0x5067, 0x5068);  // ret nc
  CYC(0x5068, 0x5069); H = D;  // ld h,d
  CYC(0x5069, 0x506b); L = 0xa9;  // ld l,Enemy.health
  CYC(0x506b, 0x506d); mem_wr(gb, HL, 0x40);  // ld (hl),$40
  CYC(0x506d, 0x506f); L = 0x84;  // ld l,Enemy.state
  CYC(0x506f, 0x5071); mem_wr(gb, HL, 0x0e);  // ld (hl),$0e
  CYC(0x5071, 0x5072); L = alu_inc8(gb, L);  // inc l
  CYC(0x5072, 0x5074); mem_wr(gb, HL, 0x00);  // ld (hl),$00 [substate]
  CYC(0x5074, 0x5075); L = alu_inc8(gb, L);  // inc l
  CYC(0x5075, 0x5077); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(0x5077, 0x5079); A = 0x01;  // ld a,$01
  CYC(0x5079, 0x507c); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(0x507c, 0x507e); A = 0x67;  // ld a,SND_BOSS_DEAD
  CALL_C(0x507e, playSound_b00_hook, 0x0c98, 0x5081);
  CYC(0x5081, 0x5083); A = 0xb4;  // ld a,GFXH_GANON_D
  CALL_C(0x5083, ganon_loadGfxHeader_hook, 0x56de, 0x5086);
  CYC(0x5086, 0x5088); A = 0x0e;  // ld a,$0e
  CALL_C(0x5088, enemySetAnimation_hook, 0x282b, 0x508b);
  CALL_C(0x508b, getThisRoomFlags_hook, 0x197d, 0x508e);
  CYC(0x508e, 0x5090); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x5090, 0x5092); L = 0xf1;  // ld l,<ROOM_ZELDA_IN_FINAL_DUNGEON
  CYC(0x5092, 0x5094); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x5094, 0x5096); L = 0xf5;  // ld l,<ROOM_TWINROVA_FIGHT
  CYC(0x5096, 0x5098); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x5098, 0x509a); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CALL_C(0x509a, playSound_b00_hook, 0x0c98, 0x509d);
  CYC(0x509d, 0x50a0); SET_BC(0x2f0e);  // ld bc,TX_2f0e
  CYC(0x50a0, 0x50a3); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);  // jp showText
normalStatus:
  CYC(0x50a3, 0x50a5); push_effect(gb, 0x50a5); enemyCode04__normalStatus_hook(gb);  // call @normalStatus
}

void ganon_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50c5, 0x50c6); H = D;  // ld h,d
  CYC(0x50c6, 0x50c7); L = E;  // ld l,e
  CYC(0x50c7, 0x50c8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x50c8, 0x50ca); L = 0x9d;  // ld l,Enemy.oamTileIndexBase
  CYC(0x50ca, 0x50cc); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x50cc, 0x50ce); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x50ce, 0x50d0); mem_wr(gb, HL, 0x48);  // ld (hl),$48
  CYC(0x50d0, 0x50d2); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x50d2, 0x50d4); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x50d4, 0x50d6); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x50d6, 0x50d7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x50d7, 0x50da); SET_HL(0xd00b);  // ld hl,w1Link.yh
  CYC(0x50da, 0x50dc); mem_wr(gb, HL, 0x88);  // ld (hl),$88
  CYC(0x50dc, 0x50de); L = 0x0d;  // ld l,<w1Link.xh
  CYC(0x50de, 0x50e0); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x50e0, 0x50e2); L = 0x08;  // ld l,<w1Link.direction
  CYC(0x50e2, 0x50e4); mem_wr(gb, HL, 0x00);  // ld (hl),DIR_UP
  CYC(0x50e4, 0x50e6); L = 0x00;  // ld l,<w1Link.enabled
  CYC(0x50e6, 0x50e8); mem_wr(gb, HL, 0x03);  // ld (hl),$03
  CYC(0x50e8, 0x50eb); SET_HL(0xcc08);  // ld hl,wLoadedObjectGfx
  CYC(0x50eb, 0x50ed); A = 0x01;  // ld a,$01
  CYC(0x50ed, 0x50ef); mem_wr(gb, HL, 0x16);  // ld (hl),OBJ_GFXH_16
  CYC(0x50ef, 0x50f0); L = alu_inc8(gb, L);  // inc l
  CYC(0x50f0, 0x50f1); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x50f1, 0x50f3); mem_wr(gb, HL, 0x18);  // ld (hl),OBJ_GFXH_18
  CYC(0x50f3, 0x50f4); L = alu_inc8(gb, L);  // inc l
  CYC(0x50f4, 0x50f5); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x50f5, 0x50f7); mem_wr(gb, HL, 0x19);  // ld (hl),OBJ_GFXH_19
  CYC(0x50f7, 0x50f8); L = alu_inc8(gb, L);  // inc l
  CYC(0x50f8, 0x50f9); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x50f9, 0x50fb); mem_wr(gb, HL, 0x1a);  // ld (hl),OBJ_GFXH_1a
  CYC(0x50fb, 0x50fc); L = alu_inc8(gb, L);  // inc l
  CYC(0x50fc, 0x50fd); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x50fd, 0x50ff); mem_wr(gb, HL, 0x1b);  // ld (hl),OBJ_GFXH_1b
  CYC(0x50ff, 0x5100); L = alu_inc8(gb, L);  // inc l
  CYC(0x5100, 0x5101); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5101, 0x5102); alu_xor(gb, A);  // xor a
  CYC(0x5102, 0x5103); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5103, 0x5104); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5104, 0x5105); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5105, 0x5106); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5106, 0x5107); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5107, 0x5108); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5108, 0x510b); SET_BC(0x0012);  // ld bc,$0012
  CALL_C(0x510b, enemyBoss_spawnShadow_b10_hook, 0x4534, 0x510e);
  CYC(0x510e, 0x5110); E = 0x98;  // ld e,Enemy.relatedObj2
  CYC(0x5110, 0x5112); A = 0xc0;  // ld a,Part.start
  CYC(0x5112, 0x5113); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5113, 0x5114); E = alu_inc8(gb, E);  // inc e
  CYC(0x5114, 0x5115); A = H;  // ld a,h
  CYC(0x5115, 0x5116); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x5116, disableLcd_hook, 0x02c1, 0x5119);
  CYC(0x5119, 0x511b); A = 0xf5;  // ld a,<ROOM_TWINROVA_FIGHT
  CYC(0x511b, 0x511e); mem_wr(gb, 0xcc30, A);  // ld (wActiveRoom),a
  CYC(0x511e, 0x5120); A = 0x03;  // ld a,$03
  CYC(0x5120, 0x5123); mem_wr(gb, 0xcca9, A);  // ld (wTwinrovaTileReplacementMode),a
  CALL_C(0x5123, loadScreenMusicAndSetRoomPack_hook, 0x341a, 0x5126);
  CALL_C(0x5126, loadTilesetData_hook, 0x3889, 0x5129);
  CALL_C(0x5129, loadTilesetGraphics_hook, 0x3796, 0x512c);
  CALL_C(0x512c, func_131f_hook, 0x131f, 0x512f);
  CALL_C(0x512f, resetCamera_hook, 0x12ce, 0x5132);
  CALL_C(0x5132, loadCommonGraphics_hook, 0x1a98, 0x5135);
  CYC(0x5135, 0x5137); A = 0x8b;  // ld a,PALH_8b
  CALL_C(0x5137, loadPaletteHeader_hook, 0x050b, 0x513a);
  CYC(0x513a, 0x513c); A = 0xb1;  // ld a,PALH_b1
  CYC(0x513c, 0x513f); mem_wr(gb, 0xcbe3, A);  // ld (wExtraBgPaletteHeader),a
  CYC(0x513f, 0x5141); A = 0xb0;  // ld a,GFXH_GANON_REVIVAL
  CALL_C(0x5141, loadGfxHeader_hook, 0x0626, 0x5144);
  CYC(0x5144, 0x5146); A = 0x02;  // ld a,$02
  CALL_C(0x5146, loadGfxRegisterStateIndex_hook, 0x02ea, 0x5149);
  CYC(0x5149, 0x514b); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x514b, 0x514c); D = A;  // ld d,a
  CALL_C(0x514c, objectSetVisible83_hook, 0x1e72, 0x514f);
  CYC(0x514f, 0x5152); if (hook_enabled_at(0x3299)) { fadeinFromWhite_hook(gb); return; } HANDOFF(0x3299);  // jp fadeinFromWhite
}

void ganon_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5152, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x5155);
  if (!(F & FZ)) { RET_TAKEN(0x5155); return; } CYC(0x5155, 0x5156);  // ret nz
  CYC(0x5156, 0x5158); mem_wr(gb, HL, 0x60);  // ld (hl),ENEMY_GANON_REVIVAL_CUTSCENE
  CYC(0x5158, 0x515a); L = 0x96;  // ld l,Enemy.relatedObj1
  CYC(0x515a, 0x515c); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(0x515c, 0x515d); L = alu_inc8(gb, L);  // inc l
  CYC(0x515d, 0x515e); mem_wr(gb, HL, D);  // ld (hl),d
  CALL_C(0x515e, ecom_incState_b10_hook, 0x4000, 0x5161);
  CYC(0x5161, 0x5163); L = 0x87;  // ld l,Enemy.counter2
  CYC(0x5163, 0x5165); mem_wr(gb, HL, 60);  // ld (hl),60
  RET(0x5165); return;  // ret
}

void ganon_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5166, 0x5168); E = 0x86;  // ld e,Enemy.counter1
  CYC(0x5168, 0x5169); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5169, 0x516a); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x516a); return; } CYC(0x516a, 0x516b);  // ret z
  CALL_C(0x516b, ecom_decCounter2_b10_hook, 0x43a3, 0x516e);
  if (!(F & FZ)) { CYCT(0x516e, 0x5171); if (hook_enabled_at(0x441f)) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(0x441f); } CYC(0x516e, 0x5171);  // jp nz,ecom_flickerVisibility
  CYC(0x5171, 0x5172); L = alu_dec8(gb, L);  // dec l
  CYC(0x5172, 0x5174); mem_wr(gb, HL, 193);  // ld (hl),193
  CYC(0x5174, 0x5176); L = 0x84;  // ld l,Enemy.state
  CYC(0x5176, 0x5177); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5177, 0x5179); A = 0x0d;  // ld a,$0d
  CALL_C(0x5179, enemySetAnimation_hook, 0x282b, 0x517c);
  CYC(0x517c, 0x517f); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void ganon_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x517f, ecom_decCounter1_b10_hook, 0x439a, 0x5182);
  if ((F & FZ)) { CYCT(0x5182, 0x5184); goto nextState; } CYC(0x5182, 0x5184);  // jr z,@nextState
  CYC(0x5184, 0x5185); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5185, 0x5187); alu_and(gb, 0x3f);  // and $3f
  CYC(0x5187, 0x5189); A = 0xb8;  // ld a,SND_RUMBLE2
  if ((F & FZ)) { CALL_C_CC(0x5189, playSound_b00_hook, 0x0c98, 0x518c); } else CYC(0x5189, 0x518c);  // call z,playSound
  CYC(0x518c, 0x518f); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
nextState:
  CYC(0x518f, 0x5190); L = E;  // ld l,e
  CYC(0x5190, 0x5191); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5191, 0x5193); L = 0x8f;  // ld l,$8f
  CYC(0x5193, 0x5195); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x5195, 0x5197); A = 0x02;  // ld a,$02
  CALL_C(0x5197, objectGetRelatedObject2Var_hook, 0x2164, 0x519a);
  CYC(0x519a, 0x519c); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x519c, 0x519e); A = 0x01;  // ld a,$01
  CYC(0x519e, 0x51a1); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x51a1, 0x51a3); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x51a3, 0x51a4); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x51a4, 0x51a5); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { CYCT(0x51a5, 0x51a8); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x51a5, 0x51a8);  // jp nz,enemyAnimate
  CALL_C(0x51a8, ecom_incState_b10_hook, 0x4000, 0x51ab);
  CYC(0x51ab, 0x51ad); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x51ad, 0x51af); mem_wr(gb, HL, 15);  // ld (hl),15
  CYC(0x51af, 0x51b1); A = 0xb1;  // ld a,GFXH_GANON_A
  CALL_C(0x51b1, loadGfxHeader_hook, 0x0626, 0x51b4);
  CYC(0x51b4, 0x51b6); A = 0x32;  // ld a,UNCMP_GFXH_32
  CALL_C(0x51b6, loadUncompressedGfxHeader_hook, 0x05da, 0x51b9);
  CYC(0x51b9, 0x51bc); SET_HL(0xcc0a);  // ld hl,wLoadedObjectGfx+2
  CYC(0x51bc, 0x51be); mem_wr(gb, HL, 0x17);  // ld (hl),OBJ_GFXH_17
  CYC(0x51be, 0x51bf); L = alu_inc8(gb, L);  // inc l
  CYC(0x51bf, 0x51c1); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x51c1, 0x51c3); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x51c3, 0x51c4); D = A;  // ld d,a
  CYC(0x51c4, 0x51c6); A = 0x02;  // ld a,$02
  CYC(0x51c6, 0x51c9); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51c9, ecom_decCounter1_b10_hook, 0x439a, 0x51cc);
  if (!(F & FZ)) { RET_TAKEN(0x51cc); return; } CYC(0x51cc, 0x51cd);  // ret nz
  CYC(0x51cd, 0x51cf); A = 120;  // ld a,120
  CYC(0x51cf, 0x51d0); mem_wr(gb, HL, A);  // ld (hl),a [counter1]
  CYC(0x51d0, 0x51d3); mem_wr(gb, 0xcd18, A);  // ld (wScreenShakeCounterY),a
  CYC(0x51d3, 0x51d4); L = E;  // ld l,e
  CYC(0x51d4, 0x51d5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x51d5, 0x51d7); A = 0x67;  // ld a,SND_BOSS_DEAD
  CALL_C(0x51d7, playSound_b00_hook, 0x0c98, 0x51da);
  CYC(0x51da, 0x51dc); A = 0x03;  // ld a,$03
  CALL_C(0x51dc, enemySetAnimation_hook, 0x282b, 0x51df);
  CALL_C(0x51df, showStatusBar_hook, 0x1aa4, 0x51e2);
  CYC(0x51e2, 0x51e4); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x51e4, 0x51e5); D = A;  // ld d,a
  CYC(0x51e5, 0x51e8); if (hook_enabled_at(0x3238)) { clearPaletteFadeVariablesAndRefreshPalettes_hook(gb); return; } HANDOFF(0x3238);  // jp clearPaletteFadeVariablesAndRefreshPalettes
}

void ganon_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x51e8, ecom_decCounter1_b10_hook, 0x439a, 0x51eb);
  if (!(F & FZ)) { CYCT(0x51eb, 0x51ee); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x51eb, 0x51ee);  // jp nz,enemyAnimate
  CYC(0x51ee, 0x51f0); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x51f0, 0x51f1); L = E;  // ld l,e
  CYC(0x51f1, 0x51f2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x51f2, 0x51f5); SET_HL(0xcc0e);  // ld hl,wLoadedObjectGfx+6
  CYC(0x51f5, 0x51f6); alu_xor(gb, A);  // xor a
  CYC(0x51f6, 0x51f7); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x51f7, 0x51f8); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x51f8, 0x51f9); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x51f9, 0x51fa); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x51fa, 0x51fd); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(0x51fd, 0x5200); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x5200, 0x5203); SET_BC(0x2f0d);  // ld bc,TX_2f0d
  CALL_C(0x5203, showText_hook, 0x1872, 0x5206);
  CYC(0x5206, 0x5208); A = 0x02;  // ld a,$02
  CYC(0x5208, 0x520b); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x520b, 0x520d); A = 0x34;  // ld a,MUS_GANON
  CYC(0x520d, 0x5210); mem_wr(gb, 0xcc35, A);  // ld (wActiveMusic),a
  CALL_C(0x5210, playSound_b00_hook, 0x0c98, 0x5213);
  CYC(0x5213, 0x5216); ganon_decideNextMove_hook(gb); return;  // jp ganon_decideNextMove
}

void ganon_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5216, 0x5217); E = alu_inc8(gb, E);  // inc e
  CYC(0x5217, 0x5218); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x5218, 0x5219); push_effect(gb, 0x5219);
  switch (ganon_jump_table(gb)) {
    case 0x5229: ganon_state8_substate0_hook(gb); return;
    case 0x5230: ganon_state8_substate1_hook(gb); return;
    case 0x523e: ganon_state8_substate2_hook(gb); return;
    case 0x5247: ganon_state8_substate3_hook(gb); return;
    case 0x525e: ganon_state8_substate4_hook(gb); return;
    case 0x526b: ganon_state8_substate5_hook(gb); return;
    case 0x5280: ganon_state8_substate6_hook(gb); return;
    case 0x52b0: ganon_state8_substate7_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

// Also used by state D substate0.
void ganon_state8_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5229, ganon_updateTeleportVarsAndPlaySound_hook, 0x570d, 0x522c);
  CYC(0x522c, 0x522e); L = 0x85;  // ld l,Enemy.substate
  CYC(0x522e, 0x522f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(0x522f); return;  // ret
}

// Also used by state D substate1.
void ganon_state8_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5230, ecom_decCounter1_b10_hook, 0x439a, 0x5233);
  if (!(F & FZ)) { CYCT(0x5233, 0x5236); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(0x5233, 0x5236);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(0x5236, 0x5237); L = E;  // ld l,e
  CYC(0x5237, 0x5238); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CALL_C(0x5238, ganon_decideTeleportLocationAndCounter_hook, 0x5787, 0x523b);
  CYC(0x523b, 0x523e); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

// Also used by state 9, C, D substate2.
void ganon_state8_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x523e, ecom_decCounter1_b10_hook, 0x439a, 0x5241);
  if (!(F & FZ)) { RET_TAKEN(0x5241); return; } CYC(0x5241, 0x5242);  // ret nz
  CYC(0x5242, 0x5243); L = E;  // ld l,e
  CYC(0x5243, 0x5244); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5244, 0x5247); ganon_updateTeleportVarsAndPlaySound_hook(gb); return;  // jp ganon_updateTeleportVarsAndPlaySound
}

void ganon_state8_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5247, ecom_decCounter1_b10_hook, 0x439a, 0x524a);
  if (!(F & FZ)) { CYCT(0x524a, 0x524d); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(0x524a, 0x524d);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(0x524d, 0x524f); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x524f, 0x5250); L = E;  // ld l,e
  CYC(0x5250, 0x5251); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5251, 0x5253); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x5253, 0x5254); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5254, 0x5256); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5256, 0x5257); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5257, 0x5259); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5259, 0x525b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x525b, 0x525e); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void ganon_state8_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x525e, ecom_decCounter1_b10_hook, 0x439a, 0x5261);
  if (!(F & FZ)) { RET_TAKEN(0x5261); return; } CYC(0x5261, 0x5262);  // ret nz
  CYC(0x5262, 0x5264); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x5264, 0x5265); L = E;  // ld l,e
  CYC(0x5265, 0x5266); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5266, 0x5268); A = 0xb3;  // ld a,GFXH_GANON_C
  CYC(0x5268, 0x526b); ganon_loadGfxHeader_hook(gb); return;  // jp ganon_loadGfxHeader
}

void ganon_state8_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x526b, ecom_decCounter1_b10_hook, 0x439a, 0x526e);
  if (!(F & FZ)) { RET_TAKEN(0x526e); return; } CYC(0x526e, 0x526f);  // ret nz
  CYC(0x526f, 0x5271); mem_wr(gb, HL, 45);  // ld (hl),45
  CYC(0x5271, 0x5272); L = E;  // ld l,e
  CYC(0x5272, 0x5273); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5273, 0x5275); A = 0x05;  // ld a,$05
  CALL_C(0x5275, enemySetAnimation_hook, 0x282b, 0x5278);
  CALL_C(0x5278, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x527b);
  CYC(0x527b, 0x527e); SET_BC(0x003c);  // ldbc $00,SPEED_180
  CYC(0x527e, 0x5280); ganon_state8_spawnProjectile_hook(gb); return;  // jr ganon_state8_spawnProjectile
}

void ganon_state8_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5280, ecom_decCounter1_b10_hook, 0x439a, 0x5283);
  if (!(F & FZ)) { CYCT(0x5283, 0x5285); goto checkAttackDone; } CYC(0x5283, 0x5285);  // jr nz,+
  CYC(0x5285, 0x5287); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x5287, 0x5288); L = E;  // ld l,e
  CYC(0x5288, 0x5289); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5289, 0x528b); A = 0x02;  // ld a,$02
  CYC(0x528b, 0x528e); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
checkAttackDone:
  CYC(0x528e, 0x528f); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x528f, 0x5291); alu_cp(gb, 0x19);  // cp $19
  if (!(F & FZ)) { RET_TAKEN(0x5291); return; } CYC(0x5291, 0x5292);  // ret nz
  CYC(0x5292, 0x5295); SET_BC(0x0264);  // ldbc $02,SPEED_280
  CALL_C(0x5295, ganon_state8_spawnProjectile_hook, 0x529b, 0x5298);
  CYC(0x5298, 0x529b); SET_BC(0xfe64);  // ldbc $fe,SPEED_280
  ganon_state8_spawnProjectile_hook(gb); return;  // fallthrough
}

void ganon_state8_spawnProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x529b, 0x529d); E = 0x52;  // ld e,PART_52
  CALL_C(0x529d, ganon_spawnPart_hook, 0x57c4, 0x52a0);
  if (!(F & FZ)) { RET_TAKEN(0x52a0); return; } CYC(0x52a0, 0x52a1);  // ret nz
  CYC(0x52a1, 0x52a3); L = 0xc9;  // ld l,Part.angle
  CYC(0x52a3, 0x52a5); E = 0x89;  // ld e,Enemy.angle
  CYC(0x52a5, 0x52a6); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x52a6, 0x52a7); alu_add(gb, B);  // add b
  CYC(0x52a7, 0x52a9); alu_and(gb, 0x1f);  // and $1f
  CYC(0x52a9, 0x52aa); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x52aa, 0x52ac); L = 0xd0;  // ld l,Part.speed
  CYC(0x52ac, 0x52ad); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(0x52ad, 0x52b0); if (hook_enabled_at(0x2242)) { objectCopyPosition_hook(gb); return; } HANDOFF(0x2242);  // jp objectCopyPosition
}

void ganon_state8_substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52b0, ecom_decCounter1_b10_hook, 0x439a, 0x52b3);
  if (!(F & FZ)) { RET_TAKEN(0x52b3); return; } CYC(0x52b3, 0x52b4);  // ret nz
  CYC(0x52b4, 0x52b7); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x52b7, 0x52b8); E = alu_inc8(gb, E);  // inc e
  CYC(0x52b8, 0x52b9); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x52b9, 0x52ba); push_effect(gb, 0x52ba);
  switch (ganon_jump_table(gb)) {
    case 0x52ca: ganon_state9_substate0_hook(gb); return;
    case 0x52d1: ganon_state9_substate1_hook(gb); return;
    case 0x523e: ganon_state8_substate2_hook(gb); return;
    case 0x52e6: ganon_state9_substate3_hook(gb); return;
    case 0x52f7: ganon_state9_substate4_hook(gb); return;
    case 0x532e: ganon_state9_substate5_hook(gb); return;
    case 0x5340: ganon_state9_substate6_hook(gb); return;
    case 0x534d: ganon_state9_substate7_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

// Also used by state A, B, C substate0.
void ganon_state9_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52ca, ganon_updateTeleportVarsAndPlaySound_hook, 0x570d, 0x52cd);
  CYC(0x52cd, 0x52cf); L = 0x85;  // ld l,Enemy.substate
  CYC(0x52cf, 0x52d0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(0x52d0); return;  // ret
}

void ganon_state9_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52d1, ecom_decCounter1_b10_hook, 0x439a, 0x52d4);
  if (!(F & FZ)) { CYCT(0x52d4, 0x52d7); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(0x52d4, 0x52d7);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(0x52d7, 0x52d9); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x52d9, 0x52da); L = E;  // ld l,e
  CYC(0x52da, 0x52db); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x52db, 0x52dd); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x52dd, 0x52df); mem_wr(gb, HL, 0x58);  // ld (hl),$58
  CYC(0x52df, 0x52e1); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x52e1, 0x52e3); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x52e3, 0x52e6); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

void ganon_state9_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52e6, ecom_decCounter1_b10_hook, 0x439a, 0x52e9);
  if (!(F & FZ)) { CYCT(0x52e9, 0x52ec); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(0x52e9, 0x52ec);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(0x52ec, 0x52ee); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x52ee, 0x52ef); L = E;  // ld l,e
  CYC(0x52ef, 0x52f0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x52f0, 0x52f2); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x52f2, 0x52f4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x52f4, 0x52f7); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void ganon_state9_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52f7, ecom_decCounter1_b10_hook, 0x439a, 0x52fa);
  if (!(F & FZ)) { RET_TAKEN(0x52fa); return; } CYC(0x52fa, 0x52fb);  // ret nz
  CYC(0x52fb, 0x52fd); mem_wr(gb, HL, 0x28);  // ld (hl),$28
  CYC(0x52fd, 0x52fe); L = E;  // ld l,e
  CYC(0x52fe, 0x52ff); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x52ff, 0x5301); A = 0xb6;  // ld a,GFXH_GANON_F
  CALL_C(0x5301, ganon_loadGfxHeader_hook, 0x56de, 0x5304);
  CYC(0x5304, 0x5306); A = 0x09;  // ld a,$09
  CALL_C(0x5306, enemySetAnimation_hook, 0x282b, 0x5309);
  CYC(0x5309, 0x530b); B = 0x1c;  // ld b,$1c
  CYC(0x530b, 0x530e); push_effect(gb, 0x530e); ganon_state9_substate4__spawnProjectile_hook(gb);  // call @spawnProjectile
  CYC(0x530e, 0x5310); B = 0x14;  // ld b,$14
  CYC(0x5310, 0x5313); push_effect(gb, 0x5313); ganon_state9_substate4__spawnProjectile_hook(gb);  // call @spawnProjectile
  CYC(0x5313, 0x5315); B = 0x0c;  // ld b,$0c
  CYC(0x5315, 0x5318); push_effect(gb, 0x5318); ganon_state9_substate4__spawnProjectile_hook(gb);  // call @spawnProjectile
  CYC(0x5318, 0x531a); B = 0x04;  // ld b,$04
  ganon_state9_substate4__spawnProjectile_hook(gb); return;  // fallthrough
}

// 10:531a (real call from three sites above, needs genuine continuation, and also reached
// by fallthrough; extracted as a plain non-hooked helper).
void ganon_state9_substate4__spawnProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x531a, 0x531c); E = 0x52;  // ld e,PART_52
  CALL_C(0x531c, ganon_spawnPart_hook, 0x57c4, 0x531f);
  CYC(0x531f, 0x5321); L = 0xc2;  // ld l,Part.subid
  CYC(0x5321, 0x5322); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [subid]=1
  CYC(0x5322, 0x5324); L = 0xc9;  // ld l,Part.angle
  CYC(0x5324, 0x5325); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5325, 0x5327); L = 0xd7;  // ld l,Part.relatedObj1+1
  CYC(0x5327, 0x5328); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(0x5328, 0x5329); L = alu_dec8(gb, L);  // dec l
  CYC(0x5329, 0x532b); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(0x532b, 0x532e); if (hook_enabled_at(0x2242)) { objectCopyPosition_hook(gb); return; } HANDOFF(0x2242);  // jp objectCopyPosition
}

void ganon_state9_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x532e, ecom_decCounter1_b10_hook, 0x439a, 0x5331);
  if (!(F & FZ)) { RET_TAKEN(0x5331); return; } CYC(0x5331, 0x5332);  // ret nz
  CYC(0x5332, 0x5334); mem_wr(gb, HL, 0x28);  // ld (hl),$28
  CYC(0x5334, 0x5335); L = E;  // ld l,e
  CYC(0x5335, 0x5336); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5336, 0x5338); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(0x5338, ganon_loadGfxHeader_hook, 0x56de, 0x533b);
  CYC(0x533b, 0x533d); A = 0x07;  // ld a,$07
  CYC(0x533d, 0x5340); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_state9_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5340, ecom_decCounter1_b10_hook, 0x439a, 0x5343);
  if (!(F & FZ)) { RET_TAKEN(0x5343); return; } CYC(0x5343, 0x5344);  // ret nz
  CYC(0x5344, 0x5346); mem_wr(gb, HL, 80);  // ld (hl),80
  CYC(0x5346, 0x5347); L = E;  // ld l,e
  CYC(0x5347, 0x5348); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5348, 0x534a); A = 0x02;  // ld a,$02
  CYC(0x534a, 0x534d); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_state9_substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x534d, ecom_decCounter1_b10_hook, 0x439a, 0x5350);
  if (!(F & FZ)) { RET_TAKEN(0x5350); return; } CYC(0x5350, 0x5351);  // ret nz
  CYC(0x5351, 0x5354); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5354, 0x5355); E = alu_inc8(gb, E);  // inc e
  CYC(0x5355, 0x5356); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x5356, 0x5357); push_effect(gb, 0x5357);
  switch (ganon_jump_table(gb)) {
    case 0x52ca: ganon_state9_substate0_hook(gb); return;
    case 0x5367: ganon_stateA_substate1_hook(gb); return;
    case 0x5374: ganon_stateA_substate2_hook(gb); return;
    case 0x53a7: ganon_stateA_substate3_hook(gb); return;
    case 0x53c4: ganon_stateA_substate4_hook(gb); return;
    case 0x53d9: ganon_stateA_substate5_hook(gb); return;
    case 0x53ef: ganon_stateA_substate6_hook(gb); return;
    case 0x540d: ganon_stateA_substate7_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

// Teleporting out.
void ganon_stateA_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5367, ecom_decCounter1_b10_hook, 0x439a, 0x536a);
  if (!(F & FZ)) { CYCT(0x536a, 0x536d); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(0x536a, 0x536d);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(0x536d, 0x536f); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x536f, 0x5370); L = E;  // ld l,e
  CYC(0x5370, 0x5371); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5371, 0x5374); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

// Delay before reappearing.
void ganon_stateA_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5374, ecom_decCounter1_b10_hook, 0x439a, 0x5377);
  if (!(F & FZ)) { RET_TAKEN(0x5377); return; } CYC(0x5377, 0x5378);  // ret nz
  CYC(0x5378, 0x5379); L = E;  // ld l,e
  CYC(0x5379, 0x537a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x537a, 0x537c); A = mem_rd(gb, 0xffb1);  // ldh a,(<hEnemyTargetX)
  CYC(0x537c, 0x537e); alu_cp(gb, 0x78);  // cp (LARGE_ROOM_WIDTH<<4)/2
  CYC(0x537e, 0x5381); SET_BC(0x0328);  // ldbc $03,$28
  if ((F & FC)) { CYCT(0x5381, 0x5383); goto haveBC; } CYC(0x5381, 0x5383);  // jr c,+
  CYC(0x5383, 0x5386); SET_BC(0x00d8);  // ldbc $00,-$28
haveBC:
  CYC(0x5386, 0x5388); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x5388, 0x5389); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5389, 0x538a); alu_add(gb, C);  // add c
  CYC(0x538a, 0x538c); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x538c, 0x538d); mem_wr(gb, HL, A); SET_HL(HL - 1);  // ldd (hl),a
  CYC(0x538d, 0x538f); A = mem_rd(gb, 0xffb0);  // ldh a,(<hEnemyTargetY)
  CYC(0x538f, 0x5391); alu_cp(gb, 0x30);  // cp $30
  if ((F & FC)) { CYCT(0x5391, 0x5393); goto setYh; } CYC(0x5391, 0x5393);  // jr c,+
  CYC(0x5393, 0x5395); alu_sub(gb, 0x18);  // sub $18
setYh:
  CYC(0x5395, 0x5396); L = alu_dec8(gb, L);  // dec l
  CYC(0x5396, 0x5397); mem_wr(gb, HL, A);  // ld (hl),a [yh]
  CYC(0x5397, 0x5399); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(0x5399, ganon_loadGfxHeader_hook, 0x56de, 0x539c);
  CYC(0x539c, 0x539e); E = 0xb2;  // ld e,Enemy.var32
  CYC(0x539e, 0x539f); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x539f, 0x53a1); alu_add(gb, 0x07);  // add $07
  CALL_C(0x53a1, enemySetAnimation_hook, 0x282b, 0x53a4);
  CYC(0x53a4, 0x53a7); ganon_updateTeleportVarsAndPlaySound_hook(gb); return;  // jp ganon_updateTeleportVarsAndPlaySound
}

void ganon_stateA_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x53a7, ecom_decCounter1_b10_hook, 0x439a, 0x53aa);
  if (!(F & FZ)) { CYCT(0x53aa, 0x53ad); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(0x53aa, 0x53ad);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(0x53ad, 0x53af); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x53af, 0x53b0); L = E;  // ld l,e
  CYC(0x53b0, 0x53b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x53b1, 0x53b3); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x53b3, 0x53b5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x53b5, 0x53b7); L = 0x90;  // ld l,Enemy.speed
  CYC(0x53b7, 0x53b9); mem_wr(gb, HL, 0x78);  // ld (hl),SPEED_300
  CALL_C(0x53b9, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x53bc);
  CYC(0x53bc, 0x53be); E = 0x50;  // ld e,PART_GANON_TRIDENT
  CALL_C(0x53be, ganon_spawnPart_hook, 0x57c4, 0x53c1);
  CYC(0x53c1, 0x53c4); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void ganon_stateA_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x53c4, ecom_decCounter1_b10_hook, 0x439a, 0x53c7);
  if (!(F & FZ)) { RET_TAKEN(0x53c7); return; } CYC(0x53c7, 0x53c8);  // ret nz
  CYC(0x53c8, 0x53ca); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(0x53ca, 0x53cb); L = E;  // ld l,e
  CYC(0x53cb, 0x53cc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x53cc, 0x53ce); A = 0xb7;  // ld a,GFXH_GANON_G
  CALL_C(0x53ce, ganon_loadGfxHeader_hook, 0x56de, 0x53d1);
  CYC(0x53d1, 0x53d3); E = 0xb2;  // ld e,Enemy.var32
  CYC(0x53d3, 0x53d4); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x53d4, 0x53d6); alu_add(gb, 0x08);  // add $08
  CALL_C(0x53d6, enemySetAnimation_hook, 0x282b, 0x53d9);
  ganon_stateA_substate5_hook(gb); return;  // fallthrough
}

void ganon_stateA_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x53d9, ecom_decCounter1_b10_hook, 0x439a, 0x53dc);
  if (!(F & FZ)) { CYCT(0x53dc, 0x53de); goto checkBounds; } CYC(0x53dc, 0x53de);  // jr nz,+++
  CYC(0x53de, 0x53e0); mem_wr(gb, HL, 16);  // ld (hl),16
  CYC(0x53e0, 0x53e1); L = E;  // ld l,e
  CYC(0x53e1, 0x53e2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x53e2, 0x53e4); A = 0xb6;  // ld a,GFXH_GANON_F
  CALL_C(0x53e4, ganon_loadGfxHeader_hook, 0x56de, 0x53e7);
  CYC(0x53e7, 0x53e9); E = 0xb2;  // ld e,Enemy.var32
  CYC(0x53e9, 0x53ea); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x53ea, 0x53ec); alu_add(gb, 0x09);  // add $09
  CYC(0x53ec, 0x53ef); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
checkBounds:
  CYC(0x53fa, 0x53fc); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x53fc, 0x53fd); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x53fd, 0x53ff); alu_sub(gb, 0x18);  // sub $18
  CYC(0x53ff, 0x5401); alu_cp(gb, 0x80);  // cp $80
  if (!(F & FC)) { RET_TAKEN(0x5401); return; } CYC(0x5401, 0x5402);  // ret nc
  CYC(0x5402, 0x5404); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5404, 0x5405); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5405, 0x5407); alu_sub(gb, 0x18);  // sub $18
  CYC(0x5407, 0x5409); alu_cp(gb, 0xc0);  // cp $c0
  if (!(F & FC)) { RET_TAKEN(0x5409); return; } CYC(0x5409, 0x540a);  // ret nc
  CYC(0x540a, 0x540d); if (hook_enabled_at(0x201d)) { objectApplySpeed_hook(gb); return; } HANDOFF(0x201d);  // jp objectApplySpeed
}

void ganon_stateA_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x53ef, ecom_decCounter1_b10_hook, 0x439a, 0x53f2);
  if (!(F & FZ)) { CYCT(0x53f2, 0x53f4); goto checkBounds; } CYC(0x53f2, 0x53f4);  // jr nz,+++
  CYC(0x53f4, 0x53f5); L = E;  // ld l,e
  CYC(0x53f5, 0x53f6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x53f6, 0x53f7); L = alu_inc8(gb, L);  // inc l
  CYC(0x53f7, 0x53f9); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(0x53f9); return;  // ret
checkBounds:
  CYC(0x53fa, 0x53fc); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x53fc, 0x53fd); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x53fd, 0x53ff); alu_sub(gb, 0x18);  // sub $18
  CYC(0x53ff, 0x5401); alu_cp(gb, 0x80);  // cp $80
  if (!(F & FC)) { RET_TAKEN(0x5401); return; } CYC(0x5401, 0x5402);  // ret nc
  CYC(0x5402, 0x5404); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5404, 0x5405); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5405, 0x5407); alu_sub(gb, 0x18);  // sub $18
  CYC(0x5407, 0x5409); alu_cp(gb, 0xc0);  // cp $c0
  if (!(F & FC)) { RET_TAKEN(0x5409); return; } CYC(0x5409, 0x540a);  // ret nc
  CYC(0x540a, 0x540d); if (hook_enabled_at(0x201d)) { objectApplySpeed_hook(gb); return; } HANDOFF(0x201d);  // jp objectApplySpeed
}

void ganon_stateA_substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x540d, ecom_decCounter1_b10_hook, 0x439a, 0x5410);
  if (!(F & FZ)) { RET_TAKEN(0x5410); return; } CYC(0x5410, 0x5411);  // ret nz
  CYC(0x5411, 0x5413); A = 0x02;  // ld a,$02
  CALL_C(0x5413, enemySetAnimation_hook, 0x282b, 0x5416);
  CYC(0x5416, 0x5419); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5419, 0x541a); E = alu_inc8(gb, E);  // inc e
  CYC(0x541a, 0x541b); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x541b, 0x541c); push_effect(gb, 0x541c);
  switch (ganon_jump_table(gb)) {
    case 0x52ca: ganon_state9_substate0_hook(gb); return;
    case 0x5432: ganon_stateB_substate1_hook(gb); return;
    case 0x543f: ganon_stateB_substate2_hook(gb); return;
    case 0x545a: ganon_stateB_substate3_hook(gb); return;
    case 0x5477: ganon_stateB_substate4_hook(gb); return;
    case 0x548e: ganon_stateB_substate5_hook(gb); return;
    case 0x54b6: ganon_stateB_substate6_hook(gb); return;
    case 0x54db: ganon_stateB_substate7_hook(gb); return;
    case 0x54f2: ganon_stateB_substate8_hook(gb); return;
    case 0x5507: ganon_stateB_substate9_hook(gb); return;
    case 0x5514: ganon_stateB_substateA_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ganon_stateB_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5432, ecom_decCounter1_b10_hook, 0x439a, 0x5435);
  if (!(F & FZ)) { CYCT(0x5435, 0x5438); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(0x5435, 0x5438);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(0x5438, 0x543a); mem_wr(gb, HL, 180);  // ld (hl),180
  CYC(0x543a, 0x543b); L = E;  // ld l,e
  CYC(0x543b, 0x543c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x543c, 0x543f); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

void ganon_stateB_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x543f, ecom_decCounter1_b10_hook, 0x439a, 0x5442);
  if (!(F & FZ)) { RET_TAKEN(0x5442); return; } CYC(0x5442, 0x5443);  // ret nz
  CYC(0x5443, 0x5444); L = E;  // ld l,e
  CYC(0x5444, 0x5445); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5445, 0x5447); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5447, 0x5449); mem_wr(gb, HL, 0x28);  // ld (hl),$28
  CYC(0x5449, 0x544b); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x544b, 0x544d); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x544d, 0x544f); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(0x544f, ganon_loadGfxHeader_hook, 0x56de, 0x5452);
  CYC(0x5452, 0x5454); A = 0x04;  // ld a,$04
  CALL_C(0x5454, enemySetAnimation_hook, 0x282b, 0x5457);
  CYC(0x5457, 0x545a); ganon_updateTeleportVarsAndPlaySound_hook(gb); return;  // jp ganon_updateTeleportVarsAndPlaySound
}

void ganon_stateB_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x545a, ecom_decCounter1_b10_hook, 0x439a, 0x545d);
  if (!(F & FZ)) { CYCT(0x545d, 0x5460); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(0x545d, 0x5460);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(0x5460, 0x5462); mem_wr(gb, HL, 0x40);  // ld (hl),$40
  CYC(0x5462, 0x5463); L = E;  // ld l,e
  CYC(0x5463, 0x5464); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5464, 0x5466); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5466, 0x5468); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x5468, objectSetVisible83_hook, 0x1e72, 0x546b);
  CYC(0x546b, 0x546d); E = 0x51;  // ld e,PART_51
  CALL_C(0x546d, ganon_spawnPart_hook, 0x57c4, 0x5470);
  if (!(F & FZ)) { RET_TAKEN(0x5470); return; } CYC(0x5470, 0x5471);  // ret nz
  CYC(0x5471, 0x5474); SET_BC(0xf810);  // ld bc,$f810
  CYC(0x5474, 0x5477); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
}

void ganon_stateB_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5477, ecom_decCounter1_b10_hook, 0x439a, 0x547a);
  if (!(F & FZ)) { RET_TAKEN(0x547a); return; } CYC(0x547a, 0x547b);  // ret nz
  CYC(0x547b, 0x547c); L = E;  // ld l,e
  CYC(0x547c, 0x547d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x547d, 0x547f); L = 0x94;  // ld l,Enemy.speedZ
  CYC(0x547f, 0x5481); A = 0x40;  // ld a,<(-$1c0)
  CYC(0x5481, 0x5482); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5482, 0x5484); mem_wr(gb, HL, 0xfe);  // ld (hl),>(-$1c0)
  CYC(0x5484, 0x5486); A = 0xb3;  // ld a,GFXH_GANON_C
  CALL_C(0x5486, ganon_loadGfxHeader_hook, 0x56de, 0x5489);
  CYC(0x5489, 0x548b); A = 0x05;  // ld a,$05
  CYC(0x548b, 0x548e); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_stateB_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x548e, 0x5490); C = 0x20;  // ld c,$20
  CALL_C(0x5490, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5493);
  if ((F & FZ)) { CYCT(0x5493, 0x5495); goto shakeAndAdvance; } CYC(0x5493, 0x5495);  // jr z,++
  CYC(0x5495, 0x5496); A = mem_rd(gb, HL); SET_HL(HL - 1);  // ldd a,(hl)
  CYC(0x5496, 0x5497); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x5497); return; } CYC(0x5497, 0x5498);  // ret nz
  CYC(0x5498, 0x5499); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5499, 0x549b); alu_cp(gb, 0xc0);  // cp $c0
  if (!(F & FZ)) { RET_TAKEN(0x549b); return; } CYC(0x549b, 0x549c);  // ret nz
  CYC(0x549c, 0x549e); A = 0xb5;  // ld a,GFXH_GANON_E
  CALL_C(0x549e, ganon_loadGfxHeader_hook, 0x56de, 0x54a1);
  CYC(0x54a1, 0x54a3); A = 0x06;  // ld a,$06
  CYC(0x54a3, 0x54a6); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
shakeAndAdvance:
  CYC(0x54a6, 0x54a8); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x54a8, 0x54aa); A = 120;  // ld a,120
  CYC(0x54aa, 0x54ab); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x54ab, 0x54ae); mem_wr(gb, 0xcd18, A);  // ld (wScreenShakeCounterY),a
  CYC(0x54ae, 0x54b0); L = 0x85;  // ld l,Enemy.substate
  CYC(0x54b0, 0x54b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x54b1, 0x54b3); A = 0x6f;  // ld a,SND_EXPLOSION
  CYC(0x54b3, 0x54b6); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void ganon_stateB_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54b6, ecom_decCounter1_b10_hook, 0x439a, 0x54b9);
  if ((F & FZ)) { CYCT(0x54b9, 0x54bb); goto nextState; } CYC(0x54b9, 0x54bb);  // jr z,++
  CYC(0x54bb, 0x54bc); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x54bc, 0x54be); alu_cp(gb, 105);  // cp 105
  if ((F & FC)) { RET_TAKEN(0x54be); return; } CYC(0x54be, 0x54bf);  // ret c
  CYC(0x54bf, 0x54c2); A = mem_rd(gb, 0xd00f);  // ld a,(w1Link.zh)
  CYC(0x54c2, 0x54c3); alu_rlca(gb);  // rlca
  if ((F & FC)) { RET_TAKEN(0x54c3); return; } CYC(0x54c3, 0x54c4);  // ret c
  CYC(0x54c4, 0x54c7); SET_HL(0xcc4f);  // ld hl,wLinkForceState
  CYC(0x54c7, 0x54c9); A = 0x14;  // ld a,LINK_STATE_COLLAPSED
  CYC(0x54c9, 0x54ca); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x54ca, 0x54cc); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  RET(0x54cc); return;  // ret
nextState:
  CYC(0x54cd, 0x54cf); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(0x54cf, 0x54d0); L = E;  // ld l,e
  CYC(0x54d0, 0x54d1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x54d1, 0x54d3); A = 0xb2;  // ld a,GFXH_GANON_B
  CALL_C(0x54d3, ganon_loadGfxHeader_hook, 0x56de, 0x54d6);
  CYC(0x54d6, 0x54d8); A = 0x04;  // ld a,$04
  CYC(0x54d8, 0x54db); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_stateB_substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54db, ecom_decCounter1_b10_hook, 0x439a, 0x54de);
  if (!(F & FZ)) { RET_TAKEN(0x54de); return; } CYC(0x54de, 0x54df);  // ret nz
  CYC(0x54df, 0x54e1); mem_wr(gb, HL, 0x18);  // ld (hl),$18
  CYC(0x54e1, 0x54e2); L = E;  // ld l,e
  CYC(0x54e2, 0x54e3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x54e3, 0x54e5); E = 0x51;  // ld e,PART_51
  CALL_C(0x54e5, ganon_spawnPart_hook, 0x57c4, 0x54e8);
  if (!(F & FZ)) { RET_TAKEN(0x54e8); return; } CYC(0x54e8, 0x54e9);  // ret nz
  CYC(0x54e9, 0x54eb); L = 0xc2;  // ld l,Part.subid
  CYC(0x54eb, 0x54ec); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x54ec, 0x54ef); SET_BC(0xf810);  // ld bc,$f810
  CYC(0x54ef, 0x54f2); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
}

void ganon_stateB_substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x54f2, ecom_decCounter1_b10_hook, 0x439a, 0x54f5);
  if (!(F & FZ)) { RET_TAKEN(0x54f5); return; } CYC(0x54f5, 0x54f6);  // ret nz
  CYC(0x54f6, 0x54f8); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x54f8, 0x54f9); L = E;  // ld l,e
  CYC(0x54f9, 0x54fa); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CALL_C(0x54fa, objectCreatePuff_hook, 0x24c1, 0x54fd);
  CYC(0x54fd, 0x54ff); A = 0xb3;  // ld a,GFXH_GANON_C
  CALL_C(0x54ff, ganon_loadGfxHeader_hook, 0x56de, 0x5502);
  CYC(0x5502, 0x5504); A = 0x05;  // ld a,$05
  CYC(0x5504, 0x5507); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_stateB_substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5507, ecom_decCounter1_b10_hook, 0x439a, 0x550a);
  if (!(F & FZ)) { RET_TAKEN(0x550a); return; } CYC(0x550a, 0x550b);  // ret nz
  CYC(0x550b, 0x550d); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x550d, 0x550e); L = E;  // ld l,e
  CYC(0x550e, 0x550f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x550f, 0x5511); A = 0x02;  // ld a,$02
  CYC(0x5511, 0x5514); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_stateB_substateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5514, ecom_decCounter1_b10_hook, 0x439a, 0x5517);
  if (!(F & FZ)) { RET_TAKEN(0x5517); return; } CYC(0x5517, 0x5518);  // ret nz
  CYC(0x5518, 0x551b); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x551b, 0x551c); E = alu_inc8(gb, E);  // inc e
  CYC(0x551c, 0x551d); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x551d, 0x551e); push_effect(gb, 0x551e);
  switch (ganon_jump_table(gb)) {
    case 0x52ca: ganon_state9_substate0_hook(gb); return;
    case 0x5534: ganon_stateC_substate1_hook(gb); return;
    case 0x523e: ganon_state8_substate2_hook(gb); return;
    case 0x5549: ganon_stateC_substate3_hook(gb); return;
    case 0x555f: ganon_stateC_substate4_hook(gb); return;
    case 0x5589: ganon_stateC_substate5_hook(gb); return;
    case 0x5599: ganon_stateC_substate6_hook(gb); return;
    case 0x55bf: ganon_stateC_substate7_hook(gb); return;
    case 0x55f7: ganon_stateC_substate8_hook(gb); return;
    case 0x562d: ganon_stateC_substate9_hook(gb); return;
    case 0x5642: ganon_stateC_substateA_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ganon_stateC_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5534, ecom_decCounter1_b10_hook, 0x439a, 0x5537);
  if (!(F & FZ)) { CYCT(0x5537, 0x553a); ganon_updateTeleportAnimationGoingOut_hook(gb); return; } CYC(0x5537, 0x553a);  // jp nz,ganon_updateTeleportAnimationGoingOut
  CYC(0x553a, 0x553c); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x553c, 0x553d); L = E;  // ld l,e
  CYC(0x553d, 0x553e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x553e, 0x5540); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5540, 0x5542); mem_wr(gb, HL, 0x58);  // ld (hl),$58
  CYC(0x5542, 0x5544); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5544, 0x5546); mem_wr(gb, HL, 0x78);  // ld (hl),$78
  CYC(0x5546, 0x5549); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

void ganon_stateC_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5549, ecom_decCounter1_b10_hook, 0x439a, 0x554c);
  if (!(F & FZ)) { CYCT(0x554c, 0x554f); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(0x554c, 0x554f);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(0x554f, 0x5551); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x5551, 0x5552); L = E;  // ld l,e
  CYC(0x5552, 0x5553); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5553, 0x5555); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5555, 0x5557); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x5557, objectSetVisible83_hook, 0x1e72, 0x555a);
  CYC(0x555a, 0x555c); A = 0xb4;  // ld a,SND_FADEOUT
  CYC(0x555c, 0x555f); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void ganon_stateC_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x555f, ecom_decCounter1_b10_hook, 0x439a, 0x5562);
  if ((F & FZ)) { CYCT(0x5562, 0x5564); goto nextSubstate; } CYC(0x5562, 0x5564);  // jr z,@nextSubstate
  CYC(0x5564, 0x5565); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5565, 0x5567); alu_cp(gb, 60);  // cp 60
  if (!(F & FC)) { RET_TAKEN(0x5567); return; } CYC(0x5567, 0x5568);  // ret nc
  CYC(0x5568, 0x556a); alu_and(gb, 0x03);  // and $03
  if (!(F & FZ)) { RET_TAKEN(0x556a); return; } CYC(0x556a, 0x556b);  // ret nz
  CYC(0x556b, 0x556d); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x556d, 0x556e); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x556e, 0x5570); alu_xor(gb, 0x05);  // xor $05
  CYC(0x5570, 0x5571); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5571, 0x5572); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x5572); return;  // ret
nextSubstate:
  CYC(0x5573, 0x5575); L = 0xa9;  // ld l,Enemy.health
  CYC(0x5575, 0x5576); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5576, 0x5577); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x5577); return; } CYC(0x5577, 0x5578);  // ret z
  CYC(0x5578, 0x5579); L = E;  // ld l,e
  CYC(0x5579, 0x557a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x557a, 0x557c); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x557c, 0x557e); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x557e, 0x5580); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x5580, 0x5582); A = 0x01;  // ld a,$01
  CYC(0x5582, 0x5583); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5583, 0x5584); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5584, 0x5586); A = 0x02;  // ld a,$02
  CYC(0x5586, 0x5589); if (hook_enabled_at(0x32ab)) { fadeoutToBlackWithDelay_hook(gb); return; } HANDOFF(0x32ab);  // jp fadeoutToBlackWithDelay
}

void ganon_stateC_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5589, 0x558c); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x558c, 0x558d); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x558d); return; } CYC(0x558d, 0x558e);  // ret nz
  CYC(0x558e, 0x5590); A = 0x06;  // ld a,$06
  CYC(0x5590, 0x5591); mem_wr(gb, DE, A);  // ld (de),a [substate]
  CYC(0x5591, 0x5593); A = 0x04;  // ld a,$04
  CALL_C(0x5593, ganon_setTileReplacementMode_hook, 0x581c, 0x5596);
  CYC(0x5596, 0x5599); ganon_makeRoomBoundarySolid_hook(gb); return;  // jp ganon_makeRoomBoundarySolid
}

void ganon_stateC_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5599, 0x559a); H = D;  // ld h,d
  CYC(0x559a, 0x559b); L = E;  // ld l,e
  CYC(0x559b, 0x559c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x559c, 0x559d); L = alu_inc8(gb, L);  // inc l
  CYC(0x559d, 0x559f); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x559f, 0x55a1); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x55a1, 0x55a3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x55a3, 0x55a5); L = 0x90;  // ld l,Enemy.speed
  CYC(0x55a5, 0x55a7); mem_wr(gb, HL, 20);  // ld (hl),SPEED_80
  CALL_C(0x55a7, getRandomNumber_noPreserveVars_hook, 0x0453, 0x55aa);
  CYC(0x55aa, 0x55ac); alu_and(gb, 0x07);  // and $07
  CYC(0x55ac, 0x55af); SET_HL(0x55b7);  // ld hl,@counter2Vals
  CYC(0x55af, 0x55b0);
  ganon_addAToHl_from_rst(gb, 0x55b0);  // rst_addAToHl
  CYC(0x55b0, 0x55b2); E = 0x87;  // ld e,Enemy.counter2
  CYC(0x55b2, 0x55b3); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x55b3, 0x55b4); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x55b4, 0x55b7); if (hook_enabled_at(0x32e6)) { fadeinFromBlack_hook(gb); return; } HANDOFF(0x32e6);  // jp fadeinFromBlack
}

void ganon_stateC_substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55bf, 0x55c1); A = 0x02;  // ld a,$02
  CYC(0x55c1, 0x55c4); mem_wr(gb, 0xcbc3, A);  // ld (wUseSimulatedInput),a
  CYC(0x55c4, 0x55c7); A = mem_rd(gb, 0xcc00);  // ld a,(wFrameCounter)
  CYC(0x55c7, 0x55c9); alu_and(gb, 0x03);  // and $03
  if (!(F & FZ)) { CYCT(0x55c9, 0x55cb); goto checkCounter1; } CYC(0x55c9, 0x55cb);  // jr nz,+
  CALL_C(0x55cb, ecom_decCounter2_b10_hook, 0x43a3, 0x55ce);
  if (!(F & FZ)) { CYCT(0x55ce, 0x55d0); goto checkCounter1; } CYC(0x55ce, 0x55d0);  // jr nz,+
  CYC(0x55d0, 0x55d2); L = 0x85;  // ld l,Enemy.substate
  CYC(0x55d2, 0x55d3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x55d3, 0x55d4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [substate]=9
  CYC(0x55d4, 0x55d6); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x55d6, 0x55d8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x55d8, 0x55db); if (hook_enabled_at(0x3263)) { fastFadeoutToWhite_hook(gb); return; } HANDOFF(0x3263);  // jp fastFadeoutToWhite
checkCounter1:
  CALL_C(0x55db, ecom_decCounter1_b10_hook, 0x439a, 0x55de);
  if (!(F & FZ)) { CYCT(0x55de, 0x55e0); goto loadGfx; } CYC(0x55de, 0x55e0);  // jr nz,+
  CYC(0x55e0, 0x55e1); L = E;  // ld l,e
  CYC(0x55e1, 0x55e2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [substate]=8
  CYC(0x55e2, 0x55e4); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x55e4, 0x55e6); mem_wr(gb, HL, 80);  // ld (hl),80
  CYC(0x55e6, 0x55e8); A = 0xb3;  // ld a,GFXH_GANON_C
  CYC(0x55e8, 0x55eb); ganon_loadGfxHeader_hook(gb); return;  // jp ganon_loadGfxHeader
loadGfx:
  CALL_C(0x55eb, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x55ee);
  CALL_C(0x55ee, ecom_applyVelocityForSideviewEnemyNoHoles_b10_hook, 0x4156, 0x55f1);
  CALL_C(0x55f1, enemyAnimate_hook, 0x2818, 0x55f4);
  CYC(0x55f4, 0x55f7); ganon_updateSeizurePalette_hook(gb); return;  // jp ganon_updateSeizurePalette
}

void ganon_stateC_substate8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55f7, 0x55f9); A = 0x02;  // ld a,$02
  CYC(0x55f9, 0x55fc); mem_wr(gb, 0xcbc3, A);  // ld (wUseSimulatedInput),a
  CALL_C(0x55fc, ganon_updateSeizurePalette_hook, 0x57fd, 0x55ff);
  CYC(0x55ff, 0x5602); A = mem_rd(gb, 0xcc00);  // ld a,(wFrameCounter)
  CYC(0x5602, 0x5604); alu_and(gb, 0x03);  // and $03
  if ((F & FZ)) { CALL_C_CC(0x5604, ecom_decCounter2_b10_hook, 0x43a3, 0x5607); } else CYC(0x5604, 0x5607);  // call z,ecom_decCounter2
  CALL_C(0x5607, ecom_decCounter1_b10_hook, 0x439a, 0x560a);
  if ((F & FZ)) { CYCT(0x560a, 0x560c); goto nextSubstate; } CYC(0x560a, 0x560c);  // jr z,@nextSubstate
  CYC(0x560c, 0x560d); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x560d, 0x560f); alu_cp(gb, 60);  // cp 60
  if (!(F & FZ)) { RET_TAKEN(0x560f); return; } CYC(0x560f, 0x5610);  // ret nz
  CYC(0x5610, 0x5612); A = 0x05;  // ld a,$05
  CALL_C(0x5612, enemySetAnimation_hook, 0x282b, 0x5615);
  CYC(0x5615, 0x5617); E = 0x52;  // ld e,PART_52
  CALL_C(0x5617, ganon_spawnPart_hook, 0x57c4, 0x561a);
  if (!(F & FZ)) { RET_TAKEN(0x561a); return; } CYC(0x561a, 0x561b);  // ret nz
  CYC(0x561b, 0x561d); L = 0xc2;  // ld l,Part.subid
  CYC(0x561d, 0x561f); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x561f, 0x5622); if (hook_enabled_at(0x2242)) { objectCopyPosition_hook(gb); return; } HANDOFF(0x2242);  // jp objectCopyPosition
nextSubstate:
  CYC(0x5622, 0x5624); L = 0x85;  // ld l,Enemy.substate
  CYC(0x5624, 0x5625); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x5625, 0x5626); L = alu_inc8(gb, L);  // inc l
  CYC(0x5626, 0x5628); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x5628, 0x562a); A = 0x02;  // ld a,$02
  CYC(0x562a, 0x562d); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void ganon_stateC_substate9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x562d, 0x5630); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x5630, 0x5631); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x5631); return; } CYC(0x5631, 0x5632);  // ret nz
  CYC(0x5632, 0x5634); A = 0x0a;  // ld a,$0a
  CYC(0x5634, 0x5635); mem_wr(gb, DE, A);  // ld (de),a [substate]
  CYC(0x5635, 0x5637); A = 0x03;  // ld a,$03
  CALL_C(0x5637, ganon_setTileReplacementMode_hook, 0x581c, 0x563a);
  CYC(0x563a, 0x563c); A = 0xb1;  // ld a,PALH_b1
  CYC(0x563c, 0x563f); mem_wr(gb, 0xcbe3, A);  // ld (wExtraBgPaletteHeader),a
  CYC(0x563f, 0x5642); if (hook_enabled_at(0x050b)) { loadPaletteHeader_hook(gb); return; } HANDOFF(0x050b);  // jp loadPaletteHeader
}

void ganon_stateC_substateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5642, 0x5643); H = D;  // ld h,d
  CYC(0x5643, 0x5645); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5645, 0x5647); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x5647, clearPaletteFadeVariablesAndRefreshPalettes_hook, 0x3238, 0x564a);
  CYC(0x564a, 0x564d); ganon_finishAttack_hook(gb); return;  // jp ganon_finishAttack
}

void ganon_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x564d, 0x564e); E = alu_inc8(gb, E);  // inc e
  CYC(0x564e, 0x564f); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x564f, 0x5650); push_effect(gb, 0x5650);
  switch (ganon_jump_table(gb)) {
    case 0x5229: ganon_state8_substate0_hook(gb); return;
    case 0x5230: ganon_state8_substate1_hook(gb); return;
    case 0x523e: ganon_state8_substate2_hook(gb); return;
    case 0x565a: ganon_stateD_substate3_hook(gb); return;
    case 0x5728: ganon_finishAttack_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ganon_stateD_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x565a, ecom_decCounter1_b10_hook, 0x439a, 0x565d);
  if (!(F & FZ)) { CYCT(0x565d, 0x5660); ganon_updateTeleportAnimationComingIn_hook(gb); return; } CYC(0x565d, 0x5660);  // jp nz,ganon_updateTeleportAnimationComingIn
  CYC(0x5660, 0x5661); L = E;  // ld l,e
  CYC(0x5661, 0x5662); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5662, 0x5664); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x5664, 0x5665); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5665, 0x5667); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5667, 0x5668); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5668, 0x566b); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void ganon_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x566b, 0x566c); E = alu_inc8(gb, E);  // inc e
  CYC(0x566c, 0x566d); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x566d, 0x566e); push_effect(gb, 0x566e);
  switch (ganon_jump_table(gb)) {
    case 0x5676: ganon_stateE_substate0_hook(gb); return;
    case 0x56a5: ganon_stateE_substate1_hook(gb); return;
    case 0x56b7: ganon_stateE_substate2_hook(gb); return;
    case 0x56cb: ganon_stateE_substate3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void ganon_stateE_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5676, ecom_decCounter1_b10_hook, 0x439a, 0x5679);
  if (!(F & FZ)) { CYCT(0x5679, 0x567c); if (hook_enabled_at(0x441f)) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(0x441f); } CYC(0x5679, 0x567c);  // jp nz,ecom_flickerVisibility
  CYC(0x567c, 0x567d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x567d, 0x567f); E = 0x04;  // ld e,PART_BOSS_DEATH_EXPLOSION
  CALL_C(0x567f, ganon_spawnPart_hook, 0x57c4, 0x5682);
  if (!(F & FZ)) { RET_TAKEN(0x5682); return; } CYC(0x5682, 0x5683);  // ret nz
  CYC(0x5683, 0x5685); L = 0xc2;  // ld l,Part.subid
  CYC(0x5685, 0x5686); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [subid]=1
  CALL_C(0x5686, objectCopyPosition_hook, 0x2242, 0x5689);
  CYC(0x5689, 0x568b); E = 0x98;  // ld e,Enemy.relatedObj2
  CYC(0x568b, 0x568d); A = 0xc0;  // ld a,Part.start
  CYC(0x568d, 0x568e); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x568e, 0x568f); E = alu_inc8(gb, E);  // inc e
  CYC(0x568f, 0x5690); A = H;  // ld a,h
  CYC(0x5690, 0x5691); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5691, 0x5694); SET_HL(0xcdd1);  // ld hl,wNumEnemies
  CYC(0x5694, 0x5695); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5695, 0x5696); H = D;  // ld h,d
  CYC(0x5696, 0x5698); L = 0x85;  // ld l,Enemy.substate
  CYC(0x5698, 0x5699); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5699, 0x569b); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x569b, 0x569d); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CALL_C(0x569d, objectSetInvisible_hook, 0x1e7b, 0x56a0);
  CYC(0x56a0, 0x56a2); A = 0xbc;  // ld a,SND_BIG_EXPLOSION_2
  CYC(0x56a2, 0x56a5); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void ganon_stateE_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56a5, 0x56a7); A = 0x21;  // ld a,Object.animParameter
  CALL_C(0x56a7, objectGetRelatedObject2Var_hook, 0x2164, 0x56aa);
  CYC(0x56aa, 0x56ac); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { RET_TAKEN(0x56ac); return; } CYC(0x56ac, 0x56ad);  // ret z
  CYC(0x56ad, 0x56ae); H = D;  // ld h,d
  CYC(0x56ae, 0x56b0); L = 0x85;  // ld l,Enemy.substate
  CYC(0x56b0, 0x56b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x56b1, 0x56b2); L = alu_inc8(gb, L);  // inc l
  CYC(0x56b2, 0x56b4); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x56b4, 0x56b7); if (hook_enabled_at(0x3263)) { fastFadeoutToWhite_hook(gb); return; } HANDOFF(0x3263);  // jp fastFadeoutToWhite
}

void ganon_stateE_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x56b7, ecom_decCounter1_b10_hook, 0x439a, 0x56ba);
  if (!(F & FZ)) { RET_TAKEN(0x56ba); return; } CYC(0x56ba, 0x56bb);  // ret nz
  CYC(0x56bb, 0x56bd); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x56bd, 0x56be); L = E;  // ld l,e
  CYC(0x56be, 0x56bf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x56bf, 0x56c0); alu_xor(gb, A);  // xor a
  CYC(0x56c0, 0x56c3); mem_wr(gb, 0xcbe3, A);  // ld (wExtraBgPaletteHeader),a
  CALL_C(0x56c3, ganon_setTileReplacementMode_hook, 0x581c, 0x56c6);
  CYC(0x56c6, 0x56c8); A = 0x02;  // ld a,$02
  CYC(0x56c8, 0x56cb); if (hook_enabled_at(0x3284)) { fadeinFromWhiteWithDelay_hook(gb); return; } HANDOFF(0x3284);  // jp fadeinFromWhiteWithDelay
}

void ganon_stateE_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56cb, 0x56ce); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x56ce, 0x56cf); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x56cf); return; } CYC(0x56cf, 0x56d0);  // ret nz
  CALL_C(0x56d0, ecom_decCounter1_b10_hook, 0x439a, 0x56d3);
  if (!(F & FZ)) { RET_TAKEN(0x56d3); return; } CYC(0x56d3, 0x56d4);  // ret nz
  CYC(0x56d4, 0x56d5); alu_xor(gb, A);  // xor a
  CYC(0x56d5, 0x56d8); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CALL_C(0x56d8, decNumEnemies_hook, 0x24b3, 0x56db);
  CYC(0x56db, 0x56de); if (hook_enabled_at(0x2e47)) { enemyDelete_hook(gb); return; } HANDOFF(0x2e47);  // jp enemyDelete
}

void ganon_loadGfxHeader_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56de, 0x56df); push_effect(gb, AF);  // push af
  CALL_C(0x56df, loadGfxHeader_hook, 0x0626, 0x56e2);
  CYC(0x56e2, 0x56e4); A = 0x33;  // ld a,UNCMP_GFXH_33
  CALL_C(0x56e4, loadUncompressedGfxHeader_hook, 0x05da, 0x56e7);
  CYC(0x56e7, 0x56e8); SET_AF(pop_effect(gb));  // pop af
  CYC(0x56e8, 0x56ea); alu_sub(gb, 0xb2);  // sub GFXH_GANON_B
  CYC(0x56ea, 0x56ec); alu_add(gb, 0x1e);  // add OBJ_GFXH_1e
  CYC(0x56ec, 0x56ef); SET_HL(0xcc0c);  // ld hl,wLoadedObjectGfx+4
  CYC(0x56ef, 0x56f0); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x56f0, 0x56f2); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x56f2, 0x56f4); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x56f4, 0x56f5); D = A;  // ld d,a
  RET(0x56f5); return;  // ret
}

// X-position alternates left & right each frame while teleporting.
void ganon_updateTeleportAnimationGoingOut_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56f6, 0x56f7); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x56f7, 0x56f9); alu_and(gb, 0x3e);  // and $3e
  CYC(0x56f9, 0x56fa); alu_rrca(gb);  // rrca
  CYC(0x56fa, 0x56fb); B = A;  // ld b,a
  CYC(0x56fb, 0x56fd); A = 0x20;  // ld a,$20
  CYC(0x56fd, 0x56fe); alu_sub(gb, B);  // sub b
  ganon_updateFlickeringXPosition_hook(gb); return;  // fallthrough
}

void ganon_updateFlickeringXPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x56fe, 0x5700); alu_bit(gb, 1, mem_rd(gb, HL));  // bit 1,(hl)
  if ((F & FZ)) { CYCT(0x5700, 0x5702); goto haveDelta; } CYC(0x5700, 0x5702);  // jr z,+
  CYC(0x5702, 0x5703); alu_cpl(gb);  // cpl
  CYC(0x5703, 0x5704); A = alu_inc8(gb, A);  // inc a
haveDelta:
  CYC(0x5704, 0x5706); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x5706, 0x5707); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(0x5707, 0x5709); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5709, 0x570a); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x570a, 0x570d); if (hook_enabled_at(0x441f)) { ecom_flickerVisibility_b10_hook(gb); return; } HANDOFF(0x441f);  // jp ecom_flickerVisibility
}

void ganon_updateTeleportVarsAndPlaySound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x570d, 0x570f); A = 0x8d;  // ld a,SND_TELEPORT
  CALL_C(0x570f, playSound_b00_hook, 0x0c98, 0x5712);
  CYC(0x5712, 0x5713); H = D;  // ld h,d
  CYC(0x5713, 0x5715); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5715, 0x5717); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x5717, 0x5719); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5719, 0x571b); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x571b, 0x571d); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x571d, 0x571e); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x571e, 0x5720); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x5720, 0x5721); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x5721); return;  // ret
}

void ganon_updateTeleportAnimationComingIn_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5722, 0x5723); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5723, 0x5725); alu_and(gb, 0x3e);  // and $3e
  CYC(0x5725, 0x5726); alu_rrca(gb);  // rrca
  CYC(0x5726, 0x5728); ganon_updateFlickeringXPosition_hook(gb); return;  // jr ganon_updateFlickeringXPosition
}

void ganon_finishAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5728, 0x5729); H = D;  // ld h,d
  CYC(0x5729, 0x572b); L = 0xb5;  // ld l,Enemy.var35
  CYC(0x572b, 0x572c); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x572c, 0x572d); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(0x572d, 0x572e); L = A;  // ld l,a
  CYC(0x572e, 0x572f); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x572f, 0x5730); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x5730, 0x5732); ganon_decideNextMove_hook(gb); return; } CYC(0x5730, 0x5732);  // jr z,ganon_decideNextMove
  label_10_135_hook(gb); return;  // fallthrough
}

void label_10_135_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5732, 0x5734); E = 0x84;  // ld e,Enemy.state
  CYC(0x5734, 0x5735); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5735, 0x5736); E = alu_inc8(gb, E);  // inc e
  CYC(0x5736, 0x5737); alu_xor(gb, A);  // xor a
  CYC(0x5737, 0x5738); mem_wr(gb, DE, A);  // ld (de),a [substate]
  CYC(0x5738, 0x573a); E = 0xb5;  // ld e,Enemy.var35
  CYC(0x573a, 0x573b); A = L;  // ld a,l
  CYC(0x573b, 0x573c); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x573c, 0x573d); E = alu_inc8(gb, E);  // inc e
  CYC(0x573d, 0x573e); A = H;  // ld a,h
  CYC(0x573e, 0x573f); mem_wr(gb, DE, A);  // ld (de),a
  RET(0x573f); return;  // ret
}

void ganon_decideNextMove_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5740, 0x5742); E = 0xa9;  // ld e,Enemy.health
  CYC(0x5742, 0x5743); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5743, 0x5745); alu_cp(gb, 0x41);  // cp $41
  CYC(0x5745, 0x5747); C = 0x00;  // ld c,$00
  if (!(F & FC)) { CYCT(0x5747, 0x5749); goto haveC; } CYC(0x5747, 0x5749);  // jr nc,+
  CYC(0x5749, 0x574b); C = 0x04;  // ld c,$04
haveC:
  CALL_C(0x574b, getRandomNumber_hook, 0x043e, 0x574e);
  CYC(0x574e, 0x5750); alu_and(gb, 0x03);  // and $03
  CYC(0x5750, 0x5751); alu_add(gb, C);  // add c
  CYC(0x5751, 0x5754); SET_HL(0x575b);  // ld hl,@stateTable
  CYC(0x5754, 0x5755);
  ganon_addDoubleIndexToHl_from_rst(gb, 0x5755);  // rst_addDoubleIndexToHl
  CYC(0x5755, 0x5756); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5756, 0x5757); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(0x5757, 0x5758); L = A;  // ld l,a
  CYC(0x5758, 0x5759); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5759, 0x575c); label_10_135_hook(gb); return;  // jr label_10_135
}

void ganon_decideTeleportLocationAndCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5787, 0x578a); SET_BC(0x0e0f);  // ld bc,$0e0f
  CALL_C(0x578a, ecom_randomBitwiseAndBCE_b10_hook, 0x434f, 0x578d);
  CYC(0x578d, 0x578e); A = B;  // ld a,b
  CYC(0x578e, 0x5791); SET_HL(0x57a4);  // ld hl,@teleportTargetTable
  CYC(0x5791, 0x5792);
  ganon_addAToHl_from_rst(gb, 0x5792);  // rst_addAToHl
  CYC(0x5792, 0x5794); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x5794, 0x5795); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5795, 0x5796); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5796, 0x5798); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5798, 0x5799); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5799, 0x579a); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x579a, 0x579b); A = C;  // ld a,c
  CYC(0x579b, 0x579e); SET_HL(0x57b4);  // ld hl,@counter1Vals
  CYC(0x579e, 0x579f);
  ganon_addAToHl_from_rst(gb, 0x579f);  // rst_addAToHl
  CYC(0x579f, 0x57a1); E = 0x86;  // ld e,Enemy.counter1
  CYC(0x57a1, 0x57a2); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x57a2, 0x57a3); mem_wr(gb, DE, A);  // ld (de),a
  RET(0x57a3); return;  // ret
}

void ganon_spawnPart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x57c4, getFreePartSlot_hook, 0x3e8e, 0x57c7);
  if (!(F & FZ)) { RET_TAKEN(0x57c7); return; } CYC(0x57c7, 0x57c8);  // ret nz
  CYC(0x57c8, 0x57c9); mem_wr(gb, HL, E);  // ld (hl),e
  CYC(0x57c9, 0x57cb); L = 0xd6;  // ld l,Part.relatedObj1
  CYC(0x57cb, 0x57cd); mem_wr(gb, HL, 0x80);  // ld (hl),Enemy.start
  CYC(0x57cd, 0x57ce); L = alu_inc8(gb, L);  // inc l
  CYC(0x57ce, 0x57cf); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(0x57cf, 0x57d0); alu_xor(gb, A);  // xor a
  RET(0x57d0); return;  // ret
}

void ganon_makeRoomBoundarySolid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x57d1, 0x57d4); SET_HL(0xce10);  // ld hl,wRoomCollisions+$10
  CYC(0x57d4, 0x57d6); B = 0x09;  // ld b,LARGE_ROOM_HEIGHT-2
topRow:
  CYC(0x57d6, 0x57d8); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(0x57d8, 0x57d9); A = L;  // ld a,l
  CYC(0x57d9, 0x57db); alu_add(gb, 0x10);  // add $10
  CYC(0x57db, 0x57dc); L = A;  // ld l,a
  CYC(0x57dc, 0x57dd); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(0x57dd, 0x57df); goto topRow; } CYC(0x57dd, 0x57df);  // jr nz,-
  CYC(0x57df, 0x57e1); L = 0x1e;  // ld l,$1e
  CYC(0x57e1, 0x57e3); B = 0x09;  // ld b,LARGE_ROOM_HEIGHT-2
bottomRow:
  CYC(0x57e3, 0x57e5); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(0x57e5, 0x57e6); A = L;  // ld a,l
  CYC(0x57e6, 0x57e8); alu_add(gb, 0x10);  // add $10
  CYC(0x57e8, 0x57e9); L = A;  // ld l,a
  CYC(0x57e9, 0x57ea); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(0x57ea, 0x57ec); goto bottomRow; } CYC(0x57ea, 0x57ec);  // jr nz,-
  CYC(0x57ec, 0x57ee); L = 0x00;  // ld l,$00
  CYC(0x57ee, 0x57f0); A = 0x0f;  // ld a,$0f
  CYC(0x57f0, 0x57f1); B = A;  // ld b,a
leftCol:
  CYC(0x57f1, 0x57f2); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x57f2, 0x57f3); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(0x57f3, 0x57f5); goto leftCol; } CYC(0x57f3, 0x57f5);  // jr nz,-
  CYC(0x57f5, 0x57f7); L = 0xa0;  // ld l,(LARGE_ROOM_HEIGHT-1)<<4
  CYC(0x57f7, 0x57f8); B = A;  // ld b,a
rightCol:
  CYC(0x57f8, 0x57f9); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x57f9, 0x57fa); B = alu_dec8(gb, B);  // dec b
  if (!(F & FZ)) { CYCT(0x57fa, 0x57fc); goto rightCol; } CYC(0x57fa, 0x57fc);  // jr nz,-
  RET(0x57fc); return;  // ret
}

void ganon_updateSeizurePalette_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x57fd, 0x5800); A = mem_rd(gb, 0xcd00);  // ld a,(wScrollMode)
  CYC(0x5800, 0x5802); alu_and(gb, 0x01);  // and $01
  if ((F & FZ)) { RET_TAKEN(0x5802); return; } CYC(0x5802, 0x5803);  // ret z
  CYC(0x5803, 0x5806); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x5806, 0x5807); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x5807); return; } CYC(0x5807, 0x5808);  // ret nz
  CYC(0x5808, 0x580b); A = mem_rd(gb, 0xcc00);  // ld a,(wFrameCounter)
  CYC(0x580b, 0x580c); alu_rrca(gb);  // rrca
  if ((F & FC)) { RET_TAKEN(0x580c); return; } CYC(0x580c, 0x580d);  // ret c
  CYC(0x580d, 0x580e); H = D;  // ld h,d
  CYC(0x580e, 0x5810); L = 0xb7;  // ld l,$b7
  CYC(0x5810, 0x5811); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5811, 0x5812); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5812, 0x5814); alu_and(gb, 0x07);  // and $07
  CYC(0x5814, 0x5816); alu_add(gb, 0xb1);  // add PALH_b1
  CYC(0x5816, 0x5819); mem_wr(gb, 0xcbe3, A);  // ld (wExtraBgPaletteHeader),a
  CYC(0x5819, 0x581c); if (hook_enabled_at(0x050b)) { loadPaletteHeader_hook(gb); return; } HANDOFF(0x050b);  // jp loadPaletteHeader
}

void ganon_setTileReplacementMode_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x581c, 0x581f); mem_wr(gb, 0xcca9, A);  // ld (wTwinrovaTileReplacementMode),a
  CALL_C(0x581f, func_131f_hook, 0x131f, 0x5822);
  CYC(0x5822, 0x5824); A = mem_rd(gb, 0xffaf);  // ldh a,(<hActiveObject)
  CYC(0x5824, 0x5825); D = A;  // ld d,a
  RET(0x5825); return;  // ret
}
