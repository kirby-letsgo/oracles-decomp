#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode49), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode49), (from), (to), true)

static uint16_t forest_fairy_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void forest_fairy_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void forestFairy_subid00_hook(GB *gb);
void forestFairy_subid00State0_hook(GB *gb);
void forestFairy_loadMovementPreset_hook(GB *gb);
void forestFairy_subid00State1_hook(GB *gb);
void forestFairy_updateMovement_hook(GB *gb);
void forestFairy_animate_hook(GB *gb);
void forestFairy_subid00State2_hook(GB *gb);
void forestFairy_subid00State3_hook(GB *gb);
void forestFairy_deleteSelf_hook(GB *gb);
void forestFairy_subid01_hook(GB *gb);
void forestFairy_initCollisionRadiusAndSetZAndIncState_hook(GB *gb);
void forestFairy_subid02_hook(GB *gb);
void forestFairy_subid03_hook(GB *gb);
void forestFairy_subid04_hook(GB *gb);
void forestFairy_subid03State1_hook(GB *gb);
void forestFairy_subid03State2_hook(GB *gb);
void forestFairy_subid03State3_hook(GB *gb);
void forestFairy_subid04State1_hook(GB *gb);
void forestFairy_subid05_hook(GB *gb);
void forestFairy_initNpcFromData_hook(GB *gb);
void forestFairy_standardUpdate_hook(GB *gb);
void forestFairy_subid08_hook(GB *gb);
void forestFairy_subid0b_hook(GB *gb);
void forestFairy_subid0c_hook(GB *gb);
void forestFairy_standardUpdate_2_hook(GB *gb);
void forestFairy_subid0e_hook(GB *gb);

void interactionCode49_hook(GB *gb) {
  BASE(interactionCode49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = 0x44;
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (forest_fairy_jump_table(gb));
    if (jt_ == SYM(forestFairy_subid00)) { forestFairy_subid00_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid01)) { forestFairy_subid01_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid02)) { forestFairy_subid02_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid03)) { forestFairy_subid03_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid04)) { forestFairy_subid04_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid05)) { forestFairy_subid05_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid08)) { forestFairy_subid08_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid0b)) { forestFairy_subid0b_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid0c)) { forestFairy_subid0c_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid0e)) { forestFairy_subid0e_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void forestFairy_subid00_hook(GB *gb) {
  BASE(forestFairy_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (forest_fairy_jump_table(gb));
    if (jt_ == SYM(forestFairy_subid00State0)) { forestFairy_subid00State0_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid00State1)) { forestFairy_subid00State1_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid00State2)) { forestFairy_subid00State2_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid00State3)) { forestFairy_subid00State3_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_deleteSelf)) { interactionDelete_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void forestFairy_subid00State0_hook(GB *gb) {
  BASE(forestFairy_subid00State0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_ROM(b_+3, SYM(forestFairy_initCollisionRadiusAndSetZAndIncState));
  CYC(b_+6, b_+8); L = 0x50;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x50);
  CYC(b_+10, b_+12); L = 0x7a;
  CYC(b_+12, SYM(forestFairy_loadMovementPreset)); mem_wr(gb, HL, 0x5a);
  forestFairy_loadMovementPreset_hook(gb);
}

void forestFairy_loadMovementPreset_hook(GB *gb) {
  BASE(forestFairy_loadMovementPreset);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x43;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+7); SET_HL(b_+69);
  CYC(b_+7, b_+8); forest_fairy_add_double_index(gb, b_+8);
  CYC(b_+8, b_+10); E = 0x4b;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_and(gb, 0xf8);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+16); E = 0x49;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+19); alu_and(gb, 7);
  CYC(b_+19, b_+20); alu_add(gb, A);
  CYC(b_+20, b_+21); alu_add(gb, A);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+24); E = 0x4d;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_and(gb, 0xf8);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+30); E = 0x46;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+33); alu_and(gb, 7);
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);
  CYC(b_+36, b_+37); mem_wr(gb, DE, A);
  CYC(b_+37, b_+39); E = 0x78;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_and(gb, 0xf8);
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+45); E = 0x48;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+46, b_+48); alu_and(gb, 1);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+51); E = 0x79;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_and(gb, 0xf8);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = 0x5c;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+60); alu_and(gb, 7);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+62); E = alu_dec8(gb, E);
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+65); E = 0x48;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+69); interactionSetAnimation_hook(gb);
}

void forestFairy_subid00State1_hook(GB *gb) {
  BASE(forestFairy_subid00State1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x78;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); C = mem_rd(gb, HL);
  CYC(b_+6, b_+8); L = 0x4b;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); mem_wr(gb, hFF8F, A);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); mem_wr(gb, hFF8E, A);
  CYC(b_+15, b_+16); alu_sub(gb, C);
  CYC(b_+16, b_+18); alu_add(gb, 4);
  CYC(b_+18, b_+20); alu_cp(gb, 9);
  if (!(F & FC)) { CYCT(b_+20, b_+22); goto outside; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = mem_rd(gb, hFF8F);
  CYC(b_+24, b_+25); alu_sub(gb, B);
  CYC(b_+25, b_+27); alu_add(gb, 4);
  CYC(b_+27, b_+29); alu_cp(gb, 9);
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto outside; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); E = 0x42;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+36); alu_cp(gb, 3);
  if (!(F & FC)) { CYCT(b_+36, b_+38); goto found; }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); mem_wr(gb, HL, C);
  CYC(b_+39, b_+41); L = 0x4b;
  CYC(b_+41, b_+42); mem_wr(gb, HL, B);
  CYC(b_+42, b_+44); L = 0x44;
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
found:
  CYC(b_+45, b_+48); SET_HL(wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+49, b_+50); alu_scf(gb);
  CYC(b_+50, b_+51); ret_effect(gb);
  return;
outside:
  CYC(b_+51, b_+53); L = 0x7a;
  CYC(b_+53, b_+54); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+54, b_+55); A = mem_rd(gb, HL);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto animation; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+60); mem_wr(gb, HL, 0x5a);
  CYC(b_+60, b_+62); L = 0x47;
  CYC(b_+62, b_+64); mem_wr(gb, HL, alu_srl(gb, mem_rd(gb, HL)));
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto decrement; }
  CYC(b_+64, b_+66);
  CYC(b_+65, b_+66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
animation:
  CYC(b_+66, b_+68); alu_and(gb, 7);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto decrement; }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); push_effect(gb, BC);
  CYC(b_+71, b_+74); SET_BC(0x8402);
  CALL_C(b_+74, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+77);
  CYC(b_+77, b_+78); SET_BC(pop_effect(gb));
decrement:
  CALL_C(b_+78, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); forestFairy_updateMovement_hook(gb); return; }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+84); L = alu_inc8(gb, L);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);
  CALL_C(b_+86, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+89);
  CALL_C(b_+89, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), SYM(forestFairy_updateMovement));
  forestFairy_updateMovement_hook(gb);
}

void forestFairy_updateMovement_hook(GB *gb) {
  BASE(forestFairy_updateMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x1f);
  CYC(b_+8, b_+10); A = 0x83;
  if (F & FZ) CALL_C_CC(b_+10, playSound_b00_hook, SYM(playSound_b00), SYM(forestFairy_animate));
  else CYC(b_+10, SYM(forestFairy_animate));
  forestFairy_animate_hook(gb);
}

void forestFairy_animate_hook(GB *gb) {
  BASE(forestFairy_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+4); alu_or(gb, D);
  CYC(b_+4, SYM(forestFairy_subid00State2)); ret_effect(gb);
}

void forestFairy_subid00State2_hook(GB *gb) {
  BASE(forestFairy_subid00State2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); forestFairy_animate_hook(gb); return; }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = 0x43;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 6);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto delete; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_add(gb, 6);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);
  CYC(b_+19, b_+22); forestFairy_loadMovementPreset_hook(gb);
  return;
delete:
  CALL_C(b_+22, objectCreatePuff_hook, SYM(objectCreatePuff), b_+25);
  CYC(b_+25, SYM(forestFairy_subid00State3)); interactionDelete_hook(gb);
}

void forestFairy_subid00State3_hook(GB *gb) {
  BASE(forestFairy_subid00State3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3); forestFairy_subid00State1_hook(gb);
  if (F & FC) { CYCT(b_+3, b_+5); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = 0x4b;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto done; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = 0x4d;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0xa0);
  if (F & FC) { CYCT(b_+17, b_+18); ret_effect(gb); return; }
  CYC(b_+17, b_+18);
done:
  CYC(b_+18, b_+21); SET_HL(wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+21, SYM(forestFairy_deleteSelf)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(SYM(forestFairy_deleteSelf), SYM(forestFairy_subid01)); interactionDelete_hook(gb);
}

void forestFairy_deleteSelf_hook(GB *gb) {
  BASE(forestFairy_deleteSelf);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, SYM(forestFairy_subid01)); interactionDelete_hook(gb);
}

void forestFairy_initCollisionRadiusAndSetZAndIncState_hook(GB *gb) {
  BASE(forestFairy_initCollisionRadiusAndSetZAndIncState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionIncState_hook, SYM(interactionIncState), b_+3);
  CYC(b_+3, b_+5); L = 0x66;
  CYC(b_+5, b_+7); A = 4;
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = 0x4f;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xfc);
  CYC(b_+13, SYM(forestFairyDiscoveredScriptTable)); objectSetVisiblec1_hook(gb);
}

void forestFairy_subid02_hook(GB *gb) {
  BASE(forestFairy_subid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, SYM(forestFairy_subid03)); interactionDelete_hook(gb);
}

void forestFairy_subid03_hook(GB *gb) {
  BASE(forestFairy_subid03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (forest_fairy_jump_table(gb));
    if (jt_ == SYM(forestFairy_subid00State0)) { forestFairy_subid00State0_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid03State1)) { forestFairy_subid03State1_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid03State2)) { forestFairy_subid03State2_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid03State3)) { forestFairy_subid03State3_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid00State3)) { forestFairy_subid00State3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void forestFairy_subid04_hook(GB *gb) {
  BASE(forestFairy_subid04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (forest_fairy_jump_table(gb));
    if (jt_ == SYM(forestFairy_subid00State0)) { forestFairy_subid00State0_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid04State1)) { forestFairy_subid04State1_hook(gb); return; }
    else if (jt_ == SYM(forestFairy_subid00State3)) { forestFairy_subid00State3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void forestFairy_subid03State1_hook(GB *gb) {
  BASE(forestFairy_subid03State1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3); forestFairy_subid00State1_hook(gb);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; }
  CYC(b_+3, b_+4);
  CALL_C(b_+4, interactionIncState_hook, SYM(interactionIncState), b_+7);
  CYC(b_+7, b_+9); A = 2;
  CYC(b_+9, b_+11); L = 0x46;
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+15); L = 0x7b;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x20);
  CYC(b_+17, SYM(forestFairy_subid03State2)); ret_effect(gb);
}

void forestFairy_subid03State2_hook(GB *gb) {
  BASE(forestFairy_subid03State2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7a;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+5); A = mem_rd(gb, HL);
  CYC(b_+5, b_+7); alu_and(gb, 7);
  if (F & FZ) {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+10); push_effect(gb, BC);
    CYC(b_+10, b_+13); SET_BC(0x8402);
    CALL_C(b_+13, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+16);
    CYC(b_+16, b_+17); SET_BC(pop_effect(gb));
  } else CYCT(b_+7, b_+9);
  CALL_C(b_+17, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+20);
  if (F & FZ) {
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+23); L = alu_dec8(gb, L);
    CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+25, b_+26); L = alu_inc8(gb, L);
    CYC(b_+26, b_+27); A = mem_rd(gb, HL);
    CYC(b_+27, b_+28); A = alu_inc8(gb, A);
    CYC(b_+28, b_+30); alu_and(gb, 0x1f);
    CYC(b_+30, b_+31); mem_wr(gb, HL, A);
    CYC(b_+31, b_+33); L = 0x7b;
    CYC(b_+33, b_+34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) {
      CYC(b_+34, b_+36);
      CYC(b_+36, b_+37); L = E;
      CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+38, b_+41); SET_HL(wTmpcfc0_fairyHideAndSeek_cfd2);
      CYC(b_+41, b_+42); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+42, b_+43); ret_effect(gb); return;
    }
    CYCT(b_+34, b_+36);
  } else CYCT(b_+20, b_+22);
  CYC(b_+43, SYM(forestFairy_subid03State3)); forestFairy_updateMovement_hook(gb);
}

void forestFairy_subid03State3_hook(GB *gb) {
  BASE(forestFairy_subid03State3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); forestFairy_animate_hook(gb); return; }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, interactionIncState_hook, SYM(interactionIncState), b_+10);
  CYC(b_+10, b_+12); L = 0x43;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = 0x4b;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); C = mem_rd(gb, HL);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); push_effect(gb, BC);
  CALL_C(b_+20, forestFairy_loadMovementPreset_hook, SYM(forestFairy_loadMovementPreset), b_+23);
  CYC(b_+23, b_+24); SET_BC(pop_effect(gb));
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = 0x4b;
  CYC(b_+27, b_+28); mem_wr(gb, HL, B);
  CYC(b_+28, b_+30); L = 0x4d;
  CYC(b_+30, b_+31); mem_wr(gb, HL, C);
  CYC(b_+31, SYM(forestFairy_subid04State1)); ret_effect(gb);
}

void forestFairy_subid04State1_hook(GB *gb) {
  BASE(forestFairy_subid04State1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_fairyHideAndSeek_cfd2);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); forestFairy_animate_hook(gb); return; }
  CYC(b_+4, b_+7);
  CALL_C(b_+7, interactionIncState_hook, SYM(interactionIncState), b_+10);
  CYC(b_+10, SYM(forestFairy_subid05)); forestFairy_loadMovementPreset_hook(gb);
}

void forestFairy_subid01_hook(GB *gb) {
  BASE(forestFairy_subid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) goto state_zero;
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+7); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+11); interactionDelete_hook(gb); return; }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14); SET_HL(w1Link);
  CALL_C(b_+14, preventObjectHFromPassingObjectD_hook, SYM(preventObjectHFromPassingObjectD), b_+17);
  CALL_C(b_+17, interactionAnimate_hook, SYM(interactionAnimate), b_+20);
  CYC(b_+20, b_+23); interactionRunScript_hook(gb); return;
state_zero:
  CYCT(b_+2, b_+4);
  CYC(b_+23, b_+25); E = 0x43;
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+29); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(b_+29, checkFlag_hook, SYM(checkFlag), b_+32);
  if (F & FZ) { CYCT(b_+32, b_+35); interactionDelete_hook(gb); return; }
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+38); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(b_+38, getNumSetBits_hook, SYM(getNumSetBits), b_+41);
  CYC(b_+41, b_+42); A = alu_dec8(gb, A);
  CYC(b_+42, b_+45); SET_HL(SYM(forestFairyDiscoveredScriptTable));
  CYC(b_+45, b_+46); forest_fairy_add_double_index(gb, b_+46);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+47, b_+48); H = mem_rd(gb, HL);
  CYC(b_+48, b_+49); L = A;
  CALL_C(b_+49, interactionSetScript_hook, SYM(interactionSetScript), b_+52);
  CALL_C(b_+52, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+55);
  CYC(b_+55, b_+57); E = 0x43;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); B = A;
  CYC(b_+59, b_+60); A = alu_inc8(gb, A);
  CYC(b_+60, b_+62); E = 0x5c;
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+64); E = alu_dec8(gb, E);
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CYC(b_+65, b_+66); A = B;
  CYC(b_+66, b_+69); SET_HL(SYM(forestFairy_discoveredPositions));
  CYC(b_+69, b_+70); forest_fairy_add_double_index(gb, b_+70);
  CYC(b_+70, b_+72); E = 0x4b;
  CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+76); E = 0x4d;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  CYC(b_+78, b_+79); A = B;
  CYC(b_+79, b_+80); alu_or(gb, A);
  if (F & FZ) CYC(b_+80, b_+82);
  else { CYCT(b_+80, b_+82); CYC(b_+82, b_+84); A = 1; }
  CALL_C(b_+84, interactionSetAnimation_hook, SYM(interactionSetAnimation), SYM(forestFairy_initCollisionRadiusAndSetZAndIncState));
  forestFairy_initCollisionRadiusAndSetZAndIncState_hook(gb);
}

void forestFairy_initNpcFromData_hook(GB *gb) {
  BASE(forestFairy_initNpcFromData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CALL_C(b_+1, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+4);
  CYC(b_+4, b_+5); SET_HL(pop_effect(gb));
  CYC(b_+5, b_+7); E = 0x72;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); E = 0x5b;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_and(gb, 0x0f);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); E = alu_inc8(gb, E);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_and(gb, 0xf0);
  CYC(b_+20, b_+22); A = alu_swap(gb, A);
  CALL_C(b_+22, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+25);
  CALL_C(b_+25, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+28);
  CALL_C(b_+28, interactionIncState_hook, SYM(interactionIncState), b_+31);
  CYC(b_+31, b_+33); L = 0x4f;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xfc);
  CYC(b_+35, b_+37); L = 0x73;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x11);
  CYC(b_+39, b_+42); SET_HL((SYM(ambi_runSubid05) + 11));
  CALL_C(b_+42, interactionSetScript_hook, SYM(interactionSetScript), b_+45);
  CYC(b_+45, SYM(forestFairy_subid5To7NpcData)); objectSetVisiblec1_hook(gb);
}

void forestFairy_standardUpdate_hook(GB *gb) {
  BASE(forestFairy_standardUpdate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CALL_C(b_+3, interactionAnimate_hook, SYM(interactionAnimate), b_+6);
  CYC(b_+6, SYM(forestFairy_subid08)); objectPreventLinkFromPassing_hook(gb);
}

void forestFairy_subid05_hook(GB *gb) {
  BASE(forestFairy_subid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); forestFairy_standardUpdate_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x0e;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x2b;
  CALL_C(b_+15, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+18);
  if (F & FZ) { CYCT(b_+18, b_+21); interactionDelete_hook(gb); return; }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, (wGroup0RoomFlags + 144));
  CYC(b_+24, b_+26); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); interactionDelete_hook(gb); return; }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+31); E = 0x42;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_sub(gb, 5);
  CYC(b_+34, b_+37); SET_HL(SYM(forestFairy_subid5To7NpcData));
  CYC(b_+37, SYM(forestFairy_initNpcFromData)); forest_fairy_add_double_index(gb, SYM(forestFairy_initNpcFromData));
  forestFairy_initNpcFromData_hook(gb);
}

void forestFairy_subid08_hook(GB *gb) {
  BASE(forestFairy_subid08);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); forestFairy_standardUpdate_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x0e;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x2b;
  CALL_C(b_+15, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+18);
  if (F & FZ) { CYCT(b_+18, b_+21); interactionDelete_hook(gb); return; }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); A = mem_rd(gb, (wGroup0RoomFlags + 144));
  CYC(b_+24, b_+26); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+26, b_+29); interactionDelete_hook(gb); return; }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+31); A = 0x14;
  CALL_C(b_+31, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+34);
  if (!(F & FZ)) { CYCT(b_+34, b_+37); interactionDelete_hook(gb); return; }
  CYC(b_+34, b_+37);
  CYC(b_+37, b_+39); E = 0x42;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_sub(gb, 8);
  CYC(b_+42, b_+45); SET_HL(b_+49);
  CYC(b_+45, b_+46); forest_fairy_add_double_index(gb, b_+46);
  CYC(b_+46, b_+49); forestFairy_initNpcFromData_hook(gb);
}

void forestFairy_subid0b_hook(GB *gb) {
  BASE(forestFairy_subid0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); forestFairy_standardUpdate_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x14;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; }
  CYC(b_+10, b_+13);
  CALL_C(b_+13, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+16);
  CALL_C(b_+16, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+19);
  CALL_C(b_+19, interactionIncState_hook, SYM(interactionIncState), b_+22);
  CYC(b_+22, b_+24); L = 0x4f;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xfc);
  CYC(b_+26, b_+28); L = 0x5c;
  CYC(b_+28, b_+30); A = 1;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+35); SET_HL((SYM(ambi_runSubid05__data) + 4));
  CALL_C(b_+35, interactionSetScript_hook, SYM(interactionSetScript), b_+38);
  CYC(b_+38, SYM(forestFairy_subid0c)); objectSetVisiblec1_hook(gb);
}

void forestFairy_subid0c_hook(GB *gb) {
  BASE(forestFairy_subid0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), SYM(forestFairy_standardUpdate_2));
  forestFairy_standardUpdate_2_hook(gb);
}

void forestFairy_standardUpdate_2_hook(GB *gb) {
  BASE(forestFairy_standardUpdate_2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); forestFairy_standardUpdate_hook(gb); return; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); A = 0x14;
  CALL_C(b_+4, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+7);
  if (F & FZ) { CYCT(b_+7, b_+10); interactionDelete_hook(gb); return; }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); E = 0x42;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_sub(gb, 0x0c);
  CYC(b_+15, b_+18); SET_HL(b_+22);
  CYC(b_+18, b_+19); forest_fairy_add_double_index(gb, b_+19);
  CYC(b_+19, b_+22); forestFairy_initNpcFromData_hook(gb);
}

void forestFairy_subid0e_hook(GB *gb) {
  BASE(forestFairy_subid0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); forestFairy_standardUpdate_2_hook(gb); return; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x23;
  CALL_C(b_+7, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); interactionDelete_hook(gb); return; }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x2b;
  CALL_C(b_+15, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); interactionDelete_hook(gb); return; }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); A = 0x42;
  CALL_C(b_+23, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+26);
  if (F & FZ) { CYCT(b_+26, b_+29); interactionDelete_hook(gb); return; }
  CYC(b_+26, b_+29);
  CYC(b_+29, b_+31); E = 0x42;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_sub(gb, 0x0e);
  CYC(b_+34, b_+37); SET_HL(b_+41);
  CYC(b_+37, b_+38); forest_fairy_add_double_index(gb, b_+38);
  CYC(b_+38, b_+41); forestFairy_initNpcFromData_hook(gb);
}
