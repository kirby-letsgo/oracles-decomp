#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/dinImprisonedEvent.s
// (INTERAC_DIN_IMPRISONED_EVENT), bank $08: the objects of the Din-imprisoned cutscene. Subids:
// 0 Din, 1 Onox explaining, 2 the four vortex/lightning rings, 3 a ring spark, 4 a flying
// debris piece, 5 Onox taunting. The dispatcher interactionCode4f itself stays generated (it has
// an Ages namesake). wTmpcfc0+$10 is the cutscene's stage counter.

static uint16_t din_event_jump_table(GB *gb) {
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

static void din_event_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void din_event_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// state0@func_7784: animation var03+6, +11 more when var32 is set.
static void din_event_set_animation_for_var03(GB *gb) {
  BASE(interactionCode4f_state0);
  CYC(b_+92, b_+94); E = 0x43; // Interaction.var03
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CYC(b_+95, b_+97); alu_add(gb, 0x06);
  CYC(b_+97, b_+98); B = A;
  CYC(b_+98, b_+100); E = 0x72; // Interaction.var32
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+102); alu_or(gb, A);
  CYC(b_+102, b_+103); A = B;
  if (F & FZ) { CYCT(b_+103, b_+105); goto set; }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+107); alu_add(gb, 0x0b);
set:
  CYC(b_+107, b_+110);
  TAIL(interactionSetAnimation);
}

// state0@func_7796: with var32 clear, place the object on the circle around (var70,var71) at
// angle var03*4; otherwise copy the position stored at var70/71.
static void din_event_place(GB *gb) {
  BASE(interactionCode4f_state0);
  CYC(b_+110, b_+111); H = D;
  CYC(b_+111, b_+113); L = 0x70;
  CYC(b_+113, b_+115); E = 0x72; // Interaction.var32
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+117); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+117, b_+119); goto copy; }
  CYC(b_+117, b_+119);
  CYC(b_+119, b_+121); E = 0x43; // Interaction.var03
  CYC(b_+121, b_+122); A = mem_rd(gb, DE);
  CYC(b_+122, b_+123); alu_add(gb, A);
  CYC(b_+123, b_+124); alu_add(gb, A);
  CYC(b_+124, b_+126); E = 0x48;
  CYC(b_+126, b_+127); mem_wr(gb, DE, A);
  CYC(b_+127, b_+128); B = mem_rd(gb, HL);
  CYC(b_+128, b_+129); L = alu_inc8(gb, L);
  CYC(b_+129, b_+130); C = mem_rd(gb, HL);
  CYC(b_+130, b_+132); A = 0x38;
  CYC(b_+132, b_+134); E = 0x48;
  CYC(b_+134, b_+137);
  TAIL(objectSetPositionInCircleArc);
copy:
  CYC(b_+137, b_+139); E = 0x4b; // Interaction.yh
  CYC(b_+139, b_+140); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CYC(b_+141, b_+142); E = alu_inc8(gb, E);
  CYC(b_+142, b_+143); E = alu_inc8(gb, E);
  CYC(b_+143, b_+144); A = mem_rd(gb, HL);
  CYC(b_+144, b_+145); mem_wr(gb, DE, A);
  RET(b_+145); return;
}

// state0@setCounter2Between1To8
static void din_event_set_counter2(GB *gb) {
  BASE(interactionCode4f_state0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+146, getRandomNumber_hook, SYM(getRandomNumber), b_+149);
  CYC(b_+149, b_+151); alu_and(gb, 0x07);
  CYC(b_+151, b_+152); A = alu_inc8(gb, A);
  CYC(b_+152, b_+154); E = 0x47; // Interaction.counter2
  CYC(b_+154, b_+155); mem_wr(gb, DE, A);
  RET(b_+155); return;
}

void s_interactionCode4f_state0_hook(GB *gb) {
  BASE(interactionCode4f_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionIncState_hook, SYM(interactionIncState), b_+3);
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CYC(b_+6, b_+8); E = 0x42; // Interaction.subid
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (din_event_jump_table(gb));
    if (jt_ == b_+22) { goto subid0; }
    else if (jt_ == b_+31) { goto subid1; }
    else if (jt_ == SYM(objectSetVisible81) && hook_is(gb, SYM(objectSetVisible81), objectSetVisible81_hook)) { objectSetVisible81_hook(gb); return; }
    else if (jt_ == b_+40) { goto subid3; }
    else if (jt_ == b_+66) { goto subid4; }
    else if (jt_ == b_+83) { goto subid5; }
    else { HANDOFF(HL); }
  } while (0);

subid0:
  CYC(b_+22, b_+25); SET_HL(SYM(dinImprisonedScript_setDinCoords));
  CALL_C(b_+25, interactionSetScript_hook, SYM(interactionSetScript), b_+28);
  CYC(b_+28, b_+31);
  TAIL(objectSetVisiblec2);

subid1:
  CYC(b_+31, b_+34); SET_HL(SYM(dinImprisonedScript_OnoxExplainsMotive));
  CALL_C(b_+34, interactionSetScript_hook, SYM(interactionSetScript), b_+37);
  CYC(b_+37, b_+40);
  TAIL(objectSetVisible82);

subid3:
  CALL_L(b_+40, din_event_set_counter2, b_+43);
  CYC(b_+43, b_+45); E = 0x43; // Interaction.var03
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_add(gb, A);
  CYC(b_+47, b_+48); alu_add(gb, A);
  CYC(b_+48, b_+50); alu_add(gb, 0x10);
  CYC(b_+50, b_+52); alu_and(gb, 0x1f);
  CYC(b_+52, b_+54); E = 0x49; // Interaction.angle
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = 0x50; // Interaction.speed
  CYC(b_+57, b_+59); A = 0x78;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CALL_C(b_+60, objectSetVisible80_hook, SYM(objectSetVisible80), b_+63);
  CYC(b_+63, b_+66);
  TAIL(objectSetInvisible);

subid4:
  CYC(b_+66, b_+68); E = 0x43; // Interaction.var03
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+70, b_+72); goto subid4_zero; }
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); A = 0x05;
  CALL_C(b_+74, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+77);
  CYC(b_+77, b_+80);
  TAIL(objectSetVisible82);
subid4_zero:
  CYC(b_+80, b_+83);
  TAIL(objectSetVisible83);

subid5:
  CYC(b_+83, b_+86); SET_HL(SYM(dinImprisonedScript_OnoxSaysComeIfYouDare));
  CALL_C(b_+86, interactionSetScript_hook, SYM(interactionSetScript), b_+89);
  CYC(b_+89, b_+92);
  TAIL(objectSetVisible82);
}

// state1@func_791f: y/x from @table_7931, entry var03+a.
static void din_event_load_position(GB *gb) {
  BASE(interactionCode4f_state1);
  CYC(b_+347, b_+348); B = A;
  CYC(b_+348, b_+350); E = 0x43; // Interaction.var03
  CYC(b_+350, b_+351); A = mem_rd(gb, DE);
  CYC(b_+351, b_+352); alu_add(gb, B);
  CYC(b_+352, b_+355); SET_HL(b_+365); // @table_7931
  CYC(b_+355, b_+356); din_event_add_double_index(gb, b_+356);
  CYC(b_+356, b_+358); E = 0x4b; // Interaction.yh
  CYC(b_+358, b_+359); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+359, b_+360); mem_wr(gb, DE, A);
  CYC(b_+360, b_+361); SET_DE(DE + 1);
  CYC(b_+361, b_+362); SET_DE(DE + 1);
  CYC(b_+362, b_+363); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+363, b_+364); mem_wr(gb, DE, A);
  RET(b_+364); return;
}

// state1@func_7941: angle from @table_794f, entry var03+a.
static void din_event_load_angle(GB *gb) {
  BASE(interactionCode4f_state1);
  CYC(b_+381, b_+382); B = A;
  CYC(b_+382, b_+384); E = 0x43; // Interaction.var03
  CYC(b_+384, b_+385); A = mem_rd(gb, DE);
  CYC(b_+385, b_+386); alu_add(gb, B);
  CYC(b_+386, b_+389); SET_HL(b_+395); // @table_794f
  CYC(b_+389, b_+390); din_event_add_a_to_hl(gb, b_+390);
  CYC(b_+390, b_+392); E = 0x49; // Interaction.angle
  CYC(b_+392, b_+393); A = mem_rd(gb, HL);
  CYC(b_+393, b_+394); mem_wr(gb, DE, A);
  RET(b_+394); return;
}

// state1@func_7957: the ring with var03 == 0 plays sound a for the group.
static void din_event_play_sound_if_first(GB *gb) {
  BASE(interactionCode4f_state1);
  CYC(b_+403, b_+404); B = A;
  CYC(b_+404, b_+406); E = 0x43; // Interaction.var03
  CYC(b_+406, b_+407); A = mem_rd(gb, DE);
  CYC(b_+407, b_+408); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+408); return; }
  CYC(b_+408, b_+409);
  CYC(b_+409, b_+410); A = B;
  CYC(b_+410, b_+413);
  TAIL(playSound_b00);
}

// state1@func_7968: spawns 8 subid-3 sparks with counter1 a, var70/71 bc, var03 7..0.
static void din_event_spawn_sparks(GB *gb) {
  BASE(interactionCode4f_state1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+420, b_+423); SET_HL(hFF8C);
  CYC(b_+423, b_+425); mem_wr(gb, HL, 0x00);
  CYC(b_+425, b_+427); mem_wr(gb, hFF8B, A);
  CYC(b_+427, b_+429); A = 0x08;
  CYC(b_+429, b_+431); mem_wr(gb, hFF8D, A);
  for (;;) {
    CALL_C(b_+431, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+434);
    if (!(F & FZ)) { RET_TAKEN(b_+434); return; }
    CYC(b_+434, b_+435);
    CYC(b_+435, b_+437); mem_wr(gb, HL, 0x4f); // INTERAC_DIN_IMPRISONED_EVENT
    CYC(b_+437, b_+438); L = alu_inc8(gb, L);
    CYC(b_+438, b_+440); mem_wr(gb, HL, 0x03);
    CYC(b_+440, b_+442); L = 0x46; // Interaction.counter1
    CYC(b_+442, b_+444); A = mem_rd(gb, hFF8B);
    CYC(b_+444, b_+445); mem_wr(gb, HL, A);
    CYC(b_+445, b_+447); L = 0x70;
    CYC(b_+447, b_+448); mem_wr(gb, HL, B);
    CYC(b_+448, b_+449); L = alu_inc8(gb, L);
    CYC(b_+449, b_+450); mem_wr(gb, HL, C);
    CYC(b_+450, b_+452); L = 0x72; // Interaction.var32
    CYC(b_+452, b_+454); A = mem_rd(gb, hFF8C);
    CYC(b_+454, b_+455); mem_wr(gb, HL, A);
    CYC(b_+455, b_+457); A = mem_rd(gb, hFF8D);
    CYC(b_+457, b_+458); A = alu_dec8(gb, A);
    CYC(b_+458, b_+460); mem_wr(gb, hFF8D, A);
    CYC(b_+460, b_+462); L = 0x43; // Interaction.var03
    CYC(b_+462, b_+463); mem_wr(gb, HL, A);
    if (!(F & FZ)) { CYCT(b_+463, b_+465); continue; }
    CYC(b_+463, b_+465);
    break;
  }
  CYC(b_+465, b_+467); A = 0x5c;
  CYC(b_+467, b_+470);
  TAIL(playSound_b00);
}

// state1@subid1@func_7a00: at animation frame $70, play the sound in a.
static void din_event_sound_on_frame_70(GB *gb) {
  BASE(interactionCode4f_state1);
  CYC(b_+572, b_+573); H = D;
  CYC(b_+573, b_+575); L = 0x61; // Interaction.animParameter
  CYC(b_+575, b_+576); A = mem_rd(gb, HL);
  CYC(b_+576, b_+578); alu_cp(gb, 0x70);
  CYC(b_+578, b_+580); mem_wr(gb, HL, 0x00);
  if (!(F & FZ)) { RET_TAKEN(b_+580); return; }
  CYC(b_+580, b_+581);
  CYC(b_+581, b_+584);
  TAIL(playSound_b00);
}

// state1@func_7a1e: scroll the screen up toward the ring at y=c, at most to SCY $10.
static void din_event_scroll_toward(GB *gb) {
  BASE(interactionCode4f_state1);
  CYC(b_+602, b_+605); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+605, b_+606); B = A;
  CYC(b_+606, b_+607); A = C;
  CYC(b_+607, b_+608); alu_sub(gb, B);
  CYC(b_+608, b_+610); alu_sub(gb, 0x40);
  CYC(b_+610, b_+611); B = A;
  CYC(b_+611, b_+614); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+614, b_+615); alu_add(gb, B);
  CYC(b_+615, b_+617); alu_cp(gb, 0x10);
  if (!(F & FC)) { RET_TAKEN(b_+617); return; }
  CYC(b_+617, b_+618);
  CYC(b_+618, b_+621); mem_wr(gb, wGfxRegs1_SCY, A);
  CYC(b_+621, b_+623); mem_wr(gb, hCameraY, A);
  RET(b_+623); return;
}

void s_interactionCode4f_state1_hook(GB *gb) {
  BASE(interactionCode4f_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42; // Interaction.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (din_event_jump_table(gb));
    if (jt_ == b_+16) { goto subid0; }
    else if (jt_ == b_+549) { goto subid1; }
    else if (jt_ == b_+58) { goto subid2; }
    else if (jt_ == b_+470) { goto subid3; }
    else if (jt_ == b_+525) { goto subid4; }
    else if (jt_ == b_+624) { goto subid5; }
    else { HANDOFF(HL); }
  } while (0);

subid0:
  CYC(b_+16, b_+19); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+19, b_+21); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(b_+21, b_+24); TAIL(interactionDelete); }
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto subid0_animate; }
  CYC(b_+26, b_+28);
  CALL_C(b_+28, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto subid0_oscillate; }
  CYC(b_+31, b_+33);
  CALL_C(b_+33, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+36);
  CYC(b_+36, b_+38); L = 0x4b; // Interaction.yh
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x4a);
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x81);
  CYC(b_+44, b_+46); A = 0x0e;
  CALL_C(b_+46, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+49);
subid0_oscillate:
  CALL_C(b_+49, objectOscillateZ_hook, SYM(objectOscillateZ), b_+52);
subid0_animate:
  CALL_C(b_+52, interactionAnimate_hook, SYM(interactionAnimate), b_+55);
  CYC(b_+55, b_+58);
  TAIL(interactionRunScript);

subid2:
  CYC(b_+58, b_+60); E = 0x45; // Interaction.substate
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); push_effect(gb, b_+62);
  do { uint16_t jt_ = (din_event_jump_table(gb));
    if (jt_ == b_+80) { goto ss0; }
    else if (jt_ == b_+119) { goto ss1; }
    else if (jt_ == b_+154) { goto ss2; }
    else if (jt_ == b_+166) { goto ss3; }
    else if (jt_ == b_+195) { goto ss4; }
    else if (jt_ == b_+220) { goto ss5; }
    else if (jt_ == b_+257) { goto ss6; }
    else if (jt_ == b_+317) { goto ss7; }
    else if (jt_ == b_+337) { goto ss8; }
    else { HANDOFF(HL); }
  } while (0);

ss0:
  CALL_C(b_+80, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+83);
  CYC(b_+83, b_+85); A = 0x7c;
  CALL_L(b_+85, din_event_play_sound_if_first, b_+88);
  CYC(b_+88, b_+90); E = 0x43; // Interaction.var03
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); alu_add(gb, A);
  CYC(b_+92, b_+95); SET_HL(b_+138); // @@table_784e
  CYC(b_+95, b_+96); din_event_add_double_index(gb, b_+96);
  CYC(b_+96, b_+98); E = 0x49; // Interaction.angle
  CYC(b_+98, b_+99); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+102); E = 0x70;
  CYC(b_+102, b_+103); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CYC(b_+104, b_+105); SET_DE(DE + 1);
  CYC(b_+105, b_+106); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+106, b_+107); mem_wr(gb, DE, A);
  CYC(b_+107, b_+108); SET_DE(DE + 1);
  CYC(b_+108, b_+109); A = mem_rd(gb, HL);
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CYC(b_+110, b_+111); alu_xor(gb, A);
  CALL_L(b_+111, din_event_load_position, b_+114);
  CYC(b_+114, b_+116); E = 0x46; // Interaction.counter1
  CYC(b_+116, b_+118); A = 0x3c;
  CYC(b_+118, b_+119); mem_wr(gb, DE, A);

ss1:
  CALL_C(b_+119, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+122);
  CYC(b_+122, b_+124); E = 0x5a; // Interaction.visible
  if (!(F & FZ)) { CYCT(b_+124, b_+126); goto ss1_blink; }
  CYC(b_+124, b_+126);
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+129); alu_or(gb, 0x80);
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  CYC(b_+130, b_+133);
  TAIL(interactionIncSubstate);
ss1_blink:
  CYC(b_+133, b_+134); A = mem_rd(gb, DE);
  CYC(b_+134, b_+136); alu_xor(gb, 0x80);
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  RET(b_+137); return;

ss2:
  CYC(b_+154, b_+155); H = D;
  CYC(b_+155, b_+157); L = 0x71;
  CYC(b_+157, b_+158); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+158); return; }
  CYC(b_+158, b_+159);
  CYC(b_+159, b_+161); L = 0x50; // Interaction.speed
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0x78);
  CYC(b_+163, b_+166);
  TAIL(interactionIncSubstate);

ss3:
  CALL_C(b_+166, objectApplySpeed_hook, SYM(objectApplySpeed), b_+169);
  CYC(b_+169, b_+171); E = 0x70;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+173); B = A;
  CYC(b_+173, b_+175); E = 0x4b; // Interaction.yh
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+177); C = A;
  CYC(b_+177, b_+178); alu_cp(gb, B);
  CYC(b_+178, b_+180); E = 0x43; // Interaction.var03
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  if (!(F & FC)) { CYCT(b_+181, b_+183); goto ss3_scroll; }
  CYC(b_+181, b_+183);
  CYC(b_+183, b_+184); alu_xor(gb, A);
  CALL_L(b_+184, din_event_load_angle, b_+187);
  CYC(b_+187, b_+190);
  TAIL(interactionIncSubstate);
ss3_scroll:
  CYC(b_+190, b_+191); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+191); return; }
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+195);
  din_event_scroll_toward(gb);
  return;

ss4:
  CYC(b_+195, b_+196); H = D;
  CYC(b_+196, b_+198); L = 0x72; // Interaction.var32
  CYC(b_+198, b_+199); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+199); return; }
  CYC(b_+199, b_+200);
  CALL_C(b_+200, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+203);
  CYC(b_+203, b_+205); L = 0x46; // Interaction.counter1
  CYC(b_+205, b_+207); mem_wr(gb, HL, 0xa0);
  CYC(b_+207, b_+209); L = 0x43; // Interaction.var03
  CYC(b_+209, b_+210); A = mem_rd(gb, HL);
  CYC(b_+210, b_+211); alu_or(gb, A);
  CYC(b_+211, b_+214); SET_BC(0x4882);
  CYC(b_+214, b_+216); A = 0xfe;
  if (F & FZ) CALL_L_CC(b_+216, din_event_spawn_sparks, b_+219);
  else CYC(b_+216, b_+219);
  RET(b_+219); return;

ss5:
  CALL_C(b_+220, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+223);
  if (!(F & FZ)) { CYCT(b_+223, b_+225); goto ss5_turn; }
  CYC(b_+223, b_+225);
  CALL_C(b_+225, objectSetVisible_hook, SYM(objectSetVisible), b_+228);
  CYC(b_+228, b_+230); L = 0x46; // Interaction.counter1
  CYC(b_+230, b_+232); mem_wr(gb, HL, 0x28);
  CYC(b_+232, b_+234); A = 0x04;
  CALL_L(b_+234, din_event_load_position, b_+237);
  CYC(b_+237, b_+240);
  TAIL(interactionIncSubstate);
ss5_turn:
  CYC(b_+240, b_+242); L = 0x49; // Interaction.angle
  CYC(b_+242, b_+243); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+243, b_+244); A = mem_rd(gb, HL);
  CYC(b_+244, b_+246); alu_and(gb, 0x1f);
  CYC(b_+246, b_+247); mem_wr(gb, HL, A);
  CYC(b_+247, b_+249); A = 0x20;
  CYC(b_+249, b_+251); E = 0x49;
  CYC(b_+251, b_+254); SET_BC(0x4882);
  CYC(b_+254, b_+257);
  TAIL(objectSetPositionInCircleArc);

ss6:
  CALL_C(b_+257, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+260);
  if (!(F & FZ)) { RET_TAKEN(b_+260); return; }
  CYC(b_+260, b_+261);
  CYC(b_+261, b_+263); L = 0x50; // Interaction.speed
  CYC(b_+263, b_+265); mem_wr(gb, HL, 0x14);
  CYC(b_+265, b_+267); L = 0x46; // Interaction.counter1
  CYC(b_+267, b_+269); mem_wr(gb, HL, 0x3c);
  CYC(b_+269, b_+271); A = 0x04;
  CALL_L(b_+271, din_event_load_angle, b_+274);
  CYC(b_+274, b_+276); B = 0x02;
  for (;;) {
    CALL_C(b_+276, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+279);
    if (!(F & FZ)) { CYCT(b_+279, b_+281); break; }
    CYC(b_+279, b_+281);
    CYC(b_+281, b_+283); mem_wr(gb, HL, 0x4f); // INTERAC_DIN_IMPRISONED_EVENT
    CYC(b_+283, b_+284); L = alu_inc8(gb, L);
    CYC(b_+284, b_+286); mem_wr(gb, HL, 0x04);
    CYC(b_+286, b_+287); L = alu_inc8(gb, L);
    CYC(b_+287, b_+288); A = B;
    CYC(b_+288, b_+289); A = alu_dec8(gb, A);
    CYC(b_+289, b_+290); mem_wr(gb, HL, A);
    CYC(b_+290, b_+292); L = 0x46; // Interaction.counter1
    CYC(b_+292, b_+294); mem_wr(gb, HL, 0x0a);
    if (F & FZ) { CYCT(b_+294, b_+296); goto ss6_copy; }
    CYC(b_+294, b_+296);
    CYC(b_+296, b_+298); mem_wr(gb, HL, 0x14);
ss6_copy:
    CALL_C(b_+298, objectCopyPosition_hook, SYM(objectCopyPosition), b_+301);
    CYC(b_+301, b_+303); E = 0x49; // Interaction.angle
    CYC(b_+303, b_+304); L = E;
    CYC(b_+304, b_+305); A = mem_rd(gb, DE);
    CYC(b_+305, b_+306); mem_wr(gb, HL, A);
    CYC(b_+306, b_+308); E = 0x50; // Interaction.speed
    CYC(b_+308, b_+309); L = E;
    CYC(b_+309, b_+310); A = mem_rd(gb, DE);
    CYC(b_+310, b_+311); mem_wr(gb, HL, A);
    CYC(b_+311, b_+312); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+312, b_+314); continue; }
    CYC(b_+312, b_+314);
    break;
  }
  CYC(b_+314, b_+317);
  TAIL(interactionIncSubstate);

ss7:
  CALL_C(b_+317, objectApplySpeed_hook, SYM(objectApplySpeed), b_+320);
  CALL_C(b_+320, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+323);
  if (!(F & FZ)) { RET_TAKEN(b_+323); return; }
  CYC(b_+323, b_+324);
  CYC(b_+324, b_+327); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+327, b_+329); mem_wr(gb, HL, 0x0c);
  CYC(b_+329, b_+331); A = 0x79;
  CALL_L(b_+331, din_event_play_sound_if_first, b_+334);
  CYC(b_+334, b_+337);
  TAIL(interactionIncSubstate);

ss8:
  CYC(b_+337, b_+340); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+340, b_+341); A = mem_rd(gb, HL);
  CYC(b_+341, b_+343); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { RET_TAKEN(b_+343); return; }
  CYC(b_+343, b_+344);
  CYC(b_+344, b_+347);
  TAIL(interactionDelete);

subid3:
  CYC(b_+470, b_+471); H = D;
  CYC(b_+471, b_+473); L = 0x46; // Interaction.counter1
  CYC(b_+473, b_+474); A = mem_rd(gb, HL);
  CYC(b_+474, b_+475); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+475, b_+477); goto subid3_run; }
  CYC(b_+475, b_+477);
  CYC(b_+477, b_+478); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+478, b_+481); TAIL(interactionDelete); }
  CYC(b_+478, b_+481);
subid3_run:
  CYC(b_+481, b_+483); E = 0x45; // Interaction.substate
  CYC(b_+483, b_+484); A = mem_rd(gb, DE);
  CYC(b_+484, b_+485); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+485, b_+487); goto subid3_fly; }
  CYC(b_+485, b_+487);
  CALL_C(b_+487, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+490);
  if (!(F & FZ)) { RET_TAKEN(b_+490); return; }
  CYC(b_+490, b_+491);
  CALL_L(b_+491, din_event_set_animation_for_var03, b_+494);
  CALL_L(b_+494, din_event_place, b_+497);
  CALL_C(b_+497, objectSetVisible_hook, SYM(objectSetVisible), b_+500);
  CYC(b_+500, b_+503);
  TAIL(interactionIncSubstate);
subid3_fly:
  CALL_C(b_+503, objectApplySpeed_hook, SYM(objectApplySpeed), b_+506);
  CALL_C(b_+506, interactionAnimate_hook, SYM(interactionAnimate), b_+509);
  CYC(b_+509, b_+511); E = 0x61; // Interaction.animParameter
  CYC(b_+511, b_+512); A = mem_rd(gb, DE);
  CYC(b_+512, b_+513); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+513); return; }
  CYC(b_+513, b_+514);
  CYC(b_+514, b_+515); H = D;
  CYC(b_+515, b_+517); L = 0x45; // Interaction.substate
  CYC(b_+517, b_+519); mem_wr(gb, HL, 0x00);
  CALL_L(b_+519, din_event_set_counter2, b_+522);
  CYC(b_+522, b_+525);
  TAIL(objectSetInvisible);

subid4:
  CALL_C(b_+525, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+528);
  if (!(F & FZ)) { CYCT(b_+528, b_+530); goto subid4_move; }
  CYC(b_+528, b_+530);
  CALL_C(b_+530, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+533);
  if (!(F & FZ)) { RET_TAKEN(b_+533); return; }
  CYC(b_+533, b_+534);
  CYC(b_+534, b_+537);
  TAIL(interactionIncSubstate);
subid4_move:
  CYC(b_+537, b_+540); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+540, b_+541); A = mem_rd(gb, HL);
  CYC(b_+541, b_+543); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+543, b_+546); TAIL(interactionDelete); }
  CYC(b_+543, b_+546);
  CYC(b_+546, b_+549);
  TAIL(objectApplySpeed);

subid1:
  CYC(b_+549, b_+551); E = 0x45; // Interaction.substate
  CYC(b_+551, b_+552); A = mem_rd(gb, DE);
  CYC(b_+552, b_+553); push_effect(gb, b_+553);
  do { uint16_t jt_ = (din_event_jump_table(gb));
    if (jt_ == b_+559) { goto s1ss0; }
    else if (jt_ == b_+584) { goto s1ss1; }
    else if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), interactionRunScript_hook)) { interactionRunScript_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

s1ss0:
  CALL_C(b_+559, interactionRunScript_hook, SYM(interactionRunScript), b_+562);
  if (F & FC) { CYCT(b_+562, b_+564); goto s1ss0_next; }
  CYC(b_+562, b_+564);
  CALL_C(b_+564, interactionAnimate_hook, SYM(interactionAnimate), b_+567);
  CYC(b_+567, b_+569);
  din_event_sound_on_frame_70(gb);
  return;
s1ss0_next:
  CYC(b_+569, b_+572);
  TAIL(interactionIncSubstate);

s1ss1:
  CYC(b_+584, b_+587); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+587, b_+589); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+589); return; }
  CYC(b_+589, b_+590);
  CALL_C(b_+590, objectSetInvisible_hook, SYM(objectSetInvisible), b_+593);
  CYC(b_+593, b_+596); SET_HL(SYM(dinImprisonedScript_OnoxSendsTempleDown));
  CALL_C(b_+596, interactionSetScript_hook, SYM(interactionSetScript), b_+599);
  CYC(b_+599, b_+602);
  TAIL(interactionIncSubstate);

subid5:
  CALL_C(b_+624, interactionAnimate_hook, SYM(interactionAnimate), b_+627);
  CALL_L(b_+627, din_event_sound_on_frame_70, b_+630);
  CYC(b_+630, b_+633);
  TAIL(interactionRunScript);
}

static uint16_t dinImprisonedEvent_d_jump_table(GB *gb) {
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

// INTERAC_DIN_IMPRISONED_EVENT
void s_interactionCode4f_hook(GB *gb) {
  BASE(interactionCode4f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dinImprisonedEvent_d_jump_table(gb));
    if (jt_ == SYM(interactionCode4f_state0) && hook_is(gb, SYM(interactionCode4f_state0), s_interactionCode4f_state0_hook)) { s_interactionCode4f_state0_hook(gb); return; }
    if (jt_ == SYM(interactionCode4f_state1) && hook_is(gb, SYM(interactionCode4f_state1), s_interactionCode4f_state1_hook)) { s_interactionCode4f_state1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

