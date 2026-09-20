#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t func_410d_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

static void add_double_index_to_hl_b05(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false);
  pop_effect(gb);
}

void add_a_to_hl_b05_hook(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    pop_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  pop_effect(gb);
}

void updateGameKeysPressed_hook(GB *gb) {
  BASE(updateGameKeysPressed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wKeysPressed);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, wMenuUnionEnd);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); goto update; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+12, b_+14); goto reverse; }
  CYC(b_+12, b_+14);
  CALL_C(b_+14, getSimulatedInput_hook, SYM(getSimulatedInput), b_+17);
  CYC(b_+17, b_+19); goto update_a;
reverse:
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+23); mem_wr(gb, wMenuUnionEnd, A);
  CYC(b_+23, b_+25); A = 0xa0;
  CYC(b_+25, b_+26); alu_and(gb, C);
  CYC(b_+26, b_+27); alu_rrca(gb);
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); A = 0x50;
  CYC(b_+30, b_+31); alu_and(gb, C);
  CYC(b_+31, b_+32); alu_rlca(gb);
  CYC(b_+32, b_+33); alu_or(gb, B);
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+36); A = 0x0f;
  CYC(b_+36, b_+37); alu_and(gb, C);
  CYC(b_+37, b_+38); alu_or(gb, B);
update_a:
  CYC(b_+38, b_+39); C = A;
update:
  CYC(b_+39, b_+42); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+42, b_+43); alu_or(gb, A);
  CYC(b_+43, b_+46); SET_HL(wGameKeysPressed);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto dying; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+50); alu_cpl(gb);
  CYC(b_+50, b_+51); B = A;
  CYC(b_+51, b_+52); A = C;
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+53, b_+54); alu_and(gb, B);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+55, b_+56); A = C;
  CYC(b_+56, b_+58); alu_and(gb, 0xf0);
  CYC(b_+58, b_+60); A = alu_swap(gb, A);
  CYC(b_+60, b_+63); SET_HL(b_+75);
  CYC(b_+63, b_+64); add_a_to_hl_b05_hook(gb, b_+64);
  CYC(b_+64, b_+65); A = mem_rd(gb, HL);
  CYC(b_+65, b_+68); mem_wr(gb, wLinkAngle, A);
  CYC(b_+68, b_+69);
  ret_effect(gb);
  return;
dying:
  CYC(b_+69, b_+70); alu_xor(gb, A);
  CYC(b_+70, b_+71); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+72, b_+73); A = alu_dec8(gb, A);
  CYC(b_+73, b_+74); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+74, b_+75);
  ret_effect(gb);
}

// updateSpecialObjects@updateSpecialObject: hl is w1Link or w1Companion; the caller has pushed
// the return address the dispatched special-object handler pops.
static void update_special_object(GB *gb) {
  BASE(updateSpecialObjects__updateSpecialObject);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  alu_or(gb, A);
  CYC(b_+1, b_+2);
  if (F & FZ) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
  CYC(b_+2, b_+3);
  A = L;
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); H8(hActiveObjectType) = A;
  A = H;
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); H8(hActiveObject) = A;
  D = H;
  L = OBJ_ID;
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (func_410d_jump_table(gb));
    if (jt_ == SYM(specialObjectCode_link)) { specialObjectCode_link_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_transformedLink)) { specialObjectCode_transformedLink_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_linkInCutscene_b00)) { specialObjectCode_linkInCutscene_b00_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_linkRidingAnimal)) { specialObjectCode_linkRidingAnimal_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_minecart_b05)) { specialObjectCode_minecart_b05_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_ricky)) { specialObjectCode_ricky_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_dimitri)) { specialObjectCode_dimitri_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_moosh)) { specialObjectCode_moosh_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_maple)) { specialObjectCode_maple_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_companionCutscene_b00)) { specialObjectCode_companionCutscene_b00_hook(gb); return; }
    else if (jt_ == SYM(specialObjectCode_raft_b05)) { specialObjectCode_raft_b05_hook(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

void updateSpecialObjects_hook(GB *gb) {
  BASE(updateSpecialObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wLinkIDOverride);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto no_transformation; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x7f);
  CYC(b_+11, b_+14); mem_wr(gb, w1Link_id, A);
no_transformation:
  CYC(b_+14, b_+17); SET_HL(w1Link_var2f);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); alu_and(gb, 0x3f);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CYC(b_+21, b_+23); A = 0x4a;
  CALL_C(b_+23, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+26);
  if (!(F & FC)) { CYCT(b_+26, b_+28); goto no_seed_shooter; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
no_seed_shooter:
  CYC(b_+30, b_+33); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+33, b_+35); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+35, b_+37); goto no_input_block; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x80));
no_input_block:
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+43); mem_wr(gb, wBraceletGrabbingNothing, A);
  CYC(b_+43, b_+46); mem_wr(gb, wcc92, A);
  CYC(b_+46, b_+49); mem_wr(gb, wForceLinkPushAnimation, A);
  CYC(b_+49, b_+52); SET_HL(wcc95);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_or(gb, 0x7f);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  CYC(b_+56, b_+59); SET_HL(wLinkTurningDisabled);
  CYC(b_+59, b_+61); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & 0x7f));
  CALL_C(b_+61, updateGameKeysPressed_hook, SYM(updateGameKeysPressed), b_+64);
  CYC(b_+64, b_+67); SET_HL(w1Companion);
  CYC(b_+67, b_+70); push_effect(gb, b_+70);
  update_special_object(gb);
  CYC(b_+70, b_+71); alu_xor(gb, A);
  CYC(b_+71, b_+74); mem_wr(gb, wLinkClimbingVine, A);
  CYC(b_+74, b_+77); mem_wr(gb, wDisallowMountingCompanion, A);
  CYC(b_+77, b_+80); SET_HL(w1Link);
  CYC(b_+80, b_+83); push_effect(gb, b_+83);
  update_special_object(gb);
  CALL_C(b_+83, updateLinkInvincibilityCounter_hook, SYM(updateLinkInvincibilityCounter), b_+86);
  CYC(b_+86, b_+89); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+89, b_+92); mem_wr(gb, wLinkRidingObject, A);
  CYC(b_+92, b_+95); SET_HL(wLinkImmobilized);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL);
  CYC(b_+96, b_+98); alu_and(gb, 0x0f);
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
  CYC(b_+99, b_+100); alu_xor(gb, A);
  CYC(b_+100, b_+103); mem_wr(gb, wcc67, A);
  CYC(b_+103, b_+106); mem_wr(gb, w1Link_var2a, A);
  CYC(b_+106, b_+109); mem_wr(gb, wccd8, A);
  CYC(b_+109, b_+112); SET_HL(wInstrumentsDisabledCounter);
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CYC(b_+113, b_+114); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+114, b_+116); goto clear; }
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+117); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
clear:
  CYC(b_+117, b_+120); SET_HL(wGrabbableObjectBuffer);
  CYC(b_+120, b_+122); B = 0x10;
  CYC(b_+122, b_+125);
  clearMemory_hook(gb);
}

void func_410d_hook(GB *gb) {
  BASE(func_410d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); H8(hActiveObjectType) = A;
  CYC(b_+3, b_+6); SET_DE(w1Companion_id);
  CYC(b_+6, b_+7); A = D;
  CYC(b_+7, b_+9); H8(hActiveObject) = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_sub(gb, 0x0a);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (func_410d_jump_table(gb));
    if (jt_ == b_+33) { goto invalid; }
    else if (jt_ == b_+34) { goto ricky; }
    else if (jt_ == b_+39) { goto dimitri; }
    else if (jt_ == b_+57) { goto moosh; }
    else if (jt_ == b_+155) { goto minecart; }
    else if (jt_ == b_+196) { goto raft; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
invalid:
  CYC(b_+33, b_+34); ret_effect(gb); return;
ricky:
  CYC(b_+34, b_+37); SET_BC(0);
  CYC(b_+37, b_+39); goto companion;
dimitri:
  CYC(b_+39, b_+41); E = 0x08;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); alu_rrca(gb);
  CYC(b_+43, b_+46); SET_BC(0xf600);
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto companion; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); C = 0xfb;
  CYC(b_+50, b_+51); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+51, b_+53); goto companion; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); C = 0x05;
  CYC(b_+55, b_+57); goto companion;
moosh:
  CYC(b_+57, b_+59); E = 0x08;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_rrca(gb);
  CYC(b_+61, b_+64); SET_BC(0xf200);
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto companion; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); B = 0xf0;
companion:
  CYC(b_+68, b_+71); SET_HL(w1Link_yh);
  CALL_C(b_+71, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+74);
  CYC(b_+74, b_+76); E = 0x08;
  CYC(b_+76, b_+78); L = 0x08;
  CYC(b_+78, b_+79); A = mem_rd(gb, DE);
  CYC(b_+79, b_+80); mem_wr(gb, HL, A);
  CYC(b_+80, b_+82); A = 1;
  CYC(b_+82, b_+85); mem_wr(gb, wDisableWarpTiles, A);
  CYC(b_+85, b_+87); L = 0x2a;
  CYC(b_+87, b_+88); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+88, b_+89); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+89, b_+91); L = 0x2d;
  CYC(b_+91, b_+92); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+92, b_+94); goto no_damage; }
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+96); L = 0x25;
  CYC(b_+96, b_+97); E = L;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+99); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+99, b_+101); goto no_damage; }
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+102); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+102, b_+104); L = 0x29;
  CYC(b_+104, b_+105); E = L;
  CYC(b_+105, b_+107); B = 0x06;
  CALL_C(b_+107, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+110);
  CYC(b_+110, b_+112); goto finish_companion;
no_damage:
  CYC(b_+112, b_+114); L = 0x25;
  CYC(b_+114, b_+115); E = L;
  CYC(b_+115, b_+116); A = mem_rd(gb, HL);
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CYC(b_+117, b_+119); D = 0xd0;
  CYC(b_+119, b_+121); H = 0xd1;
  CYC(b_+121, b_+123); L = 0x29;
  CYC(b_+123, b_+124); E = L;
  CYC(b_+124, b_+126); B = 0x06;
  CALL_C(b_+126, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+129);
finish_companion:
  CYC(b_+129, b_+131); H = 0xd0;
  CYC(b_+131, b_+133); D = 0xd1;
  CYC(b_+133, b_+135); L = 0x1c;
  CYC(b_+135, b_+136); A = mem_rd(gb, HL);
  CYC(b_+136, b_+138); L = 0x1b;
  CYC(b_+138, b_+139); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+139, b_+141); goto copy_flags; }
  CYC(b_+139, b_+141);
  CYC(b_+141, b_+143); E = 0x1b;
  CYC(b_+143, b_+144); A = mem_rd(gb, DE);
copy_flags:
  CYC(b_+144, b_+146); E = 0x1c;
  CYC(b_+146, b_+147); mem_wr(gb, DE, A);
  CYC(b_+147, b_+149); L = 0x1a;
  CYC(b_+149, b_+150); E = L;
  CYC(b_+150, b_+151); A = mem_rd(gb, DE);
  CYC(b_+151, b_+153); alu_and(gb, 0x83);
  CYC(b_+153, b_+154); mem_wr(gb, HL, A);
  CYC(b_+154, b_+155); ret_effect(gb); return;
minecart:
  CYC(b_+155, b_+156); H = D;
  CYC(b_+156, b_+158); L = 0x08;
  CYC(b_+158, b_+159); A = mem_rd(gb, HL);
  CYC(b_+159, b_+161); L = 0x21;
  CYC(b_+161, b_+162); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+162, b_+165); SET_HL(b_+180);
  CYC(b_+165, b_+166); add_double_index_to_hl_b05(gb, b_+166);
  CYC(b_+166, b_+167); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+167, b_+168); C = mem_rd(gb, HL);
  CYC(b_+168, b_+169); B = A;
  CYC(b_+169, b_+172); SET_HL(w1Link_yh);
  CALL_C(b_+172, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+175);
  CYC(b_+175, b_+177); L = 0x1a;
  CYC(b_+177, b_+179); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x40));
  CYC(b_+179, b_+180); ret_effect(gb); return;
raft:
  CYC(b_+196, b_+199); A = mem_rd(gb, wLinkForceState);
  CYC(b_+199, b_+201); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+201, b_+202); ret_effect(gb); return; }
  CYC(b_+201, b_+202);
  CYC(b_+202, b_+205); SET_HL(w1Link_state);
  CYC(b_+205, b_+206); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+206, b_+208); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+208, b_+210); goto raft_position; }
  CYC(b_+208, b_+210);
  CYC(b_+210, b_+211); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+211, b_+213); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+213, b_+214); ret_effect(gb); return; }
  CYC(b_+213, b_+214);
raft_position:
  CYC(b_+214, b_+216); L = 0x1a;
  CYC(b_+216, b_+218); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~0x40));
  CYC(b_+218, b_+221); SET_BC(0xfb00);
  CYC(b_+221, b_+223); E = 0x21;
  CYC(b_+223, b_+224); A = mem_rd(gb, DE);
  CYC(b_+224, b_+225); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+225, b_+227); goto copy_raft_position; }
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+228); B = alu_dec8(gb, B);
copy_raft_position:
  CALL_C(b_+228, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+231);
  CYC(b_+231, b_+234);
  objectSetVisiblec3_hook(gb);
}

void linkApplyDamage_b5_hook(GB *gb) {
  BASE(linkApplyDamage_b5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(companionDragToCenterOfHole__adjustX) + 9));
  CYC(b_+3, b_+5); E = 0x06;
  CALL_C(b_+5, interBankCall_hook, 0x008a, b_+8);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void specialObjectCode_minecart_b05_hook(GB *gb) {
  BASE(specialObjectCode_minecart_b05);
  CYC(b_+0, b_+3); SET_HL(SYM(updateHeartRingCounter));
  CYC(b_+3, b_+5); E = 0x06;
  CYC(b_+5, b_+8);
  interBankCall_hook(gb);
}

void specialObjectCode_raft_b05_hook(GB *gb) {
  BASE(specialObjectCode_raft_b05);
  CYC(b_+0, b_+3); SET_HL((SYM(linkUpdateFlippersSpeed__nextState) + 6));
  CYC(b_+3, b_+5); E = 0x06;
  CYC(b_+5, b_+8);
  interBankCall_hook(gb);
}
