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

// parentItemCode_harp@getSelectedSongAddr: hl = wFluteIcon with z set for the flute; Ages points
// hl at wSelectedHarpSong for the harp.
static void flute_get_selected_song_addr(GB *gb) {
  BASE(parentItemCode_harp__getSelectedSongAddr);
  CYC(b_+0, b_+3); SET_HL(wFluteIcon);
  CYC(b_+3, b_+5); E = OBJ_ID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0e);
  if (game_seasons) { CYC(b_+8, b_+9); ret_effect(gb); return; }
  if (F & FZ) { CYCT(b_+8, b_+9); ret_effect(gb); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = wSelectedHarpSong & 0xff;
  CYC(b_+11, b_+12); ret_effect(gb);
}

void parentItemCode_flute_hook(GB *gb) {
  BASE(parentItemCode_flute);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = 0x04;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); push_effect(gb, b_+OE(4));
  do { uint16_t jt_ = (harp_parent_jump_table(gb));
    if (jt_ == b_+O(8)) { goto state0; }
    else if (jt_ == b_+O(65)) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+O(8), checkLinkOnGround_hook, SYM(checkLinkOnGround), b_+OE(11));
  if (!(F & FZ)) { CYCT(b_+O(11), b_+OE(14)); TAIL(clearParentItem); }
  CYC(b_+O(11), b_+OE(14));
  CYC(b_+O(14), b_+OE(17)); A = W8(wInstrumentsDisabledCounter);
  CYC(b_+O(17), b_+OE(18)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(18), b_+OE(21)); TAIL(clearParentItem); }
  CYC(b_+O(18), b_+OE(21));
  CALL_C(b_+O(21), isLinkInHole_hook, SYM(isLinkInHole), b_+OE(24));
  if (F & FC) { CYCT(b_+O(24), b_+OE(27)); TAIL(clearParentItem); }
  CYC(b_+O(24), b_+OE(27));
  CALL_C(b_+O(27), checkNoOtherParentItemsInUse_hook, SYM(checkNoOtherParentItemsInUse), b_+OE(30));
  if (!(F & FZ)) { CYCT(b_+O(30), b_+OE(33)); TAIL(clearParentItem); }
  CYC(b_+O(30), b_+OE(33));
  CYC(b_+O(33), b_+OE(35)); A = 0x80;
  CYC(b_+O(35), b_+OE(38)); W8(wcc95) = A;
  CYC(b_+O(38), b_+OE(40)); A = 0x7e;
  CYC(b_+O(40), b_+OE(43)); W8(wDisabledObjects) = A;
  CALL_C(b_+O(43), parentItemLoadAnimationAndIncState_hook, SYM(parentItemLoadAnimationAndIncState), b_+OE(46));
  CYC(b_+O(46), b_+OE(48)); B = 0x00;
  CYC(b_+O(48), b_+OE(51)); push_effect(gb, b_+OE(51)); flute_get_selected_song_addr(gb);
  if (F & FZ) CYCT(b_+O(51), b_+OE(53));
  else { CYC(b_+O(51), b_+OE(53)); CYC(b_+O(53), b_+OE(55)); B = 0x03; }
  CYC(b_+O(55), b_+OE(56)); A = mem_rd(gb, HL);
  CYC(b_+O(56), b_+OE(57)); alu_add(gb, B);
  CYC(b_+O(57), b_+OE(60)); SET_HL(b_+O(224));
  CYC(b_+O(60), b_+OE(61)); push_effect(gb, b_+OE(61)); harp_parent_add_a_to_hl(gb);
  CYC(b_+O(61), b_+OE(62)); A = mem_rd(gb, HL);
  CALL_C(b_+O(62), playSound_b00_hook, SYM(playSound_b00), b_+OE(65));

state1:
  CYC(b_+O(65), b_+OE(68)); SET_HL(w1Link_collisionType);
  CYC(b_+O(68), b_+OE(70)); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(b_+O(70), itemDecCounter1_hook, SYM(itemDecCounter1), b_+OE(73));
  CYC(b_+O(73), b_+OE(74)); A = mem_rd(gb, HL);
  CYC(b_+O(74), b_+OE(76)); alu_and(gb, 0x1f);
  if (!(F & FZ)) CYCT(b_+O(76), b_+OE(78));
  else {
    CYC(b_+O(76), b_+OE(78));
    CYC(b_+O(78), b_+OE(80)); L = 0x21;
    CYC(b_+O(80), b_+OE(82)); alu_bit(gb, 0, mem_rd(gb, HL));
    CYC(b_+O(82), b_+OE(85)); SET_BC(0xfcf8);
    if (F & FZ) CYCT(b_+O(85), b_+OE(87));
    else { CYC(b_+O(85), b_+OE(87)); CYC(b_+O(87), b_+OE(89)); C = 0x08; }
    CALL_C(b_+O(89), getRandomNumber_hook, SYM(getRandomNumber), b_+OE(92));
    CYC(b_+O(92), b_+OE(94)); alu_and(gb, 0x01);
    CYC(b_+O(94), b_+OE(95)); push_effect(gb, DE);
    CYC(b_+O(95), b_+OE(97)); D = 0xd0;
    CALL_C(b_+O(97), objectCreateFloatingMusicNote_hook, SYM(objectCreateFloatingMusicNote), b_+OE(100));
    CYC(b_+O(100), b_+OE(101)); SET_DE(pop_effect(gb));
  }
  CALL_C(b_+O(101), specialObjectAnimate_optimized_hook, SYM(specialObjectAnimate_optimized), b_+OE(104));
  CYC(b_+O(104), b_+OE(107)); push_effect(gb, b_+OE(107)); flute_get_selected_song_addr(gb);
  CYC(b_+O(107), b_+OE(109)); A = 0xff;
  if (!game_seasons) {
    if (F & FZ) CYCT(b_+109, b_+111);
    else { CYC(b_+109, b_+111); CYC(b_+111, b_+112); A = mem_rd(gb, HL); }
  }
  CYC(b_+O(112), b_+OE(115)); W8(wLinkPlayingInstrument) = A;
  CYC(b_+O(115), b_+OE(118)); W8(wLinkRidingObject) = A;
  CYC(b_+O(118), b_+OE(120)); C = 0x80;
  if (!game_seasons && !(F & FZ)) CYCT(b_+120, b_+122);
  else {
    if (!game_seasons) CYC(b_+120, b_+122);
    CYC(b_+O(122), b_+OE(123)); A = mem_rd(gb, HL);
    CYC(b_+O(123), b_+OE(124)); alu_or(gb, A);
    if (!(F & FZ)) CYCT(b_+O(124), b_+OE(126));
    else { CYC(b_+O(124), b_+OE(126)); CYC(b_+O(126), b_+OE(128)); C = 0x40; }
  }
  CYC(b_+O(128), b_+OE(130)); E = 0x21;
  CYC(b_+O(130), b_+OE(131)); A = mem_rd(gb, DE);
  CYC(b_+O(131), b_+OE(132)); alu_and(gb, C);
  if (F & FZ) { CYCT(b_+O(132), b_+OE(133)); ret_effect(gb); return; }
  CYC(b_+O(132), b_+OE(133));
  CYC(b_+O(133), b_+OE(136)); SET_HL(w1Link_collisionType);
  CYC(b_+O(136), b_+OE(138)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  if (!game_seasons) {
    CYC(b_+138, b_+141); push_effect(gb, b_+141); flute_get_selected_song_addr(gb);
    if (!(F & FZ)) { CYCT(b_+141, b_+143); goto harp; }
    CYC(b_+141, b_+143);
  }
  CYC(b_+O(143), b_+OE(146)); SET_BC(GV(0x6780, 0x5f80));
  CALL_C(b_+O(146), objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+OE(149));

clear_self:
  CYC(b_+O(149), b_+OE(150)); alu_xor(gb, A);
  CYC(b_+O(150), b_+OE(153)); W8(wDisabledObjects) = A;
  CYC(b_+O(153), b_+OE(156)); W8(wcc95) = A;
  CYC(b_+O(156), b_+OE(159)); TAIL(clearParentItem);

  if (!game_seasons) {
echoes_in_vain:
    CYC(b_+159, b_+162); SET_BC(0x5110);
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
    CYC(b_+221, b_+224); TAIL(specialObjectAnimate_optimized);
  }
}
