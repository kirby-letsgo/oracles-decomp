#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodea5_jump_table(GB *gb) {
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

// 0b:5d0e, called from interactionCodea5@state2 and @state4.
// @param[out] zflag z if reached touching book
void interactionCodea5_updateMapleAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d0e, 0x5d11); SET_HL(w1Companion_yh);
  CYC(0x5d11, 0x5d12); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5d12, 0x5d13); B = A;
  CYC(0x5d13, 0x5d14); L = alu_inc8(gb, L);
  CYC(0x5d14, 0x5d15); C = mem_rd(gb, HL);
  CYC(0x5d15, 0x5d18); A = mem_rd(gb, wMapleState);
  CYC(0x5d18, 0x5d1a); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(0x5d1a, 0x5d1c); goto l_5d27; } // jr z
  CYC(0x5d1a, 0x5d1c);
  CYC(0x5d1c, 0x5d1e); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5d1e, 0x5d1f); A = mem_rd(gb, DE);
  CYC(0x5d1f, 0x5d20); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x5d20, 0x5d22); goto l_5d27; } // jr nz
  CYC(0x5d20, 0x5d22);
  CYC(0x5d22, 0x5d24); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5d24, 0x5d25); A = mem_rd(gb, DE);
  CYC(0x5d25, 0x5d26); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x5d26, 0x5d27); ret_effect(gb); return; } // ret z
  CYC(0x5d26, 0x5d27);

l_5d27:
  CALL_C(0x5d27, objectGetRelativeAngle_hook, 0x1ea4, 0x5d2a);
  CYC(0x5d2a, 0x5d2c); A = (uint8_t)(A ^ 0x10);
  CYC(0x5d2c, 0x5d2f); mem_wr(gb, w1Companion_angle, A);
  CYC(0x5d2f, 0x5d30); alu_or(gb, D);
  RET(0x5d30); return;
}

// ==================================================================================================
// INTERAC_TOUCHING_BOOK
// ==================================================================================================
void interactionCodea5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c11, 0x5c13); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5c13, 0x5c14); A = mem_rd(gb, DE);
  CYC(0x5c14, 0x5c15); push_effect(gb, 0x5c15);
  switch (interactionCodea5_jump_table(gb)) {
    case 0x5c27: goto state0;
    case 0x5c5e: goto state1;
    case 0x5c69: goto state2;
    case 0x5c83: goto state3;
    case 0x5ca2: goto state4;
    case 0x5cba: goto state5;
    case 0x5cd0: goto state6;
    case 0x5cde: goto state7;
    case 0x5cf5: goto state8;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x5c27, 0x5c29); A = 0x01;
  CYC(0x5c29, 0x5c2c); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5c2c, 0x5c2f); SET_HL(w1Link_knockbackCounter);
  CYC(0x5c2f, 0x5c30); A = mem_rd(gb, HL);
  CYC(0x5c30, 0x5c31); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5c31, 0x5c32); ret_effect(gb); return; } // ret nz
  CYC(0x5c31, 0x5c32);
  CYC(0x5c32, 0x5c34); A = 0x01;
  CYC(0x5c34, 0x5c35); mem_wr(gb, DE, A); // [state]
  CALL_C(0x5c35, objectTakePosition_hook, 0x2274, 0x5c38);
  CYC(0x5c38, 0x5c3b); SET_BC(0x3850);
  CALL_C(0x5c3b, objectGetRelativeAngle_hook, 0x1ea4, 0x5c3e);
  CYC(0x5c3e, 0x5c40); alu_and(gb, 0x1c);
  CYC(0x5c40, 0x5c42); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5c42, 0x5c43); mem_wr(gb, DE, A);
  CYC(0x5c43, 0x5c46); SET_BC(0xff00); // -$100
  CALL_C(0x5c46, objectSetSpeedZ_hook, 0x239d, 0x5c49);
  CYC(0x5c49, 0x5c4b); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x5c4b, 0x5c4d); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x5c4d, interactionInitGraphics_hook, 0x15fb, 0x5c50);
  CALL_C(0x5c50, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5c53);
  CYC(0x5c53, 0x5c56); A = mem_rd(gb, w1Link_visible);
  CYC(0x5c56, 0x5c58); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x5c58, 0x5c59); mem_wr(gb, DE, A);
  CYC(0x5c59, 0x5c5b); A = 0x57; // SND_GAINHEART
  CYC(0x5c5b, 0x5c5e); playSound_b00_hook(gb); return; // jp

state1:
  CYC(0x5c5e, 0x5c60); C = 0x20;
  CALL_C(0x5c60, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5c63);
  if (!(F & FZ)) { CYCT(0x5c63, 0x5c66); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x5c63, 0x5c66);
  CYC(0x5c66, 0x5c69); interactionIncState_hook(gb); return; // jp

state2:
  CALL_C(0x5c69, interactionCodea5_updateMapleAngle_hook, 0x5d0e, 0x5c6c);
  if (!(F & FZ)) { CYCT(0x5c6c, 0x5c6d); ret_effect(gb); return; } // ret nz
  CYC(0x5c6c, 0x5c6d);
  CYC(0x5c6d, 0x5c6f); A = 0xff;
  CYC(0x5c6f, 0x5c72); mem_wr(gb, w1Companion_angle, A);
  CALL_C(0x5c72, interactionIncState_hook, 0x23e0, 0x5c75);
  CALL_C(0x5c75, objectSetInvisible_hook, 0x1e7b, 0x5c78);
  CYC(0x5c78, 0x5c7a); A = 0x5e; // SND_GETSEED
  CALL_C(0x5c7a, playSound_b00_hook, 0x0c98, 0x5c7d);
  CYC(0x5c7d, 0x5c80); SET_BC(0x070e); // TX_070e
  CYC(0x5c80, 0x5c83); showText_hook(gb); return; // jp

state3:
  CALL_C(0x5c83, retIfTextIsActive_hook, 0x1859, 0x5c86);
  CYC(0x5c86, 0x5c89); SET_HL(w1Link_xh);
  CYC(0x5c89, 0x5c8a); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x5c8a, 0x5c8c); B = 0xf0;
  CYC(0x5c8c, 0x5c8e); alu_cp(gb, 0x58);
  if (!(F & FC)) { CYCT(0x5c8e, 0x5c90); goto l_5c92; } // jr nc
  CYC(0x5c8e, 0x5c90);
  CYC(0x5c90, 0x5c92); B = 0x10;

l_5c92:
  CYC(0x5c92, 0x5c93); alu_add(gb, B);
  CYC(0x5c93, 0x5c95); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5c95, 0x5c96); mem_wr(gb, DE, A);
  CYC(0x5c96, 0x5c97); L = alu_dec8(gb, L);
  CYC(0x5c97, 0x5c98); A = mem_rd(gb, HL); // [w1Link.yh]
  CYC(0x5c98, 0x5c9a); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5c9a, 0x5c9b); mem_wr(gb, DE, A);
  CYC(0x5c9b, 0x5c9c); alu_xor(gb, A);
  CYC(0x5c9c, 0x5c9f); mem_wr(gb, w1Companion_angle, A);
  CYC(0x5c9f, 0x5ca2); interactionIncState_hook(gb); return; // jp

state4:
  CALL_C(0x5ca2, interactionCodea5_updateMapleAngle_hook, 0x5d0e, 0x5ca5);
  if (!(F & FZ)) { CYCT(0x5ca5, 0x5ca6); ret_effect(gb); return; } // ret nz
  CYC(0x5ca5, 0x5ca6);
  CYC(0x5ca6, 0x5ca9); SET_HL(w1Companion_angle);
  CYC(0x5ca9, 0x5cab); A = 0xff;
  CYC(0x5cab, 0x5cac); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x5cac, 0x5cad); A = mem_rd(gb, HL); // [w1Companion.direction]
  CYC(0x5cad, 0x5caf); A = (uint8_t)(A ^ 0x02);
  CYC(0x5caf, 0x5cb0); H = alu_dec8(gb, H);
  CYC(0x5cb0, 0x5cb1); mem_wr(gb, HL, A); // [w1Link.direction]
  CALL_C(0x5cb1, interactionIncState_hook, 0x23e0, 0x5cb4);
  CYC(0x5cb4, 0x5cb7); SET_BC(0x070f); // TX_070f
  CYC(0x5cb7, 0x5cba); showText_hook(gb); return; // jp

state5:
  CALL_C(0x5cba, retIfTextIsActive_hook, 0x1859, 0x5cbd);
  CYC(0x5cbd, 0x5cc0); A = mem_rd(gb, w1Companion_direction);
  CYC(0x5cc0, 0x5cc2); A = (uint8_t)(A ^ 0x02);
  CYC(0x5cc2, 0x5cc4); A = (uint8_t)(A | (1 << 7));
  CYC(0x5cc4, 0x5cc7); mem_wr(gb, w1Companion_direction, A);
  CALL_C(0x5cc7, interactionIncState_hook, 0x23e0, 0x5cca);
  CYC(0x5cca, 0x5ccd); SET_BC(0x0710); // TX_0710
  CYC(0x5ccd, 0x5cd0); showText_hook(gb); return; // jp

state6:
  CALL_C(0x5cd0, retIfTextIsActive_hook, 0x1859, 0x5cd3);
  CYC(0x5cd3, 0x5cd6); A = mem_rd(gb, w1Companion_direction);
  CYC(0x5cd6, 0x5cd8); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x5cd8, 0x5cdb); mem_wr(gb, w1Companion_direction, A);
  CYC(0x5cdb, 0x5cde); interactionIncState_hook(gb); return; // jp

state7:
  CYC(0x5cde, 0x5ce1); SET_BC(0x4109); // TREASURE_TRADEITEM, TRADEITEM_MAGIC_OAR
  CALL_C(0x5ce1, createTreasure_hook, 0x27d4, 0x5ce4);
  if (!(F & FZ)) { CYCT(0x5ce4, 0x5ce5); ret_effect(gb); return; } // ret nz
  CYC(0x5ce4, 0x5ce5);
  CYC(0x5ce5, 0x5ce7); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5ce7, 0x5ce9); A = 0x02;
  CYC(0x5ce9, 0x5cea); mem_wr(gb, DE, A);
  CYC(0x5cea, 0x5ceb); push_effect(gb, DE);
  CYC(0x5ceb, 0x5cee); SET_DE(w1Link_yh);
  CALL_C(0x5cee, objectCopyPosition_rawAddress_hook, 0x2247, 0x5cf1);
  CYC(0x5cf1, 0x5cf2); SET_DE(pop_effect(gb));
  CYC(0x5cf2, 0x5cf5); interactionIncState_hook(gb); return; // jp

state8:
  CYC(0x5cf5, 0x5cf7); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5cf7, 0x5cf8); A = mem_rd(gb, DE);
  CYC(0x5cf8, 0x5cf9); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5cf9, 0x5cfb); goto l_5cfe; } // jr z
  CYC(0x5cf9, 0x5cfb);
  CYC(0x5cfb, 0x5cfc); A = alu_dec8(gb, A);
  CYC(0x5cfc, 0x5cfd); mem_wr(gb, DE, A);
  RET(0x5cfd); return;

l_5cfe:
  CALL_C(0x5cfe, retIfTextIsActive_hook, 0x1859, 0x5d01);
  CYC(0x5d01, 0x5d03); A = 0x01; // DISABLE_LINK
  CYC(0x5d03, 0x5d06); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5d06, 0x5d08); A = 0x02;
  CYC(0x5d08, 0x5d0b); mem_wr(gb, w1Companion_substate, A);
  CYC(0x5d0b, 0x5d0e); interactionDelete_hook(gb); return; // jp
}
