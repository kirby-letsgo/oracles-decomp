#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionCode4a_hook(GB *gb);
void introSpriteIncStateAndLoadGraphics_hook(GB *gb);
void introSpriteFunc_461a_hook(GB *gb);
void introSpritesState1_hook(GB *gb);
void introSpriteTriforceSubid_hook(GB *gb);
void introSpriteRunSubid07_hook(GB *gb);
void introSpriteRunTriforceGlowSubid_hook(GB *gb);
void introSpriteRunSubid04_hook(GB *gb);
void introSpriteRunSubid08_hook(GB *gb);
void introSpriteSetChildRelatedObject1ToSelf_hook(GB *gb);

static uint16_t introSprites1_jumpTable(GB *gb) {
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

static void introSprites1_addDoubleIndex(GB *gb, uint16_t returnAddress) {
  push_effect(gb, returnAddress);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void introSprite_addRandomVariance_hook(GB *gb) {
  BASE(introSpriteFunc_461a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+60, b_+61); B = A;
  CALL_C(b_+61, getRandomNumber_hook, SYM(getRandomNumber), b_+64);
  CYC(b_+64, b_+66); alu_and(gb, 3);
  CYC(b_+66, b_+68); alu_sub(gb, 2);
  CYC(b_+68, b_+69); alu_add(gb, B);
  CYC(b_+69, b_+70); ret_effect(gb);
}

void interactionCode4a_hook(GB *gb) {
  BASE(interactionCode4a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (introSprites1_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == SYM(introSpritesState1)) { introSpritesState1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+8, introSpriteIncStateAndLoadGraphics_hook, SYM(introSpriteIncStateAndLoadGraphics), b_+11);
  CYC(b_+11, b_+13); E = 0x42;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); push_effect(gb, b_+15);
  do { uint16_t jt_ = (introSprites1_jumpTable(gb));
    if (jt_ == b_+37) { goto initTriforce; }
    else if (jt_ == b_+59) { goto initMovingSprite; }
    else if (jt_ == b_+78) { goto initTree; }
    else if (jt_ == b_+123) { goto initGlow; }
    else if (jt_ == SYM(introSpriteIncStateAndLoadGraphics)) { introSpriteIncStateAndLoadGraphics_hook(gb); return; }
    else if (jt_ == SYM(objectSetVisible82)) { objectSetVisible82_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

initTriforce:
  CALL_C(b_+37, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+40);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto makeVisible82; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x4a);
  CYC(b_+44, b_+45); L = alu_inc8(gb, L);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 4);
  CYC(b_+47, b_+48); L = alu_inc8(gb, L);
  CYC(b_+48, b_+50); E = 0x42;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CALL_C(b_+53, introSpriteSetChildRelatedObject1ToSelf_hook, SYM(introSpriteSetChildRelatedObject1ToSelf), b_+56);
makeVisible82:
  CYC(b_+56, b_+59); objectSetVisible82_hook(gb);
  return;

initMovingSprite:
  CYC(b_+59, b_+61); E = 0x43;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_add(gb, A);
  CYC(b_+63, b_+64); alu_add(gb, A);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = 0x60;
  CYC(b_+67, b_+68); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+68, b_+69); mem_wr(gb, HL, A);
  CALL_C(b_+69, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+72);
  CALL_C(b_+72, introSpriteFunc_461a_hook, SYM(introSpriteFunc_461a), b_+75);
  CYC(b_+75, b_+78); objectSetVisible80_hook(gb);
  return;

initTree:
  CYC(b_+78, b_+80); E = 0x43;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+84); SET_HL(b_+117);
  CYC(b_+84, b_+85); introSprites1_addDoubleIndex(gb, b_+85);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+86, b_+88); E = 0x4b;
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+90); E = alu_inc8(gb, E);
  CYC(b_+90, b_+91); E = alu_inc8(gb, E);
  CYC(b_+91, b_+92); A = mem_rd(gb, HL);
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+95); B = 3;
spawnTreeBranches:
  CALL_C(b_+95, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto treeVisible82; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x4a);
  CYC(b_+102, b_+103); L = alu_inc8(gb, L);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x0a);
  CYC(b_+105, b_+106); L = alu_inc8(gb, L);
  CYC(b_+106, b_+107); mem_wr(gb, HL, B);
  CYC(b_+107, b_+108); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+108, introSpriteSetChildRelatedObject1ToSelf_hook, SYM(introSpriteSetChildRelatedObject1ToSelf), b_+111);
  CYC(b_+111, b_+112); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+112, b_+114); goto spawnTreeBranches; }
  CYC(b_+112, b_+114);
treeVisible82:
  CYC(b_+114, b_+117); objectSetVisible82_hook(gb);
  return;

initGlow:
  CALL_C(b_+123, objectSetVisible83_hook, SYM(objectSetVisible83), b_+126);
  CYC(b_+126, b_+128); alu_xor(gb, 0x80);
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+130); ret_effect(gb);
}

void introSpriteIncStateAndLoadGraphics_hook(GB *gb) {
  BASE(introSpriteIncStateAndLoadGraphics);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x44;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); interactionInitGraphics_hook(gb);
}

void introSpriteFunc_461a_hook(GB *gb) {
  BASE(introSpriteFunc_461a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+3);
  CALL_C(b_+3, objectTakePosition_hook, SYM(objectTakePosition), b_+6);
  CYC(b_+6, b_+7); push_effect(gb, BC);
  CYC(b_+7, b_+9); E = 0x42;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+13); SET_HL(b_+70);
  CYC(b_+13, b_+15); alu_cp(gb, 3);
  if (F & FZ) {
    CYCT(b_+15, b_+17);
  } else {
    CYC(b_+15, b_+17);
    CYC(b_+17, b_+19); alu_cp(gb, 0x0a);
    if (F & FZ) {
      CYCT(b_+19, b_+21);
    } else {
      CYC(b_+19, b_+21);
      CYC(b_+21, b_+24); SET_HL(b_+76);
      CYC(b_+24, b_+26); E = 0x47;
      CYC(b_+26, b_+27); A = mem_rd(gb, DE);
      CYC(b_+27, b_+28); A = alu_inc8(gb, A);
      CYC(b_+28, b_+29); mem_wr(gb, DE, A);
      CYC(b_+29, b_+31); alu_and(gb, 3);
      CYC(b_+31, b_+32); C = A;
      CYC(b_+32, b_+33); alu_add(gb, A);
      CYC(b_+33, b_+34); alu_add(gb, C);
      CYC(b_+34, b_+35); introSprites1_addDoubleIndex(gb, b_+35);
    }
  }
  CYC(b_+35, b_+37); E = 0x43;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); introSprites1_addDoubleIndex(gb, b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+40, b_+43); push_effect(gb, b_+43); introSprite_addRandomVariance_hook(gb);
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+46); E = 0x4b;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_add(gb, B);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); A = mem_rd(gb, HL);
  CYC(b_+50, b_+53); push_effect(gb, b_+53); introSprite_addRandomVariance_hook(gb);
  CYC(b_+53, b_+54); H = D;
  CYC(b_+54, b_+56); L = 0x4d;
  CYC(b_+56, b_+57); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+57, b_+58); mem_wr(gb, HL, A);
  CYC(b_+58, b_+59); SET_BC(pop_effect(gb));
  CYC(b_+59, b_+60); ret_effect(gb);
}

void introSpritesState1_hook(GB *gb) {
  BASE(introSpritesState1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 5);
  if (!(F & FC)) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+10); A = W8(wIntro_triforceState);
    CYC(b_+10, b_+12); alu_cp(gb, 4);
    if (F & FZ) { CYCT(b_+12, b_+15); interactionDelete_hook(gb); return; }
    CYC(b_+12, b_+15);
  }
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (introSprites1_jumpTable(gb));
    if (jt_ == SYM(introSpriteTriforceSubid)) { introSpriteTriforceSubid_hook(gb); return; }
    else if (jt_ == SYM(introSpriteRunTriforceGlowSubid)) { introSpriteRunTriforceGlowSubid_hook(gb); return; }
    else if (jt_ == SYM(introSpriteRunSubid04)) { introSpriteRunSubid04_hook(gb); return; }
    else if (jt_ == SYM(introSpriteRunSubid07)) { introSpriteRunSubid07_hook(gb); return; }
    else if (jt_ == SYM(introSpriteRunSubid08)) { introSpriteRunSubid08_hook(gb); return; }
    else if (jt_ == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void introSpriteTriforceSubid_hook(GB *gb) {
  BASE(introSpriteTriforceSubid);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (introSprites1_jumpTable(gb));
    if (jt_ == b_+16) { goto substate0; }
    else if (jt_ == b_+44) { goto substate1; }
    else if (jt_ == b_+94) { goto substate2; }
    else if (jt_ == b_+116) { goto substate3; }
    else if (jt_ == b_+148) { goto substate4; }
    else if (jt_ == SYM(interactionAnimate)) { interactionAnimate_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CYC(b_+16, b_+19); A = W8(wIntro_triforceState);
  CYC(b_+19, b_+21); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+21, b_+24); interactionAnimate_hook(gb); return; }
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); B = 0;
  CYC(b_+26, b_+28); E = 0x42;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+31); alu_cp(gb, 1);
  if (F & FZ) { CYCT(b_+31, b_+33); } else { CYC(b_+31, b_+33); CYC(b_+33, b_+35); B = 0x0a; }
  CALL_C(b_+35, func_2d48_hook, SYM(func_2d48), b_+38);
  CALL_C(b_+38, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+41);
  CYC(b_+41, b_+43); L = 0x46;
  CYC(b_+43, b_+44); mem_wr(gb, HL, B);

substate1:
  CALL_C(b_+44, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+47);
  if (!(F & FZ)) { CYCT(b_+47, b_+50); interactionAnimate_hook(gb); return; }
  CYC(b_+47, b_+50);
  CYC(b_+50, b_+52); L = 0x42;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_cp(gb, 1);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto centerPiece; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); L = 0x49;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0);
  CYC(b_+61, b_+63); L = 0x50;
  CYC(b_+63, b_+65); mem_wr(gb, HL, 5);
  CYC(b_+65, b_+67); B = 1;
  CYC(b_+67, b_+69); goto afterAngle;

centerPiece:
  CYC(b_+69, b_+70); alu_or(gb, A);
  CYC(b_+70, b_+72); A = 0x18;
  if (F & FZ) { CYCT(b_+72, b_+74); } else { CYC(b_+72, b_+74); CYC(b_+74, b_+76); A = 8; }
  CYC(b_+76, b_+78); L = 0x49;
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+81); L = 0x50;
  CYC(b_+81, b_+83); mem_wr(gb, HL, 5);
  CYC(b_+83, b_+85); B = 0x0b;

afterAngle:
  CALL_C(b_+85, func_2d48_hook, SYM(func_2d48), b_+88);
  CALL_C(b_+88, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+91);
  CYC(b_+91, b_+93); L = 0x46;
  CYC(b_+93, b_+94); mem_wr(gb, HL, B);

substate2:
  CALL_C(b_+94, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+97);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto applySpeed; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+101); B = 2;
  CALL_C(b_+101, func_2d48_hook, SYM(func_2d48), b_+104);
  CALL_C(b_+104, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+107);
  CYC(b_+107, b_+109); L = 0x46;
  CYC(b_+109, b_+110); mem_wr(gb, HL, B);
applySpeed:
  CALL_C(b_+110, objectApplySpeed_hook, SYM(objectApplySpeed), b_+113);
  CYC(b_+113, b_+116); interactionAnimate_hook(gb);
  return;

substate3:
  CALL_C(b_+116, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+119);
  if (!(F & FZ)) { CYCT(b_+119, b_+122); interactionAnimate_hook(gb); return; }
  CYC(b_+119, b_+122);
  CYC(b_+122, b_+124); B = 3;
  CALL_C(b_+124, func_2d48_hook, SYM(func_2d48), b_+127);
  CALL_C(b_+127, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+130);
  CYC(b_+130, b_+132); L = 0x46;
  CYC(b_+132, b_+133); mem_wr(gb, HL, B);
  CYC(b_+133, b_+135); E = 0x42;
  CYC(b_+135, b_+136); A = mem_rd(gb, DE);
  CYC(b_+136, b_+138); alu_cp(gb, 1);
  if (F & FZ) { CYCT(b_+138, b_+140); CYC(b_+143, b_+145); A = 0x5c; CYC(b_+145, b_+148); playSound_b00_hook(gb); return; }
  CYC(b_+138, b_+140);
  CYC(b_+140, b_+143); interactionIncSubstate_hook(gb);
  return;

substate4:
  CALL_C(b_+148, interactionAnimate_hook, SYM(interactionAnimate), b_+151);
  CALL_C(b_+151, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+154);
  if (!(F & FZ)) { CYCT(b_+154, b_+155); ret_effect(gb); return; }
  CYC(b_+154, b_+155);
  CALL_C(b_+155, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+158);
  CYC(b_+158, b_+160); A = 2;
  CYC(b_+160, b_+163); W8(wIntro_triforceState) = A;
  CYC(b_+163, b_+165); A = 0x7c;
  CYC(b_+165, b_+168); playSound_b00_hook(gb);
}

void introSpriteRunSubid07_hook(GB *gb) {
  BASE(introSpriteRunSubid07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectSetVisible_hook, SYM(objectSetVisible), b_+3);
  CYC(b_+3, b_+5); E = 0x43;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 1);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+12); A = W8(wIntro_frameCounter);
  CYC(b_+12, b_+14); alu_and(gb, 1);
  CYC(b_+14, b_+15); alu_xor(gb, B);
  if (F & FZ) CALL_C_CC(b_+15, objectSetInvisible_hook, SYM(objectSetInvisible), SYM(introSpriteRunTriforceGlowSubid)); else CYC(b_+15, b_+18);
  introSpriteRunTriforceGlowSubid_hook(gb);
}

void introSpriteRunTriforceGlowSubid_hook(GB *gb) {
  BASE(introSpriteRunTriforceGlowSubid);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x61;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (F & FZ) CALL_C_CC(b_+4, introSpriteFunc_461a_hook, SYM(introSpriteFunc_461a), b_+7); else CYC(b_+4, b_+7);
  CYC(b_+7, b_+10); interactionAnimate_hook(gb);
}

void introSpriteRunSubid04_hook(GB *gb) {
  BASE(introSpriteRunSubid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+5); A = 0;
  CALL_C(b_+5, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+8);
  CALL_C(b_+8, objectTakePosition_hook, SYM(objectTakePosition), b_+11);
  CYC(b_+11, b_+13); E = 0x43;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = 0x60;
  CYC(b_+17, b_+18); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+18, b_+20); L = 0x5a;
  if (!(F & FZ)) { CYCT(b_+20, b_+22); CYC(b_+25, b_+27); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f); CYC(b_+27, b_+28); ret_effect(gb); return; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+24, b_+25); ret_effect(gb);
}

void introSpriteRunSubid08_hook(GB *gb) {
  BASE(introSpriteRunSubid08);
  CYC(b_+0, b_+3); A = W8(wGfxRegs1_SCY);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); interactionDelete_hook(gb); return; }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); E = 0x4a;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_sub(gb, B);
  CYC(b_+12, b_+13); E = alu_inc8(gb, E);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void introSpriteSetChildRelatedObject1ToSelf_hook(GB *gb) {
  BASE(introSpriteSetChildRelatedObject1ToSelf);
  CYC(b_+0, b_+2); L = 0x56;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x40);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); mem_wr(gb, HL, D);
  CYC(b_+6, b_+7); ret_effect(gb);
}
