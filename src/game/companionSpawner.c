#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x4a5d, 0x4a5f); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4a5f, 0x4a60); A = mem_rd(gb, DE);
  CYC(0x4a60, 0x4a62); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x4a62, 0x4a64); goto label_0a_045; } // jr z
  CYC(0x4a62, 0x4a64);
  CYC(0x4a64, 0x4a65); A = mem_rd(gb, DE);
  CYC(0x4a65, 0x4a66); alu_rlca(gb);
  if (F & FC) { CYCT(0x4a66, 0x4a68); goto fluteCall; } // jr c
  CYC(0x4a66, 0x4a68);
  CYC(0x4a68, 0x4a6b); A = W8(w1Companion_enabled);
  CYC(0x4a6b, 0x4a6c); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4a6c, 0x4a6f); goto deleteSelf; } // jp nz
  CYC(0x4a6c, 0x4a6f);

label_0a_045:
  CYC(0x4a6f, 0x4a70); A = mem_rd(gb, DE);
  {
    CYC(0x4a70, 0x4a71); push_effect(gb, 0x4a71);
    uint16_t target = companionSpawner_jump_table(gb);
    if (target == 0x4b7f) goto subid00;
    if (target == 0x4b99) goto subid01;
    if (target == 0x4bb6) goto subid02;
    if (target == 0x4baa) goto subid03;
    if (target == 0x4bc2) goto subid04;
    if (target == 0x4bcb) goto subid05;
    HANDOFF(target);
  }

fluteCall:
  CYC(0x4a7d, 0x4a80); A = W8(w1Companion_enabled);
  CYC(0x4a80, 0x4a81); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4a81, 0x4a83); goto label_0a_047; } // jr z
  CYC(0x4a81, 0x4a83);
  CYC(0x4a83, 0x4a86); A = W8(w1Companion_id);
  CYC(0x4a86, 0x4a88); alu_cp(gb, 0x0e); // SPECIALOBJECT_MOOSH+1
  if (!(F & FC)) { CYCT(0x4a88, 0x4a8a); goto label_0a_047; } // jr nc
  CYC(0x4a88, 0x4a8a);
  CYC(0x4a8a, 0x4a8c); alu_cp(gb, 0x0a); // SPECIALOBJECT_MINECART
  if (!(F & FZ)) { CYCT(0x4a8c, 0x4a8f); goto deleteSelf; } // jp nz
  CYC(0x4a8c, 0x4a8f);

label_0a_047:
  CYC(0x4a8f, 0x4a92); A = W8(wTilesetFlags);
  CYC(0x4a92, 0x4a94); alu_and(gb, 0x81); // TILESETFLAG_PAST | TILESETFLAG_OUTDOORS
  CYC(0x4a94, 0x4a96); alu_cp(gb, 0x01); // TILESETFLAG_OUTDOORS
  if (!(F & FZ)) { CYCT(0x4a96, 0x4a99); goto deleteSelf; } // jp nz
  CYC(0x4a96, 0x4a99);
  CYC(0x4a99, 0x4a9c); SET_BC(0x510f); // TX_510f
  CYC(0x4a9c, 0x4a9f); A = W8(wFluteIcon);
  CYC(0x4a9f, 0x4aa0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4aa0, 0x4aa3); goto showTextAndDelete; } // jp z
  CYC(0x4aa0, 0x4aa3);
  CYC(0x4aa3, 0x4aa6); A = W8(wActiveRoom);
  CYC(0x4aa6, 0x4aa9); SET_HL(0x4c64); // companionCallableRooms
  CALL_C(0x4aa9, checkFlag_hook, 0x0205, 0x4aac);
  if (F & FZ) { CYCT(0x4aac, 0x4aaf); goto fluteSongFellFlat; } // jp z
  CYC(0x4aac, 0x4aaf);
  CYC(0x4aaf, 0x4ab2); A = W8(w1Companion_enabled);
  CYC(0x4ab2, 0x4ab3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4ab3, 0x4ab6); goto deleteSelf; } // jp nz
  CYC(0x4ab3, 0x4ab6);

  // [var3e/var3f] = Link's position
  CYC(0x4ab6, 0x4ab8); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x4ab8, 0x4abb); SET_HL(w1Link_yh);
  CYC(0x4abb, 0x4abc); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4abc, 0x4abe); alu_and(gb, 0xf0);
  CYC(0x4abe, 0x4abf); mem_wr(gb, DE, A);
  CYC(0x4abf, 0x4ac0); L = alu_inc8(gb, L);
  CYC(0x4ac0, 0x4ac1); E = alu_inc8(gb, E);
  CYC(0x4ac1, 0x4ac2); A = mem_rd(gb, HL);
  CYC(0x4ac2, 0x4ac4); A = alu_swap(gb, A);
  CYC(0x4ac4, 0x4ac6); alu_and(gb, 0x0f);
  CYC(0x4ac6, 0x4ac7); mem_wr(gb, DE, A);

  // Try from top at Link's x position
  CYC(0x4ac7, 0x4aca); SET_HL(wRoomCollisions);
  CYC(0x4aca, 0x4acb); companionSpawner_addAToHl_from_rst(gb, 0x4acb);
  CYC(0x4acb, 0x4ace); push_effect(gb, 0x4ace); goto checkVerticalCompanionSpawnPosition; // call
afterCheckVertical1:
  CYC(0x4ace, 0x4ad0); B = 0xf8;
  CYC(0x4ad0, 0x4ad1); L = C;
  CYC(0x4ad1, 0x4ad3); H = 0x10;
  CYC(0x4ad3, 0x4ad5); A = 0x02; // DIR_DOWN
  if (F & FZ) { CYCT(0x4ad5, 0x4ad7); goto setCompanionDestination; } // jr z
  CYC(0x4ad5, 0x4ad7);

  // Try from bottom at Link's x
  CYC(0x4ad7, 0x4ad9); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x4ad9, 0x4ada); A = mem_rd(gb, DE);
  CYC(0x4ada, 0x4add); SET_HL(wRoomCollisions + 0x60);
  CYC(0x4add, 0x4ade); companionSpawner_addAToHl_from_rst(gb, 0x4ade);
  CYC(0x4ade, 0x4ae1); push_effect(gb, 0x4ae1); goto checkVerticalCompanionSpawnPosition; // call
afterCheckVertical2:
  CYC(0x4ae1, 0x4ae3); B = 0x88; // SMALL_ROOM_HEIGHT*$10+8
  CYC(0x4ae3, 0x4ae4); L = C;
  CYC(0x4ae4, 0x4ae6); H = 0x70; // SMALL_ROOM_HEIGHT*$10-$10
  CYC(0x4ae6, 0x4ae8); A = 0x00; // DIR_UP
  if (F & FZ) { CYCT(0x4ae8, 0x4aea); goto setCompanionDestination; } // jr z
  CYC(0x4ae8, 0x4aea);

  // Try from right at Link's y
  CYC(0x4aea, 0x4aec); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x4aec, 0x4aed); A = mem_rd(gb, DE);
  CYC(0x4aed, 0x4af0); SET_HL(wRoomCollisions + 0x08);
  CYC(0x4af0, 0x4af1); companionSpawner_addAToHl_from_rst(gb, 0x4af1);
  CYC(0x4af1, 0x4af4); push_effect(gb, 0x4af4); goto checkHorizontalCompanionSpawnPosition; // call
afterCheckHorizontal1:
  CYC(0x4af4, 0x4af6); C = 0xa8; // SMALL_ROOM_WIDTH*$10+8
  CYC(0x4af6, 0x4af7); H = B;
  CYC(0x4af7, 0x4af9); L = 0x90; // SMALL_ROOM_WIDTH*$10-$10
  CYC(0x4af9, 0x4afb); A = 0x03; // DIR_LEFT
  if (F & FZ) { CYCT(0x4afb, 0x4afd); goto setCompanionDestination; } // jr z
  CYC(0x4afb, 0x4afd);

  // Try from left at Link's y
  CYC(0x4afd, 0x4aff); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x4aff, 0x4b00); A = mem_rd(gb, DE);
  CYC(0x4b00, 0x4b03); SET_HL(wRoomCollisions);
  CYC(0x4b03, 0x4b04); companionSpawner_addAToHl_from_rst(gb, 0x4b04);
  CYC(0x4b04, 0x4b07); push_effect(gb, 0x4b07); goto checkHorizontalCompanionSpawnPosition; // call
afterCheckHorizontal2:
  CYC(0x4b07, 0x4b09); C = 0xf8;
  CYC(0x4b09, 0x4b0a); H = B;
  CYC(0x4b0a, 0x4b0c); L = 0x10;
  CYC(0x4b0c, 0x4b0e); A = 0x01; // DIR_RIGHT
  if (F & FZ) { CYCT(0x4b0e, 0x4b10); goto setCompanionDestination; } // jr z
  CYC(0x4b0e, 0x4b10);

  // Try from top at a range of x positions
  CYC(0x4b10, 0x4b13); SET_HL(wRoomCollisions + 0x03);
  CYC(0x4b13, 0x4b16); push_effect(gb, 0x4b16); goto checkCompanionSpawnColumnRange; // call
afterCheckColumnRange1:
  CYC(0x4b16, 0x4b18); B = 0xf8;
  CYC(0x4b18, 0x4b19); L = C;
  CYC(0x4b19, 0x4b1b); H = 0x10;
  CYC(0x4b1b, 0x4b1d); A = 0x02; // DIR_DOWN
  if (!(F & FZ)) { CYCT(0x4b1d, 0x4b1f); goto setCompanionDestination; } // jr nz
  CYC(0x4b1d, 0x4b1f);

  // Try from bottom at a range of x positions
  CYC(0x4b1f, 0x4b22); SET_HL(wRoomCollisions + 0x63);
  CYC(0x4b22, 0x4b25); push_effect(gb, 0x4b25); goto checkCompanionSpawnColumnRange; // call
afterCheckColumnRange2:
  CYC(0x4b25, 0x4b27); B = 0x88;
  CYC(0x4b27, 0x4b28); L = C;
  CYC(0x4b28, 0x4b2a); H = 0x70;
  CYC(0x4b2a, 0x4b2c); A = 0x00; // DIR_UP
  if (!(F & FZ)) { CYCT(0x4b2c, 0x4b2e); goto setCompanionDestination; } // jr nz
  CYC(0x4b2c, 0x4b2e);

  // Try from right at a range of y positions
  CYC(0x4b2e, 0x4b31); SET_HL(wRoomCollisions + 0x28);
  CYC(0x4b31, 0x4b34); push_effect(gb, 0x4b34); goto checkCompanionSpawnRowRange; // call
afterCheckRowRange1:
  CYC(0x4b34, 0x4b36); C = 0xa8;
  CYC(0x4b36, 0x4b37); H = B;
  CYC(0x4b37, 0x4b39); L = 0x90;
  CYC(0x4b39, 0x4b3b); A = 0x03; // DIR_LEFT
  if (!(F & FZ)) { CYCT(0x4b3b, 0x4b3d); goto setCompanionDestination; } // jr nz
  CYC(0x4b3b, 0x4b3d);

  // Try from left at a range of y positions
  CYC(0x4b3d, 0x4b40); SET_HL(wRoomCollisions + 0x20);
  CYC(0x4b40, 0x4b43); push_effect(gb, 0x4b43); goto checkCompanionSpawnRowRange; // call
afterCheckRowRange2:
  CYC(0x4b43, 0x4b45); C = 0xf8;
  CYC(0x4b45, 0x4b46); H = B;
  CYC(0x4b46, 0x4b48); L = 0x10;
  CYC(0x4b48, 0x4b4a); A = 0x01; // DIR_RIGHT
  if (F & FZ) { CYCT(0x4b4a, 0x4b4c); goto fluteSongFellFlat; } // jr z
  CYC(0x4b4a, 0x4b4c);

// @param a Direction companion should move in
// @param bc Initial Y/X position
// @param hl Y/X destination
setCompanionDestination:
  CYC(0x4b4c, 0x4b4d); push_effect(gb, DE); // push de
  CYC(0x4b4d, 0x4b4e); push_effect(gb, HL); // push hl
  CYC(0x4b4e, 0x4b4f); SET_DE(pop_effect(gb)); // pop de
  CYC(0x4b4f, 0x4b52); SET_HL(wLastAnimalMountPointY);
  CYC(0x4b52, 0x4b53); mem_wr(gb, HL, D);
  CYC(0x4b53, 0x4b54); L = alu_inc8(gb, L);
  CYC(0x4b54, 0x4b55); mem_wr(gb, HL, E);
  CYC(0x4b55, 0x4b56); SET_DE(pop_effect(gb)); // pop de
  CYC(0x4b56, 0x4b59); SET_HL(w1Companion_direction);
  CYC(0x4b59, 0x4b5a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4b5a, 0x4b5c); A = alu_swap(gb, A);
  CYC(0x4b5c, 0x4b5d); alu_rrca(gb);
  CYC(0x4b5d, 0x4b5e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4b5e, 0x4b5f); L = alu_inc8(gb, L);
  CYC(0x4b5f, 0x4b60); mem_wr(gb, HL, B);
  CYC(0x4b60, 0x4b62); L = OBJ_XH;
  CYC(0x4b62, 0x4b63); mem_wr(gb, HL, C);
  CYC(0x4b63, 0x4b65); L = OBJ_ENABLED;
  CYC(0x4b65, 0x4b66); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x4b66, 0x4b67); L = alu_inc8(gb, L);
  CYC(0x4b67, 0x4b6a); A = W8(wAnimalCompanion);
  CYC(0x4b6a, 0x4b6b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [SpecialObject.id]
  // State $0c = entering screen from flute call
  CYC(0x4b6b, 0x4b6d); L = OBJ_STATE;
  CYC(0x4b6d, 0x4b6f); A = 0x0c;
  CYC(0x4b6f, 0x4b70); mem_wr(gb, HL, A);
  CYC(0x4b70, 0x4b72); goto deleteSelf; // jr

fluteSongFellFlat:
  CYC(0x4b72, 0x4b75); SET_BC(0x510c); // TX_510c

showTextAndDelete:
  CYC(0x4b75, 0x4b78); A = W8(wTextIsActive);
  CYC(0x4b78, 0x4b79); alu_or(gb, A);
  if (F & FZ) { CALL_C_CC(0x4b79, showText_hook, 0x1872, 0x4b7c); } else { CYC(0x4b79, 0x4b7c); } // call z

deleteSelf:
  CYC(0x4b7c, 0x4b7f); interactionDelete_hook(gb); return; // jp

// Moosh being attacked by ghosts
subid00:
  CYC(0x4b7f, 0x4b82); SET_HL(wMooshState);
  CYC(0x4b82, 0x4b85); A = W8(wEssencesObtained);
  CYC(0x4b85, 0x4b87); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x4b87, 0x4b89); goto deleteSelf; } // jr z
  CYC(0x4b87, 0x4b89);
  CYC(0x4b89, 0x4b8c); A = mem_rd(gb, wGroup1RoomFlags + 0x79); // wPastRoomFlags+$79
  CYC(0x4b8c, 0x4b8e); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(0x4b8e, 0x4b90); goto deleteSelf; } // jr z
  CYC(0x4b8e, 0x4b90);
  CYC(0x4b90, 0x4b92); A = 0x52; // TREASURE_CHEVAL_ROPE
  CALL_C(0x4b92, checkTreasureObtained_hook, 0x1748, 0x4b95);
  if (!(F & FC)) { CYCT(0x4b95, 0x4b97); goto loadCompanionPresetIfHasntLeft; } // jr nc
  CYC(0x4b95, 0x4b97);
  CYC(0x4b97, 0x4b99); goto deleteSelf; // jr

// Moosh saying goodbye after getting cheval rope
subid01:
  CYC(0x4b99, 0x4b9c); SET_HL(wMooshState);
  CYC(0x4b9c, 0x4b9e); A = 0x40;
  CYC(0x4b9e, 0x4b9f); alu_and(gb, mem_rd(gb, HL)); // and (hl)
  if (!(F & FZ)) { CYCT(0x4b9f, 0x4ba1); goto deleteSelf; } // jr nz
  CYC(0x4b9f, 0x4ba1);
  CYC(0x4ba1, 0x4ba3); A = 0x52; // TREASURE_CHEVAL_ROPE
  CALL_C(0x4ba3, checkTreasureObtained_hook, 0x1748, 0x4ba6);
  if (F & FC) { CYCT(0x4ba6, 0x4ba8); goto loadCompanionPresetIfHasntLeft; } // jr c
  CYC(0x4ba6, 0x4ba8);

deleteSelf2:
  CYC(0x4ba8, 0x4baa); goto deleteSelf; // jr

// Dimitri being attacked by hungry tokays
subid03:
  CYC(0x4baa, 0x4bad); SET_HL(wDimitriState);
  CYC(0x4bad, 0x4bb0); A = W8(wEssencesObtained);
  CYC(0x4bb0, 0x4bb2); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(0x4bb2, 0x4bb4); goto deleteSelf; } // jr z
  CYC(0x4bb2, 0x4bb4);
  CYC(0x4bb4, 0x4bb6); goto loadCompanionPresetIfHasntLeft; // jr

// Ricky looking for gloves
subid02:
  CYC(0x4bb6, 0x4bb8); A = 0x15; // GLOBALFLAG_GAVE_ROPE_TO_RAFTON
  CALL_C(0x4bb8, checkGlobalFlag_hook, 0x31f3, 0x4bbb);
  if (F & FZ) { CYCT(0x4bbb, 0x4bbd); goto deleteSelf; } // jr z
  CYC(0x4bbb, 0x4bbd);
  CYC(0x4bbd, 0x4bc0); SET_HL(wRickyState);
  CYC(0x4bc0, 0x4bc2); goto loadCompanionPresetIfHasntLeft; // jr

// Companion lost in forest
subid04:
  CYC(0x4bc2, 0x4bc4); A = 0x42; // GLOBALFLAG_COMPANION_LOST_IN_FOREST
  CALL_C(0x4bc4, checkGlobalFlag_hook, 0x31f3, 0x4bc7);
  if (F & FZ) { CYCT(0x4bc7, 0x4bc9); goto deleteSelf; } // jr z
  CYC(0x4bc7, 0x4bc9);
  CYC(0x4bc9, 0x4bcb); goto label_0a_052; // jr

// Cutscene outside forest where you get the flute
subid05:
  CYC(0x4bcb, 0x4bcd); A = 0x24; // GLOBALFLAG_SAVED_COMPANION_FROM_FOREST
  CALL_C(0x4bcd, checkGlobalFlag_hook, 0x31f3, 0x4bd0);
  if (F & FZ) { CYCT(0x4bd0, 0x4bd2); goto deleteSelf; } // jr z
  CYC(0x4bd0, 0x4bd2);

label_0a_052:
  CYC(0x4bd2, 0x4bd4); A = 0x23; // GLOBALFLAG_GOT_FLUTE
  CALL_C(0x4bd4, checkGlobalFlag_hook, 0x31f3, 0x4bd7);
  if (!(F & FZ)) { CYCT(0x4bd7, 0x4bd9); goto deleteSelf; } // jr nz
  CYC(0x4bd7, 0x4bd9);
  CYC(0x4bd9, 0x4bdb); goto loadCompanionPreset; // jr

// This bit of the companion's state is set if he's left after his sidequest
loadCompanionPresetIfHasntLeft:
  CYC(0x4bdb, 0x4bdc); A = mem_rd(gb, HL);
  CYC(0x4bdc, 0x4bde); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(0x4bde, 0x4be0); goto deleteSelf2; } // jr nz
  CYC(0x4bde, 0x4be0);

// Load a companion's ID and position from a table of presets based on subid.
// @presetCompanionData at 0x4c4c is pure data, 4 bytes per row: id, yh, xh, unused. It is read
// below only via mem_rd at its literal ROM address, never modeled as its own local -- see the
// transliterate report's own garbage disassembly of those bytes.
loadCompanionPreset:
  CYC(0x4be0, 0x4be2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4be2, 0x4be3); A = mem_rd(gb, DE);
  CYC(0x4be3, 0x4be4); alu_add(gb, A); // add a
  CYC(0x4be4, 0x4be7); SET_HL(0x4c4c); // @presetCompanionData
  CYC(0x4be7, 0x4be8); companionSpawner_add_double_index(gb, 0x4be8);
  CYC(0x4be8, 0x4beb); SET_BC(w1Companion_enabled);
  CYC(0x4beb, 0x4bed); A = 0x01;
  CYC(0x4bed, 0x4bee); mem_wr(gb, BC, A);
  CYC(0x4bee, 0x4bef); C = alu_inc8(gb, C);
  CYC(0x4bef, 0x4bf0); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4bf0, 0x4bf1); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4bf1, 0x4bf3); goto L_4bf6; } // jr nz
  CYC(0x4bf1, 0x4bf3);
  CYC(0x4bf3, 0x4bf6); A = W8(wAnimalCompanion);

L_4bf6:
  CYC(0x4bf6, 0x4bf7); mem_wr(gb, BC, A);
  CYC(0x4bf7, 0x4bf9); C = OBJ_YH;
  CYC(0x4bf9, 0x4bfa); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4bfa, 0x4bfb); mem_wr(gb, BC, A);
  CYC(0x4bfb, 0x4bfe); W8(wLastAnimalMountPointY) = A;
  CYC(0x4bfe, 0x4c00); C = OBJ_XH;
  CYC(0x4c00, 0x4c01); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4c01, 0x4c02); mem_wr(gb, BC, A);
  CYC(0x4c02, 0x4c05); W8(wLastAnimalMountPointX) = A;
  CYC(0x4c05, 0x4c06); alu_xor(gb, A);
  CYC(0x4c06, 0x4c09); W8(wRememberedCompanionId) = A;
  CYC(0x4c09, 0x4c0b); goto deleteSelf2; // jr

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
  CYC(0x4c0b, 0x4c0d); B = 0x10;
  CYC(0x4c0d, 0x4c0f); goto L_4c11; // jr

checkHorizontalCompanionSpawnPosition:
  CYC(0x4c0f, 0x4c11); B = 0x01;

L_4c11:
  CYC(0x4c11, 0x4c12); A = mem_rd(gb, HL);
  CYC(0x4c12, 0x4c13); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(0x4c13);
    if (gb->pc == 0x4ace && gb->sp == sp0_) goto afterCheckVertical1;
    if (gb->pc == 0x4ae1 && gb->sp == sp0_) goto afterCheckVertical2;
    if (gb->pc == 0x4af4 && gb->sp == sp0_) goto afterCheckHorizontal1;
    if (gb->pc == 0x4b07 && gb->sp == sp0_) goto afterCheckHorizontal2;
    return;
  } // ret nz
  CYC(0x4c13, 0x4c14);
  CYC(0x4c14, 0x4c15); A = L;
  CYC(0x4c15, 0x4c16); alu_add(gb, B);
  CYC(0x4c16, 0x4c17); L = A;
  CYC(0x4c17, 0x4c18); A = mem_rd(gb, HL);
  CYC(0x4c18, 0x4c19); alu_or(gb, A);
  CYC(0x4c19, 0x4c1a); A = L;
  if (!(F & FZ)) {
    RET_TAKEN(0x4c1a);
    if (gb->pc == 0x4ace && gb->sp == sp0_) goto afterCheckVertical1;
    if (gb->pc == 0x4ae1 && gb->sp == sp0_) goto afterCheckVertical2;
    if (gb->pc == 0x4af4 && gb->sp == sp0_) goto afterCheckHorizontal1;
    if (gb->pc == 0x4b07 && gb->sp == sp0_) goto afterCheckHorizontal2;
    return;
  } // ret nz
  CYC(0x4c1a, 0x4c1b);
  CALL_C(0x4c1b, convertShortToLongPosition_hook, 0x20cb, 0x4c1e);
  CYC(0x4c1e, 0x4c1f); alu_xor(gb, A);
  RET(0x4c1f);
  if (gb->pc == 0x4ace && gb->sp == sp0_) goto afterCheckVertical1;
  if (gb->pc == 0x4ae1 && gb->sp == sp0_) goto afterCheckVertical2;
  if (gb->pc == 0x4af4 && gb->sp == sp0_) goto afterCheckHorizontal1;
  if (gb->pc == 0x4b07 && gb->sp == sp0_) goto afterCheckHorizontal2;
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
  CYC(0x4c20, 0x4c21); push_effect(gb, DE); // push de
  CYC(0x4c21, 0x4c23); B = 0x01;
  CYC(0x4c23, 0x4c25); E = 0x10;
  CYC(0x4c25, 0x4c27); goto L_4c2c; // jr

checkCompanionSpawnRowRange:
  CYC(0x4c27, 0x4c28); push_effect(gb, DE); // push de
  CYC(0x4c28, 0x4c2a); B = 0x10;
  CYC(0x4c2a, 0x4c2c); E = 0x01;

L_4c2c:
  CYC(0x4c2c, 0x4c2e); C = 0x04;

nextRowOrColumn:
  CYC(0x4c2e, 0x4c2f); A = mem_rd(gb, HL);
  CYC(0x4c2f, 0x4c30); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4c30, 0x4c32); goto tryThisRowOrColumn; } // jr z
  CYC(0x4c30, 0x4c32);

resumeSearch:
  CYC(0x4c32, 0x4c33); A = L;
  CYC(0x4c33, 0x4c34); alu_add(gb, B);
  CYC(0x4c34, 0x4c35); L = A;
  CYC(0x4c35, 0x4c36); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x4c36, 0x4c38); goto nextRowOrColumn; } // jr nz
  CYC(0x4c36, 0x4c38);
  CYC(0x4c38, 0x4c39); SET_DE(pop_effect(gb)); // pop de
  RET(0x4c39);
  if (gb->pc == 0x4b16 && gb->sp == sp0_) goto afterCheckColumnRange1;
  if (gb->pc == 0x4b25 && gb->sp == sp0_) goto afterCheckColumnRange2;
  if (gb->pc == 0x4b34 && gb->sp == sp0_) goto afterCheckRowRange1;
  if (gb->pc == 0x4b43 && gb->sp == sp0_) goto afterCheckRowRange2;
  return; // ret

tryThisRowOrColumn:
  CYC(0x4c3a, 0x4c3b); A = L;
  CYC(0x4c3b, 0x4c3c); alu_add(gb, E);
  CYC(0x4c3c, 0x4c3d); L = A;
  CYC(0x4c3d, 0x4c3e); A = mem_rd(gb, HL);
  CYC(0x4c3e, 0x4c3f); alu_or(gb, A);
  CYC(0x4c3f, 0x4c40); A = L;
  if (F & FZ) { CYCT(0x4c40, 0x4c42); goto foundRowOrColumn; } // jr z
  CYC(0x4c40, 0x4c42);
  CYC(0x4c42, 0x4c43); alu_sub(gb, E);
  CYC(0x4c43, 0x4c44); L = A;
  CYC(0x4c44, 0x4c46); goto resumeSearch; // jr

foundRowOrColumn:
  CALL_C(0x4c46, convertShortToLongPosition_hook, 0x20cb, 0x4c49);
  CYC(0x4c49, 0x4c4a); alu_or(gb, D);
  CYC(0x4c4a, 0x4c4b); SET_DE(pop_effect(gb)); // pop de
  RET(0x4c4b);
  if (gb->pc == 0x4b16 && gb->sp == sp0_) goto afterCheckColumnRange1;
  if (gb->pc == 0x4b25 && gb->sp == sp0_) goto afterCheckColumnRange2;
  if (gb->pc == 0x4b34 && gb->sp == sp0_) goto afterCheckRowRange1;
  if (gb->pc == 0x4b43 && gb->sp == sp0_) goto afterCheckRowRange2;
  return; // ret
}
