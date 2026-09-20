#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t harp_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void harp_parent_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    ret_effect(gb);
  }
}

// parentItemCode_flute@getSelectedSongAddr (at +231, no symbol of its own): hl = wFluteIcon
// with z set for the flute, wSelectedHarpSong otherwise. Ages instruction stream; the routine
// is not hooked under Seasons.
static void flute_get_selected_song_addr(GB *gb) {
  BASE(parentItemCode_flute);
  SET_HL(wFluteIcon);
  E = OBJ_ID;
  CYC(b_+231, b_+236);
  CYC(b_+236, b_+237); A = mem_rd(gb, DE);
  alu_cp(gb, 0x0e);
  CYC(b_+237, b_+239);
  if (F & FZ) { CYCT(b_+239, b_+240); ret_effect(gb); return; }
  CYC(b_+239, b_+240);
  L = wSelectedHarpSong & 0xff;
  CYC(b_+240, b_+242);
  CYC(b_+242, b_+243); ret_effect(gb);
}

void parentItemCode_flute_hook(GB *gb) {
  BASE(parentItemCode_flute);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (harp_parent_jump_table(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+65) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+8, checkLinkOnGround_hook, SYM(checkLinkOnGround), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+14); goto clear_self; }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+17); A = W8(wInstrumentsDisabledCounter);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); goto clear_self; }
  CYC(b_+18, b_+21);
  CALL_C(b_+21, isLinkInHole_hook, SYM(isLinkInHole), b_+24);
  if (F & FC) { CYCT(b_+24, b_+27); goto clear_self; }
  CYC(b_+24, b_+27);
  CALL_C(b_+27, checkNoOtherParentItemsInUse_hook, SYM(checkNoOtherParentItemsInUse), b_+30);
  if (!(F & FZ)) { CYCT(b_+30, b_+33); goto clear_self; }
  CYC(b_+30, b_+33);
  CYC(b_+33, b_+35); A = 0x80;
  CYC(b_+35, b_+38); W8(wcc95) = A;
  CYC(b_+38, b_+40); A = 0x7e;
  CYC(b_+40, b_+43); W8(wDisabledObjects) = A;
  CALL_C(b_+43, parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+46);
  CYC(b_+46, b_+48); B = 0x00;
  CYC(b_+48, b_+51); push_effect(gb, b_+51); flute_get_selected_song_addr(gb);
  if (F & FZ) CYCT(b_+51, b_+53);
  else { CYC(b_+51, b_+53); CYC(b_+53, b_+55); B = 0x03; }
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); alu_add(gb, B);
  CYC(b_+57, b_+60); SET_HL(b_+224);
  CYC(b_+60, b_+61); push_effect(gb, b_+61); harp_parent_add_a_to_hl(gb);
  CYC(b_+61, b_+62); A = mem_rd(gb, HL);
  CALL_C(b_+62, playSound_b00_hook, SYM(playSound_b00), b_+65);

state1:
  CYC(b_+65, b_+68); SET_HL(w1Link_collisionType);
  CYC(b_+68, b_+70); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+70, itemDecCounter1_hook, SYM(itemDecCounter1), b_+73);
  CYC(b_+73, b_+74); A = mem_rd(gb, HL);
  CYC(b_+74, b_+76); alu_and(gb, 0x1f);
  if (!(F & FZ)) CYCT(b_+76, b_+78);
  else {
    CYC(b_+76, b_+78);
    CYC(b_+78, b_+80); L = 0x21;
    CYC(b_+80, b_+82); alu_bit(gb, 0, mem_rd(gb, HL));
    CYC(b_+82, b_+85); SET_BC(0xfcf8);
    if (F & FZ) CYCT(b_+85, b_+87);
    else { CYC(b_+85, b_+87); CYC(b_+87, b_+89); C = 0x08; }
    CALL_C(b_+89, getRandomNumber_hook, SYM(getRandomNumber), b_+92);
    CYC(b_+92, b_+94); alu_and(gb, 0x01);
    CYC(b_+94, b_+95); push_effect(gb, DE);
    CYC(b_+95, b_+97); D = 0xd0;
    CALL_C(b_+97, objectCreateFloatingMusicNote_hook, SYM(objectCreateFloatingMusicNote), b_+100);
    CYC(b_+100, b_+101); SET_DE(pop_effect(gb));
  }
  CALL_C(b_+101, specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+104);
  CYC(b_+104, b_+107); push_effect(gb, b_+107); flute_get_selected_song_addr(gb);
  CYC(b_+107, b_+109); A = 0xff;
  if (F & FZ) CYCT(b_+109, b_+111);
  else { CYC(b_+109, b_+111); CYC(b_+111, b_+112); A = mem_rd(gb, HL); }
  CYC(b_+112, b_+115); W8(wLinkPlayingInstrument) = A;
  CYC(b_+115, b_+118); W8(wLinkRidingObject) = A;
  CYC(b_+118, b_+120); C = 0x80;
  if (!(F & FZ)) CYCT(b_+120, b_+122);
  else {
    CYC(b_+120, b_+122);
    CYC(b_+122, b_+123); A = mem_rd(gb, HL);
    CYC(b_+123, b_+124); alu_or(gb, A);
    if (!(F & FZ)) CYCT(b_+124, b_+126);
    else { CYC(b_+124, b_+126); CYC(b_+126, b_+128); C = 0x40; }
  }
  CYC(b_+128, b_+130); E = 0x21;
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+132); alu_and(gb, C);
  if (F & FZ) { CYCT(b_+132, b_+133); ret_effect(gb); return; }
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+136); SET_HL(w1Link_collisionType);
  CYC(b_+136, b_+138); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+138, b_+141); push_effect(gb, b_+141); flute_get_selected_song_addr(gb);
  if (!(F & FZ)) { CYCT(b_+141, b_+143); goto harp; }
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+146); SET_BC((SYM(specialObject0bOamDataPointers) + 32));
  CALL_C(b_+146, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+149);

clear_self:
  CYC(b_+149, b_+150); alu_xor(gb, A);
  CYC(b_+150, b_+153); W8(wDisabledObjects) = A;
  CYC(b_+153, b_+156); W8(wcc95) = A;
  CYC(b_+156, b_+159); clearParentItem_hook(gb); return;

echoes_in_vain:
  CYC(b_+159, b_+162); SET_BC((SYM(parentItemCode_bracelet__state0) + 26));
  CALL_C(b_+162, showText_hook, SYM(showText), b_+165);
  CYC(b_+165, b_+167); goto clear_self;

harp:
  CYC(b_+167, b_+170); A = W8(wTilesetFlags);
  CYC(b_+170, b_+172); alu_and(gb, 0x7e);
  if (!(F & FZ)) { CYCT(b_+172, b_+174); goto clear_self; }
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+175); A = mem_rd(gb, HL);
  CYC(b_+175, b_+176); push_effect(gb, b_+176);
  do { uint16_t jt_ = (harp_parent_jump_table(gb));
    if (jt_ == b_+149) { goto clear_self; }
    else if (jt_ == b_+184) { goto tune_echoes; }
    else if (jt_ == b_+193) { goto tune_currents; }
    else if (jt_ == b_+199) { goto tune_ages; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

tune_echoes:
  CALL_C(b_+184, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+187);
  CYC(b_+187, b_+189); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+189, b_+191); goto clear_self; }
  CYC(b_+189, b_+191);
  CYC(b_+191, b_+193); goto echoes_in_vain;

tune_currents:
  CYC(b_+193, b_+196); A = W8(wTilesetFlags);
  CYC(b_+196, b_+197); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+197, b_+199); goto echoes_in_vain; }
  CYC(b_+197, b_+199);

tune_ages:
  CALL_C(b_+199, restartSound_hook, SYM(restartSound), b_+202);
  CYC(b_+202, b_+204); A = 0x1b;
  CYC(b_+204, b_+207); W8(wCutsceneTrigger) = A;
  CYC(b_+207, b_+209); A = 0x6d;
  CYC(b_+209, b_+212); W8(wDisabledObjects) = A;
  CYC(b_+212, b_+215); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+215, b_+218); W8(wcde0) = A;
  CALL_C(b_+218, clearAllItemsAndPutLinkOnGround_hook, SYM(clearAllItemsAndPutLinkOnGround), b_+221);
  CYC(b_+221, b_+224); specialObjectAnimate_optimized_hook(gb);
}
