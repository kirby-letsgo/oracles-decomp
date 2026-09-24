#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/9b.s.
// INTERAC_9b

static uint16_t i9b_jump_table(GB *gb) {
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

// INTERAC_9b
void s_interactionCode9b_hook(GB *gb) {
  BASE(interactionCode9b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i9b_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+18) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+15, b_+18); mem_wr(gb, wTmpcfc0 + 0x11, A);
state1:
  CYC(b_+18, b_+21); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+21, b_+23); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); TAIL_S(func_5b49); }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+35); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+35, b_+38); SET_HL(wWarpDestGroup);
  CYC(b_+38, b_+40); A = 0x80;
  CYC(b_+40, b_+41); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+41, b_+43); A = 0x6f;
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+44, b_+46); A = 0x0f;
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+47, b_+49); A = 0x55;
  CYC(b_+49, b_+50); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0x03);
  CYC(b_+52, b_+55);
  TAIL(interactionDelete);
}

// func_5b49@func_5b65
static void i9b_func_5b65(GB *gb) {
  BASE(func_5b49);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+28, b_+30); alu_bit(gb, 6, A);
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  if (F & FZ) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+35, b_+37); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+37); return;
}

void s_func_5b49_hook(GB *gb) {
  BASE(func_5b49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i9b_jump_table(gb));
    if (jt_ == b_+12) goto substate0;
    if (jt_ == b_+38) goto substate1;
    if (jt_ == b_+81) goto substate2;
    if (jt_ == b_+127) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+12, b_+15); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+21, b_+22); A = alu_dec8(gb, A);
  CYC(b_+22, b_+25); mem_wr(gb, wTmpcbba, A);
  CYC(b_+25, b_+28);
  TAIL(interactionIncSubstate);

substate1:
  CYC(b_+38, b_+41); SET_HL(wTmpcbb3);
  CYC(b_+41, b_+43); B = 0x02;
  CALL_C(b_+43, s_flashScreen, SYM(flashScreen), b_+46);
  if (F & FZ) { RET_TAKEN(b_+46); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+50); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0xff);
  CYC(b_+52, b_+53); push_effect(gb, DE);
  CALL_C(b_+53, s_hideStatusBar, SYM(hideStatusBar), b_+56);
  CALL_C(b_+56, s_clearItems, SYM(clearItems), b_+59);
  CYC(b_+59, b_+60); SET_DE(pop_effect(gb));
  CYC(b_+60, b_+61); alu_xor(gb, A);
  CYC(b_+61, b_+64); mem_wr(gb, w1Link_visible, A);
  CALL_C(b_+64, s_clearPaletteFadeVariablesAndRefreshPalettes, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+67);
  CYC(b_+67, b_+69); A = 0xff;
  CYC(b_+69, b_+71); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+71, b_+73); mem_wr(gb, hBgPaletteSources, A);
  CALL_C(b_+73, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+76);
  CYC(b_+76, b_+78); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x1e);
  RET(b_+80); return;
substate2:
  CYC(b_+81, b_+83); A = 0x01;
  CALL_C(b_+83, s_func_5a82_hook, SYM(func_5a82), b_+86);
  if (!(F & FZ)) { RET_TAKEN(b_+86); return; }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+89); A = 0x40;
  CYC(b_+89, b_+92); mem_wr(gb, w1Link_yh, A);
  CYC(b_+92, b_+94); A = 0x50;
  CYC(b_+94, b_+97); mem_wr(gb, w1Link_xh, A);
  CYC(b_+97, b_+99); A = 0x80;
  CYC(b_+99, b_+102); mem_wr(gb, w1Link_visible, A);
  CYC(b_+102, b_+104); A = 0x02;
  CYC(b_+104, b_+107); mem_wr(gb, w1Link_direction, A);
  CALL_C(b_+107, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+110);
  CALL_C(b_+110, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+113);
  CYC(b_+113, b_+115); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x1e);
  CYC(b_+117, b_+119); C = 0x02;
  CYC(b_+119, b_+122); SET_HL(SYM(loadDeathRespawnBufferPreset));
  CYC(b_+122, b_+124); E = 0x01;
  CYC(b_+124, b_+127);
  TAIL(interBankCall);
substate3:
  CALL_C(b_+127, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+130);
  if (!(F & FZ)) { RET_TAKEN(b_+130); return; }
  CYC(b_+130, b_+131);
  CYC(b_+131, b_+133); A = 0x12;
  CALL_C(b_+133, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+136);
  CALL_C(b_+136, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+139);
  CALL_L(b_+139, i9b_func_5b65, b_+142);
  CYC(b_+142, b_+143); C = mem_rd(gb, HL);
  CYC(b_+143, b_+145); A = 0x00;
  CYC(b_+145, b_+147); B = 0x6f;
  CALL_C(b_+147, s_getRoomFlags, SYM(getRoomFlags), b_+150);
  CYC(b_+150, b_+151); mem_wr(gb, HL, C);
  CYC(b_+151, b_+153); A = 0x03;
  CYC(b_+153, b_+156); mem_wr(gb, wLinkForceState, A);
  CYC(b_+156, b_+157); alu_xor(gb, A);
  CYC(b_+157, b_+160); mem_wr(gb, wLinkHealth, A);
  CYC(b_+160, b_+163);
  TAIL(interactionDelete);
}

