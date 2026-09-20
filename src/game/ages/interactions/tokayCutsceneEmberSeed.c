#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode8f), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode8f), (from), (to), true)

static uint16_t interactionCode8f_jump_table(GB *gb) {
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

// INTERAC_TOKAY_CUTSCENE_EMBER_SEED
void interactionCode8f_hook(GB *gb) {
  BASE(interactionCode8f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode8f_jump_table(gb);
    if (target == b_+30) goto state1;
    if (target == b_+48) goto state2;
    if (target == b_+74) goto state3;
  }

  // interactionCode8f@state0
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18); SET_BC(0xff00);
  CALL_C(b_+18, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+21);
  CALL_C(b_+21, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+24);
  CALL_C(b_+24, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+27);
  CYC(b_+27, b_+30); objectSetVisible80_hook(gb); return; // jp

state1:
  CYC(b_+30, b_+32); C = 0x10;
  CALL_C(b_+32, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+35);
  if (!(F & FZ)) { RET_TAKEN(b_+35); return; } // ret nz
  CYC(b_+35, b_+36);
  CALL_C(b_+36, objectSetInvisible_hook, SYM(objectSetInvisible), b_+39);
  CYC(b_+39, b_+42); A = W8(wTextIsActive);
  CYC(b_+42, b_+43); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+43); return; } // ret z
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(b_+47);
  return; // ret

state2:
  CALL_C(b_+48, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+51);
  CALL_C(b_+51, interactionIncState_hook, SYM(interactionIncState), b_+54);
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+56, b_+58); A = 0x0a;
  CYC(b_+58, b_+59); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x06); // [oamTileIndexBase] = $06
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x3a); // 58
  CYC(b_+66, b_+68); A = 0x0b;
  CALL_C(b_+68, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+71);
  CYC(b_+71, b_+74); objectSetVisible_hook(gb); return; // jp

state3:
  CALL_C(b_+74, interactionAnimate_hook, SYM(interactionAnimate), b_+77);
  CALL_C(b_+77, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; } // ret nz
  CYC(b_+80, b_+81);
  CYC(b_+81, SYM(interactionCode90)); interactionDelete_hook(gb); return; // jp
}
