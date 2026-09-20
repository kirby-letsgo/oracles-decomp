#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x05, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x05, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x61ce, 0x61d0); E = 0x04;
  CYC(0x61d0, 0x61d1); A = mem_rd(gb, DE);
  CYC(0x61d1, 0x61d2); push_effect(gb, 0x61d2);
  switch (transformed_link_jump_table(gb)) {
    case 0x61d6: goto state0;
    case 0x6232: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x61d6, dropLinkHeldItem_hook, 0x2c43, 0x61d9);
  CALL_C(0x61d9, clearAllParentItems_hook, 0x2c10, 0x61dc);
  CYC(0x61dc, 0x61df); A = W8(wLinkForceState);
  CYC(0x61df, 0x61e0); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x61e0, 0x61e2);
    goto reset_id_to_normal;
  }
  CYC(0x61e0, 0x61e2);
  CALL_C(0x61e2, specialObjectSetOamVariables_hook, 0x41f7, 0x61e5);
  CYC(0x61e5, 0x61e6); alu_xor(gb, A);
  CALL_C(0x61e6, specialObjectSetAnimation_hook, 0x2b0a, 0x61e9);
  CALL_C(0x61e9, objectSetVisiblec1_hook, 0x1e3c, 0x61ec);
  CALL_C(0x61ec, itemIncState_hook, 0x23ea, 0x61ef);
  CYC(0x61ef, 0x61f1); L = 0x24;
  CYC(0x61f1, 0x61f3); A = 0x80;
  CYC(0x61f3, 0x61f4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x61f4, 0x61f5); L = alu_inc8(gb, L);
  CYC(0x61f5, 0x61f7); A = 0x06;
  CYC(0x61f7, 0x61f8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x61f8, 0x61f9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x61f9, 0x61fb); L = 0x01;
  CYC(0x61fb, 0x61fc); A = mem_rd(gb, HL);
  CYC(0x61fc, 0x61fe); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x61fe, 0x61ff); ret_effect(gb); return;
  }
  CYC(0x61fe, 0x61ff);
  CYC(0x61ff, 0x6201); L = 0x06;
  CYC(0x6201, 0x6203); mem_wr(gb, HL, 0xe0);
  CYC(0x6203, 0x6204); L = alu_inc8(gb, L);
  CYC(0x6204, 0x6206); mem_wr(gb, HL, 0x01);
  CYC(0x6206, 0x6208); A = 0x9a;
  CALL_C(0x6208, playSound_b00_hook, 0x0c98, 0x620b);
  CYC(0x620b, 0x620d);
  goto create_green_poof;

disable_transformation_for_baby:
  CYC(0x620d, 0x620f); A = 0x83;
  CALL_C(0x620f, playSound_b00_hook, 0x0c98, 0x6212);

disable_transformation:
  CYC(0x6212, 0x6213); alu_xor(gb, A);
  CALL_C(0x6213, setLinkIDOverride_hook, 0x2acf, 0x6216);
  CYC(0x6216, 0x6218); A = 0x01;
  CYC(0x6218, 0x621b); W8(wDisableRingTransformations) = A;
  CYC(0x621b, 0x621d); E = 0x01;
  CYC(0x621d, 0x621e); A = mem_rd(gb, DE);
  CYC(0x621e, 0x6220); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x6220, 0x6221); ret_effect(gb); return;
  }
  CYC(0x6220, 0x6221);

create_green_poof:
  CYC(0x6221, 0x6223); B = 0x02;
  CYC(0x6223, 0x6226); objectCreateInteractionWithSubid00_hook(gb); return;

reset_id_to_normal:
  CYC(0x6226, 0x6227); alu_xor(gb, A);
  CALL_C(0x6227, setLinkID_hook, 0x2ad9, 0x622a);
  CYC(0x622a, 0x622c); A = 0x01;
  CYC(0x622c, 0x622f); W8(wDisableRingTransformations) = A;
  CYC(0x622f, 0x6232); specialObjectCode_link_hook(gb); return;

state1:
  CYC(0x6232, 0x6235); A = W8(wLinkForceState);
  CYC(0x6235, 0x6236); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6236, 0x6238);
    goto reset_id_to_normal;
  }
  CYC(0x6236, 0x6238);
  CYC(0x6238, 0x623b); A = W8(wPaletteThread_mode);
  CYC(0x623b, 0x623c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x623c, 0x623d); ret_effect(gb); return;
  }
  CYC(0x623c, 0x623d);
  CYC(0x623d, 0x6240); A = W8(wScrollMode);
  CYC(0x6240, 0x6242); alu_and(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x6242, 0x6243); ret_effect(gb); return;
  }
  CYC(0x6242, 0x6243);
  CALL_C(0x6243, updateLinkDamageTaken_hook, 0x4268, 0x6246);
  CYC(0x6246, 0x6249); A = W8(wLinkDeathTrigger);
  CYC(0x6249, 0x624a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x624a, 0x624c);
    goto disable_transformation;
  }
  CYC(0x624a, 0x624c);
  CALL_C(0x624c, retIfTextIsActive_hook, 0x1859, 0x624f);
  CYC(0x624f, 0x6252); A = W8(wDisabledObjects);
  CYC(0x6252, 0x6254); alu_and(gb, 0x81);
  if (!(F & FZ)) {
    CYCT(0x6254, 0x6255); ret_effect(gb); return;
  }
  CYC(0x6254, 0x6255);
  CALL_C(0x6255, decPegasusSeedCounter_hook, 0x2bbd, 0x6258);
  CYC(0x6258, 0x6259); H = D;
  CYC(0x6259, 0x625b); L = 0x01;
  CYC(0x625b, 0x625c); A = mem_rd(gb, HL);
  CYC(0x625c, 0x625e); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x625e, 0x6260);
    goto apply_tile_types;
  }
  CYC(0x625e, 0x6260);
  CYC(0x6260, 0x6262); L = 0x06;
  CALL_C(0x6262, decHlRef16WithCap_hook, 0x0237, 0x6265);
  if (F & FZ) {
    CYCT(0x6265, 0x6267);
    goto disable_transformation_for_baby;
  }
  CYC(0x6265, 0x6267);
  CYC(0x6267, 0x6269);
  goto update_movement;

apply_tile_types:
  CALL_C(0x6269, linkApplyTileTypes_hook, 0x42b7, 0x626c);
  CYC(0x626c, 0x626f); A = W8(wLinkSwimmingState);
  CYC(0x626f, 0x6270); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x6270, 0x6272);
    goto reset_id_to_normal;
  }
  CYC(0x6270, 0x6272);
  CYC(0x6272, 0x6275); SET_HL(0x462d);
  CYC(0x6275, 0x6277); E = 0x06;
  CALL_C(0x6277, interBankCall_hook, 0x008a, 0x627a);
  CYC(0x627a, 0x627c); E = 0x01;
  CYC(0x627c, 0x627d); A = mem_rd(gb, DE);
  CYC(0x627d, 0x627e); alu_cp(gb, B);
  CYC(0x627e, 0x627f); A = B;
  if (!(F & FZ)) {
    CYCT(0x627f, 0x6281);
    goto reset_id_to_normal;
  }
  CYC(0x627f, 0x6281);

update_movement:
  CALL_C(0x6281, specialObjectUpdateAdjacentWallsBitset_hook, 0x5e62, 0x6284);
  CALL_C(0x6284, linkUpdateKnockback_hook, 0x5d5b, 0x6287);
  CALL_C(0x6287, updateLinkSpeed_standard_hook, 0x5ce6, 0x628a);
  CYC(0x628a, 0x628b); H = D;
  CYC(0x628b, 0x628d); L = 0x01;
  CYC(0x628d, 0x628e); A = mem_rd(gb, HL);
  CYC(0x628e, 0x6290); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x6290, 0x6292);
    goto check_knockback;
  }
  CYC(0x6290, 0x6292);
  CYC(0x6292, 0x6294); L = 0x10;
  CYC(0x6294, 0x6296); mem_wr(gb, HL, alu_srl(gb, mem_rd(gb, HL)));

check_knockback:
  CYC(0x6296, 0x6298); L = 0x2d;
  CYC(0x6298, 0x6299); A = mem_rd(gb, HL);
  CYC(0x6299, 0x629a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x629a, 0x629c);
    goto animate_if_pegasus_seeds_active;
  }
  CYC(0x629a, 0x629c);
  CYC(0x629c, 0x629e); L = 0x24;
  CYC(0x629e, 0x62a0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
  CYC(0x62a0, 0x62a2); L = 0x0f;
  CYC(0x62a2, 0x62a4); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x62a4, 0x62a6);
    goto update_angle;
  }
  CYC(0x62a4, 0x62a6);
  CYC(0x62a6, 0x62a8); C = 0x20;
  CALL_C(0x62a8, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x62ab);
  if (!(F & FZ)) {
    CYCT(0x62ab, 0x62ad);
    goto update_angle;
  }
  CYC(0x62ab, 0x62ad);
  CYC(0x62ad, 0x62ae); alu_xor(gb, A);
  CYC(0x62ae, 0x62b1); W8(wLinkInAir) = A;

update_angle:
  CYC(0x62b1, 0x62b4); A = W8(wcc95);
  CYC(0x62b4, 0x62b5); B = A;
  CYC(0x62b5, 0x62b7); L = 0x09;
  CYC(0x62b7, 0x62ba); A = W8(wLinkAngle);
  CYC(0x62ba, 0x62bb); mem_wr(gb, HL, A);
  CYC(0x62bb, 0x62bc); alu_or(gb, B);
  CYC(0x62bc, 0x62bd); alu_rlca(gb);
  if (F & FC) {
    CYCT(0x62bd, 0x62bf);
    goto animate_if_pegasus_seeds_active;
  }
  CYC(0x62bd, 0x62bf);
  CYC(0x62bf, 0x62c1); L = 0x01;
  CYC(0x62c1, 0x62c2); A = mem_rd(gb, HL);
  CYC(0x62c2, 0x62c4); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x62c4, 0x62c6);
    goto update_direction;
  }
  CYC(0x62c4, 0x62c6);
  CYC(0x62c6, 0x62c8); L = 0x21;
  CYC(0x62c8, 0x62ca); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x62ca, 0x62cc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
  CYC(0x62cc, 0x62ce); A = 0x87;
  if (!(F & FZ)) {
    CALL_C_CC(0x62ce, playSound_b00_hook, 0x0c98, 0x62d1);
  } else {
    CYC(0x62ce, 0x62d1);
  }

update_direction:
  CYC(0x62d1, 0x62d4); A = W8(wLinkTurningDisabled);
  CYC(0x62d4, 0x62d5); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x62d5, updateLinkDirectionFromAngle_hook, 0x2b64, 0x62d8);
  } else {
    CYC(0x62d5, 0x62d8);
  }
  CYC(0x62d8, 0x62db); A = W8(wActiveTileType);
  CYC(0x62db, 0x62dd); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(0x62dd, 0x62df);
    goto animate;
  }
  CYC(0x62dd, 0x62df);
  CYC(0x62df, 0x62e2); A = W8(wLinkImmobilized);
  CYC(0x62e2, 0x62e3); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x62e3, 0x62e5);
    goto animate;
  }
  CYC(0x62e3, 0x62e5);
  CALL_C(0x62e5, specialObjectUpdatePosition_hook, 0x5d97, 0x62e8);

animate:
  CALL_C(0x62e8, checkPegasusSeedCounter_hook, 0x2be8, 0x62eb);
  if (F & FZ) {
    CYCT(0x62eb, 0x62ed);
    goto set_animation;
  }
  CYC(0x62eb, 0x62ed);
  CYC(0x62ed, 0x62ee); alu_rlca(gb);
  if (!(F & FC)) {
    CYCT(0x62ee, 0x62f0);
    goto set_animation;
  }
  CYC(0x62ee, 0x62f0);
  CALL_C(0x62f0, getFreeInteractionSlot_hook, 0x3aef, 0x62f3);
  if (!(F & FZ)) {
    CYCT(0x62f3, 0x62f5);
    goto set_animation;
  }
  CYC(0x62f3, 0x62f5);
  CYC(0x62f5, 0x62f7); mem_wr(gb, HL, 0x0f);
  CYC(0x62f7, 0x62f8); L = alu_inc8(gb, L);
  CYC(0x62f8, 0x62f9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x62f9, 0x62fc); SET_BC(0x0500);
  CALL_C(0x62fc, objectCopyPositionWithOffset_hook, 0x225a, 0x62ff);

set_animation:
  CYC(0x62ff, 0x6301); E = 0x30;
  CYC(0x6301, 0x6302); A = mem_rd(gb, DE);
  CYC(0x6302, 0x6303); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x6303, 0x6306);
    specialObjectAnimate_hook(gb); return;
  }
  CYC(0x6303, 0x6306);
  CYC(0x6306, 0x6307); alu_xor(gb, A);
  CYC(0x6307, 0x630a); specialObjectSetAnimation_hook(gb); return;

animate_if_pegasus_seeds_active:
  CALL_C(0x630a, checkPegasusSeedCounter_hook, 0x2be8, 0x630d);
  if (!(F & FZ)) {
    CYCT(0x630d, 0x630f);
    goto animate;
  }
  CYC(0x630d, 0x630f);
  CYC(0x630f, 0x6310); alu_xor(gb, A);
  CYC(0x6310, 0x6313); specialObjectSetAnimation_hook(gb);
}
