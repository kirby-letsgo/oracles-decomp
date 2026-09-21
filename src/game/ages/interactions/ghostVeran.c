#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t ghost_veran_jump_table(GB *gb) {
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

static void ghost_veran_rumble_and_randomize_x(GB *gb, uint16_t sp0_) {
  BASE(runVeranGhostSubid0);
  CYC(b_+209, b_+212); A = mem_rd(gb, wFrameCounter);
  CYC(b_+212, b_+214); alu_and(gb, 0x0f);
  CYC(b_+214, b_+216); A = 0xb8;
  if (F & FZ) CALL_C_CC(b_+216, playSound_b00_hook, SYM(playSound_b00), b_+219); else CYC(b_+216, b_+219);
  CALL_C(b_+219, getRandomNumber_hook, SYM(getRandomNumber), b_+222);
  CYC(b_+222, b_+223); alu_and(gb, B);
  CYC(b_+223, b_+224); alu_sub(gb, C);
  CYC(b_+224, b_+225); H = D;
  CYC(b_+225, b_+227); L = 0x7d;
  CYC(b_+227, b_+228); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+228, b_+230); L = 0x4d;
  CYC(b_+230, b_+231); mem_wr(gb, HL, A);
  CYC(b_+231, b_+232); ret_effect(gb);
}

static void ghost_veran_update_script_position(GB *gb, uint16_t sp0_, uint16_t call, uint16_t ret) {
  BANKOF(runVeranGhostSubid0);
  CALL_C(call, objectGetPosition_hook, SYM(objectGetPosition), ret);
  CYC(ret, ret + 3); SET_HL(wTmpcfc0_genericCutscene_cfd5);
  CYC(ret + 3, ret + 4); mem_wr(gb, HL, B);
  CYC(ret + 4, ret + 5); L = alu_inc8(gb, L);
  CYC(ret + 5, ret + 7); E = 0x7d;
  CYC(ret + 7, ret + 8); A = C;
  CYC(ret + 8, ret + 9); mem_wr(gb, DE, A);
  CYC(ret + 9, ret + 10); mem_wr(gb, HL, A);
}

void interactionCode3e_hook(GB *gb) {
  BASE(interactionCode3e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ghost_veran_jump_table(gb));
    if (jt_ == b_+8) {
      CYC(b_+8, b_+10); A = 1;
      CYC(b_+10, b_+11); mem_wr(gb, DE, A);
      CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
      CALL_C(b_+14, objectSetVisible83_hook, SYM(objectSetVisible83), b_+17);
      CYC(b_+17, b_+19); E = 0x42;
      CYC(b_+19, b_+20); A = mem_rd(gb, DE);
      CYC(b_+20, b_+21); push_effect(gb, b_+21);
      do { uint16_t jt_ = (ghost_veran_jump_table(gb));
        if (jt_ == b_+27) {
          CYC(b_+27, b_+29); E = 0x46;
          CYC(b_+29, b_+31); A = 0x78;
          CYC(b_+31, b_+32); mem_wr(gb, DE, A);
          CYC(b_+32, b_+35); TAIL(interactionSetAlwaysUpdateBit);
        }
        else if (jt_ == b_+35) {
          CYC(b_+35, b_+36); H = D;
          CYC(b_+36, b_+38); L = 0x49;
          CYC(b_+38, b_+40); mem_wr(gb, HL, 0x10);
          CYC(b_+40, b_+42); L = 0x50;
          CYC(b_+42, b_+44); mem_wr(gb, HL, 0x1e);
          CYC(b_+44, b_+47); SET_HL((SYM(forestFairy_subid00State3) + 9));
          CALL_C(b_+47, interactionSetScript_hook, SYM(interactionSetScript), b_+50);
          CALL_C(b_+50, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+53);
          CYC(b_+53, b_+56); TAIL(objectSetVisible81);
        }
        else if (jt_ == b_+56) {
          CYC(b_+56, b_+58); E = 0x50;
          CYC(b_+58, b_+60); A = 0x50;
          CYC(b_+60, b_+61); mem_wr(gb, DE, A);
          CYC(b_+61, b_+63); A = 0xa4;
          CYC(b_+63, b_+66); TAIL(playSound_b00);
        }
        else { HANDOFF(HL); }
      } while (0);
      goto jt_fall2;
    }
    else if (jt_ == b_+66) {
      jt_fall2:;
      CYC(b_+66, b_+68); E = 0x42;
      CYC(b_+68, b_+69); A = mem_rd(gb, DE);
      CYC(b_+69, b_+70); push_effect(gb, b_+70);
      do { uint16_t jt_ = (ghost_veran_jump_table(gb));
        if (jt_ == SYM(runVeranGhostSubid0) && hook_is(gb, SYM(runVeranGhostSubid0), runVeranGhostSubid0_hook)) { runVeranGhostSubid0_hook(gb); return; }
        else if (jt_ == SYM(runVeranGhostSubid1) && hook_is(gb, SYM(runVeranGhostSubid1), runVeranGhostSubid1_hook)) { runVeranGhostSubid1_hook(gb); return; }
        else if (jt_ == SYM(runVeranGhostSubid2) && hook_is(gb, SYM(runVeranGhostSubid2), runVeranGhostSubid2_hook)) { runVeranGhostSubid2_hook(gb); return; }
        else { HANDOFF(HL); }
      } while (0);
      goto jt_fall3;
    }
    else {
      jt_fall3:;
HANDOFF(HL);
    }
  } while (0);
}

void runVeranGhostSubid0_hook(GB *gb) {
  BASE(runVeranGhostSubid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x79;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+4, interactionAnimate_hook, SYM(interactionAnimate), b_+7); else CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); E = 0x45;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (ghost_veran_jump_table(gb));
    if (jt_ == b_+29) {
      CALL_C(b_+29, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+32);
      if (!(F & FZ)) { CYCT(b_+32, b_+34); goto substate0_visible; }
      CYC(b_+32, b_+34);
      CYC(b_+34, b_+36); mem_wr(gb, HL, 0x5a);
      CYC(b_+36, b_+38); L = 0x49;
      CYC(b_+38, b_+40); mem_wr(gb, HL, 0);
      CYC(b_+40, b_+42); L = 0x50;
      CYC(b_+42, b_+44); mem_wr(gb, HL, 0x0a);
      CALL_C(b_+44, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+47);
      CYC(b_+47, b_+49); A = 0x1d;
      CALL_C(b_+49, playSound_b00_hook, SYM(playSound_b00), b_+52);
      CYC(b_+52, b_+55); TAIL(objectSetVisible80);
substate0_visible:
      CYC(b_+55, b_+58); A = mem_rd(gb, wFrameCounter);
      CYC(b_+58, b_+59); alu_rrca(gb);
      if (!(F & FC)) { CYCT(b_+59, b_+62); objectSetVisible83_hook(gb); return; }
      CYC(b_+59, b_+62);
      CYC(b_+62, b_+65); TAIL(objectSetVisible80);
    }
    else if (jt_ == b_+65) {
      CALL_C(b_+65, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+68);
      if (!(F & FZ)) { CYCT(b_+68, b_+71); objectApplySpeed_hook(gb); return; }
      CYC(b_+68, b_+71);
      CALL_C(b_+71, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+74);
      CYC(b_+74, b_+77); SET_HL((SYM(forestFairy_subid00State2) + 5));
      CYC(b_+77, b_+80); TAIL(interactionSetScript);
    }
    else if (jt_ == b_+80) {
      CYC(b_+80, b_+83); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
      CYC(b_+83, b_+84); alu_or(gb, A);
      if (F & FZ) { CYCT(b_+84, b_+86); goto script_position; }
      CYC(b_+84, b_+86);
      CYC(b_+86, b_+89); SET_BC(0xbb00);
      CALL_C(b_+89, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+92);
      if (!(F & FZ)) { CYCT(b_+92, b_+93); ret_effect(gb); return; }
      CYC(b_+92, b_+93);
      CYC(b_+93, b_+95); L = 0x56;
      CYC(b_+95, b_+97); A = 0x40;
      CYC(b_+97, b_+98); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+98, b_+99); mem_wr(gb, HL, D);
      CALL_C(b_+99, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+102);
      CYC(b_+102, b_+104); L = 0x78;
      CYC(b_+104, b_+106); mem_wr(gb, HL, 0x78);
      CYC(b_+106, b_+108); L = 0x79;
      CYC(b_+108, b_+109); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+109, b_+110); alu_xor(gb, A);
      CALL_C(b_+110, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+113);
      CYC(b_+113, b_+115); A = 0x8d;
      CYC(b_+115, b_+118); TAIL(playSound_b00);
script_position:
      ghost_veran_update_script_position(gb, sp0_, b_+118, b_+121);
      CYC(b_+131, b_+134); TAIL(interactionRunScript);
    }
    else if (jt_ == b_+134) {
      CYC(b_+134, b_+135); H = D;
      CYC(b_+135, b_+137); L = 0x78;
      CYC(b_+137, b_+138); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
      CYC(b_+138, b_+140); B = 1;
      if (!(F & FZ)) { CYCT(b_+140, b_+143); objectFlickerVisibility_hook(gb); return; }
      CYC(b_+140, b_+143);
      CYC(b_+143, b_+145); L = 0x79;
      CYC(b_+145, b_+146); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
      CALL_C(b_+146, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+149);
      CYC(b_+149, b_+150); A = mem_rd(gb, HL);
      CYC(b_+150, b_+152); alu_cp(gb, 4);
      if (!(F & FZ)) { CYCT(b_+152, b_+155); objectSetVisible_hook(gb); return; }
      CYC(b_+152, b_+155);
      CALL_C(b_+155, objectSetInvisible_hook, SYM(objectSetInvisible), b_+158);
      CYC(b_+158, b_+160); A = 0xab;
      CYC(b_+160, b_+163); TAIL(playSound_b00);
    }
    else if (jt_ == b_+163) {
      CYC(b_+163, b_+166); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
      CYC(b_+166, b_+168); alu_cp(gb, 2);
      if (!(F & FZ)) { CYCT(b_+168, b_+169); ret_effect(gb); return; }
      CYC(b_+168, b_+169);
      CYC(b_+169, b_+171);
      CALL_C(b_+99, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+102);
      CYC(b_+102, b_+104); L = 0x78;
      CYC(b_+104, b_+106); mem_wr(gb, HL, 0x78);
      CYC(b_+106, b_+108); L = 0x79;
      CYC(b_+108, b_+109); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+109, b_+110); alu_xor(gb, A);
      CALL_C(b_+110, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+113);
      CYC(b_+113, b_+115); A = 0x8d;
      CYC(b_+115, b_+118); TAIL(playSound_b00);
    }
    else if (jt_ == b_+171) {
      CYC(b_+171, b_+174); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
      CYC(b_+174, b_+176); alu_cp(gb, 0x12);
      if (!(F & FZ)) { CYCT(b_+176, b_+178); goto script_position_2; }
      CYC(b_+176, b_+178);
      CYC(b_+178, b_+181); SET_BC(0x0302);
      CYC(b_+181, b_+184); push_effect(gb, b_+184);
      ghost_veran_rumble_and_randomize_x(gb, sp0_);
      CYC(b_+184, b_+186); goto run_script;
script_position_2:
      ghost_veran_update_script_position(gb, sp0_, b_+186, b_+189);
run_script:
      CALL_C(b_+199, interactionRunScript_hook, SYM(interactionRunScript), b_+202);
      if (!(F & FC)) { CYCT(b_+202, b_+203); ret_effect(gb); return; }
      CYC(b_+202, b_+203);
      CALL_C(b_+203, objectSetInvisible_hook, SYM(objectSetInvisible), b_+206);
      CYC(b_+206, b_+209); TAIL(interactionIncSubstate);
    }
    else if (jt_ == b_+232) {
      CYC(b_+232, b_+235); A = mem_rd(gb, wTmpcfc0_armosStatue_killedArmosPositions);
      CYC(b_+235, b_+237); alu_cp(gb, 0x17);
      if (!(F & FZ)) { CYCT(b_+237, b_+238); ret_effect(gb); return; }
      CYC(b_+237, b_+238);
      CALL_C(b_+238, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+241);
      CYC(b_+241, b_+244); SET_HL((SYM(forestFairy_subid00State2) + 9));
      CALL_C(b_+244, interactionSetScript_hook, SYM(interactionSetScript), b_+247);
      CALL_C(b_+247, objectSetVisible80_hook, SYM(objectSetVisible80), b_+250);
      goto jt_fall1;
    }
    else if (jt_ == b_+250) {
      jt_fall1:;
      CALL_C(b_+250, interactionRunScript_hook, SYM(interactionRunScript), b_+253);
      if (!(F & FC)) { CYCT(b_+253, b_+254); ret_effect(gb); return; }
      CYC(b_+253, b_+254);
      CYC(b_+254, b_+257); TAIL(interactionDelete);
    }
    else { HANDOFF(HL); }
  } while (0);
}

void runVeranGhostSubid1_hook(GB *gb) {
  BASE(runVeranGhostSubid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYC(b_+4, b_+6);
    CALL_C(b_+6, interactionRunScript_hook, SYM(interactionRunScript), b_+9);
    if (F & FC) { CYCT(b_+9, b_+12); interactionDelete_hook(gb); return; }
    CYC(b_+9, b_+12);
  } else CYCT(b_+4, b_+6);
  CYC(b_+12, b_+15); TAIL(interactionAnimate);
}

void runVeranGhostSubid2_hook(GB *gb) {
  BASE(runVeranGhostSubid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ghost_veran_jump_table(gb));
    if (jt_ == b_+12) {
      CYC(b_+12, b_+15); SET_BC(SYM(interactionCode48__initSubid0d));
      CYC(b_+15, b_+17); E = 0x4b;
      CYC(b_+17, b_+18); A = mem_rd(gb, DE);
      CYC(b_+18, b_+20); H8(hFF8F) = A;
      CYC(b_+20, b_+22); E = 0x4d;
      CYC(b_+22, b_+23); A = mem_rd(gb, DE);
      CYC(b_+23, b_+25); H8(hFF8E) = A;
      CYC(b_+25, b_+26); alu_sub(gb, C);
      CYC(b_+26, b_+27); A = alu_inc8(gb, A);
      CYC(b_+27, b_+29); alu_cp(gb, 3);
      if (!(F & FC)) { CYCT(b_+29, b_+31); goto move; }
      CYC(b_+29, b_+31);
      CYC(b_+31, b_+33); A = H8(hFF8F);
      CYC(b_+33, b_+34); alu_sub(gb, B);
      CYC(b_+34, b_+35); A = alu_inc8(gb, A);
      CYC(b_+35, b_+37); alu_cp(gb, 3);
      if (!(F & FC)) { CYCT(b_+37, b_+39); goto move; }
      CYC(b_+37, b_+39);
      CALL_C(b_+39, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+42);
      CYC(b_+42, b_+44); L = 0x4b;
      CYC(b_+44, b_+45); mem_wr(gb, HL, B);
      CYC(b_+45, b_+47); L = 0x4d;
      CYC(b_+47, b_+48); mem_wr(gb, HL, C);
      CYC(b_+48, b_+50); L = 0x46;
      CYC(b_+50, b_+52); mem_wr(gb, HL, 0x3c);
      CYC(b_+52, b_+54); goto animate;
move:
      CALL_C(b_+54, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+57);
      CYC(b_+57, b_+59); E = 0x49;
      CYC(b_+59, b_+60); mem_wr(gb, DE, A);
      CALL_C(b_+60, objectApplySpeed_hook, SYM(objectApplySpeed), b_+63);
animate:
      CYC(b_+63, b_+66); TAIL(interactionAnimate);
    }
    else if (jt_ == b_+66) {
      CALL_C(b_+66, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+69);
      if (!(F & FZ)) { CYCT(b_+69, b_+71); CYC(b_+63, b_+66); interactionAnimate_hook(gb); return; }
      CYC(b_+69, b_+71);
      CYC(b_+71, b_+72); L = E;
      CYC(b_+72, b_+73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+73, b_+76); SET_BC((SYM(checkNpcShouldExistAtGameStage_body__data5__subid1) + 1));
      CYC(b_+76, b_+79); TAIL(showText);
    }
    else if (jt_ == b_+79) {
      CALL_C(b_+79, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+82);
      if (!(F & FZ)) { CYCT(b_+82, b_+83); ret_effect(gb); return; }
      CYC(b_+82, b_+83);
      CYC(b_+83, b_+85); mem_wr(gb, HL, 6);
      CALL_C(b_+85, objectCopyPosition_hook, SYM(objectCopyPosition), b_+88);
      CYC(b_+88, b_+90); E = 0x58;
      CYC(b_+90, b_+92); A = 0x80;
      CYC(b_+92, b_+93); mem_wr(gb, DE, A);
      CYC(b_+93, b_+94); E = alu_inc8(gb, E);
      CYC(b_+94, b_+95); A = H;
      CYC(b_+95, b_+96); mem_wr(gb, DE, A);
      CALL_C(b_+96, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+99);
      CYC(b_+99, b_+101); L = 0x46;
      CYC(b_+101, b_+103); mem_wr(gb, HL, 0x3d);
      CYC(b_+103, b_+104); ret_effect(gb); return;
    }
    else if (jt_ == b_+104) {
      CALL_C(b_+104, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+107);
      if (F & FZ) { CYCT(b_+107, b_+109); goto delete; }
      CYC(b_+107, b_+109);
      CYC(b_+109, b_+111); A = 0x1a;
      CALL_C(b_+111, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+114);
      CYC(b_+114, b_+116); alu_bit(gb, 7, mem_rd(gb, HL));
      if (F & FZ) { CYCT(b_+116, b_+119); objectSetVisible82_hook(gb); return; }
      CYC(b_+116, b_+119);
      CYC(b_+119, b_+122); TAIL(objectSetInvisible);
delete:
      CYC(b_+122, b_+124); A = 1;
      CYC(b_+124, b_+127); W8(wLoadedTreeGfxIndex) = A;
      CYC(b_+127, b_+130); TAIL(interactionDelete);
    }
    else { HANDOFF(HL); }
  } while (0);
}
