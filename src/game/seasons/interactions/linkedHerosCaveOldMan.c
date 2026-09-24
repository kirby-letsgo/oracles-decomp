#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/linkedHerosCaveOldMan.s.
// INTERAC_LINKED_HEROS_CAVE_OLD_MAN

static void linkedHerosCaveOldMan_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_LINKED_HEROS_CAVE_OLD_MAN
void s_interactionCodee4_hook(GB *gb) {
  BASE(interactionCodee4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto L_6ef8; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_interactionRunScript, SYM(interactionRunScript), b_+8);
  if (F & FC) { CYCT(b_+8, b_+11); TAIL(interactionDelete); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL(npcFaceLinkAndAnimate);
L_6ef8:
  CALL_C(b_+14, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); TAIL(interactionDelete); }
  CYC(b_+19, b_+22);
  CALL_C(b_+22, s_interactionIncState, SYM(interactionIncState), b_+25);
  CALL_C(b_+25, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+28);
  CALL_C(b_+28, s_objectSetVisible82, SYM(objectSetVisible82), b_+31);
  CYC(b_+31, b_+33); A = 0x33;
  CALL_C(b_+33, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+36);
  CYC(b_+36, b_+39); SET_HL(SYM(linkedHerosCaveOldManScript));
  CYC(b_+39, b_+42);
  TAIL(interactionSetScript);
}

void s_linkedHerosCaveOldMan_spawnChests_hook(GB *gb) {
  BASE(linkedHerosCaveOldMan_spawnChests);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wcca1, A);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wcca2, A);
  CYC(b_+9, b_+11); B = 0x08;
L_6f1f:
  CALL_C(b_+11, s_func_6f39_hook, SYM(func_6f39), b_+14);
  CALL_C(b_+14, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_6f2e; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x05);
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_YH;
  CALL_C(b_+23, s_setShortPosition_paramC, SYM(setShortPosition_paramC), b_+26);
L_6f2e:
  CYC(b_+26, b_+27); push_effect(gb, BC);
  CYC(b_+27, b_+29); A = 0xf1;
  CALL_C(b_+29, s_setTile, SYM(setTile), b_+32);
  CYC(b_+32, b_+33); SET_BC(pop_effect(gb));
  CYC(b_+33, b_+34); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto L_6f1f; }
  CYC(b_+34, b_+36);
  RET(b_+36); return;
}

void s_func_6f39_hook(GB *gb) {
  BASE(func_6f39);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); A = alu_dec8(gb, A);
  CYC(b_+2, b_+5); SET_HL(SYM(table_6f41));
  CYC(b_+5, b_+6); linkedHerosCaveOldMan_add_a_to_hl(gb, b_+6);
  CYC(b_+6, b_+7); C = mem_rd(gb, HL);
  RET(b_+7); return;
}

void s_linkedHerosCaveOldMan_takeRupees_hook(GB *gb) {
  BASE(linkedHerosCaveOldMan_takeRupees);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+4, b_+6); A = 0x08;
  CALL_C(b_+6, s_cpRupeeValue, SYM(cpRupeeValue), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x08;
  CALL_C(b_+12, s_removeRupeeValue, SYM(removeRupeeValue), b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wTmpcfc0 + 0x10, A);
  RET(b_+20); return;
}

