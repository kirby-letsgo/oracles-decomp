#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t transformed_link_jump_table(GB *gb) {
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

void specialObjectCode_transformedLink_hook(GB *gb) {
  BASE(specialObjectCode_transformedLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (transformed_link_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+100) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+11);
  CALL_C(b_+11, clearAllParentItems_hook, SYM(clearAllParentItems), b_+14);
  CYC(b_+14, b_+17); A = W8(wLinkForceState);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+18, b_+20);
    goto reset_id_to_normal;
  }
  CYC(b_+18, b_+20);
  CALL_C(b_+20, specialObjectSetOamVariables_hook, SYM(specialObjectSetOamVariables), b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CALL_C(b_+24, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+27);
  CALL_C(b_+27, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+30);
  CALL_C(b_+30, itemIncState_hook, SYM(itemIncState), b_+33);
  CYC(b_+33, b_+35); L = 0x24;
  CYC(b_+35, b_+37); A = 0x80;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+41); A = 0x06;
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+43, b_+45); L = 0x01;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL);
  CYC(b_+46, b_+48); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+49); ret_effect(gb); return;
  }
  CYC(b_+48, b_+49);
  CYC(b_+49, b_+51); L = 0x06;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0xe0);
  CYC(b_+53, b_+54); L = alu_inc8(gb, L);
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x01);
  CYC(b_+56, b_+58); A = 0x9a;
  CALL_C(b_+58, playSound_b00_hook, SYM(playSound_b00), b_+61);
  CYC(b_+61, b_+63);
  goto create_green_poof;

disable_transformation_for_baby:
  CYC(b_+63, b_+65); A = 0x83;
  CALL_C(b_+65, playSound_b00_hook, SYM(playSound_b00), b_+68);

disable_transformation:
  CYC(b_+68, b_+69); alu_xor(gb, A);
  CALL_C(b_+69, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+72);
  CYC(b_+72, b_+74); A = 0x01;
  CYC(b_+74, b_+77); W8(wDisableRingTransformations) = A;
  CYC(b_+77, b_+79); E = 0x01;
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  CYC(b_+80, b_+82); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+82, b_+83); ret_effect(gb); return;
  }
  CYC(b_+82, b_+83);

create_green_poof:
  CYC(b_+83, b_+85); B = 0x02;
  CYC(b_+85, b_+88); objectCreateInteractionWithSubid00_hook(gb); return;

reset_id_to_normal:
  CYC(b_+88, b_+89); alu_xor(gb, A);
  CALL_C(b_+89, setLinkID_hook, SYM(setLinkID), b_+92);
  CYC(b_+92, b_+94); A = 0x01;
  CYC(b_+94, b_+97); W8(wDisableRingTransformations) = A;
  CYC(b_+97, b_+100); specialObjectCode_link_hook(gb); return;

state1:
  CYC(b_+100, b_+103); A = W8(wLinkForceState);
  CYC(b_+103, b_+104); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+104, b_+106);
    goto reset_id_to_normal;
  }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+109); A = W8(wPaletteThread_mode);
  CYC(b_+109, b_+110); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+110, b_+111); ret_effect(gb); return;
  }
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+114); A = W8(wScrollMode);
  CYC(b_+114, b_+116); alu_and(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(b_+116, b_+117); ret_effect(gb); return;
  }
  CYC(b_+116, b_+117);
  CALL_C(b_+117, updateLinkDamageTaken_hook, SYM(updateLinkDamageTaken), b_+120);
  CYC(b_+120, b_+123); A = W8(wLinkDeathTrigger);
  CYC(b_+123, b_+124); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+124, b_+126);
    goto disable_transformation;
  }
  CYC(b_+124, b_+126);
  CALL_C(b_+126, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+129);
  CYC(b_+129, b_+132); A = W8(wDisabledObjects);
  CYC(b_+132, b_+134); alu_and(gb, 0x81);
  if (!(F & FZ)) {
    CYCT(b_+134, b_+135); ret_effect(gb); return;
  }
  CYC(b_+134, b_+135);
  CALL_C(b_+135, decPegasusSeedCounter_hook, SYM(decPegasusSeedCounter), b_+138);
  CYC(b_+138, b_+139); H = D;
  CYC(b_+139, b_+141); L = 0x01;
  CYC(b_+141, b_+142); A = mem_rd(gb, HL);
  CYC(b_+142, b_+144); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+144, b_+146);
    goto apply_tile_types;
  }
  CYC(b_+144, b_+146);
  CYC(b_+146, b_+148); L = 0x06;
  CALL_C(b_+148, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+151);
  if (F & FZ) {
    CYCT(b_+151, b_+153);
    goto disable_transformation_for_baby;
  }
  CYC(b_+151, b_+153);
  CYC(b_+153, b_+155);
  goto update_movement;

apply_tile_types:
  CALL_C(b_+155, linkApplyTileTypes_hook, SYM(linkApplyTileTypes), b_+158);
  CYC(b_+158, b_+161); A = W8(wLinkSwimmingState);
  CYC(b_+161, b_+162); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+162, b_+164);
    goto reset_id_to_normal;
  }
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+167); SET_HL((SYM(companionDismountAndSavePosition__dimitri) + 9));
  CYC(b_+167, b_+169); E = 0x06;
  CALL_C(b_+169, interBankCall_hook, 0x008a, b_+172);
  CYC(b_+172, b_+174); E = 0x01;
  CYC(b_+174, b_+175); A = mem_rd(gb, DE);
  CYC(b_+175, b_+176); alu_cp(gb, B);
  CYC(b_+176, b_+177); A = B;
  if (!(F & FZ)) {
    CYCT(b_+177, b_+179);
    goto reset_id_to_normal;
  }
  CYC(b_+177, b_+179);

update_movement:
  CALL_C(b_+179, specialObjectUpdateAdjacentWallsBitset_hook, SYM(specialObjectUpdateAdjacentWallsBitset), b_+182);
  CALL_C(b_+182, linkUpdateKnockback_hook, SYM(linkUpdateKnockback), b_+185);
  CALL_C(b_+185, updateLinkSpeed_standard_hook, SYM(updateLinkSpeed_standard), b_+188);
  CYC(b_+188, b_+189); H = D;
  CYC(b_+189, b_+191); L = 0x01;
  CYC(b_+191, b_+192); A = mem_rd(gb, HL);
  CYC(b_+192, b_+194); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+194, b_+196);
    goto check_knockback;
  }
  CYC(b_+194, b_+196);
  CYC(b_+196, b_+198); L = 0x10;
  CYC(b_+198, b_+200); mem_wr(gb, HL, alu_srl(gb, mem_rd(gb, HL)));

check_knockback:
  CYC(b_+200, b_+202); L = 0x2d;
  CYC(b_+202, b_+203); A = mem_rd(gb, HL);
  CYC(b_+203, b_+204); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+204, b_+206);
    goto animate_if_pegasus_seeds_active;
  }
  CYC(b_+204, b_+206);
  CYC(b_+206, b_+208); L = 0x24;
  CYC(b_+208, b_+210); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(b_+210, b_+212); L = 0x0f;
  CYC(b_+212, b_+214); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+214, b_+216);
    goto update_angle;
  }
  CYC(b_+214, b_+216);
  CYC(b_+216, b_+218); C = 0x20;
  CALL_C(b_+218, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+221);
  if (!(F & FZ)) {
    CYCT(b_+221, b_+223);
    goto update_angle;
  }
  CYC(b_+221, b_+223);
  CYC(b_+223, b_+224); alu_xor(gb, A);
  CYC(b_+224, b_+227); W8(wLinkInAir) = A;

update_angle:
  CYC(b_+227, b_+230); A = W8(wcc95);
  CYC(b_+230, b_+231); B = A;
  CYC(b_+231, b_+233); L = 0x09;
  CYC(b_+233, b_+236); A = W8(wLinkAngle);
  CYC(b_+236, b_+237); mem_wr(gb, HL, A);
  CYC(b_+237, b_+238); alu_or(gb, B);
  CYC(b_+238, b_+239); alu_rlca(gb);
  if (F & FC) {
    CYCT(b_+239, b_+241);
    goto animate_if_pegasus_seeds_active;
  }
  CYC(b_+239, b_+241);
  CYC(b_+241, b_+243); L = 0x01;
  CYC(b_+243, b_+244); A = mem_rd(gb, HL);
  CYC(b_+244, b_+246); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+246, b_+248);
    goto update_direction;
  }
  CYC(b_+246, b_+248);
  CYC(b_+248, b_+250); L = 0x21;
  CYC(b_+250, b_+252); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+252, b_+254); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
  CYC(b_+254, b_+256); A = 0x87;
  if (!(F & FZ)) {
    CALL_C_CC(b_+256, playSound_b00_hook, SYM(playSound_b00), b_+259);
  } else {
    CYC(b_+256, b_+259);
  }

update_direction:
  CYC(b_+259, b_+262); A = W8(wLinkTurningDisabled);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+263, updateLinkDirectionFromAngle_hook, SYM(updateLinkDirectionFromAngle), b_+266);
  } else {
    CYC(b_+263, b_+266);
  }
  CYC(b_+266, b_+269); A = W8(wActiveTileType);
  CYC(b_+269, b_+271); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(b_+271, b_+273);
    goto animate;
  }
  CYC(b_+271, b_+273);
  CYC(b_+273, b_+276); A = W8(wLinkImmobilized);
  CYC(b_+276, b_+277); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+277, b_+279);
    goto animate;
  }
  CYC(b_+277, b_+279);
  CALL_C(b_+279, specialObjectUpdatePosition_hook, SYM(specialObjectUpdatePosition), b_+282);

animate:
  CALL_C(b_+282, checkPegasusSeedCounter_hook, SYM(checkPegasusSeedCounter), b_+285);
  if (F & FZ) {
    CYCT(b_+285, b_+287);
    goto set_animation;
  }
  CYC(b_+285, b_+287);
  CYC(b_+287, b_+288); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(b_+288, b_+290);
    goto set_animation;
  }
  CYC(b_+288, b_+290);
  CALL_C(b_+290, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+293);
  if (!(F & FZ)) {
    CYCT(b_+293, b_+295);
    goto set_animation;
  }
  CYC(b_+293, b_+295);
  CYC(b_+295, b_+297); mem_wr(gb, HL, 0x0f);
  CYC(b_+297, b_+298); L = alu_inc8(gb, L);
  CYC(b_+298, b_+299); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+299, b_+302); SET_BC(0x0500);
  CALL_C(b_+302, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+305);

set_animation:
  CYC(b_+305, b_+307); E = 0x30;
  CYC(b_+307, b_+308); A = mem_rd(gb, DE);
  CYC(b_+308, b_+309); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+309, b_+312);
    specialObjectAnimate_hook(gb); return;
  }
  CYC(b_+309, b_+312);
  CYC(b_+312, b_+313); alu_xor(gb, A);
  CYC(b_+313, b_+316); specialObjectSetAnimation_hook(gb); return;

animate_if_pegasus_seeds_active:
  CALL_C(b_+316, checkPegasusSeedCounter_hook, SYM(checkPegasusSeedCounter), b_+319);
  if (!(F & FZ)) {
    CYCT(b_+319, b_+321);
    goto animate;
  }
  CYC(b_+319, b_+321);
  CYC(b_+321, b_+322); alu_xor(gb, A);
  CYC(b_+322, b_+325); specialObjectSetAnimation_hook(gb);
}
