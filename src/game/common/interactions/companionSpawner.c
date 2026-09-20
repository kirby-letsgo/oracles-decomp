#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t companionSpawner_jump_table(GB *gb) {
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

static void companionSpawner_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void companionSpawner_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_COMPANION_SPAWNER
//
// Spawns the current animal companion -- Ricky, Dimitri, or Moosh -- either via a flute call
// (subid 6, searching the room's edges for a walkable tile to enter from) or via one of subid
// 0-5's story cutscenes, each gated on essences, treasures, or global flags, then loading the
// companion's preset id/position from @presetCompanionData. That table is itself pure data,
// confirmed via the transliterate report -- disassembling 0x4c4c as code produces garbage
// instructions that bleed into the following routine -- so it is read here only through mem_rd
// at its literal ROM address, never modeled as its own local.
//
// Nesting analysis: every real call instruction in this routine, to @checkVerticalCompanionSpawnPosition,
// @checkHorizontalCompanionSpawnPosition, @checkCompanionSpawnColumnRange, and
// @checkCompanionSpawnRowRange -- two call sites apiece -- is issued directly from this
// function's own top-level flow inside @label_0a_047, never from within another already
// outstanding goto'd local. So unlike makuTree.c or makuSprout.c, there is no nesting depth to
// track here: every one of these locals is entered with zero outstanding manual pushes, and
// every resume check below compares the emulated stack pointer against sp0_ directly, at depth
// zero, never against sp0_ minus twice some nesting depth. The convertShortToLongPosition_hook
// calls made inside those locals are plain CALL_C calls to an already-hooked leaf and need no
// resume tracking of their own since CALL_C handles that internally.
// @loadCompanionPresetIfHasntLeft falling into @loadCompanionPreset, and the various @subidNN
// handlers jumping into it, are all jr or fallthrough, translated as goto, not call, so they
// carry no stack effects either.
void interactionCode67_hook(GB *gb) {
  BASE(interactionCode67);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+5, b_+7); goto label_0a_045; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_rlca(gb);
  if (F & FC) { CYCT(b_+9, b_+11); goto fluteCall; } // jr c
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); A = W8(w1Companion_enabled);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); goto deleteSelf; } // jp nz
  CYC(b_+15, b_+18);

label_0a_045:
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = companionSpawner_jump_table(gb);
    if (target == b_+290) goto subid00;
    if (target == b_+316) goto subid01;
    if (target == b_+345) goto subid02;
    if (target == b_+333) goto subid03;
    if (target == b_+357) goto subid04;
    if (target == b_+366) goto subid05;
    HANDOFF(target);
  }

fluteCall:
  CYC(b_+32, b_+35); A = W8(w1Companion_enabled);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+36, b_+38); goto label_0a_047; } // jr z
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+41); A = W8(w1Companion_id);
  CYC(b_+41, b_+43); alu_cp(gb, 0x0e); // SPECIALOBJECT_MOOSH+1
  if (!(F & FC)) { CYCT(b_+43, b_+45); goto label_0a_047; } // jr nc
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); alu_cp(gb, 0x0a); // SPECIALOBJECT_MINECART
  if (!(F & FZ)) { CYCT(b_+47, b_+50); goto deleteSelf; } // jp nz
  CYC(b_+47, b_+50);

label_0a_047:
  CYC(b_+50, b_+53); A = W8(wTilesetFlags);
  CYC(b_+53, b_+55); alu_and(gb, 0x81); // TILESETFLAG_PAST | TILESETFLAG_OUTDOORS
  CYC(b_+55, b_+57); alu_cp(gb, 0x01); // TILESETFLAG_OUTDOORS
  if (!(F & FZ)) { CYCT(b_+57, b_+60); goto deleteSelf; } // jp nz
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+63); SET_BC(SYM(interaction6b_subid0f__state1)); // TX_510f
  CYC(b_+63, b_+66); A = W8(wFluteIcon);
  CYC(b_+66, b_+67); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+67, b_+70); goto showTextAndDelete; } // jp z
  CYC(b_+67, b_+70);
  CYC(b_+70, b_+73); A = W8(wActiveRoom);
  CYC(b_+73, b_+76); SET_HL(SYM(companionCallableRooms)); // companionCallableRooms
  CALL_C(b_+76, checkFlag_hook, SYM(checkFlag), b_+79);
  if (F & FZ) { CYCT(b_+79, b_+82); goto fluteSongFellFlat; } // jp z
  CYC(b_+79, b_+82);
  CYC(b_+82, b_+85); A = W8(w1Companion_enabled);
  CYC(b_+85, b_+86); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+86, b_+89); goto deleteSelf; } // jp nz
  CYC(b_+86, b_+89);

  // [var3e/var3f] = Link's position
  CYC(b_+89, b_+91); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+91, b_+94); SET_HL(w1Link_yh);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+95, b_+97); alu_and(gb, 0xf0);
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+99); L = alu_inc8(gb, L);
  CYC(b_+99, b_+100); E = alu_inc8(gb, E);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+103); A = alu_swap(gb, A);
  CYC(b_+103, b_+105); alu_and(gb, 0x0f);
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);

  // Try from top at Link's x position
  CYC(b_+106, b_+109); SET_HL(wRoomCollisions);
  CYC(b_+109, b_+110); companionSpawner_addAToHl_from_rst(gb, b_+110);
  CYC(b_+110, b_+113); push_effect(gb, b_+113); goto checkVerticalCompanionSpawnPosition; // call
afterCheckVertical1:
  CYC(b_+113, b_+115); B = 0xf8;
  CYC(b_+115, b_+116); L = C;
  CYC(b_+116, b_+118); H = 0x10;
  CYC(b_+118, b_+120); A = 0x02; // DIR_DOWN
  if (F & FZ) { CYCT(b_+120, b_+122); goto setCompanionDestination; } // jr z
  CYC(b_+120, b_+122);

  // Try from bottom at Link's x
  CYC(b_+122, b_+124); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+128); SET_HL(wRoomCollisions + 0x60);
  CYC(b_+128, b_+129); companionSpawner_addAToHl_from_rst(gb, b_+129);
  CYC(b_+129, b_+132); push_effect(gb, b_+132); goto checkVerticalCompanionSpawnPosition; // call
afterCheckVertical2:
  CYC(b_+132, b_+134); B = 0x88; // SMALL_ROOM_HEIGHT*$10+8
  CYC(b_+134, b_+135); L = C;
  CYC(b_+135, b_+137); H = 0x70; // SMALL_ROOM_HEIGHT*$10-$10
  CYC(b_+137, b_+139); A = 0x00; // DIR_UP
  if (F & FZ) { CYCT(b_+139, b_+141); goto setCompanionDestination; } // jr z
  CYC(b_+139, b_+141);

  // Try from right at Link's y
  CYC(b_+141, b_+143); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+143, b_+144); A = mem_rd(gb, DE);
  CYC(b_+144, b_+147); SET_HL(wRoomCollisions + 0x08);
  CYC(b_+147, b_+148); companionSpawner_addAToHl_from_rst(gb, b_+148);
  CYC(b_+148, b_+151); push_effect(gb, b_+151); goto checkHorizontalCompanionSpawnPosition; // call
afterCheckHorizontal1:
  CYC(b_+151, b_+153); C = 0xa8; // SMALL_ROOM_WIDTH*$10+8
  CYC(b_+153, b_+154); H = B;
  CYC(b_+154, b_+156); L = 0x90; // SMALL_ROOM_WIDTH*$10-$10
  CYC(b_+156, b_+158); A = 0x03; // DIR_LEFT
  if (F & FZ) { CYCT(b_+158, b_+160); goto setCompanionDestination; } // jr z
  CYC(b_+158, b_+160);

  // Try from left at Link's y
  CYC(b_+160, b_+162); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+162, b_+163); A = mem_rd(gb, DE);
  CYC(b_+163, b_+166); SET_HL(wRoomCollisions);
  CYC(b_+166, b_+167); companionSpawner_addAToHl_from_rst(gb, b_+167);
  CYC(b_+167, b_+170); push_effect(gb, b_+170); goto checkHorizontalCompanionSpawnPosition; // call
afterCheckHorizontal2:
  CYC(b_+170, b_+172); C = 0xf8;
  CYC(b_+172, b_+173); H = B;
  CYC(b_+173, b_+175); L = 0x10;
  CYC(b_+175, b_+177); A = 0x01; // DIR_RIGHT
  if (F & FZ) { CYCT(b_+177, b_+179); goto setCompanionDestination; } // jr z
  CYC(b_+177, b_+179);

  // Try from top at a range of x positions
  CYC(b_+179, b_+182); SET_HL(wRoomCollisions + 0x03);
  CYC(b_+182, b_+185); push_effect(gb, b_+185); goto checkCompanionSpawnColumnRange; // call
afterCheckColumnRange1:
  CYC(b_+185, b_+187); B = 0xf8;
  CYC(b_+187, b_+188); L = C;
  CYC(b_+188, b_+190); H = 0x10;
  CYC(b_+190, b_+192); A = 0x02; // DIR_DOWN
  if (!(F & FZ)) { CYCT(b_+192, b_+194); goto setCompanionDestination; } // jr nz
  CYC(b_+192, b_+194);

  // Try from bottom at a range of x positions
  CYC(b_+194, b_+197); SET_HL(wRoomCollisions + 0x63);
  CYC(b_+197, b_+200); push_effect(gb, b_+200); goto checkCompanionSpawnColumnRange; // call
afterCheckColumnRange2:
  CYC(b_+200, b_+202); B = 0x88;
  CYC(b_+202, b_+203); L = C;
  CYC(b_+203, b_+205); H = 0x70;
  CYC(b_+205, b_+207); A = 0x00; // DIR_UP
  if (!(F & FZ)) { CYCT(b_+207, b_+209); goto setCompanionDestination; } // jr nz
  CYC(b_+207, b_+209);

  // Try from right at a range of y positions
  CYC(b_+209, b_+212); SET_HL(wRoomCollisions + 0x28);
  CYC(b_+212, b_+215); push_effect(gb, b_+215); goto checkCompanionSpawnRowRange; // call
afterCheckRowRange1:
  CYC(b_+215, b_+217); C = 0xa8;
  CYC(b_+217, b_+218); H = B;
  CYC(b_+218, b_+220); L = 0x90;
  CYC(b_+220, b_+222); A = 0x03; // DIR_LEFT
  if (!(F & FZ)) { CYCT(b_+222, b_+224); goto setCompanionDestination; } // jr nz
  CYC(b_+222, b_+224);

  // Try from left at a range of y positions
  CYC(b_+224, b_+227); SET_HL(wRoomCollisions + 0x20);
  CYC(b_+227, b_+230); push_effect(gb, b_+230); goto checkCompanionSpawnRowRange; // call
afterCheckRowRange2:
  CYC(b_+230, b_+232); C = 0xf8;
  CYC(b_+232, b_+233); H = B;
  CYC(b_+233, b_+235); L = 0x10;
  CYC(b_+235, b_+237); A = 0x01; // DIR_RIGHT
  if (F & FZ) { CYCT(b_+237, b_+239); goto fluteSongFellFlat; } // jr z
  CYC(b_+237, b_+239);

// @param a Direction companion should move in
// @param bc Initial Y/X position
// @param hl Y/X destination
setCompanionDestination:
  CYC(b_+239, b_+240); push_effect(gb, DE); // push de
  CYC(b_+240, b_+241); push_effect(gb, HL); // push hl
  CYC(b_+241, b_+242); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+242, b_+245); SET_HL(wLastAnimalMountPointY);
  CYC(b_+245, b_+246); mem_wr(gb, HL, D);
  CYC(b_+246, b_+247); L = alu_inc8(gb, L);
  CYC(b_+247, b_+248); mem_wr(gb, HL, E);
  CYC(b_+248, b_+249); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+249, b_+252); SET_HL(w1Companion_direction);
  CYC(b_+252, b_+253); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+253, b_+255); A = alu_swap(gb, A);
  CYC(b_+255, b_+256); alu_rrca(gb);
  CYC(b_+256, b_+257); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+257, b_+258); L = alu_inc8(gb, L);
  CYC(b_+258, b_+259); mem_wr(gb, HL, B);
  CYC(b_+259, b_+261); L = OBJ_XH;
  CYC(b_+261, b_+262); mem_wr(gb, HL, C);
  CYC(b_+262, b_+264); L = OBJ_ENABLED;
  CYC(b_+264, b_+265); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+265, b_+266); L = alu_inc8(gb, L);
  CYC(b_+266, b_+269); A = W8(wAnimalCompanion);
  CYC(b_+269, b_+270); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [SpecialObject.id]
  // State $0c = entering screen from flute call
  CYC(b_+270, b_+272); L = OBJ_STATE;
  CYC(b_+272, b_+274); A = 0x0c;
  CYC(b_+274, b_+275); mem_wr(gb, HL, A);
  CYC(b_+275, b_+277); goto deleteSelf; // jr

fluteSongFellFlat:
  CYC(b_+277, b_+280); SET_BC((SYM(interaction6b_subid0f__state0) + 20)); // TX_510c

showTextAndDelete:
  CYC(b_+280, b_+283); A = W8(wTextIsActive);
  CYC(b_+283, b_+284); alu_or(gb, A);
  if (F & FZ) { CALL_C_CC(b_+284, showText_hook, SYM(showText), b_+287); } else { CYC(b_+284, b_+287); } // call z

deleteSelf:
  CYC(b_+287, b_+290); interactionDelete_hook(gb); return; // jp

// Moosh being attacked by ghosts
subid00:
  CYC(b_+290, b_+293); SET_HL(wMooshState);
  CYC(b_+293, b_+296); A = W8(wEssencesObtained);
  CYC(b_+296, b_+298); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+298, b_+300); goto deleteSelf; } // jr z
  CYC(b_+298, b_+300);
  CYC(b_+300, b_+303); A = mem_rd(gb, wGroup1RoomFlags + 0x79); // wPastRoomFlags+$79
  CYC(b_+303, b_+305); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+305, b_+307); goto deleteSelf; } // jr z
  CYC(b_+305, b_+307);
  CYC(b_+307, b_+309); A = 0x52; // TREASURE_CHEVAL_ROPE
  CALL_C(b_+309, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+312);
  if (!(F & FC)) { CYCT(b_+312, b_+314); goto loadCompanionPresetIfHasntLeft; } // jr nc
  CYC(b_+312, b_+314);
  CYC(b_+314, b_+316); goto deleteSelf; // jr

// Moosh saying goodbye after getting cheval rope
subid01:
  CYC(b_+316, b_+319); SET_HL(wMooshState);
  CYC(b_+319, b_+321); A = 0x40;
  CYC(b_+321, b_+322); alu_and(gb, mem_rd(gb, HL)); // and (hl)
  if (!(F & FZ)) { CYCT(b_+322, b_+324); goto deleteSelf; } // jr nz
  CYC(b_+322, b_+324);
  CYC(b_+324, b_+326); A = 0x52; // TREASURE_CHEVAL_ROPE
  CALL_C(b_+326, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+329);
  if (F & FC) { CYCT(b_+329, b_+331); goto loadCompanionPresetIfHasntLeft; } // jr c
  CYC(b_+329, b_+331);

deleteSelf2:
  CYC(b_+331, b_+333); goto deleteSelf; // jr

// Dimitri being attacked by hungry tokays
subid03:
  CYC(b_+333, b_+336); SET_HL(wDimitriState);
  CYC(b_+336, b_+339); A = W8(wEssencesObtained);
  CYC(b_+339, b_+341); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(b_+341, b_+343); goto deleteSelf; } // jr z
  CYC(b_+341, b_+343);
  CYC(b_+343, b_+345); goto loadCompanionPresetIfHasntLeft; // jr

// Ricky looking for gloves
subid02:
  CYC(b_+345, b_+347); A = 0x15; // GLOBALFLAG_GAVE_ROPE_TO_RAFTON
  CALL_C(b_+347, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+350);
  if (F & FZ) { CYCT(b_+350, b_+352); goto deleteSelf; } // jr z
  CYC(b_+350, b_+352);
  CYC(b_+352, b_+355); SET_HL(wRickyState);
  CYC(b_+355, b_+357); goto loadCompanionPresetIfHasntLeft; // jr

// Companion lost in forest
subid04:
  CYC(b_+357, b_+359); A = 0x42; // GLOBALFLAG_COMPANION_LOST_IN_FOREST
  CALL_C(b_+359, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+362);
  if (F & FZ) { CYCT(b_+362, b_+364); goto deleteSelf; } // jr z
  CYC(b_+362, b_+364);
  CYC(b_+364, b_+366); goto label_0a_052; // jr

// Cutscene outside forest where you get the flute
subid05:
  CYC(b_+366, b_+368); A = 0x24; // GLOBALFLAG_SAVED_COMPANION_FROM_FOREST
  CALL_C(b_+368, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+371);
  if (F & FZ) { CYCT(b_+371, b_+373); goto deleteSelf; } // jr z
  CYC(b_+371, b_+373);

label_0a_052:
  CYC(b_+373, b_+375); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(b_+375, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+378);
  if (!(F & FZ)) { CYCT(b_+378, b_+380); goto deleteSelf; } // jr nz
  CYC(b_+378, b_+380);
  CYC(b_+380, b_+382); goto loadCompanionPreset; // jr

// This bit of the companion's state is set if he's left after his sidequest
loadCompanionPresetIfHasntLeft:
  CYC(b_+382, b_+383); A = mem_rd(gb, HL);
  CYC(b_+383, b_+385); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+385, b_+387); goto deleteSelf2; } // jr nz
  CYC(b_+385, b_+387);

// Load a companion's ID and position from a table of presets based on subid.
// @presetCompanionData at 0x4c4c is pure data, 4 bytes per row: id, yh, xh, unused. It is read
// below only via mem_rd at its literal ROM address, never modeled as its own local -- see the
// transliterate report's own garbage disassembly of those bytes.
loadCompanionPreset:
  CYC(b_+387, b_+389); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+389, b_+390); A = mem_rd(gb, DE);
  CYC(b_+390, b_+391); alu_add(gb, A); // add a
  CYC(b_+391, b_+394); SET_HL(b_+495); // @presetCompanionData
  CYC(b_+394, b_+395); companionSpawner_add_double_index(gb, b_+395);
  CYC(b_+395, b_+398); SET_BC(w1Companion_enabled);
  CYC(b_+398, b_+400); A = 0x01;
  CYC(b_+400, b_+401); mem_wr(gb, BC, A);
  CYC(b_+401, b_+402); C = alu_inc8(gb, C);
  CYC(b_+402, b_+403); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+403, b_+404); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+404, b_+406); goto L_4bf6; } // jr nz
  CYC(b_+404, b_+406);
  CYC(b_+406, b_+409); A = W8(wAnimalCompanion);

L_4bf6:
  CYC(b_+409, b_+410); mem_wr(gb, BC, A);
  CYC(b_+410, b_+412); C = OBJ_YH;
  CYC(b_+412, b_+413); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+413, b_+414); mem_wr(gb, BC, A);
  CYC(b_+414, b_+417); W8(wLastAnimalMountPointY) = A;
  CYC(b_+417, b_+419); C = OBJ_XH;
  CYC(b_+419, b_+420); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+420, b_+421); mem_wr(gb, BC, A);
  CYC(b_+421, b_+424); W8(wLastAnimalMountPointX) = A;
  CYC(b_+424, b_+425); alu_xor(gb, A);
  CYC(b_+425, b_+428); W8(wRememberedCompanionId) = A;
  CYC(b_+428, b_+430); goto deleteSelf2; // jr

// Check if the first 2 tiles near the edge of the screen are walkable for a companion.
// @param hl Address in wRoomCollisions to start at
// @param[out] bc Position to spawn at
// @param[out] zflag z if the companion can spawn from there
//
// Called from 4 top-level sites -- 0x4acb and 0x4ade for the vertical entry, 0x4af1 and 0x4b04
// for the horizontal entry -- all at this hook's own base stack depth sp0_. None of them are
// issued from within another outstanding goto'd local, so every resume check below compares the
// emulated stack pointer against sp0_ directly.
checkVerticalCompanionSpawnPosition:
  CYC(b_+430, b_+432); B = 0x10;
  CYC(b_+432, b_+434); goto L_4c11; // jr

checkHorizontalCompanionSpawnPosition:
  CYC(b_+434, b_+436); B = 0x01;

L_4c11:
  CYC(b_+436, b_+437); A = mem_rd(gb, HL);
  CYC(b_+437, b_+438); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(b_+438);
    if (gb->pc == b_+113 && gb->sp == sp0_) goto afterCheckVertical1;
    if (gb->pc == b_+132 && gb->sp == sp0_) goto afterCheckVertical2;
    if (gb->pc == b_+151 && gb->sp == sp0_) goto afterCheckHorizontal1;
    if (gb->pc == b_+170 && gb->sp == sp0_) goto afterCheckHorizontal2;
    return;
  } // ret nz
  CYC(b_+438, b_+439);
  CYC(b_+439, b_+440); A = L;
  CYC(b_+440, b_+441); alu_add(gb, B);
  CYC(b_+441, b_+442); L = A;
  CYC(b_+442, b_+443); A = mem_rd(gb, HL);
  CYC(b_+443, b_+444); alu_or(gb, A);
  CYC(b_+444, b_+445); A = L;
  if (!(F & FZ)) {
    RET_TAKEN(b_+445);
    if (gb->pc == b_+113 && gb->sp == sp0_) goto afterCheckVertical1;
    if (gb->pc == b_+132 && gb->sp == sp0_) goto afterCheckVertical2;
    if (gb->pc == b_+151 && gb->sp == sp0_) goto afterCheckHorizontal1;
    if (gb->pc == b_+170 && gb->sp == sp0_) goto afterCheckHorizontal2;
    return;
  } // ret nz
  CYC(b_+445, b_+446);
  CALL_C(b_+446, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+449);
  CYC(b_+449, b_+450); alu_xor(gb, A);
  RET(b_+450);
  if (gb->pc == b_+113 && gb->sp == sp0_) goto afterCheckVertical1;
  if (gb->pc == b_+132 && gb->sp == sp0_) goto afterCheckVertical2;
  if (gb->pc == b_+151 && gb->sp == sp0_) goto afterCheckHorizontal1;
  if (gb->pc == b_+170 && gb->sp == sp0_) goto afterCheckHorizontal2;
  return; // ret

// Checks the given column and up to the following 3 after for if the companion can spawn there.
// Also checks 3 rows/columns after the starting position given in hl.
// @param hl Starting position to check
// @param[out] bc Position to spawn at
// @param[out] zflag nz if valid position to spawn from found
//
// Called from 4 top-level sites -- 0x4b13 and 0x4b22 for the column entry, 0x4b31 and 0x4b40 for
// the row entry -- all at this hook's own base stack depth sp0_, same as the vertical/horizontal
// pair above. No nesting here either: every resume check compares the emulated stack pointer
// against sp0_ directly.
checkCompanionSpawnColumnRange:
  CYC(b_+451, b_+452); push_effect(gb, DE); // push de
  CYC(b_+452, b_+454); B = 0x01;
  CYC(b_+454, b_+456); E = 0x10;
  CYC(b_+456, b_+458); goto L_4c2c; // jr

checkCompanionSpawnRowRange:
  CYC(b_+458, b_+459); push_effect(gb, DE); // push de
  CYC(b_+459, b_+461); B = 0x10;
  CYC(b_+461, b_+463); E = 0x01;

L_4c2c:
  CYC(b_+463, b_+465); C = 0x04;

nextRowOrColumn:
  CYC(b_+465, b_+466); A = mem_rd(gb, HL);
  CYC(b_+466, b_+467); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+467, b_+469); goto tryThisRowOrColumn; } // jr z
  CYC(b_+467, b_+469);

resumeSearch:
  CYC(b_+469, b_+470); A = L;
  CYC(b_+470, b_+471); alu_add(gb, B);
  CYC(b_+471, b_+472); L = A;
  CYC(b_+472, b_+473); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+473, b_+475); goto nextRowOrColumn; } // jr nz
  CYC(b_+473, b_+475);
  CYC(b_+475, b_+476); SET_DE(pop_effect(gb)); // pop de
  RET(b_+476);
  if (gb->pc == b_+185 && gb->sp == sp0_) goto afterCheckColumnRange1;
  if (gb->pc == b_+200 && gb->sp == sp0_) goto afterCheckColumnRange2;
  if (gb->pc == b_+215 && gb->sp == sp0_) goto afterCheckRowRange1;
  if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCheckRowRange2;
  return; // ret

tryThisRowOrColumn:
  CYC(b_+477, b_+478); A = L;
  CYC(b_+478, b_+479); alu_add(gb, E);
  CYC(b_+479, b_+480); L = A;
  CYC(b_+480, b_+481); A = mem_rd(gb, HL);
  CYC(b_+481, b_+482); alu_or(gb, A);
  CYC(b_+482, b_+483); A = L;
  if (F & FZ) { CYCT(b_+483, b_+485); goto foundRowOrColumn; } // jr z
  CYC(b_+483, b_+485);
  CYC(b_+485, b_+486); alu_sub(gb, E);
  CYC(b_+486, b_+487); L = A;
  CYC(b_+487, b_+489); goto resumeSearch; // jr

foundRowOrColumn:
  CALL_C(b_+489, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+492);
  CYC(b_+492, b_+493); alu_or(gb, D);
  CYC(b_+493, b_+494); SET_DE(pop_effect(gb)); // pop de
  RET(b_+494);
  if (gb->pc == b_+185 && gb->sp == sp0_) goto afterCheckColumnRange1;
  if (gb->pc == b_+200 && gb->sp == sp0_) goto afterCheckColumnRange2;
  if (gb->pc == b_+215 && gb->sp == sp0_) goto afterCheckRowRange1;
  if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCheckRowRange2;
  return; // ret
}
