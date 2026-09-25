#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/companionSpawner.s.
// INTERAC_COMPANION_SPAWNER

static uint16_t companionSpawner_jump_table(GB *gb) {
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

static void companionSpawner_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCode5f@checkCompanionSpawnRowRange
static void companionSpawner_checkCompanionSpawnRowRange(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+493, b_+494); push_effect(gb, DE);
  CYC(b_+494, b_+496); B = 0x10;
  CYC(b_+496, b_+498); E = 0x01;
  CYC(b_+498, b_+500); C = 0x04;
checkCompanionSpawnRowRange_nextRowOrColumn:
  CYC(b_+500, b_+501); A = mem_rd(gb, HL);
  CYC(b_+501, b_+502); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+502, b_+504); goto checkCompanionSpawnRowRange_tryThisRowOrColumn; }
  CYC(b_+502, b_+504);
checkCompanionSpawnRowRange_resumeSearch:
  CYC(b_+504, b_+505); A = L;
  CYC(b_+505, b_+506); alu_add(gb, B);
  CYC(b_+506, b_+507); L = A;
  CYC(b_+507, b_+508); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+508, b_+510); goto checkCompanionSpawnRowRange_nextRowOrColumn; }
  CYC(b_+508, b_+510);
  CYC(b_+510, b_+511); SET_DE(pop_effect(gb));
  RET(b_+511); return;
checkCompanionSpawnRowRange_tryThisRowOrColumn:
  CYC(b_+512, b_+513); A = L;
  CYC(b_+513, b_+514); alu_add(gb, E);
  CYC(b_+514, b_+515); L = A;
  CYC(b_+515, b_+516); A = mem_rd(gb, HL);
  CYC(b_+516, b_+517); alu_or(gb, A);
  CYC(b_+517, b_+518); A = L;
  if (F & FZ) { CYCT(b_+518, b_+520); goto checkCompanionSpawnRowRange_foundRowOrColumn; }
  CYC(b_+518, b_+520);
  CYC(b_+520, b_+521); alu_sub(gb, E);
  CYC(b_+521, b_+522); L = A;
  CYC(b_+522, b_+524);
  goto checkCompanionSpawnRowRange_resumeSearch;
checkCompanionSpawnRowRange_foundRowOrColumn:
  CALL_C(b_+524, s_convertShortToLongPosition, SYM(convertShortToLongPosition), b_+527);
  CYC(b_+527, b_+528); alu_or(gb, D);
  CYC(b_+528, b_+529); SET_DE(pop_effect(gb));
  RET(b_+529); return;
}

// interactionCode5f@checkCompanionSpawnColumnRange
static void companionSpawner_checkCompanionSpawnColumnRange(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+486, b_+487); push_effect(gb, DE);
  CYC(b_+487, b_+489); B = 0x01;
  CYC(b_+489, b_+491); E = 0x10;
  CYC(b_+491, b_+493);
  goto L_4ef6;

L_4ef6:
  CYC(b_+498, b_+500); C = 0x04;
checkCompanionSpawnRowRange_nextRowOrColumn:
  CYC(b_+500, b_+501); A = mem_rd(gb, HL);
  CYC(b_+501, b_+502); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+502, b_+504); goto checkCompanionSpawnRowRange_tryThisRowOrColumn; }
  CYC(b_+502, b_+504);
checkCompanionSpawnRowRange_resumeSearch:
  CYC(b_+504, b_+505); A = L;
  CYC(b_+505, b_+506); alu_add(gb, B);
  CYC(b_+506, b_+507); L = A;
  CYC(b_+507, b_+508); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+508, b_+510); goto checkCompanionSpawnRowRange_nextRowOrColumn; }
  CYC(b_+508, b_+510);
  CYC(b_+510, b_+511); SET_DE(pop_effect(gb));
  RET(b_+511); return;
checkCompanionSpawnRowRange_tryThisRowOrColumn:
  CYC(b_+512, b_+513); A = L;
  CYC(b_+513, b_+514); alu_add(gb, E);
  CYC(b_+514, b_+515); L = A;
  CYC(b_+515, b_+516); A = mem_rd(gb, HL);
  CYC(b_+516, b_+517); alu_or(gb, A);
  CYC(b_+517, b_+518); A = L;
  if (F & FZ) { CYCT(b_+518, b_+520); goto checkCompanionSpawnRowRange_foundRowOrColumn; }
  CYC(b_+518, b_+520);
  CYC(b_+520, b_+521); alu_sub(gb, E);
  CYC(b_+521, b_+522); L = A;
  CYC(b_+522, b_+524);
  goto checkCompanionSpawnRowRange_resumeSearch;
checkCompanionSpawnRowRange_foundRowOrColumn:
  CALL_C(b_+524, s_convertShortToLongPosition, SYM(convertShortToLongPosition), b_+527);
  CYC(b_+527, b_+528); alu_or(gb, D);
  CYC(b_+528, b_+529); SET_DE(pop_effect(gb));
  RET(b_+529); return;
}

// interactionCode5f@checkHorizontalCompanionSpawnPosition
static void companionSpawner_checkHorizontalCompanionSpawnPosition(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+469, b_+471); B = 0x01;
  CYC(b_+471, b_+472); A = mem_rd(gb, HL);
  CYC(b_+472, b_+473); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+473); return; }
  CYC(b_+473, b_+474);
  CYC(b_+474, b_+475); A = L;
  CYC(b_+475, b_+476); alu_add(gb, B);
  CYC(b_+476, b_+477); L = A;
  CYC(b_+477, b_+478); A = mem_rd(gb, HL);
  CYC(b_+478, b_+479); alu_or(gb, A);
  CYC(b_+479, b_+480); A = L;
  if (!(F & FZ)) { RET_TAKEN(b_+480); return; }
  CYC(b_+480, b_+481);
  CALL_C(b_+481, s_convertShortToLongPosition, SYM(convertShortToLongPosition), b_+484);
  CYC(b_+484, b_+485); alu_xor(gb, A);
  RET(b_+485); return;
}

// interactionCode5f@checkVerticalCompanionSpawnPosition
static void companionSpawner_checkVerticalCompanionSpawnPosition(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+465, b_+467); B = 0x10;
  CYC(b_+467, b_+469);
  goto L_4edb;

L_4edb:
  CYC(b_+471, b_+472); A = mem_rd(gb, HL);
  CYC(b_+472, b_+473); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+473); return; }
  CYC(b_+473, b_+474);
  CYC(b_+474, b_+475); A = L;
  CYC(b_+475, b_+476); alu_add(gb, B);
  CYC(b_+476, b_+477); L = A;
  CYC(b_+477, b_+478); A = mem_rd(gb, HL);
  CYC(b_+478, b_+479); alu_or(gb, A);
  CYC(b_+479, b_+480); A = L;
  if (!(F & FZ)) { RET_TAKEN(b_+480); return; }
  CYC(b_+480, b_+481);
  CALL_C(b_+481, s_convertShortToLongPosition, SYM(convertShortToLongPosition), b_+484);
  CYC(b_+484, b_+485); alu_xor(gb, A);
  RET(b_+485); return;
}

void s_interactionCode5f_hook(GB *gb) {
  BASE(interactionCode5f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+5, b_+7); goto label_0a_045; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_rlca(gb);
  if (F & FC) { CYCT(b_+9, b_+11); goto fluteCall; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Companion);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+18); goto deleteSelf; }
  CYC(b_+15, b_+18);
label_0a_045:
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (companionSpawner_jump_table(gb));
    if (jt_ == b_+428) goto subid00;
    if (jt_ == b_+360) goto subid01;
    if (jt_ == b_+348) goto subid02;
    if (jt_ == b_+404) goto subid03;
    if (jt_ == b_+313) goto subid04;
    if (jt_ == b_+298) goto subid05;
    if (jt_ == b_+384) goto subid06;
    HANDOFF(HL);
  } while (0);
fluteCall:
  CYC(b_+34, b_+37); A = mem_rd(gb, w1Companion);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+38, b_+40); goto label_0a_047; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+43); A = mem_rd(gb, w1Companion_id);
  CYC(b_+43, b_+45); alu_cp(gb, 0x0e);
  if (!(F & FC)) { CYCT(b_+45, b_+47); goto label_0a_047; }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); alu_cp(gb, 0x0a);
  if (!(F & FZ)) { CYCT(b_+49, b_+52); goto deleteSelf; }
  CYC(b_+49, b_+52);
label_0a_047:
  CYC(b_+52, b_+55); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+55, b_+57); alu_and(gb, 0x81);
  CYC(b_+57, b_+59); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+59, b_+62); goto deleteSelf; }
  CYC(b_+59, b_+62);
  CYC(b_+62, b_+65); SET_BC(0x510f);
  CYC(b_+65, b_+68); A = mem_rd(gb, wFluteIcon);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+69, b_+72); goto showTextAndDelete; }
  CYC(b_+69, b_+72);
  CYC(b_+72, b_+75); A = mem_rd(gb, wActiveRoom);
  CYC(b_+75, b_+78); SET_HL(SYM(companionCallableRooms));
  CALL_C(b_+78, s_checkFlag, SYM(checkFlag), b_+81);
  if (F & FZ) { CYCT(b_+81, b_+84); goto fluteSongFellFlat; }
  CYC(b_+81, b_+84);
  CYC(b_+84, b_+87); A = mem_rd(gb, w1Companion);
  CYC(b_+87, b_+88); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+88, b_+91); goto deleteSelf; }
  CYC(b_+88, b_+91);
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+93, b_+96); SET_HL(w1Link_yh);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+99); alu_and(gb, 0xf0);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+101); L = alu_inc8(gb, L);
  CYC(b_+101, b_+102); E = alu_inc8(gb, E);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); A = alu_swap(gb, A);
  CYC(b_+105, b_+107); alu_and(gb, 0x0f);
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+111); SET_HL(wRoomCollisions);
  CYC(b_+111, b_+112); companionSpawner_add_a_to_hl(gb, b_+112);
  CALL_L(b_+112, companionSpawner_checkVerticalCompanionSpawnPosition, b_+115);
  CYC(b_+115, b_+117); B = 0xf8;
  CYC(b_+117, b_+118); L = C;
  CYC(b_+118, b_+120); H = 0x10;
  CYC(b_+120, b_+122); A = 0x02;
  if (F & FZ) { CYCT(b_+122, b_+124); goto setCompanionDestination; }
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+126); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CYC(b_+127, b_+130); SET_HL(wRoomCollisions + 0x60);
  CYC(b_+130, b_+131); companionSpawner_add_a_to_hl(gb, b_+131);
  CALL_L(b_+131, companionSpawner_checkVerticalCompanionSpawnPosition, b_+134);
  CYC(b_+134, b_+136); B = 0x88;
  CYC(b_+136, b_+137); L = C;
  CYC(b_+137, b_+139); H = 0x70;
  CYC(b_+139, b_+141); A = 0x00;
  if (F & FZ) { CYCT(b_+141, b_+143); goto setCompanionDestination; }
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+149); SET_HL(wRoomCollisions + 0x08);
  CYC(b_+149, b_+150); companionSpawner_add_a_to_hl(gb, b_+150);
  CALL_L(b_+150, companionSpawner_checkHorizontalCompanionSpawnPosition, b_+153);
  CYC(b_+153, b_+155); C = 0xa8;
  CYC(b_+155, b_+156); H = B;
  CYC(b_+156, b_+158); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+158, b_+160); A = 0x03;
  if (F & FZ) { CYCT(b_+160, b_+162); goto setCompanionDestination; }
  CYC(b_+160, b_+162);
  CYC(b_+162, b_+164); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+164, b_+165); A = mem_rd(gb, DE);
  CYC(b_+165, b_+168); SET_HL(wRoomCollisions);
  CYC(b_+168, b_+169); companionSpawner_add_a_to_hl(gb, b_+169);
  CALL_L(b_+169, companionSpawner_checkHorizontalCompanionSpawnPosition, b_+172);
  CYC(b_+172, b_+174); C = 0xf8;
  CYC(b_+174, b_+175); H = B;
  CYC(b_+175, b_+177); L = 0x10;
  CYC(b_+177, b_+179); A = 0x01;
  if (F & FZ) { CYCT(b_+179, b_+181); goto setCompanionDestination; }
  CYC(b_+179, b_+181);
  CYC(b_+181, b_+184); SET_HL(wRoomCollisions + 0x03);
  CALL_L(b_+184, companionSpawner_checkCompanionSpawnColumnRange, b_+187);
  CYC(b_+187, b_+189); B = 0xf8;
  CYC(b_+189, b_+190); L = C;
  CYC(b_+190, b_+192); H = 0x10;
  CYC(b_+192, b_+194); A = 0x02;
  if (!(F & FZ)) { CYCT(b_+194, b_+196); goto setCompanionDestination; }
  CYC(b_+194, b_+196);
  CYC(b_+196, b_+199); SET_HL(wRoomCollisions + 0x63);
  CALL_L(b_+199, companionSpawner_checkCompanionSpawnColumnRange, b_+202);
  CYC(b_+202, b_+204); B = 0x88;
  CYC(b_+204, b_+205); L = C;
  CYC(b_+205, b_+207); H = 0x70;
  CYC(b_+207, b_+209); A = 0x00;
  if (!(F & FZ)) { CYCT(b_+209, b_+211); goto setCompanionDestination; }
  CYC(b_+209, b_+211);
  CYC(b_+211, b_+214); SET_HL(wRoomCollisions + 0x28);
  CALL_L(b_+214, companionSpawner_checkCompanionSpawnRowRange, b_+217);
  CYC(b_+217, b_+219); C = 0xa8;
  CYC(b_+219, b_+220); H = B;
  CYC(b_+220, b_+222); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+222, b_+224); A = 0x03;
  if (!(F & FZ)) { CYCT(b_+224, b_+226); goto setCompanionDestination; }
  CYC(b_+224, b_+226);
  CYC(b_+226, b_+229); SET_HL(wRoomCollisions + 0x20);
  CALL_L(b_+229, companionSpawner_checkCompanionSpawnRowRange, b_+232);
  CYC(b_+232, b_+234); C = 0xf8;
  CYC(b_+234, b_+235); H = B;
  CYC(b_+235, b_+237); L = 0x10;
  CYC(b_+237, b_+239); A = 0x01;
  if (F & FZ) { CYCT(b_+239, b_+241); goto fluteSongFellFlat; }
  CYC(b_+239, b_+241);
setCompanionDestination:
  CYC(b_+241, b_+242); push_effect(gb, DE);
  CYC(b_+242, b_+243); push_effect(gb, HL);
  CYC(b_+243, b_+244); SET_DE(pop_effect(gb));
  CYC(b_+244, b_+247); SET_HL(wLastAnimalMountPointY);
  CYC(b_+247, b_+249); mem_wr(gb, hFF8B, A);
  CYC(b_+249, b_+250); A = D;
  CYC(b_+250, b_+251); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+251, b_+252); A = E;
  CYC(b_+252, b_+253); mem_wr(gb, HL, A);
  CYC(b_+253, b_+255); A = mem_rd(gb, hFF8B);
  CYC(b_+255, b_+256); SET_DE(pop_effect(gb));
  CYC(b_+256, b_+259); SET_HL(w1Companion_direction);
  CYC(b_+259, b_+260); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+260, b_+262); A = alu_swap(gb, A);
  CYC(b_+262, b_+264); A = alu_srl(gb, A);
  CYC(b_+264, b_+265); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+265, b_+266); L = alu_inc8(gb, L);
  CYC(b_+266, b_+267); mem_wr(gb, HL, B);
  CYC(b_+267, b_+269); L = 0x0d;
  CYC(b_+269, b_+270); mem_wr(gb, HL, C);
  CYC(b_+270, b_+272); L = 0x00;
  CYC(b_+272, b_+273); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+273, b_+274); L = alu_inc8(gb, L);
  CYC(b_+274, b_+277); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+277, b_+278); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+278, b_+280); L = 0x04;
  CYC(b_+280, b_+282); A = 0x0c;
  CYC(b_+282, b_+283); mem_wr(gb, HL, A);
  CYC(b_+283, b_+285);
  goto deleteSelf;
fluteSongFellFlat:
  CYC(b_+285, b_+288); SET_BC(0x510c);
showTextAndDelete:
  CYC(b_+288, b_+291); A = mem_rd(gb, wTextIsActive);
  CYC(b_+291, b_+292); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+292, s_showText, SYM(showText), b_+295);
  else CYC(b_+292, b_+295);
deleteSelf:
  CYC(b_+295, b_+298);
  TAIL(interactionDelete);
subid05:
  CYC(b_+298, b_+301); SET_HL(wMooshState);
  CYC(b_+301, b_+304); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+304, b_+306); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+306, b_+309); goto loadCompanionPresetIfHasntLeft; }
  CYC(b_+306, b_+309);
  CYC(b_+309, b_+311); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+311, b_+313);
  goto deleteSelf;
subid04:
  CYC(b_+313, b_+316); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+316, b_+318); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(b_+318, b_+320); goto deleteSelf; }
  CYC(b_+318, b_+320);
  CYC(b_+320, b_+323); A = mem_rd(gb, wDimitriState);
  CYC(b_+323, b_+325); alu_and(gb, 0x20);
  if (F & FZ) { CYCT(b_+325, b_+327); goto deleteSelf; }
  CYC(b_+325, b_+327);
  CYC(b_+327, b_+330); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+330, b_+332); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+332, b_+334); goto deleteSelf; }
  CYC(b_+332, b_+334);
  CYC(b_+334, b_+337); SET_HL(wDimitriState);
  CYC(b_+337, b_+339); A = 0x2e;
  CALL_C(b_+339, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+342);
  if (!(F & FC)) { CYCT(b_+342, b_+344); goto loadCompanionPresetIfHasntLeft; }
  CYC(b_+342, b_+344);
  CYC(b_+344, b_+346); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+346, b_+348);
  goto deleteSelf;
subid02:
  CYC(b_+348, b_+351); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+351, b_+353); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+353, b_+355); goto deleteSelf; }
  CYC(b_+353, b_+355);
  CYC(b_+355, b_+358); SET_HL(wDimitriState);
  CYC(b_+358, b_+360);
  goto deleteSelfIfBit7OfAnimalStateSet;
subid01:
  CYC(b_+360, b_+363); SET_HL(wRickyState);
  CYC(b_+363, b_+366); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+366, b_+368); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+368, b_+370); goto deleteSelf; }
  CYC(b_+368, b_+370);
  CYC(b_+370, b_+373); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+373, b_+375); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(b_+375, b_+377); goto deleteSelfIfBit7OfAnimalStateSet; }
  CYC(b_+375, b_+377);
  CYC(b_+377, b_+378); A = mem_rd(gb, HL);
  CYC(b_+378, b_+380); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+380, b_+382); goto loadCompanionPresetIfHasntLeft; }
  CYC(b_+380, b_+382);
  CYC(b_+382, b_+384);
  goto deleteSelf;
subid06:
  CYC(b_+384, b_+387); SET_HL(wRickyState);
  CYC(b_+387, b_+390); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+390, b_+392); alu_cp(gb, 0x0b);
  if (F & FZ) { CYCT(b_+392, b_+394); goto deleteSelf2; }
  CYC(b_+392, b_+394);
  CYC(b_+394, b_+397); A = mem_rd(gb, wFluteIcon);
  CYC(b_+397, b_+398); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+398, b_+400); goto deleteSelf; }
  CYC(b_+398, b_+400);
  CYC(b_+400, b_+402); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
deleteSelf2:
  CYC(b_+402, b_+404);
  goto deleteSelf;
subid03:
  CYC(b_+404, b_+407); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+407, b_+409); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+409, b_+411); goto deleteSelf; }
  CYC(b_+409, b_+411);
  CYC(b_+411, b_+414); SET_HL(wMooshState);
  CYC(b_+414, b_+415); A = mem_rd(gb, HL);
  CYC(b_+415, b_+417); alu_and(gb, 0xa0);
  if (!(F & FZ)) { CYCT(b_+417, b_+419); goto deleteSelf; }
  CYC(b_+417, b_+419);
deleteSelfIfBit7OfAnimalStateSet:
  CYC(b_+419, b_+421); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+421, b_+423); goto deleteSelf2; }
  CYC(b_+421, b_+423);
loadCompanionPresetIfHasntLeft:
  CYC(b_+423, b_+424); A = mem_rd(gb, HL);
  CYC(b_+424, b_+426); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+426, b_+428); goto deleteSelf2; }
  CYC(b_+426, b_+428);
subid00:
  CYC(b_+428, b_+430); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+430, b_+431); A = mem_rd(gb, DE);
  CYC(b_+431, b_+432); alu_add(gb, A);
  CYC(b_+432, b_+435); SET_HL(b_+530 /* @presetCompanionData */);
  CYC(b_+435, b_+436); companionSpawner_add_double_index(gb, b_+436);
  CYC(b_+436, b_+439); SET_BC(w1Companion);
  CYC(b_+439, b_+441); A = 0x01;
  CYC(b_+441, b_+442); mem_wr(gb, BC, A);
  CYC(b_+442, b_+443); C = alu_inc8(gb, C);
  CYC(b_+443, b_+444); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+444, b_+445); mem_wr(gb, BC, A);
  CYC(b_+445, b_+447); C = 0x0b;
  CYC(b_+447, b_+448); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+448, b_+449); mem_wr(gb, BC, A);
  CYC(b_+449, b_+452); mem_wr(gb, wLastAnimalMountPointY, A);
  CYC(b_+452, b_+454); C = 0x0d;
  CYC(b_+454, b_+455); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+455, b_+456); mem_wr(gb, BC, A);
  CYC(b_+456, b_+459); mem_wr(gb, wLastAnimalMountPointX, A);
  CYC(b_+459, b_+460); alu_xor(gb, A);
  CYC(b_+460, b_+463); mem_wr(gb, wRememberedCompanionId, A);
  CYC(b_+463, b_+465);
  goto deleteSelf2;
}

