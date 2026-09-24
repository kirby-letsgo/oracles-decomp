#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/moblinKeepScenes.s.
// INTERAC_MOBLIN_KEEP_SCENES

static uint16_t moblinKeepScenes_jump_table(GB *gb) {
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

static void moblinKeepScenes_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_MOBLIN_KEEP_SCENES
void s_interactionCodeab_hook(GB *gb) {
  BASE(interactionCodeab);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (moblinKeepScenes_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+94) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x3f;
  CALL_C(b_+13, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+22); SET_HL(SYM(moblinKeepScene_scriptTable));
  CYC(b_+22, b_+23); moblinKeepScenes_add_double_index(gb, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+25); H = mem_rd(gb, HL);
  CYC(b_+25, b_+26); L = A;
  CALL_C(b_+26, s_interactionSetScript, SYM(interactionSetScript), b_+29);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); push_effect(gb, b_+33);
  do { uint16_t jt_ = (moblinKeepScenes_jump_table(gb));
    if (jt_ == b_+39) goto subid0;
    if (jt_ == b_+72) goto subid1;
    if (jt_ == b_+75) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+39, b_+41); A = 0x16;
  CALL_C(b_+41, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+47); TAIL(interactionDelete); }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+50); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+50, b_+52); alu_cp(gb, 0xd0);
  if (F & FZ) { CYCT(b_+52, b_+54); goto L_69aa; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+57); A = mem_rd(gb, w1Companion_xh);
  CYC(b_+57, b_+59);
  goto L_69ad;
L_69aa:
  CYC(b_+59, b_+62); A = mem_rd(gb, w1Link_xh);
L_69ad:
  CYC(b_+62, b_+64); alu_cp(gb, 0x3d);
  if (F & FC) { CYCT(b_+64, b_+67); TAIL(interactionDelete); }
  CYC(b_+64, b_+67);
  CYC(b_+67, b_+69); A = 0x00;
  CALL_C(b_+69, s_moblinKeepScene_spawnKingMoblin_hook, SYM(moblinKeepScene_spawnKingMoblin), b_+72);
subid1:
  CYC(b_+72, b_+75);
  goto state1;
subid2:
  CYC(b_+75, b_+77); A = 0x16;
  CALL_C(b_+77, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+80);
  if (F & FZ) { CYCT(b_+80, b_+83); TAIL(interactionDelete); }
  CYC(b_+80, b_+83);
  CALL_C(b_+83, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+86);
  CYC(b_+86, b_+88); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+88, b_+91); TAIL(interactionDelete); }
  CYC(b_+88, b_+91);
  CALL_C(b_+91, s_setDeathRespawnPoint, SYM(setDeathRespawnPoint), b_+94);
state1:
  CALL_C(b_+94, s_interactionRunScript, SYM(interactionRunScript), b_+97);
  if (F & FC) { CYCT(b_+97, b_+100); TAIL(interactionDelete); }
  CYC(b_+97, b_+100);
  RET(b_+100); return;
}

void s_moblinKeepScene_setLinkDirectionAndPositionAfterDestroyed_hook(GB *gb) {
  BASE(moblinKeepScene_setLinkDirectionAndPositionAfterDestroyed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkForceState, A);
  CYC(b_+5, b_+8); SET_HL(w1Link_direction);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x02);
  CYC(b_+10, b_+12); L = 0x0b;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x18);
  CYC(b_+14, b_+16); L = 0x0d;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x48);
  RET(b_+18); return;
}

void s_moblinKeepScene_spawnKingMoblin_hook(GB *gb) {
  BASE(moblinKeepScene_spawnKingMoblin);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_add(gb, A);
  CYC(b_+1, b_+4); SET_BC(b_+27 /* @kingMoblinSpawnData */);
  CALL_C(b_+4, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+7);
  CALL_C(b_+7, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x95);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+15); A = mem_rd(gb, BC);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); SET_BC(BC + 1);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+19, b_+20); A = mem_rd(gb, BC);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+22); SET_BC(BC + 1);
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  RET(b_+26); return;
}

void s_moblinKeepScene_spawn2MoblinsAfterKeepDestroyed_hook(GB *gb) {
  BASE(moblinKeepScene_spawn2MoblinsAfterKeepDestroyed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+33 /* @moblinSpawnData */);
  CYC(b_+3, b_+5); E = 0x02;
L_6a0f:
  CALL_C(b_+5, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x96);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x04);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); A = mem_rd(gb, BC);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+18); SET_BC(BC + 1);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+23); SET_BC(BC + 1);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+25, b_+26); A = mem_rd(gb, BC);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+28); SET_BC(BC + 1);
  CYC(b_+28, b_+29); SET_BC(BC + 1);
  CYC(b_+29, b_+30); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto L_6a0f; }
  CYC(b_+30, b_+32);
  RET(b_+32); return;
}

